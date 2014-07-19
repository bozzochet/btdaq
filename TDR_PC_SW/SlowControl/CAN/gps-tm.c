// file gps-tm.c
//
// Program to read both serial ports or a file and print GPS telemetry
// Note: this program does not setup the serial ports!!!
//
// A.Lebedev Mar-2006...
//

#include "gpsdef.h"

int JMDC_ID = 0x02;
int USCM_ID = 0x2C;
int P  = 0;
char *PP = {""};

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

bool read_file = FALSE;
FILE *file = NULL;
char *filename;

int port;
int8 mode, rate;
char *rates[4] = {"2400", "4800", "9600", "19200"};

float period = 1.0;

bool opt_H = FALSE, opt_S = FALSE, opt_C = FALSE;

//~----------------------------------------------------------------------------

void print_opt(void) {

  fprintf(stderr, "Hex     = %s \n", opt_H ? "TRUE" : "FALSE");
  fprintf(stderr, "Special = %s \n", opt_S ? "TRUE" : "FALSE");
  fprintf(stderr, "Common  = %s \n", opt_C ? "TRUE" : "FALSE");
}

//~----------------------------------------------------------------------------

void user_interface(void) {

  if (kbhit()) {
    int ch = toupper(fgetc(stdin));
    switch (ch) {
      case '?':                               print_opt(); break;
      case 'H': opt_H = opt_H ? FALSE : TRUE; print_opt(); break;
      case 'S': opt_S = opt_S ? FALSE : TRUE; print_opt(); break;
      case 'C': opt_C = opt_C ? FALSE : TRUE; print_opt(); break;
      case 'Q': exit(0);
    }
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i, k;
  char *filter = {"NO"};
  char *q[1];
  
  setbuf(stdout, NULL);
  setup_command_path();

  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s [<USCM_ID> [<JMDC_ID> [P [{H|S|C}]]]]\n",argv[0]);
  fprintf(stderr, "or\n");
  fprintf(stderr, "%s <file-name> [{<filter>|NO} [P [{H|S|C}]]]\n",argv[0]);
  fprintf(stderr, "H - raw Hexadecimal data printout\n");
  fprintf(stderr, "S - Special functions printout\n");
  fprintf(stderr, "C - Common function printout\n");
  
  if (argc > 1) {
    USCM_ID = strtoul(argv[1], q, 16);
    if (!**q) {
      read_file = FALSE;
      k = 2;
      if (argc > k) JMDC_ID = atoi(argv[k++]);
      if (argc > k) P       = atoi(argv[k++]);
      if (argc > k) PP      = argv[k++];
     }
    else {
      read_file = TRUE;
      filename = argv[1];
      file = fopen(filename, "r");
      if (!file) {
        fprintf(stderr, "File %s not found.\n", filename);
        exit(1);
      }
      k = 2;
      if (argc > k) filter = argv[k++];
      if (argc > k) P      = atoi(argv[k++]);
      if (argc > k) PP      = argv[k++];
     }
  }
  
  if (read_file) {
    fprintf(stderr, "file   = %s\n", filename);
    fprintf(stderr, "filter = %s\n", filter);
  }
  else {
    fprintf(stderr, "USCM_ID = %02X\n", USCM_ID);
    fprintf(stderr, "JMDC_ID = %d\n",   JMDC_ID);
  }
  fprintf(stderr, "P      = %d\n", P);
  fprintf(stderr, "PP     = %s\n", PP);
  if (strcasestr(PP, "H")) opt_H = TRUE;
  if (strcasestr(PP, "S")) opt_S = TRUE;
  if (strcasestr(PP, "C")) opt_C = TRUE;
  print_opt();

  if (!read_file) {
    if (!initialize_EPP()) exit(1);
    for (port=0; port<2; port++) {
      int16 err;
      check_serial_port(port, &mode, &rate, &err);
      if (err) EXIT("check serial port failed");
      printf("serial port %d setup: mode = 0x%02X, rate = %s\n", 
                        port,       mode,        rates[rate]);
    }
  }

//~---

  timer(2, START);

  while (1) {
    user_interface();
    for (port=0; port<2; port++) {
      int n_rcv;
      int8 d_rcv[8192];
      int16 err;
      if (read_file) {
        char b[8192];
        int j;
        if (!fgets(b, sizeof(b), file)) exit(0);
        n_rcv = 0;
        sscanf(b, "Length = %d", &n_rcv);
        for (j=0; j<n_rcv; j++) {
          fscanf(file, "%02hhX", &d_rcv[j]);
        }
      }
      else {
#if 0
        while (!(timer(2, EXPIRED, period)));
        read_serial_port(port, 8192, &n_rcv, d_rcv, &err);
        if (err) EXIT("read serial port failed");
        gettimeofday(&now, NULL);
        printf("%d dt = %.1f sec\n", c++, delta_t(&now, &then));
        gettimeofday(&then, NULL);
        printf("Length = %d bytes\n", n_rcv);
        timer(2, START);
#else
        Read_serial_port(port, 8192, &n_rcv, d_rcv, &err);
        if (err) EXIT("read serial port failed");
#endif
      }
      if (n_rcv) {
        int8 *p = d_rcv - 1;
        int32 sync, ID, CKS, myCKS, term[2];
        if (opt_H) {
          printf("Length = %d bytes\n", n_rcv);
          for (i=0; i<MIN(16000, n_rcv); i++) printf("%02X ", d_rcv[i]);
          printf("\n");
        }
        while (1) {
          bool found;
          p = p + 1;
          if ((p - d_rcv) >= n_rcv) break;
          packbytes(&sync, p, 2);
          if (sync == 0x9C3E) {
            int8 *pp = p + 2;
            if (P == -2) printf("sync found at %d\n", p-d_rcv);
            if ((pp - d_rcv) > n_rcv) break;
            packbytes(&ID, pp, 2);
            if (P == -2) printf("ID = %04X", ID);
            found = FALSE;
            for (i=0; i<32; i++) {
              if (ID == TM[i].ID.RS422) {
                found = TRUE;
                break;
              }
            }
            if (P == -2) {
              if (found) printf(",  l = %d\n", TM[i].len);
              else       printf("\n");
              printf("found = %d,   i = %2d,   l = %d\n", found, i, TM[i].len);
            }
            if (!found) continue;
            pp = pp + 2;
            if ((pp + 4 + TM[i].len + 8 - d_rcv) > n_rcv) break;
            if (P == -2) printf("survived length\n");
            packbytes(&term[0], pp+TM[i].len+4, 4);
            packbytes(&term[1], pp+TM[i].len+8, 4);
            if (P == -2) printf("trm1 = %08X, trm2 = %08X\n", term[0], term[1]);
            if (term[0] != 0x55555555 || term[1] != 0x55555555) continue;
            if (P == -2) printf("survived terminators\n");
            packbytes(&CKS, pp, 4);
            myCKS = checksum(16+TM[i].len, pp-4);
            if (P == -2) printf("CKS = %08X, myCKS = %08X\n", CKS, myCKS);
            if (CKS != myCKS) continue;
            if (P == -2) printf("survived checksum\n");
            pp = pp + 4;
            TM[i].msg = pp;
            if (!filter || !strcasecmp(filter, "NO")) {
              if (opt_S) TM[i].call(&TM[i]);
              if (opt_C) process_ANYTM(&TM[i]);
            }
            if (filter && !strcasecmp(filter, TM[i].nam)) {
              if (opt_S) TM[i].call(&TM[i]);
              if (opt_C) process_ANYTM(&TM[i]);
            }
            p = pp + TM[i].len + 8 - 1;
          }
        }
      }
    }
  }

//~--

  return 0;
}

//~============================================================================
