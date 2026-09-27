/*
 * FDK - Firmware Debug Kit
 * File: libpci.c
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

#include "libpci.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fdk.h"

#if defined(__i386__) || defined(__x86_64__)
#include <sys/io.h>
#define FDK_HAVE_PORT_INSN 1
#endif

#define PCI_LEGACY_ADDR_PORT 0xCF8
#define PCI_LEGACY_DATA_PORT 0xCFC
#define PCI_LEGACY_NPORTS 8

#define PCI_SEG(a) (((a) >> PCI_SEG_OFFSET) & PCI_SEG_MASK)
#define PCI_BUS(a) (((a) >> PCI_BUS_OFFSET) & PCI_BUS_MASK)
#define PCI_DEV(a) (((a) >> PCI_DEV_OFFSET) & PCI_DEV_MASK)
#define PCI_FUNC(a) (((a) >> PCI_FUNC_OFFSET) & PCI_FUNC_MASK)
#define PCI_REG(a) ((a) & PCI_REG_MASK)

static bool pciHaveSysfs(void) { return !access(PCI_SYSFS_DEVICES, R_OK); }

static void pciSysfsConfigPath(u32 addr, s8 *path, size_t len) {
  snprintf(path, len, PCI_SYSFS_DEVICES "/%04x:%02x:%02x.%x/config",
           PCI_SEG(addr), PCI_BUS(addr), PCI_DEV(addr), PCI_FUNC(addr));
}

static s32 pciComparePciDev(const void *a, const void *b) {
  const fdkPciDev_t *x = a, *y = b;
  const u32 kx = ((u32)x->bus << 8) | (x->dev << 3) | x->fun;
  const u32 ky = ((u32)y->bus << 8) | (y->dev << 3) | y->fun;
  return (kx > ky) - (kx < ky);
}

static u32 pciListSysfs(fdkPciDev_t *pFdkPciDev, u32 max) {
  DIR *dir;
  struct dirent *ent;
  u32 count = 0;

  dir = opendir(PCI_SYSFS_DEVICES);
  if (!dir) return 0;

  while (count < max && (ent = readdir(dir))) {
    u32 seg, bus, dev, fun;
    u16 ids[2];
    s8 path[2 * FDK_MAX_PATH];
    s32 fd;

    // Entries are named SSSS:BB:DD.F.
    if (sscanf(ent->d_name, "%x:%x:%x.%x", &seg, &bus, &dev, &fun) != 4) {
      continue;
    }
    if (seg > PCI_SEG_MASK || bus > PCI_BUS_MASK || dev > PCI_DEV_MASK ||
        fun > PCI_FUNC_MASK) {
      continue;
    }

    snprintf(path, sizeof(path), PCI_SYSFS_DEVICES "/%s/config", ent->d_name);
    fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) continue;
    if (pread(fd, ids, sizeof(ids), 0) != sizeof(ids)) {
      close(fd);
      continue;
    }
    close(fd);

    pFdkPciDev[count].bus = (u16)((seg << 8) | bus);
    pFdkPciDev[count].dev = (u8)dev;
    pFdkPciDev[count].fun = (u8)fun;
    pFdkPciDev[count].vendorId = ids[0];
    pFdkPciDev[count].deviceId = ids[1];
    count++;
  }
  closedir(dir);

  qsort(pFdkPciDev, count, sizeof(*pFdkPciDev), pciComparePciDev);
  return count;
}

static s32 pciSysfsAccess(u32 addr, u32 len, u8 *buf, bool isWrite) {
  s8 path[FDK_MAX_PATH];
  ssize_t n;
  s32 fd;

  pciSysfsConfigPath(addr, path, sizeof(path));
  fd = open(path, (isWrite ? O_WRONLY : O_RDONLY) | O_CLOEXEC);
  if (fd < 0) return -1;

  n = isWrite ? pwrite(fd, buf, len, PCI_REG(addr))
              : pread(fd, buf, len, PCI_REG(addr));
  close(fd);

  return n == (ssize_t)len ? 0 : -1;
}

#ifdef FDK_HAVE_PORT_INSN
// Legacy configuration mechanism #1 (ports CF8h/CFCh), used only when sysfs
// is unavailable. It reaches segment 0 and the first 256 bytes only, and
// is not serialized against the kernel's own configuration accesses.
static s32 pciLegacyAccess(u32 addr, u32 len, u8 *buf, bool isWrite) {
  u32 i;

  if (PCI_SEG(addr) || ioperm(PCI_LEGACY_ADDR_PORT, PCI_LEGACY_NPORTS, 1)) {
    return -1;
  }

  for (i = 0; i < len && PCI_REG(addr) + i <= PCI_REG_MASK; i++) {
    const u32 reg = addr + i;
    const u16 port = PCI_LEGACY_DATA_PORT + (reg & 3);

    outl(PCI_ADDR_ENABLE | (reg & 0x00FFFFFC), PCI_LEGACY_ADDR_PORT);
    if (isWrite) {
      outb(buf[i], port);
    } else {
      buf[i] = inb(port);
    }
  }

  ioperm(PCI_LEGACY_ADDR_PORT, PCI_LEGACY_NPORTS, 0);
  return i == len ? 0 : -1;
}

static u32 pciListLegacy(fdkPciDev_t *pFdkPciDev, u32 max) {
  u32 bus, dev, fun, count = 0;
  u16 ids[2];

  for (bus = 0; bus <= PCI_BUS_MASK; bus++) {
    for (dev = 0; dev <= PCI_DEV_MASK; dev++) {
      for (fun = 0; fun <= PCI_FUNC_MASK && count < max; fun++) {
        const u32 addr = PCI_ADDR_ENABLE | (bus << PCI_BUS_OFFSET) |
                         (dev << PCI_DEV_OFFSET) | (fun << PCI_FUNC_OFFSET);

        if (pciLegacyAccess(addr, sizeof(ids), (u8 *)ids, false)) return count;
        if (ids[0] == 0xFFFF) continue;

        pFdkPciDev[count].bus = (u16)bus;
        pFdkPciDev[count].dev = (u8)dev;
        pFdkPciDev[count].fun = (u8)fun;
        pFdkPciDev[count].vendorId = ids[0];
        pFdkPciDev[count].deviceId = ids[1];
        count++;
      }
    }
  }
  return count;
}
#endif  // FDK_HAVE_PORT_INSN

u32 pciListDevices(fdkPciDev_t *pFdkPciDev, u32 max) {
  if (pciHaveSysfs()) return pciListSysfs(pFdkPciDev, max);
#ifdef FDK_HAVE_PORT_INSN
  return pciListLegacy(pFdkPciDev, max);
#else
  return 0;
#endif
}

static s32 pciAccess(u32 addr, u32 len, u8 *buf, bool isWrite) {
  if (pciHaveSysfs()) return pciSysfsAccess(addr, len, buf, isWrite);
#ifdef FDK_HAVE_PORT_INSN
  return pciLegacyAccess(addr, len, buf, isWrite);
#else
  return -1;
#endif
}

s32 pciReadConfig(u32 addr, u32 len, u8 *buf) {
  // Whatever cannot be read (e.g. past the end of a conventional PCI
  // device's 256-byte space) stays 0xFF, like a master abort.
  memset(buf, 0xFF, len);
  return pciAccess(addr, len, buf, false);
}

s32 pciWriteConfig(u32 addr, u32 len, const u8 *buf) {
  return pciAccess(addr, len, (u8 *)buf, true);
}
