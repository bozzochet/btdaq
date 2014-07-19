// file boxC.c
//
// modified by AL to remove compiler warnings
//
/*********************************************************************
 *
 * Filename:     boxC.c
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
char *title  = {"UGBC Controller"};

#ifdef USE_CAN
#include "uscmlib.h"
int JMDC_ID = 0x01;
int USCM_ID = 0x4E;

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

/* void start_boxc(void); */

FL_OBJECT *yes, *but;





/*********************************************************************
 * Type definition, structures, initializations
 *********************************************************************/

int        msg = 0;
int16      sta;
bool       chk;
FL_OBJECT* MESSAGE;

typedef struct {
  FL_FORM* boxc;
  void* vdata;
  long ldata;
} FD_BOXC;

FD_BOXC*   boxc;

/* Tensions */
#define n_tensions 4

struct {
  char* nam;
  int16 msk;
  struct {
    int16 cod;
    float val;
    int   sta;
  } wri;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
} tension[n_tensions] = {
  {"25V",   0x8000},  // 1 abilita (!!)
  {"12V",   0x4000},  // 1 disabilita
  {"MCA",   0x2000},  // 1 disabilita
  {"CO2",   0x1000},  // 1 disabilita
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


/* RS232 */
#define n_serials 4

struct {
  char* nam;
  int16 msk;
  struct {
    int16 cod;
    float val;
    int   sta;
  } wri;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
} serial[n_serials] = {
  {"RS232_1",   0x8000},
  {"RS232_2",   0x4000},
  {"RS232_3",   0x2000},
  {"RS232_4",   0x1000}
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
} serials;

int serial_INP_active;


/* Marotta valves */
#define n_marotta 6

struct {
  char* nam;
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
  {"V6a",       0x8000},
  {"V6b",       0x4000},
  {"V18a",      0x2000}, 
  {"V18b",      0x1000},
  {"V6a&V18a",  0xA000},
  {"V6b&V18b",  0x5000},
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

/* Circulation pumps */
#define n_pumps 2

struct {
  char* nam;
  int16 msk;
  int16 smsk;
  struct {
    int16 cod;
    float val;
    int   sta;
  } wri;
  FL_OBJECT *WAS;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
  FL_OBJECT *SWAS;
  FL_OBJECT *SPE;
  FL_OBJECT *SREA;
  FL_OBJECT *OPN;
  FL_OBJECT *CLS;
  FL_OBJECT *ASTA;
  FL_OBJECT *SSTA;
} pump[n_pumps] = {
  {"CP1",   0x8000, 0x2000},
  {"CP2",   0x4000, 0x1000},
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
      int16 was;
      int16 cod;
      int   sta;
    } wri;
    struct {
      int   mon;
      int16 cod;
      int   sta;
    } rea;
  } spe;
  struct {
    struct {
      int   mon;
      int16 acod;
      int16 scod;
      int   sta;
    } rea;
    FL_OBJECT *GET;
  } sta;
} pumps;

int pump_INP_active;

/* Flipper valves */
#define n_Fvalve 2

struct {
  char* nam;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int   mon;
    int   sta;
  } cls;
  FL_OBJECT *OPN;
  FL_OBJECT *CLS;
} Fvalve[n_Fvalve] = {
  {"V8a"},
  {"V8b"},
};


/* Gp50 pressure sensors */
#define n_sensor 2

struct {
  char* nam;
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
  {"P3"},
  {"P4"},
};

struct {
  char* nam;
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
  {"T3"},
  {"T4"},
};

/* Multi Channel Analizer */
#define n_channels 4

struct {
  char* nam;
  int16 msk;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int16 cod;
    int   mon;
  } rea;
  FL_OBJECT *OPN;
  FL_OBJECT *STA;
} channel[n_channels] = {
  {"Ch1", 0x8000},
  {"Ch2", 0x4000},
  {"Ch3", 0x2000},
  {"Ch4", 0x1000}
};

struct {
  struct {
    int16 cod;
    int   sta;
  } rea;
  FL_OBJECT *CHK;
} channels;


void     create_main_form(void);
/* FD_BOXC* create_form_boxc(); */
void     open_map_callback(FL_OBJECT*, long);

/* Tensions */
void     create_tension_panel(int, int);
void     tension_WRI_callback(FL_OBJECT*, long);
void     tensions_ena_SET_callback(FL_OBJECT*, long);
void     tensions_ena_GET_callback(FL_OBJECT*, long);
void     read_tensions_ena_data(void);
void     read_tensions_sta_data(void);
void     show_tension_panel(void);

/* RS232 */
void     create_serial_panel(int, int);
void     serial_WRI_callback(FL_OBJECT*, long);
void     serials_ena_SET_callback(FL_OBJECT*, long);
void     serials_ena_GET_callback(FL_OBJECT*, long);
void     read_serials_ena_data(void);
void     read_serials_sta_data(void);
void     show_serial_panel(void);

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

/* Circulation pumps */
void     create_pump_panel(int, int);
void     pump_WRI_callback(FL_OBJECT*, long);           // "Enable"
void     pump_SPE_callback(FL_OBJECT*, long);           // "Speed"
void     pump_OPN_callback(FL_OBJECT*, long);           // "Open"
void     pump_CLS_callback(FL_OBJECT*, long);           // "Close"
void     pumps_ena_SET_callback(FL_OBJECT*, long);
void     pumps_ena_GET_callback(FL_OBJECT*, long);
void     pumps_sta_GET_callback(FL_OBJECT*, long);
void     read_pumps_ena_data(void);
void     read_pumps_sta_data(void);
void     show_pump_panel(void);

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

/* Flipper valves */
void     create_Fvalve_panel(int, int);
void     Fvalve_OPN_callback(FL_OBJECT*, long);
void     Fvalve_CLS_callback(FL_OBJECT*, long);
void     open_Fvalve(int);
void     close_Fvalve(int);
void     show_Fvalve_panel(void);

/* MCA */
void     create_channel_panel(int, int);
void     channel_OPN_callback(FL_OBJECT*, long);
void     channel_CHK_callback(FL_OBJECT*, long);
void     open_channels(int);
void     check_channels(void);
void     show_channel_panel(void);

/* Common commands */
void     invalidate_data(void);
void     update_objects(void);
void     show_message(void);



/*********************************************************************
 *
 * Function:      start_boxc()
 * Description:   xforms initialization 
 *
 *********************************************************************/
/* void start_boxc(void)  */
/* {  */

/*   int one = 1; */
/*   static char* title = {"Box C Controller"}; */
/*   char* adr[] = {(char*)&title}; */
/*   if ( P > 2 ) printf(" *** invoking start_boxc\n"); */
/*   fl_initialize(&one, adr, " ", 0, 0); */
/*   boxc = create_form_boxc(); */
/*   //fl_ringbell(100); */
/*   fl_set_form_position(boxc->boxc, 0, 0); */
/*   fl_show_form(boxc->boxc, FL_PLACE_GEOMETRY, FL_FULLBORDER, "Box C"); */
/*   update_objects(); */
/* } */


/*********************************************************************
 *
 * Function:      create_form_boxc(int)
 * Description:   creation of the external box 
 *
 *********************************************************************/
/* FD_BOXC* create_form_boxc() */
void create_main_form(void)
{

  int WW = 700, HH = 495;
  int x_tens    =   5, y_tens    =  60;
  int x_ser     = 180, y_ser     =  60;
  int x_pump    = 375, y_pump    =  60;
  int x_marotta =   5, y_marotta = 230;
  int x_gp50    = 375, y_gp50    = 235;
/*   int x_flip    = 375, y_flip    = 235; */
  int x_mca     = 375, y_mca     = 360;
/*   int x_hc = 220, y_hc = 0; */

  FL_OBJECT *obj;  
/*   FD_BOXC* fdui = (FD_BOXC*) fl_calloc(1, sizeof(*fdui)); */

  if ( P > 2 ) printf(" *** invoking create_form_boxc\n");

  invalidate_data();
#ifdef USE_CAN
  bgn_form(&MAIN, 0, 0, WW, HH);
#endif

/*   fdui->boxc = fl_bgn_form(FL_NO_BOX,w,h); */
/*   obj = fl_add_box(FL_FLAT_BOX,0,0,w,h," "); */
/*    obj = fl_add_box(FL_NO_BOX,0,0,w,50,"UGBC Controller"); */
/*   fl_set_object_lsize(obj,FL_LARGE_SIZE); */
/*   fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE); */
/*   fl_set_object_lcolor(obj,FL_WHITE); */
/*   create_USCM_JMDC_panels(); */

  create_HOTCOLD_panel();

  /* Tensions */
  create_tension_panel(x_tens, y_tens);
  /* RS232 */
  create_serial_panel(x_ser, y_ser);
  /* Marotta */
  create_Mvalve_panel(x_marotta, y_marotta);
  /* Pumps */
  create_pump_panel(x_pump, y_pump);
  /* Gp50 */
  create_sensor_panel(x_gp50, y_gp50);
  /* Flipper */
/*   create_Fvalve_panel(x_flip, y_flip); */
  /* MCA */
  create_channel_panel(x_mca, y_mca);

  msg = 0;
  MESSAGE = obj = fl_add_text(FL_NORMAL_TEXT,5,HH-30,WW-50,28," ");
  fl_set_object_lsize(obj,FL_LARGE_SIZE);
  fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);

/*   yes = fl_add_button(FL_NORMAL_BUTTON, 635, 5, 60, 30, "Q\010uit"); */
/*   fl_set_object_lsize(yes,FL_MEDIUM_SIZE); */
/*   fl_set_object_lstyle(yes,FL_BOLD_STYLE); */

/*   obj = fl_add_button(FL_NORMAL_BUTTON, 565, 5, 60, 30, "M\010ap"); */
/*   fl_set_object_lsize(obj,FL_MEDIUM_SIZE); */
/*   fl_set_object_lstyle(obj,FL_BOLD_STYLE); */
/*   fl_set_object_callback(obj, open_map_callback, 1); */

  add_author( WW-30, 35, date, author);

  end_form(MAIN);
  update_objects();

/*   fl_end_form(); */
/*   fdui->boxc->fdui = fdui; */
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
  int w = 495, h = 295;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) 
    {
      mapform = fl_bgn_form(FL_NO_BOX, w, h);
      obj = fl_add_box(FL_FLAT_BOX,0,0,w,h," ");
      obj = fl_add_pixmap(FL_NORMAL_PIXMAP,0,0,w,h," ");
      fl_set_pixmap_file(obj, "schemes/boxC.xpm");
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
  int wa = 40, w1 = 60, h = 22;
  
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

  if (ind != n_tensions-1) {
    tensions.ena.wri.cod ^= tension[ind].msk;
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
 *                enable: 1P00 1000 0010 1000 25V.12V.MCA.CO2 0000 0000 0000
 *                        (8/C) (8)  (2)  (8) +---- (tensions.ena.wri.cod) ----+
 *                                            1on 0on 0on 0on
 *
 **************************************************************************************/
void tensions_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_tensions_bits = 0x85A8;
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
  if ( P > 3 ) printf(" clk = %02X  dat = %04X  ena_tensions_bits = %04X", 
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
  printf("\n clk: %d, dat: %d\n", clk, dat);
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);
  //  init_LeCroy_bus(clk, dat, &err);
  //bus = bus & 0x7F;
  //init_LeCroy_bus(bus, &err);

  mask = 0x7000;
  tensions.ena.wri.was = tensions.ena.wri.cod ^ mask;
  tensions.ena.wri.sta = status_OK;  
  if (err) sta = 0;
  else     sta = 1;
  printf(" chk = %d\n",chk);
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
 *                code: 0x85A00000
 *                answer: 5V.12V.MCA.CO2 ---- ---- ---- 
 *
 **************************************************************************************/
void read_tensions_ena_data(void) 
{

  int32 wri = 0x85A00000, rea, mask, rea_en;
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

  // rea = 0x6000;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  printf("\n Reading tensions enable status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", rea);

/*   mask = 0xF000; */
  mask = 0x7000;
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
/*   int16 mask = 0xF000, code; */
  int16 mask = 0x7000, code = 0x0000;            // modified by AL to remove warning
    
  if ( P > 2 ) printf(" *** invoking show_tension_panel\n");

  // colonna 'Enable'
  for (i=0; i<n_tensions; i++) {
    obj = tension[i].WRI;
/*     code = tensions.ena.wri.cod ^ mask; */
    if ( i == 1 || i == 2 || i == 3) {
      code = tensions.ena.wri.cod ^ mask;
    } else if ( i == 0 ) {
      code = tensions.ena.wri.cod;
    }
    m = tension[i].msk & code;
    if ( P > 3 ) printf(" code = tensions.ena.wri.cod ^ mask  = %08X ^ %08X = %08X \n", 
				tensions.ena.wri.cod, mask, code);
    if ( P > 3 ) printf(" m = tension[i].msk & code  = %08X & %08X = %08X \n", 
				tension[i].msk, code, m);
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
/*     if ( i == 1 || i == 2 || i == 3 ) { */
/*       m  = tension[i].msk & tensions.ena.rea.cod; */
/*       m1 = tensions.ena.wri.was & tension[i].msk; */
/*       m2 = (tensions.ena.wri.cod ^mask)& tension[i].msk; */
/*     } else if ( i == 0 ) { */
/*       m  = tension[i].msk & tensions.ena.rea.cod; */
/*       m1 = tensions.ena.wri.was & tension[i].msk; */
/*       m2 = tensions.ena.wri.cod & tension[i].msk; */
/*     } */

    if ( P > 3 ) printf(" m =  tension[i].msk & tensions.ena.rea.cod = %08X & %08X = %08X \n", 
	   tension[i].msk, tensions.ena.rea.cod, m);
    if ( P > 3 ) printf(" m1 =  tension[i].msk & tensions.ena.rea.was = %08X & %08X = %08X \n", 
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
/*       if (m1 == m2) fl_set_object_color(obj, FL_WHEAT, 0); */
/*       else          fl_set_object_color(obj, FL_WHEAT, 0); */
/*       if (m != tension[i].msk && m != 0) fl_set_object_color(obj, FL_DARKGOLD, 0); */
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
 * Function:      create_serial_panel(int, int)
 * Description:   create serials panel
 *
 **************************************************************************************/
void create_serial_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 60, w1 = 60, h = 22;
  
  if ( P > 2 ) printf(" *** invoking create_serial_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+2*w1,2+10+(n_serials+3)*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+2*w1, h, "SERIAL PORT", size1);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, h, " ", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w1, h, "Enable", size1);
  x = x + w1;
  obj = add_frame_box(x, y, w1, h, "Status", size1);

  y = y + h;
  for (i=0; i<n_serials; i++) {
    x = x0 + 5;
    // colonna col nome delle seriali
    obj = add_frame_box(x, y, wa, h, serial[i].nam, size1);
    x = x + wa;
    // colonna di enable
    serial[i].WRI =
      obj = add_switch_button(x, y, w1, h, "X", size2);
    fl_set_object_callback(obj, serial_WRI_callback, i);
    x = x + w1;
    // colonna lettura stato di abilitazione
    serial[i].REA =
      obj = add_data_box(x, y, w1, h, "Y", size2);
    x = x0 + 5;
    y = y + h;
  }
  
  obj = add_frame_box(x, y, wa, h, " ", size1);
  x = x + wa;
  // colonna di open
  serials.ena.SET =
    obj = add_normal_button(x, y, w1, h, "ENABLE", size1);
  fl_set_object_callback(obj, serials_ena_SET_callback, 0);

  // colonna lettura stato di apertura
  x = x + w1;
  serials.ena.GET =
    obj = add_normal_button(x, y, w1, h, "READ", size1);
  fl_set_object_callback(obj, serials_ena_GET_callback, 0);

}


/**************************************************************************************
 *
 * Function:      serial_WRI_callback(FL_OBJECT*, int)
 * Description:   manage single serial enable/disable button
 *
 **************************************************************************************/
void serial_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int16 m;  

  if ( P > 2 ) printf(" *** invoking serial_WRI_callback\n");

  if (ind != n_serials-1) {
    serials.ena.wri.cod ^= serial[ind].msk;
  }
  else {
    m = serials.ena.wri.cod & serial[n_serials-1].msk;
    if (m == 0 || m == serial[n_serials-1].msk) {
      serials.ena.wri.cod ^= serial[n_serials-1].msk;
    }
    else {
      serials.ena.wri.cod &= ~serial[n_serials-1].msk;
    }
  }

  printf("\n Changing status of serial %s\n", serial[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", serials.ena.wri.cod);  
  msg = 0;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      serial_ena_SET_callback(FL_OBJECT*, int)
 * Description:   callback for serials enable
 *                enable: 1P00 1001 1010 1000 1.2.3.4 0000 0000 0000
 *                        (8/C) (9)  (A)  (8) +---- (serials.ena.wri.cod) ----+
 *
 **************************************************************************************/
void serials_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_serials_bits = 0x89A8;
  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char b[80];

  if ( P > 2 ) printf(" *** invoking serials_ena_SET_callback\n");

  chk = 1;
  msg = -1;
  update_objects();

  wri = ena_serials_bits << 16;
  wri += serials.ena.wri.cod;
  if ( P > 3 ) printf(" clk = %02X  dat = %04X  ena_serials_bits = %04X", 
		    clk, dat, ena_serials_bits);
  if ( P > 3 ) printf(" serials.ena.wri.cod = %08X, wri = %08X\n", 
		    serials.ena.wri.cod, wri);

  // Calcolo del bit di parita'
  p = parity32(wri);

  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);
  bin_wri = sprintf_binary32(b, wri, -32);

  printf("\n Enabling selected serials\n");
  if ( P > 0 ) printf("    [code = %s]\n", b);
  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  serials.ena.wri.was = serials.ena.wri.cod;
  serials.ena.wri.sta = status_OK;  
  if (err) sta = 0;
  else     sta = 1;
  if ( P > 3 ) printf(" chk = %d\n",chk);
  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      serial_ena_GET_callback(FL_OBJECT*, int)
 * Description:   manage serials read button
 *
 **************************************************************************************/
void serials_ena_GET_callback(FL_OBJECT *obj, long data) 
{

  if ( P > 2 ) printf(" *** invoking serials_ena_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    // se abbassato, tira su il pulsante col click sx.
    if (serials.ena.rea.mon) {
      serials.ena.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    // se il pulsante e' abilitato, esegue la lettura dello stato
    else {
      msg = -1;
      update_objects();
      read_serials_ena_data();
      msg = 1;  
      update_objects();
    }
  }

  // right-click disabilita il pulsante (o lo riabilita)
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    serials.ena.rea.mon = 1 - serials.ena.rea.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      read_serials_ena_data(void)
 * Description:   ask serials enable status 
 *                code: 0x85A00000
 *                answer: 5V.12V.MCA.CO2 ---- ---- ---- 
 *
 **************************************************************************************/
void read_serials_ena_data(void) 
{

  int32 wri = 0x89A00000, rea, mask, rea_en;
  chk = 1;
  int16 err;
  char a[80], b[80];
  int la, lb;

  if ( P > 2 ) printf(" *** invoking read_serials_ena_data\n");

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  //rea = 0x6000;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  printf("\n Reading serials enable status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", rea);

  mask = 0xF000;
  rea_en = mask & rea;

  la = sprintf_binary32(a,rea,-32);

  serials.ena.rea.cod = rea_en;

  lb = sprintf_binary32(b,serials.ena.rea.cod,-32);
  if ( P > 3 ) printf(" serials.ena.rea.cod = %08X = %s\n", serials.ena.rea.cod, b);

  serials.ena.rea.sta = status_OK;  

}
/**************************************************************************************
 *
 * Function:      show_serial_panel(void)
 * Description:   show serials panel
 *
 **************************************************************************************/
void show_serial_panel(void) 
{

  FL_OBJECT *obj;
  int16 m, m1, m2;
  int i;
    
  if ( P > 2 ) printf(" *** invoking show_serial_panel\n");

  
  // colonna 'Enable'
  for (i=0; i<n_serials; i++) {
    obj = serial[i].WRI;
    m = serial[i].msk & serials.ena.wri.cod;
    if (m == serial[i].msk) {
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
  for (i=0; i<n_serials; i++) {
    obj = serial[i].REA;
    m  = serial[i].msk & serials.ena.rea.cod;
    m1 = serials.ena.wri.was & serial[i].msk;
    m2 = serials.ena.rea.cod & serial[i].msk;
    if (serials.ena.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
    }
    else {
      if (m == serial[i].msk) fl_set_object_label(obj, "E");
      else if (m == 0)       fl_set_object_label(obj, "D");
      else                   fl_set_object_label(obj, "?");
    }
    if (serials.ena.rea.sta == status_OK) {
      if (m1 == m2) fl_set_object_color(obj, FL_PALEGREEN, 0);
      else          fl_set_object_color(obj, FL_DARKGOLD, 0);
      if (m != serial[i].msk && m != 0) fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (serials.ena.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (serials.ena.rea.sta == status_NA ||
        serials.ena.rea.sta == status_NN) {
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
  
  obj = serials.ena.GET;
  if (serials.ena.rea.mon) {
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
  int wa = 90, w1 = 10, w2 = 25, w3 = 80, w4 = 75, h = 22;
  
  if ( P > 2 ) printf(" *** invoking create_Mvalve_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+w1+2*w2+w3+w4+2*w2,2+10+10*h," ");
  
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
  int mode = -1;            // modified by AL to remove warning

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

/*   t = atof(fl_get_input(valve[ind].INP)); */
/*   if (t < 0.131) t = 0.131; */
/*   if (t > 8585.085)  t = 8585.085; */
/*   valve[ind].wri.val = t; */
/*   valve[ind].wri.cod = t * 1000.0 / 131.; */

  t = atof(fl_get_input(valve[ind].INP));
  if (t < 0.131) t = 0.131;
  if (t > 8585.085)  t = 8585.085;
  valve[ind].wri.val = t;
  valve[ind].wri.cod = t * 1000.0 / 131.;
/*   if (t < 0.0)    t = 0.0; */
/*   if (t > 65.535) t = 65.535; */
/*   valve[ind].wri.val = t; */
/*   valve[ind].wri.cod = t * 1000.0; */

  
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
 *                open: 1P00 VVVV 0010 1000 TTTT TTTT TTTT TTTT  (ms)
 *                           (valv)        (LSB) ----------> (MSB)
 *
 **************************************************************************************/
void Mvalve_OPN_callback(FL_OBJECT *obj, long data) 
{

  int ind = data, bin_wri;
  int32 wri, rea;
  int16 Mvalve_bits  = 0x0;
  int16 open_marotta_bits = 0x8028;
  chk = 1;
  int16 err;
  int p;
  char b[80];

  if ( P > 2 ) printf(" *** invoking Mvalve_OPN_callback\n");

  if (ind == Mvalve_INP_active) activate_Mvalve_INP(-1, 0);
  msg = -1;
  update_objects();

  if (ind == 0) Mvalve_bits = 0x600;
  if (ind == 1) Mvalve_bits = 0xE00;
  if (ind == 2) Mvalve_bits = 0x100;
  if (ind == 3) Mvalve_bits = 0x900;
  if (ind == 4) Mvalve_bits = 0x500;
  if (ind == 5) Mvalve_bits = 0xD00;
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
  if ( P > 0 )printf("     [code = %s]\n", b);

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

/*   if (!err) increase_valve_count(valve[ind].nam); */

  if (err) sta = 0;
  else     sta = 1;
  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      Mvalve_ena_SET_callback(LF_OBJECT*, int)
 * Description:   callback of Marotta valve enable button
 *                enable: 1P00 1000 0010 1000 V6A.V6B.V18A.V18B 0000 0000 0000
 *                        (8/C) (8)  (2)  (8) +---- (valves.ena.wri.cod) ----+
 *
 **************************************************************************************/
void Mvalves_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_marotta_bits = 0x8828;
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
  if ( P > 0 ) printf("\n clk = %02X  dat = %04X  ena_marotta_bits = %04X", 
	 clk, dat, ena_marotta_bits);
  if ( P > 0 ) printf("\n valves.ena.wri.cod = %08X, wri = %08X\n", valves.ena.wri.cod, wri);

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



/**************************************************************************************
 *
 * Function:      read_Mvalves_ena_data(void)
 * Description:   return Marotta valves enable status
 *                code to ask: 0x88200000
 *                answer:      ---- ---- ---- ---- V6aV6bV18aV18b ---- ---- ---- 
 *                                                 |
 *                                                 +----> valves.ena.rea.cod
 *
 **************************************************************************************/
void read_Mvalves_ena_data(void) 
{

  int32 wri = 0x88200000, rea, mask;
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

  //rea = 0x9000;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = 0xF000;
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

  int32 wri_a = 0x82200000, wri_e = 0xCA200000, rea, mask;
  int32 a_rea, e_rea;
  chk = 1;
  int16 err;
  int la, lb, lc;
  char a[80], b[80], c[80];

  if ( P > 2 ) printf(" *** invoking read_Mvalves_sta_data\n");

  mask = 0xF000;
  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 

  // "active" bits (electrical)
  exec_LeCroy_cmd(n_CMDS, &bus, &wri_a, &rea, &chk, &err);
  //rea = 0xA5A0;                              // ATTENZIONE! Solo per DEBUG!
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
  
  if ( P > 3 ) printf(" a_rea = %s\n, o_rea = %s\n", b, c);

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
 * Function:      create_pump_Panel(int, int)
 * Description:   create circulation pumps panel
 *
 **************************************************************************************/
void create_pump_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int wa = 40, w1 = 10, w2 = 25, w4 = 50, h = 25;
  
  if ( P > 2 ) printf(" *** invoking create_pump_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+2*w1+6*w2+2*w4,2+10+6*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+2*w1+6*w2+2*w4, h, "CIRCULATION PUMPS", size1);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, 2*h, "Pump", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w1+ 2 *w2, 2*h, "Speed", size1);
  x = x + w1 + 2 * w2;
  obj = add_frame_box(x, y, w1+ 2 *w2, 2*h, "Enable", size1);
  x = x + w1 + 2 * w2;
  obj = add_frame_box(x, y, w4, 2*h, " ", size1);
  x = x + w4;
  obj = add_frame_box(x, y, w4, 2*h, " ", size1);
  x = x + w4;
  obj = add_frame_box(x, y, 2*w2, h, "Status", size1);
  obj = add_frame_box(x, y+h, w2, h, "S", size1);
  x = x + w2;
  obj = add_frame_box(x, y+h, w2, h, "A", size1);
  y = y + 2 * h;
  for (i=0; i<n_pumps; i++) {
    x = x0 + 5;
    // colonna col nome delle pompe
    obj = add_frame_box(x, y, wa, h, pump[i].nam, size1);
    x = x + wa;
    // colonna di tastini senza label
    pump[i].SWAS =
      obj = add_data_box(x, y, w1, h, " ", size2);
    x = x + w1;
   // colonna di settaggio speed
    pump[i].SPE =
      obj = add_switch_button(x, y, w2, h, "X", size2);
    fl_set_object_callback(obj, pump_SPE_callback, i);
    x = x + w2;
    // colonna lettura settaggio di speed
    pump[i].SREA =
      obj = add_data_box(x, y, w2, h, "Y", size2);
    x = x + w2;
    // colonna di tastini senza label
    pump[i].WAS =
      obj = add_data_box(x, y, w1, h, " ", size2);
    x = x + w1;
    // colonna di enable
    pump[i].WRI =
      obj = add_switch_button(x, y, w2, h, "X", size2);
    fl_set_object_callback(obj, pump_WRI_callback, i);
    x = x + w2;
    // colonna lettura stato di abilitazione
    pump[i].REA =
      obj = add_data_box(x, y, w2, h, "Y", size2);
    x = x + w2;
   // colonna di open
    pump[i].OPN =
      obj = add_normal_button(x, y, w4, h, "ACTIV", size1);
    fl_set_object_callback(obj, pump_OPN_callback, i);
    x = x + w4;
    // colonna di close
    pump[i].CLS =
      obj = add_normal_button(x, y, w4, h, "STOP", size1);
    fl_set_object_callback(obj, pump_CLS_callback, i);
    x = x + w4;
    // colonna lettura stato di apertura
    pump[i].SSTA =
      obj = add_data_box(x, y, w2, h, "Z", size2);
    x = x + w2;
    pump[i].ASTA =
      obj = add_data_box(x, y, w2, h, "Z", size2);
    y = y + h;
  }
    
  x = x0 + 5;
  obj = add_frame_box(x, y, wa+2*w1+2*w2, h, " ", size1);
  x = x + wa + 2*w1 + 2*w2;
  pumps.ena.SET =
    obj = add_normal_button(x, y, w2, h, "W", size1);
  fl_set_object_callback(obj, pumps_ena_SET_callback, 0);
  x = x + w2;
  pumps.ena.GET =
    obj = add_normal_button(x, y, w2, h, "R", size1);
  fl_set_object_callback(obj, pumps_ena_GET_callback, 0);
  x = x + w2;
  obj = add_frame_box(x, y, w4, h, " ", size1);
  x = x + w4;
  obj = add_frame_box(x, y, w4, h, " ", size1);
  x = x + w4;
  pumps.sta.GET =
    obj = add_normal_button(x, y, 2*w2, h, "R", size1);
  fl_set_object_callback(obj, pumps_sta_GET_callback, 0);
}


/**************************************************************************************
 *
 * Function:      pump_WRI_callback(FL_OBJECT*, long)
 * Description:   manage single pump enable/disable button
 *
 **************************************************************************************/
void pump_WRI_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;
  int16 m;  

  if ( P > 2 ) printf(" *** invoking pump_WRI_callback\n");

  //printf("  ind %3d \n",ind);
  if (ind != n_pumps-1) {
    pumps.ena.wri.cod ^= pump[ind].msk;
  }
  else {
    m = pumps.ena.wri.cod & pump[n_pumps-1].msk;
    if (m == 0 || m == pump[n_pumps-1].msk) {
      pumps.ena.wri.cod ^= pump[n_pumps-1].msk;
    }
    else {
      pumps.ena.wri.cod &= ~pump[n_pumps-1].msk;
    }
  }
  printf("\n Changing status of pump %s\n", pump[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", pumps.ena.wri.cod);
  
  msg = 0;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      pump_SPE_callback(FL_OBJECT*, long)
 * Description:   manage half/full speed setting button
 *
 **************************************************************************************/
void pump_SPE_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int16 m;  

  if ( P > 2 ) printf(" *** invoking pump_SPE_callback\n");

  if (ind != n_pumps-1) {
    pumps.spe.wri.cod ^= pump[ind].smsk;
  }
  else {
    m = pumps.spe.wri.cod & pump[n_pumps-1].smsk;
    if (m == 0 || m == pump[n_pumps-1].smsk) {
      pumps.spe.wri.cod ^= pump[n_pumps-1].smsk;
    }
    else {
      pumps.spe.wri.cod &= ~pump[n_pumps-1].smsk;
    }
  }
  printf("\n Changing speed of pump %s\n", pump[ind].nam);
  if ( P > 0 ) printf("    [code is 0x%04X]\n", pumps.spe.wri.cod);
  
  msg = 0;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      pump_OPN_callback(FL_OBJECT*, long)
 * Description:   callback for pump opening
 *
 **************************************************************************************/
void pump_OPN_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int32 wri, rea;
  chk = 1;
  int16 err;
  char b[80];
  int bin_wri;

  if ( P > 2 ) printf(" *** invoking pump_OPN_callback\n");

  msg = -1;
  update_objects();
  
  if ( ind == 0 ) {
    wri = 0x87288000;
  } else if (ind == 1) {
    wri = 0xCF288000;
  }

  bin_wri = sprintf_binary32(b, wri, -32);
  printf("\n Starting pump %s\n", pump[ind].nam);
  if ( P > 0 ) printf("    [code = %s]\n", b);

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
 * Function:      pump_CLS_callback(FL_OBJECT*, long)
 * Description:   callback for pump closing
 *
 **************************************************************************************/
void pump_CLS_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;
  int32 wri, rea;
  chk = 1;
  int16 err;
  char b[80];
  int bin_wri;

  if ( P > 2 ) printf(" *** invoking pump_CLS_callback\n");

  msg = -1;
  update_objects();
  
  if ( ind == 0 ) {
    wri = 0xC7280000;
  } else if (ind == 1) {
    wri = 0x8F280000;
  }

  bin_wri = sprintf_binary32(b, wri, -32);
  printf("\n Stopping pump %s\n", pump[ind].nam);
  if ( P > 0 ) printf("    [code = %s]\n", b);

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
 * Function:      pumps_ena_SET_callback(FL_OBJECT*, long)
 * Description:   callback for pumps enable
 *                enable: 1P00 1000 0010 1000 V6A.V6B.V18A.V18B 0000 0000 0000
 *                        (8/C) (8)  (2)  (8) +---- (pumps.ena.wri.cod) ----+
 *
 **************************************************************************************/
void pumps_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_pumps_bits = 0x8428;
  int32 wri, rea;
  chk = 1;
  int16 err;
  int p, bin_wri;
  char b[80];


  if ( P > 2 ) printf(" *** invoking pumps_ena_SET_callback\n");

  msg = -1;
  update_objects();

  wri = ena_pumps_bits << 16;
  wri += pumps.ena.wri.cod;
  if ( P > 3 ) printf(" clk = %02X  dat = %04X  ena_pumps_bits = %04X", 
	 clk, dat, ena_pumps_bits);
  if ( P > 3 ) printf(" pumps.ena.wri.cod = %08X, wri = %08X\n", pumps.ena.wri.cod, wri);
  wri += pumps.spe.wri.cod;
  if ( P > 3 ) printf(" pumps.spe.wri.cod = %08X, wri = %08X\n", pumps.spe.wri.cod, wri);

  // Calcolo del bit di parita'
  p = parity32(wri);

  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);
  bin_wri = sprintf_binary32(b, wri, -32);

  printf("\n Enabling selected pumps\n");
  if ( P > 0 ) printf("    [code = %s]\n", b);

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  pumps.ena.wri.was = pumps.ena.wri.cod;
  pumps.ena.wri.sta = status_OK;  
  pumps.spe.wri.was = pumps.spe.wri.cod;
  pumps.spe.wri.sta = status_OK;  
  if (err) sta = 0;
  else     sta = 1;

  msg = 1;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      pumps_ena_GET_callback(FL_OBJECT*, long)
 * Description:   manage pumps enable status read button
 *
 **************************************************************************************/
void pumps_ena_GET_callback(FL_OBJECT *obj, long data) 
{

  if ( P > 2 ) printf(" *** invoking pumps_ena_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    // se abbassato, tira su il pulsante col click sx.
    if (pumps.ena.rea.mon) {
      pumps.ena.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    // se il pulsante e' abilitato, esegue la lettura dello stato
    else {
      msg = -1;
      update_objects();
      read_pumps_ena_data();
      msg = 1;  
      update_objects();
    }
  }

  // right-click disabilita il pulsante (o lo riabilita)
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    pumps.ena.rea.mon = 1 - pumps.ena.rea.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      pumps_sta_GET_callback(FL_OBJECT*, long)
 * Description:   manage pumps status read button
 *
 **************************************************************************************/
void pumps_sta_GET_callback(FL_OBJECT *obj, long data) 
{

  if ( P > 2 ) printf(" *** invoking pumps_sta_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (pumps.sta.rea.mon) {
      pumps.sta.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      read_pumps_sta_data();
      msg = 1;  
      update_objects();
    }
  }

  // right-click disabilita il pulsante
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    pumps.sta.rea.mon = 1 - pumps.sta.rea.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      read_pumps_ena_data(void)
 * Description:   ask pumps enable status
 *                code: 0x84200000
 *                answer: ---- ---- ---- ---- CP1 CP2 SP1 SP2 ---- ---- ---- 
 *
 **************************************************************************************/
void read_pumps_ena_data(void) 
{

  int32 wri = 0x84200000, rea, mask, smask, rea_en, rea_sp;
  chk = 1;
  int16 err;
  char a[80], b[80], c[80];
  int la, lb, lc;

  if ( P > 2 ) printf(" *** invoking read_pumps_ena_data\n");

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  //rea = 0xE000;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = 0xC000, smask = 0x3000;
  rea_en = mask & rea;
  rea_sp = smask & rea;

  la = sprintf_binary32(a,rea,-32);

  pumps.ena.rea.cod = rea_en;
  pumps.spe.rea.cod = rea_sp;
  printf("\n Reading pumps enable status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", pumps.ena.rea.cod);
  printf("\n Reading pumps speed status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", pumps.spe.rea.cod);

  lb = sprintf_binary32(b,pumps.ena.rea.cod,-32);
  lc = sprintf_binary32(c,pumps.spe.rea.cod,-32);
  if ( P > 3 ) printf(" pumps.ena.rea.cod = %08X = %s\n", pumps.ena.rea.cod, b);
  if ( P > 3 ) printf(" pumps.spe.rea.cod = %08X = %s\n", pumps.spe.rea.cod, c);

  pumps.ena.rea.sta = status_OK;  
  pumps.spe.rea.sta = status_OK;  // controllare il significato di questa variabile

}


/**************************************************************************************
 *
 * Function:      read_pumps_sta_data(void)
 * Description:   ask pumps opening status
 *                code: 0x82200000
 *                answer: ---- ---- ---- ---- CP1 CP2 SP1 SP2 ---- ---- ---- 
 *
 **************************************************************************************/
void read_pumps_sta_data(void) 
{

  int32 wri = 0x82200000, rea, mask;
  int32 a_rea, s_rea;
  chk = 1;
  int16 err;
  char a[80], b[80], c[80];
  int la, lb, lc;

  if ( P > 2 ) printf(" *** invoking read_pumps_sta_data\n");

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  //rea = 0xA5AB;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = 0xC;
  s_rea = (rea & mask) << 10;
  mask = 0x3;
  a_rea = (rea & mask) << 14;


  lb = sprintf_binary32(b,a_rea,-32);
  lc = sprintf_binary32(c,s_rea,-32);

  if ( P > 3 ) printf(" s_rea = %s,\n a_rea = %s\n", c, b);

  pumps.sta.rea.acod = a_rea;
  pumps.sta.rea.scod = s_rea;

  printf("\n Reading pumps status\n");
  if ( P > 0 ) printf("    [code in answer = %04X, %04X]\n", pumps.sta.rea.scod, pumps.sta.rea.acod);

  pumps.sta.rea.sta = status_OK;  

}


/**************************************************************************************
 *
 * Function:      show_pump_panel(void)
 * Description:   show circulation pumps panel
 *
 **************************************************************************************/
void show_pump_panel(void) 
{

  FL_OBJECT *obj;
  int16 m, m1, m2;
  int i;
  
  if ( P > 2 ) printf(" *** invoking show_pump_panel\n");

  for (i=0; i<n_pumps; i++) {
    // tastini senza label 
    // cambiano colore quando viene variato lo stato di abilitazione 
    // finche' non si preme W
    obj = pump[i].WAS;
    m1 = pumps.ena.wri.was & pump[i].msk;    // stato precedente della pompa 
    m2 = pumps.ena.wri.cod & pump[i].msk;    // stato impostato per la pompa
    if ((pumps.ena.wri.sta == status_OK) && (m1 == m2)) {
      fl_set_object_color(obj, FL_PALEGREEN, 0);
    }
    else {
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
    obj = pump[i].SWAS;
    m1 = pumps.spe.wri.was & pump[i].smsk;    // speed precedente della pompa 
    m2 = pumps.spe.wri.cod & pump[i].smsk;    // speed impostata per la pompa
    if ((pumps.spe.wri.sta == status_OK) && (m1 == m2)) {
      fl_set_object_color(obj, FL_PALEGREEN, 0);
    }
    else {
      fl_set_object_color(obj, FL_WHEAT, 0);
    }
  }
  // colonna 'Enable'
  for (i=0; i<n_pumps; i++) {
    obj = pump[i].WRI;
    m = pump[i].msk & pumps.ena.wri.cod;
    if (m == pump[i].msk) {
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
  for (i=0; i<n_pumps; i++) {
    obj = pump[i].REA;
    m  = pump[i].msk & pumps.ena.rea.cod;
    m1 = pumps.ena.wri.was & pump[i].msk;
    m2 = pumps.ena.rea.cod & pump[i].msk;
    if (pumps.ena.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
    }
    else {
      if (m == pump[i].msk) fl_set_object_label(obj, "E");
      else if (m == 0)       fl_set_object_label(obj, "D");
      else                   fl_set_object_label(obj, "?");
    }
    if (pumps.ena.rea.sta == status_OK) {
      if (m1 == m2) fl_set_object_color(obj, FL_PALEGREEN, 0);
      else          fl_set_object_color(obj, FL_DARKGOLD, 0);
      if (m != pump[i].msk && m != 0) fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (pumps.ena.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (pumps.ena.rea.sta == status_NA ||
        pumps.ena.rea.sta == status_NN) {
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
  // colonna 'Speed'
  for (i=0; i<n_pumps; i++) {
    obj = pump[i].SPE;
    m = pump[i].smsk & pumps.spe.wri.cod;
    if (m == pump[i].smsk) {
      fl_set_object_color(obj, FL_PALEGREEN, FL_PALEGREEN);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "F");
    }
    else if (m == 0) {
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_WHEAT);
      fl_set_object_label(obj, "H");
    }
    else {
      fl_set_object_color(obj, FL_DARKGOLD, FL_DARKGOLD);
      fl_set_object_lcolor(obj, FL_BLACK);
      fl_set_object_label(obj, "?");
    }
  }
  for (i=0; i<n_pumps; i++) {
    obj = pump[i].SREA;
    m  = pump[i].smsk & pumps.spe.rea.cod;
    m1 = pumps.spe.wri.was & pump[i].smsk;
    m2 = pumps.spe.rea.cod & pump[i].smsk;
    if (pumps.spe.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
    }
    else {
      if (m == pump[i].smsk) fl_set_object_label(obj, "F");
      else if (m == 0)       fl_set_object_label(obj, "H");
      else                   fl_set_object_label(obj, "?");
    }
    if (pumps.spe.rea.sta == status_OK) {
      if (m1 == m2) fl_set_object_color(obj, FL_PALEGREEN, 0);
      else          fl_set_object_color(obj, FL_DARKGOLD, 0);
      if (m != pump[i].smsk && m != 0) fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (pumps.spe.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (pumps.spe.rea.sta == status_NA ||
        pumps.spe.rea.sta == status_NN) {
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
  }
  // colonna "status" active
  for (i=0; i<n_pumps; i++) {
    obj = pump[i].ASTA;
    if (pumps.sta.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (pumps.sta.rea.sta == status_NA) {
      fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (pumps.sta.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (pumps.sta.rea.sta == status_OK) {
      m  = pump[i].msk & pumps.sta.rea.acod;
      if (m == pump[i].msk) {
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
  // colonna "status" speed
  for (i=0; i<n_pumps; i++) {
    obj = pump[i].SSTA;
    if (pumps.sta.rea.sta == status_NN) {
      fl_set_object_label(obj, " ");
      fl_set_object_color(obj, FL_WHEAT, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (pumps.sta.rea.sta == status_NA) {
      fl_set_object_color(obj, FL_DARKGOLD, 0);
      fl_set_object_lcolor(obj, FL_BLACK);
    }
    if (pumps.sta.rea.sta == status_KO) {
      fl_set_object_color(obj, FL_BLACK, 0);
      fl_set_object_lcolor(obj, FL_WHITE);
    }
    if (pumps.sta.rea.sta == status_OK) {
      m  = pump[i].smsk & pumps.sta.rea.scod;
      if (m == pump[i].smsk) {
        fl_set_object_label(obj, "F");
        fl_set_object_color(obj, FL_PALEGREEN, 0);
        fl_set_object_lcolor(obj, FL_BLACK);
      }
      else if (m == 0) {
        fl_set_object_label(obj, "H");
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

  obj = pumps.ena.GET;
  if (pumps.ena.rea.mon) {
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_DARKGOLD);
  }
  else {
    fl_set_object_boxtype(obj, FL_UP_BOX);
    fl_set_object_color(obj, FL_MCOL, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
  obj = pumps.sta.GET;
  if (pumps.sta.rea.mon) {
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
  int wa = 70, w2 = 25, w3 = 95, h = 25;
  
  if ( P > 2 ) printf(" *** invoking creat_sensor_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+2*w3+2*w2,2+10+4*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+2*w3+2*w2, h, "GP50 PRESSURE SENSORS", size1);
  y = y + h + 2;
  obj = add_frame_box(x, y, wa, h, "Sensor", size1);
  x = x + wa;
  obj = add_frame_box(x, y, w2, h, " ", size1);
  x = x + w2;
  obj = add_frame_box(x, y, w3, h, "Press [mV]", size1);
  x = x + w3;
  obj = add_frame_box(x, y, w3, h, "Press [psia]", size1);
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
    wri = 0x8FA88000;       // P3
  } else if (ind == 1) {
    wri = 0x8FA84000;       // P4
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

  if (ind == 0) {              // P3
    wri = 0xC8A00000;
  } else if (ind == 1) {       // P4
    wri = 0xC4A00000;
  }

  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }

  //prova_global();

  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  if (err) sta = 0;
  else     sta = 1;

  // rea = 0xA430;             // ATENZIONE solo per debug!

  mask = 0xFFF0;
  rea &= mask;
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  p_bit = (int16)rea;
  p_bit = invert_bit_order(p_bit);
  if ( P > 3 ) printf(" after invert rea = %08X\n", p_bit);
  lb = sprintf_binary(b,p_bit,-16);

  if ( P > 3 ) printf(" p_bit = %04X = %s, val in bit = %10.3f\n", p_bit, b, (float)p_bit);

  printf("\n Reading GP 50 pressure sensor %s\n", sensor[ind].nam);
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", p_bit);

  sensor[ind].rea.val = (float)p_bit*5000./4096.;  // CHIARIRE FORMULA DI CONVERSIONE!!!!!!!
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

  if (ind == 0) {              // P3
    wri = 0xC4A00000;
  } else if (ind == 1) {       // P4
    wri = 0xC2A00000;
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

  mask = 0xFFFF;
  rea &= mask;
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);
  p_bit = (int16)rea;
  p_bit = invert_bit_order(p_bit);
  lb = sprintf_binary(b,p_bit,-16);
  if ( P > 3 ) printf(" p_bit = %04X = %s, val in bit = %10.3f\n", p_bit, b, (float)p_bit);

  printf("\n Reading GP 50 pressure sensor %s\n", tsensor[ind].nam);
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
 * Check convertion factor
 *
 **************************************************************************************/
float pressure_psia(int ind, float p_mV) 
{

  float conv_factor = 0.0;            // modified by AL to remove warning
  float p_psia;

  if ( ind == 0 ) conv_factor = 25. / (5000.); //non accurata controllare
  if ( ind == 1 ) conv_factor = 25. / (5000.); //non accurata controllare

  if ( P > 2 ) printf(" *** invoking pressure_psia\n");
  p_psia = p_mV * conv_factor; //(psia/Volt)
  return p_psia;

}


/**************************************************************************************
 *
 * Function:      temp_degrees(int, float)
 * Description:   convert Gp50 mVolt into Kelvin
 // Controllare fattore di conversione
 *
 **************************************************************************************/
float temp_degrees(int ind, float t_mV) {

  float conv_factor;
  float t_deg;


  conv_factor = 22.24875/1000.;
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
/*     // finestrella temperatura in mV */
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
/*     else                                sprintf(b, "%7.1f", tsensor[i].rea.val); */
/*     fl_set_object_label(obj, b); */
/*     // finestrella temperatura in K */
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
/*     else                                 sprintf(b, "%7.1f", tsensor[i].rea.deg); */
/*     fl_set_object_label(obj, b); */
/*     // tastini "R" della temperatura */
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
 * Function:      create_Fvalve_panel(int, int)
 * Description:   create Flipper valves panel
 *
 **************************************************************************************/
void create_Fvalve_panel(int x0, int y0) 
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int wa = 100, w1 = 105, h = 25;
  
  if ( P > 2 ) printf(" *** invoking creat_Fvalve_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+wa+2*w1,2+10+3*h," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa+2*w1, h, "FLIPPER VALVES", size1);
  y = y + 2 + h;
  for (i=0; i<n_Fvalve; i++) {
    x = x0 + 5;
    obj = add_frame_box(x, y, wa, h, Fvalve[i].nam, size1);
    x = x + wa;
    Fvalve[i].OPN =
      obj = add_normal_button(x, y, w1, h, "OPEN", size1);
    fl_set_object_callback(obj, Fvalve_OPN_callback, i);
    x = x + w1;
    Fvalve[i].CLS =
      obj = add_normal_button(x, y, w1, h, "CLOSE", size1);
    fl_set_object_callback(obj, Fvalve_CLS_callback, i);
    y = y + h;
  }
}


/**************************************************************************************
 *
 * Function:      Fvalve_OPN_callback(FL_OBJECT*, long)
 * Description:   manage Flipper open button
 *
 **************************************************************************************/
void Fvalve_OPN_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking Fvalve_OPN_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (Fvalve[ind].opn.mon) {
      Fvalve[ind].opn.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      open_Fvalve(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    Fvalve[ind].opn.mon = 1 - Fvalve[ind].opn.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      Fvalve_CLS_callback(FL_OBJECT*, long)
 * Description:   manage Flipper close button
 *
 **************************************************************************************/
void Fvalve_CLS_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking Fvalve_CLS_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (Fvalve[ind].cls.mon) {
      Fvalve[ind].cls.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      close_Fvalve(ind);
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    Fvalve[ind].cls.mon = 1 - Fvalve[ind].cls.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      open_Fvalve(int)
 * Description:   callback for Flipper open button
 *
 **************************************************************************************/
void open_Fvalve(int ind) 
{

  int32 rea, wri;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking open_Fvalve\n");

  if (ind == 0) {              // V8a
    wri = 0xC32CFF00;
  } else if (ind == 1) {       // V8b
  //wri = 0x8B2C8100;     //  129 ms?
    wri = 0x8B2CFF00;     //  255 ms?
  }

  printf("\n Opening Flipper valve %s\n", Fvalve[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%08X]\n",wri);

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

  Fvalve[ind].opn.sta = status_OK;
}


/**************************************************************************************
 *
 * Function:      close_Fvalve(int)
 * Description:   callback for Flipper close button
 *
 **************************************************************************************/
void close_Fvalve(int ind) 
{

  int32 rea, wri;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking close_Fvalve\n");

  if (ind == 0) {              // V8a
    wri = 0xC32A8100;
  } else if (ind == 1) {       // V8b
    wri = 0x8B2A8100;
  }

  printf("\n Closing Flipper valve %s\n", Fvalve[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%08X]\n",wri);

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

  Fvalve[ind].cls.sta = status_OK;
}


/**************************************************************************************
 *
 * Function:      show_Fvalve_panel(void)
 * Description:   show Flipper valves panel
 *
 **************************************************************************************/
void show_Fvalve_panel(void) 
{

  FL_OBJECT *obj;
  int i;

  if ( P > 2 ) printf(" *** invoking show_valve_panel\n");

  for (i=0; i<n_Fvalve; i++) {
    // settaggi per i tastini "R"
    obj = Fvalve[i].OPN;
    if (Fvalve[i].opn.mon) {
      fl_set_object_boxtype(obj, FL_DOWN_BOX);
      fl_set_object_color(obj, FL_RED, FL_RED);
      fl_set_object_lcolor(obj, FL_DARKGOLD);
    }
    else {
      fl_set_object_boxtype(obj, FL_UP_BOX);
      fl_set_object_color(obj, FL_MCOL, FL_RED);
      fl_set_object_lcolor(obj, FL_BLACK);
    }

    obj = Fvalve[i].CLS;
    if (Fvalve[i].cls.mon) {
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
 * Function:      create_channel_panel(int, int)
 * Description:   create MCA channel panel
 *
 **************************************************************************************/
void create_channel_panel(int x0, int y0)
{

  FL_OBJECT *obj;
  int i, x, y;
  int size1 = FL_NORMAL_SIZE;
  int size2 = FL_MEDIUM_SIZE;
  int w1 = 58, w2 = 10, w3 = 24, h = 23, h2 = 22;
  
  if ( P > 2 ) printf(" *** invoking creat_channel_panel\n");

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,10+4*w1+4*w2+w3+15,2+10+h+h2+5," ");
  
  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, 4*w1+4*w2+w3+15, h, "MULTI CH ANALYZER", size1);
  y = y + 2 + h;
  x = x0 + 5;
  obj = add_frame_box(x, y, 4*w1+4*w2+w3+15, h+5, " ", size1);
  y = y + 3;
  x = x + 2;
  for (i=0; i<n_channels; i++) {
    channel[i].OPN = 
      obj = add_normal_button(x, y, w1, h2, channel[i].nam, size1);
    fl_set_object_callback(obj, channel_OPN_callback, i);
    x = x + w1;
    channel[i].STA = 
      obj = add_data_box(x, y, w2, h, " ", size2);
    x = x + w2+2;
  }
  x = x + 3;
  channels.CHK = 
    obj = add_normal_button(x, y, w3, h, "R", size1);
    fl_set_object_callback(obj, channel_CHK_callback, i);
}


/**************************************************************************************
 *
 * Function:      channel_OPN_callback(FL_OBJECT*, long)
 * Description:   callback for channel opening
 *
 **************************************************************************************/
void channel_OPN_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking channel_OPN_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (channel[ind].opn.mon) {
      channel[ind].opn.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      open_channels(ind);
      check_channels();
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    channel[ind].opn.mon = 1 - channel[ind].opn.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      channel_CHK_callback(FL_OBJECT*, long)
 * Description:   callback for channel opening
 *
 **************************************************************************************/
void channel_CHK_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking channel_CHK_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (channel[ind].rea.mon) {
      channel[ind].rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      check_channels();
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    channel[ind].opn.mon = 1 - channel[ind].opn.mon;
    update_objects();
  }
}

/**************************************************************************************
 *
 * Function:      open_channels(int)
 * Description:   manage channel opening
 *
 **************************************************************************************/
void open_channels(int ind) 
{

  int32 rea, wri = 0x8C280000;
  int16 err;
  int pp;

  if ( P > 2 ) printf(" *** invoking open_channels\n");

  chk = 1;

  printf("\n Selecting MCA channel %s\n", channel[ind].nam);  
  wri += channel[ind].msk;
  pp = parity32(wri);

  wri += pp << 30;

   if ( P > 0 ) printf("    [code = 0x%08X]\n",wri);

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

  channel[ind].opn.sta = status_OK;
}


/**************************************************************************************
 *
 * Function:      check_channels(int)
 * Description:   manage channel opening
 *
 **************************************************************************************/
void check_channels(void) 
{

  int32 rea, wri = 0xCC200000;
  int16 err, mask;

  if ( P > 2 ) printf(" *** invoking check_channels\n");

  chk = 1;


  if (HC_ID & 0x01) {
    clk = 0 ; dat = 0;
  }
  else {
    clk = 2 ; dat = 2;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

/*   r = rand() % 8 + 1;                         // ATTENZIONE! Solo per DEBUG! */
/*   printf(" r = %d\n", r);                     // ATTENZIONE! Solo per DEBUG! */
/*   rea = r << 12;                              // ATTENZIONE! Solo per DEBUG! */
  printf("\n Checking MCA channel\n");  
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", rea);

  mask = 0xF000;
  rea &= mask;

  channels.rea.cod = rea;
  if ( P > 1 ) printf(" channels.rea.cod = %08X\n", channels.rea.cod);
  
  if (err) sta = 0;
  else     sta = 1;

  channels.rea.sta = status_OK;

}



/**************************************************************************************
 *
 * Function:      show_channel_panel(void)
 * Description:   show MCA panel 
 *
 **************************************************************************************/
void show_channel_panel(void)
{

  FL_OBJECT *obj;
  int16 m;
  int i;

  if ( P > 2 ) printf(" *** invoking show_channel_panel\n");

  for (i=0; i<n_channels; i++) {
    // settaggi per i tastini "R"
    obj = channel[i].OPN;
    if (channel[i].opn.mon) {
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

  for (i=0; i<n_channels; i++) {
    // tastini abilitazione canale
    obj = channel[i].STA;
    m = channels.rea.cod & channel[i].msk;
    if ((channels.rea.sta == status_OK) && (m == channel[i].msk)) {
      fl_set_object_color(obj, FL_PALEGREEN, 0);
    }
    else {
      fl_set_object_color(obj, FL_RED, 0);
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

  if ( P > 2 ) printf(" *** msg = %d,   sta = %d\n", msg, sta);

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
  show_tension_panel();
  /* RS232 */
  show_serial_panel();
  /* Marotta */
  show_Mvalve_panel();
  /* Pumps */
  show_pump_panel();
  /* Gp50 */
  show_sensor_panel();
  /* Flipper */
/*   show_Fvalve_panel(); */
  /* MCA */
  show_channel_panel();

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

  tensions.ena.wri.cod = 0x7000;
  tensions.ena.wri.sta = status_NN;
  tensions.ena.rea.mon = 0;
  tensions.ena.rea.sta = status_NN;


  /* RS232 */
  for (i=0; i<n_serials; i++) {
    serial[i].wri.sta = status_NN;
  }

  serials.ena.wri.cod = 0;
  serials.ena.wri.sta = status_NN;
  serials.ena.rea.mon = 0;
  serials.ena.rea.sta = status_NN;

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

  /* Pumps */
  for (i=0; i<n_pumps; i++) {
    pump[i].wri.sta = status_NN;
  }
  pumps.ena.wri.cod = 0;
  pumps.ena.wri.sta = status_NN;
  pumps.ena.rea.mon = 0;
  pumps.ena.rea.sta = status_NN;
  pumps.spe.wri.cod = 0;
  pumps.spe.wri.sta = status_NN;
  pumps.spe.rea.mon = 0;
  pumps.spe.rea.sta = status_NN;
  pumps.sta.rea.mon = 0;
  pumps.sta.rea.sta = status_NN;

  /* Gp50 */
  for (i=0; i<n_sensor; i++) {
    sensor[i].rea.mon = 0;
    sensor[i].rea.sta = status_NN;
    tsensor[i].rea.sta = status_NN;
  }

  /* Flipper valves */
  for (i=0; i<n_Fvalve; i++) {
    Fvalve[i].opn.mon = 0;
    Fvalve[i].opn.sta = status_NN;
    Fvalve[i].cls.mon = 0;
    Fvalve[i].cls.sta = status_NN;
  }

  /* MCA */
  for (i=0; i<n_channels; i++) {
    channel[i].opn.mon = 0;
    channel[i].opn.sta = status_NN;
  }
}


/*********************************************************************
 *
 * Function:      main()
 *
 *********************************************************************/
int main (int argc, char **argv)
{

  ESPIONAGE = TRUE;

  printf("\033[2J\n");
  printf("\033[1;0H\n ************************************************************************\n");
  printf(" **********          This is the controller for Box C         ***********\n");
  printf(" ************************************************************************\n");
  template(argc, &argv[0]);
  
  return 0;
}
/*   printf("\033[2J\n"); */
/*   printf("\033[1;0H\n ******************************************************************************\n"); */
/*   printf(" **********             This is the controller for Box C            ***********\n"); */
/*   printf(" ******************************************************************************\n"); */
/*   printf("\n Usage: boxC [ Debug_Level ]\n"); */
/*   P = 0; */
/*   if (argc > 1) P = atoi(argv[1]); */
/*   printf("\n Debug_Level = %d\n", DEBUG_LEVEL); */
/*   P = DEBUG_LEVEL; */
/*   start_boxc(); */
/*   while ((but = fl_do_forms()) != yes) */
    
/*   exit(2); */
/*   return 0; */
/* } */

