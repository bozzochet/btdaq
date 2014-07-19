// file ugbslib.h
//
// see ugbslib.c
//
// A.Lebedev Mar-2005...

#ifndef _UGBSLIB_H
#define _UGBSLIB_H

#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define V1a     0x0001
#define V2a     0x0002
#define V3a     0x0004
#define V4a     0x0008
#define V5      0x0008
#define Sp1     0x0010
#define V10a    0x0020
#define V20a    0x0040
#define V1b     0x0080
#define V2b     0x0100
#define V3b     0x0200
#define V4b     0x0400
#define V10b    0x0800
#define V20b    0x1000
#define Sp2     0x2000

#define P1a 0x16
#define T1a 0x17
#define P1b 0x18
#define T1b 0x19
#define P2a 0x1A
#define T2a 0x1B
#define P2b 0x1C
#define T2b 0x1D

#define  _5V 0x0001
#define _24V 0x0002

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void send_UGBS_command(
  int r_w, int adr, int ext, int16 wri, int16 *rea, bool *chk, int16 *err);

void enable_valves(int16 valves, bool *chk, int16 *err);
void open_valve(int16 valve, int16 time, bool *chk, int16 *err);
void read_value(int16 sensor, int16 *dat, bool *chk, int16 *err);
void enable_regulators(int16 voltages, bool *chk, int16 *err);

//~============================================================================

#endif // _UGBSLIB_H
