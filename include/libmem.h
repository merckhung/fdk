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

#include <stdint.h>

#define FDK_MEM_MAXLEN          0x1000
#define FDK_4K_PAGE             0x1000
#define FDK_MAX_FILENAME        40
#define FDK_MAX_PATHNAME        100
#define FDK_MEM_DEV             "/dev/mem"

int32_t openMemDev(void);
void closeMemDev(int32_t fd);
volatile void *memMapping(int32_t fd, uint64_t addr, uint32_t len, uint64_t *alignOff, uint32_t *actLen);
int32_t memUnmapping(void *phyMem, uint32_t len);
uint8_t memReadByte(int32_t fd, uint64_t addr);
uint16_t memReadWord(int32_t fd, uint64_t addr);
uint32_t memReadDWord(int32_t fd, uint64_t addr);
uint32_t memReadBuffer(int32_t fd, uint64_t addr, uint32_t len, uint8_t *buf);
uint32_t memWriteBuffer(int32_t fd, uint64_t addr, uint32_t len, uint8_t *buf);
uint8_t memWriteByte(int32_t fd, uint64_t addr, uint8_t val);
uint16_t memWriteWord(int32_t fd, uint64_t addr, uint16_t val);
uint32_t memWriteDWord(int32_t fd, uint64_t addr, uint32_t val);

