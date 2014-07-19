// file gps-tc.c
//
// A simple example to send GPS TC
//
// Claude Zurbach & A.Lebedev - Jun 2007...
//

#include "gpsdef.h"

int JMDC_ID = 0x01;
int USCM_ID = 0x2C;
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char *filename;
FILE *file = NULL;

int port = 0;
int16 err;

//~----------------------------------------------------------------------------

void send_command_to_GPS(_TC *pc, _tc *tc) {

  if (tc->cnt) {
    printf("Send command %-15s \n", pc->nam);
    to_GPS(port, tc, &err);
    tc->cnt = 0;
  }
}

//~----------------------------------------------------------------------------


void read_command_file(char *filename) {

  int i, j;
  int n = sizeof(TC) / sizeof(TC[0]);
  int nn = 0;
  _TC  *pc = NULL;
  _tc_par *pp;
  _tc tc;

  if (!file) file = fopen(filename, "r");
  if (!file) {
    printf("File %s absent.\n", filename);
    return;
  }

  tc.cnt = 0;
  
  while (1) {
    char b[8192], bb[80];
    int k = -1;
    int delay;

    if (!fgets(b, sizeof(b), file)) {
      send_command_to_GPS(pc, &tc);
      break;
    }
    b[strlen(b)-(feof(file)?0:1)] = '\0';

    k = sscanf(b, "%s %d", bb, &delay);

    if (k == 2 && !strcasecmp(bb, "DELAY")) {
      send_command_to_GPS(pc, &tc);
      printf("Delay %d sec...", delay);
      sleep(delay);
      printf("resumed.\n");
    }

    for (i=0; i<n; i++) {
      if (!strcmp(b, TC[i].nam)) {
        send_command_to_GPS(pc, &tc);
        printf("Found: %s\n", TC[i].nam);
        pc = &TC[i];
        for (nn=0; nn<100; nn++) {
          pp = &pc->par[nn];
          if (!pp->nam) break;
        }
        tc.cnt = pc->len + 16;
        memset((int8*)tc.dat, 0, tc.cnt);
        unpackbytes(&tc.dat[0+0], 0x9C3E, 2);
        unpackbytes(&tc.dat[0+2], pc->ID, 2);
        for (j=0; j<nn; j++) {
          pp = &pc->par[j];
          unpackbytes(&tc.dat[8+pp->bgn], pp->def, pp->end-pp->bgn+1);
        }
        unpackbytes(&tc.dat[8+pc->len+0], 0x55555555, 4);
        unpackbytes(&tc.dat[8+pc->len+4], 0x55555555, 4);
        break;
      }
    }

    for (i=0; i<nn; i++) {
      char fmt[80];
      int32 value;
      pp = &pc->par[i];
      sprintf(fmt, "%s %s", pp->nam, pp->fmt);
      k = sscanf(b, fmt, &value);
      if (k == 1) {
        if (P > 3) {
          sprintf(fmt, "Found: %%-10s %%-20s %%16%s\n", &pp->fmt[1]);
          printf(fmt, pc->nam, pp->nam, value);
        }
        unpackbytes(&tc.dat[8+pp->bgn], value, pp->end-pp->bgn+1);
        break;
      }
    }
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

//int16 err;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage:\n");
  printf("%s <file-name> [<USCM_ID> [<JMDC_ID> [P]]]]\n",argv[0]);

  if (argc < 2) exit(1);
  filename = argv[1];
  if (argc > 2) USCM_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);
  
  printf("Filename = %s\n",   filename);
  printf("USCM_ID  = %02X\n", USCM_ID);
  printf("JMDC_ID  = %d\n",   JMDC_ID);
  printf("P        = %d\n",   P);

  read_command_file(filename);

  return 0;
}

//~============================================================================
