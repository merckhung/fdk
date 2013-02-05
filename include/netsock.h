/*
 * FDK - Firmware Debug Kit
 * File: netsock.h
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

s32 initializeSocket( s32 *fd, s8 *addr, s32 port );
s32 connectSocket( s32 *fd, s8 *addr, s32 port );
void deinitializeSocket( s32 fd );
s32 acceptSocket( s32 fd, s32 *apsd );
s32 transferSocket( s32 fd, const void *pktBuf, const u32 length, u32 *wByte );
s32 receiveSocket( s32 fd, void *pktBuf, const u32 length, u32 *rByte );


