// this is file p_data.c *** to be used on ALPHA computers ONLY ***
//
// AMS 1553 bus monitor for STS PGSC computer (ISA card 1553 interface only)
//
// - various functions for 1553 interface (BU-65529)
//
// modified to read frames from some strange place
//

#include "pgsc.h"
#include <sys/stat.h>

long last_frame_file_number;
//char *frame_directory = "/stage/srdl/HK/";       // is AK an idiot?
//char *frame_directory = "/dat0/SlowDir/srdl/HK/";  // is AK an idiot?
char *frame_directory = "/f2dat1/SlowDir/srdl/HK/";
char *frame_path[256];

//-------------------------------------------------------------------

int read_frame_file( void) {

  static int OPEN = FALSE;
  static unsigned char Timo[160];
  static FILE *file;
  
  if (!OPEN) {
    if (frame_file_number <= last_frame_file_number) {
      sprintf( frame_path, "%s%6.6ld", frame_directory, frame_file_number);
      if ((file = fopen( frame_path, "rb")) == NULL) return FAIL;
      OPEN = TRUE;
    }
    else {
      SIMULATION = FALSE;
      return FAIL;
    }
  }

  if (fread( Timo, 1, 160, file) == 160) {
    memcpy( HK.word, &Timo[12], 128);
    memcpy( block_status, &Timo[140], 4);
    return OK;
  }
  else {
    fclose( file);
    OPEN = FALSE;
    frame_file_number++;
    return FAIL;
  }
}

//-------------------------------------------------------------------

int read_temp_frame_file( void) {
  struct stat st1, st2;
  int l;

  static int FIRST = TRUE;
  static int OPEN;
  static int FULL_FRAME_WAS_READ;
  static unsigned char Timo[160];
  static int fd1, fd2;
  static int left;
  static unsigned char *pp;
  
  if (FIRST) {
    FIRST = FALSE;
    sprintf( frame_path, "%sfsd.hk.tmp", frame_directory);
    OPEN = FALSE;
  }
  
  if (!OPEN) {
    if ((fd1 = open( frame_path, O_RDONLY)) < 0) return FAIL;
    OPEN = TRUE;
    FULL_FRAME_WAS_READ = TRUE;
  }
  
  if (FULL_FRAME_WAS_READ) {
    left = sizeof( Timo);
    pp   = Timo;
    FULL_FRAME_WAS_READ = FALSE;
  }

  while (1) {
    l = read( fd1, pp, left);
    left = left - l;
    pp   = pp + l;
    if (left == 0) break;
    if (l <= 0) {
      if ((fd2 = open( frame_path, O_RDONLY)) < 0) return FAIL;
      fstat( fd2, &st2);
      close( fd2);
      fstat( fd1, &st1);
      if (st1.st_ino != st2.st_ino) {
        close( fd1);
        OPEN = FALSE;
      }
      return FAIL;
    }
  }
  memcpy( HK.word, &Timo[12], 128);
  memcpy( block_status, &Timo[140], 4);
  FULL_FRAME_WAS_READ = TRUE;
  return OK;
}

//----------------------------------------------------------------------

long last_file_number_in( char *directory_name) {
#include <dirent.h>
#include <fnmatch.h>

  long file_number;
  long last_file_number;

  DIR  *dirp;
  struct dirent *dp;
  
  int TRACE_LEVEL = 0;
  
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

//----------------------------------------------------------------------

void initialize_1553_hardware( void) {

  I_am_DEC_ALPHA = TRUE;
  
  I_am_in_BM_mode = FALSE;
  I_am_in_BC_mode = TRUE;
  I_am_in_RT_mode = FALSE;
  
  last_frame_file_number = last_file_number_in( frame_directory);
  if (frame_file_number_to_start >  0) frame_file_number = frame_file_number_to_start;
  if (frame_file_number_to_start == 0) frame_file_number = last_frame_file_number - 1;
  if (frame_file_number_to_start <  0) frame_file_number = last_frame_file_number + frame_file_number_to_start;
  if (frame_file_number_to_start == 0) SIMULATION = FALSE;
  else                                 SIMULATION = TRUE;
}

//----------------------------------------------------------------------

int new_1553_message( void) {

  int status;
  integer16 *p;
  
  if (SIMULATION) status = read_frame_file();
  else            status = read_temp_frame_file();
  
  if (status) {
    p = block_status; 
    (*p) = ((*p) >> 8) | ((*p) << 8);
    p++;
    (*p) = ((*p) >> 8) | ((*p) << 8);
    for (p=HK.word; p<HK.word+64; p++) {
      (*p) = ((*p) >> 8) | ((*p) << 8);
    }
    return OK;
  }
  else {
    return FAIL;
  }
}

//----------------------------------------------------------------------
void process_1553_message( void) {
  new_frame_number = 1;
  new_time_broadcast = 0;
  new_PSP_frame = 0;
  new_PDI_frame = 1;
}

//----------------------------------------------------------------------

