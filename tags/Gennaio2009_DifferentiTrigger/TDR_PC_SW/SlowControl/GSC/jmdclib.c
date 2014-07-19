// file jmdclib.c
//
// A.Lebedev, May-2008...
//
// A.Basili,  Aug-2008...

#include "jmdclib.h"


//~============================================================================
//
//                  Q-LIST COMMANDS
//
//~============================================================================

void write_JMDC_Q_list_status(int16 status, int16 *err) {

  int32 RQ_typ = 0x1F0700;
  int16 RQ_dat[1];
  int32 RQ_cnt = sizeof(RQ_dat) / 2;

  RQ_dat[0] = status;

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, RQ_dat,       0,    NULL,   NULL,     err);
  
  if (*err) return;
}

//~----------------------------------------------------------------------------

void start_JMDC_Q_list(int16 *err) {

  write_JMDC_Q_list_status(1, err);
}

//~----------------------------------------------------------------------------

void stop_JMDC_Q_list(int16 *err) {

  write_JMDC_Q_list_status(0, err);
}

//~----------------------------------------------------------------------------

void check_JMDC_Q_list_status(int16 *status, int16 *err) {

  int32 RQ_typ = 0x1F0700;
  int16 RP_dat[1];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_JMDC(R_W,  RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_JMDC(READ, RQ_typ,      0,   NULL,  RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err) return;

  if (RP_cnt != 1) {
    *err = 0x0100;
    return;
  }

  if (status) *status = RP_dat[0];
}

//~----------------------------------------------------------------------------

void read_JMDC_Q_list(int *n, int16 *dat, int16 *err) {

  int i;
  int32 RQ_typ = 0x1F0701;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

//to_JMDC(W_R,  RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_JMDC(READ, RQ_typ,      0,   NULL,  RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err) return;
  
  if (n) *n = RP_cnt;

  if (dat) for (i=0; i<RP_cnt; i++) dat[i] = RP_dat[i];
}

//~============================================================================
//
//                                  SYSTEM COMMAND
//
//~============================================================================

void boot_JMDC(int16 *err){
  
  int32 RQ_typ = 0x0;
  int32 RQ_cnt = 0;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int32 RP_cnt;

  int i;

//to_JMDC(W_R,   RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt,   NULL,  RP_siz, &RP_cnt, RP_dat,     err);
  
  if (*err) return;

  for (i=0; i<RP_cnt; i++) printf("%04X ", RP_dat[i]);

}


//~----------------------------------------------------------------------------

void ping_JMDC(int n, int16 *dat, int *nn, int16 *datdat, int16 *err) {

  int32 RQ_typ = 0x01;
  int32 RQ_cnt = n;
  int32 RP_cnt;

//to_JMDC(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(READ, RQ_typ, RQ_cnt,    dat, 0xFFFF, &RP_cnt, datdat,     err);

  if (*err) return;
  
  if (nn) *nn = RP_cnt;
}

//~============================================================================
//
//                                  NASA COMMAND
//
//~============================================================================

void set_owner(int16 i1553, int16 iMCC, int16 iHRDL, int16 iDAQ, int16 *err){

  int32 RQ_typ = 0x1f0105;
  int32 RQ_cnt = 1;
  int16 RQ_dat;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;

  pack16(&RQ_dat, 0xF000, i1553);
  pack16(&RQ_dat, 0x0F00, iMCC);
  pack16(&RQ_dat, 0x00F0, iHRDL);
  pack16(&RQ_dat, 0x000F, iDAQ);

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, &RQ_dat, RP_siz,    NULL,   NULL,     err);

}

//~============================================================================
//
//                          INTERFACE CONTROL COMMAND
//
//~============================================================================

void hrdl_light(bool o_o, int16 *err){

  int32 RQ_typ = 0x1f0300;
  int32 RQ_cnt = 1;
  int16 RQ_dat;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;

  RQ_dat = o_o ? 1 : 0;

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, &RQ_dat, RP_siz,    NULL,   NULL,     err);

}

//~----------------------------------------------------------------------------

void hrdl_data(bool o_o, int16 *err){

  int32 RQ_typ = 0x1f0301;
  int32 RQ_cnt = 1;
  int16 RQ_dat;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;

  RQ_dat = o_o ? 1 : 0;

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, &RQ_dat, RP_siz,    NULL,   NULL,     err);

}

//~----------------------------------------------------------------------------

void RS232_data(bool o_o, int16 *err){

  int32 RQ_typ = 0x1f0208;
  int32 RQ_cnt = 1;
  int16 RQ_dat;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;

  RQ_dat = o_o ? 1 : 0;

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, &RQ_dat, RP_siz,    NULL,   NULL,     err);

}

//~----------------------------------------------------------------------------

void RS422_data(bool o_o, int16 *err){

  int32 RQ_typ = 0x1f0302;
  int32 RQ_cnt = 1;
  int16 RQ_dat;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;

  RQ_dat = o_o ? 1 : 0;

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, &RQ_dat, RP_siz,    NULL,   NULL,     err);

}
/* //~---------------------------------------------------------------------------- */

/* void open_envelop(int16 *dat){ */

/*   use_BUFF = TRUE; */
/*   pAMSWBlock = dat; */
/*   AMSWsize = 0;  */

/* } */

/* //~---------------------------------------------------------------------------- */

/* void close_envelop(int16 **dat, int16 *n){ */

/*   use_BUFF = FALSE; */
/*   *dat = pAMSWBlock; */
/*   *n = AMSWsize; */
/*   AMSWsize = 0; */

/* } */

/* //~---------------------------------------------------------------------------- */

/* void print_envelop(int16 *dat, int16 n){ */

/*   int i; */
/*   printf ("AMSWsize = %d\n", n); */
/*   printf ("ENVELOP: "); */
/*   for (i=0; i<n; i++) printf ("%04X ", *(dat+i)); */
/*   printf ("\n"); */
/* } */

/* //~---------------------------------------------------------------------------- */

/* void send_envelop(int16 *dat, int16 n, int16 *err){ */

/*   int32 RQ_typ = 0x1f0381; */
/*   int32 RQ_cnt = n; */
/*   int16 RP_dat[10000]; */
/*   int32 RP_siz = sizeof(RP_dat) / 2; */

/* //to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err); */
/*   to_JMDC(READ, RQ_typ, RQ_cnt, dat, RP_siz,    NULL,   NULL,     err); */

/* } */

//~============================================================================
//
//                           FILE OPERATION COMMAND
//
//~============================================================================

void file_summary(int16 *err){

  int32 RQ_typ = 0x1f0588;
  int32 RQ_cnt = 0;
  int16 RQ_dat = 0;
  int16 RP_dat[10000];
  int32 RP_cnt;
  int32 RP_siz = sizeof(RP_dat) / 2;

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(READ, RQ_typ, RQ_cnt, &RQ_dat, RP_siz,  &RP_cnt, &RP_dat[0],     err);

}

//~----------------------------------------------------------------------------

void load_image(char *filename, int16 *err){

  int32 RQ_typ = 0x1f0590;
  int32 RQ_cnt;
  int16 *RQ_dat = NULL;
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;

  FILE *image = NULL;
  
  RQ_dat = malloc(10000000);
  if(RQ_dat == NULL){
    fprintf(stderr, "unable to allocate memory!\n");
    exit (1);
  }

  image = fopen(filename, "rb");
  if (image == NULL) {
    fprintf(stderr, "unable to open %s.\n", filename);
    exit (1);
  }

  RQ_cnt = fread (RQ_dat, sizeof(int16), 20000000, image);
  if (!RQ_cnt){
    fprintf(stderr, "File is empty!");
    exit (1);
  } else if (P > 1) printf ("RQ_cnt = %d\n", RQ_cnt);

  swap16(RQ_dat, RQ_cnt);

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, RQ_dat, RP_siz,    NULL,   NULL,     err);

}

//~----------------------------------------------------------------------------

void load_JMDC_FLASH_file(char filename[12], int16 *err){

  int32 RQ_typ = 0x1f0583;
  int32 RQ_cnt = 6; // 12 bytes
  int16 RP_dat[10000];
  int32 RP_siz = sizeof(RP_dat) / 2;
  int16 name[6];
  int i;
  for (i=0; i<6; i++) name[i]= filename[2*i]<<8 | filename[2*i+1];

//to_JMDC(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_JMDC(WRITE, RQ_typ, RQ_cnt, &name[0], RP_siz,    NULL,   NULL,     err);
}

//~============================================================================
//
//                      MAIN INTERFACE FUNCTION TO JMDC
//
//~============================================================================

void to_JMDC(int R_W,
             int32 RQ_typ, int32  RQ_cnt, int16 *RQ_dat,
             int32 RP_siz, int32 *RP_cnt, int16 *RP_dat, int16 *RP_err) {

  static AMSBlock Request, *pRequest = &Request;
  static AMSBlock Reply,   *pReply   = &Reply;

  pRequest->BlockType    = R_W == WRITE ? 1 : 0;
  pRequest->NodeAddress  = NODE_ADR;
  pRequest->DataType     = RQ_typ;
  pRequest->DataCount    = RQ_cnt * 2;
  pRequest->Data.p8      = NULL;
  pRequest->Data.p16     = RQ_dat;
  pRequest->Source       = 0x0000;

  pReply->Error       = 0x0000;
  pReply->BufferSize  = RP_siz * 2;
  pReply->Data.p8     = NULL;
  pReply->Data.p16    = RP_dat;
  pReply->BlockType   = 0x0002 | (pRequest->BlockType & 0x0001);
  pReply->NodeAddress = pRequest->NodeAddress;
  pReply->DataType    = pRequest->DataType;
  pReply->DataCount   = 0;
  pReply->Source      = pRequest->Source;

  if (P > 1) print_AMS_data_block("REQUEST:", pRequest);

//~---
  
  if (FALSE) {
  }
  
//~---

  else if (use_MEM) {
  }
  
//~---

  else if (use_TCP) {
    open_TCP_connection(CS_address, CS_port, &pReply->Error);
    if (!pReply->Error) TCP_SND_RCV(pRequest, pReply);
    close_TCP_connection();
  }
  
//~---

  else if (use_EAS) {
    int i;
    static int16 RQ_dat_dat[100000];
    if (RQ_dat) {
      for (i=0; i<pRequest->DataCount/2; i++) {
        RQ_dat_dat[i] = (RQ_dat[i] >> 8) | (RQ_dat[i] << 8);
      }
      pRequest->Data.p16 = RQ_dat_dat;
    }
    if (P > 0) printf("PAD: Sending command...\n");
    eAssRequestWithReply(pRequest, pReply);
    pReply->Data.p16 = RP_dat;
    if (RP_dat) {
      for (i=0; i<pReply->DataCount/2; i++) {
        RP_dat[i] = (((int16*)pReply->Contents)[i] >> 8) |
                    (((int16*)pReply->Contents)[i] << 8);
      }
    }
  }

//~---

  if (RP_cnt) *RP_cnt = pReply->DataCount / 2;
  if (RP_err) *RP_err = pReply->Error;
  if (P > 1) print_AMS_data_block("REPLY:  ", pReply);
}
  
//~============================================================================
