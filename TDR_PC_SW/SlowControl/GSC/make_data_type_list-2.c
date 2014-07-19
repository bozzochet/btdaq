// file make_data_type_list-2.c
//
// read block files, decode blocks, build DataType list
//
// A.Lebedev - Jan 2008...
//

#include "nodedef.h"
#include "gsclib.h"

int P;
int fmt;

static AMSBlock Block, *pBlock = &Block;

int current_file;

struct {
  int adr;
  int typ;
  int n[3];
} list[10000];

//~----------------------------------------------------------------------------

void print_results(void) {

  int i, j, n, adr, typ;
  
  printf("After file %07d:\n", current_file);
  for (n=0; n<DIM(list); n++) if (list[n].n[0] < 0) break;
  printf("ADR ---NAME----       DT        R        W        ?\n");

  for (adr=0; adr<512; adr++) {
    for (i=0; i<n; i++) {
      if (adr == list[i].adr) {
        if (fmt == 2) printf("%03X %-11s\n", adr, NodeName[adr]);
        for (typ=0; typ<0x1FFFFF; typ++) {
          for (j=0; j<n; j++) {
            if (adr == list[j].adr && typ == list[j].typ) {
              if (fmt != 2) printf("%03X %-11s ", adr, NodeName[adr]);
              if (fmt == 2) printf("  ");
              if (typ < 0x1F0000) {
                printf("      %02X %8d %8d %8d\n", 
                       typ, list[j].n[0], list[j].n[1], list[j].n[2]);
              }
              else {
                printf("  %06X %8d %8d %8d\n",
                       typ, list[j].n[0], list[j].n[1], list[j].n[2]);
              }
              break;
            }
          }
        }
        break;
      }
    }
  }
/*
  for (adr=0; adr<512; adr++) {
    for (i=0; i<n; i++) {
      if (adr == list[i].adr) {
        printf("%03X %-10s ", adr, NodeName[adr]);
        typ = list[i].typ;
        if (typ < 0x1F0000) printf("      %02X %8d\n", typ, list[i].n);
        else                printf("  %06X %8d\n",     typ, list[i].n);
      }
    }
  }
*/
}

//~----------------------------------------------------------------------------

void process_AMS_block_file(FILE *file, int file_number) {

  int i;
  int32 len;
  int16 dat[1000000];

  while (1) {
    if (!read_AMS_block(file, dat, &len)) break;
    unpack16_AMS_block(dat, len, TRUE, pBlock);
#if 0 // study of misterious DataType to JINJ -> this is group command to retrieve some data from U-Crate (?)
if (
   (pBlock->NodeAddress == 0x082 || pBlock->NodeAddress == 0x083) &&
   (pBlock->DataType    == 0x1F0381)) {
  print_AMS_data_block("", pBlock);
}
#endif
    for (i=0; i<DIM(list); i++) {
      if (list[i].n[0] < 0) {
        list[i].adr = pBlock->NodeAddress;
        list[i].typ = pBlock->DataType;
        list[i].n[0] = list[i].n[1] = list[i].n[2] = 0;
      }
      if (list[i].adr == pBlock->NodeAddress && list[i].typ == pBlock->DataType) {
        if      (pBlock->ReadWrite == READ)  list[i].n[0]++;
        else if (pBlock->ReadWrite == WRITE) list[i].n[1]++;
        else                                 list[i].n[2]++;
        break;
      }
    }
    if (i >= DIM(list)) {
      printf("*** Error: List overflow\n");
      print_results();
      exit(1);
    }
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
         "<Block Directory> [<First Block File> [<Nb of Files> [fmt [P]]]]\n",
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
  fmt = 1;
  if (argc > k) first_block_file = atoi(argv[k++]);
  if (argc > k) nb_of_files      = atoi(argv[k++]);
  if (argc > k) P                = atoi(argv[k++]);
  if (argc > k) fmt              = atoi(argv[k++]);

  if (first_block_file < 0) first_block_file = 0;
  last_block_file = first_block_file + nb_of_files - 1;

  printf("First Block File = %07d\n", first_block_file);
  printf("Last Block File  = %07d\n", last_block_file);
  printf("P                = %d\n",   P);
  printf("fmt              = %d\n",   fmt);
  
//~--

  memset((int8*)list, 0xFF, sizeof(list));
  
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

    sleep(10);
  }

/*
  while (1) {
    last_block_file = MIN(last_block_file, the_last_file_in(block_directory));
    for (current_file=first_block_file; 
         current_file <= last_block_file; 
         current_file++) {
      if (P > 1) printf("Process file %07d:\n", current_file);
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
    print_results();
exit(1);
    sleep(1);
  }
*/

//~--

  return 0;
}
  
//~============================================================================
