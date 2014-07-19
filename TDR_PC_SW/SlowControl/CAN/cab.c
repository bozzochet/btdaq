// file cab.c
//
//  to test communications with CAB
//
//  A.Lebedev May 2005...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x118;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 1;                           // global variable
int BUS_FOR_REPLY = 1;                             // global variable

int32 cc[4] = {0, 0, 0, 0};
int32 ec[4] = {0, 0, 0, 0};

//~----------------------------------------------------------------------------

void print_errors(void) {

  printf(
  "AA: cc = %d, ec = %d; BA: cc = %d, ec = %d; AB: cc = %d, ec = %d; BB: cc = %d, ec = %d; \n", 
  cc[0], ec[0], cc[1], ec[1], cc[2], ec[2], cc[3], ec[3]);
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int j = 0;
  int8  Data[10000];
  int32 DataCount = 2;
  int32 DataType  = 0x00000A;
  int16 err;

//~--

  setbuf(stdout, NULL);
  setup_command_path();

  if (!initialize_EPP()) exit(1);

//~--

  while (1) {
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
      case 'P': P = 3 - P; break;
      case '?': print_errors(); break;
      case 'Q': print_errors(); exit(0);
      }
    }
    DataCount = 2;
    DataType  = 0x00000A;
    Data[0] = 0x03;
    Data[1] = 0x03;
    j = (j + 1) & 0x3;
    BUS_FOR_REQUEST = unpack16(j, 0x01);
    BUS_FOR_REPLY   = unpack16(j, 0x02);
//  to_USCM(R_W,    RQ_typ,    RQ_cnt, RQ_dat, RP_siz,    &RP_cnt, RP_dat, &RP_err);
    to_USCM(READ, DataType, DataCount,   Data,  10000, &DataCount,   Data,    &err);

    if (err) {
//    printf("j = %d, err = 0x%04X\n", j, err);
      ec[j]++;
    }

    cc[j]++;
    if (!(cc[0] % 100) && !(cc[1] % 100) && !(cc[2] % 100) && !(cc[3] % 100)) {
      print_errors();
    }
  }
  
  return 0;
}

//~============================================================================
