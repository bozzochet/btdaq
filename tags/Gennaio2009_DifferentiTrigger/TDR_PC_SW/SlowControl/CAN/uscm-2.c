// file uscm-2.c
//
//  to test multi-master communications with USCM:
//  - send ECHO command from up to 4 JMDCs to the same USCM.
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
int32 ccc = 0;

int n = 255;
bool Debug = FALSE;

struct timeval now, then;

//~----------------------------------------------------------------------------

void print_errors(void) {

  gettimeofday(&now, NULL);
  
  printf(
  "AA: cc = %d, ec = %d; BA: cc = %d, ec = %d; AB: cc = %d, ec = %d; BB: cc = %d, ec = %d;", 
  cc[0], ec[0], cc[1], ec[1], cc[2], ec[2], cc[3], ec[3]);

  printf("  %.2f commands/sec\n",
         (float)(ccc)/delta_t(&now, &then));

  ccc = 0;
  gettimeofday(&then, NULL);
}

//~----------------------------------------------------------------------------

void dump_CAN_controller(int cha) {
  
  int i, j;
  int8 dat[256];
  
  for (i=0; i<256; i++) dat[i] = r_CAN(cha, i);

  printf("***************** CAN controller %i ****************\n", cha);
    
  printf("    ");
  for (i=0; i<16; i++) printf("%02X ", i);
  printf("\n");

  for (j=0; j<16; j++) {
    printf("%02X: ", 16*j);
    for (i=0; i<16; i++) printf("%02X ", dat[16*j+i]);
    if ((j == 1)) {
      printf(" TX\n");
//    printf("                         =======================");
      printf("                      ^  ");
      for (i=0; i<3*(dat[16*j+6]>>4)-1; i++) printf("=");
    }
    if ((j == 2)) {
      printf(" RX\n");
//    printf("                      ^  =======================");
      printf("                      ^  ");
      for (i=0; i<3*(dat[16*j+6]>>4)-1; i++) printf("=");
    }
    printf("\n");
  }
}

//~----------------------------------------------------------------------------

void execute_USCM_command(int16 *err) {

  int i;
  int nn;
  int8 dat[8192], datdat[8192];
  
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
    if (*err) dump_CAN_controller(0);
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

  printf("Usage: %s [<USCM_ID> [<ECHO length> [<JMDC_ID>]]]\n",argv[0]);

  if (argc > 3) {
    JMDC_ID = strtoul(argv[3], NULL, 16);
  }
  else {
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
  }

  USCM_ID = 0x177;
  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);

  n = 255;
  if (argc > 2) n = atoi(argv[2]);

  printf("hostname = %s, JMDC_ID = %d, USCM_ID = %02X, n = %d\n", 
          hostname, JMDC_ID, USCM_ID, n);
  
  if (!initialize_EPP()) exit(1);
//for (i=0; i<256; i++) w_CAN(0, i, 0x00);
//for (i=0; i<256; i++) w_CAN(1, i, 0x00);
  
//~--

  ccc = 0;
  gettimeofday(&then, NULL);
  
  while (1) {
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case 'D': Debug = TRUE; printf("Fake error:\n"); break;
        case 'P': P = 3 - P; break;
        case 'Q': print_errors(); exit(0);
        case '?': print_errors(); break;
      }
    }
    BUS_FOR_REQUEST = unpack16(j, 0x01);
    BUS_FOR_REPLY   = unpack16(j, 0x02);
    execute_USCM_command(&err);

    cc[j]++;
    ccc++;

    if (err) {
      printf("****** j = %d, err = 0x%04X\n", j, err);
      ec[j]++;
    }

    if (!(cc[0] % 100) && !(cc[1] % 100) && !(cc[2] % 100) && !(cc[3] % 100)) {
      print_errors();
    }
  j = (j + 1) & 0x3;
  }
  
  return 0;
}

//~============================================================================
