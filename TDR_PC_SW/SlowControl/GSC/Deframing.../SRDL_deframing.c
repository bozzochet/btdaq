// file SRDL_deframing.c
//
// preproduction version of deframing program for SRDL link
// reads Timo's frame files and deframes them, writing blocks into block files
//
// this is supposed my main GSE program for SRDL deframing

#include "SRDL_frames.h"

char *frame_directory = "/stage/srdl/HK/";

//char event_directory[81] = "/usr0/users/lebedev/AMS/Deframing.../SRDL/EVENTS/";
//char HKdat_directory[81] = "/usr0/users/lebedev/AMS/Deframing.../SRDL/HKDATA/";
//char reply_directory[81] = "/usr0/users/lebedev/AMS/Deframing.../SRDL/REPLIES/";
//char status_directory[81] = "/usr0/users/lebedev/AMS/Deframing.../SRDL/STATI/";

char event_directory[81] = "/dats1/SRDL/EVENTS/";
char HKdat_directory[81] = "/dats1/SRDL/HKDATA/";
char reply_directory[81] = "/dats1/SRDL/REPLIES/";
char status_directory[81] = "/stage/STATUS/";

char *log_pipe_file_name = "/tmp/SRDL_deframing_log.pipe";
char *log_file_name      = "/stage/logs/SRDL_deframing_log.file";

//------------------------------------------------------------------------------

void main( int argc, char *argv[]) {

  long last_event_file, last_HKdat_file, last_reply_file;
  clock_t t1, t2;
  time_t T1, T2;
  int counter = 0;
  int i;
  
  setbuf( stdout, NULL);	// set "no buffering" for stdout

  TRACE_LEVEL = 0;
  if (argc > 2) TRACE_LEVEL = atoi( argv[2]);
  if (TRACE_LEVEL > 9) TRACE_LEVEL = 9;

  printf("\n\t**************************************************************\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                 A.Lebedev  01-May-98 16:58                 *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*       (S)LOW RATE DATA LINK = SRDL DEFRAMING PROGRAM       *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                       version 0.94                         *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t**************************************************************\n\n");

#ifndef do_not_use_pipe
//printf( "\n\n\t\t see my output in '%s'\n", log_pipe_file_name);
//redirect_stdout_to_pipe( log_pipe_file_name);
  printf( "\n\n\t\t see my output in '%s'\n", log_file_name);
  redirect_stdout_to_file( log_file_name);
#endif
  
  if (TRACE_LEVEL > 1) printf( "Second parameter set TRACE_LEVEL = %d.", TRACE_LEVEL);

  initialize_everything();

  frame_file_number = 0;
  last_event_file = last_file_number_in( event_directory);
  last_HKdat_file = last_file_number_in( HKdat_directory);
  last_reply_file = last_file_number_in( reply_directory);
  if (last_event_file > frame_file_number) frame_file_number = last_event_file;
  if (last_HKdat_file > frame_file_number) frame_file_number = last_HKdat_file;
  if (last_reply_file > frame_file_number) frame_file_number = last_reply_file;
  
  if (TRACE_LEVEL > 1) {
    printf( "    Want to read frame file %6.6ld\n", frame_file_number);
  }

  if (argc > 1) {
    i = atoi( argv[1]);
    if (i > 0 && i <= 999999) {
      frame_file_number = i;
      if (TRACE_LEVEL > 1) {
        printf( "First parameter forced to read frame file %6.6ld\n",
                 frame_file_number);
      }
    }
  }

  if (frame_file_number == 0) {
    frame_file_number = 1;
    do_not_write_AMS_block_file_PLS = FALSE;
  }
  else {
    do_not_write_AMS_block_file_PLS = TRUE;
  }
  
  printf("\nFirst frame file to read   = %s%6.6ld\n", frame_directory, frame_file_number);
  printf(  "First event file to write  = %s%6.6ld\n", event_directory, frame_file_number+1);
  printf(  "First HKdata file to write = %s%6.6ld\n", HKdat_directory, frame_file_number+1);
  printf(  "First reply file to write  = %s%6.6ld\n", reply_directory, frame_file_number+1);
  printf(  "\n");

LOOP:
  t1 = clock();
  T1 = time( NULL);
  for ( ;
       frame_file_number <= last_file_number_in(frame_directory);
       frame_file_number++) {
    if (open_SRDL_frame_file() == OK) {
      bzero( &nb_of, sizeof( nb_of));
      open_AMS_block_tempfiles();
      while (read_SRDL_frame_file() == OK) {
        if (decode_SRDL_frame() == OK) {
          deframe_SRDL_frame();
        }
      }
      rename_AMS_block_tempfiles();
      close_SRDL_frame_file();
      print_SRDL_deframing_log();
    }
    do_not_write_AMS_block_file_PLS = FALSE;
    counter += nb_of.SRDL.frames.read;
  }

  if (counter > 6060) {
    t2 = clock();
    T2 = time( NULL);
    if (TRACE_LEVEL > 0) {
      if (t2 != t1) {
        printf( "Deframing rate = %.2f Mbit/sec (CPU time)", 
        (144.0*8.0*counter/1000000.0) / ((float)(t2-t1)/CLOCKS_PER_SEC));
      }
      if (T2 != T1) {
        printf( " = %.2f Mbit/sec (real time)\n", 
        (144.0*8.0*counter/1000000.0) / ((float)(T2-T1)));
      }
      else {
        printf( "\n");
      }
    }
    counter = 0;
  }
  sleep( 10);
  if (TRACE_LEVEL > 0) printf( "%s *** Restarted after sleep...\n", mytime());
  goto LOOP;
}

