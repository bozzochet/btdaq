// file hrdllib.h
//
// see hrdllib.c
//

#ifndef _HRDLLIB_H
#define _HRDLLIB_H

#include "mylib.h"
#include "gsclib.h"

//~----------------------------------------------------------------------------

#define PMCHRDL_BAR0_SIZE     (1024*1204)
#define PMCHRDL_BAR1_SIZE     (1024*1024)
#define PMCHRDL_DMA_SIZE      (2*4096)

#define PMCHRDL_TX0_STATUS    0
#define PMCHRDL_TX0_CTRL      1
#define PMCHRDL_TX0_GAP       2
#define PMCHRDL_TX0_START     3
#define PMCHRDL_TX0_END       4

#define PMCHRDL_TX1_STATUS    5
#define PMCHRDL_TX1_CTRL      6
#define PMCHRDL_TX1_GAP       7
#define PMCHRDL_TX1_START     8
#define PMCHRDL_TX1_END       9

#define PMCHRDL_RX_STATUS     10
#define PMCHRDL_RX_CTRL       11

#define PMCHRDL_INT_STATUS    12

#define PMCHRDL_DMA_STATUS    13
#define PMCHRDL_DMA_CTRL      14
#define PMCHRDL_DMA_PCIADR    15
#define PMCHRDL_DMA_MEMADR    16


//  Bits..

//  Receive desciptor

#define PMCHRDL_RXD_ERROR     ((int32)(1<<31))

//  Transmit descriptor

//  TX[0|1]_STATUS read

#define PMCHRDL_TX_ABORT      ((int32)(1<<22))
#define PMCHRDL_TX_FULL       ((int32)(1<<21))
#define PMCHRDL_TX_EMPTY      ((int32)(1<<20))
#define PMCHRDL_TX_RUN        ((int32)(1<<19))
#define PMCHRDL_TX_DONE       ((int32)(1<<18))

//  TX[0|1]_STATUS write

#define PMCHRDL_TX_RESET      ((int32)(1<<31))
#define PMCHRDL_TX_WRITE      ((int32)(1<<30))

//  TX[0|1]_CTRL 

#define PMCHRDL_TX_INT_ENA    ((int32)(1<<13))
#define PMCHRDL_TX_LIGHT      ((int32)(1<<12))
#define PMCHRDL_TX_STOP       ((int32)(1<<11))
#define PMCHRDL_TX_TEST       ((int32)(1<<10))

//  RX_STATUS read

#define PMCHRDL_RX_RESYNC     ((int32)(1<<24))
#define PMCHRDL_RX_ILL        ((int32)(1<<23))
#define PMCHRDL_RX_TRIP       ((int32)(1<<22))
#define PMCHRDL_RX_SIGNAL     ((int32)(1<<21))
#define PMCHRDL_RX_EMPTY      ((int32)(1<<20))
#define PMCHRDL_RX_OVRRUN     ((int32)(1<<19))
#define PMCHRDL_RX_DONE       ((int32)(1<<18))

//  RX_STATUS write

#define PMCHRDL_RX_RESET      ((int32)(1<<31))
#define PMCHRDL_RX_NEXT       ((int32)(1<<30))

//  RX_CTRL 

#define PMCHRDL_RX_ENDIAN     ((int32)(1<<21))
#define PMCHRDL_RX_TEST       ((int32)(1<<20))
#define PMCHRDL_RX_INT_ENA    ((int32)(1<<19))
#define PMCHRDL_RX_LPBACK     ((int32)(1<<18))

//  INT_STATUS

#define PMCHRDL_INT_DMA       ((int32)(1<<3))
#define PMCHRDL_INT_RX        ((int32)(1<<2))
#define PMCHRDL_INT_TX0       ((int32)(1<<1))
#define PMCHRDL_INT_TX1       ((int32)(1<<0))

//  DMA_STATUS read

#define PMCHRDL_DMA_RUN       ((int32)(1<<19))
#define PMCHRDL_DMA_DONE      ((int32)(1<<18))
#define PMCHRDL_DMA_TABRT     ((int32)(1<<17))
#define PMCHRDL_DMA_MABRT     ((int32)(1<<16))

//  DMA_STATUS write

#define PMCHRDL_DMA_START     ((int32)(1<<30))

//  DMA_CTRL

#define PMCHRDL_DMA_INT_ENA   ((int32)(1<<1))
#define PMCHRDL_DMA_WR_ILEG   ((int32)(1<<0))

//~----------------------------------------------------------------------------

bool open_pmchrdl(void);
bool close_pmchrdl(void);
bool init_pmchrdl(void);
void get_frame_from_HRDL(int32 **frame, int32 *rcv_sta, int32 *dma_sta);

//~============================================================================

#endif // _HRDLLIB_H
