// file fslib.h
//
// see file fslib.c
//

//~----------------------------------------------------------------------------

#ifndef _FSLIB_H
#define _FSLIB_H

//~----------------------------------------------------------------------------

#include "uscmlib.h"
#include "xformslib.h"
 
typedef struct {
  char x_[80];
  char y_[80];
} twochar;
 
typedef struct {
  char x_[80];
} onechar;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int clk, dat;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int   sprintf_binary32(char*, int32, int);
int   parity32( int32 );
void  wait( float );

void update_hc_config_file(void);

void create_HOTCOLD_panel(void);
void show_HOTCOLD_panel(void);
void add_author(int WW, int HH, char *date, char *author);
void  enable_Marotta_valve( char*, int16*, int8* );
void  disable_Marotta_valves( char*, int16*, int8* );
onechar  read_Marotta_valve_status( char*, int16*, bool* );
void  open_Marotta_valve( char*, float, int16*, int8* );
void  enable_heater( char*, int16*, int8* );
void  disable_heaters( int16*, int8* );
void  enable_tension( char*, int16*, int8* );
void  disable_tensions( char*, int16*, int8* );
onechar  read_tensions_status( char*, int16*, bool* );
void  start_heater( char*, float, int16*, int8* );
void  enable_serial( char*, int16*, int8* );
void  disable_serials( int16*, int8* );
void  enable_channel( char*, int16*, int8* );
void  disable_channels( int16*, int8* );
int16 get_hex_time(float);
int16 get_heat_time(float);
void  turn_on_pump( char*, char*, int16*, int8* );
void  turn_off_pump( char*, int16*, int8* );
twochar  read_pump_status( char*, int16*, bool*);
void  convert_read_gp50( char*, int16*, int8*, float* );
void  read_gp50( char*, int16*, int8*, float* );
void  convert_gp50( char*, int16*, int8* );
void  convert_read_kulite( char*, int16*, int8*, float* );
void  read_kulite( char*, int16*, int8*, float* );
void  convert_kulite( char*, int16*, int8* );
void  read_UGadc( char*, int16*, int8*, float* );
void  convert_UGadc( char*, int16*, int8* );
void  open_flipper( char*, char*, int16*, int8* );
void  close_flipper( char*, char*,  int16*, int8* );
void  read_honeywell(char* , int16*, int8*, int16* );


void  empty_mix_volume( int16*, int8*, float* );
void  empty_mix_volume_TEST( int16*, int8*, float* );
void  set_ground_state_Xe( int16*, int8*, float* );
void  set_ground_state_CO2( int16*, int8*, float* );
void  xenon_fill_step( int16*, int8*, float* );
void  co2_fill_step( int16*, int8*, float* );
void  increase_valve_count(char*);

//~============================================================================

#endif // _FSLIB_H
