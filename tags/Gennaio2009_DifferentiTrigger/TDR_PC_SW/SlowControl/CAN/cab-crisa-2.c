// file cab-crisa-2.c
//
//  Read *.TS file and execute the CAB command sequence
//  (note: *.TS format is used by CRISA for CAB testing)
//
//  A.Lebedev Oct - 2008...
//

#include "cablib.h"

int JMDC_ID = 0x03;                                // global variable
int USCM_ID = 0x18;                                // global variable
int P = 0;                                         // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void get_CAB_command(FILE *file, int *siz, int8 *cmd) {

  bool debug = FALSE;
  int n = 0;

  while (1) {
    char input[160], b[160], token[160];
    char *c, *p;
    int k;
    int8 d;
if (debug) printf("next\n");
    if (!fgets(input, sizeof(input), file)) break;
    if ((p = index(input, '\n'))) *p = '\0';
    if ((p = index(input, '\r'))) *p = '\0';
if (debug) printf("last symbol is %02hhX ", input[strlen(input)-1]);
    c = b;
    strcpy(c, input);
    for (; n<254;) {
      k = sscanf(c, "%s", token);
if (debug) printf("k=%d             token=%s\n", k, token);
      if (k != 1) break;
      k = sscanf(token, "%02hhX", &d);
      if (k != 1) break;
      cmd[n++] = d;
      c = strstr(c, token) + strlen(token);
if (debug) printf("k=%d n=%d d=%02hhX c=%08X\n", k, n, d, (int32)c);
    }
  }
  *siz = n;
}

//~----------------------------------------------------------------------------

void send_command_to_CAB(char *directory, char *input, int16 *err) {

  bool debug = FALSE;
  int i, n;
  static bool first = TRUE;
  static char *token[4];
  FILE *file = NULL;
  DIR *dir;
  struct dirent *d;
  char path[160];
  int8 cmd[256], siz, rep[256];

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  get_tokens(input, ',', DIM(token), &token[0], &n);

  if (n < 3) {
    printf("*** Error: Format error\n");
    *err = 0x1020;
    return;
  }
  
  sprintf(path, "%s/cmd_%s", directory, &token[1][1]);
if (debug) printf("path = %s\n", path);
  dir = opendir(path);
  while ((d = readdir(dir))) {
    if (!strcasecmp(token[2], d->d_name)) {
if (debug) printf("  %s\n", token[2]);
if (debug) printf("  %s\n", d->d_name);
      sprintf(path, "%s/%s", path, d->d_name);
if (debug) printf("path = %s\n\n", path);
      file = fopen(path, "r");
      break;
    }
  }

  if (!file) {
    printf("*** Error: File %s/%s is absent\n", path, token[2]);
    *err = 0x1000;
    return;
  }

  get_CAB_command(file, &n, cmd);

  if (n) {
    printf("Command: ");
    for (i=0; i<n; i++) printf("%02hhX ", cmd[i]);
    printf("\n");
  }
  else {
    printf("*** Error: Wrong file %s contents\n", path);
    *err = 0x1010;
    return;
  }

  exec_CAB_command(cmd, &siz, rep, err);

  printf("err = %04X", *err);
  if (*err) {
    if (*err > 0x2000) printf(" = %s", cab_ack_message(*err-0x2000));
    printf("\n");
  }
  else {
    printf("\n");
    printf("Response: ");
    for (i=0; i<siz; i++) printf("%02hhX ", rep[i]);
    printf("\n");
  }

  if (file) fclose(file);
}

//~----------------------------------------------------------------------------

void select_CAB_CPU(char *input, int16 *err) {

  int n;
  static bool first = TRUE;
  static char *token[2];

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  get_tokens(input, ',', DIM(token), &token[0], &n);

  if (n == 2) {
    *err = 0x0000;
    if      (strcasestr(token[1], "CPU_N")) USCM_ID = 0x18;
    else if (strcasestr(token[1], "CPU_R")) USCM_ID = 0x19;
    else                                    *err = 0x1000;
    return;
  }

  *err = 0x1100;
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int k;
  FILE *file;
  int i, n;
  char input[160], *token[3];
  int16 err;
  char *directory = {"/home/ams/AAL/SlowControl/CAN/crisa-library/incmd"};
    
//~--

  setbuf(stdout, NULL);

  printf("Usage: %s <TS-file-name> [<USCM ID>|0 [<JMDC ID> [<P>]]]\n",
          argv[0]);

  if (argc < 2) PANIC("Too few arguments\n");
  
  file = fopen(argv[1], "r");
  if (!file) {
    printf("*** Error: File %s is absent\n", argv[1]);
    exit(1);
  }
  
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

  for (i=0; i<3; i++) token[i] = malloc(160);

  while (fgets(input, sizeof(input), file)) {
    get_tokens(input, ':', DIM(token), &token[0], &n);
    printf("%s\n", input);
//printf("input  = %s\n", input);
//printf("tokens = %s*%s*%s\n", token[0], token[1], token[2]);
    if (n == 3) {
      if (strcasestr(token[1], "SEND COMMAND")) {
        if (USCM_ID) send_command_to_CAB(directory, token[2], &err);
      }
      if (strcasestr(token[1], "SELECT CAB CPU")) {
        if (USCM_ID) select_CAB_CPU(token[2], &err);
      }
    }
  }

  return 0;
}

//~----------------------------------------------------------------------------
