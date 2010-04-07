// file gps-coa.c
//
// Sends COA TeleCommand to GPS for tests
//
// Claude Zurbach & A.Lebedev
// March 2006...
//

#include "gpslib.h"

int JMDC_ID = 0x00;
int USCM_ID = 0x161;    // 1xx stands for Newborn ID
int P  = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int8 port = 0;

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int16 err;
  int8  mode, rate;
  int32 COA_mgt;
  _tc tc;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s ON|OFF [<USCM ID> [<JMDC ID> [<port> [P]]]]\n",
          argv[0]);

  if (argc < 2) exit(1);
  if      (strcasecmp(argv[1], "ON")   == 0) {
    COA_mgt = 0x00010001;
  }
  else if (strcasecmp(argv[1], "OFF")  == 0) {
    COA_mgt = 0x00010000;
  }
  else {
    printf("Wrong first argument\n");
    exit(1);
  }

  if (argc > 2) USCM_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID = atoi(argv[3]);
  if (argc > 4) port    = atoi(argv[4]);
  if (argc > 5) P       = atoi(argv[5]);

  printf("COA_mgt = %08X\n", COA_mgt);
  printf("USCM_ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("port    = %d\n", port);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  setup_serial_port(port, 0x07, 2, &err);
  if (err) EXIT("setup serial port failed");
  check_serial_port(port, &mode, &rate, &err);
  if (err) EXIT("check serial port failed");

  printf("check serial port %d: mode = 0x%02X, rate = %d\n", port, mode, rate);

//~--

  printf("sending COA: COA_msg=%08X\n", COA_mgt);
  create_COA(COA_mgt, &tc);
  printf("tc.cnt = %d\n", tc.cnt);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write message to serial port failed");
  
  return 0;
}

//~============================================================================
