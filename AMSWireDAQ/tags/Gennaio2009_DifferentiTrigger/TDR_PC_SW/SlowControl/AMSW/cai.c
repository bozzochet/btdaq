// file cai.c
//
// Program to test "Slave Test" procedure: XC said it does not work.
//
// A.Lebedev Jan-2008... 
//

#include "jlv1def.h"
#include "jinflib.h"

int TX = 1;                 // global variable
int RX = 1;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int i, k;
  int nev = 0;
  int Delay = 0;
  int32 masks[16], old_mask = 0;
  int16 err = 0;
  bool first = TRUE;

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: \n");
  printf("  %s <TX&RX> {<Path>|0} [<delay> [P]]\n", argv[0]);

  k = 1;
  if (argc > k) TX = RX      = atoi(argv[k++]);
  if (argc > k) AMSW_PATH[0] = strtoul(argv[k++], NULL, 16);
  if (argc > k) Delay        = atoi(argv[k++]);
  if (argc > k) P            = atoi(argv[k++]);
  printf("TX    = %d\n", TX);
  printf("RX    = %d\n", RX);
  printf("Path  = 0x%04X\n", AMSW_PATH[0]);
  printf("delay = %d us\n", Delay);
  printf("P     = %d\n", P);

  while (1) {
//    set_JINF_slave_mask(0, 0x000000, &err);
    if (err) EXIT("set_JINF_slave_mask failed");
    test_JINF_slaves(&err);
    if (err) EXIT("test_JINF_slaves failed");
    delay((float)Delay/1000000.0);
    get_JINF_slave_masks(masks, &err);
    if (err) EXIT("get_JINF_slave_masks failed");
    if (!first) {
      if ((old_mask != masks[0]) || (P < 0)) {
        printf("%d: was = %06X, now = %06X\n", nev, old_mask, masks[0]);
      }
    }
    first = FALSE;
    old_mask = masks[0];
    nev = nev + 1;
    if (!(nev % 1000)) printf("\r             %d\r", nev);
  }

  return 0;
}

//~============================================================================
