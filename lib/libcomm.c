/*
 * FDK - Firmware Debug Kit
 * File: libcomm.c
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

#include "libcomm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parses one "0x<hex>" token of at most 16 digits ending at |end| (or at
// the end of the string when |end| is NULL).
static bool parseHex(const s8 *buf, const s8 *end, u64 *val) {
  s8 *stop;
  size_t len = end ? (size_t)(end - buf) : strlen(buf);

  if (len < 3 || len > 18 || buf[0] != '0' || (buf[1] | 0x20) != 'x') {
    return FALSE;
  }

  *val = strtoull(buf + 2, &stop, 16);
  return stop == buf + len;
}

bool ParseOneParameter(const s8 *buf, u64 *first) {
  return parseHex(buf, NULL, first);
}

bool ParseTwoParameters(const s8 *buf, u64 *first, u64 *second) {
  const s8 *sep = strchr(buf, '/');

  if (!sep) return FALSE;
  return parseHex(buf, sep, first) && parseHex(sep + 1, NULL, second);
}

void DisplayInBits(u32 value) {
  s32 bit;

  printf(
      "\n=================================================================="
      "=============================\n");
  printf(
      "31 30 29 28 27 26 25 24|23 22 21 20 19 18 17 16|15 14 13 12 11 10 09 "
      "08|07 06 05 04 03 02 01 00\n");
  printf(
      "------------------------------------------------------------------"
      "-----------------------------\n");
  for (bit = 31; bit >= 0; bit--) {
    printf(" %u%s", FDK_GET_BIT(value, bit),
           !bit ? "\n" : (bit % 8 ? " " : "|"));
  }
  printf(
      "=================================================================="
      "=============================\n\n");
}
