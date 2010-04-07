// file Brick.c
//
// HV Brick Low level Controller
//
// A.Lebedev Nov-2004...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"HV Brick Pro Controller"};            // global variable
char *date  = {"22-Dec-07"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"
int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~----------------------------------------------------------------------------

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~--

bool rea_nn = TRUE;

int16 pwr_0, pwr_1, geo;
int rw, func, reg_adr;
FL_OBJECT *R_W, *FUNC, *REG[4];
FL_OBJECT *CODE_R, *VALUE_R, *CODE_W, *VALUE_W;

FL_OBJECT *BUS, *INP[32], *WRI[32], *REA[32];
int8       bus;
int32            inp,      wri,      rea;

FL_OBJECT *GRP[2][4];

struct {
  char *nam;
  int16 cod;
  int _0[2];
  int _1[2];
} functions[6] = {
//~--------------------func---1-32----33-64--
//~--------------------code---R--W----R--W---
  {"Channel\nReset", 0x0001, {1, 1}, {1, 1}},
  {"Status\nRead",   0x0002, {2, 2}, {2, 2}},
  {"Shut\nDown",     0x0004, {2, 2}, {2, 2}},
  {"Write\nRead",    0x0005, {1, 3}, {3, 3}},
  {"Set\nDCDC1",     0x0006, {2, 2}, {2, 2}},
  {"Set\nDCDC0",     0x0007, {2, 2}, {2, 2}}};
  
struct {
  char *nam;
} registers[48] = {

  {"LR_0"},    {"LR_1"},    {"LR_2"},    {"LR_3"},    {"LR_4"},  
  {"LR_5"},    {"LR_6"},    {"LR_7"},    {"LR_8"},    {"LR_9"},
  {"LR_10"},   {"LR_11"},   {"LR_12"},   {"LR_13"},   {"LR_14"}, 
  {"LR_15"},   {"LR_16"},   {"LR_17"},   {"LR_18"},   {"LR_19"},
  {"LR_20"},   {"LR_21"},   {"LR_22"},   {"LR_23"},   {"LR_24"}, 
  {"LR_25"},   {"LR_26"},   {"LR_27"},   {"LR_28"},   {"LR_29"},
  {"LR_30"},   {"LR_31"},   {"LR_32"},   {"LR_33"},   {"LR_34"}, 
  {"LR_35"},   {"LR_36"},   {"LR_37"},   {"LR_38"},   {"LR_39"},
  {"DC/DC 0"}, {"DC/DC 1"}, {"Error 0"}, {"Error 1"}, {"Error 2"},
  {"Error 3"}, {"Error 4"}, {"Error 5"}};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_bus_panel(int x0, int y0);
void bus_callback(FL_OBJECT *obj, long data);
void INIT_callback(FL_OBJECT *obj, long data);
void update_bus_panel(void);

void create_dat_panel(int x0, int y0);
void R_W_callback(FL_OBJECT *obj, long data);
void func_callback(FL_OBJECT *obj, long data);
void reg_adr_callback(FL_OBJECT *obj, long data);
void inp_callback(FL_OBJECT *obj, long data);
void SEND_callback(FL_OBJECT *obj, long data);
void update_dat_panel(void);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 750, HH = 275;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;
  
  create_bus_panel(140,  0);
  create_dat_panel(  5, 45);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  bus     =  4;
  func    = -1;
  reg_adr = -1;
  inp     =  0x80000000;
  wri     =  0;
  rea     =  0x00000000;
  
  read_dat_config_file(dat_config_file_name);
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

//~-------------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&bus,     sizeof(bus),     1, file);
    fread(&func,    sizeof(func),    1, file);
    fread(&reg_adr, sizeof(reg_adr), 1, file);
    fread(&inp,     sizeof(inp),     1, file);
    fread(&wri,     sizeof(wri),     1, file);
    fread(&rea,     sizeof(rea),     1, file);
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
    fwrite(&bus,     sizeof(bus),     1, file);
    fwrite(&func,    sizeof(func),    1, file);
    fwrite(&reg_adr, sizeof(reg_adr), 1, file);
    fwrite(&inp,     sizeof(inp),     1, file);
    fwrite(&wri,     sizeof(wri),     1, file);
    fwrite(&rea,     sizeof(rea),     1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~===============================================================================

void create_bus_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w, h, w1 = 35, w2 = 40, h1 = 15, h2 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Bus", FL_SMALL_SIZE);

  x = x + w1;
  obj = add_normal_button(x, y, w2, h1+h2, "INIT", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, INIT_callback, 0);

  x = x0 + 5;
  y = y + h1;
  BUS =
  obj = add_switch_button(x, y, w1, h2, " ", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, bus_callback, 0);
}

//~----------------------------------------------------------------------------

void bus_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  bus = MOD(bus+1, 8);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) bus = MOD(bus-1, 8);

  update_objects();
}

//~----------------------------------------------------------------------------

void INIT_callback(FL_OBJECT *obj, long data) {

  int16 err;

  msg = -1;
  update_objects();

  chk = 0;
  init_LeCroy_bus(bus, &err);
  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_bus_panel(void) {

  char b[80];

  sprintf(b, "%d", bus);
  fl_set_object_label(BUS, b);
}

//~===============================================================================

void create_dat_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, x1, size;
  int w, h, w1 = 20, h1 = 20, w2 = 30, h2 = 190;
  
//~--

  w = 30 + 4 * (4 * w1 + 5) + 5 + 4 * (4 * w1 + 5) + w2 + 5;
  h = 5 + 3 * h1 + 2 * h1 + 15 + 2 * h1 + h1 + 20;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

//~--

  size = FL_NORMAL_SIZE;
  x = x0 + 30;
  y = y0 + 5;
  
  obj = add_frame_box(x, y, w1, 3*h1, "1", size);
  x = x + w1;
  obj = add_frame_box(x, y, w1, 3*h1, "P", size);
  x = x + w1;
  obj = add_frame_box(x, y,  2*w1, h1,   "PWR", size);
  obj = add_frame_box(x, y+h1, w1, 2*h1, "#\n1", size);
  x = x + w1;
  obj = add_frame_box(x, y+h1, w1, 2*h1, "#\n0", size);

  x = x + w1 + 5;
  w = 4 * w1 + 5 + w1;
  obj = add_frame_box(x, y, w, 3*h1, "GEO Address", size);
  x = x + w;
  w = 3 * w1;
  obj = add_frame_box(x, y, w, 3*h1, "not used", size);

  x = x + w + 5;
  R_W =
  obj = add_switch_button( x, y, w1, h1,   "?", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, R_W_callback, 0);
  obj = add_frame_box(x, y+h1, w1, 2*h1, "R\nW", size);

  x = x + w1;
  w = 3 * w1;
  FUNC =
  obj = add_switch_button(x,      y, w, 2*h1, "Channel\nReset", size);
    fl_set_object_callback(obj, func_callback, 0);
  obj = add_frame_box(    x, y+2*h1, w, h1,   "Function", size);
  
  x1 = x + w + 10;
  
  GRP[0][0] = 
  obj = fl_bgn_group();
    x = x1;
    w = 4 * (4 * w1 + 5) - 5;
    h = 3 * h1;
    obj = add_frame_box(x, y, w, h, "?", size);
  fl_end_group();
  fl_hide_object(obj);
  
  GRP[0][1] = 
  obj = fl_bgn_group();
    x = x1;
    w = 4 * w1 + 5 + 2 * w1;
    h = 3 * h1 / 2;
    REG[0] =
//  obj = add_data_box( x, y,   w, h, "DC/DC #0", FL_MEDIUM_SIZE);
    obj = add_switch_button( x, y,   w, h, "DC/DC #0", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, reg_adr_callback, 0);
    obj = add_frame_box(x, y+h, w, h, "Register Address", size);
    x = x + w;
    w = 2 * w1 + 5 + 4 * w1 + 5 + 4 * w1;
    h = 3 * h1;
    obj = add_frame_box(x, y, w, h, "not used", size);
  fl_end_group();
  fl_hide_object(obj);
  
  GRP[0][2] = 
  obj = fl_bgn_group();
    x = x1;
    w = 4 * (4 * w1 + 5) - 5;
    h = 3 * h1;
    obj = add_frame_box(x, y, w, h, "not used", size);
  fl_end_group();
  fl_hide_object(obj);
  
  GRP[0][3] = 
  obj = fl_bgn_group();
    x = x1;
    w = 4 * w1 + 5 + 2 * w1;
    h = 3 * h1 / 2;
    REG[1] =
//  obj = add_data_box( x, y,   w, h, "DC/DC #0", FL_MEDIUM_SIZE);
    obj = add_switch_button( x, y,   w, h, "DC/DC #0", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, reg_adr_callback, 0);
    obj = add_frame_box(x, y+h, w, h, "Register Address", size);
    x = x + w;
    w = 2 * w1 + 5 + 4 * w1 + 5 + 4 * w1;
    h = 3 * h1;
    obj = add_frame_box(x, y, w, h, "Data", size);
    CODE_W =
    obj = add_data_box(x+5, y+5, 2*w1, h1, "1234", FL_MEDIUM_SIZE);
    VALUE_W =
    obj = add_data_box(x+w-3*w1-5, y+5, 3*w1, h1, "1234", FL_MEDIUM_SIZE);
  fl_end_group();
  fl_hide_object(obj);
  
  x = x + w + 5;
  obj = add_normal_button(x, y, w2, h2, "W\nR\nI\nT\nE", FL_LARGE_SIZE);
    fl_set_object_callback(obj, SEND_callback, 0);

//~--

  x = x0 + 5;
  y = y0 + 5 + 3 * h1;
  add_plain_text(x, y,    25, h1, "@->", FL_TINY_SIZE);
  add_plain_text(x, y+h1, 25, h1, "TX", FL_NORMAL_SIZE);
  
  x = x0 + 30;
  
  for (i=0; i<32; i++) {
    char b[80];
    sprintf(b, "%d", i+1);
    INP[i] =
    obj = add_switch_button(x, y,      w1, h1, "?", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, inp_callback, i);
    WRI[i] =
    obj = add_data_box(     x, y+h1,   w1, h1, "?", FL_MEDIUM_SIZE);
    add_plain_text(         x, y+2*h1, w1, 15, b,   FL_SMALL_SIZE);
    x = x + w1;
    if (!((i + 1) %  4)) x = x + 5;
    if (!((i + 1) % 16)) x = x + 5;
  }

//~--

  size = FL_NORMAL_SIZE;
  x = x0 + 30;
  y = y0 + 5 + 5 * h1 + 15;
  
  obj = add_frame_box(x, y, w1, 2*h1, "1", size);
  x = x + w1;
  obj = add_frame_box(x, y, w1, 2*h1, "P", size);
  x = x + w1;
  obj = add_frame_box(x, y, w1, 2*h1, "#\n1", size);
  x = x + w1;
  obj = add_frame_box(x, y, w1, 2*h1, "#\n0", size);

  x = x + w1 + 5;
  w = 4 * w1 + 5 + w1;
  obj = add_frame_box(x, y, w,  2*h1, "GEO Address", size);
  x = x + w;
  w = 3 * w1;
  obj = add_frame_box(x, y, w,  2*h1, "not used", size);

  x = x + w + 5;
  obj = add_frame_box(x, y, w1, 2*h1, "R\nW", size);

  x = x + w1;
  w = 3 * w1;
  obj = add_frame_box(x, y, w,  2*h1,   "Function", size);

  x1 = x + w + 10;
  
  GRP[1][0] = 
  obj = fl_bgn_group();
    x = x1;
    w = 4 * (4 * w1 + 5) - 5;
    h = 2 * h1;
    obj = add_frame_box(x, y, w, h, "?", size);
  fl_end_group();
  fl_hide_object(obj);
  
  GRP[1][1] = 
  obj = fl_bgn_group();
    x = x1;
    w = 4 * w1 + 5 + 2 * w1;
    h = h1;
    REG[2] =
    obj = add_data_box( x, y,   w, h, "DC/DC #0", FL_MEDIUM_SIZE);
    obj = add_frame_box(x, y+h, w, h, "Register Address", size);
    x = x + w;
    w = 2 * w1 + 5 + 4 * w1 + 5 + 4 * w1;
    h = 2 * h1;
    obj = add_frame_box(x, y, w, h, "not used", size);
  fl_end_group();
  fl_hide_object(obj);
  
  GRP[1][2] = 
  obj = fl_bgn_group();
    x = x1;
    obj = add_frame_box(x, y, w1,2*h1, "A", size);
    x = x + w1;
    w = 2 * w1 + 5 + 3 * w1;
    obj = add_frame_box(x, y, w,  h1,  "LR Group", size);
    x = x + w;
    w = 2 * w1;
    obj = add_frame_box(x, y, w,  h1,  "DCDC", size);
    x = x + w + 5;
    w = 4 * w1 + 5 + 4 * w1;
    h = 2 * h1;
    obj = add_frame_box(x, y, w, h, "not used", size);
    x = x1 + w1;
    obj = add_frame_box(x, y+h1, w1, h1, "1", size);
    x = x + w1;
    obj = add_frame_box(x, y+h1, w1, h1, "2", size);
    x = x + w1;
    obj = add_frame_box(x, y+h1, w1, h1, "3", size);
    x = x + w1 + 5;
    obj = add_frame_box(x, y+h1, w1, h1, "4", size);
    x = x + w1;
    obj = add_frame_box(x, y+h1, w1, h1, "5", size);
    x = x + w1;
    obj = add_frame_box(x, y+h1, w1, h1, "0", size);
    x = x + w1;
    obj = add_frame_box(x, y+h1, w1, h1, "1", size);
  fl_end_group();
  fl_hide_object(obj);
  
  GRP[1][3] = 
  obj = fl_bgn_group();
    x = x1;
    w = 4 * w1 + 5 + 2 * w1;
    h = h1;
    REG[3] =
    obj = add_data_box( x, y,   w, h, "DC/DC #0", FL_MEDIUM_SIZE);
    obj = add_frame_box(x, y+h, w, h, "Register Address", size);
    x = x + w;
    w = 2 * w1 + 5 + 4 * w1 + 5 + 4 * w1;
    h = 2 * h1;
    obj = add_frame_box(x, y, w, h, "Data", size);
    CODE_R =
    obj = add_data_box(x+5, y+5, 2*w1, h1, "1234", FL_MEDIUM_SIZE);
    VALUE_R =
    obj = add_data_box(x+w-3*w1-5, y+5, 3*w1, h1, "1234", FL_MEDIUM_SIZE);
  fl_end_group();
  fl_hide_object(obj);

//~--

  x = x0 + 5;
  y = y0 + 5 + 5 * h1 + 15 + 2 * h1;
  add_plain_text(x, y, 25, h1, "RX", FL_NORMAL_SIZE);
  
  x = x0 + 30;
  
  for (i=0; i<32; i++) {
    char b[80];
    sprintf(b, "%d", i+33);
    REA[i] =
    obj = add_data_box(x, y,    w1, h1, "?", FL_MEDIUM_SIZE);
    add_plain_text(    x, y+h1, w1, 15, b, FL_SMALL_SIZE);
    x = x + w1;
    if (!((i + 1) %  4)) x = x + 5;
    if (!((i + 1) % 16)) x = x + 5;
  }
}

//~----------------------------------------------------------------------------

void R_W_callback(FL_OBJECT *obj, long data) {

  int p;
  int32 m;
  
  inp ^= 0x00080000;

  for (m=0x80000000,p=1; m; m>>=1) if (inp & m) p ^= 1;
  if (p) inp ^= 0x40000000;

  update_objects();
}

//~----------------------------------------------------------------------------

void func_callback(FL_OBJECT *obj, long data) {

  int i, p;
  int32 m;
  
  func = -1;
  
  for (i=0; i<6; i++) {
    int16 k = swapbits(unpack32(inp, 0x00070000), 3);
    if (functions[i].cod == k) func = i;
  }

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  func = MOD(func+1, 6);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) func = MOD(func-1, 6);
  
  pack32(&inp, 0x00070000, swapbits((int32)functions[func].cod, 3));

  for (m=0x80000000,p=1; m; m>>=1) if (inp & m) p ^= 1;
  if (p) inp ^= 0x40000000;

  update_objects();
}

//~----------------------------------------------------------------------------

void reg_adr_callback(FL_OBJECT *obj, long data) {

  int p;
  int32 m;
  int16 reg;  

  reg_adr = -1;
  reg = swapbits(unpack32(inp, 0x0000FC00), 6);
  if (reg < 0x30) reg_adr = reg;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  reg_adr = MOD(reg_adr+1, 0x30);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) reg_adr = MOD(reg_adr-1, 0x30);
  
  pack32(&inp, 0x0000FC00, swapbits((int32)reg_adr, 6));

  for (m=0x80000000,p=1; m; m>>=1) if (inp & m) p ^= 1;
  if (p) inp ^= 0x40000000;

  update_objects();
}

//~----------------------------------------------------------------------------

void inp_callback(FL_OBJECT *obj, long data) {

  int bit = data;
  int32 m;
  int i, p;
  
  inp ^= 0x80000000 >> bit;

  if (0x80000000 >> bit != 0x40000000) {
    for (m=0x80000000,p=1; m; m>>=1) if (inp & m) p ^= 1;
    if (p) inp ^= 0x40000000;
  }
  
  func = -1;

  for (i=0; i<6; i++) {
    int16 k = swapbits(unpack32(inp, 0x00070000), 3);
    if (functions[i].cod == k) func = i;
  }

  update_objects();
}

//~----------------------------------------------------------------------------

void SEND_callback(FL_OBJECT *obj, long data) {

  int16 err;

  msg = -1;
  update_objects();

  wri = inp;

  chk = 1;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);

  if (err) rea_nn = TRUE;
  else     rea_nn = FALSE;
  
  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_dat_panel(void) {

  FL_OBJECT *obj;
  int i, p;
  int32 m;
  char b[80];
  static bool first = TRUE, old_rw, old_func;
  
//~--

  obj = R_W;
  rw = unpack32(inp, 0x00080000);
  if (rw) fl_set_object_label(obj, "W");
  else    fl_set_object_label(obj, "R");
  fl_set_object_color(obj, FL_WHEAT, 0);

  obj = FUNC;
  if (func != -1) {
    fl_set_object_label(obj, functions[func].nam);
    fl_set_object_color(obj, FL_WHEAT, FL_RED);
  }
  else {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_RED, FL_RED);
  }
  
//~--

  for (i=0; i<4; i++) {
    int reg = swapbits(unpack32(inp, 0x0000FC00), 6);
    obj = REG[i];
    if (reg < 0x30) {
      fl_set_object_label(obj, registers[reg].nam);
      fl_set_object_color(obj, FL_WHEAT, FL_RED);
    }
    else {
      fl_set_object_label(obj, " ");
      fl_set_object_color(obj, FL_RED, FL_BLACK);
    }
  }
  
//~--

  obj = CODE_R;
  if (rea_nn) {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_YELLOW, 0);
  }
  else {
    sprintf(b, "%d", swapbits(unpack32(rea, 0x000003FF), 10));
    fl_set_object_label(obj, b);
    fl_set_object_color(obj, FL_GREEN, 0);
  }

  obj = VALUE_R;
  if (rea_nn) {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_YELLOW, 0);
  }
  else {
    sprintf(b, "%.1fV", swapbits(unpack32(rea, 0x000003FF), 10) / 1023.0 * 900.0);
    fl_set_object_label(obj, b);
    fl_set_object_color(obj, FL_GREEN, 0);
  }

  obj = CODE_W;
  sprintf(b, "%d", swapbits(unpack32(inp, 0x000003FF), 10));
  fl_set_object_label(obj, b);
  if (unpack32(inp^wri, 0x000003FF)) fl_set_object_color(obj, FL_YELLOW, 0);
  else                               fl_set_object_color(obj, FL_GREEN, 0);

  obj = VALUE_W;
  sprintf(b, "%.1fV", swapbits(unpack32(inp, 0x000003FF), 10) / 1023.0 * 900.0);
  fl_set_object_label(obj, b);
  if (unpack32(inp^wri, 0x000003FF)) fl_set_object_color(obj, FL_YELLOW, 0);
  else                               fl_set_object_color(obj, FL_GREEN, 0);

//~--

  if (first || rw != old_rw || func != old_func) { 
    first = FALSE;
    old_func = func;
    old_rw   = rw;
    for (i=0; i<4; i++) {
      fl_hide_object(GRP[0][i]);
      fl_hide_object(GRP[1][i]);
    }  
    if (func != -1) {
      int k0 = functions[func]._0[rw];
      int k1 = functions[func]._1[rw];
      fl_show_object(GRP[0][k0]);
      fl_show_object(GRP[1][k1]);
    }
    else {
      fl_show_object(GRP[0][0]);
      fl_show_object(GRP[1][0]);
    }
  }
  
//~--

  for (i=0; i<32; i++) {
    int in = inp & 0x80000000>>i;
    int wr = wri & 0x80000000>>i;
    obj = INP[i];
    if (in) fl_set_object_label(obj, "1");
    else    fl_set_object_label(obj, "0");
    if (in != wr) fl_set_object_color(obj, FL_YELLOW, FL_RED);
    else          fl_set_object_color(obj, FL_GREEN, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
    obj = WRI[i];
    if (wr) fl_set_object_label(obj, "1");
    else    fl_set_object_label(obj, "0");
    fl_set_object_color(obj, FL_GREEN, 0);
    fl_set_object_lcolor(obj, FL_BLACK);
  }

  if (unpack32(inp, 0x80000000) == 0) {
    obj = INP[0];
    fl_set_object_color(obj, FL_RED, FL_BLACK);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }

  for (m=0x80000000,p=1; m; m>>=1) if (inp & m && m != 0x40000000) p ^= 1;
  if (p  != unpack32(inp, 0x40000000)) {
    obj = INP[1];
    fl_set_object_color(obj, FL_RED, FL_BLACK);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }

//~--

  for (i=0; i<32; i++) {
    int re = rea & 0x80000000>>i;
    obj = REA[i];
    if (rea_nn) {
      fl_set_object_label(obj, " ");
      fl_set_object_color(obj, FL_YELLOW, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    else {
      if (re) fl_set_object_label(obj, "1");
      else    fl_set_object_label(obj, "0");
      fl_set_object_color(obj, FL_GREEN, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
  rea_nn = TRUE;
}

//~===============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_bus_panel();
  update_dat_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
