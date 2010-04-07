// file hrdllib.c
//
// Stolen from Cai, SEU, VK and reworked
//
// A.Lebedev  Nov-2007...
//

#include "hrdllib.h"

int PP;
int current_file;

bool READING_HRDL_BOARD;       // used in write_frames
bool WRITING_FRAME_FILES;      // used in write_frames

bool CREATING_BLOCK_FILES;     // used in deframing
bool WRITING_BLOCK_FILES;      // used in deframing

//~============================================================================
//
//            VK PMCHRDL BOARD HANDLING LIBRARY
//
//~============================================================================

static int   PMCHRDL_FH;
static char  *HRDLDevice = "/dev/pmchrdl";
static int32 *HRDLRegisters;
static int8  *HRDLDpram;
static int32 *HRDLvDMA;
static int32 *HRDLpDMA;

static int32 *Buffer0;
static int32 *Buffer1;
static int32 *HRDLram;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

bool open_pmchrdl(void) {

/*
  if ((PMCHRDL_FH = open(HRDLDevice, O_RDWR)) < 0) return FALSE;
*/

  PMCHRDL_FH = open(HRDLDevice, O_RDWR);
  if (PMCHRDL_FH < 0) {
    printf("*** open: %s\n", strerror(errno));
    return FALSE;
  }

  printf("PMCHRDLOpen handle %d\n", PMCHRDL_FH);

  HRDLDpram     = mmap(0, PMCHRDL_BAR0_SIZE, PROT_READ | PROT_WRITE, 
                       MAP_SHARED, PMCHRDL_FH, 0xba000000);
  HRDLRegisters = mmap(0, PMCHRDL_BAR1_SIZE, PROT_READ | PROT_WRITE, 
                       MAP_SHARED, PMCHRDL_FH, 0xba100000);
  HRDLvDMA      = mmap(0, PMCHRDL_DMA_SIZE, PROT_READ | PROT_WRITE, 
                       MAP_SHARED, PMCHRDL_FH, 0xba200000);

  HRDLpDMA = (int32*)HRDLRegisters[PMCHRDL_DMA_PCIADR];

  printf("DPRAM mmap: %08X Registers mmap: %08X\n",
         (int)HRDLDpram, (int)HRDLRegisters);
  printf("DMA mmap:   %08X DMA paddr:      %08X\n", 
         (int)HRDLvDMA, (int)HRDLpDMA); 

  return TRUE;
}

//~----------------------------------------------------------------------------

bool close_pmchrdl(void) {

  if (PMCHRDL_FH >= 0) {
    munmap(HRDLRegisters, PMCHRDL_BAR1_SIZE);
    munmap(HRDLDpram,     PMCHRDL_BAR0_SIZE);
    munmap(HRDLvDMA,      PMCHRDL_DMA_SIZE);
    close(PMCHRDL_FH);
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

bool init_pmchrdl(void) {

  if (!open_pmchrdl()) {
     printf("No PMCHRDL board detected in system\n");
     close_pmchrdl();
     return FALSE;
  }
  
  Buffer0 = HRDLvDMA;
  Buffer1 = HRDLvDMA + 1024;
  HRDLram = (int32*)HRDLDpram;

  HRDLRegisters[PMCHRDL_TX0_STATUS] = 0x80000000;
//HRDLRegisters[PMCHRDL_TX0_CTRL]   = 0x00000033;
//HRDLRegisters[PMCHRDL_TX0_GAP]    = 0x0001de97;
//HRDLRegisters[PMCHRDL_TX0_START]  = 0x00038000;
//HRDLRegisters[PMCHRDL_TX0_END]    = 0x0003bfff;
//HRDLRegisters[PMCHRDL_TX0_STATUS] = 0x80000000;
  HRDLRegisters[PMCHRDL_TX1_STATUS] = 0x80000000;
//HRDLRegisters[PMCHRDL_TX1_CTRL]   = 0x00000033;
//HRDLRegisters[PMCHRDL_TX1_GAP]    = 0x0001de85;
//HRDLRegisters[PMCHRDL_TX1_START]  = 0x0003c000;
//HRDLRegisters[PMCHRDL_TX1_END]    = 0x0003ffff;
//HRDLRegisters[PMCHRDL_TX1_STATUS] = 0x80000000;
//HRDLRegisters[PMCHRDL_RX_STATUS]  = 0x80000000;
  HRDLRegisters[PMCHRDL_RX_CTRL]    = 0x0023ffff;

//HRDLRegisters[PMCHRDL_DMA_STATUS] = 0x00000000;
//HRDLRegisters[PMCHRDL_DMA_CTRL]   = 0x00000000;
  HRDLRegisters[PMCHRDL_DMA_CTRL]   = 0x00000001;
//HRDLRegisters[PMCHRDL_DMA_PCIADR] = 0x00000000;
//HRDLRegisters[PMCHRDL_DMA_MEMADR] = 0x00000000;

  return TRUE;
}

//~----------------------------------------------------------------------------

void get_frame_from_HRDL(int32 **frame, int32 *rcv_sta, int32 *dma_sta) {

  int32 error, length, addr, new_addr, descriptor;
  static int bn = 0;

  *frame = NULL;
  *rcv_sta = 0x00000000;
  *dma_sta = 0x00000000;
  
  *dma_sta = HRDLRegisters[PMCHRDL_DMA_STATUS];
  if (*dma_sta & (PMCHRDL_DMA_TABRT | PMCHRDL_DMA_MABRT)) return;
  if (*dma_sta & (PMCHRDL_DMA_DONE  | PMCHRDL_DMA_RUN)) {
    if (!(*dma_sta & PMCHRDL_DMA_DONE)) {
      while (1) {
        *dma_sta = HRDLRegisters[PMCHRDL_DMA_STATUS];
        if (*dma_sta & PMCHRDL_DMA_DONE) break;
      }
    }
    if (*dma_sta & (PMCHRDL_DMA_TABRT | PMCHRDL_DMA_MABRT)) return;
    *rcv_sta = HRDLRegisters[PMCHRDL_RX_STATUS];
    if (*rcv_sta & 
       (PMCHRDL_RX_OVRRUN | PMCHRDL_RX_RESYNC |
        PMCHRDL_RX_ILL    | PMCHRDL_RX_TRIP)) return;
    HRDLRegisters[PMCHRDL_RX_STATUS] = PMCHRDL_RX_NEXT;
    *frame = bn ? Buffer1 : Buffer0;
    bn = 1 - bn;
  }
  
  *rcv_sta = HRDLRegisters[PMCHRDL_RX_STATUS];
  if (*rcv_sta & PMCHRDL_RX_EMPTY) return;
  addr       = *rcv_sta & 0x3FFFF;
  descriptor = HRDLram[addr];
  error      = (descriptor & 0x80000000) >> 31;
  length     = (descriptor & 0x7FFC0000) >> 18;
  new_addr   =  descriptor & 0x3FFFF;
  if (error ||
     (new_addr && ((1 + addr + ((length + 3) / 4)) != new_addr))) {
    printf("*********** Descriptor Error *********** ");
    printf("RB_ERROR = %d, addr = %05X, offset = %05X, new_addr = %05X\n", 
            error, addr, (1 + addr + ((length +3 )/ 4)), new_addr);
    HRDLRegisters[PMCHRDL_RX_STATUS] = PMCHRDL_RX_RESET;
    HRDLram[0x00000] = 0;
    return;    
  }

  HRDLRegisters[PMCHRDL_DMA_PCIADR] = (int32)HRDLpDMA + 4096 * bn;
  HRDLRegisters[PMCHRDL_DMA_MEMADR] = addr + 1;
  HRDLRegisters[PMCHRDL_DMA_STATUS] = 0x40000400;

  return;
}

//~============================================================================
