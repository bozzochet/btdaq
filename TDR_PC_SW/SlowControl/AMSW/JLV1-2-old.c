// file JLV1-2-old.c
//
// JLV1-2 (OLD) "Pro" Controller
//
// A.Lebedev Sep-2006...

#include "jlv1def-old.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"JLV1-2 (OLD):  Controller"};         // global variable
char *date  = {"23-Oct-08"};                         // global variable

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
_ww ww_glob_trg;

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

//~-- TOF Look Up Tables Panel

SW_BTN  *LOOK_UP_BTN[3][16];
DAT_BOX *LOOK_UP_BOX[3][16];

//~-- Anti-Coincidence Panel

SW_BTN  *ACC_MSK_BTN[2][8];
SW_BTN  *ACC_FUN_BTN[8];
SW_BTN  *ACC_THR_BTN[2];
DAT_BOX *ACC_MSK_BOX[2][8];
DAT_BOX *ACC_FUN_BOX[8];
DAT_BOX *ACC_THR_BOX[2];

//~-- Fast Trigger Setup Panel

SW_BTN  *TOF_MSK_BTN[2][12];
SW_BTN  *TOF_FUN_BTN[12];
DAT_BOX *TOF_MSK_BOX[2][12];
DAT_BOX *TOF_FUN_BOX[12];

SW_BTN  *ECAL_MSK_BTN[2][1];
SW_BTN  *ECAL_FUN_BTN[1];
DAT_BOX *ECAL_MSK_BOX[2][1];
DAT_BOX *ECAL_FUN_BOX[1];

SW_BTN  *FTC_SEL_BTN[2];
DAT_BOX *FTC_SEL_BOX[2];

SW_BTN  *BZ_FUN_BTN[3];
SW_BTN  *BZ_WID_BTN[2];
DAT_BOX *BZ_FUN_BOX[3];
DAT_BOX *BZ_WID_BOX[2];

SW_BTN  *FT_MSK_BTN[3];
DAT_BOX *FT_MSK_BOX[3];

//~-- "Mask More" Panel

SW_BTN  *MORE_MSK_BTN[8];
DAT_BOX *MORE_MSK_BOX[8];

//~-- Output Setup Panel

SW_BTN  *OUT_SIG_BTN[2], *OUT_MSK_BTN[5];
DAT_BOX *OUT_SIG_BOX[2], *OUT_MSK_BOX[5];

//~-- Level-1 Trigger Setup Panel

SW_BTN  *LV1_MSK_BTN[9][16];
DAT_BOX *LV1_MSK_BOX[9][16];

//~-- PreScalers Panel

SW_BTN  *LV1_PRE_BTN[8][4];
DAT_BOX *LV1_PRE_BOX[8];

//~-- Trigger & Input Patterns Panel

DAT_BOX *TRG_SIG_BOX[2][16];
DAT_BOX *INP_SIG_BOX[2][16];

//~-- Configuration File Panel

SW_BTN  *CFG_FILE_BTN;
_ww w_cfg_file;
char config_file_name[80];

//~-- Busy Registers (Mask, Status, Errors)

SW_BTN  *BUSY_MSK_BTN[2][3][16];
DAT_BOX *BUSY_MSK_BOX[2][3][16];
DAT_BOX *BUSY_STA_BOX[2][3][16];
DAT_BOX *BUSY_ERR_BOX[2][3][16];
int busy_msk_layout = 0;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
char *dec_dot[10] = {
"0.", "1.", "2.", "3.", "4.", "5.", "6.", "7.", "8.", "9."};
char *five[10] = {
"0", NULL, NULL, NULL, NULL, "5", NULL, NULL, NULL, NULL};
char *wid[32];

char *mod[4] = {"Off", "Raw", "Comp", "Raw+Comp"};
char *got[2] = {"Empty", "Got It"};
char *o_o[2] = {"OFF", "ON"};
char *e_d[2] = {"E", "D"};
char *d_e[2] = {"D", "E"};
char *__a[2] = {" ", "A"};
char *__m[2] = {" ", "M"};
char *__s[2] = {" ", "S"};
char *e__[2] = {"E", " "};
char *a_o[2] = {"AND", "OR"};
char *fai[2] = {"FAIL", "O.K."};
char *ft_lv1[2] = {"FT", "LV1"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_config_file(char *filename);
bool write_config_file(char *filename);

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

void create_TOF_look_up_panel(int x0, int y0);
void update_TOF_look_up_panel(void);

void create_ACC_panel(int x0, int y0);
void update_ACC_panel(void);

void create_FT_setup_panel(int x0, int y0);
void update_FT_setup_panel(void);

void create_mask_more_panel(int x0, int y0);
void update_mask_more_panel(void);

void create_output_setup_panel(int x0, int y0);
void update_output_setup_panel(void);

//~--

void create_LV1_setup_panel(int x0, int y0);
void update_LV1_setup_panel(void);

void create_prescalers_panel(int x0, int y0);
void convert_prescaler_code_to_value(int32 code, int32 *value);
void convert_prescaler_value_to_code(int32 value, int32 *code);
void update_prescalers_panel(void);

void create_signal_pattern_panel(int x0, int y0);
void update_signal_pattern_panel(void);

void create_config_file_panel(int x0, int y0);
void write_config_file_callback(FL_OBJECT* obj, long data);
void update_config_file_panel(void);

//~--

void create_busy_registers_panel(int x0, int y0, int ind);
void change_busy_registers_panel_layout_callback(FL_OBJECT* obj, long data);
void reset_busy_mask_callback(FL_OBJECT* obj, long data);
void update_busy_registers_panel(void);

//~--

void create_read_write_update_panel(int x0, int y0);
void read_all_callback(FL_OBJECT* obj, long data);
void write_all_callback(FL_OBJECT* obj, long data);
void update_input_callback(FL_OBJECT* obj, long data);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 1035, HH = 880;

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

  create_TOF_look_up_panel(   0, 195);
  create_ACC_panel(         180, 195);
  create_FT_setup_panel(    335, 195);
  create_mask_more_panel(   730, 195);
  create_output_setup_panel(730, 375);
  
  create_LV1_setup_panel(       0, 505);
  create_prescalers_panel(    360, 505);
  create_signal_pattern_panel(480, 505);

  create_config_file_panel(755, 535);

  create_busy_registers_panel(870, 60, 0);
  create_busy_registers_panel(870, 60, 1);

  create_read_write_update_panel(755, 715);

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

  invalidate_r(first, &r_file_header, 8);
  
  invalidate_dependant_r(&r_int_trg);
  invalidate_dependant_ww(&ww_int_trg);

  invalidate_dependant_r(&r_glob_trg);
  invalidate_dependant_ww(&ww_glob_trg);

  invalidate_r(first, &r_epp_busy_sta, 1);
  invalidate_w(&w_epp_trig_msk, 1, 0x0000);

  invalidate_r(first, &r_ping_sta, 1);

  invalidate_r(first, &r_proc_mode, 1);
  invalidate_w(&w_proc_mode, 1, 0x0000);

  invalidate_r(first, &r_event_nb, 1);

  invalidate_r(first, &r_get_event, 1);
  
  invalidate_r(first, &r_jinf_sta, 18);

  w_cfg_file.unk = TRUE;
  if (first) w_cfg_file.inp = malloc(4);
  if (first) w_cfg_file.wri = malloc(4);
  w_cfg_file.inp[0] = 0;

  read_config_file(dat_config_file_name);
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

bool read_config_file(char *filename) {

  FILE *file;
  
  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&w_boot_adr.inp,     4,     1, file);
    fread(&w_proc_mode.inp,    4,     1, file);
    fread(&w_epp_trig_msk.inp, 4,     1, file);
    fread(ww_reg.inp,          4, n_reg, file);
    fread(w_cfg_file.inp,      4,     1, file);
    fread(&busy_msk_layout,    4,     1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(&w_boot_adr.inp,     4,     1, file);
    fwrite(&w_proc_mode.inp,    4,     1, file);
    fwrite(&w_epp_trig_msk.inp, 4,     1, file);
    fwrite(ww_reg.inp,          4, n_reg, file);
    fwrite(w_cfg_file.inp,      4,     1, file);
    fwrite(&busy_msk_layout,    4,     1, file);
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
  _ww *w = &ww_glob_trg;
  int ind = glob_trg_msk.ind;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;
  w->inp[ind] = msk;

  begin_IO_segment(request);
  add_IO_write(request, ind, msk);
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
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_CYAN, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_get_event = TRUE;
        fl_add_timeout(0, auto_get_event_callback, NULL);
        fl_set_object_boxtype(obj, FL_DOWN_BOX);
        fl_set_object_color(obj, FL_RED, FL_RED);
        fl_set_object_lcolor(obj, FL_YELLOW);
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
//            TOF LOOK UP TABLES PANEL
//
//~============================================================================

void create_TOF_look_up_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 20, w2 = 15, h1 = 30, h2 = 15;
  int size;

  extern _b lut_msk[3][16];
  
  char *txt[7] = {"S4", "S3", "S2", "S1", "FT0", "FT1", "BZ"};
  
  w = 5 + 4 * w1 + 3 * 2 * w2 + 5;
  h = 5 + h1 + 2 * h2 + 16 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "TOF LUTs", size);
  y = y + h1;
  
  size = FL_SMALL_SIZE;

  for (i=0; i<7; i++) {
    w = i < 4 ? w1 : 2 * w2;
    h = i < 4 ? 2 * h2 : h2;
    add_frame_box(x, y, w, h, txt[i], size);
    if (i > 3) {
      add_frame_box(x,    y+h2, w2, h2, "W", size);
      add_frame_box(x+w2, y+h2, w2, h2, "R", size);
    }
    x = x + w;
  }
  y = y + 2 * h2;

  x = x0 + 5;
  for (i=0; i<16; i++) {
    int j;
    for (j=0; j<4; j++) {
      add_frame_box(x+j*w1, y+i*h2, w1, h2, i & 8>>j ? "1":"0", size);
    }
  }
  x = x + 4 * w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<3; i++) {
    create_btn_vstring(
      16, x, y, w2, h2, __a, size, &ww_reg, &lut_msk[i][0], &LOOK_UP_BTN[i][0]);
    x = x + w2;
    create_dat_box_vstring(
      16, x, y, w2, h2,      size, &r_reg,  &lut_msk[i][0], &LOOK_UP_BOX[i][0]);
    x = x + w2;
  }
}

//~----------------------------------------------------------------------------

void update_TOF_look_up_panel(void) {

  int i;
  
  for (i=0; i<3; i++) {
    show_btn_string(    16,      NULL, &LOOK_UP_BTN[i][0]);
    show_dat_box_string(16, __a, NULL, &LOOK_UP_BOX[i][0]);
  }
}

//~============================================================================
//
//            ANTI-COINCIDENCE PANEL
//
//~============================================================================

void create_ACC_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 25, w2 = 15, w3 = 30, w4 = 40, h1 = 30, h2 = 15;
  int size;

  extern _b acc_msk[2][8];
  extern _b acc_fun[8];
  extern _b acc_thr[2];

//~--
  
  char *txt1[4] = {"", "Top", "Bot", "Function"};
  char *txt2[6] = {"ACC0", "W", "R", "ACC1", "W", "R"};
  
  w = 5 + w1 + 2 * 2 * w2 + 2 * w3 + 5;
  h = 5 + h1 + 2 * h2 + 8 * h2 + 5 + h2 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "ACC", size);
  y = y + h1;
  
//~--

  size = FL_SMALL_SIZE;

  for (i=0; i<4; i++) {
    int w = i < 1 ? w1 : (i < 3 ? 2 * w2 : 2 * w3);
    int h = i < 1 ? 2 * h2 : h2;
    add_frame_box(x, y, w, h, txt1[i], size);
    if (i > 0) {
      int w = i < 3 ? w2 : w3;
      add_frame_box(x,   y+h2, w, h2, "W", size);
      add_frame_box(x+w, y+h2, w, h2, "R", size);
    }
    x = x + w;
  }
  y = y + 2 * h2;

  x = x0 + 5;
  for (i=0; i<8; i++) {
    char b[80];
    sprintf(b, "A%d", i);
    add_frame_box(x, y+i*h2, w1, h2, b, size);
  }
  x = x + w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<2; i++) {
    create_btn_vstring(
      8, x, y, w2, h2, e_d, size, &ww_reg, &acc_msk[i][0], &ACC_MSK_BTN[i][0]);
    x = x + w2;
    create_dat_box_vstring(
      8, x, y, w2, h2,      size, &r_reg, &acc_msk[i][0],  &ACC_MSK_BOX[i][0]);
    x = x + w2;
  }

  create_btn_vstring(
    8, x, y, w3, h2, a_o, size, &ww_reg, &acc_fun[0], &ACC_FUN_BTN[0]);
  x = x + w3;
  create_dat_box_vstring(
    8, x, y, w3, h2,      size, &r_reg, &acc_fun[0],  &ACC_FUN_BOX[0]);
  y = y + 8 * h2 + 5;

//~--

  size = FL_SMALL_SIZE;

  x = x0 + 5;
  add_frame_box(x, y, w-10, h2, "Thresholds", size);
  y = y + h2;

  for (i=0; i<6; i++) {
    int w = i == 0 || i == 3 ? w4 : w2;
    int h = i == 0 || i == 3 ? 2 * h2 : h2;
    add_frame_box(x, y, w, h, txt2[i], size);
    x = x + w;
    if (i == 2) x = x + 5;
  }
  y = y + h2;
  
  size = FL_NORMAL_SIZE;

  x = x0 + 5 + w4;
  
  for (i=0; i<2; i++) {
    create_btn_vstring(
      1, x, y, w2, h2, hex, size, &ww_reg, &acc_thr[i], &ACC_THR_BTN[i]);
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w2, h2,      size, &r_reg, &acc_thr[i],  &ACC_THR_BOX[i]);
    x = x + w2;
    x = x + 5 + w4;
  }
}

//~----------------------------------------------------------------------------

void update_ACC_panel(void) {

  int i;
  
  for (i=0; i<2; i++) {
    show_btn_string(    8,      NULL, &ACC_MSK_BTN[i][0]);
    show_dat_box_string(8, e_d, NULL, &ACC_MSK_BOX[i][0]);
  }
  show_btn_string(      8,      NULL, &ACC_FUN_BTN[0]);
  show_dat_box_string(  8, a_o, NULL, &ACC_FUN_BOX[0]);
  
  for (i=0; i<2; i++) {
    show_btn_string(    1,      NULL, &ACC_THR_BTN[i]);
    show_dat_box_string(1, hex, NULL, &ACC_THR_BOX[i]);
  }
}

//~============================================================================
//
//            FAST TRIGGER SETUP PANEL
//
//~============================================================================

void create_FT_setup_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 50, w2 = 15, w3 = 30, h1 = 30, h2 = 15;
  int size;

  extern _b tof_msk[2][12];
  extern _b tof_fun[12];
  extern _b ftc_sel[1];
  extern _b bz_fun[3];
  extern _b bz_wid[2];
  extern _b ecal_msk[2][1];
  extern _b ecal_fun[1];
  extern _b ft_msk[3];

  char *txt1[4] = {"TOF", "P", "N", "Function"};
  char *txt2[4] = {"ECAL", "X", "Y", "Function"};
  char *txt3[4] = {"Selection", "Width, ns", "Function", "Sel"};
  static char txt[32][6];

//~--

  w = 5 + w1 + 2 * 2 * w2 + 2 * w3 + 5 + 3 * 2 * w3 + 2 * w2 + 5;
  h = 5 + h1 + 2 * h2 + 3 * (4 * h2 + 5) + 2 * h2 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "Fast Trigger Setup", size);
  y = y + h1;
  
//~--

  for (i=0; i<4; i++) {
    int w = i < 1 ? w1 : (i < 3 ? 2 * w2 : 2 * w3);
    int h = i < 1 ? 2 * h2 : h2;
    size = i ? FL_SMALL_SIZE : FL_MEDIUM_SIZE;
    add_frame_box(x, y, w, h, txt1[i], size);
    if (i > 0) {
      add_frame_box(x,     y+h2, w/2, h2, "W", size);
      add_frame_box(x+w/2, y+h2, w/2, h2, "R", size);
    }
    x = x + w;
  }
  y = y + 2 * h2;

  x = x0 + 5;
  for (i=0; i<12; i++) {
    char b[80];
    sprintf(b, "%s S%d", i<4 ? "CP" : i<8 ? "CT" : "BZ", i%4+1);
    add_frame_box(x, y, w1, h2, b, size);
    y = y + h2 + (i == 3 || i == 7 ? 5 : 0);
  }
  x = x + w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<2; i++) {
    y = y0 + 5 + h1 + 2 * h2;
    create_btn_vstring(
      12, x, y, w2, h2, e_d, size+400, &ww_reg, &tof_msk[i][0], &TOF_MSK_BTN[i][0]);
    x = x + w2;
    y = y0 + 5 + h1 + 2 * h2;
    create_dat_box_vstring(
      12, x, y, w2, h2, size+400, &r_reg, &tof_msk[i][0],       &TOF_MSK_BOX[i][0]);
    x = x + w2;
  }

  y = y0 + 5 + h1 + 2 * h2;
  create_btn_vstring(
    12, x, y, w3, h2, a_o, size+400, &ww_reg, &tof_fun[0], &TOF_FUN_BTN[0]);
  x = x + w3;
  y = y0 + 5 + h1 + 2 * h2;
  create_dat_box_vstring(
    12, x, y, w3, h2, size+400, &r_reg, &tof_fun[0],       &TOF_FUN_BOX[0]);

//~--

  x = x0 + 5;
  y = y0 + 5 + h1 + 2 * h2 + 3 * (4 * h2 + 5);
  for (i=0; i<4; i++) {
    int w = i < 1 ? w1 : (i < 3 ? 2 * w2 : 2 * w3);
    int h = i < 1 ? 2 * h2 : h2;
    size = i ? FL_SMALL_SIZE : FL_MEDIUM_SIZE;
    add_frame_box(x, y, w, h, txt2[i], size);
    if (i > 0) {
      add_frame_box(x,     y+h2, w/2, h2, "W", size);
      add_frame_box(x+w/2, y+h2, w/2, h2, "R", size);
    }
    x = x + w;
  }
  y = y + 2 * h2;

  x = x0 + 5;
  add_frame_box(x, y, w1, h2, "ECAL-F", size);
  x = x + w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<2; i++) {
    create_btn_vstring(
      1, x, y, w2, h2, e_d, size+400, &ww_reg, &ecal_msk[i][0], &ECAL_MSK_BTN[i][0]);
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w2, h2, size+400, &r_reg, &ecal_msk[i][0],       &ECAL_MSK_BOX[i][0]);
    x = x + w2;
  }

  create_btn_vstring(
    1, x, y, w3, h2, a_o, size+400, &ww_reg, &ecal_fun[0], &ECAL_FUN_BTN[0]);
  x = x + w3;
  create_dat_box_vstring(
    1, x, y, w3, h2, size+400, &r_reg, &ecal_fun[0],       &ECAL_FUN_BOX[0]);

//~--

  size = FL_SMALL_SIZE;
  
  x = x0 + 5 + w1 + 2 * 2 * w2 + 2 * w3 + 5;
  y = y0 + 5 + h1;
  for (i=0; i<4; i++) {
    int w = i < 3 ? 2 * w3 : 2 * w2;
    add_frame_box(x, y, w, h2, txt3[i], size);
    add_frame_box(x,     y+h2, w/2, h2, "W", size);
    add_frame_box(x+w/2, y+h2, w/2, h2, "R", size);
    x = x + w;
  }

  size = FL_NORMAL_SIZE;

  x = x0 + 5 + w1 + 2 * 2 * w2 + 2 * w3 + 5;
  y = y0 + 5 + h1 + 2 * h2;
  create_btn_vstring(
    1, x,    y, w3, 4*h2, e_d, size, &ww_reg, &ftc_sel[0], &FTC_SEL_BTN[0]);
  create_dat_box_vstring(
    1, x+w3, y, w3, 4*h2,      size, &r_reg,  &ftc_sel[0], &FTC_SEL_BOX[0]);
  y = y + 4 * h2 + 5;
  create_btn_vstring(
    1, x,    y, w3, 4*h2, d_e, size, &ww_reg, &ftc_sel[0], &FTC_SEL_BTN[1]);
  create_dat_box_vstring(
    1, x+w3, y, w3, 4*h2,      size, &r_reg,  &ftc_sel[0], &FTC_SEL_BOX[1]);
  x = x + 2 * w3;

  y = y0 + 5 + h1 + 2 * h2;
  add_frame_box(x, y, 2*2*w3, 4*h2+5+4*h2, "", size);
  x = x + 2 * 2 * w3;

  create_btn_vstring(
    1, x,    y, w2, 4*h2+5+4*h2, e_d, size, &ww_reg, &ft_msk[0], &FT_MSK_BTN[0]);
  create_dat_box_vstring(
    1, x+w2, y, w2, 4*h2+5+4*h2,      size, &r_reg,  &ft_msk[0], &FT_MSK_BOX[0]);

  x = x0 + 5 + w1 + 2 * 2 * w2 + 2 * w3 + 5;
  y = y0 + 5 + h1 + 2 * h2 + 2 * (4 * h2 + 5);
  create_btn_vstring(
    2, x,    y, w3, 2*h2, a_o, size, &ww_reg, &bz_fun[0], &BZ_FUN_BTN[0]);
  create_dat_box_vstring(
    2, x+w3, y, w3, 2*h2,      size, &r_reg,  &bz_fun[0], &BZ_FUN_BOX[0]);
  x = x + 2 * w3;

  for (i=0; i<32; i++) sprintf(txt[i], "%d", 20 * (i + 1));
  for (i=0; i<32; i++) wid[i] = &txt[i][0];
  create_btn_vstring(
    2, x,    y, w3, 2*h2, wid, size, &ww_reg, &bz_wid[0], &BZ_WID_BTN[0]);
  create_dat_box_vstring(
    2, x+w3, y, w3, 2*h2,      size, &r_reg,  &bz_wid[0], &BZ_WID_BOX[0]);
  x = x + 2 * w3;

  create_btn_vstring(
    1, x,    y, w3, 4*h2, a_o, size, &ww_reg, &bz_fun[2], &BZ_FUN_BTN[2]);
  create_dat_box_vstring(
    1, x+w3, y, w3, 4*h2,      size, &r_reg,  &bz_fun[2], &BZ_FUN_BOX[2]);
  x = x + 2 * w3;

  create_btn_vstring(
    1, x,    y, w2, 4*h2, e_d, size, &ww_reg, &ft_msk[1], &FT_MSK_BTN[1]);
  create_dat_box_vstring(
    1, x+w2, y, w2, 4*h2,      size, &r_reg,  &ft_msk[1], &FT_MSK_BOX[1]);

  x = x0 + 5 + w1 + 2 * 2 * w2 + 2 * w3 + 5;
  y = y0 + 5 + h1 + 2 * h2 + 2 * (4 * h2 + 5) + 4 * h2 + 5;
  add_frame_box(x, y, 3*2*w3, 3*h2, "", size);
  x = x + 3 * 2 * w3;

  create_btn_vstring(
    1, x,    y, w2, 3*h2, e_d, size, &ww_reg, &ft_msk[2], &FT_MSK_BTN[2]);
  create_dat_box_vstring(
    1, x+w2, y, w2, 3*h2,      size, &r_reg,  &ft_msk[2], &FT_MSK_BOX[2]);

}

//~----------------------------------------------------------------------------

void update_FT_setup_panel(void) {

  int i;
  
  for (i=0; i<2; i++) {
    show_btn_string(    12,      NULL, &TOF_MSK_BTN[i][0]);
    show_dat_box_string(12, e_d, NULL, &TOF_MSK_BOX[i][0]);
  }
  show_btn_string(    12,      NULL, &TOF_FUN_BTN[0]);
  show_dat_box_string(12, a_o, NULL, &TOF_FUN_BOX[0]);

  for (i=0; i<2; i++) {
    show_btn_string(    1,      NULL, &ECAL_MSK_BTN[i][0]);
    show_dat_box_string(1, e_d, NULL, &ECAL_MSK_BOX[i][0]);
  }
  show_btn_string(    1,      NULL, &ECAL_FUN_BTN[0]);
  show_dat_box_string(1, a_o, NULL, &ECAL_FUN_BOX[0]);

  show_btn_string(    2,      NULL, &FTC_SEL_BTN[0]);
  show_dat_box_string(1, e_d, NULL, &FTC_SEL_BOX[0]);
  show_dat_box_string(1, d_e, NULL, &FTC_SEL_BOX[1]);
  show_btn_string(    3,      NULL, &BZ_FUN_BTN[0]);
  show_dat_box_string(3, a_o, NULL, &BZ_FUN_BOX[0]);
  show_btn_string(    2,      NULL, &BZ_WID_BTN[0]);
  show_dat_box_string(2, wid, NULL, &BZ_WID_BOX[0]);

  show_btn_string(    3,      NULL, &FT_MSK_BTN[0]);
  show_dat_box_string(3, e_d, NULL, &FT_MSK_BOX[0]);
}

//~============================================================================
//
//            "MASK MORE" PANEL
//
//~============================================================================

void create_mask_more_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 70, w2 = 15, h1 = 30, h2 = 15;
  int size;
  
  extern _b more_msk[8];

//~--

  char *txt1[8] = {
  "ECAL-XA", "ECAL-YA", "EXT-GATE0", "EXT-GATE1",
  "TRST-0", "TRST-1", "LA-0", "LA-1"};

//~--

  w = 5 + w1 + 2 * w2 + 5;
  h = 5 + h1 + h2 + 8 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "Mask More", size);
  y = y + h1;
  
//~--

  size = FL_SMALL_SIZE;

  add_frame_box(x,       y, w1, h2, "", size);
  add_frame_box(x+w1,    y, w2, h2, "W", size);
  add_frame_box(x+w1+w2, y, w2, h2, "R", size);
  y = y + h2;

  create_frame_box_vstring(8, x, y, w1, h2, txt1, size, 'L');
  x = x + w1;

  size = FL_NORMAL_SIZE;

  create_btn_vstring(
    8, x, y, w2, h2, e_d, size, &ww_reg, &more_msk[0], &MORE_MSK_BTN[0]);
  x = x + w2;
  create_dat_box_vstring(
    8, x, y, w2, h2,      size, &r_reg,  &more_msk[0], &MORE_MSK_BOX[0]);
}

//~----------------------------------------------------------------------------

void update_mask_more_panel(void) {

  show_btn_string(    8,      NULL, &MORE_MSK_BTN[0]);
  show_dat_box_string(8, e_d, NULL, &MORE_MSK_BOX[0]);
}

//~============================================================================
//
//            OUTPUT SETUP PANEL
//
//~============================================================================

void create_output_setup_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 35, w2 = 30, w3 = 15, h1 = 30, h2 = 15;
  int size;
  
  extern _b out_sig[2];
  extern _b out_msk[5];

//~--

  char *txt1[5] = {
  "LA-0", "LA-1", "EXT", "FT to RICH", "FT to TOF"};

//~--

  w = 5 + w1 + 2 * w2 + 2 * w3 + 5;
  h = 5 + h1 + h2 + 5 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "Output Setup", size);
  y = y + h1;
  
//~--

  size = FL_SMALL_SIZE;

  add_frame_box(x,            y, w1, h2, "", size);
  add_frame_box(x+w1,         y, w2, h2, "W", size);
  add_frame_box(x+w1+1*w2,    y, w2, h2, "R", size);
  add_frame_box(x+w1+2*w2,    y, w3, h2, "W", size);
  add_frame_box(x+w1+2*w2+w3, y, w3, h2, "R", size);
  y = y + h2;

  create_frame_box_vstring(3, x, y,      w1,      h2, txt1,     size, 'L');
  create_frame_box_vstring(2, x, y+3*h2, w1+2*w2, h2, &txt1[3], size, 'L');
  x = x + w1;

  size = FL_NORMAL_SIZE;

  create_btn_vstring(
    1, x, y,      w2, 2*h2, ft_lv1, size, &ww_reg, &out_sig[0], &OUT_SIG_BTN[0]);
  create_btn_vstring(
    1, x, y+2*h2, w2, h2,   ft_lv1, size, &ww_reg, &out_sig[1], &OUT_SIG_BTN[1]);
  x = x + w2;
  create_dat_box_vstring(
    1, x, y,      w2, 2*h2,         size, &r_reg,  &out_sig[0], &OUT_SIG_BOX[0]);
  create_dat_box_vstring(
    1, x, y+2*h2, w2, h2,           size, &r_reg,  &out_sig[1], &OUT_SIG_BOX[1]);
  x = x + w2;

  create_btn_vstring(
    5, x, y, w3, h2, e_d, size, &ww_reg, &out_msk[0], &OUT_MSK_BTN[0]);
  x = x + w3;
  create_dat_box_vstring(
    5, x, y, w3, h2,      size, &r_reg,  &out_msk[0], &OUT_MSK_BOX[0]);
}

//~----------------------------------------------------------------------------

void update_output_setup_panel(void) {

  show_btn_string(    2,         NULL, &OUT_SIG_BTN[0]);
  show_dat_box_string(2, ft_lv1, NULL, &OUT_SIG_BOX[0]);

  show_btn_string(    5,         NULL, &OUT_MSK_BTN[0]);
  show_dat_box_string(5, e_d,    NULL, &OUT_MSK_BOX[0]);
}

//~============================================================================
//
//            LEVEL-1 TRIGGER SETUP PANEL
//
//~============================================================================

void create_LV1_setup_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 75, w2 = 15, h1 = 30, h2 = 15;
  int size;

  extern _b lv1_msk[8][18];
  extern _b lv1_or_msk[8];

  char *txt1[16] = {
  "FTCP0",     "FTCP1",       "FTCT0",     "FTCT1",
  "FTZ",       "FTE",         "ACC0",      "ACC1",
  "BZ",        "ECAL-F_and",  "ECAL-F_or", "ECAL-A_and",
  "ECAL-A_or", "EXT-GATE0",   "EXT_GATE1", ""};

//~--

  w = 5 + w1 + 8 * (2 * w2 + 5) - 5 + 5;
  h = 5 + h1 + 2 * h2 + 4 * (4 * h2 + 5) + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "Level-1 Trigger Setup", size);
  y = y + h1;
  
//~--

  size = FL_SMALL_SIZE;

  obj = add_frame_box(x, y, w1, h2, "Sub-Trigger", size);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  create_frame_box_hstring(8, x+w1, y, 2*w2, h2, hex, size+100, 'C');
  y = y + h2;

  obj = add_frame_box(x, y, w1, h2, "AND Masks:", size);
    fl_set_object_lalign(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  for (i=0; i<8; i++) {
    add_frame_box(x+w1+   i*(2*w2+5), y, w2, h2, "W", size);
    add_frame_box(x+w1+w2+i*(2*w2+5), y, w2, h2, "R", size);
  }
  y = y + h2;

  create_frame_box_vstring(16, x, y, w1, h2, txt1, size+400, 'L');
  x = x + w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<8; i++) {
    create_btn_vstring(
      16, x, y, w2, h2, e_d, size+400, &ww_reg, &lv1_msk[i][0], &LV1_MSK_BTN[i][0]);
    x = x + w2;
    create_dat_box_vstring(
      16, x, y, w2, h2,      size+400, &r_reg,  &lv1_msk[i][0], &LV1_MSK_BOX[i][0]);
    x = x + w2 + 5;
  }

//~--

  size = FL_SMALL_SIZE;

  x = x0 + 5;
  y = y0 + 5 + h1 + 2 * h2 + 4 * (4 * h2 + 5);
  obj = add_frame_box(x, y, w1, h2, "OR Mask", size);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<8; i++) {
    create_btn_hstring(
      1, x, y, w2, h2, e_d, size, &ww_reg, &lv1_or_msk[i], &LV1_MSK_BTN[8][i]);
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w2, h2,      size, &r_reg,  &lv1_or_msk[i], &LV1_MSK_BOX[8][i]);
    x = x + w2 + 5;
  }
}

//~----------------------------------------------------------------------------

void update_LV1_setup_panel(void) {

  int i;
  
  for (i=0; i<9; i++) {
    int n = i < 8 ? 16 : 8;
    show_btn_string(    n,      NULL, &LV1_MSK_BTN[i][0]);
    show_dat_box_string(n, e_d, NULL, &LV1_MSK_BOX[i][0]);
  }
}

//~============================================================================
//
//            LEVEL-1 PRESCALERS PANEL
//
//~============================================================================

void create_prescalers_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 35, h1 = 30, h2 = 15;
  int size;

  int pre_ind[8] = {0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E};

  extern _b pre_dec_msk[8][4];
  extern _b pre_num_msk[8];

//~--

  w = 5 + w1 + 4 * w1 + w2 + 5;
  h = 5 + h1 + h2 + 8 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "PreScalers", size);
  y = y + h1;

  size = FL_SMALL_SIZE;

  add_frame_box(x,         y, w1,   h2, " ", size);
  add_frame_box(x+w1,      y, 4*w1, h2, "W", size);
  add_frame_box(x+w1+4*w1, y, w2,   h2, "R", size);
  y = y + h2;

  size = FL_NORMAL_SIZE;

  for (i=0; i<8; i++) {
    char b[80];
    int j;
    pre_num_msk[i].ind = pre_ind[i];
    pre_num_msk[i].msk = pre_num_msk[0].msk;
    pre_num_msk[i].n   = pre_num_msk[0].n;
    for (j=0; j<4; j++) {
      pre_dec_msk[i][j].ind = pre_ind[i];
      pre_dec_msk[i][j].msk = pre_dec_msk[0][j].msk;
      pre_dec_msk[i][j].n   = pre_dec_msk[0][j].n;
    }
    sprintf(b, "%d", i);
    add_frame_box(x, y, w1, h2, b, FL_SMALL_SIZE);
    create_btn10_hstring(
      4, x+w1,      y, w1, h2, hex, size, &ww_reg, &pre_dec_msk[i][0], &LV1_PRE_BTN[i][0]);
    for (j=0; j<4; j++) {
      LV1_PRE_BTN[i][j]->cnv1 = convert_prescaler_code_to_value;
      LV1_PRE_BTN[i][j]->cnv2 = convert_prescaler_value_to_code;
    }
    create_dat_box_vstring(
      1, x+w1+4*w1, y, w2, h2,      size, &r_reg,  &pre_num_msk[i],    &LV1_PRE_BOX[i]);
    LV1_PRE_BOX[i]->cnv = convert_prescaler_code_to_value;
    y = y + h2;
  }
}

//~----------------------------------------------------------------------------

void convert_prescaler_code_to_value(int32 code, int32 *value) {

  *value = 1024 - code;
}

//~----------------------------------------------------------------------------

void convert_prescaler_value_to_code(int32 value, int32 *code) {

  *code = 1024 - value;
}

//~----------------------------------------------------------------------------

void update_prescalers_panel(void) {

  int i;
  
  for (i=0; i<8; i++) {
    show_btn_string(    4,       NULL,  &LV1_PRE_BTN[i][0]);
    show_dat_box_string(1, NULL, "%4d", &LV1_PRE_BOX[i]);
  }
}

//~============================================================================
//
//            INPUT & TRIGGER PATTERNS PANEL
//
//~============================================================================

void create_signal_pattern_panel(int x0, int y0) {

  int x, y, w, h, ww;
  int w1 = 40, w2 = 15, w3 = 25, w4 = 75, w5 = 20, h1 = 30, h2 = 15;
  int size;

  extern _b inp_sig[2][16];
  extern _b trg_sig[2][16];

  char *txt1[16] = {
  "CP S1", "CP S2", "CP S3", "CP S4", 
  "CT S1", "CT S2", "CT S3", "CT S4", 
  "BZ S1", "BZ S2", "BZ S3", "BZ S4", 
  "",      "",      "",      ""     };

  char *txt2[16] = {
  "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
  "",   "",   "",   "",   "",   "",   "",   ""  };

  char *txt3[16] = {
  "FTC",        "FTCP0",     "FTCP1",       "FTCT0",
  "FTCT1",      "FTZ",       "FTE",         "ACC0", 
  "ACC1",       "BZ",        "ECAL-F_and",  "ECAL-F_or",
  "ECAL-A_and", "ECAL-A_or", "EXT-GATE0",   "EXT_GATE1"};

  char *txt4[1] = {"S\nu\nb\n \nL\nV\n1"};
  char *txt5[8] = {"LA-0", "LA-1", "", "DSP", "Gen", "", "", ""};

//~--

  w = 5 + w1 + w2 + 5 + w3 + w2 + 5 + w4 + w2 + 5 + w5 + w2 + w2 + 5;
  h = 5 + h1 + h2 + 4 * (4 * h2 + 5) - 5 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = FL_LARGE_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "Input & Trigger", size);
  y = y + h1;

  size = FL_SMALL_SIZE;
  w = w1 + w2 + 5 + w3 + w2;
  add_frame_box(x,     y, w,  h2, "Input Patterns", size);
  ww = w4 + w2 + 5 + w5 + w2 + w2;
  add_frame_box(x+w+5, y, ww, h2, "Trigger Patterns", size);
  y = y + h2;

//~--

  size = FL_SMALL_SIZE;
  create_frame_box_vstring(16, x, y, w1, h2, txt1, size+400, 'L');
  x = x + w1;
  
  size = FL_NORMAL_SIZE;
  create_dat_box_vstring(
    16, x, y, w2, h2, size+400, &r_reg, &inp_sig[0][0], &INP_SIG_BOX[0][0]);
  x = x + w2 + 5;

//~--

  size = FL_SMALL_SIZE;
  create_frame_box_vstring(16, x, y, w3, h2, txt2, size+400, 'L');
  x = x + w3;
  
  size = FL_NORMAL_SIZE;
  create_dat_box_vstring(
    16, x, y, w2, h2, size+400, &r_reg, &inp_sig[1][0], &INP_SIG_BOX[1][0]);
  x = x + w2 + 5;

//~--

  size = FL_SMALL_SIZE;
  create_frame_box_vstring(16, x, y, w4, h2, txt3, size+400, 'L');
  x = x + w4;
  
  size = FL_NORMAL_SIZE;
  create_dat_box_vstring(
    16, x, y, w2, h2, size+400, &r_reg, &trg_sig[0][0], &TRG_SIG_BOX[0][0]);
  x = x + w2 + 5;

//~--

  size = FL_SMALL_SIZE;
  h = 8 * h2 + 5;
  create_frame_box_vstring(1, x, y,     w5,    h,  txt4, size, 'C');
  create_frame_box_vstring(8, x, y+h+5, w5+w2, h2, txt5, size+400, 'C');
  x = x + w5;
  create_frame_box_vstring(8, x, y,     w2,    h2, hex,  size+400, 'C');
  x = x + w2;
  
  size = FL_NORMAL_SIZE;
  create_dat_box_vstring(
    16, x, y, w2, h2, size+400, &r_reg, &trg_sig[1][0], &TRG_SIG_BOX[1][0]);
  x = x + w2 + 5;

//~--

}

//~----------------------------------------------------------------------------

void update_signal_pattern_panel(void) {

  int i;
  
  for (i=0; i<2; i++) {
    show_dat_box_string(16, hex, NULL, &TRG_SIG_BOX[i][0]);
    show_dat_box_string(16, hex, NULL, &INP_SIG_BOX[i][0]);
  }
}

//~============================================================================
//
//            CONFIGURATION FILE PANEL
//
//~============================================================================

char *cfg_nam[6] = {
  "SCI", "ECAL_SCI", "ECAL", "SCI_CHER", "Internal", "Disabled"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void create_config_file_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 90, h1 = 30, h2 = 20, h3 = 55;
  _b msk = {0, 0x00000007, 6};
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "File", FL_LARGE_SIZE);
  y = y + h1;

  create_btn_hstring(
    1, x, y, w1, h2, cfg_nam, FL_MEDIUM_SIZE, &w_cfg_file, &msk, &CFG_FILE_BTN);
  y = y + h2;

  obj = add_normal_button(x, y, w1, h3, "WRITE\nCONFIG\nFILE", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, write_config_file_callback, 0);
}

//~----------------------------------------------------------------------------

void write_config_file_callback(FL_OBJECT* obj, long data) {

  _ww *f = &w_cfg_file;
  char b[80];
  
  sprintf(b, "configuration_file_for_%s_trigger", cfg_nam[f->inp[0]]);
  write_config_file(b);

  f->wri[0] = f->inp[0];
  f->unk = FALSE;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_config_file_panel(void) {

  show_btn_string(1, NULL, &CFG_FILE_BTN);
}

//~============================================================================
//
//            BUSY REGISTERS (MASK, STATUS, ERROR) PANEL
//
//~============================================================================

extern _b busy_reg_msk[3][3][16];

_b *m_busy[3][3] = {
  {&busy_reg_msk_v[0][0][0], &busy_reg_msk_v[0][1][0], &busy_reg_msk_v[0][2][0]},
  {&busy_reg_msk_v[1][0][0], &busy_reg_msk_v[1][1][0], &busy_reg_msk_v[1][2][0]},
  {&busy_reg_msk_v[2][0][0], &busy_reg_msk_v[2][1][0], &busy_reg_msk_v[2][2][0]}};

DAT_BOX **BUSY_BOX[2][3][3] = {
  {{&BUSY_MSK_BOX[0][0][0], &BUSY_MSK_BOX[0][1][0], &BUSY_MSK_BOX[0][2][0]},
   {&BUSY_STA_BOX[0][0][0], &BUSY_STA_BOX[0][1][0], &BUSY_STA_BOX[0][2][0]},
   {&BUSY_ERR_BOX[0][0][0], &BUSY_ERR_BOX[0][1][0], &BUSY_ERR_BOX[0][2][0]}},
  {{&BUSY_MSK_BOX[1][0][0], &BUSY_MSK_BOX[1][1][0], &BUSY_MSK_BOX[1][2][0]},
   {&BUSY_STA_BOX[1][0][0], &BUSY_STA_BOX[1][1][0], &BUSY_STA_BOX[1][2][0]},
   {&BUSY_ERR_BOX[1][0][0], &BUSY_ERR_BOX[1][1][0], &BUSY_ERR_BOX[1][2][0]}}};
   
FL_OBJECT *BUSY_MSK_LAY[2];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_busy_registers_panel(int x0, int y0, int ind) {

  FL_OBJECT *obj;
  int i, k;
  int x, y, w, h, y1;
  int w1a = 20, w1b = 20, w1c = 15, w1 = w1a + w1b + w1c, w2 = 15, w3 = 35;
  int h0 = 30, h1 = 15;
     
  char *txt1[8] = {"TRST-0", "TRST-1", " ", " ", " ", "Seq", " ", " "};

  char *txt2[20] = {
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", 
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19"};
  char *txt3[20] = {
    "T2", "T3", "U1", "T0", "T1", "S1", "  ", "S0", "U0", "  ", 
    "R0", "R1", "E0", "E1", "S2", "T5", "T4", "T6", "S3", "T7"};
  char *txt4[40] = {
    "A", "B", "A", "B", "A", "B", "A", "B", "A", "B",
    "A", "B", "A", "B", "A", "B", "A", "B", "A", "B",
    "A", "B", "A", "B", "A", "B", "A", "B", "A", "B",
    "A", "B", "A", "B", "A", "B", "A", "B", "A", "B"};
  
  char *txt5[8] = {"Err", "All", "FE", " ", "Seq", " ", "DSP", " "};

  int order[2][20] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19},
    { 8, 2, 7, 5,14,18, 3, 4, 0, 1,16,15,17,19,10,11,12,13}};

  char *border[18] = {
    "TRD",       NULL, 
    "TOF & ACC", NULL, NULL, NULL, 
    "Tracker",   NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "RICH",      NULL, 
    "ECAL",      NULL};

//~--

  BUSY_MSK_LAY[ind] = fl_bgn_group();
  
  w = 5 + w1 + 2 * w2 + 5 + 2 * w2 + w3 + 5;
  h = 5 + h0 + h1 + 8 * h1 + (ind ? 36 * h1 + 5 * h1 : 40 * h1) + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h0, "Busy Control", 'L');
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w-10, h0, "");
     fl_set_object_callback(obj, change_busy_registers_panel_layout_callback, 0);
  y = y + h0;
  
  add_frame_box(x, y, w1,   h1, "",    'S');
  x = x + w1;
  add_frame_box(x, y, 2*w2, h1, "msk", 'S');
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, h1, "");
     fl_set_object_callback(obj, reset_busy_mask_callback, 0);
  x = x + 2 * w2 + 5;
  add_frame_box(x, y, w2,   h1, "S",   'S');
  x = x + w2;
  add_frame_box(x, y, w2,   h1, "E",   'S');
  x = x + w2;
  add_frame_box(x, y, w3,   h1, "",    'S');
  y = y + h1;
  y1 = y;

//~--

  x = x0 + 5;
  create_frame_box_vstring(8, x, y, w1, h1, txt1, 'S', 'C');
  x = x + w1;

  for (i=0; i<3; i++) {
    y = y1;
    if (i == 0) {
      create_btn_vstring(
        8, x, y, w2, h1, __m, 'N', &ww_reg, m_busy[i][0], BUSY_MSK_BTN[ind][0]);
      x = x + w2;
    }
    create_dat_box_vstring(
      8, x, y, w2, h1, 'N', &r_reg, m_busy[i][0], BUSY_BOX[ind][i][0]);
    x = x + w2;
    if (i == 0) x = x + 5;
  }

  create_frame_box_vstring(8, x, y, w3, h1, txt5, 'S', 'C');
  y = y + 8 * h1;
  y1 = y;

//~--

  for (k=0; k<(ind?18:20); k++) {
    int k1 = order[ind][k];
    int k2 = ((k1 * 2) + 8) / 16;
    int k3 = ((k1 * 2) + 8) % 16;
    x = x0 + 5;
    if (ind && border[k]) {
      add_frame_box(x, y, w-10, h1, border[k], 'S');
      y = y + h1;
    }
    create_frame_box_vstring(1, x, y, w1a, 2*h1, &txt2[k1], 'S', 'C');
    x = x + w1a;
    create_frame_box_vstring(1, x, y, w1b, 2*h1,&txt3[k1],  'S', 'C');
    x = x + w1b;
    create_frame_box_vstring(2, x, y, w1c, h1,   txt4,      'S', 'C');
    x = x + w1c;
    create_btn_vstring(
      2, x, y, w2, h1, __m, 'N', &ww_reg, &m_busy[0][k2][k3], &BUSY_MSK_BTN[ind][k2][k3]);
    x = x + w2;
    for (i=0; i<3; i++) {
      create_dat_box_vstring(
        2, x, y, w2, h1, 'N', &r_reg, &m_busy[i][k2][k3], &BUSY_BOX[ind][i][k2][k3]);
      x = x + w2;
      if (i == 0) x = x + 5;
    }
    create_frame_box_vstring(2, x, y, w1c, h1,   txt4,      'S', 'C');
    x = x + w1c;
    create_frame_box_vstring(1, x, y, w1b, 2*h1,&txt3[k1],  'S', 'C');
    y = y + 2 * h1;
  }

//~--

  fl_end_group();
}


//~----------------------------------------------------------------------------

void change_busy_registers_panel_layout_callback(FL_OBJECT* obj, long data) {

  busy_msk_layout = MOD(busy_msk_layout + 1, 2);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void reset_busy_mask_callback(FL_OBJECT* obj, long data) {

  int i;
  static int set = 0;

  for (i=0; i<3; i++) {
    int k = m_busy[0][i]->ind;
    int32 m = i == 0 ? 0xFF00 : 0xFFFF;
    _ww *w = &ww_reg;
    _r  *r = &r_reg;
    switch (set) {
      case 0:
        pack32(&w->inp[k], m, 0x0000);
      break;
      case 1:
        pack32(&w->inp[k], m, 0xFFFF);
      break;
      case 2:
        pack32(&w->inp[k], m, 0xAAAA);
      break;
      case 3:
        pack32(&w->inp[k], m, 0x5555);
      break;
      case 4:
        pack32(&w->inp[k], m,  r->rea[k]);
      break;
      case 5:
        pack32(&w->inp[k], m, ~r->rea[k]);
      break;
    }
  }

  set = MOD(set + 1, 6);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_busy_registers_panel(void) {

  int i, j;
  int ind = busy_msk_layout;

  for (i=0; i<3; i++) {
    for (j=0; j<16; j++) {
      if (BUSY_MSK_BTN[ind][i][j]) {
        show_btn_string(    1,      NULL, &BUSY_MSK_BTN[ind][i][j]);
        show_dat_box_string(1, __m, NULL, &BUSY_BOX[ind][0][i][j]);
        show_dat_box_string(1, hex, NULL, &BUSY_BOX[ind][1][i][j]);
        show_dat_box_string(1, e__, NULL, &BUSY_BOX[ind][2][i][j]);
      }
    }
  }

  fl_deactivate_object(BUSY_MSK_LAY[1-busy_msk_layout]);
  fl_hide_object(      BUSY_MSK_LAY[1-busy_msk_layout]);
  fl_activate_object(  BUSY_MSK_LAY[busy_msk_layout]);
  fl_show_object(      BUSY_MSK_LAY[busy_msk_layout]);
}

//~============================================================================
//
//            READ, WRITE, UPDATE PANEL
//
//~============================================================================

void create_read_write_update_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 90, h1 = 25, h2 = 40;
  int size;
  
  w = 5 + w1 + 5;
  h = 5 + 2 * h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  size = FL_MEDIUM_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "READ ALL", size);
    fl_set_object_callback(obj, read_all_callback, 0);
  y = y + h1;
  obj = add_normal_button(x, y, w1, h1, "WRITE ALL", size);
    fl_set_object_callback(obj, write_all_callback, 0);
  y = y + h1;
  obj = add_normal_button(x, y, w1, h2, "UPDATE\nINPUT", size);
    fl_set_object_callback(obj, update_input_callback, 0);
}

//~----------------------------------------------------------------------------

void read_all_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_reg;
  int i;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<n_reg; i++) r->was[i] = r->rea[i];
  
  begin_IO_segment(request);
  for (i=0; i<n_reg; i++) add_IO_read(request, i);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);

  if (!err) {
    for (i=0; i<n_reg; i++) r->rea[i] = reply[4*i+3];

//printf("READ ALL\n");
//printf("LUT:: ");for(i=0x1A;i<=0x1C;i++)printf("%02X: %04X  ",i,r->rea[i]);printf("\n");
//printf("ACC:: ");for(i=0x0F;i<=0x0F;i++)printf("%02X: %04X\n",i,r->rea[i]);
//printf("ACC:: ");for(i=0x16;i<=0x16;i++)printf("%02X: %04X\n",i,r->rea[i]);
//printf("THR:: ");for(i=0x1E;i<=0x1E;i++)printf("%02X: %04X\n",i,r->rea[i]);
//printf("TOF:: ");for(i=0x10;i<=0x11;i++)printf("%02X: %04X  ",i,r->rea[i]);printf("\n");
//printf("TOF:: ");for(i=0x15;i<=0x15;i++)printf("%02X: %04X\n",i,r->rea[i]);
//printf("LV1:: ");for(i=0x1F;i<=0x26;i++)printf("%02X: %04X  ",i,r->rea[i]);printf("\n");
printf("r->rea[0x37] = %04X\n", r->rea[37]);

    if (r->unk) for (i=0; i<n_reg; i++) r->was[i] = r->rea[i];
    r->unk = FALSE;
    r->sta = status_OK;
    for (i=0; i<100; i++) {
      _r *r = r_list[i];
      if (!r) break;
      r->unk = FALSE;
      r->sta = status_OK;
    }
  }
  else {
    r->sta = status_KO;
    for (i=0; i<100; i++) {
      _r *r = r_list[i];
      if (!r) break;
      r->sta = status_KO;
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void write_all_callback(FL_OBJECT* obj, long data) {

  _r  *r = &r_reg;
  _ww *w = &ww_reg;
  int i, j;
  int16 request[1000];
  int16 reply[1000];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;
  for (i=0; i<100; i++) {
    _r *r = r_list[i];
    if (!r) break;
    r->sta = status_NA;
  }

  for (i=0; i<n_reg; i++) w->inp[i] &= 0x0000FFFF;
  for (i=0; i<100; i++) {
    _ww *w = ww_list[i];
    if (!w) break;
    w->inp[0] &= 0x0000FFFF;
//printf("i = %d, w = %08X, w->inp[0] = %04X\n", i, (int32)w, w->inp[0]);
  }

  begin_IO_segment(request);
  for (i=0; i<n_reg; i++) add_IO_write(request, i, w->inp[i]);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);
  
//printf("WRITE ALL\n");
//printf("LUT:: ");for(i=0x1A;i<=0x1C;i++)printf("%02X: %04X  ",i,w->inp[i]);printf("\n");
//printf("ACC:: ");for(i=0x0F;i<=0x0F;i++)printf("%02X: %04X\n",i,w->inp[i]);
//printf("ACC:: ");for(i=0x16;i<=0x16;i++)printf("%02X: %04X\n",i,w->inp[i]);
//printf("THR:: ");for(i=0x1E;i<=0x1E;i++)printf("%02X: %04X\n",i,w->inp[i]);
//printf("TOF:: ");for(i=0x10;i<=0x11;i++)printf("%02X: %04X  ",i,w->inp[i]);printf("\n");
//printf("TOF:: ");for(i=0x15;i<=0x15;i++)printf("%02X: %04X\n",i,w->inp[i]);

  for (i=0; i<n_reg; i++) {
    if (!err) {
      w->wri[i] = w->inp[i];
      w->unk = FALSE;
      for (j=0; j<100; j++) {
        _ww *w = ww_list[j];
        if (!w) break;
        w->wri[0] = w->inp[0];
        w->unk = FALSE;
      }
    }
    else {
      w->unk = TRUE;
      for (j=0; j<100; j++) {
        _ww *w = ww_list[j];
        if (!w) break;
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

void update_input_callback(FL_OBJECT* obj, long data) {

  _r  *r = &r_reg;
  _ww *w = &ww_reg;
  int i;

  if (obj) {
    msg = -1;
    update_objects();
  }

printf("UPDATE INPUT:\n");

  err = 0x0000;
  if (!err) read_all_callback(NULL, 0);
  if (!err) {
    for (i=0; i<n_reg; i++) w->inp[i] = r->rea[i];
    for (i=0; i<100; i++) {
      _ww *w = ww_list[i];
      if (!w) break;
      w->inp[0] = r->rea[0];
    }
    write_all_callback(NULL, 0);
  }

  if (obj) {
    msg = 1;
    update_objects();
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

  update_TOF_look_up_panel();
  update_ACC_panel();
  update_FT_setup_panel();
  update_mask_more_panel();
  update_output_setup_panel();

  update_LV1_setup_panel();
  update_prescalers_panel();
  update_signal_pattern_panel();
  
  update_config_file_panel();

  update_busy_registers_panel();

  write_config_file(dat_config_file_name);
  w_cfg_file.unk = TRUE;
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
