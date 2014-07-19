// file sdr2def.h
//
// SDR2 Register Addresses and Bits (see Alessandro's docs)
//
// A.Lebedev - Feb-2007...

//~----------------------------------------------------------------------------

#ifndef _SDR2DEF_H
#define _SDR2DEF_H

#include "template.h"

#define SDR2    0x0 // SDR2    "link" address in SDR2
#define SFET2_A 0x1 // SFET2-A "link" address in SDR2
#define SFET2_B 0x2 // SFET2-B "link" address in SDR2
#define SPT2    0x3 // SPT2    "link" address in SDR2
#define SFET2_C 0x4 // SFET2-C "link" address in SDR2
#define SFET2_D 0x5 // SFET2-D "link" address in SDR2
#define SFEA2   0x6 // SFEA2   "link" address in SDR2
#define SFEC    0x7 // SFEC    "link" address in SDR2

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct {
  char *nam;
  int16 lnk;
  int16 adr;
} _scrate;

_scrate scrate[8] = {
  { "S0A",  6, 0x10A},
  { "S0B",  7, 0x10B},
  { "S1A",  4, 0x10E},
  { "S1B",  5, 0x10F},
  { "S2A", 18, 0x112},
  { "S2B", 19, 0x113},
  { "S3A", 20, 0x116},
  { "S3B", 21, 0x117}};

typedef struct {
  char *nam;
  int16 adr;
  int   typ;
} _sboard;

_sboard sboard[8] = {
  { "SDR2",    SDR2,    0},
  { "SFET2-A", SFET2_A, 1},
  { "SFET2-B", SFET2_B, 1},
  { "SPT2",    SPT2,    2},
  { "SFET2-C", SFET2_C, 1},
  { "SFET2-D", SFET2_D, 1},
  { "SFEA2",   SFEA2  , 1},
  { "SFEC",    SFEC,    3}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
//
//~--  SC Status Register
//

int16 sc_sta_adr = 0x000;

_b sc_sta[7] = {
  { 0x00, 0x0001, 2},   // Cmd_pend
  { 0x00, 0x0002, 2},   // Data_pend
  { 0x00, 0x0004, 2},   // Data_trunc
  { 0x00, 0x0008, 2},   // Pwr_pty_error
  { 0x00, 0x0010, 2},   // Pwr_error
  { 0x00, 0x0020, 2},   // Sc_timeout_error
  { 0x00, 0x0040, 2}};  // Seq_timeout_error
  
//
//~--  SC Timeout Register
//

int16 sc_tim_adr = 0x003;

_b sc_tim[6] = {
  { 0x00, 0x0001, 2},   // SFET2-A
  { 0x00, 0x0002, 2},   // SFET2-B
  { 0x00, 0x0004, 2},   // SPT2
  { 0x00, 0x0008, 2},   // SFET2-C
  { 0x00, 0x0010, 2},   // SFET2-D
  { 0x00, 0x0020, 2}};  // SFEA2
  
//
//~--  SEQ Timeout Register
//

int16 sq_tim_adr = 0x005;

_b sq_tim[15] = {
  { 0x00, 0x0001, 2},   // SFET2-A
  { 0x00, 0x0002, 2},   // SFET2-B
  { 0x00, 0x0004, 2},   // SPT2
  { 0x00, 0x0008, 2},   // SFET2-C
  { 0x00, 0x0010, 2},   // SFET2-D
  { 0x00, 0x0020, 2},   // SFEA2
  { 0x00, 0x0040, 2},   // SFET2-A
  { 0x00, 0x0080, 2},   // SFET2-B
  { 0x00, 0x0100, 2},   // SFET2-C
  { 0x00, 0x0200, 2},   // SFET2-D
  { 0x00, 0x0400, 2},   // SFEA2
  { 0x00, 0x0800, 2},   // SFEC0-0
  { 0x00, 0x1000, 2},   // SFEC0-1
  { 0x00, 0x2000, 2},   // SFEC1-0
  { 0x00, 0x4000, 2}};  // SFEC1-1
  
//
//~--  Power Control Register (Control & Monitor)
//

int16 pwr_ctl_adr = 0x002;

_b pwr_ctl[8] = {
  { 0x00, 0x0001, 2},   // SFET2-A
  { 0x00, 0x0002, 2},   // SFET2-B
  { 0x00, 0x0004, 2},   // SPT2
  { 0x00, 0x0008, 2},   // SFET2-C
  { 0x00, 0x0010, 2},   // SFET2-D
  { 0x00, 0x0020, 2},   // SFEA2
  { 0x00, 0x0040, 2},   // SFEC
  { 0x00, 0x0080, 2}};  // Cmd parity

_b pwr_mon[8] = {
  { 0x00, 0x0100, 2},   // SFET2-A
  { 0x00, 0x0200, 2},   // SFET2-B
  { 0x00, 0x0400, 2},   // SPT2
  { 0x00, 0x0800, 2},   // SFET2-C
  { 0x00, 0x1000, 2},   // SFET2-D
  { 0x00, 0x2000, 2},   // SFEA2
  { 0x00, 0x4000, 2},   // SFEC
  { 0x00, 0x8000, 2}};  // Cmd parity

//
//~--  Power Mask Registers
//

int16 pwr_msk_adr = 0x011;

_b pwr_msk[8] = {
  { 0x00, 0x0001, 2},   // SFET2-A
  { 0x00, 0x0002, 2},   // SFET2-B
  { 0x00, 0x0004, 2},   // SPT2
  { 0x00, 0x0008, 2},   // SFET2-C
  { 0x00, 0x0010, 2},   // SFET2-D
  { 0x00, 0x0020, 2},   // SFEA2
  { 0x00, 0x0040, 2},   // SPEC
  { 0x00, 0x0080, 2}};  // Cmd parity
  
//
//~--  Power Control Status ("software" register")
//

_b pwr_sta[8] = {
  { 0x00, 0x0003, 4},   // SFET2-A
  { 0x00, 0x000C, 4},   // SFET2-B
  { 0x00, 0x0030, 4},   // SPT2
  { 0x00, 0x00C0, 4},   // SFET2-C
  { 0x00, 0x0300, 4},   // SFET2-D
  { 0x00, 0x0C00, 4},   // SFEA2
  { 0x00, 0x3000, 4},   // SFEC
  { 0x00, 0xC000, 4}};  // Cmd parity
  
//
//~--  Actel Version ID Registers
//

int16 ver_adr[8] = {
  0x0FFF,   // SDR2
  0x0FFF,   // SFET2-A
  0x0FFF,   // SFET2-B
  0x0FFF,   // SPT2
  0x0FFF,   // SFET2-C
  0x0FFF,   // SFET2-D
  0x0FFF,   // SFEA2
  0x0FFF};  // SFEC

_b ver_msk = {0x00, 0xFFFF, -1};
  
//
//~--  Event Max Size
//

int16 max_event_size_adr = 0x008;

_b max_event_size = { 0x00, 0x07FF, 2048};
  
//
//~--  S/H Delay
//

int16 s_h_delay_adr = 0x00D;

_b s_h_delay = { 0x00, 0x07FF, 2048};
  
//
//~--  ADC Delay
//

int16 adc_delay_adr = 0x00E;

_b adc_delay = { 0x00, 0x07FF, 2048};
  
//
//~--  ADC Clock
//

int16 adc_clock_adr = 0x00F;

_b adc_clock = { 0x00, 0x07FF, 2048};
  
//
//~--  SFET/A2 Timeout
//

int16 sfet_timeout_adr = 0x010;

_b sfet_timeout = { 0x00, 0x07FF, 2048};
  
//
//~--  SFEC Readout Control
//

int16 sfec_adr = 0x012;

_b sfec = { 0x00, 0x0001, 2};
  
//
//~--  Programmable Dead Time
//

int16 dead_time_adr = 0x013;

_b dead_time = { 0x00, 0xFFFF, 65536};
  
//~============================================================================

#endif // _SDR2DEF_H
