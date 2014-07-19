// file JINJ-1.c
//
// JINJ-1 "Pro" Controller
//
// A.Lebedev May-2008...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"JINJ - 1  Pro  Controller"};          // global variable
char *date  = {"13-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"
int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH;            // global variable
int16 NODE_ADR;             // global variable
int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~-- Boot JINF Panel

SW_BTN *BOOT_ADR_BTN[4];
_w w_boot_adr;

//~-- Load Boot Panel

FL_OBJECT *FILE_HEADER_BOX[8];
_r r_file_header;

//~-- Ping JINF Panel

FL_OBJECT *PING_STA_BOX[1];
_r r_ping_sta;

//~-- Get Event Panel

FL_OBJECT *GET_EVENT_BOX[1];
_r r_get_event;
bool auto_get_event = FALSE;

//~-- CDDC Control Panel

SW_BTN  *SLAVE_MASK_BTN[24];
DAT_BOX *SLAVE_MASK_BOX[24];
_ww w_slave_mask;
_r  r_slave_mask;

SW_BTN  *SSF_BTN[25];
DAT_BOX *SSF_BOX[25];
_ww w_SSF[25];
_r  r_SSF[25];

SW_BTN  *PROC_MODE_BTN[25];
DAT_BOX *PROC_MODE_BOX[25];
_ww w_mode[25];
_r  r_mode[25];

SW_BTN  *DELAY_BTN[25];
DAT_BOX *DELAY_BOX[25];
_ww w_delay[25];
_r  r_delay[25];

DAT_BOX *FIFO_BOX[25];
_r  r_fifo[25];

DAT_BOX *STATUS_BOX[25][18];
_r  r_status[25][10];

//~--

char *hex[16] = {
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *got[2] = {"Empty", "Got It"};
char *fai[2] = {"FAIL", "O.K."};
char *__a[2] = {"-", "A"};
char *_up[2] = {"@2", "@8"};
char *mod[4] = {"-", "R", "C", "M"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
void update_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_boot_panel(int x0, int y0);
void boot_callback(FL_OBJECT* obj, long data);
void update_boot_panel(void);

void create_load_boot_panel(int x0, int y0);
void load_boot_callback(FL_OBJECT* obj, long data);
void read_FLASH_summary_callback(FL_OBJECT* obj, long data);
void update_load_boot_panel(void);

void create_test_slaves_panel(int x0, int y0);
void test_slaves_callback(FL_OBJECT* obj, long data);

void create_ping_panel(int x0, int y0);
void ping_callback(FL_OBJECT* obj, long data);
void update_ping_panel(void);

void create_get_event_panel(int x0, int y0);
void get_event_callback(FL_OBJECT* obj, long data);
void auto_get_event_callback(int tid, void *data);
void update_get_event_panel(void);

//~--

void create_CDDC_control_panel(int x0, int y0);

void set_slave_mask_callback(FL_OBJECT* obj, long data);
void write_slave_mask_callback(FL_OBJECT* obj, long data);
void read_slave_mask_callback(FL_OBJECT* obj, long data);

void set_SSF_callback(FL_OBJECT* obj, long data);
void write_SSF_callback(FL_OBJECT* obj, long data);
void read_SSF_callback(FL_OBJECT* obj, long data);

void set_proc_mode_callback(FL_OBJECT* obj, long data);
void write_proc_mode_callback(FL_OBJECT* obj, long data);
void read_proc_mode_callback(FL_OBJECT* obj, long data);

void set_delay_callback(FL_OBJECT* obj, long data);
void write_delay_callback(FL_OBJECT* obj, long data);
void read_delay_callback(FL_OBJECT* obj, long data);

void read_fifo_callback(FL_OBJECT* obj, long data);
void reset_fifo_callback(FL_OBJECT* obj, long data);

void read_status_callback(FL_OBJECT* obj, long data);

void write_all_callback(FL_OBJECT* obj, long data);
void read_all_callback(FL_OBJECT* obj, long data);

void update_CDDC_control_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 1085, HH = 720;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, NULL);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_boot_panel(0, 100);
  create_load_boot_panel(70, 45);
  create_test_slaves_panel(420, 45);
  create_ping_panel(     235,  85);
  create_get_event_panel(535,  85);

  create_CDDC_control_panel(0, 140);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  invalidate_w(&w_boot_adr, 1, 0x0000);
  invalidate_r(first, &r_file_header, 8);
  invalidate_r(first, &r_ping_sta, 1);
  invalidate_r(first, &r_get_event, 1);

  invalidate_ww(first, &w_slave_mask, 1, 0);
  invalidate_r( first, &r_slave_mask, 1);

  for (i=0; i<25; i++) {
    invalidate_ww(first, &w_SSF[i], 1, 0);
    invalidate_r( first, &r_SSF[i], 1);
    invalidate_ww(first, &w_mode[i], 1, 0);
    invalidate_r( first, &r_mode[i], 1);
    invalidate_ww(first, &w_delay[i], 1, 0);
    invalidate_r( first, &r_delay[i], 1);
    invalidate_r( first, &r_fifo[i], 1);
    invalidate_r( first, &r_status[i][0], 10);
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

//~----------------------------------------------------------------------------

void update_callback(int32 data) {

  update_objects();
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  int i;
  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&w_boot_adr.inp,      4, 1, file);
    fread(w_slave_mask.inp,     4, 1, file);
    for (i=0; i<25; i++) {
      fread(w_SSF[i].inp,       4, 1, file);
      fread(w_mode[i].inp,      4, 1, file);
      fread(w_delay[i].inp,     4, 1, file);
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  int i;
  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(&w_boot_adr.inp,      4, 1, file);
    fwrite(w_slave_mask.inp,     4, 1, file);
    for (i=0; i<25; i++) {
      fwrite(w_SSF[i].inp,       4, 1, file);
      fwrite(w_mode[i].inp,      4, 1, file);
      fwrite(w_delay[i].inp,     4, 1, file);
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//
//            BOOT JINF PANEL
//
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
//
//            LOAD BOOT PANEL
//
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
//
//            TEST JINF SLAVES PANEL
//
//
//~============================================================================

void create_test_slaves_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, h1 = 30;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;

  obj = add_normal_button(x, y, w1, h1, "SLAVE\nTEST", 'N');
    fl_set_object_callback(obj, test_slaves_callback, 0);
}

//~----------------------------------------------------------------------------

void test_slaves_callback(FL_OBJECT* obj, long data) {

  msg = -1;
  update_objects();

  test_JINF_slaves(&err);

  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//
//            PING JINF PANEL
//
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
  
  obj = add_normal_button(x, y, w1, h1, "PING", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, ping_callback, 0);
  y = y + h1;

  create_data_box_hstring(1, x, y, w1, h2, FL_NORMAL_SIZE, &PING_STA_BOX[0]);
}

//~----------------------------------------------------------------------------

void ping_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ping_sta;
  int n, nn;
  int16 dat[2*8192], datdat[2*8192+10];
  int i;
    
  msg = -1;
  update_objects();

//n = 8000;
  n = 10;
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
//
//            GET EVENT PANEL
//
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
  
  if (P) {
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
//
//            CDDC CONTROL PANEL
//
//
//~============================================================================

FL_OBJECT *MARKER[10][24];

_b m_24[24];






void create_markers(int x, int y) {

  FL_OBJECT *obj;
  static int k = 0;
  int i, j;
  
  if (!k) for (j=0; j<10; j++) for (i=0; i<24; i++) MARKER[j][i] = NULL;
  
  for (i=0; i<24; i++) {
    MARKER[k][i] =
    obj = fl_add_box(FL_FLAT_BOX, x, y+5+15*i+i/4*5-5, 5, 15, "");
//    fl_set_object_color(obj,FL_RED, FL_RED);
  }
  k = k + 1;
}





void show_markers(void) {

  int i, j;

  for (j=0; j<10; j++) {
    for (i=0; i<24; i++) {
      FL_OBJECT *obj = MARKER[j][i];
      int32 m = w_slave_mask.inp[0] & m_24[i].msk;
      if (!obj) continue;
      if (m) fl_set_object_color(obj, FL_WHITE, 0);
      else   fl_set_object_color(obj, FL_COL1, 0);
    }
  }
}








_b m_mode  = {0, 0x0003,   4};
_b m_delay = {0, 0x00FF, 256};
_b m_fifo  = {0, 0xFFFF,  -1};

char *lnk_num[24] = {
  "00", "01", "02", "03", "04", "05", "06", "07", 
  "08", "09", "10", "11", "12", "13", "14", "15",
  "16", "17", "18", "19", "20", "21", "22", "23"};

char *lnk_nam[24] = {
  "T2",  "T3",  "U1",  "T0",  "S1A", "S1B", "S0A", "S0B",
  "U0",  "T1",  "R0",  "R1",  "E0",  "E1",  "JA",  "JB",
  "T4",  "T5",  "S2A", "S2B", "S3A", "S3B", "T6",  "T7"};

int16 lnk_adr[24] = {
  0x0AE, 0x0B2, 0x0CA, 0x0A6, 0x10E, 0x10F, 0x10A, 0x10B,
  0x0C6, 0x0AA, 0x09E, 0x0A2, 0x096, 0x09A, 0x088, 0x089,
  0x0B6, 0x0BA, 0x112, 0x113, 0x116, 0x117, 0x0BE, 0x0C2};

static char *t0[16] = {
  "ROM", "DAQ", "?",    "?",   "?",   "?",   "?",    "?",
  "?",   "?",   "?",    "?",   "?",   "?",   "?",    "?"  };
static char *t1[16] = {
  "?",   "CDP", "CDDC", "?",   "?",   "?",   "?",    "?", 
  "?",   "?",   "?",    "?",   "?",   "?",   "?",    "?"  };
static char *t2[16] = {
  "CDDC","TDR", "UDR",  "RDR", "EDR", "SDR", "JLV1", "ETRG",
  "?",   "?",   "?",    "?",   "?",   "?",   "?",    "?"  };

static struct {
  int   wa;
  int   ha;
  char *ta;
  int   wb;
  int   hb;
  char *tb;
  _b    m;
  char **txt;
  char *fmt;
} *q, Q[18] = {
//  wa ha   ta             wb hb   tb ---------- m ---------- txt    fmt
  { 35, 2, "Vers",          0, 0,   0, {1-1, 0x0000FFFF, -1}, NULL, "%04X"},
  { 35, 2, "Pgm" ,          0, 0,   0, {2-1, 0x0000F000, 16}, t0,   "%s"  },
  { 35, 2, "Node",          0, 0,   0, {2-1, 0x00000F00, 16}, t1,   "%s"  },
  { 35, 2, "Node\nType",    0, 0,   0, {2-1, 0x000000F0, 16}, t2,   "%s"  },
  { 35, 2, "Ports\nto M",   0, 0,   0, {2-1, 0x0000000F, 16}, NULL, "%d"  },
  { 35, 2, "SubD\nVers",    0, 0,   0, {3-1, 0x0000FFFF, 16}, NULL, "%04X"},
  {105, 1, "Errors",       15, 1, "E", {6-1, 0x00004000,  2}, NULL, "%d"  },
  {  0, 1,  NULL,          15, 1, "A", {6-1, 0x00002000,  2}, NULL, "%d"  },
  {  0, 1,  NULL,          15, 1, "F", {6-1, 0x00001000,  2}, NULL, "%d"  },
  {  0, 1,  NULL,          15, 1, "B", {6-1, 0x00000800,  2}, NULL, "%d"  },
  {  0, 1,  NULL,          15, 1, "P", {6-1, 0x00000400,  2}, NULL, "%d"  },
  {  0, 1,  NULL,          15, 1, "D", {6-1, 0x00000200,  2}, NULL, "%d"  },
  {  0, 1,  NULL,          15, 1, "T", {6-1, 0x00000100,  2}, NULL, "%d"  },
  { 50, 2, "Time\nsec",     0, 0,   0, {4-1, 0xFFFFFFFF, -1}, NULL, "%d"  },
  { 40, 2, "Last\nEvent#",  0, 0,   0, {7-1, 0x0000FFFF, -1}, NULL, "%d"  },
  { 40, 2, "Bldg.\nErrors", 0, 0,   0, {8-1, 0x0000FFFF, -1}, NULL, "%d"  },
  { 40, 2, "AMSw\nErrors",  0, 0,   0, {9-1, 0x0000FFFF, -1}, NULL, "%d"  },
  { 40, 2, "Flash\nErrors", 0, 0,   0,{10-1, 0x0000FFFF, -1}, NULL, "%d"  }};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_CDDC_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w;

{//~--- Panel ---

  int w1 = 1085, h1 = 550;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");
}

{//~--- Link List (left) ---

  int w1 = 20, w2 = 30, h1 = 45, h2 = 15, h3 = 70;

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2,   h1, "Link", 'S');
  y = y + h1 + 5;
  add_frame_box(x, y, w1+w2, 2*h2, "JINJ", 'S');
  y = y + 2 * h2 + 5;
  
  create_frame_box_vstring(24, x,    y, w1, h2, lnk_num, 'S'+400, 'C');
  create_frame_box_vstring(24, x+w1, y, w2, h2, lnk_nam, 'S'+400, 'C');
  y = y + 24 * h2 + 5 * 5;

  add_frame_box(x, y, w1+w2, h3, "", 'S');
  x = x + w1 + w2;
}

{//~--- Slave Mask and Global READ/WRITE Buttons ---

  int i;
  int w1 = 30, w2 = 15, h1 = 45, h2 = 15, h3 = 20;
  int wb = 910, hb = 25;

  y = y0 + 5;
  w = w1 + w2;
  add_frame_box(x, y, w,   h1, "Slave\nMask", 'S');
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w, h1, "");
   fl_set_object_callback(obj, set_slave_mask_callback, 0);
  y = y + h1 + 5;
  create_markers(x+w, y+2*h2+5);
  add_frame_box(x, y, w, 2*h2, "N/A", 'S');
  y = y + 2 * h2 + 5;
  
  for (i=0; i<24; i++) {
    m_24[i].ind = 0;
    m_24[i].msk = 1 << i;
    m_24[i].n   = 2;
  }
  create_btn_vstring(
    24, x,    y, w1, h2, __a, 'N'+400, &w_slave_mask, m_24, SLAVE_MASK_BTN);
  create_dat_box_vstring(
    24, x+w1, y, w2, h2,      'N'+400, &r_slave_mask, m_24, SLAVE_MASK_BOX);
  y = y + 24 * h2 + 5 * 5;

  obj = add_normal_button(x,    y, w1, h3, "W", 'N');
    fl_set_object_callback(obj, write_slave_mask_callback, 0);
  obj = add_normal_button(x+w1, y, w2, h3, "R", 'N');
    fl_set_object_callback(obj, read_slave_mask_callback, 0);
  y = y + h3;
  
  obj = add_normal_button(x,    y, wb, hb, "WRITE AND / OR  CLEAR  ALL & EVERYTHING", 'N');
    fl_set_object_callback(obj, write_all_callback, -1);
  y = y + hb;
  obj = add_normal_button(x,    y, wb, hb, "READ  ALL & EVERYTHING", 'N');
    fl_set_object_callback(obj, read_all_callback, -1);
  x = x + w + 5;
}

{//~--- SSF ---

  int i;
  int x1 = x;
  int w1 = 15, h1 = 45, h2 = 15, h3 = 20;

  y = y0 + 5;
  w = 4 * w1;
  add_frame_box(x, y, w, h1, "SSF", 'S');
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w, h1, "");
   fl_set_object_callback(obj, set_SSF_callback, 0);
  y = y + h1 + 5;
  create_markers(x+w, y+2*h2+5);
  
  for (i=0; i<25; i++) {
    SSF_BTN[i] = NULL;
    SSF_BOX[i] = NULL;
    x = x1;
    if (i == 0) {
      add_frame_box(x, y, w, 2*h2,  "N/A", 'S');
      y = y + 2 * h2 + 5;
    }
    else if (lnk_nam[i-1][0] == 'J' || lnk_nam[i-1][0] == 'S') {
      add_frame_box(x, y, w,   h2, "N/A", 'S');
      y = y + h2;
    }
    else {
      create_btn_vstring(
        1, x, y, w1, h2, _up, 'N', &w_SSF[i], m_24, &SSF_BTN[i]);
      x = x + w1;
      obj = add_normal_button(x,    y, w1, h2, "W", 'N');
        fl_set_object_callback(obj, write_SSF_callback, i);
      x = x + w1;
      create_dat_box_vstring(
        1, x, y, w1, h2,      'N', &r_SSF[i], m_24, &SSF_BOX[i]);
      x = x + w1;
      obj = add_normal_button(x,    y, w1, h2, "R", 'N');
        fl_set_object_callback(obj, read_SSF_callback, i);
      x = x + w1;
      y = y + h2;
    }
    if (i == 4 || i == 8 || i == 12 || i == 16 || i == 20) y = y + 5;
  }

  x = x1;
  w = 2 * w1;
  obj = add_normal_button(x, y, w, h3, "W", 'N');
    fl_set_object_callback(obj, write_SSF_callback, -1);
  x = x + w;
  obj = add_normal_button(x, y, w, h3, "R", 'N');
    fl_set_object_callback(obj, read_SSF_callback, -1);
  x = x1 + 2 * w + 5;
}

{//~--- Processing Mode ---

  int i;
  int x1 = x;
  int w1 = 15, h1 = 45, h2 = 15, h3 = 20;

  y = y0 + 5;
  w = 4 * w1;
  add_frame_box(x, y, w, h1, "Processing\nMode", 'S');
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w, h1, "");
   fl_set_object_callback(obj, set_proc_mode_callback, 0);
  y = y + h1 + 5;
  create_markers(x+w, y+2*h2+5);
  
  for (i=0; i<25; i++) {
    int h = i ? h2 : 2 * h2;
    x = x1;
    create_btn_vstring(
      1, x, y, w1, h, mod, 'N', &w_mode[i], &m_mode, &PROC_MODE_BTN[i]);
    x = x + w1;
    obj = add_normal_button(x,    y, w1, h, "W", 'N');
      fl_set_object_callback(obj, write_proc_mode_callback, i);
    x = x + w1;
    create_dat_box_vstring(
      1, x, y, w1, h,      'N', &r_mode[i], &m_mode, &PROC_MODE_BOX[i]);
    x = x + w1;
    obj = add_normal_button(x,    y, w1, h, "R", 'N');
      fl_set_object_callback(obj, read_proc_mode_callback, i);
    x = x + w1;
    y = y + h;
    if (i == 0 || i == 4 || i == 8 || i == 12 || i == 16 || i == 20) y = y + 5;
  }

  x = x1;
  w = 2 * w1;
  obj = add_normal_button(x, y, w, h3, "W", 'N');
    fl_set_object_callback(obj, write_proc_mode_callback, -1);
  x = x + w;
  obj = add_normal_button(x, y, w, h3, "R", 'N');
    fl_set_object_callback(obj, read_proc_mode_callback, -1);
  x = x1 + 2 * w + 5;
}

{//~--- Delay ---

  int i;
  int x1 = x;
  int w1 = 40, w2 = 15, h1 = 45, h2 = 15, h3 = 20;

  y = y0 + 5;
  w = 2 * (w1 + w2);
  add_frame_box(x, y, w, h1, "Delay, us", 'S');
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w, h1, "");
   fl_set_object_callback(obj, set_delay_callback, 0);
  y = y + h1 + 5;
  create_markers(x+w, y+2*h2+5);
  
  for (i=0; i<25; i++) {
    int h = i ? h2 : 2 * h2;
    DELAY_BTN[i] = NULL;
    DELAY_BOX[i] = NULL;
    x = x1;
    if (i == 0) {
      add_frame_box(x, y, w, h,  "N/A", 'S');
      y = y + h + 5;
    }
    else if (lnk_nam[i-1][0] == 'J' || lnk_nam[i-1][0] == 'S') {
      add_frame_box(x, y, w, h, "N/A", 'S');
      y = y + h;
    }
    else {
      create_btn_vstring(
        1, x, y, w1, h, NULL, 'N', &w_delay[i], &m_delay, &DELAY_BTN[i]);
      add_hidden_btn_zones(DELAY_BTN[i], 3, 1, 10, 50);
      x = x + w1;
      obj = add_normal_button(x, y, w2, h, "W", 'N');
        fl_set_object_callback(obj, write_delay_callback, i);
      x = x + w2;
      create_dat_box_vstring(
        1, x, y, w1, h,       'N', &r_delay[i], &m_delay, &DELAY_BOX[i]);
      x = x + w1;
      obj = add_normal_button(x, y, w2, h, "R", 'N');
        fl_set_object_callback(obj, read_delay_callback, i);
      x = x + w2;
      y = y + h;
    }
    if (i == 4 || i == 8 || i == 12 || i == 16 || i == 20) y = y + 5;
  }

  x = x1;
  w = w1 + w2;
  obj = add_normal_button(x, y, w, h3, "W", 'N');
    fl_set_object_callback(obj, write_delay_callback, -1);
  x = x + w1 + w2;
  obj = add_normal_button(x, y, w, h3, "R", 'N');
    fl_set_object_callback(obj, read_delay_callback, -1);
  x = x + w + 5;
}

{//~--- Last Event Processed (FIFO) ---

  int i;
  int x1 = x;
  int w1 = 40, w2 = 15, h1 = 45, h2 = 15, h3 = 20;

  y = y0 + 5;
  w = w1 + 2 * w2;
  add_frame_box(x, y, w, h1, "Last Event\nProcessed\n(FIFO)", 'S');
  y = y + h1 + 5;
  create_markers(x+w, y+2*h2+5);
  
  for (i=0; i<25; i++) {
    int h = i ? h2 : 2 * h2;
    x = x1;
    create_dat_box_vstring(
      1, x, y, w1, h,       'N', &r_fifo[i], &m_fifo, &FIFO_BOX[i]);
    x = x + w1;
    obj = add_normal_button(x, y, w2, h, "R", 'N');
      fl_set_object_callback(obj, read_fifo_callback, i);
    x = x + w2;
    obj = add_normal_button(x, y, w2, h, "C", 'N');
      fl_set_object_callback(obj, reset_fifo_callback, i);
    x = x + w2;
    y = y + h;
    if (i == 0 || i == 4 || i == 8 || i == 12 || i == 16 || i == 20) y = y + 5;
  }

  x = x1;
  w = w1 + w2;
  obj = add_normal_button(x, y, w, h3, "R", 'N');
    fl_set_object_callback(obj, read_fifo_callback, -1);
  x = x + w;
  obj = add_normal_button(x, y, w2,    h3, "C", 'N');
    fl_set_object_callback(obj, reset_fifo_callback, -1);
  x = x + w2 + 5;
}

{//~--- CDDC Status ---

  int i, j;
  int x1 = x, y1;
  int h1 = 15, h2 = 15, h3 = 20, w1 = 15;
  int w = 0;
  int n = sizeof(Q) / sizeof(Q[0]);
  
  y = y0 + 5;
  for (j=0; j<n; j++) w = w + Q[j].wa;
  w = w + w1;
  add_frame_box(x, y, w, h1, "Status", 'S');
  y = y + h1;
  for (j=0; j<n; j++) {
    q = &Q[j];
    if (q->wa) {
      add_frame_box(x, y,          q->wa, h1*q->ha, q->ta, 'S');
      if (!q->wb) x = x + q->wa;
    }
    if (q->wb) {
      add_frame_box(x, y+h1*q->ha, q->wb, h1*q->hb, q->tb, 'S');
      x = x + q->wb;
    }
  }
  add_frame_box(x, y, w1, 2*h1, "", 'S');
  y = y + 2 * h1 + 5;
  create_markers(x+w1, y+2*h1+5);
  y1 = y;

  x = x1;
  for (j=0; j<n; j++) {
    int ww;
    q = &Q[j];
    ww = q->wb ? q->wb : q->wa;
    y = y1;
    for (i=0; i<25; i++) {
      int h = i ? h2 : 2 * h2;
      create_dat_box_vstring(
        1, x, y, ww, h, 'N', &r_status[i][0], &q->m, &STATUS_BOX[i][j]);
      if (j == n-1) {
        obj = add_normal_button(x+ww, y, w1, h, "R", 'N');
          fl_set_object_callback(obj, read_status_callback, i);
      }
      y = y + h;
      if (i == 0 || i == 4 || i == 8 || i == 12 || i == 16 || i == 20) y = y + 5;
    }
    x = x + ww;
  }

  x = x1;
  obj = add_normal_button(x, y, w, h3, "R", 'N');
    fl_set_object_callback(obj, read_status_callback, -1);
  x = x + w + 5;
}

{//~--- Link List (right) ---

  int w1 = 20, w2 = 30, h1 = 45, h2 = 15, h3 = 70;

  y = y0 + 5;
  add_frame_box(x, y, w1+w2,   h1, "Link", 'S');
  y = y + h1 + 5;
  add_frame_box(x, y, w1+w2, 2*h2, "JINJ", 'S');
  y = y + 2 * h2 + 5;
  
  create_frame_box_vstring(24, x,    y, w1, h2, lnk_num, 'S'+400, 'C');
  create_frame_box_vstring(24, x+w1, y, w2, h2, lnk_nam, 'S'+400, 'C');
  y = y + 24 * h2 + 5 * 5;

  add_frame_box(x, y, w1+w2, h3, "", 'S');
  x = x + w1 + w2;
}

{//~--- "Semi-Global" READ/WRITE Buttons ---

  int i;
  int w1 = 40, w2 = 20, h1 = 45, h2 = 15, h3 = 70;

  y = y0 + 5;
  add_frame_box(x,    y, w1, h1, "", 'S');
  add_frame_box(x+w1, y, w2, h1, "", 'S');
  y = y + h1 + 5;
  
  for (i=0; i<25; i++) {
    int h = i ? h2 : 2 * h2;
    obj = add_normal_button(x,    y, w1, h, "W / C", 'N');
      fl_set_object_callback(obj, write_all_callback, i);
    obj = add_normal_button(x+w1, y, w2, h, "R",     'N');
      fl_set_object_callback(obj, read_all_callback, i);
    y = y + h;
    if (i == 0 || i == 4 || i == 8 || i == 12 || i == 16 || i == 20) y = y + 5;
  }

  add_frame_box(x,    y, w1, h3, "", 'S');
  add_frame_box(x+w1, y, w2, h3, "", 'S');
  x = x + w1 + w2;
}

}

//~----------------------------------------------------------------------------

void set_slave_mask_callback(FL_OBJECT* obj, long data) {

  static int set = 0;
  _ww *w = &w_slave_mask;
  _r  *r = &r_slave_mask;

  switch (set) {
    case 0: w->inp[0] =  0x000000;  break;
    case 1: w->inp[0] =  0xFFFFFF;  break;
    case 2: w->inp[0] =  r->rea[0]; break;
    case 3: w->inp[0] = ~r->rea[0]; break;
  }
  
  set = MOD(set + 1, 4);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_slave_mask_callback(FL_OBJECT* obj, long data) {

  _ww *w = &w_slave_mask;
  _r  *r = &r_slave_mask;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  JINF_PATH = 0x0000;
  set_JINF_slave_mask(0, w_slave_mask.inp[0], &err);

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

void read_slave_mask_callback(FL_OBJECT* obj, long data) {

  int32 rea[16];
  _r *r = &r_slave_mask;

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];

  JINF_PATH = 0x0000;
  get_JINF_slave_masks(rea, &err);

  if (!err) {
    r->rea[0] = rea[0];
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

void set_SSF_callback(FL_OBJECT* obj, long data) {

  int i;
  static int set = 0;

  for (i=0; i<25; i++) {
    int16 m = m_24[0].msk;
    _ww *w = &w_SSF[i];
    _r  *r = &r_SSF[i];
    switch (set) {
      case 0: w->inp[0] =  0x0000;                 break;
      case 1: w->inp[0] =  0x0001;                 break;
      case 2: w->inp[0] =  unpack16(r->rea[0], m); break;
      case 3: w->inp[0] = ~unpack16(r->rea[0], m); break;
    }
  }
  
  set = MOD(set + 1, 4);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_SSF_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int16 wri;

  for (i=1; i<25; i++) {
    int lnk = i - 1;
    _ww *w = &w_SSF[i];
    _r  *r = &r_SSF[i];

    if (ind != -1 && ind != i) continue;

    if (lnk_nam[lnk][0] == 'J' || lnk_nam[lnk][0] == 'S') continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;
   
    wri = w->inp[0] & m_24[0].msk;

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    set_JINF_SSF(wri, &err);

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
}

//~----------------------------------------------------------------------------

void read_SSF_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int16 rea;

  for (i=1; i<25; i++) {
    int lnk = i - 1;
    _r *r = &r_SSF[i];

    if (ind != -1 && ind != i) continue;

    if (lnk_nam[lnk][0] == 'J' || lnk_nam[lnk][0] == 'S') continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;

    r->was[0] = r->rea[0];

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    get_JINF_SSF(&rea, &err);

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
}

//~----------------------------------------------------------------------------

void set_proc_mode_callback(FL_OBJECT* obj, long data) {

  int i;
  static int set = 0;

  for (i=0; i<25; i++) {
    int16 m = m_mode.msk;
    _ww *w = &w_mode[i];
    _r  *r = &r_mode[i];
    switch (set) {
      case 0: w->inp[0] =  0x0000;                 break;
      case 1: w->inp[0] =  0x0001;                 break;
      case 2: w->inp[0] =  0x0002;                 break;
      case 3: w->inp[0] =  0x0003;                 break;
      case 4: w->inp[0] =  unpack16(r->rea[0], m); break;
    }
  }
  
  set = MOD(set + 1, 5);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_proc_mode_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int16 wri;

  for (i=0; i<25; i++) {
    int lnk = i - 1;
    _ww *w = &w_mode[i];
    _r  *r = &r_mode[i];

    if (ind != -1 && ind != i) continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;
   
    wri = w->inp[0] & m_mode.msk;

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    set_JINF_proc_mode(wri, &err);

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
}

//~----------------------------------------------------------------------------

void read_proc_mode_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int16 rea;

  for (i=0; i<25; i++) {
    int lnk = i - 1;
    _r *r = &r_mode[i];

    if (ind != -1 && ind != i) continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;

    r->was[0] = r->rea[0];

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    get_JINF_proc_mode(&rea, &err);

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
}

//~----------------------------------------------------------------------------

void set_delay_callback(FL_OBJECT* obj, long data) {

  int i;
  static int set = 0;

  for (i=0; i<25; i++) {
    int16 m = m_delay.msk;
    _ww *w = &w_delay[i];
    _r  *r = &r_delay[i];
    switch (set) {
      case 0: w->inp[0] =  0x0000;                 break;
      case 1: w->inp[0] =  unpack16(r->rea[0], m); break;
    }
  }
  
  set = MOD(set + 1, 2);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_delay_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int16 wri;

  for (i=1; i<25; i++) {
    int lnk = i - 1;
    _ww *w = &w_delay[i];
    _r  *r = &r_delay[i];

    if (ind != -1 && ind != i) continue;

    if (lnk_nam[lnk][0] == 'J' || lnk_nam[lnk][0] == 'S') continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;
   
    wri = w->inp[0] & m_delay.msk;

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    set_JINF_delay(wri, &err);

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
}

//~----------------------------------------------------------------------------

void read_delay_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int8 rea;

  for (i=1; i<25; i++) {
    int lnk = i - 1;
    _r *r = &r_delay[i];

    if (ind != -1 && ind != i) continue;

    if (lnk_nam[lnk][0] == 'J' || lnk_nam[lnk][0] == 'S') continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;

    r->was[0] = r->rea[0];

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    get_JINF_delay(&rea, &err);

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
}

//~----------------------------------------------------------------------------

void read_fifo_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;
  int16 rea;

  for (i=0; i<25; i++) {
    int lnk = i - 1;
    _r *r = &r_fifo[i];

    if (ind != -1 && ind != i) continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;

    r->was[0] = r->rea[0];

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    get_JINF_event_nb(&rea, &err);

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
}

//~----------------------------------------------------------------------------

void reset_fifo_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;

  for (i=0; i<25; i++) {
    _r *r = &r_fifo[i];
    int lnk = i - 1;

    if (ind != -1 && ind != i) continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;
   
    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    reset_JINF_event_FIFO(&err);

    if (obj) {
      msg = 1;
      update_objects();
    }
  }
}

//~----------------------------------------------------------------------------

void read_status_callback(FL_OBJECT* obj, long data) {


  int ind = data;
  int i, k;
  int fmt;
  int16 rea[10];

  for (i=0; i<25; i++) {
    int lnk = i - 1;
    _r *r = r_status[i];

    if (ind != -1 && ind != i) continue;

    r->sta = status_NA;

    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;

    for (k=0; k<10; k++) r->was[k] = r->rea[k];

    if (obj) {
      msg = -1;
      update_objects();
    }
    JINF_PATH = i ? (lnk << 8) | 0x003F : 0x0000;
    NODE_ADR  = i ?        lnk_adr[lnk] : 0x082;
    get_JINF_status(&fmt, rea, NULL, NULL, NULL, &err);

    if (!err) {
      for (k=0; k<10; k++) r->rea[k] = rea[k];
      r->rea[3] = (float)(r->rea[3] | (r->rea[4] << 16)) / 100.0 + 0.5;
      if (r->unk) for (k=0; k<10; k++) r->was[k] = r->rea[k];
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
}

//~----------------------------------------------------------------------------

void write_all_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;

  for (i=0; i<25; i++) {
    int lnk = i - 1;
    if (ind != -1 && ind != i) continue;
    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;
    if (obj) {
      msg = -1;
      update_objects();
    }
    if (ind == -1) write_slave_mask_callback(NULL, i);
    write_SSF_callback(NULL, i);
    write_proc_mode_callback(NULL, i);
    write_delay_callback(NULL, i);
    reset_fifo_callback(NULL, i);
    if (obj) {
      msg = 1;
      update_objects();
    }
  }}

//~----------------------------------------------------------------------------

void read_all_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  int i;

  for (i=0; i<25; i++) {
    int lnk = i - 1;
    if (ind != -1 && ind != i) continue;
    if (i && ind == -1 && !(w_slave_mask.inp[0] & (1 << lnk))) continue;
    if (obj) {
      msg = -1;
      update_objects();
    }
    if (!i && ind == -1) read_slave_mask_callback(NULL, 0);
    read_SSF_callback(NULL, i);
    read_proc_mode_callback(NULL, i);
    read_delay_callback(NULL, i);
    read_fifo_callback(NULL, i);
    read_status_callback(NULL, i);
    if (obj) {
      msg = 1;
      update_objects();
    }
  }
}

//~----------------------------------------------------------------------------

void update_CDDC_control_panel(void) {

  int i, j;
  char b[80];
    
  show_btn_string(24,          NULL, SLAVE_MASK_BTN);
  show_dat_box_string(24, __a, NULL, SLAVE_MASK_BOX);

  show_btn_string(25,          NULL, SSF_BTN);
  show_dat_box_string(25, _up, NULL, SSF_BOX);

  show_btn_string(25,          NULL, PROC_MODE_BTN);
  show_dat_box_string(25, mod, NULL, PROC_MODE_BOX);
  
  for (i=0; i<25; i++) {
    _ww *w = &w_delay[i];
    _r  *r = &r_delay[i];
    sprintf(b, "%4.2f", 0.020*(float)(w->inp[0] & m_delay.msk));
    show_btn_string(1,           b, &DELAY_BTN[i]);
    sprintf(b, "%4.2f", 0.020*(float)(r->rea[0] & m_delay.msk));
    show_dat_box_string(1, NULL, b, &DELAY_BOX[i]);
  }

  show_dat_box_string(25, NULL, "%5d", FIFO_BOX);

  for (i=0; i<25; i++) {
    _r  *r = r_status[i];
    for (j=0; j<18; j++) {
      q = &Q[j];
      if (q->txt) show_dat_box_string(1, q->txt, NULL,   &STATUS_BOX[i][j]);
      else        show_dat_box_string(1, NULL,   q->fmt, &STATUS_BOX[i][j]);
      if (q->m.msk == 0xFFFFFFFF) {
        int32 time = r->rea[q->m.ind];
//time=time+1000000;
        if (time > 1000000) {
          sprintf(b, "..%d", time%10000);
          show_dat_box_string(1, NULL, b, &STATUS_BOX[i][j]);
        }
      }
    }
  }

  show_markers();
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_boot_panel();
  update_load_boot_panel();
  update_ping_panel();
  update_get_event_panel();

  update_CDDC_control_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
