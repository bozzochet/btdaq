// file boxS.c
//
// modified by AL to remove compiler warnings
//
/*********************************************************************
 *
 * Filename:     boxS.c
 *
 * Author(s):    Francesca Spada
 *
 * Date:         1/7/2004
 *
 *********************************************************************/

/* #include "xformslib.h" */
#include "template.h"
#include "fslib.h"

bool ESPIONAGE;

char *date   = {"07-Jan-08"};
char *author = {"F.R.Spada"}; // FS
char *title  = {"UGBS Controller"};

#ifdef USE_CAN
#include "uscmlib.h"
int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x4E;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int P = 0;                                         // global variable
int DEBUG_LEVEL = 0; // FS

bool use_CAN = TRUE;                               // global variable
bool use_MEM = FALSE;                              // global variable
bool use_UDP = FALSE;                              // global variable
bool use_TCP = FALSE;                              // global variable
bool use_Peter_TCP = FALSE;                        // global variable
bool use_PCIAMSW = FALSE;                          // global variable
#endif

char* config_file_name = {"LeCroy.config"};     // FS
char* hc_config_file_name = {"HotCold.config"}; // FS
FILE* file;                                     // FS
int   HC_ID = 0x01;                             // FS

FL_FORM *MAIN = NULL;

#define status_NN -2
#define status_NA -1
#define status_KO  0
#define status_OK  1

int   n_CMDS  = 1;   // FS
int   clk;           // FS
int   dat;           // FS
int8  bus;           // FS

/* void start_boxs(void); */

FL_OBJECT *yes, *but;


/*********************************************************************
 * Type definition, structures, initializations
 *********************************************************************/

int        msg = 0;
int16      sta;
bool       chk;
FL_OBJECT* MESSAGE;

typedef struct {
  FL_FORM* boxs;
  void* vdata;
  long ldata;
} FD_BOXS;

FD_BOXS*   boxs;


/* Tensions */
#define n_tensions 2

struct {
  char *nam;
  int16 msk;
  struct {
    int16 cod;
    float val;
    int   sta;
  } wri;
  FL_OBJECT *WAS;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
  FL_OBJECT *VAL;
} tension[n_tensions] = {
  {" 5V",      0x8000},  // 1 disabilita
  {"10V",      0x2000},  // 1 disabilita
};

struct {
  struct {
    struct {
      int16 was;
      int16 cod;
      int   sta;
    } wri;
    FL_OBJECT *SET;
    struct {
      int   mon;
      int16 cod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } ena;
} tensions;

int tension_INP_active;


/* Marotta valves */
#define n_marotta 15

struct {
  char *nam;
  int16 msk;
  struct {
    int16 cod;
    float val;
    int   sta;
  } wri;
  FL_OBJECT *WAS;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
  FL_OBJECT *INP;
  FL_OBJECT *VAL;
  FL_OBJECT *OPN;
  FL_OBJECT *ASTA;
  FL_OBJECT *OSTA;
} valve[n_marotta] = {
  {"V1a",       0x8000},
  {"V2a",       0x4000},
  {"V3a",       0x2000},
  {"V4a",       0x1000},
  {"Sp1",       0x0800},
  {"V10a",      0x0400},
  {"V20a",      0x0200},
  {"V1b",       0x0100},
  {"V2b",       0x0080},
  {"V3b",       0x0040},
  {"V4b",       0x0020},
  {"V10b",      0x0010},
  {"V20b",      0x0008},
  {"Sp2",       0x0004},
  {"V20a&V20b", 0x0208},
};

struct {
  struct {
    struct {
      int16 was;
      int16 cod;
      int   sta;
    } wri;
    FL_OBJECT *SET;
    struct {
      int   mon;
      int16 cod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } ena;
  struct {
    struct {
      int   mon;
      int16 acod;
      int16 ocod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } sta;
} valves;

int Mvalve_INP_active;

/* Heaters */
#define n_heaters 5

struct {
  char *nam;
  int16 msk;
  struct {
    int16 cod;
    float val;
    int   sta;
  } wri;
  FL_OBJECT *WAS;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
  FL_OBJECT *INP;
  FL_OBJECT *VAL;
  FL_OBJECT *OPN;
} heater[n_heaters] = {
  {"H1",        0x8000},
  {"H2",        0x4000},
  {"H3",        0x2000},
  {"H4",        0x1000},
  {"H5",        0x0800}
 };

struct {
  struct {
    struct {
      int16 was;
      int16 cod;
      int   sta;
    } wri;
    FL_OBJECT *SET;
    struct {
      int   mon;
      int16 cod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } ena;
  struct {
    struct {
      int   mon;
      int16 acod;
      int16 ocod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } sta;
} heaters;

int heater_INP_active;

/* Gp50 pressure sensors */
#define n_sensor 4

struct {
  char *nam;
  struct {
    int   mon;
    int16 cod;
    float val;
    float psia;
    int   sta;
  } rea;
  FL_OBJECT *VAL;
  FL_OBJECT *PSIA;
  FL_OBJECT *GET;
  FL_OBJECT *CVT;
} sensor[n_sensor] = {
  {"P1a"},
  {"P1b"},
  {"P2a"},
  {"P2b"},
};

struct {
  char *nam;
  struct {
    int   mon;
    int16 cod;
    float val;
    float deg;
    int   sta;
  } rea;
  FL_OBJECT *VAL;
  FL_OBJECT *DEG;
  FL_OBJECT *GET;
} tsensor[n_sensor] = {
  {"T1a"},
  {"T1b"},
  {"T2a"},
  {"T2b"},
};

/* Kulite sensors */
#define n_Ksensor 3

struct {
  char *nam;
  struct {
    int   mon;
    int16 cod;
    float val;
    float bar;
    int   sta;
  } rea;
  FL_OBJECT *VAL;
  FL_OBJECT *BAR;
  FL_OBJECT *GET;
  FL_OBJECT *CVTS;
  FL_OBJECT *CVTR;
} Ksensor[n_Ksensor] = {
  {"Pk1c"},
  {"Pk2c"},
  {"Pk1d"},
};


void     create_main_form(void);
// FD_BOXS* create_form_boxs();
void     open_map_callback(FL_OBJECT*, long);

/* Tensions */
void     create_tension_panel(int, int);
void     tension_WRI_callback(FL_OBJECT*, long);
void     tensions_ena_SET_callback(FL_OBJECT*, long);
void     tensions_ena_GET_callback(FL_OBJECT*, long);
void     read_tensions_ena_data(void);
void     read_tensions_sta_data(void);
void     show_tension_panel(void);

/* Marotta valves */
void     create_Mvalve_panel(int, int);
void     activate_Mvalve_INP(int, int);
void     Mvalve_WRI_callback(FL_OBJECT*, long);
void     Mvalve_INP_focus_callback(FL_OBJECT*, long);
void     Mvalve_INP_callback(FL_OBJECT*, long);
void     Mvalve_OPN_callback(FL_OBJECT*, long);
void     Mvalves_ena_SET_callback(FL_OBJECT*, long);
void     Mvalves_ena_GET_callback(FL_OBJECT*, long);
void     Mvalves_sta_GET_callback(FL_OBJECT*, long);
void     read_Mvalves_ena_data(void);
void     read_Mvalves_sta_data(void);
void     show_Mvalve_panel(void);

/* Heaters */
void     create_heater_panel(int, int);
void     activate_heater_INP(int, int);
void     heater_WRI_callback(FL_OBJECT*, long);
void     heater_INP_focus_callback(FL_OBJECT*, long);
void     heater_INP_callback(FL_OBJECT*, long);
void     heater_OPN_callback(FL_OBJECT*, long);
void     heaters_ena_SET_callback(FL_OBJECT*, long);
void     heaters_ena_GET_callback(FL_OBJECT*, long);
void     read_heaters_ena_data(void);
void     show_heater_panel(void);

/* Gp50 sensors */
void     create_sensor_panel(int, int);
void     sensor_CVT_callback(FL_OBJECT*, long);
void     convert_sensor(int);
void     sensor_GET_callback(FL_OBJECT*, long);
void     read_tsensor_data(int);
void     tsensor_GET_callback(FL_OBJECT*, long);
void     read_sensor_data(int);
void     show_sensor_panel(void);
float    pressure_psia(int, float);
float    temp_degrees(int, float);

/* Kulite sensors */
void     create_Ksensor_panel(int, int);
void     Ksensor_CVTS_callback(FL_OBJECT*, long);
void     convert_KsensorS(int);
void     Ksensor_CVTR_callback(FL_OBJECT*, long);
void     convert_KsensorR(int);
void     Ksensor_GET_callback(FL_OBJECT*, long);
void     read_Ksensor_data(int);
void     show_Ksensor_panel(void);
float    Kpressure_bar(int, float);

/* Common commands */
void     invalidate_data(void);
void     update_objects(void);
void     show_message(void);



/*********************************************************************
 *
 * Function:      start_boxs()
 * Description:   xforms initialization 
 *
 *********************************************************************/
/* void start_boxs(void)  */
/* {  */
/*   int one = 1; */
/*   static char* title = {"Box S Controller"}; */
/*   char* adr[] = {(char*)&title}; */
/*   if ( P > 2 ) printf(" *** invoking start_boxs\n"); */
/*   fl_initialize(&one, adr, " ", 0, 0); */
/*   boxs = create_form_boxs(); */
/*   //fl_ringbell(100); */
/*   fl_set_form_position(boxs->boxs, 0, 0); */
/*   fl_show_form(boxs->boxs, FL_PLACE_GEOMETRY, FL_FULLBORDER, "Box S"); */
/*   update_objects(); */
/* } */


/*********************************************************************
 *
 * Function:      create_form_boxs(int)
 * Description:   creation of the external box 
 *
 *********************************************************************/
/* FD_BOXS* create_form_boxs()  */

// modified by AL

void create_main_form(void)
{

  int WW = 615, HH = 610;
//int x_tens    = 260, y_tens    =  60;
  int x_kulite  =   5, y_kulite  = 225;
  int x_marotta = 260, y_marotta =  60;
  int x_heaters =   5, y_heaters = 365;
  int x_gp50    =   5, y_gp50    =  60;
//int x_hc = 220, y_hc = 0;

  FL_OBJECT *obj;
/*   FD_BOXS* fdui = (FD_BOXS*) fl_calloc(1, sizeof(*fdui)); */

  if ( P > 2 ) printf(" *** invoking create_main_form\n");

  invalidate_data();
#ifdef USE_CAN
  bgn_form(&MAIN, 0, 0, WW, HH);
#endif

/*   fdui->boxs = fl_bgn_form(FL_NO_BOX,w,h); */
/*   obj = fl_add_box(FL_FLAT_BOX,0,0,w,h," "); */
/*   obj = fl_add_box(FL_NO_BOX,0,0,w,50,"UGBS Controller"); */
/*   fl_set_object_lsize(obj,FL_LARGE_SIZE); */
/*   fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE); */
/*   fl_set_object_lcolor(obj,FL_WHITE); */
/*   create_USCM_JMDC_panels(); */

  create_HOTCOLD_panel();

  /* Tensions */
/*   create_tension_panel(x_tens, y_tens); */
  /* Marotta */
  create_Mvalve_panel(x_marotta, y_marotta);
  /* Heaters */
  create_heater_panel(x_heaters, y_heaters);
  /* Gp50 */
  create_sensor_panel(x_gp50, y_gp50);
  /* Kulite */
  create_Ksensor_panel(x_kulite, y_kulite);

  msg = 0;
  MESSAGE = obj = fl_add_text(FL_NORMAL_TEXT,5,HH-30,WW-50,28," ");
  fl_set_object_lsize(obj,FL_LARGE_SIZE);
  fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);

/*   yes = fl_add_button(FL_NORMAL_BUTTON, 540, 5, 60, 30, "Q\010uit"); */
/*   fl_set_object_lsize(yes,FL_MEDIUM_SIZE); */
/*   fl_set_object_lstyle(yes,FL_BOLD_STYLE); */

/*   obj = fl_add_button(FL_NORMAL_BUTTON, 470, 5, 60, 30, "M\010ap"); */
/*   fl_set_object_lsize(obj,FL_MEDIUM_SIZE); */
/*   fl_set_object_lstyle(obj,FL_BOLD_STYLE); */
/*   fl_set_object_callback(obj, open_map_callback, 1); */

  add_author( WW-30, 35, date, author);
  
  end_form(MAIN);
  update_objects();

/*   fl_end_form(); */
/*   fdui->boxs->fdui = fdui; */
/*   return fdui; */
}


/*********************************************************************
 *
 * Function:      open_map_callback(FL_OBJECT*, long)
 * Description:   display a map of the box
 *
 *********************************************************************/
void open_map_callback(FL_OBJECT* obj, long i) 
{

  FL_FORM* mapform;
  int w = 670, h = 400;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) 
    {
      mapform = fl_bgn_form(FL_NO_BOX, w, h);
      obj = fl_add_box(FL_FLAT_BOX,0,0,w,h," ");
      obj = fl_add_pixmap(FL_NORMAL_PIXMAP,0,0,w,h," ");
      fl_set_pixmap_file(obj, "schemes/boxS.xpm");
      fl_end_form();
      fl_show_form(mapform,FL_PLACE_GEOMETRY,FL_TRANSIENT,"MAP");
    }
}



/**************************************************************************************
 *
 * Function:      create_tension_panel(int, int)
 * Description:   create tensions panel
 *
 **************************************************************************************/
void create_tension_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 81, w1 = 130, h = 22;
  
  if ( P > 2 ) printf(" *** invoking create_tension_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+2*w1,2+10+(n_tensions+3)*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+2*w1, h, "POWER SUPPLY", size1);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, h, " ", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w1, h, "Enable", size1);
  x = x + w1;
  obj = add_frame_box(x, y, w1, h, "Status", size1);

  y = y + h;
  for (i=0; i<n_tensions; i++) {
    x = x0 + 5;
    // colonna col nome delle tensioni
    obj = add_frame_box(x, y, wa, h, tension[i].nam, size1);
    x = x + wa;
    // colonna di enable
    tension[i].WRI =
      obj = add_switch_button(x, y, w1, h, "X", size2);
    fl_set_object_callback(obj, tension_WRI_callback, i);
    x = x + w1;
    // colonna lettura stato di abilitazione
    tension[i].REA =
      obj = add_data_box(x, y, w1, h, "Y", size2);
    x = x0 + 5;
    y = y + h;
  }
  
  obj = add_frame_box(x, y, wa, h, " ", size1);
  x = x + wa;
  // colonna di open
  tensions.ena.SET =
    obj = add_normal_button(x, y, w1, h, "ENABLE", size1);
  fl_set_object_callback(obj, tensions_ena_SET_callback, 0);

  // colonna lettura stato di apertura
  x = x + w1;
  tensions.ena.GET =
    obj = add_normal_button(x, y, w1, h, "READ", size1);
  fl_set_object_callback(obj, tensions_ena_GET_callback, 0);

}


/**************************************************************************************
 *
 * Function:      tension_WRI_callback(FL_OBJECT*, int)
 * Description:   manage single tension enable/disable button
 *
 **************************************************************************************/
void tension_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int16 m;  

  if ( P > 2 ) printf(" *** invoking tension_WRI_callback\n");

  //printf("ind %d",ind); 

  if (ind != n_tensions-1) {
    if ( P > 3 ) printf(" tensions.ena.wri.cod  0x%04X\n", tensions.ena.wri.cod);
    tensions.ena.wri.cod ^= tension[ind].msk;
    if ( P > 3 ) printf(" tensions.ena.wri.cod ^= tension[ind].msk (0x%04X)   0x%04X\n", 
				   tension[ind].msk, tensions.ena.wri.cod);
  }
  else {
    m = tensions.ena.wri.cod & tension[n_tensions-1].msk;
    if (m == 0 || m == tension[n_tensions-1].msk) {
      tensions.ena.wri.cod ^= tension[n_tensions-1].msk;
    }
    else {
      tensions.ena.wri.cod &= ~tension[n_tensions-1].msk;
    }
  }

  printf("\n Changing status of tension %s\n", tension[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", tensions.ena.wri.cod);  
  msg = 0;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      tension_ena_SET_callback(FL_OBJECT*, int)
 * Description:   callback for tensions enable
 *                enable: 1P00 0111 1100 1000 5V.dummy.10V.0 0000 0000 0000
 *                        (8/C) (7)  (C)  (8) +---- (tensions.ena.wri.cod) ----+
 *
 **************************************************************************************/
void tensions_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_tensions_bits = 0x87C8;
  int32 wri, rea, mask;
  int16 err;
  int p, bin_wri;
  char b[80];

  if ( P > 2 ) printf(" *** invoking tensions_ena_SET_callback\n");

  chk = 1;
  msg = -1;
  update_objects();

  wri = ena_tensions_bits << 16;
  wri += tensions.ena.wri.cod;
  if ( P > 3 ) printf(" clk = %02X  dat = %04X  ena_tensions_bits = %04X\n", 
		    clk, dat, ena_tensions_bits);
  if ( P > 3 ) printf(" tensions.ena.wri.cod = %08X, wri = %08X\n", 
		    tensions.ena.wri.cod, wri);

  // Calcolo del bit di parita'
  p = parity32(wri);

  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);
  bin_wri = sprintf_binary32(b, wri, -32);

  printf("\n Enabling selected tensions\n");
  if ( P > 0 ) printf("    [code = %s]\n", b);

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  mask = 0xA000;
  tensions.ena.wri.was = tensions.ena.wri.cod ^ mask;
  tensions.ena.wri.sta = status_OK;  
  if (err) sta = 0;
  else     sta = 1;
  if ( P > 3 ) printf(" chk = %d\n",chk);
  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      tension_ena_GET_callback(FL_OBJECT*, int)
 * Description:   manage tensions read button
 *
 **************************************************************************************/
void tensions_ena_GET_callback(FL_OBJECT *obj, long data) 
{

  if ( P > 2 ) printf(" *** invoking tensions_ena_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    // se abbassato, tira su il pulsante col click sx.
    if (tensions.ena.rea.mon) {
      tensions.ena.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    // se il pulsante e' abilitato, esegue la lettura dello stato
    else {
      msg = -1;
      update_objects();
      read_tensions_ena_data();
      msg = 1;  
      update_objects();
    }
  }

  // right-click disabilita il pulsante (o lo riabilita)
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    tensions.ena.rea.mon = 1 - tensions.ena.rea.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      read_tensions_ena_data(void)
 * Description:   ask tensions enable status 
 *                code: 0xC7C00000
 *                answer: 5V.-.10V.- ---- ---- ---- (1 = disabled)
 *
 **************************************************************************************/
void read_tensions_ena_data(void) 
{

  int32 wri = 0xC7C00000, rea, mask, rea_en;
  chk = 1;
  int16 err;
  char a[80], b[80];
  int la, lb;

  if ( P > 2 ) printf(" *** invoking read_tensions_ena_data\n");

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  //rea = 0x2000;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  printf("\n Reading tensions enable status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", rea);

  mask = 0xA000;
  rea_en = mask ^ rea;
  
  la = sprintf_binary32(a,rea,-32);

  tensions.ena.rea.cod = rea_en;

  lb = sprintf_binary32(b,tensions.ena.rea.cod,-32);
  if ( P > 3 ) printf(" tensions.ena.rea.cod = %08X = %s\n", tensions.ena.rea.cod, b);

  tensions.ena.rea.sta = status_OK;  
  if (err) sta = 0;
  else     sta = 1;
  if ( P > 3 ) printf(" chk = %d\n",chk);
  msg = 1;
  update_objects();

}
/**************************************************************************************
 *
 * Function:      show_tension_panel(void)
 * Description:   show tensions panel
 *
 **************************************************************************************/
void show_tension_panel(void) 
{

  FL_OBJECT *obj;
  int16 m, m1, m2;
  int i;
  int16 mask = 0xA000, code;
    
  if ( P > 2 ) printf(" *** invoking show_tension_panel\n");

  // colonna 'Enable'
  for (i=0; i<n_tensions; i++) {
    obj = tension[i].WRI;
    code = tensions.ena.wri.cod ^ mask;
    m = tension[i].msk & code;
    if (m == tension[i].msk) {
      fl_set_object_color(obj, FL_PALEGREEN, FL_PALEGREEN);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "E");
    }
    else if (m == 0) {
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_WHEAT);
      fl_set_object_label(obj, "D");
    }
    else {
      fl_set_object_color(obj, FL_DARKGOLD, FL_DARKGOLD);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "?");
    }
  }

  // colonna 'Status'
  for (i=0; i<n_tensions; i++) {
    if ( P > 3 ) printf(" tension: %s \n", tension[i].nam);

    obj = tension[i].REA;
    m  = tension[i].msk & tensions.ena.rea.cod;
    m1 = tensions.ena.wri.was & tension[i].msk;
    m2 = tensions.ena.rea.cod & tension[i].msk;

    if ( P > 3 ) printf("  m =  tension[i].msk & tensions.ena.rea.cod = %08X & %08X = %08X \n", 
	   tension[i].msk, tensions.ena.rea.cod, m);
    if ( P > 3 ) printf("  m1 =  tension[i].msk & tensions.ena.rea.was = %08X & %08X = %08X \n", 
	   tension[i].msk, tensions.ena.wri.was, m);

    if (tensions.ena.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
    }
    else {
      if (m == tension[i].msk) fl_set_object_label(obj, "E");
      else if (m == 0)       fl_set_object_label(obj, "D");
      else                   fl_set_object_label(obj, "?");
    }

    if (tensions.ena.rea.sta == status_OK) {
      if (m1 == m2) fl_set_object_color(obj, FL_PALEGREEN, 0);
      else          fl_set_object_color(obj, FL_DARKGOLD, 0);
      if (m != tension[i].msk && m != 0) fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }

    if (tensions.ena.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }

    if (tensions.ena.rea.sta == status_NA ||
        tensions.ena.rea.sta == status_NN) {
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }

  
  obj = tensions.ena.GET;
  if (tensions.ena.rea.mon) {
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_DARKGOLD);
  }
  else {
    fl_set_object_boxtype(obj, FL_UP_BOX);
    fl_set_object_color(obj, FL_MCOL, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }

}


/**************************************************************************************
 *
 * Function:      create_Mvalve_panel(int, int)
 * Description:   create Marotta valves panel
 *
 **************************************************************************************/
void create_Mvalve_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 85, w1 = 10, w2 = 25, w3 = 75, w4 = 70, h = 25;
  
  if ( P > 2 ) printf(" *** invoking create_Mvalve_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+w1+2*w2+w3+w4+2*w2,2+10+(n_marotta+4)*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+w1+2*w2+w3+w4+2*w2, h, "MAROTTA VALVES", size1);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, 2*h, "Valve", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w1+2*w2, 2*h, "Enable", size1);
  x = x + w1 + 2 * w2;
  obj = add_frame_box(x, y, w3, 2*h, "Time [s]", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w4, 2*h, "Command", size1);
  x = x + w4;
  obj = add_frame_box(x, y, 2*w2, h, "Status", size1);
  obj = add_frame_box(x, y+h, w2, h, "A", size1);
  x = x + w2;
  obj = add_frame_box(x, y+h, w2, h, "E", size1);
  x = x + w2;

  y = y + 2 * h;
  for (i=0; i<n_marotta; i++) {
    x = x0 + 5;
    // colonna col nome delle valvole
    obj = add_frame_box(x, y, wa, h, valve[i].nam, size1);
    x = x + wa;
    // colonna di tastini senza label
    valve[i].WAS =
      obj = add_data_box(x, y, w1, h, " ", size2);
    x = x + w1;
    // colonna di enable
    valve[i].WRI =
      obj = add_switch_button(x, y, w2, h, "X", size2);
    fl_set_object_callback(obj, Mvalve_WRI_callback, i);
    x = x + w2;
    // colonna lettura stato di abilitazione
    valve[i].REA =
      obj = add_data_box(x, y, w2, h, "Y", size2);
    x = x + w2;
    // time input
    valve[i].VAL = 
      obj = add_switch_button(x, y, w3, h, " ", size2);
    fl_set_object_callback(obj, Mvalve_INP_focus_callback, i);
    valve[i].INP = 
      obj = fl_add_input(FL_FLOAT_INPUT,x,y-5,w3,h+10," ");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_WHEAT);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_input_color(obj,FL_BLACK,FL_BLACK);
    fl_set_input_return(obj,FL_RETURN_END);
    fl_set_object_callback(obj, Mvalve_INP_callback, i);
    x = x + w3;
    // colonna di open
    valve[i].OPN =
      obj = add_normal_button(x, y, w4, h, "OPEN", size1);
    fl_set_object_callback(obj, Mvalve_OPN_callback, i);
    x = x + w4;
    // colonne lettura stato di apertura
    valve[i].ASTA =
      obj = add_data_box(x, y, w2, h, "Z", size2);
    x = x + w2;
    valve[i].OSTA =
      obj = add_data_box(x, y, w2, h, "Z", size2);
    x = x + w2;
    y = y + h;
  }
  activate_Mvalve_INP(-1, 0);
    
  x = x0 + 5;
  obj = add_frame_box(x, y, wa+w1, h, " ", size1);
  x = x + wa + w1;
  valves.ena.SET =
    obj = add_normal_button(x, y, w2, h, "W", size1);
  fl_set_object_callback(obj, Mvalves_ena_SET_callback, 0);
  x = x + w2;
  valves.ena.GET =
    obj = add_normal_button(x, y, w2, h, "R", size1);
  fl_set_object_callback(obj, Mvalves_ena_GET_callback, 0);
  x = x + w2;
  obj = add_frame_box(x, y, w3+w4, h, " ", size1);
  x = x + w3 + w4;
  valves.sta.GET =
    obj = add_normal_button(x, y, 2*w2, h, "READ", size1);
  fl_set_object_callback(obj, Mvalves_sta_GET_callback, 0);
  x = x + 3*w2;
}


/**************************************************************************************
 *
 * Function:      activate_Mvalve_INP(int, int)
 * Description:   manage field of Marotta valves opening time
 *
 **************************************************************************************/
void activate_Mvalve_INP(int ind, int mode) 
{

  FL_OBJECT *obj;
  int i;
  char b[80];
  
  if ( P > 2 ) printf(" *** invoking activate_Mvalve_INP\n");
  for (i=0; i<n_marotta; i++) {
    obj = valve[i].INP;
    if (i == ind) {
      fl_activate_object(obj);
      fl_show_object(obj);
      if (valve[i].wri.sta == status_NN) {
        sprintf(b, " ");
      }
      else {
        if (mode) sprintf(b, "%.000f", valve[i].wri.val);
        else      sprintf(b, " ");
      }
      fl_set_input(obj, b);
    }
    else {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
  }

  for (i=0; i<n_marotta; i++) {
    obj = valve[i].VAL;
    if (i == ind) {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
    else {
      fl_activate_object(obj);
      fl_show_object(obj);
    }
  }

  Mvalve_INP_active = ind;
}


/**************************************************************************************
 *
 * Function:      Mvalve_WRI_callback(int, int)
 * Description:   manage single Marotta valve enable/disable button
 *
 **************************************************************************************/
void Mvalve_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int16 m;  

  if ( P > 2 ) printf(" *** invoking Mvalve_WRI_callback\n");
  //printf("  ind %3d \n",ind);
  if (ind != n_marotta-1) {
    valves.ena.wri.cod ^= valve[ind].msk;
  }
  else {
    m = valves.ena.wri.cod & valve[n_marotta-1].msk;
    if (m == 0 || m == valve[n_marotta-1].msk) {
      valves.ena.wri.cod ^= valve[n_marotta-1].msk;
    }
    else {
      valves.ena.wri.cod &= ~valve[n_marotta-1].msk;
    }
  }
  printf("\n Changing status of Marotta valve %s\n", valve[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", valves.ena.wri.cod);
  
  msg = 0;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      Mvalve_INP_focus_callback(LF_OBJECT*, int)
 * Description:   manage callback of Marotta valve opening time 
 *
 **************************************************************************************/
void Mvalve_INP_focus_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int mode = -1;          // modified by AL

  if ( P > 2 ) printf(" *** invoking Mvalve_INP_focus_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  mode = 0;
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) mode = 1;

  activate_Mvalve_INP(ind, mode);
  
  update_objects();
}


/**************************************************************************************
 *
 * Function:      Mvalve_INP_callback(LF_OBJECT*, int)
 * Description:   callback of Marotta valve opening time 
 *                Range:  131 ms < t < 143 min = 8585 s
 *
 **************************************************************************************/

void Mvalve_INP_callback(FL_OBJECT *obj, long data) 
{

  int ind = data, len;
  double t;
  char b[80];

  if ( P > 2 ) printf(" *** invoking Mvalve_INP_callback\n");

  t = atof(fl_get_input(valve[ind].INP));
  if (t < 0.131) t = 0.131;
  if (t > 8585.085)  t = 8585.085;
  valve[ind].wri.val = t;
  valve[ind].wri.cod = t * 1000.0 / 131.;
  /* valve[ind].wri.val = valve[ind].wri.cod / 1000.0; */
  
  printf("\n Setting opening time for Marotta valve %s to %6.3f sec\n", valve[ind].nam, t);
  len = sprintf_binary(b,valve[ind].wri.cod,-16);
  if ( P > 0 ) printf("    [code = %04X  %s]\n", valve[ind].wri.cod, b);
  valve[ind].wri.cod = invert_bit_order(valve[ind].wri.cod);
  len = sprintf_binary(b,valve[ind].wri.cod,-16);
  if ( P > 3 ) printf("    [(after invert) code = %04X  %s]\n", valve[ind].wri.cod, b);

  valve[ind].wri.sta = status_NA;
  activate_Mvalve_INP(-1, 0);
  update_objects();
}


/**************************************************************************************
 *
 * Function:      Mvalve_OPN_callback(LF_OBJECT*, int)
 * Description:   callback of Marotta valve open button (takes time into account)
 *                open: 1P00 VVVV V100 1000 TTTT TTTT TTTT TTTT  (ms)
 *                           (valv)        (LSB) ----------> (MSB)
 *
 **************************************************************************************/
void Mvalve_OPN_callback(FL_OBJECT *obj, long data) 
{

  int ind = data, bin_wri;
  int32 wri, rea;
  int16 Mvalve_bits  = 0x0;
  int16 open_marotta_bits = 0x8008;
  chk = 1;
  int16 err;
  int p;
  char b[80];

  if ( P > 2 ) printf(" *** invoking Mvalve_OPN_callback\n");

  if (ind == Mvalve_INP_active) activate_Mvalve_INP(-1, 0);
  msg = -1;
  update_objects();

  if (ind ==  0) Mvalve_bits = 0x140;  // V1A
  if (ind ==  1) Mvalve_bits = 0x940;  // V2A
  if (ind ==  2) Mvalve_bits = 0x540;  // V3A
  if (ind ==  3) Mvalve_bits = 0xD40;  // V4A
  if (ind ==  4) Mvalve_bits = 0x340;  // Sp1
  if (ind ==  5) Mvalve_bits = 0xB40;  // V10A
  if (ind ==  6) Mvalve_bits = 0x740;  // V20A
  if (ind ==  7) Mvalve_bits = 0xF40;  // V1B
  if (ind ==  8) Mvalve_bits = 0x0C0;  // V2B
  if (ind ==  9) Mvalve_bits = 0x8C0;  // V3B
  if (ind == 10) Mvalve_bits = 0x4C0;  // V4B
  if (ind == 11) Mvalve_bits = 0xCC0;  // V10B
  if (ind == 12) Mvalve_bits = 0x2C0;  // V20B
  if (ind == 13) Mvalve_bits = 0xE40;  // Sp2
  if (ind == 14) Mvalve_bits = 0xAC0;  // V20A&V20B
 
  open_marotta_bits += Mvalve_bits;
  if ( P > 3 ) printf(" open_marotta_bits = %04X\n", open_marotta_bits);
  
  wri = open_marotta_bits << 16;
  if ( P > 3 ) printf(" wri = %08X\n", wri);
  wri += valve[ind].wri.cod;
  if ( P > 3 ) printf(" with time code: wri = %08X\n", wri);
  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) printf(" with parity p = %d,  wri = %08X\n", p,wri);

  bin_wri = sprintf_binary32(b, wri, -32);
  printf("\n Opening Marotta valve %s\n", valve[ind].nam);
  if ( P > 0 ) printf("     [code = %s]\n", b);

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);
/*   printf("1 -- qui, err = %d\n", err); */
/* /\*   if(!err) increase_valve_count(valve[ind].nam); *\/ */
/*   printf("2 -- qui\n"); */


  if (err) sta = 0;
  else     sta = 1;
  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      Mvalve_ena_SET_callback(LF_OBJECT*, int)
 * Description:   callback of Marotta valve enable button
 *                enable: 1P00 1000 0010 1000 V1aV2aV3aV4a Sp1V10aV20aV1b V2bV3bV4bV10b V20bSp2.0.0
 *                        (8/C) (8)  (2)  (8) +---- (valves.ena.wri.cod) --------------------------+
 *
 **************************************************************************************/
void Mvalves_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_marotta_bits = 0x8848;
  int32 wri, rea;
  chk = 1;
  int16 err;
  int p, bin_wri;
  char b[80];

  if ( P > 2 ) printf(" *** invoking Mvalves_ena_SET_callback\n");

  msg = -1;
  update_objects();

  wri = ena_marotta_bits << 16;
  wri += valves.ena.wri.cod;
  if ( P > 3 ) printf(" clk = %02X  dat = %04X  ena_marotta_bits = %04X\n", 
	 clk, dat, ena_marotta_bits);
  if ( P > 3 ) printf(" valves.ena.wri.cod = %08X, wri = %08X\n", valves.ena.wri.cod, wri);

  // Calcolo del bit di parita'
  p = parity32(wri);

  wri += p << 30;
  if ( P > 3 ) printf(" now wri = %08X\n", wri);
  bin_wri = sprintf_binary32(b, wri, -32);

  printf("\n Enabling selected Marotta valves\n");
  if ( P > 0 ) printf("    [code = %s]\n", b);

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  valves.ena.wri.was = valves.ena.wri.cod;
  valves.ena.wri.sta = status_OK;  
  if (err) sta = 0;
  else     sta = 1;

  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      Mvalve_ena_GET_callback(LF_OBJECT*, int)
 * Description:   manage Marotta valve read button
 *                valves.ena.rea.mon = 0 => "R" button disabled
 *
 **************************************************************************************/
void Mvalves_ena_GET_callback(FL_OBJECT *obj, long data) 
{

  if ( P > 2 ) printf(" *** invoking Mvalves_ena_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    // se abbassato, tira su il pulsante col click sx.
    if (valves.ena.rea.mon) {
      valves.ena.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    // se il pulsante e' abilitato, esegue la lettura dello stato
    else {
      msg = -1;
      update_objects();
      read_Mvalves_ena_data();
      msg = 1;  
      update_objects();
    }
  }

  // right-click disabilita il pulsante (o lo riabilita)
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    valves.ena.rea.mon = 1 - valves.ena.rea.mon;
    update_objects();
  }
}



/**************************************************************************************
 *
 * Function:      Mvalve_sta_GET_callback(LF_OBJECT*, int)
 * Description:   manage Marotta valve status button
 *
 **************************************************************************************/
void Mvalves_sta_GET_callback(FL_OBJECT *obj, long data) 
{

  if ( P > 2 ) printf(" *** invoking Mvalves_sta_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (valves.sta.rea.mon) {
      valves.sta.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      read_Mvalves_sta_data();
      msg = 1;  
      update_objects();
    }
  }

  // right-click disabilita il pulsante
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    valves.sta.rea.mon = 1 - valves.sta.rea.mon;
    update_objects();
  }
}



/****************************************************************************************************
 *
 * Function:      read_Mvalves_ena_data(void)
 * Description:   return Marotta valves enable status
 *                code to ask: 0x88400000
 *                answer:      ---- ---- ---- ---- V1aV2aV3aV4a V5V10aV20aV1b V2bV3bV4bV10b V20b---
 *                                                 |
 *                                                 +----> valves.ena.rea.cod
 *
 ****************************************************************************************************/
void read_Mvalves_ena_data(void) 
{

  int32 wri = 0x88400000, rea, mask;
  chk = 1;
  int la, lb;
  int16 err;
  char a[80], b[80];

  if ( P > 2 ) printf(" *** invoking read_Mvalves_ena_data\n");

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  //rea = 0x9148;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = 0xFFFC;
  rea &= mask;

  la = sprintf_binary32(a,rea,-32);

  //valves.ena.rea.cod = rea >> 4;
  valves.ena.rea.cod = rea;
  printf("\n Reading Marotta valves enable status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", valves.ena.rea.cod);

  lb = sprintf_binary32(b,valves.ena.rea.cod,-32);
  if ( P > 3 ) printf(" valves.ena.rea.cod = %08X = %s\n", valves.ena.rea.cod, b);

  valves.ena.rea.sta = status_OK;  // controllare il significato di questa variabile

}


/**************************************************************************************
 *
 * Function:      read_Mvalves_sta_data(void)
 * Description:   return Marotta valves opening status
 *                code to ask: 0x82200000
 *
 **************************************************************************************/
void read_Mvalves_sta_data(void) 
{


  int32 wri_a = 0x84400000, wri_e = 0xCA400000;
  int32 rea, mask;
  int32 a_rea, e_rea;
  chk = 1;
  int16 err;
  int la, lb, lc;
  char a[80], b[80], c[80];

  if ( P > 2 ) printf(" *** invoking read_Mvalves_sta_data\n");


  mask = 0xFFFC;
  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 

  // "active" bits (electrical)
  exec_LeCroy_cmd(n_CMDS, &bus, &wri_a, &rea, &chk, &err);
  //rea = 0x9148;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  a_rea = rea & mask;

  // "event" bits (electrical)
  exec_LeCroy_cmd(n_CMDS, &bus, &wri_e, &rea, &chk, &err);
  //rea = 0x9348;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  e_rea = rea & mask;

  lb = sprintf_binary32(b,a_rea,-32);
  lc = sprintf_binary32(c,e_rea,-32);
  
  if ( P > 3 ) printf(" a_rea = %s\n, e_rea = %s\n", b, c);

  valves.sta.rea.acod = a_rea;
  valves.sta.rea.ocod = e_rea;

  printf("\n Reading Marotta valves status\n");
  if ( P > 0 ) printf("    [code in answer = %04X, %04X]\n", 
	 valves.sta.rea.acod, valves.sta.rea.ocod);

  valves.sta.rea.sta = status_OK;  

}




/**************************************************************************************
 *
 * Function:      show_Mvalves_panel(void)
 * Description:   show Marotta valves panel
 *
 **************************************************************************************/
void show_Mvalve_panel(void) 
{

  FL_OBJECT *obj;
  char b[80];
  int16 m, m1, m2;
  int i;
  
  if ( P > 2 ) printf(" *** invoking show_Mvalve_panel\n");

  for (i=0; i<n_marotta; i++) {
    // tastini senza label 
    // cambiano colore quando viene variato lo stato di abilitazione 
    // finche' non si preme W
    obj = valve[i].WAS;
    m1 = valves.ena.wri.was & valve[i].msk;    // stato precedente della valvola 
    m2 = valves.ena.wri.cod & valve[i].msk;    // stato impostato per la valvola
    if ((valves.ena.wri.sta == status_OK) && (m1 == m2)) {
      fl_set_object_color(obj, FL_PALEGREEN, 0);
    }
    else {
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
  }
  
  // colonna 'Enable'
  for (i=0; i<n_marotta; i++) {
    obj = valve[i].WRI;
    m = valve[i].msk & valves.ena.wri.cod;
    if (m == valve[i].msk) {
      fl_set_object_color(obj, FL_PALEGREEN, FL_PALEGREEN);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "E");
    }
    else if (m == 0) {
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_WHEAT);
      fl_set_object_label(obj, "D");
    }
    else {
      fl_set_object_color(obj, FL_DARKGOLD, FL_DARKGOLD);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "?");
    }
  }
  
  for (i=0; i<n_marotta; i++) {
    obj = valve[i].REA;
    m  = valve[i].msk & valves.ena.rea.cod;
    m1 = valves.ena.wri.was & valve[i].msk;
    m2 = valves.ena.rea.cod & valve[i].msk;
    if (valves.ena.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
    }
    else {
      if (m == valve[i].msk) fl_set_object_label(obj, "E");
      else if (m == 0)       fl_set_object_label(obj, "D");
      else                   fl_set_object_label(obj, "?");
    }
    if (valves.ena.rea.sta == status_OK) {
      if (m1 == m2) fl_set_object_color(obj, FL_PALEGREEN, 0);
      else          fl_set_object_color(obj, FL_DARKGOLD, 0);
      if (m != valve[i].msk && m != 0) fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (valves.ena.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (valves.ena.rea.sta == status_NA ||
        valves.ena.rea.sta == status_NN) {
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }

  // colonna "time"  
  for (i=0; i<n_marotta; i++) {
    obj = valve[i].VAL;
    sprintf(b, "%6.3f", valve[i].wri.val);
    if (i != Mvalve_INP_active) {
      fl_set_object_label(obj, b);
      if (valve[i].wri.sta == status_OK)
        fl_set_object_color(obj, FL_PALEGREEN, FL_RED);
      else
        fl_set_object_color(obj, FL_MCOL, FL_RED);
        fl_set_object_lcolor(obj, FL_WHITE);
    }
  }
  
  // colonna "status" attivazione
  for (i=0; i<n_marotta; i++) {
    obj = valve[i].ASTA;
    if (valves.sta.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (valves.sta.rea.sta == status_NA) {
      fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (valves.sta.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (valves.sta.rea.sta == status_OK) {
      m  = valve[i].msk & valves.sta.rea.acod;
      if (m == valve[i].msk) {
        fl_set_object_label(obj, "Y");
        fl_set_object_color(obj, FL_PALEGREEN, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
      else if (m == 0) {
        fl_set_object_label(obj, "N");
        fl_set_object_color(obj, FL_RED, 0);
        fl_set_object_lcolor(obj, FL_WHEAT);
      }
      else {
        fl_set_object_label(obj, "?");
        fl_set_object_color(obj, FL_DARKGOLD, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
    }
  }
  
  // colonna "status" open
  for (i=0; i<n_marotta; i++) {
    obj = valve[i].OSTA;
    if (valves.sta.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (valves.sta.rea.sta == status_NA) {
      fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (valves.sta.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (valves.sta.rea.sta == status_OK) {
      m  = valve[i].msk & valves.sta.rea.ocod;
      if (m == valve[i].msk) {
        fl_set_object_label(obj, "Y");
        fl_set_object_color(obj, FL_PALEGREEN, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
      else if (m == 0) {
        fl_set_object_label(obj, "N");
        fl_set_object_color(obj, FL_RED, 0);
        fl_set_object_lcolor(obj, FL_WHEAT);
      }
      else {
        fl_set_object_label(obj, "?");
        fl_set_object_color(obj, FL_DARKGOLD, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
    }
  }
  
  obj = valves.ena.GET;
  if (valves.ena.rea.mon) {
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_DARKGOLD);
  }
  else {
    fl_set_object_boxtype(obj, FL_UP_BOX);
    fl_set_object_color(obj, FL_MCOL, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }

  obj = valves.sta.GET;
  if (valves.sta.rea.mon) {
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_DARKGOLD);
  }
  else {
    fl_set_object_boxtype(obj, FL_UP_BOX);
    fl_set_object_color(obj, FL_MCOL, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }

}


/**************************************************************************************
 *
 * Function:      create_heater_panel(int, int)
 * Description:   create heaters panel
 *
 **************************************************************************************/
void create_heater_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 45, w1 = 10, w2 = 25, w3 = 71, w4 = 65, h = 25;
  
  if ( P > 2 ) printf(" *** invoking create_heater_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+w1+2*w2+w3+w4,2+10+(n_heaters+3)*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+w1+2*w2+w3+w4, h, "HEATERS", size1);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, h, "Heater", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w1+2*w2, h, "Enable", size1);
  x = x + w1 + 2 * w2;
  obj = add_frame_box(x, y, w3, h, "Time [min]", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w4, h, "Command", size1);

  y = y + h;
  for (i=0; i<n_heaters; i++) {
    x = x0 + 5;
    // colonna col nome dell'heater
    obj = add_frame_box(x, y, wa, h, heater[i].nam, size1);
    x = x + wa;
    // colonna di tastini senza label
    heater[i].WAS =
      obj = add_data_box(x, y, w1, h, " ", size2);
    x = x + w1;
    // colonna di enable
    heater[i].WRI =
      obj = add_switch_button(x, y, w2, h, "X", size2);
    fl_set_object_callback(obj, heater_WRI_callback, i);
    x = x + w2;
    // colonna lettura stato di abilitazione
    heater[i].REA =
      obj = add_data_box(x, y, w2, h, "Y", size2);
    x = x + w2;
    // time input
    heater[i].VAL = 
      obj = add_switch_button(x, y, w3, h, " ", size2);
    fl_set_object_callback(obj, heater_INP_focus_callback, i);
    heater[i].INP = 
      obj = fl_add_input(FL_FLOAT_INPUT,x,y-5,w3,h+10," ");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_WHEAT);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_input_color(obj,FL_BLACK,FL_BLACK);
    fl_set_input_return(obj,FL_RETURN_END);
    fl_set_object_callback(obj, heater_INP_callback, i);
    x = x + w3;
    // colonna di open
    heater[i].OPN =
      obj = add_normal_button(x, y, w4, h, "HEAT", size1);
    fl_set_object_callback(obj, heater_OPN_callback, i);
    y = y + h;
  }
  activate_heater_INP(-1, 0);
    
  x = x0 + 5;
  obj = add_frame_box(x, y, wa+w1, h, " ", size1);
  x = x + wa + w1;
  heaters.ena.SET =
    obj = add_normal_button(x, y, w2, h, "W", size1);
  fl_set_object_callback(obj, heaters_ena_SET_callback, 0);
  x = x + w2;
  heaters.ena.GET =
    obj = add_normal_button(x, y, w2, h, "R", size1);
  fl_set_object_callback(obj, heaters_ena_GET_callback, 0);
  x = x + w2;
  obj = add_frame_box(x, y, w3+w4, h, " ", size1);
}


/**************************************************************************************
 *
 * Function:      activate_heater_INP(int, int)
 * Description:   manage field of heaters opening time
 *
 **************************************************************************************/
void activate_heater_INP(int ind, int mode) 
{

  FL_OBJECT *obj;
  int i;
  char b[80];
  
  if ( P > 2 ) printf(" *** invoking activate_heater_INP\n");
  for (i=0; i<n_heaters; i++) {
    obj = heater[i].INP;
    if (i == ind) {
      fl_activate_object(obj);
      fl_show_object(obj);
      if (heater[i].wri.sta == status_NN) {
        sprintf(b, " ");
      }
      else {
        if (mode) sprintf(b, "%.000f", heater[i].wri.val);
        else      sprintf(b, " ");
      }
      fl_set_input(obj, b);
    }
    else {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
  }

  for (i=0; i<n_heaters; i++) {
    obj = heater[i].VAL;
    if (i == ind) {
      fl_deactivate_object(obj);
      fl_hide_object(obj);
    }
    else {
      fl_activate_object(obj);
      fl_show_object(obj);
    }
  }

  heater_INP_active = ind;
}


/**************************************************************************************
 *
 * Function:      heater_WRI_callback(int, int)
 * Description:   manage single heater enable/disable button
 *
 **************************************************************************************/
void heater_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int16 m;  

  if ( P > 2 ) printf(" *** invoking heater_WRI_callback\n");
  //printf("  ind %3d \n",ind);
  if (ind != n_heaters-1) {
    heaters.ena.wri.cod ^= heater[ind].msk;
  }
  else {
    m = heaters.ena.wri.cod & heater[n_heaters-1].msk;
    if (m == 0 || m == heater[n_heaters-1].msk) {
      heaters.ena.wri.cod ^= heater[n_heaters-1].msk;
    }
    else {
      heaters.ena.wri.cod &= ~heater[n_heaters-1].msk;
    }
  }
  printf("\n Changing status of heater %s\n", heater[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", heaters.ena.wri.cod);
  
  msg = 0;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      heater_INP_focus_callback(LF_OBJECT*, int)
 * Description:   manage callback of heater opening time 
 *
 **************************************************************************************/
void heater_INP_focus_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int mode = -1;          // modified by AL

  if ( P > 2 ) printf(" *** invoking heater_INP_focus_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  mode = 0;
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) mode = 1;

  activate_heater_INP(ind, mode);
  
  update_objects();
}


/**************************************************************************************
 *
 * Function:      heater_INP_callback(LF_OBJECT*, int)
 * Description:   callback of heater opening time 
 *                Range:  4 s < t < 72.817 h = 4369 min = (65535 * 4) s  
 *
 **************************************************************************************/
void heater_INP_callback(FL_OBJECT *obj, long data) 
{

  int ind = data, len;
  double t;
  char b[80];

  if ( P > 2 ) printf(" *** invoking heater_INP_callback\n");
  t = atof(fl_get_input(heater[ind].INP));
  if (t < 0.0)  t = 0.0;
  if (t > 4369) t = 4369;
  //heater[ind].wri.val = t;
  heater[ind].wri.cod = t * 15.0;  // 4369*15=65535
  heater[ind].wri.val = heater[ind].wri.cod / 15.0;
  
  printf("\n Setting opening time for heater %s to %6.1f min\n", heater[ind].nam, t);
  len = sprintf_binary(b,heater[ind].wri.cod,-16);
  if ( P > 0 ) printf("    [code = %04X  %s]\n", heater[ind].wri.cod, b);
  heater[ind].wri.cod = invert_bit_order(heater[ind].wri.cod);
  len = sprintf_binary(b,heater[ind].wri.cod,-16);
  if ( P > 3 ) printf("    [(after invert) code = %04X  %s]\n", heater[ind].wri.cod, b);

  heater[ind].wri.sta = status_NA;
  activate_heater_INP(-1, 0);
  update_objects();
}


/**************************************************************************************
 *
 * Function:      heater_OPN_callback(LF_OBJECT*, int)
 * Description:   callback of heater open button (takes time into account)
 *                open: 1P00 HHHH H010 1111 TTTT TTTT TTTT TTTT  (ms)
 *                           (heat)        (LSB) ----------> (MSB)
 *
 **************************************************************************************/
void heater_OPN_callback(FL_OBJECT *obj, long data) 
{

  int ind = data, bin_wri;
  int32 wri, rea;
  int16 heater_bits  = 0x0;
  int16 open_heaters_bits = 0x800F;
  chk = 1;
  int16 err;
  int p;
  char b[80];

  if ( P > 2 ) printf(" *** invoking heater_OPN_callback\n");

  if (ind == heater_INP_active) activate_heater_INP(-1, 0);
  msg = -1;
  update_objects();

  if (ind ==  0) heater_bits = 0x000; // NO heater H1 on Xe vessel at this stage
  if (ind ==  1) heater_bits = 0x000; // NO heater H2 on CO2 vessel at this stage

  if (ind ==  0) heater_bits = 0x140;
  if (ind ==  1) heater_bits = 0x940;

  if (ind ==  2) heater_bits = 0x540;
  if (ind ==  3) heater_bits = 0xD40;
  if (ind ==  4) heater_bits = 0x340;

  open_heaters_bits += heater_bits;
  if ( P > 3 ) printf(" open_heaters_bits = %04X\n", open_heaters_bits);
  
  wri = open_heaters_bits << 16;
  if ( P > 3 ) printf(" wri = %08X\n", wri);
  wri += heater[ind].wri.cod;
  if ( P > 3 ) printf(" with time code: wri = %08X\n", wri);
  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) printf(" with parity p = %d,  wri = %08X\n", p,wri);

  bin_wri = sprintf_binary32(b, wri, -32);
  printf("\n Starting heater %s\n", heater[ind].nam);
  if ( P > 0 ) printf("     [code = %s]\n", b);

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;
  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      heater_ena_SET_callback(LF_OBJECT*, int)
 * Description:   callback of heater enable button
 *                enable: 1P00 1011 1100 1000 H1.H2.H3.H4 H5.000 0000 0000
 *                        (8/C) (B)  (C)  (8) +---- (heaters.ena.wri.cod) ----+
 *
 **************************************************************************************/
void heaters_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_heaters_bits = 0x8BC8;
  int32 wri, rea;
  chk = 1;
  int16 err;
  int p, bin_wri;
  char b[80];

  if ( P > 2 ) printf(" *** invoking heaters_ena_SET_callback\n");

  msg = -1;
  update_objects();

  wri = ena_heaters_bits << 16;
  wri += heaters.ena.wri.cod;
  if ( P > 3 ) printf(" clk = %02X  dat = %04X  ena_heaters_bits = %04X\n", 
	 clk, dat, ena_heaters_bits);
  if ( P > 3 ) printf(" heaters.ena.wri.cod = %08X, wri = %08X\n", heaters.ena.wri.cod, wri);

  // Calcolo del bit di parita'
  p = parity32(wri);

  wri += p << 30;
  if ( P > 3 ) printf(" now wri = %08X\n", wri);
  bin_wri = sprintf_binary32(b, wri, -32);

  printf("\n Enabling selected heaters\n");
  if ( P > 0 ) printf("    [code = %s]\n", b);

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  heaters.ena.wri.was = heaters.ena.wri.cod;
  heaters.ena.wri.sta = status_OK;  
  if (err) sta = 0;
  else     sta = 1;

  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      heater_ena_GET_callback(LF_OBJECT*, int)
 * Description:   manage heater read button
 *                heaters.ena.rea.mon = 0 => "R" button disabled
 *
 **************************************************************************************/
void heaters_ena_GET_callback(FL_OBJECT *obj, long data) 
{

  if ( P > 2 ) printf(" *** invoking heaters_ena_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    // se abbassato, tira su il pulsante col click sx.
    if (heaters.ena.rea.mon) {
      heaters.ena.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    // se il pulsante e' abilitato, esegue la lettura dello stato
    else {
      msg = -1;
      update_objects();
      read_heaters_ena_data();
      msg = 1;  
      update_objects();
    }
  }

  // right-click disabilita il pulsante (o lo riabilita)
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    heaters.ena.rea.mon = 1 - heaters.ena.rea.mon;
    update_objects();
  }
}



/****************************************************************************************************
 *
 * Function:      read_heaters_ena_data(void)
 * Description:   return heaters enable status
 *                code to ask: 1100 1011 1100 0000 0000 0000 0000 0000
                                (C)  (B)  (C)  (0)
 *                answer:      ---- ---- ---- ---- H1.H2.H3.H4 H5.000 0000 0000
 *                                                 |
 *                                                 +----> heaters.ena.rea.cod
 *
 ****************************************************************************************************/
void read_heaters_ena_data(void) 
{

  int32 wri = 0xCBC00000, rea, mask;
  chk = 1;
  int la, lb;
  int16 err;
  char a[80], b[80];

  if ( P > 2 ) printf(" *** invoking read_heaters_ena_data\n");

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  //rea = 0x9148;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = 0xF800;
  rea &= mask;

  la = sprintf_binary32(a,rea,-32);

  //heaters.ena.rea.cod = rea >> 4;
  heaters.ena.rea.cod = rea;
  printf("\n Reading heaters enable status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", heaters.ena.rea.cod);

  lb = sprintf_binary32(b,heaters.ena.rea.cod,-32);
  if ( P > 3 ) printf(" heaters.ena.rea.cod = %08X = %s\n", heaters.ena.rea.cod, b);

  heaters.ena.rea.sta = status_OK;  // controllare il significato di questa variabile

}


/**************************************************************************************
 *
 * Function:      show_heater_panel(void)
 * Description:   show heaters panel
 *
 **************************************************************************************/
void show_heater_panel(void) 
{

  FL_OBJECT *obj;
  char b[80];
  int16 m, m1, m2;
  int i;
  
  if ( P > 2 ) printf(" *** invoking show_heater_panel\n");

  for (i=0; i<n_heaters; i++) {
    // tastini senza label 
    // cambiano colore quando viene variato lo stato di abilitazione 
    // finche' non si preme W
    obj = heater[i].WAS;
    m1 = heaters.ena.wri.was & heater[i].msk;    // stato precedente dell'heater
    m2 = heaters.ena.wri.cod & heater[i].msk;    // stato impostato per l'heater
    if ((heaters.ena.wri.sta == status_OK) && (m1 == m2)) {
      fl_set_object_color(obj, FL_PALEGREEN, 0);
    }
    else {
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
  }
  
  // colonna 'Enable'
  for (i=0; i<n_heaters; i++) {
    obj = heater[i].WRI;
    m = heater[i].msk & heaters.ena.wri.cod;
    if (m == heater[i].msk) {
      fl_set_object_color(obj, FL_PALEGREEN, FL_PALEGREEN);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "E");
    }
    else if (m == 0) {
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_WHEAT);
      fl_set_object_label(obj, "D");
    }
    else {
      fl_set_object_color(obj, FL_DARKGOLD, FL_DARKGOLD);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "?");
    }
  }
  
  for (i=0; i<n_heaters; i++) {
    obj = heater[i].REA;
    m  = heater[i].msk & heaters.ena.rea.cod;
    m1 = heaters.ena.wri.was & heater[i].msk;
    m2 = heaters.ena.rea.cod & heater[i].msk;
    if (heaters.ena.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
    }
    else {
      if (m == heater[i].msk) fl_set_object_label(obj, "E");
      else if (m == 0)       fl_set_object_label(obj, "D");
      else                   fl_set_object_label(obj, "?");
    }
    if (heaters.ena.rea.sta == status_OK) {
      if (m1 == m2) fl_set_object_color(obj, FL_PALEGREEN, 0);
      else          fl_set_object_color(obj, FL_DARKGOLD, 0);
      if (m != heater[i].msk && m != 0) fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (heaters.ena.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (heaters.ena.rea.sta == status_NA ||
        heaters.ena.rea.sta == status_NN) {
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }

  // colonna "time"  
  for (i=0; i<n_heaters; i++) {
    obj = heater[i].VAL;
    sprintf(b, "%6.1f", heater[i].wri.val);
    if (i != heater_INP_active) {
      fl_set_object_label(obj, b);
      if (heater[i].wri.sta == status_OK)
        fl_set_object_color(obj, FL_PALEGREEN, FL_RED);
      else
        fl_set_object_color(obj, FL_MCOL, FL_RED);
        fl_set_object_lcolor(obj, FL_WHITE);
    }
  }
  
  
  obj = heaters.ena.GET;
  if (heaters.ena.rea.mon) {
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_DARKGOLD);
  }
  else {
    fl_set_object_boxtype(obj, FL_UP_BOX);
    fl_set_object_color(obj, FL_MCOL, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }


}


/**************************************************************************************
 *
 * Function:      create_sensor_panel(int, int)
 * Description:   create Gp50 pressure and temperature sensors panel
 *
 **************************************************************************************/
void create_sensor_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 55, w2 = 25, w3 = 68, h = 25;
  
  if ( P > 2 ) printf(" *** invoking creat_sensor_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+2*w3+2*w2,2+10+(2+n_sensor)*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+2*w3+2*w2, h, "GP50 PRESSURE SENSORS", size1);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, h, " ", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w2, h, " ", size1);
  x = x + w2;
   obj = add_frame_box(x, y, w3, h, "Press [mV]", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w3, h, "Press [psi]", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w2, h, " ", size1);
  y = y + h;
  for (i=0; i<n_sensor; i++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wa, h, sensor[i].nam, size1);
    x = x + wa;
    sensor[i].CVT =
      obj = add_normal_button(x, y, w2, h, "C", size1);
    fl_set_object_callback(obj, sensor_CVT_callback, i);
    x = x + w2;
    sensor[i].VAL =
      obj = add_data_box(x, y, w3, h, "1234.5", size2);
    x = x + w3;
    sensor[i].PSIA =
      obj = add_data_box(x, y, w3, h, "1234.5", size2);
    x = x + w3;
    sensor[i].GET =
      obj = add_normal_button(x, y, w2, h, "R", size1);
    fl_set_object_callback(obj, sensor_GET_callback, i);
    y = y + h;
  }
}

/**************************************************************************************
 *
 * Function:      sensor_CVT_callback(FL_OBJECT*, long)
 * Description:   manage Gp50 PRESSURE sensors convert button
 *
 **************************************************************************************/
void sensor_CVT_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking sensor_CVT_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (sensor[ind].rea.mon) {
      sensor[ind].rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      convert_sensor(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    sensor[ind].rea.mon = 1 - sensor[ind].rea.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      convert_sensor(int)
 * Description:   convert Gp50 PRESSURE sensors
 *
 **************************************************************************************/
void convert_sensor(int ind) 
{

  int32 rea, wri = 0x0;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking convert_sensor\n");

  if (ind == 0) {           
    wri = 0x8FC88000;       // P1A
  } else if (ind == 1) {
    wri = 0x8FC84000;       // P1B
  } else if (ind == 2) {
    wri = 0x8FC82000;       // P2A       
  } else if (ind == 3) {
    wri = 0x8FC81000;       // P2B          
  }

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;

  printf("\n Converting GP50 pressure sensor %s\n", sensor[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", wri);

  //sensor[ind].rea.sta = status_OK;
}



/**************************************************************************************
 *
 * Function:      sensor_GET_callback(FL_OBJECT*, long)
 * Description:   manage Gp50 PRESSURE sensors read button
 *
 **************************************************************************************/
void sensor_GET_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking sensor_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (sensor[ind].rea.mon) {
      sensor[ind].rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      read_sensor_data(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    sensor[ind].rea.mon = 1 - sensor[ind].rea.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      tsensor_GET_callback(FL_OBJECT*, long)
 * Description:   manage Gp50 TEMPERATURE sensors read button
 *
 **************************************************************************************/
void tsensor_GET_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking sensor_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (tsensor[ind].rea.mon) {
      tsensor[ind].rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      read_tsensor_data(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    tsensor[ind].rea.mon = 1 - tsensor[ind].rea.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      read_sensor_data(int)
 * Description:   read Gp50 PRESSURE sensors read button
 *                answer in bit 17 - 32, con 17 = LSB
 *                ---- ---- ---- ---- PPPP PPPP PPPP PPPP
 *                                  (LSB)    --->     (MSB)
 *
 **************************************************************************************/
void read_sensor_data(int ind) 
{

  char a[80], b[80];
  int32 rea, wri = 0x0, mask;
  chk = 1;
  int16 err, p_bit;
  int la, lb;

  if ( P > 2 ) printf(" *** invoking read_sensor_data\n");

  if (ind == 0) {           
    //wri = 0xCDC00000;       // T2A used as P1A
    wri = 0x86C00000;       // P1A
  } else if (ind == 1) {
    wri = 0xC1C00000;       // P1B
  } else if (ind == 2) {
    wri = 0xCEC00000;       // P2A       
  } else if (ind == 3) {
    wri = 0x89C00000;       // P2B          
  }

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;

  //rea = 0xA430;             // ATENZIONE solo per debug!

  mask = 0xFFF0;
  rea &= mask;
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" wri = %08X, rea = %08X = %s\n", wri, rea, a);
  p_bit = (int16)rea;
  p_bit = invert_bit_order(p_bit);
  lb = sprintf_binary(b,p_bit,-16);
  if ( P > 3 ) printf(" p_bit = %04X = %s, val in bit = %10.3f\n", p_bit, b, (float)p_bit);

  printf("\n Reading GP50 pressure sensor %s\n", sensor[ind].nam);
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", p_bit);

  sensor[ind].rea.val = (float)p_bit*5000./4096.;

  sensor[ind].rea.psia = pressure_psia(ind, sensor[ind].rea.val);

  printf(" Pressure [mV] = %6.3f, Pressure [psia] = %6.3f\n",sensor[ind].rea.val,sensor[ind].rea.psia);
  sensor[ind].rea.sta = status_OK;
}


/**************************************************************************************
 *
 * Function:      read_tsensor_data(int)
 * Description:   read Gp50 TEMPERATURE sensors read button
 *                answer in bit 17 - 32, con 17 = LSB
 *                ---- ---- ---- ---- TTTT TTTT TTTT TTTT
 *                                  (LSB)    --->     (MSB)
 *
 **************************************************************************************/
void read_tsensor_data(int ind) 
{

  char a[80], b[80];
  int32 rea, wri = 0x0, mask;
  chk = 1;
  int la, lb;
  int16 err, p_bit;

  if ( P > 2 ) printf(" *** invoking read_sensor_data\n");

  if (ind == 0) {              // T1A
    wri = 0xCEC00000;
  } else if (ind == 1) {       // T1B
    wri = 0x89C00000;
  } else if (ind == 2) {       // T2A
    wri = 0xCDC00000;          
  } else if (ind == 3) {       // T2B
    wri = 0xCBC00000;          
  }

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;

  //rea = 0xc210;             // ATTENZIONE solo per debug!

  mask = 0xFFF0;
  rea &= mask;
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  p_bit = (int16)rea;
  p_bit = invert_bit_order(p_bit);
  lb = sprintf_binary(b,p_bit,-16);
  if ( P > 3 ) printf(" p_bit = %04X = %s, val in bit = %10.3f\n", p_bit, b, (float)p_bit);

  printf("\n Reading temperature sensor %s\n", tsensor[ind].nam);
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", p_bit);

  tsensor[ind].rea.val = (float)p_bit*5000./4096.;  // CHIARIRE FORMULA DI CONVERSIONE!!!!!!!
  tsensor[ind].rea.deg = temp_degrees(ind, tsensor[ind].rea.val);
  printf(" Temperature [mV] = %6.3f, Temperature [K] = %6.3f\n",tsensor[ind].rea.val,tsensor[ind].rea.deg);
  tsensor[ind].rea.sta = status_OK;
}


/**************************************************************************************
 *
 * Function:      pressure_psia(int, float) 
 * Description:   convert Gp50 mVolt into psia
 * Controllare se il range di pressione corrisponde alla conversione usata...
 * In box S: 
 * P1a ->  4.497 V = 3000 psia
 * P1b ->  4.495 V = 3000 psia
 * P2a ->  4.495 V = 300 psia
 * P2b ->  4.495 V = 300 psia
 * subtracting pedestal
 * P1a = (V2a-0.020)*3000/(4.497)
 * P1b = (V2b-0.020)*3000/(4.495)
 * P2a = (V2a-0.020)*300 /(4.495)
 *
 **************************************************************************************/
float pressure_psia(int ind, float p_mV) 
{

  float conv_factor = 0.0;            // modified by AL to remove warning
  float p_psia = 0.;

  if ( P > 2 ) printf(" *** invoking pressure_psia\n");
  if ( ind == 0) conv_factor = 3000. / (4497.);
  if ( ind == 1) conv_factor = 3000. / (4495.);
  if ( ind == 2 || ind == 3 ) conv_factor = 300. / (4495.);

  p_psia = (p_mV-20) * conv_factor; //(psia/Volt)
  if ( P > 3 ) printf(" ind = %d, p_mV = %f, conv_fact = %f, p_psia = %f\n",ind,p_mV,conv_factor,p_psia);

  return p_psia;

}



/**************************************************************************************
 *
 * Function:      temp_degrees(int, float)
 * Description:   convert Gp50 mVolt into Kelvin
 * 
 *
 **************************************************************************************/
float temp_degrees(int ind, float t_mV) {

  float conv_factor = 0.0;            // modified by AL to remove warning
  float t_deg;

  if ( ind == 0 || ind == 1 ) conv_factor = 22.24875/1000.;
  if ( ind == 2 || ind == 3 ) conv_factor = 22.24875/1000.;

  if ( P > 2 ) printf(" *** invoking temp_degrees\n");
  t_deg = t_mV * conv_factor;
  return t_deg;

}


/**************************************************************************************
 *
 * Function:      show_sensor_panel(void) 
 * Description:   show Gp50 pressure and temperature sensors panel
 *
 **************************************************************************************/
void show_sensor_panel(void) 
{

  FL_OBJECT *obj;
  char b[80];
  int i;

  if ( P > 2 ) printf(" *** invoking show_sensor_panel\n");
 // settaggi per la finestrella della pressione in mV
  for (i=0; i<n_sensor; i++) {
    obj = sensor[i].VAL;
    if (sensor[i].rea.sta == status_NN || sensor[i].rea.sta == status_NA) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (sensor[i].rea.sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, 0);
    }
    if (sensor[i].rea.sta == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (sensor[i].rea.sta == status_NN) sprintf(b, " ");
    else                                sprintf(b, "%7.0f", sensor[i].rea.val);
    fl_set_object_label(obj, b);
  // settaggi per la finestrella della pressione in psia
    obj = sensor[i].PSIA;
    if (sensor[i].rea.sta == status_NN || sensor[i].rea.sta == status_NA) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (sensor[i].rea.sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, 0);
    }
    if (sensor[i].rea.sta == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (sensor[i].rea.sta == status_NN) sprintf(b, " ");
    else                                sprintf(b, "%7.2f", sensor[i].rea.psia);
    fl_set_object_label(obj, b);
    // settaggi per i tastini "R"
    obj = sensor[i].GET;
    if (sensor[i].rea.mon) {
      fl_set_object_boxtype(obj, FL_DOWN_BOX);
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_DARKGOLD);
    }
    else {
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_MCOL, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    // finestrella temperatura in mV
/*     obj = tsensor[i].VAL; */
/*     if (tsensor[i].rea.sta == status_NN || tsensor[i].rea.sta == status_NA) { */
/*       fl_set_object_lcolor(obj, FL_BLACK); */
/*       fl_set_object_color(obj, FL_WHEAT, 0); */
/*     } */
/*     if (tsensor[i].rea.sta == status_KO) { */
/*       fl_set_object_lcolor(obj, FL_WHITE); */
/*       fl_set_object_color(obj, FL_BLACK, 0); */
/*     } */
/*     if (tsensor[i].rea.sta == status_OK) { */
/*       fl_set_object_lcolor(obj, FL_BLACK); */
/*       fl_set_object_color(obj, FL_WHEAT, 0); */
/*     } */
/*     if (tsensor[i].rea.sta == status_NN) sprintf(b, " "); */
/*     else                                sprintf(b, "%7.2f", tsensor[i].rea.val); */
/*     fl_set_object_label(obj, b); */
    // finestrella temperatura in K
/*     obj = tsensor[i].DEG; */
/*     if (tsensor[i].rea.sta == status_NN || tsensor[i].rea.sta == status_NA) { */
/*       fl_set_object_lcolor(obj, FL_BLACK); */
/*       fl_set_object_color(obj, FL_WHEAT, 0); */
/*     } */
/*     if (tsensor[i].rea.sta == status_KO) { */
/*       fl_set_object_lcolor(obj, FL_WHITE); */
/*       fl_set_object_color(obj, FL_BLACK, 0); */
/*     } */
/*     if (tsensor[i].rea.sta == status_OK) { */
/*       fl_set_object_lcolor(obj, FL_BLACK); */
/*       fl_set_object_color(obj, FL_WHEAT, 0); */
/*     } */
/*     if (tsensor[i].rea.sta == status_NN) sprintf(b, " "); */
/*     else                                 sprintf(b, "%7.2f", tsensor[i].rea.deg); */
/*     fl_set_object_label(obj, b); */
    // tastini "R" della temperatura
/*     obj = tsensor[i].GET; */
/*     if (tsensor[i].rea.mon) { */
/*       fl_set_object_boxtype(obj, FL_DOWN_BOX); */
/*       fl_set_object_color(obj, FL_RED, FL_RED); */
/*       fl_set_object_lcolor(obj, FL_DARKGOLD); */
/*     } */
/*     else { */
/*       fl_set_object_boxtype(obj, FL_UP_BOX); */
/*       fl_set_object_color(obj, FL_MCOL, FL_RED); */
/*       fl_set_object_lcolor(obj, FL_BLACK); */
/*     } */
  }
}


/**************************************************************************************
 *
 * Function:      create_Ksensor_panel(int, int)
 * Description:   create Kulite pressure sensors panel
 *
 **************************************************************************************/
void create_Ksensor_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size3 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 55, w2 = 25, w3 = 68, h = 25;
  
  if ( P > 2 ) printf(" *** invoking creat_Ksensor_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+2*w3+2*w2,2+10+5*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+2*w3+2*w2, h, "KULITE PRESSURE SENSORS", size3);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, h, " ", size1);
  x = x + wa;
/*   obj = add_frame_box(x, y, w2, h, " ", size1); */
/*   x = x + w2; */
  obj = add_frame_box(x, y, w2, h, " ", size1);
  x = x + w2;
  obj = add_frame_box(x, y, w3, h, "Press [mV]", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w3, h, "Press [bar]", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w2, h, " ", size1);
  y = y + h;
  for (i=0; i<n_Ksensor; i++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wa, h, Ksensor[i].nam, size1);
    x = x + wa;
    Ksensor[i].CVTS =
      obj = add_normal_button(x, y, w2, h, "CS", size1);
    fl_set_object_callback(obj, Ksensor_CVTS_callback, i);
    x = x + w2;
/*     Ksensor[i].CVTR = */
/*       obj = add_normal_button(x, y, w2, h, "CR", size1); */
/*     fl_set_object_callback(obj, Ksensor_CVTR_callback, i); */
/*     x = x + w2; */
    Ksensor[i].VAL =
      obj = add_data_box(x, y, w3, h, "1234.5", size2);
    x = x + w3;
    Ksensor[i].BAR =
      obj = add_data_box(x, y, w3, h, "1234.5", size2);
    x = x + w3;
    Ksensor[i].GET =
      obj = add_normal_button(x, y, w2, h, "R", size3);
    fl_set_object_callback(obj, Ksensor_GET_callback, i);
    y = y + h;
  }
}



/**************************************************************************************
 *
 * Function:      Ksensor_CVTS_callback(FL_OBJECT*, long)
 * Description:   manage kulite signal PRESSURE sensors convert button
 *
 **************************************************************************************/
void Ksensor_CVTS_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking sensor_CVTS_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (Ksensor[ind].rea.mon) {
      Ksensor[ind].rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      convert_KsensorS(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    Ksensor[ind].rea.mon = 1 - Ksensor[ind].rea.mon;
    update_objects();
  }
}



/**************************************************************************************
 *
 * Function:      convert_KsensorS(int)
 * Description:   convert kulite signal PRESSURE sensors
 *
 **************************************************************************************/
void convert_KsensorS(int ind) 
{

  int32 rea, wri = 0x0;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking convert_KsensorS\n");

  if (ind == 0) {           
    wri = 0x8FC80800;       // Pk1c
  } else if (ind == 1) {
    wri = 0x8FC80400;       // Pk2c
  } else if (ind == 2) {
    wri = 0x8FC80200;       // Pk1d   
  }

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;

  printf("\n Converting Kulite pressure sensor %s (signal)\n", Ksensor[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", wri);

  //sensor[ind].rea.sta = status_OK;
}



/**************************************************************************************
 *
 * Function:      Ksensor_CVTR_callback(FL_OBJECT*, long)
 * Description:   manage kulite reference PRESSURE sensors convert button
 *
 **************************************************************************************/
void Ksensor_CVTR_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking sensor_CVTR_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (Ksensor[ind].rea.mon) {
      Ksensor[ind].rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      convert_KsensorR(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    Ksensor[ind].rea.mon = 1 - Ksensor[ind].rea.mon;
    update_objects();
  }
}



/**************************************************************************************
 *
 * Function:      convert_KsensorR(int)
 * Description:   convert kulite reference PRESSURE sensors
 *
 **************************************************************************************/
void convert_KsensorR(int ind) 
{

  int32 rea, wri = 0x0;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking convert_KsensorR\n");

  if (ind == 0) {           
    wri = 0x8FC80100;       // Pk1c
  } else if (ind == 1) {
    wri = 0x8FC80080;       // Pk2c
  } else if (ind == 2) {
    wri = 0x8FC80040;       // Pk1d   
  }

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;

  printf("\n Converting Kulite pressure sensor %s (reference)\n", Ksensor[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", wri);

  //sensor[ind].rea.sta = status_OK;
}




/**************************************************************************************
 *
 * Function:      Ksensor_GET_callback(FL_OBJECT*, long)
 * Description:   manage Kulite sensors read button
 *
 **************************************************************************************/
void Ksensor_GET_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking Ksensor_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (Ksensor[ind].rea.mon) {
      Ksensor[ind].rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      read_Ksensor_data(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    Ksensor[ind].rea.mon = 1 - Ksensor[ind].rea.mon;
    update_objects();
  }
}



/**************************************************************************************
 *
 * Function:      read_Ksensor_data(int)
 * Description:   read Kulite sensors read button
 *                answer in bit 17 - 32, con 17 = LSB
 *                ---- ---- ---- ---- PPPP PPPP PPPP PPPP
 *                                  (LSB)    --->     (MSB)
 *
 **************************************************************************************/
void read_Ksensor_data(int ind)         // modified by AL
{

  char as[80], bs[80];
//char ar[80], br[80];
  int32 mask;
  int32 reas, wris = 0x0;
//int32 rear, wrir = 0x0;
  chk = 1;
  int16 err;
  int16 p_s_bit;
//int16 p_r_bit;
  int las, lbs;
//int lar, lbr;
  float k_sig;
//float k_ref;

  if ( P > 2 ) printf(" *** invoking read_Ksensor_data\n");

  if (ind == 0) {              // Pk1C (sig + ref)
    wris = 0x85C00000;
//  wrir = 0xCC400000;
  } else if (ind == 1) {       // Pk2C
    wris = 0xCDC00000;
//  wrir = 0x82400000;
  } else if (ind == 2) {       // Pk1D
    wris = 0x83C00000;
//  wrir = 0xC6400000;
  }

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wris, &reas, &chk, &err);
//exec_LeCroy_cmd(n_CMDS, &bus, &wrir, &rear, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;

  // rea = 0xA430;             // ATTENZIONE solo per debug!

  mask = 0xFFF0;
  reas &= mask;
//rear &= mask;
  las = sprintf_binary32(as,reas,-32);
//lar = sprintf_binary32(ar,rear,-32);
  if ( P > 3 ) printf(" reas = %08X = %s\n", reas, as);
//if ( P > 3 ) printf(" rear = %08X = %s\n", rear, ar);
  p_s_bit = (int16)reas;
//p_r_bit = (int16)rear;
  p_s_bit = invert_bit_order(p_s_bit);
//p_r_bit = invert_bit_order(p_r_bit);
  lbs = sprintf_binary(bs,p_s_bit,-16);
//lbr = sprintf_binary(br,p_r_bit,-16);
  if ( P > 3 ) printf(" p_s_bit = %04X = %s, val in bit = %10.3f\n", p_s_bit, bs, (float)p_s_bit);
//if ( P > 3 ) printf(" p_r_bit = %04X = %s, val in bit = %10.3f\n", p_r_bit, br, (float)p_r_bit);

  printf("\n Reading Kulite pressure sensor %s\n", Ksensor[ind].nam);
  if ( P > 0 ) printf("    [code in answer = 0x%04X (signal)]\n", p_s_bit);
//if ( P > 0 ) printf("    [               = 0x%04X (reference)]\n", p_r_bit);

  k_sig = (float)p_s_bit*5000./4096.;
//k_ref = (float)p_r_bit*5000./4096.;
//Ksensor[ind].rea.val = k_sig - k_ref; */
  Ksensor[ind].rea.val = k_sig;
  Ksensor[ind].rea.bar = Kpressure_bar(ind, Ksensor[ind].rea.val);
  printf(" Pressure [mV] = %6.3f, Pressure [bar] = %6.3f\n",Ksensor[ind].rea.val,Ksensor[ind].rea.bar);
  Ksensor[ind].rea.sta = status_OK;

}




/**************************************************************************************
 *
 * Function:      Kpressure_bar(int, float) 
 * Description:   convert Kulite mVolt into bar
 // Controllare se il range di pressione corrisponde alla conversione usata...
 // In box S:
 // P2a=V2a*300 /(4.517*14.5) (bar/Volt)
 // P1a=V2a*3000/(4.515*14.5) (bar/Volt)
 // P1b=V2b*3000/(4.515*14.5) (bar/Volt)
 *
 **************************************************************************************/
float Kpressure_bar(int ind, float p_mV) 
{

  const float conv_factor = 300./(4.515*14.5)/1000.;
  float p_bar;

  if ( P > 2 ) printf(" *** invoking Kpressure_bar\n");
  p_bar = p_mV * conv_factor; //(bar/Volt)
  return p_bar;

}


/**************************************************************************************
 *
 * Function:      show_Ksensor_panel(void) 
 * Description:   show Kulite sensors panel
 *
 **************************************************************************************/
void show_Ksensor_panel(void) 
{

  FL_OBJECT *obj;
  char b[80];
  int i;

  if ( P > 2 ) printf(" *** invoking show_Ksensor_panel\n");
 // settaggi per la finestrella della pressione in mV
  for (i=0; i<n_Ksensor; i++) {
    obj = Ksensor[i].VAL;
    if (Ksensor[i].rea.sta == status_NN || Ksensor[i].rea.sta == status_NA) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (Ksensor[i].rea.sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, 0);
    }
    if (Ksensor[i].rea.sta == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (Ksensor[i].rea.sta == status_NN) sprintf(b, " ");
    else                                sprintf(b, "%7.0f", Ksensor[i].rea.val);
    fl_set_object_label(obj, b);
  // settaggi per la finestrella della pressione in bar
    obj = Ksensor[i].BAR;
    if (Ksensor[i].rea.sta == status_NN || Ksensor[i].rea.sta == status_NA) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (Ksensor[i].rea.sta == status_KO) {
      fl_set_object_lcolor(obj, FL_WHITE);
      fl_set_object_color(obj, FL_BLACK, 0);
    }
    if (Ksensor[i].rea.sta == status_OK) {
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    if (Ksensor[i].rea.sta == status_NN) sprintf(b, " ");
    else                                sprintf(b, "%7.2f", Ksensor[i].rea.bar);
    fl_set_object_label(obj, b);
    // settaggi per i tastini "R"
    obj = Ksensor[i].GET;
    if (Ksensor[i].rea.mon) {
      fl_set_object_boxtype(obj, FL_DOWN_BOX);
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_DARKGOLD);
    }
    else {
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_MCOL, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
}


/**************************************************************************************
 *
 * Function:      show_message(void) 
 * Description:   show the send command message
 *
 **************************************************************************************/
void show_message(void) 
{

  FL_OBJECT *obj;

  if ( P > 2 ) printf(" *** invoking show_message\n");

  if ( P > 3 ) printf(" *** msg = %d,   sta = %d\n", msg, sta);

  obj = MESSAGE;
  if (msg) {
    if (msg < 0) {
      fl_set_object_color(obj,FL_BLACK,FL_MCOL);
      fl_set_object_lcolor(obj,FL_WHITE);
      fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
      fl_set_object_label(obj,"Command is being sent...");
    }
    else {
      if (sta) {
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lcolor(obj,FL_PALEGREEN);
        if (chk) fl_set_object_label(obj,"Command executed O.K., but...");
        else     fl_set_object_label(obj,"Command executed O.K.");
      }
      else {
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lcolor(obj,FL_RED);
        fl_set_object_label(obj,"Command execution failed!");
      }
    }
    fl_show_object(obj);
  }
  else {
    fl_hide_object(obj);
  }

}


/**************************************************************************************
 *
 * Function:      update_objects(void) 
 * Description:   redisplay the boxes
 *
 **************************************************************************************/
void update_objects(void) 
{

  if ( P > 2 ) printf(" *** invoking update_objects\n");

/*   update_USCM_JMDC_panels(); */
  show_HOTCOLD_panel();

  /* Tensions */
/*   show_tension_panel(); */
  /* Marotta */
  show_Mvalve_panel();
  /* Heaters */
  show_heater_panel();
  /* Gp50 */
  show_sensor_panel();
  /* Kulite */
  show_Ksensor_panel();

  show_message();
  fl_check_forms();

}


/**************************************************************************************
 *
 * Function:      invalidate_data(void) 
 * Description:   set initial values
 *
 **************************************************************************************/
void invalidate_data(void)
{
  int i;

  if ( P > 2 ) printf(" *** invoking invalidate_data\n");


  /* Tensions */
  for (i=0; i<n_tensions; i++) {
    tension[i].wri.sta = status_NN;
  }

  tensions.ena.wri.cod = 0xA000;
  tensions.ena.wri.sta = status_NN;
  tensions.ena.rea.mon = 0;
  tensions.ena.rea.sta = status_NN;

  /* Marotta */
  for (i=0; i<n_marotta; i++) {
    valve[i].wri.sta = status_NN;
  }
  valves.ena.wri.cod = 0;
  valves.ena.wri.sta = status_NN;
  valves.ena.rea.mon = 0;
  valves.ena.rea.sta = status_NN;
  valves.sta.rea.mon = 0;
  valves.sta.rea.sta = status_NN;

  /* Heaters */
  for (i=0; i<n_heaters; i++) {
    valve[i].wri.sta = status_NN;
  }
  heaters.ena.wri.cod = 0;
  heaters.ena.wri.sta = status_NN;
  heaters.ena.rea.mon = 0;
  heaters.ena.rea.sta = status_NN;

  /* Gp50 */
  for (i=0; i<n_sensor; i++) {
    sensor[i].rea.mon = 0;
    sensor[i].rea.sta = status_NN;
    tsensor[i].rea.sta = status_NN;
  }

  /* Gp50 */
  for (i=0; i<n_Ksensor; i++) {
    Ksensor[i].rea.mon = 0;
    Ksensor[i].rea.sta = status_NN;
  }

}



/*********************************************************************
 *
 * Function:      main()
 *
 *********************************************************************/
int main(int argc, char *argv[]) {

  ESPIONAGE = TRUE;

  printf("\033[2J\n");
  printf("\033[1;0H\n ************************************************************************\n");
  printf(" **********          This is the controller for Box S         ***********\n");
  printf(" ************************************************************************\n");
  template(argc, &argv[0]);
  
  return 0;
}

/* int main (int argc, char **argv) */
/* { */
/*   printf("\033[2J\n"); */
/*   printf("\033[1;0H\n ************************************************************************\n"); */
/*   printf(" **********          This is the controller for Box S         ***********\n"); */
/*   printf(" ************************************************************************\n"); */
/*   printf("\n Usage: boxS [ Debug_Level ]\n"); */
/*   P = 0; */
/*   if (argc > 1) P = atoi(argv[1]); */
/*   printf("\n Debug_Level = %d\n", DEBUG_LEVEL); */
/*   printf("\n Using default initialization: clock = 0x%02X\n", clk); */
/*   printf("                               data  = 0x%04X\n\n", dat); */
/*   P = DEBUG_LEVEL; */
/*   start_boxs(); */
/*   while ((but = fl_do_forms()) != yes) */
/*     ; */
/*   exit(2); */
/*   return 0; */
/* } */

