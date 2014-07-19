// file gps-ratt.c
//
// Sends RATT TeleCommand to GPS
//
// Claude Zurbach
// April 2007.
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
  int16 frame;
  short q1, q2, q3, attitude_FOM;
  int8 attitude_val;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s frame q1 q2 q3 attitude_FOM attitude_val ", argv[0]);
  printf(" [<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 7) exit(1);

  frame        = atoi(argv[ 1]);
  q1           = atoi(argv[ 2]);
  q2           = atoi(argv[ 3]);
  q3           = atoi(argv[ 4]);
  attitude_FOM = atoi(argv[ 5]);
  attitude_val = strtoul(argv[ 6], NULL, 16);

  if (argc >  7) USCM_ID = strtoul(argv[ 7], NULL, 16);
  if (argc >  8) JMDC_ID = atoi(argv[ 8]);
  if (argc >  9) port    = atoi(argv[ 9]);
  if (argc > 10) P       = atoi(argv[10]);

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

  printf("sending RATT: frame = %d, q1 = %d, q2 = %d, q3 = %d, ",
                        frame,      q1,      q2,      q3);
  printf("attitude_FOM = %d, attitude_val = 0x%02hX\n", 
          attitude_FOM,      attitude_val);

  create_RATT(frame, q1, q2, q3, attitude_FOM, attitude_val, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RATT on serial port failed");
  
  return 0;
}

//~============================================================================
