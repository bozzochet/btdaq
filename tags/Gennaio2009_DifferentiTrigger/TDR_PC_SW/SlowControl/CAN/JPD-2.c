// file JPD-2.c
//
// JPD-2 Controller
//
// A.Lebedev Aug-2007...
//

#include "template.h"
#include "jpddef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"JPD-2 Controller"};        // global variable
char *date  = {"13-Dec-08"};               // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x44;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char adr_config_file_name[80];                     // global variable
char dat_config_file_name[80];                     // global variable

int P = 0;                                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

FL_OBJECT *VOL1, *VOL2;
int hide_voltage_code = 0;

int JPD_mod;

FL_OBJECT *CUR1, *CUR2;
int hide_current_code = 0;

FL_OBJECT *BOOT_MODE;
int boot_mode;

FL_OBJECT *PERIOD;
int timer_period = 2;

FL_OBJECT *MONITOR;
int monitoring = 0;

FL_OBJECT *WRITING_LOG_FILE;
static FILE *logfile = NULL;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct {
  struct {
    int   unk;
    int   sta;
    int16 dat;
    int16 acc;
    int16 inv;
  } rea;
  struct {
    int   sta;
    int16 inp;
    int16 dat;
    int16 inv;
  } wri;
} LVDS[256];

struct {
  int   unk;
  int   sta;
  int16 dat;
  float val;
} ADC[32];

FL_OBJECT *DCDC_ctr_BOX[32];
FL_OBJECT *DCDC_ctr_WRI[32];
FL_OBJECT *DCDC_ctr_INP[32];
FL_OBJECT *DCDC_ctr_SND[32];
FL_OBJECT *DCDC_mon_BOX[32];
FL_OBJECT *DCDC_acc_BOX[32];
FL_OBJECT *DCDC_dir_BOX[32];
FL_OBJECT *DCDC_vol_cod_BOX[32];
FL_OBJECT *DCDC_vol_val_BOX1[32];
FL_OBJECT *DCDC_vol_val_BOX2[32];
FL_OBJECT *DCDC_cur_cod_BOX[32];
FL_OBJECT *DCDC_cur_val_BOX1[32];
FL_OBJECT *DCDC_cur_val_BOX2[32];

_DCDC *DCDC;

_Dallas *Dallas;

struct {
  struct {
    int   unk;
    int   sta;
    int16 dat;
    float val;
  } DS1820[256];
} bus[2];

FL_OBJECT *Dallas_tmp_BOX[12][2];

FL_OBJECT *USCM_TMP1, *USCM_TMP2;
FL_OBJECT *USCM_tmp_BOX;

int Dallas_bus;

int64 USCM_Dallas_ID;

int   USCM_tmp_unk;
int   USCM_tmp_sta;
float USCM_tmp_val;

#define N_MOD 5
struct {
  char *nam;
  _DCDC *DCDC;
  _Dallas *Dallas;
} JPD_mods[N_MOD] = {

  {"EM",   EM_DCDC, EM_Dallas },
  {"EM2",  FM_DCDC, EM2_Dallas},
  {"QM",   QM_DCDC, QM_Dallas },
  {"FM01", FM_DCDC, FM01_Dallas},
  {"FM02", FM_DCDC, FM02_Dallas}
};  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void create_JPD_mod_panel(int x0, int y0);
void create_DCDC_panel(int x0, int y0);
void create_monitor_panel(int x0, int y0);
void create_temperature_panel(int x0, int y0);

void setup_JPD_data_structures(void);
void read_all_data();
void update_objects(void);
void TIMER_callback(FL_OBJECT *obj, long data);
void command_write_LVDS_bus(int8 adr, int16 dat, int16 *err);

char *mytime(void);

//~============================================================================

void create_main_form(void) {

  FL_OBJECT *obj;
  int WW = 690, HH = 645;

  bgn_form(&MAIN, 0, 0, WW, HH);
  
  create_JPD_mod_panel(      200,   0);
  create_DCDC_panel(           0,  45);
  create_USCM_control_H2_panel(0, 565);
  create_monitor_panel(      345, 565);
  create_temperature_panel(  505,  45);

#ifdef LOGFILE
  WRITING_LOG_FILE =
  obj = fl_add_button(FL_PUSH_BUTTON,450,55+18*32+5,525,30,"");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_button(obj,0);
    fl_set_object_callback(obj, swtch_callback, 0);
#endif

  obj = fl_add_timer(FL_HIDDEN_TIMER, 0, 0, 0, 0,"");
    fl_set_timer(obj, (float)timer_period);
    fl_set_object_callback(obj, TIMER_callback, 0);

  end_form(MAIN);
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void setup_JPD_data_structures(void) {

  int i, j;

  boot_mode = 1;
  
  DCDC   = JPD_mods[JPD_mod].DCDC;
  Dallas = JPD_mods[JPD_mod].Dallas;

  for (i=0; i<256; i++) {
    LVDS[i].rea.unk = 1;
    LVDS[i].rea.sta = status_NA;
    LVDS[i].rea.acc = 0x0000;
    LVDS[i].rea.inv = 0x0000;
    LVDS[i].wri.sta = status_NA;
    LVDS[i].wri.inp = 0x0000;
    LVDS[i].wri.inv = 0x0000;
  }

//printf("setup_JPD_data_structures:\n");
  for (i=0; i<32; i++) {
    int adr = DCDC[i].adr;
    if (adr >= 0) {
      if (DCDC[i].ctr.inv.wri) LVDS[adr].wri.inv |= DCDC[i].ctr.msk;
      if (DCDC[i].ctr.inv.rea) LVDS[adr].rea.inv |= DCDC[i].ctr.msk;
      if (DCDC[i].mon.inv)     LVDS[adr].rea.inv |= DCDC[i].mon.msk;
      if (DCDC[i].dir.inv)     LVDS[adr].rea.inv |= DCDC[i].dir.msk;
    }
//  printf("LVDS[%2d].wri.inv = %04X, ", adr, LVDS[adr].wri.inv);
//  printf("LVDS[%2d].rea.inv = %04X\n", adr, LVDS[adr].rea.inv);
  }

  for (i=0; i<32; i++) {
    ADC[i].unk = 1;
    ADC[i].sta = status_NA;
  }

  Dallas_bus = 0;

  USCM_tmp_unk = 1;
  USCM_tmp_sta = status_NA;

  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      bus[j].DS1820[i].unk = 1;
      bus[j].DS1820[i].sta = status_NA;
    }
  }
}

//~============================================================================

void JPD_txt_callback(FL_OBJECT *obj, long data);
void JPD_mod_callback(FL_OBJECT *obj, long data);
void update_JPD_mod_panel(void);
void update_JPD_mod_config_file(void);

FL_OBJECT *JPD_MOD;

char *JPD_mod_config_file_name = {"JPD-2-mod.configfile"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPD_mod_panel(int x0, int y0) {

  FL_OBJECT *obj;
  FILE *file;
  int x, y;
  
  JPD_mod = 1;
  file = fopen(JPD_mod_config_file_name, "r");
  if (file != NULL) {
    fread(&JPD_mod, sizeof(JPD_mod), 1, file);
    fclose(file);
  }
  setup_JPD_data_structures();

  x = x0;
  y = y0;
  
  obj = fl_add_box(FL_DOWN_BOX,x,y,50,45,"");
  
  x = x0 + 5;
  y = y0;

  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,40,20,"JPD");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj, JPD_txt_callback, 0);

  x = x0 + 5;
  y = y0 + 20;
  
  JPD_MOD =
  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,40,20,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
    fl_set_object_callback(obj, JPD_mod_callback, 0);
}

//~-------------------------------------------------------------------------------

void JPD_txt_callback(FL_OBJECT *obj, long data) {

  JPD_mod = 0;
  
  setup_JPD_data_structures();
  update_JPD_mod_config_file();
  update_objects();
}

//~-------------------------------------------------------------------------------

void JPD_mod_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  JPD_mod = MOD(JPD_mod+1, N_MOD);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) JPD_mod = MOD(JPD_mod-1, N_MOD);

  setup_JPD_data_structures();
  update_JPD_mod_config_file();
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_JPD_mod_panel(void) {

  FL_OBJECT *obj = JPD_MOD;

  if (!obj) return;

  if (JPD_mod < N_MOD) fl_set_object_label(obj, JPD_mods[JPD_mod].nam);
  else                 fl_set_object_label(obj, "?");

  if (Dallas_bus < 0) {
    fl_set_object_color (obj, FL_RED, 0);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }
  else {
    fl_set_object_color (obj, FL_WHEAT, 0);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
}

//~-------------------------------------------------------------------------------

void update_JPD_mod_config_file(void) {

  FILE *file;

  file = fopen(JPD_mod_config_file_name, "w");
  if (file != NULL) {
    fwrite(&JPD_mod, sizeof(JPD_mod), 1, file);
    fclose(file);
  }
}

//~============================================================================

void PERIOD_callback(FL_OBJECT *obj, long data);
void MONITOR_callback(FL_OBJECT *obj, long data);
void update_monitor_panel();

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_monitor_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w;
  int wa = 45, w1 = 25, w2 = 80, h = 40;

  w = 10 + wa + w1 + w2;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+h, "");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, wa, h, "Period\nsec", FL_NORMAL_SIZE);

  x = x + wa;
  PERIOD = 
  obj = add_switch_button(x, y, w1, h, "?", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, PERIOD_callback, 0);

  x = x + w1;
  MONITOR =
  obj = add_normal_button(x, y, w2, h, "MONITOR", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, MONITOR_callback, 0);
}

//~----------------------------------------------------------------------------

void PERIOD_callback(FL_OBJECT *obj, long data) {

#if 0
  int t = timer_period - 1;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  t = MOD(t+1, 9);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) t = MOD(t-1, 9);
  timer_period = t + 1;
#else
  int t = timer_period;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  t = MOD(t+1, 10);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) t = MOD(t-1, 10);
  timer_period = t;
#endif


  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void MONITOR_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (monitoring) monitoring = 0;
    else            read_all_data();
  }
    
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) monitoring = 1 - monitoring;

  set_normal_button(obj, monitoring);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void update_monitor_panel(void) {

  char b[80];
  
  sprintf(b, "%d", timer_period);
  fl_set_object_label(PERIOD, b);
}

//~============================================================================

void update_temperature_panel(void);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_temperature_panel(int x0, int y0) {

  int i;
  int x, y, w;
  int wa = 50, wb = 25, w1 = 50, h = 15;
  int size;
  
  size = FL_SMALL_SIZE;
  
  w = 10 + wa + wb + 2 * w1;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+2*h+12*h, "");
  
  x = x0 + 5;
  y = y0 + 5;
  w = wa + wb;
  add_frame_box(x, y, w,  2*h, "Board Area", size);
  x = x + w;
  add_frame_box(x, y, w1, 2*h, "Temp",       size);
  x = x + w1;
  add_frame_box(x, y, w1, 2*h, "Temp",       size);
  
  y = y0 + 5 + 2 * h;
  for (i=0; i<12; i++) {
    size = FL_SMALL_SIZE;
    x = x0 + 5;
    if (i != 6 && i != 7 && i % 2 == 0)
      add_frame_box(x, y, wa,    2*h, Dallas[i].JPD_board,  size);
    if (i == 6) {
      USCM_TMP1 = fl_bgn_group();
      add_frame_box(x, y, wa+wb, 2*h, "USCM",               size);
      fl_end_group();
    }
    if (i == 6) USCM_TMP2 = fl_bgn_group();
    if (i == 7)             fl_addto_group(USCM_TMP2);
    if (i == 6 || i == 7)
      add_frame_box(x, y, wa+wb,   h, Dallas[i].JPD_board,  size);
    if (i == 6 || i == 7)   fl_end_group();
    x = x + wa;
    if (i != 6 && i != 7)
      add_frame_box(x, y, wb,      h, Dallas[i].area,       size);
    x = x + wb;
    size = FL_NORMAL_SIZE;
    if (i == 6) {
      fl_addto_group(USCM_TMP1);
      USCM_tmp_BOX       = add_data_box(x, y, 2*w1, 2*h, "t", FL_HUGE_SIZE);
      fl_end_group();
    }
    if (i == 6 || i == 7) fl_addto_group(USCM_TMP2);
    Dallas_tmp_BOX[i][0] = add_data_box(x, y,   w1,   h, "t", size);
    x = x + w1;
    Dallas_tmp_BOX[i][1] = add_data_box(x, y,   w1,   h, "t", size);
    x = x + w1;
    if (i == 6 || i == 7) fl_end_group();
    y = y + h;
  }
}

//~----------------------------------------------------------------------------

void update_temperature_panel(void) {

  FL_OBJECT *obj;
  int i, j;
  char b[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;

  if (Dallas_bus < 0) {
    fl_hide_object(USCM_TMP2);
    fl_show_object(USCM_TMP1);
  }
  else {
    fl_hide_object(USCM_TMP1);
    fl_show_object(USCM_TMP2);
  }

  if (USCM_tmp_unk) sprintf(b, " ");
  else              sprintf(b, "%+5.1f", USCM_tmp_val);
  
  switch (USCM_tmp_sta) {
    case status_OK: color = good;    break;
    case status_KO: color = error;   break;
    case status_NA: color = warning; break;
    default:        color = warning; break;
  }
  if ((obj = USCM_tmp_BOX)) {
    fl_set_object_label(obj, b);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      if ((i == 6 && j == 1) || (i == 7 && j == 0)) {
        sprintf(b, "-");
        color = good;
      }
      else {
        if (bus[j].DS1820[i].unk) sprintf(b, " ");
        else                      sprintf(b, "%+6.2f", bus[j].DS1820[i].val);
        switch (bus[j].DS1820[i].sta) {
          case status_OK: color = good;    break;
          case status_KO: color = error;   break;
          case status_NA: color = warning; break;
          default:        color = alarm;   break;
        }
      }
      if ((obj = Dallas_tmp_BOX[i][j])) {
        fl_set_object_label(obj, b);
        fl_set_object_color (obj,  colors[color], 0);
        fl_set_object_lcolor(obj, lcolors[color]);
      }
    }
  }
}

//~============================================================================

void reset_accumulated_status_callback(FL_OBJECT *obj, long data);
void hide_code_callback(FL_OBJECT *obj, long data);
void create_JPMD_panel(int x0, int y0, int cha);
void create_JPIF_panel(int x0, int y0, int cha);
void create_JPJT_panel(int x0, int y0, int cha);
void create_JPSC_panel(int x0, int y0, int cha);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_DCDC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10, w4 = 30, w5 = 40;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size = FL_SMALL_SIZE;

  w = 10 + wa + wb + wc + wd + 2 * w1 + w3 + w1 + 4 * w2 + w3 + w4 + w5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+2*h+32*h, "");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, wa, 2*h, "", size);
  x = x + wa;
  w = wb + wc + wd;
  add_frame_box(x, y,    w, 2*h, "", size);
  x = x + w;
  w = 2 * w1 + w3 + w1;
  add_frame_box(x, y, w, h, "Status", size);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w, 2*h, "");
    fl_set_object_callback(obj, reset_accumulated_status_callback, 0);
  add_frame_box(x,         y+h, w1,    h, "Ctr", size);
  add_frame_box(x+w1,      y+h, w1+w3, h, "Mon", size);
  add_frame_box(x+2*w1+w3, y+h, w1,    h, "Dir", size);
  x = x + w;
  VOL1 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, h, "Voltage", size);
    add_frame_box(x,    y+h,   w2, h, "Code", size);
    add_frame_box(x+w2, y+h,   w2, h, "Value", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, h, "");
      fl_set_object_callback(obj, hide_code_callback, 0);
  fl_end_group();
  VOL2 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, 2*h, "Voltage", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, 2*h, "");
      fl_set_object_callback(obj, hide_code_callback, 0);
  fl_end_group();
  x = x + 2 * w2;
  CUR1 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, h, "Current", size);
    add_frame_box(x,    y+h,   w2, h, "Code", size);
    add_frame_box(x+w2, y+h,   w2, h, "Value", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, h, "");
      fl_set_object_callback(obj, hide_code_callback, 1);
  fl_end_group();
  CUR2 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, 2*h, "Current", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, 2*h, "");
      fl_set_object_callback(obj, hide_code_callback, 1);
  fl_end_group();
  x = x + 2 * w2;
  w = w3 + w4 + w5;
  add_frame_box(x, y, w, 2*h, "Command", size);

  x = x0 + 5;
  y = y0 + 5 + 2 * h;
  create_JPMD_panel(x,      y,  0);
  create_JPMD_panel(x,  y+6*h,  6);
  create_JPIF_panel(x, y+12*h, 12);
  create_JPJT_panel(x, y+20*h, 20);
  create_JPSC_panel(x, y+26*h, 26);
}

//~----------------------------------------------------------------------------

void reset_accumulated_status_callback(FL_OBJECT *obj, long data) {

  int i;
  
  for (i=0; i<256; i++) {
    LVDS[i].rea.acc = 0x0000;
  }

  update_objects();
}

//~----------------------------------------------------------------------------

void hide_code_callback(FL_OBJECT *obj, long data) {

  if (data) hide_current_code = 1 - hide_current_code;
  else      hide_voltage_code = 1 - hide_voltage_code;

  update_objects();
}

//~============================================================================

void SND_callback(FL_OBJECT *obj, long data);
void JPMD_ON_OFF_callback(FL_OBJECT *obj, long data);
void update_JPMD_panel(int cha);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPMD_panel(int x0, int y0, int cha) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10, w4 = 30, w5 = 40;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;

  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 6*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + wa;
    if (i % 3 == 0) add_frame_box(x, y, wb, 3*h, DCDC[j].J_JT_board, size);
    x = x + wb;
    if (i % 3 == 0) add_frame_box(x, y, wc, 3*h, DCDC[j].feed,       size);
    x = x + wc;
                    add_frame_box(x, y, wd,   h, DCDC[j].nominal,    size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + w;
    if (i % 3 == 0) DCDC_ctr_BOX[j] = add_data_box(x, y, w1,   h, "0", size);
    if (i % 3 == 1) DCDC_ctr_BOX[j] = add_data_box(x, y, w1, 2*h, "0", size);
    x = x + w1;
    if (i % 3 == 0) DCDC_mon_BOX[j] = add_data_box(x, y, w1,   h, "1", size);
    if (i % 3 == 1) DCDC_mon_BOX[j] = add_data_box(x, y, w1, 2*h, "1", size);
    x = x + w1;
    if (i % 3 == 0) DCDC_acc_BOX[j] = add_data_box(x, y, w3,   h, "", size);
    if (i % 3 == 1) DCDC_acc_BOX[j] = add_data_box(x, y, w3, 2*h, "", size);
    x = x + w3;
    if (i % 3 == 0) DCDC_dir_BOX[j] = add_data_box(x, y, w1,   h, "", size);
    if (i % 3 == 1) DCDC_dir_BOX[j] = add_data_box(x, y, w1, 2*h, "", size);
    x = x + w1;
    fl_addto_group(VOL1);
      DCDC_vol_cod_BOX[j]  = add_data_box(x,      y, w2,   h, "1", size);
      DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y, w2,   h, "1", size);
    fl_end_group();
    fl_addto_group(CUR1);
      DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y, w2,   h, "2", size);
      DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y, w2,   h, "2", size);
    fl_end_group();
    fl_addto_group(VOL2);
      DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR2);
      DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, h, "3", size);
    fl_end_group();
    x = x + 4 * w2;
    if (i % 3 == 0) DCDC_ctr_WRI[j] = add_data_box(x, y, w3,   h, "", size);
    if (i % 3 == 1) DCDC_ctr_WRI[j] = add_data_box(x, y, w3, 2*h, "", size);
    x = x + w3;
    if (i % 3 == 0) {
      DCDC_ctr_INP[j] =
      obj = add_switch_button(x, y, w4,   h, "OFF", size);
        fl_set_object_callback(obj, JPMD_ON_OFF_callback, j);
    }
    if (i % 3 == 1) {
      DCDC_ctr_INP[j] =
      obj = add_switch_button(x, y, w4, 2*h, "OFF", size);
        fl_set_object_callback(obj, JPMD_ON_OFF_callback, j);
    }
    x = x + w4;
    if (i == 0) {
      DCDC_ctr_SND[j] =
      obj = add_normal_button(x, y, w5, 6*h, "SND", size);
        fl_set_object_callback(obj, SND_callback, j);
    }
    y = y + h;
  }
}

//~----------------------------------------------------------------------------

void JPMD_ON_OFF_callback(FL_OBJECT *obj, long data) {

  int cha, adr, bit;

  cha = data;
  adr = DCDC[cha].adr;
  LVDS[adr].wri.inp = LVDS[adr].wri.inp ^ DCDC[cha].ctr.msk;
  bit = LVDS[adr].wri.inp & DCDC[cha].ctr.msk;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    cha = cha / 3 * 3 + 1 - cha % 3;
    if (bit) LVDS[adr].wri.inp = LVDS[adr].wri.inp |  DCDC[cha].ctr.msk;
    else     LVDS[adr].wri.inp = LVDS[adr].wri.inp & ~DCDC[cha].ctr.msk;
  }

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void SND_callback(FL_OBJECT *obj, long data) {

  int cha = data;
  int adr = DCDC[cha].adr;

  LVDS[adr].wri.dat = LVDS[adr].wri.inp;
  LVDS[adr].wri.sta = status_NA;
  LVDS[adr].rea.sta = status_NA;

  msg = -1;
  update_objects();

//printf("SND_callback: %d %04X %04X %04X\n",
//adr, LVDS[adr].wri.dat, LVDS[adr].wri.inv, LVDS[adr].wri.dat ^ LVDS[adr].wri.inv);

  command_write_LVDS_bus(adr, LVDS[adr].wri.dat ^ LVDS[adr].wri.inv, &err);
  if (!err) LVDS[adr].wri.sta = status_OK;
  else      LVDS[adr].wri.sta = status_KO;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_DCDC_status(int cha, int mode) {

  FL_OBJECT *obj = NULL;
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color = alarm;
  int adr;
  int ctr_rea = -1, ctr_wri = -1, mon_rea = -1;

  if      (mode == 0) obj = DCDC_ctr_BOX[cha];
  else if (mode == 1) obj = DCDC_mon_BOX[cha];
  else if (mode == 2) obj = DCDC_acc_BOX[cha];
  else if (mode == 3) obj = DCDC_dir_BOX[cha];

  if (!obj) return;

  adr = DCDC[cha].adr;

  if (mode == 0 || mode == 1 || mode == 3) {
    if (adr < 0                              || 
       (mode == 0 && DCDC[cha].ctr.msk == 0) ||
       (mode == 1 && DCDC[cha].mon.msk == 0) ||
       (mode == 3 && DCDC[cha].dir.msk == 0)) {
      fl_set_object_label(obj, "-");
      color = good;
    }
    else {
      if (LVDS[adr].rea.unk) {
        fl_set_object_label(obj, " ");
      }
      else {
        if (DCDC[cha].ctr.msk) {
          ctr_rea = unpack16(LVDS[adr].rea.dat, DCDC[cha].ctr.msk);
          ctr_wri = unpack16(LVDS[adr].wri.dat, DCDC[cha].ctr.msk);
        }
        else {
          ctr_rea = 1;
          ctr_wri = 1;
        }
        if (mode == 3) mon_rea = unpack16(LVDS[adr].rea.dat, DCDC[cha].dir.msk);
        else           mon_rea = unpack16(LVDS[adr].rea.dat, DCDC[cha].mon.msk);
        if (mode == 0) {
          if (ctr_rea) fl_set_object_label(obj, "ON");
          else         fl_set_object_label(obj, "OFF");
        }
        else {
          if (mon_rea) fl_set_object_label(obj, "PG");
          else         fl_set_object_label(obj, "FAI");
        }
      }
      if (LVDS[adr].rea.sta == status_NA) color = warning;
      if (LVDS[adr].rea.sta == status_KO) color = error;
      if (LVDS[adr].rea.sta == status_OK) {
        color = good;
        if (DCDC[cha].ctr.msk && 
            LVDS[adr].wri.sta == status_OK && 
            ctr_rea != ctr_wri) color = warning;
        if (ctr_rea != mon_rea) color = warning;
      }
    }
  }
  
  if (mode == 2) {
    fl_set_object_label(obj, "");
    mon_rea = unpack16(LVDS[adr].rea.acc, DCDC[cha].mon.msk);
    if (mon_rea) color = warning;
    else         color = good;
  }
  
  fl_set_object_color (obj,  colors[color], 0);
  fl_set_object_lcolor(obj, lcolors[color]);
}

//~----------------------------------------------------------------------------

void update_DCDC_voltage(int cha) {

  FL_OBJECT *obj;
  char cod[80], val[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;
  int ind;
    
  ind = DCDC[cha].vol.ind;
  
  if (ind < 0) {
    sprintf(cod, "-");
    sprintf(val, "-");
    color = good;
  }
  else {
    if (ADC[ind].unk) sprintf(cod, " ");
    else              sprintf(cod, "%4d", ADC[ind].dat);
    if (ADC[ind].unk) sprintf(val, " ");
    else              sprintf(val, "%5.3f", ADC[ind].val);
    switch (ADC[ind].sta) {
      case status_OK: color = good;    break;
      case status_KO: color = error;   break;
      case status_NA: color = warning; break;
      default:        color = alarm;   break;
    }
  }

  if ((obj = DCDC_vol_cod_BOX[cha])) {
    fl_set_object_label(obj, cod);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_vol_val_BOX1[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_vol_val_BOX2[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }
}

//~-----------------------------------------------------------------------------

void update_DCDC_current(int cha) {

  FL_OBJECT *obj;
  char cod[80], val[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;
  int ind;
    
  ind = DCDC[cha].cur.ind;
  
  if (ind < 0) {
    sprintf(cod, "-");
    sprintf(val, "-");
    color = good;
  }
  else {
    if (ADC[ind].unk) sprintf(cod, " ");
    else              sprintf(cod, "%4d", ADC[ind].dat);
    if (ADC[ind].unk) sprintf(val, " ");
    else              sprintf(val, "%5.3f", ADC[ind].val);
    switch (ADC[ind].sta) {
      case status_OK: color = good;    break;
      case status_KO: color = error;   break;
      case status_NA: color = warning; break;
      default:        color = alarm; break;
    }
  }

  if ((obj = DCDC_cur_cod_BOX[cha])) {
    fl_set_object_label(obj, cod);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_cur_val_BOX1[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_cur_val_BOX2[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }
}

//~-----------------------------------------------------------------------------

void update_DCDC_button(int cha) {

  int adr = DCDC[cha].adr;
  FL_OBJECT *obj;
  
  obj = DCDC_ctr_INP[cha];

  if (!obj) return;

  if (LVDS[adr].wri.inp & DCDC[cha].ctr.msk) {
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_YELLOW);
    fl_set_object_label(obj, "ON");
  }
  else {
    fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_label(obj, "OFF");
  }

  obj = DCDC_ctr_WRI[cha];

  if (!obj) return;

  if (LVDS[adr].wri.sta == status_NA) {
    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    fl_set_object_label(obj, "");
  }
  else if (LVDS[adr].wri.dat & DCDC[cha].ctr.msk) {
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_label(obj, "");
  }
  else {
    fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    fl_set_object_label(obj, "");
  }
}

//~-----------------------------------------------------------------------------

void update_JPMD_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<6; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
    update_DCDC_button(j);
  }
}

//~============================================================================

void update_JPIF_panel(int cha);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPIF_panel(int x0, int y0, int cha) {

  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;
  
  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 8*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<8; i++, j++) {
    x = x0 + wa;
    if (i == 0) add_frame_box(x, y, wb, 4*h, DCDC[j].J_JT_board, size);
    if (i == 4) add_frame_box(x, y, wb, 2*h, DCDC[j].J_JT_board, size);
    if (i == 6) add_frame_box(x, y, wb, 2*h, DCDC[j].J_JT_board, size);
    x = x + wb;
                add_frame_box(x, y, wc,   h, DCDC[j].feed,       size);
    x = x + wc;
                add_frame_box(x, y, wd,   h, DCDC[j].nominal,    size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<8; i++, j++) {
    x = x0 + w;
    DCDC_ctr_BOX[j] =       add_data_box(x, y, w1,   h, "0", size);
    x = x + w1;
    DCDC_mon_BOX[j] =       add_data_box(x, y, w1,   h, "1", size);
    x = x + w1;
    DCDC_acc_BOX[j] =       add_data_box(x, y, w3,   h, "", size);
    x = x + w3;
    DCDC_dir_BOX[j] =       add_data_box(x, y, w1,   h, "", size);
    x = x + w1;
    if (i % 2 == 0) {
      fl_addto_group(VOL1);
        DCDC_vol_cod_BOX[j]  = add_data_box(x,      y,   w2, 2*h, "2", size);
        DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y,   w2, 2*h, "3", size);
      fl_end_group();
      fl_addto_group(CUR1);
        DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y,   w2, 2*h, "4", size);
        DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y,   w2, 2*h, "5", size);
      fl_end_group();
      fl_addto_group(VOL2);
        DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, 2*h, "3", size);
      fl_end_group();
      fl_addto_group(CUR2);
        DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, 2*h, "5", size);
      fl_end_group();
    }
    x = x + 4*w2;
    y = y + h;
  }
}

//~-----------------------------------------------------------------------------

void update_JPIF_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<8; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
  }
}

//~============================================================================

void JPJT_ON_OFF_callback(FL_OBJECT *obj, long data);
void update_JPJT_panel(int cha);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPJT_panel(int x0, int y0, int cha) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10, w4 = 30, w5 = 40;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;
  
  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 6*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + wa;
    add_frame_box(x, y, wb, h, DCDC[j].J_JT_board, size);
    x = x + wb;
    add_frame_box(x, y, wc, h, DCDC[j].feed,       size);
    x = x + wc;
    add_frame_box(x, y, wd, h, DCDC[j].nominal,    size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + w;
    DCDC_ctr_BOX[j] =        add_data_box(x, y, w1, h, "0", size);
    x = x + w1;
    DCDC_mon_BOX[j] =        add_data_box(x, y, w1, h, "1", size);
    x = x + w1;
    DCDC_acc_BOX[j] =        add_data_box(x, y, w3,   h, "", size);
    x = x + w3;
    DCDC_dir_BOX[j] =        add_data_box(x, y, w1,   h, "", size);
    x = x + w1;
    fl_addto_group(VOL1);
      DCDC_vol_cod_BOX[j]  = add_data_box(x,      y,   w2, h, "2", size);
      DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y,   w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR1);
      DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y,   w2, h, "4", size);
      DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y,   w2, h, "5", size);
    fl_end_group();
    fl_addto_group(VOL2);
      DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR2);
      DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, h, "5", size);
    fl_end_group();
    x = x + 4*w2;
    DCDC_ctr_WRI[j] =        add_data_box(x, y, w3, h, "", size);
    x = x + w3;
    DCDC_ctr_INP[j] =
    obj = add_switch_button(x, y, w4,   h, "OFF", size);
      fl_set_object_callback(obj, JPJT_ON_OFF_callback, j);
    x = x + w4;
    if (i == 0) {
      DCDC_ctr_SND[j] =
      obj = add_normal_button(x, y, w5, 2*h, "SND", size);
        fl_set_object_callback(obj, SND_callback, j);
    }
    if (i == 2) {
      DCDC_ctr_SND[j] =
      obj = add_normal_button(x, y, w5, 4*h, "SND", size);
        fl_set_object_callback(obj, SND_callback, j);
    }
    y = y + h;
  }
}

//~----------------------------------------------------------------------------

void JPJT_ON_OFF_callback(FL_OBJECT *obj, long data) {

  int cha, adr;

  cha = data;
  adr = DCDC[cha].adr;
  LVDS[adr].wri.inp = LVDS[adr].wri.inp ^ DCDC[cha].ctr.msk;

  msg = 0;
  update_objects();
}

//~-----------------------------------------------------------------------------

void update_JPJT_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<6; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
    update_DCDC_button(j);
  }
}

//~============================================================================

void update_JPSC_panel(int cha);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPSC_panel(int x0, int y0, int cha) {

  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;
  
  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 6*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + wa;
    if (i == 0) add_frame_box(x, y, wb, 6*h, "JPD\n&\nUSCMs", size);
    x = x + wb;
                add_frame_box(x, y, wc,   h, DCDC[j].feed,    size);
    x = x + wc;
                add_frame_box(x, y, wd,   h, DCDC[j].nominal, size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + w;
    DCDC_ctr_BOX[j] =        add_data_box(x, y, w1, h, "0", size);
    x = x + w1;
    DCDC_mon_BOX[j] =        add_data_box(x, y, w1, h, "1", size);
    x = x + w1;
    DCDC_acc_BOX[j] =        add_data_box(x, y, w3,   h, "", size);
    x = x + w3;
    DCDC_dir_BOX[j] =        add_data_box(x, y, w1,   h, "", size);
    x = x + w1;
    fl_addto_group(VOL1);
      DCDC_vol_cod_BOX[j]  = add_data_box(x,      y, w2, h, "2", size);
      DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y, w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR1);
      DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y, w2, h, "4", size);
      DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y, w2, h, "5", size);
    fl_end_group();
    fl_addto_group(VOL2);
      DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR2);
      DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, h, "5", size);
    fl_end_group();
    x = x + 4 * w2;
    y = y + h;
  }
}

//~-----------------------------------------------------------------------------

void update_JPSC_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<6; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
  }
}

//~============================================================================

void read_all_data(void) {

  bool ok[513];

  int16 err;
  int i, j, k;
  int8 n;
  int16 dat[513];
  int known[513];
  int64 Dallas_ID[513];
  float val[513];
  int n1 = 0, n2 = 0, nmax, kmax;
  int done[256];

//~-- "Go" Command ---

  setup_DS1820_readout(0xFF, 0x00, 0x00, 0x00, 1, &err);

//~-- Read JPD Status ---

  for (i=0; i<256; i++) {
    done[i] = 0;
  }
  for (i=0; i<32; i++) {
    int adr = DCDC[i].adr;
    int16 dat;
    if (adr >= 0 && adr <= 255 && done[adr] == 0) {
      read_LVDS_bus(adr, &dat, &err);
      done[adr] = 1;
      if (err) {
        LVDS[adr].rea.sta = status_KO;
        update_objects();
      }
      else {
        LVDS[adr].rea.unk = 0;
        LVDS[adr].rea.sta = status_OK;
        LVDS[adr].rea.dat = dat ^ LVDS[adr].rea.inv;
        LVDS[adr].rea.acc = LVDS[adr].rea.acc | ~LVDS[adr].rea.dat;
      }
    }
  }

//~-- Read JPD Voltages and Currents ---

  n = 32;
  read_ADC(dat, &err);
  if (err) {
    for (i=0; i<32; i++) {
      ADC[i].sta = status_KO;
    }
  }
  else {
    for (i=0; i<32; i++) {
      ADC[i].unk = 0;
      ADC[i].sta = status_OK;
      ADC[i].dat = dat[i];
    }
    for (i=0; i<32; i++) {
      int ind;
      float K, B;
      ind = DCDC[i].cur.ind;
      if (ind >= 0 && ind <= 31) {
        K   = DCDC[i].cur.k;
        B   = DCDC[i].cur.b;
        ADC[ind].val = (ADC[ind].dat - B) / K;
      }
      ind = DCDC[i].vol.ind;
      if (ind >= 0 && ind <= 31) {
        K   = DCDC[i].vol.k;
        B   = DCDC[i].vol.b;
        ADC[ind].val = (ADC[ind].dat - B) / K;
      }
    }
  }

//~-- Read Dallas Sensors IDs and Temperatures ---

  if (!USCM_tmp_unk) USCM_tmp_sta = status_KO;
  for (j=0; j<12; j++) {
    if (!bus[0].DS1820[j].unk) bus[0].DS1820[j].sta = status_KO;
    if (!bus[1].DS1820[j].unk) bus[1].DS1820[j].sta = status_KO;
  }

  for (i=0; i<513; i++) {
    known[i] = 0;
    Dallas_ID[i] = 0;
  }
  read_DS1820_table(9, &n, &Dallas_ID[0], NULL, &err);
  if (P > 0) printf("read_DS1820_table(9,..): err=%04X, n=%d\n", err, n);
  if (!err) {
    n1 = n;
    for (i=1; i<9; i++) {
      read_DS1820_table(i, &n, &Dallas_ID[n1], NULL, &err);
      if (P > 0) printf("read_DS1820_table(%d,..): err=%04X, n=%d\n", i, err, n);
      if (!err) n1 = n1 + n;
      else      break;
    }
  }
  if (err) {
    USCM_tmp_sta = status_KO;
    for (j=0; j<12; j++) {
      bus[0].DS1820[j].sta = status_KO;
      bus[1].DS1820[j].sta = status_KO;
    }
  }
  else {
    Dallas_bus = -1;
    if (n1) USCM_Dallas_ID = Dallas_ID[0];
    else    USCM_Dallas_ID = 0x0000000000000000;
    read_DS1820_temp_long(9, &n, &ok[0], &val[0], NULL, &err);
    if (P > 0) printf("read_DS1820_temp_long(9,..): err=%04X\n", err);
    if (!err) {
      n2 = n;
      for (i=1; i<9; i++) {
        read_DS1820_temp_long(i, &n, &ok[n2], &val[n2], NULL, &err);
        if (P > 0) printf("read_DS1820_temp_long(%d,..): err=%04X, n=%d\n", i, err, n);
        if (!err) n2 = n2 + n;
        else      break;
      }
    }
    if (err) {
      USCM_tmp_sta = status_KO;
      for (j=0; j<12; j++) {
        bus[0].DS1820[j].sta = status_KO;
        bus[1].DS1820[j].sta = status_KO;
      }
    }
    else {
      if (n2) {
        USCM_tmp_unk = 0;
        USCM_tmp_sta = status_OK;
        USCM_tmp_val = val[0];
      }
      nmax = -1;
      kmax = -1;
//    for (k=2*JPD_mod; k<2*JPD_mod+2; k++) {
      for (k=0; k<2; k++) {
        n = 0;        
        for (j=0; j<12; j++) {
          for (i=0; i<n1; i++) {
            if (Dallas[j].ID[k] == Dallas_ID[i]) n = n + 1;
          }
        }
        if (P > 0) printf("k=%d n=%d\n", k, n);
        if (n > 0 && n > nmax) {
          nmax = n;
          kmax = k;
        }
      }
      if (kmax >= 0) {
        Dallas_bus = kmax % 2;
        for (j=0; j<12; j++) {
          if (strncmp(Dallas[j].JPD_board,"USCM",4) == 0) {
            Dallas[j].ID[kmax] = USCM_Dallas_ID;
          }
        }
      }
      if (P > 0) {
        printf("nmax=%d at kmax=%d Dallas_bus=%d(%c) JPD_model=%d(%s)\n", 
                nmax, kmax, 
                Dallas_bus, 
                Dallas_bus<0 ? '?' : (Dallas_bus ? 'B' : 'A'),
                kmax/2, 
                kmax < 0 ? "??" : (kmax/2<N_MOD ? JPD_mods[kmax/2].nam : "??"));
      }
      if (P > 0) printf("n1 = %d\n", n1);
      for (i=0; i<n1; i++) {
        if (P > 0) {
          printf("%3d: %08X%08X ", 
                i, (int32)(Dallas_ID[i]>>32), (int32)((Dallas_ID[i]<<32)>>32));
        }
        for (j=0; j<12; j++) {
          if (kmax >=0 && (Dallas[j].ID[kmax] == Dallas_ID[i]) && ok[i]) {
            if (P > 0) printf("*");
            bus[Dallas_bus].DS1820[j].unk = 0;
            bus[Dallas_bus].DS1820[j].sta = status_OK;
            bus[Dallas_bus].DS1820[j].dat = dat[i];
            bus[Dallas_bus].DS1820[j].val = val[i];
          }
        }
        if (P > 0) printf("\n");
      }
    }
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_JPD_mod_panel();

  if (hide_voltage_code) {
    fl_hide_object(VOL1);
    fl_show_object(VOL2);
  }
  else {
    fl_hide_object(VOL2);
    fl_show_object(VOL1);
  }

  if (hide_current_code) {
    fl_hide_object(CUR1);
    fl_show_object(CUR2);
  }
  else {
    fl_hide_object(CUR2);
    fl_show_object(CUR1);
  }

  update_JPMD_panel( 0);
  update_JPMD_panel( 6);
  update_JPIF_panel(12);
  update_JPJT_panel(20);
  update_JPSC_panel(26);
  update_monitor_panel();
  update_temperature_panel();

#ifdef LOGFILE
//~-- Log File writing ---
  
  obj = WRITING_LOG_FILE;
  if (fl_get_button(obj)) {
    fl_set_object_lcolor(obj, FL_YELLOW);
    fl_set_object_label(obj, "JPD.logfile is being written now");
    if (logfile == NULL) logfile = fopen("JPD.logfile", "a");
  }
  else {
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_label(obj, "JPD.logfile is NOT being written now");
    if (logfile != NULL) {
      fclose(logfile);
      logfile = NULL;
    }
  }

//~-- monitoring ---
  
  for (i=0; i<32; i++) {
    FL_OBJECT *obj = MONITOR[i];
    fl_set_object_label(obj, (char*)mon_text(i));
    if (logfile != NULL) {
      if (i == 0) fprintf(logfile, "===================== %s ======\n", (char*)mytime());
      fprintf(logfile, (char*)mon_text(i));
      fprintf(logfile, "\n");
    }
  }
#endif

//~-- update the image ---

  fl_check_forms();
}

//~----------------------------------------------------------------------------

void command_write_LVDS_bus(int8 adr, int16 dat, int16 *sta) {

  msg = -1;
  update_objects();

  write_LVDS_bus(adr, dat, &err);

  msg = 1;
  update_objects();

  if (logfile != NULL) {
    fprintf(logfile, "===================== %s ======\n", (char*)mytime());
    fprintf(logfile, "***** Command: adr=0x%02X  dat=0x%04X  sta=%d %s\n",
            adr, dat, *sta, *sta ? "  [OK]" : "[FAIL]");
  }
}

//~----------------------------------------------------------------------------

void TIMER_callback(FL_OBJECT *obj, long data) {

  if (monitoring) {
    read_all_data();
    update_objects();
  }
  
  fl_set_timer(obj, (float)timer_period+0.1);
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
