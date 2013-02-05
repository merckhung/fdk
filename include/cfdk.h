#define FDK_MIN_LINE				0
#define FDK_MIN_COLUMN				0
#define FDK_MAX_LINE				23
#define FDK_MAX_COLUMN				80
#define FDK_MAX_READBUF				512

#define FDK_STRING_NLINE			1
#define FDK_MAX_TIMESTR				8
#define FDK_BYTE_PER_SCREEN			256

#define FDK_INFO_BAR_FMT			"Type: "

#define FDK_INFO_MEMORY_FUNC		"Memory"
#define FDK_INFO_IO_FUNC			"I/O"
#define FDK_INFO_PCI_FUNC			"PCI/PCI-E"
#define FDK_INFO_PCIL_FUNC			"PCI/PCI-E Listing"
#define FDK_INFO_IDE_FUNC			"IDE"
#define FDK_INFO_CMOS_FUNC			"CMOS"

#define FDK_INFO_MEMORY_BASE		FDK_INFO_BAR_FMT FDK_INFO_MEMORY_FUNC "    Address: "
#define FDK_INFO_IO_BASE			FDK_INFO_BAR_FMT FDK_INFO_IO_FUNC "    Address: "
#define FDK_INFO_PCI_BASE			FDK_INFO_BAR_FMT FDK_INFO_PCI_FUNC "    "
#define FDK_INFO_IDE_BASE			FDK_INFO_BAR_FMT FDK_INFO_IDE_FUNC "    Address: "
#define FDK_INFO_CMOS_BASE			FDK_INFO_BAR_FMT FDK_INFO_CMOS_FUNC "    Address: "

#define FDK_INFO_MEMORY_BASE_FMT	"%8.8X-%8.8Xh"
#define FDK_INFO_IO_BASE_FMT		"%4.4Xh"
#define FDK_INFO_PCI_BASE_FMT		"Bus: %2.2Xh, Dev: %2.2Xh, Fun: %2.2Xh"
#define FDK_INFO_IDE_BASE_FMT		"%8.8X-%8.8Xh"
#define FDK_INFO_CMOS_BASE_FMT		"%2.2Xh"

#define FDK_FTITLE_PCI				"Vendor"
#define FDK_STITLE_PCI				"Device"

#define FDK_INFO_LINE				(FDK_MAX_LINE - 1)
#define FDK_INFO_COLUMN				0

#define FDK_HELP_LINE				(FDK_MAX_LINE - 4)
#define FDK_HELP_COLUMN				(FDK_MAX_COLUMN - 4)
#define FDK_HELP_X_POS				3
#define FDK_HELP_Y_POS				2

#define FDK_WELCOME_TXT				"Welcome to " CFDK_PROGRAM_NAME ". " FDK_COPYRIGHT_TEXT ". Author: " FDK_AUTHOR_NAME ", Cell: +886-975-586938"

#define FDK_DUMP_TOP_BAR			"00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"
#define FDK_DUMP_LEFT_BAR			"000000100020003000400050006000700080009000A000B000C000D000E000F0"
#define FDK_DUMP_RTOP_BAR			"0123456789ABCDEF"

#define FDK_DUMP_OFF_LINE			4
#define FDK_DUMP_OFF_COLUMN			1
#define FDK_DUMP_HL_DIGITS			2
#define FDK_DUMP_HLA_DIGITS			1
#define FDK_DUMP_BITS_DIGITS		9

#define FDK_DUMP_BASEADDR_LINE		FDK_INFO_LINE
#define FDK_DUMP_FTITLE_LINE		1
#define FDK_DUMP_FTITLE_COLUMN		1
#define FDK_DUMP_STITLE_LINE		(FDK_DUMP_FTITLE_LINE + 1)
#define FDK_DUMP_STITLE_COLUMN  	 FDK_DUMP_FTITLE_COLUMN

#define FDK_DUMP_TOP_LINE			4
#define FDK_DUMP_TOP_COLUMN			6

#define FDK_DUMP_LEFT_LINE			(FDK_DUMP_TOP_LINE + 1)
#define FDK_DUMP_LEFT_COLUMN		1

#define FDK_DUMP_RTOP_LINE			FDK_DUMP_TOP_LINE
#define FDK_DUMP_RTOP_COLUMN		(FDK_DUMP_TOP_COLUMN + strlen(FDK_DUMP_TOP_BAR) + 2)

#define FDK_DUMP_BYTE_PER_LINE		16
#define FDK_DUMP_BUF_PER_LINE		(FDK_DUMP_BYTE_PER_LINE * 3 - 1)

#define FDK_DUMP_VALUE_LINE			(FDK_DUMP_TOP_LINE + 1)
#define FDK_DUMP_VALUE_COLUMN		(FDK_DUMP_LEFT_COLUMN + 5)
#define FDK_DUMP_ASCII_LINE			(FDK_DUMP_TOP_LINE + 1)
#define FDK_DUMP_ASCII_COLUMN		FDK_DUMP_RTOP_COLUMN
#define FDK_DUMP_ASCII_FILTER(x)	(((x) >= '!' && (x) <= '~')?(x):'.')

#define FDK_DUMP_VBUF_SZ			(FDK_BYTE_PER_SCREEN * 3 - FDK_DUMP_BYTE_PER_LINE)
#define FDK_DUMP_ABUF_SZ			FDK_BYTE_PER_SCREEN

#define FDK_STS_BUF_SZ				4096
#define FDK_STS_INTV_SECS			1
#define FDK_MAX_PCINAME				75

#define FDK_PCIL_TITLE				"PCI/PCI-E Device Listing                                   Ven  Dev  Bus Dev Fun"
#define FDK_PCIL_LINE_FMT			"%-12.12s,%-45.45s %4.4X %4.4X  %2.2X  %2.2X  %2.2X"
#define FDK_PCIL_TITLE_COLUMN		FDK_MAX_COLUMN
#define FDK_PCIL_TITLE_LINE			1
#define FDK_PCIL_TITLE_X_POS		1
#define FDK_PCIL_TITLE_Y_POS		0
#define FDK_PCIL_CON_COLUMN			FDK_MAX_COLUMN
#define FDK_PCIL_CON_LINE			(FDK_MAX_LINE - 2)
#define FDK_PCIL_CON_X_POS			2
#define FDK_PCIL_CON_Y_POS			0
#define FDK_BUF_SIZE				(FDK_PCIL_CON_COLUMN * FDK_PCIL_CON_LINE + 1)
#define FDK_REC_PER_PAGE			21

#define FDK_PCIBAR_MEM				0x00000000
#define FDK_PCIBAR_IO				0x00000001
#define FDK_PCIBAR_LA_32BIT			0x00000000
#define FDK_PCIBAR_LA_1MIB			0x00000002
#define FDK_PCIBAR_LA_64BIT			0x00000004
#define FDK_PCIBAR_PA_NO			0x00000000
#define FDK_PCIBAR_PA_YES			0x00000008
#define FDK_PCIBAR_MEMBA_MASK		0xFFFFFFF0
#define FDK_PCIBAR_IOBA_MASK		0xFFFFFFFE

#define FDK_MAXADDR_MEM				0xFFFFFFFFULL
#define FDK_MAXADDR_IO				0xFFFFULL
#define FDK_MAXADDR_CMOS			0xFFULL


#define DECLARE_WINDOW( NAME ) \
	PANEL	*panel##NAME; \
	WINDOW	*NAME;


#define printWindow( RESRC, NAME, LINE, COLUMN, X, Y, COLORPAIR, FORMAT, ARGS... ) {\
    RESRC.NAME = newwin( LINE, COLUMN, X, Y );\
    RESRC.panel##NAME = new_panel( RESRC.NAME );\
    wbkgd( RESRC.NAME, COLOR_PAIR( COLORPAIR ) );\
    wattrset( RESRC.NAME, COLOR_PAIR( COLORPAIR ) | A_BOLD );\
    wprintw( RESRC.NAME, FORMAT, ##ARGS );\
    wattrset( RESRC.NAME, A_NORMAL );\
}


#define printWindowAt( RESRC, NAME, LINE, COLUMN, X, Y, COLORPAIR, FORMAT, ARGS... ) {\
    if( !RESRC.NAME ) {\
        RESRC.NAME = newwin( LINE, COLUMN, X, Y );\
        RESRC.panel##NAME = new_panel( RESRC.NAME );\
    }\
    wbkgd( RESRC.NAME, COLOR_PAIR( COLORPAIR ) );\
    wattrset( RESRC.NAME, COLOR_PAIR( COLORPAIR ) | A_BOLD );\
    mvwprintw( RESRC.NAME, 0, 0, FORMAT, ##ARGS );\
    wattrset( RESRC.NAME, A_NORMAL );\
}


#define printWindowMove( RESRC, NAME, LINE, COLUMN, X, Y, COLORPAIR, FORMAT, ARGS... ) {\
    if( RESRC.panel##NAME ) {\
        del_panel( RESRC.panel##NAME );\
        RESRC.panel##NAME = NULL;\
    }\
    if( RESRC.NAME ) {\
        delwin( RESRC.NAME );\
        RESRC.NAME = NULL;\
    }\
    RESRC.NAME = newwin( LINE, COLUMN, X, Y );\
    RESRC.panel##NAME = new_panel( RESRC.NAME );\
    wbkgd( RESRC.NAME, COLOR_PAIR( COLORPAIR ) );\
    wattrset( RESRC.NAME, COLOR_PAIR( COLORPAIR ) | A_BOLD );\
    mvwprintw( RESRC.NAME, 0, 0, FORMAT, ##ARGS );\
    wattrset( RESRC.NAME, A_NORMAL );\
}


#define destroyWindow( RESRC, NAME ) {\
    if( RESRC.panel##NAME ) {\
        del_panel( RESRC.panel##NAME );\
        RESRC.panel##NAME = NULL;\
    }\
    if( RESRC.NAME ) {\
        delwin( RESRC.NAME );\
        RESRC.NAME = NULL;\
    }\
}


typedef enum {

	KBPRS_ESC = 0x1B,
	KBPRS_UP = 0x103,
	KBPRS_DOWN = 0x102,
	KBPRS_LEFT = 0x104,
	KBPRS_RIGHT = 0x105,
	KBPRS_PGUP = 0x153,
	KBPRS_PGDN = 0x152,

	KBPRS_F1 = 0x109,
	KBPRS_F2 = 0x10A,
	KBPRS_F3 = 0x10B,
	KBPRS_F4 = 0x10C,
	KBPRS_F5 = 0x10D,
	KBPRS_F6 = 0x10E,
	KBPRS_F7 = 0x10F,
	KBPRS_F8 = 0x110,
	KBPRS_F9 = 0x111,
	KBPRS_F10 = 0x112,
	KBPRS_F11 = 0x113,
	KBPRS_F12 = 0x114,

	KBPRS_SPACE = 0x20,
	KBPRS_ENTER = 0x0A,

} fdkKeyPress_t;


typedef enum {

	KHF_INIT,
	KHF_MEM,
	KHF_IO,
	KHF_PCI,
	KHF_PCIL,
	KHF_IDE,
	KHF_CMOS,

} fdkHwFunc_t;


typedef enum {

    WHITE_RED = 1,
    WHITE_BLUE,
	WHITE_YELLOW,
	WHITE_BLACK,

    BLACK_WHITE,
	BLACK_GREEN,
	BLACK_YELLOW,
	BLACK_BLUE,
	BLACK_CYAN,

    CYAN_BLUE,
	CYAN_WHITE,

    YELLOW_BLUE,
    YELLOW_RED,
    YELLOW_BLACK,
	YELLOW_WHITE,

	RED_WHITE,
	RED_BLUE,

	MAGENTA_BLUE,

	GREEN_BLUE,
	GREEN_WHITE,

} fdkColorPairs_t;


typedef struct {

	s8					venTxt[ FDK_MAX_PCINAME ];
	s8					devTxt[ FDK_MAX_PCINAME ];

} fdkPciIds_t;


typedef struct PACKED {

	u16					vendorId;
	u16					deviceId;
	u16					command;
	u16					status;
	u8					revisionId;
	u8					classCode1;
	u16					classCode2;
	u8					cacheLine;
	u8					latencyTimer;
	u8					headerType;
	u8					bist;

	u32					baseAddrReg0;
	u32					baseAddrReg1;
	u32					baseAddrReg2;
	u32					baseAddrReg3;
	u32					baseAddrReg4;
	u32					baseAddrReg5;
	
	u32					cardbusCisPointer;
	u16					subSysVendorId;
	u16					subSysId;
	u32					expRomBaseAddr;
	u8					capPointer;
	u8					reserved1;
	u16					reserved2;
	u32					reserved3;
	u8					intLine;
	u8					intPin;
	u8					minGnt;
	u8					maxLatency;

} fdkPciConfig_t;


typedef struct {

	DECLARE_WINDOW( background );
	DECLARE_WINDOW( logo );
	DECLARE_WINDOW( copyright );
	DECLARE_WINDOW( status );
	DECLARE_WINDOW( time );
	DECLARE_WINDOW( help );

	u32					lastSecond;
	s8					*statusStr;
	s32					strIdx;
	u8					toggleHelp;

} fdkBasePanel_t;


typedef struct {

	DECLARE_WINDOW( top );
	DECLARE_WINDOW( offset );
	DECLARE_WINDOW( left );
	DECLARE_WINDOW( rtop );
	DECLARE_WINDOW( value );
	DECLARE_WINDOW( ascii );
	DECLARE_WINDOW( info );
	DECLARE_WINDOW( highlight );
	DECLARE_WINDOW( bits );
	DECLARE_WINDOW( baseaddr );
	DECLARE_WINDOW( hlascii );
	DECLARE_WINDOW( ftitle );
	DECLARE_WINDOW( stitle );

	u64					byteBase;
	s32					byteOffset;
	s8					*infoStr;
	u8					toggleBits;
	u8					toggleEditing;
	u8					editingBuf;

} fdkDumpPanel_t;


typedef struct {

	DECLARE_WINDOW( title );
	DECLARE_WINDOW( content );
	DECLARE_WINDOW( highlight );

	s32					hlIndex;
	s32					pageOffset;

} fdkPciListPanel_t;


typedef struct {

	// General
	s32					fd;
	s32					inputBuf;
	fdkHwFunc_t			fdkHwFunc;
	fdkHwFunc_t			fdkPreviousHwFunc;

	// Packet
	u8					pktBuf[ FDK_MAXSZ_PKT ];
	fdkCommPkt_t		*pFdkCommPkt;

	// Base panel
	fdkBasePanel_t		fdkBasePanel;
	fdkDumpPanel_t		fdkDumpPanel;
	fdkPciListPanel_t	fdkPciListPanel;

	// PCI list
	fdkPciDev_t			*pFdkPciDev;
	fdkPciIds_t			*pFdkPciIds;
	u32					numOfPciDevice;
	s8					pciIdsPath[ FDK_MAX_PATH ];

	// E820 list
	fdkE820record_t		*pFdkE820record;
	u32					numOfE820Record;

} fdkUiProperty_t;


// Prototypes
s32 connectToFdkServer( fdkUiProperty_t *pFdkUiProperty );
void disconnectFromFdkServer( fdkUiProperty_t *pFdkUiProperty );
s32 readPciList( fdkUiProperty_t *pFdkUiProperty );
fdkPciDev_t *getPciDevice( fdkUiProperty_t *pFdkUiProperty, s32 num );
s32 readE820List( fdkUiProperty_t *pFdkUiProperty );
s32 readMemory( fdkUiProperty_t *pFdkUiProperty );
s32 writeMemoryByEditing( fdkUiProperty_t *pFdkUiProperty );
s32 readIo( fdkUiProperty_t *pFdkUiProperty );
s32 writeIoByEditing( fdkUiProperty_t *pFdkUiProperty );
s32 readIde( fdkUiProperty_t *pFdkUiProperty );
s32 writeIdeByEditing( fdkUiProperty_t *pFdkUiProperty );
s32 readCmos( fdkUiProperty_t *pFdkUiProperty );
s32 writeCmosByEditing( fdkUiProperty_t *pFdkUiProperty );
s32 readPci( fdkUiProperty_t *pFdkUiProperty );
s32 writePciByEditing( fdkUiProperty_t *pFdkUiProperty );
u32 calculatePciAddress( u16 bus, u8 dev, u8 func );
s32 getPciVenDevTexts( u16 venid, u16 devid, s8 *ventxt, s8 *devtxt, s8 *pciids );
u32 fdkPciDetectDevice( fdkPciDev_t *pFdkPciDev );

void printPciListBasePanel( fdkUiProperty_t *pFdkUiProperty );
void printPciListUpdatePanel( fdkUiProperty_t *pFdkUiProperty );
void clearPciListBasePanel( fdkUiProperty_t *pFdkUiProperty );
void clearPciListUpdatePanel( fdkUiProperty_t *pFdkUiProperty );
s32 handleKeyPressForPciListPanel( fdkUiProperty_t *pFdkUiProperty );

void printDumpBasePanel( fdkUiProperty_t *pFdkUiProperty );
void printDumpUpdatePanel( fdkUiProperty_t *pFdkUiProperty );
void clearDumpBasePanel( fdkUiProperty_t *pFdkUiProperty );
void clearDumpUpdatePanel( fdkUiProperty_t *pFdkUiProperty );
void handleKeyPressForDumpPanel( fdkUiProperty_t *pFdkUiProperty );

void printPciBasePanel( fdkUiProperty_t *pFdkUiProperty );
void printPciUpdatePanel( fdkUiProperty_t *pFdkUiProperty );
void clearPciBasePanel( fdkUiProperty_t *pFdkUiProperty );
void clearPciUpdatePanel( fdkUiProperty_t *pFdkUiProperty );
void handleKeyPressForPciPanel( fdkUiProperty_t *pFdkUiProperty );


