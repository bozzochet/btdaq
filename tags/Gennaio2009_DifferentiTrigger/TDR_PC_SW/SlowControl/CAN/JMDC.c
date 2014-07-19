// file JMDC.c
//
// Program to control JMDC
//
// A.Lebedev Dec-2007...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"JMDC  Controller"};             // global variable
char *date  = {"21-May-08"};                    // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int P = 0;

char adr_config_file_name[80];                     // global variable
char dat_config_file_name[80];                     // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~-- Ownership Control

SW_BTN *OWNER_BTN[4];
_ww    w_owner;
_b owner[4] = {
  {0, 0x00F0, 16},  // HRDL owner
  {0, 0xF000, 16},  // 1553 owner
  {0, 0x000F, 16},  // DAQ owner
  {0, 0x0F00, 16}}; // MCT owner
char *owners[] = { "0",  "1",  "2",  "3", NULL, NULL,  NULL, NULL,
                  NULL, NULL, NULL, NULL, NULL, NULL, "OFF", "-"};

//~---

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

char *o_o[] = {"OFF", "ON"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_ownership_panel(int x0, int y0);
void write_ownership_callback(FL_OBJECT* obj, long data);
void update_ownership_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 1005, HH = 660;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_ownership_panel(0, 45);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  static bool first = TRUE;

  invalidate_ww(first, &w_owner, 1, 0xFFFF);

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
    fread(w_owner.inp, 4, 1, file);
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
    fwrite(w_owner.inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            OWNERSHIP PANEL
//
//~============================================================================

void create_ownership_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 35, w2 = 35, h1 = 20, h2 = 15, h3 = 15;
  char *txt[] = {"Item", "HRDL", "1553", "DAQ", "MCT"};
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + 5 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1+w2, h1, "Ownership", 'N');
  y = y + h1;

  create_frame_box_vstring(5, x, y, w1, h2, txt, 'S', 'L');
  x = x + w1;

  add_frame_box(x, y, w2, h2, "JMDC", 'S');
  create_btn_hstring(
      4, x, y, w2, h1, owners, 'N', &w_owner, owner, &OWNER_BTN[0]);
}

//~----------------------------------------------------------------------------

void update_ownership_panel(void) {

  show_btn_string(4, NULL, &OWNER_BTN[0]);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_ownership_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
