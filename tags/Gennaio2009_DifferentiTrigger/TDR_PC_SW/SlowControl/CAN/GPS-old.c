// file GPS.c
//
// GPS Commander (to send some simple but usefull TC)
//
// A.Lebedev Jun-2007...

#include "template.h"
#include "gpslib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"GPS Commander"};                      // global variable
char *date  = {"28-Jun-08"};                          // global variable

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

SW_BTN *USCM_BTN;
SW_BTN *PORT_BTN;
SW_BTN *RATE_BTN;
_w uscm;
_w port;
_w rate;

//~-- WMODE TC Panel

SW_BTN *CONF_PPS_BTN;
_w conf_pps;

//~-- GALEPH TC Panel

SW_BTN *GALEPH_DAT_BTN;
SW_BTN *GALEPH_NUM_BTN;
_w galeph_dat;
_w galeph_num;

//~-- GDUMP TC Panel

SW_BTN *GDUMP_ADR_BTN[8];
SW_BTN *GDUMP_BLK_BTN[1];
_w gdump_adr;
_w gdump_blk;

//~-- LINIT TC Panel

SW_BTN *LINIT_DST_BTN[8];
SW_BTN *LINIT_LEN_BTN[8];
_w linit_dst;
_w linit_len;

//~-- LREPROM TC Panel

SW_BTN *LREPROM_SRC_BTN[8];
SW_BTN *LREPROM_DST_BTN[8];
SW_BTN *LREPROM_LEN_BTN[8];
_w lreprom_src;
_w lreprom_dst;
_w lreprom_len;

//~-- GMESS TC Panel

SW_BTN *PERIOD_BTN[21];
SW_BTN *CMD_ID_BTN;
_w period[21];
_w cmd_id;

//~-- COA TC Panel

SW_BTN *COA_BTN;
_w coa;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *a_b[2] = {"A", "B"};

char *rates[4] = {"2400", "4800", "9600", "19200"};

char *conf_pps_txt[2] = {"Free-run", "Locked"};

char *o_o[2] = {"OFF", "ON"};

char *a_e[2] = {"Almanac", "Ephemeris"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_configuration_panel(int x0, int y0);

void setup_serial_port_for_commanding(void);

void create_WMODE_TC_panel(int x0, int y0);
void send_WMODE_TC_callback(FL_OBJECT *obj, long data);

void create_GALEPH_TC_panel(int x0, int y0);
void send_GALEPH_TC_callback(FL_OBJECT *obj, long data);

void create_GDUMP_TC_panel(int x0, int y0);
void send_GDUMP_TC_callback(FL_OBJECT *obj, long data);

void create_LINIT_TC_panel(int x0, int y0);
void send_LINIT_TC_callback(FL_OBJECT *obj, long data);

void create_LREPROM_TC_panel(int x0, int y0);
void send_LREPROM_TC_callback(FL_OBJECT *obj, long data);

void create_reset_GPS_panel(int x0, int y0);
void send_reset_GPS_callback(FL_OBJECT *obj, long data);

void create_GMESS_TC_panel(int x0, int y0);
void send_GMESS_TC_callback(FL_OBJECT *obj, long data);

void create_COA_TC_panel(int x0, int y0);
void send_COA_TC_callback(FL_OBJECT *obj, long data);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 465, HH = 645;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_configuration_panel(0, 45);
  
  create_WMODE_TC_panel(  0,  75);
  create_GALEPH_TC_panel( 0, 245);
  create_GDUMP_TC_panel(  0, 305);
  create_LINIT_TC_panel(  0, 385);
  create_LREPROM_TC_panel(0, 465);
  create_reset_GPS_panel( 0, 565);

  create_GMESS_TC_panel(235,  75);
  create_COA_TC_panel(  235, 555);
  
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

  invalidate_w(&uscm, 1, 0);
  invalidate_w(&port, 1, 0);
  invalidate_w(&rate, 1, 2);
  invalidate_w(&conf_pps, 1, 0);
  invalidate_w(&galeph_dat, 1, 0);
  invalidate_w(&galeph_num, 1, 0);
  invalidate_w(&gdump_adr, 1, 0);
  invalidate_w(&gdump_blk, 1, 0);
  invalidate_w(&linit_dst, 1, 0);
  invalidate_w(&linit_len, 1, 0);
  invalidate_w(&lreprom_src, 1, 0);
  invalidate_w(&lreprom_dst, 1, 0);
  invalidate_w(&lreprom_len, 1, 0);

  invalidate_w(&period[0], DIM(period), 0);
  invalidate_w(&cmd_id, 1, 0);
  invalidate_w(&coa, 1, 0);

  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(&uscm.inp,         4, 1, file);
    fread(&port.inp,         4, 1, file);
    fread(&rate.inp,         4, 1, file);
    fread(&conf_pps.inp,     4, 1, file);
    fread(&galeph_dat.inp,   4, 1, file);
    fread(&galeph_num.inp,   4, 1, file);
    fread(&gdump_adr.inp,    4, 1, file);
    fread(&gdump_blk.inp,    4, 1, file);
    fread(&linit_dst.inp,    4, 1, file);
    fread(&linit_len.inp,    4, 1, file);
    fread(&lreprom_src.inp,  4, 1, file);
    fread(&lreprom_dst.inp,  4, 1, file);
    fread(&lreprom_len.inp,  4, 1, file);
    for (i=0; i<DIM(period); i++) {
      fread(&period[i].inp,  4, 1, file);
    }
    fread(&cmd_id.inp,       4, 1, file);
    fread(&coa.inp,          4, 1, file);
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
    fwrite(&uscm.inp,         4, 1, file);
    fwrite(&port.inp,         4, 1, file);
    fwrite(&rate.inp,         4, 1, file);
    fwrite(&conf_pps.inp,     4, 1, file);
    fwrite(&galeph_dat.inp,   4, 1, file);
    fwrite(&galeph_num.inp,   4, 1, file);
    fwrite(&gdump_adr.inp,    4, 1, file);
    fwrite(&gdump_blk.inp,    4, 1, file);
    fwrite(&linit_dst.inp,    4, 1, file);
    fwrite(&linit_len.inp,    4, 1, file);
    fwrite(&lreprom_src.inp,  4, 1, file);
    fwrite(&lreprom_dst.inp,  4, 1, file);
    fwrite(&lreprom_len.inp,  4, 1, file);
    for (i=0; i<DIM(period); i++) {
      fwrite(&period[i].inp,  4, 1, file);
    }
    fwrite(&cmd_id.inp,       4, 1, file);
    fwrite(&coa.inp,          4, 1, file);
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

  int x, y, w, h;
  int w1 = 45, w2 = 15, w3 = 35, w4 = 15, w5 = 35, w6 = 55, h1 = 20;

  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "USCM", FL_NORMAL_SIZE);
  x = x + w1;
  create_num_btn_hstring(
    1, x, y, w2, h1, a_b, FL_MEDIUM_SIZE, 2, &uscm, &USCM_BTN);
  x = x + w2;

  add_frame_box(x, y, w3, h1, "Port", FL_NORMAL_SIZE);
  x = x + w3;
  create_num_btn_hstring(
    1, x, y, w4, h1, hex, FL_MEDIUM_SIZE, 2, &port, &PORT_BTN);
  x = x + w4;

  add_frame_box(x, y, w5, h1, "Rate", FL_NORMAL_SIZE);
  x = x + w5;
  create_num_btn_hstring(
    1, x, y, w6, h1, rates, FL_MEDIUM_SIZE, 4, &rate, &RATE_BTN);
  x = x + w6;
}

//~============================================================================
//
//                  SETUP SERIAL PORT FOR COMMANDING
//
//~============================================================================

void setup_serial_port_for_commanding(void) {

  int16 dat;

  err = 0x0000;

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
    if (uscm.inp == 0) {
      if (port.inp == 0) dat |= USCM_A_PORT_0_TO_GPS;
      if (port.inp == 1) dat |= USCM_A_PORT_1_TO_GPS;
    }
    else {
      if (port.inp == 0) dat |= USCM_B_PORT_0_TO_GPS;
      if (port.inp == 1) dat |= USCM_B_PORT_1_TO_GPS;
    }
    write_LVDS_bus(base+1, dat, &err);
  }

  if (!err) {
    write_LVDS_bus(base+3, CLOSE_EPOCH_GATE, &err);
  }

  if (!err) {
    setup_serial_port(
      port.inp, SERIAL_PORT_ENABLE | SERIAL_RX_ROLL, rate.inp, &err);
  }
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
  create_num_btn_hstring(
    1, x+w1, y, w2+w3, h2, conf_pps_txt, FL_MEDIUM_SIZE, 2, &conf_pps, &CONF_PPS_BTN);
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

  msg = -1;
  update_objects();

  err = 0x0000;

  if (!err) {
    setup_serial_port_for_commanding();
  }

  if (!err) {
    create_WMODE(cod, conf_pps.inp, &tc);
    to_GPS(port.inp, &tc, &err);
    delay(1.0);
  }

  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  GALEPH TC PANEL
//
//~============================================================================

void create_GALEPH_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 90, w2 = 55, w3 = 35, w4 = 45, h1 = 30, h2 = 20;
  static char *t1[33], txt1[33][10];

  for (i=0; i<33; i++) {
    t1[i] = &txt1[i][0];
    sprintf(txt1[i], "%d", i);
  }
  sprintf(txt1[0], "ALL");
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "GALEPH  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  create_num_btn_hstring(
    1, x, y, w1, h2, a_e, FL_MEDIUM_SIZE, 2, &galeph_dat, &GALEPH_DAT_BTN);
  x = x + w1;
  add_frame_box(x, y, w2, h2, "Satellite", FL_NORMAL_SIZE);
  x = x + w2;
  create_num_btn_hstring(
    1, x, y, w3, h2, t1, FL_MEDIUM_SIZE, 33, &galeph_num, &GALEPH_NUM_BTN);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h2, "SEND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, send_GALEPH_TC_callback, 0);
}

//~----------------------------------------------------------------------------

void send_GALEPH_TC_callback(FL_OBJECT *obj, long data) {

  _tc tc;

  msg = -1;
  update_objects();

  err = 0x0000;

  if (!err) {
    setup_serial_port_for_commanding();
  }

  if (!err) {
    create_GALEPH(galeph_dat.inp ? 0x00000012: 0x00000011, galeph_num.inp, &tc);
    to_GPS(port.inp, &tc, &err);
    delay(1.0);
  }

  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  GDUMP TC PANEL
//
//~============================================================================

void create_GDUMP_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 15, w3 = 45, w4 = 25, h1 = 30, h2 = 20;
  static char *t1[64], txt1[64][10];

  for (i=0; i<64; i++) {
    t1[i] = &txt1[i][0];
    sprintf(txt1[i], "%d", i+1);
  }
  
  w = 5 + w1 + 8 * w2 + 5 + w3 + w4 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "GDUMP  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  add_frame_box(x, y, w1, h2, "Adr", FL_NORMAL_SIZE);
  x = x + w1;
  create_hex_btn_hstring(
    8, x, y, w2, h2, hex, 'M' + 400, &gdump_adr, &GDUMP_ADR_BTN[0]);
  x = x + 8 * w2 + 5;
  add_frame_box(x, y, w3, h2, "# Blks", FL_NORMAL_SIZE);
  x = x + w3;
  create_num_btn_hstring(
    1, x, y, w4, h2, t1, FL_MEDIUM_SIZE, 64, &gdump_blk, GDUMP_BLK_BTN);
  x = x0 + 5;
  y = y + h2;
  obj = add_normal_button(x, y, w-10, h2, "SEND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, send_GDUMP_TC_callback, 0);
}

//~----------------------------------------------------------------------------

void send_GDUMP_TC_callback(FL_OBJECT *obj, long data) {

  _tc tc;

  msg = -1;
  update_objects();

  err = 0x0000;

  if (!err) {
    setup_serial_port_for_commanding();
  }

  if (!err) {
    create_GDUMP(gdump_adr.inp, gdump_blk.inp+1, &tc);
    to_GPS(port.inp, &tc, &err);
    delay(1.0);
  }

  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  LINIT TC PANEL
//
//~============================================================================

void create_LINIT_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 15, w3 = 45, h1 = 30, h2 = 20;

  w = 5 + w1 + 8 * w2 + 5 + w3 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "LINIT  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  add_frame_box(x, y, w1, h2, "Destin.", FL_NORMAL_SIZE);
  x = x + w1;
  create_hex_btn_hstring(
    8, x, y, w2, h2, hex, 'M' + 400, &linit_dst, &LINIT_DST_BTN[0]);
  x = x + 8 * w2 + 5;
  obj = add_normal_button(x, y, w3, 2*h2, "SEND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, send_LINIT_TC_callback, 0);
  x = x0 + 5;
  y = y + h2;

  add_frame_box(x, y, w1, h2, "Length", FL_NORMAL_SIZE);
  x = x + w1;
  create_hex_btn_hstring(
    8, x, y, w2, h2, hex, 'M' + 400, &linit_len, &LINIT_LEN_BTN[0]);
}

//~----------------------------------------------------------------------------

void send_LINIT_TC_callback(FL_OBJECT *obj, long data) {
}

//~============================================================================
//
//                  LREPROM TC PANEL
//
//~============================================================================

void create_LREPROM_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 15, w3 = 45, h1 = 30, h2 = 20;

  w = 5 + w1 + 8 * w2 + 5 + w3 + 5;
  h = 5 + h1 + 3 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "LREPROM  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  add_frame_box(x, y, w1, h2, "Source", FL_NORMAL_SIZE);
  x = x + w1;
  create_hex_btn_hstring(
    8, x, y, w2, h2, hex, 'M' + 400, &lreprom_src, &LREPROM_SRC_BTN[0]);
  x = x + 8 * w2 + 5;
  obj = add_normal_button(x, y, w3, 3*h2, "SEND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, send_LINIT_TC_callback, 0);
  x = x0 + 5;
  y = y + h2;

  add_frame_box(x, y, w1, h2, "Destin.", FL_NORMAL_SIZE);
  x = x + w1;
  create_hex_btn_hstring(
    8, x, y, w2, h2, hex, 'M' + 400, &lreprom_dst, &LREPROM_DST_BTN[0]);
  x = x0 + 5;
  y = y + h2;

  add_frame_box(x, y, w1, h2, "Length", FL_NORMAL_SIZE);
  x = x + w1;
  create_hex_btn_hstring(
    8, x, y, w2, h2, hex, 'M' + 400, &lreprom_len, &LREPROM_LEN_BTN[0]);
}

//~----------------------------------------------------------------------------

void send_LREPROM_TC_callback(FL_OBJECT *obj, long data) {
}

//~============================================================================
//
//                  RESET GPS TC PANEL
//
//~============================================================================

void create_reset_GPS_panel(int x0, int y0) {
}

//~----------------------------------------------------------------------------

void send_reset_GPS_callback(FL_OBJECT *obj, long data) {
}

//~============================================================================
//
//                  GMESS TC PANEL
//
//~============================================================================

void create_GMESS_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 80, w2 = 40, w3 = 20, w4 = 35, w5 = 45, h1 = 30, h2 = 20;
  static char *t1[100], txt1[100][10];
  static char *t2[34],  txt2[34][10];
  struct {
    char *nam;
    char *_422;
    char *_1553;
    int  n;
  } table[21] = {
    {"STIME",     "2300", "03", 100},
    {"SNAVS",     "2372", "06", 100},
    {"SCOSDIR",   "2563", "11", 100},
    {"SRECEIVST", "2606", "13", 100},

    {"SNAVI",     "2317", "04", 100},
    {"SNAVN",     "233C", "05", 100},
    {"S1RAWD",    "2909", "14", 100},
    {"S2RAWD",    "291E", "15", 100},
    {"S3RAWD",    "2922", "16", 100},
    {"S1CHANST",  "2944", "17", 100},
    {"S2CHANST",  "2953", "18", 100},

    {"LSTATUS",   "10EB", "02", 100},

    {"NS1VECT",   "2C0C", "20", 34},
    {"NS2VECT",   "2C1B", "21", 34},
    {"NS3VECT",   "2C27", "22", 34},
    {"NDIAG",     "2C41", "24", 34},
    {"NRES",      "2C56", "25", 34},
    {"S1SYSTEM",  "2F24", "27", 34},
    {"S2SYSTEM",  "2F33", "28", 34},
    {"S3SYSTEM",  "2F42", "29", 34},

    {"Any Telecommand", NULL, NULL, 100}};

  for (i=0; i<100; i++) {
    t1[i] = &txt1[i][0];
    sprintf(txt1[i], "%d", i);
  }
  sprintf(txt1[0], "OFF");
  
  for (i=0; i<34; i++) {
    t2[i] = &txt2[i][0];
    sprintf(txt2[i], "%d", i*30);
  }
  sprintf(txt2[0], "OFF");
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + 21 * h2 + 4 * 5 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "GMESS  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  for (i=0; i<21; i++) {
    char **p;
    int n;
    int16 cod = 99;
    x = x0 + 5;
    w = i != 20 ? w1 : w1 + w2;
    obj = add_frame_box(x, y, w, h2, table[i].nam, FL_NORMAL_SIZE);
      fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w;
    if (i != 20) {
      cod = atoi(table[i]._1553);
      obj = add_frame_box(x, y, w2, h2, table[i]._422,  FL_NORMAL_SIZE);
      x = x + w2;
      obj = add_frame_box(x, y, w3, h2, table[i]._1553, FL_NORMAL_SIZE);
    }
    else {
      p = &t1[0];
      create_num_btn_hstring(
        1, x, y, w3, h2, p, FL_NORMAL_SIZE, 32, &cmd_id, &CMD_ID_BTN);
    }
    x = x + w3;
    if (i != 15) {
      n = table[i].n;
      p = (n == 100) ? &t1[0] : &t2[0];
      create_num_btn_hstring(
        1, x, y, w4, h2, p, FL_MEDIUM_SIZE, n, &period[i], &PERIOD_BTN[i]);
    }
    else {
      add_frame_box(x, y, w4, h2, "once", FL_NORMAL_SIZE);
      period[i].inp = 1;
    }
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "SEND", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, send_GMESS_TC_callback, i*100+cod);
    y = y + h2;
    if (i == 3 || i == 10 || i == 11 || i == 19) y = y + 5;
  }
}

//~----------------------------------------------------------------------------

void send_GMESS_TC_callback(FL_OBJECT *obj, long data) {

  int ind = data / 100;
  int cod = data % 100;
  _tc tc;

  msg = -1;
  update_objects();

  err = 0x0000;

  if (!err) {
    setup_serial_port_for_commanding();
  }

  if (!err) {
    if (cod == 99) cod = cmd_id.inp;
    create_GMESS(cod, period[ind].inp, &tc);
    to_GPS(port.inp, &tc, &err);
    delay(1.0);
  }

  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  COA TC PANEL
//
//~============================================================================

void create_COA_TC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 80, w2 = 40, w3 = 20, w4 = 35, w5 = 45, h1 = 30, h2 = 20;

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "COA  TC", FL_LARGE_SIZE);
  y = y + h1;
  
  add_frame_box(x, y, w1, h2, "S3SYSTEM",  FL_NORMAL_SIZE);
  x = x + w1;
  add_frame_box(x, y, w2, h2, "2F42",      FL_NORMAL_SIZE);
  x = x + w2;
  add_frame_box(x, y, w3, h2, "29",        FL_NORMAL_SIZE);
  x = x + w3;
  create_num_btn_hstring(
    1, x, y, w4, h2, o_o, FL_MEDIUM_SIZE, 2, &coa, &COA_BTN);
  x = x + w4;
  obj = add_normal_button(x, y, w5, h2, "SEND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, send_COA_TC_callback, 0);
}

//~----------------------------------------------------------------------------

void send_COA_TC_callback(FL_OBJECT *obj, long data) {

  _tc tc;

  msg = -1;
  update_objects();

  err = 0x0000;

  if (!err) {
    setup_serial_port_for_commanding();
  }

  if (!err) {
    create_COA(coa.inp ? 0x00010001 : 0x00010000, &tc);
    to_GPS(port.inp, &tc, &err);
    delay(1.0);
  }

  msg = 1;
  update_objects();
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
