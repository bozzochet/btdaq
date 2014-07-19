// this is file pgsc.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - various common functions
//

#include "pgsc.h"

int frame_sync = FALSE;

#define IBITS( code, from, to) \
( (integer16)(code << (15-to)) >> (15-to+from))

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//                LOW-LEVEL display control functions                *
//                                                                   *
//                                                                   *
//********************************************************************


//------------------------------------------------------------------------------

void clear_display( void) {
  long black=0;
  _setbkcolor( black);
  _clearscreen( _GCLEARSCREEN);
}

//------------------------------------------------------------------------------

void set_colors( struct color c) {
  _setbkcolor( c.bkcolor);
  _settextcolor( c.txcolor);
}

//------------------------------------------------------------------------------

void set_value_colors( short status) {
  if (data_are_absent || status == NO_DATA) {
    set_colors( no_data);
    return;
  }
  if (data_are_old || status == OLD_DATA) {
    set_colors( old_data);
    return;
  }
  else {
    switch (status) {
      case NORMAL:
        set_colors( normal);
        break;
      case WARNING:
        set_colors( warning);
        break;
      case ALARM:
        set_colors( alarm);
        break;
      default:
        break;
    }
  }
}

//------------------------------------------------------------------------------

void display_line( short row, short col_from, short col_to, char *text) {

  int i;
  struct rccoord c;

  _settextposition( row, col_from);
  _outtext( text);
  c = _gettextposition();
  if (c.col == 1) c.col = 81;
  for ( i=c.col; i<=col_to; i++) _outtext(" ");
  _settextposition( c.row, c.col);
}

//------------------------------------------------------------------------------

void display_message( short r, short c1, short c2, struct color c, char *t) {
  struct rccoord pos;
  pos = _gettextposition();
  set_colors( c);
  display_line( r, c1, c2, t);
  _settextposition( pos.row, pos.col);
}

//------------------------------------------------------------------------------
         
void setup_display( void) {

  bit_rate = -1.0;
  frame_rate = -1.0;
  
  stop_now = FALSE;
  page_frozen = FALSE; 
  new_page = TRUE;
  command = ' ';
  sprintf(main_page_prompt,"EXit/Detailed pages/Summary pages/Test pages");
  sprintf(page_message," ");
  sprintf(page_prompt,"%s",main_page_prompt);
    
  page_level = MAIN_page;
  page_to_display = 0;

  _setvideomode( _DEFAULTMODE);
  
  headline.bkcolor =  3;
  headline.txcolor =  0;
  
  frame.bkcolor =  1;
  frame.txcolor = 14;
  
  bottomline.bkcolor = 3;
  bottomline.txcolor = 0;

  normal.bkcolor = 2;
  normal.txcolor = 0;

  old_data.txcolor = 7;
  old_data.bkcolor = 0;

  warning.txcolor = 0;
  warning.bkcolor = 6;

  alarm.txcolor = 15;
  alarm.bkcolor =  4;

  no_data.txcolor = 0;
  no_data.bkcolor = 0;
    
}

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//                 GENERAL display control functions                 *
//                                                                   *
//                                                                   *
//********************************************************************


//------------------------------------------------------------------------------

void display_headline( char *text) {
  char *t;     
  set_colors( headline);
  if (I_am_in_BM_mode || I_am_DEC_ALPHA || SIMULATION) {
    sprintf( b, "* AMS 1553 BUS MONITOR * %s", text);
  }
  else {
    sprintf( b, "* AMS 1553 BUS MON-CTL * %s", text);
  }
  display_line( 1, 1, 80, b);
  t = mytime();
  display_line( 1, 80-23, 80, t);     
}

//------------------------------------------------------------------------------

void display_page_body( void) {

  initialize_PSB_page();
  initialize_HTC_page();
  initialize_CAN_page();
  initialize_CNT_page();
  initialize_MON_page();
  initialize_MCC_page();

  switch (page_level) {
  case MAIN_page:
    display_main_page();
    break;
  case DETAILED_pages:
    switch (page_to_display) {
    case PSB_page:
      display_PSB_page();
      break;
    case HTC_page:
      display_HTC_page();
      break;
    case CNT_1_page:
      display_CNT_page( 1);
      break;
    case CNT_2_page:
      display_CNT_page( 2);
      break;
    case CNT_3_page:
      display_CNT_page( 3);
      break;
    case MON_page:
      display_MON_page();
      break;
    case MCC_page:
      display_MCC_page();
      break;
    case DAQ_1_page:
      display_DAQ_page( 1);
      break;
    case DAQ_2_page:
      display_DAQ_page( 2);
      break;
    case DAQ_3_page:
      display_DAQ_page( 3);
      break;
    case DAQ_4_page:
      display_DAQ_page( 4);
      break;
    case DAQ_5_page:
      display_DAQ_page( 5);
      break;
    case DAQ_6_page:
      display_DAQ_page( 6);
      break;
    case DAQ_7_page:
      display_DAQ_page( 7);
      break;
    case DAQ_8_page:
      display_DAQ_page( 8);
      break;
    case CAN_page:
      display_CAN_page();
      break;
    default:
      printf(" wrong page_to_display = %d\n",page_to_display);
      break;
    }
    break;
  case TEST_pages:
    switch (page_to_display) {
    case frame_number_page:
      display_frame_number_page();
      break;
    case time_broadcast_page:
      display_time_broadcast_page();
      break;
    case PSP_frame_page:
      display_PSP_frame_page();
      break;
    case PDI_frame_page:
      display_PDI_frame_page();
      break;
    case AMS_block_page:
      display_AMS_block_page();
      break;
    default:
      printf(" wrong page_to_display = %d\n",page_to_display);
      break;
    }
    break;
  case SUMMARY_pages:
    switch (page_to_display) {
    case MCDS_page:
      display_MCDS_page();
      break;
    case log_page:
      display_log_page();
      break;
    case EHTS_page:
      display_EHTS_page();
      break;
    case IHTS_page:
      display_IHTS_page();
      break;
    case TPLT_page:
      display_TPLT_page();
      break;
    default:
      printf(" wrong page_to_display = %d\n",page_to_display);
      break;
    }
    break;
  case DIALOG_page:
    display_dialog_page();
    break;
  default:
    printf(" wrong page_level = %d\n",page_level);
    break;
  }
}

//------------------------------------------------------------------------------

void display_bottomline( void) {

  struct color c;
     
  set_colors( bottomline);
  display_line( 23, 1, 80, page_message);
  cursor_home.row = 24;
  if (page_message[0] != ' ') sprintf( page_message, " ");
  if (page_level == DIALOG_page) {
    sprintf( b, "%s", page_prompt);
  }
  else {
    sprintf( b, "%s <= %c", page_prompt, command);
  }
  cursor_home.col = strlen(b);
  cursor_home.col = cursor_home.col ? cursor_home.col : 1;
  display_line( cursor_home.row, 1, 80, b);
  if (wrong_command) {
    display_line( cursor_home.row, cursor_home.col+2, 80, "<<- INVALID COMMAND");
  }

  display_message( 25, error_in_1553_col, 79, old_data, " ");
                         
  if (SIMULATION) {
    if (I_am_DEC_ALPHA) {
      sprintf( b, "file%6.6ld", frame_file_number);
      display_message( 25, 64, 73, warning, b);
    }
    else {
      display_message( 25, 64, 73, warning, "SIMULATION");
    }
  }
  else {
    display_message( 25, 64, 72, old_data, "1553 DATA");
  }
  
  if (frame_rate >= 0.0) {
    sprintf( b, "%3d/s", (int)frame_rate);
    if (frame_rate < 0.5) c = alarm;
    else                  c = old_data;
    display_message( 25, 75, 79, c, b);
  }
  if (bit_rate >= 0.0) {
    sprintf( b, "%3d/s", (int)bit_rate);
    if (bit_rate < 5.0) c = alarm;
    else                c = old_data;
    display_message( 25, 75, 79, c, b);
  }

  if (page_level == DIALOG_page) {
    cursor_home.row = dialog_cursor.row;
    cursor_home.col = dialog_cursor.col;
  }
  _settextposition( cursor_home.row, cursor_home.col);     // leave cursor to blink here
     
  if (stop_now) exit( 0);
                              
//   1553 PRS CRC DFR BLK SIMULATION 204/s
//01234567890123456789012345678901234567890
//4         5         6         7         8
}

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//         AMS HEALTH AND STATUS (ao) display functions              *
//                                                                   *
//                                                                   *
//********************************************************************

//------------------------------------------------------------------------------

void display_AMS_health_and_status() {

  struct color color;
  char c, s[5];
  int i, RT;
  
  if (error_in_1553_data) {
    frame_sync = FALSE;
    display_message( 25, 1, 42, old_data, " ");
    return;
  }
  
  if (MCDS_data.heaters_state) c = 'E';
  else                         c = 'D';
  sprintf( b, "AMS=%2.2X P=%d H=%c", 
           MCDS_data.heartbeat, MCDS_data.power_step, c);
  display_message( 25, 1, 15, old_data, b);

  for (i=0; i<4; i++) s[i] = '-';
  s[4] = '\0';
  if (MCDS_data.pre_under_temperature) s[0] = 'u';
  if (MCDS_data.pre_over_temperature)  s[1] = 'o';
  if (MCDS_data.under_temperature)     s[2] = 'U';
  if (MCDS_data.over_temperature)      s[3] = 'O';
  sprintf( b, "T=%s", s);
  color = old_data;
  if (MCDS_data.pre_under_temperature) color = warning;
  if (MCDS_data.pre_over_temperature ) color = warning;
  if (MCDS_data.under_temperature    ) color = alarm;
  if (MCDS_data.over_temperature     ) color = alarm;

  display_message( 25, 16, 21, color, b);
  
  display_message( 25, 22, 22, old_data, " ");
  
  if (MCDS_data.RT_MCC) RT = 28;
  else                  RT = 4;
  
  sprintf( b, "RT=%-2d O/B=%2.2X GND=%2.2X ", 
           RT, MCDS_data.GPC_command_count, MCDS_data.total_command_count);
  display_message( 25, 23, 42, old_data, b);

//                 "AMS=XX P=3 H=E T=---- RT=28 O/B=XX GND=XX ");
//                 "123456789012345678901234567890123456789012");
//                 "         1         2         3         4   ");
}

//------------------------------------------------------------------------------

void display_log_page( void){
  short row;
  char *p;
  display_headline( "ERROR & LOG PAGE");
  if (new_page) {
    for (row=2; row<23; row++) display_line( row, 1, 80, " ");
  }
  if (page_frozen) return;
  if (open_log_pipe_for_reading() == FAIL) {
    display_message( 2, 20, 80, headline,
                    "No log or error messages so far...");
    return;
  }
  row = 2;
  while (1) {
    p = read_log_pipe();
    if (p == NULL) break;
    display_message( row++, 1, 80, headline, p);
  }
}

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//                    TEST PAGE display functions                    *
//                                                                   *
//                                                                   *
//********************************************************************


//------------------------------------------------------------------------------

void display_frame_number_page( void) {
  short row;
  display_headline( "OIU FRAME NUMBER PAGE");
  if (new_page) {
    for (row=2; row<23; row++) display_line( row, 1, 80, " ");
  }
  if (page_frozen) return;
  sprintf( b, "OIU Frame Number = %2d(10) = %2.2x(16)",
           OIU_frame_number,OIU_frame_number);
  _settextposition( 10, 20);
  _outtext( b);
  sprintf(b, "OIU Frame Number block status = %4.4x(16)",
              OIU_frame_number_block_status);
  _settextposition( 11, 20);
  _outtext( b);
}

//------------------------------------------------------------------------------

void display_time_broadcast_page( void) {
  short i, row;
  int p;
  display_headline( "TIME BROADCAST PAGE");
  if (new_page) {
    for (row=2; row<23; row++) display_line( row, 1, 80, " ");
  }
  if (page_frozen) return;
  p = sprintf(b,"Time Broadcast = ");
  for (i=0;i<TIME_BROADCAST_LENGTH;i++) {
    p += sprintf(b+p,"%4.4x ", time_broadcast[i]);
  }
  p = sprintf(b+p,"(16)");
  _settextposition( 10, 10);
  _outtext( b);
  p = sprintf(b,"Time Broadcast block status = ");
  p += sprintf(b+p,"%4.4x (16)", time_broadcast_block_status);
  _settextposition( 11, 10);
  _outtext( b);
}

//------------------------------------------------------------------------------

void display_PSP_frame_page( void) {
  short i, row, col;
  int p;
  display_headline( "PSP FRAME PAGE");
  if (new_page) {
    for (row=2; row<23; row++) display_line( row, 1, 80, " ");
  }
  if (page_frozen) return;
  for (row=2; row<23; row++) display_line( row, 1, 80, " ");
  sprintf(b,"PSP Frame (hexadecimal):");
//_settextposition( row=rand()%5+5, col=rand()%10+20); /* nice joke, isn't */
  _settextposition( row=5, col=25);
  _outtext(b);
  sprintf(b,"      status words = %4.4x %4.4x",
    PSP.status_word[0],PSP.status_word[1]);
  _settextposition( ++row, col-6);
  _outtext(b);
  sprintf(b,"    block stati = %4.4x %4.4x",
    PSP.block_status[0],PSP.block_status[1]);
  _settextposition( ++row, col-6);
  _outtext(b);
  p = 0;
  for (i=0;i<PSP_FRAME_LENGTH;i++) {
    p += sprintf(b+p," %4.4x", PSP.frame[i]);
    if (i % 8 == 7) {
      _settextposition( ++row, col-10);
      _outtext(b);
      p = 0;
    }
  }
}

//------------------------------------------------------------------------------

void display_PDI_frame_page( void) {
  short i, row, col;
  int p;
  display_headline( "PDI FRAME PAGE");
  if (new_page) {
    for (row=2; row<23; row++) display_line(row,1,80," ");
  }
  if (page_frozen) return;
  for (row=2; row<23; row++) display_line(row,1,80," ");
  sprintf(b,"PDI Frame (hexadecimal):");
  _settextposition( row=rand()%5+5, col=rand()%10+20); // nice joke, isn't it?
  _outtext(b);
  sprintf(b,"      status words = %4.4x %4.4x",
    PDI.status_word[0],PDI.status_word[1]);
  _settextposition( ++row, col-6);
  _outtext(b);
  sprintf(b,"    block stati = %4.4x %4.4x",
    PDI.block_status[0],PDI.block_status[1]);
  _settextposition( ++row, col-6);
  _outtext(b);
  p = 0;
  for (i=0; i<PDI_FRAME_LENGTH; i++) {
    p += sprintf(b+p," %4.4x", PDI.frame[i]);
    if (i % 8 == 7) {
      _settextposition( ++row, col-10);
      _outtext(b);
      p = 0;
    }
  }
  sprintf( b, "%2.2X %2.2X %2.2X %2.2X ",
           MCDS_data.health_discreets,
           MCDS_data.GPC_command_count,
           MCDS_data.total_command_count,
           MCDS_data.heartbeat);
  _settextposition( row+=2, col+5);
  _outtext(b);
}

//------------------------------------------------------------------------------

// what follows is NOT my best program = = = = = = = = => > >
 
void display_AMS_block_page( void) {

  unsigned i; 
  short row, col;
  int p;
  
  display_headline( "AMS BLOCK PAGE");
  if (new_page) {
    for (row=2; row<23; row++) display_line( row, 1, 80, " ");
  }
  
  if (page_frozen) return;
  
  for (row=2; row<23; row++) display_line( row, 1, 80, " ");
  
  p = sprintf( b, "AMS_block_length = %d(10) 16-bit words", bl_length);
  if (bl_length >= 4) p += sprintf( b+p, " (%d + %d)", 4, bl_length-4);
  _settextposition( row=3, 1);
  _outtext( b);
  
  if (bl_length == 0) return;
  
  block_type  = (bl_ID >> 13) & 0x07;
  node_type   = (bl_ID >>  9) & 0x0F;
  node_number = (bl_ID >>  6) & 0x07;
  data_type   =  bl_ID        & 0x3F;
  
  p  = sprintf( b,   "AMS_block_ID     = %4.4X(16) = ",  bl_ID);
  p += sprintf( b+p, "%d", IBITS( bl_ID, 15, 15));
  p += sprintf( b+p, "%d", IBITS( bl_ID, 14, 14));
  p += sprintf( b+p, "%d", IBITS( bl_ID, 13, 13));
  p += sprintf( b+p, " ");
  if (block_type==GET_REPLY_block_type) p += sprintf( b+p, "(GET) ");
  if (block_type==SET_REPLY_block_type) p += sprintf( b+p, "(SET) ");
  if (block_type==EXE_REPLY_block_type) p += sprintf( b+p, "(EXE) ");
  if (block_type==HK_DATA_block_type)   p += sprintf( b+p, "(HK) ");
  p += sprintf( b+p, "%d", IBITS( bl_ID, 12, 12));
  p += sprintf( b+p, "%d", IBITS( bl_ID, 11, 11));
  p += sprintf( b+p, "%d", IBITS( bl_ID, 10, 10));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  9,  9));
  p += sprintf( b+p, " ");
  if (node_type==DAQ_node_type)  p += sprintf( b+p, "(DAQ) ");
  if (node_type==MCC_node_type)  p += sprintf( b+p, "(MCC) ");
  if (node_type==JL3_node_type)  p += sprintf( b+p, "(JL3) ");
  if (node_type==CNT_node_type)  p += sprintf( b+p, "(CNT) ");
  if (node_type==TRA_node_type)  p += sprintf( b+p, "(TRA) ");
  if (node_type==HTC_node_type)  p += sprintf( b+p, "(HTC) ");
  if (node_type==PSB_node_type)  p += sprintf( b+p, "(PSB) ");
  if (node_type==MON_node_type)  p += sprintf( b+p, "(MON) ");
  if (node_type==TASE_node_type) p += sprintf( b+p, "(TASE) ");

  p += sprintf( b+p, "%d", IBITS( bl_ID,  8,  8));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  7,  7));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  6,  6));
  p += sprintf( b+p, " ");
  p += sprintf( b+p, "%d", IBITS( bl_ID,  5,  5));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  4,  4));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  3,  3));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  2,  2));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  1,  1));
  p += sprintf( b+p, "%d", IBITS( bl_ID,  0,  0));
  p += sprintf( b+p, " (0x%2.2X)", data_type);
  _settextposition( ++row, 1);
  _outtext(b);
  
  sprintf( b, "HK_block_header  = %4.4X %4.4X %4.4X(16)\n",
           bl_body[0], bl_body[1], bl_body[2]);
  _settextposition( ++row, 1);
  _outtext( b);

  p = sprintf( b, "block_body(16): ");
  for (i=3; i<bl_length-1; i++) {
    p += sprintf( b+p, "%6.4X", bl_body[i]);
    if ((i-3) % 10 == 9) {
      _settextposition( ++row, 1);
      _outtext( b);
      p = sprintf( b, "block_body(16): ");
    }
  }
  if (p > 16) {
    _settextposition( ++row, 1);
    _outtext( b);
  }
  
  sprintf( b, "HK_block_header  = %u %u %u(10)\n",
           bl_body[0], bl_body[1], bl_body[2]);
  _settextposition( ++row, 1);
  _outtext( b);
  
  p = sprintf( b, "block_body(10): ");
  for (i=3; i<bl_length-1; i++) {
    p += sprintf( b+p, "%6u", bl_body[i]);
    if ((i-3) % 10 == 9) {
      _settextposition( ++row, 1);
      _outtext( b);
      p = sprintf( b, "block_body(10): ");
    }
  }
  if (p > 16) {
    _settextposition( ++row, 1);
    _outtext( b);
  }
}

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//                 GENERAL data processing functions                 *
//                                                                   *
//                                                                   *
//********************************************************************

//------------------------------------------------------------------------------

void process_frame_number( void) {
#define PRINT FALSE
  new_frame_number = FALSE;
  if (PRINT) print_frame_number();
#undef PRINT
}

//------------------------------------------------------------------------------

void process_time_broadcast( void) {
#define PRINT FALSE
  new_time_broadcast = FALSE;
  if (PRINT) print_time_broadcast();
#undef PRINT
}

//------------------------------------------------------------------------------

void process_PSP_frame( void) {
#define PRINT FALSE
  int i, j, k;
  new_PSP_frame = FALSE;
  if (PRINT) print_PSP_frame();
  PSP.time = time(NULL);
  k = 0;
  for (j=0;j<2;j++) {
    PSP.status_word[j] = status_word[j];
    PSP.block_status[j] = block_status[j];
    for (i=0;i<BOX_CAR_LENGTH;i++) {
      PSP.frame[k++] = HK.box_car[j][i];
    }
  }
#undef PRINT
}

//------------------------------------------------------------------------------

void process_PDI_frame( void) {
#define PRINT FALSE
  int i, j, k;
  short temp;

  new_PDI_frame = FALSE;

  if (PRINT) print_PDI_frame();
  PDI.time = time(NULL);
  
  k = 0;
  for (j=0; j<2; j++) {
    PDI.status_word[j] = status_word[j];
    PDI.block_status[j] = block_status[j];
    for (i=0; i<BOX_CAR_LENGTH; i++) {
      PDI.frame[k++] = HK.box_car[j][i];
    }
  }

  if (PRINT) print_HK_frame();

  RT_BUS = ((block_status[0] | block_status[1]) & 0x2000) ? 'B' : 'A';
  
  if (I_am_in_BM_mode) {
    if (((block_status[0] & 0x8100) == 0x8100) && 
        ((block_status[1] & 0x8100) == 0x8100)) {
      error_in_1553_data = FALSE;
      deframe_HK_frame();
    }
    else {
      error_in_1553_data = TRUE;
      display_message( 25, error_in_1553_col, error_in_1553_col+3, alarm, "1553");
      sprintf( b, "PDI frame: bad block status = %4.4X(16),%4.4X(16)",
               block_status[0], block_status[1]);
      write_log_pipe( b);
    }
  }
  
  if (I_am_in_BC_mode) {
    if (((block_status[0] & 0x8010) == 0x8010) && 
        ((block_status[1] & 0x8010) == 0x8010)) {
      error_in_1553_data = FALSE;
      deframe_HK_frame();
    }
    else {
      error_in_1553_data = TRUE;
      display_message( 25, error_in_1553_col, error_in_1553_col+3, alarm, "1553");
      sprintf( b, "PDI frame: bad block status = %4.4X(16),%4.4X(16)",
               block_status[0], block_status[1]);
      write_log_pipe( b);
    }
  }

  process_MCDS_data();
  display_AMS_health_and_status();
  
#undef PRINT
}

//------------------------------------------------------------------------------

void deframe_HK_frame( void) {
#define DEBUG FALSE

#define E(msg) \
  display_message( 25, error_in_DFR_col, error_in_DFR_col+2, alarm, "DFR"); \
  sprintf( b, "DEFRAMING ERROR: %s, ID=%2.2X(16)", msg, HK_frame_ID); \
  write_log_pipe( b); \
  if (DEBUG) \
  printf( "DEFRAMING ERROR: %s, ID=%2.2X(16)", msg, HK_frame_ID); \
  
#define W(msg) \
  display_message( 25, error_in_DFR_col, error_in_DFR_col+2, warning, "DFR"); \
//sprintf( b, "DEFRAMING WARNING: %s, ID=%2.2X(16)", msg, HK_frame_ID); \
//write_log_pipe( b); \
  if (DEBUG) \
  printf( "DEFRAMING WARNING: %s, ID=%2.2X(16)", msg, HK_frame_ID); \
  
  unsigned char         frame_index;
  static unsigned short block_index;
  unsigned short        expected_first_block_link;
  static int            block_open;
  static unsigned char  old_frame_ID;
        
  HK_frame_ID = HK.frame.header >> 8;
  HK_frame_first_block_link = HK.frame.header & 0x00ff;
  
  if (frame_sync && ((++old_frame_ID) % 100) != HK_frame_ID) {
    E("wrong frame sequence");
    frame_sync = FALSE;
  }

  old_frame_ID = HK_frame_ID;

  if (frame_sync) {
    frame_index = 0;
    if (block_open) {
      expected_first_block_link = AMS_block_length - block_index;
      if ((HK_frame_first_block_link != 0xff)
      &&  (HK_frame_first_block_link != expected_first_block_link)) {
        E("invalid link found");
        frame_sync = FALSE;
        return;
      }
    }
  }
  else {
    if (HK_frame_first_block_link == 0xff) {
      W("frame is not locked");
      return;
    }
    if (HK_frame_first_block_link >= HK_FRAME_BODY_LENGTH) {
      W("invalid link found");
      return;
    }
    else {
      frame_index = HK_frame_first_block_link;
      frame_sync = TRUE;
      if (block_open) {
        block_open = FALSE;
      }
    }
  }

  LOOP:
    if (!block_open) {
      if ((AMS_block_length = HK.frame.body[frame_index++]) == 0) {
        W("EOF encountered");
        frame_sync = FALSE;
        return;
      }
      if (AMS_block_length > MAX_ALLOWED_AMS_BLOCK_LENGTH) {
        E("tooo big block rejected");
        frame_sync = FALSE;
        return;
      }
      block_open = TRUE;
      block_index = 0;                       
    }
    while (block_index < AMS_block_length 
        && frame_index < HK_FRAME_BODY_LENGTH) {
      AMS_block_data[block_index++] = HK.frame.body[frame_index++];
    }   
    if (block_index == AMS_block_length) {
      block_open = FALSE;
      process_AMS_block();
    }
    if (block_index > AMS_block_length) {
      E("impossible situation!");
      frame_sync = FALSE;
      return;
    }
    if (frame_index == HK_FRAME_BODY_LENGTH) {
      return;
    }
    if (frame_index > HK_FRAME_BODY_LENGTH) {
      E("impossible situation!!");
      frame_sync = FALSE;
      return;
    }
  goto LOOP;
#undef DEBUG
}    

//------------------------------------------------------------------------------

void process_AMS_block( void) {
#define PRINT FALSE
  int i;
  char *node_type_name[16] = {
      "EVE", "GEN", "DAQ", "COM", "?",   "?",   "?",   "?",
      "MCC", "JL3", "CNT", "TRA", "HTC", "PSB", "MON", "TAS"
  };
  char *block_type_name[8] = { 
      "EVE", "GET", "SET", "EXE", "HK", "GPC", "?", "NASA"
  };
  
  if (PRINT) print_AMS_block();

  block_type  = (AMS_block_ID >> 13) & 0x07;
  node_type   = (AMS_block_ID >>  9) & 0x0F;
  node_number = (AMS_block_ID >>  6) & 0x07;
  data_type   = (AMS_block_ID      ) & 0x3F;
  
//if ((node_type == CNT_node_type) && ((data_type & 0x1F) == 0x06)) {
//if (node_type == 0x9) {
//if (node_type == CNT_node_type) {
//if (node_type == MCC_node_type && data_type == 0x1f) {  // NASA commands
  if (AMS_block_ID == 0x7000) {
    bl_length = AMS_block_length;  // pickup for display_AMS_block
    for (i=0; i<bl_length; i++) {
      bl_data[i] = AMS_block_data[i];
    }
  }

  if (AMS_block_length < 4) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "AMS block too short: l=%d(10),ID=%4.4X(16)=%s %s",
             AMS_block_length, AMS_block_ID,
             block_type_name[block_type], node_type_name[node_type]);
    write_log_pipe( b);
    return;
  }
  else {
    int AMS_block_status = (AMS_block_body[0] >> 12) & 0xF;
    if (AMS_block_status) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, warning, "BLK");
      sprintf( b, "AMS block: l=%d(10),ID=%4.4X(16)=%s %s #%d %2.2X, id=%4.4X(16)",
               AMS_block_length, AMS_block_ID,
               block_type_name[block_type], node_type_name[node_type],
               node_number, data_type,
               AMS_block_body[0]);
      write_log_pipe( b);
      /*
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if (write_N_tuples) {
        static FILE *file = NULL;
        if (file == NULL) file = fopen( "spy", "a");
        if (file != NULL) fprintf( file, "%2d\n", AMS_block_status);
      }
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      */
      return;
    }
  }

  block_time = ((integer32) AMS_block_data[2] << 16) | AMS_block_data[3];
//printf("%4.4X %4.4X %8.8lX \n", 
//AMS_block_data[2], AMS_block_data[3], block_time);  
  if ((block_type == HK_DATA_block_type  ) ||
      (block_type == GET_REPLY_block_type) ||
      (block_type == EXE_REPLY_block_type)) {
       
    if (node_type == PSB_node_type) process_PSB_block();
    if (node_type == HTC_node_type) process_HTC_block();
    if (node_type == MCC_node_type) {
      if ((data_type == MAP_data_type) || (data_type == MASK_data_type)) {
        process_CAN_block();
      }
      if ((data_type == 0x03) ||               // LIST
          (data_type == 0x06) ||               // DELAYS
          (data_type == 0x07) ||               // TIME_X
          (data_type == 0x0C)) {               // STATUS
        process_MCC_block();
      }
    }
    if (node_type == MON_node_type) process_MON_block();
    if (node_type == CNT_node_type) {
      if (((data_type & 0x1F) == TOF_HV_data_type)   ||
          ((data_type & 0x1F) == TOF_temp_data_type) ||
          ((data_type & 0x1F) == ACC_HV_data_type)   ||
          ((data_type & 0x1F) == ATC_HV_data_type))  {
        process_CNT_block();
      }
    }
    if ((node_type == JL3_node_type  && 
        (data_type == JL3_status_data_type || 
         data_type == JL1_scaler_data_type)) ||
        (node_type == JDQS_node_type && 
        (data_type & 0x1F) == JDQS_status_data_type)) {
      process_DAQ_block();
    }
  }
  
//- - - ->>>
// record PING MCC replies to check BUS COMMANDER
//- - - ->>>
///*
  if (write_N_tuples) {
    if (block_type == EXE_REPLY_block_type && AMS_block_ID == 0x7000) {
      int i;
      static first = TRUE;
      static FILE *file;
      if (first) {
        first = FALSE;
        file = fopen( "watch", "wt");
      }
      for (i=0; i<16; i++) {
        fprintf( file, "%4.4hX ", AMS_block_data[i]);
      }
      fprintf( file, "\n");
    }
  }
//*/
//- - - ->>>
#undef PRINT
}

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//                     GENERAL control functions                     *
//                                                                   *
//                                                                   *
//********************************************************************

//------------------------------------------------------------------------------

void process_keyboard_hit( void) {

  char *t;
  
  if (keyboard_hit) { 
    command = input_symbol = getch();
    if (command == 0 || command == 0xE0) getch();
    wrong_command = FALSE;
    if (!isprint(command)) command = '\002';
  }
  else {
    return;
  }

  switch (page_level) {
  
  case MAIN_page:
    switch (toupper(command)) {
    case ' ':
      break;
    case 'X':
    case 'E':
      stop_now = TRUE;
      sprintf(page_message,"        Program terminated...");
      break;
    case 'D':
      page_level = DETAILED_pages;
      command = ' ';
      sprintf(page_message," ");
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      page_to_display = 0;
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'T':
      page_level = TEST_pages;
      command = ' ';
      sprintf(page_message," ");
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      page_to_display = 0;
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'S':
      page_level = SUMMARY_pages;
      command = ' ';
      sprintf(page_message," ");
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      page_to_display = 0;
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case '@':
      if (I_am_DEC_ALPHA || I_am_in_BM_mode || SIMULATION) {
        wrong_command = TRUE;
      }
      else {
        page_level = DIALOG_page;
        command = ' ';
        sprintf( page_message,"        DIALOG MODE IS ON !!!!");
        sprintf( page_prompt, "enter '@' to leave dialog mode...");
        page_to_display = 0;
        page_frozen = FALSE;
        new_page = TRUE;
      }
      break;
    default:
      wrong_command = TRUE;
      break;
    }
    break;
    
  case DETAILED_pages:
    switch (toupper(command)) {
    case ' ':
      break;
    case 'X':
    case 'E':
      stop_now = TRUE;
      sprintf(page_message,"        Program terminated...");
      break;
    case 'M':
      page_level = MAIN_page;
      command = ' ';
      page_to_display = 0;
      sprintf(page_message," ");
      sprintf(page_prompt,"%s",main_page_prompt);
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'N':
      if (command == 'n') {
        page_to_display = (page_to_display + 1) % NB_OF_DETAILED_PAGES;
      }
      else {
        page_to_display = 
          (page_to_display + NB_OF_DETAILED_PAGES - 1) % NB_OF_DETAILED_PAGES;
      }
      sprintf(page_message," ");
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'F':
      if (page_frozen) break;
      page_frozen = TRUE;
      sprintf(page_prompt,"EXit/Next page/Main page/Release display");
      t = mytime(); t[19] = '\000';
      sprintf(page_message,
      "        Display is frozen at %s, hit \"R\" to release",&t[11]);
      break;
    case 'R':
      page_frozen = FALSE;
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      sprintf(page_message,"*       Display is released, waiting for new data...");
      break;
    default:
      wrong_command = TRUE;
      break;
    }
    break;
    
  case TEST_pages:
    switch (toupper(command)) {
    case ' ':
      break;
    case 'X':
    case 'E':
      stop_now = TRUE;
      sprintf(page_message,"        Program terminated...");
      break;
    case 'M':
      page_level = MAIN_page;
      page_to_display = 0;
      sprintf(page_message," ");
      sprintf(page_prompt,"%s",main_page_prompt);
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'N':
      if (command == 'n') {
        page_to_display = (page_to_display + 1) % NB_OF_TEST_PAGES;
      }
      else {
        page_to_display = (page_to_display + NB_OF_TEST_PAGES - 1) % 
                           NB_OF_TEST_PAGES;
      }
      sprintf(page_message," ");
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'F':
      if (page_frozen) break;
      page_frozen = TRUE;
      sprintf(page_prompt,"EXit/Next page/Main page/Release display");
      t = mytime(); t[19] = '\000';
      sprintf(page_message,
      "        Display is frozen at %s, hit \"R\" to release",&t[11]);
      break;
    case 'R':
      page_frozen = FALSE;
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      sprintf(page_message,"*       Display is released, waiting for new data...");
      break;
    default:
      wrong_command = TRUE;
      break;
    }
    break;

  case SUMMARY_pages:
    switch (toupper(command)) {
    case ' ':
      break;
    case 'X':
    case 'E':
      stop_now = TRUE;
      sprintf(page_message,"        Program terminated...");
      break;
    case 'M':
      page_level = MAIN_page;
      page_to_display = 0;
      sprintf(page_message," ");
      sprintf(page_prompt,"%s",main_page_prompt);
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'N':
      if (command == 'n') {
        page_to_display = (page_to_display + 1) % NB_OF_SUMMARY_PAGES;
      }
      else {
        page_to_display = (page_to_display + NB_OF_SUMMARY_PAGES - 1) % 
                           NB_OF_SUMMARY_PAGES;
      }
      sprintf(page_message," ");
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    case 'F':
      if (page_frozen) break;
      page_frozen = TRUE;
      sprintf(page_prompt,"EXit/Next page/Main page/Release display");
      t = mytime(); t[19] = '\000';
      sprintf(page_message,
      "        Display is frozen at %s, hit \"R\" to release",&t[11]);
      break;
    case 'R':
      page_frozen = FALSE;
      sprintf(page_prompt,"EXit/Next page/Main page/Freeze display");
      sprintf(page_message,"*       Display is released, waiting for new data...");
      break;
    default:
      wrong_command = TRUE;
      break;
    }
    break;
    
  case DIALOG_page:
    switch (toupper(command)) {
    case '@':
      page_level = MAIN_page;
      command = ' ';
      page_to_display = 0;
      sprintf( page_message," ");
      sprintf( page_prompt, "%s", main_page_prompt);
      page_frozen = FALSE;
      new_page = TRUE;
      break;
    default:
      display_dialog();
      break;
    }
    break;

  }
}

//------------------------------------------------------------------------------

void display_page_if_necessary( void) {

/*************************************************************************/
/*                     PAGE DISPLAY DECISION TABLE                       */
/*                     ---------------------------                       */
/*                                                                       */
/*                 NEW_PAGE       TIMER      KEYBOARD_HIT                */
/*  CLEAR_DISPLAY.....X                                                  */
/*  HEADLINE..........X.............X                                    */
/*  FRAME.............X                                                  */
/*  RIGHT PANEL.......X                                                  */
/*  DATA..............X.............X                                    */
/*  BOTTOMLINE........X.............X.............X                      */
/*                                                                       */
/*  note:                                                                */
/*    - HEADLINE, BOTTOMLINE are displayed by universal functions.       */
/*    - FRAME, RIGHT PANEL, DATA (group name BODY) are displayed by      */
/*      individual functions, and decision, what to display, is taken    */
/*      inside them.                                                     */
/*                                                                       */
/*************************************************************************/

  short timer;
  
  timer = (time( NULL) - page_time) > 1;
  if (new_page) clear_display();
  if (new_page || timer) display_page_body();
  if (new_page || timer || keyboard_hit) display_bottomline();
  if (page_level == SUMMARY_pages && page_to_display == MCDS_page) {
    display_MCDS_data();
  }
  new_page = FALSE;
  if (timer) page_time = time(NULL);
}

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//                   various junk-printing functions                 *
//                          (for debug only)                         *
//                                                                   *
//********************************************************************

//------------------------------------------------------------------------------

void print_frame_number( void) {
  printf(" New frame number = %x(16). \n", OIU_frame_number);
}

//------------------------------------------------------------------------------

void print_time_broadcast( void) {
  int i;
  printf(" New time broadcast =");
  for ( i=0; i<TIME_BROADCAST_LENGTH; i++) {
    printf(" %x", time_broadcast[i]);
  }
  printf("(16). \n");
}

//------------------------------------------------------------------------------

void print_PSP_frame( void) {
  int i;
  printf(" New PSP frame (hexadecimal 16-bit words): \n");
  for ( i=0; i<2*BOX_CAR_LENGTH; i++) {
    printf("%4.4x%c", HK.word[i],
    (i%16==15 || i==2*BOX_CAR_LENGTH-1) ? '\n' : ' ');
  }
}

//------------------------------------------------------------------------------

void print_PDI_frame( void) {
  int i;
  printf(" New PDI frame (hexadecimal 16-bit words): \n");
  for ( i=0; i<2*BOX_CAR_LENGTH; i++) {
    printf("%4.4x%c", HK.word[i],
    (i%16==15 || i==2*BOX_CAR_LENGTH-1) ? '\n' : ' ');
  }
}

//------------------------------------------------------------------------------

void print_HK_frame( void) {
  int i;
  printf("****** HK frame (hexadecimal): \n");
  printf("HK.frame.offset = %2.2x %2.2x %2.2x %2.2x\n",
  HK.frame.offset[0], HK.frame.offset[1], HK.frame.offset[2], HK.frame.offset[3]);
  printf( "HK.frame.header = %4.4x\n", HK.frame.header);
  printf( "HK.frame.body =\n");
  for ( i=0; i<HK_FRAME_BODY_LENGTH; i++) {
    printf("%4.4x%c", HK.frame.body[i],
    (i%16==15 || i==HK_FRAME_BODY_LENGTH-1) ? '\n' : ' ');
  }
  printf("HK.frame.CRC = %4.4x\n", HK.frame.CRC);
}

//------------------------------------------------------------------------------

void print_AMS_block( void) {
  int i;
  printf(" AMS_block_length = %d(10)\n", AMS_block_length);
  printf(" AMS_block_data (16): \n");
  for (i=0; i<AMS_block_length; i++) {
    printf("%4.4x%c", AMS_block_data[i],
    (i%16 == 15 || i == AMS_block_length-1) ? '\n' : ' ');
  }
}

//------------------------------------------------------------------------------

//********************************************************************
//                                                                   *
//                                                                   *
//                        "library" functions                        *
//                                                                   *
//                                                                   *
//********************************************************************

//------------------------------------------------------------------------------

char *mytime( void) {
  int i;
  long ltime; 
  char *t; 
  static char tt[26];
  time( &ltime); 
  t = ctime( &ltime);
  for (i=0; i<24; i++) tt[i] = t[i];
  tt[24] = '\000';
  return tt;
}

//------------------------------------------------------------------------------

void wait( int seconds) {
  time_t ltime, otime;
  otime = time( &ltime); while ((time(&ltime)-otime < seconds) && !kbhit());
  
}

//------------------------------------------------------------------------------

void open_log_pipe_for_writing( void) {
  log_pipe.put_counter = 0;
  log_pipe.put_pointer = 0;
}

//------------------------------------------------------------------------------

void write_log_pipe( char *message) {
  time_t now;
  now = time( NULL);
  log_pipe.put_counter++;
  if (log_pipe.put_counter >= LOG_PIPE_SIZE) {
    log_pipe.put_counter = LOG_PIPE_SIZE; 
  }
  sprintf( log_pipe.body[log_pipe.put_pointer], "%.8s %s",
           ctime(&now)+11, message);
  log_pipe.put_pointer = ++log_pipe.put_pointer % LOG_PIPE_SIZE;
}

//------------------------------------------------------------------------------

int open_log_pipe_for_reading( void) {

  if (!(log_pipe.get_counter = log_pipe.put_counter)) return FAIL;
  if (log_pipe.get_counter < LOG_PIPE_SIZE) {
    log_pipe.get_pointer = 0;
  }
  else {
    log_pipe.get_pointer = log_pipe.put_pointer;
  }
  return OK;
}

//------------------------------------------------------------------------------

char *read_log_pipe( void) {

  char *p;

  if (log_pipe.get_counter <= 0) return NULL;
  log_pipe.get_counter--;
  p = log_pipe.body[log_pipe.get_pointer];
  log_pipe.get_pointer = ++log_pipe.get_pointer % LOG_PIPE_SIZE;
  return p;
}

//------------------------------------------------------------------------------



