// file 0.c
//
// Study EPPCAN box for AMSW operations
//
// A.Lebedev Aug-2006...

#include "jinflib.h"

//~----------------------------------------------------------------------------

//--- AMS-Wire Registers ---

#define TX_adr 0   // watch offset
#define TX_sta 1   // watch offset
#define RX_adr 2   // watch offset
#define RX_sta 3   // watch offset
#define w_adr 16
#define r_adr 17
#define w_dat 18
#define r_dat 19
#define w_CRC 22
#define r_CRC 23

//~============================================================================

int main(int argc, char *argv[]) {

  int16 addr, wdat, rdat;
    
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream

  printf("Usage: %s <addr> <wdat>\n", argv[0]);
  if (argc != 3) exit(1);
  addr = strtoul(argv[1], NULL, 16);
  wdat = strtoul(argv[2], NULL, 16);
  printf("addr = %04X\n", addr);
  printf("wdat = %04X\n", wdat);

  if (!initialize_EPP()) exit(1);

  outb(w_adr, EPP_addr);
  outw(addr,  EPP_data);
  outb(w_dat, EPP_addr);
  outw(wdat,  EPP_data);

  outb(r_adr, EPP_addr);
  outw(addr,  EPP_data);
  outb(r_dat, EPP_addr);
  rdat = inw(EPP_data);

  outb(0x17,  EPP_addr);
  printf("addr=%04X, wdat=%04X, rdat=%04X\n", addr, wdat, rdat);
  
  return 0;
}

//~============================================================================
