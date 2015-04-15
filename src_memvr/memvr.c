/*
 * FDK - Firmware Debug Kit
 * File: memvr.c
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

#include <mtypes.h>
#include <fdk.h>
#include <packet.h>
#include <libcomm.h>
#include <netsock.h>

#include "memvr.h"

static s8 *ErrorStr = "\nError: ";
static s8 *ErrInvParm = "Invalid Parameters";
static s8 *WriteStr = "WRITE";
static s8 *OrStr = "OR";
static s8 *AndStr = "AND";

static void help(void) {

  fprintf( stderr, "\n" FDK_COPYRIGHT_TEXT "\n\n");
  fprintf( stderr, MEMVR_PROGRAM_NAME ", Rev. " MEMVR_VERSION "\n\n");

  fprintf( stderr, FDK_AUTHOR_NAME "\n\n");

  fprintf( stderr,
      "help: memvr ( [-r <address>] | [-w <address/value>] | [-d <address/length>] |\n");
  fprintf( stderr,
      "                   [-o <address/value>] | [-a <address/value>] | -[hvbq] )\n\n");

  fprintf( stderr, "\t-h\tPrint help and exit\n");
  fprintf( stderr, "\t-i\tIPv4 address, default is " FDK_DEF_IPADDR "\n");
  fprintf( stderr, "\t-v\tDisplay more information\n");
  fprintf( stderr, "\t-b\tDisplay Value in bit format\n");
  fprintf( stderr, "\t-q\tQuiet mode, only show result value in hex\n");
  fprintf( stderr, "\t-r\tRead  a 32-bit value from specified address\n");
  fprintf( stderr, "\t-w\tWrite a 32-bit value to specified address\n");
  fprintf( stderr, "\t-o\tOR    a 32-bit value to specified address\n");
  fprintf( stderr, "\t-a\tAND   a 32-bit value to specified address\n");
  fprintf( stderr,
      "\t-d\tDump specified length of memory content from address\n\n");
}

void dumpMemData(s32 fd, u32 len, u32 baseAddr) {

  u32 i, j;
  u8 c;

  printf("\n\n== Dump Memory Start ==\n\n");
  printf(
      " Address | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|   ASCII DATA   \n");
  printf(
      "---------------------------------------------------------------------------\n");

  for (i = baseAddr; i <= (baseAddr + len); i++) {

    if (!((i - baseAddr) % 16)) {

      if (((i - baseAddr) > 15)) {

        for (j = i - 16; j < i; j++) {

          c = memoryReadByte(fd, j);
          if (((c >= '!') && (c <= '~'))) {

            printf("%c", c);
          } else {

            printf(".");
          }
        }
      }

      if (i) {

        printf("\n");
      }

      if (i == (baseAddr + len)) {

        break;
      }

      printf("%8.8X : ", i);
    }

    printf("%2.2X ", memoryReadByte(fd, i) & 0xFF);
  }

  printf(
      "\n---------------------------------------------------------------------------\n");
  printf("\n== Dump Memory End ==\n\n");
}

s32 main(s32 argc, s8 **argv) {

  s32 sfd, ret;
  u32 c, opflags, verbose, dbit, quiet;
  u64 address;
  u32 value, length;
  s8 ipAddr[FDK_MAX_PATH];

  ////////////////////////////////////////////////////////////////////////////
  // Initialization                                                         //
  ////////////////////////////////////////////////////////////////////////////
  ret = 0;
  opflags = 0;
  verbose = 0;
  dbit = 0;
  quiet = 0;

  address = 0;
  value = 0;
  length = 0;

  strncpy(ipAddr, FDK_DEF_IPADDR, FDK_MAX_PATH);

  ////////////////////////////////////////////////////////////////////////////
  // Handle Paramaters                                                      //
  ////////////////////////////////////////////////////////////////////////////
  while ((c = getopt(argc, argv, "r:i:w:d:o:a:qvbh")) != EOF) {

    switch (c) {

      case 'r':
        // Convert ASCII to Binary
        if (ParseOneParameter(optarg, (u32 *) &address) == FALSE)
          goto InvParm;
        opflags |= MEMVR_MEM_READ;
        break;

      case 'i':
        strncpy(ipAddr, optarg, FDK_MAX_PATH);
        break;

      case 'w':
        // Parse
        if (ParseTwoParameters(optarg, (u32 *) &address, &value) == FALSE)
          goto InvParm;

        opflags |= MEMVR_MEM_WRITE;
        break;

      case 'o':
        // Parse
        if (ParseTwoParameters(optarg, (u32 *) &address, &value) == FALSE)
          goto InvParm;

        opflags |= MEMVR_MEM_OR;
        break;

      case 'a':
        // Parse
        if (ParseTwoParameters(optarg, (u32 *) &address, &value) == FALSE)
          goto InvParm;

        opflags |= MEMVR_MEM_AND;
        break;

      case 'd':
        // Parse
        if (ParseTwoParameters(optarg, (u32 *) &address, &length) == FALSE)
          goto InvParm;

        // Check len
        if (length > MEMVR_MEM_MAXLEN)
          length = MEMVR_MEM_MAXLEN;
        else if (!length)
          goto InvParm;

        opflags |= MEMVR_MEM_DUMP;
        break;

      case 'v':
        verbose = 1;
        quiet = 0;
        break;

      case 'b':
        dbit = 1;
        quiet = 0;
        break;

      case 'q':
        quiet = 1;
        verbose = 0;
        dbit = 0;
        break;

      case 'h':
        help();
        return 0;

      default:
        InvParm: fprintf( stderr, "%s%s\n", ErrorStr, ErrInvParm);
        help();
        return 1;
    }
  }

  // Check flags
  if (!(opflags
      & (MEMVR_MEM_READ | MEMVR_MEM_WRITE | MEMVR_MEM_DUMP | MEMVR_MEM_OR
          | MEMVR_MEM_AND))) {

    help();
    return -1;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Manipulation                                                           //
  ////////////////////////////////////////////////////////////////////////////

  // Create a socket
  if (connectSocket(&sfd, ipAddr, FDK_DEF_PORT)) {

    fprintf( stderr, "Cannot open socket\n");
    return -1;
  }

  // Connect to FDK server
  if (connectToFdkServer(sfd)) {

    ret = -1;
    fprintf( stderr, "Cannot connect to FDK server\n");
    goto ErrExit;
  }

  // Handle Read Memory
  if (opflags & MEMVR_MEM_READ) {

    u32 val;

    if (verbose)
      printf("Read Value from Address: 0x%8.8X\n", (u32) address);

    // Read value
    val = memoryReadDWord(sfd, address);
    if (quiet)
      printf("0x%8.8X\n", val);
    else
      printf("\n[READ] Address = 0x%8.8X, Value = 0x%8.8X\n", (u32) address,
          val);

    if (dbit)
      DisplayInBits(val);

    goto NorExit;
  }

  // Handle Write Memory
  if (opflags & (MEMVR_MEM_WRITE | MEMVR_MEM_OR | MEMVR_MEM_AND)) {

    u32 orig, res;
    s8 *opstr = WriteStr;

    if (verbose)
      printf("Write Value: 0x%8.8X to Address: 0x%8.8X\n", value,
          (u32) address);

    // Read original value
    orig = memoryReadDWord(sfd, address);

    // Do operation
    if (opflags & MEMVR_MEM_WRITE)
      memoryWriteDWord(sfd, address, value);
    else if (opflags & MEMVR_MEM_OR) {

      memoryORDWord(sfd, address, value);
      opstr = OrStr;
    } else if (opflags & MEMVR_MEM_AND) {

      memoryANDDWord(sfd, address, value);
      opstr = AndStr;
    }

    // Read result
    res = memoryReadDWord(sfd, address);

    if (quiet)
      printf("0x%8.8X\n", res);
    else {

      printf("\n[%s] Address = 0x%8.8X, Value = 0x%8.8X\n", opstr,
          (u32) address, value);
      printf("[%s] Origin  = 0x%8.8X, Result= 0x%8.8X\n\n", opstr, orig, res);
    }

    if ((value != res) && !quiet && (opflags & MEMVR_MEM_WRITE))
      printf("[WARNING] Value and Result are different\n\n");

    if (dbit) {

      printf("[Origin]");
      DisplayInBits(orig);

      printf("[Value]");
      DisplayInBits(value);

      printf("[Result]");
      DisplayInBits(res);
    }

    goto NorExit;
  }

  // Handle Dump Memory
  if (opflags & MEMVR_MEM_DUMP) {

    if (verbose)
      printf("Dump Memory start from Address: 0x%8.8X, len: 0x%8.8X\n",
          (u32) address, length);

    printf("\n[DUMP MEMORY] Address = 0x%8.8X, len = 0x%8.8X\n", (u32) address,
        length);

    // Dump Data
    dumpMemData(sfd, length, address);

    goto NorExit;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Release resource                                                       //
  ////////////////////////////////////////////////////////////////////////////
  NorExit: disconnectFromFdkServer(sfd);

  ErrExit:
  // Close socket
  deinitializeSocket(sfd);

  return ret;
}

