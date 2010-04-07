// file PDS-4.c
//
// PDS-4 Pro Controller
//
// A.Lebedev May-2005...

#include "template.h"
#include "pdsdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"PDS-4 Pro Controller"};               // global variable
char *date  = {"26-Oct-08"};                          // global variable

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

#define NL 32

//~--

SW_BTN *C_SIDE_BTN;
_w c_side;

SW_BTN *M_SIDE_BTN;
_w m_side;

FL_OBJECT *NAM_BOX, *ADR_BOX, *ADC_BOX;
int8 slot;

FL_OBJECT *INP_COD_BOX[4], *INP_VAL_BOX[4];
_r inp_cod;
float inp_val[4];

FL_OBJECT *DIG_CHA_BOX[NL], *DIG_NAM_BOX[NL], *DIG_COD_BOX[NL], *DIG_TXT_BOX[NL];
_dig *pdig[NL];
_r dig_cod[NL];
SW_BTN *DIG_PER_BTN;
_ww w_dig_per;
_b dig_per = {0, 0x0007, 6};
bool auto_read_digital = FALSE;

FL_OBJECT *ANA_CHA_BOX[NL], *ANA_NAM_BOX[NL], *ANA_COD_BOX[NL], *ANA_VAL_BOX[NL];
_ana *pana[NL];
_r ana_cod[NL];
float A[NL], B[NL], ana_val[NL];
SW_BTN *ANA_PER_BTN;
_ww w_ana_per;
_b ana_per = {0, 0xFFFF, 6};
bool auto_read_analog = FALSE;

FL_OBJECT *CMD_CHA_BOX[NL], *CMD_NAM_BOX[NL];
SW_BTN *CMD_DAT_BTN[NL], *CMD_TXT_BTN[NL];
_cmd *pcmd[NL];
_w cmd_dat[NL];

//~--

char *bin[2]  = {"0", "1"};
char *a_b[2]  = {"A", "B"};
char *dash[2] = {"-", "-"};
char *periods[6] = {"0.2s", "0.5s", "1.0s", "2.0s", "5.0s", "10.0s"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void write_config_and_refresh_objects(int32 data);

void create_can_bus_i_f_selector_panel(int x0, int y0);
void update_can_bus_i_f_selector_panel(void);

void create_board_selector_panel(int x0, int y0);
void board_selector_callback(FL_OBJECT *obj, long data);
void update_board_selector_panel(void);

void create_input_stage_panel(int x0, int y0);
void input_stage_callback(FL_OBJECT *obj, long data);
void update_input_stage_panel(void);

void create_digital_readout_panel(int x0, int y0);
void digital_readout_callback(FL_OBJECT *obj, long data);
void auto_read_digital_callback(int tid, void *data);
void update_digital_readout_panel(void);

void create_analog_readout_panel(int x0, int y0);
void analog_readout_callback(FL_OBJECT *obj, long data);
void auto_read_analog_callback(int tid, void *data);
void update_analog_readout_panel(void);

void create_digital_commanding_panel(int x0, int y0);
void digital_commanding_callback(FL_OBJECT *obj, long data);
void update_digital_commanding_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 845, HH = 605;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
//AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_can_bus_i_f_selector_panel(0,  45);
  create_board_selector_panel(      0,  75);
  create_input_stage_panel(         0, 455);
  create_digital_readout_panel(   140,  45);
  create_analog_readout_panel(    375,  45);
  create_digital_commanding_panel(650,  45);
  
  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  static bool first = TRUE;
  
  c_side.inp = 0;
  c_side.unk = TRUE;

  m_side.inp = 0;
  m_side.unk = TRUE;

  slot = 0;
  
  for (i=0; i<NL; i++) cmd_dat[i].inp = 0;
  for (i=0; i<NL; i++) cmd_dat[i].unk = TRUE;

  for (i=0; i<NL; i++) ana_cod[i].unk = TRUE;
  for (i=0; i<NL; i++) ana_cod[i].sta = status_OK;
  
  inp_cod.unk = TRUE;
  inp_cod.sta = status_OK;
  
  for (i=0; i<NL; i++) dig_cod[i].unk = TRUE;
  for (i=0; i<NL; i++) dig_cod[i].sta = status_OK;

  invalidate_ww(first, &w_dig_per, 1, 0);
  invalidate_ww(first, &w_ana_per, 1, 0);

  read_dat_config_file(dat_config_file_name);
  first = FALSE;

  if (c_side.inp >1) c_side.inp = 0;
  if (m_side.inp >1) m_side.inp = 0;
  for (i=0; i<NL; i++) if (cmd_dat[i].inp > 1) cmd_dat[i].inp = 0;
  if (slot > 9) slot = 0;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(&c_side.inp, sizeof(c_side.inp), 1, file);
    fread(&m_side.inp, sizeof(m_side.inp), 1, file);
    fread(&slot, sizeof(slot),             1, file);
    for (i=0; i<NL; i++) fread(&cmd_dat[i].inp, sizeof(cmd_dat[i].inp), 1, file);
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
    fwrite(&c_side.inp, sizeof(c_side.inp), 1, file);
    fwrite(&m_side.inp, sizeof(m_side.inp), 1, file);
    fwrite(&slot, sizeof(slot),             1, file);
    for (i=0; i<NL; i++) fwrite(&cmd_dat[i].inp, sizeof(cmd_dat[i].inp), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

void refresh_objects(void) {

  invalidate_data();
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void write_config_and_refresh_objects(int32 data) {

  write_dat_config_file(dat_config_file_name);
  invalidate_data();
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void switch_btn_callback(int32 data) {

  msg = 0;
  update_objects();
}

//~============================================================================
//
//                  CAN_BUS_I/F_SELECTOR_PANEL
//
//~============================================================================

void create_can_bus_i_f_selector_panel(int x0, int y0) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 110, w2 = 20;
  int h1 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "CAN Bus I/F Side", FL_NORMAL_SIZE);
  x = x + w1;
  C_SIDE_BTN =
  B = add_switch_btn(x, y, w2, h1, a_b, FL_MEDIUM_SIZE, 2, -1);
  B->m = 0x00000001;
  B->p = &c_side.inp;
  B->i = unpack32(*B->p, B->m);
  fl_set_object_label(B->obj, B->l[B->i]);
  B->call = write_config_and_refresh_objects;
}

//~----------------------------------------------------------------------------

void update_can_bus_i_f_selector_panel(void) {
}

//~============================================================================
//
//                  BOARD_SELECTOR_PANEL
//
//~============================================================================

void create_board_selector_panel(int x0, int y0) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 50, w2 = 40, w3 = 40, w4 = 30, w5 = 100;
  int h1 = 35, h2 = 20, h3 = 15, h4 = 30;
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + h2 + h3 + 10 * h4 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  NAM_BOX =
  obj = add_frame_box(x, y, w1+w2+w3, h1, "", FL_MEDIUM_SIZE);

  x = x0 + 5;
  y = y + h1;
  obj = add_frame_box(x, y, w1, h2, "Side", FL_NORMAL_SIZE);
  M_SIDE_BTN =
  B = add_switch_btn(x, y+h2, w1, h3, a_b, FL_MEDIUM_SIZE, 2, -1);
  B->m = 0x00000001;
  B->p = &m_side.inp;
  B->i = unpack32(*B->p, B->m);
  fl_set_object_label(B->obj, B->l[B->i]);
  B->call = write_config_and_refresh_objects;
  x = x + w1;
  ADR_BOX =
  obj = add_frame_box(x, y, w2, h2+h3, "", FL_NORMAL_SIZE);
  x = x + w2;
  ADC_BOX =
  obj = add_frame_box(x, y, w3, h2+h3, "", FL_NORMAL_SIZE);
  
  x = x0 + 5;
  y = y + h2 + h3;
  for (i=0; i<10; i++) {
    char b[80];
    sprintf(b, "%d", i+1);
    obj = add_frame_box(x, y, w4, h4, b, FL_LARGE_SIZE);
    obj = add_normal_button(x+w4, y, w5, h4, brd[i].nam, FL_NORMAL_SIZE);
      fl_set_object_callback(obj, board_selector_callback, i);
    y = y + h4;
  }
}

//~----------------------------------------------------------------------------

void board_selector_callback(FL_OBJECT *obj, long data) {

  slot = data;

  write_config_and_refresh_objects(0);
}

//~----------------------------------------------------------------------------

void update_board_selector_panel(void) {

  int far_side;
  char b[80];

  fl_set_object_label(NAM_BOX, brd[slot].nam);

  sprintf(b, "Adr\n0x%02X", brd[slot].adr);
  fl_set_object_label(ADR_BOX, b);

  far_side = (c_side.inp != m_side.inp);
  sprintf(b, "ADC\n0x%02X", brd[slot].adc[far_side]);
  fl_set_object_label(ADC_BOX, b);
}

//~============================================================================
//
//                  INPUT_STAGE_PANEL
//
//~============================================================================

void create_input_stage_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h;
  int w1 = 20, w2 = 20, w3 = 50, w4 = 40;
  int h1 = 20, h2 = 15, h3 = 25;
  char *t1[2] = {"A", "B"};
  char *t2[2] = {"V", "I"};
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + h1 + 2 * h2 + 5 + 2 * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w-10, h1, "Input stages", FL_NORMAL_SIZE);
  y = y + h1;
  for (i=0; i<2; i++) {
    x = x0 + 5;
    add_frame_box(x, y, w1, 2*h2, t1[i], FL_NORMAL_SIZE);
    for (j=0; j<2; j++) {
      x = x0 + 5 + w1;
      add_frame_box(x, y, w2, h2, t2[j], FL_SMALL_SIZE);
      x = x + w2;
      INP_COD_BOX[2*i+j] = 
      obj = add_data_box(x, y, w3, h2, "", FL_NORMAL_SIZE);
      x = x + w3;
      INP_VAL_BOX[2*i+j] = 
      obj = add_data_box(x, y, w4, h2, "", FL_NORMAL_SIZE);
      y = y + h2;
    }
    if (i == 0) y = y + 5;
  }
  inp_cod.rea = malloc(4*4);
  inp_cod.was = malloc(4*4);

  x = x0 + 5;
  obj = add_normal_button(x, y, w1+w2+w3+w4, h3, "READ  INPUT", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, input_stage_callback, i);
}

//~----------------------------------------------------------------------------

void input_stage_callback(FL_OBJECT *obj, long data) {

  int i;
  int far_side;
  int16 dat[32];
  float a_V = (153.0 - 90.0) / 4095.0;
  float b_V =          90.0;
  float a_I = ( 22.0 -  0.0) / 4095.0;
  float b_I =          22.0;
  
  msg = -1;
  update_objects();

  for (i=0; i<4; i++) inp_cod.was[i] = inp_cod.rea[i];

  read_ADC(dat, &err);

  if (!err) {
    far_side = (c_side.inp != m_side.inp);
    inp_cod.rea[0] = dat[0x0E - 1 + far_side * 0x10];
    inp_cod.rea[1] = dat[0x0D - 1 + far_side * 0x10];
    inp_cod.rea[2] = dat[0x1E - 1 - far_side * 0x10];
    inp_cod.rea[3] = dat[0x1D - 1 - far_side * 0x10];
    inp_val[0] = a_V * inp_cod.rea[0] + b_V;
    inp_val[1] = b_I - a_I * inp_cod.rea[1];
    inp_val[2] = a_V * inp_cod.rea[2] + b_V;
    inp_val[3] = b_I - a_I * inp_cod.rea[3];
    for (i=0; i<4; i++) if (inp_cod.unk) inp_cod.was[i] = inp_cod.rea[i];
    inp_cod.unk = FALSE;
    inp_cod.sta = status_OK;
  }
  else {
    inp_cod.sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_input_stage_panel(void) {

  int i;
  char b[80];
  
  show_num_box_string(4, &inp_cod, NULL, "0x%04X", &INP_COD_BOX[0]);

  for (i=0; i<4; i++) {
    if      (inp_val[i] < 10.0)   sprintf(b, "%5.3f", inp_val[i]);
    else if (inp_val[i] < 100.0)  sprintf(b, "%5.2f", inp_val[i]);
    else if (inp_val[i] < 1000.0) sprintf(b, "%5.1f", inp_val[i]);
    else                          sprintf(b, "*****");
    show_num_box_string(1, &inp_cod, NULL, b, &INP_VAL_BOX[i]);
  }
}

//~============================================================================
//
//                  DIGITAL_READOUT_PANEL
//
//~============================================================================

void create_digital_readout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  char b[80];
  int x, y, w, h;
  int w1 = 20, w2 = 135, w3 = 15, w4 = 35, w5 = 20, w6 = 45, w7 = 45, w8 = 135;
  int h1 = 20, h2 = 15, h3 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + NL * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2, h1, "Digital Readout", FL_NORMAL_SIZE);
  x = x + w1 + w2;
  add_frame_box(x, y, w3+w4, h1, "Data", FL_NORMAL_SIZE);
  x = x + w3 + w4;
  add_frame_box(x, y, w5, h1, "", FL_NORMAL_SIZE);

  y = y + h1;
  for (i=0; i<NL; i++) {
    x = x0 + 5;
    sprintf(b, "%02X", i);
    DIG_CHA_BOX[i] =
    obj = add_frame_box(x, y, w1, h2, "", FL_SMALL_SIZE);
      fl_set_object_label(obj, b);
    x = x + w1;
    DIG_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", FL_SMALL_SIZE);
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    DIG_COD_BOX[i] = 
    obj = add_data_box(x, y, w3, h2, "", FL_NORMAL_SIZE);
    x = x + w3;
    DIG_TXT_BOX[i] = 
    obj = add_data_box(x, y, w4, h2, "", FL_NORMAL_SIZE);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "R", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, digital_readout_callback, i);
    y = y + h2;
    dig_cod[i].rea = malloc(4);
    dig_cod[i].was = malloc(4);
  }

  x = x0 + 5;
  add_frame_box(x, y, w6, h3, "Period", FL_NORMAL_SIZE);
  x = x + w6;
  create_btn_hstring(
    1, x, y, w7, h3, periods, 'N', &w_dig_per, &dig_per, &DIG_PER_BTN);
  x = x + w7;
  obj = add_normal_button(x, y, w8, h3, "READ  ALL", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, digital_readout_callback, -1);
}

//~----------------------------------------------------------------------------

void digital_readout_callback(FL_OBJECT *obj, long data) {

  int8 cha = data;
  bool read_all;
  int i;
  int far_side;
  int8 adr, dat;
  int16 cmd;
  
  if (obj && (cha == 0xFF)) {
    if (auto_read_digital) {
      auto_read_digital = FALSE;
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_CYAN, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_read_digital = TRUE;
        fl_add_timeout(0, auto_read_digital_callback, NULL);
        fl_set_object_boxtype(obj, FL_DOWN_BOX);
        fl_set_object_color(obj, FL_RED, FL_RED);
        fl_set_object_lcolor(obj, FL_YELLOW);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_read_digital) {
    msg = -1;
    update_objects();
  }

  if (obj) read_all = (cha == 0xFF);
  else     read_all = TRUE;
  
  far_side = (c_side.inp != m_side.inp);
  err = 0;

  for (i=0; i<NL; i++) {
    if ( read_all && !pdig[i]) continue;
    if (!read_all && cha != i) continue;
    
    *dig_cod[i].was = *dig_cod[i].rea;

    adr = brd[slot].adr;
    cmd = 0;
    pack16(&cmd, 0x0200, far_side);
    pack16(&cmd, 0x01E0, adr);
    pack16(&cmd, 0x001F, i);

    err = 0x0000;
//  if (!err) enable_CGS_serial_lines(&err);
    if (!err) exec_PDS_cmd(cmd, 1, &dat, NULL, &err);
//  if (!err) disable_CGS_serial_lines(&err);

    if (!err) {
      *dig_cod[i].rea = dat;
      if (dig_cod[i].unk) *dig_cod[i].was = *dig_cod[i].rea;
      dig_cod[i].sta = status_OK;
      dig_cod[i].unk = FALSE;
    }
    else {
      dig_cod[i].sta = status_KO;
    }
  }
  
  if (auto_read_digital) msg = -3;
  else                   msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void auto_read_digital_callback(int tid, void *data) {

  int period = 1000.0 * atof(periods[*w_dig_per.inp]);

  if (!auto_read_digital) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback %4d %f\n", period, delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  digital_readout_callback(NULL, 0xFF);
  fl_add_timeout(period, auto_read_digital_callback, NULL);
}

//~----------------------------------------------------------------------------

void update_digital_readout_panel(void) {

  int i, j;
  static int8 old_slot = -1;
  
  if (slot != old_slot) {
    for (i=0; i<NL; i++) pdig[i] = NULL;
    for (j=0; j<NL; j++) {
      i = brd[slot].dig[j].cha;
      if (i == 0xFF) break;
      pdig[i] = &brd[slot].dig[j];
    }
    for (i=0; i<NL; i++) {
      if (pdig[i]) fl_set_object_label(DIG_NAM_BOX[i], pdig[i]->nam);
      else         fl_set_object_label(DIG_NAM_BOX[i], "");
    }
    old_slot   = slot;
  }

  for (i=0; i<NL; i++) {
    char **p;
    show_num_box_string(1, &dig_cod[i], NULL, "%d", &DIG_COD_BOX[i]);
    if (pdig[i]) p = pdig[i]->txt;
    else         p = dash;
    show_num_box_string(1, &dig_cod[i], p, NULL, &DIG_TXT_BOX[i]);
  }
}

//~============================================================================
//
//                  ANALOG_READOUT_PANEL
//
//~============================================================================

void create_analog_readout_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  char b[80];
  int x, y, w, h;
  int w1 = 20, w2 = 135, w3 = 50, w4 = 40, w5 = 20, w6 = 45, w7 = 45, w8 = 175;
  int h1 = 20, h2 = 15, h3 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + NL * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2, h1, "Analog Readout", FL_NORMAL_SIZE);
  x = x + w1 + w2;
  add_frame_box(x, y, w3, h1, "Code", FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "Value", FL_NORMAL_SIZE);
  x = x + w4;
  add_frame_box(x, y, w5, h1, "", FL_NORMAL_SIZE);

  y = y + h1;
  for (i=0; i<NL; i++) {
    x = x0 + 5;
    sprintf(b, "%02X", i);
    ANA_CHA_BOX[i] =
    obj = add_frame_box(x, y, w1, h2, "", FL_SMALL_SIZE);
      fl_set_object_label(obj, b);
    x = x + w1;
    ANA_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", FL_SMALL_SIZE);
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    ANA_COD_BOX[i] = 
    obj = add_data_box(x, y, w3, h2, "", FL_NORMAL_SIZE);
    x = x + w3;
    ANA_VAL_BOX[i] = 
    obj = add_data_box(x, y, w4, h2, "", FL_NORMAL_SIZE);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "R", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, analog_readout_callback, i);
    y = y + h2;
    ana_cod[i].rea = malloc(4);
    ana_cod[i].was = malloc(4);
  }

  x = x0 + 5;
  add_frame_box(x, y, w6, h3, "Period", FL_NORMAL_SIZE);
  x = x + w6;
  create_btn_hstring(
    1, x, y, w7, h3, periods, 'N', &w_ana_per, &ana_per, &ANA_PER_BTN);
  x = x + w7;
  obj = add_normal_button(x, y, w8, h3, "READ  ALL", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, analog_readout_callback, -1);
}

//~----------------------------------------------------------------------------

void analog_readout_callback(FL_OBJECT *obj, long data) {

  int8 cha = data;
  bool read_all;
  int i;
  int far_side;
  int8 adr, adc;
  int16 dat;
  int16 cmd;
  
  if (obj && (cha == 0xFF)) {
    if (auto_read_analog) {
      auto_read_analog = FALSE;
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_CYAN, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
      msg = 0;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        auto_read_analog = TRUE;
        fl_add_timeout(0, auto_read_analog_callback, NULL);
        fl_set_object_boxtype(obj, FL_DOWN_BOX);
        fl_set_object_color(obj, FL_RED, FL_RED);
        fl_set_object_lcolor(obj, FL_YELLOW);
        msg = 0;
        update_objects();
      }
    }
  }

  if (!auto_read_analog) {
    msg = -1;
    update_objects();
  }

  if (obj) read_all = (cha == 0xFF);
  else     read_all = TRUE;

  far_side = (c_side.inp != m_side.inp);
  err = 0;

  for (i=0; i<NL; i++) {
    if ( read_all && !pana[i]) continue;
    if (!read_all && cha != i) continue;

    *ana_cod[i].was = *ana_cod[i].rea;

    adr = brd[slot].adr;
    adc = brd[slot].adc[far_side];
    cmd = 0;
    pack16(&cmd, 0x0200, far_side);
    pack16(&cmd, 0x01E0, adr);
    pack16(&cmd, 0x001F, i);

    err = 0x0000;
//  if (!err) enable_CGS_serial_lines(&err);
    if (!err) exec_PDS_cmd(cmd, adc, NULL, &dat, &err);
//  if (!err) disable_CGS_serial_lines(&err);
    
    if (!err) {
      *ana_cod[i].rea = dat;
//printf("i=%02X   A=%f   B=%f  \n", i, A[i], B[i]);
      ana_val[i] = A[i] * (float)(*ana_cod[i].rea) + B[i];
      if (ana_cod[i].unk) *ana_cod[i].was = *ana_cod[i].rea;
      ana_cod[i].sta = status_OK;
      ana_cod[i].unk = FALSE;
    }
    else {
      ana_cod[i].sta = status_KO;
    }
  }
  
  if (auto_read_analog) msg = -3;
  else                  msg = 1;

  update_objects();
}

//~----------------------------------------------------------------------------

void auto_read_analog_callback(int tid, void *data) {

  int period = 1000.0 * atof(periods[*w_ana_per.inp]);

  if (!auto_read_analog) return;
  
  if (P > 1) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("Timeout callback %4d %f\n", period, delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  analog_readout_callback(NULL, 0xFF);
  fl_add_timeout(period, auto_read_analog_callback, NULL);
}

//~----------------------------------------------------------------------------

void update_analog_readout_panel(void) {

  int i, j;
  static int8 old_slot = -1;
  char b[80];
  
  if (slot != old_slot) {
    for (i=0; i<NL; i++) pana[i] = NULL;
    for (j=0; j<NL; j++) {
      i = brd[slot].ana[j].cha;
      if (i == 0xFF) break;
      pana[i] = &brd[slot].ana[j];
    }
    for (i=0; i<NL; i++) {
      if (pana[i]) {
        A[i] = (pana[i]->max - pana[i]->min) / 4095.0;
        B[i] =                 pana[i]->min;
//printf("i = %d, A[i] = %f, B[i] = %f\n", i, A[i], B[i]);
        fl_set_object_label(ANA_NAM_BOX[i], pana[i]->nam);
      }
      else {
        A[i] = 0.0;
        B[i] = 0.0;
        fl_set_object_label(ANA_NAM_BOX[i], "");
      }
    }
    old_slot = slot;
  }

  for (i=0; i<NL; i++) {
    if      (ana_val[i] < 10.0)   sprintf(b, "%5.3f", ana_val[i]);
    else if (ana_val[i] < 100.0)  sprintf(b, "%5.2f", ana_val[i]);
    else if (ana_val[i] < 1000.0) sprintf(b, "%5.1f", ana_val[i]);
    else                          sprintf(b, "*****");
    show_num_box_string(1, &ana_cod[i], NULL, "0x%04X", &ANA_COD_BOX[i]);
    show_num_box_string(1, &ana_cod[i], NULL,        b, &ANA_VAL_BOX[i]);
  }
}

//~============================================================================
//
//                  DIGITAL_COMMANDING_PANEL
//
//~============================================================================

void create_digital_commanding_panel(int x0, int y0) {

  SW_BTN *B;
  FL_OBJECT *obj;
  int i;
  char b[80];
  int x, y, w, h;
  int w1 = 20, w2 = 105,  w3 = 15, w4 = 30, w5 = 20;
  int h1 = 20, h2 = 15, h3 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + NL * h2 + h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2, h1, "Commanding", FL_NORMAL_SIZE);
  x = x + w1 + w2;
  add_frame_box(x, y, w3+w4, h1, "Data", FL_NORMAL_SIZE);
  x = x + w3 + w4;
  add_frame_box(x, y, w5, h1, "", FL_NORMAL_SIZE);

  y = y + h1;
  for (i=0; i<NL; i++) {
    x = x0 + 5;
    sprintf(b, "%02X", i);
    CMD_CHA_BOX[i] =
    obj = add_frame_box(x, y, w1, h2, "", FL_SMALL_SIZE);
      fl_set_object_label(obj, b);
    x = x + w1;
    CMD_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", FL_SMALL_SIZE);
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    CMD_DAT_BTN[i] =
      B = add_switch_btn(x, y, w3, h2, bin, FL_NORMAL_SIZE, 2, -1);
      B->m = 0x00000001;
      B->p = &cmd_dat[i].inp;
      B->i = unpack32(*B->p, B->m);
      fl_set_object_label(B->obj, B->l[B->i]);
      B->call = SW_BTN_CALLBACK;
    x = x + w3;
    CMD_TXT_BTN[i] =
      B = add_switch_btn(x, y, w4, h2, bin, FL_NORMAL_SIZE, 2, -1);
      B->m = 0x00000001;
      B->p = &cmd_dat[i].inp;
      B->i = unpack32(*B->p, B->m);
      fl_set_object_label(B->obj, B->l[B->i]);
      B->call = SW_BTN_CALLBACK;
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "W", FL_NORMAL_SIZE);
      fl_set_object_callback(obj, digital_commanding_callback, i);
    y = y + h2;
  }

  x = x0 + 5;
  obj = add_normal_button(x, y, w1+w2+w3+w4+w5, h3, "WRITE  ALL", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, digital_commanding_callback, -1);
}

//~----------------------------------------------------------------------------

void digital_commanding_callback(FL_OBJECT *obj, long data) {

  int8 cha = data;
  bool write_all;
  int i;
  int far_side;
  int8 adr, dat;
  int16 cmd;

  msg = -1;
  update_objects();

  far_side = (c_side.inp != m_side.inp);
  write_all = (cha == 0xFF);
  err = 0;
  
  for (i=0; i<NL; i++) {
    if ( write_all && !pcmd[i]) continue;
    if (!write_all && cha != i) continue;

    adr = brd[slot].adr;
    dat = cmd_dat[i].inp;
    cmd = 0;
    pack16(&cmd, 0x0800, dat);
    pack16(&cmd, 0x0400, 1);
    pack16(&cmd, 0x0200, far_side);
    pack16(&cmd, 0x01E0, adr);
    pack16(&cmd, 0x001F, i);
  
    err = 0x0000;
//  if (!err) enable_CGS_serial_lines(&err);
    if (!err) exec_PDS_cmd(cmd, 1, NULL, NULL, &err);
//  if (!err) disable_CGS_serial_lines(&err);
    
    if (!err) {
      cmd_dat[i].wri = cmd_dat[i].inp;
      cmd_dat[i].unk = FALSE;
    }
    else {
      cmd_dat[i].unk = TRUE;
    }
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_digital_commanding_panel(void) {

  SW_BTN *B;
  int i, j;
  static int8 old_slot = -1;
  
  if (slot != old_slot) {
    for (i=0; i<NL; i++) pcmd[i] = NULL;
    for (j=0; j<NL; j++) {
      i = brd[slot].cmd[j].cha;
      if (i == 0xFF) break;
      pcmd[i] = &brd[slot].cmd[j];
    }
    for (i=0; i<NL; i++) {
      if (pcmd[i]) fl_set_object_label(CMD_NAM_BOX[i], pcmd[i]->nam);
      else         fl_set_object_label(CMD_NAM_BOX[i], "");
    }
    old_slot = slot;
  }

  for (i=0; i<NL; i++) {
    show_bin_btn_string(1, &cmd_dat[i], &CMD_DAT_BTN[i]);
    B = CMD_TXT_BTN[i];
    if (pcmd[i]) B->l = pcmd[i]->txt;
    else         B->l = dash;
    show_bin_btn_string(1, &cmd_dat[i], &B);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);
  update_can_bus_i_f_selector_panel();
  update_board_selector_panel();
  update_input_stage_panel();
  update_digital_readout_panel();
  update_analog_readout_panel();
  update_digital_commanding_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
