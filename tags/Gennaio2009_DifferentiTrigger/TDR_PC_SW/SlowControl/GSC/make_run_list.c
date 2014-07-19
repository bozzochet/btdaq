// file make_run_list.c
//
// read block files, decode blocks, build run ID list
//
// A.Lebedev - Jan 2008...
//

#include "nodedef.h"
#include "gsclib.h"

int P;

static AMSBlock Block, *pBlock = &Block;
static int32 run_tag;
static int32 run_id, old_run_id = 0xFFFFFFFF;
static int32 event_id, old_event_id = 0xFFFFFFFF;
static int file_number;
static long file_pos, old_file_pos = -1;
static int old_file_number = -1;
static char run_time[80];
static int nblk1 = 0, nblk2 = 0;
static int nublk = 0, nsblk = 0, ntblk = 0, nrblk = 0, neblk = 0;

static bool calibration_run;

char file_name[80], header[80];
char string[160];

void print_AMS_block_header(int ident, AMSBlock *pBlock);
void print_run_bgn(void);
void print_run_end(void);
bool is_it(char c);
void process_AMS_block_file(FILE *file);

//~----------------------------------------------------------------------------

void print_AMS_block_header(int ident, AMSBlock *pBlock) {

  int i;
  
  if (!P) return;
  
  printf(header);
  
  for (i=0; i<ident; i++) printf(" ");
  
  printf("%s ", pBlock->BlockType & 1 ? "W" : "R");
  printf("NA=%03X", pBlock->NodeAddress);
  if (pBlock->NodeAddress < 512) printf("=%s ", NodeName[pBlock->NodeAddress]);
  else                           printf("=?");
  printf("DT=%02X ", pBlock->DataType);
  printf("DC=%d", pBlock->DataCount);
  printf(" ");
  printf("\n");
}

//~----------------------------------------------------------------------------

void print_run_bgn(void) {

  char *p = string;
  
  p += sprintf(p, "%08X ", run_id);
  p += sprintf(p, "%s ", calibration_run ? "C" : "D");
  p += sprintf(p, "%08X ", run_tag);
  p += sprintf(p, "%s : ", run_time);
  p += sprintf(p, "%d: ", event_id);
  p += sprintf(p, "%04d/%03d", file_number/1000, file_number%1000);
  p += sprintf(p, "/%-8ld ", file_pos);
}

//~----------------------------------------------------------------------------

void print_run_end(void) {

  printf(string);
  printf("-->");
  printf("%8d: ", old_event_id);
  printf("%8d%c ", nblk1, nblk1 == old_event_id ? ':' : '*');
  if (nblk2) printf("%8d%c ", nblk2, nblk2 == nblk1 ? ':' : '*');
  else       printf("        : ");
  printf("%04d/%03d", old_file_number/1000, old_file_number%1000);
  printf("/%-9ld * ", old_file_pos);
  if (nublk || nsblk || ntblk || nrblk || neblk) {
    printf("U=%-3d S=%-3d T=%-3d E=%-3d R=%-3d ", nublk, nsblk, ntblk, neblk, nrblk);
    if ((nublk + nsblk + ntblk + nrblk + neblk) != nblk1) printf("?");
  }
  printf("\n");
}

//~----------------------------------------------------------------------------

bool is_it(char c) {

  switch(toupper(c)) {
    case 'U':
      if (strcasestr(NodeName[pBlock->NodeAddress], "UDR") ||
          strcasestr(NodeName[pBlock->NodeAddress], "JINF-U")) return TRUE;
      else return FALSE;

    case 'S':
      if (strcasestr(NodeName[pBlock->NodeAddress], "SDR"))    return TRUE;
      else return FALSE;

    case 'T':
      if (strcasestr(NodeName[pBlock->NodeAddress], "TDR") ||
          strcasestr(NodeName[pBlock->NodeAddress], "JINF-T")) return TRUE;
      else return FALSE;

    case 'R':
      if (strcasestr(NodeName[pBlock->NodeAddress], "RDR") ||
          strcasestr(NodeName[pBlock->NodeAddress], "JINF-R")) return TRUE;
      else return FALSE;

    case 'E':
      if (strcasestr(NodeName[pBlock->NodeAddress], "EDR") ||
          strcasestr(NodeName[pBlock->NodeAddress], "JINF-E")) return TRUE;
      else return FALSE;

    case 'J':
      if (strcasestr(NodeName[pBlock->NodeAddress], "JINJ-2") ||
          strcasestr(NodeName[pBlock->NodeAddress], "JINJ-3")) return TRUE;
      else return FALSE;

    default:
      return FALSE;
  }
}

//~----------------------------------------------------------------------------

void process_AMS_block_file(FILE *file) {

  int32 len;
  int16 dat[1000000];
  int16 *p, *end;

  sprintf(file_name, "%04d/%03d", file_number/1000, file_number%1000);

  while (1) {
    file_pos = ftell(file);
    if (!read_AMS_block(file, dat, &len)) break;
    unpack16_AMS_block(dat, len, TRUE, pBlock);
    if (pBlock->NodeAddress < 0x18 && 
        (pBlock->DataType == 0x05 || pBlock->DataType == 0x06)) {
      bool calibration = pBlock->DataType == 0x06;
      if (calibration) sprintf(header, "%s C ", file_name);
      else             sprintf(header, "%s D ", file_name);
      print_AMS_block_header(1, pBlock);
      end = dat + (len + 1) / 2;
      p   = pBlock->Data.p16;
      asctime_r(localtime((time_t*)&pBlock->DataTime), run_time);
      run_time[19] = '\0';
      while (1) {
        int16 *pp;
        len = *p++;
        unpack16_AMS_block(p, len, FALSE, pBlock);
        pp = pBlock->Data.p16;

        if (pBlock->DataType == 0x07 || pBlock->DataType == 0x05) {
          print_AMS_block_header(3, pBlock);
          if (P > 1) print_AMS_data_block("", pBlock);
          run_tag = *pp++;
          run_tag = run_tag << 16 | *pp++;
          run_id = *pp++;
          run_id = run_id << 16 | *pp++;
          event_id = *pp++;
          event_id = event_id << 16 | *pp++;
          if (run_id != old_run_id) {
            if (old_run_id != 0xFFFFFFFF) {
              print_run_end();
              nblk1 = 0;
              nblk2 = 0;
              nublk = 0;
              nsblk = 0;
              ntblk = 0;
              nrblk = 0;
              neblk = 0;
            }
            calibration_run = calibration;
            print_run_bgn();
          }
          nblk1 = nblk1 + 1;
          old_run_id = run_id;
          old_event_id = event_id;
          old_file_pos = file_pos;
          old_file_number = file_number;
        }
        
        else if (pBlock->DataType == 0x01) {
          print_AMS_block_header(3, pBlock);
          nblk2 = nblk2 + 1;
        }

        else if (pBlock->DataType == 0x13 || pBlock->DataType == 0x14) {
          print_AMS_block_header(3, pBlock);
          if (pBlock->NodeAddress < 512) {
            if (is_it('U')) {
              nublk++;
            }
            if (is_it('S')) {
              nsblk++;
            }
            if (is_it('T')) {
              ntblk++;
            }
            if (is_it('R')) {
              nrblk++;
            }
            if (is_it('E')) {
              neblk++;
            }
          }
        }

        else {
          print_AMS_block_header(3, pBlock);
        }

        p = p + (len + 1) / 2;
        if (p >= end) {
          if (p > end) {
            printf("%s       *** Error: Pointer mismatch(1): p=%08X, end=%08X\n", 
                   header, (int32)p, (int32)end);
          }
          break;
        }
      }
    }
  }
}

//~============================================================================

int main( int argc, char *argv[]) {

  int k;
  int current_file;
  char *block_directory;
  int first_block_file;
  int last_block_file;
  FILE *block_file;
    
//~--

  setbuf(stdout, NULL);
  printf("Usage: %s <Block Directory> [<First Block File> [P]]\n", argv[0]);

  P = 0;
  if (argc < 2) {
    printf("Too few parameters\n");
    exit(1);
  }
  
  k = 1;
  block_directory = argv[k++];
  remove_trailing_slash(block_directory);
  first_block_file = 0;
  P = 0;
  if (argc > k) first_block_file = atoi(argv[k++]);
  if (argc > k) P                = atoi(argv[k++]);

  if (first_block_file < 0) first_block_file = 0;

  printf("First Block File = %07d\n", first_block_file);
  printf("P                = %d\n",   P);
  
  current_file = first_block_file; 

//~--

  while (1) {
    static bool first = TRUE;
    last_block_file = the_last_file_in(block_directory);
    for (; current_file <= last_block_file; current_file++) {
      first = TRUE;
      block_file = open_file_for_reading(block_directory, current_file);
      if (!block_file) continue;
      file_number = current_file;
      process_AMS_block_file(block_file);
      fclose(block_file);
    }
    if (first) fprintf(stderr, "Waiting... push Q to stop.\n");
    first = FALSE;
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case 'Q':
          print_run_end();
          exit(0);
        break;
      }
    }
    sleep(1);
  }

//~--

  return 0;
}
  
//~============================================================================
