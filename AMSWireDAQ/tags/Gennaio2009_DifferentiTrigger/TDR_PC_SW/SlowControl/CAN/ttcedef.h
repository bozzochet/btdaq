// file ttcedef.h
//
// TTCE "definition"
//
// A.Lebedev - Feb-2008...

//~----------------------------------------------------------------------------

#ifndef _TTCEDEF_H
#define _TTCEDEF_H

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//
//~-- 28V Power Control
//

_b pwr_w_esw_msk[] = { // E-switch "action", i.e. "write"
  {0x0, 0x01, 2},          // LLW_P
  {0x0, 0x02, 2},          // LLR_P
  {0x0, 0x04, 2},          // PR1_P
  {0x0, 0x08, 2},          // PR2_P
  {0x0, 0x10, 2},          // COR_P
  {0x0, 0x20, 2},          // SUP_P
  {0x0, 0x40, 2},          // GAE_P
  {0x0, 0x80, 2},          // FAE_P
  {0x3, 0x01, 2},          // LLW_S
  {0x3, 0x02, 2},          // LLR_S
  {0x3, 0x04, 2},          // PR1_S
  {0x3, 0x08, 2},          // PR2_S
  {0x3, 0x10, 2},          // COR_S
  {0x3, 0x20, 2},          // SUP_S
  {0x3, 0x40, 2},          // GAE_S
  {0x3, 0x80, 2}};         // FAE_S
  
_b pwr_w_msw_msk[] = { // M-switch "action", i.e. "write"
  {0x1, 0x01, 2},          // LLW_P
  {0x1, 0x02, 2},          // LLR_P
  {0x1, 0x04, 2},          // PR1_P
  {0x1, 0x08, 2},          // PR2_P
  {0x1, 0x10, 2},          // COR_P
  {0x1, 0x20, 2},          // SUP_P
  {0x1, 0x40, 2},          // GAE_P
  {0x1, 0x80, 2},          // FAE_P
  {0x4, 0x01, 2},          // LLW_S
  {0x4, 0x02, 2},          // LLR_S
  {0x4, 0x04, 2},          // PR1_S
  {0x4, 0x08, 2},          // PR2_S
  {0x4, 0x10, 2},          // COR_S
  {0x4, 0x20, 2},          // SUP_S
  {0x4, 0x40, 2},          // GAE_S
  {0x4, 0x80, 2}};         // FAE_S
  
_b pwr_w_swt_msk[] = { // Switch "action", i.e. "write"
  {0x6, 0x01, 2},          // GAC_P
  {0x6, 0x02, 2},          // FAC_P
  {0x6, 0x04, 2},          // TEC_P
  {0x6, 0x08, 2},          // GAC_S
  {0x6, 0x10, 2},          // FAC_S
  {0x6, 0x20, 2},          // TEC_S
  {0x6, 0x40, 2},          // P_P
  {0x6, 0x80, 2}};         // P_S
  
_b pwr_w_ena_msk[] = { // Enable "action", i.e. "write"
  {0x2, 0x01, 2},          // LLW_P
  {0x2, 0x02, 2},          // LLR_P
  {0x2, 0x04, 2},          // PR1_P
  {0x2, 0x08, 2},          // PR2_P
  {0x2, 0x10, 2},          // COR_P
  {0x2, 0x20, 2},          // SUP_P
  {0x2, 0x40, 2},          // GAE_P
  {0x2, 0x80, 2},          // FAE_P
  {0x5, 0x01, 2},          // LLW_S
  {0x5, 0x02, 2},          // LLR_S
  {0x5, 0x04, 2},          // PR1_S
  {0x5, 0x08, 2},          // PR2_S
  {0x5, 0x10, 2},          // COR_S
  {0x5, 0x20, 2},          // SUP_S
  {0x5, 0x40, 2},          // GAE_S
  {0x5, 0x80, 2},          // FAE_S
  {0x7, 0x01, 2},          // GAC_P
  {0x7, 0x02, 2},          // FAC_P
  {0x7, 0x04, 2},          // TEC_P
  {0x7, 0x08, 2},          // GAC_S
  {0x7, 0x10, 2},          // FAC_S
  {0x7, 0x20, 2},          // TEC_S
  {0x7, 0x40, 2},          // P_P
  {0x7, 0x80, 2}};         // P_S
  
_b pwr_r_esw_msk[] = { // E-switch "control", i.e. "read"
  {0x0, 0x01, 2},          // LLW_P
  {0x0, 0x02, 2},          // LLR_P
  {0x0, 0x04, 2},          // PR1_P
  {0x0, 0x08, 2},          // PR2_P
  {0x0, 0x10, 2},          // COR_P
  {0x0, 0x20, 2},          // SUP_P
  {0x0, 0x40, 2},          // GAE_P
  {0x0, 0x80, 2},          // FAE_P
  {0x2, 0x01, 2},          // LLW_S
  {0x2, 0x02, 2},          // LLR_S
  {0x2, 0x04, 2},          // PR1_S
  {0x2, 0x08, 2},          // PR2_S
  {0x2, 0x10, 2},          // COR_S
  {0x2, 0x20, 2},          // SUP_S
  {0x2, 0x40, 2},          // GAE_S
  {0x2, 0x80, 2}};         // FAE_S
  
_b pwr_r_msw_msk[] = { // M-switch "control", i.e. "read"
  {0x1, 0x01, 2},          // LLW_P
  {0x1, 0x02, 2},          // LLR_P
  {0x1, 0x04, 2},          // PR1_P
  {0x1, 0x08, 2},          // PR2_P
  {0x1, 0x10, 2},          // COR_P
  {0x1, 0x20, 2},          // SUP_P
  {0x1, 0x40, 2},          // GAE_P
  {0x1, 0x80, 2},          // FAE_P
  {0x3, 0x01, 2},          // LLW_S
  {0x3, 0x02, 2},          // LLR_S
  {0x3, 0x04, 2},          // PR1_S
  {0x3, 0x08, 2},          // PR2_S
  {0x3, 0x10, 2},          // COR_S
  {0x3, 0x20, 2},          // SUP_S
  {0x3, 0x40, 2},          // GAE_S
  {0x3, 0x80, 2}};         // FAE_S
  
_b pwr_r_swt_msk[] = { // Switch "control", i.e. "read"
  {0x4, 0x01, 2},          // GAC_P
  {0x4, 0x02, 2},          // FAC_P
  {0x4, 0x04, 2},          // TEC_P
  {0x4, 0x08, 2},          // GAC_S
  {0x4, 0x10, 2},          // FAC_S
  {0x4, 0x20, 2},          // TEC_S
  {0x4, 0x40, 2},          // P_P
  {0x4, 0x80, 2}};         // P_S

_b pwr_cmd_msk[] = {
  { 0, 0x00000003, 3},
  { 1, 0x00000003, 3},
  { 2, 0x00000003, 3},
  { 3, 0x00000003, 3},
  { 4, 0x00000003, 3},
  { 5, 0x00000003, 3},
  { 6, 0x00000003, 3},
  { 7, 0x00000003, 3},
  { 8, 0x00000003, 3},
  { 9, 0x00000003, 3},
  {10, 0x00000003, 3},
  {11, 0x00000003, 3},
  {12, 0x00000003, 3},
  {13, 0x00000003, 3},
  {14, 0x00000003, 3},
  {15, 0x00000003, 3},
  {16, 0x00000003, 3},
  {17, 0x00000003, 3},
  {18, 0x00000003, 3},
  {19, 0x00000003, 3},
  {20, 0x00000003, 3},
  {21, 0x00000003, 3},
  {22, 0x00000003, 3},
  {23, 0x00000003, 3}};
  
_b pwr_sta_msk[] = {
  { 5, 0x01, 2},
  { 5, 0x02, 2},
  { 5, 0x04, 2},
  { 5, 0x08, 2},
  { 5, 0x10, 2},
  { 5, 0x20, 2},
  { 5, 0x40, 2},
  { 5, 0x80, 2},
  { 6, 0x01, 2},
  { 6, 0x02, 2},
  { 6, 0x04, 2},
  { 6, 0x08, 2},
  { 6, 0x10, 2},
  { 6, 0x20, 2},
  { 6, 0x40, 2},
  { 6, 0x80, 2},
  { 7, 0x01, 2},
  { 7, 0x02, 2},
  { 7, 0x04, 2},
  { 7, 0x08, 2},
  { 7, 0x10, 2},
  { 7, 0x20, 2},
  { 7, 0x40, 2},
  { 7, 0x80, 2}};

//
//~-- Pump Control
//

_b pump_volt_cod_msk[2][2] = {
 {{0, 0xF0, 16},
  {0, 0x0F, 16}},
 {{1, 0xF0, 16},
  {1, 0x0F, 16}}};
_b pump_volt_val_msk[2] = {
  {0, 0xFF, 256},
  {1, 0xFF, 256}};

_b pump_rpm_cod_msk[2] = {
  {2, 0xFFF, -1},
  {3, 0xFFF, -1}};
_b pump_rpm_val_msk[2] = {
  {2, 0xFFFF, -1},
  {3, 0xFFFF, -1}};

//
//~-- PWM Control
//

_b pwm_sel_msk[6] = {
  {0, 0x01, 2},
  {1, 0x01, 2},
  {2, 0x01, 2},
  {3, 0x01, 2},
  {4, 0x01, 2},
  {5, 0x01, 2}};

_b pwm_cod_msk[2] = {
  {0, 0xF0, 16},
  {0, 0x0F, 16}};
  
_b pwm_val_msk[1] = {
  {0, 0xFF, 256}};

_b pwm_pct_msk[1] = {
  {0, 0xFF, 90}};

//
//~-- Pressure Sensor Control
//

_b pres_msk[4] = {
  {0, 0x0FFF, -1},
  {1, 0x0FFF, -1},
  {2, 0x0FFF, -1},
  {3, 0x0FFF, -1}};

//
//~-- Pt Sensor Control
//

_b pt_rc_msk  = {0, 0x0003,      4};
_b pt_sel_msk = {0, 0xFFFFFFFF, -1};

_b pt_adc_msk[3][15] = {

//~ N-sensors:

 {{  0, 0xFFFF, -1},  //  REF N
  
  {  3, 0xFFFF, -1},  //  Pt01NP
  {  6, 0xFFFF, -1},  //  Pt02NP
  {  9, 0xFFFF, -1},  //  Pt03NP
  { 12, 0xFFFF, -1},  //  Pt04NP
  { 15, 0xFFFF, -1},  //  Pt05NP
  { 18, 0xFFFF, -1},  //  Pt06_P
  { 21, 0xFFFF, -1},  //  Pt09_P

  { 27, 0xFFFF, -1},  //  Pt01NS
  { 30, 0xFFFF, -1},  //  Pt02NS
  { 33, 0xFFFF, -1},  //  Pt03NS
  { 36, 0xFFFF, -1},  //  Pt04NS
  { 39, 0xFFFF, -1},  //  Pt05NS
  { 42, 0xFFFF, -1},  //  Pt06_S
  { 45, 0xFFFF, -1}}, //  Pt09_S

//~ L-sensors:

 {{  1, 0xFFFF, -1},  //  REF L
  
  {  4, 0xFFFF, -1},  //  Pt01LP
  {  7, 0xFFFF, -1},  //  Pt02LP
  { 10, 0xFFFF, -1},  //  Pt03LP
  { 13, 0xFFFF, -1},  //  Pt04LP
  { 16, 0xFFFF, -1},  //  Pt05LP
  { 19, 0xFFFF, -1},  //  Pt07_P
  { 22, 0xFFFF, -1},  //  Pt10_P
  
  { 28, 0xFFFF, -1},  //  Pt01LS
  { 31, 0xFFFF, -1},  //  Pt02LS
  { 34, 0xFFFF, -1},  //  Pt03LS
  { 37, 0xFFFF, -1},  //  Pt04LS
  { 40, 0xFFFF, -1},  //  Pt05LS
  { 43, 0xFFFF, -1},  //  Pt07_L
  { 46, 0xFFFF, -1}}, //  Pt10_L

//~ R-sensors:

 {{  2, 0xFFFF, -1},  //  REF R
  
  {  5, 0xFFFF, -1},  //  Pt01RP
  {  8, 0xFFFF, -1},  //  Pt02RP
  { 11, 0xFFFF, -1},  //  Pt03RP
  { 14, 0xFFFF, -1},  //  Pt04RP
  { 17, 0xFFFF, -1},  //  Pt05RP
  { 20, 0xFFFF, -1},  //  Pt08_P
  { 23, 0xFFFF, -1},  //  Pt11_P
  
  { 29, 0xFFFF, -1},  //  Pt01RS
  { 32, 0xFFFF, -1},  //  Pt02RS
  { 35, 0xFFFF, -1},  //  Pt03RS
  { 38, 0xFFFF, -1},  //  Pt04RS
  { 41, 0xFFFF, -1},  //  Pt05RS
  { 44, 0xFFFF, -1},  //  Pt08_L
  { 47, 0xFFFF, -1}}};//  Pt11_L

//~ "Control" Control:

_b ctrl_ctrl_msk[] = {
  {0x00, 0x30, 4},    // range
  {0x00, 0x08, 2},    // heat (RO)
  {0x00, 0x04, 2},    // FG
  {0x00, 0x02, 2},    // test
  {0x00, 0x01, 2}};  // pi_ena

//~ "Settings" Control:

_b ctrl_set_msk[] = {
  {0x01, 0x003F,    64},   // Set_point
  {0x02, 0x00FF,   256},   // k1
  {0x03, 0x00FF,   256},   // k2
  {0x04, 0x00FF,   256},   // k3
  {0x05, 0x003F,    64},   // iband
  {0x06, 0xFFFF, 65536},   // Feed_forw
  {0x08, 0x0FFF,  4096},   // Test_T
  {0x0F, 0x001F,    32}};  // cav_magine

//~ "Parameters" Control:

_b ctrl_par_msk[] = {
  {0x0A, 0xFFFF, 65536},   // ph_term
  {0x0C, 0xFFFF, 65536},   // ih_term
  {0x0E, 0x00FF,   256},   // pi_dcv
  {0x19, 0xFFFF, 65536}};  // cycle_cnt

//~ "Alarms":

_b ctrl_ala_ena_msk[] = {
  {0x16, 0x80, 2},   // GAC
  {0x16, 0x40, 2},   // LPS
  {0x16, 0x20, 2},   // LLR
  {0x16, 0x10, 2},   // LLW
  {0x16, 0x08, 2},   // PR2
  {0x16, 0x04, 2},   // PR1
  {0x16, 0x02, 2},   // TRK
  {0x16, 0x01, 2}};  // CAV

_b ctrl_ala_now_msk[] = {
  {0x17, 0x80, 2},   // GAC
  {0x17, 0x40, 2},   // LPS
  {0x17, 0x20, 2},   // LLR
  {0x17, 0x10, 2},   // LLW
  {0x17, 0x08, 2},   // PR2
  {0x17, 0x04, 2},   // PR1
  {0x17, 0x02, 2},   // TRK
  {0x17, 0x01, 2}};  // CAV

_b ctrl_ala_was_msk[] = {
  {0x18, 0x80, 2},   // GAC
  {0x18, 0x40, 2},   // LPS
  {0x18, 0x20, 2},   // LLR
  {0x18, 0x10, 2},   // LLW
  {0x18, 0x08, 2},   // PR2
  {0x18, 0x04, 2},   // PR1
  {0x18, 0x02, 2},   // TRK
  {0x18, 0x01, 2}};  // CAV

//~ "Loops" Control:

_b ctrl_loop_ena_msk[] = {
  {0x10, 0x40, 2},   // LLW
  {0x11, 0x40, 2},   // LLR
  {0x12, 0x40, 2},   // PR1
  {0x13, 0x40, 2},   // PR2
  {0x14, 0x40, 2},   // COR
  {0x15, 0x40, 2}};  // SUP

_b ctrl_loop_set_msk[] = {
  {0x10, 0x3F, 64},  // LLW
  {0x11, 0x3F, 64},  // LLR
  {0x12, 0x3F, 64},  // PR1
  {0x13, 0x3F, 64},  // PR2
  {0x14, 0x3F, 64},  // COR
  {0x15, 0x3F, 64}}; // SUP

_b ctrl_loop_out_msk[] = {
  {0x10, 0x80, 2},   // LLW
  {0x11, 0x80, 2},   // LLR
  {0x12, 0x80, 2},   // PR1
  {0x13, 0x80, 2},   // PR2
  {0x14, 0x80, 2},   // COR
  {0x15, 0x80, 2}};  // SUP

//~============================================================================

#endif // _TTCEDEF_H
