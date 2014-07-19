// file mca-1.c
//
// Program to study communications with MCA - A.Lebedev Jan-2006...

#include "mcalib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x1A0;    // 1xx stands for Newborn ID
int P = 0;
int PP = 1;

int8 port = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int8  mode, rate;
  int8  MCA_status[20];
  int i;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level> [<Port>]]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
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


  for (port=0; port<2; port++) {
    for (rate=2; rate<4; rate++) {
      setup_serial_port(port, 0x07, rate, &err);
      if (err) {
        printf("enable_serial_port(%d,..: err = %04X\n", port, err);
        exit(1);
      }
      printf("serial port %d enabled: mode=0x07 rate=%d\n", port, rate);

      check_serial_port(port, &mode, &rate, &err);
      if (err) {
        printf("check_serial_port(%d,..: err = %04X\n", port, err);
        exit(1);
      }
      printf("serial port %d: mode = 0x%02X, rate = %d\n", port, mode, rate);

      read_MCA_status(MCA_status, &err);
      printf("err=%04X: ", err);
      if (!err) for (i=0; i<20; i++) printf("%02hhX ", MCA_status[i]);
      printf("\n");
    }
  }

  return 0;
}

//~============================================================================
