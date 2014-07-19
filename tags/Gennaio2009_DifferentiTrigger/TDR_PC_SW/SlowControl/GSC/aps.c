// file aps.c
//
// Example program for VK (disk writing for ISIL APS tests)
// (note: this program does not work at all with HRDL board)
//
// A.Lebedev Feb-2005...
//

#include "mylib.h"

int P = 0;

char *directory_name = {"FRAMES"};

char file_name[80] = {""};
long file_number;
int32 frame_number;
int32 frames_written;
int32 frame[1024];

int opt_L = 0;
int opt_P = 0;
int opt_W = 0;

FILE *file;

//~============================================================================

long last_file_number_in(char *DirPath) {

  long file_number;
  long last_file_number;
  DIR  *dir;
  struct dirent *d;

  if (P) printf( "Start scan of directory:  %s\n", DirPath);
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
  if (P) printf("Last file number found in %s is %04ld\n", DirPath, last_file_number);
  
  return last_file_number;
}

//~----------------------------------------------------------------------------

bool get_frame(int32 *frame) {

  int i;
  
  for (i=0; i<1024; i++) {
    frame[i] = 0x012345678;
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

void print_frame(int32 frame_number, int32 *frame) {

  int i, N = 16;

  printf("Frame %d\n", frame_number);

  for (i=0; i<1024; i++) {
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

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  printf(
  "Usage: %s [<Debug Printout Level>]\n", argv[0]);
  if (argc > 1) P = atoi(argv[1]);
  printf("P       = %d\n", P);

  file_number = last_file_number_in(directory_name);
  if (file_number < 0) {
    printf("last_file_number_in failed.\n");
    exit(1);
  }
  file_number++;
  
  while (1) {
    if (get_frame(frame)) {
      if (opt_P || opt_L) print_frame(frame_number, frame);
      opt_P = 0;
      if (opt_W) {
        fwrite(frame, sizeof(frame), 1, file);
        frames_written++;
      }
      frame_number++;
    }
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
      case '?':
        printf("frame number %d\n", frame_number);
        if (opt_W) {
          printf("writing file %s", file_name);
          printf(", %d frames written so far", frames_written);
          printf(" (%.0f frames/sec)\n", rate(frames_written));
        }
        else {
          printf("not writing");
          if (strlen(file_name)) {
            printf(", last file was %s with %d frames written\n",
                      file_name, frames_written);
          }
          else {
            printf("\n");
          }
        }
        break;
      case 'L': opt_L = 1 - opt_L; break;
      case 'P': opt_P = 1 - opt_P; break;
      case 'W': 
        opt_W = 1 - opt_W;
        if (opt_W) {
          sprintf(file_name, "%s/%04ld", directory_name, file_number);
          file = fopen(file_name, "w");
          if (!file) {
            printf("fopen failed.\n");
            exit(1);
          }
          else {
            if (P) printf("file %s opened\n", file_name);
          }
          rate(0);
          frames_written = 0;
          file_number++;
        }
        else {
          fclose(file);
          if (P) printf("file %s closed, %d frames written\n",
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
