// file Brick-3.c
//
// Improved HV Brick Medium level Controller
//
// A.Lebedev Apr-2006...

#include "bricklib.h"
#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"HV Brick Pro Controller - 3"};        // global variable
char *date  = {"25-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"

int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

int8  bus;                  // global variable
int16 geo;                  // global variable

//~----------------------------------------------------------------------------

#define status_NA -1
#define status_KO  0
#define status_OK  1

FL_FORM *MAIN = NULL;

#define NLAY 3

int ilay;

int   msg;
int16 err;
bool  chk;

//~--

SW_BTN *BUS_BTN[1];
_w w_bus;
SW_BTN *GEO_BTN[2];
_w w_geo;

//~--

FL_OBJECT *DCDC_STA[2];
bool  DCDC_sta_unk[2];
int   DCDC_sta_sta[2];
int16 DCDC_sta_wri[2];

//~--

#define NREG 4

SW_BTN *ADR_HEX_BTN[NREG][2], *ADR_NAM_BTN[NREG][1];
_w w_adr[NREG];

FL_OBJECT *REA_HEX_BOX[NREG][3], *REA_VAL_BOX[NREG];
_r r_dat[NREG];

SW_BTN *WRI_HEX_BTN[NREG][3];
FL_OBJECT *WRI_VAL_BOX[NREG];
_w w_dat[NREG];

//~--

FL_OBJECT *STA_BOX[8];
_r r_sta;

//~--

FL_OBJECT *ERR_BOX[6][8];
int16 err_adr[6] = {0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F};
_r r_err[6];

//~--

char *hex[16] = {
  "0", "1", "2", "3", "4", "5", "6", "7", 
  "8", "9", "A", "B", "C", "D", "E", "F"};
  
char *rnm[48] = {
  "LR_0",    "LR_1",    "LR_2",    "LR_3",    "LR_4",  
  "LR_5",    "LR_6",    "LR_7",    "LR_8",    "LR_9",
  "LR_10",   "LR_11",   "LR_12",   "LR_13",   "LR_14", 
  "LR_15",   "LR_16",   "LR_17",   "LR_18",   "LR_19",
  "LR_20",   "LR_21",   "LR_22",   "LR_23",   "LR_24", 
  "LR_25",   "LR_26",   "LR_27",   "LR_28",   "LR_29",
  "LR_30",   "LR_31",   "LR_32",   "LR_33",   "LR_34", 
  "LR_35",   "LR_36",   "LR_37",   "LR_38",   "LR_39",
  "DC/DC 0", "DC/DC 1", "Error 0", "Error 1", "Error 2",
  "Error 3", "Error 4", "Error 5"
};

char *enm[6][8] = {
  {"LR7",  "LR6",  "LR5",  "LR4",  "LR3",  "LR2",  "LR1",  "LR0" },
  {"LR15", "LR14", "LR13", "LR12", "LR11", "LR10", "LR9",  "LR8" },
  {"LR23", "LR22", "LR21", "LR20", "LR19", "LR18", "LR17", "LR16"},
  {"LR31", "LR30", "LR29", "LR28", "LR27", "LR26", "LR25", "LR24"},
  {"LR39", "LR38", "LR37", "LR36", "LR35", "LR34", "LR33", "LR32"},
  {"    ", "    ", "    ", "    ", "    ", "    ", "DC1",  "DC0" }
};

#if future
char *rnm_TOF[48] = {
  "LR_0",    "LR_1",    "LR_2",    "LR_3",    "LR_4",  
  "LR_5",    "LR_6",    "LR_7",    "LR_8",    "LR_9",
  "LR_10",   "LR_11",   "LR_12",   "LR_13",   "LR_14", 
  "LR_15",   "LR_16",   "LR_17",   "LR_18",   "LR_19",
  "LR_20",   "LR_21",   "LR_22",   "LR_23",   "Not Used", 
  "Not_used","Not_used","Not_used","Not_used","Not_used",
  "Not_used","Not_used","Not_used","Not_used","Not_used",
  "Not_used","Not_used","Not_used","Not_used","Not_used",
  "DC/DC",   "Not Used","Error 0", "Error 1", "Error 2",
  "Not Used","Not Used","Error 5"
};

char *enm_TOF[6][8] = {
  {"LR7",  "LR6",  "LR5",  "LR4",  "LR3",  "LR2",  "LR1",  "LR0" },
  {"LR15", "LR14", "LR13", "LR12", "LR11", "LR10", "LR9",  "LR8" },
  {"LR23", "LR22", "LR21", "LR20", "LR19", "LR18", "LR17", "LR16"},
  {"    ", "    ", "    ", "    ", "    ", "    ", "    ", "    "},
  {"    ", "    ", "    ", "    ", "    ", "    ", "    ", "    "},
  {"    ", "    ", "    ", "    ", "    ", "    ", "    ", "DC0" }
};

char *rnm_RICH[48] = {
  "LR_0",    "LR_1",    "LR_2",    "LR_3",    "LR_4",  
  "LR_5",    "LR_6",    "LR_7",    "LR_8",    "LR_9",
  "LR_10",   "LR_11",   "LR_12",   "LR_13",   "LR_14", 
  "LR_15",   "LR_16",   "LR_17",   "LR_18",   "LR_19",
  "LR_20",   "LR_21",   "LR_22",   "LR_23",   "LR_24", 
  "LR_25",   "LR_26",   "LR_27",   "LR_28",   "LR_29",
  "LR_30",   "LR_31",   "LR_32",   "LR_33",   "LR_34", 
  "LR_35",   "LR_36",   "LR_37",   "LR_38",   "LR_39",
  "DC/DC",   "Not Used","Error 0", "Error 1", "Error 2",
  "Error 3", "Error 4", "Error 5"
};

char *enm_RICH[6][8] = {
  {"LR7",  "LR6",  "LR5",  "LR4",  "LR3",  "LR2",  "LR1",  "LR0" },
  {"LR15", "LR14", "LR13", "LR12", "LR11", "LR10", "LR9",  "LR8" },
  {"LR23", "LR22", "LR21", "LR20", "LR19", "LR18", "LR17", "LR16"},
  {"LR31", "LR30", "LR29", "LR28", "LR27", "LR26", "LR25", "LR24"},
  {"LR39", "LR38", "LR37", "LR36", "LR35", "LR34", "LR33", "LR32"},
  {"    ", "    ", "    ", "    ", "    ", "    ", "DC1",  "DC0" }
};

char *rnm_ECAL[48] = {
  "LR_0",    "LR_1",    "LR_2",    "LR_3",    "LR_4",  
  "LR_5",    "LR_6",    "LR_7",    "LR_8",    "LR_9",
  "LR_10",   "LR_11",   "LR_12",   "LR_13",   "LR_14", 
  "LR_15",   "LR_16",   "LR_17",   "LR_18",   "LR_19",
  "LR_20",   "LR_21",   "LR_22",   "LR_23",   "LR_24", 
  "LR_25",   "LR_26",   "LR_27",   "LR_28",   "LR_29",
  "LR_30",   "LR_31",   "LR_32",   "LR_33",   "LR_34", 
  "LR_35",   "LR_36",   "LR_37",   "LR_38",   "LR_39",
  "DC/DC 0", "DC/DC 1", "Error 0", "Error 1", "Error 2",
  "Error 3", "Error 4", "Error 5"
};

char *enm_ECAL[6][8] = {
  {"LR7",  "LR6",  "LR5",  "LR4",  "LR3",  "LR2",  "LR1",  "LR0" },
  {"LR15", "LR14", "LR13", "LR12", "LR11", "LR10", "LR9",  "LR8" },
  {"LR23", "LR22", "LR21", "LR20", "LR19", "LR18", "LR17", "LR16"},
  {"LR31", "LR30", "LR29", "LR28", "LR27", "LR26", "LR25", "LR24"},
  {"LR39", "LR38", "LR37", "LR36", "LR35", "LR34", "LR33", "LR32"},
  {"    ", "    ", "    ", "    ", "    ", "    ", "DC1",  "DC0" }
};
#endif

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_change_layout_panel(int x0, int y0);
void change_layout_callback(FL_OBJECT *obj, long data);

void create_address_panel(int x0, int y0);
void INIT_callback(FL_OBJECT *obj, long data);
void update_address_panel(void);

void create_set_DCDC_panel(int x0, int y0, int DCDC);
void set_DCDC_callback(FL_OBJECT *obj, long data);
void update_set_DCDC_panel(int DCDC);

void create_channels_panel(int x0, int y0);
void channel_address_callback(int32 i);
void channel_read_callback(FL_OBJECT *obj, long data);
void channel_write_callback(FL_OBJECT *obj, long data);
void channel_reset_callback(FL_OBJECT *obj, long data);
void update_channels_panel(void);

void create_status_read_panel(int x0, int y0);
void status_read_callback(FL_OBJECT *obj, long data);
void update_status_read_panel(void);

void create_error_readout_panel(int x0, int y0);
void error_readout_callback(FL_OBJECT *obj, long data);
void update_error_readout_panel(void);

void create_shutdown_panel(int x0, int y0);
void shutdown_callback(FL_OBJECT *obj, long data);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 670, HH = 470;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;
  
  create_change_layout_panel(0, 45);
  create_address_panel(    175, 45);

  create_set_DCDC_panel(     0,  90, 0);
  create_set_DCDC_panel(   155,  90, 1);
  create_channels_panel(     0, 160);
  create_status_read_panel(  0, 370);

  create_error_readout_panel(310,  90);
  create_shutdown_panel(     310, 360);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  w_bus.inp = 0x4;
  w_bus.unk = TRUE;

  w_geo.inp = 0x01;
  w_geo.unk = TRUE;
  
  for (i=0; i<2; i++) DCDC_sta_unk[i] = TRUE;

  for (i=0; i<NREG; i++) {
    w_adr[i].inp = 0x00;
    w_adr[i].unk = TRUE;
    w_dat[i].inp = 0x00;
    w_dat[i].unk = TRUE;
    r_dat[i].unk = TRUE;
    r_dat[i].sta = status_NA;
  }

  r_sta.unk = TRUE;
  r_sta.sta = status_NA;

  for (i=0; i<6; i++) {
    r_err[i].unk = TRUE;
    r_err[i].sta = status_NA;
  }
  
  ilay = 0;
  read_dat_config_file(dat_config_file_name);
  ilay = MIN(ilay, NLAY-1);

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

  FILE *file;
  int i;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(&ilay, sizeof(ilay), 1, file);
    fread(&w_bus.inp, sizeof(w_bus.inp), 1, file);
    fread(&w_geo.inp, sizeof(w_geo.inp), 1, file);
    for (i=0; i<NREG; i++) {
      fread(&w_adr[i].inp, sizeof(w_adr[i].inp), 1, file);
      fread(&w_dat[i].inp, sizeof(w_dat[i].inp), 1, file);
    }
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
    fwrite(&ilay, sizeof(ilay), 1, file);
    fwrite(&w_bus.inp, sizeof(w_bus.inp), 1, file);
    fwrite(&w_geo.inp, sizeof(w_geo.inp), 1, file);
    for (i=0; i<NREG; i++) {
      fwrite(&w_adr[i].inp, sizeof(w_adr[i].inp), 1, file);
      fwrite(&w_dat[i].inp, sizeof(w_dat[i].inp), 1, file);
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//
//   Change Layout Panel
//
//
//~============================================================================

void create_change_layout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h, w1 = 55, h1 = 35;
  char *txt[3] = {"TOF", "RICH", "ECAL"};

  w = 5 + 3 * w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  for (i=0; i<3; i++) {
    obj = add_normal_button(x, y, w1, h1, txt[i], FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, change_layout_callback, i);
    x = x + w1;
  }
}

//~----------------------------------------------------------------------------

void change_layout_callback(FL_OBJECT *obj, long data) {

  ilay = data;

  write_dat_config_file(dat_config_file_name);

  msg = -2;
  update_objects();
}

//~============================================================================
//
//      LECROY BUS, GEO ADDRESS AND LECROY INIT PANEL
//
//~============================================================================

void create_address_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w, h, w1 = 30, w2 = 40, w3 = 55, h1 = 15, h2 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x,    y, w1, h1, "Bus", FL_SMALL_SIZE);
  add_frame_box(x+w1, y, w2, h1, "GEO", FL_SMALL_SIZE);

  y = y + h1;
  create_hex_btn_hstring(1, x, y, w1, h2, hex, FL_MEDIUM_SIZE, &w_bus, &BUS_BTN[0]);
  BUS_BTN[0]->n = 8;

  x = x + w1;
  create_hex_btn_hstring(2, x, y, w2/2, h2, hex, FL_MEDIUM_SIZE, &w_geo, &GEO_BTN[0]);
  GEO_BTN[0]->n = 2;
  
  x = x0 + w;
  y = y0;
  obj = add_normal_button(x, y, w3, 5+h1+h2+5, "INIT\nLeCroy", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, INIT_callback, 0);
}

//~----------------------------------------------------------------------------

void INIT_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  chk = 0;
  init_LeCroy_bus(bus, &err);
  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_address_panel(void) {

  bus = w_bus.inp;
  geo = w_geo.inp;
}

//~============================================================================
//
//      SET DC/DC COMMAND PANEL
//
//~============================================================================

void create_set_DCDC_panel(int x0, int y0, int DCDC) {

  FL_OBJECT *obj;
  int x, y;
  int w, h, w1 = 90, w2 = 55, h1 = 20;
  char *txt[2] = {"Set DCDC 0(16-39)", "Set DCDC 1(0-15)"};
  
  w = 5 + w1 + w2 + 5;
  h = 5 + 3 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2, h1, txt[DCDC], FL_NORMAL_SIZE);

  x = x0 + 5;
  y = y + h1;
  DCDC_STA[DCDC] =
  add_data_box(x, y, w1, 2*h1, " ", FL_HUGE_SIZE);

  x = x + w1;
  obj = add_normal_button(x, y, w2, h1, "ON", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, set_DCDC_callback, 2*DCDC+1);

  y = y + h1;
  obj = add_normal_button(x, y, w2, h1, "OFF", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, set_DCDC_callback, 2*DCDC+0);
    
  r_sta.was = malloc(4);
  r_sta.rea = malloc(4);
}

//~----------------------------------------------------------------------------

void set_DCDC_callback(FL_OBJECT *obj, long data) {

  int DCDC  = data / 2;
  bool ONOFF = data % 2;
  int8 stat;
  _r *r = &r_sta;

  msg = -1;
  update_objects();

  chk = 0;

  *r->was = *r->rea;
  
  brick_set_DCDC(DCDC, ONOFF, &stat, &chk, &err);
  
  if (!err && !chk) {
    DCDC_sta_unk[DCDC] = FALSE;
    DCDC_sta_sta[DCDC] = status_OK;
    DCDC_sta_wri[DCDC] = ONOFF;
    *r->rea = stat;
    if (r->unk) *r->was = *r->rea;
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    DCDC_sta_sta[DCDC] = status_KO;
    r->sta = status_KO;
  }
  
  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_set_DCDC_panel(int DCDC) {

  FL_OBJECT *obj;
  char *txt[2] = {"OFF", "ON"};
    
  obj = DCDC_STA[DCDC];
  if (DCDC_sta_unk[DCDC]) {
    fl_set_object_label(obj, "?");
    fl_set_object_color(obj, FL_YELLOW, 0);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
  else {
    fl_set_object_label(obj, txt[DCDC_sta_wri[DCDC]]);
    switch (DCDC_sta_sta[DCDC]) {
      case status_NA:
        fl_set_object_color(obj, FL_YELLOW, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      break;
      case status_OK:
        if (DCDC_sta_wri[DCDC]) {
          fl_set_object_color(obj, FL_RED, 0);
          fl_set_object_lcolor(obj, FL_YELLOW);
        }
        else {
          fl_set_object_color(obj, FL_GREEN, 0);
          fl_set_object_lcolor(obj, FL_BLACK);
        }
      break;
      case status_KO:
        fl_set_object_color(obj, FL_BLACK, 0);
        fl_set_object_lcolor(obj, FL_WHITE);
      break;
    }
  }
}

//~============================================================================
//
//      CHANNEL READ / WRITE / RESET COMMAND PANEL
//
//~============================================================================

void create_channels_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h, size;
  int w1 = 15, w2 = 85, w3 = 60, w4 = 40, h1 = 20;
    
  w = 5 + 2 * w1 + w2 + 3 * w1 + w3 + 2 * w4 + 5;
  h = 5 + 2 * h1 + NREG * 2 * h1 +5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  size = FL_NORMAL_SIZE;

  w = w - 10;
  add_frame_box(x, y, w, h1, "Channel Read / Write / Reset", size);

  y = y + h1;

  x = x0 + 5;
  add_frame_box(x, y, 2*w1, h1,   "Adr", size);
  x = x + 2 * w1;
  
  add_frame_box(x, y, w2,   h1,   "Name", size);
  x = x + w2;

  add_frame_box(x, y, 3*w1, h1,   "Code", size);
  x = x + 3 * w1;

  add_frame_box(x, y, w3,   h1,   "Voltage", size);
  x = x + w3;

  add_frame_box(x, y, 2*w4, h1,   "Command", size);
  x = x + 2 * w4;

  y = y + h1;

  size = FL_MEDIUM_SIZE;
  
  for (i=0; i<NREG; i++) {
    int j;
    x = x0 + 5;

    create_hex_btn_hstring(2, x, y, w1, 2*h1, hex, size, &w_adr[i], &ADR_HEX_BTN[i][0]);
    ADR_HEX_BTN[i][0]->n = 3;
    for (j=0; j<2; j++) {
      ADR_HEX_BTN[i][j]->call = channel_address_callback;
      ADR_HEX_BTN[i][j]->data = i;
    }
    x = x + 2 * w1;

    create_num_btn_hstring(1, x, y, w2, 2*h1, rnm, size, 48, &w_adr[i], &ADR_NAM_BTN[i][0]);
    ADR_NAM_BTN[i][0]->call = channel_address_callback;
    ADR_NAM_BTN[i][0]->data = i;
    x = x + w2;

    create_data_box_hstring(3, x, y,    w1, h1,      size,            &REA_HEX_BOX[i][0]);
    create_hex_btn_hstring( 3, x, y+h1, w1, h1, hex, size, &w_dat[i], &WRI_HEX_BTN[i][0]);
    WRI_HEX_BTN[i][0]->n = 4;
    x = x + 3 * w1;

    create_data_box_hstring(1, x, y,    w3, h1,      size,            &REA_VAL_BOX[i]);
    create_data_box_hstring(1, x, y+h1, w3, h1,      size,            &WRI_VAL_BOX[i]);
    x = x + w3;

    obj = add_normal_button(x, y,    w4, h1,   "GET", size);
      fl_set_object_callback(obj, channel_read_callback, i);
    obj = add_normal_button(x, y+h1, w4, h1,   "SET", size);
      fl_set_object_callback(obj, channel_write_callback, i);
    x = x + w4;

    obj = add_normal_button(x, y,    w4, 2*h1, "RST", size);
      fl_set_object_callback(obj, channel_reset_callback, i);
    x = x + w4;

    r_dat[i].was = malloc(4);
    r_dat[i].rea = malloc(4);
    
    y = y + 2 * h1;
  }
}

//~----------------------------------------------------------------------------

void channel_address_callback(int32 i) {

  r_dat[i].unk = TRUE;

  SW_BTN_CALLBACK(1);
}

//~----------------------------------------------------------------------------

void channel_read_callback(FL_OBJECT *obj, long data) {

  int i = data;
  int16 dat;
  _r *r = &r_dat[i];
  
  msg = -1;
  update_objects();

  chk = 0;

  *r->was = *r->rea;
  
  brick_channel_read(w_adr[i].inp, &dat, &chk, &err);

  if (!err && !chk) {
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

//~----------------------------------------------------------------------------

void channel_write_callback(FL_OBJECT *obj, long data) {

  int i = data;
  _w *w = &w_dat[i];
  
  msg = -1;
  update_objects();

  chk = 0;

  brick_channel_write(w_adr[i].inp, w->inp, &chk, &err);

  if (!err && !chk) {
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

void channel_reset_callback(FL_OBJECT *obj, long data) {

  int i = data;
  
  msg = -1;
  update_objects();

  chk = 0;

  brick_channel_reset(w_adr[i].inp, &chk, &err);

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_channels_panel(void) {

  int i;
    
  for (i=0; i<NREG; i++) {
    _r ww_dat;
    float val;
    char b[80];
    show_bin_btn_string(2, &w_adr[i],          &ADR_HEX_BTN[i][0]);
    show_bin_btn_string(1, &w_adr[i],          &ADR_NAM_BTN[i][0]);
    show_hex_box_string(3, &r_dat[i], hex,     &REA_HEX_BOX[i][0]);
//  val = *r_dat[i].rea;
    val = *r_dat[i].rea / 1024.0 * 895.0;  // ad hoc Stefano conversion
//  sprintf(b, "%6.1f", val);
    sprintf(b, "%6.0f", val);              // they want integers
    show_num_box_string(1, &r_dat[i], NULL, b, &REA_VAL_BOX[i]);
    show_bin_btn_string(3, &w_dat[i],          &WRI_HEX_BTN[i][0]);
//  val =  w_dat[i].inp;
    val =  w_dat[i].inp / 1024.0 * 895.0;  // ad hoc Stefano conversion
//  sprintf(b, "%6.1f", val);
    sprintf(b, "%6.0f", val);              // they want integers
    ww_dat.unk = FALSE;
    ww_dat.sta = status_OK;
    ww_dat.rea = &w_dat[i].inp;
    ww_dat.was = &w_dat[i].wri;
    show_num_box_string(1, &ww_dat,   NULL, b, &WRI_VAL_BOX[i]);
  }
}

//~============================================================================
//
//      STATUS READ COMMAND PANEL
//
//~============================================================================

void create_status_read_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w, h, w1 = 25, w2 = 75, h1 = 20;
  char *txt[8] = {" ", "1", "2", "3", "4", "5", "0", "1"};
  int i;
    
  w = 5 + 8 * w1 + 5 * 5 + w2 + 5;
  h = 5 + 3 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, 2*h1,     "A",        FL_NORMAL_SIZE);
  x = x + w1 + 5;
  
  obj = add_frame_box(x, y, 5*w1+5, h1,   "LR Group", FL_NORMAL_SIZE);
  x = x + 5 * w1 + 5 + 5;

  obj = add_frame_box(x, y, 2*w1+5, h1,   "DCDC", FL_NORMAL_SIZE);
  x = x + 2 * w1 + 5 + 5;

  obj = add_normal_button(x, y, w2, 3*h1, "STATUS\nREAD", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, status_read_callback, 0);

  x = x0 + 5; 
  y = y + h1;

  for (i=0; i<8; i++) {
    if (i) obj = add_frame_box(x, y, w1, h1, txt[i], FL_NORMAL_SIZE);
    STA_BOX[i] =
    obj = add_data_box(x, y+h1, w1, h1, " ", FL_MEDIUM_SIZE);
    x = x + w1;
    if (i == 0 || i == 2 || i == 5 || i == 6) x = x + 5;
    r_sta.was = malloc(4);
    r_sta.rea = malloc(4);
  }
}

//~----------------------------------------------------------------------------

void status_read_callback(FL_OBJECT *obj, long data) {

  int8 stat;
  _r *r = &r_sta;
  
  msg = -1;
  update_objects();

  chk = 0;

  *r->was = *r->rea;
  
  brick_status_read(&stat, &chk, &err);
  
  if (!err && !chk) {
    *r->rea = stat;
    if (r->unk) *r->was = *r->rea;
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
printf("status = %02hhX\n", *r_sta.rea);

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_status_read_panel(void) {

  show_bit_box_string(8, &r_sta, hex, &STA_BOX[0]);
}

//~============================================================================
//
//      ERROR READOUT PANEL
//
//~============================================================================

void create_error_readout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 20, w2 = 35, w3 = 50, h1 = 20;
    
  w = 5 + w1 + 8 * w2 + w3 + 5;
  h = 5 + 13 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  w = w - 10;
  add_frame_box(x, y, w, h1, "Error Readout", FL_NORMAL_SIZE);

  y = y + h1;

  for (i=0; i<6; i++) {
    x = x0 + 5;
    create_frame_box_hstring(1, x, y,    w1, 2*h1, &hex[i],    FL_NORMAL_SIZE, 'C');
    x = x + w1;
    create_frame_box_hstring(8, x, y,    w2,   h1, &enm[i][0], FL_NORMAL_SIZE, 'C');
    create_data_box_hstring( 8, x, y+h1, w2,   h1,             FL_NORMAL_SIZE, &ERR_BOX[i][0]);
    x = x + 8 * w2;
    obj = add_normal_button(x, y, w3, 2*h1, "READ", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, error_readout_callback, i);
    y = y + 2 * h1;
    r_err[i].was = malloc(4);
    r_err[i].rea = malloc(4);
  }
}

//~----------------------------------------------------------------------------

void error_readout_callback(FL_OBJECT *obj, long data) {

  int i = data;
  int16 cod;
  _r *r = &r_err[i];
  
  msg = -1;
  update_objects();

  chk = 0;

  *r->was = *r->rea;

  brick_channel_read(err_adr[i], &cod, &chk, &err);

cod = 1 << i;

  if (!err && !chk) {
    *r->rea = cod;
    if (r->unk) *r->was = *r->rea;
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

printf("error = %02hhX (fake)\n", *r_err[i].rea);

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_error_readout_panel(void) {

  int i;
  
  for (i=0; i<6; i++) {
    show_bit_box_string(8, &r_err[i], hex, &ERR_BOX[i][0]);
  }
}

//~============================================================================
//
//      SHUTDOWN COMMAND PANEL
//
//~============================================================================

void create_shutdown_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 350, h1 = 70;
    
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "SHUT DOWN", FL_HUGE_SIZE);
    fl_set_object_callback(obj, shutdown_callback, 0);
}

//~----------------------------------------------------------------------------

void shutdown_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  chk = 0;

  brick_shutdown(&chk, &err);
  
  msg = 1;
  update_objects();
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_address_panel();
  update_set_DCDC_panel(0);
  update_set_DCDC_panel(1);
  update_channels_panel();
  update_status_read_panel();
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
