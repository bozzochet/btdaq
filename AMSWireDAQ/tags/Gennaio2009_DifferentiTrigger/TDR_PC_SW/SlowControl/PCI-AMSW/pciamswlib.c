// file pciamswlib.c
//
// My library for PCI-AMSWire card
// (stolen from Cai and Lin and reworked)
//
// A.Lebedev Feb-2007...
//

#include "pciamswlib.h"
#include "amsw_dev.h"
#include "amsw_lib.h"

int P;

static int fd[4] = { -1, -1, -1, -1};

#define TX_run       0x00000001
#define TX_BC1       0x80000000
#define TX_BC2       0x40000000
#define TX_wc        0x0FFF0000

#define RX_BC1       0x80000000
#define RX_BC2       0x40000000
#define RX_wc        0x0FFF0000
#define RX_done      0x00000010
#define RX_overrun   0x00000020
#define RX_run       0x00000040
#define RX_error     0x00000100      // The ORing of bit5 to bit10

//=============================================================================

bool initialize_PCIAMSW(_amsw_cha *cha) {

  bool debug = FALSE;
  char *names[4] = {"/dev/amsw0", "/dev/amsw1", "/dev/amsw2", "/dev/amsw3"};
  int n = cha->card % 4;
  int32 *reg, *mem;
  
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
  }
  
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

//=============================================================================

void start_amsw_RX(_amsw_msg* msg) {

  bool debug = FALSE;
  vint32 ISR;   // must be vint32, not int32 (compiler optimization problems)

//~-- read Interrupt Status Register to clear all 4 RX_DONE bits
//~-- note, that reading RX_CSR does not clear RX_DONE bit.
//~---Thank you, Cai & Lin.

  if (debug) printf("start_amsw_RX: RX_CSR = %08X\n", *msg->cha->RX_CSR);
  ISR = *msg->cha->ISR;
  if (debug) printf("start_amsw_RX: ISR    = %08X\n", ISR);
  if (debug) printf("start_amsw_RX: RX_CSR = %08X\n", *msg->cha->RX_CSR);
  if (debug) ISR = *msg->cha->ISR;
  if (debug) printf("start_amsw_RX: ISR    = %08X\n", ISR);
}

//-----------------------------------------------------------------------------

void stop_amsw_RX(_amsw_msg* msg) {
}

//-----------------------------------------------------------------------------

void start_amsw_TX(_amsw_msg* msg) {

  bool debug = FALSE;
  int i, j;
  vint32 TX_CSR;
  int n = msg->cha->card % 4;

  msg->err = 0x0000;
  
  if (fd[n] < 0) {
    msg->err = 0x0001;
    return;
  }

  if (debug) printf("start_amsw_TX: RX_CSR = %08X\n", *msg->cha->RX_CSR);

  TX_CSR  = 0;
  pack32((int32*)&TX_CSR, TX_wc,  msg->len);
  pack32((int32*)&TX_CSR, TX_BC1, msg->first);
  pack32((int32*)&TX_CSR, TX_BC2, msg->last);
  pack32((int32*)&TX_CSR, TX_run, TX_run);
  if (debug) printf("start_amsw_TX: TX_CSR = %08X\n", TX_CSR);

  for (i=0, j=0; i<msg->len; i++) {
    msg->cha->TX_mem[i]  = (int32)msg->dat[j++] << 16;
    msg->cha->TX_mem[i] |= (int32)msg->dat[j++];
  }

  *msg->cha->TX_CSR = TX_CSR;
  if (debug) printf("start_amsw_TX: RX_CSR = %08X\n", *msg->cha->RX_CSR);
}

//-----------------------------------------------------------------------------

void stop_amsw_TX(_amsw_msg* msg) {
}

//-----------------------------------------------------------------------------

bool amsw_TX_done(_amsw_msg* msg) {

  return TRUE;
}

//~----------------------------------------------------------------------------

bool amsw_RX_done(_amsw_msg* msg) {

  bool debug = FALSE;
  static int16 CRC = 0xFFFF;
  int16 g = 0x1021;
  bool error = FALSE;
  int i;

  msg->err = 0x0000;
  
  msg->sta = *msg->cha->RX_CSR;
  if (debug) printf("amsw_RX_done : *msg->cha->RX_CSR = %08X\n", *msg->cha->RX_CSR);

  msg->len = unpack32(msg->sta, RX_wc);
  pack32(&msg->sta, RX_wc, 0);
  
  msg->first = (msg->sta & RX_BC1) ? TRUE : FALSE;
  msg->last  = (msg->sta & RX_BC2) ? TRUE : FALSE;

  if (msg->sta & RX_done) {
    error = msg->sta & (RX_error | RX_run | RX_overrun);
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
      error = CRC != 0x0000;
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
  bool Timeout = timer(0, EXPIRED, timeout);

  if (action != CONTINUE) state = action;

  switch (state) {

//---

    case DONE:
    break;

//---

    case INITIALIZE:
      SND->cha = pcha;
      RCV->cha = pcha;
      pcha->card = 0;
      pcha->chan = TX;
      if (!initialize_PCIAMSW(pcha)) {
        *error = 0x0004;
      }
      else {
        *error = 0;
      }
      state = DONE;
    break;

//---

    case START:
      c = 0;

      start_amsw_RX(RCV);

      if (P > 2) print_amsw_msg(c, "JMDC TX:", NULL, SND);
      start_amsw_TX(SND);
      timer(0, START);
      state = TXING;
    break;

//---

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

//---

    case RXING:
      if (amsw_RX_done(RCV)) {
        if (RCV->err) {
          char b[80];
          sprintf(b, "Error 0x%04X", RCV->err);
          print_amsw_msg(c, "JMDC RX:", b, RCV);
          print_amsw_msg(c, "JMDC RX:", NULL, RCV);
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

//---

    case STOP:
      stop_amsw_TX(SND);
      if (P > 3) print_amsw_msg(c, "JMDC TX:", "Abort", SND);
      stop_amsw_RX(RCV);
      if (P > 3) print_amsw_msg(c, "JMDC RX:", "Abort", RCV);
      *error = 0;
      state = DONE;
    break;

//---

  } // end-of-switch

//---

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

//---

    case DONE:

      break;

//---

    case START:
      c = 0;

      pReply->BlockType = 0x0002 | (pRequest->BlockType & 0x0001);
      pReply->DataType  = pRequest->DataType;
      pReply->DataCount = 0;
      pReply->Source    = pRequest->Source;

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
        pSND->dat[pSND->len++] = unpack32(pRequest->DataType, 0x0000FFFF) |
                                         (pRequest->BlockType & 0x0001 ? 0x40 : 0x00);
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

//---

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
      if (pRCV->last && pRCV->len) nw = nw - 1;

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

//---

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
      if (pRCV->last && pRCV->len) nw = nw - 1;

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

//---

  } // end-of-switch

//---

//if (state != old_state) {
//  if (P > 3) printf("%7d STAT: state: %d->%d\n", c, old_state, state);
//  old_state = state;
//}

  c = c + 1;
  return state;
}

//~============================================================================

void to_amsw(int32 RQ_typ, int16  RQ_cnt, int16 *RQ_dat,
             int16 RP_siz, int16 *RP_cnt, int16 *RP_dat, int16 *RP_err) {

  static AMSBlock Request, *pRequest = &Request;
  static AMSBlock Reply,   *pReply   = &Reply;

  pReply->BufferSize = RP_siz * 2;
  pReply->Data.p16   = RP_dat;

  pRequest->BlockType    = unpack32(RQ_typ, 0x00000040);
  pRequest->DataType     = RQ_typ;
  pRequest->DataCount    = RQ_cnt * 2;
  pRequest->Data.p16     = RQ_dat;
  pRequest->Data.p8      = NULL;
  pack16(&pRequest->Source, 0xFF00, TX);

  if (P > 1) print_AMS_data_block("REQUEST:", pRequest);

  amswStateMachine(START, pRequest, pReply);
  while (amswStateMachine(CONTINUE, pRequest, pReply) != DONE);

  if (RP_cnt) *RP_cnt = pReply->DataCount / 2;
  if (RP_err) *RP_err = pReply->Error;

  if (P > 1) print_AMS_data_block("REPLY:  ", pReply);
}

//~============================================================================
