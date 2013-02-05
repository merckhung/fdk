/*
 * FDK - Firmware Debug Kit
 * File: fdkd.h
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

typedef struct _threadList {

	struct _threadList	*next;

	pthread_t			pth;
	s32					cfd;
	s8					packet[ FDK_PKTSIZE ];
	u32					rwByte;

	s32					memfd;

} threadList_t;


s32 handleRequestPacket( s32 cfd, s32 memfd, fdkCommPkt_t *pFdkCommPkt, u32 rByte );


