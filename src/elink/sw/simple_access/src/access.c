#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <e-hal.h>
#include <e-loader.h>
#include <fcntl.h>
#include <getopt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "elink_common.h"

extern int e_host_verbose; // use same settings as inside e-lib
FILE *diag_fd;             // only available in the e-lib but never the less useful in user code
#define diag(vN) if (e_host_verbose >= vN)

#define INPUT_SIZE (1000)

#define TAPS 64

#define N_ROWS = (4)
#define N_COLS = (4)
#define N_CORES = (N_ROWS * N_COLS)

#define _TimeSize  (20)
static int time_display = 0;
static long time_offset = 0;
static long last_nsec = 0;

static int          elink_mmap_fd = 0;
static e_epiphany_t *pecore_dev = NULL;
static e_epiphany_t ecore_dev;
static e_mmap_t     elink_tx_rx_mbuf = {.mapped_base = MAP_FAILED};
// might as well use e-hal for accesses to the ecore for now
static e_mmap_t     ecore_reg_mbuf = {.mapped_base = MAP_FAILED};
static e_mmap_t     ecore_local_mbuf = {.mapped_base = MAP_FAILED};
static e_mmap_t     global_mbuf = {.mapped_base = MAP_FAILED};

// Forward declarations
int ae_reset_system(void);
void prompt_wait(const char *);
inline static int get_uptime(struct timespec *t);
inline static double get_duration(struct timespec *start, struct timespec *end);
inline static char* get_time(char * buf, struct timespec *start);

static int get_uptime(struct timespec *t)
{
    return clock_gettime(CLOCK_MONOTONIC, t);
}

static double get_duration(struct timespec *start, struct timespec *end)
{
    return (double)(end->tv_sec - start->tv_sec) + (double)(end->tv_nsec - start->tv_nsec) / 1000000000;
}

static char* get_time(char * buf, struct timespec *start)
{
    long tv_sec = start->tv_sec;
    long tv_nsec = start->tv_nsec;
    last_nsec = start->tv_nsec;

    tv_nsec += time_offset;
    tv_nsec = tv_nsec % 1000000000;
    if (1000000000 < start->tv_nsec + time_offset) tv_sec++;
    sprintf(buf, "[ %ld.%06ld]", tv_sec, tv_nsec/1000);

    return buf;
}

static void close_elink_mem()
{
    if (elink_mmap_fd) {
        if (elink_tx_rx_mbuf.mapped_base != MAP_FAILED) {
            diag(H_D1) { fprintf(diag_fd, "close_elink_mem() munmap elink_tx_rx_mbuf\n"); }
            munmap(elink_tx_rx_mbuf.mapped_base, elink_tx_rx_mbuf.map_size);
            elink_tx_rx_mbuf.mapped_base = MAP_FAILED;
        }

        if (ecore_reg_mbuf.mapped_base != MAP_FAILED) {
            diag(H_D1) { fprintf(diag_fd, "close_elink_mem() munmap ecore_reg_mbuf\n"); }
            munmap(ecore_reg_mbuf.mapped_base, ecore_reg_mbuf.map_size);
            ecore_reg_mbuf.mapped_base = MAP_FAILED;
        }

        if (ecore_local_mbuf.mapped_base != MAP_FAILED) {
            diag(H_D1) { fprintf(diag_fd, "close_elink_mem() munmap ecore_local_mbuf\n"); }
            munmap(ecore_local_mbuf.mapped_base, ecore_local_mbuf.map_size);
            ecore_local_mbuf.mapped_base = MAP_FAILED;
        }

        if (global_mbuf.mapped_base != MAP_FAILED) {
            diag(H_D1) { fprintf(diag_fd, "close_elink_mem() munmap global_mbuf\n"); }
            munmap(global_mbuf.mapped_base, global_mbuf.map_size);
            global_mbuf.mapped_base = MAP_FAILED;
        }

        diag(H_D1) { fprintf(diag_fd, "close_elink_mem() close elink_mmap_fd\n"); }
        close(elink_mmap_fd);
        elink_mmap_fd = 0;
    }

    if (pecore_dev) {
        diag(H_D1) { fprintf(diag_fd, "close_elink_mem() close ecore_dev and finalize\n"); }
        e_close(&ecore_dev);
        pecore_dev = NULL;
        e_finalize();
    }
}

static int open_elink_mem(e_mmap_t *mbuf, off_t base, off_t addr)
{
    if (elink_mmap_fd <= 0) {
        diag(H_D1) { fprintf(diag_fd, "open_elink_mem() open elink_mmap_fd\n"); }
        elink_mmap_fd = open(EPIPHANY_DEV, O_RDWR | O_SYNC);
        if (elink_mmap_fd <= 0) {
            warnx("open_elink_mem(): EPIPHANY_DEV file open failure.");
            return E_ERR;
        }
    }

    mbuf->phy_base = base + addr;
    mbuf->page_base = ee_rndl_page(mbuf->phy_base);
    mbuf->page_offset = base + addr - mbuf->page_base;
    mbuf->map_size = sysconf(_SC_PAGE_SIZE);

    mbuf->mapped_base = mmap(NULL, mbuf->map_size, PROT_READ|PROT_WRITE, MAP_SHARED, elink_mmap_fd, mbuf->page_base);
    mbuf->base = mbuf->mapped_base;

    // page_offset = 0x%08x,
    diag(H_D1) { fprintf(diag_fd, "open_elink_mem(): phy_base = 0x%08x, page_base = 0x%08x, base = 0x%08x, size = 0x%08x\n", (uint) mbuf->phy_base, (uint) mbuf->page_base, (uint) mbuf->base, (uint) mbuf->map_size); }

    if (mbuf->mapped_base == MAP_FAILED) {
        warnx("open_elink_mem(): mmap failure. Have you enabled unsafe access?");
        close_elink_mem();
        return E_ERR;
    }

    return E_OK;
}

static int ee_write_elink_mem(e_mmap_t *mbuf, off_t base, off_t to_addr, uint32_t *data)
{
    volatile uint32_t *pto;

    off_t page_base = ee_rndl_page(base + to_addr);
    off_t page_offset = base + to_addr - page_base;
    int rtn = E_OK;
    if (mbuf->mapped_base != MAP_FAILED) {
        if (mbuf->page_base != page_base) {
            munmap(mbuf->mapped_base, mbuf->map_size);
            mbuf->mapped_base = MAP_FAILED;

            rtn = open_elink_mem(mbuf, base, to_addr);
        }    
    } else {
        rtn = open_elink_mem(mbuf, base, to_addr);
    }

    if (E_OK != rtn) {
        warnx("ee_write_elink_mem() mmap failed!\n");
        return rtn;
    }

    pto = (volatile uint32_t *) (mbuf->base + page_offset);
    diag(H_D2) { fprintf(diag_fd, "ee_write_elink_mem(): writing to to_addr=0x%08x, pto=0x%08x\n", (uint) (mbuf->page_base + page_offset), (uint) pto); }
    *pto = *data;

    return rtn;
}

static int ee_read_elink_mem(e_mmap_t *mbuf, off_t base, off_t from_addr, uint32_t *data)
{
    volatile uint32_t *pfrom;

    off_t page_base = ee_rndl_page(base + from_addr);
    off_t page_offset = base + from_addr - page_base;
    int rtn = E_OK;
    if (mbuf->mapped_base != MAP_FAILED) {
        if (mbuf->page_base != page_base) {
            munmap(mbuf->mapped_base, mbuf->map_size);
            mbuf->mapped_base = MAP_FAILED;

            rtn = open_elink_mem(mbuf, base, from_addr);
        }    
    } else {
        rtn = open_elink_mem(mbuf, base, from_addr);
    }

    if (E_OK != rtn) {
        warnx("ee_read_elink_mem() mmap failed!\n");
        return rtn;
    }

    pfrom = (volatile uint32_t *) (mbuf->base + page_offset);
    diag(H_D2) { fprintf(diag_fd, "ee_read_elink_mem(): reading from_addr=0x%08x, pto=0x%08x\n", (uint) (mbuf->page_base + page_offset), (uint) pfrom); }
    *data  = *pfrom;

    return rtn;
}

static int ee_write_elink_reg(off_t to_addr, uint32_t *data)
{
    diag(H_D2) { fprintf(diag_fd, "ee_write_elink_reg register: 0x%x\n", to_addr); }

    return ee_write_elink_mem(&elink_tx_rx_mbuf, ELINK_BASE, to_addr, data);
}

static int ee_read_elink_reg(off_t from_addr, uint32_t *rdata)
{
    diag(H_D2) { fprintf(diag_fd, "ee_read_elink_reg register: 0x%x\n", from_addr); }

    return ee_read_elink_mem(&elink_tx_rx_mbuf, ELINK_BASE, from_addr, rdata);;
}

static int elink_write(off_t dstaddr, uint32_t *data)
{
    int rtn = E_OK;
    uint32_t coreid = ADDR_TO_COREID(dstaddr);
    off_t addr = dstaddr - COREID_TO_ADDR(coreid);
    off_t base = COREID_TO_ADDR(coreid);

    diag(H_D2) { fprintf(diag_fd, "elink_write dstaddr: 0x%x coreid: 0x%x addr: 0x%x\n", dstaddr, coreid, addr); }

    // possibles are 0x810  0x808... 0x8e0... 0x3e...
    if (coreid == 0x810) {
        diag(H_D2) { fprintf(diag_fd, "elink_write elink register: 0x%x\n", addr); }
        rtn = ee_write_elink_mem(&elink_tx_rx_mbuf, base, addr, data);
    } else if ( (0x8e0 <= coreid && coreid < 0x900) ) {
        diag(H_D2) { fprintf(diag_fd, "elink_write mem base: 0x%x address: 0x%x\n", base, addr); }
        rtn = ee_write_elink_mem(&global_mbuf, base, addr, data);
    } else {
        uint32_t n_row = COREID_TO_ROW(coreid);
        uint32_t n_col = COREID_TO_COL(coreid);
        //if (32 <= n_row && n_row < 36 && 8 <= n_col && n_col < 12) {printf("elink_write ecore: 0x%x address: 0x%x\n", coreid, addr);}
        if (0xF0000 <= addr) {
            diag(H_D2) { fprintf(diag_fd, "elink_write ecore reg: 0x%x address: 0x%x\n", base, addr); }
            rtn = ee_write_elink_mem(&ecore_local_mbuf, base, addr, data);
        } else {
            diag(H_D2) { fprintf(diag_fd, "elink_write ecore mem: 0x%x address: 0x%x\n", base, addr); }
            rtn = ee_write_elink_mem(&ecore_local_mbuf, base, addr, data);
        }
    }
    
    return rtn;
}

static int elink_read(off_t dstaddr, uint32_t *rdata)
{
    int rtn = E_OK;
    uint32_t coreid = ADDR_TO_COREID(dstaddr);
    off_t addr = dstaddr - COREID_TO_ADDR(coreid);
    off_t base = COREID_TO_ADDR(coreid);

    diag(H_D2) { fprintf(diag_fd, "elink_read dstaddr: 0x%x coreid: 0x%x addr: 0x%x\n", dstaddr, coreid, addr); }

    // possibles are 0x810  0x808... 0x8e0...
    if (coreid == 0x810) {
        diag(H_D2) { fprintf(diag_fd, "elink_read elink register: 0x%x\n", addr); }
        rtn = ee_read_elink_mem(&elink_tx_rx_mbuf, base, addr, rdata);
    } else if (0x8e0 <= coreid && coreid < 0x900) {
        diag(H_D2) { fprintf(diag_fd, "elink_read mem base: 0x%x address: 0x%x\n", base, addr); }
        rtn = ee_read_elink_mem(&global_mbuf, base, addr, rdata);
    } else {
        uint32_t n_row = COREID_TO_ROW(coreid);
        uint32_t n_col = COREID_TO_COL(coreid);
        //if (32 <= n_row && n_row < 36 && 8 <= n_col && n_col < 12) {printf("elink_read ecore: 0x%x address: 0x%x\n", coreid, addr);}

        if (0xF0000 <= addr) {
            diag(H_D2) { fprintf(diag_fd, "elink_read ecore reg: 0x%x address: 0x%x\n", base, addr); }
            rtn = ee_read_elink_mem(&ecore_reg_mbuf, base, addr, rdata);
        } else {
            diag(H_D2) { fprintf(diag_fd, "elink_read ecore mem: 0x%x address: 0x%x\n", base, addr); }
            rtn = ee_read_elink_mem(&ecore_local_mbuf, base, addr, rdata);
        }
    }

    return rtn;
}

void set_idelay(int n_idelay)
{
    uint32_t idelay[TAPS]={0x00000000,0x00000000,//0
		  0x11111111,0x00000001,//1
		  0x22222222,0x00000002,//2
		  0x33333333,0x00000003,//3
		  0x44444444,0x00000004,//4
		  0x55555555,0x00000005,//5
		  0x66666666,0x00000006,//6
		  0x77777777,0x00000007,//7
		  0x88888888,0x00000008,//8
		  0x99999999,0x00000009,//9
		  0xaaaaaaaa,0x0000000a,//10
		  0xbbbbbbbb,0x0000000b,//11
		  0xcccccccc,0x0000000c,//12
		  0xdddddddd,0x0000000d,//13
		  0xeeeeeeee,0x0000000e,//14
		  0xffffffff,0x0000000f,//15
		  0x00000000,0x00000010,//16
		  0x11111111,0x00000011,//17
		  0x22222222,0x00000012,//18
		  0x33333333,0x00000013,//29
		  0x44444444,0x00000014,//20
		  0x55555555,0x00000015,//21
		  0x66666666,0x00000016,//22
		  0x77777777,0x00000017,//23
		  0x88888888,0x00000018,//24
		  0x99999999,0x00000019,//25
		  0xaaaaaaaa,0x0000001a,//26
		  0xbbbbbbbb,0x0000001b,//27
		  0xcccccccc,0x0000001c,//28
		  0xdddddddd,0x0000001d,//29
		  0xeeeeeeee,0x0000001e,//30
		  0xffffffff,0x0000001f};//31

    ee_write_elink_reg(ELINK_RXDELAY0, idelay + 2*n_idelay);
    ee_write_elink_reg(ELINK_RXDELAY1, idelay + 2*n_idelay + 1);

    printf ("DELAY=%08x ",idelay[2*n_idelay]);
}

int ae_reset_system(void)
{
	int rc = 0;
	uint32_t divider;
	uint32_t chipid;
	elink_txcfg_t txcfg         = { .reg = 0 };
	elink_rxcfg_t rxcfg         = { .reg = 0 };
	elink_dmacfg_t rx_dmacfg    = { .reg = 0 };
	// elink_clkcfg_t clkcfg       = { .reg = 0 };
	elink_reset_t resetcfg      = { .reg = 0 };
	e_epiphany_t recore_dev;

#if 1
	resetcfg.tx_soft_reset = 1;
	resetcfg.rx_soft_reset = 1;
    uint32_t reg = resetcfg.reg;
	if (E_OK != ee_write_elink_reg(ELINK_RESET, &reg)) {
		printf ("ae_reset_system(): ELINK_RESET failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);

	/* Do we need this ? */
	resetcfg.tx_soft_reset = 0;
	resetcfg.rx_soft_reset = 0;
    reg = resetcfg.reg;
	if (E_OK != ee_write_elink_reg(ELINK_RESET, &reg)) {
		printf ("ae_reset_system(): clear ELINK_RESET failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif

#if 0 // ???
	chipid = 0x808 /* >> 2 */;
	if (E_OK != ee_write_elink_reg(E_REG_COREID, &chipid /* << 2 */)) {
		printf ("ae_reset_system(): E_REG_COREID failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif

#if 1
	txcfg.enable = 1;
	txcfg.mmu_enable = 0;
    reg = txcfg.reg;
	if (E_OK != ee_write_elink_reg(ELINK_TXCFG, &reg)) {
		printf ("ae_reset_system(): ELINK_TXCFG failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif

	rxcfg.test_mode = 0; /* bug/(feature?) workaround */
	rxcfg.mmu_enable = 0;
	rxcfg.remap_mode = 1; //"static" remap_addr
	rxcfg.remap_sel = 0xfe0; // should be 0xfe0 ???
	rxcfg.remap_pattern = 0x3e0;
    reg = rxcfg.reg;
	if (E_OK != ee_write_elink_reg(ELINK_RXCFG, &reg)) {
		printf ("ae_reset_system(): ELINK_RXCFG failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);

#if 0 // ?
	rx_dmacfg.enable = 1;
    reg = rx_dmacfg.reg;
	if (E_OK != ee_write_elink_reg(ELINK_RXDMACFG, &reg)) {
		printf ("ae_reset_system(): ELINK_RXDMACFG failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif
	rc = E_ERR;

	if ( E_OK != e_open(&recore_dev, 2, 3, 1, 1) ) {
		warnx("e_reset_system(): e_open() failure.");
		goto err;
	}

	txcfg.ctrlmode = 0x5; /* Force east */
	//txcfg.ctrlmode_select = 0x1; /* */
	usleep(1000);
    reg = txcfg.reg;
	if (E_OK != ee_write_elink_reg(ELINK_TXCFG, &reg)) {
		printf ("ae_reset_system(): ELINK_TXCFG force east failed"); fflush(stdout);
		goto cleanup_platform;
	}

	//divider = 2; /* Divide by 8, see data sheet */
	//divider = 1; /* Divide by 4, see data sheet */
	divider = 0; /* Divide by 2, see data sheet */
	usleep(1000);
	if (sizeof(int) != e_write(&recore_dev, 0, 0, E_REG_LINKCFG, &divider, sizeof(int))) {
		printf ("ae_reset_system(): E_REG_LINKCFG failed"); fflush(stdout);
		goto cleanup_platform;
	}

	txcfg.ctrlmode = 0x0;
	//txcfg.ctrlmode_select = 0x0; /* */
	usleep(1000);
    reg = txcfg.reg;
	if (E_OK != ee_write_elink_reg(ELINK_TXCFG, &reg)) {
		printf ("ae_reset_system(): E_REG_TXCFG failed"); fflush(stdout);
		goto cleanup_platform;
	}

	rc = E_OK;

cleanup_platform:
	e_close(&recore_dev);

	usleep(1000);
	return E_OK;

err:
	warnx("e_reset_system(): Failed\n");
	usleep(1000);
	return E_ERR;
}

void parse_args(char *cmdpar, char **strrtn, int n_ints, int *intrtn)
{
    static const char *ws = " \t"; // token separators

    int n_int = 0;
    while (cmdpar) {
        if (isspace(cmdpar[0])) {
            // includes spaces, tab etc
            diag(H_D3) { fprintf(diag_fd, "process_args skip spaces cmdpar: \"%s\"\n", cmdpar); }
            cmdpar += strspn(cmdpar, ws);
            if (!strlen(cmdpar)) {
                // end of string
                break;
            }
        }

        // Save location of the option
        char *option = cmdpar;

        for (int c = 0; c < strlen(ws); c++) {
            // Split at next separator
            char *cmdterm = strchr(cmdpar, (int)ws[c]);
            if (cmdterm) {
                *cmdterm = 0;
                cmdpar = cmdterm + 1;
                diag(H_D3) { fprintf(diag_fd, "process_args terminate option: \"%s\"\n", option); }
                break;
            } else if (c + 1 == strlen(ws)) {
                // end of line
                cmdpar = cmdterm;
            }
        }

        if (isdigit((int)option[0])) {
            if (n_int < n_ints) {
                *intrtn = strtol(option, NULL, 0);
                diag(H_D3) { fprintf(diag_fd, "parse_args int: 0x%x\n", *intrtn); }
                intrtn++;
                n_int++;
            } else {
                diag(H_D3) { fprintf(diag_fd, "parse_args too many numbers skipped: %s\n", option); }
            }
        } else {
            *strrtn = option;
            diag(H_D3) { fprintf(diag_fd, "parse_args str: %s\n", *strrtn); }
        }

        diag(H_D3) { fprintf(diag_fd, "parse_args remaining: \"%s\"\n", cmdpar); }
    }
}

int ae_open(char *cmdpar)
{
    int rtn = E_OK;
    char *filenm;
    int n_ints = 4;
    int val[n_ints];

    if (!pecore_dev) {
        parse_args(cmdpar, &filenm, n_ints, val);
        diag(H_D1) { fprintf(diag_fd, "ae_open"); for (int n_int = 0; n_int < n_ints; n_int++) { fprintf(diag_fd, " %d", val[n_int]); } fprintf(diag_fd, "\n"); }

        rtn = e_open(&ecore_dev, val[0], val[1], val[2], val[3]);
        if (E_OK == rtn) pecore_dev = &ecore_dev;
    }

    return rtn;
}

int ae_load_group(char* cmdpar)
{
    int rtn = E_OK;
    char *filenm;
    int n_ints = 5;
    int val[n_ints];

    parse_args(cmdpar, &filenm, n_ints, val);
    diag(H_D2) { fprintf(diag_fd, "ae_load_group %s", filenm); for (int n_int = 0; n_int < n_ints; n_int++) { fprintf(diag_fd, " %d", val[n_int]); } fprintf(diag_fd, "\n"); }

    if (pecore_dev) {
        rtn = e_load_group(filenm, pecore_dev, val[0], val[1], val[2], val[3], val[4]);
    }

    return rtn;
}

int process_args(char *cmd, char *cmdpar, char *line)
{
    char check[INPUT_SIZE];

    sprintf(check, "%.*s", strlen(cmd), line);
    sprintf(cmdpar, "%.*s", strlen(line) - strlen(cmd), line + strlen(cmd));

    return strcmp(check, cmd) == 0;
}

void process_cmd(char *line)
{
    char cmdpar[INPUT_SIZE];

    if (process_args("//cmd_reset_system", cmdpar, line)) {
        diag(H_D2) { fprintf(diag_fd, "process_cmd: reset_system parm: %s\n", cmdpar); }
        if (E_OK != ae_reset_system()) abort();
    } else if (process_args("//cmd_e_init", cmdpar, line)) {
        diag(H_D2) { fprintf(diag_fd, "process_cmd: e_init parm: %s\n", cmdpar); }
        if (E_OK != e_init(NULL)) abort();
    } else if (process_args("//cmd_e_open", cmdpar, line)) {
        diag(H_D2) { fprintf(diag_fd, "process_cmd: e_open parm: %s\n", cmdpar); }
        if (E_OK != ae_open(cmdpar)) abort();
    } else if (process_args("//cmd_e_load_group", cmdpar, line)) {
        diag(H_D2) { fprintf(diag_fd, "process_cmd: e_load_group parm: %s\n", cmdpar); }
        if (E_OK != ae_load_group(cmdpar)) abort();
    } else if (process_args("//cmd_e_start_group", cmdpar, line)) {
        diag(H_D2) { fprintf(diag_fd, "process_cmd: e_start_group\n", cmdpar); }
        if (E_OK != e_start_group(&ecore_dev)) abort();
    } else if (process_args("//cmd_time_cmd", cmdpar, line)) {
        diag(H_D2) { fprintf(diag_fd, "process_cmd: display time\n", cmdpar); }
        time_display = 1;
    }
}

void process_emf(char *line)
{
    uint32_t dstaddr, rdata, data, srcaddr, ctrlmode, datamode, write;
    uint32_t command, wait_cycles;
    wait_cycles = 0;

    //sscanf (line,"%x_%x_%x_%x_%x", &srcaddr, &data, &dstaddr, &command, &wait_cycles);
    sscanf (line,"%x_%x_%x_%x", &srcaddr, &data, &dstaddr, &command);

    // Parse command field
    write    = command & 0x01;
    ctrlmode = (command & 0xF8)>>3; // TODO:implement
    datamode = (command & 0x06)>>1; // TODO:implement later

    // Access
    int rtn = E_OK;
    if(write) {
        rtn = elink_write(dstaddr, &data);
    } else{
        rtn = elink_read(dstaddr, &rdata);
        printf("[%08x]=0x%08x\n", dstaddr, rdata);
    }

    if (E_OK != rtn) abort();
}

void prompt_wait(const char * announce)
{
    printf ("%s: press enter to continue? ", announce);
    char ch;
    while ((ch = getchar()) != EOF && ch != '\n');
}

void print_usage(const char* argv0)
{
    printf("Usage: %s [--user_time|-u usec] [--dmesg_time|-d usec] filename\n",
           argv0);
}

int main(int argc, char *argv[])
{
    int c;
    long user_time = 0;
    long dmesg_time = 0;
    struct timespec stime, etime;
    char cstime[_TimeSize], cetime[_TimeSize];

    static struct option long_options[] = {
        {"user_time",     optional_argument, 0, 'u'},
        {"dmesg_time",    optional_argument, 0, 'd'},
        {0, 0, 0, 0}
    };

    while (1) {
        /* getopt_long stores the option index here. */
        int option_index = 0;
    
        c = getopt_long (argc, argv, "u:d:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
        case 0:
            /* this should never be able to happen as all options have both
             * short and long flags. */
            printf ("unsupported option %s", long_options[option_index].name);
            if (optarg)
                printf (" with arg %s", optarg);
            printf ("\n");
            exit(EXIT_FAILURE);
            break;

        case 'u':
            user_time = (long) strtol(optarg, NULL, 10) * 1000;
            break;

        case 'd':
            dmesg_time = (long) strtol(optarg, NULL, 10) * 1000;
            break;

        case '?':
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
            break;

        default:
            abort();
        }
    }

    int correction = 2161;
    if (user_time && dmesg_time) time_offset = dmesg_time - user_time + correction*1000;

    if (optind >= argc) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    e_set_host_verbosity(H_D0);
    diag_fd = stderr; // only available locally
    printf("e_host_verbose: 0x%x diag_fd: 0x%x\n", e_host_verbose, diag_fd);

    if (time_display) printf("user_time: %d dmesg_time: %d ", user_time, dmesg_time);
    printf("reading emf file: %s\n", argv[optind]);

    // read Epiphany Memory Format file
    FILE *fd = fopen(argv[optind], "r+");
    if (fd == NULL) {
        printf("ERROR: file open fails for %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }

    for (char line[INPUT_SIZE]; fgets(line, sizeof(line), fd) != NULL;) {
        int time_display_active = time_display;
        if (!time_display_active) fprintf(stderr, "line: %s", line);
        if (time_display_active) get_uptime(&stime);
        if (line[0] == '\n') continue;
        if (line[0] == '/') process_cmd(line);
        else process_emf(line);

        if (time_display_active) get_uptime(&etime);
        if (time_display_active) fprintf(stderr, "%s->%s: %f line: %s", get_time(cstime, &stime), get_time(cetime, &etime), get_duration(&stime, &etime), line);

        // prompt_wait("get next line");
    }

    printf("\n");

    if (time_display) get_uptime(&stime);
    fclose(fd);
    close_elink_mem();
    if (time_display) get_uptime(&etime);
    if (time_display) fprintf(stderr, "%s->%s: %f to close test\n", get_time(cstime, &stime), get_time(cetime, &etime), get_duration(&stime, &etime));
 
    if (time_display) fprintf(stderr, "using time_offset of: %ld last user_time usec: %ld correction: %ld\n", time_offset/1000, last_nsec/1000, correction);
    
    return EXIT_SUCCESS;
}
