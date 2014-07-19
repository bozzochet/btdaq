// file read_frames.c
//
// Reads frames from disk to check write_frames program,
//
// A.Lebedev Apr-2008...
//

#include "gsclib.h"

int P = 0;

int   file_number;
int32 frame_number;
int16 frame[4080/2];

//~============================================================================

void print_frame(int32 frame_number, int32 *frame) {

  int i, N = 16;

  printf("Frame %d   %08x\n", frame_number, (int32)frame);

  for (i=0; i<64; i++) {
    if (( i    % N) == 0) printf("%4d: ", i);
    printf("%08X ", frame[i]);
    if (((i+1) % N) == 0) printf("\n");
  }

}

//~============================================================================

int main(int argc, char *argv[]) {

  int k, s;
  char *frame_directory;
  FILE *frame_file = NULL;

  setbuf(stdout, NULL);
  printf("Usage: %s <Frame Directory> <File Number> [<Frame Number> [P]]\n", argv[0]);

  if (argc < 3) {
    printf("Too few parameters\n");
    exit(1);
  }

  k = 1;
  frame_directory = argv[k++];
  remove_trailing_slash(frame_directory);
  frame_number = 0xFFFFFFFF;
  P = 0;
  if (argc > k) file_number  = atoi(argv[k++]);
  if (argc > k) frame_number = atoi(argv[k++]);
  if (argc > k) P            = atoi(argv[k++]);

  printf("Frame Directory = %s/\n", frame_directory);
  printf("File Number     = %04d/%03d\n", file_number/1000, file_number%1000);
  printf("Frame Number    = %d\n", frame_number);
  printf("P               = %d\n",  P);

//~--

  frame_file = open_file_for_reading(frame_directory, file_number);
  if (!frame_file) {
    printf("open_file_for_reading failed.\n");
    exit(1);
  }

  if (frame_number == 0xFFFFFFFF) {
    frame_number = 0;
    while (1) {
      s = fread(frame, 2, 4080/2, frame_file);
      if (s != 4080/2) {
        printf(" %6d, s = %d\n", frame_number, s);
        exit(0);
      }
      swap16(frame, 4080/2);
      if ((frame[0] & 0x07FF) == 976) {
        int16 crc;
        crc = calculate_CRC16(frame, 4080/2);
        if (crc) printf(" %6d CRC = %04X\n", frame_number, crc);
      }
      frame_number++;
    }
  }
  
//~--

  if (fseek(frame_file, 4080*frame_number, SEEK_SET)) {
    printf("fseek failed %s\n", strerror(errno));
    exit(1);
  }

  s = fread(frame, 2, 4080/2, frame_file);
  if (s != 4080/2) {
    printf("s = %d\n", s);
    exit(1);
  }

  swap16(frame, 4080/2);
  printf("apid = %d\n", frame[0] & 0x07FF);
  if ((frame[0] & 0x07FF) == 976) {
    int16 crc;
    crc = calculate_CRC16(frame, 4080/2);
    printf(" CRC = %04X\n", crc);
  }

  return 0;
}

//~============================================================================
