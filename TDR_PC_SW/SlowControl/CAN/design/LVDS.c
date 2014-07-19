// file LVDS.c
//
// LVDS "Pro" Controller - A.Lebedev Aug-2004...
//

#include "uscmlib.h"
#include "xformslib.h"

//~----------------------------------------------------------------------------

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x00;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char *config_file_name = {"LVDS.configfile"};      // global variable

int P = 0;                                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define TIMER_PERIOD 2.0     // in seconds

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

typedef struct {
  FL_FORM *LVDS;
  void *vdata;
  long ldata;
} FD_LVDS;

FD_LVDS *LVDS;

FL_OBJECT *MESSAGE;
int msg;
int16 sta;

FL_OBJECT *CLEAR;

#define NADR 16

FL_OBJECT *WRI_ADR[NADR][2];
FL_OBJECT *WRI_DAT[NADR][16];
int16 wri_adr[NADR];
int16 wri_dat[NADR], wri_was[NADR];
int wri_sta[NADR];

FL_OBJECT *REA_ADR[NADR][2];
FL_OBJECT *REA_DAT[NADR][16];
int16 rea_adr[NADR];
int16 rea_dat[NADR], rea_was[NADR];
int rea_sta[NADR];

char *LVDS_dat_config_file_name = {"LVDS-dat.configfile"};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FD_LVDS *create_form_LVDS(void);

void invalidate_data(void);
void read_dat_config_file(void);
void write_dat_config_file(void);

void create_WRITE_panel(int x0, int y0);
void WRI_ADR_callback(FL_OBJECT *obj, long data);
void WRI_DAT_callback(FL_OBJECT *obj, long data);
void SET_callback(FL_OBJECT *obj, long data);
void update_WRITE_panel(void);

void create_READ_panel(int x0, int y0);
void REA_ADR_callback(FL_OBJECT *obj, long data);
void GET_callback(FL_OBJECT *obj, long data);
void update_READ_panel(void);

void CLEAR_callback(FL_OBJECT *obj, long data);

void TIMER_callback(FL_OBJECT *obj, long data);

void update_message(void);

void update_objects(void);
void start_LVDS(void);

//~============================================================================

FD_LVDS *create_form_LVDS(void) {

  FL_OBJECT *obj;
  
  FD_LVDS *fdui = (FD_LVDS *) fl_calloc(1, sizeof(*fdui));

  fdui->LVDS = fl_bgn_form(FL_NO_BOX,760,125+NADR*25);

  obj = fl_add_box(FL_FLAT_BOX,0,0,760,125+NADR*25,"A.Lebedev 26-Oct-08");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);

  obj = fl_add_box(FL_NO_BOX,130,5,625,45,"LVDS  Pro  Controller");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_EMBOSSED_STYLE);

  create_USCM_JMDC_panels();
  create_WRITE_panel( 5, 50);
  create_READ_panel(380, 50);

  CLEAR = 
  obj = add_normal_button(5, 85+NADR*25, 65, 35, "CLEAR", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, CLEAR_callback, 0);

  msg = 0;
  MESSAGE =
  obj = fl_add_text(FL_NORMAL_TEXT,75,85+NADR*25,680,35,"");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  obj = fl_add_timer(FL_HIDDEN_TIMER, 0, 0, 0, 0,"");
    fl_set_timer(obj, TIMER_PERIOD);
    fl_set_object_callback(obj, TIMER_callback, 0);

  fl_end_form();
  fdui->LVDS->fdui = fdui;

  invalidate_data();
  read_dat_config_file();
 
  return fdui;
}

//~-------------------------------------------------------------------------------

void invalidate_data(void) {

  int i;
  
  for (i=0; i<NADR; i++) {
    rea_sta[i] = status_NN;
    wri_sta[i] = status_NN;
    rea_adr[i] = i;
    wri_adr[i] = i;
    wri_dat[i] = 0;
  }
}

//~-------------------------------------------------------------------------------

void read_dat_config_file(void) {

  FILE *file;

  file = fopen(LVDS_dat_config_file_name, "r");
  if (file != NULL) {
    fread(&rea_adr[0], sizeof(rea_adr), 1, file);
    fread(&wri_adr[0], sizeof(wri_adr), 1, file);
    fread(&wri_dat[0], sizeof(wri_dat), 1, file);
    fclose(file);
  }
}

//~-------------------------------------------------------------------------------

void write_dat_config_file(void) {

  FILE *file;

  file = fopen(LVDS_dat_config_file_name, "w");
  if (file != NULL) {
    fwrite(&rea_adr[0], sizeof(rea_adr), 1, file);
    fwrite(&wri_adr[0], sizeof(wri_adr), 1, file);
    fwrite(&wri_dat[0], sizeof(wri_dat), 1, file);
    fclose(file);
  }
}

//~===============================================================================

void create_WRITE_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h, size;
  int wc = 40, wd = 20, w1 = 20, w2 = 15;
  int ha = 20, h1 = 25;
  
  w = 5 + wd + 2 * w1 + 5 + 16 * w2 + 3 * 5 + 5 + wc + 5;
  h = 5 + ha + NADR * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, "");

  size = FL_NORMAL_SIZE;
    
  x = x0 + 5;
  y = y0 + 5;
  w = wd + 2 * w1;
  obj = add_frame_box(x, y, w, ha, "Address", size);
  x = x + w + 5;
  w = 16 * w2 + 3 * 5;
  obj = add_frame_box(x, y, w, ha, "Writing", size);
  x = x + w + 5;
  obj = add_frame_box(x, y, wc, ha, "Cmd.", size);
  
  y = y + ha;

  for (j=0; j<NADR; j++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wd, h1, "0x", size);
    x = x + wd;
    for (i=0; i<2; i++) {
      WRI_ADR[j][i] =
      obj = add_switch_button(x, y, w1, h1, "a", FL_MEDIUM_SIZE);
        fl_set_object_callback(obj, WRI_ADR_callback, j*2+i);
      x = x + w1;
    }
    x = x + 5;
    for (i=0; i<16; i++) {
      WRI_DAT[j][i] =
      obj = add_switch_button(x, y, w2, h1, "w", FL_MEDIUM_SIZE);
        fl_set_object_callback(obj, WRI_DAT_callback, j*16+i);
      x = x + w2;
      if (i == 3 || i == 7 || i == 11) x = x + 5;
    }
    x = x + 5;
    obj = add_normal_button(x, y, wc, h1, "SET", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, SET_callback, j);
    y = y + h1;
  }
}

//~-------------------------------------------------------------------------------

void WRI_ADR_callback(FL_OBJECT *obj, long data) {

  int ind = data / 2;
  int i   = data % 2;
  int16 d[2];
  int16 k;

  k = wri_adr[ind];
  
  d[0] = unpack16(k, 0x00F0);
  d[1] = unpack16(k, 0x000F);

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  d[i] = MOD(d[i]+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) d[i] = MOD(d[i]-1, 16);

  pack16(&k, 0x00F0, d[0]);
  pack16(&k, 0x000F, d[1]);

  wri_adr[ind] = k;
  wri_sta[ind] = status_NN;
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void WRI_DAT_callback(FL_OBJECT *obj, long data) {

  int ind = data / 16;
  int bit = data % 16;
  
  wri_dat[ind] ^= 0x8000 >> bit;
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void SET_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 err;

  msg = -1;
  update_objects();

  write_LVDS_bus(wri_adr[ind], wri_dat[ind], &err);
  if (err) sta = 0;
  else     sta = 1;
  
  wri_sta[ind] = status_OK;
  wri_was[ind] = wri_dat[ind];
  
  write_dat_config_file();

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_WRITE_panel(void) {

  int i, j;
  
  for (j=0; j<NADR; j++) {
    int16 msk[2] = {0x00F0, 0x000F};
    for (i=0; i<2; i++) {
      FL_OBJECT *obj = WRI_ADR[j][i];
      char b[80];
      sprintf(b, "%01X", unpack16(wri_adr[j], msk[i]));
      fl_set_object_label(obj, b);
    }
    for (i=0; i<16; i++) {
      FL_OBJECT *obj = WRI_DAT[j][i];
      int color;
      int32 d = wri_was[j] ^ wri_dat[j];
      if (wri_dat[j] & 0x8000>>i) fl_set_object_label(obj, "1");
      else                        fl_set_object_label(obj, "0");
      if (wri_sta[j] == status_NN) {
        color = FL_YELLOW;
      }
      else {
        if (d & 0x8000>>i) color = FL_YELLOW;
        else               color = FL_GREEN;
      }
      fl_set_object_color(obj, color, FL_RED);
    }
  }
}

//~===============================================================================

void create_READ_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h, size;
  int wc = 40, wd = 20, w1 = 20, w2 = 15;
  int ha = 20, h1 = 25;
  
  w = 5 + wd + 2 * w1 + 5 + 16 * w2 + 3 * 5 + 5 + wc + 5;
  h = 5 + ha + NADR * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, "");

  size = FL_NORMAL_SIZE;
    
  x = x0 + 5;
  y = y0 + 5;
  w = wd + 2 * w1;
  obj = add_frame_box(x, y, w, ha, "Address", size);
  x = x + w + 5;
  w = 16 * w2 + 3 * 5;
  obj = add_frame_box(x, y, w, ha, "Reading", size);
  x = x + w + 5;
  obj = add_frame_box(x, y, wc, ha, "Cmd.", size);
  
  y = y + ha;

  for (j=0; j<NADR; j++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wd, h1, "0x", size);
    x = x + wd;
    for (i=0; i<2; i++) {
      REA_ADR[j][i] =
      obj = add_switch_button(x, y, w1, h1, "a", FL_MEDIUM_SIZE);
        fl_set_object_callback(obj, REA_ADR_callback, j*2+i);
      x = x + w1;
    }
    x = x + 5;
    for (i=0; i<16; i++) {
      REA_DAT[j][i] =
      obj = add_data_box(x, y, w2, h1, "w", FL_MEDIUM_SIZE);
      x = x + w2;
      if (i == 3 || i == 7 || i == 11) x = x + 5;
    }
    x = x + 5;
    obj = add_normal_button(x, y, wc, h1, "GET", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, GET_callback, j);
    y = y + h1;
  }
}

//~-------------------------------------------------------------------------------

void REA_ADR_callback(FL_OBJECT *obj, long data) {

  int ind = data / 2;
  int i   = data % 2;
  int16 d[2];
  int16 k;

  k = rea_adr[ind];
  
  d[0] = unpack16(k, 0x00F0);
  d[1] = unpack16(k, 0x000F);

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  d[i] = MOD(d[i]+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) d[i] = MOD(d[i]-1, 16);

  pack16(&k, 0x00F0, d[0]);
  pack16(&k, 0x000F, d[1]);

  rea_adr[ind] = k;
  rea_sta[ind] = status_NN;
    
  update_objects();
}

//~-------------------------------------------------------------------------------

void GET_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 rea;
  int16 err;

  msg = -1;
  update_objects();

  read_LVDS_bus(rea_adr[ind], &rea, &err);
  if (err) sta = 0;
  else     sta = 1;
  
  if (sta) {
    rea_was[ind] = rea_dat[ind];
    rea_dat[ind] = rea;
    rea_sta[ind] = status_OK;
  }
  else {
    if (rea_sta[ind] != status_NN) rea_sta[ind] = status_KO;
  }

  write_dat_config_file();

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_READ_panel(void) {

  int i, j;
  
  for (j=0; j<NADR; j++) {
    int16 msk[2] = {0x00F0, 0x000F};
    for (i=0; i<2; i++) {
      FL_OBJECT *obj = REA_ADR[j][i];
      char b[80];
      sprintf(b, "%01X", unpack16(rea_adr[j], msk[i]));
      fl_set_object_label(obj, b);
    }
    for (i=0; i<16; i++) {
      FL_OBJECT *obj = REA_DAT[j][i];
      int color;
      int32 d = rea_was[j] ^ rea_dat[j];
      if (rea_sta[j] == status_NN) {
        fl_set_object_label(obj, "");
        color = FL_YELLOW;
      }
      else {
        if (rea_dat[j] & 0x8000>>i) fl_set_object_label(obj, "1");
        else                        fl_set_object_label(obj, "0");
        if (rea_sta[j] == status_KO) {
          color = FL_BLACK;
        }
        else {
          if (d & 0x8000>>i) color = FL_YELLOW;
          else               color = FL_GREEN;
        }
      }
      fl_set_object_color(obj, color, 0);
      if (color == FL_BLACK) fl_set_object_lcolor(obj, FL_WHITE);
      else                   fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
}

//~===============================================================================

void CLEAR_callback(FL_OBJECT *obj, long data) {

  invalidate_data();
  update_objects();
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
        fl_set_object_label(obj,"Command executed O.K.");
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

void update_objects(void) {

  update_USCM_JMDC_panels();
  update_WRITE_panel();
  update_READ_panel();
  update_message();

  fl_check_forms();
}

//~----------------------------------------------------------------------------

void TIMER_callback(FL_OBJECT *obj, long data) {

#if LATER
  int i;
  for (i=0; i<N_USCM; i++) {
    if (USCM_monitoring[i] || USCM_monitoring_all) {
      read_USCM_tmp(i);
      update_USCM_tmp_value(i);
    }
  }
#endif
  fl_set_timer(obj, TIMER_PERIOD);
}

//~-------------------------------------------------------------------------------

void start_LVDS(void) {

  int one = 1;
  static char *title = {"LVDS Monitor"};
  char *adr[] = {(char*)&title};

  if (!initialize_EPP()) exit(1);

  fl_initialize(&one, adr, " ", 0, 0);
  LVDS = create_form_LVDS();
  fl_set_form_position(LVDS->LVDS, 0, 0);
  fl_show_form(LVDS->LVDS, FL_PLACE_GEOMETRY, FL_FULLBORDER, "LVDS");

  update_objects();
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);	// set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<Debug Printout Level>]\n", argv[0]);

  if (argc > 1) P = atoi(argv[1]);
  printf("P = %d\n", P);

  start_LVDS();

  while(1) {
    fl_do_forms();
  }
  return 0;
}

//~============================================================================
