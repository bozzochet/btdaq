// 3.c
// clean-up AMS_commanding_language.h Tables...

#include <stdio.h>
#include <string.h>
#include "AMS_commanding_language.h"

#define NB_OF_SYNONIMS MAX_NB_OF_SYNONIMS

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

main() {
int i, j, k;

printf(" HERE IS MY LANGUAGE:\n");

printf("\n COMMANDS:\n\n");

search( command, "\r", ALL, "");
while ((i = search( command, "\n", ALL, "")) != -1) {
  printf("%s\t block_type = %X, aliases are: ",
  command[i].name[0], command[i].data);
  for (k=1; k<NB_OF_SYNONIMS; k++) {
    if (strlen(command[i].name[k]) == 0) break;
    printf("%s ", command[i].name[k]);
  }
  printf("\n");
}

printf("\n SUBSYSTEMS:\n\n");

search( subsystem, "\r", ALL, "");
while ((i = search( subsystem, "\n", ALL, "")) != -1) {
  printf("%s\t node_type = %X, aliases are: ",
  subsystem[i].name[0], subsystem[i].data);
  for (k=1; k<NB_OF_SYNONIMS; k++) {
    if (strlen(subsystem[i].name[k]) == 0) break;
    printf("%s ", subsystem[i].name[k]);
  }
  printf("\n");
}

printf("\n OBJECTS (common list, object names must not repeat):\n\n");

search( object, "\r", ALL, "");
while ((i = search( object, "\n", ALL, "")) != -1) {
  printf("%s\t node_number = %X, aliases are: ",
  object[i].name[0], object[i].data);
  for (k=1; k<NB_OF_SYNONIMS; k++) {
    if (strlen(object[i].name[k]) == 0) break;
    printf("%s ", object[i].name[k]);
  }
  printf("\n");
}

printf("\n OBJECTS (by-subsystem list):\n\n");

search( subsystem, "\r", ALL, "");
while ((i = search( subsystem, "\n", ALL, "")) != -1) {
  printf("%s\t node_type =   %X, aliases are: ",
  subsystem[i].name[0], subsystem[i].data);
  for (k=1; k<NB_OF_SYNONIMS; k++) {
    if (strlen(subsystem[i].name[k]) == 0) break;
    printf("%s ", subsystem[i].name[k]);
  }
  printf("\n");
  
  search( object, "\r", ALL, "");
  while ((j = search( object, "\n", ALL, subsystem[i].name[0])) != -1) {
    printf("  %s\t node_number = %X, aliases are: ",
    object[j].name[0], object[j].data);
    for (k=1; k<NB_OF_SYNONIMS; k++) {
      if (strlen(object[j].name[k]) == 0) break;
      printf("%s ", object[j].name[k]);
    }
    printf("\n");
  }
}

/*   this list is too big and "repetive"

printf("\n DATASETS (common list, dataset names may repeat):\n\n");

search( dataset, "\r", ALL, "");
while ((i = search( dataset, "\n", ALL, "")) != -1) {
  printf("%s\r\t\t\t data_type = %2.2X, aliases are: ",
  dataset[i].name[0], dataset[i].data);
  for (k=1; k<NB_OF_SYNONIMS; k++) {
    if (strlen(dataset[i].name[k]) == 0) break;
    printf("%s ", dataset[i].name[k]);
  }
  printf("\n");
}
*/


printf("\n DATASETS (by-subsystem list):\n\n");

search( subsystem, "\r", ALL, "");
while ((i = search( subsystem, "\n", ALL, "")) != -1) {
  printf("%s\r\t\t\t node_type =  %X, aliases are: ",
  subsystem[i].name[0], subsystem[i].data);
  for (k=1; k<NB_OF_SYNONIMS; k++) {
    if (strlen(subsystem[i].name[k]) == 0) break;
    printf("%s ", subsystem[i].name[k]);
  }
  printf("\n");
  
  search( dataset, "\r", ALL, "");
  while ((j = search( dataset, "\n", ALL, subsystem[i].name[0])) != -1) {
    printf("   %s\r\t\t\t data_type = %2.2X, aliases are: ",
    dataset[j].name[0], dataset[j].data);
    for (k=1; k<NB_OF_SYNONIMS; k++) {
      if (strlen(dataset[j].name[k]) == 0) break;
      printf("%s ", dataset[j].name[k]);
    }
    printf("\n");
  }
}

}
//---------------------------------------------------------------------------
