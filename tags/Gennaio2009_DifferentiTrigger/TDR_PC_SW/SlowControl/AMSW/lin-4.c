// file lin-4.c
//
// Program - workbench for JLV1 studies:
// - reads and prints in nice format some registers
//
// A.Lebedev Sep-2006...

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  int16 request[10000];
  int16 reply[10000];
  int16 sub_LV1[8], pre_sca[8], or_msk;
  
//~============================================================================

int main(int argc, char *argv[]) {

  int i, n_r;
  int16 err;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<P>]]\n", argv[0]);
  if (argc > 1) TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }
  if (argc > 2) P = atoi(argv[2]);
  printf("TX&RX = %d\n", TX);
  printf("P     = %d\n", P);

  for (i=0; i<8; i++) {
    begin_IO_segment(request);
    add_IO_read(request, 0x1F+i);
    add_IO_read(request, 0x27+i);
    end_IO_segment(request);
    perform_IO_operation(request, &n_r, reply, &err);
    if (err) exit(1);
    sub_LV1[i] = reply[3];
    pre_sca[i] = reply[7];
  }

  begin_IO_segment(request);
  add_IO_read(request, 0x1E);
  end_IO_segment(request);
  perform_IO_operation(request, &n_r, reply, &err);
  if (err) exit(1);
  or_msk = reply[3];

  for (i=0; i<8; i++) {
    printf("%d:  %04X  %04X  %s\n", i, sub_LV1[i], pre_sca[i], 
           (or_msk & 1 << i) ? "OFF" : "ON");
  }

  return 0;
}

//~============================================================================
