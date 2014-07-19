// file JINF.c
//
// JINF "Pro" Controller
//
// A.Lebedev Aug-2005...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"JINF Pro Controller"};                // global variable
char *date  = {"13-Dec-08"};                          // global variable

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

//~-- xDR Path Panel

SW_BTN    *PATH_HEX_BTN[2][4];
_w w_path[2];

//~-- Boot JINF Panel

SW_BTN *BOOT_ADR_BTN[4];
_w w_boot_adr;

//~-- Load Boot Panel

FL_OBJECT *FILE_HEADER_BOX[8];
_r r_file_header;

//~-- SSF Panel

FL_OBJECT *SSF_BOX[1];
SW_BTN    *SSF_BTN[1];
_r r_SSF;
_w w_SSF;

//~-- Delay Panel

FL_OBJECT *DELAY_HEX_BOX[2];
SW_BTN    *DELAY_HEX_BTN[2];
_r r_delay;
_w w_delay;

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

//~-- Busy Panel

SW_BTN    *BUSY_MASK_BIT_BTN[24];
FL_OBJECT *BUSY_MASK_BIT_BOX[24];
FL_OBJECT *BUSY_STATUS_BIT_BOX[24];
FL_OBJECT *BUSY_STATUS_SUM_BOX;
FL_OBJECT *BUSY_ERRORS_BIT_BOX[24];
_r r_busy_mask;
_w w_busy_mask;
_r r_busy_status;
_r r_busy_sum;
_r r_busy_errors;

//~-- Slave Masks Panel

FL_OBJECT *SLAVE_MASK_BIT_BOX[24];
SW_BTN    *SLAVE_MASK_BIT_BTN[24];
SW_BTN    *SLAVE_MASK_ID_GET_BTN[1], *SLAVE_MASK_ID_SET_BTN[1];
_r r_slave_mask;
_w w_slave_mask;
_w w_slave_mask_id_get, w_slave_mask_id_set;
int32 masks[16];

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *mod[4] = {
"Off", "Raw", "Comp", "Raw+Comp"};

char *got[2] = {
"Empty", "Got It"};

char *o_o[2] = {
"OFF", "ON"};

char *fai[2] = {
"FAIL", "O.K."};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
void update_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_path_panel(int x0, int y0);
void reset_path_callback(FL_OBJECT* obj, long data);
void update_path_panel(void);

void create_boot_panel(int x0, int y0);
void boot_callback(FL_OBJECT* obj, long data);
void update_boot_panel(void);

void create_load_boot_panel(int x0, int y0);
void load_boot_callback(FL_OBJECT* obj, long data);
void read_FLASH_summary_callback(FL_OBJECT* obj, long data);
void update_load_boot_panel(void);

//~--

void create_SSF_panel(int x0, int y0);
void get_SSF_callback(FL_OBJECT* obj, long data);
void set_SSF_callback(FL_OBJECT* obj, long data);
void update_SSF_panel(void);

void create_delay_panel(int x0, int y0);
void get_delay_callback(FL_OBJECT* obj, long data);
void set_delay_callback(FL_OBJECT* obj, long data);
void update_delay_panel(void);

void create_test_slaves_panel(int x0, int y0);
void test_slaves_callback(FL_OBJECT* obj, long data);

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

void create_power_down_panel(int x0, int y0);
void power_down_callback(FL_OBJECT* obj, long data);

//~--

void create_busy_control_panel(int x0, int y0);
void reset_busy_mask_callback(FL_OBJECT* obj, long data);
void write_busy_mask_callback(FL_OBJECT* obj, long data);
void read_busy_mask_callback(FL_OBJECT* obj, long data);
void read_busy_status_callback(FL_OBJECT* obj, long data);
void read_busy_errors_callback(FL_OBJECT* obj, long data);
void update_busy_panel(void);

void create_slave_mask_panel(int x0, int y0);
void reset_slave_mask_callback(FL_OBJECT* obj, long data);
void write_slave_mask_callback(FL_OBJECT* obj, long data);
void read_slave_mask_callback(FL_OBJECT* obj, long data);
void update_slave_mask_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 595, HH = 385;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
//AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  if (!use_TCP) create_path_panel(0,  45);
  create_boot_panel(0, 100);

  create_load_boot_panel(70, 45);

  create_SSF_panel(        235, 45);
  create_delay_panel(      325, 45);
  create_test_slaves_panel(420, 45);
  if (!use_TCP) create_EPPCAN_box_panel( 485, 45);

  create_ping_panel(     235,  85);
  create_proc_mode_panel(285,  85);
  create_event_nb_panel( 385,  85);
  create_get_event_panel(535,  85);

  create_status_panel(0, 140);

  create_power_down_panel(520, 205);

  create_busy_control_panel(0, 195);
  create_slave_mask_panel(  0, 290);
 
  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  for (i=0; i<2; i++) invalidate_w(&w_path[i], 1, 0x0000);

  invalidate_w(&w_boot_adr, 1, 0x0000);

  invalidate_r(first, &r_file_header, 8);

  invalidate_r(first, &r_SSF, 1);
  invalidate_w(&w_SSF, 1, 0x0000);
  
  invalidate_r(first, &r_delay, 1);
  invalidate_w(&w_delay, 1, 0x0000);
  
  invalidate_r(first, &r_epp_busy_sta, 1);
  invalidate_w(&w_epp_trig_msk, 1, 0x0000);
  
  invalidate_r(first, &r_ping_sta, 1);

  invalidate_r(first, &r_proc_mode, 1);
  invalidate_w(&w_proc_mode, 1, 0x0000);

  invalidate_r(first, &r_event_nb, 1);

  invalidate_r(first, &r_get_event, 1);

  invalidate_r(first, &r_jinf_sta, 18);

  invalidate_r(first, &r_busy_mask, 1);
  invalidate_w(&w_busy_mask, 1, 0x0000);
  
  invalidate_r(first, &r_busy_status, 1);
  invalidate_r(first, &r_busy_sum, 1);

  invalidate_r(first, &r_busy_errors, 1);
  
  invalidate_r(first, &r_slave_mask, 1);
  invalidate_w(&w_slave_mask, 1, 0x0000);
  invalidate_w(&w_slave_mask_id_get, 1, 0x0000);
  invalidate_w(&w_slave_mask_id_set, 1, 0x0000);

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

void update_callback(int32 data) {

  update_objects();
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&w_path[0].inp,           4, 1, file);
    fread(&w_path[1].inp,           4, 1, file);
    fread(&w_boot_adr.inp,          4, 1, file);
    fread(&w_SSF.inp,               4, 1, file);
    fread(&w_delay.inp,             4, 1, file);
    fread(&w_epp_trig_msk.inp,      4, 1, file);
    fread(&w_proc_mode.inp,         4, 1, file);
    fread(&w_busy_mask.inp,         4, 1, file);
    fread(&w_slave_mask.inp,        4, 1, file);
    fread(&w_slave_mask_id_get.inp, 4, 1, file);
    fread(&w_slave_mask_id_set.inp, 4, 1, file);
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
    fwrite(&w_path[0].inp,           4, 1, file);
    fwrite(&w_path[1].inp,           4, 1, file);
    fwrite(&w_boot_adr.inp,          4, 1, file);
    fwrite(&w_SSF.inp,               4, 1, file);
    fwrite(&w_delay.inp,             4, 1, file);
    fwrite(&w_epp_trig_msk.inp,      4, 1, file);
    fwrite(&w_proc_mode.inp,         4, 1, file);
    fwrite(&w_busy_mask.inp,         4, 1, file);
    fwrite(&w_slave_mask.inp,        4, 1, file);
    fwrite(&w_slave_mask_id_get.inp, 4, 1, file);
    fwrite(&w_slave_mask_id_set.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//
//            XDR PATH PANEL
//
//
//~============================================================================

void create_path_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 15, h1 = 15;
  
  w = 5 + 4 * w1 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, 4*w1, h1, "xDR Path", 'S');
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 4*w1, h1, "");
    fl_set_object_callback(obj, reset_path_callback, 0);
  y = y + h1;

  create_hex_btn_hstring(
    4, x, y, w1, h1, hex, 'N', &w_path[0], &PATH_HEX_BTN[0][0]);
  y = y + h1;

  create_hex_btn_hstring(
    4, x, y, w1, h1, hex, 'N', &w_path[1], &PATH_HEX_BTN[1][0]);
}

//~----------------------------------------------------------------------------

void reset_path_callback(FL_OBJECT* obj, long data) {

  SW_BTN *B;
  int i, j;

  for (j=0; j<2; j++) {
    _w *w = &w_path[j];
    w->inp = 0x0000;
    for (i=0; i<4; i++) {
      B = PATH_HEX_BTN[j][i];
      B->i = 0;
      fl_set_object_label(B->obj, B->l[B->i]);
    }
  }

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_path_panel(void) {

  int i, j;
  
  for (j=0; j<2; j++) {
    _w *w = &w_path[j];
    int16 m;
    bool ok[4];
    m = unpack32(w->inp, 0x0000FF00);
    ok[0] = ok[1] = (w->inp == 0 || m < 24) ? TRUE : FALSE;
    m = unpack32(w->inp, 0x000000FF);
    ok[2] = ok[3] = (w->inp == 0 || m == 0x04 || m == 0x05 || m == 0x3F) ? TRUE : FALSE;
    for (i=0; i<4; i++) {
      SW_BTN *B = PATH_HEX_BTN[j][i];
      if (ok[i]) {
        fl_set_object_color(B->obj, FL_WHEAT, 0);
        fl_set_object_lcolor(B->obj, FL_BLACK);
      }
      else {
        fl_set_object_color(B->obj, FL_RED, 0);
        fl_set_object_lcolor(B->obj, FL_YELLOW);
      }
    }
    AMSW_PATH[j]   = (int16)w->inp;
    AMSW_PATH[j+1] = 0x0000;
  }
}

//~============================================================================
//
//
//            BOOT JINF PANEL
//
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
//
//            LOAD BOOT PANEL
//
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
//
//            SSF PANEL
//
//
//~============================================================================

void create_SSF_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 30, w3 = 20, h1 = 15;
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "SSF", 'S');
  x = x + w1;

  create_bit_btn_hstring(
    1, x, y, w2, h1, o_o, 'N', &w_SSF, &SSF_BTN[0]);
  obj = add_normal_button(x+w2, y, w3, h1, "W", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, set_SSF_callback, 0);
  y = y + h1;

  create_data_box_hstring(1, x, y, w2, h1, 'N', &SSF_BOX[0]);
  obj = add_normal_button(x+w2, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, get_SSF_callback, 0);
}

//~----------------------------------------------------------------------------

void get_SSF_callback(FL_OBJECT* obj, long data) {

  int16 mode;
  _r *r = &r_SSF;
  
  msg = -1;
  update_objects();

  r->was[0] = r->rea[0];
  
  get_JINF_SSF(&mode, &err);

  if (!err) {
    r->rea[0] = mode;
    if (r->unk) r->was[0] = r->rea[0];
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

void set_SSF_callback(FL_OBJECT* obj, long data) {

  _w *w = &w_SSF;

  msg = -1;
  update_objects();

  set_JINF_SSF(w->inp, &err);
  
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

void update_SSF_panel(void) {

  show_hex_box_string(1, &r_SSF, o_o, &SSF_BOX[0]);
  show_bin_btn_string(1, &w_SSF,      &SSF_BTN[0]);
}

//~============================================================================
//
//
//            DELAY PANEL
//
//
//~============================================================================

void create_delay_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 35, w2 = 15, w3 = 20, h1 = 15;
  
  w = 5 + w1 + 2 * w2 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, 2*h1, "Delay\n20ns", 'S');
  x = x + w1;

  create_hex_btn_hstring(2, x, y, w2, h1, hex, 'N', &w_delay, &DELAY_HEX_BTN[0]);
  obj = add_normal_button(x+2*w2, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, set_delay_callback, 0);
  y = y + h1;

  create_data_box_hstring(2, x, y, w2, h1, 'N', &DELAY_HEX_BOX[0]);
  obj = add_normal_button(x+2*w2, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, get_delay_callback, 0);
}

//~----------------------------------------------------------------------------

void get_delay_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_delay;
  int8 delay;
  
  msg = -1;
  update_objects();

  *r->was = *r->rea;
  
  get_JINF_delay(&delay, &err);

  if (!err) {
    *r->rea = delay;
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

void set_delay_callback(FL_OBJECT* obj, long data) {

  _w *w = &w_delay;
  
  msg = -1;
  update_objects();

  set_JINF_delay(w->inp, &err);
  
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

void update_delay_panel(void) {

  show_hex_box_string(2, &r_delay, NULL, &DELAY_HEX_BOX[0]);
  show_bin_btn_string(2, &w_delay,       &DELAY_HEX_BTN[0]);
}

//~============================================================================
//
//
//            TEST JINF SLAVES PANEL
//
//
//~============================================================================

void create_test_slaves_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, h1 = 30;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;

  obj = add_normal_button(x, y, w1, h1, "SLAVE\nTEST", 'N');
    fl_set_object_callback(obj, test_slaves_callback, 0);
}

//~----------------------------------------------------------------------------

void test_slaves_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  test_JINF_slaves(&err);

  msg = 1;
  update_objects();
}

//~============================================================================
//
//
//            EPPCAN BOX TRIGGER & BUSY PANEL
//
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
//
//            PING JINF PANEL
//
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

//n = 8000;
  n = 10;
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
//
//            PROCESSING MODE PANEL
//
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
//
//            EVENT NUMBER PANEL
//
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
//
//            GET EVENT PANEL
//
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
  
  if (P) {
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
//
//            JINF STATUS PANEL
//
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
//
//            POWER DOWN PANEL
//
//
//~============================================================================

void create_power_down_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, h1 = 30;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;

  obj = add_normal_button(x, y, w1, h1, "POWER\nDOWN", 'N');
    fl_set_object_callback(obj, power_down_callback, 0);
}

//~----------------------------------------------------------------------------

void power_down_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  set_JINF_power_down(&err);
  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//
//            BUSY CONTROL PANEL
//
//
//~============================================================================

void create_busy_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 20, w3 = 15, w4 = 20, h1 = 25, h2 = 15;

  char *txt[24] = {
            "2\n3", "2\n2", "2\n1", "2\n0",
    "1\n9", "1\n8", "1\n7", "1\n6", "1\n5",
    "1\n4", "1\n3", "1\n2", "1\n1", "1\n0",
    " \n9", " \n8", " \n7", " \n6", " \n5",
    " \n4", " \n3", " \n2", " \n1", " \n0"};
  
  w = 5 + w1 + w2 + 24 * w3 + w4 + 5;
  h = 5 + h1 + 4 * h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
//~-- Panel

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Busy", 'N');
  x = x + w1;
  add_frame_box(x, y, w2, h1, "3\n1", 'S');
  x = x + w2;
  create_frame_box_hstring(24, x, y, w3, h1, txt, 'S', 'C');
  x = x + 24 * w3;
  add_frame_box(x, y, w4, h1, "", 'S');
  y = y + h1;

//~-- Busy Mask

  x = x0 + 5;
  obj = add_frame_box(x, y, w1+w2, 2*h2, "Mask", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w1+w2, 2*h1, "");
    fl_set_object_callback(obj, reset_busy_mask_callback, 0);
  x = x + w1 + w2;
  create_bit_btn_hstring(
    24, x, y, w3, h2, hex, 'N', &w_busy_mask, BUSY_MASK_BIT_BTN);
  x = x + 24 * w3;
  obj = add_normal_button(x, y, w4, h2, "W", 'N');
    fl_set_object_callback(obj, write_busy_mask_callback, 0);
  y = y + h2;

  x = x0 + 5;
  x = x + w1 + w2;
  create_data_box_hstring(24, x, y, w3, h2, 'N', BUSY_MASK_BIT_BOX);
  x = x + 24 * w3;
  obj = add_normal_button(x, y, w4, h2, "R", 'N');
    fl_set_object_callback(obj, read_busy_mask_callback, 0);
  y = y + h2;

//~-- Busy Status

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h2, "Status", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  create_data_box_hstring( 1, x, y, w2, h2, 'N', &BUSY_STATUS_SUM_BOX);
  x = x + w2;
  create_data_box_hstring(24, x, y, w3, h2, 'N',  BUSY_STATUS_BIT_BOX);
  x = x + 24 * w3;
  obj = add_normal_button(x, y, w4, h2, "R", 'N');
    fl_set_object_callback(obj, read_busy_status_callback, 0);
  y = y + h2;

//~-- Busy Errors

  x = x0 + 5;
  obj = add_frame_box(x, y, w1+w2, h2, "Errors", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1 + w2;
  create_data_box_hstring(24, x, y, w3, h2, 'N', BUSY_ERRORS_BIT_BOX);
  x = x + 24 * w3;
  obj = add_normal_button(x, y, w4, h2, "R", 'N');
    fl_set_object_callback(obj, read_busy_errors_callback, 0);
}

//~----------------------------------------------------------------------------

void reset_busy_mask_callback(FL_OBJECT* obj, long data) {

  static int set = 0;
  _w *w = &w_busy_mask;

  w->inp = set ? 0xFFFFFF : 0x000000;
  set = 1 - set;

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_busy_mask_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  set_JINF_busy_mask(w_busy_mask.inp, &err);
  
  if (!err) {
    w_busy_mask.wri = w_busy_mask.inp;
    w_busy_mask.unk = FALSE;
  }
  else {
    w_busy_mask.unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_busy_mask_callback(FL_OBJECT* obj, long data) {

  int32 mask;
  
  msg = -1;
  update_objects();

  *r_busy_mask.was = *r_busy_mask.rea;
  
  get_JINF_busy_mask(&mask, &err);

  if (!err) {
    *r_busy_mask.rea = mask;
    if (r_busy_mask.unk) *r_busy_mask.was = *r_busy_mask.rea;
    r_busy_mask.unk = FALSE;
    r_busy_mask.sta = status_OK;
  }
  else {
    r_busy_mask.sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_busy_status_callback(FL_OBJECT* obj, long data) {

  int32 status;
  
  msg = -1;
  update_objects();

  *r_busy_status.was = *r_busy_status.rea;
  *r_busy_sum.was    = *r_busy_sum.rea;
  
  get_JINF_busy_status(&status, &err);

  if (!err) {
    *r_busy_status.rea = status;
    *r_busy_sum.rea    = unpack32(status, 0x80000000);
    if (r_busy_status.unk) *r_busy_status.was = *r_busy_status.rea;
    r_busy_status.unk = FALSE;
    r_busy_status.sta = status_OK;
    if (r_busy_sum.unk) *r_busy_sum.was = *r_busy_sum.rea;
    r_busy_sum.unk = FALSE;
    r_busy_sum.sta = status_OK;
  }
  else {
    r_busy_status.sta = status_KO;
    r_busy_sum.sta    = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_busy_errors_callback(FL_OBJECT* obj, long data) {

  int32 errors;
  
  msg = -1;
  update_objects();

  *r_busy_errors.was = *r_busy_errors.rea;
  
  get_JINF_busy_errors(&errors, &err);

  if (!err) {
    *r_busy_errors.rea = errors;
    if (r_busy_errors.unk) *r_busy_errors.was = *r_busy_errors.rea;
    r_busy_errors.unk = FALSE;
    r_busy_errors.sta = status_OK;
  }
  else {
    r_busy_errors.sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_busy_control_panel(void) {

  show_bin_btn_string(24, &w_busy_mask,          BUSY_MASK_BIT_BTN);
  show_bit_box_string(24, &r_busy_mask,   NULL,  BUSY_MASK_BIT_BOX);

  show_bit_box_string( 1, &r_busy_sum,    NULL, &BUSY_STATUS_SUM_BOX);
  show_bit_box_string(24, &r_busy_status, NULL,  BUSY_STATUS_BIT_BOX);

  show_bit_box_string(24, &r_busy_errors, NULL,  BUSY_ERRORS_BIT_BOX);
}

//~============================================================================
//
//
//            SLAVE MASKS PANEL
//
//
//~============================================================================

void create_slave_mask_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 20, w3 = 15, w4 = 20, h1 = 25, h2 = 15;

  char *txt[24] = {
            "2\n3", "2\n2", "2\n1", "2\n0",
    "1\n9", "1\n8", "1\n7", "1\n6", "1\n5",
    "1\n4", "1\n3", "1\n2", "1\n1", "1\n0",
    " \n9", " \n8", " \n7", " \n6", " \n5",
    " \n4", " \n3", " \n2", " \n1", " \n0"};
  
  w = 5 + w1 + w2 + 24 * w3 + w4 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
//~-- Panel

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1+2*h2, "Slave\nMask", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w1, h1+2*h2, "");
    fl_set_object_callback(obj, reset_slave_mask_callback, 0);
  x = x + w1;
  add_frame_box(x, y, w2, h1, "I\nD", 'S');
  x = x + w2;
  create_frame_box_hstring(24, x, y, w3, h1, txt, 'S', 'C');
  x = x + 24 * w3;
  add_frame_box(x, y, w4, h1, "", 'S');
  y = y + h1;

//~-- Slave Mask

  x = x0 + 5;
  x = x + w1;
  create_hex_btn_hstring(
     1, x, y, w2, h2, hex, 'N', &w_slave_mask_id_set, SLAVE_MASK_ID_SET_BTN);
  SLAVE_MASK_ID_SET_BTN[0]->call = update_callback;
  x = x + w2;
  create_bit_btn_hstring(
    24, x, y, w3, h2, hex, 'N', &w_slave_mask, SLAVE_MASK_BIT_BTN);
  x = x + 24 * w3;
  obj = add_normal_button(x, y, w4, h2, "W", 'N');
    fl_set_object_callback(obj, write_slave_mask_callback, 0);
  y = y + h2;

  x = x0 + 5;
  x = x + w1;
  create_hex_btn_hstring(
     1, x, y, w2, h2, hex, 'N', &w_slave_mask_id_get, SLAVE_MASK_ID_GET_BTN);
  SLAVE_MASK_ID_GET_BTN[0]->call = update_callback;
  x = x + w2;
  create_data_box_hstring(24, x, y, w3, h2, 'N', SLAVE_MASK_BIT_BOX);
  x = x + 24 * w3;
  obj = add_normal_button(x, y, w4, h2, "R", 'N');
    fl_set_object_callback(obj, read_slave_mask_callback, 0);
}

//~----------------------------------------------------------------------------

void reset_slave_mask_callback(FL_OBJECT* obj, long data) {

  static int set = 0;
  _w *w = &w_slave_mask;
  _r *r = &r_slave_mask;

  switch (set) {
    case 0: w->inp =  0x000000;  break;
    case 1: w->inp =  0xFFFFFF;  break;
    case 2: w->inp =  r->rea[0]; break;
    case 3: w->inp = ~r->rea[0]; break;
  }
  
  set = MOD(set + 1, 4);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_slave_mask_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  get_JINF_slave_masks(masks, &err);

  if (!err) {
//int i;
//for (i=0; i<16; i++) printf("%08X ", masks[i]);
//printf("\n");
    r_slave_mask.unk = FALSE;
    r_slave_mask.sta = status_OK;
  }
  else {
    r_slave_mask.sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_slave_mask_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  set_JINF_slave_mask(w_slave_mask_id_set.inp, w_slave_mask.inp, &err);
  
  if (!err) {
    w_slave_mask.wri = w_slave_mask.inp;
    w_slave_mask.unk = FALSE;
    w_slave_mask_id_set.wri = w_slave_mask_id_set.inp;
    w_slave_mask_id_set.unk = FALSE;
  }
  else {
    w_slave_mask.unk = TRUE;
    w_slave_mask_id_set.unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_slave_mask_panel(void) {

  *r_slave_mask.was = *r_slave_mask.rea;
  *r_slave_mask.rea = masks[w_slave_mask_id_get.inp];;
  
  show_bin_btn_string( 1, &w_slave_mask_id_set, SLAVE_MASK_ID_SET_BTN);
  show_bin_btn_string(24, &w_slave_mask,        SLAVE_MASK_BIT_BTN);
  show_bit_box_string(24, &r_slave_mask, NULL,  SLAVE_MASK_BIT_BOX);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  if (!use_TCP) update_path_panel();
  update_boot_panel();
  update_load_boot_panel();

  update_SSF_panel();
  update_delay_panel();
  if (!use_TCP) update_EPPCAN_box_panel();

  update_ping_panel();
  update_proc_mode_panel();
  update_event_nb_panel();
  update_get_event_panel();

  update_status_panel();

  update_busy_control_panel();
  update_slave_mask_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
