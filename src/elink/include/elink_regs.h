#ifndef ELINK_REGS_H
#define ELINK_REGS_H

// New Epiphany system registers - several registers are different to Parallella version

// define PARALLELLA_REGISTERS matches elink/hdl/elink_regmap.vh
#define PARALLELLA_REGISTERS // undefine / comment out for original "New Epiphany system registers"
typedef enum {
    // offset values used when calling mmap
    // EGROUP_MMR / EGROUP_TX block
    ELINK_RESET      = 0xF0200,
    ELINK_CLK        = 0xF0204,
    ELINK_CHIPID     = 0xF0208,
    ELINK_VERSION    = 0xF020C,

    // TX EGROUP_MMR / EGROUP_TX block
    ELINK_TXCFG      = 0xF0210,
    ELINK_TXSTATUS   = 0xF0214,
    ELINK_TXGPIO     = 0xF0218,
    ELINK_TXMONITOR  = 0xF021C,
    ELINK_TXPACKET   = 0xF0220,

    // EGROUP_MMU
    ELINK_TXMMU      = 0xE0000,

    // RX EGROUP_MMR / EGROUP_RX block
    ELINK_RXCFG      = 0xF0300,
    ELINK_RXSTATUS   = 0xF0304,
    ELINK_RXGPIO     = 0xF0308,
    ELINK_RXOFFSET   = 0xF030C,
#ifdef PARALLELLA_REGISTERS
    // EGROUP_MMR / EGROUP_MESH
    ELINK_MAILBOXLO  = 0xF0730,
    ELINK_MAILBOXHI  = 0xF0734,
    ELINK_MAILBOXSTAT= 0xF0738, // not in Epiphany system registers list

    // EGROUP_MMR / EGROUP_RX block
    ELINK_RXDELAY0   = 0xF0310, // ERX_IDELAY0
    ELINK_RXDELAY1   = 0xF0314, // ERX_IDELAY1
    ELINK_RXDATA     = 0xF0318, // ERX_TESTDATA
#else
    ELINK_MAILBOXLO  = 0xF0310,
    ELINK_MAILBOXHI  = 0xF0314,
    ELINK_RXDELAY0   = 0xF0318,
    ELINK_RXDELAY1   = 0xF031C,
    ELINK_RXDATA     = 0xF0320,
#endif
    // EGROUP_MMU
    ELINK_RXMMU      = 0xE8000,

    // EGROUP_DMA: TODO 0xF0500 - similar numbering to Epiphany E_REG_DMA
    // EGROUP_RR: TODO 0xD0000

    // register values (0x810 is to the east of the 16 core Epiphany @ 0x808)
    ELINK_BASE            = 0x81000000,
    ELINK_RESET_ADDR      = ELINK_BASE + ELINK_RESET,
    ELINK_CLK_ADDR        = ELINK_BASE + ELINK_CLK,
    ELINK_CHIPID_ADDR     = ELINK_BASE + ELINK_CHIPID,
    ELINK_VERSION_ADDR    = ELINK_BASE + ELINK_VERSION,
    //TX
    ELINK_TXCFG_ADDR      = ELINK_BASE + ELINK_TXCFG,
    ELINK_TXSTATUS_ADDR   = ELINK_BASE + ELINK_TXSTATUS,
    ELINK_TXGPIO_ADDR     = ELINK_BASE + ELINK_TXGPIO,
    ELINK_TXMMU_ADDR      = ELINK_BASE + ELINK_TXMMU,
    //RX
    ELINK_RXCFG_ADDR      = ELINK_BASE + ELINK_RXCFG,
    ELINK_RXSTATUS_ADDR   = ELINK_BASE + ELINK_RXSTATUS,
    ELINK_RXGPIO_ADDR     = ELINK_BASE + ELINK_RXGPIO,
    ELINK_RXOFFSET_ADDR   = ELINK_BASE + ELINK_RXOFFSET,
    ELINK_MAILBOXLO_ADDR  = ELINK_BASE + ELINK_MAILBOXLO,
    ELINK_MAILBOXHI_ADDR  = ELINK_BASE + ELINK_MAILBOXHI,
#ifdef PARALLELLA_REGISTERS
    ELINK_MAILBOXSTAT_ADDR= ELINK_BASE + ELINK_MAILBOXSTAT,
#endif
    ELINK_RXDELAY0_ADDR   = ELINK_BASE + ELINK_RXDELAY0,
    ELINK_RXDELAY1_ADDR   = ELINK_BASE + ELINK_RXDELAY1,
    ELINK_RXDATA_ADDR     = ELINK_BASE + ELINK_RXDATA,
    ELINK_RXMMU_ADDR      = ELINK_BASE + ELINK_RXMMU,
} elink_regs_id_t;

// documented in Epiphany datasheet
// parallella sets Epiphany at position row=32=0x20 col=8=0x08 (32,8) -> (35,11) ie 0x808 -> 0x8CB
#define COREID_SHIFT 20
#define ADDR_TO_COREID(_addr) ((_addr) >> COREID_SHIFT)
#define COREID_TO_ADDR(_coreid) ((_coreid) << COREID_SHIFT)
#define COORDS_TO_COREID(_row, _col) (((_row) << 6) | (_col))
#define COREID_TO_ROW(_coreid) ((_coreid) >> 6)
#define COREID_TO_COL(_coreid) ((_coreid) & 0x3f)

// NORTH_LINK   = 0x00200000,
// EAST_LINK    = 0x08300000,
// SOUTH_LINK   = 0x0C200000,
// WEST_LINK    = 0x08000000,

// add link offset to this address (see also epiphany-hal-data.h / epiphany.h for these)
typedef enum {
    E_REG_LINKCFG    = 0xF0300, // also known as E_REG_LINKMODE
    E_REG_TXCFG      = 0xF0304, // also known as E_REG_LINKTXCFG
    E_REG_RXCFG      = 0xF0308, // also known as E_REG_LINKRXCFG
#ifndef PARALLELLA_REGISTERS
    E_REG_GPIOCFG    = 0xF030C, // new to "New Epiphany system registers"
#endif
    E_REG_IOCONFIG    = 0xF030C,
    E_REG_IOFLAG      = 0xF0318, // also known as E_REG_FLAGCFG
    E_REG_SYNC        = 0xf031c,
    E_REG_HALT        = 0xf0320,
    E_REG_RESET       = 0xf0324,
    E_REG_LINKDEBUG   = 0xF0328, // also known as E_REG_LINKDEBUG
} epiphany_regs_id_t;

//FOR LEGACY ONLY!
typedef enum {
    E_SYS_BASE       = 0x70000000,
    E_SYS_RESET      = E_SYS_BASE + 0x0040,
    E_SYS_CFGTX      = E_SYS_BASE + 0x0044,
    E_SYS_CFGRX      = E_SYS_BASE + 0x0048,
    E_SYS_CFGCLK     = E_SYS_BASE + 0x004c,
    E_SYS_COREID     = E_SYS_BASE + 0x0050,
    E_SYS_VERSION    = E_SYS_BASE + 0x0054,
    E_SYS_GPIOIN     = E_SYS_BASE + 0x0058,
    E_SYS_GPIOOUT    = E_SYS_BASE + 0x005c
} e_sys_reg_id_t;

#endif
