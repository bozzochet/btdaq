// file bricklib.c
//
// Library for HV Brick Functions
//
// A.Lebedev Apr-2005...

#include "bricklib.h"

//~----------------------------------------------------------------------------

int P;                      // global variable

int8  bus;                  // global variable
int16 geo;                  // global variable

//~============================================================================

void brick_channel_reset(int cha, bool *chk, int16 *err) {

  int8 cmd = 0x01;
  int32 rea, wri;
  int chacha;
  int32 m;
  int p;
  
  if (cha < 0x00 || cha > 0x2F) {
    *err = 0x0100;
    return;
  }
  
  wri = 0x80080000;
  pack32(&wri, 0x0F800000, swapbits(geo,  5));
  pack32(&wri, 0x00070000, swapbits(cmd,  3));
  pack32(&wri, 0x0000FC00, swapbits(cha,  6));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, err);

  if (*err || *chk) return;

  chacha = swapbits(unpack32(rea, 0x0000FC00), 6);
  if (cha != chacha) {
    *err = 0x0200;
    return;
  }
  if ((rea ^ wri) & 0x0000FFFF) {
    *err = 0x0400;
    return;
  }
}

//~----------------------------------------------------------------------------

void brick_status_read(int8 *sta, bool *chk, int16 *err) {

  int8 cmd = 0x02;
  int32 rea, wri;
  int32 m;
  int p;
  
  wri = 0x80000000;
  pack32(&wri, 0x0F800000, swapbits(geo, 5));
  pack32(&wri, 0x00070000, swapbits(cmd, 3));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, err);

  if (*err || *chk) return;
  
  if (rea & 0x000000FF) {
    *err = 0x0400;
    return;
  }
  
  if (sta) *sta = unpack32(rea, 0x0000FF00);
}

//~----------------------------------------------------------------------------

void brick_shutdown(bool *chk, int16 *err) {

  int8 cmd = 0x04;
  int32 rea, wri;
  int32 m;
  int p;
  
  wri = 0x80080000;
  pack32(&wri, 0x0F800000, swapbits(geo,  5));
  pack32(&wri, 0x00070000, swapbits(cmd,  3));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, err);
}

//~----------------------------------------------------------------------------

void brick_channel_read(int cha, int16 *cod, bool *chk, int16 *err) {

  int8 cmd = 0x05;
  int chacha;
  int32 rea, wri;
  int32 m;
  int p;
  
  if (cha < 0x00 || cha > 0x2F) {
    *err = 0x0100;
    return;
  }
  
  wri = 0x80000000;
  pack32(&wri, 0x0F800000, swapbits(geo, 5));
  pack32(&wri, 0x00070000, swapbits(cmd, 3));
  pack32(&wri, 0x0000FC00, swapbits(cha, 6));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, err);

  if (*err || *chk) return;
  
  chacha = swapbits(unpack32(rea, 0x0000FC00), 6);
  if (cha != chacha) {
    *err = 0x0200;
    return;
  }

  if (cod) *cod = (int16)swapbits(unpack32(rea, 0x000003FF), 10);
}

//~----------------------------------------------------------------------------

void brick_channel_write(int cha, int16 cod, bool *chk, int16 *err) {

  int8 cmd = 0x05;
  int32 rea, wri;
  int32 m;
  int p;
  
  if (cha < 0x00 || cha > 0x2F) {
    *err = 0x0100;
    return;
  }
  
  wri = 0x80080000;
  pack32(&wri, 0x0F800000, swapbits(geo,  5));
  pack32(&wri, 0x00070000, swapbits(cmd,  3));
  pack32(&wri, 0x0000FC00, swapbits(cha,  6));
  pack32(&wri, 0x000003FF, swapbits(cod, 10));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, err);

  if (*err || *chk) return;
  
  if ((rea ^ wri) & 0x0000FFFF) {
    *err = 0x0400;
    return;
  }
}

//~----------------------------------------------------------------------------

void brick_set_DCDC(int DCDC, bool ONOFF, int8 *sta, bool *chk, int16 *err) {

  int8 cmd;
  int32 rea, wri;
  int32 m;
  int p;
  
  if (DCDC < 0 || DCDC > 1) {
    *err = 0x0100;
    return;
  }

  wri = 0x80000000;
  pack32(&wri, 0x00000200, ONOFF ? 1 : 0);
  pack32(&wri, 0x0F800000, swapbits(geo, 5));
  if (DCDC) cmd = 0x06;
  else      cmd = 0x07;
  pack32(&wri, 0x00070000, swapbits(cmd, 3));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, err);

  if (*err || *chk) return;
  if (rea & 0x000000FF) {
    *err = 0x0400;
    return;
  }
  
  if (sta) *sta = unpack32(rea, 0x0000FF00);
}

//~============================================================================
