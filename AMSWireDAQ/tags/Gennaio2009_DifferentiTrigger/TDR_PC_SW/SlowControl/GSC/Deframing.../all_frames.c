// file all_frames.c
//
// common pieces of deframing programs for HRDL and SRDL links

#include "all_frames.h"


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

  integer16 i;
  integer16 block_type;

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

int write_AMS_event_block_file( void) {
  if (fwrite( AMS_block,
              sizeof( integer16), 
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
  if (fwrite( AMS_block, sizeof( integer16), AMS_block_length+1, HKdata)
      == AMS_block_length+1) {
    return OK;
  }
  else {
    return FAIL;
  }
}

//--------------------------------------------------------------------

int write_AMS_reply_block_file( void) {
  if (fwrite( AMS_block, sizeof( integer16), AMS_block_length+1, replies)
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

int rename_AMS_event_block_tempfile( long file) {

  extern char event_directory[];
  
  sprintf( event_path, "%s%6.6u", event_directory, file);

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

int rename_AMS_HKdat_block_tempfile( long file) {

  extern char HKdat_directory[];
  
  sprintf( HKdat_path, "%s%6.6u", HKdat_directory, file);

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

int rename_AMS_reply_block_tempfile( long file) {

  extern char reply_directory[];
  
  sprintf( reply_path, "%s%6.6u", reply_directory, file);

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

long last_file_number_in( char *directory_name) {

  long file_number;
  long last_file_number;

#ifdef this_is_ALPHA_computer
  DIR  *dirp;
  struct dirent *dp;
#endif
  
#ifdef this_is_MSDOS_computer
  int first, status, all_digits, i;
  struct find_t c_file;
#endif

#ifdef this_is_ALPHA_computer
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
#endif

#ifdef this_is_MSDOS_computer
#define LOCAL_TRACE FALSE
  if (TRACE_LEVEL > 1) printf("scan directory %s\n",directory_name);
  last_file_number = 0;
  first = TRUE;
  do {
    if (first) {
      sprintf( b, "%s??????.", directory_name);
      status = _dos_findfirst( b, _A_NORMAL, &c_file);
      first = FALSE;
    }
    else {
      status = _dos_findnext( &c_file);
    }
    if (status == 0) {
      if (LOCAL_TRACE) printf( "+++ %s", c_file.name);
      all_digits = TRUE;
      for (i=0; i<6; i++) all_digits = all_digits && isdigit( c_file.name[i]);
      if (all_digits) {
        file_number = atol( c_file.name);
        if (file_number > last_file_number) last_file_number = file_number;
        if (LOCAL_TRACE) printf("***************>> %u\n", file_number);
      }
      else {
        if (LOCAL_TRACE) printf( "\n");
      }
    }
  } while (status == 0);
#endif

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

  extern char *frame_directory;
  
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

  integer32 i;
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

int rename_frame_tempfile( long file) {

  extern char *frame_directory;
  
  sprintf( frame_path, "%s%6.6u", frame_directory, file);

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

int open_AMS_event_block_file( long file) {

  extern char event_directory[];
  
  sprintf( event_path, "%s%6.6d", event_directory, file);

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

int open_AMS_HKdat_block_file( long file) {

  extern char HKdat_directory[];
  
  sprintf( HKdat_path, "%s%6.6d", HKdat_directory, file);

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

int open_AMS_reply_block_file( long file) {

  extern char reply_directory[];
  
  sprintf( reply_path, "%s%6.6d", reply_directory, file);

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
  if (fread( &AMS_block_length, sizeof( integer16), 1, HKdata) != 1) {
    return FAIL;
  }
  if (fread( AMS_block_data, sizeof( integer16), AMS_block_length, HKdata) 
      != AMS_block_length) {
    return FAIL;
  }
  return OK;
}

//--------------------------------------------------------------------

int read_AMS_reply_block_file( void) {
  nb_of_blocks_read++;
  nb_of_reply_blocks_read++;
  if (fread( &AMS_block_length, sizeof( integer16), 1, replies) != 1) {
    return FAIL;
  }
  if (fread( AMS_block_data, sizeof( integer16), AMS_block_length, replies) 
      != AMS_block_length) {
    return FAIL;
  }
  return OK;
}

//--------------------------------------------------------------------

int read_AMS_event_block_file( void) {
  nb_of_blocks_read++;
  nb_of_event_blocks_read++;
  if (fread( &AMS_block[0], sizeof( integer16), 2, events) != 2) {
    return FAIL;
  }
  actual_AMS_block_length = AMS_block_length;
  if ((AMS_block_ID & 0xFFC0) == 0) {
    actual_AMS_block_length |= (AMS_block_ID & 0x003F) << 16;
  }
  if (fread( &AMS_block[2],
             sizeof( integer16), 
             actual_AMS_block_length-1, 
             events) 
      != actual_AMS_block_length-1) {
    return FAIL;
  }
  return OK;
}

//--------------------------------------------------------------------

int close_AMS_HKdat_block_file( void) {
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
  integer16 i;
  static int old_nb = 0, n = 0;
  int N=24000;

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

//--------------------------------------------------------------------

