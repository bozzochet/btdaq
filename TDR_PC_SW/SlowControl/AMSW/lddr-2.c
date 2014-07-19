// file lddr-2.c
//
// Program for LDDR and LCTL tests with JLV1
//
// A.Lebedev Feb-2006...

#include "template.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  float period = 1.0;
  bool get_event;
  int n;
  int16 b[16384];
  int16 request[100];
  int16 reply[100];
  int16 err;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<Period> [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }
  if (argc > 2) period = atof(argv[2]);
  if (argc > 3) P      = atoi(argv[3]);
  
  printf("TX&RX  = %d\n", TX);
  printf("Period = %.1fs\n", period);
  printf("P      = %d\n", P);

  begin_IO_segment(request);
  add_IO_write(request, 0x17, 0x8000);  // disable int trigger
  add_IO_write(request, 0x18, 0x0000);  // enable LA trigger output
  add_IO_write(request, 0x30, 0x0000);  // enable trigger
  end_IO_segment(request);
  perform_IO_operation(request, NULL, reply, &err);
  if (err) {
    printf("perform_IO_operation failed (1)\n");
    exit(1);
  }

  begin_IO_segment(request);
  add_IO_write(request, 0x3F, 0x0010);  // generate trigger
  end_IO_segment(request);

  while (1) {
    get_event = get_JINF_event(&n, b, &err);
    if (err) {
      printf("get_JINF_event failed(1)\n");
      exit(1);
    }
    if (!get_event) break;
  }

  while (1) {
    static int n = 0;
    perform_IO_operation(request, NULL, reply, &err);
    if (err) {
      printf("perform_IO_operation failed (2)\n");
      exit(1);
    }
    while (1) {
      n++;
      get_event = get_JINF_event(&n, b, &err);
      if (err) {
        printf("get_JINF_event failed(2)   n=%d\n", n);
        exit(1);
      }
      if (!get_event) break;
      delay(period);
    }
  }

  return 0;
}

//~============================================================================
