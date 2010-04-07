// 33.c
// test-bench for AMS_block_ID decoder

#include <stdio.h>
#include <string.h>
#include "/usr0/users/lebedev/AMS/Commanding../AMS_commanding_language.h"

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
  search( subsystem, "\r", ALL, "");
  while ((i = search( subsystem, "\n", ALL, "")) != -1) {
    subsystem_index = i;
    subsystem_name  = subsystem[i].name[0];
    node_type       = subsystem[i].data;
    if (DEBUG) {
      printf("subsystem_index = %d, subsystem_name = %s, node_type = %X\n",
              subsystem_index, subsystem_name, node_type);
    }
    if (node_type > 0xF) return 101;
    search( object, "\r", ALL, "");
    while ((j = search( object, "\n", ALL, subsystem_name)) != -1) {
      object_index = j;
      object_name  = object[j].name[0];
      node_number  = object[j].data;
      node_ID = (node_type << 3) | node_number;
      if (DEBUG) {
        printf("  object_index = %d, object_name = %s, node_number = %X, node_ID = %2.2X\n",
                  object_index, object_name, node_number, node_ID);
      }
      if (node_number > 0x7) return 102;
      search( dataset, "\r", ALL, "");
      while ((k = search( dataset, "\n", ALL, subsystem_name)) != -1) {
        dataset_index = k;
        dataset_name  = dataset[k].name[0];
        data_type     = dataset[k].data;
        if (DEBUG) {
          printf("     dataset_index = %d, dataset_name = %s, data_type = %X\n",
                       dataset_index, dataset_name, data_type);
        }
        if (data_type > 0x1F) fatal_error( 103);
        if (Object_name[object_index][dataset_index] != NULL) return 103;
        Object_name[object_index][dataset_index] = object_name;
        if (Dataset_name[object_index][dataset_index] != NULL) return 104;
        Dataset_name[object_index][dataset_index] = dataset_name;
      }
    }
  }
  return 0;
}
//---------------------------------------------------------------------------
main() {
  int status;
  setbuf( stdout, NULL);		// set "no buffering" for stdout stream
  printf( "Creating block_ID decoding table... ");
  status = create_block_ID_decoding_table();
  if (status) {
    printf( "Failed!\n");
  }
  else {
    printf( "Created!\n");
  }
}
/*
//---------------------------------------------------------------------------
// test-3.c
// tests of deframing program for HRDL link
// reads and checks ALL event files

#include "SRDL_frames.h"

char *host = NULL;
char *user = "hropr";
char *pass = "hi!rate?";

char *frame_directory = "/home/hropr/data/";
char *remote_frame_directory = "/home/hropr/data/";
char *local_frame_directory = "/home/hropr/data/";

char *event_directory = "/dath1/HRDL/EVENTS/";
char *HKdat_directory = "/dath1/HRDL/HKDATA/";
char *reply_directory = "/dath1/HRDL/REPLIES/";

//char *event_directory = "/stage/hrdl/EVENTS/";
//char *HKdat_directory = "/stage/hrdl/HKDATA/";
//char *reply_directory = "/stage/hrdl/REPLIES/";

//-------------------------------------------------------------------

void main( int argc, char *argv[]) {

  long nev;

  event_file_number = 9770;
  if (argc > 1) event_file_number = atoi( argv[1]);
  TRACE_LEVEL = 0;
  if (argc > 2) TRACE_LEVEL = atoi( argv[2]);
  
  initialize_everything();

  printf("\nStart event file reading...\n\n");

  for ( ; 
       event_file_number<=last_file_number_in( event_directory); 
       event_file_number++) {
    if (open_AMS_event_block_file( event_file_number) == OK) {
      while (read_AMS_event_block_file() == OK) {
        print_AMS_block();
      }
      close_AMS_event_block_file();
      printf("***********************************************************\n");
    }
  }
}
*/

