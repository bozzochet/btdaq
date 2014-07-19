// file TTCE-2.c
//
// TTCE-2 Controller
//
// A.Lebedev Feb-2008...

#include "ttcedef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"TTCE-2  Controller"};                 // global variable
char *date  = {"13-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x167;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~-- 28V Power Control

SW_BTN  *PWR_ESW_BTN[16];
SW_BTN  *PWR_MSW_BTN[16];
SW_BTN  *PWR_SWT_BTN[8];
SW_BTN  *PWR_ENA_BTN[24];
SW_BTN  *PWR_CMD_BTN[24];
DAT_BOX *PWR_ESW_BOX[16];
DAT_BOX *PWR_MSW_BOX[16];
DAT_BOX *PWR_SWT_BOX[8];
DAT_BOX *PWR_STA_BOX[24];
_ww w_pwr;
_ww w_pwr_cmd;
_r  r_pwr;

//~-- Pump Control

DAT_BOX *PUMP_28V_BOX[2];
SW_BTN  *PUMP_VOLT_COD_BTN[2][2];
SW_BTN  *PUMP_VOLT_VAL_BTN[2][1];
DAT_BOX *PUMP_VOLT_COD_BOX[2][2];
DAT_BOX *PUMP_VOLT_VAL_BOX[2][1];
DAT_BOX *PUMP_RPM_COD_BOX[2];
DAT_BOX *PUMP_RPM_VAL_BOX[2];
_ww w_pump;
_r  r_pump;
bool pump_follow_all;

//~-- PWM Control

DAT_BOX *PWM_28V_BOX[6];
SW_BTN  *PWM_SEL_BTN[6];
SW_BTN  *PWM_COD_BTN[6][2];
SW_BTN  *PWM_VAL_BTN[6][1];
SW_BTN  *PWM_PCT_BTN[6][1];
DAT_BOX *PWM_COD_BOX[6][2];
DAT_BOX *PWM_VAL_BOX[6][1];
DAT_BOX *PWM_PCT_BOX[6][1];
_ww w_pwm_sel;
_ww w_pwm[6];
_r  r_pwm[6];
bool pwm_follow[6], pwm_follow_all;

//~-- Pressure Sensors

DAT_BOX *PRES_COD_BOX[4];
DAT_BOX *PRES_VAL_BOX[4];
_r  r_pres;

//~-- Pt1000 Sensors

SW_BTN  *PT_RC_BTN[10];
DAT_BOX *PT_COD_BOX[3][23];
DAT_BOX *PT_VAL_BOX[3][23];
DAT_BOX *PT_RC_BOX[22];
DAT_BOX *PT_VR_BOX[22];
DAT_BOX *PT_TEMP_BOX[22];
_ww w_pt_rc[10];
_r  r_pt_adc;
_r  r_pt_rc[22];
_r  r_pt_vr[22];
_r  r_pt_temp[22];

//~-- Read All & File Writing

bool auto_read_all = FALSE;
bool data_updated = FALSE;
SW_BTN *PER_BTN;
_ww w_per;
SW_BTN *REC_BTN;
_ww w_rec;

//~--

char *hex[] = {
  "0", "1", "2", "3", "4", "5", "6", "7", 
  "8", "9", "A", "B", "C", "D", "E", "F"};
char *o_o[] = {"OFF", "ON", "-"};
char *e__[] = {"", "E"};
char *x__[] = {"", "@9+"};
char *nlrv[] = {"N", "L", "R", "V"};
char *nlr[]  = {"N", "L", "R", "?"};
char *periods[5] = {"1s", "2s", "5s", "10s", "20s"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_28V_panel(int x0, int y0);
void control_28V_callback(FL_OBJECT *obj, long data);
void write_28V_callback(FL_OBJECT *obj, long data);
void read_28V_callback(FL_OBJECT *obj, long data);
void update_28V_panel(void);

void create_pump_control_panel(int x0, int y0);
void convert_pump_voltage_code_to_value(int32 code, int32 *value);
void convert_pump_rpm_code_to_value(int32 code, int32 *value);
void set_pump_voltage_callback(FL_OBJECT* obj, long data);
void write_pump_voltage_callback(FL_OBJECT* obj, long data);
void read_pump_parameters_callback(FL_OBJECT* obj, long data);
void update_pump_control_panel(void);

void create_PWM_control_panel(int x0, int y0);
void convert_PWM_code_to_duty_cycle(int32 code, int32 *value);
void convert_PWM_duty_cycle_to_code(int32 value, int32 *code);
void set_PWM_callback(FL_OBJECT* obj, long data);
void write_PWM_callback(FL_OBJECT* obj, long data);
void read_PWM_callback(FL_OBJECT* obj, long data);
void update_PWM_control_panel(void);

void create_pressure_sensor_panel(int x0, int y0);
void convert_absolute_pressure_code_to_value(int32 code, int32 *value);
void convert_differential_pressure_code_to_value(int32 code, int32 *value);
void read_pressure_sensor_callback(FL_OBJECT* obj, long data);
void update_pressure_sensor_panel(void);

void create_Pt_sensor_panel(int x0, int y0);
void convert_N_Pt_code_to_temperature(int32 code, int32 *value);
void convert_L_Pt_code_to_temperature(int32 code, int32 *value);
void convert_R_Pt_code_to_temperature(int32 code, int32 *value);
void write_Pt_sensor_control_callback(FL_OBJECT* obj, long data);
void read_all_Pt_sensor_data_callback(FL_OBJECT* obj, long data);
void read_selected_Pt_sensor_data_callback(FL_OBJECT* obj, long data);
void update_Pt_sensor_panel(void);

void create_read_all_file_write_panel(int x0, int y0);
void read_all_callback(FL_OBJECT* obj, long data);
void auto_read_all_callback(int tid, void *data);
void record_ttce_data(FILE *file);
void update_read_all_file_write_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 1095, HH = 505;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_28V_panel(0, 45);

  create_pump_control_panel(   245,  45);
  create_PWM_control_panel(    245, 135);
  create_pressure_sensor_panel(245, 285);
  
  create_Pt_sensor_panel(605, 45);

  create_read_all_file_write_panel(260, 415);
  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  invalidate_ww(first, &w_pwr,      8, 0);
  invalidate_ww(first, &w_pwr_cmd, 24, 0);
  invalidate_r(first,  &r_pwr,      8);

  pump_follow_all = FALSE;
  invalidate_ww(first, &w_pump, 2, 0);
  invalidate_r(first,  &r_pump, 4);

  invalidate_ww(first, &w_pwm_sel, 6, 0);
  pwm_follow_all = FALSE;
  for (i=0; i<6; i++) {
    pwm_follow[i] = FALSE;
    invalidate_ww(first, &w_pwm[i], 1, 0);
    invalidate_r(first,  &r_pwm[i], 1);
  }

  invalidate_r(first,  &r_pres,     4);

  for (i=0; i<10; i++) {
    invalidate_ww(first, &w_pt_rc[i], 1, 0);
  }
  
  for (i=0; i<22; i++) {
    invalidate_r(first,  &r_pt_rc[i],   1);
    invalidate_r(first,  &r_pt_vr[i],   1);
    invalidate_r(first,  &r_pt_temp[i], 1);
  }
  invalidate_r(first,  &r_pt_adc, 48);

  invalidate_ww(first, &w_per, 1, 0);
  invalidate_ww(first, &w_rec, 1, 0);

  read_dat_config_file(dat_config_file_name);

  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(w_pwr.inp,     4,  8, file);
    fread(w_pwr_cmd.inp, 4, 24, file);
    fread(w_pump.inp,    4,  2, file);
    fread(w_pwm_sel.inp, 4,  6, file);
    for (i=0; i< 6; i++) fread(w_pwm[i].inp,   4, 1, file);
    for (i=0; i<10; i++) fread(w_pt_rc[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i;

  file = fopen(filename, "w");
  if (file) {
    fwrite(w_pwr.inp,     4,  8, file);
    fwrite(w_pwr_cmd.inp, 4, 24, file);
    fwrite(w_pump.inp,    4,  2, file);
    fwrite(w_pwm_sel.inp, 4,  6, file);
    for (i=0; i< 6; i++) fwrite(w_pwm[i].inp,   4, 1, file);
    for (i=0; i<10; i++) fwrite(w_pt_rc[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

void refresh_objects(void) {

  invalidate_data();
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void switch_btn_callback(int32 data) {

  msg = 0;
  update_objects();
}

//~============================================================================
//
//                  28V CONTROL PANEL
//
//~============================================================================

extern _b pwr_w_esw_msk[];
extern _b pwr_w_msw_msk[];
extern _b pwr_w_swt_msk[];
extern _b pwr_w_ena_msk[];
extern _b pwr_r_esw_msk[];
extern _b pwr_r_msw_msk[];
extern _b pwr_r_swt_msk[];
extern _b pwr_cmd_msk[];
extern _b pwr_sta_msk[];

void create_28V_panel(int x0, int y0) {

  int i;
  FL_OBJECT *obj;
  int x, y, y1, y2, w, h;
  int w1 = 45, w2 = 15, w3 = 15, w4 = 15;
  int w5 = 30, w6 = 30, w7 = 25, w8 = 15, w9 = 15, w10 = 30;
  int h1 = 15, h2 = 15, h3 = 20;
  char *txt[] = {
    "LLW_P", "LLR_P", "PR1_P", "PR2_P", "COR_P", "SUP_P", "GAE_P", "FAE_P", 
    "LLW_S", "LLR_S", "PR1_S", "PR2_S", "COR_S", "SUP_S", "GAE_S", "FAE_S", 
    "GAC_P", "FAC_P", "TEC_P", "GAC_S", "FAC_S", "TEC_S", "P_P",   "P_S"};
//~--

  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + w7 + w8 + w9 + w10 + 5;
  h = 5 + h1 + h1 + 8 * h2 + 5 + 8 * h2 + 5 + 8 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "28V", 'L');
  x = x + w1;
  add_frame_box(x, y, w2, 2*h1, "E\nn", 'S');
  x = x + w2;
  add_frame_box(x, y, w3+w4, h1, "Actn", 'S');
  add_frame_box(x, y+h1, w3, h1, "E", 'S');
  x = x + w3;
  add_frame_box(x, y+h1, w4, h1, "M", 'S');
  x = x + w4;
  add_frame_box(x, y, w5+w6, 2*h1, "", 'S');
  x = x + w5 + w6;
  add_frame_box(x, y, w7, 2*h1, "", 'S');
  x = x + w7;
  add_frame_box(x, y, w8+w9, h1, "Ctrl", 'S');
  add_frame_box(x, y+h1, w8, h1, "E", 'S');
  x = x + w8;
  add_frame_box(x, y+h1, w9, h1, "M", 'S');
  x = x + w9;
  add_frame_box(x, y, w10, 2*h1, "Sta", 'S');
  x = x + w10;

//~--

  y1 = y0 + 5 + 2 * h1;
  y2 = y1 + 8 * h2 + 5 + 8 * h2 + 5;

  x = x0 + 5;
  y = y1;
  create_frame_box_vstring(24, x, y, w1, h2, &txt[0], 'S'+800, 'L');
  x = x + w1;
  
  create_btn_vstring(
    24, x, y, w2,   h2, e__, 'N'+800, &w_pwr, &pwr_w_ena_msk[0], &PWR_ENA_BTN[0]);
  x = x + w2;

  create_btn_vstring(
    16, x, y, w3,   h2, hex, 'N'+800, &w_pwr, &pwr_w_esw_msk[0], &PWR_ESW_BTN[0]);
  y = y2;
  create_btn_vstring(
    8, x, y, w3+w4, h2, hex, 'N'+800, &w_pwr, &pwr_w_swt_msk[0], &PWR_SWT_BTN[0]);
  x = x + w3;
  y = y1;
  create_btn_vstring(
    16, x, y, w4,   h2, hex, 'N'+800, &w_pwr, &pwr_w_msw_msk[0], &PWR_MSW_BTN[0]);
  x = x + w4;

  for (i=0; i<24; i++) {
    obj = add_normal_button(x,    y, w5, h2, "ON",  'N');
      fl_set_object_callback(obj, control_28V_callback, 2*i+1);
    obj = add_normal_button(x+w5, y, w6, h2, "OFF", 'N');
      fl_set_object_callback(obj, control_28V_callback, 2*i+0);
    y = y + h2;
    if (i == 7 || i == 15) y = y + 5;
  }
  x = x + w5 + w6;

  y = y1;
  create_btn_vstring(
    24, x, y, w7,   h2, o_o, 'N'+800, &w_pwr_cmd, &pwr_cmd_msk[0], &PWR_CMD_BTN[0]);
  x = x + w7;

  create_dat_box_vstring(
    16, x, y, w8,    h2, 'N'+800, &r_pwr, &pwr_r_esw_msk[0], &PWR_ESW_BOX[0]);
  y = y2;
  create_dat_box_vstring(
    8,  x, y, w8+w9, h2, 'N'+800, &r_pwr, &pwr_r_swt_msk[0], &PWR_SWT_BOX[0]);
  x = x + w8;
  y = y1;
  create_dat_box_vstring(
    16, x, y, w9,    h2, 'N'+800, &r_pwr, &pwr_r_msw_msk[0], &PWR_MSW_BOX[0]);
  x = x + w9;
  create_dat_box_vstring(
    24, x, y, w10,   h2, 'N'+800, &r_pwr, &pwr_sta_msk[0],   &PWR_STA_BOX[0]);
  x = x + w10;

//~--

  x = x0 + 5;
  y = y + 8 * h2 + 5 + 8 * h2 + 5 + 8 * h2;
  add_frame_box(x, y, w1, h3, "", 'S');
  x = x + w1;
  obj = add_normal_button(x, y, w2+w3+w4, h3, "W",  'N');
    fl_set_object_callback(obj, write_28V_callback, 0);
  x = x + w2 + w3 + w4;
  add_frame_box(x, y, w5+w6, h3, "", 'S');
  x = x + w5 + w6;
  obj = add_normal_button(x, y, w7, h3, "W",  'N');
    fl_set_object_callback(obj, control_28V_callback, -(0*32+24));
  x = x + w7;
  obj = add_normal_button(x, y, w8+w9+w10, h3, "READ",  'N');
    fl_set_object_callback(obj, read_28V_callback, 0);
  x = x + w8 + w9 + w10;
}

//~----------------------------------------------------------------------------

void control_28V_callback(FL_OBJECT *obj, long data) {

  int i;
  int8 wri[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  _ww *w = &w_pwr_cmd;
  _r  *r = &r_pwr;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  if (data < 0) {
    int i1 = ABS(data) / 32;
    int i2 = ABS(data) % 32;
    for (i=i1; i<i2; i++) {
      int32 m = 0x00000001 << i;
      int32 d = w->inp[i]  << i;
      if (w->inp[i] > 1) continue;
      wri[0] |= d >>  0;
      wri[1] |= d >>  0;
      wri[3] |= d >>  8;
      wri[4] |= d >>  8;
      wri[6] |= d >> 16;
      wri[2] |= m >>  0;
      wri[5] |= m >>  8;
      wri[7] |= m >> 16;
    }
  }
  else {
    int8 ind = data / 2;
    int8 dat = data % 2;
    int32 m = 0x00000001 << ind;
    int32 d = dat        << ind;
    wri[0] |= d >>  0;
    wri[1] |= d >>  0;
    wri[3] |= d >>  8;
    wri[4] |= d >>  8;
    wri[6] |= d >> 16;
    wri[2] |= m >>  0;
    wri[5] |= m >>  8;
    wri[7] |= m >> 16;
  }

  setup_TTCE_28VDC_power(wri, &err);

  if (data < 0) {
    if (!err) {
      for (i=0; i<24; i++) w->wri[i] = w->inp[i];
      w->unk = FALSE;
    }
    else {
      w->unk = TRUE;
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void write_28V_callback(FL_OBJECT *obj, long data) {

  int i;
  int8 wri[8];
  _ww *w = &w_pwr;
  _r *r  = &r_pwr;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;
  
  for (i=0; i<8; i++) wri[i] = w->inp[i];
  setup_TTCE_28VDC_power(wri, &err);

  if (!err) {
    for (i=0; i<8; i++) w->wri[i] = w->inp[i];
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }
    
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_28V_callback(FL_OBJECT *obj, long data) {

  int i;
  int8 rea[5];
  _r *r = &r_pwr;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<8; i++) r->was[i] = r->rea[i];
  
  check_TTCE_28VDC_power(rea, &err);
  
  if (!err) {
    for (i=0; i<5; i++) r->rea[i] = rea[i];
    r->rea[5] = rea[0] & rea[1];
    r->rea[6] = rea[2] & rea[3];
    r->rea[7] = rea[4];
    if (r->unk) for (i=0; i<8; i++) r->was[i] = r->rea[i];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_28V_panel(void) {

  show_btn_string(    16,      NULL, &PWR_ESW_BTN[0]);
  show_btn_string(     8,      NULL, &PWR_SWT_BTN[0]);
  show_btn_string(    16,      NULL, &PWR_MSW_BTN[0]);
  show_btn_string(    24,      NULL, &PWR_ENA_BTN[0]);

  show_btn_string(    24,      NULL, &PWR_CMD_BTN[0]);

  show_dat_box_string(16, hex, NULL, &PWR_ESW_BOX[0]);
  show_dat_box_string( 8, hex, NULL, &PWR_SWT_BOX[0]);
  show_dat_box_string(16, hex, NULL, &PWR_MSW_BOX[0]);
  show_dat_box_string(24, o_o, NULL, &PWR_STA_BOX[0]);
}

//~============================================================================
//
//                  PUMP CONTROL PANEL
//
//~============================================================================

extern _b pump_volt_cod_msk[2][2];
extern _b pump_volt_val_msk[2];
extern _b pump_rpm_cod_msk[2];
extern _b pump_rpm_val_msk[2];

void create_pump_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, x1;

{//~--- Panel ---

  int w1 = 360, w2 = 55, h1 = 90, h2 = 15, h3 = 20;
  char *txt[] = {"P_P", "P_S"};

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, 2*h2, "Pump", 'L');
  y = y + 2 * h2;
  create_frame_box_vstring(2, x, y, w2, h2, &txt[0], 'S', 'L');
  y = y + 2 * h2;
  add_frame_box(x, y, w2, h3, "", 'S');
  x = x + w2;
  y = y0 + 5;
}
  
{//~--- 28 VDC ---

  int w1 = 30, w2 = 30, w3 = 30, h1 = 15, h2 = 20;

  add_frame_box(x, y, w1+w2+w3, 2*h1, "28VDC", 'S');
  x1 = x;
  y  = y + 2 * h1;
  for (i=22; i<24; i++) {
    x = x1;
    obj = add_normal_button(x, y, w1, h1, "ON",  'N');
      fl_set_object_callback(obj, control_28V_callback, 2*i+1);
    x = x + w1;
    obj = add_normal_button(x, y, w2, h1, "OFF", 'N');
      fl_set_object_callback(obj, control_28V_callback, 2*i+0);
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w3, h1, 'N', &r_pwr, &pwr_sta_msk[i], &PUMP_28V_BOX[i-22]);
    y = y + h1;
  }
  x = x1;
  add_frame_box(x, y, w1+w2, h2, "", 'N');
  x = x + w1 + w2;
  obj = add_normal_button(x, y, w3, h2, "R", 'N');
    fl_set_object_callback(obj, read_28V_callback, 0);
  x = x1 + w1 + w2 + w3;
  y = y0 + 5;
}

{//~--- Control Voltage ---

  int w;
  int w1 = 15, w2 = 35, w3 = 140, h1 = 15, h2 = 20;
  
  x1 = x;
  w = 2 * (2 * w1 + w2);
  add_frame_box(x, y, w, h1, "Control Voltage", 'S');
  y = y + h1;
  for (i=0; i<2; i++) {
    add_frame_box(x, y, 2*w1, h1, "Code", 'S');
    x = x + 2 * w1;
    add_frame_box(x, y, w2,   h1, "mV",   'S');
    x = x + w2;
  }

  x = x1;
  y = y + h1;
  for (i=0; i<2; i++) {
    x = x1;
    create_btn_hstring(
      2, x, y, w1, h1,  hex, 'N', &w_pump, &pump_volt_cod_msk[i][0], &PUMP_VOLT_COD_BTN[i][0]);
    x = x + 2 * w1;
    create_btn_hstring(
      1, x, y, w2, h1, NULL, 'N', &w_pump, &pump_volt_val_msk[i], &PUMP_VOLT_VAL_BTN[i][0]);
    PUMP_VOLT_VAL_BTN[i][0]->cnv = convert_pump_voltage_code_to_value;
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,      y, w2/2, h1, "16");
      fl_set_object_callback(obj, set_pump_voltage_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w2/2, y, w2/2, h1, "1");
      fl_set_object_callback(obj, set_pump_voltage_callback, i);
    x = x + w2;
    create_dat_box_hstring(
      2, x, y, w1, h1,       'N', &r_pump, &pump_volt_cod_msk[i][0], &PUMP_VOLT_COD_BOX[i][0]);
    x = x + 2 * w1;
    create_dat_box_hstring(
      1, x, y, w2, h1,       'N', &r_pump, &pump_volt_val_msk[i], &PUMP_VOLT_VAL_BOX[i][0]);
    PUMP_VOLT_VAL_BOX[i][0]->cnv = convert_pump_voltage_code_to_value;
    y = y + h1;
  }
  x = x1;
  obj = add_normal_button(x, y, 2*w1+w2, h2, "WRITE", 'N');
      fl_set_object_callback(obj, write_pump_voltage_callback, 0);
  x = x + 2 * w1 + w2;
  obj = add_normal_button(x, y, w3, h2, "READ", 'N');
    fl_set_object_callback(obj, read_pump_parameters_callback, 0);
  x = x + 2 * w1 + w2 + 5;
  y = y0 + 5;
}

{//~--- RPM ---

  int w;
  int w1 = 30, w2 = 40, h1 = 15;
  
  x1 = x;
  w = w1 + w2;
  add_frame_box(x, y, w, h1, "RPM", 'S');
  y = y + h1;
  add_frame_box(x, y, w1, h1, "Code", 'S');
  x = x + w1;
  add_frame_box(x, y, w2, h1, "RPM",  'S');
  x = x + w2;

  x = x1;
  y = y + h1;
  for (i=0; i<2; i++) {
    x = x1;
    create_dat_box_hstring(
      1, x, y, w1, h1,       'N', &r_pump, &pump_rpm_cod_msk[i], &PUMP_RPM_COD_BOX[i]);
    x = x + w1;
    create_dat_box_hstring(
      1, x, y, w2, h1,       'N', &r_pump, &pump_rpm_val_msk[i], &PUMP_RPM_VAL_BOX[i]);
    PUMP_RPM_VAL_BOX[i]->cnv = convert_pump_rpm_code_to_value;
    y = y + h1;
  }
  x = x1;
}

}

//~----------------------------------------------------------------------------

void convert_pump_voltage_code_to_value(int32 code, int32 *value) {

  *value = (float)code / 256.0 * 4096.0;   // this is VK's wrong formula
}

//~----------------------------------------------------------------------------

void convert_pump_rpm_code_to_value(int32 code, int32 *value) {

  *value = (float)code / 9.0 * 60.0;
}

//~----------------------------------------------------------------------------

void set_pump_voltage_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _ww *w = &w_pump;
  int  d =  atoi(fl_get_object_label(obj));

  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  
  pack32(&w->inp[ind], 0xFF, MOD(w->inp[ind]+d, 256));
  w->sta = status_NA;

  if (pump_follow_all) {
    msg = -1;
    update_objects();
    write_pump_voltage_callback(NULL, 0);
    msg = 1;
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void write_pump_voltage_callback(FL_OBJECT* obj, long data) {

  int i;
  int8 wri[2];
  _ww *w = &w_pump;
  _r  *r = &r_pump;

  if (obj) {
    msg = 0;
    if (pump_follow_all) {
      set_normal_button(obj, 0);
      pump_follow_all = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        pump_follow_all = TRUE;
        update_objects();
        return;
      }
    }
    msg = -1;
    update_objects();
  }
  
  r->sta = status_NA;
  
  for (i=0; i<2; i++) wri[i] = w->inp[i];

  write_TTCE_pump_DAC(wri, &err);

  if (!err) {
    for (i=0; i<2; i++) w->wri[i] = w->inp[i];
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }

//if (!err) read_pump_voltage_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_pump_parameters_callback(FL_OBJECT* obj, long data) {

  int i;
  int8  DAC[2];
  int16 RPM[2];
  _r  *r = &r_pump;
    
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<4; i++) r->was[i] = r->rea[i];
  
  read_TTCE_pump_data(DAC, RPM, &err);

  if (!err) {
    r->rea[0] = DAC[0];
    r->rea[1] = DAC[1];
    r->rea[2] = RPM[0];
    r->rea[3] = RPM[1];
    if (r->unk) for (i=0; i<4; i++) r->was[i] = r->rea[i];
    r->unk  = FALSE;
    r->sta  = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_pump_control_panel(void) {

  int i;
  
  show_dat_box_string(2, o_o, NULL, &PUMP_28V_BOX[0]);

  for (i=0; i<2; i++) {
    show_btn_string(    2,       NULL,   &PUMP_VOLT_COD_BTN[i][0]);
    show_btn_string(    1,       "%4d",  &PUMP_VOLT_VAL_BTN[i][0]);
    show_dat_box_string(2,  hex, NULL,   &PUMP_VOLT_COD_BOX[i][0]);
    show_dat_box_string(1, NULL, "%4d",  &PUMP_VOLT_VAL_BOX[i][0]);
    show_dat_box_string(1, NULL, "%03X", &PUMP_RPM_COD_BOX[i]);
    show_dat_box_string(1, NULL, "%4d",  &PUMP_RPM_VAL_BOX[i]);
  }
}

//~============================================================================
//
//                  PWM CONTROL PANEL
//
//~============================================================================

extern _b pwm_sel_msk[6];
extern _b pwm_cod_msk[2];
extern _b pwm_val_msk[1];
extern _b pwm_pct_msk[1];

void create_PWM_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, x1;

{//~--- Panel ---

  int w1 = 360, w2 = 55, h1 = 150, h2 = 15, h3 = 20;
  char *txt[] = {"GAC_P", "FAC_P", "TEC_P", "GAC_S", "FAC_S", "TEC_S"};
  
  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, 2*h2, "PWM", 'L');
  y = y + 2 * h2;
  create_frame_box_vstring(6, x, y, w2, h2, &txt[0], 'S', 'L');
  y = y + 6 * h2;
  add_frame_box(x, y, w2, h3, "", 'S');
  x = x + w2;
  y = y0 + 5;
}
  
{//~--- 28 VDC ---

  int w1 = 30, w2 = 30, w3 = 30, h1 = 15, h2 = 20;

  add_frame_box(x, y, w1+w2+w3, 2*h1, "28VDC", 'S');
  x1 = x;
  y  = y + 2 * h1;
  for (i=16; i<22; i++) {
    x = x1;
    obj = add_normal_button(x, y, w1, h1, "ON",  'N');
      fl_set_object_callback(obj, control_28V_callback, 2*i+1);
    x = x + w1;
    obj = add_normal_button(x, y, w2, h1, "OFF", 'N');
      fl_set_object_callback(obj, control_28V_callback, 2*i+0);
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w3, h1, 'N', &r_pwr, &pwr_sta_msk[i], &PWM_28V_BOX[i-16]);
    x = x + w3;
    y = y + h1;
  }
  x = x1;
  add_frame_box(x, y, w1+w2, h2, "", 'N');
  x = x + w1 + w2;
  obj = add_normal_button(x, y, w3, h2, "R", 'N');
    fl_set_object_callback(obj, read_28V_callback, 0);
  x = x1 + w1 + w2 + w3 + 5;
  y = y0 + 5;
}

{//~--- Write PWM Controls ---

  int w;
  int w1 = 15, w2 = 30, w3 = 20, w4 = 20, h1 = 15, h2 = 20;
  
  x1 = x;
  w = 3 * w1 + w2 + w3 + w4;
  add_frame_box(x, y, w, h1, "Write", 'S');
  y = y + h1;
  add_frame_box(x, y, w1,   h1, "",     'S');
  x = x + w1;
  add_frame_box(x, y, 2*w1, h1, "Code", 'S');
  x = x + 2 * w1;
  add_frame_box(x, y, w2,   h1, "Dec",  'S');
  x = x + w2;
  add_frame_box(x, y, w3,   h1, "%",    'S');
  x = x + w3;
  add_frame_box(x, y, w4,   h1, "",     'S');
  x = x + w4;
  y = y + h1;

  x = x1;
  create_btn_vstring(
    6, x, y, w1, h1,  x__, 'N', &w_pwm_sel, &pwm_sel_msk[0], &PWM_SEL_BTN[0]);
  x = x + w1;
  for (i=0; i<6; i++) {
    int yy = y + i * h1;
    create_btn_hstring(
      2, x, yy, w1, h1,  hex, 'N', &w_pwm[i], pwm_cod_msk, &PWM_COD_BTN[i][0]);
  }
  x = x + 2 * w1;
  for (i=0; i<6; i++) {
    int yy = y + i * h1;
    create_btn_vstring(
      1, x, yy, w2, h1, NULL, 'N', &w_pwm[i], pwm_val_msk, &PWM_VAL_BTN[i][0]);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,        yy, w2/3, h1, "100");
      fl_set_object_callback(obj, set_PWM_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w2/3,   yy, w2/3, h1, "10");
      fl_set_object_callback(obj, set_PWM_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+2*w2/3, yy, w2/3, h1, "1");
      fl_set_object_callback(obj, set_PWM_callback, i);
  }
  x = x + w2;
  for (i=0; i<6; i++) {
    int yy = y + i * h1;
    create_btn_vstring(
      1, x, yy, w3, h1, NULL, 'N', &w_pwm[i], pwm_pct_msk, &PWM_PCT_BTN[i][0]);
    PWM_PCT_BTN[i][0]->cnv  = convert_PWM_code_to_duty_cycle;
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,        yy, w3/2, h1, "10");
      fl_set_object_callback(obj, set_PWM_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w3/2,   yy, w3/2, h1, "1");
      fl_set_object_callback(obj, set_PWM_callback, i);
  }
  x = x + w3;
  for (i=0; i<6; i++) {
    int yy = y + i * h1;
    obj = add_normal_button(x, yy, w4, h1, "W",  'N');
      fl_set_object_callback(obj, write_PWM_callback, i);
  }
  x = x1;
  y = y + 6 * h1;
  obj = add_normal_button(x, y, w, h2, "WRITE",  'N');
    fl_set_object_callback(obj, write_PWM_callback, -1);
  x = x + w + 5;
}

{//~--- Read PWM Controls ---

  int w;
  int w1 = 15, w2 = 30, w3 = 20, h1 = 15, h2 = 20;
  
  x1 = x;
  y  = y0 + 5;
  w = 2 * w1 + w2 + w3;
  add_frame_box(x, y, w, h1, "Read", 'S');
  y = y + h1;
  add_frame_box(x, y, 2*w1, h1, "Code", 'S');
  x = x + 2 * w1;
  add_frame_box(x, y, w2,   h1, "Dec",  'S');
  x = x + w2;
  add_frame_box(x, y, w3,   h1, "%",    'S');
  x = x + w3;
  y = y + h1;

  x = x1;
  for (i=0; i<6; i++) {
    int yy = y + i * h1;
    create_dat_box_hstring(
      2, x, yy, w1, h1, 'N', &r_pwm[i], pwm_cod_msk, &PWM_COD_BOX[i][0]);
  }
  x = x + 2 * w1;
  for (i=0; i<6; i++) {
    int yy = y + i * h1;
    create_dat_box_vstring(
      1, x, yy, w2, h1, 'N', &r_pwm[i], pwm_val_msk, &PWM_VAL_BOX[i][0]);
  }
  x = x + w2;
  for (i=0; i<6; i++) {
    int yy = y + i * h1;
    create_dat_box_vstring(
      1, x, yy, w3, h1, 'N', &r_pwm[i], pwm_val_msk, &PWM_PCT_BOX[i][0]);
      PWM_PCT_BOX[i][0]->cnv = convert_PWM_code_to_duty_cycle;
  }
  x = x1;
  y = y + 6 * h1;
  obj = add_normal_button(x, y, w, h2, "READ",  'N');
    fl_set_object_callback(obj, read_PWM_callback, 0);
  x = x + w + 5;
}

}

//~----------------------------------------------------------------------------

void convert_PWM_code_to_duty_cycle(int32 code, int32 *value) {

  *value = 99.6 / 255.0 * (float)code;
}

//~----------------------------------------------------------------------------

void convert_PWM_duty_cycle_to_code(int32 value, int32 *code) {

  *code = 255.0 / 99.6 * (float)value;
}

//~----------------------------------------------------------------------------

void set_PWM_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _ww *w = &w_pwm[ind];
  int  d =  atoi(fl_get_object_label(obj));

  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  
  pack32(&w->inp[0], 0xFF, MOD(w->inp[0]+d, 256));
  w->sta = status_NA;

  if (pwm_follow[ind] || pwm_follow_all) {
    msg = -1;
    update_objects();
    write_PWM_callback(NULL, ind);
    msg = 1;
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void write_PWM_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i, n;
  int8 adr[6], dat[6];
  _ww *s = &w_pwm_sel;

  n = 0;
  for (i=0; i<6; i++) {
    _ww  *w = &w_pwm[i];
    int32 m = pwm_sel_msk[i].msk;
    if ((ind < 0 && (s->inp[i] & m)) || (ind == i))  {
      adr[n] = i;
      dat[n] = w->inp[0];
      n = n + 1;
    }
  }

  if (!n) return;
  
  if (obj) {
    msg = 0;
    if ((ind < 0 && pwm_follow_all) || (ind >= 0 && pwm_follow[ind])) {
      set_normal_button(obj, 0);
      if (ind < 0) pwm_follow_all  = FALSE;
      else         pwm_follow[ind] = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        if (ind < 0) pwm_follow_all  = TRUE;
        else         pwm_follow[ind] = TRUE;
        update_objects();
        return;
      }
    }
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<6; i++) {
    int32 m = pwm_sel_msk[i].msk;
    _r  *r = &r_pwm[i];
    if ((ind < 0 && (s->inp[i] & m)) || (ind == i)) r->sta = status_NA;
  }

  write_TTCE_PWM(n, adr, dat, &err);

  for (i=0; i<6; i++) {
    _ww  *w = &w_pwm[i];
    int32 m = pwm_sel_msk[i].msk;
    if (!err) {
      if ((ind < 0 && (s->inp[i] & m)) || (ind == i)) {
        w->wri[0] = w->inp[0];
        w->unk = FALSE;
      }
    }
    else {
      if ((ind < 0 && (s->inp[i] & m)) || (ind == i)) {
        w->unk = TRUE;
      }
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_PWM_callback(FL_OBJECT* obj, long data) {

  int i;
  int8 dat[6];
    
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<6; i++) {
    _r  *r = &r_pwm[i];
    r->was[0] = r->rea[0];
  }
  
  read_TTCE_PWM(dat, &err);

  for (i=0; i<6; i++) {
    _r  *r = &r_pwm[i];
    if (!err) {
      r->rea[0] = dat[i];
      if (r->unk) r->was[0] = r->rea[0];
      r->unk  = FALSE;
      r->sta  = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_PWM_control_panel(void) {

  int i;
  
  show_dat_box_string(6, o_o, NULL, &PWM_28V_BOX[0]);

//show_btn_string(6, NULL, &PWM_SEL_BTN[0]);

  for (i=0; i<6; i++) {
    show_btn_string(2,  NULL, &PWM_COD_BTN[i][0]);
    show_btn_string(1, "%3d", &PWM_VAL_BTN[i][0]);
    show_btn_string(1, "%2d", &PWM_PCT_BTN[i][0]);
    show_dat_box_string(2,  hex, NULL,   &PWM_COD_BOX[i][0]);
    show_dat_box_string(1, NULL, "%3d",  &PWM_VAL_BOX[i][0]);
    show_dat_box_string(1, NULL, "%2d",  &PWM_PCT_BOX[i][0]);
  }
}

//~============================================================================
//
//                  PRESSURE SENSOR PANEL
//
//~============================================================================

extern _b pres_msk[4];

void create_pressure_sensor_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 75, w2 = 45, w3 = 30, h1 = 15, h2 = 20;
  char *txt[] = {"APS_P, MPa", "DPS_P, kPa", "APS_S, MPa", "DPS_S, kPa"};

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 2 * h1 + 4 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1,  2*h1, "PS",       'L');
  x = x + w1;
  add_frame_box(x, y, w2+w3, h1, "Pressure", 'S');
  y = y + h1;
  add_frame_box(x, y, w2,    h1, "Value",    'S');
  x = x + w2;
  add_frame_box(x, y, w3,    h1, "Code",     'S');
  y = y + h1;
  
  x = x0 + 5;
  create_frame_box_vstring(4, x, y, w1, h1, &txt[0], 'S', 'L');
  x = x + w1;
  create_dat_box_vstring(
    4, x, y, w2, h1, 'N', &r_pres, &pres_msk[0], &PRES_VAL_BOX[0]);
  x = x + w2;
  create_dat_box_vstring(
    4, x, y, w3, h1, 'N', &r_pres, &pres_msk[0], &PRES_COD_BOX[0]);
  for (i=0; i<4; i++) {
    if (i % 2) PRES_VAL_BOX[i]->cnv = convert_differential_pressure_code_to_value;
    else       PRES_VAL_BOX[i]->cnv = convert_absolute_pressure_code_to_value;
  }
  y = y + 4 * h1;

  x = x0 + 5;
  add_frame_box(x, y, w1, h2, "", 'S');
  x = x + w1;
  obj = add_normal_button(x, y, w2+w3, h2, "READ", 'N');
    fl_set_object_callback(obj, read_pressure_sensor_callback, 0);
}

//~----------------------------------------------------------------------------

void convert_absolute_pressure_code_to_value(int32 code, int32 *value) {

  float tmp = 6.5 * (float)code / 4096.0;
  *value = *(int32*)&tmp;
}

//~----------------------------------------------------------------------------

void convert_differential_pressure_code_to_value(int32 code, int32 *value) {

  float tmp = 100.0 * (float)code / 4096.0;
  *value = *(int32*)&tmp;
}

//~----------------------------------------------------------------------------

void read_pressure_sensor_callback(FL_OBJECT* obj, long data) {

  int i;
  int16 dat[4];
  _r  *r = &r_pres;
    
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<4; i++) r->was[i] = r->rea[i];
  
  read_TTCE_pressure(dat, &err);

  if (!err) {
    for (i=0; i<4; i++) r->rea[i] = dat[i];
    if (r->unk) for (i=0; i<4; i++) r->was[i] = r->rea[i];
    r->unk  = FALSE;
    r->sta  = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_pressure_sensor_panel(void) {

  show_dat_box_string(4, NULL,  "%03X", &PRES_COD_BOX[0]);
  show_dat_box_string(4, NULL, "%5.1f", &PRES_VAL_BOX[0]);
}

//~============================================================================
//
//                  PT1000 SENSOR PANEL
//
//~============================================================================

extern _b pt_rc_msk;
extern _b pt_sel_msk;
extern _b pt_adc_msk[3][15];

void create_Pt_sensor_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 45, w2 = 15, w3 = 20, w4 = 35, w5 = 55, h1 = 15, h2 = 20;
  char *txt[] = {
    "REF", 
    "Pt01_P", "Pt02_P", "Pt03_P", "Pt04_P", "Pt05_P", "Pt06_P", 
    "Pt07_P", "Pt08_P", "Pt09_P", "Pt10_P", "Pt11_P", 
    "Pt01_S", "Pt02_S", "Pt03_S", "Pt04_S", "Pt05_S", "Pt06_S", 
    "Pt07_S", "Pt08_S", "Pt09_S", "Pt10_S", "Pt11_S"};

  int matrix[6][3] = {  // Thank you, Vladimir!
    { 6,  0,  0},
    { 0,  6,  0},
    { 0,  0,  6},
    { 7,  0,  0},
    { 0,  7,  0},
    { 0,  0,  7}};

//~--

  w = 5 + w1 + w2 + w3 + 3 * (w4 + w5) + 2 * w2 + w5 + w1 + 5;
  h = 5 + 2 * h1 + h1 + 5 + 11 * h1 + 5 + 11 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Pt", 'L');
  x = x + w1;
  add_frame_box(x, y, w2, 2*h1, "R\nC", 'S');
  x = x + w2;
  add_frame_box(x, y, w3, 2*h1, "", 'S');
  x = x + w3;
  for (i=0; i<3; i++) {
    add_frame_box(x, y, w4+w5, h1, nlr[i], 'S');
    x = x + w4 + w5;
  }
  add_frame_box(x, y, w2, 2*h1, "R\nC", 'S');
  x = x + w2;
  add_frame_box(x, y, w2, 2*h1, "V\nR", 'S');
  x = x + w2;
  add_frame_box(x, y, w5, 2*h1, "Selected\nTemp.", 'S');
  x = x + w5;
  add_frame_box(x, y, w1, 2*h1, "", 'S');

//~--

  x = x0 + 5 + w1 + w2 + w3;
  y = y + h1;
  for (i=0; i<3; i++) {
    add_frame_box(x, y, w4, h1, "Code", 'S');
    x = x + w4;
    add_frame_box(x, y, w5, h1, "Temp.", 'S');
    x = x + w5;
  }

//~--

  x = x0 + 5;
  y = y + h1;
  create_frame_box_vstring( 1, x, y, w1, h1, &txt[0], 'S', 'L');
  y = y + h1 + 5;
  create_frame_box_vstring(22, x, y, w1, h1, &txt[1], 'S'+1100, 'L');
  y = y + 11 * h1 + 5 + 11 * h1;
  add_frame_box(x, y, w1, h2, "", 'S');
  x = x + w1;
  obj = add_normal_button(x, y, w2+w3, h2, "W", 'N');
    fl_set_object_callback(obj, write_Pt_sensor_control_callback, -1);
  x = x + w2 + w3;
  w = 3 * (w4 + w5);
  obj = add_normal_button(x, y, w,  h2, "READ", 'N');
    fl_set_object_callback(obj, read_all_Pt_sensor_data_callback, 0);
  x = x + w;
  w = 2 * w2 + w5;
  obj = add_normal_button(x, y, w, h2, "READ", 'N');
    fl_set_object_callback(obj, read_selected_Pt_sensor_data_callback, 0);
  x = x + w;
  add_frame_box(x, y, w1, h2, "", 'S');
  
//~--

  x = x0 + 5 + w1;
  y = y0 + 5 + 2 * h1;
  add_frame_box(x, y, w2+w3, h1, "", 'S');
  y = y + h1 + 5;
  for (i=0; i<5; i++) {
    create_btn_vstring(
      1, x, y, w2, h1, nlrv, 'N', &w_pt_rc[i], &pt_rc_msk, &PT_RC_BTN[i]);
    obj = add_normal_button(x+w2, y, w3, h1, "W", 'N');
      fl_set_object_callback(obj, write_Pt_sensor_control_callback, i);
    y = y + h1;
  }
  for (i=0; i<6; i++) {
    add_frame_box(x, y, w2+w3, h1, "", 'S');
    y = y + h1;
  }
  y = y + 5;
  for (i=5; i<10; i++) {
    create_btn_vstring(
      1, x, y, w2, h1, nlrv, 'N', &w_pt_rc[i], &pt_rc_msk, &PT_RC_BTN[i]);
    obj = add_normal_button(x+w2, y, w3, h1, "W", 'N');
      fl_set_object_callback(obj, write_Pt_sensor_control_callback, i);
    y = y + h1;
  }
  for (i=0; i<6; i++) {
    add_frame_box(x, y, w2+w3, h1, "", 'S');
    y = y + h1;
  }
  x = x0 + 5 + w1 + w2 + w3;
  
//~--

  for (i=0; i<3; i++) {
    int j;
    y = y0 + 5 + 2 * h1;
    create_dat_box_vstring(
      1, x, y, w4, h1, 'N', &r_pt_adc, &pt_adc_msk[i][0], &PT_COD_BOX[i][0]);
    create_dat_box_vstring(
      1, x+w4, y, w5, h1, 'N', &r_pt_adc, &pt_adc_msk[i][0], &PT_VAL_BOX[i][0]);
    y = y + h1 + 5;
    create_dat_box_vstring(
      5, x, y, w4, h1, 'N', &r_pt_adc, &pt_adc_msk[i][1], &PT_COD_BOX[i][1]);
    create_dat_box_vstring(
      5, x+w4, y, w5, h1, 'N', &r_pt_adc, &pt_adc_msk[i][1], &PT_VAL_BOX[i][1]);
    y = y + 5 * h1;
    for (j=0; j<6; j++) {
      if (matrix[j][i]) {
        int k = matrix[j][i];
        create_dat_box_vstring(
          1, x, y, w4, h1, 'N', &r_pt_adc, &pt_adc_msk[i][k], &PT_COD_BOX[i][k]);
        create_dat_box_vstring(
          1, x+w4, y, w5, h1, 'N', &r_pt_adc, &pt_adc_msk[i][k], &PT_VAL_BOX[i][k]);
      }
      else {
        add_frame_box(x,    y, w4, h1, "", 'S');
        add_frame_box(x+w4, y, w5, h1, "", 'S');
      }
      y = y + h1;
    }
    y = y + 5;
    create_dat_box_vstring(
      5, x, y, w4, h1, 'N', &r_pt_adc, &pt_adc_msk[i][8], &PT_COD_BOX[i][8]);
    create_dat_box_vstring(
      5, x+w4, y, w5, h1, 'N', &r_pt_adc, &pt_adc_msk[i][8], &PT_VAL_BOX[i][8]);
    y = y + 5 * h1;
    for (j=0; j<6; j++) {
      if (matrix[j][i]) {
        int k = matrix[j][i] + 7;
        create_dat_box_vstring(
          1, x, y, w4, h1, 'N', &r_pt_adc, &pt_adc_msk[i][k], &PT_COD_BOX[i][k]);
        create_dat_box_vstring(
          1, x+w4, y, w5, h1, 'N', &r_pt_adc, &pt_adc_msk[i][k], &PT_VAL_BOX[i][k]);
      }
      else {
        add_frame_box(x,    y, w4, h1, "", 'S');
        add_frame_box(x+w4, y, w5, h1, "", 'S');
      }
      y = y + h1;
    }
    x = x + w4 + w5;
  }
  for (i=0; i<15; i++) {
    PT_VAL_BOX[0][i]->cnv = convert_N_Pt_code_to_temperature;
    PT_VAL_BOX[1][i]->cnv = convert_L_Pt_code_to_temperature;
    PT_VAL_BOX[2][i]->cnv = convert_R_Pt_code_to_temperature;
  }
  
//~--

  y = y0 + 5 + 2 * h1;
  add_frame_box(x, y, 2*w2, h1, "", 'S');
  y = y + h1 + 5;
  for (i=0; i<5; i++) {
    create_dat_box_vstring(
      1, x,    y, w2, h1, 'N', &r_pt_rc[i], &pt_sel_msk, &PT_RC_BOX[i]);
    create_dat_box_vstring(
      1, x+w2, y, w2, h1, 'N', &r_pt_vr[i], &pt_sel_msk, &PT_VR_BOX[i]);
    y = y + h1;
  }
  for (i=0; i<6; i++) {
    add_frame_box(x, y, 2*w2, h1, "", 'S');
    y = y + h1;
  }
  y = y + 5;
  for (i=11; i<16; i++) {
    create_dat_box_vstring(
      1, x,    y, w2, h1, 'N', &r_pt_rc[i], &pt_sel_msk, &PT_RC_BOX[i]);
    create_dat_box_vstring(
      1, x+w2, y, w2, h1, 'N', &r_pt_vr[i], &pt_sel_msk, &PT_VR_BOX[i]);
    y = y + h1;
  }
  for (i=0; i<6; i++) {
    add_frame_box(x, y, 2*w2, h1, "", 'S');
    y = y + h1;
  }
  x = x + 2 * w2;
  
//~--

  y = y0 + 5 + 2 * h1;
  add_frame_box(x, y, w5, h1, "", 'S');
  y = y + h1 + 5;
  for (i=0; i<22; i++) {
    create_dat_box_vstring(
      1, x, y, w5, h1, 'N', &r_pt_temp[i], &pt_sel_msk, &PT_TEMP_BOX[i]);
    y = y + h1;
    if (i == 10) y = y + 5;
  }
  x = x + w5;

//~--

  y = y0 + 5 + 2 * h1;
  create_frame_box_vstring( 1, x, y, w1, h1, &txt[0], 'S', 'L');
  y = y + h1 + 5;
  create_frame_box_vstring(22, x, y, w1, h1, &txt[1], 'S'+1100, 'L');
  y = y + 11 * h1 + 5 + 11 * h1;
  add_frame_box(x, y, w1, h2, "", 'S');
}

//~----------------------------------------------------------------------------

void convert_N_Pt_code_to_temperature(int32 code, int32 *value) {

  float temp = (float)(int)(code - r_pt_adc.rea[0]) / 16.0;
  *value = *(int32*)&temp;
}

//~----------------------------------------------------------------------------

void convert_L_Pt_code_to_temperature(int32 code, int32 *value) {

  float temp = (float)(int)(code - r_pt_adc.rea[1]) / 16.0;
  *value = *(int32*)&temp;
}

//~----------------------------------------------------------------------------

void convert_R_Pt_code_to_temperature(int32 code, int32 *value) {

  float temp = (float)(int)(code - r_pt_adc.rea[2]) / 16.0;
  *value = *(int32*)&temp;
}

//~----------------------------------------------------------------------------

void write_Pt_sensor_control_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i, n = 0;
  int8 adr[10], dat[10];

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<10; i++) {
    _ww *w = &w_pt_rc[i];
    int k = i < 5 ? 0 : 6;
    _r  *r = &r_pt_rc[i+k];
    _r  *v = &r_pt_vr[i+k];
    _r  *t = &r_pt_temp[i+k];
    int32 m = pt_rc_msk.msk;
    if ((ind < 0) || (ind == i)) {
      adr[n] = i;
      dat[n] = w->inp[0] & m;
      n = n + 1;
      r->sta = status_NA;
      v->sta = status_NA;
      t->sta = status_NA;
    }
  }

  write_TTCE_Pt1000_ctrl(n, adr, dat, &err);

  for (i=0; i<10; i++) {
    _ww *w = &w_pt_rc[i];
    if (!err) {
      if ((ind < 0) || (ind == i)) {
        w->wri[0] = w->inp[0];
        w->unk = FALSE;
      }
    }
    else {
      if ((ind < 0) || (ind == i)) {
        w->unk = TRUE;
      }
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_all_Pt_sensor_data_callback(FL_OBJECT* obj, long data) {

  int i;
  int n = 48;
  int16 adc[72];
  _r  *r = &r_pt_adc;
    
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<n; i++) r->was[i] = r->rea[i];
  
  read_TTCE_Pt1000_ADC(adc, &err);

  if (!err) {
    for (i=0; i<n; i++) {
      r->rea[i] = adc[i];
      if (r->unk) r->was[i] = r->rea[i];
    }
    r->unk  = FALSE;
    r->sta  = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_selected_Pt_sensor_data_callback(FL_OBJECT* obj, long data) {

  int i;
  int n = 22;
  int8 RC[22], VR[22];
  float temp[22];
    
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<n; i++) {
    _r *r = &r_pt_rc[i];
    _r *v = &r_pt_vr[i];
    _r *t = &r_pt_temp[i];
    r->was[0] = r->rea[0];
    v->was[0] = v->rea[0];
    t->was[0] = t->rea[0];
  }
  
  read_TTCE_Pt1000_temp(RC, VR, temp, &err);

  if (!err) {
    for (i=0; i<n; i++) {
      _r *r = &r_pt_rc[i];
      _r *v = &r_pt_vr[i];
      _r *t = &r_pt_temp[i];
      r->rea[0] = RC[i];
      v->rea[0] = VR[i];
      t->rea[0] = *(int32*)&temp[i];
      if (r->unk) r->was[0] = r->rea[0];
      if (v->unk) v->was[0] = v->rea[0];
      if (t->unk) t->was[0] = t->rea[0];
      r->unk  = FALSE;
      v->unk  = FALSE;
      t->unk  = FALSE;
      r->sta  = status_OK;
      v->sta  = status_OK;
      t->sta  = status_OK;
    }
  }
  else {
    for (i=0; i<n; i++) {
      _r *r = &r_pt_rc[i];
      _r *v = &r_pt_vr[i];
      _r *t = &r_pt_temp[i];
      r->sta = status_KO;
      v->sta = status_KO;
      t->sta = status_KO;
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_Pt_sensor_panel(void) {

  int i;

  for (i=0; i<10; i++) {
    show_btn_string(1, NULL, &PT_RC_BTN[i]);
  }

  for (i=0; i<3; i++) {
    show_dat_box_string(15, NULL, "%04X",   &PT_COD_BOX[i][0]);
    show_dat_box_string(15, NULL, "%+5.2f", &PT_VAL_BOX[i][0]);
  }
  
  show_dat_box_string(5, nlrv, NULL, &PT_RC_BOX[0]);
  show_dat_box_string(5, nlrv, NULL, &PT_RC_BOX[11]);
  show_dat_box_string(5, nlr,  NULL, &PT_VR_BOX[0]);
  show_dat_box_string(5, nlr,  NULL, &PT_VR_BOX[11]);

  show_dat_box_string(22, NULL, "%+5.2f", &PT_TEMP_BOX[0]);
}

//~============================================================================
//
//            READ ALL & FILE WRITING PANEL
//
//~============================================================================

_b per = {0, 0x00000007, 5};
_b rec = {0, 0x00000001, 2};

void create_read_all_file_write_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 45, w3 = 65, h1 = 20;
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Period", 'S');
  create_btn_hstring(
    1, x, y+h1, w1, h1, periods, 'N', &w_per, &per, &PER_BTN);
  x = x + w1;

  obj = add_normal_button(x, y, w2, 2*h1, "READ\nALL", 'N');
    fl_set_object_callback(obj, read_all_callback, 0);
  x = x + w2;

  add_frame_box(x, y, w3, h1, "File Writing", 'S');
  create_btn_hstring(1, x, y+h1, w3, h1, o_o, 'L', &w_rec, &rec, &REC_BTN);
}

//~----------------------------------------------------------------------------

void read_all_callback(FL_OBJECT* obj, long data) {

  if (obj) {
    if (auto_read_all) {
      auto_read_all = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_read_all = TRUE;
        fl_add_timeout(0, auto_read_all_callback, NULL);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_read_all) {
    msg = -1;
    update_objects();
  }
  
  read_28V_callback(NULL, 0);
  read_pump_parameters_callback(NULL, 0);
  read_PWM_callback(NULL, 0);
  read_pressure_sensor_callback(NULL, 0);
  read_selected_Pt_sensor_data_callback(NULL, 0);

  if (auto_read_all) msg = -3;
  else               msg = 1;

  data_updated = TRUE;
  update_objects();
}

//~----------------------------------------------------------------------------

void auto_read_all_callback(int tid, void *data) {

  int period = 1000.0 * atof(periods[*w_per.inp]);  // in ms

  if (!auto_read_all) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback dt = %f\n", delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  read_all_callback(NULL, 0);
  fl_add_timeout(period, auto_read_all_callback, NULL);
}

//~----------------------------------------------------------------------------

void record_ttce_data(FILE *file) {

  int i;

  if (!data_updated) return;
  
  fprintf(file, "%s", timestamp(1));

  if (err) {
    fprintf(file, " *** err = 0x%04X ***\n", err);
    return;
  }

  fprintf(file, ": ");

  for (i=0; i<DIM(pwr_sta_msk); i++) {
    int ind = pwr_sta_msk[i].ind;
    int msk = pwr_sta_msk[i].msk;
    fprintf(file, "%d ", (r_pwr.rea[ind]&msk ? 1 : 0));
  }

  for (i=0; i<DIM(PUMP_RPM_VAL_BOX); i++) {
    fprintf(file, "%s ", fl_get_object_label(PUMP_RPM_VAL_BOX[i]->obj));
  }

  for (i=0; i<DIM(PWM_PCT_BOX); i++) {
    fprintf(file, "%s ", fl_get_object_label(PWM_PCT_BOX[i][0]->obj));
  }

  for (i=0; i<DIM(PRES_VAL_BOX); i++) {
    fprintf(file, "%s ", fl_get_object_label(PRES_VAL_BOX[i]->obj));
  }

  for (i=0; i<DIM(PT_TEMP_BOX); i++) {
    fprintf(file, "%s ", fl_get_object_label(PT_TEMP_BOX[i]->obj));
  }

  fprintf(file, "\n");
}

//~----------------------------------------------------------------------------

void update_read_all_file_write_panel(void) {

  SW_BTN *B = REC_BTN;
  FILE *file;
  char *log_file_name = {"ttce-2-logfile.txt"};

  if (*w_rec.inp) {
    fl_set_object_color(B->obj, FL_RED, 0);
    fl_set_object_lcolor(B->obj, FL_YELLOW);
  }
  else {
    fl_set_object_color(B->obj, FL_WHEAT, 0);
    fl_set_object_lcolor(B->obj, FL_BLACK);
  }

  if (*w_rec.inp) {
    if (P > 0) record_ttce_data(stdout);
    file = fopen(log_file_name, "a+");
    if (file) {
      setbuf(file, NULL);
      record_ttce_data(file);
      fclose(file);
    }
    else {
      perror("fopen: ");
      printf("%s writing failed...%c\n", log_file_name, 7);
    }
  }

  data_updated = FALSE;
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_28V_panel();

  update_pump_control_panel();
  update_PWM_control_panel();
  update_pressure_sensor_panel();

  update_Pt_sensor_panel();

  update_read_all_file_write_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
