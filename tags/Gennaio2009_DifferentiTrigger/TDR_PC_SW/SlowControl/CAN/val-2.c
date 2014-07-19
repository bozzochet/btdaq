// file val-2.c
//
// "Validate" command studies:
//  - read file with USCM flash memory image
//  - calculate HBB and SXu checksums
//
// A.Lebedev Mar-2006...
//

#include "uscmlib.h"

int JMDC_ID = 0;
int USCM_ID = 0x15F;
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int P = 0;                                         // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  FILE *file;
  int i;
  int8 *mem;
  int32 end, CKS;
  int16 CKS_HBB, CKS_SXu;
  int32 offset = 0x20000;
  int32 begin = 0x20000;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s <filename> [<begin>]\n", argv[0]);
  if (argc < 2) exit(1);
  if (argc > 2) begin = strtoul(argv[2], NULL, 16);
  printf("file  = %s\n",     argv[1]);
  printf("begin = 0x%06X\n", begin);
  
  mem = malloc(0x20000);
  
  file = fopen(argv[1], "r");
  if (file) {
    fread(mem, 0x20000, 1, file);
    fclose(file);
    printf("File reading OK\n");
  }
  else {
    printf("File not found!\n");
    exit(1);
  }

  packbytes(&end, &mem[begin+9-offset], 3);
  printf("end     = %06X\n", end);
  
  packbytes(&CKS, &mem[end  +1-offset], 2);
  printf("CKS     = %04X\n", CKS);

  for (CKS_HBB=0, i=begin; i<=end; i++) CKS_HBB += (int16)mem[i-offset];
  for (CKS_SXu=0, i=begin; i<=end; i++) CKS_SXu += (int16)mem[i-offset] << ((i%2) ? 8 : 0);
  printf("CKS_HBB = %04X\n", CKS_HBB);
  printf("CKS_SXu = %04X\n", CKS_SXu);

  return 0;
}

//~============================================================================
