/*
 * FDK - Firmware Debug Kit
 * File: libcomm.h
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

#ifndef FDK_INCLUDE_LIBCOMM_H_
#define FDK_INCLUDE_LIBCOMM_H_

#include "mtypes.h"

#define FDK_GET_BIT(val, bit) (((val) >> (bit)) & 1)

// Parses "0x<hex>" into |first|.
bool ParseOneParameter(const s8 *buf, u64 *first);

// Parses "0x<hex>/0x<hex>" into |first| and |second|.
bool ParseTwoParameters(const s8 *buf, u64 *first, u64 *second);

// Prints a 32-bit value as a bit table.
void DisplayInBits(u32 value);

#endif  // FDK_INCLUDE_LIBCOMM_H_
