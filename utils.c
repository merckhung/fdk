/*
 * Copyright (C) 2011 Olux Organization All rights reserved.
 * Author: Merck Hung <merck@gmail.com>
 *
 * File: utils.c
 * Description:
 *	OluxOS Kernel Debugger
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>

#include <otypes.h>
#include <packet.h>

#include <ncurses.h>
#include <panel.h>

#include <lfdk.h>


s32 connectToOluxOSKernel( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Connect to OluxOS Kernel
	return executeFunction( 
			pKdbgerUiProperty->fd,
			KDBGER_REQ_CONNECT,
			0,
			0,
			NULL,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 readPciList( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	kdbgerRspPciListPkt_t *pKdbgerRspPciListPkt;
	s32 i;

	// Read PCI list
	if( executeFunction( pKdbgerUiProperty->fd, KDBGER_REQ_PCI_LIST, 0, 0, NULL, pKdbgerUiProperty->pktBuf, KDBGER_MAXSZ_PKT ) )
		return 1;

	// Save PCI list
	pKdbgerRspPciListPkt = (kdbgerRspPciListPkt_t *)pKdbgerUiProperty->pktBuf;
	pKdbgerUiProperty->numOfPciDevice = pKdbgerRspPciListPkt->numOfPciDevice;
	pKdbgerUiProperty->pKdbgerPciDev = (kdbgerPciDev_t *)malloc( 
		sizeof( kdbgerPciDev_t ) * pKdbgerUiProperty->numOfPciDevice );

	if( !pKdbgerUiProperty->pKdbgerPciDev )
		return 1;

	memcpy( pKdbgerUiProperty->pKdbgerPciDev, 
			&pKdbgerRspPciListPkt->pciListContent, 
			sizeof( kdbgerPciDev_t ) * pKdbgerUiProperty->numOfPciDevice );

	pKdbgerUiProperty->pKdbgerPciIds = (kdbgerPciIds_t *)malloc( 
		sizeof( kdbgerPciIds_t ) * pKdbgerUiProperty->numOfPciDevice );

	if( !pKdbgerUiProperty->pKdbgerPciIds )
		return 1;

	// Read PCI IDs
	for( i = 0 ; i < pKdbgerUiProperty->numOfPciDevice ; i++ ) {

		getPciVenDevTexts( 
			(pKdbgerUiProperty->pKdbgerPciDev + i)->vendorId,
			(pKdbgerUiProperty->pKdbgerPciDev + i)->deviceId,
			(pKdbgerUiProperty->pKdbgerPciIds + i)->venTxt,
			(pKdbgerUiProperty->pKdbgerPciIds + i)->devTxt,
			pKdbgerUiProperty->pciIdsPath );
	}

	return 0;
}


kdbgerPciDev_t *getPciDevice( kdbgerUiProperty_t *pKdbgerUiProperty, s32 num ) {

	if( num >= pKdbgerUiProperty->numOfPciDevice )
		return NULL;

	return pKdbgerUiProperty->pKdbgerPciDev + num;
}


s32 readE820List( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	kdbgerRspE820ListPkt_t *pKdbgerRspE820ListPkt;

	// Read E820 list
	if( executeFunction( pKdbgerUiProperty->fd, KDBGER_REQ_E820_LIST, 0, 0, NULL, pKdbgerUiProperty->pktBuf, KDBGER_MAXSZ_PKT ) )
		return 1;

	// Save E820 list
	pKdbgerRspE820ListPkt = (kdbgerRspE820ListPkt_t *)pKdbgerUiProperty->pktBuf;
	pKdbgerUiProperty->numOfE820Record = pKdbgerRspE820ListPkt->numOfE820Record;
	pKdbgerUiProperty->pKdbgerE820record = (kdbgerE820record_t *)malloc( sizeof( kdbgerE820record_t ) * pKdbgerUiProperty->numOfE820Record );

	if( !pKdbgerUiProperty->pKdbgerE820record )
		return 1;

	memcpy( pKdbgerUiProperty->pKdbgerE820record, &pKdbgerRspE820ListPkt->e820ListContent, sizeof( kdbgerE820record_t ) * pKdbgerUiProperty->numOfE820Record );

	return 0;
}


s32 readMemory( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Read memory
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_MEM_READ,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase,
			KDBGER_BYTE_PER_SCREEN,
			NULL,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 writeMemoryByEditing( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Write memory
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_MEM_WRITE,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase + pKdbgerUiProperty->kdbgerDumpPanel.byteOffset,
			sizeof( pKdbgerUiProperty->kdbgerDumpPanel.editingBuf ),
			&pKdbgerUiProperty->kdbgerDumpPanel.editingBuf,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 readIo( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Read io
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_IO_READ,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase,
			KDBGER_BYTE_PER_SCREEN,
			NULL,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 writeIoByEditing( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Write io
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_IO_WRITE,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase + pKdbgerUiProperty->kdbgerDumpPanel.byteOffset,
			sizeof( pKdbgerUiProperty->kdbgerDumpPanel.editingBuf ),
			&pKdbgerUiProperty->kdbgerDumpPanel.editingBuf,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 readIde( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Read ide
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_IDE_READ,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase,
			KDBGER_BYTE_PER_SCREEN,
			NULL,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 writeIdeByEditing( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Write ide
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_IDE_WRITE,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase + pKdbgerUiProperty->kdbgerDumpPanel.byteOffset,
			sizeof( pKdbgerUiProperty->kdbgerDumpPanel.editingBuf ),
			&pKdbgerUiProperty->kdbgerDumpPanel.editingBuf,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 readCmos( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Read cmos
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_CMOS_READ,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase,
			KDBGER_BYTE_PER_SCREEN,
			NULL,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 writeCmosByEditing( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	// Write cmos
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_CMOS_WRITE,
			pKdbgerUiProperty->kdbgerDumpPanel.byteBase + pKdbgerUiProperty->kdbgerDumpPanel.byteOffset,
			sizeof( pKdbgerUiProperty->kdbgerDumpPanel.editingBuf ),
			&pKdbgerUiProperty->kdbgerDumpPanel.editingBuf,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 readPci( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	kdbgerPciDev_t *pKdbgerPciDev;

	// Get PCI device
	pKdbgerPciDev = getPciDevice( 
		pKdbgerUiProperty, 
		pKdbgerUiProperty->kdbgerDumpPanel.byteBase );
	if( !pKdbgerPciDev )
		return 1;

	// Read PCI
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_PCI_READ,
			calculatePciAddress( 
				pKdbgerPciDev->bus, 
				pKdbgerPciDev->dev, 
				pKdbgerPciDev->fun ),
			KDBGER_BYTE_PER_SCREEN,
			NULL,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


s32 writePciByEditing( kdbgerUiProperty_t *pKdbgerUiProperty ) {

	kdbgerPciDev_t *pKdbgerPciDev;

    // Get PCI device
	pKdbgerPciDev = getPciDevice(
		pKdbgerUiProperty,
		pKdbgerUiProperty->kdbgerDumpPanel.byteBase );
	if( !pKdbgerPciDev )
		return 1;

	// Write PCI
	return executeFunction(
			pKdbgerUiProperty->fd,
			KDBGER_REQ_PCI_WRITE,
			calculatePciAddress(
				pKdbgerPciDev->bus,
				pKdbgerPciDev->dev,
				pKdbgerPciDev->fun ) + pKdbgerUiProperty->kdbgerDumpPanel.byteOffset,
			sizeof( pKdbgerUiProperty->kdbgerDumpPanel.editingBuf ),
			&pKdbgerUiProperty->kdbgerDumpPanel.editingBuf,
			pKdbgerUiProperty->pktBuf,
			KDBGER_MAXSZ_PKT );
}


u32 calculatePciAddress( u16 bus, u8 dev, u8 func ) {

	return 0x80000000 
		| (((u32)bus) << 16) 
		| ((((u32)dev) & 0x1F) << 11) 
		| ((((u32)func) & 0x07) << 8);
}


static s32 readLine( s32 fd, s8 *lineBuf, s32 len ) {

    s8 buf;
    s32 i, tabs;

    // Clear buffer
    memset( lineBuf, 0, len );

    // Reading a line
    for( i = 0, tabs = 0 ; read( fd, &buf, 1 ) ; ) {

        if( buf == '#' ) {

            // Skip comment
            while( read( fd, &buf, 1 ) && (buf != '\n') );
            return -1;
        }
        else if( buf == '\n' ) {

            if( strlen( lineBuf ) )
                return tabs;
            return -1;
        }
        else if( buf == '\t' )
            tabs++;
        else {

            lineBuf[ i ] = buf;
            i++;
        }
    }

    return -2;
}


static s32 compartId( u32 id, s8 *lineBuf ) {

    s8 idstr[ 5 ];
    s8 temp[ 5 ];

    // Read ID string
    strncpy( idstr, lineBuf, sizeof( idstr ) );

    // Convert binary to ASCII
    snprintf( temp, sizeof( temp ), "%4.4x", id );

    return strncmp( temp, idstr, strlen( temp ) );
}


s32 getPciVenDevTexts( u16 venid, u16 devid, s8 *ventxt, s8 *devtxt, s8 *pciids ) {

    s32 fd, tabs, done, findven;
    s8 lineBuf[ KDBGER_MAX_READBUF ];

    // Open the PCI IDS file
    fd = open( pciids, O_RDONLY );
    if( fd < 0 )
        return 1;

    for( findven = 0, done = 0 ; ; ) {

        // Parse PCI Database file
        switch( tabs = readLine( fd, lineBuf, KDBGER_MAX_READBUF ) ) {

            case 0:
                if( !compartId( venid, lineBuf ) ) {

                    strncpy( ventxt, (lineBuf + 6), KDBGER_MAX_PCINAME );
                    findven = 1;
                }
                break;

            case 1:
                if( findven && !compartId( devid, lineBuf ) ) {

                    strncpy( devtxt, (lineBuf + 6), KDBGER_MAX_PCINAME );
                    done = 1;
                }
                break;

            default:
                break;
        }

        if( done )
            break;

        // End of File
        if( tabs == -2 )
            break;
    }

    // Close the file
    close( fd );

    return 0;
}


static s8 convertDWordToByte( u32 *Data, u32 Offset ) {

    u32 tmp, off, bs;


    off = Offset / 4;
    bs = Offset % 4;
    if( bs ) {

        tmp = *(Data + off);
        tmp = ((tmp >> (bs * 8)) & 0xFF);
        return tmp;
    }

    tmp = ((*(Data + off)) & 0xFF);
    return tmp;
}


void dumpData( u32 *Data, u32 Length, u32 BaseAddr ) {

    u32 i, j;
    u32 c;


    printf( "\n\n== Dump Memory Start ==\n\n" );
    printf( " Address | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|   ASCII DATA   \n" );
    printf( "---------------------------------------------------------------------------\n" );


    for( i = 0 ; i <= Length ; i++ ) {


        if( !(i % 16) ) {


            if( (i > 15) ) {

                for( j = i - 16 ; j < i ; j++ ) {

                    c = convertDWordToByte( Data, j );
                    if( ((c >= '!') && (c <= '~')) ) {

                        printf( "%c", c );
                    }
                    else {

                        printf( "." );
                    }
                }
            }

            if( i ) {

                printf( "\n" );
            }


            if( i == Length ) {

                break;
            }


            printf( "%8.8X : ", i + BaseAddr );
        }


        printf( "%2.2X ", convertDWordToByte( Data, i ) & 0xFF );
    }


    printf( "---------------------------------------------------------------------------\n" );
    printf( "\n== Dump Memory End ==\n\n" );
}


s32 isIPv4Format( const s8 *str ) {

#define IP_STR_LEN_MIN              7
#define IP_STR_LEN                  15
#define IP_STR_BUF                  (IP_STR_LEN + 1)
    
    s32 i, j, pos[ 3 ], len;
    s8 buf[ IP_STR_BUF ];
    u32 addr[ 4 ];
    s32 valid = 0;
    
    // Length check
    len = strlen( str );
    if( len > IP_STR_LEN || len < IP_STR_LEN_MIN )
        return -1;
    strncpy( buf, str, IP_STR_BUF );
    
    // Look for positions of delimiters
    for( i = 0, j = 0 ; i < len ; i++ ) {
        
        if( buf[ i ] == '.' ) {
            
            // Exceed the limit
            if( j >= 3 )
                return -1;
            
            // Record & Terminate string
            pos[ j ] = i;
            buf[ i ] = 0;
            j++;
        }
    }
    
    // Must have 3 dots at least or at most
    if( j != 3 )
        return -1;
    
    // Convert strings
    addr[ 0 ] = strtol( (const char *)&buf[ 0 ], NULL, 10 );
    addr[ 1 ] = strtol( (const char *)&buf[ pos[ 0 ] + 1 ], NULL, 10 );
    addr[ 2 ] = strtol( (const char *)&buf[ pos[ 1 ] + 1 ], NULL, 10 );
    addr[ 3 ] = strtol( (const char *)&buf[ pos[ 2 ] + 1 ], NULL, 10 );
    
    // Check range
    for( i = 0 ; i < 4 ; i++ )
        if( addr[ i ] > 255 ) {
            
            valid = -1;
            break;
        }
    
    // Not ZERO
    snprintf( buf, IP_STR_BUF, "%d.%d.%d.%d", addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
    if( strcmp( buf, str ) )
        valid = -1;
    
    return valid;
}


int countLinklist( commonLinklist_t *head ) {
    
    int i;
    
    for( i = 0 ; head ; head = head->next, i++ );
    return i;
}


commonLinklist_t **tailOfLinklist( commonLinklist_t **head ) {
    
    commonLinklist_t **ppCommonLinklist;
    
    if( !*head )
        return head;
    
    for( ppCommonLinklist = head ;
        (*ppCommonLinklist)->next ;
        ppCommonLinklist = &((*ppCommonLinklist)->next) );
    
    return ppCommonLinklist;
}


void appendLinklist( commonLinklist_t **head, commonLinklist_t *object ) {
    
    commonLinklist_t **ppCommonLinklist;
    
    ppCommonLinklist = tailOfLinklist( head );
    if( *ppCommonLinklist )
        (*ppCommonLinklist)->next = object;
    else
        *head = object;
}


commonLinklist_t *retriveFirstLinklist( commonLinklist_t **head ) {
    
    commonLinklist_t *pCommonLinklist = *head;
    
    if( !*head )
        return NULL;
    
    if( (*head)->next )
        *head = (*head)->next;
    else
        *head = NULL;
    
    return pCommonLinklist;
}


void freeLinklist( commonLinklist_t *head ) {
    
    commonLinklist_t *prev;
    for( ; head ; ) {
        
        prev = head;
        head = head->next;
        free( prev );
    }
}


