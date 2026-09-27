/*
 * FDK - Firmware Debug Kit
 * File: netsock.h
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

#ifndef FDK_INCLUDE_NETSOCK_H_
#define FDK_INCLUDE_NETSOCK_H_

#include "mtypes.h"

// Opens a listening TCP socket bound to |addr| (IPv4/IPv6 literal or host
// name, NULL means any address). Returns 0 on success.
s32 initializeSocket(s32 *fd, const s8 *addr, s32 port);

// Connects to |addr|:|port| (IPv4/IPv6 literal or host name). Returns 0 on
// success.
s32 connectSocket(s32 *fd, const s8 *addr, s32 port);

void deinitializeSocket(s32 fd);

// Accepts a new connection. Returns TRUE on success.
s32 acceptSocket(s32 fd, s32 *apsd);

#endif  // FDK_INCLUDE_NETSOCK_H_
