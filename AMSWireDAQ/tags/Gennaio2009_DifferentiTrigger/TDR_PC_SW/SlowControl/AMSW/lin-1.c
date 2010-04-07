// file lin-1.c
//
// Program - workbench for JLV1 studies
//
// A.Lebedev Feb-2006...

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable

//~----------------------------------------------------------------------------

// use this to test (inhibit trigger):
//      ./3 0 2e0f 8009 0000 060c 0030 060d 0040 060c 0030 040d 0000 0413

  int16 request[100];
  int16 reply[100];
  
//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int16 err;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s [<TX&RX> [<Path>|0 [<Debug Printout Level>]]]\n", argv[0]);
  if (argc > 1) TX           = RX = atoi(argv[1]);
  if (argc > 2) AMSW_PATH[0] = strtoul(argv[2], NULL, 16);
  if (argc > 3) P            = atoi(argv[3]);
  printf("TX    = %d\n", TX);
  printf("RX    = %d\n", RX);
  printf("Path  = 0x%04X\n", AMSW_PATH[0]);
  printf("P     = %d\n", P);

  begin_IO_segment(request);
  add_IO_write(request, 0x0030, 0x0040);
  add_IO_read(request, 0x30);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, &err);

  if (err) {
    printf("*** Error 0x%04X\n", err);
    exit(1);
  }

  return 0;
}

//~============================================================================
