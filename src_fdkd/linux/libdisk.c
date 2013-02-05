/*
 * FDK - Firmware Debug Kit
 * File: libdisk.c
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

#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mtypes.h>
#include <fdk.h>
#include <packet.h>
#include <libdisk.h>


s32 fdkDiskReadWrite( u64 addr, u32 sz, u8 *ptr, fdkOpCode_t op ) {

	s32 fd, ret = 0;
	u32 rwByte;

	// Sanity check
	if( !ptr || !sz )
		return -1;

	// Open disk file
	fd = open( "/dev/sda", O_RDWR );
	if( fd < 0 )
		return fd;

	if( lseek64( fd, addr, SEEK_SET ) != addr ) {

		ret = -1;
		goto ErrExit;
	}

	switch( op ) {

	case FDK_REQ_IDE_READ:
		rwByte = read( fd, ptr, sz );
		break;

	case FDK_REQ_IDE_WRITE:
		rwByte = write( fd, ptr, sz );
		break;
		
	default:
		break;	
	}

	if( rwByte != sz )
		ret = -1;

ErrExit:

	// Close
	close( fd );

	return ret;
}


