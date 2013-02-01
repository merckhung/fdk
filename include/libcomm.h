//
// Definitions
//
#define FDK_STD_IN					0
#define FDK_STD_OUT					1
#define FDK_STD_ERR					2
#define FDK_CLEAR_SCREEN			"\033[2J\033[f"
#define FDK_SZ_SECTOR				512
#define FDK_MAX_PATHNAME			100
#define IP_STR_LEN_MIN              7
#define IP_STR_LEN                  15
#define IP_STR_BUF                  (IP_STR_LEN + 1)


//
// Macros
//
#define FDK_GET_BIT( val, bit )		((val & (1 << bit)) >> bit)
#define FDK_BIT_MASK( x )			(1 << x)
#define FDK_ARRAY_NRCELL( x )		(sizeof(x)/sizeof(x[0]))
#define FDK_ENUM_TOSTR( NAME )		#NAME


//
// Structures
//
typedef struct _commonLinklist {

	struct _commonLinklist  *next;

} commonLinklist_t;


//
// Prototypes
//
s32 CbPower( s32 x, s32 y );
s8 CbAsciiToBin( s8 value );
u32 CbAsciiBufToBin( const s8 *buf );
bool ParseOneParameter( s8 *buf, u32 *first );
bool ParseTwoParameters( s8 *buf, u32 *first, u32 *second );
s8 ConvertDWordToByte( u32 *Data, u32 Offset );
void DumpData( s8 *pBuf, u32 size, u32 base );
void DisplayInBits( u32 value );
void ClrScr( void );
s8 NonBlockReadKey( void );
bool ReadLine( s8 *Buf, u32 Length );
s8 GetKey( void );

s32 isIPv4Format( const s8 *str );

int countLinklist( commonLinklist_t *head );
commonLinklist_t **tailOfLinklist( commonLinklist_t **head );
void appendLinklist( commonLinklist_t **head, commonLinklist_t *object );
commonLinklist_t *retriveFirstLinklist( commonLinklist_t **head );
void freeLinklist( commonLinklist_t *head );


