// file amswlib.c
//
// A.Lebedev Feb-2007...
//

//~----------------------------------------------------------------------------

#include "amswlib.h"

AMSBlock Request, *pRequest = &Request;
AMSBlock Reply,   *pReply   = &Reply;

//~============================================================================
//
//            COMMON (EPPCAN & PCIAMSW) LIBRARY
//
//~============================================================================

static bool_L timeout_set = FALSE;
static float timeout;

//~----------------------------------------------------------------------------

void set_AMSW_timeout(float value) {

  timeout     = value;
  timeout_set = TRUE;
}

//~----------------------------------------------------------------------------

bool_L get_AMSW_timeout(float *value) {

  if (value) *value = timeout;
  return timeout_set;
}

//~----------------------------------------------------------------------------

void to_AMSW(int R_W, int32 RQ_typ, int32  RQ_cnt, int16 *RQ_dat, int32 RP_siz, int32 *RP_cnt, int16 *RP_dat, int16 *RP_err) {

  pRequest->BlockType    = R_W == WRITE ? 1 : 0;
  pRequest->NodeAddress  = NODE_ADR;
  pRequest->DataType     = RQ_typ;
  pRequest->DataCount    = RQ_cnt * 2;
  pRequest->Data.p8      = NULL;
  pRequest->Data.p16     = RQ_dat;
  pack16(&pRequest->Source, 0xFF00, TX);

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

  else if (use_AMSW) {
    if (use_PCIAMSW) {
      amswStateMachine(START, pRequest, pReply);
      while (amswStateMachine(CONTINUE, pRequest, pReply) != DONE);
    }
    else {
      AMSWStateMachine(START, pRequest, pReply);
      while (AMSWStateMachine(CONTINUE, pRequest, pReply) != DONE);
    }
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
//
//            EPPCAN BOX AMS-WIRE LIBRARY
//
//~============================================================================

static bool_L EPP_must_be_initialized = TRUE;

//~-- EPP-CAN Box Trigger & Busy

#define trig_adr 24  // (W) only 2 LSB used [and not working]
#define busy_adr 24  // (R) only 2 LSB used, "busy" is coded by 0

//~-- AMS-Wire Registers ---

#define TX_adr 0   // watch offset
#define TX_sta 1   // watch offset
#define RX_adr 2   // watch offset
#define RX_sta 3   // watch offset
#define w_adr 16
#define r_adr 17
#define w_dat 18
#define r_dat 19
#define w_CRC 22
#define r_CRC 23

//~-- AMS-Wire RX Status Register ---

#define RX_run     0x8000
#define RX_done    0x4000
#define RX_overrun 0x2000
#define RX_mfail   0x1000  // memory timing error
#define RX_perr    0x0400  // parity error
#define RX_movfl   0x0200  // memory overflow attempt
#define RX_BC2     0x0100
#define RX_BC1     0x0080     
#define RX_ferr    0x0040  // AMS-Wire packet format error
#define RX_error   RX_mfail | RX_perr | RX_movfl | RX_ferr

//~-- AMS-Wire TX Status Register ---

#define TX_run     0x8000
#define TX_BC2     0x2000
#define TX_BC1     0x1000
#define TX_wc      0x0FFF

//~============================================================================

void generate_EPPCAN_box_trigger(int16 trig_msk, int16 *err) {

  *err = 0x0000;
  
  if (EPP_must_be_initialized) {
    if (!initialize_EPP()) {
      *err = 0x0001;
      return;
    }
    else {
      EPP_must_be_initialized = FALSE;
    }
  }
  outb(trig_adr, EPP_addr);
  outw(trig_msk, EPP_data);
}

//~----------------------------------------------------------------------------

void get_EPPCAN_box_busy(int16 *busy_sta, int16 *err) {

  *err = 0x0000;
  
  if (EPP_must_be_initialized) {
    if (!initialize_EPP()) {
      *err = 0x0001;
      return;
    }
    else {
      EPP_must_be_initialized = FALSE;
    }
  }

  outb(busy_adr, EPP_addr);
  *busy_sta = inw(EPP_data);
}

//~============================================================================

void start_AMSW_RX(_AMSW_msg* msg) {

  int8 offset = 4 * msg->cha;

  msg->adr = 0x2000 * msg->cha + 0x1000;

  outb(offset+RX_adr, EPP_addr);
  outw(msg->adr,      EPP_data);
  outb(offset+RX_sta, EPP_addr);
  inw(EPP_data);
}

//~----------------------------------------------------------------------------

void stop_AMSW_RX(_AMSW_msg* msg) {
}

//~----------------------------------------------------------------------------

void start_AMSW_TX(_AMSW_msg* msg) {

  int8 offset = 4 * msg->cha;
  int i;
  
  msg->adr = 0x2000 * msg->cha;

  outb(w_adr,    EPP_addr);
  outw(msg->adr, EPP_data);
  
  outb(w_dat, EPP_addr);
  for (i=0; i<msg->len; i++) outw(msg->dat[i], EPP_data);

  outb(offset+TX_adr, EPP_addr);
  outw(msg->adr,      EPP_data);

  msg->sta = TX_run | (msg->len & TX_wc);
  if (msg->first) msg->sta |= TX_BC1;
  if (msg->last)  msg->sta |= TX_BC2;
  
  outb(offset+TX_sta, EPP_addr);
  outw(msg->sta,      EPP_data);
}

//~----------------------------------------------------------------------------

void stop_AMSW_TX(_AMSW_msg* msg) {
}

//~----------------------------------------------------------------------------

bool_L AMSW_TX_done(_AMSW_msg* msg) {

  int8 offset = 4 * msg->cha;

  outb(offset+TX_sta, EPP_addr);
  msg->sta = inw(EPP_data);

  if (msg->sta & TX_run) return FALSE;
  else                   return TRUE;
}

//~----------------------------------------------------------------------------

bool_L AMSW_RX_done(_AMSW_msg* msg) {

  int8 offset = 4 * msg->cha;
  bool_L error = FALSE;
  int16 CRC;
  int i;

  msg->err = 0x0000;
  
  outb(offset+RX_sta, EPP_addr);
  msg->sta = inw(EPP_data);
  outb(offset+RX_adr, EPP_addr);
  msg->len = inw(EPP_data) - msg->adr;
  
  msg->first = (msg->sta & RX_BC1) ? TRUE : FALSE;
  msg->last  = (msg->sta & RX_BC2) ? TRUE : FALSE;

  if (msg->sta & RX_done) {

    error = (msg->sta & (RX_error | RX_run | RX_overrun)) ? TRUE : FALSE;
    if (error) msg->err = 0x0001;
    if (P > 3 || (P > 2 && error)) {
      char *bits[16] = {"Run",  "Done",   "OverRun", "MemTiming", 
                        "0800", "Parity", "MemOvfl", "BC2",
                        "BC1",  "FmtErr", "0020",    "0010",
                        "0008", "0004",   "0002",    "0001"};
      printf("       RX: %s len=%-4d sta=%04X: ", 
             error ? "*** ERROR-1 ***" : "", msg->len, msg->sta);
      for (i=0; i<16; i++) if (msg->sta & (0x8000>>i)) printf("%s ", bits[i]);
      printf("\n");
    }

    if (error) return TRUE;
    
    if (msg->first) {
      outb(r_CRC, EPP_addr);
      outw(0,     EPP_data);
    }

    outb(r_adr,    EPP_addr);
    outw(msg->adr, EPP_data);

    outb(r_dat, EPP_addr);
    for (i=0; i<msg->len; i++) msg->dat[i] = inw(EPP_data);

//printf("msg->len = %d, msg->dat = ", msg->len);
//for (i=0; i<msg->len; i++) printf("%04X ", msg->dat[i]);
//printf("\n");

    outb(offset+RX_sta, EPP_addr);
    msg->sta = inw(EPP_data);
    error = (msg->sta & (RX_done | RX_run | RX_overrun)) ? TRUE : FALSE;
    if (error) msg->err = 0x0002;
    if (P > 3 || (P > 2 && error)) {
      char *bits[16] = {"Run",  "Done",   "OverRun", "MemTiming", 
                        "0800", "Parity", "MemOvfl", "BC2",
                        "BC1",  "FmtErr", "0020",    "0010",
                        "0008", "0004",   "0002",    "0001"};
      printf("       RX: %s          sta=%04X: ", 
             error ? "*** ERROR-2 ***" : "", msg->sta);
      for (i=0; i<16; i++) if (msg->sta & (0x8000>>i)) printf("%s ", bits[i]);
      printf("\n");
    }

    if (error) return TRUE;

    if (msg->len && msg->last) {
      outb(r_CRC, EPP_addr);
      CRC = inw(EPP_data);
      error = CRC ? TRUE : FALSE;
      if (error) msg->err = 0x0003;
      if (P > 3 || (P > 2 && error)) {
        printf("       RX: %s r_CRC=%04X: (must be 00000)\n",
               error ? "*** ERROR-3 ***" : "", CRC);
      }
    }
    else {
      if (P > 3) {
        if (!msg->len)  printf("       RX:  no CRC for zero-length message\n");
        if (!msg->last) printf("       RX:  no CRC for not last message\n");
      }
    }
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//~============================================================================

void print_AMSW_msg(int c, char *txt1, char *txt2, _AMSW_msg *msg) {

  int i, BC1, BC2;
  char* t[2][4] = {{"NXT", "ERR", "ABO", "END"},
                   {" I ", " L ", " F ", "F&L"}};

#if 0
  if (txt2) {
    printf("%7d %s%d %s", c, txt1, msg->cha, txt2);
  }
  else {
    BC1 = msg->first ? 1 : 0;
    BC2 = msg->last  ? 1 : 0;
    printf("%7d %s%d %s l=%d ", c, txt1, msg->cha, 
           t[msg->len ? 1 : 0][2*BC1+BC2], msg->len);
    for (i=0; i<msg->len; i++) {
      if (i==0) printf("d =");
      printf(" %04X", msg->dat[i]);
    }
  }
#endif

  if (txt2) printf("%7d %s%d %s\n", c, txt1, msg->cha, txt2);
  BC1 = msg->first ? 1 : 0;
  BC2 = msg->last  ? 1 : 0;
  printf("%7d %s%d %s l=%d ", c, txt1, msg->cha, 
         t[msg->len ? 1 : 0][2*BC1+BC2], msg->len);
  for (i=0; i<msg->len; i++) {
    if (i==0) printf("d =");
    printf(" %04X", msg->dat[i]);
  }

  printf("\n");
}

//~============================================================================

int16 AMSW_SND_RCV(int action, _AMSW_msg *SND, _AMSW_msg *RCV, int16 *error) {

  static int state = DONE;
//static int old_state = -1;
  static int c = 0;
  bool_L Timeout = timer(0, EXPIRED, timeout);

  *error = 0;

  if (action != CONTINUE) state = action;

  switch (state) {

    case INITIALIZE:
      if (EPP_must_be_initialized) {
        if (!initialize_EPP()) *error = 0x0004;
        else                   EPP_must_be_initialized = FALSE;
      }
      state = DONE;
    break;

//~--

    case START:
      c = 0;

      start_AMSW_RX(RCV);

      if (P > 2) print_AMSW_msg(c, "JMDC TX:", NULL, SND);
      start_AMSW_TX(SND);
      timer(0, START);
      state = TXING;
    break;

//~--

    case TXING:
      if (AMSW_TX_done(SND)) {
        timer(0, START);
        state = RXING;
      }
      else {
        if (Timeout) {
          stop_AMSW_TX(SND);
          stop_AMSW_RX(RCV);
          if (P > 2) print_AMSW_msg(c, "JMDC TX:", "Timeout", SND);
          *error = 0x0005;
          state = DONE;
        }
      }
    break;

//~--

    case RXING:
      if (AMSW_RX_done(RCV)) {
        if (RCV->err) {
          char b[80];
          sprintf(b, "Error 0x%04X", RCV->err);
          if (P > 2) print_AMSW_msg(c, "JMDC RX:", b, RCV);
          *error = RCV->err;
          state = DONE;
        }
        else {
          if (P > 2) print_AMSW_msg(c, "JMDC RX:", NULL, RCV);
          *error = 0;
          state = DONE;
        }
      }
      else {
        if (Timeout) {
          stop_AMSW_RX(RCV);
          if (P > 2) print_AMSW_msg(c, "JMDC RX:", "Timeout", RCV);
          *error = 0x0006;
          state = DONE;
        }
      }
    break;

//~--

    case STOP:
      stop_AMSW_TX(SND);
      if (P > 3) print_AMSW_msg(c, "JMDC TX:", "Abort", SND);
      stop_AMSW_RX(RCV);
      if (P > 3) print_AMSW_msg(c, "JMDC RX:", "Abort", RCV);
      *error = 0;
      state = DONE;
    break;

//~--

  } // end-of-switch

//~--

//if (state != old_state) {
//  if (P > 3) printf("%7d JMDC: state: %d->%d\n", c, old_state, state);
//  old_state = state;
//}

  if (DONE && *error) EPP_must_be_initialized = TRUE;

  c = c + 1;
  return state;
}

//~============================================================================

int AMSWStateMachine(int action, pAMSBlock pRequest, pAMSBlock pReply) {

#define is_ERR(m)   ((m)->len == 0 &&  (m)->last && !(m)->first)
#define is_ABO(m)   ((m)->len == 0 && !(m)->last &&  (m)->first)
#define is_NXT(m)   ((m)->len == 0 && !(m)->last && !(m)->first)
#define is_END(m)   ((m)->len == 0 &&  (m)->last &&  (m)->first)

#define is_last(m)  ((m)->len != 0 && (m)->last)
#define is_first(m) ((m)->len != 0 && (m)->first)

int N = 4095;      // packet size (variable)
  
  int i, nw;
  int16 error;
  int status;
  int16 cnt;
  static int32 bytes_still_to_write;
  static int state = DONE;
//static int old_state = -1;
  static int c = 0;

  static _AMSW_msg SND, *pSND = &SND;
  static _AMSW_msg RCV, *pRCV = &RCV;

  if (action != CONTINUE) state = action;

  switch (state) {

//~--

    case DONE:

      break;

//~--

    case START:
      c = 0;

      pSND->cha = TX;
      pRCV->cha = RX;

      status = AMSW_SND_RCV(INITIALIZE, pSND, pRCV, &error);
      if (status == DONE && error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      pSND->first = TRUE;
      pSND->len   =0;
      
      if (JINF_PATH) pSND->dat[pSND->len++] = JINF_PATH;
      for (i=0; i<5; i++) {
        if (AMSW_PATH[i]) pSND->dat[pSND->len++] = AMSW_PATH[i];
        else              break;
      }

      if (pRequest->DataType & 0x001F0000) {
        if ((pRequest->DataType & 0x001F0000) == 0x001F0000) {
          pSND->dat[pSND->len++] = unpack32(pRequest->DataType, 0xFFFF0000);
          pSND->dat[pSND->len++] = unpack32(pRequest->DataType, 0x0000FFFF);
        }
        else {
          pReply->Error = 0x0010;
          state = DONE;
          break;
        }
      }
      else {
        pSND->dat[pSND->len++] = 
          unpack32(pRequest->DataType, 0x0000FFFF) |
                  (pRequest->BlockType & 0x0001 ? 0x40 : 0x00) | 0x2E00;
      }

      bytes_still_to_write = pRequest->DataCount;

      if (bytes_still_to_write && pRequest->Data.p16 == 0) {
        pReply->Error = 0x0011;
        state = DONE;
        break;
      }

      if (bytes_still_to_write) {
        cnt = MIN(2*(N-pSND->len), bytes_still_to_write);
        memcpy((int8*)&pSND->dat[pSND->len],
               (int8*)&pRequest->Data.p16[0], 
                cnt);
        pSND->len = pSND->len + cnt / 2;
        bytes_still_to_write = bytes_still_to_write - cnt;
        if (bytes_still_to_write) pSND->last = FALSE;
        else                      pSND->last = TRUE;
      }
      else {
        pSND->last = TRUE;
      }
      status = AMSW_SND_RCV(START, pSND, pRCV, &error);
      state = WRITING;
      break;

//~--

    case WRITING:

      status = AMSW_SND_RCV(CONTINUE, pSND, pRCV, &error);

      if (status != DONE) {
        break;
      }

      if (error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      if (bytes_still_to_write) {
        if (!is_NXT(pRCV)) {
          pReply->Error = 0x0012;
          state = DONE;
          break;
        }
        pSND->first = FALSE;
        cnt = MIN(2*N, bytes_still_to_write);
        memcpy((int8*)&pSND->dat[0],
               (int8*)&pRequest->Data.p16[(pRequest->DataCount-bytes_still_to_write)/2], 
                cnt);
        bytes_still_to_write = bytes_still_to_write - cnt;
        pSND->len = cnt / 2;
        if (bytes_still_to_write) pSND->last = FALSE;
        else                      pSND->last = TRUE;
        status = AMSW_SND_RCV(START, pSND, pRCV, &error);
        break;
      }

      if (is_END(pRCV)) {
        pReply->Error = 0;
        state = DONE;
        break;
      }

      if (pRequest->BlockType & 0x0001) {
        pReply->Error = 0x0013;
        state = DONE;
        break;
      }

      if (!is_first(pRCV)) {
        pReply->Error = 0x0014;
        state = DONE;
        break;
      }

      if (!pReply->Data.p16) {
        pReply->Error = 0x0015;
        state = DONE;
        break;
      }

      nw = pRCV->len;
//    if (pRCV->last && pRCV->len) nw = nw - 1;    // CRC, welcome back

      if (pReply->DataCount + 2 * nw > pReply->BufferSize) {
        pReply->Error = 0x0016;
        state = DONE;
        break;
      }

      memcpy((int8*)&pReply->Data.p16[pReply->DataCount/2],
             (int8*)&pRCV->dat[0], 2*nw);
      pReply->DataCount = pReply->DataCount + 2 * nw;

      if (is_last(pRCV)) {
        pReply->Error = 0;
        state = DONE;
      }
      else {
        pSND->first = FALSE;
        pSND->last  = FALSE;
        pSND->len = 0;
        status = AMSW_SND_RCV(START, pSND, pRCV, &error);
        state = READING;
      }
      break;

//~--

    case READING:

      status = AMSW_SND_RCV(CONTINUE, pSND, pRCV, &error);

      if (status != DONE) {
        break;
      }

      if (error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      nw = pRCV->len;
//    if (pRCV->last && pRCV->len) nw = nw - 1;    // CRC, welcome back

      if (pReply->DataCount + 2 * nw > pReply->BufferSize) {
        pReply->Error = 0x0017;
        state = DONE;
        break;
      }

      memcpy((int8*)&pReply->Data.p16[pReply->DataCount/2],
             (int8*)&pRCV->dat[0], 2*nw);
      pReply->DataCount = pReply->DataCount + 2 * nw;

      if (!is_first(pRCV) && !is_last(pRCV)) {
        status = AMSW_SND_RCV(START, pSND, pRCV, &error);
        break;
      }

      if (is_last(pRCV)) {
        pReply->Error = 0;
        state = DONE;
        break;
      }

      pReply->Error = 0x0018;
      state = DONE;
      break;

//~--

  } // end-of-switch

//~--

//if (state != old_state) {
//  if (P > 3) printf("%7d STAT: state: %d->%d\n", c, old_state, state);
//  old_state = state;
//}

  c = c + 1;
  return state;
}

//~============================================================================
//
//            PCIAMSW BOARD LIBRARY
//
//~============================================================================

#define AMSW_MEM_SIZE 256 * 1024  // 256 Kbytes

#define TX__run       0x00000001
#define TX__BC1       0x80000000
#define TX__BC2       0x40000000
#define TX__wc        0x0FFF0000

#define RX__BC1       0x80000000
#define RX__BC2       0x40000000
#define RX__wc        0x0FFF0000
#define RX__done      0x00000010
#define RX__overrun   0x00000020
#define RX__run       0x00000040
#define RX__error     0x00000100      // The ORing of bit5 to bit10

static int fd[4] = { -1, -1, -1, -1};

//~============================================================================

bool_L initialize_PCIAMSW(_amsw_cha *cha) {

  bool_L debug = FALSE;
  char *names[4] = {"/dev/amsw0", "/dev/amsw1", "/dev/amsw2", "/dev/amsw3"};
  int n = cha->card % 4;
  static int32 *reg = NULL, *mem = NULL;
  
  if (fd[n] < 0) {
    fd[n] = open(names[n], O_RDWR);
    if (debug) printf("initialize_PCIAMSW: fd[%d] = %d\n", n, fd[n]);
    if (fd[n] < 0) return FALSE;

    mem = mmap(0, AMSW_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd[n], 0xBA000000);
    if (debug) printf("initialize_PCIAMSW: mem = %08X\n", (int32)mem);
    if (mem <= 0) return FALSE;

    reg = mmap(0, AMSW_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd[n], 0xBA100000);
    if (debug) printf("initialize_PCIAMSW: reg = %08X\n", (int32)reg);
    if (reg <= 0) return FALSE;
  
    cha->amswmem = mem;
    cha->amswreg = reg;
  }  

  cha->ISR    = reg + 16;
  cha->TX_CSR = reg + (2 * cha->chan + 0);
  cha->RX_CSR = reg + (2 * cha->chan + 1);
  cha->TX_mem = mem + (2048 * cha->chan +     0);
  cha->RX_mem = mem + (2048 * cha->chan + 32768);

  if (debug) printf("initialize_PCIAMSW: cha->ISR    = %08X\n", (int32)cha->ISR);
  if (debug) printf("initialize_PCIAMSW: cha->TX_CSR = %08X\n", (int32)cha->TX_CSR);
  if (debug) printf("initialize_PCIAMSW: cha->RX_CSR = %08X\n", (int32)cha->RX_CSR);
  if (debug) printf("initialize_PCIAMSW: cha->TX_mem = %08X\n", (int32)cha->TX_mem);
  if (debug) printf("initialize_PCIAMSW: cha->RX_mem = %08X\n", (int32)cha->RX_mem);
  
#if 0  //~-- misterious exercise, but otherwise it was not working, now fixed?
{
	AMSW_DATA d;
	int status, size = 33;
  int8 buff[4096];
	d.pci_off = 0x00020000 + cha->chan * 0x00002000;
	d.data = (int32) buff;
	d.size = size;
	status = read(fd[n], (void *)&d, size);
  if (status < 0) return FALSE;
}
#endif
  
  return TRUE;
}

//~============================================================================

void start_amsw_RX(_amsw_msg* msg) {

  bool_L debug = FALSE;
  vint32 ISR;   // must be vint32, not int32 (compiler optimization problems)

//~-- read Interrupt Status Register to clear all 4 RX__DONE bits
//~-- note, that reading RX_CSR does not clear RX__DONE bit.
//~---Thank you, Cai & Lin.

  if (debug) printf("start_amsw_RX: RX_CSR = %08X\n", *msg->cha->RX_CSR);
  ISR = *msg->cha->ISR;
  if (debug) printf("start_amsw_RX: ISR    = %08X\n", ISR);
  if (debug) printf("start_amsw_RX: RX_CSR = %08X\n", *msg->cha->RX_CSR);
  if (debug) ISR = *msg->cha->ISR;
  if (debug) printf("start_amsw_RX: ISR    = %08X\n", ISR);
}

//~----------------------------------------------------------------------------

void stop_amsw_RX(_amsw_msg* msg) {
}

//~----------------------------------------------------------------------------

void start_amsw_TX(_amsw_msg* msg) {

  bool_L debug = FALSE;
  int i, j;
  vint32 TX_CSR;
  int n = msg->cha->card % 4;

  msg->err = 0x0000;
  
  if (fd[n] < 0) {
    msg->err = 0x0001;
    return;
  }

  if (debug) printf("start_amsw_TX: RX_CSR = %08X\n", *msg->cha->RX_CSR);

  TX_CSR = 0;
  pack32((int32*)&TX_CSR, TX__wc,  msg->len);
  pack32((int32*)&TX_CSR, TX__BC1, msg->first);
  pack32((int32*)&TX_CSR, TX__BC2, msg->last);
  pack32((int32*)&TX_CSR, TX__run, TX__run);
  if (debug) printf("start_amsw_TX: TX_CSR = %08X\n", TX_CSR);

  for (i=0, j=0; i<msg->len; i++) {
    msg->cha->TX_mem[i]  = (int32)msg->dat[j++] << 16;
    msg->cha->TX_mem[i] |= (int32)msg->dat[j++];
  }

  *msg->cha->TX_CSR = TX_CSR;
  if (debug) printf("start_amsw_TX: RX_CSR = %08X\n", *msg->cha->RX_CSR);
}

//~----------------------------------------------------------------------------

void stop_amsw_TX(_amsw_msg* msg) {
}

//~----------------------------------------------------------------------------

bool_L amsw_TX_done(_amsw_msg* msg) {

  return TRUE;
}

//~----------------------------------------------------------------------------

bool_L amsw_RX_done(_amsw_msg* msg) {

  bool_L debug = FALSE;
  static int16 CRC = 0xFFFF;
  int16 g = 0x1021;
  bool_L error = FALSE;
  int i;

  msg->err = 0x0000;
  
  msg->sta = *msg->cha->RX_CSR;
  if (debug) printf("amsw_RX_done : *msg->cha->RX_CSR = %08X\n", *msg->cha->RX_CSR);

  msg->len = unpack32(msg->sta, RX__wc);
  pack32(&msg->sta, RX__wc, 0);
  
  msg->first = (msg->sta & RX__BC1) ? TRUE : FALSE;
  msg->last  = (msg->sta & RX__BC2) ? TRUE : FALSE;

  if (msg->sta & RX__done) {
    error = (msg->sta & (RX__error | RX__run | RX__overrun)) ? TRUE : FALSE;
    if (error) msg->err = 0x0001;
    if (P > 3 || (P > 2 && error)) {
      char *bits[32] = {
      "BC1",       "BC2",        "20000000", "10000000",
      "08000000",  "04000000",   "02000000", "01000000",
      "00800000",  "00400000",   "00200000", "00100000",
      "00080000",  "00040000",   "00020000", "00010000",
      "00008000",  "BC2miss",    "BC1miss",  "Parity",
      "FCTalone",  "ESCalone",   "LenErr",   "Error",
      "00000080",  "Run",        "Overrun",  "Done",
      "00000008",  "00000004",   "00000002", "00000001"};
                        
      printf("       RX: %s len=%-4d sta=%08X: ", 
             error ? "*** ERROR-1 ***" : "", msg->len, msg->sta);
      for (i=0; i<32; i++) if (msg->sta & (0x80000000>>i)) printf("%s ", bits[i]);
      printf("\n");
    }

    if (error) return TRUE;

    if (msg->first) CRC = 0xFFFF;
    
    for (i=0; i<msg->len; i++) {
      int j;
      int16 d = msg->cha->RX_mem[i/2] >> (i % 2 ? 0 : 16);
      msg->dat[i] = d;
      for (j=0; j<16; j++) {
        if ((CRC ^ d) & 0x8000) CRC = (CRC << 1) ^ g;
        else                    CRC = (CRC << 1);
        d <<= 1;
      }
    }
    if (debug) printf("amsw_RX_done : *msg->cha->RX_CSR = %08X\n", *msg->cha->RX_CSR);
    if (msg->len && msg->last) {
      error = CRC ? TRUE : FALSE;
      if (error) msg->err = 0x0003;
      if (P > 3 || (P > 2 && error)) {
        printf("       RX: %s CRC=%04X: (must be 00000)\n",
               error ? "*** ERROR-3 ***" : "", CRC);
      }
    }
    else {
      if (P > 3) {
        if (!msg->len)  printf("       RX:  no CRC for zero-length message\n");
        if (!msg->last) printf("       RX:  no CRC for not last message\n");
      }
    }
    if (debug) printf("amsw_RX_done : CRC = %04X\n", CRC);
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//~============================================================================

void print_amsw_msg(int c, char *txt1, char *txt2, _amsw_msg *msg) {

  int i, BC1, BC2;
  char* t[2][4] = {{"NXT", "ERR", "ABO", "END"},
                   {" I ", " L ", " F ", "F&L"}};

  if (txt2) {
    printf("%7d %s%d %s", c, txt1, msg->cha->chan, txt2);
  }
  else {
    BC1 = msg->first ? 1 : 0;
    BC2 = msg->last  ? 1 : 0;
    printf("%7d %s%d %s l=%d ", c, txt1, msg->cha->chan, 
           t[msg->len ? 1 : 0][2*BC1+BC2], msg->len);
    for (i=0; i<msg->len; i++) {
      if (i==0) printf("d =");
      printf(" %04X", msg->dat[i]);
    }
  }

  printf("\n");
}

//~============================================================================

int16 amsw_SND_RCV(int action, _amsw_msg *SND, _amsw_msg *RCV, int16 *error) {

  static _amsw_cha cha, *pcha = &cha;
  static int state = DONE;
//static int old_state = -1;
  static int c = 0;
  bool_L Timeout = timer(0, EXPIRED, timeout);

  if (action != CONTINUE) state = action;

  switch (state) {

//~--

    case DONE:
    break;

//~--

    case INITIALIZE:
      SND->cha = pcha;
      RCV->cha = pcha;
      pcha->card = 0;
      pcha->chan = TX;
      *error = 0;
      if (!initialize_PCIAMSW(pcha)) *error = 0x0004;
      state = DONE;
    break;

//~--

    case START:
      c = 0;

      start_amsw_RX(RCV);

      if (P > 2) print_amsw_msg(c, "JMDC TX:", NULL, SND);
      start_amsw_TX(SND);
      timer(0, START);
      state = TXING;
    break;

//~--

    case TXING:
      if (amsw_TX_done(SND)) {
        timer(0, START);
        state = RXING;
      }
      else {
        if (Timeout) {
          stop_amsw_TX(SND);
          stop_amsw_RX(RCV);
          if (P > 2) print_amsw_msg(c, "JMDC TX:", "Timeout", SND);
          *error = 0x0005;
          state = DONE;
        }
      }
    break;

//~--

    case RXING:
      if (amsw_RX_done(RCV)) {
        if (RCV->err) {
          char b[80];
          sprintf(b, "Error 0x%04X", RCV->err);
          if (P > 2) print_amsw_msg(c, "JMDC RX:", b, RCV);
          if (P > 2) print_amsw_msg(c, "JMDC RX:", NULL, RCV);
          *error = RCV->err;
          state = DONE;
        }
        else {
          if (P > 2) print_amsw_msg(c, "JMDC RX:", NULL, RCV);
          *error = 0;
          state = DONE;
        }
      }
      else {
        if (Timeout) {
          stop_amsw_RX(RCV);
          if (P > 2) print_amsw_msg(c, "JMDC RX:", "Timeout", RCV);
          *error = 0x0006;
          state = DONE;
        }
      }
    break;

//~--

    case STOP:
      stop_amsw_TX(SND);
      if (P > 3) print_amsw_msg(c, "JMDC TX:", "Abort", SND);
      stop_amsw_RX(RCV);
      if (P > 3) print_amsw_msg(c, "JMDC RX:", "Abort", RCV);
      *error = 0;
      state = DONE;
    break;

//~--

  } // end-of-switch

//~--

//if (state != old_state) {
//  if (P > 3) printf("%7d JMDC: state: %d->%d\n", c, old_state, state);
//  old_state = state;
//}

  c = c + 1;
  return state;
}

//~============================================================================

int amswStateMachine(int action, pAMSBlock pRequest, pAMSBlock pReply) {

#define is_ERR(m)   ((m)->len == 0 &&  (m)->last && !(m)->first)
#define is_ABO(m)   ((m)->len == 0 && !(m)->last &&  (m)->first)
#define is_NXT(m)   ((m)->len == 0 && !(m)->last && !(m)->first)
#define is_END(m)   ((m)->len == 0 &&  (m)->last &&  (m)->first)

#define is_last(m)  ((m)->len != 0 && (m)->last)
#define is_first(m) ((m)->len != 0 && (m)->first)

  int N = 4095;      // packet size (variable)
//  int N = 4;      // packet size (variable)
  
  int i, nw;
  int16 error;
  int status;
  int16 cnt;
  static int32 bytes_still_to_write;
  static int state = DONE;
//static int old_state = -1;
  static int c = 0;

  static _amsw_msg SND, *pSND = &SND;
  static _amsw_msg RCV, *pRCV = &RCV;

  if (action != CONTINUE) state = action;

  switch (state) {

//~--

    case DONE:

      break;

//~--

    case START:
      c = 0;

      status = amsw_SND_RCV(INITIALIZE, pSND, pRCV, &error);
      if (status == DONE && error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      pSND->first = TRUE;
      pSND->len   =0;
      
      if (JINF_PATH) pSND->dat[pSND->len++] = JINF_PATH;
      for (i=0; i<5; i++) {
        if (AMSW_PATH[i]) pSND->dat[pSND->len++] = AMSW_PATH[i];
        else              break;
      }

      if (pRequest->DataType & 0x001F0000) {
        if ((pRequest->DataType & 0x001F0000) == 0x001F0000) {
          pSND->dat[pSND->len++] = unpack32(pRequest->DataType, 0xFFFF0000);
          pSND->dat[pSND->len++] = unpack32(pRequest->DataType, 0x0000FFFF);
        }
        else {
          pReply->Error = 0x0010;
          state = DONE;
          break;
        }
      }
      else {
        pSND->dat[pSND->len++] = 
          unpack32(pRequest->DataType, 0x0000FFFF) |
                  (pRequest->BlockType & 0x0001 ? 0x40 : 0x00) | 0x2E00;
      }

      bytes_still_to_write = pRequest->DataCount;

      if (bytes_still_to_write && pRequest->Data.p16 == 0) {
        pReply->Error = 0x0011;
        state = DONE;
        break;
      }

      if (bytes_still_to_write) {
        cnt = MIN(2*(N-pSND->len), bytes_still_to_write);
        memcpy((int8*)&pSND->dat[pSND->len],
               (int8*)&pRequest->Data.p16[0], 
                cnt);
        pSND->len = pSND->len + cnt / 2;
        bytes_still_to_write = bytes_still_to_write - cnt;
        if (bytes_still_to_write) pSND->last = FALSE;
        else                      pSND->last = TRUE;
      }
      else {
        pSND->last = TRUE;
      }
      status = amsw_SND_RCV(START, pSND, pRCV, &error);
      state = WRITING;
      break;

//~--

    case WRITING:

      status = amsw_SND_RCV(CONTINUE, pSND, pRCV, &error);

      if (status != DONE) {
        break;
      }

      if (error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      if (bytes_still_to_write) {
        if (!is_NXT(pRCV)) {
          pReply->Error = 0x0012;
          state = DONE;
          break;
        }
        pSND->first = FALSE;
        cnt = MIN(2*N, bytes_still_to_write);
        memcpy((int8*)&pSND->dat[0],
               (int8*)&pRequest->Data.p16[(pRequest->DataCount-bytes_still_to_write)/2], 
                cnt);
        bytes_still_to_write = bytes_still_to_write - cnt;
        pSND->len = cnt / 2;
        if (bytes_still_to_write) pSND->last = FALSE;
        else                      pSND->last = TRUE;
        status = amsw_SND_RCV(START, pSND, pRCV, &error);
        break;
      }

      if (is_END(pRCV)) {
        pReply->Error = 0;
        state = DONE;
        break;
      }

      if (pRequest->BlockType & 0x0001) {
        pReply->Error = 0x0013;
        state = DONE;
        break;
      }

      if (!is_first(pRCV)) {
        pReply->Error = 0x0014;
        state = DONE;
        break;
      }

      if (!pReply->Data.p16) {
        pReply->Error = 0x0015;
        state = DONE;
        break;
      }

      nw = pRCV->len;
//    if (pRCV->last && pRCV->len) nw = nw - 1;    // CRC, welcome back

      if (pReply->DataCount + 2 * nw > pReply->BufferSize) {
        pReply->Error = 0x0016;
        state = DONE;
        break;
      }

      memcpy((int8*)&pReply->Data.p16[pReply->DataCount/2],
             (int8*)&pRCV->dat[0], 2*nw);
      pReply->DataCount = pReply->DataCount + 2 * nw;

      if (is_last(pRCV)) {
        pReply->Error = 0;
        state = DONE;
      }
      else {
        pSND->first = FALSE;
        pSND->last  = FALSE;
        pSND->len = 0;
        status = amsw_SND_RCV(START, pSND, pRCV, &error);
        state = READING;
      }
      break;

//~--

    case READING:

      status = amsw_SND_RCV(CONTINUE, pSND, pRCV, &error);

      if (status != DONE) {
        break;
      }

      if (error) {
        pReply->Error = error;
        state = DONE;
        break;
      }

      nw = pRCV->len;
//    if (pRCV->last && pRCV->len) nw = nw - 1;    // CRC, welcome back

      if (pReply->DataCount + 2 * nw > pReply->BufferSize) {
        pReply->Error = 0x0017;
        state = DONE;
        break;
      }

      memcpy((int8*)&pReply->Data.p16[pReply->DataCount/2],
             (int8*)&pRCV->dat[0], 2*nw);
      pReply->DataCount = pReply->DataCount + 2 * nw;

      if (!is_first(pRCV) && !is_last(pRCV)) {
        status = amsw_SND_RCV(START, pSND, pRCV, &error);
        break;
      }

      if (is_last(pRCV)) {
        pReply->Error = 0;
        state = DONE;
        break;
      }

      pReply->Error = 0x0018;
      state = DONE;
      break;

//~--

  } // end-of-switch

//~--

//if (state != old_state) {
//  if (P > 3) printf("%7d STAT: state: %d->%d\n", c, old_state, state);
//  old_state = state;
//}

  c = c + 1;
  return state;
}

//~============================================================================

