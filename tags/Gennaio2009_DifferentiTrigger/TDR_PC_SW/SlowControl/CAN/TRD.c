// file TRD.c
//
// TRD HV Commander & Monitor - A.Lebedev Sep-2003...

#include "template.h"
#include "uscmlib.h"
 
#define TIMER_PERIOD 2.0     // in seconds

int USCM_ID = 0x12;                                // global variable
int JMDC_ID = 0x2;                                 // global variable
char *config_file_name = {"TRD.configfile"};       // global variable

typedef struct {
  FL_FORM *TRD;
  void *vdata;
  long ldata;
} FD_TRD;

FD_TRD *TRD;

FL_OBJECT *MONITOR, *TIMER, *SETUP, *RESIZE;
FL_OBJECT *HIDE[6][2][7];
FL_OBJECT *PANEL[6][2][7];
FL_OBJECT *STATUSS[6][2][7];
FL_OBJECT *INPUT[6][2][7];
FL_OBJECT *VOLTAGE[6][2][7];
FL_OBJECT *CURRENT[6][2][7];
FL_OBJECT *STEP[6][2][7][4];

int monitoring;

int hidden[6][2][7];
int status[6][2][7];

typedef struct {
    int never;
    int sta;
    int16 cod;
    float val;
} _value;

struct {
  _value rea;
  _value wri;
} voltage[6][2][7];

struct {
  _value rea;
} current[6][2][7];

int focused;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FD_TRD *create_form_TRD(void);

void create_HV_panels(int x0, int y0);
void create_HV_panel(int x0, int y0, int slot, int half, int chip);
void activate_input(int cha);
void hide_show_callback(FL_OBJECT *obj, long data);
void input_callback(FL_OBJECT *obj, long data);
void focus_callback(FL_OBJECT *obj, long data);
void step_callback(FL_OBJECT *obj, long data);

void MONITOR_callback(FL_OBJECT *obj, long data);
void SETUP_callback(FL_OBJECT *obj, long data);
void RESIZE_callback(FL_OBJECT *obj, long data);

void TIMER_callback(FL_OBJECT *obj, long data);

void read_all_data(void);
void update_objects(void);
void start_TRD(void);

//~----------------------------------------------------------------------------

FD_TRD *create_form_TRD(void) {

  FL_OBJECT *obj;
  
  FD_TRD *fdui = (FD_TRD *) fl_calloc(1, sizeof(*fdui));

  fdui->TRD = fl_bgn_form(FL_NO_BOX, 930, 700);
  
  obj = fl_add_box(FL_FLAT_BOX,0,0,930,700,"A.Lebedev 08-Oct-03");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);

  create_USCM_JMDC_panels();
  
  create_HV_panels(5, 55);
  
  MONITOR =
  obj = add_normal_button(5, 600, 85, 35, "MONITOR", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, MONITOR_callback, 0);
  
  SETUP =
  obj = add_push_button(95, 600, 85, 35, "SETUP", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, SETUP_callback, 0);
  
  RESIZE =
  obj = add_normal_button(185, 600, 85, 35, "RESIZE", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, RESIZE_callback, 0);
  
  monitoring = 0;
  TIMER =
  obj = fl_add_timer(FL_HIDDEN_TIMER, 0, 0, 0, 0,"");
    fl_set_timer(obj, TIMER_PERIOD);
    fl_set_object_callback(obj, TIMER_callback, 0);

  fl_end_form();
  fdui->TRD->fdui = fdui;

  return fdui;
}

//~----------------------------------------------------------------------------

void create_HV_panels(int x0, int y0) {

  FL_OBJECT *obj;
  char b[80];
  int slot, half, chip, step;
  int x, y, xx, yy, w, h;
  int wa=20, wb=30;
  int w1=135, w2=60, w3=60, w4=60, w5=15, w6=125;
  int h1=530, h2=20, h3=15, h4=20, h5=15, h6= 25;
  
  for (slot=0; slot<6; slot++) {
    for (half=0; half<2; half++) {
      for (chip=0; chip<7; chip++) {
        hidden[slot][half][chip] = 0;
        status[slot][half][chip] = 0;
        voltage[slot][half][chip].rea.never = 1;
        voltage[slot][half][chip].wri.never = 1;
        current[slot][half][chip].rea.never = 1;
        voltage[slot][half][chip].rea.sta = status_NA;
        voltage[slot][half][chip].wri.sta = status_NA;
        current[slot][half][chip].rea.sta = status_NA;
      }
    }
  }

  x = x0;
  y = y0;
  w = 5 + wa + wb + 6 * (5 + w6 + 5) + wb + wa + 5;
  h = h1;
  obj = add_frame_box(x, y, w, h, "", 0);

  xx = x0 + 5;
  yy = y0 + h2;
  h  = h3 + h4 + h3 + h5;
  for (chip=0; chip<7; chip++) {
    x = xx;
    y = yy + chip * (h + 5);
    w = wa + wb + 6 * (5 + w6 + 5) + wb;
    sprintf(b, "%d", chip);
    add_frame_box(x,   y, wa, h, b, FL_LARGE_SIZE);
    add_frame_box(x+w, y, wa, h, b, FL_LARGE_SIZE);
    x = xx + wa;
    w = wb + 6 * (5 + w6 + 5);
    add_frame_box(x,   y,          wb, h3, "Sta",  FL_SMALL_SIZE);
    add_frame_box(x+w, y,          wb, h3, "Sta",  FL_SMALL_SIZE);
    add_frame_box(x,   y+h3,       wb, h4, "HV",   FL_MEDIUM_SIZE);
    add_frame_box(x+w, y+h3,       wb, h4, "HV",   FL_MEDIUM_SIZE);
    add_frame_box(x,   y+h3+h4,    wb, h3, "Curr", FL_SMALL_SIZE);
    add_frame_box(x+w, y+h3+h4,    wb, h3, "Curr", FL_SMALL_SIZE);
    add_frame_box(x,   y+h3+h4+h3, wb, h3, "Cmd",  FL_SMALL_SIZE);
    add_frame_box(x+w, y+h3+h4+h3, wb, h3, "Cmd",  FL_SMALL_SIZE);
  }

  xx = x0 + 5 + wa + wb;
  yy = y0;
  h = h2 + 7 * (h3 + h4 + h3 + h5 + 5) - 5;
  for (slot=0; slot<6; slot++) {
    x = xx + slot * w1;
    y = yy;
    obj = add_frame_box(x, y, w1, h1, "", 0);
    y = yy + h;
    x = x + 5;
    sprintf(b, "%d", slot+16);
    obj = add_frame_box(x, y, w6, h6, b, FL_LARGE_SIZE);
    fl_set_object_boxtype(obj, FL_NO_BOX);
    for (half=0; half<2; half++) {
      char *txt[2] = {"A", "B"};
      x = x + half * (w2 + 5);
      y = yy;
      obj = add_frame_box(x, y, w2, h2, txt[half], FL_NORMAL_SIZE);
      fl_set_object_boxtype(obj, FL_NO_BOX);
    }
  }

  xx = x0 + 5 + wa + wb + 5;
  yy = y0 + h2;
  h  = h3 + h4 + h3 + h5;
  for (slot=0; slot<6; slot++) {
    for (half=0; half<2; half++) {
      for (chip=0; chip<7; chip++) {
        x = xx + slot * w1 + half * (w3 + 5);
        y = yy + chip * (h + 5);
        create_HV_panel(x, y, slot, half, chip);
      }
    }
  }

  activate_input(-1);
}

//~----------------------------------------------------------------------------

void create_HV_panel(int x0, int y0, int slot, int half, int chip) {

  FL_OBJECT *obj;
  int x, y, step;
  char b[80];
  int w = 60, h = 15, hh = 20, hhh = h + hh + 2 * h;

  x = x0;
  y = y0;
  
  HIDE[slot][half][chip] = 
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w, hhh, "");
    fl_deactivate_object(obj);
    fl_set_object_callback(obj, hide_show_callback, 100*slot+10*half+chip);

  PANEL[slot][half][chip] = fl_bgn_group();

    STATUSS[slot][half][chip] = 
    obj = add_data_box(x, y, w, h, "", FL_NORMAL_SIZE);

    y = y + h;
  
    INPUT[slot][half][chip] = 
    obj = fl_add_input(FL_FLOAT_INPUT,x,y-3,w,hh+3,"");
      fl_set_object_boxtype(obj,FL_DOWN_BOX);
      fl_set_object_color(obj,FL_WHITE,FL_WHITE);
      fl_set_object_lsize(obj,FL_NORMAL_SIZE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
      fl_set_input_color(obj,FL_BLACK,FL_BLACK);
      fl_set_input_return(obj,FL_RETURN_END);
      fl_set_object_callback(obj, input_callback, 100*slot+10*half+chip);
    VOLTAGE[slot][half][chip] = 
    obj = add_switch_button(x+1, y, w, hh, "", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, focus_callback, 100*slot+10*half+chip);

    y = y + hh;
  
    CURRENT[slot][half][chip] = 
    obj = add_data_box(x, y, w, h, "", FL_NORMAL_SIZE);
  
    y = y + h;
  
    for (step=0; step<4; step++) {
      sprintf(b, "%d", step);
      STEP[slot][half][chip][step] =
      add_switch_button(x+1+step*w/4, y, w/4, h, b, FL_NORMAL_SIZE);
    }

  fl_end_group();
}

//~----------------------------------------------------------------------------

void activate_input(int cha) {

  FL_OBJECT *obj;
  int slot, half, chip;
  char b[80];
  
printf("activate_input(%d)\n",cha);

  for (slot=0; slot<6; slot++) {
    for (half=0; half<2; half++) {
      for (chip=0; chip<7; chip++) {
        if (hidden[slot][half][chip]) continue;
        obj = INPUT[slot][half][chip];
        if (100*slot+10*half+chip == cha) {
          fl_activate_object(obj);
          fl_show_object(obj);
          if (voltage[slot][half][chip].wri.never)
            sprintf(b, "");
          else
            sprintf(b, "%.0f", voltage[slot][half][chip].wri.val);
          fl_set_input(obj,b);
        }
        else {
          fl_deactivate_object(obj);
          fl_hide_object(obj);
        }
      }
    }
  }

  for (slot=0; slot<6; slot++) {
    for (half=0; half<2; half++) {
      for (chip=0; chip<7; chip++) {
        if (hidden[slot][half][chip]) continue;
        obj = VOLTAGE[slot][half][chip];
        if (100*slot+10*half+chip == cha) {
          fl_deactivate_object(obj);
          fl_hide_object(obj);
        }
        else {
          fl_activate_object(obj);
          fl_show_object(obj);
        }
      }
    }
  }

  focused = cha;
}

//~----------------------------------------------------------------------------

void hide_show_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *ob;
  int slot =  data / 100;
  int half = (data % 100) / 10;
  int chip =  data %  10;
  
  printf("hide_show_callback %03d(%03d): %d %d %d\n",
          data, focused, slot, half, chip);

  if (hidden[slot][half][chip]) {
    hidden[slot][half][chip] = 0;
    fl_show_object(PANEL[slot][half][chip]);
    fl_deactivate_object(INPUT[slot][half][chip]);
    fl_hide_object(INPUT[slot][half][chip]);
    fl_deactivate_object(VOLTAGE[slot][half][chip]);
  }
  else {
    hidden[slot][half][chip] = 1;
    fl_hide_object(PANEL[slot][half][chip]);
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void input_callback(FL_OBJECT *obj, long data) {

  int slot =  data / 100;
  int half = (data % 100) / 10;
  int chip =  data %  10;
  
  printf("input_callback %03d(%03d): %d %d %d\n", data, focused, slot, half, chip);

  if (data < 0) return;
  
  voltage[slot][half][chip].wri.val   = atof(fl_get_input(obj));
  voltage[slot][half][chip].wri.never = 0;

  printf("input_callback %03d(%03d): %f\n", 
         data, focused, voltage[slot][half][chip].wri.val);

  activate_input(-1);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void focus_callback(FL_OBJECT *obj, long data) {

  int cha = data;

  printf("focus_callback %03d(%03d):\n", data, focused);

  activate_input(cha);
      
  update_objects();
}

//~============================================================================

void MONITOR_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (monitoring) monitoring = 0;
    else            read_all_data();
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

//~============================================================================

void SETUP_callback(FL_OBJECT *obj, long data) {

  int slot, half, chip, step;

  if (fl_get_button(obj)) {
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_YELLOW);
    activate_input(-1);
    for (slot=0; slot<6; slot++) {
      for (half=0; half<2; half++) {
        for (chip=0; chip<7; chip++) {
          fl_deactivate_object(VOLTAGE[slot][half][chip]);
          for (step=0; step<4; step++) {
            fl_deactivate_object(STEP[slot][half][chip][step]);
          }
          fl_activate_object(HIDE[slot][half][chip]);
        }
      }
    }
  }
  else {
    fl_set_object_color(obj, FL_CYAN, FL_CYAN);
    fl_set_object_lcolor(obj, FL_BLACK);
    for (slot=0; slot<6; slot++) {
      for (half=0; half<2; half++) {
        for (chip=0; chip<7; chip++) {
          fl_deactivate_object(HIDE[slot][half][chip]);
          if (hidden[slot][half][chip]) continue;
          fl_activate_object(VOLTAGE[slot][half][chip]);
          for (step=0; step<4; step++) {
            fl_activate_object(STEP[slot][half][chip][step]);
          }
        }
      }
    }
  }

  update_objects();
}

//~============================================================================

void RESIZE_callback(FL_OBJECT *obj, long data) {

  static int s = 0;
  
printf("resize_callback\n");

  fl_set_object_position(PANEL[0][0][0], 0, 0);
  fl_set_object_size(PANEL[0][0][0], 200, 200);
  
  update_objects();
}

//~============================================================================

void TIMER_callback(FL_OBJECT *obj, long data) {

  if (monitoring) {
    read_all_data();
    update_objects();
  }

  fl_set_timer(obj, TIMER_PERIOD);
}

//~============================================================================

void read_all_data(void) {

  int slot, half, chip;

  for (slot=0; slot<6; slot++) {
    for (half=0; half<2; half++) {
      for (chip=0; chip<7; chip++) {
        status[slot][half][chip] = rand() % 4;
        voltage[slot][half][chip].rea.never = 0;
        voltage[slot][half][chip].rea.sta = rand() % 3;
        voltage[slot][half][chip].rea.val = voltage[slot][half][chip].wri.val;
        current[slot][half][chip].rea.never = 0;
        current[slot][half][chip].rea.sta = rand() % 3;
        current[slot][half][chip].rea.val = ((float)rand())/1000.0;
      }
    }
  }
}

//~============================================================================

void show_value(FL_OBJECT *obj, _value value) {

  int color, lcolor;
  char b[80];
  int  colors[3] = {FL_YELLOW, FL_GREEN, FL_BLACK};
  int lcolors[3] = {FL_BLACK,  FL_BLACK, FL_WHITE};
  
  if (value.never) {
    sprintf(b, "");
    color  = FL_YELLOW;
    lcolor = FL_BLACK;
  }
  else {
    sprintf(b, "%6.1f", value.val);
    color  =  colors[value.sta];
    lcolor = lcolors[value.sta];
  }
  
  fl_set_object_label( obj, b);
  fl_set_object_color(obj, color, color);
  fl_set_object_lcolor(obj, lcolor);
}

//~----------------------------------------------------------------------------

void show_status(FL_OBJECT *obj, int status) {

  char *txt[4] =  {"O.K.",  "TRIP",    "OFF",      "ERROR"};
  int color[4] =  {FL_GREEN, FL_RED,    FL_YELLOW, FL_RED};
  int lcolor[4] = {FL_BLACK, FL_YELLOW, FL_BLACK,  FL_YELLOW};

  fl_set_object_label( obj, txt[status]);
  fl_set_object_color( obj, color[status], color[status]);
  fl_set_object_lcolor(obj, lcolor[status]);
}

//~----------------------------------------------------------------------------

void show_HV_panels(void) {

  FL_OBJECT *obj;
  int slot, half, chip;
  char b[80];

  for (slot=0; slot<6; slot++) {
    for (half=0; half<2; half++) {
      for (chip=0; chip<7; chip++) {
        show_status(STATUSS[slot][half][chip], status[slot][half][chip]);
        show_value(VOLTAGE[slot][half][chip], voltage[slot][half][chip].rea);
        show_value(CURRENT[slot][half][chip], current[slot][half][chip].rea);
      }
    }
  }
}

//~----------------------------------------------------------------------------

void update_objects(void) {

  FL_OBJECT *obj;

  show_USCM_JMDC_panels();
  show_HV_panels();
  
  fl_check_forms();
}

//~============================================================================

void start_TRD(void) {

  int one = 1;
  static char *title = {"TRD"};
  char *adr[] = {(char*)&title};
  
  fl_initialize(&one, adr, " ", 0, 0);
  TRD = create_form_TRD();
  fl_set_form_position(TRD->TRD, 0, 0);
  fl_show_form(TRD->TRD, FL_PLACE_GEOMETRY, FL_FULLBORDER, "TRD");
  
  update_objects();
}

//~============================================================================

main(int argc, char *argv[]) {

  start_TRD();

  while(1) {
    fl_do_forms();
  }
}

//~============================================================================
