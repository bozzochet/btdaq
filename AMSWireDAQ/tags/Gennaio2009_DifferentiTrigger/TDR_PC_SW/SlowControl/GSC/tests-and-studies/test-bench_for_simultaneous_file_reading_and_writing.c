// 1.c
// test-bench for simultaneous file reading and writing

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char *frame_directory = "/stage/srdl/HK/";
char *frame_path[256];
unsigned char buf[160];

//-------------------------------------------------------------------
int read_frame_file( void) {
  unsigned char Timo[160];
  struct stat st1, st2;
  int l;

  static int SWITCH = 0;
  static int FULL_FRAME_WAS_READ = 1;
  static int OPEN = 0;
  static int fd1, fd2;
  static int left;
  static unsigned char *pp;
  
  if (!OPEN) {
    sprintf( frame_path, "%sfsd.hk.tmp", frame_directory);
    fd1 = open( frame_path, O_RDONLY);
    OPEN = 1;
  }
  
  if (FULL_FRAME_WAS_READ) {
    left = sizeof( Timo);
    pp   = Timo;
    SWITCH = 0;
    FULL_FRAME_WAS_READ = 0;
  }

  while (1) {
    l = read( fd1, pp, left);
    left = left - l;
    pp   = pp + l;
    if (left == 0) break;
    if (l <= 0) {
      if (SWITCH) {
        SWITCH = 0;
        close( fd1);
        fd1 = open( frame_path, O_RDONLY);
        printf( "switch! \n");
      }
      else {
        fstat( fd1, &st1);
        printf( "sleep... %d  %07o  \n", st1.st_ino, st1.st_mode);
        fd2 = open( frame_path, O_RDONLY);
        fstat( fd2, &st2);
        close( fd2);
        printf( "         %d  %07o  \n", st2.st_ino, st2.st_mode);
        if (st1.st_ino != st2.st_ino) SWITCH = 1;
        return 0;
      }
    }
  }
  
  memcpy( buf, Timo, 160);
  FULL_FRAME_WAS_READ = 1;
  return 1;

}
//-------------------------------------------------------------------

void main( int argc, char *argv[]) {
int n = 0;
  while (1) {
    if (read_frame_file()) {
      printf( " n = %3d", n);
      n++;
      printf( ",    %2.2X   %2.2X   %2.2X   %2.2X\n", 
                   buf[0], buf[1], buf[2], buf[3]);
    }
    else {
      sleep( 1);
    }             
  }
}

//-------------------------------------------------------------------
