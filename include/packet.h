#define FDK_MAXSZ_PKT	2048
#define FDK_FIFO_SZ		14
#define FDK_SECTOR_SZ	512
#define FDK_SECTOR_SZULL	512ULL
#define FDK_IDE_BUF		(FDK_SECTOR_SZ * 2)
#define FDK_CMOS_ADDR	0x70
#define FDK_CMOS_DATA	0x71


typedef enum {

	UART_PORT0 = 0x03F8,
	UART_PORT1 = 0x02F8,

} fdkDebugPort_t;


typedef enum {

	FDK_UNKNOWN = 0,
	FDK_INIT,
	FDK_READY,
	FDK_PKT_TRAN,
	FDK_PKT_RECV,
	FDK_PKT_DONE,

} fdkState_t;


typedef enum {

	FDK_BAUD_115200 = 0x01,
	FDK_BAUD_38400 = 0x03,
	FDK_BAUD_19200 = 0x06,
	FDK_BAUD_9600 = 0x0C,
	FDK_BAUD_2400 = 0x30,
	FDK_BAUD_1200 = 0x60,

} fdkBaudrate_t;


typedef enum {

	FDK_REQ_CONNECT = 1,
	FDK_RSP_CONNECT,

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


typedef enum _kdbgErrorCode {

	FDK_SUCCESS = 0,
	FDK_FAILURE,

} kdbgErrorCode_t;


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
		fdkCommHdr_t				fdkCommHdr;

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
		fdkReqCmosWritePkt_t		fdkReqCmosWritePkt;
		fdkRspCmosWritePkt_t		fdkRspCmosWritePkt;

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
void fdkInitialization( fdkDebugPort_t port );


