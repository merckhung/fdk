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
#include <libcomm.h>
#include <netsock.h>
#include <packet.h>
#include <libmem.h>
#include <libpci.h>


u32 fdkPciDetectDevice( fdkPciDev_t *pFdkPciDev ) {

    u32 value;
    u16 bus;
    u8 dev, func;
    u32 count = 0;

    for( bus = 0 ; bus <= PCI_BUS_MASK ; bus++ )
        for( dev = 0 ; dev <= PCI_DEV_MASK ; dev++ )
            for( func = 0 ; func <= PCI_FUNC_MASK ; func++ ) {

                value = pciReadConfDWord( pciBaseAddress( bus, dev, func, 0 ) );
                if( value != 0xFFFFFFFF ) {

                    pFdkPciDev->bus = bus;
                    pFdkPciDev->dev = dev;
                    pFdkPciDev->fun = func;
                    pFdkPciDev->vendorId = value & 0xFFFF;
                    pFdkPciDev->deviceId = (value >> 16) & 0xFFFF;

                    count++;
                    pFdkPciDev++;
                }
            }

    return count;
}


s32 handleRequestPacket( s32 cfd, s32 memfd, fdkCommPkt_t *pFdkCommPkt, u32 rByte ) {

	u32 sz = 0;
	u64 addr = 0;
	u16 ioAddr = 0;
	u32 pciAddr = 0;
	u16 pciSz = 0;
	u8 cmosAddr = 0, cmosSz = 0;
	u32 i;
	s8 *ptr;
	s32 ret = 0;
	fdkErrorCode_t fdkErrCode;

	// Basic heck for a corrupt packet
	if( (rByte < sizeof( fdkCommHdr_t ))
		|| (rByte < pFdkCommPkt->fdkCommHdr.pktLen) )
		return -1;

	// Grant the necessary IO permission
	if( pciInitialize() == FALSE )
        return -1;

	// Handle OPcode
	switch( pFdkCommPkt->fdkCommHdr.opCode ) {

	case FDK_REQ_CONNECT:

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_CONNECT;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkCommHdr_t );
		pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
		break;

	case FDK_REQ_DISCONNECT:

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_DISCONNECT;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkCommHdr_t );
		pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
		ret = -1;
		break;

	case FDK_REQ_MEM_READ:

		// Read memory content
		fdkErrCode = FDK_SUCCESS;
		ptr = (s8 *)&pFdkCommPkt->fdkRspMemReadPkt.memContent;
		addr = pFdkCommPkt->fdkReqMemReadPkt.address;
		sz = pFdkCommPkt->fdkReqMemReadPkt.size;

		// Read memory content to a buffer
		if( memReadBuffer( memfd, addr, sz, (u8 *)ptr ) )
			fdkErrCode = FDK_FAILURE;

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_MEM_READ;
		pFdkCommPkt->fdkCommHdr.pktLen = 
			sizeof( fdkRspMemReadPkt_t ) - sizeof( s8 * ) + sz;
		pFdkCommPkt->fdkCommHdr.errorCode = fdkErrCode;
		pFdkCommPkt->fdkRspMemReadPkt.address = addr;
		pFdkCommPkt->fdkRspMemReadPkt.size = sz;
		break;

	case FDK_REQ_MEM_WRITE:

		// write memory content
		fdkErrCode = FDK_SUCCESS;
		ptr = (s8 *)&pFdkCommPkt->fdkReqMemWritePkt.memContent;
		addr = pFdkCommPkt->fdkReqMemWritePkt.address;
		sz = pFdkCommPkt->fdkReqMemWritePkt.size;

        // Write memory content with a buffer
		if( memWriteBuffer( memfd, addr, sz, (u8 *)ptr ) )
			fdkErrCode = FDK_FAILURE;

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_MEM_WRITE;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkRspMemWritePkt_t );
		pFdkCommPkt->fdkCommHdr.errorCode = fdkErrCode;
		pFdkCommPkt->fdkRspMemWritePkt.address = addr;
		pFdkCommPkt->fdkRspMemWritePkt.size = sz;
		break;

	case FDK_REQ_IO_READ:

		// Read IO content
		fdkErrCode = FDK_SUCCESS;
		ptr = (s8 *)&pFdkCommPkt->fdkRspIoReadPkt.ioContent;
		ioAddr = pFdkCommPkt->fdkReqIoReadPkt.address;
		sz = pFdkCommPkt->fdkReqIoReadPkt.size;

		if( !ioperm( ioAddr, ioAddr + sz, 1 ) ) {

			for( i = 0 ; i < sz ; i++ ) {

				// Read IO data
				*(ptr + i) = inb( ioAddr + i );
			}
		}
		else {

			fdkErrCode = FDK_FAILURE;
			memset( ptr, sz, 0xFF );
		}

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_IO_READ;
		pFdkCommPkt->fdkCommHdr.pktLen = 
			sizeof( fdkRspIoReadPkt_t ) - sizeof( s8 * ) + sz;
		pFdkCommPkt->fdkCommHdr.errorCode = fdkErrCode;
		pFdkCommPkt->fdkRspIoReadPkt.address = ioAddr;
		pFdkCommPkt->fdkRspIoReadPkt.size = sz;
		break;

	case FDK_REQ_IO_WRITE:

		// Write IO content
		fdkErrCode = FDK_SUCCESS;
		ptr = (s8 *)&pFdkCommPkt->fdkReqIoWritePkt.ioContent;
		ioAddr = pFdkCommPkt->fdkReqIoReadPkt.address;
		sz = pFdkCommPkt->fdkReqIoReadPkt.size;

        if( !ioperm( ioAddr, ioAddr + sz, 1 ) ) {

			for( i = 0 ; i < sz ; i++ ) {

				// Write IO data
				outb( *(ptr + i), ioAddr + i );
			}
		}
		else
			fdkErrCode = FDK_FAILURE;

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_IO_WRITE;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkRspIoWritePkt_t );
		pFdkCommPkt->fdkCommHdr.errorCode = fdkErrCode;
		pFdkCommPkt->fdkRspIoWritePkt.address = ioAddr;
		pFdkCommPkt->fdkRspIoWritePkt.size = sz;
		break;

	case FDK_REQ_PCI_READ:

		// Read PCI config space
        ptr = (s8 *)&pFdkCommPkt->fdkRspPciReadPkt.pciContent;
        pciAddr = pFdkCommPkt->fdkReqPciReadPkt.address;
        pciSz = pFdkCommPkt->fdkReqPciReadPkt.size;

        for( i = 0 ; i < pciSz ; i++ ) {

         	// Read PCI config data
			*(ptr + i) = pciReadConfByte( pciAddr + i );
        }

        // Prepare the response packet
        pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_PCI_READ;
        pFdkCommPkt->fdkCommHdr.pktLen =
             sizeof( fdkRspPciReadPkt_t ) - sizeof( s8 * ) + pciSz;
        pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
        pFdkCommPkt->fdkRspPciReadPkt.address = pciAddr;
        pFdkCommPkt->fdkRspPciReadPkt.size = pciSz;
        break;

	case FDK_REQ_PCI_WRITE:

		// Write PCI config space
		ptr = (s8 *)&pFdkCommPkt->fdkReqPciWritePkt.pciContent;
		pciAddr = pFdkCommPkt->fdkReqPciReadPkt.address;
		pciSz = pFdkCommPkt->fdkReqPciReadPkt.size;

		for( i = 0 ; i < pciSz ; i++ ) {

			// Write PCI config data
			pciWriteConfByte( pciAddr + i, *(ptr + i) );
		}

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_PCI_WRITE;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkRspPciWritePkt_t );
		pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
		pFdkCommPkt->fdkRspPciWritePkt.address = pciAddr;
		pFdkCommPkt->fdkRspPciWritePkt.size = pciSz;
		break;

    case FDK_REQ_IDE_READ:

#if 0
        // Read IDE device
        ptr = (s8 *)&pFdkCommPkt->fdkRspIdeReadPkt.ideContent;
        addr = pFdkCommPkt->fdkReqIdeReadPkt.address;
        sz = pFdkCommPkt->fdkReqIdeReadPkt.size;

		// Read IDE data
		FdkIdeReadWrite( addr, sz, (u8 *)ptr, Fdk_REQ_IDE_READ );
#endif

        // Prepare the response packet
        pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_IDE_READ;
        pFdkCommPkt->fdkCommHdr.pktLen =
			sizeof( fdkRspIdeReadPkt_t ) - sizeof( s8 * ) + sz;
        pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
        pFdkCommPkt->fdkRspIdeReadPkt.address = addr;
        pFdkCommPkt->fdkRspIdeReadPkt.size = sz;
        break;

	case FDK_REQ_IDE_WRITE:

#if 0
		// Write IDE device
		ptr = (s8 *)&pFdkCommPkt->fdkReqIdeWritePkt.ideContent;
		addr = pFdkCommPkt->fdkReqIdeReadPkt.address;
		sz = pFdkCommPkt->fdkReqIdeReadPkt.size;

		// Write IDE data
		FdkIdeReadWrite( addr, sz, (u8 *)ptr, Fdk_REQ_IDE_WRITE );
#endif

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_IDE_WRITE;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkRspIdeWritePkt_t );
		pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
		pFdkCommPkt->fdkRspIdeWritePkt.address = addr;
		pFdkCommPkt->fdkRspIdeWritePkt.size = sz;
		break;

	case FDK_REQ_CMOS_READ:

		// Read CMOS device
		fdkErrCode = FDK_SUCCESS;
        ptr = (s8 *)&pFdkCommPkt->fdkRspCmosReadPkt.cmosContent;
        cmosAddr = pFdkCommPkt->fdkReqCmosReadPkt.address;
        cmosSz = pFdkCommPkt->fdkReqCmosReadPkt.size;

		if( cmosInitialize() == TRUE ) {

			// Read CMOS data
			for( i = 0 ; i < cmosSz ; i++ ) {

				// Write CMOS address
				outb( i, FDK_CMOS_ADDR );

				// Read CMOS data
				*(ptr + i) = inb( FDK_CMOS_DATA );
			}
		}
		else
			fdkErrCode = FDK_FAILURE;

        // Prepare the response packet
        pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_CMOS_READ;
        pFdkCommPkt->fdkCommHdr.pktLen =
        	sizeof( fdkRspCmosReadPkt_t ) - sizeof( s8 * ) + cmosSz;
        pFdkCommPkt->fdkCommHdr.errorCode = fdkErrCode;
        pFdkCommPkt->fdkRspCmosReadPkt.address = cmosAddr;
        pFdkCommPkt->fdkRspCmosReadPkt.size = cmosSz;
        break;

	case FDK_REQ_CMOS_WRITE:

		// Write CMOS device
		fdkErrCode = FDK_SUCCESS;
		ptr = (s8 *)&pFdkCommPkt->fdkReqCmosWritePkt.cmosContent;
		cmosAddr = pFdkCommPkt->fdkReqCmosReadPkt.address;
		cmosSz = pFdkCommPkt->fdkReqCmosReadPkt.size;

		if( cmosInitialize() == TRUE ) {

			// Write CMOS data
			for( i = 0 ; i < cmosSz ; i++ ) {

				// Write CMOS address
				outb( i, FDK_CMOS_ADDR );

				// Write CMOS data
				outb( *(ptr + i), FDK_CMOS_DATA );
			}
		}
		else
			fdkErrCode = FDK_FAILURE;

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_CMOS_WRITE;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkRspCmosWritePkt_t );
		pFdkCommPkt->fdkCommHdr.errorCode = fdkErrCode;
		pFdkCommPkt->fdkRspCmosWritePkt.address = cmosAddr;
		pFdkCommPkt->fdkRspCmosWritePkt.size = cmosSz;
		break;

	case FDK_REQ_PCI_LIST:

		// Scan PCI devices
		pFdkCommPkt->fdkRspPciListPkt.numOfPciDevice =
		fdkPciDetectDevice( (fdkPciDev_t *)&pFdkCommPkt->fdkRspPciListPkt.pciListContent );

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_PCI_LIST;
		pFdkCommPkt->fdkCommHdr.pktLen = 
			sizeof( fdkRspPciListPkt_t ) - sizeof( fdkPciDev_t * )
			+ pFdkCommPkt->fdkRspPciListPkt.numOfPciDevice * sizeof( fdkPciDev_t );
		pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
		break;

	case FDK_REQ_E820_LIST:

		// Copy E820
#if 0
		pFdkCommPkt->fdkRspE820ListPkt.numOfE820Record =
			FdkE820Copy( (FdkE820record_t *)&pFdkCommPkt->fdkRspE820ListPkt.e820ListContent );
#else
		pFdkCommPkt->fdkRspE820ListPkt.numOfE820Record = 0;
#endif

		// Prepare the response packet
		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_E820_LIST;
		pFdkCommPkt->fdkCommHdr.pktLen = 
			sizeof( fdkRspE820ListPkt_t ) - sizeof( fdkE820record_t * )
			+ pFdkCommPkt->fdkRspE820ListPkt.numOfE820Record * sizeof( fdkE820record_t );
		pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
		break;
	
	default:

		pFdkCommPkt->fdkCommHdr.opCode = FDK_RSP_NACK;
		pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkCommHdr_t );
		pFdkCommPkt->fdkCommHdr.errorCode = FDK_SUCCESS;
		break;
	}

	return ret;
}


