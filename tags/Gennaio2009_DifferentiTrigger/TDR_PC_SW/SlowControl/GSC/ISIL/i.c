// file y.c
// To set PMCHRDL registers to known state
//

#include "klib.h"

#define TRUE  1
#define FALSE 0

typedef unsigned int       int32;
typedef unsigned char      int8;
typedef unsigned char      bool;

extern int32 *HRDLpDMA;
extern int32 *HRDLvDMA;
extern int8  *HRDLDpram;
extern volatile int32 *HRDLRegisters;
int32 *Buffer0;
int32 *Buffer1;
int32 *HRDLram;

bool init_pmchrdl(void) {

  if ( ! pmchrdl_open()) {
     printf("No PMCHRDL board detected in system\n");
     pmchrdl_close();
     return FALSE;
  }
  Buffer0 = HRDLvDMA;
  Buffer1 = HRDLvDMA + 1024;
  HRDLram = (int32 *)HRDLDpram;

  return TRUE;
}

//----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  if (!init_pmchrdl()) {
    printf("failed to init pmchrdl.\n");
    exit(1);
  }
  printf("\n");
  printf("TX0_STATUS %08lX\n",HRDLRegisters[PMCHRDL_TX0_STATUS]);
  printf("TX0_CTRL   %08lX\n",HRDLRegisters[PMCHRDL_TX0_CTRL]);
  printf("TX0_GAP    %08lX\n",HRDLRegisters[PMCHRDL_TX0_GAP]);
  printf("TX0_START  %08lX\n",HRDLRegisters[PMCHRDL_TX0_START]);
  printf("TX0_END    %08lX\n\n",HRDLRegisters[PMCHRDL_TX0_END]);

  printf("TX1_STATUS %08lX\n",HRDLRegisters[PMCHRDL_TX1_STATUS]);
  printf("TX1_CTR    %08lX\n",HRDLRegisters[PMCHRDL_TX1_CTRL]);
  printf("TX1_GAP    %08lX\n",HRDLRegisters[PMCHRDL_TX1_GAP]);
  printf("TX1_START  %08lX\n",HRDLRegisters[PMCHRDL_TX1_START]);
  printf("TX1_END    %08lX\n\n",HRDLRegisters[PMCHRDL_TX1_END]);

  printf("RX_STATUS  %08lX\n",HRDLRegisters[PMCHRDL_RX_STATUS]);
  printf("RX_CTRL    %08lX\n\n",HRDLRegisters[PMCHRDL_RX_CTRL]);

  printf("INT_STATUS %08lX\n",HRDLRegisters[PMCHRDL_INT_STATUS]);

  printf("DMA_STATUS %08lX\n",HRDLRegisters[PMCHRDL_DMA_STATUS]);
  printf("DMA_CTRL   %08lX\n",HRDLRegisters[PMCHRDL_DMA_CTRL]);
  printf("DMA_PCIADR %08lX\n",HRDLRegisters[PMCHRDL_DMA_PCIADR]);
  printf("DMA_MEMADR %08lX\n",HRDLRegisters[PMCHRDL_DMA_MEMADR]);

  pmchrdl_close();
}
