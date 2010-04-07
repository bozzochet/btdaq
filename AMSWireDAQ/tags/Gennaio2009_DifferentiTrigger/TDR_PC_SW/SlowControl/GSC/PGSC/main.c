// this is file main.c
//
// AMS 1553 bus monitor for STS PGSC computer
//
// - functions for main page display
//

#include "pgsc.h"               // it has some global data definitions

//------------------------------------------------------------------------------

void display_main_data_frame( void) {
  short row, col=1;
  char *line[21] = {
 "                                                               ", //  2
//         111111111122222222223333333333444444444455555555556666
//123456789012345678901234567890123456789012345678901234567890123
 " ------ HEATERS ------  ---- ELECTRONICS ----  ---- TOTAL ---- ", //  3
 " Total capacity.        Total capacity.        Capacity.       ", //  4
 " Total enabled..        Total enabled..        Enabled..       ", //  5
 " Total used.....        Total used.....        Used.....       ", //  6
 "                                                               ", //  7
//         111111111122222222223333333333444444444455555555556666
//123456789012345678901234567890123456789012345678901234567890123
 " ------- MCC ---------  ---- TEMP LIMITS ----  -- MAG FIELD -- ", //  8
 " 1553 RT#/Bus...        Over temp alarm...     Designed...     ", //  9
 " Uptime....             Over temp warn....     Maximum....     ", // 10
 " AMS Int. Time (UTC):   Under temp warn...     Minimum....     ", // 11
 "                        Under temp alarm..     Average....     ", // 12
 "                                                               ", // 13
//         111111111122222222223333333333444444444455555555556666
//123456789012345678901234567890123456789012345678901234567890123
 " ---- CAN NETWORK ----  ------ HRDL ------  ------- DAQ ------ ", // 14
 " # failed nodes....     Frame#..            Run ID..           ", // 15
 " # bad nodes.......     JBU01XXXXXX XXXXXX  Run Tag.           ", // 16
 " # bad links.......     JBU23XXXXXX XXXXXX  JDQx A..           ", // 17
 " # lost nodes......     JBU45XXXXXX XXXXXX  JDQx B..           ", // 18
 "                        JFO..XXX JBU..XXXX  Event#..           ", // 19
 "  123456789+ events     Xmit Time.XXXXXXXX  L3/L1,Hz           ", // 20
 "  with Run Tag XXXX     Run Type..XXXXXXXX  Livetime,%         ", // 21
 "       so far!                              Event Size         "  // 22
};
  
  set_colors( frame);
  for (row=2; row<23; row++) {
    display_line( row, 1, 63, line[row-2]);
  }

}

//------------------------------------------------------------------------------

void display_main_right_panel( short row, short col) {
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
}

//------------------------------------------------------------------------------

void display_main_data( void) {
  int i, j;
  short row, col;
  time_t now;
  short status;
  int data_may_be_wrong;
  int HTC_data_are_absent, PSB_data_are_absent;
  int HTC_data_are_old, PSB_data_are_old;
  
  if (page_frozen) return;
  
  now = time( NULL);

//= = = = = = = = POWER = = = = = = = = = = = = = = = = = = = = = =

//- - - - - - - - HEATERS - - - - - - - - - - - - - - - - - - - - -
//
//

  _settextposition( 4, 17);
  set_colors( frame);
  sprintf( b, "%5.0fW", HTC_grand_total.power.capacity);
  _outtext( b);

  data_are_absent = TRUE;
  data_are_old = FALSE;
  
  for (i=0; i<5; i++) {
    if (HTC[i].digital_IO_time) data_are_absent = FALSE;
    if (now - HTC[i].digital_IO_time > HTC_digital_IO_time_staleness) data_are_old = TRUE;
  }

  if (!PDD_stat_time) data_are_absent = TRUE;
  if (now - PDD_stat_time > PDD_stat_time_staleness) data_are_old = TRUE;
  
  _settextposition( 5, 17);
  set_value_colors( NORMAL);
  sprintf( b, "%5.0fW", HTC_grand_total.power.enabled);
  _outtext( b);
  _settextposition( 6, 17);
  set_value_colors( HTC_grand_total.power.status);
  sprintf( b, "%5.0fW", HTC_grand_total.power.used);
  _outtext( b);

  HTC_data_are_absent = data_are_absent;
  HTC_data_are_old    = data_are_old;
  
//- - - - - - - - ELECTRONICS - - - - - - - - - - - - - - - - - - -
//
// OK (include IMB status in total.power.used ?)
//
  _settextposition( 4, 40);
  set_colors( frame);
  sprintf( b, "%5.0fW", PSB_grand_total.power.capacity);
  _outtext( b);
  
  if (PSB_grand_total.power.used > PSB_grand_total.power.enabled) status = WARNING;
  else                                                            status = NORMAL;

  data_are_absent = TRUE;
  data_are_old = FALSE;

  for (i=0; i<4; i++) {
    if (PSB[i].digital_IO_time) data_are_absent = FALSE;
    if (now - PSB[i].digital_IO_time > PSB_digital_IO_time_staleness) data_are_old = TRUE;
  }

  _settextposition( 5, 40);
  set_value_colors( status);
  sprintf( b, "%5.0fW", PSB_grand_total.power.enabled);
  _outtext( b);
  
  PSB_data_are_absent = data_are_absent;
  PSB_data_are_old    = data_are_old;

  data_are_absent = TRUE;
  data_are_old = FALSE;

  for (i=0; i<4; i++) {
    if (PSB[i].ADC_time) data_are_absent = FALSE;
    if (now - PSB[i].ADC_time > PSB_ADC_time_staleness) data_are_old = TRUE;
  }

  _settextposition( 6, 40);
  set_value_colors( status);
  sprintf( b, "%5.0fW", PSB_grand_total.power.used);
  _outtext( b);
  
  PSB_data_are_absent &= data_are_absent;
  PSB_data_are_old    |= data_are_old;

//- - - - - - - - TOTAL - - - - - - - - - - - - - - - - - - - - - -
//
//  as OK as prev two.
//

  _settextposition( 4, 57);
  set_colors( frame);
  sprintf( b, "%5.0fW", 
      HTC_grand_total.power.capacity + PSB_grand_total.power.capacity);
  _outtext( b);

  if (PSB_grand_total.power.used > PSB_grand_total.power.enabled) status = WARNING;
  else                                                            status = NORMAL;

  data_are_absent = HTC_data_are_absent && PSB_data_are_absent;
  data_are_old    = HTC_data_are_old    || PSB_data_are_old;

  _settextposition( 5, 57);
  set_value_colors( NORMAL);
  sprintf( b, "%5.0fW", 
      HTC_grand_total.power.enabled + PSB_grand_total.power.enabled);
  _outtext( b);
  
  if (HTC_grand_total.power.status == WARNING ||
      PSB_grand_total.power.status == WARNING) status = WARNING;
  if (HTC_grand_total.power.status == ALARM ||
      PSB_grand_total.power.status == ALARM) status = ALARM;
  _settextposition( 6, 57);
  set_value_colors( status);
  sprintf( b, "%5.0fW", 
      HTC_grand_total.power.used + PSB_grand_total.power.used);
  _outtext( b);

//= = = = = = = = MCC = = = = = = = = = = = = = = = = = = = = = = = 
//
//

  if (MCDS_time && !error_in_1553_data) data_are_absent = FALSE;
  else                                  data_are_absent = TRUE;
  if (now - MCDS_time > MCDS_time_staleness) data_are_old = TRUE;
  else                                       data_are_old = FALSE;

  _settextposition( 9, 17);
  set_value_colors( MCDS_status.RT_MCC);
  sprintf( b, " %2d/%c ", MCDS_data.RT_MCC ? 28 : 4, RT_BUS);
  _outtext( b);

  if (MCC_status_time) data_are_absent = FALSE;
  else                 data_are_absent = TRUE;
  if (now - MCC_status_time > MCC_status_time_staleness) data_are_old = TRUE;
  else                                                   data_are_old = FALSE;
  
  _settextposition( 10, 12);
  set_value_colors( MCC_alivetime_status);
  {
    int dd,hh,mm,ss;
    dd = MCC_alivetime / (24*60*60);
    hh = ((MCC_alivetime - dd * (24*60*60)) / (60*60)) % 24;
    mm = ((MCC_alivetime - dd * (24*60*60) - hh*60*60) / (60)) % 60;
    ss =   MCC_alivetime % 60;                                    
    sprintf( b, "%2d/%2.2d:%2.2d:%2.2d", dd, hh, mm, ss);
  }
  _outtext( b);

  data_are_old = FALSE;
  _settextposition( 12, 2);
  {
    char *mon[12]={ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    struct tm *tm_ptr;
    if (block_time) {
      data_are_absent = FALSE;
      tm_ptr = gmtime( &block_time);
      sprintf( b, "%2d-%3s%2d %3.3d:%2.2d:%2.2d:%2.2d",
                 tm_ptr->tm_mday,  mon[tm_ptr->tm_mon], tm_ptr->tm_year%100,
                 tm_ptr->tm_yday+1,
                 tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
      if (strlen( b) != 21) sprintf( b, "---------------------");
    }
    else {
      data_are_absent = TRUE;
      sprintf( b, "---------------------");
    }
    if (abs( (short) difftime( now, block_time)) > 5 * 60) {
      set_value_colors( WARNING);
    }
    else {
      set_value_colors( NORMAL);
    }
  }
  _outtext( b);

//= = = = = = = = TEMPERATURE LIMITS  = = = = = = = = = = = = = = =
//
//  from AWR data.
//

  {
    int ii,j,nA=0,nW=0,nw=0,na=0;

    data_are_absent = TRUE;
    data_are_old = FALSE;
  
    for (i=0; i<5; i++) {
      if (HTC[i].AWR_time) data_are_absent = FALSE;
      if (now - HTC[i].AWR_time > HTC_AWR_time_staleness) data_are_old = TRUE;
      for(j=0;j<16;j++){
	if(HTC[i].chain[j].AWR & 0x80) nA++;
	if(HTC[i].chain[j].AWR & 0x40) nW++;
	if(HTC[i].chain[j].AWR & 0x08) na++;
	if(HTC[i].chain[j].AWR & 0x04) nw++;
      }
    }

    if (nA) { set_value_colors(ALARM);}
    else    { set_value_colors(NORMAL);}
    _settextposition( 9, 43);
    sprintf( b, "%3d", nA); 
    _outtext( b);

    if (nW) { set_value_colors(WARNING);}
    else    { set_value_colors(NORMAL);}
    _settextposition( 10, 43);
    sprintf( b, "%3d", nW); 
    _outtext( b);

    if (nw) { set_value_colors(WARNING);}
    else    { set_value_colors(NORMAL);}
    _settextposition( 11, 43);
    sprintf( b, "%3d", nw); 
    _outtext( b);

    if (na) { set_value_colors(ALARM);}
    else    { set_value_colors(NORMAL);}
    _settextposition( 12, 43);
    sprintf( b, "%3d", na); 
    _outtext( b);
  }

//= = = = = = = = B-FIELD = = = = = = = = = = = = = = = = = = = = =
//
//  OK 
//

  set_colors( frame);
  _settextposition( 9, 59);
  sprintf( b, "1.00"); 
  _outtext( b);

  data_are_absent = (B_sensor_time[0]|B_sensor_time[1]) ? FALSE : TRUE;
  data_are_old    = ((now - B_sensor_time[0] > B_sensor_time_staleness) ||
                     (now - B_sensor_time[1] > B_sensor_time_staleness)) ? TRUE : FALSE;
  set_value_colors( NORMAL);

  {
    int ii;
    float b_max = -1000.;
    float b_min =  1000.;
    float b_avg = 0.0;
    float b_fld[4];

    b_fld[0] = sqrt( B_sensor[0][0]*B_sensor[0][0]+
                     B_sensor[0][1]*B_sensor[0][1]+
                     B_sensor[0][2]*B_sensor[0][2] );
    b_fld[1] = sqrt( B_sensor[0][6]*B_sensor[0][6]+
                     B_sensor[0][7]*B_sensor[0][7]+
                     B_sensor[0][8]*B_sensor[0][8] );
    b_fld[2] = sqrt( B_sensor[1][0]*B_sensor[1][0]+
                     B_sensor[1][1]*B_sensor[1][1]+
                     B_sensor[1][2]*B_sensor[1][2] );
    b_fld[3] = sqrt( B_sensor[1][6]*B_sensor[1][6]+
                     B_sensor[1][7]*B_sensor[1][7]+
                     B_sensor[1][8]*B_sensor[1][8] );

    for(ii=0;ii<4;ii++){
      b_max = (b_max < b_fld[ii]) ? b_fld[ii] : b_max;
      b_min = (b_min > b_fld[ii]) ? b_fld[ii] : b_min;
      b_avg += b_fld[ii];
    }
    b_avg *= (1./4.);

    _settextposition( 10, 59);
    sprintf( b, "%4.2f",b_max); 
    _outtext( b);

    _settextposition( 11, 59);
    sprintf( b, "%4.2f",b_min); 
    _outtext( b);

    _settextposition( 12, 59);
    sprintf( b, "%4.2f",b_avg); 
    _outtext( b);

  }
  
//= = = = = = = = CAN = = = = = = = = = = = = = = = = = = = = = = =

  data_may_be_wrong = 0;
  if (CAN_map_time) data_are_absent = FALSE;
  else              data_are_absent = TRUE;
  if (now - CAN_map_time > CAN_map_time_staleness) data_are_old = TRUE;
  else                                             data_are_old = FALSE;
  if (data_are_old || data_are_absent) data_may_be_wrong++;

  sprintf( b, "%3d", nb_of_CAN_logical_nodes_failed);
  set_value_colors( NORMAL);
  if (nb_of_CAN_logical_nodes_failed) set_value_colors( ALARM);
  _settextposition( 15, 20);
  _outtext( b);

  sprintf( b, "%3d", nb_of_CAN_physical_nodes_failed);
  set_value_colors( NORMAL);
  if (nb_of_CAN_physical_nodes_failed) set_value_colors( WARNING);
  _settextposition( 16, 20);
  _outtext( b);

  sprintf( b, "%3d", nb_of_CAN_links_failed);
  set_value_colors( NORMAL);
  if (nb_of_CAN_links_failed) set_value_colors( WARNING);
  _settextposition( 17, 20);
  _outtext( b);

  if (CAN_mask_time) data_are_absent = FALSE;
  else               data_are_absent = TRUE;
  if (now - CAN_mask_time > CAN_mask_time_staleness) data_are_old = TRUE;
  else                                               data_are_old = FALSE;
  if (data_are_old || data_are_absent) data_may_be_wrong++;

  data_are_old = data_may_be_wrong;               // to set "old" color

  sprintf( b, "%3d", nb_of_CAN_logical_nodes_lost);
  set_value_colors( NORMAL);
  if (nb_of_CAN_logical_nodes_lost) set_value_colors( WARNING);
  _settextposition( 18, 20);
  _outtext( b);

//= = = = = = = = HRDL  = = = = = = = = = = = = = = = = = = = = = =
//
// buffer left & JFO mode not yet.
//

  if (data_frame_number_time) data_are_absent = FALSE;
  else                        data_are_absent = TRUE;
  if (now -data_frame_number_time > JL3_time_staleness) data_are_old = TRUE;
  else                                                  data_are_old = FALSE;

  _settextposition( 15, 33);
  set_value_colors( data_frame_number_status);
  sprintf( b, "%10ld", data_frame_number);
  _outtext( b);

  if (JBU_frame_counter_time) data_are_absent = FALSE;
  else                        data_are_absent = TRUE;
  if (now - JBU_frame_counter_time > JL3_time_staleness) data_are_old = TRUE;
  else                                                   data_are_old = FALSE;

  _settextposition( 16, 30);
  set_value_colors( JBU_frame_counter_status[0]);
  sprintf( b, "%6ld", JBU_frame_counter[0]);
  _outtext( b);
  _settextposition( 16, 36);
  set_value_colors( JBU_frame_counter_status[1]);
  sprintf( b, "%7ld", JBU_frame_counter[1]);
  _outtext( b);
  _settextposition( 17, 30);
  set_value_colors( JBU_frame_counter_status[2]);
  sprintf( b, "%6ld", JBU_frame_counter[2]);
  _outtext( b);
  _settextposition( 17, 36);
  set_value_colors( JBU_frame_counter_status[3]);
  sprintf( b, "%7ld", JBU_frame_counter[3]);
  _outtext( b);
  _settextposition( 18, 30);
  set_value_colors( JBU_frame_counter_status[4]);
  sprintf( b, "%6ld", JBU_frame_counter[4]);
  _outtext( b);
  _settextposition( 18, 36);
  set_value_colors( JBU_frame_counter_status[5]);
  sprintf( b, "%7ld", JBU_frame_counter[5]);
  _outtext( b);

  if (JFO_used_time) data_are_absent = FALSE;
  else               data_are_absent = TRUE;
  if (now - JFO_used_time > JL3_time_staleness) data_are_old = TRUE;
  else                                          data_are_old = FALSE;

  _settextposition( 19, 30);
  set_value_colors( JFO_used_status);
  sprintf( b, " %1s ", JFO_used);
  _outtext( b);

  if (JBU_output_time) data_are_absent = FALSE;
  else                 data_are_absent = TRUE;
  if (now - JBU_output_time > JL3_time_staleness) data_are_old = TRUE;
  else                                            data_are_old = FALSE;

  _settextposition( 19, 39);
  set_value_colors( JBU_output_status);
  sprintf( b, " %3s", JBU_output);
  _outtext( b);

  if (run_type_time) data_are_absent = FALSE;
  else               data_are_absent = TRUE;
  if (now - run_type_time > JL3_time_staleness) data_are_old = TRUE;
  else                                          data_are_old = FALSE;

{ 
  float total_nb_of_frames = 0.0;
  int transmittion_time;
  for (i=0; i<6; i++) total_nb_of_frames += JBU_frame_counter[i];
  transmittion_time = total_nb_of_frames * 4080.0 * 8.0 / 1960000.0 / 60.0 + 0.5;
  _settextposition( 20, 35);
  set_value_colors( NORMAL);
  sprintf( b, "%7d'", transmittion_time);
  _outtext( b);
}
  
  _settextposition( 21, 35);
  set_value_colors( run_type_status);
  if      (run_type == 0x00) sprintf( b, " NO RUN ");
  else if (run_type == 0x01) sprintf( b, " normal ");
  else if (run_type == 0x02) sprintf( b, " rawdata");
  else if (run_type == 0x04) sprintf( b, " calibr ");
  else if (run_type == 0x08) sprintf( b, " mixed  ");
  else if (run_type == 0x10) sprintf( b, " laser 1");
  else if (run_type == 0x20) sprintf( b, " laser 2");
  else                       sprintf( b, " unknown");
  _outtext( b);

//= = = = = = = = DAQ = = = = = = = = = = = = = = = = = = = = = = =
//
// much data to be filled in....
//

  if (run_number_time) data_are_absent = FALSE;
  else                 data_are_absent = TRUE;
  if (now - run_number_time > JL3_time_staleness) data_are_old = TRUE;
  else                                            data_are_old = FALSE;

  sprintf( b, " %4.4X %4.4X", 
      (integer16) (run_number >> 16), 
      (integer16) (run_number & 0xFFFF));
  if (run_number) set_value_colors( NORMAL);
  else            set_value_colors( WARNING);
  _settextposition( 15, 53);
  _outtext( b);
  
//--

  if (run_tag_time) data_are_absent = FALSE;
  else              data_are_absent = TRUE;
  if (now - run_tag_time > JL3_time_staleness) data_are_old = TRUE;
  else                                         data_are_old = FALSE;

  sprintf( b, " %4.4X %4.4X", 
      (integer16) (run_tag >> 16), 
      (integer16) (run_tag & 0xFFFF));
   set_value_colors( NORMAL);
  _settextposition( 16, 53);
  _outtext( b);

//--

  if (crate_found_time) data_are_absent = FALSE;
  else                  data_are_absent = TRUE;
  if (now - crate_found_time > JL3_time_staleness) data_are_old = TRUE;
  else                                             data_are_old = FALSE;
  set_value_colors( crate_found_status);
  {
    char c[11] = { 't', 't',  's', 's', 's', 's', 's', 's', 's', 's','\0'};
    if (crate_found & 0x00000002) c[0] ^= 0x20;
    if (crate_found & 0x00000004) c[1] ^= 0x20;
    if (crate_found & 0x00000008) c[2] ^= 0x20;
    if (crate_found & 0x00000010) c[3] ^= 0x20;
    if (crate_found & 0x00000020) c[4] ^= 0x20;
    if (crate_found & 0x00000040) c[5] ^= 0x20;
    if (crate_found & 0x00000080) c[6] ^= 0x20;
    if (crate_found & 0x00000100) c[7] ^= 0x20;
    if (crate_found & 0x00000200) c[8] ^= 0x20;
    if (crate_found & 0x00000400) c[9] ^= 0x20;
    _settextposition( 17, 53);
    _outtext( c);
  }
  {
    char c[11] = { 't', 't',  's', 's', 's', 's', 's', 's', 's', 's','\0'};
    if (crate_found & 0x00020000) c[0] ^= 0x20;
    if (crate_found & 0x00040000) c[1] ^= 0x20;
    if (crate_found & 0x00080000) c[2] ^= 0x20;
    if (crate_found & 0x00100000) c[3] ^= 0x20;
    if (crate_found & 0x00200000) c[4] ^= 0x20;
    if (crate_found & 0x00400000) c[5] ^= 0x20;
    if (crate_found & 0x00800000) c[6] ^= 0x20;
    if (crate_found & 0x01000000) c[7] ^= 0x20;
    if (crate_found & 0x02000000) c[8] ^= 0x20;
    if (crate_found & 0x04000000) c[9] ^= 0x20;
    _settextposition( 18, 53);
    _outtext( c);
  }
  
//--

  if (event_number_time) data_are_absent = FALSE;
  else                   data_are_absent = TRUE;
  if (now - event_number_time > JL3_time_staleness) data_are_old = TRUE;
  else                                              data_are_old = FALSE;

  sprintf( b, "%10ld", event_number);
  set_value_colors( event_number_status);
  _settextposition( 19, 53);
  _outtext( b);

//--

  if (L3_rate_time) data_are_absent = FALSE;
  else              data_are_absent = TRUE;
  if (now - L3_rate_time > JL3_time_staleness) data_are_old = TRUE;
  else                                         data_are_old = FALSE;

  sprintf( b, "%4.0f /%4.0f", L3_rate, L1_rate);
  set_value_colors( L3_rate_status);
  _settextposition( 20, 53);
  _outtext( b);

//--

  if (JL1_scaler_time) data_are_absent = FALSE;
  else                 data_are_absent = TRUE;
  if (now - JL1_scaler_time > JL1_scaler_time_staleness) data_are_old = TRUE;
  else                                                   data_are_old = FALSE;

  sprintf( b, " %5.1f ", JL1_scaler_livetime * 100.0);
  if (JL1_scaler_livetime) set_value_colors( JL1_scaler_livetime_status);
  else                     set_value_colors( NO_DATA);
  _settextposition( 21, 56);
  _outtext( b);

//--

  sprintf( b, "%6.0f ", event_size);
  set_value_colors( event_size_status);
  _settextposition( 22, 56);
  _outtext( b);

//--

  set_value_colors( WARNING);
  sprintf( b, "  %9ld+ events  ", this_run.nb_of_events);
  _settextposition( 20, 2);
  _outtext( b);
  sprintf( b, "  with Run Tag %4.4X  ", (int) (this_run.tag & 0xFFFF));
  _settextposition( 21, 2);
  _outtext( b);
  sprintf( b, "       so far!       ");
  _settextposition( 22, 2);
  _outtext( b);



}
//------------------------------------------------------------------------------

void display_main_page( void) {
  short row, col;
  display_headline( "*** MAIN PAGE ***");
  if (new_page) {
    display_main_data_frame();
    display_main_right_panel( row=2, col=64);
  }
  display_main_data();
}

//------------------------------------------------------------------------------
