// file gps-rcacq.c
//
// Sends RCACQ TeleCommand to GPS
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
  int16 field_val;
  short ID_algo, acq_thresh, intg_time, search_range;
  long sweep_rate;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s field_val ID_algo acq_thresh sweep_rate ", argv[0]);
  printf("intg_time search_range [<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 7) exit(1);

  field_val    = atoi(argv[ 1]);
  ID_algo      = atoi(argv[ 2]);
  acq_thresh   = atoi(argv[ 3]);
  sweep_rate   = atoi(argv[ 4]);
  intg_time    = atoi(argv[ 5]);
  search_range = atoi(argv[ 6]);

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

  printf("sending RCACQ: field_val = %d, ID_algo = %d, acq_thresh = %d, ",
                         field_val,      ID_algo,      acq_thresh);
  printf("sweep_rate = %ld, intg_time = %d, search_range = %d\n", 
          sweep_rate,       intg_time,      search_range);

  create_RCACQ(field_val, ID_algo, acq_thresh, sweep_rate, intg_time, 
               search_range, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RCACQ on serial port failed");
  
  return 0;
}

//~============================================================================
