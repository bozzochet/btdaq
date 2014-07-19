// file pciamswlib.h
//
// see pciamswlib.c
//
// A.Lebedev Feb-2007...
//

#ifndef _PCIAMSWLIB_H
#define _PCIAMSWLIB_H

#include "mylib.h"

float timeout;              // not very global variable

typedef struct {
  int8  card;
  int8  chan;
  int32 *amswreg;
  int32 *amswmem;
  int32 *ISR;
  int32 *TX_CSR;
  int32 *RX_CSR;
  int32 *TX_mem;
  int32 *RX_mem;
} _amsw_cha;

typedef struct {
  _amsw_cha *cha;
  bool  first;
  bool  last;
  int32 sta;
  int16 err;
  int   ret;
  int16 len;
  int16 dat[4096];
} _amsw_msg;

bool initialize_PCIAMSW(_amsw_cha *cha);
void start_amsw_RX(_amsw_msg* msg);
void stop_amsw_RX(_amsw_msg* msg);
void start_amsw_TX(_amsw_msg* msg);
void stop_amsw_TX(_amsw_msg* msg);
bool amsw_TX_done(_amsw_msg* msg);
bool amsw_RX_done(_amsw_msg* msg);
void print_amsw_msg(int c, char *txt1, char *txt2, _amsw_msg *msg);

int16 amsw_SND_RCV(int action, _amsw_msg *SND, _amsw_msg *RCV, int16 *error);

int amswStateMachine(int action, pAMSBlock pRequest, pAMSBlock pReply);

void to_amsw(int32 RQ_typ, int16  RQ_cnt, int16 *RQ_dat,
             int16 RP_siz, int16 *RP_cnt, int16 *RP_dat, int16 *RP_err);
             
//=============================================================================

#endif // _PCIAMSWLIB_H
