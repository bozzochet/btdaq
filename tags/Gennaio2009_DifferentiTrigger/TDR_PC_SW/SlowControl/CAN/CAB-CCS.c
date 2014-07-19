// file CAB-CCS.c
//
//  CAB-CCS Controller
//
//  A.Lebedev Dec-2008...
//

#include "cabdef.h"
#include "crisalib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CAB - CCS Controller"};     // global variable
char *date  = {"17-Jan-09"};                // global variable

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

//~-- CAB Model Selector

SW_BTN *MODEL_BTN;
_ww w_model;
_b model = {0, 0x00000001, 2};

//~-- CCS Panel

SW_BTN  *CUR_BTN[3][3], *VEL_BTN, *DCDC_BTN[6];
DAT_BOX *INT_BOX[2], *CUR_BOX[3], *SEQ_BOX;
DAT_BOX *ARM_BOX, *SEMI_BOX, *OUTP_BOX, *DCDC_BOX[6];
DAT_BOX *CPU_TIME, *CCS_TM[14];
_ww w_cur[3];
_ww w_vel;
_ww w_dcdc;
_r r_cur;
_r r_ess_sta;
_r r_dig_sta[3];   // yes, three!
_r r_ccs_tm;

//~-- CAB Acknoledgement Code

FL_OBJECT *CAB_ACK[2];
_r r_cab_ack;

//~-----

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
char *d_a[3] = {"DISARMED", "ARMED",   "?"};
char *c_o[3] = {"CLOSED",   "OPEN",    "?"};
char *d_e[3] = {"DISABLED", "ENABLED", "?"};
char *o_o[3] = {"OFF",      "ON",      "-"};
char *o_r[2] = {"OFF",      "RUN"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void get_essential_status_callback(FL_OBJECT* obj, long data);
void get_digital_status_callback(FL_OBJECT* obj, long data);

void create_model_selector_panel(int x0, int y0);

void create_CPU_control_panel(int x0, int y0);
void reset_SW_callback(FL_OBJECT* obj, long data);
void load_PGM_callback(FL_OBJECT* obj, long data);
void go_APSW_callback(FL_OBJECT* obj, long data);

void create_CCS_panel(int x0, int y0);
void control_inter_dcdc_callback(FL_OBJECT* obj, long data);
void set_current_setpoint_callback(FL_OBJECT* obj, long data);
void get_current_setpoint_callback(FL_OBJECT* obj, long data);
void start_setpoint_sequence_callback(FL_OBJECT* obj, long data);
void control_CCS_callback(FL_OBJECT* obj, long data);
void control_dcdc_callback(FL_OBJECT* obj, long data);
void set_dcdc_callback(FL_OBJECT* obj, long data);
void acquire_CCS_TM_callback(FL_OBJECT* obj, long data);
void read_CCS_TM_callback(FL_OBJECT* obj, long data);
void update_CCS_panel(void);

void create_CAB_acknoledgment_panel(FL_FORM *form);
void update_CAB_acknoledgment_panel(int16 *err);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 580, HH = 550;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_model_selector_panel(150, 0);

  create_CPU_control_panel(0, 45);
  create_CCS_panel(110, 45);

  create_CAB_acknoledgment_panel(MAIN);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  invalidate_ww(first, &w_model, 1, 0);

  for (i=0; i<3; i++) invalidate_ww(first, &w_cur[i], 1, 0);
  invalidate_r(first, &r_cur, 3);

  invalidate_ww(first, &w_vel,  1, 1);
  invalidate_ww(first, &w_dcdc, 6, 0x50);

  invalidate_r(first, &r_ess_sta, 100);
  for (i=0; i<3; i++) invalidate_r(first, &r_dig_sta[i], 100);  // yes, three
  invalidate_r(first, &r_ccs_tm, 100);

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
  int i;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(w_model.inp, 4, 1, file);
    for (i=0; i<3; i++) {
      fread(w_cur[i].inp, 4, 1, file);
    }
    fread(w_vel.inp,  4, 1, file);
    fread(w_dcdc.inp, 4, 6, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(w_model.inp, 4, 1, file);
    for (i=0; i<3; i++) {
      fwrite(w_cur[i].inp, 4, 1, file);
    }
    fwrite(w_vel.inp,  4, 1, file);
    fwrite(w_dcdc.inp, 4, 6, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            GET ESSENTIAL & DIGITAL STATUS CALLBACK FUNCTIONS
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

//~-------------------------------------------------------------------------------

void get_digital_status_callback(FL_OBJECT* obj, long data) {

  int ch  = data / 2;
  int cha = data % 2;
  int i;
  _r *r = &r_dig_sta[ch?2:cha];
  int8 sta[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<100; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  CAB_get_digital_status(cha, sta, &err);

  if (!err) {
    for (i=0; i<DIM(cab_dig_sta); i++) {
      _sta *k = &cab_dig_sta[i];
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
//            CPU SOFTWARE CONTROL PANEL
//
//~============================================================================

void create_CPU_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 100, h1 = 25;

  w = 5 + w1 + 5;
  h = 5 + 6 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1, h1, "CPU", 'M');
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "RST to StartUp", 'N');
    fl_set_object_callback(obj, reset_SW_callback, 0);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "RST to Active",    'N');
    fl_set_object_callback(obj, reset_SW_callback, 1);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "LOAD  PGM  1",   'N');
    fl_set_object_callback(obj, load_PGM_callback, 0);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "LOAD  PGM  2",   'N');
    fl_set_object_callback(obj, load_PGM_callback, 1);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "GO  APSW",                 'N');
    fl_set_object_callback(obj, go_APSW_callback, 0);
  y = y + h1;
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

//~============================================================================
//
//            CCS PANEL
//
//~============================================================================

void create_CCS_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, x1, y1;

{//~--- Panel ---

  int w1 = 470, w2 = 460, h1 = 445, h2 = 30;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "CCS  Control & Monitoring", 'M');
  y = y1 = y + h2;
}

{//~--- Interpoint DC/DC Converter Control ---

  int w1 = 90, w2 = 20, w3 = 35, w4 = 35, w5 = 35, w6 = 20, h1 = 20;

  x = x0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Interpoint\nDC/DC CV", 'N');
  x = x + w1;
  add_frame_box(x, y,    w2, h1, "N", 'N');
  add_frame_box(x, y+h1, w2, h1, "R", 'N');
  x = x + w2;
  obj = add_normal_button(x, y,    w3, h1, "ON", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*0+1);
  obj = add_normal_button(x, y+h1, w3, h1, "ON", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*1+1);
  x = x + w3;
  obj = add_normal_button(x, y,    w4, h1, "OFF", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*0+0);
  obj = add_normal_button(x, y+h1, w4, h1, "OFF", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*1+0);
  x = x + w4;
  create_dat_box_vstring(
    1, x, y,    w5, h1, 'M', &r_dig_sta[0], &ps_cmd_sw_sta[0], &INT_BOX[0]);
  create_dat_box_vstring(
    1, x, y+h1, w5, h1, 'M', &r_dig_sta[1], &ps_cmd_sw_sta[0], &INT_BOX[1]);
  x = x + w5;
  obj = add_normal_button(x, y,    w6, h1, "R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 0);
  obj = add_normal_button(x, y+h1, w6, h1, "R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 1);
  y = y + 2 * h1;
}

{//~--- Current Setpoints ---

  int w1 = 90, w2 = 15, w3 = 15, w4 = 20, w5 = 45, w6 = 20, h1 = 20;
  _b cur_w[3] = {
     {0, 0x0F00, 16},
     {0, 0x00F0, 16},
     {0, 0x000F, 16}};
  _b cur_r[3] = {
     {0, 0x0FFF, 4096},
     {1, 0x0FFF, 4096},
     {2, 0x0FFF, 4096}};

  x = x0 + 5;
  add_frame_box(x, y, w1, 3*h1, "Current\nSetpoints", 'N');
  x = x + w1;
  create_frame_box_vstring(3, x, y, w2, h1, &hex[1], 'N', 'C');
  x = x1 = x + w2;

  for (i=0; i<3; i++) {
    x = x1;
    create_btn_hstring(
      3, x, y, w3, h1, hex, 'M', &w_cur[i], cur_w, &CUR_BTN[i][0]);
    x = x + 3 * w3;
    obj = add_normal_button(x, y, w4, h1, "W", 'N');
      fl_set_object_callback(obj, set_current_setpoint_callback, i);
    x = x + w4;
    if (i == 0) {
      create_dat_box_vstring(
        3, x, y, w5, h1, 'M', &r_cur, cur_r, &CUR_BOX[0]);
      x = x + w5;
      obj = add_normal_button(x, y, w6, 3*h1, "R", 'N');
        fl_set_object_callback(obj, get_current_setpoint_callback, 0);
    }
    y = y + h1;
  }
}

{//~--- Setpoint Sequence ---

  int w1 = 90, w2 = 55, w3 = 55, w4 = 35, h1 = 20;
  _b vel = {0, 0x0007, 8};
  static char *velocity[] = {NULL, "1", "2", "3", "4", "5", "6", "7"};

  x = x0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Setpoint\nSequence", 'N');
  x = x + w1;

  add_frame_box(x, y, w2,   h1, "Velocity", 'N');
  create_btn_hstring(
    1, x, y+h1, w2, h1, velocity, 'M', &w_vel, &vel, &VEL_BTN);
  x = x + w2;

  obj = add_normal_button(x, y,    w3, h1, "START", 'N');
    fl_set_object_callback(obj, start_setpoint_sequence_callback, 0);
  obj = add_normal_button(x, y+h1, w3, h1, "CANCEL", 'N');
    fl_set_object_callback(obj, control_CCS_callback, (int32)"CANCEL");
  x = x + w3;

  create_dat_box_hstring(
    1, x, y, w4, h1, 'M', &r_ess_sta, &sw_seq[2], &SEQ_BOX);
  obj = add_normal_button(x, y+h1, w4, h1, "R", 'N');
    fl_set_object_callback(obj, get_essential_status_callback, 0);
  y = y + 2 * h1 + 5;
}

{//~--- Arm/Disarm, Semic Switch, Ouput ---

  int w1 = 90, w2 = 45, w3 = 55, h1 = 20;

  x = x0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Arm/Disarm\nCommanding", 'N');
  x = x1 = x + w1;

  create_dat_box_hstring(
    1, x, y, 2*w2, h1, 'M', &r_dig_sta[2], ccs_cmd_sta, &ARM_BOX);
  ARM_BOX->cnv = convert_0x50_0x5F;
  x = x + 2 * w2;
  obj = add_normal_button(x, y, w3, h1, "ARM", 'N');
    fl_set_object_callback(obj, control_CCS_callback, (int32)"ARM");
  y = y + h1;

  x = x1;
  obj = add_normal_button(x, y, w2, h1, "R-N", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 2);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "R-R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 3);
  x = x + w2;

  obj = add_normal_button(x, y, w3, h1, "DISARM", 'N');
    fl_set_object_callback(obj, control_CCS_callback, (int32)"DISARM");
  y = y + h1;

//~---

  x = x0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Semiconduc.\nSwitch", 'N');
  x = x1 = x + w1;

  create_dat_box_hstring(
    1, x, y, 2*w2, h1, 'M', &r_ess_sta, ccs_semic_sw, &SEMI_BOX);
  SEMI_BOX->cnv = convert_0x50_0x5F;
  x = x + 2 * w2;
  obj = add_normal_button(x, y, w3, h1, "OPEN", 'N');
    fl_set_object_callback(obj, control_CCS_callback, (int32)"OPEN");
  y = y + h1;

  x = x1;
  obj = add_normal_button(x, y, 2*w2, h1, "READ", 'N');
    fl_set_object_callback(obj, get_essential_status_callback, 0);
  x = x + 2 * w2;

  obj = add_normal_button(x, y, w3, h1, "CLOSE", 'N');
    fl_set_object_callback(obj, control_CCS_callback, (int32)"CLOSE");
  y = y + h1;

//~---

  x = x0 + 5;
  add_frame_box(x, y, w1, 2*h1, "CCS\nOutput", 'N');
  x = x1 = x + w1;

  create_dat_box_hstring(
    1, x, y, 2*w2, h1, 'M', &r_ess_sta, ccs_out_ctrl, &OUTP_BOX);
  OUTP_BOX->cnv = convert_0x50_0x5F;
  x = x + 2 * w2;
  obj = add_normal_button(x, y, w3, h1, "ENABLE", 'N');
    fl_set_object_callback(obj, control_CCS_callback, (int32)"ENABLE");
  y = y + h1;

  x = x1;
  obj = add_normal_button(x, y, 2*w2, h1, "READ", 'N');
    fl_set_object_callback(obj, get_essential_status_callback, 0);
  x = x + 2 * w2;

  obj = add_normal_button(x, y, w3, h1, "DISABLE", 'N');
    fl_set_object_callback(obj, control_CCS_callback, (int32)"DISABLE");
  y = y + h1;
}

{//~--- DC/DC Converters ---

  int y1;
  int w1 = 90, w2 = 15, w3 = 25, w4 = 30, w5 = 35, w6 = 40, h1 = 20;
  _b dcdc[6] = {
    {0, 0x0003, 3},
    {0, 0x000C, 3},
    {0, 0x0030, 3},
    {0, 0x00C0, 3},
    {0, 0x0300, 3},
    {0, 0x0C00, 3}};

  x = x0 + 5;
  add_frame_box(x, y, w1, 7*h1, "DC/DC\nConverters", 'N');
  x = x1 = x + w1;

  create_frame_box_vstring(6, x, y, w2, h1, &hex[1], 'N', 'C');
  add_frame_box(x, y+6*h1, w2+w3+w4, h1, "", 'N');
  x = x + w2;
  
  y1 = y;
  for (i=0; i<6; i++) {
    obj = add_normal_button(x,    y, w3, h1, "ON", 'N');
      fl_set_object_callback(obj, control_dcdc_callback, 2*i+1);
    obj = add_normal_button(x+w3, y, w4, h1, "OFF", 'N');
      fl_set_object_callback(obj, control_dcdc_callback, 2*i+0);
    y = y + h1;
  }
  x = x + w3 + w4;
  
  y = y1;
  create_btn_vstring(
    6, x, y, w5, h1, o_o, 'M', &w_dcdc, dcdc, &DCDC_BTN[0]);
  obj = add_normal_button(x, y+6*h1, w5, h1, "SET", 'N');
    fl_set_object_callback(obj, set_dcdc_callback, 0);
  x = x + w5;

  create_dat_box_vstring(
    6, x, y, w6, h1, 'M', &r_dig_sta[2], ccs_cv_sta, &DCDC_BOX[0]);
  obj = add_normal_button(x, y+6*h1, w6/2, h1, "R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 2);
  x = x + w6 / 2;
  obj = add_normal_button(x, y+6*h1, w6/2, h1, "R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 3);
  x = x + w6 / 2;
  x1 = x + 5;
}

{//~--- CCS Analog Telemetry ---

  int w1 = 120, w2 = 100, w3 = 85, w4 = 135, w5 = 170, w6 = 50, h1 = 20;
  int n[4] = {3,3,6,2};
  int k;
  char *txt[] = {
    "CPU Time",
    "ITARGET1_TM N",
    "ITARGET2_TM N",
    "ITARGET3_TM N",
    "CCS_CURRENT_TM N",
    "CCS_VOLTAGE_TM N",
    "E_DUMP_VOLTAGE_TM N",
    "CURRENT_TM1",
    "CURRENT_TM2",
    "CURRENT_TM3",
    "CURRENT_TM4",
    "CURRENT_TM5",
    "CURRENT_TM6",
    "Voffset_N",
    "Vgain_N"};

  x = x1;
  y = y1;
  add_frame_box(x, y, w1, 2*h1, "Analog Telemetry", 'N');
  x = x + w1;
  obj = add_normal_button(x, y, w2, 2*h1, "ACQUIRE", 'N');
    fl_set_object_callback(obj, acquire_CCS_TM_callback, 0);
  y = y + 2 * h1;

  x = x1;
  obj = add_frame_box(x, y, w3, h1, "CPU Time", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w3;
  create_dat_box_hstring(
    1, x, y, w4, h1, 'M', &r_ccs_tm, &ccs_tm[0], &CPU_TIME);
  y = y + h1 + 5;
  
  x = x1;
  for (i=0,k=1; i<4; i++) {
    create_frame_box_vstring(n[i], x, y, w5, h1, &txt[k], 'N', 'L');
    create_dat_box_vstring(
      n[i], x+w5, y, w6, h1, 'M', &r_ccs_tm, &ccs_tm[k], &CCS_TM[k-1]);
    k = k + n[i];
    y = y + n[i] * h1 + 5;
  }

  x = x1;
  add_frame_box(x, y, w5, 2*h1, "Analog Telemetry", 'N');
  x = x + w5;
  obj = add_normal_button(x, y, w6, 2*h1, "READ", 'N');
    fl_set_object_callback(obj, read_CCS_TM_callback, 0);
}

//~---

}

//~----------------------------------------------------------------------------

void control_inter_dcdc_callback(FL_OBJECT* obj, long data) {

  int cha  = data / 2;
  int ctrl = data % 2;
  _r *c   = &r_cur;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *d_X = &r_dig_sta[2];
  _r *t   = &r_ccs_tm;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (c->sta   != status_KO) c->sta   = status_NA;
  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (d_X->sta != status_KO) d_X->sta = status_NA;
  if (t->sta   != status_KO) t->sta   = status_NA;

  PS_interpoint_driver_on_off(cha, ctrl, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void set_current_setpoint_callback(FL_OBJECT* obj, long data) {

  int cha = data;
  _r *c   = &r_cur;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *d_X = &r_dig_sta[2];
  _r *t   = &r_ccs_tm;
  _ww *w  = &w_cur[cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (c->sta   != status_KO) c->sta   = status_NA;
  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (d_X->sta != status_KO) d_X->sta = status_NA;
  if (t->sta   != status_KO) t->sta   = status_NA;

  w->wri[0] = w->inp[0];
  
  CCS_current_setpoint_seq((int8)(cha+1), (int16)w->inp[0], &err);

  if (!err) {
    w->wri[0] = w->inp[0];
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

void get_current_setpoint_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_cur;
  int i;
  int16 dat[3];
 
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<3; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  CCS_read_setpoint_CMDS(dat, &err);

  if (!err) {
    for (i=0; i<3; i++) r->rea[i] = dat[i];
    if (r->unk) for (i=0; i<3; i++) r->was[i] = r->rea[i];
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

void start_setpoint_sequence_callback(FL_OBJECT* obj, long data) {

  _r *e   = &r_ess_sta;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *d_X = &r_dig_sta[2];
  _r *t   = &r_ccs_tm;
  _ww *w  = &w_vel;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (e->sta   != status_KO) e->sta   = status_NA;
  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (d_X->sta != status_KO) d_X->sta = status_NA;
  if (t->sta   != status_KO) t->sta   = status_NA;

  w->wri[0] = w->inp[0];
  
  CCS_start_current_setpoint_seq((int8)w->inp[0], &err);

  if (!err) {
    w->wri[0] = w->inp[0];
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

void control_CCS_callback(FL_OBJECT* obj, long data) {

  char *cmd = (char*)data;
  _r *e   = &r_ess_sta;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *d_X = &r_dig_sta[2];
  _r *t   = &r_ccs_tm;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (e->sta   != status_KO) e->sta   = status_NA;
  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (d_X->sta != status_KO) d_X->sta = status_NA;
  if (t->sta   != status_KO) t->sta   = status_NA;

  err = 0x0000;

  if (strcasestr(cmd, "CANCEL"))  CCS_cancel_current_setpoint_seq(&err);
  if (strcasestr(cmd, "ARM"))     CCS_commanding_arm_disarm(1, &err);
  if (strcasestr(cmd, "DISARM"))  CCS_commanding_arm_disarm(0, &err);
  if (strcasestr(cmd, "OPEN"))    CCS_open_close_semic_sw(1, &err);
  if (strcasestr(cmd, "CLOSE"))   CCS_open_close_semic_sw(0, &err);
  if (strcasestr(cmd, "ENABLE"))  CCS_ena_dis_ouput(1, &err);
  if (strcasestr(cmd, "DISABLE")) CCS_ena_dis_ouput(0, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void control_dcdc_callback(FL_OBJECT* obj, long data) {

  int cha  = data / 2;
  int ctrl = data % 2;
  int16 wri = 0x0AAA;
  _r *e   = &r_ess_sta;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *d_X = &r_dig_sta[2];
  _r *t   = &r_ccs_tm;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (e->sta   != status_KO) e->sta   = status_NA;
  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (d_X->sta != status_KO) d_X->sta = status_NA;
  if (t->sta   != status_KO) t->sta   = status_NA;

  CCS_on_off_DCDC_converters(pack16(&wri, 3<<2*cha, ctrl), &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void set_dcdc_callback(FL_OBJECT* obj, long data) {

  _r *e   = &r_ess_sta;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *d_X = &r_dig_sta[2];
  _r *t   = &r_ccs_tm;
  _ww *w  = &w_dcdc;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (e->sta   != status_KO) e->sta   = status_NA;
  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (d_X->sta != status_KO) d_X->sta = status_NA;
  if (t->sta   != status_KO) t->sta   = status_NA;

  w->wri[0] = w->inp[0];
  
  CCS_on_off_DCDC_converters((int16)w->inp[0], &err);

  if (!err) {
    w->wri[0] = w->inp[0];
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

void acquire_CCS_TM_callback(FL_OBJECT* obj, long data) {

  _r *t = &r_ccs_tm;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (t->sta != status_KO) t->sta = status_NA;

  CCS_analog_TM_acquire(&err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_CCS_TM_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ccs_tm;
  int i;
  int8 dat[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<100; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  CCS_read_analog_TM(dat, &err);

  if (!err) {
    for (i=0; i<DIM(ccs__tm); i++) {
      _sta *k = &ccs__tm[i];
      r->rea[i] = packbytes(NULL, &dat[k->i], k->n);
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

//~----------------------------------------------------------------------------

void update_CCS_panel(void) {

  char b[80];
  _r *r = &r_ccs_tm;

  show_dat_box_string(2, o_o, NULL, &INT_BOX[0]);

  show_btn_string(    9,         NULL, &CUR_BTN[0][0]);
  show_dat_box_string(3, NULL, "%04X", &CUR_BOX[0]);

  show_btn_string(    1,      NULL, &VEL_BTN);
  show_dat_box_string(1, o_r, NULL, &SEQ_BOX);

  show_dat_box_string(1, d_a, NULL, &ARM_BOX);
  show_dat_box_string(1, c_o, NULL, &SEMI_BOX);
  show_dat_box_string(1, d_e, NULL, &OUTP_BOX);

  show_btn_string(    6,      NULL, &DCDC_BTN[0]);
  show_dat_box_string(6, o_o, NULL, &DCDC_BOX[0]);

  sprintf_huge(b, (int64)(0.0125*r->rea[0]), 8, 0, 1);
  show_dat_box_string( 1, NULL,      b, &CPU_TIME);
  show_dat_box_string(14, NULL, "%03X", &CCS_TM[0]);
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

  update_CCS_panel();
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
