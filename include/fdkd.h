/*
 * FDK - Firmware Debug Kit
 * File: fdkd.h
 *
 * Copyright (C) 2006 - 2026 Merck Hung <merckhung@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef FDK_INCLUDE_FDKD_H_
#define FDK_INCLUDE_FDKD_H_

#include "fdk.h"
#include "mtypes.h"
#include "packet.h"

// Per-connection state of the server.
typedef struct {
  s32 cfd;                   // Client socket.
  s32 memfd;                 // /dev/mem, or -1 when it cannot be opened.
  const s8 *diskPath;        // Block device used for disk requests.
  u8 packet[FDK_MAXSZ_PKT];  // Request in, response out.
} fdkdConnection_t;

// Return values of handleRequestPacket().
#define FDKD_REPLY 0            // Send the response and keep going.
#define FDKD_REPLY_AND_CLOSE 1  // Send the response, then close.

// Handles the request in |pConn->packet| (|rByte| bytes long) and replaces
// it with the response.
s32 handleRequestPacket(fdkdConnection_t *pConn, u32 rByte);

#endif  // FDK_INCLUDE_FDKD_H_
