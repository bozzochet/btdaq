// file gps-wmode.c
//
// Sends WMODE TeleCommand to GPS for tests
//
// Claude Zurbach & A.Lebedev
// May 2006...
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
  bool PPS;

  struct {
    char *nam;
    int16 cmd;
    char* txt;
  } cmd[4] = {
    {"SB", 0x4333, "N -> SB"},
    {"I",  0x4321, "N or DO -> I"},
    {"N",  0x1234, "I or SB -> N"},
    {"DO", 0x1414, "I -> DO"}};

  char *config_PPS[2] = {"free", "locked"};
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s {", argv[0]);
  for (i=0; i<4; i++) printf("%s%s", cmd[i].nam, (i == 3) ? "} " : " | ");
  printf("PPS=0,1 [<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 3) exit(1);

  ind = -1;
  for (i=0; i<4; i++) {
    if (!strcasecmp(argv[1], cmd[i].nam)) {
      ind = i;
      break;
    }
  }
  if (ind < 0) exit(1);

  PPS = atoi(argv[2]);
  if (PPS > 1) exit(1);

  i = 3;
  if (argc > i) USCM_ID = strtoul(argv[i++], NULL, 16);
  if (argc > i) JMDC_ID = atoi(argv[i++]);
  if (argc > i) port    = atoi(argv[i++]);
  if (argc > i) P       = atoi(argv[i++]);

  printf("cmd     = %04X (%s)\n", cmd[ind].cmd, cmd[ind].txt);
  printf("PPS     = %d (%s)\n", PPS, config_PPS[PPS]);
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

  printf("sending WMODE: cmd = 0x%04X (%s), PPS = %d (%s)\n", 
         cmd[ind].cmd, cmd[ind].txt, PPS, config_PPS[PPS]);
  create_WMODE(cmd[ind].cmd, PPS, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write WMODE on serial port failed");
  
  return 0;
}

//~============================================================================
