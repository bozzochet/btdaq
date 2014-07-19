// file USCM.c
//
// USCM Controller
//
// A.Lebedev Apr-2005...

#include "template.h"
#include "uscmlib.h"
#include "uscmdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"USCM  Controller"};
char *date  = {"25-Dec-08"};

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

#define ECHO_NMAX      8192
#define ECHO_NOT_KNOWN   -1
#define ECHO_OK           0
#define ECHO_TOO_MUCH     1
#define ECHO_ERROR        2
#define ECHO_WRONG_LENGTH 3
#define ECHO_VFY_FAILED   4

SW_BTN *RND_ECHO_DIG[4];
FL_OBJECT *RND_ECHO_VFY;
int rnd_echo_N, rnd_echo_NN;
int8 rnd_echo_RQ[10000], rnd_echo_RP[10000];
int rnd_echo_sta, rnd_echo_error;

FL_OBJECT *ECHO_INP, *ECHO_RQ, *ECHO_RP;
int echo_N, echo_NN;
int8 echo_RQ[10], echo_RP[10000];
int echo_sta, echo_error;

//~--

SW_BTN    *VAL_BEG_NAM_BTN, *VAL_BEG_DIG_BTN[5];
FL_OBJECT                   *VAL_END_DIG_BOX[5];
FL_OBJECT *VAL_CKS_BOX[2];
_w w_val_beg_nam;
_w w_val_beg_cod;
_r r_val_end_cod;
_r r_val_cks;

//~--

SW_BTN *ADR_BTN[3], *NAM_BTN;
_ww w_adr;

//~--

#define STA_NOT_KNOWN -1
#define STA_OK         0

FL_OBJECT *STA_MODE;
FL_OBJECT *STA_VERSION;
FL_OBJECT *STA_HIST_COD, *STA_HIST_TXT;
FL_OBJECT *STA_NB_ID, *STA_LOG_ID, *STA_LOG_NAME;
FL_OBJECT *STA_FLASH_OK, *STA_CONFIG_OK;
FL_OBJECT *STA_WAS_SEL, *STA_PWR_FF;
FL_OBJECT *STA_CMD_CNTR, *STA_BACKDOOR;
int8 sta_dat[9];
int  sta_sta;
int8 sta_mode, *sta_version, sta_history, sta_NB_ID, sta_log_ID;
int8 sta_pgm_OK, sta_config_OK, sta_was_SEL, sta_PWR_FF;
int8 sta_cmd_cntr, sta_backdoor;

//~--

SW_BTN *ENA_BTN[2], *RATE_BTN[2], *ROLL_RX_BTN[2], *ROLL_TX_BTN[2];
_w w_ena[2], w_rate[2], w_roll_rx[2], w_roll_tx[2];
FL_OBJECT *ENA_BOX[2], *RATE_BOX[2], *ROLL_RX_BOX[2], *ROLL_TX_BOX[2];
_r r_ena[2], r_rate[2], r_roll_rx[2], r_roll_tx[2];

//~--

FL_OBJECT *BOOT_INFO;

//~--

#define LONG_STA_NOT_KNOWN -1
#define LONG_STA_OK         0

FL_OBJECT *LONG_STA_VER;
int8 long_sta_dat[727];
int long_sta_sta;

//~--

#define SHORT_STA_NOT_KNOWN -1
#define SHORT_STA_OK         0

FL_OBJECT *SHORT_STA_DAT[7], *SHORT_STA_BIT[8];
int8 short_sta_dat[14];
int short_sta_sta;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
char *ok_bad[2]  = {"OK",     "BAD"};
char *no_yes[2]  = {"NO",     "YES"};
char *sel[2]     = {"no SEL", "SEL"};
char *off_on[2]  = {"OFF",    "ON"};
char *cls_opn[2] = {"closed", "open"};
char *rates[5] = {"2400", "4800", "9600", "19200", "?"};
char *d_e[2] = {"D", "E"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_run_panel(int x0, int y0);
void run_callback(FL_OBJECT *obj, long data);
void reset_callback(FL_OBJECT *obj, long data);

void create_power_panel(int x0, int y0);
void power_callback(FL_OBJECT *obj, long data);

void create_random_echo_panel(int x0, int y0);
void random_echo_digit_callback(int32 data);
void random_echo_callback(FL_OBJECT *obj, long data);
void update_random_echo_panel(void);

void create_echo_panel(int x0, int y0);
void echo_input_callback(FL_OBJECT *obj, long data);
void echo_input_focus_callback(FL_OBJECT *obj, long data);
void echo_callback(FL_OBJECT *obj, long data);
void update_echo_panel(void);

void create_validate_panel(int x0, int y0);
void validate_object_name_callback(int32 data);
void validate_callback(FL_OBJECT *obj, long data);
void update_validate_panel(void);

void create_erase_flash_panel(int x0, int y0);
void erase_flash_callback(FL_OBJECT *obj, long data);

void create_exec_config_file_panel(int x0, int y0);
void exec_config_file_callback(FL_OBJECT *obj, long data);

void create_WLA_panel(int x0, int y0);
void WLA_callback(FL_OBJECT *obj, long data);
void update_WLA_panel(void);

void create_status_panel(int x0, int y0);
void status_callback(FL_OBJECT *obj, long data);
void backdoor_callback(FL_OBJECT *obj, long data);
void clear_power_FF_callback(FL_OBJECT *obj, long data);
void update_status_panel(void);

void create_ports_config_panel(int x0, int y0);
void get_ports_config_callback(FL_OBJECT *obj, long data);
void set_ports_config_callback(FL_OBJECT *obj, long data);
void update_ports_config_panel(void);

void create_boot_panel(int x0, int y0);
void boot_callback(FL_OBJECT *obj, long data);
void update_boot_panel(void);

void create_long_status_panel(int x0, int y0);
void long_status_callback(FL_OBJECT *obj, long data);
void update_long_status_panel(void);

void create_short_status_panel(int x0, int y0);
void short_status_callback(FL_OBJECT *obj, long data);
void update_short_status_panel(void);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 710, HH = 400;

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  invalidate_data();

  create_run_panel(               0,  45);
  create_power_panel(           120,  45);
  create_random_echo_panel(       0,  95);
  create_echo_panel(              0, 145);
  create_validate_panel(          0, 195);
  create_erase_flash_panel(       0, 265);
  create_exec_config_file_panel(185, 265);
  create_WLA_panel(               0, 320);
  
  create_status_panel(      250,  45);
  create_ports_config_panel(250, 250);

  create_boot_panel(        460,  45);
  create_long_status_panel( 460, 115);
  create_short_status_panel(460, 165);
  
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
  
  rnd_echo_N = 0;
  rnd_echo_sta = ECHO_NOT_KNOWN;

  echo_N = 0;
  echo_NN = 0;
  echo_sta = ECHO_NOT_KNOWN;

  invalidate_ww(first, &w_adr, 1, 0);

  w_val_beg_nam.inp = 0;
  w_val_beg_nam.unk = TRUE;
  w_val_beg_cod.inp = 0x20000;
  w_val_beg_cod.unk = TRUE;
  
  r_val_end_cod.unk = TRUE;;
  r_val_end_cod.sta = status_NA;
  r_val_cks.unk   = TRUE;;
  r_val_cks.sta = status_NA;
  
  long_sta_sta = LONG_STA_NOT_KNOWN;

  short_sta_sta = SHORT_STA_NOT_KNOWN;

  sta_sta = STA_NOT_KNOWN;

  for (i=0; i<2; i++) {
    w_ena[i].inp  = 0;
    w_ena[i].unk  = TRUE;
    w_rate[i].inp = 3;
    w_rate[i].unk = TRUE;
    w_roll_rx[i].inp = 0;
    w_roll_tx[i].inp = 0;
    w_roll_rx[i].unk = TRUE;
    w_roll_tx[i].unk = TRUE;

    r_ena[i].unk  = TRUE;
    r_ena[i].sta  = status_NA;
    r_rate[i].unk = TRUE;
    r_rate[i].sta = status_NA;
    r_roll_rx[i].unk = TRUE;
    r_roll_tx[i].unk = TRUE;
    r_roll_rx[i].sta = status_NA;
    r_roll_tx[i].sta = status_NA;
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
    fread(&rnd_echo_N, sizeof(rnd_echo_N), 1, file);
    fread(&echo_N,     sizeof(echo_N),     1, file);
    fread(&echo_RQ,    sizeof(echo_RQ),    1, file);
    fread(w_adr.inp,   4,                  1, file);
    fread(&w_val_beg_nam.inp, sizeof(w_val_beg_nam.inp), 1, file);
    fread(&w_val_beg_cod.inp, sizeof(w_val_beg_cod.inp), 1, file);
    for (i=0; i<2; i++) {
      fread(&w_ena[i].inp,  sizeof(w_ena[i].inp),  1, file);
      fread(&w_rate[i].inp, sizeof(w_rate[i].inp), 1, file);
      fread(&w_roll_rx[i].inp, sizeof(w_roll_rx[i].inp), 1, file);
      fread(&w_roll_tx[i].inp, sizeof(w_roll_tx[i].inp), 1, file);
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
    fwrite(&rnd_echo_N, sizeof(rnd_echo_N), 1, file);
    fwrite(&echo_N,     sizeof(echo_N),     1, file);
    fwrite(&echo_RQ,    sizeof(echo_RQ),    1, file);
    fwrite(w_adr.inp,   4,                  1, file);
    fwrite(&w_val_beg_nam.inp, sizeof(w_val_beg_nam.inp), 1, file);
    fwrite(&w_val_beg_cod.inp, sizeof(w_val_beg_cod.inp), 1, file);
    for (i=0; i<2; i++) {
      fwrite(&w_ena[i].inp,  sizeof(w_ena[i].inp),  1, file);
      fwrite(&w_rate[i].inp, sizeof(w_rate[i].inp), 1, file);
      fwrite(&w_roll_rx[i].inp, sizeof(w_roll_rx[i].inp), 1, file);
      fwrite(&w_roll_tx[i].inp, sizeof(w_roll_tx[i].inp), 1, file);
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
//      RUN COMMANDS PANEL (Run OTP, Run Flash, Reset CPU)
//
//~============================================================================

void create_run_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 40, w3 = 40, h1 = 20;

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, 2*h1, "Run", 'N');
  x = x + w1;
  
  obj = add_normal_button(x, y+0*h1, w2, h1, "OTP", 'M');
    fl_set_object_callback(obj, run_callback, 1);
  obj = add_normal_button(x, y+1*h1, w2, h1, "Flash", 'M');
    fl_set_object_callback(obj, run_callback, 2);
  x = x + w2;
  
  obj = add_normal_button(x, y, w3, 2*h1, "CPU\nRST", 'M');
    fl_set_object_callback(obj, reset_callback, 0);
}

//~----------------------------------------------------------------------------

void run_callback(FL_OBJECT *obj, long data) {

  int8 from = data;
  
  msg = -1;
  update_objects();
  
  boot_USCM(from, &err);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void reset_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();
  
  boot_USCM(0, &err);
  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//      POWER CONTROL PANEL
//
//~============================================================================

void create_power_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 30, w2 = 45, w3 = 45, h1 = 20;

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, 2*h1, "Pwr\nCtrl", 'N');
  x = x + w1;
  
  obj = add_normal_button(x, y+0*h1, w2, h1, "SON", 'M');
    fl_set_object_callback(obj, power_callback, 4);
  obj = add_normal_button(x, y+1*h1, w2, h1, "SOFF", 'M');
    fl_set_object_callback(obj, power_callback, 3);
  x = x + w2;
  
  obj = add_normal_button(x, y, w3, 2*h1, "SUIC", 'M');
    fl_set_object_callback(obj, power_callback, 5);
}

//~----------------------------------------------------------------------------

void power_callback(FL_OBJECT *obj, long data) {

  int8 from = data;
  
  msg = -1;
  update_objects();
  
  boot_USCM(from, &err);
  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//      CPU RESET PANEL
//
//~============================================================================

void create_reset_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 65, h1 = 20;

  w = 5 + w1 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "CPU", 'M');
  y = y + h1;
  
  obj = add_normal_button(x, y, w1, h1, "RESET", 'M');
    fl_set_object_callback(obj, reset_callback, 0);
}

//~============================================================================
//
//      RANDOM ECHO PANEL
//
//~============================================================================

void create_random_echo_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, m;
  int x, y, w, h, size;
  int w1 = 20, w2 = 110, w3 = 50;
  int h1 = 20, h2 = 20;

  w = 5 + 4 * w1 + w2 + w3 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  size = 'M';
  
  x = x0 + 5;
  y = y0 + 5;
  for (i=0, m=1000; i<4; i++, m=m/10) {
    int d = (rnd_echo_N / m) % 10;
    RND_ECHO_DIG[i] = add_switch_btn(x, y, w1, h1, hex, size, 10, d);
    RND_ECHO_DIG[i]->call = random_echo_digit_callback;
    x = x + w1;
  }
  
  obj = add_frame_box(x, y, w2, h1, "random bytes", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w2;

  obj = add_normal_button(x, y, w3, h1, "ECHO", size);
    fl_set_object_callback(obj, random_echo_callback, 0);
  x = x + w3;

  x = x0 + 5;
  y = y + h1;
  RND_ECHO_VFY = 
  obj = add_data_box(x, y, 4*w1+w2+w3, h2, "X", size);
  x = x + 4 * w1 + w2 + w3;
}

//~----------------------------------------------------------------------------

void random_echo_digit_callback(int32 data) {

  SW_BTN *B;
  int i;
    
  rnd_echo_N = 0;
  for (i=0; i<4; i++) {
    B = RND_ECHO_DIG[i];
    rnd_echo_N = 10 * rnd_echo_N + B->i;
    fl_set_object_label(B->obj, B->l[B->i]);
  }

  if (rnd_echo_N > ECHO_NMAX) rnd_echo_sta = ECHO_TOO_MUCH;
  else                        rnd_echo_sta = ECHO_NOT_KNOWN;

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void random_echo_callback(FL_OBJECT *obj, long data) {

  int i;
  
  for (i=0; i<rnd_echo_N; i++) rnd_echo_RQ[i] = rand();

  msg = -1;
  update_objects();
  
  ping_USCM(rnd_echo_N, rnd_echo_RQ, &rnd_echo_NN, rnd_echo_RP, &err);
  
  if (err) {
    rnd_echo_sta = ECHO_ERROR;
  }
  else {
    rnd_echo_sta = ECHO_OK;
    if (rnd_echo_N != rnd_echo_NN) {
      rnd_echo_sta = ECHO_WRONG_LENGTH;
    }
    else {
      rnd_echo_error = 0;
      for (i=0; i<rnd_echo_NN; i++) {
        if (rnd_echo_RQ[i] != rnd_echo_RP[i]) rnd_echo_error++;
      }
      if (rnd_echo_error) rnd_echo_sta = ECHO_VFY_FAILED;
    }
  }
    
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_random_echo_panel(void) {

  FL_OBJECT *obj;
  int i;
  char b[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;

  for (i=0; i<4; i++) {
    obj = RND_ECHO_DIG[i]->obj;
    if (rnd_echo_sta == ECHO_TOO_MUCH) color = alarm;
    else                               color = good;
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }  

  obj = RND_ECHO_VFY;

  if      (rnd_echo_sta == ECHO_TOO_MUCH) {
    color = warning;
    sprintf(b, "Too much!");
  }
  else if (rnd_echo_sta == ECHO_NOT_KNOWN) {
    color = warning;
    sprintf(b, " ");
  }
  else if (rnd_echo_sta == ECHO_ERROR) {
    color = error;
    sprintf(b, " ");
  }
  else if (rnd_echo_sta == ECHO_WRONG_LENGTH) {
    color = alarm;
    sprintf(b, "%d byte(s) read back", rnd_echo_NN);
  }
  else if (rnd_echo_sta == ECHO_VFY_FAILED) {
    color = alarm;
    sprintf(b, "%d mismatch(es)", rnd_echo_error);
  }
  else if (rnd_echo_sta == ECHO_OK) {
    color = good;
    sprintf(b, "Verification OK");
  }
  else {
    color = alarm;
    sprintf(b, "Something is wrong");
  }

  fl_set_object_label(obj, b);
  fl_set_object_color (obj,  colors[color], 0);
  fl_set_object_lcolor(obj, lcolors[color]);
}

//~============================================================================
//
//      (SHORT) ECHO PANEL
//
//~============================================================================

void create_echo_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 190, w2 = 50;
  int h1 = 20, h2 = 20;

  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  ECHO_INP = 
  obj = fl_add_input(FL_NORMAL_INPUT,x-5,y-5,w1+10,h1+10," ");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_WHEAT);
    fl_set_object_lsize(obj,'L');
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_input_color(obj,FL_BLACK,FL_BLACK);
    fl_set_input_return(obj,FL_RETURN_END);
    fl_set_object_callback(obj, echo_input_callback, 0);
    fl_deactivate_object(obj);
    fl_hide_object(obj);
  ECHO_RQ =
  obj = add_switch_button(x, y, w1, h1, "x", 'M');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_callback(obj, echo_input_focus_callback, 0);
  x = x + w1;
  
  obj = add_normal_button(x, y, w2, h1, "ECHO", 'M');
    fl_set_object_callback(obj, echo_callback, 0);
  x = x + w2;

  x = x0 + 5;
  y = y + h1;
  ECHO_RP = 
  obj = add_data_box(x, y, w1+w2, h2, "X", 'M');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1 + w2;
}

//~----------------------------------------------------------------------------

void echo_input_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *ob = ECHO_RQ;
  int i;
  int status, q[6];

  status = sscanf(fl_get_input(obj), "%X %X %X %X %X %X",
                  &q[0], &q[1], &q[2], &q[3], &q[4], &q[5]);

  if (status > 0) echo_N = status;
  else            echo_N = 0;
  for (i=0; i<echo_N; i++) echo_RQ[i] = q[i];     // fuck you, c-I/O
  
  echo_sta = ECHO_NOT_KNOWN;
  
  fl_deactivate_object(obj);
  fl_hide_object(obj);

  fl_activate_object(ob);
  fl_show_object(ob);

//  write_dat_config_file();

  update_objects();
}

//~----------------------------------------------------------------------------

void echo_input_focus_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *ob = ECHO_INP;
  int i, ii;
  char b[80];
  int mode = -1;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  mode = 0;
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) mode = 1;

  fl_activate_object(ob);
  fl_show_object(ob);
  if (mode) {
    b[0] = '\0';
    for (i=0, ii=0; i<echo_N; i++) {
      ii = ii + sprintf(&b[ii], "%02X ", echo_RQ[i]);
    }
  }
  else {
    sprintf(b, " ");
  }
  fl_set_input(ob, b);
  
  fl_deactivate_object(obj);
  fl_hide_object(obj);

  update_objects();
}

//~----------------------------------------------------------------------------

void echo_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *ob1 = ECHO_RQ;
  FL_OBJECT *ob2 = ECHO_INP;
  int i;
  
  fl_deactivate_object(ob2);
  fl_hide_object(ob2);

  fl_activate_object(ob1);
  fl_show_object(ob1);

  msg = -1;
  update_objects();
  
  ping_USCM(echo_N, echo_RQ, &echo_NN, echo_RP, &err);
  
  if (err) {
    echo_sta = ECHO_ERROR;
  }
  else {
    echo_sta = ECHO_OK;
    if (echo_N != echo_NN) {
      echo_sta = ECHO_WRONG_LENGTH;
    }
    else {
      echo_error = 0;
      for (i=0; i<echo_NN; i++) {
        if (echo_RQ[i] != echo_RP[i]) echo_error++;
      }
      if (echo_error) echo_sta = ECHO_VFY_FAILED;
    }
  }
    
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_echo_panel(void) {

  FL_OBJECT *obj;
  int i, ii;
  char b[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;

  obj = ECHO_RQ;

  b[0] = '\0';
  for (i=0, ii=0; i<echo_N; i++) {
    ii = ii + sprintf(&b[ii], "%02X ", echo_RQ[i]);
  }
  fl_set_object_label(obj, b);

  obj = ECHO_RP;

  b[0] = '\0';
  for (i=0, ii=0; i<MIN(6,echo_NN); i++) {
    ii = ii + sprintf(&b[ii], "%02X ", echo_RP[i]);
  }

  if      (echo_sta == ECHO_NOT_KNOWN) {
    color = warning;
    sprintf(b, " ");
  }
  else if (echo_sta == ECHO_ERROR) {
    color = error;
  }
  else if (echo_sta == ECHO_WRONG_LENGTH || echo_sta == ECHO_VFY_FAILED) {
    color = alarm;
    ii = ii + sprintf(&b[ii], ":ERR");
  }
  else if (echo_sta == ECHO_OK) {
    color = good;
    ii = ii + sprintf(&b[ii], ":OK");
  }
  else {
    color = alarm;
    sprintf(b, "Something is wrong");
  }

  fl_set_object_label(obj, b);
  fl_set_object_color (obj,  colors[color], 0);
  fl_set_object_lcolor(obj, lcolors[color]);
}

//~============================================================================
//
//      VALIDATE FLASH MEMORY PANEL
//
//~============================================================================

char *val_beg_names[3] = {"Flash\nProgram", "Config.\nFile", "Unknown\nObject"};

void create_validate_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 75, w2 = 15, w3 = 50, w4 = 40, h1 = 20;

  w = 5 + w1 + 5 * w2 + w3 + w4 + 5;
  h = 5 + h1 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "Object", 'N');
  x = x + w1;
  add_frame_box(x, y, 5*w2, h1, "Addresses", 'N');
  x = x + 5 * w2;
  add_frame_box(x, y, w3, h1, "CKS", 'N');
  x = x + w3;
  add_frame_box(x, y, w4, h1, "Cmd.", 'N');

  x = x0 + 5;
  y = y + h1;

  create_num_btn_hstring(
    1, x, y, w1, 2*h1, val_beg_names, 'M', 2, &w_val_beg_nam, &VAL_BEG_NAM_BTN);
  VAL_BEG_NAM_BTN->call = validate_object_name_callback;
  x = x + w1;
  create_hex_btn_hstring(
    5, x, y, w2, h1, hex, 'M', &w_val_beg_cod, &VAL_BEG_DIG_BTN[0]);
  create_data_box_hstring(
    5, x, y+h1, w2, h1, 'M', &VAL_END_DIG_BOX[0]);
  x = x + 5 * w2;
  create_data_box_vstring(
    2, x, y, w3, h1, 'M', &VAL_CKS_BOX[0]);
  x = x + w3;
  obj = add_normal_button(x, y, w4, 2*h1, "VAL", 'M');
    fl_set_object_callback(obj, validate_callback, 0);

  r_val_end_cod.was = malloc(4);
  r_val_end_cod.rea = malloc(4);
  r_val_cks.was = malloc(2*4);
  r_val_cks.rea = malloc(2*4);
}

//~----------------------------------------------------------------------------

void validate_object_name_callback(int32 data) {

  if (w_val_beg_nam.inp == 0) w_val_beg_cod.inp = 0x20000;
  if (w_val_beg_nam.inp == 1) w_val_beg_cod.inp = 0x38000;

  update_objects();
}

//~----------------------------------------------------------------------------

void validate_callback(FL_OBJECT *obj, long data) {

  int32 end;
  int16 cks_1, cks_2;
  
  msg = 0;
  update_objects();
  msg = -1;
  update_objects();
  
  *r_val_end_cod.was = *r_val_end_cod.rea;
  r_val_cks.was[0]   = r_val_cks.rea[0];
  r_val_cks.was[1]   = r_val_cks.rea[1];
  
  validate_FLASH(w_val_beg_cod.inp, NULL, &end, &cks_1, &cks_2, &err);

  if (!err) {
    w_val_beg_nam.wri = w_val_beg_nam.inp;
    w_val_beg_cod.wri = w_val_beg_cod.inp;
    w_val_beg_nam.unk = FALSE;
    w_val_beg_cod.unk = FALSE;
    *r_val_end_cod.rea = end;
    r_val_cks.rea[0]   = cks_1;
    r_val_cks.rea[1]   = cks_2;
    if (r_val_end_cod.unk) *r_val_end_cod.was = *r_val_end_cod.rea;
    if (r_val_cks.unk)      r_val_cks.was[0]  =  r_val_cks.rea[0];
    if (r_val_cks.unk)      r_val_cks.was[1]  =  r_val_cks.rea[1];
    r_val_end_cod.unk = FALSE;
    r_val_cks.unk     = FALSE;
    r_val_end_cod.sta = status_OK;
    r_val_cks.sta     = status_OK;
  }
  else {
    w_val_beg_nam.unk = TRUE;
    w_val_beg_cod.unk = TRUE;
    r_val_end_cod.sta = status_KO;
    r_val_cks.sta     = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_validate_panel(void) {

  if      (w_val_beg_cod.inp == 0x20000) w_val_beg_nam.inp = 0;
  else if (w_val_beg_cod.inp == 0x38000) w_val_beg_nam.inp = 1;
  else                                   w_val_beg_nam.inp = 2;

  show_bin_btn_string(1, &w_val_beg_nam,               &VAL_BEG_NAM_BTN);
  show_bin_btn_string(5, &w_val_beg_cod,               &VAL_BEG_DIG_BTN[0]);
  show_hex_box_string(5, &r_val_end_cod, hex,          &VAL_END_DIG_BOX[0]);
  show_num_box_string(2, &r_val_cks,     NULL, "%04X", &VAL_CKS_BOX[0]);
}

//~============================================================================
//
//      ERASE FLASH MEMORY PANEL
//
//~============================================================================

void create_erase_flash_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 55, w2 = 65, w3 = 50, h1 = 15;
  char *txt[3] = {"Program", "Config File", "Logical Adr"};

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 3 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 3*h1, "Erase\nFlash\nMemory", 'N');
  x = x + w1;
  
  create_frame_box_vstring(3, x, y, w2, h1, txt, 'S', 'L');
  x = x + w2;
  
  for (i=0; i<3; i++) {
    obj = add_normal_button(x, y, w3, h1, "ERASE", 'N');
      fl_set_object_callback(obj, erase_flash_callback, i);
    y = y + h1;
  }
}

//~----------------------------------------------------------------------------

void erase_flash_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int from[3] = {1, 7, 8};
  int to[3]   = {6, 7, 8};
  int sector;

  if (obj) {
    msg = -1;
    update_objects();
  }

  for (sector=from[ind]; sector<=to[ind]; sector++) {
    int i;
    bool done;
    start_FLASH_erasure(sector, &err);
    if (err) break;
    for (i=0; i<50; i++) {
      done = FALSE;
      usleep(100000);
      check_FLASH_erasure(&done, &err);
      if (err || done) break;
    }
    if (err || !done) break;
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//      WLA = WRITE LOGICAL ADDRESS PANEL
//
//~============================================================================

void create_WLA_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 100, w3 = 95, h1 = 20;
  _b adr_msk[] = {
    {0, 0x100,  2},
    {0, 0x0F0, 16},
    {0, 0x00F, 16}};
  _b nam_msk[] = {
    {0, 0x1FF, 512}};
  static char *USCMName[512];

  for (i=0; i<DIM(USCMName); i++) {
    int j;
    USCMName[i] = NULL;
    for (j=0; j<DIM(USCM_table); j++) {
      if (USCM_table[j].ok && USCM_table[j].ID == i) {
        USCMName[i] = USCM_table[j].name;
      }
    }
  }
  
  w = 5 + 3 * w1 + w2 + w3 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, 3*w1, h1, "Adr", 'N');
  x = x + 3 * w1;
  add_frame_box(x, y, w2,   h1, "Name", 'N');
  x = x + w2;
  obj = add_normal_button(x, y, w3, 2*h1, "WRITE\nLogical Adr", 'M');
    fl_set_object_callback(obj, WLA_callback, 0);

  x = x0 + 5;
  y = y + h1;

  create_btn_hstring(
    3, x, y, w1, h1, hex,  'M', &w_adr, adr_msk,  ADR_BTN);
  x = x + 3 * w1;
  create_btn_hstring(
    1, x, y, w2, h1, USCMName, 'M', &w_adr, nam_msk, &NAM_BTN);
}

//~----------------------------------------------------------------------------

void WLA_callback(FL_OBJECT *obj, long data) {

  _ww *w = &w_adr;
  int8 wri[2];
  
  msg = -1;
  update_objects();

  err = 0x0000;
  
  if (!err) erase_flash_callback(NULL, 2);

  wri[0] =  w->inp[0];
  wri[1] = ~w->inp[0];
  if (!err) write_USCM_memory(2, 0x3FFFE, wri, &err);

  if (!err) {
    w->wri[0] = w->inp[0];
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_WLA_panel(void) {
 
   show_btn_string(3, NULL,  ADR_BTN);
   show_btn_string(1, NULL, &NAM_BTN);
}

//~============================================================================
//
//      EXECUTE CONFIGURATION FILE PANEL
//
//~============================================================================

void create_exec_config_file_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 50, h1 = 45;

  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "EXEC\nConfig\nFile", 'N');
    fl_set_object_callback(obj, exec_config_file_callback, 0);
}

//~----------------------------------------------------------------------------

void exec_config_file_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  exec_config_file(&err);
  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//      STATUS PANEL
//
//~============================================================================

void create_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 100, w2 = 50, w3 = 55, w4 = 145, w5 = 20, w6 = 30, w7 = 150;
  int w8 = 65, w9 = 35, w10 = 100, w11 = 65;
  int h1 = 20, h2 = 15;

  w = 5 + w1 + 2 * w2 + 5;
  h = 5 + h1 + 3 * h1 + h1 + h2 + 4 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Status", 'M');
  x = x + w1;

  obj = add_normal_button(x, y, w2, h1, "READ", 'M');
    fl_set_object_callback(obj, status_callback, 0);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "WRITE", 'M');
    fl_set_object_callback(obj, status_callback, 1);
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w3, h1, "Mode", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w3;
  STA_MODE =
    obj = add_data_box(x, y, w4, h1, "", 'M');
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w3, h1, "Version", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w3;
  STA_VERSION =
    obj = add_data_box(x, y, w4, h1, "", 'M');
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w3, h1, "History", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w3;
  STA_HIST_TXT =
    obj = add_data_box(x, y, w4-w5, h1, "", 'M');
  x = x + w4 - w5;
  STA_HIST_COD =
    obj = add_data_box(x, y, w5, h1, "", 'M');
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w5, h1+h2, "ID", 'N');
  x = x + w5;
  add_frame_box(            x,    y,    w6, h2, "N/B", 'N');
  STA_NB_ID =
    obj = add_data_box(     x, y+h2,    w6, h1, "", 'M');
  add_frame_box(         x+w6,    y,    w7, h2, "Logical", 'N');
  STA_LOG_ID =
    obj = add_data_box(  x+w6, y+h2,    w6, h1, "", 'M');
  STA_LOG_NAME =
    obj = add_data_box(x+2*w6, y+h2, w7-w6, h1, "", 'M');
  y = y + h1 + h2;

  x = x0 + 5;
  obj = add_frame_box( x, y,  w8, h1, "Backdoor", 'N');
  x = x + w8;
  STA_BACKDOOR =
    obj = add_data_box(x, y,  w8, h1, "", 'M');
  x = x + w8;
  obj = add_normal_button(x, y, w9, h1, "OPN", 'M');
   fl_set_object_callback(obj, backdoor_callback, 0);
  x = x + w9;
  obj = add_normal_button(x, y, w9, h1, "CLS", 'M');
   fl_set_object_callback(obj, backdoor_callback, 1);
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box( x, y,  w8, h1, "Flash Pgm", 'N');
  x = x + w8;
  STA_FLASH_OK =
    obj = add_data_box(x, y,  w9, h1, "", 'M');
  x = x + w9;
  obj = add_frame_box( x, y,  w8, h1, "Config File", 'N');
  x = x + w8;
  STA_CONFIG_OK =
    obj = add_data_box(x, y,  w9, h1, "", 'M');
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box( x, y,  w8, h1, "Cmd. Cntr", 'N');
  x = x + w8;
  STA_CMD_CNTR =
    obj = add_data_box(x, y,  w9, h1, "", 'M');
  x = x + w9;
  obj = add_frame_box( x, y,  w8, h1, "Was SEL", 'N');
  x = x + w8;
  STA_WAS_SEL =
    obj = add_data_box(x, y,  w9, h1, "", 'M');
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box( x, y,  w10, h1, "Ext. Power FF", 'N');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w10;
  STA_PWR_FF =
    obj = add_data_box(x, y,  w9, h1, "", 'M');
  x = x + w9;
  obj = add_normal_button(x, y, w11, h1, "CLEAR", 'M');
   fl_set_object_callback(obj, clear_power_FF_callback, 0);
  y = y + h1;
}

//~----------------------------------------------------------------------------

void backdoor_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();
  
  if (data == 0) open_USCM_OTP_backdoor(&err);
  else          close_USCM_OTP_backdoor(&err);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void status_callback(FL_OBJECT *obj, long data) {

  int R_W = data;

  msg = 0;
  update_objects();
  msg = -1;
  update_objects();
  
  if (R_W) {
    reset_short_status(&err);
  }
  else {
    get_short_status(sta_dat, &err);
    if (err) {
      sta_sta = STA_NOT_KNOWN;
    }
    else {
      sta_sta = STA_OK;
      sta_mode =  sta_dat[0];
      if (sta_mode < 14) {
        if (sta_mode > 1) {
          sta_version    = &sta_dat[5];
          sta_history    = unpack16((int16)sta_dat[1], 0x0007);
          sta_backdoor   = unpack16((int16)sta_dat[1], 0x0008);
          sta_was_SEL    = unpack16((int16)sta_dat[1], 0x0010);
          sta_config_OK  = unpack16((int16)sta_dat[1], 0x0020);
          sta_pgm_OK     = unpack16((int16)sta_dat[1], 0x0040);
          sta_PWR_FF     = unpack16((int16)sta_dat[1], 0x0080);
          sta_NB_ID      = sta_dat[2];
          sta_log_ID     = sta_dat[3];
          sta_cmd_cntr   = sta_dat[4];
        }
      }
    }
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void clear_power_FF_callback(FL_OBJECT *obj, long data) {

  msg = 0;
  update_objects();
  msg = -1;
  update_objects();
  
  clear_power_FF(&err);

  if (err) sta_sta = STA_NOT_KNOWN;
  else     sta_sta = STA_OK;

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_status_panel(void) {

  int i;
  int               good = 0, warning = 1,  bad = 2;
  int colors[4]  = {FL_GREEN,   FL_YELLOW,  FL_RED};
  int lcolors[4] = {FL_BLACK,   FL_BLACK,   FL_YELLOW};
  int color;
  FL_OBJECT *obj[] = {
    STA_MODE,     STA_VERSION, STA_HIST_COD, STA_HIST_TXT,
    STA_NB_ID,    STA_LOG_ID,  STA_LOG_NAME,
    STA_FLASH_OK, STA_CONFIG_OK,
    STA_WAS_SEL,  STA_PWR_FF,
    STA_CMD_CNTR, STA_BACKDOOR};
  
  char *mode[14] = {
    "   HBB    ",           //   0   0x00
    "Impossible",           //   1   0x01
    "UNI_PROM"  ,           //   2   0x02
    "UNI_FLASH" ,           //   3   0x03
    "JPD_PROM"  ,           //   4   0x04
    "JPD_FLASH" ,           //   5   0x05
    "PDS_PROM"  ,           //   6   0x06
    "PDS_FLASH" ,           //   7   0x07
    "CC_PROM"   ,           //   8   0x08
    "CC_FLASH"  ,           //   9   0x09
    "UG_PROM"   ,           //  10   0x0A
    "UG_FLASH"  ,           //  11   0x0B
    "M_PROM"    ,           //  12   0x0C
    "M_FLASH"};             //  13   0x0D
  char *mode2[2] = {
    "CAB_PROM"  ,           //   0   0xF0
    "CAB_FLASH"};           //   1   0xF1

  char *hist[8][2] = {
//    PROM             FLASH
    {"Power ON",      "Power ON"},
    {"Reset",         "Reset"},
    {"Reset *",       "Reset *"},
    {"Flash",         "PROM"},              // "from else"
    {"Flash *",       "PROM *"},            // "from else"
    {"PROM",          "Flash"},             // "from self"
    {"PROM *",        "Flash *"},           // "from self"
    {"*************", "*************"}};
    
  char *month[13] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    "XXX"};
  
  if (sta_sta == STA_OK) {
    sta_mode =  sta_dat[0];
    if (sta_mode < 14) {
      fl_set_object_label(STA_MODE, mode[sta_mode]);
      if (sta_mode > 1) {
        char b[80], *p;
        int m;

        p = b;
        p += sprintf(p, "%02d-", sta_version[1]);
        m = MIN(12, unpack16((int16)sta_version[0], 0x000F) - 1);

        p += sprintf(p, "%s-", month[m]);
        p += sprintf(p, "%02d ", unpack16((int16)sta_version[0], 0x00F0));
        p += sprintf(p, "%02d:%02d", sta_version[2], sta_version[3]);
        fl_set_object_label(STA_VERSION, b);
        
        if (sta_mode & 0x01) m = 1;
        else                 m = 0;
        sprintf(b, "%d", sta_history);
        fl_set_object_label(STA_HIST_COD, b);
        sprintf(b, "%s", hist[sta_history][m]);
        fl_set_object_label(STA_HIST_TXT, b);

        sprintf(b, "%02hhX", sta_NB_ID);
        fl_set_object_label(STA_NB_ID, b);

        sprintf(b, "%02hhX", sta_log_ID);
        fl_set_object_label(STA_LOG_ID, b);
        fl_set_object_label(STA_LOG_NAME, "unknown");
        for (i=0; i<28; i++) {
          if (sta_log_ID == USCM_table[i].ID) {
            fl_set_object_label(STA_LOG_NAME, USCM_table[i].name);
          }
        }

        fl_set_object_label(STA_BACKDOOR,  cls_opn[sta_backdoor]);
        fl_set_object_label(STA_FLASH_OK,  ok_bad[sta_pgm_OK]);
        fl_set_object_label(STA_CONFIG_OK, ok_bad[sta_config_OK]);

        sprintf(b, "%d", sta_cmd_cntr);
        fl_set_object_label(STA_CMD_CNTR, b);

        if (sta_PWR_FF) fl_set_object_label(STA_WAS_SEL, "?");
        else            fl_set_object_label(STA_WAS_SEL, no_yes[sta_was_SEL]);

        fl_set_object_label(STA_PWR_FF,    off_on[sta_PWR_FF]);
      }
    }
    else if (sta_mode == 0xF0 && sta_mode == 0xF1) {
      fl_set_object_label(STA_MODE, mode2[sta_mode-0xF0]);
    }
    else {
      fl_set_object_label(STA_MODE, "********** ");
    }
    color = good;
    for (i=0; i<sizeof(obj)/4; i++) {
      fl_set_object_color (obj[i],  colors[color], 0);
      fl_set_object_lcolor(obj[i], lcolors[color]);
    }
  
    if (sta_pgm_OK) color = bad;
    else            color = good;
    fl_set_object_color (STA_FLASH_OK,  colors[color], 0);
    fl_set_object_lcolor(STA_FLASH_OK, lcolors[color]);

    if (sta_config_OK) color = bad;
    else               color = good;
    fl_set_object_color (STA_CONFIG_OK,  colors[color], 0);
    fl_set_object_lcolor(STA_CONFIG_OK, lcolors[color]);

    if (sta_was_SEL) color = bad;
    else             color = good;
    fl_set_object_color (STA_WAS_SEL,  colors[color], 0);
    fl_set_object_lcolor(STA_WAS_SEL, lcolors[color]);
  }
  else {
    color = warning;
    for (i=0; i<sizeof(obj)/4; i++) {
      fl_set_object_label(obj[i], "");
      fl_set_object_color (obj[i],  colors[color], 0);
      fl_set_object_lcolor(obj[i], lcolors[color]);
    }
  }
}

//~============================================================================
//
//      SERIAL PORTS CONFIGURATION PANEL
//
//~============================================================================

void create_ports_config_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 20, w3 = 55, w4 = 35, w5 = 40, h1 = 20, h2 = 15, h3 = 15;

  w = 5 + w1 + w2 + w3 + 2 * w4 + w5 + 5;
  h = 5 + h1 + 2 * h2 + 2 * 2 * h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");


  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "Serial Ports Configuration", 'M');
  y = y + h1;

  add_frame_box(x, y,    w1, 2*h2, "#",          'N');
  x = x + w1;
  add_frame_box(x, y,    w2, 2*h2, "E\nD",       'N');
  x = x + w2;
  add_frame_box(x, y,    w3, 2*h2, "Baud\nRate", 'N');
  x = x + w3;
  add_frame_box(x, y,  2*w4,   h2, "Roll",       'N');
  add_frame_box(x, y+h2, w4,   h2, "RX",         'N');
  x = x + w4;
  add_frame_box(x, y+h2, w4,   h2, "TX",         'N');
  x = x + w4;
  add_frame_box(x, y,    w5, 2*h2, "Cmd.",       'N');
  x = x + w5;

  y = y + 2 * h2;
  
  for (i=0; i<2; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, 2*h3, hex[i], 'N');
    x = x + w1;
    create_data_box_hstring(
      1, x, y,    w2, h3,         'N',                &ENA_BOX[i]);
    create_num_btn_hstring(
      1, x, y+h3, w2, h3, d_e,    'N', 2, &w_ena[i],  &ENA_BTN[i]);
    x = x + w2;
    create_data_box_hstring(
      1, x, y,    w3, h3,         'N',                &RATE_BOX[i]);
    create_num_btn_hstring(
      1, x, y+h3, w3, h3, rates,  'N', 4, &w_rate[i], &RATE_BTN[i]);
    x = x + w3;
    create_data_box_hstring(
      1, x, y,    w4, h3,         'N',                   &ROLL_RX_BOX[i]);
    create_num_btn_hstring(
      1, x, y+h3, w4, h3, no_yes, 'N', 2, &w_roll_rx[i], &ROLL_RX_BTN[i]);
    x = x + w4;
    create_data_box_hstring(
      1, x, y,    w4, h3,         'N',                   &ROLL_TX_BOX[i]);
    create_num_btn_hstring(
      1, x, y+h3, w4, h3, no_yes, 'N', 2, &w_roll_tx[i], &ROLL_TX_BTN[i]);
    x = x + w4;
    obj = add_normal_button(x, y,    w5, h3, "GET", 'N');
      fl_set_object_callback(obj, get_ports_config_callback, i);
    obj = add_normal_button(x, y+h3, w5, h3, "SET", 'N');
      fl_set_object_callback(obj, set_ports_config_callback, i);
    x = x + w5;
    y = y + 2 * h3;
    r_ena[i].rea  = malloc(4);
    r_ena[i].was  = malloc(4);
    r_rate[i].rea = malloc(4);
    r_rate[i].was = malloc(4);
    r_rate[i].rea = malloc(4);
    r_rate[i].was = malloc(4);
    r_roll_rx[i].rea = malloc(4);
    r_roll_rx[i].was = malloc(4);
    r_roll_tx[i].rea = malloc(4);
    r_roll_tx[i].was = malloc(4);
  }
}

//~----------------------------------------------------------------------------

void get_ports_config_callback(FL_OBJECT *obj, long data) {

  int8 port = data;
  int8 mode, rate;

  msg = -1;
  update_objects();

  *r_ena[port].was  = *r_ena[port].rea;
  *r_rate[port].was = *r_rate[port].rea;
  *r_roll_rx[port].was = *r_roll_rx[port].rea;
  *r_roll_tx[port].was = *r_roll_tx[port].rea;
  
  check_serial_port(port, &mode, &rate, &err);

  if (!err) {
    *r_ena[port].rea  = mode & SERIAL_PORT_ENABLE ? TRUE : FALSE;
    *r_rate[port].rea = rate;
    *r_roll_rx[port].rea = mode & SERIAL_RX_ROLL ? TRUE : FALSE;
    *r_roll_tx[port].rea = mode & SERIAL_TX_ROLL ? TRUE : FALSE;
    if (r_ena[port].unk)  *r_ena[port].was  = *r_ena[port].rea;
    if (r_rate[port].unk) *r_rate[port].was = *r_rate[port].rea;
    if (r_roll_rx[port].unk) *r_roll_rx[port].was = *r_roll_rx[port].rea;
    if (r_roll_tx[port].unk) *r_roll_tx[port].was = *r_roll_tx[port].rea;
    r_ena[port].unk  = FALSE;
    r_rate[port].unk = FALSE;
    r_roll_rx[port].unk = FALSE;
    r_roll_tx[port].unk = FALSE;
    r_ena[port].sta  = status_OK;
    r_rate[port].sta = status_OK;
    r_roll_rx[port].sta = status_OK;
    r_roll_tx[port].sta = status_OK;
  }
  else {
    r_ena[port].sta  = status_KO;
    r_rate[port].sta = status_KO;
    r_roll_rx[port].sta = status_KO;
    r_roll_tx[port].sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void set_ports_config_callback(FL_OBJECT *obj, long data) {

  int8 port = data;
  int8 mode = 0;
  
  r_ena[port].sta  = status_NA;
  r_rate[port].sta = status_NA;
  r_roll_rx[port].sta = status_NA;
  r_roll_tx[port].sta = status_NA;
  
  msg = -1;
  update_objects();
  
  mode  = w_ena[port].inp;
  mode |= w_roll_rx[port].inp ? SERIAL_RX_ROLL : 0;
  mode |= w_roll_tx[port].inp ? SERIAL_TX_ROLL : 0;
  setup_serial_port(port, mode, w_rate[port].inp, &err);
  
  if (!err) {
    w_ena[port].wri  = w_ena[port].inp;
    w_rate[port].wri = w_rate[port].inp;
    w_roll_rx[port].wri = w_roll_rx[port].inp;
    w_roll_tx[port].wri = w_roll_tx[port].inp;
    w_ena[port].unk  = FALSE;
    w_rate[port].unk = FALSE;
    w_roll_rx[port].unk = FALSE;
    w_roll_tx[port].unk = FALSE;
  }
  else {
    w_ena[port].unk  = TRUE;
    w_rate[port].unk = TRUE;
    w_roll_rx[port].unk = TRUE;
    w_roll_tx[port].unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_ports_config_panel(void) {

  int i;
  
  for (i=0; i<2; i++) {
    show_hex_box_string(1, &r_ena[i],        d_e, &ENA_BOX[i]);
    show_hex_box_string(1, &r_rate[i],     rates, &RATE_BOX[i]);
    show_hex_box_string(1, &r_roll_rx[i], no_yes, &ROLL_RX_BOX[i]);
    show_hex_box_string(1, &r_roll_tx[i], no_yes, &ROLL_TX_BOX[i]);
    show_bin_btn_string(1, &w_ena[i],             &ENA_BTN[i]);
    show_bin_btn_string(1, &w_rate[i],            &RATE_BTN[i]);
    show_bin_btn_string(1, &w_roll_rx[i],         &ROLL_RX_BTN[i]);
    show_bin_btn_string(1, &w_roll_tx[i],         &ROLL_TX_BTN[i]);
  }
}

//~============================================================================
//
//      HBB BOOT PANEL
//
//~============================================================================

void create_boot_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 80;
  int h1 = 20;

  w = 5 + 3 * w1 + 5;
  h = 5 + 3 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, 3*w1, h1, "Boot (HBB Software)", 'M');
  y = y + h1;
  
  obj = add_normal_button(x+0*w1, y, w1, h1, "BOOT", 'M');
    fl_set_object_callback(obj, boot_callback, 0);
  obj = add_normal_button(x+1*w1, y, w1, h1, "OTP", 'M');
    fl_set_object_callback(obj, boot_callback, 1);
  obj = add_normal_button(x+2*w1, y, w1, h1, "FLASH", 'M');
    fl_set_object_callback(obj, boot_callback, 2);
  y = y + h1;

  BOOT_INFO =      
  obj = add_data_box(x, y, 3*w1, h1, "x", 'L');
}

//~----------------------------------------------------------------------------

void boot_callback(FL_OBJECT *obj, long data) {

  int8 from = data;
  FL_OBJECT *ob = BOOT_INFO;
  int i;
  int16 error;
  
  fl_set_object_color(ob, FL_YELLOW, 0);
  fl_set_object_lcolor(ob, FL_BLACK);
  fl_set_object_label(ob, "WAIT");

  msg = -1;
  update_objects();
  
  boot_USCM(from, &error);
  
  fl_set_object_color(ob, FL_GREEN, 0);

  msg = -1;
  update_objects();
  
  for (i=0; i<3; i++) {
    if (i % 2) fl_set_object_color(ob, FL_YELLOW, 0);
    else       fl_set_object_color(ob, FL_GREEN, 0);

    msg = -1;
    update_objects();

    if (error) {
      ping_USCM(0, NULL, NULL, NULL, &error);
    }
    else {
      fl_set_object_color(ob, FL_GREEN, 0);
      fl_set_object_label(ob, "DONE");
      break;
    }
  }
  
  if (error) {
    fl_set_object_color(ob, FL_RED, 0);
    fl_set_object_lcolor(ob, FL_YELLOW);
    fl_set_object_label(ob, "FAILED");
  }

  err = error;
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_boot_panel(void) {

  if (!msg) {
    FL_OBJECT *obj = BOOT_INFO;
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_YELLOW, 0);
  }
}

//~============================================================================
//
//      HBB LONG STATUS PANEL
//
//~============================================================================

void create_long_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 140, w2 = 50, w3 = 45, w4 = 195;
  int h1 = 20, h2 = 20;

  w = 5 + w1 + 2 * w2 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "HBB Long Status", 'M');
  x = x + w1;

  obj = add_normal_button(x, y, w2, h1, "READ", 'M');
    fl_set_object_callback(obj, long_status_callback, 0);
  x = x + w2;

  obj = add_normal_button(x, y, w2, h1, "WRITE", 'M');
    fl_set_object_callback(obj, long_status_callback, 1);
  x = x + w2;

  x = x0 + 5;
  y = y + h1;
  obj = add_frame_box(x, y, w3, h2, "Vers:", 'N');
  x = x + w3;
  
  LONG_STA_VER = 
  obj = add_data_box(x, y, w4, h2, "X", 'M');
  x = x + w1 + w2;
}

//~----------------------------------------------------------------------------

void long_status_callback(FL_OBJECT *obj, long data) {

  int R_W = data;

  msg = -1;
  update_objects();
  
  if (R_W) reset_long_status(&err);
  else     get_long_status(long_sta_dat, &err);

  if (err) long_sta_sta = LONG_STA_NOT_KNOWN;
  else     long_sta_sta = LONG_STA_OK;
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_long_status_panel(void) {

  FL_OBJECT *obj = LONG_STA_VER;
  int from;
  char b[80];
  int               good = 0, warning = 1;
  int colors[4]  = {FL_GREEN,   FL_YELLOW};
  int lcolors[4] = {FL_BLACK,   FL_BLACK};
  int color;

  if (long_sta_dat[2] >= 0x2A) from = 21;   // Thank you, HBB
  else                         from = 17;   // Thank you, HBB

  strncpy(&b[0], (char*)&long_sta_dat[from], 20);
  b[20] ='\0';

  if (long_sta_sta == LONG_STA_NOT_KNOWN) sprintf(b, " ");
  if (long_sta_sta == LONG_STA_OK) color = good;
  else                             color = warning;

  fl_set_object_label(obj, b);
  fl_set_object_color (obj,  colors[color], 0);
  fl_set_object_lcolor(obj, lcolors[color]);
}

//~============================================================================
//
//      HBB SHORT STATUS PANEL
//
//~============================================================================

void create_short_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 140, w2 = 50, w3 = 100, w4 = 25, w5 = 115;
  int h1 = 20, h2 = 25, h3 = 30, h4 = 15;
  char *t1[7] = {"Heart Beat",       "Newborn ID", "Logical ID", 
                 "Boot Counter",     "CAN lo level err", 
                 "CAN hi level err", "USCM Status"};

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 6 * h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "HBB Short Status", 'M');
  x = x + w1;
  obj = add_normal_button(x, y, w2, h1, "READ", 'M');
    fl_set_object_callback(obj, short_status_callback, 0);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "WRITE", 'M');
    fl_set_object_callback(obj, short_status_callback, 1);
  y = y + h1;

  x = x0 + 5;
  create_frame_box_vstring(6, x, y, w3, h2, t1, 'N', 'L');
  x = x + w3;
  create_data_box_vstring(
      6, x, y, w4, h2, 'M', &SHORT_STA_DAT[0]);
  x = x + w4;

  create_frame_box_vstring(1, x, y, w5-w4, h3, &t1[6], 'N', 'L');
  create_data_box_vstring(
      1, x+w5-w4, y, w4, h3, 'M', &SHORT_STA_DAT[6]);
  create_data_box_vstring(
      8, x, y+h3, w5, h4, 'S', &SHORT_STA_BIT[0]);
}

//~----------------------------------------------------------------------------

void short_status_callback(FL_OBJECT *obj, long data) {

  int R_W = data;

  msg = 0;
  update_objects();
  msg = -1;
  update_objects();
  
  if (R_W) reset_short_status(&err);
  else     get_short_status(short_sta_dat, &err);

  if (err) short_sta_sta = SHORT_STA_NOT_KNOWN;
  else     short_sta_sta = SHORT_STA_OK;

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_short_status_panel(void) {

  int i;
  int               good = 0, warning = 1;
  int colors[4]  = {FL_GREEN,   FL_YELLOW};
  int lcolors[4] = {FL_BLACK,   FL_BLACK};
  int color;
  char *t2[8] = {"Config Data Error", "ROM Version Error",
                 "Stay in ROM",       "Executing via EEROM",
                 "Config CKS OK",     "EEROM Pgm CKS OK",
                 "Internal Power Up", "External Power UP"};
  
  for (i=0; i<7; i++) {
    FL_OBJECT *obj = SHORT_STA_DAT[i];
    char b[80];
    if (short_sta_sta == SHORT_STA_OK) {
      sprintf(b, "%02hhX", short_sta_dat[i+1]);
      color = good;
    }
    else {
      sprintf(b, " ");
      color = warning;
    }
    fl_set_object_label(obj, b);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  for (i=0; i<8; i++) {
    FL_OBJECT *obj = SHORT_STA_BIT[i];
    fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_color(obj, FL_BLUE, 0);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
    fl_set_object_label(obj, t2[i]);
    if (short_sta_sta == SHORT_STA_OK) {
      if (short_sta_dat[7] & 1<<(7-i)) fl_set_object_lcolor(obj, FL_YELLOW);
      else                             fl_set_object_lcolor(obj, FL_DODGERBLUE);
    }
    else {
      fl_set_object_lcolor(obj, FL_DODGERBLUE);
    }
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_random_echo_panel();
  update_echo_panel();
  update_validate_panel();
  update_WLA_panel();

  update_status_panel();
  update_ports_config_panel();

  update_boot_panel();
  update_long_status_panel();
  update_short_status_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
