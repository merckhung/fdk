/*
 * FDK - Firmware Debug Kit
 * File: cfdk.c
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

#include "cfdk.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "netsock.h"

static void help(void) {
  fprintf(stderr, "\n" FDK_COPYRIGHT_TEXT "\n\n");
  fprintf(stderr, CFDK_PROGRAM_NAME ", Version " FDK_REVISION "\n");
  fprintf(stderr, "Author: " FDK_AUTHOR_NAME "\n");
  fprintf(stderr,
          "usage: cfdk [-i host | -d /dev/ttyS0] [-P port] [-p pci.ids] "
          "[-h]\n\n");
  fprintf(
      stderr,
      "\t-i\tServer host name or IPv4/IPv6 address, default is " FDK_DEF_IPADDR
      "\n");
  fprintf(stderr, "\t-P\tServer TCP port, default is %d\n", FDK_DEF_PORT);
  fprintf(stderr, "\t-d\tUART TTY device to talk to the target instead\n");
  fprintf(stderr,
          "\t-p\tPCI IDs database, default is the newest of the bundled\n"
          "\t\tcopy (" FDK_DATADIR "/pci.ids) and the system one\n");
  fprintf(stderr, "\t-h\tPrint help and exit\n\n");
}

static s32 configureTtyDevice(s32 fd) {
  struct termios termSetting;

  if (tcgetattr(fd, &termSetting)) return 1;
  cfmakeraw(&termSetting);
  if (cfsetspeed(&termSetting, B115200)) return 1;
  if (tcsetattr(fd, TCSANOW, &termSetting)) return 1;
  return 0;
}

static void initColorPairs(void) {
  init_pair(WHITE_RED, COLOR_WHITE, COLOR_RED);
  init_pair(WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);
  init_pair(WHITE_YELLOW, COLOR_WHITE, COLOR_YELLOW);
  init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);

  init_pair(BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(BLACK_GREEN, COLOR_BLACK, COLOR_GREEN);
  init_pair(BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW);
  init_pair(BLACK_BLUE, COLOR_BLACK, COLOR_BLUE);
  init_pair(BLACK_CYAN, COLOR_BLACK, COLOR_CYAN);

  init_pair(CYAN_BLUE, COLOR_CYAN, COLOR_BLUE);
  init_pair(CYAN_WHITE, COLOR_CYAN, COLOR_WHITE);

  init_pair(RED_BLUE, COLOR_RED, COLOR_BLUE);
  init_pair(RED_WHITE, COLOR_RED, COLOR_WHITE);

  init_pair(YELLOW_BLUE, COLOR_YELLOW, COLOR_BLUE);
  init_pair(YELLOW_RED, COLOR_YELLOW, COLOR_RED);
  init_pair(YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(YELLOW_WHITE, COLOR_YELLOW, COLOR_WHITE);

  init_pair(MAGENTA_BLUE, COLOR_MAGENTA, COLOR_BLUE);

  init_pair(GREEN_BLUE, COLOR_GREEN, COLOR_BLUE);
  init_pair(GREEN_WHITE, COLOR_GREEN, COLOR_WHITE);
}

// Updates the clock and scrolls the status ticker once per second.
static void updateStatusTimer(fdkUiProperty_t *pFdkUiProperty) {
  fdkBasePanel_t *pBase = &pFdkUiProperty->fdkBasePanel;
  const s32 width = FDK_MAX_COLUMN - FDK_MAX_TIMESTR;
  const time_t now = time(NULL);
  struct tm tmNow;
  s32 len;

  if (now == (time_t)-1 || now - pBase->lastSecond < FDK_STS_INTV_SECS) {
    return;
  }
  if (!localtime_r(&now, &tmNow)) return;
  pBase->lastSecond = now;

  printWindowAt(*pBase, time, FDK_STRING_NLINE, FDK_MAX_TIMESTR, FDK_MAX_LINE,
                width, BLACK_WHITE, "%2.2d:%2.2d:%2.2d", tmNow.tm_hour,
                tmNow.tm_min, tmNow.tm_sec);

  if (!pBase->statusStr) return;
  len = (s32)strlen(pBase->statusStr);
  if (pBase->strIdx >= len) pBase->strIdx = 0;

  printWindowAt(*pBase, status, FDK_STRING_NLINE, width, FDK_MAX_LINE,
                FDK_MIN_COLUMN, BLACK_WHITE, "%.*s", width - 1,
                pBase->statusStr + pBase->strIdx);
  pBase->strIdx++;
}

static void printBasePanel(fdkUiProperty_t *pFdkUiProperty) {
  fdkBasePanel_t *pBase = &pFdkUiProperty->fdkBasePanel;
  const s32 authorLen = (s32)strlen(FDK_AUTHOR_NAME);

  printWindowAt(*pBase, background, FDK_MAX_LINE, FDK_MAX_COLUMN, FDK_MIN_LINE,
                FDK_MIN_COLUMN, WHITE_BLUE, "%s", "");
  printWindowAt(*pBase, logo, FDK_STRING_NLINE, FDK_MAX_COLUMN, FDK_MIN_LINE,
                FDK_MIN_COLUMN, WHITE_RED, CFDK_PROGRAM_NAME " " FDK_REVISION);
  printWindowAt(*pBase, copyright, FDK_STRING_NLINE, authorLen, FDK_MIN_LINE,
                FDK_MAX_COLUMN - authorLen, WHITE_RED, FDK_AUTHOR_NAME);

  update_panels();
  doupdate();
}

static void printBaseHelp(fdkUiProperty_t *pFdkUiProperty) {
  fdkBasePanel_t *pBase = &pFdkUiProperty->fdkBasePanel;

  if (!pBase->toggleHelp) {
    destroyWindow(*pBase, help);
    return;
  }

  printWindowMove(
      *pBase, help, FDK_MAX_LINE - 4, FDK_MAX_COLUMN - 4, FDK_HELP_X_POS,
      FDK_HELP_Y_POS, BLACK_WHITE,
      FDK_COPYRIGHT_TEXT "\nAuthor: " FDK_AUTHOR_NAME
                         "\n\n"
                         "  <F1>:  Help (toggle)\n"
                         "  <F2>:  PCI/PCI-E device listing\n"
                         "  <F3>:  PCI/PCI-E device configuration space\n"
                         "  <F4>:  CPU I/O space (x86 only)\n"
                         "  <F5>:  CPU memory space\n"
                         "  <F6>:  Disk content (first disk, see fdkd -b)\n"
                         "  <F7>:  CMOS content (x86 only)\n"
                         "  <Arrows> move  <PgUp/PgDn> page  <Space> bits\n"
                         "  <Enter> edit/commit byte  <Esc> quit\n\n"
                         "  PCI IDs: %.44s\n"
                         "  Version: %s, %u devices",
      pFdkUiProperty->pciIdsPath[0] ? pFdkUiProperty->pciIdsPath : "(none)",
      pFdkUiProperty->pciIdsVersion[0] ? pFdkUiProperty->pciIdsVersion : "-",
      pFdkUiProperty->numOfPciDevice);
}

// Switches the screen from the previous to the current function.
static void switchFunction(fdkUiProperty_t *pFdkUiProperty) {
  fdkDumpPanel_t *pDump = &pFdkUiProperty->fdkDumpPanel;

  clearDumpBasePanel(pFdkUiProperty);
  clearDumpUpdatePanel(pFdkUiProperty);
  clearPciListBasePanel(pFdkUiProperty);
  clearPciListUpdatePanel(pFdkUiProperty);

  // Keep the device picked from the PCI listing.
  if (pFdkUiProperty->fdkPreviousHwFunc != KHF_PCIL ||
      pFdkUiProperty->fdkHwFunc != KHF_PCI) {
    pDump->byteBase = 0;
  }
  pDump->byteOffset = 0;
  pDump->toggleEditing = 0;
  pFdkUiProperty->fdkPciListPanel.hlIndex = 0;
  pFdkUiProperty->fdkPciListPanel.pageOffset = 0;

  switch (pFdkUiProperty->fdkHwFunc) {
    case KHF_PCIL:
      printPciListBasePanel(pFdkUiProperty);
      return;
    case KHF_IO:
      pDump->infoStr = FDK_INFO_IO_BASE;
      break;
    case KHF_PCI:
      pDump->infoStr = FDK_INFO_PCI_BASE;
      break;
    case KHF_IDE:
      pDump->infoStr = FDK_INFO_IDE_BASE;
      break;
    case KHF_CMOS:
      pDump->infoStr = FDK_INFO_CMOS_BASE;
      break;
    case KHF_MEM:
    default:
      pDump->infoStr = FDK_INFO_MEMORY_BASE;
      break;
  }
  printDumpBasePanel(pFdkUiProperty);
}

// Reads the current screen of data. Returns 0 when it can be displayed.
static s32 readCurrent(fdkUiProperty_t *pFdkUiProperty) {
  switch (pFdkUiProperty->fdkHwFunc) {
    case KHF_IO:
      return readIo(pFdkUiProperty);
    case KHF_PCI:
      return readPci(pFdkUiProperty);
    case KHF_IDE:
      return readIde(pFdkUiProperty);
    case KHF_CMOS:
      return readCmos(pFdkUiProperty);
    case KHF_MEM:
    default:
      return readMemory(pFdkUiProperty);
  }
}

static void runUi(fdkUiProperty_t *pFdkUiProperty) {
  printBasePanel(pFdkUiProperty);

  for (;;) {
    fdkHwFunc_t next = pFdkUiProperty->fdkHwFunc;
    s32 key;

    key = pFdkUiProperty->inputBuf = getch();
    if (key == KBPRS_ESC && !pFdkUiProperty->fdkDumpPanel.toggleEditing) {
      return;
    }

    switch (key) {
      case KEY_F(1):
        pFdkUiProperty->fdkBasePanel.toggleHelp =
            !pFdkUiProperty->fdkBasePanel.toggleHelp;
        break;
      case KEY_F(2):
        next = KHF_PCIL;
        break;
      case KEY_F(3):
        next = KHF_PCI;
        break;
      case KEY_F(4):
        next = KHF_IO;
        break;
      case KEY_F(5):
        next = KHF_MEM;
        break;
      case KEY_F(6):
        next = KHF_IDE;
        break;
      case KEY_F(7):
        next = KHF_CMOS;
        break;
      case KBPRS_ESC:
        // Cancel editing.
        pFdkUiProperty->fdkDumpPanel.toggleEditing = 0;
        break;
      default:
        if (next == KHF_INIT) next = KHF_MEM;
        break;
    }

    pFdkUiProperty->fdkPreviousHwFunc = pFdkUiProperty->fdkHwFunc;
    pFdkUiProperty->fdkHwFunc = next;
    if (pFdkUiProperty->fdkPreviousHwFunc != next) {
      switchFunction(pFdkUiProperty);
    }

    if (pFdkUiProperty->fdkHwFunc == KHF_PCIL) {
      if (handleKeyPressForPciListPanel(pFdkUiProperty)) {
        // Enter on a device opens its configuration space.
        pFdkUiProperty->fdkPreviousHwFunc = KHF_PCIL;
        pFdkUiProperty->fdkHwFunc = KHF_PCI;
        pFdkUiProperty->inputBuf = ERR;
        switchFunction(pFdkUiProperty);
      } else {
        printPciListUpdatePanel(pFdkUiProperty);
      }
    }

    if (pFdkUiProperty->fdkHwFunc != KHF_PCIL) {
      handleKeyPressForDumpPanel(pFdkUiProperty);
      if (!readCurrent(pFdkUiProperty)) printDumpUpdatePanel(pFdkUiProperty);
    }

    printBaseHelp(pFdkUiProperty);
    updateStatusTimer(pFdkUiProperty);
    update_panels();
    doupdate();
  }
}

int main(int argc, char **argv) {
  static fdkUiProperty_t fdkUiProperty;
  const s8 *ipAddr = FDK_DEF_IPADDR;
  const s8 *ttyDevice = NULL;
  s32 c, ret = 1, port = FDK_DEF_PORT;
  bool connected = false;

  fdkUiProperty.fd = -1;
  fdkUiProperty.fdkHwFunc = fdkUiProperty.fdkPreviousHwFunc = KHF_INIT;
  fdkUiProperty.pFdkCommPkt = (fdkCommPkt_t *)fdkUiProperty.pktBuf;
  fdkUiProperty.fdkBasePanel.statusStr = FDK_WELCOME_TXT;

  while ((c = getopt(argc, argv, "p:i:P:d:h")) != -1) {
    switch (c) {
      case 'd':
        ttyDevice = optarg;
        break;
      case 'i':
        ipAddr = optarg;
        ttyDevice = NULL;
        break;
      case 'P':
        port = atoi(optarg);
        break;
      case 'p':
        snprintf(fdkUiProperty.pciIdsPath, sizeof(fdkUiProperty.pciIdsPath),
                 "%s", optarg);
        break;
      case 'h':
        help();
        return 0;
      default:
        help();
        return 1;
    }
  }

  if (fdkUiProperty.pciIdsPath[0]) {
    readPciIdsVersion(fdkUiProperty.pciIdsPath, fdkUiProperty.pciIdsVersion,
                      sizeof(fdkUiProperty.pciIdsVersion));
  } else {
    if (findPciIdsFile(
            fdkUiProperty.pciIdsPath, sizeof(fdkUiProperty.pciIdsPath),
            fdkUiProperty.pciIdsVersion, sizeof(fdkUiProperty.pciIdsVersion))) {
      fprintf(stderr, "Warning: no pci.ids found, device names unavailable\n");
    }
  }

  if (ttyDevice) {
    fdkUiProperty.fd = open(ttyDevice, O_RDWR | O_NOCTTY | O_CLOEXEC);
    if (fdkUiProperty.fd < 0) {
      fprintf(stderr, "Cannot open device %s\n", ttyDevice);
      return 1;
    }
    if (configureTtyDevice(fdkUiProperty.fd)) {
      fprintf(stderr, "Cannot configure device %s\n", ttyDevice);
      goto Exit;
    }
  } else if (connectSocket(&fdkUiProperty.fd, ipAddr, port)) {
    fprintf(stderr, "Cannot connect to %s port %d\n", ipAddr, port);
    return 1;
  }

  if (connectToFdkServer(&fdkUiProperty)) {
    fprintf(stderr, "Cannot connect to FDK server\n");
    goto Exit;
  }
  connected = true;

  if (readPciList(&fdkUiProperty)) {
    fprintf(stderr, "Cannot get PCI device listing\n");
    goto Exit;
  }

  if (readE820List(&fdkUiProperty)) {
    fprintf(stderr, "Cannot get E820 listing\n");
    goto Exit;
  }

  initscr();
  if (LINES < FDK_MAX_LINE + 1 || COLS < FDK_MAX_COLUMN) {
    endwin();
    fprintf(stderr, "The terminal must be at least %dx%d\n", FDK_MAX_COLUMN,
            FDK_MAX_LINE + 1);
    goto Exit;
  }
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  set_escdelay(25);
  timeout(FDK_REFRESH_MS);
  curs_set(0);
  initColorPairs();

  runUi(&fdkUiProperty);
  endwin();
  ret = 0;

Exit:
  if (connected) disconnectFromFdkServer(&fdkUiProperty);
  if (ttyDevice) {
    close(fdkUiProperty.fd);
  } else {
    deinitializeSocket(fdkUiProperty.fd);
  }
  free(fdkUiProperty.pFdkE820record);
  free(fdkUiProperty.pFdkPciDev);
  free(fdkUiProperty.pFdkPciIds);
  return ret;
}
