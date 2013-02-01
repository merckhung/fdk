CROSS_COMPILE       =
AS                  =   $(CROSS_COMPILE)as
AR                  =   $(CROSS_COMPILE)ar
CC                  =   $(CROSS_COMPILE)gcc
CPP                 =   $(CC) -E
LD                  =   $(CROSS_COMPILE)ld
NM                  =   $(CROSS_COMPILE)nm
OBJCOPY             =   $(CROSS_COMPILE)objcopy
OBJDUMP             =   $(CROSS_COMPILE)objdump
RANLIB              =   $(CROSS_COMPILE)ranlib
READELF             =   $(CROSS_COMPILE)readelf
SIZE                =   $(CROSS_COMPILE)size
STRINGS             =   $(CROSS_COMPILE)strings
STRIP               =   $(CROSS_COMPILE)strip

CFLAGS				=	-Iinclude -Wall -g3
LDFLAGS				=

MODULES				=	fdkd cfdk

OBJS_COMMON			=	packet.o utils.o netsock.o libcomm.o

OBJS_FDKD			=	fdkd.o helper.o $(OBJS_COMMON)
CFLAGS_FDKD			=	$(CFLAGS)
LFDLAGS_FDKD		=	$(LDFLAGS) -lpthread

OBJS_CFDK			=	cfdk.o pciListPanel.o dumpPanel.o $(OBJS_COMMON)
CFLAGS_CFDK			=	$(CFLAGS)
LDFLAGS_CFDK		=	$(LDFLAGS) -lpanel -lncurses


all: $(MODULES)

fdkd: $(OBJS_FDKD)
	@$(CC) $(CFLAGS_FDKD) $(LDFLAGS_FDKD) -o $@ $(OBJS_FDKD)

cfdk: $(OBJS_CFDK)
	@$(CC) $(CFLAGS_CFDK) $(LDFLAGS_CFDK) -o $@ $(OBJS_CFDK)

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) -f *.o $(MODULES)


