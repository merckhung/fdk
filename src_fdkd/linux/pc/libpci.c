/*
 * FDK - Firmware Debug Kit
 * File: libpci.c
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
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/io.h>

#include <mtypes.h>
#include <fdk.h>
#include <libpci.h>

u32 pciBaseAddress(u32 bus, u32 dev, u32 func, u32 reg) {

  return (PCI_ADDR_ENABLE | ((bus & PCI_BUS_MASK) << PCI_BUS_OFFSET)
      | ((dev & PCI_DEV_MASK) << PCI_DEV_OFFSET)
      | ((func & PCI_FUNC_MASK) << PCI_FUNC_OFFSET)
      | ((reg & PCI_REG_MASK) << PCI_REG_OFFSET));
}

s32 pciInitialize(void) {

  if (ioperm( PCI_ADDR_PORT, PCI_DATA_PORT_END, 1))
    return FALSE;

  return TRUE;
}

u32 pciReadConfDWord(u32 addr) {

  outl(addr, PCI_ADDR_PORT);
  return inl( PCI_DATA_PORT);
}

void pciWriteConfDWord(u32 addr, u32 value) {

  outl(addr, PCI_ADDR_PORT);
  outl(value, PCI_DATA_PORT);
}

u8 pciReadConfByte(u32 addr) {

  u32 off = addr & PCI_REG_ALIGN;
  return (pciReadConfDWord(addr & (~PCI_REG_ALIGN)) >> (off * PCI_BITS_BYTE))
      & 0xFFUL;
}

void pciWriteConfByte(u32 addr, u8 value) {

  u32 off = addr & PCI_REG_ALIGN;
  u32 val, mask = (0xFFUL << (off * PCI_BITS_BYTE));
  u32 input = value;

  input <<= (off * PCI_BITS_BYTE);
  val = pciReadConfDWord(addr & (~PCI_REG_ALIGN)) & mask;
  val |= input;

  pciWriteConfDWord(addr & (~PCI_REG_ALIGN), input);
}

s32 cmosInitialize(void) {

  if (ioperm( FDK_CMOS_ADDR, FDK_CMOS_DATA, 1))
    return FALSE;

  return TRUE;
}

