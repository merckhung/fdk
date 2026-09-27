/*
 * FDK - Firmware Debug Kit
 * File: libport.c
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

#include "libport.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#if defined(__i386__) || defined(__x86_64__)
#include <sys/io.h>
#define FDK_HAVE_PORT_INSN 1
#endif

#define FDK_PORT_SPACE 0x10000U

// How a range of ports is being accessed: through /dev/port (fd >= 0) or
// with in/out instructions after ioperm() (insn).
typedef struct {
  s32 fd;
  bool insn;
  u16 port;
  u32 len;
} portHandle_t;

static s32 portOpen(u16 port, u32 len, portHandle_t *h) {
  if ((u32)port + len > FDK_PORT_SPACE) return -1;

  h->port = port;
  h->len = len;
  h->insn = false;
  h->fd = open(FDK_PORT_DEV, O_RDWR | O_CLOEXEC);
  if (h->fd >= 0) return 0;

#ifdef FDK_HAVE_PORT_INSN
  // I/O permission bitmaps are per thread, so request them on every use.
  if (!ioperm(port, len, 1)) {
    h->insn = true;
    return 0;
  }
#endif
  return -1;
}

static void portClose(portHandle_t *h) {
  if (h->fd >= 0) close(h->fd);
#ifdef FDK_HAVE_PORT_INSN
  if (h->insn) ioperm(h->port, h->len, 0);
#endif
}

// Transfers one byte at |port|. Returns 0 on success.
static s32 portXfer(const portHandle_t *h, u16 port, u8 *val, bool isWrite) {
  if (h->fd >= 0) {
    ssize_t n =
        isWrite ? pwrite(h->fd, val, 1, port) : pread(h->fd, val, 1, port);
    return n == 1 ? 0 : -1;
  }
#ifdef FDK_HAVE_PORT_INSN
  if (isWrite) {
    outb(*val, port);
  } else {
    *val = inb(port);
  }
  return 0;
#else
  return -1;
#endif
}

static s32 portAccess(u16 port, u32 len, u8 *buf, bool isWrite) {
  portHandle_t h;
  s32 ret = 0;
  u32 i;

  if (portOpen(port, len, &h)) return -1;
  for (i = 0; i < len && !ret; i++) {
    ret = portXfer(&h, (u16)(port + i), &buf[i], isWrite);
  }
  portClose(&h);
  return ret;
}

s32 portRead(u16 port, u32 len, u8 *buf) {
  if (!portAccess(port, len, buf, false)) return 0;
  memset(buf, 0xFF, len);
  return -1;
}

s32 portWrite(u16 port, u32 len, const u8 *buf) {
  return portAccess(port, len, (u8 *)buf, true);
}

static s32 cmosAccess(u8 addr, u32 len, u8 *buf, bool isWrite) {
  portHandle_t h;
  s32 ret = 0;
  u32 i;

  if ((u32)addr + len > FDK_CMOS_SIZE) return -1;
  if (portOpen(FDK_CMOS_ADDR, FDK_CMOS_EXT_DATA - FDK_CMOS_ADDR + 1, &h)) {
    return -1;
  }

  for (i = 0; i < len && !ret; i++) {
    const u32 off = addr + i;
    const u16 idxPort =
        off < FDK_CMOS_BANK_SIZE ? FDK_CMOS_ADDR : FDK_CMOS_EXT_ADDR;
    // Bit 7 of port 70h masks NMI; always leave NMI enabled.
    u8 idx = off & (FDK_CMOS_BANK_SIZE - 1);

    ret = portXfer(&h, idxPort, &idx, true);
    if (!ret) ret = portXfer(&h, idxPort + 1, &buf[i], isWrite);
  }

  portClose(&h);
  return ret;
}

s32 cmosRead(u8 addr, u32 len, u8 *buf) {
  if (!cmosAccess(addr, len, buf, false)) return 0;
  memset(buf, 0xFF, len);
  return -1;
}

s32 cmosWrite(u8 addr, u32 len, const u8 *buf) {
  return cmosAccess(addr, len, (u8 *)buf, true);
}
