// file serial-3.c
//
// Program to study serial communications - A.Lebedev Sep-2005...
// Has two modes:
// - continuosly reads the serial input (USCM #0 is assumed)
// - occasionally writes the serial output (USCM #1 is assumed)
// So, one should run 2 copies of this program on 2 computers
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x123;    // 1xx stands for Newborn ID
int P = 1;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int R_W = -1;
int port = 1;

//                  R      W
int  JMDC[2] = {  0x1,   0x2};
int  USCM[2] = {0x161, 0x190};

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, n_snd, n_rcv, z;
  int8 d_snd[8192];
  int8 d_rcv[8192];
  int16 err;
  int8  mode, rate;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s R|W [<USCM_ID> [<port> [P]]]\n",argv[0]);

  if (argc < 2) exit(1);
  if      (toupper((int)*argv[1]) == 'R') R_W = 0;
  else if (toupper((int)*argv[1]) == 'W') R_W = 1;
  else exit(1);
  
  JMDC_ID = JMDC[R_W];
  USCM_ID = USCM[R_W];

  if (argc > 2) USCM_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) port    = atoi(argv[3]);
  if (argc > 4) P       = atoi(argv[4]);

  printf("%s: USCM_ID = 0x%02X, port = %d, JMDC_ID = %d, P = %d\n",
          R_W ? "Writing" : "Reading", USCM_ID, port, JMDC_ID, P);

  if (!initialize_EPP()) exit(1);

  setup_serial_port(port, 0x07, 2, &err);
  if (err) EXIT("setup serial port failed");

  check_serial_port(port, &mode, &rate, &err);
  if (err) EXIT("check serial port failed");
  printf("serial port setup: mode = 0x%02X, rate = %d\n", mode, rate);

  switch (R_W) {

//~--

    case 0:       // Reading
      for (port=0; port<2; port++) {
        setup_serial_port(port, 0x07, 2, &err);
        if (err) EXIT("setup serial port failed");
        check_serial_port(port, &mode, &rate, &err);
        if (err) EXIT("check serial port failed");
        printf("serial port %d setup: mode = 0x%02X, rate = %d\n", port, mode, rate);
        flush_serial_port(port, &err);
        if (err) EXIT("flush serial port failed");
      }
      while (1) {
        for (port=0; port<2; port++) {
          z = Read_serial_port(port, 8192, &n_rcv, d_rcv, &err);
          if (err) EXIT("read serial port failed");
          if (n_rcv) {
            static struct timeval now, then;
            static int c =0;
            gettimeofday(&now, NULL);
            printf("Port %d (%4d): n_rcv = %4d, z = %3d, dt = %.1f sec: ", 
                    port, c++, n_rcv, z, delta_t(&now, &then));
            printf("\n");
            for (i=0; i<MIN(16000, n_rcv); i++) printf("0x%02X ", d_rcv[i]);
            printf("\n");
            gettimeofday(&then, NULL);
          }
        }
      }
    break;

//~--

    case 1:       // Writing
      n_snd = 512 + 0;
      for (i=0; i<n_snd; i++) d_snd[i] = i;
      printf("n_snd = %d\n", n_snd);
      while (1) {
        if (kbhit()) {
          int ch = toupper(fgetc(stdin));
          switch (ch) {
            case 'Q':
              exit(0);
            break;
            case 'W':
              Write_serial_port(port, n_snd, d_snd, &err);
              if (err) EXIT("write serial port failed");
              printf("Writing %d bytes...\n", n_snd);
            break;
          }
        }
       }
    break;
  }

//~--

  return 0;
}

//~============================================================================
