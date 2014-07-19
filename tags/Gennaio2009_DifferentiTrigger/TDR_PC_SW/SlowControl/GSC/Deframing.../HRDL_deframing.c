// file HRDL_deframing.c
//
// (pre)production version of deframing program for HRDL link
// reads frame files and deframes them, writing blocks into block files
//
// this is supposed to be my main GSE program for HRDL deframing

#include "mylib.h"
#include "rslib.h"

#define NB_OF_DISKS 1

char *host = "pcX.hrdl";
//char *host = "pc1.hrdl";
//char *host = NULL;
char *user = "hropr";
char *pass = "hi!rate?";

char frame_directory[256];

#ifdef read_DDRS_file
char *remote_frame_directory = "/home/hropr/"; 
#else
char *remote_frame_directory = "/home/hropr/data/"; 
#endif
char *local_frame_directory  = "/dath1/HRDL/FRAMES/"; 

//char *disk_name[NB_OF_DISKS] = { "/dath1", "/dath2", "/dath3", "/stage" };

char *disk_name[5] = {
  "/dath1", "/dath3", "/pgm1", "/dats1", "/stage"
};

char *status_directory = "/stage/STATUS/";

char event_directory[81];
char HKdat_directory[81];
char reply_directory[81];

char *log_pipe_file_name = "/tmp/HRDL_deframing_log.pipe";
char *log_file_name      = "/stage/logs/HRDL_deframing_log.file";

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
//static int16 *AMS_block_body = &AMS_block[2];
    
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
//~============================================================================



//********************************************************************
//                                                                   *
//                                                                   *
//                      block writing functions                      *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

void open_AMS_block_tempfiles( void) {

  if (do_not_write_AMS_block_file_PLS) {
    if (TRACE_LEVEL > 0) printf( "Skipping started...\n");
  }
  else {
    open_AMS_event_block_tempfile();
    open_AMS_HKdat_block_tempfile();
    open_AMS_reply_block_tempfile();
  }
}

//--------------------------------------------------------------------

int open_AMS_event_block_tempfile( void) {

  extern char event_directory[];

  sprintf( evtmp_path, "%s!@#$%%^", event_directory);
  
  if ((events = fopen( evtmp_path, "wb")) == NULL) {
    printf( "fopen(%s,\"wb\") : %s\n", evtmp_path, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'wb' %s\n", evtmp_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int open_AMS_HKdat_block_tempfile( void) {

  extern char HKdat_directory[];
  
  sprintf( HKtmp_path, "%s!@#$%%^", HKdat_directory);

  if ((HKdata = fopen( HKtmp_path, "wb")) == NULL) {
    printf( "fopen(%s,\"wb\") : %s\n", HKtmp_path, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'wb' %s\n", HKtmp_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int open_AMS_reply_block_tempfile( void) {

  extern char reply_directory[];
  
  sprintf( retmp_path, "%s!@#$%%^", reply_directory);

  if ((replies = fopen( retmp_path, "wb")) == NULL) {
    printf( "fopen(%s,\"wb\") : %s\n", retmp_path, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'wb' %s\n", retmp_path);
    return OK;
  }
}

//--------------------------------------------------------------------

void process_AMS_block( void) {

  int16 block_type;

  block_type = AMS_block_ID >> 13;
  
  if (block_type == 0) {
    nb_of.event.blocks.deframed++;
    if (!do_not_write_AMS_block_file_PLS) {
      if (write_AMS_event_block_file() != OK) {
        printf( "event writing troubles\n");
        exit( 0);
      }
      else {
        nb_of.event.blocks.written++;
      }
    }
  }
  
  if (block_type == 4) {
    nb_of.HKdata.blocks.deframed++;
    if (!do_not_write_AMS_block_file_PLS) {
      if (write_AMS_HKdat_block_file() != OK) {
        printf( "HKdata writing troubles\n");
        exit( 0);
      }
      else {
        nb_of.HKdata.blocks.written++;
      }
    }
  }
    
  if (block_type != 0 && block_type != 4) {
    nb_of.reply.blocks.deframed++;
    if (!do_not_write_AMS_block_file_PLS) {
      if (write_AMS_reply_block_file() != OK) {
        printf( "reply writing troubles\n");
        exit( 0);
      }
      else {
        nb_of.reply.blocks.written++;
      }
    }
  }

}

//--------------------------------------------------------------------

int write_AMS_event_block_file(void) {

  if (fwrite( AMS_block,
              sizeof(int16), 
              actual_AMS_block_length+1, 
              events)
      == actual_AMS_block_length+1) {
    return OK;
  }
  else {
    return FAIL;
  }
}

//--------------------------------------------------------------------

int write_AMS_HKdat_block_file( void) {

  if (fwrite( AMS_block, sizeof(int16), AMS_block_length+1, HKdata)
      == AMS_block_length+1) {
    return OK;
  }
  else {
    return FAIL;
  }
}

//--------------------------------------------------------------------

int write_AMS_reply_block_file( void) {

  if (fwrite( AMS_block, sizeof(int16), AMS_block_length+1, replies)
      == AMS_block_length+1) {
    return OK;
  }
  else {
    return FAIL;
  }
}

//--------------------------------------------------------------------

void rename_AMS_block_tempfiles() {

  if (do_not_write_AMS_block_file_PLS) {
    if (TRACE_LEVEL > 0) printf(  "Skipping finished.\n");
  }
  else {
    rename_AMS_event_block_tempfile( frame_file_number);
    rename_AMS_HKdat_block_tempfile( frame_file_number);
    rename_AMS_reply_block_tempfile( frame_file_number);
  }
}

//--------------------------------------------------------------------

int rename_AMS_event_block_tempfile(long file) {

  extern char event_directory[];
  
  sprintf( event_path, "%s%6.6lu", event_directory, file);

  if (fclose( events)) {
    printf( "fclose(%s) : %s\n", evtmp_path, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", evtmp_path);
  }

  if (nb_of.event.blocks.written != 0) {
    if (rename( evtmp_path, event_path)) {
      printf( "rename(%s,%s) : %s\n",
               evtmp_path, event_path, strerror( errno));
      exit( 0);
    }
    else {
      if (TRACE_LEVEL > 1) printf( "rename -> %s\n", event_path);
      return OK;
    }
  }
  else {
    if (TRACE_LEVEL > 1) {
      printf( "empty:    %s - will be absent/unchanged\n", event_path);
    }
    return OK;
  }
}

//--------------------------------------------------------------------

int rename_AMS_HKdat_block_tempfile(long file) {

  extern char HKdat_directory[];
  
  sprintf( HKdat_path, "%s%6.6lu", HKdat_directory, file);

  if (fclose( HKdata)) {
    printf( "fclose(%s) : %s\n", HKtmp_path, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", HKtmp_path);
  }

  if (nb_of.HKdata.blocks.written != 0) {
    if (rename( HKtmp_path, HKdat_path)) {
      printf( " rename(%s,%s) : %s\n",
              HKtmp_path, HKdat_path, strerror( errno));
      exit( 0);
    }
    else {
      if (TRACE_LEVEL > 1) printf( "rename -> %s\n", HKdat_path);
      return OK;
    }
  }
  else {
    if (TRACE_LEVEL > 1) {
      printf( "empty:    %s - will be absent/unchanged\n", HKdat_path);
    }
    return OK;
  }
}

//--------------------------------------------------------------------

int rename_AMS_reply_block_tempfile(long file) {

  extern char reply_directory[];
  
  sprintf( reply_path, "%s%6.6lu", reply_directory, file);

  if (fclose( replies)) {
    printf( "fclose(%s) : %s\n", retmp_path, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", retmp_path);
  }

  if (nb_of.reply.blocks.written != 0) {
    if (rename( retmp_path, reply_path)) {
      printf( "rename(%s,%s) : %s\n", 
               retmp_path, reply_path, strerror( errno));
      exit( 0);
    }
    else {
      if (TRACE_LEVEL > 1) printf( "rename -> %s\n", reply_path);
      return OK;
    }
  }
  else {
    if (TRACE_LEVEL > 1) {
      printf( "empty:    %s - will be absent/unchanged\n", reply_path);
    }
    return OK;
  }
}

//--------------------------------------------------------------------


//********************************************************************
//                                                                   *
//                                                                   *
//                        "library" functions                        *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

void redirect_stdout_to_pipe( char *pipe_file_name) {

// - - - create (if necessary) and open the pipe for writing
  
  if (open( pipe_file_name, O_RDWR | O_NONBLOCK) < 0) {
    if (mknod( pipe_file_name, S_IFIFO | 0666, (dev_t) NULL)) {
      printf( "Failed to open '%s' pipe!\n", pipe_file_name);
      exit( 0);
    }
    else {
      if (open( pipe_file_name, O_RDWR | O_NONBLOCK) < 0) {
        printf( "Failed to open '%s' pipe!!\n", pipe_file_name);
        exit( 0);
      }
    }
  }

// - - - redirect stdout output to the pipe and set "no buffering" there

  setbuf( freopen( pipe_file_name, "w", stdout), NULL);

}

//--------------------------------------------------------------------

void redirect_stdout_to_file( char *file_name) {
  FILE *file;

  errno = 0;
  chmod( file_name, S_IRWXU | S_IRWXG | S_IRWXO);
  if ((file = freopen( file_name, "a", stdout)) == NULL) {
    printf( "freopen(%s) : %s\n", file_name, strerror( errno));
    exit( 0);
  }
  chmod( file_name, S_IRWXU | S_IRWXG | S_IRWXO);
  setbuf( file, NULL);

}

//--------------------------------------------------------------------

long last_file_number_in(char *directory_name) {

  long file_number, last_file_number;

  DIR  *dirp;
  struct dirent *dp;

  if (TRACE_LEVEL > 1) printf( "scan directory %s\n", directory_name);
  if ((dirp = opendir( directory_name)) == NULL) {
    printf( "opendir(%s) : %s\n", directory_name, strerror( errno));
    exit( 0);
  }
  last_file_number = 0;
  while ((dp = readdir( dirp)) != NULL) {
    if (fnmatch( "[0-9][0-9][0-9][0-9][0-9][0-9]", dp->d_name, 0) == 0) {
      file_number = atol( dp->d_name);
      if (file_number > last_file_number) last_file_number = file_number;
    }
  }
  if (closedir( dirp)) {
    printf( "closedir(%s) : %s\n", directory_name, strerror( errno));
    exit( 0);
  }

  return last_file_number;
}

//--------------------------------------------------------------------

char *mytime( void) {

  time_t ltime;
  char *t;
  static char tt[26];
  
  time( &ltime);
  t = ctime( &ltime);
  strcpy( tt, t);
  tt[24] = '\000';
  return tt;
}

//--------------------------------------------------------------------


//********************************************************************
//                                                                   *
//                                                                   *
//                      frame writing functions                      *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

int open_frame_tempfile( void) {


  sprintf( frtmp_path, "%s!@#$%%^", frame_directory);

  nb_of_frames_written = 0;
  if ((frames = fopen( frtmp_path, "wb")) == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "fopen(%s,\"rb\") : %s\n", frtmp_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'wb' %s\n", frtmp_path);
    return OK;
  }
}

//--------------------------------------------------------------------

void get_AMS_block( void) {

  int32 i;
  static long n = 0;
 
  bl_length = 2 * rand() + 1;
  bl_length = rand() / 4 + 1;
//bl_length = 2029/10;

  n++;

  if (n % 300000 == 0) {
    bl_length = 0;                         /* write EOF */
    EOF_written = TRUE;
    nb_of_EOFs_framed++;
    if (TRACE_LEVEL > 1) printf("please, write EOF\n");
    return;
  }

  nb_of_blocks_framed++;
  EOF_written = FALSE;

  bl_ID = nb_of_blocks_framed;
  bl_ID = frame_file_number;

//for (i=0; i<MAX_AMS_BLOCK_LENGTH-1; i++) bl_body[i] = 2 * rand() + 1;

  for (i=0; i<bl_length-1; i++) {
    bl_body[i] = i;
  }
}

//--------------------------------------------------------------------

int rename_frame_tempfile(long file) {

  
  sprintf( frame_path, "%s%6.6lu", frame_directory, file);

  if (fclose( frames)) {
    if (TRACE_LEVEL > 0) {
      printf( "fclose(%s) : %s\n", frtmp_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf("close     %s\n",frtmp_path);
  }

  if (nb_of_frames_written != 0) {
    if (rename( frtmp_path, frame_path)) {
      if (TRACE_LEVEL > 0) {
        printf( "rename(%s,%s) : %s\n",
            frtmp_path, frame_path, strerror( errno));
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) printf("rename -> %s\n",frame_path);
      return OK;
    }
  }
  else {
    if (TRACE_LEVEL > 1) {
      printf("empty:    %s - will be absent/unchanged\n",frame_path);
    }
    return OK;
  }
}

//--------------------------------------------------------------------


//********************************************************************
//                                                                   *
//                                                                   *
//                      block reading functions                      *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

int open_AMS_event_block_file(long file) {

  extern char event_directory[];
  
  sprintf( event_path, "%s%6.6ld", event_directory, file);

  nb_of_event_blocks_read = 0;
  if ((events = fopen( event_path, "rb")) == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "fopen(%s,\"rb\") : %s\n", event_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'rb' %s\n", event_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int open_AMS_HKdat_block_file(long file) {

  extern char HKdat_directory[];
  
  sprintf( HKdat_path, "%s%6.6ld", HKdat_directory, file);

  nb_of_HKdat_blocks_read = 0;
  if ((HKdata = fopen( HKdat_path, "rb")) == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "fopen(%s,\"rb\") : %s\n", HKdat_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'rb' %s\n", HKdat_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int open_AMS_reply_block_file(long file) {

  extern char reply_directory[];
  
  sprintf( reply_path, "%s%6.6ld", reply_directory, file);

  nb_of_reply_blocks_read = 0;
  if ((replies = fopen( reply_path, "rb")) == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "fopen(%s,\"rb\") : %s\n", reply_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'rb' %s\n", reply_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int read_AMS_HKdat_block_file( void) {

  nb_of_blocks_read++;
  nb_of_HKdat_blocks_read++;
  if (fread( &AMS_block_length, sizeof(int16), 1, HKdata) != 1) {
    return FAIL;
  }
  if (fread( AMS_block_data, sizeof(int16), AMS_block_length, HKdata) 
      != AMS_block_length) {
    return FAIL;
  }
  return OK;
}

//--------------------------------------------------------------------

int read_AMS_reply_block_file( void) {

  nb_of_blocks_read++;
  nb_of_reply_blocks_read++;
  if (fread( &AMS_block_length, sizeof(int16), 1, replies) != 1) {
    return FAIL;
  }
  if (fread( AMS_block_data, sizeof(int16), AMS_block_length, replies) 
      != AMS_block_length) {
    return FAIL;
  }
  return OK;
}

//--------------------------------------------------------------------

int read_AMS_event_block_file(void) {

  nb_of_blocks_read++;
  nb_of_event_blocks_read++;
  if (fread( &AMS_block[0], sizeof(int16), 2, events) != 2) {
    return FAIL;
  }
  actual_AMS_block_length = AMS_block_length;
  if ((AMS_block_ID & 0xFFC0) == 0) {
    actual_AMS_block_length |= (AMS_block_ID & 0x003F) << 16;
  }
  if (fread( &AMS_block[2],
             sizeof(int16), 
             actual_AMS_block_length-1, 
             events) 
      != actual_AMS_block_length-1) {
    return FAIL;
  }
  return OK;
}

//--------------------------------------------------------------------

int close_AMS_HKdat_block_file(void) {

  if (fclose( HKdata)) {
    if (TRACE_LEVEL > 0) {
      printf( "fclose(%s) : %s\n", HKdat_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", HKdat_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int close_AMS_event_block_file( void) {
  if (fclose( events)) {
    if (TRACE_LEVEL > 0) {
      printf( "fclose(%s) : %s\n", event_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", event_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int close_AMS_reply_block_file( void) {
  if (fclose( replies)) {
    if (TRACE_LEVEL > 0) {
      printf( "fclose(%s) : %s\n", reply_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", reply_path);
    return OK;
  }
}

//--------------------------------------------------------------------


//********************************************************************
//                                                                   *
//                                                                   *
//                  block printing/checking Function                 *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

void print_AMS_block( void) {

  int16 i;
//  static int old_nb = 0, n = 0;
//  int N=24000;

//if (AMS_block_length == 3083) return;
//if (nb_of_event_blocks_read < 262) return;
//if (nb_of_event_blocks_read > 264) exit(0);
//printf("%d\n",nb_of_event_blocks_read);
//exit(0);
//  printf("\n\n*** printing_AMS_block:...\n\n");
//  printf(" block.length = %u(10) = %8.8X(16)\n",
//           actual_AMS_block_length, actual_AMS_block_length);
//return;
//  printf(" block_data (16): \n");
//for (i=0; i<actual_AMS_block_length; i++) {
  for (i=0; i<16; i++) {
    printf("%4.4hX%c", AMS_block_data[i],
    (i%16 == 15 || i == actual_AMS_block_length-1) ? '\n' : ' ');
  }
return;

/*
  if (AMS_block_length != 3083) printf(" %6.6d l=%d\n",event_file_number,AMS_block_length);
  if (old_nb != AMS_block_data[3]) {
    if (n != 23) printf(" %6.6d n=%d\n",event_file_number,n);
    if (n == 23) printf("                                 %6.6d n=%d\n",event_file_number,n);
    n = 0;
    old_nb = AMS_block_data[3];
//  printf("\n");
//  printf("%4.4X ",AMS_block_data[3]);
  }
  else {
    n++;
//  printf("%4.4X ",AMS_block_data[3]);
  }
  
  if (n % 2 == 0) {
    if (AMS_block_data[0] != 0x0002) {
      printf(" %6.6d c=%4.4X (0002)\n",event_file_number,AMS_block_data[0]);
    }
  }
  else {
    if (AMS_block_data[0] != 0x1002) {
      printf(" %6.6d c=%4.4X (1002)\n",event_file_number,AMS_block_data[0]);
    }
  }
  if (AMS_block_data[1] != 2*(n/2)) {
      printf(" %6.6d t=%4.4X (%4.4X)\n",event_file_number,AMS_block_data[1],2*(n/2));
  }
  
  if (old_nb != AMS_block_data[1544]) printf(" %6.6d N=%d\n",event_file_number,AMS_block_data[1544]);
  if (AMS_block_data[1542] != 2*(n/2)+1) {
      printf(" %6.6d T=%4.4X (%4.4X)\n",event_file_number,AMS_block_data[1542],2*(n/2)+1);
  }
*/

/*  
i=1539;
printf("%d   ",i);
for( ;i<1548;i++) printf("%4.4X ", AMS_block_data[i]);
printf("\n");
*/

/*
  if (old_nb != AMS_block_data[3]) {
    n = 0;
    old_nb = AMS_block_data[3];
  }
  n++;
  if (n > N) return;
  for (i=0; i<16; i++) {
    printf("%4.4hX%c", AMS_block_data[i],
    (i%16 == 15 || i == AMS_block_length-1) ? '\n' : ' ');
  }
  if (n == N) {
    printf("the rest was thrown away...\n");
  }
  return;
*/
//  printf("...\n");
//  for (i=AMS_block_length-11; i<AMS_block_length; i++) {
//    printf("%4.4hX%c", AMS_block_data[i],
//    (i%16 == 15 || i == AMS_block_length-1) ? '\n' : ' ');
//  }
}

//~----------------------------------------------------------------------------

//
//----- HRDL frame data structure description ----------------
//  (it is so funny to be similar to that of AMS block below)
//

#define HRDL_FRAME_LENGTH_IN_BYTES 4080
int     HRDL_FRAME_BODY_LENGTH;

struct {
  int32 frame_sync_pattern;
  union {
    unsigned char bytes[HRDL_FRAME_LENGTH_IN_BYTES];
    int16     words[HRDL_FRAME_LENGTH_IN_BYTES/2];
    int32     wordl[HRDL_FRAME_LENGTH_IN_BYTES/4];
  } frame;
} HRDL;

#define        HRDL_frame_status                              HRDL.frame.wordl[   0]
#define        HRDL_frame_major_ID                            HRDL.frame.wordl[   1]
#define        HRDL_frame_minor_ID                            HRDL.frame.wordl[   2]
#define        HRDL_frame_first_block_link                    HRDL.frame.wordl[   3]
int16     *HRDL_frame_body = (int16 *)               &HRDL.frame.wordl[   4];
#define        HRDL_frame_CRC                                 HRDL.frame.wordl[1019]
unsigned char *HRDL_frame_parity_symbols = (unsigned char *) &HRDL.frame.wordl[ 892];

//--------------------------------------------------------------------
struct {
  int32 bit_mask;
  int32 votes;
} data_frame       = { 0x88888000 };

struct {
  int32 bit_mask;
  int32 votes;
} SW_fill_frame    = { 0x44444000 };

struct {
  int32 bit_mask;
  int32 votes;
} RS_encoded_frame = { 0x22222000 };

struct {
  int32 bit_mask;
  int32 votes;
} some_frame       = { 0x11111000 };
//--------------------------------------------------------------------
  int16 code_table[255] = {
    0xFF48, 0x0EC0, 0x9A0D, 0x70BC, 0x8E2C, 0x93AD, 0xA7B7, 0x46CE,
    0x5A97, 0x7DCC, 0x32A2, 0xBF3E, 0x0A10, 0xF188, 0x94CD, 0xEAB1,
    0xFE90, 0x1D81, 0x341A, 0xE179, 0x1C59, 0x275B, 0x4F6E, 0x8D9C,
    0xB52E, 0xFB98, 0x6545, 0x7E7C, 0x1421, 0xE311, 0x299B, 0xD563,
    0xFD20, 0x3B02, 0x6835, 0xC2F2, 0x38B2, 0x4EB6, 0x9EDD, 0x1B39,
    0x6A5D, 0xF730, 0xCA8A, 0xFCF8, 0x2843, 0xC622, 0x5337, 0xAAC7,
    0xFA40, 0x7604, 0xD06B, 0x85E4, 0x7164, 0x9D6D, 0x3DBA, 0x3672,
    0xD4BB, 0xEE61, 0x9515, 0xF9F0, 0x5087, 0x8C44, 0xA66F, 0x558F,
    0xF480, 0xEC09, 0xA0D7, 0x0BC8, 0xE2C9, 0x3ADA, 0x7B74, 0x6CE5,
    0xA977, 0xDCC3, 0x2A2B, 0xF3E0, 0xA10F, 0x1889, 0x4CDE, 0xAB1F,
    0xE901, 0xD813, 0x41AE, 0x1791, 0xC592, 0x75B4, 0xF6E8, 0xD9CB,
    0x52EF, 0xB986, 0x5457, 0xE7C1, 0x421E, 0x3112, 0x99BD, 0x563F,
    0xD203, 0xB026, 0x835C, 0x2F23, 0x8B24, 0xEB69, 0xEDD1, 0xB396,
    0xA5DF, 0x730C, 0xA8AF, 0xCF82, 0x843C, 0x6225, 0x337A, 0xAC7F,
    0xA407, 0x604D, 0x06B8, 0x5E47, 0x1649, 0xD6D3, 0xDBA3, 0x672D,
    0x4BBE, 0xE619, 0x515F, 0x9F05, 0x0878, 0xC44A, 0x66F5, 0x58FF,
    0x480E, 0xC09A, 0x0D70, 0xBC8E, 0x2C93, 0xADA7, 0xB746, 0xCE5A,
    0x977D, 0xCC32, 0xA2BF, 0x3E0A, 0x10F1, 0x8894, 0xCDEA, 0xB1FE,
    0x901D, 0x8134, 0x1AE1, 0x791C, 0x5927, 0x5B4F, 0x6E8D, 0x9CB5,
    0x2EFB, 0x9865, 0x457E, 0x7C14, 0x21E3, 0x1129, 0x9BD5, 0x63FD,
    0x203B, 0x0268, 0x35C2, 0xF238, 0xB24E, 0xB69E, 0xDD1B, 0x396A,
    0x5DF7, 0x30CA, 0x8AFC, 0xF828, 0x43C6, 0x2253, 0x37AA, 0xC7FA,
    0x4076, 0x04D0, 0x6B85, 0xE471, 0x649D, 0x6D3D, 0xBA36, 0x72D4,
    0xBBEE, 0x6195, 0x15F9, 0xF050, 0x878C, 0x44A6, 0x6F55, 0x8FF4,
    0x80EC, 0x09A0, 0xD70B, 0xC8E2, 0xC93A, 0xDA7B, 0x746C, 0xE5A9,
    0x77DC, 0xC32A, 0x2BF3, 0xE0A1, 0x0F18, 0x894C, 0xDEAB, 0x1FE9,
    0x01D8, 0x1341, 0xAE17, 0x91C5, 0x9275, 0xB4F6, 0xE8D9, 0xCB52,
    0xEFB9, 0x8654, 0x57E7, 0xC142, 0x1E31, 0x1299, 0xBD56, 0x3FD2,
    0x03B0, 0x2683, 0x5C2F, 0x238B, 0x24EB, 0x69ED, 0xD1B3, 0x96A5,
    0xDF73, 0x0CA8, 0xAFCF, 0x8284, 0x3C62, 0x2533, 0x7AAC, 0x7FA4,
    0x0760, 0x4D06, 0xB85E, 0x4716, 0x49D6, 0xD3DB, 0xA367, 0x2D4B,
    0xBEE6, 0x1951, 0x5F9F, 0x0508, 0x78C4, 0x4A66, 0xF558
  };
  
  unsigned char DDRS_table[256] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 
    0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 
    0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 
    0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 
    0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA, 
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 
    0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE, 
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 
    0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1, 
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 
    0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 
    0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD, 
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 
    0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB, 
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 
    0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 
    0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
  };

//--------------------------------------------------------------------


//********************************************************************
//                                                                   *
//                                                                   *
//                    functions used for deframing                   *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

void initialize_everything( void) {

  if (sizeof( int16) != 2) {
    printf("The size of 'int16' is wrong! Good bye!\n");
    exit( 0);
  }
  if (sizeof( int32) != 4) {
    printf("The size of 'int32' is wrong! Good bye!\n");
    exit( 0);
  }

  printf("\n\t********************************************* A.Lebedev ******\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*        HIGH RATE DATA LINK = HRDL DEFRAMING PROGRAM        *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                     trace level %d                          *\n",
  TRACE_LEVEL);  
  printf(  "\t*                                                            *\n");  
  printf(  "\t**************************************************************\n\n");

  frame_sync = FALSE;		// initialize the deframing process
  initialize_RS( 223);		// initialize RS decoding program

#if 0
  if (host != NULL) {
    printf( "Frame files will be read from host %s\n\n", host);
    sprintf( frame_directory, "%s", remote_frame_directory);
    if (Ftp_Connect( host, user, pass) == 1) {
      if (TRACE_LEVEL > 1) printf( "connected to host %s\n", host);
    }
    else {
      if (TRACE_LEVEL > 0) printf( "failed to connect to host %s\n", host);
    }
  }
  else {
    printf( "Frame files will be read from local host\n\n");
    sprintf( frame_directory, "%s", local_frame_directory);
  }
#endif

  printf( "Frame files will be read from local host\n\n");
  sprintf( frame_directory, "%s", local_frame_directory);
}

//--------------------------------------------------------------------

long last_HRDL_frame_file_number( void) {

#if 0
  if (host != NULL) {
    return Ftp_last_file_number_in(frame_directory);
  }
  else {
    return last_file_number_in(frame_directory);
  }
#endif

  return last_file_number_in(frame_directory);
}

//--------------------------------------------------------------------

int open_HRDL_frame_file(long file) {

  sprintf( frame_path, "%s%6.6lu", frame_directory, file);
  
  errno = 0;
  
#if 0
  if (host != NULL) {
    if ((frames_fd = Ftp_Open( frame_path)) < 0) {
      if (TRACE_LEVEL > 0) {
        printf( "Ftp_Open  %s  failed\n", frame_path);
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) printf( "Ftp_Open  %s  O.K.\n", frame_path);
      return OK;
    }
  }
  else {
    if ((frames_fd = open( frame_path, O_RDONLY)) < 0) {
      if (TRACE_LEVEL > 0) {
        printf( "open(%s,O_RDONLY) : %s\n", frame_path, strerror( errno));
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) printf( "open      %s\n", frame_path);
      return OK;
    }
  }
#endif

  if ((frames_fd = open( frame_path, O_RDONLY)) < 0) {
    if (TRACE_LEVEL > 0) {
      printf( "open(%s,O_RDONLY) : %s\n", frame_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open      %s\n", frame_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int read_HRDL_frame_file( void) {

  int i, left, l, ltot;
  unsigned char *pp;
  int16 *p;
  
  errno = 0;

  for (left=sizeof(HRDL), pp=(unsigned char *)&HRDL, l=left, ltot=0;
       left>0 && l>0;
       l=read(frames_fd,pp,left), left-=l, pp+=l, ltot+=l);

  if (ltot == sizeof(HRDL)) {

/********* this piece of code deals with DDRS bit swapping...

    for (pp=(unsigned char *)&HRDL;
         pp<(unsigned char *)&HRDL+sizeof(HRDL); 
         pp++) {
      (*pp) = *(DDRS_table + (*pp));		// DDRS bit swap...
    }
    
*********/

    i = 0;
    for (p=&HRDL.frame.words[0];
         p<&HRDL.frame.words[0]+HRDL_FRAME_LENGTH_IN_BYTES/2;
         p++) {
      (*p) = ((*p) >> 8) | ((*p) << 8);
      (*p) ^= code_table[i++];
      i %= 255;
    }
    
    HRDL.frame_sync_pattern = ((HRDL.frame_sync_pattern & 0x000000FF) << 24) |
                              ((HRDL.frame_sync_pattern & 0x0000FF00) <<  8) |
                              ((HRDL.frame_sync_pattern & 0x00FF0000) >>  8) |
                              ((HRDL.frame_sync_pattern & 0xFF000000) >> 24) ;
    nb_of.HRDL.frames.read++;
    return OK;
  }
  
  else {
    if (ltot > 0) {
      if (TRACE_LEVEL > 0) {
        printf( "file %s: last frame has only %d bytes.\n", frame_path, ltot);
      }
    }
    if (errno) {
      printf( "read(%s) : %s\n", frame_path, strerror( errno));
      printf( "Execution terminated.\n");
      exit( 0);
    }
    return FAIL;
  }
}

//--------------------------------------------------------------------

int decode_HRDL_frame( void) {

#define SWAP16(x) (x >> 16) | (x << 16)
#define VOTE_FOR( candidate) \
{ \
  int32 m; \
  int i; \
  m = HRDL__frame__status & candidate.bit_mask; \
  candidate.votes = 0; \
  for (i=0; i<32; i++) { \
    if (m & 1) candidate.votes++; \
    m >>= 1; \
  } \
}

  int32 FCS;
  int i, j, ii, nb_of_errors, status;
  unsigned char *s, *d;
  unsigned char code_block[255], parity_symbol[32];
  int32 HRDL__frame__status;

  HRDL__frame__status = SWAP16( HRDL_frame_status);
  
  if (TRACE_LEVEL > 2) {
    if ((nb_of.data.frames.read+nb_of.SW_fill.frames.read+nb_of.HW_fill.frames.read) <
         TRACE_LEVEL-1) {
      printf(" FIRST WORDS OF FEW FIRST FRAMES IN THIS FILE ARE:\n");
      printf(" sync     = %8.8X\n",        HRDL.frame_sync_pattern);
      printf(" status   = %8.8X\n",        HRDL__frame__status);
      printf(" major_ID = %8.8X\n", SWAP16(HRDL_frame_major_ID));
      printf(" minor ID = %8.8X\n", SWAP16(HRDL_frame_minor_ID));
      printf(" link     = %8.8X\n", SWAP16(HRDL_frame_first_block_link));
      for (i=0; i<16; i++) {
        printf("%4.4X ", HRDL_frame_body[i]);
      }
      printf("\n");
    }
  }
    
  if ((HRDL.frame_sync_pattern & 0xFFFFFF00) != 0xFAF32000) {
    if (TRACE_LEVEL > 0) {
      printf( "%s #%d: SYNC pattern error: HRDL.frame_sync_pattern=%8.8X (FAF320xx)\n",
              frame_path, 
              nb_of.data.frames.read + nb_of.SW_fill.frames.read + nb_of.HW_fill.frames.read,
              HRDL.frame_sync_pattern);
    }
    nb_of.HRDL.frames.with_SYNC_errors++;
    frame_sync = FALSE;
    goto FAILURE; 
  }
  it_is_data_frame    = FALSE;
  it_is_SW_fill_frame = FALSE;
  it_is_HW_fill_frame = FALSE;
  RS_encoding_is_used = FALSE;
  VOTE_FOR( data_frame);
  VOTE_FOR( SW_fill_frame);
  VOTE_FOR( RS_encoded_frame);
  if (data_frame.votes > 2 && SW_fill_frame.votes < 3) it_is_data_frame    = TRUE;
  if (data_frame.votes < 3 && SW_fill_frame.votes > 2) it_is_SW_fill_frame = TRUE;
  if (data_frame.votes < 3 && SW_fill_frame.votes < 3) it_is_HW_fill_frame = TRUE;
  if (data_frame.votes > 2 && SW_fill_frame.votes > 2) {
    if (data_frame.votes >  SW_fill_frame.votes) it_is_data_frame    = TRUE;
    if (data_frame.votes <  SW_fill_frame.votes) it_is_SW_fill_frame = TRUE;
    if (data_frame.votes == SW_fill_frame.votes) {
      nb_of.HRDL.frames.with_FS_errors++;
      frame_sync = FALSE;
      if (TRACE_LEVEL > 0) {
        printf( "%s #%d: frame status error: HRDL__frame__status=%8.8X\n",
                frame_path, 
                nb_of.data.frames.read + nb_of.SW_fill.frames.read + nb_of.HW_fill.frames.read,
                HRDL__frame__status);
      }
      goto FAILURE;
    }
  }
  if (it_is_data_frame)    nb_of.data.frames.read++;
  if (it_is_SW_fill_frame) nb_of.SW_fill.frames.read++;
  if (it_is_HW_fill_frame) nb_of.HW_fill.frames.read++;
  if (RS_encoded_frame.votes > 3) RS_encoding_is_used = TRUE;
  if ((data_frame.votes       != 0 && data_frame.votes       != 5) ||
      (SW_fill_frame.votes    != 0 && SW_fill_frame.votes    != 5) ||
      (RS_encoded_frame.votes != 0 && RS_encoded_frame.votes != 5) ||
      (it_is_HW_fill_frame         && HRDL__frame__status    != 0)) {
    if (it_is_data_frame)    nb_of.data.frames.with_FS_warnings++;
    if (it_is_SW_fill_frame) nb_of.SW_fill.frames.with_FS_warnings++;
    if (it_is_HW_fill_frame) nb_of.HW_fill.frames.with_FS_warnings++;
  }
//--
  if (it_is_HW_fill_frame) goto SUCCESS;
  
  if (RS_encoding_is_used) {
    nb_of_errors = 0;
    for (j=0; j<16; j++) {
      d = code_block;
      s = &HRDL.frame.bytes[j];
      for (i=0; i<255; i++) {
        *(d++) = *s;
        s += 16;
      }
      encode_RS( &code_block[0], &parity_symbol[0]);
      status = 0;
      for (ii=0; ii<32; ii++) if (code_block[223+ii] != parity_symbol[ii]) status++;
      if (status) {
        status = decode_RS( &code_block[0]);
        if (status == -1) {
          if (TRACE_LEVEL > 0) printf("RS decoder failed\n");
          if (it_is_data_frame)    nb_of.data.frames.with_RS_failures++;
          if (it_is_SW_fill_frame) nb_of.SW_fill.frames.with_RS_failures++;
          if (it_is_HW_fill_frame) nb_of.HW_fill.frames.with_RS_failures++;
          frame_sync = FALSE;
          goto FAILURE;
        }
        nb_of_errors += status;
        s = code_block;
        d = &HRDL.frame.bytes[j];
        for (i=0; i<255; i++) {
          *d = *(s++);
          d += 16;
        }
      }
    }
    if (nb_of_errors) {
      if (TRACE_LEVEL > 0) printf("RS decoder corrected %d error(s)/frame\n", nb_of_errors);
      if (it_is_data_frame)    nb_of.data.frames.with_RS_errors++;
      if (it_is_SW_fill_frame) nb_of.SW_fill.frames.with_RS_errors++;
      if (it_is_HW_fill_frame) nb_of.HW_fill.frames.with_RS_errors++;
    }
    goto SUCCESS;
  }
  else {
    HRDL_frame_CRC = SWAP16( HRDL_frame_CRC);
    if ((FCS = CRC32()) != HRDL_frame_CRC) {
      if (TRACE_LEVEL > 0) {
        printf( "%s #%d: CRC32 error: HRDL_frame_CRC=%8.8X FCS=%8.8X HRDL_frame_status=%8.8X\n",
                frame_path,
                nb_of.data.frames.read + nb_of.SW_fill.frames.read + nb_of.HW_fill.frames.read,
                HRDL_frame_CRC, FCS, SWAP16(HRDL_frame_status));
      }
      if (it_is_data_frame)    nb_of.data.frames.with_CRC_errors++;
      if (it_is_SW_fill_frame) nb_of.SW_fill.frames.with_CRC_errors++;
      if (it_is_HW_fill_frame) nb_of.HW_fill.frames.with_CRC_errors++;
      frame_sync = FALSE;
      goto FAILURE;
    }
    goto SUCCESS;
  }

FAILURE:
  HRDL_frame_status           = SWAP16( HRDL_frame_status);
  HRDL_frame_major_ID         = SWAP16( HRDL_frame_major_ID);
  HRDL_frame_minor_ID         = SWAP16( HRDL_frame_minor_ID);
  HRDL_frame_first_block_link = SWAP16( HRDL_frame_first_block_link);
  return FAIL;

SUCCESS:  
  HRDL_frame_status           = SWAP16( HRDL_frame_status);
  HRDL_frame_major_ID         = SWAP16( HRDL_frame_major_ID);
  HRDL_frame_minor_ID         = SWAP16( HRDL_frame_minor_ID);
  HRDL_frame_first_block_link = SWAP16( HRDL_frame_first_block_link);
  return OK;
}

//--------------------------------------------------------------------

/****************************************************************************/
/*                                                                          */
/*  cyclic redundancy check source was "borrowed" from VK (and "reworked")  */
/*                                                                          */
/****************************************************************************/

int32 CRC32( void) {

  int32 *p, d, FCS;
  int32 g = 0x04C11DB7;
  int i;

  FCS = 0xFFFFFFFF;

  for (p=&HRDL_frame_status; p<&HRDL_frame_CRC; p++) {
    d = ((*p) >> 16) | ((*p) << 16 );
    for (i=0; i<32; i++) {
      if ((FCS ^ d) & 0x80000000) FCS = (FCS << 1) ^ g;
      else                        FCS = (FCS << 1);
      d <<= 1;
    }
  }

  FCS = FCS ^ 0xFFFFFFFF;		// for some reason Dr.Cai inverts it
  
  return FCS; 
}

//--------------------------------------------------------------------

void deframe_HRDL_frame( void) {

#define E(msg) \
  if (TRACE_LEVEL > 0) \
  printf( "%s #%d: deframing error '%s': HRDL_frame_ID=%8.8X,%8.8X Status=%8.8X\n", \
          frame_path, \
          nb_of.data.frames.read \
          + nb_of.SW_fill.frames.read \
          + nb_of.HW_fill.frames.read, \
          msg, HRDL_frame_major_ID, HRDL_frame_minor_ID, HRDL_frame_status); \
  if (it_is_data_frame)    nb_of.data.frames.with_deframing_errors++; \
  if (it_is_SW_fill_frame) nb_of.SW_fill.frames.with_deframing_errors++; \
  if (it_is_HW_fill_frame) nb_of.HW_fill.frames.with_deframing_errors++;

#define W(msg) \
  if (TRACE_LEVEL > 0) \
  printf( "%s #%d: deframing warning '%s': HRDL_frame_ID=%8.8X,%8.8X Status=%8.8X\n", \
          frame_path, \
          nb_of.data.frames.read \
          + nb_of.SW_fill.frames.read \
          + nb_of.HW_fill.frames.read, \
          msg, HRDL_frame_major_ID, HRDL_frame_minor_ID, HRDL_frame_status); \
  if (it_is_data_frame)    nb_of.data.frames.with_deframing_warnings++; \
  if (it_is_SW_fill_frame) nb_of.SW_fill.frames.with_deframing_warnings++; \
  if (it_is_HW_fill_frame) nb_of.HW_fill.frames.with_deframing_warnings++;
  
  static int       block_open;
  int16        frame_index;
  static int32 block_index;
  int32        expected_first_block_link;
  static int32 old_frame_major_ID;
  static int32 old_frame_minor_ID;
/*  
if (  HRDL_frame_minor_ID > 0x3478)
printf("%8.8X %8.8X \n", HRDL_frame_minor_ID, HRDL_frame_status);
*/
  if (!it_is_data_frame) return;	// do nothing with both SW & HW fill frames
        
  if (TRACE_LEVEL > 2) {
    if ((nb_of.data.frames.read
         + nb_of.SW_fill.frames.read
         + nb_of.HW_fill.frames.read) <
         TRACE_LEVEL-1) {
      int i;
      printf(" FIRST WORDS OF FEW FIRST NON-FILL FRAMES IN THIS FILE ARE:\n");
      printf(" sync     = %8.8X\n",HRDL.frame_sync_pattern);
      printf(" status   = %8.8X\n",HRDL_frame_status);
      printf(" major_ID = %8.8X\n",HRDL_frame_major_ID);
      printf(" minor ID = %8.8X\n",HRDL_frame_minor_ID);
      printf(" link     = %8.8X\n",HRDL_frame_first_block_link);
      for (i=0; i<16; i++) {
        printf("%4.4X ", HRDL_frame_body[i]);
      }
      printf("\n");
    }
  }
  
  EOF_encountered = FALSE;

  if (RS_encoding_is_used) {
    HRDL_FRAME_BODY_LENGTH 
    = (int16 *)HRDL_frame_parity_symbols - HRDL_frame_body;
  }
  else {
    HRDL_FRAME_BODY_LENGTH 
    = (int16 *)&HRDL_frame_CRC - HRDL_frame_body;
  }

  if (frame_sync && (old_frame_major_ID) != HRDL_frame_major_ID) {
    W("frame major_ID changed");
    frame_sync = FALSE;
  }

  if (frame_sync && (++old_frame_minor_ID) != HRDL_frame_minor_ID) {
    E("wrong frame sequence");
//printf("expect=%8.8X   actual=%8.8X\n", old_frame_minor_ID , HRDL_frame_minor_ID  );
    frame_sync = FALSE;
  }

  old_frame_major_ID = HRDL_frame_major_ID;
  old_frame_minor_ID = HRDL_frame_minor_ID;
  if (frame_sync) {
    frame_index = 0;
    if (block_open) {
      expected_first_block_link = actual_AMS_block_length - block_index;
      if (expected_first_block_link < HRDL_FRAME_BODY_LENGTH) {
        if (HRDL_frame_first_block_link != expected_first_block_link) {
          E("invalid link found!");
/////////////////
///////////////// remove this junk printf sometime
/////////////////
printf("Cai's link=%d, my link=%d\n", HRDL_frame_first_block_link  ,  expected_first_block_link );    
          frame_sync = FALSE;
          block_open = FALSE;
          return;
        }
      }
      else {
        if (HRDL_frame_first_block_link != 0xFFF) {
          E("invalid link found!!");
          frame_sync = FALSE;
          block_open = FALSE;
          return;
        }
      }
    }
  }
  else {
    if (HRDL_frame_first_block_link == 0xFFF) {
      W("frame is not locked");
      return;
    }
    if (HRDL_frame_first_block_link >= HRDL_FRAME_BODY_LENGTH) {
      W("invalid link found");
      return;
    }
    else {
      frame_index = HRDL_frame_first_block_link;
      frame_sync = TRUE;
      if (block_open) {
        block_open = FALSE;
      }
    }
  }

  LOOP:
    if (!block_open) {
/*    
{ 
int i; 
printf("%d : ",frame_index);
for (i=0;i<10;i++) printf("%4.4X ",HRDL_frame_body[frame_index+i]); 
printf("\n"); 
}
*/    
    AMS_block_length = HRDL_frame_body[frame_index++];
    actual_AMS_block_length = AMS_block_length;
//printf("%d : %d", frame_index, actual_AMS_block_length ); 
    if ((HRDL_frame_body[frame_index] & 0xFFC0) == 0) {
      actual_AMS_block_length |= (HRDL_frame_body[frame_index] & 0x003F) << 16;
    }
//printf(" --> %d\n", actual_AMS_block_length ); exit(0);
    if (actual_AMS_block_length == 0) {
//////////  if ((AMS_block_length = HRDL_frame_body[frame_index++]) == 0) {
//
// ****************** here is an error !!!  ****************************
//
// ****************** sometime X.Cai should fix it *********************
//
//
//////////    AMS_block_length = HRDL_frame_body[frame_index++] + 1;
//////////    AMS_block_length = HRDL_frame_body[frame_index++] * 2 - 1;
//////////    AMS_block_length = HRDL_frame_body[frame_index++] - 1;
//////////    if (AMS_block_length == 0) {
        W("EOF encountered");
        if (it_is_data_frame)    nb_of.data.frames.with_EOFs++; \
        if (it_is_SW_fill_frame) nb_of.SW_fill.frames.with_EOFs++; \
        if (it_is_HW_fill_frame) nb_of.HW_fill.frames.with_EOFs++; \
        EOF_encountered = TRUE;
        frame_sync = FALSE;
        return;
      }
      if (actual_AMS_block_length > MAX_AMS_BLOCK_LENGTH) {
        E("tooo big block rejected");
        frame_sync = FALSE;
        return;
      }
      block_open = TRUE;
      block_index = 0;
    }
    while (block_index < actual_AMS_block_length 
        && frame_index < HRDL_FRAME_BODY_LENGTH) {
      AMS_block_data[block_index++] = HRDL_frame_body[frame_index++];
    }   
    if (block_index == actual_AMS_block_length) {
      block_open = FALSE;
      process_AMS_block();
    }
    if (block_index > actual_AMS_block_length) {
      E("impossible situation!");
      frame_sync = FALSE;
      return;
    }
    if (frame_index == HRDL_FRAME_BODY_LENGTH) {
      return;
    }
    if (frame_index > HRDL_FRAME_BODY_LENGTH) {
      E("impossible situation!!");
      frame_sync = FALSE;
      return;
    }
  goto LOOP;
}    

//--------------------------------------------------------------------

int close_HRDL_frame_file( void) {

  errno = 0;
  
#if 0
  if (host != NULL) {
    if (Ftp_Close( frames_fd) != 0) {
      if (TRACE_LEVEL > 0) {
        printf( "Ftp_Close %s  failed\n", frame_path);
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) {
        printf( "Ftp_Close %s  O.K.\n", frame_path);
      }
      return OK;
    }
  }
  else {
    if (close( frames_fd) < 0) {
      if (TRACE_LEVEL > 0) {
        printf( "close(%s) : %s\n", frame_path, strerror( errno));
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) printf( "close     %s\n", frame_path);
      return OK;
    }
  }
#endif

  if (close( frames_fd) < 0) {
    if (TRACE_LEVEL > 0) {
      printf( "close(%s) : %s\n", frame_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", frame_path);
    return OK;
  }
}

//--------------------------------------------------------------------

void print_HRDL_deframing_log( void) {

      printf( "%s * %s : RD=%d=%d=%d+%d+%d SYNCe=%d FSe=%d FSw=%d=%d+%d+%d CRCe=%d=%d+%d DFe=%d DFw=%d DF=%d=%d+%d+%d WR=%d=%d+%d+%d \n",
      
          mytime(),
          frame_path,
          
          nb_of.HRDL.frames.read,
          nb_of.data.frames.read
          + nb_of.SW_fill.frames.read 
          + nb_of.HW_fill.frames.read,
          nb_of.data.frames.read, 
          nb_of.SW_fill.frames.read, 
          nb_of.HW_fill.frames.read,
          
          nb_of.HRDL.frames.with_SYNC_errors,
         
          nb_of.HRDL.frames.with_FS_errors,
          
          nb_of.data.frames.with_FS_warnings
          + nb_of.SW_fill.frames.with_FS_warnings
          + nb_of.HW_fill.frames.with_FS_warnings,
          nb_of.data.frames.with_FS_warnings,
          nb_of.SW_fill.frames.with_FS_warnings,
          nb_of.HW_fill.frames.with_FS_warnings,

          nb_of.data.frames.with_CRC_errors
          + nb_of.SW_fill.frames.with_CRC_errors 
          + nb_of.HW_fill.frames.with_CRC_errors,
          nb_of.data.frames.with_CRC_errors, 
          nb_of.SW_fill.frames.with_CRC_errors, 
          
          nb_of.data.frames.with_deframing_errors
          + nb_of.SW_fill.frames.with_deframing_errors 
          + nb_of.HW_fill.frames.with_deframing_errors,
          
          nb_of.data.frames.with_deframing_warnings
          + nb_of.SW_fill.frames.with_deframing_warnings 
          + nb_of.HW_fill.frames.with_deframing_warnings,
          
          nb_of.event.blocks.deframed
          + nb_of.HKdata.blocks.deframed 
          + nb_of.reply.blocks.deframed,
          nb_of.event.blocks.deframed,
          nb_of.HKdata.blocks.deframed,
          nb_of.reply.blocks.deframed,
          
          nb_of.event.blocks.written
          + nb_of.HKdata.blocks.written 
          + nb_of.reply.blocks.written,
          nb_of.event.blocks.written,
          nb_of.HKdata.blocks.written,
          nb_of.reply.blocks.written);
}

//--------------------------------------------------------------------


//********************************************************************
//                                                                   *
//                                                                   *
//              functions used for testing of deframing              *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

void get_HRDL_frame( void) {
//  int i;
  RS_encoding_is_used = TRUE;
  frame_HRDL_frame();
  encode_HRDL_frame();

//   introduce some errors for RS decoder checks:
//
//   for (i=0; i<1; i++) HRDL.frame.bytes[123+i] = ~HRDL.frame.bytes[123+i];
//   for (i=0; i<16; i++) HRDL.frame.bytes[123+i] = ~HRDL.frame.bytes[123+i];
//   for (i=0; i<256; i++) HRDL.frame.bytes[123+i] = ~HRDL.frame.bytes[123+i];

//if (file == 2 && frame == 0) introduce_error_in_HRDL_frame( 1, 0);
//if (file == 1) introduce_error_in_HRDL_frame( 2, 1);
//if (file == 1) introduce_error_in_HRDL_frame( 3, 1);
}

//--------------------------------------------------------------------

void frame_HRDL_frame( void) {

#define FRAME_HEADER ((HK_frame_ID << 8) | HK_frame_first_block_link)

  static int block_open = FALSE;
  int16 frame_index;
  static int32 block_index;
  
  nb_of_frames_framed++;
  
  HRDL_frame_status           = 0x01234568L;
  HRDL_frame_major_ID         = 0xABCDEF01L;
  HRDL_frame_minor_ID++;
  HRDL_frame_first_block_link = 0xFFF;
  frame_index = 0;

  if (RS_encoding_is_used) {
    HRDL_frame_status = HRDL_frame_status | RS_encoding_bit_mask;
    HRDL_FRAME_BODY_LENGTH 
    = (int16 *)HRDL_frame_parity_symbols - HRDL_frame_body;
  }
  else {
    HRDL_frame_status = HRDL_frame_status & ~RS_encoding_bit_mask;
    HRDL_FRAME_BODY_LENGTH 
    = (int16 *)&HRDL_frame_CRC - HRDL_frame_body;
  }

LOOP:
  if (!block_open) {
    if (HRDL_frame_first_block_link == 0xFFF) {
      HRDL_frame_first_block_link = frame_index;
    }
    get_AMS_block();
    if ((HRDL_frame_body[frame_index++] = bl_length) == 0) {
      return;
    }
    block_index = 0;
    block_open = TRUE;
  }
  while (block_index < bl_length && frame_index < HRDL_FRAME_BODY_LENGTH) {
    HRDL_frame_body[frame_index++] = bl_data[block_index++];
  }   
  if (block_index >= bl_length) {
    block_open = FALSE;
  }
  if (frame_index >= HRDL_FRAME_BODY_LENGTH) {
    return;
  }
goto LOOP;
}    

//--------------------------------------------------------------------

void frame_HRDL_fill_frame( void) {
  int16 frame_index;
  nb_of_frames_framed++;
  HRDL_frame_status           = 0x01234567L;
  HRDL_frame_major_ID         = 0xABCDEF01L;
//HRDL_frame_minor_ID++;  // should we have correct minor_ID for fill frame?
  HRDL_frame_first_block_link = 0xFFF;
  for (frame_index = 0; frame_index < HRDL_FRAME_BODY_LENGTH; frame_index++) {
    HRDL_frame_body[frame_index] = 0;
  }
  HRDL_frame_CRC              = 0x76543210L;
}

//--------------------------------------------------------------------

void encode_HRDL_frame( void) {
  int i, j, k;
  unsigned char code_block[255];

  if (RS_encoding_is_used) {
    for (j=0; j<16; j++) {
      k = 0;
      for (i=j; i<16*255; i+=16) {
        code_block[k++] = HRDL.frame.bytes[i];
      }
      encode_RS( &code_block[0], &code_block[223]);
      k = 0;
      for (i=j; i<16*255; i+=16) {
        HRDL.frame.bytes[i] = code_block[k++];
      }
    }
  }
  else {
    HRDL_frame_CRC = 0x76543210L;  // should be changed sometime
  }
}

//--------------------------------------------------------------------

void introduce_error_in_HRDL_frame(int error_code, int not_more_than) {

  static unsigned long error_count_2 = 0, error_count_3 = 0;
  
  if (error_code == 1) {
    printf("-1- %u %X -> ", HRDL_frame_minor_ID, HRDL_frame_first_block_link);
    HRDL_frame_first_block_link++;
    printf("%X\n", HRDL_frame_first_block_link);
  }
  if ((error_code == 2) && (error_count_2 < not_more_than)) {
    if (HRDL_frame_first_block_link == 0xFFF) {
      printf("-2- %u %X -> ",HRDL_frame_minor_ID,HRDL_frame_first_block_link);
      HRDL_frame_first_block_link = 0x123;
      printf("%X\n",HRDL_frame_first_block_link);
      error_count_2++;
    }
  }
  if ((error_code == 3) && (error_count_3 < not_more_than)) {
    if (HRDL_frame_first_block_link != 0xFFF) {
      printf("-3- %u %X -> ",HRDL_frame_minor_ID,HRDL_frame_first_block_link);
      HRDL_frame_first_block_link = 0xFFF;
      printf("%u\n",HRDL_frame_first_block_link);
      error_count_3++;
    }
  }
}

//--------------------------------------------------------------------

int write_HRDL_frame_file( void) {
  nb_of_frames_written++;
  if (fwrite( HRDL.frame.bytes, sizeof( char), HRDL_FRAME_LENGTH_IN_BYTES, frames)
      == HRDL_FRAME_LENGTH_IN_BYTES) {
    return OK;
  }
  else {
    return FAIL;
  }
}

//--------------------------------------------------------------------

void print_HRDL_frame( void) {
  int i, j;
  
//  if (HRDL_frame_first_block_link == 0x00000FFF) return;
//  if (HRDL_frame_minor_ID != 0x000053F0) return;
  if (HRDL_frame_minor_ID > 0x000053F0) exit(0);

  printf(" status   = %8.8X\n", HRDL_frame_status);
  printf(" major_ID = %8.8X\n", HRDL_frame_major_ID);
  printf(" minor_ID = %8.8X\n", HRDL_frame_minor_ID);
  printf(" link     = %8.8X\n", HRDL_frame_first_block_link);

  for (j=0; j<HRDL_FRAME_LENGTH_IN_BYTES/2-8; j+=16) {
    printf( "%4.4X: ", j);
    for (i=j; i<j+16; i++) printf( "%4.4X ", HRDL_frame_body[i]);
    printf( "\n");
  }
  
  return;

  for (i=0; i<16; i++) printf("%4.4X ", HRDL_frame_body[i]);
  printf("...\n");
  for (i=16; i>0; i--) printf("%4.4X ", HRDL_frame_body[HRDL_FRAME_BODY_LENGTH-i]);

  if (RS_encoding_is_used) {
    for (i=0; i<24; i++) printf("%2.2X ", HRDL_frame_parity_symbols[i]);
    printf("\n...\n");
    for (i=16*32-24; i<16*32; i++) printf("%2.2X ", HRDL_frame_parity_symbols[i]);
    printf("\n");
  }
  
  printf("\n");
  printf(" CRC      = %8.8X\n", HRDL_frame_CRC);
  
/*
#ifdef this_is_ALPHA_computer
  for (j=0; j<HRDL_FRAME_LENGTH_IN_BYTES/4; j++) {
    printf("%8.8X%c", HRDL.frame[j],
    (j%8 == 7 || j == HRDL_FRAME_LENGTH_IN_BYTES/4-1) ? '\n' : ' ');
  }
#endif

#ifdef this_is_PC_computer
  for (j=0; j<HRDL_FRAME_LENGTH_IN_BYTES/4; j++) {
    printf("%8.8lX%c", HRDL.frame[j],
    (j%8 == 7 || j == HRDL_FRAME_LENGTH_IN_BYTES/4-1) ? '\n' : ' ');
  }
#endif

*/
}

//--------------------------------------------------------------------


//~----------------------------------------------------------------------------

//~----------------------------------------------------------------------------

int main( int argc, char *argv[]) {

  int disk;
  long last_file[3];
  long last_event_file[3], last_HKdat_file[3], last_reply_file[3];
  clock_t t1, t2;
  time_t T1, T2;
  int i;
  
  setbuf( stdout, NULL);	// set "no buffering" for stdout

  TRACE_LEVEL = 0;
  if (argc > 3) TRACE_LEVEL = atoi( argv[3]);
  if (TRACE_LEVEL > 9) TRACE_LEVEL = 9;

  printf("\n\t**************************************************************\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                 A.Lebedev  10-Apr-98 13:38                 *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*        HIGH RATE DATA LINK = HRDL DEFRAMING PROGRAM        *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                       version 0.9                          *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t**************************************************************\n\n");

#ifndef do_not_use_pipe
//printf( "\n\n\t\t see my output in '%s'\n", log_pipe_file_name);
//redirect_stdout_to_pipe( log_pipe_file_name);
  printf( "\n\n\t\t see my output in '%s'\n", log_file_name);
  redirect_stdout_to_file( log_file_name);
#endif
  
  if (TRACE_LEVEL > 1) printf( "Third parameter set TRACE_LEVEL = %d.", TRACE_LEVEL);

  initialize_everything();

  if (TRACE_LEVEL > 1) printf( "Scanning disks for block file writing...\n");
  disk = 1;
  frame_file_number = 0;
  for (i=0; i<NB_OF_DISKS; i++) {
    sprintf( event_directory, "%s/HRDL/EVENTS/", disk_name[i]);
    sprintf( HKdat_directory, "%s/HRDL/HKDATA/", disk_name[i]);
    sprintf( reply_directory, "%s/HRDL/REPLIES/", disk_name[i]);
    last_event_file[i] = last_file_number_in( event_directory);
    last_HKdat_file[i] = last_file_number_in( HKdat_directory);
    last_reply_file[i] = last_file_number_in( reply_directory);
    last_file[i] = 0;
    if (last_event_file[i] > last_file[i]) last_file[i] = last_event_file[i];
    if (last_HKdat_file[i] > last_file[i]) last_file[i] = last_HKdat_file[i];
    if (last_reply_file[i] > last_file[i]) last_file[i] = last_reply_file[i];
    if (last_file[i] > frame_file_number) {
      disk = i + 1;
      frame_file_number = last_file[i];
    }
    if (TRACE_LEVEL > 1) {
      printf( "  disk %d (%s), last_file %6.6ld\n",
               i+1, disk_name[i], last_file[i]);
    }
  }
  
  if (TRACE_LEVEL > 1) {
    printf( "    Want to read frame file %6.6ld and write on disk %d (%s)\n",
                 frame_file_number, disk, disk_name[disk-1]);
  }
  
  if (argc > 1) {
    i = atoi( argv[1]);
    if (i > 0 && i <= NB_OF_DISKS) {
      disk = i;
      if (TRACE_LEVEL > 1) {
        printf( "First parameter forced to use disk %d (%s)\n",
                 disk, disk_name[disk-1]);
      }
    }
  }
  
  if (argc > 2) {
    i = atoi( argv[2]);
    if (i > 0 && i <= 999999) {
      frame_file_number = i;
      if (TRACE_LEVEL > 1) {
        printf( "Second parameter forced to read frame file %6.6ld\n",
                 frame_file_number);
      }
    }
  }
  
  sprintf( event_directory, "%s/HRDL/EVENTS/", disk_name[disk-1]);
  sprintf( HKdat_directory, "%s/HRDL/HKDATA/", disk_name[disk-1]);
  sprintf( reply_directory, "%s/HRDL/REPLIES/", disk_name[disk-1]);
  
  if (frame_file_number == 0) {
    frame_file_number = 1;
    do_not_write_AMS_block_file_PLS = FALSE;
  }
  else {
    do_not_write_AMS_block_file_PLS = TRUE;
  }
  
  printf( "\nFirst frame file to read   = %s%6.6ld\n",  frame_directory, frame_file_number);
  printf(   "First event file to write  = %s%6.6ld\n",  event_directory, frame_file_number+1);
  printf(   "First HKdata file to write = %s%6.6ld\n",  HKdat_directory, frame_file_number+1);
  printf(   "First reply file to write  = %s%6.6ld\n\n",reply_directory, frame_file_number+1);

LOOP:
  for ( ;
       frame_file_number <= last_HRDL_frame_file_number();
       frame_file_number++) {
    t1 = clock();
    T1 = time( NULL);
    if (open_HRDL_frame_file( frame_file_number) == OK) {
      bzero( &nb_of, sizeof( nb_of));
      open_AMS_block_tempfiles();
      while (read_HRDL_frame_file() == OK) {
        if (decode_HRDL_frame() == OK) {
          deframe_HRDL_frame();
        }
      }
      rename_AMS_block_tempfiles();
      close_HRDL_frame_file();
      print_HRDL_deframing_log();
    }
    do_not_write_AMS_block_file_PLS = FALSE;

    t2 = clock();
    T2 = time( NULL);
    if (TRACE_LEVEL > 0) {
      if (t2 != t1) {
        printf( "Deframing rate = %.2f Mbit/sec (CPU time)", 
                4080.0 * 8.0 / 1000000.0 * 
                (nb_of.data.frames.read + nb_of.SW_fill.frames.read + nb_of.HW_fill.frames.read) / 
                ((float)(t2-t1)/CLOCKS_PER_SEC));
        if (T2 != T1) {
          printf( " = %.2f Mbit/sec (real time)\n", 
                  4080.0 * 8.0 / 1000000.0 *
                  (nb_of.data.frames.read + nb_of.SW_fill.frames.read + nb_of.HW_fill.frames.read) / 
                  (float)(T2-T1));
        }
        else {
          printf( "\n");
        }
      }
    }
  }
  sleep( 10);
  if (TRACE_LEVEL > 0) printf( "%s *** Restarted after sleep...\n", mytime());
  goto LOOP;
}

//~============================================================================
