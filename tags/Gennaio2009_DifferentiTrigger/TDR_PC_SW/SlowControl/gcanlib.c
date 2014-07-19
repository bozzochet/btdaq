// file gcanlib.c
//
// Stolen from S.Xu (gcan) and reworked
// (is used in cab-slave programs etc).
//
// A.Lebedev - Feb 2008 ...
//

#include "gcanlib.h"

static int16 PortBase = 0;

#define LPT_STAT PortBase+1
#define LPT_CTRL PortBase+2
#define EPP_ADDR PortBase+3
#define EPP_DATA PortBase+4

int8 CAN_ADDR[2] = {0x20, 0x24};
int8 CAN_DATA[2] = {0x21, 0x25};
int8 CAN_INCR[2] = {0x22, 0x26};

static int8 rxbuff[0x40000];
static int8 txbuff[0x40000];

static SFSM_CHANNEL ch[4] = 
{
{ SACT_INIT_CHANNEL, 0, 0, {0,}, 0, BUSA, MSG(1), 0, 0, 0, rxbuff, BUSA, MSG(5), 0, 0, 0, txbuff },
{ SACT_INIT_CHANNEL, 0, 0, {0,}, 0, BUSA, MSG(2), 0, 0, 0, rxbuff, BUSA, MSG(6), 0, 0, 0, txbuff },
{ SACT_INIT_CHANNEL, 0, 0, {0,}, 0, BUSA, MSG(3), 0, 0, 0, rxbuff, BUSA, MSG(7), 0, 0, 0, txbuff },
{ SACT_INIT_CHANNEL, 0, 0, {0,}, 0, BUSA, MSG(4), 0, 0, 0, rxbuff, BUSA, MSG(8), 0, 0, 0, txbuff }
};

//~----------------------------------------------------------------------------

int test_epp(void) {

  int w, r, n;
        
  if (ioperm(PortBase, 8, 1)) return(-1);

  outb(0x00, LPT_CTRL);
  usleep(5000);
  outb(0x04, LPT_CTRL);

  n = 0;
  for (w=0x00; w<=0xFF; w++) {
    outb(w, EPP_ADDR);
    r = inb(EPP_ADDR);
    if (r != w) n++;
  }
    
  if (n) return(-1);

  return(0);
}

//~----------------------------------------------------------------------------

void open_epp(void) {

  struct timeval t;
    
  gettimeofday(&t, NULL);
  srand(t.tv_usec);
  atexit(close_epp);
        
again:
  switch (PortBase) {
    default   : exit(1);    
    case 0x000: PortBase = 0x278; break;
    case 0x278: PortBase = 0x378; break;
    case 0x378: PortBase = 0x3BC; break;
  }

  printf("Opening EPPBOX at 0x%03X ...", PortBase);

  if (test_epp()) {
    printf("KO!\n");
    goto again;
  }
  else {
    printf("OK!\n");
    return;
  }
}

//~----------------------------------------------------------------------------

void close_epp(void) {

  outb(0x00, LPT_CTRL);

  if (ioperm(PortBase, 8, 0)) {
    printf("ioperm 0");
    abort();
  }
}

//~----------------------------------------------------------------------------

int8 rcan(CANBUS bus, int8 reg) {

  outb(CAN_ADDR[bus], EPP_ADDR);
  outb(reg,           EPP_DATA);
  outb(CAN_DATA[bus], EPP_ADDR);    
  return inb(EPP_DATA);
}

//~----------------------------------------------------------------------------

void wcan(CANBUS bus, int8 reg, int8 val) {

  outb(CAN_ADDR[bus], EPP_ADDR);
  outb(reg,           EPP_DATA);
  outb(CAN_DATA[bus], EPP_ADDR);
  outb(val,           EPP_DATA);
}

//~----------------------------------------------------------------------------

void init_can(CANCFG *p) {

  CANBUS bus = p->bus;
  int i;

  wcan(bus, MCPU,0x40);
  wcan(bus, CTL, 0x41);
  wcan(bus, STA, 0x00);
  wcan(bus, BUS, 0x48);
  wcan(bus, BT0, 0x00);
  wcan(bus, BT1, 0x14);
  wcan(bus, P2C, 0xFF);
  wcan(bus, P2O, 0x00);

  wcan(bus, SGM0,  0xFF);
  wcan(bus, SGM1,  0xFF);

  wcan(bus, EGM0,  p->egm[0]);
  wcan(bus, EGM1,  p->egm[1]);
  wcan(bus, EGM2,  p->egm[2]);
  wcan(bus, EGM3,  p->egm[3]);

  wcan(bus, M15M0, 0xFF);
  wcan(bus, M15M1, 0xFF);
  wcan(bus, M15M2, 0xFF);
  wcan(bus, M15M3, 0xFF);

  for (i=1; i<16; i++) {
    wcan(bus, MSG(i) + MSGCTL0, 0x55);
    wcan(bus, MSG(i) + MSGCTL1, 0x55);

    wcan(bus, MSG(i) + MSGID0,  0x00);
    wcan(bus, MSG(i) + MSGID1,  0x00);
    wcan(bus, MSG(i) + MSGID2,  0x00);
    wcan(bus, MSG(i) + MSGID3,  0x00);
    wcan(bus, MSG(i) + MSGCFG,  0x00);
        
    wcan(bus, MSG(i) + MSGDAT0, 0x00);
    wcan(bus, MSG(i) + MSGDAT1, 0x00);
    wcan(bus, MSG(i) + MSGDAT2, 0x00);
    wcan(bus, MSG(i) + MSGDAT3, 0x00);
    wcan(bus, MSG(i) + MSGDAT4, 0x00);
    wcan(bus, MSG(i) + MSGDAT5, 0x00);
    wcan(bus, MSG(i) + MSGDAT6, 0x00);
    wcan(bus, MSG(i) + MSGDAT7, 0x00);
  }

  wcan(bus, CTL, 0x00);
}

//~----------------------------------------------------------------------------

void dump_can(CANBUS bus) {

  int i;

  printf("\r[ADDR] +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F");

  for (i=0; i<256; i++) {
    if (!(i % 16))  printf("\n 0x%02X ", i);
    printf(" %02X", rcan(bus, i));
  }

  printf("\n");
}

//~----------------------------------------------------------------------------

void dump_buff(int8 *buff, int nbytes) {

  int i;

  for (i=0; i<nbytes; i++) {
    printf("%02X ",buff[i]);
    if ((i %  8)  == 7) printf("  ");
    if ((i % 16) == 15) printf("\n");
  }

  if (nbytes % 16)  printf("\n");
}

//~----------------------------------------------------------------------------

void dump_packet(CANPKT *pkt) {

  printf("\nPKT CID: %02X %02X %02X %02X", 
         pkt->cid[0], pkt->cid[1], pkt->cid[2], pkt->cid[3]);

  switch(pkt->cid[0] & 0x60) {
    case 0x00:  printf("\t\t%s\t", (pkt->dlc) ? "I" : "NEXT" );  break;
    case 0x20:  printf("\t\t%s\t", (pkt->dlc) ? "L" : "ERROR" ); break;
    case 0x40:  printf("\t\t%s\t", (pkt->dlc) ? "F" : "ABORT" ); break;
    case 0x60:  printf("\t\t%s\t", (pkt->dlc) ? "S" : "END" );   break;
  }

  switch(pkt->cid[0] & 0x18) {
    case 0x00:  printf("REQUEST\tREAD  ");  break;
    case 0x08:  printf("REQUEST\tWRITE ");  break;
    case 0x10:  printf("REPLY  \tREAD  ");  break;
    case 0x18:  printf("REPLY  \tWRITE ");  break;
  }

  printf("\nPKT DLC: %02d", pkt->dlc);
  printf("\nPKT DAT: "); dump_buff(pkt->dat, pkt->dlc);
}

//~----------------------------------------------------------------------------

void dump_block(CANBLK *blk) {

  printf("\nBLK CID: %02X %02X %02X %02X",
         blk->cid[0], blk->cid[1], blk->cid[2], blk->cid[3]);

  switch(blk->cid[0] & 0x60) {
    case 0x00:  printf("\t\t%s\t", (blk->len) ? "I" : "NEXT" );  break;
    case 0x20:  printf("\t\t%s\t", (blk->len) ? "L" : "ERROR" ); break;
    case 0x40:  printf("\t\t%s\t", (blk->len) ? "F" : "ABORT" ); break;
    case 0x60:  printf("\t\t%s\t", (blk->len) ? "S" : "END" );   break;
  }

  switch(blk->cid[0] & 0x18) {
    case 0x00:  printf("REQUEST\tREAD  ");  break;
    case 0x08:  printf("REQUEST\tWRITE ");  break;
    case 0x10:  printf("REPLY  \tREAD  ");  break;
    case 0x18:  printf("REPLY  \tWRITE ");  break;
  }

  printf("\t %s", (blk->cid[0]&1) ? "NEWBORN" : "LOGICAL");

  printf("\nBLK LEN: %02d", blk->len);
  printf("\nBLK DAT: ");

  if (blk->len > 8) printf("\n");

  dump_buff(blk->dat, blk->len);
  printf("\n\n");
}

//~============================================================================

inline void slave_send_s(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTS | (p->rxmsb & 0x0F);
  p->txdlc = p->txlen;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_f(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTF | (p->rxmsb & 0x0F);
  p->txdlc = 8;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_i(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTI | (p->rxmsb & 0x0F);
  p->txdlc = 8;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_l(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTL | (p->rxmsb & 0x0F);
  p->txdlc = p->txlen;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_abort(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTABORT | (p->rxmsb & 0x0F);
  p->txdlc = 0;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_next(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTNEXT | (p->rxmsb & 0x0F);
  p->txdlc = 0;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_error(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTERROR | (p->rxmsb & 0x0F);
  p->txdlc = 0;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_end(SFSM_CHANNEL *p) {

  p->txmsb = REPPKTEND | (p->rxmsb & 0x0F);
  p->txdlc = 0;
  p->action = SACT_SEND_REPPKT;
}

//~----------------------------------------------------------------------------

inline void slave_send_nothing(SFSM_CHANNEL *p) {

  p->action = SACT_INIT_CHANNEL; 
}

//~----------------------------------------------------------------------------

void sfsm(int8 master, int8 slave, sfsm_user_call *user_func) {

  SFSM_CHANNEL *p;
  int8 i;

  p = & ch[master];

  switch (p->action) {

//~---

    default:

//~---

    case SACT_INIT_CHANNEL:    
      p->mark    = 0;
      p->error   = 0;
      p->history = SHIS_RECV_REQPKTEND;
            
      p->rxmsb   = 0;
      p->rxdlc   = 0;
      p->rxlen   = 0;

      p->txmsb   = 0;
      p->txdlc   = 0;
      p->txlen   = 0;

      wcan(BUSA, p->txbox + MSGCTL0, 0x55);
      wcan(BUSB, p->txbox + MSGCTL0, 0x55);
      wcan(BUSA, p->txbox + MSGCTL1, 0x55);
      wcan(BUSB, p->txbox + MSGCTL1, 0x55);
      wcan(BUSA, p->txbox + MSGID0,  0x00);
      wcan(BUSB, p->txbox + MSGID0,  0x00);
      wcan(BUSA, p->txbox + MSGID1,  slave);
      wcan(BUSB, p->txbox + MSGID1,  slave);
      wcan(BUSA, p->txbox + MSGID2,  master);
      wcan(BUSB, p->txbox + MSGID2,  master);
      wcan(BUSA, p->txbox + MSGID3,  0x00);
      wcan(BUSB, p->txbox + MSGID3,  0x00);
      wcan(BUSA, p->txbox + MSGCFG,  0x0C);
      wcan(BUSB, p->txbox + MSGCFG,  0x0C);
            
      wcan(BUSA, p->rxbox + MSGCTL0, 0x55);
      wcan(BUSB, p->rxbox + MSGCTL0, 0x55);
      wcan(BUSA, p->rxbox + MSGCTL1, 0x55);
      wcan(BUSB, p->rxbox + MSGCTL1, 0x55);
      wcan(BUSA, p->rxbox + MSGID0,  0x00);
      wcan(BUSB, p->rxbox + MSGID0,  0x04);
      wcan(BUSA, p->rxbox + MSGID1,  master);
      wcan(BUSB, p->rxbox + MSGID1,  master);
      wcan(BUSA, p->rxbox + MSGID2,  slave);
      wcan(BUSB, p->rxbox + MSGID2,  slave);
      wcan(BUSA, p->rxbox + MSGID3,  0x00);
      wcan(BUSB, p->rxbox + MSGID3,  0x00);
      wcan(BUSA, p->rxbox + MSGCFG,  0x04);
      wcan(BUSB, p->rxbox + MSGCFG,  0x04);
            
      wcan(BUSA, p->rxbox + MSGCTL0, 0x95);
      wcan(BUSB, p->rxbox + MSGCTL0, 0x95);

      p->action = SACT_WAIT_REQPKT;
    return;

//~---
    case SACT_WAIT_REQPKT:
      if (CHKNEWDAT(rcan(BUSA, p->rxbox + MSGCTL1)))       p->rxbus = BUSA;
      else if (CHKNEWDAT(rcan(BUSB, p->rxbox + MSGCTL1)))  p->rxbus = BUSB;
      else return;

      wcan(p->rxbus, p->rxbox + MSGCTL0, 0x55);
      p->rxmsb = rcan(p->rxbus, p->rxbox + MSGID0);
      p->rxdlc = (rcan(p->rxbus, p->rxbox + MSGCFG) >> 4) & 0x0F;
      p->counter.rx++;           

      if (p->rxdlc) {              
        switch (p->rxmsb & 0x60) {
          case 0x00: p->action = SACT_RECV_REQPKTI; return;
          case 0x20: p->action = SACT_RECV_REQPKTL; return;
          case 0x40: p->action = SACT_RECV_REQPKTF; return;
          case 0x60: p->action = SACT_RECV_REQPKTS; return;
        }
      }
      else {
        switch (p->rxmsb & 0x60) {
          case 0x00: p->action = SACT_RECV_REQPKTNEXT;  return;
          case 0x20: p->action = SACT_RECV_REQPKTERROR; return;
          case 0x40: p->action = SACT_RECV_REQPKTABORT; return;
          case 0x60: p->action = SACT_RECV_REQPKTEND;   return;
        }
      }
    return;

//~---

    case SACT_RECV_REQPKTS:
      p->mark = p->rxmsb & 0x0F;
      p->history = SHIS_RECV_REQPKTS;

      p->rxdat = rxbuff;
            
      for (i=0; i<p->rxdlc; i++) {
        p->rxdat[i] = rcan(p->rxbus, p->rxbox + MSGDAT0 + i);
      }

      p->rxlen = p->rxdlc;
      p->rxdat += p->rxdlc;

      if (CHKMSGLST(rcan(p->rxbus, p->rxbox + MSGCTL1))) {
        p->error = SERR_PKTLOST;
        p->counter.pkt_lost_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }

      p->action = SACT_PROC_CENTER;
    return;

//~---

    case SACT_RECV_REQPKTF:
      p->mark = p->rxmsb & 0x0F;
      p->history = SHIS_RECV_REQPKTF;
            
      p->rxdat = rxbuff;
            
      for (i=0; i<p->rxdlc; i++) {
        p->rxdat[i] = rcan(p->rxbus, p->rxbox + MSGDAT0 + i);
      }
      
      p->rxlen = p->rxdlc;
      p->rxdat += p->rxdlc;

      if (CHKMSGLST(rcan(p->rxbus, p->rxbox + MSGCTL1))) {
        p->error = SERR_PKTLOST;
        p->counter.pkt_lost_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }

      p->txmsb = REPPKTNEXT | (p->rxmsb & 0x0F);
      p->txdlc = 0;

      p->action = SACT_SEND_REPPKT;
    return;

//~---

    case SACT_RECV_REQPKTI:
      if ((p->history != SHIS_RECV_REQPKTF) && 
          (p->history != SHIS_RECV_REQPKTI)) {
        p->error = SERR_PKTSEQU;
        p->counter.pkt_sequ_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }
      else {
        p->history = SHIS_RECV_REQPKTI;
      }
       
      if ((p->rxmsb & 0x0F) != p->mark) {
        p->error = SERR_PKTTYPE;
        p->counter.pkt_type_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }

      for (i=0; i<p->rxdlc; i++) {
        p->rxdat[i] = rcan(p->rxbus, p->rxbox + MSGDAT0 + i);
      }
            
      p->rxlen += p->rxdlc;
      p->rxdat += p->rxdlc;

      if (CHKMSGLST(rcan(p->rxbus, p->rxbox + MSGCTL1))) {
        p->error  = SERR_PKTLOST;
        p->counter.pkt_lost_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }

      p->txmsb = REPPKTNEXT | (p->rxmsb & 0x0F);
      p->txdlc = 0;

      p->action = SACT_SEND_REPPKT;
    return;

//~---

    case SACT_RECV_REQPKTL:
      if ((p->history != SHIS_RECV_REQPKTF) &&
          (p->history != SHIS_RECV_REQPKTI)) {
        p->error = SERR_PKTSEQU;
        p->counter.pkt_sequ_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }
      else {
        p->history = SHIS_RECV_REQPKTL;
      }
            
      if ((p->rxmsb & 0x0F) != p->mark) {
        p->error = SERR_PKTTYPE;
        p->counter.pkt_type_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }

      for (i=0; i<p->rxdlc; i++) {
        p->rxdat[i] = rcan(p->rxbus, p->rxbox + MSGDAT0 + i);
      }

      p->rxlen += p->rxdlc;
      p->rxdat += p->rxdlc;
            
      if (CHKMSGLST(rcan(p->rxbus, p->rxbox + MSGCTL1))) {
        p->error = SERR_PKTLOST;
        p->counter.pkt_lost_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }

      p->action = SACT_PROC_CENTER;
    return;

//~---

    case SACT_RECV_REQPKTNEXT:
      if ((p->history != SHIS_RECV_REQPKTS) &&
          (p->history != SHIS_RECV_REQPKTL) &&
          (p->history != SHIS_RECV_REQPKTNEXT)) {
        p->error = SERR_PKTSEQU;
        p->counter.pkt_sequ_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }
      else {
        p->history = SHIS_RECV_REQPKTNEXT;
      }

      if ((p->rxmsb & 0x0F) != p->mark) {
        p->error = SERR_PKTTYPE;
        p->counter.pkt_type_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }
            
      if (CHKMSGLST(rcan(p->rxbus, p->rxbox + MSGCTL1))) {
        p->error = SERR_PKTLOST;
        p->counter.pkt_lost_err++;
        p->action = SACT_PROC_CENTER;
        return;
      }
                        
      if (p->txlen == 0) {
        p->txmsb = REPPKTNEXT | (p->rxmsb & 0x0F);
        p->txdlc = 0;
        p->action = SACT_SEND_REPPKT;
        return;
      }
            
      if (p->txlen > 8) {
        p->txmsb = REPPKTI | (p->rxmsb & 0x0F);
        p->txdlc = 8;
      }
      else {
        p->txmsb = REPPKTL | (p->rxmsb & 0x0F);
        p->txdlc = p->txlen;
      }

      p->action = SACT_SEND_REPPKT;
    return;
            
//~---

    case SACT_RECV_REQPKTERROR:
    case SACT_RECV_REQPKTABORT:
    case SACT_RECV_REQPKTEND:
      p->history = SHIS_RECV_REQPKTEND;
            
      if (CHKMSGLST(rcan(p->rxbus, p->rxbox + MSGCTL1))) {
        p->error = SERR_PKTLOST;
        p->action = SACT_PROC_CENTER;
        return;
      }

      p->txmsb = REPPKTABORT | (p->rxmsb & 0x0F);
      p->txdlc = 0;

      p->action = SACT_SEND_REPPKT;
    return;

//~---

    case SACT_PROC_CENTER:
      if (p->error) {
        p->history = SHIS_RECV_REQPKTEND;
        p->txmsb = REPPKTABORT | (p->rxmsb & 0x0F);
        p->txdlc = 0;
        p->action = SACT_SEND_REPPKT;
        return;
      }
            
      if (user_func == NULL) {
        memcpy(txbuff, rxbuff+1, p->rxlen-1);
        p->txdat = txbuff;
        p->txlen = p->rxlen - 1;
      }
      else {
        p->txlen = user_func(txbuff, rxbuff, p->rxlen);
        if (p->txlen == 0) {
          p->txmsb = txbuff[0] | (p->rxmsb & 0x0F);
          p->txdlc = 0;
          p->action = SACT_SEND_REPPKT;
          return;
        }
        p->txdat = txbuff;
      }

      if (p->txlen > 8) slave_send_f(p);
      else              slave_send_s(p);
    return; 

//~---

    case SACT_SEND_REPPKT:
      wcan(p->rxbus, p->rxbox + MSGCTL1, 0x55);
      wcan(p->rxbus, p->rxbox + MSGCTL0, 0x95);
            
      p->txbus = (p->rxmsb & 0x02) ? BUSB : BUSA;
      wcan(p->txbus, p->txbox + MSGCTL1, 0x55);
      wcan(p->txbus, p->txbox + MSGCTL0, 0x55);

      for (i=0; i<p->txdlc; i++) {
        wcan(p->txbus, p->txbox + MSGDAT0 + i, p->txdat[i]);
      }

      wcan(p->txbus, p->txbox + MSGID0, p->txmsb);
      wcan(p->txbus, p->txbox + MSGCFG, (p->txdlc << 4) | 0x0C);
      wcan(p->txbus, p->txbox + MSGCTL1, 0x66);
      wcan(p->txbus, p->txbox + MSGCTL0, 0x95);

      p->txlen -= p->txdlc;
      p->txdat += p->txdlc;
            
      p->counter.tx++;
      p->action = SACT_WAIT_REQPKT;
    return;
  }
}

//~----------------------------------------------------------------------------

void sfsm_status(int8 slave) {

  int i;

  system("clear");

  for (i=0; i<4; i++) {
    printf("[ECHO from 0x%02X to 0x%02X]:"
            "rx=%d; tx=%d;"
            "sequ_err=%d;"
            "type_err=%d;"
            "lost_err=%d\n",
            i, slave,
            ch[i].counter.rx, ch[i].counter.tx,
            ch[i].counter.pkt_sequ_err,
            ch[i].counter.pkt_type_err,
            ch[i].counter.pkt_lost_err);
  }
}

//~============================================================================
