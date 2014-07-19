// file pri.c
//
// "priority" for DS readout studies
//
// A.Lebedev Aug-2006...
//

#include "uscmlib.h"

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int8 loop;
  int8 pri[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  
  setbuf(stdout, NULL);

  loop = 0;
  while (1) {
    printf("loop = %3d: ", loop);
    for (i=0; i<10; i++) {
      if (!pri[i] || (int8)(loop << (pri[i] - 1))) continue;
      printf("%d ", i);
    }
    printf("\n");
    loop++;
    if (!loop) break;
  }
  
  return 0;
}

//~============================================================================
