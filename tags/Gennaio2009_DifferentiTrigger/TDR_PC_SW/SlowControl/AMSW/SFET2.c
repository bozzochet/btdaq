// file SFET2.c
//
// SFET2 & SFEA2 "Pro" Controller
//
// A.Lebedev May-2007...

#include "template.h"
#include "sfet2def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"SFET2 & SFEA2  Controller"};        // global variable
char *date  = {"13-Dec-08"};                        // global variable

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

//~-- Board Selector

SW_BTN *BRD_BTN;
_ww ww_brd;

//~-- Power Control (addresses SDR2)

_b *sfet2_pwr_ctl; // power control
_b *sfet2_pwr_msk; // power mask
_b *sfet2_pwr_mon; // power monitor

DAT_BOX *PWR_BOX;
_r r_pwr_sta[5];

//~-- Actel Version ID

DAT_BOX *VER_BOX;
_r r_ver[5];

//~-- DACs

SW_BTN  *DAC_BTN[6][16];
DAT_BOX *DAC_BOX[6][16];
_r         r_dac[5][6];
_ww       ww_dac[5][6];

//~-- Thresholds

SW_BTN   *THR_VAL_BTN[6];
SW_BTN   *THR_COD_BTN[6];
SW_BTN   *REF_BTN[6];
DAT_BOX  *THR_VAL_BOX[6];
DAT_BOX  *THR_COD_BOX[6];
DAT_BOX  *REF_BOX[6];
_r         r_thr[5][6];
_ww       ww_thr[5][6];
bool      follow[6], follow_all;

//~-- Temperature

DAT_BOX  *TMP_BOX[3];
_r          r_tmp[5][3];

//~-- JTAG

SW_BTN   *JTAG_CMD_COD_BTN[2];
SW_BTN   *JTAG_CMD_COM_BTN[1];
DAT_BOX  *JTAG_CMD_COD_BOX[4];
DAT_BOX  *JTAG_CMD_COM_BOX[1];
_r         r_jtag_cmd;
_ww       ww_jtag_cmd;

SW_BTN   *JTAG_IR_COD_BTN[5];
SW_BTN   *JTAG_IR_INS_BTN[1];
DAT_BOX  *JTAG_IR_COD_BOX[5];
DAT_BOX  *JTAG_IR_INS_BOX[1];
_r         r_jtag_ir;
_ww       ww_jtag_ir;

SW_BTN   *EXT_JTAG_ENA_BTN[1];
SW_BTN   *EXT_JTAG_COD_BTN[4];
DAT_BOX  *EXT_JTAG_ENA_BOX[1];
DAT_BOX  *EXT_JTAG_COD_BOX[5];
_r         r_ext_jtag_ena;
_r         r_ext_jtag_cod;
_ww       ww_ext_jtag_ena;
_ww       ww_ext_jtag_cod;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *brd_nam[5] = {"SFET2-A", "SFET2-B", "SFET2-C", "SFET2-D", "SFEA2"};
int   brd_adr[5] = { SFET2_A,   SFET2_B,   SFET2_C,   SFET2_D,   SFEA2};

char *pwr[4] = {"OFF", "ON", "TRIP", "?"};

char *i_e[2] = {"INT", "EXT"};

char *w_r[2] = {"W", "R"};

char *d_e[2] = {"D", "E"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_board_selector_panel(int x0, int y0);
void board_selector_callback(int32 data);

void create_power_control_panel(int x0, int y0);
void power_control_callback(FL_OBJECT* obj, long data);
void read_power_status_callback(FL_OBJECT* obj, long data);
void update_power_control_panel(void);

void create_actel_version_panel(int x0, int y0);
void read_actel_version_callback(FL_OBJECT* obj, long data);
void update_actel_version_panel(void);

void create_DAC_control_panel(int x0, int y0);
void write_DAC_callback(FL_OBJECT* obj, long data);
void read_DAC_callback(FL_OBJECT* obj, long data);
void update_DAC_control_panel(void);

void create_threshold_control_panel(int x0, int y0);
void set_threshold_callback(FL_OBJECT* obj, long data);
void write_threshold_callback(FL_OBJECT* obj, long data);
void read_threshold_callback(FL_OBJECT* obj, long data);
void update_threshold_control_panel(void);

void create_temperature_control_panel(int x0, int y0);
void read_temperature_callback(FL_OBJECT* obj, long data);
void update_temperature_control_panel(void);

void create_jtag_control_panel(int x0, int y0);
void update_jtag_control_panel(void);
void update_jtag_parity(int32 data);
void write_jtag_cmd_callback(FL_OBJECT* obj, long data);
void read_jtag_cmd_callback(FL_OBJECT* obj, long data);
void write_jtag_ir_callback(FL_OBJECT* obj, long data);
void read_jtag_ir_callback(FL_OBJECT* obj, long data);
void write_ext_jtag_ena_callback(FL_OBJECT* obj, long data);
void read_ext_jtag_ena_callback(FL_OBJECT* obj, long data);
void write_ext_jtag_cod_callback(FL_OBJECT* obj, long data);
void read_ext_jtag_cod_callback(FL_OBJECT* obj, long data);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 650, HH = 425;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "SDR2 Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_board_selector_panel( 0, 45);
  create_power_control_panel(125, 45);
  create_actel_version_panel(375, 45);

  create_DAC_control_panel(          0, 75);
  create_threshold_control_panel(  245, 75);

  create_temperature_control_panel(245, 335);

  create_jtag_control_panel(440, 75);
  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i, j;
  static bool first = TRUE;

  if (first) {
    for (i=0, j=0; i<8; i++) {
      if (sboard[i].typ == 1) {
        brd_nam[j]   = sboard[i].nam;
        brd_adr[j++] = sboard[i].adr;
      }
    }
  }
  
  invalidate_ww(first, &ww_brd, 1, 0);
  for (j=0; j<5; j++) {
    invalidate_r(first, &r_pwr_sta[j], 1);
    invalidate_r(first, &r_ver[j], 1);
  }
  
  for (i=0; i<6; i++) {
    for (j=0; j<5; j++) {
      invalidate_r(first,   &r_dac[j][i], 1);
      invalidate_ww(first, &ww_dac[j][i], 1, 0x00000000);
    }
  }

  follow_all = FALSE;
  for (i=0; i<6; i++) {
    follow[i] = FALSE;
    for (j=0; j<5; j++) {
      invalidate_r(first,   &r_thr[j][i], 1);
      invalidate_ww(first, &ww_thr[j][i], 1, 0x00000000);
    }
  }

  for (i=0; i<3; i++) {
    for (j=0; j<5; j++) {
      invalidate_r(first,   &r_tmp[j][i], 1);
    }
  }

  invalidate_r(first, &r_jtag_cmd, 1);
  invalidate_ww(first, &ww_jtag_cmd, 1, 0);
  invalidate_r(first, &r_jtag_ir, 1);
  invalidate_ww(first, &ww_jtag_ir, 1, 0);
  invalidate_r(first, &r_ext_jtag_ena, 1);
  invalidate_r(first, &r_ext_jtag_cod, 1);
  invalidate_ww(first, &ww_ext_jtag_ena, 1, 0);
  invalidate_ww(first, &ww_ext_jtag_cod, 1, 0);

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
    fread(ww_brd.inp, 4, 1, file);
    for (i=0; i<6; i++) for (j=0; j<5; j++) fread(ww_dac[j][i].inp, 4, 1, file);
    for (i=0; i<6; i++) for (j=0; j<5; j++) fread(ww_thr[j][i].inp, 4, 1, file);
    fread(ww_jtag_cmd.inp,      4, 1, file);
    fread(ww_jtag_ir.inp,       4, 1, file);
    fread(ww_ext_jtag_ena.inp, 4, 1, file);
    fread(ww_ext_jtag_cod.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i, j;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(ww_brd.inp, 4, 1, file);
    for (i=0; i<6; i++) for (j=0; j<5; j++) fwrite(ww_dac[j][i].inp, 4, 1, file);
    for (i=0; i<6; i++) for (j=0; j<5; j++) fwrite(ww_thr[j][i].inp, 4, 1, file);
    fwrite(ww_jtag_cmd.inp,      4, 1, file);
    fwrite(ww_jtag_ir.inp,       4, 1, file);
    fwrite(ww_ext_jtag_ena.inp, 4, 1, file);
    fwrite(ww_ext_jtag_cod.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            BOARD SELECTOR PANEL
//
//~============================================================================

_b brd = {0, 0x00000007, 5};
  
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_board_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 40, w2 = 75, h1 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Board", 'S');
  x = x + w1;

  create_btn_hstring(1, x, y, w2, h1, brd_nam, 'M', &ww_brd, &brd, &BRD_BTN);
  BRD_BTN->call = board_selector_callback;
}

//~-------------------------------------------------------------------------------

void board_selector_callback(int32 data) {

  int i, j;
  int brd = *ww_brd.inp;
  int adr = brd_adr[brd] - 1;
  
  sfet2_pwr_ctl = &pwr_ctl[adr];
  sfet2_pwr_msk = &pwr_msk[adr];
  sfet2_pwr_mon = &pwr_mon[adr];

  PWR_BOX->d = &r_pwr_sta[brd];
  VER_BOX->d = &r_ver[brd];

  for (i=0; i<6; i++) {
    for (j=0; j<16; j++) {
      DAC_BTN[i][j]->d = &ww_dac[brd][i];
      DAC_BOX[i][j]->d = &r_dac[brd][i];
    }
    THR_VAL_BTN[i]->d = &ww_thr[brd][i];
    THR_COD_BTN[i]->d = &ww_thr[brd][i];
    REF_BTN[i]->d     = &ww_thr[brd][i];
    THR_VAL_BOX[i]->d = &r_thr[brd][i];
    THR_COD_BOX[i]->d = &r_thr[brd][i];
    REF_BOX[i]->d     = &r_thr[brd][i];
  }
  for (i=0; i<3; i++) {
    TMP_BOX[i]->d = &r_tmp[brd][i];
  }

  SW_BTN_CALLBACK(0);
}

//~============================================================================
//
//            POWER CONTROL PANEL
//
//~============================================================================

extern int16 pwr_ctl_adr;
extern int16 pwr_msk_adr;

_b pwr_sta_msk = {0x00, 0x0003, 4};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_power_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 35, w3 = 35, w4 = 20, h1 = 20;
  int brd = *ww_brd.inp;
  int adr = brd_adr[brd] - 1;
  
  sfet2_pwr_ctl = &pwr_ctl[adr];
  sfet2_pwr_msk = &pwr_msk[adr];
  sfet2_pwr_mon = &pwr_mon[adr];

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
    1, x, y, w3, h1, 'N', &r_pwr_sta[brd], &pwr_sta_msk, &PWR_BOX);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h1, "R", 'N');
    fl_set_object_callback(obj, read_power_status_callback, 0);
  y = y + h1;
}

//~----------------------------------------------------------------------------

void power_control_callback(FL_OBJECT* obj, long data) {

  bool o_o = data;
  int brd = *ww_brd.inp;
  _r *r = &r_pwr_sta[brd];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    int16 wri = ~sfet2_pwr_msk->msk;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_msk_adr, wri, NULL, NULL, &err);
  }
  
  if (!err) {
    int16 wri = o_o ? sfet2_pwr_ctl->msk | 0x80 : 0x00;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_ctl_adr, wri, NULL, NULL, &err);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_power_status_callback(FL_OBJECT* obj, long data) {

  int brd = *ww_brd.inp;
  int16 rea;
  int16 r_msk, r_ctl;
  _r *r = &r_pwr_sta[brd];

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
      bool ctl = unpack32(r_ctl, sfet2_pwr_ctl->msk);
      bool mon = unpack32(r_ctl, sfet2_pwr_mon->msk);
      if (ctl) {
        if (mon) r->rea[0] = 1;  // ON
        else     r->rea[0] = 2;  // TRIP
      }
      else {
        if (mon) r->rea[0] = 3;  // ?
        else     r->rea[0] = 0;  // OFF
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

  int brd = *ww_brd.inp;
  DAT_BOX *B = PWR_BOX;
  _r *r = &r_pwr_sta[brd];

  show_dat_box_string(1, pwr, NULL, &B);

  if ((r->sta == status_OK) && !r->unk && (r->rea[0] > 1)) {
    fl_set_object_color( B->obj, FL_RED, 0);
    fl_set_object_lcolor(B->obj, FL_YELLOW);
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

  int brd = *ww_brd.inp;
  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 50, w2 = 35, w3 = 20, h1 = 20;

//~--

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1, h1, "Actel ID", 'S');
  x = x + w1;
  create_dat_box_vstring(
    1, x, y, w2, h1, 'N', &r_ver[brd], &ver_msk, &VER_BOX);
  x = x + w2;
  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_actel_version_callback, 0);
}

//~----------------------------------------------------------------------------

void read_actel_version_callback(FL_OBJECT* obj, long data) {

  int brd = *ww_brd.inp;
  int adr = brd_adr[brd];
  int16 rea;
  _r *r = &r_ver[brd];

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  r->was[0] = r->rea[0];
  exec_SDR2_SC_command(READ, adr, ver_adr[adr], 0, &rea, NULL, &err);

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

void update_actel_version_panel(void) {

  show_dat_box_string(1, NULL, "%04X", &VER_BOX);
}

//~============================================================================
//
//            DAC CONTROL PANEL
//
//~============================================================================

extern int16 dac_adr[3][2];

extern _b dac_msk[16];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_DAC_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 45, w2 = 15, h1 = 20, h2 = 15;
  int brd = *ww_brd.inp;
  char *txt1[3] = {"LT", "HT", "SHT"};
  char *txt2[2] = {"\"A\"", "\"B\""};
  char *txt3[16] = {
    "Int/Ext", "   ", "LDAC", "PDB", "PDA", "A/B", "CR1", "CR0",
    "DB7",     "DB6", "DB5",  "DB4", "DB3", "DB2", "DB1", "DB0"};
  char *Cmd[1] = {"Cmd"};

  w = 5 + w1 + 3 * (4 * w2 + 5) -5 + 5;
  h = 5 + h1 + 2 * h2 + 16 * h2 + h1 +5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Manual DAC Control", 'N');
  y = y + h1;

  add_frame_box(              x, y,   w1, 2*h2, "  ", 'S');
  x = x + w1;
  create_frame_box_hstring(3, x, y, 4*w2,   h2, txt1, 'S'+100, 'C');
  y = y + h2;
  create_frame_box_hstring(2, x, y, 2*w2,   h2, txt2, 'S', 'C');
  x = x + 4 * w2 + 5;
  create_frame_box_hstring(2, x, y, 2*w2,   h2, txt2, 'S', 'C');
  x = x + 4 * w2 + 5;
  create_frame_box_hstring(2, x, y, 2*w2,   h2, txt2, 'S', 'C');
  y = y + h2;

  x = x0 + 5;
  create_frame_box_vstring(16, x, y,       w1, h2, txt3, 'S', 'L');
  create_frame_box_vstring(1,  x, y+16*h2, w1, h1, Cmd,  'S', 'R');
  x = x + w1;

  for (i=0; i<6; i++) {
    create_btn_vstring(
      16, x, y, w2, h2, hex, 'N', &ww_dac[brd][i], dac_msk, &DAC_BTN[i][0]);
    obj = add_normal_button(x, y+16*w2, w2, h1, "W", 'N');
      fl_set_object_callback(obj, write_DAC_callback, i);
    x = x + w2;
    create_dat_box_vstring(
      16, x, y, w2, h2,      'N', &r_dac[brd][i],  dac_msk, &DAC_BOX[i][0]);
    obj = add_normal_button(x, y+16*w2, w2, h1, "R", 'N');
      fl_set_object_callback(obj, read_DAC_callback, i);
    x = x + w2;
    if (i % 2) x = x + 5;
  }
}

//~----------------------------------------------------------------------------

void write_DAC_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int brd = *ww_brd.inp;
  int adr = brd_adr[brd];
  int dac = ind / 2;
  int cha = ind % 2;
  _r  *t  =  &r_thr[brd][3*cha+dac];
  _r  *r  =  &r_dac[brd][2*dac+cha];
  _ww *ww = &ww_dac[brd][2*dac+cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  t->sta = status_NA;
  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(
      WRITE, adr, dac_adr[dac][cha], ww->inp[0], NULL, NULL, &err);
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

void read_DAC_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int brd = *ww_brd.inp;
  int adr = brd_adr[brd];
  int dac = ind / 2;
  int cha = ind % 2;
  int16 rea;
  _r  *r  =  &r_dac[brd][2*dac+cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  r->was[0] = r->rea[0];

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(
      READ, adr, dac_adr[dac][cha], 0, &rea, NULL, &err);
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

void update_DAC_control_panel(void) {

  int i;

  for (i=0; i<6; i++) {
    show_btn_string(    16,      NULL, &DAC_BTN[i][0]);
    show_dat_box_string(16, hex, NULL, &DAC_BOX[i][0]);
  }
}

//~============================================================================
//
//            THRESHOLD CONTROL PANEL
//
//~============================================================================

extern float V_ref_int;
extern float V_ref_ext;
extern _b thr_msk;
extern _b ref_msk;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_threshold_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h, x1;
  int w1 = 35, w2 = 30, w4 = 35, w5 = 30, w6 = 35, w7 = 20;
  int h1 = 20, h2 = 15;
  char *txt1[2] = {"0,1,2", "3,4"};
  char *txt2[3] = {"LT", "HT", "SHT"};
  char *Cmd[1] = {"Command"};
  int brd = *ww_brd.inp;

  w = 5 + w1 + w2 + w4 + w5 + w6 + w7 + 5;
  h = 5 + h1 + h2 + 6 * h2 + 5 + 6 * h2 + 2*h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1+h2, "Cha",        'N');
  x = x + w1;
  w = w2 + w4 + w5;
  add_frame_box(x, y, w,     h1, "Thresholds", 'N');
  x = x + w;
  add_frame_box(x, y, w6, h1+h2, "Vref",       'N');
  x = x + w6;
  add_frame_box(x, y, w7, h1+h2, "",           'N');
  y = y + h1;

  x = x0 + 5 + w1;
  add_frame_box(x, y, w2, h2, "",     'S');
  x = x + w2;
  add_frame_box(x, y, w4, h2, "mV",   'S');
  x = x + w4;
  add_frame_box(x, y, w5, h2, "code", 'S');
  y = y + h2;

  x = x0 + 5;
  create_frame_box_vstring(2, x, y,         w1,    6*h2, txt1, 'S'+100, 'C');
  create_frame_box_vstring(1, x, y+12*h2+5, w1+w2, 2*h2, Cmd,  'S',     'R');
  x = x + w1;

  create_frame_box_vstring(3, x, y,        w2, 2*h2, txt2, 'S', 'C');
  create_frame_box_vstring(3, x, y+6*h2+5, w2, 2*h2, txt2, 'S', 'C');
  x = x + w2;

  x1 = x;
  for (i=0; i<6; i++) {
    create_btn_vstring(
      1, x, y,    w4, h2, NULL, 'N', &ww_thr[brd][i], &thr_msk, &THR_VAL_BTN[i]);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,      y, w4/2, h2, "16");
      fl_set_object_callback(obj, set_threshold_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w4/2, y, w4/2, h2, "1");
      fl_set_object_callback(obj, set_threshold_callback, i);
    create_dat_box_vstring(
      1, x, y+h2, w4, h2,       'N',  &r_thr[brd][i], &thr_msk, &THR_VAL_BOX[i]);
    x = x + w4;
    create_btn_vstring(
      1, x, y,    w5, h2, NULL, 'N', &ww_thr[brd][i], &thr_msk, &THR_COD_BTN[i]);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,      y, w5/2, h2, "16");
      fl_set_object_callback(obj, set_threshold_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w5/2, y, w5/2, h2, "1");
      fl_set_object_callback(obj, set_threshold_callback, i);
    create_dat_box_vstring(
      1, x, y+h2, w5, h2,       'N',  &r_thr[brd][i], &thr_msk, &THR_COD_BOX[i]);
    x = x + w5;
    create_btn_vstring(
      1, x, y,    w6, h2, i_e,  'N', &ww_thr[brd][i], &ref_msk, &REF_BTN[i]);
    create_dat_box_vstring(
      1, x, y+h2, w6, h2,       'N',  &r_thr[brd][i], &ref_msk, &REF_BOX[i]);
    x = x + w6;
    obj = add_normal_button(x, y,    w7, h2, "W", 'N');
      fl_set_object_callback(obj, write_threshold_callback, i);
    obj = add_normal_button(x, y+h2, w7, h2, "R", 'N');
      fl_set_object_callback(obj, read_threshold_callback, i);
    y = y + 2 * h2;
    if (i == 2) y = y + 5;
    x = x1;
  }

  obj = add_normal_button(x, y, w4+w5+w6+w7, h2, "WRITE", 'N');
    fl_set_object_callback(obj, write_threshold_callback, 99);
  y = y + h2;
  obj = add_normal_button(x, y, w4+w5+w6+w7, h2, "READ", 'N');
    fl_set_object_callback(obj, read_threshold_callback, 99);
}

//~----------------------------------------------------------------------------

void set_threshold_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int brd = *ww_brd.inp;
  _ww *w = &ww_thr[brd][ind];
  int  d =  atoi(fl_get_object_label(obj));

  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  
  pack32(&w->inp[0], 0xFF, MOD(w->inp[0]+d, 256));
  w->sta = status_NA;

  if (follow[ind] || follow_all) {
    msg = -1;
    update_objects();
    write_threshold_callback(NULL, ind);
    msg = 1;
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void write_threshold_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int brd = *ww_brd.inp;
  int adr = brd_adr[brd];
  int i;
  
  if (obj) {
    msg = 0;
    if ((ind < 99 && follow[ind]) || (ind == 99 && follow_all)) {
      set_normal_button(obj, 0);
      if (ind < 99) follow[ind] = FALSE;
      else          follow_all  = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        if (ind < 99) follow[ind] = TRUE;
        else          follow_all  = TRUE;
        update_objects();
        return;
      }
    }
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  for (i=0; i<6; i++) {
    if (ind != 99 && ind != i) continue;
    if (go_on(err)) {
      int dac = i % 3;
      int cha = i / 3;
      _r  *d =  &r_dac[brd][2*dac+cha];
      _r  *r =  &r_thr[brd][3*cha+dac];
      _ww *w = &ww_thr[brd][3*cha+dac];
      int16 wri = w->inp[0] | (cha * 0x0400) | 0x0300;
      d->sta = status_NA;
      r->sta = status_NA;
      exec_SDR2_SC_command(WRITE, adr, dac_adr[dac][cha], wri, NULL, NULL, &err);
      if (!err) {
        w->wri[0] = w->inp[0];
        w->unk = FALSE;
      }
      else {
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

void read_threshold_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int brd = *ww_brd.inp;
  int adr = brd_adr[brd];
  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  for (i=0; i<6; i++) {
    if (ind != 99 && ind != i) continue;
    if (go_on(err)) {
      int dac = i % 3;
      int cha = i / 3;
      _r  *r  = &r_thr[brd][3*cha+dac];
      int16 rea;
      r->was[0] = r->rea[0];
      exec_SDR2_SC_command(
        READ, adr, dac_adr[dac][cha], 0, &rea, NULL, &err);
      if (!err) {
        r->rea[0] = rea;
        if (r->unk) r->was[0] = r->rea[0];
        r->unk = FALSE;
        if (unpack32(rea, 0x0400) != cha) r->sta = status_KO;
        else                              r->sta = status_OK;
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

void update_threshold_control_panel(void) {

  int i;
  int brd = *ww_brd.inp;
  
  for (i=0; i<6; i++) {
    SW_BTN *B;
    _r  *r  =  &r_thr[brd][i];
    _ww *w  = &ww_thr[brd][i];
    bool w_unk = w->unk;
    float V_ref;
    int val;
    int32 cod;
    char b[80];

    if (w->inp[0] != w->wri[0]) w->unk = TRUE;

    B = THR_VAL_BTN[i];
    show_btn_string(1, NULL, &B);
    cod = B->i;
    V_ref = (w->inp[0] & ref_msk.msk) ? V_ref_ext : V_ref_int;
    val = 2.0 * (float)cod / 256.0 * V_ref;
    sprintf(b, "%4d", val);
    fl_set_object_label(B->obj, b);

    w->unk = w_unk;

    B = THR_COD_BTN[i];
    show_btn_string(1, NULL, &B);
    sprintf(b, "%3d", cod);
    fl_set_object_label(B->obj, b);

    B = REF_BTN[i];
    show_btn_string(1, NULL, &B);

    cod = r->rea[0] & 0xFF;
    V_ref = (r->rea[0] & ref_msk.msk) ? V_ref_ext : V_ref_int;
    val = 2.0 * (float)cod / 256.0 * V_ref;
    sprintf(b, "%4d", val);
    show_dat_box_string(1, NULL, b, &THR_VAL_BOX[i]);
    sprintf(b, "%3d", cod);
    show_dat_box_string(1, NULL, b, &THR_COD_BOX[i]);

    show_dat_box_string(1,  i_e, NULL,  &REF_BOX[i]);
  }
}

//~============================================================================
//
//            TEMPERATURE CONTROL PANEL
//
//~============================================================================

extern int16 tmp_adr[2];
extern _b tmp_msk[3];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_temperature_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 30, w3 = 50, w4 = 20, h1 = 20, h2 = 15;
  int brd = *ww_brd.inp;
  char *txt[2] = {"High", "Low"};

  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10,  h1, "Temperature", 'N');
  y = y + h1;

  create_frame_box_vstring(2, x, y,      w1, h2, txt, 'S', 'L');
  x = x + w1;

  create_dat_box_vstring(
    1, x, y,    w2, h2, 'N', &r_tmp[brd][0], &tmp_msk[0], &TMP_BOX[0]);
  create_dat_box_vstring(
    1, x, y+h2, w2, h2, 'N', &r_tmp[brd][1], &tmp_msk[1], &TMP_BOX[1]);
  x = x + w2;
  
  create_dat_box_vstring(
    1, x, y, w3,  2*h2, 'N', &r_tmp[brd][2], &tmp_msk[2], &TMP_BOX[2]);
  x = x + w3;
  
  obj = add_normal_button(x, y, w4, 2*h2, "R", 'N');
    fl_set_object_callback(obj, read_temperature_callback, 0);
}

//~----------------------------------------------------------------------------

void read_temperature_callback(FL_OBJECT* obj, long data) {

  int brd = *ww_brd.inp;
  int adr = brd_adr[brd];
  int16 rea;
  float T[2];
  _r *r;
  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;
  
  for (i=0; i<2; i++) {
    r = &r_tmp[brd][i];
    r->was[0] = r->rea[0];
    if (go_on(err)) {
      exec_SDR2_SC_command(READ, adr, tmp_adr[i], 0, &rea, NULL, &err);
      if (!err) {
        r->rea[0] = rea;
        T[i] = (float)rea;
        if (r->unk) r->was[0] = r->rea[0];
        r->unk = FALSE;
        r->sta = status_OK;
      }
      else {
        r->sta = status_KO;
      }
    }
  }
  
  r = &r_tmp[brd][2];
  r->was[0] = r->rea[0];
  if (!err) {
    float t = 235.0 - 400.0 * T[0] / T[1];
    r->rea[0] = *(int32*)&t;
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

void update_temperature_control_panel(void) {

  show_dat_box_string(1, NULL, "%03X",   &TMP_BOX[0]);
  show_dat_box_string(1, NULL, "%03X",   &TMP_BOX[1]);
  show_dat_box_string(1, NULL, "%+6.1f", &TMP_BOX[2]);
}

//~============================================================================
//
//            JTAG CONTROL PANEL
//
//~============================================================================

extern int16 jtag_cmd_adr;
extern int16 jtag_ir_adr;
extern int16 ext_jtag_ena_adr;
extern int16 ext_jtag_cod_adr;

extern _b jtag_cmd_cod[4];
extern _b jtag_cmd_com[1];

extern _b jtag_ir_cod[5];
extern _b jtag_ir_ins[1];

extern _b ext_jtag_ena[1];
extern _b ext_jtag_cod[5];

char *com[4] = {"Unknown", "Start Write", "Start Read", "Reset"};

char *ins[16] = {"EXTEST", "IDCODE",  "SAMPLE", "INTEST",
                 "BIST",   "SCAN",    "N/A",    "READOUT",
                 "SETUP",  "CONTROL", "STATUS", "CORETEST",
                 "N/A",    "N/A",     "N/A",    "BYPASS"};

char *jtag[5] = {"O", "I", "R", "M", "C"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_jtag_control_panel(int x0, int y0) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int i;
  int x, y;
 
{//~--- Panel ---

int w1 = 210, h1 = 180, h2 =20;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1-10, h2, "JTAG Control (new)", 'N');
  y = y + h2 + 5;
}

{//~--- Cmd ---

int w1 = 40, w2 = 15, w3 = 80, w4 = 20, h1 = 15;

  add_frame_box(x, y, w1, 3*h1, "Cmd",     'N');
  x = x + w1;
  add_frame_box(x, y, 4*w2, h1, "code",    'S');
  x = x + 4 * w2;
  add_frame_box(x, y, w3,   h1, "command", 'S');
  x = x + w3;
  add_frame_box(x, y, w4,   h1, "",        'S');
  y = y + h1;

  x = x0 + 5 + w1;
  add_frame_box(x, y, w2,   h1, "E",       'S');
  x = x + w2;
  add_frame_box(x, y, w2,   h1, "S",       'S');
  x = x + w2;
  create_btn_hstring(
    2, x, y, w2, h1, hex, 'N', &ww_jtag_cmd, &jtag_cmd_cod[2], JTAG_CMD_COD_BTN);
  x = x + 2 * w2;
  create_btn_hstring(
    1, x, y, w3, h1, com, 'N', &ww_jtag_cmd, jtag_cmd_com,     JTAG_CMD_COM_BTN);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h1, "W", 'N');
    fl_set_object_callback(obj, write_jtag_cmd_callback, 0);
  y = y + h1;

  x = x0 + 5 + w1;
  create_dat_box_hstring(
    4, x, y, w2, h1,      'N', &r_jtag_cmd,  &jtag_cmd_cod[0], JTAG_CMD_COD_BOX);
  x = x + 4 * w2;
  create_dat_box_hstring(
    1, x, y, w3, h1,      'N', &r_jtag_cmd,  jtag_cmd_com,     JTAG_CMD_COM_BOX);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h1, "R", 'N');
    fl_set_object_callback(obj, read_jtag_cmd_callback, 0);
  y = y + h1 + 5;
}

{//~--- IR Reg ---

int w1 = 40, w2 = 15, w3 = 65, w4 = 20, h1 = 15;

  x = x0 + 5;
  
  add_frame_box(x, y, w1,  3*h1, "IR\nReg",     'N');
  x = x + w1;
  add_frame_box(x, y, 5*w2,  h1, "code",        'S');
  x = x + 5 * w2;
  add_frame_box(x, y, w3,    h1, "instruction", 'S');
  x = x + w3;
  add_frame_box(x, y, w4,    h1, "",            'S');
  y = y + h1;

  x = x0 + 5 + w1;
  create_btn_hstring(
    5, x, y, w2, h1, hex, 'N', &ww_jtag_ir, jtag_ir_cod, &JTAG_IR_COD_BTN[0]);
  for (i=1; i<5; i++) {
    B = JTAG_IR_COD_BTN[i];
    B->call = update_jtag_parity;
    B->data = (int32)&ww_jtag_ir;
  }
  x = x + 5 * w2;
  create_btn_hstring(
    1, x, y, w3, h1, ins, 'N', &ww_jtag_ir, jtag_ir_ins, &JTAG_IR_INS_BTN[0]);
    B = JTAG_IR_INS_BTN[0];
    B->call = update_jtag_parity;
    B->data = (int32)&ww_jtag_ir;
  x = x + w3;
  obj = add_normal_button(x, y, w4, h1, "W", 'N');
    fl_set_object_callback(obj, write_jtag_ir_callback, 0);
  y = y + h1;

  x = x0 + 5 + w1;
  create_dat_box_hstring(
    5, x, y, w2, h1,      'N', &r_jtag_ir,  jtag_ir_cod, &JTAG_IR_COD_BOX[0]);
  x = x + 5 * w2;
  create_dat_box_hstring(
    1, x, y, w3, h1,      'N', &r_jtag_ir,  jtag_ir_ins, &JTAG_IR_INS_BOX[0]);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h1, "R", 'N');
    fl_set_object_callback(obj, read_jtag_ir_callback, 0);
  y = y + h1 + 5;
}

{//~--- Ext JTAG ---

int w1 = 40, w2 = 15, w3 = 20, h1 = 15;

  x = x0 + 5;
  
  add_frame_box(x, y, w1,  3*h1, "Ext\nJTAG",     'N');
  x = x + w1;
  add_frame_box(x, y, w2+w3, h1, "enabl",         'S');
  x = x + w2 + w3 + 5;
  create_frame_box_hstring(5, x, y, w3, h1, jtag, 'S', 'C');
  x = x + 5 * w3;
  add_frame_box(x, y, w3,    h1, "",              'S');
  y = y + h1;

  x = x0 + 5 + w1;
  create_btn_hstring(
    1, x, y, w2, h1, d_e, 'N', &ww_ext_jtag_ena, ext_jtag_ena, EXT_JTAG_ENA_BTN);
  x = x + w2;
  obj = add_normal_button(x, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_ext_jtag_ena_callback, 0);
  x = x + w3 + 5;
  add_frame_box(x, y, w3,    h1, "",              'S');
  x = x + w3;
  create_btn_hstring(
    4, x, y, w3, h1, hex, 'N', &ww_ext_jtag_cod, &ext_jtag_cod[1], EXT_JTAG_COD_BTN);
  x = x + 4 * w3;
  obj = add_normal_button(x, y, w3, h1, "W", 'N');
    fl_set_object_callback(obj, write_ext_jtag_cod_callback, 0);
  y = y + h1;

  x = x0 + 5 + w1;
  create_dat_box_hstring(
    1, x, y, w2, h1,      'N', &r_ext_jtag_ena,  ext_jtag_ena, EXT_JTAG_ENA_BOX);
  x = x + w2;
  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_ext_jtag_ena_callback, 0);
  x = x + w3 + 5;
  create_dat_box_hstring(
    5, x, y, w3, h1,      'N', &r_ext_jtag_cod,  &ext_jtag_cod[0], EXT_JTAG_COD_BOX);
  x = x + 5 * w3;
  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_ext_jtag_cod_callback, 0);
}

}

//~----------------------------------------------------------------------------

void update_jtag_parity(int32 data) {

  int16 m, p;
  _ww *ww = (_ww*)data;

  for (m=0x0010,p=0; m; m>>=1) if (ww->inp[0] & m) p ^= 1;
  if (p) ww->inp[0] ^= 0x0010;

  SW_BTN_CALLBACK(0);
}


//~----------------------------------------------------------------------------

void write_jtag_cmd_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r  *r  =  &r_jtag_cmd;
  _ww *ww = &ww_jtag_cmd;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  exec_SDR2_SC_command(WRITE, adr, jtag_cmd_adr, ww->inp[0], NULL, NULL, &err);

  if (!err) {
    ww->wri[0] = ww->inp[0];
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

void read_jtag_cmd_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r *r = &r_jtag_cmd;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];

  exec_SDR2_SC_command(READ, adr, jtag_cmd_adr, 0, &rea, NULL, &err);

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

void write_jtag_ir_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r  *r  =  &r_jtag_ir;
  _ww *ww = &ww_jtag_ir;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  exec_SDR2_SC_command(WRITE, adr, jtag_ir_adr, ww->inp[0], NULL, NULL, &err);

  if (!err) {
    ww->wri[0] = ww->inp[0];
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

void read_jtag_ir_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r *r = &r_jtag_ir;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];

  exec_SDR2_SC_command(READ, adr, jtag_ir_adr, 0, &rea, NULL, &err);

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

void write_ext_jtag_ena_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r  *r  =  &r_ext_jtag_ena;
  _ww *ww = &ww_ext_jtag_ena;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  exec_SDR2_SC_command(WRITE, adr, ext_jtag_ena_adr, ww->inp[0], NULL, NULL, &err);

  if (!err) {
    ww->wri[0] = ww->inp[0];
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

void read_ext_jtag_ena_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r *r = &r_ext_jtag_ena;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];

  exec_SDR2_SC_command(READ, adr, ext_jtag_ena_adr, 0, &rea, NULL, &err);

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

void write_ext_jtag_cod_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r  *r  =  &r_ext_jtag_cod;
  _ww *ww = &ww_ext_jtag_cod;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  exec_SDR2_SC_command(WRITE, adr, ext_jtag_cod_adr, ww->inp[0], NULL, NULL, &err);

  if (!err) {
    ww->wri[0] = ww->inp[0];
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

void read_ext_jtag_cod_callback(FL_OBJECT* obj, long data) {

  int adr = brd_adr[*ww_brd.inp];
  _r *r = &r_ext_jtag_cod;
  int16 rea;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];

  exec_SDR2_SC_command(READ, adr, ext_jtag_cod_adr, 0, &rea, NULL, &err);

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

void update_jtag_control_panel(void) {

  SW_BTN *B;
  int16 m, p;

  show_btn_string(    2,      NULL, JTAG_CMD_COD_BTN);
  show_btn_string(    1,      NULL, JTAG_CMD_COM_BTN);
  show_dat_box_string(4, hex, NULL, JTAG_CMD_COD_BOX);
  show_dat_box_string(1, com, NULL, JTAG_CMD_COM_BOX);

  show_btn_string(    5,      NULL, &JTAG_IR_COD_BTN[0]);
  for (m=0x0010,p=0; m; m>>=1) if (ww_jtag_ir.inp[0] & m) p ^= 1;
  if (p) {
    B = JTAG_IR_COD_BTN[0];
    fl_set_object_color(B->obj, FL_RED, 0);
    fl_set_object_lcolor(B->obj, FL_YELLOW);
  }
  show_btn_string(    1,      NULL, &JTAG_IR_INS_BTN[0]);
  show_dat_box_string(5, hex, NULL, &JTAG_IR_COD_BOX[0]);
  show_dat_box_string(1, ins, NULL, &JTAG_IR_INS_BOX[0]);

  show_btn_string(    1,      NULL, EXT_JTAG_ENA_BTN);
  show_btn_string(    4,      NULL, EXT_JTAG_COD_BTN);
  show_dat_box_string(1, d_e, NULL, EXT_JTAG_ENA_BOX);
  show_dat_box_string(5, hex, NULL, EXT_JTAG_COD_BOX);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_power_control_panel();
  update_actel_version_panel();
  update_DAC_control_panel();
  update_threshold_control_panel();
  update_temperature_control_panel();
  update_jtag_control_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
