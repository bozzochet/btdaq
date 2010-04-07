// file fslib.c
//
// F.Spada - May-2004...
//
// Stolen by AL and reworked

#undef DEBUG_mytypes
#undef DEBUG_mykbhit
#undef VERBOSE_mykbhit

#include "fslib.h"
#include "forms.h"


/* UTILITIES */
/*************/

void add_author(int WW, int HH, char *date, char *author) {

  FL_OBJECT *obj;
  int x, y, w, h;
  char b[80];

  w = 30;
  h = 25;
  x = WW;
  y = HH;

  sprintf(b, "%s %s", author, date);

  obj = fl_add_box(FL_FLAT_BOX, x, y, w, h, b);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);
}

/**************************************************************************************
 * FB october 2005
 **************************************************************************************/
void increase_valve_count(char* b) {
  FILE* file;
  int x;
  char filename[50];

  sprintf(filename, "./log/%s.log", b);

  file = fopen(filename, "r");

  if(file==NULL) {
    file = fopen(filename, "w");
    fprintf(file, "%d", 1);
  }

  else {
    fscanf(file, "%d", &x);
    x += 1;
    fclose(file);

    file = fopen(filename, "w");
    
    fprintf(file, "%d", x);
    
    if(feof(file))
      {
	return;
      }
  }

  fclose(file);

}



/**************************************************************************************
 * FS may 2004
 **************************************************************************************/
int sprintf_binary32(char* b, int32 number, int mode) 
{

  int nb_of_digits;
  int i, j = 0, nb_of_ones = 0;

  if (mode  < 0) nb_of_digits = -mode;
  else if (mode == 0) nb_of_digits =  32;
  else nb_of_digits =  mode;

  //printf("------- char = %s, number = %08X, mode = %d\n",b,number,mode);

  for (i=0; i<nb_of_digits; i++) {
    if (mode < 0) {
      if (i != 0 && i % 4 == 0) {
        sprintf(b+j, " ");
        j++;
      }
    }
    //printf("------ i = %d\n",i);
    if (number & 1 << (nb_of_digits-1-i)) {
      nb_of_ones++;
      sprintf(b+j, "1");
      j++;
    }
    else {
      if (mode || nb_of_ones || i == nb_of_digits-1) {
        sprintf(b+j, "0");
        j++;
      }
    }
  }

  return strlen(b);
}



/**************************************************************************************
 * FS. may 2004
 **************************************************************************************/
int parity32( int32 code ) 
{
  int p = 1;
  int i;
  int32 mask;
  for (i=0; i<32; i++) {
    mask = 1 << (31 - i);
    mask &= code;
    mask = mask >> (31 - i);
    p ^= mask;
  }
  return p;
}



/**************************************************************************************
 * FS. mar 2005
 * Print sleeping time
 **************************************************************************************/
void wait(float t) 
{

/*   int i; */  
/*   printf("   Waiting %2.1f s: ", t); */
/*   for (i=0; i<(int16)t; i++) { */
/*     sleep(1); */
/*     printf("%d", (i+1)%10); */
/*   } */
/*   printf(" \n"); */

  usleep(t);

}


/**************************************************************************************
 * FS. oct 2004
 * Get marotta open time as float and return hex
 **************************************************************************************/
int16 get_hex_time( float t )
{

  int16 cod;

  if (t < 0.131) t = 0.131;
  if (t > 8585.085)  t = 8585.085;
  cod = t * 1000.0 / 131.;
  //printf(" ************ get_hex_time: cod %x, t %f\n", cod, t);

  return cod;

}


/**************************************************************************************
 * FS. oct 2004
 * Get heater open time as float and return hex
 **************************************************************************************/
int16 get_heat_time( float t )
{

  int16 cod;

  if (t < 0.0)  t = 0.0;
  if (t > 4369) t = 4369;
  cod = t * 15.;
  //printf(" ************ get_hex_time: cod %x, t %f\n", cod, t);

  return cod;

}





int HC_ID;                               // global variables
char *hc_config_file_name;               // global variables

static FL_OBJECT *HC_KIND;

static void HC_zero_callback(FL_OBJECT *obj, long data);
static void HC_kind_callback(FL_OBJECT *obj, long data);


/**************************************************************************************
 * FS. jul 2006
 * Create HOT/COLD panel for commanders
 **************************************************************************************/
void create_HOTCOLD_panel() 
{

  FL_OBJECT *obj;
  FILE *file;
  int x, y, w, h;
  int x0 = 200, y0 = 0, w1 = 45, h1 = 15, h2 = 20;
  
  file = fopen(hc_config_file_name, "r");
  if (file != NULL) {
    fread(&HC_ID, sizeof(HC_ID), 1, file);
    fclose(file);
  }

  x = x0;
  y = y0;
  w = 5 + w1 + 5;
  h = 5 + h1 + h2 + 5;
  
  obj = fl_add_box(FL_DOWN_BOX,x,y,w,h,"");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w1, h1, "SLOT", FL_SMALL_SIZE);

/*   obj = add_plain_text(x, y, 40, 20, "SLOT", FL_NORMAL_SIZE); */
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w1, h2, "");
    fl_set_object_callback(obj, HC_zero_callback, 0);

  x = x0 + 5;
  y = y + h1;
  
  HC_KIND =
  obj = add_switch_button(x, y, w1, h2, "", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, HC_kind_callback, 0);

}



/**************************************************************************************
 * FS. jul 2006
 * Show HOT/COLD panel for commanders
 **************************************************************************************/
void show_HOTCOLD_panel(void) 
{

  if (HC_ID & 0x01) fl_set_object_label(HC_KIND, "Hot");
  else              fl_set_object_label(HC_KIND, "Cold");

  //printf("\n Board slot: %d\n", HC_ID);

}


/**************************************************************************************
 * FS. jul 2006
 * Reset hot/cold switch
 **************************************************************************************/
static void HC_zero_callback(FL_OBJECT *obj, long data) 
{

  if (HC_ID & 0x01) HC_ID = 0x01;
  else              HC_ID = 0x00;

  update_hc_config_file();
  show_HOTCOLD_panel();

}



/**************************************************************************************
 * FS. jul 2006
 * Hot/cold switch
 **************************************************************************************/
static void HC_kind_callback(FL_OBJECT *obj, long data) 
{

  HC_ID ^= 0x01;

  update_hc_config_file();
  show_HOTCOLD_panel();

}



/**************************************************************************************
 * FS. jul 2006
 * update hot/cold configuration file
 **************************************************************************************/
void update_hc_config_file(void)
{

  FILE *file;

  file = fopen(hc_config_file_name, "w");
  if (file != NULL) {
    fwrite(&HC_ID, sizeof(HC_ID), 1, file);
    fclose(file);
  }

}












/* BASIC COMMANDS */
/******************/

/**************************************************************************************
 * FS. oct 2004
 * Enable one Marotta valve, disable all the others of its box.
 **************************************************************************************/
void enable_Marotta_valve( char* valve_nam, int16* sta, int8* chk )
{

#define n_marotta 21
  
  struct {
    char* nam;
    int16 msk;
    int16 bts;
  } valve[n_marotta] = {
    // box C
    {"V6a",        0x8000, 0x8828},
    {"V6b",        0x4000, 0x8828},
    {"V18a",       0x2000, 0x8828}, 
    {"V18b",       0x1000, 0x8828},
    {"V6a&V18a",   0xA000, 0x8828},
    {"V6b&V18b",   0x5000, 0x8828},
    // box S
    {"V1a",        0x8000, 0x8848},
    {"V2a",        0x4000, 0x8848},
    {"V3a",        0x2000, 0x8848},
    {"V4a",        0x1000, 0x8848},
    {"Sp1",        0x0800, 0x8848},
    {"V10a",       0x0400, 0x8848},
    {"V20a",       0x0200, 0x8848},
    {"V1b",        0x0100, 0x8848},
    {"V2b",        0x0080, 0x8848},
    {"V3b",        0x0040, 0x8848},
    {"V4b",        0x0020, 0x8848},
    {"V10b",       0x0010, 0x8848},
    {"V20b",       0x0008, 0x8848},
    {"Sp2",        0x0004, 0x8848},
    {"V20a&V20b",  0x0208, 0x8848},
  };
  
  int8 bus;

  int16 ena_marotta_bits;
  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char w[80];
  int valve_ind = 999;
  int uno = 1, zero = 0;
  int i;

  if ( P > 2 ) printf(" *** invoking enable_Marotta_valve\n");

  for (i=0; i<n_marotta; i++) {
    if ( strcasecmp(valve[i].nam, valve_nam)==0 )  valve_ind = i;
  }

  ena_marotta_bits = valve[valve_ind].bts;

  wri = ena_marotta_bits << 16;
  if ( P > 3 ) 
    printf("\n ena_marotta_bits = %04X, now wri = %08X\n", ena_marotta_bits, wri);
  wri += valve[valve_ind].msk;
  if ( P > 3 ) 
    printf("\n valve[valve_ind].msk = %08X, now wri = %08X\n", valve[valve_ind].msk, wri);
  p = parity32(wri);
  wri += p << 30;

  if ( P > 3 ) 
    printf(" \n parity %d, now wri = %08X\n", p, wri);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Enabling Marotta valve %s\n", valve[valve_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}



/**************************************************************************************
 * FS. oct 2004
 * Disable all Marotta valves in the given box. Args: "box C", "box S".
 **************************************************************************************/
void disable_Marotta_valves( char* box_nam, int16* sta, int8* chk )
{

#define n_box 2

  struct {
    char* nam;
    int32 wri;
  } box[n_box] = {
    // box C
    {"box C", 0xC8280000},
    {"box S", 0xC8480000}
  };

  int8 bus;
 
  int32 wri, rea;
  int16 err;
  int bin_wri;
  char w[80];
  int uno = 1, zero = 0;
  int i, box_ind = 999;

  if ( P > 2 ) printf(" *** invoking disable_Marotta_valves\n");

  for (i=0; i<n_box; i++) {
    if ( strcasecmp(box[i].nam, box_nam)==0 )  box_ind = i;
  }
  wri = box[box_ind].wri;

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Disabling Marotta valves in %s\n", box[box_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}





/**************************************************************************************
 * FS. oct 2004
 * Open Marotta valve for a time "t"
 **************************************************************************************/
void open_Marotta_valve( char* valve_nam, float t, int16* sta, int8* chk )
{
#define n_marotta 21
  
  struct {
    char* nam;
    int16 msk;
    int16 bts;
  } valve[n_marotta] = {
    // box C
    {"V6a",        0x600, 0x8028},
    {"V6b",        0xE00, 0x8028},
    {"V18a",       0x100, 0x8028}, 
    {"V18b",       0x900, 0x8028},
    {"V6a&V18a",   0x500, 0x8028},
    {"V6b&V18b",   0xD00, 0x8028},
    // box S
    {"V1a",        0x140, 0x8008},
    {"V2a",        0x940, 0x8008},
    {"V3a",        0x540, 0x8008},
    {"V4a",        0xD40, 0x8008},
    {"Sp1",        0x340, 0x8008},
    {"V10a",       0xB40, 0x8008},
    {"V20a",       0x740, 0x8008},
    {"V1b",        0xF40, 0x8008},
    {"V2b",        0x0C0, 0x8008},
    {"V3b",        0x8C0, 0x8008},
    {"V4b",        0x4C0, 0x8008},
    {"V10b",       0xCC0, 0x8008},
    {"V20b",       0x2C0, 0x8008},
    {"Sp2",        0xE40, 0x8008},
    {"V20a&V20b",  0xAC0, 0x8008},
  };

  int8 bus;

  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char w[80];
  int uno = 1, zero = 0;
  int i, valve_ind = 999;
  int16 time_cod, inv_time_cod;

 if ( P > 2 ) printf(" *** invoking open_Marotta_valve\n");

  time_cod = get_hex_time(t);
  if ( P > 3 ) 
    printf(" time_cod %X\n", time_cod);

  inv_time_cod = invert_bit_order(time_cod);

  for (i=0; i<n_marotta; i++) {
    if ( strcasecmp(valve[i].nam, valve_nam)==0 )  valve_ind = i;
  }

  wri = ( valve[valve_ind].msk + valve[valve_ind].bts ) << 16;
  if ( P > 3 ) 
    printf(" wri = %08X\n", wri);
  wri += inv_time_cod;
  if ( P > 3 ) 
    printf(" with time code: wri = %08X\n", wri);
  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 ) 
    printf(" with parity p = %d,  wri = %08X\n", p, wri);

  bin_wri = sprintf_binary32(w, wri, -32);
  printf("\n Opening Marotta valve %s for %3.1f second(s)\n", valve[valve_ind].nam, t);
  if ( P > 0 )
    printf("     [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);
  //  if(!err) increase_valve_count(valve[valve_ind].nam);

  if (err) *sta = zero;
  else     *sta = uno;

/*   *sta = uno;             // ATTENZIONE! DEBUG */
/*   *chk = uno; */
}





/**************************************************************************************
 * FS. jan 2007
 * Read open/closed status of Marotta valve
 **************************************************************************************/
onechar read_Marotta_valve_status( char* valve_nam, int16* sta, bool* chk )
{

#define n_marotta 21
  
  struct {
    char* nam;
    int16 msk;
  } valve[n_marotta] = {
    // box C
    {"V6a",        0x8000},
    {"V6b",        0x4000},
    {"V18a",       0x2000}, 
    {"V18b",       0x1000},
    {"V6a&V18a",   0xA000},
    {"V6b&V18b",   0x5000},
    // box S
    {"V1a",        0x8000},
    {"V2a",        0x4000},
    {"V3a",        0x2000},
    {"V4a",        0x1000},
    {"Sp1",        0x0800},
    {"V10a",       0x0400},
    {"V20a",       0x0200},
    {"V1b",        0x0100},
    {"V2b",        0x0080},
    {"V3b",        0x0040},
    {"V4b",        0x0020},
    {"V10b",       0x0010},
    {"V20b",       0x0008},
    {"Sp2",        0x0004},
    {"V20a&V20b",  0x0208}
  };

  int8 bus;

  int i;
  int valve_ind = 999;
  int32 wri = 0x84400000, rea, mask;
  int16 err;
  int la;
  char a[80];
  int16 c;
  onechar s;

  if ( P > 2 ) printf(" *** invoking read_Marotta_valve_status\n");


  for (i=0; i<n_marotta; i++) {
    if ( strcasecmp(valve[i].nam, valve_nam)==0 )  valve_ind = i;
  }

  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  //rea = 0x2820;                // ATTENZIONE !!! DEBUG  !!!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = 0xFFFC;
  rea &= mask;

  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea & mask = %08X = %s\n", rea, a);

  printf("\n Reading Marotta valves status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", rea);

  if ( P > 3 ) printf(" valve.msk = %08X\n", valve[valve_ind].msk);

  rea = rea & valve[valve_ind].msk;
  if ( P > 3 ) printf(" rea = %04X\n", rea);
   do {
    if ( P > 3 ) printf("%08X \n", rea);
    c = rea;
    rea >>= 1; 
  } while(rea);
  if ( P > 3 ) printf(" c = %08X\n", c);
  if ( c ) sprintf(s.x_, "%s","O");
  else     sprintf(s.x_, "%s","C");    
  return s; 

}



/**************************************************************************************
 * FS. jan 2007
 * Read open/closed status of Marotta valve
 **************************************************************************************/
onechar read_tensions_status( char* tension_nam, int16* sta, bool* chk )
{

#ifdef n_tensions
#undef n_tensions
#endif
#define n_tensions 8
  
  struct {
    char* nam;
    int16 msk;
    int16 cmd;
  } tension[n_tensions] = {
    // box C
  {"25V", 0x7000, 0x85A0},  // 1 abilita (!!)
  {"12V", 0x7000, 0x85A0},  // 1 disabilita
  {"MCA", 0x7000, 0x85A0},  // 1 disabilita
  {"CO2", 0x7000, 0x85A0},  // 1 disabilita
    // manifold
  {"12VPS", 0xC000, 0xC4C0},  // 1 disabilita
  {"12VFV", 0xC000, 0xC4C0},  // 1 disabilita
  {"21V",   0xC000, 0xC4C0},  // 1 abilita (!!)
  {"MUX",   0xC000, 0xC4C0},  // 1 abilita (!!)
  };

  int8 bus;

  int i;
  int tension_ind = 999;
  int32 wri, rea, mask;
  int16 err;
  int la;
  char a[80];
  int16 c;
  onechar s;

  if ( P > 2 ) printf(" *** invoking read_tensions_status\n");

  for (i=0; i<n_tensions; i++) {
    if ( strcasecmp(tension[i].nam, tension_nam)==0 )  tension_ind = i;
  }

  wri = tension[tension_ind].cmd << 16;
  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  //rea = 0x2820;                // ATTENZIONE !!! DEBUG  !!!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = tension[tension_ind].msk;
  rea &= mask;

  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea & mask = %08X = %s\n", rea, a);

  printf("\n Reading tensions status\n");
  if ( P > 0 ) printf("    [code in answer = 0x%04X]\n", rea);

  if ( P > 3 ) printf(" tension.msk = %08X\n", tension[tension_ind].msk);

  rea = rea & tension[tension_ind].msk;
  if ( P > 3 ) printf(" rea = %04X\n", rea);
   do {
    if ( P > 3 ) printf("%08X \n", rea);
    c = rea;
    rea >>= 1; 
  } while(rea);
  if ( P > 3 ) printf(" c = %08X\n", c);
  if ( c ) sprintf(s.x_, "%s","O");
  else     sprintf(s.x_, "%s","C");    
  return s; 

}



/**************************************************************************************
 * FS. nov 2006
 * read pump speed and active status
 **************************************************************************************/
twochar read_pump_status( char* pump_nam, int16* sta, bool* chk)
{
#define n_pumps 2

  struct {
    char* nam;
    int16 msk;
    int16 smsk;
    int16 bts;
    int32 strt;
    
  } pump[n_pumps] = {
    {"CP1", 0x8000, 0x2000},
    {"CP2", 0x4000, 0x1000}  
  };
 
  int8 bus;

  int i;
  int pump_ind = 999;
  int32 wri = 0x82200000, rea, a_rea, s_rea, mask;
  int16 d, e;
  int16 err;
  char a[80], b[80], c[80];
  int la, lb, lc;
  twochar pstat;

  if ( P > 2 ) printf(" *** invoking read_pump_status\n");
  
  for (i=0; i<n_pumps; i++) {
    if ( strcasecmp(pump[i].nam, pump_nam)==0 )  pump_ind = i;
  }

  bus = (clk<<4)|dat; 
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  //rea = 0xA5AA;                              // ATTENZIONE! Solo per DEBUG!
  la = sprintf_binary32(a,rea,-32);
  if ( P > 3 ) printf(" rea = %08X = %s\n", rea, a);

  mask = 0xC;
  s_rea = (rea & mask) << 10;
  mask = 0x3;
  a_rea = (rea & mask) << 14;

  lb = sprintf_binary32(b,a_rea,-32);
  lc = sprintf_binary32(c,s_rea,-32);

  printf("\n Reading pumps status\n");
  if ( P > 0 ) printf("    [code in answer = %04X, %04X]\n", a_rea, s_rea);

  if ( P > 3 ) printf(" s_rea = %s,\n a_rea = %s\n", c, b);

  a_rea &= pump[pump_ind].msk;
  s_rea &= pump[pump_ind].smsk;

  if ( P > 3 ) printf(" a_rea = %04X\n", a_rea);

  /* check if pump is active */
  do {
    if ( P > 3 ) printf("%08X \n", a_rea);
    d = a_rea;
    a_rea >>= 1; 
  } while(a_rea);
  
  if ( P > 3 ) printf(" d = %08X\n", d);
  
  if ( d ) {

  /* if active, check pump speed */
    do {
      if ( P > 3 ) printf("%08X \n", s_rea);
      e = s_rea;
      s_rea >>= 1; 
    } while(s_rea);
    if ( P > 3 ) printf(" e = %08X\n", e);
    if ( e )  sprintf(pstat.y_, "%s", "F");
    else      sprintf(pstat.y_, "%s", "H");

    sprintf(pstat.x_, "%s","A");
  }
  else 
    sprintf(pstat.x_, "%s","N");


  if ( P > 3 ) printf(" pstat: %s, %s\n", pstat.x_, pstat.y_);
  return pstat;

}



/**************************************************************************************
 * FS. oct 2004
 * set speed, enable, activate pump
 **************************************************************************************/
void turn_on_pump( char* pump_nam, char* speed, int16* sta, int8* chk )
{
#define n_pumps 2

  struct {
    char* nam;
    int16 msk;
    int16 smsk;
    int16 bts;
    int32 strt;
    
  } pump[n_pumps] = {
    {"CP1", 0x8000, 0x2000, 0x8428, 0x87288000},
    {"CP2", 0x4000, 0x1000, 0x8428, 0xCF288000}  
  };
 
  int8 bus;

  int16 ena_pumps_bits;
  int32 wri1, wri2, rea;
  int16 err;
  int p, bin_wri1, bin_wri2;
  char w1[80], w2[80];
  int uno = 1, zero = 0;

  char full = 'F'; 
  char half = 'H';
  int i, pump_ind = 999;
  
  if ( P > 2 ) printf(" *** invoking turn_on_pump\n");

  for (i=0; i<n_pumps; i++) {
    if ( strcasecmp(pump[i].nam, pump_nam)==0 )  pump_ind = i;
  }
  
  ena_pumps_bits = pump[pump_ind].bts;
  bus = (clk<<4)|dat;

  /* enable */
  wri1 = ena_pumps_bits << 16;
  if ( P > 3 ) 
    printf("\n ena_pumps_bits = %04X, now wri1 = %08X\n", ena_pumps_bits, wri1);
  wri1 += pump[pump_ind].msk;
  if ( P > 3 ) 
    printf("\n pump[pump_ind].msk = %08X, now wri1 = %08X\n", pump[pump_ind].msk, wri1);

  /* set speed -- change only if speed is set to full */
  if (!(*speed == full || *speed == half))
    printf("\n WARNING! Wrong speed definition\n");

  if (*speed == full ) wri1 += pump[pump_ind].smsk;
  if ( P > 3 ) 
    printf("\n pump[pump_ind].smsk = %08X, now wri1 = %08X\n", pump[pump_ind].smsk, wri1);

  p = parity32(wri1);
  wri1 += p << 30;
  if ( P > 3 ) 
    printf(" \n parity %d, now wri1 = %08X\n", p, wri1);
  bin_wri1 = sprintf_binary32(w1, wri1, -32);
  printf("\n Enabling circulation pump %s at %s speed\n", 
	 pump[pump_ind].nam, speed);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w1);

  exec_LeCroy_cmd(1, &bus, &wri1, &rea, chk, &err);
 
  if (err) *sta = zero;
  else     *sta = uno;
 
  /* start */
  wri2 = pump[pump_ind].strt;
  bin_wri2 = sprintf_binary32(w2, wri2, -32);
  printf("\n Starting circulation pump %s\n", pump[pump_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w2);

  exec_LeCroy_cmd(1, &bus, &wri2, &rea, chk, &err);

  if (!err || *sta == uno ) *sta = uno;

}



/**************************************************************************************
 * FS. jul 2006
 * turn off pump
 **************************************************************************************/
void turn_off_pump( char* pump_nam, int16* sta, int8* chk )
{
#define n_pumps 2

  struct {
    char* nam;
    int32 cmd;
  } pump[n_pumps] = {
    {"CP1", 0xC7280000},
    {"CP2", 0x8F280000}
  };

  int uno = 1, zero = 0;
  int16 err;
  int8 bus;

  int i, pump_ind = 999;
  int bin_wri;
  int32 wri, rea;
  char w[80];

  if ( P > 2 ) printf(" *** invoking turn_off_pump\n");

  for (i=0; i<n_pumps; i++) {
    if ( strcasecmp(pump[i].nam, pump_nam)==0 )  pump_ind = i;
  }

  wri = pump[pump_ind].cmd;
  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Stopping circulation pump %s\n", pump[pump_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;
 
}





/**************************************************************************************
 * FS. jul 2006
 * convert ADC for gp50 pressure sensors
 **************************************************************************************/
void convert_gp50( char* sensor_nam, int16* sta, int8* chk )
{
#define n_gp50 6

  struct {
    char* nam;
    int32 cmd;
    int32 cvt;
  } sensor[n_gp50] = {
    // box S
    {"P1a", 0x86C00000, 0x8FC88000},
    {"P1b", 0xC1C00000, 0x8FC84000},
    {"P2a", 0xCEC00000, 0x8FC82000},
    {"P2b", 0x89C00000, 0x8FC81000},
    // box C
    {"P3",  0xC8A00000, 0x8FA88000},
    {"P4",  0xC4A00000, 0x8FA84000}
  };

  int uno = 1, zero = 0;
  int16 err1;
  int8 bus;

  int i, sensor_ind = 999;
  int bin_wric;
  int32 wric, reac;
  char wc[80];

  if ( P > 2 ) printf(" *** invoking convert_gp50\n");

  for (i=0; i<n_gp50; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wric = sensor[sensor_ind].cvt;
  bin_wric = sprintf_binary32(wc, wric, -32);

  printf("\n Reading sensor %s\n", sensor[sensor_ind].nam);
  if ( P > 0 )
    printf("    converting adc [code = %s]\n", wc);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wric, &reac, chk, &err1);  // adc conversion

  if ( P > 0 ) 
    printf("    ERR =  0x%04X\n", err1);

   if (err1) *sta = zero;   else     *sta = uno;

}



/**************************************************************************************
 * FS. jul 2006
 * read gp50 pressure sensors - convert ADC first!
 **************************************************************************************/
void convert_read_gp50( char* sensor_nam, int16* sta, int8* chk, float* tens )
{
#define n_gp50 6

  struct {
    char* nam;
    int32 cmd;
    int32 cvt;
  } sensor[n_gp50] = {
    // box S
    {"P1a", 0x86C00000, 0x8FC88000},
    {"P1b", 0xC1C00000, 0x8FC84000},
    {"P2a", 0xCEC00000, 0x8FC82000},
    {"P2b", 0x89C00000, 0x8FC81000},
    // box C
    {"P3",  0xC8A00000, 0x8FA88000},
    {"P4",  0xC4A00000, 0x8FA84000}
  };

  int16 err1, err2, p_bit;
  int8 bus;

  int i, sensor_ind = 999;
  int bin_wri;
  int32 wri, rea;
  int16 mask;
  int bin_wric;
  int32 wric, reac;
  char w[80], wc[80];

  if ( P > 2 ) printf(" *** invoking convert_read_gp50\n");

  for (i=0; i<n_gp50; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wric = sensor[sensor_ind].cvt;
  bin_wric = sprintf_binary32(wc, wric, -32);

  wri = sensor[sensor_ind].cmd;
  bin_wri = sprintf_binary32(w, wri, -32);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wric, &reac, chk, &err1);  // adc conversion
  usleep(1000);                                             // wait 1 ms
  exec_LeCroy_cmd(1, &bus, &wric, &reac, chk, &err1);  // adc conversion
  usleep(1000);                                             // wait 1 ms
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err2);

  mask = 0xFFF0;
  rea &= mask;
  p_bit = (int16)rea;
  p_bit = invert_bit_order(p_bit);

  // p_bit = 0x0FFF;  // ATTENZIONE!! Solo per debug!

  *tens = (float)p_bit*5000./4096.;

  printf("\n Reading sensor %s = %8.2f V\n", sensor[sensor_ind].nam, *tens);
  if ( P > 0 ){
    printf("    converting adc [code = %s]\n", wc);
    printf("    reading        [code = %s]\n", w);
  }

  if ( P > 0 ) 
    printf("    [code in answer = 0x%04X = %8.2f V]\n", p_bit, *tens);
  //    printf("    ERR =  0x%04X\n", err2);

//   if (err1 || err2) *sta = zero;   else     *sta = uno;
  *sta = err2;

}



/**************************************************************************************
 * FS. jan 2007
 * read gp50 pressure sensors - NO conversion ADC first!
 **************************************************************************************/
void read_gp50( char* sensor_nam, int16* sta, int8* chk, float* tens )
{
#define n_gp50 6

  struct {
    char* nam;
    int32 cmd;
    int32 cvt;
  } sensor[n_gp50] = {
    // box S
    {"P1a", 0x86C00000, 0x8FC88000},
    {"P1b", 0xC1C00000, 0x8FC84000},
    {"P2a", 0xCEC00000, 0x8FC82000},
    {"P2b", 0x89C00000, 0x8FC81000},
    // box C
    {"P3",  0xC8A00000, 0x8FA88000},
    {"P4",  0xC4A00000, 0x8FA84000}
  };

  int uno = 1, zero = 0;
  int16 err2, p_bit;
  int8 bus;

  int i, sensor_ind = 999;
  int bin_wri;
  int32 wri, rea;
  int16 mask;
  char w[80];

  if ( P > 2 ) printf(" *** invoking read_gp50\n");

  for (i=0; i<n_gp50; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wri = sensor[sensor_ind].cmd;
  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Reading sensor %s\n", sensor[sensor_ind].nam);
  if ( P > 0 )
    printf("    reading        [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err2);

  mask = 0xFFF0;
  rea &= mask;
  p_bit = (int16)rea;
  p_bit = invert_bit_order(p_bit);

  // p_bit = 0x0FFF;  // ATTENZIONE!! Solo per debug!

  *tens = (float)p_bit*5000./4096.;

  if ( P > 0 ) 
    printf("    [code in answer = 0x%04X = %8.2f V]\n", p_bit, *tens);
    printf("    ERR =  0x%04X\n", err2);

   if (err2) *sta = zero;   else     *sta = uno;

}



/**************************************************************************************
 * FS. may 2007
 * read gp50 pressure sensors - NO conversion ADC first!
 **************************************************************************************/
void read_UGadc( char* sensor_nam, int16* sta, int8* chk, float* tens )
{
#define n_sensors 12

  struct {
    char* nam;
    int32 cmd;
    int32 cvt;
  } sensor[n_sensors] = {
    // gp50 box S
    {"P1a", 0x86C00000, 0x8FC88000},
    {"P1b", 0xC1C00000, 0x8FC84000},
    {"P2a", 0xCEC00000, 0x8FC82000},
    {"P2b", 0x89C00000, 0x8FC81000},
    // gp50 box C
    {"P3",  0xC8A00000, 0x8FA88000},
    {"P4",  0x8CA00000, 0x8FA84000},
    // kulite
    {"Pk1cS", 0x85C00000, 0x8FC80800},
    {"Pk2cS", 0xCDC00000, 0x8FC80400},
    {"Pk1dS", 0x83C00000, 0x8FC80200},
    {"Pk1cR", 0xCC400000, 0x8FC80100},
    {"Pk2cR", 0x82400000, 0x8FC80080},
    {"Pk1dR", 0xC6400000, 0x8FC80040}


  };

  int uno = 1, zero = 0;
  int16 err2, p_bit;
  int8 bus;

  int i, sensor_ind = 999;
  int bin_wri;
  int32 wri, rea;
  int16 mask;
  char w[80];

  if ( P > 2 ) printf(" *** invoking read_gp50\n");

  for (i=0; i<n_sensors; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wri = sensor[sensor_ind].cmd;
  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Reading sensor %s\n", sensor[sensor_ind].nam);
  if ( P > 0 )
    printf("    reading        [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err2);

  mask = 0xFFF0;
  rea &= mask;
  p_bit = (int16)rea;
  p_bit = invert_bit_order(p_bit);

  // p_bit = 0x0FFF;  // ATTENZIONE!! Solo per debug!

  *tens = (float)p_bit*5000./4096.;

  if ( P > 0 ) 
    printf("    [code in answer = 0x%04X = %8.2f V]\n", p_bit, *tens);
    printf("    ERR =  0x%04X\n", err2);

   if (err2) *sta = zero;   else     *sta = uno;

}



/**************************************************************************************
 * FS. jul 2006
 * convert ADC for gp50 pressure sensors
 **************************************************************************************/
void convert_UGadc( char* sensor_nam, int16* sta, int8* chk )
{
#define n_sensors 12

  struct {
    char* nam;
    int32 cmd;
    int32 cvt;
  } sensor[n_sensors] = {
    // gp50 box S
    {"P1a", 0x86C00000, 0x8FC88000},
    {"P1b", 0xC1C00000, 0x8FC84000},
    {"P2a", 0xCEC00000, 0x8FC82000},
    {"P2b", 0x89C00000, 0x8FC81000},
    // gp50 box C
    {"P3",  0xC8A00000, 0x8FA88000},
    {"P4",  0x8CA00000, 0x8FA84000},
     // kulite
    {"Pk1cS", 0x85C00000, 0x8FC80800},
    {"Pk2cS", 0xCDC00000, 0x8FC80400},
    {"Pk1dS", 0x83C00000, 0x8FC80200},
    {"Pk1cR", 0xCC400000, 0x8FC80100},
    {"Pk2cR", 0x82400000, 0x8FC80080},
    {"Pk1dR", 0xC6400000, 0x8FC80040}
 };

  int uno = 1, zero = 0;
  int16 err1;
  int8 bus;

  int i, sensor_ind = 999;
  int bin_wric;
  int32 wric, reac;
  char wc[80];

  if ( P > 2 ) printf(" *** invoking convert_gp50\n");

  for (i=0; i<n_sensors; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wric = sensor[sensor_ind].cvt;
  bin_wric = sprintf_binary32(wc, wric, -32);

  printf("\n Converting sensor %s\n", sensor[sensor_ind].nam);
  if ( P > 0 )
    printf("    converting adc [code = %s]\n", wc);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wric, &reac, chk, &err1);  // adc conversion

  if ( P > 0 ) 
    printf("    ERR =  0x%04X\n", err1);

   if (err1) *sta = zero;   else     *sta = uno;

}



/**************************************************************************************
 * FS. jan 2007
 * read kulite pressure sensors - NO conversion ADC first!
 **************************************************************************************/
void read_kulite( char* sensor_nam, int16* sta, int8* chk, float* tsig )
{
#define n_ksensors 3

  struct {
    char* nam;
    int32 sig;
    int32 ref;
    int32 csig;
    int32 cref;
  } sensor[n_ksensors] = {
    // box S
    {"Pk1c", 0x85C00000, 0xCC400000, 0x8FC80800, 0x8FC80100},
    {"Pk2c", 0xCDC00000, 0x82400000, 0x8FC80400, 0x8FC80080},
    {"Pk1d", 0x83C00000, 0xC6400000, 0x8FC80200, 0x8FC80040}
  };

  int uno = 1, zero = 0;
  int16 err2,  p_bits;
  int8 bus;
  int16 mask;

  int i, sensor_ind = 999;
  int bin_wris;
  int32 wris, reas;
  char ws[80];

  if ( P > 2 ) printf(" *** invoking read_kulite\n");

  for (i=0; i<n_ksensors; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wris = sensor[sensor_ind].sig;
  bin_wris = sprintf_binary32(ws, wris, -32);

  printf("\n Reading signal of sensor %s\n", sensor[sensor_ind].nam);
  if ( P > 0 )
    printf("    reading        [code = %s]\n", ws);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wris, &reas, chk, &err2);

  mask = 0xFFF0;

  reas &= mask;
  p_bits = (int16)reas;
  p_bits = invert_bit_order(p_bits);
  *tsig = (float)p_bits*5000./4096.;
  if ( P > 0 ) 
    printf("    [code in answer = 0x%04X = %8.2f V]\n", p_bits, *tsig);
  if ( P > 1 ) 
    printf("    ERR =  0x%04X\n", err2);

  if (err2) *sta = zero;
  else      *sta = uno;
 
}




/**************************************************************************************
 * FS. jul 2006
 * read kulite pressure sensors - convert ADC first!
 **************************************************************************************/
void convert_read_kulite( char* sensor_nam, int16* sta, int8* chk, float* tsig )
{
#define n_ksensors 3

  struct {
    char* nam;
    int32 sig;
    int32 ref;
    int32 csig;
    int32 cref;
  } sensor[n_ksensors] = {
    // box S
    {"Pk1c", 0x85C00000, 0xCC400000, 0x8FC80800, 0x8FC80100},
    {"Pk2c", 0xCDC00000, 0x82400000, 0x8FC80400, 0x8FC80080},
    {"Pk1d", 0x83C00000, 0xC6400000, 0x8FC80200, 0x8FC80040}
  };

  int uno = 1, zero = 0;
  int16 err1, err2, p_bits;
  int8 bus;
  int16 mask;

  int i, sensor_ind = 999;
  int bin_wris, bin_wrisc;
  int32 wris, reas, wrisc, reasc;
  char ws[80], wsc[80];

  if ( P > 2 ) printf(" *** invoking convert_read_kulite\n");

  for (i=0; i<n_ksensors; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wrisc = sensor[sensor_ind].csig;
  bin_wrisc = sprintf_binary32(wsc, wrisc, -32);
  wris = sensor[sensor_ind].sig;
  bin_wris = sprintf_binary32(ws, wris, -32);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wrisc, &reasc, chk, &err1);
  usleep(1000);
  exec_LeCroy_cmd(1, &bus, &wrisc, &reasc, chk, &err1);
  usleep(1000);
  exec_LeCroy_cmd(1, &bus, &wris, &reas, chk, &err2);

  mask = 0xFFF0;

  reas &= mask;
  p_bits = (int16)reas;
  p_bits = invert_bit_order(p_bits);
  *tsig = (float)p_bits*5000./4096.;
  printf("\n Reading signal of sensor %s = %8.2f V\n", sensor[sensor_ind].nam, *tsig);
  if ( P > 0 ) {
    printf("    converting adc [code = %s]\n", wsc);
    printf("    reading        [code = %s]\n", ws);
  }
  if ( P > 0 ) 
    printf("    [code in answer = 0x%04X = %8.2f V]\n", p_bits, *tsig);
  if ( P > 1 ) 
    printf("    ERR =  0x%04X\n", err2);

  if (err1 || err2) *sta = zero;
  else              *sta = uno;
 
}




/**************************************************************************************
 * FS. jul 2006
 * convert ADC of kulite pressure sensors
 **************************************************************************************/
void convert_kulite( char* sensor_nam, int16* sta, int8* chk )
{
#define n_ksensors 3

  struct {
    char* nam;
    int32 sig;
    int32 ref;
    int32 csig;
    int32 cref;
  } sensor[n_ksensors] = {
    // box S
    {"Pk1c", 0x85C00000, 0xCC400000, 0x8FC80800, 0x8FC80100},
    {"Pk2c", 0xCDC00000, 0x82400000, 0x8FC80400, 0x8FC80080},
    {"Pk1d", 0x83C00000, 0xC6400000, 0x8FC80200, 0x8FC80040}
  };

  int uno = 1, zero = 0;
  int16 err1;
  int8 bus;

  int i, sensor_ind = 999;
  int bin_wrisc, bin_wrirc;
  int32 wrisc, wrirc, reasc;
  char wsc[80], wrc[80];

  if ( P > 2 ) printf(" *** invoking convert_kulite\n");

  for (i=0; i<n_ksensors; i++) {
    if ( strcasecmp(sensor[i].nam, sensor_nam)==0 )  sensor_ind = i;
  }

  wrisc = sensor[sensor_ind].csig;
  wrirc = sensor[sensor_ind].cref;
  bin_wrisc = sprintf_binary32(wsc, wrisc, -32);
  bin_wrirc = sprintf_binary32(wrc, wrirc, -32);

  printf("\n Converting signal of sensor %s\n", sensor[sensor_ind].nam);
  if ( P > 0 )
    printf("     [code = %s]\n", wsc);
 
  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wrisc, &reasc, chk, &err1);

  if ( P > 1 ) 
    printf("    ERR =  0x%04X\n", err1);

  if (err1) *sta = zero;
  else      *sta = uno;
 
}




/**************************************************************************************
 * FS. oct 2004
 * Enable one heater, disable all the others of its box.
 **************************************************************************************/
void enable_heater( char* heater_nam, int16* sta, int8* chk )
{

#define n_heaters 5
  
  struct {
    char* nam;
    int16 msk;
    int16 bts;
  } heater[n_heaters] = {
    // box C
    // box S
    {"H1",       0x8000, 0x8BC8},
    {"H2",       0x4000, 0x8BC8},
    {"H3",       0x2000, 0x8BC8}, 
    {"H4",       0x1000, 0x8BC8},
    {"H5",       0x0800, 0x8BC8},
  };
  
  int8 bus;

  int16 ena_heaters_bits;
  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char w[80];
  int heater_ind = 999;
  int uno = 1, zero = 0;
  int i;

  if ( P > 2 ) printf(" *** invoking enable_heater\n");

  for (i=0; i<n_heaters; i++) {
    if ( strcasecmp(heater[i].nam, heater_nam)==0 )  heater_ind = i;
  }

  ena_heaters_bits = heater[heater_ind].bts;

  wri = ena_heaters_bits << 16;
  if ( P > 3 ) 
    printf("\n ena_heaters_bits = %04X, now wri = %08X\n", ena_heaters_bits, wri);
  wri += heater[heater_ind].msk;
  if ( P > 3 ) 
    printf("\n heater[heater_ind].msk = %08X, now wri = %08X\n", heater[heater_ind].msk, wri);
  p = parity32(wri);
  wri += p << 30;

  if ( P > 3 ) 
    printf(" \n parity %d, now wri = %08X\n", p, wri);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Enabling heater %s\n", heater[heater_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}




/**************************************************************************************
 * FS. oct 2004
 * Disable all heaters in the given box. Args: "box C", "box S".
 **************************************************************************************/
void disable_heaters( int16* sta, int8* chk )
{


  int8 bus;
 
  int32 wri = 0x8BC80000, rea;
  int16 err;
  int bin_wri;
  char w[80];
  int uno = 1, zero = 0;

  if ( P > 2 ) printf(" *** invoking disable_heaters\n");

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Disabling all heaters\n");
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}




/**************************************************************************************
 * FS. oct 2004
 * Start heater for a time "t"
 **************************************************************************************/
void start_heater( char* heater_nam, float t, int16* sta, int8* chk )
{
#define n_heaters 5
  
  struct {
    char* nam;
    int16 msk;
    int16 bts;
  } heater[n_heaters] = {
    // box C
    // box S
/*     {"H1",       0x140, 0x800F}, */
/*     {"H2",       0x940, 0x800F}, */
    {"H1",       0x000, 0x800F},  // NO Heater H1 on Xe line at this stage
    {"H2",       0x000, 0x800F},  // NO Heater H2 on CO2 line at this stage
    {"H3",       0x540, 0x800F}, 
    {"H4",       0xD40, 0x800F},
    {"H5",       0x340, 0x800F},
  };

  int8 bus;

  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char w[80];
  int uno = 1, zero = 0;
  int i, heater_ind = 999;
  int16 time_cod, inv_time_cod;

  if ( P > 2 ) printf(" *** invoking start_heater\n");

  time_cod = get_heat_time(t);
  if ( P > 3 )     printf(" time_cod %X\n", time_cod);
  inv_time_cod = invert_bit_order(time_cod);

  for (i=0; i<n_heaters; i++) {
    if ( strcasecmp(heater[i].nam, heater_nam)==0 )  heater_ind = i;
  }

  wri = ( heater[heater_ind].msk + heater[heater_ind].bts ) << 16;
  if ( P > 3 )     printf(" wri = %08X\n", wri);
  wri += inv_time_cod;
  if ( P > 3 )     printf(" with time code: wri = %08X\n", wri);
  p = parity32(wri);
  wri += p << 30;
  if ( P > 3 )     printf(" with parity p = %d,  wri = %08X\n", p, wri);

  bin_wri = sprintf_binary32(w, wri, -32);
  printf("\n Starting heater %s for %3.1f minute(s)\n", heater[heater_ind].nam, t);
  if ( P > 0 )
    printf("     [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;

/*   *sta = uno;             // ATTENZIONE! DEBUG */
/*   *chk = uno; */
}



/**************************************************************************************
 * FS. oct 2004
 * Disable all serial ports in the given box. Args: "box C", "box S".
 **************************************************************************************/
void disable_serials( int16* sta, int8* chk )
{

  int8 bus;
 
  int32 wri = 0xC9A80000, rea;
  int16 err;
  int bin_wri;
  char w[80];
  int uno = 1, zero = 0;

  if ( P > 2 ) printf(" *** invoking disable_serials\n");

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Disabling serial ports\n");
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}




/**************************************************************************************
 * FS. oct 2004
 * Enable one serial port, disable all the others of its box.
 **************************************************************************************/
void enable_serial( char* serial_nam, int16* sta, int8* chk )
{

#define n_serials 4
  
  struct {
    char* nam;
    int16 msk;
    int16 bts;
  } serial[n_serials] = {
    // box C
    // box S
    {"RS232-1",       0x8000, 0x89A8},
    {"RS232-2",       0x4000, 0x89A8},
    {"RS232-3",       0x2000, 0x89A8}, 
    {"RS232-4",       0x1000, 0x89A8},
  };
  
  int8 bus;

  int16 ena_serials_bits;
  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char w[80];
  int serial_ind = 999;
  int uno = 1, zero = 0;
  int i;

  if ( P > 2 ) printf(" *** invoking enable_serial\n");

  for (i=0; i<n_serials; i++) {
    if ( strcasecmp(serial[i].nam, serial_nam)==0 )  serial_ind = i;
}

  ena_serials_bits = serial[serial_ind].bts;

  wri = ena_serials_bits << 16;
  if ( P > 3 ) 
    printf("\n ena_serials_bits = %04X, now wri = %08X\n", ena_serials_bits, wri);
  wri += serial[serial_ind].msk;
  if ( P > 3 ) 
    printf("\n serial[serial_ind].msk = %08X, now wri = %08X\n", serial[serial_ind].msk, wri);
  p = parity32(wri);
  wri += p << 30;

  if ( P > 3 ) 
    printf(" \n parity %d, now wri = %08X\n", p, wri);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Enabling serial %s\n", serial[serial_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}




/**************************************************************************************
 * FS. oct 2004
 * Disable all MCA channels
 **************************************************************************************/
void disable_channels( int16* sta, int8* chk )
{


  int8 bus;
 
  int32 wri = 0xCC280000, rea;
  int16 err;
  int bin_wri;
  char w[80];
  int uno = 1, zero = 0;

  if ( P > 2 ) printf(" *** invoking disable_channels\n");

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Disabling all MCA channels\n");
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}




/**************************************************************************************
 * FS. oct 2004
 * Enable one channel port, disable all the others of its box.
 **************************************************************************************/
void enable_channel( char* channel_nam, int16* sta, int8* chk )
{

#define n_channels 4
  
  struct {
    char* nam;
    int16 msk;
    int16 bts;
  } channel[n_channels] = {
    {"Ch1",       0x8000, 0xCC28},
    {"Ch2",       0x4000, 0xCC28},
    {"Ch3",       0x2000, 0xCC28}, 
    {"Ch4",       0x1000, 0xCC28},
  };
  
  int8 bus;

  int16 ena_channels_bits;
  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char w[80];
  int channel_ind = 999;
  int uno = 1, zero = 0;
  int i;

  if ( P > 2 ) printf(" *** invoking enable_channel\n");

  for (i=0; i<n_channels; i++) {
    if ( strcasecmp(channel[i].nam, channel_nam)==0 )  channel_ind = i;
  }

  ena_channels_bits = channel[channel_ind].bts;

  wri = ena_channels_bits << 16;
  if ( P > 3 ) 
    printf("\n ena_channels_bits = %04X, now wri = %08X\n", ena_channels_bits, wri);
  wri += channel[channel_ind].msk;
  if ( P > 3 ) 
    printf("\n channel[channel_ind].msk = %08X, now wri = %08X\n", channel[channel_ind].msk, wri);
  p = parity32(wri);
  wri += p << 30;

  if ( P > 3 ) 
    printf(" \n parity %d, now wri = %08X\n", p, wri);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Enabling channel %s\n", channel[channel_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}



/**************************************************************************************
 * FS. feb 2008
 * Enable one tension, disable all the others in the box
 **************************************************************************************/
void enable_tension( char* tension_nam, int16* sta, int8* chk )
{

#ifdef n_tensions
#undef n_tensions
#endif
#define n_tensions 9
  
  struct {
    char* nam;
    int16 msk;
    int16 bts;
  } tension[n_tensions] = {
    // Box C
    {"25V",       0xF000, 0x85A8},
    {"12V",       0x3000, 0x85A8},
    {"MCA",       0x5000, 0x85A8}, 
    {"CO2",       0x6000, 0x85A8},
    // Manifolds
    {"12VPS",     0x4000, 0x84C8},
    {"12VFV",     0x8000, 0x84C8},
    {"21V",       0xE000, 0x84C8},
    {"MUX",       0xD000, 0x84C8},
    {"21VeMUX",   0xF000, 0x84C8}
  };
  
  int8 bus;

  int16 ena_tensions_bits;
  int32 wri, rea;
  int16 err;
  int p, bin_wri;
  char w[80];
  int tension_ind = 999;
  int uno = 1, zero = 0;
  int i;

  if ( P > 2 ) printf(" *** invoking enable_tension\n");

  for (i=0; i<n_tensions; i++) {
    if ( strcasecmp(tension[i].nam, tension_nam)==0 )  tension_ind = i;
  }

  ena_tensions_bits = tension[tension_ind].bts;

  wri = ena_tensions_bits << 16;
  if ( P > 3 ) 
    printf("\n ena_tensions_bits = %04X, now wri = %08X\n", ena_tensions_bits, wri);
  wri += tension[tension_ind].msk;
  if ( P > 3 ) 
    printf("\n tension[tension_ind].msk = %08X, now wri = %08X\n", tension[tension_ind].msk, wri);
  p = parity32(wri);
  wri += p << 30;

  if ( P > 3 ) 
    printf(" \n parity %d, now wri = %08X\n", p, wri);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Enabling tension %s\n", tension[tension_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}




/**************************************************************************************
 * FS. feb 2008
 * Disable all tensions in the given box. Args: "box C", "manifold".
 **************************************************************************************/
void disable_tensions( char* box_nam, int16* sta, int8* chk )
{

#define n_boxxx 2

  struct {
    char* nam;
    int32 wri;
  } box[n_boxxx] = {
    {"box C",     0x85A87000},
    {"Manifolds", 0x84C8C000}
  };

  int8 bus;
 
  int32 wri, rea;
  int16 err;
  int bin_wri;
  char w[80];
  int uno = 1, zero = 0;
  int i, box_ind = 999;

  if ( P > 2 ) printf(" *** invoking disable_tensions\n");

  for (i=0; i<n_boxxx; i++) {
    if ( strcasecmp(box[i].nam, box_nam)==0 ) box_ind = i;
  }

  wri = box[box_ind].wri;

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Disabling tensions in %s\n", box[box_ind].nam);
  if ( P > 0 ) 
    printf("    [code = %s]\n", w);

  if (err) *sta = zero;
  else     *sta = uno;

  //*sta = uno;             // ATTENZIONE! DEBUG
}





/**************************************************************************************
 * FS. feb 2008
 * open flipper valve in manifolds
 **************************************************************************************/
void open_flipper( char* flipper_nam, char* module_nam, int16* sta, int8* chk )
{
#define n_flippers 5

  struct {
    char* nam;
    int32 cmd;
  } flipper[n_flippers] = {
    {"FV1", 0x20},
    {"FV2", 0x10},
    {"FV3", 0x08},
    {"FV4", 0x04},
    {"FV5", 0x02}
  };

#define n_modules 4

  struct {
    char* nam;
    int32 cmd;
  } module[n_modules] = {
    {"MOD1", 0x8000000},
    {"MOD2", 0x4000000},
    {"MOD3", 0xC000000},
    {"MOD4", 0x2000000}
  };

  int uno = 1, zero = 0;
  int16 err;
  int8 bus;

  int i, j, flipper_ind = 999, module_ind = 999;
  int bin_wri, p;
  int32 wri = 0x804CF800, rea;
  char w[80];

  if ( P > 2 ) printf(" *** invoking open_flipper\n");

  for (i=0; i<n_flippers; i++) {
    if ( strcasecmp(flipper[i].nam, flipper_nam)==0 )  flipper_ind = i;
  }

  for (j=0; j<n_modules; j++) {
    if ( strcasecmp(module[j].nam, module_nam)==0 )  module_ind = j;
  }

  wri += module[module_ind].cmd;
  wri += flipper[flipper_ind].cmd;
  p = parity32(wri);
  wri += p << 30;
  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Opening flipper %s on module %s\n", flipper[flipper_ind].nam, module[module_ind].nam);
  if ( P > 0 )
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;
 
}




/**************************************************************************************
 * FS. jul 2006
 * close flipper valve
 **************************************************************************************/
 void close_flipper( char* flipper_nam, char* module_nam, int16* sta, int8* chk )
{
#define n_flippers 5

  struct {
    char* nam;
    int32 cmd;
  } flipper[n_flippers] = {
    {"FV1", 0x20},
    {"FV2", 0x10},
    {"FV3", 0x08},
    {"FV4", 0x04},
    {"FV5", 0x02}
  };

#define n_modules 4

  struct {
    char* nam;
    int32 cmd;
  } module[n_modules] = {
    {"MOD1", 0x8000000},
    {"MOD2", 0x4000000},
    {"MOD3", 0xC000000},
    {"MOD4", 0x2000000}
  };

  int uno = 1, zero = 0;
  int16 err;
  int8 bus;

  int i, j, flipper_ind = 999, module_ind = 999;
  int bin_wri, p;
  int32 wri = 0x804AF800, rea;
  char w[80];

  if ( P > 2 ) printf(" *** invoking open_flipper\n");

  for (i=0; i<n_flippers; i++) {
    if ( strcasecmp(flipper[i].nam, flipper_nam)==0 )  flipper_ind = i;
  }

  for (j=0; j<n_modules; j++) {
    if ( strcasecmp(module[j].nam, module_nam)==0 )  module_ind = j;
  }

  wri += module[module_ind].cmd;
  wri += flipper[flipper_ind].cmd;
  p = parity32(wri);
  wri += p << 30;
  bin_wri = sprintf_binary32(w, wri, -32);

  printf("\n Closing flipper %s on module %s\n", flipper[flipper_ind].nam, module[module_ind].nam);
  if ( P > 0 )
    printf("    [code = %s]\n", w);

  bus = (clk<<4)|dat;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err);

  if (err) *sta = zero;
  else     *sta = uno;
 

}


/**************************************************************************************
 * FS. feb 2008
 * read pressure sensors on manifolds
 **************************************************************************************/


void read_honeywell(char* mux_nam, int16* sta, int8* chk, int16* sig)
{

#define n_muxs 4

struct {
  char *nam;
  int16 msk;
} mux[n_muxs] = {
  {"MUX1", 0x0},
  {"MUX2", 0x4},
  {"MUX3", 0x2},
  {"MUX4", 0x6}
};

  int16 err1, mask;
  int8 bus;
  int32 wri = 0x80C80000, rea, p;
  int i, mux_ind = 999;
  char w[80];
  int bin_wri;

  if ( P > 2 ) printf(" *** invoking read_honeywell\n");


// Select MUX Channel on UGFV to get analog signal to UGSCM ADCs:
   for (i=0; i<n_muxs; i++) {   
     if ( strcasecmp(mux[i].nam, mux_nam)==0 )  mux_ind = i;   
   }   

   mask  = mux[mux_ind].msk << 16;   
   mask += mux[mux_ind].msk << 13;   
   mask += mux[mux_ind].msk << 10;   
   mask += mux[mux_ind].msk <<  7;   

   wri += mask;   
   p = parity32(wri);   
   wri += p << 30;   
   bin_wri = sprintf_binary32(w, wri, -32);   

   if ( P > 0 ){   
     printf("\n Reading sensors on mux %s \n", mux[mux_ind].nam);   
     printf("    [code = %s]\n", w);   
   }
   bus = (clk<<4)|dat;    
   exec_LeCroy_cmd(1, &bus, &wri, &rea, chk, &err1);   

  *sta = 0;
  if(err1!=0) *sta  += 1;

/*
   sleep(1);   //  Wait for UGSCM to convert ???????

// Read ADCs from UGSCM:
  read_ADC(adc, &err2);
  if(err2!=0) *sta  += 2;

  if( P>0) {

   printf(" read_ADC   ERR =  0x%04X\n", err2);

   for (i=0; i<n; i++) {
    if (!(i % 8)) printf("\n");
    printf("[%2d]: %5d  ", i, adc[i]);
   }
   printf("\n\n");
  }

  for (j=0; j<256; j++)  sig[j] = adc[j];
*/



}

/**********************/





/* COMPLEX OPERATIONS */
/**********************/

/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void empty_mix_volume(int16* sta, int8* chk, float* tempo1 )
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;
  float t1, t2;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 60 s, t2 = 75 s; */
  t1 = *tempo1;
  t2 = t1*1.25;

  enable_Marotta_valve( "V4a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V18a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V4a", t1, &loc_sta, &loc_chk );
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;
  open_Marotta_valve( "V18a", t1, &loc_sta, &loc_chk );
  //sleep(t2);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box C", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

}

/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void empty_mix_volume_TEST(int16* sta, int8* chk, float* tempo1 )
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;
  float t1, t2;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 60 s, t2 = 75 s; */
  t1 = *tempo1;
  t2 = t1;

  enable_Marotta_valve( "V5", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V5", t1, &loc_sta, &loc_chk );
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;  
  sleep(t2);

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

}



/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void set_ground_state_Xe(int16* sta, int8* chk, float* tempo1)
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;
  float t1, t2, t3, t4, t5, t6;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 1 s, t3 = 1 s, t5 = 60 s */
  t1 = *tempo1;
  t2 = t1*5.;
  t3 = t1;
  t4 = t2;
  t5 = t1*60.;
  t6 = t1*1.25;  

  enable_Marotta_valve( "V2a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V2a", t1, &loc_sta, &loc_chk );
  //sleep(t2);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V3a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V3a", t3, &loc_sta, &loc_chk );
  //sleep(t4);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V4a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V18a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V4a", t5, &loc_sta, &loc_chk );
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;
  open_Marotta_valve( "V18a", t5, &loc_sta, &loc_chk );
  //sleep(t6);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box C", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

}


/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void set_ground_state_CO2(int16* sta, int8* chk, float* tempo1)
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;
  float t1, t2, t3, t4, t5, t6;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 1 s, t3 = 1 s, t5 = 60 s */
  t1 = *tempo1;
  t2 = t1*5.;
  t3 = t1;
  t4 = t2;
  t5 = t1*60.;
  t6 = t1*1.25;  


  enable_Marotta_valve( "V2b", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V2b", t1, &loc_sta, &loc_chk );
  //sleep(t2);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V3b", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V3b", t3, &loc_sta, &loc_chk );
  //sleep(t4);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V4a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V18a", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V4a", t5, &loc_sta, &loc_chk );
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;
  open_Marotta_valve( "V18a", t5, &loc_sta, &loc_chk );
  //sleep(t6);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box C", &loc_sta, &loc_chk );
  //sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

}



/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void xenon_fill_step(int16* sta, int8* chk, float* tempo1)
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;

  float t1, t2, t3, t4, t5, t6;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 1 s, t3 = 1 s, t5 = 40 s */
  t1 = *tempo1;
  t2 = t1*5.;
  t3 = 2*t1;
  t4 = t2;
  t5 = t1*30.;
  t6 = t5*1.25;  

  enable_Marotta_valve( "V1a", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V1a", t1, &loc_sta, &loc_chk );
  sleep(t2);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V2a", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V2a", t3, &loc_sta, &loc_chk );
  sleep(t4);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V3a", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V3a", t5, &loc_sta, &loc_chk );
  sleep(t6);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

}




/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void co2_fill_step(int16* sta, int8* chk, float* tempo1)
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;

  float t1, t2, t3, t4, t5, t6;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 2 s, t3 = 4 s, t5 = 60 s */
  t1 = *tempo1;
  t2 = t1*5.;
  t3 = 2*t1;
  t4 = t2;
  t5 = 30.*t1;
  t6 = t5*1.25;  

  enable_Marotta_valve( "V1b", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V1b", t1, &loc_sta, &loc_chk );
  sleep(t2);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V2b", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V2b", t3, &loc_sta, &loc_chk );
  sleep(t4);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V3b", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V3b", t5, &loc_sta, &loc_chk );
  sleep(t6);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  //printf(" ***  loc_sta %x, loc_chk %x\n", loc_sta, loc_chk);
  //printf(" *** *** sta %x, chk %x\n", *sta, *chk);

}




/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void xenon_fill_step3t(int16* sta, int8* chk, float* tempo1, float* tempo2, float* tempo3 )
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;

  float t1, t2, t3, t4, t5, t6;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 1 s, t3 = 1 s, t5 = 40 s */
  t1 = *tempo1;
  t2 = t1*2.;
  t3 = *tempo2;
  t4 = t3*2.;
  t5 = *tempo3;
  t6 = t5;  

  enable_Marotta_valve( "V1a", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V1a", t1, &loc_sta, &loc_chk );
  sleep(t2);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V2a", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V2a", t3, &loc_sta, &loc_chk );
  sleep(t4);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V3a", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V3a", t5, &loc_sta, &loc_chk );  
  sleep(t6);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

}




/**************************************************************************************
 * FS. oct 2004
 **************************************************************************************/
void co2_fill_step3t(int16* sta, int8* chk, float* tempo1, float* tempo2, float* tempo3)
{

  int16 loc_sta = 1;
  int8  loc_chk = 0;
  int uno = 1, zero = 0;

  float t1, t2, t3, t4, t5, t6;

  *sta = uno;
  *chk = zero;

  /* Default: t1 = 2 s, t3 = 4 s, t5 = 60 s */
  t1 = *tempo1;
  t2 = t1*2.;
  t3 = *tempo2;
  t4 = t3*2.;
  t5 = *tempo3;
  t6 = t5;

  enable_Marotta_valve( "V1b", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V1b", t1, &loc_sta, &loc_chk );
  sleep(t2);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V2b", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V2b", t3, &loc_sta, &loc_chk );
  sleep(t4);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  enable_Marotta_valve( "V3b", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  open_Marotta_valve( "V3b", t5, &loc_sta, &loc_chk );  
  sleep(t6);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  disable_Marotta_valves( "box S", &loc_sta, &loc_chk );
  sleep(1);
  if (loc_sta == 0 ) *sta = loc_sta;
  if (loc_chk != 0 ) *chk = loc_chk;

  //printf(" ***  loc_sta %x, loc_chk %x\n", loc_sta, loc_chk);
  //printf(" *** *** sta %x, chk %x\n", *sta, *chk);

}



