// file uscmlib.h
//
// See uscmlib.c
//
// A.Lebedev Nov-2003...
//

//~----------------------------------------------------------------------------

#ifndef _USCMLIB_H
#define _USCMLIB_H

//~----------------------------------------------------------------------------

#include "canlib.h"
#include "tcplib.h"
#include "easslib.h"

//~----------------------------------------------------------------------------

#define SERIAL_PORT_ENABLE      0x01
#define SERIAL_RX_ROLL          0x02
#define SERIAL_TX_ROLL          0x04

#define SERIAL_BAUD_RATE__2400  0
#define SERIAL_BAUD_RATE__4800  1
#define SERIAL_BAUD_RATE__9600  2
#define SERIAL_BAUD_RATE_19200  3

//~--

#define EPOCH_GATE_AUTO_OPEN 0x0001
#define COM_GATE_AUTO_OPEN   0x0002
#define COM_GATE_AUTO_CLOSE  0x0004

#define SEND_PPS             0x0001
#define OPEN_EPOCH_GATE      0x0002
#define CLOSE_EPOCH_GATE     0x0004
#define OPEN_COM_GATE        0x0008
#define CLOSE_COM_GATE       0x0010
#define RESET_GPS            0x0020

#define USCM_A_PORT_0_TO_GPS 0x0001
#define USCM_A_PORT_1_TO_GPS 0x0002
#define USCM_B_PORT_0_TO_GPS 0x0004
#define USCM_B_PORT_1_TO_GPS 0x0008

#define USCM_A_PORT_0_TO_AST 0x0010
#define USCM_A_PORT_1_TO_AST 0x0020
#define USCM_B_PORT_0_TO_AST 0x0040
#define USCM_B_PORT_1_TO_AST 0x0080

#define GPS_TO_USCM_A_PORT_0 0x0001
#define GPS_TO_USCM_A_PORT_1 0x0002
#define GPS_TO_USCM_B_PORT_0 0x0004
#define GPS_TO_USCM_B_PORT_1 0x0008

#define AST_TO_USCM_A_PORT_0 0x0010
#define AST_TO_USCM_A_PORT_1 0x0020
#define AST_TO_USCM_B_PORT_0 0x0040
#define AST_TO_USCM_B_PORT_1 0x0080

//~--

#define LDDR_OPERATION_MODE   0x0700
#define LDDR_PULSE_WIDTH      0x000F
#define LDDR_ENABLE_L0        0x0010
#define LDDR_POWER_L0         0x0020
#define LDDR_ENABLE_L1        0x0040
#define LDDR_POWER_L1         0x0080
#define LDDR_BIAS_L0          0x1000
#define LDDR_BIAS_L1          0x2000
#define LDDR_FIRE_INT_TRIGGER 0x0800
#define LDDR_POWER_L0_TRIP    0x4000
#define LDDR_POWER_L1_TRIP    0x8000

//~----------------------------------------------------------------------------

typedef struct {
  int8  lsb;
  int8  msb;
  int8  cfg;
  int8  remain;
  int8  perc;
  int8  temp1;
  int16 temp2;
  int16 age;
  bool  ok_short;
  float temp_short;
  bool  ok_long;
  float temp_long;
} _DS1820_data;

typedef struct {
  int8  cha;
  int16 thr;
  int8  pro;
  int8  par[8];
} __16ms_par;

typedef struct {
  bool  use;
  bool  ok;
  int16 age;
  int16 value[32];
} _CCBT_data;

//~----------------------------------------------------------------------------

//~--- Boot Commands ---

void reset_CPU(int16 *err);

void boot_from_OTP(int16 *err);
void boot_from_FLASH(int16 *err);
void boot_USCM(int8 from, int16 *err);

//~--- "System" Commands ---

void clear_power_FF(int16 *err);
void open_USCM_OTP_backdoor(int16 *err);
void close_USCM_OTP_backdoor(int16 *err);

//~--- USCM Software Debug Commands ---

void read_debug_printout(int siz, int *n, int8 *dat, int16 *err);
void reset_debug_printout(int16 *err);

//~--- Ping/Echo Command ---

void ping_USCM(int n, int8 *dat, int *nn, int8 *datdat, int16 *err);

//~--- Status Commands ---

void get_USCM_status(int8 which, int8 *dat, int16 *err);
void reset_USCM_status(int8 which, int16 *err);
void get_short_status(int8 *dat, int16 *err);
void reset_short_status(int16 *err);
void get_long_status(int8 *dat, int16 *err);
void reset_long_status(int16 *err);

void get_SXu_status(int8 *dat, int16 *err);
void reset_SXu_status(int16 *err);

//~--- Configuration File Commands ---

void read_config_log(int siz, int *n, int8 *dat, int16 *err);
void exec_config_file(int16 *err);

//~--- Console Commands (for HBB Software Only) ---

void read_console(int *n, int8 *dat, int16 *err);
void write_console(int n, int8 *dat, int16 *err);

//~--- Memory Commands ---

void read_USCM_memory(int n, int32 adr, int8 *dat, int16 *err);
void write_USCM_memory(int n, int32 adr, int8 *dat, int16 *err);

void start_FLASH_erasure(int8 sector, int16 *err);
void check_FLASH_erasure(bool *done, int16 *err);
void validate_FLASH(
     int32 begin, int32 *bgn, int32 *end, int16 *chk_1, int16 *chk_2, int16 *err);

//~--- Serial Ports Commands ---

void setup_serial_port(int8 port, int8  mode, int8  rate, int16 *err);
void check_serial_port(int8 port, int8 *mode, int8 *rate, int16 *err);

void enable_serial_port( int8 port, int16 *err);
void disable_serial_port(int8 port, int16 *err);
void flush_serial_port(int8 port, int16 *err);

void read_serial_port( int8 port, int siz, int *n, int8 *dat, int16 *err);
int  Read_serial_port( int8 port, int siz, int *n, int8 *dat, int16 *err);

void write_serial_port(int8 port, int  n, int8 *dat, int16 *err);
void Write_serial_port(int8 port, int  n, int8 *dat, int16 *err);
void write_then_read_serial_port(
     int8 port, int n_wri, int8 wri, int *n_rea, int8 *rea, int16 *err);

//~--- ADC & DAC Commands ---

void read_ADC(int16 *dat, int16 *err);
void read_DAC(int16 *dat, int16 *err);
void write_DAC(int16 *dat, int16 *err);

//~--- Dallas Temperature Sensors Commands ---

void start_DS1820_scan(int8 bus, int16 *err);
void check_DS1820_scan(int8 *bus, int8 *n, int16 *err);
void read_DS1820_table(int8 bus, int8 *n, int64 *ID, int8 *prio, int16 *err);
void write_DS1820_table(int8 bus, int8 n, int64 *ID, int8 *prio, int16 *err);

void setup_DS1820_readout(
     int8 ena, int8 rst, int8 man, int8 mdw, int8 par, int16 *err);
void check_DS1820_readout(
     int8 *ena, int8 *par, int8 *sta, int8 *man, int8 *mdw, int8 *mdr, int16 *err);

void read_DS1820_temp_short(
     int8 bus, int8 *n, bool *ok, float *tmp, int16 *err);
void read_DS1820_temp_long(
     int8 bus, int8 *n, bool *ok, float *tmp, int16 *age, int16 *err);
void read_DS1820_temp_raw(
     int8 bus, int8 *n, _DS1820_data *dat, int16 *err);

//~--- LVDS Bus Commands ---

void read_LVDS_bus(int8 adr, int16 *dat, int16 *err);
void Read_LVDS_bus(int8 adr, int n, int16 *dat, int16 *err);
void write_LVDS_bus(int8 adr, int16 dat, int16 *err);

//~--- LeCroy Commands ---

void init_LeCroy_bus(int8 bus, int16 *err);
void exec_LeCroy_cmd(
     int n, int8 *bus, int32 *wri, int32 *rea, bool *chk, int16 *err);

//~--- CCEB Specific Commands ---

void setup_CCBT_task(int8 ctl, int16 *err);
void read_CCBT_data(_CCBT_data *dat, int16 *err);

void setup_16ms_task(int  n, __16ms_par *par, int16 *err);
void check_16ms_task(int *n, __16ms_par *par, int16 *err);
void run_16ms_task_procedure(int8 pro, int8 *par, int16 *err);

void write_CCEB_task_controls(
  int32 *fai_chk, int32 *ala_chk, int32 *fai_lim, int32 *ala_lim, int16 *err);
void write_CCEB_task_target_values(int8 *phase, int32 *power, int16 *err);
void write_CCEB_task_target_value(
     int8 cha, int8 phase, int32 power, int16 *err);

void send_CCEB_task_command(int8 cha, int8 cmd, int32 *dat, int16 *err);
void read_CCEB_task_status(int8 *dat, int16 *err);

//~--- PDS Specific Commands ---

void enable_CGS_serial_lines(int16 *err);
void disable_CGS_serial_lines(int16 *err);
void exec_PDS_cmd(int16 cmd, int8 adc_cha, int8 *dig, int16 *adc_dat, int16 *err);
void exec_new_PDS_cmd(
     int16 cmd, int8 adc_cha, int8 del, int8 *dig, int16 *adc_dat, int16 *err);

//~--- TTCE Specific Commands ---

void check_TTCE_FLASH_erasure(int8 sector, bool *done, int16 *err);

void setup_TTCE_28VDC_power(int8 *dat, int16 *err);
void check_TTCE_28VDC_power(int8 *dat, int16 *err);

void check_TTCE_DS1820_scan(int8 *bus, int8 *n, int16 *err);
void read_TTCE_DS1820_table(int8 bus, int8 *n, int64 *ID, int16 *err);
void write_TTCE_DS1820_table(int8 bus, int8 n, int64 *ID, int16 *err);
void setup_TTCE_DS1820_readout(int8 ena, int16 *err);
void check_TTCE_DS1820_readout(int8 *ena, int16 *err);
void read_TTCE_DS1820_temp(int8 bus, int8 *n, bool *ok, float *temp, int16 *err);

void write_TTCE_Pt1000_ctrl(int8 n, int8 *adr, int8 *dat, int16 *err);
void read_TTCE_Pt1000_ADC(int16 *adc, int16 *err);
void read_TTCE_Pt1000_temp(int8 *RC, int8 *VR, float *temp, int16 *err);

void read_TTCE_pressure(int16 *dat, int16 *err);

void write_TTCE_pump_DAC(int8 *DAC, int16 *err);
void read_TTCE_pump_data(int8 *DAC, int16 *RPM, int16 *err);

void write_TTCE_PWM(int8 n, int8 *adr, int8 *dat, int16 *err);
void read_TTCE_PWM(int8 *dat, int16 *err);

void write_TTCE_loop_ctrl(int8 n, int8 *adr, int8 *dat, int16 *err);
void read_TTCE_loop_ctrl(int8 *ctrl, int16 *err);

//~--- CAB Specific Commands ---

void exec_CAB_command(int8 *cmd, int8 *cnt, int8 *rep, int16 *err);
int16 check_CAB_reply(int32 RQ_typ, int8 *RQ_dat, int32 RP_cnt, int8 *RP_dat);


//~--- Main Interface Function to USCM ---

void to_USCM(int R_W,
             int32 RQ_typ, int32  RQ_cnt, int8 *RQ_dat,
             int32 RP_siz, int32 *RP_cnt, int8 *RP_dat, int16 *RP_err);

//~============================================================================

#endif // _USCMLIB_H
