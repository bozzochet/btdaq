// file print_APID_statistics.c
//
// reads frame files and prints APID statistics
//
// A.Lebedev - Jan 2008...
//

#include "gsclib.h"

int P;

//~============================================================================

int main( int argc, char *argv[]) {

  int i, k;
  int current_file;
  char *frame_directory;
  int first_frame_file;
  int last_frame_file;
  int n[100];
  FILE *frame_file;
  static  AMSframe  Frame;
  static pAMSframe pFrame = &Frame;
  
//~--

  setbuf(stdout, NULL);
  printf("Usage: %s <Frame Directory> [<First Frame File> [P]]\n", argv[0]);

  if (argc < 2) {
    printf("Too few parameters\n");
    exit(1);
  }
  
  k = 1;
  frame_directory = argv[k++];
  remove_trailing_slash(frame_directory);
  first_frame_file = 0;
  P = 0;
  if (argc > k) first_frame_file = atoi(argv[k++]);
  if (argc > k) P                = atoi(argv[k++]);

  if (first_frame_file < 0) first_frame_file = 0;

  printf("First Frame File = %07d\n", first_frame_file);
  printf("P                = %d\n",   P);
  
//~--

  printf("           FILL   SCI    HK   P2P\n");
  printf("    File: 0x7FF   976   977   981\n");
  while (1) {
    last_frame_file = the_last_file_in(frame_directory);
    for (current_file=first_frame_file; current_file <= last_frame_file; current_file++) {
      if (P > 1) printf("Process file %07d:\n", current_file);
      frame_file = open_file_for_reading(frame_directory, current_file);
      if (!frame_file) continue;
      for (i=0; i<100; i++) n[i] = 0;
      while (get_frame_from_file(frame_file, pFrame)) {
        if      (pFrame->APID == 0x07FF) n[0]++;
        else if (pFrame->APID ==    976) n[1]++;
        else if (pFrame->APID ==    977) n[2]++;
        else if (pFrame->APID ==    981) n[3]++;
        else {
          printf("*** unknown APID = %03X = %d\n", pFrame->APID, pFrame->APID);
        }
      }
      printf("%04d/%03d: ", current_file/1000, current_file%1000);
      printf("%5d %5d %5d %5d\n", n[0], n[1], n[2], n[3]);
    }
exit(1);
    sleep(1);
  }

  return 0;
}

//~============================================================================

