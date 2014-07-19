// file e.c
//
// Send "Echo" command to JINF
//
// A.Lebedev Apr-2005...

#include "daqlib.h"

//~----------------------------------------------------------------------------
int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int n = 5, nn;
  int16 dat[100000], datdat[100001];
  int16 err;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<n> [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) TX = RX = atoi(argv[1]);
  if (argc > 2) n  = atoi(argv[2]);
  if (argc > 3) P  = atoi(argv[3]);
  printf("TX = %d\n", TX);
  printf("RX = %d\n", RX);
  printf("n  = %d\n", n);
  printf("P  = %d\n", P);

  for (i=0; i<n; i++) dat[i] = i;
  
  ping_JINF(n, dat, &nn, datdat, &err);
  
  if (err) {
    printf("*** Error 0x%04X\n", err);
    exit(1);
  }

  return 0;
}

//~============================================================================
