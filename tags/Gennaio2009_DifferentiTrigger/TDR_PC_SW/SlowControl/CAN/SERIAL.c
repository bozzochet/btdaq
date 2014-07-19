// file SERIAL.c
//
// Serial TX and RX Controllers 
// (Two different forms are built for TX and for RX)
//
// A.Lebedev Feb-2006...

#include "template.h"
#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *titles[2] = {"Serial  RX  Controller",
                   "Serial  TX  Controller"};
char *title;                                          // global variable
char *date  = {"13-Dec-08"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

bool is_TX;

//~-- Serial Ports Configuration Panel (used by both TX and RX)

SW_BTN *ENA_BTN[2], *RATE_BTN[2], *ROLL_BTN[2];
_w w_ena[2], w_rate[2], w_roll[2];
FL_OBJECT *ENA_BOX[2], *RATE_BOX[2], *ROLL_BOX[2];
_r r_ena[2], r_rate[2], r_roll[2];

//~-- Pattern Configuration Panel (used by both TX and partially by RX)

SW_BTN *TX_SIZE_BTN, *LEN_DIG_BTN[2][4], *PAT_BTN[2];
FL_OBJECT *PAT_VIEW_BOX[2];
_w w_sizes, w_len[2], w_pat[2];
int8 TX_dat[2][10000];

//~-- TX Control Panel (used by TX only)

SW_BTN *TX_PORTS_BTN, *TX_DELAY_BTN;
_w w_tx_ports, w_tx_delay;
bool auto_TX = FALSE;

//~-- Pattern Display Panel (used by RX only)

FL_OBJECT *RCV_CNT_BOX[2], *LEN_BOX[2], *Z_BOX[2];
FL_OBJECT *DATA_BOX[2], *CHK_BOX[2], *ERR_CNT_BOX[2];
_r r_len[2], r_z[2];
int8 RX_dat[2][10000];
int rcv_cnt[2] = {0, 0};
int rcv_tot[2] = {0, 0};
bool chk_OK[2] = {TRUE, TRUE};
int err_cnt[2] = {0, 0};

//~-- RX Control Panel (used by RX only)

SW_BTN *RX_PORTS_BTN;
_w w_rx_ports;
bool auto_RX = FALSE;

//~--

char *_01[3] = {"0", "1", "0&1"};

char *no_yes[2] = {"NO", "YES"};

char *d_e[2] = {"D", "E"};

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *rates[5] = {"2400", "4800", "9600", "19200", "?"};

char *delays[7] = {"0", "0.1", "0.2", "0.5", "1.0", "2.0", "5.0"};

char *sizes[2] = {"512", "4096"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_ports_config_panel(int x0, int y0);
void get_ports_config_callback(FL_OBJECT *obj, long data);
void set_ports_config_callback(FL_OBJECT *obj, long data);
void update_ports_config_panel(void);

void create_pattern_config_panel(int x0, int y0);
void pattern_config_callback(int32 data);
void update_pattern_config_panel(void);

void create_TX_control_panel(int x0, int y0);
void TX_callback(FL_OBJECT *obj, long data);
void auto_TX_callback(int tid, void *data);

void create_pattern_display_panel(int x0, int y0);
void reset_err_cnt_callback(FL_OBJECT *obj, long data);
void update_pattern_display_panel(void);

void create_RX_control_panel(int x0, int y0);
void RX_callback(FL_OBJECT *obj, long data);
void auto_RX_callback(int tid, void *data);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW, HH;

  if (is_TX) {WW = 505; HH = 205;}
  else       {WW = 700; HH = 205;}
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

             create_ports_config_panel(     0,  45);
  if (is_TX) create_pattern_config_panel( 195,  45);
  else       create_pattern_display_panel(195,  45);
  if (is_TX) create_TX_control_panel(     195, 135);
  else       create_RX_control_panel(     195, 135);

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
  
  for (i=0; i<2; i++) {
    w_ena[i].inp  = 0;
    w_ena[i].unk  = TRUE;
    w_rate[i].inp = 3;
    w_rate[i].unk = TRUE;
    w_roll[i].inp = 0;
    w_roll[i].unk = TRUE;

    r_ena[i].unk  = TRUE;
    r_ena[i].sta  = status_NA;
    r_rate[i].unk = TRUE;
    r_rate[i].sta = status_NA;
    r_roll[i].unk = TRUE;
    r_roll[i].sta = status_NA;

    w_len[i].inp = 256;
    w_pat[i].inp = 0;

    r_len[i].unk  = TRUE;
    r_len[i].sta  = status_NA;
    r_z[i].unk    = TRUE;
    r_z[i].sta    = status_NA;
  }

  w_sizes.inp    = 0;
  w_tx_ports.inp = 2;
  w_tx_delay.inp = 0;

  w_rx_ports.inp = 2;
  
  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    for (i=0; i<2; i++) {
      fread(&w_ena[i].inp,  sizeof(w_ena[i].inp),  1, file);
      fread(&w_rate[i].inp, sizeof(w_rate[i].inp), 1, file);
      fread(&w_roll[i].inp, sizeof(w_roll[i].inp), 1, file);
      fread(&w_len[i].inp,  sizeof(w_len[i].inp),  1, file);
      fread(&w_pat[i].inp,  sizeof(w_pat[i].inp),  1, file);
    }
    fread(&w_sizes.inp,    sizeof(w_sizes.inp),    1, file);
    fread(&w_tx_ports.inp, sizeof(w_tx_ports.inp), 1, file);
    fread(&w_tx_delay.inp, sizeof(w_tx_delay.inp), 1, file);
    fread(&w_rx_ports.inp, sizeof(w_tx_ports.inp), 1, file);
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
    for (i=0; i<2; i++) {
      fwrite(&w_ena[i].inp,  sizeof(w_ena[i].inp),  1, file);
      fwrite(&w_rate[i].inp, sizeof(w_rate[i].inp), 1, file);
      fwrite(&w_roll[i].inp, sizeof(w_roll[i].inp), 1, file);
      fwrite(&w_len[i].inp,  sizeof(w_len[i].inp),  1, file);
      fwrite(&w_pat[i].inp,  sizeof(w_pat[i].inp),  1, file);
    }
    fwrite(&w_sizes.inp,    sizeof(w_sizes.inp),    1, file);
    fwrite(&w_tx_ports.inp, sizeof(w_tx_ports.inp), 1, file);
    fwrite(&w_tx_delay.inp, sizeof(w_tx_delay.inp), 1, file);
    fwrite(&w_rx_ports.inp, sizeof(w_tx_ports.inp), 1, file);
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
//                  SERIAL PORTS CONFIGURATION PANEL
//
//~============================================================================

int8 mode;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_ports_config_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 25, w3 = 55, w4 = 35, w5 = 40, h1 = 20;

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + h1 + 2 * 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");


  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "Serial Ports Configuration", FL_NORMAL_SIZE);

  y = y + h1;

  add_frame_box(x, y, w1, h1, "Port", FL_NORMAL_SIZE);
  x = x + w1;
  add_frame_box(x, y, w2, h1, "E/D",  FL_NORMAL_SIZE);
  x = x + w2;
  add_frame_box(x, y, w3, h1, "Rate", FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "Roll", FL_NORMAL_SIZE);
  x = x + w4;
  add_frame_box(x, y, w5, h1, "Cmd.", FL_NORMAL_SIZE);
  x = x + w5;

  y = y + h1;
  
  for (i=0; i<2; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, 2*h1, hex[i], FL_NORMAL_SIZE);
    x = x + w1;
    create_data_box_hstring(
      1, x, y,    w2, h1,        FL_MEDIUM_SIZE,                &ENA_BOX[i]);
    create_num_btn_hstring(
      1, x, y+h1, w2, h1, d_e,   FL_MEDIUM_SIZE, 2, &w_ena[i],  &ENA_BTN[i]);
    x = x + w2;
    create_data_box_hstring(
      1, x, y,    w3, h1,        FL_MEDIUM_SIZE,                &RATE_BOX[i]);
    create_num_btn_hstring(
      1, x, y+h1, w3, h1, rates, FL_MEDIUM_SIZE, 4, &w_rate[i], &RATE_BTN[i]);
    x = x + w3;
    create_data_box_hstring(
      1, x, y,    w4, h1,         FL_MEDIUM_SIZE,                &ROLL_BOX[i]);
    create_num_btn_hstring(
      1, x, y+h1, w4, h1, no_yes, FL_MEDIUM_SIZE, 2, &w_roll[i], &ROLL_BTN[i]);
    x = x + w4;
    obj = add_normal_button(x, y,    w5, h1, "GET", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, get_ports_config_callback, i);
    obj = add_normal_button(x, y+h1, w5, h1, "SET", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, set_ports_config_callback, i);
    x = x + w5;
    y = y + 2 * h1;
    r_ena[i].rea  = malloc(4);
    r_ena[i].was  = malloc(4);
    r_rate[i].rea = malloc(4);
    r_rate[i].was = malloc(4);
    r_roll[i].rea = malloc(4);
    r_roll[i].was = malloc(4);
  }
}

//~----------------------------------------------------------------------------

void get_ports_config_callback(FL_OBJECT *obj, long data) {

  int8 port = data;
  int8 rate, mask;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;
  
  *r_ena[port].was  = *r_ena[port].rea;
  *r_rate[port].was = *r_rate[port].rea;
  *r_roll[port].was = *r_roll[port].rea;
  
  check_serial_port(port, &mode, &rate, &err);

  if (!err) {
    *r_ena[port].rea  = mode & SERIAL_PORT_ENABLE ? TRUE : FALSE;
    *r_rate[port].rea = rate;
    if (is_TX) mask = SERIAL_TX_ROLL;
    else       mask = SERIAL_RX_ROLL;
    *r_roll[port].rea = mode & mask ? TRUE : FALSE;
    if (r_ena[port].unk)  *r_ena[port].was  = *r_ena[port].rea;
    if (r_rate[port].unk) *r_rate[port].was = *r_rate[port].rea;
    if (r_roll[port].unk) *r_roll[port].was = *r_roll[port].rea;
    r_ena[port].unk  = FALSE;
    r_rate[port].unk = FALSE;
    r_roll[port].unk = FALSE;
    r_ena[port].sta  = status_OK;
    r_rate[port].sta = status_OK;
    r_roll[port].sta = status_OK;
  }
  else {
    r_ena[port].sta  = status_KO;
    r_rate[port].sta = status_KO;
    r_roll[port].sta = status_KO;
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void set_ports_config_callback(FL_OBJECT *obj, long data) {

  int8 port = data;
  int8 mask;
  
  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  if (!err) {
    get_ports_config_callback(NULL, (long)port);
  }

  if (!err) {
    r_ena[port].sta  = status_NA;
    r_rate[port].sta = status_NA;
    r_roll[port].sta = status_NA;

    if (is_TX) mask = SERIAL_TX_ROLL;
    else       mask = SERIAL_RX_ROLL;
    mode = mode & ~(mask | SERIAL_PORT_ENABLE);

    if (w_ena[port].inp)  mode |= SERIAL_PORT_ENABLE;
    if (w_roll[port].inp) mode |= mask;

    setup_serial_port(port, mode, w_rate[port].inp, &err);

    if (!err) {
      w_ena[port].wri  = w_ena[port].inp;
      w_rate[port].wri = w_rate[port].inp;
      w_roll[port].wri = w_roll[port].inp;
      w_ena[port].unk  = FALSE;
      w_rate[port].unk = FALSE;
      w_roll[port].unk = FALSE;
    }
    else {
      w_ena[port].unk  = TRUE;
      w_rate[port].unk = TRUE;
      w_roll[port].unk = TRUE;
    }
  }
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void update_ports_config_panel(void) {

  int i;
  
  for (i=0; i<2; i++) {
    show_hex_box_string(1, &r_ena[i],  d_e,    &ENA_BOX[i]);
    show_hex_box_string(1, &r_rate[i], rates,  &RATE_BOX[i]);
    show_hex_box_string(1, &r_roll[i], no_yes, &ROLL_BOX[i]);
    show_bin_btn_string(1, &w_ena[i],          &ENA_BTN[i]);
    show_bin_btn_string(1, &w_rate[i],         &RATE_BTN[i]);
    show_bin_btn_string(1, &w_roll[i],         &ROLL_BTN[i]);
  }
}

//~============================================================================
//
//                  PATTERN CONFIGURATION PANEL (TX only)
//
//~============================================================================

void create_pattern_config_panel(int x0, int y0) {

  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 225, w3 = 45, w4 = 15, w5 = 50, w6 = 160, h1 = 20;

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1,       2*h1, "Port",    FL_NORMAL_SIZE);
  x = x + w1;
  add_frame_box(x, y, w2, h1, "Transmit assuming RX Buffer Size:", FL_NORMAL_SIZE);
  x = x + w2;
  create_num_btn_hstring(
    1, x, y, w3, h1, sizes, FL_MEDIUM_SIZE, 2, &w_sizes, &TX_SIZE_BTN);
  x = x0 + 5 + w1;
  y = y + h1;
  add_frame_box(x, y, 4*w4,       h1, "Length",  FL_NORMAL_SIZE);
  x = x + 4*w4;
  add_frame_box(x, y, w5,         h1, "Pattern", FL_NORMAL_SIZE);
  x = x + w5;
  add_frame_box(x, y, w6,         h1, "Preview", FL_NORMAL_SIZE);
  x = x + w6;

  y = y + h1;
  
  for (i=0; i<2; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, h1, hex[i], FL_NORMAL_SIZE);
    x = x + w1;
//printf("create: w_len.inp = %d %d\n", w_len[0].inp, w_len[1].inp); 

    create_dec_btn_hstring(
      4, x, y, w4, h1, hex, FL_MEDIUM_SIZE,    &w_len[i], &LEN_DIG_BTN[i][0]);
    x = x + 4 * w4;
    create_num_btn_hstring(
      1, x, y, w5, h1, hex, FL_MEDIUM_SIZE, 4, &w_pat[i], &PAT_BTN[i]);
    PAT_BTN[i]->call = pattern_config_callback;
    x = x + w5;
    create_data_box_hstring(
      1, x, y, w6, h1,      FL_MEDIUM_SIZE,               &PAT_VIEW_BOX[i]);
    x = x + w6;
    y = y + h1;
  }
  pattern_config_callback(0);
}

//~----------------------------------------------------------------------------

void pattern_config_callback(int32 data) {

  int i, j;
  for (j=0; j<2; j++) {
    for (i=0; i<10000; i++) {
      switch (w_pat[j].inp) {
        case 0:
          TX_dat[j][i] = i & 0xFF;
        break;
        case 1:
          TX_dat[j][i] = 0xFF - (i & 0xFF);
        break;
        case 2:
          TX_dat[j][i] = 0x00;
        break;
        case 3:
          TX_dat[j][i] = 0xFF;
        break;
      }
    }
  }
  SW_BTN_CALLBACK(0);
}

//~----------------------------------------------------------------------------

void update_pattern_config_panel(void) {

  int i, j;
  for (j=0; j<2; j++) {
    char b[80], *p = b;
    for (i=0; i<6; i++) {
      p += sprintf(p, "%s%02hhX",  i ? " " : "", TX_dat[j][i]);
      fl_set_object_label(PAT_VIEW_BOX[j], b);
    }
    fl_set_object_color( PAT_VIEW_BOX[j], FL_GREEN, 0);
    fl_set_object_lcolor(PAT_VIEW_BOX[j], FL_BLACK);
  }
};

//~============================================================================
//
//                  TX CONTROL PANEL (TX only)
//
//~============================================================================

void create_TX_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 45, w3 = 55, w4 = 45, w5 = 100, h1 = 30;

  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");


  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "Ports", FL_NORMAL_SIZE);
  x = x + w1;
  create_num_btn_hstring(
    1, x, y, w2, h1, _01,    FL_MEDIUM_SIZE, 3, &w_tx_ports,  &TX_PORTS_BTN);
  x = x + w2;
  add_frame_box(x, y, w3, h1, "Delay", FL_NORMAL_SIZE);
  x = x + w3;
  create_num_btn_hstring(
    1, x, y, w4, h1, delays, FL_MEDIUM_SIZE, 7, &w_tx_delay,  &TX_DELAY_BTN);
  x = x + w4;

  obj = add_normal_button(x, y, w5, h1, "SEND", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, TX_callback, 0);
}

//~----------------------------------------------------------------------------

void TX_callback(FL_OBJECT *obj, long data) {

//printf("send:   w_len.inp = %d %d\n", w_len[0].inp, w_len[1].inp); 
  if (obj) {
    if (auto_TX) {
      auto_TX = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        int period = 1000.0 * atof(delays[w_tx_delay.inp]);
        auto_TX = TRUE;
        fl_add_timeout(period, auto_TX_callback, NULL);
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_TX) {
    msg = -1;
    update_objects();
  }
  
  err = 0;

  serial_RX_buffer_size = atoi(sizes[w_sizes.inp]);
  
  if (!err) {
    if ((w_tx_ports.inp == 0) || (w_tx_ports.inp == 2)) {
      Write_serial_port(0, w_len[0].inp, TX_dat[0], &err);
    }
  }
  
  if (!err) {
    if ((w_tx_ports.inp == 1) || (w_tx_ports.inp == 2)) {
      Write_serial_port(1, w_len[1].inp, TX_dat[1], &err);
    }
  }
  
  if (auto_TX) msg = 0;
  else         msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void auto_TX_callback(int tid, void *data) {

  int period = 1000.0 * atof(delays[w_tx_delay.inp]);
  
  if (!auto_TX) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback dt = %f\n", delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  TX_callback(NULL, 0);
  fl_add_timeout(period, auto_TX_callback, NULL);
}

//~============================================================================
//
//                  PATTERN DISPLAY PANEL (RX only)
//
//~============================================================================

void create_pattern_display_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 15, w3 = 45, w4 = 25, w5 = 175;
  int w6 = 15, w7 = 50, w8 = 40, w9 = 55;
  int wa = 245, wb = 150, wc = 55;
  int h1 = 20;

  w = 5 + w1 + w2 + w3 + w4 + w5 + 4 * w6 + w7 + w8 + w9 + 5;
  h = 5 + 4 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");


  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x,    y,   w1, 2*h1, "Port",     FL_NORMAL_SIZE);
  x = x + w1;
  add_frame_box(x,    y,   w2, 2*h1, "",         FL_NORMAL_SIZE);
  x = x + w2;
  add_frame_box(x,    y,   wa, h1,   "Received", FL_NORMAL_SIZE);
  add_frame_box(x+wa, y,   wb, h1,   "Expected", FL_NORMAL_SIZE);
  add_frame_box(x+wa+wb, y,wc, h1,   "Err Cnt",  FL_NORMAL_SIZE);
  y = y + h1;
  add_frame_box(x,    y,   w3, h1,   "Length",   FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x,    y,   w4, h1,   "z",        FL_NORMAL_SIZE);
  x = x + w4;
  add_frame_box(x,    y,   w5, h1,   "Data",     FL_NORMAL_SIZE);
  x = x + w5;
  add_frame_box(x,    y, 4*w6, h1,   "Length",   FL_NORMAL_SIZE);
  x = x + 4 * w6;
  add_frame_box(x,    y,   w7, h1,   "Pattern",  FL_NORMAL_SIZE);
  x = x + w7;
  add_frame_box(x,    y,   w8, h1,   "Check",    FL_NORMAL_SIZE);
  x = x + w8;
  obj = add_normal_button(x, y, wc, h1, "RESET", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, reset_err_cnt_callback, 0);
  x = x + w9;

  y = y + h1;
  
  for (i=0; i<2; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, h1, hex[i], FL_NORMAL_SIZE);
    x = x + w1;
    create_data_box_hstring(1, x, y, w2, h1, FL_MEDIUM_SIZE, &RCV_CNT_BOX[i]);
    x = x + w2;
    create_data_box_hstring(1, x, y, w3, h1, FL_MEDIUM_SIZE, &LEN_BOX[i]);
    x = x + w3;
    create_data_box_hstring(1, x, y, w4, h1, FL_MEDIUM_SIZE, &Z_BOX[i]);
    x = x + w4;
    create_data_box_hstring(1, x, y, w5, h1, FL_MEDIUM_SIZE, &DATA_BOX[i]);
    x = x + w5;
    create_dec_btn_hstring(
      4, x, y, w6, h1, hex, FL_MEDIUM_SIZE,    &w_len[i], &LEN_DIG_BTN[i][0]);
    x = x + 4 * w6;
    create_num_btn_hstring(
      1, x, y, w7, h1, hex, FL_MEDIUM_SIZE, 4, &w_pat[i], &PAT_BTN[i]);
    PAT_BTN[i]->call = pattern_config_callback;
    x = x + w7;
    create_data_box_hstring(1, x, y, w8, h1, FL_MEDIUM_SIZE, &CHK_BOX[i]);
    x = x + w8;
    create_data_box_hstring(1, x, y, w9, h1, FL_MEDIUM_SIZE, &ERR_CNT_BOX[i]);
    x = x + w9;
    y = y + h1;

    r_len[i].rea = malloc(4);
    r_len[i].was = malloc(4);
    r_z[i].rea   = malloc(4);
    r_z[i].was   = malloc(4);
  }

  pattern_config_callback(0);
}

//~----------------------------------------------------------------------------

void reset_err_cnt_callback(FL_OBJECT *obj, long data) {

  int i;
  
  for (i=0; i<2; i++) {
    err_cnt[i] = 0;
    rcv_tot[i] = 0;
  }
  
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_pattern_display_panel(void) {

  int i;
  
  for (i=0; i<2; i++) {
    FL_OBJECT *obj;
    int j;
    char b[80], *p;
    if (!auto_RX || (auto_RX && *r_len[i].rea)) {
      sprintf(b, "%d", rcv_cnt[i]);
      obj = RCV_CNT_BOX[i];
      fl_set_object_label(obj, b);
      fl_set_object_color( obj, FL_GREEN, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
      show_num_box_string(1, &r_len[i], NULL, "%d", &LEN_BOX[i]);
      show_num_box_string(1, &r_z[i],   NULL, "%d", &Z_BOX[i]);
      p = b;
      sprintf(p, " ");
      for (j=0; j<MIN(6, *r_len[i].rea); j++) {
        p += sprintf(p, "%02hhX ", RX_dat[i][j]);
      }
      show_num_box_string(1, &r_len[i], NULL, b,    &DATA_BOX[i]);
      obj = CHK_BOX[i];
      if (chk_OK[i]) {
        fl_set_object_label(obj, "OK");
        fl_set_object_color( obj, FL_GREEN, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
      else {
        fl_set_object_label(obj, "ERR");
        fl_set_object_color( obj, FL_RED, 0);
        fl_set_object_lcolor(obj, FL_YELLOW);
      }
    }
    sprintf(b, "%d", err_cnt[i]);
    obj = ERR_CNT_BOX[i];
    fl_set_object_label(obj, b);
    fl_set_object_color( obj, FL_GREEN, 0);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
}

//~============================================================================
//
//                  RX CONTROL PANEL (RX only)
//
//~============================================================================

void create_RX_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 55, w2 = 45, w3 = 395, h1 = 30;

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");


  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "Ports", FL_NORMAL_SIZE);
  x = x + w1;
  create_num_btn_hstring(
    1, x, y, w2, h1, _01,    FL_MEDIUM_SIZE, 3, &w_rx_ports,  &RX_PORTS_BTN);
  x = x + w2;

  obj = add_normal_button(x, y, w3, h1, "RECEIVE", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, RX_callback, 0);
}

//~----------------------------------------------------------------------------

void RX_callback(FL_OBJECT *obj, long data) {

  int i, j;
  
  if (obj) {
    if (auto_RX) {
      auto_RX = FALSE;
      set_normal_button(obj, 0);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_RX = TRUE;
        set_normal_button(obj, 1);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_RX) {
    msg = -1;
    update_objects();
  }
  
  for (i=0; i<2; i++) {
    if ((w_rx_ports.inp == i) || (w_rx_ports.inp == 2)) {
      *r_len[i].was = *r_len[i].rea;
      *r_z[i].was   = *r_z[i].rea;
      *r_len[i].was = *r_len[i].rea;
      *r_z[i].was   = *r_z[i].rea;
      *r_z[i].rea = Read_serial_port(i, 10000, (int*)r_len[i].rea, &RX_dat[i][0], &err);
      if (!err) {
        if (*r_len[i].rea) {
          rcv_cnt[i] = MOD(rcv_cnt[i]+1, 10);
          rcv_tot[i] = rcv_tot[i]+1;
          if (r_len[i].unk) *r_len[i].was  = *r_len[i].rea;
          if (r_z[i].unk)   *r_z[i].was = *r_z[i].rea;
          r_len[i].unk = FALSE;
          r_z[i].unk   = FALSE;
          r_len[i].sta = status_OK;
          r_z[i].sta   = status_OK;
          chk_OK[i] = (*r_len[i].rea == w_len[i].inp) ? TRUE : FALSE;
          if (chk_OK[i]) {
            for (j=0; j<*r_len[i].rea; j++) {
              if (TX_dat[i][j] != RX_dat[i][j]) {
                chk_OK[i] = FALSE;
                if (P > 0) {
                  printf("port%d:%d:%d Data mismatch: RX[%d]=%02hhX, TX[%d]=%02hhX\n",
                         i, rcv_tot[i], err_cnt[i]+1,
                         j, RX_dat[i][j], j, TX_dat[i][j]);
                }
                break;
              }
            }
          }
          else {
            if (P > 0) {
              printf("port%d:%d:%d Length mismatch: RX:%d, TX:%d\n",
                     i, rcv_tot[i], err_cnt[i]+1,
                     *r_len[i].rea, w_len[i].inp);
            }
          }
          if (!chk_OK[i]) err_cnt[i]++;
        }
      }
      else {
        if (P > 0) {
          printf("port%d:%d:%d Error %04X\n", i, rcv_tot[i], err_cnt[i]+1, err);
        }
        r_len[i].sta = status_KO;
        r_z[i].sta   = status_KO;
      }
    }
  }

  if (auto_RX) msg = 0;
  else         msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void auto_RX_callback(int tid, void *data) {

  if (!auto_RX) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback dt = %f\n", delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  RX_callback(NULL, 0);
  fl_add_timeout(0, auto_RX_callback, NULL);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_ports_config_panel();
  if (is_TX) update_pattern_config_panel();
  else       update_pattern_display_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s W|TX|R|RX\n", argv[0]);
  if (argc < 2) {
    printf("Too few arguments\n");
    exit(1);
  }
  if     ((strcasecmp(argv[1], "W")  == 0)  ||
          (strcasecmp(argv[1], "TX") == 0)) {
    is_TX = TRUE;
  }
  else if ((strcasecmp(argv[1], "R")  == 0)  ||
           (strcasecmp(argv[1], "RX") == 0)) {
    is_TX = FALSE;
  }
  else {
    printf("Wrong first argument\n");
    exit(1);
  }
  
  title = titles[is_TX];
  printf("Will run %s\n", title);
  
  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
