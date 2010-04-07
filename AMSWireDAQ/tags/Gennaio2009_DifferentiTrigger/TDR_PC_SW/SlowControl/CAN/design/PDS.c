// file PDS.c
//
// PDS Low Level Primitive "Controller"
//
// A.Lebedev Dec-2004...

#include "template.h"
#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"PDS Controller"};                     // global variable
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

int32 wri_adr[2] = {0x07F004, 0x07F005};
int32 rea_adr = 0x07F005;

//~--

FL_FORM *MAIN = NULL;

int  msg;
int16 err;

//~--

SW_BTN *WRI_MEM_BIN_BTN[2][8], *WRI_MEM_HEX_BTN[2][2];

_w wri_mem[2];

//~--

FL_OBJECT *REA_MEM_BIN_BOX[8], *REA_MEM_HEX_BOX[2];

_r rea_mem;

//~--

FL_OBJECT *REA_ADC_BOX[32];

_r rea_adc;

//~--

SW_BTN *WRI_PDS_SIDE_BTN[2], *WRI_PDS_ADR_BTN[2], *WRI_PDS_CHA_BTN[2][2], *WRI_PDS_DIG_BTN[2];

_w wri_pds[2];

//~--

SW_BTN *REA_PDS_SIDE_BTN, *REA_PDS_ADR_BTN, *REA_PDS_CHA_BTN[2];
FL_OBJECT *REA_PDS_DIG_BOX;

_w rea_pds_adr;
_r rea_pds_dig;

SW_BTN *REA_PDS_ADC_HEX_BTN[2];

_w rea_pds_adc_adr;
_r rea_pds_adc_dat;

FL_OBJECT *REA_PDS_ADC_COD_BOX, *REA_PDS_ADC_VAL_BOX;

_r rea_pds_adc_dat;
float rea_pds_adc_dat_val;

FL_OBJECT *REA_PDS_ADC_REF_BTN, *REA_PDS_ADC_REF_INP;

bool  rea_pds_adc_ref_unk;
float rea_pds_adc_ref_inp;
float rea_pds_adc_ref_wri;

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *a_b[2] = {"A", "B"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);
void ENABLE_callback(FL_OBJECT *obj, long data);

void refresh_objects(void);
void invalidate_data(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_write_memory_panel(int x0, int y0);
void write_memory_callback(FL_OBJECT *obj, long data);
void update_write_memory_panel(void);

void create_read_memory_panel(int x0, int y0);
void read_memory_callback(FL_OBJECT *obj, long data);
void update_read_memory_panel(void);

void create_read_adc_panel(int x0, int y0);
void read_adc_callback(FL_OBJECT *obj, long data);
void update_read_adc_panel(void);

void create_write_pds_channel_panel(int x0, int y0, int ind);
void write_pds_channel_callback(FL_OBJECT *obj, long data);
void update_write_pds_channel_panel(int ind);

void create_read_pds_channel_panel(int x0, int y0);
void read_pds_adc_ref_inp_focus_callback(FL_OBJECT *obj, long data);
void read_pds_adc_ref_inp_callback(FL_OBJECT *obj, long data);
void read_pds_channel_callback(FL_OBJECT *obj, long data);
void update_read_pds_channel_panel(void);

void update_objects(void);

//~============================================================================
//
//                  MAIN FORM
//
//~============================================================================

void create_main_form(void) {

  FL_OBJECT *obj;
  int WW = 445, HH = 660;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  obj = add_normal_button(0, 45, 445, 45, "ENABLE SOME PORT", 'L');
    fl_set_object_callback(obj, ENABLE_callback, 0);

  create_write_memory_panel(       0, 90);
  create_read_memory_panel(        0, 255);
  create_read_adc_panel(           0, 340);
  create_write_pds_channel_panel(  0, 510, 0);
  create_write_pds_channel_panel(225, 510, 1);
  create_read_pds_channel_panel(   0, 560);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void ENABLE_callback(FL_OBJECT *obj, long data) {

  msg = -1;
  update_objects();

  enable_CGS_serial_lines(&err);
  
  msg = 1;
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
  
  for (i=0; i<2; i++) wri_mem[i].inp = 0x00;
  for (i=0; i<2; i++) wri_mem[i].unk = TRUE;

  rea_mem.unk = TRUE;

  rea_adc.unk = TRUE;

  for (i=0; i<2; i++) wri_pds[i].inp = 0x00;
  for (i=0; i<2; i++) wri_pds[i].unk = TRUE;

  rea_pds_adr.inp = 0x00;
  rea_pds_adr.unk = TRUE;

  rea_pds_dig.unk = TRUE;

  rea_pds_adc_adr.inp = 0;
  rea_pds_adc_adr.unk = TRUE;

  rea_pds_adc_dat.unk = TRUE;

  rea_pds_adc_ref_inp = 1.000;
  rea_pds_adc_ref_unk = TRUE;

  read_dat_config_file(dat_config_file_name);
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "r");
  if (file) {
    fread(&wri_mem[0].inp,      sizeof(wri_mem[0].inp),      1, file);
    fread(&wri_mem[1].inp,      sizeof(wri_mem[1].inp),      1, file);
    fread(&wri_pds[0].inp,      sizeof(wri_pds[0].inp),      1, file);
    fread(&wri_pds[1].inp,      sizeof(wri_pds[1].inp),      1, file);
    fread(&rea_pds_adr.inp,     sizeof(rea_pds_adr.inp),     1, file);
    fread(&rea_pds_adc_adr.inp, sizeof(rea_pds_adc_adr.inp), 1, file);
    fread(&rea_pds_adc_ref_inp, sizeof(rea_pds_adc_ref_inp), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~----------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file) {
    fwrite(&wri_mem[0].inp,      sizeof(wri_mem[0].inp),      1, file);
    fwrite(&wri_mem[1].inp,      sizeof(wri_mem[1].inp),      1, file);
    fwrite(&wri_pds[0].inp,      sizeof(wri_pds[0].inp),      1, file);
    fwrite(&wri_pds[1].inp,      sizeof(wri_pds[1].inp),      1, file);
    fwrite(&rea_pds_adr.inp,     sizeof(rea_pds_adr.inp),     1, file);
    fwrite(&rea_pds_adc_adr.inp, sizeof(rea_pds_adc_adr.inp), 1, file);
    fwrite(&rea_pds_adc_ref_inp, sizeof(rea_pds_adc_ref_inp), 1, file);
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
//                  WRITE_MEMORY
//
//~===============================================================================

void create_write_memory_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int j;
  char b[80];
  int x, y, w, h;
  int w1 = 60, w2 = 40, w3 = 55;
  int h1 = 35, h2 = 20;
  char *txt[2][8] = {
    {"ADD2", "ADD1", "ADD0", "CH4", "CH3",      "CH2", "CH1",      "CH0"},
    {"NC",   "NC",   "NC",   "NC",  "Dig\nOut", "R/W", "Sel\nA/B", "ADD3"}};
  
  w = 5 + w1 + 8 * w2 + w3 +5;
  h = 5 + h1 + 2 * h2 + 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  y = y0 + 5;
  h = h1 + 2 * h2;
  for (j=0; j<2; j++) {
    x = x0 + 5;
    sprintf(b, "%06X", wri_adr[j]);
    add_frame_box(x, y, w1, h, b, 'N');
    x = x + w1;
    create_frame_box_hstring(8, x, y, w2, h1, &txt[j][0], 'N', 'C');
    y = y + h1;
    create_bit_btn_hstring(
      8, x, y, w2, h2, hex, 'M', &wri_mem[j], &WRI_MEM_BIN_BTN[j][0]);
    y = y + h2;
    create_hex_btn_hstring(
      2, x, y, 4*w2, h2, hex, 'M', &wri_mem[j], &WRI_MEM_HEX_BTN[j][0]);
    y = y + h2 + 5;
  }
  
  x = x0 + 5 + w1 + 8 * w2;
  y = y0 + 5;
  h = h1 + 2 * h2 + 5 + h1 + 2 * h2;
  obj = add_normal_button(x, y, w3, h, "WRITE", 'M');
    fl_set_object_callback(obj, write_memory_callback, 0);
}

//~----------------------------------------------------------------------------

void write_memory_callback(FL_OBJECT *obj, long data) {

  int8 dat[2];
  
  msg = -1;
  update_objects();

  dat[0] = wri_mem[0].inp;
  dat[1] = wri_mem[1].inp;
  write_USCM_memory(2, wri_adr[0], dat, &err);

  if (!err) {
    wri_mem[0].wri = wri_mem[0].inp;
    wri_mem[1].wri = wri_mem[1].inp;
    wri_mem[0].unk = FALSE;
    wri_mem[1].unk = FALSE;
  }
  else {
    wri_mem[0].unk = TRUE;
    wri_mem[1].unk = TRUE;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_write_memory_panel(void) {

  int j;
  
  for (j=0; j<2; j++) {
    show_bin_btn_string(8, &wri_mem[j], &WRI_MEM_BIN_BTN[j][0]);
    show_bin_btn_string(2, &wri_mem[j], &WRI_MEM_HEX_BTN[j][0]);
  }
}

//~============================================================================
//
//                  READ_MEMORY
//
//~===============================================================================

void create_read_memory_panel(int x0, int y0) {

  FL_OBJECT *obj;
  char b[80];
  int x, y, w, h;
  int w1 = 60, w2 = 40, w3 = 55;
  int h1 = 35, h2 = 20;
  char *txt[8] = {"NC", "NC", "NC", "NC", "NC", "NC", "NC", "Dig\nInp"};
  
  w = 5 + w1 + 8 * w2 + w3 + 5;
  h = 5 + h1 + 2 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  h = h1 + 2 * h2;
  sprintf(b, "%06X", rea_adr);
  add_frame_box(x, y, w1, h, b, 'N');
  x = x + w1;
  create_frame_box_hstring(8, x, y, w2, h1, &txt[0], 'N', 'C');
  y = y + h1;
  create_data_box_hstring(8, x, y,   w2, h2, 'M', &REA_MEM_BIN_BOX[0]);
  y = y + h2;
  create_data_box_hstring(2, x, y, 4*w2, h2, 'M', &REA_MEM_HEX_BOX[0]);
  y = y + h2 + 5;
  rea_mem.was = malloc(4);
  rea_mem.rea = malloc(4);
  
  x = x0 + 5 + w1 + 8 * w2;
  y = y0 + 5;
  h = h1 + 2 * h2;
  obj = add_normal_button(x, y, w3, h, "READ", 'M');
    fl_set_object_callback(obj, read_memory_callback, 0);
}

//~----------------------------------------------------------------------------

void read_memory_callback(FL_OBJECT *obj, long data) {

  int8 dat;
  
  msg = -1;
  update_objects();

  *rea_mem.was = *rea_mem.rea;

  read_USCM_memory(1, rea_adr, &dat, &err);

  if (!err) {
    *rea_mem.rea = dat;
    if (rea_mem.unk) *rea_mem.was = *rea_mem.rea;
    rea_mem.unk = FALSE;
    rea_mem.sta = status_OK;
  }
  else {
    rea_mem.sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_read_memory_panel(void) {

  show_bit_box_string(8, &rea_mem, NULL, &REA_MEM_BIN_BOX[0]);
  show_hex_box_string(2, &rea_mem, NULL, &REA_MEM_HEX_BOX[0]);
}

//~============================================================================
//
//                  READ_ADC
//
//~===============================================================================

void create_read_adc_panel(int x0, int y0) {

  FL_OBJECT *obj;
  char b[80];
  int i;
  int x, y, w, h;
  int w1 = 25, w2 = 60, w3 = 80;
  int h1 = 20;
  
  w = 5 + 4 * (w1 + w2) + 3 * 5 + w3 + 5;
  h = 5 + 8 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  for (i=0; i<32; i++) {
    sprintf(b, "%02X", i+1);
    add_frame_box(x, y, w1, h1, b, 'N');
    x = x + w1;
    REA_ADC_BOX[i] =
    obj = add_data_box(x, y, w2, h1, "0xABCD", 'M');
    x = x + w2 + 5;
    if (i == 3) {
      obj = add_normal_button(x-5, y, w3, 8*h1, "READ\nALL\nADC", 'M');
      fl_set_object_callback(obj, read_adc_callback, 0);
    }
    if (!((i + 1) % 4)) {
      x = x0 + 5;
      y = y + h1;
    }
  }
  rea_adc.was = malloc(32*4);
  rea_adc.rea = malloc(32*4);
}

//~----------------------------------------------------------------------------

void read_adc_callback(FL_OBJECT *obj, long data) {

  int i;
  int16 dat[32];
  
  msg = -1;
  update_objects();

  for (i=0; i<32; i++) rea_adc.was[i] = rea_adc.rea[i];

  read_ADC(dat, &err);

  if (!err) {
    for (i=0; i<32; i++) {
      rea_adc.rea[i] = dat[i];
      if (rea_adc.unk) rea_adc.was[i] = rea_adc.rea[i];
    }
    rea_adc.unk = FALSE;
    rea_adc.sta = status_OK;
  }
  else {
    rea_adc.sta = status_KO;
  }
  
  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_read_adc_panel(void) {

  show_num_box_string(32, &rea_adc, NULL, "0x%04X", &REA_ADC_BOX[0]);
}

//~============================================================================
//
//                  WRITE_PDS_CHANNEL
//
//~===============================================================================

void create_write_pds_channel_panel(int x0, int y0, int ind) {

  SW_BTN *B;
  SW_BTN *BTN[100];
  FL_OBJECT *obj;
  int i, n = 0;
  int x, y, w, h;
  int w1 = 30, w2 = 30, w3 = 40, w4 = 20, w5 = 90;
  int h1 = 20, h2 = 20;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "Side", 'N');
  BTN[n++] = 
  WRI_PDS_SIDE_BTN[ind] =
    B = add_switch_btn(x, y+h1, w1, h2, a_b, 'M', 2, -1);
    B->m = 0x0200;
  x = x + w1;

  add_frame_box(x, y, w2, h1, "Adr", 'N');
  BTN[n++] = 
  WRI_PDS_ADR_BTN[ind] =
    B = add_switch_btn(x, y+h1, w2, h2, hex, 'M', 16, -1);
    B->m = 0x01E0;
  x = x + w2;
  
  add_frame_box(x, y, w3, h1, "Chan.", 'N');
  BTN[n++] = 
  WRI_PDS_CHA_BTN[ind][0] =
    B = add_switch_btn(x, y+h1, w3/2, h2, hex, 'M', 2, -1);
    B->m = 0x0010;
  x = x + w3 / 2;
  BTN[n++] = 
  WRI_PDS_CHA_BTN[ind][1] =
    B = add_switch_btn(x, y+h1, w3/2, h2, hex, 'M', 16, -1);
    B->m = 0x000F;
  x = x + w3 / 2;
  
  add_frame_box(x, y, w4, h1, "D", 'N');
  BTN[n++] = 
  WRI_PDS_DIG_BTN[ind] =
    B = add_switch_btn(x, y+h1, w4, h2, hex, 'M', 2, -1);
    B->m = 0x0800;
  x = x + w4;

  for (i=0; i<n; i++) {
    B = BTN[i];
    B->p = &wri_pds[ind].inp;
    B->i = unpack32(*B->p, B->m);
    fl_set_object_label(B->obj, B->l[B->i]);
    B->call = SW_BTN_CALLBACK;
  }

  pack32((int32*)&wri_pds[ind].inp, 0x0400, 1);
  
  obj = add_normal_button(x, y, w5, h1+h2, "WRITE", 'M');
    fl_set_object_callback(obj, write_pds_channel_callback, ind);
}

//~----------------------------------------------------------------------------

void write_pds_channel_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  _w *d = &wri_pds[ind];
  
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

void update_write_pds_channel_panel(int ind) {

  _w *d = &wri_pds[ind];
  
  show_bin_btn_string(1, d, &WRI_PDS_SIDE_BTN[ind]);
  show_bin_btn_string(1, d, &WRI_PDS_ADR_BTN[ind]);
  show_bin_btn_string(2, d, &WRI_PDS_CHA_BTN[ind][0]);
  show_bin_btn_string(1, d, &WRI_PDS_DIG_BTN[ind]);
}

//~============================================================================
//
//                  READ_PDS_CHANNEL
//
//~===============================================================================

void create_read_pds_channel_panel(int x0, int y0) {

  SW_BTN *B;
  SW_BTN *BTN[100];
  FL_OBJECT *obj;
  int i, n = 0;
  int x, y, w, h;
  int w1 = 30, w2 = 30, w3 = 40, w4 = 20, w5 = 60, w6 = 75;
  int h1 = 40, h2 = 20;
  char *txt[4] = {"ADC cha", "Code", "Value", "Ref."};
  
  w = 5 + w1 + w2 + w3 + w4 + 4 * w5 + w6 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, h1, "Side", 'N');
  BTN[n++] =
  REA_PDS_SIDE_BTN =
    B = add_switch_btn(x, y+h1, w1, h2, a_b, 'M', 2, -1);
    B->m = 0x0200;
  x = x + w1;

  add_frame_box(x, y, w2, h1, "Adr", 'N');
  BTN[n++] =
  REA_PDS_ADR_BTN =
    B = add_switch_btn(x, y+h1, w2, h2, hex, 'M', 16, -1);
    B->m = 0x01E0;
  x = x + w2;
  
  add_frame_box(x, y, w3, h1, "Chan.", 'N');
  BTN[n++] =
  REA_PDS_CHA_BTN[0] =
    B = add_switch_btn(x, y+h1, w3/2, h2, hex, 'M', 2, -1);
    B->m = 0x0010;
  x = x + w3 / 2;
  BTN[n++] =
  REA_PDS_CHA_BTN[1] =
    B = add_switch_btn(x, y+h1, w3/2, h2, hex, 'M', 16, -1);
    B->m = 0x000F;
  x = x + w3 / 2;

  for (i=0; i<n; i++) {
    B = BTN[i];
    B->p = &rea_pds_adr.inp;
    B->i = unpack32(*B->p, B->m);
    fl_set_object_label(B->obj, B->l[B->i]);
    B->call = SW_BTN_CALLBACK;
  }

  pack32((int32*)&rea_pds_adr.inp, 0x0400, 0);

//~--

  add_frame_box(x, y, w4, h1, "D", 'N');
  REA_PDS_DIG_BOX =
    obj = add_data_box(x, y+h1, w4, h2, "", 'M');
  x = x + w4;

  rea_pds_dig.rea = malloc(4);
  rea_pds_dig.was = malloc(4);
  
  
//~--

  add_frame_box(x, y, 4*w5, h2, "Analog", 'N');
  y = y + h2;

  create_frame_box_hstring(4, x, y, w5, h2, &txt[0], 'N', 'C');
  y = y + h2;

  create_hex_btn_hstring(
    2, x, y, w5/2, h2, hex, 'M', &rea_pds_adc_adr, &REA_PDS_ADC_HEX_BTN[0]);
  x = x + w5;

  REA_PDS_ADC_COD_BOX = add_data_box(x, y, w5, h2, "", 'M');
  x = x + w5;

  REA_PDS_ADC_VAL_BOX = add_data_box(x, y, w5, h2, "", 'M');
  x = x + w5;

  rea_pds_adc_dat.rea = malloc(4);
  rea_pds_adc_dat.was = malloc(4);

  REA_PDS_ADC_REF_BTN =
  obj = add_switch_button(x, y, w5, h2, "", 'M');
    fl_set_object_callback(obj, read_pds_adc_ref_inp_focus_callback, 0);
  REA_PDS_ADC_REF_INP =
  obj = fl_add_input(FL_FLOAT_INPUT, x-5, y-5, w5+10, h2+10,"");
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_WHEAT, FL_WHEAT);
    fl_set_object_lsize(obj, 'L');
    fl_set_object_lstyle(obj, FL_BOLD_STYLE + FL_ENGRAVED_STYLE);
    fl_set_input_color(obj, FL_BLACK, FL_BLACK);
    fl_set_input_return(obj, FL_RETURN_END);
    fl_set_object_callback(obj, read_pds_adc_ref_inp_callback, 0);
    fl_deactivate_object(obj);
    fl_hide_object(obj);
  x = x + w5;


//~--

  y = y0 + 5;
  obj = add_normal_button(x, y, w6, h1+h2, "READ", 'M');
    fl_set_object_callback(obj, read_pds_channel_callback, 0);
}

//~----------------------------------------------------------------------------

void read_pds_adc_ref_inp_focus_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *obj1 = REA_PDS_ADC_REF_INP;
  FL_OBJECT *obj2 = REA_PDS_ADC_REF_BTN;
  char b[80];
  int mode = -1;

  if (fl_get_button_numb(obj2) == FL_LEFT_MOUSE)  mode = 0;
  if (fl_get_button_numb(obj2) == FL_RIGHT_MOUSE) mode = 1;

  fl_activate_object(obj1);
  fl_show_object(obj1);
  if (mode) sprintf(b, "%5.3f", rea_pds_adc_ref_inp);
  else      sprintf(b, " ");
  fl_set_input(obj1, b);
  
  fl_deactivate_object(obj2);
  fl_hide_object(obj2);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_pds_adc_ref_inp_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *obj1 = REA_PDS_ADC_REF_INP;
  FL_OBJECT *obj2 = REA_PDS_ADC_REF_BTN;

  rea_pds_adc_ref_inp = atof(fl_get_input(obj1));
  
  fl_deactivate_object(obj1);
  fl_hide_object(obj1);

  fl_activate_object(obj2);
  fl_show_object(obj2);

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void read_pds_channel_callback(FL_OBJECT *obj, long data) {

  FL_OBJECT *obj1 = REA_PDS_ADC_REF_INP;
  FL_OBJECT *obj2 = REA_PDS_ADC_REF_BTN;
  _w *cmd = &rea_pds_adr;
  _r *dig = &rea_pds_dig;
  _w *a_c = &rea_pds_adc_adr;
  _r *a_d = &rea_pds_adc_dat;

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
    rea_pds_adc_ref_wri  = rea_pds_adc_ref_inp;
    rea_pds_adc_ref_unk  = FALSE;
    rea_pds_adc_dat_val  = (float)*a_d->rea / (float)0x0FFF * rea_pds_adc_ref_wri;
    fl_deactivate_object(obj1);
    fl_hide_object(obj1);
    fl_activate_object(obj2);
    fl_show_object(obj2);
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

void update_read_pds_channel_panel(void) {

  FL_OBJECT *obj;
  _w *a;
  _r *d;
  int c = FL_GREEN;
  char b[80];

//~--
  
  a = &rea_pds_adr;
  d = &rea_pds_dig;

  show_bin_btn_string(1, a, &REA_PDS_SIDE_BTN);
  show_bin_btn_string(1, a, &REA_PDS_ADR_BTN);
  show_bin_btn_string(2, a, &REA_PDS_CHA_BTN[0]);
  show_bit_box_string(1, d, NULL, &REA_PDS_DIG_BOX);

//~--

  a = &rea_pds_adc_adr;
  d = &rea_pds_adc_dat;

  show_bin_btn_string(2, a, &REA_PDS_ADC_HEX_BTN[0]);
  if (a->inp > 0x20) {
    fl_set_object_color(REA_PDS_ADC_HEX_BTN[0]->obj, FL_RED, 0);
    fl_set_object_color(REA_PDS_ADC_HEX_BTN[1]->obj, FL_RED, 0);
  }

//~--

  if (d->unk) sprintf(b, " ");
  else        sprintf(b, "0x%04X", *d->rea);
  fl_set_object_label(REA_PDS_ADC_COD_BOX, b);

  if (d->unk) sprintf(b, " ");
  else        sprintf(b, "%5.3f", rea_pds_adc_dat_val);
  fl_set_object_label(REA_PDS_ADC_VAL_BOX, b);

  if (d->unk)        c = FL_YELLOW;
  if (a->inp > 0x20) c = FL_RED;
  fl_set_object_color(REA_PDS_ADC_COD_BOX, c, 0);
  fl_set_object_color(REA_PDS_ADC_VAL_BOX, c, 0);

//~--

  obj = REA_PDS_ADC_REF_BTN;
  sprintf(b, "%5.3f", rea_pds_adc_ref_inp);
  fl_set_object_label(obj, b);
  if (rea_pds_adc_ref_inp != rea_pds_adc_ref_wri) c = FL_YELLOW;
  else                                            c = FL_GREEN;
  if (rea_pds_adc_ref_unk)                        c = FL_YELLOW;
  fl_set_object_color(obj, c, FL_RED);
}

//~===============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);
  update_write_memory_panel();
  update_read_memory_panel();
  update_read_adc_panel();
  update_write_pds_channel_panel(0);
  update_write_pds_channel_panel(1);
  update_read_pds_channel_panel();

  write_dat_config_file(dat_config_file_name);

  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);

  return 0;
}

//~============================================================================
