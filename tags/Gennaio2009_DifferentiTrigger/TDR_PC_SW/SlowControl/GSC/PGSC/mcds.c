// this is file mcds.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for MCDS mirror display
//

#include "pgsc.h"               // it has some global data definitions

struct MCDS MCDS_prev;

#define NLINES 21
char *lines[NLINES][2] = {
/*    123456789012345678   1234567890123456789012345678901234567890 */
     " AMS COUNTER      ","(should spin at 10Hz when AMS has power )",
     "                  ","(Power Step reflects what is powered    )",
     " POWER STEP     1 ","(CMD+H/K+Heaters, use about  22W+Heating)",
     "                2 ","(ditto+logging+Ku-Tx, about 100W+Heating)",
     "                3 ","(full function,       about 700W+Heating)",
     " HEATERS STATUS   ","                                         ",
     "     ENBL       4 ","(Heater power connected at input        )",
     "     DSBL       5 ","(Heater power blocked at input          )",
     " TEMPERATURES     ","                                         ",
     "    PRE UNDER     ","(at least 1 temp sensor too cool        )",
     "    PRE OVER      ","(at least 1 temp sensor too warm        )",
     "    UNDER         ","(at least 1 temp sensor too cold        )",
     "    OVER          ","(at least 1 temp sensor too hot         )",
     "                  ","                                         ",
     " RT/MCC           ","(4 prim, 28 alt, an OIU cmd switches    )",
     "                  ","                                         ",
     " RESET          6 ","(Soft reset of all internal processors  )",
     "                  ","                                         ",
     " O/B CMD COUNTER  ","(count of \"onboard\" commands received   )",
     "                  ","                                         ",
     " GND CMD COUNTER  ","(count of \"ground\" commands received    )"};
     
     
//------------------------------------------------------------------------------

void initialize_MCDS_page( void) {
  static short first = TRUE;
  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void simulate_MCDS_data( void) {
}

//------------------------------------------------------------------------------

void process_MCDS_data( void) {

  static short first = TRUE;

  MCDS_time = time( NULL);
  
  MCDS_prev = MCDS_data;

  MCDS_data.health_discreets    = (PDI.frame[0] >> 8) & 0x00FF;
  MCDS_data.GPC_command_count   = (PDI.frame[0]     ) & 0x00FF;
  MCDS_data.total_command_count = (PDI.frame[1] >> 8) & 0x00FF;
  MCDS_data.heartbeat           = (PDI.frame[2] >> 8) & 0x00FF;

  if (first) {
    first = FALSE;
    MCDS_prev.GPC_command_count   = MCDS_data.GPC_command_count;
    MCDS_prev.total_command_count = MCDS_data.total_command_count; 
    MCDS_prev.heartbeat           = MCDS_data.heartbeat;
  }

  MCDS_data.heaters_state         = (MCDS_data.health_discreets>> 0) & 1;
  MCDS_data.power_step            = (MCDS_data.health_discreets>> 1) & 3;
  MCDS_data.RT_MCC                = (MCDS_data.health_discreets>> 3) & 1;
  MCDS_data.pre_under_temperature = (MCDS_data.health_discreets>> 4) & 1;
  MCDS_data.pre_over_temperature  = (MCDS_data.health_discreets>> 5) & 1;
  MCDS_data.under_temperature     = (MCDS_data.health_discreets>> 6) & 1;
  MCDS_data.over_temperature      = (MCDS_data.health_discreets>> 7) & 1;

  MCDS_status.health_discreets      = NORMAL;
  MCDS_status.GPC_command_count     = NORMAL;
  MCDS_status.total_command_count   = NORMAL;
  MCDS_status.heartbeat             = NORMAL;
  MCDS_status.heaters_state         = NORMAL;
  MCDS_status.power_step            = NORMAL;
  MCDS_status.RT_MCC                = NORMAL;
  MCDS_status.pre_under_temperature = NORMAL;
  MCDS_status.pre_over_temperature  = NORMAL;
  MCDS_status.under_temperature     = NORMAL;
  MCDS_status.over_temperature      = NORMAL;
  
  if (MCDS_data.heaters_state == 0) MCDS_status.heaters_state = WARNING;

  if (MCDS_data.power_step != 3) MCDS_status.power_step = WARNING;

  if (error_in_1553_data) MCDS_status.RT_MCC = ALARM;

  if (MCDS_data.pre_under_temperature) MCDS_status.pre_under_temperature = WARNING;

  if (MCDS_data.pre_over_temperature) MCDS_status.pre_over_temperature = WARNING;

  if (MCDS_data.under_temperature) MCDS_status.under_temperature = ALARM;

  if (MCDS_data.over_temperature) MCDS_status.over_temperature = ALARM;

  if ((MCDS_prev.GPC_command_count !=  MCDS_data.GPC_command_count) &&
     ((MCDS_prev.GPC_command_count+1)%255 != MCDS_data.GPC_command_count)) {
    MCDS_status.GPC_command_count = ALARM;
  }

  if ((MCDS_prev.total_command_count !=  MCDS_data.total_command_count) &&
     ((MCDS_prev.total_command_count+1)%255 != MCDS_data.total_command_count)) {
    MCDS_status.total_command_count = ALARM;
  }
 
  if ((MCDS_prev.heartbeat+1) % 100 != MCDS_data.heartbeat) {
    MCDS_status.heartbeat = ALARM;
  }

}

//------------------------------------------------------------------------------

void display_MCDS_right_panel( short row, short col) {

  int i;
  
  set_colors( headline); 
  
  for (i=row; i<20; i++) display_line(i, col, 80, " ");

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
  
}

//------------------------------------------------------------------------------
    
void display_MCDS_data_frame( void) {
  short row, col0, col1;
  int i;

  row = 2;
  col0 = 1;
  col1 = 23;
  
  set_colors( frame);
  
  for(i=0; i<NLINES; i++){
    _settextposition( row+i,col0);                                 
    sprintf( b, "%s    ", lines[i][0]);
    _outtext( b);
    _settextposition( row+i,col1);                                 
    sprintf( b, " %s", lines[i][1]);
    _outtext( b);
  }

}

//------------------------------------------------------------------------------

void display_MCDS_data( void) {

  short row, col;
  time_t now;
  char stale;

  if (page_frozen) return;
  now = time( NULL);

  if (MCDS_time) data_are_absent = FALSE;
  else           data_are_absent = TRUE;
  if ((MCDS_status.RT_MCC == ALARM) || (now - MCDS_time > MCDS_time_staleness)) {
    data_are_old = TRUE;
  }
  else {
    data_are_old = FALSE;
  }

  row = 2;
  col = 19;

  sprintf( b,"%3d ",MCDS_data.heartbeat);
  _settextposition( row++, col);
  set_value_colors( MCDS_status.heartbeat);
  _outtext( b);

  row++;

  if(MCDS_data.power_step == 1){
    sprintf( b," *  ");
  }
  else {
    sprintf( b,"    ");
  }    
  _settextposition( row++, col);
  set_value_colors( MCDS_status.power_step);
  _outtext( b);

  if(MCDS_data.power_step == 2){
    sprintf( b," *  ");
  }
  else {
    sprintf( b,"    ");
  }    
  _settextposition( row++, col);
  set_value_colors( MCDS_status.power_step);
  _outtext( b);

  if(MCDS_data.power_step == 3){
    sprintf( b," *  ");
  }
  else {
    sprintf( b,"    ");
  }    
  _settextposition( row++, col);
  set_value_colors( MCDS_status.power_step);
  _outtext( b);

  row++;

  if(MCDS_data.heaters_state){
    sprintf( b," *  ");
  }
  else {
    sprintf( b,"    ");
  }    
  _settextposition( row++, col);
  set_value_colors( MCDS_status.heaters_state);
  _outtext( b);

  if(!MCDS_data.heaters_state){
    sprintf( b," *  ");
  }
  else {
    sprintf( b,"    ");
  }    
  _settextposition( row++, col);
  set_value_colors( MCDS_status.heaters_state);
  _outtext( b);

  row++;

  stale = (data_are_old) ? 'S' : ' ';

  if(MCDS_data.pre_under_temperature){
    sprintf( b," *%c ",stale);
  }
  else {
    sprintf( b,"  %c ",stale);
  }
  _settextposition( row++, col);
  set_value_colors( MCDS_status.pre_under_temperature);
  _outtext( b);

  if(MCDS_data.pre_over_temperature){
    sprintf( b," *%c ",stale);
  }
  else {
    sprintf( b,"  %c ",stale);
  }
  _settextposition( row++, col);
  set_value_colors( MCDS_status.pre_over_temperature);
  _outtext( b);

  if(MCDS_data.under_temperature){
    sprintf( b," *%c ",stale);
  }
  else {
    sprintf( b,"  %c ",stale);
  }
  _settextposition( row++, col);
  set_value_colors( MCDS_status.under_temperature);
  _outtext( b);

  if(MCDS_data.over_temperature){
    sprintf( b," *%c ",stale);
  }
  else {
    sprintf( b,"  %c ",stale);
  }
  _settextposition( row++, col);
  set_value_colors( MCDS_status.over_temperature);
  _outtext( b);

  row++;

  if(MCDS_data.RT_MCC){
    sprintf( b," 28 ");
  }
  else {
    sprintf( b,"  4 ");
  }
  _settextposition( row++, col);
  set_value_colors( MCDS_status.RT_MCC );
  _outtext( b);

  row++;
  row++;
  row++;

  sprintf( b,"%3d ",MCDS_data.GPC_command_count);
  _settextposition( row++, col);
  set_value_colors( MCDS_status.GPC_command_count);
  _outtext( b);

  row++;

  sprintf( b,"%3d ",MCDS_data.total_command_count);
  _settextposition( row++, col);
  set_value_colors( MCDS_status.total_command_count);
  _outtext( b);

  _settextposition( cursor_home.row, cursor_home.col);// leave cursor to blink here
  
}

//------------------------------------------------------------------------------

void display_MCDS_page( void) {
  short row, col;
  display_headline( "MCDS MIRROR PAGE");
  if (new_page) {
    display_MCDS_data_frame();
    display_MCDS_right_panel( row=2, col=65);
  }
//display_MCDS_data();
}
 
//------------------------------------------------------------------------------
