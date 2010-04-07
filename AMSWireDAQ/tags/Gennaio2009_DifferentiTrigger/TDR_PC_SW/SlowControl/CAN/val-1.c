// file val-1.c
//
// "Validate" command studies:
//  - read contence of USCM flash memory
//  - write file with USCM flash memory image
//
// A.Lebedev Mar-2006...
//

#include "uscmlib.h"

int JMDC_ID = 3;
int USCM_ID = 0x15F;
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int P = 0;                                         // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, j, k;

  FILE *file;
  int8 *mem, *pmem;
  
  int8 d[5][4096];
  int16 err;
  int N = 1024;
  int32 begin = 0x20000;
  int ndiff = 0;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [USCM_ID [begin [N]]]\n", argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) begin   = strtoul(argv[2], NULL, 16);
  if (argc > 3) N       = atoi(argv[3]);
  printf("USCM_D = %03X\n",   USCM_ID);
  printf("begin  = 0x%06X\n", begin);
  printf("N      = %d\n",     N);
  
  pmem = mem = malloc(0x20000);

  for (i=0; i<0x40000-begin; i+=N) {
    int32 adr = begin + i;//   int8 n;
    for (j=0; j<5; j++) {
      read_USCM_memory(N, adr, &d[j][0], &err);
      if (err) {
        printf("err = 0x%04X\n", err);
        exit(1);
      }
    }
    for (j=0; j<N; j++) {
      int ii, jj;
      int diff = 0;
      for (ii=0; ii<5; ii++) {
        for (jj=0; jj<5; jj++) {
          if ((ii < jj) && (d[ii][j] != d[jj][j])) diff++;
        }
      }
      if (diff) {
        printf("Diff at adr=%06X(%4d): ", adr+j, j);
        for (k=0; k<5; k++) printf("%02hhX ", d[k][j]);
        printf("\n");
        ndiff++;
      }
      *pmem++ = d[0][j] & d[1][j] & d[2][j];
    }
    printf("adr = %06X\n", adr);
  }

  if (ndiff) printf("Flash reading finished with %d errors.\n", ndiff);
  else       printf("Flash reading finished without errors.\n");


  file = fopen("memory1", "w");
  if (file) {
    fwrite(mem, 0x20000, 1, file);
    fclose(file);
    printf("File writing OK.\n");
  }
  
  return 0;

}

//~============================================================================
