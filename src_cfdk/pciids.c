/*
 * FDK - Firmware Debug Kit
 * File: pciids.c
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfdk.h"

// Candidate PCI ID databases. The one with the newest "Version:" header
// wins, so a freshly updated system database beats an older bundled copy
// and vice versa.
static const s8 *const kPciIdsPaths[] = {
    FDK_DATADIR "/pci.ids",       // make install
    "data/pci.ids",               // Running from the source tree.
    "/usr/share/hwdata/pci.ids",  // Fedora, Arch, openSUSE, Gentoo
    "/usr/share/misc/pci.ids",    // Debian, Ubuntu, FreeBSD
    "/usr/share/pci.ids",
};

// Reads the "Version: YYYY.MM.DD" line from the header of a pci.ids file.
s32 readPciIdsVersion(const s8 *path, s8 *version, u32 len) {
  s8 line[FDK_MAX_READBUF];
  FILE *fp;
  s32 ret = -1;

  fp = fopen(path, "re");
  if (!fp) return -1;

  while (fgets(line, sizeof(line), fp) && line[0] == '#') {
    const s8 *v = strstr(line, "Version:");
    if (v) {
      v += strlen("Version:");
      while (isspace((unsigned char)*v)) v++;
      snprintf(version, len, "%.*s", (int)strcspn(v, " \t\n"), v);
      ret = 0;
      break;
    }
  }
  fclose(fp);
  return ret;
}

s32 findPciIdsFile(s8 *path, u32 len, s8 *version, u32 versionLen) {
  s8 best[32] = "";
  size_t i;
  s32 ret = -1;

  for (i = 0; i < sizeof(kPciIdsPaths) / sizeof(kPciIdsPaths[0]); i++) {
    s8 v[32];

    if (readPciIdsVersion(kPciIdsPaths[i], v, sizeof(v))) continue;
    // YYYY.MM.DD versions compare correctly as strings.
    if (ret || strcmp(v, best) > 0) {
      snprintf(best, sizeof(best), "%s", v);
      snprintf(path, len, "%s", kPciIdsPaths[i]);
      ret = 0;
    }
  }

  if (!ret) snprintf(version, versionLen, "%s", best);
  return ret;
}

// Parses "<4 hex digits><spaces><name>". Returns the name or NULL.
static const s8 *parseIdLine(const s8 *line, u32 *id) {
  s8 *end;

  if (!isxdigit((unsigned char)line[0])) return NULL;
  *id = (u32)strtoul(line, &end, 16);
  if (end != line + 4 || !isspace((unsigned char)*end)) return NULL;
  while (isspace((unsigned char)*end)) end++;
  return end;
}

s32 loadPciNames(fdkUiProperty_t *pFdkUiProperty) {
  s8 line[FDK_MAX_READBUF];
  const s8 *name;
  FILE *fp;
  u32 i, id;
  s32 vendor = -1;

  for (i = 0; i < pFdkUiProperty->numOfPciDevice; i++) {
    snprintf(pFdkUiProperty->pFdkPciIds[i].venTxt, FDK_MAX_PCINAME,
             "Unknown vendor");
    snprintf(pFdkUiProperty->pFdkPciIds[i].devTxt, FDK_MAX_PCINAME,
             "Unknown device");
  }

  fp = fopen(pFdkUiProperty->pciIdsPath, "re");
  if (!fp) return 1;

  while (fgets(line, sizeof(line), fp)) {
    line[strcspn(line, "\n")] = 0;
    if (!line[0] || line[0] == '#') continue;

    // The device class list at the end of the file has no vendors.
    if (line[0] == 'C' && line[1] == ' ') break;

    if (line[0] != '\t') {
      // Vendor: "vvvv  Vendor Name"
      name = parseIdLine(line, &id);
      vendor = name ? (s32)id : -1;
      for (i = 0; name && i < pFdkUiProperty->numOfPciDevice; i++) {
        if (pFdkUiProperty->pFdkPciDev[i].vendorId == id) {
          snprintf(pFdkUiProperty->pFdkPciIds[i].venTxt, FDK_MAX_PCINAME, "%s",
                   name);
        }
      }
    } else if (line[1] != '\t' && vendor >= 0) {
      // Device of the current vendor: "\tdddd  Device Name"
      name = parseIdLine(line + 1, &id);
      for (i = 0; name && i < pFdkUiProperty->numOfPciDevice; i++) {
        if (pFdkUiProperty->pFdkPciDev[i].vendorId == vendor &&
            pFdkUiProperty->pFdkPciDev[i].deviceId == id) {
          snprintf(pFdkUiProperty->pFdkPciIds[i].devTxt, FDK_MAX_PCINAME, "%s",
                   name);
        }
      }
    }
  }

  fclose(fp);
  return 0;
}
