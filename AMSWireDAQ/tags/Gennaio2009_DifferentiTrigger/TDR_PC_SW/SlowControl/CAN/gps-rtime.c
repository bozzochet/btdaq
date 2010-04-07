// file gps-rtime.c
//
// Sends RTIME TeleCommand to GPS
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

  int i, ind;
  int16 err;
  int8  mode, rate;
  _tc tc;
  int16 typ;
  short week;
  long second;

  struct {
    int16 typ;
    char* txt;
  } dat_typ[5] = {
    {0x0001, "GPS absolute Time"},
    {0x0011, "GPS relative Time"},
    {0x0100, "Local clock drift"},
    {0x1000, "Synchronization message"},
    {0x8001, "GPS Time - UTC"}};

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s {", argv[0]);
  for (i=0; i<5; i++) printf("%04X%s", dat_typ[i].typ, (i == 4) ? "} " : " | ");
  printf("week second [<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 4) exit(1);
  
  typ    = strtoul(argv[1], NULL, 16);
  week   = atoi(argv[2]);
  second = atoi(argv[3]);

  ind = -1;
  for (i=0; i<5; i++) {
    if (typ == dat_typ[i].typ) {
      ind = i;
      break;
    }
  }

  if (ind < 0) exit(1);
  if (argc > 4) USCM_ID = strtoul(argv[4], NULL, 16);
  if (argc > 5) JMDC_ID = atoi(argv[5]);
  if (argc > 6) port    = atoi(argv[6]);
  if (argc > 7) P       = atoi(argv[7]);

  printf("USCM_ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("port    = %d\n", port);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  setup_serial_port(port, 0x07, 3, &err);
  if (err) EXIT("setup serial port failed");
  check_serial_port(port, &mode, &rate, &err);
  if (err) EXIT("check serial port failed");

  printf("check serial port %d: mode = 0x%02X, rate = %d\n", port, mode, rate);

//~--

  printf("sending RTIME: typ = 0x%04X (%s), week = %d, second = %ld\n",
         typ, dat_typ[ind].txt, week, second);
  create_RTIME(typ, week, second, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RTIME on serial port failed");
  
  return 0;
}

//~============================================================================
