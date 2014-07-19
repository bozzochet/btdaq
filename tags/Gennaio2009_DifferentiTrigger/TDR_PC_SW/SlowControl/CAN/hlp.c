// file hlp.c
//
// To convert CERNOX calibration data provided by Stuart 
// into ccebdef.h format
//
// A.Lebedev Sep-2007...

#include "mylib.h"

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  char filename[] = {"X48012.txt"};
  FILE *file = NULL;
  char b[10000];
  int k;
  char n[100], R[100], T[100];
    
  setbuf(stdout, NULL);

  file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "File %s absent.\n", filename);
    exit(1);
  }
  
  while (fgets(b, sizeof(b), file)) {
    b[strlen(b)-1] = '\0';
    k = sscanf(b, "%s %s %s", n, T, R);
    if (k == 3) printf("  {%s, %s},\n", T, R);
//printf("k = %d, n = %s, T = %s, R = %s\n", k, n, T, R);
  }

  return 0;
}

//~============================================================================
