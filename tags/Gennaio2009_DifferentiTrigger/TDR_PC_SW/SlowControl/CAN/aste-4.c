// file aste-4.c
//
// to test ASTE board
// (note, does NOT setup USCM serial ports
//
// A.Lebedev - Feb-2007...

#include "uscmlib.h"

int P = 0;
int USCM_ID = 0x161;
int JMDC_ID = 0x00;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int8 port = 0;
  int16 err;

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

  while (1) {
    if (kbhit()) {
      int i, n_rcv;
      int8 rcv[4096];
      int ch = fgetc(stdin);
      int8 cha = (int8)ch;
      printf("%c: ", cha);
      write_serial_port(port, 1, &cha, &err);
//    Read_serial_port(port, 4096, &n_rcv, rcv, &err);
      read_serial_port(port, 4096, &n_rcv, rcv, &err);
      if (cha == 'o' || cha == 'p') for (i=0; i<n_rcv; i++) printf(" %02hX", rcv[i]);
      else                          for (i=0; i<n_rcv; i++) printf("%c", rcv[i]);
      printf("%s", n_rcv ? "\n" : " no reply\n");
      Read_serial_port(port, 4096, &n_rcv, rcv, &err);
      if (n_rcv) printf("more...\n");
    }
  }

  return 0;
}

//~============================================================================
