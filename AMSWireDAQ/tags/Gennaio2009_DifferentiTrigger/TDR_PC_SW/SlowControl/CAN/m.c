// file m.c
//
// Example program to test some memory
//
// A.Lebedev Jun-2005...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x190;      // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int n;
  int32 adr;
  int8 rea[8192];
  int8 wri[8192];
  int16 err;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);
  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  n   = 512;
  adr = 0x00038000;
  
  while (1) {
    write_USCM_memory(n, adr, wri, &err);
    if (err) {
      printf("write_USCM_memory... err = 0x%04X\n", err);
      exit(1);
    }
    else {
      read_USCM_memory(n, adr, rea, &err);
      if (err) {
        printf("read_USCM_memory... err = 0x%04X\n", err);
        exit(1);
      }
      else {
      }
    }
  }

  return 0;
}

//~============================================================================
