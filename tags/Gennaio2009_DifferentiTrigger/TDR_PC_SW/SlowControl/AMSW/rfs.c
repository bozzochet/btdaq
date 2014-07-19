// file rfs.c
//
// Get JINF Flash Status
//
// A.Lebedev Aug-2005...

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

int16 dat[8][4];
int n;
int16 datdat[24][8][4];
int16 sta[24];

//~----------------------------------------------------------------------------

void print_Flash_Summary_1(void) {

  int i, j;
  
  for (j=0; j<n; j++) {
    printf("Sector    File Header  Address  Status  Reserved     sta = 0x%04X\n", sta[j]);
    for (i=0; i<8; i++) {
      printf("   %d", i);
      printf("     0x%04X", datdat[j][i][0]);
      if      (unpack16(datdat[j][i][0], 0xC000) == 0) printf(" = PGM");
      else if (unpack16(datdat[j][i][0], 0xC000) == 1) printf(" = DAT");
      else                                             printf(" = ???");
      if      (unpack16(datdat[j][i][0], 0x3000) == 2) printf("*");
      else if (unpack16(datdat[j][i][0], 0x3000) == 3) printf(" ");
      else                                             printf("-");
      printf("  0x%04X", datdat[j][i][1]);
      printf("  0x%04X", datdat[j][i][2]);
      printf("   0x%04X", datdat[j][i][3]);
      printf("\n");
    }
  }
}

//~----------------------------------------------------------------------------

void print_Flash_Summary_0(void) {

  int i;
  
  printf("Sector    File Header  Address  Status  Reserved\n");
  for (i=0; i<8; i++) {
    printf("   %d", i);
    printf("     0x%04X", dat[i][0]);
    if      (unpack16(dat[i][0], 0xC000) == 0) printf(" = PGM");
    else if (unpack16(dat[i][0], 0xC000) == 1) printf(" = DAT");
    else                                       printf(" = ???");
    if      (unpack16(dat[i][0], 0x3000) == 2) printf("*");
    else if (unpack16(dat[i][0], 0x3000) == 3) printf(" ");
    else                                       printf("-");
    printf("  0x%04X", dat[i][1]);
    printf("  0x%04X", dat[i][2]);
    printf("   0x%04X", dat[i][3]);
    printf("\n");
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int fmt;
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

  get_JINF_FLASH_summary(&fmt, &dat[0][0], &n, &datdat[0][0][0], sta, &err);

  if (err) {
    printf("*** Error 0x%04X\n", err);
    exit(1);
  }

  if (!fmt) print_Flash_Summary_0();
  else      print_Flash_Summary_1();
    
  return 0;
}

//~============================================================================
