// SRDL_frames.h
// pieces of deframing program for SRDL link

// one should use D-switch when calling compiler:
// "cl ... /Dthis_is_PC_computer ..." on PC-computers
// "cc ... -Dthis_is_ALPHA_computer ..." on ALPHA-computers

#include "all_frames.h"

//----------------------------------------------------------------------------------
//
//---- declaration of functions defined in file HRDL_frames.c
//

//
//  functions used for deframing
//

void initialize_everything( void);
int open_SRDL_frame_file( void);
int read_SRDL_frame_file( void);
int decode_SRDL_frame( void);
void deframe_SRDL_frame( void);
int close_SRDL_frame_file( void);
void print_SRDL_deframing_log( void);

//
//  functions used for testing of deframing
//


//----------------------------------------------------------------------------------
