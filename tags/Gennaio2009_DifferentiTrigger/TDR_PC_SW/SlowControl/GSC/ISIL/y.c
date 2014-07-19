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

  HRDLRegisters[PMCHRDL_TX0_STATUS] = 0x80000000;
  HRDLRegisters[PMCHRDL_TX0_CTRL]   = 0x00000033;
  HRDLRegisters[PMCHRDL_TX0_GAP]    = 0x0001de97;
  HRDLRegisters[PMCHRDL_TX0_START]  = 0x00038000;
  HRDLRegisters[PMCHRDL_TX0_END]    = 0x0003bfff;
  HRDLRegisters[PMCHRDL_TX0_STATUS] = 0x80000000;
  HRDLRegisters[PMCHRDL_TX1_STATUS] = 0x80000000;
  HRDLRegisters[PMCHRDL_TX1_CTRL]   = 0x00000033;
  HRDLRegisters[PMCHRDL_TX1_GAP]    = 0x0001de85;
  HRDLRegisters[PMCHRDL_TX1_START]  = 0x0003c000;
  HRDLRegisters[PMCHRDL_TX1_END]    = 0x0003ffff;
  HRDLRegisters[PMCHRDL_TX1_STATUS] = 0x80000000;
  HRDLRegisters[PMCHRDL_RX_STATUS]  = 0x80000000;
  HRDLRegisters[PMCHRDL_RX_CTRL]    = 0x00037fff;

  //  HRDLRegisters[PMCHRDL_DMA_STATUS] = 0x00000000;
  //  HRDLRegisters[PMCHRDL_DMA_CTRL]   = 0x00000000;
  //  HRDLRegisters[PMCHRDL_DMA_PCIADR] = 0x00000000;
  //  HRDLRegisters[PMCHRDL_DMA_MEMADR] = 0x00000000;
  return TRUE;
}
bool get_frame(int32 **frame) {

  int32 val;
  int32 length;
  int32 addr;
  int32 new_addr;
  int32 descriptor;
  static bool dma_active = FALSE;
  static int bn = 0;


  if (dma_active) {  
    while ((HRDLRegisters[PMCHRDL_DMA_STATUS] & PMCHRDL_DMA_DONE) == 0);
    dma_active = FALSE;
    val = HRDLRegisters[PMCHRDL_RX_STATUS];
    if ((val & PMCHRDL_RX_OVRRUN) != 0) {
      printf("Overrun\n");
    }
    else {
      HRDLRegisters[PMCHRDL_RX_STATUS] = PMCHRDL_RX_NEXT;
    }
  }
was_overrun:
  val = HRDLRegisters[PMCHRDL_RX_STATUS];
  if ((val & PMCHRDL_RX_OVRRUN) != 0) {
    printf("Overrun\n");
    goto was_overrun;
  }
  if ((val & PMCHRDL_RX_EMPTY) != 0) return FALSE;

  addr = val & 0x3ffff;
  descriptor = HRDLram[addr];
  length = (descriptor & 0x7FFC0000) >> 18;
  new_addr = descriptor & 0x3ffff;
  if ((new_addr != 0) && ((1 + addr +((length +3)/4)) != new_addr)) {
    printf("%05lx %05lx %05lx\n",addr,(1 + addr + ((length +3)/4)),new_addr);
    exit(1);
  }

  HRDLRegisters[PMCHRDL_DMA_PCIADR] = (int32)HRDLpDMA + 4096*bn;
  HRDLRegisters[PMCHRDL_DMA_MEMADR] = addr + 1;
  HRDLRegisters[PMCHRDL_DMA_STATUS] = 0x40000400;
  dma_active = TRUE;

  *frame = bn ? Buffer0 : Buffer1;
  bn = (bn + 1) & 1;

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
