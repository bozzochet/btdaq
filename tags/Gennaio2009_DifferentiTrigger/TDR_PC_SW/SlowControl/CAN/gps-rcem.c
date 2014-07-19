// file gps-rcem.c
//
// Sends RCEM TeleCommand to GPS
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
  int8 source;
  int16 field_val;
  short mask_angle, mask_radius, pattern_A, pattern_n;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s field_val mask_angle mask_radius source ",argv[0]);
  printf("pattern_A pattern_n  [<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 7) exit(1);

  field_val   = atoi(argv[ 1]);
  mask_angle  = atoi(argv[ 2]);
  mask_radius = atoi(argv[ 3]);
  source      = atoi(argv[ 4]);
  pattern_A   = atoi(argv[ 5]);
  pattern_n   = atoi(argv[ 6]);

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

  printf("sending RCEM: field_val = %d, mask_angle = %d, mask_radius = %d, ",
                        field_val,      mask_angle,      mask_radius);
  printf("source = %d, pattern_A = %d, pattern_n = %d\n", 
          source,      pattern_A,      pattern_n);
  create_RCEM(field_val, mask_angle, mask_radius, source, pattern_A, 
              pattern_n, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RCEM on serial port failed");
  
  return 0;
}

//~============================================================================
