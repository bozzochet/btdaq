// file write_frames.c
//
// Reads frames from HRDL and writes them on disk.
//
// Stolen from VK (aps1.c) and reworked
//
// A.Lebedev Mar-2008...
//

#include "hrdllib.h"

bool READING_HRDL_BOARD  = TRUE;
bool WRITING_FRAME_FILES = TRUE;

int P = 0;

int file_number;

int32 frames_in_file = 0;
int32 frame_number   = 0;
int32 *frame;
int temp_file = 0;

int32 dummy[4080/4];

static int opt_L = 0;
static int opt_P = 0;

static struct timeval time_now, time_then;

static pthread_t thr_thread = 0;

static struct for_thread {
  char *frame_directory;
  int   temp_file;
  int32 file_number;
} arg;

static int c1 = 0, c2 = 0;

//~============================================================================

void adjust_scheduler(void) {

  struct sched_param sched, *sched_param = &sched;

#if 0

  if (setpriority(PRIO_PROCESS, 0, -19) < 0) {
    printf("\n*** setpriority: %s\n", strerror(errno));
    exit(1);
  }
#endif

  errno = 0;
  sched_param->sched_priority = sched_get_priority_max(SCHED_FIFO);
  if (errno) {
    printf("\n*** sched_get_priority_max: %s\n", strerror(errno));
    exit(1);
  }
  if (sched_setscheduler(0, SCHED_FIFO, sched_param) < 0) {
    printf("\n*** sched_setscheduler: %s\n", strerror(errno));
    exit(1);
  }
  if (mlockall(MCL_CURRENT) < 0) {
    printf("\n*** mlockall: %s\n", strerror(errno));
    exit(1);
  }
}

//~----------------------------------------------------------------------------

void print_warning_message(void) {
  
  if (!READING_HRDL_BOARD || !WRITING_FRAME_FILES) {
    printf("***************************************************************\n");
    printf("*                                                             *\n");
  }
  if (!READING_HRDL_BOARD) {
    printf("*           THIS VERSION DOES NOT READ HRDL BOARD             *\n");
  }
  if (!WRITING_FRAME_FILES) {
    printf("*           THIS VERSION DOES NOT WRITE FRAME FILES           *\n");
  }
  if (!READING_HRDL_BOARD || !WRITING_FRAME_FILES) {
    printf("*                                                             *\n");
    printf("***************************************************************\n");
  }
}

//~----------------------------------------------------------------------------

bool signal_is_on(int32 *rcv_sta) {

  static bool no_signal = FALSE;
  
  if (!(*rcv_sta & PMCHRDL_RX_SIGNAL)) {
    if (!no_signal) printf("%04d/%03d %s*** NO SIGNAL ***\n", 
                          file_number/1000, file_number%1000,
                          timestamp(1));
    no_signal = TRUE;
    ShortSleep(0, 1);
  }
  else {
    if (no_signal) printf("%04d/%03d %s*** SIGNAL IS BACK ***\n", 
                          file_number/1000, file_number%1000,
                          timestamp(1));
    no_signal = FALSE;
  }
  
  return (!no_signal);
}

//~----------------------------------------------------------------------------

void small_check(int32 *frame) {

  int16 myframe[4080/2];

  memcpy(myframe, (int8*)frame, 4080);
  swap16(myframe, 4080/2);
  if ((myframe[0] & 0x07FF) == 976) {
    static int16 old_seq;
    int16 new_seq, crc;
    new_seq = myframe[1] & 0x3FFF;
    if ((old_seq != 11799) &&                          // this is for tests only ***
        (new_seq != MOD(old_seq + 1, 0x4000))) {
      printf("%04d/%03d %6d", file_number/1000, file_number%1000, frames_in_file);
      printf(" *** Error: Wrong seq: was = %d, now = %d\n", old_seq, new_seq);
    }
    old_seq = new_seq;
    crc = calculate_CRC16(myframe, 4080/2);
    if (crc) {
      printf("%04d/%03d %6d", file_number/1000, file_number%1000, frames_in_file);
      printf(" *** Wrong CRC = %04X\n", crc);
    }
  }
}

//~----------------------------------------------------------------------------

void print_error_message_if_any(int32 *rcv_sta, int32 *dma_sta) {

  if ((*rcv_sta & 
      (PMCHRDL_RX_OVRRUN | PMCHRDL_RX_RESYNC | 
       PMCHRDL_RX_ILL    | PMCHRDL_RX_TRIP)) ||
      (*dma_sta & 
      (PMCHRDL_DMA_TABRT | PMCHRDL_DMA_MABRT))) {
    printf("%04d/%03d %s*** Error: ", 
           file_number/1000, file_number%1000, timestamp(1));
    if (*rcv_sta & PMCHRDL_RX_OVRRUN) printf("RX_OVRRUN ");
    if (*rcv_sta & PMCHRDL_RX_RESYNC) printf("RX_RESYNC ");
    if (*rcv_sta & PMCHRDL_RX_ILL)    printf("RX_ILL ");
    if (*rcv_sta & PMCHRDL_RX_TRIP)   printf("RX_TRIP ");
    if (*dma_sta & PMCHRDL_DMA_TABRT) printf("DMA_TABRT ");
    if (*dma_sta & PMCHRDL_DMA_MABRT) printf("DMA_MABRT ");
    printf("\n");
  }
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
  static float counter_now, counter_then = 0.0;
  float Rate;

  gettimeofday(&time_now, NULL);
  counter_now = counter;
  if (counter_then) {
    Rate = (counter_now - counter_then) / delta_t(&time_now, &time_then);
  }
  else {
    Rate = -1.0;
  }
  gettimeofday(&time_then, NULL);
  counter_then = counter;

  return Rate;
}

//~----------------------------------------------------------------------------

void user_interface(void) {

  float Rate;

  if (kbhit()) {
    int ch = toupper(fgetc(stdin));
    switch (ch) {
      case '?':
        fprintf(stderr, "%d frames written to current file so far", frames_in_file);
        Rate = rate(frame_number);
        if (Rate < 0.0) fprintf(stderr, ", to get rate press '?' again\n");
        else            fprintf(stderr, " (%.1f Mbit/sec)\n", Rate*4080.0*8.0/1000000.);
        fprintf(stderr, "c1 = %d (%f), c2 = %d(%f)\n",
                c1, (float)c1/(float)(c1+c2), c2, (float)c2/(float)(c1+c2));
        c1 = c2 = 0;
      break;
      case 'L': opt_L = 1 - opt_L; break;
      case 'P': opt_P = 1 - opt_P; break;
      case 'Q':
        exit(0);
      break;
    }
  }
}

//~----------------------------------------------------------------------------

void *the_thread(void *arg) {

  struct for_thread *p = arg;

  if (P > 1) {
    printf("the_thread:\n");
    printf("arg->frame_directory = %s\n", p->frame_directory);
    printf("arg->temp_file       = %d\n", p->temp_file);
    printf("arg->file_number     = %d\n", p->file_number);
  }
  
  rename_temp_file(p->frame_directory, p->temp_file, p->file_number);
  pthread_exit((void*)25);
}

//~============================================================================

int main(int argc, char *argv[]) {

  int k;
  char *frame_directory;
  FILE *frame_file = NULL;

  frame = dummy;
  
  setbuf(stdout, NULL);
  printf("Usage: %s <Frame Directory> [P]]\n", argv[0]);

  if (argc < 2) {
    printf("Too few parameters\n");
    exit(1);
  }

  k = 1;
  frame_directory = argv[k++];
  remove_trailing_slash(frame_directory);
  P = 0;
  if (argc > k) P                = atoi(argv[k++]);

  printf("Frame Directory = %s/\n", frame_directory);
  printf("P               = %d\n",  P);

  adjust_scheduler();

//~--

  if (READING_HRDL_BOARD) {
    if (!init_pmchrdl()) {
      printf("failed to init pmchrdl.\n");
      exit(1);
    }
  }

  if (WRITING_FRAME_FILES) {
    file_number = the_last_file_in(frame_directory);
    if (file_number < 0) {
      file_number = 0;
    }
    else {
      file_number++;
    }
  
    printf("First frame file to write  = %s/%04d/%03d\n", 
           frame_directory, file_number/1000, file_number%1000);

    frame_file = open_temp_file_for_writing(frame_directory, temp_file);
    if (!frame_file) {
      printf("open_temp_file_for_writing(1) failed.\n");
      exit(1);
    }
    printf("%04d/%03d... \n", file_number/1000, file_number%1000);
    if (P) {
      printf("File %s/%04d/%03d opened. \n", 
             frame_directory, file_number/1000, file_number%1000);
    }
    gettimeofday(&time_then, NULL);
  }

//~---

  while (1) {
    int s;
    int32 rcv_sta, dma_sta;
    if (READING_HRDL_BOARD) {
      get_frame_from_HRDL(&frame, &rcv_sta, &dma_sta);
      if (signal_is_on(&rcv_sta)) {
        if (!frame) {
          c1++;
          print_error_message_if_any(&rcv_sta, &dma_sta);
          if (rcv_sta & PMCHRDL_RX_EMPTY) ShortSleep(0, 1);
        }
        else {
          c2++;
          small_check(frame);
          if (opt_P || opt_L) print_frame(frame_number, frame);
          opt_P = 0;
          if (WRITING_FRAME_FILES) {
            s = fwrite(frame, 4080, 1, frame_file);
            if (s != 1) {
              printf("%04d/%03d %s*** Error: Disk Full\n", 
                     file_number/1000, file_number%1000, timestamp(1));
              exit(1);
            }
            frames_in_file++;
            gettimeofday(&time_now, NULL);
            if ((delta_t(&time_now, &time_then) > 60.0) ||
                (frames_in_file > 100000)) {
              fclose(frame_file);
              if (P) {
                printf("File %s/%04d/%03d closed, ", 
                       frame_directory, file_number/1000, file_number%1000);
                printf("%d frames written\n", frames_in_file);
              }
              if (P > 1) printf("wait...");
              if (thr_thread) pthread_join(thr_thread, NULL);
              if (P > 1) printf("done\n");
//            rename_temp_file(frame_directory, temp_file, file_number);
              arg.frame_directory = frame_directory;
              arg.temp_file       = temp_file;
              arg.file_number     = file_number;
              if (pthread_create(&thr_thread, NULL, the_thread, (void*)&arg)) {
                printf("Can't create thr_thread\n");
                exit(1);
              }
              temp_file = 1 - temp_file;
              file_number++;
              frame_file = open_temp_file_for_writing(frame_directory, temp_file);
              if (!frame_file) {
                printf("open_temp_file_for_writing(2) failed.\n");
                exit(1);
              }
              else {
                printf("%04d/%03d... \n", file_number/1000, file_number%1000);
                frames_in_file = 0;
                if (P) {
                  printf("File %s/%04d/%03d opened. \n", 
                         frame_directory, file_number/1000, file_number%1000);
                }
              }
              gettimeofday(&time_then, NULL);
            }
          }
          frame_number++;
        }
      }
    }
    user_interface();
  }

  return 0;
}

//~============================================================================
