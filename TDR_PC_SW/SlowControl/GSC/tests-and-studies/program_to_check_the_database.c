// 1.c
// program_to_check_the_database

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define integer16 unsigned short

//-- this was for the STS-91 flight data
//char *block_directory = "/dats1/SRDL/DATABASE/";

//-- this is for the GSI testbeam data
char *block_directory = "/dat0/SlowDir/GSI/dats/SRDL/DATABASE/";

//-------------------------------------------------------------------

void main( int argc, char *argv[]) {
  integer16 AMS_block_ID = 0x0000, block_type;
  FILE *f;
  char file_name[8192][81];
  struct stat st;
  int i, n;
  char *command, *object, *dataset;
  integer16 l, b[20000], l_old;
  
  setbuf( stdout, NULL);	// set "no buffering" for stdout stream
  
  for (i=0; i<8192; i++) file_name[i][0] = '\0';

  AMS_block_ID = 0x0;
  while (1) {
    block_type = (AMS_block_ID >> 13) & 0x07;
    if (block_type == 0x1 || block_type == 0x3 || block_type == 0x4) {
      if (decode_AMS_block_ID( AMS_block_ID, &command, &object, &dataset)) {
        i = AMS_block_ID & 0x1FFF;
        sprintf( file_name[i], "%s%s.%s", block_directory, object, dataset);
      }
    }
    AMS_block_ID++;
    if (AMS_block_ID == 0) break;
  }
  
  for (i=0; i<8192; i++) {
    int first = 1;
    if (file_name[i][0] == '\0') continue;
    stat( file_name[i], &st);
    if (st.st_size == 0) continue;
    printf( "File '%s' %d bytes ... ", file_name[i], st.st_size);
    if ((f = fopen( file_name[i], "rb")) == NULL) {
      printf( "failed to open!\n");
      continue;
    }
    n = 0;  
    while (1) {
      if (fread( &l, sizeof( integer16), 1, f) == NULL) {
        printf( "EOF after %d blocks.\n", n);
        break;
      }
      if (n == 0) l_old =l;
      if (fread( b, sizeof( integer16), l, f) == NULL) {
        printf( "Format error in block %d\n", n);
        break;
      }
      if (i != (0x9003 & 0x1FFF) &&
          i != (0x7000 & 0x1FFF)) {
        if (l != l_old) {
          if (first) printf( "\n");
          first = 0;
          printf( "   *** Block %d l=%d/%d ", n, l, l_old);
          if (decode_AMS_block_ID( b[0], &command, &object, &dataset)) {
            printf( "ID=%4.4X (%s %s %s) ", b[0], command, object, dataset); 
          }
          else {
            printf( "ID=%4.4X (failure) ", b[0]);
          }
          printf( "%4.4X %4.4X %4.4X %4.4X %4.4X %4.4X %4.4X \n",
                   b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
        }
      }
      if (i != (b[0] & 0x1FFF)) {
        if (first) printf( "\n");
        first = 0;
        printf( "   ****** Block %d l=%d ", n, l);
        if (decode_AMS_block_ID( b[0], &command, &object, &dataset)) {
          printf( "ID=%4.4X (%s %s %s) ", b[0], command, object, dataset); 
        }
        else {
          printf( "ID=%4.4X (failure) ", b[0]);
        }
        printf( "%4.4X %4.4X %4.4X %4.4X %4.4X %4.4X %4.4X \n",
                 b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
      }
      n++;
    }
    fclose( f);
  }
}

//-------------------------------------------------------------------
