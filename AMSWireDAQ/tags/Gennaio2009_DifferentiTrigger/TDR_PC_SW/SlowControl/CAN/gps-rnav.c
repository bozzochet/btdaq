// file gps-rnav.c
//
// Sends RNAV TeleCommand to GPS
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
  short week;
  long second, sec_fract, pos_x, pos_y, pos_z, vel_x, vel_y, vel_z;
  long drift, pos_FOM, vel_FOM, drift_FOM, time_FOM;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s week second sec_fract pos_x pos_y pos_z ", argv[0]);
  printf("vel_x vel_y vel_z drift pos_FOM vel_FOM drift_FOM time_FOM ");
  printf("[<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 15) exit(1);
  week      = atoi(argv[ 1]);
  second    = atoi(argv[ 2]);
  sec_fract = atoi(argv[ 3]);
  pos_x     = atoi(argv[ 4]);
  pos_y     = atoi(argv[ 5]);
  pos_z     = atoi(argv[ 6]);
  vel_x     = atoi(argv[ 7]);
  vel_y     = atoi(argv[ 8]);
  vel_z     = atoi(argv[ 9]);
  drift     = atoi(argv[10]);
  pos_FOM   = atoi(argv[11]);
  vel_FOM   = atoi(argv[12]);
  drift_FOM = atoi(argv[13]);
  time_FOM  = atoi(argv[14]);

  if (argc > 15) USCM_ID = strtoul(argv[15], NULL, 16);
  if (argc > 16) JMDC_ID = atoi(argv[16]);
  if (argc > 17) port    = atoi(argv[17]);
  if (argc > 18) P       = atoi(argv[18]);

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

  printf("sending RNAV: week = %d, second = %ld, sec_fract = %ld, ",
                        week,      second,       sec_fract);
  printf("pos_x = %ld, pos_y = %ld, pos_z = %ld, vel_x = %ld, vel_y = %ld, ",
          pos_x,       pos_y,      pos_z,      vel_x,      vel_y);
  printf("vel_z = %ld, drift = %ld, pos_FOM = %ld, vel_FOM = %ld, ",
          vel_z,       drift,       pos_FOM,       vel_FOM);
  printf("drift_FOM = %ld, time_FOM = %ld\n", 
          drift_FOM,       time_FOM);

  create_RNAV(week, second, sec_fract, pos_x, pos_y, pos_z, vel_x, vel_y, vel_z,
              drift, pos_FOM, vel_FOM, drift_FOM, time_FOM, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RNAV on serial port failed");
  
  return 0;
}

//~============================================================================
