/*
 * FDK - Firmware Debug Kit
 * File: memvr.h
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

#ifndef FDK_INCLUDE_MEMVR_H_
#define FDK_INCLUDE_MEMVR_H_

#include "mtypes.h"

#define MEMVR_VERSION "1.1.0"
#define MEMVR_MEM_READ 0x01
#define MEMVR_MEM_WRITE 0x02
#define MEMVR_MEM_OR 0x04
#define MEMVR_MEM_AND 0x08
#define MEMVR_MEM_DUMP 0x10

#define MEMVR_MEM_MAXLEN 0x1000

s32 connectToFdkServer(s32 sfd);
void disconnectFromFdkServer(s32 sfd);

// Return 0 on success.
s32 memoryReadDWord(s32 sfd, u64 address, u32 *value);
s32 memoryWriteDWord(s32 sfd, u64 address, u32 value);
s32 memoryReadBuffer(s32 sfd, u64 address, u32 len, u8 *buf);
s32 memoryORDWord(s32 sfd, u64 address, u32 value);
s32 memoryANDDWord(s32 sfd, u64 address, u32 value);

#endif  // FDK_INCLUDE_MEMVR_H_
