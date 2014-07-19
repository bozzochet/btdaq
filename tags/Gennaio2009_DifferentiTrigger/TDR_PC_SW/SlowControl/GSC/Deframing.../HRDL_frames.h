// HRDL_frames.h
// pieces of deframing program for HRDL link

// one should use D-switch when calling compiler:
// "cl ... /Dthis_is_PC_computer ..." on PC-computers
// "cc ... -Dthis_is_ALPHA_computer ..." on ALPHA-computers

#include "all_frames.h"

#define data_frame_bit_mask  0x80000000
#define RS_encoding_bit_mask 0x40000000
#define JBU_number_bit_mask  0x000000F0
#define JL3_number_bit_mask  0x0000000F

int it_is_data_frame;
int it_is_SW_fill_frame;
int it_is_HW_fill_frame;
int RS_encoding_is_used;

//----------------------------------------------------------------------------------
//
//---- declaration of functions defined in file HRDL_frames.c
//

//
//  functions used for deframing
//

void initialize_everything( void);
long last_HRDL_frame_file_number( void);
int open_HRDL_frame_file( long file);
int read_HRDL_frame_file( void);
int decode_HRDL_frame( void);
int32 CRC32( void);
void deframe_HRDL_frame( void);
int close_HRDL_frame_file( void);
void print_HRDL_deframing_log( void);

//
//  functions used for testing of deframing
//

void get_HRDL_frame( void);
void frame_HRDL_frame( void);
void frame_HRDL_fill_frame( void);
void encode_HRDL_frame( void);
void introduce_error_in_HRDL_frame( int error_code, int not_more_than);
int write_HRDL_frame_file( void);
void print_HRDL_frame( void);

//----------------------------------------------------------------------------------
