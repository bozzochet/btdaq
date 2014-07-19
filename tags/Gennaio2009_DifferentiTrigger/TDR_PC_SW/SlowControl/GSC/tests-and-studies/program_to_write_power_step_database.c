// 1.c
// program_to_write_power_step_database
//
// - reads Timo's frame files
// - extracts OIU time information (see program text)
// - extracts MCC information on AMS power steps (uses discreets)
// - writes this somewhere
//

#include "SRDL_frames.h"

#define do_not_use_logfile 1

char *frame_directory = "/stage/srdl/HK/";
char *block_directory = "/dats1/SRDL/DATABASE/";
//char *log_file_name   = "/stage/logs/power_step_database_writing_log.file";
char *log_file_name   = "power_step_database_writing_log.file";

//
//----- PDI/HK/SRDL frame data structure description ---------
//     (it is "compatible" with HRDL frame description)
//    (it is so funny to be similar to that of AMS block)
//

#define PDI_FRAME_LENGTH_IN_BYTES 160

#define SRDL_FRAME_LENGTH_IN_BYTES 124
#define SRDL_FRAME_BODY_LENGTH 60

union {
  unsigned char bytes[PDI_FRAME_LENGTH_IN_BYTES];
  integer16     words[PDI_FRAME_LENGTH_IN_BYTES/2];
  integer32     wordl[PDI_FRAME_LENGTH_IN_BYTES/4];
} PDI_frame;

#define         PDI_sync_pattern          PDI_frame.wordl[0]
integer16      *OIU_health_and_status =  &PDI_frame.words[2];
integer16      *AMS_health_and_status =  &PDI_frame.words[6];
  unsigned char AMS_health_discreets;
  unsigned char AMC_GPC_command_counter;
  unsigned char AMS_command_counter;
#define         SRDL_frame_header         PDI_frame.words[8]
  unsigned char SRDL_frame_ID;                         
  unsigned char SRDL_frame_first_block_link;
integer16      *SRDL_frame_body =        &PDI_frame.words[9];
#define         SRDL_frame_CRC            PDI_frame.words[69]
integer16      *OIU_transaction_status = &PDI_frame.words[70]; 
integer16      *OIU_fill_words =         &PDI_frame.words[72]; 

//--------------------------------------------------------------------

void initialize( void) {
  static done = FALSE;

  printf("\n\t************************* A.Lebedev ******\n");  
  printf(  "\t*                                        *\n");  
  printf(  "\t*   POWER STEP DATABASE WRITING PROGRAM  *\n");  
  printf(  "\t*                                        *\n");  
  printf(  "\t*             trace level %d              *\n",
  TRACE_LEVEL);  
  printf(  "\t*                                        *\n");  
  printf(  "\t******************************************\n\n");
  
  if (done) return;
  
  done = TRUE;

  if (sizeof( integer16) != 2) {
    printf( "The size of 'integer16' is wrong! Good bye!\n");
    exit( 0);
  }
  if (sizeof( integer32) != 4) {
    printf( "The size of 'integer32' is wrong! Good bye!\n");
    exit( 0);
  }
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

int open_SRDL_frame_file( void) {

  extern char *frame_directory;
  
  sprintf( frame_path, "%s%6.6u", frame_directory, frame_file_number);
  
  if ((frames = fopen( frame_path, "rb")) == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "fopen(%s,\"rb\") : %s\n", frame_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open      %s\n", frame_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int read_SRDL_frame_file( void) {
  int ltot;
  integer16 *p;

  errno = 0;

// the number "160" is a magic number introduced by Timo & Timo

  ltot = fread( PDI_frame.bytes, sizeof( unsigned char), 160, frames);
  
  if (ltot == 160) {
    for (p=PDI_frame.words; 
         p<PDI_frame.words+PDI_FRAME_LENGTH_IN_BYTES/2;
         p++) {
      (*p) = ((*p) >> 8) | ((*p) << 8);
    }
    nb_of.SRDL.frames.read++;
    return OK;
  }
  else {
    if (ltot > 0) {
      if (TRACE_LEVEL > 0) {
        printf( "file %s: last frame has only %d bytes.\n", 
                 frame_path, ltot);
      }
    }
    if (errno) {
      printf( "fread(%s) : %s\n", frame_path, strerror( errno));
      printf( "Execution terminated.\n");
      exit( 0);
    }
    return FAIL;
  }
}

//--------------------------------------------------------------------

/****************************************************************************/
/*                                                                          */
/*  cyclic redundancy check source was "borrowed" from VK (and "reworked")  */
/*                                                                          */
/****************************************************************************/

int decode_SRDL_frame( void) {

  integer16 *p, d, i, FCS;
  integer16 g = 0x1021;

  PDI_sync_pattern = (PDI_sync_pattern >> 16) | (PDI_sync_pattern << 16);

  if ((PDI_sync_pattern & 0xFFFFFF00) != 0xC95A6300) {
    if (TRACE_LEVEL > 0) {
      printf( "%s #%d: SYNC pattern error: PDI_sync_pattern=%8.8X (C95A63xx) \n",
              frame_path, 
              nb_of.SRDL.frames.read,
              PDI_sync_pattern);
    }
    nb_of.SRDL.frames.with_SYNC_errors++;
    frame_sync = FALSE;
    return FAIL; 
  }

  if (((OIU_transaction_status[0] & 0x8010) != 0x8010) ||
      ((OIU_transaction_status[1] & 0x8010) != 0x8010)) {
    if (TRACE_LEVEL > 0) {
      printf( "%s #%d: 1553 error: block status words = %4.4X %4.4X\n",
              frame_path,
              nb_of.SRDL.frames.read,
              OIU_transaction_status[0], OIU_transaction_status[1]);
    }
    nb_of.SRDL.frames.with_1553_errors++;
    frame_sync = FALSE;
    return FAIL; 
  }

  FCS = 0xFFFF;
  for (p=&SRDL_frame_header; p<&SRDL_frame_CRC; p++) {
    d = *p;
    for (i=0; i<16; i++) {
      if ((FCS ^ d) & 0x8000)
        FCS = (FCS << 1) ^ g;
      else
        FCS = (FCS << 1);
      d <<= 1;
    }
  }

  if (FCS != SRDL_frame_CRC) {
    if (TRACE_LEVEL > 0) {
      printf( "%s #%d: CRC16 error: SRDL_frame_CRC=%8.8X FCS=%8.8X\n",
              frame_path,
              nb_of.SRDL.frames.read,
              SRDL_frame_CRC, FCS);
    }
    nb_of.SRDL.frames.with_CRC_errors++;
    frame_sync = FALSE;
    return FAIL;
  }
  return OK;

}

//--------------------------------------------------------------------

int close_SRDL_frame_file( void) {

  if (fclose( frames)) {
    if (TRACE_LEVEL > 0) {
      printf( "fclose(%s) : %s\n", frame_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", frame_path);
    return OK;
  }
}

//====================================================================

time_t converted_OIU_time( void) {
  struct tm OIU_tm;
  time_t now, OIU_t;
  memcpy( (char *)&OIU_tm, (char *)localtime( &now), sizeof( OIU_tm));
  OIU_tm.tm_year = ((OIU_health_and_status[0] >> 12) & 0x000F) * 10
                 + ((OIU_health_and_status[0] >>  8) & 0x000F);
  OIU_tm.tm_mon  = ((OIU_health_and_status[0] >>  4) & 0x000F) * 10
                 + ((OIU_health_and_status[0]      ) & 0x000F)
                 - 1;
  OIU_tm.tm_mday = ((OIU_health_and_status[1] >> 12) & 0x000F) * 10
                 + ((OIU_health_and_status[1] >>  8) & 0x000F);
  OIU_tm.tm_hour = ((OIU_health_and_status[1] >>  4) & 0x000F) * 10
                 + ((OIU_health_and_status[1]      ) & 0x000F);
  OIU_tm.tm_min  = ((OIU_health_and_status[2] >> 12) & 0x000F) * 10
                 + ((OIU_health_and_status[2] >>  8) & 0x000F);
  OIU_tm.tm_sec  = ((OIU_health_and_status[2] >>  4) & 0x000F) * 10
                 + ((OIU_health_and_status[2]      ) & 0x000F);
//printf( "%2.2d %2.2d %2.2d %2.2d %2.2d %2.2d ", 
//        OIU_tm.tm_year, OIU_tm.tm_mon, OIU_tm.tm_mday,
//        OIU_tm.tm_hour, OIU_tm.tm_min, OIU_tm.tm_sec);
  OIU_t = mktime( &OIU_tm);
  OIU_t += mktime( localtime( &now)) - mktime( gmtime( &now));
  return OIU_t;
}

//--------------------------------------------------------------------

void do_the_job( void) {
  int DEBUG = FALSE;
  int    minor_frame_counter;
  char   ascii_time[26];
  time_t OIU_time;
  int    AMS_power_step;
  static int    FIRST = TRUE;
  static time_t old_OIU_time;
  static int    old_AMS_power_step;
  
  minor_frame_counter = PDI_sync_pattern % 10;
  if (minor_frame_counter == 2) {
    OIU_time = converted_OIU_time();
    AMS_power_step = (AMS_health_and_status[0] >> 9) & 0x03;
    if (FIRST) {
      FIRST = FALSE;
      strcpy( ascii_time, (char *)(ctime(&OIU_time)+4));
      ascii_time[15] = '\0';
      printf( "%8.8X * %s * %d\n", OIU_time, ascii_time, AMS_power_step);
    }
    else {
      if (AMS_power_step != old_AMS_power_step) {
        strcpy( ascii_time, (char *)(ctime(&old_OIU_time)+4));
        ascii_time[15] = '\0';
        printf( "%8.8X * %s * %d\n", old_OIU_time, ascii_time, old_AMS_power_step);
        strcpy( ascii_time, (char *)(ctime(&OIU_time)+4));
        ascii_time[15] = '\0';
        printf( "%8.8X * %s * %d\n", OIU_time, ascii_time, AMS_power_step);
      }
    }
    old_AMS_power_step = AMS_power_step;
    old_OIU_time = OIU_time;
    if (DEBUG) {
      int i;
      static cntr = 0;
      for (i=0; i<3; i++) printf( "%4.4X ", OIU_health_and_status[i]);
      printf( "%8.8X ", OIU_time);
      SRDL_frame_first_block_link = SRDL_frame_header & 0x00FF;
      printf( "%2.2X ", SRDL_frame_first_block_link);
      if (!SRDL_frame_first_block_link) {
        integer32 block_time;
        block_time = (SRDL_frame_body[3] << 16) | (SRDL_frame_body[4] & 0xFFFF);
        for (i=0; i<5; i++) printf( "%4.4X ", SRDL_frame_body[i]);
        printf( "%8.8X ", block_time);
      }
      printf( "P=%d ", AMS_power_step);
      printf( "\n");
      cntr++;
      if (cntr > 10) exit( 0);
    }
  }
}

//===================================================================

void print_database_writing_log( void) {

      printf( "%s * %s : RD=%d SYNCe=%d 1553e=%d CRCe=%d \n",
 
          mytime(),
          frame_path,
          
          nb_of.SRDL.frames.read, 
          nb_of.SRDL.frames.with_SYNC_errors, 
          nb_of.SRDL.frames.with_1553_errors, 
          nb_of.SRDL.frames.with_CRC_errors);
}

//--------------------------------------------------------------------

void main( int argc, char *argv[]) {

  integer32 first_frame_file_number, last_frame_file_number;
  integer32 i;

  setbuf( stdout, NULL);	// set "no buffering" for stdout
  TRACE_LEVEL = 0;
  if (argc > 3) TRACE_LEVEL = atoi( argv[3]);
  if (TRACE_LEVEL > 9) TRACE_LEVEL = 9;
  initialize();
  
#ifndef do_not_use_logfile
  printf( "\n\t see my output in '%s'\n", log_file_name);
  redirect_stdout_to_file( log_file_name);
  initialize();
#endif
    
  first_frame_file_number = 0;
  if (argc > 1) {
    i = atoi( argv[1]);
    if (i > 0 && i <= 999999) first_frame_file_number = i;
  }
  if (!first_frame_file_number) first_frame_file_number = 32218;
  
  last_frame_file_number = 0;
  if (argc > 2) {
    i = atoi( argv[2]);
    if (i > 0 && i <= 999999) last_frame_file_number = i;
  }
  if (!last_frame_file_number) last_frame_file_number = 45282;
  
  printf( "Will read frame files %6.6u..%6.6u\n",
           first_frame_file_number, last_frame_file_number);

  for (frame_file_number  = first_frame_file_number;
       frame_file_number <= last_frame_file_number;
       frame_file_number++) {
    if (open_SRDL_frame_file() == OK) {
      bzero( &nb_of, sizeof( nb_of));
      while (read_SRDL_frame_file() == OK) {
        if (decode_SRDL_frame() == OK) {
          do_the_job();
        }
      }
      close_SRDL_frame_file();
//    print_database_writing_log();
    }
  }
}

//------------------------------------------------------------------------------

