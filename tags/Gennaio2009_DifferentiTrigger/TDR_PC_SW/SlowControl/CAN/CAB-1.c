// file CAB-1.c
//
//  CAB-1 Controller
//  Show various "statuses"
//
//  A.Lebedev Nov-2008...
//

#include "cabdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CAB - 1 Controller"};       // global variable
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

//~-- Error Table

FL_OBJECT *ERR_TAB_BRO;
_cab_err cab_err;
_r r_err_tab;
bool err_tab_unk = {TRUE};
int  err_tab_updated = -1;

//~-- CAB Essential Status

FL_OBJECT *ESS_STA_BRO;
_r r_ess_sta;
bool ess_sta_unk = {TRUE};
int  ess_sta_updated = -1;

//~-- CAB Digital Status

FL_OBJECT *DIG_STA_BRO;
_r r_dig_sta;
bool dig_sta_unk = {TRUE};
int  dig_sta_updated = -1;

//~-- CAB Acknoledgement Code

FL_OBJECT *CAB_ACK[2];
_r r_cab_ack;

//~-----

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_model_selector_panel(int x0, int y0);

void create_CPU_control_panel(int x0, int y0);
void reset_SW_callback(FL_OBJECT* obj, long data);
void load_PGM_callback(FL_OBJECT* obj, long data);
void go_APSW_callback(FL_OBJECT* obj, long data);

void create_error_table_panel(int x0, int y0);
void get_error_table_callback(FL_OBJECT* obj, long data);
void update_error_table_panel(void);

void create_essential_status_panel(int x0, int y0);
void get_essential_status_callback(FL_OBJECT* obj, long data);
void update_essential_status_panel(void);

void create_digital_status_panel(int x0, int y0);
void get_digital_status_callback(FL_OBJECT* obj, long data);
void update_digital_status_panel(void);

void create_CAB_acknoledgment_panel(FL_FORM *form);
void update_CAB_acknoledgment_panel(int16 *err);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 1050, HH = 970;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_model_selector_panel(150, 0);

  create_CPU_control_panel(0, 45);

  create_error_table_panel(       0, 430);
  create_essential_status_panel(360,  45);
  create_digital_status_panel(  740,  45);

  create_CAB_acknoledgment_panel(MAIN);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  static bool first = TRUE;

  invalidate_ww(first, &w_model, 1, 0);

  invalidate_r(first, &r_ess_sta, 100);
  invalidate_r(first, &r_err_tab, 1+3*30);
  invalidate_r(first, &r_dig_sta, 100);
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
//            ERROR TABLE PANEL
//
//~============================================================================


void create_error_table_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 350, h1 = 445, h2 = 25;

  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  ERR_TAB_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1, h1, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
//  fl_set_browser_fontstyle(obj, FL_FIXED_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
//  fl_set_browser_fontsize(obj, FL_SMALL_SIZE);
  y = y + h1;
  
  obj = add_normal_button(x, y, w1, h2, "READ & RESET  ERROR  INFO", 'N');
    fl_set_object_callback(obj, get_error_table_callback, 0);
  y = y + h2;
}

//~-------------------------------------------------------------------------------

void get_error_table_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_err_tab;
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  err_tab_updated = 1;
  
  err = 0x0000;
  
  CAB_read_and_reset_error_info(&cab_err, &err);

//cab_err.cod[0] = 0x30; // the longest message
  if (!err) {
    err_tab_unk = r->unk;
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

void update_error_table_panel(void) {

  FL_OBJECT *obj = ERR_TAB_BRO;
  int i;

  if (err_tab_updated < 0) return;

  fl_freeze_form(MAIN);
  fl_clear_browser(obj);

  fl_add_browser_line(obj, "@cError Table");
  if (cab_err.n) fl_add_browser_line(obj, "Nb  Time,s Error Code & Parameters");

  for (i=0; i<cab_err.n; i++) {
    char b[80];
    sprintf(b, "%2d %7.1f %02hhX %s %02hhX %02hhX", i, 
            0.0125*(float)cab_err.tim[i], cab_err.cod[i], 
            cab_err_message(cab_err.cod[i]), 
            (cab_err.par[i]&0xFF00)>>8,
            (cab_err.par[i]&0x00FF)>>0);
    fl_add_browser_line(obj, b);
  }

  fl_unfreeze_form(MAIN);
  err_tab_updated = -1;
}

//~============================================================================
//
//            ESSENTIAL STATUS PANEL
//
//~============================================================================

void create_essential_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 370, h1 = 830, h2 = 25;

  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  ESS_STA_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1, h1, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
//  fl_set_browser_fontstyle(obj, FL_FIXED_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
//  fl_set_browser_fontsize(obj, FL_SMALL_SIZE);
  y = y + h1;
  
  obj = add_normal_button(x, y, w1, h2, "GET  ESSENTIAL  STATUS", 'N');
    fl_set_object_callback(obj, get_essential_status_callback, 0);
  y = y + h2;
}

//~-------------------------------------------------------------------------------

void get_essential_status_callback(FL_OBJECT* obj, long data) {

  int i;
  _r *r = &r_ess_sta;
  int8 sta[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  ess_sta_updated = 1;
  
  err = 0x0000;
  
  CAB_get_essential_status(sta, &err);

  if (!err) {
    for (i=0; i<100; i++) r->rea[i] = sta[i];
    ess_sta_unk = r->unk;
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

void update_essential_status_panel(void) {

  FL_OBJECT *obj = ESS_STA_BRO;
  int i, n;
  int8 sta[100];
  char b_rea[100][80], b_was[100][80];
  _r *r = &r_ess_sta;

  if (ess_sta_updated < 0) return;

  if (!ess_sta_unk) {
    for (i=0; i<fl_get_browser_maxline(obj); i++) {
      strcpy(b_was[i], fl_get_browser_line(obj, i+1));
    }
  }

  for (i=0; i<100; i++) sta[i] = r->rea[i];

  sprintf(b_rea[0], "@cCAB Essential TM Status");
  sprint_CAB_essential_status(sta, &n, &b_rea[1]);

  fl_freeze_form(MAIN);
  fl_clear_browser(obj);

  for (i=0; i<n+1; i++) {
    fl_add_browser_line(obj, b_rea[i]);
    if (!ess_sta_unk && strcmp(b_rea[i], b_was[i])) {
      fl_select_browser_line(obj, i+1);
    }
    if (r->unk) break;
  }

  fl_unfreeze_form(MAIN);
  ess_sta_updated = -1;
}

//~============================================================================
//
//            DIGITAL STATUS PANEL
//
//~============================================================================

void create_digital_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 300, h1 = 830, h2 = 25;

  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  DIG_STA_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1, h1, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
//  fl_set_browser_fontstyle(obj, FL_FIXED_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
//  fl_set_browser_fontsize(obj, FL_SMALL_SIZE);
  y = y + h1;

  obj = add_normal_button(x,      y, w1/2, h2, "GET DIGITAL STATUS N", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 0);
  obj = add_normal_button(x+w1/2, y, w1/2, h2, "GET DIGITAL STATUS R", 'N');
    fl_set_object_callback(obj, get_digital_status_callback, 1);
  y = y + h2;
}

//~-------------------------------------------------------------------------------

void get_digital_status_callback(FL_OBJECT* obj, long data) {

  int cha = data;
  int i;
  _r *r = &r_dig_sta;
  int8 sta[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  dig_sta_updated = cha;
  
  err = 0x0000;
  
  CAB_get_digital_status(cha, sta, &err);

  if (!err) {
    for (i=0; i<100; i++) r->rea[i] = sta[i];
    dig_sta_unk = r->unk;
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

void update_digital_status_panel(void) {

  FL_OBJECT *obj = DIG_STA_BRO;
  int i, n;
  int8 sta[100];
  char b_rea[100][80], b_was[100][80];
  _r *r = &r_dig_sta;

  if (dig_sta_updated < 0) return;

  if (!dig_sta_unk) {
    for (i=0; i<fl_get_browser_maxline(obj); i++) {
      strcpy(b_was[i], fl_get_browser_line(obj, i+1));
    }
  }

  for (i=0; i<100; i++) sta[i] = r->rea[i];

  sprintf(b_rea[0], "@cCAB Digital TM Status Chain %s", dig_sta_updated ? "R" : "N");
  sprint_CAB_digital_status(sta, &n, &b_rea[1]);

  fl_freeze_form(MAIN);
  fl_clear_browser(obj);

  for (i=0; i<n+1; i++) {
    fl_add_browser_line(obj, b_rea[i]);
    if (!dig_sta_unk && strcmp(b_rea[i], b_was[i])) {
      fl_select_browser_line(obj, i+1);
    }
    if (r->unk) break;
  }

  fl_unfreeze_form(MAIN);
  dig_sta_updated = -1;
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

  update_essential_status_panel();
  update_error_table_panel();
  update_digital_status_panel();
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
