// file paolo.c
//
// try to read tracker calibration results from the file
//
// A.Lebedev - Jan 2008...
//

#include "nodedef.h"
#include "gsclib.h"

int P;
static AMSBlock Block, *pBlock = &Block;

typedef struct {
  int32 DataTime;
  float AvePed;
  float AveSig;
} _TDR;

typedef struct {
  int file_number;
  _TDR TDR[192];
} _CalibResult;

_CalibResult CalibResult[100];

//~----------------------------------------------------------------------------

void print_all_calibration_data(AMSBlock *pBlock) {

  int16 *p = pBlock->Data.p16;
  int i, j;

  printf("\n");
  printf("What is this?:\n");
  printf("%04X ", *p++);
  printf("\n");

  printf("\n");
  printf("Pedestals:\n");
  for (j=0; j<1024/16; j++) {
    for (i=0; i<16; i++) printf("%04X ", *p++);
    printf("\n");
  }

  printf("\n");
  printf("Status:\n");
  for (j=0; j<1024/16; j++) {
    for (i=0; i<16; i++) printf("%04X ", *p++);
    printf("\n");
  }

  printf("\n");
  printf("Sigma:\n");
  for (j=0; j<1024/16; j++) {
    for (i=0; i<16; i++) {
      printf("%04X ", *p++);
    }
    printf("\n");
  }

  printf("\n");
  printf("Raw Sigma:\n");
  for (j=0; j<1024/16; j++) {
    for (i=0; i<16; i++) printf("%04X ", *p++);
    printf("\n");
  }

  printf("\n");
  printf("What is this?:\n");
  printf("%04X ", *p++);
  printf("%04X ", *p++);
  printf("%04X ", *p++);
  printf("%04X ", *p++);
  printf("%04X ", *p++);
  printf("%04X ", *p++);
  printf("%04X ", *p++);
  printf("%04X ", *p++);
  printf("\n");
}

//~----------------------------------------------------------------------------

void process_calibration_data(AMSBlock *pBlock, _TDR *TDR) {

  int16 *p = pBlock->Data.p16;
  int16 key;
  int i;
  float npeds  = 0.0, peds  = 0.0;
  float nlothr = 0.0, lothr = 0.0;
  float nraw   = 0.0, raw   = 0.0;
  float nhithr = 0.0, hithr = 0.0;
  float ngain  = 0.0, gain  = 0.0;
  int16 vers, klothr, khithr, kraw, evnts, cstatus;
  float sigma = 0.0;

  key = *p++;
  
  if (key & 0x0001) {
    for (i=0; i<1024; i++) {
      float value = (float)*p++ / 8.0;
      if (value < 500.0) {
        peds  = peds  + value;
        npeds = npeds + 1.0;
      }
    }
  }

  if (key & 0x0002) {
    for (i=0; i<1024; i++) {
      int flag = *p++;
    }
  }

  if (key & 0x0004) {
    for (i=0; i<1024; i++) {
      float value = (float)*p++ / 8.0;
      if (value < 20.0) {
        lothr  = lothr  + value;
        nlothr = nlothr + 1.0;
      }
    }
  }

  if (key & 0x0008) {
    for (i=0; i<1024; i++) {
      float value = (float)*p++ / 8.0;
      if (value < 10000000.0) {
        raw  = raw  + value;
        nraw = nraw + 1.0;
      }
    }
  }

  if (key & 0x0010) {
    for (i=0; i<1024; i++) {
      float value = (float)*p++ / 8.0;
      if (value < 10000000.0) {
        hithr  = hithr  + value;
        nhithr = nhithr + 1.0;
      }
    }
  }

  if (key & 0x0020) {
    for (i=0; i<1024; i++) {
      float value = (float)*p++ / 8.0;
      if (value < 10000000.0) {
        gain  = gain  + value;
        ngain = ngain + 1.0;
      }
    }
  }

  vers    = *p++;
  klothr  = *p++;
  khithr  = *p++;
  kraw    = *p++;
  evnts   = *p++;
  cstatus = *p++;;

#if 0
printf("vers    = %04X\n", vers);
printf("klothr  = %d\n", klothr);
printf("khithr  = %d\n", khithr);
printf("kraw    = %d\n", kraw);
printf("evnts   = %d\n", evnts);
printf("cstatus = %04X\n", cstatus);
#endif

  if (nlothr) TDR->AveSig = lothr / (float)nlothr * 8.0 / (float)klothr;
}

//~----------------------------------------------------------------------------

void process_AMS_block_file(FILE *file, int file_number) {

  int32 len;
  int16 dat[1000000];
  int16 *p, *end;
  _CalibResult *R = &CalibResult[0];

  while (read_AMS_block(file, dat, &len)) {
    int k;
    char b[80];
    _TDR *TDR;
    decode_AMS_block(dat, len, TRUE, pBlock);

//~--

    if (pBlock->DataType != 0x14) continue;
//  print_all_calibration_data(pBlock);
    if (pBlock->DataCount != 8210) continue;
    k = pBlock->NodeAddress - 0x11A;
    if (k < 0 || k >= 192) continue;
    while (R->file_number != -1) {
      if (R->file_number == file_number) break;
      R++;
    }
    R->file_number = file_number;
    TDR = &R->TDR[k];
    TDR->DataTime    = pBlock->DataTime;
    process_calibration_data(pBlock, TDR);

#if 0
    printf("%03d ", file_number);
    asctime_r(localtime((time_t*)&pBlock->DataTime), b);
    b[19] = '\0';
    printf("%s : ", b);
    printf("%s ", pBlock->BlockType & 1 ? "W" : "R");
    printf("NA=%03X", pBlock->NodeAddress);
    if (pBlock->NodeAddress < 512) printf("=%s ", NodeName[pBlock->NodeAddress]);
    else                           printf(" ");
    printf("DT=%02X ", pBlock->DataType);
    printf("DC=%d", pBlock->DataCount);
    printf(" AvePed=%.1f AveSig=%.1f", TDR->AvePed, TDR->AveSig);
    printf("\n");
#endif

//~--

#if 0
    if (pBlock->NodeAddress < 0x18 && pBlock->DataType == 0x06) {
      printf(" : ");
      end = dat + (len + 1) / 2;
      p   = pBlock->Data.p16;
      while (1) {
        len = *p++;
        decode_AMS_block(p, len, FALSE, pBlock);
        printf("%s ", pBlock->BlockType & 1 ? "W" : "R");
        printf("NA=%03X", pBlock->NodeAddress);
        if (pBlock->NodeAddress < 512) printf("=%s ", NodeName[pBlock->NodeAddress]);
        else                           printf(" ");
        printf("DT=%02X ", pBlock->DataType);
        printf("DC=%d", pBlock->DataCount);
        printf(" ");
//      print_AMS_data_block("", pBlock, 16);
        p = p + (len + 1) / 2;
        if (p >= end) break;
      }
      printf("\n");
    }
#endif
  }

}

//~============================================================================

int main( int argc, char *argv[]) {

  int i, j, k;
  int  file_number;
  FILE *block_file;
  _CalibResult *R;
    
//~--

  setbuf(stdout, NULL);
  printf("Usage: %s <P2P file number> [P]\n", argv[0]);

  P = 0;
  if (argc < 2) {
    printf("Too few parameters\n");
    exit(1);
  }
  
  k = 1;
  if (argc > k) file_number = atoi(argv[k++]);
  if (argc > k) P           = atoi(argv[k++]);

  memset(&CalibResult[0], -1, sizeof(CalibResult));
  
  while ((block_file = open_file_for_reading("/Data/BLOCKS/HRDL/P2P", file_number))) {
    process_AMS_block_file(block_file, file_number++);
    fclose(block_file);
  }

//~--

  for (j=0; j<8; j++) {
    if ((j==0) || (j==1) || (j==4) || (j==5)) continue;
    printf("\nCrate T%d: ", j);
    for (i=0; i<12; i++) printf(" %02d-A  %02d-B ", i, i);
    printf("\n");
    R = &CalibResult[0];
    while (R->file_number != -1) {
      printf("      %03d ", R->file_number);
      for (i=24*j; i<24*j+24; i++) {
        if (R->TDR[i].AveSig > 0) printf("%5.1f ", R->TDR[i].AveSig);
        else                      printf("      ");
      }
      printf("\n");
      R++;
    }
  }
  
//~--

  return 0;
}
  
//~============================================================================
