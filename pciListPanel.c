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
#include <libcomm.h>
#include <packet.h>

#include <ncurses.h>
#include <panel.h>

#include <fdk.h>
#include <cfdk.h>


void printPciListBasePanel( fdkUiProperty_t *pFdkUiProperty ) {

	// Title
	printWindowAt( pFdkUiProperty->fdkPciListPanel,
		title, 
		FDK_PCIL_TITLE_LINE,
		FDK_PCIL_TITLE_COLUMN,
		FDK_PCIL_TITLE_X_POS,
		FDK_PCIL_TITLE_Y_POS,
		YELLOW_BLACK,
		FDK_PCIL_TITLE );
}


void printPciListUpdatePanel( fdkUiProperty_t *pFdkUiProperty ) {

	s8 *p, buf[ FDK_BUF_SIZE ], hlbuf[ FDK_PCIL_CON_COLUMN + 1 ];
	s32 i;
	s32 start, end;

	// Range
	start = pFdkUiProperty->fdkPciListPanel.pageOffset;
	end = pFdkUiProperty->fdkPciListPanel.pageOffset + FDK_REC_PER_PAGE;
	if( end > pFdkUiProperty->numOfPciDevice )
		end = pFdkUiProperty->numOfPciDevice;

	// Prepare data
	p = buf;
	for( i = start ; i < end ; i++ ) {

		// Prepare for the list
		p += snprintf( p, 
			(FDK_BUF_SIZE - (p - buf)),
			FDK_PCIL_LINE_FMT,
			(pFdkUiProperty->pFdkPciIds + i)->venTxt,
			(pFdkUiProperty->pFdkPciIds + i)->devTxt,
			(pFdkUiProperty->pFdkPciDev + i)->vendorId,
			(pFdkUiProperty->pFdkPciDev + i)->deviceId,
			(pFdkUiProperty->pFdkPciDev + i)->bus,
			(pFdkUiProperty->pFdkPciDev + i)->dev,
			(pFdkUiProperty->pFdkPciDev + i)->fun );

		// Prepare for the highlight bar
		if( i == (pFdkUiProperty->fdkPciListPanel.hlIndex + pFdkUiProperty->fdkPciListPanel.pageOffset) )
			snprintf( hlbuf,
				FDK_PCIL_CON_COLUMN + 1,
				FDK_PCIL_LINE_FMT,
				(pFdkUiProperty->pFdkPciIds + i)->venTxt,
				(pFdkUiProperty->pFdkPciIds + i)->devTxt,
				(pFdkUiProperty->pFdkPciDev + i)->vendorId,
				(pFdkUiProperty->pFdkPciDev + i)->deviceId,
				(pFdkUiProperty->pFdkPciDev + i)->bus,
				(pFdkUiProperty->pFdkPciDev + i)->dev,
				(pFdkUiProperty->pFdkPciDev + i)->fun );
	}

	// Update the screen
	printWindowAt( pFdkUiProperty->fdkPciListPanel,
		content, 
		FDK_PCIL_CON_LINE,
		FDK_PCIL_CON_COLUMN,
		FDK_PCIL_CON_X_POS,
		FDK_PCIL_CON_Y_POS,
		WHITE_BLUE,
		"%s",
		buf );

	// Highlight
	printWindowMove( pFdkUiProperty->fdkPciListPanel,
		highlight, 
		FDK_STRING_NLINE,
		FDK_PCIL_CON_COLUMN,
		FDK_PCIL_CON_X_POS + pFdkUiProperty->fdkPciListPanel.hlIndex,
		FDK_PCIL_CON_Y_POS,
		BLACK_CYAN,
		"%s",
		hlbuf );
}


void clearPciListBasePanel( fdkUiProperty_t *pFdkUiProperty ) {

	destroyWindow( pFdkUiProperty->fdkPciListPanel, title );
}


void clearPciListUpdatePanel( fdkUiProperty_t *pFdkUiProperty ) {

	destroyWindow( pFdkUiProperty->fdkPciListPanel, content );
	destroyWindow( pFdkUiProperty->fdkPciListPanel, highlight );
}


s32 handleKeyPressForPciListPanel( fdkUiProperty_t *pFdkUiProperty ) {

	switch( pFdkUiProperty->inputBuf ) {

		case KBPRS_UP:

			if( pFdkUiProperty->fdkPciListPanel.hlIndex )
				pFdkUiProperty->fdkPciListPanel.hlIndex--;
			else
				if( pFdkUiProperty->fdkPciListPanel.pageOffset )
					pFdkUiProperty->fdkPciListPanel.pageOffset--;
            break;

		case KBPRS_DOWN:

			if( pFdkUiProperty->fdkPciListPanel.hlIndex 
					< (FDK_REC_PER_PAGE - 1) 
				&& (pFdkUiProperty->fdkPciListPanel.pageOffset 
					+ pFdkUiProperty->fdkPciListPanel.hlIndex) 
				< (pFdkUiProperty->numOfPciDevice - 1) )
				pFdkUiProperty->fdkPciListPanel.hlIndex++;
			else
				if( (pFdkUiProperty->fdkPciListPanel.pageOffset + FDK_REC_PER_PAGE)
					< pFdkUiProperty->numOfPciDevice )
					pFdkUiProperty->fdkPciListPanel.pageOffset++;
            break;

		case KBPRS_PGUP:

			if( pFdkUiProperty->numOfPciDevice < FDK_REC_PER_PAGE )
				break;

			if( pFdkUiProperty->fdkPciListPanel.pageOffset >= FDK_REC_PER_PAGE )
				pFdkUiProperty->fdkPciListPanel.pageOffset -= FDK_REC_PER_PAGE;
			else
				pFdkUiProperty->fdkPciListPanel.pageOffset = 0;

			pFdkUiProperty->fdkPciListPanel.hlIndex = 0;
            break;

		case KBPRS_PGDN:

			if( pFdkUiProperty->numOfPciDevice < FDK_REC_PER_PAGE )
				break;

			if( (pFdkUiProperty->fdkPciListPanel.pageOffset + FDK_REC_PER_PAGE) 
				< pFdkUiProperty->numOfPciDevice ) {

				pFdkUiProperty->fdkPciListPanel.pageOffset += FDK_REC_PER_PAGE;
				if( (pFdkUiProperty->numOfPciDevice 
					- pFdkUiProperty->fdkPciListPanel.pageOffset) 
					< FDK_REC_PER_PAGE )
					pFdkUiProperty->fdkPciListPanel.pageOffset =
						pFdkUiProperty->numOfPciDevice - FDK_REC_PER_PAGE;
			}
			else
				pFdkUiProperty->fdkPciListPanel.pageOffset =
					pFdkUiProperty->numOfPciDevice - FDK_REC_PER_PAGE;

			pFdkUiProperty->fdkPciListPanel.hlIndex = FDK_REC_PER_PAGE - 1;
            break;

		case KBPRS_ENTER:
			
			pFdkUiProperty->fdkDumpPanel.byteBase = 
				pFdkUiProperty->fdkPciListPanel.hlIndex + 
				pFdkUiProperty->fdkPciListPanel.pageOffset;
			return 1;

		default:
			break;
	}

	return 0;
}


