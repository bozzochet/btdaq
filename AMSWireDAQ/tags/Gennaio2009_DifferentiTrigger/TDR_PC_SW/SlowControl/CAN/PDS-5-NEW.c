// file PDS-5-NEW.c
//
// PDS-5-NEW Controller - uses new CGS protocol
//
// A.Lebedev Dec-2007...

#include "template.h"
#include "pdsdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"PDS-5-NEW  Controller"};              // global variable
char *date  = {"26-Oct-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

_brd *brd;

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~-- PDS Model Selector

SW_BTN *MODEL_BTN;
_ww w_model;
char *models[2] = {"EM", "FM"};

//~-- CAN I/F ("USCM") Selector

SW_BTN *I_F_BTN;
_ww w_i_f;
_b i_f = {0, 0x00000001, 2};
char *i_fs[2] = {"A", "B"};

//~-- PDS Side Selector

SW_BTN *SIDE_BTN;
_ww w_side;
_b side = {0, 0x00000001, 2};
char *sides[2] = {"A", "B"};
int far_side;

//~-- Overall Board Status

DAT_BOX *STA_BOX[10];
DAT_BOX *OVT_BOX[10];
DAT_BOX *TMP_BOX[10];
DAT_BOX *REF_BOX[10];
DAT_BOX *TST_BOX[10];
_r r_sta[10];
_r r_ovt[10];
_r r_tmp[10];
_r r_ref[10];
_r r_tst[10];

//~-- 28V DC/DC Control

DAT_BOX *_28_STA_BOX[4];
DAT_BOX *_28_OVT_BOX[4];
DAT_BOX *_28_OVC_BOX[4];
DAT_BOX *_28_UNV_BOX[4];
DAT_BOX *_28_TRIP_BOX[4];
DAT_BOX *_28_CMD_BOX[4];
DAT_BOX *_28_STAT_BOX[4];
DAT_BOX *_28_VOL_BOX[4];
_r r_ovc[4];
_r r_unv[4];
_r r_trip[4];
_r r_cmd[4];
_r r_stat[4];
_r r_vol[4];
//                  sta   ovt   ovc   unv  trip   cmd  stat   vol
int8 _28_cha[8] = {0x00, 0x01, 0x04, 0x15, 0x16, 0x17, 0x05, 0x04};
char *num[16] = {"1", "2", "3", "4"};

//~-- Input Stage

DAT_BOX *INP_BOX[4];
_r r_inp;

//~-- 28V Outlets Control

SW_BTN  *OUT_CMD_BTN[32];
DAT_BOX *IN__STA_BOX[32];
DAT_BOX *IN__VOL_BOX[32];
DAT_BOX *OUT_STA_BOX[32];
DAT_BOX *OUT_CMD_BOX[32];
DAT_BOX *OUT_STAT_BOX[32];
DAT_BOX *OUT_CUR_BOX[32];
_ww w_cmd[32];
_r r_in__sta[32];
_r r_out_sta[32];
_r r_out_cmd[32];
_r r_out_stat[32];
_r r_out_cur[32];

//~--

char *bin[]     = {"0", "1"};
char *a_b[]     = {"A", "B"};
char *o_f[]     = {"OK", "FAIL"};
char *o_o[]     = {"OFF", "ON", "-"};
char *dash[]    = {"-", "-"};
char *periods[] = {"0.2s", "0.5s", "1.0s", "2.0s", "5.0s", "10.0s"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_box_model_selector_panel(int x0, int y0);
void box_model_callback(int32 data);

void create_CAN_bus_I_F_selector_panel(int x0, int y0);

void create_PDS_side_selector_panel(int x0, int y0);

void create_overall_status_panel(int x0, int y0);
void read_overall_status_callback(FL_OBJECT *obj, long data);
void test_boards_callback(FL_OBJECT *obj, long data);
void update_overall_status_panel(void);

void create_28V_DCDC_panel(int x0, int y0);
void control_28V_DCDC_callback(FL_OBJECT *obj, long data);
void read_28V_DCDC_callback(FL_OBJECT *obj, long data);
void update_28V_DCDC_panel(void);

void create_input_stage_panel(int x0, int y0);
void input_stage_callback(FL_OBJECT *obj, long data);
void update_input_stage_panel(void);

void create_ALL_OFF_button(int x0, int y0);
void ALL_OFF_callback(FL_OBJECT *obj, long data);

void create_28V_outlets_panel(int x0, int y0);
void control_28V_outlets_callback(FL_OBJECT *obj, long data);
void write_28V_outlets_callback(FL_OBJECT *obj, long data);
void read_28V_outlets_callback(FL_OBJECT *obj, long data);
void update_28V_outlets_panel(void);

void create_120V_outlets_panel(int x0, int y0);
void read_120V_outlets_callback(FL_OBJECT *obj, long data);
void write_120V_outlets_callback(FL_OBJECT *obj, long data);
void update_120V_outlets_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 595, HH = 670;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_box_model_selector_panel(200,   0);
  box_model_callback(0);

  create_CAN_bus_I_F_selector_panel(0,  45);
  create_PDS_side_selector_panel( 145,  45);

  create_overall_status_panel(  0,  75);
  create_28V_DCDC_panel(        0, 285);
  create_input_stage_panel(     0, 510);
  create_USCM_control_H3_panel( 0, 585);
  
  create_ALL_OFF_button(170, 550);

  create_28V_outlets_panel(240, 45);
  
  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;
  
  invalidate_ww(first, &w_model, 1, 0);

  invalidate_ww(first, &w_i_f,  1, 0);
  invalidate_ww(first, &w_side, 1, 0);

  for (i=0; i<10; i++) {
    invalidate_r(first, &r_sta[i], 1);
    invalidate_r(first, &r_ovt[i], 1);
    invalidate_r(first, &r_tmp[i], 1);
    invalidate_r(first, &r_ref[i], 1);
    invalidate_r(first, &r_tst[i], 1);
  }

  for (i=0; i<4; i++) {
    invalidate_r(first, &r_ovc[i], 1);
    invalidate_r(first, &r_unv[i], 1);
    invalidate_r(first, &r_trip[i], 1);
    invalidate_r(first, &r_cmd[i], 1);
    invalidate_r(first, &r_stat[i], 1);
    invalidate_r(first, &r_vol[i], 1);
  }

  invalidate_r(first, &r_inp, 4);

  for (i=0; i<32; i++) {
    invalidate_ww(first, &w_cmd[i], 1, 0);
    invalidate_r(first, &r_in__sta[i], 1);
    invalidate_r(first, &r_out_sta[i], 1);
    invalidate_r(first, &r_out_cmd[i], 1);
    invalidate_r(first, &r_out_stat[i], 1);
    invalidate_r(first, &r_out_cur[i], 1);
  }

  read_dat_config_file(dat_config_file_name);

  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(w_model.inp, 4, 1, file);
    fread(w_i_f.inp,   4, 1, file);
    fread(w_side.inp,  4, 1, file);
    for (i=0; i<32; i++) {
      fread(w_cmd[i].inp, 4, 1, file);
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i;

  file = fopen(filename, "w");
  if (file) {
    fwrite(w_model.inp, 4, 1, file);
    fwrite(w_i_f.inp,   4, 1, file);
    fwrite(w_side.inp,  4, 1, file);
    for (i=0; i<32; i++) {
      fwrite(w_cmd[i].inp, 4, 1, file);
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
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

//~============================================================================
//
//                  BOX MODEL SELECTOR_PANEL
//
//~============================================================================

void create_box_model_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 40, h1 = 15, h2 = 20;
  _b msk2[] = {{0, 0x00000001, 2}};
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Model", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, models, 'M', &w_model, msk2, &MODEL_BTN);
  MODEL_BTN->call = box_model_callback;
  MODEL_BTN->data = 1;
}

//~----------------------------------------------------------------------------

void box_model_callback(int32 data) {

  switch (*w_model.inp) {
    case 0:  brd = &em_brd[0]; break;
    case 1:  brd = &fm_brd[0]; break;
    default: brd = &em_brd[0]; break;
  }
  if (data) {
    update_objects();
    refresh_objects();
  }
}

//~============================================================================
//
//                  CAN_BUS_I/F_SELECTOR_PANEL
//
//~============================================================================

void create_CAN_bus_I_F_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 110, w2 = 25;
  int h1 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "CAN Bus I/F Side", FL_NORMAL_SIZE);
  x = x + w1;
  create_btn_hstring(1, x, y, w2, h1, i_fs, 'M', &w_i_f, &i_f, &I_F_BTN);
}

//~============================================================================
//
//                  PDS SIDE SELECTOR PANEL
//
//~============================================================================

void create_PDS_side_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 60, w2 = 25;
  int h1 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "PDS Side", 'N');
  x = x + w1;
  create_btn_hstring(1, x, y, w2, h1, sides, 'M', &w_side, &side, &SIDE_BTN);
}

//~============================================================================
//
//                  OVERALL STATUS PANEL
//
//~============================================================================

void create_overall_status_panel(int x0, int y0) {

  int i;
  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 70, w2 = 30, w3 = 30, w4 = 35, w5 = 35, w6 = 30;
  int h1 = 20, h2 = 15, h3 = 15;
  int n = 10;
  _b msk2[] = {{0, 0x00000001,  2}};
  _b mska[] = {{0, 0xFFFFFFFF, -1}};
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + 5;
  h = 5 + h1 + h2 + n * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Overall Boards Status", 'M');
  y = y + h1;
  add_frame_box(x, y, w1, h2, "Board", 'S');
  x = x + w1;
  add_frame_box(x, y, w2, h2, "Sta",   'S');
  x = x + w2;
  add_frame_box(x, y, w3, h2, "OVT",   'S');
  x = x + w3;
  add_frame_box(x, y, w4, h2, "Temp",  'S');
  x = x + w4;
  add_frame_box(x, y, w5, h2, "Ref",   'S');
  x = x + w5;
  add_frame_box(x, y, w6, h2, "Test",  'S');
  y = y + h2;
  
  x = x0 + 5;
  for (i=0; i<n; i++) {
    int j;
    char b[80];
    char *t[1] = {b};
    for (j=0; j<strlen(brd[i].nam); j++) {
      if (brd[i].nam[j] == '\n') b[j] = '\0';
      else                       b[j] = brd[i].nam[j];
    }
    create_frame_box_vstring(1, x, y+i*h2, w1, h2, t, 'S', 'L');
  }
  x = x + w1;
  for (i=0; i<n; i++) {
    create_dat_box_vstring(1, x, y+i*h2, w2, h2, 'N', &r_sta[i], msk2, &STA_BOX[i]);
  }
  x = x + w2;
  for (i=0; i<n; i++) {
    create_dat_box_vstring(1, x, y+i*h2, w3, h2, 'N', &r_ovt[i], msk2, &OVT_BOX[i]);
  }
  x = x + w3;
  for (i=0; i<n; i++) {
    create_dat_box_vstring(1, x, y+i*h2, w4, h2, 'N', &r_tmp[i], mska, &TMP_BOX[i]);
  }
  x = x + w4;
  for (i=0; i<n; i++) {
    create_dat_box_vstring(1, x, y+i*h2, w5, h2, 'N', &r_ref[i], mska, &REF_BOX[i]);
  }
  x = x + w5;
  for (i=0; i<n; i++) {
    create_dat_box_vstring(1, x, y+i*h2, w6, h2, 'N', &r_tst[i], msk2, &TST_BOX[i]);
  }

  x = x0 + 5;
  y = y + n * h2;
  obj = add_frame_box(x, y, w1, h3, "Command",  FL_SMALL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  x = x + w1;
  obj = add_normal_button(x, y, w2+w3+w4+w5, h3, "READ  ALL", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_overall_status_callback, 0);
  x = x + w2 + w3 + w4 + w5;
  obj = add_normal_button(x, y, w6,          h3, "TST",       FL_NORMAL_SIZE);
    fl_set_object_callback(obj, test_boards_callback, 0);
}

//~----------------------------------------------------------------------------

void read_overall_status_callback(FL_OBJECT *obj, long data) {

  int i;
  int n = 10;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=0; i<n; i++) {
    int ii;
    int8 adr = brd[i].adr;
    int8 adc = brd[i].adc[far_side];
    int16 cmd;
    int8  dig;
    int16 ana;
    for (ii=0; ii<4; ii++) {
      int8 cha = 0xFF;
      _r *r = NULL;
      switch (ii) {
        case 0: cha = brd[i].sta; r = &r_sta[i];  break;
        case 1: cha = brd[i].ovt; r = &r_ovt[i];  break;
        case 2: cha = brd[i].tmp; r = &r_tmp[i];  break;
        case 3: cha = brd[i].ref; r = &r_ref[i];  break;
      }
      if (cha == 0xFF) continue;
      if (go_on(err)) {
        r->was[0] = r->rea[0];
        cmd = 0;
        pack16(&cmd, 0x0200, far_side);
        pack16(&cmd, 0x01E0, adr);
        pack16(&cmd, 0x001F, cha);
printf("OK\n");
        exec_PDS_cmd(cmd, 0x80+adc, &dig, &ana, &err);
        if (!err) {
          if (r != &r_tmp[i] && r != &r_ref[i]) {
            r->rea[0] = dig & 0x01;
          }
          else {
            float A = (brd[i].ana[cha].max - brd[i].ana[cha].min) / 4095.0;
            float B =                        brd[i].ana[cha].min;
            float v = A * (float)ana + B;
            r->rea[0] = *(int32*)&v;
          }
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

void test_boards_callback(FL_OBJECT *obj, long data) {

  int i;
  int n = 10;
  int8 test_pattern = 0xAA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  for (i=0; i<n; i++) {
    int ii;
    int8  adr = brd[i].adr;
    int16 error = 0x0000;
    _r *r = &r_tst[i];
    r->was[0] = r->rea[0];
    for (ii=0; ii<8; ii++) {
      int8  cha;
      int16 cmd;
      int8  dat = 0xFF;
      int8  dig;
      if (go_on(err)) {
        cha = brd[i].t_w;
        dat = unpack16(test_pattern, 0x80 >> ii);
        cmd = 0;
        pack16(&cmd, 0x0800, dat);
        pack16(&cmd, 0x0400, 1);
        pack16(&cmd, 0x0200, far_side);
        pack16(&cmd, 0x01E0, adr);
        pack16(&cmd, 0x001F, cha);
        exec_PDS_cmd(cmd, 0x80+0, NULL, NULL, &err);
        error |= err;
      }
      if (go_on(err)) {
        cha = brd[i].t_r;
        cmd = 0;
        pack16(&cmd, 0x0200, far_side);
        pack16(&cmd, 0x01E0, adr);
        pack16(&cmd, 0x001F, cha);
        exec_PDS_cmd(cmd, 0x80+0, &dig, NULL, &err);
        error |= err;
        if (dig != dat) error |= 0x8000;
      }
    }
    r->rea[0] = error ? 1 : 0;
    if (!err) {
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

void update_overall_status_panel(void) {

  int i;
  _r *r;

  for (i=0; i<10; i++) {
    float value;
    char b[80];
//printf("i = %2d, name = %s, adr = %d\n", i, brd[i].nam, brd[i].adr);

    show_dat_box_string(1, brd[i].dig[brd[i].sta].txt, NULL, &STA_BOX[i]);
    show_dat_box_string(1, brd[i].dig[brd[i].ovt].txt, NULL, &OVT_BOX[i]);
    r = &r_tmp[i];
    value = *(float*)&r->rea[0];
    if      (value < 10.0)   sprintf(b, "%4.2f", value);
    else if (value < 100.0)  sprintf(b, "%4.1f", value);
    else if (value < 1000.0) sprintf(b, "%4.0f", value);
    else                     sprintf(b, "*****");
    show_dat_box_string(1,  NULL,     b, &TMP_BOX[i]);
    r = &r_ref[i];
    value = *(float*)&r->rea[0];
    if      (value < 10.0)   sprintf(b, "%4.2f", value);
    else if (value < 100.0)  sprintf(b, "%4.1f", value);
    else if (value < 1000.0) sprintf(b, "%4.0f", value);
    else                     sprintf(b, "*****");
    show_dat_box_string(1, NULL,    b, &REF_BOX[i]);
    show_dat_box_string(1,  o_f, NULL, &TST_BOX[i]);
  }
}

//~============================================================================
//
//                  28V DCDC CONTROL PANEL
//
//~============================================================================

void create_28V_DCDC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, yy, w, h;
  int w1 = 90, w2 = 35, h1 = 20, h2 = 15, h3 = 15;
  char *txt[] = {
    "Board Status",
    "OVT Alarm",
    "Input OVC",
    "27V UNV",
    "29V Trip",
    "ON/OFF Cmd",
    "ON/OFF Status",
    "Output Voltage"};
  int n = sizeof(txt) / sizeof(txt[0]);
  _b msk2[1] = {{0, 0x00000001, 2}};
  _b mska[1] = {{0, 0xFFFFFFFF, -1}};

  w = 5 + w1 + 4 * w2 + 5;
  h = 5 + h1 + h2 + n * h2 + 4 * h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "28V DC/DC Control", 'M');
  y = y + h1;

  x = x0 + 5;
  add_frame_box(x, y, w1, h2, "", 'N');
  x = x + w1;
  create_frame_box_hstring(4, x, y, w2, h2, num, 'S', 'C');
  y = y + h2;

  x = x0 + 5;
  create_frame_box_vstring(n, x, y, w1, h2, txt, 'S', 'L');

  x = x0 + 5 + w1;
  yy = y;
  for (i=0; i<4; i++) {
    int j[4] = {1, 3, 5, 7};
    int k = j[i];
    y = yy;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_sta[k], msk2, &_28_STA_BOX[i]);
    y = y + h2;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_ovt[k], msk2, &_28_OVT_BOX[i]);
    y = y + h2;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_ovc[i], msk2, &_28_OVC_BOX[i]);
    y = y + h2;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_unv[i], msk2, &_28_UNV_BOX[i]);
    y = y + h2;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_trip[i],msk2, &_28_TRIP_BOX[i]);
    y = y + h2;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_cmd[i], msk2, &_28_CMD_BOX[i]);
    y = y + h2;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_stat[i],msk2, &_28_STAT_BOX[i]);
    y = y + h2;
    create_dat_box_hstring(
      1, x, y, w2, h2, 'N', &r_vol[i], mska, &_28_VOL_BOX[i]);
    y = y + h2;
    x = x + w2;
  }

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, 4*h3, "Commands",  'S');
    fl_set_object_lalign(obj, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
  x = x + w1;

  for (i=0; i<4; i++) {
    obj = add_normal_button(x, y,      w2, h3, "ON",  'N');
      fl_set_object_callback(obj, control_28V_DCDC_callback, 2*i+0);
    obj = add_normal_button(x, y+h3,   w2, h3, "OFF", 'N');
      fl_set_object_callback(obj, control_28V_DCDC_callback, 2*i+1);
    obj = add_normal_button(x, y+2*h3, w2, h3, "R",   'N');
      fl_set_object_callback(obj, read_28V_DCDC_callback, i);
    x = x + w2;
  }
  
  x = x0 + 5 + w1;
  y = y + 3 * h3;
  obj = add_normal_button(x, y, 4*w2, h3, "READ  ALL", 'N');
    fl_set_object_callback(obj, read_28V_DCDC_callback, -1);
}

//~----------------------------------------------------------------------------

void control_28V_DCDC_callback(FL_OBJECT *obj, long data) {

  int8 ind = data / 2;
  int8 dat = data % 2;
  int j[4] = {1, 3, 5, 7};
  int k = j[ind];
  int8 adr = brd[k].adr;
  int8 cha = 0x17;
  int16 cmd;

  if (obj) {
    msg = -1;
    update_objects();
  }

  cmd = 0;
  pack16(&cmd, 0x0800, dat);
  pack16(&cmd, 0x0400, 1);
  pack16(&cmd, 0x0200, far_side);
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);
//enable_CGS_serial_lines(&err);
  exec_PDS_cmd(cmd, 0x80+0, NULL, NULL, &err);
//printf("Command sent\n");
//disable_CGS_serial_lines(&err);
    
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_28V_DCDC_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=0; i<4; i++) {
    int j[4] = {1, 3, 5, 7};
    int k = j[i];
    int ii;
    int8 adr = brd[k].adr;
    int8 adc = brd[k].adc[far_side];
    int16 cmd;
    int8  dig;
    int16 ana;
    _r *r = NULL;
    if (ind != -1 && ind != i) continue;
    for (ii=0; ii<8; ii++) {
      int8 cha = _28_cha[ii];
      switch (ii) {
        case 0: r = &r_sta[k];  break;
        case 1: r = &r_ovt[k];  break;
        case 2: r = &r_ovc[i];  break;
        case 3: r = &r_unv[i];  break;
        case 4: r = &r_trip[i]; break;
        case 5: r = &r_cmd[i];  break;
        case 6: r = &r_stat[i]; break;
        case 7: r = &r_vol[i];  break;
      }
      if (go_on(err)) {
        r->was[0] = r->rea[0];
        cmd = 0;
        pack16(&cmd, 0x0200, far_side);
        pack16(&cmd, 0x01E0, adr);
        pack16(&cmd, 0x001F, cha);
        exec_PDS_cmd(cmd, 0x80+adc, &dig, &ana, &err);
        if (!err) {
          if (r != &r_vol[i]) {
            r->rea[0] = dig & 0x01;
          }
          else {
            float A = (brd[k].ana[cha].max - brd[k].ana[cha].min) / 4095.0;
            float B =                        brd[k].ana[cha].min;
            float v = A * (float)ana + B;
            r->rea[0] = *(int32*)&v;
          }
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

void update_28V_DCDC_panel(void) {

  int i;

  for (i=0; i<4; i++) {
    float value;
    char b[80];
    int j[4] = {1, 3, 5, 7};
    int k = j[i];
    _r *r;

    show_dat_box_string(1, brd[k].dig[brd[k].sta].txt,  NULL, &_28_STA_BOX[i]);
    show_dat_box_string(1, brd[k].dig[brd[k].ovt].txt,  NULL, &_28_OVT_BOX[i]);
    show_dat_box_string(1, brd[k].dig[0x04].txt,        NULL, &_28_OVC_BOX[i]);
    show_dat_box_string(1, brd[k].dig[0x15].txt,        NULL, &_28_UNV_BOX[i]);
    show_dat_box_string(1, brd[k].dig[0x16].txt,        NULL, &_28_TRIP_BOX[i]);
    show_dat_box_string(1, brd[k].dig[0x17].txt,        NULL, &_28_CMD_BOX[i]);
    show_dat_box_string(1, brd[k].dig[0x05].txt,        NULL, &_28_STAT_BOX[i]);

    r = &r_vol[i];
    value = *(float*)&r->rea[0];
    if      (value < 10.0)   sprintf(b, "%4.2f", value);
    else if (value < 100.0)  sprintf(b, "%4.1f", value);
    else if (value < 1000.0) sprintf(b, "%4.0f", value);
    else                     sprintf(b, "*****");
    show_dat_box_string(1, NULL, b, &_28_VOL_BOX[i]);
  }
}

//~============================================================================
//
//                  INPUT STAGE PANEL
//
//~============================================================================

void create_input_stage_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 20, w2 = 15, w3 = 40, h1 = 20, h2 = 15, h3 = 15;
  _b mska[] = {{0, 0xFFFFFFFF, -1}, {0, 0xFFFFFFFF, -1}};
  char *t1[1] = {"Input Stages"};
  char *t2[2] = {"A", "B"};
  char *t3[2] = {"V", "I"};
  
  w = 5 + 2 * (w1 + w2 + w3) + 5;
  h = 5 + h1 + 2 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  create_frame_box_hstring(1, x, y, w-10, h1, t1, 'N', 'C');

  y = y + h1;
  for (i=0; i<2; i++) {
    create_frame_box_vstring(1, x, y, w1, 2*h2, &t2[i], 'S', 'C');
    x = x + w1;
    create_frame_box_vstring(2, x, y, w2, h2,    t3,    'S', 'C');
    x = x + w2;
    create_dat_box_vstring(
      2, x, y, w3, h2, 'N', &r_inp, mska, &INP_BOX[2*i]);
    x = x + w3;
  }
  y = y + 2 * h2;

  x = x0 + 5;
  obj = add_normal_button(x, y, w-10, h3, "READ  INPUT", 'N');
    fl_set_object_callback(obj, input_stage_callback, i);
}

//~----------------------------------------------------------------------------

void input_stage_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[32];
  _r *r = &r_inp;
  struct {
    char *nam;
    int8 adc[2];
    float min;
    float max;
  } inp[4] = {
    {"Input Stage A Voltage", {0x0D, 0x1D}, 90.0, 153.0},
    {"Input Stage A Current", {0x0C, 0x1C}, 22.0,   0.0},
    {"Input Stage B Voltage", {0x1D, 0x0D}, 90.0, 153.0},
    {"Input Stage B Current", {0x1C, 0x0C}, 22.0,   0.0}};
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<4; i++) r->was[i] = r->rea[i];

  read_ADC(dat, &err);
//err=0;
//for (i=0; i<32; i++) dat[i]=0x0100 + i;

  if (!err) {
    for (i=0; i<4; i++) {
      int8  cha = inp[i].adc[far_side];
      float A   = (inp[i].max - inp[i].min) / 4095.0;
      float B   =               inp[i].min;
      float v;
//printf("i=%d, A=%f, B=%f\n", i, A, B);
      v = A * (float)dat[cha] + B;
      r->rea[i] = *(int32*)&v;
      if (r->unk) r->was[i] = r->rea[i];
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

void update_input_stage_panel(void) {

  int i;
  _r *r = &r_inp;

  for (i=0; i<4; i++) {
    float value;
    char b[80];
    value = *(float*)&r->rea[i];
    if      (value < 10.0)   sprintf(b, "%5.3f", value);
    else if (value < 100.0)  sprintf(b, "%5.2f", value);
    else if (value < 1000.0) sprintf(b, "%5.1f", value);
    else                     sprintf(b, "*****");
    show_dat_box_string(1, NULL, b, &INP_BOX[i]);
  }
}

//~============================================================================
//
//                  ALL OFF BUTTON
//
//~============================================================================

void create_ALL_OFF_button(int x0, int y0) {

  FL_OBJECT *obj;
  int w1 = 60, h1 = 60;
  
  obj = add_normal_button(x0, y0, w1, h1, "ALL\nOFF", 'H');
    fl_set_object_callback(obj, ALL_OFF_callback, 0);
}

//~----------------------------------------------------------------------------

void ALL_OFF_callback(FL_OBJECT *obj, long data) {

  int8 adr, cha, dat;
  int16 cmd;

  if (obj) {
    msg = -1;
    update_objects();
  }

  adr = 0x0A;
  cha = 0x07;
  dat = 1;
  cmd = 0;
  pack16(&cmd, 0x0800, dat);
  pack16(&cmd, 0x0400, 1);
  pack16(&cmd, 0x0200, far_side);
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);
  
  exec_PDS_cmd(cmd, 0x80+0, NULL, NULL, &err);

  if (obj) {
    static char *txt = {"Main PDS Power is OFF!"};
    set_message_text(txt);
    msg = -4;
    update_objects();
  }
}

//~============================================================================
//
//                  28V OUTLETS CONTROL PANEL
//
//~============================================================================

void create_28V_outlets_panel(int x0, int y0) {

  int i;
  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 15, w2 = 10, w3 = 40, w4 = 50, w5 = 30, w6 = 10, w7 = 40, w8 = 20;
  int h1 = 20, h2 = 15, h3 = 15;
  _b msk2[] = {{0, 0x00000001,  2}};
  _b msk3[] = {{0, 0x00000003,  3}};
  _b mska[] = {{0, 0xFFFFFFFF, -1}};
  
  w = 5 + w1 + w2 + w3 + 5 + w4 + 3 * w5 + 5 + w6 + 2 * w5 + w7 + w8 + 5;
  h = 5 + h1 + h2 + h2 + 32 * h2 + + 8 * 5 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "28V Outlets Control", 'M');
  y = y + h1;

  w = w1 + w2 + w3;
  add_frame_box(x, y, w, h2, "DC/DC", 'S');
  x = x + w + 5;
  w = w4 + 3 * w5 + 5 + w6 + 2 * w5 + w7 + w8;
  add_frame_box(x, y, w, h2, "Distributors",   'S');
  y = y + h2;
  
  x = x0 + 5;
  add_frame_box(x, y, w1+w2, h2, "",        'S');
  x = x + w1 + w2;
  add_frame_box(x, y, w3,    h2, "V",       'S');
  x = x + w3 + 5;
  add_frame_box(x, y, w4,    h2, "Outlet",  'S');
  x = x + w4;
  add_frame_box(x, y, 3*w5,  h2, "Command", 'S');
  x = x + 3 * w5 + 5;
  add_frame_box(x, y, w6,    h2, "",        'S');
  x = x + w6;
  add_frame_box(x, y, w5,    h2, "cmd",     'S');
  x = x + w5;
  add_frame_box(x, y, w5,    h2, "sta",     'S');
  x = x + w5;
  add_frame_box(x, y, w7,    h2, "I",       'S');
  x = x + w7;
  add_frame_box(x, y, w8,    h2, "",        'S');
  y = y + h2;

  for (i=0; i<32; i++) {
    char b[80];
    char *t[1] = {b};
    x = x0 + 5;
    if (i && strncmp(out[i].nam, out[i-1].nam, 2)) y = y + 5;
    sprintf(b, "%d", out[i].pb2);
    create_frame_box_vstring(1, x, y, w1, h2, t,           'S', 'C');
    x = x + w1;
    create_dat_box_vstring(
      1, x, y, w2, h2, 'N', &r_in__sta[i],        msk3, &IN__STA_BOX[i]);
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w3, h2, 'N', &r_vol[out[i].pb2-1], mska, &IN__VOL_BOX[i]);
    x = x + w3 + 5;
    create_frame_box_vstring(1, x, y, w4, h2, &out[i].nam, 'S', 'L');
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "ON",  'N');
      fl_set_object_callback(obj, control_28V_outlets_callback, 2*i+1);
    x = x + w5;
    obj = add_normal_button(x, y, w5, h2, "OFF", 'N');
      fl_set_object_callback(obj, control_28V_outlets_callback, 2*i+0);
    x = x + w5;
    create_btn_hstring(
      1, x, y, w5, h2, o_o, 'N', &w_cmd[i], msk3, &OUT_CMD_BTN[i]);
    x = x + w5 + 5;
    create_dat_box_vstring(
      1, x, y, w6, h2, 'N', &r_out_sta[i],        msk3, &OUT_STA_BOX[i]);
    x = x + w6;
    create_dat_box_vstring(
      1, x, y, w5, h2, 'N', &r_out_cmd[i],        msk2, &OUT_CMD_BOX[i]);
    x = x + w5;
    create_dat_box_vstring(
      1, x, y, w5, h2, 'N', &r_out_stat[i],       msk2, &OUT_STAT_BOX[i]);
    x = x + w5;
    create_dat_box_vstring(
      1, x, y, w7, h2, 'N', &r_out_cur[i],        mska, &OUT_CUR_BOX[i]);
    x = x + w7;
    obj = add_normal_button(x, y, w8, h3, "R", 'N');
      fl_set_object_callback(obj, read_28V_outlets_callback, i);
    y = y + h2;
  }

  x = x0 + 5;
  add_frame_box(x, y, w1, h3, "", 'S');
  x = x + w1;
  
  obj = add_normal_button(x, y, w2+w3, h3, "R", 'N');
    fl_set_object_callback(obj, read_28V_DCDC_callback, -1);
  x = x + w2 + w3 + 5;

  add_frame_box(x, y, w4,   h3, "", 'S');
  x = x + w4;
  add_frame_box(x, y, 2*w5, h3, "", 'S');
  x = x + 2 * w5;

  obj = add_normal_button(x, y, w5, h3, "W", 'N');
    fl_set_object_callback(obj, write_28V_outlets_callback, -1);
  x = x + w5 + 5;

  w = w6 + 2 * w5 + w7 + w8;
  obj = add_normal_button(x, y, w, h3, "READ  ALL", 'N');
    fl_set_object_callback(obj, read_28V_outlets_callback, -1);
}

//~----------------------------------------------------------------------------

void control_28V_outlets_callback(FL_OBJECT *obj, long data) {

  int8 ind = data / 2;
  int8 dat = data % 2;
  int8 adr = out[ind].adr;
  int8 cha = out[ind].cmd;
  int16 cmd;

  if (obj) {
    msg = -1;
    update_objects();
  }

  cmd = 0;
  pack16(&cmd, 0x0800, dat);
  pack16(&cmd, 0x0400, 1);
  pack16(&cmd, 0x0200, far_side);
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);
  exec_PDS_cmd(cmd, 0x80+0, NULL, NULL, &err);
    
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void write_28V_outlets_callback(FL_OBJECT *obj, long data) {

  int i;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=0; i<32; i++) {
    if (go_on(err)) {
      _ww *w = &w_cmd[i];
      int8 dat = w->inp[0];
      int8 adr = out[i].adr;
      int8 cha = out[i].cmd;
      err = 0x0000;
      if (dat == 0 || dat == 1) {
        int16 cmd = 0x0000;
        pack16(&cmd, 0x0800, dat);
        pack16(&cmd, 0x0400, 1);
        pack16(&cmd, 0x0200, far_side);
        pack16(&cmd, 0x01E0, adr);
        pack16(&cmd, 0x001F, cha);
        exec_PDS_cmd(cmd, 0x80+0, NULL, NULL, &err);
      }
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

void read_28V_outlets_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=0; i<32; i++) {
    int ii;
    if (ind != -1 && ind != i) continue;
    for (ii=0; ii<3; ii++) {
      int8 adr = out[i].adr;
      int8 adc = brd[adr].adc[far_side];
      int8 cha = 0xFF;
      int8 dig;
      int16 ana;
      _r *r = NULL;
      int16 cmd = 0x0000;
      switch (ii) {
        case 0: r = &r_out_cmd[i];  cha = out[i].cmd; break;
        case 1: r = &r_out_stat[i]; cha = out[i].sta; break;
        case 2: r = &r_out_cur[i];  cha = out[i].cur; break;
      }
      if (go_on(err)) {
        r->was[0] = r->rea[0];
        cmd = 0;
        pack16(&cmd, 0x0200, far_side);
        pack16(&cmd, 0x01E0, adr);
        pack16(&cmd, 0x001F, cha);
        exec_PDS_cmd(cmd, 0x80+adc, &dig, &ana, &err);
        if (!err) {
          if (r != &r_out_cur[i]) {
            r->rea[0] = dig & 0x01;
          }
          else {
            float A = (brd[adr].ana[cha].max - brd[adr].ana[cha].min) / 4095.0;
            float B =                          brd[adr].ana[cha].min;
            float v = A * (float)ana + B;
            r->rea[0] = *(int32*)&v;
          }
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

void update_28V_outlets_panel(void) {

  int i;

  for (i=0; i<32; i++) {
    int8 k = out[i].adr;
    float value;
    char b[80];
    _r *r;

    show_btn_string(1, NULL, &OUT_CMD_BTN[i]);
    show_dat_box_string(1, brd[k].dig[out[i].cmd].txt,  NULL, &OUT_CMD_BOX[i]);
    show_dat_box_string(1, brd[k].dig[out[i].sta].txt,  NULL, &OUT_STAT_BOX[i]);

    r = &r_vol[out[i].pb2-1];
    value = *(float*)&r->rea[0];
    if      (value < 10.0)   sprintf(b, "%4.2f", value);
    else if (value < 100.0)  sprintf(b, "%4.1f", value);
    else if (value < 1000.0) sprintf(b, "%4.0f", value);
    else                     sprintf(b, "*****");
    show_dat_box_string(1, NULL, b, &IN__VOL_BOX[i]);

    r = &r_out_cur[i];
    value = *(float*)&r->rea[0];
    if      (value < 10.0)   sprintf(b, "%4.2f", value);
    else if (value < 100.0)  sprintf(b, "%4.1f", value);
    else if (value < 1000.0) sprintf(b, "%4.0f", value);
    else                     sprintf(b, "*****");
    show_dat_box_string(1, NULL, b, &OUT_CUR_BOX[i]);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  far_side = ((w_i_f.inp[0] & i_f.msk) != (w_side.inp[0] & side.msk));

  update_overall_status_panel();
  update_28V_DCDC_panel();
  update_input_stage_panel();

  update_28V_outlets_panel();
//update_120V_outlets_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
