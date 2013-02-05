/*
 * FDK - Firmware Debug Kit
 * File: memvr.h
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

#define MEMVR_VERSION               "1.0.0"
#define MEMVR_MEM_READ              0x01
#define MEMVR_MEM_WRITE             0x02
#define MEMVR_MEM_OR                0x04
#define MEMVR_MEM_AND               0x08
#define MEMVR_MEM_DUMP              0x10

#define MEMVR_MEM_MAXLEN			0x1000
#define MEMVR_GET_BIT( val, bit )	((val & (1 << bit)) >> bit)
#define MEMVR_BIT_MASK( x )			(1 << x)
#define MEMVR_ARRAY_NRCELL( x )		(sizeof(x)/sizeof(x[0]))
#define MEMVR_ENUM_TOSTR( NAME )	#NAME


s32 connectToFdkServer( s32 sfd );
void disconnectFromFdkServer( s32 sfd );
u32 memoryReadDWord( s32 sfd, u64 address );
s32 memoryWriteDWord( s32 sfd, u64 address, u32 value );
u8 memoryReadByte( s32 sfd, u64 address );
s32 memoryWriteByte( s32 sfd, u64 address, u8 value );
void memoryORDWord( s32 sfd, u32 address, u32 value );
void memoryANDDWord( s32 sfd, u32 address, u32 value );
void memoryCOMANDDWord( s32 sfd, u32 address, u32 value );


