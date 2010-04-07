// 33.c
// reads and analyzes the HRDL files

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <unistd.h>

char directory[4][81] = 
  { "/dath1/HRDL/EVENTS/",
    "/dath2/HRDL/EVENTS/", 
    "/dath3/HRDL/EVENTS/", 
    "/dath4/HRDL/EVENTS/"};
    
char event_directory[81] = "/dath1/HRDL/EVENTS/";

#define integer32 unsigned int
#define integer16 unsigned short
#define TRUE  1
#define FALSE 0
#define OK   1
#define FAIL 0

int TRACE_LEVEL;
int event_disk_index;
long event_file_number;
fpos_t event_file_position;
FILE *events;
char event_path[256];
  
long last_file_number_in( char *directory_name);
int open_AMS_event_block_file( long file);
int decode_event( void);
int it_is_calibration_event( void);
int it_is_laser_event( void);
int decode_GEN_block( void);
int decode_JL1_block( void);
int decode_JL3_block( void);
int decode_CNT_block( int index);
int decode_TRA_block( int index, int data);
int process_TRA_calibration( void);
int data_inventory( int mode);
int new_data_inventory( int mode);
void ctrl_C_handler( void);
void exit_handler( void);

int CNT_index[8] = {  0,  1, 2,  3,  4, 5,  6,  7};
int TRA_index[8] = { -1, -1, 0, -1, -1, 1, -1, -1};

#define GEN_node_type 0x1
#define JL1_node_type 0x2
#define JL3_node_type 0x9
#define CNT_node_type 0xA
#define TRA_node_type 0xB

struct {
  int GEN_blocks;
  int JL1_blocks;
  int JL3_blocks;
  int CNT_blocks[8];
  int TRA_blocks[2][32];
} nb_of;

FILE *database;

integer32 run_ID;
integer16 run_type, run_tag;
integer32 event_number;
time_t event_time;
char ascii_event_time[26];

int it_is_bad_event;

struct {
  integer32 ID;
  integer16 type;
  integer16 tag;
  integer32 nb_of_events;
  integer32 nb_of_bad_events;
  struct {
    integer32 number;
    time_t time;
    int    disk_index;
    long   file_number;
    fpos_t file_position;
  } first_event;
  struct {
    integer32 number;
    time_t time;
    int    disk_index;
    long   file_number;
    fpos_t file_position;
  } last_event;
} run[1000];

int indeX = 0;

//
//----- AMS block data structure description ----------------
//  (it is so funny to comply with MicroSoft C restrictions)
//

#define MAX_AMS_BLOCK_LENGTH 4194303     // 16-bit words

integer32 actual_AMS_block_length;
integer16 AMS_block[MAX_AMS_BLOCK_LENGTH+1];

#define           AMS_block_length  AMS_block[0]
#define           AMS_block_ID      AMS_block[1]
static integer16 *AMS_block_data = &AMS_block[1];
static integer16 *AMS_block_body = &AMS_block[2];
    
integer16 block_length, block_ID;
int block_type, node_type, node_number, data_type;

integer16 *GEN_block;
integer16 *JL1_block;
integer16 *JL3_block;
integer16 *CNT_block[8];
integer16 *TRA_block[2][32];

//--------------------------------------------------------------------

long last_file_number_in( char *directory_name) {

  long file_number;
  long last_file_number;

  DIR  *dirp;
  struct dirent *dp;
  

  if (TRACE_LEVEL > 1) printf( "scan directory %s\n", directory_name);
  if ((dirp = opendir( directory_name)) == NULL) {
    printf( "opendir(%s) : %s\n", directory_name, strerror( errno));
    exit( 0);
  }
  last_file_number = 0;
  while ((dp = readdir( dirp)) != NULL) {
    if (fnmatch( "[0-9][0-9][0-9][0-9][0-9][0-9]", dp->d_name, 0) == 0) {
      file_number = atol( dp->d_name);
      if (file_number > last_file_number) last_file_number = file_number;
    }
  }
  if (closedir( dirp)) {
    printf( "closedir(%s) : %s\n", directory_name, strerror( errno));
    exit( 0);
  }

  return last_file_number;
}

//--------------------------------------------------------------------

int open_AMS_event_block_file( long file) {

  extern char event_directory[];
  
  sprintf( event_path, "%s%6.6d", event_directory, file);

  if ((events = fopen( event_path, "rb")) == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "fopen(%s,\"rb\") : %s\n", event_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open 'rb' %s\n", event_path);
    fsetpos( events, &event_file_position);
    return OK;
  }
}

//--------------------------------------------------------------------

int read_AMS_event_block_file( void) {
  fgetpos( events, &event_file_position);
  if (fread( &AMS_block[0], sizeof( integer16), 2, events) != 2) {
    return FAIL;
  }
  actual_AMS_block_length = AMS_block_length;
  if ((AMS_block_ID & 0xFFC0) == 0) {
    actual_AMS_block_length |= (AMS_block_ID & 0x003F) << 16;
  }
  if (fread( &AMS_block[2],
             sizeof( integer16), 
             actual_AMS_block_length-1, 
             events) 
      != actual_AMS_block_length-1) {
    return FAIL;
  }
  return OK;
}

//--------------------------------------------------------------------

int close_AMS_event_block_file( void) {
  if (fclose( events)) {
    if (TRACE_LEVEL > 0) {
      printf( "fclose(%s) : %s\n", event_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", event_path);
    event_file_position = 0;
    return OK;
  }
}

//====================================================================

void main( int argc, char *argv[]) {

  int i, j;

  setbuf( stdout, NULL);	// set "no buffering" for stdout
  atexit( exit_handler);
  signal( SIGINT, ctrl_C_handler);
  
  TRACE_LEVEL = 0;

  if (argc > 1) event_file_number = atol( argv[1]);
  
  if ((database = fopen( "database", "rb")) != NULL) {
    printf( "DataBase found and read-in!\n");
    fread( run, sizeof( run), 1, database);
    fclose( database);
    for (i=0; i<1000; i++) {
      if (run[i].ID == 0) break;
      event_disk_index    = run[i].first_event.disk_index;
      event_file_number   = run[i].first_event.file_number;
      event_file_position = run[i].first_event.file_position;
      indeX = i;
    }
    printf( "indeX = %d\n", indeX);
  }
  else {
    printf( "No DataBase found!\n");
  }
//sprintf( event_directory, directory[event_disk_index]);

LOOP:
  for ( ; 
       event_file_number <= last_file_number_in( event_directory); 
       event_file_number++) {
    if (open_AMS_event_block_file( event_file_number) == OK) {
      printf( "\r    %ld\r", event_file_number);
      while (read_AMS_event_block_file() == OK) {
        static long counter=0;
        counter++;
        if (!(counter % 1000)) {
          printf( "\r %12ld    %ld\r", counter, event_file_number);
        }
        decode_event();
        process_TRA_calibration();
        data_inventory( 0);
        new_data_inventory( 0);
      }
      close_AMS_event_block_file();
      if ((database = fopen( "database", "wb")) != NULL) {
        fwrite( run, sizeof( run), 1, database);
        fclose( database);
      }
    }
  }
  exit( 0);
  if (argc < 3) {
    sleep( 5);
    goto LOOP;
  }
}

//====================================================================

void ctrl_C_handler( void) {
  exit( 0);
}

//====================================================================

void exit_handler( void) {
  data_inventory( 1);
  new_data_inventory( 1);
  printf( "\n");
}

//====================================================================

int decode_event( void) {
  
  int i, j, k;
  
  it_is_bad_event = FALSE;
  bzero( &nb_of, sizeof( nb_of));
  block_length = actual_AMS_block_length;
  if ((AMS_block_ID & 0xFFC0) == 0) block_ID = 0;
  else                              block_ID = AMS_block_ID;
  if (block_ID != 0) {
    it_is_bad_event = TRUE;
    printf( "*** EVT: wrong block_ID = %4.4X\n", block_ID);
  }
  if (TRACE_LEVEL > 10) {
    printf( "  %12d %4.4X ", block_length, block_ID);
  }
  k = 2;
  while (1) {
    if (k > actual_AMS_block_length) break;
    block_length = AMS_block[k];
    block_ID     = AMS_block[k+1];
    if (TRACE_LEVEL > 10) printf( " %6d %4.4X", block_length, block_ID);
    block_type  = (block_ID >> 13) & 0x07;
    node_type   = (block_ID >>  9) & 0x0F;
    node_number = (block_ID >>  6) & 0x07;
    data_type   =  block_ID        & 0x3F;
    if (block_type) {
      it_is_bad_event = TRUE;
      printf( "*** TYP: wrong block_ID = %4.4X\n", block_ID);
    }
    switch (node_type) {
    case GEN_node_type:
      if (node_number != 0) {
        it_is_bad_event = TRUE;
        printf( "*** GEN: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
          it_is_bad_event = TRUE;
          printf( "*** GEN: wrong block_ID = %4.4X\n", block_ID);
        }
        else {
          nb_of.GEN_blocks++;
          if (nb_of.GEN_blocks == 1) {
            GEN_block = &AMS_block[k];
          }
        }
      }
      break;
    case JL1_node_type:
      if (node_number != 1) {
        it_is_bad_event = TRUE;
        printf( "*** JL1: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
          it_is_bad_event = TRUE;
          printf( "*** JL1: wrong block_ID = %4.4X\n", block_ID);
        }
        else {
          nb_of.JL1_blocks++;
          if (nb_of.JL1_blocks == 1) {
            JL1_block = &AMS_block[k];
          }
        }
      }
      break;
    case JL3_node_type:
      if (node_number != 0) {
        it_is_bad_event = TRUE;
        printf( "*** JL3: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
          it_is_bad_event = TRUE;
          printf( "*** JL3: wrong block_ID = %4.4X\n", block_ID);
        }
        else {
          nb_of.JL3_blocks++;
          if (nb_of.JL3_blocks == 1) {
            JL3_block = &AMS_block[k];
          }
        }
      }
      break;
    case CNT_node_type:
      if (data_type != 1) {
        it_is_bad_event = TRUE;
        printf( "*** CNT: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        i = CNT_index[node_number];
        nb_of.CNT_blocks[i]++;
        if (nb_of.CNT_blocks[i] == 1) {
          CNT_block[i] = &AMS_block[k];
        }
      }
      break;
    case TRA_node_type:
      if (data_type != 0 & 
          data_type != 2 & 
          data_type != 3 & 
          data_type != 4 & 
          data_type != 5 & 
          data_type != 6) {
        it_is_bad_event = TRUE;
        printf( "*** TRK: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        i = TRA_index[node_number];
        if (i < 0) {
          it_is_bad_event = TRUE;
          printf( "*** TRK: wrong block_ID = %4.4X\n", block_ID);
        }
        else {
          nb_of.TRA_blocks[i][data_type]++;
          if (nb_of.TRA_blocks[i][data_type] == 1) {
            TRA_block[i][data_type] = &AMS_block[k];
          }
        }
      }
      break;
    default:
      it_is_bad_event = TRUE;
      printf( "*** NOD: wrong block_ID = %4.4X\n", block_ID);
      break;
    }
    k = k + block_length + 1;
  }
  if (k != actual_AMS_block_length + 1) {
    it_is_bad_event = TRUE;
    printf( "*** wrong event block structure\n");
  }
  else {
    if (TRACE_LEVEL > 10) printf( "  \n");
  }

//-------

  if (nb_of.GEN_blocks == 1) {
    decode_GEN_block();
  }
  else {
    it_is_bad_event = TRUE;
    printf( "*** wrong number of GEN blocks: %d(1)\n", nb_of.GEN_blocks);
  }
  if (it_is_calibration_event()) {
    return;
  }
  if (it_is_laser_event()) {
    return;
  }
  if (run_type != 0x1F) {
    it_is_bad_event = TRUE;
    printf( "*** wrong run type: %02X(1F)\n", run_type);
  }
  if (nb_of.JL1_blocks == 1) {
    decode_JL1_block();
  }
  else {
    it_is_bad_event = TRUE;
    printf( "*** wrong number of JL1 blocks: %d(1)\n", nb_of.JL1_blocks);
  }
  if (nb_of.JL3_blocks == 1) {
    decode_JL3_block();
  }
  else {
    it_is_bad_event = TRUE;
    printf( "*** wrong number of JL3 blocks: %d(1)\n", nb_of.JL3_blocks);
  }
  for (i=0; i<8; i++) {
    if (nb_of.CNT_blocks[i] == 1) {
      decode_CNT_block( i);
    }
    else {
      it_is_bad_event = TRUE;
      printf( "*** wrong number of CNT[%d] blocks: %d(1)\n", i, nb_of.CNT_blocks[i]);
    }
  }
  for (i=0; i<2; i++) {
    if (nb_of.TRA_blocks[i][0] == 1) {
      decode_TRA_block( i, j);
    }
    else {
      it_is_bad_event = TRUE;
      printf( "*** wrong number of TRA[%d][0] blocks: %d(1)\n", i, nb_of.TRA_blocks[i][0]);
    }
    for (j=1; j<32; j++) {
      if (nb_of.TRA_blocks[i][j] != 0) {
        it_is_bad_event = TRUE;
        printf( "*** wrong number of TRA[%d][%d] blocks: %d(0)\n", i, j, nb_of.TRA_blocks[i][j]);
      }
    }
  }
/*
{//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static FILE *file = NULL;
  if (file == NULL) file = fopen( "spy", "w");
  if (file != NULL) 
      fprintf( file, "%d\n", actual_AMS_block_length);
}//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
}

//--------------------------------------------------------------------

int it_is_calibration_event( void) {

  int i, j;
  
  if (it_is_bad_event) return FALSE;
  
  if (run_type != 0x1F) return FALSE;
  
  if (nb_of.GEN_blocks != 1) return FALSE;
  if (nb_of.JL1_blocks != 0) return FALSE;
  if (nb_of.JL3_blocks != 0) return FALSE;
  
  for (i=0; i<8; i++) if (nb_of.CNT_blocks[0] != 0) return FALSE;
  
  for (i=0; i<2; i++) {
    for (j=0; j<32; j++) {
      if (j == (event_number + 1) && nb_of.TRA_blocks[i][j] != 1) return FALSE; 
      if (j != (event_number + 1) && nb_of.TRA_blocks[i][j] != 0) return FALSE; 
    }
  }

  return TRUE;
}

//--------------------------------------------------------------------

int it_is_laser_event( void) {

  if (it_is_bad_event) return FALSE;

  if (run_type != 0x07) return FALSE;
  
  return TRUE;

}

//--------------------------------------------------------------------

int decode_GEN_block( void) {
  int i;
  static integer32 old_run_ID;
  
  run_ID   = (GEN_block[2] << 16) | GEN_block[3];
  run_type = GEN_block[4];
  run_tag  = GEN_block[5];
  event_number = (GEN_block[6] << 16) | GEN_block[7];
  event_time   = (GEN_block[8] << 16) | GEN_block[9];
  
  if (old_run_ID != run_ID) {
    if (TRACE_LEVEL > 5) printf( "RUN: %8.8X    TYPE: %4.4X\n", run_ID, run_type);
    old_run_ID = run_ID;
  }
  
  strcpy( ascii_event_time, (char *)(ctime(&event_time)+4));
  ascii_event_time[15] = '\0';
  
  if (TRACE_LEVEL > 5) {
    printf( "GEN: ");
    for (i=1; i<=GEN_block[0]; i++) printf( "%4.4X ", GEN_block[i]);
    printf( "\n");
  }
  
  if (TRACE_LEVEL > 2) {
    printf( "GEN: ");
    printf( "run_ID=%8.8X ", run_ID);
    printf( "run_type=%4.4X ", run_type);
    printf( "run_tag=%4.4X ", run_tag);
    printf( "event_number=%d ", event_number);
    printf( "%s ", ascii_event_time);
    printf( "\n");
  }
}

//--------------------------------------------------------------------

int decode_JL1_block( void) {
  int i;
  if (TRACE_LEVEL > 5) {
    printf( "JL1: ");
    for (i=1; i<=JL1_block[0]; i++) printf( "%4.4X ", JL1_block[i]);
    printf( "\n");
  }
}

//--------------------------------------------------------------------

int decode_JL3_block( void) {
  int i;
  if (TRACE_LEVEL > 5) {
    printf( "JL3: ");
    for (i=1; i<=JL3_block[0]; i++) printf( "%4.4X ", JL3_block[i]);
    printf( "\n");
  }
}

//--------------------------------------------------------------------

int decode_CNT_block( int index) {
  int i;
  if (TRACE_LEVEL > 5) {
    printf( "CNT%d: ", index);
    for (i=1; i<=CNT_block[index][0]; i++) printf( "%4.4X ", CNT_block[index][i]);
    printf( "\n");
  }
}

//--------------------------------------------------------------------

int decode_TRA_block( int index, int data) {
  int i;
  if (TRACE_LEVEL > 5) {
    printf( "TRA%d: ", index);
    for (i=1; i<=TRA_block[index][data][0]; i++) printf( "%4.4X ", TRA_block[index][data][i]);
    printf( "\n");
  }
}

//--------------------------------------------------------------------

int process_TRA_calibration( void) {
  int i, j, min, sec;
  static int on = 0;
  static integer32 old_run_ID;
  static long old_event_file_number;
  static time_t old_event_time;
  char c;
return;  
  for (i=0; i<2; i++) {
    for (j=1; j<32; j++) {
      if (nb_of.TRA_blocks[i][j] == 1) {
        c = ' ';
        if (!on) {
          on = 1;
          min = sec = 0;
          if (old_event_time) {
            min = (event_time - old_event_time) / 60;
            sec = (event_time - old_event_time) % 60;
          }
          printf( "%s %3d.%02d: ", ascii_event_time, min, sec);
          printf( "%06d ", event_file_number);
          printf( "%8.8X  ", run_ID);
          old_run_ID = run_ID;
          old_event_file_number = event_file_number;
          old_event_time        = event_time;
        }
        else {
          if (old_run_ID != run_ID) c = '?';
        }
        printf( "%4.4X%c ", TRA_block[i][j][1], c);
      }
    }
  }
  for (i=0; i<2; i++) {
    for (j=1; j<32; j++) {
      if (nb_of.TRA_blocks[i][j]) return;
    }
  }  
  if (on) {
    on = 0;
    if (old_event_file_number != event_file_number) printf( "* ");
    else                                            printf( "  ");
    printf( "-> %8.8X ", run_ID);
    printf( "\n");
  }
}

//--------------------------------------------------------------------

int data_inventory( int mode) {
  int i, min, sec;
  char c;
  static integer32 run_ID_table[1000];
  static int run_ID_table_size, run_ID_table_index;
  static time_t total_time;
  static nb_of_events, nb_of_bad_events;
  static total_nb_of_events, total_nb_of_bad_events;
  static integer32 current_run_ID;
  static integer16 current_run_type;
  static integer32 first_event_file_number, first_event_number;
  static integer32 last_event_file_number, last_event_number;
  static time_t first_event_time, last_event_time;
  char first_ascii_time[26], last_ascii_time[26];

  if (run_ID != current_run_ID || event_number < last_event_number || mode) {
    if (current_run_ID) {
      char c = ' ';
      if (run_ID_table_index >= 0) c = '*';
      strcpy( first_ascii_time, (char *)(ctime(&first_event_time)+4));
      first_ascii_time[15] = '\0';
      strcpy( last_ascii_time, (char *)(ctime(&last_event_time)+11));
      last_ascii_time[8] = '\0';
      min = (last_event_time - first_event_time) / 60;
      sec = (last_event_time - first_event_time) % 60;
      total_time += last_event_time - first_event_time;
      printf( "%8.8X%c %2.2X %8d %8d %8d (%d)   %s-%s  %3d.%02d %06d..%06d \n",
               current_run_ID, c, current_run_type,
               first_event_number, last_event_number,
               nb_of_events, nb_of_bad_events,
               first_ascii_time, last_ascii_time,
               min, sec,
               first_event_file_number, last_event_file_number);
    }
    current_run_ID = run_ID;
    run_ID_table_index = -1;
    for (i=0; i<run_ID_table_size; i++) if (run_ID == run_ID_table[i]) run_ID_table_index = i;
    if (run_ID_table_index < 0) run_ID_table[run_ID_table_size++] = run_ID;
    current_run_type        = run_type;
    first_event_file_number = event_file_number;
    first_event_number      = event_number;
    first_event_time        = event_time;
    total_nb_of_events     += nb_of_events;
    total_nb_of_bad_events += nb_of_bad_events;
    nb_of_events            = 0;
    nb_of_bad_events        = 0;
  }
  
  if (mode) {
    printf( "Total nb. of events = %d(%d)\n", 
        total_nb_of_events, total_nb_of_bad_events);
    printf( "Total time = %d mins\n", ((int) total_time + 30) / 60);
  }
  
  last_event_file_number = event_file_number;
  last_event_number      = event_number;
  last_event_time        = event_time;
  nb_of_events++;
  if (it_is_bad_event) nb_of_bad_events++;
}

//--------------------------------------------------------------------

int new_data_inventory( int mode) {

  int i, min, sec;
  char ascii_time[26];
  static integer32 old_run_ID;
  static integer32 old_event_number;
  static time_t    old_event_time;
  static long      old_event_file_number;
  static fpos_t    old_event_file_position;

  if (run_ID != old_run_ID || event_number < old_event_number || mode) {
    if (old_run_ID) {
      run[indeX].last_event.number        = old_event_number;
      run[indeX].last_event.time          = old_event_time;
      run[indeX].last_event.disk_index    = event_disk_index;
      run[indeX].last_event.file_number   = old_event_file_number;
      run[indeX].last_event.file_position = old_event_file_position;
      indeX++;
    }
    if (mode == 0) {
      run[indeX].ID   = run_ID;
      run[indeX].type = run_type;
      run[indeX].tag  = run_tag;
      run[indeX].nb_of_events     = 0;
      run[indeX].nb_of_bad_events = 0;
      run[indeX].first_event.number        = event_number;
      run[indeX].first_event.time          = event_time;
      run[indeX].first_event.disk_index    = event_disk_index;
      run[indeX].first_event.file_number   = event_file_number;
      run[indeX].first_event.file_position = event_file_position;
    }
  }
  
  if (mode) {
    long   total_nb_of_events     = 0;
    long   total_nb_of_bad_events = 0;
    time_t total_time             = 0;
    printf( "----------------------------------------------------------------------------------------------------------\n");
    for (i=0; i<1000; i++) {
      char c = ' ';
      if (run[i].ID == 0) break;
      total_nb_of_events     += run[i].nb_of_events;
      total_nb_of_bad_events += run[i].nb_of_bad_events;
      total_time += run[i].last_event.time - run[i].first_event.time;
      if (i > 0) if (run[i].ID <= run[i-1].ID) c = '*';
      printf( "%8.8X = %09d%c ", run[i].ID, run[i].ID, c);
      printf( "%2.2X ", run[i].type);
      printf( "%8d %8d ", run[i].first_event.number, run[i].last_event.number);
      printf( "%8d (%d)   ", run[i].nb_of_events, run[i].nb_of_bad_events);
      strcpy( ascii_time, (char *)(ctime(&run[i].first_event.time)+4));
      ascii_time[15] = '\0';
      printf( "%s-", ascii_time);
      strcpy( ascii_time, (char *)(ctime(&run[i].last_event.time)+11));
      ascii_time[8] = '\0';
      printf( "%s ", ascii_time);
      min = (run[i].last_event.time - run[i].first_event.time) / 60;
      sec = (run[i].last_event.time - run[i].first_event.time) % 60;
      printf( "%3d:%02d ", min, sec);
      printf( "%d ", run[i].first_event.disk_index);
      printf( "%06d..", run[i].first_event.file_number);
      printf( "%06d ", run[i].last_event.file_number);
      printf( "\n");
    }
    printf( "----------------------------------------------------------------------------------------------------------\n");
    printf( "Total nb. of events = %d (%d)\n",
        total_nb_of_events, total_nb_of_bad_events);
    printf( "Total time = %d mins\n", ((int) total_time + 30) / 60);
    if ((database = fopen( "database", "wb")) != NULL) {
      fwrite( run, sizeof( run), 1, database);
      fclose( database);
    }
    else {
    }
    return;
  }
  
  old_run_ID              = run_ID;
  old_event_number        = event_number;
  old_event_time          = event_time;
  old_event_file_number   = event_file_number;
  old_event_file_position = event_file_position;
  run[indeX].nb_of_events++;
  if (it_is_bad_event) run[indeX].nb_of_bad_events++;
}

//--------------------------------------------------------------------
