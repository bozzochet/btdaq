// test-2.c
// tests of deframing program for SRDL link
// reads Timo's OIU test frame files 
// and deframes them, writing blocks into event files
//
// this is supposed to be a model of my main GSE program for "deframing"

#include "SRDL_frames.h"

//------------------------------------------------------------------------------

void main( void) {

  long file;
  clock_t t1, t2;
  time_t T1, T2;
  
  initialize_everything();

  frame_file_number = 0;
  file = last_file_number_in( event_directory);
  if (file > frame_file_number) frame_file_number = file;
  file = last_file_number_in( HKdat_directory);
  if (file > frame_file_number) frame_file_number = file;
  file = last_file_number_in( reply_directory);
  if (file > frame_file_number) frame_file_number = file;

  if (frame_file_number == 0) {
    frame_file_number = 1;
    do_not_write_AMS_block_file_PLS = FALSE;
  }
  else {
    do_not_write_AMS_block_file_PLS = TRUE;
  }
  
  printf("\nStart reading SRDL frame files from %6.6u\n\n", frame_file_number);

  t1 = clock();
  T1 = time( NULL);

  if (do_not_write_AMS_block_file_PLS) printf("Skipping started...\n");

  while (frame_file_number <= last_file_number_in( frame_directory)) {
    if (open_frame_file( frame_file_number) == OK) {
      if (!do_not_write_AMS_block_file_PLS) {
        nb_of_blocks_written = 0;
        open_AMS_event_block_tempfile();
        open_AMS_HKdat_block_tempfile();
        open_AMS_reply_block_tempfile();
      }
      while (read_SRDL_frame_file() == OK) {
        if (decode_SRDL_frame() == OK) {
          deframe_SRDL_frame();
        }
        else {
          printf(" SRDL frame decoding failed...\n");
        }
      }
      printf( "%s: read:    %ld %ld %ld\n",
      frame_path, nb_of_frames_deframed, nb_of_blocks_deframed, nb_of_EOFs_deframed);
      close_frame_file();
      if (!do_not_write_AMS_block_file_PLS) {
        rename_AMS_event_block_tempfile( frame_file_number);
        rename_AMS_HKdat_block_tempfile( frame_file_number);
        rename_AMS_reply_block_tempfile( frame_file_number);
      }
    }
    else {
    }
    if (do_not_write_AMS_block_file_PLS) {
      do_not_write_AMS_block_file_PLS = FALSE;
      printf("Skipping finished.\n");
    }
    frame_file_number++;
  }

  t2 = clock();
  if (t2 != t1) printf("Deframing rate = %.2f Mbit/sec (CPU time)\n", 
  (144.0 * 8.0 * nb_of_frames_deframed / 1000000.0) / 
  ( ( float) (t2 - t1) / CLOCKS_PER_SEC));

  T2 = time( NULL);
  if (T2 != T1) printf("Deframing rate = %.2f Mbit/sec (real time)\n", 
  (144.0 * 8.0 * nb_of_frames_deframed / 1000000.0) / 
  ( ( float) (T2 - T1)));
}

