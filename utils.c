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
#include <packet.h>

#include <ncurses.h>
#include <panel.h>

#include <fdk.h>
#include <cfdk.h>


s32 connectToFdkServer( fdkUiProperty_t *pFdkUiProperty ) {

	// Connect to FDK server
	return executeFunction( 
			pFdkUiProperty->fd,
			FDK_REQ_CONNECT,
			0,
			0,
			NULL,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 readPciList( fdkUiProperty_t *pFdkUiProperty ) {

	fdkRspPciListPkt_t *pFdkRspPciListPkt;
	s32 i;

	// Read PCI list
	if( executeFunction( pFdkUiProperty->fd, FDK_REQ_PCI_LIST, 0, 0, NULL, pFdkUiProperty->pktBuf, FDK_MAXSZ_PKT ) )
		return 1;

	// Save PCI list
	pFdkRspPciListPkt = (fdkRspPciListPkt_t *)pFdkUiProperty->pktBuf;
	pFdkUiProperty->numOfPciDevice = pFdkRspPciListPkt->numOfPciDevice;
	pFdkUiProperty->pFdkPciDev = (fdkPciDev_t *)malloc( 
		sizeof( fdkPciDev_t ) * pFdkUiProperty->numOfPciDevice );

	if( !pFdkUiProperty->pFdkPciDev )
		return 1;

	memcpy( pFdkUiProperty->pFdkPciDev, 
			&pFdkRspPciListPkt->pciListContent, 
			sizeof( fdkPciDev_t ) * pFdkUiProperty->numOfPciDevice );

	pFdkUiProperty->pFdkPciIds = (fdkPciIds_t *)malloc( 
		sizeof( fdkPciIds_t ) * pFdkUiProperty->numOfPciDevice );

	if( !pFdkUiProperty->pFdkPciIds )
		return 1;

	// Read PCI IDs
	for( i = 0 ; i < pFdkUiProperty->numOfPciDevice ; i++ ) {

		getPciVenDevTexts( 
			(pFdkUiProperty->pFdkPciDev + i)->vendorId,
			(pFdkUiProperty->pFdkPciDev + i)->deviceId,
			(pFdkUiProperty->pFdkPciIds + i)->venTxt,
			(pFdkUiProperty->pFdkPciIds + i)->devTxt,
			pFdkUiProperty->pciIdsPath );
	}

	return 0;
}


fdkPciDev_t *getPciDevice( fdkUiProperty_t *pFdkUiProperty, s32 num ) {

	if( num >= pFdkUiProperty->numOfPciDevice )
		return NULL;

	return pFdkUiProperty->pFdkPciDev + num;
}


s32 readE820List( fdkUiProperty_t *pFdkUiProperty ) {

	fdkRspE820ListPkt_t *pFdkRspE820ListPkt;

	// Read E820 list
	if( executeFunction( pFdkUiProperty->fd, FDK_REQ_E820_LIST, 0, 0, NULL, pFdkUiProperty->pktBuf, FDK_MAXSZ_PKT ) )
		return 1;

	// Save E820 list
	pFdkRspE820ListPkt = (fdkRspE820ListPkt_t *)pFdkUiProperty->pktBuf;
	pFdkUiProperty->numOfE820Record = pFdkRspE820ListPkt->numOfE820Record;
	pFdkUiProperty->pFdkE820record = (fdkE820record_t *)malloc( sizeof( fdkE820record_t ) * pFdkUiProperty->numOfE820Record );

	if( !pFdkUiProperty->pFdkE820record )
		return 1;

	memcpy( pFdkUiProperty->pFdkE820record, &pFdkRspE820ListPkt->e820ListContent, sizeof( fdkE820record_t ) * pFdkUiProperty->numOfE820Record );

	return 0;
}


s32 readMemory( fdkUiProperty_t *pFdkUiProperty ) {

	// Read memory
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_MEM_READ,
			pFdkUiProperty->fdkDumpPanel.byteBase,
			FDK_BYTE_PER_SCREEN,
			NULL,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 writeMemoryByEditing( fdkUiProperty_t *pFdkUiProperty ) {

	// Write memory
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_MEM_WRITE,
			pFdkUiProperty->fdkDumpPanel.byteBase + pFdkUiProperty->fdkDumpPanel.byteOffset,
			sizeof( pFdkUiProperty->fdkDumpPanel.editingBuf ),
			&pFdkUiProperty->fdkDumpPanel.editingBuf,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 readIo( fdkUiProperty_t *pFdkUiProperty ) {

	// Read io
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_IO_READ,
			pFdkUiProperty->fdkDumpPanel.byteBase,
			FDK_BYTE_PER_SCREEN,
			NULL,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 writeIoByEditing( fdkUiProperty_t *pFdkUiProperty ) {

	// Write io
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_IO_WRITE,
			pFdkUiProperty->fdkDumpPanel.byteBase + pFdkUiProperty->fdkDumpPanel.byteOffset,
			sizeof( pFdkUiProperty->fdkDumpPanel.editingBuf ),
			&pFdkUiProperty->fdkDumpPanel.editingBuf,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 readIde( fdkUiProperty_t *pFdkUiProperty ) {

	// Read ide
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_IDE_READ,
			pFdkUiProperty->fdkDumpPanel.byteBase,
			FDK_BYTE_PER_SCREEN,
			NULL,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 writeIdeByEditing( fdkUiProperty_t *pFdkUiProperty ) {

	// Write ide
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_IDE_WRITE,
			pFdkUiProperty->fdkDumpPanel.byteBase + pFdkUiProperty->fdkDumpPanel.byteOffset,
			sizeof( pFdkUiProperty->fdkDumpPanel.editingBuf ),
			&pFdkUiProperty->fdkDumpPanel.editingBuf,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 readCmos( fdkUiProperty_t *pFdkUiProperty ) {

	// Read cmos
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_CMOS_READ,
			pFdkUiProperty->fdkDumpPanel.byteBase,
			FDK_BYTE_PER_SCREEN,
			NULL,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 writeCmosByEditing( fdkUiProperty_t *pFdkUiProperty ) {

	// Write cmos
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_CMOS_WRITE,
			pFdkUiProperty->fdkDumpPanel.byteBase + pFdkUiProperty->fdkDumpPanel.byteOffset,
			sizeof( pFdkUiProperty->fdkDumpPanel.editingBuf ),
			&pFdkUiProperty->fdkDumpPanel.editingBuf,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 readPci( fdkUiProperty_t *pFdkUiProperty ) {

	fdkPciDev_t *pFdkPciDev;

	// Get PCI device
	pFdkPciDev = getPciDevice( 
		pFdkUiProperty, 
		pFdkUiProperty->fdkDumpPanel.byteBase );
	if( !pFdkPciDev )
		return 1;

	// Read PCI
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_PCI_READ,
			calculatePciAddress( 
				pFdkPciDev->bus, 
				pFdkPciDev->dev, 
				pFdkPciDev->fun ),
			FDK_BYTE_PER_SCREEN,
			NULL,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
}


s32 writePciByEditing( fdkUiProperty_t *pFdkUiProperty ) {

	fdkPciDev_t *pFdkPciDev;

    // Get PCI device
	pFdkPciDev = getPciDevice(
		pFdkUiProperty,
		pFdkUiProperty->fdkDumpPanel.byteBase );
	if( !pFdkPciDev )
		return 1;

	// Write PCI
	return executeFunction(
			pFdkUiProperty->fd,
			FDK_REQ_PCI_WRITE,
			calculatePciAddress(
				pFdkPciDev->bus,
				pFdkPciDev->dev,
				pFdkPciDev->fun ) + pFdkUiProperty->fdkDumpPanel.byteOffset,
			sizeof( pFdkUiProperty->fdkDumpPanel.editingBuf ),
			&pFdkUiProperty->fdkDumpPanel.editingBuf,
			pFdkUiProperty->pktBuf,
			FDK_MAXSZ_PKT );
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
    s8 lineBuf[ FDK_MAX_READBUF ];

    // Open the PCI IDS file
    fd = open( pciids, O_RDONLY );
    if( fd < 0 )
        return 1;

    for( findven = 0, done = 0 ; ; ) {

        // Parse PCI Database file
        switch( tabs = readLine( fd, lineBuf, FDK_MAX_READBUF ) ) {

            case 0:
                if( !compartId( venid, lineBuf ) ) {

                    strncpy( ventxt, (lineBuf + 6), FDK_MAX_PCINAME );
                    findven = 1;
                }
                break;

            case 1:
                if( findven && !compartId( devid, lineBuf ) ) {

                    strncpy( devtxt, (lineBuf + 6), FDK_MAX_PCINAME );
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


