// file 66.c
//
// Low-level CAN-bus tester
// Graphical version of my program "6" (one CAN_SND-RCV cycle on CAN-bus)
//
// A.Lebedev Oct-2004...
//

#include "canlib.h"
#include "xformslib.h"

//~----------------------------------------------------------------------------

char *dat_config_filename = {"66-dat.configfile"};

int P = 3;                                          // global variable

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

typedef struct {
  FL_FORM *FORM;
  void *vdata;
  long ldata;
} FD_FORM;

FD_FORM *MAIN;

//~--

#define NP 4

//~--

int16 err[NP];

struct {
  int        w;
  int32      msk;
  char      *txt[2][16];
  FL_OBJECT *BTN[NP];
  FL_OBJECT *BOX[NP];
} ID1[12] = {
  { 25, 0x80000000, {{ "0",  "1"}}},
  { 50, 0x60000000, {{"NXT", "ERR", "ABO", "END"}, { "I",  "L", "F", "F&L"}}},
  { 25, 0x10000000, {{"RQ", "RP"}}},
  { 25, 0x08000000, {{ "R",  "W"}}},
  { 25, 0x04000000, {{ "A",  "B"}}},
  { 25, 0x02000000, {{ "A",  "B"}}},
  { 25, 0x01000000, {{ "L", "NB"}}},
  { 25, 0x00000080, {{ "0",  "1"}}},
  { 25, 0x00000040, {{ "0",  "1"}}},
  { 25, 0x00000020, {{ "0",  "1"}}},
  { 25, 0x00000010, {{ "0",  "1"}}},
  { 25, 0x00000008, {{ "0",  "1"}}}};

struct {
  int        w;
  int32      msk;
  FL_OBJECT *BTN[NP];
  FL_OBJECT *BOX[NP];
} ID2[8] = {
  { 100, 0xF0000000},
  { 100, 0x0F000000},
  {  30, 0x00F00000},
  {  30, 0x000F0000},
  {  30, 0x0000F000},
  {  30, 0x00000F00},
  { 100, 0x000000F0},
  {  25, 0x00000008}};

struct {
  FL_OBJECT *BTN[NP];
  FL_OBJECT *BOX[NP];
} DLC;

struct {
  FL_OBJECT *BTN[NP][2];
  FL_OBJECT *BOX[NP][2];
} dat[8];

//~--

int32 ID_snd[NP], ID_snd_was[NP];
int32 ID_rcv[NP], ID_rcv_was[NP];

int8 DLC_snd[NP], DLC_snd_was[NP];
int8 DLC_rcv[NP], DLC_rcv_was[NP];

int dat_snd[NP][8], dat_snd_was[NP][8];
int dat_rcv[NP][8], dat_rcv_was[NP][8];

//~--

FL_OBJECT *SNDRCV[NP];

FL_OBJECT *MESSAGE[NP];
int msg[NP];

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

FD_FORM *create_main_form(void);

void invalidate_data(void);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

void create_SNDRCV_panel(int x0, int y0, int ind);
void btn_callback(FL_OBJECT *obj, long data);
void DLC_callback(FL_OBJECT *obj, long data);
void dat_callback(FL_OBJECT *obj, long data);
void SNDRCV_callback(FL_OBJECT *obj, long data);
void update_SNDRCV_panel(int ind);

void update_message(void);

void update_objects(void);

//~============================================================================

FD_FORM *create_main_form(void) {

  FL_OBJECT *obj;
  FD_FORM *fdui = NULL;
  int i;
  int H = 145;
  
  fdui = (FD_FORM *) fl_calloc(1, sizeof(*fdui));
  fdui->FORM = fl_bgn_form(FL_NO_BOX,855,40+NP*H);

  obj = fl_add_box(FL_FLAT_BOX,0,0,855,40+NP*H,"A.Lebedev 21-May-08");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);
    
  obj = fl_add_box(FL_NO_BOX,5,0,845,40,"Program 66");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_EMBOSSED_STYLE);

  for (i=0; i<NP; i++) {
    create_SNDRCV_panel(5, 40+i*H, i);
  }

  fl_end_form();
  fdui->FORM->fdui = fdui;
  
  invalidate_data();

  return fdui;
}

//~-------------------------------------------------------------------------------

void invalidate_data(void) {

  int i, j;
  
  for (j=0; j<NP; j++) {
    ID_snd[j]      = 0;
    ID_snd_was[j]  = 0;
    ID_rcv[j]      = 0;
    ID_rcv_was[j]  = 0;
    DLC_snd[j]     = 0;
    DLC_snd_was[j] = 0;
    DLC_rcv[j]     = 0;
    DLC_rcv_was[j] = 0;
    for (i=0; i<8; i++) {
      dat_snd[j][i]     = 0;
      dat_snd_was[j][i] = 0;
      dat_rcv[j][i]     = 0;
      dat_rcv_was[j][i] = 0;
    }
  }

  read_dat_config_file(dat_config_filename);
}

//~-------------------------------------------------------------------------------

bool read_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(ID_snd, sizeof(ID_snd), 1, file);
    fread(DLC_snd, sizeof(DLC_snd), 1, file);
    fread(dat_snd, sizeof(dat_snd), 1, file);
    fclose(file);
  }
  file = fopen(filename, "r");
  if (file != NULL) {
    fread(ID_snd_was, sizeof(ID_snd_was), 1, file);
    fread(DLC_snd_was, sizeof(DLC_snd_was), 1, file);
    fread(dat_snd_was, sizeof(dat_snd_was), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~-------------------------------------------------------------------------------

bool write_dat_config_file(char *filename) {

  FILE *file;

  file = fopen(filename, "w");
  if (file != NULL) {
    fwrite(ID_snd, sizeof(ID_snd), 1, file);
    fwrite(DLC_snd, sizeof(DLC_snd), 1, file);
    fwrite(dat_snd, sizeof(dat_snd), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================

void create_SNDRCV_panel(int x0, int y0, int ind) {

  FL_OBJECT *obj;
  char b[80];
  int i, j;
  int x, y;
  
  x = x0;
  y = y0;
  obj = fl_add_box(FL_DOWN_BOX,x,y,845,140,"");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, 30, 20, "", FL_NORMAL_SIZE);
  x = x + 30;
  add_frame_box(x, y, 445, 20, "CAN Message 29-bit ID", FL_NORMAL_SIZE);
  x = x + 445 + 5;
  add_frame_box(x, y, 30, 20, "DLC", FL_NORMAL_SIZE);
  x = x + 30 + 5;
  add_frame_box(x, y, 275, 20, "Data Bytes", FL_NORMAL_SIZE);
  x = x + 275 + 5;
  add_frame_box(x, y, 40, 20, "Cmd.", FL_NORMAL_SIZE);
  x = x + 40 + 5;

  x = x0 + 5;
  y = y + 20;

  add_frame_box(x, y,    30, 40, "SND", FL_NORMAL_SIZE);
  add_frame_box(x, y+40, 30, 40, "RCV", FL_NORMAL_SIZE);
  x = x + 30;

  for (i=0; i<7; i++) {
    ID1[i].BTN[ind] = 
    obj = add_switch_button(x, y, ID1[i].w, 20, "q", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, btn_callback, 64*ind+32*0+i);
    ID1[i].BOX[ind] = 
    obj = add_data_box(x, y+40, ID1[i].w, 20, "q", FL_MEDIUM_SIZE);
    x = x + ID1[i].w;
  }

  add_frame_box(x, y,    60, 20, "JMDC ID", FL_NORMAL_SIZE);
  add_frame_box(x, y+40, 60, 20, "USCM ID", FL_NORMAL_SIZE);
  x = x + 60;
  add_frame_box(x, y,    60, 20, "USCM ID", FL_NORMAL_SIZE);
  add_frame_box(x, y+40, 60, 20, "JMDC ID", FL_NORMAL_SIZE);
  x = x + 60;

  for (i=7; i<12; i++) {
    ID1[i].BTN[ind] = 
    obj = add_switch_button(x, y, ID1[i].w, 20, "q", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, btn_callback, 64*ind+32*0+i);
    ID1[i].BOX[ind] = 
    obj = add_data_box(x, y+40, ID1[i].w, 20, "q", FL_MEDIUM_SIZE);
    x = x + ID1[i].w;
  }

  x = x + 5;
  DLC.BTN[ind] =
  obj = add_switch_button(x, y, 30, 40, "Q", FL_MEDIUM_SIZE);
  fl_set_object_callback(obj, DLC_callback, ind);
  DLC.BOX[ind] =
  obj = add_data_box(x, y+40, 30, 40, "Q", FL_MEDIUM_SIZE);
  x = x + 30 + 5;
  
  for (i=0; i<8; i++) {
    sprintf(b, "#%d", i);
    add_frame_box(x, y,    30, 20, b, FL_NORMAL_SIZE);
    add_frame_box(x, y+40, 30, 20, b, FL_NORMAL_SIZE);
    for (j=0; j<2; j++) {
      dat[i].BTN[ind][j] =
      obj = add_switch_button(x, y+20, 15, 20, "F", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, dat_callback, 64*ind+32*j+i);
      dat[i].BOX[ind][j] =
      obj = add_data_box(x, y+60, 15, 20, "0", FL_MEDIUM_SIZE);
      x = x + 15;
    }
    x = x + 5;
  }
  
  SNDRCV[ind] =
  obj = add_normal_button(x, y, 40, 80, "SND\nRCV", FL_MEDIUM_SIZE);
  fl_set_object_callback(obj, SNDRCV_callback, ind);
  
  x = x0 + 5 + 30;
  y = y + 20;
  for (i=0; i<8; i++) {
    ID2[i].BTN[ind] = 
    obj = add_switch_button(x, y, ID2[i].w, 20, "q", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, btn_callback, 64*ind+32*1+i);
    ID2[i].BOX[ind] = 
    obj = add_data_box(x, y+40, ID2[i].w, 20, "q", FL_MEDIUM_SIZE);
    x = x + ID2[i].w;
  }

  x = x0 + 5;
  y = y + 40 + 20;
  msg[ind] = 0;
  MESSAGE[ind] =
  obj = fl_add_text(FL_NORMAL_TEXT,x,y,835,30,"");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
}

//~----------------------------------------------------------------------------

void btn_callback(FL_OBJECT *obj, long data) {

  int ind =  data / 64;
  int row = (data / 32) %  2;
  int i   =  data       % 32;
  int d = 0;
  int32 msk = row ? ID2[i].msk : ID1[i].msk;
  int val, max;
  int btn = fl_get_button_numb(obj);

  if (btn == FL_LEFT_MOUSE)  d =  1;
  if (btn == FL_RIGHT_MOUSE) d = -1;

  val = unpack32(ID_snd[ind], msk);
  max = unpack32(0xFFFFFFFF, msk);
  val = MOD(val+d, max+1);
  pack32(&ID_snd[ind], msk, val);

  msg[ind] = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void DLC_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int d = 0;
  int btn = fl_get_button_numb(obj);
  
  if (btn == FL_LEFT_MOUSE)  d =  1;
  if (btn == FL_RIGHT_MOUSE) d = -1;

  DLC_snd[ind] = MOD(DLC_snd[ind] + d, 16);

  msg[ind] = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void dat_callback(FL_OBJECT *obj, long data) {

  int ind =  data / 64;
  int j   = (data / 32) %  2;
  int i   =  data       % 32;
  int dig[2];
  int d = 0;
  int btn = fl_get_button_numb(obj);

  if (btn == FL_LEFT_MOUSE)  d =  1;
  if (btn == FL_RIGHT_MOUSE) d = -1;

  dig[0] = dat_snd[ind][i] / 16;
  dig[1] = dat_snd[ind][i] % 16;

  dig[j] = MOD(dig[j] + d, 16);

  dat_snd[ind][i] = 16 * dig[0] + dig[1];
  
  msg[ind] = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void SNDRCV_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  static _CAN_msg *request;
  static _CAN_msg *reply;
  static bool first = TRUE;
  int i;
  
  if (first) {
    request = malloc(sizeof(*request));
    reply   = malloc(sizeof(*reply));
    first = FALSE;
  }

  write_dat_config_file(dat_config_filename);

  msg[ind] = -1;
  update_objects();

  request->ID  = ID_snd[ind];
  request->len = DLC_snd[ind];
  for (i=0; i<MIN(DLC_snd[ind],8); i++) request->dat[i] = dat_snd[ind][i];
  
  CAN_SND_RCV(INITIALIZE, request, reply, &err[ind]);
  if (!err[ind]) CAN_SND_RCV(START, request, reply, &err[ind]);
  if (!err[ind]) while (CAN_SND_RCV(CONTINUE, request, reply, &err[ind]) != DONE);

  if (!err[ind]) {
    ID_snd_was[ind]  = ID_snd[ind];
    DLC_snd_was[ind] = DLC_snd[ind];
    for (i=0; i<8; i++) dat_snd_was[ind][i] = dat_snd[ind][i];
    ID_rcv[ind]  = reply->ID;
    DLC_rcv[ind] = reply->len;
    for (i=0; i<MIN(DLC_rcv[ind],8); i++) dat_rcv[ind][i] = reply->dat[i];
  }
  
  msg[ind] = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void update_SNDRCV_panel(int ind) {

  FL_OBJECT *obj;
  char b[80];
  int val;
  int i, j;
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;
  
  for (i=0; i<12; i++) {
    char *txt;
    obj = ID1[i].BTN[ind];
    val = unpack32(ID_snd[ind], ID1[i].msk);
    if (DLC_snd[ind] && ID1[i].txt[1][0]) txt = ID1[i].txt[1][val];
    else                                  txt = ID1[i].txt[0][val];
    if (val != unpack32(ID_snd_was[ind], ID1[i].msk)) color = warning;
    else                                              color = good;
    if ((i == 0 || i >= 7) && val != 0)               color = alarm;
    fl_set_object_label(obj, txt);
    fl_set_object_color (obj,  colors[color], FL_RED);
    fl_set_object_lcolor(obj, lcolors[color]);

    obj = ID1[i].BOX[ind];
    val = unpack32(ID_rcv[ind], ID1[i].msk);
    if (DLC_rcv[ind] && ID1[i].txt[1][0]) txt = ID1[i].txt[1][val];
    else                                  txt = ID1[i].txt[0][val];
    if (err[ind]) {
      color = error;
    }
    else {
      color = good;
      if ((i == 0 || i >= 7) && val != 0) color = alarm;
      if ((DLC_rcv[ind] == 0) && (i == 1) && (val == 1 || val == 2)) color = warning;
      if ((i == 2) && (val == 0)) color = warning;
    }
    fl_set_object_label(obj, txt);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  obj = DLC.BTN[ind];
  sprintf(b, "%X", DLC_snd[ind]);
  if (DLC_snd[ind] != DLC_snd_was[ind]) color = warning;
  else                                  color = good;
  if (DLC_snd[ind] > 8)                 color = alarm;
  fl_set_object_label(obj, b);
  fl_set_object_color (obj,  colors[color], FL_RED);
  fl_set_object_lcolor(obj, lcolors[color]);

  obj = DLC.BOX[ind];
  sprintf(b, "%X", DLC_rcv[ind]);
  if (err[ind]) {
    color = error;
  }
  else {
    if (DLC_rcv[ind] > 8) color = alarm;
    else                  color = good;
  }
  fl_set_object_label(obj, b);
  fl_set_object_color (obj,  colors[color], 0);
  fl_set_object_lcolor(obj, lcolors[color]);
  
  for (i=0; i<8; i++) {
    int8 msk[2] = {0xF0, 0x0F};
    for (j=0; j<2; j++) {
      obj = dat[i].BTN[ind][j];
      val = unpack16(dat_snd[ind][i], msk[j]);
      sprintf(b, "%X", val);
      if (val != unpack16(dat_snd_was[ind][i], msk[j])) color = warning;
      else                                              color = good;
      if (DLC_snd[ind] > 8 || i >= DLC_snd[ind])        color = alarm;
      fl_set_object_label(obj, b);
      fl_set_object_color (obj,  colors[color], FL_RED);
      fl_set_object_lcolor(obj, lcolors[color]);

      obj = dat[i].BOX[ind][j];
      val = unpack16(dat_rcv[ind][i], msk[j]);
      if (i >= DLC_rcv[ind]) sprintf(b, " ");
      else                   sprintf(b, "%X", val);
      if (err[ind]) {
        color = error;
      }
      else {
        if (DLC_rcv[ind] > 8) color = alarm;
        else                  color = good;
      }
      fl_set_object_label(obj, b);
      fl_set_object_color (obj,  colors[color], 0);
      fl_set_object_lcolor(obj, lcolors[color]);
    }
  }

  if (DLC_snd[ind] && (ID_snd[ind] & 0x40000000)) {
    if (unpack32(ID_snd[ind], ID1[2].msk) != unpack16(dat_snd[ind][0], 0x80)) {
      color = alarm;
      fl_set_object_color (ID1[2].BTN[ind],     colors[color], FL_RED);
      fl_set_object_lcolor(ID1[2].BTN[ind],    lcolors[color]);
      fl_set_object_color (dat[0].BTN[ind][0],  colors[color], FL_RED);
      fl_set_object_lcolor(dat[0].BTN[ind][0], lcolors[color]);
    }
 
    if (unpack32(ID_snd[ind], ID1[3].msk) != unpack16(dat_snd[ind][0], 0x40)) {
      color = alarm;
      fl_set_object_color (ID1[3].BTN[ind],     colors[color], FL_RED);
      fl_set_object_lcolor(ID1[3].BTN[ind],    lcolors[color]);
      fl_set_object_color (dat[0].BTN[ind][0],  colors[color], FL_RED);
      fl_set_object_lcolor(dat[0].BTN[ind][0], lcolors[color]);
    }
  }
 
  for (i=0; i<8; i++) {
  
    int32 msk = 0xF0000000 >> (4 * i);

    obj = ID2[i].BTN[ind];
    val = unpack32(ID_snd[ind], msk);
    sprintf(b, "%X", val);
    if (val != unpack32(ID_snd_was[ind], msk))      color = warning;
    else                                            color = good;
    if ((i == 2 && val > 0) || (i == 3 && val > 3)) color = alarm;
    fl_set_object_label(obj, b);
    fl_set_object_color (obj,  colors[color], FL_RED);
    fl_set_object_lcolor(obj, lcolors[color]);

    if (err[ind]) color = error;
    else          color = good;
    obj = ID2[i].BOX[ind];
    val = unpack32(ID_rcv[ind], msk);
    sprintf(b, "%X", val);
    fl_set_object_label(obj, b);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  obj = MESSAGE[ind];
  
  if (msg[ind]) {
    if (msg[ind] < 0) {
      fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
      fl_set_object_lcolor(obj,FL_BLACK);
      fl_set_object_label(obj,"Command is being sent...");
    }
    else {
      if (err[ind]) {
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lcolor(obj,FL_YELLOW);
        fl_set_object_label(obj,"Command execution failed!");
      }
      else {
        fl_set_object_color(obj,FL_CYAN,FL_MCOL);
        fl_set_object_lcolor(obj,FL_BLACK);
        fl_set_object_label(obj,"Command executed O.K.");
      }
    }
    fl_show_object(obj);
  }
  else {
    fl_hide_object(obj);
  }
}

//~============================================================================

void update_objects(void) {

  int i;
  
  for (i=0; i<NP; i++) update_SNDRCV_panel(i);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  int one = 1;

  printf("Usage: %s [<Debug Printout Level>]\n", argv[0]);
  if (argc > 1) P = atoi(argv[1]);
  printf("P = %d\n", P);

  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream

  if (!initialize_EPP()) exit(1);

  fl_initialize(&one, &argv[0], " ", 0, 0);

  MAIN = create_main_form();
  fl_set_form_position(MAIN->FORM, 0, 0);
  fl_show_form(MAIN->FORM, FL_PLACE_GEOMETRY, FL_FULLBORDER, argv[0]);
  update_objects();

  while(1) {
    fl_do_forms();
  }
  return 0;
}

//~============================================================================
