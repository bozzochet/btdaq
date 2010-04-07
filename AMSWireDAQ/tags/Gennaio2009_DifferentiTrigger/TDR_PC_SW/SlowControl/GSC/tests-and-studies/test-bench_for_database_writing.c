// 1.c
// test-bench for database writing

#include <stdio.h>
#include <stdlib.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define integer16 unsigned short
char *block_directory = "/dats1/SRDL/DATABASE/";

//-------------------------------------------------------------------

void main( int argc, char *argv[]) {
  integer16 AMS_block_ID = 0x0000, block_type;
  FILE *file[8192];
  int index;
  char path[81];
  int n = 0;
  char *command_name, *subsystem_name, *object_name, *dataset_name;
  char line[256];
  setbuf( stdout, NULL);	// set "no buffering" for stdout stream
  for (index=0; index<8192; index++) file[index] = NULL;
  while (1) {
//    printf( "enter block ID <- ");
//    gets( line);
//    sscanf( line, " %x ", &AMS_block_ID);
    block_type = (AMS_block_ID >> 13) & 0x07;
    if (block_type == 0x1 || block_type == 0x3 || block_type == 0x4) {
      if (decode_AMS_block_ID( 
          AMS_block_ID, &command_name, &object_name, &dataset_name)) {
        n++;
        printf( "%4d %4.4X %s %s %s\n", 
                n, AMS_block_ID, command_name, object_name, dataset_name); 
        index = AMS_block_ID & 0x1FFF;
        sprintf( path, "%s%s.%s", block_directory, object_name, dataset_name);
        if (file[index] != NULL) fclose( file[index]);
        if ((file[index] = fopen( path, "wb")) == NULL) {
          printf( "Failed to 'fopen'!\n");
          exit( 0);
        }
      }
    }
    AMS_block_ID++;
    if (AMS_block_ID == 0) break;
  }
  n = 0;
  for (index=0; index<8192; index++) {
    if (file[index] != NULL) n++;
  }
  printf( "n = %d\n", n);
}

//-------------------------------------------------------------------
