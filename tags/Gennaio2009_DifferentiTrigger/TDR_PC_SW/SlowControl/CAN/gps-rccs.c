// file gps-rccs.c
//
// Sends RCCS TeleCommand to GPS
//
// Claude Zurbach
// May 2007
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
  int16 cmd;
  short from, to;
  int8 sat_ID[8];

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s cmd from to sat_ID[0...7] ", argv[0]);
  printf("[<JMDC ID> [<port> [P]]]]\n");

  if (argc < 12) exit(1);

  cmd       = atoi(argv[ 1]);
  from      = atoi(argv[ 2]);
  to        = atoi(argv[ 3]);
  sat_ID[0] = atoi(argv[ 4]);
  sat_ID[1] = atoi(argv[ 5]);
  sat_ID[2] = atoi(argv[ 6]);
  sat_ID[3] = atoi(argv[ 7]);
  sat_ID[4] = atoi(argv[ 8]);
  sat_ID[5] = atoi(argv[ 9]);
  sat_ID[6] = atoi(argv[10]);
  sat_ID[7] = atoi(argv[11]);

  if (argc > 12) USCM_ID = strtoul(argv[12], NULL, 16);
  if (argc > 13) JMDC_ID = atoi(argv[13]);
  if (argc > 14) port    = atoi(argv[14]);
  if (argc > 15) P       = atoi(argv[15]);

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

  printf("sending RCCS: cmd = %d, from = %d, to = %d, ",
                        cmd,      from,      to);
  printf("sat_ID[0...7] = %d, %d, %d, %d, %d, %d, %d, %d\n",
          sat_ID[0], sat_ID[1], sat_ID[2], sat_ID[3], 
          sat_ID[4], sat_ID[5], sat_ID[6], sat_ID[7]);

  create_RCCS(cmd, from, to, sat_ID, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RCCS on serial port failed");
  
  return 0;
}

//~============================================================================
