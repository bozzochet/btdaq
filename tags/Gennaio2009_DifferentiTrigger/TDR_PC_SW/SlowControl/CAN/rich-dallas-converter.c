// file rich-dallas-converter.c
//
// Program to convert RICH Dallas sensor IDs
//
// A.Lebedev - Mar-2008...
//

#include "mylib.c"

int P;


//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  char *filename;
  FILE *file = NULL;
  int n = 0;
  
  setbuf(stdout, NULL);
  printf("Usage:\n");
  printf("%s <file-name> [<P>]\n",argv[0]);

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
    int64 ID;
    char b[8192], c1[80], c2[80];
    int k;
    int16 _1,  _2,  _3,  _4;
    
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';

    if (P) printf(b);

    k = sscanf(b, "%hx %hx %hx %hx %s %s", &_1,  &_2,  &_3,  &_4, c1, c2);

    ID  = (int64)_1 << (3 * 16);
    ID |= (int64)_2 << (2 * 16);
    ID |= (int64)_3 << (1 * 16);
    ID |= (int64)_4 << (0 * 16);

    if (P) printf(" : k = %d, ID = %016llX %s %s\n", k, ID, c1, c2);
    if (k == 6) {
      printf("%2d %016llX  9 %s %s\n", n, ID, c1, c2);
      n++;
    }
  }

  return 0;
}

//~============================================================================
