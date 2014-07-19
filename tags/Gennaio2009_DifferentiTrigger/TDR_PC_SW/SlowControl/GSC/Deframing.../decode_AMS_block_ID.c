// file decode_AMS_block_ID

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "AMS_commanding_language.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define NB_OF_SYNONIMS MAX_NB_OF_SYNONIMS

#define integer16 unsigned short
#define integer32 unsigned int
#define FALSE 0
#define TRUE 1
#define FAIL 0
#define OK 1

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
//-------------------------------------------------------------------

int decode_AMS_block_ID(
    integer16 AMS_block_ID, 
    char **command_name, char **object_name, char **dataset_name) {

  int DEBUG = 0;
  int block_type, node_type, node_number, data_type;
  int command_index, subsystem_index, object_index, dataset_index;
  int command_mode;
  char *subsystem_name;
  int it_is_HK_data_block = FALSE;
  static char *HK = { "HK"};

  block_type  = (AMS_block_ID >> 13) & 0x0007;
  if (block_type == 0x4) {
    it_is_HK_data_block = TRUE;
    block_type = 0x1;
  }
  node_type   = (AMS_block_ID >>  9) & 0x000F;
  node_number = (AMS_block_ID >>  6) & 0x0007;
  data_type   =  AMS_block_ID        & 0x003F;
  
  if (DEBUG) {
    printf( "AMS_block_ID = %4.4X\n", AMS_block_ID);
    printf( "block_type  = %1.1X\n", block_type);
    printf( "node_type   = %1.1X\n", node_type);
    printf( "node_number = %1.1X\n", node_number);
    printf( "data_type   = %2.2X\n", data_type);
  }

  search( command, "\r", ALL, "");
  while ((command_index = search( command, "\n", ALL, "")) != -1) {
    if (command[command_index].data != block_type) continue;
    command_mode  = command[command_index].command_mode;
    if (it_is_HK_data_block) {
      *command_name = HK;
    }
    else {
      *command_name = command[command_index].name[0];
    }
    if (DEBUG) {
      printf( "command_index = %d\n", command_index);
      printf( "command_mode  = %4.4X\n", command_mode);
      printf( "command_name  = %s\n", *command_name);
    }
    search( subsystem, "\r", command_mode, "");
    while ((subsystem_index = search( subsystem, "\n", command_mode, "")) != -1) {
      if (subsystem[subsystem_index].data != node_type) continue;
      subsystem_name = subsystem[subsystem_index].name[0];
      if (DEBUG) {
        printf( "  subsystem_index = %d\n", subsystem_index);
        printf( "  subsystem_name  = %s\n", subsystem_name);
      }
      search( object, "\r", command_mode, "");
      while ((object_index = search( object, "\n", command_mode, subsystem_name)) != -1) {
        if (object[object_index].data != node_number) continue;
        *object_name = object[object_index].name[0];
        if (DEBUG) {
          printf( "    object_index = %d\n", object_index);
          printf( "    object_name  = %s\n", *object_name);
        }
        search( dataset, "\r", command_mode, "");
        while ((dataset_index = search( dataset, "\n", command_mode, subsystem_name)) != -1) {
          if (dataset[dataset_index].data != data_type) continue;
          *dataset_name = dataset[dataset_index].name[0];
          if (DEBUG) {
            printf( "      dataset_index = %d\n", dataset_index);
            printf( "      dataset_name  = %s\n", *dataset_name);
          }
          return OK;
        }
      }
    }
    break;
  }
  return FAIL;
}
//-------------------------------------------------------------------
