// file HV.c
//
// HV Controller for UHVG module
//
// A.Lebedev Sep-2003...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"HV  Pro  Controller"};                // global variable
char *date  = {"13-Dec-08"};                          // global variable

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

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

//~---

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

#define TIMER_PERIOD 1.0     // in seconds

struct _map {
  int   brd;
  int   clk;
  int   dat;
  char *slt;
  char *hlf;
};

#ifdef USE_CAN
struct _map map[7][2] = {
//
//~brd-clk-dat-slot--half
 {{ 0,  4,  4, "UG", "A"},
  { 0,  5,  5, "UG", "B"}},
 {{ 0,  0,  0, "16", "A"},
  { 0,  1,  1, "16", "B"}},
 {{ 0,  0,  2, "17", "A"},
  { 0,  1,  3, "17", "B"}},
 {{ 0,  0,  4, "18", "A"},
  { 0,  1,  5, "18", "B"}},
 {{ 0,  2,  6, "19", "A"},
  { 0,  3,  7, "19", "B"}},
 {{ 0,  2,  8, "20", "A"},
  { 0,  3,  9, "20", "B"}},
 {{ 0,  2, 10, "21", "A"},
  { 0,  3, 11, "21", "B"}}
};
#endif

#ifdef USE_AMSW
struct _map map[6][2] = {
//
//~brd-clk-dat-slot--half
 {{ 0,  1,  1, "11", "A"},
  { 0,  5,  5, "11", "B"}},
 {{ 1,  1,  1, "12", "A"},
  { 1,  5,  5, "12", "B"}},
 {{ 2,  1,  1, "13", "A"},
  { 2,  5,  5, "13", "B"}},
 {{ 0,  0,  0, "14", "A"},
  { 0,  6,  6, "14", "B"}},
 {{ 1,  0,  0, "15", "A"},
  { 1,  6,  6, "15", "B"}},
 {{ 2,  0,  0, "16", "A"},
  { 2,  6,  6, "16", "B"}}
};
#endif

int nslot = sizeof(map) / sizeof(struct _map) / 2;

FL_OBJECT *SLOT, *HALF, *CHIP;
FL_OBJECT *CLK, *DAT, *BRD[5], *CHP[3];
int slot, half, chip, brd, clk, dat, chp;

SW_BTN *FRANCESCA[1];
_w Francesca;
char *F_modes[2] = {"Normal\nMode", "Francesca\nMode"};

SW_BTN *ALESSANDRO[1];
_w Alessandro;
char *A_modes[2] = {"Normal\nMode", "Alessandro\nMode"};

FL_OBJECT *WRI[32], *WAS[32], *REA[32];
FL_OBJECT *R_W, *SECTION;
FL_OBJECT *SEND_BUTTON, *STEP_BUTTON;

FL_OBJECT *BOOT_MODE;
int boot_mode = 2;

FL_OBJECT *BROADCAST, *POWER;
int broadcast = 0;
int power = 1;

FL_OBJECT *HV_REA, *READ_HV;

FL_OBJECT *STA_DIGIT[5], *STA_GET_BUTTON;
int sta_monitoring;

FL_OBJECT *DAC_WRI_DIGIT[4][4], *DAC_INPUT[4], *DAC_WRI_VALUE[4];
FL_OBJECT *DAC_REA_DIGIT[4][4], *DAC_REA_VALUE[4];

FL_OBJECT *ADC_DIGIT[8][4], *ADC_VALUE[8], *ADC_GET_BUTTON[8];

FL_OBJECT *PGM_WRITE;

int32 wri, rea;
int32 wri_was_cod;
int wri_was_sta;

int HV_wri_sta, HV_rea_sta;
int16 HV_wri_cod, HV_wri_was, HV_rea_cod, HV_rea_was;

int chip_sta_sta;
int16 chip_sta_cod;

int DAC_input_active;

int DAC_wri_sta[4], DAC_rea_sta[4];
int16 DAC_wri_cod[4], DAC_wri_was[4], DAC_rea_cod[4], DAC_rea_was[4];
float DAC_wri_val[4], DAC_rea_val[4];

int ADC_sta[8];
int16 ADC_cod[8];
float ADC_val[8];

int ADC_monitoring[8];

struct pgm_selector {
  char *label;
  int16 msk;
  int n;
  struct {
    char *nam;
    int16 cod;
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

int16 pgm_default = 0x3840;
int pgm_wri_sta, pgm_rea_sta;
int16 pgm_wri_cod, pgm_wri_was, pgm_rea_cod, pgm_rea_was;

FL_OBJECT *USCM_TMP_VALUE, *USCM_TMP_GET_BUTTON;
int64 USCM_Dallas_ID;
int   USCM_tmp_unk;
int   USCM_tmp_sta;
float USCM_tmp_val;
int   USCM_tmp_monitoring;

int16 max_DAC_cod[4] = {0x3FFF, 0x00FF, 0x003F, 0x003F};
int16 DAC_msk[4]     = {0xFFFC, 0xFF00, 0xFC00, 0xFC00};

int16 max_ADC_cod[4] = {0x1FFF, 0x7FFF, 0x3FFF, 0xFFFF};

#ifdef USE_CAN
float Vref = 4.30;
float Vdd  = 5.06;

float A   = 506.0;
float B   =   0.0;
float C   =   0.1065;
float K   =   0.020;
float R80 = 100.2;

float V14  = 1.40;
float V19  = 1.90;
float V179 = 0.179;
#endif

#ifdef USE_AMSW
float Vref = 4.096;
float Vdd  = 5.06;

float A   = 506.0;
float B   =   0.0;
float C   =   0.1065;
float K   =   0.020;
float R80 = 100.2;

float V14  = 1.40 / 4.30 * 4.096;
float V19  = 1.90 / 4.30 * 4.096;
float V179 = 0.179;
#endif

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int16 convert_DAC_to_code(float val, int16 DAC);
float convert_DAC_to_value(int16 DAC_cod, int16 DAC);
float convert_ADC_to_value(int16 ADC_cod, int ADC, int res);

void read__MHV_chip(int16 chip_address, int16 section, int16 *data);
void write__MHV_chip(int16 chip_address, int16 section, int16 data);
void read__DAC(int16 chip_address, int16 DAC);
void write__DAC(int16 chip_address, int16 DAC);
void read__chip_program(int16 chip_address);
void write__chip_program(int16 chip_address);
void read__HV_status(int16 chip_address);
void write__HV_status(int16 chip_address);
void read__chip_status(int16 chip_address);
void read__ADC(int16 chip_address, int16 ADC);
void get__ADC(int16 chip_address, int16 ADC);

void create_main_form(void);

void push_button_callback(FL_OBJECT *obj, long data);
void invalidate_data(void);

void create_hi_lvl_adr_panel(int x0, int y0);
void slot_callback(FL_OBJECT *obj, long data);
void half_callback(FL_OBJECT *obj, long data);
void chip_callback(FL_OBJECT *obj, long data);
void update_hi_lvl_adr_panel(void);

void create_lo_lvl_adr_panel(int x0, int y0);
void brd_callback(FL_OBJECT *obj, long data);
void chp_callback(FL_OBJECT *obj, long data);
void clk_callback(FL_OBJECT *obj, long data);
void dat_callback(FL_OBJECT *obj, long data);
void update_lo_lvl_adr_panel(void);

void INIT_callback(FL_OBJECT *obj, long data);

void create_Francesca_panel(int x0, int y0);
void create_Alessandro_panel(int x0, int y0);

void create_ADC_DAC_panels(int x0, int y0);
void create_ADC_panel(
     int x0, int y0, int w1, int w2, int w3, int h, int ADC);
void READ_ADC_callback(FL_OBJECT *obj, long data);
void create_DAC_panel(
     int x0, int y0, int w1, int w2, int w3, int h, int DAC);
void activate_DAC_input(int DAC, int mode);
void DAC_digit_callback(FL_OBJECT *obj, long data);
void DAC_input_focus_callback(FL_OBJECT *obj, long data);
void DAC_input_callback(FL_OBJECT *obj, long data);
void WRITE_DAC_callback(FL_OBJECT *obj, long data);
void READ_DAC_callback(FL_OBJECT *obj, long data);
void update_ADC_DAC_panels(void);

void create_HV_dis_panel(int x0, int y0);
void WRITE_HV_dis_callback(FL_OBJECT *obj, long data);
void READ_HV_dis_callback(FL_OBJECT *obj, long data);
void update_HV_dis_panel(void);

void create_chip_status_panel(int x0, int y0);
void READ_chip_status_callback(FL_OBJECT *obj, long data);
void update_chip_status_panel(void);

void create_USCM_tmp_panel(int x0, int y0);
void read_USCM_tmp_data();
void USCM_tmp_callback(FL_OBJECT *obj, long data);
void update_USCM_tmp_panel();

void create_broadcast_power_panel(int x0, int y0);
void BROADCAST_callback(FL_OBJECT *obj, long data);
void POWER_callback(FL_OBJECT *obj, long data);
void update_broadcast_power_panel(void);

void create_chip_program_panel(int x0, int y0);
void create_chip_program_selector(int x0, int y0, int w, int ind);
void pgm_default_callback(FL_OBJECT *obj, long data);
void pgm_selector_callback(FL_OBJECT *obj, long data);
void pgm_digit_callback(FL_OBJECT *obj, long data);
void PGM_WRITE_callback(FL_OBJECT *obj, long data);
void PGM_READ_callback(FL_OBJECT *obj, long data);
void update_chip_program_panel(void);

void create_LeCroy_bus_code_panel(int x0, int y0);
void wri_callback(FL_OBJECT *obj, long data);
void SEND_callback(FL_OBJECT *obj, long data);
void update_LeCroy_bus_code_panel(void);

void TIMER_callback(FL_OBJECT *obj, long data);
void read_all_data(void);

void update_objects(void);

//~============================================================================

int16 convert_DAC_to_code(float val, int16 DAC) {

  float R;

  switch (DAC) {
    case 0: R = (val / Vref + B) / A; break;
    case 1: R =  val / Vdd;           break;
    case 2: R = (val / Vdd  + B) / A; break;
    case 3: R =  val * K / Vdd + C;   break;
    default:                          return 0;
  }
  return (int16) (R * (float)max_DAC_cod[DAC]);
}

//~----------------------------------------------------------------------------

float convert_DAC_to_value(int16 DAC_cod, int16 DAC) {

  float val;
  float R;

  R = (float)DAC_cod / (float)max_DAC_cod[DAC];

  switch (DAC) {
    case 0: val = Vref * (A * R - B); break;
    case 1: val = Vdd  * R;           break;
    case 2: val = Vdd  * (A * R - B); break;
    case 3: val = Vdd  / K * (R - C); break;
    default:                          return 0.0;
  }

  return val;
}

//~----------------------------------------------------------------------------

float convert_ADC_to_value(int16 ADC_cod, int ADC, int res) {

  float v;
  float R;

  R = (float)ADC_cod / (float)max_ADC_cod[res];

  switch (ADC) {
    case 0: v =  Vref * (A * R - B);                                  break;
    case 1: v = (Vref * R - V14 + V179) * 100.0 / (V19 - V14) - 50.0; 
//printf("ADC_cod=%04X Vref=%f R=%f V14=%f V179=%f V19=%f v=%f\n",
//        ADC_cod,     Vref,   R,   V14,   V179,   V19,   v);

break;
    case 2: v =  Vref / K * (R - C);                                  break;
    case 3: v =  Vref * (A * R - B);                                  break;
    case 4: v =  Vref * R;                                            break;
    case 5: v =  Vref / K * (R - C);                                  break;
    case 6: v =  Vref * (A * R - B);                                  break;
    case 7: v =  Vref * R * 1000.0;                                   break;
    default:                                                          return 0.0;
  }

  return v;
}

//~============================================================================

void read__MHV_chip(int16 chip_address, int16 section, int16 *data) {

  int32 m;
  int p;
  int8 bus;

  wri = 0x80000000;
  pack32(&wri, 0x20000000, power);
  pack32(&wri, 0x10000000, broadcast);
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00700000, chip_address);
  pack32(&wri, 0x00070000, section);
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;

  err = 0x0000;
  chk = 1;

#ifdef USE_CAN
  bus = (clk<<4) | dat;
#endif

#ifdef USE_AMSW
  bus = dat;
#endif

  if (Francesca.inp) init_LeCroy_bus(bus, &err);

  if (!err) {
    exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);
    if (!err && !chk) {
      *data = unpack32(rea, 0x0000FFFF);
      if (Alessandro.inp) *data &= 0xFFFE;
    }
    wri_was_sta = status_OK;
    wri_was_cod = wri;
  }
}

//~----------------------------------------------------------------------------

void write__MHV_chip(int16 chip_address, int16 section, int16 data) {

  int32 m;
  int p;
  int8 bus;

  wri = 0x80080000;
  pack32(&wri, 0x20000000, power);
  pack32(&wri, 0x10000000, broadcast);
  pack32(&wri, 0x0F800000, brd);
  pack32(&wri, 0x00700000, chip_address);
  pack32(&wri, 0x00070000, section);
  pack32(&wri, 0x0000FFFF, data);
  
  for (m=0x80000000,p=1; m; m>>=1) if (wri & m) p ^= 1;
  if (p) wri ^= 0x40000000;
  
  err = 0x0000;
  chk = 1;

#ifdef USE_CAN
  bus = (clk<<4) | dat;
#endif

#ifdef USE_AMSW
  bus = dat;
#endif

  if (Francesca.inp) init_LeCroy_bus(bus, &err);

  if (!err) {
    exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);
    wri_was_sta = status_OK;
    wri_was_cod = wri;
  }
}

//~----------------------------------------------------------------------------

void read__DAC(int16 chip_address, int16 DAC) {

  int16 data;
  
  read__MHV_chip(chip_address, DAC, &data);

  if (!err && !chk) {
    DAC_rea_sta[DAC] = status_OK;
    if (DAC_wri_sta[DAC] == status_OK) DAC_rea_was[DAC] = DAC_wri_was[DAC];
    DAC_rea_cod[DAC] = unpack16(data, DAC_msk[DAC]);
    DAC_rea_val[DAC] = convert_DAC_to_value(DAC_rea_cod[DAC], DAC);
  }
  else {
    if (DAC_rea_sta[DAC] != status_NN) DAC_rea_sta[DAC] = status_KO;
  }
}

//~----------------------------------------------------------------------------

void write__DAC(int16 chip_address, int16 DAC) {

  int16 data;

  data = 0x0000;
  pack16(&data, DAC_msk[DAC], DAC_wri_cod[DAC]);
  
  write__MHV_chip(chip_address, DAC, data);
  
  if (DAC_rea_sta[DAC] != status_NN) DAC_rea_sta[DAC] = status_NA;

  if (!err && !chk) {
    DAC_wri_sta[DAC] = status_OK;
    DAC_wri_was[DAC] = DAC_wri_cod[DAC];
  }
  else {
    DAC_wri_sta[DAC] = status_KO;
  }
}

//~----------------------------------------------------------------------------

void read__chip_program(int16 chip_address) {
  
  int16 data;

  read__MHV_chip(chip_address, 4, &data);

  if (!err && !chk) {
    pgm_rea_sta = status_OK;
    if (pgm_wri_sta == status_OK) pgm_rea_was = pgm_wri_was;
    pgm_rea_cod = data;
  }
  else {
    if (pgm_rea_sta != status_NN) pgm_rea_sta = status_KO;
  }
}
  
//~----------------------------------------------------------------------------

void write__chip_program(int16 chip_address) {
  
  write__MHV_chip(chip_address, 4, pgm_wri_cod);
  
  if (!err && !chk) {
    pgm_wri_sta = status_OK;
    pgm_wri_was = pgm_wri_cod;
    if (pgm_rea_sta != status_NN) pgm_rea_sta = status_NA;
  }
  else {
    pgm_wri_sta = status_KO;
  }
}

//~----------------------------------------------------------------------------

void read__HV_status(int16 chip_address) {

  int16 data;

  read__MHV_chip(chip_address, 5, &data);

  if (!err && !chk) {
    HV_rea_sta = status_OK;
    if (HV_wri_sta == status_OK) HV_rea_was = HV_wri_was;
    HV_rea_cod = data;
  }
  else {
    if (HV_rea_sta != status_NN) HV_rea_sta = status_KO;
  }
}

//~----------------------------------------------------------------------------

void write__HV_status(int16 chip_address) {
  
  write__MHV_chip(chip_address, 5, HV_wri_cod);
  
  if (!err && !chk) {
    HV_wri_sta = status_OK;
    HV_wri_was = HV_wri_cod;
    if (HV_rea_sta != status_NN) HV_rea_sta = status_NA;
  }
  else {
    HV_wri_sta = status_KO;
  }
}

//~----------------------------------------------------------------------------

void read__chip_status(int16 chip_address) {

  int16 data;

  read__MHV_chip(chip_address, 6, &data);

  if (!err && !chk) {
    chip_sta_sta = status_OK;
    chip_sta_cod = data;
  }
  else {
    if (chip_sta_sta != status_NN) chip_sta_sta = status_KO;
  }
}

//~----------------------------------------------------------------------------

void read__ADC(int16 chip_address, int16 ADC) {

  int16 data;
  int16 res;

  read__MHV_chip(chip_address, 7, &data);

  if (!err && !chk) {
    res = unpack16(pgm_wri_cod, 0x0C00);
    ADC_sta[ADC] = status_OK;
    ADC_cod[ADC] = invert_bit_order(data);
    ADC_val[ADC] = convert_ADC_to_value(ADC_cod[ADC], ADC, res);
  }
  else {
    if (ADC_sta[ADC] != status_NN) ADC_sta[ADC] = status_KO;
  }
}

//~----------------------------------------------------------------------------

void get__ADC(int16 chip_address, int16 ADC) {

  static struct timeval now, then;  
  int16 frq, res;
  int32 delay, sec, usec;
  int ADC_conversion_time_Table[4][4] = {  // thank you, Mr. LeCroy!
    {328, 1300,  656, 2620},
    { 82,  328,  164,  656},
    {164,  656,  328, 1300},
    { 41,  164,   82,  328}};

  pack16(&pgm_wri_cod, 0x0380, ADC);
  
  write__chip_program(chip_address);
  if (err || chk) {
    if (ADC_sta[ADC] != status_NN) ADC_sta[ADC] = status_KO;
    return;
  }

  write__chip_program(chip_address);        // yes, twice!
  if (err || chk) {
    if (ADC_sta[ADC] != status_NN) ADC_sta[ADC] = status_KO;
    return;
  }
  
  frq = unpack16(pgm_wri_cod, 0x3000);
  res = unpack16(pgm_wri_cod, 0x0C00);
  delay = ADC_conversion_time_Table[frq][res];
  delay = 1000 * delay;
  sec  = delay / 1000000;
  usec = delay % 1000000;

  gettimeofday(&then, NULL);
  sleep(sec);
  usleep(usec);
  gettimeofday(&now, NULL);
  if (P > 2) printf("frq=%1X res=%1X delay=%.0fms\n", 
                     frq, res, 1000.0*delta_t(&now, &then));

  read__ADC(chip_address, ADC);
}

//~============================================================================

void create_main_form(void) {

  int WW = 600, HH = 615;

  invalidate_data();
  Francesca.inp  = 0;
  Alessandro.inp = 0;
  
#ifdef USE_CAN
  bgn_form(&MAIN, 0, 0, WW, HH);
#endif
#ifdef USE_AMSW
  bgn_form(&MAIN, 0, 0, WW, HH, "JINF Path");
#endif
  
//AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
//SW_BTN_CALLBACK            = switch_btn_callback;

  create_hi_lvl_adr_panel(       0,  45);
  create_lo_lvl_adr_panel(     100,  45);
  create_ADC_DAC_panels(         0,  95);
  create_HV_dis_panel(         255, 315);
  create_chip_status_panel(    255, 365);
#ifdef USE_CAN
  create_USCM_tmp_panel(       255, 435);
#endif
  create_broadcast_power_panel(405,  95);
#ifdef USE_CAN
  create_Francesca_panel(      400,  45);
  create_Alessandro_panel(     495,  45);
  create_USCM_control_V_panel( 405, 165);
#endif
  create_chip_program_panel(   485,  95);
  create_LeCroy_bus_code_panel(  0, 465);
  
  end_form(MAIN);
  update_objects();
}

//~----------------------------------------------------------------------------

void push_button_callback(FL_OBJECT *obj, long data) {

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;

  Francesca.inp  = 0;
  Alessandro.inp = 0;
  
  HV_wri_sta = status_NN;
  HV_rea_sta = status_NN;

  chip_sta_sta = status_NN;

  for (i=0; i<4; i++) {
    DAC_wri_sta[i] = status_NN;
    DAC_rea_sta[i] = status_NN;
  }

  for (i=0; i<8; i++) {
    ADC_sta[i] = status_NN;
  }
  
  pgm_wri_sta = status_NN;
  pgm_rea_sta = status_NN;
  
//for (i=0; i<8; i++) ADC_monitoring[i] = 0;
  
//sta_monitoring = 0;
  
  USCM_tmp_unk = 1;
  USCM_tmp_sta = status_NA;
//USCM_tmp_monitoring = 0;
}

//~============================================================================

void create_hi_lvl_adr_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, size, w, h, w1 = 30, h1 = 20;

  slot = 0;
  half = 0;
  chip = 0;

  brd = map[slot][half].brd;
  clk = map[slot][half].clk;
  dat = map[slot][half].dat;

  w = 5 + 3 * w1 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  size = FL_SMALL_SIZE;
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Slot", size);

  x = x + w1;
  obj = add_frame_box(x, y, w1, h1, "Half", size);

  x = x + w1;
  obj = add_frame_box(x, y, w1, h1, "Chip", size);

  size = FL_MEDIUM_SIZE;
  x = x0 + 5;
  y = y + h1;
  SLOT =
  obj = add_switch_button(x, y, w1, h1, " ", size);
    fl_set_object_callback(obj, slot_callback, 0);

  x = x + w1;
  HALF =
  obj = add_switch_button(x, y, w1, h1, " ", size);
    fl_set_object_callback(obj, half_callback, 0);

  x = x + w1;
  CHIP =
  obj = add_switch_button(x, y, w1, h1, " ", size);
    fl_set_object_callback(obj, chip_callback, 0);
}

//~----------------------------------------------------------------------------

void slot_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  slot = MOD(slot+1, nslot);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) slot = MOD(slot-1, nslot);

  if (slot != -1 && half != -1) {
    brd = map[slot][half].brd;
    clk = map[slot][half].clk;
    dat = map[slot][half].dat;
  }

  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void half_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  half = MOD(half+1, 2);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) half = MOD(half-1, 2);

  if (slot != -1 && half != -1) {
    brd = map[slot][half].brd;
    clk = map[slot][half].clk;
    dat = map[slot][half].dat;
  }

  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void chip_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  chip = MOD(chip+1, 7);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) chip = MOD(chip-1, 7);

  chp = chip;
  
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void update_hi_lvl_adr_panel(void) {

  FL_OBJECT *obj;
  char b[80];

  obj = SLOT;
  if (slot != -1) {
    fl_set_object_label(obj, map[slot][0].slt);
    fl_set_object_color(obj, FL_WHEAT, FL_RED);
  }
  else {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_RED, FL_RED);
  }

  obj = HALF;
  if (half != -1) {
    fl_set_object_label(obj, map[0][half].hlf);
    fl_set_object_color(obj, FL_WHEAT, FL_RED);
  }
  else {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_RED, FL_RED);
  }

  obj = CHIP;
  if (chip != -1) {
    sprintf(b, "%d", chip);
    fl_set_object_label(obj, b);
    fl_set_object_color(obj, FL_WHEAT, FL_RED);
  }
  else {
    fl_set_object_label(obj, " ");
    fl_set_object_color(obj, FL_RED, FL_RED);
  }
}

//~============================================================================

void create_lo_lvl_adr_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int size, x, y, w, h, w1 = 20, w2 = 30, h1 = 20;
  
  w = 5 + 5 * w1 + 5 + 3 * w1 + 5 + 2 * w2 + 5;
  h = 5 + 2 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  size = FL_SMALL_SIZE;
  x = x0 + 5;
  y = y0 + 5;
  
  w = 5 * w1;
  obj = add_frame_box(x, y, w, h1, "Board Address", size);
  
  x = x + 5 + w;
  w = 3 * w1;
  obj = add_frame_box(x, y, w, h1, "Chip", size);
  
  x = x + 5 + w;
  w = w2;
  obj = add_frame_box(x, y, w, h1, "Clk", size);
  
  x = x + w;
  w = w2;
  obj = add_frame_box(x, y, w, h1, "Dat", size);
  
  x = x + w + 5;
  obj = add_normal_button(x, y0, 60, 5+2*h1+5, "INIT\nLeCroy", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, INIT_callback, 0);

  size = FL_MEDIUM_SIZE;
  x = x0 + 5;
  y = y + h1;
  for (i=0; i<5; i++) {
    int j = 4 - i;
    BRD[j] = 
    obj = add_switch_button(x, y, w1, h1, " ", size);
      fl_set_object_callback(obj, brd_callback, j);
    x = x + w1;
  }
  
  x = x + 5;
  for (i=0; i<3; i++) {
    int j = 2 - i;
    CHP[j] = 
    obj = add_switch_button(x, y, w1, h1, " ", size);
      fl_set_object_callback(obj, chp_callback, j);
    x = x + w1;
  }
  
  x = x + 5;
  CLK = 
  obj = add_switch_button(x, y, w2, h1, " ", size);
    fl_set_object_callback(obj, clk_callback, 0);

  x = x + w2;
  DAT = 
  obj = add_switch_button(x, y, w2, h1, " ", size);
    fl_set_object_callback(obj, dat_callback, 0);
}

//~----------------------------------------------------------------------------

void brd_callback(FL_OBJECT *obj, long data) {

  int bit = data;
  int i, j;

  slot = -1;
  half = -1;
  
  brd ^= 1 << bit;

  for (i=0; i<nslot; i++) {
    for (j=0; j<2; j++) {
      if (clk == map[i][j].clk && 
          dat == map[i][j].dat &&
          brd == map[i][j].brd) {
        slot = i;
        half = j;
      }
    }
  }
  
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void chp_callback(FL_OBJECT *obj, long data) {

  int bit = data;
  
  chip = -1;
  
  chp ^= 1 << bit;
  
  if (chp < 7) chip = chp;
    
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void clk_callback(FL_OBJECT *obj, long data) {

  int i, j;

  slot = -1;
  half = -1;
  
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  clk = MOD(clk+1, 8);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) clk = MOD(clk-1, 8);

#ifdef USE_AMSW
  dat = clk;
#endif

  for (i=0; i<nslot; i++) {
    for (j=0; j<2; j++) {
      if (clk==map[i][j].clk && dat==map[i][j].dat && brd==map[i][j].brd) {
        slot = i;
        half = j;
      }
    }
  }
    
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void dat_callback(FL_OBJECT *obj, long data) {

  int i, j;

  slot = -1;
  half = -1;
  
#ifdef USE_CAN
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  dat = MOD(dat+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) dat = MOD(dat-1, 16);
#endif

#ifdef USE_AMSW
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  dat = MOD(dat+1, 8);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) dat = MOD(dat-1, 8);
  clk = dat;
#endif

  for (i=0; i<nslot; i++) {
    for (j=0; j<2; j++) {
      if (clk==map[i][j].clk && dat==map[i][j].dat && brd==map[i][j].brd) {
        slot = i;
        half = j;
      }
    }
  }
    
  invalidate_data();
  update_objects();
}

//~----------------------------------------------------------------------------

void update_lo_lvl_adr_panel(void) {

  FL_OBJECT *obj;
  int i;
  char b[80];

  for (i=0; i<5; i++) {
    obj = BRD[i];
    if (brd & (1<<i)) fl_set_object_label(obj, "1");
    else              fl_set_object_label(obj, "0");
  }
  
  for (i=0; i<3; i++) {
    obj = CHP[i];
    if (chp & (1<<i)) fl_set_object_label(obj, "1");
    else              fl_set_object_label(obj, "0");
  }
  
  sprintf(b, "%d", clk);
  fl_set_object_label(CLK, b);

  sprintf(b, "%d", dat);
  fl_set_object_label(DAT, b);
}

//~============================================================================

void INIT_callback(FL_OBJECT *obj, long data) {

  int8 bus;

  msg = -1;
  update_objects();

  chk = 0;
  
#ifdef USE_CAN
  bus = (clk<<4) | dat;
#endif

#ifdef USE_AMSW
  bus = dat;
#endif

  init_LeCroy_bus(bus, &err);

  msg = 1;

  update_objects();
}

//~============================================================================

void create_Francesca_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 85, h1 = 40;

  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  create_bit_btn_hstring(
    1, x, y, w1, h1, F_modes, FL_MEDIUM_SIZE, &Francesca, &FRANCESCA[0]);
}

//~============================================================================

void create_Alessandro_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 95, h1 = 40;

  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  create_bit_btn_hstring(
    1, x, y, w1, h1, A_modes, FL_MEDIUM_SIZE, &Alessandro, &ALESSANDRO[0]);
}

//~============================================================================

void create_ADC_DAC_panels(int x0, int y0) {

  FL_OBJECT *obj;
  int i, x, y;
  int size = FL_NORMAL_SIZE;
  int w, h, w1 = 55, w2 = 60, w3 = 65, w4 = 45, h1 = 40;

  struct {
    char *nam;
    int   ADC;
    int   DAC;
  } cha[8] = {
//   Name    ADC   DAC
  {"14 bit",   0,   0}, 
  {"OVC lim",  2,   3},
  {"OVV lim",  3,   2},
  {"AUX",      4,   1},
  {"Temp.",    1,  -1},
  {"HV curr.", 5,  -1},
  {"HV volt.", 6,  -1},
  {"Ref.",     7,  -1}};

  w = 5 + w1 + w2 + w3 + w4 + w2 + w3 + w4 + 5;
  h = 5 + h1 + 8 * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1,   " ",    size);
    
  x = x + w1;
  w = w2 + w3 + w4;
  obj = add_frame_box(x, y,  w, h1/2, "ADC", size);

  x = x + w;
  w = w2 + w3 + w4;
  obj = add_frame_box(x, y,  w, h1/2, "DAC", size);

  x = x0 + 5 + w1;
  y = y + h1 / 2;
  obj = add_frame_box(x, y, w2, h1/2, "Code",  size);
    
  x = x + w2;
  obj = add_frame_box(x, y, w3, h1/2, "Value", size);
    
  x = x + w3;
  obj = add_frame_box(x, y, w4, h1/2, "Cmd.",  size);

  x = x + w4;
  obj = add_frame_box(x, y, w2, h1/2, "Code",  size);
    
  x = x + w2;
  obj = add_frame_box(x, y, w3, h1/2, "Value", size);
    
  x = x + w3;
  obj = add_frame_box(x, y, w4, h1/2, "Cmd.",  size);
  
  y = y + h1 / 2;
  for (i=0; i<8; i++) {
    char *t = cha[i].nam;
    int ADC = cha[i].ADC;
    int DAC = cha[i].DAC;
    char b[80];
    x = x0 + 5;
    sprintf(b, "%s\nADC %d", t, ADC);
    obj = add_frame_box(x, y, w1, h1, b, size);
    x = x + w1;
    create_ADC_panel(x, y, w2/4, w3, w4, h1, ADC);
    if (DAC >= 0) {
      DAC_wri_sta[i] = status_NN;
      DAC_rea_sta[i] = status_NN;
      x = x + w2 + w3 + w4;
      create_DAC_panel(x, y, w2/4, w3, w4, h1/2, DAC);
    }
    y = y + h1;
  }
  
  activate_DAC_input(-1, 0);
}

//~----------------------------------------------------------------------------

void create_ADC_panel(
     int x0, int y0, int w1, int w2, int w3, int h, int ADC) {

  FL_OBJECT *obj;
  int x, y, size = FL_MEDIUM_SIZE;
  int i;

  ADC_sta[ADC] = status_NN;
  
  x = x0;
  y = y0;

  for (i=0; i<4; i++) {
    ADC_DIGIT[ADC][i] =
    obj = add_data_box(x, y, w1, h, " ", size);
    x = x + w1;
  }

  ADC_VALUE[ADC] =
  obj = add_data_box(x, y, w2, h, " ", size);
    
  x = x + w2;
  ADC_GET_BUTTON[ADC] =
  obj = add_normal_button(x, y, w3, h, "GET", size);
    fl_set_object_callback(obj, READ_ADC_callback, ADC);
}

//~----------------------------------------------------------------------------

void READ_ADC_callback(FL_OBJECT *obj, long data) {

  int ADC = data;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (ADC_monitoring[ADC]) {
      ADC_monitoring[ADC] = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      get__ADC(chip, ADC);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    ADC_monitoring[ADC] = 1 - ADC_monitoring[ADC];
    msg = 0;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void create_DAC_panel(
     int x0, int y0, int w1, int w2, int w3, int h, int DAC) {

  FL_OBJECT *obj;
  int i;
  int x, y;
  
  DAC_wri_sta[DAC] = status_NN;
  DAC_rea_sta[DAC] = status_NN;

  x = x0;
  y = y0;
  
  for (i=0; i<4; i++) {
    DAC_WRI_DIGIT[DAC][i] =
    obj = add_switch_button(x, y, w1, h, " ", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, DAC_digit_callback, 10*DAC+i);
    x = x + w1;
  }

  DAC_INPUT[DAC] = 
  obj = fl_add_input(FL_FLOAT_INPUT,x,y-5,w2,h+10," ");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
//  fl_set_object_color(obj,FL_DARKORANGE,FL_DARKORANGE);
//  fl_set_object_color(obj,FL_WHITE,FL_WHITE);
    fl_set_object_color(obj,FL_WHEAT,FL_WHEAT);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_input_color(obj,FL_BLACK,FL_BLACK);
//  fl_set_input_return(obj,FL_RETURN_ALWAYS);
    fl_set_input_return(obj,FL_RETURN_END);
    fl_set_object_callback(obj, DAC_input_callback, DAC);

  DAC_WRI_VALUE[DAC] = 
  obj = add_switch_button(x, y, w2, h, " ", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, DAC_input_focus_callback, DAC);

  x = x + w2;
  obj = add_normal_button(x, y, w3, h, "SET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, WRITE_DAC_callback, DAC);
    
  x = x0;
  y = y + h;
  for (i=0; i<4; i++) {
    DAC_REA_DIGIT[DAC][i] =
    obj = add_data_box(x, y, w1, h, " ", FL_MEDIUM_SIZE);
    x = x + w1;
  }

  DAC_REA_VALUE[DAC] =
  obj = add_data_box(x, y, w2, h, " ", FL_MEDIUM_SIZE);

  x = x + w2;
  obj = add_normal_button(x, y, w3, h, "GET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, READ_DAC_callback, DAC);
}

//~----------------------------------------------------------------------------

void activate_DAC_input(int DAC, int mode) {

  FL_OBJECT *obj;
  int i;
  char b[80];
  
  for (i=0; i<4; i++) {
    obj = DAC_INPUT[i];
    if (i == DAC) {
      fl_activate_object(obj);
      fl_show_object(obj);
      if (DAC_wri_sta[i] == status_NN) {
        sprintf(b, " ");
      }
      else {
        if (mode) sprintf(b, "%.0f", DAC_wri_val[i]);
        else      sprintf(b, " ");
      }
      fl_set_input(obj,b);
    }
    else {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
  }

  for (i=0; i<4; i++) {
    obj = DAC_WRI_VALUE[i];
    if (i == DAC) {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
    else {
      fl_activate_object(obj);
      fl_show_object(obj);
    }
  }

  DAC_input_active = DAC;
}

//~----------------------------------------------------------------------------

void DAC_digit_callback(FL_OBJECT *obj, long data) {

  int d[4], dmax[4];
  int DAC = data / 10;
  int i   = data % 10;
  int16 k;

  k = DAC_wri_cod[DAC];
  
  d[0] = unpack16(k, 0xF000);
  d[1] = unpack16(k, 0x0F00);
  d[2] = unpack16(k, 0x00F0);
  d[3] = unpack16(k, 0x000F);

  k = max_DAC_cod[DAC];
  
  dmax[0] = unpack16(k, 0xF000);
  dmax[1] = unpack16(k, 0x0F00);
  dmax[2] = unpack16(k, 0x00F0);
  dmax[3] = unpack16(k, 0x000F);

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  d[i] = MOD(d[i]+1, dmax[i]+1);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) d[i] = MOD(d[i]-1, dmax[i]+1);

  pack16(&k, 0xF000, d[0]);
  pack16(&k, 0x0F00, d[1]);
  pack16(&k, 0x00F0, d[2]);
  pack16(&k, 0x000F, d[3]);

  DAC_wri_cod[DAC] = k;

  DAC_wri_val[DAC] = convert_DAC_to_value(DAC_wri_cod[DAC], DAC);
  
  DAC_wri_sta[DAC] = status_NA;

  activate_DAC_input(-1, 0);
        
  update_objects();
}

//~----------------------------------------------------------------------------

void DAC_input_focus_callback(FL_OBJECT *obj, long data) {

  int DAC = data;
  int mode = -1;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  mode = 0;
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) mode = 1;

  activate_DAC_input(DAC, mode);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void DAC_input_callback(FL_OBJECT *obj, long data) {

  int DAC = data;

  DAC_wri_val[DAC] = atof(fl_get_input(DAC_INPUT[DAC]));
  
  DAC_wri_cod[DAC] = convert_DAC_to_code(DAC_wri_val[DAC], DAC);
  DAC_wri_cod[DAC] = MIN(DAC_wri_cod[DAC], max_DAC_cod[DAC]);
  DAC_wri_val[DAC] = convert_DAC_to_value(DAC_wri_cod[DAC], DAC);
  
  DAC_wri_sta[DAC] = status_NA;

  activate_DAC_input(-1, 0);

  update_objects();
}

//~----------------------------------------------------------------------------

void WRITE_DAC_callback(FL_OBJECT *obj, long data) {

  int DAC = data;

  if (DAC == DAC_input_active) activate_DAC_input(-1, 0);

  msg = -1;
  update_objects();

  write__DAC(chip, DAC);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void READ_DAC_callback(FL_OBJECT *obj, long data) {

  int DAC = data;

  msg = -1;
  update_objects();

  read__DAC(chip, DAC);
  
  msg = 1;  
  update_objects();
}

//~----------------------------------------------------------------------------

void update_ADC_DAC_panels(void) {

  FL_OBJECT *obj;
  int i, ADC, DAC;
  char b[80];
  
//~-- ADC Read Code ---

  for (ADC=0; ADC<8; ADC++) {
    int16 msk[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
    for (i=0; i<4; i++) {
      obj = ADC_DIGIT[ADC][i];
      if (ADC_sta[ADC] == status_NA || ADC_sta[ADC] == status_NN) {
        fl_set_object_lcolor(obj, FL_BLACK);
        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      }
      if (ADC_sta[ADC] == status_KO) {
        fl_set_object_lcolor(obj, FL_WHITE);
        fl_set_object_color(obj, FL_BLACK, FL_BLACK);
      }
      if (ADC_sta[ADC] == status_OK) {
        fl_set_object_lcolor(obj, FL_BLACK);
        fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      }
      if (ADC_sta[ADC] == status_NN)
        sprintf(b, " ");
      else
        sprintf(b, "%01X", unpack16(ADC_cod[ADC], msk[i]));
      fl_set_object_label(obj, b);
    }
  }

//~-- ADC Read Value ---

  for (ADC=0; ADC<8; ADC++) {
    obj = ADC_VALUE[ADC];
    if (ADC_sta[ADC] == status_NA || ADC_sta[ADC] == status_NN) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    if (ADC_sta[ADC] == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, FL_BLACK);
    }
    if (ADC_sta[ADC] == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    }
    if (ADC_sta[ADC] == status_NN) sprintf(b, " ");
    else                           sprintf(b, "%6.1f", ADC_val[ADC]);
    fl_set_object_label(obj, b);
  }

//~-- ADC Get Buttons ---

  for (ADC=0; ADC<8; ADC++) {
    obj = ADC_GET_BUTTON[ADC];
    if (ADC_monitoring[ADC]) set_normal_button(obj, 1);
    else                     set_normal_button(obj, 0);
  }

//~-- DAC Input Code ---

  for (DAC=0; DAC<4; DAC++) {
    int16 msk[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
    for (i=0; i<4; i++) {
      int d_now = unpack16(DAC_wri_cod[DAC], msk[i]);
      int d_was = unpack16(DAC_wri_was[DAC], msk[i]);
      obj = DAC_WRI_DIGIT[DAC][i];
      sprintf(b, "%01X", d_now);
      fl_set_object_label(obj, b);
      if (DAC_wri_sta[DAC] == status_OK && d_now == d_was) {
        fl_set_object_color(obj, FL_GREEN, FL_RED);
      }
      else {
        fl_set_object_color(obj, FL_YELLOW, FL_RED);
      }
    }
  }

//~-- DAC Input Value ---

  for (DAC=0; DAC<4; DAC++) {
    obj = DAC_WRI_VALUE[DAC];
    sprintf(b, "%7.1f", DAC_wri_val[DAC]);
    if (DAC != DAC_input_active) {
      fl_set_object_label(obj, b);
      if (DAC_wri_sta[DAC] == status_OK)
        fl_set_object_color(obj, FL_GREEN, FL_RED);
      else
        fl_set_object_color(obj, FL_YELLOW, FL_RED);
    }
  }
  
//~-- DAC Read Code ---

  for (DAC=0; DAC<4; DAC++) {
    int16 msk[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
    for (i=0; i<4; i++) {
      int d_rea = unpack16(DAC_rea_cod[DAC], msk[i]);
      int d_was = unpack16(DAC_rea_was[DAC], msk[i]);
      obj = DAC_REA_DIGIT[DAC][i];
      if (DAC_rea_sta[DAC] == status_NA || DAC_rea_sta[DAC] == status_NN) {
        fl_set_object_lcolor(obj, FL_BLACK);
        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      }
      if (DAC_rea_sta[DAC] == status_KO) {
        fl_set_object_lcolor(obj, FL_WHITE);
        fl_set_object_color(obj, FL_BLACK, FL_BLACK);
      }
      if (DAC_rea_sta[DAC] == status_OK) {
        fl_set_object_lcolor(obj, FL_BLACK);
        if (DAC_wri_sta[DAC] == status_OK && d_rea != d_was) {
          fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
        }
        else {
          fl_set_object_color(obj, FL_GREEN, FL_GREEN);
        }
      }
      if (DAC_rea_sta[DAC] == status_NN) sprintf(b, " ");
      else                             sprintf(b, "%01X", d_rea);
      fl_set_object_label(obj, b);
    }
  }

//~-- DAC Read Value ---

  for (DAC=0; DAC<4; DAC++) {
    int16 rea = DAC_rea_cod[DAC];
    int16 was = DAC_rea_was[DAC];
    obj = DAC_REA_VALUE[DAC];
    if (DAC_rea_sta[DAC] == status_NA || DAC_rea_sta[DAC] == status_NN) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    if (DAC_rea_sta[DAC] == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, FL_BLACK);
    }
    if (DAC_rea_sta[DAC] == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      if (DAC_wri_sta[DAC] == status_OK && rea != was) {
        fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
      }
      else {
        fl_set_object_color(obj, FL_GREEN, FL_GREEN);
      }
    }
    if (DAC_rea_sta[DAC] == status_NN) sprintf(b, " ");
    else                               sprintf(b, "%7.1f", DAC_rea_val[DAC]);
    fl_set_object_label(obj, b);
  }
}

//~============================================================================

void create_HV_dis_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, w1 = 35, w2 = 70, w3 = 70, w4 = 45, h = 20;

  HV_wri_sta = status_NN;
  HV_rea_sta = status_NN;
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,10+2*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, 2*h, "HV", FL_NORMAL_SIZE);

  x = x + w1;
  obj = add_normal_button(x, y, w2, h, "ENABLE", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, WRITE_HV_dis_callback, (long)0x0000);

  x = x + w2;
  obj = add_normal_button(x, y, w3, h, "DISABLE", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, WRITE_HV_dis_callback, (long)0x8000);

  x = x + w3;
  READ_HV =
  obj = add_normal_button(x, y, w4, 2*h, "GET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, READ_HV_dis_callback, 0);

  x = x0 + 5 + w1;
  y = y + h;
  w = w2 + w3;
  HV_REA =
  obj = add_data_box(x, y, w, h, " ", FL_MEDIUM_SIZE);
}

//~----------------------------------------------------------------------------

void WRITE_HV_dis_callback(FL_OBJECT *obj, long data) {

  HV_wri_cod = data;
  
  msg = -1;  
  update_objects();

  write__HV_status(chip);
    
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void READ_HV_dis_callback(FL_OBJECT *obj, long data) {

  msg = -1;  
  update_objects();

  read__HV_status(chip);
  
  msg = 1;  
  update_objects();
}

//~----------------------------------------------------------------------------

void update_HV_dis_panel(void) {

  FL_OBJECT *obj;

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
    fl_set_object_label(obj, " ");
  }
  else {
    if (HV_rea_cod) fl_set_object_label(obj, "Disabled");
    else            fl_set_object_label(obj, "Enabled");
  }
}

//~============================================================================

void create_chip_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, w1 = 35, w2 = 45, h = 20;
  int i;

  chip_sta_sta = status_NN;

  w = 5 + 5 * w1 + w2 + 5;  
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,10+3*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  w = 5 * w1 + w2;
  obj = add_frame_box(x, y, w, h, "Chip Status", FL_NORMAL_SIZE);

  y = y + h;
  for (i=0; i<5; i++) {
    char *txt[5] = {"OC1", "OC2", "OVV", "OVC", "?"};
    obj = add_frame_box(x, y, w1, h, txt[i], FL_NORMAL_SIZE);
    x = x + w1;
  }

  STA_GET_BUTTON =
  obj = add_normal_button(x, y, w2, 2*h, "GET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, READ_chip_status_callback, 0);

  x = x0 + 5;
  y = y + h;
  for (i=0; i<5; i++) {
    STA_DIGIT[i] =
    obj = add_data_box(x, y, w1, h, " ", FL_MEDIUM_SIZE);
    x = x + w1;
  }
}

//~----------------------------------------------------------------------------

void READ_chip_status_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (sta_monitoring) {
      sta_monitoring = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;  
      update_objects();
      read__chip_status(chip);
      msg = 1;  
      update_objects();
    }
  }
  
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    sta_monitoring = 1 - sta_monitoring;
    msg = 0;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_chip_status_panel(void) {

  FL_OBJECT *obj;
  int i;
  char b[80];
  int16 msk[5] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800};
  
  for (i=0; i<5; i++) {
    int bit = unpack16(chip_sta_cod, msk[i]);
    obj = STA_DIGIT[i];
    if (chip_sta_sta == status_NA || chip_sta_sta == status_NN) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    }
    if (chip_sta_sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, FL_BLACK);
    }
    if (chip_sta_sta == status_NN) {
      sprintf(b, " ");
    }
    else {
      sprintf(b, "%01X", bit);
      if (chip_sta_sta == status_OK) {
        if (bit) {
          fl_set_object_lcolor(obj, FL_YELLOW);
          fl_set_object_color(obj, FL_RED, FL_RED);
        }
        else {
          fl_set_object_lcolor(obj, FL_BLACK);
          fl_set_object_color(obj, FL_GREEN, FL_GREEN);
        }
      }
    }
    fl_set_object_label(obj, b);
  }

  obj = STA_GET_BUTTON;
  if (sta_monitoring) set_normal_button(obj, 1);
  else                set_normal_button(obj, 0);

  fl_check_forms();
}

//~============================================================================
#ifdef USE_CAN

void create_USCM_tmp_panel(int x0, int y0) {

  FL_OBJECT *obj;
  
  int x, y, w;
  int w1 = 110, w2 = 65, w3 = 45, h = 20;
  
  w = 5 + w1 + w2 + w3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h, "USCM Temp.", FL_NORMAL_SIZE);

  x = x + w1;
  USCM_TMP_VALUE = 
  obj = add_data_box(x, y, w2, h, "t", FL_NORMAL_SIZE);

  x = x + w2;
  USCM_TMP_GET_BUTTON =
  obj = add_normal_button(x, y, w3, h, "GET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, USCM_tmp_callback, 0);
}

//~----------------------------------------------------------------------------

void read_USCM_tmp_data() {

  bool ok[64];
  float temp[64];
  int8 n;

  if (P > 0) printf("read_USCM_tmp_data\n");

  n = 0;
  if (!USCM_tmp_unk) USCM_tmp_sta = status_KO;
  read_DS1820_temp_short(9, &n, ok, temp, &err);
  if (P > 0) {
    printf("err = %04X  n = %d   ok[0] = %d\n", err, n, ok[0]);
  }
  if (err) {
    USCM_tmp_sta = status_KO;
  }
  else {
    if (n == 1 && ok[0]) {
      USCM_tmp_unk = 0;
      USCM_tmp_sta = status_OK;
      USCM_tmp_val = temp[0];
    }
    if (P > 0) {
      printf("USCM_tmp_unk = %d, ", USCM_tmp_unk);
      printf("USCM_tmp_sta = %d, ", USCM_tmp_sta);
      printf("USCM_tmp_val = %f\n", USCM_tmp_val);
    }
  }

}

//~----------------------------------------------------------------------------

void USCM_tmp_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (USCM_tmp_monitoring) {
      USCM_tmp_monitoring = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;  
      update_objects();
      read_USCM_tmp_data();
      msg = 1;  
      update_objects();
    }
  }
  
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    USCM_tmp_monitoring = 1 - USCM_tmp_monitoring;
    msg = 0;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_USCM_tmp_panel() {

  FL_OBJECT *obj;
  char b[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;

  if (USCM_tmp_unk) sprintf(b, " ");
  else              sprintf(b, "%+5.1f", USCM_tmp_val);

  switch (USCM_tmp_sta) {
    case status_OK: color = good;    break;
    case status_KO: color = error;   break;
    case status_NA: color = warning; break;
    default:        color = alarm;   break;
  }
  if ((obj = USCM_TMP_VALUE)) {
    fl_set_object_label(obj, b);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  obj = USCM_TMP_GET_BUTTON;
  if (USCM_tmp_monitoring) set_normal_button(obj, 1);
  else                     set_normal_button(obj, 0);
  
  fl_check_forms();
}

#endif
//~============================================================================

void create_broadcast_power_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, size, w = 35, h = 20;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+2*w,10+2*h," ");

  size = FL_SMALL_SIZE;
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w, h, "Bcast", size);

  x = x + w;
  obj = add_frame_box(x, y, w, h, "PWR", size);

  size = FL_MEDIUM_SIZE;
  x = x0 + 5;
  y = y + h;
  BROADCAST =
  obj = add_switch_button(x, y, w, h, " ", size);
    fl_set_object_callback(obj, BROADCAST_callback, 0);

  x = x + w;
  POWER =
  obj = add_switch_button(x, y, w, h, " ", size);
    fl_set_object_callback(obj, POWER_callback, 0);
}

//~----------------------------------------------------------------------------

void BROADCAST_callback(FL_OBJECT *obj, long data) {

  broadcast = 1 - broadcast;

  update_objects();
}

//~----------------------------------------------------------------------------

void POWER_callback(FL_OBJECT *obj, long data) {

  power = 1 - power;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_broadcast_power_panel(void) {

  if (broadcast) fl_set_object_label(BROADCAST, "ON");
  else           fl_set_object_label(BROADCAST, "OFF");

  if (power) fl_set_object_label(POWER, "ON");
  else       fl_set_object_label(POWER, "OFF");
}

//~============================================================================

void create_chip_program_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w1 = 105, w2 = 95, w3 = 15, w4 = 45, h1 = 20;
  int i;

  pgm_wri_sta = status_NN;
  pgm_rea_sta = status_NN;
  pgm_wri_cod = pgm_default;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,115,370," ");

  x = x0 + 5;
  y = y0 + 5;
  
  obj = add_frame_box(x, y, w1, h1, "Chip Program", FL_NORMAL_SIZE);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w1, h1, " ");
    fl_set_object_callback(obj, pgm_default_callback, 0);
    
  create_chip_program_selector(x0+10, y0+40,  w2, 1);
  create_chip_program_selector(x0+10, y0+120, w2, 2);
  create_chip_program_selector(x0+10, y0+200, w2, 3);
  create_chip_program_selector(x0+10, y0+280, w2, 4);
  
  x = x0 + 5;
  y = y0 + 5 + 320;

  for (i=0; i<4; i++) {
    PGM_WRI_DIGIT[i] =
    obj = add_switch_button(x, y, w3, h1, " ", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, pgm_digit_callback, i);
    x = x + w3;
  }

  PGM_WRITE =
  obj = add_normal_button(x, y, w4, h1, "SET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, PGM_WRITE_callback, 0);
  
  x = x0 + 5;
  y = y + h1;

  for (i=0; i<4; i++) {
    PGM_REA_DIGIT[i] =
    obj = add_data_box(x, y, w3, h1, " ", FL_MEDIUM_SIZE);
    x = x + w3;
  }

  obj = add_normal_button(x, y, w4, h1, "GET", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, PGM_READ_callback, 0);
}

//~----------------------------------------------------------------------------

void create_chip_program_selector(int x0, int y0, int w, int ind) {

  FL_OBJECT *obj;
  struct pgm_selector *s;
  int x, y, h;
  int i;
  
  s = &pgm_selector[ind];
  
  h = 5 + s->n * 15 + 5;
  
  x = x0;
  y = y0;
  
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,x,y,w,h,s->label);
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);

  fl_bgn_group();
  
    x = x0 + 5;
    
    for (i=0; i<s->n; i++) {
      y = y0 + 5 + i * 15;
      s->BUTTON[i] =
      obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,100,20,s->field[i].nam);
        fl_set_object_color(obj,FL_LEFT_BCOL,FL_RED);
        fl_set_object_lsize(obj,FL_NORMAL_SIZE);
        fl_set_object_lstyle(obj,FL_BOLD_STYLE);
        fl_set_object_callback(obj,pgm_selector_callback,10*ind+i);
    }
    
  fl_end_group();
}

//~----------------------------------------------------------------------------

void pgm_default_callback(FL_OBJECT *obj, long data) {

  pgm_wri_cod = pgm_default;

  update_objects();
}

//~----------------------------------------------------------------------------

void pgm_selector_callback(FL_OBJECT *obj, long data) {

  struct pgm_selector *s;
  int i = data / 10;
  int j = data % 10;
  
  s = &pgm_selector[i];
  
  pack16(&pgm_wri_cod, s->msk, s->field[j].cod);

  update_objects();
}

//~----------------------------------------------------------------------------

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

//~----------------------------------------------------------------------------

void PGM_WRITE_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  write__chip_program(chip);
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void PGM_READ_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  read__chip_program(chip);
  
  msg = 1;  
  update_objects();
}

//~----------------------------------------------------------------------------

void update_chip_program_panel(void) {

  FL_OBJECT *obj;
  struct pgm_selector *s;
  int16 m;
  int i, j;
  int16 msk[4] = {0xF000, 0x0F00, 0x00F0, 0x000F};
  char b[80];

//~-- pgm selector (buttons) ---

  for (i=1; i<5; i++) {
    s = &pgm_selector[i];
    m = unpack16(pgm_wri_cod, s->msk);
    for (j=0; j<s->n; j++) {
      if (m == s->field[j].cod) {
        fl_set_button(s->BUTTON[j], 1);
      }
    }
  }

//~-- pgm selector (fields) ---

  for (i=1; i<5; i++) {
    s = &pgm_selector[i];
    m = unpack16(pgm_rea_cod, s->msk);
    for (j=0; j<s->n; j++) {
      if (pgm_rea_sta != status_NN) {
        if (m == s->field[j].cod) {
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
    int d_now = unpack16(pgm_wri_cod, msk[i]);
    int d_was = unpack16(pgm_wri_was, msk[i]);
    obj = PGM_WRI_DIGIT[i];
    sprintf(b, "%01X", d_now);
    fl_set_object_label(obj, b);
    if (pgm_wri_sta == status_OK && d_now == d_was) {
      fl_set_object_color(obj, FL_GREEN, FL_RED);
    }
    else {
      fl_set_object_color(obj, FL_YELLOW, FL_RED);
    }
  }

//~-- pgm Read Code ---

  for (i=0; i<4; i++) {
    int d_rea = unpack16(pgm_rea_cod, msk[i]);
    int d_was = unpack16(pgm_rea_was, msk[i]);
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

//~============================================================================

void create_LeCroy_bus_code_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, x, y, w, h, wa = 25, w1 = 15, w2 = 45, h1 =20, h2 = 15, h3 = 90, h4 = 20;

  wri_was_sta = status_NN;
    
  w = wa + 16 * w1 + 3 * 5 + 10 + 16 * w1 + 3 * 5 + 5 + w2 + 5;
  h = 5 + 5 * h1 + h2;
  obj = fl_add_box(FL_DOWN_BOX,x0,y0,w,h," ");

  x = x0 + wa;
  y = y0 + 5;
  for (i=0; i<4; i++) {
    char *t[4] = {"1", "P", "P\nW\nR", "B"};
    obj = add_frame_box(x, y, w1, 2*h1, t[i], FL_SMALL_SIZE);
    x = x + w1;
  }

  x = x + 5;
  w = 8 * w1 + 5;
  obj = add_frame_box(x, y, w, 2*h1, "Select Code\n(Chip Address)", FL_SMALL_SIZE);

  x = x + w + 5;
  R_W = 
  obj = add_frame_box(x, y, w1, (2*h1)/3+1, " ", FL_SMALL_SIZE);
    fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
    fl_set_object_lcolor(obj,FL_BLACK);
    
  obj = add_frame_box(x, y+(2*h1)/3, w1, 2*h1-(2*h1)/3-1, "R\nW", FL_SMALL_SIZE);

  x = x + w1;
  w = 3 * w1;
  SECTION =
  obj = add_frame_box(x, y, w, h1, " ", FL_SMALL_SIZE);
    fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
    fl_set_object_lcolor(obj,FL_BLACK);
  obj = add_frame_box(x, y+h1, w, h1, "Sect.", FL_SMALL_SIZE);

  x = x + w + 10;
  w = 16 * w1 + 3 * 5;
  obj = add_frame_box(x, y, w, 2 * h1, "Data", FL_SMALL_SIZE);

  x = x + w + 5;
  SEND_BUTTON =
  obj = add_normal_button(x, y, w2, h3, "SEND", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, SEND_callback, 0);
  STEP_BUTTON =
  obj = add_push_button(x, y+h3, w2, h4, "Step", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, push_button_callback, 0);

  x = x0 + wa;
  y = y + 2 * h1;
  for (i=0; i<32; i++) {
    char b[80];
    if (i == 0) {
      add_plain_text(x0, y,      wa, h1, "@->", FL_NORMAL_SIZE);
      add_plain_text(x0, y+h1,   wa, h1, "TX",  FL_NORMAL_SIZE);
      add_plain_text(x0, y+2*h1, wa, h1, "RX",  FL_NORMAL_SIZE);
    }
    WRI[i] =
    obj = add_switch_button(x, y, w1, h1, " ", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, wri_callback, i);
    WAS[i] =
    obj = add_data_box(x, y+h1, w1, h1, " ", FL_MEDIUM_SIZE);
    REA[i] =
    obj = add_data_box(x, y+2*h1, w1, h1, " ", FL_MEDIUM_SIZE);
    sprintf(b, "%d", i+1);
    obj = add_plain_text(x-5, y+3*h1, w1+10, h2, b, FL_SMALL_SIZE);
    x = x + w1;
    if ((i + 1) % 4 == 0) x = x + 5;
    if (i == 15) x = x + 5;
  }
}

//~----------------------------------------------------------------------------

void wri_callback(FL_OBJECT *obj, long data) {

  int bit = data;

  wri ^= 0x80000000 >> bit;
    
  update_objects();
}

//~----------------------------------------------------------------------------

void SEND_callback(FL_OBJECT *obj, long data) {

  int8 bus;

  msg = -1;
  update_objects();

  chk = 1;

#ifdef USE_CAN
  bus = (clk<<4) | dat;
#endif

#ifdef USE_AMSW
  bus = dat;
#endif
 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err);

  wri_was_sta = status_OK;
  wri_was_cod = wri;
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_LeCroy_bus_code_panel(void) {

  FL_OBJECT *obj;
  int k, p, color;
  char *section[8] = {"14bit", "AUX",    "OVV",    "OVC", 
                      "Prog.", "HV dis", "Status", "ADC"};

//~-- R/W Bit ---

  if (wri & 0x00080000) fl_set_object_label(R_W, "W");
  else                  fl_set_object_label(R_W, "R");

//~-- Section Field ---

  fl_set_object_label(SECTION, section[unpack32(wri, 0x00070000)]);
  
//~== WRI Bits ===

  for (k=0; k<32; k++) {
    int32 d = wri_was_cod ^ wri;
    obj = WRI[k];
    if (wri & 0x80000000>>k) fl_set_object_label(obj, "1");
    else                     fl_set_object_label(obj, "0");
    if (wri_was_sta == status_NN) {
      color = FL_GREEN;
    }
    else {
      if (d & 0x80000000>>k) color = FL_YELLOW;
      else                   color = FL_GREEN;
    }
    fl_set_object_color(obj, color, FL_RED);
  }

  for (k=0, p=1; k<32; k++) if (wri & 0x80000000>>k && k != 1) p ^= 1;

//~-- First Bit ---

  obj = WRI[0];
  if (wri & 0x80000000) {
    fl_set_object_color(obj, FL_GREEN, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
  else {
    fl_set_object_color(obj, FL_RED, FL_GREEN);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }

//~-- Parity Bit ---

  obj = WRI[1];
  if (unpack32(wri, 0x40000000) == p) {
    fl_set_object_color(obj, FL_GREEN, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
  else {
    fl_set_object_color(obj, FL_RED, FL_GREEN);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }

//~-- Power Bit ---

  obj = WRI[2];
  if (wri & 0x20000000) {
    fl_set_object_color(obj, FL_GREEN, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
  else {
    fl_set_object_color(obj, FL_RED, FL_GREEN);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }

//~-- Broadcast Bit ---

  obj = WRI[3];
  if (wri & 0x10000000) {
    fl_set_object_color(obj, FL_RED, FL_GREEN);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }
  else {
    fl_set_object_color(obj, FL_GREEN, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }

//~== WAS Bits ===

  for (k=0; k<32; k++) {
    obj = WAS[k];
    if (wri_was_sta == status_NN) {
      color = FL_YELLOW;
      fl_set_object_label(obj, " ");
    }
    else {
      color = FL_GREEN;
      if (unpack32(wri_was_cod, 0x80000000>>k)) fl_set_object_label(obj, "1");
      else                                      fl_set_object_label(obj, "0");
    }
    fl_set_object_color(obj, color, color);
  }

//~== REA Bits ===

  if (chk == 0) color = FL_GREEN;
  else          color = FL_YELLOW;

  for (k=0; k<32; k++) {
    obj = REA[k];
    if (unpack32(rea, 0x80000000>>k)) fl_set_object_label(obj, "1");
    else                              fl_set_object_label(obj, "0");
    fl_set_object_color(obj, color, color);
  }

//~== "Step" Button ===

  obj = STEP_BUTTON;
  if (fl_get_button(obj)) fl_set_object_lcolor(obj, FL_YELLOW);
  else                    fl_set_object_lcolor(obj, FL_BLACK);
}

//~============================================================================

void TIMER_callback(FL_OBJECT *obj, long data) {

  read_all_data();
  fl_set_timer(obj, TIMER_PERIOD);
}

//~----------------------------------------------------------------------------

void read_all_data(void) {

  int i;
  int changed = 0;

#ifdef USE_CAN
  if (USCM_tmp_monitoring) {
    read_USCM_tmp_data();
    update_USCM_tmp_panel();
  }
#endif

  for (i=0; i<8; i++) {
    if (ADC_monitoring[i]) {
      changed = 1;
      get__ADC(chip, i);
    }
  }
  if (changed) update_ADC_DAC_panels();

  if (sta_monitoring) {
    read__chip_status(chip);
    update_chip_status_panel();
  }

  fl_check_forms();
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);
  
  update_hi_lvl_adr_panel();
  update_lo_lvl_adr_panel();
  update_ADC_DAC_panels();
  update_broadcast_power_panel();
  update_chip_program_panel();
  update_HV_dis_panel();
  update_chip_status_panel();
#ifdef USE_CAN
  update_USCM_tmp_panel();
#endif
  update_LeCroy_bus_code_panel();

//write_dat_config_file();
  
  fl_check_forms();

  if (P > 1) {
    printf("clk = %02X dat = %04X wri = %08X rea = %08X\n", 
            clk,       dat,       wri,       rea);
    printf("msg = %d err = 0x%04X chk = %d\n", msg, err, chk);
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
