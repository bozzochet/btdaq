// file TTCE-4.c
//
// TTCE-4 Controller
//
// A.Lebedev Mar-2008...

#include "ttcedef.h"

// Lucio asked to split TTCE-4 program in two: one for P and one for S loop.
//
// see TTCE-4P.c and TTCE-4S.c files
//

#ifndef ifrom
#define ifrom  0
#endif
#ifndef ito
#define ito    2
#endif

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if ifrom == 0 && ito == 2
char *title = {"TTCE-4  Controller"};                 // global variable
#endif
#if ifrom == 0 && ito == 1
char *title = {"TTCE-4P"};                            // global variable
#endif
#if ifrom == 1 && ito == 2
char *title = {"TTCE-4S"};                            // global variable
#endif
char *date  = {"11-Jun-08"};                          // global variable

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
_ww w_ctrl[2];
_r  r_ctrl[2];

//~-- "Control"

SW_BTN  *CTRL_BTN[2][6];
DAT_BOX *CTRL_BOX[2][6];

//~-- "Settings"

SW_BTN  *SET_BTN[2][24];
DAT_BOX *SET_BOX[2][24];

//~-- "Parameters"

DAT_BOX *PAR_BOX[2][4];

//~-- "Alarms"

SW_BTN  *ALA_ENA_BTN[2][8];
DAT_BOX *ALA_ENA_BOX[2][8];
DAT_BOX *ALA_NOW_BOX[2][8];
DAT_BOX *ALA_WAS_BOX[2][8];


//~-- "Loops"

SW_BTN  *LOOP_ENA_BTN[2][6];
SW_BTN  *LOOP_SET_BTN[2][6];
DAT_BOX *LOOP_ENA_BOX[2][6];
DAT_BOX *LOOP_SET_BOX[2][6];
DAT_BOX *LOOP_OUT_BOX[2][6];

//~--

char *hex[] = {
  "0", "1", "2", "3", "4", "5", "6", "7", 
  "8", "9", "A", "B", "C", "D", "E", "F"};
char *d_e[] = {"D", "E"};
char *__e[] = {" ", "E"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_stupied_panel(int x0, int y0, int ind);

void create_control_panel(int x0, int y0, int ind);
void update_control_panel(int ind);

void create_setting_panel(int x0, int y0, int ind);
void convert_Set_point_code_to_value(int32 code, int32 *value);
void convert_k2_code_to_value(int32 code, int32 *value);
void convert_iband_code_to_value(int32 code, int32 *value);
void convert_Feed_forw_code_to_value(int32 code, int32 *value);
void convert_Feed_forw_code_to_value_1(int32 code, int32 *value);
void convert_Feed_forw_code_to_value_2(int32 code, int32 *value);
void convert_Test_T_code_to_value(int32 code, int32 *value);
void convert_cav_magine_code_to_value(int32 code, int32 *value);
void update_setting_panel(int ind);

void create_parameter_panel(int x0, int y0, int ind);
void convert_ph_ih_term_code_to_value(int32 code, int32 *value);
void update_parameter_panel(int ind);

void create_alarm_panel(int x0, int y0, int ind);
void update_alarm_panel(int ind);

void create_loop_panel(int x0, int y0, int ind);
void convert_set_point_A_code_to_value(int32 code, int32 *value);
void convert_set_point_B_code_to_value(int32 code, int32 *value);
void update_loop_panel(int ind);

void create_button_panel(int x0, int y0, int ind);
void write_all_loop_controls_callback(FL_OBJECT *obj, long data);
void read_all_loop_controls_callback(FL_OBJECT *obj, long data);
void update_all_loop_controls_callback(FL_OBJECT *obj, long data);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

#if ifrom == 0 && ito == 2
  int WW = 625, HH = 585;
#endif
#if ifrom == 0 && ito == 1
  int WW = 310, HH = 585;
#endif
#if ifrom == 1 && ito == 2
  int WW = 310, HH = 585;
#endif
  int i;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  for (i=ifrom; i<ito; i++) {
    int s = 315 * (i - ifrom);
    create_stupied_panel(   0+s,  45, i);
    create_control_panel(  80+s,  45, i);
    create_setting_panel(   0+s, 130, i);
    create_button_panel(    0+s, 510, i);
    create_parameter_panel( 0+s, 410, i);
    create_alarm_panel(   160+s, 130, i);
    create_loop_panel(    160+s, 320, i);
  }

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  for (i=ifrom; i<ito; i++) {
    invalidate_ww(first, &w_ctrl[i], n_ctrl, 0);
    invalidate_r(first,  &r_ctrl[i], n_ctrl);
  }

  read_dat_config_file(dat_config_file_name);

  first = FALSE;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  int i;
  FILE *file;
  
  file = fopen(filename, "r");
  if (file) {
    for (i=ifrom; i<ito; i++) {
      fread(w_ctrl[i].inp, 4, n_ctrl, file);
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  int i;
  FILE *file;

  file = fopen(filename, "w");
  if (file) {
    for (i=ifrom; i<ito; i++) {
      fwrite(w_ctrl[i].inp, 4, n_ctrl, file);
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
//                  "STUPIED" PANEL
//
//~============================================================================

void create_stupied_panel(int x0, int y0, int ind) {

  int x, y, w, h;
  int w1 = 70, h1 = 75;
  char *txt[] = {"P", "S"};

  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, txt[ind], 'H');
}

//~============================================================================
//
//                  "CONTROL" PANEL
//
//~============================================================================

extern _b ctrl_ctrl_msk[];

void create_control_panel(int x0, int y0, int ind) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 40, h1 = 30, h2 = 15;
  char *txt[] = {"range", "heat", "FG", "test", "pi_ena"};
    
//~--

  w = 5 + w1 + 5 * w2 + 5;
  h = 5 + h1 + 3 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "CONTROL", 'L');
  y = y + h1;
  add_frame_box(x, y, w1, h2, "", 'S');
  create_frame_box_hstring(5, x+w1, y, w2, h2, &txt[0], 'S', 'C');
  y = y + h2;

//~--

  add_frame_box(x, y, w1, h2, "W", 'S');
  create_btn_hstring(
    5, x+w1, y, w2, h2, hex, 'N', &w_ctrl[ind], ctrl_ctrl_msk, &CTRL_BTN[ind][0]);
  y = y + h2;
  add_frame_box(x, y, w1, h2, "R", 'S');
  create_dat_box_hstring(
    5, x+w1, y, w2, h2,      'N', &r_ctrl[ind], ctrl_ctrl_msk, &CTRL_BOX[ind][0]);
    
  obj = CTRL_BTN[ind][1]->obj;
  fl_hide_object(obj);
  fl_deactivate_object(obj);
  fl_get_object_geometry(obj, &x, &y, &w, &h);
  add_frame_box(x, y, w, h, "", 'S');
}

//~----------------------------------------------------------------------------

void update_control_panel(int ind) {

  show_btn_string(    5,      NULL, &CTRL_BTN[ind][0]);
  show_dat_box_string(5, hex, NULL, &CTRL_BOX[ind][0]);
}

//~============================================================================
//
//                  "SETTINGS" PANEL
//
//~============================================================================

extern _b ctrl_set_msk[];

int d[8][5] = {
  {2, 1, 10},              // Set_point
  {3, 1, 10, 100},         // k1
  {3, 1,  8,  64},         // k2
  {3, 1, 10, 100},         // k3
  {2, 1,  8},              // iband
  {4, 1, 10, 100, 1000},   // Feed_forw
  {4, 1,  8,  64,  256},   // Test_T
  {2, 1,  8}};             // cav_magine

void create_setting_panel(int x0, int y0, int ind) {

  int i;
  int x, y, w, h;
  int w1 = 70, w2 = 20, w3 = 60, h1 = 30, h2 = 15;
  char *txt[] = {"Set_point", "k1",        "k2",     "k3", 
                 "iband",     "Feed_forw", "Test_T", "cav_magin"};
    
//~--

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 8 * 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "SETTINGS", 'L');
  y = y + h1;
  create_frame_box_vstring(8, x, y, w1, 2*h2, &txt[0], 'S', 'C');
  x = x + w1;
  
//~--

  for (i=0; i<8; i++) {
    add_frame_box(x, y, w2, h2, "W", 'S');
    create_btn_vstring(
      1, x+w2, y, w3, h2, NULL, 'N', &w_ctrl[ind], &ctrl_set_msk[i], &SET_BTN[ind][i]);
    add_hidden_btn_zones(SET_BTN[ind][i], (int*)d[i]);
    y = y + h2;
    add_frame_box(x, y, w2, h2, "R", 'S');
    create_dat_box_vstring(
      1, x+w2, y, w3, h2,       'N', &r_ctrl[ind], &ctrl_set_msk[i], &SET_BOX[ind][i]);
    y = y + h2;
  }
  SET_BTN[ind][0]->cnv = convert_Set_point_code_to_value;
  SET_BOX[ind][0]->cnv = convert_Set_point_code_to_value;
  SET_BTN[ind][1]->cnv = NULL;
  SET_BOX[ind][1]->cnv = NULL;
  SET_BTN[ind][2]->cnv = convert_k2_code_to_value;
  SET_BOX[ind][2]->cnv = convert_k2_code_to_value;
  SET_BTN[ind][4]->cnv = convert_iband_code_to_value;
  SET_BOX[ind][4]->cnv = convert_iband_code_to_value;
  SET_BTN[ind][5]->cnv = convert_Feed_forw_code_to_value_1;
  SET_BOX[ind][5]->cnv = convert_Feed_forw_code_to_value_2;
  SET_BTN[ind][6]->cnv = convert_Test_T_code_to_value;
  SET_BOX[ind][6]->cnv = convert_Test_T_code_to_value;
  SET_BTN[ind][7]->cnv = convert_cav_magine_code_to_value;
  SET_BOX[ind][7]->cnv = convert_cav_magine_code_to_value;
}

//~----------------------------------------------------------------------------

void convert_Set_point_code_to_value(int32 code, int32 *value) {

  *value = (char)((int8)code << 2) / 4;
}

//~----------------------------------------------------------------------------

void convert_k2_code_to_value(int32 code, int32 *value) {

  float val = (float)code / 16.0;
  *value = *(int32*)&val;
}

//~----------------------------------------------------------------------------

void convert_iband_code_to_value(int32 code, int32 *value) {

  float val = (float)code / 16.0;
  *value = *(int32*)&val;
}

//~----------------------------------------------------------------------------

void convert_Feed_forw_code_to_value_1(int32 code, int32 *value) {

  *value = (short)(code);
}

//~----------------------------------------------------------------------------

void convert_Feed_forw_code_to_value_2(int32 code, int32 *value) {

  *value = (short)(code);
}

//~----------------------------------------------------------------------------

void convert_Test_T_code_to_value(int32 code, int32 *value) {

  float val = (float)(short)(code << 4) / 16.0 / 16.0;
  *value = *(int32*)&val;
}

//~----------------------------------------------------------------------------

void convert_cav_magine_code_to_value(int32 code, int32 *value) {

  float val = (float)code / 2.0;
  *value = *(int32*)&val;
}

//~----------------------------------------------------------------------------

void update_setting_panel(int ind) {

  int i;
  char *fmt[] = {
//Set_point  k1      k2        k3      iband  Feed_forw  Test_T   cav_magine
//  "%+d",  "%d", "%+7.4f", "%+7.5f", "%+7.4f", "%+d",   "%+7.3f", "%+3.1f"};
    "%+d",  "%d", "%+7.4f", "%d",     "%+7.4f", "%+d",   "%+7.3f", "%+3.1f"};

  for (i=0; i<8; i++) {
    show_btn_string(    1,       fmt[i], &SET_BTN[ind][i]);
    show_dat_box_string(1, NULL, fmt[i], &SET_BOX[ind][i]);
  }
}

//~============================================================================
//
//                  "PARAMETERS" PANEL
//
//~============================================================================

extern _b ctrl_par_msk[];

void create_parameter_panel(int x0, int y0, int ind) {

  int x, y, w, h;
  int w1 = 80, w2 = 70, h1 = 30, h2 = 15;
  char *txt[] = {"ph_term", "ih_term", "pi_dcv", "cycle_cnt"};
    
//~--

  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + 4 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "PARAMETERS", 'L');
  y = y + h1;
  create_frame_box_vstring(4, x, y, w1, h2, &txt[0], 'S', 'C');
  x = x + w1;
  
//~--

  create_dat_box_vstring(
    4, x, y, w2, h2, 'N', &r_ctrl[ind], ctrl_par_msk, &PAR_BOX[ind][0]);
  PAR_BOX[ind][0]->cnv = convert_ph_ih_term_code_to_value;
  PAR_BOX[ind][1]->cnv = convert_ph_ih_term_code_to_value;
  PAR_BOX[ind][2]->cnv = NULL;
  PAR_BOX[ind][3]->cnv = NULL;
}

//~----------------------------------------------------------------------------

void convert_ph_ih_term_code_to_value(int32 code, int32 *value) {

  *value = (short)code;
}

//~----------------------------------------------------------------------------

void update_parameter_panel(int ind) {

  int i;
  char *fmt[] = {
// ph_term   ih_term   pi_dcv   cycle_cnt
    "%+d",    "%+d",    "%d",    "%d"};

  for (i=0; i<4; i++) {
    show_dat_box_string(1, NULL, fmt[i], &PAR_BOX[ind][i]);
  }
}

//~============================================================================
//
//                  "ALARMS" PANEL
//
//~============================================================================

extern _b ctrl_ala_ena_msk[];
extern _b ctrl_ala_now_msk[];
extern _b ctrl_ala_was_msk[];

void create_alarm_panel(int x0, int y0, int ind) {

  int x, y, w, h;
  int w1 = 35, w2 = 20, w3 = 30, h1 = 30, h2 = 15;
  char *txt[] = {"GAC", "LPS", "LLR", "LLW", "PR2", "PR1", "TRK", "CAV"};
    
//~--

  w = 5 + w1 + 2 * w2 + 5 + 2 * w3 + 5;
  h = 5 + h1 + 2 * h2 + 8 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "ALARMS", 'L');
  y = y + h1;
  add_frame_box(x, y,   w1, 2*h2,    "", 'L');
  x = x + w1;
  add_frame_box(x, y, 2*w2,   h2, "Ena", 'S');
  x = x + 2 * w2 + 5;
  add_frame_box(x, y,   w3,   h2, "Now", 'S');
  x = x + w3;
  add_frame_box(x, y,   w3,   h2, "Was", 'S');
  y = y + h2;

  x = x0 + 5 + w1;
  add_frame_box(x, y, w2, h2, "W", 'S');
  x = x + w2;
  add_frame_box(x, y, w2, h2, "R", 'S');
  x = x + w2 + 5;
  add_frame_box(x, y, w3, h2, "R", 'S');
  x = x + w3;
  add_frame_box(x, y, w3, h2, "R", 'S');
  y = y + h2;

  x = x0 + 5;
  create_frame_box_vstring(8, x, y, w1, h2, &txt[0], 'S', 'C');
  x = x + w1;
  
//~--

  create_btn_vstring(
    8, x, y, w2, h2, d_e, 'N', &w_ctrl[ind], ctrl_ala_ena_msk, &ALA_ENA_BTN[ind][0]);
  x = x + w2;
  create_dat_box_vstring(
    8, x, y, w2, h2,      'N', &r_ctrl[ind], ctrl_ala_ena_msk, &ALA_ENA_BOX[ind][0]);
  x = x + w2 + 5;
  create_dat_box_vstring(
    8, x, y, w3, h2,      'N', &r_ctrl[ind], ctrl_ala_now_msk, &ALA_NOW_BOX[ind][0]);
  x = x + w3;
  create_dat_box_vstring(
    8, x, y, w3, h2,      'N', &r_ctrl[ind], ctrl_ala_was_msk, &ALA_WAS_BOX[ind][0]);
}

//~----------------------------------------------------------------------------

void update_alarm_panel(int ind) {

  show_btn_string(    8,      NULL, &ALA_ENA_BTN[ind][0]);
  show_dat_box_string(8, d_e, NULL, &ALA_ENA_BOX[ind][0]);
  show_dat_box_string(8, hex, NULL, &ALA_NOW_BOX[ind][0]);
  show_dat_box_string(8, hex, NULL, &ALA_WAS_BOX[ind][0]);
}

//~============================================================================
//
//                  "LOOPS" PANEL
//
//~============================================================================

void create_loop_panel(int x0, int y0, int ind) {

extern _b ctrl_loop_ena_msk[];
extern _b ctrl_loop_set_msk[];
extern _b ctrl_loop_out_msk[];

  int i;
  int x, y, w, h;
  int w1 = 35, w2 = 20, w3 = 15, w4 = 50, w5 = 20, h1 = 30, h2 = 15;
  char *txt[] = {"LLW", "LLR", "PR1", "PR2", "COR", "SUP"};
    
//~--

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + h2 + 6 * 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "LOOPS",     'L');
  y = y + h1;
  add_frame_box(x, y,   w1, h2, "",          'S');
  x = x + w1;
  add_frame_box(x, y,   w2, h2, "",          'S');
  x = x + w2;
  add_frame_box(x, y,   w3, h2, "",          'S');
  x = x + w3;
  add_frame_box(x, y,   w4, h2, "set point", 'S');
  x = x + w4;
  add_frame_box(x, y,   w5, h2, "",          'S');
  y = y + h2;

  x = x0 + 5;
  create_frame_box_vstring(6, x, y, w1, 2*h2, &txt[0], 'S', 'C');

  for (i=0; i<6; i++) {
    x = x0 + 5 + w1;
    add_frame_box(x, y, w2, h2, "W", 'S');
    x = x + w2;
    create_btn_vstring(
      1, x, y, w3, h2, d_e,  'N', &w_ctrl[ind], &ctrl_loop_ena_msk[i], &LOOP_ENA_BTN[ind][i]);
    x = x + w3;
    create_btn_vstring(
      1, x, y, w4, h2, NULL, 'N', &w_ctrl[ind], &ctrl_loop_set_msk[i], &LOOP_SET_BTN[ind][i]);
    add_hidden_btn_zones(LOOP_SET_BTN[ind][i], 2, 1, 8);
    x = x + w4;
    add_frame_box(x, y, w5, h2, "", 'S');
    x = x0 + 5 + w1;
    y = y + h2;
    add_frame_box(x, y, w2, h2, "R", 'S');
    x = x + w2;
    create_dat_box_vstring(
      1, x, y, w3, h2,       'N', &r_ctrl[ind], &ctrl_loop_ena_msk[i], &LOOP_ENA_BOX[ind][i]);
    x = x + w3;
    create_dat_box_vstring(
      1, x, y, w4, h2,       'N', &r_ctrl[ind], &ctrl_loop_set_msk[i], &LOOP_SET_BOX[ind][i]);
    x = x + w4;
    create_dat_box_vstring(
      1, x, y, w5, h2,       'N', &r_ctrl[ind], &ctrl_loop_out_msk[i], &LOOP_OUT_BOX[ind][i]);
    y = y + h2;
  }

  LOOP_SET_BTN[ind][0]->cnv = convert_set_point_A_code_to_value;
  LOOP_SET_BOX[ind][0]->cnv = convert_set_point_A_code_to_value;
  LOOP_SET_BTN[ind][1]->cnv = convert_set_point_A_code_to_value;
  LOOP_SET_BOX[ind][1]->cnv = convert_set_point_A_code_to_value;
  LOOP_SET_BTN[ind][2]->cnv = convert_set_point_B_code_to_value;
  LOOP_SET_BOX[ind][2]->cnv = convert_set_point_B_code_to_value;
  LOOP_SET_BTN[ind][3]->cnv = convert_set_point_B_code_to_value;
  LOOP_SET_BOX[ind][3]->cnv = convert_set_point_B_code_to_value;
  LOOP_SET_BTN[ind][4]->cnv = convert_set_point_A_code_to_value;
  LOOP_SET_BOX[ind][4]->cnv = convert_set_point_A_code_to_value;
  LOOP_SET_BTN[ind][5]->cnv = convert_set_point_A_code_to_value;
  LOOP_SET_BOX[ind][5]->cnv = convert_set_point_A_code_to_value;
}

//~----------------------------------------------------------------------------

void convert_set_point_A_code_to_value(int32 code, int32 *value) {

  *value = (char)((int8)code << 2) / 4;
}

//~----------------------------------------------------------------------------

void convert_set_point_B_code_to_value(int32 code, int32 *value) {

  float val = (char)((int8)code << 2) / 4. / 8.0;
  *value = *(int32*)&val;
}

//~----------------------------------------------------------------------------

void update_loop_panel(int ind) {

  int i;
  char *fmt[] = {
//   LLW     LLR      PR1       PR2     COR    SUP
    "%+d",  "%+d", "%+6.3f", "%+6.3f", "%+d", "%+d"};

  for (i=0; i<6; i++) {
    show_btn_string(    1,       NULL,   &LOOP_ENA_BTN[ind][i]);
    show_btn_string(    1,       fmt[i], &LOOP_SET_BTN[ind][i]);
    show_dat_box_string(1, d_e,  NULL,   &LOOP_ENA_BOX[ind][i]);
    show_dat_box_string(1, NULL, fmt[i], &LOOP_SET_BOX[ind][i]);
    show_dat_box_string(1, hex,  NULL,   &LOOP_OUT_BOX[ind][i]);
  }
}

//~============================================================================
//
//                  COMMAND BUTTON PANEL
//
//~============================================================================

void create_button_panel(int x0, int y0, int ind) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 50, w2 = 45, w3 = 55, h1 = 35;
    
//~--

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

//~--

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "WRITE\nALL", 'N');
    fl_set_object_callback(obj, write_all_loop_controls_callback, ind);
  x = x + w1;
  obj = add_normal_button(x, y, w2, h1, "READ\nALL", 'N');
    fl_set_object_callback(obj, read_all_loop_controls_callback, ind);
  x = x + w2;
  obj = add_normal_button(x, y, w3, h1, "UPDATE\nINPUT", 'N');
    fl_set_object_callback(obj, update_all_loop_controls_callback, ind);
}

//~----------------------------------------------------------------------------

void write_all_loop_controls_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  int8 adr[n_ctrl], dat[n_ctrl];
  _ww *w = &w_ctrl[ind];
  _r  *r = &r_ctrl[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  for (i=0; i<n_ctrl; i++) {
    adr[i] = n_ctrl * ind + i;
    dat[i] = w->inp[i];
  }

  dat[0x06] = w->inp[0x06] >> 8;
  dat[0x07] = w->inp[0x06] >> 0;
  dat[0x08] = w->inp[0x08] >> 8;
  dat[0x09] = w->inp[0x08] >> 0;
  dat[0x0A] = w->inp[0x0A] >> 8;
  dat[0x0B] = w->inp[0x0A] >> 0;
  dat[0x0C] = w->inp[0x0C] >> 8;
  dat[0x0D] = w->inp[0x0C] >> 0;
  
  write_TTCE_loop_ctrl(n_ctrl, adr, dat, &err);

  if (!err) {
    for (i=0; i<n_ctrl; i++) w->wri[i] = w->inp[i];
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

void read_all_loop_controls_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  int8 rea[2*n_ctrl];
  _r *r = &r_ctrl[ind];
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<n_ctrl; i++) r->was[i] = r->rea[i];
  
  read_TTCE_loop_ctrl(rea, &err);
  
  if (!err) {
    int s = n_ctrl * ind;
    for (i=0; i<n_ctrl; i++) r->rea[i] = rea[s+i];
    r->rea[0x06] = ((int16)rea[s+0x06] << 8) | rea[s+0x07];
    r->rea[0x08] = ((int16)rea[s+0x08] << 8) | rea[s+0x09];
    r->rea[0x0A] = ((int16)rea[s+0x0A] << 8) | rea[s+0x0B];
    r->rea[0x0C] = ((int16)rea[s+0x0C] << 8) | rea[s+0x0D];
    if (r->unk) for (i=0; i<n_ctrl; i++) r->was[i] = r->rea[i];
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

void update_all_loop_controls_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int i;
  _ww *w = &w_ctrl[ind];
  _r  *r = &r_ctrl[ind];

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  if (!err) read_all_loop_controls_callback(NULL, ind);
  if (!err) {
    for (i=0; i<n_ctrl; i++) w->inp[i] = r->rea[i];
    write_all_loop_controls_callback(NULL, ind);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }

}

//~============================================================================

void update_objects(void) {

  int i;
  
  update_template_objects(&msg, &err, NULL, &P);

  for (i=ifrom; i<ito; i++) {
    update_control_panel(i);
    update_setting_panel(i);
    update_parameter_panel(i);
    update_alarm_panel(i);
    update_loop_panel(i);
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
