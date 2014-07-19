// file CCEB-TASK.c
//
// Program to control CCEB task in USCM
//
// A.Lebedev Mar-2007...

#define BUILD_FOR_CC

#include "ccebdef.h"
#include "uscmsimlib.h"
#include "cceb.h"
#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CCEB - TASK Controller"};       // global variable
char *date  = {"13-Dec-08"};                    // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;
int USCM_ID = 0x192;                               // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char adr_config_file_name[80];                     // global variable
char dat_config_file_name[80];                     // global variable

struct _ADC *adc;

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~---

#ifdef  NA
#undef  NA
#endif
#define NA 16

#ifdef  NS
#undef  NS
#endif
#define NS 13

#ifdef  NF
#undef  NF
#endif
#define NF 12

//~=== Temporary Things ===

SW_BTN *TIMING_BTN[5][4];
SW_BTN *IF_SEND_BTN[1];
_ww    w_if_send;
_ww    w_timing;

//~-- CCEB Crate Model Selector

SW_BTN *CRT_BTN;
_ww ww_crt;

//~-- States

DAT_BOX *STATE_BOX[4];
_r      r_state;

//~-- Check Failures Control

SW_BTN  *FAI_CHK_BTN[4][16];
SW_BTN  *FAI_LIM_BTN[4][16];
DAT_BOX *FAI_CHK_BOX[4][16];
DAT_BOX *FAI_LIM_BOX[4][16];
DAT_BOX *FAI_CNT_BOX[4][16];
DAT_BOX *FAI_NOW_BOX[4][16];
DAT_BOX *FAI_WAS_BOX[4][16];
_ww     w_fai_chk;
_ww     w_fai_lim;
_r      r_fai_chk;
_r      r_fai_lim;
_r      r_fai_cnt;
_r      r_fai_now;
_r      r_fai_was;

//~-- Check Alarms Control

SW_BTN  *ALA_CHK_BTN[4][16];
SW_BTN  *ALA_LIM_BTN[4][16];
DAT_BOX *ALA_CHK_BOX[4][16];
DAT_BOX *ALA_LIM_BOX[4][16];
DAT_BOX *ALA_CNT_BOX[4][16];
DAT_BOX *WRN_NOW_BOX[4][16];
DAT_BOX *WRN_WAS_BOX[4][16];
DAT_BOX *ALA_NOW_BOX[4][16];
DAT_BOX *ALA_WAS_BOX[4][16];
_ww     w_ala_chk;
_ww     w_ala_lim;
_r      r_ala_chk;
_r      r_ala_lim;
_r      r_ala_cnt;
_r      r_wrn_now;
_r      r_wrn_was;
_r      r_ala_now;
_r      r_ala_was;

//~-- Control and Monitoring

bool auto_read_all = FALSE;

SW_BTN  *PHA_TAR_BTN[4];
DAT_BOX *PHA_TAR_BOX[4];
SW_BTN  *PWR_TAR_BTN[4];
DAT_BOX *PWR_TAR_BOX[4];
_ww     w_pha_tar[4];
_r      r_pha_tar[4];
_ww     w_pwr_tar[4];
_r      r_pwr_tar[4];
bool follow[4];

DAT_BOX *STATE_BOX_2[4];

DAT_BOX *PHASE_BOX[4];
DAT_BOX *P_BUS_BOX[4];
DAT_BOX *P_MIN_BOX[4];
DAT_BOX *P_MAX_BOX[4];
DAT_BOX *PWR_MIN_BOX[4];
DAT_BOX *PWR_MAX_BOX[4];
_r      r_phase;
_r      r_p_bus;
_r      r_p_min;
_r      r_p_max;
_r      r_pwr_min;
_r      r_pwr_max;

DAT_BOX *V_BUS_BOX[4];
DAT_BOX *I_BUS_BOX[4];
DAT_BOX *V_RMS_BOX[4];
DAT_BOX *I_RMS_BOX[4];
DAT_BOX *Z_RMS_BOX[4];
DAT_BOX *REJ_T_BOX[4];
DAT_BOX *CASE_T_BOX[4];
DAT_BOX *COLD_T_BOX[4];
DAT_BOX *VCS_T_BOX[4];
_r      r_v_bus;
_r      r_i_bus;
_r      r_v_rms;
_r      r_i_rms;
_r      r_z_rms;
_r      r_rej_t[4];
_r      r_case_t[4];
_r      r_cold_t;
_r      r_vcs_t;

//~-- Data Recording Switch

bool data_updated = FALSE;
SW_BTN *REC_BTN;
_ww ww_rec;

//~---

char *crt_nam[] = {"EM1", "EM2", "QM", "FM"};

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

char *limits[10] = {
 " ",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9"};

char *o_o[2] = {"OFF", "ON"};

char *states_long[NS], *states_short[NS];

char *checks[2] = {"", "@1+"};

char *fai[2] = {"", "F"};

char *wrn[2] = {"", "W"};
char *ala[2] = {"", "A"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_temporary_delay_panel(int x0, int y0);
void update_temporary_delay_panel(void);

void create_temporary_relay_panel(int x0, int y0);
void temporary_relay_callback(FL_OBJECT* obj, long data);

//~--

void create_crate_model_selector_panel(int x0, int y0);
void update_crate_model_selector_panel(void);

void create_states_and_controls_panel(int x0, int y0);
void write_all_controls_callback(FL_OBJECT* obj, long data);
void update_states_and_controls_panel(void);

void create_command_and_monitor_panel(int x0, int y0);
void set_phase_target_callback(FL_OBJECT* obj, long data);
void set_power_target_callback(FL_OBJECT* obj, long data);
void write_target_callback(FL_OBJECT* obj, long data);
void write_all_targets_callback(FL_OBJECT* obj, long data);
void send_command_callback(FL_OBJECT* obj, long data);
void update_command_and_monitor_panel(void);

//~--

void create_read_write_update_panel(int x0, int y0);
void read_all_callback(FL_OBJECT* obj, long data);
void auto_read_all_callback(int tid, void *data);
void write_all_callback(FL_OBJECT* obj, long data);
void update_input_callback(FL_OBJECT* obj, long data);

void create_data_recording_panel(int x0, int y0);
void update_data_recording_panel(void);
void record_ccebtask_data(FILE *file);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 945, HH = 675;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_temporary_delay_panel(795, 505);
  create_temporary_relay_panel(  0, 600);

  create_crate_model_selector_panel(200, 0);

  create_states_and_controls_panel(  0, 45);
  create_command_and_monitor_panel(655, 45);

  create_read_write_update_panel(655, 505);
  create_data_recording_panel(   735, 505);

  create_USCM_control_H3S_panel(655, 590);

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;

  invalidate_ww(first, &w_if_send, 1, TRUE);
  invalidate_ww(first, &w_timing,  5, 0x0000000);

  invalidate_ww(first, &ww_crt, 1, 0);

  invalidate_r(first, &r_state, 4);

  invalidate_ww(first, &w_fai_chk,  4, 0x0000000);
  invalidate_ww(first, &w_fai_lim, 64, 0);
  invalidate_r(first, &r_fai_chk,  4);
  invalidate_r(first, &r_fai_lim, 64);
  invalidate_r(first, &r_fai_cnt, 64);
  invalidate_r(first, &r_fai_now,  4);
  invalidate_r(first, &r_fai_was,  4);

  invalidate_ww(first, &w_ala_chk,  4, 0x0000000);
  invalidate_ww(first, &w_ala_lim, 64, 0);
  invalidate_r(first, &r_ala_chk,  4);
  invalidate_r(first, &r_ala_lim, 64);
  invalidate_r(first, &r_ala_cnt, 64);
  invalidate_r(first, &r_wrn_now,  4);
  invalidate_r(first, &r_wrn_was,  4);
  invalidate_r(first, &r_ala_now,  4);
  invalidate_r(first, &r_ala_was,  4);

  for (i=0; i<4; i++) {
    follow[i] = FALSE;
    invalidate_ww(first, &w_pha_tar[i], 1, 0x0000000);
    invalidate_r(first, &r_pha_tar[i], 1);
    invalidate_ww(first, &w_pwr_tar[i], 1, 0x0000000);
    invalidate_r(first, &r_pwr_tar[i], 1);
  }

  invalidate_r(first, &r_phase, 4);
  invalidate_r(first, &r_p_bus, 4);
  invalidate_r(first, &r_p_min, 4);
  invalidate_r(first, &r_p_max, 4);
  invalidate_r(first, &r_pwr_min, 4);
  invalidate_r(first, &r_pwr_max, 4);
  invalidate_r(first, &r_v_bus, 4);
  invalidate_r(first, &r_i_bus, 4);
  invalidate_r(first, &r_v_rms, 4);
  invalidate_r(first, &r_i_rms, 4);
  invalidate_r(first, &r_z_rms, 4);
  for (i=0; i<4; i++) {
    invalidate_r(first, &r_rej_t[i], 1);
    invalidate_r(first, &r_case_t[i], 1);
  }
  invalidate_r(first, &r_cold_t, 4);
  invalidate_r(first, &r_vcs_t, 4);

  invalidate_ww(first, &ww_rec, 1, 0);

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

  int i;
  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(ww_crt.inp,    4, 1, file);
    fread(w_if_send.inp, 4, 1, file);
    fread(w_timing.inp,  4, 5, file);
    fread(w_fai_chk.inp, 4, 4, file);
    fread(w_fai_lim.inp, 4,64, file);
    fread(w_ala_chk.inp, 4, 4, file);
    fread(w_ala_lim.inp, 4,64, file);
    for (i=0; i<4; i++) fread(w_pha_tar[i].inp, 4, 1, file);
    for (i=0; i<4; i++) fread(w_pwr_tar[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  int i;
  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(ww_crt.inp,    4, 1, file);
    fwrite(w_if_send.inp, 4, 1, file);
    fwrite(w_timing.inp,  4, 5, file);
    fwrite(w_fai_chk.inp, 4, 4, file);
    fwrite(w_fai_lim.inp, 4,64, file);
    fwrite(w_ala_chk.inp, 4, 4, file);
    fwrite(w_ala_lim.inp, 4,64, file);
    for (i=0; i<4; i++) fwrite(w_pha_tar[i].inp, 4, 1, file);
    for (i=0; i<4; i++) fwrite(w_pwr_tar[i].inp, 4, 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            TEMPORARY DELAY PANEL
//
//~============================================================================

#ifdef N
#undef N
#endif
#define N 5

_b if_send = {0x0, 0x0001, 2};

_b timing[N][4] = {
  {{ 0x0, 0x7FFF, 10},
   { 0x0, 0xFFFF, 10},
   { 0x0, 0xFFFF, 10},
   { 0x0, 0xFFFF, 10}},
  {{ 0x1, 0xFFFF, 10},
   { 0x1, 0xFFFF, 10},
   { 0x1, 0xFFFF, 10},
   { 0x1, 0xFFFF, 10}},
  {{ 0x2, 0xFFFF, 10},
   { 0x2, 0xFFFF, 10},
   { 0x2, 0xFFFF, 10},
   { 0x2, 0xFFFF, 10}},
  {{ 0x3, 0xFFFF, 10},
   { 0x3, 0xFFFF, 10},
   { 0x3, 0xFFFF, 10},
   { 0x3, 0xFFFF, 10}},
  {{ 0x4, 0xFFFF, 10},
   { 0x4, 0xFFFF, 10},
   { 0x4, 0xFFFF, 10},
   { 0x4, 0xFFFF, 10}}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_temporary_delay_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 80, w2 = 15, h1 = 25, h2 = 15, h3 = 30;
  char *txt1[] = {"Timers and Delays"};
  char *txt2[N] = {
    "repet period",
    "CCIF on tout",
    "relay tout",
    "after PA on",
    "PA tout"};
  static char *txt3[] = {
    "DO NOT SEND WHEN\nWRITE TARGET",
    "SEND WHEN\nWRITE TARGET"};
  
  w = 5 + w1 + 4 * w2 + 5;
  h = 5 + h1 + N * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  create_frame_box_vstring(1, x, y, w1+4*w2, h1, txt1, 'N', 'C');
  y = y + h1;
  create_frame_box_vstring(N, x, y, w1,      h2, txt2, 'S', 'L');
  x = x + w1;

  for (i=0; i<N; i++) {
    create_btn10_hstring(
      4, x, y, w2, h2, hex, 'N', &w_timing, &timing[i][0], &TIMING_BTN[i][0]);
    y = y + h2;
  }

  x = x0 + 5;
  create_btn_vstring(
    1, x, y, w1+4*w2, h3, txt3, 'N', &w_if_send, &if_send, &IF_SEND_BTN[0]);
}

//~----------------------------------------------------------------------------

void update_temporary_delay_panel(void) {

  int i;

  for (i=0; i<N; i++) {
    show_btn_string(4, NULL, &TIMING_BTN[i][0]);
  }
  show_btn_string(  1, NULL, &IF_SEND_BTN[0]);
}

//~============================================================================
//
//            TEMPORARY RELAY PANEL
//
//~============================================================================

void create_temporary_relay_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 90, w2 = 70, w3 = 65, h1 = 35;

  w = 5 + w1 + 4 * (w2 + w3 + 5) - 5 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Relays:", 'L');
  x = x + w1;
  
  for (i=0; i<4; i++) {
    obj = add_normal_button(x, y, w2, h1, "LOCK&B", 'N');
      fl_set_object_callback(obj, temporary_relay_callback, 2*i+1);
    x = x + w2;
    obj = add_normal_button(x, y, w3, h1, "LOCK&A", 'N');
      fl_set_object_callback(obj, temporary_relay_callback, 2*i+0);
    x = x + w3 + 5;
  }
}

//~----------------------------------------------------------------------------

void temporary_relay_callback(FL_OBJECT* obj, long data) {

  int cc   = data / 2;
  int feed = data % 2;
  int i;
  int8 cha = 2 + 4 * cc;
  int16 dac[16];
  int8 adr = 0 + 4 * cc;
  int16 dat;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0;
  
  if (!err) {
    for (i=0; i<16; i++) dac[i] = 0x8000;
    dac[cha+0] = 0x0000;
    dac[cha+1] = 0x0000;
    write_DAC(dac, &err);
  }

  if (!err) {
    for (i=0; i<16; i++) dac[i] = 0x8000;
    dac[cha+0] = 0x0FFF;
    write_DAC(dac, &err);
  }

  if (!err) {
    delay(0.5);
    dat = LLR_TO_LOCK | (feed ? PAR_TO_FEED_B : PAR_TO_FEED_A);
    write_LVDS_bus(adr, dat, &err);
  }
  
  sleep(1.0);
  
  if (!err) {
    delay(0.1);
    write_LVDS_bus(adr, 0, &err);
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
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
  int w1 = 40, h1 = 15, h2 = 20;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Crate", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, crt_nam, 'M', &ww_crt, &crt, &CRT_BTN);
}

//~----------------------------------------------------------------------------

void update_crate_model_selector_panel(void) {

  switch (*ww_crt.inp) {
    case 0:  adc = &EM1_ADC[0]; break;
    case 1:  adc = &EM2_ADC[0]; break;
    case 2:  adc =  &QM_ADC[0]; break;
    case 3:  adc =  &FM_ADC[0]; break;
    default: adc = &EM1_ADC[0]; break;
  }
}

//~============================================================================
//
//            STATES AND CONTROLS PANEL
//
//~============================================================================

_b state[4] = {
  {0x0, 0x000F, NS},
  {0x1, 0x000F, NS},
  {0x2, 0x000F, NS},
  {0x3, 0x000F, NS}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_states_and_controls_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y;

{//~--- Panel ---

int w1 = 655, h1 = 550;

  fl_add_box(FL_DOWN_BOX, x0, y0, w1, h1, " ");
  x = x0 + 5;
  y = y0 + 5;
}

{//~--- States ---

struct {
  char *txt;
  int   cod;
} cc_state[NS] = {
  {"UNKNOWN",      0},
  {"IDLE",         CC_IDLE},
  {"SWITCH ON 0",  CC_SWITCH_ON_0},
  {"SWITCH ON 1",  CC_SWITCH_ON_1},
  {"SWITCH ON 2",  CC_SWITCH_ON_2},
  {"SWITCH OFF",   CC_SWITCH_OFF},
  {"READY",        CC_READY},
  {"STARTUP 0",    CC_STARTUP_0},
  {"STARTUP 1",    CC_STARTUP_1},
  {"STARTUP 2",    CC_STARTUP_2},
  {"STARTUP 3",    CC_STARTUP_3},
  {"SHUTDOWN",     CC_SHUTDOWN},
  {"RUN",          CC_RUN}};

int w1 = 90, w2 = 135, h1 = 15, h2 = 15;

  for (i=0; i<NS; i++) {
    int j;
    for (j=0; j<NS; j++) if (cc_state[j].cod == i) {
      states_long[i] = cc_state[j].txt;
    }
  }

  add_frame_box(x, y, w1, h1+h2, "States:", 'L');
  x = x + w1;

  for (i=0; i<4; i++) {
    char b[80];
    sprintf(b, "CC #%d", i);
    add_frame_box(x, y, w2, h1, b, 'S');
    create_dat_box_vstring(
      1, x, y+h1, w2, h2, 'N', &r_state, &state[i], &STATE_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1 + h2 +5;
}

{//~--- Failures ---

struct {
  char *txt;
  int32 msk;
} fail[NF] = {
    {"OSCILLATOR",   CC_FAILED_OSCILLATOR           },
    {"POWER_ON_IF",  CC_FAILED_TO_POWER_ON_CCIF     },
    {"ACTI_RELAY",   CC_FAILED_TO_ACTIVATE_RELAYS   },
    {"CHECK_RELAY",  CC_FAILED_TO_CHECK_RELAY_STATUS},
    {"DEACT_RELAY",  CC_FAILED_TO_DEACTIVATE_RELAYS },
    {"SETUP_RELAYS", CC_FAILED_TO_SETUP_RELAYS      },
    {"POWER_ON_PA",  CC_FAILED_TO_POWER_ON_PA       },
    {"START_PA",     CC_FAILED_TO_START_PA          },
    {"CHECK_PA",     CC_FAILED_TO_CHECK_PA          },
    {"STOP_PA",      CC_FAILED_TO_STOP_PA           },
    {"PA_POWER",     CC_FAILED_PA_POWER             },
    {"SET_PHASE",    CC_FAILED_TO_SET_PHASE         }};

_b fai_chk[4][NF];
_b fai_lim[4][NF];

int w1 = 90, w2 = 15, w3 = 15, w4 = 30, w5 = 15, h1 = 15, h2 = 15;
char *txt[NF];

  for (i=0; i<4; i++) {
    int j;
    for (j=0; j<NF; j++) {
      fai_chk[i][j].ind = i;
      fai_chk[i][j].msk = fail[j].msk;
      fai_chk[i][j].n   = 2;
      txt[j]            = fail[j].txt;
      fai_lim[i][j].ind = 16 * i + j;
      fai_lim[i][j].msk = 0x000F;
      fai_lim[i][j].n   = 10;
    }
  }

  x = x0 + 5;
  add_frame_box(x, y, w1, h1+h2, "Failures:", 'L');
  create_frame_box_vstring(NF, x, y+h1+h2, w1, h2, txt, 'S', 'L');
  x = x + w1;

  for (i=0; i<4; i++) {
    char b[80];
    sprintf(b, "CC #%d", i);
    add_frame_box(x, y, 2*w2+2*w3+2*w4+w5, h1, b, 'N');
    add_frame_box(x, y+h1, w2+w3, h2, "W",   'S');
    create_btn_vstring(
      NF, x, y+h1+h2, w2, h2, checks, 'S', &w_fai_chk, &fai_chk[i][0], &FAI_CHK_BTN[i][0]);
    x = x + w2;
    create_btn_vstring(
      NF, x, y+h1+h2, w3, h2, limits, 'N', &w_fai_lim, &fai_lim[i][0],    &FAI_LIM_BTN[i][0]);
    x = x + w3;
    add_frame_box(x, y+h1, w2+w3, h2, "R",   'S');
    create_dat_box_vstring(
      NF, x, y+h1+h2, w2, h2, 'S', &r_fai_chk, &fai_chk[i][0], &FAI_CHK_BOX[i][0]);
    x = x + w2;
    create_dat_box_vstring(
      NF, x, y+h1+h2, w3, h2, 'N', &r_fai_lim, &fai_lim[i][0],    &FAI_LIM_BOX[i][0]);
    x = x + w3;
    add_frame_box(x, y+h1, w4, h2, "Now", 'S');
    create_dat_box_vstring(
      NF, x, y+h1+h2, w4, h2, 'N', &r_fai_now, &fai_chk[i][0], &FAI_NOW_BOX[i][0]);
    x = x + w4;
    add_frame_box(x, y+h1, w4, h2, "Was",   'S');
    create_dat_box_vstring(
      NF, x, y+h1+h2, w4, h2, 'N', &r_fai_was, &fai_chk[i][0], &FAI_WAS_BOX[i][0]);
    x = x + w4;
    add_frame_box(x, y+h1, w5, h2, "C",     'S');
    create_dat_box_vstring(
      NF, x, y+h1+h2, w5, h2, 'N', &r_fai_cnt, &fai_lim[i][0], &FAI_CNT_BOX[i][0]);
    x = x + w5 + 5;
  }
  y = y + h1 + h2 + NF * h2 + 5;
}

{//~--- Alarms ---

struct {
  char *txt;
  int32 msk;
} alrm[NA] = {
    {"REJ_T_TIMER",  CC_ALARM_REJECT_T_TIMEOUT},
    {"REJ_T_LOW",    CC_ALARM_REJECT_T_LOW    },
    {"REJ_T_HIGH",   CC_ALARM_REJECT_T_HIGH   },
    {"CASE_T_TIMER", CC_ALARM_CASE_T_TIMEOUT  },
    {"CASE_T_LOW",   CC_ALARM_CASE_T_LOW      },
    {"CASE_T_HIGH",  CC_ALARM_CASE_T_HIGH     },
    {"P_BUS_LOW",    CC_ALARM_P_BUS_LOW       },
    {"P_BUS_HIGH",   CC_ALARM_P_BUS_HIGH      },
    {"V_RMS_LOW",    CC_ALARM_V_RMS_LOW       },
    {"V_RMS_HIGH",   CC_ALARM_V_RMS_HIGH      },
    {"I_RMS_LOW",    CC_ALARM_I_RMS_LOW       },
    {"I_RMS_HIGH",   CC_ALARM_I_RMS_HIGH      },
    {"Z_RMS_LOW",    CC_ALARM_Z_RMS_LOW       },
    {"Z_RMS_HIGH",   CC_ALARM_Z_RMS_HIGH      },
    {"POWER_LOW",    CC_ALARM_POWER_LOW       },
    {"POWER_HIGH",   CC_ALARM_POWER_HIGH      }};

_b ala_chk[4][NA];
_b ala_lim[4][NA];

int w1 = 90, w2 = 15, w3 = 15, w4 = 15, w5 = 15, h1 = 15, h2 = 15;
char *txt[NA];

  for (i=0; i<4; i++) {
    int j;
    for (j=0; j<NA; j++) {
      ala_chk[i][j].ind = i;
      ala_chk[i][j].msk = alrm[j].msk;
      ala_chk[i][j].n   = 2;
      txt[j]            = alrm[j].txt;
      ala_lim[i][j].ind = 16 * i + j;
      ala_lim[i][j].msk = 0x000F;
      ala_lim[i][j].n   = 10;
    }
  }

  x = x0 + 5;
  add_frame_box(x, y, w1, h1+h2, "Alarms:", 'L');
  create_frame_box_vstring(NA, x, y+h1+h2, w1, h2, txt, 'S', 'L');
  x = x + w1;

  for (i=0; i<4; i++) {
    char b[80];
    sprintf(b, "CC #%d", i);
    add_frame_box(x, y, 2*w2+2*w3+4*w4+w5, h1, b, 'N');
    add_frame_box(x, y+h1, w2+w3, h2, "W",   'S');
    create_btn_vstring(
      NA, x, y+h1+h2, w2, h2, checks, 'N', &w_ala_chk, &ala_chk[i][0], &ALA_CHK_BTN[i][0]);
    x = x + w2;
    create_btn_vstring(
      NA, x, y+h1+h2, w3, h2, limits, 'N', &w_ala_lim, &ala_lim[i][0], &ALA_LIM_BTN[i][0]);
    x = x + w3;
    add_frame_box(x, y+h1, w2+w3, h2, "R",   'S');
    create_dat_box_vstring(
      NA, x, y+h1+h2, w2, h2, 'N', &r_ala_chk, &ala_chk[i][0], &ALA_CHK_BOX[i][0]);
    x = x + w2;
    create_dat_box_vstring(
      NA, x, y+h1+h2, w3, h2, 'N', &r_ala_lim, &ala_lim[i][0], &ALA_LIM_BOX[i][0]);
    x = x + w3;
    add_frame_box(x, y+h1, 2*w4, h2, "Now", 'S');
    create_dat_box_vstring(
      NA, x, y+h1+h2, w4, h2, 'N', &r_wrn_now, &ala_chk[i][0], &WRN_NOW_BOX[i][0]);
    x = x + w4;
    create_dat_box_vstring(
      NA, x, y+h1+h2, w4, h2, 'N', &r_ala_now, &ala_chk[i][0], &ALA_NOW_BOX[i][0]);
    x = x + w4;
    add_frame_box(x, y+h1, 2*w4, h2, "Was",   'S');
    create_dat_box_vstring(
      NA, x, y+h1+h2, w4, h2, 'N', &r_wrn_was, &ala_chk[i][0], &WRN_WAS_BOX[i][0]);
    x = x + w4;
    create_dat_box_vstring(
      NA, x, y+h1+h2, w4, h2, 'N', &r_ala_was, &ala_chk[i][0], &ALA_WAS_BOX[i][0]);
    x = x + w4;
    add_frame_box(x, y+h1, w5,   h2, "C",     'S');
    create_dat_box_vstring(
      NA, x, y+h1+h2, w5, h2, 'N', &r_ala_cnt, &ala_lim[i][0], &ALA_CNT_BOX[i][0]);
    x = x + w5 + 5;
  }
  y = y + h1 + h2 + NA * h2;
}

{//~--- Command ---

int w1 = 90, w2 = 555, h1 = 20;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "", 'M');
    fl_set_object_lalign(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  x = x + w1;
  
  obj = add_normal_button(
    x, y, w2, h1, "WRITE  ALL  CONTROLS", 'N');
    fl_set_object_callback(obj, write_all_controls_callback, 0);
}

}

//~----------------------------------------------------------------------------

void write_all_controls_callback(FL_OBJECT* obj, long data) {

  int i, j;
  _r *st = &r_state;
  _r *r[] = {
    &r_fai_chk, &r_fai_now, &r_fai_was, 
    &r_ala_chk, &r_wrn_now, &r_ala_now, &r_wrn_was, &r_ala_was,
    &r_fai_lim, &r_fai_cnt,
    &r_ala_lim, &r_ala_cnt};
  _ww *w[] = {&w_fai_chk, &w_ala_chk, &w_fai_lim, &w_ala_lim};
  int nr = DIM(r);
  int nw = DIM(w);
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  st->sta = status_NA;
  for (j=0; j<nr; j++) r[j]->sta = status_NA;

  write_CCEB_task_controls(
    w_fai_chk.inp, w_ala_chk.inp, w_fai_lim.inp, w_ala_lim.inp, &err);

  for (i=0; i<nw; i++) {
    if (!err) {
      for (j=0; j<(i<2?4:64); j++) w[i]->wri[j] = w[i]->inp[j];
      w[i]->unk = FALSE;
    }
    else {
      w[i]->unk = TRUE;
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_states_and_controls_panel(void) {

  int i;

  for (i=0; i<4; i++) {
    show_dat_box_string( 1, states_long, NULL, &STATE_BOX[i]);

    show_btn_string(    NF,         NULL, &FAI_CHK_BTN[i][0]);
    show_btn_string(    NF,         NULL, &FAI_LIM_BTN[i][0]);
    show_dat_box_string(NF, checks, NULL, &FAI_CHK_BOX[i][0]);
    show_dat_box_string(NF, limits, NULL, &FAI_LIM_BOX[i][0]);
    show_dat_box_string(NF, fai,    NULL, &FAI_NOW_BOX[i][0]);
    show_dat_box_string(NF, fai,    NULL, &FAI_WAS_BOX[i][0]);
    show_dat_box_string(NF, limits, NULL, &FAI_CNT_BOX[i][0]);

    show_btn_string(    NA,         NULL, &ALA_CHK_BTN[i][0]);
    show_btn_string(    NA,         NULL, &ALA_LIM_BTN[i][0]);
    show_dat_box_string(NA, checks, NULL, &ALA_CHK_BOX[i][0]);
    show_dat_box_string(NA, limits, NULL, &ALA_LIM_BOX[i][0]);
    show_dat_box_string(NA, wrn,    NULL, &WRN_NOW_BOX[i][0]);
    show_dat_box_string(NA, ala,    NULL, &ALA_NOW_BOX[i][0]);
    show_dat_box_string(NA, wrn,    NULL, &WRN_WAS_BOX[i][0]);
    show_dat_box_string(NA, ala,    NULL, &ALA_WAS_BOX[i][0]);
    show_dat_box_string(NA, limits, NULL, &ALA_CNT_BOX[i][0]);
  }
}

//~============================================================================
//
//            COMMAND AND MONITOR PANEL
//
//~============================================================================

_b pha_msk = {0, 0x00FF, 256};
_b pwr_msk = {0, 0x00FF, 256};

_b msk[4] = {
  {0, 0xFFFFFFFF, -1},
  {1, 0xFFFFFFFF, -1},
  {2, 0xFFFFFFFF, -1},
  {3, 0xFFFFFFFF, -1}};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_command_and_monitor_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y;
  int w1 = 45, w2 = 55, h1 = 15, h2 = 15, h3 = 20, h4 = 30;
  int w, h;
  struct {
    char *txt;
    int   cod;
  } command[] = {
    {"ON",    CC_ON},
    {"START", CC_START},
    {"STOP",  CC_STOP},
    {"OFF",   CC_OFF},
    {"HALT",  CC_HALT}};

  struct {
    char *txt;
    int   cod;
  } cc_state[NS] = {
    {"UNKNOWN",      0},
    {"IDLE",         CC_IDLE},
    {"SWITCH\nON 0", CC_SWITCH_ON_0},
    {"SWITCH\nON 1", CC_SWITCH_ON_1},
    {"SWITCH\nON 2", CC_SWITCH_ON_2},
    {"SWITCH\nOFF",  CC_SWITCH_OFF},
    {"READY",        CC_READY},
    {"STARTUP\n0",   CC_STARTUP_0},
    {"STARTUP\n1",   CC_STARTUP_1},
    {"STARTUP\n2",   CC_STARTUP_2},
    {"STARTUP\n3",   CC_STARTUP_3},
    {"SHUT\nDOWN",   CC_SHUTDOWN},
    {"RUN",          CC_RUN}};

  for (i=0; i<NS; i++) {
    int j;
    for (j=0; j<NS; j++) if (cc_state[j].cod == i) {
      states_short[i] = cc_state[j].txt;
    }
  }

  w = 5 + w1 + 4 * (w2 + 5) - 5 + 5;
  h = 5 + h1 + 4 * h2 + h2 + h3 + 
      5 * h1 + h4 + 6 * h1 + 5 + 5 * h1 + 5 + 4 * h1 +5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "", 'N');
  x = x + w1;
  for (i=0; i<4; i++) {
    char b[80];
    sprintf(b, "CC #%d", i);
    add_frame_box(x, y, w2, h1, b, 'N');
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  add_frame_box(x, y, w1, 2*h2, "Phase\nTarget", 'S');
  x = x + w1;
  for (i=0; i<4; i++) {
    create_btn_vstring(
      1, x, y,    w2, h2, NULL, 'N', &w_pha_tar[i], &pha_msk, &PHA_TAR_BTN[i]);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,        y, w2/3, h2, "100");
      fl_set_object_callback(obj, set_phase_target_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w2/3,   y, w2/3, h2, "10");
      fl_set_object_callback(obj, set_phase_target_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+2*w2/3, y, w2/3, h2, "1");
      fl_set_object_callback(obj, set_phase_target_callback, i);
    create_dat_box_hstring(
      1, x, y+h2, w2, h2,       'N', &r_pha_tar[i], &pha_msk, &PHA_TAR_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + 2 * h2;
    
  x = x0 + 5;
  add_frame_box(x, y, w1, 2*h2, "Power\nTarget", 'S');
  x = x + w1;
  for (i=0; i<4; i++) {
    create_btn_vstring(
      1, x, y,    w2, h2, NULL, 'N', &w_pwr_tar[i], &pwr_msk, &PWR_TAR_BTN[i]);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x,        y, w2/3, h2, "100");
      fl_set_object_callback(obj, set_power_target_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+w2/3,   y, w2/3, h2, "10");
      fl_set_object_callback(obj, set_power_target_callback, i);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+2*w2/3, y, w2/3, h2, "1");
      fl_set_object_callback(obj, set_power_target_callback, i);
    create_dat_box_hstring(
      1, x, y+h2, w2, h2,       'N', &r_pwr_tar[i], &pwr_msk, &PWR_TAR_BOX[i]);
    obj = add_normal_button(x, y+2*h2, w2, h2, "WRITE", 'N');
      fl_set_object_callback(obj, write_target_callback, i);
    x = x + w2 + 5;
  }
  y = y + 2 * h2;

  x = x0 + 5;
  add_frame_box(x, y, w1, h2+h3, "", 'S');
  y = y + h2;
  obj = add_normal_button(x+w1, y, w-10-w1, h3, "WRITE  ALL  TARGETS", 'N');
    fl_set_object_callback(obj, write_all_targets_callback, 0);
  y = y + h3;

  x = x0 + 5;
  add_frame_box(x, y, w1, 5*h1, "!", 'H');
  x = x + w1;
  for (i=0; i<4; i++) {
    for (j=0; j<DIM(command); j++) {
      obj = add_normal_button(x, y+j*h1, w2, h1, command[j].txt, 'N');
        fl_set_object_callback(obj, send_command_callback, 100*i+command[j].cod);
    }
    x = x + w2 + 5;
  }
  y = y + 5 * h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h4, "State", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h4, 'N', &r_state, &state[i], &STATE_BOX_2[i]);
    x = x + w2 + 5;
  }
  y = y + h4;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Phase", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_phase, &msk[i], &PHASE_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "P_Bus", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_p_bus, &msk[i], &P_BUS_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "P_Min", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_p_min, &msk[i], &P_MIN_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "P_Max", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_p_max, &msk[i], &P_MAX_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, 15, 2*h1, "P\nW", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

  x = x0 + 5 + 15;
  obj = add_frame_box(x, y, w1-15, h1, "MIN", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1 - 15;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_pwr_min, &msk[i], &PWR_MIN_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5 + 15;
  obj = add_frame_box(x, y, w1-15, h1, "MX", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1 - 15;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_pwr_max, &msk[i], &PWR_MAX_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1 + 5;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "V_Bus", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_v_bus, &msk[i], &V_BUS_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "I_Bus", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_i_bus, &msk[i], &I_BUS_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "V_RMS", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_v_rms, &msk[i], &V_RMS_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "I_RMS", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_i_rms, &msk[i], &I_RMS_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Z_RMS", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_z_rms, &msk[i], &Z_RMS_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1 + 5;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Rej_T", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_rej_t[i], &msk[i], &REJ_T_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Case_T", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_case_t[i], &msk[i], &CASE_T_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Cold_T", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_cold_t, &msk[i], &COLD_T_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;

  x = x0 + 5;
  obj = add_frame_box(x, y, w1, h1, "VCS_T", 'S');
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w1;
  for (i=0; i<4; i++) {
    create_dat_box_hstring(
      1, x, y, w2, h1, 'N', &r_vcs_t, &msk[i], &VCS_T_BOX[i]);
    x = x + w2 + 5;
  }
  y = y + h1;
}

//~----------------------------------------------------------------------------

void set_phase_target_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _ww *w = &w_pha_tar[ind];
  int  d =  atoi(fl_get_object_label(obj));

  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  
  pack32(&w->inp[0], 0xFF, MOD(w->inp[0]+d, 256));
  w->sta = status_NA;

  if (follow[ind]) {
    msg = -1;
    update_objects();
    write_target_callback(NULL, ind);
    msg = 1;
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void set_power_target_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _ww *w = &w_pwr_tar[ind];
  int  d =  atoi(fl_get_object_label(obj));

  msg = 0;

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  
  pack32(&w->inp[0], 0xFF, MOD(w->inp[0]+d, 256));
  w->sta = status_NA;

  if (follow[ind]) {
    msg = -1;
    update_objects();
    write_target_callback(NULL, ind);
    msg = 1;
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void write_target_callback(FL_OBJECT* obj, long data) {

  int8 ind = data;
  int i;
  float AV, AI;
  int32 pwr;
  _r  *r[2] = {&r_pha_tar[ind], &r_pwr_tar[ind]};
  _ww *w[2] = {&w_pha_tar[ind], &w_pwr_tar[ind]};
  
  if (obj) {
    msg = 0;
    if (follow[ind]) {
      set_normal_button(obj, 0);
      follow[ind] = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        follow[ind] = TRUE;
        update_objects();
        return;
      }
    }
    msg = -1;
    update_objects();
  }

  for (i=0; i<2; i++) r[i]->sta = status_NA;

  AV  = adc[V_BUS_0+8*ind].A;
  AI  = adc[I_BUS_0+8*ind].A;
  pwr = (float)*w_pwr_tar[ind].inp / (AV * AI) + 0.5;

  write_CCEB_task_target_value(ind, *w_pha_tar[ind].inp, pwr, &err);

  if (!err) {
    for (i=0; i<2; i++) {
      w[i]->wri[0] = w[i]->inp[0];
      w[i]->unk = FALSE;
    }
  }
  else {
    for (i=0; i<2; i++) {
      w[i]->unk = TRUE;
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void write_all_targets_callback(FL_OBJECT* obj, long data) {

  int i;
  int8  pha[4];
  int32 pwr[4];
  _r  *r[] = {&r_pha_tar[0], &r_pha_tar[1], &r_pha_tar[2], &r_pha_tar[3], 
              &r_pwr_tar[0], &r_pwr_tar[1], &r_pwr_tar[2], &r_pwr_tar[3]};
  _ww *w[] = {&w_pha_tar[0], &w_pha_tar[1], &w_pha_tar[2], &w_pha_tar[3], 
              &w_pwr_tar[0], &w_pwr_tar[1], &w_pwr_tar[2], &w_pwr_tar[3]};
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  for (i=0; i<DIM(r); i++) r[i]->sta = status_NA;

  for (i=0; i<4; i++) {
    float AV = adc[V_BUS_0+8*i].A;
    float AI = adc[I_BUS_0+8*i].A;
    pha[i] = *w_pha_tar[i].inp;
    pwr[i] = (float)*w_pwr_tar[i].inp / (AV * AI) + 0.5;
  }

  write_CCEB_task_target_values(pha, pwr, &err);

  if (!err) {
    for (i=0; i<DIM(r); i++) {
      w[i]->wri[0] = w[i]->inp[0];
      w[i]->unk = FALSE;
    }
  }
  else {
    for (i=0; i<DIM(r); i++) {
      w[i]->unk = TRUE;
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-----------------------------------------------------------------------------

void send_command_callback(FL_OBJECT* obj, long data) {

  int8 cha = data / 100;
  int8 cmd = data % 100;
  _ww *d = &w_timing;
  _ww *s = &w_if_send;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  if (!err) {
    if (s->inp[0] & 0x1) send_CCEB_task_command(cha, cmd, d->inp, &err);
    else                 send_CCEB_task_command(cha, cmd, NULL,   &err);
    if (!err) {
      int i;
      for (i=0; i<5; i++) d->wri[i] = d->inp[i];
      s->wri[0] = s->inp[0];
      d->unk = FALSE;
      s->unk = FALSE;
    }
  }
  
  if (err) {
    d->unk = TRUE;
    s->unk = TRUE;
  }
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_command_and_monitor_panel(void) {

  int i, j;
  char *cold_t_ID[4] = {"X48012", "X47959", "X47967", "X48007"}; 
  char *vcs_t_ID[4]  = {"X47975", "X47926", "X47934", "X47932"}; 
  struct _cernox *cold_t_ref[4], *vcs_t_ref[4];
  int COLD_T[4] = {CC0_COLD_TIP, CC1_COLD_TIP, CC2_COLD_TIP, CC3_COLD_TIP};
  int VCS[4]    = {CC0_VCS,      CC1_VCS,      CC2_VCS,      CC3_VCS};

  for (i=0; i<4; i++) {
    float AV_BUS = adc[V_BUS_0+8*i].A;
    float AI_BUS = adc[I_BUS_0+8*i].A;
    float AV_RMS = adc[V_RMS_0+8*i].A;
    float AI_RMS = adc[I_RMS_0+8*i].A;
    real M0 = adc[COLD_T[i]].A;
    real M1 = adc[VCS[i]].A;
    int16 P;
    char b[80];

    for (j=0; j<DIM(ref_table); j++) {
      if (!strcmp(ref_table[j].nam, cold_t_ID[i])) cold_t_ref[i] = &ref_table[j];
      if (!strcmp(ref_table[j].nam, vcs_t_ID[i]))  vcs_t_ref[i]  = &ref_table[j];
    }

    if (w_pwr_tar[i].inp[0] != w_pwr_tar[i].wri[0]) w_pha_tar[i].inp[0] = 0;

    show_btn_string(    1, "%3d",        &PHA_TAR_BTN[i]);
    show_dat_box_string(1,  NULL, "%3d", &PHA_TAR_BOX[i]);
    show_btn_string(    1, "%3d",        &PWR_TAR_BTN[i]);
    P = (float)*r_pwr_tar[i].rea * (AV_BUS * AI_BUS) + 0.5;
    sprintf(b, "%3d", P);
    show_dat_box_string(1,  NULL,     b, &PWR_TAR_BOX[i]);

    show_dat_box_string(1, states_short, NULL, &STATE_BOX_2[i]);

    show_dat_box_string(1,  NULL, "%3d", &PHASE_BOX[i]);
    sprintf(b, "%5.1f", (float)r_p_bus.rea[i] * (AV_BUS * AI_BUS));
    show_dat_box_string(1,  NULL,     b, &P_BUS_BOX[i]);
    sprintf(b, "%5.1f", (float)r_p_min.rea[i] * (AV_BUS * AI_BUS));
    show_dat_box_string(1,  NULL,     b, &P_MIN_BOX[i]);
    sprintf(b, "%5.1f", (float)r_p_max.rea[i] * (AV_BUS * AI_BUS));
    show_dat_box_string(1,  NULL,     b, &P_MAX_BOX[i]);
    sprintf(b, "%5.1f", (float)r_pwr_min.rea[i] * (AV_BUS * AI_BUS));
    show_dat_box_string(1,  NULL,     b, &PWR_MIN_BOX[i]);
    sprintf(b, "%5.1f", (float)r_pwr_max.rea[i] * (AV_BUS * AI_BUS));
    show_dat_box_string(1,  NULL,     b, &PWR_MAX_BOX[i]);

    sprintf(b, "%5.1f", (float)r_v_bus.rea[i] * AV_BUS);
    show_dat_box_string(1,  NULL,     b, &V_BUS_BOX[i]);
    sprintf(b, "%5.3f", (float)r_i_bus.rea[i] * AI_BUS);
    show_dat_box_string(1,  NULL,     b, &I_BUS_BOX[i]);

    sprintf(b, "%5.1f", (float)r_v_rms.rea[i] * AV_RMS);
    show_dat_box_string(1,  NULL,     b, &V_RMS_BOX[i]);
    sprintf(b, "%5.3f", (float)r_i_rms.rea[i] * AI_RMS);
    show_dat_box_string(1,  NULL,     b, &I_RMS_BOX[i]);
    sprintf(b, "%5.1f", (float)r_z_rms.rea[i] / (float)0x10000 * (AV_RMS / AI_RMS));
    show_dat_box_string(1,  NULL,     b, &Z_RMS_BOX[i]);

    sprintf(b, "%+5.1f", 0.5*(float)(char)(r_rej_t[i].rea[0]&0xFF));
    show_dat_box_string(1,  NULL,     b, &REJ_T_BOX[i]);
    sprintf(b, "%+5.1f", 0.5*(float)(char)(r_case_t[i].rea[0]&0xFF));
    show_dat_box_string(1,  NULL,     b, &CASE_T_BOX[i]);
    sprintf(b, "%7.3Lf", cernox_code_to_T(r_cold_t.rea[i], M0, cold_t_ref[i]));
    show_dat_box_string(1,  NULL,     b, &COLD_T_BOX[i]);
    sprintf(b, "%7.3Lf", cernox_code_to_T(r_vcs_t.rea[i],  M1, vcs_t_ref[i]));
    show_dat_box_string(1,  NULL,     b, &VCS_T_BOX[i]);
  }
}

//~============================================================================
//
//            READ, WRITE, UPDATE PANEL
//
//~============================================================================

void create_read_write_update_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 70, h1 = 20, h2 = 35;
  int size;
  
  w = 5 + w1 + 5;
  h = 5 + 2 * h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  size = FL_NORMAL_SIZE;
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "READ ALL", size);
    fl_set_object_callback(obj, read_all_callback, 0);
  y = y + h1;
  obj = add_normal_button(x, y, w1, h1, "WRITE ALL", size);
    fl_set_object_callback(obj, write_all_callback, 0);
  y = y + h1;
  obj = add_normal_button(x, y, w1, h2, "UPDATE\nINPUT", size);
    fl_set_object_callback(obj, update_input_callback, 0);
}

//~----------------------------------------------------------------------------

void read_all_callback(FL_OBJECT* obj, long data) {

  int i, j, k;
  int8 rea[1000];
  _r *st = &r_state;
  _r *r[] = {
    &r_fai_chk, &r_fai_now, &r_fai_was, 
    &r_ala_chk, &r_wrn_now, &r_ala_now, &r_wrn_was, &r_ala_was,
    &r_fai_lim, &r_fai_cnt,
    &r_ala_lim, &r_ala_cnt,
    &r_pha_tar[0], &r_pha_tar[1], &r_pha_tar[2], &r_pha_tar[3], 
    &r_pwr_tar[0], &r_pwr_tar[1], &r_pwr_tar[2], &r_pwr_tar[3], 
    &r_phase, &r_v_bus, &r_i_bus, &r_p_bus,
    &r_p_min, &r_p_max, &r_pwr_min, &r_pwr_max,
    &r_v_rms, &r_i_rms, &r_z_rms,
    &r_rej_t[0], &r_rej_t[1], &r_rej_t[2], &r_rej_t[3], 
    &r_case_t[0], &r_case_t[1], &r_case_t[2], &r_case_t[3], 
    &r_cold_t, &r_vcs_t};
  
  if (obj) {
    if (auto_read_all) {
      auto_read_all = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_read_all = TRUE;
        fl_add_timeout(0, auto_read_all_callback, NULL);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_read_all) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<4; i++) st->was[i] = st->rea[i];
  for (j=0; j<DIM(r); j++) {
    for (i=0; i<(8<=j&&j<12?64:4); i++) {
      r[j]->was[i] = r[j]->rea[i];
    }
  }
  
  read_CCEB_task_status(rea, &err);

  if (!err) {
    k = 1;
    for (i=0; i<4; i++) {
      st->rea[i] = rea[k++];
      for (j=0; j<8; j++) {
        r[j]->rea[i]  = rea[k++] << 8;
        r[j]->rea[i] |= rea[k++] << 0;
      }
      for (j=8; j<12; j++) {
        int ii;
        int32 *p = &r[j]->rea[16*i];
        for (ii=0; ii<8; ii++) {
          *p++ = (rea[k]   >> 4) & 0x0F;
          *p++ = (rea[k++] >> 0) & 0x0F;
        }
      }

      r_pha_tar[i].rea[0]  = rea[k++];

      r_pwr_tar[i].rea[0]  = (int32)rea[k++] << 24;
      r_pwr_tar[i].rea[0] |= (int32)rea[k++] << 16;
      r_pwr_tar[i].rea[0] |= (int32)rea[k++] <<  8;
      r_pwr_tar[i].rea[0] |= (int32)rea[k++] <<  0;

      r_phase.rea[i]  = rea[k++];
      
      r_v_bus.rea[i]  = (int32)rea[k++] <<  8;
      r_v_bus.rea[i] |= (int32)rea[k++] <<  0;

      r_i_bus.rea[i]  = (int32)rea[k++] <<  8;
      r_i_bus.rea[i] |= (int32)rea[k++] <<  0;

      r_p_bus.rea[i]  = (int32)rea[k++] << 24;
      r_p_bus.rea[i] |= (int32)rea[k++] << 16;
      r_p_bus.rea[i] |= (int32)rea[k++] <<  8;
      r_p_bus.rea[i] |= (int32)rea[k++] <<  0;

      r_p_min.rea[i]  = (int32)rea[k++] << 24;
      r_p_min.rea[i] |= (int32)rea[k++] << 16;
      r_p_min.rea[i] |= (int32)rea[k++] <<  8;
      r_p_min.rea[i] |= (int32)rea[k++] <<  0;

      r_p_max.rea[i]  = (int32)rea[k++] << 24;
      r_p_max.rea[i] |= (int32)rea[k++] << 16;
      r_p_max.rea[i] |= (int32)rea[k++] <<  8;
      r_p_max.rea[i] |= (int32)rea[k++] <<  0;

      r_pwr_min.rea[i]  = (int32)rea[k++] << 24;
      r_pwr_min.rea[i] |= (int32)rea[k++] << 16;
      r_pwr_min.rea[i] |= (int32)rea[k++] <<  8;
      r_pwr_min.rea[i] |= (int32)rea[k++] <<  0;

      r_pwr_max.rea[i]  = (int32)rea[k++] << 24;
      r_pwr_max.rea[i] |= (int32)rea[k++] << 16;
      r_pwr_max.rea[i] |= (int32)rea[k++] <<  8;
      r_pwr_max.rea[i] |= (int32)rea[k++] <<  0;

      r_v_rms.rea[i]  = (int32)rea[k++] <<  8;
      r_v_rms.rea[i] |= (int32)rea[k++] <<  0;

      r_i_rms.rea[i]  = (int32)rea[k++] <<  8;
      r_i_rms.rea[i] |= (int32)rea[k++] <<  0;

      r_z_rms.rea[i]  = (int32)rea[k++] << 24;
      r_z_rms.rea[i] |= (int32)rea[k++] << 16;
      r_z_rms.rea[i] |= (int32)rea[k++] <<  8;
      r_z_rms.rea[i] |= (int32)rea[k++] <<  0;

      r_rej_t[i].rea[0]  = rea[k++];
      r_case_t[i].rea[0] = rea[k++];

      r_cold_t.rea[i]  = (int32)rea[k++] <<  8;
      r_cold_t.rea[i] |= (int32)rea[k++] <<  0;

      r_vcs_t.rea[i]  = (int32)rea[k++] <<  8;
      r_vcs_t.rea[i] |= (int32)rea[k++] <<  0;
    }
    st->sta = status_OK;
    for (j=0; j<DIM(r); j++) r[j]->sta = status_OK;
    for (i=0; i<4; i++) {
      int32 m = r_wrn_now.rea[i] | r_wrn_was.rea[i] | 
                r_ala_now.rea[i] | r_ala_was.rea[i];
      if (m & CC_ALARM_REJECT_T_TIMEOUT) r_rej_t[i].sta = status_KO;
      if (m & CC_ALARM_CASE_T_TIMEOUT)   r_case_t[i].sta = status_KO;
    }
    if (st->unk) for (i=0; i<4; i++) st->was[i] = st->rea[i];
    st->unk = FALSE;
    for (j=0; j<DIM(r); j++) {
      if (r[j]->sta == status_OK && r[j]->unk) {
        r[j]->unk = FALSE;
        if (r[j] == &r_fai_lim || r[j] == &r_fai_cnt ||
            r[j] == &r_ala_lim || r[j] == &r_ala_cnt) {
          for (i=0; i<64; i++) r[j]->was[i] = r[j]->rea[i];
        }
        else {
          bool bingo = FALSE;
          for (i=0; i<4; i++) {
            if (r[j] == &r_pha_tar[i] || r[j] == &r_pwr_tar[i] ||
                r[j] == &r_rej_t[i]   || r[j] == &r_case_t[i]) bingo = TRUE;
          }
          if (bingo) r[j]->was[0] = r[j]->rea[0];
          else for (i=0; i<4; i++) r[j]->was[i] = r[j]->rea[i];
        }
      }
    }
  }
  else {
    st->sta = status_KO;
    for (j=0; j<DIM(r); j++) r[j]->sta = status_KO;
  }

  if (auto_read_all) msg = -3;
  else               msg = 1;

  data_updated = TRUE;
  update_objects();
}

//~----------------------------------------------------------------------------

void auto_read_all_callback(int tid, void *data) {

  int period = 1000;  // in ms

  if (!auto_read_all) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback dt = %f\n", delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  read_all_callback(NULL, 0);
  fl_add_timeout(period, auto_read_all_callback, NULL);
}

//~----------------------------------------------------------------------------

void write_all_callback(FL_OBJECT* obj, long data) {

  if (obj) {
    msg = -2;
    update_objects();
  }

}

//~----------------------------------------------------------------------------

void update_input_callback(FL_OBJECT* obj, long data) {

  if (obj) {
    msg = -2;
    update_objects();
  }
}

//~============================================================================
//
//            DATA RECORDING PANEL
//
//~============================================================================

_b rec = {0, 0x00000001, 2};
  
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_data_recording_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 50, h1 = 40, h2 = 35;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Writing\nthe file", 'N');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, o_o, 'H', &ww_rec, &rec, &REC_BTN);
}

//~----------------------------------------------------------------------------

void update_data_recording_panel(void) {

  SW_BTN *B = REC_BTN;
  FILE *file;
  char *log_file_name = {"ccebtask-logfile.txt"};

  if (*ww_rec.inp) {
    fl_set_object_color(B->obj, FL_RED, 0);
    fl_set_object_lcolor(B->obj, FL_YELLOW);
  }
  else {
    fl_set_object_color(B->obj, FL_WHEAT, 0);
    fl_set_object_lcolor(B->obj, FL_BLACK);
  }

  if (*ww_rec.inp) {
    if (P > 0) record_ccebtask_data(stdout);
    file = fopen(log_file_name, "a+");
    if (file) {
      setbuf(file, NULL);
      record_ccebtask_data(file);
      fclose(file);
    }
    else {
      printf("%s writing failed...%c\n", log_file_name, 7);
    }
  }

  data_updated = FALSE;
}

//~----------------------------------------------------------------------------

void record_ccebtask_data(FILE *file) {

  int i;

  if (!data_updated) return;
  
  fprintf(file, "%s", timestamp(1));

  if (err) {
    fprintf(file, " *** err = 0x%04X ***\n", err);
    return;
  }

  for (i=0; i<4; i++) {
    fprintf(file, ": %2d ", r_state.rea[i]);

    fprintf(file, "%04X ", r_fai_chk.rea[i]);
    fprintf(file, "%04X ", r_fai_now.rea[i]);
    fprintf(file, "%04X ", r_fai_was.rea[i]);

    fprintf(file, "%04X ", r_ala_chk.rea[i]);
    fprintf(file, "%04X ", r_wrn_now.rea[i]);
    fprintf(file, "%04X ", r_ala_now.rea[i]);
    fprintf(file, "%04X ", r_wrn_was.rea[i]);
    fprintf(file, "%04X ", r_ala_was.rea[i]);

    fprintf(file, "%s ", fl_get_object_label(PWR_TAR_BOX[i]->obj));

    fprintf(file, "%s ", fl_get_object_label(PHASE_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(P_BUS_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(P_MAX_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(PWR_MIN_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(PWR_MAX_BOX[i]->obj));

    fprintf(file, "%s ", fl_get_object_label(V_BUS_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(I_BUS_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(V_RMS_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(I_RMS_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(Z_RMS_BOX[i]->obj));

    fprintf(file, "%s ", fl_get_object_label(REJ_T_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(CASE_T_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(COLD_T_BOX[i]->obj));
    fprintf(file, "%s ", fl_get_object_label(VCS_T_BOX[i]->obj));
  }

  fprintf(file, "\n");
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_temporary_delay_panel();

  update_crate_model_selector_panel();
  update_states_and_controls_panel();
  update_command_and_monitor_panel();

  update_data_recording_panel();
  
  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
