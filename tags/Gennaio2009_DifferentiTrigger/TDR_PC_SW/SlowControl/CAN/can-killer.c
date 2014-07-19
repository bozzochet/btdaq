// file can-killer.c
//
//  to deinitialize EPP-CAN box
//
//  A.Lebedev - Jul 2008...
//

#include "canlib.h"

int P = 3;

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int8 cha;
  
  if (initialize_EPP()) printf("initialize_EPP OK\n");
  else                  printf("initialize_EPP failed\n");
  
  for (cha=0; cha<2; cha++) de_initialize_CAN(cha);

  printf("de_initialize_CAN completed\n");
  return 0;
}

//~============================================================================
