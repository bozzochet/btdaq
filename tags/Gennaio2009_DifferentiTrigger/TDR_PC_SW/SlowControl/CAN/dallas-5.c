// file dallas-5.c
//
// Program to read Dallas sensors (prepared for ECAL beam test)
// USCM must have config file loaded (see example at the bottom)
//
// A.Lebedev - Oct-2006...

#include "uscmlib.h"

int P = 0;                                     // global variable
int USCM_ID = 0x15B;                           // global variable
int JMDC_ID = 0x00;                            // global variable

int BUS_FOR_REQUEST = 0;                       // global variable
int BUS_FOR_REPLY = 0;                         // global variable

int ntry = 5;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int8 n[9], n1[9],      n2[9],      n3[9];
bool              ok2[9][64], ok3[9][64];
float              t2[9][64],  t3[9][64];
int64 ID[9][64];

//~----------------------------------------------------------------------------

void find_Dallas_sensors(void) {

  int i, t;
  int16 err;

  for (i=0; i<9; i++) {
    for (t=0; t<ntry; t++) {
      read_DS1820_table(i+1, &n1[i], &ID[i][0], NULL, &err);
      if (!err) break;
    }
    if (err) PANIC("find_Dallas_sensors FAILED\n");
  }
  
  printf("Bus #                  ");
  for (i=0; i<9; i++) printf(" %2d",    9-1-i+1);
  printf("\n");
  printf("Nb of sensors to read: ");
  for (i=0; i<9; i++) printf(" %2d", n1[9-1-i]);
  printf("\n");
}

//~----------------------------------------------------------------------------

void read_Dallas_sensors(void) {

  int i, t;
  int16 err;

  for (i=0; i<9; i++) {
    if (n1[i]) {
      for (t=0; t<ntry; t++) {
        read_DS1820_temp_long(i+1, &n3[i], &ok3[i][0], &t3[i][0], NULL, &err);
        if (!err) break;
      }
      if (err) PANIC("read_Dallas_sensors FAILED\n");
    }
  }
}

//~----------------------------------------------------------------------------

void show_Dallas_data(FILE *file) {

  int i, j, nmax;

  fprintf(file, "0x%08lX %10ld : %s\n", time(NULL), time(NULL), mytime());

  nmax = 0;
  for (j=1; j<9; j++) nmax = MAX(nmax, n3[j]);

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
    for (j=1; j<9; j++) {
      int jj = 8 - j;
      if (n3[jj]) fprintf(file, " -------- Bus %d -------- ", jj+1);
    }
    fprintf(file, "\n");
  }

  for (i=0; i<nmax; i++) {
    fprintf(file, "%2d", i);
    for (j=1; j<9; j++) {
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
          if (ok3[jj][i]) fprintf(file, " %+6.2f ", t3[jj][i]);
          else            fprintf(file, "        ");
        }
        else {
          fprintf(file, "                 ");
          fprintf(file, "        ");
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

  int t;
  int16 err = 0x0000;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);
  if (argc > 4) ntry = atoi(argv[4]);

  printf("USCM ID     = 0x%02X\n", USCM_ID);
  printf("JMDC ID     = %d\n", JMDC_ID);
  printf("P           = %d\n", P);
  printf("ntry        = %d\n", ntry);

  for (t=0; t<ntry; t++) {
    if (!initialize_EPP()) err = 0xFFFF;
    if (!err) break;
  }
  if (err) PANIC("initialize_EPP FAILED\n");

  find_Dallas_sensors();
  read_Dallas_sensors();
  show_Dallas_data(stdout);

  return 0;
}

//~============================================================================

#if 0

// file ecal.conf
//
// Configuration File for USCM to measure ECAL temperatures
//
// A.Lebedev - Oct 2006...
//

Dallas Bus 2
NB ---Dallas ID ---  P Comments
 0 B2000800C3C59A10  9 face A 1
 1 CF000800ABFA5D10  9 face A 2
 2 4E000800C3879310  9 face A 3
 3 08000800CF233C10  9 face A 4
 4 8F000800ABC35C10  9 face A 5
 5 7B000800AB7C1410  9 face A 6

 6 1C000800C39D3910  9 face B 1
 7 C1000800D0702910  9 face B 2
 8 87000800C386B510  9 face B 3
 9 F7000800C37B5B10  9 face B 4
10 39000800D09BF010  9 face B 5
11 E2000800ABE93910  9 face B 6

12 29000800C375D210  9 face C 1
13 E8000800ABD20710  9 face C 2
14 AE000800D19B2410  9 face C 3
15 55000800CBD3D810  9 face C 4
16 F4000800C3AB7E10  9 face C 5
17 DA000800C381DF10  9 face C 6

18 1A000800C3A20910  9 face D 1
19 6B000800C3B79B10  9 face D 2
20 57000800ABC98410  9 face D 3
21 AF000800AB9A3B10  9 face D 4
22 58000800ABDD9510  9 face D 5
23 80000800C3E36510  9 face D 6

Dallas Bus 3
NB ---Dallas ID ---  P Comments
 0 F7000800ABDAAB10  9 face A 1
 1 11000800ABDB5810  9 face A 2
 2 95000800C37D6510  9 face A 3
 3 6A000800CFADE610  9 face A 4
 4 9A000800ABCED210  9 face A 5
 5 39000800AB558F10  9 face A 6

 6 F4000800C3C84810  9 face B 1
 7 E8000800AB556410  9 face B 2
 8 07000800C373D410  9 face B 3
 9 1D000800AC058E10  9 face B 4
10 3A000800D1349E10  9 face B 5
11 48000800ABAFAB10  9 face B 6

12 05000800C36FE210  9 face C 1
13 24000800ABB39F10  9 face C 2
14 CD000800D1775410  9 face C 3
15 3E000800D1670C10  9 face C 4
16 74000800C37D3F10  9 face C 5
17 84000800C3778210  9 face C 6

18 AE000800ABE1C910  9 face D 1
19 82000800ABCD2D10  9 face D 2
20 A6000800AB9F8F10  9 face D 3
21 A9000800C381AA10  9 face D 4
22 C2000800C396CD10  9 face D 5
23 2F000800C3BF2710  9 face D 6

Dallas Setup 8 7 6 5 4 3 2 1
Enable       0 0 0 0 0 1 1 0
Reset        0 0 0 0 0 1 1 0
Manual       0 0 0 0 0 0 0 0
Mode         N N N N N N N N
Parallel     1

#endif

//~============================================================================
