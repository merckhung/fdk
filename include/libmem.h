#define FDK_MEM_MAXLEN          0x1000
#define FDK_4K_PAGE             0x1000
#define FDK_MAX_FILENAME        40
#define FDK_MAX_PATHNAME        100
#define FDK_MEM_DEV             "/dev/mem"


inline s32 openMemDev( void );
inline void closeMemDev( s32 fd );
volatile void *memMapping( s32 fd, u64 addr, u32 len, u64 *alignOff, u32 *actLen );
s32 memUnmapping( void *phyMem, u32 len );
u8 memReadByte( s32 fd, u64 addr );
u16 memReadWord( s32 fd, u64 addr );
u32 memReadDWord( s32 fd, u64 addr );
u32 memReadBuffer( s32 fd, u64 addr, u32 len, u8 *buf );
u32 memWriteBuffer( s32 fd, u64 addr, u32 len, u8 *buf );
u8 memWriteByte( s32 fd, u64 addr, u8 val );
u16 memWriteWord( s32 fd, u64 addr, u16 val );
u32 memWriteDWord( s32 fd, u64 addr, u32 val );


