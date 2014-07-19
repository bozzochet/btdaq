// file UGPD.c
//
// UGPD Controller
//
// A.Lebedev Feb-2008...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"UGPD  Controller"};                   // global variable
char *date  = {"16-Jul-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;
bool  chk;

//~-- Actel (or LeCroy Bus) Selector

SW_BTN  *ACTEL_BTN[1];
SW_BTN  *BUS_BTN[1];
_ww w_actel;
_b actel = {0, 0x00000007, 8};
char *actel_a_b[] = {NULL, NULL, NULL, NULL, NULL, NULL, "A", "B"};
char *actel_bus[] = {"0",  "1",  "2",  "3",  "4",  "5",  "6", "7"};

//~-- LeCroy Bus Command Counters

DAT_BOX *CNT_W_BOX[2];
DAT_BOX *CNT_R_BOX[2];
_r  r_cnt_w;
_r  r_cnt_r;
_b cnt[2] = {
  {0, -1, -1}, 
  {1, -1, -1}};

//~-- DC/DC (and Actel) Control

FL_OBJECT *ACTEL_BOX;
SW_BTN  *CMD_BTN[7];
DAT_BOX *STA_BOX[4][7];
_ww w_cmd;
_r  r_sta;
bool auto_read_DCDC  = FALSE;
bool auto_write_DCDC = FALSE;

_b cmd[7] = {
  {0, 0xC000, 3},   // Actel on the other half
  {0, 0x0003, 3},   //    5V half A
  {0, 0x000C, 3},   //   12V half A
  {0, 0x0030, 3},   //   29V half A
  {0, 0x0300, 3},   //    5V half B
  {0, 0x0C00, 3},   //   12V half B
  {0, 0x3000, 3}};  //   29V half B
_b sta[4][7] = {
 {{0, 0x0008, 2},   // Actel on the other half LOCAL Control
  {0, 0x0001, 2},   //    5V half A Local Control
  {0, 0x0002, 2},   //   12V half A Local Control
  {0, 0x0004, 2},   //   29V half A Local Control
  {0, 0x0010, 2},   //    5V half B Local Control
  {0, 0x0020, 2},   //   12V half B Local Control
  {0, 0x0040, 2}},  //   29V half B Local Control
  
 {{0, 0x0008, 2},   // Actel on the other half LOCAL Control
  {1, 0x0001, 2},   //    5V half A Global Control
  {1, 0x0002, 2},   //   12V half A Global Control
  {1, 0x0004, 2},   //   29V half A Global Control
  {1, 0x0010, 2},   //    5V half B Global Control
  {1, 0x0020, 2},   //   12V half B Global Control
  {1, 0x0040, 2}},  //   29V half B Global Control
  
 {{0, 0x0080, 2},   // Actel on the other half Trip
  {1, 0x0100, 2},   //    5V half A Trip
  {1, 0x0200, 2},   //   12V half A Trip
  {1, 0x0400, 2},   //   29V half A Trip
  {1, 0x1000, 2},   //    5V half B Trip
  {1, 0x2000, 2},   //   12V half B Trip
  {1, 0x4000, 2}},  //   29V half B Trip
  
 {{0, 0x0800, 2},   // Actel on the other half Status
  {0, 0x0100, 2},   //    5V half A Status
  {0, 0x0200, 2},   //   12V half A Status
  {0, 0x0400, 2},   //   29V half A Status
  {0, 0x1000, 2},   //    5V half B Status
  {0, 0x2000, 2},   //   12V half B Status
  {0, 0x4000, 2}}}; //   29V half B Status
  
char *o_o[] = {"-", "ON", "OFF"};
char *o_t[] = {"",   "TRIP"};
char *O_O[] = {"ON", "OFF"};
char *o_n[] = {"OK", "NOK"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_actel_selector_panel(int x0, int y0);
void actel_selector_callback(int32 data);
void INIT_callback(FL_OBJECT *obj, long data);
void update_actel_selector_panel(void);

void create_command_counters_panel(int x0, int y0);
void reset_command_counters_callback(FL_OBJECT *obj, long data);
void update_command_counters_panel(void);

void create_DCDC_control_panel(int x0, int y0);
void control_DCDC_callback(FL_OBJECT *obj, long data);
void write_DCDC_callback(FL_OBJECT *obj, long data);
void read_DCDC_callback(FL_OBJECT *obj, long data);
void auto_write_read_DCDC_callback(int tid, void *data);
void update_DCDC_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 430, HH = 250;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
//AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_actel_selector_panel(  0,  45);
  create_command_counters_panel(0,  85);
  create_DCDC_control_panel(  140,  45);

  create_USCM_control_H3S_panel(0, 165);
  
  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  static bool first = TRUE;
  
  invalidate_ww(first, &w_actel, 1, 0);

  invalidate_ww(first, &w_cmd, 1, 0);
  invalidate_r(first, &r_sta, 2);

  invalidate_r(first, &r_cnt_w, 2);
  invalidate_r(first, &r_cnt_r, 2);

  read_dat_config_file(dat_config_file_name);

  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  
  file = fopen(filename, "r");
  if (file) {
    fread(w_actel.inp, 4, 1, file);
    fread(w_cmd.inp,   4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file) {
    fwrite(w_actel.inp, 4, 1, file);
    fwrite(w_cmd.inp,   4, 1, file);
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
//                  ACTEL SELECTOR PANEL
//
//~============================================================================

void create_actel_selector_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 35, w2 = 15, w3 = 25, w4 = 15, w5 = 40, h1 = 15;

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Actel", 'S');
  x = x + w1;
  
  create_btn_hstring(1, x, y, w2, h1, actel_a_b, 'N', &w_actel, &actel, &ACTEL_BTN[0]);
  ACTEL_BTN[0]->call = actel_selector_callback;
  x = x + w2;

  add_frame_box(x, y, w3, h1, "Bus", 'S');
  x = x + w3;
  
  create_btn_hstring(1, x, y, w4, h1, actel_bus, 'N', &w_actel, &actel, &BUS_BTN[0]);
  BUS_BTN[0]->call = actel_selector_callback;
  x = x + w4;

  obj = add_normal_button(x, y, w5, h1, "INIT", 'N');
    fl_set_object_callback(obj, INIT_callback, 0);
}

//~----------------------------------------------------------------------------

void INIT_callback(FL_OBJECT *obj, long data) {

  int8 bus = w_actel.inp[0];
  
  msg = -1;
  update_objects();

  chk = 0;
  
  bus = (bus << 4) | bus;
  init_LeCroy_bus(bus, &err);

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void actel_selector_callback(int32 data) {

  _r *r = &r_sta;
  
  r->sta = status_NA;
  r->unk = TRUE;
  
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_actel_selector_panel(void) {

  int color, lcolor;

  show_btn_string(1, NULL, &ACTEL_BTN[0]);
  show_btn_string(1, NULL, &BUS_BTN[0]);

  color  = (ACTEL_BTN[0]->obj->label[0]) ? FL_WHEAT : FL_RED;
  lcolor = (ACTEL_BTN[0]->obj->label[0]) ? FL_BLACK : FL_YELLOW;
  fl_set_object_color(ACTEL_BTN[0]->obj, color, 0);
  fl_set_object_lcolor(ACTEL_BTN[0]->obj, lcolor);

  fl_set_object_color(BUS_BTN[0]->obj, color, 0);
  fl_set_object_lcolor(BUS_BTN[0]->obj, lcolor);
}

//~============================================================================
//
//                  LECROY BUS COMMAND COUNTERS PANEL
//
//~============================================================================

void create_command_counters_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 45, h1 = 15;
  
  w = 5 + w1 + 2 * w2 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1,  h1, "LCR",    'S');
  y = y + h1;
  add_frame_box(x, y, w1,  h1, "Total",  'S');
  y = y + h1;
  add_frame_box(x, y, w1,  h1, "Errors", 'S');
  y = y + h1;
  add_frame_box(x, y, w1,  h1, "",       'S');
  x = x + w1;

  y = y0 + 5;
  add_frame_box(x, y, w2,  h1, "write", 'S');
  create_dat_box_vstring(
    2, x, y+h1, w2, h1, 'N', &r_cnt_w, cnt, CNT_W_BOX);
  obj = add_normal_button(x, y+3*h1, 2*w2, h1, "RESET",  'N');
    fl_set_object_callback(obj, reset_command_counters_callback, 0);
  x = x + w2;

  add_frame_box(x, y, w2,  h1, "read",  'S');
  create_dat_box_vstring(
    2, x, y+h1, w2, h1, 'N', &r_cnt_r, cnt, CNT_R_BOX);

  reset_command_counters_callback(NULL, 0);
}

//~----------------------------------------------------------------------------

void reset_command_counters_callback(FL_OBJECT *obj, long data) {

  r_cnt_w.unk = FALSE;
  r_cnt_r.unk = FALSE;
  
  r_cnt_w.sta = status_OK;
  r_cnt_r.sta = status_OK;

  r_cnt_w.rea[0] = 0;
  r_cnt_w.rea[1] = 0;
  r_cnt_r.rea[0] = 0;
  r_cnt_r.rea[1] = 0;

  if (obj) update_objects();  
}

//~----------------------------------------------------------------------------

void update_command_counters_panel(void) {

  show_dat_box_string(2, NULL, "%5d", CNT_W_BOX);
  show_dat_box_string(2, NULL, "%5d", CNT_R_BOX);
}

//~============================================================================
//
//                  DC/DC (AND ACTEL) CONTROL PANEL
//
//~============================================================================

void create_DCDC_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 25, w2 = 40, w3 = 30, w4 = 30, w5 = 30, w6 = 30, w7 = 30, w8 = 35, w9 = 30;
  int h1 = 15, h2 = 15, h3 = 20;
  char *txt1[] = {"A", "B"};
  char *txt2[] = {"5V", "12V", "29V", "5V", "12V", "29V"};
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + w7 + w8 + w9 + 5;
  h = 5 + 2 * h1 + h2 + 5 + 3 * h2 + 5 + 3 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  w = w1 + w2;
  add_frame_box(x, y,    w,  2*h1, "Unit", 'S');
  x = x + w;
  w = w3 + w4 + w5;
  add_frame_box(x, y,    w,  2*h1, "Switch", 'S');
  x = x + w;
  w = w6 + w7;
  add_frame_box(x, y,    w,    h1, "Ctrl", 'S');
  add_frame_box(x, y+h1, w6,   h1, "Locl", 'S');
  x = x + w6;
  add_frame_box(x, y+h1, w7,   h1, "Glbl", 'S');
  x = x + w7;
  add_frame_box(x, y,    w8, 2*h1, "Trip", 'S');
  x = x + w8;
  add_frame_box(x, y,    w9, 2*h1, "Sta", 'S');
  x = x + w9;
  y = y + 2 * h1;
  
  x = x0 + 5;
  w = w1 + w2;
  ACTEL_BOX = add_frame_box(x, y, w, h2, "", 'S');
  y = y + h2 + 5;

  create_frame_box_vstring(2, x, y, w1, 3*h2, &txt1[0], 'M'+100, 'C');
  x = x + w1;
  
  create_frame_box_vstring(6, x, y, w2,   h2, &txt2[0], 'S'+300, 'C');
  x = x + w2;

  y = y0 + 5 + 2 * h1;
  for (i=0; i<7; i++) {
    int h = i ? h2 : h1;
    obj = add_normal_button(x, y, w3, h, "ON",  'N');
      fl_set_object_callback(obj, control_DCDC_callback, 2*i+1);
    y = y + h;
    if (i == 0 || i == 3) y = y + 5;
  }
  x = x + w3;

  y = y0 + 5 + 2 * h1;
  for (i=0; i<7; i++) {
    int h = i ? h2 : h1;
    obj = add_normal_button(x, y, w4, h, "OFF",  'N');
      fl_set_object_callback(obj, control_DCDC_callback, 2*i+0);
    y = y + h;
    if (i == 0 || i == 3) y = y + 5;
  }
  x = x + w4;

  y = y0 + 5 + 2 * h1;

  add_frame_box(x, y, w3, h1, "", 'M');
  create_btn_vstring(
    6, x, y+h1+5, w5, h2, o_o, 'N'+300, &w_cmd, &cmd[1], &CMD_BTN[1]);
  x = x + w5;

  create_dat_box_vstring(
    1, x, y,      w6, h1, 'N',     &r_sta, &sta[0][0], &STA_BOX[0][0]);
  create_dat_box_vstring(
    6, x, y+h1+5, w6, h2, 'N'+300, &r_sta, &sta[0][1], &STA_BOX[0][1]);
  x = x + w6;

  add_frame_box(x, y, w7, h1, "", 'M');
  create_dat_box_vstring(
    6, x, y+h1+5, w7, h2, 'N'+300, &r_sta, &sta[1][1], &STA_BOX[1][1]);
  x = x + w7;

  create_dat_box_vstring(
    1, x, y,      w8, h1, 'N',     &r_sta, &sta[2][0], &STA_BOX[2][0]);
  create_dat_box_vstring(
    6, x, y+h1+5, w8, h2, 'N'+300, &r_sta, &sta[2][1], &STA_BOX[2][1]);
  x = x + w8;
  
  create_dat_box_vstring(
    1, x, y,      w9, h1, 'N',     &r_sta, &sta[3][0], &STA_BOX[3][0]);
  create_dat_box_vstring(
    6, x, y+h1+5, w9, h2, 'N'+300, &r_sta, &sta[3][1], &STA_BOX[3][1]);
  x = x + w9;
  
  x = x0 + 5;
  y = y0 + 5 + 2 * h1 + h2 + 5 + 3 * h2 + 5 + 3 * h2;
  w = w1 + w2 + w3 + w4;
  obj = add_frame_box(x, y, w, h3, "Command", 'S');
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  x = x + w;
  obj = add_normal_button(x, y, w5, h3, "W",  'N');
    fl_set_object_callback(obj, write_DCDC_callback, 0);
  x = x + w5;
  w = w6 + w7 + w8 + w9;
  obj = add_normal_button(x, y, w, h3, "R",  'N');
    fl_set_object_callback(obj, read_DCDC_callback, 0);
  x = x + w;
}

//~----------------------------------------------------------------------------

void control_DCDC_callback(FL_OBJECT *obj, long data) {

  int8  ind = data / 2;
  int8  dat = data % 2;
  int8  bus = w_actel.inp[0];
  int16 brd = 3, reg = 0;
  int32 wri, rea;
  int32 m;
  int p;
  _r  *r = &r_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;
  
  bus = (bus << 4) | bus;
  wri = 0xA0080000;
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00070000, unpack16(reg, 0x0007));
  pack32(&wri, 0x00700000, unpack16(reg, 0x0038));
  pack32(&wri, 0x0000FFFF, pack16(NULL, cmd[ind].msk, dat ? 1 : 2));
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
  
  chk = TRUE;

  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);
  if (!err) {
    r_cnt_w.rea[0]++;
    if (chk) r_cnt_w.rea[1]++;
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void write_DCDC_callback(FL_OBJECT *obj, long data) {

  int8  bus = w_actel.inp[0];
  int16 brd = 3, reg = 0;
  int32 wri, rea;
  int32 m;
  int p;
  _ww *w = &w_cmd;
  _r  *r = &r_sta;

  if (obj) {
    if (auto_write_DCDC) {
      auto_write_DCDC = FALSE;
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_CYAN, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_write_DCDC = TRUE;
        if(!auto_read_DCDC) {
          fl_add_timeout(0, auto_write_read_DCDC_callback, NULL);
        }
        fl_set_object_boxtype(obj, FL_DOWN_BOX);
        fl_set_object_color(obj, FL_RED, FL_RED);
        fl_set_object_lcolor(obj, FL_YELLOW);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_write_DCDC) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;
  
  bus = (bus << 4) | bus;
  wri = 0xA0080000;
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00070000, unpack16(reg, 0x0007));
  pack32(&wri, 0x00700000, unpack16(reg, 0x0038));
  pack32(&wri, 0x00003FFF, w->inp[0]);
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
  
  chk = TRUE;

  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);
  if (!err) {
    r_cnt_w.rea[0]++;
    if (chk) r_cnt_w.rea[1]++;
  }

  if (!err && !chk) {
    w->wri[0] = w->inp[0];
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }

  if (auto_write_DCDC) msg = -3;
  else                 msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void read_DCDC_callback(FL_OBJECT *obj, long data) {

  int8  bus[2] = {w_actel.inp[0], w_actel.inp[0]};
  int8  check[2] = {TRUE, TRUE};
  int16 brd = 3, reg;
  int32 wri[2], rea[2];
  int32 m;
  int p;
  _r *r = &r_sta;

  if (obj) {
    if (auto_read_DCDC) {
      auto_read_DCDC = FALSE;
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_CYAN, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_read_DCDC = TRUE;
        if(!auto_write_DCDC) {
          fl_add_timeout(0, auto_write_read_DCDC_callback, NULL);
        }
        fl_set_object_boxtype(obj, FL_DOWN_BOX);
        fl_set_object_color(obj, FL_RED, FL_RED);
        fl_set_object_lcolor(obj, FL_YELLOW);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_read_DCDC) {
    msg = -1;
    update_objects();
  }

  for (reg=0; reg<2; reg++) {
    r->was[reg] = r->rea[reg];
    bus[reg] = (bus[reg] << 4) | bus[reg];
    wri[reg] = 0xA0000000;
    pack32(&wri[reg], 0x0F800000, brd);
    pack32(&wri[reg], 0x00070000, unpack16(reg, 0x0007));
    pack32(&wri[reg], 0x00700000, unpack16(reg, 0x0038));
    for (m=0x80000000,p=1; m; m>>=1) if (wri[reg] & m) p ^= 1;
    if (p) wri[reg] ^= 0x40000000;
  }
  
  exec_LeCroy_cmd(2, bus, wri, rea, check, &err);
  chk = check[0] | check[1];
  if (!err) {
    r_cnt_r.rea[0]++;
    if (chk) r_cnt_r.rea[1]++;
  }

  if (!err && !chk) {
    for (reg=0; reg<2; reg++) {
      r->rea[reg] = unpack32(rea[reg], 0x0000FFFF);
      if (r->unk) r->was[reg] = r->rea[reg];
    }
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  if (auto_read_DCDC) msg = -3;
  else                msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void auto_write_read_DCDC_callback(int tid, void *data) {

//int period = 1000;  // in ms
  int period =    1;  // in ms

  if (auto_write_DCDC) {
    if (P > 1) {
      static struct timeval now, then;
      gettimeofday(&now, NULL);
      printf("auto_write_DCDC: tid=%d dt=%f\n", tid, delta_t(&now, &then));
      memcpy(&then, &now, sizeof(now));
    }
    write_DCDC_callback(NULL, 0);
  }
  
  if (auto_read_DCDC) {
    if (P > 1) {
      static struct timeval now, then;
      gettimeofday(&now, NULL);
      printf("auto_read_DCDC: tid=%d dt=%f\n", tid, delta_t(&now, &then));
      memcpy(&then, &now, sizeof(now));
    }
    read_DCDC_callback(NULL, 0);
  }
  
  if (auto_write_DCDC || auto_read_DCDC) {
    fl_add_timeout(period, auto_write_read_DCDC_callback, NULL);
  }
}

//~----------------------------------------------------------------------------

void update_DCDC_panel(void) {

  int i;
  
  fl_set_object_label(ACTEL_BOX, w_actel.inp[0] ? "Actel A" : "Actel B");

  show_btn_string(6, NULL, &CMD_BTN[1]);
  show_dat_box_string(7, O_O, NULL, &STA_BOX[0][0]);
  show_dat_box_string(6, O_O, NULL, &STA_BOX[1][1]);
  show_dat_box_string(7, o_t, NULL, &STA_BOX[2][0]);
  for (i=0; i<7; i++) {
    DAT_BOX *B = STA_BOX[2][i];
    if (!strcmp(fl_get_object_label(B->obj),"TRIP")) {
      fl_set_object_color(B->obj, FL_RED, 0);
      fl_set_object_lcolor(B->obj, FL_YELLOW);
    }
  }
  show_dat_box_string(7, o_n, NULL, &STA_BOX[3][0]);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_actel_selector_panel();
  update_command_counters_panel();
  update_DCDC_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
