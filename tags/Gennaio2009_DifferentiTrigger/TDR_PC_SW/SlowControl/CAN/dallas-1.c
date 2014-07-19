// file dallas-1.c
//
// Example of a program to read Dallas sensors (prepared for M-Crate tests)
//
// A.Lebedev - Dec-2005...

#include "uscmlib.h"

int P = 0;                                     // global variable
int USCM_ID = 0x196;                           // global variable
int JMDC_ID = 0x00;                            // global variable

int BUS_FOR_REQUEST = 0;                       // global variable
int BUS_FOR_REPLY = 0;                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int8 n[9], n1[9],      n2[9],      n3[9];
bool              ok2[9][64], ok3[9][64];
float              t2[9][64],  t3[9][64];
int64 ID[9][64];

//~----------------------------------------------------------------------------

void reboot_USCM_from_flash(void) {

  int i;
  int16 err;

  printf("Reboot started...\n");
  boot_from_FLASH(&err);
  for (i=0; i<3; i++) {
    ping_USCM(0, NULL, NULL, NULL, &err);
    if (!err) {
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

void show_Dallas_data(FILE *file) {

  int i, j, nmax;

  fprintf(file, "0x%08lX %10ld : %s\n", time(NULL), time(NULL), mytime());

  nmax = 0;
  for (j=0; j<9; j++) nmax = MAX(nmax, n3[j]);

#if 0
printf("n3 = ");
for (i=0; i<9; i++) printf("%2d ", n3[i]);
printf("\n");
printf("nn = ");
for (i=0; i<9; i++) printf("%2d ", nn[i]);
printf("\n");
printf("nmax = %2d\n", nmax);
#endif

  if (nmax) {
    fprintf(file, "Nb");
    for (j=0; j<9; j++) {
      int jj = 8 - j;
      if (n3[jj]) fprintf(file, " -------- Bus %d --------", jj+1);
    }
    fprintf(file, "\n");
  }

  for (i=0; i<nmax; i++) {
    fprintf(file, "%2d", i);
    for (j=0; j<9; j++) {
      int jj = 8 - j;
      if (n3[jj]) {
#if 0
        if (i < n1[jj]) fprintf(file, " %016llX", ID[jj][i]);
        else            fprintf(file, "                 ");
        if (ok3[jj][i]) fprintf(file, " %+6.1f", t3[jj][i]);
        else            fprintf(file, "       ");
//      fprintf(file, " %016llX", ID[jj][i]);
//      if (ok3[jj][i]) fprintf(file, " %+5.1f", t3[jj][i]);
//      else            fprintf(file, "      ");
#endif

        if (i < n1[jj]) {
          fprintf(file, " %016llX", ID[jj][i]);
          if (ok3[jj][i]) fprintf(file, " %+6.1f", t3[jj][i]);
          else            fprintf(file, "       ");
        }
        else {
          fprintf(file, "                 ");
          fprintf(file, "       ");
        }
//      fprintf(file, " %016llX", ID[jj][i]);
//      if (ok3[jj][i]) fprintf(file, " %+5.1f", t3[jj][i]);
//      else            fprintf(file, "      ");
      }
    }
    fprintf(file, "\n");
  }
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int period = 5;
  
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
    show_Dallas_data(stdout);
  }

  return 0;
}

//~============================================================================
