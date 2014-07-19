// file read-events.c
//
// Program to read events from CDR2, JINF, JLV1, JINJ
//
// A.Lebedev May-2007...

#include "daqlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i, n;
  int n1 = 0, n2 = 0;
  int16 b[16384];
  bool get_event;
  struct timeval now, then;
  int16 err;
  
  setvbuf(stdout, NULL, _IOFBF, 0);
  setup_command_path();

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  fprintf(stderr, "Usage: %s [TX&RX [{Path | 0} [P]]]\n", argv[0]);
  if (argc > 1) TX = RX = atoi(argv[1]);
  if (argc > 2) AMSW_PATH[0] = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);
  
  fprintf(stderr, "TX&RX = %d\n", TX);
  fprintf(stderr, "Path  = 0x%04X\n", AMSW_PATH[0]);
  fprintf(stderr, "P     = %d\n", P);

  reset_JINF_event_FIFO(&err);
  if (err) {
    fprintf(stderr, "reset_JINF_event_FIFO failed\n");
    exit(1);
  }

  gettimeofday(&then, NULL);

  while (1) {
    get_event = get_JINF_event(&n, b, &err);
    n1 = n1 + 1;
    if (err) {
      fprintf(stderr, "get_JINF_event failed\n");
      exit(1);
    }
    if (get_event) {
      n2 = n2 + 1;
      for (i=0; i<90; i++) {
        printf("%04X ", b[i]);
      }
      printf("\n");
    }
    if (!(n1 % 1000)) {
      float d;
      gettimeofday(&now, NULL);
      d = delta_t(&now, &then);
      fprintf(stderr, "%.2f (%.2f) per second\n", (float)(n1)/d, (float)(n2)/d);
      n1 = 0;
      n2 = 0;
      gettimeofday(&then, NULL);
    }
  }

  return 0;
}

//~============================================================================
