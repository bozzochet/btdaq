// file cablib.c
//
// A.Lebedev - Jul 2008...
//

#include "cablib.h"

//~============================================================================
//
//                 TABLES
//
//~============================================================================

// taken from the book:
//   "AMS-02 Cryomagnet Avionics Box Software Interface Control Document (SICD)"
//   Doc. No.: CAB-ICD-CRS-0016  Issue: 4.1  Date: 19 Dec.07  Page: 23

char *cab_cmd_name(int cmd_id) {

  int i;
  static struct {
    char *txt;
    int   cod;
  } table[] = {
// GENERAL CAB COMMANDS
    {"CAB_RESET_SS",                       0x11},
    {"CAB_SERIAL_RESET_MODULE",            0x12},
    {"CAB_RESET_SW",                       0x14},
    {"CAB_LOAD_APSW",                      0x18},
    {"CAB_GO_APSW",                        0x1B},
    {"CAB_READ_AND_RESET_ERROR_INFO",      0x1E},
    {"CAB_GET_ESSENTIAL_STATUS",           0x03},
    {"CAB_GET_DIGITAL_STATUS",             0x05},
    {"CAB_PATCH_MEMORY",                   0x06},
    {"CAB_DUMP_MEMORY",                    0x09},
    {"CAB_LATCHUP_PROTECT",                0x0A},
    {"CAB_ON_OFF_LCL_BATT",                0xB8},
// STM COMMANDS
    {"STM_ACQUIRE",                        0x21},
    {"STM_READ",                           0x22},
    {"STM_ACQUIRE_COLD_VALVE_STATUS",      0x24},
    {"STM_READ_COLD_VALVE_STATUS",         0x27},
// PTM COMMANDS
    {"PTM_START_ACQ_SEQ",                  0x28},
    {"PTM_READ",                           0x2B},
    {"PTM_CALIBRATION",                    0x2D},
// DLCM COMMANDS
    {"DLCM_START_ACQ_SEQ",                 0x33},
    {"DLCM_READ_SLICE",                    0x35},
    {"DLCM_CANCEL_SEQ",                    0x36},
// CCS COMMANDS
    {"CCS_CURRENT_SETPOINT",               0x41},
    {"CCS_START_CURRENT_SETPOINT_SEQ",     0x39},
    {"CCS_CANCEL_CURRENT_SETPOINT_SEQ",    0x42},
    {"CCS_READ_SETPOINT_CMDS",             0x44},
    {"CCS_ ANALOG_TM_ACQUIRE",             0x47},
    {"CCS_READ_ANALOG_TM",                 0x48},
    {"CCS_OPEN_CLOSE_SEMIC_SW",            0x4B},
    {"CCS_ENA_DIS_OUTPUT",                 0x4D},
    {"CCS_ON_OFF_DC_DC_CONVERTERS",        0x4E},
    {"CCS_COMMANDING_ARM_DISARM",          0x53},
// PS COMMANDS
    {"PS_ENA_DIS_HT_HEATER_GROUP",         0x71},
    {"PS_ENA_DIS_VACUUM_SENSOR",           0x72},
    {"PS_ENA_DIS_PULSED_PILOT_VALVES",     0x74},
    {"PS_OPERATE_PULSED_PILOT_VALVE_SEQ",  0x77},
    {"PS_ENA_DIS_BILEVEL_PILOT_VALVES",    0x78},
    {"PS_OPEN_CLOSE_BILEVEL_PILOT_VALVE",  0x7B},
    {"PS_ENA_DIS_MOOG_LATCHING_VALVES",    0x79},
    {"PS_OPERATE_MOOG_LATCHING_VALVE",     0x7C},
    {"PS_ON_OFF_CV",                       0x7D},
    {"PS_INTERPOINT_DRIVER_ON_OFF",        0x7E},
// CSP COMMANDS
    {"CSP_RDS_CONFIGURATION",              0x93},
    {"CSP_AQS_CONFIGURATION",              0x95},
    {"CSP_GET_RDS_AQS_CONFIGURATION",      0xCC},
    {"CSP_SECTION_DELAY_CONFIGURATION",    0xF7},
    {"CSP_WD_COUNT_DOWN_PROGRAMMING",      0x96},
    {"CSP_WD_REFRESH",                     0x99},
    {"CSP_WD_ARM_DISARM",                  0x9A},
    {"CSP_ GET_WD_COUNT",                  0x9C},
    {"CSP_ENA_DIS_DETECTION_FUNCTION",     0x9F},
    {"CSP_ARM_DISARM_AUTO_RDS",            0xB1},
    {"CSP_INITIATE_AUTO_RDS",              0xB2},
    {"CSP_ARM_DISARM_AUTO_RDS_CANCEL",     0xCA},
    {"CSP_INITIATE_AUTO_RDS_CANCEL",       0xB4},
    {"CSP_ON_OFF_CONVERTERS",              0xB7},
    {"CSP_ARM_QUENCH_SEQ",                 0xBB},
    {"CSP_FIRE_QUENCH_SEQ",                0xBD},
    {"CSP_RESET_ALARM",                    0xBE},
    {"CSP_GET_ALARM_SUMMARY",              0xC3},
    {"CSP_GET_GENERAL_STATUS",             0xC5},
    {"CSP_GET_DETECTOR_STATUS",            0xC6},
    {"CSP_GET_VALVES_STATUS",              0xC9},
    {"CSP_ARM_DISARM_HEATER",              0xE1},
    {"CSP_OPERATE_HEATER",                 0xE2},
    {"CSP_ARM_DISARM_BILEVEL_PILOT_VALVE", 0xE4},
    {"CSP_OPERATE_BILEVEL_PILOT_VALVE",    0xE7},
    {"CSP_ARM_DISARM_PULSED_PILOT_VALVE",  0xE8},
    {"CSP_OPERATE_PULSED_PILOT_VALVE",     0xEB},
    {"CSP_SEL_CLEAR_ARM_SEQ",              0xF3},
    {"CSP_SEL_CLEAR_FIRE_SEQ",             0xF5},
    {"CSP_ARM_DISARM_MOOG_VALVE",          0xED},
    {"CSP_OPERATE_MOOG_VALVE",             0xEE},
    {"CSP_LINE_DRIVER_ARM",                0xF9},
    {"CSP_LINE_DRIVER_OPERATE",            0xFB}};

  static char *error = {"UNKNOWN_CMD_CODE"};

  for (i=0; i<DIM(table); i++) if (table[i].cod == cmd_id) return table[i].txt;

  return error;
}

//~----------------------------------------------------------------------------
// taken from the book:
//   "AMS-02 Cryomagnet Avionics Box Software Interface Control Document (SICD)"
//   Doc. No.: CAB-ICD-CRS-0016  Issue: 4.1  Date: 19 Dec.07  Page: 102

char *cab_ack_message(int errnum) {

  int i;
  static struct {
    char *txt;
    int   cod;
  } table[] = {
    {"Command OK",                           0x00},
    {"Error in Command Parameter 1",         0x01},
    {"Error in Command Parameter 2",         0x02},
    {"Error in Command Parameter 3",         0x03},
    {"Error in Command Parameter 4",         0x04},
    {"Error in Command Parameter 5",         0x05},
    {"Error in Command Parameter 6",         0x06},
    {"Error in Command Parameter 7",         0x07},
    {"Error in Command Parameter 8",         0x08},
    {"Error in Command CRC",                 0xF7},
    {"Error in CAN Block Header",            0xF8},
    {"Patch not allowed",                    0xF9},
    {"Command not allowed by HW",            0xFA},
    {"Command not allowed now",              0xFB},
    {"Command not allowed in Current State", 0xFC},
    {"Command Length out of Limits",         0xFD},
    {"Error in Command Length",              0xFE},
    {"Unknown Command",                      0xFF}};

  static char *error = {"UNKNOWN_ACK_CODE"};

  for (i=0; i<DIM(table); i++) if (table[i].cod == errnum) return table[i].txt;

  return error;
}

//~----------------------------------------------------------------------------
// taken from the book:
//   "AMS-02 Cryomagnet Avionics Box User Manual"
//   Doc. No.:	CAB-MA-CRS-0123  Issue:	1 Date:	06 Jun.08   Page: 30...

char *cab_err_message(int errnum) {

  int i;
  static struct {
    char *txt;
    int   cod;
  } table[] = {
    {"NO_ERROR",                                       0x00},
    {"ERR_D_SERIAL_ERROR_PARITY",                      0x0A},
    {"ERR_D_CANBUS_FAKE_FRAME",                        0x0B},
    {"ERR_D_SERIAL_ERROR_RI_SET_BEFORE_TX",            0x0C},
    {"ERR_D_SERIAL_ERROR_RX_TOO_LONG",                 0x0D},
    {"ERR_D_DIRECT_SET_CLEAR_CMD_NOT_ENABLED",         0x10},
    {"ERR_D_DIRECT_SET_CLEAR_CMD_CMD_ID",              0x11},
    {"ERR_D_DIRECT_SET_BILEVEL_CMD_ID",                0x12},
    {"ERR_D_DIRECT_GET_STAT_STAT_ID",                  0x13},
    {"ERR_D_DIRECT_GET_STAT_BYTE_STAT_ID",             0x14},
    {"ERR_MAIN_MEM_ADDR_ERR",                          0x20},
    {"ERR_MAIN_SW_CYCLE_LOST",                         0x21},
    {"ERR_MAIN_SW_CYCLE_DURATION",                     0x22},
    {"ERR_MAIN_ARDQ_NOM_RED",                          0x23},
    {"ERR_MAIN_ARDQ_NOM",                              0x24},
    {"ERR_MAIN_ARDQ_RED",                              0x25},
    {"ERR_SUINIT_ANALRESET_LATCH_UP_FLAG_NOT_CLEARED", 0x30},
    {"ERR_SUINIT_ANALCPUNOMRED_CPU_UNDEFINED",         0x31},
    {"ERR_RAMTEST_TEST_ADDRESSING",                    0x32},
    {"ERR_RAMTEST_TEST_PATTERN",                       0x33},
    {"ERR_CRCTEST",                                    0x34},
    {"ERR_PATCH_PROC_MEM_ID_INVALID",                  0x41},
    {"ERR_DUMP_PROC_MEM_ID_INVALID",                   0x42},
    {"ERR_PATCH_PROC_MEM_TYPE_CANNOT_BE_PATCHED",      0x43},
    {"ERR_D_EEPROM_PORT_EEPROM_READY_REMAINS_ACTIVE",  0x44},
    {"ERR_RESET_PERFORM_RESET_FAILED",                 0x45},
    {"ERR_CMDEX_SEU_INV_CMD",                          0x50},
    {"ERR_CMDEX_SEU_INV_SLICE",                        0x51},
    {"ERR_CMDEX_INVALID_PARAM_TYPE",                   0x52},
    {"ERR_SEQEX_SEQ_ID",                               0x53},
    {"ERR_SCOMM_FPGA_ID",                              0x60},
    {"ERR_SCOMM_CTS_ACTIVE_BEFORE_MASTER_TX",          0x61},
    {"ERR_SCOMM_STAT_BYTE_FPGA",                       0x62},
    {"ERR_SCOMM_STAT_BYTE_BITS",                       0x63},
    {"ERR_SCOMM_TX_TIMEOUT",                           0x64},
    {"ERR_SCOMM_RX_TIMEOUT",                           0x65},
    {"ERR_SCOMM_RECV_NUM_BYTES_UNEXPECTED",            0x66},
    {"ERR_SCOMM_RECV_WRONG_PARITY",                    0x67},
    {"ERR_SCOMM_FPGA_ID_BROAD",                        0x68},
    {"ERR_CBCOMM_LINK_STATE_ERROR",                    0x70},
    {"ERR_CBCOMM_NO_TRANSAC_STARTED",                  0x71},
    {"ERR_CBCOMM_PREV_TRANSAC_DISCARDED",              0x72},
    {"ERR_CBCOMM_UNKNOWN_FRAME",                       0x73},
    {"ERR_CBCOMM_JMDC_ABORT_FRAME",                    0x74},
    {"ERR_CBCOMM_JMDC_ERR_FRAME",                      0x75},
    {"ERR_CBCOMM_FRAME_OVERWRITE",                     0x76},
    {"ERR_CBCOMM_JMDC_WRITE_FRAME",                    0x77},
    {"ERR_TMTC_START_CONV_ANA_CH",                     0x80},
    {"ERR_TMTC_READ_ANA_CH",                           0x81},
    {"ERR_TMTC_GET_DIG8_CH",                           0x82},
    {"ERR_TMTC_WRITE_DIG8_CH",                         0x83},
    {"ERR_TMTC_GET_DIG16_CH",                          0x84},
    {"ERR_TMTC_WRITE_DIG16_CH",                        0x85},
    {"ERR_TMTC_RESET_FPGA",                            0x86},
    {"ERR_TMTC_GET_AND_VAL_DIG8_CH",                   0x87},
    {"ERR_TMTC_GET_CHAIN_IX",                          0x88},
    {"ERR_TMTC_GET_SUBCHAIN_IX",                       0x89},
    {"ERR_TMTC_HIGH_LEVEL_CMD",                        0x8A},
    {"ERR_TMTC_DIRECT_PULSE",                          0x8B},
    {"ERR_TMTC_WRITE_DIG8_CH_BROAD",                   0x8C},
    {"ERR_CCS_GET_TM_INFO_INV_CHANNEL",                0x90},
    {"ERR_CCS_GET_SETPOINT_INV_ADDR",                  0x91},
    {"ERR_CCS_SET_SETPOINT_INV_ADDR",                  0x92},
    {"ERR_CCS_CMM_CONV_INV_ADDR",                      0x93},
    {"ERR_CCS_CMM_SEMIC_SW",                           0x94},
    {"ERR_CCS_CMM_OUTPUT",                             0x95},
    {"ERR_CCSPROG_CRC_SETPOINT",                       0xA0},
    {"ERR_CCSPROG_INCONSISTENT_SETPOINTS",             0xA1},
    {"ERR_CCSPROG_FPGA_AND_PROG_SETPOINT_DIFFER",      0xA2},
    {"ERR_CCSPROG_STOPPED",                            0xA3},
    {"ERR_CCSPROG_IX_SLICE",                           0xA4},
    {"ERR_STM_CMM_SW_ID_INV",                          0xB0},
    {"ERR_STM_GET_TM_INFO_INV_CHANNEL",                0xB1},
    {"ERR_PTM_CMM_SW_ID_INV",                          0xB2},
    {"ERR_PTM_GET_TM_INFO_INV_CHANNEL",                0xB3},
    {"ERR_PTMSEQ_PROC_SLICE_IX_SLICE",                 0xB4},
    {"ERR_CSP_SER_BROAD_WD_REFRESH_NOT_RECV",          0xC1},
    {"ERR_CSP_SER_BROAD_QUENCH_FIRE_NOT_RECV",         0xC0},
    {"ERR_CSP_SER_BROAD_AUTO_RDS_NOT_RECV",            0xC2},
    {"ERR_CSP_SER_BROAD_MOOG_VALVE_OPER_NOT_RECV",     0xC3},
    {"ERR_CSPQSEQ_STOPPED",                            0xC4},
    {"ERR_CSPCL_STOPPED",                              0xC5},
    {"ERR_PS_ENA_DIS_GROUP",                           0xD0},
    {"ERR_PS_TMTC_ENA_DIS_GROUP",                      0xD1}};

  static char *error = {"UNKNOWN_ERROR_CODE"};

  for (i=0; i<DIM(table); i++) if (table[i].cod == errnum) return table[i].txt;

  return error;
}

//~============================================================================
//
//                 CONVERTERS
//
//~============================================================================

char *cab_cpu_time(int8* sta) {

  static char b[80];

  sprintf(b,"%.1f", 0.0125*(((sta[0]*256+sta[1])*256+sta[2])*256+sta[3]));
  
  return b;
}

//~============================================================================

void convert_0x50_0x5F(int32 code, int32* value) {

  *value = 2;
  if (code == 0x50) *value = 0;
  if (code == 0x5F) *value = 1;
}

//~-------------------------------------------------------------------------------

void convert_0x00_0xFF(int32 code, int32* value) {

  *value = 2;
  if (code == 0x00) *value = 0;
  if (code == 0xFF) *value = 1;
}

//~============================================================================

real convert_cernox_to_T(real R, struct _cernox *Table) {

  int i;
/*
  real x = 1.0 / R, x1 = 0, x2 = 0, y = 0, y1 = 0, y2 = 0;
*/
  real x = 1.0 / R, x1 = 0, x2 = 0, y1 = 0, y2 = 0;

  for (i=0; i<Table->n-1; i++) {
    x1 = 1.0 / Table->_[i  ].R;
    x2 = 1.0 / Table->_[i+1].R;
    y1 =       Table->_[i  ].T;
    y2 =       Table->_[i+1].T;
/*
    if (x < x1 || (x1 <= x && x < x2)) break;
*/
    if (x < x1)            return 0.0;
    if (x1 <= x && x < x2) return (y1 + (y2 - y1) / (x2 - x1) * (x - x1));
  }

  return 999.999;
/*
  y = y1 + (y2 - y1) / (x2 - x1) * (x - x1);
  return MAX(0, y);
*/
}

//~----------------------------------------------------------------------------

real convert_PTM_data(
  _sta *p, int Dm, real Doffset_m, real D_Vref1, real D_Vref2, real D_Vref3) {

//~---

  real Rt, Vref;
  real K_RANGE[] = {
    6.5196903786e-3,  // Superfluid Cooling Loop (100K)
    3.2533124334e-3,  // Persistent Switch       (200K)
    2.3256610970e-3,  // Cool Down Circuit & VCS (300K)
    1.9206302310e-3}; // Magnet Current Leads    (400K)
  real K1  = 107.4113856069;
  real K2  = 100.0;
  real K3  = 1.2462061251e-2;
  real KK1 = 53.7056928034;
  real KK2 = 50.0;
  real KK3 = 6.7985197536e-3;
  
//~---

  switch (p->F) {

    case 0: 
    break;

    case 1:
      Rt = (2.0 * D_Vref2 / 65536.0 - 1.0) * K_RANGE[p->K]
         / (2.0 * D_Vref1 / 65536.0 - 1.0) 
         / (((2.0 * (Dm - Doffset_m + 13002.1322645) / 65536.0) - 1.0)
         / 30100.0 + 1.0 / 49900.0);
    return Rt;

    case 2:
      Rt = (2.0 * D_Vref1 / 65536.0 - 1.0) / (2.0 * D_Vref2 / 65536.0 - 1.0) 
         * (K1  * (2.0 * (Dm - Doffset_m + 2260.992) / 65536.0 - 1.0) + K2 )
         / K3;
    return Rt;

    case 3:
      Rt = (2.0 * D_Vref1 / 65536.0 - 1.0) / (2.0 * D_Vref3 / 65536.0 - 1.0) 
         * (K1  * (2.0 * (Dm - Doffset_m + 2260.992) / 65536.0 - 1.0) + K2 )
         / K3;
    return Rt;

    case 4:
      Rt = (2.0 * D_Vref1 / 65536.0 - 1.0) / (2.0 * D_Vref3 / 65536.0 - 1.0) 
         * (KK1 * (2.0 * (Dm - Doffset_m + 2260.992) / 65536.0 - 1.0) + KK2)
         / KK3;
    return Rt;

    case 5:
      Vref = Dm * 10.0 / 65536.0 - 5.0;
    return Vref;
  }

  return 0xDEADBEAF;
}

//~============================================================================
//
//                 PRINTERS
//
//~============================================================================

void sprint_CAB_essential_status(int8* sta, int *n, char b[][80]) {

  volatile int i = 0;        // to avoid "optimization" problems
  int k;
  
//~--

  k = sprintf(&b[i][0],"CPU_ID               = %02hhX ", sta[0]);
  if      (sta[0] == 0x01) sprintf(&b[i++][0]+k,"(Nominal CPU)");
  else if (sta[0] == 0x02) sprintf(&b[i++][0]+k,"(Redundant CPU)");
  else                     sprintf(&b[i++][0]+k,"(?)");

  sprintf(&b[i++][0],"RESET_CAUSE          = %02hhX: ", sta[1]);
  if (sta[1] & 0x01) sprintf(&b[i++][0],"  Power_ON_Reset");
  if (sta[1] & 0x02) sprintf(&b[i++][0],"  Watchdog_Reset");
  if (sta[1] & 0x04) sprintf(&b[i++][0],"  Latch_UP_Reset");
  if (sta[1] & 0x08) sprintf(&b[i++][0],"  Commanded_JMDC_Reset to Start-up");
  if (sta[1] & 0x10) sprintf(&b[i++][0],"  Invalid_Memory_Access_Reset)");
  if (sta[1] & 0x20) sprintf(&b[i++][0],"  Commanded_JMDC_Reset to Active");
  if (sta[1] & 0xC0) sprintf(&b[i++][0],"  ?");

  k = sprintf(&b[i][0],"SW_STATE_CURRENT_CPU = %02hhX ", sta[2]);
  if      (sta[2] == 0x01) sprintf(&b[i++][0]+k,"(Start-Up)");
  else if (sta[2] == 0x02) sprintf(&b[i++][0]+k,"(Active)");
  else                     sprintf(&b[i++][0]+k,"(?)");

  k = sprintf(&b[i][0],"SW_STATE_OTHER_CPU   = %02hhX ", sta[3]);
  if      (sta[3] == 0x01) sprintf(&b[i++][0]+k,"(Start-Up)");
  else if (sta[3] == 0x02) sprintf(&b[i++][0]+k,"(Active)");
  else                     sprintf(&b[i++][0]+k,"(?)");

  sprintf(&b[i++][0],"SW_SEQ_IN_COURSE     = %02hhX:", sta[4]);
  sprintf(&b[i++][0],"  PTM_Acquisition              %s", (sta[4] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  DLCM_Acquisition             %s", (sta[4] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Current_SetPoint_Programming %s", (sta[4] & 0x04) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Arm_Quench                   %s", (sta[4] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Current_Lead_SEL_Clear       %s", (sta[4] & 0x10) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Magnet_Detector_SEL_Clear    %s", (sta[4] & 0x20) ? "ON" : "OFF");

  sprintf(&b[i++][0],"SUSW_VERSION         = %02hhX", sta[5]);
  sprintf(&b[i++][0],"APSW_VERSION         = %02hhX", sta[6]);

  sprintf(&b[i++][0],"SW_CYCLE_COUNTER     = %02hhX%02hhX%02hhX%02hhX (%ssec)", 
          sta[7], sta[8], sta[9], sta[10], cab_cpu_time(&sta[7]));

//~--

  sprintf(&b[i++][0],"ERROR_COUNTER        = %d", sta[11]*8+sta[12]);
  sprintf(&b[i++][0],"LAST_ENTRY_ERROR_TABLE:");
  sprintf(&b[i++][0],"  Time  = %ssec", cab_cpu_time(&sta[14]));
  k = sprintf(&b[i][0],"  Code  = %02hhX ", sta[13]);
  sprintf(&b[i++][0]+k,"(%s)", cab_err_message(sta[13]));
  sprintf(&b[i++][0],"  Param = %02hhX %02hhX", sta[18], sta[19]);

  sprintf(&b[i++][0],"CMD_REJECTED_COUNTER = %d", sta[20]*8+sta[21]);

  sprintf(&b[i++][0],"CAN_0_STATUS         = %02hhX", sta[22]);
  sprintf(&b[i++][0],"CAN_0_TX_STATUS      = %02hhX", sta[23]);
  sprintf(&b[i++][0],"CAN_0_RX_STATUS      = %02hhX", sta[24]);

  sprintf(&b[i++][0],"CAN_1_STATUS         = %02hhX", sta[25]);
  sprintf(&b[i++][0],"CAN_1_TX_STATUS      = %02hhX", sta[26]);
  sprintf(&b[i++][0],"CAN_1_RX_STATUS      = %02hhX", sta[27]);

//~--

  sprintf(&b[i++][0],"PROM_CRC_EXP, CALC         = %02hhX%02hhX %02hhX%02hhX",
          sta[28], sta[29], sta[30], sta[31]);
  sprintf(&b[i++][0],"SW_PROGRAM_1_CRC_EXP, CALC = %02hhX%02hhX %02hhX%02hhX",
          sta[32], sta[33], sta[34], sta[35]);
  sprintf(&b[i++][0],"SW_PROGRAM_2_CRC_EXP, CALC = %02hhX%02hhX %02hhX%02hhX",
          sta[36], sta[37], sta[38], sta[39]);
  sprintf(&b[i++][0],"SW_TABLE_CRC_EXP, CALC     = %02hhX%02hhX %02hhX%02hhX",
          sta[40], sta[41], sta[42], sta[43]);
  sprintf(&b[i++][0],"PROGRAM_RAM_CRC_CALC       =      %02hhX%02hhX",
          sta[44], sta[45]);
  sprintf(&b[i++][0],"RAM_TEST = %02hhX:", sta[46]);
  sprintf(&b[i++][0],"  Data RAM result:    %s", (sta[46] & 0x01) ? "OK" : "Error");
  sprintf(&b[i++][0],"  Program RAM result: %s", (sta[46] & 0x02) ? "OK" : "Error");

//~--

  k = sprintf(&b[i][0],"CCSC_OTHER_DCDC_STATUS    = %02hhX ", sta[47]);
  if      (sta[47] == 0x5F) sprintf(&b[i++][0]+k,"(Converter is ON)");
  else if (sta[47] == 0x50) sprintf(&b[i++][0]+k,"(Converter is OFF)");
  else                      sprintf(&b[i++][0]+k,"(?)");

  k = sprintf(&b[i][0],"LATCHUP_PROTECTION_STATUS = %02hhX ", sta[48]);
  if      (sta[48] == 0x5F) sprintf(&b[i++][0]+k,"(ENABLED)");
  else if (sta[48] == 0x50) sprintf(&b[i++][0]+k,"(DISABLED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

//~--

  k = sprintf(&b[i][0],"PS_NCV_STATUS = %02hhX ", sta[49]);
  if      (sta[49] == 0x5F) sprintf(&b[i++][0]+k,"(OPEN)");
  else if (sta[49] == 0x50) sprintf(&b[i++][0]+k,"(CLOSED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

  k = sprintf(&b[i][0],"PS_RCV_STATUS = %02hhX ", sta[50]);
  if      (sta[50] == 0x5F) sprintf(&b[i++][0]+k,"(OPEN)");
  else if (sta[50] == 0x50) sprintf(&b[i++][0]+k,"(CLOSED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

//~--

  sprintf(&b[i++][0],"CSP_CV_STATUS  = %02hhX:", sta[51]);
  sprintf(&b[i++][0],"  CSP Magnet Detector CV 1 Status: %s",
         (sta[51] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  CSP Magnet Detector CV 2 Status: %s",
         (sta[51] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  CSP Magnet Detector CV 3 Status: %s",
         (sta[51] & 0x04) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  CSP Current Leads CV 1 Status:   %s",
         (sta[51] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  CSP Current Leads CV 2 Status:   %s",
         (sta[51] & 0x10) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  CSP Current Leads CV 3 Status:   %s",
         (sta[51] & 0x20) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  CSP Nominal Control CV Status:   %s",
         (sta[51] & 0x40) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  CSP Redundant Control CV Status: %s",
         (sta[51] & 0x80) ? "ON" : "OFF");

  sprintf(&b[i++][0],"CSP_28V_STATUS = %02hhX:",  sta[52]);
  sprintf(&b[i++][0],"  28V ISO LCL Nominal Battery Charger Status:   %s",
         (sta[52] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  28V ISO LCL Redundant Battery Charger Status: %s",
         (sta[52] & 0x02) ? "ON" : "OFF");

  k = sprintf(&b[i][0],"CSP_ALARM      = %02hhX ", sta[53]);
  if      (sta[53] == 0xFF) sprintf(&b[i++][0]+k,"(ALARM DETECTED)");
  else if (sta[53] == 0x00) sprintf(&b[i++][0]+k,"(NO ALARM DETECTED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

//~--

  k = sprintf(&b[i][0],"  CCS_SEMIC_SW_STATUS = %02hhX ", sta[54]);
  if      (sta[54] == 0x5F) sprintf(&b[i++][0]+k,"(OPEN)");
  else if (sta[54] == 0x50) sprintf(&b[i++][0]+k,"(CLOSED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

  k = sprintf(&b[i][0],"  CCS_OUTPUT_ENABLE   = %02hhX ", sta[55]);
  if      (sta[55] == 0x5F) sprintf(&b[i++][0]+k,"(ENABLED)");
  else if (sta[55] == 0x50) sprintf(&b[i++][0]+k,"(DISABLED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

//~--

  *n = i;
}

//~-------------------------------------------------------------------------------

void sprint_CAB_digital_status(int8* sta, int *n, char b[][80]) {

  volatile int i = 0;        // to avoid "optimization" problems
  int k;

//~--

  sprintf(&b[i++][0],
          "SW_CYCLE_COUNTER = %02hhX%02hhX%02hhX%02hhX (%ssec)", 
          sta[0], sta[1], sta[2], sta[3], cab_cpu_time(&sta[0])); 

//~--

  sprintf(&b[i++][0],"CSC_CONV_CMD_STATUS    = %02hhX", sta[4]);
  sprintf(&b[i++][0],"  Converter 1 Status: %s", (sta[4] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Converter 2 Status: %s", (sta[4] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Converter 3 Status: %s", (sta[4] & 0x04) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Converter 4 Status: %s", (sta[4] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Converter 5 Status: %s", (sta[4] & 0x10) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Converter 6 Status: %s", (sta[4] & 0x20) ? "ON" : "OFF");

  sprintf(&b[i++][0],"CURRENT_1_CMD_SETTINGS = %02hhX%02hhX", sta[ 5], sta[ 6]);
  sprintf(&b[i++][0],"CURRENT_2_CMD_SETTINGS = %02hhX%02hhX", sta[ 7], sta[ 8]);
  sprintf(&b[i++][0],"CURRENT_3_CMD_SETTINGS = %02hhX%02hhX", sta[ 9], sta[10]);

  k = sprintf(&b[i][0],"CCS_COMMANDING_STATUS  = %02hhX ", sta[11]);
  if      (sta[11] == 0x5F) sprintf(&b[i++][0]+k,"(ARMED)");
  else if (sta[11] == 0x50) sprintf(&b[i++][0]+k,"(DISARMED)");
  else                      sprintf(&b[i++][0]+k,"(?)");

//~--

  sprintf(&b[i++][0],"STM_STATUS = %02hhX", sta[12]);
  sprintf(&b[i++][0],"  COLD_VALVE_CMD_STS: %s", (sta[12] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  COLD_VALVE_SW_STS:  %s", (sta[12] & 0x02) ? "ON" : "OFF");

//~--

  sprintf(&b[i++][0],"PTM_STATUS = %02hhX", sta[13]);
  sprintf(&b[i++][0],"  ON_OFFSET_CAL2_CMD_STS: %s", (sta[13] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  ON_OFFSET_CAL3_CMD_STS: %s", (sta[13] & 0x02) ? "ON" : "OFF");

//~--

  sprintf(&b[i++][0],"PS_CMD_SW_STATUS = %02hhX", sta[14]);
  sprintf(&b[i++][0],"  28V_ISO_CMD:         %s", (sta[14] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Vacuum Sensor:       %s", (sta[14] & 0x02) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"  Helium Tank Heater:  %s", (sta[14] & 0x04) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"  Pressure Sensor:     %s", (sta[14] & 0x08) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"  Status Conditioning: %s", (sta[14] & 0x10) ? "POWERED" : "NOT POWERED");
  sprintf(&b[i++][0],"  Pilot DV55:          %s", (sta[14] & 0x20) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"  Pilot DV57:          %s", (sta[14] & 0x40) ? "OPENED" : "CLOSED");

  sprintf(&b[i++][0],"PS_A_STATUS      = %02hhX", sta[15]);
  sprintf(&b[i++][0],"  Moog A Group: %s", (sta[15] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Moog B Group: %s", (sta[15] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Moog C Group: %s", (sta[15] & 0x04) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Moog D Group: %s", (sta[15] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Moog DV20A:   %s", (sta[15] & 0x10) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"  Moog DV20B:   %s", (sta[15] & 0x20) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"  Moog DV20C:   %s", (sta[15] & 0x40) ? "OPENED" : "CLOSED");
  sprintf(&b[i++][0],"  Moog DV20D:   %s", (sta[15] & 0x80) ? "OPENED" : "CLOSED");

  sprintf(&b[i++][0],"PS_B_STATUS      = %02hhX", sta[16]);
  sprintf(&b[i++][0],"  Moog Deco[0]: %s", (sta[16] & 0x01) ? "1" : "0");
  sprintf(&b[i++][0],"  Moog Deco[1]: %s", (sta[16] & 0x02) ? "1" : "0");
  sprintf(&b[i++][0],"  Moog Deco[2]: %s", (sta[16] & 0x04) ? "1" : "0");
  sprintf(&b[i++][0],"  Moog Deco[3]: %s", (sta[16] & 0x08) ? "1" : "0");
  sprintf(&b[i++][0],"  Warm Deco[0]: %s", (sta[16] & 0x10) ? "1" : "0");
  sprintf(&b[i++][0],"  Warm Deco[1]: %s", (sta[16] & 0x20) ? "1" : "0");
  sprintf(&b[i++][0],"  Warm Deco[2]: %s", (sta[16] & 0x40) ? "1" : "0");
  sprintf(&b[i++][0],"  Warm Deco[3]: %s", (sta[16] & 0x80) ? "1" : "0");

  sprintf(&b[i++][0],"PS_C_STATUS      = %02hhX", sta[17]);
  sprintf(&b[i++][0],"  Warm ON Group:  %s", (sta[17] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Warm OFF Group: %s", (sta[17] & 0x02) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Warm DV09A:     %s", (sta[17] & 0x04) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"  Warm DV09B:     %s", (sta[17] & 0x08) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"  Warm DV11A:     %s", (sta[17] & 0x10) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"  Warm DV11B:     %s", (sta[17] & 0x20) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"  Warm DV16A:     %s", (sta[17] & 0x40) ? "CLOSED" : "NOT CLOSED");
  sprintf(&b[i++][0],"  Warm DV16B:     %s", (sta[17] & 0x80) ? "CLOSED" : "NOT CLOSED");

  sprintf(&b[i++][0],"PS_D_STATUS      = %02hhX", sta[18]);
  sprintf(&b[i++][0],"  DLCM & Vacuum Group:   %s", (sta[18] & 0x01) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Vacuum Sensor:         %s", (sta[18] & 0x02) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"  Helium Tank Heater:    %s", (sta[18] & 0x04) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"  Pressure Sensor Group: %s", (sta[18] & 0x08) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  Pressure Sensor:       %s", (sta[18] & 0x10) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"  PCold Group:           %s", (sta[18] & 0x20) ? "ON" : "OFF");
  sprintf(&b[i++][0],"  uSW Energization:      %s", (sta[18] & 0x40) ? "ENERGIZED" : "NOT ENERGIZED");
  sprintf(&b[i++][0],"  PS_CV:                 %s", (sta[18] & 0x80) ? "ON" : "OFF");

//~--

  *n = i;
}

//~============================================================================
//
//                 GENERAL CAB COMMANDS
//
//~============================================================================

void CAB_reset_SW(int16 target, int16* err) {

  int8 cmd[5] = {0x0A, 0x14, 5};
  
  if ((target != 0x8008) && (target != 0x0220)) {
    *err = 0x0100;
    return;
  }

  cmd[3] = (target >> 8) & 0x00FF;
  cmd[4] = (target >> 0) & 0x00FF;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CAB_load_APSW(int8 target, int16* err) {

  int8 cmd[4] = {0x0A, 0x18, 4};
  
  if ((target != 0xA2) && (target != 0xA4)) {
    *err = 0x0100;
    return;
  }

  cmd[3] = target;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CAB_go_APSW(int32 adr, int16* err) {

  int8 cmd[7] = {0x0A, 0x1B, 7};
  
  cmd[3] = (adr >> 24) & 0x000000FF;
  cmd[4] = (adr >> 16) & 0x000000FF;
  cmd[5] = (adr >>  8) & 0x000000FF;
  cmd[6] = (adr >>  0) & 0x000000FF;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CAB_read_and_reset_error_info(_cab_err *cab_err, int16* err) {
  
  int i, j;
  int8 dat[256];
  int8 cmd[3] = {0x0A, 0x1E, 3};

  exec_CAB_command(cmd, NULL, dat, err);

  if (*err) return;
  
  cab_err->n = dat[0];

  if (cab_err->n > 30) {
    *err = 0x0100;
    return;
  }
  
  for (i=0,j=1; i<cab_err->n; i++,j+=7) {
    cab_err->cod[i] =                  dat[j+0];
    cab_err->tim[i] = packbytes(NULL, &dat[j+1], 4);
    cab_err->par[i] = packbytes(NULL, &dat[j+5], 2);
  }
}

//~----------------------------------------------------------------------------

void CAB_get_essential_status(int8* sta, int16* err) {

  int8 cmd[3] = {0x0A, 0x03, 3};

  exec_CAB_command(cmd, NULL, sta, err);
}

//~----------------------------------------------------------------------------

void CAB_get_digital_status(int8 cha, int8* sta, int16* err) {

  int8 cmd[4] = {0x0A, 0x05, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, sta, err);
}

//~----------------------------------------------------------------------------

void CAB_latch_up_protect(int8 ctrl, int16* err) {

  int8 cmd[4] = {0x0A, 0x0A, 4};

  cmd[3] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CAB_on_off_LCL_battery(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0xB8, 5};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~============================================================================
//
//                 STM (Standard TeleMetry) COMMANDS
//
//~============================================================================

void STM_acquire(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0x21, 5};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void STM_read(int8 cha, int8* dat, int16* err) {

  int8 cmd[4] = {0x0A, 0x22, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, dat, err);
}

//~----------------------------------------------------------------------------

void STM_acquire_cold_valve_status(int8 cha, int16* err) {

  int8 cmd[4] = {0x0A, 0x24, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void STM_read_cold_valve_status(int8 cha, int8* dat, int16* err) {

  int8 cmd[4] = {0x0A, 0x27, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, dat, err);
}

//~============================================================================
//
//                 PTM (Precision TeleMetry) COMMANDS
//
//~============================================================================

void PTM_start_acq_seq(int8 cha, int16* err) {

  int8 cmd[4] = {0x0A, 0x28, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PTM_read(int8 cha, int8* dat, int16* err) {

  int8 cmd[4] = {0x0A, 0x2B, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, dat, err);
}

//~----------------------------------------------------------------------------

void PTM_calibration(int8 cha, int16* err) {

  int8 cmd[4] = {0x0A, 0x2D, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~============================================================================
//
//                 DLCM (Direct Liquid Content Measurement) COMMANDS
//
//~============================================================================

void DLCM_read_slice(int8* dat, int16* err) {

  int8 cmd[6] = {0x0A, 0x35, 6, 0, 1, 10};

  exec_CAB_command(cmd, NULL, dat, err);
}

//~============================================================================
//
//                 CCS (Cryo-magnet Current Source) COMMANDS
//
//~============================================================================

void CCS_current_setpoint_seq(int8 ID, int16 value, int16* err) {

  int8 cmd[6] = {0x0A, 0x41, 6};

  cmd[3] = ID;
  cmd[4] = value >> 8;
  cmd[5] = value >> 0;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CCS_start_current_setpoint_seq(int8 velocity, int16* err) {

  int8 cmd[4] = {0x0A, 0x39, 4};

  cmd[3] = velocity;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CCS_cancel_current_setpoint_seq(int16* err) {

  int8 cmd[3] = {0x0A, 0x42, 3};

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CCS_read_setpoint_CMDS(int16* value, int16* err) {

  int i;
  int8 dat[256];
  int8 cmd[3] = {0x0A, 0x44, 3};

  exec_CAB_command(cmd, NULL, dat, err);

  if (!*err) {
    for (i=0; i<3; i++) value[i] = (((int16)dat[2*i]) << 8) + dat[2*i+1];
  }  
}

//~----------------------------------------------------------------------------

void CCS_analog_TM_acquire(int16* err) {

  int8 cmd[3] = {0x0A, 0x47, 3};

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CCS_read_analog_TM(int8* tm, int16* err) {

  int8 cmd[3] = {0x0A, 0x48, 3};

  exec_CAB_command(cmd, NULL, tm, err);
}

//~----------------------------------------------------------------------------

void CCS_open_close_semic_sw(int8 ctrl, int16* err) {

  int8 cmd[4] = {0x0A, 0x4B, 4};

  cmd[3] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CCS_ena_dis_ouput(int8 ctrl, int16* err) {

  int8 cmd[4] = {0x0A, 0x4D, 4};

  cmd[3] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CCS_on_off_DCDC_converters(int16 ctrl, int16* err) {

  int8 cmd[9] = {0x0A, 0x4E, 9};
  int i;
//                   OFF   ON    --    --
  int convert[4] = {0x50, 0x5F, 0x00, 0x00};

  for (i=0; i<6; i++) cmd[3+i] = convert[(ctrl>>(2*i))&0x03];

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CCS_commanding_arm_disarm(int8 ctrl, int16* err) {

  int8 cmd[4] = {0x0A, 0x53, 4};

  cmd[3] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~============================================================================
//
//                 PS (Power Switches) COMMANDS
//
//~============================================================================

void PS_ena_dis_HT_heater_group(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0x71, 5};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_ena_dis_vacuum_sensor(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0x72, 5};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_ena_dis_pulsed_pilot_valves(int8 cha, int8 ctrl, int8 grp, int16* err) {

  int8 cmd[6] = {0x0A, 0x74, 6};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;
  cmd[5] = grp ? 0x01 : 0x02;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_operate_pulsed_pilot_valves_seq(int8 cha, int8 valve, int8 duration, int16* err) {

  int8 cmd[7] = {0x0A, 0x77, 7};
  int16 code[] = {
    0x059A, 0x159A, 0x059B, 0x159B, 
    0x061A, 0x161A, 0x061B, 0x161B, 
    0x066A, 0x166A, 0x066B, 0x166B};

  if (valve > DIM(code)) {
    *err = 0x0100;
    return;
  }
  
  cmd[3] = cha + 1;
  cmd[4] = code[valve] >> 8;
  cmd[5] = code[valve] >> 0;
  cmd[6] = duration;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_ena_dis_bilevel_pilot_valves(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0x78, 5};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_open_close_bilevel_pilot_valve(int8 cha, int8 valve, int8 ctrl, int16* err) {

  int8 cmd[7] = {0x0A, 0x7B, 7};
  int16 code[] = {0x0055, 0x0057};

  if (valve > DIM(code)) {
    *err = 0x0100;
    return;
  }
  
  cmd[3] = cha + 1;
  cmd[4] = code[valve] >> 8;
  cmd[5] = code[valve] >> 0;
  cmd[6] = ctrl ? 0x5F : 0x50;   // OPEN/CLOSE VALVE, NOT SWITCH !!!!

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_ena_dis_moog_latching_valves(int8 cha, int8 ctrl, int8 grp, int16* err) {

  int8 cmd[6] = {0x0A, 0x79, 6};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;
  cmd[5] = grp + 1;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_operate_moog_latching_valve(int8 cha, int8 valve, int8 ctrl, int8 duration, int16* err) {

  int8 cmd[8] = {0x0A, 0x7C, 8};
  int16 code[] = {
    0x020A, 0x020B, 0x020C, 0x020D};

  if (valve > DIM(code)) {
    *err = 0x0100;
    return;
  }
  
  cmd[3] = cha + 1;
  cmd[4] = code[valve] >> 8;
  cmd[5] = code[valve] >> 0;
  cmd[6] = ctrl ? 0x5F : 0x50;   // OPEN/CLOSE VALVE, NOT SWITCH !!!!
  cmd[7] = duration;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_on_off_CV(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0x7D, 5};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void PS_interpoint_driver_on_off(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0x7E, 5};

  cmd[3] = cha + 1;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~============================================================================
//
//                 CSP (Cryo-magnet Self-Protection) COMMANDS
//
//~============================================================================

void CSP_on_off_converters(int8 cha, int8 ctrl, int16* err) {

  int8 cmd[5] = {0x0A, 0xB7, 5};

  cmd[3] = cha;
  cmd[4] = ctrl ? 0x5F : 0x50;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CSP_reset_alarm(int8 msk, int16* err) {

  int8 cmd[4] = {0x0A, 0xBE, 4};

  cmd[3] = msk;

  exec_CAB_command(cmd, NULL, NULL, err);
}

//~----------------------------------------------------------------------------

void CSP_get_alarm_summary(int8 cha, int8* sta, int16* err) {

  int8 cmd[4] = {0x0A, 0xC3, 4};

  cmd[3] = cha + 1;

  exec_CAB_command(cmd, NULL, sta, err);
}

//~============================================================================
