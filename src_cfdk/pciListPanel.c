/*
 * FDK - Firmware Debug Kit
 * File: pciListPanel.c
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

#include <stdio.h>
#include <string.h>

#include "cfdk.h"

void printPciListBasePanel(fdkUiProperty_t *pFdkUiProperty) {
  printWindowAt(pFdkUiProperty->fdkPciListPanel, title, FDK_PCIL_TITLE_LINE,
                FDK_PCIL_TITLE_COLUMN, FDK_PCIL_TITLE_X_POS,
                FDK_PCIL_TITLE_Y_POS, YELLOW_BLACK, FDK_PCIL_TITLE);
}

// Formats one line of the device listing.
static s32 formatPciLine(const fdkUiProperty_t *pFdkUiProperty, s32 i, s8 *buf,
                         size_t len) {
  const fdkPciIds_t *pIds = &pFdkUiProperty->pFdkPciIds[i];
  const fdkPciDev_t *pDev = &pFdkUiProperty->pFdkPciDev[i];

  return snprintf(buf, len, FDK_PCIL_LINE_FMT, pIds->venTxt, pIds->devTxt,
                  pDev->vendorId, pDev->deviceId, pDev->bus, pDev->dev,
                  pDev->fun);
}

void printPciListUpdatePanel(fdkUiProperty_t *pFdkUiProperty) {
  fdkPciListPanel_t *pPanel = &pFdkUiProperty->fdkPciListPanel;
  s8 buf[FDK_BUF_SIZE] = "", hlbuf[FDK_PCIL_CON_COLUMN + 1] = "";
  s8 *p = buf;
  s32 i, start, end;

  if (!pFdkUiProperty->numOfPciDevice) {
    printWindowAt(pFdkUiProperty->fdkPciListPanel, content, FDK_PCIL_CON_LINE,
                  FDK_PCIL_CON_COLUMN, FDK_PCIL_CON_X_POS, FDK_PCIL_CON_Y_POS,
                  WHITE_BLUE, "%s", "No PCI devices found.");
    return;
  }

  start = pPanel->pageOffset;
  end = pPanel->pageOffset + FDK_REC_PER_PAGE;
  if (end > (s32)pFdkUiProperty->numOfPciDevice) {
    end = (s32)pFdkUiProperty->numOfPciDevice;
  }

  for (i = start; i < end; i++) {
    p += formatPciLine(pFdkUiProperty, i, p, sizeof(buf) - (size_t)(p - buf));
    if (i == pPanel->hlIndex + pPanel->pageOffset) {
      formatPciLine(pFdkUiProperty, i, hlbuf, sizeof(hlbuf));
    }
  }

  printWindowAt(pFdkUiProperty->fdkPciListPanel, content, FDK_PCIL_CON_LINE,
                FDK_PCIL_CON_COLUMN, FDK_PCIL_CON_X_POS, FDK_PCIL_CON_Y_POS,
                WHITE_BLUE, "%s", buf);

  printWindowMove(pFdkUiProperty->fdkPciListPanel, highlight, FDK_STRING_NLINE,
                  FDK_PCIL_CON_COLUMN, FDK_PCIL_CON_X_POS + pPanel->hlIndex,
                  FDK_PCIL_CON_Y_POS, BLACK_CYAN, "%s", hlbuf);
}

void clearPciListBasePanel(fdkUiProperty_t *pFdkUiProperty) {
  destroyWindow(pFdkUiProperty->fdkPciListPanel, title);
}

void clearPciListUpdatePanel(fdkUiProperty_t *pFdkUiProperty) {
  destroyWindow(pFdkUiProperty->fdkPciListPanel, content);
  destroyWindow(pFdkUiProperty->fdkPciListPanel, highlight);
}

s32 handleKeyPressForPciListPanel(fdkUiProperty_t *pFdkUiProperty) {
  fdkPciListPanel_t *pPanel = &pFdkUiProperty->fdkPciListPanel;
  const s32 num = (s32)pFdkUiProperty->numOfPciDevice;

  switch (pFdkUiProperty->inputBuf) {
    case KEY_UP:
      if (pPanel->hlIndex) {
        pPanel->hlIndex--;
      } else if (pPanel->pageOffset) {
        pPanel->pageOffset--;
      }
      break;

    case KEY_DOWN:
      if (pPanel->hlIndex < FDK_REC_PER_PAGE - 1 &&
          pPanel->pageOffset + pPanel->hlIndex < num - 1) {
        pPanel->hlIndex++;
      } else if (pPanel->pageOffset + FDK_REC_PER_PAGE < num) {
        pPanel->pageOffset++;
      }
      break;

    case KEY_PPAGE:
      if (num < FDK_REC_PER_PAGE) break;
      if (pPanel->pageOffset >= FDK_REC_PER_PAGE) {
        pPanel->pageOffset -= FDK_REC_PER_PAGE;
      } else {
        pPanel->pageOffset = 0;
      }
      pPanel->hlIndex = 0;
      break;

    case KEY_NPAGE:
      if (num < FDK_REC_PER_PAGE) break;
      pPanel->pageOffset += FDK_REC_PER_PAGE;
      if (pPanel->pageOffset > num - FDK_REC_PER_PAGE) {
        pPanel->pageOffset = num - FDK_REC_PER_PAGE;
      }
      pPanel->hlIndex = FDK_REC_PER_PAGE - 1;
      break;

    case KBPRS_ENTER:
      if (!num) break;
      pFdkUiProperty->fdkDumpPanel.byteBase =
          (u64)(pPanel->hlIndex + pPanel->pageOffset);
      return 1;

    default:
      break;
  }

  return 0;
}
