// file TEMPERATURE.c
//
// DS Temperature Calculator - A.Lebedev Aug-2006...
//

#include "template.h"

//-----------------------------------------------------------------------------

char *title = {"DS Temperature Calculator"};         // global variable
char *date  = {"13-Aug-06"};                         // global variable

_w w_msb;
_w w_lsb;

SW_BTN *MSB[8], *LSB[8];

_r r_long;
_r r_short;
FL_OBJECT *LONG[16], *SHORT[8];
FL_OBJECT *LONG_TEMP[1], *SHORT_TEMP[1];

FL_FORM *MAIN = NULL;

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void read_dat_config_file(void);
void write_dat_config_file(void);
void switch_btn_callback(int32 data);

void create_scratch_pad_panel(int x0, int y0);
void show_scratch_pad_panel(void);

void create_long_temp_panel(int x0, int y0);
void show_long_temp_panel(void);

void create_short_temp_panel(int x0, int y0);
void show_short_temp_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 330, HH = 275;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_scratch_pad_panel(0,  45);
  create_long_temp_panel(  0, 100);
  create_short_temp_panel(80, 190);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void refresh_objects(void) {

  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  static bool first = TRUE;
  
  w_msb.inp = 0;
  w_msb.unk = TRUE;
  w_lsb.inp = 0;
  w_lsb.unk = TRUE;

  r_long.unk  = TRUE;
  r_long.sta  = status_NA;
  r_short.unk = TRUE;
  r_short.sta = status_NA;
  if (first) r_long.rea  = malloc(32*4);
  if (first) r_long.was  = malloc(32*4);
  if (first) r_short.rea = malloc(32*4);
  if (first) r_short.was = malloc(32*4);
  
  read_dat_config_file();
  first = FALSE;
}

//~----------------------------------------------------------------------------

void read_dat_config_file(void) {

  FILE *file;
  
  file = fopen(dat_config_file_name, "r");
  if (file) {
    fread(&w_msb.inp, sizeof(w_msb.inp), 1, file);
    fread(&w_lsb.inp, sizeof(w_lsb.inp), 1, file);
    fclose(file);
  }
}

//~----------------------------------------------------------------------------

void write_dat_config_file(void) {

  FILE *file;
  
  file = fopen(dat_config_file_name, "w");
  if (file) {
    fwrite(&w_msb.inp, sizeof(w_msb.inp), 1, file);
    fwrite(&w_lsb.inp, sizeof(w_lsb.inp), 1, file);
    fclose(file);
  }
}

//~----------------------------------------------------------------------------

void switch_btn_callback(int32 data) {

  update_objects();
}

//~============================================================================
//
//                  SCRATCH PAD PANEL
//
//~============================================================================

void create_scratch_pad_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 20, h1 = 20;

  w = 5 + 16 * w1 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  w = 8 * w1;
  add_frame_box(x, y, w, h1, "MSB", FL_SMALL_SIZE);
  x = x + w;
  add_frame_box(x, y, w, h1, "LSB", FL_SMALL_SIZE);

  x = x0 + 5;
  y = y + h1;
  create_bit_btn_hstring(
    8, x, y, w1, h1, hex, FL_MEDIUM_SIZE, &w_msb, &MSB[0]);
  x = x + 8 * w1;
  create_bit_btn_hstring(
    8, x, y, w1, h1, hex, FL_MEDIUM_SIZE, &w_lsb, &LSB[0]);
}

//~----------------------------------------------------------------------------

void show_scratch_pad_panel(void) {
}

//~============================================================================
//
//                  LONG TEMPERATURE PANEL
//
//~============================================================================

void create_long_temp_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 20, h1 = 20, h2 = 35;

  w = 5 + 16 * w1 + 5;
  h = 5 + 2 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  w = 16 * w1;
  add_frame_box(x, y, w, h1, "Temperature Long", FL_SMALL_SIZE);

  x = x0 + 5;
  y = y + h1;
  create_data_box_hstring(16, x, y, w1, h1, FL_MEDIUM_SIZE, &LONG[0]);

  x = x0 + 5;
  y = y + h1;
  w = 16 * w1;
  create_data_box_hstring(1, x, y, w, h2, FL_HUGE_SIZE, &LONG_TEMP[0]);
}

//~----------------------------------------------------------------------------

void show_long_temp_panel(void) {
}

//~============================================================================
//
//                  SHORT TEMPERATURE PANEL
//
//~============================================================================

void create_short_temp_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 20, h1 = 20, h2 = 35;

  w = 5 + 8 * w1 + 5;
  h = 5 + 2 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  w = 8 * w1;
  add_frame_box(x, y, w, h1, "Temperature Short", FL_SMALL_SIZE);

  x = x0 + 5;
  y = y + h1;
  create_data_box_hstring(8, x, y, w1, h1, FL_MEDIUM_SIZE, &SHORT[0]);

  x = x0 + 5;
  y = y + h1;
  w = 8 * w1;
  create_data_box_hstring(1, x, y, w, h2, FL_HUGE_SIZE, &SHORT_TEMP[0]);
}

//~----------------------------------------------------------------------------

void show_short_temp_panel(void) {
}

//~============================================================================

void update_objects(void) {

  show_scratch_pad_panel();
  show_long_temp_panel();
  show_short_temp_panel();

  write_dat_config_file();

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
