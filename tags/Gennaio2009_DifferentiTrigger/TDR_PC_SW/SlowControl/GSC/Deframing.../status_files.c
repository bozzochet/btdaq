// status_files.c
// appendix to SRDL deframing for AMS status file writing

#include <stdio.h>
#include <string.h>
#include "AMS_commanding_language.h"

#include "SRDL_frames.h"

#define NB_OF_SYNONIMS MAX_NB_OF_SYNONIMS

char *Object_name[128][64], *Dataset_name[128][64];

//---------------------------------------------------------------------------
int strequ( char *a, char *b) {
  if (strlen(a)==0 && strlen(b)==0 || strcmp(a,b)==0) return 1; else return 0;
}
//---------------------------------------------------------------------------
int search( TABLE *table, char *name, int command_mode, char *parent) {
  int i, k;
  if (name[0] == '\r') {
    table[0].q = -1;		// needed for nested usage
    return -1;
  }
  for (i=0; i<10000; i++) {	// how to use smthng better than 10000?
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
int create_block_ID_decoding_table( void) {
  int DEBUG = 0;
  int i, j, k, mask = GET+EXE+HKP;

  int   command_index, subsystem_index, object_index, dataset_index;
  char *command_name, *subsystem_name, *object_name, *dataset_name;

  int block_type, node_type, node_number, data_type;
  int node_ID;
  
  for (i=0; i<128; i++) {
    for (j=0; j<64; j++) {
      Object_name[i][j] = NULL;
      Dataset_name[i][j] = NULL;
    }
  }
  search( subsystem, "\r", mask, "");
  while ((i = search( subsystem, "\n", mask, "")) != -1) {
    subsystem_index = i;
    subsystem_name  = subsystem[i].name[0];
    node_type       = subsystem[i].data;
    if (DEBUG) {
      printf("subsystem_index = %d, subsystem_name = %s, node_type = %X\n",
              subsystem_index, subsystem_name, node_type);
    }
    if (node_type > 0xF) return 101;
    search( object, "\r", mask, "");
    while ((j = search( object, "\n", mask, subsystem_name)) != -1) {
      object_index = j;
      object_name  = object[j].name[0];
      node_number  = object[j].data;
      node_ID = (node_type << 3) | node_number;
      if (DEBUG) {
        printf("  object_index = %d, object_name = %s, node_number = %X, node_ID = %2.2X\n",
                  object_index, object_name, node_number, node_ID);
      }
      if (node_number > 0x7) return 102;
      search( dataset, "\r", mask, "");
      while ((k = search( dataset, "\n", mask, subsystem_name)) != -1) {
        dataset_index = k;
        dataset_name  = dataset[k].name[0];
        data_type     = dataset[k].data;
        if (DEBUG) {
          printf("     dataset_index = %d, dataset_name = %s, data_type = %X\n",
                       dataset_index, dataset_name, data_type);
        }
        if (data_type > 0x3F) return 103;
        if (Object_name[node_ID][data_type] != NULL) return 104;
        Object_name[node_ID][data_type] = object_name;
        if (Dataset_name[node_ID][data_type] != NULL) return 105;
        Dataset_name[node_ID][data_type] = dataset_name;
      }
    }
  }
  return 0;
}
//---------------------------------------------------------------------------
int write_AMS_status_block_file( void) {

  extern char status_directory[];
  
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
        printf( "AMS_block_ID decoding table created!\n");
      }
    }
  }
    
  if (do_not_write_AMS_block_file_PLS) return OK;
  if (NO_TABLE) return FAIL;
  
  block_type = AMS_block_ID >> 13;
  
  if ((block_type == 6) || (AMS_block_body[0] & 0xF000)) {
    if (block_type == 6) nb_of.type_6_blocks++;
    if (AMS_block_body[0] & 0xF000) nb_of.command_failures++;
    return OK;
  }

  if (block_type != 1 && block_type != 3 && block_type != 4) return OK;

  node_ID   = (AMS_block_ID >> 6) & 0x7F;
  data_type =  AMS_block_ID       & 0x3F;

  object_name  = Object_name[node_ID][data_type];
  dataset_name = Dataset_name[node_ID][data_type];

  if (object_name == NULL || dataset_name == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "Decoding failure: block_ID = %4.4X object_name = %s dataset_name = %s\n",
               AMS_block_ID, object_name, dataset_name);
    }
    nb_of.block_ID_decoding_failures++;
    return FAIL;
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
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "rename -> %s\n", path);
  }

  nb_of.status.blocks.written++;
  return OK;

}
//---------------------------------------------------------------------------
