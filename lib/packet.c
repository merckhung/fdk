/*
 * FDK - Firmware Debug Kit
 * File: packet.c
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

#include "packet.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Response opcodes are always the request opcode plus one.
s32 verifyResponsePacket(const fdkCommPkt_t *pFdkCommPkt, fdkOpCode_t op) {
  const u16 rsp = pFdkCommPkt->fdkCommHdr.opCode;

  if (rsp == FDK_RSP_NACK || rsp == FDK_RSP_CPU_EXCEPTION) return 1;
  if (op < FDK_REQ_CONNECT || op > FDK_REQ_E820_LIST || !(op & 1)) return 1;
  return rsp == op + 1 ? 0 : 1;
}

// Transfers exactly |len| bytes, retrying on short transfers and EINTR.
// Returns the number of bytes moved (less than |len| only on EOF/error).
static s32 transferAll(s32 fd, void *buf, u32 len, bool isWrite) {
  u8 *p = buf;
  u32 done = 0;

  while (done < len) {
    ssize_t n = isWrite ? write(fd, p + done, len - done)
                        : read(fd, p + done, len - done);
    if (n < 0 && errno == EINTR) continue;
    if (n <= 0) break;
    done += (u32)n;
  }
  return (s32)done;
}

s32 readPacket(s32 fd, void *pktBuf, u32 lenPktBuf) {
  fdkCommHdr_t *hdr = pktBuf;
  s32 n;

  if (lenPktBuf < sizeof(*hdr)) return -1;

  n = transferAll(fd, hdr, sizeof(*hdr), false);
  if (!n) return 0;
  if (n != (s32)sizeof(*hdr)) return -1;
  if (hdr->pktLen < sizeof(*hdr) || hdr->pktLen > lenPktBuf) return -1;

  n = transferAll(fd, (u8 *)pktBuf + sizeof(*hdr), hdr->pktLen - sizeof(*hdr),
                  false);
  if (n != (s32)(hdr->pktLen - sizeof(*hdr))) return -1;

  return (s32)hdr->pktLen;
}

s32 writePacket(s32 fd, const void *pktBuf) {
  const fdkCommHdr_t *hdr = pktBuf;
  return transferAll(fd, (void *)pktBuf, hdr->pktLen, true) == (s32)hdr->pktLen
             ? 0
             : -1;
}

// Copies the |size|-byte payload of a write request to |offset| and
// returns the resulting packet length, or 0 when it is empty or too large.
static u32 fillPayload(u8 *pktBuf, size_t offset, const u8 *payload, u32 size) {
  if (!payload || !size || size > FDK_MAXSZ_PKT - offset) return 0;
  memcpy(pktBuf + offset, payload, size);
  return (u32)(offset + size);
}

s32 executeFunction(s32 fd, fdkOpCode_t op, u64 addr, u32 size,
                    const u8 *cntBuf, u8 *pktBuf, u32 lenPktBuf) {
  fdkCommPkt_t *pPkt = (fdkCommPkt_t *)pktBuf;
  u32 len = 0;

  if (lenPktBuf < FDK_MAXSZ_PKT) return 1;
  memset(pktBuf, 0, lenPktBuf);

  switch (op) {
    case FDK_REQ_CONNECT:
    case FDK_REQ_DISCONNECT:
      len = sizeof(fdkCommHdr_t);
      break;

    case FDK_REQ_MEM_READ:
      pPkt->fdkReqMemReadPkt.address = addr;
      pPkt->fdkReqMemReadPkt.size = size;
      len = sizeof(fdkReqMemReadPkt_t);
      break;

    case FDK_REQ_MEM_WRITE:
      pPkt->fdkReqMemWritePkt.address = addr;
      pPkt->fdkReqMemWritePkt.size = size;
      len = fillPayload(pktBuf, offsetof(fdkReqMemWritePkt_t, memContent),
                        cntBuf, size);
      break;

    case FDK_REQ_IO_READ:
      pPkt->fdkReqIoReadPkt.address = (u16)addr;
      pPkt->fdkReqIoReadPkt.size = size;
      len = sizeof(fdkReqIoReadPkt_t);
      break;

    case FDK_REQ_IO_WRITE:
      pPkt->fdkReqIoWritePkt.address = (u16)addr;
      pPkt->fdkReqIoWritePkt.size = size;
      len = fillPayload(pktBuf, offsetof(fdkReqIoWritePkt_t, ioContent), cntBuf,
                        size);
      break;

    case FDK_REQ_PCI_READ:
      pPkt->fdkReqPciReadPkt.address = (u32)addr;
      pPkt->fdkReqPciReadPkt.size = (u16)size;
      len = sizeof(fdkReqPciReadPkt_t);
      break;

    case FDK_REQ_PCI_WRITE:
      pPkt->fdkReqPciWritePkt.address = (u32)addr;
      pPkt->fdkReqPciWritePkt.size = (u16)size;
      len = fillPayload(pktBuf, offsetof(fdkReqPciWritePkt_t, pciContent),
                        cntBuf, size);
      break;

    case FDK_REQ_IDE_READ:
      pPkt->fdkReqIdeReadPkt.address = addr;
      pPkt->fdkReqIdeReadPkt.size = size;
      len = sizeof(fdkReqIdeReadPkt_t);
      break;

    case FDK_REQ_IDE_WRITE:
      pPkt->fdkReqIdeWritePkt.address = addr;
      pPkt->fdkReqIdeWritePkt.size = size;
      len = fillPayload(pktBuf, offsetof(fdkReqIdeWritePkt_t, ideContent),
                        cntBuf, size);
      break;

    case FDK_REQ_CMOS_READ:
      // The size field is 8 bits wide; 256 is sent as 0.
      pPkt->fdkReqCmosReadPkt.address = (u8)addr;
      pPkt->fdkReqCmosReadPkt.size = (u8)size;
      len = sizeof(fdkReqCmosReadPkt_t);
      break;

    case FDK_REQ_CMOS_WRITE:
      pPkt->fdkReqCmosWritePkt.address = (u8)addr;
      pPkt->fdkReqCmosWritePkt.size = (u8)size;
      len = fillPayload(pktBuf, offsetof(fdkReqCmosWritePkt_t, cmosContent),
                        cntBuf, size);
      break;

    case FDK_REQ_PCI_LIST:
      len = sizeof(fdkReqPciListPkt_t);
      break;

    case FDK_REQ_E820_LIST:
      len = sizeof(fdkReqE820ListPkt_t);
      break;

    default:
      fprintf(stderr, "Unsupported operation, %d\n", op);
      return 1;
  }

  if (!len) return 1;
  pPkt->fdkCommHdr.opCode = op;
  pPkt->fdkCommHdr.pktLen = len;

  if (writePacket(fd, pktBuf)) return 1;
  if (readPacket(fd, pktBuf, lenPktBuf) <= 0) return 1;

  return verifyResponsePacket(pPkt, op);
}
