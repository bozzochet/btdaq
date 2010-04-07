// file GPSE-2.c
//
// GPSE Controller
//
// A.Lebedev Nov-2005...

#include "template.h"
#include "gpslib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"GPSE-2 Controller"};                  // global variable
char *date  = {"07-Mar-08"};                          // global variable

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

//~-- Configuration Panel

SW_BTN *WHICH_USCM_BTN, *RATE_BTN[2];
_w which_USCM, rate[2];

//~-- Initialize GPS Panel

SW_BTN *INI_PORT_BTN;
SW_BTN *CONF_PPS_BTN;
FL_OBJECT *INI_LOG;
_w ini_port;
_w conf_pps;

//~-- Open TM Panel

SW_BTN *TM_PORT_BTN;
_w tm_port;

//~-- All Register' Panels

SW_BTN    *BIT_BTN[3][16];
FL_OBJECT *BIT_BOX[3][16];
_w wri_bit[4];
_r rea_bit[4];

//~--

SW_BTN *BGN_PORT_BTN;
_w bgn_port;
FL_OBJECT *SERIAL_INPUT[2][2];
int  serial_input_cnt[2];
int8 serial_input_dat[2][512];
bool serial_input_unk[2];
int  serial_input_sta[2];

//~--

char *_01[3] = {"0", "1", "0&1"};

char *a_b[2] = {"A", "B"};

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

char *rates[4] = {"2400", "4800", "9600", "19200"};

char *conf_pps_txt[2] = {"Free", "Locked"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_configuration_panel(int x0, int y0);

void create_initialize_GPS_panel(int x0, int y0);
void initialize_GPS_callback(FL_OBJECT *obj, long data);

void create_open_TM_panel(int x0, int y0);
void open_TM_callback(FL_OBJECT *obj, long data);

void write_register_callback(FL_OBJECT *obj, long data);
void read_register_callback(FL_OBJECT *obj, long data);

void create_control_and_status_register_panel(int x0, int y0);
void update_control_and_status_register_panel(void);

void create_serial_ports_outputs_register_panel(int x0, int y0);
void update_serial_ports_outputs_register_panel(void);

void create_serial_ports_inputs_register_panel(int x0, int y0);
void update_serial_ports_input_register_panel(void);

void create_pulse_commands_panel(int x0, int y0);
void pulse_commands_callback(FL_OBJECT *obj, long data);

void create_begin_epoch_panel(int x0, int y0);
void begin_epoch_callback(FL_OBJECT *obj, long data);
void update_begin_epoch_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 450, HH = 665;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_configuration_panel(                  0,  45);
  create_initialize_GPS_panel(               165,  45);
  create_open_TM_panel(                      375,  45);

  create_control_and_status_register_panel(    0,  95);
  create_serial_ports_outputs_register_panel(230,  95);
  create_serial_ports_inputs_register_panel( 230, 320);
  
  create_pulse_commands_panel(                 0, 495);
  create_begin_epoch_panel(                    0, 545);

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
  
  which_USCM.inp = 1;
  
  for (i=0; i<2; i++) {
    rate[i].inp = 3;
  }

  ini_port.inp = 0;
  conf_pps.inp = 0;
  tm_port.inp  = 0;
  
  for (i=0; i<4; i++) {
    wri_bit[i].inp = 0x0000;
    wri_bit[i].unk = TRUE;
  }

  for (i=0; i<4; i++) {
    rea_bit[i].unk = TRUE;
    rea_bit[i].sta = status_NA;
  }

  bgn_port.inp = 0;
  for (i=0; i<2; i++) {
    serial_input_unk[i] = TRUE;
    serial_input_sta[i] = status_NA;
  }
  
  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(&which_USCM.inp, sizeof(which_USCM.inp), 1, file);
    for (i=0; i<2; i++) {
      fread(&rate[i].inp, sizeof(rate[i].inp), 1, file);
    }
    fread(&ini_port.inp, sizeof(ini_port.inp), 1, file);
    fread(&conf_pps.inp, 4, 1, file);
    fread(&tm_port.inp,  sizeof(tm_port.inp),  1, file);
    for (i=0; i<4; i++) {
      fread(&wri_bit[i].inp, sizeof(wri_bit[i].inp), 1, file);
    }
    fread(&bgn_port.inp, sizeof(bgn_port.inp), 1, file);
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
    fwrite(&which_USCM.inp, sizeof(which_USCM.inp), 1, file);
    for (i=0; i<2; i++) {
      fwrite(&rate[i].inp, sizeof(rate[i].inp), 1, file);
    }
    fwrite(&ini_port.inp, sizeof(ini_port.inp), 1, file);
    fwrite(&conf_pps.inp, 4, 1, file);
    fwrite(&tm_port.inp,  sizeof(tm_port.inp),  1, file);
    for (i=0; i<4; i++) {
      fwrite(&wri_bit[i].inp, sizeof(wri_bit[i].inp), 1, file);
    }
    fwrite(&bgn_port.inp, sizeof(bgn_port.inp), 1, file);
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
//                  CONFIGURATION PANEL
//
//~============================================================================

void create_configuration_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 45, w2 = 20, w3 = 40, w4 = 50, h1 = 20;

  char *txt[2] = {"Port 0", "Port 1"};

  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, 2*h1, "Which\nUSCM", FL_NORMAL_SIZE);
  x = x + w1;
  create_num_btn_hstring(
    1, x, y, w2, 2*h1, a_b, FL_HUGE_SIZE, 2, &which_USCM, &WHICH_USCM_BTN);
  x = x + w2;

  for (i=0; i<2; i++) {
    add_frame_box(x, y, w3, h1, txt[i], FL_NORMAL_SIZE);
    create_num_btn_hstring(
      1, x+w3, y, w4, h1, rates, FL_MEDIUM_SIZE, 4, &rate[i], &RATE_BTN[i]);
    y = y + h1;
  }
}

//~============================================================================
//
//                  INITIALIZE GPS PANEL
//
//~============================================================================

void create_initialize_GPS_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 20, w3 = 30, w4 = 65, w5 = 55, h1 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  obj = add_frame_box(x, y, w1, h1, "Port", FL_NORMAL_SIZE);
  x = x + w1;
  
  create_num_btn_hstring(
    1, x, y, w2, h1, hex, FL_MEDIUM_SIZE, 2, &ini_port, &INI_PORT_BTN);
  x = x + w2;

  obj = add_frame_box(x, y, w3, h1, "PPS", FL_NORMAL_SIZE);
  x = x + w3;
  
  create_num_btn_hstring(
    1, x, y, w4, h1, conf_pps_txt, FL_MEDIUM_SIZE, 2, &conf_pps, &CONF_PPS_BTN);
  x = x + w4;

  obj = add_normal_button(x, y, w5, h1, "INIT GPS", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, initialize_GPS_callback, 0);

  x = x0 + 5;
  y = y + h1;
  
  INI_LOG =
  obj = add_data_box(x, y, w1+w2+w3+w4+w5, h1, "", FL_MEDIUM_SIZE);
    fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_color(obj, FL_YELLOW, 0);
}

//~----------------------------------------------------------------------------

void initialize_GPS_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *ob = INI_LOG;
  int16 dat;
  _tc tc;
  int i;
  char b[80];
  
  rea_bit[0].sta = status_NA;
  rea_bit[1].sta = status_NA;

  msg = -1;
  update_objects();

  fl_set_object_color(ob, FL_WHITE, 0);
  fl_set_object_lcolor(ob, FL_BLACK);

  err = 0x0000;
  
//~--
  
  if (!err) {
    read_LVDS_bus(base+0, &dat, &err);
  }
  if (!err) {
    dat &= 0xFF00;
    write_LVDS_bus(base+0, dat, &err);
  }

  if (!err) {
    read_LVDS_bus(base+1, &dat, &err);
  }
  if (!err) {
    dat &= ~(USCM_A_PORT_0_TO_GPS |
             USCM_A_PORT_1_TO_GPS |
             USCM_B_PORT_0_TO_GPS |
             USCM_B_PORT_1_TO_GPS);
    if (which_USCM.inp == 0) {
      if (ini_port.inp == 0) dat |= USCM_A_PORT_0_TO_GPS;
      if (ini_port.inp == 1) dat |= USCM_A_PORT_1_TO_GPS;
    }
    else {
      if (ini_port.inp == 0) dat |= USCM_B_PORT_0_TO_GPS;
      if (ini_port.inp == 1) dat |= USCM_B_PORT_1_TO_GPS;
    }
    write_LVDS_bus(base+1, dat, &err);
  }

  if (!err) {
//  write_LVDS_bus(base+3, CLOSE_EPOCH_GATE | CLOSE_COM_GATE, &err);
    write_LVDS_bus(base+3, CLOSE_EPOCH_GATE, &err);
  }

  if (!err) {
    setup_serial_port(ini_port.inp, 0x03, rate[ini_port.inp].inp, &err);
  }

//~--

  if (!err) {
    sprintf(b, "WMODE: 4321 %d", conf_pps.inp);
    fl_set_object_label(ob, b);
    fl_check_forms();
    create_WMODE(0x4321, conf_pps.inp, &tc);
    to_GPS(ini_port.inp, &tc, &err);
    delay(15.0);
  }

//~--

  if (!err) {
    sprintf(b, "WMODE: 1234 %d", conf_pps.inp);
    fl_set_object_label(ob, b);
    fl_check_forms();
    create_WMODE(0x1234, conf_pps.inp, &tc);
    to_GPS(ini_port.inp, &tc, &err);
    delay(7.0);
  }

//~--

  for (i=0x20; i<0x25; i++) {
    if (!err) {
      sprintf(b, "RCCS: cmd = 0x%2X", i);
      fl_set_object_label(ob, b);
      fl_check_forms();
//    create_RCCS(i, -7000, 7000, &tc);
      create_RCCS(i, -7000, 7000, NULL, &tc);
      to_GPS(ini_port.inp, &tc, &err);
      delay(1.0);
    }
  }
  
//~--

  if (!err) {
    sprintf(b, "RATT: ...");
    fl_set_object_label(ob, b);
    fl_check_forms();
//  create_RATT(&tc);
    create_RATT(2, 0, 0, 0, 0, 0xFF, &tc);
    to_GPS(ini_port.inp, &tc, &err);
    delay(1.0);
  }

//~--

#if 0
  if (!err) {
    int32 now, week, sec;
    now  = (int32)time(NULL) - 0x12D53D80;
    week = now / 604800;
    sec  = now % 604800;
    sprintf(b, "RTIME: %d %d", week, sec);
    fl_set_object_label(ob, b);
    fl_check_forms();
//  create_RTIME(week, sec, &tc);
    create_RTIME(1, week, sec, &tc);
    to_GPS(ini_port.inp, &tc, &err);
    delay(2.0);
  }
#endif

//~--

  if (!err) {
    int32 COA_mgt = 0x00010000;
    sprintf(b, "COA: %08X", COA_mgt);
    fl_set_object_label(ob, b);
    fl_check_forms();
    create_COA(COA_mgt, &tc);
    to_GPS(ini_port.inp, &tc, &err);
    delay(1.0);
  }

//~--

  for (i=2; i<30; i++) {
    if (!err) {
      sprintf(b, "GMESS: ID=%d p=0", i);
      fl_set_object_label(ob, b);
      fl_check_forms();
      create_GMESS(i, 0, &tc);
      to_GPS(ini_port.inp, &tc, &err);
      delay(1.0);
    }
  }
  
  if (!err) {
    sprintf(b, "GMESS: ID=3 p=1");
    fl_set_object_label(ob, b);
    fl_check_forms();
    create_GMESS(3, 1, &tc);
    to_GPS(ini_port.inp, &tc, &err);
    delay(1.0);
  }

//~--

  if (!err) {
    setup_serial_port(ini_port.inp, 0x00, rate[i].inp, &err);
  }
  
//~--

  if (!err) {
    fl_set_object_color(ob, FL_GREEN, 0);
    fl_set_object_lcolor(ob, FL_BLACK);
    fl_set_object_label(ob, "DONE");
  }
  else {
    fl_set_object_color(ob, FL_RED, 0);
    fl_set_object_lcolor(ob, FL_YELLOW);
    fl_set_object_label(ob, "FAILED");
  }
  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  OPEN TM PANEL
//
//~============================================================================

void create_open_TM_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 35, h1 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + 2*h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  obj = add_frame_box(x, y, w1, h1, "Port", FL_NORMAL_SIZE);
  x = x + w1;
  
  create_num_btn_hstring(
    1, x, y, w2, h1, _01, FL_MEDIUM_SIZE, 3, &tm_port, &TM_PORT_BTN);
  x = x + w2;

  x = x0 + 5;
  y = y + h1;

  obj = add_normal_button(x, y, w1+w2, h1, "OPEN  TM", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, open_TM_callback, 0);
}

//~----------------------------------------------------------------------------

void open_TM_callback(FL_OBJECT *obj, long data) {

  int16 dat;
  
  rea_bit[0].sta = status_NA;
  rea_bit[2].sta = status_NA;

  msg = -1;
  update_objects();

  err = 0x0000;

  if (!err) {
    read_LVDS_bus(base+0, &dat, &err);
  }
  if (!err) {
    dat &= 0xFF00;
    write_LVDS_bus(base+0, dat, &err);
  }

  if (!err) {
    read_LVDS_bus(base+2, &dat, &err);
  }
  if (!err) {
    if (which_USCM.inp == 0) {
      dat &= ~(GPS_TO_USCM_A_PORT_0 | GPS_TO_USCM_A_PORT_1);
      if (tm_port.inp == 0 || tm_port.inp == 2) dat |= GPS_TO_USCM_A_PORT_0;
      if (tm_port.inp == 1 || tm_port.inp == 2) dat |= GPS_TO_USCM_A_PORT_1;
    }
    else {
      dat &= ~(GPS_TO_USCM_B_PORT_0 | GPS_TO_USCM_B_PORT_1);
      if (tm_port.inp == 0 || tm_port.inp == 2) dat |= GPS_TO_USCM_B_PORT_0;
      if (tm_port.inp == 1 || tm_port.inp == 2) dat |= GPS_TO_USCM_B_PORT_1;
    }
    write_LVDS_bus(base+2, dat, &err);
  }

  if (!err) {
    if (tm_port.inp == 0 || tm_port.inp == 2) {
      setup_serial_port(0, 0x03, rate[0].inp, &err);
    }
  }
  if (!err) {
    if (tm_port.inp == 1 || tm_port.inp == 2) {
      setup_serial_port(1, 0x03, rate[1].inp, &err);
    }
  }
  
  if (!err) {
    write_LVDS_bus(base+3, OPEN_COM_GATE, &err);
  }
  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  WRITE AND READ REGISTER CALLBACKs
//
//~============================================================================

void write_register_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  r->sta = status_NA;

  msg = -1;
  update_objects();

  write_LVDS_bus(base+ind, (int16)w->inp, &err);
  
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

void read_register_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _r *r = &rea_bit[ind];
  int16 dat;

  msg = -1;
  update_objects();

  *r->was = *r->rea;

  read_LVDS_bus(base+ind, &dat, &err);
  
  if (!err) {
    *r->rea = dat;
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

//~============================================================================
//
//                  CONTROL AND STATUS REGISTER PANEL
//
//~============================================================================

void create_control_and_status_register_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 160, w3 = 20, h1 = 30, h2 = 20;

  int ind = 0;
  
  char *txt[16] = {
    "EPOCH Gate Auto Close",
    "COM Gate Auto Open",
    "COM Gate Auto Close",
    "EPOCH Gate Status",
    "COM Gate Status",
    "USCM A Power Status",
    "USCM B Power Status",
    "GPS DWTO Status",
    "PPS Gate Port A Status",
    "PPS Gate Port B Status",
    "GPS Output Polarity",
    "GPS Input Polarity",
    "AST Serial Port",
    "AST Output Polarity",
    "AST Input Polarity",
    "GPSE Power Bit"};
  
  w = 5 + w1 + w2 + 2 * w3 + 5;
  h = 5 + h1 + 16 * h2 + 4 * 5 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+2*w3, h1, "Control & Status Register", FL_MEDIUM_SIZE);
  y = y + h1;
  
  rea_bit[ind].rea = malloc(4);
  rea_bit[ind].was = malloc(4);

  create_frame_box_vstring(16, x,    y, w1, h2, &dec[0], FL_NORMAL_SIZE + 400, 'C');
  create_frame_box_vstring(16, x+w1, y, w2, h2, &txt[0], FL_NORMAL_SIZE + 400, 'L');
  create_bit_btn_vstring(
    -16, x+w1+w2, y, w3, h2, hex, FL_MEDIUM_SIZE + 400, &wri_bit[ind], &BIT_BTN[ind][0]);
  create_data_box_vstring(16, x+w1+w2+w3, y, w3, h2, FL_MEDIUM_SIZE + 400, &BIT_BOX[ind][0]);
  y = y + 16 * h2 + 4 * 5;
  
  obj = add_frame_box(x, y, w1+w2, h2, "Command:", FL_NORMAL_SIZE);
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  obj = add_normal_button(x+w1+w2, y, w3, h2, "W", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, write_register_callback, ind);
  obj = add_normal_button(x+w1+w2+w3, y, w3, h2, "R", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_register_callback, ind);
}

//~----------------------------------------------------------------------------

void update_control_and_status_register_panel(void) {

  int ind = 0;

  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  show_bin_btn_string(-16, w, &BIT_BTN[ind][0]);
  show_bit_box_string(-16, r, NULL, &BIT_BOX[ind][0]);
}

//~============================================================================
//
//                  USCM SERIAL PORTS OUTPUTS REGISTER PANEL
//
//~============================================================================

void create_serial_ports_outputs_register_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 150, w3 = 20, h1 = 30, h2 = 20;

  int ind = 1;
  
  char *txt[8] = {
    "USCM A Port 0 to GPS",
    "USCM A Port 1 to GPS",
    "USCM B Port 0 to GPS",
    "USCM B Port 1 to GPS",
    "USCM A Port 0 to AST",
    "USCM A Port 1 to AST",
    "USCM B Port 0 to AST",
    "USCM B Port 1 to AST"};
  
  w = 5 + w1 + w2 + 2 * w3 + 5;
  h = 5 + h1 + 8 * h2 + 5 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+2*w3, h1, "USCM Serial Ports Outputs", FL_MEDIUM_SIZE);
  y = y + h1;
  
  rea_bit[ind].rea = malloc(4);
  rea_bit[ind].was = malloc(4);

  create_frame_box_vstring(8, x,    y, w1, h2, &dec[0], FL_NORMAL_SIZE + 400, 'C');
  create_frame_box_vstring(8, x+w1, y, w2, h2, &txt[0], FL_NORMAL_SIZE + 400, 'L');
  create_bit_btn_vstring(
    -8, x+w1+w2, y, w3, h2, hex, FL_MEDIUM_SIZE + 400, &wri_bit[ind], &BIT_BTN[ind][0]);
  create_data_box_vstring(8, x+w1+w2+w3, y, w3, h2, FL_MEDIUM_SIZE + 400, &BIT_BOX[ind][0]);
  y = y + 8 * h2 + 5;
  
  obj = add_frame_box(x, y, w1+w2, h2, "Command:", FL_NORMAL_SIZE);
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  obj = add_normal_button(x+w1+w2, y, w3, h2, "W", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, write_register_callback, ind);
  obj = add_normal_button(x+w1+w2+w3, y, w3, h2, "R", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_register_callback, ind);
}

//~----------------------------------------------------------------------------

void update_serial_ports_outputs_register_panel(void) {

  int ind = 1;

  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  show_bin_btn_string(-8, w, &BIT_BTN[ind][0]);
  show_bit_box_string(-8, r, NULL, &BIT_BOX[ind][0]);
}

//~============================================================================
//
//                  USCM SERIAL PORTS INPUTS REGISTER PANEL
//
//~============================================================================

void create_serial_ports_inputs_register_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 150, w3 = 20, h1 = 30, h2 = 20;

  int ind = 2;
  
  char *txt[8] = {
    "GPS to USCM A Port 0",
    "GPS to USCM A Port 1",
    "GPS to USCM B Port 0",
    "GPS to USCM B Port 1",
    "AST to USCM A Port 0",
    "AST to USCM A Port 1",
    "AST to USCM B Port 0",
    "AST to USCM B Port 1"};
  
  w = 5 + w1 + w2 + 2 * w3 + 5;
  h = 5 + h1 + 8 * h2 + 5 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+2*w3, h1, "USCM Serial Ports Inputs", FL_MEDIUM_SIZE);
  y = y + h1;
  
  rea_bit[ind].rea = malloc(4);
  rea_bit[ind].was = malloc(4);

  create_frame_box_vstring(8, x,    y, w1, h2, &dec[0], FL_NORMAL_SIZE + 400, 'C');
  create_frame_box_vstring(8, x+w1, y, w2, h2, &txt[0], FL_NORMAL_SIZE + 400, 'L');
  create_bit_btn_vstring(
    -8, x+w1+w2, y, w3, h2, hex, FL_MEDIUM_SIZE + 400, &wri_bit[ind], &BIT_BTN[ind][0]);
  create_data_box_vstring(8, x+w1+w2+w3, y, w3, h2, FL_MEDIUM_SIZE + 400, &BIT_BOX[ind][0]);
  y = y + 8 * h2 + 5;
  
  obj = add_frame_box(x, y, w1+w2, h2, "Command:", FL_NORMAL_SIZE);
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  obj = add_normal_button(x+w1+w2, y, w3, h2, "W", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, write_register_callback, ind);
  obj = add_normal_button(x+w1+w2+w3, y, w3, h2, "R", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_register_callback, ind);
}

//~----------------------------------------------------------------------------

void update_serial_ports_inputs_register_panel(void) {

  int ind = 2;

  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  show_bin_btn_string(-8, w, &BIT_BTN[ind][0]);
  show_bit_box_string(-8, r, NULL, &BIT_BOX[ind][0]);
}

//~============================================================================
//
//                  PULSE COMMANDS PANEL
//
//~============================================================================

void create_pulse_commands_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 35, h1 = 20;

  w = 5 + 6 * w1 + 2 * 5 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x,           y,      w1, h1, "PPS", FL_NORMAL_SIZE);
  obj = add_normal_button(x, y+h1,   w1, h1, "SND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, pulse_commands_callback, SEND_PPS);
  x = x + w1 + 5;

  add_frame_box(x,           y,    2*w1, h1, "EPO Gate", FL_NORMAL_SIZE);
  obj = add_normal_button(x, y+h1,   w1, h1, "OPN",        FL_NORMAL_SIZE);
    fl_set_object_callback(obj, pulse_commands_callback, OPEN_EPOCH_GATE);
  x = x + w1;
  obj = add_normal_button(x, y+h1,   w1, h1, "CLS",        FL_NORMAL_SIZE);
    fl_set_object_callback(obj, pulse_commands_callback, CLOSE_EPOCH_GATE);
  x = x + w1;

  add_frame_box(x,           y,    2*w1, h1, "COM Gate", FL_NORMAL_SIZE);
  obj = add_normal_button(x, y+h1,   w1, h1, "OPN",      FL_NORMAL_SIZE);
    fl_set_object_callback(obj, pulse_commands_callback, OPEN_COM_GATE);
  x = x + w1;
  obj = add_normal_button(x, y+h1,   w1, h1, "CLS",      FL_NORMAL_SIZE);
    fl_set_object_callback(obj, pulse_commands_callback, CLOSE_COM_GATE);
  x = x + w1;

  x = x + 5;
  add_frame_box(x,           y,      w1, h1, "GPS", FL_NORMAL_SIZE);
  obj = add_normal_button(x, y+h1,   w1, h1, "RST", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, pulse_commands_callback, RESET_GPS);
  x = x + w1;
}

//~----------------------------------------------------------------------------

void pulse_commands_callback(FL_OBJECT *obj, long data) {

  int16 val = data;

  rea_bit[0].sta = status_NA;
  wri_bit[3].unk = TRUE;

  msg = -1;
  update_objects();

  write_LVDS_bus(base+3, val, &err);

  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  BEGIN EPOCH PANEL
//
//~============================================================================

void create_begin_epoch_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h;
  int w1 = 20, w2 = 350, w3 = 35, w4 = 35, h1 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  for (i=0; i<2; i++) {
    add_frame_box(x, y, w1, 2*h1, dec[i], FL_NORMAL_SIZE);
    for (j=0; j<2; j++) {
      SERIAL_INPUT[i][j] =
      obj = add_data_box(x+w1, y, w2, h1, "", j ? FL_MEDIUM_SIZE : FL_SMALL_SIZE);
        fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
      y = y + h1;
    }
  }

  x = x + w1 + w2;
  y = y0 + 5;

  obj = add_frame_box(x, y, w3, h1, "Port", FL_NORMAL_SIZE);
  
  create_num_btn_hstring(
    1, x+w3, y, w4, h1, _01, FL_MEDIUM_SIZE, 3, &bgn_port, &BGN_PORT_BTN);

  y = y + h1;

  obj = add_normal_button(x, y, w3+w4, 3*h1, "BEGIN\nEPOCH", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, begin_epoch_callback, 0);
}

//~----------------------------------------------------------------------------

void begin_epoch_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat;
  bool use_port[2];

//~--
  
  use_port[0] = bgn_port.inp == 0 || bgn_port.inp == 2;
  use_port[1] = bgn_port.inp == 1 || bgn_port.inp == 2;
  
  rea_bit[0].sta = status_NA;
  rea_bit[2].sta = status_NA;
  
  for (i=0; i<2; i++) {
    serial_input_sta[i] = status_NA;
  }
  
//~--
  
  msg = -1;
  update_objects();

  err = 0;

//~--
  
  if (P > 0) printf("***BEGIN EPOCH: starting...\n");
  if (P > 0) printf("***BEGIN EPOCH: Setup registers\n");
  
  if (!err) {
    read_LVDS_bus(base+0, &dat, &err);
  }
  if (!err) {
    dat &= 0xFF00;
    dat |= EPOCH_GATE_AUTO_OPEN | COM_GATE_AUTO_OPEN | COM_GATE_AUTO_CLOSE;
    write_LVDS_bus(base+0, dat, &err);
  }
  if (!err) {
    write_LVDS_bus(base+3, CLOSE_EPOCH_GATE | CLOSE_COM_GATE, &err);
  }
  
  if (!err) {
    read_LVDS_bus(base+2, &dat, &err);
  }
  if (!err) {
    if (which_USCM.inp == 0) {
      dat &= ~(GPS_TO_USCM_A_PORT_0 | GPS_TO_USCM_A_PORT_1);
      if (use_port[0]) dat |= GPS_TO_USCM_A_PORT_0;
      if (use_port[1]) dat |= GPS_TO_USCM_A_PORT_1;
    }
    else {
      dat &= ~(GPS_TO_USCM_B_PORT_0 | GPS_TO_USCM_B_PORT_1);
      if (use_port[0]) dat |= GPS_TO_USCM_B_PORT_0;
      if (use_port[1]) dat |= GPS_TO_USCM_B_PORT_1;
    }
    write_LVDS_bus(base+2, dat, &err);
  }

//~--
  
  if (P > 0) printf("***BEGIN EPOCH: Enable serial ports\n");

  for (i=0; i<2; i++) {
    if (!err) {
      if (use_port[i]) setup_serial_port(i, 0x03, rate[i].inp, &err);
    }
  }

//~--
  
  if (P > 0) printf("***BEGIN EPOCH: Open EPOCH DATE (should be once) and wait\n");

  if (!err) {
    write_LVDS_bus(base+3, OPEN_EPOCH_GATE, &err);
  }
  
  if (!err) {
    sleep(1);
    sleep(1);
  }
  
//~--

  if (P > 0) printf("***BEGIN EPOCH: Read serial port(s) and disable them\n");

  for (i=0; i<2; i++) {
    if (!err) {
      if (use_port[i]) {
        read_serial_port(i, 512, &serial_input_cnt[i], &serial_input_dat[i][0], &err);
        if (!err) {
          setup_serial_port(i, 0x00, rate[i].inp, &err);
        }
      }
    }
  }

  if (P > 0) printf("***BEGIN EPOCH: all done\n");

//~--

  if (!err) {
    for (i=0; i<2; i++) {
      if (use_port[i]) serial_input_unk[i] = FALSE;
      if (use_port[i]) serial_input_sta[i] = status_OK;
    }
  }
  else {
    for (i=0; i<2; i++) {
      if (use_port[i]) serial_input_sta[i] = status_KO;
    }
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_begin_epoch_panel(void) {

  FL_OBJECT *obj;
  int i, j;
  int c, lc;
  bool use_port[2];

  use_port[0] = bgn_port.inp == 0 || bgn_port.inp == 2;
  use_port[1] = bgn_port.inp == 1 || bgn_port.inp == 2;
  
  for (i=0; i<2; i++) {
    char b[80], *p = b;
    sprintf(p, " ");
    if (!serial_input_unk[i]) {
      for (j=0; j<serial_input_cnt[i]; j++) {
        if (j > 16) {
          sprintf(p-1, "...");
          break;
        }
        p += sprintf(p, "%02X ", serial_input_dat[i][j]);
      }
    }
    obj = SERIAL_INPUT[i][0];
    fl_set_object_label(obj, b);
  }

  for (i=0; i<2; i++) {
    char b[80], *p = b;
    sprintf(p, " ");
    if (!serial_input_unk[i] && (serial_input_cnt[i] > (11 + 8))) {
      time_t t;
      struct tm r;
      int16 ID, week;
      int32 sec;
      ID   =               serial_input_dat[i][2];
      ID   = (ID   << 8) + serial_input_dat[i][3];
      week =               serial_input_dat[i][ 6+8];
      week = (week << 8) + serial_input_dat[i][ 7+8];
      sec  =               serial_input_dat[i][ 8+8];
      sec  = (sec  << 8) + serial_input_dat[i][ 9+8];
      sec  = (sec  << 8) + serial_input_dat[i][10+8];
      sec  = (sec  << 8) + serial_input_dat[i][11+8];
      p += sprintf(p, "WK=%d S=%d  ", week, sec);
      t = 0x12D53D80 + 604800 * week + sec;
      localtime_r(&t, &r);
      p += sprintf(p, "%02d/%02d/%02d ", r.tm_mday, 1+r.tm_mon, 1900+r.tm_year);
      p += sprintf(p, "%02d:%02d:%02d", r.tm_hour, r.tm_min, r.tm_sec);
    }

    obj = SERIAL_INPUT[i][1];
    fl_set_object_label(obj, b);
  }

  for (i=0; i<2; i++) {
    for (j=0; j<2; j++) {
      obj = SERIAL_INPUT[i][j];
      c  = FL_GREEN;
      lc = FL_BLACK;
      if (serial_input_sta[i] == status_NA) c = FL_YELLOW;
      if (serial_input_sta[i] == status_KO) {
        c  = FL_BLACK;
        lc = FL_WHITE;
      }
      fl_set_object_color( obj,  c, 0);
      fl_set_object_lcolor(obj, lc);
    }
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_control_and_status_register_panel();
  update_serial_ports_outputs_register_panel();
  update_serial_ports_inputs_register_panel();
  update_begin_epoch_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
