// file cabdef.h
//
// CAB Data Structures
//
// A.Lebedev - Nov-2006...

//~----------------------------------------------------------------------------

#ifndef _CABDEF_H
#define _CABDEF_H

#include "cablib.h"
#include "template.h"

//~----------------------------------------------------------------------------
//
//~--  CAB Essential TM Status
//

_sta cab_ess_sta[] = {
// SW_STATUS
  {  0,  1},    //  0 CPU_ID (CPU Used)
  {  1,  1},    //  1 RESET_CAUSE
  {  2,  1},    //  2 SW_STATE_CURRENT_CPU
  {  3,  1},    //  3 SW_STATE_OTHER_CPU
  {  4,  1},    //  4 SW_SEQUENCE_IN_COURSE
  {  5,  1},    //  5 SUSW_VERSION
  {  6,  1},    //  6 APSW_VERSION
  {  7,  4},    //  7 SW_CYCLE_COUNTER (CPU Time)
// ERROR_INFORMATION
  { 11,  2},    //  8 ERROR_COUNTER
  { 13,  1},    //  9 ERROR_TABLE: ERROR_CODE
  { 14,  4},    // 10 ERROR_TABLE: SW_CYCLE_COUNTER
  { 18,  2},    // 11 ERROR_TABLE: ERROR_PARAMETER
  { 20,  2},    // 12 CMD_REJECTED_COUNTER
  { 22,  1},    // 13 CAN_0_STATUS
  { 23,  1},    // 14 CAN_0_TX_STATUS
  { 24,  1},    // 15 CAN_0_RX_STATUS
  { 25,  1},    // 16 CAN_1_STATUS
  { 26,  1},    // 17 CAN_1_TX_STATUS
  { 27,  1},    // 18 CAN_1_RX_STATUS
// START_UP_REPORT
  { 28,  2},    // 19 PROM_CRC_EXP
  { 30,  2},    // 20 PROM_CRC_CALC
  { 32,  2},    // 21 SW_PROGRAM_1_CRC_EXP
  { 34,  2},    // 22 SW_PROGRAM_1_CRC_CALC
  { 36,  2},    // 23 SW_PROGRAM_2_CRC_EXP
  { 38,  2},    // 24 SW_PROGRAM_2_CRC_CALC
  { 40,  2},    // 25 SW_TABLE_CRC_EXP
  { 42,  2},    // 26 SW_TABLE_CRC_CALC
  { 44,  2},    // 27 PROGRAM_RAM_CRC_CALC
  { 46,  1},    // 28 RAM_TEST
// CONVERTER'S AND PROTECTION STATUSES
  { 47,  1},    // 29 CCSC_OTHER_DC_DC_STATUS
  { 48,  1},    // 30 LATCHUP_PROTECTION_STATUS
  { 49,  1},    // 31 PS_NCV_STATUS
  { 50,  1},    // 32 PS_RCV_STATUS
  { 51,  1},    // 33 CSP_CV_STATUS
  { 52,  1},    // 34 CSP_28V_STATUS (LCL Battery)
  { 53,  1},    // 35 CSP_ALARM
  { 54,  1},    // 36 CCS_SEMIC_SW_STATUS
  { 55,  1}};   // 37 CCS_OUTPUT_ENABLE_STATUS

//~--

  _b cpu_used = {
       0, 0xFF,  3};
  _b sw_vers[] = {
    {  5, 0xFF, -1},
    {  6, 0xFF, -1}};
  _b cpu_time = { 
       7, 0xFFFFFFFF, -1};
  _b cpu_state[] = {
    {  2, 0xFF, 3},
    {  3, 0xFF, 3}};
  _b exp_crc[] = {
    { 19, 0xFFFF, -1},
    { 21, 0xFFFF, -1},
    { 23, 0xFFFF, -1},
    { 25, 0xFFFF, -1}};
  _b calc_crc[] = {
    { 20, 0xFFFF, -1},
    { 22, 0xFFFF, -1},
    { 24, 0xFFFF, -1},
    { 26, 0xFFFF, -1},
    { 27, 0xFFFF, -1}};
  _b ram_test[] = {
    { 28, 0x01, 2},
    { 28, 0x02, 2}};
  _b reset_cause[] = {
    {  1, 0x01, 2},
    {  1, 0x02, 2},
    {  1, 0x04, 2},
    {  1, 0x08, 2},
    {  1, 0x10, 2},
    {  1, 0x20, 2}};
  _b sw_seq[] = {
    {  4, 0x01, 2},  // 0 PTM Acquisition
    {  4, 0x02, 2},  // 1 DLCM Acquisition
    {  4, 0x04, 2},  // 2 Current Setpoint Programming
    {  4, 0x08, 2},  // 3 Arm Quench
    {  4, 0x10, 2},  // 4 Current Lead SEL Clear
    {  4, 0x20, 2}}; // 5 Magnet Detector SEL Clear
  _b other_dcdc_ctrl[] = {
    { 29, 0xFF, 3}};
  _b latch_up_ctrl[] = {
    {  30, 0xFF, 3}};
  _b ps_cv[] = {
    { 31, 0xFF, 3},
    { 32, 0xFF, 3}};
  _b csp_cv[] = {
    { 33, 0x01, 2},
    { 33, 0x02, 2},
    { 33, 0x04, 2},
    { 33, 0x08, 2},
    { 33, 0x10, 2},
    { 33, 0x20, 2},
    { 33, 0x40, 2},
    { 33, 0x80, 2}};
  _b lcl_bat_ctrl[] = {
    { 34, 0x01, 2},
    { 34, 0x02, 2}};
  _b csp_alarm[] = {
    { 35, 0xFF, 3}};
  _b ccs_semic_sw[] = {
    { 36, 0xFF, 3}};
  _b ccs_out_ctrl[] = {
    { 37, 0xFF, 3}};
    
//~----------------------------------------------------------------------------
//
//~--  CAB Digital TM Status
//

_sta cab_dig_sta[] = {
  {  0,  4},    //  0 SW_CYCLE_COUNTER (CPU Time)
// CCS_STATUS
  {  4,  1},    //  1 CCS_CONV_CMD_STATUS
  {  5,  2},    //  2 CURRENT_1_CMD_SETTINGS
  {  7,  2},    //  3 CURRENT_1_CMD_SETTINGS
  {  9,  2},    //  4 CURRENT_1_CMD_SETTINGS
  { 11,  1},    //  5 CCS_COMMANDING_STATUS
// STM_STATUS
  { 12,  1},    //  6 STM_STATUS
// PTM_STATUS
  { 13,  1},    //  7 PTM_STATUS
// PS_STATUS
  { 14,  1},    //  8 PS_CMD_SW_STATUS
  { 15,  1},    //  9 PS_A_STATUS
  { 16,  1},    // 10 PS_B_STATUS
  { 17,  1},    // 11 PS_C_STATUS
  { 18,  1}};   // 12 PS_D_STATUS

//~--

  _b ccs_cv_sta[] = {
    { 1, 0x01, 2},
    { 1, 0x02, 2},
    { 1, 0x04, 2},
    { 1, 0x08, 2},
    { 1, 0x10, 2},
    { 1, 0x20, 2}};
  _b ccs_cur[] = {
    { 2, 0x0FFF, -1},
    { 3, 0x0FFF, -1},
    { 4, 0x0FFF, -1}};
  _b ccs_cmd_sta[] = {
    { 5, 0xFF, 3}};
  _b stm_sta[] = {
    { 6, 0x01, 2},
    { 6, 0x02, 2}};
  _b ptm_sta[] = {
    { 7, 0x01, 2},
    { 7, 0x02, 2}};
  _b ps_cmd_sw_sta[] = { // "Read Control"
    { 8, 0x01, 2},       // 0 Interpoint DC/DC Converters ON/OFF (28V_ISO)
    { 8, 0x02, 2},       // 1 Vacuum Sensor Line Driver ON/OFF
    { 8, 0x04, 2},       // 2 HT Heater ON/OFF (DLCM)
    { 8, 0x08, 2},       // 3 Pressure Sensor ON/OFF (STM_ACQUIRE)
    { 8, 0x10, 2},       // 4 
    { 8, 0x20, 2},       // 5 Pilot DV55 (1=OPEN, 0=CLOSED) 
    { 8, 0x40, 2},       // 6 Pilot DV57 (1=OPEN, 0=CLOSED) 
    { 8, 0x80, 2}};      // 7 Not Used
  _b ps_a_sta[] = {
    { 9, 0x01, 2},       // 0 DV20A group 1=ENA/0=DIS 
    { 9, 0x02, 2},       // 1 DV20B group 1=ENA/0=DIS 
    { 9, 0x04, 2},       // 2 DV20C group 1=ENA/0=DIS 
    { 9, 0x08, 2},       // 3 DV20D group 1=ENA/0=DIS 
    { 9, 0x10, 2},       // 4 DV20A valve status 1=OPN/0=CLS
    { 9, 0x20, 2},       // 5 DV20B valve status 1=OPN/0=CLS
    { 9, 0x40, 2},       // 6 DV20C valve status 1=OPN/0=CLS
    { 9, 0x80, 2}};      // 7 DV20D valve status 1=OPN/0=CLS
  _b ps_b_sta[] = {
    {10, 0x01, 2},
    {10, 0x02, 2},
    {10, 0x04, 2},
    {10, 0x08, 2},
    {10, 0x10, 2},
    {10, 0x20, 2},
    {10, 0x40, 2},
    {10, 0x80, 2}};
  _b ps_c_sta[] = {
    {11, 0x01, 2},       // 0 Warm ON Group  1=ENA/0=DIS 
    {11, 0x02, 2},       // 1 Warm OFF Group 1=ENA/0=DIS 
    {11, 0x04, 2},       // 2 Warm DV09A 1=CLOSED/0=NOT CLOSED
    {11, 0x08, 2},       // 3 Warm DV09B 1=CLOSED/0=NOT CLOSED
    {11, 0x10, 2},       // 4 Warm DV11A 1=CLOSED/0=NOT CLOSED
    {11, 0x20, 2},       // 5 Warm DV11B 1=CLOSED/0=NOT CLOSED
    {11, 0x40, 2},       // 6 Warm DV16A 1=CLOSED/0=NOT CLOSED
    {11, 0x80, 2}};      // 7 Warm DV16B 1=CLOSED/0=NOT CLOSED
  _b ps_d_sta[] = {
    {12, 0x01, 2},      // 0  HT Heater Group ON/OFF
    {12, 0x02, 2},      // 1  Vacuum Sensor Line Driver ON/OFF
    {12, 0x04, 2},      // 2  HT Heater Line Driver ON/OFF (DLCM)
    {12, 0x08, 2},      // 3  Pressure Sensor Group ON/OFF (STM ACQUIRE)
    {12, 0x10, 2},      // 4  Pressure Sensor Line Driver ON/OFF (STM ACQUIRE)
    {12, 0x20, 2},      // 5  Pilot Cold Group (1=ENA/ON, 0=DIS/OFF)
    {12, 0x40, 2},      // 6
    {12, 0x80, 2}};     // 7  PS DC/DC Converter ON/OFF
    
//~----------------------------------------------------------------------------
//
//~--  CSP Digital TM Status
//

  _sta csp_dig_sta;

//~----------------------------------------------------------------------------
//
//~--  STM Channels
//

  _sta stm_cha[] = {
    { 0,  4},    //  0 CPU Time
    { 4,  2},    //  1 
    { 6,  2},    //  2 
    { 8,  2},    //  3 
    {10,  2},    //  4 
    {12,  2},    //  5 
    {14,  2},    //  6 
    {16,  2},    //  7 
    {18,  2},    //  8 
    {20,  2},    //  9 
    {22,  2},    // 10 
    {24,  2},    // 11 
    {26,  2},    // 12 
    {28,  2},    // 13 
    {30,  2},    // 14 
    {32,  2},    // 15 
    {34,  2},    // 16 
    {36,  2},    // 17 
    {38,  2},    // 18 
    {40,  2},    // 19 
    {42,  2},    // 20 
    {44,  2},    // 21 
    {46,  2},    // 22 
    {48,  2},    // 23 
    {50,  2},    // 24 
    {52,  2},    // 25 
    {54,  2},    // 26 
    {56,  2},    // 27 
    {58,  2},    // 28 
    {60,  2},    // 29 Vgain Thermistors
    {62,  2},    // 30 
    {64,  2},    // 31 
    {66,  2},    // 32 
    {68,  2},    // 33 
    {70,  2},    // 34 
    {72,  2},    // 35 
    {74,  2},    // 36 
    {76,  2},    // 37 
    {78,  2},    // 38 
    {80,  2},    // 39 
    {82,  2},    // 40 
    {84,  2},    // 41 
    {86,  2},    // 42 
    {88,  2},    // 43 
    {90,  2},    // 44 Voffset Voltage
    {92,  2}};   // 45 Vgain Voltage

  _b stm_time[] = {{ 0, 0xFFFFFFFF, -1}};
  _b VgainT[]   = {{29, 0x00000FFF, -1}};
  _b Voffset[]  = {{44, 0x00000FFF, -1}};
  _b VgainV[]   = {{45, 0x00000FFF, -1}};

//
//~--  Cold Valve Status (position)
//

  _sta cv__pos[] = {
    { 0,  4},    //  0 CPU Time
    { 4,  2},    //  1 DV05
    { 6,  2},    //  2 DV06
    { 8,  2},    //  3 DV07
    {10,  2}};   //  4 DV14

  _b cv_pos[]  = {
    {0, 0xFFFFFFFF, -1},  //  CPU Time
    {1, 0x00000FFF, -1},  //  DV05
    {2, 0x00000FFF, -1},  //  DV06
    {3, 0x00000FFF, -1},  //  DV07
    {4, 0x00000FFF, -1}}; //  DV14

//~----------------------------------------------------------------------------
//
//~--  PTM Channels
//

  _sta ptm_cha[] = {
    { 0,  4,  0},       //  0 CPU Time
    { 4,  2,  1, 0},    //  1 Offset of Superfluid Cooling Loop 1
    { 6,  2,  1, 0},    //  2 Offset of Superfluid Cooling Loop 2
    { 8,  2,  1, 0},    //  3 Offset of Superfluid Cooling Loop 3
    {10,  2,  1, 0},    //  4 Offset of Superfluid Cooling Loop 4
    {12,  2,  1, 0},    //  5 Offset of Superfluid Cooling Loop 5
    {14,  2,  1, 0},    //  6 Offset of Superfluid Cooling Loop 6
    {16,  2,  3},       //  7 Offset of Helium II Bath
    {18,  2,  4},       //  8 Offset of Mass Gauging
    {20,  2,  2},       //  9 Offset of Porous Plug
    {22,  2,  1, 2},    // 10 Offset of Vapour Cooled Shield 1
    {24,  2,  1, 2},    // 11 Offset of Vapour Cooled Shield 2
    {26,  2,  1, 2},    // 12 Offset of Vapour Cooled Shield 3
    {28,  2,  1, 2},    // 13 Offset of Vapour Cooled Shield 4
    {30,  2,  2},       // 14 Offset of Cool Down Thermo-Mechanical Pump
    {32,  2,  2},       // 15 Offset of Current Lead Thermo-Mechanical Pump
    {34,  2,  1, 3},    // 16 Offset of Magnet Current Lead 1
    {36,  2,  1, 3},    // 17 Offset of Magnet Current Lead 2
    {38,  2,  1, 3},    // 18 Offset of Magnet Current Lead 3
    {40,  2,  1, 2},    // 19 Offset of Cool Down Circuit 1
    {42,  2,  1, 2},    // 20 Offset of Cool Down Circuit 2
    {44,  2,  1, 2},    // 21 Offset of Cool Down Circuit 3
    {46,  2,  1, 1},    // 22 Offset of Persistent Switch
    {48,  2,  5},       // 23 -Vref1_Calibration
    {50,  2,  1, 0},    // 24 Superfluid Cooling Loop 1
    {52,  2,  1, 0},    // 25 Superfluid Cooling Loop 2
    {54,  2,  1, 0},    // 26 Superfluid Cooling Loop 3
    {56,  2,  1, 0},    // 27 Superfluid Cooling Loop 4
    {58,  2,  1, 0},    // 28 Superfluid Cooling Loop 5
    {60,  2,  1, 0},    // 29 Superfluid Cooling Loop 6
    {62,  2,  3},       // 30 Helium II Bath
    {64,  2,  4},       // 31 Mass Gauging
    {66,  2,  2},       // 32 Porous Plug
    {68,  2,  1, 2},    // 33 Vapour Cooled Shield 1
    {70,  2,  1, 2},    // 34 Vapour Cooled Shield 2
    {72,  2,  1, 2},    // 35 Vapour Cooled Shield 3
    {74,  2,  1, 2},    // 36 Vapour Cooled Shield 4
    {76,  2,  2},       // 37 Cool Down Thermo-Mechanical Pump
    {78,  2,  2},       // 38 Current Lead Thermo-Mechanical Pump
    {80,  2,  1, 3},    // 39 Magnet Current Lead 1
    {82,  2,  1, 3},    // 40 Magnet Current Lead 2
    {84,  2,  1, 3},    // 41 Magnet Current Lead 3
    {86,  2,  1, 2},    // 42 Cool Down Circuit 1
    {88,  2,  1, 2},    // 43 Cool Down Circuit 2
    {90,  2,  1, 2},    // 44 Cool Down Circuit 3
    {92,  2,  1, 1},    // 45 Persistent Switch
    {94,  2,  5},       // 46 +Vref1_Calibration
    {96,  2,  5},       // 47 +Vref2_Calibration
    {98,  2,  5}};      // 48 +Vref3_Calibration

  _b ptm_time[] = {{ 0, 0xFFFFFFFF, -1}};
  _b ptm_Vref[] = {
    {23, 0x0000FFFF, -1},
    {46, 0x0000FFFF, -1},
    {47, 0x0000FFFF, -1},
    {48, 0x0000FFFF, -1}};

//~----------------------------------------------------------------------------
//
//~--  CCS Analog TM
//

  _sta ccs__tm[] = {
    { 0,  4},    //  0 CPU Time
    { 4,  2},    //  1 ITARGET1_TM N
    { 6,  2},    //  2 ITARGET2_TM N
    { 8,  2},    //  3 ITARGET3_TM N
    {10,  2},    //  4 CCS_CURRENT_TM N
    {12,  2},    //  5 CCS_VOLTAGE_TM N
    {14,  2},    //  6 CCS_E_DUMP_VOLTAGE_TM N
    {16,  2},    //  7 CURRENT_TM1
    {18,  2},    //  8 CURRENT_TM2
    {20,  2},    //  9 CURRENT_TM3
    {22,  2},    // 10 CURRENT_TM4
    {24,  2},    // 11 CURRENT_TM5
    {26,  2},    // 12 CURRENT_TM6
    {28,  2},    // 13 Voffset_N
    {30,  2}};   // 14 Vgain_N

  _b ccs_tm[] = {
    { 0, 0xFFFFFFFF, -1},
    { 1, 0x00000FFF, -1},
    { 2, 0x00000FFF, -1},
    { 3, 0x00000FFF, -1},
    { 4, 0x00000FFF, -1},
    { 5, 0x00000FFF, -1},
    { 6, 0x00000FFF, -1},
    { 7, 0x00000FFF, -1},
    { 8, 0x00000FFF, -1},
    { 9, 0x00000FFF, -1},
    {10, 0x00000FFF, -1},
    {11, 0x00000FFF, -1},
    {12, 0x00000FFF, -1},
    {13, 0x00000FFF, -1},
    {14, 0x00000FFF, -1}};

//~----------------------------------------------------------------------------
//
//~--  DLCM TM
//

_sta dlcm_tm;

//~----------------------------------------------------------------------------
//
//~--  CSP Commands
//

  _b csp_ala_sum[] = {
    { 0, 0xFF, 3},
    { 1, 0xFF, 2},
    { 2, 0xFF, 2},
    { 3, 0xFF, 2},
    { 4, 0xFF, 2},
    { 5, 0xFF, 2},
    { 6, 0xFF, 2}};

//~============================================================================

#endif // _CABDEF_H
