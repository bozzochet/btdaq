// this is file daq.c
//
// AMS 1553 bus monitor for STS PGSC computer
//
// - functions for JL3A/B and JDQS stati monitoring
//


#include "pgsc.h"               // it has some global data definitions

//------------------------------------------------------------------------------

static int JL3_index[8] = { 0, 1, 2, 3, 4, 5, -1, -1};
static int JL3_node_number[6] = { 0x0, 0x1, 0x2, 0x4, 0x5, 0x7};

static char *JL3_name[6] = 
  { "JL3A02", "JL3B04", "JL3B06", "JL3B13", "JL3B15", "JL3A17"};

//---

static int JDQS_index[8]       = {    0,    1,    2,    3,    4,    5,    6,    7};
static int JDQS_node_number[8] = {  0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7};

char *JDQS_name[8] = { "JDQS01", "JDQS31", "JDQS41", "JDQS71", 
                       "JDQS03", "JDQS33", "JDQS43", "JDQS73"};
//------------------------------------------------------------------------------

void simulate_DAQ_block( void) {

  int ii;   static long q = 0;
  time_t now;
  
  ii = 0;                                                 // JL3 index
  q++;  if (q > 20) q = 0; if (q > 10 && ii == 1) return;
  
//AMS_block_ID = block_type<<13 | node_type<<9 | node_number<<6 | data_type;
  AMS_block_ID =        0x4<<13 |       0x9<<9 |          ii<<0 |         0;
  
//*********************************************
//*                                           *
//*      simulate HK block 'header'           *
//*                                           *
//*********************************************

  AMS_block_body[0] = (AMS_block_body[0] + 1) & 0x0FFF;
  if (rand()%32==0) AMS_block_body[0] |= 0x8000;
  now = time( NULL);
  AMS_block_body[1] = (now & 0xFFFF0000) >> 16;
  AMS_block_body[2] =  now & 0x0000FFFF;

//*********************************************
//*                                           *
//*        simulate JL3/DAQ status data       *
//*                                           *
//*********************************************

  AMS_block_ID = GET_REPLY_block_type << 13 |
                 JL3_node_type        <<  9 |
                 JL3_node_number[ii]  <<  6 |
                 JL3_status_data_type;
  AMS_block_length = 1 + 3 + 2 * 30;
}

//------------------------------------------------------------------------------

void initialize_DAQ_page( void) {
  static short first = TRUE;
  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------
  
void process_DAQ_block( void) {

  int i, ii, j, k;
  integer32 system_mode;
  integer32 system_status;
  
  static time_t    event_number_t1 = 0;
  static time_t    event_number_t0 = 0;
  static integer32 event_number_0  = 0xFFFFFFFF;
  static integer32 event_number_1  = 0xFFFFFFFF;

  static time_t    accepted_event_number_t1 = 0;
  static time_t    accepted_event_number_t0 = 0;
  static integer32 accepted_event_number_0  = 0xFFFFFFFF;
  static integer32 accepted_event_number_1  = 0xFFFFFFFF;

  static time_t    data_frame_number_t1 = 0;
  static time_t    data_frame_number_t0 = 0;
  static integer32 data_frame_number_0  = 0xFFFFFFFF;
  static integer32 data_frame_number_1  = 0xFFFFFFFF;
  static float     data_frame_rate;

  static integer32 old_JBU_frame_counter[6];

  static long nb_of_events_before_crash = 0;

//************************************
//*                                  *
//*  JL3/JDQS block structure checks *
//*                                  *
//************************************

  if ((block_type != GET_REPLY_block_type) &&
      (block_type != EXE_REPLY_block_type) &&
      (block_type != HK_DATA_block_type  )) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "JL3/JDQS block with wrong block type %X(16)", block_type);
    write_log_pipe( b);
    return;
  }

//*************************************
//*                                   *
//*  check & process JL3 status data  *
//*                                   *
//*************************************

  if (node_type == JL3_node_type && data_type == JL3_status_data_type) {

    ii = JL3_index[node_number];

    if (ii < 0) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "JL3 block with wrong node number %X(16)", node_number);
      write_log_pipe( b);
      return;
    }

    if (AMS_block_length != (1+3+2*38) && AMS_block_length != (1+3+2*100)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "JL3_status_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
  
    JL3_time[ii] = time( NULL); 

    for (i=0, k=3; i<(AMS_block_length-1-3)/2; i++, k+=2) {
      JL3_status[ii][i] =
      ((unsigned long) AMS_block_body[k] << 16) | AMS_block_body[k+1];
    }
    system_mode   = JL3_status[ii][0];
    system_status = JL3_status[ii][1];

//---  if it is "lv3_proc" JL3 then ...

    if (system_mode & 0x08000000) {

      event_number_time = time( NULL);
      event_number = JL3_status[ii][12];
      event_number_0  = event_number;
      event_number_t0 = (AMS_block_data[2] << 16) | AMS_block_data[3];
      if (event_number_0 == event_number_1) event_number_status = WARNING;
      else                                  event_number_status = NORMAL;
      L1_rate = 0.0;
      L1_rate_status = NO_DATA;
      if ((event_number_0  > event_number_1)  && 
          (event_number_t0 > event_number_t1) &&
           event_number_1  != 0               &&
           event_number_t1 != 0) {
        L1_rate_time = time( NULL);
        L1_rate = (float) (event_number_0  - event_number_1) / 
                  (float) (event_number_t0 - event_number_t1);
        if (L1_rate > 0.0) L1_rate_status = NORMAL;
      }
      event_number_1  = event_number_0;
      event_number_t1 = event_number_t0;

      accepted_event_number_time = time( NULL);
      accepted_event_number = JL3_status[ii][47];
      accepted_event_number_0  = accepted_event_number;
      accepted_event_number_t0 = (AMS_block_data[2] << 16) | AMS_block_data[3];
      if (accepted_event_number_0 == accepted_event_number_1) accepted_event_number_status = WARNING;
      else                                                    accepted_event_number_status = NORMAL;
      L3_rate = 0.0;
      L3_rate_status = NO_DATA;
      if ((accepted_event_number_0  > accepted_event_number_1)  && 
          (accepted_event_number_t0 > accepted_event_number_t1) &&
           accepted_event_number_1  != 0               &&
           accepted_event_number_t1 != 0) {
        L3_rate_time = time( NULL);
        L3_rate = (float) (accepted_event_number_0  - accepted_event_number_1) / 
                  (float) (accepted_event_number_t0 - accepted_event_number_t1);
        if (L3_rate > 0.0) L3_rate_status = NORMAL;
      }
      accepted_event_number_1  = accepted_event_number_0;
      accepted_event_number_t1 = accepted_event_number_t0;
///*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if (write_N_tuples) {
  struct tm *tm_ptr;
  static FILE *file = NULL;
  if (file == NULL) file = fopen( "rates", "w");
  if (file != NULL) {
    tm_ptr = localtime( &block_time);
    fprintf( file, "%3d %6.3f %6.0f %6.0f %6.3f %6.2f\n",
    tm_ptr->tm_yday+1,
    (float)tm_ptr->tm_hour+((float)tm_ptr->tm_min+(float)tm_ptr->tm_sec/60.0)/60.0,
    L1_rate,
    L3_rate,
    JL1_scaler_livetime,
    data_frame_rate
    );
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///*
    }

//---  if it is "readout" JL3 then ...

    if (system_mode & 0x02000000) {
      integer16 A, B;
      run_number_time = time( NULL);
      run_number      = JL3_status[ii][8];
      run_tag_time = time( NULL);
      run_tag      = JL3_status[ii][10];
      run_type_time = time( NULL);
      run_type      = JL3_status[ii][1] & 0x0000003F;
      if (run_type == 0x01) run_type_status = NORMAL;
      else if (run_type == 0x02 || 
               run_type == 0x04 || 
               run_type == 0x08 || 
               run_type == 0x10 || 
               run_type == 0x20) run_type_status = WARNING;
           else run_type_status = ALARM;
      crate_found_time = time( NULL);
      crate_found      = JL3_status[ii][5];
      A = crate_found & 0x0000FFFF;
      B = crate_found >> 16;
      crate_found_status                        = NORMAL;
      if (A != B)            crate_found_status = WARNING;
      if ((A | B) != 0x07FE) crate_found_status = ALARM;
    }

//---  if it is "jfo_ctrl" JL3 then ...

    if (system_mode & 0x01000000) {
      data_frame_number_time = time( NULL);
      data_frame_number = JL3_status[ii][14];
      data_frame_number_0  = data_frame_number;
      data_frame_number_t0 = (AMS_block_data[2] << 16) | AMS_block_data[3];
      if (data_frame_number_0 == data_frame_number_1) data_frame_number_status = WARNING;
      else                                            data_frame_number_status = NORMAL;
      data_frame_rate = 0.0;
      data_frame_rate_status = NO_DATA;
      if ((data_frame_number_0  > data_frame_number_1)  && 
          (data_frame_number_t0 > data_frame_number_t1) &&
           data_frame_number_1  != 0               &&
           data_frame_number_t1 != 0) {
        data_frame_rate_time = time( NULL);
        data_frame_rate = (float) (data_frame_number_0  - data_frame_number_1) / 
                  (float) (data_frame_number_t0 - data_frame_number_t1);
        if (data_frame_rate > 0.0) data_frame_rate_status = NORMAL;
      }
      data_frame_number_1  = data_frame_number_0;
      data_frame_number_t1 = data_frame_number_t0;
      JBU_frame_counter_time = time( NULL);
      for (j=0; j<6; j++) {
        JBU_frame_counter[j] = JL3_status[ii][41+j];
        if (JBU_frame_counter[j] != 0 &&
            JBU_frame_counter[j] == old_JBU_frame_counter[j]) {
          JBU_frame_counter_status[j] = WARNING;
        }
        else {
          JBU_frame_counter_status[j] = NORMAL;
        }
        old_JBU_frame_counter[j] = JBU_frame_counter[j];
      }
      JFO_used_time = time( NULL);
      if (JL3_status[ii][50]) {
        sprintf( JFO_used, "B");
        JFO_used_status = WARNING;
      }
      else {
        sprintf( JFO_used, "A");
        JFO_used_status = NORMAL;
      }
      JBU_output_time = time( NULL);
      if (system_status & 0x00000200) {
        sprintf( JBU_output, " ON");
        JBU_output_status = NORMAL;
      }
      else {
        sprintf( JBU_output, "OFF");
        JBU_output_status = WARNING;
      }
///*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if (write_N_tuples) {
  struct tm *tm_ptr;
  static FILE *file = NULL;
  if (file == NULL) file = fopen( "jbu", "w");
  if (file != NULL) {
    tm_ptr = localtime( &block_time);
    fprintf( file, "%3d %6.3f %12d\n",
    tm_ptr->tm_yday+1,
    (float) tm_ptr->tm_hour +
    ((float) tm_ptr->tm_min + 
    (float) tm_ptr->tm_sec / 60.0) / 60.0,
    JBU_frame_counter[0] + 
    JBU_frame_counter[1] + 
    JBU_frame_counter[2] + 
    JBU_frame_counter[3] + 
    JBU_frame_counter[4] + 
    JBU_frame_counter[5]
    );
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//*/
    }

//---  if it is "jl1_ctrl" JL3 then ...

    if (system_mode & 0x04000000) {
      JL1_used_time = time( NULL);
      if (JL3_status[ii][51]) {
        sprintf( JL1_used, "B");
        JL1_used_status = WARNING;
      }
      else {
        sprintf( JL1_used, "A");
        JL1_used_status = NORMAL;
      }
    }
    
//---  if it is "jfo_ctrl" JL3 or if it is "lv3_proc" JL3 then ...

    if ((system_mode & 0x01000000) || (system_mode & 0x08000000)) {
      event_size_status = NO_DATA;
      if (L3_rate && data_frame_rate) {
        event_size = 4080.0 * (float) data_frame_rate / (float) L3_rate;
        event_size_status = NORMAL;
      }
    }

//---  if it is "lv3_proc" JL3 or if it is "readout" JL3 then ...

    if ((system_mode & 0x08000000) || (system_mode & 0x02000000)) {
      static integer32 old_event_number;    
      if (run_number && run_tag && run_type == 0x01 && run_tag != this_run.tag) {
        nb_of_events_before_crash = 0;
        this_run.tag = run_tag;
      }
      else {
        if (event_number < old_event_number) {
          nb_of_events_before_crash += old_event_number;
        }
      }
      this_run.nb_of_events = nb_of_events_before_crash + event_number;
      old_event_number = event_number;
    }

    return;
  }
    
//*************************************
//*                                   *
//*  check & process JL1 scaler data  *
//*                                   *
//*************************************

  if (node_type == JL3_node_type && data_type == JL1_scaler_data_type) {

    float C;
    
    if (AMS_block_length != (1+3+100)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "JL1_scaler_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
  
    JL1_scaler_time = time( NULL); 

    JL1_scalers_status  = AMS_block_body[5];
    JL1_scaler_livetime = (float) AMS_block_body[6] / 16384.0;
    if (JL1_scaler_livetime > 1.0) JL1_scaler_livetime_status = ALARM;
    else                           JL1_scaler_livetime_status = NORMAL;
    
    if (JL1_scaler_livetime) {
      C = 2.0 / 5.24248 / JL1_scaler_livetime;
      for (i=0, k=7; i<96; i++, k++) {
        JL1_scaler_data[i] = (float)AMS_block_body[k] * C;
        JL1_scaler_status[i] = NORMAL;
      }
    }
    else {
      for (i=0, k=7; i<96; i++, k++) {
        JL1_scaler_data[i] = AMS_block_body[k];
        JL1_scaler_status[i] = WARNING;
      }
    }

    for (i=0; i<8; i++) {
      JL1_scaler_sums[i] = 0;
      if (JL1_scaler_livetime) JL1_scaler_sums_status[i] = NORMAL;
      else                     JL1_scaler_sums_status[i] = WARNING;
    }

    for (i=0;  i<10; i++) JL1_scaler_sums[0] += JL1_scaler_data[i];
    for (i=10; i<20; i++) JL1_scaler_sums[1] += JL1_scaler_data[i];
    for (i=20; i<34; i++) JL1_scaler_sums[2] += JL1_scaler_data[i];
    for (i=34; i<48; i++) JL1_scaler_sums[3] += JL1_scaler_data[i];
    for (i=48; i<62; i++) JL1_scaler_sums[4] += JL1_scaler_data[i];
    for (i=62; i<76; i++) JL1_scaler_sums[5] += JL1_scaler_data[i];
    for (i=76; i<86; i++) JL1_scaler_sums[6] += JL1_scaler_data[i];
    for (i=86; i<96; i++) JL1_scaler_sums[7] += JL1_scaler_data[i];

/*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if (write_N_tuples) {
  struct tm *tm_ptr;
  static FILE *file = NULL;
  if (file == NULL) file = fopen( "Luca", "w");
  if (file != NULL) {
    tm_ptr = localtime( &block_time);
    fprintf( file, "%3d %6.3f %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d \n",
    tm_ptr->tm_yday+1,
    (float) tm_ptr->tm_hour +
    ((float) tm_ptr->tm_min + 
    (float) tm_ptr->tm_sec / 60.0) / 60.0,

    JL1_scaler_data[76],
    JL1_scaler_data[77],
    JL1_scaler_data[78],
    JL1_scaler_data[79],
    JL1_scaler_data[80],
    JL1_scaler_data[81],
    JL1_scaler_data[82],
    JL1_scaler_data[83],
    JL1_scaler_data[84],
    JL1_scaler_data[85],

    JL1_scaler_data[86],
    JL1_scaler_data[87],
    JL1_scaler_data[88],
    JL1_scaler_data[89],
    JL1_scaler_data[90],
    JL1_scaler_data[91],
    JL1_scaler_data[92],
    JL1_scaler_data[93],
    JL1_scaler_data[94],
    JL1_scaler_data[95]
    );
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
    return;
  }


//**************************************
//*                                    *
//*  check & process JDQS status data  *
//*                                    *
//**************************************

  if (node_type == JDQS_node_type) {

    ii = JDQS_index[node_number];
  
    if (ii < 0) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "JDQS block with wrong node number %X(16)", node_number);
      write_log_pipe( b);
      return;
    }

    if (AMS_block_length != (1+3+42)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "JDQS_status_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
  
    JDQS_time[ii] = time( NULL); 

    for (i=0, k=3; i<42; i++, k++) {
      JDQS_status[ii][i] = AMS_block_body[k];
      JDQS_status_status[ii][i] = NORMAL;
    }

    JDQS_event_size[ii] = JDQS_status[ii][5] & 0x00FF;

    if (data_type & 0x20)                JDQS_status_status[ii][3] = WARNING;
    if ((JDQS_status[ii][3] >> 8) != ii) JDQS_status_status[ii][3] = ALARM;

    if (JDQS_status[ii][4] & 0x0080) JDQS_status_status[ii][4] = WARNING;
    if (JDQS_status[ii][4] & 0x015F) JDQS_status_status[ii][4] = ALARM;

    if (JDQS_status[ii][8] != 0x0200) JDQS_status_status[ii][8] = ALARM;

    if (ii < 4 && JDQS_status[ii][9] != 0xCAFE) JDQS_status_status[ii][9] = WARNING;

    if (JDQS_status[ii][10] & 0xFF00) {
      if ((JDQS_status[ii][11] & 0xFF00) == 0x1F00) {
        JDQS_status_status[ii][10] = ALARM;
      }
      else {
        JDQS_status_status[ii][10] = WARNING;
      }
    }
    
    if (JDQS_status[ii][28] & 0xC004) JDQS_status_status[ii][28] = ALARM;

///*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if (write_N_tuples) {
  struct tm *tm_ptr;
  static FILE *JDQS_file[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char *JDQS_file_name[8] = { "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7"};
  if (JDQS_file[ii] == NULL) JDQS_file[ii] = fopen( JDQS_file_name[ii], "w");
  if (JDQS_file[ii] != NULL) {
    tm_ptr = localtime( &block_time);
    fprintf( JDQS_file[ii], "%3d %6.3f %6.0f %6.0f %6.3f %6.2f %3d\n",
    tm_ptr->tm_yday+1,
    (float)tm_ptr->tm_hour+((float)tm_ptr->tm_min+(float)tm_ptr->tm_sec/60.0)/60.0,
    L1_rate,
    L3_rate,
    JL1_scaler_livetime,
    data_frame_rate,
    JDQS_event_size[ii]
    );
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///*

    return;
  } 
} 

//------------------------------------------------------------------------------

void display_DAQ_right_panel( short row, short col) {

  int i;

  set_colors( headline); 
  
  for (i=row; i<23; i++) display_line( i, col, 80, " ");

  _settextposition( ++row, col+1);
  _outtext("Color");
  _settextposition( ++row, col+1);
  _outtext("coding:");
  
  row++;
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("7 x");
  set_colors( headline); 
  _outtext(" - ");
  _settextposition( ++row, col+1);
  _outtext("const.");

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( no_data); 
  _outtext("   ");  
  set_colors( headline); 
  _outtext(" - ");  
  _settextposition( ++row, col+1);
  _outtext("no data");  
  
  row++;
  
  _settextposition( ++row, col+1);
  set_colors( normal); 
  _outtext("0.1");  
  set_colors( headline); 
  _outtext(" - ");    
  _settextposition( ++row, col+1);
  _outtext("normal");    

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( warning); 
  _outtext("0.5");  
  set_colors( headline); 
  _outtext(" - ");
  _settextposition( ++row, col+1);
  _outtext("warning");

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( alarm); 
  _outtext("0.9");  
  set_colors( headline); 
  _outtext(" - ");  
  _settextposition( ++row, col+1);
  _outtext("alarm");  

  row++;
  
  _settextposition( ++row, col+1);
  set_colors( old_data); 
  _outtext("1.5");  
  set_colors( headline); 
  _outtext(" -");  
  _settextposition( ++row, col+1);
  _outtext("old/bad");  

}

//------------------------------------------------------------------------------
  

void display_DAQ_data_frame( int page) {

  int i;

  int NLINES[3] = { 21, 21, 21};
  
  char *jl3_lines[4][21] = {
//          1         2         3         4         5         6         7
//   34567890123456789012345678901234567890123456789012345678901234567890
 {"    Status of                                                         ", // 2
  "   System Mode......... XXXX XXXX   System Status....... XXXX XXXX    ", // 3  
  "   Resource Status..... XXXX XXXX   I/O Status.......... XXXX XXXX    ", // 4
  "   DAQ Status.......... XXXX XXXX   Crate found......... XXXX XXXX    ", // 5  
  "   Readout Mask........ XXXX XXXX   LPORT Status........ XXXX XXXX    ", // 6 
  "   Run Number.......... XXXX XXXX   System Version...... XXXX XXXX    ", // 7  
  "   Run Type............ XXXX XXXX   LV3 Mode............ XXXX XXXX    ", // 8  
  "   Event Number........ XXXX XXXX   Trigger Number...... XXXX XXXX    ", // 9  
  "   Data Frame Count.... XXXX XXXX   Last Error Code..... XXXX XXXX    ", //10  
  "                                                                      ", //11  
  "   Crate 32 Status..... XXXX XXXX   Crate 72 Status..... XXXX XXXX    ", //12
  "   Crate 01 Status..... XXXX XXXX   Crate 31 Status..... XXXX XXXX    ", //13 
  "   Crate 41 Status..... XXXX XXXX   Crate 71 Status..... XXXX XXXX    ", //14  
  "   Crate 03 Status..... XXXX XXXX   Crate 33 Status..... XXXX XXXX    ", //15  
  "   Crate 43 Status..... XXXX XXXX   Crate 73 Status..... XXXX XXXX    ", //16  
  "   TDR Found in Cr 32.. XXXX XXXX   TDR Found in Cr 72.. XXXX XXXX    ", //17  
  "   Crate 32 Err Count.. XXXX XXXX   Crate 72 Err Count.. XXXX XXXX    ", //18
  "   Crate 01 Err Count.. XXXX XXXX   Crate 31 Err Count.. XXXX XXXX    ", //19 
  "   Crate 41 Err Count.. XXXX XXXX   Crate 71 Err Count.. XXXX XXXX    ", //20  
  "   Crate 03 Err Count.. XXXX XXXX   Crate 33 Err Count.. XXXX XXXX    ", //21  
  "   Crate 43 Err Count.. XXXX XXXX   Crate 73 Err Count.. XXXX XXXX    "},//22  
//   34567890123456789012345678901234567890123456789012345678901234567890
//          1         2         3         4         5         6         7
//   34567890123456789012345678901234567890123456789012345678901234567890
 {"                                                                      ", // 2  
  " -------------JDQS01-JDQS31-JDQS41-JDQS71-JDQS03-JDQS33-JDQS43-JDQS73 ", // 3  
  " Version...... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", // 4
  " Crate ID/Mask XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", // 5
  " DAQ status... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", // 6
  " Reduct/Size.. XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", // 7
  " IO/IRQ2 stat. XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", // 8
  " PFData/DataAv XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", // 9
  " Init/Fl1..... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //10
  " PWR status... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //11
  " RDY/LastCMD.. XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //12
  " Run type..... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //13
  " Temp prescal. XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //14
  " Events read.. XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //15
  " Event Nb..... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //16
  " Trig counter. XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //17
  " Temp int cnt. XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //18
  " CAN status... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //19
  " Timeouts..... XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //20
  " Bufs Max/Used XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX . XXXX  ", //21
  "                                                                      "},//22
//   34567890123456789012345678901234567890123456789012345678901234567890
//          1         2         3         4         5         6         7
// 1234567890123456789012345678901234567890123456789012345678901234567890
 {"                                                                      ", // 2  
  "  ----TOF.1P-TOF.1N-TOF.2P-TOF.2N----TOF.3P-TOF.3N-TOF.4P-TOF.4N----  ", // 3  
  "  01..XXXXXX.XXXXXX.XXXXXX.XXXXXX.01.XXXXXX.XXXXXX.XXXXXX.XXXXXX..01  ", // 4
  "  02..XXXXXX.XXXXXX.XXXXXX.XXXXXX.02.XXXXXX.XXXXXX.XXXXXX.XXXXXX..02  ", // 5
  "  03..XXXXXX.XXXXXX.XXXXXX.XXXXXX.03.XXXXXX.XXXXXX.XXXXXX.XXXXXX..03  ", // 6
  "  04..XXXXXX.XXXXXX.XXXXXX.XXXXXX.04.XXXXXX.XXXXXX.XXXXXX.XXXXXX..04  ", // 7
  "  05..XXXXXX.XXXXXX.XXXXXX.XXXXXX.05.XXXXXX.XXXXXX.XXXXXX.XXXXXX..05  ", // 8
  "  06..XXXXXX.XXXXXX.XXXXXX.XXXXXX.06.XXXXXX.XXXXXX.XXXXXX.XXXXXX..06  ", // 9
  "  07..XXXXXX.XXXXXX.XXXXXX.XXXXXX.07.XXXXXX.XXXXXX.XXXXXX.XXXXXX..07  ", // 10
  "  08..XXXXXX.XXXXXX.XXXXXX.XXXXXX.08.XXXXXX.XXXXXX.XXXXXX.XXXXXX..08  ", // 11
  "  09..XXXXXX.XXXXXX.XXXXXX.XXXXXX.09.XXXXXX.XXXXXX.XXXXXX.XXXXXX..09  ", // 12
  "  10..XXXXXX.XXXXXX.XXXXXX.XXXXXX.10.XXXXXX.XXXXXX.XXXXXX.XXXXXX..10  ", // 13
  "  11..XXXXXX.XXXXXX.XXXXXX.XXXXXX.11.XXXXXX.XXXXXX.XXXXXX.XXXXXX..11  ", // 14
  "  12..XXXXXX.XXXXXX.XXXXXX.XXXXXX.12.XXXXXX.XXXXXX.XXXXXX.XXXXXX..12  ", // 15
  "  13..XXXXXX.XXXXXX.XXXXXX.XXXXXX.13.XXXXXX.XXXXXX.XXXXXX.XXXXXX..13  ", // 16
  "  14..XXXXXX.XXXXXX.XXXXXX.XXXXXX.14.XXXXXX.XXXXXX.XXXXXX.XXXXXX..14  ", // 17
  "  ----TOF.1P-TOF.1N-TOF.2P-TOF.2N----TOF.3P-TOF.3N-TOF.4P-TOF.4N----  ", // 18
  "  SUM.XXXXXX.XXXXXX.XXXXXX.XXXXXX....XXXXXX.XXXXXX.XXXXXX.XXXXXX.SUM  ", // 19
  "                                                                      ", // 20
  "                            LIVETIME = 100.5%                         ", // 21
// 1234567890123456789012345678901234567890123456789012345678901234567890
  "                                                                      "} // 22
 };   

  set_colors( frame);

  if (page < 7) {
    for (i=0; i<NLINES[0]; i++) {
      _settextposition( 2+i, 1);
      _outtext( jl3_lines[0][i]);
    }
    return;
  }
  if (page == 7) {
    for (i=0; i<NLINES[1]; i++) {
      _settextposition( 2+i, 1);
      _outtext( jl3_lines[1][i]);
    }
    return;
  }
  if (page == 8) {
    for (i=0; i<NLINES[2]; i++) {
      _settextposition( 2+i, 1);
      _outtext( jl3_lines[2][i]);
    }
    return;
  }
}

//------------------------------------------------------------------------------

void display_DAQ_data( int page) {

  int i, j, k;
  short row, col;
  time_t now;
  integer32 system_mode;

  if (page_frozen) return;

  now = time( NULL);
  
  if (page < 7) {
    if (JL3_time[page-1]) data_are_absent = FALSE;
    else                  data_are_absent = TRUE;
    if (now - JL3_time[page-1] > JL3_time_staleness) data_are_old = TRUE;
    else                                             data_are_old = FALSE;
    system_mode = JL3_status[page-1][0];
    _settextposition( 2, 15);
    set_colors( frame);
    _outtext( JL3_name[page-1]);
    if (!data_are_absent) {
      _outtext( ", function(s) = ");
      if (system_mode & 0x1F000000) {
        if (system_mode & 0x01000000) printf( "JFO_ctrl ");
        if (system_mode & 0x02000000) printf( "Readout ");
        if (system_mode & 0x04000000) printf( "JL1_ctrl ");
        if (system_mode & 0x08000000) printf( "LV3_proc ");
        if (system_mode & 0x10000000) printf( "Scaler ");
      }
      else {
        _outtext( "unknown");
      }
    }
    row = 3;
    for (i=0, j=0; i<38; i++) {
      if( row == 11 ) row++;
      set_value_colors( NORMAL);
      if (system_mode & 0x08000000) {
        if (i == 12) set_value_colors( event_number_status);
        if (i == 14) set_value_colors( data_frame_number_status);
      }
      if (system_mode & 0x02000000) {
        if (i == 5) set_value_colors( crate_found_status);
      }
      _settextposition( row , (j % 2) ? 57 : 24);
      sprintf( b, " %4.4X %4.4X ", 
          (integer16) (JL3_status[page-1][i]>>16), 
          (integer16) (JL3_status[page-1][i]&0xFFFF));
      _outtext( b);
      if (j++ % 2) row++;
    }
    return;
  }
  
  if (page == 7) {
  
    for (j=0; j<8; j++) {
  
      if (JDQS_time[j]) data_are_absent = FALSE;
      else              data_are_absent = TRUE;
      if (now - JDQS_time[j] > JDQS_time_staleness) data_are_old = TRUE;
      else                                          data_are_old = FALSE;


      row = 4;
    
      set_value_colors( JDQS_status_status[j][0]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][0]);
      _outtext( b);
    
      set_value_colors( JDQS_status_status[j][3]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][3]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][4]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][4]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][5]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][5]);
      _outtext( b);

       set_value_colors( JDQS_status_status[j][6]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][6]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][7]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][7]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][8]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][8]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][9]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][9]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][10]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][10]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][11]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][11]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][14]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][14]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][15]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][15]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][16]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][16]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][19]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][19]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][23]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][23]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][28]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][28]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][29]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][29]);
      _outtext( b);

      set_value_colors( JDQS_status_status[j][31]);
      _settextposition( row++, 15+7*j);
      sprintf( b, " %4.4X ", JDQS_status[j][31]);
      _outtext( b);
    }
    return;
  }
  
  if (page == 8) {
  
    if (JL1_scaler_time) data_are_absent = FALSE;
    else                 data_are_absent = TRUE;
    if (now - JL1_scaler_time > JL1_scaler_time_staleness) data_are_old = TRUE;
    else                                                   data_are_old = FALSE;

    k = 0;
    for (i=0; i<8; i++) {
      row = 4;
      col = 7 + 7 * i;
      if (i > 3) col = col + 3;
      for (j=0; j<14; j++) {
        if ((j==0 || j==1 || j==12 || j==13) && 
            (i==0 || i==1 || i==6 || i==7)) {
          set_value_colors( NO_DATA);
          sprintf( b, "%5d ", 0);
        }
        else {
          set_value_colors( JL1_scaler_status[k]);
          sprintf( b, "%5d ", JL1_scaler_data[k]);
          k++;
        }
        _settextposition( row, col);
        _outtext( b);
        row++;
      }
    }
    for (i=0; i<8; i++) {
      col = 7 + 7 * i;
      if (i > 3) col = col + 3;
      if (JL1_scaler_sums[i] > 99999) {
        set_value_colors( ALARM);
        sprintf( b, " XXXX ");
      }
      else {
        set_value_colors( JL1_scaler_sums_status[i]);
        sprintf( b, "%5ld ", JL1_scaler_sums[i]);
      }
      _settextposition( 19, col);
      _outtext( b);
    }
    if (JL1_scaler_livetime) set_value_colors( JL1_scaler_livetime_status);
    else                     set_value_colors( NO_DATA);
    sprintf( b, "%5.1f%% ", JL1_scaler_livetime * 100.0);
    _settextposition( 21, 40);
    _outtext( b);
    set_colors( frame);
    _settextposition( 20, 18);
    if (JL1_scaler_livetime) _outtext( "  Rates (Hz) are corrected for a livetime  ");
    else                     _outtext( "Rates (Hz) are NOT corrected for a livetime");
    return;
  }
}

//------------------------------------------------------------------------------

void display_DAQ_page( int page) {
  short row, col;
  switch (page) {
  case 1:
    display_headline( "JL3/DAQ STATUS PAGE 1/7");
    break;
  case 2:
    display_headline( "JL3/DAQ STATUS PAGE 2/7");
    break;
  case 3:
    display_headline( "JL3/DAQ STATUS PAGE 3/7");
    break;
  case 4:
    display_headline( "JL3/DAQ STATUS PAGE 4/7");
    break;
  case 5:
    display_headline( "JL3/DAQ STATUS PAGE 5/7");
    break;
  case 6:
    display_headline( "JL3/DAQ STATUS PAGE 6/7");
    break;
  case 7:
    display_headline( "JL3/DAQ STATUS PAGE 7/7");
    break;
  case 8:
    display_headline( "JL1 SCALER PAGE");
    break;
  default:
    return;
  }
  if (new_page) {
    display_DAQ_data_frame( page);
    display_DAQ_right_panel( row=2, col=71);
  }
  display_DAQ_data( page);
}
 
//------------------------------------------------------------------------------
