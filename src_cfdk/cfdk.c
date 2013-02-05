/*
 * FDK - Firmware Debug Kit
 * File: cfdk.c
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
#include <netsock.h>


static void help( void ) {

    fprintf( stderr, "\n" FDK_COPYRIGHT_TEXT "\n\n" );
    fprintf( stderr, CFDK_PROGRAM_NAME ", Version " FDK_REVISION "\n" );
    fprintf( stderr, "Author: " FDK_AUTHOR_NAME "\n" );
    fprintf( stderr, "help: fdk [-i 127.0.0.1 | -d /dev/ttyS0] [-h]\n\n" );
	fprintf( stderr, "\t-p\tPCI IDs filepath, default is " FDK_DEF_PCIIDS "\n" );
	fprintf( stderr, "\t-i\tIPv4 address, default is " FDK_DEF_IPADDR "\n");
	fprintf( stderr, "\t-d\tUART TTY device, default is " FDK_DEF_TTYDEV "\n");
    fprintf( stderr, "\t-h\tPrint help and exit\n\n");
}


static s32 configureTtyDevice( s32 fd ) {

	struct termios termSetting;

	if( tcgetattr( fd, &termSetting ) )
		return 1;

	if( cfsetspeed( &termSetting, B115200 ) )
		return 1;

    if( tcsetattr( fd, TCSANOW, &termSetting ) )
        return 1;

	return 0;
}


void initColorPairs( void ) {

    init_pair( WHITE_RED, COLOR_WHITE, COLOR_RED );
    init_pair( WHITE_BLUE, COLOR_WHITE, COLOR_BLUE );
	init_pair( WHITE_YELLOW, COLOR_WHITE, COLOR_YELLOW );
	init_pair( WHITE_BLACK, COLOR_WHITE, COLOR_BLACK );

    init_pair( BLACK_WHITE, COLOR_BLACK, COLOR_WHITE );
	init_pair( BLACK_GREEN, COLOR_BLACK, COLOR_GREEN );
	init_pair( BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW );
	init_pair( BLACK_BLUE, COLOR_BLACK, COLOR_BLUE );
	init_pair( BLACK_CYAN, COLOR_BLACK, COLOR_CYAN );

    init_pair( CYAN_BLUE, COLOR_CYAN, COLOR_BLUE );
	init_pair( CYAN_WHITE, COLOR_CYAN, COLOR_WHITE );

    init_pair( RED_BLUE, COLOR_RED, COLOR_BLUE );
	init_pair( RED_WHITE, COLOR_RED, COLOR_WHITE );

    init_pair( YELLOW_BLUE, COLOR_YELLOW, COLOR_BLUE );
    init_pair( YELLOW_RED, COLOR_YELLOW, COLOR_RED );
    init_pair( YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK );
	init_pair( YELLOW_WHITE, COLOR_YELLOW, COLOR_WHITE );

	init_pair( MAGENTA_BLUE, COLOR_MAGENTA, COLOR_BLUE );

	init_pair( GREEN_BLUE, COLOR_GREEN, COLOR_BLUE );
	init_pair( GREEN_WHITE, COLOR_GREEN, COLOR_WHITE );
}


void updateStatusTimer( fdkUiProperty_t *pFdkUiProperty ) {

	time_t timer;
	struct tm *nowtime;
	u32 thisSecond;
	u8 update = 0;
	s8 stsBuf[ FDK_MAX_COLUMN - FDK_MAX_TIMESTR ];

	// Read second
	thisSecond = time( &timer );
	if( thisSecond == -1 )
		return;

	// Convert to localtime
	nowtime = localtime( &timer );
	if( !nowtime )
		return;

	if( pFdkUiProperty->fdkBasePanel.lastSecond < thisSecond ) {

		// Update timer per second
		printWindowAt( pFdkUiProperty->fdkBasePanel, 
			time,
			FDK_STRING_NLINE,
			FDK_MAX_TIMESTR,
			FDK_MAX_LINE,
			FDK_MAX_COLUMN - FDK_MAX_TIMESTR,
			BLACK_WHITE,
			"%2.2d:%2.2d:%2.2d",
			nowtime->tm_hour,
			nowtime->tm_min,
			nowtime->tm_sec );

		update = 1;
	}

	if( pFdkUiProperty->fdkBasePanel.statusStr && ((thisSecond - pFdkUiProperty->fdkBasePanel.lastSecond) >= FDK_STS_INTV_SECS) ) {

		s32 len = strlen( pFdkUiProperty->fdkBasePanel.statusStr );
		if( pFdkUiProperty->fdkBasePanel.strIdx >= len )
			pFdkUiProperty->fdkBasePanel.strIdx = 0;

		if( (len - pFdkUiProperty->fdkBasePanel.strIdx) > (FDK_MAX_COLUMN - FDK_MAX_TIMESTR) )
			len = FDK_MAX_COLUMN - FDK_MAX_TIMESTR;

		snprintf( stsBuf, len, "%s", (pFdkUiProperty->fdkBasePanel.statusStr + pFdkUiProperty->fdkBasePanel.strIdx) );

		// Update status bar
    	printWindow( 
			pFdkUiProperty->fdkBasePanel,
			status,
			FDK_STRING_NLINE,
			FDK_MAX_COLUMN - FDK_MAX_TIMESTR,
			FDK_MAX_LINE,
			FDK_MIN_COLUMN,
			BLACK_WHITE,
			"%s",
			stsBuf );

		pFdkUiProperty->fdkBasePanel.strIdx++;
		update = 1;
	}

	if( update )
		pFdkUiProperty->fdkBasePanel.lastSecond = thisSecond;
}


void printBasePlane( fdkUiProperty_t *pFdkUiProperty ) {

    // Background Color
    printWindow( 
		pFdkUiProperty->fdkBasePanel, 
		background,
		FDK_MAX_LINE,
		FDK_MAX_COLUMN,
		FDK_MIN_LINE,
		FDK_MIN_COLUMN,
		WHITE_BLUE,
		"" );

    // Logo
    printWindow(
		pFdkUiProperty->fdkBasePanel,
		logo,
		FDK_STRING_NLINE,
		FDK_MAX_COLUMN,
		FDK_MIN_LINE,
		FDK_MIN_COLUMN,
		WHITE_RED,
		CFDK_PROGRAM_NAME );

	// Copyright
    printWindow(
		pFdkUiProperty->fdkBasePanel,
		copyright,
		FDK_STRING_NLINE,
		strlen( FDK_AUTHOR_NAME ),
		FDK_MIN_LINE,
		FDK_MAX_COLUMN - strlen( FDK_AUTHOR_NAME ),
		WHITE_RED,
		FDK_AUTHOR_NAME );

	// Update screen
    update_panels();
    doupdate();
}


void printBaseHelp( fdkUiProperty_t *pFdkUiProperty ) {

	s8 *helpTxt = FDK_COPYRIGHT_TEXT "\n\
Author: " FDK_AUTHOR_NAME "\n\n\
  <F1>:  Help\n\
  <F2>:  PCI/PCI-E device listing\n\
  <F3>:  PCI/PCI-E device configuration space\n\
  <F4>:  CPU I/O space (Intel x86 arch only)\n\
  <F5>:  CPU Memory space\n\
  <F6>:  IDE hard drive content (1st HDD)\n\
  <F7>:  CMOS content (Intel x86 arch only)\n\
  <F8>:  TBD\n\
  <F9>:  TBD\n\
  <F10>: TBD\n\
  <F11>: TBD\n\
  <F12>: TBD\n\
";

	if( pFdkUiProperty->fdkBasePanel.toggleHelp ) {

		printWindowMove(
			pFdkUiProperty->fdkBasePanel, 
			help,
			(FDK_MAX_LINE - 4),
			(FDK_MAX_COLUMN - 4),
			FDK_HELP_X_POS,
			FDK_HELP_Y_POS,
			BLACK_WHITE,
			"%s",
			helpTxt );
	}
	else
		destroyWindow( pFdkUiProperty->fdkBasePanel, help );
}


s32 main( s32 argc, s8 **argv ) {

	s8 c, ttyDevice[ FDK_MAX_PATH ], ipAddr[ FDK_MAX_PATH ];
	fdkUiProperty_t fdkUiProperty;
	s8 useIp = 1;

	// Initialization
	strncpy( ttyDevice, FDK_DEF_TTYDEV, FDK_MAX_PATH );
	strncpy( ipAddr, FDK_DEF_IPADDR, FDK_MAX_PATH );
	memset( &fdkUiProperty, 0, sizeof( fdkUiProperty_t ) );
	fdkUiProperty.fdkHwFunc = fdkUiProperty.fdkPreviousHwFunc = KHF_INIT;
	fdkUiProperty.pFdkCommPkt = (fdkCommPkt_t *)fdkUiProperty.pktBuf;
	fdkUiProperty.fdkBasePanel.statusStr = FDK_WELCOME_TXT;
	strncpy( fdkUiProperty.pciIdsPath, FDK_DEF_PCIIDS, FDK_MAX_PATH );


	// Handle parameters
    while( (c = getopt( argc, argv, "p:i:d:h" )) != EOF ) {

        switch( c ) {

			case 'd':
				strncpy( ttyDevice, optarg, FDK_MAX_PATH );
				useIp = 0;
				break;

			case 'i':
				strncpy( ipAddr, optarg, FDK_MAX_PATH );
				useIp = 1;
				break;

			case 'p':
				strncpy( fdkUiProperty.pciIdsPath, optarg, FDK_MAX_PATH );
				break;

            case 'h':
                help();
                return 0;

            default:

                help();
                return 1;
        }
    }


	if( !useIp ) {

		// Open TTY device
		fdkUiProperty.fd = open( ttyDevice, O_RDWR );
		if( fdkUiProperty.fd < 0 ) {

			fprintf( stderr, "Cannot open device %s\n", ttyDevice );
			return 1;
		}


		// Configure TTY device
		if( configureTtyDevice( fdkUiProperty.fd ) ) {

			fprintf( stderr, "Cannot configure device %s\n", ttyDevice );
			goto ErrExit;
		}
	}
	else {

    	// Open a socket
    	if( connectSocket( &fdkUiProperty.fd, ipAddr, FDK_DEF_PORT ) ) {

        	fprintf( stderr, "Cannot open socket\n" );
			goto ErrExit;
    	}
	}


	// Connect to FDK server
	if( connectToFdkServer( &fdkUiProperty ) ) {

		fprintf( stderr, "Cannot connect to FDK server\n" );
		goto ErrExit;
	}

	// Read PCI list
	if( readPciList( &fdkUiProperty ) ) {

		fprintf( stderr, "Cannot get PCI device listing\n" );
		goto ErrExit;
	}

	// Read E820 list
	if( readE820List( &fdkUiProperty ) ) {

		fprintf( stderr, "Cannot get E820 listing\n" );
		goto ErrExit1;
	}


	// Initialize ncurses
    initscr();
    start_color();
    cbreak();
    noecho();
    nodelay( stdscr, 1 );
    keypad( stdscr, 1 );
    curs_set( 0 );
	initColorPairs();


	// Print base panel
	printBasePlane( &fdkUiProperty );


	// Main loop
	for( ; ; ) {


		// Get keyboard input
		fdkUiProperty.inputBuf = getch();
		switch( fdkUiProperty.inputBuf ) {

			// ESC
			case KBPRS_ESC:
				goto Exit;

			// Help
			case KBPRS_F1:
				fdkUiProperty.fdkBasePanel.toggleHelp = !fdkUiProperty.fdkBasePanel.toggleHelp;
				break;

			// PCI/PCI-E listing
			case KBPRS_F2:
				fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
				fdkUiProperty.fdkHwFunc = KHF_PCIL;
				break;

			// PCI/PCI-E config space
			case KBPRS_F3:
				fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
				fdkUiProperty.fdkHwFunc = KHF_PCI;
				break;

			// I/O
			case KBPRS_F4:
				fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
				fdkUiProperty.fdkHwFunc = KHF_IO;
				break;

			// Memory
			case KBPRS_F5:
				fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
				fdkUiProperty.fdkHwFunc = KHF_MEM;
				break;

			// IDE
			case KBPRS_F6:
				fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
				fdkUiProperty.fdkHwFunc = KHF_IDE;
				break;

			// CMOS
			case KBPRS_F7:
				fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
				fdkUiProperty.fdkHwFunc = KHF_CMOS;
				break;

			default:
				fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
				if( fdkUiProperty.fdkHwFunc == KHF_INIT )
					fdkUiProperty.fdkHwFunc = KHF_MEM; // Default
				break;
		}


DirectTransition:


		// Clear previous function
		if( fdkUiProperty.fdkPreviousHwFunc 
			!= fdkUiProperty.fdkHwFunc ) {

			// Clear screen & reset
			clearDumpBasePanel( &fdkUiProperty );
			clearDumpUpdatePanel( &fdkUiProperty );
			clearPciListBasePanel( &fdkUiProperty );
			clearPciListUpdatePanel( &fdkUiProperty );

			if( (fdkUiProperty.fdkPreviousHwFunc != KHF_PCIL)
				&& (fdkUiProperty.fdkHwFunc != KHF_PCI) )
				fdkUiProperty.fdkDumpPanel.byteBase = 0;

			fdkUiProperty.fdkDumpPanel.byteOffset = 0;
			fdkUiProperty.fdkPciListPanel.hlIndex = 0;
			fdkUiProperty.fdkPciListPanel.pageOffset = 0;

			// Print
			switch( fdkUiProperty.fdkHwFunc ) {

				default:
				case KHF_MEM:
					fdkUiProperty.fdkDumpPanel.infoStr = FDK_INFO_MEMORY_BASE;
					printDumpBasePanel( &fdkUiProperty );
					break;

				case KHF_IO:
					fdkUiProperty.fdkDumpPanel.infoStr = FDK_INFO_IO_BASE;
					printDumpBasePanel( &fdkUiProperty );
					break;

				case KHF_PCI:
					fdkUiProperty.fdkDumpPanel.infoStr = FDK_INFO_PCI_BASE;
					printDumpBasePanel( &fdkUiProperty );
					break;

				case KHF_PCIL:
					printPciListBasePanel( &fdkUiProperty );
					break;

				case KHF_IDE:
					fdkUiProperty.fdkDumpPanel.infoStr = FDK_INFO_IDE_BASE;
					printDumpBasePanel( &fdkUiProperty );
					break;

				case KHF_CMOS:
					fdkUiProperty.fdkDumpPanel.infoStr = FDK_INFO_CMOS_BASE;
					printDumpBasePanel( &fdkUiProperty );
					break;
			}
		}


		switch( fdkUiProperty.fdkHwFunc ) {

			default:
			case KHF_MEM:
				handleKeyPressForDumpPanel( &fdkUiProperty );
				if( !readMemory( &fdkUiProperty ) )
					printDumpUpdatePanel( &fdkUiProperty );
				break;

			case KHF_IO:
				handleKeyPressForDumpPanel( &fdkUiProperty );
				if( !readIo( &fdkUiProperty ) )
					printDumpUpdatePanel( &fdkUiProperty );
				break;

			case KHF_PCI:
				handleKeyPressForDumpPanel( &fdkUiProperty );
				if( !readPci( &fdkUiProperty ) )
					printDumpUpdatePanel( &fdkUiProperty );
				break;

			case KHF_PCIL:
				if( handleKeyPressForPciListPanel( &fdkUiProperty ) ) {

					fdkUiProperty.fdkPreviousHwFunc = fdkUiProperty.fdkHwFunc;
					fdkUiProperty.fdkHwFunc = KHF_PCI;
					fdkUiProperty.inputBuf = KBPRS_F3;
					goto DirectTransition;
				}
				else
					printPciListUpdatePanel( &fdkUiProperty );
				break;

			case KHF_IDE:
				handleKeyPressForDumpPanel( &fdkUiProperty );
				if( !readIde( &fdkUiProperty ) )
					printDumpUpdatePanel( &fdkUiProperty );
				break;

			case KHF_CMOS:
				handleKeyPressForDumpPanel( &fdkUiProperty );
				if( !readCmos( &fdkUiProperty ) )
					printDumpUpdatePanel( &fdkUiProperty );
				break;
		}

		// Help Text
		printBaseHelp( &fdkUiProperty );

        // Update timer
		updateStatusTimer( &fdkUiProperty );

		// Refresh Screen
		update_panels();
		doupdate();

		// Delay for a while
		usleep( 3000 );
	}

Exit:

	// Terminate ncurses
	endwin();

	// Free resources
	free( fdkUiProperty.pFdkE820record );

ErrExit1:
	free( fdkUiProperty.pFdkPciDev );

ErrExit:

	// Disconnect from the server
	disconnectFromFdkServer( &fdkUiProperty );
	if( !useIp ) {

		// Close TTY device
		close( fdkUiProperty.fd );
	}
	else {

		// Close the socket
		deinitializeSocket( fdkUiProperty.fd );	
	}

    return 0;
}


