// file ds1.c
//
// Example program for Dallas project - A.Lebedev Dec-2004...
//
// Scans the Dallas ID
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
  int8 old_bus = 0x00;
  int8  n[9];

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
    timer(1, START);
    start_DS1820_scan(0xFF, &err);
    if (err) {
      printf("start_DS1820_scan: err = %04X\n", err);
      exit(1);
    }
    while (1) {
      int8 bus;
      check_DS1820_scan(&bus, n, &err);
      if (err) {
        printf("check_DS1820_scan: err = %04X\n", err);
        exit(1);
      }
      if (bus != old_bus) {
        printf("%02X.", bus);
        old_bus = bus;
        if (!bus) {
          break;
        }
      }
      if (timer(1, EXPIRED, 0.5)) {
        printf(".");
        timer(1, START);
      }
    }
    printf("* scan completed: ");
    for (i=0; i<9; i++) printf(" %d", n[9-1-i]);
    printf("\n");
  }
  
  return 0;
}

//~============================================================================
