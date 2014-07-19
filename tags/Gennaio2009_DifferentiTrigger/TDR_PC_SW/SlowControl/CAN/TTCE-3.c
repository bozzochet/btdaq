// file TTCE-3.c
//
// TTCE-3 Controller (Loop Control)
//
// A.Lebedev Mar-2008...

#include "ttcedef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"TTCE-3  Controller"};                 // global variable
char *date  = {"12-Mar-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x167;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~-- "all" "TTCE Loop Control" Registers

int n_ctrl = 32;
_ww w_ctrl[2][32];
_r  r_ctrl[2];

//~-- Loop Control

SW_BTN  *CTRL_BTN[2][32][8];
DAT_BOX *CTRL_BOX[2][32][8];

//~--

char *hex[] = {
  "0", "1", "2", "3", "4", "5", "6", "7", 
  "8", "9", "A", "B", "C", "D", "E", "F"};
char *num[] = {
  "00", "01", "02", "03", "04", "05", "06", "07", 
  "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
  "10", "11", "12", "13", "14", "15", "16", "17", 
  "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
  "20", "21", "22", "23", "24", "25", "26", "27", 
  "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
  "30", "31", "32", "33", "34", "35", "36", "37", 
  "38", "39", "3A", "3B", "3C", "3D", "3E", "3F"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_loop_control_panel(int x0, int y0, int ind);
void write_one_loop_control_callback(FL_OBJECT *obj, long data);
void write_all_loop_control_callback(FL_OBJECT *obj, long data);
void read_all_loop_control_callback(FL_OBJECT *obj, long data);
void update_one_loop_control_callback(FL_OBJECT *obj, long data);
void update_loop_control_panel(int ind);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 745, HH = 635;
  int i;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  for (i=0; i<2; i++) {
    int s = 375 * i;
    create_loop_control_panel(0+s, 45, i);
  }
  
  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i, j;
  static bool first = TRUE;

  for (i=0; i<2; i++) {
    for (j=0; j<n_ctrl; j++) {
      invalidate_ww(first, &w_ctrl[i][j], 1, 0);
    }
    invalidate_r(first,  &r_ctrl[i], n_ctrl);
  }

  read_dat_config_file(dat_config_file_name);

  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i, j;
  
  file = fopen(filename, "r");
  if (file) {
    for (i=0; i<2; i++) {
      for (j=0; j<n_ctrl; j++) {
        fread(w_ctrl[i][j].inp, 4, 1, file);
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
  int i, j;

  file = fopen(filename, "w");
  if (file) {
    for (i=0; i<2; i++) {
      for (j=0; j<n_ctrl; j++) {
        fwrite(w_ctrl[i][j].inp, 4, 1, file);
      }
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
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

//~============================================================================
//
//                  LOOP CONTROL PANEL
//
//~============================================================================

_b bit_msk[32][8];
  
void create_loop_control_panel(int x0, int y0, int ind) {

  int i, j;
  FL_OBJECT *obj;
  int x, y, x1, w, h;
  int w1 = 20, w2 = 75, w3 = 15, w4 = 20;
  int h1 = 30, h2 = 15, h3 = 20;
  char *txt1[] = {"PRIMARY LOOP CONTROL", "SECONDARY LOOP CONTROL"};
  char *txt2[] = {
    "Control",     "Set_point",   "k1",          "k2", 
    "k3",          "iband",       "Feed_forw",   "Feed_forw",
    "Test_T",      "Test_T",      "ph_term",     "ph_term", 
    "ih_term",     "ih_term",     "pi_dcv",      "cav_magine",
    "LLW_Loop",    "LLR_Loop",    "PR1_Loop",    "PR2_Loop",
    "COR_Loop",    "SUP_Loop",    "alarm_ena",   "alarm_now",
    "alarm_was",   "cycle_cnt",   "Not defined", "Not defined",
    "Not defined", "Not defined", "Not defined", "Not defined"};

//~--

  w = 5 + w1 + w2 + 8 * w3 + w4 + 5 + 8 * w3 + 5;
  h = 5 + h1 + 32 * h2 + 2 * h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, txt1[ind], 'L');
  y = y + h1;

//~--

  for (i=0; i<n_ctrl; i++) {
    for (j=0; j<8; j++) {
      _b *b = &bit_msk[i][j];
      b->ind = i;
      b->msk = 1 << (7 - j);
      b->n   = 2;
    }
  }
  
//~--

  create_frame_box_vstring(n_ctrl, x, y, w1, h2, &num[n_ctrl*ind], 'S', 'C');
  x = x + w1;
  create_frame_box_vstring(n_ctrl, x, y, w2, h2, &txt2[0],         'S', 'L');
  x = x + w2;

  x1 = x;
  for (i=0; i<n_ctrl; i++) {
    x = x1;
    create_btn_hstring(
      8, x, y, w3, h2, hex, 'N', &w_ctrl[ind][i], &bit_msk[0][0], &CTRL_BTN[ind][i][0]);
    x = x + 8 * w3;
    obj = add_normal_button(x, y, w4, h2, "W", 'N');
      fl_set_object_callback(obj, write_one_loop_control_callback, n_ctrl*ind+i);
    x = x + w4 + 5;
    create_dat_box_hstring(
      8, x, y, w3, h2,      'N', &r_ctrl[ind],    &bit_msk[i][0], &CTRL_BOX[ind][i][0]);
    y = y + h2;
  }
  
//~--

  x = x0 + 5;
  add_frame_box(x, y, w1+w2, 2*h3, "", 'S');
  x = x + w1 + w2;
  obj = add_normal_button(x, y, 8*w3+w4, h3, "WRITE  ALL",  'N');
    fl_set_object_callback(obj, write_all_loop_control_callback, ind);
  obj = add_normal_button(x, y+h3, 16*w3+w4+5, h3, "UPDATE  INPUT",  'N');
    fl_set_object_callback(obj, update_one_loop_control_callback, ind);
  x = x + 8 * w3 + w4 + 5;
  obj = add_normal_button(x, y, 8*w3, h3, "READ  ALL",  'N');
    fl_set_object_callback(obj, read_all_loop_control_callback, ind);
}

//~----------------------------------------------------------------------------

void write_one_loop_control_callback(FL_OBJECT *obj, long data) {

  int ind = data / n_ctrl;
  int i   = data % n_ctrl;
  int8 adr[1], dat[1];
  _ww *w = &w_ctrl[ind][i];
  _r *r  = &r_ctrl[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  adr[0] = n_ctrl * ind + i;
  dat[0] = w->inp[0];
  
  write_TTCE_loop_ctrl(1, adr, dat, &err);

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

void write_all_loop_control_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  int8 adr[n_ctrl], dat[n_ctrl];
  _r *r  = &r_ctrl[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  for (i=0; i<n_ctrl; i++) {
    _ww *w = &w_ctrl[ind][i];
    adr[i] = n_ctrl * ind + i;
    dat[i] = w->inp[0];
  }
  
  write_TTCE_loop_ctrl(n_ctrl, adr, dat, &err);

  for (i=0; i<n_ctrl; i++) {
    _ww *w = &w_ctrl[ind][i];
    if (!err) {
      w->wri[0] = w->inp[0];
      w->unk = FALSE;
    }
    else {
      w->unk = TRUE;
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void read_all_loop_control_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  int8 rea[64];
  _r *r = &r_ctrl[ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<n_ctrl; i++) r->was[i] = r->rea[i];
  
  read_TTCE_loop_ctrl(rea, &err);
  
  if (!err) {
    for (i=0; i<n_ctrl; i++) {
      r->rea[i] = rea[n_ctrl*ind+i];
      if (r->unk) r->was[i] = r->rea[i];
    }
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

void update_one_loop_control_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  _r  *r = &r_ctrl[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  if (!err) read_all_loop_control_callback(NULL, ind);
  if (!err) {
    for (i=0; i<n_ctrl; i++) {
      _ww *w = &w_ctrl[ind][i];
      w->inp[0] = r->rea[i];
      write_all_loop_control_callback(NULL, ind);
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }

}

//~----------------------------------------------------------------------------

void update_loop_control_panel(int ind) {

  int i;
  
  for (i=0; i<n_ctrl; i++) {
    show_btn_string(    8,      NULL, &CTRL_BTN[ind][i][0]);
    show_dat_box_string(8, hex, NULL, &CTRL_BOX[ind][i][0]);
  }
}

//~============================================================================

void update_objects(void) {

  int i;

  update_template_objects(&msg, &err, NULL, &P);

  for (i=0; i<2; i++) {
    update_loop_control_panel(i);
  }

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
