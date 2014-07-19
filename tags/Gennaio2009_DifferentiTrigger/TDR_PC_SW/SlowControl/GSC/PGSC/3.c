// 3.c
//  reads and checks the files with JL3A02 STATUS

#include <time.h>
#include "/usr0/users/lebedev/AMS/Deframing.../SRDL_frames.h"

char *frame_directory = "/stage/srdl/HK/";

char event_directory[81]  = "/dats1/SRDL/EVENTS/";
char HKdat_directory[81]  = "/dats1/SRDL/HKDATA/";
char reply_directory[81]  = "/dats1/SRDL/REPLIES/";
char status_directory[81] = "/stage/STATUS/";

#define JL3_status(ind) ((AMS_block_data[4+2*ind] << 16) | AMS_block_data[5+2*ind]) 
  
//--------------------------------------------------------------------

void main( int argc, char *argv[]) {

  integer32 system_mode;
  integer32 readout_system_status, old_readout_system_status;
  integer32 JFO_ctrl_system_status, old_JFO_ctrl_system_status;
  char ascii_time[26];
  time_t command_time;

  setbuf( stdout, NULL);	// set "no buffering" for stdout
  
  HKdat_file_number = last_file_number_in( HKdat_directory);
  if (argc > 1) {
    if (atoi( argv[1]) >= 0) {
      HKdat_file_number = atoi( argv[1]);
    }
    else {
      HKdat_file_number = last_file_number_in( HKdat_directory) + atoi( argv[1]);
    }
  }
  TRACE_LEVEL = 0;
  if (argc > 2) TRACE_LEVEL = atoi( argv[2]);
  
  initialize_everything();

LOOP:
  for ( ; 
       HKdat_file_number<=last_file_number_in( HKdat_directory); 
       HKdat_file_number++) {
    if (open_AMS_HKdat_block_file( HKdat_file_number) == OK) {
      printf( "\r    %ld\r", HKdat_file_number);
      while (read_AMS_HKdat_block_file() == OK) {
        if ( ((AMS_block_ID & 0xFE00) == 0x9200) &&
             ((AMS_block_ID & 0x003F) == 0x0001) &&
             !(AMS_block_data[1] & 0xF000)) {
          command_time = JL3_status(-1);
//        printf( "%8.8X ", command_time);
  strcpy( ascii_time, (char *)(ctime(&command_time)+4));
  ascii_time[15] = '\0';
//printf( " %s\n", ascii_time);
          system_mode   = JL3_status(0);
          if (system_mode & 0x02000000) readout_system_status = JL3_status(1);
          if (system_mode & 0x01000000) JFO_ctrl_system_status = JL3_status(1);
          if (readout_system_status  != old_readout_system_status ||
              JFO_ctrl_system_status != old_JFO_ctrl_system_status) {
            printf( " %8.8X %8.8X", readout_system_status, JFO_ctrl_system_status);
            printf( " %2.2X ", readout_system_status & 0x3F);
            printf( "RD:");
            if (readout_system_status & 0x00008000) printf( " Sleep");
            else                                    printf( " .....");
            if (readout_system_status & 0x00400000) printf( " No_Time");
            else                                    printf( " .......");
            if (readout_system_status & 0x00001000) printf( " Init");
            else                                    printf( " ....");
            printf( "  JFO:");
            if (JFO_ctrl_system_status & 0x00008000) printf( " Sleep");
            else                                     printf( " .....");
            if (JFO_ctrl_system_status & 0x00400000) printf( " No_Time");
            else                                     printf( " .......");
            if (JFO_ctrl_system_status & 0x00000400) printf( " JBU");
            else                                     printf( " ...");
            if (JFO_ctrl_system_status & 0x00000200) printf( " JFO_2");
            else                                     printf( " .....");
            if (JFO_ctrl_system_status & 0x00000100) printf( " JFO_1");
            else                                     printf( " .....");
            if ((readout_system_status & 0x0000003F) == 0x00) printf( "   ");
            else if ((readout_system_status & 0x0000003F) == 0x01) printf( " R ");
                 else if ((readout_system_status & 0x0000003F) == 0x04) printf( " C ");
                      else printf( " %2.2X", readout_system_status & 0x3F);
            printf( " %8.8X", JL3_status(8));
            printf( " %8.8X", JL3_status(10));
            printf( " %s", ascii_time);
            printf( "\n");
            old_readout_system_status = readout_system_status;
            old_JFO_ctrl_system_status = JFO_ctrl_system_status;
          }
        }
      }
      close_AMS_HKdat_block_file();
    }
  }
  if (argc == 1) {
    sleep( 10);
    goto LOOP;
  }
}

