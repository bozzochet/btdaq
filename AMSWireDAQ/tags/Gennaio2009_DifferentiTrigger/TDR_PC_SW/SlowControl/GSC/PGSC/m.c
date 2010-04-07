// this is file m.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer (PCMCIA 1553 interface only)
//
// - main program
//

#include "pgsc.h"

void main( int argc, char *argv[]) {
  time_t t1, t2;
  long nb_of_frames_received = 0;
  long total_nb_of_frames_received;
/*
{
  char *page[20] = {
//           1         2         3         4         5         6         7
//  1234567890123456789012345678901234567890123456789012345678901234567890
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   "This value is XXXX kilometers, and that value is ZZZZ kilograms",
   NULL };
  char *p, **pp, line[81];
  for (pp=page; *pp!=NULL; pp++) {
    strcpy( line, *pp);
    p = strstr( line, "XXXX");
    if (p != NULL) strncpy( p, "1234", 4);
    p = strstr( line, "ZZZZ");
    if (p != NULL) strncpy( p, "4321", 4);
    printf( "%s\n", line);
  }
  exit(0);
}
*/

  if (argc > 1) frame_file_number_to_start = atol( argv[1]);
  else          frame_file_number_to_start = 0;
  
  if (argc > 2) write_N_tuples = TRUE;
  else          write_N_tuples = FALSE;
  
  initialize_1553_hardware();
  setup_display();
  open_log_pipe_for_writing();

LOOP:
  total_nb_of_frames_received = 0;
  t1 = time( NULL);
  while (1) {
    if (new_1553_message()) {
      process_1553_message();
      if (new_frame_number)   nb_of_frames_received++;
      if (new_frame_number)   process_frame_number();
      if (new_time_broadcast) process_time_broadcast();
      if (new_PSP_frame)      process_PSP_frame();
      if (new_PDI_frame)      process_PDI_frame();
    }
    if (keyboard_hit = kbhit()) process_keyboard_hit();
    display_page_if_necessary();
    t2 = time( NULL);
    if (t2 - t1) {
      if (!nb_of_frames_received || (t2 - t1) > 9) break;
      total_nb_of_frames_received += nb_of_frames_received;
      nb_of_frames_received = 0;
    }
  }
  frame_rate = (float)total_nb_of_frames_received/(float)(t2-t1);
goto LOOP; 
}

