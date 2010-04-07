// this is file can.c
//
// AMS 1553 bus monitor for STS PGSC computer
//
// - functions for CAN-bus map data
//

#include "pgsc.h"               // it has some global data definitions

//------------------------------------------------------------------------------

struct rccoord summary1, summary2, summary3, summary4;

static struct {
  char *name;
  unsigned char code;
} CAN_node[64] = {

  { NULL,          0x00 },
  { " spare MCC ", 0x0F },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },

  { " JL3A.02.02", 0x0F },
  { " JL3B.02.04", 0x0F },
  { " JL3B.02.06", 0x0F },
  { " JL3B.02.13", 0x0F },
  { " JL3B.02.15", 0x0F },
  { " JL3A.02.17", 0x0F },
  { NULL,          0x00 },
  { NULL,          0x00 },
 
  { " JDQS.01.05", 0xFF },
  { " JDQS.31.05", 0xFF },
  { " JDQS.41.05", 0xFF },
  { " JDQS.71.05", 0xFF },
  { " JDQS.03.05", 0xFF },
  { " JDQS.33.05", 0xFF },
  { " JDQS.43.05", 0xFF },
  { " JDQS.73.05", 0xFF },

  { NULL,          0x00 },
  { NULL,          0x00 },
  { " JDQT.32.10", 0x0F },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { " JDQT.72.10", 0x0F },
  { NULL,          0x00 },
  { NULL,          0x00 },

  { "   HTC.11  ", 0xFF },
  { "   HTC.23  ", 0xFF },
  { "   HTC.51  ", 0xFF },
  { "   HTC.63  ", 0xFF },
  { "   HTC.83  ", 0xFF },
  { "    PDD    ", 0xFF },
  { NULL,          0x00 },
  { NULL,          0x00 },

  { "   PSB.0   ", 0xFF },
  { NULL,          0x00 },
  { "   PSB.4   ", 0xFF },
  { "   PSB.7   ", 0xFF },
  { "   PSB.3   ", 0xFF },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },

  { NULL,          0x00 },
  { " MSEA.31.08", 0x0F },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { " MSEB.02.19", 0x0F },
  { " MSEA.71.08", 0x0F },
  { NULL,          0x00 },

  { " TASE.73.07", 0x0F },
  { " TASE.33.07", 0x0F },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 },
  { NULL,          0x00 }
/*
  { "  not used ", 0x00 },
  { "  not used ", 0x00 },
  { "  not used ", 0x00 }
*/
};

//------------------------------------------------------------------------------

void simulate_CAN_block( void) {
  time_t now;
  static int first = TRUE;

  int ii, iii, j, k, l;   static long q = 0;
  q++;  if (q > 20) q = 0; if (q > 10 && ii == 2) return;
  
//AMS_block_ID = block_type<<13 | node_type<<9 | node_number<<6 | data_type;

  iii = rand() % 2;
  
//*********************************************
//*                                           *
//*      simulate HK block 'header'           *
//*                                           *
//*********************************************

    AMS_block_body[0] = (AMS_block_body[0] + 1) & 0x0FFF;
    if (rand()%32==0) AMS_block_body[0] |= 0x4000;
    now = time( NULL);
    AMS_block_body[1] = (now & 0xFFFF0000) >> 16;
    AMS_block_body[2] =  now & 0x0000FFFF;
                                                      
//*************************************
//*                                   *
//*        simulate CAN MAP table     *
//*                                   *
//*************************************

  if (iii == 0) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   MCC_node_type        <<  9 |
                   0x0                  <<  6 |
                   MAP_data_type;
    
    for (j=0; j<32; j++) {
      AMS_block_body[j+3] = CAN_node[2*j].code<<8 | CAN_node[2*j+1].code;
      if (rand() % 10 == 0) AMS_block_body[j+3] &= 0xFFF0;
      if (rand() % 10 == 0) AMS_block_body[j+3] &= 0xFF0F;
      k = rand() % 16;
      if (rand() % 32 == 0) AMS_block_body[j+3] ^= 1 << k;
/*      
      k = rand() % 16;
      AMS_block_body[j+3] ^= 1 << k;
      k = rand() % 16;
      AMS_block_body[j+3] ^= 1 << k;
      k = rand() % 8;
      AMS_block_body[j+3] ^= 1 << k;
      k = rand() % 8;
      AMS_block_body[j+3] ^= 1 << k;
      k = rand() % 8;
      AMS_block_body[j+3] ^= 1 << k;
*/
    }

    AMS_block_length = 1 + 3 + 32;
    if (first) {
      AMS_block_length++;
      first = FALSE;
    }
  
    return;
  }
  
//*************************************
//*                                   *
//*       simulate CAN MASK table     *
//*                                   *
//*************************************

  if (iii == 1) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   MCC_node_type        <<  9 |
                   0x0                  <<  6 |
                   MASK_data_type;

    for (j=0; j<32; j++) {
      AMS_block_body[j+3] = CAN_node[2*j].code<<8 | CAN_node[2*j+1].code;
      k = rand() % 16;
      AMS_block_body[j+3] ^= 1 << k;
    }

    AMS_block_length = 1 + 3 + 32;
  
    return;
  }
}  
//------------------------------------------------------------------------------

void initialize_CAN_page( void) {
  int i, j;
  static short first = TRUE;
  if (first) {
    first = FALSE;
  }
}

//------------------------------------------------------------------------------

void process_CAN_block( void) {
  int i, j;
  int code, map, mask, m;

//***************************************
//*                                     *
//*  CAN (=MCC) block structure checks  *
//*                                     *
//***************************************

  if ((block_type != GET_REPLY_block_type) &&
      (block_type != EXE_REPLY_block_type) &&
      (block_type != HK_DATA_block_type  )) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MCC/CAN block with wrong block type %X(16)", block_type);
    write_log_pipe( b);
    return;
  }

  if (node_type != MCC_node_type) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MCC/CAN block with wrong node type %X(16)", node_type);
    write_log_pipe( b);
    return;
  }  

  if (node_number != 0) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MCC/CAN block with wrong node number %X(16)", node_number);
    write_log_pipe( b);
    return;
  } 

  if (AMS_block_length != (1+3+32)) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "MCC/CAN block with wrong length %d(10)",
             AMS_block_length);
    write_log_pipe( b);
    return;
  }
    
//*********************************************
//*                                           *
//*            process CAN MAP table          *
//*                                           *
//*********************************************

  if (data_type == MAP_data_type) {

//  if (SIMULATION && ((time(NULL) % 60) > 40)) return; // simulate old data

    CAN_map_time = time(NULL);
     
    for (j=0; j<32; j++) {
      actual_CAN_node[2*j].map   =  AMS_block_body[j+3]       >> 8;
      actual_CAN_node[2*j+1].map = (AMS_block_body[j+3] << 8) >> 8;
    }

// check data and set error counters, flags, etc.

    nb_of_CAN_logical_nodes_failed  = 0;
    nb_of_CAN_physical_nodes_failed = 0;
    nb_of_CAN_links_failed          = 0;
    
    CAN_map_corrupted = FALSE;

    for (i=0; i<64; i++) {
      code = CAN_node[i].code;
      map = actual_CAN_node[i].map;
      actual_CAN_node[i].map_corrupted = FALSE;
      actual_CAN_node[i].failed        = FALSE;
      actual_CAN_node[i].has_bad_nodes = FALSE;
      actual_CAN_node[i].has_bad_links = FALSE;
      if (code) {
        if (~code & map) {
          CAN_map_corrupted = TRUE;     // map is more than allowed
          actual_CAN_node[i].map_corrupted = TRUE;
        }
        if ((!(map & 0x08) &&  (map & 0x07)) ||
            ( (map & 0x08) && !(map & 0x07)) ||
            (!(map & 0x80) &&  (map & 0x70)) ||
            ( (map & 0x80) && !(map & 0x70))) {
          CAN_map_corrupted = TRUE;     // incorrect disabling
          actual_CAN_node[i].map_corrupted = TRUE;
        }
      }
      if (code && !map) {
        nb_of_CAN_logical_nodes_failed++;
        actual_CAN_node[i].failed = TRUE;
      }
      if (code & 0x0F) {
        if (!(map & 0x01)) nb_of_CAN_links_failed++;
        if (!(map & 0x02)) nb_of_CAN_links_failed++;
        if (!(map & 0x04)) nb_of_CAN_links_failed++;
        if ((map & 0x07) != 0x07) actual_CAN_node[i].has_bad_links = TRUE;
        if (!(map & 0x0F)) {
          nb_of_CAN_physical_nodes_failed++;
          actual_CAN_node[i].has_bad_nodes = TRUE;
        }
      } 
      if (code & 0xF0) {
        if (!(map & 0x10)) nb_of_CAN_links_failed++;
        if (!(map & 0x20)) nb_of_CAN_links_failed++;
        if (!(map & 0x40)) nb_of_CAN_links_failed++;
        if ((map & 0x70) != 0x70) actual_CAN_node[i].has_bad_links = TRUE;
        if (!(map & 0xF0)) {
          nb_of_CAN_physical_nodes_failed++; 
          actual_CAN_node[i].has_bad_nodes = TRUE;
        }
      } 
    }
  }

//*************************************
//*                                   *
//*       process CAN MASK table      *
//*                                   *
//*************************************

  if (data_type == MASK_data_type) {

//  if (SIMULATION && ((time(NULL) % 60) < 20)) return; // simulate old data

    CAN_mask_time = time(NULL);
     
    for (j=0; j<32; j++) {
      actual_CAN_node[2*j].mask   =  AMS_block_body[j+3]       >> 8;
      actual_CAN_node[2*j+1].mask = (AMS_block_body[j+3] << 8) >> 8;
    }

// check data and set error counters, flags, etc.

    nb_of_CAN_masks_modified = 0;
    
    for (i=0; i<64; i++) {
      code = CAN_node[i].code;
      mask = actual_CAN_node[i].mask;
      actual_CAN_node[i].access_modified = FALSE;
      if (code && (code != mask)) {
        nb_of_CAN_masks_modified++;
        actual_CAN_node[i].access_modified = TRUE;
      }
    }
  }

//************************************************************
//*                                                          *
//*             process CAN both MASK and MAP tables         *
//*                                                          *
//************************************************************

  if (CAN_mask_time && CAN_map_time) {
  
    nb_of_CAN_logical_nodes_lost = 0;
  
    for (i=0; i<64; i++) {
      actual_CAN_node[i].lost = FALSE;
      m = actual_CAN_node[i].map & actual_CAN_node[i].mask;
      if (CAN_node[i].code) {
        if ( (!(m & 0x07) || !(m & 0x08)) &&
             (!(m & 0x70) || !(m & 0x80)) ) {
          actual_CAN_node[i].lost = TRUE;
          nb_of_CAN_logical_nodes_lost++;
        }
      }
    }
  }
}

//------------------------------------------------------------------------------

void display_CAN_right_panel( short row, short col) {

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
  _outtext("CBA");  
  set_colors( headline); 
  _outtext(" - 3 buses");
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("cBa");  
  set_colors( headline); 
  _outtext(" - off/on/off");
  
  ++row;
  ++row;
  ++row;
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext("<== CAN node name");
  
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext("<== CAN map");
  
  set_colors( headline); 
  _settextposition( ++row, col+1);
  _outtext("<== CAN mask");
}

//------------------------------------------------------------------------------

void display_CAN_data_frame( void) {     

  int i, j;
  short row, col;
  struct rccoord end1, end2;
  char *bar;
  char *BLANK = { " "};
  
  set_colors( frame);
  bar = VBAR;  
  for (i=j=0; i<64; i++) {
    if (CAN_node[i].name != NULL) {
      sprintf( b, "%s%11s%s", bar, CAN_node[i].name, bar);
      row = (j / 5) * 3;
      col = (j % 5) * 12;
      _settextposition( row+2, col+1);
      _outtext( b);
      end1 = _gettextposition();
      _settextposition( row+3, col+1);
      _outtext( bar);
      _settextposition( row+4, col+1);
      _outtext( bar);
      _settextposition( end1.row+1, end1.col-1);
      _outtext( bar);
      _settextposition( end1.row+2, end1.col-1);
      _outtext( bar);
      j++;
    }
  }

  _settextposition( end1.row, end1.col);
  sprintf( b, " ------ CAN network summary ------ %s", bar); 
  _outtext( b); 
  end2 = _gettextposition();
  _settextposition( end2.row+1, end2.col-1);
  _outtext( bar);
  _settextposition( end2.row+2, end2.col-1);
  _outtext( bar);

  _settextposition( end1.row+1, end1.col);
  _outtext( " Nb. of logical nodes failed:");
  summary1 = _gettextposition();
  _settextposition( summary1.row, summary1.col+2);
  _outtext( "    ");

  _settextposition( end1.row+2, end1.col);
  _outtext( " Bad nodes:");
  summary2 = _gettextposition();

  _settextposition( summary2.row, summary2.col+2);
  _outtext( " links:");
  summary3 = _gettextposition();

  _settextposition( summary3.row, summary3.col+2);
  _outtext( " Lost nodes");
  summary4 = _gettextposition();

//_outtext("  CAN map corrupted?......");
}

//------------------------------------------------------------------------------

void display_CAN_data( void) {

  int i, j, k, m, data_may_be_wrong;
  unsigned char code, map, mask;
  char c1[5] = { ' ', ' ', ' ', ' ', '\0'};
  char c2[5] = { ' ', ' ', ' ', ' ', '\0'};
  short row, col;
  long now;
      
  if (page_frozen) return;
 
  now = time( NULL);

  data_may_be_wrong = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - CAN MAP - - - - -  

  if (CAN_map_time) data_are_absent = FALSE;
  else              data_are_absent = TRUE;
      
  if (now - CAN_map_time > 30) data_are_old = TRUE;
  else                         data_are_old = FALSE;

  for (i=j=0; i<64; i++) {
    code   = CAN_node[i].code;
    map = actual_CAN_node[i].map;
    if (code) {
      c1[0] = (map & 0x80) ? '+' : '-';
      c1[1] = (map & 0x40) ? 'C' : 'c';
      c1[2] = (map & 0x20) ? 'B' : 'b';
      c1[3] = (map & 0x10) ? 'A' : 'a';
      c2[0] = (map & 0x08) ? '+' : '-';
      c2[1] = (map & 0x04) ? 'C' : 'c';
      c2[2] = (map & 0x02) ? 'B' : 'b';
      c2[3] = (map & 0x01) ? 'A' : 'a';
      sprintf( b, " %4s %4s ", c1, c2);
      set_value_colors( NORMAL);
      if (actual_CAN_node[i].has_bad_nodes) set_value_colors( WARNING);
      if (actual_CAN_node[i].has_bad_links) set_value_colors( WARNING);
      if (actual_CAN_node[i].map_corrupted) set_value_colors( ALARM);
      if (actual_CAN_node[i].failed)        set_value_colors( ALARM);
      if (actual_CAN_node[i].lost)          set_value_colors( ALARM);
      row = (j / 5) * 3;
      col = (j % 5) * 12;
      _settextposition( row+3, col+2);
      _outtext( b);
      j++;
    }
  }

  sprintf( b, "%2d", nb_of_CAN_logical_nodes_failed);
  if (nb_of_CAN_logical_nodes_failed) set_value_colors( ALARM);
  else                                set_value_colors( NORMAL);
  _settextposition( summary1.row, summary1.col);
  _outtext( b);

  sprintf( b, "%2d", nb_of_CAN_physical_nodes_failed);
  if (nb_of_CAN_physical_nodes_failed) set_value_colors( WARNING);
  else                                 set_value_colors( NORMAL);
  _settextposition( summary2.row, summary2.col);
  _outtext( b);

  sprintf( b, "%2d", nb_of_CAN_links_failed);
  if (nb_of_CAN_links_failed) set_value_colors( WARNING);
  else                        set_value_colors( NORMAL);
  _settextposition( summary3.row, summary3.col);
  _outtext( b);

  _settextposition( 22, 64);
  if (CAN_map_corrupted) {
    set_value_colors( ALARM);
    _outtext( "Bad CAN map!");
  }
  else {
    set_colors( headline);
    _outtext( "            ");
  }

  if (data_are_old || data_are_absent) data_may_be_wrong++;

//- - - - - - - - - - - - - - - - - - - - - - - - - - CAN MASK - - - - -  

  if (CAN_mask_time) data_are_absent = FALSE;
  else                data_are_absent = TRUE;
      
  if (now - CAN_mask_time > 30) data_are_old = TRUE;
  else                           data_are_old = FALSE;

  for (i=j=0; i<64; i++) {
    code   = CAN_node[i].code;
    map = actual_CAN_node[i].map;
    mask  = actual_CAN_node[i].mask;
    if (code != 0) {
      c1[0] = (mask & 0x80) ? '+' : '-';
      c1[1] = (mask & 0x40) ? 'C' : 'c';
      c1[2] = (mask & 0x20) ? 'B' : 'b';
      c1[3] = (mask & 0x10) ? 'A' : 'a';
      c2[0] = (mask & 0x08) ? '+' : '-';
      c2[1] = (mask & 0x04) ? 'C' : 'c';
      c2[2] = (mask & 0x02) ? 'B' : 'b';
      c2[3] = (mask & 0x01) ? 'A' : 'a';
      sprintf( b, " %4s %4s ", c1, c2);
      set_value_colors( NORMAL);
      if (actual_CAN_node[i].access_modified)  set_value_colors( WARNING);
      if (actual_CAN_node[i].failed)           set_value_colors( ALARM);
      if (actual_CAN_node[i].lost)             set_value_colors( ALARM);
      row = (j / 5) * 3;
      col = (j % 5) * 12;
      _settextposition( row+4, col+2);
      _outtext( b);
      j++;
    }
  }

  if (data_are_old || data_are_absent) data_may_be_wrong++;

  data_are_old = data_may_be_wrong;               // to set "old" color
  
  sprintf( b, "%2d", nb_of_CAN_logical_nodes_lost);
  if (nb_of_CAN_logical_nodes_lost) set_value_colors( WARNING);
  else                              set_value_colors( NORMAL);
  _settextposition( summary4.row, summary4.col);
  _outtext( b);

}

//------------------------------------------------------------------------------

void display_CAN_page( void) {
  short row, col;
  display_headline( "CAN BUS NETWORK PAGE");
  if (new_page) {
    display_CAN_data_frame();
    display_CAN_right_panel( row=2, col=62);
  }
  display_CAN_data();
}
 
//------------------------------------------------------------------------------
