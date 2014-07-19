// file lecroy-1.c
//
// Franco: LeCroy init gives error 23 sometimes.
// Here is a program to study this problem.
//
// A.Lebedev Jun-2006...

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int16 err;
  int32 c = 0;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<Path>|0 [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) TX           = RX = atoi(argv[1]);
  if (argc > 2) AMSW_PATH[0] = strtoul(argv[2], NULL, 16);
  if (argc > 3) P            = atoi(argv[3]);
  printf("TX    = %d\n", TX);
  printf("RX    = %d\n", RX);
  printf("Path  = 0x%04X\n", AMSW_PATH[0]);
  printf("P     = %d\n", P);

  set_AMSW_timeout(1.0);

  while(1) {
    for (i=0; i<8; i++) {
      init_LeCroy_bus(i, &err);
      if (err) {
        printf("*** c = %d, bus = %d, err = 0x%04X%c\n", c, i, err, 7);
      }
    }
    if (!(++c % 1000)) printf("c = %d\n", c);
  }

  return 0;
}

//~============================================================================
