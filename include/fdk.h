/*
 * FDK - Firmware Debug Kit
 * File: fdk.h
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

#ifndef FDK_INCLUDE_FDK_H_
#define FDK_INCLUDE_FDK_H_

#define FDK_MAX_PATH 256

#define FDK_DEF_IPADDR "127.0.0.1"
#define FDK_DEF_LISTEN_ADDR "127.0.0.1"
#define FDK_DEF_PORT 7123

// Install location of the bundled PCI ID database; overridden by the Makefile.
#ifndef FDK_DATADIR
#define FDK_DATADIR "/usr/local/share/fdk"
#endif

#define FDK_COPYRIGHT_TEXT "Copyright (c) 2006 - 2026 Merck Hung"
#define FDK_REVISION "2.1.0"
#define FDK_AUTHOR_NAME "Merck Hung <merckhung@gmail.com>"
#define FDKD_PROGRAM_NAME "Firmware Debug Kit Server"
#define CFDK_PROGRAM_NAME "Firmware Debug Kit"
#define MEMVR_PROGRAM_NAME "Text Mode Memory Viewer"

#endif  // FDK_INCLUDE_FDK_H_
