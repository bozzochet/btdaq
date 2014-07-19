// file UHVG.c
//
// UHVG Pro Controller - A.Lebedev Aug-2003...
//

#include "uscmlib.h"
#include "xformslib.h"

//~----------------------------------------------------------------------------

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x44;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char *config_file_name = {"UHVG.configfile"};      // global variable

int P = 0;                                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

typedef struct {
  FL_FORM *UHVG;
  void *vdata;
  long ldata;
} FD_UHVG;

FD_UHVG *UHVG;

FL_OBJECT *CLOCK_LINE[8], *DATA_LINE[16];
FL_OBJECT *WRI[32], *WAS[32], *REA[32];
FL_OBJECT *R_W, *SECTION;
FL_OBJECT *SEND_BUTTON;
FL_OBJECT *MESSAGE;

FL_OBJECT *CHIP_ADR;

FL_OBJECT *HV_WRI, *HV_REA;

FL_OBJECT *DAC_WRI_DIGIT[4][4], *DAC_INPUT[4];
FL_OBJECT *DAC_REA_DIGIT[4][4], *DAC_REA_VALUE[4];

FL_OBJECT *ADC_INP, *ADC_DIGIT[4], *ADC_VALUE, *ADC_READ;

int USCM_radix = 0;

int8  clk;
int16 dat;
int32 wri, rea;
int8 chk;
int16 sta;
int was[32];

int msg;

int16 chip_adr;

int HV_wri_sta, HV_rea_sta;
int16 HV_wri_cod, HV_wri_was, HV_rea_cod, HV_rea_was;

int DAC_input_active = -1;
int DAC_input_changed = 0;

int DAC_wri_sta[4], DAC_rea_sta[4];
int16 DAC_wri_code[4], DAC_wri_was[4], DAC_rea_code[4], DAC_rea_was[4];
float DAC_wri_value[4], DAC_rea_value[4];

int16 ADC, res;
int ADC_sta;
int16 ADC_code;
float ADC_value;

struct pgm_selector {
  char *label;
  int16 mask;
  int n;
  struct {
    char *name;
    int16 code;
  } field[8];
  FL_OBJECT *BUTTON[8];
} pgm_selector[5] = {

{"ADC Input:", 0x0380,8,{{"14 bit DAC",0},{"Temperature",1},{"OVC DAC",   2},{"OVV DAC",  3},
                         {"AUX DAC",   4},{"HV current", 5},{"HV voltage",6},{"Reference",7}}},
{"DAC Clock:", 0xC000,4,{{"400 kHz",   0},{"200 kHz",    1},{"100 kHz",   2},{"Undef.",   3}}},
{"ADC Clock:", 0x3000,4,{{"200 kHz",   3},{"100 kHz",    1},{" 50 kHz",   2},{" 25 kHz",  0}}},
{"ADC Res:",   0x0C00,4,{{"16 bits",   3},{"15 bits",    1},{"14 bits",   2},{"13 bits",  0}}},
{"Duty Cycle:",0x0040,2,{{"50%",       1},{"25%",        0}}}};

FL_OBJECT *PGM_WRI_DIGIT[4], *PGM_REA_DIGIT[4];

int pgm_wri_sta, pgm_rea_sta;
int16 pgm_wri_cod, pgm_wri_was, pgm_rea_cod, pgm_rea_was;

int16 max_DAC_code[4] = {0x3FFF, 0x00FF, 0x003F, 0x003F};
int16 DAC_mask[4]     = {0xFFFC, 0xFF00, 0xFC00, 0xFC00};

int16 max_ADC_code[4] = {0x1FFF, 0x7FFF, 0x3FFF, 0xFFFF};

float V43 = 4.30;
float Vdd = 5.06;

float A   = 506.0;
float B   =   0.0;
float C   =   0.1065;
float K   =   0.020;
float R80 = 100.2;

float V14  = 1.40;
float V19  = 1.90;
float V179 = 0.179;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FD_UHVG *create_form_UHVG(void);
void push_button_callback(FL_OBJECT *obj, long data);

void create_line_panel(int x0, int y0);
void INITIALIZE_callback(FL_OBJECT *obj, long data);
void update_line_panel(void);

void create_binary_code_panel(int x0, int y0);
void SEND_callback(FL_OBJECT *obj, long data);
void update_binary_code_panel(void);

void update_message(void);

void create_chip_address_panel(int x0, int y0);
void chip_address_callback(FL_OBJECT *obj, long data);
void update_chip_address_panel(void);

void create_power_broadcast_panel(int x0, int y0);

void create_HV_dis_panel(int x0, int y0);
void HV_dis_callback(FL_OBJECT *obj, long data);
void WRITE_HV_dis_callback(FL_OBJECT *obj, long data);
void READ_HV_dis_callback(FL_OBJECT *obj, long data);
void update_HV_dis_panel(void);

void create_chip_status_panel(int x0, int y0);

void create_chip_program_panel(int x0, int y0);
void create_chip_program_selector(int x0, int y0, int w, int ind);
void pgm_selector_callback(FL_OBJECT *obj, long data);
void pgm_digit_callback(FL_OBJECT *obj, long data);
void WRITE_PGM_callback(FL_OBJECT *obj, long data);
void READ_PGM_callback(FL_OBJECT *obj, long data);
void update_chip_program_panel(void);

void create_DAC_panels(int x0, int y0);
void create_DAC_panel(
     int x0, int y0, int w1, int w2, int w3, int w4, int h, int ind);
void DAC_digit_callback(FL_OBJECT *obj, long data);
void DAC_input_focus_callback(FL_OBJECT *obj, long data);
void DAC_input_callback(FL_OBJECT *obj, long data);
void WRITE_DAC_callback(FL_OBJECT *obj, long data);
void READ_DAC_callback(FL_OBJECT *obj, long data);
void update_DAC_panels(void);

void create_ADC_panel(int x0, int y0);
void READ_ADC_callback(FL_OBJECT *obj, long data);
void update_ADC_panel(void);

void update_objects(void);
void start_UHVG(void);

//~============================================================================

int16 convert_DAC_to_code(float value, int16 DAC) {

  float R;

  switch (DAC) {
    case 0: R = (value / V43 + B) / A; break;
    case 1: R =  value / Vdd;          break;
    case 2: R = (value / Vdd + B) / A; break;
    case 3: R =  value * K / Vdd + C;  break;
    default:                           return 0;
  }
  return (int16) (R * (float)max_DAC_code[DAC]);
}

//~-------------------------------------------------------------------------------

float convert_DAC_to_value(int16 DAC_code, int16 DAC) {

  float value;
  float R;

  R = (float)DAC_code / (float)max_DAC_code[DAC];

  switch (DAC) {
    case 0: value = V43 * (A * R - B); break;
    case 1: value = Vdd * R;           break;
    case 2: value = Vdd * (A * R - B); break;
    case 3: value = Vdd / K * (R - C); break;
    default:                           return 0.0;
  }

  return value;
}

//~-------------------------------------------------------------------------------

float convert_ADC_to_value(int16 ADC_code, int ADC, int res) {

  float v;
  float R;

  R = (float)ADC_code / (float)max_ADC_code[res];

  switch (ADC) {
    case 0: v =  V43 * (A * R - B);                                  break;
    case 1: v = (V43 * R - V14 + V179) * 100.0 / (V19 - V14) - 50.0; break;
    case 2: v =  V43 / K * (R - C);                                  break;
    case 3: v =  V43 * (A * R - B);                                  break;
    case 4: v =  V43 * R;                                            break;
    case 5: v =  V43 / K * (R - C);                                  break;
    case 6: v =  V43 * (A * R - B);                                  break;
    case 7: v =  V43 * R * 1000.0;                                   break;
    default:                                                         return 0.0;
  }

  return v;
}

//~===============================================================================

FD_UHVG *create_form_UHVG(void) {

  FL_OBJECT *obj;
  
  FD_UHVG *fdui = (FD_UHVG *) fl_calloc(1, sizeof(*fdui));

  fdui->UHVG = fl_bgn_form(FL_NO_BOX,795,650);

  obj = fl_add_box(FL_FLAT_BOX,0,0,795,650,"A.Lebedev 26-Sep-04");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);

  obj = fl_add_box(FL_NO_BOX,130,5,660,45,"UHVG  Pro  Controller");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_EMBOSSED_STYLE);

  create_USCM_JMDC_panels();
  create_line_panel(5, 55);
  create_binary_code_panel(5, 470);

  create_chip_address_panel(5,140);
  create_power_broadcast_panel(115, 140);
  create_HV_dis_panel(5, 205);
  create_chip_status_panel(5, 270);

  create_chip_program_panel(250, 140);

  create_DAC_panels(500, 140);
  create_ADC_panel(500, 405);

  msg = 0;
  MESSAGE =
  obj = fl_add_text(FL_NORMAL_TEXT,5,610,785,35,"");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  fl_end_form();
  fdui->UHVG->fdui = fdui;

  return fdui;
}

//~-------------------------------------------------------------------------------

void push_button_callback(FL_OBJECT *obj, long data) {

  update_objects();
}

//~===============================================================================

void create_line_panel(int x0, int y0) {

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

//~-------------------------------------------------------------------------------

void INITIALIZE_callback(FL_OBJECT *obj, long data) {

  FILE *file;
  int16 err;

  msg = -1;
  update_objects();

  chk = 0;
  init_LeCroy_bus(clk, dat, &err);
  if (err) sta = 0;
  else     sta = 1;
  msg = 1;

  file = fopen("UHVG.configfile", "w");
  if (file != NULL) {
    fwrite(&USCM_ID, sizeof(USCM_ID), 1, file);
    fwrite(&JMDC_ID, sizeof(JMDC_ID), 1, file);
    fclose(file);
  }
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_line_panel(void) {

  FL_OBJECT *obj;
  int i, n;

  n = 0;
  for (i=0; i<8; i++) {
    int8 bit = 1 << (7 - i);
    obj = CLOCK_LINE[i];
    if (fl_get_button(obj)) {
      fl_set_object_label(obj, "1");
      clk |=  bit;
      n++;
    }
    else {
      fl_set_object_label(obj, "0");
      clk &= ~bit;
    }
  }  

  for (i=0; i<8; i++) {
    obj = CLOCK_LINE[i];
    if (n == 1) fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    else        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  }

  n = 0;
  for (i=0; i<16; i++) {
    int16 bit = 1 << (15 - i);
    obj = DATA_LINE[i];
    if (fl_get_button(obj)) {
      fl_set_object_label(obj, "1");
      dat |=  bit;
      n++;
    }
    else {
      fl_set_object_label(obj, "0");
      dat &= ~bit;
    }
  }

  for (i=0; i<16; i++) {
    obj = DATA_LINE[i];
    if (n == 1) fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    else        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  }
}

//~===============================================================================

void create_binary_code_panel(int x0, int y0) {

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
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_BOLD_STYLE);
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
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_BOLD_STYLE);
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
  
  SEND_BUTTON =
  obj = fl_add_button(FL_NORMAL_BUTTON,x6,y6,60,125,"SEND");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, SEND_callback, 0);
}

//~-------------------------------------------------------------------------------

void SEND_callback(FL_OBJECT *obj, long data) {

  FILE *file;
  int16 err;
  int i;
  
  msg = -1;
  update_objects();

  chk = 1;
  exec_LeCroy_bus(clk, dat, wri, &rea, &chk, &err);
  if (err) sta = 0;
  else     sta = 1;
   msg = 1;
  
  for (i=0; i<32; i++) {
    was[i] = fl_get_button(WRI[i]);
  }
  
  file = fopen("UHVG.configfile", "w");
  if (file != NULL) {
    fwrite(&USCM_ID, sizeof(USCM_ID), 1, file);
    fwrite(&JMDC_ID, sizeof(JMDC_ID), 1, file);
    fclose(file);
  }
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_binary_code_panel(void) {

  FL_OBJECT *obj;
  int i, j, p;
  char b[80];
  char *section[8] = 
    {"HV DAC",  "AUX DAC",    "OVV lim", "OVC lim", 
     "Program", "HV dis", "Status",  "ADC"};

//~== R/W Bit ===

  obj = WRI[12];
  if (fl_get_button(obj)) fl_set_object_label(R_W, "W");
  else                    fl_set_object_label(R_W, "R");

//~== Section Field ===

  j = 0;
  for (i=13; i<16; i++) {
    int8 bit = 1 << (15 - i);
    obj = WRI[i];
    if (fl_get_button(obj)) j |=  bit;
  }
  fl_set_object_label(SECTION, section[j]);
  
//~== WRI Bits ===

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

//~== WAS Bits ===

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
  
//~== REA Bits ===

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
    msg = 0;
  }
  else {
    fl_hide_object(obj);
  }

}

//~===============================================================================

void create_chip_address_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w1, w2, h;
  
  chip_adr = 0;
  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,105,60,"");

  w1 = 65;
  w2 = 30;
  h  = 50;
  
  x = x0 + 5;
  y = y0 + 5;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w1,h,"Chip\nAddress");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x0 + 5 + w1;

  CHIP_ADR =
  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w2,h,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
    fl_set_object_callback(obj, chip_address_callback, 0);
}

//~-------------------------------------------------------------------------------

void chip_address_callback(FL_OBJECT *obj, long data) {

  int i;
  
  HV_wri_sta = status_NN;
  HV_rea_sta = status_NN;

  for (i=0; i<4; i++) {
    DAC_wri_sta[i] = status_NN;
    DAC_rea_sta[i] = status_NN;
  }

  ADC_sta = status_NN;

  pgm_wri_sta = status_NN;
  pgm_rea_sta = status_NN;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  chip_adr = MOD(chip_adr+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) chip_adr = MOD(chip_adr-1, 16);

  update_objects();
}

//~-------------------------------------------------------------------------------

void update_chip_address_panel(void) {

  char b[80];
  
  sprintf(b, "%01X", chip_adr);
  fl_set_object_label(CHIP_ADR, b);
}

//~===============================================================================

void create_power_broadcast_panel(int x0, int y0) {

  FL_OBJECT *obj;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,130,60,"");
}

//~===============================================================================

void create_HV_dis_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, h, w1, w2, w3;

  HV_wri_sta = status_NN;
  HV_rea_sta = status_NN;
  
  h  =  50;
  w1 =  65;
  w2 = 105;
  w3 =  60;
  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,240,60,"");
  
  x = x0 + 5;
  y = y0 + 5;

  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w1,h,"HV dis");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x0 + 5 + w1;

  HV_WRI =
  obj = fl_add_button(FL_PUSH_BUTTON,x,y,w2,h/2,"DISABLE");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_GREEN,FL_GREEN);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
    fl_set_object_callback(obj, HV_dis_callback, 0);

  x = x0 + 5 + w1 + w2;

  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w3,h/2,"WRITE");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, WRITE_HV_dis_callback, 0);

  x = x0 + 5 + w1;
  y = y0 + 5 + h/2;

  HV_REA =
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w2,h/2,"Disabled");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_GREEN,FL_MCOL);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_BOLD_STYLE);

  x = x0 + 5 + w1 + w2;

  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w3,h/2,"READ");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, READ_HV_dis_callback, 0);
}

//~-------------------------------------------------------------------------------

void HV_dis_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button(obj)) HV_wri_cod = 0x0000;
  else                    HV_wri_cod = 0x8000;

  update_objects();
}

//~-------------------------------------------------------------------------------

void WRITE_HV_dis_callback(FL_OBJECT *obj, long data) {

  int32 cod = 0xA0080000;
  int i, p;

  pack32(&cod, 0x0FF00000, chip_adr);
  pack32(&cod, 0x00070000, 0x5);
  pack32(&cod, 0x0000FFFF, HV_wri_cod);
  
  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    FL_OBJECT *obj = WRI[i];
    if (cod & bit) fl_set_button(obj, 1);
    else           fl_set_button(obj, 0);
    if (cod & bit && i != 1) p ^= 1;
  }  

  if (p) fl_set_button(WRI[1], 1);
  
  update_objects();

  fl_call_object_callback(SEND_BUTTON);
  
  if (HV_rea_sta != status_NN) HV_rea_sta = status_NA;

  if (sta && !chk) {
    HV_wri_sta = status_OK;
    HV_wri_was = HV_wri_cod;
  }
  else {
    HV_wri_sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void READ_HV_dis_callback(FL_OBJECT *obj, long data) {

  int32 cod = 0xA0000000;
  int i, p;

  pack32(&cod, 0x0FF00000, chip_adr);
  pack32(&cod, 0x00070000, 0x5);
  
  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    FL_OBJECT *obj = WRI[i];
    if (cod & bit) fl_set_button(obj, 1);
    else           fl_set_button(obj, 0);
    if (cod & bit && i != 1) p ^= 1;
  }  

  if (p) fl_set_button(WRI[1], 1);
  
  update_objects();

  fl_call_object_callback(SEND_BUTTON);

  if (sta && !chk) {
    HV_rea_sta = status_OK;
    if (HV_wri_sta == status_OK) HV_rea_was = HV_wri_was;
    HV_rea_cod = (int16)unpack32(rea, 0x0000FFFF);
  }
  else {
    if (HV_rea_sta != status_NN) HV_rea_sta = status_KO;
  }

  msg = 1;  
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_HV_dis_panel(void) {

  FL_OBJECT *obj;

  obj = HV_WRI;

  if (HV_wri_cod) fl_set_object_label(obj, "DISABLE");
  else            fl_set_object_label(obj, "ENABLE");

  if (HV_wri_sta == status_OK && HV_wri_cod == HV_wri_was) {
    fl_set_object_color(obj, FL_GREEN, FL_GREEN);
  }
  else {
    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  }

  obj = HV_REA;

  if (HV_rea_sta == status_NA || HV_rea_sta == status_NN) {
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  }
  if (HV_rea_sta == status_KO) {
    fl_set_object_lcolor(obj, FL_WHITE);
    fl_set_object_color(obj, FL_BLACK, FL_BLACK);
  }
  if (HV_rea_sta == status_OK) {
    fl_set_object_lcolor(obj, FL_BLACK);
    if (HV_wri_sta == status_OK && HV_rea_cod != HV_rea_was) {
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    else {
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    }
  }
  if (HV_rea_sta == status_NN) {
    fl_set_object_label(obj, "");
  }
  else {
    if (HV_rea_cod) fl_set_object_label(obj, "Disabled");
    else            fl_set_object_label(obj, "Enabled");
  }
}

//~===============================================================================

void create_chip_status_panel(int x0, int y0) {

  FL_OBJECT *obj;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,240,195,"");
}

//~===============================================================================

void create_chip_program_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, w1, w2, w3, w4, h;
  int i;

  pgm_wri_sta = status_NN;
  pgm_rea_sta = status_NN;
  pgm_wri_cod = 0x3840;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,245,325,"");

  w = 110;

  create_chip_program_selector(x0+5,    y0+20,  w, 0);
  create_chip_program_selector(x0+5,    y0+180, w, 1);
  create_chip_program_selector(x0+w+15, y0+20,  w, 2);
  create_chip_program_selector(x0+w+15, y0+115, w, 3);
  create_chip_program_selector(x0+w+15, y0+210, w, 4);
  
  w1 = 65;
  w2 = 30;
  w3 = 20;
  w4 = 60;
  
  h = 50;
  
  x = x0 + 5;
  y = y0 + 270;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w1,h,"Chip\nProgram");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x0 + 5 + w1;

  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w2,h/2,"0x");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  
  x = x + w2;
  
  for (i=0; i<4; i++) {
    PGM_WRI_DIGIT[i] =
    obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w3,h/2,"");
      fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
      fl_set_object_color(obj,FL_WHEAT,FL_RED);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
      fl_set_object_callback(obj, pgm_digit_callback, i);
    x = x + w3;
  }

  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w4,h/2,"WRITE");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,WRITE_PGM_callback, 0);
  
  x = x0 + 5 + w1;
  y = y + h / 2;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w2,h/2,"0x");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  
  x = x + w2;
  
  for (i=0; i<4; i++) {
    PGM_REA_DIGIT[i] =
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,w3,h/2,"");
      fl_set_object_boxtype(obj,FL_FRAME_BOX);
      fl_set_object_color(obj,FL_GREEN,FL_MCOL);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_FIXEDBOLD_STYLE);
    x = x + w3;
  }

  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w4,h/2,"READ");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,READ_PGM_callback, 0);
}

//~-------------------------------------------------------------------------------

void create_chip_program_selector(int x0, int y0, int w, int ind) {

  FL_OBJECT *obj;
  struct pgm_selector *s;
  int x, y, h;
  int i;
  
  s = &pgm_selector[ind];
  
  h = 10 + s->n * 15 + 10;
  
  x = x0;
  y = y0;
  
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,x,y,w,h,s->label);
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  fl_bgn_group();
  
    x = x0 + 5;
    
    for (i=0; i<s->n; i++) {
      y = y0 + 10 + i * 15;
      s->BUTTON[i] =
      obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,100,20,s->field[i].name);
        fl_set_object_color(obj,FL_LEFT_BCOL,FL_RED);
        fl_set_object_lsize(obj,FL_NORMAL_SIZE);
        fl_set_object_lstyle(obj,FL_BOLD_STYLE);
        fl_set_object_callback(obj,pgm_selector_callback,10*ind+i);
    }
    
  fl_end_group();
}

//~-------------------------------------------------------------------------------

void pgm_selector_callback(FL_OBJECT *obj, long data) {

  struct pgm_selector *s;
  int i = data / 10;
  int j = data % 10;
  
  s = &pgm_selector[i];
  
  pack16(&pgm_wri_cod, s->mask, s->field[j].code);

  update_objects();
}

//~-------------------------------------------------------------------------------

void pgm_digit_callback(FL_OBJECT *obj, long data) {

  int d[4];
  int ind = data;
  int16 m;

  m = pgm_wri_cod;
  
  d[0] = unpack16(m, 0xF000);
  d[1] = unpack16(m, 0x0F00);
  d[2] = unpack16(m, 0x00F0);
  d[3] = unpack16(m, 0x000F);

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  d[ind] = MOD(d[ind]+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) d[ind] = MOD(d[ind]-1, 16);

  pack16(&m, 0xF000, d[0]);
  pack16(&m, 0x0F00, d[1]);
  pack16(&m, 0x00F0, d[2]);
  pack16(&m, 0x000F, d[3]);

  pgm_wri_cod = m;

  update_objects();
}

//~-------------------------------------------------------------------------------

void WRITE_PGM_callback(FL_OBJECT *obj, long data) {

  int32 cod = 0xA0080000;
  int i, p;

  pack32(&cod, 0x0FF00000, chip_adr);
  pack32(&cod, 0x00070000, 0x4);
  pack32(&cod, 0x0000FFFF, pgm_wri_cod);
  
  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    FL_OBJECT *obj = WRI[i];
    if (cod & bit) fl_set_button(obj, 1);
    else           fl_set_button(obj, 0);
    if (cod & bit && i != 1) p ^= 1;
  }  

  if (p) fl_set_button(WRI[1], 1);
  
  update_objects();

  fl_call_object_callback(SEND_BUTTON);
  
  if (pgm_rea_sta != status_NN) pgm_rea_sta = status_NA;

  if (sta && !chk) {
    pgm_wri_sta = status_OK;
    pgm_wri_was = pgm_wri_cod;
  }
  else {
    pgm_wri_sta = status_KO;
  }

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void READ_PGM_callback(FL_OBJECT *obj, long data) {

  int32 cod = 0xA0000000;
  int i, p;

  pack32(&cod, 0x0FF00000, chip_adr);
  pack32(&cod, 0x00070000, 0x4);
  
  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    FL_OBJECT *obj = WRI[i];
    if (cod & bit) fl_set_button(obj, 1);
    else           fl_set_button(obj, 0);
    if (cod & bit && i != 1) p ^= 1;
  }  

  if (p) fl_set_button(WRI[1], 1);
  
  update_objects();

  fl_call_object_callback(SEND_BUTTON);

  if (sta && !chk) {
    pgm_rea_sta = status_OK;
    if (pgm_wri_sta == status_OK) pgm_rea_was = pgm_wri_was;
    pgm_rea_cod = (int16)unpack32(rea, 0x0000FFFF);
  }
  else {
    if (pgm_rea_sta != status_NN) pgm_rea_sta = status_KO;
  }

  msg = 1;  
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_chip_program_panel(void) {

  FL_OBJECT *obj;
  struct pgm_selector *s;
  int16 m;
  int i, j;
  int16 mask[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
  char b[80];

//~-- pgm selector (buttons) ---

  for (i=0; i<5; i++) {
    s = &pgm_selector[i];
    m = unpack16(pgm_wri_cod, s->mask);
    for (j=0; j<s->n; j++) {
      if (m == s->field[j].code) {
        fl_set_button(s->BUTTON[j], 1);
      }
    }
  }

//~-- pgm selector (fields) ---

  for (i=0; i<5; i++) {
    s = &pgm_selector[i];
    m = unpack16(pgm_rea_cod, s->mask);
    for (j=0; j<s->n; j++) {
      if (pgm_rea_sta != status_NN) {
        if (m == s->field[j].code) {
          fl_set_object_lcolor(s->BUTTON[j], FL_YELLOW);
        }
        else {
          fl_set_object_lcolor(s->BUTTON[j], FL_BLACK);
        }
      }
      else {
        fl_set_object_lcolor(s->BUTTON[j], FL_BLACK);
      }
    }
  }

//~-- pgm Input Code ---

  for (i=0; i<4; i++) {
    int d_now = unpack16(pgm_wri_cod, mask[i]);
    int d_was = unpack16(pgm_wri_was, mask[i]);
    obj = PGM_WRI_DIGIT[i];
    sprintf(b, "%01X", d_now);
    fl_set_object_label(obj, b);
    if (pgm_wri_sta == status_OK && d_now == d_was) {
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    }
    else {
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
  }

//~-- pgm Read Code ---

  for (i=0; i<4; i++) {
    int d_rea = unpack16(pgm_rea_cod, mask[i]);
    int d_was = unpack16(pgm_rea_was, mask[i]);
    obj = PGM_REA_DIGIT[i];
    if (pgm_rea_sta == status_NA || pgm_rea_sta == status_NN) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    if (pgm_rea_sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, FL_BLACK);
    }
    if (pgm_rea_sta == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      if (pgm_wri_sta == status_OK && d_rea != d_was) {
        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      }
      else {
        fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      }
    }
    if (pgm_rea_sta == status_NN) sprintf(b, " ");
    else                          sprintf(b, "%01X", d_rea);
    fl_set_object_label(obj, b);
  }
}

//~===============================================================================

void create_DAC_panels(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y;
  int h1, h2, w1, w2, w3, w4;

  h1 = 30;
  h2 = 50;
  w1 = 65;
  w2 = 80;
  w3 = 75;
  w4 = 60;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,290, 260,"");
  
  x = x0 + 5;
  y = y0 + 5;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w1,h1,"DAC");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    
  x = x0 + 5 + w1;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w2,h1,"Code");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    
  x = x0 + 5 + w1 + w2;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w3,h1,"Value");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    
  x = x0 + 5 + w1 + w2 + w3;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w4,h1,"Cmd.");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  for (i=0; i<4; i++) {
    DAC_wri_sta[i] = status_NN;
    DAC_rea_sta[i] = status_NN;
    x = x0 + 5;
    y = y0 + 5 + h1 + 5 + i * (h2 + 5);
    create_DAC_panel(x, y, w1, w2, w3, w4, h2, i);
  }
}

//~-------------------------------------------------------------------------------

void create_DAC_panel(
     int x0, int y0, int w1, int w2, int w3, int w4, int h, int ind) {

  FL_OBJECT *obj;
  int i;
  int x, y, xx;
  int wa, wb;
  char *DAC_name[4] = {"14 bit", "AUX", "OVV lim", "OVC lim"};
  
  wa = 20;
  wb = 15;

  x = x0;
  y = y0;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w1,h,DAC_name[ind]);
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x + w1;

  xx = x;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,wa,h/2,"0x");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x + wa;

  for (i=0; i<4; i++) {
    DAC_WRI_DIGIT[ind][i] =
    obj = fl_add_button(FL_NORMAL_BUTTON,x,y,wb,h/2,"");
      fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
      fl_set_object_color(obj,FL_WHEAT,FL_RED);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
      fl_set_object_callback(obj, DAC_digit_callback, 10*ind+i);
    x = x + wb;
  }


  DAC_INPUT[ind] = 
  obj = fl_add_input(FL_NORMAL_INPUT,x,y,w3,h/2,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_WHEAT);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE+FL_BOLD_STYLE);
    fl_set_input_color(obj,FL_BLACK,FL_BLACK);
    fl_set_input_return(obj,FL_RETURN_ALWAYS);
    fl_set_object_callback(obj, DAC_input_callback, ind);
  obj = fl_add_button(FL_HIDDEN_BUTTON,x,y,w3,h/2,"for focus");
    fl_set_object_callback(obj, DAC_input_focus_callback, ind);

  x = x + w3;
  
  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w4,h/2,"WRITE");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, WRITE_DAC_callback, ind);
    
  x = xx;
  y = y + h/2;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,wa,h/2,"0x");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x + wa;

  for (i=0; i<4; i++) {
    DAC_REA_DIGIT[ind][i] =
    obj = fl_add_button(FL_NORMAL_BUTTON,x,y,wb,h/2,"");
      fl_set_object_boxtype(obj,FL_FRAME_BOX);
      fl_set_object_color(obj,FL_GREEN,FL_MCOL);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_BOLD_STYLE);
    x = x + wb;
  }

  DAC_REA_VALUE[ind] =
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w3,h/2,"");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_GREEN,FL_GREEN);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_BOLD_STYLE);

  x = x + w3;

  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w4,h/2,"READ");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, READ_DAC_callback, ind);
}

//~-------------------------------------------------------------------------------

void DAC_digit_callback(FL_OBJECT *obj, long data) {

  int d[4];
  int DAC = data / 10;
  int i   = data % 10;
  int16 k;

  k = DAC_wri_code[DAC];
  
  d[0] = unpack16(k, 0xF000);
  d[1] = unpack16(k, 0x0F00);
  d[2] = unpack16(k, 0x00F0);
  d[3] = unpack16(k, 0x000F);

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  d[i] = MOD(d[i]+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) d[i] = MOD(d[i]-1, 16);

  pack16(&k, 0xF000, d[0]);
  pack16(&k, 0x0F00, d[1]);
  pack16(&k, 0x00F0, d[2]);
  pack16(&k, 0x000F, d[3]);

  DAC_wri_code[DAC] = k;

  DAC_wri_value[DAC] = convert_DAC_to_value(DAC_wri_code[DAC], DAC);

  DAC_input_active = -1;
      
  update_objects();
}

//~-------------------------------------------------------------------------------

void DAC_input_focus_callback(FL_OBJECT *obj, long data) {

  int DAC = data;

  if (DAC_input_active != DAC) {
    DAC_input_changed = 1;
    DAC_input_active = DAC;
  }

  update_objects();
}

//~-------------------------------------------------------------------------------

void DAC_input_callback(FL_OBJECT *obj, long data) {

  int DAC = data;

  DAC_wri_value[DAC] = atof(fl_get_input(DAC_INPUT[DAC]));
  
  DAC_wri_code[DAC] = convert_DAC_to_code(DAC_wri_value[DAC], DAC);
  DAC_wri_value[DAC] = convert_DAC_to_value(DAC_wri_code[DAC], DAC);
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void WRITE_DAC_callback(FL_OBJECT *obj, long data) {

  int DAC = data;
  int32 cod = 0xA0080000;
  int i, p;

  pack32(&cod, 0x0FF00000, chip_adr);
  pack32(&cod, 0x00070000, DAC);
//  pack(&k, DAC_mask[DAC], DAC_wri_code[DAC]);
//  pack32(&cod, 0x0000FFFF, unpack(DAC_wri_code[DAC]), DAC_mask[DAC]);
  pack32(&cod, (int32)DAC_mask[DAC], DAC_wri_code[DAC]);
  
  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    FL_OBJECT *obj = WRI[i];
    if (cod & bit) fl_set_button(obj, 1);
    else           fl_set_button(obj, 0);
    if (cod & bit && i != 1) p ^= 1;
  }  

  if (p) fl_set_button(WRI[1], 1);
  
  update_objects();

  fl_call_object_callback(SEND_BUTTON);
  
  if (DAC_rea_sta[DAC] != status_NN) DAC_rea_sta[DAC] = status_NA;

  if (sta && !chk) {
    DAC_wri_sta[DAC] = status_OK;
    DAC_wri_was[DAC] = DAC_wri_code[DAC];
  }
  else {
    DAC_wri_sta[DAC] = status_KO;
  }

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void READ_DAC_callback(FL_OBJECT *obj, long data) {

  int DAC = data;
  int32 cod = 0xA0000000;
  int i, p;

  pack32(&cod, 0x0FF00000, chip_adr);
  pack32(&cod, 0x00070000, DAC);
  
  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    FL_OBJECT *obj = WRI[i];
    if (cod & bit) fl_set_button(obj, 1);
    else           fl_set_button(obj, 0);
    if (cod & bit && i != 1) p ^= 1;
  }  

  if (p) fl_set_button(WRI[1], 1);
  
  update_objects();

  fl_call_object_callback(SEND_BUTTON);

  if (sta && !chk) {
    DAC_rea_sta[DAC] = status_OK;
    if (DAC_wri_sta[DAC] == status_OK) DAC_rea_was[DAC] = DAC_wri_was[DAC];
    DAC_rea_code[DAC] = unpack32(rea, (int32)DAC_mask[DAC]);
//    DAC_rea_code[DAC] = invert_bit_order((int16)unpack32(rea, 0x0000FFFF));
    DAC_rea_value[DAC] = convert_DAC_to_value(DAC_rea_code[DAC], DAC);
  }
  else {
    if (DAC_rea_sta[DAC] != status_NN) DAC_rea_sta[DAC] = status_KO;
  }

  msg = 1;  
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_DAC_panels(void) {

  FL_OBJECT *obj;
  int i, j;
  char b[80];

//~-- DAC Input Code ---

  for (i=0; i<4; i++) {
    int16 mask[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
    for (j=0; j<4; j++) {
      int d_now = unpack16(DAC_wri_code[i], mask[j]);
      int d_was = unpack16(DAC_wri_was[i], mask[j]);
      obj = DAC_WRI_DIGIT[i][j];
      sprintf(b, "%01X", d_now);
      fl_set_object_label(obj, b);
      if (DAC_wri_sta[i] == status_OK && d_now == d_was) {
        fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      }
      else {
        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      }
    }
  }

//~-- DAC Input Value ("Input") ---

  if (DAC_input_active == -1) {
    for (i=0; i<4; i++) {
      obj = DAC_INPUT[i];
      if (UHVG->UHVG->focusobj == obj) UHVG->UHVG->focusobj = NULL;
      fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
      if (DAC_wri_sta[i] == status_OK && DAC_wri_code[i] == DAC_wri_was[i]) {
        fl_set_input_color(obj, FL_BLACK, FL_GREEN);
      }
      else {
        fl_set_input_color(obj, FL_BLACK, FL_YELLOW);
      }
    }
  }
  else {
    for (i=0; i<4; i++) {
      obj = DAC_INPUT[i];
      if (UHVG->UHVG->focusobj == obj && DAC_input_active != i) {
        DAC_input_changed = 1;
      }
    }
    if (DAC_input_changed) {
      for (i=0; i<4; i++) {
        obj = DAC_INPUT[i];
        if (UHVG->UHVG->focusobj == obj) {
          fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
          if (DAC_wri_sta[i] == status_OK && DAC_wri_code[i] == DAC_wri_was[i]) {
            fl_set_input_color(obj, FL_BLACK, FL_GREEN);
          }
          else {
            fl_set_input_color(obj, FL_BLACK, FL_YELLOW);
          }
          UHVG->UHVG->focusobj = NULL;
        }
      }
      obj = DAC_INPUT[DAC_input_active];
      UHVG->UHVG->focusobj = obj;
      fl_set_object_boxtype(obj, FL_DOWN_BOX);
      fl_set_input_color(obj, FL_BLACK, FL_BLACK);
      fl_set_input_cursorpos(obj, 100, 0);
    }
  }
  
  for (i=0; i<4; i++) {
    obj = DAC_INPUT[i];
    if (DAC_input_active != i || DAC_input_changed) {
      sprintf(b, "%7.1f", DAC_wri_value[i]);
      fl_set_input(obj, b);
    }
    if (DAC_wri_sta[i] == status_OK && DAC_wri_code[i] == DAC_wri_was[i]) {
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    }
    else {
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
  }

  DAC_input_changed = 0;
  
//~-- DAC Read Code ---

  for (i=0; i<4; i++) {
    int16 mask[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
    for (j=0; j<4; j++) {
      int d_rea = unpack16(DAC_rea_code[i], mask[j]);
      int d_was = unpack16(DAC_rea_was[i], mask[j]);
      obj = DAC_REA_DIGIT[i][j];
      if (DAC_rea_sta[i] == status_NA || DAC_rea_sta[i] == status_NN) {
        fl_set_object_lcolor(obj, FL_BLACK);
        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      }
      if (DAC_rea_sta[i] == status_KO) {
        fl_set_object_lcolor(obj, FL_WHITE);
        fl_set_object_color(obj, FL_BLACK, FL_BLACK);
      }
      if (DAC_rea_sta[i] == status_OK) {
        fl_set_object_lcolor(obj, FL_BLACK);
        if (DAC_wri_sta[i] == status_OK && d_rea != d_was) {
          fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
        }
        else {
          fl_set_object_color(obj, FL_GREEN, FL_GREEN);
        }
      }
      if (DAC_rea_sta[i] == status_NN) sprintf(b, " ");
      else                             sprintf(b, "%01X", d_rea);
      fl_set_object_label(obj, b);
    }
  }

//~-- DAC Read Value ---

  for (i=0; i<4; i++) {
    int16 rea = DAC_rea_code[i];
    int16 was = DAC_rea_was[i];
    obj = DAC_REA_VALUE[i];
    if (DAC_rea_sta[i] == status_NA || DAC_rea_sta[i] == status_NN) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    if (DAC_rea_sta[i] == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, FL_BLACK);
    }
    if (DAC_rea_sta[i] == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      if (DAC_wri_sta[i] == status_OK && rea != was) {
        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      }
      else {
        fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      }
    }
    if (DAC_rea_sta[i] == status_NN) sprintf(b, " ");
    else                             sprintf(b, "%7.1f", DAC_rea_value[i]);
    fl_set_object_label(obj, b);
  }
}

//~===============================================================================

void create_ADC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, h, w1, w2, w3, w4, w5, w6, w7;
  int i;

  ADC_sta = status_NN;
  
  h  =  50;
  w1 =  65;
  w2 =  50;
  w3 = 165;
  w4 =  20;
  w5 =  15;
  w6 =  75;
  w7 =  60;
  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,290,60,"");
  
  y = y0 + 5;

  x = x0 + 5;

  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w1,h,"ADC");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x0 + 5 + w1;

  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w2,h/2,"Input:");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  x = x0 + 5 + w1 + w2;

  ADC_INP = 
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w3,h/2,"Temperature");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_MCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  y = y0 + 5 + h/2;

  x = x0 + 5 + w1;
  
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w4,h/2,"0x");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  for (i=0; i<4; i++) {
    x = x0 + 5 + w1 + w4 + w5 * i;
    ADC_DIGIT[i] =
    obj = fl_add_text(FL_NORMAL_TEXT,x,y,w5,h/2,"F");
      fl_set_object_boxtype(obj,FL_FRAME_BOX);
      fl_set_object_color(obj,FL_GREEN,FL_MCOL);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_BOLD_STYLE);
  }
    
  x = x0 + 5 + w1 + w4 + w5 * 4;

  ADC_VALUE =
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,w6,h/2,"1234.5");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_GREEN,FL_MCOL);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_BOLD_STYLE);
    
  x = x0 + 5 + w1 + w4 + w5 * 4 + w6;
  
  ADC_READ = 
  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,w7,h/2,"READ");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj, READ_ADC_callback, 0);
}

//~-------------------------------------------------------------------------------

void READ_ADC_callback(FL_OBJECT *obj, long data) {

  int32 cod = 0xA0000000;
  int16 pgm = 0xFFFF;
  int i, p;

  pack32(&cod, 0x0FF00000, chip_adr);
  pack32(&cod, 0x00070000, 0x7);
  
  p = 1;
  for (i=0; i<32; i++) {
    int32 bit = 1 << (31 - i);
    FL_OBJECT *obj = WRI[i];
    if (cod & bit) fl_set_button(obj, 1);
    else           fl_set_button(obj, 0);
    if (cod & bit && i != 1) p ^= 1;
  }  

  if (p) fl_set_button(WRI[1], 1);
  
  update_objects();

  fl_call_object_callback(SEND_BUTTON);

  if (sta && !chk) {
    ADC_sta = status_OK;
    ADC_code = invert_bit_order((int16)unpack32(rea, 0x0000FFFF));
    if (pgm_rea_sta == status_OK) {
      pgm = pgm_rea_cod;
    }
    else {
      if (pgm_wri_sta == status_OK) {
        pgm = pgm_wri_was;
      }
      else {
        ADC_sta = status_NA;
      }
    }
    if (ADC_sta == status_OK) {
      ADC = unpack16(pgm, 0x0380);
      res = unpack16(pgm, 0x0C00);
      ADC_value = convert_ADC_to_value(ADC_code, ADC, res);
    }
  }
  else {
    if (ADC_sta != status_NN) ADC_sta = status_KO;
  }

  msg = 1;  
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_ADC_panel(void) {

  FL_OBJECT *obj;
  int i;
  int16 mask[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
  char *ADC_inp[8] = {
  "14 bit DAC", "Temperature", "OVC DAC",    "OVV DAC", 
  "AUX DAC",     "HV current", "HV voltage", "Reference"};
  char b[80];
  
//~-- ADC READ button ---

  obj = ADC_READ;
  
  if (pgm_rea_sta != status_OK && pgm_wri_sta != status_OK) {
    fl_set_object_color(obj,FL_MCOL,FL_MCOL);
    fl_set_object_lcolor(obj, FL_MCOL);
    fl_deactivate_object(obj);
  }
  else {
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_activate_object(obj);
  }

//~-- ADC Input ---

  obj = ADC_INP;
  if (ADC_sta == status_NA || ADC_sta == status_NN) {
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  }
  if (ADC_sta == status_KO) {
    fl_set_object_lcolor(obj, FL_WHITE);
    fl_set_object_color(obj, FL_BLACK, FL_BLACK);
  }
  if (ADC_sta == status_OK) {
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_color(obj, FL_GREEN, FL_GREEN);
  }
  if (ADC_sta == status_NN) sprintf(b, " ");
  else                      sprintf(b, "%s", ADC_inp[ADC]);
  fl_set_object_label(obj, b);

//~-- ADC Read Code ---

  for (i=0; i<4; i++) {
    obj = ADC_DIGIT[i];
    if (ADC_sta == status_NA || ADC_sta == status_NN) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    if (ADC_sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, FL_BLACK);
    }
    if (ADC_sta == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    }
    if (ADC_sta == status_NN) sprintf(b, " ");
    else                      sprintf(b, "%01X", unpack16(ADC_code, mask[i]));
    fl_set_object_label(obj, b);
  }

//~-- ADC Read Value ---

  obj = ADC_VALUE;
  if (ADC_sta == status_NA || ADC_sta == status_NN) {
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
  }
  if (ADC_sta == status_KO) {
    fl_set_object_lcolor(obj, FL_WHITE);
    fl_set_object_color(obj, FL_BLACK, FL_BLACK);
  }
  if (ADC_sta == status_OK) {
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_color(obj, FL_GREEN, FL_GREEN);
  }
  if (ADC_sta == status_NN) sprintf(b, " ");
  else                      sprintf(b, "%7.1f", ADC_value);
  fl_set_object_label(obj, b);
}

//~===============================================================================

void update_objects(void) {

  update_USCM_JMDC_panels();
  update_line_panel();
  update_binary_code_panel();
  update_message();
  update_chip_address_panel();
  update_HV_dis_panel();
  update_chip_program_panel();
  update_DAC_panels();
  update_ADC_panel();

  fl_check_forms();

  if (P > 1) {
    printf("USCM_ID = 0x%02X = %3d JMDC = %1d\n", USCM_ID, USCM_ID, JMDC_ID);
    printf("clk = %02X dat = %04X wri = %08X rea = %08X\n", 
            clk,       dat,       wri,       rea);
    printf("msg = %d sta = %d chk = %d\n", msg, sta, chk);
  }
}

//~-------------------------------------------------------------------------------

void start_UHVG(void) {

  FILE *file;
  int one = 1;
  static char *title = {"UHVG Controller"};
  char *adr[] = {(char*)&title};

  if (!initialize_EPP()) exit(1);

  file = fopen("UHVG.configfile", "r");
  if (file != NULL) {
    fread(&USCM_ID, sizeof(USCM_ID), 1, file);
    fread(&JMDC_ID, sizeof(JMDC_ID), 1, file);
    fclose(file);
  }
  
  fl_initialize(&one, adr, " ", 0, 0);
  UHVG = create_form_UHVG();
  fl_set_form_position(UHVG->UHVG, 0, 0);
  fl_show_form(UHVG->UHVG, FL_PLACE_GEOMETRY, FL_FULLBORDER, "UHVG");

  update_objects();
}

//~============================================================================

int main(int argc, char *argv[]) {

  printf("Usage: UHVG [<Debug Printout Level>]\n");

  if (argc > 1) P = atoi(argv[1]);

  printf("P = %d\n", P);

  setbuf(stdout, NULL);	// set "no buffering" for stdout stream

  start_UHVG();

  while(1) {
    fl_do_forms();
  }
  return 0;
}

//~============================================================================
