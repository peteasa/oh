#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <e-hal.h>
#include <e-loader.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <assert.h>
#include "elink_common.h"

// Set to 1 to display verbose results
#define DISPLAY_RESULTS 0

#define TAPS 64

// Forward declarations
int my_reset_system(void);

static ssize_t ee_write_esys(off_t to_addr, int data)
{
       e_mmap_t  esys;
       int               memfd;
       int              *pto;

       // Open memory device
       memfd = open(EPIPHANY_DEV, O_RDWR | O_SYNC);
       if (memfd == -1)
       {
               warnx("ee_write_esys(): EPIPHANY_DEV file open failure.");
               return E_ERR;
       }

       esys.phy_base = (ELINK_BASE + to_addr);
       esys.page_base = ee_rndl_page(esys.phy_base);
       esys.page_offset = esys.phy_base - esys.page_base;
       esys.map_size = sizeof(int) + esys.page_offset;

       esys.mapped_base = mmap(NULL, esys.map_size, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, esys.page_base);
       esys.base = esys.mapped_base + esys.page_offset;

       // diag(H_D2) { fprintf(diag_fd, "ee_write_esys(): esys.phy_base = 0x%08x, esys.page_base = 0x%08x, esys.page_offset = 0x%08x, esys.base = 0x%08x, esys.size = 0x%08x\n", (uint) esys.phy_base, (uint) esys.page_base, (uint) esys.page_offset, (uint) esys.base, (uint) esys.map_size); }

       if (esys.mapped_base == MAP_FAILED)
       {
               warnx("ee_write_esys(): ESYS mmap failure. Have you enabled unsafe access?");
               return E_ERR;
       }

       pto = (int *) (esys.base);
       // diag(H_D2) { fprintf(diag_fd, "ee_write_esys(): writing to to_addr=0x%08x, pto=0x%08x\n", (uint) (platform.regs_base + to_addr), (uint) pto); }
       *pto = data;

       munmap(esys.mapped_base, esys.map_size);
       close(memfd);

       return sizeof(int);
}

static int ee_read_esys(off_t from_addr)
{
       e_mmap_t          esys;
       int                       memfd;
       volatile int *pfrom;
       int                       data;

       // Open memory device
       memfd = open(EPIPHANY_DEV, O_RDWR | O_SYNC);
       if (memfd == -1)
       {
               warnx("ee_read_esys(): EPIPHANY_DEV file open failure.");
               return E_ERR;
       }

       esys.phy_base = (ELINK_BASE + from_addr);
       esys.page_base = ee_rndl_page(esys.phy_base);
       esys.page_offset = esys.phy_base - esys.page_base;
       esys.map_size = sizeof(int) + esys.page_offset;

       esys.mapped_base = mmap(NULL, esys.map_size, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, esys.page_base);
       esys.base = esys.mapped_base + esys.page_offset;

       // diag(H_D2) { fprintf(diag_fd, "ee_read_esys(): esys.phy_base = 0x%08x, esys.base = 0x%08x, esys.size = 0x%08x\n", (uint) esys.phy_base, (uint) esys.base, (uint) esys.map_size); }

       if (esys.mapped_base == MAP_FAILED)
       {
               warnx("ee_read_esys(): ESYS mmap failure.");
               return E_ERR;
       }

       pfrom = (int *) (esys.base);
       // diag(H_D2) { fprintf(diag_fd, "ee_read_esys(): reading from from_addr=0x%08x, pto=0x%08x\n", (uint) from_addr, (uint) pfrom); }
       data  = *pfrom;

       munmap(esys.mapped_base, esys.map_size);
       close(memfd);

       return data;
}

int main(int argc, char *argv[]){
  e_loader_diag_t e_verbose;
  e_platform_t platform;
  e_epiphany_t dev, *pdev;
  e_mem_t      dram, *pdram;
  int          err;
  int status=1;//pass
  char elfFile[4096];
  pdev  = &dev;
  pdram = &dram;
  int a,b;
  int i,j;
  unsigned result[N];
  unsigned data = 0xDEADBEEF;
  unsigned tmp,fail;
  int idelay[TAPS]={0x00000000,0x00000000,//0
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

  //Gets ELF file name from command line
  strcpy(elfFile, "e-task.elf");

  //Initalize Epiphany device
  e_set_host_verbosity(H_D0);
  e_init(NULL);
  my_reset_system();
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, 1, 1); //open core 0,0
  e_alloc(pdram, 0x00000000, 0x00400000);

  //Load program to cores and run
  //Test all values
  for (i=0;i<TAPS;i=i+2){  
    //reset system
    my_reset_system();
    //write/read register 
    a=0x0;
    ee_write_esys(ELINK_RXDELAY0, idelay[i]);
    ee_write_esys(ELINK_RXDELAY1, idelay[i+1]);
    ee_write_esys(ELINK_TXSTATUS, a);
    ee_write_esys(ELINK_TXMONITOR,a);
    ee_write_esys(ELINK_RXSTATUS, a);
    printf ("DELAY=%08x ",idelay[i]);
    usleep(100000); 
    e_write(pdram, 0, 0, 0x0, (void *) &(data), sizeof(int));
    e_read(pdram, 0, 0, 0x0, (void *) &(result), sizeof(int));
    usleep(100000);
    for (j=0;j<N;j++){
      e_write(pdram, 0, 0, 4*j, (void *) &(data), sizeof(int));     
      e_read(pdram, 0, 0, 4*j, (void *) &(tmp), sizeof(int));        
      //printf("input[%d]=%08x\n", j,tmp);
    }
    //load program
    err = e_load_group(elfFile, &dev, 0, 0, 1, 1, E_FALSE);    
    err = ee_write_esys(0xF021C, a);//clear monitor
    err = e_start_group(&dev);        
    usleep(1000000);   
    for (j=0;j<N;j++){
      err = e_read(pdram, 0, 0, 4*j, (void *) &tmp, sizeof(int));      
      if (0 == j%100 && DISPLAY_RESULTS)
            printf("result[%d]=%08x\n", j,tmp);
    }
    //check result
    usleep(100000);   
    unsigned int status;
    unsigned int failures=0xDEADBEEF;
    unsigned int write_failures=0;
    err = e_read(pdram, 0, 0, 0, (void *) &status, sizeof(int));     
    err = e_read(pdram, 0, 0, 4, (void *) &failures, sizeof(int));     
    int txstatus  = ee_read_esys(ELINK_TXSTATUS);
    int txmonitor = ee_read_esys(ELINK_TXMONITOR);
    int packet    = ee_read_esys(ELINK_TXPACKET);
    int rxstatus  = ee_read_esys(ELINK_RXSTATUS);
    printf("TXMON=%d TXSTAT=0x%08x RXSTAT=0x%08x PACKET=0x%08x", txmonitor, txstatus, rxstatus, packet);
    if((status==0x12345678) & (write_failures==0)){
      printf(" PASS\n");
    }
      else{
	printf (" FAIL (%d)\n",failures);
      }
  }
  
  //Close down Epiphany device
  e_close(&dev);
  e_finalize();
  
  //self check
  if(status){
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }   
}
//////////////////////////////////////////////////////////////////////////////////////////////

int my_reset_system(void)
{
	int rc = 0;
	uint32_t divider;
	uint32_t chipid;
	elink_txcfg_t txcfg         = { .reg = 0 };
	elink_rxcfg_t rxcfg         = { .reg = 0 };
	elink_dmacfg_t rx_dmacfg    = { .reg = 0 };
	// elink_clkcfg_t clkcfg       = { .reg = 0 };
	elink_reset_t resetcfg      = { .reg = 0 };
	e_epiphany_t dev;

#if 1
	resetcfg.tx_soft_reset = 1;
	resetcfg.rx_soft_reset = 1;
	if (sizeof(int) != ee_write_esys(ELINK_RESET, resetcfg.reg)) {
		printf ("my_reset_system(): ELINK_RESET failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);

	/* Do we need this ? */
	resetcfg.tx_soft_reset = 0;
	resetcfg.rx_soft_reset = 0;
	if (sizeof(int) != ee_write_esys(ELINK_RESET, resetcfg.reg)) {
		printf ("my_reset_system(): clear ELINK_RESET failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif

#if 0 // ???
	chipid = 0x808 /* >> 2 */;
	if (sizeof(int) != ee_write_esys(E_REG_COREID, chipid /* << 2 */)) {
		printf ("my_reset_system(): E_REG_COREID failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif

#if 1
	txcfg.enable = 1;
	txcfg.mmu_enable = 0;
	if (sizeof(int) != ee_write_esys(ELINK_TXCFG, txcfg.reg)) {
		printf ("my_reset_system(): ELINK_TXCFG failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif

	rxcfg.test_mode = 0; /* bug/(feature?) workaround */
	rxcfg.mmu_enable = 0;
	rxcfg.remap_mode = 1; //"static" remap_addr
	rxcfg.remap_sel = 0xfe0; // should be 0xfe0 ???
	rxcfg.remap_pattern = 0x3e0;
	if (sizeof(int) != ee_write_esys(ELINK_RXCFG, rxcfg.reg)) {
		printf ("my_reset_system(): ELINK_RXCFG failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);

#if 0 // ?
	rx_dmacfg.enable = 1;
	if (sizeof(int) != ee_write_esys(ELINK_RXDMACFG, rx_dmacfg.reg)) {
		printf ("my_reset_system(): ELINK_RXDMACFG failed"); fflush(stdout);
		goto err;
	}
	usleep(1000);
#endif
	rc = E_ERR;

	if ( E_OK != e_open(&dev, 2, 3, 1, 1) ) {
		warnx("e_reset_system(): e_open() failure.");
		goto err;
	}

	txcfg.ctrlmode = 0x5; /* Force east */
	//txcfg.ctrlmode_select = 0x1; /* */
	usleep(1000);
	if (sizeof(int) != ee_write_esys(ELINK_TXCFG, txcfg.reg)) {
		printf ("my_reset_system(): ELINK_TXCFG force east failed"); fflush(stdout);
		goto cleanup_platform;
	}

	//divider = 2; /* Divide by 8, see data sheet */
	//divider = 1; /* Divide by 4, see data sheet */
	divider = 0; /* Divide by 2, see data sheet */
	usleep(1000);
	if (sizeof(int) != e_write(&dev, 0, 0, E_REG_LINKCFG, &divider, sizeof(int))) {
		printf ("my_reset_system(): E_REG_LINKCFG failed"); fflush(stdout);
		goto cleanup_platform;
	}

	txcfg.ctrlmode = 0x0;
	//txcfg.ctrlmode_select = 0x0; /* */
	usleep(1000);
	if (sizeof(int) != ee_write_esys(ELINK_TXCFG, txcfg.reg)) {
		printf ("my_reset_system(): E_REG_TXCFG failed"); fflush(stdout);
		goto cleanup_platform;
	}

	rc = E_OK;

cleanup_platform:
	e_close(&dev);

	usleep(1000);
	return E_OK;

err:
	warnx("e_reset_system(): Failed\n");
	usleep(1000);
	return E_ERR;
}
