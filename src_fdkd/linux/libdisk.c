/*
 * FDK - Firmware Debug Kit
 * File: libdisk.c
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

#include "libdisk.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fdk.h"

#define FDK_SYSFS_BLOCK "/sys/block"

s32 diskFindDefault(s8 *path, u32 len) {
  DIR *dir;
  struct dirent *ent;
  s8 best[sizeof(ent->d_name)] = "";

  dir = opendir(FDK_SYSFS_BLOCK);
  if (!dir) return -1;

  while ((ent = readdir(dir))) {
    s8 link[2 * FDK_MAX_PATH];

    if (ent->d_name[0] == '.') continue;

    // Virtual devices (loop, zram, dm-*, md*, ...) have no "device" link.
    snprintf(link, sizeof(link), FDK_SYSFS_BLOCK "/%s/device", ent->d_name);
    if (access(link, F_OK)) continue;

    // Skip optical and floppy drives.
    if (!strncmp(ent->d_name, "sr", 2) || !strncmp(ent->d_name, "fd", 2)) {
      continue;
    }

    if (!best[0] || strcmp(ent->d_name, best) < 0) {
      snprintf(best, sizeof(best), "%s", ent->d_name);
    }
  }
  closedir(dir);

  if (!best[0]) return -1;
  snprintf(path, len, "/dev/%s", best);
  return 0;
}

s32 diskRead(const s8 *dev, u64 addr, u32 len, u8 *buf) {
  ssize_t n = -1;
  s32 fd;

  memset(buf, 0xFF, len);
  if (!dev || !len) return -1;

  fd = open(dev, O_RDONLY | O_CLOEXEC);
  if (fd < 0) return -1;
  n = pread(fd, buf, len, (off_t)addr);
  close(fd);

  return n == (ssize_t)len ? 0 : -1;
}

s32 diskWrite(const s8 *dev, u64 addr, u32 len, const u8 *buf) {
  ssize_t n;
  s32 fd;

  if (!dev || !len) return -1;

  // Kernels built without CONFIG_BLK_DEV_WRITE_MOUNTED refuse to open a
  // mounted block device for writing (EBUSY).
  fd = open(dev, O_WRONLY | O_CLOEXEC);
  if (fd < 0) return -1;
  n = pwrite(fd, buf, len, (off_t)addr);
  if (n == (ssize_t)len && fsync(fd)) n = -1;
  close(fd);

  return n == (ssize_t)len ? 0 : -1;
}
