// file gps-ninit.c
//
// Sends NINIT TeleCommand to GPS
//
// Claude Zurbach
// May 2007.
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
  int8 function;
  int16 day;
  real day_fraction;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s day day_fraction function ", argv[0]);
  printf("[<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 4) exit(1);
  day          = atoi(argv[1]);
  day_fraction = atof(argv[2]);
  function     = atoi(argv[3]);

  if (argc > 4) USCM_ID = strtoul(argv[4], NULL, 16);
  if (argc > 5) JMDC_ID = atoi(argv[5]);
  if (argc > 6) port    = atoi(argv[6]);
  if (argc > 7) P       = atoi(argv[7]);

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

  printf("sending NINIT: day = %d, day_fraction = %Lf, function = %d\n", 
                         day,      day_fraction,       function);
  create_NINIT(day, day_fraction, function, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write NINIT on serial port failed");
  
  return 0;
}

//~============================================================================
