// file cab-slave-2.c
//
//  Example of CAB simulator program:
//  - replies to ECHO command
//  - replies to CAB_GET_ESSENTIAL_STATUS command
//
//  (for tests in SJTU with cab-master program)
//
// Stolen from X.Cai & S.Xu (gcan) and reworked
//
// A.Lebedev - Nov 2006 ...
//

#include "gcanlib.h"

//~----------------------------------------------------------------------------

static int slave_proc(int8 *txbuf, int8 *rxbuf, int rxlen) {

  int txlen;
  int16 CRC, CRC1, CRC2;
  int8 CMD_ID, CMD_LEN;

  switch (rxbuf[0]) {

    case 0x01:
      printf("\rECHO command, %d bytes.\n", rxlen-1);
      memcpy(txbuf, rxbuf+1, rxlen-1);
      txlen = rxlen - 1;
      if (txlen == 0) txbuf[0] = REPPKTEND;
    break;

    case 0x0A:
      printf("\rCAB command. ");
      txbuf[0] = REPPKTABORT;
      txlen    = 0;
      if (rxlen < 5) {
        printf("Wrong length: %d (rxlen).\n", rxlen);
        break;
      }
      CRC1 = rxbuf[rxlen-2] << 8 | rxbuf[rxlen-1];
      CRC2 = CRC_CCITT(rxbuf, rxlen-2);
      if (CRC1 != CRC2) {
        printf("Wrong CRC: 0x%04X (received), 0x%04X (calculated).\n", CRC1, CRC2);
// 0xF7
        break;
      }
      CMD_LEN = rxbuf[2];
      if (CMD_LEN + 2 != rxlen) {
        printf("Wrong length: %d (rxlen), %d (CMD_LEN).\n", rxlen, CMD_LEN);
// 0xFD
        break;
      }
      CMD_ID  = rxbuf[1];
      printf("CMD_ID = 0x%02hX, CMD_LEN = %d bytes.\n", CMD_ID, CMD_LEN);
      if (CMD_ID == 0x03) {     // CAB_GET_ESSENTIAL_STATUS
        int i;
        txbuf[0] = 0x8A;        // CAN_BHDR
        txbuf[1] = 0x30;        // RESP_ID
        txbuf[2] = 60;          // RESP_LEN
        txbuf[3] = 0x00;        // RESP_ACK (see elsewhere)
        for (i=0; i<56; i++) {
          txbuf[4+i] = i;       // RESP_DATA
        }
        CRC = CRC_CCITT(txbuf, 60);
        txbuf[60] = CRC >> 8;
        txbuf[61] = CRC >> 0;
        txlen = 62;
      }
      else {
        printf("Unknown command: 0x%02hX (CMD_ID).\n", CMD_ID);
// 0xFF
        break;
      }
    break;

    default:
      txbuf[0] = REPPKTABORT;
      txlen    = 0;
    break;
  }

  return txlen;
}

//~----------------------------------------------------------------------------

int main (int argc, char **argv) {

  CANCFG cfg_a = {BUSA, {0x15, 0xFF, 0xFF, 0x00}};
  CANCFG cfg_b = {BUSB, {0x15, 0xFF, 0xFF, 0x00}};
  int8 slave;

  setbuf(stdout, NULL);
  printf("\rUsage:\t%s <Slave Address>\n", argv[0]);

  if (argc != 2) exit(1);
  sscanf(argv[1], "%x",(unsigned int *)&slave);
  printf("Slave Address = 0x%02X\n", slave);

  open_epp();
  init_can(&cfg_a);
  init_can(&cfg_b);

  while (1) {
    sfsm(JMDC_0, slave, slave_proc);
    sfsm(JMDC_1, slave, slave_proc);
    sfsm(JMDC_2, slave, slave_proc);
    sfsm(JMDC_3, slave, slave_proc);
  }
}

//~============================================================================
