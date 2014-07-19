// file e.c
//
// Example program to boot USCM from OTP - A.Lebedev May-2004...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x190;    // 1xx stands for Newborn ID
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int8 dat[8];

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int16 err;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: t [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n");

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  printf("Waiting for reboot...");
  boot_from_OTP(&err);
  for (i=0; i<3; i++) {
    printf(".");
    get_short_status(dat, &err);
    if (!err) {
      printf("Done\n");
      exit(0);
    }
  }
  printf("Failed\n");

  return 0;
}

//~============================================================================
