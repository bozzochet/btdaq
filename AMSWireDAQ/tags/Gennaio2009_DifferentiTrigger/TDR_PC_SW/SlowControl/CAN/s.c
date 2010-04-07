// file s.c
//
// get short status from USCM
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

  int8 USCM[8];

  char *USCM_status_bytes[8] = {
    "should be 0",
    "heart beat",
    "newborn_ID",
    "logical_ID",
    "boot cntr",
    "CAN lo lvl err",
    "CAN hi lvl err",
    "USCM_status"};
  
  char *USCM_status_bits[8] = {
    "config data error",
    "ROM version error",
    "stay in ROM",
    "executing via EEROM",
    "config CKS ok.",
    "EEROM program CKS ok.",
    "internal power up",
    "external power up"};

  int i;
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

  get_short_status(&USCM[0], &err);

  if (err) {
    printf("get_short_status: err = %04X\n", err);
    exit(1);
  }

  for (i=0; i<8; i++) {
    printf("%-15s: 0x%02X\n", USCM_status_bytes[i], USCM[i]);
  }
  for (i=0; i<8; i++) {
    if (USCM[7] & 1<<(7-i)) 
      printf("       D%d: %s\n", i, USCM_status_bits[i]);
  }
  
  return 0;
}

//~============================================================================
