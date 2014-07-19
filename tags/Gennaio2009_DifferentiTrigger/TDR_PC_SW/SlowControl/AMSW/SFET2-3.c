// file SFET2-3.c
//
// SFET2 & SFEA2 Controller
//
// A.Lebedev May-2008...

#include "template.h"
#include "sfet2def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"SFET2/SFEA2 - 3  Controller"};      // global variable
char *date  = {"13-Dec-08"};                        // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"

int TX = 2;                 // global variable
int RX = 2;                 // global variable
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH;            // global variable
int16 NODE_ADR;             // global variable

int P = 0;                  // global variable

char adr_config_file_name[80];                        // global variable
char dat_config_file_name[80];                        // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;
bool  chk;

//~-- Crate Selector

SW_BTN *CRATE_BTN;
int crate = 0;

//~-- Power Control (addresses SDR2)

DAT_BOX *PWR_BOX[5];
_r r_pwr_sta[9][5];

//~-- Thresholds

SW_BTN   *THR_VAL_BTN[5][6];
SW_BTN   *THR_COD_BTN[5][6];
SW_BTN   *THR_REF_BTN[5][6];
DAT_BOX  *THR_VAL_BOX[5][6];
DAT_BOX  *THR_COD_BOX[5][6];
DAT_BOX  *THR_REF_BOX[5][6];
_r         r_thr[9][5][6];
_ww       ww_thr[9][5][6];
bool      follow_thr[5][6], follow_thr_all[5];

//~-- Temperature

DAT_BOX  *TMP_BOX[5];
_r          r_tmp[9][5];

//~-- Actel Version ID

DAT_BOX *VER_BOX[5];
_r r_ver[9][5];

//~--

char *hex[16] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

char *brd_nam[5] = {"SFET2-A", "SFET2-B", "SFET2-C", "SFET2-D", "SFEA2"};
int   brd_adr[5] = { SFET2_A,   SFET2_B,   SFET2_C,   SFET2_D,   SFEA2};

char *pwr[4] = {"OFF", "ON", "TRIP", "?"};

char *i_e[2] = {"INT", "EXT"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void create_crate_selector_panel(int x0, int y0);
int get_crate_address(void);
void crate_selector_callback(int32 data);
void update_crate_selector_panel(void);

void create_sfet_sfea_control_panel(int x0, int y0, int brd);
void power_control_callback(FL_OBJECT* obj, long data);
void read_power_status_callback(FL_OBJECT* obj, long data);
void write_threshold_callback(FL_OBJECT* obj, long data);
void read_threshold_callback(FL_OBJECT* obj, long data);
void read_temperature_callback(FL_OBJECT* obj, long data);
void read_actel_version_callback(FL_OBJECT* obj, long data);
void read_all_sfet_sfea_callback(FL_OBJECT* obj, long data);
void update_sfet_sfea_control_panel(void);

//~--

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int brd;
  int WW = 875, HH = 435;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH, "SDR2 Path");
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_crate_selector_panel(140, 0);

  for (brd=0; brd<5; brd++) {
    create_sfet_sfea_control_panel( 0+175*brd, 45, brd);
  }

  end_form(MAIN);
  
  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  int thr, brd, crt;
  static bool first = TRUE;

  if (first) {
    for (crt=0, brd=0; crt<8; crt++) {
      if (sboard[crt].typ == 1) {
        brd_nam[brd]   = sboard[crt].nam;
        brd_adr[brd++] = sboard[crt].adr;
      }
    }
  }

  for (crt=0; crt<9; crt++) {
    for (brd=0; brd<5; brd++) {
      invalidate_r(first, &r_pwr_sta[crt][brd], 1);
      invalidate_r(first, &r_tmp[crt][brd],     1);
      invalidate_r(first, &r_ver[crt][brd],     1);
    }
  }
  
  for (brd=0; brd<5; brd++) {
    follow_thr_all[brd] = FALSE;
  }
  
  for (crt=0; crt<9; crt++) {
    for (brd=0; brd<5; brd++) {
      for (thr=0; thr<6; thr++) {
        follow_thr[brd][thr] = FALSE;
        invalidate_r(first,   &r_thr[crt][brd][thr], 1);
        invalidate_ww(first, &ww_thr[crt][brd][thr], 1, 0x00000000);
      }
    }
  }

  read_dat_config_file(dat_config_file_name);
  first = FALSE;
}

//~----------------------------------------------------------------------------

void refresh_objects(void) {

  invalidate_data();
  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void switch_btn_callback(int32 data) {

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;
  int thr, brd, crt;

  file = fopen(filename, "r");
  if (file != NULL) {
    for (crt=0; crt<9; crt++) {
      for (brd=0; brd<5; brd++) {
        for (thr=0; thr<6; thr++) {
          fread(ww_thr[crt][brd][thr].inp, 4, 1, file);
        }
      }
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;
  int thr, brd, crt;

  file = fopen(filename, "w");
  if (file != NULL) {
    for (crt=0; crt<9; crt++) {
      for (brd=0; brd<5; brd++) {
        for (thr=0; thr<6; thr++) {
          fwrite(ww_thr[crt][brd][thr].inp, 4, 1, file);
        }
      }
    }
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            CRATE SELECTOR PANEL
//
//~============================================================================

extern _scrate scrate[8];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_crate_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 35, h1 = 15, h2 = 20;
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Crate", 'S');
  y = y + h1;

  crate = get_crate_address();

  CRATE_BTN = add_switch_btn(x, y, w1, h2, NULL, 'N', 8, crate);
  CRATE_BTN->call = crate_selector_callback;
}

//~-------------------------------------------------------------------------------

int get_crate_address() {

  int crt = 8;
  
  if (use_AMSW) {
    for (crt=0; crt<8; crt++) {
      if ((JINF_PATH == (scrate[crt].lnk << 8 | 0x003F))  ||
          (JINF_PATH == (scrate[crt].lnk << 8 | 0x0004))  ||
          (JINF_PATH == (scrate[crt].lnk << 8 | 0x0005))) break;
    }
  }

  if (use_TCP || use_EAS) {
    for (crt=0; crt<8; crt++) {
      if (NODE_ADR == scrate[crt].adr) break;
    }
  }

  return crt;
}

//~-------------------------------------------------------------------------------

void crate_selector_callback(int32 data) {

  crate = CRATE_BTN->i;

  if (use_AMSW) {
    JINF_PATH = scrate[crate].lnk << 8 | 0x003F;
  }

  if (use_TCP || use_EAS) {
    NODE_ADR = scrate[crate].adr;
  }

  SW_BTN_CALLBACK(0);
}

//~-------------------------------------------------------------------------------

void update_crate_selector_panel(void) {

  int thr, brd;
  static int old_crate = -1;
  
  SW_BTN *B = CRATE_BTN;

  crate = get_crate_address();

  if (crate < 8) set_object_label(B->obj, scrate[crate].nam, FL_WHEAT, 0, FL_BLACK);
  else           set_object_label(B->obj, "N/A",             FL_RED,   0, FL_YELLOW);

  if (crate != old_crate) {
    old_crate = crate;
    for (brd=0; brd<5; brd++) {
      _r  *p = &r_pwr_sta[crate][brd];
      _r *t = &r_tmp[crate][brd];
      _r *v = &r_ver[crate][brd];
      if (p->sta != status_KO) p->sta = status_NA;
      PWR_BOX[brd]->d = p;
      for (thr=0; thr<6; thr++) {
        _r  *r = &r_thr[crate][brd][thr];
        _ww *w = &ww_thr[crate][brd][thr];
        if (r->sta != status_KO) r->sta = status_NA;
        THR_VAL_BTN[brd][thr]->d = w;
        THR_COD_BTN[brd][thr]->d = w;
        THR_REF_BTN[brd][thr]->d = w;
        THR_VAL_BOX[brd][thr]->d = r;
        THR_COD_BOX[brd][thr]->d = r;
        THR_REF_BOX[brd][thr]->d = r;
      }
      if (t->sta != status_KO) t->sta = status_NA;
      TMP_BOX[brd]->d = t;
      if (v->sta != status_KO) v->sta = status_NA;
      VER_BOX[brd]->d = v;
    }
  }
}

//~============================================================================
//
//            SFET / SFEA CONTROL PANEL
//
//~============================================================================

extern int16 pwr_ctl_adr;
extern int16 pwr_msk_adr;
extern float V_ref_int;
extern float V_ref_ext;
extern _b thr_msk;
extern _b ref_msk;


_b pwr_sta_msk = {0x00, 0x0003, 4};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_sfet_sfea_control_panel(int x0, int y0, int brd) {

  FL_OBJECT *obj;
  int thr;
  int x, y;

{//~--- Panel and Actel ID(Version) ---

  int w1 = 110, w2 = 35, w3 = 20, h1 = 340, h2 = 15;

  fl_add_box(FL_DOWN_BOX, x0, y0, 5+w1+w2+w3+5, h1, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1,    2*h2, brd_nam[brd], 'L');
  x = x + w1;

  add_frame_box(x, y, w2+w3, h2,   "Actel ID",   'S');
  y = y + h2;
  create_dat_box_vstring(
    1, x, y, w2, h2, 'N', &r_ver[crate][brd], &ver_msk, &VER_BOX[brd]);
  x = x + w2;
  obj = add_normal_button(x, y, w3, h2, "R", 'N');
    fl_set_object_callback(obj, read_actel_version_callback, brd);
  y = y + h2;
}

{//~--- Power Control ---


  int w1 = 40, w2 = 35, w3 = 35, w4 = 20, h1 = 20;

  x = x0 + 5;
  add_frame_box(x, y, w1, h1, "Power", 'S');
  x = x + w1;
  obj = add_normal_button(x, y, w2, h1, "ON", 'N');
    fl_set_object_callback(obj, power_control_callback, 10*brd+1);
  x = x + w2;
  obj = add_normal_button(x, y, w2, h1, "OFF", 'N');
    fl_set_object_callback(obj, power_control_callback, 10*brd+0);
  x = x + w2;
  create_dat_box_vstring(
    1, x, y, w3, h1, 'N', &r_pwr_sta[crate][brd], &pwr_sta_msk, &PWR_BOX[brd]);
  x = x + w3;
  obj = add_normal_button(x, y, w4, h1, "R", 'N');
    fl_set_object_callback(obj, read_power_status_callback, brd);
  y = y + h1 + 5;
}

{//~--- Thresholds ---

  int x1, w;
  int w1 = 20, w2 = 30, w3 = 35, w4 = 30, w5 = 30, w6 = 20, h1 = 15;
  char **txt1;
  char *txt1a[2] = {"0\n1\n2", "3\n4"};
  char *txt1b[2] = {"0\n1",    "2\n3"};
  char *txt2[3] = {"LT", "HT", "SHT"};
  char *txt3[1] = {"Cmd"};

  x = x0 + 5;
  add_frame_box(x, y, w1, h1, "Ch",   'S');
  x = x + w1;
  add_frame_box(x, y, w2, h1, "Thr",  'S');
  x = x + w2;
  add_frame_box(x, y, w3, h1, "mV",   'S');
  x = x + w3;
  add_frame_box(x, y, w4, h1, "code", 'S');
  x = x + w4;
  add_frame_box(x, y, w5, h1, "Vref", 'S');
  x = x + w5;
  add_frame_box(x, y, w6, h1, "",     'S');
  y = y + h1;

  x = x0 + 5;
  txt1 = brd != 4 ? txt1a : txt1b;
  create_frame_box_vstring(2, x, y,         w1, 6*h1, txt1, 'S'+100, 'C');
  w = w1 + w2;
  create_frame_box_vstring(1, x, y+12*h1+5, w,  2*h1, txt3, 'S',     'R');
  x = x + w1;
  create_frame_box_vstring(3, x, y,         w2, 2*h1, txt2, 'S', 'C');
  create_frame_box_vstring(3, x, y+6*h1+5,  w2, 2*h1, txt2, 'S', 'C');
  x = x + w2;
  
  x1 = x;
  for (thr=0; thr<6; thr++) {
    create_btn_vstring(
      1, x, y,    w3, h1, NULL, 'N', &ww_thr[crate][brd][thr], &thr_msk, &THR_VAL_BTN[brd][thr]);
    add_hidden_btn_zones(THR_VAL_BTN[brd][thr], 2, 1, 16);
    create_dat_box_vstring(
      1, x, y+h1, w3, h1,       'N',  &r_thr[crate][brd][thr], &thr_msk, &THR_VAL_BOX[brd][thr]);
    x = x + w3;

    create_btn_vstring(
      1, x, y,    w4, h1, NULL, 'N', &ww_thr[crate][brd][thr], &thr_msk, &THR_COD_BTN[brd][thr]);
    add_hidden_btn_zones(THR_COD_BTN[brd][thr], 2, 1, 16);
    create_dat_box_vstring(
      1, x, y+h1, w4, h1,       'N',  &r_thr[crate][brd][thr], &thr_msk, &THR_COD_BOX[brd][thr]);
    x = x + w4;

    create_btn_vstring(
      1, x, y,    w5, h1, i_e,  'N', &ww_thr[crate][brd][thr], &ref_msk, &THR_REF_BTN[brd][thr]);
    create_dat_box_vstring(
      1, x, y+h1, w5, h1,       'N',  &r_thr[crate][brd][thr], &ref_msk, &THR_REF_BOX[brd][thr]);
    x = x + w5;
    if (brd == 4 && thr != 0 && thr != 3) {
      fl_deactivate_object(THR_VAL_BTN[brd][thr]->obj);
      fl_hide_object(      THR_VAL_BTN[brd][thr]->obj);
      fl_deactivate_object(THR_COD_BTN[brd][thr]->obj);
      fl_hide_object(      THR_COD_BTN[brd][thr]->obj);
      fl_deactivate_object(THR_REF_BTN[brd][thr]->obj);
      fl_hide_object(      THR_REF_BTN[brd][thr]->obj);
      fl_deactivate_object(THR_VAL_BOX[brd][thr]->obj);
      fl_hide_object(      THR_VAL_BOX[brd][thr]->obj);
      fl_deactivate_object(THR_COD_BOX[brd][thr]->obj);
      fl_hide_object(      THR_COD_BOX[brd][thr]->obj);
      fl_deactivate_object(THR_REF_BOX[brd][thr]->obj);
      fl_hide_object(      THR_REF_BOX[brd][thr]->obj);
      add_frame_box(x1, y, w3+w4+w5+w6, 2*h1, "N/A", 'S');
    }
    else {
      obj = add_normal_button(x, y,    w6, h1, "W", 'N');
        fl_set_object_callback(obj, write_threshold_callback, 10*brd+thr);
      obj = add_normal_button(x, y+h1, w6, h1, "R", 'N');
        fl_set_object_callback(obj, read_threshold_callback, 10*brd+thr);
    }
    y = y + 2 * h1;
    if (thr == 2) y = y + 5;
    x = x1;
  }

  w = w3 + w4 + w5 + w6;
  obj = add_normal_button(x, y, w, h1, "WRITE", 'N');
    fl_set_object_callback(obj, write_threshold_callback, 10*brd+9);
  y = y + h1;
  obj = add_normal_button(x, y, w, h1, "READ", 'N');
    fl_set_object_callback(obj, read_threshold_callback, 10*brd+9);
  y = y + h1 + 5;
}

{//~--- Temperature ---

  int w1 = 95, w2 = 50, w3 = 20, h1 = 20;

  x = x0 + 5;
  add_frame_box(x, y, w1, h1, "Temperature, C", 'S');
  x = x + w1;

  create_dat_box_vstring(
    1, x, y, w2, h1, 'N', &r_tmp[crate][brd], &tmp_msk[2], &TMP_BOX[brd]);
  x = x + w2;

  obj = add_normal_button(x, y, w3, h1, "R", 'N');
    fl_set_object_callback(obj, read_temperature_callback, brd);
  y = y + h1;
}

{//~--- READ ALL Buttons ---

  int w1 = 165, w2 = 875, h1 = 20, h2 = 20;

  x = x0 + 5;
  obj = add_normal_button(x, y, w1, h1, "READ  ALL", 'N');
    fl_set_object_callback(obj, read_all_sfet_sfea_callback, brd);
  y = y + h1 + 5;

  if (!brd) {
    x = x0;
    obj = add_normal_button(x, y, w2, h2, "READ  ALL", 'N');
      fl_set_object_callback(obj, read_all_sfet_sfea_callback, 9);
  }
  y = y + h2;
}

}

//~-------------------------------------------------------------------------------

void power_control_callback(FL_OBJECT* obj, long data) {

  int  brd = data / 10;
  bool o_o = data % 10;
  int adr = brd_adr[brd] - 1;
  _r *r = &r_pwr_sta[crate][brd];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta = status_NA;

  err = 0x0000;
  
  if (!err) {
    int16 wri = ~pwr_ctl[adr].msk;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_msk_adr, wri, NULL, NULL, &err);
  }
  
  if (!err) {
    int16 wri = o_o ? pwr_ctl[adr].msk | 0x80 : 0x00;
    exec_SDR2_SC_command(WRITE, SDR2, pwr_ctl_adr, wri, NULL, NULL, &err);
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void read_power_status_callback(FL_OBJECT* obj, long data) {

  int brd = data;
  int adr = brd_adr[brd] - 1;
  int16 rea, r_msk, r_ctl;
  _r *r = &r_pwr_sta[crate][brd];

  if (obj) {
    msg = -1;
    update_objects();
  }

  r->sta  = status_NA;
  *r->was = *r->rea;

  err = 0x0000;
  
  if (!err) {
    exec_SDR2_SC_command(READ, SDR2, pwr_msk_adr, 0, &rea, NULL, &err);
    if (!err) r_msk = rea;
  }

  if (!err) {
    exec_SDR2_SC_command(READ, SDR2, pwr_ctl_adr, 0, &rea, NULL, &err);
    if (!err) r_ctl = rea;
  }

  if (TRUE) {
    if (!err) {
      bool ctl = unpack32(r_ctl, pwr_ctl[adr].msk);
      bool mon = unpack32(r_ctl, pwr_mon[adr].msk);
      if (ctl) {
        if (mon) r->rea[0] = 1;  // ON
        else     r->rea[0] = 2;  // TRIP
      }
      else {
        if (mon) r->rea[0] = 3;  // ?
        else     r->rea[0] = 0;  // OFF
      }
      if (r->unk) *r->was = *r->rea;
      r->unk = FALSE;
      r->sta = status_OK;
    }
    else {
      r->sta = status_KO;
    }
  }
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void write_threshold_callback(FL_OBJECT* obj, long data) {

  int brd = data / 10;
  int ind = data % 10;
  int adr = brd_adr[brd];
  int thr;

  if (obj) {
#if 0
    msg = 0;
    if ((ind < 9 && follow_thr[brd][ind]) || (ind == 9 && follow_thr_all[brd])) {
      set_normal_button(obj, 0);
      if (ind < 9) follow_thr[brd][ind] = FALSE;
      else         follow_thr_all[brd]  = FALSE;
      update_objects();
      return;
    }
    else {
      if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
        set_normal_button(obj, 1);
        if (ind < 9) follow_thr[brd][ind] = TRUE;
        else         follow_thr_all[brd]  = TRUE;
        update_objects();
        return;
      }
    }
#endif
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (thr=0; thr<6; thr++) {
    if (ind != 9 && ind != thr) continue;
    if (go_on(err)) {
      int dac = thr % 3;
      int cha = thr / 3;
      _r  *r =  &r_thr[crate][brd][3*cha+dac];
      _ww *w = &ww_thr[crate][brd][3*cha+dac];
      int16 wri = w->inp[0] | (cha * 0x0400) | 0x0300;
      r->sta = status_NA;
      exec_SDR2_SC_command(WRITE, adr, dac_adr[dac][cha], wri, NULL, NULL, &err);
      if (!err) {
        w->wri[0] = w->inp[0];
        w->unk = FALSE;
      }
      else {
        w->unk = TRUE;
      }
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void read_threshold_callback(FL_OBJECT* obj, long data) {

  int brd = data / 10;
  int ind = data % 10;
  int adr = brd_adr[brd];
  int thr;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;

  for (thr=0; thr<6; thr++) {
    if (ind != 9 && ind != thr) continue;
    if (go_on(err)) {
      int dac = thr % 3;
      int cha = thr / 3;
      _r  *r  = &r_thr[crate][brd][3*cha+dac];
      int16 rea;
      r->was[0] = r->rea[0];
      exec_SDR2_SC_command(READ, adr, dac_adr[dac][cha], 0, &rea, NULL, &err);
      if (!err) {
        r->rea[0] = rea;
        if (r->unk) r->was[0] = r->rea[0];
        r->unk = FALSE;
        if (unpack32(rea, 0x0400) != cha) r->sta = status_KO;
        else                              r->sta = status_OK;
      }
      else {
        r->sta = status_KO;
      }
    }
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void read_temperature_callback(FL_OBJECT* obj, long data) {

  int brd = data;
  int adr = brd_adr[brd];
  int i;
  int16 rea;
  float T[2];
  _r *r = &r_tmp[crate][brd];
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  err = 0x0000;
  
  for (i=0; i<2; i++) {
    if (!err) {
      exec_SDR2_SC_command(READ, adr, tmp_adr[i], 0, &rea, NULL, &err);
      if (!err) T[i] = (float)rea;
    }
  }
  
  r->was[0] = r->rea[0];
  if (!err) {
    float t = 235.0 - 400.0 * T[0] / T[1];
    r->rea[0] = *(int32*)&t;
    if (r->unk) r->was[0] = r->rea[0];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void read_actel_version_callback(FL_OBJECT* obj, long data) {

  int brd = data;
  int adr = brd_adr[brd];
  int16 rea;
  _r *r = &r_ver[crate][brd];

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  r->was[0] = r->rea[0];
  exec_SDR2_SC_command(READ, adr, ver_adr[adr], 0, &rea, NULL, &err);

  if (!err) {
    r->rea[0] = rea;
    if (r->unk) r->was[0] = r->rea[0];
    r->unk = FALSE;
    r->sta = status_OK;
  }
  else {
    r->sta = status_KO;
  }

  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void read_all_sfet_sfea_callback(FL_OBJECT* obj, long data) {

  int brd = data;
  int i;

  if (obj) {
    msg = -1;
    update_objects();
  }

  err = 0x0000;

  for (i=0; i<5; i++) {
    if (brd != 9 && i != brd) continue;
    if (go_on(err)) read_power_status_callback(NULL, i);
    if (go_on(err)) read_threshold_callback(NULL, 10*i+9);
    if (go_on(err)) read_temperature_callback(NULL, i);
    if (go_on(err)) read_actel_version_callback(NULL, i);
  }

  if (obj) {
    msg = 0;
    update_objects();
  }
}

//~-------------------------------------------------------------------------------

void update_sfet_sfea_control_panel(void) {

  SW_BTN  *BTN;
  DAT_BOX *BOX;
  int brd, thr;
  int32 cod;
  int val;
  char b[80];
  
//~---

  for (brd=0; brd<5; brd++) {

    BOX = PWR_BOX[brd];
    _r *r = &r_pwr_sta[crate][brd];
    show_dat_box_string(1, pwr, NULL, &BOX);
    if ((r->sta == status_OK) && !r->unk && (r->rea[0] > 1)) {
      fl_set_object_color( BOX->obj, FL_RED, 0);
      fl_set_object_lcolor(BOX->obj, FL_YELLOW);
    }

//~---

    for (thr=0; thr<6; thr++) {
      _r  *r  =  &r_thr[crate][brd][thr];
      _ww *w  = &ww_thr[crate][brd][thr];
      bool w_unk = w->unk;
      float V_ref;

      if (w->inp[0] != w->wri[0]) w->unk = TRUE;

      BTN = THR_VAL_BTN[brd][thr];
      show_btn_string(1, NULL, &BTN);
      cod = BTN->i;
      V_ref = (w->inp[0] & ref_msk.msk) ? V_ref_ext : V_ref_int;
      val = 2.0 * (float)cod / 256.0 * V_ref;
      sprintf(b, "%4d", val);
      fl_set_object_label(BTN->obj, b);

      w->unk = w_unk;

      BTN = THR_COD_BTN[brd][thr];
      show_btn_string(1, NULL, &BTN);
      sprintf(b, "%3d", cod);
      fl_set_object_label(BTN->obj, b);

      BTN = THR_REF_BTN[brd][thr];
      show_btn_string(1, NULL, &BTN);

      cod = r->rea[0] & 0xFF;
      V_ref = (r->rea[0] & ref_msk.msk) ? V_ref_ext : V_ref_int;
      val = 2.0 * (float)cod / 256.0 * V_ref;
      sprintf(b, "%4d", val);
      show_dat_box_string(1, NULL, b,     &THR_VAL_BOX[brd][thr]);
      sprintf(b, "%3d", cod);
      show_dat_box_string(1, NULL, b,     &THR_COD_BOX[brd][thr]);
      show_dat_box_string(1,  i_e, NULL,  &THR_REF_BOX[brd][thr]);
    }

//~---

    show_dat_box_string(1, NULL, "%+6.1f", &TMP_BOX[brd]);
    show_dat_box_string(1, NULL, "%04X",   &VER_BOX[brd]);
  }
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, &chk, &P);

  update_crate_selector_panel();
  update_sfet_sfea_control_panel();

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
