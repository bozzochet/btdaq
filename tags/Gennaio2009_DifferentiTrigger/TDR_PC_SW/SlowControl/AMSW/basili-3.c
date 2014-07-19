// file basili-3.c
//
// Program - workbench for SDR2 studies:
// - inputs I/O segment body in nice format
// - sends it to SDR2
//
// A.Lebedev Oct-2006...

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~----------------------------------------------------------------------------

// use this to test (set/get power status):
//
//      ./basili-3 0 060e 1 060f 00a0 060e 0 060f 0
//      RQ: 8009 0000 060E 0001 060F 00A0 060E 0000 060F 0000 3F54
//      RP:           060E 0001 060F 00A0 060E 0000 060F 0000 3F54 0160
//
//      ./basili-3 0 WA 1 WD 00a0 WA 0 WD 0
//      RQ: 8009 0000 060E 0001 060F 00A0 060E 0000 060F 0000 3F54
//      RP:           060E 0001 060F 00A0 060E 0000 060F 0000 3F54 0160
//
//      ./basili-3 0 060e 0 040f 0
//      RQ: 8005 0000 060E 0000 040F 0000 65BD
//      RP:           060E 0000 040F 0002 45FF 0160
//
//      ./basili-3 0 WA 0 RD 0
//      RQ: 8005 0000 060E 0000 040F 0000 65BD
//      RP:           060E 0000 040F 0002 45FF 0160

  int16 request[10000];
  int16 reply[10000];
  
//~============================================================================

int main(int argc, char *argv[]) {

  int i, n_w, n_r;
  int16 err;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s <TX&RX> <I/O Segment Body>\n", argv[0]);
  printf("       Use RA==040E, RD==040F WA==060E, WD==060F.\n");
  if (argc < 4) {
    printf("Too few arguments.\n");
    exit(1);
  }
  TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }

  P = 3;
  
  printf("TX&RX = %d\n", TX);
  printf("P     = %d\n", P);

  begin_IO_segment(request);
  n_w = argc - 2;
  request[0] += n_w;
  for (i=0; i<n_w; i++) {
    char *arg = argv[i+2];
    if      (!strcasecmp(arg, "WA")) request[i+2] = 0x060E;
    else if (!strcasecmp(arg, "WD")) request[i+2] = 0x060F;
    else if (!strcasecmp(arg, "RD")) request[i+2] = 0x040F;
    else                             request[i+2] = strtoul(arg, NULL, 16);
  }
  end_IO_segment(request);

  perform_IO_operation(request, &n_r, reply, &err);

  if (err) {
    printf("*** Error 0x%04X\n", err);
    exit(1);
  }

  printf("RQ: ");
  for (i=0; i<n_w+3; i++) printf("%04X ", request[i]);
  printf("\n");

  printf("RP:           ");
  for (i=0; i<n_r+1; i++) printf("%04X ", reply[i]);
  printf("\n");

  return 0;
}

//~============================================================================
