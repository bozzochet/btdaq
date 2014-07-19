// file tvt.c
//
// Program to check GPSE board serial commutator during TVT test of FM M-Crate
//
// It checks communication with:
//  - GPS (loopback jumper should be installed)
//  - ASTE board (not much can be done here)
//
// A.Lebedev Aug-2007...

#include "uscmlib.h"

int JMDC_ID = 0x00;
int USCM_ID = 0x123;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//                A     B
int USCM[2] = {0x2C, 0x2D};

int8  base = 0x00;
int16 err;

//~----------------------------------------------------------------------------

void print(char *msg) {

  fprintf(stdout, "\r%s%s", timestamp(1), msg);
}

//~----------------------------------------------------------------------------

bool setup_USCMs(void) {

  int i;
  int uscm, port;
  int8 sta_dat[1000], sta_mode;

  for (uscm=0; uscm<2; uscm++) {
    USCM_ID = USCM[uscm];
    get_short_status(sta_dat, &err);
    if (err) {
      print("get_short_status failed\n");
      return FALSE;
    }
    sta_mode = sta_dat[0];
    if (sta_mode < 2 || sta_mode > 13) {
      print("Unknown mode\n");
      return FALSE;
    }
    if (!(sta_mode & 0x01)) {
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
        print("boot_from_FLASH failed\n");
        return FALSE;
      }
      else {
        print("boot_from_FLASH O.K.\n");
      }
    }
    for (port=0; port<2; port++) {
      setup_serial_port(port, 0x07, 3, &err);
      if (err) {
        print("setup_serial_port failed\n");
        return FALSE;
      }
    }
  }

  USCM_ID = USCM[0];
  write_LVDS_bus(base+0, 0, &err);
  if (err) {
    print("Reset AUTO OPEN/CLOSE failed\n");
    return FALSE;
  }
  write_LVDS_bus(base+3, CLOSE_EPOCH_GATE | OPEN_COM_GATE, &err);
  if (err) {
    print("Open COM Gate failed\n");
    return FALSE;
  }
  
  return TRUE;
}

//~----------------------------------------------------------------------------

bool test_GPS(int n_snd, int n_try) {

  int i;
  int8 d_snd[8192];
  int8 d_rcv[8192];
  bool error = FALSE;
  static int test = 0;
  int16 dat;
  int ouscm, iuscm;
  int oport, iport;
  int n_rcv;

  for (i=0; i<sizeof(d_snd); i++) d_snd[i] = i;

  printf("--- TX ---   --- RX ---              \n");
  printf("USCM  port   USCM  port     Comments:\n");

  for (ouscm=0; ouscm<2; ouscm++) {
    for (oport=0; oport<2; oport++) {
      for (iuscm=0; iuscm<2; iuscm++) {
        for (iport=0; iport<2; iport++) {
          test = test + 1;
          printf("   %c     %d      %c     %d  %5d: ",
                  ouscm ? 'B' : 'A', oport, 
                  iuscm ? 'B' : 'A', iport, test);
          USCM_ID = USCM[0];
          if (ouscm) {
            if (oport) dat = USCM_B_PORT_1_TO_GPS;
            else       dat = USCM_B_PORT_0_TO_GPS;
          }
          else {
            if (oport) dat = USCM_A_PORT_1_TO_GPS;
            else       dat = USCM_A_PORT_0_TO_GPS;
          }
          write_LVDS_bus(base+1, dat, &err);
          if (err) {
            print("Set USCM output port failed\n");
            return FALSE;
          }
          if (iuscm) {
            if (iport) dat = GPS_TO_USCM_B_PORT_1;
            else       dat = GPS_TO_USCM_B_PORT_0;
          }
          else {
            if (iport) dat = GPS_TO_USCM_A_PORT_1;
            else       dat = GPS_TO_USCM_A_PORT_0;
          }
          write_LVDS_bus(base+2, dat, &err);
          if (err) {
            print("Set USCM input port failed\n");
            return FALSE;
          }
          for (i=0; i<n_try; i++) {
            error = FALSE;
            USCM_ID = USCM[iuscm];
            USCM_ID = USCM[ouscm];
            write_serial_port(oport, n_snd, d_snd, &err);
            if (err) {
              print("Write_serial_port failed\n");
              return FALSE;
            }
            USCM_ID = USCM[iuscm];
            Read_serial_port(iport, 8192, &n_rcv, d_rcv, &err);
            if (err) {
              print("Read_serial_port failed\n");
              return FALSE;
            }
            if (n_rcv != n_snd) {
              printf("Wrong length at try=%d: n_rcv = %d, must be %d. ",
                      i, n_rcv, n_snd);
              Read_serial_port(iport, 8192, &n_rcv, d_rcv, &err);
              if (err) {
                print("Read_serial_port failed\n");
                return FALSE;
              }
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

  return TRUE;
}

//~----------------------------------------------------------------------------

bool test_ASTE(int n_try) {

  int i;
  int n_snd;
  int8 d_snd[8192];
  int8 d_rcv[8192];
  static int test = 0;
  int16 dat;
  int ouscm, iuscm;
  int oport, iport;
  int n_rcv;

  n_snd = 1;
  d_snd[0] = 'o';  // read houskeeping from inactive image memory

  printf("--- TX ---   --- RX ---              \n");
  printf("USCM  port   USCM  port     Comments:\n");

  for (ouscm=0; ouscm<2; ouscm++) {
    for (oport=0; oport<2; oport++) {
      for (iuscm=0; iuscm<2; iuscm++) {
        for (iport=0; iport<2; iport++) {
          test = test + 1;
          printf("   %c     %d      %c     %d  %5d: ",
                  ouscm ? 'B' : 'A', oport, 
                  iuscm ? 'B' : 'A', iport, test);
          USCM_ID = USCM[0];
          if (ouscm) {
            if (oport) dat = USCM_B_PORT_1_TO_AST;
            else       dat = USCM_B_PORT_0_TO_AST;
          }
          else {
            if (oport) dat = USCM_A_PORT_1_TO_AST;
            else       dat = USCM_A_PORT_0_TO_AST;
          }
          write_LVDS_bus(base+1, dat, &err);
          if (err) {
            print("Set USCM output port failed\n");
            return FALSE;
          }
          if (iuscm) {
            if (iport) dat = AST_TO_USCM_B_PORT_1;
            else       dat = AST_TO_USCM_B_PORT_0;
          }
          else {
            if (iport) dat = AST_TO_USCM_A_PORT_1;
            else       dat = AST_TO_USCM_A_PORT_0;
          }
          write_LVDS_bus(base+2, dat, &err);
          if (err) {
            print("Set USCM input port failed\n");
            return FALSE;
          }
          for (i=0; i<n_try; i++) {
            USCM_ID = USCM[iuscm];
            USCM_ID = USCM[ouscm];
            write_serial_port(oport, n_snd, d_snd, &err);
            if (err) {
              print("Write_serial_port failed\n");
              return FALSE;
            }
            USCM_ID = USCM[iuscm];
            Read_serial_port(iport, 8192, &n_rcv, d_rcv, &err);
            if (err) {
              print("Read_serial_port failed\n");
              return FALSE;
            }
            printf("sent 'o', reply = ");
            for (i=0; i<n_rcv; i++) printf("%02X ", d_rcv[i]);
            printf("%s", n_rcv ? "\n" : " no reply\n");
            Read_serial_port(iport, 8192, &n_rcv, d_rcv, &err);
            if (n_rcv) printf("more...\n");
          }
        }
      }
    }
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

bool DTS_readout(void) {

  int i, j, nmax;
  int uscm;
  FILE *file;

  int8 n1[9], n3[9];
  bool       ok3[9][64];
  float       t3[9][64];
  int16     age3[9][64];
  int64 ID[9][64];

//~--

  for (uscm=0; uscm<2; uscm++) {
    USCM_ID = USCM[uscm];
    printf("USCM_ID = %02X\n", USCM_ID);
    
    for (i=0; i<9; i++) n1[i] = n3[i] = 0;

    for (i=0; i<9; i++) {
      read_DS1820_table(i+1, &n1[i], &ID[i][0], NULL, &err);
      if (err) break;
    }
    if (err) {
      print("read_DS1820_table FAILED\n");
      return FALSE;
    }
  
    printf("Bus #                  ");
    for (i=0; i<9; i++) printf(" %2d",    9-1-i+1);
    printf("\n");
    printf("Nb of sensors to read: ");
    for (i=0; i<9; i++) printf(" %2d", n1[9-1-i]);
    printf("\n");

//~--

    for (i=0; i<9; i++) {
      if (n1[i]) {
        read_DS1820_temp_long(i+1, &n3[i], &ok3[i][0], &t3[i][0], &age3[i][0], &err);
        if (err) break;
      }
    }
    if (err) {
      print("read_DS1820_temp_long FAILED\n");
      return FALSE;
    }

    printf("Nb of sensors to read: ");
    for (i=0; i<9; i++) printf(" %2d", n3[9-1-i]);
    printf("\n");

//~--

    file = stdout;

    nmax = 0;
    for (j=1; j<9; j++) nmax = MAX(nmax, n3[9-1-j]);

    if (nmax) {
      fprintf(file, "Nb");
      for (j=1; j<9; j++) {
        int jj = 8 - j;
        if (n3[jj]) fprintf(file, "  Bus %d  ", jj+1);
      }
      fprintf(file, "\n");
    }

    for (i=0; i<nmax; i++) {
      fprintf(file, "%2d", i);
      for (j=1; j<9; j++) {
        int jj = 8 - j;
        if (n3[jj]) {
          if (i < n1[jj]) {
            if (ok3[jj][i]) fprintf(file, " %+6.2f%s ", t3[jj][i], age3[jj][i]>600 ? "*" : " ");
            else            fprintf(file, "         ");
          }
          else {
//          fprintf(file, "                 ");
            fprintf(file, "         ");
          }
        }
      }
      fprintf(file, "\n");
    }
  }
  
  return TRUE;
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int n_snd, n_try;

  setbuf(stdout, NULL);
  setup_command_path();

  fprintf(stderr, "Usage: %s [USCM_A [USCM_B [JMDC [P]]]]\n",argv[0]);

  if (argc > 1) USCM[0]  = strtoul(argv[1], NULL, 16);
  if (argc > 2) USCM[1]  = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID  = strtoul(argv[3], NULL, 16);
  if (argc > 4) P        = atoi(argv[4]);

  fprintf(stderr, "USCM_A  = %03X\n", USCM[0]);
  fprintf(stderr, "USCM_B  = %03X\n", USCM[1]);
  fprintf(stderr, "JMDC_ID = %d\n",   JMDC_ID);
  fprintf(stderr, "P       = %d\n",   P);
  
  while (1) {
    if (setup_USCMs()) {
      print("USCM setup O.K.\n");
      n_snd = 100;
      n_try = 10;
      print("test_GPS started\n");
      printf("n_snd = %d, n_try = %d\n", n_snd, n_try);
      if (test_GPS(n_snd, n_try)) {
        print("test_GPS completed\n");
      }
      else {
        print("test_GPS failed\n");
      }
    }
    else {
      print("USCM setup failed\n");
    }
      
    if (setup_USCMs()) {
      print("USCM setup O.K.\n");
      n_snd = 100;
      n_try = 1;
      print("test_GPS started\n");
      printf("n_snd = %d, n_try = %d\n", n_snd, n_try);
      if (test_GPS(n_snd, n_try)) {
        print("test_GPS completed\n");
      }
      else {
        print("test_GPS failed\n");
      }
    }
    else {
      print("USCM setup failed\n");
    }
      
#if 0
    if (setup_USCMs()) {
      print("USCM setup O.K.\n");
      n_try = 1;
      print("test_ASTE started\n");
      printf("n_try = %d\n", n_try);
      if (test_ASTE(n_try)) {
        print("test_ASTE completed\n");
      }
      else {
        print("test_ASTE failed\n");
      }
    }
    else {
      print("USCM setup failed\n");
    }
#endif

    if (setup_USCMs()) {
      print("USCM setup O.K.\n");
      print("DTS_readout started\n");
      if (DTS_readout()) {
        print("DTS_readout completed\n");
      }
      else {
        print("DTS_readout failed\n");
      }
    }
    else {
      print("USCM setup failed\n");
    }

    sleep(5*60);
  }

  return 0;
}

//~============================================================================
