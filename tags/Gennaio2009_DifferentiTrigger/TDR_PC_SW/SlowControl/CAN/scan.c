// file scan.c
//
//  to scan CAN bus to find active nodes (uses newborn IDs)
//
//  A.Lebedev Jan 2006...
//

#include "uscmdef.h"
#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x118;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int j;
  int16 err;
  int16 first = 0;
  int16 last  = 0x1FF;
  float t_out = 0.1;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<first USCM_ID> [<last USCM_ID> [<JMDC_ID [<P> [<timeout>]]]]]\n",
          argv[0]);
  if (argc > 1) first   = strtoul(argv[1], NULL, 16);
  if (argc > 2) last    = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);
  if (argc > 5) t_out   = atof(argv[5]);
  printf("first USCM_ID = 0x%03X\n", first);
  printf("last USCM_ID  = 0x%03X\n", last);
  printf("JMDC_ID       = %d\n",     JMDC_ID);
  printf("P             = %d\n",     P);
  printf("timeout       = %3.1f\n",  t_out);

  printf("To stop the program - press \"Q\"\n");
  
  set_CAN_timeout(t_out);

/*
{
USCM_ID = first;
ping_USCM(0, NULL, NULL, NULL, &err);
initialize_EPP();
ping_USCM(0, NULL, NULL, NULL, &err);
initialize_EPP();
ping_USCM(0, NULL, NULL, NULL, &err);
initialize_EPP();
exit(1);
}
*/

  if (!initialize_EPP()) exit(1);

  while (1) {
    for (j=0; j<2 ; j++) {
      BUS_FOR_REQUEST = j;
      BUS_FOR_REPLY   = j;
      printf("CAN bus %s: USCM found:", j ? "B" : "A");
      for (USCM_ID=first; USCM_ID<=last; USCM_ID++) {
        if (kbhit()) {
          int ch = toupper(fgetc(stdin));
          switch (ch) {
          case 'Q': printf("\nStopped.\n"); exit(0);
          }
        }
        ping_USCM(0, NULL, NULL, NULL, &err);
        if (err == 0x0005) {
          printf(" - none.  CAN bus seems to be disconnected!");
          break;
        }
        if (err && (err != 0x0006)) {
          printf("Bus %s, USCM_ID = 0x%02X, error = 0x%04X\n",
                  j ? "B" : "A", USCM_ID, err);
          if (err != 0x0004) break;
        }
        if (!err) {
          int i;
          printf(" 0x%02X", USCM_ID);
          for (i=0; i<DIM(USCM_table); i++) {
            if (USCM_ID == USCM_table[i].ID) printf("=%s", USCM_table[i].name);
          }
        }
      }
      printf("\n");
    }
break;
  }
  
  return 0;
}

//~============================================================================
