//
// this is file ehts.c 
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for - External Heater, Temp Summary page display
//

#include "pgsc.h"               // it has some global data definitions

#define NLINES 19
char *flines[NLINES] = {
  /*         1         2         3         4         5         6         7         8 */
  /*12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
  "+X  PS00     PS06  +Y  PS12     PS18   -X   PS24     PS30  -Y  PS36     PS42  +X", /* 1*/
  "   D- +99   D- +99  | D- +99   D- +99   |  D- +99   D- +99  | D- +99   D- +99   ", /* 2*/
  "    CR01            |           CR31    |   CR41            |           CR71    ", /* 3*/
  "   D- +99           |          D- +99   |  D- +99           |          D- +99   ", /* 4*/
  " M00    M03 M04 M05 |M06 M07 M08    M11 |M12    M15 M16 M17 |M18 M19 M20    M23 ", /* 5*/
  " D-     D-  D-  D-  |D-  D-  D-     D-  |D-     D-  D-  D-  |D-  D-  D-     D-  ", /* 6*/
  " +99    +99 +99 +99 |+99 +99 +99    +99 |+99    +99 +99 +99 |+99 +99 +99    +99 ", /* 7*/
  "    CR02   PSB0     |    PSB3   CR32    |   PDD    PSB4     |    PSB7   CR72    ", /* 8*/
  "   D- +99  D +99  MCCYP  D +99 D- +99   |  D- +99  D +99  MCCYN  D +99 D- +99   ", /* 9*/
  "   D- +99  - +99   +99   - +99 D- +99   |  D- +99  - +99   +99   - +99 D- +99   ", /*10*/
  " M00    M03 M04 M05 |M06 M07 M08    M11 |M12    M15 M16 M17 |M18 M19 M20    M23 ", /* 1*/
  " D-     D-  D-  D-  |D-  D-  D-     D-  |D-     D-  D-  D-  |D-  D-  D-     D-  ", /* 2*/
  " +99    +99 +99 +99 |+99 +99 +99    +99 |+99    +99 +99 +99 |+99 +99 +99    +99 ", /* 3*/
  "    CR03            |           CR33    |   CR43            |           CR73    ", /* 4*/
  "   D- +99           |          D- +99   |  D- +99           |          D- +99   ", /* 5*/
  "+X  SK00     SK06  +Y  SK12     SK18   -X   SK24     SK30  -Y  SK36     SK42  +X", /* 6*/
  "   D- +99   D- +99  | D- +99   D- +99   |  D- +99   D- +99  | D- +99   D- +99   ", /* 7*/
  "+X AC00   AC01   AC02   AC03   AC04   AC05   AC06   AC07   AC08   AC09   AC10 -X", /* 8*/
  "  D- +99 D- +99 D- +99 D- +99 D- +99 D- +99 D- +99 D- +99 D- +99 D- +99 D- +99  "};/* 9*/
/*Color Key: M03 fixed, ___ old data, +10 normal, +37 warn, +45 alarm, +10 old/bad" */
/*Heaters: E/D = En/Disabled, +/- = On/Off.  Temperatures ^C                      " */ 
struct rccoord htcs_off[2] = {{0,40},{0,40}}; /*HTC 51,63 */
struct rccoord htcs[3][16][2]={  /* heater state, temp */
  /*                        HTC11 */ 
  {{{ 6,37}, { 1, 0}},   /* MG11  */
   {{ 4,32}, { 0, 3}},   /* CR31  */
   {{ 9,32}, { 0, 3}},   /* CR32  */
   {{ 6,30}, { 1, 0}},   /* MG08  */
   {{ 6,26}, { 1, 0}},   /* MG07  */
   {{ 6,22}, { 1, 0}},   /* MG06  */
   {{ 2,23}, { 0, 3}},   /* PS12  */
   {{ 2,32}, { 0, 3}},   /* PS18  */
   {{ 6, 2}, { 1, 0}},   /* MG00  */
   {{ 4, 4}, { 0, 3}},   /* CR01  */
   {{ 9, 4}, { 0, 3}},   /* CR02  */
   {{ 6, 9}, { 1, 0}},   /* MG03  */
   {{ 6,13}, { 1, 0}},   /* MG04  */
   {{ 6,17}, { 1, 0}},   /* MG05  */
   {{ 2, 4}, { 0, 3}},   /* PS00  */
   {{ 2,13}, { 0, 3}}},   /* PS06  */
  /*                       HTC23  */
  {{{12, 2}, { 1, 0}},   /* MG00  */
   {{15, 4}, { 0, 3}},   /* CR03  */
   {{10, 4}, { 0, 3}},   /* CR02  */
   {{12, 9}, { 1, 0}},   /* MG03  */
   {{12,13}, { 1, 0}},   /* MG04  */
   {{12,17}, { 1, 0}},   /* MG05  */
   {{17, 4}, { 0, 3}},   /* SK00  */
   {{17,13}, { 0, 3}},   /* SK06  */
   {{12,37}, { 1, 0}},   /* MG11  */
   {{15,32}, { 0, 3}},   /* CR33  */
   {{10,32}, { 0, 3}},   /* CR32  */
   {{12,30}, { 1, 0}},   /* MG08  */
   {{12,26}, { 1, 0}},   /* MG07  */
   {{12,22}, { 1, 0}},   /* MG06  */
   {{17,23}, { 0, 3}},   /* SK12  */
   {{17,32}, { 0, 3}}},  /* SK18  */
  /*                        HTC83 */ 
  {{{19, 3}, { 0, 3}},   /* AC00  */
   {{19,10}, { 0, 3}},   /* AC01  */
   {{19,17}, { 0, 3}},   /* AC02  */
   {{19,24}, { 0, 3}},   /* AC03  */
   {{19,31}, { 0, 3}},   /* AC04  */
   {{19,38}, { 0, 3}},   /* AC05  */
   {0},{0},
   {{19,45}, { 0, 3}},   /* AC06  */
   {{19,52}, { 0, 3}},   /* AC07  */
   {{19,59}, { 0, 3}},   /* AC08  */
   {{19,66}, { 0, 3}},   /* AC09  */
   {{19,73}, { 0, 3}}}   /* AC10  */
};

struct rccoord psbs[4][3]={ /* heater state, temp, temp */
   {{ 9,12}, { 0, 2}, {1, 2}},
   {{ 9,26}, { 0, 2}, {1, 2}},
   {{ 9,52}, { 0, 2}, {1, 2}},
   {{ 9,66}, { 0, 2}, {1, 2}}
};

struct rccoord mses[2][2]={ // temp, stupid, temp
  {{10,20},{0,0}},
  {{10,60},{0,0}}
};

//------------------------------------------------------------------------------

void initialize_EHTS_page( void) {
  static short first = TRUE;
  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void simulate_EHTS_data( void) {
}

//------------------------------------------------------------------------------

void process_EHTS_data( void) {

  static short first = TRUE;

  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void display_EHTS_right_panel( short row, short col) {
  int i;
  
  _settextposition( row, col);
  set_colors( headline); 
  sprintf( b, "Heaters: E/D = Ctl En/Disabled, +/- = ON/OFF, XX = PDD OFF. Temperatures in %sC                   ", GRAD);  
  _outtext(b);

  _settextposition( row+1, col);
  set_colors( headline); 
  _outtext("Color Key:");
  
  set_colors( frame); 
  _outtext(" M03");  
  set_colors( headline); 
  _outtext(" fixed, ");

  set_colors( old_data); 
  _outtext("   ");  
  set_colors( headline); 
  _outtext(" no data, ");  
  
  set_colors( normal); 
  _outtext("+10");  
  set_colors( headline); 
  _outtext(" normal, ");    

  set_colors( warning); 
  _outtext("+37");  
  set_colors( headline); 
  _outtext(" warn, ");

  set_colors( alarm); 
  _outtext("+45");  
  set_colors( headline); 
  _outtext(" alarm, ");  

  set_colors( old_data); 
  _outtext("+10");  
  set_colors( headline); 
  _outtext(" old/bad ");  

}

//------------------------------------------------------------------------------
    
void display_EHTS_data_frame( void) {
  short row, col;
  int i,j;

  row = 2;
  col = 1;
  
  set_colors( frame);
  
  for(i=0; i<NLINES; i++){
    _settextposition( row+i,col);                                 
    _outtext( flines[i]);
  }

}

//------------------------------------------------------------------------------

void display_EHTS_data( void) {

  int i, ii, j, k;
  long now;
  short row, col, mse_status;
  int cc_joke[16]={0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1};

  if (page_frozen) return;

  now = time( NULL);

  /* HTCs  -- dig i/o (heater state) */

  for (i=0; i<5; i++) {
    data_are_absent = (HTC[i].digital_IO_time && PDD_stat_time) ? FALSE : TRUE;
    data_are_old    = ((now - HTC[i].digital_IO_time > HTC_digital_IO_time_staleness) ||
                       (now - PDD_stat_time > PDD_stat_time_staleness)) ? TRUE : FALSE;
    for (j=0; j<16; j++) {
      if((i==4)&&(cc_joke[j])) continue;
      if (!HTC[i].chain[j].PDD_ON) {
        sprintf( b, "XX ");
      }
      else {
	b[0] = (HTC[i].chain[j].ENABLED) ? 'E' : 'D';
	b[1] = (HTC[i].chain[j].ON     ) ? '+' : '-';
	b[2] = ' ';
	b[3] = '\0';
      }
      ii = i;
      if(i>1){ii = ii - 2;}
      row = htcs[ii][j][0].row+1;
      col = htcs[ii][j][0].col;
      if((i==2)||(i==3)){
	row += htcs_off[ii].row;
	col += htcs_off[ii].col;
      }
      _settextposition( row, col);
      set_value_colors( HTC[i].chain[j].status);
      _outtext( b);
    }
  }

  /* HTCs -- DTS temps */

  for (i=0; i<5; i++) {
    data_are_absent = (HTC[i].temperature_time) ? FALSE : TRUE;
    data_are_old    = (now - HTC[i].temperature_time > HTC_temperature_time_staleness) ? TRUE : FALSE;
    for (j=0; j<16 ; j++) {
      if((i==4)&&(cc_joke[j])) continue;
      if (abs(HTC[i].chain[j].heater_temperature) > 99) {
	sprintf( b, "XXX");
      }
      else {
	sprintf( b, "%+3d", HTC[i].chain[j].heater_temperature);
      }
      ii = i;
      if(i>1){ii = ii - 2;}
      row = htcs[ii][j][0].row + htcs[ii][j][1].row+1;
      col = htcs[ii][j][0].col + htcs[ii][j][1].col;
      if((i==2)||(i==3)){
	row += htcs_off[ii].row;
	col += htcs_off[ii].col;
      }
      _settextposition( row, col);
      set_value_colors( HTC[i].chain[j].temperature_status);
      _outtext( b);
    }
  }

  /* PSB data -- heaters */

  for (i=0; i<4; i++) {
    data_are_absent = (PSB[i].digital_IO_time) ? FALSE : TRUE;
    data_are_old    = (now - PSB[i].digital_IO_time > PSB_digital_IO_time_staleness) ? TRUE : FALSE;
    set_value_colors( PSB[i].heater.status);
    b[0] = (PSB[i].heater.ENABLED) ? 'E' : 'D';
    b[1] = ' ';
    b[2] = '\0';
    _settextposition( psbs[i][0].row+1, psbs[i][0].col);
    _outtext( b);
    b[0] = (PSB[i].heater.ON) ? '+' : '-';
    b[1] = ' ';
    b[2] = '\0';
    _settextposition( psbs[i][0].row+2, psbs[i][0].col);
    _outtext( b);
  }


  /* PSB data -- temps */

  for (i=0; i<4; i++) {
    data_are_absent = (PSB[i].temperature_time) ? FALSE : TRUE;
    data_are_old    = (now - PSB[i].temperature_time > PSB_temperature_time_staleness) ? TRUE : FALSE;
    for ( j=0; j<2; j++) {
      if (abs(PSB[i].temperature[j].value) > 99) {
        sprintf( b, "XXX");
      }
      else {
        sprintf( b, "%+3d", PSB[i].temperature[j].value);
      }
      row = psbs[i][0].row + psbs[i][j+1].row+1;
      col = psbs[i][0].col + psbs[i][j+1].col;
      _settextposition( row, col);
      set_value_colors( PSB[i].temperature[j].status);
      _outtext( b);
    }
  }


  /* MCC temp data from MSEA */

  mse_status = ALARM;
  for(i=0; i<2; i++){
    int nb_of_T_sensors[3] = { 28, 31, 16};
    data_are_absent = (T_sensor_time[i]) ? FALSE : TRUE;
    data_are_old    = (now - T_sensor_time[i] > T_sensor_time_staleness) ? TRUE : FALSE;
    if (abs(T_sensor[i][nb_of_T_sensors[i]-1]) > 99) {   // MCCs are last sensors each MSEa
      sprintf( b, "XXX");
    }
    else {
      sprintf( b, "%+3d", T_sensor[i][nb_of_T_sensors[i]-1]);
    }
    row = mses[i][0].row + mses[i][1].row+1;
    col = mses[i][0].col + mses[i][1].col;
    _settextposition( row, col);
    set_value_colors( T_sensor_status[i][nb_of_T_sensors[i]-1]);
    _outtext( b);
  }      
}


//------------------------------------------------------------------------------

void display_EHTS_page( void) {
  short row, col;
  display_headline( "EXTERNAL HEATERS & TEMPS ");
  if (new_page) {
    display_EHTS_data_frame();
    display_EHTS_right_panel( row=21, col=1);
  }
  display_EHTS_data();
}
 
//------------------------------------------------------------------------------
