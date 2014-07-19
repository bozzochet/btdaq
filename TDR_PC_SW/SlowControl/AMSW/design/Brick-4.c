// file Brick-4.c
//
// HV Brick Controller for ECAL, RICH, TOF
//
// A.Lebedev Mar-2008...

#include "bricklib.h"
#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title;                                        // global variable
char *date  = {"13-Dec-08"};                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"

int TX = 1;                 // global variable
int RX = 1;                 // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

int8  bus;                  // global variable
int16 geo;                  // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~-- HV Brick Model Selector

#define NLAY 3

int ilay;

//~-- Module Address

SW_BTN  *BUS_BTN[1];
_w w_bus;
SW_BTN  *GEO_BTN[2];
_w w_geo;
DAT_BOX *BUS_BOX[8];
DAT_BOX *GEO_BOX[8];
_r r_adr[8];

//~-- Statuses

DAT_BOX *STA_BOX[8];
_r r_sta;

//~-- Errors

DAT_BOX *ERR_BOX[6][8];
_r r_err[6];

//~-- HV Channels (Linear Regulators and DCDCs)

DAT_BOX *CHA_BOX[42];
SW_BTN  *CHA_BTN[42]; 
_r   r_cha[42];
_ww ww_cha[42];
bool follow[42], follow_all[2];

//~--

SW_BTN  *DCDC_BTN[2]; 
_ww  ww_dcdc[2];

//~--

_b sta_msk[8] = {
  {0, 0x80, 2},       // Alarm
  {0, 0x40, 2},       // LRG0 Status
  {0, 0x20, 2},       // LRG1 Status
  {0, 0x10, 2},       // LRG2 Status
  {0, 0x08, 2},       // LRG3 Status
  {0, 0x04, 2},       // LRG4 Status
  {0, 0x02, 2},       // DCDC0 Status
  {0, 0x01, 2},       // DCDC1 Status
};

//~--

#define ECAL 0
#define RICH 1
#define TOF  2

struct {
  char *tag;
  char *title;
  char *controller;
} layout[NLAY] = {

 {"Brick-4-ECAL", "HV Brick Controller - 4  (ECAL)", "JINF Path"},
 {"Brick-4-RICH", "HV Brick Controller - 4  (RICH)", "JINR Path"},
 {"Brick-4-TOF",  "HV Brick Controller - 4  (TOF)",  "SDR2 Path"}};

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *o_t[2] = {"OK", "TRIP"};    // for DCDC
char *t_o[2] = {"TRIP", "OK"};    // for LR Groups

char *o_o[2] = {"DCDC OFF", "DCDC ON"};

char *o_a[2] = {"OK", "ALARM"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);
void reset_everything_callback(FL_OBJECT *obj, long data);
void shutdown_callback(FL_OBJECT *obj, long data);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_change_layout_panel(int x0, int y0);
void change_layout_callback(FL_OBJECT *obj, long data);

void create_module_address_panel(int x0, int y0);
void init_callback(FL_OBJECT *obj, long data);
void update_module_address_panel(void);

void create_alarm_panel(int x0, int y0);
void update_alarm_panel(void);

void create_find_brick_panel(int x0, int y0);
void find_brick_callback(FL_OBJECT *obj, long data);
void update_find_brick_panel(void);

void create_channels_panel(int x0, int y0, int ind);
void convert_LR_channel_code_to_value(int32 code, int32 *value);
void convert_DCDC_channel_code_to_value(int32 code, int32 *value);
void set_channel_voltage_callback(FL_OBJECT *obj, long data);
void channel_write_callback(FL_OBJECT *obj, long data);
void channel_read_callback(FL_OBJECT *obj, long data);
void channel_reset_callback(FL_OBJECT *obj, long data);
void all_LR_channel_write_callback(FL_OBJECT *obj, long data);
void all_LR_channel_read_callback(FL_OBJECT *obj, long data);
void all_LR_channel_reset_callback(FL_OBJECT *obj, long data);
void status_read_callback(FL_OBJECT *obj, long data);
void set_DCDC_callback(FL_OBJECT *obj, long data);
void update_channels_panel(void);

void create_error_readout_panel(int x0, int y0);
void error_readout_callback(FL_OBJECT *obj, long data);
void all_error_readout_callback(FL_OBJECT *obj, long data);
void update_error_readout_panel(void);

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

  FL_OBJECT *obj;
  int WW = 540, HH = 640;
  int w1 = WW, h1 = 25;

  invalidate_data();
  ilay = MIN(ilay, NLAY-1);

  sprintf(progname, "%s", layout[ilay].tag);
  title = layout[ilay].title;
  bgn_form(&MAIN, 0, 0, WW, HH, layout[ilay].controller);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_change_layout_panel(   0, 45);
  create_module_address_panel(130, 45);
  create_alarm_panel(         245, 45);
  create_find_brick_panel(    335, 45);

  create_channels_panel(  0, 85, 0);

  create_error_readout_panel(      0, 430);
  create_config_file_panel(      165, 430);
  create_read_write_update_panel(165, 475);

  create_channels_panel(270, 85, 1);

  obj = add_normal_button(0, 560, w1, h1, "RESET  ALL", 'N');
    fl_set_object_callback(obj, reset_everything_callback, 0);
  obj = add_normal_button(0, 585, w1, h1, "SHUT  DOWN", 'N');
    fl_set_object_callback(obj, shutdown_callback, 0);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  ilay = 0;
  
  invalidate_w(&w_bus, 1, 0);
  invalidate_w(&w_geo, 1, 1);

  for (i=0; i<8; i++) {
    invalidate_r(first, &r_adr[i], 2);
  }

  invalidate_r(first, &r_sta, 1);

  for (i=0; i<2; i++) {
    follow_all[i] = FALSE;
  }
  for (i=0; i<42; i++) {
    follow[i] = FALSE;
    invalidate_r(first,  &r_cha[i],  1);
    invalidate_ww(first, &ww_cha[i], 1, 0);
  }

  for (i=0; i<2; i++) {
    invalidate_ww(first, &ww_dcdc[i], 1, 0);
  }

  for (i=0; i<6; i++) {
    invalidate_r(first, &r_err[i], 1);
  }

  read_dat_config_file(dat_config_file_name);
  ilay = MIN(NLAY, ilay);
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
  int i;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&ilay,      4, 1, file);
    fread(&w_bus.inp, 4, 1, file);
    fread(&w_geo.inp, 4, 1, file);
    for (i=0; i<42; i++) fread(ww_cha[i].inp, 4, 1, file);
    for (i=0; i<2; i++) fread(ww_dcdc[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(&ilay,      4, 1, file);
    fwrite(&w_bus.inp, 4, 1, file);
    fwrite(&w_geo.inp, 4, 1, file);
    for (i=0; i<42; i++) fwrite(ww_cha[i].inp, 4, 1, file);
    for (i=0; i<2; i++) fwrite(ww_dcdc[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

void reset_everything_callback(FL_OBJECT *obj, long data) {

  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=0; i<42; i++) {
    if (go_on(err)) channel_reset_callback(NULL, i);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void shutdown_callback(FL_OBJECT *obj, long data) {

  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<42; i++) {
    _r *r = &r_cha[i];
    r->sta = status_NA;
  }

  r_sta.sta = status_NA;

  brick_shutdown(&chk, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            CHANGE LAYOUT PANEL
//
//~============================================================================

void create_change_layout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h, w1 = 40, h1 = 30;
  char *txt[] = {"ECAL", "RICH", "TOF"};

  w = 5 + NLAY * w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  for (i=0; i<NLAY; i++) {
    obj = add_normal_button(x, y, w1, h1, txt[i], 'N');
      fl_set_object_callback(obj, change_layout_callback, i);
    x = x + w1;
  }
}

//~----------------------------------------------------------------------------

void change_layout_callback(FL_OBJECT *obj, long data) {

  ilay = data;

  write_dat_config_file(dat_config_file_name);
  
  create_main_form();

  msg = 0;
  update_objects();
}

//~============================================================================
//
//            MODULE ADDRESS PANEL
//            
//~============================================================================

void create_module_address_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w, h, w1 = 25, w2 = 30, w3 = 45, h1 = 15, h2 = 15;
  
  w = 5 + w1 + w2 + 5 + w3 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x,    y, w1, h1, "Bus", 'S');
  add_frame_box(x+w1, y, w2, h1, "GEO", 'S');

  y = y + h1;
  create_hex_btn_hstring(1, x, y, w1, h2, hex, 'N', &w_bus, &BUS_BTN[0]);
  BUS_BTN[0]->n = 8;
  x = x + w1;

  create_hex_btn_hstring(2, x, y, w2/2, h2, hex, 'N', &w_geo, &GEO_BTN[0]);
  GEO_BTN[0]->n = 2;
  x = x + w2 + 5;
  
  y = y0 + 5;
  obj = add_normal_button(x, y, w3, h1+h2, "INIT\nLeCroy", 'N');
    fl_set_object_callback(obj, init_callback, 0);
}

//~----------------------------------------------------------------------------

void init_callback(FL_OBJECT *obj, long data) {

  if (obj) {
    msg = -1;
    update_objects();
  }

  init_LeCroy_bus(bus, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_module_address_panel(void) {

  bus = w_bus.inp;
  geo = w_geo.inp;
}

//~============================================================================
//
//            ALARM PANEL
//            
//~============================================================================

void create_alarm_panel(int x0, int y0) {

  int w1 = 85, h1 = 30;
  int x, y, w, h;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  create_dat_box_vstring(
    1, x, y, w1, h1, 'H', &r_sta, &sta_msk[0], &STA_BOX[0]);
}

//~----------------------------------------------------------------------------

void update_alarm_panel(void) {

  show_dat_box_string(1, o_a, NULL,  &STA_BOX[0]);
}

//~============================================================================
//
//            FIND BRICK PANEL
//            
//~============================================================================

void create_find_brick_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 15, w3 = 45, h1 = 15, h2 = 15;
  _b msk[2] = {
    {0x0, 0x07,  8},
    {0x1, 0x1F, 32}};
  
  w = 5 + w1 + 8 * w2 + w3 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y,    w1, h1, "Bus", 'S');
  add_frame_box(x, y+h1, w1, h1, "GEO", 'S');
  x = x + w1;

  for (i=0; i<8; i++) {
    create_dat_box_vstring(
      1, x, y,    w2, h1, 'N', &r_adr[i], &msk[0], &BUS_BOX[i]);
    create_dat_box_vstring(
      1, x, y+h1, w2, h1, 'N', &r_adr[i], &msk[1], &GEO_BOX[i]);
    x = x + w2;
  }

  obj = add_normal_button(x, y, w3, h1+h2, "FIND\nBrick", 'N');
    fl_set_object_callback(obj, find_brick_callback, 0);
}

//~----------------------------------------------------------------------------

void find_brick_callback(FL_OBJECT *obj, long data) {

  int i, j, k;
  bool debug = FALSE;
  int16 cod;
  int save_bus, save_geo;
  
  save_bus = bus;
  save_geo = geo;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<8; i++) {
    _r *r = &r_adr[i];
    for (j=0; j<2; j++) {
      r->was[j] = r->rea[j];
    }
    r->unk = TRUE;
    r->sta = status_KO;
  }
  
  k = 0;
  
  for (bus=0; bus<8; bus++) {
    for (geo=0; geo<16; geo++) {
      brick_channel_read(1, &cod, &chk, &err);
      if (err) {
        if (debug) printf("bus = %d, geo = %2d, err = 0x%04X\n", bus, geo, err);
        if (!go_on(err)) break;
      }
      else {
        if (debug) printf("bus = %d, geo = %2d, chk = %d ", bus, geo, chk);
        if (!chk) {
          if (debug) printf("*******************************");
          if (k < 8) {
            _r *r = &r_adr[k++];
            r->rea[0] = bus;
            r->rea[1] = geo;
            if (r->unk) for (i=0; i<2; i++) r->was[i] = r->rea[i];
            r->unk = FALSE;
            r->sta = status_OK;
          }
        }
        if (debug) printf("\n");
      }
    }
    if (!go_on(err)) break;
  }

  chk = 0;
  if (obj) {
    msg = 1;
    update_objects();
  }

  bus = save_bus;
  geo = save_geo;
}

//~----------------------------------------------------------------------------

void update_find_brick_panel(void) {

  show_dat_box_string(8, hex, NULL,  &BUS_BOX[0]);
  show_dat_box_string(8, hex, NULL,  &GEO_BOX[0]);
}

//~============================================================================
//
//            HV CHANNELS PANEL
//
//~============================================================================

_b cha_msk = {0x00, 0x0FFF, 1024};

_b dcdc_msk = {0, 0x01, 2};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_channels_panel(int x0, int y0, int ind) {

  FL_OBJECT *obj;
  int i, j;
  int x, y;

{//~--- Check if this Panel is needed at all ---

  if (ilay == TOF && ind == 0) return;
}

{//~--- Panel ---

int w1 = 270, h1 = ind ? 475 : ilay == RICH ? 305 : 345;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");
  x = x0 + 5;
  y = y0 + 5;
}

{//~--- LR ---

int w1 = 45, w2 = 35, w3 = 45, w4 = 20, w5 = 50;
int h1 = 20, h2 = 15;

  add_frame_box(x, y,    w1,    h1+h2, "LR\nGroup",'N');
  x = x + w1;
  add_frame_box(x, y,    w2,    h1+h2, "Chan",     'N');
  x = x + w2;
  add_frame_box(x, y,    w3+w4, h1,    "Write",    'N');
  add_frame_box(x, y+h1, w3,    h2,    "Volts",    'S');
  x = x + w3;
  add_frame_box(x, y+h1, w4,    h2,    "",         'N');
  x = x + w4;
  add_frame_box(x, y,    w3+w4, h1,    "Read",     'N');
  add_frame_box(x, y+h1, w3,    h2,    "Volts",    'S');
  x = x + w3;
  add_frame_box(x, y+h1, w4,    h2,    "",         'N');
  x = x + w4;
  add_frame_box(x, y,    w5,    h1+h2, "Reset",    'N');
  y = y + h1 + h2;
  
  for (i=0; i<(ind ? 3 : 2); i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, 8*h2, hex[i + (ilay == TOF ? 0 : 2 * ind + 1)], 'N');
    create_dat_box_vstring(
      1, x, y+7*h2, w1-10, h2, 'N', &r_sta, &sta_msk[i+1], &STA_BOX[i + 1 +(ilay == TOF ? 0 : 2 * ind)]);
    x = x + w1;
    for (j=0; j<8; j++) {
      int k = 8 * i + j + (ilay == TOF ? 0 : 16 * ind);
      char b[80];
      
      x = x0 + 5 + w1;
      sprintf(b, "LR %2d", k);
      add_frame_box(x, y, w2, h2, b, 'S');
      x = x + w2;
      create_btn_hstring(
        1, x, y, w3, h2, NULL, 'N', &ww_cha[k], &cha_msk, &CHA_BTN[k]);
      CHA_BTN[k]->cnv = convert_LR_channel_code_to_value;
      obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w3/3, h2, "128");
        fl_set_object_callback(obj, set_channel_voltage_callback, 100*ind+k);
      x = x + w3 / 3;
      obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w3/3, h2, "16");
        fl_set_object_callback(obj, set_channel_voltage_callback, 100*ind+k);
      x = x + w3 / 3;
      obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w3/3, h2, "1");
        fl_set_object_callback(obj, set_channel_voltage_callback, 100*ind+k);
      x = x + w3 / 3;
      obj = add_normal_button(x, y, w4, h2, "W",     'N');
        fl_set_object_callback(obj, channel_write_callback, k);
      x = x + w4;
      create_dat_box_vstring(
        1, x, y, w3, h2,       'N',  &r_cha[k], &cha_msk, &CHA_BOX[k]);
      CHA_BOX[k]->cnv = convert_LR_channel_code_to_value;
      x = x + w3;
      obj = add_normal_button(x, y, w4, h2, "R",     'N');
        fl_set_object_callback(obj, channel_read_callback, k);
      x = x + w4;
      obj = add_normal_button(x, y, w5, h2, "RESET", 'N');
        fl_set_object_callback(obj, channel_reset_callback, k);
      y = y + h2;
    }
    y = y + 5;
  }
  
  x = x0 + 5;
  y = y - 5;
  obj = add_frame_box(x, y, w1+w2, h2, "All", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  x = x + w1 + w2;
  obj = add_normal_button(x, y, w3+w4, h2, "WRITE", 'N');
    fl_set_object_callback(obj, all_LR_channel_write_callback, ind);
  x = x + w3 + w4;
  obj = add_normal_button(x, y, w3+w4, h2, "READ",  'N');
    fl_set_object_callback(obj, all_LR_channel_read_callback, ind);
  x = x + w3 + w4;
  obj = add_normal_button(x, y, w5,    h2, "RESET", 'N');
    fl_set_object_callback(obj, all_LR_channel_reset_callback, ind);
  y = y + h2 + 5;
}

{//~--- DCDC ---

int w1 = 80, w2 = 35, w3 = 45, w4 = 20, w5 = 50, w6 = 80, w7 = 80;
int h1 = 40, h2 = 15, h3 = 20;
char *txt[2] = {"DCDC-1", "DCDC-0"};
int dcdc = 41 - ind;

  if (ilay == RICH && ind == 0) return;
  
  x = x0 + 5;
  if (ind == 0) y = y - 5;
  obj = add_frame_box(x, y, w1, h1, txt[ind], 'L');
    fl_set_object_lalign(obj, FL_ALIGN_TOP | FL_ALIGN_INSIDE);
  create_dat_box_vstring(
    1, x, y+h1-h2, w2, h2, 'N', &r_sta, &sta_msk[7-ind], &STA_BOX[7-ind]);
  obj = add_normal_button(x+w2, y+h1-h2, w4, h2, "R", 'N');
    fl_set_object_callback(obj, status_read_callback, 0);

  x = x0 + 5 + w1;
  create_btn_hstring(
    1, x, y, w3, h3, NULL, 'N', &ww_cha[dcdc], &cha_msk, &CHA_BTN[dcdc]);
  CHA_BTN[dcdc]->cnv = convert_DCDC_channel_code_to_value;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w3/3, h3, "128");
    fl_set_object_callback(obj, set_channel_voltage_callback, dcdc);
  x = x + w3 / 3;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w3/3, h3, "16");
    fl_set_object_callback(obj, set_channel_voltage_callback, dcdc);
  x = x + w3 / 3;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w3/3, h3, "1");
    fl_set_object_callback(obj, set_channel_voltage_callback, dcdc);
  x = x + w3 / 3;
  obj = add_normal_button(x, y, w4, h3, "W",       'N');
    fl_set_object_callback(obj, channel_write_callback, dcdc);
  x = x + w4;
  create_dat_box_vstring(
    1, x, y, w3, h3,       'N',  &r_cha[dcdc], &cha_msk, &CHA_BOX[dcdc]);
  CHA_BOX[dcdc]->cnv = convert_DCDC_channel_code_to_value;
  x = x + w3;
  obj = add_normal_button(x, y, w4, h3, "R",       'N');
    fl_set_object_callback(obj, channel_read_callback, dcdc);
  x = x + w4;
  obj = add_normal_button(x, y, w5, h3, "RESET", 'N');
    fl_set_object_callback(obj, channel_reset_callback, dcdc);
  y = y + h3;

  x = x0 + 5 + w1;
  create_btn_hstring(
    1, x, y, w6, h3, o_o, 'M', &ww_dcdc[1-ind], &dcdc_msk, &DCDC_BTN[1-ind]);
  x = x + w6;
  obj = add_normal_button(x, y, w4, h3, "W", 'N');
    fl_set_object_callback(obj, set_DCDC_callback, 1-ind);
  x = x + w4;
  add_frame_box(x, y, w7, h3, "", 'H');
  y = y + h3 + 5;
}

}

//~----------------------------------------------------------------------------

void convert_LR_channel_code_to_value(int32 code, int32 *value) {

//                 ECAL     RICH   TOF
  float Q[] = {    0.0,     3.0,   5.0  };
  float K[] = {887.0/1024., 0.950, 2.437};

  *value = Q[ilay] + K[ilay] * ((float)code + 1.0) + 0.5;
}

//~----------------------------------------------------------------------------

void convert_DCDC_channel_code_to_value(int32 code, int32 *value) {

//                 ECAL     RICH   TOF
  float Q[] = {    0.0,     3.0,   5.0  };
  float K[] = {910.0/1024., 0.975, 2.437};

  *value = Q[ilay] + K[ilay] * ((float)code + 1.0) + 0.5;
}

//~----------------------------------------------------------------------------

void set_channel_voltage_callback(FL_OBJECT *obj, long data) {

  int ind = data % 100;
  int jnd = data / 100;
  _ww *w = &ww_cha[ind];
  int  d =  atoi(fl_get_object_label(obj));
  
  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  
  pack32(&w->inp[0], 0x3FF, MOD(w->inp[0]+d, 1024));
  w->sta = status_NA;

  if (follow[ind] || follow_all[jnd]) {
    msg = -1;
    update_objects();
    channel_write_callback(NULL, ind);
    msg = 1;
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void channel_write_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _r  *r =  &r_cha[ind];
  _ww *w = &ww_cha[ind];
  
  if (obj) {
    msg = 0;
    if (follow[ind]) {
      set_normal_button(obj, 0);
      follow[ind] = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        follow[ind] = TRUE;
        update_objects();
        return;
      }
    }
    msg = -1;
    update_objects();
  }

  r-> sta = status_NA;
  
  brick_channel_write(ind, w->inp[0], &chk, &err);

  if (!err && !chk) {
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

void channel_read_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 dat;
  _r  *r =  &r_cha[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  
  brick_channel_read(ind, &dat, &chk, &err);

  if (!err && !chk) {
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

void channel_reset_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _r  *r =  &r_cha[ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r-> sta = status_NA;

  brick_channel_reset(ind, &chk, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void all_LR_channel_write_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  
  if (ilay == TOF && ind == 0) return;

  if (obj) {
    msg = 0;
    if (follow_all[ind]) {
      set_normal_button(obj, 0);
      follow_all[ind] = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        follow_all[ind] = TRUE;
        update_objects();
        return;
      }
    }
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=(ind ? 16 : 0); i<(ind ? 40 : 16); i++) {
    int ii = i - (ilay == TOF ? 16 : 0);
    if (go_on(err)) channel_write_callback(NULL, ii);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void all_LR_channel_read_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  
  if (ilay == TOF && ind == 0) return;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=(ind ? 16 : 0); i<(ind ? 40 : 16); i++) {
    int ii = i - (ilay == TOF ? 16 : 0);
    if (go_on(err)) channel_read_callback(NULL, ii);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void all_LR_channel_reset_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  
  if (ilay == TOF && ind == 0) return;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=(ind ? 16 : 0); i<(ind ? 40 : 16); i++) {
    int ii = i - (ilay == TOF ? 16 : 0);
    if (go_on(err)) channel_reset_callback(NULL, ii);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void set_DCDC_callback(FL_OBJECT *obj, long data) {

  int ind  = data;
  int8 stat;
  _ww *w = &ww_dcdc[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  brick_set_DCDC(ind, w->inp[0], &stat, &chk, &err);
  
  if (!err && !chk) {
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

  update_objects();
}

//~----------------------------------------------------------------------------

void status_read_callback(FL_OBJECT *obj, long data) {

  int8 stat;
  _r *r = &r_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  
  brick_status_read(&stat, &chk, &err);
  
  if (!err && !chk) {
    r->rea[0] = stat;
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

void update_channels_panel(void) {

  int i;
  
  for (i=1; i<(ilay == TOF ? 4 : 6); i++) {
    show_dat_box_string(1, t_o, NULL, &STA_BOX[i]);
  }
  
  for (i=0; i<(ilay == TOF ? 24 : 40); i++) {
    show_btn_string(    1,       "%4d", &CHA_BTN[i]);
    show_dat_box_string(1, NULL, "%4d", &CHA_BOX[i]);
  }

  for (i=0; i<(ilay == ECAL ? 2 : 1); i++) {
    show_btn_string(    1,       "%4d", &CHA_BTN[40+i]);
    show_dat_box_string(1, NULL, "%4d", &CHA_BOX[40+i]);
    if (ilay == TOF) {
      FL_OBJECT *obj;
      obj = CHA_BTN[40+i]->obj;
      if (atoi(fl_get_object_label(obj)) >= 2350) {
        fl_set_object_color(obj, FL_RED, FL_RED);
        fl_set_object_lcolor(obj, FL_YELLOW);
      }
      obj = CHA_BOX[40+i]->obj;
      if (atoi(fl_get_object_label(obj)) >= 2350) {
        fl_set_object_color(obj, FL_RED, FL_RED);
        fl_set_object_lcolor(obj, FL_YELLOW);
      }
    }
    show_btn_string(    1,        NULL, &DCDC_BTN[i]);
    show_dat_box_string(1, o_t,   NULL, &STA_BOX[6+i]);
  }
}

//~============================================================================
//
//             ERROR READOUT PANEL
//
//~============================================================================

_b err_msk[8] = {
  {0, 0x01, 2},
  {0, 0x02, 2},
  {0, 0x04, 2},
  {0, 0x08, 2},
  {0, 0x10, 2},
  {0, 0x20, 2},
  {0, 0x40, 2},
  {0, 0x80, 2}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_error_readout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 20, h1 = 15, h2 = 15;
    
  w = 5 + w1 + 8 * w1 + w2 + 5;
  h = 5 + h1 + 6 * h2 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Error Readout", 'S');
  y = y + h1;

  for (i=0; i<6; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, h2, hex[i], 'S');
    x = x + w1;
    create_dat_box_hstring(8, x, y, w1, h2, 'N', &r_err[i], err_msk, &ERR_BOX[i][0]);
    x = x + 8 * w1;
    obj = add_normal_button(x, y, w2, h2, "R", 'N');
      fl_set_object_callback(obj, error_readout_callback, i);
    y = y + h2;
  }

  x = x0 + 5;
  obj = add_normal_button(x, y, w-10, h2, "READ", 'N');
    fl_set_object_callback(obj, all_error_readout_callback, 0);
}

//~----------------------------------------------------------------------------

void error_readout_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 cod;
  _r *r = &r_err[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];

  brick_channel_read(0x2A+ind, &cod, &chk, &err);
  
  if (!err && !chk) {
    r->rea[0] = cod;
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

void all_error_readout_callback(FL_OBJECT *obj, long data) {

  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=0; i<6; i++) {
//  if (i == 3 || i == 4) continue;
    if (go_on(err)) error_readout_callback(NULL, i);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_error_readout_panel(void) {

  int i;
  
  for (i=0; i<6; i++) {
    show_dat_box_string(8, hex, NULL, &ERR_BOX[i][0]);
  }
}

//~============================================================================
//
//            CONFIGURATION FILE PANEL
//
//~============================================================================

void create_config_file_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 95, w2 = 50, w3 = 45, h1 = 15, h2 = 20;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Config File", 'S');
  y = y + h1;

  obj = add_normal_button(x, y, w2, h2, "WRITE", 'N');
    fl_set_object_callback(obj, write_config_file_callback, 0);
  x = x + w1 / 2;
  obj = add_normal_button(x, y, w3, h2, "READ", 'N');
    fl_set_object_callback(obj, read_config_file_callback, 0);
}

//~----------------------------------------------------------------------------

void write_config_file_callback(FL_OBJECT* obj, long data) {

  FILE *file;
  int i;

  file = fopen("Veronica.conf", "w");
  if (file != NULL) {
    fwrite(&w_bus.inp, 4, 1, file);
    fwrite(&w_geo.inp, 4, 1, file);
    for (i=0; i<42; i++) fwrite(ww_cha[i].inp, 4, 1, file);
    fclose(file);
  }
}

//~----------------------------------------------------------------------------

void read_config_file_callback(FL_OBJECT* obj, long data) {

  msg = -2;
  update_objects();
}

//~============================================================================
//
//            READ, WRITE, UPDATE PANEL
//
//~============================================================================

void create_read_write_update_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 95, h1 = 20, h2 = 35;
  
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

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  if (go_on(err)) all_LR_channel_read_callback(NULL, 0);
  if (go_on(err)) all_LR_channel_read_callback(NULL, 1);
  if (go_on(err)) status_read_callback(NULL, 0);
  if (go_on(err)) all_error_readout_callback(NULL, 0);
  if (go_on(err)) channel_read_callback(NULL, 40);
  if (go_on(err)) if (ilay == ECAL) channel_read_callback(NULL, 41);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void write_all_callback(FL_OBJECT* obj, long data) {

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  if (go_on(err)) set_DCDC_callback(NULL, 0);
  if (go_on(err)) if (ilay == ECAL) set_DCDC_callback(NULL, 1);
  if (go_on(err)) channel_write_callback(NULL, 40);
  if (go_on(err)) if (ilay == ECAL) channel_write_callback(NULL, 41);
  if (go_on(err)) all_LR_channel_write_callback(NULL, 0);
  if (go_on(err)) all_LR_channel_write_callback(NULL, 1);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_input_callback(FL_OBJECT* obj, long data) {

  int i;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  read_all_callback(NULL, 0);

  if (!err) {
    for (i=0; i<2; i++) {
      _r  *r =  &r_sta;
      _ww *w = &ww_dcdc[i];
      if (r->rea[0] & sta_msk[6+i].msk) w->inp[0] = 0x00000000;
      else                              w->inp[0] = 0x00000001;
    }

    for (i=0; i<42; i++) {
      _r  *r =  &r_cha[i];
      _ww *w = &ww_cha[i];
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

  update_module_address_panel();
  update_alarm_panel();
  update_find_brick_panel();
  update_channels_panel();
  update_error_readout_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
