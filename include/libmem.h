/*
 * FDK - Firmware Debug Kit
 * File: libmem.h
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

#define FDK_MEM_MAXLEN          0x1000
#define FDK_4K_PAGE             0x1000
#define FDK_MAX_FILENAME        40
#define FDK_MAX_PATHNAME        100
#define FDK_MEM_DEV             "/dev/mem"


inline s32 openMemDev( void );
inline void closeMemDev( s32 fd );
volatile void *memMapping( s32 fd, u64 addr, u32 len, u64 *alignOff, u32 *actLen );
s32 memUnmapping( void *phyMem, u32 len );
u8 memReadByte( s32 fd, u64 addr );
u16 memReadWord( s32 fd, u64 addr );
u32 memReadDWord( s32 fd, u64 addr );
u32 memReadBuffer( s32 fd, u64 addr, u32 len, u8 *buf );
u32 memWriteBuffer( s32 fd, u64 addr, u32 len, u8 *buf );
u8 memWriteByte( s32 fd, u64 addr, u8 val );
u16 memWriteWord( s32 fd, u64 addr, u16 val );
u32 memWriteDWord( s32 fd, u64 addr, u32 val );


