#
# FDK - Firmware Debug Kit
# File: Makefile
#
# Copyright (C) 2006 - 2026 Merck Hung <merckhung@gmail.com>
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

CROSS_COMPILE ?=
CC            := $(CROSS_COMPILE)gcc
PKG_CONFIG    ?= pkg-config
CLANG_FORMAT  ?= clang-format

PREFIX        ?= /usr/local
BINDIR        ?= $(PREFIX)/bin
SBINDIR       ?= $(PREFIX)/sbin
DATADIR       ?= $(PREFIX)/share/fdk

PCIIDS_URL    ?= https://pci-ids.ucw.cz/v2.2/pci.ids
PCIIDS_MIRROR ?= https://raw.githubusercontent.com/pciutils/pciids/master/pci.ids

CFLAGS        ?= -O2 -g
CFLAGS        += -std=gnu11 -Wall -Wextra -Wno-unused-parameter
CPPFLAGS      += -Iinclude -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 \
                 -DFDK_DATADIR='"$(DATADIR)"' -MMD -MP
LDFLAGS       ?=

NCURSES_CFLAGS := $(shell $(PKG_CONFIG) --cflags ncurses panel 2>/dev/null)
NCURSES_LIBS   := $(shell $(PKG_CONFIG) --libs panel ncurses 2>/dev/null || \
                    echo -lpanel -lncurses)

PROGRAMS      := fdkd cfdk memvr

OBJS_COMMON   := lib/packet.o lib/netsock.o lib/libcomm.o
OBJS_FDKD     := src_fdkd/fdkd.o src_fdkd/handler.o \
                 src_fdkd/linux/libmem.o src_fdkd/linux/libdisk.o \
                 src_fdkd/linux/libpci.o src_fdkd/linux/libport.o \
                 src_fdkd/linux/libe820.o $(OBJS_COMMON)
OBJS_CFDK     := src_cfdk/cfdk.o src_cfdk/client.o src_cfdk/pciids.o \
                 src_cfdk/pciListPanel.o src_cfdk/dumpPanel.o $(OBJS_COMMON)
OBJS_MEMVR    := src_memvr/memvr.o src_memvr/client.o $(OBJS_COMMON)

ALL_OBJS      := $(sort $(OBJS_FDKD) $(OBJS_CFDK) $(OBJS_MEMVR))
SOURCES       := $(wildcard include/*.h lib/*.c src_*/*.c src_*/*/*.c)

.PHONY: all clean install uninstall format check-format update-pciids

all: $(PROGRAMS)

fdkd: $(OBJS_FDKD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lpthread

cfdk: $(OBJS_CFDK)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(NCURSES_LIBS)

memvr: $(OBJS_MEMVR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

src_cfdk/%.o: CPPFLAGS += $(NCURSES_CFLAGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

install: all
	install -d $(DESTDIR)$(SBINDIR) $(DESTDIR)$(BINDIR) $(DESTDIR)$(DATADIR)
	install -m 0755 fdkd $(DESTDIR)$(SBINDIR)/fdkd
	install -m 0755 cfdk memvr $(DESTDIR)$(BINDIR)/
	install -m 0644 data/pci.ids $(DESTDIR)$(DATADIR)/pci.ids

uninstall:
	$(RM) $(DESTDIR)$(SBINDIR)/fdkd $(DESTDIR)$(BINDIR)/cfdk \
	      $(DESTDIR)$(BINDIR)/memvr $(DESTDIR)$(DATADIR)/pci.ids

# Formats the sources with the Google C++ style (see .clang-format).
format:
	$(CLANG_FORMAT) -i $(SOURCES)

check-format:
	$(CLANG_FORMAT) --dry-run --Werror $(SOURCES)

# Refreshes the bundled PCI ID database from the PCI ID Project.
update-pciids:
	curl -fsSL -o data/pci.ids.new $(PCIIDS_URL) || \
	  curl -fsSL -o data/pci.ids.new $(PCIIDS_MIRROR)
	grep -q '^#.*Version:' data/pci.ids.new
	mv data/pci.ids.new data/pci.ids
	@grep -m1 'Version:' data/pci.ids

clean:
	$(RM) $(PROGRAMS) $(ALL_OBJS) $(ALL_OBJS:.o=.d)

-include $(ALL_OBJS:.o=.d)
