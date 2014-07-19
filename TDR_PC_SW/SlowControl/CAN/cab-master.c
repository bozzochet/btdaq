// file cab-master.c
//
//  Example of CAB commander program:
//  - sends command CAB_GET_ESSENTIAL_STATUS to CAB
//  - sends command CAB_GET_DIGITAL_STATUS to CAB
//  (with correct and incorrect CRC).
//  - sends some other commands (in between)...
//
//  (for tests in SJTU with cab-slave programs)
//  (for tests at CRISA with real CAB FM)
//  (for tests at CERN with real CAB EM)
//
//  A.Lebedev Nov 2006...
//

#include "cablib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x18;    // cab-slave program does not recognize Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void execute_CAB_DUMP_MEMORY_command(
     int8 id, int8 len, int16 off, int8 *idid, int8 *lenlen, int16 *offoff,
     int16 *err) {

  int32 RQ_typ  = 0x0A;
  int8  CMD_ID  = 0x09;   // CAB_DUMP_MEMORY
  int8  CMD_LEN = 7;
  int8  CAB_CMD[256] = {RQ_typ, CMD_ID, CMD_LEN};
  int8  *RQ_dat = &CAB_CMD[1];
  int16 RQ_cnt = CMD_LEN + 1;
  int8  RP_dat[256];
  int16 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int16 CRC;
  
//~--

  CAB_CMD[3] = id;
  CAB_CMD[4] = len;
  CAB_CMD[5] = off >> 8;
  CAB_CMD[6] = off >> 0;

  CRC = CRC_CCITT(CAB_CMD, CMD_LEN);
  printf("Sending CAB_DUMP_MEMORY CRC = %04X\n", CRC);

  CAB_CMD[CMD_LEN]   = CRC >> 8;
  CAB_CMD[CMD_LEN+1] = CRC >> 0;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  *err = check_CAB_reply(RQ_typ, RQ_dat, RP_cnt, RP_dat);
}

//~----------------------------------------------------------------------------

void execute_CAB_GET_ESSENTIAL_STATUS_command(int8 *sta, int16 *err) {

  static bool make_wrong_CRC = FALSE;
  int32 RQ_typ  = 0x0A;
  int8  CMD_ID  = 0x03;   // CAB_GET_ESSENTIAL_STATUS
  int8  CMD_LEN = 3;
  int8  CAB_CMD[256] = {RQ_typ, CMD_ID, CMD_LEN};
  int8  *RQ_dat = &CAB_CMD[1];
  int16 RQ_cnt = CMD_LEN + 1;
  int8  RP_dat[256];
  int16 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int16 CRC;
  
//~--

  printf("Sending CAB_GET_ESSENTIAL_STATUS with %s CRC...\n",
          make_wrong_CRC ? "wrong" : "correct");

  CRC = CRC_CCITT(CAB_CMD, CMD_LEN);
  printf("Correct CRC = %04X", CRC);
  if (make_wrong_CRC) {
    CRC = ~CRC;
    printf(", but will use wrong CRC = %04X", CRC);
  }
  printf("\n");
  make_wrong_CRC = ~make_wrong_CRC;

  CAB_CMD[CMD_LEN]   = CRC >> 8;
  CAB_CMD[CMD_LEN+1] = CRC >> 0;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  *err = check_CAB_reply(RQ_typ, RQ_dat, RP_cnt, RP_dat);
  
  if (!*err && sta) {
    int i;
    for (i=0; i<56; i++) sta[i] = RP_dat[4+i];
  }
}

//~----------------------------------------------------------------------------

void execute_CAB_LOAD_APSW_command(int16 *err) {

  int32 RQ_typ  = 0x0A;
  int8  CMD_ID  = 0x18;   // CAB_LOAD_APSW
  int8  CMD_LEN = 4;
  int8  CMD_DAT = 0xA2;   // SW-Program 1
//int8  CMD_DAT = 0xA4;   // SW-Program 2
  int8  CAB_CMD[256] = {RQ_typ, CMD_ID, CMD_LEN, CMD_DAT};
  int8  *RQ_dat = &CAB_CMD[1];
  int16 RQ_cnt = CMD_LEN + 1;
  int8  RP_dat[256];
  int16 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int16 CRC;
  
//~--

  printf("Sending CAB_LOAD_APSW with correct CRC...\n");

  CRC = CRC_CCITT(CAB_CMD, CMD_LEN);
  printf("Correct CRC = %04X", CRC);
  printf("\n");

  CAB_CMD[CMD_LEN]   = CRC >> 8;
  CAB_CMD[CMD_LEN+1] = CRC >> 0;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  *err = check_CAB_reply(RQ_typ, RQ_dat, RP_cnt, RP_dat);
}

//~----------------------------------------------------------------------------

void execute_CAB_GO_APSW_command(int16 *err) {

  int32 RQ_typ  = 0x0A;
  int8  CMD_ID  = 0x1B;   // CAB_GO_APSW
  int8  CMD_LEN = 7;
  int8  CMD_DAT[4] = {0x00, 0x00, 0x00, 0x00};
  int8  CAB_CMD[256] = {RQ_typ, CMD_ID, CMD_LEN, 
                        CMD_DAT[0], CMD_DAT[1], CMD_DAT[2], CMD_DAT[3]};
  int8  *RQ_dat = &CAB_CMD[1];
  int16 RQ_cnt = CMD_LEN + 1;
  int8  RP_dat[256];
  int16 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int16 CRC;
  
//~--

  printf("Sending CAB_GO_APSW with correct CRC...\n");

  CRC = CRC_CCITT(CAB_CMD, CMD_LEN);
  printf("Correct CRC = %04X", CRC);
  printf("\n");

  CAB_CMD[CMD_LEN]   = CRC >> 8;
  CAB_CMD[CMD_LEN+1] = CRC >> 0;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err || use_MEM) return;

  *err = check_CAB_reply(RQ_typ, RQ_dat, RP_cnt, RP_dat);
}

//~----------------------------------------------------------------------------

void execute_CAB_GET_DIGITAL_STATUS_command(int16 *err) {

  static bool make_wrong_CRC = FALSE;
  int32 RQ_typ  = 0x0A;
  int8  CMD_ID  = 0x05;   // CAB_GET_DIGITAL_STATUS
  int8  CMD_LEN = 4;
  int8  CMD_DAT = 0x01;   // N = Nominal
  int8  CAB_CMD[256] = {RQ_typ, CMD_ID, CMD_LEN, CMD_DAT};
  int8  *RQ_dat = &CAB_CMD[1];
  int16 RQ_cnt = CMD_LEN + 1;
  int8  RP_dat[256];
  int16 RP_siz = sizeof(RP_dat);
  int32 RP_cnt;
  int16 CRC;
  
//~--

  printf("Sending CAB_GET_DIGITAL_STATUS with %s CRC...\n",
         make_wrong_CRC ? "wrong" : "correct");

  CRC = CRC_CCITT(CAB_CMD, CMD_LEN);
  printf("Correct CRC = %04X", CRC);
  if (make_wrong_CRC) {
    CRC = ~CRC;
    printf(", but will use wrong CRC = %04X", CRC);
  }
  printf("\n");
  make_wrong_CRC = ~make_wrong_CRC;

  CAB_CMD[CMD_LEN]   = CRC >> 8;
  CAB_CMD[CMD_LEN+1] = CRC >> 0;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err || use_MEM) return;

  *err = check_CAB_reply(RQ_typ, RQ_dat, RP_cnt, RP_dat);
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int16 err;
  int8 sta[56];
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  execute_CAB_GET_ESSENTIAL_STATUS_command(sta, &err);
//execute_CAB_DUMP_MEMORY_command(0xA7, 0x08, 0x0000, NULL, NULL, NULL, &err);
//exit(1);

  printf("err = %04X\n\n", err);

//execute_CAB_GET_ESSENTIAL_STATUS_command(NULL, &err);
//printf("err = %04X\n\n", err);

  execute_CAB_LOAD_APSW_command(&err);
  printf("err = %04X\n\n", err);
  printf("Delay...\r");
  sleep(5);

  execute_CAB_GO_APSW_command(&err);
  printf("err = %04X\n\n", err);
  printf("Delay...\r");
  sleep(5);
  
  execute_CAB_GET_DIGITAL_STATUS_command(&err);
  printf("err = %04X\n\n", err);

//execute_CAB_GET_DIGITAL_STATUS_command(&err);
//printf("err = %04X\n\n", err);
  
  return 0;
}

//~----------------------------------------------------------------------------
