// file LDDR-3.c
//
// LDDR Controller
//
// A.Lebedev Jan-2008...

#include "template.h"
#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"LDDR - 3  Controller"};               // global variable
char *date  = {"13-Dec-08"};                          // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

bool _H_;

int  msg;
int16 err;

//~--

_w wri_reg[5];
_r rea_reg[5];

SW_BTN    *MOD_BTN[5];
FL_OBJECT *MOD_BOX[5];

SW_BTN    *WID_BTN[5];
FL_OBJECT *WID_BOX[5];

SW_BTN    *PWR_BTN[5][2], *BIA_BTN[5][2];
FL_OBJECT *PWR_BOX[5][2], *BIA_BOX[5][2], *TRI_BOX[5][2];

//~--

SW_BTN  *DAC_BTN[5][2][3];
DAT_BOX *DAC_BOX[5][2];
_r  r_dac[5][2];
_ww w_dac[5][2];

//~--

DAT_BOX *ADC_BOX[5][2];
_r  r_adc[5][2];

//~--

_r rea_tmp[5][2];

FL_OBJECT *TMP_BOX[5][2];

//~--

SW_BTN  *LCTL_PWR_BTN[2];
DAT_BOX *LCTL_PWR_BOX[2];
_ww w_lctl_pwr;
_r  r_lctl_pwr;

//~--

bool autotrigger[5] = {FALSE, FALSE, FALSE, FALSE, FALSE};

_w wri_per[5];

SW_BTN *PER_BTN[5];

//~--

#define LOAD_OK         0
#define LOAD_NO_FILE    1
#define LOAD_BAD_DATA   2
#define LOAD_WRITE_FAIL 3

_w wri_dts_uscm;
_r rea_dts_load;
_r rea_dts_chk;

SW_BTN    *USCM_BTN;
FL_OBJECT *DTS_LOAD, *DTS_CHK;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

char *a_b[2] = {"A", "B"};
char *h_c[2] = {"H", "C"};
char *o_o[3] = {"OFF", "ON", "?"};
char *O_O[4] = {"OFF", "OFF", "OFF", "ON"};
char *o_t[2] = {"O.K.", "TRIP"};
//char *modes[8] = {"INT", "CONT", "EXT A", NULL,
//                   NULL,  NULL,  "EXT B", NULL};
char *w_mod[8] = {"INT", NULL, "EXT A", NULL,
                  NULL,  NULL, "EXT B", NULL};

char *r_mod[8] = {"INT", "CONT", "EXT A", "CONT",
                  "INT", "CONT", "EXT B", "CONT"};

char *widths[16] = {"8.0us", "0.5us", "1.0us", "1.5us",
                    "2.0us", "2.5us", "3.0us", "3.5us", 
                    "4.0us", "4.5us", "5.0us", "5.5us", 
                    "6.0us", "6.5us", "7.0us", "7.5us"}; 

char *periods[8] = {"0.05s", "0.1s", "0.2s", "0.5s", 
                     "1.0s", "2.0s", "5.0s", "10.0s"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_change_layout_panel(int x0, int y0);
void change_layout_callback(FL_OBJECT *obj, long data);

void create_LDDR_board_panel(int x0, int y0, int ind);
void set_LDDR_data_callback(FL_OBJECT *obj, long data);
void get_LDDR_data_callback(FL_OBJECT *obj, long data);
void trigger_callback(FL_OBJECT *obj, long data);
void autotrigger_callback(int tid, void *data);
void update_LDDR_board_panel(int ind);

void create_Dallas_panel(int x0, int y0);
void load_Dallas_ID_callback(FL_OBJECT *obj, long data);
void start_Dallas_readout_callback(FL_OBJECT *obj, long data);
void stop_Dallas_readout_callback(FL_OBJECT *obj, long data);
void check_Dallas_readout_callback(FL_OBJECT *obj, long data);
void update_Dallas_panel(void);

void create_LCTL_power_panel(int x0, int y0);
void set_LCTL_power_callback(FL_OBJECT *obj, long data);
void get_LCTL_power_callback(FL_OBJECT *obj, long data);
void update_LCTL_power_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int i;
  int WW, HH;
  
  invalidate_data();

  if (_H_) {
    WW = 825;
    HH = 550;
  } 
  else {
    WW = 1050;
    HH =  490;
  }

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_change_layout_panel(200, 0);

  for (i=0; i<5; i++) {
    if (_H_) create_LDDR_board_panel(0,  45+85*i, i);
    else     create_LDDR_board_panel(0+210*i, 45, i);
  }
  if (_H_) {
    create_Dallas_panel(           0, 470);
    create_LCTL_power_panel(     355, 470);
    create_USCM_control_H2_panel(625, 475);
  }
  else {
    create_Dallas_panel(           0, 410);
    create_LCTL_power_panel(     460, 410);
    create_USCM_control_H2_panel(850, 415);
  }
  
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

  int i, j;
  static bool first = TRUE;
  
  _H_ = FALSE;
  
  for (i=0; i<5; i++) {
    wri_reg[i].inp = 0x0000;
    wri_reg[i].unk = TRUE;
    rea_reg[i].unk = TRUE;
    rea_reg[i].sta = status_NA;
    if (first) rea_reg[i].rea = malloc(4);
    if (first) rea_reg[i].was = malloc(4);
    wri_per[i].inp = 0;
    for (j=0; j<2; j++) {
      invalidate_r(first,  &r_dac[i][j], 1);
      invalidate_ww(first, &w_dac[i][j], 1, 0);
      invalidate_r(first,  &r_adc[i][j], 1);
      rea_tmp[i][j].unk = TRUE;
      rea_tmp[i][j].sta = status_NA;
      if (first) rea_tmp[i][j].rea = malloc(4);
      if (first) rea_tmp[i][j].was = malloc(4);
    }
  }

  wri_dts_uscm.inp = 0;
  wri_dts_uscm.unk = TRUE;
  rea_dts_load.unk = TRUE;
  rea_dts_load.sta = status_NA;
  if (first) rea_dts_load.rea = malloc(4);
  if (first) rea_dts_load.was = malloc(4);
  rea_dts_chk.unk = TRUE;
  rea_dts_chk.sta = status_NA;
  if (first) rea_dts_chk.rea = malloc(4);
  if (first) rea_dts_chk.was = malloc(4);

  r_lctl_pwr.unk = TRUE;
  r_lctl_pwr.sta = status_NA;
  if (first) r_lctl_pwr.rea = malloc(4);
  if (first) r_lctl_pwr.was = malloc(4);
  w_lctl_pwr.unk = TRUE;
  if (first) w_lctl_pwr.inp = malloc(4);
  if (first) w_lctl_pwr.wri = malloc(4);
  w_lctl_pwr.inp[0] = 0;

  read_dat_config_file(dat_config_file_name);
  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i, j;
  
  file = fopen(filename, "r");
  if (file) {
    fread(&_H_, sizeof(_H_), 1, file);
    for (i=0; i<5; i++) {
      fread(&wri_reg[i].inp, 4, 1, file);
      fread(&wri_per[i].inp, 4, 1, file);
      for (j=0; j<2; j++) {
        fread(w_dac[i][j].inp, 4, 1, file);
      }
    }
    fread(&wri_dts_uscm.inp, 4, 1, file);
    fread(w_lctl_pwr.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i, j;

  file = fopen(filename, "w");
  if (file) {
    fwrite(&_H_, sizeof(_H_), 1, file);
    for (i=0; i<5; i++) {
      fwrite(&wri_reg[i].inp, 4, 1, file);
      fwrite(&wri_per[i].inp, 4, 1, file);
      for (j=0; j<2; j++) {
        fwrite(w_dac[i][j].inp, 4, 1, file);
      }
    }
    fwrite(&wri_dts_uscm.inp, 4, 1, file);
    fwrite(w_lctl_pwr.inp, 4, 1, file);
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
//                  CHANGE LAYOUT PANEL
//
//~============================================================================

void create_change_layout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, h1 = 35;

  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "Change\nLayout", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, change_layout_callback, 0);
}

//~----------------------------------------------------------------------------

void change_layout_callback(FL_OBJECT *obj, long data) {

  msg = 0;
  
  _H_ = !_H_;
  
  write_dat_config_file(dat_config_file_name);
  
  create_main_form();
}

//~============================================================================
//
//                  LDDR BOARD PANEL
//
//~============================================================================

void create_LDDR_board_panel(int x0, int y0, int ind) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int j;
  char b[80];
  int x, y, w, h;
  _b dac_msk[3] = {
    {0, 0x00000FFF,  2},
    {0, 0x00000FFF, 10},
    {0, 0x00000FFF, 10}};
  _b adc_msk[1] = {
    {0, 0xFFFFFFFF, -1}};
  
//~----- "horizontal" layout ------

  if (_H_) {
    int w1 = 70;
    int w2 = 50, w3 = 50;
    int w4 = 40, w5 = 40, w6 = 45, w7 = 45, w8 = 45, w9 = 60;
    int w10 = 30, w11 = 50;
    int h1 = 20, h2 = 15, h3 = 20;
  
//~--

    w = 5 + w1 + w2 + w3 + 
        2 * (w4 + w5 + w6 + w7 + w8 + w9 + 5) + 5 + w10 + w11 + 5;
    h = 5 + h1 + h2 + 2 * h3 + 5;
    fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

    x = x0 + 5;
    y = y0 + 5;

    sprintf(b, "LDDR #%d\n(Slot %d)", ind, ind+6);
    add_frame_box(x, y, w1, h1+h2+2*h3, b, 'M');
    x = x + w1;
  
//~--

    h = h1 + h2;
    add_frame_box(x, y, w2, h, "Operat.\nMode", 'N');
    y = y + h;
    MOD_BTN[ind] = add_switch_btn(x, y, w2, h3, w_mod, 'M', 8, -1);
    y = y + h3;
    MOD_BOX[ind] = add_data_box(x, y, w2, h3, "", 'M');
    y = y + h3;
    x = x + w2;

    y = y0 + 5;
    h = h1 + h2;
    add_frame_box(x, y, w3, h, "Pulse\nWidth", 'N');
    y = y + h;
    WID_BTN[ind] = add_switch_btn(x, y, w3, h3, widths, 'N', 16, -1);
    y = y + h3;
    WID_BOX[ind] = add_data_box(x, y, w3, h3, "", 'M');
    y = y + h3;
    x = x + w3 + 5;

//~--

    for (j=0; j<2; j++) {
      y = y0 + 5;
      sprintf(b, "Laser %d", j);
      w = w4 + w5 + w6 + w7 + w8 + w9;
      add_frame_box(x, y, w, h1, b, 'N');
  
      y = y0 + 5 + h1;
      add_frame_box(x, y, w4, h2, "Diode", 'N');
      y = y + h2;
      PWR_BTN[ind][j] = add_switch_btn(x, y, w4, h3, o_o, 'M', 2, -1);
      y = y + h3;
      PWR_BOX[ind][j] = add_data_box(x, y, w4, h3, "", 'M');
      x = x + w4;

      y = y0 + 5 + h1;
      add_frame_box(x, y, w5, h2, "Bias", 'N');
      y = y + h2;
      BIA_BTN[ind][j] = add_switch_btn(x, y, w5, h3, o_o, 'M', 2, -1);
      y = y + h3;
      BIA_BOX[ind][j] = add_data_box(x, y, w5, h3, "", 'M');
      x = x + w5;

      y = y0 + 5 + h1;
      h = h2 + h3;
      add_frame_box(x, y, w6, h, "Power\nStatus", 'N');
      y = y + h;
      TRI_BOX[ind][j] = add_data_box(x, y, w6, h3, "", 'M');
      x = x + w6;

      y = y0 + 5 + h1;
      add_frame_box(x, y, w7, h2, "I, mA", 'N');
      y = y + h2;
      create_btn10_hstring(
        3, x, y,    w7/3, h3, hex, 'M', &w_dac[ind][j], dac_msk, &DAC_BTN[ind][j][0]);
      create_dat_box_vstring(
        1, x, y+h3, w7,   h3, 'M', &r_dac[ind][j], dac_msk, &DAC_BOX[ind][j]);
      x = x + w7;

      y = y0 + 5 + h1;
      h = h2 + h3;
      add_frame_box(x, y, w8, h, "I, mA\nADC", 'N');
      y = y + h;
      create_dat_box_vstring(
        1, x, y, w8, h3, 'M', &r_adc[ind][j], adc_msk, &ADC_BOX[ind][j]);
      x = x + w8;

      y = y0 + 5 + h1;
      h = h2 + h3;
      add_frame_box(x, y, w9, h, "Temp.", 'N');
      y = y + h;
      create_data_box_hstring(
        1, x, y, w9, h3, 'M', &TMP_BOX[ind][j]);
      x = x + w9 + 5;
    }
  
//~--

    y = y0 + 5;
    h = h1 + h2;
    add_frame_box(x, y, w10, h, "Cmd", 'N');
    y = y + h;
    obj = add_normal_button(x, y, w10, h3, "SET", 'N');
      fl_set_object_callback(obj, set_LDDR_data_callback, ind);
    y = y + h3;
    obj = add_normal_button(x, y, w10, h3, "GET", 'N');
      fl_set_object_callback(obj, get_LDDR_data_callback, ind);
    x = x + w10;

//~--

    y = y0 + 5;
    add_frame_box(x, y, w11, h2, "Period", 'N');
    y = y + h2;
    create_num_btn_hstring(
      1, x, y, w11, h3, periods, 'M', 8, &wri_per[ind], &PER_BTN[ind]);
    y = y + h1;
    h = 2 * h3;
    obj = add_normal_button(x, y, w11, h, "FIRE", 'L');
      fl_set_object_callback(obj, trigger_callback, ind);
  }
  
//~----- "vertical" layout ------

  else {
    int w1 = 80, w2 = 60, w3 = 60;
    int w4 = 20, w5 = 60;
    int w6 = 50, w7 = 55, w8 = 95;
    int h1 = 20;
  
//~--

    w = 5 + w1 + w2 + w3 + 5;
    h = 5 + 3 * h1 + 5 + 2 * (6 * h1 + 5) + 2 * h1 + 5;
    fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

    x = x0 + 5;
    y = y0 + 5;

    sprintf(b, "LDDR #%d  (Slot %d)", ind, ind+6);
    add_frame_box(x, y, w1+w2+w3, h1, b, 'M');
    y = y + h1;
  
//~--

    add_frame_box(x, y, w1, h1, "Oper. Mode", 'N');
    x = x + w1;
    MOD_BTN[ind] = add_switch_btn(x, y, w2, h1, w_mod, 'M', 8, -1);
    x = x + w2;
    MOD_BOX[ind] = add_data_box(x, y, w3, h1, "", 'M');
    y = y + h1;

    x = x0 + 5;
    add_frame_box(x, y, w1, h1, "Pulse Width", 'N');
    x = x + w1;
    WID_BTN[ind] = add_switch_btn(x, y, w2, h1, widths, 'M', 16, -1);
    x = x + w2;
    WID_BOX[ind] = add_data_box(x, y, w3, h1, "", 'M');
    y = y + h1 + 5;

//~--

    for (j=0; j<2; j++) {
      x = x0 + 5;
      sprintf(b, "L\n%d", j);
      add_frame_box(x, y, w4, 6*h1, b, 'L');
  
      x = x0 + 5 + w4;
      add_frame_box(x, y, w5, h1, "Diode", 'N');
      x = x + w5;
      PWR_BTN[ind][j] = add_switch_btn(x, y, w2, h1, o_o, 'M', 2, -1);
      x = x + w2;
      PWR_BOX[ind][j] = add_data_box(x, y, w3, h1, "", 'M');
      y = y + h1;

      x = x0 + 5 + w4;
      add_frame_box(x, y, w5, h1, "Bias", 'N');
      x = x + w5;
      BIA_BTN[ind][j] = add_switch_btn(x, y, w2, h1, o_o, 'M', 2, -1);
      x = x + w2;
      BIA_BOX[ind][j] = add_data_box(x, y, w3, h1, "", 'M');
      y = y + h1;

      x = x0 + 5 + w4;
      w = w5 + w2;
      add_frame_box(x, y, w, h1, "Power Status", 'N');
      x = x + w;
      TRI_BOX[ind][j] = add_data_box(x, y, w3, h1, "", 'M');
      y = y + h1;

      x = x0 + 5 + w4;
      add_frame_box(x, y, w5, h1, "I, mA", 'N');
      x = x + w5;
      create_btn10_hstring(
        3, x, y, w2/3, h1, hex, 'M', &w_dac[ind][j], dac_msk, &DAC_BTN[ind][j][0]);
      x = x + w2;
      create_dat_box_vstring(
        1, x, y, w3,   h1, 'M', &r_dac[ind][j], dac_msk, &DAC_BOX[ind][j]);
      y = y + h1;

      x = x0 + 5 + w4;
      w = w5 + w2;
      add_frame_box(x, y, w, h1, "I, mA (ADC)", 'N');
      x = x + w;

      create_dat_box_vstring(
        1, x, y, w3,   h1, 'M', &r_adc[ind][j], adc_msk, &ADC_BOX[ind][j]);
      y = y + h1;

      x = x0 + 5 + w4;
      w = w5 + w2;
      add_frame_box(x, y, w, h1, "Temperature", 'N');
      x = x + w;
      create_data_box_hstring(
        1, x, y, w3, h1, 'M', &TMP_BOX[ind][j]);
      y = y + h1 + 5;
    }
  
//~--

    x = x0 + 5;
    add_frame_box(x, y, w1, h1, "Command", 'N');
    x = x + w1;
    obj = add_normal_button(x, y, w2, h1, "SET", 'N');
      fl_set_object_callback(obj, set_LDDR_data_callback, ind);
    x = x + w2;
    obj = add_normal_button(x, y, w3, h1, "GET", 'N');
      fl_set_object_callback(obj, get_LDDR_data_callback, ind);
    y = y + h1;

//~--

    x = x0 + 5;
    add_frame_box(x, y, w6, h1, "Period", 'N');
    x = x + w6;
    create_num_btn_hstring(
      1, x, y, w7, h1, periods, 'M', 8, &wri_per[ind], &PER_BTN[ind]);
    x = x + w7;
    obj = add_normal_button(x, y, w8, h1, "FIRE", 'N');
      fl_set_object_callback(obj, trigger_callback, ind);
  }
  
//~----- common part for both layouts ------

  B = MOD_BTN[ind];
  B->p = &wri_reg[ind].inp;
  B->m = LDDR_OPERATION_MODE;
  B->i = unpack32(*B->p, B->m);
  B->call = SW_BTN_CALLBACK;

  B = WID_BTN[ind];
  B->p = &wri_reg[ind].inp;
  B->m = LDDR_PULSE_WIDTH;
  B->i = unpack32(*B->p, B->m);
  B->call = SW_BTN_CALLBACK;

//~--

  for (j=0; j<2; j++) {
    B = PWR_BTN[ind][j];
    if (j == 0) B->m = LDDR_ENABLE_L0 | LDDR_POWER_L0;
    else        B->m = LDDR_ENABLE_L1 | LDDR_POWER_L1;
    if ((wri_reg[ind].inp & B->m) == B->m) B->i = 1;
    else                                   B->i = 0;
    B->call = SW_BTN_CALLBACK;
    B = BIA_BTN[ind][j];
    if (j == 0) B->m = LDDR_BIAS_L0;
    else        B->m = LDDR_BIAS_L1;
    if ((wri_reg[ind].inp & B->m) == B->m) B->i = 1;
    else                                   B->i = 0;
    B->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void set_LDDR_data_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int8 adr[5] = {0x10, 0x12, 0x14, 0x16, 0x18};
  _r *r;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
//~--

  if (!err) {
    _w *w = &wri_reg[ind];
    if (obj) {
      _r *r = &rea_reg[ind];
      r->sta = status_NA;
    }
    write_LVDS_bus(adr[ind]+0, w->inp&0x00FF, &err);
    if (!err) {
      write_LVDS_bus(adr[ind]+1, w->inp>>8, &err);
    }
    if (!err) {
      w->wri = w->inp;
      w->unk = FALSE;
    }
    else {
      w->unk = TRUE;
    }
  }

//~--

  if (!err) {
    int j, k;
    int16 dat[16];
    for (k=0; k<16; k++) dat[k] = 0x8000;
    for (j=0; j<2; j++) {
      _ww *w = &w_dac[ind][j];
      if (obj) {
        r = &r_dac[ind][j];
        r->sta = status_NA;
        r = &r_adc[ind][j];
        r->sta = status_NA;
      }
      for (k=0; k<16; k++) {
        if (k == (2 * ind + j)) {
          dat[k] = (int32)((float)w->inp[0] / 0.05 + 0.5) & 0x0FFF;
        }
      }
    }
    write_DAC(dat, &err);
    for (j=0; j<2; j++) {
      _ww *w = &w_dac[ind][j];
      if (!err) {
        w->wri[0] = w->inp[0];
        w->unk    = FALSE;
      }
      else {
        w->unk = TRUE;
      }
    }
  }

//~--

  if (obj) {
    msg = 1;
    update_objects();
  }
  else {
    update_LDDR_board_panel(ind);
  }
}

//~----------------------------------------------------------------------------

void get_LDDR_data_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int8 adr[5] = {0x10, 0x12, 0x14, 0x16, 0x18};
  int8 bus = 1;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
//~--

  if (!err) {
    int16 dat[2];
    _r *r = &rea_reg[ind];
    *r->was = *r->rea;
    read_LVDS_bus(adr[ind]+0, &dat[0], &err);
    if (!err) {
      read_LVDS_bus(adr[ind]+1, &dat[1], &err);
    }
    if (!err) {
      *r->rea = (dat[1] << 8) + (dat[0] & 0x00FF);
      if (r->unk) *r->was = *r->rea;
      r->unk = FALSE;
      r->sta = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }
  
//~--

  if (!err) {
    int i, j;
    int16 dat[16];
    for (i=0; i<5; i++) {
      for (j=0; j<2; j++) {
        _r *r = &r_dac[i][j];
        *r->was = *r->rea;
      }
    }
    read_DAC(dat, &err);
    for (i=0; i<5; i++) {
      for (j=0; j<2; j++) {
        _r *r = &r_dac[i][j];
        if (!err) {
          *r->rea = dat[2*i+j];
          if (r->unk) *r->was = *r->rea;
          r->unk = FALSE;
          r->sta = status_OK;
        }
        else {
          r->sta = status_KO;
        }
      }
    }
  }

//~--

  if (!err) {
    int i, j;
    int16 dat[32];
    for (i=0; i<5; i++) {
      for (j=0; j<2; j++) {
        _r *r = &r_adc[i][j];
        *r->was = *r->rea;
      }
    }
    read_ADC(dat, &err);
    for (i=0; i<5; i++) {
      for (j=0; j<2; j++) {
        _r *r = &r_adc[i][j];
        if (!err) {
          *r->rea = dat[2*i+j];
          if (r->unk) *r->was = *r->rea;
          r->unk = FALSE;
          r->sta = status_OK;
        }
        else {
          r->sta = status_KO;
        }
      }
    }
  }

//~--

  if (!err) {
    int i, j;
    int8 n;
    bool ok[64];
    float val[64];
    for (i=0; i<5; i++) {
      for (j=0; j<2; j++) {
        _r *r = &rea_tmp[i][j];
        *r->was = *r->rea;
      }
    }
    read_DS1820_temp_short(bus, &n, ok, val, &err);
    for (i=0; i<5; i++) {
      for (j=0; j<2; j++) {
        _r *r = &rea_tmp[i][j];
        int k = 2 * i + j;
        if (!err) {
          *r->rea = *(int32*)&val[2*i+j];
          if (r->unk) *r->was = *r->rea;
          if (k < n && ok[k]) r->unk = FALSE;
          else                r->unk = TRUE;
          r->sta = status_OK;
        }
        else {
          r->sta = status_KO;
        }
      }
    }
  }

  if (!err) check_Dallas_readout_callback(NULL, 0);
  if (!err) {
    int i, j;
    _r *s = &rea_dts_chk;
    if (!*s->rea) {
      for (i=0; i<5; i++) {
        for (j=0; j<2; j++) {
          rea_tmp[i][j].sta = status_KO;
        }
      }
    }
  }
  
//~--

  if (obj) {
    msg = 1;
    update_objects();
  }
  else {
    update_LDDR_board_panel(ind);
  }
}

//~----------------------------------------------------------------------------

void trigger_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  SW_BTN *B;
  _w *w = &wri_reg[ind];
  int period = 1000.0 * atof(periods[wri_per[ind].inp]);
  
  B = MOD_BTN[ind];
  while (!B->l[B->i] || strcmp(B->l[B->i],"INT")) B->i = MOD(B->i+1, B->n);
  pack32(B->p, B->m, B->i);
  
  if (obj) {
    if (autotrigger[ind]) {
      autotrigger[ind] = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        autotrigger[ind] = TRUE;
        fl_add_timeout(period, autotrigger_callback, (void*)ind);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!autotrigger[ind]) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;
  if (!err) {
    w->inp &= ~LDDR_FIRE_INT_TRIGGER;
    set_LDDR_data_callback(NULL, ind);
    if (!err) {
      w->inp |=  LDDR_FIRE_INT_TRIGGER;
      set_LDDR_data_callback(NULL, ind);
      if (!err) {
        get_LDDR_data_callback(NULL, ind);
      }
    }
  }
  
  if (!autotrigger[ind]) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void autotrigger_callback(int tid, void *data) {

  int ind = (int)data;
  int period = 1000.0 * atof(periods[wri_per[ind].inp]);

  if (!autotrigger[ind]) return;
  
  if (P) {
    static struct timeval now[5], then[5];
    gettimeofday(&now[ind], NULL);
    printf("Timeout callback %d %4d %f\n",
           ind, period, delta_t(&now[ind], &then[ind]));
    memcpy(&then[ind], &now[ind], sizeof(now[ind]));
  }

  trigger_callback(NULL, ind);
  fl_add_timeout(period, autotrigger_callback, (void*)ind);
}

//~----------------------------------------------------------------------------

void update_LDDR_board_panel(int ind) {

  SW_BTN *B;
  int j;
  int32 m;
  bool power, trip;
  char b[80];
  _w *w = &wri_reg[ind];
  _r *r = &rea_reg[ind];

  show_bin_btn_string(1, w, &MOD_BTN[ind]);
  show_msk_box(LDDR_OPERATION_MODE, r, r_mod, MOD_BOX[ind]);
  if (!r->unk && (r->rea[0] & 0x0100)) {
    fl_set_object_color(MOD_BOX[ind], FL_RED, FL_RED);
    fl_set_object_lcolor(MOD_BOX[ind], FL_YELLOW);
  }

  show_bin_btn_string(1, w, &WID_BTN[ind]);
  show_msk_box(LDDR_PULSE_WIDTH, r, widths, WID_BOX[ind]);

  for (j=0; j<2; j++) {
    B = PWR_BTN[ind][j];
    if (B->i == 0) w->inp &= ~B->m;
    else           w->inp |=  B->m;
    show_msk_btn_string(1, w, &PWR_BTN[ind][j]);
    if (j == 0) m = LDDR_ENABLE_L0 | LDDR_POWER_L0;
    else        m = LDDR_ENABLE_L1 | LDDR_POWER_L1;
    show_msk_box(m, r, O_O, PWR_BOX[ind][j]);

    B = BIA_BTN[ind][j];
    if (B->i == 0) w->inp &= ~B->m;
    else           w->inp |=  B->m;
    show_msk_btn_string(1, w, &BIA_BTN[ind][j]);
    if (j == 0) m = LDDR_BIAS_L0;
    else        m = LDDR_BIAS_L1;
    show_msk_box(m, r, o_o, BIA_BOX[ind][j]);
    
    if (j == 0) {
      m     = LDDR_POWER_L0 | LDDR_POWER_L0_TRIP;
      power = *r->rea & LDDR_POWER_L0      ? TRUE : FALSE;
      trip  = *r->rea & LDDR_POWER_L0_TRIP ? TRUE : FALSE;
    }
    else {
      m     = LDDR_POWER_L1 | LDDR_POWER_L1_TRIP;
      power = *r->rea & LDDR_POWER_L1      ? TRUE : FALSE;
      trip  = *r->rea & LDDR_POWER_L1_TRIP ? TRUE : FALSE;
    }
    if (!power && !trip) sprintf(b, "OFF");
    if (!power &&  trip) sprintf(b, "OFF");
    if ( power && !trip) sprintf(b, "O.K.");
    if ( power &&  trip) sprintf(b, "TRIP");
    show_mask_box(m, r, b, TRI_BOX[ind][j]);
    if (!r->unk && power && trip) {
      fl_set_object_color(TRI_BOX[ind][j], FL_RED, 0);
      fl_set_object_lcolor(TRI_BOX[ind][j], FL_YELLOW);
    }

    if (*w_dac[ind][j].inp > 150) *w_dac[ind][j].inp = 150;
    show_btn_string(3, NULL, &DAC_BTN[ind][j][0]);

    sprintf(b, "%d", (int)(0.050*(float)r_dac[ind][j].rea[0]+0.5));
    show_dat_box_string(1, NULL, b, &DAC_BOX[ind][j]);

    sprintf(b, "%d", (int)(0.110*(float)r_adc[ind][j].rea[0]+0.5));
    show_dat_box_string(1, NULL, b, &ADC_BOX[ind][j]);

    sprintf(b, "%+.1f", *(float*)rea_tmp[ind][j].rea);
    show_num_box_string(1, &rea_tmp[ind][j], NULL, b, &TMP_BOX[ind][j]);
  }
}

//~============================================================================
//
//                  DALLAS CONTROL PANEL
//
//~============================================================================

void create_Dallas_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 125, w2 = 20, w3 = 45, w4 = 100, w5 = 50, w6 = 40, w7 = 35, w8 = 35;
  int h1 = 20;

  w = 5 + w1 + w2 + 5 + w5 + w6 + w7 + w8 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Dallas ID for USCM:", FL_NORMAL_SIZE);
  x = x + w1;
  create_bit_btn_hstring(
    1, x, y, w2, h1, h_c, FL_MEDIUM_SIZE, &wri_dts_uscm, &USCM_BTN);
  x = x + w2 + 5;

  w = w5 + w6 + w7 + w8;
  add_frame_box(x, y, w, h1, "Dallas Readout Status", FL_NORMAL_SIZE);
  y = y + h1;
  
  x = x0 + 5;
  obj = add_normal_button(x, y, w3, h1, "LOAD", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, load_Dallas_ID_callback, 0);
  x = x + w3;
  DTS_LOAD =
  add_data_box(x, y, w4, h1, "", FL_MEDIUM_SIZE);
  x = x + w4 + 5;
  obj = add_normal_button(x, y, w5, h1, "START", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, start_Dallas_readout_callback, 0);
  x = x + w5;
  obj = add_normal_button(x, y, w6, h1, "STOP", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, stop_Dallas_readout_callback, 0);
  x = x + w6;
  DTS_CHK =
  add_data_box(x, y, w7, h1, "", FL_MEDIUM_SIZE);
  x = x + w7;
  obj = add_normal_button(x, y, w8, h1, "CHK", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, check_Dallas_readout_callback, 0);
}

//~----------------------------------------------------------------------------

bool read_Dallas_ID_file(char *filename, int8 *n, int64 *ID) {

  FILE *file = NULL;
  int i;
  
  if (!file) file = fopen(filename, "r");
  if (!file) return FALSE;

  for (i=0; i<8; i++)    n[i] = 0;
  for (i=0; i<8*64; i++) ID[i] = 0;

  while (1) {
    static int swtc = 0;
    char b[8192];
    int k;
    static int bus = -1;
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';
    if (P) printf(b);
    k = sscanf(b, "Dallas Bus %d", &bus);
    if (k == 1 && bus > 0 && bus < 9) {
      if (P) printf(":: switched to Dallas Bus %d OK\n", bus);
      swtc = 1;
      continue;
    }
    switch (swtc) {
      int   i;
      int64 id;
      case 1:
        k = sscanf(b, "%d %16llX", &i, &id);
        if (k == 2) {
          if (i >= 0 && i < 64) {
            if (DOWCRC(id)) {
              n[bus-1] = MAX(n[bus-1], i+1);
              ID[64*(bus-1)+i] = id;
              if (P) printf(":: OK\n");
            }
            else {
              if (P) printf(":: ID CRC error\n");
            }
          }
          else {
            if (P) printf(":: number %d is invalid\n", i);
          }
        }
        else {
          if (P) printf(":: syntax error\n");
        }
      break;
    }
  }

  fclose(file);
  return TRUE;
}

//~----------------------------------------------------------------------------

void load_Dallas_ID_callback(FL_OBJECT *obj, long data) {

  int i, j;
  int8 n[8];
  int64 ID[8][64];
  _w *u = &wri_dts_uscm;
  _r *l = &rea_dts_load;
  _r *c = &rea_dts_chk;
  char *filename[2] = {
    "Dallas_ID_for_M-Crate_H.txt",
    "Dallas_ID_for_M-Crate_C.txt"};
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  c->sta = status_NA;
  for (i=0; i<5; i++) for (j=0; j<2; j++) rea_tmp[i][j].sta = status_NA;
  
  l->unk = FALSE;
  if (!read_Dallas_ID_file(filename[u->inp], n, &ID[0][0])) {
    *l->rea = LOAD_NO_FILE;
    err = 0x1000;
    l->sta = status_KO;
  }
  else {
    if (!n[0]) {
      *l->rea = LOAD_BAD_DATA;
      err = 0x2000;
      l->sta = status_KO;
    }
  }

  if (!err) {
    *l->was = *l->rea;
    write_DS1820_table(1, n[0], &ID[0][0], NULL, &err);
    if (!err) {
      if (l->unk) *l->was = *l->rea;
      *l->rea = LOAD_OK;
      l->sta = status_OK;
    }
    else {
      *l->rea = LOAD_WRITE_FAIL;
      l->sta = status_KO;
      for (i=0; i<5; i++) for (j=0; j<2; j++) rea_tmp[i][j].sta = status_KO;
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void start_Dallas_readout_callback(FL_OBJECT *obj, long data) {

  int i, j;
  int8 msk = 0x01;
  int8 ena, par, sta[8], man, mdw, mdr, rst;
  _r *s = &rea_dts_chk;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  s->sta = status_NA;
  for (i=0; i<5; i++) for (j=0; j<2; j++) rea_tmp[i][j].sta = status_NA;
  
  if (!err) {
    check_DS1820_readout(&ena, &par, &sta[0], &man, &mdw, &mdr, &err);
  }

  if (!err) {
    ena |=  msk;
    rst  =  msk;
    man &= ~msk;
    mdw &= ~msk;
//  delay(2.0);                                               // Thank you, HBB
    setup_DS1820_readout(ena, rst, man, mdw, par, &err);
  }

  if (!err) check_Dallas_readout_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void stop_Dallas_readout_callback(FL_OBJECT *obj, long data) {

  int i, j;
  int8 msk = 0x01;
  int8 ena, par, sta[8], man, mdw, mdr, rst;
  _r *s = &rea_dts_chk;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  s->sta = status_NA;
  for (i=0; i<5; i++) for (j=0; j<2; j++) rea_tmp[i][j].sta = status_KO;

  if (!err) {
    check_DS1820_readout(&ena, &par, &sta[0], &man, &mdw, &mdr, &err);
  }

  if (!err) {
    ena &= ~msk;
//  rst  =  msk;
    rst  = 0x00;
    setup_DS1820_readout(ena, rst, man, mdw, par, &err);
  }

  if (!err) check_Dallas_readout_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void check_Dallas_readout_callback(FL_OBJECT *obj, long data) {

  int i, j;
  int8 msk = 0x01;
  int8 ena, par, sta[8], man, mdw, mdr;
  _r *s = &rea_dts_chk;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  if (!err) {
    *s->was = *s->rea;
    check_DS1820_readout(&ena, &par, &sta[0], &man, &mdw, &mdr, &err);
    if (!err) {
      *s->rea = (ena & msk) ? 1 : 0;
      if (s->unk) *s->was = *s->rea;
      s->unk = FALSE;
      s->sta = status_OK;
    }
    else {
      s->sta = status_KO;
      for (i=0; i<5; i++) for (j=0; j<2; j++) rea_tmp[i][j].sta = status_KO;
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_Dallas_panel(void) {

  _r *l = &rea_dts_load;
  _r *s = &rea_dts_chk;
  char b[80];
  char *txt[4] = {"O.K.", "NO FILE", "BAD DATA", "WRITE FAIL"};

  show_num_box_string(1, l, txt, NULL, &DTS_LOAD);
  sprintf(b, *s->rea ? "ON" : "OFF");
  show_num_box_string(1, s,  NULL, b, &DTS_CHK);
}

//~============================================================================
//
//                  LCTL POWER CONTROL PANEL
//
//~============================================================================

  int   lctl_pwr_adr    = 14;
//                           BA    BA    BA    BA
  int16 lctl_pwr_cod[4] = {0x00, 0x01, 0x10, 0x11};
  int16 lctl_pwr_DAC[4] = {3200, 2200, 4095,    0};   // in mV, see WK e-mails

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_LCTL_power_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 25, w3 = 35, w4 = 35, h1 = 20;
  
  _b set_msk[2] = {
  { 0, 0x000001, 2},
  { 0, 0x000002, 2}};

  _b get_msk[2] = {
  { 0, 0x000003, 3},
  { 0, 0x000030, 3}};

  w = 5 + w1 + w2 + 2 * (w3 + w4) + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y,    w1, 2*h1, "LCTL\nPower", FL_MEDIUM_SIZE);
  x = x + w1;
  add_frame_box(x, y,    w2, h1,   "A",           FL_NORMAL_SIZE);
  add_frame_box(x, y+h1, w2, h1,   "B",           FL_NORMAL_SIZE);
  x = x + w2;
  
  create_btn_vstring(
    2, x, y, w3, h1, o_o, FL_MEDIUM_SIZE, &w_lctl_pwr, &set_msk[0], &LCTL_PWR_BTN[0]);
  x = x + w3;

  obj = add_normal_button(x, y, w4, 2*h1, "SET", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, set_LCTL_power_callback, 0);
  x = x + w4;
  
  create_dat_box_vstring(
    2, x, y, w3, h1,      FL_MEDIUM_SIZE, &r_lctl_pwr, &get_msk[0], &LCTL_PWR_BOX[0]);
  x = x + w3;


  obj = add_normal_button(x, y, w4, 2*h1, "GET", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, get_LCTL_power_callback, 0);
  x = x + w4;
}

//~----------------------------------------------------------------------------

void set_LCTL_power_callback(FL_OBJECT *obj, long data) {

  int i;
  _r  *r = &r_lctl_pwr;
  _ww *w = &w_lctl_pwr;
  int16 DAC[16];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  for (i=0; i<16; i++) {
    DAC[i] = (i == lctl_pwr_adr) ? lctl_pwr_DAC[w->inp[0]] : 0x8000;
  }
  
  write_DAC(DAC, &err);
  
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

void get_LCTL_power_callback(FL_OBJECT *obj, long data) {

  int i;
  _r  *r = &r_lctl_pwr;
  int16 DAC[16];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->was[0] = r->rea[0];
  
  read_DAC(DAC, &err);

  if (!err) {
    r->rea[0] = 0x22;
    for (i=0; i<4; i++) {
      if (DAC[lctl_pwr_adr] == lctl_pwr_DAC[i]) r->rea[0] = lctl_pwr_cod[i];
    }
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

void update_LCTL_power_panel(void) {

  show_btn_string(    2,      NULL, &LCTL_PWR_BTN[0]);
  show_dat_box_string(2, o_o, NULL, &LCTL_PWR_BOX[0]);
}

//~============================================================================

void update_objects(void) {

  int i;
  
  update_template_objects(&msg, &err, NULL, &P);

  for (i=0; i<5; i++) update_LDDR_board_panel(i);

  update_Dallas_panel();
  update_LCTL_power_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
