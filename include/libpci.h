/*
 * FDK - Firmware Debug Kit
 * File: libpci.h
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

#ifndef FDK_INCLUDE_LIBPCI_H_
#define FDK_INCLUDE_LIBPCI_H_

#include "mtypes.h"
#include "packet.h"

// PCI configuration addresses use the classic CF8h layout, with the PCI
// segment (domain) stored in the otherwise reserved bits 30:24:
//
//   31     30:24    23:16  15:11  10:8   7:0
//   enable segment  bus    dev    func   register
#define PCI_ADDR_ENABLE 0x80000000U
#define PCI_SEG_MASK 0x7F
#define PCI_BUS_MASK 0xFF
#define PCI_DEV_MASK 0x1F
#define PCI_FUNC_MASK 0x07
#define PCI_REG_MASK 0xFF
#define PCI_SEG_OFFSET 24
#define PCI_BUS_OFFSET 16
#define PCI_DEV_OFFSET 11
#define PCI_FUNC_OFFSET 8

#define PCI_SYSFS_DEVICES "/sys/bus/pci/devices"

// Fills |pFdkPciDev| with up to |max| devices sorted by segment, bus,
// device and function. Returns the number of devices found.
u32 pciListDevices(fdkPciDev_t *pFdkPciDev, u32 max);

// Reads/writes |len| bytes of configuration space starting at |addr|.
// Bytes that cannot be read are returned as 0xFF. Return 0 on success.
s32 pciReadConfig(u32 addr, u32 len, u8 *buf);
s32 pciWriteConfig(u32 addr, u32 len, const u8 *buf);

#endif  // FDK_INCLUDE_LIBPCI_H_
