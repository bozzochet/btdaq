// file l.c
//
// get long status from USCM
//
// A.Lebedev - Dec-2004...

#include "uscmlib.h"

int P = 0;                                         // global variable
int USCM_ID = 0x44;                                // global variable
int JMDC_ID = 0x00;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int8 USCM[727];

  int i, from;
  int16 err;

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

  get_long_status(&USCM[0], &err);

  if (err) {
    printf("get_long_status: err = %04X\n", err);
    exit(1);
  }
  
  printf("Version: ");
  if (USCM[2] >= 0x2A) from = 21;   // Thank you, HBB
  else                 from = 17;   // Thank you, HBB
  for (i=from; i<from+20; i++) printf("%c", USCM[i]);
  printf("\n");

#if later
  printf("Long USCM status:\n");
  for (i=0; i<727; i++) printf("0x%02hhX%c", USCM[i], (i+1)%20 ? ' ' : '\n');
  printf("\n");
  for (i=0; i<727; i++) printf("%c%c", USCM[i] < 0x20 ? '.' : USCM[i], (i+1)%80 ? '\0' : '\n');
  printf("\n");
#endif

  return 0;
}

//~============================================================================
