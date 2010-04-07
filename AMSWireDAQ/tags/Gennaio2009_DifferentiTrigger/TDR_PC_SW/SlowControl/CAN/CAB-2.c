// file CAB-2.c
//
//  CAB-2 Essential Status Controller
//
//  A.Lebedev Nov-2008...
//

#include "cabdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CAB - 2 Essential Status Controller"};       // global variable
char *date  = {"17-Jan-09"};                                 // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x18;                                   // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

int32 DO_NOT_CREATE = 0x12345678;                     // global variable

int8 CAB_CMD_ID;                                      // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;

_r r_ess_sta;

//~-- CAB Model Selector

SW_BTN *MODEL_BTN;
_ww w_model;
_b model = {0, 0x00000001, 2};

//~-- Software Status

DAT_BOX *CPU_USED, *SW_VERS[2], *CPU_TIME, *CPU_STATE[2];
extern _b cpu_used;
extern _b sw_vers[2];
extern _b cpu_time;
extern _b cpu_state[2];

//~-- Start-Up Report

DAT_BOX *CRC_EXP[4], *CRC_CALC[5], *RAM_TEST[2];
extern _b exp_crc[4];
extern _b calc_crc[5];
extern _b ram_test[2];

//~-- Software Sequences

DAT_BOX *SW_SEQ[6];
extern _b sw_seq[6];

//~-- Reset Cause

DAT_BOX *RESET_CAUSE[6];
extern _b reset_cause[6];

//~-- CCSC Status

DAT_BOX *OTHER_DCDC_CTRL, *LATCH_UP_CTRL;
extern _b other_dcdc_ctrl[];
extern _b latch_up_ctrl[];

//~-- PS Status

DAT_BOX *PS_CV[2];
extern _b ps_cv[2];

//~-- CSP Status

DAT_BOX *CSP_CV[8], *LCL_BAT_CTRL[2], *CSP_ALARM;
extern _b csp_cv[8];
extern _b lcl_bat_ctrl[2];
extern _b csp_alarm[];
DAT_BOX *CSP_ALA_SUM[2][7];
_r r_csp_ala_sum[2];
extern _b csp_ala_sum[7];

//~-- CCS Status

DAT_BOX *CCS_SEMIC_SW, *CCS_OUT_CTRL;
extern _b ccs_semic_sw[];
extern _b ccs_out_ctrl[];

//~-- CAB Acknoledgement Code

FL_OBJECT *CAB_ACK[2];
_r r_cab_ack;

//~-----

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *n_r[3] = {"?", "N", "R"};
char *s_a[3] = {"?", "Start-Up", "Active"};
char *f_o[2] = {"FAIL", "O.K."};
char *x__[2] = {" ", "@9+"};
char *o_o[3] = {"OFF", "ON", "?"};
char *d_e[3] = {"DIS", "ENA", "?"};
char *n_a[3] = {"NO ALARM", "ALARM", "?"};
char *o_c[3] = {"OPN", "CLS", "?"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void get_essential_status_callback(FL_OBJECT* obj, long data);

void create_model_selector_panel(int x0, int y0);

void create_software_status_panel(int x0, int y0);
void reset_SW_callback(FL_OBJECT* obj, long data);
void load_PGM_callback(FL_OBJECT* obj, long data);
void go_APSW_callback(FL_OBJECT* obj, long data);
void update_software_status_panel(void);

void create_start_up_report_panel(int x0, int y0);
void update_start_up_report_panel(void);

void create_software_sequences_panel(int x0, int y0);
void update_software_sequences_panel(void);

void create_reset_cause_panel(int x0, int y0);
void update_reset_cause_panel(void);

void create_CCSC_status_panel(int x0, int y0);
void other_dcdc_control_callback(FL_OBJECT* obj, long data);
void latch_up_control_callback(FL_OBJECT* obj, long data);
void update_CCSC_status_panel(void);

void create_PS_status_panel(int x0, int y0);
void PS_converters_control_callback(FL_OBJECT* obj, long data);
void update_PS_status_panel(void);

void create_CSP_status_panel(int x0, int y0);
void CSP_converters_control_callback(FL_OBJECT* obj, long data);
void LCL_battery_control_callback(FL_OBJECT* obj, long data);
void CSP_get_alarm_summary_callback(FL_OBJECT* obj, long data);
void CSP_reset_alarm_callback(FL_OBJECT* obj, long data);
void update_CSP_status_panel(void);

void create_CCS_status_panel(int x0, int y0);
void semic_sw_control_callback(FL_OBJECT* obj, long data);
void CCS_output_control_callback(FL_OBJECT* obj, long data);
void update_CCS_status_panel(void);

void create_CAB_acknoledgment_panel(FL_FORM *form);
void update_CAB_acknoledgment_panel(int16 *err);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  FL_OBJECT *obj;
  int WW = 680, HH = 745;
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_model_selector_panel(150, 0);

  create_software_status_panel(0,  45);
  create_start_up_report_panel(0, 265);

  create_software_sequences_panel(190,  45);
  create_reset_cause_panel(       190, 195);

  create_CCSC_status_panel(425,  45);
  create_PS_status_panel(  425, 115);
  create_CSP_status_panel( 425, 185);
  create_CCS_status_panel( 425, 615);
  
  obj = add_normal_button(215, 375, 185, 60, "GET  ESSENTIAL  STATUS", 'N');
    fl_set_object_callback(obj, get_essential_status_callback, 0);

  create_CAB_acknoledgment_panel(MAIN);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  static bool first = TRUE;

  invalidate_ww(first, &w_model, 1, 0);

  invalidate_r(first, &r_ess_sta, 100);
  invalidate_r(first, &r_csp_ala_sum[0], 7);
  invalidate_r(first, &r_csp_ala_sum[1], 7);
  invalidate_r(first, &r_cab_ack, 1);

  read_dat_config_file(dat_config_file_name);
  first = FALSE;
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

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(w_model.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(w_model.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            GET ESSENTIAL STATUS CALLBACK FUNCTION
//
//~============================================================================

void get_essential_status_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ess_sta;
  int i;
  int8 sta[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<100; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  CAB_get_essential_status(sta, &err);

  if (!err) {
    for (i=0; i<DIM(cab_ess_sta); i++) {
      _sta *k = &cab_ess_sta[i];
      r->rea[i] = packbytes(NULL, &sta[k->i], k->n);
    }
    if (r->unk) for (i=0; i<100; i++) r->was[i] = r->rea[i];
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

//~============================================================================
//
//            MODEL SELECTOR PANEL
//
//~============================================================================

void create_model_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 40, h1 = 15, h2 = 20;
  static char *txt[] = {"EM", "FM"};
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Model", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, txt, 'M', &w_model, &model, &MODEL_BTN);
}

//~============================================================================
//
//            SOFTWARE STATUS PANEL
//
//~============================================================================

void create_software_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;

{//~--- Panel ---

  int w1 = 190, w2 = 180, h1 = 220, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "Software Status", 'M');
  y = y + h2;
}
  
{//~--- CPU Time ---

  int w1 = 60, w2 = 120, h1 = 20;

  x = x0 + 5;

  obj = add_frame_box(x, y, w1, h1, "Time, s", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_dat_box_hstring(
    1, x, y, w2, h1, 'M', &r_ess_sta, &cpu_time, &CPU_TIME);
  x = x + w2;

  y = y + h1;
}

{//~--- Reset SW Buttons ---

  int w1 = 90, h1 = 45;

  x = x0 + 5;
  obj = add_normal_button(x,    y, w1, h1, "RESET  SW\nTO START-UP", 'N');
    fl_set_object_callback(obj, reset_SW_callback, 0);
  obj = add_normal_button(x+w1, y, w1, h1, "RESET  SW\nTO ACTIVE", 'N');
    fl_set_object_callback(obj, reset_SW_callback, 1);
  y = y + h1;
}

{//~--- Load/Go Buttons ---

  int w1 = 60, h1 = 45;

  x = x0 + 5;
  obj = add_normal_button(x,      y, w1, h1, "LOAD\nPGM 1", 'N');
    fl_set_object_callback(obj, load_PGM_callback, 0);
  obj = add_normal_button(x+w1,   y, w1, h1, "LOAD\nPGM 2", 'N');
    fl_set_object_callback(obj, load_PGM_callback, 1);
  obj = add_normal_button(x+2*w1, y, w1, h1, "GO\nAPSW",    'N');
    fl_set_object_callback(obj, go_APSW_callback, 0);
  y = y + h1;
}

{//~--- CPU Used & SW Version ---

  int w1 = 35, w2 = 20, w3 = 100, w4 = 25, h1 = 20;

  x = x0 + 5;

  obj = add_frame_box(x, y, w1, 2*h1, "CPU\nused", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_dat_box_hstring(
    1, x, y, w2, 2*h1, 'M', &r_ess_sta, &cpu_used, &CPU_USED);
  x = x + w2;

  add_frame_box(x, y,    w3, h1, "SUSW Version", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  add_frame_box(x, y+h1, w3, h1, "APSW Version", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w3;
  
  create_dat_box_vstring(
    2, x, y, w4, h1, 'M', &r_ess_sta, sw_vers, &SW_VERS[0]);
  x = x + w4;
  
  y = y + 2 * h1;
}

{//~--- CPU State ---

  int w1 = 45, w2 = 50, w3 = 85, h1 = 20;

  x = x0 + 5;

  obj = add_frame_box(x, y, w1, 2*h1, "CPU\nState", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  obj = add_frame_box(x, y,    w2, h1, "This",  'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  obj = add_frame_box(x, y+h1, w2, h1, "Other", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w2;

  create_dat_box_vstring(
    2, x, y, w3, h1, 'M', &r_ess_sta, cpu_state, &CPU_STATE[0]);
  x = x + w3;
  
  y = y + 2 * h1;
}

//~---

}

//~----------------------------------------------------------------------------

void reset_SW_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _r *r = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  if (ind == 0) CAB_reset_SW(0x8008, &err);
  if (ind == 1) CAB_reset_SW(0x0220, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void load_PGM_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _r *r = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  if (ind == 0) CAB_load_APSW(0xA2, &err);
  if (ind == 1) CAB_load_APSW(0xA4, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void go_APSW_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CAB_go_APSW(0x00000000, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_software_status_panel(void) {

  char b[80];
  _r *r = &r_ess_sta;

  show_dat_box_string(1,  n_r,     NULL, &CPU_USED);
  show_dat_box_string(2, NULL, "%02hhX", &SW_VERS[0]);
  sprintf_huge(b, (int64)(0.0125*r->rea[cpu_time.ind]), 8, 0, 1);
  show_dat_box_string(1, NULL,        b, &CPU_TIME);
  show_dat_box_string(2,  s_a,     NULL, &CPU_STATE[0]);
}

//~============================================================================
//
//            START UP REPORT PANEL
//
//~============================================================================

void create_start_up_report_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;

{//~--- Panel ---

  int w1 = 190, w2 = 180, h1 = 190, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "Start Up Report", 'M');
  y = y + h2;
}
  
{//~--- CRC

  int w1 = 60, w2 = 60, h1 = 20;
  char *txt[5] = {"PROM", "PGM 1", "PGM 2", "SW table", "RAM"};
  
  x = x0 + 5;

  add_frame_box(x,      y, w1, h1, "CRC:", 'N');
  add_frame_box(x+w1,   y, w1, h1, "Exp",  'N');
  add_frame_box(x+2*w1, y, w1, h1, "Calc", 'N');
  y = y + h1;

  create_frame_box_vstring(5, x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;

  create_dat_box_vstring(
    4, x, y, w2, h1, 'M', &r_ess_sta, exp_crc,  &CRC_EXP[0]);
  add_frame_box(x, y+4*h1, w2, h1, "", 'N');
  x = x + w2;
  create_dat_box_vstring(
    5, x, y, w2, h1, 'M', &r_ess_sta, calc_crc, &CRC_CALC[0]);
  x = x + w2;

  y = y + 5 * h1;
}

{//~--- RAM Test ---

  int w1 = 90, w2 = 45, h1 = 20;
  char *txt[2] = {"Data", "PGM"};
  
  x = x0 + 5;

  obj = add_frame_box(x, y, w1, 2*h1, "RAM Test", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_frame_box_hstring(2, x, y, w2, h1, &txt[0], 'N', 'C');
  y = y + h1;

  create_dat_box_hstring(
    2, x, y, w2, h1, 'M', &r_ess_sta, ram_test,  &RAM_TEST[0]);
  x = x + 2 * w2;

  y = y + h1;
}

//~---

}

//~----------------------------------------------------------------------------

void update_start_up_report_panel(void) {

  show_dat_box_string(4, NULL, "0x%04X", &CRC_EXP[0]);
  show_dat_box_string(5, NULL, "0x%04X", &CRC_CALC[0]);
  show_dat_box_string(2,  f_o,     NULL, &RAM_TEST[0]);
}

//~============================================================================
//
//            SOFTWARE SEQUENCES PANEL
//
//~============================================================================

void create_software_sequences_panel(int x0, int y0) {

  int x, y;

{//~--- Panel ---

  int w1 = 235, w2 = 225, h1 = 150, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "Software Sequences", 'M');
  y = y + h2;
}
  
{//~--- Software Sequences ---

  int w1 = 190, w2 = 35, h1 = 20;
  char *txt[6] = {
    "PTM Acquisition",
    "DLCM Acquisition",
    "Current SetPoint Programming",
    "Arm Quench",
    "Current Lead SEL Clear",
    "Magnet Detector SEL Clear"};
  
  x = x0 + 5;

  create_frame_box_vstring(6, x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;

  create_dat_box_vstring(
    6, x, y, w2, h1, 'M', &r_ess_sta, sw_seq,  &SW_SEQ[0]);
  x = x + w2;

  y = y + 6 * h1;
}

//~---

}

//~----------------------------------------------------------------------------

void update_software_sequences_panel(void) {

  show_dat_box_string(6, o_o, NULL, &SW_SEQ[0]);
}

//~============================================================================
//
//            RESET CAUSE PANEL
//
//~============================================================================

void create_reset_cause_panel(int x0, int y0) {

  int x, y;

{//~--- Panel ---

  int w1 = 235, w2 = 225, h1 = 150, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "Reset Cause", 'M');
  y = y + h2;
}
  
{//~--- Reset Cause ---

  int w1 = 205, w2 = 20, h1 = 20;
  char *txt[6] = {
    "Power On Reset",
    "Watchdog Reset",
    "Latch-Up Reset",
    "Commanded Reset (to Start-Up)",
    "Invalid Memory Access Reset",
    "Commanded Reset (to Active)"};
    
  x = x0 + 5;

  create_frame_box_vstring(6, x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;

  create_dat_box_vstring(
    6, x, y, w2, h1, 'M', &r_ess_sta, reset_cause,  &RESET_CAUSE[0]);
  x = x + w2;

  y = y + 6 * h1;
}

//~---

}

//~----------------------------------------------------------------------------

void update_reset_cause_panel(void) {

  show_dat_box_string(6, x__, NULL, &RESET_CAUSE[0]);
}

//~============================================================================
//
//            CCSC STATUS PANEL
//
//~============================================================================

void create_CCSC_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;

{//~--- Panel ---

  int w1 = 255, w2 = 245, h1 = 70, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "CCSC Status", 'M');
  y = y + h2;
}
  
{//~--- Other DC/DC Converter ---

  int w1 = 140, w2 = 35, h1 = 20;

  x = x0 + 5;

  obj = add_frame_box(x, y, w1, h1, "Other DCDC CV", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_dat_box_hstring(
    1, x, y, w2, h1, 'M', &r_ess_sta, other_dcdc_ctrl, &OTHER_DCDC_CTRL);
    OTHER_DCDC_CTRL->cnv = convert_0x50_0x5F;
  x = x + w2;

  obj = add_normal_button(x, y, w2, h1, "ON", 'N');
    fl_set_object_callback(obj, other_dcdc_control_callback, 1);
  x = x + w2;

  obj = add_normal_button(x, y, w2, h1, "OFF", 'N');
    fl_set_object_callback(obj, other_dcdc_control_callback, 0);
  x = x + w2;

  y = y + h1;
}

{//~--- Latch Up Protection ---

  int w1 = 140, w2 = 35, h1 = 20;

  x = x0 + 5;

  obj = add_frame_box(x, y, w1, h1, "Latch-Up Protection", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_dat_box_hstring(
    1, x, y, w2, h1, 'M', &r_ess_sta, latch_up_ctrl, &LATCH_UP_CTRL);
    LATCH_UP_CTRL->cnv = convert_0x50_0x5F;
  x = x + w2;

  obj = add_normal_button(x, y, w2, h1, "ENA", 'N');
    fl_set_object_callback(obj, latch_up_control_callback, 1);
  x = x + w2;

  obj = add_normal_button(x, y, w2, h1, "DIS", 'N');
    fl_set_object_callback(obj, latch_up_control_callback, 0);
  x = x + w2;

  y = y + h1;
}

//~---

}

//~----------------------------------------------------------------------------

void latch_up_control_callback(FL_OBJECT* obj, long data) {

  int8 ctrl = data;
  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CAB_latch_up_protect(ctrl, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void other_dcdc_control_callback(FL_OBJECT* obj, long data) {

msg = -2;
update_objects();
}

//~----------------------------------------------------------------------------

void update_CCSC_status_panel(void) {

  show_dat_box_string(1, o_o, NULL, &OTHER_DCDC_CTRL);
  show_dat_box_string(1, d_e, NULL, &LATCH_UP_CTRL);
}

//~============================================================================
//
//            PS STATUS PANEL
//
//~============================================================================

void create_PS_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;

{//~--- Panel ---

  int w1 = 255, w2 = 245, h1 = 70, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "PS Status", 'M');
  y = y + h2;
}
  
{//~--- PS Converters ---

  int i;
  int w1 = 140, w2 = 35, h1 = 20;
  char *txt[2] = {
    "Nominal PS CV",
    "Redundant PS CV"};
    
  x = x0 + 5;

  create_frame_box_vstring(2, x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;

  create_dat_box_vstring(
    2, x, y, w2, h1, 'M', &r_ess_sta, ps_cv,  &PS_CV[0]);
  for (i=0; i<2; i++) PS_CV[i]->cnv = convert_0x50_0x5F;
  x = x + w2;

  for (i=0; i<2; i++) {
    obj = add_normal_button(x,    y, w2, h1, "ON", 'N');
      fl_set_object_callback(obj, PS_converters_control_callback, 2*i+1);
    obj = add_normal_button(x+w2, y, w2, h1, "OFF", 'N');
      fl_set_object_callback(obj, PS_converters_control_callback, 2*i+0);
    y = y + h1;
  }
}

//~---

}

//~----------------------------------------------------------------------------

void PS_converters_control_callback(FL_OBJECT* obj, long data) {

  int8 cha  = data / 2;
  int8 ctrl = data % 2;
  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  PS_on_off_CV(cha, ctrl, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_PS_status_panel(void) {

  show_dat_box_string(2, o_o, NULL, &PS_CV[0]);
}

//~============================================================================
//
//            CSP STATUS PANEL
//
//~============================================================================

void create_CSP_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;

{//~--- Panel ---

  int w1 = 255, w2 = 245, h1 = 430, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "CSP Status", 'M');
  y = y + h2;
}
  
{//~--- CSP Converters ---

  int i;
  int w1 = 140, w2 = 35, h1 = 20;
  char *txt[] = {
    "Magnet Detector CV 1",
    "Magnet Detector CV 2",
    "Magnet Detector CV 3",
    "Current Leads CV 1",
    "Current Leads CV 2",
    "Current Leads CV 3",
    "Nominal Control CV",
    "Redundant Control CV"};
    
  x = x0 + 5;

  create_frame_box_vstring(DIM(txt), x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;

  create_dat_box_vstring(
    DIM(txt), x, y, w2, h1, 'M', &r_ess_sta, csp_cv,  &CSP_CV[0]);
  x = x + w2;

  for (i=0; i<DIM(txt); i++) {
    obj = add_normal_button(x,    y, w2, h1, "ON", 'N');
      fl_set_object_callback(obj, CSP_converters_control_callback, 2*i+1);
    obj = add_normal_button(x+w2, y, w2, h1, "OFF", 'N');
      fl_set_object_callback(obj, CSP_converters_control_callback, 2*i+0);
    y = y + h1;
  }
}

{//~--- LCL Battery ---

  int i;
  int w1 = 140, w2 = 35, h1 = 20;
  char *txt[] = {
    "LCL Battery N",
    "LCL Battery R"};

  x = x0 + 5;

  create_frame_box_vstring(DIM(txt), x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;

  create_dat_box_vstring(
    DIM(txt), x, y, w2, h1, 'M', &r_ess_sta, lcl_bat_ctrl,  &LCL_BAT_CTRL[0]);
  x = x + w2;

  for (i=0; i<DIM(txt); i++) {
    obj = add_normal_button(x,    y, w2, h1, "ON", 'N');
      fl_set_object_callback(obj, LCL_battery_control_callback, 2*i+1);
    obj = add_normal_button(x+w2, y, w2, h1, "OFF", 'N');
      fl_set_object_callback(obj, LCL_battery_control_callback, 2*i+0);
    y = y + h1;
  }
}

{//~--- CSP Alarm ---

  int w1 = 140, w2 = 105, h1 = 20;
  char *txt[] = {"CSP Alarm"};

  x = x0 + 5;

  create_frame_box_vstring(DIM(txt), x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;

  create_dat_box_vstring(
    DIM(txt), x, y, w2, h1, 'M', &r_ess_sta, csp_alarm,  &CSP_ALARM);
  CSP_ALARM->cnv = convert_0x00_0xFF;
  y = y + h1;
}

{//~--- CSP Alarm Summary (i.e. Details)---

  int i, j;
  int w1 = 185, w2 = 20, h1 = 20;
  char *txt[] = {
    "Chain",
    "Magnet Alarm",
    "Magnet Quench Alarm",
    "CL Upper Threshold Alarm",
    "Manual RDS Alarm",
    "Auto RDS Alarm",
    "CL Lower Threshold Alarm",
    "Get Alarm Summary"};

  x = x0 + 5;
  add_frame_box(x, y, w1, h1, "CSP Alarm Summary:", 'N');
  x = x + w1;
  add_frame_box(x, y, w2,   h1, "N",       'N');
  x = x + w2;
  add_frame_box(x, y, w2,   h1, "R",       'N');
  x = x + w2;
  add_frame_box(x, y, w2, 2*h1, "R\nS\nT", 'S');
  y = y + h1;

  x = x0 + 5;
  create_frame_box_vstring(DIM(txt), x, y, w1, h1, &txt[0], 'N', 'L');
  x = x + w1;
  for (i=0, j=0; i<DIM(txt)-1; i++) {
    create_dat_box_vstring(
      1, x,    y, w2, h1, 'M', &r_csp_ala_sum[0], &csp_ala_sum[i], &CSP_ALA_SUM[0][i]);
    if (i) CSP_ALA_SUM[0][i]->cnv = convert_0x50_0x5F;
    create_dat_box_vstring(
      1, x+w2, y, w2, h1, 'M', &r_csp_ala_sum[1], &csp_ala_sum[i], &CSP_ALA_SUM[1][i]);
    if (i) CSP_ALA_SUM[1][i]->cnv = convert_0x50_0x5F;
    if (i == 1 || i == 4 || i == 6) {
      int h[3] = {3*h1, 2*h1, h1};
      obj = add_normal_button(x+2*w2, y, w2, h[j], "R", 'N');
        fl_set_object_callback(obj, CSP_reset_alarm_callback, 1<<j);
      j = j + 1;
    }
    y = y + h1;
  }

  obj = add_normal_button(x, y, w2, h1, "G", 'N');
    fl_set_object_callback(obj, CSP_get_alarm_summary_callback, 0);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "G", 'N');
    fl_set_object_callback(obj, CSP_get_alarm_summary_callback, 1);
  x = x + w2;
  add_frame_box(x, y, w2,   h1, " ",       'N');
}

//~---

}

//~----------------------------------------------------------------------------

void CSP_converters_control_callback(FL_OBJECT* obj, long data) {

  int8 cha  = data / 2;
  int8 ctrl = data % 2;
  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CSP_on_off_converters(cha, ctrl, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void LCL_battery_control_callback(FL_OBJECT* obj, long data) {

  int8 cha  = data / 2;
  int8 ctrl = data % 2;
  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CAB_on_off_LCL_battery(cha, ctrl, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void CSP_get_alarm_summary_callback(FL_OBJECT* obj, long data) {

  int8 cha = data;
  int i;
  int8 dat[7];
  _r *r = &r_csp_ala_sum[cha];
 
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<7; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  CSP_get_alarm_summary(cha, dat, &err);

  if (!err) {
    for (i=0; i<7; i++) r->rea[i] = dat[i];
    if (r->unk) for (i=0; i<7; i++) r->was[i] = r->rea[i];
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

void CSP_reset_alarm_callback(FL_OBJECT* obj, long data) {

  int8 msk = data;
  int i;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<2; i++) {
    _r *r = &r_csp_ala_sum[i];
    if (r->sta != status_KO) r->sta = status_NA;
  }

  CSP_reset_alarm(msk, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_CSP_status_panel(void) {

  show_dat_box_string(8, o_o, NULL, &CSP_CV[0]);
  show_dat_box_string(2, o_o, NULL, &LCL_BAT_CTRL[0]);
  show_dat_box_string(1, n_a, NULL, &CSP_ALARM);
  show_dat_box_string(1, n_r, NULL, &CSP_ALA_SUM[0][0]);
  show_dat_box_string(1, n_r, NULL, &CSP_ALA_SUM[1][0]);
  show_dat_box_string(6, x__, NULL, &CSP_ALA_SUM[0][1]);
  show_dat_box_string(6, x__, NULL, &CSP_ALA_SUM[1][1]);
}

//~============================================================================
//
//            CCS STATUS PANEL
//
//~============================================================================

void create_CCS_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;

{//~--- Panel ---

  int w1 = 255, w2 = 245, h1 = 70, h2 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "CCS Status", 'M');
  y = y + h2;
}
  
{//~--- Semiconductor Switch ---

  int w1 = 140, w2 = 35, h1 = 20;
    
  x = x0 + 5;

  obj = add_frame_box(x, y, w1, h1, "Semiconductor SW", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_dat_box_vstring(
    1, x, y, w2, h1, 'M', &r_ess_sta, ccs_semic_sw,  &CCS_SEMIC_SW);
    CCS_SEMIC_SW->cnv = convert_0x50_0x5F;
  x = x + w2;

  obj = add_normal_button(x, y, w2, h1, "OPN", 'N');
    fl_set_object_callback(obj, semic_sw_control_callback, 1);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "CLS", 'N');
    fl_set_object_callback(obj, semic_sw_control_callback, 0);
  y = y + h1;
}

{//~--- CCS Output ---

  int w1 = 140, w2 = 35, h1 = 20;
    
  x = x0 + 5;

  obj = add_frame_box(x, y, w1, h1, "CCS Output Enable", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_dat_box_vstring(
    1, x, y, w2, h1, 'M', &r_ess_sta, ccs_out_ctrl,  &CCS_OUT_CTRL);
    CCS_OUT_CTRL->cnv = convert_0x50_0x5F;
  x = x + w2;

  obj = add_normal_button(x, y, w2, h1, "ENA", 'N');
    fl_set_object_callback(obj, CCS_output_control_callback, 1);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "DIS", 'N');
    fl_set_object_callback(obj, CCS_output_control_callback, 0);
  y = y + h1;
}

//~---

}

//~----------------------------------------------------------------------------

void semic_sw_control_callback(FL_OBJECT* obj, long data) {

  int8 ctrl = data % 2;
  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CCS_open_close_semic_sw(ctrl, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void CCS_output_control_callback(FL_OBJECT* obj, long data) {

  int8 ctrl = data % 2;
  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CCS_ena_dis_ouput(ctrl, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_CCS_status_panel(void) {

  show_dat_box_string(1, o_c, NULL, &CCS_SEMIC_SW);
  show_dat_box_string(1, d_e, NULL, &CCS_OUT_CTRL);
}

//~============================================================================
//
//            CAB ACKNOLEDGMENT PANEL
//
//~============================================================================

void create_CAB_acknoledgment_panel(FL_FORM *form) {

  FL_OBJECT *obj;
  int x0, y0, x, y, w, h;
  int w1 = 35, w2 = 290, w3, h1 = 20;

  x0 = form->x;
  y0 = form->y + form->h - 30 - 5 - h1 - 5;
  w3 = form->w - 5 - w1 - w2 - 5;

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "CAB:", 'S');
  x = x + w1;
  
  CAB_ACK[0] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w2, h1, "");
    fl_set_object_color(obj, FL_WHITE, FL_MCOL);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
  x = x + w2;

  CAB_ACK[1] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w3, h1, "");
    fl_set_object_color(obj, FL_WHITE, FL_MCOL);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
}

//~----------------------------------------------------------------------------

void update_CAB_acknoledgment_panel(int16 *err) {

  char b[2][80];
  
  sprintf(b[0], " ");
  sprintf(b[1], " ");
  if (msg == 1) {
    sprintf(b[0], "%s... ", cab_cmd_name(CAB_CMD_ID));
    if (*err > 0x2000) sprintf(b[1], "%s", cab_ack_message(*err-0x2000));
    else {
      if (*err) sprintf(b[1], "Command did not reach CAB");
      else      sprintf(b[1], "%s", cab_ack_message(0x00));
    }
  }
  fl_set_object_label(CAB_ACK[0], b[0]);
  fl_set_object_label(CAB_ACK[1], b[1]);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_software_status_panel();
  update_start_up_report_panel();

  update_software_sequences_panel();
  update_reset_cause_panel();

  update_CCSC_status_panel();
  update_PS_status_panel();
  update_CSP_status_panel();
  update_CCS_status_panel();

  update_CAB_acknoledgment_panel(&err);

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
