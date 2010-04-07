// file write_file_during_reading.c
//
// Study how to read file when it is being written
//
// A.Lebedev - Jan-2008...

#include "gsclib.h"

int P;

//~----------------------------------------------------------------------------

void print_help(void) {

  setbuf(stdout, NULL);
  printf("Commands:\n");
  printf("  ? = print help\n");
  printf("  H = print help\n");
  printf("  O = open file\n");
  printf("  W = write file\n");
  printf("  C = close file\n");
  printf("  Q = quit\n");
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  FILE *file = NULL;
  char *file_name = {"testfile"};
  int s, c = 50;
  int8 dat[100];
  
  print_help();
  
  while (1) {
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
      case '?':
      case 'H':
        print_help();
      break;
      case 'O':
        printf("Open file %s...  ", file_name);
        file = fopen(file_name, "w");
        if (!file) printf("fopen %s failed: %s\n", file_name, strerror(errno));
        else       printf("%s opened\n", file_name);
      break;
      case 'C':
        if (!file) {
          printf("Error: no file open\n");
          break;
        }
        printf("Close file %s... ", file_name);
        if (fclose(file)) printf("fclose %s failed: %s\n", file_name, strerror(errno));
        else              printf("%s closed\n", file_name);
        file = NULL;
      break;
      case 'W':
        if (!file) {
          printf("Error: no file open\n");
          break;
        }
        printf("Write file %s... ", file_name);
        s = fwrite(dat, 1, c, file);
        if (s != c) printf("fwrite %s failed: %s\n", file_name, strerror(errno));
        else        printf("fwrite %s OK\n", file_name);
      break;
      case 'Q':
        exit(0);
      break;
      }
    }
  }

  return 0;
}

//~============================================================================
