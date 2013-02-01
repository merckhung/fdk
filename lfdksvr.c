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

#include <otypes.h>
#include <packet.h>

#include <ncurses.h>
#include <panel.h>

#include <lfdk.h>


typedef struct _afdkdSvrTask {

    struct _afdkdSvrTask        *next;
    s32                         fd;
    char          pktData[ AFDKD_PKTSIZE ];

} afdkdSvrTask_t;


// Global Variables
static afdkdSvrTask_t *afdkdSvrTaskHead = NULL;
pthread_mutex_t taskMutexLock = PTHREAD_MUTEX_INITIALIZER;


static void usage( void ) {
    
    fprintf( stderr, "\n" );
    fprintf( stderr, "REVISION: "AFDKD_REV"\n" );
    fprintf( stderr, "Usage: "AFDKD_NAME" [-d] | [-h]\n\n" );
    fprintf( stderr, "\t-d\tDon't run as daemon.\n" );
    fprintf( stderr, "\t-h\tprint this message.\n");
    fprintf( stderr, "\n");
}


s32 main( s32 argc, s8 **argv ) {
    
    s32 sts;
    s8 c;
    s32 daemon = 1, terminate = 0;
    pid_t pid, sid;
    
    s32 sfd, cfd;
    s8 packet[ AFDKD_PKTSIZE ];
    afdkdSvrTask_t *pAfdkdSvrTask;
    

    // Handle arguments
    while( (c = getopt( argc, argv, ":dh" )) != EOF ) {

        switch( c ) {
            case 'd':
                daemon = 0;
                break;
                
            default:
            case 'h':
                usage();
                return 0;
        }
    }
    
    
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
    if( initializeSocket( &sfd, NULL, AFDKD_SERVER_PORT ) ) {
        
        fprintf( stderr, "Cannot open socket\n" );
        return -1;
    }


    // Handle incoming connections
    while( !terminate ) {

        // Accept new connection
        if( acceptSocket( sfd, &cfd ) != TRUE )
            continue;
        
        // Clear buffer
        memset( packet, 0, AFDKD_PKTSIZE );
        
        // Read incoming data
        if( receiveSocket( cfd, packet, AFDKD_PKTSIZE ) == FALSE ) {
            
            // Allocate a task
            pAfdkdSvrTask = (afdkdSvrTask_t *)malloc( sizeof( afdkdSvrTask_t ) );
            if( !pAfdkdSvrTask ) {
            
                fprintf( stderr, "Out of Memory\n" );
                goto EndOfRecv;
            }
            
            // Clear buffer
            pAfdkdSvrTask->next = NULL;
            memcpy( pAfdkdSvrTask->pktData, packet, AFDKD_PKTSIZE );

			printf( "\n======= PLAY RECV START =================================\n" );
			dumpData( (u32 *)packet, AFDKD_PKTSIZE, 0x0 );
			printf( "\n======= PLAY RECV END    =================================\n" );
            
#if 0
            // Add to the Linklist
            pthread_mutex_lock( &taskMutexLock );
            appendLinklist( (commonLinklist_t **)&afdkdSvrTaskHead, (commonLinklist_t *)pAfdkdSvrTask );
            pthread_mutex_unlock( &taskMutexLock );
#endif
        }
        
EndOfRecv:
        // Close this connection
        deinitializeSocket( cfd );
    }


	// Wait for the remaining tasks to finish
	

    // Close the socket
    deinitializeSocket( sfd );


    // Return
    return 0;
}


