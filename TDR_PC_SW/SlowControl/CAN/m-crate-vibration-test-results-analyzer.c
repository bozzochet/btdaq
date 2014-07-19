// file m-crate-vibration-test-result-analyzer.c
//
// Program to analyze results of the "failed" M-Crate vibration test
//
// A.Lebedev Aug-2007...

#include "mylib.h"


//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  char filename[] = {"m-crate-vibration-test.txt"};
  FILE *file = NULL;
  char b[8192];
  bool print = FALSE;
  
  setbuf(stdout, NULL);

  file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "File %s absent.\n", filename);
    exit(1);
  }
  
  while (fgets(b, sizeof(b), file)) {
    if (strstr(b, "_serial_port failed")) print = TRUE;
    if (print) printf(b);
    if (strstr(b, "USCM setup"))     print = FALSE;
  }

  return 0;
}

//~============================================================================
