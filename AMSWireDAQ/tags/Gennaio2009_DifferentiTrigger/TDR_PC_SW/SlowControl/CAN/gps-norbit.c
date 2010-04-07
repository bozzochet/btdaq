// file gps-norbit.c
//
// Sends NORBIT TeleCommand to GPS
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
  short day;
  double day_fraction, axis, ex, ey, hx, hy, orbit;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s day day_fraction axis ex ey hx hy orbit ", argv[0]);
  printf("[<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 9) exit(1);
  day          = atoi(argv[ 1]);
  day_fraction = atof(argv[ 2]);
  axis         = atof(argv[ 3]);
  ex           = atof(argv[ 4]);
  ey           = atof(argv[ 5]);
  hx           = atof(argv[ 6]);
  hy           = atof(argv[ 7]);
  orbit        = atof(argv[ 8]);

  if (argc >  9) USCM_ID = strtoul(argv[ 9], NULL, 16);
  if (argc > 10) JMDC_ID = atoi(argv[10]);
  if (argc > 11) port    = atoi(argv[11]);
  if (argc > 12) P       = atoi(argv[12]);

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

  printf("sending NORBIT: day= %d, day_fraction = %f, axis = %f, ",
                          day,     day_fraction,      axis);
  printf("ex = %f, ey = %f, hx = %f, hy = %f, orbit = %f\n",
          ex,      ey,      hx,      hy,      orbit);

  create_NORBIT(day, day_fraction, axis, ex, ey, hx, hy, orbit, &tc);
                
  to_GPS(port, &tc, &err);
  if (err) EXIT("write NORBIT on serial port failed");
  
  return 0;
}

//~============================================================================
