/*
 * FDK - Firmware Debug Kit
 * File: handler.c
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

#include "fdkd.h"
#include "libdisk.h"
#include "libe820.h"
#include "libmem.h"
#include "libpci.h"
#include "libport.h"

// A CMOS request carries an 8-bit size; 0 stands for the full 256 bytes.
#define CMOS_REQ_SIZE(sz) ((sz) ? (u32)(sz) : (u32)FDK_CMOS_SIZE)

static void setResponse(fdkCommPkt_t *pPkt, fdkOpCode_t op, u32 len,
                        bool success) {
  pPkt->fdkCommHdr.opCode = op;
  pPkt->fdkCommHdr.errorCode = success ? FDK_SUCCESS : FDK_FAILURE;
  pPkt->fdkCommHdr.pktLen = len;
}

s32 handleRequestPacket(fdkdConnection_t *pConn, u32 rByte) {
  fdkCommPkt_t *pPkt = (fdkCommPkt_t *)pConn->packet;
  u8 *data;
  u64 addr;
  u32 sz, n;
  s32 err;

  switch (pPkt->fdkCommHdr.opCode) {
    case FDK_REQ_CONNECT:
      setResponse(pPkt, FDK_RSP_CONNECT, sizeof(fdkCommHdr_t), true);
      return FDKD_REPLY;

    case FDK_REQ_DISCONNECT:
      setResponse(pPkt, FDK_RSP_DISCONNECT, sizeof(fdkCommHdr_t), true);
      return FDKD_REPLY_AND_CLOSE;

    case FDK_REQ_MEM_READ:
      addr = pPkt->fdkReqMemReadPkt.address;
      sz = pPkt->fdkReqMemReadPkt.size;
      if (rByte < sizeof(fdkReqMemReadPkt_t) ||
          sz > FDK_PKT_MAX_PAYLOAD(fdkRspMemReadPkt_t, memContent)) {
        break;
      }
      data = (u8 *)&pPkt->fdkRspMemReadPkt.memContent;
      err = memReadBuffer(pConn->memfd, addr, sz, data);
      setResponse(pPkt, FDK_RSP_MEM_READ,
                  FDK_PKT_LEN(fdkRspMemReadPkt_t, memContent, sz), !err);
      return FDKD_REPLY;

    case FDK_REQ_MEM_WRITE:
      addr = pPkt->fdkReqMemWritePkt.address;
      sz = pPkt->fdkReqMemWritePkt.size;
      if (sz > FDK_PKT_MAX_PAYLOAD(fdkReqMemWritePkt_t, memContent) ||
          rByte < FDK_PKT_LEN(fdkReqMemWritePkt_t, memContent, sz)) {
        break;
      }
      data = (u8 *)&pPkt->fdkReqMemWritePkt.memContent;
      err = memWriteBuffer(pConn->memfd, addr, sz, data);
      setResponse(pPkt, FDK_RSP_MEM_WRITE, sizeof(fdkRspMemWritePkt_t), !err);
      return FDKD_REPLY;

    case FDK_REQ_IO_READ:
      addr = pPkt->fdkReqIoReadPkt.address;
      sz = pPkt->fdkReqIoReadPkt.size;
      if (rByte < sizeof(fdkReqIoReadPkt_t) ||
          sz > FDK_PKT_MAX_PAYLOAD(fdkRspIoReadPkt_t, ioContent)) {
        break;
      }
      data = (u8 *)&pPkt->fdkRspIoReadPkt.ioContent;
      err = portRead((u16)addr, sz, data);
      setResponse(pPkt, FDK_RSP_IO_READ,
                  FDK_PKT_LEN(fdkRspIoReadPkt_t, ioContent, sz), !err);
      return FDKD_REPLY;

    case FDK_REQ_IO_WRITE:
      addr = pPkt->fdkReqIoWritePkt.address;
      sz = pPkt->fdkReqIoWritePkt.size;
      if (sz > FDK_PKT_MAX_PAYLOAD(fdkReqIoWritePkt_t, ioContent) ||
          rByte < FDK_PKT_LEN(fdkReqIoWritePkt_t, ioContent, sz)) {
        break;
      }
      data = (u8 *)&pPkt->fdkReqIoWritePkt.ioContent;
      err = portWrite((u16)addr, sz, data);
      setResponse(pPkt, FDK_RSP_IO_WRITE, sizeof(fdkRspIoWritePkt_t), !err);
      return FDKD_REPLY;

    case FDK_REQ_PCI_READ:
      addr = pPkt->fdkReqPciReadPkt.address;
      sz = pPkt->fdkReqPciReadPkt.size;
      if (rByte < sizeof(fdkReqPciReadPkt_t) ||
          sz > FDK_PKT_MAX_PAYLOAD(fdkRspPciReadPkt_t, pciContent)) {
        break;
      }
      data = (u8 *)&pPkt->fdkRspPciReadPkt.pciContent;
      err = pciReadConfig((u32)addr, sz, data);
      setResponse(pPkt, FDK_RSP_PCI_READ,
                  FDK_PKT_LEN(fdkRspPciReadPkt_t, pciContent, sz), !err);
      return FDKD_REPLY;

    case FDK_REQ_PCI_WRITE:
      addr = pPkt->fdkReqPciWritePkt.address;
      sz = pPkt->fdkReqPciWritePkt.size;
      if (sz > FDK_PKT_MAX_PAYLOAD(fdkReqPciWritePkt_t, pciContent) ||
          rByte < FDK_PKT_LEN(fdkReqPciWritePkt_t, pciContent, sz)) {
        break;
      }
      data = (u8 *)&pPkt->fdkReqPciWritePkt.pciContent;
      err = pciWriteConfig((u32)addr, sz, data);
      setResponse(pPkt, FDK_RSP_PCI_WRITE, sizeof(fdkRspPciWritePkt_t), !err);
      return FDKD_REPLY;

    case FDK_REQ_IDE_READ:
      addr = pPkt->fdkReqIdeReadPkt.address;
      sz = pPkt->fdkReqIdeReadPkt.size;
      if (rByte < sizeof(fdkReqIdeReadPkt_t) ||
          sz > FDK_PKT_MAX_PAYLOAD(fdkRspIdeReadPkt_t, ideContent)) {
        break;
      }
      data = (u8 *)&pPkt->fdkRspIdeReadPkt.ideContent;
      err = diskRead(pConn->diskPath, addr, sz, data);
      setResponse(pPkt, FDK_RSP_IDE_READ,
                  FDK_PKT_LEN(fdkRspIdeReadPkt_t, ideContent, sz), !err);
      return FDKD_REPLY;

    case FDK_REQ_IDE_WRITE:
      addr = pPkt->fdkReqIdeWritePkt.address;
      sz = pPkt->fdkReqIdeWritePkt.size;
      if (sz > FDK_PKT_MAX_PAYLOAD(fdkReqIdeWritePkt_t, ideContent) ||
          rByte < FDK_PKT_LEN(fdkReqIdeWritePkt_t, ideContent, sz)) {
        break;
      }
      data = (u8 *)&pPkt->fdkReqIdeWritePkt.ideContent;
      err = diskWrite(pConn->diskPath, addr, sz, data);
      setResponse(pPkt, FDK_RSP_IDE_WRITE, sizeof(fdkRspIdeWritePkt_t), !err);
      return FDKD_REPLY;

    case FDK_REQ_CMOS_READ:
      if (rByte < sizeof(fdkReqCmosReadPkt_t)) break;
      addr = pPkt->fdkReqCmosReadPkt.address;
      sz = CMOS_REQ_SIZE(pPkt->fdkReqCmosReadPkt.size);
      data = (u8 *)&pPkt->fdkRspCmosReadPkt.cmosContent;
      err = cmosRead((u8)addr, sz, data);
      setResponse(pPkt, FDK_RSP_CMOS_READ,
                  FDK_PKT_LEN(fdkRspCmosReadPkt_t, cmosContent, sz), !err);
      return FDKD_REPLY;

    case FDK_REQ_CMOS_WRITE:
      addr = pPkt->fdkReqCmosWritePkt.address;
      sz = CMOS_REQ_SIZE(pPkt->fdkReqCmosWritePkt.size);
      if (rByte < FDK_PKT_LEN(fdkReqCmosWritePkt_t, cmosContent, sz)) break;
      data = (u8 *)&pPkt->fdkReqCmosWritePkt.cmosContent;
      err = cmosWrite((u8)addr, sz, data);
      setResponse(pPkt, FDK_RSP_CMOS_WRITE, sizeof(fdkRspCmosWritePkt_t), !err);
      return FDKD_REPLY;

    case FDK_REQ_PCI_LIST:
      n = pciListDevices(
          (fdkPciDev_t *)&pPkt->fdkRspPciListPkt.pciListContent,
          FDK_PKT_MAX_PAYLOAD(fdkRspPciListPkt_t, pciListContent) /
              sizeof(fdkPciDev_t));
      pPkt->fdkRspPciListPkt.numOfPciDevice = n;
      setResponse(pPkt, FDK_RSP_PCI_LIST,
                  FDK_PKT_LEN(fdkRspPciListPkt_t, pciListContent,
                              n * sizeof(fdkPciDev_t)),
                  true);
      return FDKD_REPLY;

    case FDK_REQ_E820_LIST:
      n = e820ReadMap(
          pPkt->fdkRspE820ListPkt.e820ListContent,
          FDK_PKT_MAX_PAYLOAD(fdkRspE820ListPkt_t, e820ListContent) /
              sizeof(fdkE820record_t));
      pPkt->fdkRspE820ListPkt.numOfE820Record = n;
      setResponse(pPkt, FDK_RSP_E820_LIST,
                  FDK_PKT_LEN(fdkRspE820ListPkt_t, e820ListContent,
                              n * sizeof(fdkE820record_t)),
                  true);
      return FDKD_REPLY;

    default:
      break;
  }

  // Unknown or malformed request.
  setResponse(pPkt, FDK_RSP_NACK, sizeof(fdkCommHdr_t), false);
  return FDKD_REPLY;
}
