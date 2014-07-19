// file s9011at.c
//
// To check collisions in S9011AT when working with 2 LeCroy buses.
// Should run on 2 computers sending LeCroy commands to 2 JINFs.
//
// A.Lebedev Mar-2007...
//

#include "amswlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~============================================================================

int main(int argc, char* argv[]) {

  static _AMSW_msg SND, *pSND = &SND;
  static _AMSW_msg RCV, *pRCV = &RCV;
  int32 wri, rea;
  bool chk;
  int i, n = 0, n1 = 0, n2 = 0;
  float timeout;
  int16 err;

  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s <Port> <AMS-Wire Data Block - LeCroy command>\n", argv[0]);
  if (argc < 3) {
    printf("Too few arguments.\n");
    exit(1);
  }
  TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }

  timeout = 1.0;
  P = 0;
  
  printf("Hit P/p to switch ON/OFF debug printout\n");
  printf("TX&RX = %d\n", TX);
  printf("P     = %d\n", P);

  pSND->len        = argc - 2;
  pSND->cha        = TX;
  pSND->first      = TRUE;
  pSND->last       = TRUE;
  for (i=0; i<pSND->len; i++) pSND->dat[i] = strtoul(argv[i+2], NULL, 16);

  pRCV->cha = RX;

  set_AMSW_timeout(timeout);
  
  while (1) {
    n = n + 1;
    if (kbhit()) {
      int ch = fgetc(stdin);
      switch (ch) {
        case 'P': P = 4; printf("P = %d\n", P); break;
        case 'p': P = 0; printf("P = %d\n", P); break;
        case 'Q':
        case 'q': printf("n = %d, n1 = %d, n2 = %d\n", n, n1, n2); exit(0);
        case '?':
        case '/': printf("n = %d, n1 = %d, n2 = %d\n", n, n1, n2);
      }
    }
    AMSW_SND_RCV(INITIALIZE, pSND, pRCV, &err);
    if (err) {
      printf("%d:INITIALIZE failed, err = 0x%04X.\n", n, err);
      continue;
    }
    AMSW_SND_RCV(START, pSND, pRCV, &err);
    if (err) {
      printf("%d:START failed, err = 0x%04X.\n", n, err);
      continue;
    }
    while (AMSW_SND_RCV(CONTINUE, pSND, pRCV, &err) != DONE);
    if (err) {
      printf("%d:CONTINUE failed, err = 0x%04X.\n", n, err);
      continue;
    }
    wri = (int32)pSND->dat[4] << 16 | (int32)pSND->dat[5];
    rea = (int32)pRCV->dat[3] << 16 | (int32)pRCV->dat[4];
    if (rea != wri) {
      printf("%d:Wrong reply: wri = 0x%08X, rea = 0x%08X\n", n, wri, rea);
      n1 = n1 + 1;
    }
    rea = (int32)pRCV->dat[5] << 16 | (int32)pRCV->dat[6];
    chk = ((rea ^ wri) & 0x7FFF0000) ? TRUE : FALSE;
    if (chk) {
      printf("%d:LeCroy failed: wri = 0x%08X, rea = 0x%08X\n", n, wri, rea);
      n2 = n2 + 1;
    }
    if (!(n % 1000)) printf("n = %d, n1 = %d, n2 = %d\n", n, n1, n2);
  }
  return 0;
}

//~============================================================================
