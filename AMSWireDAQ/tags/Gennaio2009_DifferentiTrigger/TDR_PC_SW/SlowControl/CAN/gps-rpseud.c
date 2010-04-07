// file gps-rpseud.c
//
// Sends RPSEUD TeleCommand to GPS
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
  int16 cha_ID, cmd, sig_src;
  short sig_src_PRN, doppler;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s cha_ID cmd sig_src sig_src_PRN doppler ", argv[0]);
  printf("[<USCM ID> [<JMDC ID> [<port> [P]]]]\n");

  if (argc < 6) exit(1);

  cha_ID      = atoi(argv[1]);
  cmd         = atoi(argv[2]);
  sig_src     = atoi(argv[3]);
  sig_src_PRN = atoi(argv[4]);
  doppler     = atoi(argv[5]);

  if (argc > 6) USCM_ID = strtoul(argv[6], NULL, 16);
  if (argc > 7) JMDC_ID = atoi(argv[7]);
  if (argc > 8) port    = atoi(argv[8]);
  if (argc > 9) P       = atoi(argv[9]);

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

  printf("sending RPSEUD: cha_ID = %d, cmd = %d, sig_src = %d, ",
                          cha_ID,      cmd,      sig_src);
  printf("sig_src_PRN = %d, doppler = %d\n", 
          sig_src_PRN,      doppler);
          
  create_RPSEUD(cha_ID, cmd, sig_src, sig_src_PRN, doppler, &tc);
  to_GPS(port, &tc, &err);
  if (err) EXIT("write RPSEUD on serial port failed");
  
  return 0;
}

//~============================================================================
