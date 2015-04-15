/*
 * FDK - Firmware Debug Kit
 * File: utils.h
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>

#include <mtypes.h>
#include <packet.h>
#include <fdk.h>

static u8 pktBuf[FDK_MAXSZ_PKT];

s32 connectToFdkServer(s32 sfd) {

  // Connect to FDK server
  return executeFunction(sfd, FDK_REQ_CONNECT, 0, 0,
  NULL, pktBuf,
  FDK_MAXSZ_PKT);
}

void disconnectFromFdkServer(s32 sfd) {

  // Disconnect to FDK server
  executeFunction(sfd, FDK_REQ_DISCONNECT, 0, 0,
  NULL, pktBuf,
  FDK_MAXSZ_PKT);
}

u32 memoryReadDWord(s32 sfd, u64 address) {

  s32 ret;
  fdkCommPkt_t *pFdkCommPkt = (fdkCommPkt_t *) pktBuf;
  s8 *ptr = (s8 *) &pFdkCommPkt->fdkRspMemReadPkt.memContent;

  // Read memory
  ret = executeFunction(sfd, FDK_REQ_MEM_READ, address, sizeof(u32),
  NULL, pktBuf,
  FDK_MAXSZ_PKT);

  if (ret)
    return 0xFFFFFFFF;

  return *((u32 *) ptr);
}

s32 memoryWriteDWord(s32 sfd, u64 address, u32 value) {

  s32 ret;

  // Read memory
  ret = executeFunction(sfd, FDK_REQ_MEM_WRITE, address, sizeof(u32),
      (u8 *) &value, pktBuf,
      FDK_MAXSZ_PKT);

  if (ret)
    return -1;

  return 0;
}

u8 memoryReadByte(s32 sfd, u64 address) {

  s32 ret;
  fdkCommPkt_t *pFdkCommPkt = (fdkCommPkt_t *) pktBuf;
  s8 *ptr = (s8 *) &pFdkCommPkt->fdkRspMemReadPkt.memContent;

  // Read memory
  ret = executeFunction(sfd, FDK_REQ_MEM_READ, address, sizeof(u8),
  NULL, pktBuf,
  FDK_MAXSZ_PKT);

  if (ret)
    return 0xFF;

  return (u8) *ptr;
}

s32 memoryWriteByte(s32 sfd, u64 address, u8 value) {

  s32 ret;

  // Read memory
  ret = executeFunction(sfd, FDK_REQ_MEM_WRITE, address, sizeof(u8), &value,
      pktBuf,
      FDK_MAXSZ_PKT);

  if (ret)
    return -1;

  return 0;
}

void memoryORDWord(s32 sfd, u32 address, u32 value) {

  u32 tmp;

  tmp = memoryReadDWord(sfd, address);
  tmp |= value;
  memoryWriteDWord(sfd, address, tmp);
}

void memoryANDDWord(s32 sfd, u32 address, u32 value) {

  u32 tmp;

  tmp = memoryReadDWord(sfd, address);
  tmp &= value;
  memoryWriteDWord(sfd, address, tmp);
}

void memoryCOMANDDWord(s32 sfd, u32 address, u32 value) {

  u32 tmp;

  value = ~value;
  tmp = memoryReadDWord(sfd, address);
  tmp &= value;
  memoryWriteDWord(sfd, address, tmp);
}

