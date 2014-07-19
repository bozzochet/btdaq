// file sfr.c
//
//  to read SFR registers from 2 USCMs and compare
//
//  A.Lebedev Feb 2006...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x118;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 1;                           // global variable
int BUS_FOR_REPLY = 1;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int USCM_ID_1 = 0x15F, USCM_ID_2 = 0x198;
  int8 dat_1, dat_2;
  int16 err;

  setbuf(stdout, NULL);
  printf("Usage: %s [<USCM_ID_1> [<USCM_ID_2> [<JMDC_ID> [<Debug Printout Level>]]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID_1 = strtoul(argv[1], NULL, 16);
  if (argc > 2) USCM_ID_2 = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID   = atoi(argv[3]);
  if (argc > 4) P         = atoi(argv[4]);
  printf("USCM_ID_1 = %03X\n", USCM_ID_1);
  printf("USCM_ID_2 = %03X\n", USCM_ID_2);
  printf("JMDC_ID   = %d\n", JMDC_ID);
  printf("P         = %d\n", P);

  for (i=0x80; i<=0xFF; i++) {
    USCM_ID = USCM_ID_1;
    read_USCM_memory(1, 0x0300000 + i, &dat_1, &err);
    if (err) exit(1);
    USCM_ID = USCM_ID_2;
    read_USCM_memory(1, 0x0300000 + i, &dat_2, &err);
    if (err) exit(1);
    printf("%02Xh: 0x%03X 0x%02hhX   0x%03X 0x%02hhX %c\n",
           i, USCM_ID_1, dat_1, USCM_ID_2, dat_2, 
           (dat_1 == dat_2) ? '*' : ' ');
  }
 
  return 0;
}

//~----------------------------------------------------------------------------
