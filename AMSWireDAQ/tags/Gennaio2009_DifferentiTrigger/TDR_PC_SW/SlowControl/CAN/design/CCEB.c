// file CCEB.c
//
// CCEB Primitive Controller
//
// A.Lebedev Jun-2006...

#include "template.h"
#include "ccebdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CCEB  Primitive  Controller"};        // global variable
char *date  = {"25-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

struct _ADC *adc;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~-- CCEB Crate Model Selector

SW_BTN *CRT_BTN;
_ww ww_crt;

//~--

SW_BTN    *CCIF_STA_BTN[4][8];
FL_OBJECT *CCIF_STA_BOX[4][8];
_w wri_ccif_sta[4];
_r rea_ccif_sta[4];

//~--

SW_BTN    *CCIF_PHA_HEX_BTN[4][2], *CCIF_PHA_DEC_BTN[4][3];
FL_OBJECT *CCIF_PHA_HEX_BOX[4][2], *CCIF_PHA_DEC_BOX[4][3];
FL_OBJECT *CCIF_PHA_WRI_BOX[4][1], *CCIF_PHA_REA_BOX[4][1];
_w wri_ccif_pha[4];
_r rea_ccif_pha[4];

//~--

FL_OBJECT *CC_TMP_BOX[8];
FL_OBJECT *DTS_STA_BOX[16];
_r rea_tmp[8];
_r rea_dts_sta;

//~--

SW_BTN    *DAC_BTN[2][16];
FL_OBJECT *DAC_BOX[16];
_w ena_dac_wri;
_w wri_dac_sta;
_r rea_dac_sta;

//~--

FL_OBJECT *ADC_BRO;
_r rea_adc;

//~-- CCBT Browsers ---

SW_BTN    *CCBT_CTL_BTN[2];
DAT_BOX   *CCBT_USE_BOX[2], *CCBT_AGE_BOX[2], *CCBT_STA_BOX[2];
FL_OBJECT *CCBT_BRO[2];
_ww w_ccbt_ctl;
_r  r_ccbt_dat;

//~--

char *crt_nam[] = {"EM1", "EM2", "QM", "FM"};

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *e_d[2] = {" ", "@1+"};
char *o_o[4] = {"OFF", "ON", "?", "??"};
char *n_o[2] = {"NA", "OK"};
char *d_e[2] = {"D", "E"};
char *f_o[2] = {"FAIL", "O.K."};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_crate_model_selector_panel(int x0, int y0);
void update_crate_model_selector_panel(void);

void create_CCIF_status_panel(int x0, int y0);
void write_CCIF_status_callback(FL_OBJECT *obj, long data);
void read_CCIF_status_callback(FL_OBJECT *obj, long data);
void update_CCIF_status_panel(void);

void create_CCIF_phase_panel(int x0, int y0);
void write_CCIF_phase_callback(FL_OBJECT *obj, long data);
void read_CCIF_phase_callback(FL_OBJECT *obj, long data);
void update_CCIF_phase_panel(void);

void create_DAC_panel(int x0, int y0);
void enable_all_DAC_callback(FL_OBJECT *obj, long data);
void write_DAC_callback(FL_OBJECT *obj, long data);
void read_DAC_callback(FL_OBJECT *obj, long data);
void update_DAC_panel(void);

void create_CC_temperature_panel(int x0, int y0);
void read_CC_temperature_callback(FL_OBJECT *obj, long data);
void start_DTS_readout_callback(FL_OBJECT *obj, long data);
void stop_DTS_readout_callback(FL_OBJECT *obj, long data);
void update_CC_temperature_panel(void);

void create_ADC_panel(int x0, int y0);
void read_ADC_callback(FL_OBJECT *obj, long data);
void update_ADC_panel(void);

void create_CCBT_panel(int x0, int y0);
void setup_CCBT_board_callback(FL_OBJECT *obj, long data);
void read_CCBT_data_callback(FL_OBJECT *obj, long data);
void update_CCBT_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 1025, HH = 575;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_crate_model_selector_panel(200, 0);

  create_CCIF_status_panel( 0, 45);
  create_CCIF_phase_panel(275, 45);

  create_DAC_panel(             0, 235);
  create_CC_temperature_panel(180, 235);

  create_ADC_panel( 430, 45);
  create_CCBT_panel(675, 45);

  create_USCM_control_H3_panel(200, 490);

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
  
  invalidate_ww(first, &ww_crt, 1, 0);

  for (i=0; i<4; i++) {
    wri_ccif_sta[i].inp = 0x0000;
    wri_ccif_sta[i].unk = TRUE;
    rea_ccif_sta[i].unk = TRUE;
    rea_ccif_sta[i].sta = status_NA;
    if (first) rea_ccif_sta[i].rea = malloc(4);
    if (first) rea_ccif_sta[i].was = malloc(4);
  }

  for (i=0; i<4; i++) {
    wri_ccif_pha[i].inp = 0x0000;
    wri_ccif_pha[i].unk = TRUE;
    rea_ccif_pha[i].unk = TRUE;
    rea_ccif_pha[i].sta = status_NA;
    if (first) rea_ccif_pha[i].rea = malloc(4);
    if (first) rea_ccif_pha[i].was = malloc(4);
  }

  for (i=0; i<8; i++) {
    rea_tmp[i].unk = TRUE;
    rea_tmp[i].sta = status_NA;
    if (first) rea_tmp[i].rea = malloc(4);
    if (first) rea_tmp[i].was = malloc(4);
  }

   rea_dts_sta.unk = TRUE;
   rea_dts_sta.sta = status_NA;
   if (first) rea_dts_sta.rea = malloc(16*4);
   if (first) rea_dts_sta.was = malloc(16*4);

  ena_dac_wri.inp = 0;
  ena_dac_wri.unk = TRUE;
  wri_dac_sta.inp = 0x0000;
  wri_dac_sta.unk = TRUE;
  rea_dac_sta.unk = TRUE;
  rea_dac_sta.sta = status_NA;
  if (first) rea_dac_sta.rea = malloc(4);
  if (first) rea_dac_sta.was = malloc(4);

  rea_adc.unk = TRUE;
  rea_adc.sta = status_NA;
  if (first) rea_adc.rea = malloc(32*4);
  if (first) rea_adc.was = malloc(32*4);

  invalidate_ww(first, &w_ccbt_ctl, 1, 0);
  invalidate_r(first, &r_ccbt_dat, 2*(1+1+1+32));

  read_dat_config_file(dat_config_file_name);
  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(ww_crt.inp, 4, 1, file);
    for (i=0; i<4; i++) {
      fread(&wri_ccif_sta[i].inp, sizeof(wri_ccif_sta[i].inp), 1, file);
      fread(&wri_ccif_pha[i].inp, sizeof(wri_ccif_pha[i].inp), 1, file);
    }
    fread(&ena_dac_wri.inp, sizeof(ena_dac_wri.inp), 1, file);
    fread(&wri_dac_sta.inp, sizeof(wri_dac_sta.inp), 1, file);
    fread(w_ccbt_ctl.inp, 4, 1, file);
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
    fwrite(ww_crt.inp, 4, 1, file);
    for (i=0; i<4; i++) {
      fwrite(&wri_ccif_sta[i].inp, sizeof(wri_ccif_sta[i].inp), 1, file);
      fwrite(&wri_ccif_pha[i].inp, sizeof(wri_ccif_pha[i].inp), 1, file);
    }
    fwrite(&ena_dac_wri.inp, sizeof(ena_dac_wri.inp), 1, file);
    fwrite(&wri_dac_sta.inp, sizeof(wri_dac_sta.inp), 1, file);
    fwrite(w_ccbt_ctl.inp, 4, 1, file);
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
//            CRATE MODEL SELECTOR PANEL
//
//~============================================================================

_b crt = {0, 0x00000003, 4};
  
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_crate_model_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 40, h1 = 15, h2 = 20;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Crate", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, crt_nam, 'M', &ww_crt, &crt, &CRT_BTN);
}

//~----------------------------------------------------------------------------

void update_crate_model_selector_panel(void) {

  switch (*ww_crt.inp) {
    case 0:  adc = &EM1_ADC[0]; break;
    case 1:  adc = &EM2_ADC[0]; break;
    case 2:  adc =  &QM_ADC[0]; break;
    case 3:  adc =  &FM_ADC[0]; break;
    default: adc = &EM1_ADC[0]; break;
  }
}

//~============================================================================
//
//                  CCIF STATUS PANEL
//
//~============================================================================

void create_CCIF_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 65, w2 = 135, w3 = 65, w4 = 15;
  int h1 = 20, h2 = 15;
  char *left[8] = {
    "PA1 Power",
    "PA1 Run",
    "PA2 Power",
    "PA2 Run",
    "PAR Coil 2",
    "PAR Coil 1",
    "LLR Coil 2",
    "LLR Coil 1"};
  char *right[8] = {
    "PA1 Power",
    "PA1 Run",
    "PA2 Power",
    "PA2 Run",
    "PA1 Pwr FF",
    "PA2 Pwr FF",
    "PAR Status",
    "LLR Status"};
  char *command[1] = {"Command"};
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 2 * h1 + 8 * h2 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Write\nStatus", 'M');
  x = x + w1;
  
  add_frame_box(x, y, w2, h1, "CCIF Status", 'M');
  x = x + w2;

  add_frame_box(x, y, w3, 2*h1, "Read\nStatus", 'M');

  x = x0 + 5 + w1;
  y = y + h1;
  create_frame_box_hstring(4, x, y,    2*w4, h1, &dec[0], 'N'+100, 'C');

  x = x0 + 5;
  y = y + h1;
  create_frame_box_vstring(8, x, y,      w1, h2, left,    'S', 'L');
  create_frame_box_vstring(1, x, y+8*h2, w1, h1, command, 'S', 'R');
  x = x + w1;

  x = x + w2;
  create_frame_box_vstring(8, x, y,      w3, h2, right,   'S', 'L');
  create_frame_box_vstring(1, x, y+8*h2, w1, h1, command, 'S', 'L');

  x = x0 + 5 + w1;
  for (i=0; i<4; i++) {
    create_bit_btn_vstring(
      -8, x, y, w4, h2, hex, 'N', &wri_ccif_sta[i], &CCIF_STA_BTN[i][0]);
    obj = add_normal_button(x, y+8*h2, w4, h1, "W", 'S');
      fl_set_object_callback(obj, write_CCIF_status_callback, i);
    x = x + w4;
    create_data_box_vstring(8, x, y, w4, h2, 'N', &CCIF_STA_BOX[i][0]);
    obj = add_normal_button(x, y+8*h2, w4, h1, "R", 'S');
      fl_set_object_callback(obj, read_CCIF_status_callback, i);
    x = x + w4 + 5;
  }
}

//~----------------------------------------------------------------------------

void write_CCIF_status_callback(FL_OBJECT *obj, long data) {

  int i = data;
  _w *w = &wri_ccif_sta[i];
  _r *r = &rea_ccif_sta[i];
  int8 adr[4] = {0x00, 0x04, 0x08, 0x0C};
  
  r->sta = status_NA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  write_LVDS_bus(adr[i], w->inp, &err);
  
  if (!err) {
    w->wri = w->inp;
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

void read_CCIF_status_callback(FL_OBJECT *obj, long data) {

  int i = data;
  _r *r = &rea_ccif_sta[i];
  int16 dat;
  int8 adr[4] = {0x00, 0x04, 0x08, 0x0C};
  
  msg = -1;
  update_objects();

  *r->was = *r->rea;
  
  read_LVDS_bus(adr[i], &dat, &err);
  
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

//~----------------------------------------------------------------------------

void update_CCIF_status_panel(void) {

  int i;
  
  for (i=0; i<4; i++) {
    _w *w = &wri_ccif_sta[i];
    _r *r = &rea_ccif_sta[i];
    show_bin_btn_string(-8, w, &CCIF_STA_BTN[i][0]);
    show_bit_box_string(-8, r, NULL, &CCIF_STA_BOX[i][0]);
  }
}

//~============================================================================
//
//                  CCIF PHASE SHIFT PANEL
//
//~============================================================================

void create_CCIF_phase_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 30, w3 = 45, w4 = 30, w5 = 20;
  int h1 = 40, h2 = 20, h3 = 15;
  
  w = 5 + w1 + w2 + 5 + w3 + w4 + w5 + 5;
  h = 5 + h1 + h2 + 8 * h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "CCIF Phase Shift", 'M');
  y = y + h1;
  
  x = x0 + 5;
  add_frame_box(x, y, w1, h2, "#", 'N');
  x = x + w1;
  add_frame_box(x, y, w2, h2, "Hex", 'N');
  x = x + w2 + 5;
  add_frame_box(x, y, w3, h2, "Dec", 'N');
  x = x + w3;
  add_frame_box(x, y, w4, h2, "Pha", 'N');
  x = x + w4;
  add_frame_box(x, y, w5, h2, "", 'N');
  x = x + w5;
  
  x = x0 + 5;
  y = y + h2;
  create_frame_box_vstring(4, x, y, w1, 2*h3, &dec[0], 'N', 'C');

  for (i=0; i<4; i++) {
    x = x0 + 5 + w1;
    create_hex_btn_hstring(
      2, x, y, w2/2, h3, hex, 'N', &wri_ccif_pha[i], &CCIF_PHA_HEX_BTN[i][0]);
    x = x + w2 + 5;
    create_dec_btn_hstring(
      3, x, y, w3/3, h3, hex, 'N', &wri_ccif_pha[i], &CCIF_PHA_DEC_BTN[i][0]);
    x = x + w3;
    create_data_box_hstring(1, x, y, w4, h3, 'N', CCIF_PHA_WRI_BOX[i]);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h3, "W", 'S');
      fl_set_object_callback(obj, write_CCIF_phase_callback, i);
    x = x0 + 5 + w1;
    y = y + h3;
    create_data_box_hstring(2, x, y, w2/2, h3, 'N', CCIF_PHA_HEX_BOX[i]);
    x = x + w2 + 5;
    create_data_box_hstring(3, x, y, w3/3, h3, 'N', CCIF_PHA_DEC_BOX[i]);
    x = x + w3;
    create_data_box_hstring(1, x, y, w4,   h3, 'N', CCIF_PHA_REA_BOX[i]);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h3, "R", 'S');
      fl_set_object_callback(obj, read_CCIF_phase_callback, i);
    y = y + h3;
  }
}

//~----------------------------------------------------------------------------

void write_CCIF_phase_callback(FL_OBJECT *obj, long data) {

  int i = data;
  _w *w = &wri_ccif_pha[i];
  _r *r = &rea_ccif_pha[i];

  int8 adr[4] = {0x01, 0x05, 0x09, 0x0D};
  
  r->sta = status_NA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  write_LVDS_bus(adr[i], w->inp & 0x00FF, &err);
  
  if (!err) {
    w->wri = w->inp;
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

void read_CCIF_phase_callback(FL_OBJECT *obj, long data) {

  int i = data;
  _r *r = &rea_ccif_pha[i];
  int16 dat;
  int8 adr[4] = {0x01, 0x05, 0x09, 0x0D};
  
  msg = -1;
  update_objects();

  *r->was = *r->rea;
  
  read_LVDS_bus(adr[i], &dat, &err);

  if (!err) {
    *r->rea = dat & 0x00FF;
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

void update_CCIF_phase_panel(void) {

  int i, j;
  
  for (i=0; i<4; i++) {
    char b[80];
    _w *w = &wri_ccif_pha[i];
    _r *r = &rea_ccif_pha[i];
    _r qq, *q = &qq;
    int32 qqq, qqqq;
    q->unk = FALSE;
    q->sta = w->unk ? status_NA : status_OK;
    q->rea = &qqq;
    q->was = &qqqq;
    *q->rea = w->inp;
    *q->was = w->wri;
    show_bin_btn_string(2, w, &CCIF_PHA_HEX_BTN[i][0]);
    show_dec_btn_string(3, w, &CCIF_PHA_DEC_BTN[i][0]);
    show_hex_box_string(2, r, NULL, &CCIF_PHA_HEX_BOX[i][0]);
    show_dec_box_string(3, r, NULL, &CCIF_PHA_DEC_BOX[i][0]);
    sprintf(b, "%3.0f", ((float)*q->rea + 1) * 180.0 / 256.0);
    show_num_box_string(1, q, NULL, b, CCIF_PHA_WRI_BOX[i]);
    sprintf(b, "%3.0f", ((float)*r->rea + 1) * 180.0 / 256.0);
    show_num_box_string(1, r, NULL, b, CCIF_PHA_REA_BOX[i]);
    if (w->inp > 255) {
      for (j=0; j<2; j++) fl_set_object_color(CCIF_PHA_HEX_BTN[i][j]->obj, FL_RED, 0);
      for (j=0; j<3; j++) fl_set_object_color(CCIF_PHA_DEC_BTN[i][j]->obj, FL_RED, 0);
      fl_set_object_color(CCIF_PHA_WRI_BOX[i][0], FL_RED, 0);
    }
  }
}

//~============================================================================
//
//                  DAC PANEL
//
//~============================================================================

void create_DAC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 20, w2 = 75, w3 = 15, w4 = 30;
  int h1 = 20, h2 = 15;
    
  w = 5 + w1 + w2 + w3 + 2 * w4 + 5;
  h = 5 + 2 * h1 + 16 * h2 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  w = w1 + w2;
  add_frame_box(x, y, w, 2*h1, "DAC", 'M');
  x = x + w;
  w = w3 + 2 * w4;
  add_frame_box(x, y, w, h1, "Status", 'M');
  y = y + h1;
  w = w3 + w4;
  add_frame_box(x, y, w, h1, "W", 'N');
  x = x + w;
  add_frame_box(x, y, w4, h1, "R", 'N');
  y = y + h1;

  x = x0 + 5;
  for (i=0; i<16; i++) {
    struct _DAC *dac = &DAC[i];
    char b[80];
    sprintf(b, "%2d", i);
    obj = add_frame_box(x,    y+h2*i, w1, h2, b,        'S');
    obj = add_frame_box(x+w1, y+h2*i, w2, h2, dac->nam, 'S');
      fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  }

  x = x0 + 5;
  w = w1 + w2;
  obj = add_frame_box(x, y+16*h2, w, h2, "Command", 'S');
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

  x = x0 + 5 + w1 + w2;
  create_bit_btn_vstring(
    -16, x, y, w3, h2, e_d, 'N', &ena_dac_wri, &DAC_BTN[0][0]);
  x = x + w3;
  create_bit_btn_vstring(
    -16, x, y, w4, h2, o_o, 'N', &wri_dac_sta, &DAC_BTN[1][0]);
  x = x + w4;
  create_data_box_vstring(16, x, y, w4, h2, 'N', &DAC_BOX[0]);
  y = y + 16 * h2;
  
  x = x0 + 5 + w1 + w2;
  obj = add_normal_button(x, y, w3, h2, "@1+", 'N');
    fl_set_object_callback(obj, enable_all_DAC_callback, 0);
  x = x + w3;

  obj = add_normal_button(x, y, w4, h2, "W", 'N');
    fl_set_object_callback(obj, write_DAC_callback, 0);
  x = x + w4;
  
  obj = add_normal_button(x, y, w4, h2, "R", 'N');
    fl_set_object_callback(obj, read_DAC_callback, 0);
  x = x + w4;
}

//~----------------------------------------------------------------------------

void enable_all_DAC_callback(FL_OBJECT *obj, long data) {

  static int cnt = 0;
  _w *e = &ena_dac_wri;

  e->inp = cnt ? 0xFFFF : 0x0000;
  cnt = MOD(cnt+1, 2);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_DAC_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[16];
  _w *e = &ena_dac_wri;
  _w *w = &wri_dac_sta;
  _r *r = &rea_dac_sta;
    
  msg = -1;
  update_objects();

  for (i=0; i<16; i++) {
    int32 m = 1 << i;
    if (e->inp & m) dat[i] = (w->inp & m) ? 0x0FFF : 0x0000;
    else            dat[i] = 0x8000;
  }
  r->sta = status_NA;
  
  write_DAC(dat, &err);

  if (!err) {
    e->wri = e->inp;
    e->unk = FALSE;
    w->wri = w->inp;
    w->unk = FALSE;
  }
  else {
    e->unk = TRUE;
    w->unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_DAC_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[16];
  _r *r = &rea_dac_sta;
    
  msg = -1;
  update_objects();

  *r->was = *r->rea;

  read_DAC(dat, &err);

  if (!err) {
    *r->rea = 0;
    for (i=0; i<16; i++) {
      int32 m = 0x00000003 << (2 * i);
      if      (dat[i] == 0x0000) pack32(r->rea, m, 0);
      else if (dat[i] == 0x0FFF) pack32(r->rea, m, 1);
      else                       pack32(r->rea, m, 2);
    }
    if (r->unk) *r->was = *r->rea;
    r->unk  = FALSE;
    r->sta  = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_DAC_panel(void) {

  int i;
  _w *e = &ena_dac_wri;
  _w *w = &wri_dac_sta;
  _r *r = &rea_dac_sta;

  show_bin_btn_string(-16, e,      &DAC_BTN[0][0]);
  show_bin_btn_string(-16, w,      &DAC_BTN[1][0]);
  for (i=0; i<16; i++) {
    int m = 0x00000003 << (2 * i);
    show_msk_box(m, r, o_o, DAC_BOX[i]);
  }
}

//~============================================================================
//
//                  CC TEMPERATURE PANEL
//
//~============================================================================

void create_CC_temperature_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 15, w2 = 50, w3 = 55, w4 = 35, w5 = 20;
  int h1 = 20, h2 = 45, h3 = 15, h4 = 20;
  char *txt[8] = {"H-Reject", "Case", "H-Reject", "Case",
                  "H-Reject", "Case", "H-Reject", "Case"}; 
  
  w = 5 + w1 + w2 + w3 + 5 + w4 + 4 * w5 + 5;
  h = 5 + h1 + 8 * h3 + h4 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2+w3, h1, "CC Temperature", 'M');
  y = y + h1;
  
  x = x0 + 5;
  create_frame_box_vstring(4, x, y, w1, 2*h3, &dec[0], 'N', 'C');
  create_frame_box_vstring(8, x+w1, y, w2,   h3, &txt[0], 'S', 'L');
  create_data_box_vstring(8,  x+w1+w2, y, w3,   h3, 'N', CC_TMP_BOX);
  obj = add_normal_button(x, y+8*h3, w-10, h4, "READ", 'N');
    fl_set_object_callback(obj, read_CC_temperature_callback, 0);

  x = x0 + 5 + w1 + w2 + w3 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w4+4*w5, h2, "Dallas\nReadout\nControl", 'N');
  y = y + h2;

  add_frame_box(x, y, w4, h3, "Bus", 'S');
  create_frame_box_hstring(4, x+w4, y, w5, h3, &dec[0], 'S', 'C');
  y = y + h3;

  add_frame_box(x, y, w4, h3, "Scan", 'S');
  create_data_box_hstring(4, x+w4, y, w5, h3, 'N', &DTS_STA_BOX[0]);
  y = y + h3;

  add_frame_box(x, y, w4, h3, "Read", 'S');
  create_data_box_hstring(4, x+w4, y, w5, h3, 'N', &DTS_STA_BOX[4]);
  y = y + h3;

  add_frame_box(x, y, w4, h3, "Good", 'S');
  create_data_box_hstring(4, x+w4, y, w5, h3, 'N', &DTS_STA_BOX[8]);
  y = y + h3;

  add_frame_box(x, y, w4, h3, "Stat.", 'S');
  create_data_box_hstring(4, x+w4, y, w5, h3, 'N', &DTS_STA_BOX[12]);
  y = y + h3;

  obj = add_normal_button(x, y, w4+w5, h4, "START", 'N');
    fl_set_object_callback(obj, start_DTS_readout_callback, 0);
  x = x + w4 + w5;
  obj = add_normal_button(x, y, 3*w5, h4, "STOP", 'N');
    fl_set_object_callback(obj, stop_DTS_readout_callback, 0);
}

//~----------------------------------------------------------------------------

void read_CC_temperature_callback(FL_OBJECT *obj, long data) {

  int i, j;
  int8 ena, par, sta[8], man, mdw, mdr;
  int8 nscan[9], nread, ngood;
  bool ok[64];
  float tmp[64];
  _r *s = &rea_dts_sta;
  int8 bus[4] = {1, 2, 5, 6};
  int LUT[4][2] = {{0, 2}, {1, 3}, {4, 6}, {5, 7}};   //Thank you, Ulf

  msg = -1;
  update_objects();

  err = 0x0000;
  
  if (!err) {
    for (i=0; i<4; i++) s->was[i] = s->rea[i];
    check_DS1820_scan(NULL, nscan, &err);
    if (!err) {
      for (i=0; i<4; i++) s->rea[i] = nscan[bus[i]-1];
      if (s->unk) for (i=0; i<4; i++) s->was[i] = s->rea[i];
      s->unk = FALSE;
      s->sta = status_OK;
    }
    else {
      s->sta = status_KO;
      for (i=0; i<8; i++) rea_tmp[i].sta = status_KO;
    }
  }

  if (!err) {
    for (i=0; i<4; i++) s->was[i+12] = s->rea[i+12];
    check_DS1820_readout(&ena, &par, &sta[0], &man, &mdw, &mdr, &err);
    if (!err) {
      for (i=0; i<4; i++) s->rea[i+12] = (ena  & (1 << (bus[i] - 1))) ? 1 : 0;
      if (s->unk) for (i=0; i<4; i++) s->was[i+12] = s->rea[i+12];
      s->unk = FALSE;
      s->sta = status_OK;
    }
    else {
      s->sta = status_KO;
      for (i=0; i<8; i++) rea_tmp[i].sta = status_KO;
    }
  }

  if (!err) {
    for (i=0; i<4; i++) {
      for (j=0; j<2; j++) ok[j] = FALSE;
      s->was[i+4] = s->rea[i+4];
      s->was[i+8] = s->rea[i+8];
      read_DS1820_temp_long(bus[i], &nread, ok, tmp, NULL, &err);
      if (!err) {
        s->rea[i+4] = nread;
        if (s->unk) s->was[i+4] = s->rea[i+4];
        if (s->unk) s->was[i+8] = s->rea[i+8];
        s->unk = FALSE;
        s->sta = status_OK;
        ngood = 0;
        for (j=0; j<2; j++) {
          _r *r = &rea_tmp[LUT[i][j]];
          if (ok[j]) {
            *r->rea = *(int32*)&tmp[j];
            *r->was = *r->rea;
            r->unk = FALSE;
            r->sta = status_OK;
            ngood = ngood + 1;
          }
          else {
            *r->rea = *r->was;
            r->sta = status_KO;
          }
        }
        s->rea[i+8] = ngood;
      }
      else {
        for (j=0; j<2; j++) {
          _r *r = &rea_tmp[LUT[i][j]];
          r->sta = status_KO;
        }
        s->sta = status_KO;
      }
    }
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void start_DTS_readout_callback(FL_OBJECT *obj, long data) {

  int i;
  int8 msk, ena, par, sta[8], man, mdw, mdr, rst;
  _r *r = &rea_dts_sta;
  int8 bus[4] = {1, 2, 5, 6};

  msg = -1;
  update_objects();

  err = 0x0000;

  r->sta = status_NA;
  for (i=0; i<8; i++) rea_tmp[i].sta = status_NA;
  
  if (!err) {
    msk = 0x00;
    for (i=0; i<4; i++) {
      int8 m = 1 << (bus[i] - 1);
      msk |= m;
    }
    start_DS1820_scan(msk, &err);
    delay(2.0);                                               // Thank you, HBB
  }
  
  if (!err) {
    check_DS1820_readout(&ena, &par, &sta[0], &man, &mdw, &mdr, &err);
  }

  if (!err) {
    ena |=  msk;
    rst  =  msk;
    man &= ~msk;
    mdw &= ~msk;
    delay(2.0);                                               // Thank you, HBB
    setup_DS1820_readout(ena, rst, man, mdw, par, &err);
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void stop_DTS_readout_callback(FL_OBJECT *obj, long data) {

  int i;
  int8 msk;
  int8 ena, par, sta[8], man, mdw, mdr, rst;
  _r *r = &rea_dts_sta;
  int8 bus[4] = {1, 2, 5, 6};

  msg = -1;
  update_objects();

  err = 0x0000;

  r->sta = status_NA;
  for (i=0; i<8; i++) rea_tmp[i].sta = status_KO;

  if (!err) {
    check_DS1820_readout(&ena, &par, &sta[0], &man, &mdw, &mdr, &err);
  }

  if (!err) {
    msk = 0x00;
    for (i=0; i<4; i++) {
      int8 m = 1 << (bus[i] - 1);
      msk |= m;
    }
    ena &= ~msk;
    rst  =  msk;
    setup_DS1820_readout(ena, rst, man, mdw, par, &err);
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_CC_temperature_panel(void) {

  int i;
  char b[80];
  _r *s = &rea_dts_sta;
  
  for (i=0; i<8; i++) {
    _r *r = &rea_tmp[i];
    sprintf(b, "%+.2f", *(float*)r->rea);
    show_num_box_string(1, r,  NULL, b, &CC_TMP_BOX[i]);
  }

  for (i=0; i<12; i++) {
    sprintf(b, "%d", s->rea[i]);
    show_num_box_string(1, s, NULL, b, &DTS_STA_BOX[i]);
  }

  for (i=12; i<16; i++) {
    if      (s->rea[i] == 0) sprintf(b, "D");
    else if (s->rea[i] == 1) sprintf(b, "E");
    else                     sprintf(b, "?");
    show_num_box_string(1, s, NULL, b, &DTS_STA_BOX[i]);
  }
}

//~============================================================================
//
//                  ADC PANEL
//
//~============================================================================

void ADC_browser_callback(FL_OBJECT *obj, long data) {

  printf("ADC browser callback\n");
}

////////////////////////////////////////////////////////////

void create_ADC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 215;
  int h1 = 20, h2 = 15, h3 = 425, h3a = 215, h3b = 210, h4 = 30;
  char *legend = {"Nb  Channel Hex  Dec   Value"};
    
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + h3 + h4 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1+h2, "", 'M');
  y = y + h1 + h2;
  add_frame_box(x, y, w1, h3a, "C\nC\nM\nO\n\n0", 'M');
  y = y + h3a;
  add_frame_box(x, y, w1, h3b, "C\nC\nM\nO\n\n1", 'M');
  y = y + h3b;
  add_frame_box(x, y, w1, h4, "", 'M');

  x = x0 + 5 + w1;
  y = y0 + 5;
  add_frame_box(x, y, w2, h1, "ADC", 'M');
  y = y + h1;

  obj = add_frame_box(x, y, w2, h2, legend, 'N');
    fl_set_object_lstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  y = y + h2;

  ADC_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w2, h3, "");
///////////////////////////////
//  fl_set_object_callback(obj, ADC_browser_callback, 0);
///////////////////////////////
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
  y = y + h3;
  
  obj = add_normal_button(x, y, w2, h4, "READ", 'N');
    fl_set_object_callback(obj, read_ADC_callback, 0);
}

//~----------------------------------------------------------------------------

void read_ADC_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[32];
  _r *r = &rea_adc;
    
  msg = -1;
  update_objects();

  for (i=0; i<32; i++) r->was[i] = r->rea[i];

  read_ADC(dat, &err);

  if (!err) {
    for (i=0; i<32; i++) r->rea[i] = dat[i];
    if (r->unk) for (i=0; i<32; i++) r->was[i] = r->rea[i];
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

void update_ADC_panel(void) {

  FL_OBJECT *obj;
  int i;
  _r *r = &rea_adc;

  fl_freeze_form(MAIN);

  obj = ADC_BRO;
  fl_clear_browser(obj);
  for (i=0; i<32; i++) {
    char b[80];
    int16 cod = r->rea[i];
    float val;
    if (adc[i].A > 100000.0) val = adc[i].A / MAX(1.0, (float)cod);
    else                     val = adc[i].A * (float)cod + adc[i].B;
    if (r->unk) {
      sprintf(b, "%2d %s", i, adc[i].nam);
    }
    else {
      sprintf(b, "%2d %s %03X %4d %7.3f", i, adc[i].nam, cod, cod, val);
    }
    fl_add_browser_line(obj, b);
    if (r->rea[i] != r->was[i]) fl_select_browser_line(obj, i+1);
  }

  if (r->unk) fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  else        fl_set_object_color(obj, FL_WHITE, FL_YELLOW);

  fl_unfreeze_form(MAIN);
}

//~============================================================================
//
//                  CCBT PANEL
//
//~============================================================================

_b ccbt_ctl[2] = {
   { 0, 0x00000001, 2},
   { 0, 0x00000002, 2}};

_b ccbt_use[2] = {
   { 0+ 0, 0x0001, 2},
   { 0+35, 0x0001, 2}};

_b ccbt_sta[2] = {
   { 1+ 0, 0x0001, 2},
   { 1+35, 0x0001, 2}};

_b ccbt_age[2] = {
   { 2+ 0, 0xFFFF, -1},
   { 2+35, 0xFFFF, -1}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_CCBT_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 170;
  int w2 = 40, w3 = 55, w4 = 30, w5 = 65, w6 = 10, w7 = 30, w8 = 70;
  int h1 = 20, h2 = 15, h3 = 425, h4 = 15;
  char b[80];
  char *legend = {"Chan Code   Dec  Value"};
    
  w = 5 + 2 * w1 + 5;
  h = 5 + h1 + h2 + h3 + 2 * h4 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  for (i=0; i<2; i++) {
    y = y0 + 5;
    sprintf(b, "CCBT %d", i);
    add_frame_box(x, y, w1, h1, b, 'M');
    y = y + h1;

    obj = add_frame_box(x, y, w1, h2, legend, 'N');
      fl_set_object_lstyle(obj, FL_FIXEDBOLD_STYLE);
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    y = y + h2;

   CCBT_BRO[i] =
    obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1, h3, "");
      fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
      fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
      fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
      fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
    y = y + h3;
    
    x = x + w1;
  }
  
  x = x0 + 5;
  for (i=0; i<2; i++) {
    add_frame_box(x, y, w2, h4, "Status", 'S');
    create_dat_box_hstring(
      1, x+w2, y, w3, h4, 'N',  &r_ccbt_dat, &ccbt_sta[i], &CCBT_STA_BOX[i]);
    add_frame_box(x, y+h4, w4, h4, "Age", 'S');
    create_dat_box_hstring(
      1, x+w4, y+h4, w5, h4, 'N', &r_ccbt_dat, &ccbt_age[i], &CCBT_AGE_BOX[i]);
    x = x + w2 + w3;

    if (i) break;
    add_frame_box(x, y, w6, 2*h4, "", 'S');
    x = x + w6;
    create_btn_hstring(
      1, x, y,    w7, h4, o_o, 'N', &w_ccbt_ctl, &ccbt_ctl[0], &CCBT_CTL_BTN[0]);
    create_dat_box_hstring(
      1, x, y+h4, w7, h4, 'N', &r_ccbt_dat, &ccbt_use[0], &CCBT_USE_BOX[0]);
    x = x + w7;
    obj = add_normal_button(x, y,    w8, h4, "WRITE", 'N');
      fl_set_object_callback(obj, setup_CCBT_board_callback, 0);
    obj = add_normal_button(x, y+h4, w8, h4, "READ", 'N');
      fl_set_object_callback(obj, read_CCBT_data_callback, 0);
    x = x + w8;
    create_btn_hstring(
      1, x, y,    w7, h4, o_o, 'N', &w_ccbt_ctl, &ccbt_ctl[1], &CCBT_CTL_BTN[1]);
    create_dat_box_hstring(
      1, x, y+h4, w7, h4, 'N', &r_ccbt_dat, &ccbt_use[1], &CCBT_USE_BOX[1]);
    x = x + w7;
    add_frame_box(x, y, w6, 2*h4, "", 'S');
    x = x + w6;
  }
}

//~----------------------------------------------------------------------------

void setup_CCBT_board_callback(FL_OBJECT *obj, long data) {

  _ww *w = &w_ccbt_ctl;
  _r  *r = &r_ccbt_dat;

  if (obj) {
    msg = -1;
    update_objects();
  }

  w->sta = status_NA;
  r->sta = status_NA;
  
  setup_CCBT_task(w->inp[0] & 0x00000003, &err);
  
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

void read_CCBT_data_callback(FL_OBJECT *obj, long data) {

  int i, j;
  _CCBT_data dat[2];
  _r *r = &r_ccbt_dat;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<70; i++) r->was[i] = r->rea[i];
  
  read_CCBT_data(dat, &err);

  if (!err) {
    for (j=0; j<2; j++) {
      r->rea[35*j+0] = dat[j].use;
      r->rea[35*j+1] = dat[j].ok;
      r->rea[35*j+2] = dat[j].age;
      for (i=0; i<32; i++) r->rea[35*j+3+i] = dat[j].value[i];
    }
    if (r->unk) for (i=0; i<70; i++) r->was[i] = r->rea[i];
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

void update_CCBT_panel(void) {

  int i, j;
  char b[80];
  _r *r = &r_ccbt_dat;

  fl_freeze_form(MAIN);
  for (j=0; j<2; j++) {
    FL_OBJECT *obj = CCBT_BRO[j];
    fl_clear_browser(obj);
    for (i=0; i<32; i++) {
      int k = 35 * j + 3 + i;
      struct _CCBT *ccbt = &CCBT[i][j];
      short cod;
      float val;
      cod = (short)((r->rea[k] & 0x0FFF) - 0x0800);
      val = ccbt->A * (float)cod + ccbt->B;
      if (r->unk) sprintf(b, "%s",                  ccbt->nam);
      else        sprintf(b, "%s %04X %+5d %+6.1f", ccbt->nam, r->rea[k], cod, val);
      fl_add_browser_line(obj, b);
      if (r->rea[k] != r->was[k]) fl_select_browser_line(obj, i+1);
    }
    if (r->unk) fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    else        fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
  }
  fl_unfreeze_form(MAIN);

  for (j=0; j<2; j++) {
    int16 age;
    show_btn_string(    1,       NULL, &CCBT_CTL_BTN[j]);
    show_dat_box_string(1,  o_o, NULL, &CCBT_USE_BOX[j]);
    show_dat_box_string(1,  f_o, NULL, &CCBT_STA_BOX[j]);
    age = r->rea[35*j+2];
    sprintf(b, "%02d:%02d:%02d", (age/60)/60, (age/60)%60, (age%60)%60);
    show_dat_box_string(1, NULL,    b, &CCBT_AGE_BOX[j]);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_crate_model_selector_panel();

  update_CCIF_status_panel();
  update_CCIF_phase_panel();
  update_CC_temperature_panel();

  update_DAC_panel();
  update_ADC_panel();
  update_CCBT_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
