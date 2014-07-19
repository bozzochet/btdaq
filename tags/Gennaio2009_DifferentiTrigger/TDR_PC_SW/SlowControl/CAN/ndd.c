// file ndd.c
//
// ndd (TRD Gas Analyzer) "Pro" Controller - A.Lebedev Aug-2004...

#include "uscmlib.h"
#include "xformslib.h"

//~----------------------------------------------------------------------------

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x00;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char *config_file_name = {"ndd.configfile"};       // global variable

int P = 0;                                         // global variable

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define TIMER_PERIOD 2.0     // in seconds

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

typedef struct {
  FL_FORM *ndd;
  void *vdata;
  long ldata;
} FD_ndd;

FD_ndd *ndd;

void P_callback(FL_OBJECT *obj, long data);
void update_P_panel(void);

int port = 1;

FL_OBJECT *PORT_BOX;

FL_OBJECT *MESSAGE;
int msg;
int16 sta;

#define NCMD 16
int8       cmd[NCMD][4];
FL_OBJECT *CMD[NCMD][4];

char *ndd_dat_config_file_name = {"ndd-dat.configfile"};

FL_OBJECT *BROWSER_1;
FL_OBJECT *BROWSER_2;

int nn;
int8 dat[512];

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FD_ndd *create_form_ndd(void);

void invalidate_data(void);
void read_dat_config_file(void);
void write_dat_config_file(void);

void create_port_panel(int x0, int y0);
void port_callback(FL_OBJECT *obj, long data);
void update_port_panel(void);

void INIT_callback(FL_OBJECT *obj, long data);

void create_P_panel(int x0, int y0);
void P_callback(FL_OBJECT *obj, long data);
void update_P_panel(void);

void create_WRITE_panel(int x0, int y0);
void CMD_callback(FL_OBJECT *obj, long data);
void SEND_callback(FL_OBJECT *obj, long data);
void update_WRITE_panel(void);

void create_READ_panel(int x0, int y0);
void CLEAR_callback(FL_OBJECT *obj, long data);
void FLUSH_callback(FL_OBJECT *obj, long data);
void READ_callback(FL_OBJECT *obj, long data);
void update_READ_panel(void);

void TIMER_callback(FL_OBJECT *obj, long data);

void update_message(void);

void update_objects(void);
void start_ndd(void);

//~============================================================================

FD_ndd *create_form_ndd(void) {

  FL_OBJECT *obj;
  
  FD_ndd *fdui = (FD_ndd *) fl_calloc(1, sizeof(*fdui));

  fdui->ndd = fl_bgn_form(FL_NO_BOX,455,225+NCMD*25);

  obj = fl_add_box(FL_FLAT_BOX,0,0,455,225+NCMD*25,"A.Lebedev 26-Oct-08");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);

  obj = fl_add_box(FL_NO_BOX,120,5,330,45,"ndd  Pro  Controller");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_EMBOSSED_STYLE);

  create_USCM_JMDC_panels();
  create_port_panel(  5, 50);
  obj = add_normal_button(50, 50, 370, 50, "INITIALIZE USCM", FL_LARGE_SIZE);
    fl_set_object_callback(obj, INIT_callback, 0);
  create_P_panel(   420,  50);
  create_WRITE_panel( 5, 100);
  create_READ_panel(180, 100);

  msg = 0;
  MESSAGE =
  obj = fl_add_text(FL_NORMAL_TEXT,5,185+NCMD*25,445,35,"");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  obj = fl_add_timer(FL_HIDDEN_TIMER, 0, 0, 0, 0,"");
    fl_set_timer(obj, TIMER_PERIOD);
    fl_set_object_callback(obj, TIMER_callback, 0);

  fl_end_form();
  fdui->ndd->fdui = fdui;

  invalidate_data();
  read_dat_config_file();
 
  return fdui;
}

//~-------------------------------------------------------------------------------

void invalidate_data(void) {

  int j;
  
  for (j=0; j<NCMD; j++) {
    cmd[j][0] = 0x02;
    cmd[j][1] = 'M';
    cmd[j][2] = '0';
    cmd[j][3] = '0';
  }
}

//~-------------------------------------------------------------------------------

void read_dat_config_file(void) {

  FILE *file;

  file = fopen(ndd_dat_config_file_name, "r");
  if (file != NULL) {
    fread(cmd, sizeof(cmd), 1, file);
    fclose(file);
  }
}

//~-------------------------------------------------------------------------------

void write_dat_config_file(void) {

  FILE *file;

  file = fopen(ndd_dat_config_file_name, "w");
  if (file != NULL) {
    fwrite(cmd, sizeof(cmd), 1, file);
    fclose(file);
  }
}

//~============================================================================

void create_port_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w = 35, h = 20;
  
  x = x0;
  y = y0;
  
  obj = fl_add_box(FL_DOWN_BOX,x,y,w+10,2*h+10,"");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w, h, "Port", FL_NORMAL_SIZE);

  y = y + h;
  PORT_BOX = 
  obj = add_switch_button(x, y, w, h, "?", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, port_callback, 0);
}

//~-------------------------------------------------------------------------------

void port_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  port = MOD(port+1, 2);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) port = MOD(port-1, 2);

  update_objects();
}

//~-------------------------------------------------------------------------------

void update_port_panel(void) {

  FL_OBJECT *obj = PORT_BOX;
  char b[80];
  
  if (!obj) return;

  sprintf(b, "%d", port);
  fl_set_object_label(obj, b);
}

//~============================================================================

void INIT_callback(FL_OBJECT *obj, long data) {

  int16 err, error = 0x0000;
  int8 dat[256];
  int i;

  msg = -1;
  update_objects();

  boot_from_FLASH(&err);
  for (i=0; i<3; i++) {
    get_short_status(dat, &err);
    if (!err) break;
  }
  error |= err;

  enable_serial_port(port, &err);
  error |= err;

  if (error) sta = 0;
  else       sta = 1;

  msg = 1;
  update_objects();
}

//~============================================================================

void create_WRITE_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w, h, size;
  int w1 = 40, w2 = 45;
  int ha = 25, h1 = 25;
  
  w = 5 + 3 * w1 + w2 + 5;
  h = 5 + 3 * ha + NCMD * h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, "");

  size = FL_NORMAL_SIZE;
    
  x = x0 + 5;
  y = y0 + 5;
  w = 3 * w1 + w2;
  obj = add_frame_box(x, y, w, ha, "Transmitting", size);
  x = x0 + 5;
  y = y + ha;
  obj = add_frame_box(x, y,    w1, 2*ha, "Cmd", size);
  x = x + w1;
  w = 2 * w2;
  obj = add_frame_box(x, y,     w,   ha, "Data", size);
  obj = add_frame_box(x, y+ha, w1,   ha, "MSB", size);
  x = x + w1;
  obj = add_frame_box(x, y+ha, w1,   ha, "MSB", size);
  x = x + w1;
  obj = add_frame_box(x, y,    w2, 2*ha, "Exec", size);
  y = y + 2 * ha;

  for (j=0; j<NCMD; j++) {
    x = x0 + 5;
    for (i=1; i<4; i++) {
      CMD[j][i] =
      obj = add_switch_button(x, y, w1, h1, "a", FL_MEDIUM_SIZE);
        fl_set_object_callback(obj, CMD_callback, j*NCMD+i);
      x = x + w1;
    }
    obj = add_normal_button(x, y, w2, h1, "SEND", FL_MEDIUM_SIZE);
      fl_set_object_callback(obj, SEND_callback, j);
    y = y + h1;
  }
}

//~-------------------------------------------------------------------------------

void CMD_callback(FL_OBJECT *obj, long data) {

  int ind = data / NCMD;
  int i   = data % NCMD;
  int16 k, kmax;

  if (i == 1) {
    k = cmd[ind][i] - 'M';
    kmax = 10;
  }
  else {
    k = cmd[ind][i];
    if (k > '9') k = k + 10 - 'A';
    else         k = k      - '0';
    kmax = 16;
  }
  
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  k = MOD(k+1, kmax);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) k = MOD(k-1, kmax);

  if (i == 1) {
    cmd[ind][i] = k + 'M';
  }
  else {
    if (k > 9) cmd[ind][i] = k - 10 + 'A';
    else       cmd[ind][i] = k      + '0';
  }
  
  update_objects();
}

//~-------------------------------------------------------------------------------

void SEND_callback(FL_OBJECT *obj, long data) {

  int ind = data;
  int16 err;

  msg = -1;
  update_objects();

  write_serial_port(port, 4, cmd[ind], &err);
  if (err) sta = 0;
  else     sta = 1;
  
  write_dat_config_file();

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_WRITE_panel(void) {

  int i, j;
  
  for (j=0; j<NCMD; j++) {
    for (i=1; i<4; i++) {
      FL_OBJECT *obj = CMD[j][i];
      char b[80];
      sprintf(b, "%c", cmd[j][i]);
      fl_set_object_label(obj, b);
    }
  }
}

//~===============================================================================

void create_READ_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h, size;
  int w1 = 145, w2 = 115, w3 = 60;
  int h1 = 450, h2 = 450, h3 = 25;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h3 + h1 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, h, "");

  size = FL_NORMAL_SIZE;

  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w3, h3, "FLUSH", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, FLUSH_callback, 0);
  x = x + w3;
  w = w1 + w2 - 2 * w3;
  obj = add_frame_box(x, y, w, h3, "Receiving", size);
  x = x + w;
  obj = add_normal_button(x, y, w3, h3, "READ", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, READ_callback, 0);
  x = x0 + 5;
  y = y + h3;
  BROWSER_1 =
  obj = fl_add_browser(FL_NORMAL_BROWSER,x,y,w1,h1,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHITE,FL_YELLOW);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    fl_set_object_lstyle(obj,FL_FIXEDBOLDITALIC_STYLE);
  x = x + w1;
  BROWSER_2 =
  obj = fl_add_browser(FL_NORMAL_BROWSER,x,y,w2,h2,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHITE,FL_YELLOW);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    fl_set_object_lstyle(obj,FL_FIXEDBOLDITALIC_STYLE);
}

//~-------------------------------------------------------------------------------

void FLUSH_callback(FL_OBJECT *obj, long data) {

  int8 stop[4] = {0x02, 'M', '0', '0'};
  int16 err, error = 0x0000;

  msg = -1;
  update_objects();

  write_serial_port(port, 4, stop, &err);
  error |= err;
  
  flush_serial_port(port, &err);
  error |= err;

  if (error) sta = 0;
  else       sta = 1;

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void READ_callback(FL_OBJECT *obj, long data) {

  int8 start[4] = {0x02, 'M', '0', '1'};
  int8 stop[4]  = {0x02, 'M', '0', '0'};
  int i, n;
  int16 err, error = 0x0000;

  msg = -1;
  update_objects();

  write_serial_port(port, 4, start, &err);
  error |= err;
  usleep(100000);
//  sleep(1);
  write_serial_port(port, 4, stop, &err);
  error |= err;

  nn = 0;
  for (i=0; i<256; i=i+n) {
    read_serial_port(port, 256-nn, &n, &dat[i], &err);
    error |= err;
    if (n == 0) break;
    nn = nn + n;
  }
  if (error) sta = 0;
  else       sta = 1;

  for (i=0; i<nn; i++) {
    if (!(i % 7)) printf("\n");
    printf("%02X ", dat[i]);
  }
  printf("\n");

  for (i=0; i<nn; i++) {
    if ((i % 7) == 0) printf("\n");
    if ((i % 7) == 0) printf("%02XH ", dat[i]);
    else              printf("%c ", dat[i]);
  }
  printf("\n");

  msg = 1;
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_READ_panel(void) {

  FL_OBJECT *obj;
  int i;
  char b[80];

  obj = BROWSER_1;
  fl_freeze_form(ndd->ndd);
  fl_clear_browser(obj);
  for (i=0; i<nn; i=i+7) {
    sprintf(b, "%02X %02X %02X %02X %02X %02X %02X ",
            dat[i+0], dat[i+1], dat[i+2], dat[i+3],
            dat[i+4], dat[i+5], dat[i+6]);
    fl_add_browser_line(obj, b);
  }
  fl_unfreeze_form(ndd->ndd);

  obj = BROWSER_2;
  fl_freeze_form(ndd->ndd);
  fl_clear_browser(obj);
  for (i=0; i<nn; i=i+7) {
    sprintf(b, "%02XH %c %c %c %c %c %c",
            dat[i+0], dat[i+1], dat[i+2], dat[i+3],
            dat[i+4], dat[i+5], dat[i+6]);
    fl_add_browser_line(obj, b);
  }
  fl_unfreeze_form(ndd->ndd);
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
  update_port_panel();
  update_P_panel();
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

void start_ndd(void) {

  int one = 1;
  static char *title = {"ndd Monitor"};
  char *adr[] = {(char*)&title};

  if (!initialize_EPP()) exit(1);

  fl_initialize(&one, adr, " ", 0, 0);
  ndd = create_form_ndd();
  fl_set_form_position(ndd->ndd, 0, 0);
  fl_show_form(ndd->ndd, FL_PLACE_GEOMETRY, FL_FULLBORDER, "ndd");

  update_objects();
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);	// set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<Debug Printout Level>]\n", argv[0]);

  if (argc > 1) P = atoi(argv[1]);
  printf("P = %d\n", P);

  start_ndd();

  while(1) {
    fl_do_forms();
  }
  return 0;
}

//~============================================================================
