// file kill.c
//
// Continuation of the first try to use PCI-AMSWire card
// Try to kill the card by initializing...
//
// A.Lebedev Nov-2006...
//

#include "pciamswlib.h"
#include "amsw_dev.h"
#include "amsw_lib.h"

int P;

//~============================================================================

int main(int argc, char* argv[]) {

  int i;
  int card = 0, chan = 0, size = 4096;
  int status;
  int8 req[] = {0x2E, 0x0D, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};  // ping
  int32 mem;
  _amsw_cha *cha = malloc(sizeof(_amsw_cha));
  _amsw_msg *snd = malloc(sizeof(_amsw_msg));
  _amsw_msg *rcv = malloc(sizeof(_amsw_msg));
  int32 check[40000];
  
  setbuf(stdout, NULL);
  printf("Usage: %s <card> <chan> [<AMS-Wire Data Block>]\n", argv[0]);
  if (argc > 1) card = atoi(argv[1]);
  if (argc > 2) chan = atoi(argv[2]);
  if (argc > 3) size = atoi(argv[3]);
  printf("card = %d\n", card);
  printf("chan = %d\n", chan);
  printf("size = %d\n", size);

  P = 4;

  cha->card  = card;
  cha->chan  = chan;
  status = initialize_PCIAMSW(cha);
  printf("initialize_PCIAMSW(cha) = %d\n", status);

  snd->cha   = cha;
  snd->first = TRUE;
  snd->last  = TRUE;
  snd->len   = sizeof(req);
  memcpy((int8*)snd->dat, req, sizeof(req));

  printf("Sending request...\n");
  start_amsw_TX(snd);
  while (!amsw_TX_done(snd));
  printf("err = 0x%04X, ret = %d\n", snd->err, snd->ret);

  rcv->cha = cha;

  mem = chan * 0x00002000;
  memcpy((int8*)check, (int8*)&snd->cha->amswmem[mem/4], 100);
  printf("TX buffer: ");
  for (i=0; i<4; i++) printf("%08X ", check[i]);
  printf("\n");
  
  printf("CSR_TX: ");
  for (i=0; i<8; i+=2) printf("%08X ", snd->cha->amswreg[i]);
  printf("\n");
/*
  printf("CSR_TX: ");
  for (i=0; i<8; i+=2) printf("%08X ", snd->cha->amswreg[i]);
  printf("\n");
  printf("CSR_TX: ");
  for (i=0; i<8; i+=2) printf("%08X ", snd->cha->amswreg[i]);
  printf("\n");
*/
  printf("Waiting for reply...\n");
  while (!amsw_RX_done(rcv));
  print_amsw_msg(0, "JMDC RX:", NULL, rcv);
  
  return 0;
}

//~============================================================================
