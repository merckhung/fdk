/*
 * FDK - Firmware Debug Kit
 * File: libmem.h
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

#ifndef FDK_INCLUDE_LIBMEM_H_
#define FDK_INCLUDE_LIBMEM_H_

#include "mtypes.h"

#define FDK_MEM_DEV "/dev/mem"

s32 openMemDev(void);
void closeMemDev(s32 fd);

// Copies |len| bytes of physical memory at |addr| into |buf|. Returns 0 on
// success; on failure |buf| is filled with 0xFF and -1 is returned.
s32 memReadBuffer(s32 fd, u64 addr, u32 len, u8 *buf);

// Copies |len| bytes from |buf| to physical memory at |addr|. Returns 0 on
// success.
s32 memWriteBuffer(s32 fd, u64 addr, u32 len, const u8 *buf);

// Single, width-exact accesses (suitable for MMIO registers). The write
// helpers return the value read back after the write; all return all-ones
// on failure.
u8 memReadByte(s32 fd, u64 addr);
u16 memReadWord(s32 fd, u64 addr);
u32 memReadDWord(s32 fd, u64 addr);
u8 memWriteByte(s32 fd, u64 addr, u8 val);
u16 memWriteWord(s32 fd, u64 addr, u16 val);
u32 memWriteDWord(s32 fd, u64 addr, u32 val);

#endif  // FDK_INCLUDE_LIBMEM_H_
