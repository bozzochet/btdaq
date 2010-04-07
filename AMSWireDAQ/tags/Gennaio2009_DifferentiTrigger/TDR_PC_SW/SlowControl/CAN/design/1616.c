// file 1616.c
//
// CC "16 ms" Task Controller
//
// A.Lebedev Jul-2006...

#include "template.h"
#include "ccebdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"\"16 ms\"  Task  Controller"};        // global variable
char *date  = {"26-Oct-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~-- CCEB Crate Model Selector

SW_BTN *CRT_BTN;
_ww ww_crt;

//~--

_w w_adc_use[16];
_w w_adc_cha[16];

_r r_adc_val;

_w w_thr[16];
_w w_pro[16];
_w w_par[16][3][8];

char *dac_nam[11] = {"OH", "OC", "0A", "0B", "1A", "1B", "2A", "2B", "3A", "3B", "-" };
int8  dac_adr[11] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF};

char *reg_nam[9] = {"0S", "1S", "2S", "3S", "0P", "1P", "2P", "3P", "-" };
int8  reg_adr[9] = {0x00, 0x04, 0x08, 0x0C, 0x01, 0x05, 0x09, 0x0D, 0xFF};
int8  reg_dat[9];

int cnt;
__16ms_par par[32];
_r r_par;

//~--

FL_OBJECT *ADC_USE_BTN[16];
SW_BTN *ADC_CHA_DEC_BTN[16][2];
SW_BTN *ADC_CHA_NAM_BTN[16];

FL_OBJECT *ADC_VAL_HEX_BOX[16][4];
FL_OBJECT *ADC_VAL_NUM_BOX[16];

SW_BTN *THR_HEX_BTN[16][4];
SW_BTN *THR_NUM_BTN[16];

SW_BTN *PRO_SEL_BTN[16];
FL_OBJECT *PRO_PAR_GRP[16][3];
SW_BTN *PRO_PAR_BTN[16][3][12];    // yes, 12, not 8 (count on the picture!)

FL_OBJECT *READ_BACK_BRO;

//~--

char *crt_nam[] = {"EM1", "EM2", "QM", "FM"};

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *pro[3] = {"D", "R", "T"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_crate_model_selector_panel(int x0, int y0);

void create_parameter_panel(int x0, int y0);
void use_ADC_callback(FL_OBJECT *obj, long data);
void read_ADC_callback(FL_OBJECT *obj, long data);
void write_parameter_callback(FL_OBJECT *obj, long data);
void run_procedure_callback(FL_OBJECT *obj, long data);
void update_parameter_panel(void);

void create_stop_task_panel(int x0, int y0);
void stop_task_callback(FL_OBJECT *obj, long data);

void create_read_back_panel(int x0, int y0);
void read_parameter_callback(FL_OBJECT *obj, long data);
void update_read_back_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 1120, HH = 520;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_crate_model_selector_panel(200, 0);

  create_parameter_panel(0,  45);
  create_stop_task_panel(0, 440);

  create_read_back_panel(715, 45);

  create_USCM_control_H2_panel(510, 440);

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

  int i, j, k;
  static bool first = TRUE;
  
  invalidate_ww(first, &ww_crt, 1, 0);

  for (i=0; i<16; i++) {
    w_adc_use[i].inp = 0;
    w_adc_use[i].unk = TRUE;
    w_adc_cha[i].inp = 0;
    w_adc_cha[i].unk = TRUE;
  }

  r_adc_val.unk = TRUE;
  r_adc_val.sta = status_NA;
  if (first) r_adc_val.rea = malloc(32*4);
  if (first) r_adc_val.was = malloc(32*4);
  
  for (i=0; i<16; i++) {
    w_thr[i].inp = 0;
    w_thr[i].unk = TRUE;
    w_pro[i].inp = 0;
    w_pro[i].unk = TRUE;
    for (j=0; j<3; j++) {
      for (k=0; k<8; k++) {
        w_par[i][j][k].inp = 0;
        w_par[i][j][k].unk = TRUE;
      }
    }
  }

  r_par.unk = TRUE;
  r_par.sta = status_NA;
  if (first) r_par.rea = malloc(32*4*4);
  if (first) r_par.was = malloc(32*4*4);
  
  read_dat_config_file(dat_config_file_name);
  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i, j, k;
  
  file = fopen(filename, "r");
  if (file) {
    fread(ww_crt.inp, 4, 1, file);
    for (i=0; i<16; i++) {
      fread(&w_adc_use[i].inp, sizeof(w_adc_use[i].inp), 1, file);
      fread(&w_adc_cha[i].inp, sizeof(w_adc_cha[i].inp), 1, file);
      fread(&w_thr[i].inp,     sizeof(w_thr[i].inp),     1, file);
      fread(&w_pro[i].inp,     sizeof(w_pro[i].inp),     1, file);
      for (j=0; j<3; j++) {
        for (k=0; k<8; k++) {
          fread(&w_par[i][j][k].inp, sizeof(w_par[i][j][k].inp), 1, file);
        }
      }
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int i, j, k;
  
  file = fopen(filename, "w");
  if (file) {
    fwrite(ww_crt.inp, 4, 1, file);
    for (i=0; i<16; i++) {
      fwrite(&w_adc_use[i].inp, sizeof(w_adc_use[i].inp), 1, file);
      fwrite(&w_adc_cha[i].inp, sizeof(w_adc_cha[i].inp), 1, file);
      fwrite(&w_thr[i].inp,     sizeof(w_thr[i].inp),     1, file);
      fwrite(&w_pro[i].inp,     sizeof(w_pro[i].inp),     1, file);
      for (j=0; j<3; j++) {
        for (k=0; k<8; k++) {
          fwrite(&w_par[i][j][k].inp, sizeof(w_par[i][j][k].inp), 1, file);
        }
      }
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
//
//            CRATE MODEL SELECTOR PANEL
//
//~============================================================================

_b crt = {0, 0x00000003, 4};
  
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_crate_model_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 35, h1 = 15, h2 = 20;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Crate", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, crt_nam, 'M', &ww_crt, &crt, &CRT_BTN);
}

//~============================================================================
//
//                  PARAMETER PANEL
//
//~============================================================================

void create_parameter_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j, k;
  static char *nam[32];
  int x, y, xx, w, h;
  int w1 = 20, w2 = 15, w3 = 85, w4 = 75, w5 = 20, w6 = 25, w7 = 35;
  int h1 = 25, h2 = 20;

  w = 5 + w1 + 2 * w2 + w3 + 4 * w2 + w4 + 4 * w2 + w4 + 5 + 
      w5 + 8 * (5 + w6) + w7 + 5;
  h = 5 + h1 + h2 + 16 * h2 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~-----

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1+h2, "U\ns\ne", FL_SMALL_SIZE);
  x = x + w1;
  w = 2 * w2 + w3;
  add_frame_box(x, y, w,  h1, "ADC", FL_LARGE_SIZE);
  x = x + w;
  w = 4 * w2 + w4;
  add_frame_box(x, y, w,  h1, "Data", FL_LARGE_SIZE);
  x = x + w;
  w = 4 * w2 + w4;
  add_frame_box(x, y, w,  h1, "Threshold", FL_LARGE_SIZE);
  x = x + w + 5;
  w = w5 + 8 * (5 + w6) + w7;
  add_frame_box(x, y, w,  h1, "Procedure", FL_LARGE_SIZE);
  x = x + w;
  y = y + h1;

  x = x0 + 5 + w1;
  w = 2 * w2;
  add_frame_box(x, y, w,  h2, "Ch", FL_NORMAL_SIZE);
  x = x + w;
  add_frame_box(x, y, w3, h2, "Name", FL_NORMAL_SIZE);
  x = x + w3;
  w = 4 * w2;
  add_frame_box(x, y, w,  h2, "Code", FL_NORMAL_SIZE);
  x = x + w;
  add_frame_box(x, y, w4, h2, "Value", FL_NORMAL_SIZE);
  x = x + w4;
  w = 4 * w2;
  add_frame_box(x, y, w,  h2, "Code", FL_NORMAL_SIZE);
  x = x + w;
  add_frame_box(x, y, w4, h2, "Value", FL_NORMAL_SIZE);
  x = x + w4 + 5;
  add_frame_box(x, y, w5, h2, "P", FL_NORMAL_SIZE);
  x = x + w5;
  w = 8 * (5 + w6);
  add_frame_box(x, y, w,  h2, "DACs or Registers to load", FL_NORMAL_SIZE);
  x = x + w;
  add_frame_box(x, y, w7, h2, "Cmd", FL_NORMAL_SIZE);
  x = x + w7;
  y = y + h2;

//~-----

  for (k=0; k<32; k++) nam[k] = EM1_ADC[k].nam;

  for (i=0; i<16; i++) {
    x = x0 + 5;
    ADC_USE_BTN[i] =
    obj = fl_add_checkbutton(FL_PUSH_BUTTON, x-5, y-5, 5+w1+5, 5+h2+5, "");
      fl_set_object_color(obj, FL_DARKER_COL1, FL_RED);
      fl_set_object_callback(obj, use_ADC_callback, i);
    x = x + w1;
    create_dec_btn_hstring(
      2, x, y, w2, h2, hex, FL_MEDIUM_SIZE, &w_adc_cha[i], &ADC_CHA_DEC_BTN[i][0]);
    x = x + 2 * w2;
    create_num_btn_hstring(
      1, x, y, w3, h2, nam, FL_MEDIUM_SIZE, 32, &w_adc_cha[i], &ADC_CHA_NAM_BTN[i]);
    x = x + w3;
    create_data_box_hstring(4, x, y, w2, h2, FL_MEDIUM_SIZE, &ADC_VAL_HEX_BOX[i][0]);
    x = x + 4 * w2;
    create_data_box_hstring(1, x, y, w4, h2, FL_MEDIUM_SIZE, &ADC_VAL_NUM_BOX[i]);
    x = x + w4;
    create_hex_btn_hstring(
      4, x, y, w2, h2, hex, FL_MEDIUM_SIZE, &w_thr[i], &THR_HEX_BTN[i][0]);
    x = x + 4 * w2;
//////    
    create_num_btn_hstring(
      1, x, y, w4, h2, NULL, FL_MEDIUM_SIZE, 0x0FFF, &w_thr[i], &THR_NUM_BTN[i]);
    x = x + w4 + 5;
//////

//~--

    create_num_btn_hstring(
      1, x, y, w5, h2, pro, FL_MEDIUM_SIZE, 3, &w_pro[i], &PRO_SEL_BTN[i]);
    x = x + w5 + 5;
    xx = x;
    PRO_PAR_GRP[i][0] = fl_bgn_group();
      for (j=0; j<8; j++) {
        create_num_btn_hstring(
          1, x, y, w6, h2, dac_nam, FL_MEDIUM_SIZE, 11, &w_par[i][0][j], &PRO_PAR_BTN[i][0][j]);
        x = x + w6;
        if (j != 7) x = x + 5;
      }
    fl_end_group();
    x = xx;
    PRO_PAR_GRP[i][1] = fl_bgn_group();
      for (j=0, k=0; j<8; j++) {
        if (!(j%2)) {
          create_num_btn_hstring(
            1, x, y, w6, h2, reg_nam, FL_MEDIUM_SIZE, 9, &w_par[i][1][j], &PRO_PAR_BTN[i][1][k]);
          x = x + w6;
        }
        else {
          create_hex_btn_hstring(
            2, x, y, w2, h2, hex, FL_MEDIUM_SIZE, &w_par[i][1][j], &PRO_PAR_BTN[i][1][k]);
          x = x + 2 * w2 + 5;
        }
        k = k + 1 + j % 2;
      }
    fl_end_group();
    x = xx;
    PRO_PAR_GRP[i][2] = fl_bgn_group();
      for (j=0; j<8; j++) {
        create_dec_btn_hstring(
          2, x, y, w2, h2, hex, FL_MEDIUM_SIZE, &w_par[i][2][j], &PRO_PAR_BTN[i][2][j]);
        x = x + 2 * w2;
        w = 7 * w6 + 6 * 5;
        obj = add_frame_box(x, y, w, h2, "", FL_NORMAL_SIZE);
        x = x + w;
        break;
      }
    fl_end_group();
    obj = add_normal_button(x, y, w7, h2, "RUN", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, run_procedure_callback, i);
    y = y + h2;
  }

//~-----

  x = x0 + 5;
  w = w1 + 2 * w2 + w3;
  obj = add_frame_box(x, y, w, h2, "Command:", FL_NORMAL_SIZE);
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  x = x + w;
  w = 4 * w2 + w4;
  obj = add_normal_button(x, y, w, h2, "READ", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_ADC_callback, 0);
  x = x + w;
  w = 4 * w2 + w4 + 5 + w5 + 8 * (5 + w6);
  obj = add_normal_button(x, y, w, h2, "WRITE", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, write_parameter_callback, 0);
  x = x + w;
  add_frame_box(x, y, w7, h2, "Cmd", FL_NORMAL_SIZE);
  x = x + w7;
  y = y + h2;
}

//~----------------------------------------------------------------------------

void use_ADC_callback(FL_OBJECT *obj, long data) {

  int i = data;
  _w *w = &w_adc_use[i];
  
  w->inp = fl_get_button(obj);
  
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_ADC_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[32];
  _r *r = &r_adc_val;

  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<64; i++) r->was[i] = r->rea[i];

  err = 0x0000;

  if (!err) {
    read_ADC(dat, &err);
    if (!err) {
      for (i=0; i<32; i++) {
        struct _ADC *adc;
        float val;
        switch (*ww_crt.inp) {
          case 0:  adc = &EM1_ADC[i]; break;
          case 1:  adc = &EM2_ADC[i]; break;
          case 2:  adc = &QM_ADC[i];  break;
          case 3:  adc = &FM_ADC[i];  break;
          default: adc = &EM1_ADC[i]; break;
        }
adc->A = 1;
adc->B = 0;
        if (adc->A < 0.0) val = 0.0000;
        else              val = adc->A * (float)dat[i] + adc->B;
        r->rea[i]    = dat[i];
        r->rea[i+32] = *(int32*)&val;
      }
      if (r->unk) for (i=0; i<64; i++) r->was[i] = r->rea[i];
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

void write_parameter_callback(FL_OBJECT *obj, long data) {

  int i, j, n;
  __16ms_par par[32];
    
  if (obj) {
    msg = -1;
    update_objects();
  }

  r_par.sta = status_NA;

  n = 0;
  for (i=0; i<16; i++) {
    _w *u = &w_adc_use[i];
    _w *c = &w_adc_cha[i];
    _w *t = &w_thr[i];
    _w *p = &w_pro[i];
    if (u->inp) {
      par[n].cha = c->inp;
      par[n].thr = t->inp;
      par[n].pro = pro[p->inp][0];
      for (j=0; j<8; j++) {
        int8 adr, dat;
        switch (p->inp) {
          case 0:
            adr = w_par[i][0][j].inp;
            par[n].par[j] = (adr < 10) ? dac_adr[adr] : 0xFF;
          break;
          case 1:
            if (!(j%2)) {
              adr = w_par[i][1][j].inp;
              par[n].par[j] = (adr < 8) ? reg_adr[adr] : 0xFF;
            }
            else {
              dat = w_par[i][1][j].inp;
              par[n].par[j] = dat;
            }
          break;
          case 2:
            dat = w_par[i][2][j].inp;
            par[n].par[j] = dat;
          break;
        }
      }
      n = n + 1;
    }
  }
  
  setup_16ms_task(n, par, &err);

  for (i=0; i<16; i++) {
    _w *u = &w_adc_use[i];
    _w *t = &w_thr[i];
    _w *p = &w_pro[i];
    if (u->inp) {
      if (!err) {
        t->wri = t->inp;
        t->unk = FALSE;
        p->wri = p->inp;
        p->unk = FALSE;
        if (p->inp < 3) {
          for (j=0; j<8; j++) {
            _w *w = &w_par[i][p->inp][j];
            w->wri = w->inp;
            w->unk = FALSE;
          }
        }
      }
      else {
        t->unk = TRUE;
        p->unk = TRUE;
        if (p->inp < 3) {
          for (j=0; j<8; j++) {
            _w *w = &w_par[i][p->inp][j];
            w->unk = TRUE;
          }
        }
      }
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void run_procedure_callback(FL_OBJECT *obj, long data) {

  int i = data;
  int j;
  __16ms_par par;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  par.pro = pro[w_pro[i].inp][0];
  for (j=0; j<8; j++) {
    int8 adr, dat;
    switch (w_pro[i].inp) {
      case 0:
        adr = w_par[i][0][j].inp;
        par.par[j] = (adr < 10) ? dac_adr[adr] : 0xFF;
      break;
      case 1:
        if (!(j%2)) {
          adr = w_par[i][1][j].inp;
          par.par[j] = (adr < 8) ? reg_adr[adr] : 0xFF;
        }
        else {
          dat = w_par[i][1][j].inp;
          par.par[j] = dat;
        }
      break;
      case 2:
        dat = w_par[i][2][j].inp;
        par.par[j] = dat;
      break;
    }
  }

  run_16ms_task_procedure(par.pro, par.par, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_parameter_panel(void) {

  int i, j;
  _r *r;
  
  for (i=0; i<16; i++) {
    fl_set_button(ADC_USE_BTN[i], w_adc_use[i].inp);
  }
  
  for (i=0; i<16; i++) {
    SW_BTN *B;
    int k = w_adc_cha[i].inp;
    B = ADC_CHA_DEC_BTN[i][0];
    fl_set_object_label(B->obj, B->l[k/10]);
    if (k > 31) fl_set_object_color(B->obj, FL_RED, FL_WHEAT);
    else        fl_set_object_color(B->obj, FL_WHEAT, FL_RED);
    B = ADC_CHA_DEC_BTN[i][1];
    fl_set_object_label(B->obj, B->l[k%10]);
    if (k > 31) fl_set_object_color(B->obj, FL_RED, FL_WHEAT);
    else        fl_set_object_color(B->obj, FL_WHEAT, FL_RED);
    B = ADC_CHA_NAM_BTN[i];
    k = k & 0x0000001F;
    fl_set_object_label(B->obj, B->l[k]);
    if (k > 31) fl_set_object_color(B->obj, FL_RED, FL_WHEAT);
    else        fl_set_object_color(B->obj, FL_WHEAT, FL_RED);
  }

  r = &r_adc_val;
  for (i=0; i<16; i++) {
    int k = w_adc_cha[i].inp;
    show_hex_box_sstring(4, r, k, hex, &ADC_VAL_HEX_BOX[i][0]);
    show_float_num_box(r, k+32, "%7.3f", ADC_VAL_NUM_BOX[i]);
  }

  for (i=0; i<16; i++) {
    SW_BTN *B;
    _w *w = &w_thr[i];
    _w *p = &w_pro[i];
    int k = w_adc_cha[i].inp;
    struct _ADC *adc;
    char val[80];
    switch (*ww_crt.inp) {
      case 0:  adc = &EM1_ADC[k]; break;
      case 1:  adc = &EM2_ADC[k]; break;
      case 2:  adc = &QM_ADC[k];  break;
      case 3:  adc = &FM_ADC[k];  break;
      default: adc = &EM1_ADC[k]; break;
    }
    show_bin_btn_string(4, w, &THR_HEX_BTN[i][0]);
if (adc->A>0) adc->A = 1;
adc->B = 0;
    if (adc->A < 0.0) sprintf(val, " ");
    else              sprintf(val, "%7.3f", adc->A * (float)w->inp + adc->B);
    B = THR_NUM_BTN[i];
    fl_set_object_label(B->obj, val);
    show_num_btn_string(1, w, &B);
    show_bin_btn_string(1, p, &PRO_SEL_BTN[i]);
    switch (p->inp) {
      case 0:
        for (j=0; j<8; j++) {
          _w *w = &w_par[i][0][j];
          show_bin_btn_string(1, w, &PRO_PAR_BTN[i][0][j]);
        }
      break;
      case 1:
        for (j=0, k=0; j<8; j++) {
          _w *w = &w_par[i][1][j];
          show_bin_btn_string(1+j%2, w, &PRO_PAR_BTN[i][1][k]);
          k = k + 1 + j % 2;
        }
      break;
      case 2:
        for (j=0, k=0; j<8; j++) {
          _w *w = &w_par[i][2][j];
          show_dec_btn_string(2, w, &PRO_PAR_BTN[i][2][k]);
          k = k + 2;
          break;
        }
      break;
    }
  }

  for (i=0; i<16; i++) {
    _w *p = &w_pro[i];
    for (j=0; j<3; j++) {
      if (j == p->inp) {
        fl_activate_object(PRO_PAR_GRP[i][j]);
        fl_show_object(PRO_PAR_GRP[i][j]);
      }
      else {
        fl_deactivate_object(PRO_PAR_GRP[i][j]);
        fl_hide_object(PRO_PAR_GRP[i][j]);
      }
    }
  }
}

//~============================================================================
//
//                  STOP TASK PANEL
//
//~============================================================================

void create_stop_task_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 315;
  int h1 = 40;

  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "STOP  \"16 ms\"  TASK  NOW", FL_HUGE_SIZE);
    fl_set_object_callback(obj, stop_task_callback, 0);
}

//~----------------------------------------------------------------------------

void stop_task_callback(FL_OBJECT *obj, long data) {

  if (obj) {
    msg = -1;
    update_objects();
  }

  r_par.sta = status_NA;

  setup_16ms_task(0, NULL, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//                  READ BACK PANEL
//
//~============================================================================

void create_read_back_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 395;
  int h1 = 25, h2 = 20, h3 = 370;

  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + h3 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Read Back Thresholds & Parameters", FL_LARGE_SIZE);
  y = y + h1;
  obj = add_frame_box(
    x, y, w1, h2, "Ch    Name  Code    Value P Parameters", FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  y = y + h2;
  READ_BACK_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1, h3, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
    fl_set_browser_hscrollbar(obj, FL_OFF);
    fl_set_browser_vscrollbar(obj, FL_ON);
  y = y + h3;
  obj = add_normal_button(x, y, w1, h2, "READ", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_parameter_callback, 0);
  y = y + h2;
}

//~----------------------------------------------------------------------------

void read_parameter_callback(FL_OBJECT *obj, long data) {

  _r *r = &r_par;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  check_16ms_task(&cnt, par, &err);

  if (!err) {
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

void update_read_back_panel(void) {

  FL_OBJECT *obj;
  int i, j;
  _r *r;
  
  r = &r_par;
  fl_freeze_form(MAIN);
  obj = READ_BACK_BRO;
  fl_clear_browser(obj);
  if (!r->unk) {
    for (i=0; i<cnt; i++) {
      __16ms_par *p = &par[i];
      struct _ADC *adc;
      char b[80], *pp = b;
      switch (*ww_crt.inp) {
        case 0:  adc = &EM1_ADC[p->cha]; break;
        case 1:  adc = &EM2_ADC[p->cha]; break;
        case 2:  adc = &QM_ADC[p->cha];  break;
        case 3:  adc = &FM_ADC[p->cha];  break;
        default: adc = &EM1_ADC[p->cha]; break;
      }
if (adc->A>0) adc->A = 1;
adc->B = 0;
      pp += sprintf(pp, "%2d %s %04X ", p->cha, adc->nam, p->thr);
      if (adc->A < 0.0) pp += sprintf(pp, "         ");
      else              pp += sprintf(pp, "%8.3f ", adc->A * (float)p->thr + adc->B);
      if (p->pro  > ' ') pp += sprintf(pp, "%c ", p->pro);
      else               pp += sprintf(pp, "%c ", '?');
      for (j=0; j<8; j++) pp += sprintf(pp, "%02hhX ", p->par[j]);
      fl_add_browser_line(obj, b);
    }
    if (r->sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    else {
      if (r->sta == status_NA) fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      else                     fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
  fl_unfreeze_form(MAIN);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_parameter_panel();
  update_read_back_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
