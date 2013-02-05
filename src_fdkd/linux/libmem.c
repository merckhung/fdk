#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <mtypes.h>
#include <fdk.h>
#include <libmem.h>


inline s32 openMemDev( void ) {

	return open( FDK_MEM_DEV, O_RDWR );
}


inline void closeMemDev( s32 fd ) {

	close( fd );
}


volatile void *memMapping( s32 fd, u64 addr, u32 len, u64 *alignOff, u32 *actLen ) {

    u64 alignAddr;

    // Must align 4kb boundary
    alignAddr = addr & 0xFFFFFFFFFFFFE000ULL;
    *alignOff = addr - alignAddr;

    // Calculate maping size
    if( len < FDK_4K_PAGE )
        len = FDK_4K_PAGE;
    else if( len % FDK_4K_PAGE )
        len = ((len + FDK_4K_PAGE) / FDK_4K_PAGE) * FDK_4K_PAGE;

    // Aligned size plus 4kb
    len += FDK_4K_PAGE;
    *actLen = len;

    // Map Memory
    return mmap( NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, alignAddr );
}


s32 memUnmapping( void *phyMem, u32 len ) {

    return munmap( phyMem, len );
}


u8 memReadByte( s32 fd, u64 addr ) {

    volatile u8 *phyMem;
	u8 val;
    u64 alignOff;
	u32 actLen;

    // Map memory
    phyMem = (volatile u8 *)memMapping( fd, addr, FDK_4K_PAGE, &alignOff, &actLen );
    if( !phyMem )
        return 0xFF;

    // Write val
    val = *(phyMem + alignOff);

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

    return val;
}



u16 memReadWord( s32 fd, u64 addr ) {

    volatile u16 *phyMem;
	u16 val;
    u64 alignOff;
	u32 actLen;

    // Map memory
    phyMem = (volatile u16 *)memMapping( fd, addr, FDK_4K_PAGE, &alignOff, &actLen );
    if( !phyMem )
        return 0xFFFF;

    // Write val
    val = *(phyMem + (alignOff / 2));

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

    return val;
}


u32 memReadDWord( s32 fd, u64 addr ) {

    volatile u32 *phyMem;
	u64 alignOff;
	u32 actLen, val;

    // Map memory
    phyMem = (volatile u32 *)memMapping( fd, addr, FDK_4K_PAGE, &alignOff, &actLen );
    if( !phyMem )
        return 0xFFFFFFFF;

    // Write val
    val = *(phyMem + (alignOff / 4));

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

    return val;
}


u32 memReadBuffer( s32 fd, u64 addr, u32 len, u8 *buf ) {

    volatile u8 *phyMem;
    u64 alignOff;
	u32 actLen, i;

    // Map memory
    phyMem = (volatile u8 *)memMapping( fd, addr, len, &alignOff, &actLen );
    if( !phyMem )
        return 0xFFFFFFFF;

    // Write val
    for( i = 0 ; i < len ; i++ )
        *(buf + i) = *(phyMem + alignOff + i);

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

	return 0;
}


u32 memWriteBuffer( s32 fd, u64 addr, u32 len, u8 *buf ) {

    volatile u8 *phyMem;
    u64 alignOff;
	u32 actLen, i;

    // Map memory
    phyMem = (volatile u8 *)memMapping( fd, addr, len, &alignOff, &actLen );
    if( !phyMem )
        return 0xFFFFFFFF;

    // Write val
    for( i = 0 ; i < len ; i++ )
		*(phyMem + alignOff + i) = *(buf + i);

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

	return 0;
}


u8 memWriteByte( s32 fd, u64 addr, u8 val ) {

    volatile u8 *phyMem;
    u64 alignOff;
	u32 actLen;
	u8 tmp;

    // Map memory
    phyMem = (volatile u8 *)memMapping( fd, addr, FDK_4K_PAGE, &alignOff, &actLen );
    if( !phyMem )
        return 0xFF;

    // Write val
    *(phyMem + alignOff) = val;
	tmp = *(phyMem + alignOff);

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

	return tmp;
}


u16 memWriteWord( s32 fd, u64 addr, u16 val ) {

    volatile u16 *phyMem;
    u64 alignOff;
	u32 actLen;
	u16 tmp;

    // Map memory
    phyMem = (volatile u16 *)memMapping( fd, addr, FDK_4K_PAGE, &alignOff, &actLen );
    if( !phyMem )
        return 0xFFFF;

    // Write val
    *(phyMem + (alignOff / 2)) = val;
	tmp = *(phyMem + (alignOff / 2));

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

	return tmp;
}


u32 memWriteDWord( s32 fd, u64 addr, u32 val ) {

    volatile u32 *phyMem;
	u64 alignOff;
	u32 actLen, tmp;

    // Map memory
    phyMem = (volatile u32 *)memMapping( fd, addr, FDK_4K_PAGE, &alignOff, &actLen );
    if( !phyMem )
        return 0xFFFFFFFF;

    // Write val
    *(phyMem + (alignOff / 4)) = val;
	tmp = *(phyMem + (alignOff / 4));

    // Unmap memory
    memUnmapping( (void *)phyMem, actLen );

	return tmp;
}


