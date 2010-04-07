// file gpse-1.c
//
// Program to check GPSE board serial commutator
// (with gps-sim-loopback program running on the other PC)
// (or with loopback connector)
//
// A.Lebedev Apr-2007...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x123;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//                 A      B
int USCM[2] = {0x198, 0x161};

int8 base = 0x00;

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, j;
  int8 d_snd[8192];
  int8 d_rcv[8192];
  int n_snd, n_rcv, n_try, n_tst;
  int16 dat;
  int tst;
  int uscm, ouscm, iuscm;
  int port, oport, iport;
  int16 err;
  bool error = FALSE;
  int test = 0;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [USCM_A [USCM_B [JMDC [P]]]]\n",argv[0]);

  if (argc > 1) USCM[0]  = strtoul(argv[1], NULL, 16);
  if (argc > 2) USCM[1]  = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID  = strtoul(argv[3], NULL, 16);
  if (argc > 4) P        = atoi(argv[4]);

  printf("USCM_A  = %03X\n", USCM[0]);
  printf("USCM_B  = %03X\n", USCM[1]);
  printf("JMDC_ID = %d\n",   JMDC_ID);
  printf("P       = %d\n",   P);
  
  n_tst = 10;
  n_try = 100;

  if (!initialize_EPP()) exit(1);
  
  n_snd = 100;
  for (i=0; i<n_snd; i++) d_snd[i] = i;

  printf("n_tst = %d\n", n_tst);
  printf("n_snd = %d\n", n_snd);
  printf("n_try = %d\n", n_try);

  for (uscm=0; uscm<2; uscm++) {
    USCM_ID = USCM[uscm];
    boot_from_FLASH(&err);
    if (err) {
      for (i=0; i<3; i++) {
        ping_USCM(0, NULL, NULL, NULL, &err);
        if (!err) {
          break;
        }
      }
    }
    if (err) PANIC("boot_from_FLASH failed\n");
    for (port=0; port<2; port++) {
      setup_serial_port(port, 0x07, 3, &err);
      if (err) PANIC("setup_serial_port failed\n");
    }
  }

  USCM_ID = USCM[0];
  write_LVDS_bus(base+0, 0, &err);
  if (err) PANIC("Reset AUTO OPEN/CLOSE failed\n");
  write_LVDS_bus(base+3, CLOSE_EPOCH_GATE | OPEN_COM_GATE, &err);
  if (err) PANIC("Open COM Gate failed\n");

for (tst=0; tst<n_tst; tst++) {

  printf("--- TX ---   --- RX ---            \n");
  printf("USCM  port   USCM  port   Comments:\n");
  for (ouscm=0; ouscm<2; ouscm++) {
    for (oport=0; oport<2; oport++) {
      for (iuscm=0; iuscm<2; iuscm++) {
        for (iport=0; iport<2; iport++) {
          test = test + 1;
          printf("   %c     %d      %c     %d  %3d: ",
                  ouscm ? 'B' : 'A', oport, iuscm ? 'B' : 'A', iport, test);
          uscm = 0;
          USCM_ID = USCM[uscm];
          if (ouscm) {
            if (oport) dat = USCM_B_PORT_1_TO_GPS;
            else       dat = USCM_B_PORT_0_TO_GPS;
          }
          else {
            if (oport) dat = USCM_A_PORT_1_TO_GPS;
            else       dat = USCM_A_PORT_0_TO_GPS;
          }
          write_LVDS_bus(base+1, dat, &err);
          if (err) PANIC("Set USCM output port failed\n");
          if (iuscm) {
            if (iport) dat = GPS_TO_USCM_B_PORT_1;
            else       dat = GPS_TO_USCM_B_PORT_0;
          }
          else {
            if (iport) dat = GPS_TO_USCM_A_PORT_1;
            else       dat = GPS_TO_USCM_A_PORT_0;
          }
          write_LVDS_bus(base+2, dat, &err);
          if (err) PANIC("Set USCM input port failed\n");
          
          for (j=0; j<n_try; j++) {
            error = FALSE;
            USCM_ID = USCM[iuscm];
            USCM_ID = USCM[ouscm];
            write_serial_port(oport, n_snd, d_snd, &err);
            if (err) PANIC("Write_serial_port failed\n");
            USCM_ID = USCM[iuscm];
            Read_serial_port(iport, 8192, &n_rcv, d_rcv, &err);
            if (err) PANIC("Read_serial_port failed\n");
            if (n_rcv != n_snd) {
              printf("Wrong length at try=%d: n_rcv = %d, must be %d. ", j, n_rcv, n_snd);
              Read_serial_port(iport, 8192, &n_rcv, d_rcv, &err);
              if (err) PANIC("Read_serial_port failed\n");
              if (n_rcv) printf("Later %d bytes more. ", n_rcv);
              printf("\n");
              error = TRUE;
            }
            else {
              int i;
              for (i=0; i<n_rcv; i++) {
                if (d_snd[i] != d_rcv[i]) {
                  printf("%d-th byte: sent 0x%02hX, received 0x%02hX.\n",
                          i, d_snd[i], d_rcv[i]);
                  error = TRUE;
                  break;
                }
              }
            }
            if (error) break;
          }
          if (!error) printf("Test OK\n");
        }
      }
    }
  }
}

  return 0;
}

//~============================================================================
