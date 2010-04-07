// all_frames.h
// common pieces of deframing programs for SRDL and HRDL links

// one should use D-switch when calling compiler:
// "cl ... /Dthis_is_PC_computer ..." on PC-computers
// "cc ... -Dthis_is_ALPHA_computer ..." on ALPHA-computers

#include "mylib.h"

#define TRUE  1
#define FALSE 0

#define OK   1
#define FAIL 0

int TRACE_LEVEL;

long nb_of_blocks_framed;
long nb_of_EOFs_framed;
long nb_of_frames_framed;
int EOF_written;
long nb_of_frames_written;
long nb_of_blocks_read;
long nb_of_event_blocks_read;
long nb_of_HKdat_blocks_read;
long nb_of_reply_blocks_read;

int EOF_encountered;

struct {
  struct {
    struct {
      int read;
      int with_SYNC_errors;
      int with_1553_errors;
      int with_CRC_errors;
      int with_deframing_errors;
      int with_deframing_warnings;
      int with_EOFs;
    } frames;
  } SRDL;
  struct {
    struct {
      int read;
      int with_SYNC_errors;
      int with_FS_errors;
    } frames;
  } HRDL;
  struct {
    struct {
      int read;
      int with_FS_warnings;
      int with_CRC_errors;
      int with_RS_errors;
      int with_RS_failures;
      int with_deframing_errors;
      int with_deframing_warnings;
      int with_EOFs;
    } frames;
  } data;
  struct {
    struct {
      int read;
      int with_FS_warnings;
      int with_CRC_errors;
      int with_RS_errors;
      int with_RS_failures;
      int with_deframing_errors;
      int with_deframing_warnings;
      int with_EOFs;
    } frames;
  } SW_fill;
  struct {
    struct {
      int read;
      int with_FS_warnings;
      int with_CRC_errors;
      int with_RS_errors;
      int with_RS_failures;
      int with_deframing_errors;
      int with_deframing_warnings;
      int with_EOFs;
    } frames;
  } HW_fill;
  struct {
    struct {
      int deframed;
      int written;
    } blocks;
  } event;
  struct {
    struct {
      int deframed;
      int written;
    } blocks;
  } HKdata;
  struct {
    struct {
      int deframed;
      int written;
    } blocks;
  } reply;
  struct {
    struct {
      int written;
      int failed;
    } blocks;
  } status;
  struct {
    int deframed;
    int written;
  } blocks;
  int type_6_blocks;
  int command_failures;
  int block_ID_decoding_failures;
} nb_of;

int   frames_fd;
FILE *frames;
FILE *events;
FILE *HKdata;
FILE *replies;

int frame_sync;
int do_not_write_AMS_block_file_PLS;

long frame_file_number;
long event_file_number;
long HKdat_file_number;
long reply_file_number;

char frame_path[256];
char frtmp_path[256];

char event_path[256];
char evtmp_path[256];

char HKdat_path[256];
char HKtmp_path[256];

char reply_path[256];
char retmp_path[256];

char b[2048];


//
//----- AMS block data structure description ----------------
//  (it is so funny to comply with MicroSoft C restrictions)
//

#define MAX_AMS_BLOCK_LENGTH 4194303     // 16-bit words

int32 actual_AMS_block_length;
int16 AMS_block[MAX_AMS_BLOCK_LENGTH+1];

#define       AMS_block_length  AMS_block[0]
#define       AMS_block_ID      AMS_block[1]
static int16 *AMS_block_data = &AMS_block[1];
static int16 *AMS_block_body = &AMS_block[2];
    
//
//----- yet another AMS block data structure description ----
//  (it is so funny to comply with MicroSoft C restrictions)
//

int32 actual_bl_length;
int16 bl[MAX_AMS_BLOCK_LENGTH+1];

#define       bl_length  bl[0]
#define       bl_ID      bl[1]
static int16 *bl_data = &bl[1];
static int16 *bl_body = &bl[2];

//----------------------------------------------------------------------------------
//
//---- declaration of functions defined in file all_frames.c
//

//
//  block writing functions
//

void open_AMS_block_tempfiles( void);
  int open_AMS_event_block_tempfile( void);
  int open_AMS_HKdat_block_tempfile( void);
  int open_AMS_reply_block_tempfile( void);
void process_AMS_block( void);
  int write_AMS_event_block_file( void);
  int write_AMS_HKdat_block_file( void);
  int write_AMS_reply_block_file( void);
void rename_AMS_block_tempfiles();
  int rename_AMS_event_block_tempfile( long file);
  int rename_AMS_HKdat_block_tempfile( long file);
  int rename_AMS_reply_block_tempfile( long file);
  
//
//  'library' functions
//

void redirect_stdout_to_pipe( char *pipe_file_name);
void redirect_stdout_to_file( char *file_name);
long last_file_number_in( char *directory_name);
char *mytime( void);

//
//  frame writing functions
//

int open_frame_tempfile( void);
void get_AMS_block( void);
int rename_frame_tempfile( long file);

//
//  block reading functions
//

int open_AMS_event_block_file( long file);
int open_AMS_HKdat_block_file( long file);
int open_AMS_reply_block_file( long file);
int read_AMS_HKdat_block_file( void);
int read_AMS_reply_block_file( void);
int read_AMS_event_block_file( void);
int close_AMS_HKdat_block_file( void);
int close_AMS_event_block_file( void);
int close_AMS_reply_block_file( void);

//
// block printing/checking Function
//

void print_AMS_block( void);

//----------------------------------------------------------------------------------
