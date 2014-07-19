// file make_data_type_list.c
//
// read block files, decode blocks, build DataType list
//
// A.Lebedev - Jan 2008...
//

#include "gsclib.h"

int P;

static AMSBlock Block, *pBlock = &Block;

int current_file;

int32 DataType[0x200000];

//~----------------------------------------------------------------------------

void print_results(void) {

  int i;
  
  printf("After file %07d:\n", current_file);
  for (i=0; i<0x1FFFFF; i++) {
  int32 n = DataType[i];
    if (n) {
      if (i < 0x1F0000) printf("    %02X %8d\n", i, n);
      else              printf("%06X %8d\n", i, n);
    }
  }
}

//~----------------------------------------------------------------------------

void process_AMS_block_file(FILE *file, int file_number) {

  int32 len;
  int16 dat[1000000];

  while (1) {
    if (!read_AMS_block(file, dat, &len)) break;
    unpack16_AMS_block(dat, len, TRUE, pBlock);
    DataType[pBlock->DataType]++;
  }
}

//~============================================================================

int main( int argc, char *argv[]) {

  int k;
  char *block_directory;
  int first_block_file, last_block_file;
  int nb_of_files;
  FILE *block_file;
    
//~--

  setbuf(stdout, NULL);
  printf("Usage: %s "
         "<Block Directory> [<First Block File> [<Nb of Files> [P]]]\n",
         argv[0]);

  P = 0;
  if (argc < 2) {
    printf("Too few parameters\n");
    exit(1);
  }
  
  k = 1;
  block_directory = argv[k++];
  remove_trailing_slash(block_directory);
  first_block_file = 0;
  nb_of_files = 1;
  P = 0;
  if (argc > k) first_block_file = atoi(argv[k++]);
  if (argc > k) nb_of_files      = atoi(argv[k++]);
  if (argc > k) P                = atoi(argv[k++]);

  if (first_block_file < 0) first_block_file = 0;
  last_block_file = first_block_file + nb_of_files - 1;

  printf("First Block File = %07d\n", first_block_file);
  printf("Last Block File  = %07d\n", last_block_file);
  printf("P                = %d\n",   P);
  
//~--

  memset((int8*)DataType, 0, sizeof(DataType));
  
  current_file = first_block_file; 

  while (1) {
    static bool first = TRUE;
    for (; current_file <= the_last_file_in(block_directory); current_file++) {
      if (current_file > last_block_file) {
        current_file = current_file - 1;
        print_results();
        exit(0);
      }
      first = TRUE;
      block_file = open_file_for_reading(block_directory, current_file);
      if (!block_file) continue;
      process_AMS_block_file(block_file, current_file);
      fclose(block_file);
      if (kbhit()) {
        int ch = toupper(fgetc(stdin));
        switch (ch) {
        case '?':
          print_results();
        break;
        }
      }
    }
//exit(1);
    if (first) fprintf(stderr, "Waiting for a new file... \r");
    first = FALSE;

    sleep(1);
  }

//~--

  return 0;
}
  
//~============================================================================
