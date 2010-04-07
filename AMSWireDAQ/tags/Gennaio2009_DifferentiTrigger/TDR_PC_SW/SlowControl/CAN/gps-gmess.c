// file gps-gmess.c
//
// Sends GMESS TeleCommand to GPS for tests
//
// Claude Zurbach & A.Lebedev
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

  int16 err;
  int8  mode, rate;
  _tc tc;
  int32 TM_ID;
  long period;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s TM_ID period [<USCM ID> [<JMDC ID> [<port> [P]]]]\n",
          argv[0]);

  if (argc < 3) exit(1);
  TM_ID  = atoi(argv[1]);
  period = atoi(argv[2]);

  if (argc > 3) USCM_ID = strtoul(argv[3], NULL, 16);
  if (argc > 4) JMDC_ID = atoi(argv[4]);
  if (argc > 5) port    = atoi(argv[5]);
  if (argc > 6) P       = atoi(argv[6]);

  printf("TM_ID   = %d\n", TM_ID);
  printf("period  = %ld\n", period);
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

  printf("sending GMESS: TM_ID=%2d, period=%ld\n", TM_ID, period);
  create_GMESS(TM_ID, period, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write GMESS on serial port failed");
  
  return 0;
}

//~============================================================================
