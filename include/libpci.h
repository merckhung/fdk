#define PCI_ADDR_PORT           0xCF8
#define PCI_DATA_PORT           0xCFC
#define PCI_DATA_PORT_END       0xCFC

#define PCI_ADDR_ENABLE         0x80000000

#define PCI_BUS_MASK            0xFF
#define PCI_DEV_MASK            0x1F
#define PCI_FUNC_MASK           0x07
#define PCI_REG_MASK            0xFF
#define PCI_REG_ALIGN           0x00000003

#define PCI_BUS_OFFSET          16
#define PCI_DEV_OFFSET          11
#define PCI_FUNC_OFFSET         8
#define PCI_REG_OFFSET          2
#define PCI_BITS_BYTE           8

#define FDK_CMOS_ADDR			0x70
#define FDK_CMOS_DATA			0x71


u32 pciBaseAddress( u32 bus, u32 dev, u32 fun, u32 reg );
s32 pciInitialize( void );
u32 pciReadConfDWord( u32 addr );
void pciWriteConfDWord( u32 addr, u32 value );
u8 pciReadConfByte( u32 addr );
void pciWriteConfByte( u32 addr, u8 value );
s32 cmosInitialize( void );


