// file UG66.c
//
// UG66 Controller
//
// A.Lebedev Jul-2008...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"UG66"};                               // global variable
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

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~--

SW_BTN  *BUS_BTN[16], *BRD_BTN[16], *ADR_BTN[16][2], *EXT_BTN[16], *DATA_BTN[16][4];
DAT_BOX               *BRD_BOX[16], *ADR_BOX[16][2], *EXT_BOX[16], *DATA_BOX[16][4];
DAT_BOX *CNT_BOX[16][3];

_ww w_wri[16];
_r  r_rea[16];
_r  r_cnt[16];

_b b_bus   =  {1, 0x7, 6};
_b b_brd   =  {0, 0x00700000,  8};
_b b_adr[] = {{0, 0x08000000,  2},
              {0, 0x07800000, 16}};
_b b_ext   =  {0, 0x00070000,  8};
_b b_dat[] = {{0, 0x0000F000, 16},
              {0, 0x00000F00, 16},
              {0, 0x000000F0, 16},
              {0, 0x0000000F, 16}};

_b b_cnt[] = {{0, -1, -1}, 
              {1, -1, -1},
              {2, -1, -1}};

int32 auto_repeat = 0;

//~--

char *hex[] = {"0", "1", "2", "3", "4", "5", "6", "7",
               "8", "9", "A", "B", "C", "D", "E", "F"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_exe_panel(int x0, int y0);
void exe_callback(FL_OBJECT *obj, long data);
void auto_repeat_callback(int tid, void *data);
void clear_callback(FL_OBJECT *obj, long data);
void update_exe_panel(void);

void create_init_panel(int x0, int y0);
void init_callback(FL_OBJECT *obj, long data);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 495, HH = 680;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
//AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_exe_panel(   0, 45);
  create_init_panel(415, 45);

  create_USCM_control_V_panel(415, 550);
  
  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;
  
  for (i=0; i<16; i++) invalidate_ww(first, &w_wri[i], 2, 0);
  for (i=0; i<16; i++) invalidate_r( first, &r_rea[i], 1);
  for (i=0; i<16; i++) invalidate_r( first, &r_cnt[i], 3);

  read_dat_config_file(dat_config_file_name);

  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  
  file = fopen(filename, "r");
  if (file) {
    int i;
    for (i=0; i<16; i++) fread(w_wri[i].inp, 4, 2, file);
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
    int i;
    for (i=0; i<16; i++) fwrite(w_wri[i].inp, 4, 2, file);
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
//                  INIT PANEL
//
//~============================================================================

void create_init_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 35, w2 = 35, h1 = 20;

  w = 5 + w1 + w2 + 5;
  h = 5 + 6 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  for (i=0; i<6; i++) {
    char b[80];
    sprintf(b, "Bus %d", i);
    add_frame_box(x, y, w1, h1, b, 'S');
    y = y + h1;
  }
  x = x + w1;
  
  y = y0 + 5;
  for (i=0; i<6; i++) {
    obj = add_normal_button(x, y, w2, h1, "INIT", 'N');
      fl_set_object_callback(obj, init_callback, i);
    y = y + h1;
  }
}

//~----------------------------------------------------------------------------

void init_callback(FL_OBJECT *obj, long data) {

  int8 bus = data;
  
  msg = -1;
  update_objects();

  chk = 0;
  
  bus = (bus << 4) | bus;
  init_LeCroy_bus(bus, &err);

  msg = 1;
  update_objects();
}

//~============================================================================

void create_exe_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 35, w3 = 60, h1 = 20, h2 = 15;
  
  w = 5 + 2 * w1 + 5 + 3 * 2 * w1 + 5 + 4 * w1 + w2 + 3 * w3 + 5;
  h = 5 + h1 + 16 * (2 * h2 + 5) - 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, 2*w1,  h1, "Bus",  'S');
  x = x + 2 * w1 + 5;
  add_frame_box(x, y, 2*w1,  h1, "Brd",  'S');
  x = x + 2 * w1;
  add_frame_box(x, y, 2*w1,  h1, "Adr",  'S');
  x = x + 2 * w1;
  add_frame_box(x, y, 2*w1,  h1, "Ext",  'S');
  x = x + 2 * w1 + 5;
  add_frame_box(x, y, 4*w1,  h1, "Data", 'S');
  x = x + 4 * w1;
  add_frame_box(x, y,   w2,  h1, "Cmd",  'S');
  x = x + w2;
  add_frame_box(x, y,   w3,  h1, "Total",  'S');
  x = x + w3;
  add_frame_box(x, y,   w3,  h1, "Errors", 'S');
  x = x + w3;
  add_frame_box(x, y,   w3,  h1, "Rate, %",'S');
  y = y + h1;

  for (i=0; i<16; i++) {
    x = x0 + 5;
    create_btn_hstring(
      1, x, y,    2*w1, 2*h2, hex, 'N', &w_wri[i], &b_bus, &BUS_BTN[i]);
    x = x + 2 * w1 + 5;
    create_btn_hstring(
      1, x, y,    2*w1,   h2, hex, 'N', &w_wri[i], &b_brd, &BRD_BTN[i]);
    create_dat_box_hstring(
      1, x, y+h2, 2*w1,   h2,      'N', &r_rea[i], &b_brd, &BRD_BOX[i]);
    x = x + 2 * w1;
    create_btn_hstring(
      2, x, y,      w1,   h2, hex, 'N', &w_wri[i],  b_adr, &ADR_BTN[i][0]);
    create_dat_box_hstring(
      2, x, y+h2,   w1,   h2,      'N', &r_rea[i],  b_adr, &ADR_BOX[i][0]);
    x = x + 2 * w1;
    create_btn_hstring(
      1, x, y,    2*w1,   h2, hex, 'N', &w_wri[i], &b_ext, &EXT_BTN[i]);
    create_dat_box_hstring(
      1, x, y+h2, 2*w1,   h2,      'N', &r_rea[i], &b_ext, &EXT_BOX[i]);
    x = x + 2 * w1 + 5;
    create_btn_hstring(
      4, x, y,      w1,   h2, hex, 'N', &w_wri[i],  b_dat, &DATA_BTN[i][0]);
    create_dat_box_hstring(
      4, x, y+h2,   w1,   h2,      'N', &r_rea[i],  b_dat, &DATA_BOX[i][0]);
    x = x + 4 * w1;
    obj = add_normal_button(x, y, w2, 2*h2, "EXE", 'N');
      fl_set_object_callback(obj, exe_callback, i);
    x = x + w2;
    create_dat_box_hstring(
      3, x,    y,   w3, 2*h2,      'N', &r_cnt[i],  b_cnt, &CNT_BOX[i][0]);
    y = y + 2 * h2 + 5;
  }
  
  x = x0 + 5;
  y = y - 5;
  
  add_frame_box(x, y, 2*w1,  h1, "Bus",  'S');
  x = x + 2 * w1 + 5;
  add_frame_box(x, y, 2*w1,  h1, "Brd",  'S');
  x = x + 2 * w1;
  add_frame_box(x, y, 2*w1,  h1, "Adr",  'S');
  x = x + 2 * w1;
  add_frame_box(x, y, 2*w1,  h1, "Ext",  'S');
  x = x + 2 * w1 + 5;
  add_frame_box(x, y, 4*w1,  h1, "Data", 'S');
  x = x + 4 * w1;
  add_frame_box(x, y,   w2,  h1, "Cmd",  'S');
  x = x + w2;
  obj = add_normal_button(x, y, 3*w3, h1, "CLEAR", 'N');
    fl_set_object_callback(obj, clear_callback, 0);

  clear_callback(NULL, 0);
}

//~----------------------------------------------------------------------------

void exe_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int8 bus;
  int r_w;
  int16 brd, adr, ext, wri, rea;
  int32 wri32, rea32, m;
  int p;

  _ww *w = &w_wri[ind];
  _r  *r = &r_rea[ind];
  _r  *c = &r_cnt[ind];

  if (obj) {
    if (auto_repeat & (1 << ind)) {
      auto_repeat &= ~(1 << ind);
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        if (!auto_repeat) fl_add_timeout(0, auto_repeat_callback, NULL);
        auto_repeat |=  (1 << ind);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!(auto_repeat & (1 << ind))) {
    msg = -1;
    update_objects();
  }

  bus = unpack32(w->inp[b_bus.ind], b_bus.msk);
  bus = (bus << 4) | bus;

  brd = unpack32(w->inp[b_brd.ind], b_brd.msk);
  adr = unpack32(w->inp[b_adr[0].ind], b_adr[0].msk | b_adr[1].msk);
  ext = unpack32(w->inp[b_ext.ind], b_ext.msk);
  m   = b_dat[0].msk | b_dat[1].msk | b_dat[2].msk | b_dat[3].msk;
  wri = unpack32(w->inp[b_dat[0].ind], m);

  r_w = READ;
  wri32 =        0x80000000;
  pack32(&wri32, 0x00080000, r_w);
  pack32(&wri32, 0x0F800000, swapbits(adr,  5));
  pack32(&wri32, 0x00700000, swapbits(brd,  3));
  pack32(&wri32, 0x00070000, swapbits(ext,  3));
  pack32(&wri32, 0x0000FFFF, swapbits(wri, 16));
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri32 & m) p ^= 1;
  if (p) wri32 ^= 0x40000000;

  r->was[0] = r->rea[0];

  exec_LeCroy_cmd(1, &bus, &wri32, &rea32, &chk, &err);

  brd = swapbits(unpack32(rea32, 0x00700000),  3);
  adr = swapbits(unpack32(rea32, 0x0F800000),  5);
  ext = swapbits(unpack32(rea32, 0x00070000),  3);
  rea = swapbits(unpack32(rea32, 0x0000FFFF), 16);
  r->rea[0] = 0x00000000;
  pack32(&r->rea[0], 0x0F800000, adr);
  pack32(&r->rea[0], 0x00700000, brd);
  pack32(&r->rea[0], 0x00070000, ext);
  pack32(&r->rea[0], 0x0000FFFF, rea);

  if (!err) {
    c->rea[0]++;
    if (chk) c->rea[1]++;
  }

  if (!err && !chk) {
    w->wri[0] = w->inp[0];
    w->wri[1] = w->inp[1];
    w->unk = FALSE;
    if (r->unk) r->was[0] = r->rea[0];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    w->unk = TRUE;
    r->sta = status_KO;
  }

  if (auto_repeat & (1 << ind)) msg = -3;
  else                          msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void auto_repeat_callback(int tid, void *data) {

  int i;
//int period = 1000;  // in ms
  int period =    1;  // in ms

  for (i=0; i<16; i++) {
    if (auto_repeat & (1 << i)) {
      if (P > 1) {
        static struct timeval now, then;
        gettimeofday(&now, NULL);
        printf("auto_repeat: i=%2d tid=%d dt=%f\n", i, tid, delta_t(&now, &then));
        memcpy(&then, &now, sizeof(now));
      }
      exe_callback(NULL, i);
    }
  }
  
  if (auto_repeat) {
    fl_add_timeout(period, auto_repeat_callback, NULL);
  }
}

//~----------------------------------------------------------------------------

void clear_callback(FL_OBJECT *obj, long data) {

  int i;
  
  for (i=0; i<16; i++) {
    _r  *c = &r_cnt[i];
    c->unk = FALSE;
    c->sta = status_OK;
    c->rea[0] = 0;
    c->rea[1] = 0;
  }
  
  if (obj) update_objects();  
}

//~----------------------------------------------------------------------------

void update_exe_panel(void) {

  int i;
  
  for (i=0; i<16; i++) {
    _r *c = &r_cnt[i];
    char b[80];
    show_btn_string(1, NULL, &BUS_BTN[i]);
    show_btn_string(1, NULL, &BRD_BTN[i]);
    show_btn_string(2, NULL, &ADR_BTN[i][0]);
    show_btn_string(1, NULL, &EXT_BTN[i]);
    show_btn_string(4, NULL, &DATA_BTN[i][0]);
    show_dat_box_string(1,  hex,  NULL, &BRD_BOX[i]);
    show_dat_box_string(2,  hex,  NULL, &ADR_BOX[i][0]);
    show_dat_box_string(1,  hex,  NULL, &EXT_BOX[i]);
    show_dat_box_string(4,  hex,  NULL, &DATA_BOX[i][0]);
    show_dat_box_string(2, NULL, "%6d", &CNT_BOX[i][0]);
    if (c->rea[0]) sprintf(b, "%.1f%%", 100.0*c->rea[1]/c->rea[0]);
    else           sprintf(b, " ");
    show_dat_box_string(1, NULL,     b, &CNT_BOX[i][2]);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_exe_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
