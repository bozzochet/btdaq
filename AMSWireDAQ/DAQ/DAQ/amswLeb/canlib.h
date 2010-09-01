// file canlib.h
//
// see file canlib.c
//
// A.Lebedev Mar-2005...
//

#ifndef _CANLIB_H
#define _CANLIB_H

//~----------------------------------------------------------------------------

#include "epplib.h"

typedef struct {
  int8  cha;
  int8  sta;
  int32 msk;
} _CAN_cha;

typedef struct {
  _CAN_cha *CAN;
  int8  buf;
  int8  err;
  int32 ID;
  int8  len;
  int8  dat[8];
} _CAN_msg;

typedef struct {
  int8 First;
  int8 Last;
  int8 Request_Reply;
  int8 Read_Write;
  int8 Bus_for_Request;
  int8 Bus_for_Reply;
  int8 Newborn;
  int8 Source;
  int8 Destination;
} _ID;

typedef struct {
  _ID *SND_msg_ID;
  _CAN_msg *SND;
  _CAN_msg *RCV;
  int timer;
  int state_1;
  int old_state_1;
  int c_1;
  AMSBlock *RQ;
  AMSBlock *RP;
  int state_2;
  int old_state_2;
  int c_2;
  int32 bytes_still_to_write;
} _channel;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void w_CAN(int8 cha, int8 reg, int8 val);
int8 r_CAN(int8 cha, int8 reg);

void initialize_CAN(_CAN_cha* CAN);
void de_initialize_CAN(int8 cha);
void get_CAN_status(_CAN_cha *CAN);

void start_CAN_TX(_CAN_msg* msg);
void stop_CAN_TX(_CAN_msg* msg);
void start_CAN_RX(_CAN_msg* msg);
void stop_CAN_RX(_CAN_msg* msg);
bool CAN_TX_done(_CAN_msg* msg);
bool CAN_RX_done(_CAN_msg* msg);

void pack_CAN_msg_ID(int32 *msg_ID, _ID *ID);
void unpack_CAN_msg_ID(int32 msg_ID, _ID *ID);

void p_CAN_sta(_CAN_cha* CAN);
void p_CAN_msg(_CAN_msg* msg);

void print_AMS_CAN_msg(int c, char *txt1, char *txt2, _CAN_msg *msg);

int16 CAN_SND_RCV(int action, _CAN_msg *SND, _CAN_msg *RCV, int16 *error);
int CANStateMachine(int action, pAMSBlock pRequest, pAMSBlock pReply);

//~============================================================================

#endif // _CANLIB_H

