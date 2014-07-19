#ifdef BUILD_FOR_CC

// file ccbt.c
//
// A.Lebedev Dec 2006 - ...
//

#include "ain1.h"

//~---

static _ccbt ccbt[2];

//~--- Local Functions ---
   
static void HandleCommand1bR(void);
static void HandleCommand1bW(void);

//~-----------------------------------------------------------------------------

void initialize_CCBT_task(void) {

  static uint8  volatile far i;
  static _ccbt  volatile far *c;

  for (i=0; i<2; i++) {
    c = &ccbt[i];
    c->use   = FALSE;
    c->state = CCBT_START;
    c->ok    = FALSE;
  }
}

//~-----------------------------------------------------------------------------

void CCBT_task(void) {

  static uint8  volatile far i, j;
  static uint8  volatile far sta_adr, dat_adr;
  static uint16 volatile far status;
  static uint16 volatile far dat[32];
  static _ccbt  volatile far *c;

//~--

  for (i=0; i<2; i++) {
    c = &ccbt[i];

    if (!c->use) continue;

    if (i) {
      sta_adr = CCBT1_STATUS;
      dat_adr = CCBT1_DATA;
    }
    else {
      sta_adr = CCBT0_STATUS;
      dat_adr = CCBT0_DATA;
    }

    switch (c->state) {
  
    case CCBT_START:
      c->timer = TimeGet();
      if (LvdsWrite(sta_adr, 0x0001) == OK) {
        c->state = CCBT_READ;
        break;
      }
    break;
    
    case CCBT_READ:
      if (TimeLapse(c->timer) > 5000) {
        c->state = CCBT_START;
        break;
      }
      if (LvdsRead(sta_adr, &status) == OK) {
        if (status & 0x0001) {
          for (j=0; j<32; j++) {
            if (LvdsRead(dat_adr, &dat[j]) == KO) {
              c->state = CCBT_START;
              break;
            }
          }
          c->ok     = TRUE;
          c->tstamp = TimeGet();
          for (j=0; j<32; j++) c->value[j] = dat[j];
          c->state  = CCBT_START;
          break;
        }
      }
      else {
        c->ok    = FALSE;
        c->state = CCBT_START;
        break;
      }
    break;
    }    // end of switch
  }      // end of for
}

//~-----------------------------------------------------------------------------

void read_CCBT_data(void) {

  static uint8  volatile far i, j;
  static uint32 volatile far age;
  static _ccbt  volatile far *c;
  
  CanRepBlkLen = 0;

  for (i=0; i<2; i++) {
    c = &ccbt[i];
    CanRepBlkDat[CanRepBlkLen++] = c->use;
    CanRepBlkDat[CanRepBlkLen++] = c->ok;
    age = TimeLapse(c->tstamp) / 1000;
    if (age & 0xFFFF0000) age = 0xFFFFFFFF;
    CanRepBlkDat[CanRepBlkLen++] = age >> 8;
    CanRepBlkDat[CanRepBlkLen++] = age & 0xFF;
    for (j=0; j<32; j++) {
      CanRepBlkDat[CanRepBlkLen++] = c->value[j] >> 8;
      CanRepBlkDat[CanRepBlkLen++] = c->value[j] & 0xFF;
    }
  }
}

//~=============================================================================

void HandleCommand1b(uint8 type) {

  if ((Gchip == PROM_CHIP) && (BACKDOOR_IS_DISABLED)) {
    SEND_REPLY_ERROR();
  }
  else {
    if (type & 0x40) HandleCommand1bW();
    else             HandleCommand1bR();
  }
} 

//~----------------------------------------------------------------------------

void HandleCommand1bR(void) {

  if (CanReqBlkLen) {
    SEND_REPLY_ERROR();
    return;
  }

  read_CCBT_data();
  SEND_REPLY_DATA();
}

//~----------------------------------------------------------------------------

void HandleCommand1bW(void) {

  static uint8 volatile far *p;

  if (CanReqBlkLen != 1) {
    SEND_REPLY_ERROR();
    return;
  }

  p = CanReqBlkDat;

  switch (*p & 0x03) {

    case 0: ccbt[1].use = FALSE; ccbt[0].use = FALSE; break;
    case 1: ccbt[1].use = FALSE; ccbt[0].use = TRUE;  break;
    case 2: ccbt[1].use = TRUE;  ccbt[0].use = FALSE; break;
    case 3: ccbt[1].use = TRUE;  ccbt[0].use = TRUE;  break;
  }

  SEND_REPLY_END();
}

//~=============================================================================

#endif   // #ifdef BUILD_FOR_CC

