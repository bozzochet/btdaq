// file LeCroy.c
//
// LeCroy Bus Controller - A.Lebedev Aug-2003...
//

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USE_CAN
char *title = {"LeCroy Bus Controller for CAN"};      // global variable
char *date  = {"12-Aug-07"};                          // global variable
#endif
#ifdef USE_AMSW
char *title = {"LeCroy Bus Controller for AMS-Wire"}; // global variable
char *date  = {"12-Aug-07"};                          // global variable
#endif

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USE_CAN
#include "uscmlib.h"
int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x44;                                // global variable
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable
int P = 0;                                         // global variable
#endif

#ifdef USE_AMSW
#include "daqlib.h"
int TX = 2;                 // global variable
int RX = 2;                 // global variable
int P = 0;                  // global variable
#endif

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

#ifdef USE_CAN
FL_OBJECT *CLOCK_LINE[8], *DATA_LINE[16];
#endif

#ifdef USE_AMSW
FL_OBJECT *PORT, *VERS;
FL_OBJECT *BUS;
#endif

FL_OBJECT *WRI[32], *WAS[32], *REA[32];
FL_OBJECT *R_W, *SECTION;

int8  clk;
int16 dat;
int8 bus;
int32 wri, rea;
int was[32];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void create_line_selector(int x0, int y0);

void create_bus_panel(int x0, int y0);
void bus_callback(FL_OBJECT *obj, long data);
void INIT_callback(FL_OBJECT *obj, long data);
void update_bus_panel(void);

void create_code_selector(int x0, int y0);

void push_button_callback(FL_OBJECT *obj, long data);
void INITIALIZE_callback(FL_OBJECT *obj, long data);
void SEND_callback(FL_OBJECT *obj, long data);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

#ifdef USE_CAN
  int WW = 785, HH = 290;
#endif
#ifdef USE_AMSW
  int WW = 785, HH = 255;
#endif

#ifdef USE_CAN
  bgn_form(&MAIN, 0, 0, WW, HH);
#endif
#ifdef USE_AMSW
  bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
#endif

//AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
//SW_BTN_CALLBACK            = switch_btn_callback;

#ifdef USE_CAN
  create_line_selector(0, 45);
  create_code_selector(0, 125);
#endif
#ifdef USE_AMSW
  create_bus_panel(    0, 45);
  create_code_selector(0, 90);
#endif

  end_form(MAIN);
  update_objects();
}

//~============================================================================

#ifdef USE_CAN

void create_line_selector(int x0, int y0) {

  FL_OBJECT *obj;
  int i, s, h, x1, y1, x2, y2, x3, y3, w1, w2, x, y;
  
  s = 25;
  h = 15;
  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,785,80,"");

  x1 = x0 + 5;
  y1 = y0 + 5;
  w1 = 8 * s + 10;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x1,y1,w1,30,"Clock Lines");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  
  for (i=0; i<8; i++) {
    char b[80];
    x = x1 + i * s + i / 4 * 10;
    y = y1 + 30;
    CLOCK_LINE[i] = 
    obj = fl_add_button(FL_PUSH_BUTTON,x,y,s,s,"");
      fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
      fl_set_object_color(obj,FL_GREEN,FL_GREEN);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
      fl_set_object_callback(obj, push_button_callback, 0);
    y = y1 + 30 + s;
    sprintf(b, "%d", 7-i);
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,h,b);
      fl_set_object_boxtype(obj,FL_NO_BOX);
      fl_set_object_color(obj,FL_BLACK,FL_MCOL);
      fl_set_object_lsize(obj,FL_SMALL_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  }
  
  x2 = x0 + 5 + w1 + 20;
  y2 = y0 + 5;
  w2 = 16 * s + 3 * 10;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x2,y2,w2,30,"Data Lines");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  for (i=0; i<16; i++) {
    char b[80];
    x = x2 + i * s + i / 4 * 10;
    y = y2 + 30;
    DATA_LINE[i] = 
    obj = fl_add_button(FL_PUSH_BUTTON,x,y,s,s,"");
      fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
      fl_set_object_color(obj,FL_GREEN,FL_GREEN);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
      fl_set_object_callback(obj, push_button_callback, 0);
    y = y2 + 30 + s;
    sprintf(b, "%d", 15-i);
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,h,b);
      fl_set_object_boxtype(obj,FL_NO_BOX);
      fl_set_object_color(obj,FL_BLACK,FL_MCOL);
      fl_set_object_lsize(obj,FL_SMALL_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  }

  x3 = x0 + 5 + w1 + 20 + w2 + 5;
  y3 = y0 + 5;
    
  obj = fl_add_button(FL_NORMAL_BUTTON,x3,y3,110,70,"INITIALIZE");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, INITIALIZE_callback, 0);
}

#endif

//~============================================================================

#ifdef USE_AMSW

void create_bus_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w, h, w1 = 35, w2 = 45, h1 = 15, h2 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Bus", FL_SMALL_SIZE);

  x = x + w1;
  obj = add_normal_button(x, y, w2, h1+h2, "INIT", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, INIT_callback, 0);

  x = x0 + 5;
  y = y + h1;
  BUS =
  obj = add_switch_button(x, y, w1, h2, " ", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, bus_callback, 0);
}

//~----------------------------------------------------------------------------

void bus_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  bus = MOD(bus+1, 8);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) bus = MOD(bus-1, 8);

  update_objects();
}

//~----------------------------------------------------------------------------

void INIT_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  chk = 0;
  init_LeCroy_bus(bus, &err);
  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_bus_panel(void) {

  char b[80];

  sprintf(b, "%d", bus);
  fl_set_object_label(BUS, b);
}

//~============================================================================

#endif

//~----------------------------------------------------------------------------

void create_code_selector(int x0, int y0) {

  FL_OBJECT *obj;
  int i, s, h, h5, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6;
  int w1, w2, w3, w4, x, y;
  
  s  = 20;
  h  = 50;
  h5 = 15;
  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,785,135,"");

  x1 = x0 + 30;
  y1 = y0 + 5;
  w1 = 4 * s;
  
  y = y1;

  for (i=0; i<4; i++) {
    char *t[4] = {"1", "P", "P\nW\nR", "B"};
    x = x1 + i * s;
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,h,t[i]);
      fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
      fl_set_object_color(obj,FL_BLUE,FL_MCOL);
      fl_set_object_lcolor(obj,FL_YELLOW);
      fl_set_object_lsize(obj,FL_NORMAL_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  }

  x2 = x0 + 30 + w1 + 5;
  y2 = y0 + 5;
  w2 = 8 * s + 5;
  
  x = x2;
  y = y2;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w2,h,"Select Code\n(Chip Address)");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x3 = x0 + 30 + w1 + 5 + w2 + 5;
  y3 = y0 + 5;
  w3 = 4 * s;
  
  x = x3;
  y = y3;
  
  R_W = 
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,h/3,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_MCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    
  y = y3 + h / 3;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,2*h/3,"R\nW");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x3 + s;
  y = y3;
  
  SECTION =
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,3*s,h/2,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_MCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    
  y = y3 + h / 2;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,3*s,h/2,"Section");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x4 = x0 + 30 + w1 + 5 + w2 + 5 + w3 + 15;
  y4 = y0 + 5;
  w4 = 16 * s + 3 * 5;
  
  x = x4;
  y = y4;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w4,h,"Data");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x5 = x0 + 30;
  y5 = y0 + 5 + h;
  
  for (i=0; i<32; i++) {
    char b[80];
    x = x5 + i * s + i / 4 * 5 + i / 16 * 10;
    y = y5;
    if (i == 0) {
      obj = fl_add_text(FL_NORMAL_TEXT,x-s-5,y,s+5,s,"@->");
        fl_set_object_boxtype(obj,FL_NO_BOX);
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lsize(obj,FL_NORMAL_SIZE);
        fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
        fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    }
    WRI[i] =
    obj = fl_add_button(FL_PUSH_BUTTON,x,y,s,s,"");
      fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
      fl_set_object_color(obj,FL_GREEN,FL_GREEN);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
      fl_set_object_callback(obj, push_button_callback, 0);
    y = y5 + s;
    if (i == 0) {
      obj = fl_add_text(FL_NORMAL_TEXT,x-s-5,y,s+5,s,"TX");
        fl_set_object_boxtype(obj,FL_NO_BOX);
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lsize(obj,FL_NORMAL_SIZE);
        fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
        fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    }
    was[i] = -1;
    WAS[i] =
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,s,"?");
      fl_set_object_boxtype(obj,FL_BORDER_BOX);
      fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    y = y5 + 2 * s;
    if (i == 0) {
      obj = fl_add_text(FL_NORMAL_TEXT,x-s-5,y,s+5,s,"RX");
        fl_set_object_boxtype(obj,FL_NO_BOX);
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lsize(obj,FL_NORMAL_SIZE);
        fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
        fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    }
    REA[i] =
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,s,"");
      fl_set_object_boxtype(obj,FL_BORDER_BOX);
      fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    y = y5 + 3 * s;
    sprintf(b, "%d", i+1);
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,s,h5,b);
      fl_set_object_boxtype(obj,FL_NO_BOX);
      fl_set_object_color(obj,FL_BLACK,FL_MCOL);
      fl_set_object_lsize(obj,FL_SMALL_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  }

  x6 = x0 + 30 + w1 + 5 + w2 + 5 + w3 + 15 + w4 + 5;
  y6 = y0 + 5;
  
  obj = fl_add_button(FL_NORMAL_BUTTON,x6,y6,60,125,"SEND");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, SEND_callback, 0);
}

//~----------------------------------------------------------------------------

void push_button_callback(FL_OBJECT *obj, long data) {

  update_objects();
}

//~----------------------------------------------------------------------------

void INITIALIZE_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  chk = 0;
  
#ifdef USE_CAN
  bus = bus & 0x7F;
#endif
#ifdef USE_AMSW
  bus = bus & 0x07;
#endif
  init_LeCroy_bus(bus, &err);

  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void SEND_callback(FL_OBJECT *obj, long data) {

  int i;
  
  msg = -1;
  update_objects();

  chk = 1;

#ifdef USE_CAN
  bus = bus & 0x7F;
#endif

#ifdef USE_AMSW
  bus = bus & 0x07;
#endif

  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);

  msg = 1;
  
  for (i=0; i<32; i++) {
    was[i] = fl_get_button(WRI[i]);
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void update_objects(void) {

  FL_OBJECT *obj;
  int i, j;
  int p;
#ifdef USE_CAN
  int16 n, last;
#endif
  char b[80];
  char *section[8] = 
    {"HV DAC",  "AUX DAC",    "OVV lim", "OVC lim", 
     "Program", "HV dis", "Status",  "ADC"};

//~-- common things from template library ---

  update_template_objects(&msg, &err, &chk, &P);

#ifdef USE_CAN

//~-- Clock Lines ---

  bus = 0x00;

  n = 0;
  last = 0;
  for (i=0; i<8; i++) {
    int8 m = 1 << (7 - i);
    obj = CLOCK_LINE[i];
    if (fl_get_button(obj)) {
      fl_set_object_label(obj, "1");
      clk |=  m;
      n++;
      last = 7 - i;
    }
    else {
      fl_set_object_label(obj, "0");
      clk &= ~m;
    }
  }  

  if (n != 1) bus = bus | 0x80;
  else        bus = bus | (last << 4);

  for (i=0; i<8; i++) {
    obj = CLOCK_LINE[i];
    if (n == 1) {
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    else {
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_YELLOW);
    }
  }

//~-- Data Lines ---

  n = 0;
  last = 0;
  for (i=0; i<16; i++) {
    int16 m = 1 << (15 - i);
    obj = DATA_LINE[i];
    if (fl_get_button(obj)) {
      fl_set_object_label(obj, "1");
      dat |=  m;
      n++;
      last = 15 - i;
    }
    else {
      fl_set_object_label(obj, "0");
      dat &= ~m;
    }
  }

  if (n != 1) bus = bus | 0x80;
  else        bus = bus | last;

  for (i=0; i<16; i++) {
    obj = DATA_LINE[i];
    if (n == 1) {
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    else {
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_YELLOW);
    }
  }

#endif

#ifdef USE_AMSW
  update_bus_panel();
#endif

//~-- R/W Bit ---

  obj = WRI[12];
  if (fl_get_button(obj)) fl_set_object_label(R_W, "W");
  else                    fl_set_object_label(R_W, "R");

//~-- Section Field ---

  j = 0;
  for (i=13; i<16; i++) {
    int8 bit = 1 << (15 - i);
    obj = WRI[i];
    if (fl_get_button(obj)) j |=  bit;
  }
  fl_set_object_label(SECTION, section[j]);
  
//~-- WRI Bits ---

  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    obj = WRI[i];
    if (fl_get_button(obj)) {
      fl_set_object_label(obj, "1");
      wri |=  bit;
      if (i != 1) p ^= 1;
    }
    else {
      fl_set_object_label(obj, "0");
      wri &= ~bit;
    }
  }  

//~-- First Bit ---

  obj = WRI[0];
  if (fl_get_button(obj)) fl_set_object_color(obj, FL_GREEN, FL_GREEN);
  else                    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);

//~-- Parity Bit ---

  obj = WRI[1];
  if (fl_get_button(obj) == p) fl_set_object_color(obj, FL_GREEN, FL_GREEN);
  else                         fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);

//~-- Power Bit ---

  obj = WRI[2];
  if (fl_get_button(obj)) fl_set_object_color(obj, FL_GREEN, FL_GREEN);
  else                    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);

//~-- Broadcast Bit ---

  obj = WRI[3];
  if (!fl_get_button(obj)) fl_set_object_color(obj, FL_GREEN, FL_GREEN);
  else                     fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);

//~-- WAS Bits ---

  for (i=0; i<32; i++) {
    obj = WAS[i];
    sprintf(b, "%d", was[i]);
    if (was[i] < 0) fl_set_object_label(obj, "?");
    else            fl_set_object_label(obj, b);
    if (fl_get_button(WRI[i]) != was[i] || was[i] < 0) {
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    else {
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    }
  }
  
//~-- REA Bits ---

  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    obj = REA[i];
    if (rea & bit) fl_set_object_label(obj, "1");
    else           fl_set_object_label(obj, "0");
  }  

  for (i=0; i<32; i++) {
    obj = REA[i];
    if (chk == 0) fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    else          fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  }

//~-- update the image ---

  fl_check_forms();

//~-- Debug printing ---

  if (P > 1) {
#ifdef USE_CAN
    printf("USCM_ID = 0x%02X = %3d JMDC = %1d\n", USCM_ID, USCM_ID, JMDC_ID);
#endif
    printf("clk = %02X dat = %04X wri = %08X p=%d rea = %08X\n", 
            clk,       dat,       wri,       p,   rea);
    printf("msg = %d err = 0x%04X chk = %d\n", msg, err, chk);
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
