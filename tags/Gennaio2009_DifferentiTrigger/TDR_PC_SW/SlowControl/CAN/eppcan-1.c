// file eppcan-1.c
//
//  to understand EPPCAN Box
//
//  A.Lebedev Aug 2006...
//

#include "epplib.h"

int P = 3;

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  if (initialize_EPP()) printf("OK\n");
  else                  printf("KO\n");
  while (1) {
    delay(1.0);
    if (initialize_EPP()) printf("OK\n");
    else                  printf("KO\n");
  }
  
  return 0;
}

//~============================================================================
