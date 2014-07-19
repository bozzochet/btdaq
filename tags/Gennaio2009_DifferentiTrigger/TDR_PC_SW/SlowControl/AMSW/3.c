// file 3.c
//
// Low-level AMS-Wire tester
//
// A.Lebedev Aug-2005...
//

#include "amswlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 4;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable

//~============================================================================

int main(int argc, char* argv[]) {

  static _AMSW_msg SND, *pSND = &SND;
  static _AMSW_msg RCV, *pRCV = &RCV;
  int i;
  int16 err;

  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s <Port> [<AMS-Wire Data Block>]\n", argv[0]);
  if (argc < 2) {
    printf("Too few arguments.\n");
    exit(1);
  }
  TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }

  printf("TX&RX = %d\n", TX);
  printf("P     = %d\n", P);

  pSND->len        = argc - 2;
  pSND->cha        = TX;
  pSND->first      = TRUE;
  pSND->last       = TRUE;
  for (i=0; i<pSND->len; i++) pSND->dat[i] = strtoul(argv[i+2], NULL, 16);

  pRCV->cha = RX;

  AMSW_SND_RCV(INITIALIZE, pSND, pRCV, &err);
  if (err) {
    printf("Initialization failed.\n");
    exit(1);
  }
  AMSW_SND_RCV(START, pSND, pRCV, &err);
  if (err) {
    printf("Starting failed.\n");
    exit(1);
  }
  while (AMSW_SND_RCV(CONTINUE, pSND, pRCV, &err) != DONE);
  return 0;
}

//~============================================================================
