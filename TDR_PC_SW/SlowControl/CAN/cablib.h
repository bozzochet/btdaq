// file cablib.h
//
// see file cablib.c
//
// A.Lebedev - Jul-2008...

//~----------------------------------------------------------------------------

#ifndef _CABLIB_H
#define _CABLIB_H

#include "uscmlib.h"

typedef struct {
  int8  n;
  int32 tim[30];
  int8  cod[30];
  int16 par[30];
} _cab_err;

struct _cernox {
  char *nam;
  int n;
  struct {
    real T;
    real R;
  } _[1000];
};

typedef struct {
  int i;
  int n;
  int F;
  int K;
} _sta;

char *cab_cmd_name(int cmd_id);
char *cab_ack_message(int errnum);
char *cab_err_message(int errnum);

char *cab_cpu_time(int8* sta);
void convert_0x50_0x5F(int32 code, int32* value);
void convert_0x00_0xFF(int32 code, int32* value);

real convert_cernox_to_T(real R, struct _cernox *Table);
real convert_PTM_data(
  _sta *p, int Dm, real Doffset_m, real D_Vref1, real D_Vref2, real D_Vref3);

void sprint_CAB_essential_status(int8* sta, int *n, char b[][80]);
void sprint_CAB_digital_status(int8* sta, int *n, char b[][80]);

void CAB_reset_SW(int16 target, int16* err);
void CAB_load_APSW(int8 target, int16* err);
void CAB_go_APSW(int32 adr, int16* err);
void CAB_read_and_reset_error_info(_cab_err *cab_err, int16* err);
void CAB_get_essential_status(int8* sta, int16* err);
void CAB_get_digital_status(int8 cha, int8* sta, int16* err);
void CAB_latch_up_protect(int8 ctrl, int16* err);
void CAB_on_off_LCL_battery(int8 cha, int8 ctrl, int16* err);

void STM_acquire(int8 cha, int8 ctrl, int16* err);
void STM_read(int8 cha, int8* dat, int16* err);
void STM_acquire_cold_valve_status(int8 cha, int16* err);
void STM_read_cold_valve_status(int8 cha, int8* dat, int16* err);

void PTM_start_acq_seq(int8 cha, int16* err);
void PTM_read(int8 cha, int8* dat, int16* err);
void PTM_calibration(int8 cha, int16* err);

void DLCM_read_slice(int8* dat, int16* err);

void CCS_current_setpoint_seq(int8 ID, int16 value, int16* err);
void CCS_start_current_setpoint_seq(int8 velocity, int16* err);
void CCS_cancel_current_setpoint_seq(int16* err);
void CCS_read_setpoint_CMDS(int16* value, int16* err);
void CCS_analog_TM_acquire(int16* err);
void CCS_read_analog_TM(int8* tm, int16* err);
void CCS_open_close_semic_sw(int8 ctrl, int16* err);
void CCS_ena_dis_ouput(int8 ctrl, int16* err);
void CCS_on_off_DCDC_converters(int16 ctrl, int16* err);
void CCS_commanding_arm_disarm(int8 ctrl, int16* err);

void PS_ena_dis_HT_heater_group(int8 cha, int8 ctrl, int16* err);
void PS_ena_dis_vacuum_sensor(int8 cha, int8 ctrl, int16* err);
void PS_ena_dis_pulsed_pilot_valves(int8 cha, int8 ctrl, int8 grp, int16* err);
void PS_operate_pulsed_pilot_valves_seq(int8 cha, int8 valve, int8 duration, int16* err);
void PS_ena_dis_bilevel_pilot_valves(int8 cha, int8 ctrl, int16* err);
void PS_open_close_bilevel_pilot_valve(int8 cha, int8 valve, int8 ctrl, int16* err);
void PS_ena_dis_moog_latching_valves(int8 cha, int8 ctrl, int8 grp, int16* err);
void PS_operate_moog_latching_valve(int8 cha, int8 valve, int8 ctrl, int8 duration, int16* err);
void PS_on_off_CV(int8 cha, int8 ctrl, int16* err);
void PS_interpoint_driver_on_off(int8 cha, int8 ctrl, int16* err);

void CSP_on_off_converters(int8 cha, int8 ctrl, int16* err);
void CSP_reset_alarm(int8 msk, int16* err);
void CSP_get_alarm_summary(int8 cha, int8* sta, int16* err);

//~============================================================================

#endif // _CABLIB_H
