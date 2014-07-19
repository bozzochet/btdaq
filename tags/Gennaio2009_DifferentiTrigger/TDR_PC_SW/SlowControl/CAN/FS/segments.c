// file segments.c
//
// modified by AL to remove compiler warnings
//
/*********************************************************************
 *
 * Filename:     segments.c
 *
 * Author(s):    Francesca Spada
 *
 * Date:         1/5/2004
 *
 *********************************************************************/

/* #include "xformslib.h" */
#include "template.h"
#include "fslib.h"

bool ESPIONAGE;

char *date   = {"07-Jan-08"};                          
char *author = {"F.R.Spada"}; // FS
char *title  = {"UGFV Controller"};

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


/* void start_manifold(int); */

FL_OBJECT *yes, *but;



/*********************************************************************
 * Type definition, structures, initializations
 *********************************************************************/

int        msg = 0;
int16      sta;
bool       chk;
FL_OBJECT* MESSAGE;

typedef struct {
  FL_FORM* manifold;
  void* vdata;
  long ldata;
} FD_MANIFOLD;

FD_MANIFOLD*   manifold;

/* Honeywell */
#define n_sensor 5

struct {
  char *nam;
  struct {
    int16 cod;
    float val;
    float bar;
    int   sta;
  } rea;
  FL_OBJECT *VAL;
  FL_OBJECT *BAR;
} sensor[n_sensor] = {
  {"P1A"},
  {"P2A"},
  {"P3A"},
  {"P4A"},
  {"P5A"},
/*   {"P6A"}, */
/*   {"P1B"}, */
/*   {"P2B"}, */
/*   {"P3B"}, */
/*   {"P4B"}, */
/*   {"P5B"}, */
/*   {"P6B"} */
};

struct {
  struct {
    int mon;
  } rea;
  FL_OBJECT *GET;
} sensors;

/* Tensions */
#define n_tensions 4

struct {
  char *nam;
  int16 cmd;
  int16 msk;
  struct {
    int   sta;
  } wri;
  FL_OBJECT *WRI;
  FL_OBJECT *REA;
} tension[n_tensions] = {
  {"12VPS",     0x12, 0x8000},  // 1 disabilita
  {"12VFV",     0x12, 0x4000},  // 1 disabilita
  {"21V",       0x12, 0x2000},  // 1 abilita (!!)
  {"MUX",       0x12, 0x1000},  // 1 abilita (!!)
};


struct {
  struct {
    struct {
      int16 was;
      int16 cod;
      int   sta;
    } wri;
    struct {
      int16 cod;
      int   mon;
      int   sta;
    } rea;
    FL_OBJECT *SET;
    FL_OBJECT *GET;
  } ena;
} tensions;

int tension_INP_active;

/* Flipper valves */
#define n_Fvalve 5

struct {
  char *nam;
  int16 cmd;
  int16 msk;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int   mon;
    int   sta;
  } cls;
  struct {
    FL_OBJECT *WRI;
  } ena;
} Fvalve[n_Fvalve] = {
  {" 1",  0x12, 0x20},
  {" 2",  0x12, 0x10},
  {" 3",  0x12, 0x08},
  {" 4",  0x12, 0x04},
  {" 5",  0x12, 0x02},
/*   {"(6)", 0x12, 0x01}, */
};

struct {
  struct {
    int16 cmd;
    struct {
      int16 was;
      int16 cod;
      int   sta;
    } wri;
  } ena;
  FL_OBJECT *OPN;
  FL_OBJECT *CLS;
  FL_OBJECT *OPNA;
  FL_OBJECT *CLSA;
} Fvalves;

/* Module */
#define n_module 4

struct {
  char *nam;
  int32 msk;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int   mon;
    int   sta;
  } cls;
  struct {
    FL_OBJECT *WRI;
  } ena;
} module[n_module] = {
  {" 1", 0x8000000},
  {" 2", 0x4000000},
  {" 3", 0xC000000},
  {" 4", 0x2000000},
/*   {" 5", 0xA000000}, */
/*   {" 6", 0x6000000}, */
/*   {" 7", 0xE000000}, */
/*   {" 8", 0x1000000}, */
};


struct {
  struct {
    int16 cmd;
    struct {
      int16 was;
      int32 cod;
      int   sta;
    } wri;
  } ena;
} modules;

/* Multiplexer */
#define n_mux 4

struct {
  char *nam;
  int16 msk;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int   mon;
    int   sta;
  } cls;
  struct {
    FL_OBJECT *WRI;
  } ena;
} mux[n_mux] = {
  {" 1", 0x0},
  {" 2", 0x4},
  {" 3", 0x2},
  {" 4", 0x6},
/*   {" 5", 0x1}, */
/*   {" 6", 0x5}, */
/*   {" 7", 0x3}, */
/*   {" 8", 0x7} */
};

struct {
  struct {
    int16 cmd;
    struct {
      int16 was;
      int16 cod;
      int   sta;
    } wri;
  } ena;
  FL_OBJECT *SEL;
} muxs;

/* UGFV Board */
#define n_board 2

struct {
  char *nam;
  int16 cmd;
  int32 msk;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int   mon;
    int   sta;
  } cls;
  struct {
    FL_OBJECT *WRI;
  } ena;
} board[n_board] = {
  {" AC", 0x12, 0x400000},
  {" BD", 0x12, 0x200000}
};

struct {
  struct {
    int16 cmd;
    struct {
      int16 was;
      int32 cod;
      int   sta;
    } wri;
  } ena;
} boards;


/* Coordinates for boxes */
int WW = 580, HH = 400;
int x_tens  =   5, y_tens    =  60;
int x_honey = 260, y_honey   = 240;
int x_flip  = 310, y_flip    =  65;
int x_boa   = 300, y_boa     = 490;
int x_mod   = 265, y_mod     =  65;
int x_mux   =  10, y_mux     = 240;
int h   = 25;
int w0  =  5, offs = 2;
int wt1 = 80, wt2 = 81;
int wh1 = 60;
int wf1 = 50, wf2 = 50, yf1 = 0, hf1 = 30;
int wb1 = 70;
int wm1 = 53, wm2 = 10, hm1 = 30, ym1 = 0;
int wx1 = 55, wx2 = 16, yx1 = 0;
int size1 = FL_NORMAL_SIZE;
int size2 = FL_MEDIUM_SIZE;

void     create_main_form(void);
/* FD_MANIFOLD* create_form_manifold(); */
void     open_map_callback(FL_OBJECT*, long);

/* Tensions */
void     create_tension_panel(void);
void     tension_WRI_callback(FL_OBJECT*, long);
void     tensions_ena_SET_callback(FL_OBJECT*, long);
void     tensions_ena_GET_callback(FL_OBJECT*, long);
void     read_tensions_ena_data(void);
void     read_tensions_sta_data(void);
void     show_tension_panel(void);

/* Honeywell sensors */
void     create_sensor_panel(void);
void     sensor_GET_callback(FL_OBJECT*, long);
void     read_sensor_data(void);
void     show_sensor_panel(void);
float    pressure_bar(int, float);

/* Flipper valves */
void     create_Fvalve_panel(void);
void     Fvalve_OPN_callback(FL_OBJECT*, long);
void     Fvalve_CLS_callback(FL_OBJECT*, long);
void     Fvalve_all_OPN_callback(FL_OBJECT*, long);
void     Fvalve_all_CLS_callback(FL_OBJECT*, long);
void     Fvalve_WRI_callback(FL_OBJECT*, long);
void     open_all_Fvalve(void);
void     close_all_Fvalve(void);
void     open_Fvalve(void);
void     close_Fvalve(void);
void     show_Fvalve_panel(void);

/* Modules */
void     create_module_panel(void);
void     module_WRI_callback(FL_OBJECT*, long);
void     show_module_panel(void);
void     mux_SEL_callback(FL_OBJECT*, long);
void     select_mux(void);

/* Multiplexers */
void     create_mux_panel(void);
void     mux_WRI_callback(FL_OBJECT*, long);
void     show_mux_panel(void);

/* UGFV Board */
void     create_board_panel(void);
void     board_WRI_callback(FL_OBJECT*, long);
void     show_board_panel(void);

/* Common commands */
void     invalidate_data(void);
void     update_objects(void);
void     show_message(void);



/*********************************************************************
 *
 * Function:      start_manifold(int)
 * Description:   xforms initialization 
 *
 *********************************************************************/
/* void start_manifold(int nr)  */
/* {  */

/*   int one = 1; */
/*   static char* title = {"Manifold\nController"}; */
/*   char* adr[] = {(char*)&title}; */
/*   if ( P > 2 ) printf(" *** invoking start_manifold\n"); */
/*   fl_initialize(&one, adr, " ", 0, 0); */
/*   manifold = create_form_manifold(); */
/*   //fl_ringbell(100); */
/*   fl_set_form_position(manifold->manifold, 0, 0); */
/*   fl_show_form(manifold->manifold, FL_PLACE_GEOMETRY, FL_FULLBORDER, "Manifold"); */
/*   update_objects(); */
/* } */


/*********************************************************************
 *
 * Function:      create_form_manifold(int)
 * Description:   creation of the external box 
 *
 *********************************************************************/
/* FD_MANIFOLD* create_form_manifold()  */
void create_main_form(void)
{

  FL_OBJECT *obj;  
  if ( P > 2 ) printf(" *** invoking create_form_manifold\n");

  invalidate_data();
#ifdef USE_CAN
  bgn_form(&MAIN, 0, 0, WW, HH);
#endif


  create_HOTCOLD_panel();

  /* Box around Module+Valves */
  obj = fl_add_box(FL_DOWN_BOX, x_mod-w0, y_mod-w0, offs+10*w0+wm1+wm2+3*wf1+wf2, offs+4*w0+2*h+(n_module-1)*hm1, " "); 
  /* Box around Module+Honeywell PS */
  obj = fl_add_box(FL_DOWN_BOX, x_mux-w0, y_mux-w0, 2*offs+6*w0+n_mux*wx1+wx2+n_sensor*wh1, 4*offs+3*w0+4*h, " "); 

  /* Honeywell */
  create_sensor_panel();
  /* Tensions */
  create_tension_panel();
  /* Flipper */
  create_Fvalve_panel();
  /* UGFV Board */
/*   create_board_panel(); */
  /* Module */
  create_module_panel();
  /* Multiplexer */
  create_mux_panel();

  msg = 0;
  MESSAGE = obj = fl_add_text(FL_NORMAL_TEXT,5,HH-30,WW-50,28," ");
  fl_set_object_lsize(obj,FL_LARGE_SIZE);
  fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);

/*   yes = fl_add_button(FL_NORMAL_BUTTON, w-55, 5, 50, 30, "Q\010uit"); */
/*   fl_set_object_lsize(yes,FL_MEDIUM_SIZE); */
/*   fl_set_object_lstyle(yes,FL_BOLD_STYLE); */

/*   obj = fl_add_button(FL_NORMAL_BUTTON, w-110, 5, 50, 30, "M\010ap"); */
/*   fl_set_object_lsize(obj,FL_MEDIUM_SIZE); */
/*   fl_set_object_lstyle(obj,FL_BOLD_STYLE); */
/*   fl_set_object_callback(obj, open_map_callback, 1); */

  add_author( WW-30, 35, date, author);
  end_form(MAIN);
  boards.ena.wri.cod = 0x400000;
  printf("  Setting default for UGFV   [code = 0x%08X]\n", boards.ena.wri.cod);
  update_objects();

/*   fl_end_form(); */
/*   fdui->manifold->fdui = fdui; */
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
  int w = 510, h = 130;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) 
    {
      mapform = fl_bgn_form(FL_NO_BOX, w, h);
      obj = fl_add_box(FL_FLAT_BOX,0,0,w,h," ");
     obj = fl_add_pixmap(FL_NORMAL_PIXMAP,0,0,w,h," ");
      fl_set_pixmap_file(obj, "schemes/segment.xpm");
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
void create_tension_panel() 
{

  FL_OBJECT *obj;
  int i, x, y;
  int x0, y0;

  if ( P > 2 ) printf(" *** invoking create_tension_panel\n");

  x0 = x_tens;
  y0 = y_tens;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,2*w0+wt2+2*wt1,offs+2*w0+(2+n_tensions)*h," ");
  
  x = x0 + w0;
  y = y0 + w0;
  obj = add_frame_box(x, y, wt2+2*wt1, h, "POWER SUPPLY", size1);
  y = y + h + 2;
/*   obj = add_frame_box(x, y, wt1, h, " ", size1); */
/*   x = x + wt1; */
/*   obj = add_frame_box(x, y, wt1/2., h, "AC", size1); */
/*   x = x + wt1/2.; */
/*   obj = add_frame_box(x, y, wt1/2., h, "BD", size1); */
/*   x = x + wt1/2.; */
/*   obj = add_frame_box(x, y, wt1/2., h, "AC", size1); */
/*   x = x + wt1/2.; */
/*   obj = add_frame_box(x, y, wt1/2., h, "BD", size1); */
/*   y = y + h; */

  for (i=0; i<n_tensions; i++) {
    x = x0 + w0;
    // colonna col nome delle tensioni
    obj = add_frame_box(x, y, wt2, h, tension[i].nam, size1);
    x = x + wt2;
    // colonna di enable AC
    tension[i].WRI =
      obj = add_switch_button(x, y, wt1, h, "X", size2);
    fl_set_object_callback(obj, tension_WRI_callback, i);
    x = x + wt1;
   // colonna lettura stato di abilitazione
    tension[i].REA =
      obj = add_data_box(x, y, wt1, h, "Y", size2);
    x = x0 + w0;
    y = y + h;
  }
  
  obj = add_frame_box(x, y, wt1, h, " ", size1);
  x = x + wt1;
  // tasto di enable
  tensions.ena.SET =
    obj = add_normal_button(x, y, wt1, h, "ENA", size1);
  fl_set_object_callback(obj, tensions_ena_SET_callback, 0);
  x = x + wt1;

  // tasto lettura stato di abilitazione
  tensions.ena.GET =
    obj = add_normal_button(x, y, wt1, h, "READ", size1);
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
  if ( P > 3 ) printf(" tensions.ena.wri.cod = 0x%04X\n", tensions.ena.wri.cod);
  if ( P > 3 ) printf(" tension[ind].msk = 0x%04X\n", tension[ind].msk);

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
 * Function:      tensions_ena_SET_callback(FL_OBJECT*, int)
 * Description:   callback for tensions enable
 *                enable: 1P00 1000 0.BD.AC.0 1000 V6A.V6B.V18A.V18B 0000 0000 0000
 *                        (8/C) (8)  (4/2)     (8) +---- (tensions.ena.wri.cod) ----+
 *
 **************************************************************************************/
void tensions_ena_SET_callback(FL_OBJECT *obj, long data) 
{

  int16 ena_tensions_bits = 0x84C8;
  int32 wri, rea, mask;
  int16 err;
  int p, bin_wri;
  char a[80];

  if ( P > 2 ) printf(" *** invoking tensions_ena_SET_callback\n");

  chk = 1;
  msg = -1;
  update_objects();

  // AC
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
  bin_wri = sprintf_binary32(a, wri, -32);

  printf("\n Enabling selected tensions (AC)\n");
  if ( P > 0 ) printf("    [code = %s]\n", a); 

  if (HC_ID & 0x01) {
    clk = 1 ; dat = 1;
  }
  else {
    clk = 3 ; dat = 3;
  }
  bus = (clk<<4)|dat; 

  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);


  mask = 0xC000;
/*   tensions.ena.wri.was = tensions.ena.wri.cod ^ mask; */
/*   tensions.ena.wri.sta = status_OK;  */
 
  tensions.ena.wri.was = tensions.ena.wri.cod ^ mask;
  tensions.ena.wri.sta = status_OK;  
 
  if (err) sta = 0;
  else     sta = 1;

  msg = 1;
  update_objects();
}



/**************************************************************************************
 *
 * Function:      tensions_ena_GET_callback(FL_OBJECT*, int)
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
 *                codes: 0xC4C00000, C4A00000
 *                answer: ---- ---- ---- ---- 12Vac ---- ---- ----
 *                answer: ---- ---- ---- ---- 12Vbd ---- ---- ----
 *
 **************************************************************************************/
void read_tensions_ena_data(void) 
{

  int32 wri = 0xC4C00000;
  int32 rea, mask, rea_en;
  chk = 1;
  int16 err;
  char a[80], c[80];
  int la, lc;

  if ( P > 2 ) printf(" *** invoking read_tensions_ena_data\n");

  if (HC_ID & 0x01) {
    clk = 1 ; dat = 1;
  }
  else {
    clk = 3 ; dat = 3;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

  // rea = 0x3000;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);

  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

//  mask = 0x8000;
  mask = 0xC000;
  rea_en = mask ^ rea;

  tensions.ena.rea.cod = rea_en;

  printf("\n Reading tensions (AC) enable status\n");

  if ( P > 0 ) printf("    [code in answer:  0x%04X]\n", rea);

  lc = sprintf_binary32(c,tensions.ena.rea.cod,-32);

  if ( P > 3 ) printf(" tensions.ena.rea.cod = %08X = %s\n", tensions.ena.rea.cod, c);

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
  //  int16 mask = 0x8000, code;
  int16 mask = 0xC000, code = 0xFFFF;
    
  if ( P > 2 ) printf(" *** invoking show_tension_panel\n");

  // colonna 'Enable' AC
  for (i=0; i<n_tensions; i++) {
    obj = tension[i].WRI;
/*     code = tensions.ena.wri.cod ^ mask; */
    if ( i == 0 || i == 1 ) {
      code = tensions.ena.wri.cod ^ mask;
    } else if ( i == 2 || i == 3 ) {
      code = tensions.ena.wri.cod;
    }
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
  

  // Colonna "Status" AC
  for (i=0; i<n_tensions; i++) {
    if ( P > 3 ) printf(" tension: %s \n", tension[i].nam);

    obj = tension[i].REA;
    m  = tension[i].msk & tensions.ena.rea.cod;
    m1 = tensions.ena.wri.was & tension[i].msk;
    m2 = tensions.ena.rea.cod & tension[i].msk;

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
 * Function:      create_sensor_panel(int, int)
 * Description:   create Honeywell pressure sensors panel
 *
 **************************************************************************************/
void create_sensor_panel() 
{

  FL_OBJECT *obj;
  int i, x, y;
  int n_sens_half = 0;
  int x0,  y0;

  if ( P > 2 ) printf(" *** invoking creat_sensor_panel\n");

  x0 = x_honey;
  y0 = y_honey;

  n_sens_half = n_sensor / 2.;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,2*w0+n_sensor*wh1, offs+2*w0+4*h," ");
  
  x = x0 + w0;
  y = y0 + w0;
  obj = add_frame_box(x, y, n_sensor*wh1, h, "HONEYWELL PRESSURE SENSOR", size1);

  y = y + h + offs;
  for (i=0; i<n_sensor; i++) {
    obj = add_frame_box(x, y, wh1, h, sensor[i].nam, size1);
    x = x + wh1;
  }
  y = y + h;
  x = x0 + w0;
  for (i=0; i<n_sensor; i++) {
    sensor[i].VAL =
      obj = add_data_box(x, y, wh1, h, "1234.", size2);
    x = x + wh1;
  }
/*   y = y + h; */
/*   x = x0 + w0; */
/*   for (i=n_sens_half; i<n_sensor; i++) { */
/*     obj = add_frame_box(x, y, wh1, h, sensor[i].nam, size1); */
/*     x = x + wh1; */
/*   } */
/*   y = y + h; */
/*   x = x0 + w0; */
/*   for (i=n_sens_half; i<n_sensor; i++) { */
/*     sensor[i].VAL = */
/*       obj = add_data_box(x, y, wh1, h, "1234.", size2); */
/*     x = x + wh1; */
/*   } */

/*     sensor[i].BAR = */
/*       obj = add_data_box(x, y, w3, h, "1234.5", size2); */
/*     x = x + w3; */
  x = x0 + w0;
  y = y + h;
  sensors.GET =
      obj = add_normal_button(x, y, n_sensor*wh1, h, "READ", size1);
    fl_set_object_callback(obj, sensor_GET_callback, i);
}


/**************************************************************************************
 *
 * Function:      sensor_GET_callback(FL_OBJECT*, long)
 * Description:   manage Honeywell PRESSURE sensors read button
 *
 **************************************************************************************/
void sensor_GET_callback(FL_OBJECT* obj, long data) 
{


  if ( P > 2 ) printf(" *** invoking sensor_GET_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (sensors.rea.mon) {
      sensors.rea.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      read_sensor_data();
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    sensors.rea.mon = 1 - sensors.rea.mon;
    update_objects();
  }

}


/**************************************************************************************
 *
 * Function:      read_sensor_data()
 * Description:   read Honeywell sensors from ADCs
 *                AC-HOT  from  0 to  4
 *                AC-COLD from  8 to 12
 *
 **************************************************************************************/
void read_sensor_data(void) 
{

  int16 err;
  int16 adc[256];
  int n, i;

  // if (!initialize_EPP()) exit(1);

  if ( P > 2 ) printf(" *** invoking read_sensor_data\n");

  read_ADC(adc, &err);            // modified by AL

/*   for (j=0; j<n_board; j++) { */
/*     if ( boards.ena.wri.cod == board[j].msk) b_ind = j; */
/*   } */
/*   if ( ! (b_ind == 0 || b_ind == 1) ) { */
/*     printf(" WARNING! Some setting is missing\n"); */
/*   } */

  n = 30;
 
 /* Hot */
  if (HC_ID & 0x01) {
    for (i=0; i<n; i++) {
      if (!(i % 8)) printf("\n");
      printf("[%2d]: %5d  ", i, adc[i]);
      if ( i < 5 ) {
	sensor[i].rea.val = adc[i];
	sensor[i].rea.sta = status_OK;
      }
/*       else if (i == 16 || i==17 || i==18 || i==19 || i==20 || i==21 ) { */
/* 	sensor[i-10].rea.val = adc[i]; */
/* 	sensor[i-10].rea.sta = status_OK; */
/*       } */
    }
    printf("\n");
  }
  
  /* Cold */
  else {
    for (i=0; i<n; i++) {
      if (!(i % 8)) printf("\n");
      printf("[%2d]: %5d  ", i, adc[i]);
      if ( i == 8 || i==9 || i==10 || i==11 || i==12 ) {
	sensor[i-8].rea.val = adc[i];
	sensor[i-8].rea.sta = status_OK;
      }
/*       else if ( i>23 ) { */
/* 	sensor[i-18].rea.val = adc[i]; */
/* 	sensor[i-18].rea.sta = status_OK; */
/*       } */
    }
    printf("\n");
    
  }

  if (err) sta = 0;
  else     sta = 1;
}


/**************************************************************************************
 *
 * Function:      pressure_bar(int, float) 
 * Description:   convert Honeywell mVolt into bar
 // Controllare se il range di pressione corrisponde alla conversione usata...
 // In box S:
 // P2a=V2a*300 /(4.517*14.5) (bar/Volt)
 // P1a=V2a*3000/(4.515*14.5) (bar/Volt)
 // P1b=V2b*3000/(4.515*14.5) (bar/Volt)
 *
 **************************************************************************************/
float pressure_bar(int ind, float p_mV) 
{

  const float conv_factor = 300./(4.515*14.5)/1000.;
  float p_bar;

  if ( P > 2 ) printf(" *** invoking pressure_bar\n");
  p_bar = p_mV * conv_factor; //(bar/Volt)
  return p_bar;

}
/**************************************************************************************
 *
 * Function:      show_sensor_panel(void) 
 * Description:   show Honeywell pressure sensors panel
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
    else                                sprintf(b, "%5.0f", sensor[i].rea.val);
    fl_set_object_label(obj, b);

  // settaggi per la finestrella della pressione in bar
/*     obj = sensor[i].BAR; */
/*     if (sensor[i].rea.sta == status_NN || sensor[i].rea.sta == status_NA) { */
/*       fl_set_object_lcolor(obj, FL_BLACK); */
/*       fl_set_object_color(obj, FL_WHEAT, 0); */
/*     } */
/*     if (sensor[i].rea.sta == status_KO) { */
/*       fl_set_object_lcolor(obj, FL_WHITE); */
/*       fl_set_object_color(obj, FL_BLACK, 0); */
/*     } */
/*     if (sensor[i].rea.sta == status_OK) { */
/*       fl_set_object_lcolor(obj, FL_BLACK); */
/*       fl_set_object_color(obj, FL_WHEAT, 0); */
/*     } */
/*     if (sensor[i].rea.sta == status_NN) sprintf(b, " "); */
/*     else                                sprintf(b, "%7.1f", sensor[i].rea.bar); */
/*     fl_set_object_label(obj, b); */
  }

  // settaggi per il tastino "READ"
  obj = sensors.GET;
  if (sensors.rea.mon) {
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
 * Function:      create_Fvalve_panel(int, int)
 * Description:   create Flipper valves panel
 *
 **************************************************************************************/
void create_Fvalve_panel() 
{

  FL_OBJECT *obj;
  int i, x, y;
  int half_n_Fv = 0;
  int x0,  y0;
  
  if ( P > 2 ) printf(" *** invoking creat_Fvalve_panel\n");

  y0 = y_flip;
  x0 = x_mod+2*w0+wm1+wm2+offs;

  half_n_Fv = n_Fvalve / 2.;

  obj = fl_add_box(FL_DOWN_BOX, x0, y0, 2*w0+3*wf1+2*w0+wf2+2*w0, offs+2*w0+2*h+(half_n_Fv+1)*hf1," ");
  obj = fl_add_box(FL_DOWN_BOX, x0+w0, y0+h+w0+offs, 2*w0+3*wf2, (half_n_Fv+1)*hf1+h," ");
  obj = fl_add_box(FL_DOWN_BOX, x0+3*wf1+3*w0, y0+h+w0+offs, 2*w0+wf2, (half_n_Fv+1)*hf1+h," ");
  
  x = x0 + w0;
  y = y0 + w0;
  obj = add_frame_box(x, y, 2*w0+3*wf1+2*w0+wf2, h, "VALVES", size1);

  y = y + 2*h - w0;
  yf1 = y;
  x = 4*w0 + x + wf1/3.;
  fl_bgn_group();
  for (i=0; i<half_n_Fv; i++) { 
    Fvalve[i].ena.WRI =
      obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,wf1,h,Fvalve[i].nam);
    fl_set_object_callback(obj, Fvalve_WRI_callback, i);
    x = x + wf1; 
  }

  y = y + hf1;
  x = x0 + 3*w0;
  for (i=half_n_Fv; i<n_Fvalve; i++) { 
    Fvalve[i].ena.WRI =
      obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,wf1,h,Fvalve[i].nam);
    fl_set_object_callback(obj, Fvalve_WRI_callback, i);
    x = x + wf1; 
  }
  fl_end_group();

  y = y + hf1 + offs;
  x = x0 + 4*w0;
  Fvalves.OPN =
    obj = add_normal_button(x, y, wf1+wf1/4., h, "OPEN", size1);
  fl_set_object_callback(obj, Fvalve_OPN_callback, 0); 
  x = x + wf1 + wf1/4. + w0;
  Fvalves.CLS =
    obj = add_normal_button(x, y, wf1+wf1/4., h, "CLOSE", size1);
  fl_set_object_callback(obj, Fvalve_CLS_callback, 0);
 
  x = x0 + 3*wf1 + 4*w0;
  y = yf1 + 4 - h/2.;

  Fvalves.OPNA =
    obj = add_normal_button(x, y, wf2, 2*h-4, "OPEN\nALL", size1);
  fl_set_object_callback(obj, Fvalve_all_OPN_callback, 0); 

  y = y + 2*h;
  Fvalves.CLSA =
    obj = add_normal_button(x, y, wf2, 2*h-4, "CLOSE\nALL", size1);
  fl_set_object_callback(obj, Fvalve_all_CLS_callback, 0); 


}





/**************************************************************************************
 *
 * Function:      Fvalve_WRI_callback(int, int)
 * Description:   manage single Marotta valve enable/disable button
 *
 **************************************************************************************/
void Fvalve_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking Fvalve_WRI_callback\n");
  //printf("  ind %3d \n",ind);
  Fvalves.ena.wri.cod = Fvalve[ind].msk;
    
  printf("\n Setting opening of valve %s\n", Fvalve[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", Fvalves.ena.wri.cod);
  
  msg = 0;
  update_objects();
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
      open_Fvalve();
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
 * Function:      Fvalve_all_OPN_callback(FL_OBJECT*, long)
 * Description:   manage all Flipper open button
 *
 **************************************************************************************/
void Fvalve_all_OPN_callback(FL_OBJECT* obj, long data) 
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
      open_all_Fvalve();
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
      close_Fvalve();
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
 * Function:      Fvalve_all_CLS_callback(FL_OBJECT*, long)
 * Description:   manage Flipper close button
 *
 **************************************************************************************/
void Fvalve_all_CLS_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking Fvalve_all_CLS_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (Fvalve[ind].cls.mon) {
      Fvalve[ind].cls.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      close_all_Fvalve();
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
 * Function:      open_Fvalve()
 * Description:   callback for Flipper open button
 *
 **************************************************************************************/
void open_Fvalve(void) 
{

    int32 rea, wri = 0x800CF800, p;
    chk = 1;
    int16 err;
  if ( P > 2 ) printf(" *** invoking open_Fvalve\n");

  printf("\n Opening valve\n");

  if (HC_ID & 0x01) {
    clk = 1 ; dat = 1;
  }
  else {
    clk = 3 ; dat = 3;
  }
  bus = (clk<<4)|dat; 

  wri += boards.ena.wri.cod;
  if ( P > 3 ) printf(" boards.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", boards.ena.wri.cod, wri);

  wri += modules.ena.wri.cod;
  if ( P > 3 ) printf(" modules.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", modules.ena.wri.cod, wri);

  wri += Fvalves.ena.wri.cod;
  if ( P > 3 ) printf(" Fvalves.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", Fvalves.ena.wri.cod, wri);

  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);

  if ( P > 0 ) printf("       [code = 0x%08X]\n",wri);

  if ( boards.ena.wri.cod && modules.ena.wri.cod && Fvalves.ena.wri.cod ) {
    exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);
  }
  else {
    printf(" WARNING! Some setting is missing\n");
  }

  if (err) sta = 0;
  else     sta = 1;

}


/**************************************************************************************
 *
 * Function:      open_all_Fvalve()
 * Description:   open all flipper valves on the selected module
 *
 **************************************************************************************/
void open_all_Fvalve(void) 
{

  int32 rea, wri = 0x800CF83F, p;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking open_all_Fvalve\n");

  printf("\n Opening all valves\n");

  if (HC_ID & 0x01) {
    clk = 1 ; dat = 1;
  }
  else {
    clk = 3 ; dat = 3;
  }
  bus = (clk<<4)|dat; 

  wri += boards.ena.wri.cod;
  if ( P > 3 ) printf(" boards.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", boards.ena.wri.cod, wri);

  wri += modules.ena.wri.cod;
  if ( P > 3 ) printf(" modules.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", modules.ena.wri.cod, wri);

  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);

  if ( P > 0 ) printf("       [code = 0x%08X]\n",wri);

  if ( boards.ena.wri.cod && modules.ena.wri.cod ) {
    exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);
  }
  else {
    printf(" WARNING! Some setting is missing\n");
  }

  if (err) sta = 0;
  else     sta = 1;

}


/**************************************************************************************
 *
 * Function:      close_Fvalve(int)
 * Description:   callback for Flipper close button
 *
 **************************************************************************************/
void close_Fvalve(void) 
{

 
  int32 rea, wri = 0x800AF800, p;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking close_Fvalve\n");

  printf("\n Closing valve\n");
  
  if (HC_ID & 0x01) {
    clk = 1 ; dat = 1;
  }
  else {
    clk = 3 ; dat = 3;
  }
  bus = (clk<<4)|dat; 

  wri += boards.ena.wri.cod;
  if ( P > 3 ) printf(" boards.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", boards.ena.wri.cod, wri);

  wri += modules.ena.wri.cod;
  if ( P > 3 ) printf(" modules.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", modules.ena.wri.cod, wri);

  wri += Fvalves.ena.wri.cod;
  if ( P > 3 ) printf(" Fvalves.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", Fvalves.ena.wri.cod, wri);

  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);

  if ( P > 0 ) printf("       [code = 0x%08X]\n",wri);

  if ( boards.ena.wri.cod && modules.ena.wri.cod && Fvalves.ena.wri.cod ) {
    exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);
  }
  else {
    printf(" WARNING! Some setting is missing\n");
  }

  if (err) sta = 0;
  else     sta = 1;

}


/**************************************************************************************
 *
 * Function:      close_all_Fvalve(int)
 * Description:   close all flipper valves on the selected module
 *
 **************************************************************************************/
void close_all_Fvalve(void) 
{

 
  int32 rea, wri = 0x800AF83F, p;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking close_all_Fvalve\n");

  printf("\n Closing all valves\n");
  
  if (HC_ID & 0x01) {
    clk = 1 ; dat = 1;
  }
  else {
    clk = 3 ; dat = 3;
  }
  bus = (clk<<4)|dat; 

  wri += boards.ena.wri.cod;
  if ( P > 3 ) printf(" boards.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", boards.ena.wri.cod, wri);

  wri += modules.ena.wri.cod;
  if ( P > 3 ) printf(" modules.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", modules.ena.wri.cod, wri);

  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);

  if ( P > 0 ) printf("       [code = 0x%08X]\n",wri);

  if ( boards.ena.wri.cod && modules.ena.wri.cod ) {
    exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);
  }
  else {
    printf("\n WARNING! Some setting is missing\n");
  }

  if (err) sta = 0;
  else     sta = 1;

}


/**************************************************************************************
 *
 * Function:      show_Fvalve_panel(void)
 * Description:   show Flipper valves panel
 *
 **************************************************************************************/
void show_Fvalve_panel(void) 
{

  int i;

  if ( P > 2 ) printf(" *** invoking show_Fvalve_panel\n");

  for (i=0; i<n_Fvalve; i++) {
    ;
  }
}




/**************************************************************************************
 *
 * Function:      create_board_panel(int, int)
 * Description:   create UGFV boards panel
 *
 **************************************************************************************/
void create_board_panel() 
{

  FL_OBJECT *obj;
  int i, x, y;
  int x0,  y0;
  
  if ( P > 2 ) printf(" *** invoking create_board_panel\n");

  x0 = x_boa;
  y0 = y_boa;

  obj = fl_add_box(FL_DOWN_BOX,x0,y0,2*w0+n_board*wb1,offs+2*w0+2*h," ");
  
  x = x0 + w0;
  y = y0 + w0;
  obj = add_frame_box(x, y, n_board*wb1, h, "UGFV BOARD", size1);
  y = y + offs + h;
  x = x + wb1/5.;
  fl_bgn_group();
  for (i=0; i<n_board; i++) { 
    board[i].ena.WRI =
      obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,wb1,h,board[i].nam);
    fl_set_object_callback(obj, board_WRI_callback, i);
    x = x + wb1; 
  }
  fl_end_group();
}





/**************************************************************************************
 *
 * Function:      board_WRI_callback(int, int)
 * Description:   manage board enable/disable button
 *
 **************************************************************************************/
void board_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking board_WRI_callback\n");
  //printf("  ind %3d \n",ind);
/*   boards.ena.wri.cod = board[ind].msk; */
  boards.ena.wri.cod = 0x400000;
    
  printf("\n Switching on UGFV board %s\n", board[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%08X]\n", boards.ena.wri.cod);
  
  msg = 0;
  update_objects();
}

/**************************************************************************************
 *
 * Function:      show_board_panel(void)
 * Description:   show UGFV boards panel
 *
 **************************************************************************************/
void show_board_panel(void) 
{

  int i;

  if ( P > 2 ) printf(" *** invoking show_board_panel\n");

  for (i=0; i<n_board; i++) {
    //
  }
}




/**************************************************************************************
 *
 * Function:      create_module_panel(int, int)
 * Description:   create modules panel
 *
 **************************************************************************************/
void create_module_panel() 
{

  FL_OBJECT *obj;
  int i, x, y;
  int half_n_mod = 0;
  int x0,  y0;
  
  if ( P > 2 ) printf(" *** invoking create_module_panel\n");

  x0 = x_mod;
  y0 = y_mod;

  half_n_mod = n_module/2.;

  obj = fl_add_box(FL_DOWN_BOX, x0, y0, 2*w0+wm1+wm2, offs+2*w0+2*h+(n_module-1)*hm1," ");

  x = x0 + w0;
  y = y0 + w0;
  obj = add_frame_box(x, y, wm1+wm2, h, "MODULE", size1);
  y = y + offs + h;
  x = x + wm2;
  ym1 = y;

  fl_bgn_group();
  for (i=0; i<n_module; i++) {
    module[i].ena.WRI =
      obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,wm1,h,module[i].nam);
    fl_set_object_callback(obj, module_WRI_callback, i);
    y = y + hm1;
  }
/*   for (i=0; i<half_n_mod; i++) { */
/*     module[i].ena.WRI = */
/*       obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,wm1,h,module[i].nam); */
/*     fl_set_object_callback(obj, module_WRI_callback, i); */
/*     y = y + hm1; */
/*   } */
/*   y = ym1; */
/*   x = x + wm1 + wm2/2.; */
/*   for (i=half_n_mod; i<n_module; i++) { */
/*     module[i].ena.WRI = */
/*       obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,wm1,h,module[i].nam); */
/*     fl_set_object_callback(obj, module_WRI_callback, i); */
/*     y = y + hm1; */
/*   } */
  fl_end_group();
}
 


/**************************************************************************************
 *
 * Function:      module_WRI_callback(int, int)
 * Description:   manage single module enable/disable button
 *
 **************************************************************************************/
void module_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking module_WRI_callback\n");
  //printf("  ind %3d \n",ind);
  modules.ena.wri.cod = module[ind].msk;
    
  printf("\n Switching on module %s\n", module[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%08X]\n", modules.ena.wri.cod);
  
  msg = 0;
  update_objects();
}

/**************************************************************************************
 *
 * Function:      show_module_panel(void)
 * Description:   show module panel
 *
 **************************************************************************************/
void show_module_panel(void) 
{

  int i;

  if ( P > 2 ) printf(" *** invoking show_module_panel\n");

  for (i=0; i<n_module; i++) {
    //
  }
}




/**************************************************************************************
 *
 * Function:      create_mux_panel(int, int)
 * Description:   create muxs panel
 *
 **************************************************************************************/
void create_mux_panel() 
{

  FL_OBJECT *obj;
  int i, x, y;
  int half_n_mux = 0;
  int x0,  y0;

  if ( P > 2 ) printf(" *** invoking create_mux_panel\n");

  half_n_mux = n_mux /2.;

  x0 = x_mux;
  y0 = y_mux;

  obj = fl_add_box(FL_DOWN_BOX, x0, y0, 2*w0+n_mux*wx1+wx2, offs+2*w0+4*h," ");

  x = x0 + w0;
  y = y0 + w0;
  obj = add_frame_box(x, y, n_mux*wx1+wx2, h, "MODULE", size1);

  y = y + h + h/2.;
  x = x + wx2;
  yx1 = y;
  fl_bgn_group();
  for (i=0; i<n_mux; i++) { 
    mux[i].ena.WRI =
      obj = fl_add_checkbutton(FL_RADIO_BUTTON,x,y,wx1,h,mux[i].nam);
    fl_set_object_callback(obj, mux_WRI_callback, i);
    x = x + wx1;
  }
  fl_end_group();

  y = y + h + h/2.+ 3*offs/2.; 
  x = x0 + w0;
  //y = y - 1.2*h;
  muxs.SEL =
    obj = add_normal_button(x, y, wx2+n_mux*wx1, h, "SELECT", size1);
  fl_set_object_callback(obj, mux_SEL_callback, 0); 

}



/**************************************************************************************
 *
 * Function:      mux_WRI_callback(int, int)
 * Description:   manage single mux enable/disable button
 *
 **************************************************************************************/
void mux_WRI_callback(FL_OBJECT *obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking mux_WRI_callback\n");
  if ( P > 3 ) printf(" mux[%d].msk = %08x ", ind, mux[ind].msk);

  //muxs.ena.wri.cod = 0x00000000;

  muxs.ena.wri.cod = mux[ind].msk << 16;
  if ( P > 3 ) printf(" muxs.ena.wri.cod %04x ", muxs.ena.wri.cod);
  muxs.ena.wri.cod += mux[ind].msk << 13;
  if ( P > 3 ) printf(" muxs.ena.wri.cod %04x ", muxs.ena.wri.cod);
  muxs.ena.wri.cod += mux[ind].msk <<  10;
  if ( P > 3 ) printf(" muxs.ena.wri.cod %04x ", muxs.ena.wri.cod);
  muxs.ena.wri.cod += mux[ind].msk <<  7;
    
  printf("\n Switching on mux %s\n", mux[ind].nam);
  if ( P > 0 ) printf("    [code = 0x%04X]\n", muxs.ena.wri.cod);
  
  msg = 0;
  update_objects();
}


/**************************************************************************************
 *
 * Function:      Fvalve_OPN_callback(FL_OBJECT*, long)
 * Description:   manage Flipper open button
 *
 **************************************************************************************/
void mux_SEL_callback(FL_OBJECT* obj, long data) 
{

  int ind = data;

  if ( P > 2 ) printf(" *** invoking mux_SEL_callback\n");

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (mux[ind].opn.mon) {
      mux[ind].opn.mon = 0;
      msg = 0;
      update_objects();
    }
    else {
      msg = -1;
      update_objects();
      select_mux();
      msg = 1;  
      update_objects();
    }
  }

  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) {
    mux[ind].opn.mon = 1 - mux[ind].opn.mon;
    update_objects();
  }
}


/**************************************************************************************
 *
 * Function:      select_mux()
 * Description:   callback for Flipper open button
 *
 **************************************************************************************/
void select_mux(void) 
{

  int32 rea, wri = 0x80880000, p;
  chk = 1;
  int16 err;

  if ( P > 2 ) printf(" *** invoking select_mux\n");

  printf("\n Selecting multiplexer\n");

  wri += boards.ena.wri.cod;
  if ( P > 3 ) printf(" boards.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", boards.ena.wri.cod, wri);

  wri += muxs.ena.wri.cod;
  if ( P > 3 ) printf(" muxs.ena.wri.cod = 0x%08X, now wri = 0x%08X\n", muxs.ena.wri.cod, wri);

  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) printf(" parity = %d, now wri = %08X\n", p, wri);

  if ( P > 0 ) printf("    [code = 0x%08X]\n",wri);

  //  if ( boards.ena.wri.cod && muxs.ena.wri.cod ) {

  if (HC_ID & 0x01) {
    clk = 1 ; dat = 1;
  }
  else {
    clk = 3 ; dat = 3;
  }
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(n_CMDS, &bus, &wri, &rea, &chk, &err);

    // }
    //else {
    //printf(" WARNING! Some setting is missing\n");
    // }

  if (err) sta = 0;
  else     sta = 1;

}





/**************************************************************************************
 *
 * Function:      show_mux_panel(void)
 * Description:   show mux panel
 *
 **************************************************************************************/
void show_mux_panel(void) 
{

  int i;

  if ( P > 2 ) printf(" *** invoking show_mux_panel\n");

  for (i=0; i<n_mux; i++) {
    //
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

  /* Honeywell */
  show_sensor_panel();
  /* Tensions */
  show_tension_panel();
  /* Flipper */
  show_Fvalve_panel();
  /* Board */
/*   show_board_panel(); */
  /* Modules */
  show_module_panel();
  /* Multiplexers */
  show_mux_panel();

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

 
  /* Honeywell */
  for (i=0; i<n_sensor; i++) {
    sensor[i].rea.sta = status_NN;
  }
  sensors.rea.mon = 0;


  /* Tensions */
  for (i=0; i<n_tensions; i++) {
    tension[i].wri.sta = status_NN;
  }

  tensions.ena.wri.cod = 0xC000;
  tensions.ena.wri.sta = status_NN;
  tensions.ena.rea.mon = 0;
  tensions.ena.rea.sta = status_NN;

  /* Flipper valves */
  for (i=0; i<n_Fvalve; i++) {
    Fvalve[i].opn.mon = 0;
    Fvalve[i].opn.sta = status_NN;
    Fvalve[i].cls.mon = 0;
    Fvalve[i].cls.sta = status_NN;
  }

  Fvalves.ena.wri.cod = 0x0000;
  Fvalves.ena.wri.sta = status_NN;

  /* Board */
  boards.ena.wri.cod = 0x0000;
  boards.ena.wri.sta = status_NN;

  /* Module */
  modules.ena.wri.cod = 0x0000;
  modules.ena.wri.sta = status_NN;

  /* Multiplexer */
  muxs.ena.wri.cod = 0x0000;
  muxs.ena.wri.sta = status_NN;



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
  printf(" **********        This is the controller for Manifolds       ***********\n");
  printf(" ************************************************************************\n");
  template(argc, &argv[0]);
  
  return 0;
}
/*   int nr; */
/*   printf("\033[2J\n"); */
/*   printf("\033[1;0H\n ******************************************************************************\n"); */
/*   printf(" **********            This is the controller for Manifold          ***********\n"); */
/*   printf(" ******************************************************************************\n"); */
/*   printf("\n Usage: segments [ Debug_Level ]\n"); */
/*   P = 0; */
/*   if (argc > 1) P = atoi(argv[1]); */
/*   printf("\n Debug_Level = %d\n", P); */
/*   printf("\n Using default initialization: clock = 0x%02X\n", clk); */
/*   printf("                               data  = 0x%04X\n\n", dat); */
/*   DEBUG_LEVEL = P; */

/*   nr = 0; */

/*   start_manifold(nr); */
/*   while ((but = fl_do_forms()) != yes) */
/*     ; */
/*   exit(2); */
/*   return 0; */
/* } */
