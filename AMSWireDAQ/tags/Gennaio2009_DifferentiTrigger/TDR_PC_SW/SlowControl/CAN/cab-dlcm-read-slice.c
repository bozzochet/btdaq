// file cab-dlcm-read-slice.c
//
//  Get DLCM "Slice" and print it.
//
//  A.Lebedev Nov - 2008...
//

#include "cablib.h"

int JMDC_ID = 0x03;                                // global variable
int USCM_ID = 0x18;                                // global variable
int P = 0;                                         // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

//~----------------------------------------------------------------------------

void print_DLCM_slice(int8 *dat) {

  int i, j;

//~--

  printf("DLCM Slice:\n");
  for (i=0; i<19; i++) printf("%02hhX ", dat[i]);
  printf("\n");

//~--

  printf("  TM_ID          = %02hhX%02hhX%02hhX%02hhX\n", 
         dat[0], dat[1], dat[2], dat[3]);
  printf("  CYCLE_POSITION = %4d\n", (int16)dat[4]*256+dat[5]);
  printf("  NUM_CYCLES     = %4d\n", dat[6]);
  
  for (i=0; i<dat[6]; i++) {
    int k = 20 * i;
    printf("  CYCLE %4d: ", (int16)dat[4]*256+dat[5]+i);
    printf("%02hhX%02hhX%02hhX%02hhX %.3fs", 
           dat[7+k], dat[8+k], dat[9+k], dat[10+k],
           0.0125*(((dat[7+k]*256+dat[8+k])*256+dat[9+k])*256+dat[10+k]));
    for (j=0; j<16; j+=2) {
      printf("%6d ", (int16)dat[11+j+k]*256+dat[12+j+k]);
    }
    printf("\n");
  }
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int16 err;
  int8 dat[256];
  
  setbuf(stdout, NULL);

  printf("Usage: %s [<USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",
          argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = atoi(argv[2]);
  if (argc > 3) P = atoi(argv[3]);

  printf("USCM ID = 0x%02X\n", USCM_ID);
  printf("JMDC ID = %d\n", JMDC_ID);
  printf("P       = %d\n", P);

  setup_command_path_special();

  if (!initialize_EPP()) exit(1);

  DLCM_read_slice(dat, &err);
  printf("err = %04X", err);
  if (err) {
    if (err > 0x2000) printf(" = %s", cab_ack_message(err-0x2000));
    printf("\n");
  }
  else {
    printf("\n");
    print_DLCM_slice(dat);
  }
  
  return 0;
}

//~----------------------------------------------------------------------------
