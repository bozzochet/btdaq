// file al-test-3.c
//
// Workbench for "jinflib" for PCI-AMSWire card
// (a la "ping-cdp.c")
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
  int16 dat[10*8192+1], datdat[10*8192+10];
  int i, c = 0;
    
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream

  printf("Usage: %s <TX&RX> {<Path>|0} <length> [<Debug Printout Level>]\n", argv[0]);
  if (argc < 4) {
    printf("Too few arguments.\n");
    exit(1);
  }
  TX              = RX = atoi(argv[1]);
  AMSW_PATH[0]    = strtoul(argv[2], NULL, 16);
  n               = atoi(argv[3]);
  if (argc > 4) P = atoi(argv[4]);
  printf("TX    = %d\n", TX);
  printf("RX    = %d\n", RX);
  printf("Path  = 0x%04X\n", AMSW_PATH[0]);
  printf("n     = %d\n", n);
  printf("P     = %d\n", P);

  timeout = 1.0;
  
  for (i=0; i<n; i++) dat[i] = i;

  while (1) {
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
//    if (!(++c % 100)) printf("\r             %d", c);
      if (!(++c % 1)) printf("\r             %d", c);
    }
    else {
//    printf("\r*** err = 0x%04X %c\n", err, 7);
      printf("\r*** err = 0x%04X   \n", err);
    }
    break;
  }
  
  return 0;
}

//=============================================================================
