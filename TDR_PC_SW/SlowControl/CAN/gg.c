// file gg.c
//
// to study TRD Gas Analyzer (see also file g.c)
//
// A.Lebedev - Mar-2005...

#include "uscmlib.h"

int P = 0;
int USCM_ID = 0x196;
int JMDC_ID = 0x00;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int port = 1;

//~----------------------------------------------------------------------------

void ndd_snd_rcv(int8 *cmd) {

  int16 err;
  int n;
#if debug
  int8 dat[8];
#endif
  int i;
  
  n = 4;
  printf("W: n=%d: ", n);
  for (i=0; i<n; i++) printf("%02X ", cmd[i]);
  printf("\n");
  write_serial_port(port, n, cmd, &err);

#if debug
  read_serial_port(port, &n, dat, &err);
  printf("R: n=%d: ", n);
  for (i=0; i<n; i++) printf("%02X ", dat[i]);
  printf("\n");
//sleep(1);
  read_serial_port(port, &n, dat, &err);
  printf("R: n=%d: ", n);
  for (i=0; i<n; i++) printf("%02X ", dat[i]);
  printf("\n");
//sleep(1);
  read_serial_port(port, &n, dat, &err);
  printf("R: n=%d: ", n);
  for (i=0; i<n; i++) printf("%02X ", dat[i]);
  printf("\n");
#endif
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, n, nn;
  int16 err;
  int8 init[4]  = {0x02, 'Q', 0x31, 0x34};
  int8 start[4] = {0x02, 'M', 0x30, 0x31};
  int8 stop[4]  = {0x02, 'M', 0x30, 0x30};
//int8 junk[4]  = {0x02, 'A', 0x30, 0x30};

  int8 dat[256];

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level> [<Port>]]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) P = atoi(argv[3]);

  if (argc > 4) port = atoi(argv[4]);
  if (port != 0 && port != 1) {
    printf("<Port> is wrong! Execution terminated!!\n");
    exit(1);
  }

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);
  printf("port    = %d\n", port);

  if (!initialize_EPP()) exit(1);

  boot_from_FLASH(&err);
  for (i=0; i<3; i++) {
#ifdef USE_NEW
    get_short_status(dat, &err);
#endif
#ifdef USE_OLD
    ping(dat, &err);
#endif
    if (!err) break;
  }

  enable_serial_port(port, &err);
  ndd_snd_rcv(init);
  ndd_snd_rcv(stop);
  flush_serial_port(port, &err);

  ndd_snd_rcv(start);
  usleep(10000);
//sleep(1);
  ndd_snd_rcv(stop);

  nn = 0;
  for (i=0; i<256; i=i+n) {
    read_serial_port(port, 256-nn, &n, &dat[i], &err);
    if (n == 0) break;
    nn = nn + n;
  }

  for (i=0; i<nn; i++) {
    if (!(i % 7)) printf("\n");
    printf("%02X ", dat[i]);
  }
  printf("\n");

  for (i=0; i<nn; i++) {
    if ((i % 7) == 0) printf("\n");
    if ((i % 7) == 0) printf("%02XH ", dat[i]);
    else              printf("%c ", dat[i]);
  }
  printf("\n");

#if ddebug
  for (i=0; i<nn; i++) {
    int8 c = dat[i];
    if (c == 0x02) printf("\n");
    if (c == 0x02) printf("%02XH ", c);
    else           printf("%c ", c);
  }
  printf("\n");
#endif

  disable_serial_port(port, &err);

  return 0;
}

//~============================================================================
