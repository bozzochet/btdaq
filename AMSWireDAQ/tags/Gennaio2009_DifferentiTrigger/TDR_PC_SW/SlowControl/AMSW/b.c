// file b.c
//
// Boot JINF
//
// A.Lebedev Aug-2005...
//

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int16 fh = 0x0000;
    
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<File Header> [<Debug Printout Level>]]]\n",
          argv[0]);
  if (argc > 1) TX = RX = atoi(argv[1]);
  if (argc > 2) fh = strtoul(argv[2], NULL, 16);
  if (argc > 3) P  = atoi(argv[3]);
  printf("TX = %d\n",   TX);
  printf("RX = %d\n",   RX);
  printf("fh = %04X\n", fh);
  printf("P  = %d\n",   P);

  boot_JINF(fh, &err);

  printf("boot_JINF: err = 0x%04X\n", err);
  
  return 0;
}

//~============================================================================
