// file ds5.c
//
// Example program for Dallas project - A.Lebedev May-2005...
//
// To study long DS1820 format and priority
// (use DS to scan and enable reading)
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
  float temp1[64], temp2[64];
  int16 age[64];
  bool ok1[64], ok2[64];
  int8 bus, n1, n2;

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

  bus = 8;
  
  read_DS1820_temp_short(bus, &n1, ok1, temp1, &err);
  if (err) {
    printf("read_DS1820_temp_short: bus = %d, err = %04X\n", 
            bus, err);
    exit(1);
  }

  read_DS1820_temp_long(bus, &n2, ok2, temp2, age, &err);
  if (err) {
    printf("read_DS1820_temp_long: bus = %d, err = %04X\n", 
            bus, err);
    exit(1);
  }

  printf("bus %d, n1 = %d, n2 = %d\n", bus, n1, n2);
  
  if (n1 != n2) {
    printf("n1 != n2\n");
    exit(1);
  }
  
//setup_DS1820_readout( ena,  rst,  man,  mdw, par, &err);
  setup_DS1820_readout(0x00, 0xFF, 0x00, 0x00,   1, &err);
  if (err) {
    printf("setup_DS1820_readout: err = %04X\n", err);
    exit(1);
  }

  delay(1.0);
  
//setup_DS1820_readout( ena,  rst,  man,  mdw, par, &err);
  setup_DS1820_readout(0x80, 0xFF, 0x00, 0x00,   1, &err);
  if (err) {
    printf("setup_DS1820_readout: err = %04X\n", err);
    exit(1);
  }

  while (1) {
    static int old_i = -1;
    static bool found;
    read_DS1820_temp_short(bus, &n1, ok1, temp1, &err);
    if (err) {
      printf("read_DS1820_temp_short: bus = %d, err = %04X\n", 
              bus, err);
      exit(1);
    }
    read_DS1820_temp_long(bus, &n2, ok2, temp2, age, &err);
    if (err) {
      printf("read_DS1820_temp_long: bus = %d, err = %04X\n", 
              bus, err);
      exit(1);
    }
    if (n1 != n2) {
      printf("n1 != n2\n");
      exit(1);
    }
#if 0
    if (age[8] == 0) {
      for (i=0; i<n1; i++) {
        int j;
        printf("%2d ", i);
        if (ok1[i]) printf("%+6.1f ", temp1[i]);
        else        printf("       ");
        if (ok2[i]) printf("%+7.2f ", temp2[i]);
        else        printf("        ");
        printf("  0x%08X:", age[i]);
        if (ok2[i]) for (j=0; j<MIN(256,age[i]); j++) printf("X");
        printf("\n");
      }
      printf("%4d ------------------------------\n", c++);
    }
    delay(0.5);
#endif
    found = FALSE;
    for (i=0; i<n1; i++) {
      if (!age[i] && (i != old_i)) {
        found = TRUE;
        if (i < old_i) printf("\n");
        printf(" %d", i);
        old_i = i;
      }
    }
//  if (!found) old_i = -1;
    delay(0.5);

  }

  return 0;
}

//~============================================================================
#if 0

Use this file to study how PRIORITY works in DTS readout

Dallas Bus 8
NB ---Dallas ID ---  P Comments
 0 A7000800AA9D6010  0
 1 A7000800AB08E010  1
 2 0A000800AAC8E010  2
 3 350008005031E010  3
 4 4A000800AADB1010  4
 5 74000800AAB67010  5
 6 DA00080054338810  6
 7 EF0008005030C810  7
 8 E9000800AAD0E810  8
 9 E9000800AAD0E810  9
10 E9000800AAD0E810  9

#endif

//~============================================================================
