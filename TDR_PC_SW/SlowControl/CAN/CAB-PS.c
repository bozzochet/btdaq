// file CAB-PS.c
//
//  CAB PS Controller
//
//  A.Lebedev Jan-2009...
//

#include "cabdef.h"
#include "crisalib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CAB - PS Controller"};      // global variable
char *date  = {"21-Jan-09"};                // global variable

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

//~-- 

DAT_BOX *ON_OFF[100], *ENA_DIS[100], *OPN_CLS[100], *CLS_NCL[100];
int    n_ON_OFF = 0, n_ENA_DIS = 0, n_OPN_CLS = 0, n_CLS_NCL = 0;
SW_BTN  *DUR[12+22];
int    n_DUR = 0;
_ww w_dur[DIM(DUR)];
_r r_ess_sta;
_r r_dig_sta[2];

//~-- CAB Acknoledgement Code

FL_OBJECT *CAB_ACK[2];
_r r_cab_ack;

//~-----

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
char *o_o[3] = {"OFF",        "ON",     "?"};
char *d_e[3] = {"DIS",        "ENA",    "?"};
char *c_o[3] = {"CLOSED",     "OPEN",   "?"};
char *c_n[3] = {"NOT CLOSED", "CLOSED", "?"};

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
void get_essential_and_digital_status_callback(FL_OBJECT* obj, long data);

void create_model_selector_panel(int x0, int y0);

void create_CPU_control_panel(int x0, int y0);
void reset_SW_callback(FL_OBJECT* obj, long data);
void load_PGM_callback(FL_OBJECT* obj, long data);
void go_APSW_callback(FL_OBJECT* obj, long data);

void create_PS_CV_panel(int x0, int y0);
void control_PS_CV_callback(FL_OBJECT* obj, long data);

void create_inter_dcdc_panel(int x0, int y0);
void control_inter_dcdc_callback(FL_OBJECT* obj, long data);

void create_cold_valve_panel(int x0, int y0);
void ena_dis_cold_valve_callback(FL_OBJECT* obj, long data);
void operate_cold_valve_callback(FL_OBJECT* obj, long data);

void create_moog_valve_panel(int x0, int y0);
void ena_dis_moog_valve_callback(FL_OBJECT* obj, long data);
void operate_moog_valve_callback(FL_OBJECT* obj, long data);

void create_warm_valve_panel(int x0, int y0);
void ena_dis_warm_valve_callback(FL_OBJECT* obj, long data);
void operate_warm_valve_callback(FL_OBJECT* obj, long data);

void update_all_panels(void);

void create_CAB_acknoledgment_panel(FL_FORM *form);
void update_CAB_acknoledgment_panel(int16 *err);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 905, HH = 685;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_model_selector_panel(150, 0);

  create_CPU_control_panel(0, 45);

  create_PS_CV_panel(     115, 45);
  create_inter_dcdc_panel(300, 45);

  create_cold_valve_panel(20, 205);
  create_moog_valve_panel( 0, 375);

  create_warm_valve_panel(450, 45);

  create_CAB_acknoledgment_panel(MAIN);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  invalidate_ww(first, &w_model, 1, 0);

  invalidate_r(first, &r_ess_sta, 100);
  for (i=0; i<DIM(r_dig_sta); i++) {
    invalidate_r(first, &r_dig_sta[i], 100);
  }
  for (i=0; i<DIM(w_dur); i++) {
    invalidate_ww(first, &w_dur[i], 1, 2);
  }

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
    for (i=0; i<DIM(w_dur); i++) {
      fread(w_dur[i].inp, 4, 1, file);
    }
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
    for (i=0; i<DIM(w_dur); i++) {
      fwrite(w_dur[i].inp, 4, 1, file);
    }
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

  int cha = data % 2;
  int i;
  _r *r = &r_dig_sta[cha];
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

//~-------------------------------------------------------------------------------

void get_essential_and_digital_status_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  if (!err) get_essential_status_callback(NULL, 0);
  if (!err) get_digital_status_callback(NULL, cha);

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
//            PS DC/DC CONVERTER PANEL
//
//~============================================================================

void create_PS_CV_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 20, w2 = 30, w3 = 35, w4 = 35, w5 = 35, w6 = 20, h1 = 20, h2 = 20;

  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "PS DC/DC CV", 'N');
  y = y + h1;
  
  add_frame_box(x, y,    w1, h2, "N", 'N');
  add_frame_box(x, y+h2, w1, h2, "R", 'N');
  x = x + w1;
  
  obj = add_normal_button(x, y,    w2, h2, "ON", 'N');
    fl_set_object_callback(obj, control_PS_CV_callback, 2*0+1);
  obj = add_normal_button(x, y+h2, w2, h2, "ON", 'N');
    fl_set_object_callback(obj, control_PS_CV_callback, 2*1+1);
  x = x + w2;
  
  obj = add_normal_button(x, y,    w3, h2, "OFF", 'N');
    fl_set_object_callback(obj, control_PS_CV_callback, 2*0+0);
  obj = add_normal_button(x, y+h2, w3, h2, "OFF", 'N');
    fl_set_object_callback(obj, control_PS_CV_callback, 2*1+0);
  x = x + w3;

  create_dat_box_vstring(
    2, x, y,    w4, h2, 'M', &r_ess_sta, ps_cv, &ON_OFF[n_ON_OFF]);
  for (i=0; i<2; i++) ON_OFF[n_ON_OFF++]->cnv = convert_0x50_0x5F;
  x = x + w4;

  create_dat_box_vstring(
    1, x, y,    w5, h2, 'M', &r_dig_sta[0], &ps_d_sta[7], &ON_OFF[n_ON_OFF++]);
  create_dat_box_vstring(
    1, x, y+h2, w5, h2, 'M', &r_dig_sta[1], &ps_d_sta[7], &ON_OFF[n_ON_OFF++]);
  x = x + w5;

  obj = add_normal_button(x, y,    w6, h2, "R", 'N');
    fl_set_object_callback(obj, get_essential_and_digital_status_callback, 0);
  obj = add_normal_button(x, y+h2, w6, h2, "R", 'N');
    fl_set_object_callback(obj, get_essential_and_digital_status_callback, 1);
  x = x + w6;
}

//~----------------------------------------------------------------------------

void control_PS_CV_callback(FL_OBJECT* obj, long data) {

  int cha  = data / 2;
  int ctrl = data % 2;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  PS_on_off_CV(cha, ctrl, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            INTERPOINT DC/DC CONVERTER PANEL
//
//~============================================================================

void create_inter_dcdc_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 30, w3 = 35, w4 = 35, w5 = 20, h1 = 20, h2 = 20;

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Interpoint DC/DC CV", 'N');
  y = y + h1;
  
  add_frame_box(x, y,    w1, h2, "N", 'N');
  add_frame_box(x, y+h2, w1, h2, "R", 'N');
  x = x + w1;
  
  obj = add_normal_button(x, y,    w2, h2, "ON", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*0+1);
  obj = add_normal_button(x, y+h2, w2, h2, "ON", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*1+1);
  x = x + w2;
  
  obj = add_normal_button(x, y,    w3, h2, "OFF", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*0+0);
  obj = add_normal_button(x, y+h2, w3, h2, "OFF", 'N');
    fl_set_object_callback(obj, control_inter_dcdc_callback, 2*1+0);
  x = x + w3;

  create_dat_box_vstring(
    1, x, y,    w4, h2, 'M', &r_dig_sta[0], &ps_cmd_sw_sta[0], &ON_OFF[n_ON_OFF++]);
  create_dat_box_vstring(
    1, x, y+h2, w4, h2, 'M', &r_dig_sta[1], &ps_cmd_sw_sta[0], &ON_OFF[n_ON_OFF++]);
  x = x + w4;

  obj = add_normal_button(x, y,    w5, h2, "R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 0);
  obj = add_normal_button(x, y+h2, w5, h2, "R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 1);
  x = x + w5;
}

//~----------------------------------------------------------------------------

void control_inter_dcdc_callback(FL_OBJECT* obj, long data) {

  int cha  = data / 2;
  int ctrl = data % 2;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  PS_interpoint_driver_on_off(cha, ctrl, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            COLD VALVE PANEL
//
//~============================================================================

void create_cold_valve_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 40, w2 = 40, w3 = 20, w4 = 120, w5 = 100, w6 = 60, w7 = 20;
  int w41 = 35, w42 = 30, w43 = 35, w44 = 20;
  int w51 = 50, w52 = 50;
  int h1 = 30, h2 = 35, h3 = 20;

//~---

  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + w7 + 5;
  h = 5 + h1 + h2 + 2 * h3 + 5 + 2 * h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Cold Valve Control", 'L');
  y = y + h1;

//~---

  add_frame_box(x, y, w1, h2, "Cold\nValve",    'N');
  x = x + w1;
  add_frame_box(x, y, w2, h2, "Pilot\nValve",   'N');
  x = x + w2;
  add_frame_box(x, y, w3, h2, "",               'N');
  x = x + w3;
  add_frame_box(x, y, w4, h2, "Enable/Disable", 'N');
  x = x + w4;
  add_frame_box(x, y, w5, h2, "Operate",        'N');
  x = x + w5;
  add_frame_box(x, y, w6, h2, "Pilot\nValve",   'N');
  x = x + w6;
  add_frame_box(x, y, w7, h2, "",               'N');
  y = y + h2;

//~---

  for (i=0; i<2; i++) {
    char* txt1[] = {"DV05", "DV07"};
    char* txt2[] = {"DV55", "DV57"};

    x = x0 + 5;

    add_frame_box(x, y, w1, 2*h3, txt1[i], 'N');
    x = x + w1;
    add_frame_box(x, y, w2, 2*h3, txt2[i], 'N');
    x = x + w2;
    add_frame_box(x, y,    w3, h3, "N", 'N');
    add_frame_box(x, y+h3, w3, h3, "R", 'N');
    x = x + w3;
    
    obj = add_normal_button(x, y,    w41, h3, "ENA", 'N');
      fl_set_object_callback(obj, ena_dis_cold_valve_callback, 2*0+1);
    obj = add_normal_button(x, y+h3, w41, h3, "ENA", 'N');
      fl_set_object_callback(obj, ena_dis_cold_valve_callback, 2*1+1);
    x = x + w41;
  
    obj = add_normal_button(x, y,    w42, h3, "DIS", 'N');
      fl_set_object_callback(obj, ena_dis_cold_valve_callback, 2*0+0);
    obj = add_normal_button(x, y+h3, w42, h3, "DIS", 'N');
      fl_set_object_callback(obj, ena_dis_cold_valve_callback, 2*1+0);
    x = x + w42;
  
    create_dat_box_vstring(1, x, y,    w43, h3, 'M', 
      &r_dig_sta[0], &ps_d_sta[5], &ENA_DIS[n_ENA_DIS++]);
    create_dat_box_vstring(1, x, y+h3, w43, h3, 'M', 
      &r_dig_sta[1], &ps_d_sta[5], &ENA_DIS[n_ENA_DIS++]);
    x = x + w43;

    obj = add_normal_button(x, y,    w44, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 0);
    obj = add_normal_button(x, y+h3, w44, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 1);
    x = x + w44;

    obj = add_normal_button(x, y,    w51, h3, "OPEN", 'N');
      fl_set_object_callback(obj, operate_cold_valve_callback, 4*i+2*0+1);
    obj = add_normal_button(x, y+h3, w51, h3, "OPEN", 'N');
      fl_set_object_callback(obj, operate_cold_valve_callback, 4*i+2*1+1);
    x = x + w51;

    obj = add_normal_button(x, y,    w52, h3, "CLOSE", 'N');
      fl_set_object_callback(obj, operate_cold_valve_callback, 4*i+2*0+0);
    obj = add_normal_button(x, y+h3, w52, h3, "CLOSE", 'N');
      fl_set_object_callback(obj, operate_cold_valve_callback, 4*i+2*1+0);
    x = x + w52;

    create_dat_box_vstring(1, x, y,    w6, h3, 'M', 
      &r_dig_sta[0], &ps_cmd_sw_sta[5+i], &OPN_CLS[n_OPN_CLS++]);
    create_dat_box_vstring(1, x, y+h3, w6, h3, 'M', 
      &r_dig_sta[1], &ps_cmd_sw_sta[5+i], &OPN_CLS[n_OPN_CLS++]);
    x = x + w6;
  
    obj = add_normal_button(x, y,    w7, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 0);
    obj = add_normal_button(x, y+h3, w7, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 1);
    y = y + 2 * h3 + 5;
  }
}

//~----------------------------------------------------------------------------

void ena_dis_cold_valve_callback(FL_OBJECT* obj, long data) {

  int cha  = data / 2;
  int ctrl = data % 2;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  PS_ena_dis_bilevel_pilot_valves(cha, ctrl, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void operate_cold_valve_callback(FL_OBJECT* obj, long data) {

  int valve = data / 2 / 2;
  int cha   = data / 2 % 2;
  int ctrl  = data % 2;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  PS_open_close_bilevel_pilot_valve(cha, valve, ctrl, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            MOOG VALVE PANEL
//
//~============================================================================

void create_moog_valve_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 45, w2 = 20, w3 = 120, w4 = 170, w5 = 65, w6 = 20;
  int w31 = 35, w32 = 30, w33 = 35, w34 = 20;
  int w41 = 35, w42 = 50, w43 = 35, w44 = 50;
  int h1 = 30, h2 = 35, h3 = 20;
  _b dur[] = {{0, 0x01FF, 126}};

//~---

  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + 5;
  h = 5 + h1 + h2 + 4 * (2 * h3 + 5) - 5 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Moog Valve Control", 'L');
  y = y + h1;

//~---

  add_frame_box(x, y, w1, h2, "Moog\nValve",    'N');
  x = x + w1;
  add_frame_box(x, y, w2, h2, "",               'N');
  x = x + w2;
  add_frame_box(x, y, w3, h2, "Enable/Disable", 'N');
  x = x + w3;
  add_frame_box(x, y, w4, h2, "Operate",        'N');
  x = x + w4;
  add_frame_box(x, y, w5, h2, "Moog\nValve",   'N');
  x = x + w5;
  add_frame_box(x, y, w6, h2, "",               'N');
  y = y + h2;

//~---

  for (i=0; i<4; i++) {
    char* txt[] = {"DV20A", "DV20B", "DV20C", "DV20D"};

    x = x0 + 5;

    add_frame_box(x, y, w1, 2*h3, txt[i], 'N');
    x = x + w1;
    add_frame_box(x, y,    w2, h3, "N", 'N');
    add_frame_box(x, y+h3, w2, h3, "R", 'N');
    x = x + w2;

    obj = add_normal_button(x, y,    w31, h3, "ENA", 'N');
      fl_set_object_callback(obj, ena_dis_moog_valve_callback, 4*i+2*0+1);
    obj = add_normal_button(x, y+h3, w31, h3, "ENA", 'N');
      fl_set_object_callback(obj, ena_dis_moog_valve_callback, 4*i+2*1+1);
    x = x + w31;
  
    obj = add_normal_button(x, y,    w32, h3, "DIS", 'N');
      fl_set_object_callback(obj, ena_dis_moog_valve_callback, 4*i+2*0+0);
    obj = add_normal_button(x, y+h3, w32, h3, "DIS", 'N');
      fl_set_object_callback(obj, ena_dis_moog_valve_callback, 4*i+2*1+0);
    x = x + w32;
  
    create_dat_box_vstring(1, x, y,    w33, h3, 'M', 
      &r_dig_sta[0], &ps_a_sta[i], &ENA_DIS[n_ENA_DIS++]);
    create_dat_box_vstring(1, x, y+h3, w33, h3, 'M', 
      &r_dig_sta[1], &ps_a_sta[i], &ENA_DIS[n_ENA_DIS++]);
    x = x + w33;

    obj = add_normal_button(x, y,    w34, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 0);
    obj = add_normal_button(x, y+h3, w34, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 1);
    x = x + w34;

    if (i % 2) {
      add_frame_box(x, y, w41+w42, h3, "", 'N');
      create_btn_hstring(
        1, x, y+h3, w41, h3, NULL, 'M', &w_dur[n_DUR], dur, &DUR[n_DUR]);
      obj = add_normal_button(x+w41, y+h3, w42, h3, "OPEN", 'N');
        fl_set_object_callback(obj, operate_moog_valve_callback, 256*n_DUR+4*i+2*1+1);
    }
    else {
      create_btn_hstring(
        1, x, y   , w41, h3, NULL, 'M', &w_dur[n_DUR], dur, &DUR[n_DUR]);
      obj = add_normal_button(x+w41, y, w42, h3, "OPEN", 'N');
        fl_set_object_callback(obj, operate_moog_valve_callback, 256*n_DUR+4*i+2*0+1);
      add_frame_box(x, y+h3, w41+w42, h3, "", 'N');
    }
    n_DUR = n_DUR + 1;
    x = x + w41 + w42;

    create_btn_hstring(
      1, x, y   , w43, h3, NULL, 'M', &w_dur[n_DUR], dur, &DUR[n_DUR]);
    obj = add_normal_button(x+w43, y,    w44, h3, "CLOSE", 'N');
      fl_set_object_callback(obj, operate_moog_valve_callback, 256*n_DUR+4*i+2*0+0);
    n_DUR = n_DUR + 1;
    create_btn_hstring(
      1, x, y+h3, w43, h3, NULL, 'M', &w_dur[n_DUR], dur, &DUR[n_DUR]);
    obj = add_normal_button(x+w43, y+h3, w44, h3, "CLOSE", 'N');
      fl_set_object_callback(obj, operate_moog_valve_callback, 256*n_DUR+4*i+2*1+0);
    n_DUR = n_DUR + 1;
    x = x + w43 + w44;

    create_dat_box_vstring(1, x, y,    w5, h3, 'M', 
      &r_dig_sta[0], &ps_a_sta[4+i], &OPN_CLS[n_OPN_CLS++]);
    create_dat_box_vstring(1, x, y+h3, w5, h3, 'M', 
      &r_dig_sta[1], &ps_a_sta[4+i], &OPN_CLS[n_OPN_CLS++]);
    x = x + w5;
  
    obj = add_normal_button(x, y,    w6, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 0);
    obj = add_normal_button(x, y+h3, w6, h3, "R", 'N');
      fl_set_object_callback(obj, get_digital_status_callback, 1);

    y = y + 2 * h3 + 5;
  }
}

//~----------------------------------------------------------------------------

void ena_dis_moog_valve_callback(FL_OBJECT* obj, long data) {

  int grp  = data / 2 / 2;
  int cha  = data / 2 % 2;
  int ctrl = data % 2;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  PS_ena_dis_moog_latching_valves(cha, ctrl, grp, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void operate_moog_valve_callback(FL_OBJECT* obj, long data) {

  int n_dur = data / 256;
  int valve = data % 256 / 2 / 2;
  int cha   = data % 256 / 2 % 2;
  int ctrl  = data % 2;
  _ww *w  = &w_dur[n_dur];
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  w->wri[0] = w->inp[0];
  
  PS_operate_moog_latching_valve(cha, valve, ctrl, (int8)w->inp[0], &err);

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

//~============================================================================
//
//            WARM VALVE PANEL
//
//~============================================================================

void create_warm_valve_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int warm, pilot, chain;
  int x, y, w, h;
  int w1 = 45, w2 = 55, w3 = 20, w4 = 120, w5 = 85, w6 = 100, w7 = 20;
  int w41 = 35, w42 = 30, w43 = 35, w44 = 20;
  int w51 = 35, w52 = 50;
  int h1 = 30, h2 = 35, h3 = 20;
  _b dur[] = {{0, 0x01FF, 126}};
  char* txt1[] = {"DV09A", "DV09B", "DV11A", "DV11B", "DV16A", "DV16B"};
  char* txt2[][2] = {
    {"DV59AO", "DV59AS"}, {"DV59BO", "DV59BS"},
    {"DV61AO", "DV61AS"}, {"DV61BO", "DV61BS"},
    {"DV66AO", "DV66AS"}, {"DV66BO", "DV66BS"}};
  char* txt3[] = {"N", "R"};
  char* txt4[] = {"OPEN", "CLOSE"};

//~---

  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + w7 + 5;
  h = 5 + h1 + h2 + 6 * (4 * h3 + 5) - 5 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Warm Valve Control", 'L');
  y = y + h1;

//~---

  add_frame_box(x, y, w1, h2, "Warm\nValve",    'N');
  x = x + w1;
  add_frame_box(x, y, w2, h2, "Pilot\nValve",   'N');
  x = x + w2;
  add_frame_box(x, y, w3, h2, "",               'N');
  x = x + w3;
  add_frame_box(x, y, w4, h2, "Enable/Disable", 'N');
  x = x + w4;
  add_frame_box(x, y, w5, h2, "Operate",        'N');
  x = x + w5;
  add_frame_box(x, y, w6, h2, "Warm\nValve",    'N');
  x = x + w6;
  add_frame_box(x, y, w7, h2, "",               'N');
  y = y + h2;

//~---

  for (warm=0; warm<6; warm++) {
    int x1;
    x = x0 + 5;
    add_frame_box(x, y, w1, 4*h3, txt1[warm], 'N');
    x1 = x + w1;
    for (pilot=0; pilot<2; pilot++) {
      int x2;
      x = x1;
      add_frame_box(x, y, w2, 2*h3, txt2[warm][pilot], 'N');
      x2 = x + w2;
      for (chain=0; chain<2; chain++) {
        x = x2;
        add_frame_box(x, y, w3, h3, txt3[chain], 'N');
        x = x + w3;
        if ((warm == 2 && pilot == 0 && chain == 1) ||
            (warm == 3 && pilot == 0 && chain == 0)) {
          add_frame_box(x, y, w4+w5+w6+w7, h3, "", 'N');
          y = y + h3;
          continue;
        }
        obj = add_normal_button(x, y, w41, h3, "ENA", 'N');
          fl_set_object_callback(
            obj, ena_dis_warm_valve_callback, 4*(1-pilot)+2*chain+1);
        x = x + w41;
        obj = add_normal_button(x, y, w42, h3, "DIS", 'N');
          fl_set_object_callback(
            obj, ena_dis_warm_valve_callback, 4*(1-pilot)+2*chain+0);
        x = x + w42;
        create_dat_box_vstring(1, x, y, w43, h3, 'M', 
          &r_dig_sta[chain], &ps_c_sta[pilot], &ENA_DIS[n_ENA_DIS++]);
        x = x + w43;
        obj = add_normal_button(x, y, w44, h3, "R", 'N');
          fl_set_object_callback(obj, get_digital_status_callback, chain);
        x = x + w44;
        create_btn_hstring(
          1, x, y, w51, h3, NULL, 'M', &w_dur[n_DUR], dur, &DUR[n_DUR]);
        x = x + w51;
        obj = add_normal_button(x, y, w52, h3, txt4[pilot], 'N');
          fl_set_object_callback(
            obj, operate_warm_valve_callback, 256*n_DUR+4*(2*warm+pilot)+2*chain);
        n_DUR = n_DUR + 1;
        x = x + w52;
        create_dat_box_vstring(1, x, y, w6, h3, 'M', 
          &r_dig_sta[chain], &ps_c_sta[warm+2], &CLS_NCL[n_CLS_NCL++]);
        x = x + w6;
        obj = add_normal_button(x, y, w7, h3, "R", 'N');
          fl_set_object_callback(obj, get_digital_status_callback, chain);
        y = y + h3;
      }
    }
    y = y + 5;
  }
}

//~----------------------------------------------------------------------------

void ena_dis_warm_valve_callback(FL_OBJECT* obj, long data) {

  int grp  = data / 2 / 2;
  int cha  = data / 2 % 2;
  int ctrl = data % 2;
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  PS_ena_dis_pulsed_pilot_valves(cha, ctrl, grp, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void operate_warm_valve_callback(FL_OBJECT* obj, long data) {

  int n_dur = data / 256;
  int valve = data % 256 / 4;
  int cha   = data % 256 % 4 / 2;
  _ww *w  = &w_dur[n_dur];
  _r *d_N = &r_dig_sta[0];
  _r *d_R = &r_dig_sta[1];
  _r *e_X = &r_ess_sta;


printf("n_dur = %d, dur = %d, valve = %d, cha = %d\n", n_dur, w->inp[0], valve, cha);


  if (obj) {
    msg = -1;
    update_objects();
  }

  if (d_N->sta != status_KO) d_N->sta = status_NA;
  if (d_R->sta != status_KO) d_R->sta = status_NA;
  if (e_X->sta != status_KO) e_X->sta = status_NA;

  w->wri[0] = w->inp[0];
  
  PS_operate_pulsed_pilot_valves_seq(cha, valve, (int8)w->inp[0], &err);

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

//~============================================================================
//
//            UPDATE "ALL" PANELS
//
//~============================================================================

void update_all_panels(void) {

  show_btn_string(n_DUR, "%d", &DUR[0]);

  show_dat_box_string(n_ON_OFF,  o_o, NULL, &ON_OFF[0]);
  show_dat_box_string(n_ENA_DIS, d_e, NULL, &ENA_DIS[0]);
  show_dat_box_string(n_OPN_CLS, c_o, NULL, &OPN_CLS[0]);
  show_dat_box_string(n_CLS_NCL, c_n, NULL, &CLS_NCL[0]);
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

  update_all_panels();

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
