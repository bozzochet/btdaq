// n.c
// test-bench for named pipes

#include "HRDL_frames.h"

char *pipe_file_name = "pipe.pipe";

//-------------------------------------------------------------------

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
//-------------------------------------------------------------------
void main( int argc, char *argv[]) {

  int i=0, k;
  
  redirect_stdout_to_pipe( pipe_file_name);

  while (1) {
    printf( "line %d\n", i);
    fprintf( stderr, "line %d\n", i);
    i++;
  }
}

