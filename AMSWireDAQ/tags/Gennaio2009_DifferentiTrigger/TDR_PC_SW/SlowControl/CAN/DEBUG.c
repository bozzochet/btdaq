// file DEBUG.c
//
// DEBUG Controller
//
// A.Lebedev Jul-2006...

#include "template.h"
#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"DEBUG  Controller"};                  // global variable
char *date  = {"13-Dec-08"};                          // global variable

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

//~--

int  cntr;
char text[8192];
_r r_deb;
FL_OBJECT *DEB_BRO;

//~--

bool autorepeat = {FALSE};
_w w_per;
SW_BTN *PER_BTN;

//~--

char *periods[7] = {"1s", "2s", "5s", "10s", "20s", "50s", "100s"};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_debug_panel(int x0, int y0);
void read_debug_callback(FL_OBJECT *obj, long data);
void reset_debug_callback(FL_OBJECT *obj, long data);
void autorepeat_callback(int tid, void *data);
void update_debug_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 520, HH = 615;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_debug_panel(0, 45);

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

  r_deb.unk = TRUE;
  r_deb.sta = status_NA;

  w_per.inp = 0;

  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  
  file = fopen(filename, "r");
  if (file) {
    fread(&w_per.inp, sizeof(w_per.inp), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  
  file = fopen(filename, "w");
  if (file) {
    fwrite(&w_per.inp, sizeof(w_per.inp), 1, file);
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
//                  DEBUG PRINTOUT PANEL
//
//~============================================================================

void create_debug_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 510, w2 = 50, w3 = 40, w4 = 210, w5 = 210;
  int h1 = 25, h2 = 20, h3 = 485;

  w = 5 + w1 + 5;
  h = 5 + h1 + h3 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Debug Printout", FL_LARGE_SIZE);
  y = y + h1;
  DEB_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1, h3, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
    fl_set_browser_hscrollbar(obj, FL_AUTO);
    fl_set_browser_vscrollbar(obj, FL_ON);
  y = y + h3;
  add_frame_box(x, y, w2, h2, "Period", FL_NORMAL_SIZE);
  x = x + w2;
  create_num_btn_hstring(
    1, x, y, w3, h2, periods, FL_MEDIUM_SIZE, 7, &w_per, &PER_BTN);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h2, "READ", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, read_debug_callback, 0);
  x = x + w4;
  obj = add_normal_button(x, y, w5, h2, "RESET", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, reset_debug_callback, 0);
  y = y + h2;
}

//~----------------------------------------------------------------------------

void read_debug_callback(FL_OBJECT *obj, long data) {

  int period = 1000.0 * atof(periods[w_per.inp]);
  _r *r = &r_deb;
  
  if (obj) {
    if (autorepeat) {
      autorepeat = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        autorepeat = TRUE;
        fl_add_timeout(period, autorepeat_callback, 0);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!autorepeat) {
    msg = -1;
    update_objects();
  }

  read_debug_printout(sizeof(text), &cntr, (int8*)text, &err);

  if (!err) {
    text[cntr] = '\0';
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  if (!autorepeat) msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void reset_debug_callback(FL_OBJECT *obj, long data) {

  _r *r = &r_deb;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  reset_debug_printout(&err);

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void autorepeat_callback(int tid, void *data) {

  int period = 1000.0 * atof(periods[w_per.inp]);

  if (!autorepeat) return;
  
  if (P) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback %4d %f\n",
           period, delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  read_debug_callback(NULL, 0);
  fl_add_timeout(period, autorepeat_callback, 0);
}

//~----------------------------------------------------------------------------

void update_debug_panel(void) {

  FL_OBJECT *obj;
  _r *r = &r_deb;
  
  fl_freeze_form(MAIN);

  obj = DEB_BRO;

  fl_clear_browser(obj);
  fl_addto_browser_chars(obj, text);
  if (r->sta == status_KO) {
    fl_set_object_color(obj, FL_BLACK, 0);
    fl_set_object_lcolor(obj, FL_WHITE);
  }
  else {
    if (r->sta == status_NA) fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    else                     fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_object_lcolor(obj, FL_BLACK);
  }

  fl_unfreeze_form(MAIN);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_debug_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
