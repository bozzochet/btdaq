// file crisalib.c
//
// A.Lebedev - Dec 2008...
//

#include "crisalib.h"

//~============================================================================

void get_CAB_command(FILE *file, int *siz, int8 *cmd) {

  bool debug = FALSE;
  int n = 0;

  while (1) {
    char input[160], b[160], token[160];
    char *c, *p;
    int k;
    int16 d;
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
      k = sscanf(token, "%04hX", &d);
      if (k != 1) break;
if (debug) printf("d=%04X\n", d);
      if      (strlen(token) == 2) {
        cmd[n++] = d;
      }
      else if (strlen(token) == 4) {
        cmd[n++] = d>>8;
        cmd[n++] = d>>0;
      }
      else {
if (debug) printf("strlen(token) =%d\n", strlen(token));
      }
      c = strstr(c, token) + strlen(token);
if (debug) printf("k=%d n=%d d=%04X c=%08X\n", k, n, d, (int32)c);
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

  bool debug = FALSE;
  int n;
  static bool first = TRUE;
  static char *token[2];

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  if (debug) {
    static struct timeval now, then;
    gettimeofday(&now, NULL);
    printf("dt = %f\n", delta_t(&now, &then));
    memcpy(&then, &now, sizeof(now));
  }

  *err = 0x0000;

  get_tokens(input, ',', DIM(token), &token[0], &n);

  if (n == 2) {
    if      (strcasestr(token[1], "CPU_N")) USCM_ID = NODE_ADR = 0x18;
    else if (strcasestr(token[1], "CPU_R")) USCM_ID = NODE_ADR = 0x19;
    else                                    *err = 0x1000;
    return;
  }

  *err = 0x1100;
}

//~----------------------------------------------------------------------------

void select_REQUEST_BUS(char *input, int16 *err) {

  int n;
  static bool first = TRUE;
  static char *token[2];

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  *err = 0x0000;

  get_tokens(input, ',', DIM(token), &token[0], &n);

  if (n == 2) {
    if      (strcasestr(token[1], "BUS_N")) BUS_FOR_REQUEST = 0;
    else if (strcasestr(token[1], "BUS_R")) BUS_FOR_REQUEST = 1;
    else                                    *err = 0x1000;
    return;
  }

  *err = 0x1100;
}

//~----------------------------------------------------------------------------

void select_REPLY_BUS(char *input, int16 *err) {

  int n;
  static bool first = TRUE;
  static char *token[2];

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  *err = 0x0000;

  get_tokens(input, ',', DIM(token), &token[0], &n);

  if (n == 2) {
    if      (strcasestr(token[1], "BUS_N")) BUS_FOR_REPLY = 0;
    else if (strcasestr(token[1], "BUS_R")) BUS_FOR_REPLY = 1;
    else                                    *err = 0x1000;
    return;
  }

  *err = 0x1100;
}

//~----------------------------------------------------------------------------

int wait_n_seconds(char *input, int16 *err) {

  int n;
  static bool first = TRUE;
  static char *token[3];

  if (first) {
    int i;
    for (i=0; i<DIM(token); i++) token[i] = malloc(160);
    first = FALSE;
  }

  *err = 0x0000;

  get_tokens(input, ',', DIM(token), &token[0], &n);

  if (n == 3) {
    int t;
    if (sscanf(token[1], "%d", &t) == 1) {
      if (strcasestr(token[2], "mSEC")) {
        delay((float)t/1000.0);
        return 0;
      }
      if (strcasestr(token[2],  "SEC")) {
        return t;
      }
      *err = 0x1000;
      return 0;
    }
  }
  *err = 0x1100;
  return 0;
}

//~============================================================================
