#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>

#include <ncurses.h>
#include <panel.h>

#include <mtypes.h>
#include <fdk.h>
#include <packet.h>
#include <cfdk.h>
#include <libcomm.h>


static u32 editorColorCount = 0;


void printDumpBasePanel( fdkUiProperty_t *pFdkUiProperty ) {

	// Print Top bar
	printWindowAt( pFdkUiProperty->fdkDumpPanel,
		top, 
		FDK_STRING_NLINE,
		strlen( FDK_DUMP_TOP_BAR ),
		FDK_DUMP_TOP_LINE,
		FDK_DUMP_TOP_COLUMN,
		GREEN_BLUE,
		FDK_DUMP_TOP_BAR );

	// Print Rtop bar
	if( pFdkUiProperty->fdkHwFunc != KHF_PCI )
		printWindowAt( pFdkUiProperty->fdkDumpPanel,
			rtop, 
			FDK_STRING_NLINE,
			FDK_DUMP_BYTE_PER_LINE,
			FDK_DUMP_RTOP_LINE,
			FDK_DUMP_RTOP_COLUMN,
			RED_BLUE,
			FDK_DUMP_RTOP_BAR );

	// Print Left bar
	printWindowAt( pFdkUiProperty->fdkDumpPanel,
		left,
		FDK_DUMP_BYTE_PER_LINE,
		4,
		FDK_DUMP_LEFT_LINE,
		FDK_DUMP_LEFT_COLUMN,
		GREEN_BLUE,
		FDK_DUMP_LEFT_BAR );

	// Print Info base
	printWindowAt( pFdkUiProperty->fdkDumpPanel,
		info,
		FDK_STRING_NLINE,
		FDK_MAX_COLUMN,
		FDK_INFO_LINE,
		FDK_INFO_COLUMN,
		WHITE_BLUE,
		"%s",
		pFdkUiProperty->fdkDumpPanel.infoStr );
}


void printDumpUpdatePanel( fdkUiProperty_t *pFdkUiProperty ) {

	s32 i, x, y, color;
	u8 valueBuf[ FDK_DUMP_VBUF_SZ + 1 ];
	u8 asciiBuf[ FDK_DUMP_ABUF_SZ + 1 ];
	u8 *vp = valueBuf, *ap = asciiBuf;
	u8 *dataPtr, *pDataPtr;
	fdkPciDev_t *pFdkPciDev;
	fdkPciConfig_t *pFdkPciConfig;
	s8 *mem = "Mem", *io = "I/O";

	switch( pFdkUiProperty->fdkHwFunc ) {

		case KHF_PCI:
			dataPtr = (u8 *)&pFdkUiProperty->pFdkCommPkt->fdkRspPciReadPkt.pciContent;
			break;

		case KHF_IO:
			dataPtr = (u8 *)&pFdkUiProperty->pFdkCommPkt->fdkRspIoReadPkt.ioContent;
			break;

		case KHF_IDE:
			dataPtr = (u8 *)&pFdkUiProperty->pFdkCommPkt->fdkRspIdeReadPkt.ideContent;
			break;

		case KHF_CMOS:
			dataPtr = (u8 *)&pFdkUiProperty->pFdkCommPkt->fdkRspCmosReadPkt.cmosContent;
			break;

		default:
		case KHF_MEM:
			dataPtr = (u8 *)&pFdkUiProperty->pFdkCommPkt->fdkRspMemReadPkt.memContent;
		break;
	}
	pDataPtr = dataPtr;

	// Terminate buffers
	valueBuf[ FDK_DUMP_VBUF_SZ ] = 0;
	asciiBuf[ FDK_DUMP_ABUF_SZ ] = 0;

	// Format data for value & ascii
	for( i = 0 ; i < FDK_DUMP_BYTE_PER_LINE ; i++ ) {
		 
		vp += sprintf( (s8 *)vp,
		"%2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X",
		dataPtr[ 0 ], dataPtr[ 1 ], dataPtr[ 2 ], dataPtr[ 3 ], dataPtr[ 4 ], dataPtr[ 5 ], dataPtr[ 6 ],
		dataPtr[ 7 ], dataPtr[ 8 ], dataPtr[ 9 ], dataPtr[ 10 ], dataPtr[ 11 ], dataPtr[ 12 ], dataPtr[ 13 ],
		dataPtr[ 14 ], dataPtr[ 15 ] );

		if( pFdkUiProperty->fdkHwFunc != KHF_PCI )
			ap += sprintf( (s8 *)ap,
			"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
			FDK_DUMP_ASCII_FILTER( dataPtr[ 0 ] ),
			FDK_DUMP_ASCII_FILTER( dataPtr[ 1 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 2 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 3 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 4 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 5 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 6 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 7 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 8 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 9 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 10 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 11 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 12 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 13 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 14 ] ),
        	FDK_DUMP_ASCII_FILTER( dataPtr[ 15 ] ) );

		// Move to next line
		dataPtr += FDK_DUMP_BYTE_PER_LINE;
	}

	if( pFdkUiProperty->fdkHwFunc == KHF_PCI ) {

		pFdkPciConfig = (fdkPciConfig_t *)pDataPtr;
		sprintf( (s8 *)ap,
			"VEN ID: %4.4Xh\n"
			"DEV ID: %4.4Xh\n\n"
			"Rev ID  : %2.2Xh\n"
			"Int Line: %2.2Xh\n"
			"Int Pin : %2.2Xh\n\n"
			"%s: %8.8Xh\n"
			"%s: %8.8Xh\n"
			"%s: %8.8Xh\n"
			"%s: %8.8Xh\n"
			"%s: %8.8Xh\n"
			"%s: %8.8Xh\n\n"
			"ROM: %8.8Xh\n",
			pFdkPciConfig->vendorId,
			pFdkPciConfig->deviceId,
			pFdkPciConfig->revisionId,
			pFdkPciConfig->intLine,
			pFdkPciConfig->intPin,

			(pFdkPciConfig->baseAddrReg0 & FDK_PCIBAR_IO) ? io : mem,
			(pFdkPciConfig->baseAddrReg0 & FDK_PCIBAR_IO) ?
				(pFdkPciConfig->baseAddrReg0 & FDK_PCIBAR_IOBA_MASK) :
				(pFdkPciConfig->baseAddrReg0 & FDK_PCIBAR_MEMBA_MASK),

			(pFdkPciConfig->baseAddrReg1 & FDK_PCIBAR_IO) ? io : mem,
            (pFdkPciConfig->baseAddrReg1 & FDK_PCIBAR_IO) ?
                (pFdkPciConfig->baseAddrReg1 & FDK_PCIBAR_IOBA_MASK) :
                (pFdkPciConfig->baseAddrReg1 & FDK_PCIBAR_MEMBA_MASK),

			(pFdkPciConfig->baseAddrReg2 & FDK_PCIBAR_IO) ? io : mem,
            (pFdkPciConfig->baseAddrReg2 & FDK_PCIBAR_IO) ?
                (pFdkPciConfig->baseAddrReg2 & FDK_PCIBAR_IOBA_MASK) :
                (pFdkPciConfig->baseAddrReg2 & FDK_PCIBAR_MEMBA_MASK),

			(pFdkPciConfig->baseAddrReg3 & FDK_PCIBAR_IO) ? io : mem,
            (pFdkPciConfig->baseAddrReg3 & FDK_PCIBAR_IO) ?
                (pFdkPciConfig->baseAddrReg3 & FDK_PCIBAR_IOBA_MASK) :
                (pFdkPciConfig->baseAddrReg3 & FDK_PCIBAR_MEMBA_MASK),

			(pFdkPciConfig->baseAddrReg4 & FDK_PCIBAR_IO) ? io : mem,
            (pFdkPciConfig->baseAddrReg4 & FDK_PCIBAR_IO) ?
                (pFdkPciConfig->baseAddrReg4 & FDK_PCIBAR_IOBA_MASK) :
                (pFdkPciConfig->baseAddrReg4 & FDK_PCIBAR_MEMBA_MASK),

			(pFdkPciConfig->baseAddrReg5 & FDK_PCIBAR_IO) ? io : mem,
            (pFdkPciConfig->baseAddrReg5 & FDK_PCIBAR_IO) ?
                (pFdkPciConfig->baseAddrReg5 & FDK_PCIBAR_IOBA_MASK) :
                (pFdkPciConfig->baseAddrReg5 & FDK_PCIBAR_MEMBA_MASK),

			pFdkPciConfig->expRomBaseAddr );
	}

	// Print value
	printWindowAt(
		pFdkUiProperty->fdkDumpPanel,
		value, 
		FDK_DUMP_BYTE_PER_LINE,
		FDK_DUMP_BUF_PER_LINE,
		FDK_DUMP_VALUE_LINE,
		FDK_DUMP_VALUE_COLUMN,
		WHITE_BLUE,
		"%s",
		valueBuf );

	// Print ASCII
	printWindowAt(
		pFdkUiProperty->fdkDumpPanel,
		ascii,
		FDK_DUMP_BYTE_PER_LINE,
		FDK_DUMP_BYTE_PER_LINE,
		FDK_DUMP_ASCII_LINE,
		FDK_DUMP_ASCII_COLUMN,
		WHITE_BLUE,
		"%s",
		asciiBuf );

	// Print Offset bar
	printWindowAt(
		pFdkUiProperty->fdkDumpPanel,
		offset, 
		FDK_STRING_NLINE,
		4,
		FDK_DUMP_OFF_LINE,
		FDK_DUMP_OFF_COLUMN,
		YELLOW_BLUE,
		"%4.4X",
		pFdkUiProperty->fdkDumpPanel.byteOffset );

	// Print base address & First/Second title
	switch( pFdkUiProperty->fdkHwFunc ) {

		default:
		case KHF_MEM:

			// Base address
			printWindowAt(
				pFdkUiProperty->fdkDumpPanel,
				baseaddr, 
				FDK_STRING_NLINE,
				20,
				FDK_DUMP_BASEADDR_LINE,
				strlen( pFdkUiProperty->fdkDumpPanel.infoStr ),
				WHITE_BLUE,
				FDK_INFO_MEMORY_BASE_FMT,
				(u32)(pFdkUiProperty->fdkDumpPanel.byteBase >> 32),
				(u32)(pFdkUiProperty->fdkDumpPanel.byteBase & 0xFFFFFFFFULL) );
			break;

		case KHF_IO:

			// Base address
			printWindowAt(
				pFdkUiProperty->fdkDumpPanel,
				baseaddr, 
				FDK_STRING_NLINE,
				5,
				FDK_DUMP_BASEADDR_LINE,
				strlen( pFdkUiProperty->fdkDumpPanel.infoStr ),
				WHITE_BLUE,
				FDK_INFO_IO_BASE_FMT,
				(u32)(pFdkUiProperty->fdkDumpPanel.byteBase & 0x0000FFFFULL) );
			break;

		case KHF_PCI:

			// Base address
			pFdkPciDev = getPciDevice( pFdkUiProperty, 
							pFdkUiProperty->fdkDumpPanel.byteBase );
			if( pFdkPciDev )
				printWindowAt(
					pFdkUiProperty->fdkDumpPanel,
					baseaddr, 
					FDK_STRING_NLINE,
					29,
					FDK_DUMP_BASEADDR_LINE,
					strlen( pFdkUiProperty->fdkDumpPanel.infoStr ),
					WHITE_BLUE,
					FDK_INFO_PCI_BASE_FMT,
					pFdkPciDev->bus, pFdkPciDev->dev, pFdkPciDev->fun );

			// PCI first/second title
			printWindowMove(
				pFdkUiProperty->fdkDumpPanel,
				ftitle, 
				FDK_STRING_NLINE,
				FDK_MAX_PCINAME,
				FDK_DUMP_FTITLE_LINE,
				FDK_DUMP_FTITLE_COLUMN,
				WHITE_BLUE,
				"%s: %s",
				FDK_FTITLE_PCI,
				(pFdkUiProperty->pFdkPciIds + pFdkUiProperty->fdkDumpPanel.byteBase)->venTxt );

			printWindowMove(
				pFdkUiProperty->fdkDumpPanel,
				stitle, 
				FDK_STRING_NLINE,
				FDK_MAX_PCINAME,
				FDK_DUMP_STITLE_LINE,
				FDK_DUMP_FTITLE_COLUMN,
				WHITE_BLUE,
				"%s: %s",
				FDK_STITLE_PCI,
				(pFdkUiProperty->pFdkPciIds + pFdkUiProperty->fdkDumpPanel.byteBase)->devTxt );
			break;

		case KHF_PCIL:
			break;

		case KHF_IDE:

			// Base address
			printWindowAt(
				pFdkUiProperty->fdkDumpPanel,
				baseaddr, 
				FDK_STRING_NLINE,
				20,
				FDK_DUMP_BASEADDR_LINE,
				strlen( pFdkUiProperty->fdkDumpPanel.infoStr ),
				WHITE_BLUE,
				FDK_INFO_IDE_BASE_FMT,
				(u32)(pFdkUiProperty->fdkDumpPanel.byteBase >> 32),
				(u32)(pFdkUiProperty->fdkDumpPanel.byteBase & 0xFFFFFFFFULL) );
			break;

		case KHF_CMOS:

			// Base address
			printWindowAt(
				pFdkUiProperty->fdkDumpPanel,
				baseaddr, 
				FDK_STRING_NLINE,
				20,
				FDK_DUMP_BASEADDR_LINE,
				strlen( pFdkUiProperty->fdkDumpPanel.infoStr ),
				WHITE_BLUE,
				FDK_INFO_CMOS_BASE_FMT,
				(u8)(pFdkUiProperty->fdkDumpPanel.byteBase & 0xFFULL) );
			break;
	}


	// Highlight & Editing
	y = (pFdkUiProperty->fdkDumpPanel.byteOffset / FDK_DUMP_BYTE_PER_LINE) + FDK_DUMP_VALUE_LINE;
	x = ((pFdkUiProperty->fdkDumpPanel.byteOffset % FDK_DUMP_BYTE_PER_LINE) * 3) + FDK_DUMP_VALUE_COLUMN;
	if( pFdkUiProperty->fdkDumpPanel.toggleEditing ) {

		color = (editorColorCount++ % 2) ? YELLOW_RED : YELLOW_BLACK;

		printWindowMove(
			pFdkUiProperty->fdkDumpPanel,
			highlight,
			FDK_STRING_NLINE,
			FDK_DUMP_HL_DIGITS,
			y,
			x,
			color,
			"%2.2X",
			pFdkUiProperty->fdkDumpPanel.editingBuf );
		}
	else {

		printWindowMove(
			pFdkUiProperty->fdkDumpPanel,
			highlight,
			FDK_STRING_NLINE,
			FDK_DUMP_HL_DIGITS,
			y,
			x,
			YELLOW_RED,
			"%2.2X",
			*(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset) );

		pFdkUiProperty->fdkDumpPanel.editingBuf = *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset);
	}


	// Bits
	if( pFdkUiProperty->fdkDumpPanel.toggleBits ) {

		printWindowMove(
			pFdkUiProperty->fdkDumpPanel,
			bits,
			FDK_STRING_NLINE,
			FDK_DUMP_BITS_DIGITS,
			y + 1,
			x,
			WHITE_RED,
			"%d%d%d%d_%d%d%d%d",
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 7 ),			
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 6 ),
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 5 ),
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 4 ),
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 3 ),
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 2 ),
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 1 ),
			FDK_GET_BIT( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset), 0 ) );
	}
	else
		destroyWindow( pFdkUiProperty->fdkDumpPanel, bits );


	// Print ASCII highlight
	if( pFdkUiProperty->fdkHwFunc != KHF_PCI ) {
    	y = (pFdkUiProperty->fdkDumpPanel.byteOffset / FDK_DUMP_BYTE_PER_LINE) + FDK_DUMP_ASCII_LINE;
    	x = (pFdkUiProperty->fdkDumpPanel.byteOffset % FDK_DUMP_BYTE_PER_LINE) + FDK_DUMP_ASCII_COLUMN;
		printWindowMove(
			pFdkUiProperty->fdkDumpPanel,
			hlascii,
			FDK_STRING_NLINE,
			FDK_DUMP_HLA_DIGITS,
			y,
			x,
			YELLOW_RED,
			"%c",
			FDK_DUMP_ASCII_FILTER( *(pDataPtr + pFdkUiProperty->fdkDumpPanel.byteOffset) ) );
	}
}


void clearDumpBasePanel( fdkUiProperty_t *pFdkUiProperty ) {

	destroyWindow( pFdkUiProperty->fdkDumpPanel, top );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, rtop );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, left );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, info );
}


void clearDumpUpdatePanel( fdkUiProperty_t *pFdkUiProperty ) {

	destroyWindow( pFdkUiProperty->fdkDumpPanel, value );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, ascii );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, offset );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, baseaddr );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, highlight );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, hlascii );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, bits );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, ftitle );
	destroyWindow( pFdkUiProperty->fdkDumpPanel, stitle );
}


void handleKeyPressForDumpPanel( fdkUiProperty_t *pFdkUiProperty ) {

	if( pFdkUiProperty->fdkDumpPanel.toggleEditing ) {

		// Trigger write action
		if( pFdkUiProperty->inputBuf == KBPRS_ENTER ) {

			switch( pFdkUiProperty->fdkHwFunc ) {

				case KHF_MEM:
					writeMemoryByEditing( pFdkUiProperty );
					break;

				case KHF_IO:
					writeIoByEditing( pFdkUiProperty );
					break;

				case KHF_PCI:
					writePciByEditing( pFdkUiProperty );
					break;

				case KHF_IDE:
					writeIdeByEditing( pFdkUiProperty );
					break;

				case KHF_CMOS:
					writeCmosByEditing( pFdkUiProperty );
					break;

				default:
					break;
			}

			// Exit
			pFdkUiProperty->fdkDumpPanel.toggleEditing = 0;
			return;
		}

		// Editing
		if( (pFdkUiProperty->inputBuf >= '0' 
			&& pFdkUiProperty->inputBuf <= '9')
			|| (pFdkUiProperty->inputBuf >= 'a'
			&& pFdkUiProperty->inputBuf <= 'f')
			|| (pFdkUiProperty->inputBuf >= 'A'
			&& pFdkUiProperty->inputBuf <= 'F') ) {

			// Left shift 4 bits
			pFdkUiProperty->fdkDumpPanel.editingBuf <<= 4;

			if( pFdkUiProperty->inputBuf <= '9' ) {

				pFdkUiProperty->fdkDumpPanel.editingBuf |=
					(u8)((pFdkUiProperty->inputBuf - 0x30) & 0x0F);
			}
			else if( pFdkUiProperty->inputBuf > 'F' ) {

				pFdkUiProperty->fdkDumpPanel.editingBuf |=
					(u8)((pFdkUiProperty->inputBuf - 0x60 + 9) & 0x0F);
			}
			else {

				pFdkUiProperty->fdkDumpPanel.editingBuf |=
					(u8)((pFdkUiProperty->inputBuf - 0x40 + 9) & 0x0F);
			}
		}

		return;
	}

	switch( pFdkUiProperty->inputBuf ) {

		case KBPRS_UP:

			if( (pFdkUiProperty->fdkDumpPanel.byteOffset - FDK_DUMP_BYTE_PER_LINE) >= 0 )
				pFdkUiProperty->fdkDumpPanel.byteOffset -= FDK_DUMP_BYTE_PER_LINE;
			else
				pFdkUiProperty->fdkDumpPanel.byteOffset = FDK_BYTE_PER_SCREEN - (FDK_DUMP_BYTE_PER_LINE - pFdkUiProperty->fdkDumpPanel.byteOffset);
            break;

		case KBPRS_DOWN:

			if( (pFdkUiProperty->fdkDumpPanel.byteOffset + FDK_DUMP_BYTE_PER_LINE) < FDK_BYTE_PER_SCREEN )
				pFdkUiProperty->fdkDumpPanel.byteOffset += FDK_DUMP_BYTE_PER_LINE;
			else
				pFdkUiProperty->fdkDumpPanel.byteOffset %= FDK_DUMP_BYTE_PER_LINE;
            break;

		case KBPRS_LEFT:

			if( (pFdkUiProperty->fdkDumpPanel.byteOffset - 1) >= 0 && (pFdkUiProperty->fdkDumpPanel.byteOffset % FDK_DUMP_BYTE_PER_LINE) > 0 )
				pFdkUiProperty->fdkDumpPanel.byteOffset--;
			else
				pFdkUiProperty->fdkDumpPanel.byteOffset += (FDK_DUMP_BYTE_PER_LINE - 1);
            break;

		case KBPRS_RIGHT:

			if( (pFdkUiProperty->fdkDumpPanel.byteOffset + 1) < FDK_BYTE_PER_SCREEN 
				&& ((pFdkUiProperty->fdkDumpPanel.byteOffset + 1) % FDK_DUMP_BYTE_PER_LINE) )
				pFdkUiProperty->fdkDumpPanel.byteOffset++;
			else
				pFdkUiProperty->fdkDumpPanel.byteOffset -= (FDK_DUMP_BYTE_PER_LINE - 1);
            break;

		case KBPRS_PGUP:

			switch( pFdkUiProperty->fdkHwFunc ) {

				case KHF_PCI:
					if( !pFdkUiProperty->fdkDumpPanel.byteBase )
						pFdkUiProperty->fdkDumpPanel.byteBase = pFdkUiProperty->numOfPciDevice - 1;
					else
						pFdkUiProperty->fdkDumpPanel.byteBase--;
					break;

				case KHF_MEM:
					pFdkUiProperty->fdkDumpPanel.byteBase -= FDK_BYTE_PER_SCREEN;
					if( pFdkUiProperty->fdkDumpPanel.byteBase >= FDK_MAXADDR_MEM )
						pFdkUiProperty->fdkDumpPanel.byteBase =
							(pFdkUiProperty->fdkDumpPanel.byteBase & 0xFFULL)
							| (FDK_MAXADDR_MEM & ~0xFFULL);
					break;

				case KHF_IO:
					pFdkUiProperty->fdkDumpPanel.byteBase -= FDK_BYTE_PER_SCREEN;
					if( pFdkUiProperty->fdkDumpPanel.byteBase >= FDK_MAXADDR_IO )
						pFdkUiProperty->fdkDumpPanel.byteBase =
							(pFdkUiProperty->fdkDumpPanel.byteBase & 0xFFULL)
							| (FDK_MAXADDR_IO & ~0xFFULL);
					break;

				default:
					break;
			}
				pFdkUiProperty->fdkDumpPanel.byteBase -= FDK_BYTE_PER_SCREEN;
            break;

		case KBPRS_PGDN:

			switch( pFdkUiProperty->fdkHwFunc ) {

				case KHF_PCI:
					if( (pFdkUiProperty->fdkDumpPanel.byteBase + 1) 
						>= pFdkUiProperty->numOfPciDevice )
						pFdkUiProperty->fdkDumpPanel.byteBase = 0;
					else
						pFdkUiProperty->fdkDumpPanel.byteBase++;
					break;

				case KHF_MEM:
					pFdkUiProperty->fdkDumpPanel.byteBase += FDK_BYTE_PER_SCREEN;
					if( pFdkUiProperty->fdkDumpPanel.byteBase >= FDK_MAXADDR_MEM )
						pFdkUiProperty->fdkDumpPanel.byteBase &= 0xFFULL;
					break;

				case KHF_IO:
					pFdkUiProperty->fdkDumpPanel.byteBase += FDK_BYTE_PER_SCREEN;
					if( pFdkUiProperty->fdkDumpPanel.byteBase >= FDK_MAXADDR_IO )
						pFdkUiProperty->fdkDumpPanel.byteBase &= 0xFFULL;
					break;

				default:
					break;
			}
            break;

		case KBPRS_ENTER:

			pFdkUiProperty->fdkDumpPanel.toggleEditing = 1;
			break;

		case KBPRS_SPACE:

			pFdkUiProperty->fdkDumpPanel.toggleBits = !pFdkUiProperty->fdkDumpPanel.toggleBits;
			break;

		default:
			break;
	}
}


