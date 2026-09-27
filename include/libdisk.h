/*
 * FDK - Firmware Debug Kit
 * File: libdisk.h
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

#ifndef FDK_INCLUDE_LIBDISK_H_
#define FDK_INCLUDE_LIBDISK_H_

#include "mtypes.h"

// Stores the path of the first physical whole disk listed in /sys/block
// (e.g. /dev/nvme0n1, /dev/sda, /dev/vda) in |path|. Returns 0 on success.
s32 diskFindDefault(s8 *path, u32 len);

// Reads/writes |len| bytes at byte offset |addr| of block device |dev|.
// Failed reads are returned as 0xFF. Return 0 on success.
s32 diskRead(const s8 *dev, u64 addr, u32 len, u8 *buf);
s32 diskWrite(const s8 *dev, u64 addr, u32 len, const u8 *buf);

#endif  // FDK_INCLUDE_LIBDISK_H_
