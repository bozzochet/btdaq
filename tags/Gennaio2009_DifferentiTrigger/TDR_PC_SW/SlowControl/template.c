// file template.c
//
// template objects for my graphical commanding programs
//
// A.Lebedev Apr-2005...

#include "template.h"
#ifdef USE_CAN_OR_AMSW
#include "nodedef.h"
#endif

extern void create_main_form(void);

static char servername[80];
static FL_OBJECT *FORM_BOX;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USE_CAN_OR_AMSW // USE_CAN_OR_AMSW

static _ww w_node;

static char *hex[16] = {
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

static void read_adr_config_file(void);
static void write_adr_config_file(void);

static void create_node_address_panel(int x0, int y0, int *x1, int *y1);
static void node_address_callback(int32 data);
static void update_node_address_panel(void);

static void create_message_panel(int x0, int y0, int w, int h);
static void update_message_panel(int *msg, int16 *err, bool *chk);

static void create__P__panel(int x0, int y0);
static void P__callback(int32 data);

static void create__timeout__panel(int x0, int y0);
static void hide_show__timeout__callback(FL_OBJECT *obj, long data);
static void timeout__callback(int32 data);
static void update__timeout__panel(void);

#endif                 // USE_CAN_OR_AMSW

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USE_CAN         // USE_CAN

static void create_destination_panel(int x0, int y0, int *x1, int *y1);
static void create_USCM_address_panel(int x0, int y0, int *x1, int *y1);
static void update_USCM_address_panel(void);
static void USCM_address_callback(int32 data);
static void create_USCM_boot_flash_button(int x0, int y0, int *x1, int *y1);

static void update_USCM_control_panel(void);
static void invalidate_USCM_status_data(void);

static void USCM_control_callback(FL_OBJECT *obj, long data);
static void USCM_status_callback(FL_OBJECT *obj, long data);
#endif                 // USE_CAN

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USE_AMSW        // USE_AMSW

static void create_destination_panel(int x0, int y0, int *x1, int *y1, char *pathname);

static void create_AMSW_address_panel(int x0, int y0, int *x1, int *y1, char *pathname);
static void AMSW_port_callback(int32 data);
static void JINF_path_callback(int32 data);
static void reset_JINF_path_callback(FL_OBJECT* obj, long data);
static void update_AMSW_address_panel(void);

#endif                 // USE_AMSW

//~============================================================================
//
//            STANDARD TEMPLATE TO USE XFORMS
//
//~============================================================================

void template(int argc, char **argv) {

  int one = 1;

  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  
  progname = argv[0];
  servername[0] = '\0';
  
#ifdef USE_AMSW
  use_MEM     = FALSE;
  use_CAN     = FALSE;
  use_AMSW    = TRUE;
  use_PCIAMSW = FALSE;
  use_TCP     = FALSE;
  use_EAS     = FALSE;

  printf("Usage: \n");
  printf("  %s [{EAS:1553|EAS:HRDL|EAS:ECHO|1553|HRDL } <Server>]\n", progname);

  if (argc > 1) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    if (FALSE) {
    }
    else if (!strcasecmp(argv[1], "1553")) {
      CS_port       = 61002;
      use_APID      = 0x0018;
      use_AMSW      = FALSE;
      use_TCP       = TRUE;
      use_Peter_TCP = TRUE;
      printf("1553 Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("APID        = %d(0x%X)\n", use_APID, use_APID);
      printf("Peter's format will be used\n");
      sprintf(servername, "via 1553@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "HRDL")) {
      CS_port       = 61001;
      use_APID      = 0x03D6;
      use_AMSW      = FALSE;
      use_TCP       = TRUE;
      use_Peter_TCP = TRUE;
      printf("HRDL Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("APID        = %d(0x%X)\n", use_APID, use_APID);
      printf("Peter's format will be used\n");
      sprintf(servername, "via HRDL@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "EAS:1553")) {
      CS_port       = 61002;
      use_APID      = 0x03D6;
      use_AMSW      = FALSE;
      use_Peter_TCP = FALSE;
      use_EAS       = TRUE;
      printf("1553 Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("APID        = %d(0x%X)\n", use_APID, use_APID);
      printf("New Peter's eassserver will be used\n");
      sprintf(servername, "via EAS:1553@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "EAS:HRDL")) {
      CS_port       = 61001;
      use_APID      = 0x03D6;
      use_AMSW      = FALSE;
      use_Peter_TCP = FALSE;
      use_EAS       = TRUE;
      printf("HRDL Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("APID        = %d(0x%X)\n", use_APID, use_APID);
      printf("New Peter's eassserver will be used\n");
      sprintf(servername, "via EAS:HRDL@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "EAS:ECHO")) {
      CS_port       = 61009;
      use_APID      = 0x03D6;
      use_AMSW      = FALSE;
      use_Peter_TCP = FALSE;
      use_EAS       = TRUE;
      printf("ECHO Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("APID        = %d(0x%X)\n", use_APID, use_APID);
      printf("New Peter's eassserver will be used\n");
      sprintf(servername, "to EAS:ECHO@%s:%d", CS_address, CS_port);
    }
    else {
      PANIC("First argument is invalid.\n");
    }
  }
#endif // USE_AMSW

#ifdef USE_CAN
  use_MEM     = FALSE;
  use_CAN     = TRUE;
  use_AMSW    = FALSE;
  use_PCIAMSW = FALSE;
  use_TCP     = FALSE;
  use_EAS     = FALSE;

  printf("Usage: \n");
  printf("  %s [{EAS:1553|EAS:HRDL|EAS:CAN|EAS:ECHO|1553|HRDL|CAN} <Server>]\n", progname);

  if (argc > 1) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    if (FALSE) {
    }
    else if (!strcasecmp(argv[1], "CAN")) {
      CS_port       = CAN_SERVER_PORT;
      use_CAN       = FALSE;
      use_TCP       = TRUE;
      use_Peter_TCP = FALSE;
      printf("CAN Server = %s\n", CS_address);
      printf("Port       = %d\n", CS_port);
      printf("My format will be used\n");
      sprintf(servername, "via CAN@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "1553")) {
      CS_port       = 61002;
      use_APID      = 0x0018;
      use_CAN       = FALSE;
      use_TCP       = TRUE;
      use_Peter_TCP = TRUE;
      printf("1553 Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("APID        = %d(0x%X)\n", use_APID, use_APID);
      printf("Peter's format will be used\n");
      sprintf(servername, "via 1553@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "HRDL")) {
      CS_port       = 61001;
      use_APID      = 0x03D6;
      use_CAN       = FALSE;
      use_TCP       = TRUE;
      use_Peter_TCP = TRUE;
      printf("HRDL Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("APID        = %d(0x%X)\n", use_APID, use_APID);
      printf("Peter's format will be used\n");
      sprintf(servername, "via HRDL@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "EAS:1553")) {
      CS_port       = 61002;
      use_APID      = 0x03D6;
      use_CAN       = FALSE;
      use_Peter_TCP = FALSE;
      use_EAS       = TRUE;
      printf("1553 Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("New Peter's eassserver will be used\n");
      sprintf(servername, "via EAS:1553@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "EAS:HRDL")) {
      CS_port       = 61001;
      use_APID      = 0x03D6;
      use_CAN       = FALSE;
      use_Peter_TCP = FALSE;
      use_EAS       = TRUE;
      printf("HRDL Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("New Peter's eassserver will be used\n");
      sprintf(servername, "via EAS:HRDL@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "EAS:CAN")) {
      CS_port       = 61005;
      use_APID      = 0x03D6;
      use_CAN       = FALSE;
      use_Peter_TCP = FALSE;
      use_EAS       = TRUE;
      printf("CAN Server = %s\n", CS_address);
      printf("Port       = %d\n", CS_port);
      printf("New Peter's eassserver will be used\n");
      sprintf(servername, "via EAS:CAN@%s:%d", CS_address, CS_port);
    }
    else if (!strcasecmp(argv[1], "EAS:ECHO")) {
      CS_port       = 61009;
      use_APID      = 0x03D6;
      use_CAN       = FALSE;
      use_Peter_TCP = FALSE;
      use_EAS       = TRUE;
      printf("ECHO Server = %s\n", CS_address);
      printf("Port        = %d\n", CS_port);
      printf("New Peter's eassserver will be used\n");
      sprintf(servername, "to EAS:ECHO@%s:%d", CS_address, CS_port);
    }
    else {
      PANIC("First argument is invalid.\n");
    }
  }
#endif // USE_CAN

  sprintf(adr_config_file_name, "%s.configfile",     progname);
  sprintf(dat_config_file_name, "%s-dat.configfile", progname);

  fl_initialize(&one, &argv[0], " ", 0, 0);
  create_main_form();

  while(1) fl_do_forms();
}

//~----------------------------------------------------------------------------

#ifdef USE_CAN
void bgn_form(FL_FORM **fdui, int X0, int Y0, int W, int H) {
#else
#ifdef USE_AMSW
void bgn_form(FL_FORM **fdui, int X0, int Y0, int W, int H, char *pathname) {
#else
void bgn_form(FL_FORM **fdui, int X0, int Y0, int W, int H) {
#endif // USE_AMSW
#endif // USE_CAN

  int X = Y0, Y = Y0;
  FL_OBJECT *obj;
  int x, y, w, h;
  int x1, y1;
  char b[80];
  
  AFTER_ADDRESS_CHANGED_CALL = NULL;
  AFTER_RUN_FLASH_CALL       = NULL;
  SW_BTN_CALLBACK            = NULL;

  if (*fdui) {
    fl_get_winorigin(fl_winget(), &X, &Y);
    fl_hide_form(*fdui);
    fl_free_form(*fdui);
  }

  *fdui = fl_bgn_form(FL_NO_BOX, W, H);

  fl_set_form_position(*fdui, X, Y);

  w = W;
  h = H;
  x = 0;
  y = 0;

  sprintf(b, "A.Lebedev %s", date);
  FORM_BOX =
  obj = fl_add_box(FL_FLAT_BOX, x, y, w, h, b);
    fl_set_object_lsize(obj, FL_NORMAL_SIZE);
    fl_set_object_lalign(obj, FL_ALIGN_TOP_RIGHT | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_TIMESITALIC_STYLE);

  x1 = x;
  y1 = y;

#ifdef USE_CAN
  create_destination_panel(x, y, &x1, &y1);
#endif // USE_CAN
#ifdef USE_AMSW
  create_destination_panel(x, y, &x1, &y1, pathname);
#endif // USE_AMSW

  w = W - x1;
  h = MAX(45, y1 - y);
  x = x1;
  y = 0;
  obj = fl_add_box(FL_NO_BOX, x, y, w, h, title);
    fl_set_object_lsize(obj, FL_HUGE_SIZE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE + FL_EMBOSSED_STYLE);
    fl_set_object_lalign(obj, FL_ALIGN_BOTTOM | FL_ALIGN_INSIDE);

#ifdef USE_CAN_OR_AMSW
  w = W - 40;
  h = 30;
  x = 0;
  y = H - h;
  create_message_panel(x, y, w, h);

  x = W - 40 - 105;
  y = H - h;
  create__timeout__panel(x, y);

  x = W - 40;
  y = H - h;
  create__P__panel(x, y);
#endif // USE_CAN_OR_AMSW
}

//~----------------------------------------------------------------------------

void rob_form(char *name) {

  char b[80];

  sprintf(b, "%s %s", name, date);
  fl_set_object_label(FORM_BOX, b);
}

//~----------------------------------------------------------------------------

void end_form(FL_FORM *fdui) {

  char hostname[80], fullname[80];

  fl_end_form();
  fdui->fdui = fdui;

  gethostname(hostname, sizeof(hostname));
  sprintf(fullname, "%s@%s %s", progname, hostname, servername);
  fl_show_form(fdui, FL_PLACE_GEOMETRY, FL_FULLBORDER, fullname);
}

//~----------------------------------------------------------------------------

void update_template_objects(int *msg, int16 *err, bool *chk, int *P) {

#ifdef USE_CAN_OR_AMSW // USE_CAN_OR_AMSW
  update_message_panel(msg, err, chk);
  update__timeout__panel();
#endif                 // USE_CAN_OR_AMSW
#ifdef USE_CAN

  if (use_CAN) {
    update_USCM_address_panel();
  }
  else if (use_TCP || use_EAS) {
    update_node_address_panel();
  }
  update_USCM_control_panel();
#endif // USE_CAN

#ifdef USE_AMSW
  if (use_AMSW) {
    update_AMSW_address_panel();
  }
  else if (use_TCP || use_EAS) {
    update_node_address_panel();
  }
#endif // USE_AMSW

  fl_check_forms();
}

//~============================================================================

bool go_on(int16 err) {

  if (!err) return TRUE;
  if (use_CAN  && err > 0x0006) return TRUE;
  if (use_AMSW && err > 0x0006) return TRUE;
  
  return FALSE;
}

//~============================================================================
//
//            CAN DESTINATION PANEL
//
//~============================================================================
 
#ifdef USE_CAN

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void create_destination_panel(int x0, int y0, int *x1, int *y1) {
 
  if (use_CAN) {
    create_USCM_address_panel(x0, y0, x1, y1);
  }
  else if (use_TCP || use_EAS) {
    create_node_address_panel(x0, y0, x1, y1);
  }
  if (DO_NOT_CREATE != 0x12345678) {
    create_USCM_boot_flash_button(*x1, y0, x1, y1);
  }
}
 
//~----------------------------------------------------------------------------

static SW_BTN *USCM_ID_BTN[3], *JMDC_ID_BTN, *CAN_TX_BTN, *CAN_RX_BTN;
static _ww w_uscm_id;
static _ww w_jmdc_id;
static _ww w_can_tx;
static _ww w_can_rx;

static char *l_n[2] = {"Log", "N/B"};
static char *a_b[2] = {"A",   "B"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void create_USCM_address_panel(int x0, int y0, int *x1, int *y1) {

  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 35, w3 = 35, w4 = 20;
  int h1 = 15, h2 = 20;
  static bool first = TRUE;

  _b uscm_id[3] = {
  { 0, 0x00F0, 16},
  { 0, 0x000F, 16},
  { 0, 0x0100,  2}};
  _b jmdc_id = {0, 0x0003, 4};  
  _b can_tx  = {0, 0x0001, 2};
  _b can_rx  = {0, 0x0001, 2};
 
  invalidate_ww(first, &w_node, 1, 0x01FF);

  invalidate_ww(first, &w_uscm_id, 1, 0x01FF);
  invalidate_ww(first, &w_jmdc_id, 1, 3);
  invalidate_ww(first, &w_can_tx,  1, 0);
  invalidate_ww(first, &w_can_rx,  1, 0);

  read_adr_config_file();
  first = FALSE;

  x = x0;
  y = y0;
  w = 5 + 2 * w1 + w2 + w3 + 2 * w4 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x, y, w, h, "");

  if (x1) *x1 = x + w;
  if (y1) *y1 = y + h;
  
  x = x0 + 5;
  y = y0 + 5;

  w = 2 * w1 + w2;
  add_frame_box(x, y, w, h1, "USCM", 'S');
  create_btn_hstring(
    2, x, y+h1, w1, h2, hex, 'N', &w_uscm_id, &uscm_id[0], &USCM_ID_BTN[0]);
  x = x + 2 * w1;
  create_btn_hstring(
    1, x, y+h1, w2, h2, l_n, 'N', &w_uscm_id, &uscm_id[2], &USCM_ID_BTN[2]);
  for (i=0; i<3; i++) USCM_ID_BTN[i]->call = USCM_address_callback;
  x = x + w2;

  add_frame_box(x, y, w3, h1, "JMDC", 'S');
  create_btn_hstring(
    1, x, y+h1, w3, h2, hex, 'N', &w_jmdc_id, &jmdc_id,    &JMDC_ID_BTN);
  JMDC_ID_BTN->call = USCM_address_callback;
  x = x + w3;

  add_frame_box(x, y, w4, h1, "Tx", 'S');
  create_btn_hstring(
    1, x, y+h1, w4, h2, a_b, 'N', &w_can_tx,  &can_tx,     &CAN_TX_BTN);
  CAN_TX_BTN->call = USCM_address_callback;
  x = x + w4;

  add_frame_box(x, y, w4, h1, "Rx", 'S');
  create_btn_hstring(
    1, x, y+h1, w4, h2, a_b, 'N', &w_can_rx,  &can_rx,     &CAN_RX_BTN);
  CAN_RX_BTN->call = USCM_address_callback;
  x = x + w4;
}

//~----------------------------------------------------------------------------

static void USCM_address_callback(int32 data) {

  int msg;
  
  USCM_ID         = w_uscm_id.inp[0];
  JMDC_ID         = w_jmdc_id.inp[0];
  BUS_FOR_REQUEST = w_can_tx.inp[0];
  BUS_FOR_REPLY   = w_can_rx.inp[0];

  invalidate_USCM_status_data();
  
  msg = 0;
  update_template_objects(&msg, NULL, NULL, NULL);

  if (AFTER_ADDRESS_CHANGED_CALL) AFTER_ADDRESS_CHANGED_CALL();
}

//~----------------------------------------------------------------------------

static void update_USCM_address_panel(void) {

  int i;
  
  w_uscm_id.inp[0] = USCM_ID;
  w_jmdc_id.inp[0] = JMDC_ID;
  w_can_tx.inp[0]  = BUS_FOR_REQUEST;
  w_can_rx.inp[0]  = BUS_FOR_REPLY;

  show_btn_string(3, NULL, &USCM_ID_BTN[0]);
  for (i=0; i<3; i++) fl_set_object_color(USCM_ID_BTN[i]->obj, FL_WHEAT, 0);

  show_btn_string(1, NULL, &JMDC_ID_BTN);
  fl_set_object_color(JMDC_ID_BTN->obj, FL_WHEAT, 0);

  show_btn_string(1, NULL, &CAN_TX_BTN);
  fl_set_object_color(CAN_TX_BTN->obj, FL_WHEAT, 0);

  show_btn_string(1, NULL, &CAN_RX_BTN);
  fl_set_object_color(CAN_RX_BTN->obj, FL_WHEAT, 0);

  write_adr_config_file();
}

//~----------------------------------------------------------------------------

static void create_USCM_boot_flash_button(int x0, int y0, int *x1, int *y1) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, h1 = 35;
  
  x = x0;
  y = y0;
  w = 5 + w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x, y, w, h, "");
  if (x1) *x1 = x + w;
  if (y1) *y1 = y + h;
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_normal_button(x, y, w1, h1, "RUN\nFlash", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, USCM_boot_callback, 2);
}

//~----------------------------------------------------------------------------

void USCM_boot_callback(FL_OBJECT *obj, long data) {

  int from = data;
  int i;
  int msg;
  int16 err;
  
  msg = -1;
  update_template_objects(&msg, NULL, NULL, NULL);
  
  boot_USCM(from, &err);
  
  if (err) {
    for (i=0; i<3; i++) {
      ping_USCM(0, NULL, NULL, NULL, &err);
      if (!err) break;
    }
  }
  
  invalidate_USCM_status_data();

  msg = 1;
  update_template_objects(&msg, &err, NULL, NULL);

  if (from == 2 && AFTER_RUN_FLASH_CALL) AFTER_RUN_FLASH_CALL();
}

#endif // USE_CAN

//~============================================================================
//
//            AMS-WIRE DESTINATION PANEL
//
//~============================================================================

#ifdef USE_AMSW

static SW_BTN *INTERFACE, *PORT, *PATH_BTN[4];
static _ww w_interface;
static _ww w_port;
static _ww w_jinf_path;

static char *e_p[2] = {"EPP", "PCI"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void create_destination_panel(int x0, int y0, int *x1, int *y1, char *pathname) {

  if (use_AMSW) {
    create_AMSW_address_panel(x0, y0, x1, y1, pathname);
  }
  else if (use_TCP || use_EAS) {
    create_node_address_panel(x0, y0, x1, y1);
  }
}

//~----------------------------------------------------------------------------

static void create_AMSW_address_panel(int x0, int y0, int *x1, int *y1, char *pathname) {

  FL_OBJECT *obj;
  int i;
  int x, y, w, h;
  int w1 = 30, w2 = 30, w3 = 15, h1 = 15, h2 = 20;
  static bool first = TRUE;

  _b interface = {0, 0x0001, 2};
  _b port      = {0, 0x0003, 4};
  _b path_hex_msk[4] = {
  { 0, 0xF000, 16},
  { 0, 0x0F00, 16},
  { 0, 0x00F0, 16},
  { 0, 0x000F, 16}};
  
  invalidate_ww(first, &w_node, 1, 0x01FF);

  invalidate_ww(first, &w_interface, 1, 0x0000);
  invalidate_ww(first, &w_port,      1, 0x0000);
  invalidate_ww(first, &w_jinf_path, 1, 0x0000);

  first = FALSE;
  
  read_adr_config_file();

//~--

  x = x0;
  y = y0;
  w = 5 + w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x, y, w, h, "");
  if (x1) *x1 = x + w;
  if (y1) *y1 = y + h;

  x = x + 5;
  y = y + 5;
  create_btn_hstring(
    1, x, y,    w1, h1+h2, e_p, 'N', &w_interface, &interface, &INTERFACE);
  INTERFACE->call = AMSW_port_callback;
  x = x + w1;

  add_frame_box(x, y, w2, h1, "Port", 'S');
  create_btn_hstring(
    1, x, y+h1, w2, h2,    hex, 'N', &w_port,      &port,      &PORT);
  PORT->call = AMSW_port_callback;
  x = x + w2;

//~--

  if (!pathname) {
    for (i=0; i<4; i++) PATH_BTN[i] = NULL;
    w_jinf_path.inp[0] = 0x0000;
    return;
  }

  x = x + 5;
  y = y0;

  w = 5 + 4 * w3 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x, y, w, h, " ");
  if (x1) *x1 = x + w;
  if (y1) *y1 = y + h;
  
  x = x + 5;
  y = y + 5;
  add_frame_box(x, y, 4*w3, h1, pathname, FL_SMALL_SIZE);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 4*w1, h1, "");
    fl_set_object_callback(obj, reset_JINF_path_callback, 0);
  y = y + h1;

  create_btn_hstring(
    4, x, y, w3, h2, hex, FL_NORMAL_SIZE, &w_jinf_path, &path_hex_msk[0], &PATH_BTN[0]);
  for (i=0; i<4; i++) PATH_BTN[i]->call = JINF_path_callback;
}

//~----------------------------------------------------------------------------

static void AMSW_port_callback(int32 data) {

  int msg;
  
  use_PCIAMSW = w_interface.inp[0];
  RX          = w_port.inp[0];

  TX = RX;
  
  msg = 0;
  update_template_objects(&msg, NULL, NULL, NULL);

  if (AFTER_ADDRESS_CHANGED_CALL) AFTER_ADDRESS_CHANGED_CALL();
}

//~----------------------------------------------------------------------------

static void JINF_path_callback(int32 data) {

  int msg;
  
  JINF_PATH = w_jinf_path.inp[0];

  msg = 0;
  update_template_objects(&msg, NULL, NULL, NULL);
  
  if (AFTER_ADDRESS_CHANGED_CALL) AFTER_ADDRESS_CHANGED_CALL();
}

//~----------------------------------------------------------------------------

static void reset_JINF_path_callback(FL_OBJECT* obj, long data) {

  int msg;

  JINF_PATH = 0x0000;
  
  msg = 0;
  update_template_objects(&msg, NULL, NULL, NULL);
  
  if (AFTER_ADDRESS_CHANGED_CALL) AFTER_ADDRESS_CHANGED_CALL();
}

//~----------------------------------------------------------------------------

static void update_AMSW_address_panel(void) {

  _ww *w = &w_jinf_path;
  bool ok[4] = {TRUE, TRUE, TRUE, TRUE};
  int i;

//~--

  w_interface.inp[0] = use_PCIAMSW;
  w_port.inp[0]      = RX;

  show_btn_string(1, NULL, &INTERFACE);
  fl_set_object_color(INTERFACE->obj, FL_WHEAT, 0);
  show_btn_string(1, NULL, &PORT);
  fl_set_object_color(PORT->obj, FL_WHEAT, 0);

//~--

  if (!PATH_BTN[0]) return;
  
  w->inp[0] = JINF_PATH;
  
  show_btn_string(4, NULL, &PATH_BTN[0]);

  if (w->inp[0]) {
    int16 m;
    m = unpack32(w->inp[0], 0x0000FF00);
    ok[0] = ok[1] = (m < 24) ? TRUE : FALSE;
    m = unpack32(w->inp[0], 0x000000FF);
    ok[2] = ok[3] = (m == 0x04 || m == 0x05 || m == 0x3F) ? TRUE : FALSE;
  }
  
  for (i=0; i<4; i++) {
    SW_BTN *B = PATH_BTN[i];
    if (ok[i]) {
      fl_set_object_color(B->obj, FL_WHEAT, 0);
      fl_set_object_lcolor(B->obj, FL_BLACK);
    }
    else {
      fl_set_object_color(B->obj, FL_RED, 0);
      fl_set_object_lcolor(B->obj, FL_YELLOW);
    }
  }

//~--

  write_adr_config_file();
}

//~----------------------------------------------------------------------------

#endif // USE_AMSW

//~============================================================================
//
//            NODE DESTINATION PANEL
//
//~============================================================================

#ifdef USE_CAN_OR_AMSW

static SW_BTN *NODE_BTN[4];

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void create_node_address_panel(int x0, int y0, int *x1, int *y1) {

  int i;
  int x, y, w, h;
  int w1 = 15, w2 = 85, h1 = 15, h2 = 20;
  static bool first = TRUE;

  _b node_hex_msk[3] = {
  { 0, 0x0100,  2},
  { 0, 0x00F0, 16},
  { 0, 0x000F, 16}};
  
  _b node_nam_msk = {0, 0x01FF, 512};
  
  invalidate_ww(first, &w_node, 1, 0x01FF);

#ifdef USE_AMSW
  invalidate_ww(first, &w_jinf_path, 1, 0x0000);
#endif

#ifdef USE_CAN
  invalidate_ww(first, &w_uscm_id, 1, 0x01FF);
  invalidate_ww(first, &w_jmdc_id, 1, 3);
  invalidate_ww(first, &w_can_tx,  1, 0);
  invalidate_ww(first, &w_can_rx,  1, 0);
#endif

  first = FALSE;

  read_adr_config_file();

  x = x0;
  y = y0;
  w = 5 + 3 * w1 + w2 + 5;
  h = 5 + h1 + h2 + 5;
  fl_add_box(FL_DOWN_BOX, x, y, w, h, "");
  if (x1) *x1 = x + w;
  if (y1) *y1 = y + h;

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, 3*w1, h1, "Node", FL_SMALL_SIZE);
  create_btn_hstring(
    3, x, y+h1, w1, h2, hex,      'N', &w_node, &node_hex_msk[0], &NODE_BTN[0]);
  x = x + 3 * w1;
  add_frame_box(x, y, w2  , h1, "Name", FL_SMALL_SIZE);
  create_btn_hstring(
    1, x, y+h1, w2, h2, NodeName, 'N', &w_node, &node_nam_msk,    &NODE_BTN[3]);
  for (i=0; i<4; i++) NODE_BTN[i]->call = node_address_callback;
}

//~----------------------------------------------------------------------------

static void node_address_callback(int32 data) {

  int msg;
  
  NODE_ADR = w_node.inp[0];
  
  msg = 0;
  update_template_objects(&msg, NULL, NULL, NULL);

  if (AFTER_ADDRESS_CHANGED_CALL) AFTER_ADDRESS_CHANGED_CALL();
}

//~----------------------------------------------------------------------------

static void update_node_address_panel(void) {

  int i;
  _ww *w = &w_node;

  w->inp[0] = NODE_ADR;

  show_btn_string(4, NULL, &NODE_BTN[0]);

  for (i=0; i<4; i++) {
    SW_BTN *B = NODE_BTN[i];
    if (strcmp(fl_get_object_label(B->obj), "-")) {
      fl_set_object_color(B->obj, FL_WHEAT, 0);
      fl_set_object_lcolor(B->obj, FL_BLACK);
    }
    else {
      fl_set_object_color(B->obj, FL_RED, 0);
      fl_set_object_lcolor(B->obj, FL_YELLOW);
    }
  }

  write_adr_config_file();  
}

#endif // USE_CAN_OR_AMSW

//~============================================================================
//
//            READ/WRITE ADDRESS CONFIGURATION FILE
//
//~============================================================================

#ifdef USE_CAN_OR_AMSW // USE_CAN_OR_AMSW

void read_adr_config_file(void) {

  FILE *file;

  file = fopen(adr_config_file_name, "r");
  if (file) {
#ifdef USE_CAN
    fread(&USCM_ID,         sizeof(USCM_ID),         1, file);
    fread(&JMDC_ID,         sizeof(JMDC_ID),         1, file);
    fread(&BUS_FOR_REQUEST, sizeof(BUS_FOR_REQUEST), 1, file);
    fread(&BUS_FOR_REPLY,   sizeof(BUS_FOR_REPLY),   1, file);
    fread(&NODE_ADR,        sizeof(NODE_ADR),        1, file);
#endif // USE_CAN
#ifdef USE_AMSW
    fread(&use_PCIAMSW, sizeof(use_PCIAMSW), 1, file);
    fread(&TX,          sizeof(TX),          1, file);
    fread(&RX,          sizeof(RX),          1, file);
    fread(&JINF_PATH,   sizeof(JINF_PATH),   1, file);
    fread(&NODE_ADR,    sizeof(NODE_ADR),    1, file);
#endif // USE_AMSW
    fclose(file);
  }
}

//~----------------------------------------------------------------------------

void write_adr_config_file(void) {

  FILE *file;

  file = fopen(adr_config_file_name, "w");
  if (file) {
#ifdef USE_CAN
    fwrite(&USCM_ID,         sizeof(USCM_ID),         1, file);
    fwrite(&JMDC_ID,         sizeof(JMDC_ID),         1, file);
    fwrite(&BUS_FOR_REQUEST, sizeof(BUS_FOR_REQUEST), 1, file);
    fwrite(&BUS_FOR_REPLY,   sizeof(BUS_FOR_REPLY),   1, file);
    fwrite(&NODE_ADR,        sizeof(NODE_ADR),        1, file);
#endif // USE_CAN
#ifdef USE_AMSW
    fwrite(&use_PCIAMSW, sizeof(use_PCIAMSW), 1, file);
    fwrite(&TX,          sizeof(TX),          1, file);
    fwrite(&RX,          sizeof(RX),          1, file);
    fwrite(&JINF_PATH,   sizeof(JINF_PATH),   1, file);
    fwrite(&NODE_ADR,    sizeof(NODE_ADR),    1, file);
#endif // USE_AMSW
    fclose(file);
  }
}

#endif                 // USE_CAN_OR_AMSW

//~============================================================================
//
//            MESSAGE PANEL
//
//~============================================================================

#ifdef USE_CAN_OR_AMSW // USE_CAN_OR_AMSW

static FL_OBJECT *MSG, *MSG_NA, *MSG_ON, *MSG_OK, *MSG_KO, *MSG_COM_OK, *MSG_LCR_KO;
static FL_OBJECT *msg1, *msg2;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void create_message_panel(int x0, int y0, int w, int h) {

  int size = FL_HUGE_SIZE;
  
  MSG = 
    add_message_box(
    x0,     y0, w,   h, "",                         size, FL_BLACK,  FL_YELLOW);
  MSG_NA = 
    add_message_box(
    x0,     y0, w,   h, "This button does not work",size, FL_BLACK,  FL_YELLOW);
  MSG_ON = 
    add_message_box(
    x0,     y0, w,   h, "Command is being sent...", size, FL_YELLOW, FL_BLACK);
  MSG_OK =
    add_message_box(
    x0,     y0, w,   h, "Command executed O.K.",    size, FL_CYAN,   FL_BLACK);
  MSG_KO =
    add_message_box(
    x0,     y0, w,   h, "Command execution failed!",size, FL_BLACK,  FL_YELLOW);
  MSG_COM_OK =
#ifdef USE_CAN
    add_message_box(
    x0,     y0, w/2, h, "CAN comm. - O.K.",         size, FL_CYAN,   FL_BLACK);
#endif // USE_CAN
#ifdef USE_AMSW
    add_message_box(
    x0,     y0, w/2, h, "AMSW comm. - O.K.",        size, FL_CYAN,   FL_BLACK);
#endif // USE_AMSW
  MSG_LCR_KO =
    add_message_box(
    x0+w/2, y0, w/2, h, "LeCroy - failed!",         size, FL_BLACK,  FL_YELLOW);

  msg1 = NULL;
  msg2 = NULL;
}

//~----------------------------------------------------------------------------

void set_message_text(char *txt) {

  fl_set_object_label(MSG, txt);
}

//~----------------------------------------------------------------------------

static void update_message_panel(int *msg, int16 *err, bool *chk) {

//printf("show_message: msg1=%08X msg2=%08X msg=%2d err=%04X chk=%d\n",
//       (int32)msg1, (int32)msg2, *msg, *err, *chk);

  if (*msg == -3) return;
  
  if (msg1) fl_hide_object(msg1);
  if (msg2) fl_hide_object(msg2);

  msg1 = NULL;
  msg2 = NULL;
  
  if (! msg) return;
  if (!*msg) return;

  if      (*msg == -1) {
    if (err) *err = 0x0000;
    if (chk) *chk = 0;
    msg1 = MSG_ON;
  }
  else if (*msg == -2) {
    msg1 = MSG_NA;
  }
  else if (*msg == 1) {
    if (*err) {
      msg1 = MSG_KO;
    }
    else {
      if (chk && *chk) {
        msg1 = MSG_COM_OK;
        msg2 = MSG_LCR_KO;
      }
      else {
        msg1 = MSG_OK;
      }
    }
  }
  else if (*msg == -4) {
    msg1 = MSG;
  }

  if (msg1) fl_show_object(msg1);
  if (msg2) fl_show_object(msg2);
}

#endif                 // USE_CAN_OR_AMSW

//~============================================================================
//
//            TIMEOUT PANEL
//
//~============================================================================

#ifdef USE_CAN_OR_AMSW // USE_CAN_OR_AMSW

static SW_BTN    *TIMEOUT_BTN[2];
static FL_OBJECT *TIMEOUT_GRP;
static bool hide_timeout_panel = TRUE;

static char *dec_dot[10] = {
  "0.", "1.", "2.", "3.", "4.", "5.", "6.", "7.", "8.", "9."};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void create__timeout__panel(int x0, int y0) {

  FL_OBJECT *obj;
  SW_BTN *B;
  float timeout;
  int x, y, w, h;
  int w1 = 60, w2 = 20, w3 = 15, h1 = 20;

#ifdef USE_CAN
    if (!get_CAN_timeout(NULL)) set_CAN_timeout(1.0);
    get_CAN_timeout(&timeout);
#endif
#ifdef USE_AMSW
//  if (!get_AMSW_timeout(NULL)) set_AMSW_timeout(9.9);
//  if (!get_AMSW_timeout(NULL)) set_AMSW_timeout(0.9);   // changed for AB
    if (!get_AMSW_timeout(NULL)) set_AMSW_timeout(1.5);   // changed for JINJ
    get_AMSW_timeout(&timeout);
#endif

  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + h1 + 5;
  obj = fl_add_button(FL_HIDDEN_BUTTON, x0, y0, w, h, " ");
    fl_set_object_callback(obj, hide_show__timeout__callback, 0);

  TIMEOUT_GRP = fl_bgn_group();

  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, w1, h1, "Timeout, s", FL_SMALL_SIZE);
  x = x + w1;
  
  TIMEOUT_BTN[0] = 
    B = add_switch_btn(x, y, w2, h1, dec_dot, FL_NORMAL_SIZE, 10, (int)timeout);
    B->call = timeout__callback;
  fl_set_object_lalign(B->obj, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  x = x + w2;

  TIMEOUT_BTN[1] = 
    B = add_switch_btn(x, y, w3, h1, hex, FL_NORMAL_SIZE, 10, (int)(10.0*timeout)%10);
    B->call = timeout__callback;

  fl_end_group();
  
  fl_hide_object(TIMEOUT_GRP);
}

//~----------------------------------------------------------------------------

static void hide_show__timeout__callback(FL_OBJECT *obj, long data) {

  hide_timeout_panel = !hide_timeout_panel;
  update__timeout__panel();
}

//~----------------------------------------------------------------------------

static void timeout__callback(int32 data) {

  float timeout;
  timeout = (TIMEOUT_BTN[0]->i * 10 + TIMEOUT_BTN[1]->i) / 10.0;
#ifdef USE_CAN
    set_CAN_timeout(timeout);
#endif
#ifdef USE_AMSW
    set_AMSW_timeout(timeout);
#endif
  if (P > 2) printf("timeout = %3.1fs\n", timeout);
}

//~----------------------------------------------------------------------------

static void update__timeout__panel(void) {

  if (hide_timeout_panel) fl_hide_object(TIMEOUT_GRP);
  else                    fl_show_object(TIMEOUT_GRP);
}

#endif                 // USE_CAN_OR_AMSW

//~============================================================================
//
//            P PANEL
//
//~============================================================================

#ifdef USE_CAN_OR_AMSW // USE_CAN_OR_AMSW

static SW_BTN *P_BTN;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void create__P__panel(int x0, int y0) {

  int x, y, w, h;
  int w1 = 15, h1 = 20;
  
  x = x0;
  y = y0;
  
  w = 5 + 2 * w1 + 5;
  h = 5 + h1 + 5;
  fl_add_box(FL_DOWN_BOX, x, y, w, h, "");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "P", FL_SMALL_SIZE);

  x = x + w1;
  P_BTN = add_switch_btn(x, y, w1, h1, hex, FL_NORMAL_SIZE, 10, P);
  P_BTN->call = P__callback;
}

//~----------------------------------------------------------------------------

static void P__callback(int32 data) {

  P = P_BTN->i;
}

#endif                 // USE_CAN_OR_AMSW

//~XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//
//      STANDARD OBJECT LIBRARY
//
//~XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//~============================================================================
//
//            USCM MONITORING AND CONTROL
//
//~============================================================================

#ifdef USE_CAN

static FL_OBJECT *MOD_BOX = NULL, *VER_BOX = NULL;
static _r uscm_mod, uscm_ver;

static char *month[13] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  "XXX"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_USCM_control_H2_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 35, w3 = 30, w4 = 70, w5 = 15;
  int h1 = 15;
  
  w = 5 + w1 + w2 + w3 + w4 + w5 + 5;
  h = 5 + 2 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1, 2*h1, "USCM\nCtrl", 'S');
  
  x = x + w1;
  obj = add_normal_button(x, y,    w2, h1, "SOFF", 'N');
    fl_set_object_callback(obj, USCM_control_callback, 3);
  obj = add_normal_button(x, y+h1, w2, h1, "SON",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 4);
  x = x + w2;
  obj = add_normal_button(x, y,    w3, h1, "OTP",  'N');
    fl_set_object_callback(obj, USCM_boot_callback, 1);
  obj = add_normal_button(x, y+h1, w3, h1, "SUI",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 5);
  x = x + w3;

  MOD_BOX = add_data_box(x,  y,    w4,    h1, "",  'N');
  VER_BOX = add_data_box(x,  y+h1, w4,    h1, "",  'N');
  x = x + w4;
  obj = add_normal_button(x, y,    w5,  2*h1, "R", 'N');
    fl_set_object_callback(obj, USCM_status_callback, 0);

  invalidate_USCM_status_data();
}

//~----------------------------------------------------------------------------

void create_USCM_control_H3_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 35, w2 = 30, w3 = 70, w4 = 15;
  int h1 = 15;
  
  w = 5 + w1 + w2 + w3 + w4 + 5;
  h = 5 + 3 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w-10, h1, "USCM Control", 'S');
  
  y = y + h1;
  obj = add_normal_button(x, y,    w1, h1, "SOFF", 'N');
    fl_set_object_callback(obj, USCM_control_callback, 3);
  obj = add_normal_button(x, y+h1, w1, h1, "SON",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 4);
  x = x + w1;
  obj = add_normal_button(x, y,    w2, h1, "OTP",  'N');
    fl_set_object_callback(obj, USCM_boot_callback, 1);
  obj = add_normal_button(x, y+h1, w2, h1, "SUI",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 5);
  x = x + w2;

  MOD_BOX = add_data_box(x,  y,    w3,    h1, "",  'N');
  VER_BOX = add_data_box(x,  y+h1, w3,    h1, "",  'N');
  x = x + w3;
  obj = add_normal_button(x, y,    w4,  2*h1, "R", 'N');
    fl_set_object_callback(obj, USCM_status_callback, 0);

  invalidate_USCM_status_data();
}

//~----------------------------------------------------------------------------

void create_USCM_control_H3S_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 45, w2 = 70, w3 = 15;
  int w4 = 35, w5 = 35, w6 = 30, w7 = 30;
  int h1 = 15;
  
  w = 5 + w1 + w2 + w3 + 5;
  h = 5 + 3 * h1 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, 2*h1, "USCM\nCtrl", 'S');
  x = x + w1;
  MOD_BOX = add_data_box(x,  y,    w2,    h1, "", 'N');
  VER_BOX = add_data_box(x,  y+h1, w2,    h1, "", 'N');
  x = x + w2;
  obj = add_normal_button(x, y,    w3,  2*h1, "R", 'N');
    fl_set_object_callback(obj, USCM_status_callback, 0);
  y = y + 2 * h1;

  x = x0 + 5;
  obj = add_normal_button(x, y, w4, h1, "SOFF", 'N');
    fl_set_object_callback(obj, USCM_control_callback, 3);
  x = x + w4;
  obj = add_normal_button(x, y, w5, h1, "SON",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 4);
  x = x + w5;
  obj = add_normal_button(x, y, w6, h1, "SUI",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 5);
  x = x + w6;
  obj = add_normal_button(x, y, w7, h1, "OTP",  'N');
    fl_set_object_callback(obj, USCM_boot_callback, 1);

  invalidate_USCM_status_data();
}

//~----------------------------------------------------------------------------

void create_USCM_control_V_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, h;
  int w1 = 40, w2 = 30, w3 = 35, w4 = 35;
  int h1 = 15, h2 = 15;
  
  w = 5 + w1 + w2 + 5;
  h = 5 + h2 + 2 * h1 + 3 * h2 + 5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, h, " ");

  x = x0 + 5;
  y = y0 + 5;

  add_frame_box(x, y, w1+w2, h2, "USCM Ctrl", 'S');
  
  y = y + h2;
  obj = add_normal_button(x,    y, w1, h1, "SOFF", 'N');
    fl_set_object_callback(obj, USCM_control_callback, 3);
  obj = add_normal_button(x+w1, y, w2, h1, "OTP",  'N');
    fl_set_object_callback(obj, USCM_boot_callback, 1);
  y = y + h1;
  obj = add_normal_button(x,    y, w3, h1, "SON",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 4);
  obj = add_normal_button(x+w3, y, w4, h1, "SUI",  'N');
    fl_set_object_callback(obj, USCM_control_callback, 5);
  y = y + h1;

  MOD_BOX = add_data_box( x, y, w1+w2, h2, "",     'N');
  y = y + h2;
  VER_BOX = add_data_box( x, y, w1+w2, h2, "",     'N');
  y = y + h2;
  obj = add_normal_button(x, y, w1+w2, h2, "READ", 'N');
    fl_set_object_callback(obj, USCM_status_callback, 0);

  invalidate_USCM_status_data();
}

//~----------------------------------------------------------------------------

static void invalidate_USCM_status_data(void) {

  static bool first = TRUE;

  invalidate_r(first, &uscm_mod, 1);
  invalidate_r(first, &uscm_ver, 1);

  first = FALSE;
}

//~----------------------------------------------------------------------------

static void USCM_control_callback(FL_OBJECT *obj, long data) {

  int8 from = data;
  int msg;
  int16 err;
  
  msg = -1;
  update_template_objects(&msg, NULL, NULL, NULL);
  
  boot_USCM(from, &err);
  if (from == 1) {
    if (err) {
      int i;
      for (i=0; i<3; i++) {
        ping_USCM(0, NULL, NULL, NULL, &err);
        if (!err) break;
      }
    }
  }
  
  msg = 1;
  update_template_objects(&msg, &err, NULL, NULL);
}

//~----------------------------------------------------------------------------

static void USCM_status_callback(FL_OBJECT *obj, long data) {

  int8 dat[727];
  int msg;
  int16 err;
  
  msg = -1;
  update_template_objects(&msg, NULL, NULL, NULL);

  err = 0x0000;
  
  *uscm_mod.was = *uscm_mod.rea;
  *uscm_ver.was = *uscm_ver.rea;

  if (!err) {
    get_short_status(dat, &err);
    if (!err) {
      dat[0] = MIN(14, dat[0]);
      if (!dat[0]) *uscm_mod.rea = dat[7] & 0x10 ? 1 : 0;
      else         *uscm_mod.rea = dat[0];
      if (uscm_mod.unk) *uscm_mod.was = *uscm_mod.rea;
      uscm_mod.unk = FALSE;
      uscm_mod.sta = status_OK;
      if (dat[0]) {
        *uscm_ver.rea = 0x00000000;
        pack32(uscm_ver.rea, 0xFF000000, (int32)dat[5]);
        pack32(uscm_ver.rea, 0x00FF0000, (int32)dat[6]);
        pack32(uscm_ver.rea, 0x0000FF00, (int32)dat[7]);
        pack32(uscm_ver.rea, 0x000000FF, (int32)dat[8]);
        if (uscm_ver.unk) *uscm_ver.was = *uscm_ver.rea;
        uscm_ver.unk = FALSE;
        uscm_ver.sta = status_OK;
      }
      else {
        get_long_status(dat, &err);
        if (!err) {
          char b[80];
          int m;
          memcpy(b, &dat[dat[2] >= 0x2A ? 21 : 17], 11);
          for (m=0; m<12; m++) if (!strncmp(month[m], &b[3], 3)) break;
          *uscm_ver.rea = 0x00000000;
          pack32(uscm_ver.rea, 0xF0000000, (int32)(b[10]-'0'));
          pack32(uscm_ver.rea, 0x0F000000, (int32)(m+1));
          pack32(uscm_ver.rea, 0x00F00000, (int32)(b[0]-'0'));
          pack32(uscm_ver.rea, 0x000F0000, (int32)(b[1]-'0'));
          if (uscm_ver.unk) *uscm_ver.was = *uscm_ver.rea;
          uscm_ver.unk = FALSE;
          uscm_ver.sta = status_OK;
        }
        else {
          uscm_ver.sta = status_KO;
        }
      }
    }
    else {
      uscm_mod.sta = status_KO;
      uscm_ver.sta = status_KO;
    }
  }

  msg = 1;
  update_template_objects(&msg, &err, NULL, NULL);
}

//~----------------------------------------------------------------------------

static void update_USCM_control_panel(void) {

  char b[80], *p = b;
  int m;
  char *mode[15] = {
    "HBB_PROM"  ,           //   0   0x00
    "HBB_FLASH" ,           //   1   0x01
    "UNI_PROM"  ,           //   2   0x02
    "UNI_FLASH" ,           //   3   0x03
    "JPD_PROM"  ,           //   4   0x04
    "JPD_FLASH" ,           //   5   0x05
    "PDS_PROM"  ,           //   6   0x06
    "PDS_FLASH" ,           //   7   0x07
    "CC_PROM"   ,           //   8   0x08
    "CC_FLASH"  ,           //   9   0x09
    "UG_PROM"   ,           //  10   0x0A
    "UG_FLASH"  ,           //  11   0x0B
    "M_PROM"    ,           //  12   0x0C
    "M_FLASH"   ,           //  13   0x0D
    "?"};                   //  14   0x0E

  if (!MOD_BOX) return;

  show_num_box(&uscm_mod, 0, mode, NULL, MOD_BOX);
  p += sprintf(p, "%02d-", unpack32(*uscm_ver.rea, 0x00FF0000));
  m = MIN(12, unpack32(*uscm_ver.rea, 0x0F000000) - 1);
  p += sprintf(p, "%s-", month[m]);
  p += sprintf(p, "%02d", unpack32(*uscm_ver.rea, 0xF0000000));
  show_num_box(&uscm_ver, 0, NULL, b,    VER_BOX);
}

#endif // USE_CAN

//~============================================================================
