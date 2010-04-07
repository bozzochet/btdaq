// file basili-5a.c
//
// Program - workbench for JTAG studies:
//
// A.Lebedev Feb-2007...

#include "daqlib.h"
#include "sdr2def.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i, n;
  FILE *file;
  char *filename;
  int16 wri[10240], rea[10240];
  char note[10240][80];
  int16 err;
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);	               // set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: %s <TX&RX> <filename> [<Debug Printout Level>]\n", argv[0]);
  if (argc < 3) {
    printf("Too few arguments.\n");
    exit(1);
  }
  TX = RX = atoi(argv[1]);
  if (TX < 0 || TX > 3) {
    printf("Wrong TX&RX port %d, must be 0..3.\n", TX);
    exit(1);
  }
  filename = argv[2];
  if (argc > 3) P  = atoi(argv[3]);
  
  printf("TX&RX    = %d\n", TX);
  printf("filename = %s\n", filename);
  printf("P        = %d\n", P);
  printf("\n");

  file = fopen(filename, "r");
  if (!file) {
    printf("*** File %s was not found.\n", filename);
    exit(1);
  }

  n = 0;
  while (1) {
    char b[8192];
    int k;
    int data_in, reset, frame, clock;

    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';
//  if (P) printf("%s\n", b);

    k = sscanf(b, "%d %d %d %d %60c", &data_in, &reset, &frame, &clock, &note[n][0]);
//printf("%d %d %d %d %s\n", data_in, reset, frame, clock, note[n]);
    if (k < 4) {
//    printf("%s\n", b);
      continue;
    }

    wri[n]  = 0;
    pack16(&wri[n], 0x0008, (data_in & 1));
    pack16(&wri[n], 0x0004, (reset   & 1));
    pack16(&wri[n], 0x0002, (frame   & 1));
    pack16(&wri[n], 0x0001, (clock   & 1));
//    printf("%04X\n", wri[n]);
    n++;
  }

  exec_SDR2_SC_command(WRITE, SFEA2, 0x2C7, 1, NULL, NULL, &err);
  if (err) PANIC("Failed to enable.\n");
    printf("  #:  TDI TRST  TMS  TCK   TOUT  Notes\n");
    for (i=0; i<n; i++) {
      int data_out, data_in, reset, frame, clock;
      exec_SDR2_SC_command(WRITE, SFEA2, 0x2C8, wri[i], NULL, NULL, &err);
      if (err) PANIC("Failed to write.\n");
      exec_SDR2_SC_command(READ,  SFEA2, 0x2C8, 0, &rea[i], NULL, &err);
      if (err) PANIC("Failed to read.\n");
      data_out = unpack16(rea[i], 0x0010);
      data_in  = unpack16(rea[i], 0x0008);
      reset    = unpack16(rea[i], 0x0004);
      frame    = unpack16(rea[i], 0x0002);
      clock    = unpack16(rea[i], 0x0001);
      printf("%3d:    %d    %d    %d    %d ->   %d  %s\n", 
              i, data_in, reset, frame, clock, data_out, note[i]);
    }

  return 0;
}

//~============================================================================
