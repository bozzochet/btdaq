// file CCEB-3.c
//
// CCEB-3 Controller
//
// A.Lebedev Jan-2007...

#include "template.h"
#include "ccebdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CCEB - 3  Controller"};               // global variable
char *date  = {"15-Oct-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

struct _ADC *adc;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~-- CCEB Crate Model Selector

SW_BTN *MODEL_BTN;
_ww ww_model;
_b model = {0, 0x00000003, 4};

//~--

_w wri_ccif_sta[4];
_r rea_ccif_sta[4];
_w wri_ccif_pha[4];
_r rea_ccif_pha[4];
_w ena_dac_wri;
_w wri_dac_sta;
_r r_dac_sta;
_r rea_adc;
_r rea_ccbt_sta[2];
_r rea_ccbt_dat[2];
_r rea_tmp[8];

_w wri_osc_sel;
_ww ww_ccif_sel;
_w wri_llr_sta[4];
_w wri_feed_sel[4];

//~--

SW_BTN    *DAC_BTN[16] = {NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL};

FL_OBJECT *DAC_BOX[16] = {NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL};

FL_OBJECT *DTS_TMP_BOX[8] =  {NULL, NULL, NULL, NULL,
                              NULL, NULL, NULL, NULL};

//~--

FL_OBJECT *PA_SEL_FRAME[4];

//~--

char *models[] = {"EM1", "EM2", "QM", "FM"};

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *e_d[2] = {" ", "@1+"};
char *o_o[4] = {"OFF", "ON", "?", "??"};
char *n_o[2] = {"NA", "OK"};
char *d_e[2] = {"D", "E"};
char *h_c[2] = {"H", "C"};
char *o_f[2] = {"O.K.", "FAIL"};
char *a_b[4] = {"A", "B", "-", "?"};
char *b_a[2] = {"B", "A"};
char *l_o[2] = {"LOCK", "OPER"};

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

void update_DAC_data(void);
void update_DTS_data(void);

void create_crate_model_selector_panel(int x0, int y0);
void update_crate_model_selector_panel(void);

void create_global_power_panel(int x0, int y0);
void update_global_power_panel(void);

void create_CCMO_CCBT_configuration_panel(int x0, int y0);
void write_CCMO_CCBT_configuration_callback(FL_OBJECT *obj, long data);

void create_oscillator_panel(int x0, int y0);
void read_oscillator_callback(FL_OBJECT *obj, long data);
void start_oscillator_callback(FL_OBJECT *obj, long data);
void stop_oscillator_callback(FL_OBJECT *obj, long data);
void update_oscillator_panel(void);

void create_CC_panel(int x0, int y0, int ind);
void write_CC_config_callback(FL_OBJECT *obj, long data);
void read_CC_config_callback(FL_OBJECT *obj, long data);
void set_CC_power_on_callback(FL_OBJECT *obj, long data);
void set_CC_power_off_callback(FL_OBJECT *obj, long data);
void dec_CC_phase_callback(FL_OBJECT *obj, long data);
void inc_CC_phase_callback(FL_OBJECT *obj, long data);
void set_CC_phase_callback(FL_OBJECT *obj, long data);
void read_CC_monitor_callback(FL_OBJECT *obj, long data);
void update_CC_panel(int ind);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int i;
  int WW = 665, HH = 660;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_crate_model_selector_panel(200, 0);

  create_global_power_panel(             0, 45);
  create_CCMO_CCBT_configuration_panel(270, 45);
  create_oscillator_panel(             450, 45);

  for (i=0; i<4; i++) create_CC_panel(0+i*145, 135, i);

  create_USCM_control_V_panel(580, 160);

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
  
  invalidate_ww(first, &ww_model, 1, 0);

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

  ena_dac_wri.inp = 0x0000;
  ena_dac_wri.unk = TRUE;
  wri_dac_sta.inp = 0x0000;
  wri_dac_sta.unk = TRUE;

  r_dac_sta.unk = TRUE;
  r_dac_sta.sta = status_NA;
  if (first) r_dac_sta.rea = malloc(4);
  if (first) r_dac_sta.was = malloc(4);

  rea_adc.unk = TRUE;
  rea_adc.sta = status_NA;
  if (first) rea_adc.rea = malloc(64*4);
  if (first) rea_adc.was = malloc(64*4);

  for (i=0; i<2; i++) {
    rea_ccbt_sta[i].unk = TRUE;
    rea_ccbt_sta[i].sta = status_NA;
    rea_ccbt_dat[i].unk = TRUE;
    rea_ccbt_dat[i].sta = status_NA;
    if (first) rea_ccbt_sta[i].rea = malloc(4);
    if (first) rea_ccbt_sta[i].was = malloc(4);
    if (first) rea_ccbt_dat[i].rea = malloc(32*4);
    if (first) rea_ccbt_dat[i].was = malloc(32*4);
  }

  for (i=0; i<8; i++) {
    rea_tmp[i].unk = TRUE;
    rea_tmp[i].sta = status_NA;
    if (first) rea_tmp[i].rea = malloc(4);
    if (first) rea_tmp[i].was = malloc(4);
  }

  wri_osc_sel.inp = 0;
  wri_osc_sel.unk = TRUE;

  ww_ccif_sel.unk = TRUE;
  if (first) ww_ccif_sel.inp = malloc(4*4);
  if (first) ww_ccif_sel.wri = malloc(4*4);
  for (i=0; i<4; i++) ww_ccif_sel.inp[i] = 0;

  for (i=0; i<4; i++) {
    wri_llr_sta[i].inp = 0;
    wri_llr_sta[i].unk = TRUE;
    wri_feed_sel[i].inp = 0;
    wri_feed_sel[i].unk = TRUE;
  }

  read_dat_config_file(dat_config_file_name);
  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(ww_model.inp, 4, 1, file);
    for (i=0; i<4; i++) {
      fread(&wri_ccif_sta[i].inp, sizeof(wri_ccif_sta[i].inp), 1, file);
      fread(&wri_ccif_pha[i].inp, sizeof(wri_ccif_pha[i].inp), 1, file);
    }
    fread(&ena_dac_wri.inp, sizeof(ena_dac_wri.inp), 1, file);
    fread(&wri_dac_sta.inp, sizeof(wri_dac_sta.inp), 1, file);
    fread(&wri_osc_sel.inp, sizeof(wri_osc_sel.inp), 1, file);
    fread(ww_ccif_sel.inp, 4, 4, file);
    for (i=0; i<4; i++) {
      fread(&wri_llr_sta[i].inp,  sizeof(wri_llr_sta[i].inp),  1, file);
      fread(&wri_feed_sel[i].inp, sizeof(wri_feed_sel[i].inp), 1, file);
    }
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
    fwrite(ww_model.inp, 4, 1, file);
    for (i=0; i<4; i++) {
      fwrite(&wri_ccif_sta[i].inp, sizeof(wri_ccif_sta[i].inp), 1, file);
      fwrite(&wri_ccif_pha[i].inp, sizeof(wri_ccif_pha[i].inp), 1, file);
    }
    fwrite(&ena_dac_wri.inp, sizeof(ena_dac_wri.inp), 1, file);
    fwrite(&wri_dac_sta.inp, sizeof(wri_dac_sta.inp), 1, file);
    fwrite(&wri_osc_sel.inp, sizeof(wri_osc_sel.inp), 1, file);
    fwrite(ww_ccif_sel.inp, 4, 4, file);
    for (i=0; i<4; i++) {
      fwrite(&wri_llr_sta[i].inp,  sizeof(wri_llr_sta[i].inp),  1, file);
      fwrite(&wri_feed_sel[i].inp, sizeof(wri_feed_sel[i].inp), 1, file);
    }
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

void update_DAC_data(void) {

  int i;
  int32 m;

  for (i=0; i<16; i++) {
    SW_BTN    *btn = DAC_BTN[i];
    FL_OBJECT *obj = DAC_BOX[i];
    if (btn) show_bin_btn_string(1, &wri_dac_sta, &btn);
    m = 0x00000003 << (2 * i);
    if (obj) show_msk_box(m, &r_dac_sta, o_o, obj);
  }
}

//~----------------------------------------------------------------------------

void update_DTS_data(void) {

  int i;
  
  for (i=0; i<8; i++) {
    _r *r = &rea_tmp[i];
    show_float_num_box(r, 0, "%+.2f", DTS_TMP_BOX[i]);
  }
}

//~============================================================================
//
//                  COMMON CALLBACKS
//
//~============================================================================

void write_CCIF_status_callback(FL_OBJECT *obj, long data);
void read_CCIF_status_callback(FL_OBJECT *obj, long data);
void write_CCIF_phase_callback(FL_OBJECT *obj, long data);
void read_CCIF_phase_callback(FL_OBJECT *obj, long data);
void write_DAC_callback(FL_OBJECT *obj, long data);
void read_DAC_callback(FL_OBJECT *obj, long data);
void read_ADC_callback(FL_OBJECT *obj, long data);
void read_CCBT_data_callback(FL_OBJECT *obj, long data);
void read_DTS_data_callback(FL_OBJECT *obj, long data);

//~----------------------------------------------------------------------------

void write_CCIF_status_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _w *w = &wri_ccif_sta[ind];
  _r *r = &rea_ccif_sta[ind];
  int8 adr[4] = {0x00, 0x04, 0x08, 0x0C};
  
  r->sta = status_NA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  write_LVDS_bus(adr[ind], w->inp, &err);
  
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

  int ind = data;
  _r *r = &rea_ccif_sta[ind];
  int16 dat;
  int8 adr[4] = {0x00, 0x04, 0x08, 0x0C};
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  *r->was = *r->rea;
  
  read_LVDS_bus(adr[ind], &dat, &err);
  
  if (!err) {
    *r->rea = dat;
    if (r->unk) *r->was = *r->rea;
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

void write_CCIF_phase_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _w *w = &wri_ccif_pha[ind];
  _r *r = &rea_ccif_pha[ind];
  int8 adr[4] = {0x01, 0x05, 0x09, 0x0D};
  
  r->sta = status_NA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  write_LVDS_bus(adr[ind], w->inp & 0x00FF, &err);
  
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

  int ind = data;
  _r *r = &rea_ccif_pha[ind];
  int16 dat;
  int8 adr[4] = {0x01, 0x05, 0x09, 0x0D};
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  *r->was = *r->rea;
  
  read_LVDS_bus(adr[ind], &dat, &err);

  if (!err) {
    *r->rea = dat & 0x00FF;
    if (r->unk) *r->was = *r->rea;
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

void write_DAC_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[16];
  _w *e = &ena_dac_wri;
  _w *w = &wri_dac_sta;
  _r *r = &r_dac_sta;
    
  if (obj) {
    msg = -1;
    update_objects();
  }

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
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_DAC_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[16];
  _r *r = &r_dac_sta;
    
  if (obj) {
    msg = -1;
    update_objects();
  }

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
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_ADC_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[32];
  _r *r = &rea_adc;
  int OSC_OK[2] = {
    OSC_H_OK, OSC_C_OK};
  struct {
    int V;
    int I;
    int P_or_Z;
  } ind[] = {
    {V_FEED_A, I_FEED_A, P_FEED_A},
    {V_FEED_B, I_FEED_B, P_FEED_B},
    {V_BUS_0,  I_BUS_0,  P_BUS_0},
    {V_BUS_1,  I_BUS_1,  P_BUS_1},
    {V_BUS_2,  I_BUS_2,  P_BUS_2},
    {V_BUS_3,  I_BUS_3,  P_BUS_3},
    {V_RMS_0,  I_RMS_0,  Z_RMS_0},
    {V_RMS_1,  I_RMS_1,  Z_RMS_1},
    {V_RMS_2,  I_RMS_2,  Z_RMS_2},
    {V_RMS_3,  I_RMS_3,  Z_RMS_3}};
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<64; i++) r->was[i] = r->rea[i];

  read_ADC(dat, &err);

  if (!err) {
    for (i=0; i<32; i++) {
      float val;
      if (adc[i].A > 100000.0) val = adc[i].A / MAX(1.0, (float)dat[i]);
      else                     val = adc[i].A * (float)dat[i] + adc[i].B;
      r->rea[i] = *(int32*)&val;
    }
    for (i=0; i<sizeof(ind)/sizeof(ind[0]); i++) {
      float V = *(float*)&r->rea[ind[i].V];
      float I = *(float*)&r->rea[ind[i].I];
      float P = V * I;
      float Z = I ? V / I : 0.0;
      int k = ind[i].P_or_Z;
      if (k == Z_RMS_0 || k == Z_RMS_1 || k == Z_RMS_2 || k == Z_RMS_3) {
        r->rea[ind[i].P_or_Z] = *(int32*)&Z;
      }
      else {
        r->rea[ind[i].P_or_Z] = *(int32*)&P;
      }
    }
    for (i=0; i<2; i++) {
      float V = *(float*)&r->rea[OSC_OK[i]];
      if ((V < 3.5) || (V > 4.5)) r->rea[OSC_OK[i]] = 1;
      else                        r->rea[OSC_OK[i]] = 0;
    }
    if (r->unk) for (i=0; i<64; i++) r->was[i] = r->rea[i];
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

void read_CCBT_data_callback(FL_OBJECT *obj, long data) {

  int i, j;
  _CCBT_data CCBT_data[2];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (j=0; j<2; j++) {
    _r *r = &rea_ccbt_dat[j];
    for (i=0; i<32; i++) r->was[i] = r->rea[i];
  }
  
  read_CCBT_data(CCBT_data, &err);

  for (j=0; j<2; j++) {
    _r *r = &rea_ccbt_dat[j];
    if (!err && CCBT_data[j].ok) {
      for (i=0; i<32; i++) r->rea[i] = CCBT_data[j].value[i];
      if (r->unk) for (i=0; i<32; i++) r->was[i] = r->rea[i];
      r->unk = FALSE;
      r->sta = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_DTS_data_callback(FL_OBJECT *obj, long data) {

  int i, j;
  int8 nr;
  bool ok[64];
  float tmp[64];
  int8 bus[4] = {1, 2, 5, 6};
//int LUT[4][2] = {{0, 2}, {1, 3}, {4, 6}, {5, 7}};   //Thank you, Ulf
  int LUT[4][2] = {{0, 4}, {1, 5}, {2, 6}, {3, 7}};   //Thank you, Ulf

  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<4; i++) {
    for (j=0; j<2; j++) {
      _r *r = &rea_tmp[LUT[i][j]];
      ok[j] = FALSE;
      *r->was = *r->rea;
    }
    read_DS1820_temp_long(bus[i], &nr, ok, tmp, NULL, &err);
    for (j=0; j<2; j++) {
      _r *r = &rea_tmp[LUT[i][j]];
      if (!err) {
        if (ok[j]) {
          *r->rea = *(int32*)&tmp[j];
          if (r->unk) *r->was = *r->rea;
          r->unk = FALSE;
          r->sta = status_OK;
        }
        else {
          *r->rea = *r->was;
          r->sta = status_KO;
        }
      }
      else {
        r->sta = status_KO;
      }
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            CRATE MODEL SELECTOR PANEL
//
//~============================================================================

void create_crate_model_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 40, h1 = 15, h2 = 20;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Model", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, models, 'M', &ww_model, &model, &MODEL_BTN);
}

//~----------------------------------------------------------------------------

void update_crate_model_selector_panel(void) {

  switch (*ww_model.inp) {
    case 0:  adc = &EM1_ADC[0]; break;
    case 1:  adc = &EM2_ADC[0]; break;
    case 2:  adc =  &QM_ADC[0]; break;
    case 3:  adc =  &FM_ADC[0]; break;
    default: adc = &EM1_ADC[0]; break;
  }
}

//~============================================================================
//
//                  GLOBAL POWER PANEL
//
//~============================================================================

FL_OBJECT *V_FEED_BOX[2], *I_FEED_BOX[2], *P_FEED_BOX[2];
FL_OBJECT *I_VCC_BOX, *I_VRR_BOX;

//~----------------------------------------------------------------------------

void create_global_power_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 35, w2 = 45, w3 = 40, w4 = 35, w5 = 45, w6 =40, w7 = 20;
  int h1 = 20;
  char *a_b[2] = {"A", "B"};

  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 +w7 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  w =  w1 + w2 + w3 + w4;
  add_frame_box(x, y, w, h1, "Input 120V Power", 'M');
  x = x + w;
  w = w5 + w6;
  add_frame_box(x, y, w, 2*h1, "Secondary\nPower", 'M');
  x = x + w;
  obj = add_normal_button(x, y, w7, 4*h1, "R", 'N');
    fl_set_object_callback(obj, read_ADC_callback, 0);
  y = y + h1;
  
  x = x0 + 5;
  add_frame_box(x, y, w1, h1, "Feed", 'N');
  x = x + w1;
  add_frame_box(x, y, w2, h1, "V",    'N');
  x = x + w2;
  add_frame_box(x, y, w3, h1, "I",    'N');
  x = x + w3;
  add_frame_box(x, y, w4, h1, "P",    'N');
  y = y + h1;

  for (i=0; i<2; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, h1, a_b[i], 'N');
    x = x + w1;
    V_FEED_BOX[i] =
    obj = add_data_box(x, y, w2, h1, "", 'N');
    x = x + w2;
    I_FEED_BOX[i] =
    obj = add_data_box(x, y, w3, h1, "", 'N');
    x = x + w3;
    P_FEED_BOX[i] =
    obj = add_data_box(x, y, w4, h1, "", 'N');
    x = x + w4;
    if (i == 0) {
      add_frame_box(x, y, w5, h1, "I VCC", 'N');
      x = x + w5;
      I_VCC_BOX =
      obj = add_data_box(x, y, w6, h1, "", 'N');
    }
    else {
      add_frame_box(x, y, w5, h1, "I VRR", 'N');
      x = x + w5;
      I_VRR_BOX =
      obj = add_data_box(x, y, w6, h1, "", 'N');
    }
    y = y + h1;
  }
}

//~----------------------------------------------------------------------------

void update_global_power_panel(void) {

  int i;
  _r *r = &rea_adc;
  int V_FEED[2] = {V_FEED_A, V_FEED_B};
  int I_FEED[2] = {I_FEED_A, I_FEED_B};
  int P_FEED[2] = {P_FEED_A, P_FEED_B};
  
  for (i=0; i<2; i++) {
    show_float_num_box(r, V_FEED[i], "%5.1f", V_FEED_BOX[i]);
    show_float_num_box(r, I_FEED[i], "%4.2f", I_FEED_BOX[i]);
    show_float_num_box(r, P_FEED[i], "%3.0f", P_FEED_BOX[i]);
  }
  
  show_float_num_box(r, I_VCC, "%4.2f", I_VCC_BOX);
  show_float_num_box(r, I_VRR, "%4.2f", I_VRR_BOX);
}

//~============================================================================
//
//                  CCMO & CCBT CONFIGURATION PANEL
//
//~============================================================================

void create_CCMO_CCBT_configuration_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 35, w2 = 20, h1 = 20;

  w = 5 + 2 * (2 * w1 + 5) + w2 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  w = 2 * w1;
  add_frame_box(x, y, w,  h1, "CCMO", 'N');
  x = x + w + 5;
  add_frame_box(x, y, w,  h1, "CCBT", 'N');
  x = x + w + 5;
  h = 2 * h1;
  add_frame_box(x, y, w2, h,  "",     'N');
  y = y + h1;

  x = x0 + 5;
  create_frame_box_hstring(2, x, y, w1, h1, dec, 'N', 'C');
  x = x + 2 * w1 + 5;
  create_frame_box_hstring(2, x, y, w1, h1, dec, 'N', 'C');
  y = y + h1;

  x = x0 + 5;
  for (i=0; i<4; i++) {
    int ind[4] = {CCMO0_ON, CCMO1_ON, CCBT0_ON, CCBT1_ON};
    create_bit_btn_hsstring(
      -1, x, y, w1, h1, o_o, 'M', &wri_dac_sta, ind[i], &DAC_BTN[ind[i]]);
    DAC_BOX[ind[i]] =
    obj = add_data_box(x, y+h1, w1, h1, "", 'M');
    x = x + w1;
    if ((i % 2) == 1) x = x + 5;
  }
  obj = add_normal_button(x, y, w2, h1, "W", 'N');
    fl_set_object_callback(obj, write_CCMO_CCBT_configuration_callback, 0);
  y = y + h1;
  obj = add_normal_button(x, y, w2, h1, "R", 'N');
    fl_set_object_callback(obj, read_DAC_callback, 0);
  y = y + h1;
}

//~----------------------------------------------------------------------------

void write_CCMO_CCBT_configuration_callback(FL_OBJECT *obj, long data) {

  int i;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  if (!err) {
    _w *e = &ena_dac_wri;
    e->inp = 0x00000000;
    for (i=0; i<4; i++) {
      int ind[4] = {CCMO0_ON, CCMO1_ON, CCBT0_ON, CCBT1_ON};
      int32 m = 0x00000001 << ind[i];
      e->inp |= m;
    }
    write_DAC_callback(NULL, 0);
  }

  if (go_on(err)) read_DAC_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//                  OSCILLATOR PANEL
//
//~============================================================================

SW_BTN    *OSC_SEL_BTN[1];
FL_OBJECT *OSC_ADC_BOX[2];

//~----------------------------------------------------------------------------

void create_oscillator_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 35, w3 = 40, w4 = 20, w5 = 30, w6 = 60;
  int h1 = 20;
  char *txt[2] = {"H", "C"};
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Oscillators", 'M');
  y = y + h1;
  
  x = x0 + 5;
  add_frame_box(x, y, w1, h1, "", 'N');
  x = x + w1;
  w = w2 + w3;
  add_frame_box(x, y, w,  h1, "Status", 'N');
  x = x + w;
  obj = add_normal_button(x, y, w4, 3*h1, "R", 'N');
    fl_set_object_callback(obj, read_oscillator_callback, 0);
  y = y + h1;

  x = x0 + 5;
  create_frame_box_vstring(2, x, y, w1, h1, txt, 'N', 'C');
  x = x + w1;

  DAC_BOX[OSC_H_ON] =
  obj = add_data_box(x, y,    w2, h1, "", 'M');
  DAC_BOX[OSC_C_ON] =
  obj = add_data_box(x, y+h1, w2, h1, "", 'M');
  x = x + w2;

  create_data_box_vstring(2, x, y, w3, h1, 'M', &OSC_ADC_BOX[0]);
  x = x + w3;

  x = x + w4;
  y = y0 + h1 + 5;
  add_frame_box(x, y, w5+w6, h1, "Run", 'M');
  y = y + h1;

  add_frame_box(x, y, w5, h1, "Use", 'N');
  create_bit_btn_hstring(
    1, x, y+h1, w5, h1, h_c, 'M', &wri_osc_sel, &OSC_SEL_BTN[0]);
  x = x + w5;
  obj = add_normal_button(x, y,    w6, h1, "START", 'N');
    fl_set_object_callback(obj, start_oscillator_callback, 0);
  obj = add_normal_button(x, y+h1, w6, h1, "STOP", 'N');
    fl_set_object_callback(obj, stop_oscillator_callback, 0);
}

//~----------------------------------------------------------------------------

void read_oscillator_callback(FL_OBJECT *obj, long data) {

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  if (go_on(err)) read_DAC_callback(NULL, 0);
  if (go_on(err)) read_ADC_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void start_oscillator_callback(FL_OBJECT *obj, long data) {

  _w *s = &wri_osc_sel;
  _w *e = &ena_dac_wri;
  _w *w = &wri_dac_sta;
  int32 m_h = 0x00000001 << OSC_H_ON;
  int32 m_c = 0x00000001 << OSC_C_ON;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  e->inp = m_h | m_c;
  if (s->inp) w->inp = (w->inp & ~m_h ) | m_c;
  else        w->inp = (w->inp & ~m_c ) | m_h;

  err = 0x0000;
  if (go_on(err)) write_DAC_callback(NULL, 0);
  if (go_on(err)) read_oscillator_callback(NULL, 0);

  if (!err) {
    s->wri = s->inp;
    s->unk = FALSE;
  }
  else {
    s->unk = TRUE;
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void stop_oscillator_callback(FL_OBJECT *obj, long data) {

  _w *e = &ena_dac_wri;
  _w *w = &wri_dac_sta;
  int32 m_h = 0x00000001 << OSC_H_ON;
  int32 m_c = 0x00000001 << OSC_C_ON;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  e->inp = m_h | m_c;
  w->inp = (w->inp & ~m_c ) & ~m_h;

  err = 0x0000;
  if (go_on(err)) write_DAC_callback(NULL, 0);
  if (go_on(err)) read_oscillator_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_oscillator_panel(void) {

  FL_OBJECT *obj;
  int i;
  _w *s = &wri_osc_sel;
  _r *d = &r_dac_sta;
  _r *a = &rea_adc;
  int OSC_ON[2] = {OSC_H_ON, OSC_C_ON};
  int OSC_OK[2] = {OSC_H_OK, OSC_C_OK};

  show_bin_btn_string(1, s, &OSC_SEL_BTN[0]);

  for (i=0; i<2; i++) {
    int32 m = 3 << (2 * OSC_ON[i]);
    int sta = unpack32(*d->rea, m);
    obj = OSC_ADC_BOX[i];
    if (sta == 1) {
      show_num_box(a, OSC_OK[i], o_f, NULL, obj);
      if (a->rea[OSC_OK[i]]) {
        fl_set_object_color(obj, FL_RED, 0);
        fl_set_object_lcolor(obj, FL_YELLOW);
      }
    }
    else {
      show_num_box(a, OSC_OK[i], NULL, "OFF", obj);
    }
  }
}

//~============================================================================
//
//                  CC PANELS
//
//~============================================================================

int CCIF_A_ON[4] ={CCIF0_A_ON, CCIF1_A_ON, CCIF2_A_ON, CCIF3_A_ON};
int CCIF_B_ON[4] ={CCIF0_B_ON, CCIF1_B_ON, CCIF2_B_ON, CCIF3_B_ON};

SW_BTN    *CCIF_SEL_BTN[4], *LLR_STA_BTN[4], *FEED_SEL_BTN[4];
DAT_BOX   *CCIF_SEL_BOX[4];
FL_OBJECT *LLR_STA_BOX[4], *FEED_SEL_BOX[4];

SW_BTN    *CCIF_PHA_HEX_BTN[4][2],*CCIF_PHA_DEC_BTN[4][3];
FL_OBJECT *CCIF_PHA_HEX_BOX[4][2],*CCIF_PHA_DEC_BOX[4][3];

FL_OBJECT *PWR_CTL_BOX[4], *PWR_STA_BOX[4];

FL_OBJECT *V_BUS_BOX[4], *I_BUS_BOX[4], *P_BUS_BOX[4];
FL_OBJECT *V_RMS_BOX[4], *I_RMS_BOX[4], *Z_RMS_BOX[4];

FL_OBJECT *COLD_T_TMP_BOX[4], *VCS_TMP_BOX[4];
FL_OBJECT *COLD_T_R_BOX[4],   *VCS_R_BOX[4];

//~----------------------------------------------------------------------------

void create_CC_panel(int x0, int y0, int ind) {

  FL_OBJECT *obj;
  int x, y, w, h;

{//~--- Panel ---

  int w1 = 130, h1 = 495, h2 = 25;
  char b[80];

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  sprintf(b, "CC #%d", ind);
  add_frame_box(x, y, w1-10, h2, b, 'L');
  y = y + h2 + 5;
}
  
{//~--- Configuration ---

  int w1 = 20, w2 = 45, w3 = 35, w4 = 20, h1 = 20;

  _b msk_ccif_sel_wri[4] = {
    {0, 0x00000003, 3},
    {1, 0x00000003, 3},
    {2, 0x00000003, 3},
    {3, 0x00000003, 3}};

  _b msk_ccif_sel_rea[4] = {
    {0, 0x000000F0, 16},
    {0, 0x00000F00, 16},
    {0, 0x0000F000, 16},
    {0, 0x000F0000, 16}};

  x = x0 + 5;
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + 5 * h1 + 5;
  add_frame_box(x, y, w-10, h1, "Configuration", 'M');
  y = y + h1;
  
  x = x0 + 5;
  add_frame_box(x, y, w1, h1, "IF", 'N');
  create_btn_hstring(
    1, x, y+h1,   w1, h1, a_b, 'M', &ww_ccif_sel, &msk_ccif_sel_wri[ind], &CCIF_SEL_BTN[ind]);
  create_dat_box_hstring(
    1, x, y+2*h1, w1, h1,      'M', &r_dac_sta,   &msk_ccif_sel_rea[ind], &CCIF_SEL_BOX[ind]);
  x = x + w1;
  add_frame_box(x, y, w2, h1, "LLR", 'N');
  create_bit_btn_hstring(
    1, x, y+h1, w2, h1, l_o, 'M', &wri_llr_sta[ind],  &LLR_STA_BTN[ind]);
  create_data_box_hstring(1, x, y+2*h1, w2, h1, 'M', &LLR_STA_BOX[ind]);
  x = x + w2;
  add_frame_box(x, y, w3, h1, "Feed", 'N');
  create_bit_btn_hstring(
    1, x, y+h1, w3, h1, a_b, 'M', &wri_feed_sel[ind], &FEED_SEL_BTN[ind]);
  create_data_box_hstring(1, x, y+2*h1, w3, h1, 'M', &FEED_SEL_BOX[ind]);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "", 'N');
  obj = add_normal_button(x, y+h1, w4, h1, "W", 'N');
    fl_set_object_callback(obj, write_CC_config_callback, ind);
  obj = add_normal_button(x, y+2*h1, w4, h1, "R", 'N');
    fl_set_object_callback(obj, read_CC_config_callback, ind);
  y = y + 3 * h1 + 5;
}

{//~--- PA Control ---

  int w1 = 35, w2 = 35, w3 = 50, h1 = 20;

  x = x0 + 5;
  w = w1 + w2 + w3;
  PA_SEL_FRAME[ind] =
  obj = add_frame_box(x, y, w, h1, "PA ?", 'N');
  y = y + h1;

  obj = add_normal_button(x, y, w2, h1, "ON", 'N');
    fl_set_object_callback(obj, set_CC_power_on_callback, ind);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "OFF", 'N');
    fl_set_object_callback(obj, set_CC_power_off_callback, ind);
  x = x + w2;
  add_frame_box(x, y, w3, h1, "Status", 'N');
  y = y + h1;
  
  x = x0 + 5;
  w = 2 * w2;
  create_data_box_hstring(1, x, y, w, h1, 'M', &PWR_CTL_BOX[ind]);
  x = x + w;
  create_data_box_hstring(1, x, y, w3, h1, 'M', &PWR_STA_BOX[ind]);
  y = y + h1;

  x = x0 + 5;
  w = w1 + w2 + w3;
  obj = add_normal_button(x, y, w, h1, "READ", 'N');
    fl_set_object_callback(obj, read_CCIF_status_callback, ind);
  y = y + h1 + 5;
}
  
{//~--- Phase ---

  int w1 = 25, w2 = 70, w3 = 25, w4 = 20, w5 = 15, w6 = 30, h1 = 20;
  
  x = x0 + 5;
  obj = add_normal_button(x, y, w1, h1, "@8<-", 'N');
    fl_set_object_callback(obj, dec_CC_phase_callback, ind);
  x = x + w1;
  add_frame_box(x, y, w2, h1, "Phase", 'N');
  x = x + w2;
  obj = add_normal_button(x, y, w3, h1, "@8->", 'N');
    fl_set_object_callback(obj, inc_CC_phase_callback, ind);
  y = y + h1;

  x = x0 + 5;
  create_hex_btn_hstring(
    2, x, y, w4, h1, hex, 'M', &wri_ccif_pha[ind], &CCIF_PHA_HEX_BTN[ind][0]);
  x = x + 2 * w4 + 5;
  create_dec_btn_hstring(
    3, x, y, w5, h1, hex, 'M', &wri_ccif_pha[ind], &CCIF_PHA_DEC_BTN[ind][0]);
  x = x + 3 * w5;
  obj = add_normal_button(x, y, w6, h1, "W", 'N');
    fl_set_object_callback(obj, set_CC_phase_callback, ind);
  y = y + h1;
  
  x = x0 + 5;
  create_data_box_hstring(2, x, y, w4, h1, 'M', CCIF_PHA_HEX_BOX[ind]);
  x = x + 2 * w4 + 5;
  create_data_box_hstring(3, x, y, w5, h1, 'M', CCIF_PHA_DEC_BOX[ind]);
  x = x + 3 * w5;
  obj = add_normal_button(x, y, w6, h1, "R", 'N');
    fl_set_object_callback(obj, read_CCIF_phase_callback, ind);
  y = y + h1 + 5;
}
  
{//~--- Monitor ---

  int w1 = 55, w2 = 65, h1 = 20, h2 = 15;
  char *txt[10] = {
    "V Bus",    "I Bus", "P Bus",    "V RMS", "I RMS", "Z RMS",
    "H-Reject", "Case",  "Cold Tip", "VCS"};
  
  x = x0 + 5;
  w = w1 + w2;
  add_frame_box(x, y, w, h1, "Monitor", 'N');
  y = y + h1;
  
  create_frame_box_vstring(8, x, y, w1, h2, &txt[0], 'S', 'L');
  x = x + w1;

  V_BUS_BOX[ind] = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  I_BUS_BOX[ind] = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  P_BUS_BOX[ind] = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  V_RMS_BOX[ind] = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  I_RMS_BOX[ind] = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  Z_RMS_BOX[ind] = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;

  create_data_box_vstring(2, x, y, w2, h2, 'N', &DTS_TMP_BOX[2*ind]);
  y = y + 2 * h2;

  x = x0 + 5;
  create_frame_box_vstring(2, x, y, w1, 2*h2, &txt[8], 'S', 'L');
  x = x + w1;

  COLD_T_R_BOX[ind]   = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  COLD_T_TMP_BOX[ind] = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  VCS_R_BOX[ind]      = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;
  VCS_TMP_BOX[ind]    = add_data_box(x, y, w2, h2, "", 'N');
  y = y + h2;

  x = x0 + 5;
  obj = add_normal_button(x, y, w, h1, "READ", 'N');
    fl_set_object_callback(obj, read_CC_monitor_callback, ind);
}
}

//~----------------------------------------------------------------------------

void write_CC_config_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _ww *s = &ww_ccif_sel;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  if (!err) {
    _w *e = &ena_dac_wri;
    _w *w = &wri_dac_sta;
    int32 m_a = 0x00000001 << CCIF_A_ON[ind];
    int32 m_b = 0x00000001 << CCIF_B_ON[ind];
    e->inp  = m_a | m_b;
    w->inp &= ~e->inp;
    if (s->inp[ind] == 0) w->inp |= m_a;
    if (s->inp[ind] == 1) w->inp |= m_b;
    write_DAC_callback(NULL, 0);
    if (!err) {
      s->wri[ind] = s->inp[ind];
      s->unk      = FALSE;
    }
    else {
      s->unk = TRUE;
    }
  }

  if (!err) {
    _w *w = &wri_ccif_sta[ind];
    delay(0.020);
    pack32(&w->inp, 0x0000000F, 0);
    pack32(&w->inp, 0x000000C0, 2-wri_llr_sta[ind].inp);
    pack32(&w->inp, 0x00000030, 1+wri_feed_sel[ind].inp);
    write_CCIF_status_callback(NULL, ind);
    if (!err) {
      _w *w = &wri_ccif_sta[ind];
      _w *l = &wri_llr_sta[ind];
      _w *f = &wri_feed_sel[ind];
      delay(0.1);
      pack32(&w->inp, 0x000000F0, 0);
      write_CCIF_status_callback(NULL, ind);
      if (!err) {
        l->wri = l->inp;
        l->unk = FALSE;
        f->wri = f->inp;
        f->unk = FALSE;
      }
      else {
        l->unk = TRUE;
        f->unk = TRUE;
      }
    }
  }

  if (!err) read_CC_config_callback(NULL, ind);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_CC_config_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  if (go_on(err)) read_DAC_callback(NULL, 0);
  if (go_on(err)) read_CCIF_status_callback(NULL, ind);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void set_CC_power_on_callback(FL_OBJECT *obj, long data) {

  int ind = data;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  if (go_on(err)) {
    _w *s = &wri_feed_sel[ind];
    _w *w = &wri_ccif_sta[ind];
    if (s->wri) pack32(&w->inp, 0x0000000F, 0x00000004);
    else        pack32(&w->inp, 0x0000000F, 0x00000001);
    write_CCIF_status_callback(NULL, ind);
    if (go_on(err)) {
      delay(0.1);
      if (s->wri) pack32(&w->inp, 0x0000000F, 0x0000000C);
      else        pack32(&w->inp, 0x0000000F, 0x00000003);
      write_CCIF_status_callback(NULL, ind);
    }
  }

  if (go_on(err)) {
    delay(1.0);
    read_CCIF_status_callback(NULL, ind);
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void set_CC_power_off_callback(FL_OBJECT *obj, long data) {

  int ind = data;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  if (go_on(err)) {
    _w *w = &wri_ccif_sta[ind];
    _w *s = &wri_feed_sel[ind];
    if (s->wri) pack32(&w->inp, 0x0000000F, 0x00000008);
    else        pack32(&w->inp, 0x0000000F, 0x00000002);
    write_CCIF_status_callback(NULL, ind);
    if (go_on(err)) {
      delay(0.1);
      pack32(&w->inp, 0x0000000F, 0x00000000);
      write_CCIF_status_callback(NULL, ind);
    }
  }

  if (go_on(err)) {
    delay(1.0);
    read_CCIF_status_callback(NULL, ind);
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void dec_CC_phase_callback(FL_OBJECT *obj, long data) {

  int ind = data;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  if (go_on(err)) {
    _w *w = &wri_ccif_pha[ind];
    w->inp = w->inp - 1;
    if (w->inp > 255) w->inp = 0;
    write_CCIF_phase_callback(NULL, ind);
  }
  if (go_on(err)) read_CCIF_phase_callback(NULL, ind);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void inc_CC_phase_callback(FL_OBJECT *obj, long data) {

  int ind = data;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  if (go_on(err)) {
    _w *w = &wri_ccif_pha[ind];
    w->inp = MIN(255, w->inp + 1);
    write_CCIF_phase_callback(NULL, ind);
  }
  if (go_on(err)) read_CCIF_phase_callback(NULL, ind);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void set_CC_phase_callback(FL_OBJECT *obj, long data) {

  int ind = data;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  if (go_on(err)) write_CCIF_phase_callback(NULL, ind);
  if (go_on(err)) read_CCIF_phase_callback(NULL, ind);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_CC_monitor_callback(FL_OBJECT *obj, long data) {

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  if (go_on(err)) read_ADC_callback(NULL, 0);
  if (go_on(err)) read_DTS_data_callback(NULL, 0);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_CC_panel(int ind) {

  FL_OBJECT *obj;
  int j, m;
  int32 msk;
  _r *a = &rea_adc;
  _w *w, *s;
  _r *r;
  bool power, check;
  char b[80];
  int V_BUS[4] = {V_BUS_0, V_BUS_1, V_BUS_2, V_BUS_3};
  int I_BUS[4] = {I_BUS_0, I_BUS_1, I_BUS_2, I_BUS_3};
  int P_BUS[4] = {P_BUS_0, P_BUS_1, P_BUS_2, P_BUS_3};
  int V_RMS[4] = {V_RMS_0, V_RMS_1, V_RMS_2, V_RMS_3};
  int I_RMS[4] = {I_RMS_0, I_RMS_1, I_RMS_2, I_RMS_3};
  int Z_RMS[4] = {Z_RMS_0, Z_RMS_1, Z_RMS_2, Z_RMS_3};
  int COLD_T[4] = {CC0_COLD_TIP, CC1_COLD_TIP, CC2_COLD_TIP, CC3_COLD_TIP};
  int VCS[4]    = {CC0_VCS,      CC1_VCS,      CC2_VCS,      CC3_VCS};
  char *txt[4] = {"OFF", "?", "??", "ON"};
  char *cold_t_ID[4] = {"X48012", "X47959", "X47967", "X48007"}; 
  char *vcs_t_ID[4]  = {"X47975", "X47926", "X47934", "X47932"}; 
  struct _cernox *cold_t_ref[4], *vcs_t_ref[4];
  real M0 = adc[COLD_T[ind]].A;
  real M1 = adc[VCS[ind]].A;

{//~--- Panel ---

  fl_set_object_label(PA_SEL_FRAME[ind], wri_feed_sel[ind].wri ? "PA 2" : "PA 1");
}

{//~--- Configuration ---

  int i;
  char *txt[16] = {
    "-", "A", "?", "?",
    "B", "?", "?", "?",
    "?", "?", "?", "?",
    "?", "?", "?", "?"};

  show_btn_string(    4,      NULL, CCIF_SEL_BTN);
  show_dat_box_string(4, txt, NULL, CCIF_SEL_BOX);

  for (i=0; i<4; i++) {
    show_bin_btn_string(1, &wri_llr_sta[i],  &LLR_STA_BTN[i]);
    show_msk_box(0x00000080, &rea_ccif_sta[i], l_o, LLR_STA_BOX[i]);
    show_bin_btn_string(1, &wri_feed_sel[i],  &FEED_SEL_BTN[i]);
    show_msk_box(0x00000040, &rea_ccif_sta[i], b_a, FEED_SEL_BOX[i]);
  }
}

{//~--- PA Power Control ---

  s = &wri_feed_sel[ind];
  w = &wri_ccif_sta[ind];
  r = &rea_ccif_sta[ind];
  msk = s->wri ? 0x0000000C : 0x00000003;
  m = unpack32(*r->rea, msk);
  show_mask_box(msk, r, txt[m], PWR_CTL_BOX[ind]);

  power = (m == 3);
  msk = s->wri ? 0x00000020 : 0x00000010;
  check = unpack32(*r->rea, msk);
  if (!power && !check) sprintf(b, "OFF");
  if (!power &&  check) sprintf(b, "OFF");
  if ( power &&  check) sprintf(b, "O.K.");
  if ( power && !check) sprintf(b, "TRIP");
  obj = PWR_STA_BOX[ind];
  show_mask_box(msk, r, b, obj);
  if (!r->unk && power && !check) {
    fl_set_object_color(obj, FL_RED, 0);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }
}

{//~--- Phase ---

  w = &wri_ccif_pha[ind];
  r = &rea_ccif_pha[ind];
  show_bin_btn_string(2, w, &CCIF_PHA_HEX_BTN[ind][0]);
  show_dec_btn_string(3, w, &CCIF_PHA_DEC_BTN[ind][0]);
  show_hex_box_string(2, r, NULL, &CCIF_PHA_HEX_BOX[ind][0]);
  show_dec_box_string(3, r, NULL, &CCIF_PHA_DEC_BOX[ind][0]);
  if (w->inp > 255) {
    for (j=0; j<2; j++) fl_set_object_color(CCIF_PHA_HEX_BTN[ind][j]->obj, FL_RED, 0);
    for (j=0; j<3; j++) fl_set_object_color(CCIF_PHA_DEC_BTN[ind][j]->obj, FL_RED, 0);
  }
}

{//~--- Monitor ---

  show_float_num_box(a, V_BUS[ind], "%5.1f", V_BUS_BOX[ind]);
  show_float_num_box(a, I_BUS[ind], "%5.3f", I_BUS_BOX[ind]);
  show_float_num_box(a, P_BUS[ind], "%5.1f", P_BUS_BOX[ind]);

  show_float_num_box(a, V_RMS[ind], "%5.1f", V_RMS_BOX[ind]);
  show_float_num_box(a, I_RMS[ind], "%5.3f", I_RMS_BOX[ind]);
  show_float_num_box(a, Z_RMS[ind], "%5.1f", Z_RMS_BOX[ind]);

//~--

  for (j=0; j<sizeof(ref_table)/sizeof(ref_table[0]); j++) {
    if (!strcmp(ref_table[j].nam, cold_t_ID[ind])) cold_t_ref[ind] = &ref_table[j];
    if (!strcmp(ref_table[j].nam, vcs_t_ID[ind]))  vcs_t_ref[ind]  = &ref_table[j];
  }

  show_float_num_box(a, COLD_T[ind], "R=%6.2f", COLD_T_R_BOX[ind]);
  show_float_num_box(a, VCS[ind],    "R=%6.2f", VCS_R_BOX[ind]);

  sprintf(b, "%7.3Lf", 
    cernox_code_to_T(M0/(*(float*)&a->rea[COLD_T[ind]]), M0, cold_t_ref[ind]));
  show_num_box(a, COLD_T[ind], NULL, b, COLD_T_TMP_BOX[ind]);

  sprintf(b, "%7.3Lf", 
    cernox_code_to_T(M1/(*(float*)&a->rea[VCS[ind]]), M1, vcs_t_ref[ind]));
  show_num_box(a, VCS[ind], NULL, b, VCS_TMP_BOX[ind]);
}
}

//~============================================================================

void update_objects(void) {

  int i;
  
  update_template_objects(&msg, &err, NULL, &P);

  update_crate_model_selector_panel();
  
  update_DAC_data();
  update_DTS_data();
  
  update_global_power_panel();
  update_oscillator_panel();

  for (i=0; i<4; i++) update_CC_panel(i);
  
  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
