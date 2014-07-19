// file gps-rctr.c
//
// Sends RCTR TeleCommand to GPS
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
  short code_K[3], carrier_K[3], interf_K[3];
  int8 correl, chip;
  short track_01, track_02, track_03;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s field_val ", argv[0]);
  printf("code_K[0...2] carrier_K[0...2] interf_K[0...2] correl ");
  printf("chip track_01 track_02 track_03 ");
  printf("[<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 16) exit(1);

  field_val    = atoi(argv[ 1]);
  code_K[0]    = atof(argv[ 2]);
  code_K[1]    = atof(argv[ 3]);
  code_K[2]    = atof(argv[ 4]);
  carrier_K[0] = atof(argv[ 5]);
  carrier_K[1] = atof(argv[ 6]);
  carrier_K[2] = atof(argv[ 7]);
  interf_K[0]  = atof(argv[ 8]);
  interf_K[1]  = atof(argv[ 9]);
  interf_K[2]  = atof(argv[10]);
  correl       = atoi(argv[11]);
  chip         = atoi(argv[12]);
  track_01     = atoi(argv[13]);
  track_02     = atoi(argv[14]);
  track_03     = atoi(argv[15]);

  if (argc > 16) USCM_ID = strtoul(argv[16], NULL, 16);
  if (argc > 17) JMDC_ID = atoi(argv[17]);
  if (argc > 18) port    = atoi(argv[18]);
  if (argc > 19) P       = atoi(argv[19]);

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

  printf("sending RCTR: field_val = %d, ", field_val);
  printf("code_K[0...2] = %d, %d, %d, ", code_K[0], code_K[1], code_K[2]);
  printf("carrier_K[0...2] = %d, %d, %d, ", 
          carrier_K[0], carrier_K[1], carrier_K[2]);
  printf("interf_K[0...2] = %d, %d, %d, ", 
          interf_K[0], interf_K[1], interf_K[2]);
  printf("track_02 = %d, track_03 = %d\n", track_02, track_03);
  
  create_RCTR(field_val, code_K, carrier_K, interf_K, correl, chip, 
              track_01, track_02, track_03, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RCTR on serial port failed");
  
  return 0;
}

//~============================================================================
