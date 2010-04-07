// this is file htc.c
//
// AMS 1553 bus monitor for STS PGSC computer
//
// - functions for HTC data
//

#include "pgsc.h"               // it has some global data definitions

//------------------------------------------------------------------------------

static int HTC_index[8] = { 0, 1, 2, 3, 4,  5, -1, -1};
static int HTC_node_number[5] = { 0x0, 0x1, 0x2, 0x3, 0x4};

static char *HTC_name[5] = { "HTC.11", "HTC.23", "HTC.51", "HTC.63", "HTC.83"};
static short HTC_col[5]  = {     4,       16,       28,       40,       52};

static struct {
  char  *number;
  short row;
  char  *name_in_HTC[5];
  float power_in_HTC[5];
  short bit_in_HTC[5];
  short index_in_HTC[5];
} chain[16] = {

//  chain disp <-- heater location -->   <-- power --->  <- power bit->  <-temp.index->
//   name row  11   23   51   63   83   11 23 51 63 83   11 23 51 63 83  11 23 51 63 83

   {"0a",  4, "MG","MG","MG","MG","CC", 20,20,20,20, 4,   1, 1, 1, 1, 9,  1, 1, 1, 1, 6},
   {"1a",  5, "CR","CR","CR","CR","CC", 31,31,31,31, 4,   2, 2, 2, 2,10,  2, 2, 2, 2, 7},
   {"2a",  6, "CR","CR","CR","PD","CC", 31,31,31,31, 4,   3, 3, 3, 3,11,  3, 3, 3, 3, 8},
   {"3a",  7, "MG","MG","MG","MG","CC", 20,20,20,20, 4,   4, 4, 4, 4,12,  4, 4, 4, 4, 9},
   {"4a",  8, "MG","MG","MG","MG","CC", 20,20,20,20, 4,   5, 5, 5, 5,13,  5, 5, 5, 5,10},
   {"5a",  9, "MG","MG","MG","MG","CC", 20,20,20,20, 4,   6, 6, 6, 6,14,  6, 6, 6, 6,11},
   {"6a", 10, "PS","SK","PS","SK","  ", 31,14,31,14, 0,   7, 7, 7, 7,15,  7, 7, 7, 7, 0},
   {"7a", 11, "PS","SK","PS","SK","  ", 31,14,31,14, 0,   8, 8, 8, 8,16,  8, 8, 8, 8, 0},
                                                                                            
   {"0b", 12, "MG","MG","MG","MG","CC", 20,20,20,20, 4,   9, 9, 9, 9, 1,  9, 9, 9, 9, 1},
   {"1b", 13, "CR","CR","CR","CR","CC", 31,31,31,31, 4,  10,10,10,10, 2, 10,10,10,10, 2},
   {"2b", 14, "CR","CR","PD","CR","CC", 31,31,31,31, 4,  11,11,11,11, 3, 11,11,11,11, 3},
   {"3b", 15, "MG","MG","MG","MG","CC", 20,20,20,20, 4,  12,12,12,12, 4, 12,12,12,12, 4},
   {"4b", 16, "MG","MG","MG","MG","CC", 20,20,20,20, 4,  13,13,13,13, 5, 13,13,13,13, 5},
   {"5b", 17, "MG","MG","MG","MG","  ", 20,20,20,20, 0,  14,14,14,14, 6, 14,14,14,14, 0},
   {"6b", 18, "PS","SK","PS","SK","  ", 31,14,31,14, 0,  15,15,15,15, 7, 15,15,15,15, 0},
   {"7b", 19, "PS","SK","PS","SK","CC", 31,14,31,14, 0,  16,16,16,16, 8, 16,16,16,16,12}
};  

static short HTC_total_power_row = 21;

/*      Mike's PDD_bits are wrong
static int PDD_bits[5][2] = {{4,6}, {3,5}, {14,12}, {13,15}, {7,11}};
*/

static int PDD_bits[5][2] = {{12,10}, {13,11}, {2,4}, {3,1}, {5,9}};

//------------------------------------------------------------------------------

void simulate_HTC_block( void) {

  int i, ii, iii, j, m;   static long q = 0;
  short temperature;
  time_t now;
  static int first = TRUE;
  
  ii = rand() % 5;                                           // HTC number

  q++;  if (q > 20) q = 0; if (q > 10 && ii == 2) return;
  
//AMS_block_ID = block_type<<13 | node_type<<9 | node_number<<6 | data_type;

  iii = rand() % 2;
  
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

//*************************************
//*                                   *
//*   simulate heaters ON/OFF states  *
//*       (a.k.a. digital I/O)        *
//*                                   *
//*************************************

  if (iii == 0) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   HTC_node_type         << 9 |
                   HTC_node_number[ii]   << 6 |
                   STAT_data_type;
    
    AMS_block_body[11] = 0;                     // "MCC off mask"
    for (j=0; j<16; j++) {
      if (chain[j].power_in_HTC[ii] != 0.0) {
        m = 1 << (16 - chain[j].bit_in_HTC[ii]);
        if (rand()%2!=0) {
          AMS_block_body[11] |= m;              // "MCC off mask"
        }
      }
    }

    AMS_block_body[15] = 0;                     // "image"
    for (j=0; j<16; j++) {
      if (chain[j].power_in_HTC[ii] != 0.0) {
        m = 1 << (16 - chain[j].bit_in_HTC[ii]);
        AMS_block_body[15] = AMS_block_body[11];// "image"
        if (rand()%4==0) {
          AMS_block_body[15] ^= m;              // "image"
        }
      }
    }

    AMS_block_length = 1 + 3 + 14;
  
    if (first) {
      AMS_block_length = 2;
      first = FALSE;
    }
    
    return;
  }
  
//*********************************************
//*                                           *
//*         simulate temperature data         *
//*         (a.k.a. DS1820 or Dallas)         *
//*                                           *
//*********************************************

  if (iii == 1) {

    AMS_block_ID = GET_REPLY_block_type << 13 |
                   HTC_node_type         << 9 |
                   HTC_node_number[ii]   << 6 |
                   TEMP_data_type;

    i = 3;
    for (j=0; j<16; j++) {
      if (chain[j].power_in_HTC[ii] != 0.0) {
        temperature = (rand() % 100) - 50;
        if (rand() % 16 == 0) temperature = 0x1FFF / 2;
        AMS_block_body[i++] = 2 * temperature;
      }
    }

    AMS_block_length = 1 + 3 + 64;
    return;
  }
  
}

//------------------------------------------------------------------------------

void initialize_HTC_page( void) {
  int i, j;
  static short first = TRUE;
  if (first) {
    first = FALSE;
    HTC_grand_total.power.capacity = 0.0;
    for (i=0; i<5; i++) {
      HTC[i].total.power.capacity = 0.0;
      for (j=0; j<16; j++) {
        HTC[i].chain[j].AWR = 0;
        HTC[i].total.power.capacity += chain[j].power_in_HTC[i];
      }
      HTC_grand_total.power.capacity += HTC[i].total.power.capacity;
    }
  }
}

//------------------------------------------------------------------------------

void process_HTC_block( void) {
  int i, ii, j, k, l, m;

//********************************
//*                              *
//*  HTC block structure checks  *
//*                              *
//********************************

  if ((block_type != GET_REPLY_block_type) &&
      (block_type != EXE_REPLY_block_type) &&
      (block_type != HK_DATA_block_type  )) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "HTC block with wrong block type %X(16)", block_type);
    write_log_pipe( b);
    return;
  }

  if (node_type != HTC_node_type) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "HTC block with wrong node type %X(16)", node_type);
    write_log_pipe( b);
    return;
  }  

  ii = HTC_index[node_number];

  if (ii < 0) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "HTC block with wrong node number %X(16)", node_number);
    write_log_pipe( b);
    return;
  }  

//**************************************
//*                                    *
//* process PDD switches ON/OFF states *
//*       (a.k.a. digital I/O)         *
//*                                    *
//**************************************

  if((node_number == 5)&&((data_type & 0x1F) == STAT_data_type)) { 
    if (AMS_block_length != (1+3+16)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "PDD STAT_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    PDD_stat_time = time(NULL);
    PDD_stat      = AMS_block_body[15];
    for (ii=0; ii<5; ii++){
      for (j=0; j<16; j++) {
        m = 1 << (16 - PDD_bits[ii][j/8]);                         
//==>   if bit is set, PDD power is OFF
        if (PDD_stat & m) HTC[ii].chain[j].PDD_ON = FALSE;
        else              HTC[ii].chain[j].PDD_ON = TRUE;
//      printf("%d %2d %4.4X %4.4X %4.4X\n",
//      ii,j,m,PDD_stat,HTC[ii].chain[j].PDD_ON);
      }
    }
    return;
  }
  
//*************************************
//*                                   *
//*    process heaters ON/OFF states  *
//*       (a.k.a. digital I/O)        *
//*                                   *
//*************************************

  if ((data_type & 0x1F) == STAT_data_type) {

    if (AMS_block_length != (1+3+16)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "HTC STAT_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }

    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==1) return; // simulate old data
    }

    HTC[ii].digital_IO_time = time(NULL);
     
    HTC[ii].total.power.enabled = 0.0;
    for (j=0; j<16; j++) {
      m = 1 << (16 - chain[j].bit_in_HTC[ii]);
      HTC[ii].chain[j].ENABLED = FALSE;
      if (AMS_block_body[11] & m) {
        HTC[ii].chain[j].ENABLED = TRUE;
        if (HTC[ii].chain[j].PDD_ON && PDD_stat_time) {
          HTC[ii].total.power.enabled += chain[j].power_in_HTC[ii];
        }
      }
    }
    
    HTC[ii].total.power.used = 0.0;
    for (j=0; j<16; j++) {
      m = 1 << (16 - chain[j].bit_in_HTC[ii]);
      HTC[ii].chain[j].ON = FALSE;
      if (AMS_block_body[15] & m) {
        HTC[ii].chain[j].ON = TRUE;
        if (HTC[ii].chain[j].PDD_ON && PDD_stat_time) {
          HTC[ii].total.power.used += chain[j].power_in_HTC[ii];
        }
      }
    }
    
    HTC_grand_total.power.enabled = 0.0;
    HTC_grand_total.power.used = 0.0;
    for (i=0; i<5; i++) {
      if (HTC[i].digital_IO_time && PDD_stat_time) {
        HTC_grand_total.power.enabled += HTC[i].total.power.enabled;
        HTC_grand_total.power.used += HTC[i].total.power.used;
      }
    }

// check limits, set warnings or alarms

    for (j=0; j<16; j++) {
      if (chain[j].power_in_HTC[ii]) {
        HTC[ii].chain[j].status = NORMAL;
        if ((HTC[ii].chain[j].ON && !HTC[ii].chain[j].ENABLED) ||
            !HTC[ii].chain[j].PDD_ON) {
          HTC[ii].chain[j].status = WARNING;
          HTC_grand_total.power.status = WARNING;
        }
      }
    }

    HTC_grand_total.power.status = NORMAL;
    for (i=0; i<5; i++) {
      if (HTC[i].digital_IO_time && PDD_stat_time) {
        HTC[i].total.power.status = NORMAL;
        if (HTC[i].total.power.used > HTC[i].total.power.enabled) {
          HTC[i].total.power.status = WARNING;
          HTC_grand_total.power.status = WARNING;
        }
      }
    }

    if (HTC[ii].total.power.used > 0.8 * HTC[ii].total.power.capacity)
        HTC[ii].total.power.status = ALARM;
  
    if (HTC_grand_total.power.used > 0.8 * HTC_grand_total.power.capacity)
        HTC_grand_total.power.status = ALARM;
  
    return;
  }

//*********************************************
//*                                           *
//*         process temperature data          *
//*         (a.k.a. DS1820 or Dallas)         *
//*                                           *
//*********************************************

  if ((data_type & 0x1F) == TEMP_data_type) {

    if (AMS_block_length != (1+3+64)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "HTC TEMP_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }

    if (SIMULATION) {
//    if (time(NULL)%60<40 && ii==3) return; // simulate old data
    }

    HTC[ii].temperature_time = time(NULL);
    
    for (j=0; j<16; j++) {
      if (chain[j].name_in_HTC[ii][0] != ' ') {
        i = chain[j].index_in_HTC[ii] + 2;
        HTC[ii].chain[j].heater_temperature = ((short) AMS_block_body[i]) / 2;
      }
    }

    for (j=0; j<16; j++) {
      HTC[ii].chain[j].temperature_status = NORMAL;
      if (HTC[ii].AWR_time) {
        if (HTC[ii].chain[j].AWR & 0x44)
            HTC[ii].chain[j].temperature_status = WARNING;
        if (HTC[ii].chain[j].AWR & 0x88) 
            HTC[ii].chain[j].temperature_status = ALARM;      
      }
      if (abs(HTC[ii].chain[j].heater_temperature) > 99)
              HTC[ii].chain[j].temperature_status = ALARM;      
    }
///*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if (write_N_tuples) {
  struct tm *tm_ptr;
  static FILE *file_02 = NULL;
  static FILE *file_32 = NULL;
  static FILE *file_72 = NULL;
  if (file_02 == NULL) file_02 = fopen( "cr02", "w");
  if (file_02 != NULL) {
    tm_ptr = localtime( &block_time);
    if (ii == 0) fprintf( file_02, "%3d %6.3f %d\n",
    tm_ptr->tm_yday+1,
    (float)tm_ptr->tm_hour+((float)tm_ptr->tm_min+(float)tm_ptr->tm_sec/60.0)/60.0,
    HTC[0].chain[10].heater_temperature
    );
  }
  if (file_32 == NULL) file_32 = fopen( "cr32", "w");
  if (file_32 != NULL) {
    tm_ptr = localtime( &block_time);
    if (ii == 1) fprintf( file_32, "%3d %6.3f %d\n",
    tm_ptr->tm_yday+1,
    (float)tm_ptr->tm_hour+((float)tm_ptr->tm_min+(float)tm_ptr->tm_sec/60.0)/60.0,
    HTC[1].chain[10].heater_temperature
    );
  }
  if (file_72 == NULL) file_72 = fopen( "cr72", "w");
  if (file_72 != NULL) {
    tm_ptr = localtime( &block_time);
    if (ii == 3) fprintf( file_72, "%3d %6.3f %d\n",
    tm_ptr->tm_yday+1,
    (float)tm_ptr->tm_hour+((float)tm_ptr->tm_min+(float)tm_ptr->tm_sec/60.0)/60.0,
    HTC[3].chain[10].heater_temperature
    );
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///*
    return;
  }

//*********************************************
//*                                           *
//*         process AWR data                  *
//*                                           *
//*********************************************

  if ((data_type & 0x1F) == AWR_data_type) {

     if (AMS_block_length != (1+3+48)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "HTC AWR_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
   
    HTC[ii].AWR_time = time(NULL);
    for (j=0; j<16; j++) {
      int shifter;
      i = (chain[j].bit_in_HTC[ii]-1)/2 + 19;
      shifter = 8*( chain[j].bit_in_HTC[ii]%2 );
      HTC[ii].chain[j].AWR = (AMS_block_body[i]>>shifter) & 0xFF;
    }
  }
}

//------------------------------------------------------------------------------

void display_HTC_right_panel( short row, short col) {

  int i;
  
  set_colors( headline); 
  
  for (i=row; i<21; i++) display_line( i, col, 80, " ");

  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext("Color coding:");
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("7 x");  
  set_colors( headline); 
  _outtext(" - constant");

  _settextposition( ++row, col+1);
  set_colors( old_data); 
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
  
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext("Abrvtns:");
  
  _settextposition( ++row, col+1);
  _outtext("Lc - HE location");
  
  _settextposition( ++row, col+1);
  _outtext("St - HE state:");
  _settextposition( ++row, col+1);
  _outtext("  E  = Ctl Enab");
  _settextposition( ++row, col+1);
  _outtext("  D  = Ctl Disab");
  _settextposition( ++row, col+1);
  _outtext(" +/- = ON/OFF   ");
  _settextposition( ++row, col+1);
  _outtext(" XX  = PDD OFF  ");

  _settextposition( ++row, col+1);
  _outtext("P  - HE power W");

  _settextposition( ++row, col+1);
  sprintf( b, "t%s - temp. %sC", GRAD, GRAD);  
  _outtext( b);  
  
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext("HE - heater");
  _settextposition( ++row, col+1);
  _outtext("     element(s)");
  
}

//------------------------------------------------------------------------------

void display_HTC_data_frame( void) {     

  int i, j;
  
  set_colors( frame);
  
  _settextposition( 2, 0);                                 
  sprintf( b, "  %s", VBAR);
  _outtext( b);
  _settextposition( 3, 0);
  sprintf( b, " #%s", VBAR);
  _outtext( b);

  for (i=0; i<5; i++) {
    _settextposition( 2, HTC_col[i]);                                 
    sprintf(b," %6s(%1d) %s",HTC_name[i],i,VBAR);
    _outtext(b);
    _settextposition( 3, HTC_col[i]);
    sprintf(b,"Lc P St  t%s%s",GRAD,VBAR);
    _outtext(b);
  }
 
  for (j=0; j<16; j++) {
    sprintf( b, "%s", chain[j].number);
    _settextposition( chain[j].row, 0);
    _outtext( b);
    for (i=0; i<5; i++) {
      if (chain[j].name_in_HTC[i][0] != ' ') {
        sprintf( b, "%s%2.2s %-2.0f",
                 VBAR,
                 chain[j].name_in_HTC[i],
                 chain[j].power_in_HTC[i]);
      }
      else {
        sprintf( b, "%s%2.2s   ",VBAR,chain[j].name_in_HTC[i]);
      }
      _settextposition( chain[j].row, HTC_col[i]-1);
      _outtext( b);
    }
    _settextposition( chain[j].row, HTC_col[4]+11);
    sprintf( b, "%s", VBAR);
    _outtext( b);
  }
  
  _settextposition( HTC_total_power_row, 0);
  _outtext( "P=");
  for (i=0; i<5; i++) {
    sprintf( b, "%s%3.0fW ", VBAR, HTC[i].total.power.capacity);
    _settextposition( HTC_total_power_row, HTC_col[i]-1);
    _outtext( b);
  }
  sprintf( b,"%s %s=%4.0fW ", VBAR, SIGMA, HTC_grand_total.power.capacity);
  _settextposition( HTC_total_power_row, 63);
  _outtext( b);
  
}

//------------------------------------------------------------------------------

void display_HTC_data( void) {

  int i, j, k, data_may_be_wrong;
  char c1, c2, c3;
  long now;
      
  if (page_frozen) return;
    
  now = time( NULL);
  
  data_may_be_wrong = 0;

  for (i=0; i<5; i++) {
    if (HTC[i].digital_IO_time && PDD_stat_time) data_are_absent = FALSE;
    else                                         data_are_absent = TRUE;
    if ((now - HTC[i].digital_IO_time > HTC_digital_IO_time_staleness) ||
        (now - PDD_stat_time          > PDD_stat_time_staleness)) data_are_old = TRUE;
    else                                                          data_are_old = FALSE;
    for (j=0; j<16; j++) {
      c3 = ' ';
      // FOR DEBUGGING 
      // if(HTC[i].chain[j].AWR & 0x40) c3 = 'W';
      // if(HTC[i].chain[j].AWR & 0x80) c3 = 'A';
      // if(HTC[i].chain[j].AWR & 0x04) c3 = 'w';
      // if(HTC[i].chain[j].AWR & 0x08) c3 = 'a';
      if (!HTC[i].chain[j].PDD_ON) {
        sprintf( b, "XX%c", c3);
      }
      else {
        if (HTC[i].chain[j].ENABLED) c1 = 'E';
        else                         c1 = 'D';
        if (HTC[i].chain[j].ON) c2 = '+';
        else                    c2 = '-';
        sprintf( b, "%c%c%c", c1, c2, c3);
      }
      _settextposition( chain[j].row, HTC_col[i]+5);
      if (chain[j].power_in_HTC[i]) {
        set_value_colors( HTC[i].chain[j].status);
        _outtext( b);
      }
      else {
        set_colors( frame);
        sprintf( b, "  %c", c3);
        _outtext( b);
      }
    }
    sprintf( b, "%4.0fW ", HTC[i].total.power.used);
    set_value_colors( HTC[i].total.power.status);
    _settextposition( HTC_total_power_row, HTC_col[i]+5);
    _outtext( b);
  }

  data_are_absent = TRUE;

  for (i=0; i<5; i++) {
    if (HTC[i].digital_IO_time) data_are_absent = FALSE;
    if (now - HTC[i].digital_IO_time > HTC_digital_IO_time_staleness) data_are_old = TRUE;
  }

  if (!PDD_stat_time) data_are_absent = TRUE;
  if (now - PDD_stat_time > PDD_stat_time_staleness) data_are_old = TRUE;

  sprintf( b, " %s=%4.0fW ", SIGMA, HTC_grand_total.power.used);
  set_value_colors( HTC_grand_total.power.status);
  _settextposition( HTC_total_power_row, 72);
  _outtext( b);
  
  for (i=0; i<5; i++) {
    if (HTC[i].temperature_time) data_are_absent = FALSE;
    else                         data_are_absent = TRUE;
    if (now - HTC[i].temperature_time > HTC_temperature_time_staleness) data_are_old = TRUE;
    else                                                                data_are_old = FALSE;
    for (j=0; j<16; j++) {
      _settextposition( chain[j].row, HTC_col[i]+8);
      if (chain[j].name_in_HTC[i][0] != ' ') {
        if (abs(HTC[i].chain[j].heater_temperature) > 99) {
          sprintf( b, "XXX");
        }
        else {
          sprintf( b, "%+3d", HTC[i].chain[j].heater_temperature);
        }
        set_value_colors( HTC[i].chain[j].temperature_status);
        _outtext( b);
      }
      else {
        set_colors( frame);
        _outtext( "   ");
      }
    }
  }
  
}

//------------------------------------------------------------------------------

void display_HTC_page( void) {
  short row, col;
  display_headline( "HEATERS & TEMP. PAGE");
  if (new_page) {
    display_HTC_data_frame();
    display_HTC_right_panel( row=2, col=64);
  }
  display_HTC_data();
}
 
//------------------------------------------------------------------------------
