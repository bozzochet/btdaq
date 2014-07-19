// file gps-neco.c
//
// Sends NECO TeleCommand to GPS
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
  int8 cov_orbit, cov_clock, cov_ind, cov_type;
  short day;
  float day_fraction, sma_axis, ex, ey, hx, hy, position;
  float clock_bias, clock_drift, clock_drate, coefficient;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s day day_fraction cov_orbit cov_clock cov_ind ", argv[0]);
  printf("cov_type sma_axis ex ey hx hy position clock_bias clock_drift "); 
  printf("clock_drate coefficient [<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 16) exit(1);

  day          = atoi(argv[ 1]);
  day_fraction = atof(argv[ 2]);
  cov_orbit    = atoi(argv[ 3]);
  cov_clock    = atoi(argv[ 4]);
  cov_ind      = atoi(argv[ 5]);
  cov_type     = atoi(argv[ 6]);
  sma_axis     = atof(argv[ 7]);
  ex           = atof(argv[ 8]);
  ey           = atof(argv[ 9]);
  hx           = atof(argv[10]);
  hy           = atof(argv[11]);
  position     = atof(argv[12]);
  clock_bias   = atof(argv[13]);
  clock_drift  = atof(argv[14]);
  clock_drate  = atof(argv[15]);
  coefficient  = atof(argv[16]);

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

  printf("sending NECO: day= %d, day_fraction = %f, cov_orbit = %d, ",
                        day,     day_fraction,      cov_orbit);
  printf("cov_clock = %d, cov_ind = %d, cov_type = %d, ",
          cov_clock,      cov_ind,      cov_type);
  printf("sma_axis = %f, ex = %f, ey = %f, hx = %f, hy = %f, ",
          sma_axis,      ex,      ey,      hx,      hy);
  printf("position = %f, clock_bias = %f, clock_drift = %f, ",
          position,      clock_bias,      clock_drift);
  printf("clock_drate = %f, coefficient = %f\n",
          clock_drate,      coefficient);

  create_NECO(day, day_fraction, cov_orbit, cov_clock, cov_ind, cov_type,
              sma_axis, ex, ey, hx, hy, position, clock_bias, clock_drift,
              clock_drate, coefficient, &tc);
              
  to_GPS(port, &tc, &err);
  if (err) EXIT("write NECO on serial port failed");
  
  return 0;
}

//~============================================================================
