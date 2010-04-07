// file Brick-2.c
//
// HV Brick Medium level Controller
//
// A.Lebedev Apr-2005...
//

#include "bricklib.h"
#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"HV Brick Pro Controller - 2"};        // global variable
char *date  = {"22-Dec-07"};                          // global variable

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

SW_BTN *REA_ADR_HEX_BTN[2], *REA_ADR_NAM_BTN[1];
_w w_rea_adr;

FL_OBJECT *CHA_REA_COD[3], *CHA_REA_VAL;
bool  cha_rea_unk;
int   cha_rea_sta;
int16 cha_rea_cod;
float cha_rea_val;

//~--

SW_BTN *CHA_WRI_HEX[2], *CHA_WRI_TXT;
int16 cha_wri_adr;

SW_BTN *CHA_WRI_COD[3];
FL_OBJECT *CHA_WRI_VAL;

bool  cha_wri_unk;
int   cha_wri_sta;
int16 cha_wri_cod;
float cha_wri_val;

FL_OBJECT *STATUSS[8];
bool  status_unk;
int   status_sta;
int16 status_rea[8];

SW_BTN *CHA_RST_HEX[2], *CHA_RST_TXT;
int16 cha_rst_adr;

FL_OBJECT *ERR[6][8];
int16 err_adr[6] = {0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F};
bool  err_unk[6];
int   err_sta[6];
int16 err_rea[6][8];

char *hex[16] = {
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"
};
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_change_layout_panel(int x0, int y0);
void change_layout_callback(FL_OBJECT *obj, long data);

void create_address_panel(int x0, int y0);
void INIT_callback(FL_OBJECT *obj, long data);
void update_address_panel(void);

void create_set_DCDC_panel(int x0, int y0, int DCDC);
void set_DCDC_callback(FL_OBJECT *obj, long data);
void update_set_DCDC_panel(int DCDC);

void create_channel_read_panel(int x0, int y0);
void channel_read_adr_callback(int32 data);
void channel_read_nam_callback(int32 data);
void channel_read_callback(FL_OBJECT *obj, long data);
void update_channel_read_panel(void);

void create_channel_write_panel(int x0, int y0);
void channel_write_adr_callback(int32 data);
void channel_write_nam_callback(int32 data);
void channel_write_cod_callback(int32 data);
void channel_write_callback(FL_OBJECT *obj, long data);
void update_channel_write_panel(void);

void create_status_read_panel(int x0, int y0);
void status_read_callback(FL_OBJECT *obj, long data);
void update_status_read_panel(void);

void create_channel_reset_panel(int x0, int y0);
void channel_reset_adr_callback(int32 data);
void channel_reset_nam_callback(int32 data);
void channel_reset_callback(FL_OBJECT *obj, long data);

void create_shutdown_panel(int x0, int y0);
void shutdown_callback(FL_OBJECT *obj, long data);

void create_error_readout_panel(int x0, int y0);
void error_readout_callback(FL_OBJECT *obj, long data);
void update_error_readout_panel(void);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 670, HH = 470;

  ilay = 0;
  read_dat_config_file(dat_config_file_name);
  ilay = MIN(ilay, NLAY-1);

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;
  
  create_change_layout_panel(0, 45);
  create_address_panel(    175, 45);

  create_set_DCDC_panel(     0,  90, 0);
  create_set_DCDC_panel(   155,  90, 1);
  create_channel_read_panel( 0, 160);
  create_channel_write_panel(0, 230);
  create_status_read_panel(  0, 300);
  create_channel_reset_panel(0, 370);
  create_shutdown_panel(   190, 370);

  create_error_readout_panel(310, 90);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  
  w_bus.inp = 0x4;
  w_bus.unk = TRUE;

  w_geo.inp = 0x01;
  w_geo.unk = TRUE;
  
  for (i=0; i<2; i++) DCDC_sta_unk[i] = TRUE;

  w_rea_adr.inp = 0x00;
  w_rea_adr.unk = TRUE;

  cha_rea_unk = TRUE;
  cha_wri_unk = TRUE;
  status_unk  = TRUE;
  for (i=0; i<6; i++) err_unk[i] = TRUE;
  
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
    fread(&ilay, sizeof(ilay), 1, file);
    fread(&w_bus.inp, sizeof(w_bus.inp), 1, file);
    fread(&w_geo.inp, sizeof(w_geo.inp), 1, file);
    fread(&w_rea_adr.inp, sizeof(w_rea_adr.inp), 1, file);
    fread(&cha_wri_adr, sizeof(cha_wri_adr), 1, file);
    fread(&cha_wri_cod, sizeof(cha_wri_cod), 1, file);
    fread(&cha_rst_adr, sizeof(cha_rst_adr), 1, file);
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
    fwrite(&ilay, sizeof(ilay), 1, file);
    fwrite(&w_bus.inp, sizeof(w_bus.inp), 1, file);
    fwrite(&w_geo.inp, sizeof(w_geo.inp), 1, file);
    fwrite(&w_rea_adr.inp, sizeof(w_rea_adr.inp), 1, file);
    fwrite(&cha_wri_adr, sizeof(cha_wri_adr), 1, file);
    fwrite(&cha_wri_cod, sizeof(cha_wri_cod), 1, file);
    fwrite(&cha_rst_adr, sizeof(cha_rst_adr), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~===============================================================================
//
//
//   Change Layout Panel
//
//
//~===============================================================================

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

//~-------------------------------------------------------------------------------

void change_layout_callback(FL_OBJECT *obj, long data) {

  ilay = data;

  write_dat_config_file(dat_config_file_name);
  
  msg = -2;
  update_objects();

//MAIN = create_main_form();
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
}

//~----------------------------------------------------------------------------

void set_DCDC_callback(FL_OBJECT *obj, long data) {

  int i;
  int DCDC  = data / 2;
  bool ONOFF = data % 2;
  int8 stat;

  msg = -1;
  update_objects();

  chk = 0;

  brick_set_DCDC(DCDC, ONOFF, &stat, &chk, &err);
  
  if (!err && !chk) {
    DCDC_sta_unk[DCDC] = FALSE;
    DCDC_sta_sta[DCDC] = status_OK;
    DCDC_sta_wri[DCDC] = ONOFF;
    status_unk = FALSE;
    status_sta = status_OK;
    for (i=0; i<8; i++) status_rea[i] = (stat & (0x80 >> i)) ? 1 : 0;
  }
  else {
    DCDC_sta_sta[DCDC] = status_KO;
    status_sta = status_KO;
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
//      CHANNEL READ COMMAND PANEL
//
//~============================================================================

void create_channel_read_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h, size;
  int w1 = 20, w2 = 85, w3 = 60, w4 = 60, w5 = 55, h1 = 20;
    
  w = 5 + 2 * w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  size = FL_NORMAL_SIZE;

  w = w - 10;
  add_frame_box(x, y, w, h1, "Channel Read", size);

  y = y + h1;

  x = x0 + 5;
  add_frame_box(x, y, 2*w1, h1,   "Addr", size);
  x = x + 2 * w1;
  
  add_frame_box(x, y, w2,   h1,   "Name", size);
  x = x + w2;

  add_frame_box(x, y, w3,   h1,   "Code", size);
  x = x + w3;

  add_frame_box(x, y, w4,   h1,   "Voltage", size);
  x = x + w4;

  obj = add_normal_button(x, y, w5, 2*h1, "READ", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, channel_read_callback, 0);
  x = x + w5;

  x = x0 + 5;
  y = y + h1;

  size = FL_MEDIUM_SIZE;
  
  create_hex_btn_hstring(2, x, y, w1, h1, hex, size, &w_rea_adr, &REA_ADR_HEX_BTN[0]);
  REA_ADR_HEX_BTN[0]->n = 3;
  x = x + 2 * w1;

  create_num_btn_hstring(1, x, y, w2, h1, rnm, size, 48, &w_rea_adr, &REA_ADR_NAM_BTN[0]);
  x = x + w2;

  for (i=0; i<3; i++) {
    CHA_REA_COD[i] =
    obj = add_data_box(x, y, w1, h1, " ", size);
    x = x + w1;
  }

  CHA_REA_VAL =
  obj = add_data_box(x, y, w4, h1, " ", size);
  x = x + 4;
}

//~----------------------------------------------------------------------------

void channel_read_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  chk = 0;

  brick_channel_read(w_rea_adr.inp, &cha_rea_cod, &chk, &err);

  if (!err && !chk) {
    cha_rea_unk = FALSE;
    cha_rea_sta = status_OK;
    cha_rea_val = (float)(cha_rea_cod) / (float)0x03FF * 900.0;
  }
  else {
    cha_rea_sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_channel_read_panel(void) {

  FL_OBJECT *ob[4] = {CHA_REA_COD[0], CHA_REA_COD[1], CHA_REA_COD[2], CHA_REA_VAL};
  int i;
  char b[80];
    
  show_bin_btn_string(2, &w_rea_adr, &REA_ADR_HEX_BTN[0]);
  show_bin_btn_string(1, &w_rea_adr, &REA_ADR_NAM_BTN[0]);


  for (i=0; i<4; i++) {
    FL_OBJECT *obj = ob[i];
    if (cha_rea_unk) {
      fl_set_object_label(obj, "?");
      fl_set_object_color(obj, FL_YELLOW, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    else {
      if (i != 3) sprintf(b, "%X", unpack16(cha_rea_cod, 0x0F00 >> 4 * i));
      else        sprintf(b, "%6.1f", cha_rea_val);
      fl_set_object_label(obj, b);
      if (cha_rea_sta == status_OK) {
        fl_set_object_color(obj, FL_GREEN, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
      else {
        fl_set_object_color(obj, FL_BLACK, 0);
        fl_set_object_lcolor(obj, FL_WHITE);
      }
    }
  }
}

//~============================================================================
//
//      CHANNEL WRITE COMMAND PANEL
//
//~============================================================================

void create_channel_write_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h, size;
  int w1 = 20, w2 = 85, w4 = 60, w5 = 55, h1 = 20;
    
  w = 5 + 2 * w1 + w2 + 3 * w1 + w4 + w5 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  x = x0 + 5;
  y = y0 + 5;
  w = w - 10;
  obj = add_frame_box(x, y, w, h1, "Channel Write", FL_NORMAL_SIZE);

  y = y + h1;

  x = x0 + 5;
  w = 2 * w1;
  obj = add_frame_box(x, y, w, h1,   "Addr", FL_NORMAL_SIZE);

  x = x + w;
  w = w2;
  obj = add_frame_box(x, y, w, h1,   "Name", FL_NORMAL_SIZE);

  x = x + w;
  w = 3 * w1;
  obj = add_frame_box(x, y, w, h1,   "Code", FL_NORMAL_SIZE);

  x = x + w;
  w = w4;
  obj = add_frame_box(x, y, w, h1,   "Voltage", FL_NORMAL_SIZE);

  x = x + w;
  w = w5;
  obj = add_normal_button(x, y, w, 2*h1, "WRITE", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, channel_write_callback, 0);

  y = y + h1;

  size = FL_MEDIUM_SIZE;
  
  x = x0 + 5;
  w = w1;
  CHA_WRI_HEX[0] = add_switch_btn(x, y, w, h1, hex, size,  3, cha_wri_adr/16);
  CHA_WRI_HEX[0]->call = channel_write_adr_callback;
  
  x =  x + w;
  w = w1;
  CHA_WRI_HEX[1] = add_switch_btn(x, y, w, h1, hex, size, 16, cha_wri_adr%16);
  CHA_WRI_HEX[1]->call = channel_write_adr_callback;
  
  x =  x + w;
  w = w2;
  CHA_WRI_TXT    = add_switch_btn(x, y, w, h1, rnm, size, 48, cha_wri_adr);
  CHA_WRI_TXT->call = channel_write_nam_callback;
  
  x = x + w;
  w = w1;
  CHA_WRI_COD[0] = add_switch_btn(x, y, w, h1, hex, size,  4, cha_wri_cod/256);
  CHA_WRI_COD[0]->call = channel_write_cod_callback;

  x = x + w;
  w = w1;
  CHA_WRI_COD[1] = add_switch_btn(x, y, w, h1, hex, size, 16, cha_wri_cod/16%16);
  CHA_WRI_COD[1]->call = channel_write_cod_callback;

  x = x + w;
  w = w1;
  CHA_WRI_COD[2] = add_switch_btn(x, y, w, h1, hex, size, 16, cha_wri_cod%16);
  CHA_WRI_COD[2]->call = channel_write_cod_callback;

  x = x + w;
  w = w4;
  CHA_WRI_VAL =
  obj = add_data_box(x, y, w, h1, " ", size);
}

//~----------------------------------------------------------------------------

void channel_write_adr_callback(int32 data) {

  SW_BTN *B = CHA_WRI_TXT;
  
  cha_wri_adr = CHA_WRI_HEX[0]->i * 16 + CHA_WRI_HEX[1]->i;
  B->i = cha_wri_adr;
  fl_set_object_label(B->obj, B->l[B->i]);
  
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void channel_write_nam_callback(int32 data) {

  SW_BTN *B;

  B = CHA_WRI_TXT;
  cha_wri_adr = B->i;

  B = CHA_WRI_HEX[0];
  B->i = cha_wri_adr / 16;
  fl_set_object_label(B->obj, B->l[B->i]);

  B = CHA_WRI_HEX[1];
  B->i = cha_wri_adr % 16;
  fl_set_object_label(B->obj, B->l[B->i]);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void channel_write_cod_callback(int32 data) {

  cha_wri_cod = CHA_WRI_COD[0]->i * 256 + 
                CHA_WRI_COD[1]->i *  16 + 
                CHA_WRI_COD[2]->i *   1;

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void channel_write_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  chk = 0;

  brick_channel_write(cha_wri_adr, cha_wri_cod, &chk, &err);

  if (!err && !chk) {
    cha_wri_unk = FALSE;
    cha_wri_sta = status_OK;
  }
  else {
    cha_wri_sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_channel_write_panel(void) {

  FL_OBJECT *obj = CHA_WRI_VAL;
  char b[80];
    
  cha_wri_val = (float)(cha_wri_cod) / (float)0x03FF * 900.0;

  sprintf(b, "%6.1f", cha_wri_val);
  fl_set_object_label(obj, b);
  fl_set_object_color(obj, FL_GREEN, 0);
  fl_set_object_lcolor(obj, FL_BLACK);
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
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  x = x0 + 5;
  y = y0 + 5;
  w = w1;
  obj = add_frame_box(x, y, w, 2*h1, "A", FL_NORMAL_SIZE);

  x = x + w + 5;
  w = 5 * w1 + 5;
  obj = add_frame_box(x, y, w, h1,   "LR Group", FL_NORMAL_SIZE);

  x = x + w + 5;
  w = 2 * w1 + 5;
  obj = add_frame_box(x, y, w, h1,   "DCDC", FL_NORMAL_SIZE);

  x = x + w + 5;
  w = w2;
  obj = add_normal_button(x, y, w, 3*h1, "STATUS\nREAD", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, status_read_callback, 0);

  x = x0 + 5; 
  y = y + h1;

  for (i=0; i<8; i++) {
    if (i) obj = add_frame_box(x, y, w1, h1, txt[i], FL_NORMAL_SIZE);
    STATUSS[i] =
    obj = add_data_box(x, y+h1, w1, h1, " ", FL_MEDIUM_SIZE);
    x = x + w1;
    if (i == 0 || i == 2 || i == 5 || i == 6) x = x + 5;
  }
}

//~----------------------------------------------------------------------------

void status_read_callback(FL_OBJECT *obj, long data) {

  int8 stat;
  int i;
  
  msg = -1;
  update_objects();

  chk = 0;

  brick_status_read(&stat, &chk, &err);
  
  if (!err && !chk) {
    status_unk = FALSE;
    status_sta = status_OK;
    for (i=0; i<8; i++) status_rea[i] = (stat & (0x80 >> i)) ? 1 : 0;
  }
  else {
    status_sta = status_KO;
  }
  
  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_status_read_panel(void) {

  FL_OBJECT *obj;
  int i;
  
  for (i=0; i<8; i++) {
    obj = STATUSS[i];
    if (status_unk) {
      fl_set_object_label(obj, "?");
      fl_set_object_color(obj, FL_YELLOW, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    else {
      if (status_rea[i]) fl_set_object_label(obj, "1");
      else               fl_set_object_label(obj, "0");
      if (status_sta == status_OK) {
        fl_set_object_color(obj, FL_GREEN, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
        if ((i == 0           &&  status_rea[i]) ||
            (1 <= i && i <= 5 && !status_rea[i]) ||
            (6 <= i && i <= 7 &&  status_rea[i])) {
          fl_set_object_color(obj, FL_YELLOW, 0);
        }
      }
      else {
        fl_set_object_color(obj, FL_BLACK, 0);
        fl_set_object_lcolor(obj, FL_WHITE);
      }
    }
  }
}

//~============================================================================
//
//      CHANNEL RESET COMMAND PANEL
//
//~============================================================================

void create_channel_reset_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h, size;
  int w1 = 20, w2 = 85, w5 = 55, h1 = 20;
    
  w = 5 + 2 * w1 + w2 + w5 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  x = x0 + 5;
  y = y0 + 5;
  w = w - 10;
  obj = add_frame_box(x, y, w, h1, "Channel Reset", FL_NORMAL_SIZE);

  y = y + h1;

  x = x0 + 5;
  w = 2 * w1;
  obj = add_frame_box(x, y, w, h1,   "Addr", FL_NORMAL_SIZE);

  x = x + w;
  w = w2;
  obj = add_frame_box(x, y, w, h1,   "Name", FL_NORMAL_SIZE);

  x = x + w;
  w = w5;
  obj = add_normal_button(x, y, w, 2*h1, "RESET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, channel_reset_callback, 0);

  y = y + h1;

  size = FL_MEDIUM_SIZE;
  
  x = x0 + 5;
  w = w1;
  CHA_RST_HEX[0] = add_switch_btn(x, y, w, h1, hex, size,  3, cha_rst_adr/16);
  CHA_RST_HEX[0]->call = channel_reset_adr_callback;
  
  x =  x + w;
  w = w1;
  CHA_RST_HEX[1] = add_switch_btn(x, y, w, h1, hex, size, 16, cha_rst_adr%16);
  CHA_RST_HEX[1]->call = channel_reset_adr_callback;
  
  x =  x + w;
  w = w2;
  CHA_RST_TXT    = add_switch_btn(x, y, w, h1, rnm, size, 48, cha_rst_adr);
  CHA_RST_TXT->call = channel_reset_nam_callback;
}

//~----------------------------------------------------------------------------

void channel_reset_adr_callback(int32 data) {

  SW_BTN *B = CHA_RST_TXT;
  
  cha_rst_adr = CHA_RST_HEX[0]->i * 16 + CHA_RST_HEX[1]->i;
  B->i = cha_rst_adr;
  fl_set_object_label(B->obj, B->l[B->i]);
  
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void channel_reset_nam_callback(int32 data) {

  SW_BTN *B;

  B = CHA_RST_TXT;
  cha_rst_adr = B->i;

  B = CHA_RST_HEX[0];
  B->i = cha_rst_adr / 16;
  fl_set_object_label(B->obj, B->l[B->i]);

  B = CHA_RST_HEX[1];
  B->i = cha_rst_adr % 16;
  fl_set_object_label(B->obj, B->l[B->i]);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void channel_reset_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  chk = 0;

  brick_channel_reset(cha_rst_adr, &chk, &err);

  msg = 1;
  update_objects();
}

//~============================================================================
//
//      SHUTDOWN COMMAND PANEL
//
//~============================================================================

void create_shutdown_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 110, h1 = 60;
    
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "SHUT\nDOWN", FL_MEDIUM_SIZE);
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

void create_error_readout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  char b[80];
  int x, y, w, h;
  int w1 = 20, w2 = 35, w3 = 50, h1 = 20;
    
  w = 5 + w1 + 8 * w2 + w3 + 5;
  h = 5 + 13 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  x = x0 + 5;
  y = y0 + 5;
  w = w - 10;
  obj = add_frame_box(x, y, w, h1, "Error Readout", FL_NORMAL_SIZE);

  y = y + h1;

  for (j=0; j<6; j++) {
    x = x0 + 5;
    sprintf(b, "%d", j);
    w = w1;
    obj = add_frame_box(x, y, w, 2*h1, b, FL_NORMAL_SIZE);
    x = x + w;
    for (i=0; i<8; i++) {
      if (j != 5) {
        sprintf(b, "LR%2d", 39-i-8*j);
      }
      else {
        if (i <= 5) sprintf(b, " ");
        else        sprintf(b, "DC%2d", 7-i);
      }
      w = w2;
      obj = add_frame_box(x, y, w, h1, b, FL_NORMAL_SIZE);
      ERR[j][i] =
      obj = add_data_box(x, y+h1, w, h1, "1", FL_MEDIUM_SIZE);
      x = x + w;
    }
    w = w3;
    obj = add_normal_button(x, y, w, 2*h1, "READ", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, error_readout_callback, j);
    x = x + w;
    y = y + 2 * h1;
  }
}

//~----------------------------------------------------------------------------

void error_readout_callback(FL_OBJECT *obj, long data) {

  int j = data;
  int16 cod;
  int i;
  
  msg = -1;
  update_objects();

  chk = 0;

  brick_channel_read(err_adr[j], &cod, &chk, &err);
  
  if (!err && !chk) {
    err_unk[j] = FALSE;
    err_sta[j] = status_OK;
    for (i=0; i<8; i++) err_rea[j][i] = (cod & (0x0080 >> i)) ? 1 : 0;
  }
  else {
    err_sta[j] = status_KO;
  }
  
  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_error_readout_panel(void) {

  FL_OBJECT *obj;
  int i, j;
  
  for (j=0; j<6; j++) {
    for (i=0; i<8; i++) {
      obj = ERR[j][i];
      if (err_unk[j]) {
        fl_set_object_label(obj, "?");
        fl_set_object_color(obj, FL_YELLOW, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
      else {
        if (err_rea[j][i]) fl_set_object_label(obj, "1");
        else               fl_set_object_label(obj, "0");
        if (err_sta[j] == status_OK) {
          fl_set_object_color(obj, FL_GREEN, 0);
          fl_set_object_lcolor(obj, FL_BLACK);
          if (err_rea[j][i]) fl_set_object_color(obj, FL_YELLOW, 0);
        }
        else {
          fl_set_object_color(obj, FL_BLACK, 0);
          fl_set_object_lcolor(obj, FL_WHITE);
        }
      }
    }
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_address_panel();
  update_set_DCDC_panel(0);
  update_set_DCDC_panel(1);
  update_channel_read_panel();
  update_channel_write_panel();
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
