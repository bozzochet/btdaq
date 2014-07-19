//
// this is file tplt.c 
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for - temp histo plot
//

#include "pgsc.h"               // it has some global data definitions

#define NLINES 21
#define NBINS  73
char *tplt_lines[NLINES] = {
/*         1         2         3         4         5         6         7         8*/
/*12345678901234567890123456789012345678901234567890123456789012345678901234567890*/
 "   |                                                                        |   ", /* 1*/
 "   |                                                                        |   ", /* 2*/
 "   |                                                                        |   ", /* 3*/
 " 15+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -+15 ", /* 4*/
 "   |                                                                        |   ", /* 5*/
 "   |                                                                        |   ", /* 6*/
 "   |                                                                        |   ", /* 7*/
 "   |                                                                        |   ", /* 8*/
 "10 + = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =+10 ", /* 9*/
 "   |                                                                        |   ", /* 0*/
 "   |                                                                        |   ", /* 1*/
 "   |                                                                        |   ", /* 2*/
 "   |                                                                        |   ", /* 3*/
 "  5+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -+5  ", /* 4*/
 "UND|                                                                        |OVR", /* 5*/
 "   |                                                                        |   ", /* 6*/
 "   |                                                                        |   ", /* 7*/
 "   |                                                                        |   ", /* 8*/
 "    +-----+----+----+----+----+----+----+----+----+----+----+----+----+----+    ", /* 9*/
 "   -20       -10        0\370C       +10       +20       +30       +40       +50   ",
 " Read  XXX = Normal  XXX + Warn  XXX + Alarm  XXX + Old/Bad  XXX, Absent  XXX   "}; /* 9*/


//------------------------------------------------------------------------------

void initialize_TPLT_page( void) {
  static short first = TRUE;
  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void simulate_TPLT_data( void) {
}

//------------------------------------------------------------------------------

void process_TPLT_data( void) {

  static short first = TRUE;

  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void display_TPLT_right_panel( short row, short col) {
  
}

//------------------------------------------------------------------------------
    
void display_TPLT_data_frame( void) {
  short row, col;
  int i,j;

  if (page_frozen) return;

  row = 2;
  col = 1;
  
  set_colors( frame);
  
  for(i=0; i<NLINES; i++){
    _settextposition( row+i,col);                                 
    _outtext( tplt_lines[i]);
  }

}

//------------------------------------------------------------------------------

#define IXT(t) ( (t < -20) ? 0 : (( t > 50) ? 72 : t+21) )
#define NENT (NLINES-3)

void display_TPLT_data( void) {

  int i, j, ixt, jys, k, klo;
  long now;
  short row, col;

  int sum[NBINS][4], sum_absent=0,sum_read=0,sumj[4]={0,0,0,0};   
  static int osum[NBINS][4];    
  static int not_first=0;

  if (page_frozen) return;

  now = time( NULL);

  sum_read = 0;
  sum_absent = 0;
  for(i=0; i<NBINS; i++){
    for(j=0; j<4; j++){
      sum[i][j] = 0;
    }
  }

  /* HTCs -- DTS temps */
   
  for (i=0; i<5; i++) {
    int cc_joke[16]={0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1};
    if((i==4)&&(cc_joke[j])) continue;
    data_are_absent = (HTC[i].temperature_time) ? FALSE : TRUE;
    data_are_old    = (now - HTC[i].temperature_time > HTC_temperature_time_staleness) ? TRUE : FALSE;
    for (j=0; ((j<16))&&((i<4)||(cc_joke[j])); j++) {
      if(data_are_absent){
	sum_absent++;
      }
      else {
	ixt = IXT(HTC[i].chain[j].heater_temperature);
	if(data_are_old){
	  jys = OLD_DATA;
	}
	else {
	  jys =  HTC[i].chain[j].temperature_status;
	}
	sum[ixt][jys]++;
	sumj[jys]++;
	sum_read++;
      }
    }
  }

  /* PSB data -- DTS temps */


  for (i=0; i<4; i++) {
    data_are_absent = (PSB[i].temperature_time) ? FALSE : TRUE;
    data_are_old    = (now - PSB[i].temperature_time > PSB_temperature_time_staleness) ? TRUE : FALSE;
    for ( j=0; j<2; j++) {
      if(data_are_absent){
	sum_absent++;
      }
      else {
	ixt = IXT(PSB[i].temperature[j].value);
	if(data_are_old){
	  jys = OLD_DATA;
	}
	else {
	  jys =  PSB[i].temperature[j].status;
	}
	sum[ixt][jys]++;
	sumj[jys]++;
	sum_read++;
      }
    }
  }


  /* temps (DTS,PT100) from MSEA */
  for (i=0; i<3; i++) {
    int nb_of_T_sensors[3] = { 28, 31, 16};
    data_are_absent = (T_sensor_time[i])                                 ? FALSE : TRUE;
    data_are_old    = (now - T_sensor_time[i] > T_sensor_time_staleness) ? TRUE : FALSE;
    for ( j=0; j<nb_of_T_sensors[i]; j++) {
      if(data_are_absent){
        sum_absent++;
      }
      else {
        ixt = IXT(T_sensor[i][j]);
        if(data_are_old){
          jys = OLD_DATA;
        }
        else {
          jys =  T_sensor_status[i][j];
        }
        sum[ixt][jys]++;
        sumj[jys]++;
        sum_read++;
      }
    }
  }

  row = 19;
  col =  4;

  data_are_absent = FALSE;
  data_are_old    = FALSE;
  for(i=1; i<NBINS-1; i++){
    if(not_first){
      char s[2];
      set_colors(frame);
      j=3;
      for(k= 0; (k<osum[i][j])&&(k<NENT); k++){
	_settextposition( row-k, col+i);
	sprintf(s,"%c",tplt_lines[row-k-2][col+i+1]);
	_outtext(s);
      }
      if(osum[i][j] > NENT){
        if(j==0){
	  _settextposition( row-NENT+1, col+i);
	  sprintf(s,"%c",tplt_lines[row-NENT+1-2][col+i+1]);
	  _outtext(s);
	}
        else if(osum[i][j] > osum[i][j-1]){
          _settextposition( row-NENT+1, col+i);
	  sprintf(s,"%c",tplt_lines[row-NENT+1-2][col+i+1]);
	  _outtext(s);
        }
      }
    }
    for(j=0; j<4; j++){
      short value = j;
      set_value_colors(value);
      if(j==0) {
	klo = 0;
      }
      else {
	klo = sum[i][j-1];
	sum[i][j] = sum[i][j] + sum[i][j-1];
      }
      for(k= klo; (k<sum[i][j])&&(k<NENT); k++){
	_settextposition( row-k, col+i);
	_outtext("X");
      }
      if(sum[i][j] > NENT){
        if(j==0){
	  _settextposition( row-NENT+1, col+i);
	  _outtext("^");
	}
        else if(sum[i][j] > sum[i][j-1]){
          _settextposition( row-NENT+1, col+i);
          _outtext("^");
        }
      }
      osum[i][j] = sum[i][j];
    }
  }


  if(not_first){
    for(j=0; j<4; j++){
      short value = j;
      set_colors(frame);
      if(osum[0][j]){
	_settextposition( 20-j, 1);
	sprintf(b,"   |");
	_outtext(b);
      }
      if(osum[NBINS-1][j]){
	_settextposition( 20-j,80-3);
	sprintf(b,"|   ");
	_outtext(b);
      }
    }
  }

  for(j=0; j<4; j++){
    short value = j;
    set_value_colors(value);
    if(sum[0][j]){
      _settextposition( 20-j, 1);
      sprintf(b,"%3d ",sum[0][j]);
      _outtext(b);
    } 
    if(sum[NBINS-1][j]){
      _settextposition( 20-j,80-3);
      sprintf(b,"%3d ",sum[NBINS-1][j]);
      _outtext(b);
    }
    osum[0][j] = sum[0][j];
    osum[NBINS-1][j] = sum[NBINS-1][j];
  }

  not_first = 1;

  _settextposition( 22, 1);
  set_colors( frame);
  _outtext(" Read ");
  set_colors(headline);
  sprintf(b, "%4d", sum_read);
  _outtext(b);
  set_colors( frame);
  _outtext(" = Normal ");
  sprintf(b, "%4d", sumj[0]);
  set_colors( normal);
  _outtext(b);
  set_colors( frame);
  _outtext(" + Warn ");
  sprintf(b, "%4d", sumj[1]);
  set_colors( warning);
  _outtext(b);
  set_colors( frame);
  _outtext(" + Alarm ");
  sprintf(b, "%4d", sumj[2]);
  set_colors( alarm);
  _outtext(b);
  set_colors( frame);
  _outtext(" + Old/Bad ");
  sprintf(b, "%4d", sumj[3]);
  set_colors( old_data);
  _outtext(b);
  set_colors( frame);
  _outtext("; Absent ");
  sprintf(b, "%4d", sum_absent);
  set_colors( warning);
  _outtext(b);
  set_colors( frame);
  _outtext("   ");

}

//------------------------------------------------------------------------------

void display_TPLT_page( void) {
  short row, col;
  display_headline( "TEMPERATURE HISTOGRAM");
  if (new_page) {
    display_TPLT_right_panel( row=22, col=24);
    display_TPLT_data_frame();
  }
  display_TPLT_data();
}
 
//------------------------------------------------------------------------------
