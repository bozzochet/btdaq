// file philip-1.c
//
// Program to read events from SDR2, JINF, JLV1, JINJ
//
// A.Lebedev May-2007...

#include "daqlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~----------------------------------------------------------------------------

bool find_event_fragment(
     int n, int16 *b, int ID, int *nn, int16 **bb, int16 *event_nb) {
  
  int16 *p, *pp;
  
  *event_nb = b[0];
  
  if (ID < 0) {
    *nn = n - 1;
    *bb = b + 1;
    return TRUE;
  }
  
  p  = b + 1;

  while (1) {
    pp = p + *p;
    if (pp >= b + n) {
      return FALSE;
    }
    else {
      if ((*pp & 0x001F) == ID) {
        *bb = p + 1;
        *nn = *p;
        return TRUE;
      }
      else {
        p = pp + 1;
      }
    }
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int nev = 0, nr = 0;
  int16 link = 1;
  int ID = -1;
  int n, nn;
  int16 b[16384], *bb, event_nb;
  bool got_event, fragment_found;
  struct timeval now, then;
  int16 err = 0;
  
  setvbuf(stdout, NULL, _IOFBF, 0);
  setup_command_path();

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  fprintf(stderr, "Usage: %s [port [{Path|0} [ID [link (1-9) [nev [P]]]]]]\n",
          argv[0]);
  i = 1;
  if (argc > i) TX = RX = atoi(argv[i++]);
  if (argc > i) AMSW_PATH[0] = strtoul(argv[i++], NULL, 16);
  if (argc > i) link = atof(argv[i++]);
  if (argc > i) ID   = atof(argv[i++]);
  if (argc > i) nev  = atof(argv[i++]);
  if (argc > i) P    = atoi(argv[i++]);
  
  fprintf(stderr, "port = %d\n", TX);
  fprintf(stderr, "Path = 0x%04X\n", AMSW_PATH[0]);
  fprintf(stderr, "link = %d\n", link);
  fprintf(stderr, "ID   = %d", ID);
  fprintf(stderr, " %s", ID<0 ? "(SDR2 event" : "(JINJ event");
  if (ID >= 0) fprintf(stderr, ", link %d", ID);
  fprintf(stderr, ")\n");
  fprintf(stderr, "nev  = %d", nev);
  fprintf(stderr, "%s", nev ? "\n" : " (means forever)\n");
  fprintf(stderr, "P    = %d\n", P);

  reset_JINF_event_FIFO(&err);
  if (err) {
    fprintf(stderr, "reset_JINF_event_FIFO failed\n");
    exit(1);
  }

  gettimeofday(&then, NULL);

  while (1) {
    got_event = get_JINF_event(&n, b, &err);
    if (err) {
      fprintf(stderr, "get_JINF_event failed\n");
      exit(1);
    }
    if (!link) continue;
    if (got_event) {
      fragment_found = find_event_fragment(n, b, ID, &nn, &bb, &event_nb);
      if (fragment_found) {
        for (i=0; i<90; i++) {
          if ((bb[i] & 0xF000) == (link<<12)) printf("%04X ", bb[i]);
        }
        printf("\n");
      }
      nr = nr + 1;
      if (nev && nr >= nev) break;
    }
  }

  gettimeofday(&now, NULL);
  fprintf(stderr, "%.2f events per second\n", (float)(nr)/delta_t(&now, &then));

  return 0;
}

//~============================================================================
