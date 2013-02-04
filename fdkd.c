#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <mtypes.h>
#include <fdk.h>
#include <libcomm.h>
#include <netsock.h>
#include <packet.h>
#include <fdkd.h>


static volatile u8 terminate = 0;
static threadList_t *headThreadList = NULL;
static pthread_mutex_t threadLock = PTHREAD_MUTEX_INITIALIZER;


static void help( void ) {

    fprintf( stderr, "\n" FDK_COPYRIGHT_TEXT "\n\n" );
    fprintf( stderr, FDKD_PROGRAM_NAME ", Version " FDK_REVISION "\n" );
    fprintf( stderr, "Author: " FDK_AUTHOR_NAME "\n" );
    fprintf( stderr, "help: fdkd [-d | -h]\n\n" );
    fprintf( stderr, "\t-d\tDon't run as daemon.\n" );
    fprintf( stderr, "\t-h\tPrint help and exit\n\n");
}


void *handleIncomingConnection( void *arg ) {

	threadList_t *pThreadList = (threadList_t *)arg;

	// Sanity check
	if( !pThreadList )
		pthread_exit( 0 );

	// Main thread loop
	while( 1 ) {

		// Read incoming data
		receiveSocket(
			pThreadList->cfd,
			pThreadList->packet,
			FDK_PKTSIZE,
			&pThreadList->rwByte );
		if( pThreadList->rwByte <= 0 )
			break;

		// Handle this packet
		if( !handleRequestPacket(
				pThreadList->cfd,
				(fdkCommPkt_t *)pThreadList->packet,
				pThreadList->rwByte ) ) {

			// Response
			transferSocket(
				pThreadList->cfd, 
				pThreadList->packet,
				((fdkCommPkt_t *)pThreadList->packet)->fdkCommHdr.pktLen,
				&pThreadList->rwByte );
		}
		else
			break;
	}

	// Close this connection
	deinitializeSocket( pThreadList->cfd );

	// Detach my context
	pthread_mutex_lock( &threadLock );
	removeLinklist( (commonLinklist_t **)&headThreadList, (commonLinklist_t *)pThreadList );
	pthread_mutex_unlock( &threadLock );

	// Free memory
	free( pThreadList );

	// Return
	pthread_exit( 0 );
}


void handleSignal( int no ) {

	// Terminate the main program
	terminate = 1;
}


s32 main( s32 argc, s8 **argv ) {
    
    s8 c;
    s32 daemon = 1, sfd, cfd, ret;
    pid_t pid, sid;
	uid_t uid, euid;
	threadList_t *pThreadList;


	// Initialize
	uid = getuid();
	euid = geteuid();
	if( uid != 0 || euid != 0 ) {

		fprintf( stderr, "Must be run with ROOT privilege\n" );
		help();
		return -1;
	}


    // Handle arguments
    while( (c = getopt( argc, argv, ":dh" )) != EOF ) {

        switch( c ) {
            case 'd':
                daemon = 0;
                break;
                
            default:
            case 'h':
                help();
                return 0;
        }
    }


#if 0
	// Signal register
	signal( SIGKILL, handleSignal );
	signal( SIGTERM, handleSignal );
	signal( SIGHUP, handleSignal );
	signal( SIGINT, handleSignal );
#endif


    // Run as daemon
    if( daemon ) {

        // Fork a child process
        pid = fork();
        if( pid < 0 ) {
            
            fprintf( stderr, "Cannot fork child process.\n" );
            exit( 1 );
        }
        
        // Terminate the parent process
        if( pid > 0 )
            exit( 0 );
        umask( 0 );
        
        // Set SID
        sid = setsid();
        if( sid < 0 )
            exit( 1 );
        
        // Change location to root
        if( chdir( "/" ) < 0 )
            exit( 1 );
        
        // Close standard input, output, and error
        close( 0 );
        close( 1 );
        close( 2 );
    }
    
    
    // Open a socket
    if( initializeSocket( &sfd, NULL, FDK_DEF_PORT ) ) {
        
        fprintf( stderr, "Cannot open socket\n" );
        return -1;
    }


    // Handle incoming connections
    while( !terminate ) {

        // Accept new connection
        if( acceptSocket( sfd, &cfd ) != TRUE )
            continue;

		// Allocate a new thread context
		pThreadList = malloc( sizeof( threadList_t ) );
		if( !pThreadList ) {

			fprintf( stderr, "Out of memory\n" );
			deinitializeSocket( cfd );
			usleep( 1000 );
			continue;
		}

		// Attach the thread context
		pthread_mutex_lock( &threadLock );
		appendLinklist( (commonLinklist_t **)&headThreadList, (commonLinklist_t *)pThreadList );
		pthread_mutex_unlock( &threadLock );

		// Fill in the data
		pThreadList->cfd = cfd;

		// Create a thread
		ret = pthread_create( 
			&pThreadList->pth,
			NULL,
			handleIncomingConnection,
			(void *)pThreadList );
		if( ret ) {

			fprintf( stderr, "Failed to create a thread\n" );
			break;
		}

		// Delay for a while
		usleep( 100 );
    }


	// Cancel all running threads, but don't free, the OS will do
	for( pThreadList = headThreadList ; pThreadList ; pThreadList = pThreadList->next )
		pthread_cancel( pThreadList->pth );


    // Close the socket
    deinitializeSocket( sfd );


    // Return
    return 0;
}


