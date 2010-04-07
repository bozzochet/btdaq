// file cab-crisa-old.c
//
//  Read file and execute the CAB command
//  Note: 
//    it is not "full" *.cmm format which is used by CRISA for CAB testing)
//
//  A.Lebedev Jul - 2008...
//

#include "cablib.h"

int JMDC_ID = 0x03;                                // global variable
int USCM_ID = 0x18;                                // global variable
int P = 0;                                         // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, k, n;
  int16 err;
  FILE *file;
  int8 cmd[256], siz, rep[256];
    
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s <file-name> [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc < 2) PANIC("Too few arguments\n");
  
  file = fopen(argv[1], "r");
  if (!file) PANIC("File is absent\n");
  
  k = 2;

  if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
  if (argc > k) JMDC_ID = atoi(argv[k++]);
  if (argc > k) P = atoi(argv[k++]);

  printf("File    = %s\n", argv[1]);
  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

//~--

  for (i=0; i<DIM(cmd); i++) if (fscanf(file, "%02hhX", &cmd[i]) != 1) break;
  n = i;
  printf("Command: ");
  for (i=0; i<n; i++) printf("%02hhX ", cmd[i]);
  printf("\n");

//~--

  if (!initialize_EPP()) exit(1);
  
  exec_CAB_command(cmd, &siz, rep, &err);
  printf("err = %04X\n", err);
  if (!err) {
    printf("Response: ");
    for (i=0; i<siz; i++) printf("%02hhX ", rep[i]);
    printf("\n");
  }

//~--

  return 0;
}

//~----------------------------------------------------------------------------
