#
# FDK - Firmware Debug Kit
# File: Makefile
#
# Copyright (C) 2006 - 2013 Merck Hung <merckhung@gmail.com>
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

CROSS_COMPILE       :=
AS                  :=   $(CROSS_COMPILE)as
AR                  :=   $(CROSS_COMPILE)ar
CC                  :=   $(CROSS_COMPILE)gcc
CPP                 :=   $(CC) -E
LD                  :=   $(CROSS_COMPILE)ld
NM                  :=   $(CROSS_COMPILE)nm
OBJCOPY             :=   $(CROSS_COMPILE)objcopy
OBJDUMP             :=   $(CROSS_COMPILE)objdump
RANLIB              :=   $(CROSS_COMPILE)ranlib
READELF             :=   $(CROSS_COMPILE)readelf
SIZE                :=   $(CROSS_COMPILE)size
STRINGS             :=   $(CROSS_COMPILE)strings
STRIP               :=   $(CROSS_COMPILE)strip

CFLAGS				:=	-Iinclude -Wall -g3
LDFLAGS				:=

MODULES				:=	fdkd cfdk memvr

PATH_LIB			:=	lib
OBJS_COMMON			:=	$(PATH_LIB)/packet.o $(PATH_LIB)/netsock.o $(PATH_LIB)/libcomm.o

PATH_FDKD			:=	src_fdkd
OBJS_FDKD			:=	$(PATH_FDKD)/fdkd.o $(PATH_FDKD)/helper.o $(PATH_FDKD)/linux/libmem.o $(PATH_FDKD)/linux/libdisk.o $(PATH_FDKD)/linux/pc/libpci.o $(OBJS_COMMON)
CFLAGS_FDKD			:=	$(CFLAGS)
LDFLAGS_FDKD		:=	$(LDFLAGS) -lpthread

PATH_CFDK			:=	src_cfdk
OBJS_CFDK			:=	$(PATH_CFDK)/cfdk.o $(PATH_CFDK)/pciListPanel.o $(PATH_CFDK)/dumpPanel.o $(PATH_CFDK)/utils.o $(OBJS_COMMON)
CFLAGS_CFDK			:=	$(CFLAGS)
LDFLAGS_CFDK		:=	$(LDFLAGS) -lpanel -lncurses

PATH_MEMVR			:=	src_memvr
OBJS_MEMVR			:=	$(PATH_MEMVR)/memvr.o $(PATH_MEMVR)/utils.o $(OBJS_COMMON)
CFLAGS_MEMVR		:=	$(CFLAGS)
LDFLAGS_MEMVR		:=	$(LDFLAGS)


all: $(MODULES)

fdkd: $(OBJS_FDKD)
	@$(CC) $(CFLAGS_FDKD) -o $@ $(OBJS_FDKD) $(LDFLAGS_FDKD)

cfdk: $(OBJS_CFDK)
	@$(CC) $(CFLAGS_CFDK) -o $@ $(OBJS_CFDK) $(LDFLAGS_CFDK)

memvr: $(OBJS_MEMVR)
	@$(CC) $(CFLAGS_MEMVR) -o $@ $(OBJS_MEMVR) $(LDFLAGS_MEMVR)

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) -f $(MODULES)
	$(shell find ./ -name "*.o" -exec rm -f {} \;)


