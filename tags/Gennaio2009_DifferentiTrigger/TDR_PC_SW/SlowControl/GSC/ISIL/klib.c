// file klib.c
//
// V.Koutsenko May 2005

#include "klib.h"

int PMCHRDL_FH;
static char *HRDLDevice = "/dev/pmchrdl";
unsigned long *HRDLRegisters;
unsigned char *HRDLDpram;
unsigned long *HRDLvDMA;
unsigned long *HRDLpDMA;

//==============================================================================

int pmchrdl_open(void) {

  if ( (PMCHRDL_FH = open(HRDLDevice, O_RDWR)) < 0 ) 
    return 0;

  printf("PMCHRDLOpen handle %d\n", PMCHRDL_FH);

  HRDLDpram = mmap(0, PMCHRDL_BAR0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, 
                   PMCHRDL_FH, 0xba000000);
  HRDLRegisters = mmap(0, PMCHRDL_BAR1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                       PMCHRDL_FH, 0xba100000);
  HRDLvDMA = mmap(0, PMCHRDL_DMA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                       PMCHRDL_FH, 0xba200000);
  HRDLpDMA = (unsigned long *) HRDLRegisters[PMCHRDL_DMA_PCIADR];

  printf("DPRAM mmap: %08X Registers mmap: %08X\nDMA mmap:   %08X DMA paddr:      %08X\n", 
         (int)HRDLDpram, (int)HRDLRegisters, (int)HRDLvDMA, (int)HRDLpDMA); 

  return 1;
}


//==============================================================================

int pmchrdl_close(void) {

  munmap(HRDLRegisters, PMCHRDL_BAR1_SIZE);
  munmap(HRDLDpram, PMCHRDL_BAR0_SIZE);
  munmap(HRDLvDMA, PMCHRDL_DMA_SIZE);
  close(PMCHRDL_FH);

  return 1;
}
//==============================================================================
