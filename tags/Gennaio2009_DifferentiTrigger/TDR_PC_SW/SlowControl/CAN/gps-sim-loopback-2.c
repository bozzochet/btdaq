// file gps-sim-loopback-2.c
//
// GPS-simulator gets a byte from RS422 and sends it back.
// (GPS-simulator is used as RS232 <--> RS422 converter only)
//
// A.Lebedev Apr-2007...

#include "uscmlib.h"

int JMDC_ID = 0x02;
int USCM_ID = 0x166;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~======================================================================================

int main(int argc, char* argv[]) {

  int port = 0;
  int8 dat[4096];
  int  cnt;
  int16 err;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM_ID> [<JMDC_ID> [<port> [<P>]]]]]\n",argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) port    = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);

  printf("USCM_ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("port    = %d\n", port);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  setup_serial_port(port, 0x07, 3, &err);
  if (err) PANIC("setup_serial_port failed\n");

  while (1) {
    read_serial_port(port, sizeof(dat), &cnt, dat, &err);
    if (err) PANIC("read_serial_port failed\n");
    if (cnt) {
      write_serial_port(port, cnt, dat, &err);
      if (err) PANIC("Write_serial_port failed\n");
    }
  }

  return 0;
}

//~======================================================================================
