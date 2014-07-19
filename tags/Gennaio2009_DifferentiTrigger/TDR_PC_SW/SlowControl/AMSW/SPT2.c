// file SPT2.c
//
// SPT2 "Pro" Controller
//
// A.Lebedev Feb-2007...

#include "template.h"
#include "spt2def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"SPT2 Pro Controller"};          // global variable
char *date  = {"15-Oct-08"};                    // global variable

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

//~-- Power Control (addresses SDR2)

DAT_BOX *PWR_BOX;
_r r_pwr_sta;

//~-- Internal Generator Control

DAT_BOX *GEN_STA_BOX;
SW_BTN  *GEN_PERIOD_BTN[5];
DAT_BOX *GEN_PERIOD_BOX;
_r     r_gen_sta;
_r     r_gen_period;
_ww   ww_gen_period;

//~-- Scaler' Gate Control

SW_BTN  *SCA_GATE_BTN;
DAT_BOX *SCA_GATE_BOX;
_r     r_sca_gate;
_ww   ww_sca_gate;

//~-- Masks

SW_BTN  *MSK_BTN[2][3][12];
DAT_BOX *MSK_BOX[2][3][12];
_r         r_msk[2][3];
_ww       ww_msk[2][3];

//~-- "States"

DAT_BOX *STAT_BOX[2][2][13];
_r         r_stat[2][2];

//~-- Patterns

DAT_BOX *PTRN_BOX[2][6][13];
_r         r_ptrn[2][6];

//~-- Scalers

DAT_BOX *SCA_BOX[2][2][11];
_r         r_sca[2][2][11];

//~-- Output Control (LVDS Drivers Control)

SW_BTN  *LVDS_BTN;
DAT_BOX *LVDS_BOX;
_r     r_lvds;
_ww   ww_lvds;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *e_d[2] = {"E", "D"};
char *dis_ena[2] = {"DISABLE", "ENABLE"};
char *pwr[4] = {"OFF", "ON", "TRIP", "?"};
char *gates[4] = {"0.25s", "0.5s", "1.0s", "2.0s"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_power_control_panel(int x0, int y0);
void power_control_callback(FL_OBJECT* obj, long data);
void read_power_status_callback(FL_OBJECT* obj, long data);
void update_power_control_panel(void);

void create_generator_panel(int x0, int y0);
void generator_control_callback(FL_OBJECT* obj, long data);
void read_generator_status_callback(FL_OBJECT* obj, long data);
void write_generator_period_callback(FL_OBJECT* obj, long data);
void read_generator_period_callback(FL_OBJECT* obj, long data);
void trigger_generator_callback(FL_OBJECT* obj, long data);
void update_generator_panel(void);

void create_scaler_gate_panel(int x0, int y0);
void write_scaler_gate_callback(FL_OBJECT* obj, long data);
void read_scaler_gate_callback(FL_OBJECT* obj, long data);
void update_scaler_gate_panel(void);

void create_registers_and_scalers_panel(int x0, int y0);
void write_mask_callback(FL_OBJECT* obj, long data);
void read_mask_callback(FL_OBJECT* obj, long data);
void read_state_callback(FL_OBJECT* obj, long data);
void read_pattern_callback(FL_OBJECT* obj, long data);
void read_scaler_callback(FL_OBJECT* obj, long data);
void update_registers_and_scalers_panel(void);

//~--

void create_output_control_panel(int x0, int y0);
void write_output_control_callback(FL_OBJECT* obj, long data);
void read_output_status_callback(FL_OBJECT* obj, long data);
void update_output_control_panel(void);

void create_config_file_panel(int x0, int y0);
void write_config_file_callback(FL_OBJECT* obj, long data);
void read_config_file_callback(FL_OBJECT* obj, long data);

void create_read_write_update_panel(int x0, int y0);
void read_all_callback(FL_OBJECT* obj, long data);
void write_all_callback(FL_OBJECT* obj, long data);
void update_input_callback(FL_OBJECT* obj, long data);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 625, HH = 735;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "SDR2 Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_power_control_panel(0, 45);
  create_generator_panel(    0, 70);
  create_scaler_gate_panel(450, 70);

  create_registers_and_scalers_panel(0, 95);

  create_output_control_panel(     0, 625);
  create_config_file_panel(      405, 625);
  create_read_write_update_panel(515, 625);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i, j, k;
  static bool first = TRUE;

  invalidate_r(first, &r_pwr_sta, 1);

  invalidate_r(first, &r_gen_sta,    1);
  invalidate_r(first, &r_gen_period, 1);
  invalidate_ww(first, &ww_gen_period, 1, 0x00000000);

  invalidate_r(first, &r_sca_gate, 1);
  invalidate_ww(first, &ww_sca_gate, 1, 0x0000);

  for (j=0; j<2; j++) {
    for (i=0; i<3; i++) {
      invalidate_r(first,   &r_msk[j][i], 1);
      invalidate_ww(first, &ww_msk[j][i], 1, 0x0000FFFF);
    }
  }

  for (j=0; j<2; j++) {
    for (i=0; i<2; i++) {
      invalidate_r(first, &r_stat[j][i], 1);
    }
  }

  for (j=0; j<2; j++) {
    for (i=0; i<6; i++) {
      invalidate_r(first, &r_ptrn[j][i], 1);
    }
  }

  for (j=0; j<2; j++) {
    for (i=0; i<2; i++) {
      for (k=0; k<11; k++) {
        invalidate_r(first, &r_sca[j][i][k], 1);
      }
    }
  }

  invalidate_r(first, &r_lvds, 1);
  invalidate_ww(first, &ww_lvds, 1, 0x0000);

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
  int i, j;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(ww_gen_period.inp, 4, 1, file);
    fread(ww_sca_gate.inp,   4, 1, file);
    for (j=0; j<2; j++) for (i=0; i<3; i++) fread(ww_msk[j][i].inp, 4, 1, file);
    fread(ww_lvds.inp,       4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i, j;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(ww_gen_period.inp, 4, 1, file);
    fwrite(ww_sca_gate.inp,   4, 1, file);
    for (j=0; j<2; j++) for (i=0; i<3; i++) fwrite(ww_msk[j][i].inp, 4, 1,file);
    fwrite(ww_lvds.inp,       4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            POWER CONTROL PANEL
//
//~============================================================================

extern int16 pwr_ctl_adr;
extern int16 pwr_msk_adr;

extern _b *spt2_pwr_ctl;
extern _b *spt2_pwr_msk;
extern _b *spt2_pwr_mon;

_b spt2_pwr_sta = {0x00, 0x0003, 4};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_power_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 35, w3 = 35, w4 = 20, h1 = 15;
  
  w = 5 + w1 + 2 * w2 + w3 + w4 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Power", 'S');
  x = x + w1;
  obj = add_normal_button(x, y, w2, h1, "ON", 'N');
    fl_set_object_callback(obj, power_control_callback, 1);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "OFF", 'N');
    fl_set_object_callback(obj, power_control_callback, 0);
  x = x + w2;
  create_dat_box_vstring(
    1, x, y, w3, h1, 'N', &r_pwr_sta, &spt2_pwr_sta, &PWR_BOX);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h1, "R", 'N');
    fl_set_object_callback(obj, read_power_status_callback, 0);
  y = y + h1;
}

//~----------------------------------------------------------------------------

void power_control_callback(FL_OBJECT* obj, long data) {

  bool o_o = data;
  _r *r = &r_pwr_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    int16 wri = ~spt2_pwr_msk->msk;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_msk_adr, wri, NULL, NULL, &err);
  }
  
  if (!err) {
    int16 wri = o_o ? spt2_pwr_ctl->msk | 0x80 : 0x00;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_ctl_adr, wri, NULL, NULL, &err);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_power_status_callback(FL_OBJECT* obj, long data) {

  int16 rea;
  int16 r_msk, r_ctl;
  _r *r = &r_pwr_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta  = status_NA;
  *r->was = *r->rea;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(READ, SDR2, pwr_msk_adr, 0, &rea, NULL, &err);
    if (!err) r_msk = rea;
  }

  if (!err) {
    exec_SDR2_SC_command(READ, SDR2, pwr_ctl_adr, 0, &rea, NULL, &err);
    if (!err) r_ctl = rea;
  }

  if (TRUE) {
    if (!err) {
      bool ctl = unpack32(r_ctl, spt2_pwr_ctl->msk);
      bool mon = unpack32(r_ctl, spt2_pwr_mon->msk);
      if (ctl) {
        if (mon) r->rea[0] = 1;    // ON
        else     r->rea[0] = 2;    // TRIP
      }
      else {
        if (mon) r->rea[0] = 3;    // ?
        else     r->rea[0] = 0;    // OFF
      }
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

void update_power_control_panel(void) {

  DAT_BOX *B = PWR_BOX;
  _r *r = &r_pwr_sta;

  show_dat_box_string(1, pwr, NULL, &B);

  if ((r->sta == status_OK) && !r->unk && (r->rea[0] > 1)) {
    fl_set_object_color( B->obj, FL_RED, 0);
    fl_set_object_lcolor(B->obj, FL_YELLOW);
  }
}

//~============================================================================
//
//            INTERNAL GENERATOR CONTROL PANEL
//
//~============================================================================

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

extern int16 gen_control_adr;
extern int16 gen_period_adr;

extern _b gen_sta;
extern _b gen_period[5];

//~----------------------------------------------------------------------------

void create_generator_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int h1 = 15;
  int w1 = 90, w2 = 15, w3 = 20, w4 = 75, w5 = 35, w6 = 30;

  w = 5 + w1 + 5 * w2 + w3 + w4 + w3 + 2 * w5 + w6 + w3 + w5 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Int. Generator", 'S');
  x = x + w1;

  create_btn10_hstring(
    5, x, y, w2, h1, hex, 'N', &ww_gen_period, gen_period, &GEN_PERIOD_BTN[0]);
  x = x + 5 * w2;

  obj = add_normal_button(x, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_generator_period_callback, 0);
  x = x + w3;

  create_dat_box_hstring(
    1, x, y, w4, h1,      'N', &r_gen_period, gen_period, &GEN_PERIOD_BOX);
  x = x + w4;

  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_generator_period_callback, 0);
  x = x + w3;

  obj = add_normal_button(x, y, w5, h1, "ON", 'N');
    fl_set_object_callback(obj, generator_control_callback, 1);
  x = x + w5;

  obj = add_normal_button(x, y, w5, h1, "OFF", 'N');
    fl_set_object_callback(obj, generator_control_callback, 0);
  x = x + w5;

  create_dat_box_hstring(
    1, x, y, w6, h1,      'N', &r_gen_sta, &gen_sta, &GEN_STA_BOX);
  x = x + w6;

  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_generator_status_callback, 0);
  x = x + w3;

  obj = add_normal_button(x, y, w5, h1, "TRIG", 'N');
    fl_set_object_callback(obj, trigger_generator_callback, 0);
}

//~-----------------------------------------------------------------------------

void generator_control_callback(FL_OBJECT* obj, long data) {

  bool o_o = data;
  _r *r = &r_gen_sta;
  int16 adr = gen_ctrl_adr;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    int16 wri = o_o ? START_SPT2_GENERATOR : STOP_SPT2_GENERATOR;
    exec_SDR2_SC_command(WRITE, SPT2, adr, wri, NULL, NULL, &err);
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-----------------------------------------------------------------------------

void read_generator_status_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_gen_sta;
  int16 adr = gen_sta_adr;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  exec_SDR2_SC_command(READ, SPT2, adr, 0, &rea, NULL, &err);
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

//~-----------------------------------------------------------------------------

void write_generator_period_callback(FL_OBJECT* obj, long data) {

  _r  *r  =  &r_gen_period;
  _ww *ww = &ww_gen_period;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(WRITE, SPT2, gen_period_adr, ww->inp[0], NULL, NULL, &err);
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

//~-----------------------------------------------------------------------------

void read_generator_period_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_gen_period;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  exec_SDR2_SC_command(READ, SPT2, gen_period_adr, 0, &rea, NULL, &err);
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

//~-----------------------------------------------------------------------------

void trigger_generator_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_gen_sta;
  int16 adr = gen_ctrl_adr;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    int16 wri = TRIGGER_SPT2_GENERATOR;
    exec_SDR2_SC_command(WRITE, SPT2, adr, wri, NULL, NULL, &err);
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-----------------------------------------------------------------------------

void update_generator_panel(void) {

  show_dat_box_string(1, pwr,    NULL, &GEN_STA_BOX);
  show_btn_string(    5,         NULL, &GEN_PERIOD_BTN[0]);
  show_dat_box_string(1, NULL, "%dus", &GEN_PERIOD_BOX);
}

//~============================================================================
//
//            SCALER' GATE CONTROL PANEL
//
//~============================================================================

extern int16 sca_gate_adr;
extern _b sca_gate;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_scaler_gate_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 35, w2 = 45, w3 = 20, h1 = 15;
  
  w = 5 + w1 + 2 * (w2 + w3) + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Gate", 'S');
  x = x + w1;

  create_btn_vstring(
    1, x, y, w2, h1, gates, 'N', &ww_sca_gate, &sca_gate, &SCA_GATE_BTN);
  x = x + w2;

  obj = add_normal_button(x, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_scaler_gate_callback, 0);
  x = x + w3;

  create_dat_box_vstring(
    1, x, y, w2, h1, 'N', &r_sca_gate, &sca_gate, &SCA_GATE_BOX);
  x = x + w2;
  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_scaler_gate_callback, 0);
}

//~----------------------------------------------------------------------------

void write_scaler_gate_callback(FL_OBJECT* obj, long data) {

  _r  *r  =  &r_sca_gate;
  _ww *ww = &ww_sca_gate;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(WRITE, SPT2, sca_gate_adr, ww->inp[0], NULL, NULL, &err);
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

void read_scaler_gate_callback(FL_OBJECT* obj, long data) {

  int16 rea;
  _r *r = &r_sca_gate;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  if (!err) {
    r->was[0] = r->rea[0];
    exec_SDR2_SC_command(READ, SPT2, sca_gate_adr, 0, &rea, NULL, &err);
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

void update_scaler_gate_panel(void) {

  show_btn_string(    1,        NULL, &SCA_GATE_BTN);
  show_dat_box_string(1, gates, NULL, &SCA_GATE_BOX);
}

//~============================================================================
//
//            REGISTERS (MASKS & PATTERNS) & SCALERS PANEL
//
//~============================================================================

extern int16  msk_adr[2][3];
extern _b sig_msk[12];

extern int16 stat_adr[2][2];
extern _b sig_stat[13];

extern int16 ptrn_adr[2][6];
extern _b sig_ptrn[13];

extern int16 sca_adr[2][2][11];
extern _b sca_msk;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_registers_and_scalers_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, x1, y1;
  
{//~--- The Panel ---

  int w1 = 115, w2 = 105, w3 = 210, w4 = 170, h0 = 530, h1 = 25;

  w = 5 + w1 + 5 + w2 + 5 + w3 + 5 + w4 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h0, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Pretrigger Setup", 'N');
  x = x + w1 + 5;
  add_frame_box(x, y, w2, h1, "\"States\"", 'N');
  x = x + w2 + 5;
  add_frame_box(x, y, w3, h1, "Signal Patterns", 'N');
  x = x + w3 + 5;
  add_frame_box(x, y, w4, h1, "Scalers", 'N');
  y = y + h1;

  x1 = x0 + 5;
  y1 = y;
}

{//~--- Signal Masks Control ---

  int i, j;
  int w1 = 25, w2 = 15, h1 = 30, h2 = 15, h3 = 20;
  char *txt1[2] = {"0", "1"};
  char *txt2[3] = {"CP", "CT", "BZ"};
  char *txt3[12] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Gen", "Ext"};

  y = y1;
  for (j=0; j<2; j++) {
    x = x1;
    add_frame_box(x, y, w1, h1, txt1[j], 'H');
    create_frame_box_hstring(3, x+w1, y, 2*w2, h1, txt2, 'S', 'C');
    y = y + h1;
    create_frame_box_vstring(11, x, y, w1, h2, txt3, 'S', 'C');
    add_frame_box(x, y+11*h2, w1, 2*h2, "Ext", 'S');
    x = x + w1;

    for (i=0; i<3; i++) {
      create_btn_vstring(
        12, x, y, w2, h2, e_d, 'N', &ww_msk[j][i], sig_msk, &MSK_BTN[j][i][0]);
      x = x + w2;
      create_dat_box_vstring(
        12, x, y, w2, h2, 'N', &r_msk[j][i], sig_msk, &MSK_BOX[j][i][0]);
      x = x + w2;
    }

    x = x1;
    y = y + 12 * h2;
    add_frame_box(x+w1, y, 6*w2, h2, "", 'N');
    y = y + h2;
    add_frame_box(x, y, w1, h3, "", 'N');
    x = x + w1;
    for (i=0; i<3; i++) {
      obj = add_normal_button(x, y, w2, h3, "W", 'N');
        fl_set_object_callback(obj, write_mask_callback, 10*j+i);
      x = x + w2;
      obj = add_normal_button(x, y, w2, h3, "R", 'N');
        fl_set_object_callback(obj, read_mask_callback, 10*j+i);
      x = x + w2;
    }
    y = y + h3 + 5;
  }
  x1 = x + 5;
}

{//~--- Input Line States Readout ---

  int j;
  int w1 = 25, w2 = 50, w3 = 30, h1 = 30, h2 = 15, h3 = 20;
  char *txt1[2] = {"0", "1"};
  char *txt2[3] = {"CP", "CT", "BZ"};
  char *txt3[12] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Gen", "Ext"};

  y = y1;
  for (j=0; j<2; j++) {
    x = x1;
    add_frame_box(x, y, w1, h1, txt1[j], 'H');
    x = x + w1;
    add_frame_box(x, y, w2, h1, "HT", 'S');
    x = x + w2;
    add_frame_box(x, y, w3, h1, "SHT", 'S');
    y = y + h1 / 2;
    y = y + h1 / 2;

    x = x1;
    create_frame_box_vstring(11, x, y, w1, h2, txt3, 'S', 'C');
    add_frame_box(x, y+11*h2, w1, 2*h2, "Ext", 'S');
    create_frame_box_hstring(2, x+w1,    y+12*h2, w2/2, h2, &txt2[0], 'S', 'C');
    create_frame_box_hstring(1, x+w1+w2, y+12*h2,   w3, h2, &txt2[2], 'S', 'C');
    x = x + w1;
  
    create_dat_box_vstring(
      10, x, y,         w2, h2, 'N', &r_stat[j][0], &sig_stat[0],  &STAT_BOX[j][0][0]);
    add_frame_box(x, y+10*h2, w2+w3, h2, "", 'S');
    create_dat_box_hstring(
       2, x, y+11*h2, w2/2, h2, 'N', &r_stat[j][0], &sig_stat[10], &STAT_BOX[j][0][10]);
    x = x + w2;

    create_dat_box_vstring(
      10, x, y,         w3, h2, 'N', &r_stat[j][1], &sig_stat[0],  &STAT_BOX[j][1][0]);
    create_dat_box_hstring(
       1, x, y+11*h2,   w3, h2, 'N', &r_stat[j][1], &sig_stat[12], &STAT_BOX[j][1][10]);

    x = x1;
    y = y + 13 * h2;
    add_frame_box(x, y, w1, h3, "", 'N');
    x = x + w1;
    obj = add_normal_button(x, y, w2, h3, "R", 'N');
      fl_set_object_callback(obj, read_state_callback, 10*j+0);
    x = x + w2;
    obj = add_normal_button(x, y, w3, h3, "R", 'N');
      fl_set_object_callback(obj, read_state_callback, 10*j+1);
    x = x + w3;
    y = y + h3 + 5;
  }
  x1 = x + 5;
}

{//~--- Signal Patterns Readout ---

  int i, j;
  int w1 = 25, w2 = 50, w3 = 30, w4 = 25, w5 = 15;
  int h1 = 30, h2 = 15, h3 = 20;
  char *txt1[2] = {"0", "1"};
  char *txt2[3] = {"CP", "CT", "BZ"};
  char *txt3[13] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Gen", "Ext", "Int"};

  y = y1;
  for (j=0; j<2; j++) {
    x = x1;
    add_frame_box(x, y, w1, h1, txt1[j], 'H');
    x = x + w1;
    add_frame_box(x, y, w2, h1, "HT", 'S');
    x = x + w2;
    add_frame_box(x, y, w3, h1, "SHT", 'S');
    x = x + w3;
    add_frame_box(x, y, w1, h1, txt1[j], 'H');
    x = x + w1;
    add_frame_box(x, y, 2*w4+2*w5, h1/2, "Masked", 'S');
    y = y + h1 / 2;
    create_frame_box_hstring(2, x, y,   w4, h1/2, &txt2[0], 'S', 'C');
    x = x + 2 * w4;
    create_frame_box_hstring(1, x, y, 2*w5, h1/2, &txt2[2], 'S', 'C');
    y = y + h1 / 2;

    x = x1;
    create_frame_box_vstring(11, x, y, w1, h2, txt3, 'S', 'C');
    add_frame_box(x, y+11*h2, w1, 2*h2, "Ext", 'S');
    create_frame_box_hstring(2, x+w1,    y+12*h2, w2/2, h2, &txt2[0], 'S', 'C');
    create_frame_box_hstring(1, x+w1+w2, y+12*h2,   w3, h2, &txt2[2], 'S', 'C');
    x = x + w1;
  
    create_dat_box_vstring(
      11, x, y,         w2, h2, 'N', &r_ptrn[j][0], &sig_ptrn[0],  &PTRN_BOX[j][0][0]);
    create_dat_box_hstring(
       2, x, y+11*h2, w2/2, h2, 'N', &r_ptrn[j][0], &sig_ptrn[11], &PTRN_BOX[j][0][11]);
    x = x + w2;

    create_dat_box_vstring(
      12, x, y,         w3, h2, 'N', &r_ptrn[j][1], &sig_ptrn[0],  &PTRN_BOX[j][1][0]);
    x = x + w3;
    
    create_frame_box_vstring(13, x, y, w1, h2, txt3, 'S', 'C');
    x = x + w1;

    for (i=2; i<6; i++) {
      int w = i > 3 ? w5 : w4;
      create_dat_box_vstring(
        13, x, y, w, h2, 'N', &r_ptrn[j][i], sig_ptrn, &PTRN_BOX[j][i][0]);
      x = x + w;
    }

    x = x1;
    y = y + 13 * h2;
    add_frame_box(x, y, w1, h3, "", 'N');
    x = x + w1;
    obj = add_normal_button(x, y, w2, h3, "R", 'N');
      fl_set_object_callback(obj, read_pattern_callback, 10*j+0);
    x = x + w2;
    obj = add_normal_button(x, y, w3, h3, "R", 'N');
      fl_set_object_callback(obj, read_pattern_callback, 10*j+1);
    x = x + w3;

    add_frame_box(x, y, w1, h3, "", 'N');
    x = x + w1;
    for (i=2; i<6; i++) {
      int w = i > 3 ? w5 : w4;
      obj = add_normal_button(x, y, w, h3, "R", 'N');
        fl_set_object_callback(obj, read_pattern_callback, 10*j+i);
      x = x + w;
    }
    y = y + h3 + 5;
  }
  x1 = x + 5;
}

{//~--- Scalers Readout ---

  int i, j;
  int y2;
  int w1 = 20, w2 = 45, w3 = 20, h1 = 30, h2 = 15, h3 = 25;
  char *txt[2] = {"High\nThreshold", "Super High\nThreshold(BZ)"};

  y = y1;
  for (j=0; j<2; j++) {
    x = x1;
    create_frame_box_hstring(2, x, y, w1+w2+w3, h1, txt, 'S', 'C');
    y = y + h1;
    y2 = y;
    for (i=0; i<2; i++) {
      int k;
      y = y2;
      create_frame_box_vstring(10, x, y, w1, h2, hex, 'S', 'C');
      for (k=0; k<(i?11:10); k++) {
        create_dat_box_hstring(
          1, x+w1, y, w2, h2, 'N', &r_sca[j][i][k], &sca_msk, &SCA_BOX[j][i][k]);
        obj = add_normal_button(x+w1+w2, y, w3, h2, "R", 'N');
          fl_set_object_callback(obj, read_scaler_callback, 10000*j+100*i+k);
        y = y + h2;
      }
      if (i == 0) {
        add_frame_box(x, y, w1+w2+w3+w1, h2, "Internal Generator", 'S');
        y = y + h2;
      }
      obj = add_normal_button(x, y, w1+w2+w3, h3, "READ", 'N');
        fl_set_object_callback(obj, read_scaler_callback, 10000*j+100*i+99);
      y = y + h3;
      if (i == 0) {
        obj = add_normal_button(x, y, 2*(w1+w2+w3), h3, "READ", 'N');
          fl_set_object_callback(obj, read_scaler_callback, 10000*j+100*99+99);
      }
      y = y + h3 + 5;
      x = x + w1 + w2 + w3;
    }
  }
}

}

//~----------------------------------------------------------------------------

void write_mask_callback(FL_OBJECT* obj, long data) {

  int ind = data % 10;
  int jnd = data / 10;
  _r  *r  =  &r_msk[jnd][ind];
  _ww *ww = &ww_msk[jnd][ind];
  int16 adr = msk_adr[jnd][ind];

  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(WRITE, SPT2, adr, ww->inp[0], NULL, NULL, &err);
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

void read_mask_callback(FL_OBJECT* obj, long data) {

  int ind = data % 10;
  int jnd = data / 10;
  int16 rea;
  _r  *r  =  &r_msk[jnd][ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  r->was[0] = r->rea[0];

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(READ, SPT2, msk_adr[jnd][ind], 0, &rea, NULL, &err);
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

void read_state_callback(FL_OBJECT* obj, long data) {

  int ind = data % 10;
  int jnd = data / 10;
  int16 rea;
  _r *r = &r_stat[jnd][ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  r->was[0] = r->rea[0];

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(READ, SPT2, stat_adr[jnd][ind], 0, &rea, NULL, &err);
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

void read_pattern_callback(FL_OBJECT* obj, long data) {

  int ind = data % 10;
  int jnd = data / 10;
  int16 rea;
  _r *r = &r_ptrn[jnd][ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  r->was[0] = r->rea[0];

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(READ, SPT2, ptrn_adr[jnd][ind], 0, &rea, NULL, &err);
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

void read_scaler_callback(FL_OBJECT* obj, long data) {

  int knd =  data % 100;
  int ind = (data / 100) % 100;
  int jnd =  data / 10000;
  int i, k;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;
  
  for (i=0; i<2; i++) {
    if (ind != 99 && ind != i) continue;
    for (k=0; k<(i?11:10); k++) {
      if (knd != 99 && knd != k) continue;
      if (go_on(err)) {
        _r *r = &r_sca[jnd][i][k];
        r->was[0] = r->rea[0];
        exec_SDR2_SC_command(READ, SPT2, sca_adr[jnd][i][k], 0, &rea, NULL, &err);
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
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_registers_and_scalers_panel() {

{//~--- Masks Control ---

  int i, j;
  
  for (j=0; j<2; j++) {
    for (i=0; i<3; i++) {
      show_btn_string(    12,      NULL, &MSK_BTN[j][i][0]);
      show_dat_box_string(12, e_d, NULL, &MSK_BOX[j][i][0]);
    }
  }
}

{//~--- States Readout ---

  int j;
  
  for (j=0; j<2; j++) {
    show_dat_box_string(12, hex, NULL, &STAT_BOX[j][0][0]);
    show_dat_box_string(11, hex, NULL, &STAT_BOX[j][1][0]);
  }
}

{//~--- Patterns Readout ---

  int i, j;
  
  for (j=0; j<2; j++) {
    show_dat_box_string(13, hex, NULL, &PTRN_BOX[j][0][0]);
    show_dat_box_string(12, hex, NULL, &PTRN_BOX[j][1][0]);
  }

  for (j=0; j<2; j++) {
    for (i=2; i<6; i++) {
      show_dat_box_string(13, hex, NULL, &PTRN_BOX[j][i][0]);
    }
  }
}

{//~--- Scalers Readout ---

  int i, j, k;
  
  for (j=0; j<2; j++) {
    for (i=0; i<2; i++) {
      for (k=0; k<(i?11:10); k++) {
        show_dat_box_string(1, NULL, "%5d", &SCA_BOX[j][i][k]);
      }
    }
  }
}

}

//~============================================================================
//
//            OUTPUT CONTROL (LVDS DRIVERS CONTROL) PANEL
//
//~============================================================================

extern int16 lvds_adr;

extern _b lvds;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_output_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 55, h1 = 25, h2 = 15;
  
  w = 5 + w1 + 5 + w2 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Pretrigger Output", 'N');
  y = y + h1;
  create_btn_vstring(
    1, x, y, w1, h2, dis_ena, 'N', &ww_lvds, &lvds, &LVDS_BTN);
  obj = add_normal_button(x, y+h2, w1, h2, "WRITE", 'N');
    fl_set_object_callback(obj, write_output_control_callback, 0);
  x = x + w1 + 5;
  create_dat_box_vstring(
    1, x, y, w2, h2, 'N', &r_lvds, &lvds, &LVDS_BOX);
  obj = add_normal_button(x, y+h2, w2, h2, "READ", 'N');
    fl_set_object_callback(obj, read_output_status_callback, 0);
}

//~----------------------------------------------------------------------------

void write_output_control_callback(FL_OBJECT* obj, long data) {

  _ww *ww = &ww_lvds;
  _r  *r  = &r_lvds;
  int16 adr = lvds_adr;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(WRITE, SPT2, adr, ww->inp[0], NULL, NULL, &err);
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

void read_output_status_callback(FL_OBJECT* obj, long data) {

  int16 rea;
  _r *r = &r_lvds;
  int16 adr = lvds_adr;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  if (!err) {
    r->was[0] = r->rea[0];
    exec_SDR2_SC_command(READ, SPT2, adr, 0, &rea, NULL, &err);
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

void update_output_control_panel() {

  show_btn_string(    1,          NULL, &LVDS_BTN);
  show_dat_box_string(1, dis_ena, NULL, &LVDS_BOX);
}

//~============================================================================
//
//            CONFIGURATION FILE PANEL
//
//~============================================================================

void create_config_file_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 100, h1 = 20, h2 = 15;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Config File", 'N');
  y = y + h1;

  obj = add_normal_button(x, y, w1/2, h2, "WRITE", 'N');
    fl_set_object_callback(obj, write_config_file_callback, 0);
  x = x + w1 / 2;
  obj = add_normal_button(x, y, w1/2, h2, "READ", 'N');
    fl_set_object_callback(obj, read_config_file_callback, 0);
}

//~----------------------------------------------------------------------------

void write_config_file_callback(FL_OBJECT* obj, long data) {

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  if (!write_dat_config_file("SPT2.conf")) err = 0x1000;

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_config_file_callback(FL_OBJECT* obj, long data) {

  if (obj) {
    msg = -2;
    update_objects();
  }
}

//~============================================================================
//
//            READ, WRITE, UPDATE PANEL
//
//~============================================================================

void create_read_write_update_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 100, h1 = 20, h2 = 30;
  
  w = 5 + w1 + 5;
  h = 5 + 2 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "READ ALL", 'M');
    fl_set_object_callback(obj, read_all_callback, 0);
  y = y + h1;
  obj = add_normal_button(x, y, w1, h1, "WRITE ALL", 'M');
    fl_set_object_callback(obj, write_all_callback, 0);
  y = y + h1;
  obj = add_normal_button(x, y, w1, h2, "UPDATE\nINPUT", 'M');
    fl_set_object_callback(obj, update_input_callback, 0);
}

//~----------------------------------------------------------------------------

void read_all_callback(FL_OBJECT* obj, long data) {

  int i, j;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  if (go_on(err)) read_power_status_callback(NULL, 0);
  if (go_on(err)) read_generator_status_callback(NULL, 0);
  if (go_on(err)) read_generator_period_callback(NULL, 0);
  if (go_on(err)) read_scaler_gate_callback(NULL, 0);

  for (j=0; j<2; j++) {
    for (i=0; i<3; i++) if (go_on(err)) read_mask_callback(NULL, 10*j+i);
    for (i=0; i<2; i++) if (go_on(err)) read_state_callback(NULL, 10*j+i);
    for (i=0; i<6; i++) if (go_on(err)) read_pattern_callback(NULL, 10*j+i);
    if (go_on(err)) read_scaler_callback(NULL, 10000*j+100*99+99);
  }

  if (go_on(err)) read_output_status_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void write_all_callback(FL_OBJECT* obj, long data) {

  int i, j;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  if (go_on(err)) write_generator_period_callback(NULL, 0);
  if (go_on(err)) write_scaler_gate_callback(NULL, 0);

  for (j=0; j<2; j++) {
    for (i=0; i<3; i++) {
      if (go_on(err)) write_mask_callback(NULL, 10*j+i);
    }
  }

  if (go_on(err)) write_output_control_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_input_callback(FL_OBJECT* obj, long data) {

  int i, j;
  _r  *r;
  _ww *w;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  read_all_callback(NULL, 0);

  if (!err) {
    r =  &r_gen_period;
    w = &ww_gen_period;
    w->inp[0] = r->rea[0];

    r =  &r_sca_gate;
    w = &ww_sca_gate;
    w->inp[0] = r->rea[0];

    for (j=0; j<2; j++) {
      for (i=0; i<3; i++) {
        _r  *r =  &r_msk[j][i];
        _ww *w = &ww_msk[j][i];
        w->inp[0] = r->rea[0];
      }
    }

    r =  &r_lvds;
    w = &ww_lvds;
    w->inp[0] = r->rea[0];

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

  update_power_control_panel();
  update_generator_panel();
  update_scaler_gate_panel();
  update_registers_and_scalers_panel();

  update_output_control_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
