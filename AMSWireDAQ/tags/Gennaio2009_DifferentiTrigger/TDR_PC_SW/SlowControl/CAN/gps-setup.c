// file gps-setup.c
//
// Program to setup GPS for tests
//
// Claude Zurbach (seria-2bis.c) & Alexei Lebedev
// November 2005...
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

  int i;
  int16 err;
  int8  mode, rate;
  _tc tc;
  int32 COA_mgt;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<port> [P]]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) port    = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);

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

  printf("sending WMODE 4321...\n");
  create_WMODE(0x4321, 1, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write WMODE on serial port failed");
  delay(15.0);

  printf("sending WMODE 1234...\n");
  create_WMODE(0x1234, 1, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write WMODE on serial port failed");
  delay(7.0);

  for (i=0x20; i<0x25; i++) {
    printf("sending RCCS: cmd = 0x%2X\n", i);
//  create_RCCS(i, -7000, 7000, &tc);
    create_RCCS(i, -7000, 7000, NULL, &tc);
    to_GPS(port, &tc, &err);
    if (err) EXIT("write RCCS on serial port failed");
    delay(1.0);
  }

  printf("sending RATT...\n");
//create_RATT(&tc);
  create_RATT(2, 0, 0, 0, 0, 0xFF, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RATT on serial port failed");
  delay(1.0);

  COA_mgt = 0x00010000;
  printf("sending COA: COA_mgt = %08X\n", COA_mgt);
  create_COA(COA_mgt, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write COA on serial port failed");
  delay(1.0);

#if 0
  printf("sending RTIME: week=1234, second=654321\n");
  create_RTIME(1234, 4321, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write rtime on serial port failed");
  delay(2.0);
#endif

#if 0
  for (i=2; i<30; i++) {
    printf("sending GMESS: TM_ID=%2d, period=%d\n", i, 0);
    create_GMESS(i, 0, &tc);
    to_GPS(port, &tc, &err);
    if (err) EXIT("write GMESS on serial port failed");
    delay(1.0);
  }
#endif
  
  printf("sending GMESS: TM_ID=%2d, period=%d\n", 3, 1);
  create_GMESS(3, 1, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write GMESS on serial port failed");
  delay(1.0);
  
  return 0;
}

//~============================================================================
