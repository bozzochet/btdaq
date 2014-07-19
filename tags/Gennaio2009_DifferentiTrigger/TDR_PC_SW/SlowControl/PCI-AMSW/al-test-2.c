// file al-test-2.c
//
// Continuation of the first try to use PCI-AMSWire card
//
// A.Lebedev Nov-2006...
//

#include "pciamswlib.h"
#include "amsw_dev.h"
#include "amsw_lib.h"

int P;

//~============================================================================

int main(int argc, char* argv[]) {

  int card = 0, chan = 0, size = 3;
  int i;
  int status;
  int16 req[] = {0x2E0D, 0x0102, 0x0304, 0x0506};  // ping
  _amsw_cha *cha = malloc(sizeof(_amsw_cha));
  _amsw_msg *snd = malloc(sizeof(_amsw_msg));
  _amsw_msg *rcv = malloc(sizeof(_amsw_msg));
  
  req[1] = time(NULL);
  
  setbuf(stdout, NULL);
  printf("Usage: %s <card> <chan> [<AMS-Wire Data Block>]\n", argv[0]);
  if (argc > 1) card = atoi(argv[1]);
  if (argc > 2) chan = atoi(argv[2]);
  if (argc > 3) size = atoi(argv[3]);
  size = MIN(3, size);
  printf("card = %d\n", card);
  printf("chan = %d\n", chan);
  printf("size = %d\n", size);

  P = 4;

  for (i=0; i<2; i++) {
    cha->card  = card;
    cha->chan  = chan;
    status = initialize_PCIAMSW(cha);
    printf("initialize_PCIAMSW(cha) = %d\n", status);

    snd->cha   = cha;
    snd->first = TRUE;
    snd->last  = TRUE;
    snd->len   = size + 1;
    memcpy((int8*)snd->dat, req, 2*(size+1));

    rcv->cha = cha;

    printf("Sending request...\n");
    print_amsw_msg(0, "JMDC TX:", NULL, snd);
    start_amsw_TX(snd);
    while (!amsw_TX_done(snd));
    printf("err = 0x%04X, ret = %d\n", snd->err, snd->ret);

    if (snd->err) printf("%c", 7);

    printf("Waiting for reply...\n");
    while (!amsw_RX_done(rcv));
    print_amsw_msg(0, "JMDC RX:", NULL, rcv);
  }
  
  return 0;
}

//~============================================================================
