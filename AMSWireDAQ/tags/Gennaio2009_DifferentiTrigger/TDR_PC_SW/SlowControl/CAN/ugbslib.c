// file ugbslib.c
//
// A library of UGBS box functions
//
// A.Lebedev Mar-2005...

#include "ugbslib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int8 BUS;                               // global variable
int  BRD;                               // global variable
int  PP;                                // global variable

//~============================================================================

void send_UGBS_command(
  int r_w, int adr, int ext, int16 wri, int16 *rea, bool *chk, int16 *err) {

  int32 wri32, rea32, m;
  int p;

  wri32 =        0x80000000;
  pack32(&wri32, 0x00080000, r_w);
  pack32(&wri32, 0x0F800000, swapbits(adr,  5));
  pack32(&wri32, 0x00700000, swapbits(BRD,  3));
  pack32(&wri32, 0x00070000, swapbits(ext,  3));
  pack32(&wri32, 0x0000FFFF, swapbits(wri, 16));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri32 & m) p ^= 1;
  if (p) wri32 ^= 0x40000000;
  
  exec_LeCroy_cmd(1, &BUS, &wri32, &rea32, chk, err);

  if (rea && !*err) *rea = swapbits(rea32, 16);
  
  if (PP > 1) {
    printf(" wri=%08X rea=%08X chk=%d err=0x%04X", wri32, rea32, *chk, *err);
  }
}

//~============================================================================

void enable_valves(int16 valves, bool *chk, int16 *err) {

  int adr = 0x01;

  if (PP > 1) printf("   Enabling valves:     mask = 0x%04X         ...        ", valves);
  
//send_UGBS_command(  r_w, adr, ext,    wri, &rea, &chk, &err);
  send_UGBS_command(WRITE, adr,   0, valves, NULL,  chk,  err);

  if (PP > 1) printf(" Done.\n");
}

//~----------------------------------------------------------------------------

void open_valve(int16 valve, int16 time, bool *chk, int16 *err) {

  int   adr = 0x00;
  int16 wri = (float)time / 1.024;
  int i;
  
  if (PP > 1) {
    printf("   Opening valve:       mask = 0x%04X, time = %5dms ...", valve, time);
  }
  
  if (valve == V20a + V20b) {
    adr = 0x15;
  }
  else {
    for (i=0; i<13; i++) {
      if (valve >> i == 0x0001) {
        adr = 0x08 + i;
        break;
      }
    }
  }

  if (!adr) {
    *err = 0x0400;
    return;
  }

//send_UGBS_command(  r_w, adr, ext, wri, &rea, &chk, &err);
  send_UGBS_command(WRITE, adr,   0, wri, NULL,  chk,  err);

  if (PP > 1) printf(" Done.\n");
}

//~----------------------------------------------------------------------------

void read_value(int16 sensor, int16 *dat, bool *chk, int16 *err) {

  int adr;
  
  if (PP > 1) printf("   Reading value: code = 0x%04X               ...        ", sensor);

  if (sensor < P1a || sensor > T2b) {
    *err = 0x0400;
    return;
  }

  adr = sensor;

//send_UGBS_command( r_w, adr, ext, wri, &rea, &chk, &err);
  send_UGBS_command(READ, adr,   0,   0,  dat,  chk,  err);

  if (PP > 1) printf(" Done.\n");
}

//~----------------------------------------------------------------------------

void enable_regulators(int16 voltages, bool *chk, int16 *err) {

  int adr = 0x1E;
  int16 wri;
  
  if (PP > 1) printf("   Enabling regulators: mask = 0x%04X         ...        ", voltages);

  if (voltages > _5V + _24V) {
    *err = 0x0400;
    return;
  }
  
  switch (voltages) {
    case        0: wri = 0x0001; break;
    case      _5V: wri = 0x0000; break;
    case     _24V: wri = 0x0003; break;
    case _5V+_24V: wri = 0x0002; break;
    default      : wri = 0x0001; break;
  }

//send_UGBS_command(  r_w, adr, ext, wri, &rea, &chk, &err);
  send_UGBS_command(WRITE, adr,   0, wri, NULL,  chk,  err);

  if (PP > 1) printf(" Done.\n");
}

//~============================================================================
