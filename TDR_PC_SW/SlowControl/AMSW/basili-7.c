// file basili-7.c
//
// To read Actel "version_id" for all S-Crate boards
//
// A.Lebedev May-2007...

#include "daqlib.h"
#include "sdr2def.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int16 adr;
  int16 rea;
  int16 err;

  char *nam[8] = {
    "SDR2",    "SFET2-A", "SFET2-B", "SPT2",  
    "SFET2-C", "SFET2-D", "SFEA2",   "SFEC"};
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s <TX&RX> [<Path> [P]]\n", argv[0]);
  if (argc < 2) {
    printf("Too few arguments.\n");
    exit(1);
  }
  TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }
  if (argc > 2) AMSW_PATH[0] = strtoul(argv[2], NULL, 16);
  if (argc > 3) P   = atoi(argv[3]);
  
  printf("TX&RX = %d\n",   TX);
  printf("Path  = %04X\n", AMSW_PATH[0]);
  printf("P     = %d\n",   P);

  set_AMSW_timeout(0.1);

  for (adr=0; adr<8; adr++) {
    int16 reg = strncmp(nam[adr], "SPT2", 4) ? 0xFFF : 0xFFE;
    exec_SDR2_SC_command(READ, adr, reg, 0, &rea, NULL, &err);
    if (!err) {
      printf("Board Address = %d (%7s), Actel version_id = 0x%04X\n", 
             adr, nam[adr], rea);
    }
    else {
      printf("Board Address = %d (%7s)  *** error 0x%04X\n",
             adr, nam[adr], err);
    }
  }  
  return 0;
}

//~============================================================================
