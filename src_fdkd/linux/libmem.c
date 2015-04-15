/*
 * FDK - Firmware Debug Kit
 * File: libmem.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "mtypes.h"
#include "fdk.h"
#include "libmem.h"

int32_t openMemDev(void) {
  return open(FDK_MEM_DEV, O_RDWR);
}

void closeMemDev(int32_t fd) {
  close(fd);
}

volatile void *memMapping(int32_t fd, uint64_t addr, uint32_t len, uint64_t *alignOff, uint32_t *actLen) {
  uint64_t alignAddr;

  // Must align 4kb boundary
  alignAddr = addr & 0xFFFFFFFFFFFFE000ULL;
  *alignOff = addr - alignAddr;

  // Calculate maping size
  if (len < FDK_4K_PAGE)
    len = FDK_4K_PAGE;
  else if (len % FDK_4K_PAGE)
    len = ((len + FDK_4K_PAGE) / FDK_4K_PAGE) * FDK_4K_PAGE;

  // Aligned size plus 4kb
  len += FDK_4K_PAGE;
  *actLen = len;

  // Map Memory
  return mmap( NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, alignAddr);
}

int32_t memUnmapping(void *phyMem, uint32_t len) {
  return munmap(phyMem, len);
}

uint8_t memReadByte(int32_t fd, uint64_t addr) {
  volatile uint8_t *phyMem;
  uint8_t val;
  uint64_t alignOff;
  uint32_t actLen;

  // Map memory
  phyMem = (volatile uint8_t *) memMapping(fd, addr, FDK_4K_PAGE, &alignOff,
      &actLen);
  if (!phyMem)
    return 0xFF;

  // Write val
  val = *(phyMem + alignOff);

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return val;
}

uint16_t memReadWord(int32_t fd, uint64_t addr) {
  volatile uint16_t *phyMem;
  uint16_t val;
  uint64_t alignOff;
  uint32_t actLen;

  // Map memory
  phyMem = (volatile uint16_t *) memMapping(fd, addr, FDK_4K_PAGE, &alignOff,
      &actLen);
  if (!phyMem)
    return 0xFFFF;

  // Write val
  val = *(phyMem + (alignOff / 2));

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return val;
}

uint32_t memReadDWord(int32_t fd, uint64_t addr) {
  volatile uint32_t *phyMem;
  uint64_t alignOff;
  uint32_t actLen, val;

  // Map memory
  phyMem = (volatile uint32_t *) memMapping(fd, addr, FDK_4K_PAGE, &alignOff,
      &actLen);
  if (!phyMem)
    return 0xFFFFFFFF;

  // Write val
  val = *(phyMem + (alignOff / 4));

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return val;
}

uint32_t memReadBuffer(int32_t fd, uint64_t addr, uint32_t len, uint8_t *buf) {
  volatile uint8_t *phyMem;
  uint64_t alignOff;
  uint32_t actLen, i;

  // Map memory
  phyMem = (volatile uint8_t *) memMapping(fd, addr, len, &alignOff, &actLen);
  if (!phyMem)
    return 0xFFFFFFFF;

  // Write val
  for (i = 0; i < len; i++)
    *(buf + i) = *(phyMem + alignOff + i);

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return 0;
}

uint32_t memWriteBuffer(int32_t fd, uint64_t addr, uint32_t len, uint8_t *buf) {
  volatile uint8_t *phyMem;
  uint64_t alignOff;
  uint32_t actLen, i;

  // Map memory
  phyMem = (volatile uint8_t *) memMapping(fd, addr, len, &alignOff, &actLen);
  if (!phyMem)
    return 0xFFFFFFFF;

  // Write val
  for (i = 0; i < len; i++)
    *(phyMem + alignOff + i) = *(buf + i);

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return 0;
}

uint8_t memWriteByte(int32_t fd, uint64_t addr, uint8_t val) {
  volatile uint8_t *phyMem;
  uint64_t alignOff;
  uint32_t actLen;
  uint8_t tmp;

  // Map memory
  phyMem = (volatile uint8_t *) memMapping(fd, addr, FDK_4K_PAGE, &alignOff,
      &actLen);
  if (!phyMem)
    return 0xFF;

  // Write val
  *(phyMem + alignOff) = val;
  tmp = *(phyMem + alignOff);

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return tmp;
}

uint16_t memWriteWord(int32_t fd, uint64_t addr, uint16_t val) {
  volatile uint16_t *phyMem;
  uint64_t alignOff;
  uint32_t actLen;
  uint16_t tmp;

  // Map memory
  phyMem = (volatile uint16_t *) memMapping(fd, addr, FDK_4K_PAGE, &alignOff,
      &actLen);
  if (!phyMem)
    return 0xFFFF;

  // Write val
  *(phyMem + (alignOff / 2)) = val;
  tmp = *(phyMem + (alignOff / 2));

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return tmp;
}

uint32_t memWriteDWord(int32_t fd, uint64_t addr, uint32_t val) {
  volatile uint32_t *phyMem;
  uint64_t alignOff;
  uint32_t actLen, tmp;

  // Map memory
  phyMem = (volatile uint32_t *) memMapping(fd, addr, FDK_4K_PAGE, &alignOff,
      &actLen);
  if (!phyMem)
    return 0xFFFFFFFF;

  // Write val
  *(phyMem + (alignOff / 4)) = val;
  tmp = *(phyMem + (alignOff / 4));

  // Unmap memory
  memUnmapping((void *) phyMem, actLen);

  return tmp;
}

