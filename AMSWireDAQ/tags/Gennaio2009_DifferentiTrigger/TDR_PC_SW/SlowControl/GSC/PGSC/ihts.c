//
// this is file ihts.c 
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for - Internal Temp Summary page display
//

#include "pgsc.h"               // it has some global data definitions

#define NLINES 21
char *ihts_lines[NLINES] = {
/*         1         2         3         4         5         6         7         8 */
/*12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
 "        |                |                 |                |                  |", /* 2*/
 "       +X(aft)          +Y(star)          -X(fore)         -Y(port)           +X", /* 3*/
 " TR.1. 09P 08P 07P  BT11 |        07N 08N 09N 10N      BT12 |            10P    ", /* 4*/
 "       +99 +99 +99   +99 |        +99 +99 +99 +99       +99 |            +99    ", /* 5*/
 "    TS01+99          TS11+99           TS21+99          TS31+99                 ", /* 6*/
 " TR.2.  |07P 06P 05P     |      05N 06N 07N|08N 09N 10N     |      10P 09P 08P  ", /* 7*/
 "        |+99 +99 +99     |      +99 +99 +99|+99 +99 +99     |      +99 +99 +99  ", /* 8*/
 "    TS02+99          TS12+99           TS22+99          TS32+99                 ", /* 9*/
 " TR.3.  |07P 06P         |          06N 07N|08N 09N         |      10P 09P 08P  ", /*10*/
 "        |+99 +99         |          +99 +99|+99 +99         |      +99 +99 +99  ", /* 1*/
 " TR.4.  |07P 06P 05P     |          06N 07N|08N 09N         |      10P 09P 08P  ", /* 2*/
 "        |+99 +99 +99     |          +99 +99|+99 +99         |      +99 +99 +99  ", /* 3*/
 "    TS03+99          TS13+99           TS23+99          TS33+99                 ", /* 4*/
 " TR.5.  |07P 06P 05P     |      05N 06N 07N|08N 09N 10N     |      10P 09P 08P  ", /* 5*/
 "        |+99             |      +99 +99 +99|+99 +99 +99     |      +99 +99 +99  ", /* 6*/
 "    TS04+99          TS14+99           TS24+99          TS34+99                 ", /* 7*/
 " TR.6. 09P 08P           |  BT61      08N 09N 10N           |  BT62      10P    ", /* 8*/
 "       +99 +99           |   +99      +99 +99 +99           |   +99      +99    ", /* 9*/
 "       +X(aft)          +Y(star)          -X(fore)         -Y(port)           +X", /*20*/
 "        |                |                 |                |                  |", /*20*/
 "                                                                                "};/*20*/
/*Color Key: M03 fixed, ___ old data, +10 normal, +37 warn, +45 alarm, +10 old/bad" */
/*Heaters: E/D = En/Disabled, +/- = On/Off.  Temperatures ^C                      " */ 

struct rccoord msea_tr[2][32]={ /* temp */
  { {  5, 35}, {  5, 39}, {  5, 43}, {  5, 47},
    {  8, 33}, {  8, 37}, {  8, 41}, {  8, 45}, {  8, 49}, {  8, 53},
               { 11, 37}, { 11, 41}, { 11, 45}, { 11, 49},
               { 13, 37}, { 13, 41}, { 13, 45}, { 13, 49},
    { 16, 33}, { 16, 37}, { 16, 41}, { 16, 45}, { 16, 49}, { 16, 53},
               { 19, 39}, { 19, 43}, { 19, 47} },
  { {  5, 16}, {  5, 12}, {  5,  8}, {  5, 74}, 
    {  8, 18}, {  8, 14}, {  8, 10}, {  8, 76}, {  8, 72}, {  8, 68},
               { 11, 14}, { 11, 10}, { 11, 76}, { 11, 72}, { 11, 68},
    { 13, 18}, { 13, 14}, { 13, 10}, { 13, 76}, { 13, 72}, { 13, 68},
    { 16, 18}, { 16, 14}, { 16, 10}, { 16, 76}, { 16, 72}, { 16, 68},
               { 19, 12}, { 19,  8}, { 19, 74} } };

struct rccoord mseb_ts[4][4]={
  { {  6,  9}, {  6, 26}, {  6, 44}, {  6, 61} }, 
  { {  9,  9}, {  9, 26}, {  9, 44}, {  9, 61} }, 
  { { 14,  9}, { 14, 26}, { 14, 44}, { 14, 61} }, 
  { { 17,  9}, { 17, 26}, { 17, 44}, { 17, 61} } };

struct rccoord msea_bt[2][2]={
  { { 19, 30}, {  5, 22} },
  { { 19, 65}, {  5, 57} } };

//------------------------------------------------------------------------------

void initialize_IHTS_page( void) {
  static short first = TRUE;
  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void simulate_IHTS_data( void) {
}

//------------------------------------------------------------------------------

void process_IHTS_data( void) {

  static short first = TRUE;

  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void display_IHTS_right_panel( short row, short col) {
  int i;
  

  _settextposition( row, col);
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
    
void display_IHTS_data_frame( void) {
  short row, col;
  int i,j;

  row = 2;
  col = 1;
  
  set_colors( frame);
  
  for(i=0; i<NLINES; i++){
    _settextposition( row+i,col);                                 
    _outtext( ihts_lines[i]);
  }

}

//------------------------------------------------------------------------------

void display_IHTS_data( void) {

  int i, j;
  long now;

  if (page_frozen) return;

  now = time( NULL);

  /* TFEK DTS from MSEA */
  
  for( i=0; i<2; i++){
    int nb_of_T_sensors[3] = { 28, 31, 16};
    data_are_absent = (T_sensor_time[i])                                 ? FALSE : TRUE;
    data_are_old    = (now - T_sensor_time[i] > T_sensor_time_staleness) ? TRUE : FALSE;
    for( j=0; j< nb_of_T_sensors[i] - 1; j++){
      if (abs(T_sensor[i][j]) > 99) {   // MCCs are last sensors each MSEa
	sprintf( b, "XXX");
      }
      else {
	sprintf( b, "%+3d", T_sensor[i][j]);
      }
      _settextposition( msea_tr[i][j].row, msea_tr[i][j].col);
      set_value_colors( T_sensor_status[i][j]);
      _outtext( b);
    }
  }

  /* PT100 temp sensors inside magnet from MSEB */

  data_are_absent = (T_sensor_time[2])                                 ? FALSE : TRUE;
  data_are_old    = (now - T_sensor_time[2] > T_sensor_time_staleness) ? TRUE : FALSE;
  for( i=0; i<4; i++){
    for( j=0; j<4; j++){
      int idx = i*4+j;
      if (abs(T_sensor[2][idx]) > 99) {   // MCCs are last sensors each MSEa
	sprintf( b, "XXX");
      }
      else {
	sprintf( b, "%+3d", T_sensor[2][idx]);
      }
      _settextposition( mseb_ts[i][j].row, mseb_ts[i][j].col);
      set_value_colors( T_sensor_status[2][idx]);
      _outtext( b);
    }
  }

  /* BT temp sensors on tracker planes from MSEA */
  
  for( i=0; i<2; i++){
    int idx[2] = { 3, 9};
    data_are_absent = (T_sensor_time[i])                                 ? FALSE : TRUE;
    data_are_old    = (now - T_sensor_time[i] > T_sensor_time_staleness) ? TRUE : FALSE;
    for( j=0; j< 2; j++){
      if (abs(T_sensor[i][idx[j]]) > 99) {   // MCCs are last sensors each MSEa 
	sprintf( b, "XXX");
      }
      else {
	sprintf( b, "%+3d", T_sensor[i][idx[j]]);
      }
      _settextposition( msea_bt[i][j].row, msea_bt[i][j].col);
      set_value_colors( T_sensor_status[i][idx[j]]);
      _outtext( b);
    }
  }

}


//------------------------------------------------------------------------------

void display_IHTS_page( void) {
  short row, col;
  display_headline( "INTERNAL TEMP SUMMARY");
  if (new_page) {
    display_IHTS_data_frame();
    display_IHTS_right_panel( row=22, col=1);
  }
  display_IHTS_data();
}
 
//------------------------------------------------------------------------------
