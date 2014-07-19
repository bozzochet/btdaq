// file uscm-3.c
//
//  to test multi-master communications with USCM:
//  - send ECHO command from up to 2 JMDCs to the one USCM, 
//  - send ECHO command from up to 2 other JMDCs to another USCM.
//
//  A.Lebedev Sep 2005...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x123;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int32 cc[4] = {0, 0, 0, 0};
int32 ec[4] = {0, 0, 0, 0};

int n = 255;
bool Debug = FALSE;

//~----------------------------------------------------------------------------

void print_errors(void) {

  printf(
  "AA: cc = %d, ec = %d; BA: cc = %d, ec = %d; AB: cc = %d, ec = %d; BB: cc = %d, ec = %d; \n", 
  cc[0], ec[0], cc[1], ec[1], cc[2], ec[2], cc[3], ec[3]);
}

//~----------------------------------------------------------------------------

void execute_USCM_command(int16 *err) {

  int i;
  int nn;
  int8 dat[8192], datdat[8192];
  
  n = 255;
  for (i=0; i<n; i++) dat[i] = i & 0x00FF;
  
  ping_USCM(n, dat, &nn, datdat, err);

  if (Debug) {
    nn = n - 1;
    datdat[15] = 0x00;
    Debug = FALSE;
  }
  
  if (!*err) {
    int error = 0;
    if (n != nn) {
      printf("****** ECHO error: n = %d, nn = %d\n", n, nn);
      *err = 0x1000;
    }
    for (i=0; i<MIN(n,nn); i++) {
      if (dat[i] != datdat[i]) error++;
    }
    if (error) {
      printf("****** ECHO error: %d difference(s)\n", error);
      printf("  i  SND  RCV\n");
      for (i=0; i<MIN(n,nn); i++) {
        if (dat[i] != datdat[i]) {
          printf("%3d 0x%02X 0x%02X\n", i, dat[i], datdat[i]);
        }
      }
      *err |= 0x2000;
    }
  }
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  char hostname[80];
  int j = 0;
  int16 err;

//~--

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM_ID_1> [<USCM_ID_2> [<ECHO length>]]]\n",argv[0]);

  JMDC_ID = -1;
  gethostname(hostname, sizeof(hostname));
  if (strncmp(hostname, "pcaal03", 7) == 0) JMDC_ID = 0;
  if (strncmp(hostname, "pcaal16", 7) == 0) JMDC_ID = 1;
  if (strncmp(hostname, "pcaal17", 7) == 0) JMDC_ID = 2;
  if (strncmp(hostname, "pcaal21", 7) == 0) JMDC_ID = 3;
  if (JMDC_ID == -1) {
    printf("Unknown hostname.\n");
    exit(1);
  }

  if (JMDC_ID % 2) {
    USCM_ID = 0x14D;
    if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  }
  else {
    USCM_ID = 0x152;
    if (argc > 2) USCM_ID = strtoul(argv[2], NULL, 16);
  }

  n = 255;
  if (argc > 3) n = atoi(argv[3]);

  printf("hostname = %s, JMDC_ID = %d, USCM_ID = %02X, n = %d\n", 
          hostname, JMDC_ID, USCM_ID, n);
  
  if (!initialize_EPP()) exit(1);

//~--

  while (1) {
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case 'D': Debug = TRUE; break;
        case 'P': P = 3 - P; break;
        case 'Q': print_errors(); exit(0);
        case '?': print_errors(); break;
      }
    }
    BUS_FOR_REQUEST = unpack16(j, 0x01);
    BUS_FOR_REPLY   = unpack16(j, 0x02);
    execute_USCM_command(&err);

    cc[j]++;

    if (err) {
      printf("****** j = %d, err = 0x%04X\n", j, err);
      ec[j]++;
    }

    if (!(cc[0] % 100) && !(cc[1] % 100) && !(cc[2] % 100) && !(cc[3] % 100)) {
      print_errors();
    }
//  j = (j + 1) & 0x3;
  }
  
  return 0;
}

//~============================================================================
