/*
 * FDK - Firmware Debug Kit
 * File: mtypes.h
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

typedef     unsigned char           u8;
typedef     unsigned short int      u16;
typedef     unsigned int            u32;
typedef     unsigned long long int  u64;

typedef     char                    s8;
typedef     short int               s16;
typedef     int                     s32;
typedef     long long int           s64;


#ifndef bool
typedef     unsigned char           bool;
#endif


#define     TRUE                    1
#define     FALSE                   0


#define		PACKED					__attribute__((packed))


