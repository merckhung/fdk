/*
 * FDK - Firmware Debug Kit
 * File: libport.h
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

#ifndef FDK_INCLUDE_LIBPORT_H_
#define FDK_INCLUDE_LIBPORT_H_

#include "mtypes.h"

#define FDK_PORT_DEV "/dev/port"

#define FDK_CMOS_ADDR 0x70
#define FDK_CMOS_DATA 0x71
#define FDK_CMOS_EXT_ADDR 0x72
#define FDK_CMOS_EXT_DATA 0x73
#define FDK_CMOS_BANK_SIZE 0x80
#define FDK_CMOS_SIZE 0x100

// Byte-wise CPU I/O port access. Uses /dev/port when present and falls back
// to ioperm() + in/out instructions on x86. Return 0 on success; failed
// reads are returned as 0xFF.
s32 portRead(u16 port, u32 len, u8 *buf);
s32 portWrite(u16 port, u32 len, const u8 *buf);

// CMOS/RTC NVRAM access. Offsets 00h-7Fh use ports 70h/71h and offsets
// 80h-FFh the extended bank at 72h/73h.
s32 cmosRead(u8 addr, u32 len, u8 *buf);
s32 cmosWrite(u8 addr, u32 len, const u8 *buf);

#endif  // FDK_INCLUDE_LIBPORT_H_
