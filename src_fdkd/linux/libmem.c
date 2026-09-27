/*
 * FDK - Firmware Debug Kit
 * File: libmem.c
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

#include "libmem.h"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
  volatile u8 *map;  // Start of the mapping.
  size_t mapLen;     // Length of the mapping.
  volatile u8 *ptr;  // Requested address inside the mapping.
} memMapping_t;

s32 openMemDev(void) {
  // O_SYNC makes the kernel map MMIO ranges uncached.
  return open(FDK_MEM_DEV, O_RDWR | O_SYNC | O_CLOEXEC);
}

void closeMemDev(s32 fd) {
  if (fd >= 0) close(fd);
}

// Maps the pages covering [addr, addr + len). Returns 0 on success.
static s32 mapRange(s32 fd, u64 addr, u32 len, memMapping_t *m) {
  const u64 pageSize = (u64)sysconf(_SC_PAGESIZE);
  const u64 base = addr & ~(pageSize - 1);
  const u64 end = (addr + len + pageSize - 1) & ~(pageSize - 1);
  void *p;

  if (fd < 0 || !len) return -1;

  p = mmap(NULL, end - base, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
           (off_t)base);
  if (p == MAP_FAILED) return -1;

  m->map = p;
  m->mapLen = end - base;
  m->ptr = m->map + (addr - base);
  return 0;
}

static void unmapRange(memMapping_t *m) { munmap((void *)m->map, m->mapLen); }

s32 memReadBuffer(s32 fd, u64 addr, u32 len, u8 *buf) {
  memMapping_t m;
  u32 i;

  if (!mapRange(fd, addr, len, &m)) {
    for (i = 0; i < len; i++) buf[i] = m.ptr[i];
    unmapRange(&m);
    return 0;
  }

  // CONFIG_STRICT_DEVMEM refuses to mmap() RAM, while read() still works
  // for the ranges the kernel permits (it returns zeros for the rest).
  if (fd >= 0 && pread(fd, buf, len, (off_t)addr) == (ssize_t)len) return 0;

  memset(buf, 0xFF, len);
  return -1;
}

s32 memWriteBuffer(s32 fd, u64 addr, u32 len, const u8 *buf) {
  memMapping_t m;
  u32 i;

  if (!mapRange(fd, addr, len, &m)) {
    for (i = 0; i < len; i++) m.ptr[i] = buf[i];
    unmapRange(&m);
    return 0;
  }

  if (fd >= 0 && pwrite(fd, buf, len, (off_t)addr) == (ssize_t)len) return 0;
  return -1;
}

// Performs one naturally sized access of |width| bytes at |addr|. When
// |write| is set, |*val| is written first; the value read back is returned
// in |*val|. Returns 0 on success.
static s32 accessMem(s32 fd, u64 addr, u32 width, bool write, u32 *val) {
  memMapping_t m;

  if (mapRange(fd, addr, width, &m)) return -1;

  switch (width) {
    case sizeof(u8):
      if (write) *(volatile u8 *)m.ptr = (u8)*val;
      *val = *(volatile u8 *)m.ptr;
      break;
    case sizeof(u16):
      if (write) *(volatile u16 *)m.ptr = (u16)*val;
      *val = *(volatile u16 *)m.ptr;
      break;
    default:
      if (write) *(volatile u32 *)m.ptr = *val;
      *val = *(volatile u32 *)m.ptr;
      break;
  }

  unmapRange(&m);
  return 0;
}

u8 memReadByte(s32 fd, u64 addr) {
  u32 val = 0;
  return accessMem(fd, addr, sizeof(u8), false, &val) ? 0xFF : (u8)val;
}

u16 memReadWord(s32 fd, u64 addr) {
  u32 val = 0;
  return accessMem(fd, addr, sizeof(u16), false, &val) ? 0xFFFF : (u16)val;
}

u32 memReadDWord(s32 fd, u64 addr) {
  u32 val = 0;
  return accessMem(fd, addr, sizeof(u32), false, &val) ? 0xFFFFFFFF : val;
}

u8 memWriteByte(s32 fd, u64 addr, u8 val) {
  u32 tmp = val;
  return accessMem(fd, addr, sizeof(u8), true, &tmp) ? 0xFF : (u8)tmp;
}

u16 memWriteWord(s32 fd, u64 addr, u16 val) {
  u32 tmp = val;
  return accessMem(fd, addr, sizeof(u16), true, &tmp) ? 0xFFFF : (u16)tmp;
}

u32 memWriteDWord(s32 fd, u64 addr, u32 val) {
  u32 tmp = val;
  return accessMem(fd, addr, sizeof(u32), true, &tmp) ? 0xFFFFFFFF : tmp;
}
