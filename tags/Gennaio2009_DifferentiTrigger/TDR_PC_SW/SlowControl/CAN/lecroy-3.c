// file lecroy-3.c
//
// Program to check LeCroy commands 
// (UG-Crate should be used (FM?))
//
// A.Lebedev Jul-2008...

#include "uscmlib.h"

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x78;                                // global variable
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable
int P = 0;                                         // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int8 clk, dat, bus;
  int r_w;
  int16 brd, cmd, ext, wri, rea;
  int32 wri32, rea32, m;
  bool chk;
  int16 err;
  int p;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);
  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  r_w = READ;
  clk = 0;
  dat = 0;     // good
//dat = 1;     // bad
  brd = 1;
  cmd = 22;
  ext = 0;
  wri = 0x0001;

  bus = (clk << 4) | dat;

  wri32 =        0x80000000;
  pack32(&wri32, 0x00080000, r_w);
  pack32(&wri32, 0x0F800000, swapbits(cmd,  5));
  pack32(&wri32, 0x00700000, swapbits(brd,  3));
  pack32(&wri32, 0x00070000, swapbits(ext,  3));
  pack32(&wri32, 0x0000FFFF, swapbits(wri, 16));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri32 & m) p ^= 1;
  if (p) wri32 ^= 0x40000000;
  
  while (1) {
    static int c = 0, c_err = 0, c_chk = 0;
    exec_LeCroy_cmd(1, &bus, &wri32, &rea32, &chk, &err);
    if (!err) rea = swapbits(rea32, 16);
    c++;
    if (err) {
      c_err = c_err + 1;
      printf("*** Communication Error: c=%d, err=0x%04X\n", c, err);
    }
    if (chk) {
      c_chk = c_chk + 1;
      if (rea32) {
        printf("*** LeCroy Bus Error(s): c=%d, chk=%d, wri=%08X, rea=%08X\n", 
               c, chk, wri32, rea32);
      }
    }
    if (!(c % 100)) {
      printf("c=%d, c_err=%d, c_chk=%d(%.1f%%)\n", c, c_err, c_chk, 100.0*c_chk/c);
    }
  }
  
  return 0;
}

//~============================================================================
