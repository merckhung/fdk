/*
 * FDK - Firmware Debug Kit
 * File: client.c
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

#include <string.h>

#include "memvr.h"
#include "packet.h"

static u8 pktBuf[FDK_MAXSZ_PKT];

static bool responseOk(void) {
  return ((fdkCommPkt_t *)pktBuf)->fdkCommHdr.errorCode == FDK_SUCCESS;
}

s32 connectToFdkServer(s32 sfd) {
  return executeFunction(sfd, FDK_REQ_CONNECT, 0, 0, NULL, pktBuf,
                         sizeof(pktBuf));
}

void disconnectFromFdkServer(s32 sfd) {
  executeFunction(sfd, FDK_REQ_DISCONNECT, 0, 0, NULL, pktBuf, sizeof(pktBuf));
}

s32 memoryReadBuffer(s32 sfd, u64 address, u32 len, u8 *buf) {
  const fdkCommPkt_t *pPkt = (fdkCommPkt_t *)pktBuf;

  if (len > FDK_PKT_MAX_PAYLOAD(fdkRspMemReadPkt_t, memContent)) return -1;
  if (executeFunction(sfd, FDK_REQ_MEM_READ, address, len, NULL, pktBuf,
                      sizeof(pktBuf)) ||
      pPkt->fdkRspMemReadPkt.size != len || !responseOk()) {
    return -1;
  }
  memcpy(buf, &pPkt->fdkRspMemReadPkt.memContent, len);
  return 0;
}

s32 memoryReadDWord(s32 sfd, u64 address, u32 *value) {
  return memoryReadBuffer(sfd, address, sizeof(*value), (u8 *)value);
}

s32 memoryWriteDWord(s32 sfd, u64 address, u32 value) {
  if (executeFunction(sfd, FDK_REQ_MEM_WRITE, address, sizeof(value),
                      (const u8 *)&value, pktBuf, sizeof(pktBuf)) ||
      !responseOk()) {
    return -1;
  }
  return 0;
}

s32 memoryORDWord(s32 sfd, u64 address, u32 value) {
  u32 tmp;

  if (memoryReadDWord(sfd, address, &tmp)) return -1;
  return memoryWriteDWord(sfd, address, tmp | value);
}

s32 memoryANDDWord(s32 sfd, u64 address, u32 value) {
  u32 tmp;

  if (memoryReadDWord(sfd, address, &tmp)) return -1;
  return memoryWriteDWord(sfd, address, tmp & value);
}
