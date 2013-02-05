#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>

#include <mtypes.h>
#include <fdk.h>
#include <packet.h>


#define VERIFY_PAIR( REQ, RSP ) { \
case RSP: \
	if( op != REQ ) \
		return 1; \
	break; \
}


s32 verifyResponsePacket( fdkCommPkt_t *pFdkCommPkt, fdkOpCode_t op ) {

	switch( pFdkCommPkt->fdkCommHdr.opCode ) {

	VERIFY_PAIR( FDK_REQ_CONNECT,	FDK_RSP_CONNECT )
	VERIFY_PAIR( FDK_REQ_DISCONNECT,FDK_RSP_DISCONNECT )
	VERIFY_PAIR( FDK_REQ_MEM_READ,	FDK_RSP_MEM_READ )
	VERIFY_PAIR( FDK_REQ_MEM_WRITE,	FDK_RSP_MEM_WRITE )
	VERIFY_PAIR( FDK_REQ_IO_READ,	FDK_RSP_IO_READ )
	VERIFY_PAIR( FDK_REQ_IO_WRITE,	FDK_RSP_IO_WRITE )
	VERIFY_PAIR( FDK_REQ_PCI_READ,	FDK_RSP_PCI_READ )
	VERIFY_PAIR( FDK_REQ_PCI_WRITE,	FDK_RSP_PCI_WRITE )
	VERIFY_PAIR( FDK_REQ_IDE_READ,	FDK_RSP_IDE_READ )
	VERIFY_PAIR( FDK_REQ_IDE_WRITE,	FDK_RSP_IDE_WRITE )
	VERIFY_PAIR( FDK_REQ_CMOS_READ,	FDK_RSP_CMOS_READ )
	VERIFY_PAIR( FDK_REQ_CMOS_WRITE,FDK_RSP_CMOS_WRITE )
	VERIFY_PAIR( FDK_REQ_PCI_LIST,	FDK_RSP_PCI_LIST )
	VERIFY_PAIR( FDK_REQ_E820_LIST,	FDK_RSP_E820_LIST )

	case FDK_RSP_NACK:
	default:
		return 1;
	}

	return 0;
}


s32 executeFunction( s32 fd, fdkOpCode_t op, u64 addr, u32 size, u8 *cntBuf, u8 *pktBuf, s32 lenPktBuf ) {

	fdkCommPkt_t *pFdkCommPkt = (fdkCommPkt_t *)pktBuf;
	s32 rwByte;

	// Clear buffer
	memset( pktBuf, 0, lenPktBuf );

	// Fill in data fields
	switch( op ) {

		case FDK_REQ_CONNECT:

			pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkCommHdr_t );
			break;

		case FDK_REQ_DISCONNECT:

			pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkCommHdr_t );
			break;

		case FDK_REQ_MEM_READ:

			pFdkCommPkt->fdkReqMemReadPkt.address = addr;
			pFdkCommPkt->fdkReqMemReadPkt.size = size;
			pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkReqMemReadPkt_t );
			break;

		case FDK_REQ_MEM_WRITE:

			if( !size || !cntBuf )
				return 1;

			pFdkCommPkt->fdkReqMemWritePkt.address = addr;
			pFdkCommPkt->fdkReqMemWritePkt.size = size;
			memcpy( &pFdkCommPkt->fdkReqMemWritePkt.memContent, cntBuf, size );
			pFdkCommPkt->fdkCommHdr.pktLen = 
				sizeof( fdkReqMemWritePkt_t ) - sizeof( s8 * ) + size; 
			break;  

		case FDK_REQ_IO_READ:

			pFdkCommPkt->fdkReqIoReadPkt.address = (u16)(addr & 0xFFFFULL);
			pFdkCommPkt->fdkReqIoReadPkt.size = size;
			pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkReqIoReadPkt_t );
			break;

		case FDK_REQ_IO_WRITE:

			if( !size || !cntBuf )
				return 1;

			pFdkCommPkt->fdkReqIoWritePkt.address = (u16)(addr & 0xFFFFULL);
			pFdkCommPkt->fdkReqIoWritePkt.size = size;
			memcpy( &pFdkCommPkt->fdkReqIoWritePkt.ioContent, cntBuf, size );
			pFdkCommPkt->fdkCommHdr.pktLen = 
				sizeof( fdkReqIoWritePkt_t ) - sizeof( s8 * ) + size; 
			break;

		case FDK_REQ_PCI_READ:

			pFdkCommPkt->fdkReqPciReadPkt.address = (u32)(addr & 0xFFFFFFFFULL);
			pFdkCommPkt->fdkReqPciReadPkt.size = (u16)(size & 0xFFFF);
			pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkReqPciReadPkt_t );
			break;

		case FDK_REQ_PCI_WRITE:

			if( !size || !cntBuf )
				return 1;

			pFdkCommPkt->fdkReqPciWritePkt.address = (u32)(addr & 0xFFFFFFFFULL);
			pFdkCommPkt->fdkReqPciWritePkt.size = (u16)(size & 0xFFFF);
			memcpy( &pFdkCommPkt->fdkReqPciWritePkt.pciContent, cntBuf, size );
			pFdkCommPkt->fdkCommHdr.pktLen = 
				sizeof( fdkReqPciWritePkt_t ) - sizeof( s8 * ) + size; 
			break;

		case FDK_REQ_IDE_READ:

			pFdkCommPkt->fdkReqIdeReadPkt.address = addr;
			pFdkCommPkt->fdkReqIdeReadPkt.size = size;
			pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkReqIdeReadPkt_t );
			break;

		case FDK_REQ_IDE_WRITE:

			if( !size || !cntBuf )
				return 1;

			pFdkCommPkt->fdkReqIdeWritePkt.address = addr;
			pFdkCommPkt->fdkReqIdeWritePkt.size = size;
			memcpy( &pFdkCommPkt->fdkReqIdeWritePkt.ideContent, cntBuf, size );
			pFdkCommPkt->fdkCommHdr.pktLen = 
				sizeof( fdkReqIdeWritePkt_t ) - sizeof( s8 * ) + size; 
			break;

		case FDK_REQ_CMOS_READ:

			pFdkCommPkt->fdkReqCmosReadPkt.address = (u8)(addr & 0xFFULL);
			pFdkCommPkt->fdkReqCmosReadPkt.size = (u8)(size & 0xFF);
			pFdkCommPkt->fdkCommHdr.pktLen = sizeof( fdkReqCmosReadPkt_t );
			break;

		case FDK_REQ_CMOS_WRITE:

			if( !size || !cntBuf )
				return 1;

			pFdkCommPkt->fdkReqCmosWritePkt.address = (u8)(addr & 0xFFULL);
			pFdkCommPkt->fdkReqCmosWritePkt.size = (u8)(size & 0xFF);
			memcpy( &pFdkCommPkt->fdkReqCmosWritePkt.cmosContent, cntBuf, size );
			pFdkCommPkt->fdkCommHdr.pktLen = 
				sizeof( fdkReqCmosWritePkt_t ) - sizeof( s8 * ) + size; 
			break;

		case FDK_REQ_PCI_LIST:

			pFdkCommPkt->fdkCommHdr.pktLen =
				sizeof( fdkReqPciWritePkt_t );
			break;

		case FDK_REQ_E820_LIST:

			pFdkCommPkt->fdkCommHdr.pktLen =
				sizeof( fdkReqE820ListPkt_t );
			break;

		default:

			fprintf( stderr, "Unsupported operation, %d\n", op );
			return 1;
	}

	// Fill in OpCode
	pFdkCommPkt->fdkCommHdr.opCode = op;

	// Transmit the request
	rwByte = write( fd, pktBuf, pFdkCommPkt->fdkCommHdr.pktLen );
	if( rwByte != pFdkCommPkt->fdkCommHdr.pktLen ) {

		fprintf( stderr, "Error on transmitting request wsize = %d\n", rwByte );
		return 1;
	}

	// Receive the response
	memset( pktBuf, 0, lenPktBuf );
	rwByte = read( fd, pktBuf, lenPktBuf );
	if( !rwByte ) {

		fprintf( stderr, "No response\n" );
		return 1;
	}


	// Whether it valid or not
	return verifyResponsePacket( pFdkCommPkt, op );
}


