// file CAB-PTM.c
//
//  CAB-PTM Controller
//
//  A.Lebedev Jan-2009...
//

#include "cabdef.h"
#include "crisalib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CAB - PTM Controller"};     // global variable
char *date  = {"19-Jan-09"};                // global variable

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

//~--- Common Things

DAT_BOX *PTM_TIME[2];
DAT_BOX *PTM_OFFS[2][22], *PTM_DATA[2][22], *PTM_RESI[2][22], *PTM_TEMP[2][22];
DAT_BOX *PTM_VREF[2][4];
_r r_ess_sta;
_r r_ptm_cha[2];

//~-- CAB Acknoledgement Code

FL_OBJECT *CAB_ACK[2];
_r r_cab_ack;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_model_selector_panel(int x0, int y0);

void create_CPU_control_panel(int x0, int y0);
void reset_SW_callback(FL_OBJECT* obj, long data);
void load_PGM_callback(FL_OBJECT* obj, long data);
void go_APSW_callback(FL_OBJECT* obj, long data);

void create_PTM_panel(int x0, int y0);
void calibrate_PTM_callback(FL_OBJECT* obj, long data);
void acquire_PTM_callback(FL_OBJECT* obj, long data);
void read_PTM_callback(FL_OBJECT* obj, long data);
void update_PTM_panel(void);

void create_CAB_acknoledgment_panel(FL_FORM *form);
void update_CAB_acknoledgment_panel(int16 *err);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 775, HH = 775;
  FL_OBJECT *obj;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_model_selector_panel(150, 0);

  create_CPU_control_panel(0, 45);

  create_PTM_panel(115, 45);
  obj = add_normal_button( 5, 520, 50, 55, "FAKE\nREAD\nNOM.", 'N');
    fl_set_object_callback(obj, read_PTM_callback, 2+0);
  obj = add_normal_button(60, 520, 50, 55, "FAKE\nREAD\nRED.", 'N');
    fl_set_object_callback(obj, read_PTM_callback, 2+1);

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
    invalidate_r(first, &r_ptm_cha[i], 3*100);
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

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(w_model.inp, 4, 1, file);
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
    fwrite(w_model.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
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
//            PTM PANEL
//
//~============================================================================

void create_PTM_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, x1, y1, w, h;
  int w1 = 165, w2 = 55, w3 = 75, h1 = 20;
  char* txt[] = {
    "CPU Time",
    "Superfluid Cooling Loop 1",
    "Superfluid Cooling Loop 2",
    "Superfluid Cooling Loop 3",
    "Superfluid Cooling Loop 4",
    "Superfluid Cooling Loop 5",
    "Superfluid Cooling Loop 6",
    "Helium II Bath",
    "Mass Gauging",
    "Porous Plug",
    "Vapour Cooled Shield 1",
    "Vapour Cooled Shield 2",
    "Vapour Cooled Shield 3",
    "Vapour Cooled Shield 4",
    "Cool Down TMP",
    "Current Lead TMP",
    "Magnet Current Lead 1",
    "Magnet Current Lead 2",
    "Magnet Current Lead 3",
    "Cool Down Circuit 1",
    "Cool Down Circuit 2",
    "Cool Down Circuit 3",
    "Persistent Switch",
    "-Vref1 Calibration",
    "+Vref1 Calibration",
    "+Vref2 Calibration",
    "+Vref3 Calibration"};
  int n = DIM(txt) - 1 - 4;
  _b ptm_data[2*n];

//~--

  for (i=0; i<2*n; i++) {
    ptm_data[i].msk = 0xFFFFFFFF;
    ptm_data[i].n   = -1;
  }

//~--

  w = 5 + w1 + 2 * (3 * w2 + w3 + 5) - 5 + 5;
  h = 5 + 2 * h1 + DIM(txt) * h1 + 3 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "Parameters", 'N');
  x = x + w1;
  for (j=0; j<2; j++) {
    add_frame_box(x, y, 3*w2+w3, h1, j?"Redundant":"Nominal", 'N');
    add_frame_box(x, y+h1,   w2, h1, "Offset", 'N');
    x = x + w2;
    add_frame_box(x, y+h1,   w2, h1, "Data",   'N');
    x = x + w2;
    add_frame_box(x, y+h1,   w2, h1, "Rt",     'N');
    x = x + w2;
    add_frame_box(x, y+h1,   w3, h1, "T, K",   'N');
    x = x + w3 + 5;
  }
  y = y1 = y + 2 * h1;
  
  x = x0 + 5;
  create_frame_box_vstring(DIM(txt), x, y, w1, h1, txt, 'N', 'L');
  x = x1 = x + w1;
  
//~--

  for (j=0; j<2; j++) {
    y = y1;
    create_dat_box_vstring(
      1, x, y, 3*w2+w3, h1, 'M', &r_ptm_cha[j], ptm_time, &PTM_TIME[j]);
    y = y + h1;
    for (i=0; i<n; i++) ptm_data[i].ind = i + 1;
    create_dat_box_vstring(
      n, x, y, w2, h1, 'M', &r_ptm_cha[j], ptm_data, &PTM_OFFS[j][0]);
    x = x + w2;
    for (i=0; i<n; i++) ptm_data[i].ind = i + 1 + n + 1;
    create_dat_box_vstring(
      n, x, y, w2, h1, 'M', &r_ptm_cha[j], ptm_data, &PTM_DATA[j][0]);
    x = x + w2;
    for (i=0; i<n; i++) ptm_data[i].ind = i + 1 + 100;
    create_dat_box_vstring(
      n, x, y, w2, h1, 'M', &r_ptm_cha[j], ptm_data, &PTM_RESI[j][0]);
    x = x + w2;
    for (i=0; i<n; i++) ptm_data[i].ind = i + 1 + 200;
    create_dat_box_vstring(
      n, x, y, w3, h1, 'M', &r_ptm_cha[j], ptm_data, &PTM_TEMP[j][0]);
    x = x + w3 + 5;
  }
  y = y1 = y + n * h1;

  x = x1;
  for (j=0; j<2; j++) {
    y = y1;
    create_dat_box_vstring(
      4, x, y, 3*w2+w3, h1, 'M', &r_ptm_cha[j], ptm_Vref, &PTM_VREF[j][0]);
    x = x + 3 * w2 + w3 + 5;
  }
  y = y + 4 * h1;
  
//~--

  x = x0 + 5;
  add_frame_box(x, y, w1, 3*h1, "Commands", 'N');
  x = x + w1;

  for (j=0; j<2; j++) {
    obj = add_normal_button(x, y,      3*w2+w3, h1, "CALIBRATE", 'N');
      fl_set_object_callback(obj, calibrate_PTM_callback, j);
    obj = add_normal_button(x, y+h1,   3*w2+w3, h1, "ACQUIRE", 'N');
      fl_set_object_callback(obj, acquire_PTM_callback, j);
    obj = add_normal_button(x, y+2*h1, 3*w2+w3, h1, "READ", 'N');
      fl_set_object_callback(obj, read_PTM_callback, j);
    x = x + 3 * w2 + w3 + 5;
  }
}

//~----------------------------------------------------------------------------

void calibrate_PTM_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;
  _r  *t = &r_ptm_cha[cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (t->sta != status_KO) t->sta = status_NA;

  PTM_calibration(cha, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void acquire_PTM_callback(FL_OBJECT* obj, long data) {

  int cha = data % 2;
  _r  *t = &r_ptm_cha[cha];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  if (t->sta != status_KO) t->sta = status_NA;

  PTM_start_acq_seq(cha, &err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

struct _cernox Table[] = {
  {"X33101", 124,
 {{  4.000, 3.268648E+03},
  {  4.200, 3.069372E+03},
  {  4.400, 2.894588E+03},
  {  4.600, 2.740023E+03},
  {  4.800, 2.602279E+03},
  {  5.000, 2.478786E+03},
  {  5.200, 2.367456E+03},
  {  5.400, 2.266618E+03},
  {  5.600, 2.174864E+03},
  {  5.800, 2.091023E+03},
  {  6.000, 2.014143E+03},
  {  6.500, 1.847212E+03},
  {  7.000, 1.708955E+03},
  {  7.500, 1.592463E+03},
  {  8.000, 1.492945E+03},
  {  8.500, 1.406808E+03},
  {  9.000, 1.331474E+03},
  {  9.500, 1.264939E+03},
  { 10.000, 1.205704E+03},
  { 10.500, 1.152563E+03},
  { 11.000, 1.104588E+03},
  { 11.500, 1.061016E+03},
  { 12.000, 1.021242E+03},
  { 12.500, 9.847571E+02},
  { 13.000, 9.511513E+02},
  { 13.500, 9.200739E+02},
  { 14.000, 8.912353E+02},
  { 14.500, 8.643856E+02},
  { 15.000, 8.393148E+02},
  { 15.500, 8.158397E+02},
  { 16.000, 7.938041E+02},
  { 16.500, 7.730704E+02},
  { 17.000, 7.535199E+02},
  { 17.500, 7.350470E+02},
  { 18.000, 7.175600E+02},
  { 18.500, 7.009765E+02},
  { 19.000, 6.852239E+02},
  { 19.500, 6.702370E+02},
  { 20.000, 6.559586E+02},
  { 21.000, 6.293215E+02},
  { 22.000, 6.049455E+02},
  { 23.000, 5.825656E+02},
  { 24.000, 5.619373E+02},
  { 25.000, 5.428014E+02},
  { 26.000, 5.249879E+02},
  { 27.000, 5.083764E+02},
  { 28.000, 4.928425E+02},
  { 29.000, 4.782787E+02},
  { 30.000, 4.645953E+02},
  { 31.000, 4.517132E+02},
  { 32.000, 4.395618E+02},
  { 33.000, 4.280794E+02},
  { 34.000, 4.172108E+02},
  { 35.000, 4.069078E+02},
  { 36.000, 3.971265E+02},
  { 37.000, 3.878270E+02},
  { 38.000, 3.789743E+02},
  { 39.000, 3.705365E+02},
  { 40.000, 3.624842E+02},
  { 42.000, 3.474348E+02},
  { 44.000, 3.336419E+02},
  { 46.000, 3.209522E+02},
  { 48.000, 3.092366E+02},
  { 50.000, 2.983848E+02},
  { 52.000, 2.883028E+02},
  { 54.000, 2.789111E+02},
  { 56.000, 2.701390E+02},
  { 58.000, 2.619258E+02},
  { 60.000, 2.542192E+02},
  { 65.000, 2.368762E+02},
  { 70.000, 2.218334E+02},
  { 75.000, 2.086522E+02},
  { 77.350, 2.030024E+02},
  { 80.000, 1.969999E+02},
  { 85.000, 1.866189E+02},
  { 90.000, 1.773071E+02},
  { 95.000, 1.689034E+02},
  {100.000, 1.612777E+02},
  {105.000, 1.543212E+02},
  {110.000, 1.479507E+02},
  {115.000, 1.420961E+02},
  {120.000, 1.366945E+02},
  {125.000, 1.316914E+02},
  {130.000, 1.270447E+02},
  {135.000, 1.227185E+02},
  {140.000, 1.186813E+02},
  {145.000, 1.149053E+02},
  {150.000, 1.113668E+02},
  {155.000, 1.080443E+02},
  {160.000, 1.049194E+02},
  {165.000, 1.019752E+02},
  {170.000, 9.919701E+01},
  {175.000, 9.657154E+01},
  {180.000, 9.408696E+01},
  {185.000, 9.173255E+01},
  {190.000, 8.949876E+01},
  {195.000, 8.737685E+01},
  {200.000, 8.535901E+01},
  {205.000, 8.343805E+01},
  {210.000, 8.160753E+01},
  {215.000, 7.986148E+01},
  {220.000, 7.819452E+01},
  {225.000, 7.660168E+01},
  {230.000, 7.507841E+01},
  {235.000, 7.362051E+01},
  {240.000, 7.222415E+01},
  {245.000, 7.088573E+01},
  {250.000, 6.960198E+01},
  {255.000, 6.836984E+01},
  {260.000, 6.718647E+01},
  {265.000, 6.604922E+01},
  {270.000, 6.495565E+01},
  {273.150, 6.428809E+01},
  {275.000, 6.390346E+01},
  {280.000, 6.289052E+01},
  {285.000, 6.191482E+01},
  {290.000, 6.097449E+01},
  {295.000, 6.006778E+01},
  {300.000, 5.919303E+01},
  {305.000, 5.834870E+01},
  {310.000, 5.753334E+01},
  {315.000, 5.674558E+01},
  {320.000, 5.598412E+01},
  {325.000, 5.524775E+01}}}};

void read_PTM_callback(FL_OBJECT* obj, long data) {

  int  cha  = data % 2;
  bool fake = data / 2;
  int i;
  _r *r = &r_ptm_cha[cha];
  int8 dat[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<300; i++) r->was[i] = r->rea[i];
  err = 0x0000;
  
  if (fake) {
    int offset[23] = {
    0,                                         // CPU Time
    13008, 13008, 13008, 13008, 13008, 13008,  // SF Cooling Loop
    2253,                                      // Helium II Bath
    2261,                                      // Mass Gauging
    2276,                                      // Porous Plug
    13007, 13007, 13007, 13007,                // VCS
    2276,                                      // Cool Down TMP
    2276,                                      // Current Lead TMP
    13006, 13006, 13006,                       // Magnet Current Lead
    13007, 13007, 13007,                       // Cool Down Circuit
    12962};                                    // Persistent Switch
    int data[23] = {
    0,                                         // CPU Time
    13375, 13374, 13376, 13375, 13375, 13375,  // SF Cooling Loop
    9522,                                      // Helium II Bath
    35883,                                     // Mass Gauging
    27526,                                     // Porous Plug
    14097, 13708, 13709, 13710,                // VCS
    27526,                                     // Cool Down TMP
    27526,                                     // Current Lead TMP
    13196, 13195, 13197,                       // Magnet Current Lead
    14097, 14096, 14098,                       // Cool Down Circuit
    55660};                                    // Persistent Switch
    r->rea[46] = 62560;                        // Vref1
    r->rea[47] = 62553;                        // Vref2
    r->rea[48] = 62564;                        // Vref3
    for (i=1; i<23; i++) {
      r->rea[i]    = offset[i];
      r->rea[i+23] = data[i];
    }
    for (i=1; i<49; i++) {
      dat[2*i+0+2] = r->rea[i] >> 8;
      dat[2*i+1+2] = r->rea[i] >> 0;
    }
  }
  else {
    PTM_read(cha, dat, &err);
  }
  
  if (!err) {
    for (i=0; i<DIM(ptm_cha); i++) {
      _sta *k = &ptm_cha[i];
      r->rea[i] = packbytes(NULL, &dat[k->i], k->n);
    }
    for (i=1; i<23; i++) {
      float Rt, t;
printf("%2d %6d  %6d  ", i, r->rea[i], r->rea[i+23]);
      Rt = convert_PTM_data(&ptm_cha[i], r->rea[i+23], (real)r->rea[i], 
                           (real)r->rea[46], (real)r->rea[47], (real)r->rea[48]);
//printf("%f ", Rt);
      r->rea[i+100] = *(int32*)&Rt;
      t = convert_cernox_to_T(Rt, Table);
      r->rea[i+200] = *(int32*)&t;
printf("   %f %f\n", *(float*)&r->rea[i+100], *(float*)&r->rea[i+200]);
    }
    if (r->unk) for (i=0; i<300; i++) r->was[i] = r->rea[i];
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

void update_PTM_panel(void) {

  int i;
  char b[80];

  for (i=0; i<2; i++) {
    _r *r = &r_ptm_cha[i];
    sprintf_huge(b, (int64)(0.0125*r->rea[0]), 8, 0, 1);
    show_dat_box_string( 1, NULL,    b,   &PTM_TIME[i]);
    show_dat_box_string(22, NULL, "%d",   &PTM_OFFS[i][0]);
    show_dat_box_string(22, NULL, "%d",   &PTM_DATA[i][0]);
    show_dat_box_string(22, NULL, "%.0f", &PTM_RESI[i][0]);
    show_dat_box_string(22, NULL, "%.3f", &PTM_TEMP[i][0]);
    show_dat_box_string( 4, NULL, "%d",   &PTM_VREF[i][0]);
  }
}

//~============================================================================
//
//            CAB ACKNOLEDGMENT PANEL
//
//~============================================================================

void create_CAB_acknoledgment_panel(FL_FORM *form) {

  FL_OBJECT *obj;
  int x0, y0, x, y, w, h;
  int w1 = 35, w2, h1 = 20;

  x0 = form->x;
  y0 = form->y + form->h - 30 - 5 - 2 * h1 - 5;
  w2 = form->w - 5 - w1 - 5;

  w = 5 + w1 + w2 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "CAB:", 'S');
  x = x + w1;
  
  CAB_ACK[0] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w2, h1, "");
    fl_set_object_color(obj, FL_WHITE, FL_MCOL);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
  y = y + h1;

  CAB_ACK[1] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w2, h1, "");
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

  update_PTM_panel();

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

