// test-3.c
// tests of deframing program for HRDL link
// reads and checks ALL event files

#include "HRDL_frames.h"

char *host = NULL;
char *user = "hropr";
char *pass = "hi!rate?";

char *frame_directory = "/home/hropr/data/";
char *remote_frame_directory = "/home/hropr/data/";
char *local_frame_directory = "/home/hropr/data/";

char *event_directory = "/dath1/HRDL/EVENTS/";
char *HKdat_directory = "/dath1/HRDL/HKDATA/";
char *reply_directory = "/dath1/HRDL/REPLIES/";

//char *event_directory = "/stage/hrdl/EVENTS/";
//char *HKdat_directory = "/stage/hrdl/HKDATA/";
//char *reply_directory = "/stage/hrdl/REPLIES/";

//-------------------------------------------------------------------

void main( int argc, char *argv[]) {

  long nev;

  event_file_number = 9770;
  if (argc > 1) event_file_number = atoi( argv[1]);
  TRACE_LEVEL = 0;
  if (argc > 2) TRACE_LEVEL = atoi( argv[2]);
  
  initialize_everything();

  printf("\nStart event file reading...\n\n");

  for ( ; 
       event_file_number<=last_file_number_in( event_directory); 
       event_file_number++) {
    if (open_AMS_event_block_file( event_file_number) == OK) {
      while (read_AMS_event_block_file() == OK) {
        print_AMS_block();
      }
      close_AMS_event_block_file();
      printf("***********************************************************\n");
    }
  }
}

