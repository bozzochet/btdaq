// this is file mon.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for MONitor data (MSEA and MSEB modules)
//

#include "pgsc.h"               // it has some global data definitions

//------------------------------------------------------------------------------

static int MON_index[8] = { -1, 0, -1, -1, -1, 2, 1, -1};
static int MON_node_number[3] = { 0x1, 0x6, 0x5};

int nb_of_T_sensors[3] = { 28, 31, 16};

char *T_name[3][32] = {
  "TR1.07.n", "TR1.08.n", "TR1.09.n", "TR1.10.n", 
  "TR2.05.n", "TR2.06.n", "TR2.07.n", "TR2.08.n", "TR2.09.n", "TR2.10.n", 
  "TR3.06.n", "TR3.07.n", "TR3.08.n", "TR3.09.n", 
  "TR4.06.n", "TR4.07.n", "TR4.08.n", "TR4.09.n", 
  "TR5.05.n", "TR5.06.n", "TR5.07.n", "TR5.08.n", "TR5.09.n", "TR5.10.n", 
  "TR6.08.n", "TR6.09.n", "TR6.10.n", 
  " MCC.YP ", 
  "           ", "           ", "           ", "           ",

  "TR1.07.p", "TR1.08.p", "TR1.09.p", "TR1.10.p",
  "TR2.05.p", "TR2.06.p", "TR2.07.p", "TR2.08.p", "TR2.09.p", "TR2.10.p", 
  "TR3.06.p", "TR3.07.p", "TR3.08.p", "TR3.09.p", "TR3.10.p", 
  "TR4.05.p", "TR4.06.p", "TR4.07.p", "TR4.08.p", "TR4.09.p", "TR4.10.p", 
  "TR5.05.p", "TR5.06.p", "TR5.07.p", "TR5.08.p", "TR5.09.p", "TR5.10.p", 
  "TR6.08.p", "TR6.09.p", "TR6.10.p", 
  " MCC.YN ", 
  "           ",
  
  "TS.01", "TS.11", "TS.21", "TS.31", "TS.03", "TS.13", "TS.23", "TS.33",
  "TS.02", "TS.12", "TS.22", "TS.32", "TS.04", "TS.14", "TS.24", "TS.34",
  
  };

char *B_name[2][11] = {
  "BT61_x", "BT61_y", "BT61_z", "BT61_T", "BT61_V", "BT61_t", 
  "BT11_x", "BT11_y", "BT11_z", "BT11_T", "BT11_V",
  
  "BT62_x", "BT62_y", "BT62_z", "BT62_T", "BT62_V", "BT62_t", 
  "BT12_x", "BT12_y", "BT12_z", "BT12_T", "BT12_V"
  
  };
  
char *M_name[3] = {"MSEA.31", "MSEA.71", "MSEB.02"};  

//------------------------------------------------------------------------------

//************************************************
//                                               *
//  these tables came from Werner Lustermann     *
//            (and were "reworked")              *
//                                               *
//  Bfield = BC_M * ADC - BC_N                   *
//                                               *
//  PT100  = MSEB_TM * ADC - MSEB_TN             *
//                                               *
//************************************************

#define BC_M   0  /* slope */
#define BC_N   1  /* negative intercept */

float MSEA_BC[2][11][2] = {{
//         BC_M        BC_N           MSEA31
//                                    ******
  {    5.191e-4,   8.409e-1 }, /* BT61_x */
  {    5.174e-4,   8.383e-1 }, /* BT61_y */
  {    5.177e-4,   8.387e-1 }, /* BT61_y */
  {    1.83575e-1, 553.568  }, /* BT61_T */
  {    0.001,            0. }, /* BT61_Vref */
  {    0.5263,      269.7   }, /* BT61_Tref */
  {    4.551e-4,   7.363e-1 }, /* BT11_x */
  {    4.526e-4,   7.324e-1 }, /* BT11_y */
  {    4.540e-4,   7.345e-1 }, /* BT11_y */
  {    1.83761e-1, 551.944  }, /* BT11_T */
  {    0.001,      0.       }  /* BT11_Vref */
},{
//         BC_M        BC_N           MSEA71
//                                    ******
  {    5.164e-4,   8.356e-1 }, /* BT62_x */
  {    5.173e-4,   8.370e-1 }, /* BT62_y */
  {    5.169e-4,   8.363e-1 }, /* BT62_y */
  {    1.83761e-1, 553.415  }, /* BT62_T */
  {    0.001,            0. }, /* BT62_Vref */
  {    0.5263,      269.7   }, /* BT62_Tref */
  {    4.507e-4,   7.292e-1 }, /* BT12_x */
  {    4.541e-4,   7.347e-1 }, /* BT12_y */
  {    4.544e-4,   7.352e-1 }, /* BT12_y */
  {    1.83761e-1, 552.863  }, /* BT12_T */
  {    0.001,      0.       }  /* BT12_Vref */
}};

float MSEB_TM = 0.112313;

float MSEB_TN[16] = { 
  252.991, 250.633, 247.151, 247.825, 249.622, 250.745, 249.734, 251.307,
  250.184, 249.510, 248.724, 247.713, 254.115, 248.948, 250.857, 248.387
};

//------------------------------------------------------------------------------

void simulate_MON_block( void) {

  int i, ii, iii, j, m;   static long q = 0;
  short temperature;
  time_t now;
  static int first = TRUE;
  
  ii = rand() % 3;                          // MON (MSEA/B) card number
  
  q++;  if (q > 20) q = 0; if (q > 10 && ii == 2) return;
  
//AMS_block_ID = block_type<<13 | node_type<<9 | node_number<<6 | data_type;

  iii = rand() % 3;

//*********************************************
//*                                           *
//*      simulate HK block 'header'           *
//*                                           *
//*********************************************

    AMS_block_body[0] = (AMS_block_body[0] + 1) & 0x0FFF;
    if (rand()%32==0) AMS_block_body[0] |= 0x2000;
    now = time( NULL);
    AMS_block_body[1] = (now & 0xFFFF0000) >> 16;
    AMS_block_body[2] =  now & 0x0000FFFF;

//***************************************
//*                                     *
//*   simulate Dallas temperature data  *
//*                                     *
//***************************************

  if (iii == 0) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   MON_node_type         << 9 |
                   MON_node_number[ii]   << 6 |
                   TEMP_data_type;

    i = 3;
    for (j=0; j<nb_of_T_sensors[ii]; j++) {
      temperature = (rand() % 100) - 50;
      if (rand() % 16 == 0) temperature = 0x1FFF / 2;
      AMS_block_body[i++] = 2 * temperature;
      AMS_block_body[i++] = 50;
    }

    AMS_block_length = 1 + 3 + 64;
    return;
  }

//************************************************************
//*                                                          *
//*   simulate non-Dallas (= analog input) temperature data  *
//*                                                          *
//************************************************************
  
  if (iii == 1) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   MON_node_type         << 9 |
                   MON_node_number[ii]   << 6 |
                   VOLT_data_type;

    i = 3;
    for (j=0; j<nb_of_T_sensors[ii]; j++) {
      temperature = (rand() % 198) - 99;
      AMS_block_body[i++] = temperature;
    }

    AMS_block_length = 1 + 3 + 32;
    return;
  }
  
//*****************************
//*                           *
//*   simulate B-sensor data  *
//*                           *
//*****************************
  
  if (iii == 2) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   MON_node_type         << 9 |
                   MON_node_number[ii]   << 6 |
                   MAG_data_type;

    i = 3;
    for (j=0; j<11; j++) {
      AMS_block_body[i++] = rand() % 999;
    }

    AMS_block_length = 1 + 3 + 18;
    return;
  }
 
}

//------------------------------------------------------------------------------

void initialize_MON_page( void) {
  int i, j, k;
  static short first = TRUE;

  if (first) {
    first = FALSE;
  }
  
/************************************************************************
// print "Werner's" tables for checkup, do not bother with nice format...
  {
    for (i=0; i<2; i++) {
      for (j=0; j<11; j++) {
        for (k=0; k<2; k++) {
          printf( "%f ", MSEA_BC[i][j][k]);
        }
        printf( "\n");
      }
      printf( "\n");
    }
    exit( 0);
  }
************************************************************************/ 
 
}

//------------------------------------------------------------------------------

void process_MON_block( void) {
  int i, ii, j, k;
  float data;
  float TC_b = 10.;    /* mV          */
  float TC_c = 0.039;  /* mV / deg C  */
  float TC_d = 228.3;  /* mV / mV     */

//********************************
//*                              *
//*  MON block structure checks  *
//*                              *
//********************************

  if ((block_type != GET_REPLY_block_type) &&
      (block_type != EXE_REPLY_block_type) &&
      (block_type != HK_DATA_block_type  )) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MON block with wrong block type %X(16)", block_type);
    write_log_pipe( b);
    return;
  }
  
  if (node_type != MON_node_type) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MON block with wrong node type %X(16)", node_type);
    write_log_pipe( b);
    return;
  }  

  ii = MON_index[node_number];

  if (ii < 0) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MON block with wrong node number %X(16)", node_number);
    write_log_pipe( b);
    return;
  }  
  
//***************************************
//*                                     *
//*    process Dallas temperature data  *
//*                                     *
//***************************************

  if (((data_type & 0x1F) == TEMP_data_type) && ((ii == 0) || (ii == 1))) {

    if (AMS_block_length != (1+3+64)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "MON TEMP_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    
    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==1) return; // simulate old data
    }

    T_sensor_time[ii] = time( NULL);

    for (j=0; j<nb_of_T_sensors[ii]; j++) {
      T_sensor[ii][j] = ((float)(short) AMS_block_body[3+j]) / 2.0;
      T_sensor_status[ii][j] = NORMAL;
      if (T_sensor[ii][j] > +30) T_sensor_status[ii][j] = WARNING;
      if (T_sensor[ii][j] < -30) T_sensor_status[ii][j] = WARNING;
      if (T_sensor[ii][j] > +40) T_sensor_status[ii][j] = ALARM;
      if (T_sensor[ii][j] < -40) T_sensor_status[ii][j] = ALARM;
    }

  }
  
//********************************************************************
//*                                                                  *
//*    process non-Dallas (= analog input = PT100) temperature data  *
//*                                                                  *
//********************************************************************

  if (((data_type & 0x1F) == VOLT_data_type) && (ii == 2)) {

    if (AMS_block_length != (1+3+32)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "MON VOLT_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }

    if (SIMULATION) {
//    if (time(NULL)%60<40) return; // simulate old data
    }

    T_sensor_time[ii] = time (NULL);

    for (j=0; j<nb_of_T_sensors[ii]; j++) {
      data = (short) AMS_block_body[3+j];
      T_sensor[ii][j] = (MSEB_TM * data - MSEB_TN[j]);
    }

    for (j=0; j<nb_of_T_sensors[ii]; j++) {
      T_sensor_status[ii][j] = NORMAL;
      if (T_sensor[ii][j] > +30) T_sensor_status[ii][j] = WARNING;
      if (T_sensor[ii][j] < -30) T_sensor_status[ii][j] = WARNING;
      if (T_sensor[ii][j] > +40) T_sensor_status[ii][j] = ALARM;
      if (T_sensor[ii][j] < -40) T_sensor_status[ii][j] = ALARM;
    }
  }
  
//*****************************
//*                           *
//*    process B-sensor data  *
//*                           *
//*****************************

  if (((data_type & 0x1F) == MAG_data_type) && ((ii == 0) || (ii == 1))) {

    if (AMS_block_length != (1+3+18)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "MON MAG_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }

    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==1) return; // simulate old data
    }

    B_sensor_time[ii] = time( NULL);

    for (j=0; j<11; j++) {
      data = (short) AMS_block_body[3+j];
      B_sensor[ii][j] = MSEA_BC[ii][j][BC_M] * data - MSEA_BC[ii][j][BC_N];
      B_sensor[ii][j] = fabs( B_sensor[ii][j]);
    }

    for (j=0; j<11; j++) {
      B_sensor_status[ii][j] = NORMAL;
      if (j == 3 || j == 5 || j == 9) {    // these are Temperatures...
        if (B_sensor[ii][j] > +30) B_sensor_status[ii][j] = WARNING;
        if (B_sensor[ii][j] < -30) B_sensor_status[ii][j] = WARNING;
        if (B_sensor[ii][j] > +40) B_sensor_status[ii][j] = ALARM;
        if (B_sensor[ii][j] < -40) B_sensor_status[ii][j] = ALARM;
      }
    }
  }
  
}  
//------------------------------------------------------------------------------

void display_MON_right_panel( short row, short col) {

  int i;
  
  set_colors( headline); 
  
  for ( i=row; i<22; i++) display_line( i, col, 80, " ");

  _settextposition( ++row, col+1);
  _outtext("Color coding:");
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("7 x");  
  set_colors( headline); 
  _outtext(" - constant");

  _settextposition( ++row, col+1);
  set_colors( no_data); 
  _outtext("   ");  
  set_colors( headline); 
  _outtext(" - no data");  
  
  _settextposition( ++row, col+1);
  set_colors( normal); 
  _outtext("0.1");  
  set_colors( headline); 
  _outtext(" - normal");    

  _settextposition( ++row, col+1);
  set_colors( warning); 
  _outtext("0.5");  
  set_colors( headline); 
  _outtext(" - warning");

  _settextposition( ++row, col+1);
  set_colors( alarm); 
  _outtext("0.9");  
  set_colors( headline); 
  _outtext(" - alarm");  

  _settextposition( ++row, col+1);
  set_colors( old_data); 
  _outtext("1.5");  
  set_colors( headline); 
  _outtext(" - old/bad");  
  
}

//------------------------------------------------------------------------------
    
void display_MON_data_frame( void) {     

  int i, j, k;
  short row, col;

  set_colors( frame);
  
  for (j=0; j<17; j++) {
    _settextposition( row=2+j, col=1);
    _outtext( VBAR);
  }
  
  for (i=0; i<2; i++) {
    _settextposition( row=2, col=2+24*i);
    sprintf( b, "<-----  %s  ----->%s", M_name[i], VBAR); 
    _outtext( b);
    for (j=0; j<32; j++) {
      _settextposition( row=3+(j%16), col=2+12*(j/16)+24*i);
      sprintf( b, "%s", T_name[i][j]); 
      _outtext( b);
      _settextposition( row, col+=11);
      _outtext( VBAR);
    }
  }

  _settextposition( row=2, col=50);
  sprintf( b, " %s%s", M_name[2], VBAR); 
  _outtext( b);
  for (j=0; j<16; j++) {
    _settextposition( row=3+j, col=50);
    sprintf( b, "%s", T_name[2][j]); 
    _outtext( b);
    _settextposition( row, col+=8);
    _outtext( VBAR);
  }

  for (i=0; i<2; i++) {
    _settextposition( row=2, col=59+11*i);
    sprintf( b, "  %s %s", M_name[i], VBAR); 
    _outtext( b);
    for (j=0; j<11; j++) {
      _settextposition( row=3+j, col=59+11*i);
      sprintf( b, "%s", B_name[i][j]); 
      _outtext( b);
      _settextposition( row, col+=10);
      _outtext( VBAR);
    }
  }

}

//------------------------------------------------------------------------------

void display_MON_data( void) {

  int i, j;
  long now;
  short row, col;
  
  if (page_frozen) return;
    
  now = time( NULL);

  for (i=0; i<2; i++) {
    if (T_sensor_time[i]) data_are_absent = FALSE;
    else                  data_are_absent = TRUE;
    if (now - T_sensor_time[i] > T_sensor_time_staleness) data_are_old = TRUE;
    else                                                  data_are_old = FALSE;
    for (j=0; j<nb_of_T_sensors[i]; j++) {
      set_value_colors( T_sensor_status[i][j]);
      _settextposition( row=3+(j%16), col=10+12*(j/16)+24*i);
      if (abs(T_sensor[i][j]) > 99 ) sprintf( b, "XXX");
      else                           sprintf( b, "%+3d", T_sensor[i][j]);
     _outtext( b);
    }
  }

  if (T_sensor_time[2]) data_are_absent = FALSE;
  else                  data_are_absent = TRUE;
  if (now - T_sensor_time[2] > T_sensor_time_staleness) data_are_old = TRUE;
  else                                                  data_are_old = FALSE;
  for (j=0; j<nb_of_T_sensors[2]; j++) {
    set_value_colors( T_sensor_status[2][j]);
    _settextposition( row=3+j, col=50+5);
    sprintf( b, "%+3d", T_sensor[2][j]);
    _outtext( b);
  }

  for (i=0; i<2; i++) {
    if (B_sensor_time[i]) data_are_absent = FALSE;
    else                  data_are_absent = TRUE;
    if (now - B_sensor_time[i] > B_sensor_time_staleness) data_are_old = TRUE;
    else                                                  data_are_old = FALSE;
    for (j=0; j<11; j++) {
      set_value_colors( B_sensor_status[i][j]);
      _settextposition( row=3+j, col=65+11*i);
      sprintf( b, "%4.2f", B_sensor[i][j]);
      if (B_sensor[i][j] > 10.0) sprintf( b, "%4.1f", B_sensor[i][j]);
      _outtext( b);
    }
  }

}

//------------------------------------------------------------------------------

void display_MON_page( void) {
  short row, col;
  display_headline( "MSEA & MSEB PAGE");
  if (new_page) {
    display_MON_data_frame();
    display_MON_right_panel( row=14, col=59);
  }
  display_MON_data();
} /* end of display_MON_page() */


