// file ds4.c
//
// Example program for Dallas project - A.Lebedev Feb-2005...
//
// Checks Dallas bus status
// and prints it only it is changed.
// Thank you Prof. Becker!
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x190;                               // 1xx stands for Newborn ID
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int16 err;
  int8 ena, par, sta[9], man, mdw, mdr;
  int8 old_sta[9];
  bool first = TRUE;
  char *txt[4] = {"  OK  ", "IO->GND", "IO->VCC", "noVCC?"};

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  while (1) {
    check_DS1820_readout(&ena, &par, sta, &man, &mdw, &mdr, &err);
    if (err) {
      printf("check_DS1820_readout: err = %04X\n", err);
      exit(1);
    }
    if (first || memcmp(old_sta, sta, 9)) {
      if (!first) printf("%c", 7);
      first = FALSE;
      memcpy(old_sta, sta, 9);
      printf("check_DS1820_readout( ena, par,  man,  mdw,  mdr...\n");
      printf("check_DS1820_readout(0x%02X, %3d, 0x%02X, 0x%02X, 0x%02X...\n\n",
                                    ena, par,  man,  mdw, mdr);
      printf("bus:     ");
      for (i=0; i<9; i++) printf("   %d  %c", 9-i, i < 8 ? ' ' : '\n');
      printf("sta =    ");
      for (i=0; i<9; i++) printf("   %d  %c", sta[9-1-i], i < 8 ? ' ' : '\n');
      printf("sta =    ");
      for (i=0; i<9; i++) printf("%s%c", txt[sta[9-1-i]], i < 8 ? ' ' : '\n');
    }
  }
  
  return 0;
}

//~============================================================================
