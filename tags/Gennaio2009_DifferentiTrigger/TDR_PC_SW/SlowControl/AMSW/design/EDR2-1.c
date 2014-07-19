// file EDR2-1.c
//
// EDR2-1 "Pro" Controller
// - simple program to set delays & thresholds
//
// A.Lebedev Sep-2006...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"EDR2-1: Pro Controller"};            // global variable
char *date  = {"25-Nov-07"};                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"
int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~----------------------------------------------------------------------------

struct {
  char *name;
  char *half;
  int   slot;
  int   link;
} map[12] = {
//
// board---half-slot-link
  {"EDR0", "A",   1,   0},
  {"EDR0", "B",   1,   1},
  {"EDR1", "A",   3,   4},
  {"EDR1", "B",   3,   5},

  {"EDR2", "A",   4,   8},
  {"EDR2", "B",   4,   9},
  {"EDR3", "A",   6,  12},
  {"EDR3", "B",   6,  13},

  {"EDR4", "A",   8,  16},
  {"EDR4", "B",   8,  17},
  {"EDR5", "A",  10,  20},
  {"EDR5", "B",  10,  21}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~-- Slave Mask Panel

SW_BTN  *SLAVE_BTN[12];
DAT_BOX *SLAVE_BOX[12];
_r  r_slave;
_ww w_slave;

//~-- Delay Panel

SW_BTN  *DELAY_BTN[12][2];
DAT_BOX *DELAY_BOX[12];
_r  r_delay[12];
_ww w_delay[12];

//~-- Threshold Panel

SW_BTN  *THR_BTN[2][12][5];
DAT_BOX *THR_BOX[2][12];
_r  r_thr[2][12];
_ww w_thr[2][12];

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
char *_05[2] = {".0", ".5"};
char *A_B[12] = {"A", "B", "A", "B", "A", "B", "A", "B", "A", "B", "A", "B"};
  
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_slave_mask_panel(int x0, int y0);
void set_slave_mask_callback(FL_OBJECT* obj, long data);
void get_slave_mask_callback(FL_OBJECT* obj, long data);
void test_slaves_callback(FL_OBJECT* obj, long data);
void update_slave_mask_panel(void);

void create_delay_panel(int x0, int y0);
void convert_delay_code_to_value(int32 code, int32 *value);
void set_delay_callback(FL_OBJECT* obj, long data);
void get_delay_callback(FL_OBJECT* obj, long data);
void set_all_delays_callback(FL_OBJECT* obj, long data);
void get_all_delays_callback(FL_OBJECT* obj, long data);
void update_delay_panel(void);

void create_threshold_panel(int x0, int y0);
void set_threshold_callback(FL_OBJECT* obj, long data);
void get_threshold_callback(FL_OBJECT* obj, long data);
void set_all_thresholds_callback(FL_OBJECT* obj, long data);
void get_all_thresholds_callback(FL_OBJECT* obj, long data);
void update_threshold_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 630, HH = 315;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_slave_mask_panel( 0, 45);
  create_delay_panel(    100, 45);
  create_threshold_panel(245, 45);

  end_form(MAIN);
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void invalidate_data(void) {

  int i, j;
  static bool first = TRUE;

  r_slave.unk = TRUE;
  r_slave.sta = status_NA;
  if (first) r_slave.was = malloc(4);
  if (first) r_slave.rea = malloc(4);
  
  w_slave.unk = TRUE;
  if (first) w_slave.inp = malloc(4);
  if (first) w_slave.wri = malloc(4);
  w_slave.inp[0] = 0x00000000;

  for (i=0; i<12; i++) {
    r_delay[i].unk = TRUE;
    r_delay[i].sta = status_NA;
    if (first) r_delay[i].was = malloc(4);
    if (first) r_delay[i].rea = malloc(4);
    w_delay[i].unk = TRUE;
    if (first) w_delay[i].inp = malloc(4);
    if (first) w_delay[i].wri = malloc(4);
    w_delay[i].inp[0] = 0x00;
  }

  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      r_thr[j][i].unk = TRUE;
      r_thr[j][i].sta = status_NA;
      if (first) r_thr[j][i].was = malloc(4);
      if (first) r_thr[j][i].rea = malloc(4);
      w_thr[j][i].unk = TRUE;
      if (first) w_thr[j][i].inp = malloc(4);
      if (first) w_thr[j][i].wri = malloc(4);
      w_thr[j][i].inp[0] = 0x000;
    }
  }

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

//~-------------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  int i, j;
  FILE *file;
  
  file = fopen(filename, "r");
  if (file != NULL) {
    fread(w_slave.inp,     4, 1, file);
    for (i=0; i<12; i++) {
      fread(w_delay[i].inp,    4, 1, file);
    }
    for (j=0; j<2; j++) {
      for (i=0; i<12; i++) {
        fread(w_thr[j][i].inp, 4, 1, file);
      }
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  int i, j;
  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(w_slave.inp,     4, 1, file);
    for (i=0; i<12; i++) {
      fwrite(w_delay[i].inp,    4, 1, file);
    }
    for (j=0; j<2; j++) {
      for (i=0; i<12; i++) {
        fwrite(w_thr[j][i].inp, 4, 1, file);
      }
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~===============================================================================
//
//            SLAVE MASK PANEL
//
//~===============================================================================

void create_slave_mask_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int size;
  int w1 = 15, w2 = 20, w3 = 20, h1 = 15, h2 = 20;
  
  _b slave_msk[12] = {   // board half slot link link
  { 0, 0x000001, 2},     //  EDR1   A    1    0  0x00
  { 0, 0x000002, 2},     //  EDR1   B    1    1  0x01
  { 0, 0x000010, 2},     //  EDR2   A    3    4  0x04
  { 0, 0x000020, 2},     //  EDR2   B    3    5  0x05

  { 0, 0x000100, 2},     //  EDR3   A    4    8  0x08
  { 0, 0x000200, 2},     //  EDR3   B    4    9  0x09
  { 0, 0x001000, 2},     //  EDR4   A    6   12  0x0C
  { 0, 0x002000, 2},     //  EDR4   B    6   13  0x0D

  { 0, 0x010000, 2},     //  EDR5   A    8   16  0x10
  { 0, 0x020000, 2},     //  EDR5   B    8   17  0x11
  { 0, 0x100000, 2},     //  EDR6   A   10   20  0x14
  { 0, 0x200000, 2}};    //  EDR6   B   10   21  0x14

  for (i=0; i<12; i++) {
    int32 m = 1 << map[i].link;
    if (slave_msk[i].msk != m) {
      printf("wrong data in slave mask: ");
      printf("i = %2d, link = %2d, slave_msk = %08X, m = %08X\n",
              i, map[i].link, slave_msk[i].msk, m);
    }
  }

  w = 5 + 2 * w1 + 2 * w2 + w3 + 5;
  h = 5 + 2 * h1 + 12 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, 2*w1,    2*h1, "EDR", FL_SMALL_SIZE);
  x = x + 2 * w1;
  add_frame_box(x, y, 2*w2+w3, 2*h1, "Slave\nMask", FL_NORMAL_SIZE);
  x = x0 + 5;
  y = y + 2 * h1;
  
  size = FL_SMALL_SIZE;
  
  create_frame_box_vstring( 6, x, y, w1, 2*h1, hex, size, 'C');
  x = x + w1;
  create_frame_box_vstring(12, x, y, w1,   h1, A_B, size, 'C');
  x = x + w1;

  size = FL_NORMAL_SIZE;

  create_btn_vstring(
    12, x, y, w2, h1, hex, size, &w_slave, &slave_msk[0], &SLAVE_BTN[0]);
  x = x + w2;
  create_dat_box_vstring(
    12, x, y, w2, h1,      size, &r_slave, &slave_msk[0], &SLAVE_BOX[0]);
  x = x + w2;
  obj = add_normal_button(
        x, y, w3, 6*2*h1+h2,"S\nL\nA\nV\nE\n \nT\nE\nS\nT", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, test_slaves_callback, 0);

  x = x0 + 5;
  y = y + 12 * h1;
  add_frame_box(x, y, 2*w1, h2, "", size);
  x = x + 2 * w1;
  obj = add_normal_button(x, y, w2, h2, "W", size);
    fl_set_object_callback(obj, set_slave_mask_callback, 0);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h2, "R", size);
    fl_set_object_callback(obj, get_slave_mask_callback, 0);
}

//~-------------------------------------------------------------------------------

void set_slave_mask_callback(FL_OBJECT* obj, long data) {

  _r  *r = &r_slave;
  _ww *w = &w_slave;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  AMSW_PATH[0] = 0x0000;
  set_JINF_slave_mask(0, w->inp[0], &err);

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

//~-------------------------------------------------------------------------------

void get_slave_mask_callback(FL_OBJECT* obj, long data) {

  _r  *r = &r_slave;
  int32 masks[16];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  
  AMSW_PATH[0] = 0x0000;
  get_JINF_slave_masks(masks, &err);

  if (!err) {
    r->rea[0] = masks[0];
#ifdef SIMULATION
    r->rea[0] = w_slave.wri[0];
    printf("simulated slave mask read: %08X %c\n", r->rea[0], 7);
#endif
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

//~-------------------------------------------------------------------------------

void test_slaves_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  AMSW_PATH[0] = 0x0000;
  test_JINF_slaves(&err);
  
  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_slave_mask_panel(void) {

  show_btn_string(    12,      NULL, &SLAVE_BTN[0]);
  show_dat_box_string(12, hex, NULL, &SLAVE_BOX[0]);
}

//~===============================================================================
//
//            DELAY PANEL
//
//~===============================================================================

void create_delay_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int size;
  int w1 = 15, w2 = 20, w3 = 35, h1 = 15, h2 = 20;

  _b delay_hex_msk[12][2] = {
 {{ 0, 0x0070,  8},   // 
  { 0, 0x000F, 16}}}; // 

  _b delay_num_msk[12] = {
  { 0, 0x007F, -1}};
  
  w = 5 + 2 * w1 + 2 * w1 +  w2 + w3 + w2 + 5;
  h = 5 + 2 * h1 + 12 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, 2*w1, 2*h1, "EDR", FL_SMALL_SIZE);
  x = x + 2 * w1;
  add_frame_box(x, y, 2*w1+w2+w3+w2, 2*h1, "Delay, ns", FL_NORMAL_SIZE);
  x = x0 + 5;
  y = y + 2 * h1;
  
  size = FL_SMALL_SIZE;
  
  create_frame_box_vstring( 6, x, y, w1, 2*h1, hex, size, 'C');
  x = x + w1;
  create_frame_box_vstring(12, x, y, w1,   h1, A_B, size, 'C');
  x = x + w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<12; i++) {
    int j;
    delay_num_msk[i].ind = delay_num_msk[0].ind;
    delay_num_msk[i].msk = delay_num_msk[0].msk;
    for (j=0; j<2; j++) {
      delay_hex_msk[i][j].ind = delay_hex_msk[0][j].ind;
      delay_hex_msk[i][j].msk = delay_hex_msk[0][j].msk;
      delay_hex_msk[i][j].n   = delay_hex_msk[0][j].n;
    }
    x = x0 + 5 + 2 * w1;
    create_btn_hstring(
      2, x, y, w1, h1, hex, size, &w_delay[i], &delay_hex_msk[i][0], &DELAY_BTN[i][0]);
    x = x + 2 * w1;
    obj = add_normal_button(x, y, w2, h1, "W", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, set_delay_callback, i);
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w3, h1,      size, &r_delay[i], &delay_num_msk[i], &DELAY_BOX[i]);
    DELAY_BOX[i] -> cnv = convert_delay_code_to_value;
    x = x + w3;
    obj = add_normal_button(x, y, w2, h1, "R", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, get_delay_callback, i);
    y = y + h1;
  }

  x = x0 + 5;
  add_frame_box(x, y, 2*w1, h2, "", size);
  x = x + 2 * w1;
  obj = add_normal_button(x, y, 2*w1+w2, h2, "SET", size);
    fl_set_object_callback(obj, set_all_delays_callback, 0);
  x = x + 2 * w1 + w2;
  obj = add_normal_button(x, y, w3+w2, h2, "GET", size);
    fl_set_object_callback(obj, get_all_delays_callback, 0);
}

//~-------------------------------------------------------------------------------

void convert_delay_code_to_value(int32 code, int32 *value) {

  *value = 200 + 20 * code;
}

//~-------------------------------------------------------------------------------

void set_delay_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _r  *r = &r_delay[ind];
  _ww *w = &w_delay[ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  AMSW_PATH[0] = map[ind].link << 8 | 0x04;
  AMSW_PATH[1] = 0x0000;
  set_EDR2_delay(w->inp[0], &err);

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

//~-------------------------------------------------------------------------------

void get_delay_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int8 delay;
  _r  *r = &r_delay[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  
  AMSW_PATH[0] = map[ind].link << 8 | 0x04;
  AMSW_PATH[1] = 0x0000;
  get_EDR2_delay(&delay, &err);

  if (!err) {
#ifdef SIMULATION
    r->rea[0] = w_delay[ind].wri[0];
    printf("simulated delay read: %02X %c\n", r->rea[0], 7);
#else
    r->rea[0] = delay;
#endif
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

//~-------------------------------------------------------------------------------

void set_all_delays_callback(FL_OBJECT* obj, long data) {

  int i;
  int16 error = 0x0000;
  _r *s = &r_slave;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<12; i++) {
    int32 m = 1 << map[i].link;
    r_delay[i].sta = status_NA;
    w_delay[i].unk = TRUE;
    if (!s->unk && (s->sta == status_OK) && (s->rea[0] & m)) {
      set_delay_callback(NULL, i);
      error |= err;
    }
  }

  err = error;
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void get_all_delays_callback(FL_OBJECT* obj, long data) {

  int i;
  int16 error = 0x0000;
  _r *s = &r_slave;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<12; i++) {
    int32 m = 1 << map[i].link;
    r_delay[i].sta = status_NA;
    if (!s->unk && (s->sta == status_OK) && (s->rea[0] & m)) {
      get_delay_callback(NULL, i);
      error |= err;
    }
  }

  err = error;
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void update_delay_panel(void) {

  int i;
  
  for (i=0; i<12; i++) {
    show_btn_string(    2,       NULL,  &DELAY_BTN[i][0]);
    show_dat_box_string(1, NULL, "%4d", &DELAY_BOX[i]);
  }
}

//~===============================================================================
//
//            TIGGER THRESHOLD PANEL
//
//~===============================================================================

void create_threshold_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, x1, y1, w, h;
  int size;
  int w1 = 15, w2 = 20, w3 = 50, h1 = 15, h2 = 20;

  _b thr_dec_msk[12][5] = {
  {{ 0, 0x0FFE, 10},
   { 0, 0x0FFE, 10},
   { 0, 0x0FFE, 10},
   { 0, 0x0FFE, 10},
   { 0, 0x0001,  2}}};

  _b thr_num_msk[12] = {
   { 0, 0x0FFF, -1}};
  
  w = 5 + 2 * w1 + 2 * (4 * w1 + w2 + w2 + w3 + w2 + 5) - 5 + 5;
  h = 5 + 2 * h1 + 12 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, 2*w1, 2*h1, "EDR", FL_SMALL_SIZE);
  x = x + 2 * w1;
  add_frame_box(x, y, w-10-2*w1, h1, "Trigger Threshold, mV", FL_NORMAL_SIZE);
  y = y + h1;
  w = 4 * w1 + w2 + w2 + w3 + w2;
  add_frame_box(x, y, w, h1, "DAC0", FL_SMALL_SIZE);
  x = x + w + 5;
  add_frame_box(x, y, w, h1, "DAC1", FL_SMALL_SIZE);
  y = y + h1;

  x = x0 + 5;
  
  size = FL_SMALL_SIZE;
  
  create_frame_box_vstring( 6, x, y, w1, 2*h1, hex, size, 'C');
  add_frame_box(x, y+6*2*h1, 2*w1, h2, "", size);
  x = x + w1;
  create_frame_box_vstring(12, x, y, w1,   h1, A_B, size, 'C');
  x = x + w1;

  size = FL_NORMAL_SIZE;

  for (i=0; i<12; i++) {
    thr_num_msk[i].ind = thr_num_msk[0].ind;
    thr_num_msk[i].msk = thr_num_msk[0].msk;
    for (j=0; j<5; j++) {
      thr_dec_msk[i][j].ind = thr_dec_msk[0][j].ind;
      thr_dec_msk[i][j].msk = thr_dec_msk[0][j].msk;
      thr_dec_msk[i][j].n   = thr_dec_msk[0][j].n;
    }
  }

  x1 = x0 + 5 + 2 * w1;
  y1 = y;
  for (j=0; j<2; j++) {
    y = y1;
    for (i=0; i<12; i++) {
      x = x1;
      create_btn10_hstring(
        4, x, y, w1, h1, hex, size, &w_thr[j][i], &thr_dec_msk[i][0], &THR_BTN[j][i][0]);
      x = x + 4 * w1;
      create_btn_hstring(
        1, x, y, w2, h1, _05, size, &w_thr[j][i], &thr_dec_msk[i][4], &THR_BTN[j][i][4]);
      x = x + w2;
      obj = add_normal_button(x, y, w2, h1, "W", FL_MEDIUM_SIZE);
        fl_set_object_callback(obj, set_threshold_callback, 12*j+i);
      x = x + w2;
      create_dat_box_vstring(
        1, x, y, w3, h1,      size, &r_thr[j][i], &thr_num_msk[i], &THR_BOX[j][i]);
      x = x + w3;
      obj = add_normal_button(x, y, w2, h1, "R", FL_MEDIUM_SIZE);
        fl_set_object_callback(obj, get_threshold_callback, 12*j+i);
      x = x + w2;
      y = y + h1;
    }
    x = x1;
    w = 4 * w1 + w2 + w2;
    obj = add_normal_button(x, y, w, h2, "SET", size);
      fl_set_object_callback(obj, set_all_thresholds_callback, j);
    x = x + w;
    w = w3 + w2;
    obj = add_normal_button(x, y, w, h2, "GET", size);
      fl_set_object_callback(obj, get_all_thresholds_callback, j);
    x = x + w;
    x1 = x + 5;
  }
}

//~----------------------------------------------------------------------------

void set_threshold_callback(FL_OBJECT* obj, long data) {

  int ind = data % 12;
  int jnd = data / 12;
  _r  *r = &r_thr[jnd][ind];
  _ww *w = &w_thr[jnd][ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  AMSW_PATH[0] = map[ind].link << 8 | 0x04;
  AMSW_PATH[1] = 0x0000;
  set_EDR2_DAC(jnd, w->inp[0], &err);

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

//~-------------------------------------------------------------------------------

void get_threshold_callback(FL_OBJECT* obj, long data) {

  int ind = data % 12;
  int jnd = data / 12;
  int16 DAC;
  _r  *r = &r_thr[jnd][ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  
  AMSW_PATH[0] = map[ind].link << 8 | 0x04;
  AMSW_PATH[1] = 0x0000;
  get_EDR2_DAC(jnd, &DAC, &err);

  if (!err) {
#ifdef SIMULATION
    r->rea[0] = w_thr[jnd][ind].wri[0];
    printf("simulated threshold read: %02X %c\n", r->rea[0], 7);
#else
    r->rea[0] = DAC;
#endif
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

//~-------------------------------------------------------------------------------

void set_all_thresholds_callback(FL_OBJECT* obj, long data) {

  int jnd = data;
  int i;
  int16 error = 0x0000;
  _r *s = &r_slave;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<12; i++) {
    int32 m = 1 << map[i].link;
    r_thr[jnd][i].sta = status_NA;
    w_thr[jnd][i].unk = TRUE;
    if (!s->unk && (s->sta == status_OK) && (s->rea[0] & m)) {
      set_threshold_callback(NULL, 12*jnd+i);
      error |= err;
    }
  }

  err = error;
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void get_all_thresholds_callback(FL_OBJECT* obj, long data) {

  int jnd = data;
  int i;
  int16 error = 0x0000;
  _r *s = &r_slave;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<12; i++) {
    int32 m = 1 << map[i].link;
    r_thr[jnd][i].sta = status_NA;
    if (!s->unk && (s->sta == status_OK) && (s->rea[0] & m)) {
      get_threshold_callback(NULL, 12*jnd+i);
      error |= err;
    }
  }

  err = error;
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_threshold_panel(void) {

  int i, j, k;
  
  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      show_btn_string(4, NULL, &THR_BTN[j][i][0]);
      show_btn_string(1, NULL, &THR_BTN[j][i][4]);
    }
  }

//~--- Sivie asked not to set thresholds below 10.0mV

  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      for (k=0; k<5; k++) {
        SW_BTN *B = THR_BTN[j][i][k];
        _ww *d = B->d;
        static float value = 0.0;
        if (!k) value = unpack32(d->inp[B->k], B->m);
        if (value < 10.0) {
          fl_set_object_color(B->obj, FL_RED, 0);
          fl_set_object_lcolor(B->obj, FL_YELLOW);
        }
      }
    }
  }

  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      DAT_BOX *B = THR_BOX[j][i];
      _r *d = B->d;
      char b[80];
      float value = unpack32(d->rea[B->k], B->m) / 2.0;
      sprintf(b, "%6.1f", value);
      B->l = b;
      show_dat_box_string(1, NULL, NULL, &B);
      if (!d->unk && value < 10.0) {
        fl_set_object_color(B->obj, FL_RED, 0);
        fl_set_object_lcolor(B->obj, FL_YELLOW);
      }
    }
  }
}

//~============================================================================
//
//            UPDATE OBJECTS
//
//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_slave_mask_panel();
  update_delay_panel();
  update_threshold_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
