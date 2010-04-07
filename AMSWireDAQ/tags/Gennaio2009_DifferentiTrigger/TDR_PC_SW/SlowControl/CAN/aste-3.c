// file aste-3.c
//
// Program to check ASTE board PPS signal reaction thru GPSE
// (this program uses PPS from GPS-simulator + gps-sim program)
//
// A.Lebedev May-2006...

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x123;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//                 A
int USCM[1] = {0x15A};

int n_rcv;

int8 base = 0x00;

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int8 d_rcv[8192];
  int16 dat;
  int uscm, port;
  int32 cntr;
  int16 err;

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [USCM_A [JMDC [P]]]]\n",argv[0]);

  if (argc > 1) USCM[0]  = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID  = strtoul(argv[2], NULL, 16);
  if (argc > 3) P        = atoi(argv[3]);

  printf("USCM_A  = %03X\n", USCM[0]);
  printf("JMDC_ID = %d\n",   JMDC_ID);
  printf("P       = %d\n",   P);
  
  if (!initialize_EPP()) exit(1);

  for (uscm=0; uscm<1; uscm++) {
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
    if (err) {
      printf("boot_from_FLASH of USCM %c (%03X) failed (err=%04X)\n",
              uscm ? 'B' : 'A', USCM_ID, err);
      exit(1);
    }
    for (port=0; port<1; port++) {
      setup_serial_port(port, 0x07, 3, &err);
      if (err) {
        printf("setup_serial_port %d of USCM %c (%03X) failed (err=%04X)\n",
                port, uscm ? 'B' : 'A', USCM_ID, err);
        exit(1);
      }
    }
  }

  uscm = 0;
  USCM_ID = USCM[uscm];

  dat = 0x0000;
  write_LVDS_bus(base+0, dat, &err);
  if (err) {
    printf("Set AST_SERIAL_PORT via USCM %c (%03X) failed (err=%04X)\n",
            uscm ? 'B' : 'A', USCM_ID, err);
    exit(1);
  }

  dat = OPEN_EPOCH_GATE;
  write_LVDS_bus(base+3, dat, &err);
  if (err) {
    printf("OPEN_EPOCH_GATE via USCM %c (%03X) failed (err=%04X)\n",
            uscm ? 'B' : 'A', USCM_ID, err);
    exit(1);
  }

  dat = USCM_A_PORT_0_TO_AST;
  write_LVDS_bus(base+1, dat, &err);
  if (err) {
    printf("Set USCM output port via USCM %c (%03X) failed (err=%04X)\n",
            uscm ? 'B' : 'A', USCM_ID, err);
    exit(1);
  }

  dat = AST_TO_USCM_A_PORT_0;
  write_LVDS_bus(base+2, dat, &err);
  if (err) {
    printf("Set USCM input port via USCM %c (%03X) failed (err=%04X)\n",
            uscm ? 'B' : 'A', USCM_ID, err);
    exit(1);
  }

  while (1) {
    Write_serial_port(0, 1, (int8*)"a", &err);
    if (err) {
      printf("Write_serial_port failed (err=%04X)\n", err);
      exit(1);
    }

    Read_serial_port(0, 8192, &n_rcv, d_rcv, &err);
    if (err) {
      printf("Read_serial_port failed (err=%04X)\n", err);
      exit(1);
    }
    printf("n_rcv = %d, d_rcv = ", n_rcv);
    for (i=0; i<n_rcv; i++) printf("%c", d_rcv[i]);
    d_rcv[n_rcv] ='\0';
    cntr = strtoul((char*)&d_rcv[1], NULL, 16);
    printf(", cntr = %5d\n", cntr);
  }
  return 0;
}

//~============================================================================
