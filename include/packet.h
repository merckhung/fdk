/*
 * FDK - Firmware Debug Kit
 * File: packet.h
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

#define FDK_MAXSZ_PKT		2048
#define FDK_FIFO_SZ			14

#define FDK_SECTOR_SZ		512
#define FDK_SECTOR_SZULL	512ULL
#define FDK_IDE_BUF			(FDK_SECTOR_SZ * 2)


typedef enum {

	FDK_CAP_MEM		= 0x00000001,
	FDK_CAP_IO		= 0x00000002,
	FDK_CAP_PCI		= 0x00000004,
	FDK_CAP_IDE		= 0x00000008,
	FDK_CAP_E820	= 0x00000010,

	FDK_CAP_I2C		= 0x00000020,
	FDK_CAP_MSR		= 0x00000040,
	FDK_CAP_ACPI	= 0x00000080,
	FDK_CAP_SFI		= 0x00000100,
	FDK_CAP_GPIO	= 0x00000200,
	FDK_CAP_SMBIOS	= 0x00000400,
	FDK_CAP_SIO		= 0x00000800,
	FDK_CAP_SPD		= 0x00001000,

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


typedef struct PACKED {

	u16						bus;
	u8						dev;
	u8						fun;
	u16						vendorId;
	u16						deviceId;

} fdkPciDev_t;


typedef struct PACKED {

	u64						baseAddr;
	u64						length;
	u32						type;
	u32						attr;

} fdkE820record_t;


// Common packet
typedef struct PACKED _fdkCommHdr {

	u16		opCode;

	union {

		u16		pad;
		u16		errorCode;
	};

	u32		pktLen;

} fdkCommHdr_t;


// Memory space Read/Write packets
typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u64						address;
	u32						size;

} fdkReqMemReadPkt_t, fdkRspMemWritePkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u64						address;
	u32						size;
	u8						*memContent;

} fdkRspMemReadPkt_t, fdkReqMemWritePkt_t;


// IO space Read/Write packets
typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u16						address;
	u32						size;

} fdkReqIoReadPkt_t, fdkRspIoWritePkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u16						address;
	u32						size;
	u8						*ioContent;

} fdkRspIoReadPkt_t, fdkReqIoWritePkt_t;


// PCI config Read/Write packets
typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u32						address;
	u16						size;

} fdkReqPciReadPkt_t, fdkRspPciWritePkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u32						address;
	u16						size;
	u8						*pciContent;

} fdkRspPciReadPkt_t, fdkReqPciWritePkt_t;


// IDE Read/Write packets
typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u64						address;
	u32						size;

} fdkReqIdeReadPkt_t, fdkRspIdeWritePkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u64						address;
	u32						size;
	u8						*ideContent;

} fdkRspIdeReadPkt_t, fdkReqIdeWritePkt_t;


// CMOS Read/Write packets
typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u8						address;
	u8						size;

} fdkReqCmosReadPkt_t, fdkRspCmosWritePkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u8						address;
	u8						size;
	u8						*cmosContent;

} fdkRspCmosReadPkt_t, fdkReqCmosWritePkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;

} fdkReqPciListPkt_t, fdkReqE820ListPkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u32						numOfPciDevice;
	fdkPciDev_t			*pciListContent;

} fdkRspPciListPkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u32						numOfE820Record;
	fdkE820record_t		e820ListContent[ 1 ];

} fdkRspE820ListPkt_t;


typedef struct PACKED {

	fdkCommHdr_t			fdkCommHdr;
	u32						exNum;

} fdkRspCpuExceptionPkt_t;


typedef struct PACKED {

	union {

		// Common
		fdkCommHdr_t			fdkCommHdr;

		// Memory Read
		fdkReqMemReadPkt_t		fdkReqMemReadPkt;
		fdkRspMemReadPkt_t		fdkRspMemReadPkt;

		// Memory Write
		fdkReqMemWritePkt_t		fdkReqMemWritePkt;
		fdkRspMemWritePkt_t		fdkRspMemWritePkt;

		// IO Read
		fdkReqIoReadPkt_t		fdkReqIoReadPkt;
		fdkRspIoReadPkt_t		fdkRspIoReadPkt;

		// IO Write
		fdkReqIoWritePkt_t		fdkReqIoWritePkt;
		fdkRspIoWritePkt_t		fdkRspIoWritePkt;

		// PCI Read
		fdkReqPciReadPkt_t		fdkReqPciReadPkt;
		fdkRspPciReadPkt_t		fdkRspPciReadPkt;

		// PCI Write
		fdkReqPciWritePkt_t		fdkReqPciWritePkt;
		fdkRspPciWritePkt_t		fdkRspPciWritePkt;

		// IDE Read
		fdkReqIdeReadPkt_t		fdkReqIdeReadPkt;
		fdkRspIdeReadPkt_t		fdkRspIdeReadPkt;

		// IDE Write
		fdkReqIdeWritePkt_t		fdkReqIdeWritePkt;
		fdkRspIdeWritePkt_t		fdkRspIdeWritePkt;

		// CMOS Read
		fdkReqCmosReadPkt_t		fdkReqCmosReadPkt;
		fdkRspCmosReadPkt_t		fdkRspCmosReadPkt;

		// CMOS Write
		fdkReqCmosWritePkt_t	fdkReqCmosWritePkt;
		fdkRspCmosWritePkt_t	fdkRspCmosWritePkt;

		// PCI List
		fdkReqPciListPkt_t		fdkReqPciListPkt;
		fdkRspPciListPkt_t		fdkRspPciListPkt;

		// E820 List
		fdkReqE820ListPkt_t		fdkReqE820ListPkt;
		fdkRspE820ListPkt_t		fdkRspE820ListPkt;

		// CPU Exception
		fdkRspCpuExceptionPkt_t	fdkRspCpuExceptionPkt;
	};

} fdkCommPkt_t;


// Prototypes
s32 verifyResponsePacket( fdkCommPkt_t *pFdkCommPkt, fdkOpCode_t op );
s32 executeFunction( s32 fd, fdkOpCode_t op, u64 addr, u32 size, u8 *cntBuf, u8 *pktBuf, s32 lenPktBuf );


