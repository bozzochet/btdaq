// file JLV1-1.c
//
// JLV1-1a "Pro" Controller
//
// A.Lebedev Feb-2006...
// A.Basili Oct-2008...

#include "jlv1def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"JLV1-1:  Controller"};       // global variable
char *date  = {"13-Dec-08"};                 // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"
int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~-- "all" JLV1 registers

int n_reg = 64;
_r r_reg;
_ww ww_reg;

//~-- Dependants Lists ---

_r  *r_list[100]  = {NULL};
_ww *ww_list[100] = {NULL};

//~-- Boot JINF Panel

SW_BTN *BOOT_ADR_BTN[4];
_w w_boot_adr;

//~-- Load Boot Panel

FL_OBJECT *FILE_HEADER_BOX[8];
_r r_file_header;

//~-- Internal Trigger Panel

SW_BTN  *INT_TRG_MSK_BTN[1];
SW_BTN  *INT_TRG_NUM_BTN[6];
DAT_BOX *INT_TRG_MSK_BOX[1];
DAT_BOX *INT_TRG_NUM_BOX[1];
_r   r_int_trg;
_ww ww_int_trg;

//~-- Global Trigger Panel

DAT_BOX *GLOB_TRG_BOX[1];
_r   r_glob_trg;

//~-- EPPCAN Box Trigger & Busy Panel

SW_BTN    *EPP_TRIG_MSK_BTN[2];
_w w_epp_trig_msk;
FL_OBJECT *EPP_BUSY_STA_BOX[2];
_r r_epp_busy_sta;

//~-- Ping JINF Panel

FL_OBJECT *PING_STA_BOX[1];
_r r_ping_sta;

//~-- Processing Mode Panel

FL_OBJECT *PROC_MODE_BOX[1];
SW_BTN    *PROC_MODE_BTN[1];
_r r_proc_mode;
_w w_proc_mode;

//~-- Event Number Panel

FL_OBJECT *EVENT_NB_DEC_BOX[1];
FL_OBJECT *EVENT_NB_HEX_BOX[4];
_r r_event_nb;

//~-- Get Event Panel

FL_OBJECT *GET_EVENT_BOX[1];
_r r_get_event;
bool auto_get_event = FALSE;

//~-- JINF Status Panel

FL_OBJECT *JINF_STA_BOX[18];
_r r_jinf_sta;

//~-- PPS Input Masks Panel

SW_BTN    *PPS_MSK_BTN[2];
_w w_PPS_msk;
FL_OBJECT *PPS_MSK_BOX[2];
_r r_PPS_msk;

//~-- Live Time Panel

SW_BTN    *LIVE_TIME_BTN[2];
FL_OBJECT *LIVE_TIME_BOX[2][2];
_r r_live_time;

//~-- Time Panel

DAT_BOX *TIME_BOX[5];
_r r_time;

//~-- Busy Registers (Mask, Status, Errors)

SW_BTN  *BUSY_MSK_BTN[3][16];
DAT_BOX *BUSY_MSK_BOX[3][16];
_r   r_busy_msk;
_ww ww_busy_msk;
DAT_BOX *BUSY_STA_BOX[3][16];
_r   r_busy_sta;
DAT_BOX *BUSY_ERR_BOX[3][16];
_r   r_busy_err;

//~-- Scalers

DAT_BOX *GATE_BOX;
DAT_BOX *SCALER_BOX[0x60];
_r   r_gate;
_r   r_scaler;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
char *dec_dot[10] = {
"0.", "1.", "2.", "3.", "4.", "5.", "6.", "7.", "8.", "9."};
char *five[10] = {
"0", NULL, NULL, NULL, NULL, "5", NULL, NULL, NULL, NULL};
char *mod[4] = {"Off", "Raw", "Comp", "Raw+Comp"};
char *got[2] = {"Empty", "Got It"};
char *o_o[2] = {"OFF", "ON"};
char *e_d[2] = {"E", "D"};
char *fai[2] = {"FAIL", "O.K."};
char *__m[2] = {" ", "M"};
char *__s[2] = {" ", "S"};
char *e__[2] = {"E", " "};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_global_trigger_control_panel(int x0, int y0);
void control_global_trigger_callback(FL_OBJECT* obj, long data);
void read_global_trigger_status_callback(FL_OBJECT* obj, long data);
void update_global_trigger_control_panel(void);

void create_boot_panel(int x0, int y0);
void boot_callback(FL_OBJECT* obj, long data);
void update_boot_panel(void);

void create_load_boot_panel(int x0, int y0);
void load_boot_callback(FL_OBJECT* obj, long data);
void read_FLASH_summary_callback(FL_OBJECT* obj, long data);
void update_load_boot_panel(void);

//~--

void create_trigger_generator_panel(int x0, int y0);
void convert_trigger_period_code_to_value(int32 code, int32 *value);
void convert_trigger_period_value_to_code(int32 value, int32 *code);
void convert_trigger_period_code_to_float(int32 code, int32 *value);
void read_trigger_generator_status_callback(FL_OBJECT* obj, long data);
void write_trigger_generator_status_callback(FL_OBJECT* obj, long data);
void update_trigger_generator_panel(void);

void create_DSP_trigger_panel(int x0, int y0);
void generate_DSP_trigger_callback(FL_OBJECT* obj, long data);

void create_EPPCAN_box_panel(int x0, int y0);
void generate_EPPCAN_box_trigger_callback(FL_OBJECT* obj, long data);
void read_EPPCAN_box_busy_callback(FL_OBJECT* obj, long data);
void update_EPPCAN_box_panel(void);

//~--

void create_ping_panel(int x0, int y0);
void ping_callback(FL_OBJECT* obj, long data);
void update_ping_panel(void);

void create_proc_mode_panel(int x0, int y0);
void read_proc_mode_callback(FL_OBJECT* obj, long data);
void write_proc_mode_callback(FL_OBJECT* obj, long data);
void update_proc_mode_panel(void);

void create_event_nb_panel(int x0, int y0);
void read_event_nb_callback(FL_OBJECT* obj, long data);
void reset_event_FIFO_callback(FL_OBJECT* obj, long data);
void update_event_nb_panel(void);

void create_get_event_panel(int x0, int y0);
void get_event_callback(FL_OBJECT* obj, long data);
void auto_get_event_callback(int tid, void *data);
void update_get_event_panel(void);

//~--

void create_status_panel(int x0, int y0);
void read_status_callback(FL_OBJECT* obj, long data);
void update_status_panel(void);

//~--

void create_time_panel(int x0, int y0);
void read_time_callback(FL_OBJECT* obj, long data);
void reset_time_callback(FL_OBJECT* obj, long data);
void update_time_panel(void);

//~--

void create_busy_registers_panel(int x0, int y0);
void reset_busy_mask_callback(FL_OBJECT* obj, long data);
void read_busy_register_callback(FL_OBJECT* obj, long data);
void write_busy_register_callback(FL_OBJECT* obj, long data);
void update_busy_registers_panel(void);

//~--

void create_scalers_panel(int x0, int y0);
void read_scalers_callback(FL_OBJECT* obj, long data);
void update_scalers_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 790, HH = 670;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "JLV1 Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_global_trigger_control_panel(0,  45);
  create_boot_panel(                  0, 100);

  create_load_boot_panel(70, 45);

  create_trigger_generator_panel(235, 45);
  create_DSP_trigger_panel(      405, 45);
  if (!use_TCP) create_EPPCAN_box_panel(       485, 45);

  create_ping_panel(     235, 85);
  create_proc_mode_panel(285, 85);
  create_event_nb_panel( 385, 85);
  create_get_event_panel(535, 85);

  create_status_panel(0, 140);

  create_time_panel(0, 195);

  create_busy_registers_panel(0, 250);

  create_scalers_panel(0, 465);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_dependant_r(_r *r) {

  int i;

  r->unk = TRUE;
  r->sta = status_NA;
  r->was = r_reg.was;
  r->rea = r_reg.rea;

  for (i=0; i<sizeof(r_list)-1; i++) {
    if (r_list[i]) continue;
    r_list[i]   = r;
    r_list[i+1] = NULL;
    break;
  }
}

//~----------------------------------------------------------------------------

void invalidate_dependant_ww(_ww *ww) {

  int i;

  ww->unk = TRUE;
  ww->inp = ww_reg.inp;
  ww->wri = ww_reg.wri;

  for (i=0; i<sizeof(ww_list)-1; i++) {
    if (ww_list[i]) continue;
    ww_list[i]   = ww;
    ww_list[i+1] = NULL;
    break;
  }
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  static bool first = TRUE;

//~-- "all registers" initialization must be the first

  invalidate_r(first, &r_reg, n_reg);
  invalidate_ww(first, &ww_reg, n_reg, 0x0000);

//~--

  invalidate_w(&w_boot_adr, 1, 0x0000);

  invalidate_r(first, &r_file_header, 8);
  
  invalidate_dependant_r(&r_int_trg);
  invalidate_dependant_ww(&ww_int_trg);

  invalidate_dependant_r(&r_glob_trg);

  invalidate_r(first, &r_epp_busy_sta, 1);
  invalidate_w(&w_epp_trig_msk, 1, 0x0000);
  
  invalidate_r(first, &r_ping_sta, 1);

  invalidate_r(first, &r_proc_mode, 1);
  invalidate_w(&w_proc_mode, 1, 0x0000);

  invalidate_r(first, &r_event_nb, 1);

  invalidate_r(first, &r_get_event, 1);
  
  invalidate_r(first, &r_jinf_sta, 18);

  invalidate_r(first, &r_PPS_msk, 1);
  invalidate_w(&w_PPS_msk, 1, 0x0000);

  invalidate_r(first, &r_time, 7);

  invalidate_r(first, &r_live_time, 4);

  invalidate_dependant_r ( &r_int_trg);
  invalidate_dependant_ww(&ww_int_trg);

  invalidate_dependant_r(&r_busy_msk);
  invalidate_dependant_ww(&ww_busy_msk);
  invalidate_dependant_r(&r_busy_sta);
  invalidate_dependant_r(&r_busy_err);
  
  invalidate_r(first, &r_gate, 1);
  invalidate_r(first, &r_scaler, 0x60);

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
    fread(&w_boot_adr.inp,     4,     1, file);
    fread(&w_proc_mode.inp,    4,     1, file);
    fread(&w_epp_trig_msk.inp, 4,     1, file);
    fread(&w_PPS_msk.inp,      4,     1, file);
    fread(ww_reg.inp,          4, n_reg, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(&w_boot_adr.inp,     4,     1, file);
    fwrite(&w_proc_mode.inp,    4,     1, file);
    fwrite(&w_epp_trig_msk.inp, 4,     1, file);
    fwrite(&w_PPS_msk.inp,      4,     1, file);
    fwrite(ww_reg.inp,          4, n_reg, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            GLOBAL TRIGGER CONTROL PANEL
//
//~============================================================================

  extern _b glob_trg_msk;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_global_trigger_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 20, w3 = 20, h1 = 15;
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2+w3, h1, "Trigger", 'S');
  y = y + h1;

  obj = add_normal_button(x, y,    w1, h1, "D", 'N');
    fl_set_object_callback(obj, control_global_trigger_callback, 0);
  obj = add_normal_button(x, y+h1, w1, h1, "E", 'N');
    fl_set_object_callback(obj, control_global_trigger_callback, 1);
  x = x + w1;
  
  create_dat_box_hstring(
    1, x, y, w2, 2*h1, 'L', &r_glob_trg, &glob_trg_msk, GLOB_TRG_BOX);
  x = x + w2;

  obj = add_normal_button(x, y, w3, 2*h1, "R", 'N');
    fl_set_object_callback(obj, read_global_trigger_status_callback, 0);
}

//~----------------------------------------------------------------------------

void control_global_trigger_callback(FL_OBJECT* obj, long data) {

  int16 msk = data ? 0x0000 : glob_trg_msk.msk;
  _r  *r = &r_glob_trg;
  int ind = glob_trg_msk.ind;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  begin_IO_segment(request);
  add_IO_write(request, ind, msk);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_global_trigger_status_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_glob_trg;
  int ind = glob_trg_msk.ind;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[ind] = r->rea[ind];
  
  begin_IO_segment(request);
  add_IO_read(request, ind);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);

  if (!err) {
    r->rea[ind] = reply[3];
    if (r->unk) r->was[ind] = r->rea[ind];
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

void update_global_trigger_control_panel(void) {

  show_dat_box_string(1, e_d, NULL, GLOB_TRG_BOX);
}

//~============================================================================
//
//            BOOT JINF PANEL
//
//~============================================================================

void create_boot_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 15, h1 = 15, h2 = 15;
  int size;
  
  w = 5 + 4 * w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_NORMAL_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  create_hex_btn_hstring(
    4, x, y, w1, h1, hex, size, &w_boot_adr, &BOOT_ADR_BTN[0]);
  y = y + h1;

  obj = add_normal_button(x, y, 4*w1, h2, "BOOT", size);
    fl_set_object_callback(obj, boot_callback, 0);
}

//~----------------------------------------------------------------------------

void boot_callback(FL_OBJECT* obj, long data) {

  _w *w = &w_boot_adr;

  msg = -1;
  update_objects();

  boot_JINF((int16)w->inp, &err);
  
  if (!err) {
    w->wri = w->inp;
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_boot_panel(void) {

  show_bin_btn_string(4, &w_boot_adr, &BOOT_ADR_BTN[0]);
}

//~============================================================================
//
//            LOAD BOOT PANEL
//
//~============================================================================

void create_load_boot_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 35, w2 = 20, h1 = 15, h2 = 25;
  int size;
  
  w = 5 + 2 * (w1 + 2 * w2) + 5 + 5;
  h = 5 + 4 * h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  size = FL_NORMAL_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;

  create_data_box_vstring(4, x, y, w1, h1, size, &FILE_HEADER_BOX[0]);
  
  for (i=0; i<4; i++) {
    obj = add_normal_button(x+w1,    y, w2, h1, "L", size);
      fl_set_object_callback(obj, load_boot_callback, 8*0+i);
    obj = add_normal_button(x+w1+w2, y, w2, h1, "B", size);
      fl_set_object_callback(obj, load_boot_callback, 8*1+i);
    y = y + h1;
  }

  x = x + w1 + 2 * w2 + 5;
  y = y0 + 5;

  create_data_box_vstring(4, x, y, w1, h1, size, &FILE_HEADER_BOX[4]);
  
  for (i=4; i<8; i++) {
    obj = add_normal_button(x+w1,    y, w2, h1, "L", size);
      fl_set_object_callback(obj, load_boot_callback, 8*0+i);
    obj = add_normal_button(x+w1+w2, y, w2, h1, "B", size);
      fl_set_object_callback(obj, load_boot_callback, 8*1+i);
    y = y + h1;
  }

  x = x0 + 5;

  obj = add_normal_button(x, y, 2*(w1+2*w2)+5, h2, "GET FLASH SUMMARY", size);
    fl_set_object_callback(obj, read_FLASH_summary_callback, 0);
}

//~----------------------------------------------------------------------------

void load_boot_callback(FL_OBJECT* obj, long data) {

  int ind = data / 8;
  int i   = data % 8;
  _r *r = &r_file_header;
    
  msg = -1;
  update_objects();

  if (ind) boot_JINF((int16)r->rea[i], &err);
  else     load_JINF_FLASH_file((int16)r->rea[i], &err);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_FLASH_summary_callback(FL_OBJECT* obj, long data) {

  int i;
  _r *r = &r_file_header;
  int fmt;
  int16 dat[8][4];

  msg = -1;
  update_objects();

  r->was = r->rea;

  get_JINF_FLASH_summary(&fmt, &dat[0][0], NULL, NULL, NULL, &err);
  
  if (!err && !fmt) {
    for (i=0; i<8; i++) r->rea[i] = dat[i][0];
    if (r->unk) for (i=0; i<8; i++) r->was[i] = r->rea[i];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_load_boot_panel(void) {

  int i;
  int style;
  _r *r = &r_file_header;
  
  show_num_box_string(8, r, NULL, "%04X", &FILE_HEADER_BOX[0]);

  for (i=0; i<8; i++) {
    if (unpack16(r->rea[i], 0x3000) == 2) style = FL_FIXEDBOLDITALIC_STYLE;
    else                                  style = FL_FIXEDBOLD_STYLE;
    fl_set_object_lstyle(FILE_HEADER_BOX[i], style);
  }
}

//~============================================================================
//
//            TRIGGER GENERATOR PANEL
//
//~============================================================================

  extern _b int_trg_msk;
  extern _b int_trg_dec[6];
  extern _b int_trg_num;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_trigger_generator_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 15, w3 = 20, h1 = 15;

  w = 5 + w1 + w2 + 6 * w2 + 5 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Gen", 'S');
  x = x + w1;

  create_btn_hstring(
    1, x, y, w2,    h1, e_d, 'N', &ww_int_trg, &int_trg_msk, INT_TRG_MSK_BTN);
  create_btn10_hstring(
    6, x+w2, y, w2, h1, hex, 'N', &ww_int_trg,  int_trg_dec, INT_TRG_NUM_BTN);
  INT_TRG_NUM_BTN[3]->l = dec_dot;
  INT_TRG_NUM_BTN[5]->l = five;
  for (i=0; i<6; i++) {
    SW_BTN *B = INT_TRG_NUM_BTN[i];
    B->cnv1 = convert_trigger_period_code_to_value;
    B->cnv2 = convert_trigger_period_value_to_code;
    if (i > 2) {
      FL_OBJECT *obj = B->obj;
      int x, y, w, h;
      if (i == 3) {
        fl_get_object_geometry(obj, &x, &y, &w, &h);
        w = w + 5;
        fl_set_object_geometry(obj,  x,  y,  w,  h);
      }
      else {
        fl_get_object_geometry(obj, &x, &y, &w, &h);
        x = x + 5;
        fl_set_object_geometry(obj,  x,  y,  w,  h);
      }
    }
  }
  obj = add_normal_button(x+w2+6*w2+5, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_trigger_generator_status_callback, 0);
  y = y + h1;

  create_dat_box_hstring(
    1, x,    y,     w2, h1, 'N', &r_int_trg, &int_trg_msk, INT_TRG_MSK_BOX);
  create_dat_box_hstring(
    1, x+w2, y, 6*w2+5, h1, 'N', &r_int_trg, &int_trg_num, INT_TRG_NUM_BOX);
  INT_TRG_NUM_BOX[0]->cnv = convert_trigger_period_code_to_float;
  obj = add_normal_button(x+w2+6*w2+5, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_trigger_generator_status_callback, 0);
}

//~----------------------------------------------------------------------------

void convert_trigger_period_code_to_value(int32 code, int32 *value) {

  *value = 5 * (code + 1);
}

//~----------------------------------------------------------------------------

void convert_trigger_period_value_to_code(int32 value, int32 *code) {

  *code = value / 5 - 1;
}

//~----------------------------------------------------------------------------

void convert_trigger_period_code_to_float(int32 code, int32 *value) {

  float tmp = 0.05 * (code + 1);
  *value = *(int32*)&tmp;
}

//~----------------------------------------------------------------------------

void read_trigger_generator_status_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_int_trg;
  int ind = int_trg_msk.ind;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[ind] = r->rea[ind];

  begin_IO_segment(request);
  add_IO_read(request, ind);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);

  if (!err) {
    r->rea[ind] = reply[3];
    if (r->unk) r->was[ind] = r->rea[ind];
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

void write_trigger_generator_status_callback(FL_OBJECT* obj, long data) {

  _r  *r = &r_int_trg;
  _ww *w = &ww_int_trg;
  int ind = int_trg_msk.ind;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  w->inp[ind] &= 0x0000FFFF;

  begin_IO_segment(request);
  add_IO_write(request, ind, w->inp[ind]);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);
  
  if (!err) {
    w->wri[ind] = w->inp[ind];
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

void update_trigger_generator_panel(void) {

  show_dat_box_string(1,  e_d,  NULL,      INT_TRG_MSK_BOX);
  show_dat_box_string(1, NULL, "P=%.2fms", INT_TRG_NUM_BOX);
  show_btn_string(    1,        NULL,      INT_TRG_MSK_BTN);
  show_btn_string(    6,        NULL,      INT_TRG_NUM_BTN);
}

//~============================================================================
//
//            DSP TRIGGER PANEL
//
//~============================================================================

  extern _b glob_trg_msk;
  extern _b glob_trg_gen;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_DSP_trigger_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 70, h1 = 30;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "DSP\nTRIGGER", 'N');
    fl_set_object_callback(obj, generate_DSP_trigger_callback, 0);
}

//~----------------------------------------------------------------------------

void generate_DSP_trigger_callback(FL_OBJECT* obj, long data) {

  int ind = glob_trg_gen.ind;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  begin_IO_segment(request);
  add_IO_write(request, ind, glob_trg_gen.msk);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            EPPCAN BOX TRIGGER & BUSY PANEL
//
//~============================================================================

void create_EPPCAN_box_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 15, w3 = 40, h1 = 15;
  
  w = 5 + w1 + 2 * w2 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "EPP\nCAN", 'S');
  x = x + w1;

  create_bit_btn_hstring(
    2, x, y, w2, h1, hex, 'N', &w_epp_trig_msk, &EPP_TRIG_MSK_BTN[0]);

  obj = add_normal_button(x+2*w2, y, w3, h1, "TRIG", 'N');
    fl_set_object_callback(obj, generate_EPPCAN_box_trigger_callback, 0);
  y = y + h1;

  create_data_box_hstring(2, x, y, w2, h1, 'N', &EPP_BUSY_STA_BOX[0]);

  obj = add_normal_button(x+2*w2, y, w3, h1, "BUSY", 'N');
    fl_set_object_callback(obj, read_EPPCAN_box_busy_callback, 0);
}

//~----------------------------------------------------------------------------

void generate_EPPCAN_box_trigger_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_epp_busy_sta;
  _w *w = &w_epp_trig_msk;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  r->sta = status_NA;
  
  generate_EPPCAN_box_trigger((int16)w->inp, &err);
  
  if (!err) {
    w->wri = w->inp;
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

void read_EPPCAN_box_busy_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_epp_busy_sta;
  int16 dat;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  
  get_EPPCAN_box_busy(&dat, &err);

  if (!err) {
    r->rea[0] = dat;
    if (r->unk) r->was[0] = r->rea[0];
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

void update_EPPCAN_box_panel(void) {

  show_bin_btn_string(2, &w_epp_trig_msk,       &EPP_TRIG_MSK_BTN[0]);
  show_bit_box_string(2, &r_epp_busy_sta, NULL, &EPP_BUSY_STA_BOX[0]);
}

//~============================================================================
//
//            PING PANEL
//
//~============================================================================

void create_ping_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, h1 = 30, h2 = 15;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  
  obj = add_normal_button(x, y, w1, h1, "PING", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, ping_callback, 0);
  y = y + h1;

  create_data_box_hstring(1, x, y, w1, h2, FL_NORMAL_SIZE, &PING_STA_BOX[0]);
}

//~----------------------------------------------------------------------------

void ping_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ping_sta;
  int n, nn;
  int16 dat[2*8192], datdat[2*8192+10];
  int i;
    
  msg = -1;
  update_objects();

  n = 8000;
  for (i=0; i<n; i++) dat[i] = i;

  *r->was = *r->rea;
  
  ping_JINF(n, dat, &nn, datdat, &err);

  if (!err) {
    bool ping_ok = TRUE;
    if (n != nn) {
      ping_ok = FALSE;
    }
    else {
      for (i=0; i<n; i++) {
        if (dat[i] != datdat[i]) {
          ping_ok = FALSE;
          break;
        }
      }
    }  
    *r->rea = ping_ok;
    if (r->unk) *r->was = *r->rea;
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_ping_panel(void) {

  FL_OBJECT **ob = &PING_STA_BOX[0];
  FL_OBJECT *obj = *ob;
  _r *r = &r_ping_sta;

  show_hex_box_string(1, r, fai, ob);

  if (!r->unk && (r->sta == status_OK)) {
    if (*r->rea) {
      fl_set_object_color( obj, FL_GREEN, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    else {
      fl_set_object_color( obj, FL_RED, 0);
      fl_set_object_lcolor(obj, FL_YELLOW);
    }
  }
}

//~============================================================================
//
//            PROCESSING MODE PANEL
//
//~============================================================================

void create_proc_mode_panel(int x0, int y0) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 65, w2 = 25, h1 = 15;
  int size;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_SMALL_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2, h1, "Processing Mode", size);
  y = y + h1;

  size = FL_NORMAL_SIZE;
  
  PROC_MODE_BTN[0] =
  B = add_switch_btn(x, y, w1, h1, mod, size, 4, w_proc_mode.inp&0x3);
  B->p = &w_proc_mode.inp;
  B->m = 0x00000003;
  B->call = SW_BTN_CALLBACK;
  obj = add_normal_button(x+w1, y, w2, h1, "W", size);
    fl_set_object_callback(obj, write_proc_mode_callback, 0);
  y = y + h1;

  create_data_box_hstring(1, x, y, w1, h1, size, &PROC_MODE_BOX[0]);
  obj = add_normal_button(x+w1, y, w2, h1, "R", size);
    fl_set_object_callback(obj, read_proc_mode_callback, 0);
}

//~----------------------------------------------------------------------------

void read_proc_mode_callback(FL_OBJECT* obj, long data) {

  int16 proc_mode;
  
  msg = -1;
  update_objects();

  *r_proc_mode.was = *r_proc_mode.rea;
  
  get_JINF_proc_mode(&proc_mode, &err);

  if (!err) {
    *r_proc_mode.rea = proc_mode;
    if (r_proc_mode.unk) *r_proc_mode.was = *r_proc_mode.rea;
    r_proc_mode.unk = FALSE;
    r_proc_mode.sta = status_OK;
  }
  else {
    r_proc_mode.sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_proc_mode_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  set_JINF_proc_mode(w_proc_mode.inp, &err);
  
  if (!err) {
    w_proc_mode.wri = w_proc_mode.inp;
    w_proc_mode.unk = FALSE;
  }
  else {
    w_proc_mode.unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_proc_mode_panel(void) {

  show_hex_box_string(1, &r_proc_mode, mod, &PROC_MODE_BOX[0]);
  show_bin_btn_string(1, &w_proc_mode,      &PROC_MODE_BTN[0]);
}

//~============================================================================
//
//            EVENT NUMBER PANEL
//
//~============================================================================

void create_event_nb_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 60, w2 = 15, w3 = 20, h1 = 15;
  int size;

  w = 5 + w1 + 4 * w2 + w3 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  size = FL_SMALL_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 3*h1, "Last\nProcessed\nEvent Nb", size);
  x = x + w1;

  size = FL_NORMAL_SIZE;
  
  create_data_box_hstring(1, x, y, 4*w2, h1, size, &EVENT_NB_DEC_BOX[0]);

  obj = add_normal_button(x+4*w2, y, w3, 2*h1, "R", size);
    fl_set_object_callback(obj, read_event_nb_callback, 0);
  y = y + h1;

  create_data_box_hstring(4, x, y, w2, h1, size, &EVENT_NB_HEX_BOX[0]);
  y = y + h1;

  obj = add_normal_button(x, y, 4*w2+w3, h1, "RESET  FIFO", size);
    fl_set_object_callback(obj, reset_event_FIFO_callback, 0);
}

//~----------------------------------------------------------------------------

void read_event_nb_callback(FL_OBJECT* obj, long data) {

  int16 event_nb;
  _r *r = &r_event_nb;
  
  msg = -1;
  update_objects();

  *r->was = *r->rea;
  
  get_JINF_event_nb(&event_nb, &err);

  if (!err) {
    *r->rea = event_nb;
    if (r->unk) *r->was = *r->rea;
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void reset_event_FIFO_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  reset_JINF_event_FIFO(&err);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_event_nb_panel(void) {

  show_num_box_string(1, &r_event_nb, NULL, "%d", &EVENT_NB_DEC_BOX[0]);
  show_hex_box_string(4, &r_event_nb, NULL,       &EVENT_NB_HEX_BOX[0]);
}

//~============================================================================
//
//            GET EVENT PANEL
//
//~============================================================================

void create_get_event_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 50, h1 = 30, h2 = 15;
  int size;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_NORMAL_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  obj = add_normal_button(x, y, w1, h1, "GET\nEVENT", size);
    fl_set_object_callback(obj, get_event_callback, 0);
  y = y + h1;

  create_data_box_hstring(1, x, y, w1, h2, size, &GET_EVENT_BOX[0]);
}

//~----------------------------------------------------------------------------

void get_event_callback(FL_OBJECT* obj, long data) {

  bool got_event;
  _r *r = &r_get_event;
  int16 b[16384];
  int n;
  
  if (obj) {
    if (auto_get_event) {
      auto_get_event = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_get_event = TRUE;
        fl_add_timeout(0, auto_get_event_callback, NULL);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_get_event) {
    msg = -1;
    update_objects();
  }
  
  *r->was = *r->rea;
  
  got_event = get_JINF_event(&n, b, &err);

  if (!err) {
    *r->rea = got_event;
    if (r->unk) *r->was = *r->rea;
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

  if (auto_get_event) msg = 0;
  else                msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void auto_get_event_callback(int tid, void *data) {

  if (!auto_get_event) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback dt = %f\n", delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  get_event_callback(NULL, 0);
  fl_add_timeout(0, auto_get_event_callback, NULL);
}

//~----------------------------------------------------------------------------

void update_get_event_panel(void) {

  show_hex_box_string(1, &r_get_event, got, &GET_EVENT_BOX[0]);
}

//~============================================================================
//
//            JLV1 STATUS PANEL
//
//~============================================================================

static char *t0[16] = {"ROM", "DAQ"};
static char *t1[16] = {"?",   "CDP", "CDDC"};
static char *t2[16] = {"CDDC","TDR", "UDR", "RDR", "EDR", "SDR", "JLV1", "ETRG"};

static struct {
  int   wa;
  int   ha;
  char *ta;
  int   wb;
  int   hb;
  char *tb;
  int   i;
  int16 m;
  char *fmt;
  char **txt;
} *q, Q[18] = {
  
  { 35, 2, "Vers",          0, 0,   0, 1, 0xFFFF, "%04X", NULL},
  { 35, 2, "Pgm" ,          0, 0,   0, 2, 0xF000, "%s",   t0},
  { 35, 2, "Node",          0, 0,   0, 2, 0x0F00, "%s",   t1},
  { 35, 2, "Node\nType",    0, 0,   0, 2, 0x00F0, "%s",   t2},
  { 35, 2, "Ports\nto M",   0, 0,   0, 2, 0x000F, "%d",   NULL},
  { 35, 2, "SubD\nVers",    0, 0,   0, 3, 0xFFFF, "%04X", NULL},
  {105, 1, "Errors",       15, 1, "E", 6, 0x4000, "%d",   NULL},
  {  0, 1,  NULL,          15, 1, "A", 6, 0x2000, "%d",   NULL},
  {  0, 1,  NULL,          15, 1, "F", 6, 0x1000, "%d",   NULL},
  {  0, 1,  NULL,          15, 1, "B", 6, 0x0800, "%d",   NULL},
  {  0, 1,  NULL,          15, 1, "P", 6, 0x0400, "%d",   NULL},
  {  0, 1,  NULL,          15, 1, "D", 6, 0x0200, "%d",   NULL},
  {  0, 1,  NULL,          15, 1, "T", 6, 0x0100, "%d",   NULL},
  { 50, 2, "Time\nsec",     0, 0,   0, 4,      0, "%d",   NULL},
  { 40, 2, "Last\nEvent#",  0, 0,   0, 7, 0xFFFF, "%d",   NULL},
  { 40, 2, "Bldg.\nErrors", 0, 0,   0, 8, 0xFFFF, "%d",   NULL},
  { 40, 2, "AMSw\nErrors",  0, 0,   0, 9, 0xFFFF, "%d",   NULL},
  { 40, 2, "Flash\nErrors", 0, 0,   0,10, 0xFFFF, "%d",   NULL}};

//~----------------------------------------------------------------------------

void create_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 60, h1 = 15;
    
  w = 5;
  for (i=0; i<18; i++) w = w + Q[i].wa;
  w = w + w1 + 5;
  h = 5 + 3 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  for (i=0; i<18; i++) {
    q = &Q[i];
    if (q->wa) {
      add_frame_box(x, y, q->wa, h1*q->ha, q->ta, FL_SMALL_SIZE);
      if (!q->wb) {
        JINF_STA_BOX[i] =
        add_data_box( x, y+h1*q->ha, q->wa, h1, "", FL_NORMAL_SIZE);
        x = x + q->wa;
      }
    }
    if (q->wb) {
      add_frame_box(x, y+h1*q->ha, q->wb, h1*q->hb, q->tb, FL_SMALL_SIZE);
      JINF_STA_BOX[i] =
      add_data_box( x, y+h1*(q->ha+q->hb), q->wb, h1, "", FL_NORMAL_SIZE);
      x = x + q->wb;
    }
  }

  obj = add_normal_button(x, y, w1, 3*h1, "GET\nSTATUS", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_status_callback, 0);
}

//~----------------------------------------------------------------------------

void read_status_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_jinf_sta;
  int i;
  int fmt;
  int16 status[10];
  
  msg = -1;
  update_objects();

  for (i=0; i<18; i++) r->was[i] = r->rea[i];
  
  get_JINF_status(&fmt, status, NULL, NULL, NULL, &err);

  if (!err) {
    for (i=0; i<18; i++) {
      q = &Q[i];
      if (q->m) {
        r->rea[i] = unpack32((int32)status[(q->i)-1], (int32)q->m);
      }
      else {
        r->rea[i] = (int32)status[q->i] << 16 | 
                    (int32)status[q->i-1];
        r->rea[i] = r->rea[i] / 100;
      }
    }
    if (r->unk) for (i=0; i<18; i++) r->was[i] = r->rea[i];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_status_panel(void) {

  _r *r = &r_jinf_sta;
  int i;
  
  show_num_box_string(18, r, NULL, "%X", &JINF_STA_BOX[0]);
  
  if (!r->unk) {
    for (i=0; i<18; i++) {
      FL_OBJECT *obj = JINF_STA_BOX[i];
      char b[80];
      q = &Q[i];
      if (q->txt) {
//printf("i = %d, r->rea[i] = %d, q->txt[r->rea[i]] = %08X = %s\n",
//        i,      r->rea[i],      (int32)q->txt[r->rea[i]], q->txt[r->rea[i]]);
        if (q->txt[r->rea[i]]) sprintf(b, q->fmt, q->txt[r->rea[i]]);
        else                   sprintf(b, "?");
      }
      else {
        if (q->m) {
          sprintf(b, q->fmt, r->rea[i]);
        }
        else {
          int32 m = r->rea[i];
          if (m < 1000000) sprintf(b, "%d", m);
          else             sprintf(b, "..%d", m%10000);
        }
      }
      fl_set_object_label(obj, b);
    }
  }
}

//~============================================================================
//
//            TIME PANEL
//
//~============================================================================

void create_time_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 45, w2 = 140, w3 = 20, w4 = 45, h1 = 15;
  _b m_time[5] = {
    {2, -1, 1}, 
    {0, -1, 1},
    {5, -1, 1}, 
    {3, -1, 1},
    {6, -1, 1}};
  
  w = 5 + w1 + 3 * w2 + w3 + w4 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y+0*h1, w1, h1, "",          'S');
  add_frame_box(x, y+1*h1, w1, h1, "Coarse",    'S');
  add_frame_box(x, y+2*h1, w1, h1, "Fine     ", 'S');
  x = x + w1;

  add_frame_box(x, y, w2, h1, "Event Time", 'S');
  create_dat_box_vstring(
    2, x, y+h1, w2, h1, 'N', &r_time, &m_time[0], &TIME_BOX[0]);
  x = x + w2;
  
  add_frame_box(x, y, w2, h1, "Local Time", 'S');
  create_dat_box_vstring(
    2, x, y+h1, w2, h1, 'N', &r_time, &m_time[2], &TIME_BOX[2]);
  x = x + w2;
  
  add_frame_box(x, y, w2, h1, "Calibration", 'S');
  create_dat_box_vstring(
    1, x, y+h1, w2, 2*h1, 'N', &r_time, &m_time[4], &TIME_BOX[4]);
  x = x + w2;

  for (i=0; i<5; i++) {
    fl_set_object_lalign(TIME_BOX[i]->obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  }
  
  add_frame_box(x, y, w3+w4, h1, "", 'S');
  obj = add_normal_button(x, y+h1, w3, 2*h1, "R", 'N');
    fl_set_object_callback(obj, read_time_callback, 0);
  x = x + w3;
  obj = add_normal_button(x, y+h1, w4, 2*h1, "RESET\nTIME", 'N');
    fl_set_object_callback(obj, reset_time_callback, 0);
}

//~----------------------------------------------------------------------------

void read_time_callback(FL_OBJECT* obj, long data) {

  int i;
  int16 request[1000];
  int16 reply[1000];
  _r *r = &r_time;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<7; i++) r->was[i] = r->rea[i];

  begin_IO_segment(request);
  add_IO_write(request, 0x3F, 0x0000);
  add_IO_read(request, 0x04);
  add_IO_read(request, 0x05);
  add_IO_read(request, 0x06);
  add_IO_read(request, 0x07);
  add_IO_read(request, 0x39);
  add_IO_read(request, 0x3A);
  add_IO_read(request, 0x3B);
  add_IO_read(request, 0x3C);
  add_IO_read(request, 0x0B);
  add_IO_read(request, 0x0C);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);

  if (!err) {
    r->rea[0] = (int32)reply[7+4*0] + ((int32)reply[7+4*1] << 16);
    r->rea[1] = (int32)reply[7+4*2] & 0x000000FF;
    r->rea[2] = ((int32)reply[7+4*2] >> 8) + ((int32)reply[7+4*3] << 8);
    r->rea[3] = (int32)reply[7+4*4] + ((int32)reply[7+4*5] << 16);
    r->rea[4] = (int32)reply[7+4*6] & 0x000000FF;
    r->rea[5] = ((int32)reply[7+4*6] >> 8) + ((int32)reply[7+4*7] << 8);
    r->rea[6] = (int32)reply[7+4*8] + ((int32)reply[7+4*9] << 16);
    for (i=0; i<7; i++) if (r->unk) r->was[i] = r->rea[i];
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

void reset_time_callback(FL_OBJECT* obj, long data) {

  int16 request[100];
  int16 reply[100];
    
  msg = -1;
  update_objects();

  begin_IO_segment(request);
  add_IO_write(request, 0x3F, 0x0004);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_time_panel(void) {

  char b[80];
  _r *r = &r_time;
  int32 coarse_time, fine_time_M, fine_time_L, fine_time, delta;
  
  coarse_time = r->rea[2];
  fine_time_M = r->rea[1];
  fine_time_L = r->rea[0];
  fine_time   = 640 * (((int64)fine_time_M << 32) + (int64)fine_time_L);
  sprintf(b, "%'d", coarse_time);
  show_dat_box_string(1, NULL, b, &TIME_BOX[0]);
  sprintf_huge(b, fine_time/1000000000, 9, fine_time%1000000000, 9);
  show_dat_box_string(1, NULL, b, &TIME_BOX[1]);

  coarse_time = r->rea[5];
  fine_time_M = r->rea[4];
  fine_time_L = r->rea[3];
  fine_time   = 640 * (((int64)fine_time_M << 32) + (int64)fine_time_L);
  sprintf(b, "%'d", coarse_time);
  show_dat_box_string(1, NULL, b, &TIME_BOX[2]);
  sprintf_huge(b, fine_time/1000000000, 9, fine_time%1000000000, 9);
  show_dat_box_string(1, NULL, b, &TIME_BOX[3]);

  delta = 640 * r->rea[6];
  sprintf_huge(b, delta/1000000000, 9, delta%1000000000, 9); 
  show_dat_box_string(1, NULL, b, &TIME_BOX[4]);
}

//~============================================================================
//
//            BUSY REGISTERS (MASK, STATUS, ERROR) PANEL
//
//~============================================================================

extern _b busy_reg_msk[3][3][16];

_r *r_busy[3]    = {&r_busy_msk,  &r_busy_sta,  &r_busy_err};

_b *m_busy[3][3] = {
  {&busy_reg_msk_h[0][2][0], &busy_reg_msk_h[0][1][0], &busy_reg_msk_h[0][0][0]},
  {&busy_reg_msk_h[1][2][0], &busy_reg_msk_h[1][1][0], &busy_reg_msk_h[1][0][0]},
  {&busy_reg_msk_h[2][2][0], &busy_reg_msk_h[2][1][0], &busy_reg_msk_h[2][0][0]}};

DAT_BOX **BUSY_BOX[3][3] = {
  {&BUSY_MSK_BOX[2][0], &BUSY_MSK_BOX[1][0], &BUSY_MSK_BOX[0][0]},
  {&BUSY_STA_BOX[2][0], &BUSY_STA_BOX[1][0], &BUSY_STA_BOX[0][0]},
  {&BUSY_ERR_BOX[2][0], &BUSY_ERR_BOX[1][0], &BUSY_ERR_BOX[0][0]}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_busy_registers_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h, x1;
  int w1 = 40, w2 = 15, w3 = 20, h1 = 15;
     
  char *txt1[3] = {"Busy\nMask", "Busy\nStatus", "Busy\nError"};

  char *txt2[20] = {
    "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", 
    "09", "08", "07", "06", "05", "04", "03", "02", "01", "00"};
  char *txt3[20] = {
    "T7", "S3", "T6", "T4", "T5", "S2", "E1", "E0", "R1", "R0",
    "  ", "U0", "S0", "  ", "S1", "T1", "T0", "U1", "T3", "T2"};
  char *txt4[40] = {
    "B", "A", "B", "A", "B", "A", "B", "A", "B", "A", 
    "B", "A", "B", "A", "B", "A", "B", "A", "B", "A", 
    "B", "A", "B", "A", "B", "A", "B", "A", "B", "A", 
    "B", "A", "B", "A", "B", "A", "B", "A", "B", "A"};
  
  char *txt5[3][8] = {
    {" ", " ", "S\ne\nq", " ", " ", " ",    " ", " "},
    {" ", "D\nS\nP", " ", "S\ne\nq", " ", "F\nE", "A\nl\nl", "E\nr\nr"},
    {" ", " ", " ", " ", " ", " ",    " ", "E\nr\nr"}};

  w = 5 + w1 + 48 * w2 + w3 + 5;
  h = 5 + 5 * h1 + 5 + 4 * h1 + 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  y = y0 + 5;

  for (i=0; i<3; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, i ? 4*h1 : 5*h1, txt1[i], 'S');
    if (i == 0) {
      obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w1, 5*h1, "");
        fl_set_object_callback(obj, reset_busy_mask_callback, 0);
    }
    x = x + w1;
    x1 = x;
    create_frame_box_hstring(20, x, y,    2*w2, h1, txt2,        'S', 'C');
    create_frame_box_hstring(20, x, y+h1, 2*w2, h1, txt3,        'S', 'C');
    x = x + 20 * 2 * w2;
    create_frame_box_hstring( 8, x, y,    w2, 3*h1, &txt5[i][0], 'S', 'C');
    if (i == 0) {
      add_frame_box(x+6*w2, y, 2*w2, 3*h1, "TR\nST\n1  0", 'S');
    }
    add_frame_box(x+8*w2, y, w3, 3*h1, "", 'S');
    y = y + 2 * h1;
    x = x1;
    create_frame_box_hstring(40, x, y, w2,   h1, txt4,        'S', 'C');
    y = y + h1;

    if (i == 0) {
      for (j=0; j<3; j++) {
        create_btn_hstring(
          16, x, y, w2, h1, __m, 'N', &ww_busy_msk, m_busy[i][j], &BUSY_MSK_BTN[j][0]);
        x = x + 16 * w2;
      }
      obj = add_normal_button(x, y, w3, h1, "W", 'N');
        fl_set_object_callback(obj, write_busy_register_callback, 0);
      y = y + h1;
    }

    x = x1;
    for (j=0; j<3; j++) {
      create_dat_box_hstring(
        16, x, y, w2, h1, 'N', r_busy[i], m_busy[i][j], BUSY_BOX[i][j]);
      x = x + 16 * w2;
    }
    obj = add_normal_button(x, y, w3, h1, "R", 'N');
      fl_set_object_callback(obj, read_busy_register_callback, i);
    y = y + h1 + 5;
  }
}

//~----------------------------------------------------------------------------

void reset_busy_mask_callback(FL_OBJECT* obj, long data) {

  int i;
  static int set = 0;

  for (i=0; i<3; i++) {
    _ww *w = &ww_reg;
    int32 m = i != 2 ? 0xFFFF : 0xFF00;
    switch (set) {
      case 0:
        pack32(&w->inp[m_busy[0][i]->ind], m, 0x0000);
      break;
      case 1:
        pack32(&w->inp[m_busy[0][i]->ind], m, 0xFFFF);
      break;
      case 2:
        pack32(&w->inp[m_busy[0][i]->ind], m, 0xAAAA);
      break;
      case 3:
        pack32(&w->inp[m_busy[0][i]->ind], m, 0x5555);
      break;
    }
  }
  set = MOD(set + 1, 4);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_busy_register_callback(FL_OBJECT* obj, long data) {

  int i = data;
  int j;
  int16 request[1000];
  int16 reply[1000];
  _r  *r  = &r_busy_msk;
  _ww *ww = &ww_busy_msk;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  begin_IO_segment(request);
  for (j=0; j<3; j++) {
    int ind = m_busy[i][j]->ind;
    add_IO_write(request, ind, ww->inp[ind]);
  }
  end_IO_segment(request);

  perform_IO_operation(request, NULL, reply, &err);

  if (!err) {
    for (j=0; j<3; j++) {
      int ind = m_busy[i][j]->ind;
      ww->wri[ind] = ww->inp[ind];
    }
    ww->unk = FALSE;
  }
  else {
    ww->unk = TRUE;
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_busy_register_callback(FL_OBJECT* obj, long data) {

  int i = data;
  int j;
  int16 request[1000];
  int16 reply[1000];
  _r *r = r_busy[i];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (j=0; j<3; j++) {
    int ind = m_busy[i][j]->ind;
    r->was[ind] = r->rea[ind];
  }

  begin_IO_segment(request);
  for (j=0; j<3; j++) {
    int ind = m_busy[i][j]->ind;
    add_IO_read(request, ind);
  }
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);

  if (!err) {
    for (j=0; j<3; j++) {
      int ind = m_busy[i][j]->ind;
      r->rea[ind] = reply[3+4*j];
      if (r->unk) r->was[ind] = r->rea[ind];
    }
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

void update_busy_registers_panel(void) {

  int i;
  
  for (i=0; i<3; i++) {
    show_btn_string(16, NULL, BUSY_MSK_BTN[i]);
    show_dat_box_string(16, __m, NULL, BUSY_BOX[0][i]);
    show_dat_box_string(16, hex, NULL, BUSY_BOX[1][i]);
    show_dat_box_string(16, e__, NULL, BUSY_BOX[2][i]);
  }
}

//~============================================================================
//
//            SCALERS PANEL
//
//~============================================================================

extern struct scalers scaler[];

_b gate_msk = {0x00, 0x3000, 4};
_b scaler_msk[0x60];

int ind1[9]  = {0x00, 0x01, 0x02, 0x03, 0x48, 0x49, 0x4A, 0x4B, 0x05};
int ind2[8]  = {0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D};
int ind3a[8] = {0x20, 0x21, 0x23, 0x22, 0x24, 0x25, 0x26, 0x27}; //22 & 23 swapped
int ind3b[8] = {0x28, 0x29, 0x2B, 0x2A, 0x2C, 0x2D, 0x2E, 0x2F}; //2A & 2B swapped
int ind3c[8] = {0x30, 0x31, 0x33, 0x32, 0x34, 0x35, 0x36, 0x37}; //32 & 33 swapped
int ind4a[8] = {0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};
int ind4b[8] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47};
int ind5[8]  = {0x0E, 0x0F, 0x10, 0x11, 0x12, 0x04, 0x51, 0x53};

char *gates[4] = {"0.25", "0.50", "1.00", "2.00"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_scalers_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, k;
  int x, y, w, h, y1;
  int w0 = 45, w1 = 30, w2 = 15, w3 = 25, w4 = 25, w5 = 35, w6 = 45, w7 = 35;
  int h0 = 15, h1 = 30;
  int wb = 80, hb = 15;
     
  w = 5 + w1 + w0 + 5 + w2 + w0 + 5 + w3 + 3 * w0 + 5 + w4 + 2 * w0 + 5 + w5 + w0 + 5;
  h = 5 + h1 + 9 * h0 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Rates from Scalers, count/sec", 'L');

  for (i=0; i<0x60; i++) {
    scaler_msk[i].ind = i;
    scaler_msk[i].msk = 0x3FFFF;
    scaler_msk[i].n   = -1;
    SCALER_BOX[i] = NULL;
  }
  
//~---

  x = x0 + w - 5 - w7 - w6;
  y = y0 + 5;
  add_frame_box(x, y, w6, h0, "Gate, s", 'S');
  x = x + w6;
  create_dat_box_hstring(
    1, x, y, w7, h0, 'N', &r_gate, &gate_msk, &GATE_BOX);
  y = y + h1;

//~---

  k = 0;
  
  x = x0 + 5;
  for (i=0; i<sizeof(ind1)/sizeof(ind1[0]); i++) {
    y1 = y + i * h0;
    add_frame_box(x, y1, w1, h0, scaler[ind1[i]].nam, 'S');
    create_dat_box_hstring(
      1, x+w1, y1, w0, h0, 'N', &r_scaler, &scaler_msk[ind1[i]], &SCALER_BOX[k++]);
  }
  x = x + w1 + w0 + 5;

  add_frame_box(x, y, w2+w0, h0, "Sub Trigg.", 'S');
  for (i=0; i<sizeof(ind2)/sizeof(ind2[0]); i++) {
    y1 = h0 + y + i * h0;
    add_frame_box(x, y1, w2, h0, scaler[ind2[i]].nam, 'S');
    create_dat_box_hstring(
      1, x+w2, y1, w0, h0, 'N', &r_scaler, &scaler_msk[ind2[i]], &SCALER_BOX[k++]);
  }
  x = x + w2 + w0 + 5;

  add_frame_box(x,         y, w3, h0, "",   'S');
  add_frame_box(x+w3,      y, w0, h0, "CP", 'S');
  add_frame_box(x+w3+w0,   y, w0, h0, "CT", 'S');
  add_frame_box(x+w3+2*w0, y, w0, h0, "BZ", 'S');
  for (i=0; i<sizeof(ind3a)/sizeof(ind3a[0]); i++) {
    if (strcmp(scaler[ind3a[i]].nam, scaler[ind3b[i]].nam) ||
        strcmp(scaler[ind3a[i]].nam, scaler[ind3c[i]].nam) ||
        strcmp(scaler[ind3b[i]].nam, scaler[ind3c[i]].nam)) {
      printf("Scaler Names Error: %s %s %s\n",
             scaler[ind3a[i]].nam, scaler[ind3b[i]].nam, scaler[ind3c[i]].nam);
      exit(1);
    }
    y1 = h0 + y + i * h0;
    add_frame_box(x, y1, w3, h0, scaler[ind3a[i]].nam, 'S');
    create_dat_box_hstring(
      1, x+w3,      y1, w0, h0, 'N', &r_scaler, &scaler_msk[ind3a[i]], &SCALER_BOX[k++]);
    create_dat_box_hstring(
      1, x+w3+w0,   y1, w0, h0, 'N', &r_scaler, &scaler_msk[ind3b[i]], &SCALER_BOX[k++]);
    create_dat_box_hstring(
      1, x+w3+2*w0, y1, w0, h0, 'N', &r_scaler, &scaler_msk[ind3c[i]], &SCALER_BOX[k++]);
  }
  x = x + w3 + 3 * w0 + 5;

  add_frame_box(x,         y, w4, h0, "",    'S');
  add_frame_box(x+w4,      y, w0, h0, "Top", 'S');
  add_frame_box(x+w4+w0,   y, w0, h0, "Bot", 'S');
  for (i=0; i<sizeof(ind4a)/sizeof(ind4a[0]); i++) {
    if (strcmp(scaler[ind4a[i]].nam, scaler[ind4b[i]].nam)) {
      printf("Scaler Names Error: %s %s\n",
             scaler[ind4a[i]].nam, scaler[ind4b[i]].nam);
      exit(1);
    }
    y1 = h0 + y + i * h0;
    add_frame_box(x, y1, w4, h0, scaler[ind4a[i]].nam, 'S');
    create_dat_box_hstring(
      1, x+w4,      y1, w0, h0, 'N', &r_scaler, &scaler_msk[ind4a[i]], &SCALER_BOX[k++]);
    create_dat_box_hstring(
      1, x+w4+w0,   y1, w0, h0, 'N', &r_scaler, &scaler_msk[ind4b[i]], &SCALER_BOX[k++]);
  }
  x = x + w4 + 2 * w0 + 5;

  for (i=0; i<sizeof(ind5)/sizeof(ind5[0]); i++) {
    if (i <  6) add_frame_box(x, y, w5,   h0, scaler[ind5[i]].nam, 'S');
    if (i == 6) add_frame_box(x, y, w5, 2*h0, scaler[ind5[i]].nam, 'S');
    create_dat_box_hstring(
      1, x+w5, y, w0, h0, 'N', &r_scaler, &scaler_msk[ind5[i]], &SCALER_BOX[k++]);
    y = y + h0;
  }

  obj = add_normal_button(x, y, wb, hb, "READ  ALL", 'N');
    fl_set_object_callback(obj, read_scalers_callback, 0);
}

//~----------------------------------------------------------------------------

void read_scalers_callback(FL_OBJECT* obj, long data) {

  int i;
  int16 request[1000];
  int16 reply[1000];
  int16 dat[0x60+2];
  float gate = 0.0;
  int p0, t0, l0, p1, t1, l1;
  int n = sizeof(dat) / sizeof(dat[0]);
  _r *g = &r_gate;
  _r *s = &r_scaler;

  if (obj) {
    msg = -1;
    update_objects();
  }

  g->was[0] = g->rea[0];
  for (i=0; i<n; i++) s->was[i] = s->rea[i];

  err = 0x0000;
  
  begin_IO_segment(request);
  add_IO_read(request, 0x37);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);

  if (!err) {
    g->rea[0] = reply[3];
    gate = atof(gates[unpack16(g->rea[0], gate_msk.msk)]);
    if (g->unk) g->was[0] = g->rea[0];
    g->unk = FALSE;
    g->sta = status_OK;
  }
  else {
    g->sta = status_KO;
  }
  
  if (!err) read_JLV1_scalers(dat, &err);

  if (!err) {
    for (i=0; i<n; i++) {
      if (i<0x4C) s->rea[i] = (float)dat[i] / gate;
      else        s->rea[i] =        dat[i];
    }
//printf("%04X %04X %04X %04X \n", r->rea[0x50], r->rea[0x51], r->rea[0x52], r->rea[0x53]);
    p0 = unpack16(s->rea[0x50], 0x3000);
    t0 = ((int32)unpack16(s->rea[0x50], 0x07FF) << 16) + s->rea[0x51];
    l0 = (float)t0 * 0.000000020 / atof(gates[p0]) * 100.0 + 0.5;
    p1 = unpack16(s->rea[0x52], 0x3000);
    t1 = ((int32)unpack16(s->rea[0x52], 0x07FF) << 16) + s->rea[53];
    l1 = (float)t1 * 0.000000020 / atof(gates[p1]) * 100.0 + 0.5;
//printf("p0 = %d, period0 = %4.2f, t0 = %8d, l0 = %3d \n", p0, period[p0], t0, l0);
//printf("p1 = %d, period1 = %4.2f, t1 = %8d, l1 = %3d \n", p1, period[p1], t1, l1);
    s->rea[0x50] = p0;
    s->rea[0x51] = l0;
    s->rea[0x52] = p1;
    s->rea[0x53] = l1;
//printf("%d %2d %d %2d  \n", r->rea[0x50], r->rea[0x51], r->rea[0x52], r->rea[0x53]);
    for (i=0; i<n; i++) if (s->unk) s->was[i] = s->rea[i];
    s->unk = FALSE;
    s->sta = status_OK;
  }
  else {
    s->sta = status_KO;
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_scalers_panel(void) {

  int i;
  
  show_dat_box_string(1, gates, NULL, &GATE_BOX);

  for (i=0; i<0x60; i++) {
    DAT_BOX *B = SCALER_BOX[i];
    if (B) {
      if (i < 63) show_dat_box_string(1, NULL, "%5d", &B);     // <-- very bad
      else        show_dat_box_string(1, NULL, "%3d %%", &B);  // <-- very bad
    }
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_global_trigger_control_panel();
  update_boot_panel();
  update_load_boot_panel();

  update_trigger_generator_panel();
  if (!use_TCP) update_EPPCAN_box_panel();

  update_ping_panel();
  update_proc_mode_panel();
  update_event_nb_panel();
  update_get_event_panel();

  update_status_panel();

  update_time_panel();

  update_busy_registers_panel();

  update_scalers_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
