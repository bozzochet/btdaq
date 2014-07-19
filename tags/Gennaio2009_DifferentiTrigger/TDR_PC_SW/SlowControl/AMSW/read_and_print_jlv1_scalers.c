// file read_and_print_jlv1_scalers.c
//
// Test program to read JLV1 scalers
//
// A.Lebedev Feb-2008...
//

#include "daqlib.h"
#include "jlv1def.h"

//~----------------------------------------------------------------------------

int TX = 1;                 // global variable
int RX = 1;                 // global variable
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

int P = 0;                                         // global variable
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int16 err;

extern struct scalers scaler[];

//~============================================================================

int main(int argc, char *argv[]) {

  int i, n = sizeof(scaler) / sizeof(scaler[0]);
  int16 dat[100];
    
  AMSW_PATH[0] = 0x0E3F;
  AMSW_PATH[1] = 0;

  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<Path> [<Debug Printout Level>]]]\n",
          argv[0]);
  if (argc > 1) TX = RX = atoi(argv[1]);
  if (argc > 2) AMSW_PATH[0] = strtoul(argv[2], NULL, 16);
  if (argc > 3) P  = atoi(argv[3]);
  printf("TX = %d\n",   TX);
  printf("RX = %d\n",   RX);
  printf("Path = %04X\n", AMSW_PATH[0]);
  printf("P  = %d\n",   P);

  read_JLV1_scalers(dat, &err);

  if (err) EXIT("read_JLV1_scalers failed");
  
  for (i=0; i<n; i++) {
    int k = scaler[i].ind - 0x2000;
    if (dat[k]) printf("%2d %-10s: %6d\n", k, scaler[i].txt, dat[k]);
  }
  
  read_JLV1_FPGA_version_ID(dat, &err);

  if (err) EXIT("read_JLV1_FPGA_version_ID failed");
  
  printf("Trigger FPGA Version ID: %04X\n", dat[0]);
  printf("Scaler  FPGA Version ID: %04X\n", dat[1]);
  
  return 0;
}

//~============================================================================
