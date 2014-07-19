// file GPSE.c
//
// GPSE Primitive Controller
//
// A.Lebedev Aug-2005...

#include "template.h"
#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"GPSE Controller"};                    // global variable
char *date  = {"25-Dec-08"};                          // global variable

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

SW_BTN    *BIT_BTN[4][16];
FL_OBJECT *BIT_BOX[4][16];
_w wri_bit[4];
_r rea_bit[4];

//~--

SW_BTN *RATE_BTN[2];
_w rate[2];

//~--

FL_OBJECT *SERIAL_INPUT[2][2];
int  serial_input_cnt[2];
int8 serial_input_dat[2][512];
int serial_input_unk[2], serial_input_sta[2];

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *dec[32] = {
 "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31"};

char *rates[4] = {"2400", "4800", "9600", "19200"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void write_register_callback(FL_OBJECT *obj, long data);
void read_register_callback(FL_OBJECT *obj, long data);

void create_control_and_status_register_panel(int x0, int y0);
void update_control_and_status_register_panel(void);

void create_serial_ports_outputs_register_panel(int x0, int y0);
void update_serial_ports_outputs_register_panel(void);

void create_serial_ports_inputs_register_panel(int x0, int y0);
void update_serial_ports_input_register_panel(void);

void create_pulse_register_panel(int x0, int y0);
void update_pulse_register_panel(void);

void create_serial_ports_rates_panel(int x0, int y0);

void create_begin_epoch_panel(int x0, int y0);
void begin_epoch_callback(FL_OBJECT *obj, long data);
void update_begin_epoch_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 450, HH = 730;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_control_and_status_register_panel(    0,  45);
  create_serial_ports_outputs_register_panel(230,  45);
  create_serial_ports_inputs_register_panel( 230, 270);
  create_pulse_register_panel(                 0, 430);
  create_serial_ports_rates_panel(           230, 495);
  create_begin_epoch_panel(                    0, 610);

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
  
  for (i=0; i<4; i++) {
    wri_bit[i].inp = 0x0000;
    wri_bit[i].unk = TRUE;
  }

  for (i=0; i<2; i++) {
    rate[i].inp = 3;
  }

  for (i=0; i<3; i++) {
    rea_bit[i].unk = TRUE;
    rea_bit[i].sta = status_NA;
  }

  for (i=0; i<2; i++) {
    serial_input_unk[i] = TRUE;
    serial_input_sta[i] = status_NA;
  }
  
  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    for (i=0; i<4; i++) {
      fread(&wri_bit[i].inp, sizeof(wri_bit[i].inp), 1, file);
    }
    for (i=0; i<2; i++) {
      fread(&rate[i].inp, sizeof(rate[i].inp), 1, file);
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
    for (i=0; i<4; i++) {
      fwrite(&wri_bit[i].inp, sizeof(wri_bit[i].inp), 1, file);
    }
    for (i=0; i<2; i++) {
      fwrite(&rate[i].inp, sizeof(rate[i].inp), 1, file);
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
//                  WRITE AND READ REGISTER CALLBACKs
//
//~============================================================================

void write_register_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  int8 adr = 0x00;
  
  r->sta = status_NA;

  msg = -1;
  update_objects();

  write_LVDS_bus(adr+ind, (int16)w->inp, &err);
  
  if (!err) {
    w->wri = w->inp;
    w->unk = FALSE;
  }
  else {
    w->unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_register_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _r *r = &rea_bit[ind];
  int16 dat;

  int8 adr = 0x00;
  
  msg = -1;
  update_objects();

  *r->was = *r->rea;
  
  read_LVDS_bus(adr+ind, &dat, &err);
  
  if (!err) {
    *r->rea = dat;
    if (r->unk) *r->was = *r->rea;
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~============================================================================
//
//                  CONTROL AND STATUS REGISTER PANEL
//
//~============================================================================

void create_control_and_status_register_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 160, w3 = 20, h1 = 30, h2 = 20;

  int ind = 0;
  
  char *txt[16] = {
    "EPOCH Gate Auto Close",
    "COM Gate Auto Open",
    "COM Gate Auto Close",
    "EPOCH Gate Status",
    "COM Gate Status",
    "USCM A Power Status",
    "USCM B Power Status",
    "GPS DWTO Status",
    "PPS Gate Port A Status",
    "PPS Gate Port B Status",
    "GPS Output Polarity",
    "GPS Input Polarity",
    "AST Serial Port",
    "AST Output Polarity",
    "AST Input Polarity",
    "GPSE Power Bit"};
  
  w = 5 + w1 + w2 + 2 * w3 + 5;
  h = 5 + h1 + 16 * h2 + 1 * 5 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+2*w3, h1, "Control & Status Register", FL_MEDIUM_SIZE);
  y = y + h1;
  
  rea_bit[ind].rea = malloc(4);
  rea_bit[ind].was = malloc(4);

  create_frame_box_vstring(16, x,    y, w1, h2, &dec[0], 'N' + 800, 'C');
  create_frame_box_vstring(16, x+w1, y, w2, h2, &txt[0], 'N' + 800, 'L');
  create_bit_btn_vstring(
    -16, x+w1+w2, y, w3, h2, hex, FL_MEDIUM_SIZE + 800, &wri_bit[ind], &BIT_BTN[ind][0]);
  create_data_box_vstring(16, x+w1+w2+w3, y, w3, h2, FL_MEDIUM_SIZE + 800, &BIT_BOX[ind][0]);
  y = y + 16 * h2 + 1 * 5;
  
  obj = add_frame_box(x, y, w1+w2, h2, "Command:", 'N');
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  obj = add_normal_button(x+w1+w2, y, w3, h2, "W", 'N');
    fl_set_object_callback(obj, write_register_callback, ind);
  obj = add_normal_button(x+w1+w2+w3, y, w3, h2, "R", 'N');
    fl_set_object_callback(obj, read_register_callback, ind);
}

//~----------------------------------------------------------------------------

void update_control_and_status_register_panel(void) {

  int ind = 0;

  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  show_bin_btn_string(-16, w, &BIT_BTN[ind][0]);
  show_bit_box_string(-16, r, NULL, &BIT_BOX[ind][0]);
}

//~============================================================================
//
//                  USCM SERIAL PORTS OUTPUTS REGISTER PANEL
//
//~============================================================================

void create_serial_ports_outputs_register_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 150, w3 = 20, h1 = 30, h2 = 20;

  int ind = 1;
  
  char *txt[8] = {
    "USCM A Port 0 to GPS",
    "USCM A Port 1 to GPS",
    "USCM B Port 0 to GPS",
    "USCM B Port 1 to GPS",
    "USCM A Port 0 to AST",
    "USCM A Port 1 to AST",
    "USCM B Port 0 to AST",
    "USCM B Port 1 to AST"};
  
  w = 5 + w1 + w2 + 2 * w3 + 5;
  h = 5 + h1 + 8 * h2 + 5 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+2*w3, h1, "USCM Serial Ports Outputs", FL_MEDIUM_SIZE);
  y = y + h1;
  
  rea_bit[ind].rea = malloc(4);
  rea_bit[ind].was = malloc(4);

  create_frame_box_vstring(8, x,    y, w1, h2, &dec[0], 'N' + 400, 'C');
  create_frame_box_vstring(8, x+w1, y, w2, h2, &txt[0], 'N' + 400, 'L');
  create_bit_btn_vstring(
    -8, x+w1+w2, y, w3, h2, hex, FL_MEDIUM_SIZE + 400, &wri_bit[ind], &BIT_BTN[ind][0]);
  create_data_box_vstring(8, x+w1+w2+w3, y, w3, h2, FL_MEDIUM_SIZE + 400, &BIT_BOX[ind][0]);
  y = y + 8 * h2 + 5;
  
  obj = add_frame_box(x, y, w1+w2, h2, "Command:", 'N');
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  obj = add_normal_button(x+w1+w2, y, w3, h2, "W", 'N');
    fl_set_object_callback(obj, write_register_callback, ind);
  obj = add_normal_button(x+w1+w2+w3, y, w3, h2, "R", 'N');
    fl_set_object_callback(obj, read_register_callback, ind);
}

//~----------------------------------------------------------------------------

void update_serial_ports_outputs_register_panel(void) {

  int ind = 1;

  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  show_bin_btn_string(-8, w, &BIT_BTN[ind][0]);
  show_bit_box_string(-8, r, NULL, &BIT_BOX[ind][0]);
}

//~============================================================================
//
//                  USCM SERIAL PORTS INPUTS REGISTER PANEL
//
//~============================================================================

void create_serial_ports_inputs_register_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 150, w3 = 20, h1 = 30, h2 = 20;

  int ind = 2;
  
  char *txt[8] = {
    "GPS to USCM A Port 0",
    "GPS to USCM A Port 1",
    "GPS to USCM B Port 0",
    "GPS to USCM B Port 1",
    "AST to USCM A Port 0",
    "AST to USCM A Port 1",
    "AST to USCM B Port 0",
    "AST to USCM B Port 1"};
  
  w = 5 + w1 + w2 + 2 * w3 + 5;
  h = 5 + h1 + 8 * h2 + 5 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+2*w3, h1, "USCM Serial Ports Inputs", FL_MEDIUM_SIZE);
  y = y + h1;
  
  rea_bit[ind].rea = malloc(4);
  rea_bit[ind].was = malloc(4);

  create_frame_box_vstring(8, x,    y, w1, h2, &dec[0], 'N' + 400, 'C'); 
  create_frame_box_vstring(8, x+w1, y, w2, h2, &txt[0], 'N' + 400, 'L');
  create_bit_btn_vstring(
    -8, x+w1+w2, y, w3, h2, hex, FL_MEDIUM_SIZE + 400, &wri_bit[ind], &BIT_BTN[ind][0]);
  create_data_box_vstring(8, x+w1+w2+w3, y, w3, h2, FL_MEDIUM_SIZE + 400, &BIT_BOX[ind][0]);
  y = y + 8 * h2 + 5;
  
  obj = add_frame_box(x, y, w1+w2, h2, "Command:", 'N');
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  obj = add_normal_button(x+w1+w2, y, w3, h2, "W", 'N');
    fl_set_object_callback(obj, write_register_callback, ind);
  obj = add_normal_button(x+w1+w2+w3, y, w3, h2, "R", 'N');
    fl_set_object_callback(obj, read_register_callback, ind);
}

//~----------------------------------------------------------------------------

void update_serial_ports_inputs_register_panel(void) {

  int ind = 2;

  _w *w = &wri_bit[ind];
  _r *r = &rea_bit[ind];

  show_bin_btn_string(-8, w, &BIT_BTN[ind][0]);
  show_bit_box_string(-8, r, NULL, &BIT_BOX[ind][0]);
}

//~============================================================================
//
//                  PULSE REGISTER PANEL
//
//~============================================================================

void create_pulse_register_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 20, w2 = 160, w3 = 20, h1 = 30, h2 = 20;

  int ind = 3;
  
  char *txt[6] = {
    "Generate PPS",
    "Open EPOCH Gate",
    "Close EPOCH Gate",
    "Open COM Gate",
    "Close COM Gate",
    "Reset GPS"};

  w = 5 + w1 + w2 + 2 * w3 + 5;
  h = 5 + h1 + 6 * h2 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+2*w3, h1, "Pulse Register", FL_MEDIUM_SIZE);
  y = y + h1;
  
  rea_bit[ind].rea = malloc(4);
  rea_bit[ind].was = malloc(4);

  create_frame_box_vstring(6, x,    y, w1, h2, &dec[0], 'N', 'C');
  create_frame_box_vstring(6, x+w1, y, w2, h2, &txt[0], 'N', 'L');
  create_bit_btn_vstring(
    -6, x+w1+w2, y, 2*w3, h2, hex, FL_MEDIUM_SIZE, &wri_bit[ind], &BIT_BTN[ind][0]);
  y = y + 6 * h2;
  
  obj = add_frame_box(x, y, w1+w2, h2, "Command:", 'N');
    fl_set_object_align(obj, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  obj = add_normal_button(x+w1+w2, y, 2*w3, h2, "W", 'N');
    fl_set_object_callback(obj, write_register_callback, ind);
}

//~----------------------------------------------------------------------------

void update_pulse_register_panel(void) {

  int ind = 3;

  _w *w = &wri_bit[ind];

  show_bin_btn_string(-6, w, &BIT_BTN[ind][0]);
}

//~============================================================================
//
//                  SERIAL PORTS RATES PANEL
//
//~============================================================================

void create_serial_ports_rates_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 45, w2 = 60, h1 = 30, h2 = 20;

  char *txt[2] = {"Port 0", "Port 1"};

  w = 5 + w1 + w2 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2+w1+w2, h1, "USCM Serial Ports Rates", FL_MEDIUM_SIZE);
  y = y + h1;
  
  for (i=0; i<2; i++) {
    add_frame_box(x, y, w1, h2, txt[i], 'N');
    x = x + w1;
    create_num_btn_hstring(
      1, x, y, w2, h2, rates, FL_MEDIUM_SIZE, 4, &rate[i], &RATE_BTN[i]);
    x = x + w2;
  }
}

//~============================================================================
//
//                  BEGIN EPOCH PANEL
//
//~============================================================================

void create_begin_epoch_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h;
  int w1 = 20, w2 = 345, w3 = 75, h1 = 20;
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  for (i=0; i<2; i++) {
    add_frame_box(x, y, w1, 2*h1, dec[i], 'N');
    for (j=0; j<2; j++) {
      SERIAL_INPUT[i][j] =
      obj = add_data_box(x+w1, y, w2, h1, "", FL_MEDIUM_SIZE);
        fl_set_object_align(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
      y = y + h1;
    }
  }

  x = x0 + 5;
  y = y0 + 5;

  obj = add_normal_button(x+w1+w2, y, w3, 4*h1, "BEGIN\nEPOCH", FL_LARGE_SIZE);
    fl_set_object_callback(obj, begin_epoch_callback, 0);
}

//~----------------------------------------------------------------------------

void begin_epoch_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat;
  int8 adr = 0x00;
  
  rea_bit[0].sta = status_NA;
  wri_bit[3].unk = TRUE;
  
  for (i=0; i<2; i++) {
    serial_input_sta[i] = status_NA;
  }
  
  msg = -1;
  update_objects();

  err = 0;

  if (!err) {
    write_LVDS_bus(adr+3, CLOSE_EPOCH_GATE, &err);
  }
  if (!err) {
    write_LVDS_bus(adr+3, CLOSE_COM_GATE, &err);
  }
  if (!err) {
    read_LVDS_bus(adr+0, &dat, &err);
  }
  if (!err) {
    dat &= 0xFF00;
    dat |= EPOCH_GATE_AUTO_OPEN | COM_GATE_AUTO_OPEN | COM_GATE_AUTO_CLOSE;
    write_LVDS_bus(adr+0, dat, &err);
  }
  
  if (!err) {
    setup_serial_port(0, 0x01, rate[0].inp, &err);
  }
  if (!err) {
    setup_serial_port(1, 0x01, rate[1].inp, &err);
  }
  
  if (!err) {
    write_LVDS_bus(adr+3, OPEN_EPOCH_GATE, &err);
  }
  
  if (!err) {
    sleep(1);
    sleep(1);
  }
  
  if (!err) {
    read_serial_port(0, 512, &serial_input_cnt[0], &serial_input_dat[0][0], &err);
  }
  if (!err) {
    read_serial_port(1, 512, &serial_input_cnt[1], &serial_input_dat[1][0], &err);
  }
  if (!err) {
    disable_serial_port(0, &err);
  }
  if (!err) {
    disable_serial_port(1, &err);
  }

  if (!err) {
    for (i=0; i<2; i++) {
      serial_input_unk[i] = FALSE;
      serial_input_sta[i] = status_OK;
    }
  }
  else {
    for (i=0; i<2; i++) {
      serial_input_sta[i] = status_KO;
    }
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_begin_epoch_panel(void) {

  FL_OBJECT *obj;
  int i, j;
  int c, lc;
  
  for (i=0; i<2; i++) {
    char b[80];
    char *p = b;
    sprintf(p, " ");
    if (!serial_input_unk[i]) {
      for (j=0; j<serial_input_cnt[i]; j++) {
        if (j > 11) {
          sprintf(p-1, "...");
          break;
        }
        p += sprintf(p, "%02X ", serial_input_dat[i][j]);
      }
    }
    obj = SERIAL_INPUT[i][0];
    fl_set_object_label(obj, b);
  }

  for (i=0; i<2; i++) {
    char b[80];
//  int week = i ?   1045 :   1035;
//  int sec  = i ? 846001 : 846000;
    int16 week;
    int32  sec;
printf("ID = %02X%02X\n", serial_input_dat[i][2], serial_input_dat[i][3]);

printf("WK  = %02X%02X\n", 
       serial_input_dat[i][ 6+8], serial_input_dat[i][ 7+8]);
printf("sec = %02X%02X%02X%02X\n", 
       serial_input_dat[i][ 8+8], serial_input_dat[i][ 9+8],
       serial_input_dat[i][10+8], serial_input_dat[i][11+8]);

printf("WK   = %02X%02X\n", 
       serial_input_dat[i][20+8], serial_input_dat[i][21+8]);
printf("sec = %02X%02X%02X%02X\n", 
       serial_input_dat[i][22+8], serial_input_dat[i][23+8],
       serial_input_dat[i][24+8], serial_input_dat[i][25+8]);

printf("loc = %02X%02X%02X%02X\n", 
       serial_input_dat[i][36+8], serial_input_dat[i][37+8],
       serial_input_dat[i][38+8], serial_input_dat[i][39+8]);

    week =               serial_input_dat[i][14];
    week = (week << 8) + serial_input_dat[i][15];
    sec  =               serial_input_dat[i][16];
    sec  = (sec  << 8) + serial_input_dat[i][17];
    sec  = (sec  << 8) + serial_input_dat[i][18];
    sec  = (sec  << 8) + serial_input_dat[i][19];
    if (!serial_input_unk[i]) sprintf(b, "WK=%d Sec=%d", week, sec);
    else                      sprintf(b, " ");
    obj = SERIAL_INPUT[i][1];
    fl_set_object_label(obj, b);
  }

  for (i=0; i<2; i++) {
    for (j=0; j<2; j++) {
      obj = SERIAL_INPUT[i][j];
      c  = FL_GREEN;
      lc = FL_BLACK;
      if (serial_input_sta[i] == status_NA) c = FL_YELLOW;
      if (serial_input_sta[i] == status_KO) {
        c  = FL_BLACK;
        lc = FL_WHITE;
      }
      fl_set_object_color( obj,  c, 0);
      fl_set_object_lcolor(obj, lc);
    }
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_control_and_status_register_panel();
  update_serial_ports_outputs_register_panel();
  update_serial_ports_inputs_register_panel();
  update_pulse_register_panel();
  update_begin_epoch_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  return 0;
}

//~============================================================================
