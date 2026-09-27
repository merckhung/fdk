/*
 * FDK - Firmware Debug Kit
 * File: netsock.c
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

#include "netsock.h"

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "fdk.h"

#define FDK_LISTEN_BACKLOG 5

typedef s32 (*socketAction_t)(s32 fd, const struct sockaddr *sa, socklen_t len);

static s32 listenAction(s32 fd, const struct sockaddr *sa, socklen_t len) {
  const s32 on = 1;

  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  if (bind(fd, sa, len) < 0) return -1;
  return listen(fd, FDK_LISTEN_BACKLOG);
}

static s32 connectAction(s32 fd, const struct sockaddr *sa, socklen_t len) {
  const s32 on = 1;

  // Requests are small and strictly request/response.
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
  return connect(fd, sa, len);
}

// Resolves |addr|:|port| and runs |action| on a socket for each candidate
// address until one succeeds.
static s32 openSocket(s32 *fd, const s8 *addr, s32 port, bool passive,
                      socketAction_t action) {
  struct addrinfo hints, *res, *ai;
  s8 service[16];

  if (port <= 0) port = FDK_DEF_PORT;
  snprintf(service, sizeof(service), "%d", port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = passive ? AI_PASSIVE : 0;
  if (getaddrinfo(addr, service, &hints, &res)) return -1;

  for (ai = res; ai; ai = ai->ai_next) {
    *fd =
        socket(ai->ai_family, ai->ai_socktype | SOCK_CLOEXEC, ai->ai_protocol);
    if (*fd < 0) continue;
    if (!action(*fd, ai->ai_addr, ai->ai_addrlen)) break;
    close(*fd);
  }
  freeaddrinfo(res);

  return ai ? 0 : -1;
}

s32 initializeSocket(s32 *fd, const s8 *addr, s32 port) {
  return openSocket(fd, addr, port, true, listenAction);
}

s32 connectSocket(s32 *fd, const s8 *addr, s32 port) {
  return openSocket(fd, addr, port, false, connectAction);
}

void deinitializeSocket(s32 fd) {
  if (fd >= 0) close(fd);
}

s32 acceptSocket(s32 fd, s32 *apsd) {
  *apsd = accept4(fd, NULL, NULL, SOCK_CLOEXEC);
  return *apsd < 0 ? FALSE : TRUE;
}
