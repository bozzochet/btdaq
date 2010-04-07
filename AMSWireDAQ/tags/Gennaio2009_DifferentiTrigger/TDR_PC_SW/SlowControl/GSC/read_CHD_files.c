// file read_CHD_files.c
//
// Read, decode and print CHD files
//
// A.Lebedev - Mar 2008...
//

#include "gsclib.h"

int P;

static AMSCHD CHD, *pCHD = &CHD;

int current_file;

char file_name[80];

//~----------------------------------------------------------------------------

void process_CHD_file(FILE *file, int file_number) {

  sprintf(file_name, "%04d/%03d", file_number/1000, file_number%1000);
  if (P) printf("File %s: \n", file_name);

  while (1) {
    int i;
    int s = 0;
    int16 rea[5], *p = rea;
    s += fread(&pCHD->time,   4,  1, file);
    s += fread(&pCHD->APID,   2,  1, file);
    s += fread(&pCHD->seq,    2,  1, file);
//  s += fread(&pCHD->dat[0], 2,  5, file);
    s += fread(&rea[0], 2,  5, file);
    if (!s) break;
    if (s != 1 + 1 + 1 + 5) {
      if (P > 1) printf("%s         ***Error: s = %d\n", file_name, s);
    }
    for (i=0; i<10;) {
      pCHD->dat[i++] = *p   >> 8;
      pCHD->dat[i++] = *p++ >> 0;
    }
    if (P > 1) {
      struct tm *t;
      char b[80];
      t = localtime(&pCHD->time);
      sprintf(b, "%4d%02d%02d %02d:%02d:%02d ", 
              t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
              t->tm_hour, t->tm_min, t->tm_sec);
      printf("%s %s %d %5d ", 
             file_name, b, pCHD->APID, pCHD->seq);
      for (i=0; i<10; i++) printf("%02hhX ", pCHD->dat[i]);
      printf("\n");
    }
  }
}

//~============================================================================

int main( int argc, char *argv[]) {

  int k;
  char *CHD_directory;
  int first_CHD_file;
  int last_CHD_file;
  int nb_of_files;
  FILE *CHD_file;
    
//~--

  setbuf(stdout, NULL);
  fprintf(stderr, "Usage: %s "
         "<CHD Directory> [<First CHD File> [<Nb of Files> [P]]]\n",
         argv[0]);

  P = 0;
  if (argc < 2) {
    fprintf(stderr, "Too few parameters\n");
    exit(1);
  }
  
  k = 1;
  CHD_directory = argv[k++];
  remove_trailing_slash(CHD_directory);
  first_CHD_file = 0;
  nb_of_files = 1;
  P = 0;
  if (argc > k) first_CHD_file = atoi(argv[k++]);
  if (argc > k) nb_of_files    = atoi(argv[k++]);
  if (argc > k) P              = atoi(argv[k++]);

  if (first_CHD_file < 0) first_CHD_file = 0;
  last_CHD_file = first_CHD_file + nb_of_files - 1;

  fprintf(stdout, "First CHD File = %07d\n", first_CHD_file);
  fprintf(stdout, "Last CHD File  = %07d\n", last_CHD_file);
  fprintf(stdout, "P              = %d\n",   P);
  
//~--

  while (1) {
    last_CHD_file = MIN(last_CHD_file, the_last_file_in(CHD_directory));
    for (current_file=first_CHD_file; 
         current_file <= last_CHD_file; 
         current_file++) {
      CHD_file = open_file_for_reading(CHD_directory, current_file);
      if (!CHD_file) continue;
      process_CHD_file(CHD_file, current_file);
      fclose(CHD_file);
    }
exit(1);
    sleep(1);
  }

//~--

  return 0;
}
  
//~============================================================================
