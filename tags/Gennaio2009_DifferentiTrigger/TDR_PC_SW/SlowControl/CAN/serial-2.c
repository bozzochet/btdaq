// file serial-2.c
//
// Program to study serial communications - A.Lebedev Sep-2005...
// Works with 2 Serial Ports of the same USCM or of 2 USCMs
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x123;    // 1xx stands for Newborn ID
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int  USCM_1 = 0x161;
int8 port_1 = 1;
int  USCM_2 = 0x190;
int8 port_2 = 1;

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, n_snd, n_rcv, z;
  int8 d_snd[1024];
  int8 d_rcv[1024];
  int16 err;
  int8  mode, rate;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM_1 ID> [<port_1> [<USCM_2 ID> [<port_2> [P]]]]]\n",
          argv[0]);

  if (argc > 1) USCM_1 = strtoul(argv[1], NULL, 16);
  if (argc > 2) port_1 = atoi(argv[2]);
  if (argc > 3) USCM_2 = strtoul(argv[3], NULL, 16);
  if (argc > 4) port_2 = atoi(argv[4]);
  if (argc > 5) P = atoi(argv[5]);

  printf("USCM_1_ID = 0x%02X, port_1 = %d\n", USCM_1, port_1);
  printf("USCM_2_ID = 0x%02X, port_2 = %d\n", USCM_2, port_2);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  if (!initialize_EPP()) exit(1);

  USCM_ID = USCM_1;
  setup_serial_port(port_1, 0x07, 3, &err);
  if (err) EXIT("setup serial port_1 failed");
  check_serial_port(port_1, &mode, &rate, &err);
  if (err) EXIT("check serial port_1 failed");

  USCM_ID = USCM_2;
  setup_serial_port(port_2, 0x07, 3, &err);
  if (err) EXIT("setup serial port_2 failed");
  check_serial_port(port_2, &mode, &rate, &err);
  if (err) EXIT("check serial port_2 failed");

//~--

  n_snd = 512 + 0;
  for (i=0; i<n_snd; i++) d_snd[i] = i;

  printf("n_snd = %d\n", n_snd);

  USCM_ID = USCM_2;
  flush_serial_port(port_2, &err);
  if (err) EXIT("flush serial port_2 failed");

  USCM_ID = USCM_1;
  Write_serial_port(port_1, n_snd, d_snd, &err);
  if (err) EXIT("write serial port_1 failed");
    
  USCM_ID = USCM_2;
  z = Read_serial_port(port_2, 1024, &n_rcv, d_rcv, &err);
  if (err) EXIT("read serial port_2 failed");

  printf("n_rcv = %d, z = %d\n", n_rcv, z);
  
  for (i=0; i<n_rcv; i++) printf("0x%02X ", d_rcv[i]);
  printf("\n");

  for (i=0, err=0; i<MIN(n_snd, n_rcv); i++) if (d_snd[i] != d_rcv[i]) err++;
  printf("%d error(s)\n", err);
  
  return 0;
}

//~============================================================================
