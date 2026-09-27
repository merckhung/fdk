/*
 * FDK - Firmware Debug Kit
 * File: memvr.c
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

#include "memvr.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fdk.h"
#include "libcomm.h"
#include "netsock.h"

#define MEMVR_BYTE_PER_LINE 16

static void help(void) {
  fprintf(stderr, "\n" FDK_COPYRIGHT_TEXT "\n\n");
  fprintf(stderr, MEMVR_PROGRAM_NAME ", Rev. " MEMVR_VERSION "\n\n");
  fprintf(stderr, FDK_AUTHOR_NAME "\n\n");
  fprintf(stderr,
          "usage: memvr ( -r <address> | -w <address/value> | "
          "-d <address/length> |\n"
          "               -o <address/value> | -a <address/value> ) "
          "[-i host] [-p port] [-vbq] [-h]\n\n");
  fprintf(stderr, "\t-h\tPrint help and exit\n");
  fprintf(stderr, "\t-i\tServer address, default is " FDK_DEF_IPADDR "\n");
  fprintf(stderr, "\t-p\tServer TCP port, default is %d\n", FDK_DEF_PORT);
  fprintf(stderr, "\t-v\tDisplay more information\n");
  fprintf(stderr, "\t-b\tDisplay value in bit format\n");
  fprintf(stderr, "\t-q\tQuiet mode, only show result value in hex\n");
  fprintf(stderr, "\t-r\tRead  a 32-bit value from specified address\n");
  fprintf(stderr, "\t-w\tWrite a 32-bit value to specified address\n");
  fprintf(stderr, "\t-o\tOR    a 32-bit value to specified address\n");
  fprintf(stderr, "\t-a\tAND   a 32-bit value to specified address\n");
  fprintf(stderr,
          "\t-d\tDump up to 0x%X bytes of memory from address\n\n"
          "Addresses and values are hexadecimal with a 0x prefix, e.g.\n"
          "  memvr -r 0xFED00000\n  memvr -d 0xF0000/0x100\n\n",
          MEMVR_MEM_MAXLEN);
}

static s32 dumpMemData(s32 fd, u64 baseAddr, u32 len) {
  u8 buf[MEMVR_MEM_MAXLEN];
  u32 i, j;

  if (memoryReadBuffer(fd, baseAddr, len, buf)) {
    fprintf(stderr, "Cannot read memory at 0x%llX\n",
            (unsigned long long)baseAddr);
    return -1;
  }

  printf("\n\n== Dump Memory Start ==\n\n");
  printf(
      "     Address     | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |"
      " ASCII DATA\n");
  printf(
      "-----------------------------------------------------------------"
      "------------------\n");

  for (i = 0; i < len; i += MEMVR_BYTE_PER_LINE) {
    printf("%16.16llX : ", (unsigned long long)(baseAddr + i));
    for (j = 0; j < MEMVR_BYTE_PER_LINE; j++) {
      if (i + j < len) {
        printf("%2.2X ", buf[i + j]);
      } else {
        printf("   ");
      }
    }
    printf("| ");
    for (j = 0; j < MEMVR_BYTE_PER_LINE && i + j < len; j++) {
      const u8 c = buf[i + j];
      putchar(c >= '!' && c <= '~' ? c : '.');
    }
    putchar('\n');
  }

  printf(
      "-----------------------------------------------------------------"
      "------------------\n");
  printf("\n== Dump Memory End ==\n\n");
  return 0;
}

static s32 doRead(s32 sfd, u64 address, bool verbose, bool dbit, bool quiet) {
  u32 val;

  if (verbose) {
    printf("Read value from address: 0x%llX\n", (unsigned long long)address);
  }
  if (memoryReadDWord(sfd, address, &val)) {
    fprintf(stderr, "Cannot read memory at 0x%llX\n",
            (unsigned long long)address);
    return -1;
  }

  if (quiet) {
    printf("0x%8.8X\n", val);
  } else {
    printf("\n[READ] Address = 0x%llX, Value = 0x%8.8X\n",
           (unsigned long long)address, val);
  }
  if (dbit) DisplayInBits(val);
  return 0;
}

static s32 doModify(s32 sfd, u32 opflags, u64 address, u32 value, bool verbose,
                    bool dbit, bool quiet) {
  const s8 *opstr = "WRITE";
  u32 orig, res;
  s32 err;

  if (verbose) {
    printf("Write value: 0x%8.8X to address: 0x%llX\n", value,
           (unsigned long long)address);
  }

  if (memoryReadDWord(sfd, address, &orig)) {
    fprintf(stderr, "Cannot read memory at 0x%llX\n",
            (unsigned long long)address);
    return -1;
  }

  if (opflags & MEMVR_MEM_WRITE) {
    err = memoryWriteDWord(sfd, address, value);
  } else if (opflags & MEMVR_MEM_OR) {
    err = memoryORDWord(sfd, address, value);
    opstr = "OR";
  } else {
    err = memoryANDDWord(sfd, address, value);
    opstr = "AND";
  }
  if (err || memoryReadDWord(sfd, address, &res)) {
    fprintf(stderr, "Cannot write memory at 0x%llX\n",
            (unsigned long long)address);
    return -1;
  }

  if (quiet) {
    printf("0x%8.8X\n", res);
  } else {
    printf("\n[%s] Address = 0x%llX, Value = 0x%8.8X\n", opstr,
           (unsigned long long)address, value);
    printf("[%s] Origin  = 0x%8.8X, Result = 0x%8.8X\n\n", opstr, orig, res);
    if (value != res && (opflags & MEMVR_MEM_WRITE)) {
      printf("[WARNING] Value and Result are different\n\n");
    }
  }

  if (dbit) {
    printf("[Origin]");
    DisplayInBits(orig);
    printf("[Value]");
    DisplayInBits(value);
    printf("[Result]");
    DisplayInBits(res);
  }
  return 0;
}

int main(int argc, char **argv) {
  const s8 *ipAddr = FDK_DEF_IPADDR;
  bool verbose = false, dbit = false, quiet = false;
  u32 opflags = 0;
  u64 address = 0, value = 0, length = 0;
  s32 sfd, ret, c, port = FDK_DEF_PORT;

  while ((c = getopt(argc, argv, "r:i:p:w:d:o:a:qvbh")) != -1) {
    switch (c) {
      case 'r':
        if (!ParseOneParameter(optarg, &address)) goto InvParm;
        opflags |= MEMVR_MEM_READ;
        break;

      case 'w':
      case 'o':
      case 'a':
        if (!ParseTwoParameters(optarg, &address, &value) ||
            value > 0xFFFFFFFFULL) {
          goto InvParm;
        }
        opflags |= c == 'w' ? MEMVR_MEM_WRITE
                            : (c == 'o' ? MEMVR_MEM_OR : MEMVR_MEM_AND);
        break;

      case 'd':
        if (!ParseTwoParameters(optarg, &address, &length) || !length) {
          goto InvParm;
        }
        if (length > MEMVR_MEM_MAXLEN) length = MEMVR_MEM_MAXLEN;
        opflags |= MEMVR_MEM_DUMP;
        break;

      case 'i':
        ipAddr = optarg;
        break;

      case 'p':
        port = atoi(optarg);
        break;

      case 'v':
        verbose = true;
        quiet = false;
        break;

      case 'b':
        dbit = true;
        quiet = false;
        break;

      case 'q':
        quiet = true;
        verbose = false;
        dbit = false;
        break;

      case 'h':
        help();
        return 0;

      default:
      InvParm:
        fprintf(stderr, "\nError: Invalid Parameters\n");
        help();
        return 1;
    }
  }

  if (!opflags) {
    help();
    return 1;
  }

  if (connectSocket(&sfd, ipAddr, port)) {
    fprintf(stderr, "Cannot connect to %s port %d\n", ipAddr, port);
    return 1;
  }

  if (connectToFdkServer(sfd)) {
    fprintf(stderr, "Cannot connect to FDK server\n");
    deinitializeSocket(sfd);
    return 1;
  }

  if (opflags & MEMVR_MEM_READ) {
    ret = doRead(sfd, address, verbose, dbit, quiet);
  } else if (opflags & (MEMVR_MEM_WRITE | MEMVR_MEM_OR | MEMVR_MEM_AND)) {
    ret = doModify(sfd, opflags, address, (u32)value, verbose, dbit, quiet);
  } else {
    if (verbose || !quiet) {
      printf("\n[DUMP MEMORY] Address = 0x%llX, len = 0x%X\n",
             (unsigned long long)address, (u32)length);
    }
    ret = dumpMemData(sfd, address, (u32)length);
  }

  disconnectFromFdkServer(sfd);
  deinitializeSocket(sfd);
  return ret ? 1 : 0;
}
