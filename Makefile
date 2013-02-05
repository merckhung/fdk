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

MODULES				:=	fdkd cfdk

PATH_LIB			:=	lib
OBJS_COMMON			:=	$(PATH_LIB)/packet.o $(PATH_LIB)/netsock.o $(PATH_LIB)/libcomm.o

PATH_FDKD			:=	src_fdkd
OBJS_FDKD			:=	$(PATH_FDKD)/fdkd.o $(PATH_FDKD)/helper.o $(PATH_FDKD)/linux/libmem.o $(PATH_FDKD)/linux/pc/libpci.o $(OBJS_COMMON)
CFLAGS_FDKD			:=	$(CFLAGS)
LDFLAGS_FDKD		:=	$(LDFLAGS) -lpthread

PATH_CFDK			:=	src_cfdk
OBJS_CFDK			:=	$(PATH_CFDK)/cfdk.o $(PATH_CFDK)/pciListPanel.o $(PATH_CFDK)/dumpPanel.o $(PATH_CFDK)/utils.o $(OBJS_COMMON)
CFLAGS_CFDK			:=	$(CFLAGS)
LDFLAGS_CFDK		:=	$(LDFLAGS) -lpanel -lncurses


all: $(MODULES)

fdkd: $(OBJS_FDKD)
	@$(CC) $(CFLAGS_FDKD) $(LDFLAGS_FDKD) -o $@ $(OBJS_FDKD)

cfdk: $(OBJS_CFDK)
	@$(CC) $(CFLAGS_CFDK) $(LDFLAGS_CFDK) -o $@ $(OBJS_CFDK)

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) -f $(MODULES)
	$(shell find ./ -name "*.o" -exec rm -f {} \;)


