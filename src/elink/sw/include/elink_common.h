#ifndef ELINK_COMMON_H
#define ELINK_COMMON_H

// coreid and coreid address from (row, col)
// From e-meshdump.c and epiphany.c
#define COREID_SHIFT 20
#define ADDR_TO_COREID(_addr) (( (_addr) >> COREID_SHIFT ) & 0xFFF)
#define COREID_TO_ADDR(_coreid) ((_coreid) << COREID_SHIFT)
#define COORDS_TO_COREID(_row, _col) (((_row) << 6) | (_col))
#define COREID_TO_ROW(_coreid) ((_coreid) >> 6)
#define COREID_TO_COL(_coreid) ((_coreid) & 0x3f)

// From epiphany-hal-api-local.h
unsigned long ee_rndl_page(unsigned long size);

// Epiphany system registers
// Following registers are defined in elink_regmap.vh
// They should match offset addressed defined in kernel epiphany.h

typedef enum {
    ELINK_BASE       = 0x81000000UL,
    ELINK_RESET      = 0xF0200,
    ELINK_CLK        = 0xF0204,
    ELINK_CHIPID     = 0xF0208,
    ELINK_VERSION    = 0xF020C,
    ELINK_TXCFG      = 0xF0210,
    ELINK_TXSTATUS   = 0xF0214,
    ELINK_TXGPIO     = 0xF0218,
    ELINK_TXMONITOR  = 0xF021C,
    ELINK_TXPACKET   = 0xF0220,
    ELINK_RXCFG      = 0xF0300,
    ELINK_RXSTATUS   = 0xF0304,
    ELINK_RXGPIO     = 0xF0308,
    ELINK_RXOFFSET   = 0xF030C,
    ELINK_RXDELAY0   = 0xF0310, // ERX_IDELAY0
    ELINK_RXDELAY1   = 0xF0314, // ERX_IDELAY1
    ELINK_RXTESTDATA = 0xF0318,

    // TODO see edma_regs.v
    //E_SYS_RXDMACFG    = 0xF0500, // Same numbering as in Epiphany (E_REG_DMA0CONFIG)
    //E_SYS_RXDMASTATUS = 0xF051C, // Same numbering as in Epiphany (E_REG_DMA0STATUS)

    ELINK_MAILBOXLO  = 0xF0730,
    ELINK_MAILBOXHI  = 0xF0734,
    ELINK_MAILBOXSTAT= 0xF0738, // not in "New Epiphany system registers" list

    ELINK_TXMMU      = 0xE0000,
    ELINK_RXMMU      = 0xE8000,
} e_elink_regs_t;

// Core registers documented in epiphany architecture document
typedef enum {
    // following are not on the core - you have to set the ctrlmode to skip the core
    E_REG_IOCONFIG    = 0xF030C, // no documentation but could be like ELINK_RXOFFSET
    E_REG_IOFLAG      = 0xF0318, // documented in the epiphany datasheet - toggles the led
    // E_REG_SYNC        = 0xf031c,
    // E_REG_HALT        = 0xf0320,
    // E_REG_RESET       = 0xf0324,
    // E_REG_LINKDEBUG   = 0xF0328,
} epiphany_regs_id_t;

// Following registers are defined in http://adapteva.com/docs/epiphany_arch_ref.pdf
// These offsets are defined in epiphany-hal-data.h (e-hal.h)
//typedef enum {
//    E_REG_LINKCFG    = 0xF0300, // also known as E_REG_LINKMODE
//    E_REG_CONFIG     = 0xF0400,

//    // Control registers
//    E_REG_ILATST     = 0xf042C,

//    // Node Registers
//    E_REG_MESHCONFIG = 0xF0700,

//    E_REG_COREID     = 0xF0704,

//    E_REG_RESETCORE  = 0xF070C,
//} e_core_reg_id_t;

// Following bits for ELINK_RESET are defined in elink_cfg.v
typedef union __attribute__((packed)) {
    unsigned int reg;
    struct {
        unsigned int tx_soft_reset:1;
        unsigned int rx_soft_reset:1;
    };
} elink_reset_t; // replaces e_sys_reset_t;

typedef union __attribute__((packed)) {
    unsigned int reg;
    struct {
        unsigned int corecol:6;
        unsigned int corerow:6;
    };
    struct {
        unsigned:4;
        unsigned int chip16col:2;
        unsigned:4;
        unsigned int chip16row:2;
    };
} elink_chipid_t; // replaces e_sys_chipid_t

typedef union __attribute__((packed)) {
    unsigned int reg;
    struct {
        unsigned int platform:8;
        unsigned int revision:8;
    };
} elink_version_t; // replaces e_sys_version_t;

typedef union __attribute__((packed)) {
    unsigned int reg;
    struct {
        unsigned int enable:1;
        unsigned int mmu_enable:1;
        unsigned int remap_cfg:2;
        unsigned int ctrlmode:4;
        unsigned int :1;
        unsigned int ctrlmode_bypass:1;
        unsigned int burst_enable:1;
        unsigned int transmit_mode:3;
    };
} elink_txcfg_t; // replaces e_sys_txcfg_t;

typedef union __attribute__((packed)) {
    unsigned int reg;
    struct {
        unsigned int test_mode:1;
        unsigned int mmu_enable:1;
        unsigned int remap_mode:2;
        unsigned int remap_sel:12;
        unsigned int remap_pattern:12;
        unsigned int mailbox_irq_en:1;
    };
} elink_rxcfg_t; // replaces e_sys_rxcfg_t;

typedef union __attribute__((packed)) {
    unsigned int reg;
    struct {
        unsigned int dma_en:1;
        unsigned int master_mode:1;
        unsigned int chain_mode:1;
        unsigned int manual_mode:1;
        unsigned int irq_mode:1;
        unsigned int data_mode:2;
        unsigned int :9;
        unsigned int next_descr:16;
    };
} elink_dmacfg_t;

#define EPIPHANY_DEV         "/dev/epiphany/mesh0"
#define ESYS_REGS_BASE       (ELINK_BASE)
#define EPIPHANY_BASE        (0x80800000UL)
#define EMEM_BASE_ADDRESS    (0x8E000000UL)

#endif
