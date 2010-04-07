// file fw.c
//
// Example program for VK (disk writing for ISIL APS tests) 
//
// A.Lebedev Feb-2005...
//

#include "mylib.h"
#include "klib.h"

int32 max_frames_written = 250000;
int P = 0;

char *directory_name = {"FRAMES"};

char file_name[80] = {""};
long file_number;
int32 frame_number;
int32 frames_written;
int32 *frame;

int opt_L = 0;
int opt_P = 0;
int opt_W = 0;

FILE *file;

extern int32 *HRDLpDMA;
extern int32 *HRDLvDMA;
extern int8  *HRDLDpram;
extern volatile int32 *HRDLRegisters;
int32 *Buffer0;
int32 *Buffer1;
int32 *HRDLram;

//~----------------------------------------------------------------------------

bool init_pmchrdl(void) {

  int32 val;

  if ( ! pmchrdl_open()) {
     printf("No PMCHRDL board detected in system\n");
     pmchrdl_close();
     return FALSE;
  }
  Buffer0 = HRDLvDMA;
  Buffer1 = HRDLvDMA + 1024;
  HRDLram = (int32 *)HRDLDpram;

  HRDLRegisters[PMCHRDL_TX0_STATUS] = 0x80000000;
  //   HRDLRegisters[PMCHRDL_TX0_CTRL]   = 0x00000033;
  //   HRDLRegisters[PMCHRDL_TX0_GAP]    = 0x0001de97;
  HRDLRegisters[PMCHRDL_TX0_START]  = 0x00038000;
  HRDLRegisters[PMCHRDL_TX0_END]    = 0x0003bfff;
  HRDLRegisters[PMCHRDL_TX0_STATUS] = 0x80000000;
  HRDLRegisters[PMCHRDL_TX1_STATUS] = 0x80000000;
  //   HRDLRegisters[PMCHRDL_TX1_CTRL]   = 0x00000033;
  //   HRDLRegisters[PMCHRDL_TX1_GAP]    = 0x0001de85;
  HRDLRegisters[PMCHRDL_TX1_START]  = 0x0003c000;
  HRDLRegisters[PMCHRDL_TX1_END]    = 0x0003ffff;
  HRDLRegisters[PMCHRDL_TX1_STATUS] = 0x80000000;

  HRDLRegisters[PMCHRDL_RX_CTRL]    = 0x00037fff;
  //  HRDLRegisters[PMCHRDL_RX_STATUS]  = 0x80000000;
  //  printf("RX_STATUS %08lX  ",HRDLRegisters[PMCHRDL_RX_STATUS]);
  //  printf("RX_DESCRIP %08lX\n",HRDLram[0x00000]);

  //  HRDLRegisters[PMCHRDL_DMA_STATUS] = 0x00000000;
  //  HRDLRegisters[PMCHRDL_DMA_CTRL]   = 0x00000000;
  //  HRDLRegisters[PMCHRDL_DMA_PCIADR] = 0x00000000;
  //  HRDLRegisters[PMCHRDL_DMA_MEMADR] = 0x00000000;
  return TRUE;
}

//~----------------------------------------------------------------------------

bool get_frame(int32 **frame) {

  int32 val;
  int32 length;
  int32 addr;
  int32 new_addr;
  int32 descriptor;
  static bool dma_active = FALSE;
  static int bn = 0;

  if (dma_active) {  
    while ((HRDLRegisters[PMCHRDL_DMA_STATUS] & PMCHRDL_DMA_DONE) == 0);
    dma_active = FALSE;
    val = HRDLRegisters[PMCHRDL_RX_STATUS];
    if ((val & PMCHRDL_RX_OVRRUN) != 0) {
      printf("Overrun\n");
    }
    else {
      HRDLRegisters[PMCHRDL_RX_STATUS] = PMCHRDL_RX_NEXT;
    }
  }

was_overrun:
  val = HRDLRegisters[PMCHRDL_RX_STATUS];
  if ((val & PMCHRDL_RX_OVRRUN) != 0) {
    printf("Overrun\n");
    goto was_overrun;
  }
  if ((val & PMCHRDL_RX_EMPTY) != 0) return FALSE;

  addr = val & 0x3ffff;
  descriptor = HRDLram[addr];
  length = (descriptor & 0x7FFC0000) >> 18;
  new_addr = descriptor & 0x3ffff;
  if ((new_addr != 0) && ((1 + addr +((length +3)/4)) != new_addr)) {
    printf("%05lx %05lx %05lx\n",addr,(1 + addr + ((length +3)/4)),new_addr);
    exit(1);
  }

  HRDLRegisters[PMCHRDL_DMA_PCIADR] = (int32)HRDLpDMA + 4096*bn;
  HRDLRegisters[PMCHRDL_DMA_MEMADR] = addr + 1;
  HRDLRegisters[PMCHRDL_DMA_STATUS] = 0x40000400;
  dma_active = TRUE;

  *frame = bn ? Buffer0 : Buffer1;
  bn = (bn + 1) & 1;

  return TRUE;
}

//~============================================================================

long last_file_number_in(char *DirPath) {

  long file_number;
  long last_file_number;
  DIR  *dir;
  struct dirent *d;

  if (P) printf("-- Start scan of directory:  %s\n", DirPath);
  if ((dir = opendir(DirPath)) == NULL) {
    printf("opendir(%s) failed: %s\n", DirPath, strerror(errno));
    return -1;
  }
  last_file_number = 0;
  while ((d = readdir(dir)) != NULL) {
    if (fnmatch("[0-9][0-9][0-9][0-9]", d->d_name, 0) == 0) {
      file_number = atol(d->d_name);
      last_file_number = MAX(last_file_number, file_number);
    }
  }
  if (closedir(dir)) {
    printf("closedir(%s) failed: %s\n", DirPath, strerror(errno));
    return -1; 
  }
  if (P) printf("-- Last file number found in %s is %04ld\n", 
                DirPath, last_file_number);
  
  return last_file_number;
}

//~----------------------------------------------------------------------------

void print_frame(int32 frame_number, int32 *frame) {

  int i, N = 16;

  printf("Frame %d   %08x\n", frame_number, (int32)frame);

  for (i=0; i<64; i++) {
    if (( i    % N) == 0) printf("%4d: ", i);
    printf("%08X ", frame[i]);
    if (((i+1) % N) == 0) printf("\n");
  }

}

//~----------------------------------------------------------------------------

float rate(int32 counter) {

  static struct timeval time_now, time_then;
  static float counter_now, counter_then;
  float Rate;

  gettimeofday(&time_now, NULL);
  counter_now = counter;
  Rate = (counter_now - counter_then) / delta_t(&time_now, &time_then);
  gettimeofday(&time_then, NULL);
  counter_then = counter;

  return Rate;
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  printf(
  "Usage: %s [<Max Nb of Frames in a File> [<Debug Printout Level>]]\n", argv[0]);
  if (argc > 1) max_frames_written = atoi(argv[1]);
  if (argc > 2) P = atoi(argv[2]);
  printf("max_frames_written = %ld\n", max_frames_written);
  printf("P                  = %d\n", P);

  if (!init_pmchrdl()) {
    printf("failed to init pmchrdl.\n");
    exit(1);
  }

  file_number = last_file_number_in(directory_name);
  if (file_number < 0) {
    printf("last_file_number_in failed.\n");
    exit(1);
  }
  file_number++;
  
  while (1) {
    if (get_frame(&frame)) {
      if (opt_P || opt_L) print_frame(frame_number, frame);
      opt_P = 0;
      if (opt_W) {
        fwrite(frame, 4096, 1, file);
        frames_written++;
        if (!(frames_written % max_frames_written)) {
          fclose(file);
          if (P) {
            printf("-- File %s closed (size limit reached).", file_name);
            printf(" %d frames written (total)\n", frames_written);
          }
          sprintf(file_name, "%s/%04ld", directory_name, file_number);
          file = fopen(file_name, "w");
          if (!file) {
            printf("fopen failed.\n");
            exit(1);
          }
          else {
            if (P) printf("-- File %s opened\n", file_name);
          }
          file_number++;
        }
      }
      frame_number++;
    }
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
      case '?':
      case '/':
        printf("Frame number %d.", frame_number);
        if (opt_W) {
          printf(" Writing file %s", file_name);
          printf(", %d frames written (total) so far", frames_written);
          printf(" (%.0f frames/sec).", rate(frames_written));
        }
        else {
          printf(" Not writing.");
          if (strlen(file_name)) {
            printf(" Last file was %s.", file_name);
          }
        }
        printf("\n");
        break;
      case 'L': opt_L = 1 - opt_L; break;
      case 'P': opt_P = 1 - opt_P; break;
      case 'W': 
        opt_W = 1 - opt_W;
	printf("Writing is %s.\n", opt_W?"ON":"OFF");
        if (opt_W) {
          sprintf(file_name, "%s/%04ld", directory_name, file_number);
          file = fopen(file_name, "w");
          if (!file) {
            printf("fopen failed.\n");
            exit(1);
          }
          else {
            if (P) printf("-- File %s opened\n", file_name);
          }
          rate(0);
          frames_written = 0;
          file_number++;
        }
        else {
          fclose(file);
          if (P) printf("-- File %s closed, %d frames written (total)\n",
                        file_name, frames_written);
        }
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
