// file ds2.c
//
// Example program for Dallas project - A.Lebedev Dec-2004...
//
// Reads Dallas ID Table from USCM, 
// prints it and writes into ds2.conf file
// then writes it back to USCM
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
  int64 ID[64];
  int8 prio[64];
  int8 n;
  int8 bus;
  FILE *file = fopen("ds2.conf", "w");;

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

  fprintf(file, "Result of ds2.c program\n");
  fprintf(file, "USCM::\n");
  for (i=0; i<9; i++) {
    bus = i + 1;
    read_DS1820_table(bus, &n, ID, prio, &err);
    if (err) {
      printf("read_DS1820_table: bus = %d, err = %04X\n", 
              bus, err);
      exit(1);
    }
    printf(       "bus=%d n=%d\n", bus, n);
    fprintf(file, "WRITE_DALLAS_ID_BUS_%d:\n", bus);
    fprintf(file, "//NB ---Dallas ID ---  P Comments\n");
    for (j=0; j<n; j++) {
      printf(       "  %2d %016llX %2d\n", j, ID[j], prio[j]);
      fprintf(file, "  %2d %016llX %2d\n", j, ID[j], prio[j]);
    }
    write_DS1820_table(bus,  n, ID, prio, &err);
    if (err) {
      printf("write_DS1820_table: bus = %d, err = %04X\n", 
              bus, err);
      exit(1);
    }
  }

  return 0;
}

//~============================================================================
