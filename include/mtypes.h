/*
 * FDK - Firmware Debug Kit
 * File: mtypes.h
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

#ifndef FDK_INCLUDE_MTYPES_H_
#define FDK_INCLUDE_MTYPES_H_

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// s8 is plain char: it is used for text buffers throughout the code base.
typedef char s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// ncurses defines these too.
#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

#define PACKED __attribute__((packed))

#endif  // FDK_INCLUDE_MTYPES_H_
