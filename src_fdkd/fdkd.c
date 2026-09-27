/*
 * FDK - Firmware Debug Kit
 * File: fdkd.c
 *
 * Copyright (C) 2006 - 2026 Merck Hung <merckhung@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "fdkd.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "libdisk.h"
#include "libmem.h"
#include "libpci.h"
#include "netsock.h"

static volatile sig_atomic_t terminate = 0;
static s8 diskPath[FDK_MAX_PATH];

static void help(void) {
  fprintf(stderr, "\n" FDK_COPYRIGHT_TEXT "\n\n");
  fprintf(stderr, FDKD_PROGRAM_NAME ", Version " FDK_REVISION "\n");
  fprintf(stderr, "Author: " FDK_AUTHOR_NAME "\n");
  fprintf(stderr,
          "usage: fdkd [-d] [-l address] [-p port] [-b blockdev] [-h]\n\n");
  fprintf(stderr, "\t-d\tStay in the foreground (don't run as a daemon)\n");
  fprintf(stderr, "\t-l\tAddress to listen on, default is " FDK_DEF_LISTEN_ADDR
                  " (use 0.0.0.0 or :: for remote clients)\n");
  fprintf(stderr, "\t-p\tTCP port, default is %d\n", FDK_DEF_PORT);
  fprintf(stderr,
          "\t-b\tBlock device for disk access, default is the first disk "
          "in /sys/block\n");
  fprintf(stderr, "\t-h\tPrint help and exit\n\n");
}

// Reports which hardware access paths the running kernel allows.
static void probeEnvironment(void) {
  s8 buf[128] = "";
  FILE *fp;
  s32 fd;

  fd = openMemDev();
  if (fd < 0) {
    fprintf(stderr,
            "Warning: " FDK_MEM_DEV
            ": %s; memory access is disabled (CONFIG_DEVMEM, lockdown?)\n",
            strerror(errno));
  }
  closeMemDev(fd);

  if (access(PCI_SYSFS_DEVICES, R_OK)) {
    fprintf(stderr, "Warning: " PCI_SYSFS_DEVICES
                    " missing; using legacy CF8h/CFCh PCI access\n");
  }

  // Lockdown also blocks /dev/port, ioperm() and iopl().
  fp = fopen("/sys/kernel/security/lockdown", "re");
  if (fp) {
    if (fgets(buf, sizeof(buf), fp) && !strstr(buf, "[none]")) {
      fprintf(stderr, "Warning: kernel lockdown is active: %s", buf);
    }
    fclose(fp);
  }

  if (diskPath[0]) fprintf(stderr, "Disk requests use %s\n", diskPath);
}

static void *handleIncomingConnection(void *arg) {
  fdkdConnection_t *pConn = arg;
  s32 rByte;

  // /dev/mem may be missing or locked down; keep serving other requests.
  pConn->memfd = openMemDev();

  for (;;) {
    rByte = readPacket(pConn->cfd, pConn->packet, sizeof(pConn->packet));
    if (rByte <= 0) break;

    if (handleRequestPacket(pConn, (u32)rByte) == FDKD_REPLY_AND_CLOSE) {
      writePacket(pConn->cfd, pConn->packet);
      break;
    }
    if (writePacket(pConn->cfd, pConn->packet)) break;
  }

  closeMemDev(pConn->memfd);
  deinitializeSocket(pConn->cfd);
  free(pConn);
  return NULL;
}

static void handleSignal(int no) {
  (void)no;
  terminate = 1;
}

static void installSignalHandlers(void) {
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handleSignal;
  sigemptyset(&sa.sa_mask);
  // No SA_RESTART: accept() must return EINTR so the main loop can exit.
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGHUP, &sa, NULL);

  // A client that disconnects mid-reply must not kill the server.
  signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char **argv) {
  const s8 *listenAddr = FDK_DEF_LISTEN_ADDR;
  bool runAsDaemon = true;
  s32 port = FDK_DEF_PORT, sfd, cfd, c;
  pthread_attr_t attr;
  pthread_t pth;

  while ((c = getopt(argc, argv, "dl:p:b:h")) != -1) {
    switch (c) {
      case 'd':
        runAsDaemon = false;
        break;
      case 'l':
        listenAddr = optarg;
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'b':
        snprintf(diskPath, sizeof(diskPath), "%s", optarg);
        break;
      case 'h':
        help();
        return 0;
      default:
        help();
        return 1;
    }
  }

  if (geteuid() != 0) {
    fprintf(stderr, "Must be run with root privilege\n");
    return 1;
  }

  if (!diskPath[0] && diskFindDefault(diskPath, sizeof(diskPath))) {
    fprintf(stderr, "Warning: no disk found, disk access is disabled\n");
  }

  probeEnvironment();

  if (initializeSocket(&sfd, listenAddr, port)) {
    fprintf(stderr, "Cannot listen on %s port %d: %s\n", listenAddr, port,
            strerror(errno));
    return 1;
  }

  if (runAsDaemon && daemon(0, 0)) {
    fprintf(stderr, "Cannot run as daemon: %s\n", strerror(errno));
    return 1;
  }

  installSignalHandlers();
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  while (!terminate) {
    fdkdConnection_t *pConn;

    if (acceptSocket(sfd, &cfd) != TRUE) continue;

    pConn = calloc(1, sizeof(*pConn));
    if (!pConn) {
      deinitializeSocket(cfd);
      continue;
    }
    pConn->cfd = cfd;
    pConn->memfd = -1;
    pConn->diskPath = diskPath[0] ? diskPath : NULL;

    if (pthread_create(&pth, &attr, handleIncomingConnection, pConn)) {
      fprintf(stderr, "Failed to create a thread\n");
      deinitializeSocket(cfd);
      free(pConn);
    }
  }

  // Worker threads are torn down with the process.
  pthread_attr_destroy(&attr);
  deinitializeSocket(sfd);
  return 0;
}
