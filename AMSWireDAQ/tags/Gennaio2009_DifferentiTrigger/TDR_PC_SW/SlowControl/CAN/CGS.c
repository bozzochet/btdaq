// file CGS.c
//
// Example program for GGS
//
// A.Lebedev Dec-2004...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x130;      // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int8 dat[8];

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int16 err;
  int32 RQ_typ = 0x0C;
  int8  RQ_enable[6]  = {0x00, 0x01, 0x40, 0x00, 0x91, 0x00};
  int8  RQ_HBB[3] = {0x00, 0x00, 0x00};
  int8  RP_HBB[6];
  int32 RQ_cnt;
  int32 RP_siz;
  int32 RP_cnt;

  setbuf(stdout, NULL);

  if (argc < 4) {
    printf(
    "Usage: %s <first byte> <second byte> <ADC channel> [<Debug Printout Level>]]\n",
            argv[0]);
    exit(1);
  }

  if (!initialize_EPP()) exit(1);

// 46 00 01 40 00 91 00      enable
// 46 00 02 07 F0 04 ** **   command

  if (argc > 1) RQ_HBB[0] = strtoul(argv[1], NULL, 16);
  if (argc > 2) RQ_HBB[1] = strtoul(argv[2], NULL, 16);
  if (argc > 3) RQ_HBB[2] = atoi(argv[3]);
  if (argc > 4) P = atoi(argv[4]);

  RQ_cnt = sizeof(RQ_enable);
  if (!P) printf("Sending ENABLE...");
//to_USCM(R_W,   RQ_typ, RQ_cnt,    RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE,   0x06, RQ_cnt, RQ_enable,      0,    NULL,   NULL,    &err);
  if (!P) {
    if (err) printf("Failed!\n");
    else     printf("OK\n");
  }

  RQ_cnt = sizeof(RQ_HBB);
  RP_siz = sizeof(RP_HBB);
  if (!P) printf("Sending HBB Command with 0x%02X 0x%02X %d ...",
                  RQ_HBB[0], RQ_HBB[1], RQ_HBB[2]);
//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_HBB, RP_siz, &RP_cnt, RP_HBB,    &err);
  if (RP_cnt != RP_siz) err |= 0x1000;
  if (!P) {
    if (err) printf("Failed!\n");
    else     printf("OK\n");
  }
  if (err) exit(1);
  if (!P) printf("Values = %02X%02X %02X\n", RP_HBB[3], RP_HBB[4], RP_HBB[5]);

  return 0;
}

//~============================================================================
