// file al-test-4.c
//
// Silent program to send one ping. Intended to be run in a loop.sh script
//
// A.Lebedev Feb-2007...
//

#include "pciamswlib.h"

int TX = 0;                 // global variable
int RX = 0;                 // global variable
int16 JINF_PATH = 0x0000;   // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable

//~============================================================================

void ping_JINF(int n, int16 *dat, int *nn, int16 *datdat, int16 *err) {

  int32 RQ_typ = 0x2E0D;
  int16 RQ_cnt = n;
  int16 RP_cnt;

//to_amsw(RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_amsw(RQ_typ, RQ_cnt,    dat, 0xFFFF, &RP_cnt, datdat,     err);

  if (*err) return;
  
  if (nn) *nn = MAX(0, RP_cnt-1);
}

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int n, nn;
  int16 dat[10*8192], datdat[10*8192+10];
  int i;
    
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  n = 100;
  timeout = 1.0;
  for (i=0; i<n; i++) dat[i] = i;

  ping_JINF(n, dat, &nn, datdat, &err);
  if (!err) {
    if (n != nn) {
      printf("sent %d words, received %d words.\n", n, nn);
    }
    else {
      for (i=0; i<n; i++) {
        if (dat[i] != datdat[i]) {
          printf("%d-th word sent 0x%04X, received 0x%04X.\n", i, dat[i], datdat[i]);
          break;
        }
      }
    }
  }
  
  return 0;
}

//=============================================================================
