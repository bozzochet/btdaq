// file PDS-3.c
//
// PDS Pro Controller
//
// A.Lebedev May-2005...

#include "template.h"
#include "pdsdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"PDS-3 Pro Controller"};               // global variable
char *date  = {"12-Aug-07"};                          // global variable

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

#define NC 13
#define NA 10
#define ND 28

//~--

FL_OBJECT *SIDE_BOX, *SLOT_BOX, *ADR_BOX, *NAM_BOX;
int8 side, slot;

FL_OBJECT *CMD_CHA_BOX[ND], *CMD_NAM_BOX[ND];
SW_BTN *CMD_DAT_BTN[NC];
_w cmd_dat[NC];

FL_OBJECT *ADC_CHA_BOX[ND], *ADC_NAM_BOX[ND], *ADC_COD_BOX[NA], *ADC_VAL_BOX[NA];
_r adc_cod[NA];
float adc_val[NA];
float ref_val = 4.096;

FL_OBJECT *DIG_CHA_BOX[ND], *DIG_NAM_BOX[ND], *DIG_DAT_BOX[ND];
_r dig_dat[ND];

//~--

char *bin[2] = {"0", "1"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_digital_readout_panel(int x0, int y0);
void digital_readout_callback(FL_OBJECT *obj, long data);
void update_digital_readout_panel(void);

void create_digital_commanding_panel(int x0, int y0);
void digital_commanding_callback(FL_OBJECT *obj, long data);
void update_digital_commanding_panel(void);

void create_analog_readout_panel(int x0, int y0);
void analog_readout_callback(FL_OBJECT *obj, long data);
void update_analog_readout_panel(void);

void create_board_selector_panel(int x0, int y0);
void board_selector_callback(FL_OBJECT *obj, long data);
void update_board_selector_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 520, HH = 700;
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_board_selector_panel(       0,  45);
  create_digital_commanding_panel( 110,  45);
  create_analog_readout_panel(       0, 340);
  create_digital_readout_panel(    315,  45);
  
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
  
  side = 0;
  slot = 0;
  
  for (i=0; i<NC; i++) cmd_dat[i].inp = 0;
  for (i=0; i<NA; i++) adc_cod[i].unk = TRUE;
  for (i=0; i<NC; i++) cmd_dat[i].unk = TRUE;
  for (i=0; i<ND; i++) dig_dat[i].unk = TRUE;

  read_dat_config_file(dat_config_file_name);

  if (slot > 9) slot = 0;
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    fread(&side, sizeof(side), 1, file);
    fread(&slot, sizeof(slot), 1, file);
    for (i=0; i<NC; i++) fread(&cmd_dat[i].inp, sizeof(cmd_dat[i].inp), 1, file);
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
    fwrite(&side, sizeof(side), 1, file);
    fwrite(&slot, sizeof(slot), 1, file);
    for (i=0; i<NC; i++) fwrite(&cmd_dat[i].inp, sizeof(cmd_dat[i].inp), 1, file);
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
//                  BOARD_SELECTOR_PANEL
//
//~============================================================================

void create_board_selector_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 35, w2 = 30, w3 = 35;
  int h1 = 55, h2 = 30, h3 = 20;
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + h2 + 10 * h3 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  NAM_BOX =
  obj = add_frame_box(x, y, w1+w2+w3, h1, "", FL_MEDIUM_SIZE);

  x = x0 + 5;
  y = y + h1;
  SIDE_BOX =
  obj = add_frame_box(x, y, w1, h2, "", FL_NORMAL_SIZE);
  x = x + w1;
  SLOT_BOX =
  obj = add_frame_box(x, y, w2, h2, "", FL_NORMAL_SIZE);
  x = x + w2;
  ADR_BOX =
  obj = add_frame_box(x, y, w3, h2, "", FL_NORMAL_SIZE);
  
  x = x0 + 5;
  y = y + h2;
  for (i=0; i<10; i++) {
    char b[80];
    char *p;
    strcpy(b, brd[i].nam);
    p = strstr(b, "\n");
    if (p) *p = 0;
    obj = add_normal_button(x, y, w1+w2+w3, h3, b, FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, board_selector_callback, i);
    y = y + h3;
  }
}

//~----------------------------------------------------------------------------

void board_selector_callback(FL_OBJECT *obj, long data) {

  slot = data;

  write_dat_config_file(dat_config_file_name);
  
  refresh_objects();
}

//~----------------------------------------------------------------------------

void update_board_selector_panel(void) {

  char b[80];

  sprintf(b, "Side\n%c", side ? 'B' : 'A');
  fl_set_object_label(SIDE_BOX, b);

  sprintf(b, "Slot\n%d", slot+1);
  fl_set_object_label(SLOT_BOX, b);

  sprintf(b, "Adr\n0x%X", brd[slot].adr);
  fl_set_object_label(ADR_BOX, b);

  fl_set_object_label(NAM_BOX, brd[slot].nam);
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
  int x, y, w, h;
  int w1 = 25, w2 = 120, w3 = 20, w4 = 30;
  int h1 = 25, h2 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + + 5;
  h = 5 + h1 + NC * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2, h1, "Digital Commanding", FL_NORMAL_SIZE);
  x = x + w1 + w2;
  add_frame_box(x, y, w3, h1, "D", FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "Cmd", FL_NORMAL_SIZE);

  y = y + h1;
  for (i=0; i<NC; i++) {
    x = x0 + 5;
    CMD_CHA_BOX[i] =
    obj = add_frame_box(x, y, w1, h2, "", FL_NORMAL_SIZE);
    x = x + w1;
    CMD_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", FL_NORMAL_SIZE);
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    CMD_DAT_BTN[i] =
      B = add_switch_btn(x, y, w3, h2, bin, FL_MEDIUM_SIZE, 2, -1);
      B->m = 0x00000001;
      B->p = &cmd_dat[i].inp;
      B->i = unpack32(*B->p, B->m);
      fl_set_object_label(B->obj, B->l[B->i]);
      B->call = SW_BTN_CALLBACK;
    x = x + w3;
    obj = add_normal_button(x, y, w4, h2, "W", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, digital_commanding_callback, i);
    y = y + h2;
  }
}

//~----------------------------------------------------------------------------

void digital_commanding_callback(FL_OBJECT *obj, long data) {

  int8 ind = data;
  int8 adr, cha, dat;
  int16 cmd;

  msg = -1;
  update_objects();

  adr = brd[slot].adr;
  cha = brd[slot].cmd[ind].cha;
  dat = cmd_dat[ind].inp;
  cmd = 0;
  pack16(&cmd, 0x0800, dat);
  pack16(&cmd, 0x0400, 1);
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);
  
  exec_PDS_cmd(cmd, 1, NULL, NULL, &err);

  if (!err) {
    cmd_dat[ind].wri = cmd_dat[ind].inp;
    cmd_dat[ind].unk = FALSE;
  }
  else {
    cmd_dat[ind].unk = TRUE;
  }

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_digital_commanding_panel(void) {

  int i;
  static int8 old_slot = -1;
  char b[80];
  bool clear;
  
  if (slot != old_slot) {
    clear = FALSE;
    for (i=0; i<NC; i++) {
      if (brd[slot].cmd[i].cha == 0xFF) clear = TRUE;
      if (clear) {
        fl_set_object_label(CMD_CHA_BOX[i], "");
        fl_set_object_label(CMD_NAM_BOX[i], "");
      }
      else {
        sprintf(b, "%02X", brd[slot].cmd[i].cha);
        fl_set_object_label(CMD_CHA_BOX[i], b);
        fl_set_object_label(CMD_NAM_BOX[i], brd[slot].cmd[i].nam);
      }
    }
    old_slot = slot;
  }

  clear = FALSE;
  for (i=0; i<NC; i++) {
    if (brd[slot].cmd[i].cha == 0xFF) clear = TRUE;
    if (clear) cmd_dat[i].unk = TRUE;
    show_bin_btn_string(1, &cmd_dat[i], &CMD_DAT_BTN[i]);
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
  int x, y, w, h;
  int w1 = 25, w2 = 120, w3 = 65, w4 = 65, w5 = 30;
  int h1 = 25, h2 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + NA * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2, h1, "Analog Readout", FL_NORMAL_SIZE);
  x = x + w1 + w2;
  add_frame_box(x, y, w3, h1, "Code", FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "Value", FL_NORMAL_SIZE);
  x = x + w4;
  add_frame_box(x, y, w5, h1, "Cmd", FL_NORMAL_SIZE);

  y = y + h1;
  for (i=0; i<NA; i++) {
    x = x0 + 5;
    ADC_CHA_BOX[i] =
    obj = add_frame_box(x, y, w1, h2, "", FL_NORMAL_SIZE);
    x = x + w1;
    ADC_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", FL_NORMAL_SIZE);
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    ADC_COD_BOX[i] = 
    obj = add_data_box(x, y, w3, h2, "", FL_MEDIUM_SIZE);
    x = x + w3;
    ADC_VAL_BOX[i] = 
    obj = add_data_box(x, y, w4, h2, "", FL_MEDIUM_SIZE);
    x = x + w4;
    obj = add_normal_button(x, y, w5, h2, "R", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, analog_readout_callback, i);
    y = y + h2;
    adc_cod[i].rea = malloc(4);
    adc_cod[i].was = malloc(4);
  }
}

//~----------------------------------------------------------------------------

void analog_readout_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int8 adr, cha, adc;
  int16 dat;
  int16 cmd;
  
  msg = -1;
  update_objects();

  *adc_cod[ind].was = *adc_cod[ind].rea;

  adr = brd[slot].adr;
  cha = brd[slot].ana[ind].cha;
  adc = brd[slot].adc[0];
  cmd = 0;
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);
  
  exec_PDS_cmd(cmd, adc, NULL, &dat, &err);

  if (!err) {
    *adc_cod[ind].rea = dat;
    adc_val[ind] = (float)*adc_cod[ind].rea / (float)0x0FFF * ref_val;
//  if (adc_cod[ind].unk) *adc_cod[ind].was = *adc_cod[ind].rea;
    *adc_cod[ind].was = *adc_cod[ind].rea;
    adc_cod[ind].sta = status_OK;
    adc_cod[ind].unk = FALSE;
  }
  else {
    adc_cod[ind].sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_analog_readout_panel(void) {

  int i;
  static int8 old_slot = -1;
  char b[80];
  bool clear;
  
  if (slot != old_slot) {
    clear = FALSE;
    for (i=0; i<NA; i++) {
      if (brd[slot].ana[i].cha == 0xFF) clear = TRUE;
      if (clear) {
        fl_set_object_label(ADC_CHA_BOX[i], "");
        fl_set_object_label(ADC_NAM_BOX[i], "");
      }
      else {
        sprintf(b, "%02X", brd[slot].ana[i].cha);
        fl_set_object_label(ADC_CHA_BOX[i], b);
        fl_set_object_label(ADC_NAM_BOX[i], brd[slot].ana[i].nam);
      }
    }
    old_slot = slot;
  }

  clear = FALSE;
  for (i=0; i<NA; i++) {
    if (brd[slot].ana[i].cha == 0xFF) clear = TRUE;
    if (clear) adc_cod[i].unk = TRUE;
    sprintf(b, "%5.3f", adc_val[i]);
    show_num_box_string(1, &adc_cod[i], NULL, "0x%04X", &ADC_COD_BOX[i]);
    show_num_box_string(1, &adc_cod[i], NULL,        b, &ADC_VAL_BOX[i]);
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
  int x, y, w, h;
  int w1 = 25, w2 = 120, w3 = 20, w4 = 30;
  int h1 = 25, h2 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + h1 + ND * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2, h1, "Digital Readout", FL_NORMAL_SIZE);
  x = x + w1 + w2;
  add_frame_box(x, y, w3, h1, "D", FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "Cmd", FL_NORMAL_SIZE);

  y = y + h1;
  for (i=0; i<ND; i++) {
    x = x0 + 5;
    DIG_CHA_BOX[i] =
    obj = add_frame_box(x, y, w1, h2, "", FL_NORMAL_SIZE);
    x = x + w1;
    DIG_NAM_BOX[i] =
    obj = add_frame_box(x, y, w2, h2, "", FL_NORMAL_SIZE);
      fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    x = x + w2;
    DIG_DAT_BOX[i] = 
    obj = add_data_box(x, y, w3, h2, "", FL_MEDIUM_SIZE);
    x = x + w3;
    obj = add_normal_button(x, y, w4, h2, "R", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, digital_readout_callback, i);
    y = y + h2;
    dig_dat[i].rea = malloc(4);
    dig_dat[i].was = malloc(4);
  }
}

//~----------------------------------------------------------------------------

void digital_readout_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int8 adr, cha, dat;
  int16 cmd;
  
  msg = -1;
  update_objects();

  *dig_dat[ind].was = *dig_dat[ind].rea;

  adr = brd[slot].adr;
  cha = brd[slot].dig[ind].cha;
  cmd = 0;
  pack16(&cmd, 0x01E0, adr);
  pack16(&cmd, 0x001F, cha);
  
  exec_PDS_cmd(cmd, 1, &dat, NULL, &err);

  if (!err) {
    *dig_dat[ind].rea = dat;
    if (dig_dat[ind].unk) *dig_dat[ind].was = *dig_dat[ind].rea;
    dig_dat[ind].sta = status_OK;
    dig_dat[ind].unk = FALSE;
  }
  else {
    dig_dat[ind].sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_digital_readout_panel(void) {

  int i;
  static int8 old_slot = -1;
  char b[80];
  bool clear;
  
  if (slot != old_slot) {
    clear = FALSE;
    for (i=0; i<ND; i++) {
      if (brd[slot].dig[i].cha == 0xFF) clear = TRUE;
      if (clear) {
        fl_set_object_label(DIG_CHA_BOX[i], "");
        fl_set_object_label(DIG_NAM_BOX[i], "");
      }
      else {
        sprintf(b, "%02X", brd[slot].dig[i].cha);
        fl_set_object_label(DIG_CHA_BOX[i], b);
        fl_set_object_label(DIG_NAM_BOX[i], brd[slot].dig[i].nam);
      }
    }
    old_slot = slot;
  }

  clear = FALSE;
  for (i=0; i<ND; i++) {
    if (brd[slot].dig[i].cha == 0xFF) clear = TRUE;
    if (clear) dig_dat[i].unk = TRUE;
    show_num_box_string(1, &dig_dat[i], NULL, "%d", &DIG_DAT_BOX[i]);
  }
}

//~===============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);
  update_board_selector_panel();
  update_digital_commanding_panel();
  update_analog_readout_panel();
  update_digital_readout_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
