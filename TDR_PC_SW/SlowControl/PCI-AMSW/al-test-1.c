// file al-test-1.c
//
// First try to use PCI-AMSWire card
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
  int8 req[] = {0x2E, 0x0D, 0x01, 0x02, 0x03, 0x04, 0xFF, 0x06};  // ping
  int8 rep[40000];
  int32 data;
  int32 reg, mem;
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

  rcv->cha   = cha;

//status = amsw_tx_packet(chan, (void*)req, sizeof(req), 0x3);
//amsw_tx_packet(&snd);
  start_amsw_TX(snd);
  printf("err = 0x%04X, ret = %d\n", snd->err, snd->ret);

  if (snd->err) printf("%c", 7);
  
  mem = chan * 0x00002000;
  memcpy((int8*)check, (int8*)&snd->cha->amswmem[mem/4], 100);
  printf("TX buffer: ");
  for (i=0; i<4; i++) printf("%08X ", check[i]);
  printf("\n");
  
  printf("CSR_TX: ");
  for (i=0; i<8; i+=2) printf("%08X ", snd->cha->amswreg[i]);
  printf("\n");
  printf("CSR_RX: ");
  for (i=1; i<8; i+=2) printf("%08X ", rcv->cha->amswreg[i]);
  printf("\n");
  printf("CSR_RX: ");
  for (i=1; i<8; i+=2) printf("%08X ", rcv->cha->amswreg[i]);
  printf("\n");

  mem = 0x00020000 + chan * 0x00002000;
  memcpy((int8*)check, (int8*)&rcv->cha->amswmem[mem/4], 100);
  printf("RX buffer: ");
  for (i=0; i<4; i++) printf("%08X ", check[i]);
  printf("\n");

  P = 4;
  
  printf("amsw_RX_done=%d\n", amsw_RX_done(rcv));
  printf("amsw_RX_done=%d\n", amsw_RX_done(rcv));

  print_amsw_msg(0, "JMDC RX:", NULL, rcv);
  
  reg = 16; // Interrupt
  status = amsw_reg_read(card, reg, (u_long*)&data);
  printf("amsw_reg_read(%d,%d) = %2d, data = %08X\n", card, reg, status, data);
  printf("amsw_RX_done=%d\n", amsw_RX_done(rcv));
  status = amsw_reg_read(card, reg, (u_long*)&data);
  printf("amsw_reg_read(%d,%d) = %2d, data = %08X\n", card, reg, status, data);
  printf("amsw_RX_done=%d\n", amsw_RX_done(rcv));
  status = amsw_reg_read(card, reg, (u_long*)&data);
  printf("amsw_reg_read(%d,%d) = %2d, data = %08X\n", card, reg, status, data);
  printf("amsw_RX_done=%d\n", amsw_RX_done(rcv));

  mem = 0x00020000 + chan * 0x00002000;
  
  status = amsw_mem_read(card, mem, (int32*)&rep[0], size);
/*

  printf("amsw_mem_read(%d) = %2d, data = ", card, status);
  for (i=0; i<10; i++) printf("%02X ", rep[i]);
  printf("\n");

  printf("amsw_RX_done=%d\n", amsw_RX_done(rcv));
  printf("amsw_RX_done=%d\n", amsw_RX_done(rcv));
*/

  return 0;
}

//~============================================================================
