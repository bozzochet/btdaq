// file TTCE.c
//
// Controller for TTCE: Pump etc
//
// A.Lebedev Jun-2007...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"TTCE Controller"};          // global variable
char *date  = {"20-Feb-08"};                // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~----- Pump Control

//~-- 28 VDC Power Control

DAT_BOX *PUMP_28V_BOX[2];
_r r_pump_28v[2];

//~-- Pump Control Voltage Control

SW_BTN  *PUMP_VOLT_COD_BTN[2][2];
SW_BTN  *PUMP_VOLT_VAL_BTN[2][1];
DAT_BOX *PUMP_VOLT_COD_BOX[2][2];
DAT_BOX *PUMP_VOLT_VAL_BOX[2][1];
_ww ww_pump_volt[2];
_r r_pump_volt[2];

//~-- Pump RPM Control

DAT_BOX *PUMP_RPM_COD_BOX[2];
DAT_BOX *PUMP_RPM_VAL_BOX[2];
_r r_pump_rpm[2];

//~-----

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *o_o[2] = {"OFF", "ON"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_pump_control_panel(int x0, int y0);

void convert_pump_voltage_code_to_value(int32 code, int32 *value);
void convert_pump_rpm_code_to_value(int32 code, int32 *value);

void pump_28V_control_callback(FL_OBJECT* obj, long data);
void read_pump_28V_status_callback(FL_OBJECT* obj, long data);

void set_pump_voltage_callback(FL_OBJECT* obj, long data);
void write_pump_voltage_callback(FL_OBJECT* obj, long data);
void read_pump_parameters_callback(FL_OBJECT* obj, long data);

void update_pump_control_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 485, HH = 185;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_pump_control_panel(0, 45);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  for (i=0; i<2; i++) {
    invalidate_r(first,  &r_pump_28v[i],   1);
    invalidate_ww(first, &ww_pump_volt[i], 1, 0);
    invalidate_r(first,  &r_pump_volt[i],  1);
    invalidate_r(first,  &r_pump_rpm[i],   1);
  }

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

  int i;
  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    for (i=0; i<2; i++) fread(ww_pump_volt[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  int i;
  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    for (i=0; i<2; i++) fwrite(ww_pump_volt[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            PUMP CONTROL PANEL
//
//~============================================================================

_b pump_28v  = {0, 0x00000001, 1};
_b pump_volt_cod[2] = {
  {0, 0xF0, 16},
  {0, 0x0F, 16}};
_b pump_volt_val[1] = {
  {0, 0xFF, 256}};
_b pump_rpm_cod[1] = {
  {0, 0x0FFF, -1}};
_b pump_rpm_val[1] = {
  {0, 0xFFFF, -1}};
  
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_pump_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, x1;

{//~--- Panel ---

  int w1 = 485, w2 = 75, h1 = 110, h2 = 40, h3 = 20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "Pump", 'M');
  y = y + h2;
  obj = add_frame_box(x, y, w2, h3, "Primary", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  y = y + h3;
  obj = add_frame_box(x, y, w2, h3, "Secondary", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  y = y + h3;
  add_frame_box(x, y, w2, h3, "", 'N');
  x = x + w2 + 5;
  y = y0 + 5;
}
  
{//~--- 28 VDC ---

  int w;
  int w1 = 35, w2 = 35, w3 = 50, h1 = 40, h2 = 20;

  w = w1 + w2 + w3;
  add_frame_box(x, y, w, h1, "28VDC", 'N');
  x1 = x;
  y  = y + h1;
  for (i=0; i<2; i++) {
    x = x1;
    obj = add_normal_button(x, y, w1, h2, "ON",  'N');
      fl_set_object_callback(obj, pump_28V_control_callback, 2*i+1);
    x = x + w1;
    obj = add_normal_button(x, y, w2, h2, "OFF", 'N');
      fl_set_object_callback(obj, pump_28V_control_callback, 2*i+0);
    x = x + w2;
    create_dat_box_hstring(
      1, x, y, w3, h2, 'M', &r_pump_28v[i], &pump_28v, &PUMP_28V_BOX[i]);
    y = y + h2;
  }
  x = x1;
  add_frame_box(x, y, w1+w2, h2, "", 'N');
  x = x + w1 + w2;
  obj = add_normal_button(x, y, w3, h2, "R", 'N');
    fl_set_object_callback(obj, read_pump_28V_status_callback, 0);
  x = x1 + w + 5;
  y = y0 + 5;
}

{//~--- Control Voltage ---

  int w;
  int w1 = 20, w2 = 45, w3 = 185, h1 = 20;
  
  x1 = x;
  w = 2 * (2 * w1 + w2);
  add_frame_box(x, y, w, h1, "Pump Control Voltage", 'N');
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
      2, x, y, w1, h1,  hex, 'M', &ww_pump_volt[i], pump_volt_cod, &PUMP_VOLT_COD_BTN[i][0]);
    x = x + 2 * w1;
    create_btn_hstring(
      1, x, y, w2, h1, NULL, 'M', &ww_pump_volt[i], pump_volt_val, &PUMP_VOLT_VAL_BTN[i][0]);
    PUMP_VOLT_VAL_BTN[i][0]->cnv = convert_pump_voltage_code_to_value;
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,      y, w2/2, h1, "16");
      fl_set_object_callback(obj, set_pump_voltage_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w2/2, y, w2/2, h1, "1");
      fl_set_object_callback(obj, set_pump_voltage_callback, i);
    x = x + w2;
    create_dat_box_hstring(
      2, x, y, w1, h1,       'M', &r_pump_volt[i],  pump_volt_cod, &PUMP_VOLT_COD_BOX[i][0]);
    x = x + 2 * w1;
    create_dat_box_hstring(
      1, x, y, w2, h1,       'M', &r_pump_volt[i],  pump_volt_val, &PUMP_VOLT_VAL_BOX[i][0]);
    PUMP_VOLT_VAL_BOX[i][0]->cnv = convert_pump_voltage_code_to_value;
    y = y + h1;
  }
  x = x1;
  obj = add_normal_button(x, y, 2*w1+w2, h1, "WRITE", 'N');
      fl_set_object_callback(obj, write_pump_voltage_callback, 0);
  x = x + 2 * w1 + w2;
  obj = add_normal_button(x, y, w3, h1, "READ", 'N');
    fl_set_object_callback(obj, read_pump_parameters_callback, 0);
  x = x + 2 * w1 + w2 + 5;
  y = y0 + 5;
}

{//~--- RPM ---

  int w;
  int w1 = 45, w2 = 50, h1 = 20;
  
  x1 = x;
  w = w1 + w2;
  add_frame_box(x, y, w,  h1, "RPM",  'N');
  y = y + h1;
  add_frame_box(x, y, w1, h1, "Code", 'S');
  x = x + w1;
  add_frame_box(x, y, w2, h1, "RPM",  'S');

  x =  x1;
  y = y + h1;
  for (i=0; i<2; i++) {
    x = x1;
    create_dat_box_hstring(
      1, x, y, w1, h1,       'M', &r_pump_rpm[i], pump_rpm_cod, &PUMP_RPM_COD_BOX[i]);
    x = x + w1;
    create_dat_box_hstring(
      1, x, y, w2, h1,       'M', &r_pump_rpm[i], pump_rpm_val, &PUMP_RPM_VAL_BOX[i]);
    PUMP_RPM_VAL_BOX[i]->cnv = convert_pump_rpm_code_to_value;
    y = y + h1;
  }
}

//~---
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

void pump_28V_control_callback(FL_OBJECT* obj, long data) {

  int ind  = data / 2;
  bool o_o = data % 2;
  int8 wri[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  _r *r = &r_pump_28v[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  wri[6] = o_o << (ind ? 7 : 6);
  wri[7] = 0x1 << (ind ? 7 : 6);
  setup_TTCE_28VDC_power(wri, &err);

//if (!err) read_pump_28V_status_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_pump_28V_status_callback(FL_OBJECT* obj, long data) {

  int i;
  int8 rea[5];

  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<2; i++) {
    _r *r = &r_pump_28v[i];
    r->sta  = status_NA;
    *r->was = *r->rea;
  }

  check_TTCE_28VDC_power(rea, &err);

  for (i=0; i<2; i++) {
    _r *r = &r_pump_28v[i];
    if (!err) {
      r->rea[0] = rea[4] >> (i ? 7 : 6);
      if (r->unk) *r->was = *r->rea;
      r->unk = FALSE;
      r->sta = status_OK;
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

void set_pump_voltage_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _ww *w = &ww_pump_volt[ind];
  int  d =  atoi(fl_get_object_label(obj));

  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  
  pack32(&w->inp[0], 0xFF, MOD(w->inp[0]+d, 256));
  w->sta = status_NA;

  update_objects();
}

//~----------------------------------------------------------------------------

void write_pump_voltage_callback(FL_OBJECT* obj, long data) {

  int i;
  int8 wri[2];

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<2; i++) {
    _r *r = &r_pump_volt[i];
    r->sta  = status_NA;
  }

  for (i=0; i<2; i++) {
    _ww *w = &ww_pump_volt[i];
    wri[i] = w->inp[0];
  }

  write_TTCE_pump_DAC(wri, &err);

  for (i=0; i<2; i++) {
    _ww *w = &ww_pump_volt[i];
    if (!err) {
      w->wri[0] = w->inp[0];
      w->unk = FALSE;
    }
    else {
      w->unk = TRUE;
    }
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
  _r *r;
    
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<2; i++) {
    r = &r_pump_volt[i];
    *r->was = *r->rea;
    r = &r_pump_rpm[i];
    *r->was = *r->rea;
  }
  
  read_TTCE_pump_data(DAC, RPM, &err);

  for (i=0; i<2; i++) {
    if (!err) {
      r = &r_pump_volt[i];
      r->rea[0] = DAC[i];
      if (r->unk) *r->was = *r->rea;
      r->unk  = FALSE;
      r->sta  = status_OK;
      r = &r_pump_rpm[i];
      r->rea[0] = RPM[i];
      if (r->unk) *r->was = *r->rea;
      r->unk  = FALSE;
      r->sta  = status_OK;
    }
    else {
      r = &r_pump_volt[i];
      r->sta = status_KO;
      r = &r_pump_rpm[i];
      r->sta = status_KO;
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_pump_control_panel(void) {

  int i;
  
  for (i=0; i<2; i++) {
    show_dat_box_string(1,  o_o, NULL,   &PUMP_28V_BOX[i]);
    show_btn_string(    2,       NULL,   &PUMP_VOLT_COD_BTN[i][0]);
    show_btn_string(    1,       "%4d",  &PUMP_VOLT_VAL_BTN[i][0]);
    show_dat_box_string(2,  hex, NULL,   &PUMP_VOLT_COD_BOX[i][0]);
    show_dat_box_string(1, NULL, "%4d",  &PUMP_VOLT_VAL_BOX[i][0]);
    show_dat_box_string(1, NULL, "%04X", &PUMP_RPM_COD_BOX[i]);
    show_dat_box_string(1, NULL, "%4d",  &PUMP_RPM_VAL_BOX[i]);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_pump_control_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
