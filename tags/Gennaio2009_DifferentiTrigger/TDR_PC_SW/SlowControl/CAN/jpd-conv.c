// file jpd-conv.c
//
// Program to convert JPD "data base" - Dallas sensors IDs...
//
// A.Lebedev - Dec 2005...
//

#include "mylib.c"

int main(void) {

  FILE *file;
  int i, n;
  char b[120], bb[120];
  
  file = fopen("jpd-temp.txt", "r");

  while (!feof(file)) {
    fgets(b, 120, file);
    n = strlen(b);
//  printf("%4d :%s", n, b);
    for (i=0; i<n; i++) {
      bb[i] = b[i];
      if (n > 50) {
        bb[22] = b[36];
        bb[23] = b[37];
        bb[24] = b[34];
        bb[25] = b[35];
        bb[26] = b[32];
        bb[27] = b[33];
        bb[28] = b[30];
        bb[29] = b[31];
        bb[30] = b[28];
        bb[31] = b[29];
        bb[32] = b[26];
        bb[33] = b[27];
        bb[34] = b[24];
        bb[35] = b[25];
        bb[36] = b[22];
        bb[37] = b[23];

        bb[44] = b[58];
        bb[45] = b[59];
        bb[46] = b[56];
        bb[47] = b[57];
        bb[48] = b[54];
        bb[49] = b[55];
        bb[50] = b[52];
        bb[51] = b[53];
        bb[52] = b[50];
        bb[53] = b[51];
        bb[54] = b[48];
        bb[55] = b[49];
        bb[56] = b[46];
        bb[57] = b[47];
        bb[58] = b[44];
        bb[59] = b[45];
      }
    }
    bb[n] = '\0';
//  printf("%4d *%s", n, bb);
    printf(bb);
  }
  
  return 0;
}

