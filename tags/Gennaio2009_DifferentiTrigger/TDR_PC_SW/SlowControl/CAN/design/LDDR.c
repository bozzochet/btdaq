// file LDDR.c
//
// LDDR Primitive Controller
//
// A.Lebedev May-2005...

#include "template.h"
#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"LDDR Primitive Controller"};          // global variable
char *date  = {"13-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~--

SW_BTN    *BIT_BTN[2][5][8];
FL_OBJECT *BIT_BOX[2][5][8];
_w wri_bit[2][5];
_r rea_bit[2][5];
SW_BTN *PER_BTN;
_w per;
bool autotrigger[5] = {FALSE, FALSE, FALSE, FALSE, FALSE};

SW_BTN    *DAC_BTN[16][4];
FL_OBJECT *DAC_BOX[16][3];
_w ena_dac[16];
_w wri_dac[16];
_r rea_dac[16];


FL_OBJECT *ADC_BOX[32][3];
_r rea_adc[32];

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *o_o[2] = {" ", "@1+"};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

char *periods[8] = {"0.05", "0.1", "0.2", "0.5", "1.0", "2.0", "5.0", "10.0"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_register_panel(int x0, int y0, int ind);
void write_register_callback(FL_OBJECT *obj, long data);
void read_register_callback(FL_OBJECT *obj, long data);
void trigger_callback(FL_OBJECT *obj, long data);
void autotrigger_callback(int tid, void *data);
void update_register_panel(int ind);

void create_dac_panel(int x0, int y0);
void write_dac_callback(FL_OBJECT *obj, long data);
void read_dac_callback(FL_OBJECT *obj, long data);
void update_dac_panel(void);

void create_adc_panel(int x0, int y0);
void read_adc_callback(FL_OBJECT *obj, long data);
void update_adc_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 795, HH = 515;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_register_panel(0,  45, 0);
  create_register_panel(0, 255, 1);
  create_dac_panel(375, 45);
  create_adc_panel(570, 45);
  create_USCM_control_H2_panel(595, 440);

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
  
  for (j=0; j<2; j++) {
    for (i=0; i<5; i++) {
      wri_bit[j][i].inp = 0x0000;
      wri_bit[j][i].unk = TRUE;
      rea_bit[j][i].unk = TRUE;
      rea_bit[j][i].sta = status_NA;
    }
  }

  per.inp = 0;

  for (i=0; i<16; i++) {
    ena_dac[i].inp = 0;
    ena_dac[i].unk = TRUE;
    wri_dac[i].inp = 0x0000;
    wri_dac[i].unk = TRUE;
    rea_dac[i].unk = TRUE;
    rea_dac[i].sta = status_NA;
  }

  for (i=0; i<32; i++) {
    rea_adc[i].unk = TRUE;
    rea_adc[i].sta = status_NA;
  }

  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i, j;
  
  file = fopen(filename, "r");
  if (file) {
    for (j=0; j<2; j++) {
      for (i=0; i<5; i++) {
        fread(&wri_bit[j][i].inp, sizeof(wri_bit[j][i].inp), 1, file);
      }
    }
    fread(&per.inp, sizeof(per.inp), 1, file);
    for (i=0; i<16; i++) {
      fread(&ena_dac[i].inp, sizeof(ena_dac[i].inp), 1, file);
      fread(&wri_dac[i].inp, sizeof(wri_dac[i].inp), 1, file);
    }
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
    for (j=0; j<2; j++) {
      for (i=0; i<5; i++) {
        fwrite(&wri_bit[j][i].inp, sizeof(wri_bit[j][i].inp), 1, file);
      }
    }
    fwrite(&per.inp, sizeof(per.inp), 1, file);
    for (i=0; i<16; i++) {
      fwrite(&ena_dac[i].inp, sizeof(ena_dac[i].inp), 1, file);
      fwrite(&wri_dac[i].inp, sizeof(wri_dac[i].inp), 1, file);
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
//                  REGISTER_PANEL(S)
//
//~============================================================================

void create_register_panel(int x0, int y0, int ind) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 140, w2 = 20, w3 = 45;
  int h1 = 20, h2 = 20;
  char *t1[2] = {"Laser Registers", "Trigger Registers"};
  char *t2[2][9] = {
   {"Pulse duration 500ns",
    "Pulse duration 1000ns",
    "Pulse duration 2000ns",
    "Pulse duration 4000ns",
    "Enable L0",
    "Power L0",
    "Enable L1",
    "Power L1",
    "Command"},
   {"Continuous operation",
    "Enable EXT trigger",
    "EXT trigger port A/B",
    "Fire INT trigger",
    "Bias current L0",
    "Bias current L1",
    "Power L0 trip",
    "Power L1 trip",
    "Command"}};
  
  w = 5 + w1 + 5 * (5 + 2 * w2) + 5;
  h = 5 + h1 + 9 * h2 + 5;
  if (ind) h = h + h2;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, t1[ind], FL_MEDIUM_SIZE);
  x = x + w1 + 5;
  
  for (i=0; i<5; i++) {
    char b[80];
    sprintf(b, "#%d", i);
    add_frame_box(x, y, 2*w2, h1, b, FL_MEDIUM_SIZE);
    x = x + 2 * w2 + 5;
  }
    
  x = x0 + 5;
  y = y + h1;

  create_frame_box_vstring(8, x, y,      w1, h2, &t2[ind][0], FL_NORMAL_SIZE, 'L');
  create_frame_box_vstring(1, x, y+8*h2, w1, h2, &t2[ind][8], FL_NORMAL_SIZE, 'R');

  x = x + w1 + 5;
  for (i=0; i<5; i++) {
    create_bit_btn_vstring(
      -8, x, y, w2, h2, hex, FL_MEDIUM_SIZE, &wri_bit[ind][i], &BIT_BTN[ind][i][0]);
    obj = add_normal_button(x, y+8*h2, w2, h2, "W", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, write_register_callback, 5*ind+i);
    x = x + w2;
    create_data_box_vstring(8, x, y, w2, h2, FL_MEDIUM_SIZE, &BIT_BOX[ind][i][0]);
    rea_bit[ind][i].rea = malloc(4);
    rea_bit[ind][i].was = malloc(4);
    obj = add_normal_button(x, y+8*h2, w2, h2, "R", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, read_register_callback, 5*ind+i);
    x = x + w2 + 5;
  }
  
  if (ind) {
    x = x0 + 5;
    y = y0 + 5 + h1 + 9 * h2;
    add_frame_box(x, y, w1-w3, h1, "Period, sec", FL_MEDIUM_SIZE);
    x = x + w1 - w3;
    create_num_btn_hstring(
      1, x, y, w3, h2, periods, FL_MEDIUM_SIZE, 8, &per, &PER_BTN);
    x = x + w3 + 5;
    for (i=0; i<5; i++) {
      obj = add_normal_button(x, y, 2*w2, h2, "FIRE", FL_NORMAL_SIZE);
        fl_set_object_callback(obj, trigger_callback, i);
      x = x + 2 * w2 + 5;
    }
  }
}

//~----------------------------------------------------------------------------

void write_register_callback(FL_OBJECT *obj, long data) {

  int ind = data / 5;
  int i   = data % 5;
  _w *w = &wri_bit[ind][i];
  _r *r = &rea_bit[ind][i];
  int8 adr[5] = {0x10, 0x12, 0x14, 0x16, 0x18};
  
  r->sta = status_NA;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  write_LVDS_bus(adr[i]+ind, w->inp, &err);
  
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

void read_register_callback(FL_OBJECT *obj, long data) {

  int ind = data / 5;
  int i   = data % 5;
  _r *r = &rea_bit[ind][i];
  int8 adr[5] = {0x10, 0x12, 0x14, 0x16, 0x18};
  int16 dat;
  
  msg = -1;
  update_objects();

  *r->was = *r->rea;
  
  read_LVDS_bus(adr[i]+ind, &dat, &err);
  
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

void trigger_callback(FL_OBJECT *obj, long data) {

  int ind = 1;
  int i   = data % 5;
  int16 error;
  _w *w = &wri_bit[ind][i];
  
  if (obj) {
    if (autotrigger[i]) {
      autotrigger[i] = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        int period = 1000.0 * atof(periods[per.inp]);
        autotrigger[i] = TRUE;
        fl_add_timeout(period, autotrigger_callback, (void*)i);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!autotrigger[i]) {
    msg = -1;
    update_objects();
  }
  
  error = 0;
  w->inp = w->inp & 0xF7;
  write_register_callback(NULL, 5*ind+i);
  error |= err;
  w->inp = w->inp | 0x08;
  write_register_callback(NULL, 5*ind+i);
  error |= err;
  err = error;
  
  if (!autotrigger[i]) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void autotrigger_callback(int tid, void *data) {

  int ind = 1;
  int i = (int)data;
  int period = 1000.0 * atof(periods[per.inp]);

  if (!autotrigger[i]) return;
  
  if (P) {
    static struct timeval now[5], then[5];
    gettimeofday(&now[i], NULL);
    printf("Timeout callback %d %4d %f\n", i, period, delta_t(&now[i], &then[i]));
    memcpy(&then[i], &now[i], sizeof(now[i]));
  }

  trigger_callback(NULL, 5*ind+i);
  fl_add_timeout(period, autotrigger_callback, (void*)i);
}

//~----------------------------------------------------------------------------

void update_register_panel(int ind) {

  int i;
  
  for (i=0; i<5; i++) {
    _w *w = &wri_bit[ind][i];
    _r *r = &rea_bit[ind][i];
    show_bin_btn_string(-8, w, &BIT_BTN[ind][i][0]);
    show_bit_box_string(-8, r, NULL, &BIT_BOX[ind][i][0]);
  }
}

//~============================================================================
//
//                  DAC_PANEL
//
//~============================================================================

void create_dac_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 40, w2 = 20;
  int h1 = 20;
  
  w = 5 + w1 + 4 * w2 + 5 + 3 * w2 + 5;
  h = 5 + 2 * h1 + 16 * h1 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, 2*h1, "DAC", FL_MEDIUM_SIZE);
  x = x + w1;
  add_frame_box(x, y, w2, 2*h1, "", FL_MEDIUM_SIZE);
  x = x + w2;
  add_frame_box(x, y, 3*w2+5+3*w2, h1, "Code", FL_MEDIUM_SIZE);
  y = y + h1;
  add_frame_box(x, y, 3*w2, h1, "Write", FL_MEDIUM_SIZE);
  x = x + 3 * w2 + 5;
  add_frame_box(x, y, 3*w2, h1, "Read", FL_MEDIUM_SIZE);
  y = y + h1;
  
  for (i=0; i<16; i++) {
    char b[80];
    x = x0 + 5;
    sprintf(b, "%d", i);
    add_frame_box(x, y, w1, h1, b, FL_NORMAL_SIZE);
    x = x + w1;
    create_bit_btn_hstring(
      1, x, y, w2, h1, o_o, FL_MEDIUM_SIZE, &ena_dac[i], &DAC_BTN[i][0]);
    x = x + w2;
    create_hex_btn_hstring(
      3, x, y, w2, h1, hex, FL_MEDIUM_SIZE, &wri_dac[i], &DAC_BTN[i][1]);
    x = x + 3 * w2 + 5;
    create_data_box_hstring(3, x, y, w2, h1, FL_MEDIUM_SIZE, &DAC_BOX[i][0]);
    rea_dac[i].rea = malloc(4);
    rea_dac[i].was = malloc(4);
    y = y + h1;
  }

  x = x0 + 5;

  add_frame_box(x, y, w1, h1, "Cmd.", FL_NORMAL_SIZE);
  x = x + w1;
  obj = add_normal_button(x, y, 4*w2, h1, "WRITE", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, write_dac_callback, 0);
  x = x + 4 * w2 + 5;
  obj = add_normal_button(x, y, 3*w2, h1, "READ", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_dac_callback, 0);
}

//~----------------------------------------------------------------------------

void write_dac_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[16];
    
  msg = -1;
  update_objects();

  for (i=0; i<16; i++) {
    _w *e = &ena_dac[i];
    _w *w = &wri_dac[i];
    _r *r = &rea_dac[i];
    dat[i] = w->inp & 0x0FFF;
    if (!e->inp) dat[i] |= 0x8000;
    r->sta = status_NA;
  }
  
  write_DAC(dat, &err);

  for (i=0; i<16; i++) {
    _w *e = &ena_dac[i];
    _w *w = &wri_dac[i];
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
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_dac_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[16];
    
  msg = -1;
  update_objects();

  for (i=0; i<16; i++) {
    _r *r = &rea_dac[i];
    *r->was = *r->rea;
  }

  read_DAC(dat, &err);

  for (i=0; i<16; i++) {
    _r *r = &rea_dac[i];
    if (!err) {
      *r->rea = dat[i];
      r->unk = FALSE;
      r->sta = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_dac_panel(void) {

  int i;
  
  for (i=0; i<16; i++) {
    _w *e = &ena_dac[i];
    _w *w = &wri_dac[i];
    _r *r = &rea_dac[i];
    show_bin_btn_string(1, e, &DAC_BTN[i][0]);
    show_bin_btn_string(3, w, &DAC_BTN[i][1]);
    show_hex_box_string(3, r, hex, &DAC_BOX[i][0]);
  }
}

//~============================================================================
//
//                  ADC_PANEL
//
//~============================================================================

void create_adc_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h;
  int w1 = 45, w2 = 20;
  int h1 = 20;
  
  w = 5 + w1 + 3 * w2 + 5 + w1 + 3 * w2 + 5;
  h = 5 + 2 * h1 + 16 * h1 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  for (j=0; j<2; j++) {
    x = x0 + 5 + j *(w1 + 3 * w2 + 5);;
    y = y0 + 5;
    add_frame_box(x, y, w1, 2*h1, "ADC", FL_MEDIUM_SIZE);
    x = x + w1;
    add_frame_box(x, y, 3*w2, 2*h1, "Code", FL_MEDIUM_SIZE);
    x = x0 + 5 + j *(w1 + 3 * w2 + 5);;
    y = y + 2 * h1;
    for (i=0; i<16; i++) {
      char b[80];
      x = x0 + 5 + j *(w1 + 3 * w2 + 5);;
      sprintf(b, "%d", 16*j+i);
      add_frame_box(x, y, w1, h1, b, FL_NORMAL_SIZE);
      x = x + w1;
      create_data_box_hstring(3, x, y, w2, h1, FL_MEDIUM_SIZE, &ADC_BOX[16*j+i][0]);
      rea_adc[16*j+i].rea = malloc(4);
      rea_adc[16*j+i].was = malloc(4);
      y = y + h1;
    }
  }

  x = x0 + 5;

  add_frame_box(x, y, w1, h1, "Cmd.", FL_NORMAL_SIZE);
  x = x + w1;
  obj = add_normal_button(x, y, 3*w2+5+w1+3*w2, h1, "READ", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_adc_callback, 0);
}

//~----------------------------------------------------------------------------

void read_adc_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[32];
    
  msg = -1;
  update_objects();

  for (i=0; i<32; i++) {
    _r *r = &rea_adc[i];
    *r->was = *r->rea;
  }

  read_ADC(dat, &err);

  for (i=0; i<32; i++) {
    _r *r = &rea_adc[i];
    if (!err) {
      *r->rea = dat[i];
      r->unk = FALSE;
      r->sta = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_adc_panel(void) {

  int i;
  
  for (i=0; i<32; i++) {
    _r *r = &rea_adc[i];
    show_hex_box_string(3, r, hex, &ADC_BOX[i][0]);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_register_panel(0);
  update_register_panel(1);
  update_dac_panel();
  update_adc_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
