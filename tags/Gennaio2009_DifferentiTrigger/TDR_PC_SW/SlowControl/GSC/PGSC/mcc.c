// this is file mcc.c
//
// AMS 1553 bus monitor for STS PGSC computer
//
// - functions for MCC status etc...
//

#include "pgsc.h"               // it has some global data definitions

int MCC_status_limits[2] = { 1, 9};

#define NLINES 21
char *MCC_lines[NLINES]={
/*1234567890123456789012345678901234567890123456789012345678901  */
 "                                                             ", /*  2*/
 " Live time    -00/00:00:00-      Commands on List = XXXX-    ", /*  3*/
 "                                       Pri  #loops  #Cmds    ", /*  4*/
 " Error Counters -                        0  123456- 1234-    ", /*  5*/
 "   Cmd FIFO O'flow  123456-              1                   ", /*  6*/
 "   Cmd Length       XXXXXX-              2                   ", /*  7*/
 "   Cmd CRC          XXXXXX-              3                   ", /*  8*/
 "   CAN errors       XXXXXX-              4                   ", /*  9*/
 "   1553 Brdcst w/SD XXXXXX-              5                   ", /*  0*/
 "   1553 RT BC xmit  XXXXXX-              6                   ", /*  1*/
 "   MCC-OIU frame #  XXXXXX-              7                   ", /*  2*/
 "                                         8                   ", /*  3*/
 " Delays     ( ticks   => approx sec)     9                   ", /*  4*/
 "   1 (loop)  -0x0000- =>  12.4567-      10                   ", /*  5*/
 "   2 (step)  -0x0000- =>  12.4567-      11                   ", /*  6*/
 "                                        12                   ", /*  7*/
 "                                        13                   ", /*  8*/
 "                                        14                   ", /*  9*/
 "                                        15                   ", /* 20*/
 " Commands with Priority 'n' are executed each #loops loops.  ", /* 21*/
 "                                                             "}; /*22*/

//------------------------------------------------------------------------------

void simulate_MCC_block( void) {
}  

//------------------------------------------------------------------------------

void initialize_MCC_page( void) {
  int i, j;
  static short first = TRUE;
  if (first) {
    first = FALSE;
    RT_BUS = ' ';
  }
}

//------------------------------------------------------------------------------

void process_MCC_block( void) {
  int i;
//***************************************
//*                                     *
//*  CAN (=MCC) block structure checks  *
//*                                     *
//***************************************

  if ((block_type != GET_REPLY_block_type) &&
      (block_type != EXE_REPLY_block_type) &&
      (block_type != HK_DATA_block_type  )) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MCC block with wrong block type %X(16)", block_type);
    write_log_pipe( b);
    return;
  }

  if (node_type != MCC_node_type) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MCC block with wrong node type %X(16)", node_type);
    write_log_pipe( b);
    return;
  }  

  if (node_number != 0) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MCC block with wrong node number %X(16)", node_number);
    write_log_pipe( b);
    return;
  } 
    
  /* *******************************************
   *                                           *
   *            process 'data'                 *
   *                                           *
   *********************************************
   */

  /*
   * process List
   */
  if (data_type == 0x3) {
    integer16 *command_block = AMS_block_data + 5;
    integer16 command_block_length;
    integer16 *block = AMS_block_data;
    integer16 block_length = AMS_block_length;
    MCC_list_time = time(NULL);
    MCC_list_total = 0;
    for(i=0;i<15;i++){
      MCC_list_subt[i] = 0;
    }
    while (command_block-block <= block_length) {
      int command_block_length = command_block[-1];
      int command_priority = (command_block[1] >> 12);
      MCC_list_subt[command_priority]++;
      MCC_list_total++;
      command_block += command_block_length + 1;
    }
    MCC_list_status = (MCC_list_total) ? 
                          (MCC_list_total>10 ? NORMAL : WARNING) : ALARM;
  } 

  /*
   * process Execution delays
   */
  if (data_type == 0x6) {
    if (AMS_block_length != (1+3+2)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "MCC block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    MCC_delay_time = time(NULL);
    MCC_delay[0] = AMS_block_body[3];
    MCC_delay[1] = AMS_block_body[4];
    MCC_delay_status = (MCC_delay[0]+MCC_delay[1]) ? NORMAL : WARNING;
  } 

  /*
   * process expand values
   */
  if (data_type == 0x7) {
    if (AMS_block_length != (1+3+15)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "MCC block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    MCC_expand_time = time(NULL);
    for(i=0;i<15;i++){
      MCC_expand[i] = AMS_block_body[3+i];
    }
    MCC_expand_status = NORMAL;
  } 


  /*
   * process status values
   */
  if (data_type == 0xC) {
    if (AMS_block_length != (1+3+9)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "MCC block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }
    MCC_status_time = time(NULL);
    MCC_alivetime = AMS_block_body[3] << 16 | AMS_block_body[4];
    MCC_alivetime_status = (MCC_alivetime > 5*60) ? NORMAL : WARNING;
    for(i=0;i<7;i++){
      MCC_status[i] = AMS_block_body[5+i];
      if(MCC_status[i] > MCC_status_limits[1]){ 
        MCC_status_status[i] = ALARM;
      } else if(MCC_status[i] > MCC_status_limits[0]){ 
        MCC_status_status[i] = WARNING;
      } else {
        MCC_status_status[i] = NORMAL;
      }
      if(i==3) {MCC_status_status[i] = NORMAL;}
    }
  }
}

//------------------------------------------------------------------------------

void display_MCC_right_panel( short row, short col) {

  int i;
  
  set_colors( headline); 
  
  for (i=row; i<23; i++) display_line( i, col, 80, " ");

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
  
  ++row;
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext("Abrvtns:");
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("");  
  set_colors( headline); 
  _outtext(" ");
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("");  
  set_colors( headline); 
  _outtext(" ");
  
  ++row;
  ++row;
  ++row;
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext(" ");
  
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext(" ");
  
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext(" ");
}

//------------------------------------------------------------------------------

void display_MCC_data_frame( void) {     
  short row, col;
  int i,j;

  row = 2;
  col = 1;
  
  set_colors( frame);
  
  for(i=0; i<NLINES; i++){
    _settextposition( row+i,col);                                 
    _outtext( MCC_lines[i]);
  }
}

//------------------------------------------------------------------------------

void display_MCC_data( void) {
  int i, j, k, m;
  time_t now;
  int dd,hh,mm,ss;
  short row,col;

  if (page_frozen) return;
  
  now = time( NULL);
  /*
   * display status data, massage alive time.
   */

  data_are_absent = (MCC_status_time)                                   ? FALSE : TRUE;
  data_are_old    = (now - MCC_status_time > MCC_status_time_staleness) ? TRUE : FALSE;
  dd = MCC_alivetime/(24*60*60);
  hh = ((MCC_alivetime - dd*(24*60*60))/(60*60))%24;
  mm = ((MCC_alivetime - dd*(24*60*60) - hh*60*60)/(60))%60;
  ss =   MCC_alivetime%60;                                    
  sprintf( b, " %2d/%2.2d:%2.2d:%2.2d ", dd, hh, mm, ss);
  _settextposition( 3, 15);
  set_value_colors( MCC_alivetime_status);
  _outtext( b);

  for(i=0;i<7;i++){
    if(MCC_status[i]){sprintf( b, "%6u ",MCC_status[i]);}
    else {            sprintf( b, "     - ");}
    _settextposition(6+i, 21);
    set_value_colors(MCC_status_status[i]);
    _outtext( b);
  }

  /* 
   * display delays 
   */
  data_are_absent = (MCC_delay_time)                                  ? FALSE : TRUE;
  data_are_old    = (now - MCC_delay_time > MCC_delay_time_staleness) ? TRUE : FALSE;
  set_value_colors( MCC_delay_status);
  for (i=0; i<2; i++) {
    sprintf( b, " 0x%4.4x ", MCC_delay[i]);
    _settextposition( 15+i, 14);
    _outtext( b);
    sprintf( b, "%7.4f ", MCC_delay[i]/2048.);
    _settextposition( 15+i, 27);
    _outtext( b);
  }

  /*
   * display expand values
   */
  set_colors( frame);
  _settextposition( 5, 45);
  _outtext( "Delayed");
  data_are_absent = (MCC_expand_time)                                   ? FALSE : TRUE;
  data_are_old    = (now - MCC_expand_time > MCC_expand_time_staleness) ? TRUE : FALSE;
  set_value_colors( MCC_expand_status);
  for (i=0; i<15; i++) {
    sprintf( b, "%6d ", MCC_expand[i]);
    _settextposition( 6+i, 45);
    _outtext( b);
  }

  /*
   * display list values
   */
  data_are_absent = (MCC_list_time)                                 ? FALSE : TRUE;
  data_are_old    = (now - MCC_list_time > MCC_list_time_staleness) ? TRUE : FALSE;
  set_value_colors( MCC_list_status);
  sprintf( b, "%4d ", MCC_list_total);
  _settextposition( 3, 53);
  _outtext( b);
  for (i=0; i<16; i++) {
    if (MCC_list_subt[i]) {sprintf( b, "%4d ", MCC_list_subt[i]);}
    else {                 sprintf( b, "   - ");}
    _settextposition( 5+i, 53);
    _outtext( b);
  }


}

//------------------------------------------------------------------------------

void display_MCC_page( void) {
  short row, col;
  display_headline( "MCC STATUS PAGE");
  if (new_page) {
    display_MCC_data_frame();
    display_MCC_right_panel( row=2, col=62);
  }
  display_MCC_data();
}
 
//------------------------------------------------------------------------------
