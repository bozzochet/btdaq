// file c.c
//
// to study USCM console commands via CAN-bus
//
// A.Lebedev - Aug-2004...

#include "uscmlib.h"

int P = 0;
int USCM_ID = 0x91;
int JMDC_ID = 0x00;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, n;
  int16 err;
  int8 dat[8];

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n", argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

#if old
  while (1) {
    if (kbhit()) {
      int ch = fgetc(stdin);
      int8 cha = (int8)ch;
      if (ch == '\n') cha = '\r';
      else            cha = (int8)ch;
      write_console(1, &cha, &err);
      while (1) {
        read_console(&n, &dat[0], &err);
        if (n == 0) break;
        for (i=0; i<n; i++) printf("%c", dat[i]);
      }
    }
  }
#endif

  while (1) {
    if (kbhit()) {
      int ch = fgetc(stdin);
      int8 cha = (int8)ch;
      if (ch == '\n') cha = '\r';
      else            cha = (int8)ch;
      write_console(1, &cha, &err);
    }
    while (1) {
      read_console(&n, &dat[0], &err);
      if (n == 0) break;
      for (i=0; i<n; i++) printf("%c", dat[i]);
    }
  }

  return 0;
}

//~============================================================================
