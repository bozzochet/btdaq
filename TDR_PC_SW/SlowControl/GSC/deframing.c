// file deframing.c
//
// Universal program for AMS-02 data deframing
//
// My software for AMS-1 deframing was used.
// A lot of software was stolen from X.Cai and reworked.
//
// A.Lebedev - Nov 2007...
//





// to do list (01/12/2007):
//
//   - check link vs current block end (check end)
//   - "nb_of" log file
//   - testing
//   - primitive event block decoder, print event number for tests
//   - framing software for tests (?)
//   - other frame formats (1553, 422, what else)
//   - in reading frames use "read" not "fread" (to read via network)







#include "gsclib.h"

bool CREATING_BLOCK_FILES = TRUE;
bool WRITING_BLOCK_FILES  = TRUE;

int P;
int PP = 1;

int current_file;

static  AMSframe  Frame;
static pAMSframe pFrame = &Frame;

static  AMSblock  Block[5];
static pAMSblock pBlock[5] = {
       &Block[0], &Block[1], &Block[2], &Block[3], &Block[4]};

int debug = 0x00000000;

//~============================================================================

int main( int argc, char *argv[]) {

  int i, k;
  char *frame_directory, *block_directory, *CHD_directory;
  int first_frame_file;
  int last_frame_file;
  FILE *frame_file = NULL, *block_file = NULL, *CHD_file = NULL;
  
//~--

  setbuf(stdout, NULL);
  printf("Usage: %s <Frame Directory> <Block Directory> <CHD Directory> [<First Frame File> [P]]\n",
         argv[0]);

  if (argc < 4) {
    printf("Too few parameters\n");
    exit(1);
  }
  
  k = 1;
  frame_directory = argv[k++];
  remove_trailing_slash(frame_directory);
  block_directory = argv[k++];
  remove_trailing_slash(block_directory);
  CHD_directory = argv[k++];
  remove_trailing_slash(CHD_directory);
  first_frame_file = -1;
  P = 0;
  if (argc > k) first_frame_file = atoi(argv[k++]);
  if (argc > k) P                = atoi(argv[k++]);

  printf("First Frame File = %0*d\n", 
    first_frame_file < 0 ? 1 : 7, first_frame_file);
  printf("P                = %d\n",   P);

  if (setpriority(PRIO_PROCESS, 0, -10) < 0) {
    printf("\n*** setpriority: %s\n", strerror(errno));
    exit(1);
  }

  if (!CREATING_BLOCK_FILES || !WRITING_BLOCK_FILES) {
    printf("***************************************************************\n");
    printf("*                                                             *\n");
  }
  if (!CREATING_BLOCK_FILES) {
    printf("*           THIS VERSION DOES NOT CREATE BLOCK FILES          *\n");
  }
  if (!WRITING_BLOCK_FILES) {
    printf("*           THIS VERSION DOES NOT WRITE BLOCK FILES           *\n");
  }
  if (!CREATING_BLOCK_FILES || !WRITING_BLOCK_FILES) {
    printf("*                                                             *\n");
    printf("***************************************************************\n");
  }
//~--

//initialize_everything();

  pFrame->wri = FALSE;
  for (i=0; i<DIM(Block); i++) pBlock[i]->was = 0xFFFF;
  for (i=0; i<DIM(Block); i++) pBlock[i]->sta = IDLE;

  if (first_frame_file < 0) {
    first_frame_file = the_last_file_in(block_directory);
  }
  else if (first_frame_file > 0) {
    first_frame_file = first_frame_file - 1;
  }

  if (first_frame_file < 0) {
    pFrame->wri = TRUE;
    first_frame_file = 0;
  }

  current_file = first_frame_file;

  printf("First frame file to read  = %s/%04d/%03d\n", 
         frame_directory, current_file/1000, current_file%1000);
  printf("First block file to write = %s/%04d/%03d\n",
         block_directory, current_file/1000, current_file%1000);
  printf("First CHD file to write   = %s/%04d/%03d\n",
         CHD_directory, current_file/1000, current_file%1000);

//~--
  
  while (1) {
    static bool first = TRUE;
    last_frame_file = the_last_file_in(frame_directory) - 1;  //thank you, Peter!
    for ( ; current_file <= last_frame_file; current_file++) {
      sprintf(file_name, "%04d/%03d", current_file/1000, current_file%1000);
      printf("%s...\n", file_name);
      first = TRUE;
      frame_file = open_file_for_reading(frame_directory, current_file);
      frame_number = 0;
      if (!frame_file) {
        for (i=0; i<DIM(Block); i++) pBlock[i]->sta = IDLE;
        continue;
      }
      if (pFrame->wri) {
        if (WRITING_BLOCK_FILES) {
          block_file = open_temp_file_for_writing(block_directory, 0);
          CHD_file   = open_temp_file_for_writing(CHD_directory, 0);
        }
      }
//    bzero(&nb_of, sizeof( nb_of));
      while (get_frame_from_file(frame_file, pFrame)) {
        frame_number++;
        if (pFrame->APID == 0x07FF) continue;
        if      (pFrame->APID == 976) i = 0;
        else if (pFrame->APID == 977) i = 1;
        else if (pFrame->APID == 981) i = 2;
        else if (pFrame->APID == 876) i = 3;
        else if (pFrame->APID == 980) i = 4;
        else {
          printf("%s: %6d *** Wrong APID = %03X = %d\n", 
                  file_name, frame_number, pFrame->APID, pFrame->APID);
          continue;
        }
        if (check_frame(pFrame, pBlock[i])) {
          extract_CHD(pFrame, CHD_file);
          deframe_frame(pFrame, pBlock[i], block_file);
        }
      }
      if (pFrame->wri) {
        fclose(frame_file);
        fclose(block_file);
        if (CREATING_BLOCK_FILES) {
          rename_temp_file(block_directory, 0, current_file);
        }
        fclose(CHD_file);
        if (CREATING_BLOCK_FILES) {
          rename_temp_file(CHD_directory, 0, current_file);
        }
//      print_deframing_log();
      }
      if (P > 2) {
        if (!pFrame->wri) printf("pFrame->wri now will be TRUE!\n");
      }
      pFrame->wri = TRUE;
//printf("switched\n"); P = 10;
    }
//  if (first) printf("Waiting...\n");
    first = FALSE;
    sleep(10);
//exit(1);
  }

  return 0;
}

//~============================================================================
