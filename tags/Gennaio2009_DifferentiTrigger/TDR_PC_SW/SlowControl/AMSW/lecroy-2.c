// file lecroy-2.c
//
// To find HV Brick bus and geo
//
// A.Lebedev Dec-2007...

#include "bricklib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable

int8  bus;                  // global variable
int16 geo;                  // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int16 cod;
  bool  chk;
  int16 err;
  
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

  for (bus=0; bus<8; bus++) {
    for (geo=0; geo<32; geo++) {
      brick_channel_read(1, &cod, &chk, &err);
      if (err) {
        printf("bus = %d, geo = %2d, err = 0x%04X\n", bus, geo, err);
      }
      else {
        printf("bus = %d, geo = %2d, chk = %d ", bus, geo, chk);
        if (!chk) printf("*******************************");
        printf("\n");
      }
    }
  }

  return 0;
}

//~============================================================================
