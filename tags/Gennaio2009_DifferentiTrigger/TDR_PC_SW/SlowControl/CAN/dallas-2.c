// file dallas-2.c
//
// Program to monitor the stability of Dallas sensor readout
// (prepared for M-Crate tests)
//
// A.Lebedev - Feb-2006...

#include "uscmlib.h"

int P = 0;                                     // global variable
int USCM_ID = 0x15f;                           // global variable
int JMDC_ID = 0x00;                            // global variable

int BUS_FOR_REQUEST = 0;                       // global variable
int BUS_FOR_REPLY = 0;                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int8 n[9], n1[9], n2[9],      n3[9];
bool              ok2[9][64], ok3[9][64];
float             t2[9][64],  t3[9][64];
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
  int8 bus_old;
  int16 err;

  start_DS1820_scan(0xFF, &err);
  if (err) PANIC("find_Dallas_sensor_ID FAILED (1)\n");

  printf("Scan started...\n");

  bus_old = 0xFF;
  while (bus) {
    sleep(3);
    check_DS1820_scan(&bus, n, &err);
    if (err) PANIC("find_Dallas_sensor_ID FAILED (2)\n");
    if (bus == bus_old) PANIC("find_Dallas_sensor_ID FAILED (3)\n");
    bus_old = bus;
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

void start_Dallas_sensors(void) {

  int i;
  int8 ena, rst, man, mdw, par;
  int16 err;

  ena = 0x00;
  for (i=0; i<8; i++) if (n1[i]) ena |= 1 << i;
  rst = 0xFF;
  man = 0x00;
  mdw = 0x00;
  par = 1;
  
  setup_DS1820_readout(ena, rst, man, mdw, par, &err);
  if (err) PANIC("start_Dallas_sensors FAILED\n");
  printf("Readout started...\n");
}

//~----------------------------------------------------------------------------

void read_Dallas_sensors(void) {

  int i;
  int16 err;

  for (i=0; i<9; i++) {
    if (n1[i]) {
      read_DS1820_temp_long(i+1, &n3[i], &ok3[i][0], &t3[i][0], NULL, &err);
      if (err) PANIC("read_Dallas_sensors FAILED\n");
    }
  }
}

//~----------------------------------------------------------------------------

void check_Dallas_readout_stability() {

  int i, j;
  int n4[9];
  bool ok = TRUE;
  
  printf("%s:   ", mytime());

  for (j=0; j<9; j++) {
    int jj = 8 - j;
    n4[jj] = 0;
    for (i=0; i<64; i++) {
      if (i < n1[jj] && ok3[jj][i]) n4[jj] = n4[jj] + 1;
    }
    if (n1[jj] != n4[jj]) {
      ok = FALSE;
      printf(" Bus %d: %2d sensor(s) missing.  ", jj+1, n1[jj]-n4[jj]);
    }
  }

  if (ok) printf("O.K.\n");
  else    printf("\n");
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int period = 30;
  
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
  start_Dallas_sensors();
  
  while (1) {
    sleep(period);
    read_Dallas_sensors();
    check_Dallas_readout_stability();
  }

  return 0;
}

//~============================================================================
