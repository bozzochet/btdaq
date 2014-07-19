// file ds6.c
//
// Example program for Dallas project - A.Lebedev Aug-2006...
//
// Parallelizma studies...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x190;                               // 1xx stands for Newborn ID
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, j;
  int16 err;
  int64 ID[9][64];
  int8  prio[9][64];
  float temp[9][64];
  bool ok[9][64];
  int mea[9];
  int8 ena, rst, man, mdw, par;
  int8 n1[9], n2[9];
  int8 bus;

  par = 1;
  man = 0x00;
  mdw = 0x00;

  setbuf(stdout, NULL);
  setup_command_path();

  printf(
  "Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level> [<Paralellism>] [<Manual>] [<Mode>]]]]\n",
  argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P       = atoi(argv[3]);
  if (argc > 4) par     = atoi(argv[4]);
  if (argc > 5) man     = strtoul(argv[5], NULL, 16);
  if (argc > 6) mdw     = strtoul(argv[6], NULL, 16);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);
  printf("par     = %d\n", par);
  printf("man     = 0x%02hhX\n", man);
  printf("mdw     = 0x%02hhX\n", mdw);

  if (!initialize_EPP()) exit(1);

//setup_DS1820_readout( ena,  rst,  man,  mdw, par, &err);
  setup_DS1820_readout(0x00, 0x00, 0x00, 0x00,   0, &err);
  if (err) {
    printf("setup_DS1820_readout: err = %04X\n", err);
    exit(1);
  }

  for (i=0; i<9; i++) {
    bus = i + 1;
    read_DS1820_table(bus, &n1[i], &ID[i][0], &prio[i][0], &err);
    if (err) {
      printf("read_DS1820_table: bus = %d, err = %04X\n", bus, err);
      exit(1);
    }
  }

  ena = 0xFF;
  rst = 0xFF;
  setup_DS1820_readout(ena, rst, man, mdw, par, &err);
  if (err) {
    printf("setup_DS1820_readout: err = %04X\n", err);
    exit(1);
  }

  printf("setup_DS1820_readout( ena,  rst,  man,  mdw, par, ...\n");
  printf("setup_DS1820_readout(0x%02X, 0x%02X, 0x%02X, 0x%02X, %3d, ...\n\n",
                                  ena,    rst,    man,    mdw, par);

  while (1) {
    static int mea_old[10];
    for (i=0; i<9; i++) {
      bus = i + 1;
      read_DS1820_temp_short(bus, &n2[i], &ok[i][0], &temp[i][0], &err);
      if (err) {
        printf("read_DS1820_temp_short: bus = %d, err = %04X\n", 
                bus, err);
        exit(1);
      }
      if (n1[i] != n2[i]) {
        printf("bus=%d n1=%d n2=%d\n", bus, n1[i], n2[i]);
        exit(1);
      }
      mea[i] = 0;
      for (j=0; j<MIN(n1[i],n2[i]); j++) {
        if (ok[i][j]) mea[i] = mea[i] + 1;
      }
      if (mea[i] != mea_old[i]) {
        printf("bus %d: %2d (%d)\n", i+1, mea[i], mea[i]-mea_old[i]);
        mea_old[i] = mea[i];
      }
    }
    if (mea[0]==n1[0] && mea[1]==n1[1] && mea[2]==n1[2] && mea[3]==n1[3] && 
        mea[4]==n1[4] && mea[5]==n1[5] && mea[6]==n1[6] && mea[7]==n1[7]) {
      printf("All Done.\n");
      exit(0);
    }
  }

  return 0;
}

//~============================================================================
