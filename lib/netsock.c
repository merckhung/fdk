/*
 * FDK - Firmware Debug Kit
 * File: netsock.c
 *
 * Copyright (C) 2006 - 2013 Merck Hung <merckhung@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <mtypes.h>
#include <fdk.h>
#include <netsock.h>
#include <libcomm.h>

s32 initializeSocket(s32 *fd, s8 *addr, s32 port) {

  s32 sts = 0;
  struct sockaddr_in servaddr;

  // Argument check
  if (!addr)
    addr = "0.0.0.0";  // Any
  else if (isIPv4Format(addr))
    return -1;

  // Open a socket
  *fd = socket( PF_INET, SOCK_STREAM, 0);
  if (*fd < 0)
    return -1;

  // Default values of address & port
  if (port <= 0)
    port = FDK_DEF_PORT;

  // Set address
  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = PF_INET;
  servaddr.sin_port = htons(port);
  if (inet_aton(addr, &servaddr.sin_addr) < 0) {

    sts = -1;
    goto ErrExit;
  }

  // Bind network port
  if (bind(*fd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in))
      < 0) {

    sts = -1;
    goto ErrExit;
  }

  // Listen network port
  if (listen(*fd, 5) < 0) {

    sts = -1;
    goto ErrExit;
  }

  // Return socket fd
  return sts;

  ErrExit: close(*fd);
  return sts;
}

s32 connectSocket(s32 *fd, s8 *addr, s32 port) {

  s32 sts = 0;
  struct sockaddr_in servaddr;

  // Argument check
  if (!addr)
    addr = "0.0.0.0";  // Any
  else if (isIPv4Format(addr))
    return -1;

  // Open a socket
  *fd = socket( PF_INET, SOCK_STREAM, 0);
  if (*fd < 0)
    return -1;

  // Default values of address & port
  if (port <= 0)
    port = FDK_DEF_PORT;

  // Set address
  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = PF_INET;
  servaddr.sin_port = htons(port);
  if (inet_aton(addr, &servaddr.sin_addr) < 0) {

    sts = -1;
    goto ErrExit;
  }

  // Connect to network port
  if (connect(*fd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in))
      < 0) {

    sts = -1;
    goto ErrExit;
  }

  // Return socket fd
  return sts;

  ErrExit: close(*fd);
  return sts;
}

void deinitializeSocket(s32 fd) {

  close(fd);
}

s32 acceptSocket(s32 fd, s32 *apsd) {

  struct sockaddr_in cliaddr;
  socklen_t clilen;

  // Accept new connection
  clilen = sizeof(struct sockaddr_in);
  *apsd = accept(fd, (struct sockaddr *) &cliaddr, &clilen);
  if (*apsd < 0)
    return FALSE;

  return TRUE;
}

s32 transferSocket(s32 fd, const void *pktBuf, const u32 length, u32 *wByte) {

  *wByte = send(fd, pktBuf, length, 0);
  if (*wByte != length)
    return FALSE;

  return TRUE;
}

s32 receiveSocket(s32 fd, void *pktBuf, const u32 length, u32 *rByte) {

  *rByte = recv(fd, pktBuf, length, 0);
  if (*rByte != length)
    return FALSE;

  return TRUE;
}

