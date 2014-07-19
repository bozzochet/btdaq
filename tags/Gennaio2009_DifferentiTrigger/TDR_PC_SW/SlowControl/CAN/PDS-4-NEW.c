// file PDS-4-NEW.c
//
// PDS-4-NEW Controller - uses new CGS protocol
//
// A.Lebedev Dec-2007...

#include "template.h"
#include "pdsdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"PDS-4-NEW  Controller"};              // global variable
char *date  = {"13-Dec-08"};                          // global variable

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

//~--

#define NL 32

//~-- PDS Model Selector

SW_BTN *MODEL_BTN;
_ww w_model;
_b model = {0, 0x00000001, 2};
char *models[2] = {"EM", "FM"};

//~-- CAN I/F ("USCM") Selector

SW_BTN *I_F_BTN;
_ww w_i_f;
_b i_f = {0, 0x0001, 2};
char *i_fs[2] = {"A", "B"};

//~-- PDS Side Selector

SW_BTN *SIDE_BTN;
_ww w_side;
_b side = {0, 0x0001, 2};
char *sides[2] = {"A", "B"};

//~-- Board Selector

DAT_BOX *STA_BOX[10];
DAT_BOX *OVT_BOX[10];
DAT_BOX *TMP_BOX[10];
DAT_BOX *REF_BOX[10];
_r r_sta[10];
_r r_ovt[10];
_r r_tmp[10];
_r r_ref[10];
int slot;
int far_side;
char brd_nam[80];

//~-- Input Stage

DAT_BOX *INP_COD_BOX[4], *INP_VAL_BOX[4];
_r r_inp;

//~-- Readout and Commanding Panel

FL_OBJECT *BRD_NAM;

FL_OBJECT *DIG_NAM_BOX[NL];
DAT_BOX *DIG_COD_BOX[NL], *DIG_TXT_BOX[NL];
_dig *pdig[NL];
_r r_dig[NL];
SW_BTN *DIG_PER_BTN;
_ww w_dig_per;
_b dig_per = {0, 0x00000007, 6};
bool auto_read_digital = FALSE;

FL_OBJECT *ANA_NAM_BOX[NL];
DAT_BOX *ANA_COD_BOX[NL], *ANA_VAL_BOX[NL];
_ana *pana[NL];
_r r_ana[NL];
SW_BTN *ANA_PER_BTN;
_ww w_ana_per;
_b ana_per = {0, 0x00000007, 6};
bool auto_read_analog = FALSE;

FL_OBJECT *CMD_NAM_BOX[NL];
SW_BTN *CMD_DAT_BTN[NL], *CMD_TXT_BTN[NL];
_cmd *pcmd[NL];
_ww w_cmd[NL];

//~--

char *bin[2]     = {"0", "1"};
char *a_b[2]     = {"A", "B"};
char *dash[2]    = {"-", "-"};
char *periods[6] = {"0.2s", "0.5s", "1.0s", "2.0s", "5.0s", "10.0s"};

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

void create_board_selector_panel(int x0, int y0);
void board_selector_callback(FL_OBJECT *obj, long data);
void read_overall_status_callback(FL_OBJECT *obj, long data);
void update_board_selector_panel(void);

void create_input_stage_panel(int x0, int y0);
void input_stage_callback(FL_OBJECT *obj, long data);
void update_input_stage_panel(void);

void create_readout_and_commanding_panel(int x0, int y0);
void digital_readout_callback(FL_OBJECT *obj, long data);
void auto_read_digital_callback(int tid, void *data);
void analog_readout_callback(FL_OBJECT *obj, long data);
void auto_read_analog_callback(int tid, void *data);
void digital_commanding_callback(FL_OBJECT *obj, long data);
void update_readout_and_commanding_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 955, HH = 640;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_box_model_selector_panel(200,   0);
  box_model_callback(0);

  create_CAN_bus_I_F_selector_panel(0,  45);
  create_PDS_side_selector_panel( 140,  45);

  create_board_selector_panel(  0,  75);
  create_input_stage_panel(     0, 425);
  create_USCM_control_H3_panel(40, 530);

  create_readout_and_commanding_panel(245, 45);
  
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
  }

  invalidate_r(first, &r_inp, 8);

  for (i=0; i<NL; i++) {
    invalidate_r(first, &r_dig[i], 1);
    invalidate_r(first, &r_ana[i], 1);
    invalidate_ww(first, &w_cmd[i], 1, 0);
  }
  
  invalidate_ww(first, &w_dig_per, 1, 0);
  invalidate_ww(first, &w_ana_per, 1, 0);

  read_dat_config_file(dat_config_file_name);

  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
//int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(w_model.inp, 4, 1, file);
    fread(w_i_f.inp,   4, 1, file);
    fread(w_side.inp,  4, 1, file);
    fread(&slot,       4, 1, file);
//  for (i=0; i<NL; i++) fread(w_cmd[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
//int i;

  file = fopen(filename, "w");
  if (file) {
    fwrite(w_model.inp, 4, 1, file);
    fwrite(w_i_f.inp,   4, 1, file);
    fwrite(w_side.inp,  4, 1, file);
    fwrite(&slot,       4, 1, file);
//  for (i=0; i<NL; i++) fwrite(w_cmd[i].inp, 4, 1, file);
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
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Model", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, models, 'M', &w_model, &model, &MODEL_BTN);
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
  int w1 = 110, w2 = 20;
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
  int w1 = 70, w2 = 25;
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
//                  BOARD_SELECTOR_PANEL
//
//~============================================================================

void create_board_selector_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 80, w3 = 25, w4 = 30, w5 = 35, w6 = 35;
  int h1 = 20, h2 = 30, h3 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + 5;
  h = 5 + h1 + 10 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2, h1, "",  'S');
  x = x + w1 + w2;
  add_frame_box(x, y, w3, h1, "Sta",  'S');
  x = x + w3;
  add_frame_box(x, y, w4, h1, "OVT",  'S');
  x = x + w4;
  add_frame_box(x, y, w5, h1, "Temp", 'S');
  x = x + w5;
  add_frame_box(x, y, w6, h1, "Ref",  'S');
  y = y + h1;

  for (i=0; i<10; i++) {
    _b msk = {0, 0xFFFFFFFF, -1};
    char b[80];
    x = x0 + 5;
    sprintf(b, "%d", i+1);
    add_frame_box(x, y, w1, h2, b, 'L');
    x = x + w1;
    obj = add_normal_button(x, y, w2, h2, brd[i].nam, 'N');
      fl_set_object_callback(obj, board_selector_callback, i);
    x = x + w2;
    create_dat_box_vstring(1, x, y, w3, h2, 'N', &r_sta[i], &msk, &STA_BOX[i]);
    x = x + w3;
    create_dat_box_vstring(1, x, y, w4, h2, 'N', &r_ovt[i], &msk, &OVT_BOX[i]);
    x = x + w4;
    create_dat_box_vstring(1, x, y, w5, h2, 'N', &r_tmp[i], &msk, &TMP_BOX[i]);
    x = x + w5;
    create_dat_box_vstring(1, x, y, w6, h2, 'N', &r_ref[i], &msk, &REF_BOX[i]);
    y = y + h2;
  }
  
  x = x0 + 5;
  add_frame_box(x, y, w1+w2, h3, "",  'S');

  x = x + w1 + w2;
  obj = add_normal_button(x, y, w3+w4+w5+w6, h3, "READ", 'N');
    fl_set_object_callback(obj, read_overall_status_callback, 0);
}

//~----------------------------------------------------------------------------

void board_selector_callback(FL_OBJECT *obj, long data) {

  msg = 0;
  invalidate_data();
  slot = data;
  update_objects();
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
    int8 adr = brd[i].adr;
    int8 adc = brd[i].adc[far_side];
    int16 cmd;
    int8  dig;
    int16 ana;

//~--- Board Status
    
    if (go_on(err)) {
      _r *r = &r_sta[i];
      int8 cha = brd[i].sta;
      r->was[0] = r->rea[0];
      cmd = 0;
      pack16(&cmd, 0x0200, far_side);
      pack16(&cmd, 0x01E0, adr);
      pack16(&cmd, 0x001F, cha);
      exec_PDS_cmd(cmd, 0x80+adc, &dig, NULL, &err);
      if (!err) {
        r->rea[0] = dig & 0x01;
        if (r->unk) r->was[0] = r->rea[0];
        r->unk = FALSE;
        r->sta = status_OK;
      }
      else {
        r->sta = status_KO;
      }
    }

//~--- Board OVT

    if (go_on(err)) {
      _r *r = &r_ovt[i];
      int8 cha = brd[i].ovt;
      if (cha != 0xFF) {
        r->was[0] = r->rea[0];
        cmd = 0;
        pack16(&cmd, 0x0200, far_side);
        pack16(&cmd, 0x01E0, adr);
        pack16(&cmd, 0x001F, cha);
        exec_PDS_cmd(cmd, 0x80+adc, &dig, NULL, &err);
        if (!err) {
          r->rea[0] = dig & 0x01;
          if (r->unk) r->was[0] = r->rea[0];
          r->unk = FALSE;
          r->sta = status_OK;
        }
        else {
          r->sta = status_KO;
        }
      }
    }
    
//~--- Board Temperature

    if (go_on(err)) {
      _r *r = &r_tmp[i];
      int8 cha = brd[i].tmp;
      r->was[0] = r->rea[0];
      cmd = 0;
      pack16(&cmd, 0x0200, far_side);
      pack16(&cmd, 0x01E0, adr);
      pack16(&cmd, 0x001F, cha);
      exec_PDS_cmd(cmd, 0x80+adc, NULL, &ana, &err);
      if (!err) {
        float A = (brd[i].ana[cha].max - brd[i].ana[cha].min) / 4095.0;
        float B =                        brd[i].ana[cha].min;
        float v = A * (float)ana + B;
        r->rea[0] = *(int32*)&v;
        if (r->unk) r->was[0] = r->rea[0];
        r->unk = FALSE;
        r->sta = status_OK;
      }
      else {
        r->sta = status_KO;
      }
    }
    
//~--- Board Reference Voltage

    if (go_on(err)) {
      _r *r = &r_ref[i];
      int8 cha = brd[i].ref;
      r->was[0] = r->rea[0];
      cmd = 0;
      pack16(&cmd, 0x0200, far_side);
      pack16(&cmd, 0x01E0, adr);
      pack16(&cmd, 0x001F, cha);
      exec_PDS_cmd(cmd, 0x80+adc, NULL, &ana, &err);
      if (!err) {
        float A = (brd[i].ana[cha].max - brd[i].ana[cha].min) / 4095.0;
        float B =                        brd[i].ana[cha].min;
        float v = A * (float)ana + B;
        r->rea[0] = *(int32*)&v;
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

void update_board_selector_panel(void) {

  int i;
  _r *r;

  for (i=0; i<strlen(brd[slot].nam); i++) {
    if (brd[slot].nam[i] == '\n') brd_nam[i] = ' ';
    else                          brd_nam[i] = brd[slot].nam[i];
  }
  brd_nam[i] = '\0';

//printf("\n%20s Adr=0x%02X ADC=0x%02X\n", 
//brd_nam, brd[slot].adr, brd[slot].adc[far_side]);

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
    show_dat_box_string(1,  NULL,     b, &REF_BOX[i]);
  }
}

//~============================================================================
//
//                  INPUT_STAGE_PANEL
//
//~============================================================================

void create_input_stage_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 25, w2 = 50, w3 = 40;
  int h1 = 20, h2 = 15, h3 = 25;
  char *t1[2] = {"Input Stage A", "Input Stage B"};
  char *t2[2] = {"V", "I"};
  
  w = 5 + w1 + w2 + w3 + 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 2 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  create_frame_box_hstring(2, x, y, w1+w2+w3, h1, t1, 'N'+100, 'C');

  y = y + h1;
  for (i=0; i<2; i++) {
    _b msk[2] = {{0, 0xFFFFFFFF, -1}, {0, 0xFFFFFFFF, -1}};
    create_frame_box_vstring(2, x, y, w1, h2, t2, 'S', 'C');
    x = x + w1;
    create_dat_box_vstring(
      2, x, y, w2, h2, 'N', &r_inp, msk, &INP_COD_BOX[2*i]);
    x = x + w2;
    create_dat_box_vstring(
      2, x, y, w3, h2, 'N', &r_inp, msk, &INP_VAL_BOX[2*i]);
    x = x + w3 + 5;
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

  for (i=0; i<8; i++) r->was[i] = r->rea[i];

  read_ADC(dat, &err);
//err=0;
//for (i=0; i<32; i++) dat[i]=0x0100 + i;

  if (!err) {
    for (i=0; i<4; i++) {
      float A = (inp[i].max - inp[i].min) / 4095.0;
      float B =               inp[i].min;
      float v;
//printf("i=%d, A=%f, B=%f\n", i, A, B);
      r->rea[i] = dat[inp[i].adc[far_side]];
      v = A * (float)r->rea[i] + B;
      r->rea[i+4] = *(int32*)&v;
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
    sprintf(b, "0x%04X", r->rea[i]);
    show_dat_box_string(1, NULL, b, &INP_COD_BOX[i]);
    value = *(float*)&r->rea[4+i];
    if      (value < 10.0)   sprintf(b, "%5.3f", value);
    else if (value < 100.0)  sprintf(b, "%5.2f", value);
    else if (value < 1000.0) sprintf(b, "%5.1f", value);
    else                     sprintf(b, "*****");
    show_dat_box_string(1, NULL, b, &INP_VAL_BOX[i]);
  }
}

//~============================================================================
//
//                  READOUT AND COMMANDING PANEL
//
//~============================================================================

void create_readout_and_commanding_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  char b[80];
  int xx0, yy0, x, y, w, h;
  int ww, hh = 35;
  
{//~-- Create Digital Readout Panel

  int w1 = 20, w2 = 135, w3 = 15, w4 = 35, w5 = 20, h1 = 20, h2 = 15;
  int w6 = 45, w7 = 45, w8 = 135, h3 = 20;

  xx0 = x0;
  yy0 = y0;
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + hh + h1 + NL * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, xx0, yy0, w, h, " ");
  ww = w;

  x = xx0 + 5;
  y = yy0 + 5 + hh;
  add_frame_box(x, y, w1+w2, h1, "Digital Readout", 'N');
  x = x + w1 + w2;
  add_frame_box(x, y, w3+w4, h1, "Data",            'N');
  x = x + w3 + w4;
  add_frame_box(x, y, w5,    h1, "",                'N');

  y = y + h1;
  for (i=0; i<NL; i++) {
    _b msk = {0, 0x00000001, 2};
    x = xx0 + 5;
    sprintf(b, "%02X", i);
    add_frame_box(x, y, w1, h2, b, 'S');
    x = x + w1;
    DIG_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", 'S');
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    create_dat_box_vstring(1, x, y, w3, h2, 'N', &r_dig[i], &msk, &DIG_COD_BOX[i]);
    x = x + w3;
    create_dat_box_vstring(1, x, y, w4, h2, 'N', &r_dig[i], &msk, &DIG_TXT_BOX[i]);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "R", 'N');
      fl_set_object_callback(obj, digital_readout_callback, i);
    y = y + h2;
  }

  x = xx0 + 5;
  add_frame_box(x, y, w6, h3, "Period", 'N');
  x = x + w6;
  create_btn_hstring(
    1, x, y, w7, h3, periods, 'N', &w_dig_per, &dig_per, &DIG_PER_BTN);
  x = x + w7;
  obj = add_normal_button(x, y, w8, h3, "READ  ALL", 'N');
    fl_set_object_callback(obj, digital_readout_callback, -1);
}

{//~-- Create Analog Readout Panel

  int w1 = 20, w2 = 135, w3 = 50, w4 = 40, w5 = 20, h1 = 20, h2 = 15;
  int w6 = 45, w7 = 45, w8 = 175, h3 = 20;

  xx0 = xx0 + w;
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + hh + h1 + NL * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, xx0, yy0, w, h, " ");
  ww = ww + w;

  x = xx0 + 5;
  y = yy0 + 5 + hh;
  add_frame_box(x, y, w1+w2, h1, "Analog Readout", 'N');
  x = x + w1 + w2;
  add_frame_box(x, y, w3,    h1, "Code",           'N');
  x = x + w3;
  add_frame_box(x, y, w4,    h1, "Value",          'N');
  x = x + w4;
  add_frame_box(x, y, w5,    h1, "",               'N');

  y = y + h1;
  for (i=0; i<NL; i++) {
    _b msk = {0, 0xFFFFFFFF, -1};
    x = xx0 + 5;
    sprintf(b, "%02X", i);
    add_frame_box(x, y, w1, h2, b, 'S');
    x = x + w1;
    ANA_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", 'S');
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    create_dat_box_vstring(1, x, y, w3, h2, 'N', &r_ana[i], &msk, &ANA_COD_BOX[i]);
    x = x + w3;
    create_dat_box_vstring(1, x, y, w4, h2, 'N', &r_ana[i], &msk, &ANA_VAL_BOX[i]);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "R", 'N');
      fl_set_object_callback(obj, analog_readout_callback, i);
    y = y + h2;
  }

  x = xx0 + 5;
  add_frame_box(x, y, w6, h3, "Period", 'N');
  x = x + w6;
  create_btn_hstring(
    1, x, y, w7, h3, periods, 'N', &w_ana_per, &ana_per, &ANA_PER_BTN);
  x = x + w7;
  obj = add_normal_button(x, y, w8, h3, "READ  ALL", 'N');
    fl_set_object_callback(obj, analog_readout_callback, -1);
}

{//~-- Create Commanding Panel

  int w1 = 20, w2 = 105, w3 = 15, w4 = 30, w5 = 20, h1 = 20, h2 = 15;
  int h3 = 20;

  xx0 = xx0 + w;
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + hh + h1 + NL * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, xx0, yy0, w, h, " ");
  ww = ww + w;

  x = xx0 + 5;
  y = yy0 + 5 + hh;
  add_frame_box(x, y, w1+w2, h1, "Commanding", 'N');
  x = x + w1 + w2;
  add_frame_box(x, y, w3+w4, h1, "Data",           'N');
  x = x + w3 + w4;
  add_frame_box(x, y, w5,    h1, "",               'N');

  y = y + h1;
  for (i=0; i<NL; i++) {
    _b msk = {0, 0x00000001, 2};
    x = xx0 + 5;
    sprintf(b, "%02X", i);
    add_frame_box(x, y, w1, h2, b, 'S');
    x = x + w1;
    CMD_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", 'S');
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    create_btn_hstring(1, x, y, w3, h2, bin, 'N',  &w_cmd[i], &msk, &CMD_DAT_BTN[i]);
    x = x + w3;
    create_btn_hstring(1, x, y, w4, h2, bin, 'N', &w_cmd[i], &msk, &CMD_TXT_BTN[i]);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "W", 'N');
      fl_set_object_callback(obj, digital_commanding_callback, i);
    y = y + h2;
  }

  x = xx0 + 5;
  add_frame_box(x, y, w-10, h3, "", 'N');
}

{//~-- Create Panel Headline

  x = x0 + 5;
  y = y0 + 5;
  BRD_NAM = add_frame_box(x, y, ww-10, hh, "", 'H');
}

//~--

}

//~----------------------------------------------------------------------------

void digital_readout_callback(FL_OBJECT *obj, long data) {

  int8 cha = data;
  bool read_all;
  int i;
  int8 adr, dat;
  int16 cmd;

  if (obj && (cha == 0xFF)) {
    if (auto_read_digital) {
      auto_read_digital = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_read_digital = TRUE;
        fl_add_timeout(0, auto_read_digital_callback, NULL);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_read_digital) {
    msg = -1;
    update_objects();
  }

  read_all = (cha == 0xFF) ? TRUE : FALSE;
  
  err = 0x0000;

  for (i=0; i<NL; i++) {
    _r *r = &r_dig[i];
    if ( read_all && !pdig[i]) continue;
    if (!read_all && cha != i) continue;
    
    if (go_on(err)) {
      r->was[0] = r->rea[0];
      adr = brd[slot].adr;
      cmd = 0;
      pack16(&cmd, 0x0200, far_side);
      pack16(&cmd, 0x01E0, adr);
      pack16(&cmd, 0x001F, i);
      exec_PDS_cmd(cmd, 0x80+0, &dat, NULL, &err);
      if (!err) {
        r->rea[0] = dat;
        if (r->unk) r->was[0] = r->rea[0];
        r->sta = status_OK;
        r->unk = FALSE;
      }
      else {
        r->sta = status_KO;
      }
    }
  }

  if (auto_read_digital) msg = -3;
  else                   msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void auto_read_digital_callback(int tid, void *data) {

  int period = 1000.0 * atof(periods[*w_dig_per.inp]);

  if (!auto_read_digital) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback %4d %f\n", period, delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  digital_readout_callback(NULL, -1);
  fl_add_timeout(period, auto_read_digital_callback, NULL);
}

//~----------------------------------------------------------------------------

void analog_readout_callback(FL_OBJECT *obj, long data) {

  int8 cha = data;
  bool read_all;
  int i;
  int8 adr, adc;
  int16 dat;
  int16 cmd;
  
  if (obj && (cha == 0xFF)) {
    if (auto_read_analog) {
      auto_read_analog = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_read_analog = TRUE;
        fl_add_timeout(0, auto_read_analog_callback, NULL);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_read_analog) {
    msg = -1;
    update_objects();
  }

  read_all = (cha == 0xFF) ? TRUE : FALSE;

  err = 0x0000;

  for (i=0; i<NL; i++) {
    _r *r = &r_ana[i];
    if ( read_all && !pana[i]) continue;
    if (!read_all && cha != i) continue;
    if (go_on(err)) {
      r->was[0] = r->rea[0];
      adr = brd[slot].adr;
      adc = brd[slot].adc[far_side];
      cmd = 0;
      pack16(&cmd, 0x0200, far_side);
      pack16(&cmd, 0x01E0, adr);
      pack16(&cmd, 0x001F, i);
      exec_PDS_cmd(cmd, 0x80+adc, NULL, &dat, &err);
      if (!err) {
        r->rea[0] = dat;
        if (r->unk) r->was[0] = r->rea[0];
        r->sta = status_OK;
        r->unk = FALSE;
      }
      else {
        r->sta = status_KO;
      }
    }
  }

  if (auto_read_analog) msg = -3;
  else                  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void auto_read_analog_callback(int tid, void *data) {

  int period = 1000.0 * atof(periods[*w_ana_per.inp]);

  if (!auto_read_analog) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback %4d %f\n", period, delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  analog_readout_callback(NULL, -1);
  fl_add_timeout(period, auto_read_analog_callback, NULL);
}

//~----------------------------------------------------------------------------

void digital_commanding_callback(FL_OBJECT *obj, long data) {

  int8 cha = data;
  int8 adr, dat;
  int16 cmd;
  _ww *w = &w_cmd[cha];

  if (obj) {
    msg = -1;
    update_objects();
  }

  adr = brd[slot].adr;
  dat = w->inp[0];
  cmd = 0;
  pack16(&cmd, 0x0800, dat);
  pack16(&cmd, 0x0400, 1);
  pack16(&cmd, 0x0200, far_side);
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);
  
  exec_PDS_cmd(cmd, 0x80+0, NULL, NULL, &err);
    
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

//~----------------------------------------------------------------------------

void update_readout_and_commanding_panel(void) {

  int i;
  static int8  old_slot  = -1;
  static int32 old_model = -1;
  char b[80];
  
{//~-- Update Panel Headline

  fl_set_object_label(BRD_NAM, brd_nam);
}

{//~-- Update Digital Readout Panel

  if (slot != old_slot || *w_model.inp != old_model) {
    for (i=0; i<NL; i++) {
      pdig[i] = brd[slot].dig[i].nam ? &brd[slot].dig[i] : NULL;
      if (pdig[i]) fl_set_object_label(DIG_NAM_BOX[i], pdig[i]->nam);
      else         fl_set_object_label(DIG_NAM_BOX[i], "");
    }
  }

  for (i=0; i<NL; i++) {
    char **p;
    show_dat_box_string(1, bin,  NULL, &DIG_COD_BOX[i]);
    if (pdig[i]) p = pdig[i]->txt;
    else         p = dash;
    show_dat_box_string(1, p, NULL, &DIG_TXT_BOX[i]);
  }
}

{//~-- Update Analog Readout Panel

  if (slot != old_slot || *w_model.inp != old_model) {
    for (i=0; i<NL; i++) {
      pana[i] = brd[slot].ana[i].nam ? &brd[slot].ana[i] : NULL;
      if (pana[i]) fl_set_object_label(ANA_NAM_BOX[i], pana[i]->nam);
      else         fl_set_object_label(ANA_NAM_BOX[i], "");
    }
  }

  for (i=0; i<NL; i++) {
    _r *r = &r_ana[i];
    if (pana[i]) {
      float A = (pana[i]->max - pana[i]->min) / 4095.0;
      float B =                 pana[i]->min;
      float v = A * (float)r->rea[0] + B;
      show_dat_box_string(1, NULL, "0x%04X", &ANA_COD_BOX[i]);
      if      (v <   10.0) sprintf(b, "%5.3f", v);
      else if (v <  100.0) sprintf(b, "%5.2f", v);
      else if (v < 1000.0) sprintf(b, "%5.1f", v);
      else                 sprintf(b, "*****");
      show_dat_box_string(1, NULL, b,        &ANA_VAL_BOX[i]);
    }
    else {
      show_dat_box_string(1, NULL, "",       &ANA_COD_BOX[i]);
      show_dat_box_string(1, NULL, "",       &ANA_VAL_BOX[i]);
    }
  }
}

{//~-- Update Commanding Panel

  if (slot != old_slot || *w_model.inp != old_model) {
    for (i=0; i<NL; i++) {
      pcmd[i] = brd[slot].cmd[i].nam ? &brd[slot].cmd[i] : NULL;
      if (pcmd[i]) fl_set_object_label(CMD_NAM_BOX[i], pcmd[i]->nam);
      else         fl_set_object_label(CMD_NAM_BOX[i], "");
    }
  }

  for (i=0; i<NL; i++) {
    SW_BTN *B = CMD_TXT_BTN[i];
    show_btn_string(1, NULL, &CMD_DAT_BTN[i]);
    if (pcmd[i]) B->l = pcmd[i]->txt;
    else         B->l = dash;
    show_btn_string(1, NULL, &CMD_TXT_BTN[i]);
  }
}

//~--

  old_slot  = slot;
  old_model = *w_model.inp;
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  far_side = ((w_i_f.inp[0] & i_f.msk) != (w_side.inp[0] & side.msk));

  update_board_selector_panel();
  update_input_stage_panel();
  update_readout_and_commanding_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
