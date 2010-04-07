// file UGBS.c
//
// UGBS - "Pro" Controller - A.Lebedev Mar-2004...

#include "uscmlib.h"
#include "xformslib.h"

//~----------------------------------------------------------------------------

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x00;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char *config_file_name = {"UGBS.configfile"};      // global variable

int P = 0;                                         // global variable

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

#define ALL  0
#define SOME 1

typedef struct {
  FL_FORM *UGBS;
  void *vdata;
  long ldata;
} FD_UGBS;

FD_UGBS *UGBS;

int8  clk;
int16 dat;
int32 wri, rea;

FL_OBJECT *MESSAGE;
int msg;
int16 sta;
int8 chk;

FL_OBJECT *UGBS_BRD, *UGBS_CLK, *UGBS_DAT;
int brd;

struct {
  char *nam;
  int16 cmd;
  int16 msk;
  struct {
    int16 cod;
    float val;
    int   sta;
  } wri;
  FL_OBJECT *WAS;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
  FL_OBJECT *INP;
  FL_OBJECT *VAL;
  FL_OBJECT *OPN;
  FL_OBJECT *STA;
  FL_OBJECT *CNT;
} valve[14] = {

  {"V1a",       0x12, 0x8000},
  {"V2a",       0x12, 0x4000},
  {"V3a",       0x12, 0x2000},
  {"V4a",       0x12, 0x1000},
  {"V5",        0x12, 0x0800},
  {"V10a",      0x12, 0x0400},
  {"V20a",      0x12, 0x0200},
  {"V1b",       0x12, 0x0100},
  {"V2b",       0x12, 0x0080},
  {"V3b",       0x12, 0x0040},
  {"V4b",       0x12, 0x0020},
  {"V10b",      0x12, 0x0010},
  {"V20b",      0x12, 0x0008},
  {"V20a&V20b", 0x12, 0x0208}};

struct {
  struct {
    struct {
      int16 cmd;
      int16 cod;
      int16 was;
      int   sta;
    } wri;
    FL_OBJECT *SET;
    struct {
      int16 cmd;
      int16 cod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } ena;
  struct {
    struct {
      int16 cmd;
      int16 cod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } sta;
  struct {
    struct {
      int16 cmd;
      int16 cod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
    FL_OBJECT *CLR;
  } cnt;
} valves;

int valve_INP_active;

struct {
  char *nam;
  int16 cmd;
  int16 msk;
  FL_OBJECT *WAS;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
} regulator[2] = {

  {"24V",       0x12, 0x1234},
  {"5V",        0x12, 0x1234}};

struct {
  struct {
    int16 cmd;
    int16 cod;
    int16 was;
    int   sta;
  } wri;
  FL_OBJECT *SET;
  struct {
    int16 cmd;
    int16 cod;
    int   sta;
  } rea;
  FL_OBJECT *GET;
} regulators;

struct {
  char *nam;
  int16 cmd;
  int   mon;
  struct {
    int16 cod;
    float val;
    int   sta;
  } rea;
  FL_OBJECT *VAL;
  FL_OBJECT *GET;
} sensor[8] = {

  {"P1a", 0x12},
  {"T1a", 0x12},
  {"P1b", 0x12},
  {"T1b", 0x12},
  {"P2a", 0x12},
  {"T2a", 0x12},
  {"P2b", 0x12},
  {"T2b", 0x12}};

FL_OBJECT *PERIOD;
int timer_period = 2;

FL_OBJECT *MONITOR;
int monitoring = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FD_UGBS *create_form_UGBS(void);

void invalidate_data(void);

void create_UGBS_address_panel(int x0, int y0);
void UGBS_brd_callback(FL_OBJECT *obj, long data);
void UGBS_clk_callback(FL_OBJECT *obj, long data);
void UGBS_dat_callback(FL_OBJECT *obj, long data);
void INIT_callback(FL_OBJECT *obj, long data);
void update_UGBS_address_panel(void);

void create_valve_panel(int x0, int y0);
void activate_valve_INP(int ind, int mode);
void valve_WRI_callback(FL_OBJECT *obj, long data);
void valve_INP_focus_callback(FL_OBJECT *obj, long data);
void valve_INP_callback(FL_OBJECT *obj, long data);
void valve_OPN_callback(FL_OBJECT *obj, long data);
void valves_ena_SET_callback(FL_OBJECT *obj, long data);
void valves_ena_GET_callback(FL_OBJECT *obj, long data);
void valves_sta_GET_callback(FL_OBJECT *obj, long data);
void valves_cnt_GET_callback(FL_OBJECT *obj, long data);
void valves_cnt_CLR_callback(FL_OBJECT *obj, long data);
void update_valve_panel(void);

void create_regulator_panel(int x0, int y0);
void regulator_WRI_callback(FL_OBJECT *obj, long data);
void regulators_SET_callback(FL_OBJECT *obj, long data);
void regulators_GET_callback(FL_OBJECT *obj, long data);
void update_regulator_panel(void);

void create_sensor_panel(int x0, int y0);
void sensor_GET_callback(FL_OBJECT *obj, long data);
void read_sensor_data(int ind);
void update_sensor_panel(void);

void create_monitor_panel(int x0, int y0);
void PERIOD_callback(FL_OBJECT *obj, long data);
void MONITOR_callback(FL_OBJECT *obj, long data);
void update_monitor_panel(void);

void TIMER_callback(FL_OBJECT *obj, long data);

void read_data(int opt);

void create_P_panel(int x0, int y0);
void P_callback(FL_OBJECT *obj, long data);
void update_P_panel(void);

void update_message(void);

void update_objects(void);
void start_UGBS(void);

//~============================================================================

FD_UGBS *create_form_UGBS(void) {

  FL_OBJECT *obj;
  
  FD_UGBS *fdui = (FD_UGBS *) fl_calloc(1, sizeof(*fdui));

  invalidate_data();

  fdui->UGBS = fl_bgn_form(FL_NO_BOX,485,575);

  obj = fl_add_box(FL_FLAT_BOX,0,0,485,575,"A.Lebedev 26-Oct-08");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);

  obj = fl_add_box(FL_NO_BOX,125,5,360,45,"UGBS  Controller");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_EMBOSSED_STYLE);

  create_USCM_JMDC_panels();

  create_UGBS_address_panel(5, 50);
  create_valve_panel(5, 90);
  create_regulator_panel(325, 90);
  create_sensor_panel(325, 175);
  create_monitor_panel(325, 490);

  create_P_panel(450, 530);

  msg = 0;
  MESSAGE =
  obj = fl_add_text(FL_NORMAL_TEXT,5,530,440,40,"");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  obj = fl_add_timer(FL_HIDDEN_TIMER, 0, 0, 0, 0,"");
    fl_set_timer(obj, (float)timer_period);
    fl_set_object_callback(obj, TIMER_callback, 0);

  fl_end_form();

  fdui->UGBS->fdui = fdui;

  return fdui;
}

//~-------------------------------------------------------------------------------

void invalidate_data(void) {

  int i;

  for (i=0; i<14; i++) {
    valve[i].wri.sta = status_NN;
  }

  valves.ena.wri.cod = 0;
  valves.ena.wri.sta = status_NN;
  valves.ena.rea.sta = status_NN;
  valves.cnt.rea.sta = status_NN;
  
  for (i=0; i<8; i++) {
    sensor[i].rea.sta = status_NN;
    sensor[i].mon = 0;
  }
}

//~===============================================================================

void create_UGBS_address_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, size, w1 = 40, w2 = 75, w3 = 145, w4 = 40, h = 15;

  brd = 0;
  clk = 1;
  dat = 1;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+w1+w2+w3+w4,10+2*h,"");

  size = FL_SMALL_SIZE;
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h, "Board", size);

  x = x + w1;
  obj = add_frame_box(x, y, w2, h, "Clock Line", size);

  x = x + w2;
  obj = add_frame_box(x, y, w3, h, "Data Line", size);

  size = FL_MEDIUM_SIZE;

  x = x + w3;
  obj = add_normal_button(x, y, w4, 2*h, "INIT", size);
    fl_set_object_callback(obj, INIT_callback, 0);

  size = FL_NORMAL_SIZE;

  x = x0 + 5;
  y = y + h;
  UGBS_BRD =
  obj = add_switch_button(x, y, w1, h, "", size);
    fl_set_object_callback(obj, UGBS_brd_callback, 0);

  x = x + w1;
  UGBS_CLK =
  obj = add_switch_button(x, y, w2, h, "", size);
    fl_set_object_callback(obj, UGBS_clk_callback, 0);

  x = x + w2;
  UGBS_DAT =
  obj = add_switch_button(x, y, w3, h, "", size);
    fl_set_object_callback(obj, UGBS_dat_callback, 0);
}

//~-------------------------------------------------------------------------------

void UGBS_brd_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  brd = MOD(brd+1, 4);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) brd = MOD(brd-1, 4);

  invalidate_data();
  update_objects();
}

//~-------------------------------------------------------------------------------

void UGBS_clk_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  if (!(clk<<=1)) clk = 0x01;
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) if (!(clk>>=1)) clk = 0x80;

  invalidate_data();
  update_objects();
}

//~-------------------------------------------------------------------------------

void UGBS_dat_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  if (!(dat<<=1)) dat = 0x0001;
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) if (!(dat>>=1)) dat = 0x8000;

  invalidate_data();
  update_objects();
}

//~-------------------------------------------------------------------------------

void INIT_callback(FL_OBJECT *obj, long data) {

  int16 err;
  int bus = (clk << 4) | dat;

  msg = -1;
  update_objects();

  chk = 0;
//init_LeCroy_bus(clk, dat, &err);
  init_LeCroy_bus(bus, &err);
  if (err) sta = 0;
  else     sta = 1;
  msg = 1;

  update_objects();
}

//~-------------------------------------------------------------------------------

void update_UGBS_address_panel(void) {

  char b[80];

  sprintf_binary(b, (int32)brd, -2);
  fl_set_object_label(UGBS_BRD, b);

  sprintf_binary(b, (int32)clk, -8);
  fl_set_object_label(UGBS_CLK, b);

  sprintf_binary(b, (int32)dat, -16);
  fl_set_object_label(UGBS_DAT, b);
}

//~===============================================================================

void create_valve_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 75, w1 = 10, w2 = 25, w3 = 75, w4 = 50, h = 25;
  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+w1+2*w2+w3+w4+2*w2,10+17*h,"");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa, 2*h, "Valve", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w1+2*w2, 2*h, "Enable", size1);
  x = x + w1 + 2 * w2;
  obj = add_frame_box(x, y, w3, 2*h, "Time, s", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w4, 2*h, "Cmnd", size1);
  x = x + w4;
  obj = add_frame_box(x, y, 2*w2, h, "Status", size1);
  obj = add_frame_box(x, y+h, w2, h, "S", size1);
  x = x + w2;
  obj = add_frame_box(x, y+h, w2, h, "C", size1);

  y = y + 2 * h;
  for (i=0; i<14; i++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wa, h, valve[i].nam, size1);
    x = x + wa;
    valve[i].WAS =
    obj = add_data_box(x, y, w1, h, "", size2);
    x = x + w1;
    valve[i].WRI =
    obj = add_switch_button(x, y, w2, h, "X", size2);
      fl_set_object_callback(obj, valve_WRI_callback, i);
    x = x + w2;
    valve[i].REA =
    obj = add_data_box(x, y, w2, h, "Y", size2);
    x = x + w2;
    valve[i].VAL = 
    obj = add_switch_button(x, y, w3, h, "", size2);
      fl_set_object_callback(obj, valve_INP_focus_callback, i);
    valve[i].INP = 
    obj = fl_add_input(FL_FLOAT_INPUT,x,y-5,w3,h+10,"");
      fl_set_object_boxtype(obj,FL_DOWN_BOX);
      fl_set_object_color(obj,FL_WHEAT,FL_WHEAT);
      fl_set_object_lsize(obj,FL_LARGE_SIZE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
      fl_set_input_color(obj,FL_BLACK,FL_BLACK);
      fl_set_input_return(obj,FL_RETURN_END);
      fl_set_object_callback(obj, valve_INP_callback, i);
    x = x + w3;
    valve[i].OPN =
    obj = add_normal_button(x, y, w4, h, "OPEN", size2);
      fl_set_object_callback(obj, valve_OPN_callback, i);
    x = x + w4;
    valve[i].STA =
    obj = add_data_box(x, y, w2, h, "Z", size2);
    x = x + w2;
    valve[i].CNT =
    obj = add_data_box(x, y, w2, h, "9", size2);
    y = y + h;
  }

  activate_valve_INP(-1, 0);
    
  x = x0 + 5;
  obj = add_frame_box(x, y, wa+w1, h, "", size1);
  x = x + wa + w1;
  valves.ena.SET =
  obj = add_normal_button(x, y, w2, h, "W", size2);
    fl_set_object_callback(obj, valves_ena_SET_callback, 0);
  x = x + w2;
  valves.ena.GET =
  obj = add_normal_button(x, y, w2, h, "R", size2);
    fl_set_object_callback(obj, valves_ena_GET_callback, 0);
  x = x + w2;
  obj = add_frame_box(x, y, w3+w4, h, "", size1);
  x = x + w3 + w4;
  valves.sta.GET =
  obj = add_normal_button(x, y, w2, h, "R", size2);
    fl_set_object_callback(obj, valves_sta_GET_callback, 0);
  x = x + w2;
  valves.cnt.GET =
  obj = add_normal_button(x, y, w2, h, "R", size2);
    fl_set_object_callback(obj, valves_cnt_GET_callback, 0);
}

//~-------------------------------------------------------------------------------

void activate_valve_INP(int ind, int mode) {

  FL_OBJECT *obj;
  int i;
  char b[80];
  
  for (i=0; i<14; i++) {
    obj = valve[i].INP;
    if (i == ind) {
      fl_activate_object(obj);
      fl_show_object(obj);
      if (valve[i].wri.sta == status_NN) {
        sprintf(b, " ");
      }
      else {
        if (mode) sprintf(b, "%.000f", valve[i].wri.val);
        else      sprintf(b, " ");
      }
      fl_set_input(obj, b);
    }
    else {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
  }

  for (i=0; i<14; i++) {
    obj = valve[i].VAL;
    if (i == ind) {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
    else {
      fl_activate_object(obj);
      fl_show_object(obj);
    }
  }

  valve_INP_active = ind;
}

//~-------------------------------------------------------------------------------

void valve_WRI_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 m;
  
  if (ind != 13) {
    valves.ena.wri.cod ^= valve[ind].msk;
  }
  else {
    m = valves.ena.wri.cod & valve[13].msk;
    if (m == 0 || m == valve[13].msk) {
      valves.ena.wri.cod ^= valve[13].msk;
    }
    else {
      valves.ena.wri.cod &= ~valve[13].msk;
    }
  }
  
printf("Enable valve %2d,    %04X\n", ind, valves.ena.wri.cod);
  
  msg = 0;
  update_objects();
}

//~-------------------------------------------------------------------------------

void valve_INP_focus_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int mode = -1;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  mode = 0;
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) mode = 1;

  activate_valve_INP(ind, mode);
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void valve_INP_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  double t;

  t = atof(fl_get_input(valve[ind].INP));
  if (t < 0.0)    t = 0.0;
  if (t > 65.535) t = 65.535;
  valve[ind].wri.val = t;
  valve[ind].wri.cod = t * 1000.0;
  valve[ind].wri.val = valve[ind].wri.cod / 1000.0;
  
  valve[ind].wri.sta = status_NA;

  activate_valve_INP(-1, 0);

  update_objects();
}

//~-------------------------------------------------------------------------------

void valve_OPN_callback(FL_OBJECT *obj, long data) {

  int ind = data;

  if (ind == valve_INP_active) activate_valve_INP(-1, 0);

  msg = -1;
  update_objects();

printf("OPEN valve %d\n", ind);
  
  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void valves_ena_SET_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

printf("Enable valves %04X\n", valves.ena.wri.cod);

  valves.ena.wri.was = valves.ena.wri.cod;
  valves.ena.wri.sta = status_OK;  
  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void valves_ena_GET_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

valves.ena.rea.cod = valves.ena.wri.cod & 0x7FFF;
printf("Get faked status %04X\n", valves.ena.rea.cod);

  if (rand() % 10) valves.ena.rea.sta = status_OK;  
  else             valves.ena.rea.sta = status_KO;  
  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void valves_sta_GET_callback(FL_OBJECT *obj, long data) {
}

//~-------------------------------------------------------------------------------

void valves_cnt_GET_callback(FL_OBJECT *obj, long data) {
}

//~-------------------------------------------------------------------------------

void valves_cnt_CLR_callback(FL_OBJECT *obj, long data) {
}

//~-------------------------------------------------------------------------------

void update_valve_panel(void) {

  FL_OBJECT *obj;
  char b[80];
  int16 m, m1, m2;
  int i;
  
  for (i=0; i<14; i++) {
    obj = valve[i].WAS;
    m1 = valves.ena.wri.was & valve[i].msk;
    m2 = valves.ena.wri.cod & valve[i].msk;
    if ((valves.ena.wri.sta == status_OK) && (m1 == m2)) {
      fl_set_object_color(obj, FL_GREEN, 0);
    }
    else {
      fl_set_object_color(obj, FL_YELLOW, 0);
    }
  }
  
  for (i=0; i<14; i++) {
    obj = valve[i].WRI;
    m = valve[i].msk & valves.ena.wri.cod;
    if (m == valve[i].msk) {
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_YELLOW);
      fl_set_object_label(obj, "E");
    }
    else if (m == 0) {
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "D");
    }
    else {
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "?");
    }
  }
  
  for (i=0; i<14; i++) {
    obj = valve[i].REA;
    m  = valve[i].msk & valves.ena.rea.cod;
    m1 = valves.ena.wri.was & valve[i].msk;
    m2 = valves.ena.rea.cod & valve[i].msk;
    if (valves.ena.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
    }
    else {
      if (m == valve[i].msk) fl_set_object_label(obj, "E");
      else if (m == 0)       fl_set_object_label(obj, "D");
      else                   fl_set_object_label(obj, "?");
    }
    if (valves.ena.rea.sta == status_OK) {
      if (m1 == m2) fl_set_object_color(obj, FL_GREEN, 0);
      else          fl_set_object_color(obj, FL_YELLOW, 0);
      if (m != valve[i].msk && m != 0) fl_set_object_color(obj, FL_YELLOW, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (valves.ena.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (valves.ena.rea.sta == status_NA ||
        valves.ena.rea.sta == status_NN) {
      fl_set_object_color(obj, FL_YELLOW, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
  
  for (i=0; i<14; i++) {
    obj = valve[i].VAL;
    sprintf(b, "%6.3f", valve[i].wri.val);
    if (i != valve_INP_active) {
      fl_set_object_label(obj, b);
      if (valve[i].wri.sta == status_OK)
        fl_set_object_color(obj, FL_GREEN, FL_RED);
      else
        fl_set_object_color(obj, FL_YELLOW, FL_RED);
    }
  }
}

//~===============================================================================

void create_regulator_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, x, y, w;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 35, w1 = 10, w2 = 25, h = 25;
  
  w = wa + w1 + 4 * w2;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+w,10+3*h,"");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w, h, "Regulators", size1);
  
  y = y + h;
  for (i=0; i<2; i++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wa, h, regulator[i].nam, size1);
    x = x + wa;
    regulator[i].WAS =
    obj = add_data_box(x, y, w1, h, "", size2);
    x = x + w1;
    regulator[i].WRI =
    obj = add_switch_button(x, y, w2, h, "X", size2);
      fl_set_object_callback(obj, regulator_WRI_callback, 0);
    x = x + w2;
    if (i == 0) {
      regulators.SET =
      obj = add_normal_button(x, y, w2, 2*h, "W", size2);
        fl_set_object_callback(obj, regulators_SET_callback, 0);
    }
    x = x + w2;
    regulator[i].REA =
    obj = add_data_box(x, y, w2, h, "Y", size2);
    x = x + w2;
    if (i == 0) {
      regulators.GET =
      obj = add_normal_button(x, y, w2, 2*h, "R", size2);
        fl_set_object_callback(obj, regulators_GET_callback, 0);
    }
    y = y + h;
  }
}

//~-------------------------------------------------------------------------------

void regulator_WRI_callback(FL_OBJECT *obj, long data) {
}

//~-------------------------------------------------------------------------------

void regulators_SET_callback(FL_OBJECT *obj, long data) {
}

//~-------------------------------------------------------------------------------

void regulators_GET_callback(FL_OBJECT *obj, long data) {
}

//~-------------------------------------------------------------------------------

void update_regulator_panel(void) {
}

//~===============================================================================

void create_sensor_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 50, w1 = 70, w2 = 25, h = 25;
  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+w1+w2,10+9*h,"");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa, h, "Sensor", size1);
  
  x = x + wa;
  obj = add_frame_box(x, y, w1, h, "Value", size1);
  
  x = x + w1;
  obj = add_frame_box(x, y, w2, h, "", size1);
  
  y = y + h;
  for (i=0; i<8; i++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wa, h, sensor[i].nam, size1);
    x = x + wa;
    sensor[i].VAL =
    obj = add_data_box(x, y, w1, h, "1234.5", size2);
    x = x + w1;
    sensor[i].GET =
    obj = add_normal_button(x, y, w2, h, "R", size2);
      fl_set_object_callback(obj, sensor_GET_callback, i);
    y = y + h;
  }
}

//~-------------------------------------------------------------------------------

void sensor_GET_callback(FL_OBJECT *obj, long data) {

  int ind = data;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (sensor[ind].mon) {
      sensor[ind].mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      read_sensor_data(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    sensor[ind].mon = 1 - sensor[ind].mon;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void read_sensor_data(int ind) {

  printf("read_sensor_data(%d)\n", ind);
  sensor[ind].rea.val = rand();
  if (rand() % 2) sensor[ind].rea.sta = status_OK;
  else            sensor[ind].rea.sta = status_KO;
}

//~-------------------------------------------------------------------------------

void update_sensor_panel(void) {

  FL_OBJECT *obj;
  char b[80];
  int i;

  for (i=0; i<8; i++) {
    obj = sensor[i].VAL;
    if (sensor[i].rea.sta == status_NN || sensor[i].rea.sta == status_NA) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_YELLOW, 0);
    }
    if (sensor[i].rea.sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, 0);
    }
    if (sensor[i].rea.sta == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_GREEN, 0);
    }
    if (sensor[i].rea.sta == status_NN) sprintf(b, " ");
    else                                sprintf(b, "%7.1f", sensor[i].rea.val);
    fl_set_object_label(obj, b);
    obj = sensor[i].GET;
    if (sensor[i].mon) {
      fl_set_object_boxtype(obj, FL_DOWN_BOX);
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_YELLOW);
    }
    else {
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_CYAN, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }

}

//~===============================================================================

void create_monitor_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int wa = 70, w1 = 25, w2 = 50, h = 25;

  fl_add_box(FL_DOWN_BOX, x0, y0, 10+wa+w1+w2, 10+h, "");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, wa, h, "Period, s", FL_NORMAL_SIZE);

  x = x + wa;
  PERIOD = 
  obj = add_switch_button(x, y, w1, h, "?", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, PERIOD_callback, 0);

  x = x + w1;
  MONITOR =
  obj = add_normal_button(x, y, w2, h, "MON", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, MONITOR_callback, 0);
}

//~----------------------------------------------------------------------------

void PERIOD_callback(FL_OBJECT *obj, long data) {

  int t = timer_period - 1;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  t = MOD(t+1, 9);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) t = MOD(t-1, 9);
  
  timer_period = t + 1;

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void MONITOR_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (monitoring) monitoring = 0;
    else            read_data(ALL);
  }
    
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) monitoring = 1 - monitoring;

  if (monitoring) {
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }
  else {
    fl_set_object_boxtype(obj, FL_UP_BOX);
    fl_set_object_color(obj, FL_CYAN, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }

  update_objects();
}

//~----------------------------------------------------------------------------

void update_monitor_panel(void) {

  char b[80];
  
  sprintf(b, "%d", timer_period);
  fl_set_object_label(PERIOD, b);
}

//~===============================================================================

void TIMER_callback(FL_OBJECT *obj, long data) {

  if (monitoring) {
    read_data(ALL);
  }
  else {
    read_data(SOME);
  }
  update_objects();
  fl_set_timer(obj, (float)timer_period);
}

//~-------------------------------------------------------------------------------

void read_data(int opt) {

  int i;
  
  if (opt == ALL) printf("read_data(ALL)\n");

  for (i=0; i<8; i++) {
    if (opt == ALL) printf("  ");
    if (opt == ALL || sensor[i].mon) read_sensor_data(i);
  }
}

//~===============================================================================

void update_message(void) {

  FL_OBJECT *obj;

  obj = MESSAGE;
  if (msg) {
    if (msg < 0) {
      fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
      fl_set_object_lcolor(obj,FL_BLACK);
      fl_set_object_label(obj,"Command is being sent...");
    }
    else {
      if (sta) {
        fl_set_object_color(obj,FL_CYAN,FL_MCOL);
        fl_set_object_lcolor(obj,FL_BLACK);
        if (chk) fl_set_object_label(obj,"Command executed O.K., but...");
        else     fl_set_object_label(obj,"Command executed O.K.");
      }
      else {
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lcolor(obj,FL_YELLOW);
        fl_set_object_label(obj,"Command execution failed!");
      }
    }
    fl_show_object(obj);
  }
  else {
    fl_hide_object(obj);
  }

}

//~===============================================================================

void update_objects(void) {

  update_USCM_JMDC_panels();
  update_UGBS_address_panel();
  update_valve_panel();
  update_regulator_panel();
  update_sensor_panel();
  update_monitor_panel();
  update_P_panel();
  update_message();

  fl_check_forms();
}

//~===============================================================================

void start_UGBS(void) {

  int one = 1;
  static char *title = {"UGBS Monitor"};
  char *adr[] = {(char*)&title};

//if (!initialize_EPP()) exit(1);

  fl_initialize(&one, adr, " ", 0, 0);
  UGBS = create_form_UGBS();
  fl_set_form_position(UGBS->UGBS, 0, 0);
  fl_show_form(UGBS->UGBS, FL_PLACE_GEOMETRY, FL_FULLBORDER, "UGBS");

  update_objects();
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);	// set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: UGBS [<Debug Printout Level>]\n");
  if (argc > 1) P = atoi(argv[1]);
  printf("P = %d\n", P);

  start_UGBS();

  while(1) {
    fl_do_forms();
  }
  return 0;
}

//~============================================================================
