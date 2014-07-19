// file spt2def.h
//
// SPT2 Register Addresses and Bits (see Alessandro's docs)
//
// A.Lebedev - Feb-2007...

//~----------------------------------------------------------------------------

#ifndef _SPT2DEF_H
#define _SPT2DEF_H

#include "template.h"
#include "sdr2def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
//
//~--  Power (see sdr2def.h)
//

_b *spt2_pwr_ctl = &pwr_ctl[SPT2-1];   // SPT2 power control
_b *spt2_pwr_msk = &pwr_msk[SPT2-1];   // SPT2 power mask
_b *spt2_pwr_mon = &pwr_mon[SPT2-1];   // SPT2 power monitor

//
//~--  Output Control (LVDS Drivers)
//

int16 lvds_adr  = 0x300;

_b lvds = {0, 0x0001, 2};

//
//~--  Internal Generator
//

#define START_SPT2_GENERATOR    0x0001
#define STOP_SPT2_GENERATOR     0x0000
#define TRIGGER_SPT2_GENERATOR  0x0002

int16 gen_ctrl_adr   = 0x500;
int16 gen_sta_adr    = 0x500;
int16 gen_period_adr = 0x501;

_b gen_sta = {0, 0x0001,  2};

_b gen_period[5] = {
  {0, 0xFFFF, 10},
  {0, 0xFFFF, 10},
  {0, 0xFFFF, 10},
  {0, 0xFFFF, 10},
  {0, 0xFFFF, 10}};

//
//~--  Scaler' Gate
//

int16 sca_gate_adr = 0x029;

_b sca_gate = {0, 0x0003, 4};

//
//~--  Signal Masks
//

int16 msk_adr[2][3] = {
//    CP     CT     BZ
  {0x201, 0x203, 0x205},  // L (0)
  {0x200, 0x202, 0x204}}; // H (1)

_b sig_msk[12] = {
  { 0, 0x00000001, 2}, //  0
  { 0, 0x00000002, 2}, //  1
  { 0, 0x00000004, 2}, //  2
  { 0, 0x00000008, 2}, //  3
  { 0, 0x00000010, 2}, //  4
  { 0, 0x00000020, 2}, //  5
  { 0, 0x00000040, 2}, //  6
  { 0, 0x00000080, 2}, //  7
  { 0, 0x00000100, 2}, //  8
  { 0, 0x00000200, 2}, //  9
  { 0, 0x00000400, 2}, //  Generator
  { 0, 0x00000800, 2}};//  External Signal
  
//
//~--  "States"
//

int16 stat_adr[2][2] = {
//    HT    SHT
  {0x101, 0x103},  // L (0)
  {0x100, 0x102}}; // H (1)

_b sig_stat[13] = {
  { 0, 0x00000001, 2}, //  0
  { 0, 0x00000002, 2}, //  1
  { 0, 0x00000004, 2}, //  2
  { 0, 0x00000008, 2}, //  3
  { 0, 0x00000010, 2}, //  4
  { 0, 0x00000020, 2}, //  5
  { 0, 0x00000040, 2}, //  6
  { 0, 0x00000080, 2}, //  7
  { 0, 0x00000100, 2}, //  8
  { 0, 0x00000200, 2}, //  9
  { 0, 0x00000400, 2}, //  CP Ext (HT) or not used (SHT)
  { 0, 0x00000800, 2}, //  CT Ext (HT) or not used (SHT)
  { 0, 0x00001000, 2}};//  not used (HT) or BZ (SHT)
  
//
//~--  Signal Patterns
//

int16 ptrn_adr[2][6] = {
//    HT    SHT  CPmsk  CTmsk  BZmsk  BZmsk
  {0x402, 0x404, 0x407, 0x40C, 0x409, 0x40E},  // L (0)
  {0x401, 0x403, 0x406, 0x40B, 0x408, 0x40D}}; // H (1)

_b sig_ptrn[13] = {
  { 0, 0x00000001, 2}, //  0
  { 0, 0x00000002, 2}, //  1
  { 0, 0x00000004, 2}, //  2
  { 0, 0x00000008, 2}, //  3
  { 0, 0x00000010, 2}, //  4
  { 0, 0x00000020, 2}, //  5
  { 0, 0x00000040, 2}, //  6
  { 0, 0x00000080, 2}, //  7
  { 0, 0x00000100, 2}, //  8
  { 0, 0x00000200, 2}, //  9
  { 0, 0x00000400, 2}, //  Generator
  { 0, 0x00000800, 2}, //  External Signal
  { 0, 0x00001000, 2}};//  External Signal (HT) or Output (masked CP, CT, BZ)
  
//
//~--  Scalers
//

int16 sca_adr[2][2][11] = {
//    0     1     2     3     4     5     6     7     8     9   Gen
 {{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x28},   // L (0), HT
  {0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x28}},  // L (0), SHT
 {{0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x28},   // H (1), HT
  {0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28}}}; // H (1), SHT

_b sca_msk = {0x00, 0xFFFF, -1};

//~============================================================================

#endif // _SPT2DEF_H
