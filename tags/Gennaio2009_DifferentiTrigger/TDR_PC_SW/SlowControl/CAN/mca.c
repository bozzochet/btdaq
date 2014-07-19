// file mca.c
//
// Program to study communications with MCA - A.Lebedev Mar-2005...

#include "mcalib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x14E;    // 1xx stands for Newborn ID
int P = 1;
int PP = 2;

int8 port = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int8  mode, rate;
  int8  MCA_status[20];
  int16 chan;
  int32 cont[50];
  int i, j;
  
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

  enable_serial_port(port, &err);
  if (err) {
    printf("enable_serial_port(%d,..: err = %04X\n", port, err);
    exit(1);
  }
  printf("serial port %d enabled.\n", port);

  check_serial_port(port, &mode, &rate, &err);
  if (err) {
    printf("check_serial_port(%d,..: err = %04X\n", port, err);
    exit(1);
  }
  printf("check serial port %d: mode = 0x%02X, rate = %d\n", port, mode, rate);

  power_MCA_up(&err);
  printf("err=%04X\n", err);

  read_MCA_status(MCA_status, &err);
  printf("err=%04X: ", err);
  for (i=0; i<20; i++) printf("%02hhX ", MCA_status[i]);
  printf("\n");

  stop_MCA_DAQ(&err);
  printf("err=%04X\n", err);

  clear_MCA_memory(&err);
  printf("err=%04X\n", err);

  stop_MCA_DAQ(&err);                 // yes, this "stop" is needed
  printf("err=%04X\n", err);

  start_MCA_DAQ(&err);
  printf("err=%04X\n", err);

  sleep(2);
  
  stop_MCA_DAQ(&err);
  printf("err=%04X\n", err);

  transfer_MCA_data(&err);
  printf("err=%04X\n", err);

  reset_MCA_counter(&err);
  printf("err=%04X\n", err);

  for (j=0; j<20; j++) {
    read_MCA_50_channels(&chan, cont, &err);
    printf("err=%04X\n", err);
    if (!err) for (i=0; i<50; i++) printf("%4u: %10u\n", chan+i, cont[i]);
  }
exit(1);

  reset_MCA_counter(&err);
  printf("err=%04X\n", err);

  for (i=0; i<100; i++) {
    read_MCA_channel(&chan, cont, &err);
    printf("err=%04X\n", err);
    if (!err) printf("%4u: %10u\n", chan, cont[0]);
  }

  return 0;
}

//~============================================================================
