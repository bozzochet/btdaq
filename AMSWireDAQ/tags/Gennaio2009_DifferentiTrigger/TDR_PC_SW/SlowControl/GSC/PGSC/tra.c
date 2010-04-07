// this is file tra.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for TRAcker data
//

#include "pgsc.h"

static struct {
  char  *name;
  short row;      /* it is relative row number wrt TRA[].row */
  short col;      /* it is relative col number wrt TRA[].col */
} param[12] = {
   {"3", 2,0},
   {"2", 3,0},
   {"4", 4,0},
   {"2", 5,0},
   {"e", 6,0},
   {"E", 7,0},
   {"a", 8,0},
   {"a", 9,0},
   {"a",10,0},
   {"a",11,0},
   {"a",12,0},
   {"F",13,0} };  

struct {
  char  *name;
  short row;
  short col;
  long  time;                
  struct {
    short value1;
    short value2;
    short status;
    char  *state;
    short temperature[2];
  } param[12];
} TRA[2] =                        {
   { "Tracker Crate 32",  2,  1}, 
   { "Tracker Crate 72",  2, 31}
                                  };  

/* end of global data section */


void simulate_TRA_block( void) {

  int ii, j, k, block_index;
  int i, temp;
  
  ii = rand() % 2;                          /* TRA node number */
  if (ii == 0) {
    ii = 0x2;
  }
  else {
    ii = 0x5;
  }
//AMS_block_ID = block_type<<13 | node_type<<9 | node_number<<6 | data_type;
  AMS_block_ID =        0x4<<13 |       0xB<<9 |          ii<<6 |         0;
  block_index = 0;

/*********************************************/
/*                                           */
/*          simulate TRA state data          */
/*                                           */
/*********************************************/

  for (j=0; j<12; j++) {
    AMS_block_body[block_index++] = (rand() % 2) ?  0 :  1;    // some status
    AMS_block_body[block_index++] = (rand() % 3) ? 10 : 20;    // some parameter
    AMS_block_body[block_index++] = (rand() % 4) ? 15 : 25;    // some parameter
  }

/*********************************************/
/*                                           */
/*           simulate temperature data       */
/*                                           */
/*********************************************/

  for (j=0; j<12; j++) {
    for (k=0; k<2; k++) {
      AMS_block_body[block_index++] = 0x7f + (rand() % 0x7f); // temperature
    }
  }

  AMS_block_length = block_index + 1;
  
} /* end of simulate_TRA_block() */


void process_TRA_block( void) {
  int ii, j, k, block_index;
  int i, temp;
                          //  0   1   2   3   4   5   6   7 yet another lookup Table
  static short TRA_ID[8] = { -1, -1, 32, -1, -1, 72, -1, -1};   

/***************************************************/
/*                                                 */
/*  TRA block structure checks and initialization  */
/*                                                 */
/***************************************************/

return;

  if ((AMS_block_ID & 0xFE00) != 0x9600) {
    printf("\n it is not a TRA block: %4.4x \n",AMS_block_ID);
    exit(0);
  }
  ii = (AMS_block_ID >> 6) & 0x7;
  if (TRA_ID[ii] < 0) {
    printf("\n wrong TRA number found: %4.4x \n",AMS_block_ID);
    exit(0);
  }
  if (TRA_ID[ii] == 32) ii = 0;     // yet another transcoding
  if (TRA_ID[ii] == 72) ii = 1;     // yet another transcoding  

                           /* this is for simulation ONLY*/
                           if (time(NULL)%60<15 && ii==3) return;
  TRA[ii].time = time(NULL); 
  block_index = 0;
  
/*********************************************/
/*                                           */
/*          process TRA state data           */
/*                                           */
/*********************************************/
  
/* unpack TRA data block */

  for (j=0; j<12; j++) {
    TRA[ii].param[j].state  = AMS_block_body[block_index++] ? "OFF" : "ON";
    TRA[ii].param[j].value1 = AMS_block_body[block_index++];
    TRA[ii].param[j].value2 = AMS_block_body[block_index++];
  }

/* check limits, set warnings or alarms */

  for (j=0; j<12; j++) {
    TRA[ii].param[j].status = NORMAL;
    if ((rand() % 10) == 0)  TRA[ii].param[j].status = WARNING;
    if ((rand() % 100) == 0) TRA[ii].param[j].status = ALARM;
  }

/*********************************************/
/*                                           */
/*           process temperature data        */
/*                                           */
/*********************************************/

/* unpack TRA data block, convert to degrees */
  
  for (j=0; j<12; j++) {
    for ( k=0; k<2; k++) {
      TRA[ii].param[j].temperature[k] = 
      (AMS_block_body[block_index++] - 0x7f) / 2;
    }
  }
 
/***************************************************/
/*                                                 */
/*        final TRA block structure checks         */
/*                                                 */
/***************************************************/

  if (block_index != AMS_block_length-1) {
    printf("\n TRA block_index error: %d %d",block_index,AMS_block_length);
    exit(0);
  } 
    
} /* end of process_TRA_block() */

void display_TRA_right_panel( short row, short col) {

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
  
} /* end of display_TRA_right_panel( short row, short col) */

    
void display_TRA_data_frame( void) {     

  int i, j, k;
  static short TRA_node_nb[2] = { 2, 5};  // yet another lookup Table 

  set_colors( frame);
  
  for (i=0; i<2; i++) {
    _settextposition( TRA[i].row, TRA[i].col);                                 
    sprintf(b,"<- %6s(%1d) ->\263",TRA[i].name,TRA_node_nb[i]); _outtext(b);
    _settextposition( TRA[i].row+1, TRA[i].col);
    sprintf(b,"# St XY  zZ        t1 t2 \263");
        //     <- Tracker Crate 32(2) ->|
    _outtext(b);
  }
 
  for ( i=0; i<2; i++) {
    for ( j=0; j<12; j++) {
    _settextposition( TRA[i].row+param[j].row, TRA[i].col+param[j].col);
    sprintf(b,"%s", param[j].name);
    _outtext(b);
    _settextposition( TRA[i].row+param[j].row, TRA[i].col+param[j].col+19);
    _outtext("\263");
    }
  }

} /* end of display_TRA_data_frame( row) */


void display_TRA_data( void) {

  int i, j, k, data_may_be_wrong;
  
  if (page_frozen) return;
    
  data_may_be_wrong = 0;
  
  for ( i=0; i<2; i++) {
    data_are_old = (time(NULL) - TRA[i].time > 10/*sec*/) ? TRUE : FALSE;
    if (data_are_old) data_may_be_wrong++;
    if (TRA[i].time) {
      for ( j=0; j<12; j++) {
        sprintf(b,"%3.3s%3d %3d %+2.2d%+2.2d ",
        TRA[i].param[j].state,
        TRA[i].param[j].value1,
        TRA[i].param[j].value2,
        TRA[i].param[j].temperature[0],
        TRA[i].param[j].temperature[1]);
        set_value_colors( TRA[i].param[j].status);
        _settextposition( TRA[i].row+param[j].row, TRA[i].col+param[j].col+1);
        _outtext( b);
      }
    }
  }
} /* end of display_TRA_data() */

void display_TRA_page( void) {
  short row, col;
  display_headline( "* AMS 1553 BUS MONITOR *  TRACKER PAGE");
  if (new_page) {
    display_TRA_data_frame();
    display_TRA_right_panel( row=2, col=61);
  }
  display_TRA_data();
} /* end of display_TRA_page() */


