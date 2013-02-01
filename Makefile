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

CFLAGS				=	-g3 -Wall -Iinclude
LDFLAGS				=

all: $(TARGET)

lfdk: lfdk.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lpanel -lncurses -o $@ $< packet.c dumpPanel.c pciListPanel.c utils.c socket.c

lfdksvr: lfdksvr.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< packet.c utils.c socket.c

clean:
	rm -f *.o $(TARGET)

