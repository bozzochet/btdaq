// file dallas-3.c
//
// Program to find which chain is connected to which bus
// (prepared for M-Crate tests)
//
// A.Lebedev - Feb-2006...

#include "uscmlib.h"
#include "trddef.h"
//#include "q.h"

int P = 0;                                     // global variable
int USCM_ID = 0x1A1;                           // global variable
int JMDC_ID = 0x00;                            // global variable

int BUS_FOR_REQUEST = 0;                       // global variable
int BUS_FOR_REPLY = 0;                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int8  n[9], n1[9];
int64 ID[9][64];

//~----------------------------------------------------------------------------

void reboot_USCM_from_flash(void) {

  int i;
  int16 err;

  printf("Reboot started...\n");
  boot_from_FLASH(&err);
  for (i=0; i<3; i++) {
    if (err) {
      ping_USCM(0, NULL, NULL, NULL, &err);
    }
    else {
      printf("Reboot completed.\n");
      return;
    }
  }
  PANIC("reboot_USCM_from_flash FAILED\n");
}

//~----------------------------------------------------------------------------

void find_Dallas_sensors(void) {

  int i;
  int8 bus = 0xFF;
  int16 err;

  start_DS1820_scan(0xFF, &err);
  if (err) PANIC("find_Dallas_sensor_ID FAILED (1)\n");

  printf("Scan started...\n");

  timer(1, START);
  while (bus) {
    sleep(1);
    check_DS1820_scan(&bus, n, &err);
    if (err) PANIC("find_Dallas_sensor_ID FAILED (2)\n");
    if (timer(1, EXPIRED, 15.0)) PANIC("find_Dallas_sensor_ID FAILED (3)\n");
  }

  for (i=0; i<9; i++) {
    if (n[i]) read_DS1820_table(i+1, &n1[i], &ID[i][0], NULL, &err);
    if (err) PANIC("find_Dallas_sensor_ID FAILED (4)\n");
    if (n1[i] != n[i]) PANIC("find_Dallas_sensor_ID FAILED (5)\n");
  }
  
  printf("Scan completed:\n");
  printf("Bus #            ");
  for (i=0; i<9; i++) printf(" %2d",    9-1-i+1);
  printf("\n");
  printf("Sensors found:   ");
  for (i=0; i<9; i++) printf(" %2d",  n[9-1-i]);
  printf("\n");
  printf("Sensors to read: ");
  for (i=0; i<9; i++) printf(" %2d", n1[9-1-i]);
  printf("\n");
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, j, ii, jj;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID     = 0x%02X\n", USCM_ID);
  printf("JMDC ID     = %d\n", JMDC_ID);
  printf("P           = %d\n", P);

  if (!initialize_EPP()) exit(1);

  reboot_USCM_from_flash();
  find_Dallas_sensors();
  
  for (j=0; j<8; j++) {
    printf("Bus %d:: ", j+1);
    for (jj=0; jj<40; jj++) {
      _chain *p = &chain[jj];
      int nn = 0;
      for (i=0; i<n[j]; i++) {
        for (ii=0; ii<p->n; ii++) if (ID[j][i] == p->ID[ii]) nn++;
      }
      if (nn) printf(" %4s:%d/%d ", p->name, nn, p->n);
    }
    printf("\n");
  }
  
  return 0;
}

//~============================================================================
