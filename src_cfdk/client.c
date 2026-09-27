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

#include <stdlib.h>
#include <string.h>

#include "cfdk.h"

// Issues a request using the UI's packet buffer.
static s32 execute(fdkUiProperty_t *pFdkUiProperty, fdkOpCode_t op, u64 addr,
                   u32 size, const u8 *cntBuf) {
  return executeFunction(pFdkUiProperty->fd, op, addr, size, cntBuf,
                         pFdkUiProperty->pktBuf,
                         sizeof(pFdkUiProperty->pktBuf));
}

// Writes the byte being edited at the cursor position.
static s32 writeEditedByte(fdkUiProperty_t *pFdkUiProperty, fdkOpCode_t op,
                           u64 base) {
  const fdkDumpPanel_t *pDump = &pFdkUiProperty->fdkDumpPanel;
  return execute(pFdkUiProperty, op, base + (u64)pDump->byteOffset,
                 sizeof(pDump->editingBuf), &pDump->editingBuf);
}

s32 connectToFdkServer(fdkUiProperty_t *pFdkUiProperty) {
  return execute(pFdkUiProperty, FDK_REQ_CONNECT, 0, 0, NULL);
}

void disconnectFromFdkServer(fdkUiProperty_t *pFdkUiProperty) {
  execute(pFdkUiProperty, FDK_REQ_DISCONNECT, 0, 0, NULL);
}

s32 readPciList(fdkUiProperty_t *pFdkUiProperty) {
  const fdkRspPciListPkt_t *pRsp = (fdkRspPciListPkt_t *)pFdkUiProperty->pktBuf;
  u32 n;

  if (execute(pFdkUiProperty, FDK_REQ_PCI_LIST, 0, 0, NULL)) return 1;

  n = pRsp->numOfPciDevice;
  if (n > FDK_PKT_MAX_PAYLOAD(fdkRspPciListPkt_t, pciListContent) /
              sizeof(fdkPciDev_t)) {
    return 1;
  }

  // Allocate at least one entry so an empty bus still yields valid pointers.
  pFdkUiProperty->pFdkPciDev = calloc(n ? n : 1, sizeof(fdkPciDev_t));
  pFdkUiProperty->pFdkPciIds = calloc(n ? n : 1, sizeof(fdkPciIds_t));
  if (!pFdkUiProperty->pFdkPciDev || !pFdkUiProperty->pFdkPciIds) return 1;

  memcpy(pFdkUiProperty->pFdkPciDev, &pRsp->pciListContent,
         n * sizeof(fdkPciDev_t));
  pFdkUiProperty->numOfPciDevice = n;

  loadPciNames(pFdkUiProperty);
  return 0;
}

fdkPciDev_t *getPciDevice(fdkUiProperty_t *pFdkUiProperty, u64 num) {
  if (num >= pFdkUiProperty->numOfPciDevice) return NULL;
  return pFdkUiProperty->pFdkPciDev + num;
}

s32 readE820List(fdkUiProperty_t *pFdkUiProperty) {
  const fdkRspE820ListPkt_t *pRsp =
      (fdkRspE820ListPkt_t *)pFdkUiProperty->pktBuf;
  u32 n;

  if (execute(pFdkUiProperty, FDK_REQ_E820_LIST, 0, 0, NULL)) return 1;

  n = pRsp->numOfE820Record;
  if (n > FDK_PKT_MAX_PAYLOAD(fdkRspE820ListPkt_t, e820ListContent) /
              sizeof(fdkE820record_t)) {
    return 1;
  }

  pFdkUiProperty->pFdkE820record = calloc(n ? n : 1, sizeof(fdkE820record_t));
  if (!pFdkUiProperty->pFdkE820record) return 1;

  memcpy(pFdkUiProperty->pFdkE820record, pRsp->e820ListContent,
         n * sizeof(fdkE820record_t));
  pFdkUiProperty->numOfE820Record = n;
  return 0;
}

s32 readMemory(fdkUiProperty_t *pFdkUiProperty) {
  return execute(pFdkUiProperty, FDK_REQ_MEM_READ,
                 pFdkUiProperty->fdkDumpPanel.byteBase, FDK_BYTE_PER_SCREEN,
                 NULL);
}

s32 writeMemoryByEditing(fdkUiProperty_t *pFdkUiProperty) {
  return writeEditedByte(pFdkUiProperty, FDK_REQ_MEM_WRITE,
                         pFdkUiProperty->fdkDumpPanel.byteBase);
}

s32 readIo(fdkUiProperty_t *pFdkUiProperty) {
  return execute(pFdkUiProperty, FDK_REQ_IO_READ,
                 pFdkUiProperty->fdkDumpPanel.byteBase, FDK_BYTE_PER_SCREEN,
                 NULL);
}

s32 writeIoByEditing(fdkUiProperty_t *pFdkUiProperty) {
  return writeEditedByte(pFdkUiProperty, FDK_REQ_IO_WRITE,
                         pFdkUiProperty->fdkDumpPanel.byteBase);
}

s32 readIde(fdkUiProperty_t *pFdkUiProperty) {
  return execute(pFdkUiProperty, FDK_REQ_IDE_READ,
                 pFdkUiProperty->fdkDumpPanel.byteBase, FDK_BYTE_PER_SCREEN,
                 NULL);
}

s32 writeIdeByEditing(fdkUiProperty_t *pFdkUiProperty) {
  return writeEditedByte(pFdkUiProperty, FDK_REQ_IDE_WRITE,
                         pFdkUiProperty->fdkDumpPanel.byteBase);
}

s32 readCmos(fdkUiProperty_t *pFdkUiProperty) {
  return execute(pFdkUiProperty, FDK_REQ_CMOS_READ,
                 pFdkUiProperty->fdkDumpPanel.byteBase, FDK_BYTE_PER_SCREEN,
                 NULL);
}

s32 writeCmosByEditing(fdkUiProperty_t *pFdkUiProperty) {
  return writeEditedByte(pFdkUiProperty, FDK_REQ_CMOS_WRITE,
                         pFdkUiProperty->fdkDumpPanel.byteBase);
}

s32 readPci(fdkUiProperty_t *pFdkUiProperty) {
  const fdkPciDev_t *pDev =
      getPciDevice(pFdkUiProperty, pFdkUiProperty->fdkDumpPanel.byteBase);

  if (!pDev) return 1;
  return execute(pFdkUiProperty, FDK_REQ_PCI_READ,
                 calculatePciAddress(pDev->bus, pDev->dev, pDev->fun),
                 FDK_BYTE_PER_SCREEN, NULL);
}

s32 writePciByEditing(fdkUiProperty_t *pFdkUiProperty) {
  const fdkPciDev_t *pDev =
      getPciDevice(pFdkUiProperty, pFdkUiProperty->fdkDumpPanel.byteBase);

  if (!pDev) return 1;
  return writeEditedByte(pFdkUiProperty, FDK_REQ_PCI_WRITE,
                         calculatePciAddress(pDev->bus, pDev->dev, pDev->fun));
}

// |bus| carries the PCI segment in bits 15:8, which lands in the reserved
// bits 30:24 of the address.
u32 calculatePciAddress(u16 bus, u8 dev, u8 func) {
  return 0x80000000U | (((u32)bus & 0x7FFF) << 16) | (((u32)dev & 0x1F) << 11) |
         (((u32)func & 0x07) << 8);
}
