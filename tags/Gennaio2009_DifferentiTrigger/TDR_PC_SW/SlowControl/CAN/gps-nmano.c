// file gps-nmano.c
//
// Sends NMANO TeleCommand to GPS
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
  int8 thrust_error, thrust_type;
  short day;
  long thrust_duration;
  float day_fraction, axis_1st, axis_2nd, axis_3rd, mass, impulse;
  float cmaxis_1st, cmaxis_2nd, cmaxis_3rd;
  float cmmaxis_1st, cmmaxis_2nd, cmmaxis_3rd;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s day day_fraction thrust_error thrust_type ", argv[0]);
  printf("thrust_duration axis_1st axis_2nd axis_3rd mass impulse ");
  printf("cmaxis_1st cmaxis_2nd cmaxis_3rd cmmaxis_1st, cmmaxis_2nd ");
  printf("cmmaxis_3rd [<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 17) exit(1);
  day             = atoi(argv[ 1]);
  day_fraction    = atof(argv[ 2]);
  thrust_error    = atoi(argv[ 3]);
  thrust_type     = atoi(argv[ 4]);
  thrust_duration = atoi(argv[ 5]);
  axis_1st        = atof(argv[ 6]);
  axis_2nd        = atof(argv[ 7]);
  axis_3rd        = atof(argv[ 8]);
  mass            = atof(argv[ 9]);
  impulse         = atof(argv[10]);
  cmaxis_1st      = atof(argv[11]);
  cmaxis_2nd      = atof(argv[12]);
  cmaxis_3rd      = atof(argv[13]);
  cmmaxis_1st     = atof(argv[14]);
  cmmaxis_2nd     = atof(argv[15]);
  cmmaxis_3rd     = atof(argv[16]);

  if (argc > 17) USCM_ID = strtoul(argv[17], NULL, 16);
  if (argc > 18) JMDC_ID = atoi(argv[18]);
  if (argc > 19) port    = atoi(argv[19]);
  if (argc > 20) P       = atoi(argv[20]);

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

  printf("sending NORBIT: day= %d, day_fraction = %f, thrust_error = %d, ",
                          day,     day_fraction,      thrust_error);
  printf("thrust_type = %d, thrust_duration = %ld, axis_1st = %f, ",
          thrust_type,      thrust_duration,       axis_1st);
  printf("axis_2nd = %f, axis_3rd = %f, mass = %f, impulse = %f, ",
          axis_2nd,      axis_3rd,      mass,      impulse);
  printf("cmaxis_1st = %f, cmaxis_2nd = %f, cmaxis_3rd = %f, ",
          cmaxis_1st,      cmaxis_2nd,      cmaxis_3rd);
  printf("cmmaxis_1st = %f, cmmaxis_2nd = %f cmmaxis_3rd = %f\n", 
          cmmaxis_1st,      cmmaxis_2nd,     cmmaxis_3rd);
          
  create_NMANO(day, day_fraction, thrust_error, thrust_type, thrust_duration,
               axis_1st, axis_2nd, axis_3rd, mass, impulse, cmaxis_1st,
               cmaxis_2nd, cmaxis_3rd, cmmaxis_1st, cmmaxis_2nd, cmmaxis_3rd,
               &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write NMANO on serial port failed");
  
  return 0;
}

//~============================================================================
