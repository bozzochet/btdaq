// file wdsid.c
//
// Program wdsid reads file with Dallas ID and priorities 
// and sends command to write them to USCM.
//
// A.Lebedev Jun-2006...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x161;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char *filename;
FILE *file = NULL;
int16 err;

//~----------------------------------------------------------------------------

bool read_Dallas_ID_file(char *filename, int8 *n, int64 *ID, int8 *pri) {

  int i;
  int swtc = 0;
  
  if (!file) file = fopen(filename, "r");
  if (!file) {
    printf("File %s absent.\n", filename);
    return FALSE;
  }

  for (i=0; i<8; i++)    n[i] = 0;
  for (i=0; i<8*64; i++) ID[i] = 0;

  while (1) {
    char b[8192];
    int k;
    int bu;
    static int bus = 100000;
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';
    if (P) printf(b);
    k = sscanf(b, "Dallas Bus %d", &bu);
    if (k == 1) {
      if (bu > 0 && bu < 9) {
        bus = bu;
        if (P) printf(":: OK, switched to Dallas Bus %d\n", bus);
        swtc = 1;
        continue;
      }
      else {
        if (P) printf(":: bus %d is invalid\n", bu);
        swtc = 0;
        continue;
      }
    }
    switch (swtc) {
      int   i;
      int64 id;
      int  prio;
      case 0:
        if (P) printf("\n");
      break;
      case 1:
        k = sscanf(b, "%d %16llX %d", &i, &id, &prio);
        if (k == 3) {
          if      (i < 0 || i > 63) {
            if (P) printf(":: number %d is invalid\n", i);
          }
          else if (!DOWCRC(id)) {
            if (P) printf(":: ID CRC error\n");
          }
          else if (prio < 0 || prio > 9) {
            if (P) printf(":: priority %d is invalid\n", prio);
          }
          else {
            n[bus-1] = MAX(n[bus-1], i+1);
            ID[64*(bus-1)+i] = id;
            pri[64*(bus-1)+i] = prio;
            if (P) printf(":: OK\n");
          }
        }
        else {
          if (P) printf("\n");
        }
      break;
    }
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int8 n[8];
  int64 ID[8][64];
  int8 pri[8][64];
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage:\n");
  printf("%s <file-name> [<USCM_ID> [<JMDC_ID> [P]]]]\n",argv[0]);

  if (argc < 2) exit(1);
  filename = argv[1];
  if (argc > 2) USCM_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);
  
  printf("USCM_ID = %02X\n", USCM_ID);
  printf("JMDC_ID = %d\n",   JMDC_ID);
  printf("P       = %d\n",   P);

  if (!read_Dallas_ID_file(filename, n, &ID[0][0], &pri[0][0])) exit(1);
  if (!initialize_EPP()) exit(1);

  for (i=0; i<8; i++) {
    if (n[i]) {
      write_DS1820_table(i+1, n[i], &ID[i][0], &pri[i][0], &err);
      if (!err) {
        if (P) printf("Bus %d writing completed OK\n", i+1);
      }
      else {
        if (P) printf("Bus %d writing failed\n", i+1);
        break;
      }
    }
  }

  printf("Writing %s\n", err ? "failed." : "completed OK.");

  return 0;
}

//~============================================================================

#if this_is_a_test_file

Dallas Bus 5
NB ---Dallas ID ---  P Comments
 0 A7000800AA9D6010  1 LDDR #0 Laser 0
 1 1C0008004C26B010  2 LDDR #0 Laser 1
 2 35000800AAACD410  3 LDDR #1 Laser 0
 3 60000800AB040A10  4 LDDR #1 Laser 1
 4 DB0008004C097A10  5 LDDR #2 Laser 0
 5 FE0008004C2C8610  6 LDDR #2 Laser 1
 6 C5000800AA939610  7 LDDR #3 Laser 0
 7 95000800AAB50110  8 LDDR #3 Laser 1
 8 D3000800504E1910  9 LDDR #4 Laser 0
 9 18000800AAB03710 10 LDDR #4 Laser 1
69 18000800AAB03710  9 LDDR #4 Laser 1
10 18000800AAB03711  9 LDDR #4 Laser 1

#endif

//~============================================================================
