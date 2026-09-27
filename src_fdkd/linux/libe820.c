/*
 * FDK - Firmware Debug Kit
 * File: libe820.c
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

#include "libe820.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdk.h"

static const struct {
  const s8 *name;
  u32 type;
} kE820Types[] = {
    {"System RAM", FDK_E820_RAM},
    {"Reserved", FDK_E820_RESERVED},
    {"ACPI Tables", FDK_E820_ACPI},
    {"ACPI Non-volatile Storage", FDK_E820_NVS},
    {"Unusable memory", FDK_E820_UNUSABLE},
    {"Persistent Memory", FDK_E820_PMEM},
    {"Soft Reserved", FDK_E820_SOFT_RESERVED},
};

// Reads the first line of /sys/firmware/memmap/<entry>/<attr>.
static s32 readAttr(const s8 *entry, const s8 *attr, s8 *buf, size_t len) {
  s8 path[FDK_MAX_PATH];
  FILE *fp;

  snprintf(path, sizeof(path), FDK_FIRMWARE_MEMMAP "/%s/%s", entry, attr);
  fp = fopen(path, "re");
  if (!fp) return -1;
  if (!fgets(buf, (int)len, fp)) {
    fclose(fp);
    return -1;
  }
  fclose(fp);
  buf[strcspn(buf, "\n")] = 0;
  return 0;
}

static u32 typeFromName(const s8 *name) {
  size_t i;

  for (i = 0; i < sizeof(kE820Types) / sizeof(kE820Types[0]); i++) {
    if (!strcmp(name, kE820Types[i].name)) return kE820Types[i].type;
  }
  return FDK_E820_RESERVED;
}

static s32 compareRecord(const void *a, const void *b) {
  const fdkE820record_t *x = a, *y = b;
  return (x->baseAddr > y->baseAddr) - (x->baseAddr < y->baseAddr);
}

u32 e820ReadMap(fdkE820record_t *recs, u32 max) {
  DIR *dir;
  struct dirent *ent;
  u32 count = 0;

  dir = opendir(FDK_FIRMWARE_MEMMAP);
  if (!dir) return 0;

  while (count < max && (ent = readdir(dir))) {
    s8 start[32], end[32], type[64];
    u64 s, e;

    if (ent->d_name[0] == '.') continue;
    if (readAttr(ent->d_name, "start", start, sizeof(start)) ||
        readAttr(ent->d_name, "end", end, sizeof(end)) ||
        readAttr(ent->d_name, "type", type, sizeof(type))) {
      continue;
    }

    s = strtoull(start, NULL, 0);
    e = strtoull(end, NULL, 0);
    if (e < s) continue;

    recs[count].baseAddr = s;
    recs[count].length = e - s + 1;
    recs[count].type = typeFromName(type);
    recs[count].attr = 0;
    count++;
  }
  closedir(dir);

  qsort(recs, count, sizeof(*recs), compareRecord);
  return count;
}
