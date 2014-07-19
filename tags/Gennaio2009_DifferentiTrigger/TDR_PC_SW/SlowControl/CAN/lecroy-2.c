// file lecroy-2.c
//
// Program to check multiple LeCroy commands 
// (old U-Crate backplane should be used)
//
// A.Lebedev Aug-2006...

#include "uscmlib.h"

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x14E;                               // global variable
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable
int P = 0;                                         // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int8 bus[10];
  int32 rea[10], wri[10];
  bool chk[10];
  int8 clk, dat;
  int16 chip, board;
  int16 err;

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

  for (i=0; i<10; i++) {
    int32 m;
    int p;

    clk   =  2;
    dat   =  8;     // good
//  dat   = 10;     // bad
    board =  0;
    chip  =  0;

    bus[i] = (clk << 4) | dat;

    wri[i] = 0x80000000;
//  pack32(&wri[i], 0x20000000, power);
    pack32(&wri[i], 0x20000000, 1);
//  pack32(&wri[i], 0x10000000, broadcast);
    pack32(&wri[i], 0x10000000, 0);
    pack32(&wri[i], 0x0F800000, board);
    pack32(&wri[i], 0x00700000, chip);
//  pack32(&wri[i], 0x00070000, section);
    pack32(&wri[i], 0x00070000, 0);
  
    for (m=0x80000000,p=1; m; m>>=1) if (wri[i] & m) p ^= 1;
    if (p) wri[i] ^= 0x40000000;
  }

#if 0
  for (n=0; n<12; n++) {
    for (i=0; i<10; i++) chk[i] = 1;
    exec_LeCroy_cmd(n, bus, wri, rea, chk, &err);
    printf("n = %2d, err = 0x%04X, chk = ", n, err);
    for (i=0; i<10; i++) printf("%d ", chk[i]);
    printf("\n");
  }
#endif

#if 1
  while (1) {
    static int c = 0, c_err = 0, c_chk = 0;
    int n = 10, chks = 0;
    exec_LeCroy_cmd(n, bus, wri, rea, chk, &err);
    if (!err) for (i=0; i<n; i++) chks = chks + chk[i];
    c++;
    if (err) {
      c_err = c_err + 1;
      printf("*** Communication Error: c=%d, err=0x%04X\n", c, err);
    }
    if (chks) {
      c_chk = c_chk + 1;
      printf("*** LeCroy Bus Error(s): c=%d, chks=%d\n", c, chks);
    }
    if (!(c % 100)) printf("c=%d, c_err=%d, c_chk=%d\n", c, c_err, c_chk);
  }
#endif
  
  return 0;
}

//~============================================================================
