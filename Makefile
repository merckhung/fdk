################################################################################
#
# Copyright (C) 2006 - 2008 Olux Organization All rights reserved.
# Authors: Merck Hung <merck@gmail.com>
#
# File: Makefile
# Description:
#   None
#
################################################################################
TARGET				=	lfdk lfdksvr

CFLAGS				=	-O2 -Wall -Iinclude
LDFLAGS				=

all: $(TARGET)

lfdk: lfdk.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lpanel -lncurses -o $@ $< packet.c dumpPanel.c pciListPanel.c utils.c

clean:
	rm -f *.o $(TARGET)

