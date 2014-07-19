// file sfet2def.h
//
// SFET2 & SFEA2 Register Addresses and Bits (see Alessandro's docs)
//
// A.Lebedev - May-2007...

//~----------------------------------------------------------------------------

#ifndef _SFET2DEF_H
#define _SFET2DEF_H

#include "template.h"
#include "sdr2def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
//
//~--  DACs
//

//                       LT-A   LT-B     HT-A   HT-B    SHT-A  SHT-B
int16 dac_adr[3][2] = {{0x000, 0x001}, {0x002, 0x003}, {0x004, 0x005}};

_b dac_msk[16] = {
  { 0, 0x00008000, 2}, // Int/Ext
  { 0, 0x00004000, 2}, //
  { 0, 0x00002000, 2}, // LDAC
  { 0, 0x00001000, 2}, // PDB
  { 0, 0x00000800, 2}, // PDA
  { 0, 0x00000400, 2}, // A/B
  { 0, 0x00000200, 2}, // CR1
  { 0, 0x00000100, 2}, // CR0
  { 0, 0x00000080, 2}, // DB7
  { 0, 0x00000040, 2}, // DB6
  { 0, 0x00000020, 2}, // DB5
  { 0, 0x00000010, 2}, // DB4
  { 0, 0x00000008, 2}, // DB3
  { 0, 0x00000004, 2}, // DB2
  { 0, 0x00000002, 2}, // DB1
  { 0, 0x00000001, 2}};// DB0

//
//~--  Thresholds
//

float V_ref_ext = 2500.0;         // in mV
float V_ref_int = 3300.0 / 2.0;   // in mV

_b thr_msk = {0x00, 0x00FF, 256};
_b ref_msk = {0x00, 0x8000,   2};

//
//~--  Trigger Delay
//

int16 del_adr = {0x900};

_b del_msk = {0x00, 0x03FF, 1024};

//
//~--  Temperature
//

//                  H(T1)  L(T2)
int16 tmp_adr[2] = {0x102, 0x103};

_b tmp_msk[3] = {
  {0x00,     0x0FFF, -1},
  {0x00,     0x0FFF, -1},
  {0x00, 0xFFFFFFFF, -1}};

//
//~--  JTAG Control
//

int16 jtag_cmd_adr     = {0x700};
int16 jtag_ir_adr      = {0x701};
int16 ext_jtag_ena_adr = {0x702};
int16 ext_jtag_cod_adr = {0x703};

_b jtag_cmd_cod[4] = {
  {0, 0x0008, 2},
  {0, 0x0004, 2},
  {0, 0x0002, 2},
  {0, 0x0001, 2}};

_b jtag_cmd_com[1] = {
  {0, 0x0003, 4}};

_b jtag_ir_cod[5]  = {
  {0, 0x0010, 2},
  {0, 0x0008, 2},
  {0, 0x0004, 2},
  {0, 0x0002, 2},
  {0, 0x0001, 2}};

_b jtag_ir_ins[1]  = {
  {0, 0x000F, 16}};

_b ext_jtag_ena[1] = {
  {0, 0x0001, 2}};

_b ext_jtag_cod[5] = {
  {0, 0x0010, 2},
  {0, 0x0008, 2},
  {0, 0x0004, 2},
  {0, 0x0002, 2},
  {0, 0x0001, 2}};

//~============================================================================

#endif // _SFET2DEF_H
