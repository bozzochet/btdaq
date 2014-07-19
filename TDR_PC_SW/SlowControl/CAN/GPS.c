// file GPS.c
//
// GPS Commander (to send some simple but usefull TC)
//
// A.Lebedev Jun-2007...

#include "template.h"
#include "gpslib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"GPS Commander"};                      // global variable
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

int8 base = 0x00;

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~-- GPSE Configuration Panel

SW_BTN *USCM_BTN[2], *PORT_BTN[2], *RATE_BTN[2];
_ww w_uscm[2];
_ww w_port[2];
_ww w_rate[2];

_b b_uscm    =  {0, 0x0001, 2};
_b b_port[2] = {{0, 0x0003, 3}, {0, 0x0003, 4}};
_b b_rate    =  {0, 0x0003, 4};

bool auto_config = FALSE;

//~-- WMODE TC Panel

SW_BTN *CONF_PPS_BTN;
_ww w_conf_pps;

_b b_conf_pps = {0, 0x0001, 2};

//~-- GALEPH TC Panel

SW_BTN *GALEPH_TYP_BTN;
SW_BTN *GALEPH_PRN_BTN;
_ww w_galeph;

_b b_galeph[2] = {
  {0, 0x0001,  2},   // Type of Data (GPS Almanac/GPS Ephemeris)
  {1, 0x003F, 33}};  // GPS PRN

//~-- GMESS TC Panel

SW_BTN *GMESS_PERIOD_BTN[26];
SW_BTN *GMESS_TM_ID_BTN;
_ww w_gmess_period[26];
_ww w_gmess_tm_id;

_b b_gmess_period[26];
_b b_gmess_tm_id = {0, 0x001F, 32};

//~-- COA TC Panel

SW_BTN *COA_PERIOD_BTN;
_ww w_coa_period;

_b b_coa_period = {0, 0x0001, 2};

//~--

char *a_b[2] = {"A", "B"};

char *_01[4] = {"OFF", "0", "1", "0+1"};

char *rat[4] = {"2400", "4800", "9600", "19200"};

char *f_l[2] = {"Free-run", "Locked"};

char *o_o[2] = {"OFF", "ON"};

char *a_e[2] = {"Almanac", "Ephemeris"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_GPSE_configuration_panel(int x0, int y0);
void setup_TC_callback(FL_OBJECT *obj, long data);
void setup_TM_callback(FL_OBJECT *obj, long data);
void show_GPSE_configuration_panel(void);

void create_reset_GPS_panel(int x0, int y0);
void reset_GPS_callback(FL_OBJECT *obj, long data);

void create_WMODE_TC_panel(int x0, int y0);
void send_WMODE_TC_callback(FL_OBJECT *obj, long data);
void show_WMODE_TC_panel(void);

void create_GALEPH_TC_panel(int x0, int y0);
void send_GALEPH_TC_callback(FL_OBJECT *obj, long data);
void show_GALEPH_TC_panel(void);

void create_GMESS_TC_panel(int x0, int y0);
void send_GMESS_TC_callback(FL_OBJECT *obj, long data);
void show_GMESS_TC_panel(void);

void create_COA_TC_panel(int x0, int y0);
void send_COA_TC_callback(FL_OBJECT *obj, long data);
void show_COA_TC_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 470, HH = 660;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_GPSE_configuration_panel(0,  45);
  create_reset_GPS_panel(        180, 60);
  
  create_WMODE_TC_panel( 0, 120);
  create_GALEPH_TC_panel(0, 290);
  create_COA_TC_panel(   0, 350);

  create_GMESS_TC_panel(235,  45);
  
  create_USCM_control_H2_panel(15, 545);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void refresh_objects(void) {

  invalidate_data();
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;
  static int32 rate;
  
  for (i=0; i<2; i++) {
    invalidate_ww(first, &w_uscm[i], 1, 0);
    invalidate_ww(first, &w_port[i], 1, 0);
    invalidate_ww(first, &w_rate[i], 1, 0);
    w_rate[i].inp = &rate;
  }

  invalidate_ww(first, &w_conf_pps, 1, 0);
  invalidate_ww(first, &w_galeph, 2, 0);
  invalidate_ww(first, &w_coa_period, 1, 0);

  for (i=0; i<DIM(w_gmess_period); i++) {
    invalidate_ww(first, &w_gmess_period[i], 1, 0);
  }
  invalidate_ww(first, &w_gmess_tm_id, 1, 0);

  read_dat_config_file(dat_config_file_name);
  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  int i;
  FILE *file;
  
  file = fopen(filename, "r");
  if (file) {
    for (i=0; i<2; i++) {
      fread(w_uscm[i].inp,  4, 1, file);
      fread(w_port[i].inp,  4, 1, file);
      fread(w_rate[i].inp,  4, 1, file);
    }
    fread(w_conf_pps.inp,   4, 1, file);
    fread(w_galeph.inp,     4, 2, file);
    fread(w_coa_period.inp, 4, 1, file);
    for (i=0; i<DIM(w_gmess_period); i++) {
      fread(w_gmess_period[i].inp, 4, 1, file);
    }
    fread(w_gmess_tm_id.inp,4, 1, file);
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
  if (file) {
    for (i=0; i<2; i++) {
      fwrite(w_uscm[i].inp,  4, 1, file);
      fwrite(w_port[i].inp,  4, 1, file);
      fwrite(w_rate[i].inp,  4, 1, file);
    }
    fwrite(w_conf_pps.inp,   4, 1, file);
    fwrite(w_galeph.inp,     4, 2, file);
    fwrite(w_coa_period.inp, 4, 1, file);
    for (i=0; i<DIM(w_gmess_period); i++) {
      fwrite(w_gmess_period[i].inp, 4, 1, file);
    }
    fwrite(w_gmess_tm_id.inp,4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

void switch_btn_callback(int32 data) {

  msg = 0;
  update_objects();
}

//~============================================================================
//
//                  GPSE CONFIGURATION PANEL
//
//~============================================================================

void create_GPSE_configuration_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 25, w2 = 40, w3 = 30, w4 = 45, w5 = 30, h1 = 20, h2 = 15;
  char *txt[] = {"", "TC", "TM"};

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + 3 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "GPSE Configuration", 'M');
  y = y + h1;

  create_frame_box_vstring(3, x, y, w1, h2, txt, 'S', 'L');
  x = x + w1;

  add_frame_box(x, y, w2, h2, "USCM", 'S');
  create_btn_vstring(
    1, x, y+h2,   w2, h2, a_b, 'N', &w_uscm[0], &b_uscm,    &USCM_BTN[0]);
  create_btn_vstring(
    1, x, y+2*h2, w2, h2, a_b, 'N', &w_uscm[1], &b_uscm,    &USCM_BTN[1]);
  x = x + w2;

  add_frame_box(x, y, w3, h2, "Port", 'S');
  create_btn_vstring(
    1, x, y+h2,   w3, h2, _01, 'N', &w_port[0], &b_port[0], &PORT_BTN[0]);
  create_btn_vstring(
    1, x, y+2*h2, w3, h2, _01, 'N', &w_port[1], &b_port[1], &PORT_BTN[1]);
  x = x + w3;

  add_frame_box(x, y, w4, h2, "Rate", 'S');
  create_btn_vstring(
    1, x, y+h2,   w4, h2, rat, 'N', &w_rate[0], &b_rate,    &RATE_BTN[0]);
  create_btn_vstring(
    1, x, y+2*h2, w4, h2, rat, 'N', &w_rate[1], &b_rate,    &RATE_BTN[1]);
  x = x + w4;

  add_frame_box(x, y, w5, h2, "", 'S');
  obj = add_normal_button(x, y+h2,   w5, h2, "SET", 'N');
    fl_set_object_callback(obj, setup_TC_callback, 0);
  obj = add_normal_button(x, y+2*h2, w5, h2, "SET", 'N');
    fl_set_object_callback(obj, setup_TM_callback, 0);
}

//~----------------------------------------------------------------------------

void setup_TC_callback(FL_OBJECT *obj, long data) {

  int16 dat;
  int8 uscm_tc = WVAL(w_uscm[0], b_uscm);
  int8 port_tc = WVAL(w_port[0], b_port[0]);
  int8 rate_tc = WVAL(w_rate[0], b_rate);

  int8 mode = SERIAL_RX_ROLL | SERIAL_PORT_ENABLE;

//~--

  if (obj) {
    if (auto_config) {
      auto_config = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_config = TRUE;
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
        return;
      }
    }
  }

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  if (!err) read_LVDS_bus(base+0, &dat, &err);
  dat &= 0xFF00;
  if (!err) write_LVDS_bus(base+0, dat, &err);

//~--

  if (!err) read_LVDS_bus(base+1, &dat, &err);

  dat &= ~(USCM_A_PORT_0_TO_GPS | USCM_A_PORT_1_TO_GPS |
           USCM_B_PORT_0_TO_GPS | USCM_B_PORT_1_TO_GPS);

  if (uscm_tc) {
    if (port_tc == 1) dat |= USCM_B_PORT_0_TO_GPS;
    if (port_tc == 2) dat |= USCM_B_PORT_1_TO_GPS;
  }
  else {
    if (port_tc == 1) dat |= USCM_A_PORT_0_TO_GPS;
    if (port_tc == 2) dat |= USCM_A_PORT_1_TO_GPS;
  }
  if (!err) write_LVDS_bus(base+1, dat, &err);

  if (port_tc) {
    if (!err) setup_serial_port(port_tc-1, mode, rate_tc, &err);
  }

//~--

  if (!err) {
    w_uscm[0].wri[0] = w_uscm[0].inp[0];
    w_port[0].wri[0] = w_port[0].inp[0];
    w_rate[0].wri[0] = w_rate[0].inp[0];
    w_uscm[0].unk = FALSE;
    w_port[0].unk = FALSE;
    w_rate[0].unk = FALSE;
  }
  else {
    w_uscm[0].unk = TRUE;
    w_port[0].unk = TRUE;
    w_rate[0].unk = TRUE;
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void setup_TM_callback(FL_OBJECT *obj, long data) {

  int16 dat;
  int8 uscm_tm = WVAL(w_uscm[1], b_uscm);
  int8 port_tm = WVAL(w_port[1], b_port[1]);
  int8 rate_tm = WVAL(w_rate[1], b_rate);

  int8 mode = SERIAL_RX_ROLL | SERIAL_PORT_ENABLE;

//~--

  msg = -1;
  update_objects();

  err = 0x0000;

  if (!err) read_LVDS_bus(base+0, &dat, &err);
  dat &= 0xFF00;
  if (!err) write_LVDS_bus(base+0, dat, &err);

//~--

  if (!err) read_LVDS_bus(base+2, &dat, &err);

  dat &= ~(GPS_TO_USCM_A_PORT_0 | GPS_TO_USCM_A_PORT_1 |
           GPS_TO_USCM_B_PORT_0 | GPS_TO_USCM_B_PORT_1);

  if (uscm_tm) {
    if (port_tm == 1 || port_tm == 3) dat |= GPS_TO_USCM_B_PORT_0;
    if (port_tm == 2 || port_tm == 3) dat |= GPS_TO_USCM_B_PORT_1;
  }
  else {
    if (port_tm == 1 || port_tm == 3) dat |= GPS_TO_USCM_A_PORT_0;
    if (port_tm == 2 || port_tm == 3) dat |= GPS_TO_USCM_A_PORT_1;
  }
  if (!err) write_LVDS_bus(base+2, dat, &err);

  if (port_tm) {
    if (port_tm == 1 || port_tm == 3) {
      if (!err) setup_serial_port(0, mode, rate_tm, &err);
    }
    if (port_tm == 2 || port_tm == 3) {
      if (!err) setup_serial_port(1, mode, rate_tm, &err);
    }
    if (!err) write_LVDS_bus(base+3, OPEN_COM_GATE, &err);
  }
  else {
    if (!err) write_LVDS_bus(base+3, CLOSE_COM_GATE, &err);
  }
  
//~--

  if (!err) {
    w_uscm[1].wri[0] = w_uscm[1].inp[0];
    w_port[1].wri[0] = w_port[1].inp[0];
    w_rate[1].wri[0] = w_rate[1].inp[0];
    w_uscm[1].unk = FALSE;
    w_port[1].unk = FALSE;
    w_rate[1].unk = FALSE;
  }
  else {
    w_uscm[1].unk = TRUE;
    w_port[1].unk = TRUE;
    w_rate[1].unk = TRUE;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void show_GPSE_configuration_panel(void) {

  show_btn_string(2, NULL, &USCM_BTN[0]);
  show_btn_string(2, NULL, &PORT_BTN[0]);
  show_btn_string(2, NULL, &RATE_BTN[0]);
}

//~============================================================================
//
//                  RESET GPS PANEL
//
//~============================================================================

void create_reset_GPS_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 45, h1 = 40;

  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "RESET\nGPS", 'N');
    fl_set_object_callback(obj, reset_GPS_callback, 0);
}

//~----------------------------------------------------------------------------

void reset_GPS_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  write_LVDS_bus(base+3, RESET_GPS, &err);

  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  WMODE TC PANEL
//
//~============================================================================

void create_WMODE_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 145, w2 = 35, w3 = 45, h1 = 30, h2 = 20, h3 = 35;
  struct {
    char *nam;
    char *cod;
  } txt[4] = {
    {"Navigation to StandBy",                     "4333"},
    {"Navigation to Init\nDirect Ops to Init",    "4321"},
    {"Init to Navigation\nStandBy to Navigation", "1234"},
    {"Init to Direct Ops",                        "1414"}};
  int hh[4] = {h2, h3, h3, h2};

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + h2 + h2 + 2 * h3 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, "");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "WMODE  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  obj = add_frame_box(x, y, w1, h2, "Configuration for PPS", FL_NORMAL_SIZE);
    fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  create_btn_hstring(
    1, x+w1, y, w2+w3, h2, f_l, 'M', &w_conf_pps, &b_conf_pps, &CONF_PPS_BTN);
  y = y + h2;
  
  for (i=0; i<4; i++) {
    int h = hh[i];
    int16 cod = strtoul(txt[i].cod, NULL, 16);
    obj = add_frame_box(x, y, w1, h, txt[i].nam, FL_NORMAL_SIZE);
      fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    obj = add_frame_box(x+w1, y, w2, h, txt[i].cod, FL_NORMAL_SIZE);
    obj = add_normal_button(x+w1+w2, y, w3, h, "SEND", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, send_WMODE_TC_callback, cod);
    y = y + h;
  }
}

//~----------------------------------------------------------------------------

void send_WMODE_TC_callback(FL_OBJECT *obj, long data) {

  int16 cod = data;
  _tc tc;
  _ww *w = &w_conf_pps;
  int16 conf_pps = WVAL(w_conf_pps, b_conf_pps);
  int8  port_tc  = WVAL(w_port[0], b_port[0]);

  msg = -1;
  update_objects();

  err = 0x0000;
  
  create_WMODE(cod, conf_pps, &tc);
  if (auto_config) setup_TC_callback(NULL, 0);
  if (!err) to_GPS(port_tc-1, &tc, &err);
  if (!err) delay(1.0);

  if (!err) {
    w->wri[0] = w->inp[0];
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void show_WMODE_TC_panel(void) {

  show_btn_string(1, NULL, &CONF_PPS_BTN);
}

//~============================================================================
//
//                  GALEPH TC PANEL
//
//~============================================================================

void create_GALEPH_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 90, w2 = 55, w3 = 35, w4 = 45, h1 = 30, h2 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "GALEPH  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  create_btn_hstring(
    1, x, y, w1, h2, a_e,  'M', &w_galeph, &b_galeph[0], &GALEPH_TYP_BTN);
  x = x + w1;
  add_frame_box(x, y, w2, h2, "Satellite", FL_NORMAL_SIZE);
  x = x + w2;
  create_btn_hstring(
    1, x, y, w3, h2, NULL, 'M', &w_galeph, &b_galeph[1], &GALEPH_PRN_BTN);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h2, "SEND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, send_GALEPH_TC_callback, 0);
}

//~----------------------------------------------------------------------------

void send_GALEPH_TC_callback(FL_OBJECT *obj, long data) {

  _tc tc;
  _ww *w = &w_galeph;
  int32 alm_eph = WVAL(w_galeph, b_galeph[0]);
  long  PRN     = WVAL(w_galeph, b_galeph[1]);
  int8  port_tc = WVAL(w_port[0], b_port[0]);

  msg = -1;
  update_objects();

  err = 0x0000;
  
  create_GALEPH(alm_eph ? 0x00000012: 0x00000011, PRN, &tc);

  if (auto_config) setup_TC_callback(NULL, 0);
  if (!err) to_GPS(port_tc-1, &tc, &err);
  if (!err) delay(1.0);

  if (!err) {
    w->wri[0] = w->inp[0];
    w->wri[1] = w->inp[1];
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void show_GALEPH_TC_panel(void) {

  char b[80];
  int PRN = WVAL(w_galeph, b_galeph[1]);

  show_btn_string(1, NULL, &GALEPH_TYP_BTN);
  if (!PRN) sprintf(b, "ALL");
  else      sprintf(b, "%d", PRN);
  show_btn_string(1, b,    &GALEPH_PRN_BTN);
}

//~============================================================================
//
//                  COA TC PANEL
//
//~============================================================================

void create_COA_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 80, w2 = 40, w3 = 25, w4 = 35, w5 = 45, h1 = 30, h2 = 20;

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "COA  TC", 'L');
  y = y + h1;
  
  obj = add_frame_box(x, y, w1, h2, "S3SYSTEM",  'N');
    fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  add_frame_box(x, y, w2, h2, "2F42", 'N');
  x = x + w2;
  add_frame_box(x, y, w3, h2, "29",   'N');
  x = x + w3;
  create_btn_hstring(
    1, x, y, w4, h2, o_o, 'M', &w_coa_period, &b_coa_period, &COA_PERIOD_BTN);

  x = x + w4;
  obj = add_normal_button(x, y, w5, h2, "SEND", 'N');
    fl_set_object_callback(obj, send_COA_TC_callback, 0);
}

//~----------------------------------------------------------------------------

void send_COA_TC_callback(FL_OBJECT *obj, long data) {

  _tc tc;
  _ww *p = &w_coa_period;
  int32 period  = WVAL(w_coa_period, b_coa_period);
  int8  port_tc = WVAL(w_port[0], b_port[0]);

  msg = -1;
  update_objects();

  err = 0x0000;
  
  create_COA(period ? 0x00010001 : 0x00010000, &tc);

  if (auto_config) setup_TC_callback(NULL, 0);
  if (!err) to_GPS(port_tc-1, &tc, &err);
  if (!err) delay(1.0);

  if (!err) {
    p->wri[0] = p->inp[0];
    p->unk = FALSE;
  }
  else {
    p->unk = TRUE;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void show_COA_TC_panel(void) {

  show_btn_string(1, NULL, &COA_PERIOD_BTN);
}

//~============================================================================
//
//                  GMESS TC PANEL
//
//~============================================================================

static struct {
  char  *nam;
  char  *_422;
  char  *_1553;
  int   n;
  float s;
} table[26] = {
    {"STIME",     "2300", "03", 32,  1.0},    // 1 s
    {"SNAVS",     "2372", "06", 32,  1.0},    // 1 s
    {"SPHINT",    "252E", "09",  2,  0.1},    // 100 ms fixed
    {"SVELINT",   "2548", "10",  2,  0.1},    // 100 ms fixed
    {"SCOSDIR",   "2563", "11",  2,  1.0},    // 1 s fixed ?
    {"SRECEIVST", "2606", "13", 32,  1.0},    // 1 s
    {"SDUMP",     "2F0F", "26",  0},          // once

    {"SNAVI",     "2317", "04", 32,  1.0},    // 1 s
    {"SNAVN",     "233C", "05", 32,  1.0},    // 1 s
    {"SALEPH",    "239A", "07",  0},          // once
    {"S1RAWD",    "2909", "14", 32,  1.0},    // 1 s
    {"S2RAWD",    "291E", "15", 32,  1.0},    // 1 s
    {"S3RAWD",    "2922", "16", 32,  1.0},    // 1 s
    {"S1CHANST",  "2944", "17", 32,  1.0},    // 1 s
    {"S2CHANST",  "2953", "18", 32,  1.0},    // 1 s

    {"LSTATUS",   "10EB", "02", 32,  1.0},    // 1 sec ?

    {"NS1VECT",   "2C0C", "20", 32, 30.0},    // 30 s
    {"NS2VECT",   "2C1B", "21", 32, 30.0},    // 30 s
    {"NS3VECT",   "2C27", "22", 32, 30.0},    // 30 s
    {"NDIAG",     "2C41", "24",  0},          // once
    {"NRES",      "2C56", "25", 32, 30.0},    // 30 s
    {"S1SYSTEM",  "2F24", "27", 32,  1.0},    // 1 sec ?
    {"S2SYSTEM",  "2F33", "28", 32,  1.0},    // 1 sec ?
    {"S3SYSTEM",  "2F42", "29", 32,  1.0},    // 1 sec ?

    {"SPVTDAT",   "234E", "12", 32,  1.0},    // 1 sec ?

    {"Any Telecommand", NULL, NULL, 32, 1.0}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void create_GMESS_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 80, w2 = 40, w3 = 25, w4 = 35, w5 = 45, h1 = 30, h2 = 20;

  for (i=0; i<DIM(table); i++) {
    b_gmess_period[i].ind = 0;
    b_gmess_period[i].msk = -1;
    b_gmess_period[i].n   = table[i].n;
  }

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + DIM(table) * h2 + 5 * 5 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "GMESS  TC", 'L');
  y = y + h1;
  
  for (i=0; i<DIM(table); i++) {
    int32 TM_ID = 99;
    x = x0 + 5;
    w = i != (DIM(table) - 1) ? w1 : w1 + w2;
    obj = add_frame_box(x, y, w, h2, table[i].nam, 'N');
      fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w;
    if (i != (DIM(table) - 1)) {
      TM_ID = atoi(table[i]._1553);
      add_frame_box(x, y, w2, h2, table[i]._422,  'N');
      x = x + w2;
      add_frame_box(x, y, w3, h2, table[i]._1553, 'N');
    }
    else {
      create_btn_hstring(
        1, x, y, w3, h2, NULL, 'N', &w_gmess_tm_id, &b_gmess_tm_id, &GMESS_TM_ID_BTN);
    }
    x = x + w3;
    if (table[i].n) {
      create_btn_hstring(
        1, x, y, w4, h2, NULL, 'M', &w_gmess_period[i], &b_gmess_period[i], &GMESS_PERIOD_BTN[i]);
    }
    else {
      GMESS_PERIOD_BTN[i] = NULL;
      add_frame_box(x, y, w4, h2, "once", 'N');
      w_gmess_period[i].inp[0] = 1;
    }
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "SEND", 'N');
      fl_set_object_callback(obj, send_GMESS_TC_callback, i*100+TM_ID);
    y = y + h2;
    if (i == 6 || i == 14 || i == 15 || i == 23 || i == 24) y = y + 5;
  }
}

//~----------------------------------------------------------------------------

void send_GMESS_TC_callback(FL_OBJECT *obj, long data) {

  int ind   = data / 100;
  int TM_ID = data % 100;
  _tc tc;
  _ww *p = &w_gmess_period[ind];
  _ww *t = &w_gmess_tm_id;
  int32 period  = WVAL(w_gmess_period[ind], b_gmess_period[0]);
  int8  port_tc = WVAL(w_port[0], b_port[0]);

  msg = -1;
  update_objects();

  if (TM_ID == 99) TM_ID = WVAL(w_gmess_tm_id, b_gmess_tm_id);

  err = 0x0000;
  
  create_GMESS(TM_ID, period, &tc);

  if (auto_config) setup_TC_callback(NULL, 0);
  if (!err) to_GPS(port_tc-1, &tc, &err);
  if (!err) delay(1.0);

  if (!err) {
    p->wri[0] = p->inp[0];
    t->wri[0] = t->inp[0];
    p->unk = FALSE;
    t->unk = FALSE;
  }
  else {
    p->unk = TRUE;
    t->unk = TRUE;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void show_GMESS_TC_panel(void) {

  int i;
  char b[80];

  for (i=0; i<DIM(table); i++) {
    SW_BTN *B = GMESS_PERIOD_BTN[i];
    float s = table[i].s;
    _ww *w = &w_gmess_period[i];
    if (B) {
      if      (!w->inp[0]) sprintf(b, "OFF");
      else if (s  < 1.0)   sprintf(b, "%.1f", w->inp[0]*s);
      else                 sprintf(b, "%d", w->inp[0]*(int)s);
      show_btn_string(1, b, &B);
    }
  }

  sprintf(b, "%02d", w_gmess_tm_id.inp[0]);
  show_btn_string(1, b, &GMESS_TM_ID_BTN);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  show_GPSE_configuration_panel();

  show_WMODE_TC_panel();
  show_GALEPH_TC_panel();

  show_GMESS_TC_panel();
  show_COA_TC_panel();
  
  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
