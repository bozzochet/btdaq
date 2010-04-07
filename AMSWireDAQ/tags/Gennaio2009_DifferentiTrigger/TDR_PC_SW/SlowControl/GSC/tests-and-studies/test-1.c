// test-1.c
// tests of deframing program for HRDL link
// generates blocks, frames them, and writes frame files
//
// this is a model program for a GSE front-end PCs
//

#include "HRDL_frames.h"

//------------------------------------------------------------------------------

void main( void) {

  long frame, file;
  
  initialize_everything();

  frame_file_number = 1 + last_file_number_in( frame_directory);
  printf("\nStart writing HRDL frame files from %6.6u\n\n", frame_file_number);

  for (file=0; file<60; file++) {
    open_frame_tempfile();
    for (frame=0; frame<100; frame++) {
      get_HRDL_frame();
      if (write_HRDL_frame_file() != OK) {
        printf("writing troubles\n");
        exit( 0);
      }
    }

    rename_frame_tempfile( frame_file_number);
    printf( "%s: written: %ld %ld %ld\n",
    frame_path, nb_of_frames_framed, nb_of_blocks_framed, nb_of_EOFs_framed);
    frame_file_number++;
  }
}

