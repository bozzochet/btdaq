// file CAB-0.c
//
//  CAB-0 Controller
//  Execute Command Sequences
//
//  A.Lebedev Dec-2008...
//

#include "cabdef.h"
#include "crisalib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *title = {"CAB-0 Controller"};         // global variable
char *date  = {"17-Jan-09"};                // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int JMDC_ID = 0x03;                                   // global variable
int USCM_ID = 0x18;                                   // global variable

int BUS_FOR_REQUEST = 0;                              // global variable
int BUS_FOR_REPLY = 0;                                // global variable

int P = 0;                                            // global variable

int32 DO_NOT_CREATE = 0x12345678;                     // global variable

int8 CAB_CMD_ID;                                      // global variable

//~----------------------------------------------------------------------------

FL_FORM *MAIN = NULL;

int   msg;
int16 err;

//~-- CAB Model Selector

SW_BTN *MODEL_BTN;
_ww w_model;
_b model = {0, 0x00000001, 2};

//~-- Wait Panel

FL_OBJECT *HOLD, *RESUME, *CANCEL;
DAT_BOX *TIME[2];
_r r_time;
int wait_tid;
bool waiting;
bool wait_on_hold;

//~-- Directory Panel

FL_OBJECT *SEQ_DIR_NAM, *SEQ_DIR_BRO;
char def_dir_name[512] = {"crisa-library/Sequences"};
char cur_dir_name[512];
char seq_fil_name[512];

//~-- Sequence Panel

FL_OBJECT *RELOAD, *EXEC, *SKIP, *BACK, *ON, *OFF, *SIZE, *FONT;
FL_OBJECT *SEQ_NAM, *SEQ_BRO;
int top_line = 1;
int sel_line = 0;
int bro_size, bot_line, mid_line, max_line;
int bro_font_size = 0;
int bro_font_font = 0;
bool executing_command;
bool auto_exec;

//~-- CAB Essential Status

FL_OBJECT *ESS_STA_BRO;
_r r_ess_sta;
bool ess_sta_unk = {TRUE};
int  ess_sta_updated = -1;

//~-- CAB Acknoledgement Code

FL_OBJECT *CAB_ACK[2];
_r r_cab_ack;

//~-----

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_main_form(void);

void invalidate_data(void);
void refresh_objects(void);
void switch_btn_callback(int32 data);
bool read_dat_config_file(char *filename);
bool write_dat_config_file(char *filename);

//~--

void reload_sequence_browser(void);
void next_command_line(FL_OBJECT *obj, int d);
void update_line_tag(FL_OBJECT *obj);

bool is_command(FL_OBJECT *obj, int line);
void execute_command(FL_OBJECT *obj, int line);

//~--

void create_model_selector_panel(int x0, int y0);

void create_CPU_control_panel(int x0, int y0);
void reset_SW_callback(FL_OBJECT* obj, long data);
void load_PGM_callback(FL_OBJECT* obj, long data);
void go_APSW_callback(FL_OBJECT* obj, long data);

void create_wait_panel(int x0, int y0);
void wait_callback(int tid, void *data);
void hold_wait_callback(FL_OBJECT* obj, long data);
void resume_wait_callback(FL_OBJECT* obj, long data);
void cancel_wait_callback(FL_OBJECT* obj, long data);
void update_wait_panel(void);

void create_directory_panel(int x0, int y0);
void directory_callback(FL_OBJECT* obj, long data);
void update_directory_panel(void);

void create_sequence_panel(int x0, int y0);
void auto_exec_callback(int tid, void *data);
void sequence_callback(FL_OBJECT* obj, long data);
void update_sequence_panel(void);

void create_essential_status_panel(int x0, int y0);
void get_essential_status_callback(FL_OBJECT* obj, long data);
void update_essential_status_panel(void);

void create_CAB_acknoledgment_panel(FL_FORM *form);
void update_CAB_acknoledgment_panel(int16 *err);

void update_objects(void);

//~============================================================================

void create_main_form(void) {

  int WW = 1100, HH = 970;

  invalidate_data();

  bgn_form(&MAIN, 0, 0, WW, HH);
  AFTER_ADDRESS_CHANGED_CALL = refresh_objects;
  AFTER_RUN_FLASH_CALL       = refresh_objects;
  SW_BTN_CALLBACK            = switch_btn_callback;

  create_model_selector_panel(150, 0);

  create_CPU_control_panel(0, 45);
  create_wait_panel(80, 315);

  create_directory_panel(0, 430);
  create_sequence_panel(360, 45);
  create_essential_status_panel(WW, 45);

  create_CAB_acknoledgment_panel(MAIN);

  end_form(MAIN);

  update_objects();
}

//~----------------------------------------------------------------------------

void invalidate_data(void) {

  static bool first = TRUE;

  strcpy(cur_dir_name, def_dir_name);
  strcpy(seq_fil_name, "");
  top_line = 1;
  sel_line = 0;
  bro_font_size = 0;
  bro_font_font = 0;
  waiting = FALSE;
  wait_on_hold = FALSE;
  executing_command = FALSE;
  auto_exec = FALSE;

  invalidate_ww(first, &w_model, 1, 0);

  invalidate_r(first, &r_time, 2);

  invalidate_r(first, &r_ess_sta, 100);
  invalidate_r(first, &r_cab_ack, 1);

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

  file = fopen(filename, "r");
  if (file != NULL) {
    fread(w_model.inp, 4, 1, file);
    fread(cur_dir_name, 1, sizeof(cur_dir_name), file);
    fread(seq_fil_name, 1, sizeof(seq_fil_name), file);
    fread(&top_line, 1, sizeof(top_line), file);
    fread(&sel_line, 1, sizeof(sel_line), file);
    fread(&bro_font_size, 1, sizeof(bro_font_size), file);
    fread(&bro_font_font, 1, sizeof(bro_font_font), file);
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
    fwrite(w_model.inp, 4, 1, file);
    fwrite(cur_dir_name, 1, sizeof(cur_dir_name), file);
    fwrite(seq_fil_name, 1, sizeof(seq_fil_name), file);
    fwrite(&top_line, 1, sizeof(top_line), file);
    fwrite(&sel_line, 1, sizeof(sel_line), file);
    fwrite(&bro_font_size, 1, sizeof(bro_font_size), file);
    fwrite(&bro_font_font, 1, sizeof(bro_font_font), file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

//~============================================================================
//
//            "LIBRARY"
//
//~============================================================================

void reload_sequence_browser(void) {

  FL_OBJECT *obj = SEQ_BRO;
  FILE *file;
  char input[2+160]= {"  "};

  file = fopen(seq_fil_name, "r");
  if (!file) return;

  rewind(file);
  
  fl_set_object_label(SEQ_NAM, seq_fil_name);
  fl_set_object_color(SEQ_NAM, FL_WHITE, 0);

  fl_freeze_form(MAIN);
  fl_clear_browser(obj);
  
  while (fgets(&input[2], sizeof(input), file)) {
    char *p;
    if ((p = index(input, '\n'))) *p = '\0';
    if ((p = index(input, '\r'))) *p = '\0';
    fl_add_browser_line(obj, input);
  }
  fclose(file);
  
  fl_unfreeze_form(MAIN);
}

//~----------------------------------------------------------------------------

void next_command_line(FL_OBJECT *obj, int d) {

  int i;

  if (sel_line) {
    for (i=sel_line+d; i<max_line && i>0; i=i+d) {
      if (is_command(obj, i)) {
        sel_line = i;
        break;
      }
    }
  }
}

//~----------------------------------------------------------------------------

void update_line_tag(FL_OBJECT *obj) {

  char b[180];
  
  strncpy(b, fl_get_browser_line(obj, sel_line), DIM(b));

  if (err) {
    b[0] = 'X';
  }
  else {
    int n = b[0] - '0';
    if (0 <= n && n <= 9) b[0] = '0' + MIN(n+1, 9);
    else                  b[0] = '0' + 1;
  }
  
  fl_replace_browser_line(obj, sel_line, b);
}

//~============================================================================

bool is_command(FL_OBJECT *obj, int line) {

  char input[160];
  static char *token[3];
  static bool first = TRUE;
  int n;

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  if (line <= 0 || line > fl_get_browser_maxline(obj)) return FALSE;
  
  strncpy(input, fl_get_browser_line(obj, line), DIM(input));
  get_tokens(input, ':', DIM(token), &token[0], &n);

  if (n == 3 && strcasestr(token[1], "SEND COMMAND")) return TRUE;
  if (n == 3 && strcasestr(token[1], "SELECT CAB CPU")) return TRUE;
  if (n == 3 && strcasestr(token[1], "SELECT REQUEST BUS")) return TRUE;
  if (n == 3 && strcasestr(token[1], "SELECT REPLY BUS")) return TRUE;
  if (n == 3 && strcasestr(token[1], "WAIT N SECONDS")) return TRUE;
   
  return FALSE;
}

//~----------------------------------------------------------------------------

void execute_command(FL_OBJECT *obj, int line) {

  char *directory = {"/home/ams/AAL/SlowControl/CAN/crisa-library/incmd"};
  char input[160];
  static char *token[3];
  static bool first = TRUE;
  int n;

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  strncpy(input, fl_get_browser_line(obj, line), DIM(input));
  get_tokens(input, ':', DIM(token), &token[0], &n);

  if (n == 3) {
    if (strcasestr(token[1], "SEND COMMAND")) {
      send_command_to_CAB(directory, token[2], &err);
    }
    if (strcasestr(token[1], "SELECT CAB CPU")) {
      select_CAB_CPU(token[2], &err);
    }
    if (strcasestr(token[1], "SELECT REQUEST BUS")) {
      select_REQUEST_BUS(token[2], &err);
    }
    if (strcasestr(token[1], "SELECT REPLY BUS")) {
      select_REPLY_BUS(token[2], &err);
    }
    if (strcasestr(token[1], "WAIT N SECONDS")) {
      _r *r = &r_time;
      r->rea[0] = wait_n_seconds(token[2], &err);
      if (r->rea[0] && !err) {
        waiting = TRUE;
        wait_on_hold = FALSE;
        r->rea[1] = 0;
        r->unk = FALSE;
        r->sta = status_NA;
        wait_tid = fl_add_timeout(1000, wait_callback, NULL);
        set_normal_button(HOLD, 0);
        set_normal_button(CANCEL, 0);
        update_wait_panel();
      }
    }
  }
}

//~============================================================================
//
//            MODEL SELECTOR PANEL
//
//~============================================================================

void create_model_selector_panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 40, h1 = 15, h2 = 20;
  static char *txt[] = {"EM", "FM"};
  
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "Model", 'S');
  y = y + h1;

  create_btn_hstring(1, x, y, w1, h2, txt, 'M', &w_model, &model, &MODEL_BTN);
}

//~============================================================================
//
//            CPU SOFTWARE CONTROL PANEL
//
//~============================================================================

void create_CPU_control_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 100, h1 = 25;

  w = 5 + w1 + 5;
  h = 5 + 6 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1, h1, "CPU", 'M');
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "RST to StartUp", 'N');
    fl_set_object_callback(obj, reset_SW_callback, 0);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "RST to Active",    'N');
    fl_set_object_callback(obj, reset_SW_callback, 1);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "LOAD  PGM  1",   'N');
    fl_set_object_callback(obj, load_PGM_callback, 0);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "LOAD  PGM  2",   'N');
    fl_set_object_callback(obj, load_PGM_callback, 1);
  y = y + h1;

  obj = add_normal_button(x, y, w1, h1, "GO  APSW",                 'N');
    fl_set_object_callback(obj, go_APSW_callback, 0);
  y = y + h1;
}

//~----------------------------------------------------------------------------

void reset_SW_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _r *r = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  if (ind == 0) CAB_reset_SW(0x8008, &err);
  if (ind == 1) CAB_reset_SW(0x0220, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void load_PGM_callback(FL_OBJECT* obj, long data) {

  int ind = data;
  _r *r = &r_ess_sta;
  
  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  if (ind == 0) CAB_load_APSW(0xA2, &err);
  if (ind == 1) CAB_load_APSW(0xA4, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~----------------------------------------------------------------------------

void go_APSW_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_ess_sta;

  if (obj) {
    msg = -1;
    update_objects();
  }
  
  if (r->sta != status_KO) r->sta = status_NA;

  CAB_go_APSW(0x00000000, &err);
  
  if (obj) {
    msg = 1;
    update_objects();
  }
}

//~============================================================================
//
//            WAIT PANEL
//
//~============================================================================

void create_wait_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 100, w2 = 80, w3 = 60, h1 = 20, h2 = 20;
  char *txt[] = {"Time Left", "Time Elapsed"};
  _b q[2] = {
    {0, 0xFFFFFFFF, -1},
    {1, 0xFFFFFFFF, -1}};
  
  w = 5 + w1 + w2 + 5;
  h = 5 + 3 * h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  
  add_frame_box(x, y, w1+w2, h1, "WAITING...", 'M');
  y = y + h1;

  create_frame_box_vstring(2, x, y, w1, h1, txt, 'N', 'L');
  x = x + w1;
  
  create_dat_box_vstring(2, x, y, w2, h1, 'M', &r_time, q, &TIME[0]);

  x = x0 + 5;
  y = y + 2 * h1;

  HOLD =
  obj = add_normal_button(x, y, w3, h2, "HOLD", 'N');
    fl_set_object_callback(obj, hold_wait_callback, 0);
    set_normal_button(obj, 2);
  x = x + w3;
  RESUME =
  obj = add_normal_button(x, y, w3, h2, "RESUME", 'N');
    fl_set_object_callback(obj, resume_wait_callback, 0);
    set_normal_button(obj, 2);
  x = x + w3;
  CANCEL =
  obj = add_normal_button(x, y, w3, h2, "CANCEL", 'N');
    fl_set_object_callback(obj, cancel_wait_callback, 0);
    set_normal_button(obj, 2);
  x = x + w3;

  update_wait_panel();
}

//~----------------------------------------------------------------------------

void wait_callback(int tid, void *data) {

  _r *r = &r_time;

  err = 0x0000;
  
  r->sta = status_NA;
  if (!wait_on_hold) r->rea[0] = MAX(r->rea[0]-1, 0);
  r->rea[1] = r->rea[1] + 1;

  if (r->rea[0]) {
    wait_tid = fl_add_timeout(1000, wait_callback, NULL);
  }
  else {
    r->sta = status_OK;
    waiting = FALSE;
    set_normal_button(HOLD, 2);
    set_normal_button(RESUME, 2);
    set_normal_button(CANCEL, 2);
    sequence_callback(NULL, (int32)"EXEC_COMPLETE");
  }

  update_wait_panel();
}

//~----------------------------------------------------------------------------

void hold_wait_callback(FL_OBJECT* obj, long data) {

  wait_on_hold = TRUE;
  set_normal_button(HOLD, 2);
  set_normal_button(RESUME, 0);
  update_wait_panel();
}

//~----------------------------------------------------------------------------

void resume_wait_callback(FL_OBJECT* obj, long data) {

  wait_on_hold = FALSE;
  set_normal_button(HOLD, 0);
  set_normal_button(RESUME, 2);
  update_wait_panel();
}

//~----------------------------------------------------------------------------

void cancel_wait_callback(FL_OBJECT* obj, long data) {

  _r *r = &r_time;

  wait_on_hold = FALSE;
  waiting = FALSE;
  fl_remove_timeout(wait_tid);
  err = 0x0200;
  r->sta = status_KO;
  update_wait_panel();
  set_normal_button(HOLD, 2);
  set_normal_button(RESUME, 2);
  set_normal_button(CANCEL, 2);
  sequence_callback(NULL, (int32)"EXEC_COMPLETE");
}

//~----------------------------------------------------------------------------

void update_wait_panel(void) {

  int i, t[2];
  char b[80];
  _r *r = &r_time;

  for (i=0; i<2; i++) {
    t[i] = r->was[i] = r->rea[i];
    sprintf(b, "%02d:%02d:%02d", t[i]/60/60, t[i]/60%60, t[i]%60%60);
    show_dat_box_string(1, NULL, b, &TIME[i]);
    if (wait_on_hold) {
      DAT_BOX *B = TIME[i];
      fl_set_object_color(B->obj, FL_RED, 0);
      fl_set_object_lcolor(B->obj, FL_YELLOW);
    }
  }
}

//~============================================================================
//
//            DIRECTORY PANEL
//
//~============================================================================

void create_directory_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 350, h1 = 20, h2 = 425, h3 = 25;

  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + h3 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  SEQ_DIR_NAM = add_data_box(x, y, w1, h1, "", 'N');
  y = y + h1;
  
  SEQ_DIR_BRO =
  obj = fl_add_browser(FL_HOLD_BROWSER, x, y, w1, h2, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
    fl_set_browser_dblclick_callback(obj, directory_callback, 0);
  y = y + h2;
  
  obj = add_normal_button(x, y, w1, h3, 
    "RELOAD  SEQUENCE  LIBRARY  DEFAULT  DIRECTORY", 'N');
    fl_set_object_callback(obj, directory_callback, 0);
  y = y + h3;
}

//~----------------------------------------------------------------------------

void directory_callback(FL_OBJECT* ob, long data) {

  FL_OBJECT *obj = SEQ_DIR_BRO;

  if (ob == obj) {
    int k = fl_get_browser(obj);
    char name[512];
    const char *p = fl_get_browser_line(obj, k);
    if (p && strcmp(p, ".") && strcmp(p, "..")) {
      sprintf(name, "%s/%s", cur_dir_name, p);
      if (opendir(name)) {
        strcpy(cur_dir_name, name);
      }
      else {
        strcpy(seq_fil_name, name);
        sequence_callback(NULL, (int32)"RELOAD");
      }
    }
  }
  else {
    strcpy(cur_dir_name, def_dir_name);
  }

  update_objects();
}

//~----------------------------------------------------------------------------

void update_directory_panel(void) {

  FL_OBJECT *obj = SEQ_DIR_BRO;
  DIR *dir;
  struct dirent *d;

  fl_set_object_label(SEQ_DIR_NAM, cur_dir_name);
  fl_set_object_color(SEQ_DIR_NAM, FL_WHITE, 0);

  fl_freeze_form(MAIN);
  fl_clear_browser(obj);
  
  dir = opendir(cur_dir_name);
  while ((d = readdir(dir))) {
    char *p = d->d_name;
    if (strcmp(p, ".") && strcmp(p, "..")) fl_add_browser_line(obj, p);
  }
  closedir(dir);

  fl_set_object_color(obj, executing_command?FL_TOP_BCOL:FL_WHITE, FL_YELLOW);

  if (executing_command) fl_deactivate_object(obj);
  else                   fl_activate_object(obj);

  fl_unfreeze_form(MAIN);
}

//~============================================================================
//
//            SEQUENCE PANEL
//
//~============================================================================

void create_sequence_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 700, w2 = 70, h1 = 20, h2 = 810, h3 = 25;
  static struct {
    char       *txt;
    FL_OBJECT **obj;
  } btn[] = {
    {"RELOAD", &RELOAD},
    {"EXEC",   &EXEC},
    {"SKIP",   &SKIP},
    {"BACK",   &BACK},
    {"",        NULL},
    {"",        NULL},
    {"ON",     &ON},
    {"OFF",    &OFF},
    {"SIZE",   &SIZE},
    {"FONT",   &FONT}};

  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + h3 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  SEQ_NAM = add_data_box(x, y, w1, h1, "", 'N');
  y = y + h1;

  SEQ_BRO =
  obj = fl_add_browser(FL_HOLD_BROWSER, x, y, w1, h2, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_object_callback(obj, sequence_callback, (int32)"UPDATE");
  y = y + h2;

  for (i=0; i<DIM(btn); i++) {
    FL_OBJECT **o = btn[i].obj;
    char       *t = btn[i].txt;
    obj = add_normal_button(x, y, w2, h3, t, 'N');
      fl_set_object_callback(obj, sequence_callback, (int32)&t[0]);
    if (o) *o = obj;
    x = x + w2;
  }
  y = y + h3;
}

//~----------------------------------------------------------------------------

void auto_exec_callback(int tid, void *data) {

  sequence_callback(NULL, (int32)"EXEC");
}

//~----------------------------------------------------------------------------

void sequence_callback(FL_OBJECT* obj, long data) {

  char *cmd = data ? (char*)data : "RELOAD";
  FL_OBJECT *bro = SEQ_BRO;

//~--

  sel_line = fl_get_browser(bro);
  max_line = fl_get_browser_maxline(bro);

//~--

  if (strcasestr(cmd, "UPDATE")) {
  }

//~--

  if (strcasestr(cmd, "RELOAD")) {
    top_line = 1;
    sel_line = 0;
    reload_sequence_browser();
  }

//~--

  if (strcasestr(cmd, "EXEC")) {
    if (sel_line) {
      if (is_command(bro, sel_line)) {
        if (P > 1) {
          static struct timeval now, then;
          printf("%s\n", cmd);
          gettimeofday(&now, NULL);
          if (strcasestr(cmd, "COMPLETE")) {
            printf("Timeout callback dt = %f\n", delta_t(&now, &then));
          }
          memcpy(&then, &now, sizeof(now));
        }
        if (obj) {
          if (auto_exec) {
            auto_exec = FALSE;
            set_normal_button(EXEC, 2);
            return;
          }
          else {
            if (fl_get_button_numb(EXEC) == FL_RIGHT_MOUSE) {
              auto_exec = TRUE;
              set_normal_button(EXEC, 1);
            }
          }
        }
        if (!strcasestr(cmd, "COMPLETE")) {
          if (!auto_exec) set_normal_button(EXEC, 2);
          if (executing_command) return;
          set_normal_button(SKIP, 2);
          set_normal_button(BACK, 2);
          executing_command = TRUE;
          msg = -1;
          update_objects();
          execute_command(bro, sel_line);
          if (waiting) return;
        }
        executing_command = FALSE;
        msg = 1;
        update_line_tag(bro);
        if (!err) next_command_line(bro, +1);
        else      auto_exec = FALSE;
        if (auto_exec) {
          fl_add_timeout(1000, auto_exec_callback, NULL);
        }
        else {
          set_normal_button(EXEC, 0);
          set_normal_button(SKIP, 0);
          set_normal_button(BACK, 0);
        }
      }
    }
  }
      
//~--

  if (strcasestr(cmd, "ON"))  auto_exec = TRUE;
  if (strcasestr(cmd, "OFF")) auto_exec = FALSE;

  if (strcasestr(cmd, "SKIP")) next_command_line(bro, +1);
  if (strcasestr(cmd, "BACK")) next_command_line(bro, -1);
  if (strcasestr(cmd, "SIZE")) bro_font_size = 1 - bro_font_size;
  if (strcasestr(cmd, "FONT")) bro_font_font = 1 - bro_font_font;

//~--

  update_objects();
}

//~----------------------------------------------------------------------------

void update_sequence_panel(void) {

  FL_OBJECT *obj = SEQ_BRO;
  static bool first = TRUE;
  
  if (first) {
    reload_sequence_browser();
    first = FALSE;
  }
  
  fl_freeze_form(MAIN);

  if (bro_font_size) fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
  else               fl_set_browser_fontsize(obj, FL_SMALL_SIZE);
  if (bro_font_font) fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
  else               fl_set_browser_fontstyle(obj, FL_FIXED_STYLE);

  max_line = fl_get_browser_maxline(obj);
  bro_size = fl_get_browser_screenlines(obj);
  top_line = MIN(MAX(sel_line-bro_size/2, 1), max_line);
  fl_set_browser_topline(obj, top_line);

  if (sel_line) {
    int color;
    if      (executing_command)         color = FL_RED;
    else if (is_command(obj, sel_line)) color = FL_CHARTREUSE;
    else                                color = FL_YELLOW;
    fl_set_object_color(obj, executing_command?FL_TOP_BCOL:FL_WHITE, color);
    fl_select_browser_line(obj, sel_line);
  }

  if (executing_command) fl_deactivate_object(obj);
  else                   fl_activate_object(obj);

  fl_unfreeze_form(MAIN);
}

//~============================================================================
//
//            ESSENTIAL STATUS PANEL
//
//~============================================================================

void create_essential_status_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 370, h1 = 830, h2 = 25;

  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  ESS_STA_BRO =
  obj = fl_add_browser(FL_MULTI_BROWSER, x, y, w1, h1, "");
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHITE, FL_YELLOW);
    fl_set_browser_fontstyle(obj, FL_FIXEDBOLD_STYLE);
//  fl_set_browser_fontstyle(obj, FL_FIXED_STYLE);
    fl_set_browser_fontsize(obj, FL_NORMAL_SIZE);
//  fl_set_browser_fontsize(obj, FL_SMALL_SIZE);
  y = y + h1;
  
  obj = add_normal_button(x, y, w1, h2, "GET  ESSENTIAL  STATUS", 'N');
    fl_set_object_callback(obj, get_essential_status_callback, 0);
  y = y + h2;
}

//~-------------------------------------------------------------------------------

void get_essential_status_callback(FL_OBJECT* obj, long data) {

  int i;
  _r *r = &r_ess_sta;
  int8 sta[100];
 
  if (obj) {
    msg = -1;
    update_objects();
  }

  ess_sta_updated = 1;
  
  err = 0x0000;
  
  CAB_get_essential_status(sta, &err);

  if (!err) {
    for (i=0; i<100; i++) r->rea[i] = sta[i];
    ess_sta_unk = r->unk;
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

void update_essential_status_panel(void) {

  FL_OBJECT *obj = ESS_STA_BRO;
  int i, n;
  int8 sta[100];
  char b_rea[100][80], b_was[100][80];
  _r *r = &r_ess_sta;

  if (ess_sta_updated < 0) return;

  if (!ess_sta_unk) {
    for (i=0; i<fl_get_browser_maxline(obj); i++) {
      strcpy(b_was[i], fl_get_browser_line(obj, i+1));
    }
  }

  for (i=0; i<100; i++) sta[i] = r->rea[i];

  sprintf(b_rea[0], "@cCAB Essential TM Status");
  sprint_CAB_essential_status(sta, &n, &b_rea[1]);

  fl_freeze_form(MAIN);
  fl_clear_browser(obj);

  for (i=0; i<n+1; i++) {
    fl_add_browser_line(obj, b_rea[i]);
    if (!ess_sta_unk && strcmp(b_rea[i], b_was[i])) {
      fl_select_browser_line(obj, i+1);
    }
    if (r->unk) break;
  }

  fl_unfreeze_form(MAIN);
  ess_sta_updated = -1;
}

//~============================================================================
//
//            CAB ACKNOLEDGMENT PANEL
//
//~============================================================================

void create_CAB_acknoledgment_panel(FL_FORM *form) {

  FL_OBJECT *obj;
  int x0, y0, x, y, w, h;
  int w1 = 35, w2 = 290, w3, h1 = 20;

  x0 = form->x;
  y0 = form->y + form->h - 30 - 5 - h1 - 5;
  w3 = form->w - 5 - w1 - w2 - 5;

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "CAB:", 'S');
  x = x + w1;
  
  CAB_ACK[0] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w2, h1, "");
    fl_set_object_color(obj, FL_WHITE, FL_MCOL);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
  x = x + w2;

  CAB_ACK[1] = 
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w3, h1, "");
    fl_set_object_color(obj, FL_WHITE, FL_MCOL);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
}

//~----------------------------------------------------------------------------

void update_CAB_acknoledgment_panel(int16 *err) {

  char b[2][80];
  
  sprintf(b[0], " ");
  sprintf(b[1], " ");
  if (msg == 1) {
    sprintf(b[0], "%s... ", cab_cmd_name(CAB_CMD_ID));
    if (*err > 0x2000) sprintf(b[1], "%s", cab_ack_message(*err-0x2000));
    else {
      if (*err) sprintf(b[1], "Command did not reach CAB");
      else      sprintf(b[1], "%s", cab_ack_message(0x00));
    }
  }
  fl_set_object_label(CAB_ACK[0], b[0]);
  fl_set_object_label(CAB_ACK[1], b[1]);
}

//~============================================================================

void update_objects(void) {

  update_template_objects(&msg, &err, NULL, &P);

  update_directory_panel();
  update_sequence_panel();
  update_essential_status_panel();
  update_CAB_acknoledgment_panel(&err);

  write_dat_config_file(dat_config_file_name);
  
  fl_check_forms();
}

//~============================================================================

int main(int argc, char *argv[]) {

  template(argc, &argv[0]);
  
  return 0;
}

//~============================================================================
