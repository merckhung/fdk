/*
 * FDK - Firmware Debug Kit
 * File: packet.h
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

#ifndef FDK_INCLUDE_PACKET_H_
#define FDK_INCLUDE_PACKET_H_

#include <stddef.h>

#include "mtypes.h"

// Largest packet either side will send or accept.
#define FDK_MAXSZ_PKT 8192

typedef enum {
  FDK_CAP_MEM = 0x00000001,
  FDK_CAP_IO = 0x00000002,
  FDK_CAP_PCI = 0x00000004,
  FDK_CAP_IDE = 0x00000008,
  FDK_CAP_E820 = 0x00000010,
  FDK_CAP_I2C = 0x00000020,
  FDK_CAP_MSR = 0x00000040,
  FDK_CAP_ACPI = 0x00000080,
  FDK_CAP_SFI = 0x00000100,
  FDK_CAP_GPIO = 0x00000200,
  FDK_CAP_SMBIOS = 0x00000400,
  FDK_CAP_SIO = 0x00000800,
  FDK_CAP_SPD = 0x00001000,
} fdkCapability;

typedef enum {
  FDK_REQ_CONNECT = 1,
  FDK_RSP_CONNECT,
  FDK_REQ_DISCONNECT,
  FDK_RSP_DISCONNECT,
  FDK_REQ_MEM_READ,
  FDK_RSP_MEM_READ,
  FDK_REQ_MEM_WRITE,
  FDK_RSP_MEM_WRITE,
  FDK_REQ_IO_READ,
  FDK_RSP_IO_READ,
  FDK_REQ_IO_WRITE,
  FDK_RSP_IO_WRITE,
  FDK_REQ_PCI_READ,
  FDK_RSP_PCI_READ,
  FDK_REQ_PCI_WRITE,
  FDK_RSP_PCI_WRITE,
  FDK_REQ_IDE_READ,
  FDK_RSP_IDE_READ,
  FDK_REQ_IDE_WRITE,
  FDK_RSP_IDE_WRITE,
  FDK_REQ_CMOS_READ,
  FDK_RSP_CMOS_READ,
  FDK_REQ_CMOS_WRITE,
  FDK_RSP_CMOS_WRITE,
  FDK_REQ_PCI_LIST,
  FDK_RSP_PCI_LIST,
  FDK_REQ_E820_LIST,
  FDK_RSP_E820_LIST,
  FDK_RSP_CPU_EXCEPTION,
  FDK_RSP_NACK,
} fdkOpCode_t;

typedef enum _fdkErrorCode {
  FDK_SUCCESS = 0,
  FDK_FAILURE,
} fdkErrorCode_t;

// PCI device as reported by FDK_RSP_PCI_LIST. For devices outside PCI
// segment 0 the segment number is carried in bits 15:8 of |bus|.
typedef struct PACKED {
  u16 bus;
  u8 dev;
  u8 fun;
  u16 vendorId;
  u16 deviceId;
} fdkPciDev_t;

// Firmware memory map entry. |type| uses the ACPI/E820 numbering.
typedef struct PACKED {
  u64 baseAddr;
  u64 length;
  u32 type;
  u32 attr;
} fdkE820record_t;

typedef struct PACKED _fdkCommHdr {
  u16 opCode;
  union {
    u16 pad;
    u16 errorCode;
  };
  u32 pktLen;
} fdkCommHdr_t;

// The last member of each packet below marks where its variable-length
// payload starts on the wire; use FDK_PKT_LEN() instead of sizeof().

// Memory space read/write packets.
typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u64 address;
  u32 size;
} fdkReqMemReadPkt_t, fdkRspMemWritePkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u64 address;
  u32 size;
  u8 *memContent;
} fdkRspMemReadPkt_t, fdkReqMemWritePkt_t;

// I/O space read/write packets.
typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u16 address;
  u32 size;
} fdkReqIoReadPkt_t, fdkRspIoWritePkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u16 address;
  u32 size;
  u8 *ioContent;
} fdkRspIoReadPkt_t, fdkReqIoWritePkt_t;

// PCI configuration space read/write packets.
typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u32 address;
  u16 size;
} fdkReqPciReadPkt_t, fdkRspPciWritePkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u32 address;
  u16 size;
  u8 *pciContent;
} fdkRspPciReadPkt_t, fdkReqPciWritePkt_t;

// Disk read/write packets.
typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u64 address;
  u32 size;
} fdkReqIdeReadPkt_t, fdkRspIdeWritePkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u64 address;
  u32 size;
  u8 *ideContent;
} fdkRspIdeReadPkt_t, fdkReqIdeWritePkt_t;

// CMOS read/write packets.
typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u8 address;
  u8 size;
} fdkReqCmosReadPkt_t, fdkRspCmosWritePkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u8 address;
  u8 size;
  u8 *cmosContent;
} fdkRspCmosReadPkt_t, fdkReqCmosWritePkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
} fdkReqPciListPkt_t, fdkReqE820ListPkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u32 numOfPciDevice;
  fdkPciDev_t *pciListContent;
} fdkRspPciListPkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u32 numOfE820Record;
  fdkE820record_t e820ListContent[1];
} fdkRspE820ListPkt_t;

typedef struct PACKED {
  fdkCommHdr_t fdkCommHdr;
  u32 exNum;
} fdkRspCpuExceptionPkt_t;

typedef struct PACKED {
  union {
    fdkCommHdr_t fdkCommHdr;

    fdkReqMemReadPkt_t fdkReqMemReadPkt;
    fdkRspMemReadPkt_t fdkRspMemReadPkt;
    fdkReqMemWritePkt_t fdkReqMemWritePkt;
    fdkRspMemWritePkt_t fdkRspMemWritePkt;

    fdkReqIoReadPkt_t fdkReqIoReadPkt;
    fdkRspIoReadPkt_t fdkRspIoReadPkt;
    fdkReqIoWritePkt_t fdkReqIoWritePkt;
    fdkRspIoWritePkt_t fdkRspIoWritePkt;

    fdkReqPciReadPkt_t fdkReqPciReadPkt;
    fdkRspPciReadPkt_t fdkRspPciReadPkt;
    fdkReqPciWritePkt_t fdkReqPciWritePkt;
    fdkRspPciWritePkt_t fdkRspPciWritePkt;

    fdkReqIdeReadPkt_t fdkReqIdeReadPkt;
    fdkRspIdeReadPkt_t fdkRspIdeReadPkt;
    fdkReqIdeWritePkt_t fdkReqIdeWritePkt;
    fdkRspIdeWritePkt_t fdkRspIdeWritePkt;

    fdkReqCmosReadPkt_t fdkReqCmosReadPkt;
    fdkRspCmosReadPkt_t fdkRspCmosReadPkt;
    fdkReqCmosWritePkt_t fdkReqCmosWritePkt;
    fdkRspCmosWritePkt_t fdkRspCmosWritePkt;

    fdkReqPciListPkt_t fdkReqPciListPkt;
    fdkRspPciListPkt_t fdkRspPciListPkt;

    fdkReqE820ListPkt_t fdkReqE820ListPkt;
    fdkRspE820ListPkt_t fdkRspE820ListPkt;

    fdkRspCpuExceptionPkt_t fdkRspCpuExceptionPkt;
  };
} fdkCommPkt_t;

// Wire length of a |type| packet carrying |payload| bytes after |field|.
#define FDK_PKT_LEN(type, field, payload) (offsetof(type, field) + (payload))

// Largest payload a |type| packet can carry after |field|.
#define FDK_PKT_MAX_PAYLOAD(type, field) (FDK_MAXSZ_PKT - offsetof(type, field))

// Returns 0 when |pFdkCommPkt| is the response matching request |op|.
s32 verifyResponsePacket(const fdkCommPkt_t *pFdkCommPkt, fdkOpCode_t op);

// Builds request |op|, sends it over |fd| and waits for the response, which
// is left in |pktBuf|. Returns 0 on success.
s32 executeFunction(s32 fd, fdkOpCode_t op, u64 addr, u32 size,
                    const u8 *cntBuf, u8 *pktBuf, u32 lenPktBuf);

// Reads one complete packet (header plus payload) from |fd|. Returns the
// packet length, 0 on orderly shutdown, or -1 on error / malformed packet.
s32 readPacket(s32 fd, void *pktBuf, u32 lenPktBuf);

// Writes one complete packet to |fd|. Returns 0 on success.
s32 writePacket(s32 fd, const void *pktBuf);

#endif  // FDK_INCLUDE_PACKET_H_
