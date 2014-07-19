// file PDS-2.c
//
// PDS Pro Controller
//
// A.Lebedev May-2005...

#include "template.h"
#include "pdsdef.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"PDS-2 Pro Controller"};               // global variable
char *date  = {"07-Nov-07"};                          // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x1FF;                                  // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32 wri_adr[2] = {0x07F004, 0x07F005};
int32 rea_adr    = {0x07F005};

//~--

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~--

#define NL 28

//~--

SW_BTN *WRI_SIDE_BTN[NL], *WRI_ADR_BTN[NL], *WRI_CHA_BTN[NL][2], *WRI_DIG_BTN[NL];
_w digital[NL];

SW_BTN *REA_SIDE_BTN[NL], *REA_ADR_BTN[NL], *REA_CHA_BTN[NL][2];
FL_OBJECT *REA_DIG_BOX[NL];
_w dig_adr[NL];
_r dig_dat[NL];

SW_BTN *ADC_CHA_BTN[NL][2];
FL_OBJECT *ADC_COD_BOX[NL], *ADC_VAL_BOX[NL];
_w adc_cha[NL];
_r adc_cod[NL];
float adc_val[NL];

FL_OBJECT *REF_VAL_BTN, *REF_VAL_INP;
float ref_val;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *a_b[2] = {"A", "B"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_board_selector_panel(int x0, int y0);
void board_selector_callback(FL_OBJECT *obj, long data);

void create_reference_panel(int x0, int y0);
void ref_inp_focus_callback(FL_OBJECT *obj, long data);
void ref_inp_callback(FL_OBJECT *obj, long data);
void update_reference_panel(void);

void create_read_pds_channels_panel(int x0, int y0);
void read_pds_channels_callback(FL_OBJECT *obj, long data);
void update_read_pds_channels_panel(void);

void create_write_pds_channels_panel(int x0, int y0);
void write_pds_channels_callback(FL_OBJECT *obj, long data);
void update_write_pds_channels_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  int WW = 540, HH = 175 + 20 * NL + 5 * (NL / 4);
  
  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_board_selector_panel(      0, 45);
  create_reference_panel(         470, 45);
  create_read_pds_channels_panel(   0, 95);
  create_write_pds_channels_panel(350, 95);
  
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
  
  for (i=0; i<NL; i++) {
    digital[i].inp = 0x0000;
    digital[i].unk = TRUE;
    dig_adr[i].inp = 0x0000;
    dig_adr[i].unk = TRUE;
    dig_dat[i].unk = TRUE;
    adc_cha[i].inp = 1;
    adc_cha[i].unk = TRUE;
    adc_cod[i].unk = TRUE;
  }

  ref_val = 4.095;
  
  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int i;
  
  file = fopen(filename, "r");
  if (file) {
    for (i=0; i<NL; i++) fread(&digital[i].inp, sizeof(digital[i].inp), 1, file);
    for (i=0; i<NL; i++) fread(&dig_adr[i].inp, sizeof(dig_adr[i].inp), 1, file);
    for (i=0; i<NL; i++) fread(&adc_cha[i].inp, sizeof(adc_cha[i].inp), 1, file);
    fread(&ref_val, sizeof(ref_val), 1, file);
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
    for (i=0; i<NL; i++) fwrite(&digital[i].inp, sizeof(digital[i].inp), 1, file);
    for (i=0; i<NL; i++) fwrite(&dig_adr[i].inp, sizeof(dig_adr[i].inp), 1, file);
    for (i=0; i<NL; i++) fwrite(&adc_cha[i].inp, sizeof(adc_cha[i].inp), 1, file);
    fwrite(&ref_val, sizeof(ref_val), 1, file);
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
  int wa = 65, w1 = 95, w2 = 55;
  int h1 = 20;
  
  w = 5 + wa + 3 * w1 + 2 * w2 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, wa, 2*h1, "Board\nSelector", FL_NORMAL_SIZE);

  x = x0 + 5 + wa;
  for (i=0; i<5; i++) {
    char b[80];
    char *p;
    strcpy(b, brd[i].nam);
    p = strstr(b, "\n");
    if (p) *p = 0;
    w = i % 2 ? w2 : w1;
    obj = add_normal_button(x, y, w, h1, b, FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, board_selector_callback, i);
    x = x + w;
  }
  x = x0 + 5 + wa;
  y = y + h1;
  for (i=5; i<10; i++) {
    char b[80];
    char *p;
    strcpy(b, brd[i].nam);
    p = strstr(b, "\n");
    if (p) *p = 0;
    w = i % 2 ? w2 : w1;
    if (i == 9) w = w1;
    obj = add_normal_button(x, y, w, h1, b, FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, board_selector_callback, i);
    x = x + w;
  }
}

//~----------------------------------------------------------------------------

void board_selector_callback(FL_OBJECT *obj, long data) {

  int slot = data;
  int i;
  bool clear;

  clear = FALSE;
  for (i=0; i<NL; i++) {
    if (brd[slot].dig[i].cha == 0xFF) clear = TRUE;
    if (clear) {
      dig_adr[i].inp = 0;
      adc_cha[i].inp = 0;
    }
    else {
      dig_adr[i].inp = (int16)brd[slot].dig[i].cha | ((int16)brd[slot].adr) << 5;
      adc_cha[i].inp = brd[slot].adc[0];
    }
  }

  clear = FALSE;
  for (i=0; i<NL; i++) {
    if (brd[slot].cmd[i].cha == 0xFF) clear = TRUE;
    if (clear) {
      digital[i].inp = 0;
    }
    else {
      digital[i].inp = (int16)brd[slot].cmd[i].cha | ((int16)brd[slot].adr) << 5 | 0x0400;
    }
  }

  write_dat_config_file(dat_config_file_name);
  
  refresh_objects();
}

//~============================================================================
//
//                  REFERENCE_PANEL
//
//~============================================================================

void create_reference_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 0, w2 = 60;
  int h1 = 20, h2 = 20;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w2, h1, "Ref", FL_NORMAL_SIZE);
  x = x + w2;
  y = y + h1;
  
  x = x0 + 5;
  
  REF_VAL_BTN =
  obj = add_switch_button(x, y, w2, h2, "", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, ref_inp_focus_callback, 0);
  REF_VAL_INP =
  obj = fl_add_input(FL_FLOAT_INPUT, x-5, y-5, w2+10, h2+10,"");
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_WHEAT, FL_WHEAT);
    fl_set_object_lsize(obj, FL_LARGE_SIZE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE + FL_ENGRAVED_STYLE);
    fl_set_input_color(obj, FL_BLACK, FL_BLACK);
    fl_set_input_return(obj, FL_RETURN_END);
    fl_set_object_callback(obj, ref_inp_callback, 0);
    fl_deactivate_object(obj);
    fl_hide_object(obj);
  x = x + w2;
}

//~----------------------------------------------------------------------------

void ref_inp_focus_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *obj1 = REF_VAL_INP;
  FL_OBJECT *obj2 = REF_VAL_BTN;
  char b[80];
  int mode = -1;

  if (fl_get_button_numb(obj2) == FL_LEFT_MOUSE)  mode = 0;
  if (fl_get_button_numb(obj2) == FL_RIGHT_MOUSE) mode = 1;

  fl_activate_object(obj1);
  fl_show_object(obj1);
  if (mode) sprintf(b, "%5.3f", ref_val);
  else      sprintf(b, " ");
  fl_set_input(obj1, b);
  
  fl_deactivate_object(obj2);
  fl_hide_object(obj2);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void ref_inp_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *obj1 = REF_VAL_INP;
  FL_OBJECT *obj2 = REF_VAL_BTN;

  ref_val = atof(fl_get_input(obj1));
  
  fl_deactivate_object(obj1);
  fl_hide_object(obj1);

  fl_activate_object(obj2);
  fl_show_object(obj2);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_reference_panel(void) {

  FL_OBJECT *obj;
  char b[80];

  obj = REF_VAL_BTN;
  sprintf(b, "%5.3f", ref_val);
  fl_set_object_label(obj, b);
}

//~============================================================================
//
//                  READ_PDS_CHANNEL
//
//~============================================================================

void create_read_pds_channels_panel(int x0, int y0) {

  SW_BTN *B;
  SW_BTN *BTN[100];
  FL_OBJECT *obj;
  int j, i, n;
  int x, y, w, h;
  int w1 = 30, w2 = 30, w3 = 40, w4 = 20, w5 = 40, w6 = 60, w7 = 60, w8 = 60;
  int h1 = 40, h2 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + w6 + w7 + w8 + 5;
  h = 5 + h1 + NL * h2 + (NL / 4) * 5 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "Side", FL_NORMAL_SIZE);
  x = x + w1;
  add_frame_box(x, y, w2, h1, "Adr", FL_NORMAL_SIZE);
  x = x + w2;
  add_frame_box(x, y, w3, h1, "Chan", FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "D", FL_NORMAL_SIZE);
  x = x + w4;
  add_frame_box(x, y, w5+w6+w7, h2, "Analog", FL_NORMAL_SIZE);
  add_frame_box(x, y+h2, w5, h2, "ADC", FL_NORMAL_SIZE);
  x = x + w5;
  add_frame_box(x, y+h2, w6, h2, "Code", FL_NORMAL_SIZE);
  x = x + w6;
  add_frame_box(x, y+h2, w7, h2, "Value", FL_NORMAL_SIZE);
  x = x + w7;
  add_frame_box(x, y, w8, h1, "Cmd", FL_NORMAL_SIZE);
  x = x + w8;
  y = y + h1;

  for (j=0; j<NL; j++) {
    n = 0;
    x = x0 + 5;
    BTN[n++] =
    REA_SIDE_BTN[j] =
      B = add_switch_btn(x, y, w1, h2, a_b, FL_MEDIUM_SIZE, 2, -1);
      B->m = 0x0200;
    x = x + w1;
    BTN[n++] =
    REA_ADR_BTN[j] =
      B = add_switch_btn(x, y, w2, h2, hex, FL_MEDIUM_SIZE, 16, -1);
      B->m = 0x01E0;
    x = x + w2;
    BTN[n++] =
    REA_CHA_BTN[j][0] =
      B = add_switch_btn(x, y, w3/2, h2, hex, FL_MEDIUM_SIZE, 2, -1);
      B->m = 0x0010;
    x = x + w3 / 2;
    BTN[n++] =
    REA_CHA_BTN[j][1] =
      B = add_switch_btn(x, y, w3/2, h2, hex, FL_MEDIUM_SIZE, 16, -1);
      B->m = 0x000F;
    x = x + w3 / 2;
    for (i=0; i<n; i++) {
      B = BTN[i];
      B->p = &dig_adr[j].inp;
      B->i = unpack32(*B->p, B->m);
      fl_set_object_label(B->obj, B->l[B->i]);
      B->call = SW_BTN_CALLBACK;
    }
    pack32((int32*)&dig_adr[j].inp, 0x0400, 0);
    REA_DIG_BOX[j] = add_data_box(x, y, w4, h2, "", FL_MEDIUM_SIZE);
    x = x + w4;
    dig_dat[j].rea = malloc(4);
    dig_dat[j].was = malloc(4);
    create_hex_btn_hstring(
      2, x, y, w5/2, h2, hex, FL_MEDIUM_SIZE, &adc_cha[j], &ADC_CHA_BTN[j][0]);
    x = x + w5;
    ADC_COD_BOX[j] = add_data_box(x, y, w6, h2, "", FL_MEDIUM_SIZE);
    x = x + w6;
    ADC_VAL_BOX[j] = add_data_box(x, y, w7, h2, "", FL_MEDIUM_SIZE);
    x = x + w7;
    adc_cod[j].rea = malloc(4);
    adc_cod[j].was = malloc(4);
    obj = add_normal_button(x, y, w8, h2, "READ", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, read_pds_channels_callback, j);
    y = y + h2;
    if (!((j + 1) % 4)) y = y + 5;
  }
}

//~----------------------------------------------------------------------------

void read_pds_channels_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _w *cmd = &dig_adr[ind];
  _r *dig = &dig_dat[ind];
  _w *a_c = &adc_cha[ind];
  _r *a_d = &adc_cod[ind];
  
  msg = -1;
  update_objects();

  *dig->was = *dig->rea;
  *a_d->was = *a_d->rea;

  exec_PDS_cmd(cmd->inp, a_c->inp, (int8*)dig->rea, (int16*)a_d->rea, &err);

  if (!err) {
    if (dig->unk) *dig->was = *dig->rea;
    if (a_d->unk) *a_d->was = *a_d->rea;
    cmd->wri = cmd->inp;
    cmd->unk = FALSE;
    dig->sta = status_OK;
    dig->unk = FALSE;
    a_c->wri = a_c->inp;
    a_c->unk = FALSE;
    a_d->sta = status_OK;
    a_d->unk = FALSE;
    adc_val[ind] = (float)*a_d->rea / (float)0x0FFF * ref_val;
printf("ind = %d, *a_d->rea = 0x%04X\n", ind, *a_d->rea);

  }
  else {
    cmd->unk = TRUE;
    dig->sta = status_KO;
    a_c->unk = TRUE;
    a_d->sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_read_pds_channels_panel(void) {

  int j;

//~--
  
  for (j=0; j<NL; j++) {
    _w *a = &dig_adr[j];
    show_bin_btn_string(1, a, &REA_SIDE_BTN[j]);
    show_bin_btn_string(1, a, &REA_ADR_BTN[j]);
    show_bin_btn_string(2, a, &REA_CHA_BTN[j][0]);
  }
  for (j=0; j<NL; j++) {
    _r *d = &dig_dat[j];
    show_bit_box_string(1, d, NULL, &REA_DIG_BOX[j]);
  }
  
//~--

  for (j=0; j<NL; j++) {
    _w *a = &adc_cha[j];
    show_bin_btn_string(2, a, &ADC_CHA_BTN[j][0]);
    if (a->inp > 0x20) {
      fl_set_object_color(ADC_CHA_BTN[j][0]->obj, FL_RED, FL_YELLOW);
      fl_set_object_color(ADC_CHA_BTN[j][1]->obj, FL_RED, FL_YELLOW);
    }
  }

//~--

  for (j=0; j<NL; j++) {
    _w *a = &adc_cha[j];
    _r *d = &adc_cod[j];
    int c = FL_GREEN;
    char b[80];
    if (d->unk) sprintf(b, " ");
    else        sprintf(b, "0x%04X", *d->rea);
    fl_set_object_label(ADC_COD_BOX[j], b);
    if (d->unk) sprintf(b, " ");
    else        sprintf(b, "%5.3f", adc_val[j]);
    fl_set_object_label(ADC_VAL_BOX[j], b);
    if (d->unk)        c = FL_YELLOW;
    if (a->inp > 0x20) c = FL_RED;
    fl_set_object_color(ADC_COD_BOX[j], c, 0);
    fl_set_object_color(ADC_VAL_BOX[j], c, 0);
  }
}

//~============================================================================
//
//                  WRITE_PDS_CHANNELS
//
//~============================================================================

void create_write_pds_channels_panel(int x0, int y0) {

  SW_BTN *B;
  SW_BTN *BTN[100];
  FL_OBJECT *obj;
  int j, i, n;
  int x, y, w, h;
  int w1 = 30, w2 = 30, w3 = 40, w4 = 20, w5 = 60;
  int h1 = 40, h2 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + NL * h2 + (NL / 4) * 5 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "Side", FL_NORMAL_SIZE);
  x = x + w1;
  add_frame_box(x, y, w2, h1, "Adr", FL_NORMAL_SIZE);
  x = x + w2;
  add_frame_box(x, y, w3, h1, "Chan", FL_NORMAL_SIZE);
  x = x + w3;
  add_frame_box(x, y, w4, h1, "D", FL_NORMAL_SIZE);
  x = x + w4;
  add_frame_box(x, y, w5, h1, "Cmd", FL_NORMAL_SIZE);
  x = x + w5;
  y = y + h1;
  
  for (j=0; j<NL; j++) {
    n = 0;
    x = x0 + 5;
    BTN[n++] = 
    WRI_SIDE_BTN[j] =
      B = add_switch_btn(x, y, w1, h2, a_b, FL_MEDIUM_SIZE, 2, -1);
      B->m = 0x0200;
    x = x + w1;
    BTN[n++] = 
    WRI_ADR_BTN[j] =
      B = add_switch_btn(x, y, w2, h2, hex, FL_MEDIUM_SIZE, 16, -1);
      B->m = 0x01E0;
    x = x + w2;
    BTN[n++] = 
    WRI_CHA_BTN[j][0] =
      B = add_switch_btn(x, y, w3/2, h2, hex, FL_MEDIUM_SIZE, 2, -1);
      B->m = 0x0010;
    x = x + w3 / 2;
    BTN[n++] = 
    WRI_CHA_BTN[j][1] =
      B = add_switch_btn(x, y, w3/2, h2, hex, FL_MEDIUM_SIZE, 16, -1);
      B->m = 0x000F;
    x = x + w3 / 2;
    BTN[n++] = 
    WRI_DIG_BTN[j] =
      B = add_switch_btn(x, y, w4, h2, hex, FL_MEDIUM_SIZE, 2, -1);
      B->m = 0x0800;
    x = x + w4;
    for (i=0; i<n; i++) {
      B = BTN[i];
      B->p = &digital[j].inp;
      B->i = unpack32(*B->p, B->m);
      fl_set_object_label(B->obj, B->l[B->i]);
      B->call = SW_BTN_CALLBACK;
    }
    pack32((int32*)&digital[j].inp, 0x0400, 1);
    obj = add_normal_button(x, y, w5, h2, "WRITE", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, write_pds_channels_callback, j);
    x = x + w5;
    y = y + h2;
    if (!((j + 1) % 4)) y = y + 5;
  }
}

//~----------------------------------------------------------------------------

void write_pds_channels_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _w *d = &digital[ind];
    
  msg = -1;
  update_objects();

  exec_PDS_cmd((int16)d->inp, 1, NULL, NULL, &err);

  if (!err) {
    d->wri = d->inp;
    d->unk = FALSE;
  }
  else {
    d->unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_write_pds_channels_panel(void) {

  int j;
  
  for (j=0; j<NL; j++) {
    _w *d = &digital[j];
    show_bin_btn_string(1, d, &WRI_SIDE_BTN[j]);
    show_bin_btn_string(1, d, &WRI_ADR_BTN[j]);
    show_bin_btn_string(2, d, &WRI_CHA_BTN[j][0]);
    show_bin_btn_string(1, d, &WRI_DIG_BTN[j]);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);
  update_reference_panel();
  update_read_pds_channels_panel();
  update_write_pds_channels_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  return 0;
}

//~============================================================================
