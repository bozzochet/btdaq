// file cab-crisa.c
//
//  Read *.CMM file and execute the CAB command
//  (note: *.CMM format is used by CRISA for CAB testing)
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

void get_CAB_command(FILE *file, int *siz, int8 *cmd) {

  int n = 0;

  while (1) {
    char input[160], b[160], token[160];
    char *c;
    int k;
    int8 d;
printf("next\n");
    if (!fgets(input, sizeof(input), file)) break;
printf("last symbol is %02hhX ", input[strlen(input)-1]);
printf("feof(file) = %d\n ", feof(file));
    input[strlen(input)-(feof(file)?0:1)] = '\0';
    c = b;
    strcpy(c, input);
    for (; n<254;) {
      k = sscanf(c, "%s", token);
printf("k=%d             token=%s\n", k, token);
      if (k != 1) break;
      k = sscanf(token, "%02hhX", &d);
      if (k != 1) break;
      cmd[n++] = d;
      c = strstr(c, token) + strlen(token);
printf("k=%d n=%d d=%02hhX c=%08X\n", k, n, d, (int32)c);
    }
  }
  *siz = n;
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, k, n;
  int16 err;
  FILE *file;
  int8 cmd[256], siz, rep[256];
    
  setbuf(stdout, NULL);

  printf("Usage: %s <CMM-file-name> [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
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

  setup_command_path_special();

//~--

  get_CAB_command(file, &n, cmd);

  if (n) {
    printf("Command: ");
    for (i=0; i<n; i++) printf("%02hhX ", cmd[i]);
    printf("\n");
  }
  else {
    printf("Wrong file\n");
  }

//~--

  if (!initialize_EPP()) exit(1);
  
  exec_CAB_command(cmd, &siz, rep, &err);
/*
  printf("err = %04X\n", err);
  if (!err) {
    printf("Response: ");
    for (i=0; i<siz; i++) printf("%02hhX ", rep[i]);
    printf("\n");
  }
*/

  printf("err = %04X", err);
  if (err) {
    if (err > 0x2000) printf(" = %s", cab_ack_message(err-0x2000));
    printf("\n");
  }
  else {
    printf("\n");
    printf("Response: ");
    for (i=0; i<siz; i++) printf("%02hhX ", rep[i]);
    printf("\n");
  }

//~--

  return 0;
}

//~----------------------------------------------------------------------------
