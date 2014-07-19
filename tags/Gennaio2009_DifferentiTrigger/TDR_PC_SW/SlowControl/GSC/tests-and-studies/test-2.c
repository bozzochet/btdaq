// file nettoyer.c
//
// reads frame files and deframes them, looking for "empty" frame files
//

#include "HRDL_frames.h"

char *host = "pc1.hrdl";
//char *host = NULL;
char *user = "hropr";
char *pass = "hi!rate?";

char frame_directory[256];
char *remote_frame_directory = "/home/hropr/data/"; 
char *local_frame_directory  = "/dath1/HRDL/FRAMES/"; 

//char *event_directory = "/stage/hrdl/EVENTS/";
//char *HKdat_directory = "/stage/hrdl/HKDATA/";
//char *reply_directory = "/stage/hrdl/REPLIES/";

char *event_directory = "/dath1/HRDL/EVENTS/";
char *HKdat_directory = "/dath1/HRDL/HKDATA/";
char *reply_directory = "/dath1/HRDL/REPLIES/";

char *log_pipe_file_name = "/tmp/HRDL_deframing_log.pipe";

//------------------------------------------------------------------------------

void main( int argc, char *argv[]) {

  long file;
  clock_t t1, t2;
  time_t T1, T2;
  FILE* list;
    
  setbuf( stdout, NULL);	// set "no buffering" for stdout

  frame_file_number = 0;
  if (argc > 1) frame_file_number = atoi( argv[1]);

  TRACE_LEVEL = 0;
  if (argc > 2) TRACE_LEVEL = atoi( argv[2]);
  if (TRACE_LEVEL > 9) TRACE_LEVEL = 9;

  printf("\n\t**************************************************************\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                 A.Lebedev  07-Dec-97 14:39                 *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*        HIGH RATE DATA LINK = HRDL DEFRAMING PROGRAM        *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                       version 0.2b                         *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t**************************************************************\n\n");

  initialize_everything();
  
  list = fopen( "killer", "w");
  setbuf( list, NULL);
  
  if (frame_file_number == 0) frame_file_number = 1;
  do_not_write_AMS_block_file_PLS = TRUE;
  
  printf( "\nFirst frame file to read   = %s%6.6ld\n\n",  frame_directory, frame_file_number);

  for ( ;
       frame_file_number <= last_HRDL_frame_file_number();
       frame_file_number++) {
       
    t1 = clock();
    T1 = time( NULL);
    
    if (open_HRDL_frame_file( frame_file_number) == OK) {
      bzero( &nb_of, sizeof( nb_of));
      while (read_HRDL_frame_file() == OK);
      close_HRDL_frame_file();
      print_HRDL_deframing_log();
      if (nb_of.data.frames.read == 0) fprintf( list, "rm -f %6.6d\n", frame_file_number);
    }

    t2 = clock();
    T2 = time( NULL);
    if (TRACE_LEVEL > 0) {
      if (t2 != t1) {
        printf( "Deframing rate = %.2f Mbit/sec (CPU time)", 
                4080.0 * 8.0 / 1000000.0 * 
                (nb_of.data.frames.read + nb_of.SW_fill.frames.read + nb_of.HW_fill.frames.read) / 
                ((float)(t2-t1)/CLOCKS_PER_SEC));
        if (T2 != T1) {
          printf( " = %.2f Mbit/sec (real time)\n", 
                  4080.0 * 8.0 / 1000000.0 *
                  (nb_of.data.frames.read + nb_of.SW_fill.frames.read + nb_of.HW_fill.frames.read) / 
                  (float)(T2-T1));
        }
        else {
          printf( "\n");
        }
      }
    }
  }
}

