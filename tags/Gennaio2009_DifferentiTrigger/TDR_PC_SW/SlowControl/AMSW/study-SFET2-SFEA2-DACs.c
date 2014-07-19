// file q.c
//
// To understand SFET2/SFEA2 DACs
//
// A.Lebedev May-2007...

#include "mylib.h"

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  float V_ref = 2.50;
  float V, myV;
  
  for (i=0; i<256; i++) {
    V   = 2.0 * (float)i / 256.0 * V_ref;
    myV = 2.0 * (float)i / 256.0 * 2.56;
    printf("i = %3d, V = %f, myV = %f, dif = %f\n", i, V, myV, V-myV);
  }  
  return 0;
}

//~============================================================================
