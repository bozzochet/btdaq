// file ccbt-4.c
//
// To test read_CCBT_data function.
//
// A.Lebedev - Dec 2006...
//

#include "uscmlib.h"

_CCBT_data CCBT_data[2];

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  bool iopt_P = FALSE;
  int16 err;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [USCM_ID [JMDC_ID [P]]]]\n",argv[0]);

  if (argc > 1) USCM_ID  = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID  = strtoul(argv[2], NULL, 16);
  if (argc > 3) P        = atoi(argv[3]);

  printf("USCM_ID = %03X\n", USCM_ID);
  printf("JMDC_ID = %d\n",   JMDC_ID);
  printf("P       = %d\n",   P);
  
  if (!initialize_EPP()) exit(1);

  while (1) {
    read_CCBT_data(CCBT_data, &err);
    if (iopt_P) {
      int i, j;
      for (j=0; j<2; j++) {
        printf("%d: %s age=%d ", j, CCBT_data[j].ok ? "OK" : "KO", CCBT_data[j].age);
        for (i=0; i<32; i++) {
          printf("%04X ", CCBT_data[j].value[i]);
        }
        printf("\n");
      }
    }
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case 'Q':
          exit(0);
        break;
        case 'P':
          iopt_P = iopt_P ? FALSE : TRUE;
        break;
      }
    }
  }
}

//~=============================================================================
