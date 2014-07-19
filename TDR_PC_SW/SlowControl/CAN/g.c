// file g.c
//
// to study TRD Gas Analyzer
//
// A.Lebedev - May-2004...

#include "uscmlib.h"

int P = 0;
int USCM_ID = 0x196;
int JMDC_ID = 0x00;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, n, nn;
  int port = 1;
  int16 err;
  int8 init[4]  = {0x02, 'Q', 0x31, 0x34};
  int8 gray[4]  = {0x02, 'N', 0x30, 0x31};
  int8 start[4] = {0x02, 'M', 0x30, 0x31};
  int8 stop[4]  = {0x02, 'M', 0x30, 0x30};

  int8 dat[256];

  setbuf(stdout, NULL);
  setup_command_path();

  printf(
    "Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level> [<Port>]]]]\n",
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
    get_short_status(dat, &err);
    if (!err) break;
  }
  if (err) {
    printf("Reboot failed!\n");
    exit(1);
  }

  enable_serial_port(port, &err);          printf("ENABLE: err=%04X\n", err);

  write_serial_port(port, 4, init, &err);  printf("INIT:   err=%04X\n", err);

  write_serial_port(port, 4, gray, &err);  printf("GRAY:   err=%04X\n", err);

  write_serial_port(port, 4, stop, &err);  printf("STOP:   err=%04X\n", err);
  flush_serial_port(port, &err);           printf("FLUSH:  err=%04X\n", err);

  write_serial_port(port, 4, start, &err); printf("START:  err=%04X\n", err);
//  usleep(10000);
  sleep(1);
  write_serial_port(port, 4, stop, &err);  printf("STOP:   err=%04X\n", err);

  nn = 0;
  for (i=0; i<256; i=i+n) {
    read_serial_port(port, 256, &n, &dat[i], &err); //printf("READ:   err=%04X\n", err);
    if (n == 0) break;
    nn = nn + n;
  }

  printf("nn = %d\n", nn);
  
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

  disable_serial_port(port, &err);

  return 0;
}

//~============================================================================
