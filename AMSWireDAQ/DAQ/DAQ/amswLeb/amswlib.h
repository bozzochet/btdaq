// file amswlib.h
//
// A.Lebedev Feb-2007...
//

//~----------------------------------------------------------------------------

#ifndef _AMSWLIB_H
#define _AMSWLIB_H

#include "mylib.h"
#include "tcplib.h"
#include "easslib.h"

//~============================================================================

//~--- Common Function(s) ---

void to_AMSW(int R_W,
             int32 RQ_typ, int32  RQ_cnt, int16 *RQ_dat,
             int32 RP_siz, int32 *RP_cnt, int16 *RP_dat, int16 *RP_err);

//~============================================================================

//~--- EPPCAN Box Interface Functions ---

#include "epplib.h"

typedef struct {
  int8  cha;
  bool  first;
  bool  last;
  int16 sta;
  int16 err;
  int16 adr;
  int16 len;
  int16 dat[4096];
} _AMSW_msg;

//~----------------------------------------------------------------------------

void generate_EPPCAN_box_trigger(int16 trig_msk, int16 *err);
void get_EPPCAN_box_busy(int16 *busy_sta, int16 *err);

void start_AMSW_RX(_AMSW_msg* msg);
void stop_AMSW_RX(_AMSW_msg* msg);
void start_AMSW_TX(_AMSW_msg* msg);
void stop_AMSW_TX(_AMSW_msg* msg);
bool AMSW_TX_done(_AMSW_msg* msg);
bool AMSW_RX_done(_AMSW_msg* msg);

int16 AMSW_SND_RCV(int action, _AMSW_msg *SND, _AMSW_msg *RCV, int16 *error);

int AMSWStateMachine(int action, pAMSBlock pRequest, pAMSBlock pReply);

//~============================================================================

//~--- PCIAMSW Board Interface Functions ---

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

//~----------------------------------------------------------------------------

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

//~============================================================================

#endif // _AMSWLIB_H

