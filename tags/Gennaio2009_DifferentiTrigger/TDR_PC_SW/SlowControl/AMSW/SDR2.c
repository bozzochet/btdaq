// file SDR2.c
//
// SDR2 "Pro" Commander & Monitor
//
// A.Lebedev Feb-2007...

#include "template.h"
#include "sdr2def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"SDR2 Controller"};          // global variable
char *date  = {"25-Dec-08"};                // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"

int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct {
  char     t1[80];
  int      x0;
  int      y0;
  SW_BTN  *BTN;
  DAT_BOX *BOX;
  void   (*cnv)(int32 code, int32 *value);
  _r       r;
  _ww      w;
  _b      *b;
  bool     follow;
  int      a;
  char     f[80];
} _object925;

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~========= GENERAL JINF-LIKE THINGS =========

//~-- Boot Panel

SW_BTN *BOOT_ADR_BTN[4];
_w w_boot_adr;

//~-- Load Boot Panel

FL_OBJECT *FILE_HEADER_BOX[8];
_r r_file_header;

//~-- Ping Panel

FL_OBJECT *PING_STA_BOX[1];
_r r_ping_sta;

//~-- EPPCAN Box Trigger & Busy Panel

SW_BTN    *EPP_TRIG_MSK_BTN[2];
_w w_epp_trig_msk;
FL_OBJECT *EPP_BUSY_STA_BOX[2];
_r r_epp_busy_sta;

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

//~-- SDR2 Status Panel

FL_OBJECT *JINF_STA_BOX[18];
_r r_jinf_sta;

//~========= SDR2 BOARD STUFF =========

//~-- "all" SDR2 Internal Registers

int n_reg = 14;
_r  r_reg;
_ww ww_reg;

//~-- Programmable Dead Time Panel

_object925 DEAD_TIME;

//~-- SC Timeout

DAT_BOX *SC_TIM_BOX[6];
_r r_sc_tim;

//~-- SC Status

DAT_BOX *SC_STA_BOX[7];
_r r_sc_sta;

//~-- SQ Timeout

DAT_BOX *SQ_TIM_BOX[15];
_r r_sq_tim;

//~-- Power Control

SW_BTN  *PWR_MSK_BTN[8];
SW_BTN  *PWR_CTL_BTN[8];
SW_BTN  *PWR_MON_BTN[8];
DAT_BOX *PWR_MSK_BOX[8];
DAT_BOX *PWR_CTL_BOX[8];
DAT_BOX *PWR_MON_BOX[8];
DAT_BOX *PWR_STA_BOX[8];
_r r_pwr_msk;
_r r_pwr_ctl;
_r r_pwr_sta;
_ww ww_pwr_msk;
_ww ww_pwr_ctl;

//~-- Actel Version ID

DAT_BOX *VER_BOX[8];
_r r_ver[8];

//~-- "Other" Registers

_object925 S_H_DELAY;
_object925 ADC_DELAY;
_object925 ADC_CLOCK;
_object925 SFET_TIMEOUT;
_object925 MAX_EVENT_SIZE;
_object925 EVENT_SIZE;

//~-- SFEC Readout Control

SW_BTN  *SFEC_BTN;
DAT_BOX *SFEC_BOX;
_r   r_sfec;
_ww  w_sfec;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

char *even[10] = {"0", NULL, "2", NULL, "4", NULL, "6", NULL, "8", NULL};
char *mod[4] = {"Off", "Raw", "Comp", "Raw+Comp"};
char *got[2] = {"Empty", "Got It"};
char *o_o[2] = {"OFF", "ON"};
char *e_d[2] = {"E", "D"};
char *d_e[2] = {"D", "E"};
char *ena_dis[2] = {"ENA", "DIS"};
char *dis_ena[2] = {"DIS", "ENA"};
char *fai[2] = {"FAIL", "O.K."};
char *pwr[4] = {"OFF", "ON", "TRIP", "?"};
char *gates[4] = {"0.25s", "0.5s", "1.0s", "2.0s"};
char *disable_enable[2] = {"DISABLE", "ENABLE"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~-- sometime this section will go to the template library

void set_callback(FL_OBJECT *obj, long data);
void write_callback(FL_OBJECT* obj, long data);
void read_callback(FL_OBJECT* obj, long data);

void create_object925_1(_object925 *object925, int *xx, int *y);
void create_object925_2(_object925 *object925, int *xx, int *y);
void update_object925(_object925 *object925);

//~--

void create_boot_panel(int x0, int y0);
void boot_callback(FL_OBJECT* obj, long data);
void update_boot_panel(void);

void create_load_boot_panel(int x0, int y0);
void load_boot_callback(FL_OBJECT* obj, long data);
void read_FLASH_summary_callback(FL_OBJECT* obj, long data);
void update_load_boot_panel(void);

//~--

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

void create_dead_time_panel(int x0, int y0);
void convert_dead_time_code_to_float(int32 code, int32 *value);
void update_dead_time_panel(void);

void create_SC_timeout_panel(int x0, int y0);
void read_SC_timeout_callback(FL_OBJECT* obj, long data);
void update_SC_timeout_panel(void);

void create_SC_status_panel(int x0, int y0);
void read_SC_status_callback(FL_OBJECT* obj, long data);
void update_SC_status_panel(void);

void create_SQ_timeout_panel(int x0, int y0);
void read_SQ_timeout_callback(FL_OBJECT* obj, long data);
void update_SQ_timeout_panel(void);

void create_power_control_panel(int x0, int y0);
void update_power_parity(int32 data);
void write_power_control_callback(FL_OBJECT* obj, long data);
void read_power_control_callback(FL_OBJECT* obj, long data);
void update_power_control_panel(void);

void create_actel_version_panel(int x0, int y0);
void read_actel_version_callback(FL_OBJECT* obj, long data);
void update_actel_version_panel(void);

//~--

void create_other_registers_panel(int x0, int y0);
void convert_S_H_delay_code_to_float(int32 code, int32 *value);
void convert_ADC_delay_code_to_float(int32 code, int32 *value);
void convert_SFET_timeout_code_to_float(int32 code, int32 *value);
void update_other_registers_panel(void);

//~--

void create_SFEC_readout_control_panel(int x0, int y0);
void write_SFEC_readout_control_callback(FL_OBJECT* obj, long data);
void read_SFEC_readout_status_callback(FL_OBJECT* obj, long data);
void update_SFEC_readout_control_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 595, HH = 530;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "SDR2 Path");
//AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_boot_panel(0, 100);

  create_load_boot_panel(70, 45);

  create_power_down_panel(240, 45);
  create_dead_time_panel(310, 45);

  if (!use_TCP) create_EPPCAN_box_panel(485, 45);

  create_ping_panel(     235,  85);
  create_proc_mode_panel(285,  85);
  create_event_nb_panel( 385,  85);
  create_get_event_panel(535,  85);

  create_status_panel(0, 140);

  create_SC_timeout_panel(0, 195);
  create_SC_status_panel( 0, 350);

  create_SQ_timeout_panel(85, 195);
  
  create_power_control_panel(165, 195);
  create_actel_version_panel(385, 195);

  create_other_registers_panel(165, 380);

  create_SFEC_readout_control_panel(425, 390);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

//~-- "all registers" initialization must be the first

  invalidate_r(first, &r_reg, n_reg);
  invalidate_ww(first, &ww_reg, n_reg, 0x0000);

  invalidate_r(first, &r_file_header, 8);
  
  invalidate_w(&w_boot_adr, 1, 0x0000);

  invalidate_r(first, &r_ping_sta, 1);
  
  invalidate_r(first, &r_epp_busy_sta, 1);
  invalidate_w(&w_epp_trig_msk, 1, 0x0000);
  
  invalidate_r(first, &r_proc_mode, 1);
  invalidate_w(&w_proc_mode, 1, 0x0000);
  
  invalidate_r(first, &r_event_nb, 1);
  
  invalidate_r(first, &r_get_event, 1);
  
  invalidate_r(first, &r_jinf_sta, 18);

  invalidate_r(first,  &DEAD_TIME.r, 1);
  invalidate_ww(first, &DEAD_TIME.w, 1, 0x0000);

  invalidate_r(first, &r_sc_tim, 1);

  invalidate_r(first, &r_sc_sta, 1);

  invalidate_r(first, &r_sq_tim, 1);

  invalidate_r(first, &r_pwr_msk, 1);
  invalidate_r(first, &r_pwr_ctl, 1);
  invalidate_r(first, &r_pwr_sta, 1);
  invalidate_ww(first, &ww_pwr_msk, 1, 0x0000);
  invalidate_ww(first, &ww_pwr_ctl, 1, 0x0000);

  for (i=0; i<8; i++) invalidate_r(first, &r_ver[i], 1);

  invalidate_r(first,  &S_H_DELAY.r, 1);
  invalidate_ww(first, &S_H_DELAY.w, 1, 0x0000);

  invalidate_r(first,  &ADC_DELAY.r, 1);
  invalidate_ww(first, &ADC_DELAY.w, 1, 0x0000);
  
  invalidate_r(first,  &ADC_CLOCK.r, 1);
  invalidate_ww(first, &ADC_CLOCK.w, 1, 0x0000);
  
  invalidate_r(first,  &SFET_TIMEOUT.r, 1);
  invalidate_ww(first, &SFET_TIMEOUT.w, 1, 0x0000);
  
  invalidate_r(first,  &MAX_EVENT_SIZE.r, 1);
  invalidate_ww(first, &MAX_EVENT_SIZE.w, 1, 0x0000);
  
  invalidate_r(first, &r_sfec, 1);
  invalidate_ww(first, &w_sfec, 1, 0x0000);

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
    fread(&w_boot_adr.inp,      4,     1, file);
    fread(&w_epp_trig_msk.inp,  4,     1, file);
    fread(&w_proc_mode.inp,     4,     1, file);
    fread(ww_reg.inp,           4, n_reg, file);
    fread(DEAD_TIME.w.inp,      4,     1, file);
    fread(ww_pwr_msk.inp,       4,     1, file);
    fread(ww_pwr_ctl.inp,       4,     1, file);
    fread(S_H_DELAY.w.inp,      4,     1, file);
    fread(ADC_DELAY.w.inp,      4,     1, file);
    fread(ADC_CLOCK.w.inp,      4,     1, file);
    fread(SFET_TIMEOUT.w.inp,   4,     1, file);
    fread(MAX_EVENT_SIZE.w.inp, 4,     1, file);
    fread(w_sfec.inp,           4,     1, file);
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
    fwrite(&w_boot_adr.inp,      4,     1, file);
    fwrite(&w_epp_trig_msk.inp,  4,     1, file);
    fwrite(&w_proc_mode.inp,     4,     1, file);
    fwrite(ww_reg.inp,           4, n_reg, file);
    fwrite(DEAD_TIME.w.inp,      4,     1, file);
    fwrite(ww_pwr_msk.inp,       4,     1, file);
    fwrite(ww_pwr_ctl.inp,       4,     1, file);
    fwrite(S_H_DELAY.w.inp,      4,     1, file);
    fwrite(ADC_DELAY.w.inp,      4,     1, file);
    fwrite(ADC_CLOCK.w.inp,      4,     1, file);
    fwrite(SFET_TIMEOUT.w.inp,   4,     1, file);
    fwrite(MAX_EVENT_SIZE.w.inp, 4,     1, file);
    fwrite(w_sfec.inp,           4,     1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            TEMPLATE LIBRARY
//
//~============================================================================

void set_callback(FL_OBJECT *obj, long data) {

  _object925 *o = (_object925*)data;
  _b  *m =  o->b;
  _ww *w = &o->w;
  int  d =  atoi(fl_get_object_label(obj));
  
  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;

  pack32(&w->inp[0], m->msk, MOD(w->inp[0]+d, m->n));
  w->sta = status_NA;

  if (o->follow) {
    msg = -1;
    update_objects();
    write_callback(NULL, (long)o);
    msg = 1;
  }

  update_objects();
}

//~----------------------------------------------------------------------------

void write_callback(FL_OBJECT* obj, long data) {

  _object925 *o = (_object925*)data;
  _r  *r = &o->r;
  _ww *w = &o->w;
  int  a =  o->a;

  if (obj) {
    msg = 0;
    if (o->follow) {
      set_normal_button(obj, 0);
      o->follow = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        o->follow = TRUE;
        update_objects();
        return;
      }
    }
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(WRITE, SDR2, a, w->inp[0], NULL, NULL, &err);
    if (!err) {
      w->wri[0] = w->inp[0];
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

void read_callback(FL_OBJECT* obj, long data) {

  _object925 *o = (_object925*)data;
  _r  *r = &o->r;
  int  a =  o->a;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  exec_SDR2_SC_command(READ, SDR2, a, 0, &rea, NULL, &err);
  if (!err) {
    r->rea[0] = rea;
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

void create_object925_1(_object925 *object925, int *xx, int *yy) {

  int x0 = object925->x0;
  int y0 = object925->y0;
  FL_OBJECT *obj;
  int x, y;
  int w1 = 95, w2 = 45, w3 = 20, w4 = 45, w5 = 20, h1 = 15;

//~--

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, object925->t1, 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_btn_hstring(
    1, x, y, w2, h1, NULL, 'N', &object925->w, object925->b, &object925->BTN);
  object925->BTN->cnv = object925->cnv;
  object925->follow = FALSE;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w2/3, h1, "256");
    fl_set_object_callback(obj, set_callback, (long)object925);
  x = x + w2 / 3;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w2/3, h1, "16");
    fl_set_object_callback(obj, set_callback, (long)object925);
  x = x + w2 / 3;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w2/3, h1, "1");
    fl_set_object_callback(obj, set_callback, (long)object925);
  x = x + w2 / 3;

  obj = add_normal_button(x, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_callback, (long)object925);
  x = x + w3;

  create_dat_box_vstring(
    1, x, y, w4, h1,       'N', &object925->r, object925->b, &object925->BOX);
  object925->BOX->cnv = object925->cnv;
  x = x + w4;

  obj = add_normal_button(x, y, w5, h1, "R", 'N');
    fl_set_object_callback(obj, read_callback, (long)object925);
  x = x + w5;

  if (xx) *xx = x;
  if (yy) *yy = y0 + h1;
}

//~----------------------------------------------------------------------------

void create_object925_2(_object925 *object925, int *xx, int *yy) {

  int x0 = object925->x0;
  int y0 = object925->y0;
  FL_OBJECT *obj;
  int x, y;
  int w1 = 80, w2 = 60, w3 = 20, h1 = 15;

//~--

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, 2*h1, object925->t1, 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;

  create_btn_hstring(
    1, x, y, w2, h1, NULL, 'N', &object925->w, object925->b, &object925->BTN);
  object925->BTN->cnv = object925->cnv;
  object925->follow = FALSE;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w2/4, h1, "500");
    fl_set_object_callback(obj, set_callback, (long)object925);
  x = x + w2 / 4;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w2/4, h1, "50");
    fl_set_object_callback(obj, set_callback, (long)object925);
  x = x + w2 / 4;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w2/4, h1, "5");
    fl_set_object_callback(obj, set_callback, (long)object925);
  x = x + w2 / 4;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w2/4, h1, "1");
    fl_set_object_callback(obj, set_callback, (long)object925);
  x = x + w2 / 4;

  obj = add_normal_button(x, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_callback, (long)object925);
  y = y + h1;

  x = x0 + 5 + w1;
  create_dat_box_vstring(
    1, x, y, w2, h1,       'N', &object925->r, object925->b, &object925->BOX);
  object925->BOX->cnv = object925->cnv;
  x = x + w2;

  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_callback, (long)object925);
  x = x + w3;

  if (xx) *xx = x;
  if (yy) *yy = y0 + 2 * h1;
}

//~-----------------------------------------------------------------------------

void update_object925(_object925 *object925) {

  show_btn_string(    1,       object925->f, &object925->BTN);
  show_dat_box_string(1, NULL, object925->f, &object925->BOX);
}

//~============================================================================
//
//            BOOT PANEL
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
//            PROGRAMMABLE DEAD TIME PANEL
//
//~============================================================================

extern int16 dead_time_adr;

extern _b dead_time;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_dead_time_panel(int x0, int y0) {

  int y = y0;
  int w = 170, h = 40;

//~--

  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  strcpy(DEAD_TIME.t1, "Programmable\nDead Time us");
  strcpy(DEAD_TIME.f,  "%.2f");

  DEAD_TIME.x0  = x0;
  DEAD_TIME.y0  = y;
  DEAD_TIME.b   = &dead_time;
  DEAD_TIME.a   = dead_time_adr;
  DEAD_TIME.cnv = convert_dead_time_code_to_float;

  create_object925_2(&DEAD_TIME, NULL, NULL);
}

//~----------------------------------------------------------------------------

void convert_dead_time_code_to_float(int32 code, int32 *value) {

  float tmp = code * 20 / 1000.0;
  *value = *(int32*)&tmp;
}

//~----------------------------------------------------------------------------

void update_dead_time_panel(void) {

  update_object925(&DEAD_TIME);
}

//~============================================================================
//
//            POWER DOWN PANEL
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
  
  obj = add_normal_button(x, y, w1, h1, "PING", 'N');
    fl_set_object_callback(obj, ping_callback, 0);
  y = y + h1;

  create_data_box_hstring(1, x, y, w1, h2, 'N', &PING_STA_BOX[0]);
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
//            SDR2 STATUS PANEL
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
      add_frame_box(x, y, q->wa, h1*q->ha, q->ta, 'S');
      if (!q->wb) {
        JINF_STA_BOX[i] =
        add_data_box( x, y+h1*q->ha, q->wa, h1, "", 'N');
        x = x + q->wa;
      }
    }
    if (q->wb) {
      add_frame_box(x, y+h1*q->ha, q->wb, h1*q->hb, q->tb, 'S');
      JINF_STA_BOX[i] =
      add_data_box( x, y+h1*(q->ha+q->hb), q->wb, h1, "", 'N');
      x = x + q->wb;
    }
  }

  obj = add_normal_button(x, y, w1, 3*h1, "GET\nSTATUS", 'N');
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
//            SLOW CONTROL (SC) TIMEOUT PANEL
//
//~============================================================================

extern int16 sc_tim_adr;

extern _b sc_tim[6];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_SC_timeout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 60, w2 = 15, h1 = 20, h2 = 15, h3 = 20;

//~--

  char *txt[6] = {
  "SFET2-A", "SFET2-B", "SPT2", "SFET2-C", "SFET2-D", "SFEA2"};
  
//~--

  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 6 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1+w2, h1, "SC Tmout", 'N');
  y = y + h1;

//~--

  add_frame_box(x, y, w1+w2, h2, "TOF-wire", 'S');
  y = y + h2;
  create_frame_box_vstring(6, x, y, w1, h2, txt, 'S', 'L');
  create_dat_box_vstring(
    6, x+w1, y, w2, h2, 'N', &r_sc_tim, &sc_tim[0], &SC_TIM_BOX[0]);
  y = y + 6 * h2;
  obj = add_normal_button(x, y, w1+w2, h3, "READ", 'N');
    fl_set_object_callback(obj, read_SC_timeout_callback, 0);
}

//~----------------------------------------------------------------------------

void read_SC_timeout_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_sc_tim;
  int16 rea;
  
  r_sc_sta.sta = status_NA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  *r->was = *r->rea;
  
  exec_SDR2_SC_command(READ, SDR2, sc_tim_adr, 0, &rea, NULL, &err);

  if (!err) {
    *r->rea = rea;
    if (r->unk) *r->was = *r->rea;
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

void update_SC_timeout_panel(void) {

  show_dat_box_string(6, hex, NULL, &SC_TIM_BOX[0]);
}

//~============================================================================
//
//            SLOW CONTROL (SC) STATUS PANEL
//
//~============================================================================

extern int16 sc_sta_adr;

extern _b sc_sta[7];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_SC_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 60, w2 = 15, h1 = 20, h2 = 15, h3 = 15;

//~--

  char *txt[7] = {
  "Cmd pend",  "Data pend",  "Data trunc", "Pwr parity", 
  "Pwr error", "SC tmout", "Seq tmout"};
  
//~--

  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + 7 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1+w2, h1, "SC Status", 'N');
  y = y + h1;

//~--

  create_frame_box_vstring(7, x, y, w1, h2, txt, 'S', 'L');
  create_dat_box_vstring(
    7, x+w1, y, w2, h2, 'N', &r_sc_sta, &sc_sta[0], &SC_STA_BOX[0]);
  y = y + 7 * h2;
  obj = add_normal_button(x, y, w1+w2, h3, "READ", 'N');
    fl_set_object_callback(obj, read_SC_status_callback, 0);
}

//~----------------------------------------------------------------------------

void read_SC_status_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_sc_sta;
  int16 sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  *r->was = *r->rea;
  
  exec_SDR2_SC_command(READ, SDR2, sc_sta_adr, 0, NULL, &sta, &err);

  if (!err) {
    *r->rea = sta;
    if (r->unk) *r->was = *r->rea;
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

void update_SC_status_panel(void) {

  show_dat_box_string(7, hex, NULL, &SC_STA_BOX[0]);
}

//~============================================================================
//
//            SEQUENCER (SQ) TIMEOUT PANEL
//
//~============================================================================

extern int16 sq_tim_adr;

extern _b sq_tim[15];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_SQ_timeout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 15, h1 = 20, h2 = 15, h3 = 20;

//~--

  char *txt1[6] = {
  "SFET2-A", "SFET2-B", "SPT2", "SFET2-C", "SFET2-D", "SFEA2"};
  char *txt2[9] = {
  "SFET2-A", "SFET2-B", "SFET2-C", "SFET2-D", "SFEA2",
  "SFEC0-0", "SFEC0-1", "SFEC1-0", "SFEC1-1"};
  
//~--

  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 6 * h2 + h2 + 9 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2, h1, "Seq Tmout", 'N');
  y = y + h1;

//~--

  add_frame_box(x, y, w1+w2, h2, "TOF-wire", 'S');
  y = y + h2;
  create_frame_box_vstring(6, x, y, w1, h2, txt1, 'S', 'L');
  create_dat_box_vstring(
    6, x+w1, y, w2, h2, 'N', &r_sq_tim, &sq_tim[0], &SQ_TIM_BOX[0]);
  y = y + 6 * h2;
  add_frame_box(x, y, w1+w2, h2, "Charge link", 'S');
  y = y + h2;
  create_frame_box_vstring(9, x, y, w1, h2, txt2, 'S', 'L');
  create_dat_box_vstring(
    9, x+w1, y, w2, h2, 'N', &r_sq_tim, &sq_tim[6], &SQ_TIM_BOX[6]);
  y = y + 9 * h2;
  obj = add_normal_button(x, y, w1+w2, h3, "READ", 'N');
    fl_set_object_callback(obj, read_SQ_timeout_callback, 0);
}

//~----------------------------------------------------------------------------

void read_SQ_timeout_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_sq_tim;
  int16 rea;
  
  r_sc_sta.sta = status_NA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  *r->was = *r->rea;
  
  exec_SDR2_SC_command(READ, SDR2, sq_tim_adr, 0, &rea, NULL, &err);

  if (!err) {
    *r->rea = rea;
    if (r->unk) *r->was = *r->rea;
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

void update_SQ_timeout_panel(void) {

  show_dat_box_string(15, hex, NULL, &SQ_TIM_BOX[0]);
}

//~============================================================================
//
//            POWER CONTROL PANEL
//
//~============================================================================

extern int16 pwr_ctl_adr;
extern int16 pwr_msk_adr;

extern _b pwr_ctl[8];
extern _b pwr_msk[8];
extern _b pwr_mon[8];
extern _b pwr_sta[8];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_power_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 55, w2 = 15, w3 = 30, w4 = 35, h1 = 20, h2 = 15;

//~--

  char *txt[8] = {
  "SFET2-A", "SFET2-B", "SPT2",  "SFET2-C",
  "SFET2-D", "SFEA2",   "SFEC",  "Parity"};
  char *Cmd[1] = {"Cmd"};
  
//~--

  w = 5 + w1 + 2 * (w2 + w3) + w3 + w4 + 5;
  h = 5 + h1 + 9 * h2 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w-10, h1, "Power Control", 'N');
  y = y + h1;

  add_frame_box(x, y, w1, h2, "", 'S');
  x = x + w1;
  add_frame_box(x, y, w2, h2, "M", 'S');
  x = x + w2;
  add_frame_box(x, y, w3, h2, "Ctrl", 'S');
  x = x + w3;
  add_frame_box(x, y, w2, h2, "M", 'S');
  x = x + w2;
  add_frame_box(x, y, w3, h2, "Ctrl", 'S');
  x = x + w3;
  add_frame_box(x, y, w3, h2, "Mon", 'S');
  x = x + w3;
  add_frame_box(x, y, w4, h2, "Stat", 'S');
  y = y + h2;

  x = x0 + 5;
  create_frame_box_vstring(8, x, y,      w1, h2, txt, 'S', 'L');
  create_frame_box_vstring(1, x, y+8*h2, w1, h1, Cmd, 'S', 'R');
  x = x + w1;

  create_btn_vstring(
    7, x, y, w2, h2, e_d, 'N', &ww_pwr_msk, pwr_msk, &PWR_MSK_BTN[0]);
  add_frame_box(x, y+7*h2, w2, h2, "", 'N');
  obj = add_normal_button(x, y+8*h2, w2+w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_power_control_callback, 0);
  x = x + w2;

  create_btn_vstring(
    7, x, y, w3, h2, o_o, 'N', &ww_pwr_ctl, pwr_ctl, &PWR_CTL_BTN[0]);
  for (i=0; i<7; i++) PWR_CTL_BTN[i]->call = update_power_parity;
  create_btn_vstring(
    1, x, y+7*h2, w3, h2, hex, 'N', &ww_pwr_ctl, &pwr_ctl[7], &PWR_CTL_BTN[7]);
  x = x + w3;

  create_dat_box_vstring(
    7, x, y, w2, h2, 'N', &r_pwr_msk, pwr_msk, &PWR_MSK_BOX[0]);
  add_frame_box(x, y+7*h2, w2, h2, "", 'N');
  obj = add_normal_button(x, y+8*h2, w2+w3+w3+w4, h1, "R", 'N');
    fl_set_object_callback(obj, read_power_control_callback, 0);
  x = x + w2;
  
  create_dat_box_vstring(
    8, x, y, w3, h2, 'N', &r_pwr_ctl, pwr_ctl, &PWR_CTL_BOX[0]);
  x = x + w3;
  create_dat_box_vstring(
    8, x, y, w3, h2, 'N', &r_pwr_ctl, pwr_mon, &PWR_MON_BOX[0]);
  x = x + w3;
  create_dat_box_vstring(
    8, x, y, w4, h2, 'N', &r_pwr_sta, pwr_sta, &PWR_STA_BOX[0]);
}

//~----------------------------------------------------------------------------

void update_power_parity(int32 data) {

  int16 m, p;
  _ww *ww = &ww_pwr_ctl;
  
  for (m=0x0080,p=0; m; m>>=1) if (ww->inp[0] & m) p ^= 1;
  if (p) ww->inp[0] ^= 0x0080;

  SW_BTN_CALLBACK(0);
}

//~----------------------------------------------------------------------------

void write_power_control_callback(FL_OBJECT* obj, long data) {

  if (obj) {
    msg = -1;
    update_objects();
  }

  r_pwr_sta.sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    _ww *ww = &ww_pwr_msk;
    _r *r = &r_pwr_msk;
    r->sta = status_NA;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_msk_adr, ww->inp[0], NULL, NULL, &err);
    if (!err) {
      ww->wri[0] = ww->inp[0];
      ww->unk = FALSE;
    }
    else {
      ww->unk = TRUE;
    }
  }
  
  if (!err) {
    _ww *ww = &ww_pwr_ctl;
    _r *r = &r_pwr_ctl;
    r->sta = status_NA;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_ctl_adr, ww->inp[0], NULL, NULL, &err);
    if (!err) {
      ww->wri[0] = ww->inp[0];
      ww->unk = FALSE;
    }
    else {
      ww->unk = TRUE;
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_power_control_callback(FL_OBJECT* obj, long data) {

  int16 rea;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;
  
  if (!err) {
    _r *r = &r_pwr_msk;
    r->was[0] = r->rea[0];
    exec_SDR2_SC_command(READ, SDR2, pwr_msk_adr, 0, &rea, NULL, &err);
    if (!err) {
      r->rea[0] = rea;
      if (r->unk) r->was[0] = r->rea[0];
      r->unk = FALSE;
      r->sta = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }

  if (!err) {
    _r *r = &r_pwr_ctl;
    r->was[0] = r->rea[0];
    exec_SDR2_SC_command(READ, SDR2, pwr_ctl_adr, 0, &rea, NULL, &err);
    if (!err) {
      r->rea[0] = rea;
      if (r->unk) r->was[0] = r->rea[0];
      r->unk = FALSE;
      r->sta = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }

  if (TRUE) {
    int i;
    _r *r = &r_pwr_sta;
    r->was[0] = r->rea[0];
    if (!err) {
      r->rea[0] = 0x0000;
      for (i=0; i<7; i++) {
        bool ctl = unpack32(r_pwr_ctl.rea[0], pwr_ctl[i].msk);
        bool mon = unpack32(r_pwr_ctl.rea[0], pwr_mon[i].msk);
        if (ctl) {
          if (mon) pack32(r->rea, pwr_sta[i].msk, 1);  // ON
          else     pack32(r->rea, pwr_sta[i].msk, 2);  // TRIP
        }
        else {
          if (mon) pack32(r->rea, pwr_sta[i].msk, 3);  // ?
          else     pack32(r->rea, pwr_sta[i].msk, 0);  // OFF
        }
      }
      if (r->unk) r->was[0] = r->rea[0];
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

void update_power_control_panel(void) {

  int i;
  int16 m, p;
  _ww *ww = &ww_pwr_ctl;
  
  show_btn_string(7, NULL, &PWR_MSK_BTN[0]);
  show_btn_string(7, NULL, &PWR_CTL_BTN[0]);

  show_btn_string(1, NULL, &PWR_CTL_BTN[7]);
  for (m=0x0080,p=0; m; m>>=1) if (ww->inp[0] & m) p ^= 1;
  if (p) {
    SW_BTN *B = PWR_CTL_BTN[7];
    fl_set_object_color(B->obj, FL_RED, 0);
    fl_set_object_lcolor(B->obj, FL_YELLOW);
  }

  show_dat_box_string(7, e_d, NULL, &PWR_MSK_BOX[0]);
  show_dat_box_string(7, o_o, NULL, &PWR_CTL_BOX[0]);
  show_dat_box_string(1, hex, NULL, &PWR_CTL_BOX[7]);
  show_dat_box_string(7, o_o, NULL, &PWR_MON_BOX[0]);
  show_dat_box_string(1, hex, NULL, &PWR_MON_BOX[7]);

  show_dat_box_string(7, pwr, NULL, &PWR_STA_BOX[0]);

  for (i=0; i<7; i++) {
    DAT_BOX *B = PWR_STA_BOX[i];
    _r *r = &r_pwr_sta;
    if ((r->sta == status_OK) && 
        !r->unk               && 
        (unpack32(r->rea[0], pwr_sta[i].msk) > 1)) {
      fl_set_object_color( B->obj, FL_RED, 0);
      fl_set_object_lcolor(B->obj, FL_YELLOW);
    }
  }
}

//~============================================================================
//
//            ACTEL VERSION ID PANEL
//
//~============================================================================

extern int16  ver_adr[8];
extern _b ver_msk;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_actel_version_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 55, w2 = 35, w3 = 20, h1 = 20, h2 = 15, h3 = 35;

//~--

  char *txt[8] = {
  "SDR2",    "SFET2-A", "SFET2-B", "SPT2",  
  "SFET2-C", "SFET2-D", "SFEA2",   "SFEC"};
  
//~--

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 8 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1+w2+w3, h1, "Actel Version ID", 'N');
  y = y + h1;

  x = x0 + 5;
  create_frame_box_vstring(8, x, y,      w1, h2, txt, 'S', 'L');

  for (i=0; i<8; i++) {
    create_dat_box_vstring(
      1, x+w1, y, w2, h2, 'N', &r_ver[i], &ver_msk, &VER_BOX[i]);
    obj = add_normal_button(x+w1+w2, y, w3, h2, "R", 'N');
      fl_set_object_callback(obj, read_actel_version_callback, i);
    y = y + h2;
  }

  obj = add_normal_button(x, y, w1+w2+w3, h3, "READ", 'N');
    fl_set_object_callback(obj, read_actel_version_callback, 99);
}

//~----------------------------------------------------------------------------

void read_actel_version_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;
  
  for (i=0; i<8; i++) {
    _r *r = &r_ver[i];
    if (ind != 99 && ind != i) continue;
    r->was[0] = r->rea[0];
    if (go_on(err)) {
      exec_SDR2_SC_command(READ, i, ver_adr[i], 0, &rea, NULL, &err);
      if (!err) {
        r->rea[0] = rea;
        if (r->unk) r->was[0] = r->rea[0];
        r->unk = FALSE;
        r->sta = status_OK;
      }
      else {
        r->sta = status_KO;
      }
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_actel_version_panel(void) {

  int i;
  
  for (i=0; i<8; i++) {
    show_dat_box_string(1, NULL, "%04X", &VER_BOX[i]);
  }
}

//~============================================================================
//
//            "OTHER" REGISTERS PANEL
//
//~============================================================================

extern int16 s_h_delay_adr;
extern int16 adc_delay_adr;
extern int16 adc_clock_adr;
extern int16 sfet_timeout_adr;
extern int16 event_max_size_adr;

extern _b s_h_delay;
extern _b adc_delay;
extern _b adc_clock;
extern _b sfet_timeout;
extern _b event_max_size;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_other_registers_panel(int x0, int y0) {

  int y = y0;
  int w = 235, h = 100;

//~--

  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  strcpy(S_H_DELAY.t1, "S/H Delay us");
  strcpy(S_H_DELAY.f,  "%.2f");

  S_H_DELAY.x0  = x0;
  S_H_DELAY.y0  = y;
  S_H_DELAY.b   = &s_h_delay;
  S_H_DELAY.a   = s_h_delay_adr;
  S_H_DELAY.cnv = convert_S_H_delay_code_to_float;

  create_object925_1(&S_H_DELAY, NULL, &y);

//~--

  strcpy(ADC_DELAY.t1, "ADC Delay us");
  strcpy(ADC_DELAY.f,  "%.2f");

  ADC_DELAY.x0  = x0;
  ADC_DELAY.y0  = y;
  ADC_DELAY.b   = &adc_delay;
  ADC_DELAY.a   = adc_delay_adr;
  ADC_DELAY.cnv = convert_ADC_delay_code_to_float;

  create_object925_1(&ADC_DELAY, NULL, &y);

//~--

  strcpy(ADC_CLOCK.t1, "ADC Clock");
  strcpy(ADC_CLOCK.f,  "%4d");

  ADC_CLOCK.x0  = x0;
  ADC_CLOCK.y0  = y;
  ADC_CLOCK.b   = &adc_clock;
  ADC_CLOCK.a   = adc_clock_adr;
  ADC_CLOCK.cnv = NULL;

  create_object925_1(&ADC_CLOCK, NULL, &y);

//~--

  strcpy(SFET_TIMEOUT.t1, "SFET/A2 Timeout");
  strcpy(SFET_TIMEOUT.f,  "%.2f");

  SFET_TIMEOUT.x0  = x0;
  SFET_TIMEOUT.y0  = y;
  SFET_TIMEOUT.b   = &sfet_timeout;
  SFET_TIMEOUT.a   = sfet_timeout_adr;
  SFET_TIMEOUT.cnv = convert_SFET_timeout_code_to_float;

  create_object925_1(&SFET_TIMEOUT, NULL, &y);

//~--

  strcpy(MAX_EVENT_SIZE.t1, "Max Event Size");
  strcpy(MAX_EVENT_SIZE.f,  "%4d");

  MAX_EVENT_SIZE.x0  = x0;
  MAX_EVENT_SIZE.y0  = y;
  MAX_EVENT_SIZE.b   = &max_event_size;
  MAX_EVENT_SIZE.a   = max_event_size_adr;
  MAX_EVENT_SIZE.cnv = NULL;

  create_object925_1(&MAX_EVENT_SIZE, NULL, &y);

//~--

}

//~----------------------------------------------------------------------------

void convert_S_H_delay_code_to_float(int32 code, int32 *value) {

  float tmp = code * 20 / 1000.0;
  *value = *(int32*)&tmp;
}

//~----------------------------------------------------------------------------

void convert_ADC_delay_code_to_float(int32 code, int32 *value) {

  float tmp = code * 20 / 1000.0;
  *value = *(int32*)&tmp;
}

//~----------------------------------------------------------------------------

void convert_SFET_timeout_code_to_float(int32 code, int32 *value) {

  float tmp = code * 20 / 1000.0;
  *value = *(int32*)&tmp;
}

//~----------------------------------------------------------------------------

void update_other_registers_panel(void) {

  update_object925(&S_H_DELAY);
  update_object925(&ADC_DELAY);
  update_object925(&ADC_CLOCK);
  update_object925(&SFET_TIMEOUT);
  update_object925(&MAX_EVENT_SIZE);
//update_object925(&EVENT_SIZE);
}

//~============================================================================
//
//            SFEC READOUT CONTROL PANEL
//
//~============================================================================

extern int16 sfec_adr;

extern _b sfec;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_SFEC_readout_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 55, h1 = 25, h2 = 15;
  
  w = 5 + w1 + 5 + w2 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "SFEC Readout", 'N');
  y = y + h1;
  create_btn_vstring(
    1, x, y, w1, h2, disable_enable, 'N', &w_sfec, &sfec, &SFEC_BTN);
  obj = add_normal_button(x, y+h2, w1, h2, "WRITE", 'N');
    fl_set_object_callback(obj, write_SFEC_readout_control_callback, 0);
  x = x + w1 + 5;
  create_dat_box_vstring(
    1, x, y, w2, h2, 'N', &r_sfec, &sfec, &SFEC_BOX);
  obj = add_normal_button(x, y+h2, w2, h2, "READ", 'N');
    fl_set_object_callback(obj, read_SFEC_readout_status_callback, 0);
}

//~----------------------------------------------------------------------------

void write_SFEC_readout_control_callback(FL_OBJECT* obj, long data) {

  _ww *w = &w_sfec;
  _r  *r = &r_sfec;
  int16 adr = sfec_adr;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(WRITE, SDR2, adr, w->inp[0], NULL, NULL, &err);
    if (!err) {
      w->wri[0] = w->inp[0];
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

void read_SFEC_readout_status_callback(FL_OBJECT* obj, long data) {

  int16 rea;
  _r *r = &r_sfec;
  int16 adr = sfec_adr;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  if (!err) {
    r->was[0] = r->rea[0];
    exec_SDR2_SC_command(READ, SDR2, adr, 0, &rea, NULL, &err);
    if (!err) {
      r->rea[0] = rea;
      if (r->unk) r->was[0] = r->rea[0];
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

void update_SFEC_readout_control_panel(void) {

  show_btn_string(    1,                 NULL, &SFEC_BTN);
  show_dat_box_string(1, disable_enable, NULL, &SFEC_BOX);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_boot_panel();
  update_load_boot_panel();

  if (!use_TCP) update_EPPCAN_box_panel();

  update_ping_panel();
  update_proc_mode_panel();
  update_event_nb_panel();
  update_get_event_panel();

  update_status_panel();

  update_dead_time_panel();

  update_SC_timeout_panel();
  update_SC_status_panel();

  update_SQ_timeout_panel();

  update_power_control_panel();
  update_actel_version_panel();

  update_other_registers_panel();

  update_SFEC_readout_control_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
