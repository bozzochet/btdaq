// file canlib.c
//
// Provides a set of basic low-level functions to work with CAN-bus
// via EPP-CAN box.
//
// Software stolen from VK, AK and others was reworked and is used here.
//
// A.Lebedev Mar-2005...
//

#include "canlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//~-- Intel 82527 CAN Controller Registers ---

#define SET   0x02
#define RESET 0x01

#define control_reg       0x00
#define status_reg        0x01
#define cpu_interface     0x02
#define global_mask_std_0 0x06
#define global_mask_std_1 0x07
#define global_mask_ext_0 0x08
#define global_mask_ext_1 0x09
#define global_mask_ext_2 0x0A
#define global_mask_ext_3 0x0B
#define message_15_mask_0 0x0C
#define message_15_mask_1 0x0D
#define message_15_mask_2 0x0E
#define message_15_mask_3 0x0F
#define clkout            0x1F
#define bus_config        0x2F
#define bit_timing_0      0x3F
#define bit_timing_1      0x4F
#define interrupt_reg     0x5F
#define p1conf            0x9F
#define p2conf            0xAF
#define p1in              0xBF
#define p2in              0xCF
#define p1out             0xDF
#define p2out             0xEF

//~-- Intel 82527 Message Object Structure ---

#define control_0     0
#define control_1     1
#define arbitration_0 2
#define arbitration_1 3
#define arbitration_2 4
#define arbitration_3 5
#define mess_config   6
#define data_0        7
#define data_1        8
#define data_2        9
#define data_3       10
#define data_4       11
#define data_5       12
#define data_6       13
#define data_7       14

//~-- CAN address XX1XXNAA ---
//
//    X - do not care,
//    N - CAN0/CAN1 select,
//    AA fully decoded

int8 CAN_addr[2] = {0x20, 0x24};
int8 CAN_data[2] = {0x21, 0x25};
int8 CAN_incr[2] = {0x22, 0x26};

//~============================================================================

void w_CAN(int8 cha, int8 reg, int8 val) {

  if (cha > 1) return;

  outb(CAN_addr[cha], EPP_addr);
  outb(reg,           EPP_data);
  outb(CAN_data[cha], EPP_addr);
  outb(val,           EPP_data);
}

//~----------------------------------------------------------------------------

int8 r_CAN(int8 cha, int8 reg) {

  static int first = TRUE;
  static int8 old_cha;
  static int8 old_reg;

  if (cha > 1) return 0xFF;

  if (first || cha != old_cha || reg != old_reg) {
    first = FALSE;
    outb(CAN_addr[cha], EPP_addr);
    outb(reg,           EPP_data);
    outb(CAN_data[cha], EPP_addr);
    old_cha = cha;
    old_reg = reg;
  }
  return inb(EPP_data);
}

//~============================================================================

void initialize_CAN(_CAN_cha* CAN) {

  int i;
  int8 cha = CAN->cha;

  if (cha > 1) return;

  w_CAN(cha, cpu_interface, 0x40);
  w_CAN(cha, control_reg,   0x41);        // CCE=1, Init=1
  w_CAN(cha, status_reg,    0x07);        // clear status
  w_CAN(cha, bus_config,    0x48);        // bypass comparator, disconnect TX1
  w_CAN(cha, bit_timing_0,  0x00);        // tq = 1 * tsclk = 125 nsec
  w_CAN(cha, bit_timing_1,  0x14);        // sjw = 1, tseg1 = 5, tseg2 = 2
                                          // bittime = 8 * tq = 1000 nsec
  w_CAN(cha, p2conf,        0xFF);        // switch on tranceiver
  w_CAN(cha, p2out,         0x00);

  for(i=1; i<16; i++) {
    int8 offset = 0x10 * i;
    w_CAN(cha, offset + control_0, 0x55); // invalidate all messages
    w_CAN(cha, offset + control_1, 0x55); // invalidate all messages
  }

  w_CAN(cha, global_mask_std_0, 0x00);
  w_CAN(cha, global_mask_std_1, 0x00);

  w_CAN(cha, global_mask_ext_0, CAN->msk>>24);
  w_CAN(cha, global_mask_ext_1, CAN->msk>>16);
  w_CAN(cha, global_mask_ext_2, CAN->msk>> 8);
  w_CAN(cha, global_mask_ext_3, CAN->msk>> 0);

  w_CAN(cha, message_15_mask_0, 0x00);
  w_CAN(cha, message_15_mask_1, 0x00);
  w_CAN(cha, message_15_mask_2, 0x00);
  w_CAN(cha, message_15_mask_3, 0x00);

  w_CAN(cha, control_reg, 0x00);          // CCE=0, Init=0

  r_CAN(cha, status_reg);
}

//~----------------------------------------------------------------------------

void de_initialize_CAN(int8 cha) {

  int8 reg, val;

  if (cha > 1) return;
  
  for (reg=0; reg<0xFF; reg++) {
    val = 0x00;
    w_CAN(cha, reg, val);
  }
}

//~----------------------------------------------------------------------------

void get_CAN_status(_CAN_cha *CAN) {

  int8 cha = CAN->cha;

  if (cha > 1) PANIC("get_CAN_status: wrong cha\n");

  CAN->sta = r_CAN(cha, status_reg);
}

//~============================================================================

void start_CAN_TX(_CAN_msg* msg) {

  int8 cha = msg->CAN->cha;
  int8 offset;
  int i;

  if (cha > 1)                       PANIC("start_CAN_TX: wrong cha\n");
  if (msg->buf < 1 || msg->buf > 14) PANIC("start_CAN_TX: wrong buf\n");
  if (msg->len > 0x0F)               PANIC("start_CAN_TX: wrong len\n");

  offset = 0x10 * msg->buf;

  w_CAN(cha, offset+control_0, 0x55);     // MsgVal=0, TXIE=0
                                          // RXIE=0, IntPnd=0

  w_CAN(cha, offset+arbitration_0, msg->ID>>24);
  w_CAN(cha, offset+arbitration_1, msg->ID>>16);
  w_CAN(cha, offset+arbitration_2, msg->ID>> 8);
  w_CAN(cha, offset+arbitration_3, msg->ID>> 0);

  w_CAN(cha, offset+mess_config, 0x0C+(msg->len<<4));

  for (i=0; i<MIN(msg->len,8); i++) {
    w_CAN(cha, offset+data_0+i, msg->dat[i]);
  }

  w_CAN(cha, offset+control_1, 0x66);     // RmtPnd=0, TxRqst=1,
                                          // CPUUpd=0, NewDat=1.
  w_CAN(cha, offset+control_0, 0x95);     // MsgVal=1, TXIE=0,
                                          // RXIE=0, IntPnd=0.
}

//~----------------------------------------------------------------------------

void stop_CAN_TX(_CAN_msg* msg) {

  int8 cha = msg->CAN->cha;
  int8 offset;

  if (cha > 1)                       PANIC("stop_CAN_TX: wrong cha\n");
  if (msg->buf < 1 || msg->buf > 14) PANIC("stop_CAN_TX: wrong buf\n");

  offset = 0x10 * msg->buf;

  w_CAN(cha, offset+control_0, 0x55);     // MsgVal=0, TXIE=0,
                                          // RXIE=0, IntPnd=0.
  w_CAN(cha, offset+control_1, 0x55);     // RmtPnd=0, TxRqst=0,
                                          // CPUUpd=0, NewDat=0.
}

//~----------------------------------------------------------------------------

void start_CAN_RX(_CAN_msg* msg) {

  int8 cha = msg->CAN->cha;
  int8 offset;

  if (cha > 1)                       PANIC("start_CAN_RX: wrong cha\n");
  if (msg->buf < 1 || msg->buf > 14) PANIC("start_CAN_RX: wrong buf\n");

  offset = 0x10 * msg->buf;

  if (P > 4) {
    printf("start_CAN_RX:\n");
    p_CAN_sta(msg->CAN);
    p_CAN_msg(msg);
  }

  w_CAN(cha, offset+control_0, 0x55);     // MsgVal=0, TXIE=0,
                                          // RXIE=0, IntPnd=0.

  w_CAN(cha, offset+arbitration_0, msg->ID>>24);
  w_CAN(cha, offset+arbitration_1, msg->ID>>16);
  w_CAN(cha, offset+arbitration_2, msg->ID>> 8);
  w_CAN(cha, offset+arbitration_3, msg->ID>> 0);

#ifdef HBB
  w_CAN(cha, offset+mess_config, 0xF4);   // DLC=F, Dir=RX, Xtd
#else
  w_CAN(cha, offset+mess_config, 0x84);   // DLC=8, Dir=RX, Xtd
#endif
  w_CAN(cha, offset+control_1,   0x55);   // RmtPnd=0, TxRqst=0,
                                          // MsgLst=0, NewDat=0.
  w_CAN(cha, offset+control_0,   0x95);   // MsgVal=1, TXIE=0,
                                          // RXIE=0, IntPnd=0.
}

//~----------------------------------------------------------------------------

void stop_CAN_RX(_CAN_msg* msg) {

  int8 cha = msg->CAN->cha;
  int8 offset;

  if (cha > 1)                       PANIC("stop_CAN_RX: wrong cha\n");
  if (msg->buf < 1 || msg->buf > 14) PANIC("stop_CAN_RX: wrong buf\n");

  offset = 0x10 * msg->buf;

  w_CAN(cha, offset+control_0, 0x55);     // MsgVal=0, TXIE=0,
                                          // RXIE=0, IntPnd=0.
  w_CAN(cha, offset+control_1, 0x55);     // RmtPnd=0, TxRqst=0,
                                          // CPUUpd=0, NewDat=0.
}

//~----------------------------------------------------------------------------

bool CAN_TX_done(_CAN_msg* msg) {

  int8 cha = msg->CAN->cha;
  int8 offset;
  int8 TxRqst;

  if (cha > 1)                       PANIC("CAN_TX_done: wrong cha\n");
  if (msg->buf < 1 || msg->buf > 14) PANIC("CAN_TX_done: wrong buf\n");

  msg->err = 0x00;

  if (r_CAN(cha, control_reg) & 0x01) {
    msg->err = 0x04;
    return TRUE;
  }
  
  offset = 0x10 * msg->buf;

  TxRqst = (r_CAN(cha, offset+control_1) >> 4) & 0x03;
  if (TxRqst == RESET) return TRUE;
  else                 return FALSE;
}

//~----------------------------------------------------------------------------

bool CAN_RX_done(_CAN_msg* msg) {

  int8 cha = msg->CAN->cha;
  int8 offset;
  int8 ctr1;
  int8 NewDat;
  int8 MsgLst;

  if (cha > 1)                       PANIC("CAN_RX_done: wrong cha\n");
  if (msg->buf < 1 || msg->buf > 14) PANIC("CAN_RX_done: wrong buf\n");

  offset = 0x10 * msg->buf;

  ctr1 = r_CAN(cha, offset+control_1);
  NewDat = ctr1 & 0x03;

#ifdef HBB
  if ((r_CAN(cha, offset+mess_config) & 0xF0) != 0xF0) {
#else
  if (NewDat == SET) {
#endif
    int i;

    if (P > 4) {
      printf("CAN_RX_done 1:\n");
      p_CAN_sta(msg->CAN);
      p_CAN_msg(msg);
    }

    w_CAN(cha, offset+control_0, 0x7F);                   // reset MsgVal
    w_CAN(cha, offset+control_1, 0xFD);                   // reset NewDat

    msg->ID  = r_CAN(cha, offset+arbitration_0) << 24;
    msg->ID |= r_CAN(cha, offset+arbitration_1) << 16;
    msg->ID |= r_CAN(cha, offset+arbitration_2) <<  8;
    msg->ID |= r_CAN(cha, offset+arbitration_3) <<  0;

    msg->len = r_CAN(cha, offset+mess_config) >> 4;

    for (i=0; i<MIN(msg->len,8); i++) msg->dat[i] = r_CAN(cha, offset+data_0+i);

    msg->err = 0x00;
    ctr1 = r_CAN(cha, offset+control_1);

    if (P > 4) {
      printf("CAN_RX_done 2:\n");
      p_CAN_sta(msg->CAN);
      p_CAN_msg(msg);
    }

    NewDat =  ctr1 & 0x03;
    if (NewDat == SET) msg->err |= 0x01;
    MsgLst = (ctr1 >> 2) & 0x03;
    if (MsgLst == SET) msg->err |= 0x02;
    return TRUE;
  }
  else {
    if (P > 5) {
      printf("CAN_RX_done 3:\n");
      p_CAN_sta(msg->CAN);
      p_CAN_msg(msg);
    }
    return FALSE;
  }
}

//~============================================================================


void pack_CAN_msg_ID(int32 *msg_ID, _ID *ID) {

  *msg_ID = 0;
  pack32(msg_ID, 0x40000000L, ID->First);
  pack32(msg_ID, 0x20000000L, ID->Last);
  pack32(msg_ID, 0x10000000L, ID->Request_Reply);
  pack32(msg_ID, 0x08000000L, ID->Read_Write);
  pack32(msg_ID, 0x04000000L, ID->Bus_for_Request);
  pack32(msg_ID, 0x02000000L, ID->Bus_for_Reply);
  pack32(msg_ID, 0x01000000L, ID->Newborn);
  pack32(msg_ID, 0x00FF0000L, ID->Source);
  pack32(msg_ID, 0x0000FF00L, ID->Destination);
}

//~----------------------------------------------------------------------------

void unpack_CAN_msg_ID(int32 msg_ID, _ID *ID) {

  ID->First           = unpack32(msg_ID, 0x40000000L);
  ID->Last            = unpack32(msg_ID, 0x20000000L);
  ID->Request_Reply   = unpack32(msg_ID, 0x10000000L);
  ID->Read_Write      = unpack32(msg_ID, 0x08000000L);
  ID->Bus_for_Request = unpack32(msg_ID, 0x04000000L);
  ID->Bus_for_Reply   = unpack32(msg_ID, 0x02000000L);
  ID->Newborn         = unpack32(msg_ID, 0x01000000L);
  ID->Source          = unpack32(msg_ID, 0x00FF0000L);
  ID->Destination     = unpack32(msg_ID, 0x0000FF00L);
}

//~============================================================================

void p_CAN_sta(_CAN_cha* CAN) {

  int8 bits[5] = {   0x80,   0x40,   0x20,   0x10,   0x08};
  char *nam[5] = { "Boff", "Warn", "Wake", "RXOK", "TXOK"};
  int i;

  get_CAN_status(CAN);
  printf("CAN%d: sta=%02X: ", CAN->cha, CAN->sta);
  for (i=0; i<5; i++) if (CAN->sta & bits[i]) printf("%s ", nam[i]);
  printf("LEC=0x%01X\n", CAN->sta & 0x07);
}

//~----------------------------------------------------------------------------

void p_CAN_msg(_CAN_msg* msg) {

  int8 cha = msg->CAN->cha;
  int8 ctr0 = r_CAN(cha, 0x10*msg->buf+control_0);
  int8 ctr1 = r_CAN(cha, 0x10*msg->buf+control_1);
  int8 cnfg = r_CAN(cha, 0x10*msg->buf+mess_config);
  char *v[4] = {"?", "R", "S", "?"};
  int i;

  printf("CAN%d/%d: (%02X %02X) ", cha, msg->buf, ctr0, ctr1);

  if (1) {
    int  shft[4] = {       6,      4,      2,        0};
    char *nam[4] = { "MsgVal", "TXIE", "RXIE", "IntPnd"};
    for (i=0; i<4; i++) printf("%s=%s ", nam[i], v[(ctr0>>shft[i])&0x03]);
  }

  if (cnfg & 0x08) {
    int  shft[4] = {       6,        4,        2,        0};
    char *nam[4] = { "RmtPnd", "TxRqst", "CPUUpd", "NewDat"};
    for (i=0; i<4; i++) printf("%s=%s ", nam[i], v[(ctr1>>shft[i])&0x03]);
  }

  else {
    int  shft[4] = {       6,        4,        2,        0};
    char *nam[4] = { "RmtPnd", "TxRqst", "MsgLst", "NewDat"};
    for (i=0; i<4; i++) printf("%s=%s ", nam[i], v[(ctr1>>shft[i])&0x03]);
  }

  printf("\n");
}

//~============================================================================

void print_AMS_CAN_msg(int c, char *txt1, char *txt2, _CAN_msg *msg) {

  _ID ID;
  int i;
  char* t[2][4] = {{"NXT", "ERR", "ABO", "END"},
                   {" I ", " L ", " F ", "F&L"}};

  if (txt2) {
    printf("%7d %s%d %s", c, txt1, msg->CAN->cha, txt2);
  }
  else {
    unpack_CAN_msg_ID(msg->ID, &ID);
    printf("%7d %s%d ID=%08X %d%s%02X%c %s %s l=%d ", 
           c, txt1, msg->CAN->cha, msg->ID, 
           ID.Request_Reply ? ID.Destination : ID.Source,
           ID.Request_Reply ? "<-" : "->",
           ID.Request_Reply ? ID.Source : ID.Destination,
           ID.Newborn ? 'n' : ' ',
           ID.Read_Write ? "W" : "R",
           t[msg->len ? 1 : 0][2*ID.First+ID.Last],
           msg->len);
    for (i=0; i<msg->len; i++) {
      if (i==0) printf("d =");
      printf(" %02X", msg->dat[i]);
    }
  }
  
  printf("\n");
}

//~============================================================================

int16 CAN_SND_RCV(int action, _CAN_msg *SND, _CAN_msg *RCV, int16 *error) {

  static _CAN_cha CAN0, *pCAN0 = &CAN0;
  static _CAN_cha CAN1, *pCAN1 = &CAN1;
  static _ID SND_msg_ID, *pSND_msg_ID = &SND_msg_ID;
  static _ID RCV_msg_ID, *pRCV_msg_ID = &RCV_msg_ID;
  static int state = DONE;
  int8 tmp;
  static int old_state = -1;
  static int c = 0;
  bool Timeout = timer(0, EXPIRED, command_timeout);

  if (action != CONTINUE) state = action;

  switch (state) {

//~--

    case DONE:
    break;

//~--

    case INITIALIZE:
      state = DONE;
      if (!initialize_EPP()) {
        *error = 0x0004;
      }
      else {
        pCAN0->cha = 0;
//      pCAN0->msk = 0xFFFFFFF8L;
//      pCAN0->msk = 0x00FFFF00L;
        pCAN0->msk = 0x17FFFF00L;
        initialize_CAN(pCAN0);
        pCAN1->cha = 1;
//      pCAN1->msk = 0xFFFFFFF8L;
//      pCAN1->msk = 0x00FFFF00L;
        pCAN1->msk = 0x17FFFF00L;
        initialize_CAN(pCAN1);
        if (P > 3) printf("JMDC: CAN-controllers initialized\n");
        *error = 0;
      }
    break;

//~--

    case START:
      c = 0;

      SND->buf = 1;
      RCV->buf = 2;

      unpack_CAN_msg_ID(SND->ID, pSND_msg_ID);
      if (pSND_msg_ID->Bus_for_Request) SND->CAN = pCAN1;
      else                              SND->CAN = pCAN0;
      if (pSND_msg_ID->Bus_for_Reply) RCV->CAN = pCAN1;
      else                            RCV->CAN = pCAN0;
      RCV->ID = SND->ID;
      unpack_CAN_msg_ID(RCV->ID, pRCV_msg_ID);
      pRCV_msg_ID->Request_Reply = REPLY;
      tmp                        = pRCV_msg_ID->Source;
      pRCV_msg_ID->Source        = pRCV_msg_ID->Destination;
      pRCV_msg_ID->Destination   = tmp;
      pack_CAN_msg_ID(&RCV->ID, pRCV_msg_ID);

      start_CAN_RX(RCV);

      if (P > 2) print_AMS_CAN_msg(c, "JMDC TX:", NULL, SND);
      start_CAN_TX(SND);
      timer(0, START);
      *error = 0;
      state = TXING;
    break;

//~--

    case TXING:
      if (CAN_TX_done(SND)) {
        if (SND->err) {
          char b[80];
          stop_CAN_TX(SND);
          stop_CAN_RX(RCV);
          sprintf(b, "Transmitter Error 0x%04X", SND->err);
          print_AMS_CAN_msg(c, "JMDC TX:", b, SND);
          *error = SND->err;
          state = DONE;
        }
        else {
          timer(0, START);
          state = RXING;
        }
      }
      else if (Timeout) {
        stop_CAN_TX(SND);
        stop_CAN_RX(RCV);
        if (P > 2) print_AMS_CAN_msg(c, "JMDC TX:", "Timeout", SND);
        *error = 0x0005;
        state = DONE;
      }
    break;

//~--

    case RXING:
      if (CAN_RX_done(RCV)) {
        if (RCV->err) {
          char b[80];
          stop_CAN_RX(RCV);
          sprintf(b, "Receiver Error 0x%04X", RCV->err);
          print_AMS_CAN_msg(c, "JMDC RX:", b, RCV);
          *error = RCV->err;
          state = DONE;
        }
        else {
          if (P > 2) print_AMS_CAN_msg(c, "JMDC RX:", NULL, RCV);
          *error = 0;
          state = DONE;
        }
      }
      else if (Timeout) {
        stop_CAN_RX(RCV);
        if (P > 2) print_AMS_CAN_msg(c, "JMDC RX:", "Timeout", RCV);
        *error = 0x0006;
        state = DONE;
      }
    break;

//~--

    case STOP:
      stop_CAN_TX(SND);
      if (P > 3) print_AMS_CAN_msg(c, "JMDC TX:", "Abort", SND);
      stop_CAN_RX(RCV);
      if (P > 3) print_AMS_CAN_msg(c, "JMDC RX:", "Abort", RCV);
      *error = 0;
      state = DONE;
    break;

//~--

  } // end-of-switch

//~--

  if (state != old_state) {
    if (P > 3) printf("%6d JMDC: state: %d->%d\n", c, old_state, state);
    old_state = state;
  }

  c = c + 1;
  return state;
}

//~============================================================================

int CANStateMachine(int action, pAMSBlock pRequest, pAMSBlock pReply) {

#define is_req(m)   (((m)->ID & 0x10000000L) == 0x00000000L)
#define is_rep(m)   (((m)->ID & 0x10000000L) == 0x10000000L)

#define is_read(m)  (((m)->ID & 0x08000000L) == 0x00000000L)
#define is_write(m) (((m)->ID & 0x08000000L) == 0x08000000L)

#define is_ERR(m)   ((m)->len == 0 && ((m)->ID & 0x60000000L) == 0x20000000L)
#define is_ABO(m)   ((m)->len == 0 && ((m)->ID & 0x60000000L) == 0x40000000L)
#define is_NXT(m)   ((m)->len == 0 && ((m)->ID & 0x60000000L) == 0x00000000L)
#define is_END(m)   ((m)->len == 0 && ((m)->ID & 0x60000000L) == 0x60000000L)

#define is_last(m)  ((m)->len != 0 && ((m)->ID & 0x20000000L))
#define is_first(m) ((m)->len != 0 && ((m)->ID & 0x40000000L))

  int16 error;
  int status;
  int16 cnt;
  static int32 bytes_still_to_write;
  static int state = DONE;
  static int old_state = -1;
  static int c = 0;

  static _CAN_msg SND, *pSND = &SND;
  static _CAN_msg RCV, *pRCV = &RCV;
  static _ID SND_msg_ID, *pSND_msg_ID = &SND_msg_ID;

  if (action != CONTINUE) state = action;

  switch (state) {

//~--

    case DONE:

      break;

//~--

    case START:
      c = 0;

      status = CAN_SND_RCV(INITIALIZE, pSND, pRCV, &error);
      if (status == DONE && error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      pSND_msg_ID->First           = 1;
      pSND_msg_ID->Request_Reply   = pRequest->BlockType & 0x0002 ? 1 : 0;
      pSND_msg_ID->Read_Write      = pRequest->BlockType & 0x0001 ? 1 : 0;
      pSND_msg_ID->Bus_for_Request = BUS_FOR_REQUEST;
      pSND_msg_ID->Bus_for_Reply   = BUS_FOR_REPLY;
      pSND_msg_ID->Newborn         = unpack16(pRequest->NodeAddress, 0x0100);
      pSND_msg_ID->Source          = unpack16((int16)pRequest->Source, 0xFF00);
      pSND_msg_ID->Destination     = unpack16(pRequest->NodeAddress, 0x00FF);

      if (pRequest->DataType & 0x001F0000) {
        if ((pRequest->DataType & 0x001F0000) == 0x001F0000) {
          pSND->dat[0] = unpack32(pRequest->DataType, 0x001F0000) |
                      (pRequest->BlockType & 0x0001 ? 0x40 : 0x00);
          pSND->dat[1] = unpack32(pRequest->DataType, 0x0000FF00);
          pSND->dat[2] = unpack32(pRequest->DataType, 0x000000FF);
          pSND->len    = 3;
        }
        else {
          pReply->Error = 0x0010;
          state = DONE;
          break;
        }
      }
      else {
        pSND->dat[0] = unpack32(pRequest->DataType, 0x0000001F) |
                      (pRequest->BlockType & 0x0001 ? 0x40 : 0x00);
        pSND->len    = 1;
      }

      bytes_still_to_write = pRequest->DataCount;

      if (bytes_still_to_write && pRequest->Data.p8 == 0) {
        pReply->Error = 0x0011;
        state = DONE;
        break;
      }

      if (bytes_still_to_write) {
        cnt = MIN(8-pSND->len, bytes_still_to_write);                    //~==========
        memcpy((int8*)&pSND->dat[pSND->len],
               (int8*)&pRequest->Data.p8[0], 
                cnt);
        pSND->len = pSND->len + cnt;
        bytes_still_to_write = bytes_still_to_write - cnt;
        if (bytes_still_to_write) pSND_msg_ID->Last = 0;
        else                      pSND_msg_ID->Last = 1;
      }
      else {
        pSND_msg_ID->Last = 1;
      }
      pack_CAN_msg_ID(&pSND->ID, pSND_msg_ID);
      status = CAN_SND_RCV(START, pSND, pRCV, &error);
      pReply->Error = 0;
      state = WRITING;
      break;

//~--

    case WRITING:

      status = CAN_SND_RCV(CONTINUE, pSND, pRCV, &error);

      if (status != DONE) {
        break;
      }

      if (error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      if (!is_rep(pRCV)) {
        pReply->Error = 0x0020;
        state = DONE;
        break;
      }

      if ((is_read(pSND)  && is_write(pRCV)) || 
          (is_write(pSND) && is_read(pRCV))) {
        pReply->Error = 0x0021;
        state = DONE;
        break;
      }

      if (is_ERR(pRCV)) {
        pReply->Error = 0x0022;
        state = DONE;
        break;
      }

      if (is_ABO(pRCV)) {
        pReply->Error = 0x0023;
        state = DONE;
        break;
      }

      if (bytes_still_to_write) {
        if (!is_NXT(pRCV)) {
          pReply->Error = 0x0024;
          state = DONE;
          break;
        }
        else {
          pSND_msg_ID->First = 0;
          pSND->len = MIN(8, bytes_still_to_write);
          memcpy((int8*)&pSND->dat[0],
                 (int8*)&pRequest->Data.p8[pRequest->DataCount-bytes_still_to_write], 
                  pSND->len);
          bytes_still_to_write = bytes_still_to_write - pSND->len;
          if (bytes_still_to_write ) pSND_msg_ID->Last = 0;
          else                       pSND_msg_ID->Last = 1;
          pack_CAN_msg_ID(&pSND->ID, pSND_msg_ID);
          status = CAN_SND_RCV(START, pSND, pRCV, &error);
          break;
        }
      }

      if (is_END(pRCV)) {
        pReply->Error = 0;
        state = DONE;
        break;
      }

      if (is_write(pRCV) && !is_END(pRCV)) {
        pReply->Error = 0x0025;
        state = DONE;
        break;
      }

      if (!is_first(pRCV)) {
        pReply->Error = 0x0026;
        state = DONE;
        break;
      }

      if (!pReply->Data.p8) {
        pReply->Error = 0x0027;
        state = DONE;
        break;
      }

      if (pReply->DataCount + pRCV->len > pReply->BufferSize) {
        pReply->Error = 0x0028;
        state = DONE;
        break;
      }

      memcpy((int8*)&pReply->Data.p8[pReply->DataCount],
             (int8*)&pRCV->dat[0],
              pRCV->len);
      pReply->DataCount = pReply->DataCount + pRCV->len;

      if (is_last(pRCV)) {
        pReply->Error = 0;
        state = DONE;
      }
      else {
        pSND_msg_ID->First = 0;
        pSND_msg_ID->Last  = 0;
        pack_CAN_msg_ID(&pSND->ID, pSND_msg_ID);
        pSND->len = 0;
        status = CAN_SND_RCV(START, pSND, pRCV, &error);
        state = READING;
      }
      break;

//~--

    case READING:

      status = CAN_SND_RCV(CONTINUE, pSND, pRCV, &error);

      if (status != DONE) {
        break;
      }

      if (error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      if (!is_rep(pRCV)) {
        pReply->Error = 0x0030;
        state = DONE;
        break;
      }

      if (!is_read(pRCV)) {
        pReply->Error = 0x0031;
        state = DONE;
        break;
      }

      if (is_END(pRCV)) {
        pReply->Error = 0;
        state = DONE;
        break;
      }

      if (is_ERR(pRCV)) {
        pReply->Error = 0x0032;
        state = DONE;
        break;
      }

      if (is_ABO(pRCV)) {
        pReply->Error = 0x0033;
        state = DONE;
        break;
      }

      if (is_NXT(pRCV)) {
        pReply->Error = 0x0034;
        state = DONE;
        break;
      }

      if (is_first(pRCV)) {
        pReply->Error = 0x0035;
        state = DONE;
        break;
      }

      if (pReply->DataCount + pRCV->len > pReply->BufferSize) {
        pReply->Error = 0x0036;
        state = DONE;
        break;
      }

      memcpy((int8*)&pReply->Data.p8[pReply->DataCount],
             (int8*)&pRCV->dat[0],
              pRCV->len);
      pReply->DataCount = pReply->DataCount + pRCV->len;

      if (is_last(pRCV)) {
        pReply->Error = 0;
        state = DONE;
        break;
      }
      else {
        status = CAN_SND_RCV(START, pSND, pRCV, &error);
        break;
      }

//~--

  } // end-of-switch

//~--

  if (state != old_state) {
    if (P > 3) printf("%6d STAT: state: %d->%d\n", c, old_state, state);
    old_state = state;
  }

  c = c + 1;
  return state;
}

//~============================================================================
