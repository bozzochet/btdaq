// file HRDL_deframing_log_viewing.c
//*********************************************************************
//
//  reads '/tmp/HRDL_deframing_log.pipe' and prints log messages
//
//*********************************************************************

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

char *LogPipeName = "/tmp/HRDL_deframing_log.pipe";
int LogPipeFileDescriptor = -1;
#define READ_WRITE_PIPE_MODE 0666
char b[PIPE_BUF];

int nb_of_chars_read;

main() {
  setbuf( stdout, NULL);
  while (1) {
    printf( "Opening pipe '%s' for reading... ", LogPipeName);
    LogPipeFileDescriptor = open( LogPipeName, O_RDONLY);
    if (LogPipeFileDescriptor < 0) {
      printf( "\a Failed!\n");
      printf( "Creating pipe '%s'... ", LogPipeName);
      if (mknod( LogPipeName, S_IFIFO | 0666, (dev_t) NULL)) {
        printf( "\a Failed!!\n");
        sleep( 5);
        exit( 0);
      }
      else {
        printf( " Created!\n");
        printf( "Opening pipe '%s' for reading... ", LogPipeName);
        LogPipeFileDescriptor = open( LogPipeName, O_RDONLY);
        if (LogPipeFileDescriptor < 0) {
          printf( "\a Failed!!\n");
          sleep( 5);
          exit( 0);
        }
      }
    }
    else {
      printf( " Opened!!!\n");
    }
    while (1) {
      nb_of_chars_read = read( LogPipeFileDescriptor, b, (size_t) PIPE_BUF);
      if (nb_of_chars_read > 0) {
        b[nb_of_chars_read] = '\000';
        printf( b);
      }
      else {
        printf( "No anymore writers to pipe '%s'!\n", LogPipeName);
        printf( "Closing pipe '%s'... ", LogPipeName);
        close( LogPipeName);
        printf( " Closed!\n");
        break;
      }
    }
  }
}
