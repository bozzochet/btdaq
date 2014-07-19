// file cab-cernox-file-converter.c
//
// Converts CERNOX sensor table files *.tbl
//
// A.Lebedev Jan-2009...

#include "mylib.h"

int P;

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  char *filename;
  FILE *file = NULL;
  int i, n = 0;
  char b[8192];

  setbuf(stdout, NULL);

  printf("Usage:\n");
  printf("%s <file-name> \n",argv[0]);

  if (argc < 2) exit(1);
  filename = argv[1];
  if (argc > 2) P = atoi(argv[2]);
  
  printf("filename = %s\n", filename);
  printf("P       = %d\n",   P);

  file = fopen(filename, "r");
  if (!file) {
    printf("File %s absent.\n", filename);
    exit(1);
  }

  while (1) {
    if (!fgets(b, sizeof(b), file)) break;
    n++;
  }
  fclose(file);
  
  n = n - 3;

  file = fopen(filename, "r");

  for (i=0; i<3; i++) fgets(b, sizeof(b), file);

  *index(filename, '.') = '\0';
  printf("  {\"%s\", %2d,\n", strrchr(filename, '/')+1, n);

  for (i=0; i<n; i++) {
    char c1[80], c2[80];
    int k;
    
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';
    if (P) printf(b);

    k = sscanf(b, "%s %s ", c1, c2);
    if (k == 2) {
      printf(" %c{%7s, %12s}%s,\n", i?' ':'{', c1, c2, i==(n-1)?"}}":"");
    }
  }

  return 0;
}

//~============================================================================
