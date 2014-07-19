// file CAB-STM.c
//
//  CAB-STM Controller
//
//  A.Lebedev Dec-2008...
//

#include "cabdef.h"
#include "crisalib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CAB - STM Controller"};     // global variable
char *date  = {"17-Jan-09"};                // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x18;                                   // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

int32 DO_NOT_CREATE = 0x12345678;                     // global variable

int8 CAB_CMD_ID;                                      // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;

//~-- CAB Model Selector

SW_BTN *MODEL_BTN;
_ww w_model;
_b model = {0, 0x00000001, 2};

//~--- Common things

DAT_BOX *STM[200];
int n_STM = 0;
_r r_ess_sta;
_r r_dig_sta[2];
_r r_stm_cha[2];

//~-- STM Control Panel

SW_BTN  *PS_PWR[2];
DAT_BOX *HTR_GRP[2], *VAC_SEN[2][2], *CPU_TIME_STM[2];
_ww w_ps_pwr[2];
_r r_htr_grp[2];
_r r_vac_sen[2];

//~-- Cold Valve Position Panel

DAT_BOX *CPU_TIME_CV[2], *CV_POS[2][4];
_r r_cv_pos[2];

//~-- CAB Acknoledgement Code

FL_OBJECT *CAB_ACK[2];
_r r_cab_ack;

//~-----

char *d_e[3] = {"DIS", "ENA", "?"};
char *o_o[3] = {"OFF", "ON",  "?"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void get_essential_status_callback(FL_OBJECT* obj, long data);
void get_digital_status_callback(FL_OBJECT* obj, long data);

void create_model_selector_panel(int x0, int y0);

void create_CPU_control_panel(int x0, int y0);
void reset_SW_callback(FL_OBJECT* obj, long data);
void load_PGM_callback(FL_OBJECT* obj, long data);
void go_APSW_callback(FL_OBJECT* obj, long data);

void create_STM_control_panel(int x0, int y0);
void enable_disable_callback(FL_OBJECT* obj, long data);
void acquire_STM_callback(FL_OBJECT* obj, long data);
void read_STM_callback(FL_OBJECT* obj, long data);
void update_STM_control_panel(void);

void create_temperature_panel(int x0, int y0);
void create_other_panel(int x0, int y0);
void update_STM_values(void);

void create_CV_position_panel(int x0, int y0);
void acquire_CV_position_callback(FL_OBJECT* obj, long data);
void read_CV_position_callback(FL_OBJECT* obj, long data);
void update_CV_position_panel(void);

void create_CAB_acknoledgment_panel(FL_FORM *form);
void update_CAB_acknoledgment_panel(int16 *err);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 810, HH = 580;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_model_selector_panel(150, 0);

  create_CPU_control_panel(0, 45);

  create_STM_control_panel(115, 45);
  create_temperature_panel(440, 45);
  create_other_panel(0, 200);
  create_CV_position_panel(0, 430);

  create_CAB_acknoledgment_panel(MAIN);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  invalidate_ww(first, &w_model, 1, 0);

  invalidate_r(first, &r_ess_sta, 100);

  for (i=0; i<2; i++) {
    invalidate_r(first, &r_dig_sta[i], 100);
    invalidate_r(first, &r_stm_cha[i], 100);
    invalidate_ww(first, &w_ps_pwr[i], 1, 0);
    invalidate_r(first, &r_htr_grp[i], 1);
    invalidate_r(first, &r_vac_sen[i], 1);
    invalidate_r(first, &r_cv_pos[i], 5);
  }

  invalidate_r(first, &r_cab_ack, 1);

  read_dat_config_file(dat_config_file_name);
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

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(w_model.inp, 4, 1, file);
    for (i=0; i<2; i++) {
      fread(w_ps_pwr[i].inp, 4, 1, file);
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
    fwrite(w_model.inp, 4, 1, file);
    for (i=0; i<2; i++) {
      fwrite(w_ps_pwr[i].inp, 4, 1, file);
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            GET ESSENTIAL & DIGITAL STATUS CALLBACK FUNCTIONS
//
//~============================================================================

void get_essential_status_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ess_sta;
  int i;
  int8 sta[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<100; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  CAB_get_essential_status(sta, &err);

  if (!err) {
    for (i=0; i<DIM(cab_ess_sta); i++) {
      _sta *k = &cab_ess_sta[i];
      r->rea[i] = packbytes(NULL, &sta[k->i], k->n);
    }
    if (r->unk) for (i=0; i<100; i++) r->was[i] = r->rea[i];
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

//~-------------------------------------------------------------------------------

void get_digital_status_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;
  int i;
  _r *r = &r_dig_sta[cha];
  int8 sta[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<100; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  CAB_get_digital_status(cha, sta, &err);

  if (!err) {
    for (i=0; i<DIM(cab_dig_sta); i++) {
      _sta *k = &cab_dig_sta[i];
      r->rea[i] = packbytes(NULL, &sta[k->i], k->n);
    }
    if (r->unk) for (i=0; i<100; i++) r->was[i] = r->rea[i];
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

//~============================================================================
//
//            "LIBRARY"
//
//~============================================================================

void create_object925(
     int x0, int y0, int w1, int* xx, int* yy, char** t, int n, int* ind) {

  int i, j, x, y;
  int w2 = 50, h1 = 20;
  char *txt[2] = {"N", "R"};
  _b b[] = {
    {0, 0x00000FFF, -1},
    {0, 0x00000FFF, -1},
    {0, 0x00000FFF, -1},
    {0, 0x00000FFF, -1},
    {0, 0x00000FFF, -1},
    {0, 0x00000FFF, -1}};
  
  x = x0;
  y = y0;
  if (strlen(t[0])) {
    add_frame_box(x, y, w1, h1, "", 'N');
    create_frame_box_hstring(n, x+w1, y, w2, h1, t, 'N', 'C');
    y = y + h1;
  }

  for (i=0; i<2; i++) {
    add_frame_box(x, y, w1, h1, txt[i], 'N');
    for (j=0; j<n; j++) {
      b[j].ind = ind[j];
    }
    create_dat_box_hstring(
      n, x+w1, y, w2, h1, 'M', &r_stm_cha[i], b, &STM[n_STM]);
    n_STM = n_STM + n;
    y = y + h1;
  }

  if (xx) *xx = x0 + w1 + n * w2;
  if (yy) *yy = y;
}

//~============================================================================
//
//            MODEL SELECTOR PANEL
//
//~============================================================================

void create_model_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 40, h1 = 15, h2 = 20;
  static char *txt[] = {"EM", "FM"};
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Model", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, txt, 'M', &w_model, &model, &MODEL_BTN);
}

//~============================================================================
//
//            CPU SOFTWARE CONTROL PANEL
//
//~============================================================================

void create_CPU_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 105, h1 = 20, h2 = 25;

  w = 5 + w1 + 5;
  h = 5 + h1 + 5 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1, h1, "CPU", 'M');
  y = y + h1;

  obj = add_normal_button(x, y, w1, h2, "RST to StartUp", 'N');
    fl_set_object_callback(obj, reset_SW_callback, 0);
  y = y + h2;

  obj = add_normal_button(x, y, w1, h2, "RST to Active",    'N');
    fl_set_object_callback(obj, reset_SW_callback, 1);
  y = y + h2;

  obj = add_normal_button(x, y, w1, h2, "LOAD  PGM  1",   'N');
    fl_set_object_callback(obj, load_PGM_callback, 0);
  y = y + h2;

  obj = add_normal_button(x, y, w1, h2, "LOAD  PGM  2",   'N');
    fl_set_object_callback(obj, load_PGM_callback, 1);
  y = y + h2;

  obj = add_normal_button(x, y, w1, h2, "GO  APSW",                 'N');
    fl_set_object_callback(obj, go_APSW_callback, 0);
  y = y + h2;
}

//~----------------------------------------------------------------------------

void reset_SW_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _r *r = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  if (ind == 0) CAB_reset_SW(0x8008, &err);
  if (ind == 1) CAB_reset_SW(0x0220, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void load_PGM_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _r *r = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  if (ind == 0) CAB_load_APSW(0xA2, &err);
  if (ind == 1) CAB_load_APSW(0xA4, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void go_APSW_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CAB_go_APSW(0x00000000, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            STM CONTROL PANEL
//
//~============================================================================

void create_STM_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, x1, y1;

  memset((int8*)STM, 0, sizeof(STM));
  
{//~--- Panel ---

  int w1 = 325, w2 = 315, h1 = 155, h2 = 25;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "STM Control", 'L');
  y = y + h2;
}

{//~--- HT Heater Group & Vacuum Sensor ---

  int w1 = 20, w2 = 35, w3 = 30, w4 = 35, w5 = 20, h1 = 20;
  char *txt1[2] = {"HT Heater Group", "Vacuum Sensor"};
  char *txt2[2] = {"N", "R"};
  
  x = x0 + 5;
  y1 = y;
  for (j=0; j<2; j++) {
    int w = w1 + w2 + w3 + w4 + w5;
    if (j) w = w + w4;
    y = y1;
    add_frame_box(x, y, w, h1, txt1[j], 'N');
    y = y + h1;
    x1 = x;
    for (i=0; i<2; i++) {
      x = x1;
      add_frame_box(x, y, w1, h1, txt2[i], 'N');
      x = x + w1;
      obj = add_normal_button(x, y, w2, h1, "ENA", 'N');
        fl_set_object_callback(obj, enable_disable_callback, 4*j+2*i+1);
      x = x + w2;
      obj = add_normal_button(x, y, w3, h1, "DIS", 'N');
        fl_set_object_callback(obj, enable_disable_callback, 4*j+2*i+0);
      x = x + w3;
      if (j) {
        create_dat_box_hstring(
          1, x, y, w4, h1, 'M', &r_dig_sta[i], &ps_cmd_sw_sta[1], &VAC_SEN[i][0]);
        x = x + w4;
        create_dat_box_hstring(
          1, x, y, w4, h1, 'M', &r_dig_sta[i], &ps_d_sta[1], &VAC_SEN[i][1]);
        x = x + w4;
      }
      else {
        create_dat_box_hstring(
          1, x, y, w4, h1, 'M', &r_dig_sta[i], &ps_d_sta[0], &HTR_GRP[i]);
        x = x + w4;
      }
      obj = add_normal_button(x, y, w5, h1, "R", 'N');
        fl_set_object_callback(obj, get_digital_status_callback, i);
      x = x + w5;
      y = y + h1;
    }
  }
}

{//~--- STM Commands & CPU Time ---

  int w1 = 20, w2 = 35, w3 = 70, w4 = 70, w5 = 120, h1 = 20;
  char *txt[2] = {"N", "R"};
  _b ps_pwr[] = {{0, 0x1, 2}};

  x = x0 + 5;
  add_frame_box(x, y, w1+w2, h1, "PS Pwr",       'N');
  x = x + w1 + w2;
  add_frame_box(x, y, w3+w4, h1, "STM Commands", 'N');
  x = x + w3 + w4;
  add_frame_box(x, y, w5,    h1, "CPU Time",     'N');
  y = y + h1;

  x = x0 + 5;
  x1 = x;
  for (i=0; i<2; i++) {
    x = x1;
    add_frame_box(x, y, w1, h1, txt[i], 'N');
    x = x + w1;
    create_btn_hstring(
      1, x, y, w2, h1, d_e, 'M', &w_ps_pwr[i], ps_pwr, &PS_PWR[i]);
    x = x + w2;
    obj = add_normal_button(x, y, w3, h1, "ACQUIRE", 'N');
      fl_set_object_callback(obj, acquire_STM_callback, i);
    x = x + w3;
    obj = add_normal_button(x, y, w4, h1, "READ", 'N');
      fl_set_object_callback(obj, read_STM_callback, i);
    x = x + w4;
    create_dat_box_hstring(
      1, x, y, w5, h1, 'M', &r_stm_cha[i], stm_time, &CPU_TIME_STM[i]);
    y = y + h1;
  }
}
    
//~---

}

//~----------------------------------------------------------------------------

void enable_disable_callback(FL_OBJECT* obj, long data) {

  int who  = data / 2 / 2;
  int cha  = data / 2 % 2;
  int ctrl = data % 2 % 2;

  _r *r = &r_dig_sta[cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (r->sta != status_KO) r->sta = status_NA;

  if (who) PS_ena_dis_vacuum_sensor(cha, ctrl, &err);
  else     PS_ena_dis_HT_heater_group(cha, ctrl, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void acquire_STM_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;
  _r  *t = &r_stm_cha[cha];
  _ww *w = &w_ps_pwr[cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (t->sta != status_KO) t->sta = status_NA;

  w->wri[0] = w->inp[0];

  STM_acquire(cha, (int8)w->inp[0], &err);

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

void read_STM_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;
  int i;
  _r *r = &r_stm_cha[cha];
  int8 dat[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<100; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  STM_read(cha, dat, &err);

/*
{
static int k = 0;
int i;
for (i=0; i<47; i++) {
dat[4+2*i+0] = 0;
dat[4+2*i+1] = i+1;
//printf("%2d\n", i);
}
k++;
}
*/
  if (!err) {
    for (i=0; i<DIM(stm_cha); i++) {
      _sta *k = &stm_cha[i];
      r->rea[i] = packbytes(NULL, &dat[k->i], k->n);
    }
    if (r->unk) for (i=0; i<100; i++) r->was[i] = r->rea[i];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

{
int i;
for (i=0; i<46; i++) {
//printf("%2d %04X\n", i, r->rea[i]);
}
}
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_STM_control_panel(void) {

  int i;
  char b[80];

  for (i=0; i<2; i++) {
    _r *r = &r_stm_cha[i];
    show_dat_box_string(1, d_e, NULL, &HTR_GRP[i]);
    show_dat_box_string(1, d_e, NULL, &VAC_SEN[i][0]);
    show_dat_box_string(1, o_o, NULL, &VAC_SEN[i][1]);
    show_btn_string(    1,      NULL, &PS_PWR[i]);
    sprintf_huge(b, (int64)(0.0125*r->rea[0]), 8, 0, 1);
    show_dat_box_string(1, NULL, b, &CPU_TIME_STM[i]);
  }
}

//~============================================================================
//
//            TEMPERATURE PANEL
//
//~============================================================================

void create_temperature_panel(int x0, int y0) {

  int i, x, y, yy;

  struct _q {
    int w1, h1, n1;
    char* t1[10];
    int w2, h2, n2;
    char* t2[10];
    int w3, h3, n3;
    char* t3[10];
    int ind[10];
  } q[] = {

  {  0,  0, 0,  {""}, 
   120, 20, 3,  {"CCSC DC/DC CV", "CSP DC/DC CV", "PS DC/DC CV" }, 
    20, 20, 2,  {"Nom.", "Red." , "Nom.", "Red.", "Nom.", "Red."},
                {   1  ,    2   ,   18  ,   19  ,   26  ,   27  }},
  {  0,  0, 0,  {""}, 
   120, 20, 3,  {"28V ISO DC/DC CV", "Battery Charger", "Battery"     }, 
    20, 20, 2,  {"Nom.", "Red."    , "Nom.", "Red."   , "Nom.", "Red."},
                {  12  ,   13      ,   14  ,   15     ,   16  ,   17  }},
  {  0,  0, 0,  {""}, 
   180, 20, 2,  {"Magnet Detector DC/DC CV",  "Current Lead DC/DC CV" }, 
    30, 20, 3,  {"CV1", "CV2", "CV3"       ,  "CV1", "CV2", "CV3"     },
                {  20 ,   21 ,   22        ,    23 ,   24 ,   25      }},
  {360, 20, 1,  {"CCS DC/DC CV"}, 
     0,  0, 3,  {""}, 
    20, 20, 2,  {"CV1", "CV2", "CV3", "CV4", "CV5", "CV6"},
                {   3 ,    4 ,    5 ,    6 ,    7 ,    8 }},
  {  0,  0, 0,  {""}, 
   220, 20, 1,  {"CCS"}, 
    20, 20, 4,  {"Base", "Shunt", "Diode 1", "Diode 2"},
                {    9 ,     10 ,       11 ,       28 }}
  };

{//~--- Panel ---

  int w1 = 370, w2 = 360, h1 = 475, h2 = 40;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "Temperatures", 'L');
  y = y + h2 + 5;
}

{//~--- Universal Tables ---

  for (i=0; i<DIM(q); i++) {
    int j;
    struct _q* p = &q[i];
    x = x0 + 5;
    create_frame_box_hstring(p->n1, x, y, p->w1, p->h1, p->t1, 'N', 'C');
    y = y + p->h1;
    create_frame_box_hstring(p->n2, x, y, p->w2, p->h2, p->t2, 'N', 'C');
    y = y + p->h2;
    for (j=0; j<p->n2; j++) {
      int n = p->n3;
      create_object925(x, y, p->w3, &x, &yy, &p->t3[n*j], n, &p->ind[n*j]);
    }
    y = yy + 5;
  }
}

{//~--- Calibration Parameters ---

  int w1 = 50, w2 = 40, h1 = 20;
  char* txt1[4] = {"", "VgainT", "Voffset", "VgainV"};
  char* txt2[2] = {"N", "R"};
  
  x = x + 10;
  y = yy - 4 * h1;
  create_frame_box_vstring(4, x, y, w1, h1, txt1, 'N', 'C');
  x = x + w1;

  create_frame_box_hstring(2, x, y, w2, h1, txt2, 'N', 'C');
  y = y + h1;
  for (i=0; i<2; i++) {
    create_dat_box_vstring(
      1, x, y,      w2, h1, 'M', &r_stm_cha[i], VgainT,  &STM[n_STM++]);
    create_dat_box_vstring(
      1, x, y+h1,   w2, h1, 'M', &r_stm_cha[i], Voffset, &STM[n_STM++]);
    create_dat_box_vstring(
      1, x, y+2*h1, w2, h1, 'M', &r_stm_cha[i], VgainV,  &STM[n_STM++]);
    x = x + w2;
  }
}

//~--- 

}

//~============================================================================
//
//            "OTHER" (i.e. NON-TEMPERATURE) PANEL
//
//~============================================================================

void create_other_panel(int x0, int y0) {

  int i, x, y, yy;

  struct _q {
    int w1, h1, n1;
    char* t1[10];
    int w2, h2, n2;
    char* t2[10];
    int w3, h3, n3;
    char* t3[10];
    int ind[10];
  } q[] = {

  {240, 20, 1,  {"CSP Battery"}, 
   120, 20, 2,  {"Nominal", "Redundant"}, 
    20, 20, 2,  {"V", "I" , "V", "I"   },
                { 30,  32 ,  31,  33   }},
  {240, 25, 1,  {"Current Lead Voltages"}, 
   120, 20, 2,  {"Inner",        "Outer"}, 
    20, 20, 2,  {"Pos.", "Neg.", "Pos.", "Neg."},
                {  34  ,   35  ,   36  ,   37  }},
  {120, 40, 1,  {"HT Heater\n(DLCM)"}, 
     0,  0, 1,  {""},
    20, 20, 2,  {"V", "I" },
                { 40,  41}},
  {120, 45, 1,  {"Current Lead\nDisconnect\nVoltages"}, 
     0,  0, 1,  {""},
    20, 20, 2,  {"Pos.", "Neg."},
                {  38  ,   39  }},
  { 70, 60, 1,  {"Helium\nPressure\nSensor"}, 
     0,  0, 1,  {""},
    20,  0, 1,  {""},
                {42}},
  { 70, 65, 1,  {"Vacuum\nGauge\nSensor"}, 
     0,  0, 1,  {""},
    20,  0, 1,  {""},
                {43}}
  };

{//~--- Panel ---

  int w1 = 440, h1 = 220;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
}

{//~--- Universal Tables ---

  int x1 = x0 + 5;
  
  for (i=0; i<DIM(q); i++) {
    int j;
    struct _q* p = &q[i];
    x = x1;
    create_frame_box_hstring(p->n1, x, y, p->w1, p->h1, p->t1, 'N', 'C');
    y = y + p->h1;
    create_frame_box_hstring(p->n2, x, y, p->w2, p->h2, p->t2, 'N', 'C');
    y = y + p->h2;
    for (j=0; j<p->n2; j++) {
      int n = p->n3;
      create_object925(x, y, p->w3, &x, &yy, &p->t3[n*j], n, &p->ind[n*j]);
    }
    y = yy + 5;
    if (i == 1 || i == 3) {
      x1 = x;
      y  = y0 + 5;
    }
  }
}

//~--- 

}

//~============================================================================
//
//            UPDATE STM VALUES
//
//~============================================================================

void update_STM_values(void) {

  show_dat_box_string(n_STM, NULL, "%d", &STM[0]);
}

//~============================================================================
//
//            COLD VALVE POSITION PANEL
//
//~============================================================================

void create_CV_position_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, x, y;

{//~--- Panel ---

  int w1 = 440, w2 = 120, h1 = 90, h2 = 40;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w2, h2, "Cold Valve\nPosition", 'L');
  x = x + w2;
}

{//~--- 

  int w1 = 20, w2 = 65, w3 = 45, h1 = 20;
  char* txt[] = {"DV05", "DV06", "DV07", "DV14", ""};
  
  add_frame_box(x, y,    w1, h1, "N", 'N');
  obj = add_normal_button(x+w1, y,    w2, h1, "ACQUIRE", 'N');
    fl_set_object_callback(obj, acquire_CV_position_callback, 0);
  add_frame_box(x, y+h1, w1, h1, "R", 'N');
  obj = add_normal_button(x+w1, y+h1, w2, h1, "ACQUIRE", 'N');
    fl_set_object_callback(obj, acquire_CV_position_callback, 1);
  x = x + w1 + w2;

  create_frame_box_hstring(DIM(txt), x, y, w3, 2*h1, txt, 'N', 'C');
  y = y + 2 * h1;
}

{//~--- 

  int w1 = 20, w2 = 65, w3 = 120, w4 = 45, w5 = 45, h1 = 20;
  
  for (i=0; i<2; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, h1, (i ? "R" : "N"), 'N');
    x = x + w1;
    add_frame_box(x, y, w2, h1, "CPU Time",    'N');
    x = x + w2;
    create_dat_box_hstring(
      1, x, y, w3, h1, 'M', &r_cv_pos[i], &cv_pos[0], &CPU_TIME_CV[i]);
    x = x + w3;
    create_dat_box_hstring(
      4, x, y, w4, h1, 'M', &r_cv_pos[i], &cv_pos[1], &CV_POS[i][0]);
    x = x + 4 * w4;
    obj = add_normal_button(x, y, w5, h1, "READ", 'N');
      fl_set_object_callback(obj, read_CV_position_callback, i);
    y = y + h1;
  }
}

//~--- 

}

//~----------------------------------------------------------------------------

void acquire_CV_position_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;
  _r *r = &r_cv_pos[cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (r->sta != status_KO) r->sta = status_NA;

  STM_acquire_cold_valve_status(cha, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_CV_position_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;
  int i;
  _r *r = &r_cv_pos[cha];
  int8 dat[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<5; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  STM_read_cold_valve_status(cha, dat, &err);

/*
{
static int32 k = 0;
int i;
dat[0] = unpack32(k, 0xFF000000);
dat[1] = unpack32(k, 0x00FF0000);
dat[2] = unpack32(k, 0x0000FF00);
dat[3] = unpack32(k, 0x000000FF);
k = k + 80;
for (i=0; i<5; i++) {
dat[4+2*i+0] = 0;
dat[4+2*i+1] = i+1;
//printf("%2d\n", i);
}
}
*/

  if (!err) {
    for (i=0; i<DIM(cv__pos); i++) {
      _sta *k = &cv__pos[i];
      r->rea[i] = packbytes(NULL, &dat[k->i], k->n);
    }
    if (r->unk) for (i=0; i<5; i++) r->was[i] = r->rea[i];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

{
int i;
for (i=0; i<5; i++) {
//printf("%2d %04X\n", i, r->rea[i]);
}
}
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_CV_position_panel(void) {

  int i;
  char b[80];

  for (i=0; i<2; i++) {
    _r *r = &r_cv_pos[i];
    sprintf_huge(b, (int64)(0.0125*r->rea[0]), 8, 0, 1);
    show_dat_box_string(1, NULL, b, &CPU_TIME_CV[i]);
  }
  show_dat_box_string(8, NULL, "%d", &CV_POS[0][0]);
}

//~============================================================================
//
//            CAB ACKNOLEDGMENT PANEL
//
//~============================================================================

void create_CAB_acknoledgment_panel(FL_FORM *form) {

  FL_OBJECT *obj;
  int x0, y0, x, y, w, h;
  int w1 = 35, w2 = 290, w3, h1 = 20;

  x0 = form->x;
  y0 = form->y + form->h - 30 - 5 - h1 - 5;
  w3 = form->w - 5 - w1 - w2 - 5;

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "CAB:", 'S');
  x = x + w1;
  
  CAB_ACK[0] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w2, h1, "");
    fl_set_object_color(obj, FL_WHITE, FL_MCOL);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
  x = x + w2;

  CAB_ACK[1] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w3, h1, "");
    fl_set_object_color(obj, FL_WHITE, FL_MCOL);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
}

//~----------------------------------------------------------------------------

void update_CAB_acknoledgment_panel(int16 *err) {

  char b[2][80];
  
  sprintf(b[0], " ");
  sprintf(b[1], " ");
  if (msg == 1) {
    sprintf(b[0], "%s... ", cab_cmd_name(CAB_CMD_ID));
    if (*err > 0x2000) sprintf(b[1], "%s", cab_ack_message(*err-0x2000));
    else {
      if (*err) sprintf(b[1], "Command did not reach CAB");
      else      sprintf(b[1], "%s", cab_ack_message(0x00));
    }
  }
  fl_set_object_label(CAB_ACK[0], b[0]);
  fl_set_object_label(CAB_ACK[1], b[1]);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_STM_control_panel();
  update_STM_values();
  update_CV_position_panel();

  update_CAB_acknoledgment_panel(&err);

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
