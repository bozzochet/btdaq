// file lvds-2.c
//
// Simple program for read/write LVDS registers, uses test setup.
//
// A.Lebedev Aug-2006...

#include "uscmlib.h"

//~----------------------------------------------------------------------------

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x00;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int P = 0;                                         // global variable

//~----------------------------------------------------------------------------

void generate_data(int pattern, int16 *wri) {

  int i, j;
  
  switch (pattern) {
    case 1:
      for (i=0; i<256; i++) wri[i] = rand();
    break;
    case 2:
      for (i=0; i<256; i++) wri[i] = i % 2 ? 0x0000 : 0xFFFF;
    break;
    case 3:
      for (i=0; i<256; i++) wri[i] = i % 2 ? 0x5555 : 0xAAAA;
    break;
    case 4:
      for (i=0; i<16; i++) for (j=0; j<16; j++) wri[16*i+j] = 1 << j;
    break;
    case 5:
      for (i=0; i<16; i++) for (j=0; j<16; j++) wri[16*i+j] = ~(1 << j);
    break;
    default:
    break;
  }
  
  if (P) {
    for (i=0; i<256; i++) printf("%04X%c", wri[i], !((i+1)%16) ? '\n' : ' ');
  }
}

//~----------------------------------------------------------------------------

void execute_one_cycle(int16 *wri) {

  int8  adr;
  int16 rea;
  int16 err;

  for (adr=0; ; adr++) {
    write_LVDS_bus(adr, wri[adr], &err);
    if (err) {
      printf("write_LVDS_bus(0x%02hhX,... failed\n", adr);
      exit(1);
    }
    read_LVDS_bus(adr, &rea, &err);
    if (err) {
      printf("read_LVDS_bus(0x%02hhX,... failed\n", adr);
      exit(1);
    }
    if (rea != wri[adr]) {
      printf("adr = %02hhX, wri = %04X, rea = %04X\n", adr, wri[adr], rea);
    }
    if (adr == 0xFF) break;
  }
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int pattern;
  int16 wri[256];

  setbuf(stdout, NULL);
  setup_command_path();

  printf(
  "Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n", argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  for (pattern=1; pattern<=5; pattern++) {
    generate_data(pattern, wri);
    execute_one_cycle(wri);
    printf("Pattern %d - completed\n", pattern);
  }
  
  return 0;
}

//~============================================================================
