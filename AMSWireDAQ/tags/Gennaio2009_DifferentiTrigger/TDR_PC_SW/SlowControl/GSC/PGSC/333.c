// 333.c
// reads and analyzes the HRDL files - nonrepeated event number

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

char event_directory[81];
char event_path[256];
FILE *events;

#define integer32 unsigned int
#define integer16 unsigned short
#define TRUE  1
#define FALSE 0
#define OK   1
#define FAIL 0

int TRACE_LEVEL;

int    event_disk_index;
long   event_file_number;
fpos_t event_file_position;
  
long last_file_number_in( char *directory_name);
int open_AMS_event_block_file( long file);
int decode_event( void);
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

struct {
  integer32 ID;
  integer16 type;
  integer16 tag;
  integer32 nb_of_events;
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

integer32 EVENTS[1000000];
struct {
  integer32 ID;
  integer16 type;
  integer32 nb_of_events;
} RUN[1000];

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

  int i, j, k;
  integer32 nb_of_events;

  setbuf( stdout, NULL);	// set "no buffering" for stdout
  atexit( exit_handler);
  signal( SIGINT, ctrl_C_handler);
  
  TRACE_LEVEL = 0;

  if ((database = fopen( "database", "rb")) == NULL) {
    printf( "DataBase unreachable!\n");
    exit( 0);
  }
  else {
    fread( run, sizeof( run), 1, database);
    fclose( database);
  }

  if ((database = fopen( "DATABASE", "rb")) == NULL) {
    printf( "DataBase unreachable!!\n");
  }
  else {
    fread( RUN, sizeof( RUN), 1, database);
    fclose( database);
  }
  for (i=0; i<1000; i++) {
    if (RUN[i].ID == 0) {
      if (i > 0) RUN[i-1].ID =0;
      break;
    }
  }

  for (i=0; i<1000; i++) {
    if (run[i].ID == 0) break;
    for (k=0; k<1000; k++) {
      if (RUN[k].ID == 0) break;
      if (RUN[k].ID == run[i].ID) {
        k = -1;
        break;
      }
    }
    if (k < 0) continue;
    printf( "%3d RUN: %8.8X:: ", k+1, run[i].ID);
    RUN[k].ID   = run[i].ID;
    RUN[k].type = run[i].type;
    bzero( EVENTS, sizeof( EVENTS));
    for (j=0; j<1000; j++) {
      if (run[j].ID == 0) break;
      if (run[j].ID == run[i].ID) {
        printf( "%3d ", j);
        nb_of_events = 0;
        event_disk_index    = run[j].first_event.disk_index;
        sprintf( event_directory, directory[event_disk_index]);
        event_file_number   = run[j].first_event.file_number;
        event_file_position = run[j].first_event.file_position;
        for ( ;
              event_file_number <= run[j].last_event.file_number; 
              event_file_number++) {
          if (open_AMS_event_block_file( event_file_number) == OK) {
            while (read_AMS_event_block_file() == OK) {
              decode_event();
              nb_of_events++;
              EVENTS[event_number]++;
              if (event_file_number   == run[j].last_event.file_number &&
                  event_file_position == run[j].last_event.file_position) break;
            }
            close_AMS_event_block_file();
          }
        }
        printf( "%6d, ", nb_of_events);
      }
    }
    RUN[k].nb_of_events = 0;
    for (j=0; j<1000000; j++) {
      if (EVENTS[j]) RUN[k].nb_of_events++;
    }
    printf( "== %6d", RUN[k].nb_of_events);
    printf( "\n");
  }
}

//====================================================================

void ctrl_C_handler( void) {
  exit( 0);
}

//====================================================================

void exit_handler( void) {
  int i;
  int total_nb_of_events;
  if ((database = fopen( "DATABASE", "wb")) != NULL) {
    fwrite( RUN, sizeof( RUN), 1, database);
    fclose( database);
    printf( "\n-----------------------------------------\n");
    total_nb_of_events = 0;
    for (i=0; i<1000; i++) {
      if (RUN[i].ID == 0) break;
      printf( "%3d  %8.8X = %08d %02X %12d\n", 
          i+1, RUN[i].ID, RUN[i].ID, RUN[i].type, RUN[i].nb_of_events);
      total_nb_of_events += RUN[i].nb_of_events;
    }
    printf( "-----------------------------------------\n");
    printf( "   Total number of events:   %12d\n", total_nb_of_events);
    printf( "-----------------------------------------\n");
  }
  else {
    printf( "\nFailed to write DATABASE!\n");
  }
  printf( "\n");
}

//====================================================================

int decode_event( void) {
  
  int i, j, k;
  
  bzero( &nb_of, sizeof( nb_of));
  block_length = actual_AMS_block_length;
  if ((AMS_block_ID & 0xFFC0) == 0) block_ID = 0;
  else                              block_ID = AMS_block_ID;
  if (block_ID != 0) printf( "*** EVT: wrong block_ID = %4.4X\n", block_ID);
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
    if (block_type) printf( "*** TYP: wrong block_ID = %4.4X\n", block_ID);
    switch (node_type) {
    case GEN_node_type:
      if (node_number != 0) {
        printf( "*** GEN: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
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
        printf( "*** JL1: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
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
        printf( "*** JL3: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
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
        printf( "*** TRK: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        i = TRA_index[node_number];
        if (i < 0) {
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
      printf( "*** NOD: wrong block_ID = %4.4X\n", block_ID);
      break;
    }
    k = k + block_length + 1;
  }
  if (k != actual_AMS_block_length + 1) printf( "*** wrong event block structure\n");
  else if (TRACE_LEVEL > 10)            printf( "  \n");
  if (nb_of.GEN_blocks == 1) decode_GEN_block();
  if (nb_of.JL1_blocks == 1) decode_JL1_block();
  if (nb_of.JL3_blocks == 1) decode_JL3_block();
  for (i=0; i<8; i++) {
    if (nb_of.CNT_blocks[i] == 1) decode_CNT_block( i);
  }
  for (i=0; i<2; i++) {
    for (j=0; j<32; j++) {
      if (nb_of.TRA_blocks[i][j] == 1) decode_TRA_block( i, j);
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
