/*
 * FDK - Firmware Debug Kit
 * File: dumpPanel.c
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

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "cfdk.h"
#include "libcomm.h"

static u32 editorColorCount = 0;

// Returns the payload of the last read response for the current function.
static const u8 *dumpData(const fdkUiProperty_t *pFdkUiProperty) {
  const fdkCommPkt_t *pPkt = pFdkUiProperty->pFdkCommPkt;

  switch (pFdkUiProperty->fdkHwFunc) {
    case KHF_PCI:
      return (const u8 *)&pPkt->fdkRspPciReadPkt.pciContent;
    case KHF_IO:
      return (const u8 *)&pPkt->fdkRspIoReadPkt.ioContent;
    case KHF_IDE:
      return (const u8 *)&pPkt->fdkRspIdeReadPkt.ideContent;
    case KHF_CMOS:
      return (const u8 *)&pPkt->fdkRspCmosReadPkt.cmosContent;
    default:
      return (const u8 *)&pPkt->fdkRspMemReadPkt.memContent;
  }
}

void printDumpBasePanel(fdkUiProperty_t *pFdkUiProperty) {
  printWindowAt(pFdkUiProperty->fdkDumpPanel, top, FDK_STRING_NLINE,
                (s32)strlen(FDK_DUMP_TOP_BAR), FDK_DUMP_TOP_LINE,
                FDK_DUMP_TOP_COLUMN, GREEN_BLUE, FDK_DUMP_TOP_BAR);

  if (pFdkUiProperty->fdkHwFunc != KHF_PCI) {
    printWindowAt(pFdkUiProperty->fdkDumpPanel, rtop, FDK_STRING_NLINE,
                  FDK_DUMP_BYTE_PER_LINE, FDK_DUMP_RTOP_LINE,
                  FDK_DUMP_RTOP_COLUMN, RED_BLUE, FDK_DUMP_RTOP_BAR);
  }

  printWindowAt(pFdkUiProperty->fdkDumpPanel, left, FDK_DUMP_BYTE_PER_LINE, 4,
                FDK_DUMP_LEFT_LINE, FDK_DUMP_LEFT_COLUMN, GREEN_BLUE,
                FDK_DUMP_LEFT_BAR);

  printWindowAt(pFdkUiProperty->fdkDumpPanel, info, FDK_STRING_NLINE,
                FDK_MAX_COLUMN, FDK_INFO_LINE, FDK_INFO_COLUMN, WHITE_BLUE,
                "%s", pFdkUiProperty->fdkDumpPanel.infoStr);
}

// Formats the decoded type 0 header shown to the right of a PCI dump.
static void formatPciSummary(const u8 *data, s8 *buf, size_t len) {
  const fdkPciConfig_t *pCfg = (const fdkPciConfig_t *)data;
  s32 i, n;

  n = snprintf(buf, len,
               "VEN ID: %4.4Xh\nDEV ID: %4.4Xh\n\n"
               "Rev ID  : %2.2Xh\nInt Line: %2.2Xh\nInt Pin : %2.2Xh\n\n",
               pCfg->vendorId, pCfg->deviceId, pCfg->revisionId, pCfg->intLine,
               pCfg->intPin);

  for (i = 0; i < 6 && n > 0 && (size_t)n < len; i++) {
    const u32 bar = pCfg->baseAddrReg[i];
    const bool io = bar & FDK_PCIBAR_IO;

    n += snprintf(buf + n, len - (size_t)n, "%s: %8.8Xh\n", io ? "I/O" : "Mem",
                  bar & (io ? FDK_PCIBAR_IOBA_MASK : FDK_PCIBAR_MEMBA_MASK));
  }

  if (n > 0 && (size_t)n < len) {
    snprintf(buf + n, len - (size_t)n, "\nROM: %8.8Xh\n", pCfg->expRomBaseAddr);
  }
}

static void printBaseAddress(fdkUiProperty_t *pFdkUiProperty) {
  fdkDumpPanel_t *pDump = &pFdkUiProperty->fdkDumpPanel;
  const s32 column = (s32)strlen(pDump->infoStr);
  const fdkPciDev_t *pDev;
  const fdkPciIds_t *pIds;

  switch (pFdkUiProperty->fdkHwFunc) {
    case KHF_IO:
      printWindowAt(*pDump, baseaddr, FDK_STRING_NLINE, 5,
                    FDK_DUMP_BASEADDR_LINE, column, WHITE_BLUE,
                    FDK_INFO_IO_BASE_FMT, (u32)(pDump->byteBase & 0xFFFF));
      break;

    case KHF_PCI:
      pDev = getPciDevice(pFdkUiProperty, pDump->byteBase);
      if (!pDev) break;
      pIds = &pFdkUiProperty->pFdkPciIds[pDump->byteBase];

      printWindowAt(*pDump, baseaddr, FDK_STRING_NLINE, 29,
                    FDK_DUMP_BASEADDR_LINE, column, WHITE_BLUE,
                    FDK_INFO_PCI_BASE_FMT, pDev->bus, pDev->dev, pDev->fun);
      printWindowAt(*pDump, ftitle, FDK_STRING_NLINE, FDK_MAX_PCINAME,
                    FDK_DUMP_FTITLE_LINE, FDK_DUMP_FTITLE_COLUMN, WHITE_BLUE,
                    "%s: %s", FDK_FTITLE_PCI, pIds->venTxt);
      printWindowAt(*pDump, stitle, FDK_STRING_NLINE, FDK_MAX_PCINAME,
                    FDK_DUMP_STITLE_LINE, FDK_DUMP_FTITLE_COLUMN, WHITE_BLUE,
                    "%s: %s", FDK_STITLE_PCI, pIds->devTxt);
      break;

    case KHF_CMOS:
      printWindowAt(*pDump, baseaddr, FDK_STRING_NLINE, 20,
                    FDK_DUMP_BASEADDR_LINE, column, WHITE_BLUE,
                    FDK_INFO_CMOS_BASE_FMT, (u32)(pDump->byteBase & 0xFF));
      break;

    case KHF_IDE:
    case KHF_MEM:
    default:
      printWindowAt(*pDump, baseaddr, FDK_STRING_NLINE, 20,
                    FDK_DUMP_BASEADDR_LINE, column, WHITE_BLUE,
                    FDK_INFO_MEMORY_BASE_FMT, (u32)(pDump->byteBase >> 32),
                    (u32)(pDump->byteBase & 0xFFFFFFFFULL));
      break;
  }
}

void printDumpUpdatePanel(fdkUiProperty_t *pFdkUiProperty) {
  fdkDumpPanel_t *pDump = &pFdkUiProperty->fdkDumpPanel;
  const bool isPci = pFdkUiProperty->fdkHwFunc == KHF_PCI;
  const u8 *data = dumpData(pFdkUiProperty);
  s8 valueBuf[FDK_DUMP_VBUF_SZ + 1];
  s8 asciiBuf[FDK_DUMP_ABUF_SZ + 1];
  s8 *vp = valueBuf;
  u8 cur;
  s32 i, x, y, color;

  // Hex values, 16 per line, and their ASCII rendering.
  for (i = 0; i < FDK_BYTE_PER_SCREEN; i++) {
    vp +=
        sprintf(vp, (i % FDK_DUMP_BYTE_PER_LINE) ? " %2.2X" : "%2.2X", data[i]);
    asciiBuf[i] = (s8)FDK_DUMP_ASCII_FILTER(data[i]);
  }
  asciiBuf[FDK_BYTE_PER_SCREEN] = 0;

  // PCI shows the decoded header instead of ASCII.
  if (isPci) formatPciSummary(data, asciiBuf, sizeof(asciiBuf));

  printWindowAt(*pDump, value, FDK_DUMP_BYTE_PER_LINE, FDK_DUMP_BUF_PER_LINE,
                FDK_DUMP_VALUE_LINE, FDK_DUMP_VALUE_COLUMN, WHITE_BLUE, "%s",
                valueBuf);
  printWindowAt(*pDump, ascii, FDK_DUMP_BYTE_PER_LINE, FDK_DUMP_BYTE_PER_LINE,
                FDK_DUMP_ASCII_LINE, FDK_DUMP_ASCII_COLUMN, WHITE_BLUE, "%s",
                asciiBuf);
  printWindowAt(*pDump, offset, FDK_STRING_NLINE, 4, FDK_DUMP_OFF_LINE,
                FDK_DUMP_OFF_COLUMN, YELLOW_BLUE, "%4.4X", pDump->byteOffset);

  printBaseAddress(pFdkUiProperty);

  // The server fills unreadable ranges with FFh and flags the response.
  if (pFdkUiProperty->pFdkCommPkt->fdkCommHdr.errorCode != FDK_SUCCESS) {
    printWindowAt(*pDump, error, FDK_STRING_NLINE, 48, FDK_DUMP_ERROR_LINE,
                  FDK_DUMP_ERROR_COLUMN, WHITE_RED, "%s",
                  " Read failed: not present or denied by kernel ");
  } else {
    destroyWindow(*pDump, error);
  }

  // Cursor, or the byte being edited.
  cur = data[pDump->byteOffset];
  y = pDump->byteOffset / FDK_DUMP_BYTE_PER_LINE + FDK_DUMP_VALUE_LINE;
  x = (pDump->byteOffset % FDK_DUMP_BYTE_PER_LINE) * 3 + FDK_DUMP_VALUE_COLUMN;
  if (pDump->toggleEditing) {
    color = (editorColorCount++ % 2) ? YELLOW_RED : YELLOW_BLACK;
    printWindowMove(*pDump, highlight, FDK_STRING_NLINE, FDK_DUMP_HL_DIGITS, y,
                    x, color, "%2.2X", pDump->editingBuf);
  } else {
    printWindowMove(*pDump, highlight, FDK_STRING_NLINE, FDK_DUMP_HL_DIGITS, y,
                    x, YELLOW_RED, "%2.2X", cur);
    pDump->editingBuf = cur;
  }

  if (pDump->toggleBits) {
    printWindowMove(*pDump, bits, FDK_STRING_NLINE, FDK_DUMP_BITS_DIGITS, y + 1,
                    x, WHITE_RED, "%u%u%u%u_%u%u%u%u", FDK_GET_BIT(cur, 7),
                    FDK_GET_BIT(cur, 6), FDK_GET_BIT(cur, 5),
                    FDK_GET_BIT(cur, 4), FDK_GET_BIT(cur, 3),
                    FDK_GET_BIT(cur, 2), FDK_GET_BIT(cur, 1),
                    FDK_GET_BIT(cur, 0));
  } else {
    destroyWindow(*pDump, bits);
  }

  if (!isPci) {
    y = pDump->byteOffset / FDK_DUMP_BYTE_PER_LINE + FDK_DUMP_ASCII_LINE;
    x = pDump->byteOffset % FDK_DUMP_BYTE_PER_LINE + FDK_DUMP_ASCII_COLUMN;
    printWindowMove(*pDump, hlascii, FDK_STRING_NLINE, FDK_DUMP_HLA_DIGITS, y,
                    x, YELLOW_RED, "%c", FDK_DUMP_ASCII_FILTER(cur));
  }
}

void clearDumpBasePanel(fdkUiProperty_t *pFdkUiProperty) {
  destroyWindow(pFdkUiProperty->fdkDumpPanel, top);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, rtop);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, left);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, info);
}

void clearDumpUpdatePanel(fdkUiProperty_t *pFdkUiProperty) {
  destroyWindow(pFdkUiProperty->fdkDumpPanel, value);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, ascii);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, offset);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, baseaddr);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, highlight);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, hlascii);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, bits);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, ftitle);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, stitle);
  destroyWindow(pFdkUiProperty->fdkDumpPanel, error);
}

static void writeEditedByte(fdkUiProperty_t *pFdkUiProperty) {
  switch (pFdkUiProperty->fdkHwFunc) {
    case KHF_MEM:
      writeMemoryByEditing(pFdkUiProperty);
      break;
    case KHF_IO:
      writeIoByEditing(pFdkUiProperty);
      break;
    case KHF_PCI:
      writePciByEditing(pFdkUiProperty);
      break;
    case KHF_IDE:
      writeIdeByEditing(pFdkUiProperty);
      break;
    case KHF_CMOS:
      writeCmosByEditing(pFdkUiProperty);
      break;
    default:
      break;
  }
}

// Moves one screen up or down in the current address space.
static void pageDump(fdkUiProperty_t *pFdkUiProperty, bool down) {
  fdkDumpPanel_t *pDump = &pFdkUiProperty->fdkDumpPanel;
  const u64 num = pFdkUiProperty->numOfPciDevice;

  switch (pFdkUiProperty->fdkHwFunc) {
    case KHF_PCI:
      // byteBase is the index of the device being shown.
      if (num) pDump->byteBase = (pDump->byteBase + (down ? 1 : num - 1)) % num;
      break;

    case KHF_IO:
      pDump->byteBase = (down ? pDump->byteBase + FDK_BYTE_PER_SCREEN
                              : pDump->byteBase - FDK_BYTE_PER_SCREEN) &
                        (FDK_MAXADDR_IO & ~0xFFULL);
      break;

    case KHF_CMOS:
      // The whole CMOS fits on one screen.
      break;

    case KHF_IDE:
      if (down) {
        pDump->byteBase += FDK_BYTE_PER_SCREEN;
      } else if (pDump->byteBase >= FDK_BYTE_PER_SCREEN) {
        pDump->byteBase -= FDK_BYTE_PER_SCREEN;
      }
      break;

    case KHF_MEM:
    default:
      // Wraps around the 64-bit physical address space.
      if (down) {
        pDump->byteBase += FDK_BYTE_PER_SCREEN;
      } else {
        pDump->byteBase -= FDK_BYTE_PER_SCREEN;
      }
      break;
  }
}

void handleKeyPressForDumpPanel(fdkUiProperty_t *pFdkUiProperty) {
  fdkDumpPanel_t *pDump = &pFdkUiProperty->fdkDumpPanel;
  const s32 key = pFdkUiProperty->inputBuf;
  const s32 col = pDump->byteOffset % FDK_DUMP_BYTE_PER_LINE;

  if (pDump->toggleEditing) {
    if (key == KBPRS_ENTER) {
      writeEditedByte(pFdkUiProperty);
      pDump->toggleEditing = 0;
    } else if (key >= 0 && key < 0x80 && isxdigit(key)) {
      // Shift the new hex digit in from the right.
      pDump->editingBuf =
          (u8)((pDump->editingBuf << 4) |
               (isdigit(key) ? key - '0' : tolower(key) - 'a' + 10));
    }
    return;
  }

  switch (key) {
    case KEY_UP:
      pDump->byteOffset =
          (pDump->byteOffset - FDK_DUMP_BYTE_PER_LINE + FDK_BYTE_PER_SCREEN) %
          FDK_BYTE_PER_SCREEN;
      break;

    case KEY_DOWN:
      pDump->byteOffset =
          (pDump->byteOffset + FDK_DUMP_BYTE_PER_LINE) % FDK_BYTE_PER_SCREEN;
      break;

    case KEY_LEFT:
      pDump->byteOffset += col ? -1 : FDK_DUMP_BYTE_PER_LINE - 1;
      break;

    case KEY_RIGHT:
      pDump->byteOffset += (col < FDK_DUMP_BYTE_PER_LINE - 1)
                               ? 1
                               : -(FDK_DUMP_BYTE_PER_LINE - 1);
      break;

    case KEY_PPAGE:
      pageDump(pFdkUiProperty, false);
      break;

    case KEY_NPAGE:
      pageDump(pFdkUiProperty, true);
      break;

    case KBPRS_ENTER:
      pDump->toggleEditing = 1;
      break;

    case KBPRS_SPACE:
      pDump->toggleBits = !pDump->toggleBits;
      break;

    default:
      break;
  }
}
