/*
 * FDK - Firmware Debug Kit
 * File: libe820.h
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

#ifndef FDK_INCLUDE_LIBE820_H_
#define FDK_INCLUDE_LIBE820_H_

#include "mtypes.h"
#include "packet.h"

#define FDK_FIRMWARE_MEMMAP "/sys/firmware/memmap"

// ACPI address range types (E820).
#define FDK_E820_RAM 1
#define FDK_E820_RESERVED 2
#define FDK_E820_ACPI 3
#define FDK_E820_NVS 4
#define FDK_E820_UNUSABLE 5
#define FDK_E820_PMEM 7
#define FDK_E820_SOFT_RESERVED 0xEFFFFFFF

// Reads the firmware-provided memory map from /sys/firmware/memmap into
// |recs|, sorted by base address. Returns the number of records.
u32 e820ReadMap(fdkE820record_t *recs, u32 max);

#endif  // FDK_INCLUDE_LIBE820_H_
