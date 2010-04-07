// file DS.c
//
// Dallas Sensors Controller
//
// A.Lebedev Apr-2005...

#include "template.h"
#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"Dallas  Sensors  Controller"};        // global variable
char *date  = {"13-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

char *log_file_name = {"DS.logfile"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define status_NA -1
#define status_KO  0
#define status_OK  1

//~--

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~--

SW_BTN *SCA_BTN[8];
_w sca_bus;
bool ID_updated;

//~--

FL_OBJECT *SCA_STA[8], *SCA_NUM[9];
_r         sca_sta,     sca_num;

//~--

SW_BTN *ENA_BTN[8], *RST_BTN[8], *MAN_BTN[8], *MDW_BTN[8], *PAR_BTN[2];
_w      set_ena,     set_rst,     set_man,     set_mdw,     set_par;

//~--

FL_OBJECT *ENA_BOX[8], *STA_BOX[9], *MAN_BOX[8], *MDW_BOX[8], *MDR_BOX[8], *PAR_BOX;
_r         chk_ena,     chk_sta,     chk_man,     chk_mdw,     chk_mdr,     chk_par;

//~--

FL_OBJECT *ONE_BUS_BRO;
bool browser_unk;
SW_BTN *BUS_BTN;
_w bus_get;
int8 n0[9], n1[9], n2[9];
int64 ID[9][64];
int8  pri[9][64];
bool  ok1[9][64],   ok2[9][64];
float tmp1[9][64], tmp2[9][64];
int16              age2[9][64];

//~--

FL_OBJECT *ALL_BUS_BRO;
bool brouser_unk;
SW_BTN *PER_BTN;
_w per_get;
FL_OBJECT *AUTOREAD;
bool autoread = FALSE;
int8 n[9];
bool ok[9][64];
float tmp[9][64];
int16 age[9][64];
FILE *logfile;
bool writing  = FALSE;

//~-----

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *par[5] = {
 "1",  "2",  "4",  "8",  "16"};

char *mod[2] = {"N", "P"};

char *sta[4] = {"OK", "@2->", "@8->", "?"};

char *bus[9] = {
"1", "2", "3", "4", "5", "6", "7", "8", "9"};

char *per[9] = {"1", "2", "5", "30", "60", "120", "180", "240", "300"};

//~--

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);
void update_callback(int32 data);

void create_start_scan_panel(int x0, int y0);
void start_scan_callback(FL_OBJECT *obj, long data);
void update_start_scan_panel(void);

void create_check_scan_panel(int x0, int y0);
void check_scan_callback(FL_OBJECT *obj, long data);
void update_check_scan_panel(void);

void create_setup_readout_panel(int x0, int y0);
void setup_readout_callback(FL_OBJECT *obj, long data);
void update_setup_readout_panel(void);

void create_check_readout_panel(int x0, int y0);
void check_readout_callback(FL_OBJECT *obj, long data);
void update_check_readout_panel(void);

void create_read_id_and_data_panel(int x0, int y0);
void read_id_and_data_callback(FL_OBJECT *obj, long data);
void update_read_id_and_data_panel(void);

void create_read_temperature_panel(int x0, int y0);
void read_temperature_callback(FL_OBJECT *obj, long data);
void write_temperature_callback(FL_OBJECT *obj, long data);
void record_DS1820_data(FILE *file);
void update_read_temperature_panel(void);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 925, HH = 965;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_start_scan_panel(   0,  45);
  create_check_scan_panel(   0, 130);
  create_setup_readout_panel(0, 255);
  create_check_readout_panel(0, 465);

  create_read_id_and_data_panel(170, 45);
  create_read_temperature_panel(495, 45);

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

  ID_updated = TRUE;

  sca_bus.inp = 0x00;
  sca_bus.unk = TRUE;

  sca_sta.unk = TRUE;
  sca_num.unk = TRUE;
  sca_sta.sta = status_NA;
  sca_num.sta = status_NA;
  
  set_par.inp = 0;
  set_ena.inp = 0x00;
  set_rst.inp = 0xFF;
  set_man.inp = 0x00;
  set_mdw.inp = 0x00;
  set_par.unk = TRUE;
  set_ena.unk = TRUE;
  set_rst.unk = TRUE;
  set_man.unk = TRUE;
  set_mdw.unk = TRUE;

  chk_par.unk = TRUE;
  chk_ena.unk = TRUE;
  chk_sta.unk = TRUE;
  chk_man.unk = TRUE;
  chk_man.unk = TRUE;
  chk_mdw.unk = TRUE;
  chk_mdr.unk = TRUE;
  chk_par.sta = status_NA;
  chk_ena.sta = status_NA;
  chk_sta.sta = status_NA;
  chk_man.sta = status_NA;
  chk_man.sta = status_NA;
  chk_mdw.sta = status_NA;
  chk_mdr.sta = status_NA;

  browser_unk = TRUE;
  bus_get.inp = 0;
  bus_get.unk = TRUE;
  
  brouser_unk = TRUE;
  per_get.inp = 0;
  per_get.unk = TRUE;

  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "r");
  if (file) {
    fread(&sca_bus.inp, sizeof(sca_bus.inp), 1, file);
    fread(&set_par.inp, sizeof(set_par.inp), 1, file);
    fread(&set_ena.inp, sizeof(set_ena.inp), 1, file);
    fread(&set_man.inp, sizeof(set_man.inp), 1, file);
    fread(&set_mdw.inp, sizeof(set_mdw.inp), 1, file);
    fread(&bus_get.inp, sizeof(bus_get.inp), 1, file);
    fread(&per_get.inp, sizeof(per_get.inp), 1, file);
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
    fwrite(&sca_bus.inp, sizeof(sca_bus.inp), 1, file);
    fwrite(&set_par.inp, sizeof(set_par.inp), 1, file);
    fwrite(&set_ena.inp, sizeof(set_ena.inp), 1, file);
    fwrite(&set_man.inp, sizeof(set_man.inp), 1, file);
    fwrite(&set_mdw.inp, sizeof(set_mdw.inp), 1, file);
    fwrite(&bus_get.inp, sizeof(bus_get.inp), 1, file);
    fwrite(&per_get.inp, sizeof(per_get.inp), 1, file);
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

//~----------------------------------------------------------------------------

void update_callback(int32 data) {

  update_objects();
}

//~============================================================================
//
//                  START SCAN
//
//~============================================================================

void create_start_scan_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, h1 = 15, h2 = 20, h3 = 15, h4 = 25;
  
  w = 5 + 8 * w1 + 5;
  h = 5 + h1 + h2 + h3 + h4 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  w = w - 10;
  obj = add_frame_box(x, y, w, h1, "Scan Bus", FL_SMALL_SIZE);
  y = y + h1;

  create_bit_btn_hstring(8, x, y, w1, h2, hex, FL_MEDIUM_SIZE, &sca_bus, &SCA_BTN[0]);
  y = y + h2;
  
  x = x0 + 5;
  create_bit_num_string(-8, 8, x, y, w1, h3, FL_SMALL_SIZE);
  y = y + h3;
  
  obj = add_normal_button(x, y, w, h4, "START SCAN", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, start_scan_callback, 0);
  y = y + h4;
}

//~----------------------------------------------------------------------------

void start_scan_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  start_DS1820_scan(sca_bus.inp, &err);

  if (!err) {
    sca_bus.wri = sca_bus.inp;
    sca_bus.unk = FALSE;
  }
  else {
    sca_bus.unk = TRUE;
  }
    
  ID_updated = TRUE;

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_start_scan_panel(void) {

  show_bin_btn_string(8, &sca_bus, &SCA_BTN[0]);
}

//~============================================================================
//
//                  CHECK SCAN
//
//~============================================================================

void create_check_scan_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, h1 = 15, h2 = 15, h3 = 15, h4 = 15, h5 = 15, h6 = 15, h7 = 25;
  
  w = 5 + 8 * w1 + 5;
  h = 5 + h1 + h2 + h3 + h4 + h5 + h6 + h7 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;

  w = w - 10;
  obj = add_frame_box(x, y, w, h1, "Scan Status", FL_SMALL_SIZE);
  y = y + h2;

  create_data_box_hstring(8, x, y, w1, h2, FL_NORMAL_SIZE, SCA_STA);
  y = y + h3;
  
  sca_sta.was = malloc(4);
  sca_sta.rea = malloc(4);

  obj = add_frame_box(x, y, w, h3, "Number of Sensors Found", FL_SMALL_SIZE);
  y = y + h4;

  create_data_box_hstring(8, x, y, w1, h4, FL_NORMAL_SIZE, &SCA_NUM[1]);
  y = y + h5;
  
  create_bit_num_string(-8, 8, x, y, w1, h5, FL_SMALL_SIZE);
  y = y + h6;
  
  obj = add_frame_box(x, y, 7*w1, h6, "Nb. of Sensors on Bus 9", FL_SMALL_SIZE);

  x = x + 7 * w1;
  SCA_NUM[0] =
  obj = add_data_box(x, y, w1, h6, " ", FL_NORMAL_SIZE);
  y = y + h1;

  sca_num.was = malloc(9*4);
  sca_num.rea = malloc(9*4);

  x = x0 + 5;
  obj = add_normal_button(x, y, w, h7, "CHECK  SCAN", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, check_scan_callback, 0);
  y = y + h7;
}

//~----------------------------------------------------------------------------

void check_scan_callback(FL_OBJECT *obj, long data) {

  int i;
  int8 sta, num[9];
  
  msg = -1;
  update_objects();

  *sca_sta.was = *sca_sta.rea;
  for (i=0; i<9; i++) sca_num.was[i] = sca_num.rea[i];
  
  check_DS1820_scan(&sta, num, &err);
  
  if (!err) {
    *sca_sta.rea = sta;
    for (i=0; i<9; i++) sca_num.rea[i] = num[i];
    if (sca_sta.unk) *sca_sta.was = *sca_sta.rea;
    if (sca_num.unk) for (i=0; i<9; i++) sca_num.was[i] = sca_num.rea[i];
    sca_sta.unk = FALSE;
    sca_num.unk = FALSE;
    sca_sta.sta = status_OK;
    sca_num.sta = status_OK;
  }
  else {
    sca_sta.sta = status_KO;
    sca_num.sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_check_scan_panel(void) {

  show_bit_box_string(8, &sca_sta, NULL, &SCA_STA[0]);
  show_num_box_string(-9, &sca_num, NULL, "%d", &SCA_NUM[0]);
}

//~============================================================================
//
//                  SETUP READOUT
//
//~============================================================================

void create_setup_readout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 25, h1 = 15, h2 = 20, h3 = 15, h4 = 25;
  
  w = 5 + 8 * w1 + 5;
  h = 5 + h2 + 4 * (h1 + h2) + h3 + h4 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;

  w = 8 * w1;
  obj = add_frame_box(x, y, w, h1, "Enable Bus", FL_SMALL_SIZE);
  y = y + h1;
  create_bit_btn_hstring(8, x, y, w1, h2, hex, FL_MEDIUM_SIZE, &set_ena, &ENA_BTN[0]);
  y = y + h2;
  
  obj = add_frame_box(x, y, w, h1, "Reset Data", FL_SMALL_SIZE);
  y = y + h1;
  create_bit_btn_hstring(8, x, y, w1, h2, hex, FL_MEDIUM_SIZE, &set_rst, &RST_BTN[0]);
  y = y + h2;

  obj = add_frame_box(x, y, w, h1, "Manual Readout Mode Control", FL_SMALL_SIZE);
  y = y + h1;
  create_bit_btn_hstring(8, x, y, w1, h2, hex, FL_MEDIUM_SIZE, &set_man, &MAN_BTN[0]);
  y = y + h2;

  obj = add_frame_box(x, y, w, h1, "Set Readout Mode", FL_SMALL_SIZE);
  y = y + h1;
  create_bit_btn_hstring(8, x, y, w1, h2, mod, FL_MEDIUM_SIZE, &set_mdw, &MDW_BTN[0]);
  y = y + h2;

  create_bit_num_string(-8, 8, x, y, w1, h3, FL_SMALL_SIZE);
  y = y + h3;

  w = w - 2 * w2;
  obj = add_frame_box(x, y, w, h2, "Parallel Readout", FL_SMALL_SIZE);
  x = x + w;

  create_dec_btn_hstring(2, x, y, w2, h2, hex, FL_MEDIUM_SIZE, &set_par, &PAR_BTN[0]);
  y = y + h2;

  x = x0 + 5;
  obj = add_normal_button(x, y, 8*w1, h4, "SETUP READOUT", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, setup_readout_callback, 0);
  y = y + h4;
}


//~----------------------------------------------------------------------------

void setup_readout_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  setup_DS1820_readout(
    set_ena.inp, set_rst.inp, set_man.inp, set_mdw.inp, set_par.inp, &err);

  if (!err) {
    set_ena.wri = set_ena.inp;
    set_rst.wri = set_rst.inp;
    set_man.wri = set_man.inp;
    set_mdw.wri = set_mdw.inp;
    set_par.wri = set_par.inp;
    set_ena.unk = FALSE;
    set_rst.unk = FALSE;
    set_man.unk = FALSE;
    set_mdw.unk = FALSE;
    set_par.unk = FALSE;
  }
  else {
    set_ena.unk = TRUE;
    set_rst.unk = TRUE;
    set_man.unk = TRUE;
    set_mdw.unk = TRUE;
    set_par.unk = TRUE;
  }
    
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_setup_readout_panel(void) {

  show_dec_btn_string(2, &set_par, &PAR_BTN[0]);
  if (set_par.inp > 64) {
    fl_set_object_color(PAR_BTN[0]->obj, FL_RED, FL_YELLOW);
    fl_set_object_color(PAR_BTN[1]->obj, FL_RED, FL_YELLOW);
  }
    
  show_bin_btn_string(8, &set_ena, &ENA_BTN[0]);
  show_bin_btn_string(8, &set_rst, &RST_BTN[0]);
  show_bin_btn_string(8, &set_man, &MAN_BTN[0]);
  show_bin_btn_string(8, &set_mdw, &MDW_BTN[0]);
}

//~============================================================================
//
//                  CHECK READOUT
//
//~============================================================================

void create_check_readout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 70, w3 = 50, h1 = 15, h2 = 15, h3 = 15, h4 = 25;
  
  w = 5 + 8 * w1 + 5;
  h = 5 + h2 + 5 * (h1 + h2) + h3 + h4 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  w = 8 * w1;

  obj = add_frame_box(x, y, w, h1, "Bus Enabled", FL_SMALL_SIZE);
  y = y + h1;
  create_data_box_hstring(8, x, y, w1, h2, FL_NORMAL_SIZE, ENA_BOX);
  y = y + h2;
  chk_ena.was = malloc(4);
  chk_ena.rea = malloc(4);

  obj = add_frame_box(x, y, w, h1, "Bus Status", FL_SMALL_SIZE);
  y = y + h1;
  create_data_box_hstring(8, x, y, w1, h2, FL_NORMAL_SIZE, &STA_BOX[1]);
  y = y + h2;
  chk_sta.was = malloc(9*4);
  chk_sta.rea = malloc(9*4);

  obj = add_frame_box(x, y, w, h1, "Manual Readout Mode Control", FL_SMALL_SIZE);
  y = y + h1;
  create_data_box_hstring(8, x, y, w1, h2, FL_NORMAL_SIZE, MAN_BOX);
  y = y + h2;
  chk_man.was = malloc(4);
  chk_man.rea = malloc(4);

  obj = add_frame_box(x, y, w, h1, "Readout Mode Set", FL_SMALL_SIZE);
  y = y + h1;
  create_data_box_hstring(8, x, y, w1, h2, FL_NORMAL_SIZE, MDW_BOX);
  y = y + h2;
  chk_mdw.was = malloc(4);
  chk_mdw.rea = malloc(4);

  obj = add_frame_box(x, y, w, h1, "Actual Readout Mode", FL_SMALL_SIZE);
  y = y + h1;
  create_data_box_hstring(8, x, y, w1, h2, FL_NORMAL_SIZE, MDR_BOX);
  y = y + h2;
  chk_mdr.was = malloc(4);
  chk_mdr.rea = malloc(4);

  create_bit_num_string(-8, 8, x, y, w1, h3, FL_SMALL_SIZE);
  y = y + h3;

  obj = add_frame_box(x, y, w2, h2, "Bus 9 Status", FL_SMALL_SIZE);
  x = x + w2;
  STA_BOX[0] =
  obj = add_data_box(x, y, w1, h2, " ", FL_NORMAL_SIZE);
  x = x + w1;

  obj = add_frame_box(x, y, w3, h2, "Parallel.", FL_SMALL_SIZE);
  x = x + w3;
  PAR_BOX =
  obj = add_data_box(x, y, w1, h2, " ", FL_NORMAL_SIZE);
  x = x + w1;
  chk_par.was = malloc(4);
  chk_par.rea = malloc(4);
  y = y + h2;
    
  x = x0 + 5;
  obj = add_normal_button(x, y, 8*w1, h4, "CHECK READOUT", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, check_readout_callback, 0);
  y = y + h4;
}

//~----------------------------------------------------------------------------

void check_readout_callback(FL_OBJECT *obj, long data) {

  int i;
  int8 ena, par, sta[9], man, mdw, mdr;
  
  msg = -1;
  update_objects();

  *chk_par.was = *chk_par.rea;
  *chk_ena.was = *chk_ena.rea;
  for (i=0; i<9; i++) chk_sta.was[i] = chk_sta.rea[i];
  *chk_man.was = *chk_man.rea;
  *chk_mdw.was = *chk_mdw.rea;
  *chk_mdr.was = *chk_mdr.rea;
  
  check_DS1820_readout(&ena, &par, sta, &man, &mdw, &mdr, &err);

  if (!err) {
    *chk_ena.rea = ena;
    *chk_par.rea = par;
    for (i=0; i<9; i++) chk_sta.rea[i] = sta[i];
    *chk_man.rea = man;
    *chk_mdw.rea = mdw;
    *chk_mdr.rea = mdr;
    if (chk_par.unk) *chk_par.was = *chk_par.rea;
    if (chk_ena.unk) *chk_ena.was = *chk_ena.rea;
    if (chk_sta.unk) for (i=0; i<9; i++) chk_sta.was[i] = chk_sta.rea[i];
    if (chk_man.unk) *chk_man.was = *chk_man.rea;
    if (chk_mdw.unk) *chk_mdw.was = *chk_mdw.rea;
    if (chk_mdr.unk) *chk_mdr.was = *chk_mdr.rea;
    chk_ena.unk = FALSE;
    chk_par.unk = FALSE;
    chk_sta.unk = FALSE;
    chk_man.unk = FALSE;
    chk_mdw.unk = FALSE;
    chk_mdr.unk = FALSE;
    chk_ena.sta = status_OK;
    chk_par.sta = status_OK;
    chk_sta.sta = status_OK;
    chk_man.sta = status_OK;
    chk_mdw.sta = status_OK;
    chk_mdr.sta = status_OK;
  }
  else {
    chk_par.sta = status_KO;
    chk_ena.sta = status_KO;
    chk_sta.sta = status_KO;
    chk_man.sta = status_KO;
    chk_mdw.sta = status_KO;
    chk_mdr.sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_check_readout_panel(void) {

  show_num_box_string( 1, &chk_par, NULL, "%d", &PAR_BOX);
  show_bit_box_string( 8, &chk_ena, NULL, &ENA_BOX[0]);
  show_num_box_string(-9, &chk_sta, sta, NULL, &STA_BOX[0]);
  show_bit_box_string( 8, &chk_man, NULL, &MAN_BOX[0]);
  show_bit_box_string( 8, &chk_mdw, mod, &MDW_BOX[0]);
  show_bit_box_string( 8, &chk_mdr, mod, &MDR_BOX[0]);
}

//~============================================================================
//
//                  READ ID & DATA OF ONE BUS
//
//~============================================================================

void create_read_id_and_data_panel(int x0, int y0) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 25, w3 = 260, h1 = 15, h2 = 845, h3 = 20;
  char *legend = {"Nb -- Dallas ID --- P   t   long_t --laps-- "};
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + h2 + h3 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  obj = fl_add_text(FL_NORMAL_TEXT, x+2, y, w1+w2+w3, h1, legend);
    fl_set_object_lstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  y = y + h1;

  ONE_BUS_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1+w2+w3, h2, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
    fl_set_browser_vscrollbar(obj, FL_OFF);
    fl_set_browser_hscrollbar(obj, FL_OFF);
  y = y + h2;
  
  obj = add_frame_box(x, y, w1, h3, "Bus", FL_SMALL_SIZE);
  x = x + w1;

  BUS_BTN =   
  B = add_switch_btn(x, y, w2, h3, bus, FL_MEDIUM_SIZE, 9, bus_get.inp);
  B->call = switch_btn_callback;
  x = x + w2;

  obj = add_normal_button(x, y, w3, h3, "READ  ID  &  DATA", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, read_id_and_data_callback, 0);
  x = x + w3;
}

//~----------------------------------------------------------------------------

void read_id_and_data_callback(FL_OBJECT *obj, long data) {

  int8 b = atoi(bus[bus_get.inp]) - 1;
  int16 err1 = 0, err2 = 0, err3 = 0;
    
  msg = -1;
  update_objects();

  read_DS1820_table(b+1, &n0[b], &ID[b][0], &pri[b][0], &err1);

  if (!err1) {
    read_DS1820_temp_short(b+1, &n1[b], &ok1[b][0], &tmp1[b][0], &err2);
  }

  if (!err1 && !err2) {
    read_DS1820_temp_long(b+1, &n2[b], &ok2[b][0], &tmp2[b][0], &age2[b][0], &err3);
  }

  err = err1 | err2 | err3;

  if (!err) {
    browser_unk = FALSE;
    bus_get.wri = bus_get.inp;
    bus_get.unk = FALSE;
  }
  else {
    browser_unk = TRUE;
    bus_get.unk = TRUE;
  }
    
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_read_id_and_data_panel(void) {

  SW_BTN *B;
  FL_OBJECT *o;
  int i, c, b;
  bool old;
  char bu[80];

  fl_freeze_form(MAIN);

  B = BUS_BTN;
  o = B->obj;
  bus_get.inp = B->i;
  if (bus_get.inp != bus_get.wri) c = FL_YELLOW;
  else                            c = FL_GREEN;
  if (bus_get.unk)                c = FL_YELLOW;
  fl_set_object_color(o, c, FL_RED);

  o = ONE_BUS_BRO;
  fl_clear_browser(o);
  b = atoi(bus[bus_get.inp]) - 1;
  for (i=0; i<MIN(MIN(n0[b],n1[b]),n2[b]); i++) {
    int k = 0;
    old = FALSE;
    if (ok2[b][i] && age2[b][i] > 3600) old = TRUE;
    k += sprintf(bu+k, "%2d %016llX %d", i, ID[b][i], pri[b][i]);
    if (ok1[b][i]) k += sprintf(bu+k, "%+6.1f", tmp1[b][i]);
    else           k += sprintf(bu+k, "      ");
    if (ok2[b][i]) {
      k += sprintf(bu+k, "%+7.2f", tmp2[b][i]);
      k += sprintf(bu+k, " %02d:%02d:%02d", 
                  (age2[b][i]/60)/60, (age2[b][i]/60)%60, (age2[b][i]%60)%60);
    }
    else {
      k += sprintf(bu+k, "       ");
      k += sprintf(bu+k, "         ");
    }
//  fl_addto_browser(o, bu);
    fl_add_browser_line(o, bu);
    if (old) fl_select_browser_line(o, i+1);
  }

  if (browser_unk) fl_set_object_color(o, FL_YELLOW, FL_YELLOW);
  else             fl_set_object_color(o, FL_WHITE, FL_YELLOW);

  fl_unfreeze_form(MAIN);
}

//~============================================================================
//
//                  READ (and WRITE) ALL TEMPERATURES
//
//~============================================================================

void create_read_temperature_panel(int x0, int y0) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 60, w2 = 35, w3 = 260, w4 = 65, h1 = 15, h2 = 845, h3 = 20;
  static char *legend ={"Nb  --8--  --7--  --6--  --5--  --4--  --3--  --2--  --1--"};
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + h1 + h2 + h3 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  obj = fl_add_text(FL_NORMAL_TEXT, x+2, y, w1+w2+w3+w4, h1, legend);
    fl_set_object_lstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  y = y + h1;

  ALL_BUS_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1+w2+w3+w4, h2, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
    fl_set_browser_vscrollbar(obj, FL_OFF);
    fl_set_browser_hscrollbar(obj, FL_OFF);
  y = y + h2;
  
  obj = add_frame_box(x, y, w1, h3, "Period, s", FL_SMALL_SIZE);
  x = x + w1;

  PER_BTN =   
  B = add_switch_btn(x, y, w2, h3, per, FL_MEDIUM_SIZE, 9, per_get.inp);
//B->call = switch_btn_callback;
  B->call = update_callback;
  x = x + w2;

  obj = add_normal_button(x, y, w3, h3, "READ  ALL  TEMPERATURES", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, read_temperature_callback, 0);
  x = x + w3;

  obj = add_normal_button(x, y, w4, h3, "WRITE", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, write_temperature_callback, 0);
  x = x + w4;

  AUTOREAD =
  obj = fl_add_timer(FL_HIDDEN_TIMER, 0, 0, 0, 0,"");
    fl_set_object_callback(obj, read_temperature_callback, 0);
}

//~----------------------------------------------------------------------------

void read_temperature_callback(FL_OBJECT *obj, long data) {

  int i;
  double period = atoi(per[per_get.inp]);
    
  if (obj != AUTOREAD) {
    if (autoread) {
      autoread = FALSE;
      fl_set_timer(AUTOREAD, 0.0);
      set_normal_button(obj, 0);
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        autoread = TRUE;
        fl_set_timer(AUTOREAD, 0.0);
        set_normal_button(obj, 1);
      }
    }
    msg = -1;
    update_objects();
  }
  
  err = 0;

  if (ID_updated) {
    for (i=0; i<8; i++) {
      if (!err) read_DS1820_table(i+1, &n[i], &ID[i][0], NULL, &err);
    }
    if (!err) ID_updated = FALSE;
  }
  
  for (i=0; i<8; i++) {
    if (!err) read_DS1820_temp_long(i+1, &n[i], &ok[i][0], &tmp[i][0], &age[i][0], &err);
  }
    
  if (!err) {
    brouser_unk = FALSE;
    per_get.wri = per_get.inp;
    per_get.unk = FALSE;
  }
  else {
    brouser_unk = TRUE;
    per_get.unk = TRUE;
  }

  if (obj != AUTOREAD) msg = 1;
  update_objects();

  if (autoread) fl_set_timer(AUTOREAD, period);

  if (writing) write_temperature_callback(NULL, 0);
}

//~----------------------------------------------------------------------------

void write_temperature_callback(FL_OBJECT *obj, long data) {

  if (obj) {
    if (writing) {
      writing = FALSE;
      set_normal_button(obj, 0);
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        writing = TRUE;
        set_normal_button(obj, 1);
        update_objects();
        return;
      }
    }
  }
  
  record_DS1820_data(stdout);
  logfile = fopen(log_file_name, "a+");
  if (logfile) {
    setbuf(logfile, NULL);
    record_DS1820_data(logfile);
    fclose(logfile);
  }
  else {
    printf("writing failed...%c\n", 7);
  }
}

//~----------------------------------------------------------------------------

void record_DS1820_data(FILE *file) {

  int i, j, nmax;

  fprintf(file, "0x%08lX %10ld : %s\n", time(NULL), time(NULL), mytime());

  nmax = 0;
  for (j=0; j<8; j++) nmax = MAX(nmax, n[j]);

  if (nmax) {
    fprintf(file, "Nb");
    for (j=0; j<8; j++) {
      int jj = 7 - j;
      if (n[jj]) fprintf(file, " -------- Bus %d --------", jj+1);
    }
    fprintf(file, "\n");
  }

  for (i=0; i<nmax; i++) {
    fprintf(file, "%2d", i);
    for (j=0; j<8; j++) {
      int jj = 7 - j;
      if (n[jj]) {
        if (i < n[jj]) {
          fprintf(file, " %016llX", ID[jj][i]);
          if (ok[jj][i]) fprintf(file, " %+5.2f", tmp[jj][i]);
          else           fprintf(file, "       ");
        }
        else {
          fprintf(file, "                 ");
          fprintf(file, "       ");
        }
      }
    }
    fprintf(file, "\n");
  }
}

//~----------------------------------------------------------------------------

void update_read_temperature_panel(void) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int i, j, nmax, c;
  bool old;
  
  fl_freeze_form(MAIN);

  B = PER_BTN;
  obj = B->obj;
  per_get.inp = B->i;
  if (per_get.inp != per_get.wri) c = FL_YELLOW;
  else                            c = FL_GREEN;
  if (per_get.unk)                c = FL_YELLOW;
  fl_set_object_color(obj, c, FL_RED);

  obj = ALL_BUS_BRO;
  fl_clear_browser(obj);
  nmax = 0;
  for (j=0; j<8; j++) nmax = MAX(nmax, n[j]);
  for (i=0; i<nmax; i++) {
    char b[80];
    int k = 0;
    old = FALSE;
    for (j=0; j<8; j++) if (ok[j][i] && age[j][i] > 3600) old = TRUE;
    k += sprintf(b+k, "%2d", i);
    for (j=0; j<8; j++) {
      int jj = 7 - j;
      if (i < n[jj] && ok[jj][i]) {
        if (age[jj][i] > 3600) k += sprintf(b+k, "%+6.1f*", tmp[jj][i]);
        else                   k += sprintf(b+k, "%+7.2f",  tmp[jj][i]);
      }
      else {
        k += sprintf(b+k, "       ");
      }
    }
//  fl_addto_browser(obj, b);
    fl_add_browser_line(obj, b);
    if (old) fl_select_browser_line(obj, i+1);
  }

  if (brouser_unk) fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  else             fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
  
  fl_unfreeze_form(MAIN);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);
  update_start_scan_panel();
  update_check_scan_panel();
  update_setup_readout_panel();
  update_check_readout_panel();
  update_read_id_and_data_panel();
  update_read_temperature_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
