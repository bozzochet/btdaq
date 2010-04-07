// test-33.c
// test-bench for AMS_block_ID decoder and AMS status monitor

#include <stdio.h>
#include <string.h>
#include "/usr0/users/lebedev/AMS/Commanding../AMS_commanding_language.h"

#include "SRDL_frames.h"

char *host = NULL;
char *user = NULL;
char *pass = NULL;
char *frame_directory = NULL;

char *event_directory = "/dats1/SRDL/EVENTS/";
char *HKdat_directory = "/dats1/SRDL/HKDATA/";
char *reply_directory = "/dats1/SRDL/REPLIES/";
char *status_directory = "/stage/STATUS/";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define NB_OF_SYNONIMS MAX_NB_OF_SYNONIMS

int   command_index, subsystem_index, object_index, dataset_index;
char *command_name, *subsystem_name, *object_name, *dataset_name;

char *Object_name[128][32], *Dataset_name[128][32];
char *Object_Name,          *Dataset_Name;

int block_type, node_type, node_number, data_type;
int node_ID;

//---------------------------------------------------------------------------
int strequ( char *a, char *b) {
  if (strlen(a)==0 && strlen(b)==0 || strcmp(a,b)==0) return 1; else return 0;
}
//---------------------------------------------------------------------------
int search( TABLE *table, char *name, int command_mode, char *parent) {
  int i, k;
  if (name[0] == '\r') {
    table[0].q = -1;  // needed for nested usage
    return -1;
  }
  for (i=0; i<10000; i++) {
    if (table[i].command_mode == 0) break;
    if (table[i].command_mode & command_mode) {
      if (parent[0]=='\0' || strequ(table[i].parent,parent)) {
        for (k=0; k<NB_OF_SYNONIMS; k++) {
          if (k>0 && strlen(table[i].name[k])==0) break;
          if (name[0]=='\n' && i>table[0].q) return (table[0].q=i);
          if (name[0]!='\n' && strequ(table[i].name[k],name)) return i;
        }
      }
    }
  }
  return -1;
}
//---------------------------------------------------------------------------
void fatal_error( int error_number) {
  printf( "FATAL ERROR #%d\n", error_number);
  exit( 0);
}
//---------------------------------------------------------------------------
int create_block_ID_decoding_table( void) {
  int DEBUG = 0;
  int i, j, k;
  for (i=0; i<128; i++) {
    for (j=0; j<32; j++) {
      Object_name[i][j] = NULL;
      Dataset_name[i][j] = NULL;
    }
  }
  search( subsystem, "\r", GET+EXE, "");
  while ((i = search( subsystem, "\n", GET+EXE, "")) != -1) {
    subsystem_index = i;
    subsystem_name  = subsystem[i].name[0];
    node_type       = subsystem[i].data;
    if (DEBUG) {
      printf("subsystem_index = %d, subsystem_name = %s, node_type = %X\n",
              subsystem_index, subsystem_name, node_type);
    }
    if (node_type > 0xF) return 101;
    search( object, "\r", GET+EXE, "");
    while ((j = search( object, "\n", GET+EXE, subsystem_name)) != -1) {
      object_index = j;
      object_name  = object[j].name[0];
      node_number  = object[j].data;
      node_ID = (node_type << 3) | node_number;
      if (DEBUG) {
        printf("  object_index = %d, object_name = %s, node_number = %X, node_ID = %2.2X\n",
                  object_index, object_name, node_number, node_ID);
      }
      if (node_number > 0x7) return 102;
      search( dataset, "\r", GET+EXE, "");
      while ((k = search( dataset, "\n", GET+EXE, subsystem_name)) != -1) {
        dataset_index = k;
        dataset_name  = dataset[k].name[0];
        data_type     = dataset[k].data;
        if (DEBUG) {
          printf("     dataset_index = %d, dataset_name = %s, data_type = %X\n",
                       dataset_index, dataset_name, data_type);
        }
        if (data_type > 0x1F) fatal_error( 103);
        if (Object_name[node_ID][data_type] != NULL) return 103;
        Object_name[node_ID][data_type] = object_name;
        if (Dataset_name[node_ID][data_type] != NULL) return 104;
        Dataset_name[node_ID][data_type] = dataset_name;
      }
    }
  }
  return 0;
}
//---------------------------------------------------------------------------
int write_AMS_status_block_file( void) {

  extern char *status_directory;
  
  static int FIRST = TRUE;
  static int NO_TABLE = TRUE;
  int block_type, node_ID, data_type;
  char *object_name, *dataset_name;
  char temp[256], path[256];
  FILE *file;

  if (FIRST) {
    FIRST = FALSE;
    if (create_block_ID_decoding_table()) {
      if (TRACE_LEVEL > 0) {
        printf( "Failed to create AMS_block_ID decoding table!\n");
      }
    }
    else {
      NO_TABLE = FALSE;
      if (TRACE_LEVEL > 1) {
        printf( "AMS_block_ID decoding table reated!\n");
      }
    }
  }
    
  if (do_not_write_AMS_block_file_PLS || NO_TABLE) return OK;
  
  block_type = AMS_block_ID >> 13;
  if (block_type != 1 && block_type != 3 && block_type != 4) return FAIL;
  
  node_ID   = (AMS_block_ID >> 6) & 0x7F;
  data_type =  AMS_block_ID       & 0x3F;
  object_name  = Object_name[node_ID][data_type];
  dataset_name = Dataset_name[node_ID][data_type];
  if (object_name == NULL || dataset_name == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "Decoding failure: block_ID = %4.4X object_name = %s dataset_name = %s\n",
               AMS_block_ID, object_name, dataset_name);
    return FAIL;
    }
  }

  sprintf( path, "%s%s.%s", status_directory, object_name, dataset_name);
  sprintf( temp, "%s!@#$%%^", status_directory);

  if ((file = fopen( temp, "wb")) == NULL) {
    printf( "fopen(%s,\"wb\") : %s\n", temp, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'wb' %s\n", temp);
  }
  if (fwrite( AMS_block, sizeof( integer16), AMS_block_length+1, file)
        != AMS_block_length+1) {
    printf( "status writing troubles\n");
    exit( 0);
    return FAIL;
  }

  if (fclose( file)) {
    printf( "fclose(%s) : %s\n", temp, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", temp);
  }
  if (rename( temp, path)) {
    printf( "rename(%s,%s) : %s\n", 
             temp, path, strerror( errno));
    exit( 0);
  }
  else {
    if (TRACE_LEVEL > 1) printf( "rename -> %s\n", path);
  }

  return OK;
}
//---------------------------------------------------------------------------

void main( int argc, char *argv[]) {

  long last_HKdat_file_number;
  long last_reply_file_number;
  long last_file_number;
  long file_number;
  
  setbuf( stdout, NULL);	// set "no buffering" for stdout

  file_number = 0;
  if (argc > 1) file_number = atoi( argv[1]);

  TRACE_LEVEL = 0;
  if (argc > 2) TRACE_LEVEL = atoi( argv[2]);
  if (TRACE_LEVEL > 9) TRACE_LEVEL = 9;
  
  initialize_everything();

LOOP:
  last_HKdat_file_number = last_file_number_in( HKdat_directory); 
  last_reply_file_number = last_file_number_in( reply_directory);
  if (last_HKdat_file_number > last_reply_file_number) {
    last_file_number = last_HKdat_file_number;
  }
  else {
    last_file_number = last_reply_file_number;
  }
  for ( ; file_number<=last_file_number; file_number++) {
    if (open_AMS_HKdat_block_file( file_number) == OK) {
      while (read_AMS_HKdat_block_file() == OK) {
        write_AMS_status_block_file();
      }
      close_AMS_HKdat_block_file();
    }
    if (open_AMS_reply_block_file( file_number) == OK) {
      while (read_AMS_reply_block_file() == OK) {
        write_AMS_status_block_file();
      }
      close_AMS_reply_block_file();
    }
  }
  sleep( 10);
  if (TRACE_LEVEL > 0) printf( "%s *** Restarted after sleep...\n", mytime());
  goto LOOP;
}
//---------------------------------------------------------------------------
