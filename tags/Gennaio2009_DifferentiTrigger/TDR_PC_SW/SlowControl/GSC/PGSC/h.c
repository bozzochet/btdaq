// file 33c.c
// Program h - history
// 
// Reads and analyzes HRDL event files placed on several "nodes"
// A "node" means host-user-directory

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

#define integer32 unsigned int
#define integer16 unsigned short
#define TRUE  1
#define FALSE 0
#define OK   1
#define FAIL 0
#define N 256

int TRACE_LEVEL;

struct {
  char *host;
  char *user;
  char *pass;
  char *path;
} node[3] = {  "acarbon.cern.ch", "lebedev", "killcomm", "/dath1/HRDL/EVENTS/",
              "amsboron.ethz.ch", "lebedev", "KILLCOMM", "/dath1/HRDL/EVENTS/",
                            NULL,      NULL,       NULL, "/dath1/HRDL/EVENTS/" };

FILE *events;

//
//--- function declarations ---
//

int connect_to_remote_host( void);

int open_AMS_event_file( void);
int close_AMS_event_file( void);
int read_AMS_event_file( void);

long last_event_file_number( void);
long last_file_number_in( char *directory_name);
long first_file_number_in( char *directory_name);

int decode_event( void);
int decode_GEN_block( void);
int decode_JL1_block( void);
int decode_JL3_block( void);
int decode_CNT_block( int index);
int decode_TRA_block( int index, int data);
int it_is_calibration_event( void);
int it_is_laser_event( void);
int process_TRA_calibration( void);
int data_inventory( int mode);
int new_data_inventory( int mode);

void ctrl_C_handler( void);
void exit_handler( void);

//
//--- AMS event data structures description ---
//
    
integer16 block_length, block_ID;
int block_type, node_type, node_number, data_type;

#define GEN_node_type 0x1
#define JL1_node_type 0x2
#define JL3_node_type 0x9
#define CNT_node_type 0xA
#define TRA_node_type 0xB

int CNT_index[8] = {  0,  1, 2,  3,  4, 5,  6,  7};
int TRA_index[8] = { -1, -1, 0, -1, -1, 1, -1, -1};

struct {
  int GEN_blocks;
  int JL1_blocks;
  int JL3_blocks;
  int CNT_blocks[8];
  int TRA_blocks[2][32];
} nb_of;

integer16 *GEN_block;
integer16 *JL1_block;
integer16 *JL3_block;
integer16 *CNT_block[8];
integer16 *TRA_block[2][32];

integer32 run_ID;
integer16 run_type;
integer16 run_tag;
integer32 nb_of_events;
integer32 nb_of_bad_events;

integer32 event_number;
time_t    event_time;
char      ascii_event_time[26];
int       event_node_index;
long      event_file_number;
fpos_t    event_file_position;
char      event_path[256];
  
int it_is_bad_event;

//
// run database
//

FILE *database;

struct {
  integer32 ID;
  integer16 type;
  integer16 tag;
  integer32 nb_of_events;
  integer32 nb_of_bad_events;
  struct {
    integer32 number;
    time_t    time;
    int       node_index;
    long      file_number;
    fpos_t    file_position;
  } first_event;
  struct {
    integer32 number;
    time_t    time;
    int       node_index;
    long      file_number;
    fpos_t    file_position;
  } last_event;
} run[10000];

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

//--------------------------------------------------------------------

int connect_to_remote_host( void) {
  static char connected[256] = { "\0" };
  if (node[event_node_index].host == NULL) return OK;
  if (strcmp( node[event_node_index].host, connected)) {
    if (connected[0] != '\0') {
      close_AMS_event_file();
      Ftp_DisConnect();
      connected[0] = '\0';
    }
    if (Ftp_Connect( node[event_node_index].host,
                     node[event_node_index].user, 
                     node[event_node_index].pass) != 1) {
      if (TRACE_LEVEL > 0) {
        printf( "failed to connect to host '%s'\n", node[event_node_index].host);
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) {
        printf( "connected to host '%s'\n", node[event_node_index].host);
      }
      strcpy( connected, node[event_node_index].host);
    }
  }
  return OK;
}

//--------------------------------------------------------------------

int open_AMS_event_file( void) {

  int fd;
  
  sprintf( event_path, "%s%6.6d", node[event_node_index].path, event_file_number);

  if (node[event_node_index].host != NULL) {
    if ((fd = Ftp_Open( event_path)) < 0) {
      if (TRACE_LEVEL > 0) {
        printf( "Ftp_Open( '%s')  failed\n", event_path);
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) {
        printf( "Ftp_Open( '%s')  O.K.\n", event_path);
      }
      events = fdopen( fd, "r");
      fsetpos( events, &event_file_position);
      return OK;
    }
  }
  else {
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
}

//--------------------------------------------------------------------

int close_AMS_event_file( void) {
  int fd;
  if (node[event_node_index].host != NULL) {
    fd = fileno( events);
    if (Ftp_Close( fd) != 0) {
      if (TRACE_LEVEL > 0) {
        printf( "Ftp_Close %s  failed\n", event_path);
      }
      return FAIL;
    }
    else {
      if (TRACE_LEVEL > 1) {
        printf( "Ftp_Close %s  O.K.\n", event_path);
      }
      event_file_position = 0;
      return OK;
    }
  }
  else {
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
}

//--------------------------------------------------------------------

int read_AMS_event_file( void) {
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

long last_event_file_number( void) {

  long last_file_number;
  
  if (node[event_node_index].host != NULL) {
    last_file_number = Ftp_last_file_number_in( node[event_node_index].path);
    if (last_file_number < 0) {
      printf( "Ftp_last_file_number_in error\n");
      exit( 0);
    }
  }
  else {
    last_file_number = last_file_number_in( node[event_node_index].path);
  }

  return last_file_number;
}

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

long first_file_number_in( char *directory_name) {

  long file_number;
  long first_file_number;

  DIR  *dirp;
  struct dirent *dp;

  if (TRACE_LEVEL > 1) printf( "scan directory %s\n", directory_name);
  if ((dirp = opendir( directory_name)) == NULL) {
    printf( "opendir(%s) : %s\n", directory_name, strerror( errno));
    exit( 0);
  }
  first_file_number = 999999;
  while ((dp = readdir( dirp)) != NULL) {
    if (fnmatch( "[0-9][0-9][0-9][0-9][0-9][0-9]", dp->d_name, 0) == 0) {
      file_number = atol( dp->d_name);
      if (file_number < first_file_number) first_file_number = file_number;
    }
  }
  if (closedir( dirp)) {
    printf( "closedir(%s) : %s\n", directory_name, strerror( errno));
    exit( 0);
  }

  return first_file_number;
}

//====================================================================

void main( int argc, char *argv[]) {

  int i, j;
  long last_file_number;
  int  forced_node_index        = 2;
  long forced_first_file_number = 0;
  long forced_last_file_number  = 0;
  

  setbuf( stdout, NULL);	// set "no buffering" for stdout

  TRACE_LEVEL = 0;
  if (argc > 3) TRACE_LEVEL = atoi( argv[3]);
  if (TRACE_LEVEL > 9) TRACE_LEVEL = 9;
  if (argc > 1) forced_first_file_number = atoi( argv[1]);
  if (argc > 2) forced_last_file_number  = atoi( argv[2]);

  if (forced_first_file_number) {
    event_node_index  = forced_node_index;
    event_file_number = forced_first_file_number;
    if (forced_first_file_number <= 0) 
        event_file_number += first_file_number_in( node[event_node_index].path);
    event_file_position = 0;
    printf( "Existing database file (if any) will be overwritten!\n");
  }
  else {
    if ((database = fopen( "h.database", "rb")) != NULL) {
      printf( "Database file found...");
      fread( run, sizeof( run), 1, database);
      fclose( database);
      for (i=0; i<10000; i++) {
        if (run[i].ID == 0) break;
        event_node_index    = run[i].first_event.node_index;
        event_file_number   = run[i].first_event.file_number;
        event_file_position = run[i].first_event.file_position;
        indeX = i;
      }
      printf( " and read-in!\n");
      printf( "indeX = %d\n", indeX);
//==============>>>>>>>>>>>>>>
// new_data_inventory( 1);
// exit( 0);
//==============>>>>>>>>>>>>>>
    }
    else {
      event_node_index    = forced_node_index;
      event_file_number   = first_file_number_in( node[event_node_index].path);
      event_file_position = 0;
      printf( "No database file found, will be created!\n");
    }
  }

  atexit( exit_handler);
  signal( SIGINT, ctrl_C_handler);
  
  for (; event_node_index<3; event_node_index++) {
    if (!connect_to_remote_host()) exit( 0);

    last_file_number = forced_last_file_number;
    if (forced_last_file_number <= 0) 
        last_file_number += last_event_file_number();
    
    for (;
         event_file_number <= last_file_number;
         event_file_number++) {
//    printf( "event_file_number = %d\n", event_file_number);
//    exit( 0);
      if (open_AMS_event_file() == OK) {
        while (read_AMS_event_file() == OK) {
          static long counter=0;
          counter++;
          if (!(counter % 3000)) {
            fprintf( stderr, "\r %12ld    %ld\r", counter, event_file_number);
          }
          decode_event();
          process_TRA_calibration();
          data_inventory( 0);
          new_data_inventory( 0);
        }
        close_AMS_event_file();
        if ((database = fopen( "h.database", "wb")) != NULL) {
          fwrite( run, sizeof( run), 1, database);
          fclose( database);
        }
      }
    }
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
    if (TRACE_LEVEL > 0) printf( "*** EVT: wrong block_ID = %4.4X\n", block_ID);
  }
  if (TRACE_LEVEL > 1) {
    printf( "length/ID: %d/%4.4X =", block_length, block_ID);
  }
  k = 2;
  while (1) {
    if (k > actual_AMS_block_length) break;
    block_length = AMS_block[k];
    block_ID     = AMS_block[k+1];
    if (TRACE_LEVEL > 1) printf( " %d/%4.4X", block_length, block_ID);
    block_type  = (block_ID >> 13) & 0x07;
    node_type   = (block_ID >>  9) & 0x0F;
    node_number = (block_ID >>  6) & 0x07;
    data_type   =  block_ID        & 0x3F;
    if (block_type) {
      it_is_bad_event = TRUE;
      if (TRACE_LEVEL > 0) printf( "*** TYP: wrong block_ID = %4.4X\n", block_ID);
    }
    switch (node_type) {
    case GEN_node_type:
      if (node_number != 0) {
        it_is_bad_event = TRUE;
        if (TRACE_LEVEL > 0) printf( "*** GEN: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
          it_is_bad_event = TRUE;
          if (TRACE_LEVEL > 0) printf( "*** GEN: wrong block_ID = %4.4X\n", block_ID);
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
        if (TRACE_LEVEL > 0) printf( "*** JL1: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
          it_is_bad_event = TRUE;
          if (TRACE_LEVEL > 0) printf( "*** JL1: wrong block_ID = %4.4X\n", block_ID);
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
        if (TRACE_LEVEL > 0) printf( "*** JL3: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        if (data_type != 0) {
          it_is_bad_event = TRUE;
          if (TRACE_LEVEL > 0) printf( "*** JL3: wrong block_ID = %4.4X\n", block_ID);
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
        if (TRACE_LEVEL > 0) printf( "*** CNT: wrong block_ID = %4.4X\n", block_ID);
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
        if (TRACE_LEVEL > 0) printf( "*** TRK: wrong block_ID = %4.4X\n", block_ID);
      }
      else {
        i = TRA_index[node_number];
        if (i < 0) {
          it_is_bad_event = TRUE;
          if (TRACE_LEVEL > 0) printf( "*** TRK: wrong block_ID = %4.4X\n", block_ID);
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
      if (TRACE_LEVEL > 0) printf( "*** NOD: wrong block_ID = %4.4X\n", block_ID);
      break;
    }
    k = k + block_length + 1;
  }
  if (k != actual_AMS_block_length + 1) {
    it_is_bad_event = TRUE;
    if (TRACE_LEVEL > 0) printf( "*** wrong event block structure\n");
  }
  else {
    if (TRACE_LEVEL > 1) printf( "  \n");
  }

//-------

  if (nb_of.GEN_blocks == 1) {
    decode_GEN_block();
  }
  else {
    it_is_bad_event = TRUE;
    if (TRACE_LEVEL > 0) printf( "*** wrong number of GEN blocks: %d(1)\n", nb_of.GEN_blocks);
  }
  if (it_is_calibration_event()) {
    return;
  }
  if (it_is_laser_event()) {
    return;
  }
  if (run_type != 0x1F) {
    it_is_bad_event = TRUE;
    if (TRACE_LEVEL > 0) printf( "*** wrong run type: %02X(1F)\n", run_type);
  }
  if (nb_of.JL1_blocks == 1) {
    decode_JL1_block();
  }
  else {
    it_is_bad_event = TRUE;
    if (TRACE_LEVEL > 0) printf( "*** wrong number of JL1 blocks: %d(1)\n", nb_of.JL1_blocks);
  }
  if (nb_of.JL3_blocks == 1) {
    decode_JL3_block();
  }
  else {
    it_is_bad_event = TRUE;
    if (TRACE_LEVEL > 0) printf( "*** wrong number of JL3 blocks: %d(1)\n", nb_of.JL3_blocks);
  }
  for (i=0; i<8; i++) {
    if (nb_of.CNT_blocks[i] == 1) {
      decode_CNT_block( i);
    }
    else {
      it_is_bad_event = TRUE;
      if (TRACE_LEVEL > 0) printf( "*** wrong number of CNT[%d] blocks: %d(1)\n", i, nb_of.CNT_blocks[i]);
    }
  }
  for (i=0; i<2; i++) {
    if (nb_of.TRA_blocks[i][0] == 1) {
      decode_TRA_block( i, 0);
    }
    else {
      it_is_bad_event = TRUE;
      if (TRACE_LEVEL > 0) printf( "*** wrong number of TRA[%d][0] blocks: %d(1)\n", i, nb_of.TRA_blocks[i][0]);
    }
    for (j=1; j<32; j++) {
      if (nb_of.TRA_blocks[i][j] != 0) {
        it_is_bad_event = TRUE;
        if (TRACE_LEVEL > 0) printf( "*** wrong number of TRA[%d][%d] blocks: %d(0)\n", i, j, nb_of.TRA_blocks[i][j]);
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
    if (TRACE_LEVEL > 5) printf( "RUN: %8.8X    TYPE: %4.4X   TAG: %4.4\n", 
                         run_ID, run_type, run_tag);
    old_run_ID = run_ID;
  }
  
  strcpy( ascii_event_time, (char *)(ctime(&event_time)+4));
  ascii_event_time[15] = '\0';
  
/*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  static FILE *file = NULL;
  long t1, t2, t3;
  static long t2_old, t3_old, t2_max;
  if (file == NULL) file = fopen( "rates", "w");
  if (file != NULL) {
    t1 = (long)GEN_block[8]  << 16 | (long)GEN_block[9];
    t2 = (long)GEN_block[10] << 16 | (long)GEN_block[11];
    t3 = (long)GEN_block[8]  << 48 | (long)GEN_block[9] << 32 | (long)GEN_block[10] << 16 | (long)GEN_block[11];
//    if (t2_old) fprintf( file, "%ld %ld\n", t2, t2-t2_old);
    if (t2 > t2_max) {
      t2_max = t2;
      fprintf( file, " %ld\n", t2_max);
    }
    t2_old = t2;
    t3_old = t3;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

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
  int i, hour, min, sec;
  char c;
  static integer32 run_ID_table[10000];
  static int run_ID_table_size, run_ID_table_index;
  static time_t total_time;
  static nb_of_events, nb_of_bad_events;
  static total_nb_of_events, total_nb_of_bad_events;
  static integer32 current_run_ID;
  static integer16 current_run_type;
  static integer16 current_run_tag;
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
      printf( "%8.8X%c %2.2X %4.4X %8d %8d %8d (%d)   %s-%s  %3d.%02d %06d..%06d \n",
               current_run_ID, c, current_run_type, current_run_tag,
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
    current_run_tag         = run_tag;
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
    min  = (total_time + 30) / 60;
    hour = min / 60;
    min  = min % 60;
    if (hour) printf( "Total time = %d hours %d mins\n", hour, min);
    else      printf( "Total time = %d mins\n", min);
}
  
  last_event_file_number = event_file_number;
  last_event_number      = event_number;
  last_event_time        = event_time;
  nb_of_events++;
  if (it_is_bad_event) nb_of_bad_events++;
}

//--------------------------------------------------------------------

int new_data_inventory( int mode) {

  int i, hour, min, sec;
  char ascii_time[26];
  static integer32 old_run_ID;
  static integer32 old_event_number;
  static time_t    old_event_time;
  static long      old_event_file_number;
  static fpos_t    old_event_file_position;
  static struct {
    int nb_of_events;
    int calibration_present;
    time_t started;
    time_t duration;
  } usefull[65536];

  if (run_ID != old_run_ID || event_number < old_event_number || mode) {
    if (old_run_ID) {
      run[indeX].last_event.number        = old_event_number;
      run[indeX].last_event.time          = old_event_time;
      run[indeX].last_event.node_index    = event_node_index;
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
      run[indeX].first_event.node_index    = event_node_index;
      run[indeX].first_event.file_number   = event_file_number;
      run[indeX].first_event.file_position = event_file_position;
    }
  }
  
  if (mode) {
    long   total_nb_of_events             = 0;
    long   total_nb_of_normal_events      = 0;
    long   total_nb_of_laser_events       = 0;
    long   total_nb_of_calibration_events = 0;
    long   total_nb_of_bad_events         = 0;
    time_t total_time                     = 0;
    int    nb_of_usefull_runs           = 0;
    long   total_nb_of_usefull_events   = 0;
    time_t total_time_of_usefull_events = 0;
    printf( "---------------------------------------------------------------------------------------------------------------\n");
    for (i=0; i<10000; i++) {
      char c = ' ';
      if (run[i].ID == 0) break;
      total_nb_of_events += run[i].nb_of_events;
      if (run[i].type == 0x07) {
        total_nb_of_laser_events += run[i].nb_of_events;
      }
      if (run[i].type == 0x1F) {
        if (run[i].first_event.number == 1 &&
            run[i].last_event.number  == 5 &&
            run[i].nb_of_events       == 5) {
          total_nb_of_calibration_events += run[i].nb_of_events;
        }
        else {
          total_nb_of_normal_events += run[i].nb_of_events;
        }
        if (run[i].tag >= 0) {
          int j = run[i].tag;
          if (run[i].type == 0x1F) {
            if (run[i].first_event.number == 1 &&
                run[i].last_event.number  == 5 &&
                run[i].nb_of_events       == 5) {
              usefull[j].calibration_present = 1;
            }
            else {
              usefull[j].nb_of_events += run[i].nb_of_events;
              usefull[j].duration     += run[i].last_event.time - run[i].first_event.time;
              if (usefull[j].started == 0 ||
                  usefull[j].started > run[i].first_event.time) {
                usefull[j].started = run[i].first_event.time;
              }
            }
          }
        }
      }
      total_nb_of_bad_events += run[i].nb_of_bad_events;
      total_time += run[i].last_event.time - run[i].first_event.time;
      if (i > 0) if (run[i].ID <= run[i-1].ID) c = '*';
      printf( "%8.8X = %09d%c ", run[i].ID, run[i].ID, c);
      printf( "%2.2X ", run[i].type);
      printf( "%4.4X ", run[i].tag);
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
      printf( "%d ", run[i].first_event.node_index);
      printf( "%06d..", run[i].first_event.file_number);
      printf( "%06d ", run[i].last_event.file_number);
      printf( "\n");
/*
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
static FILE *file = NULL;
if (file == NULL) file = fopen( "ddd", "w");
fprintf( file, "%3d %6d %6d %6d\n",
  run[i].type,
  run[i].last_event.number - run[i].first_event.number + 1,
  run[i].nb_of_events,
  run[i].last_event.time - run[i].first_event.time
  );
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
    }
    printf( "---------------------------------------------------------------------------------------------------------------\n");
    printf( "Total nb. of runs = %d\n", i);
    printf( "Total nb. of events = %d = normal:%d + laser:%d + calib:%d + bad:%d\n",
        total_nb_of_events, 
        total_nb_of_normal_events, 
        total_nb_of_laser_events, 
        total_nb_of_calibration_events, 
        total_nb_of_bad_events);
    min  = (total_time + 30) / 60;
    hour = min / 60;
    min  = min % 60;
    if (hour) printf( "Total time = %d hours %d mins\n", hour, min);
    else      printf( "Total time = %d mins\n", min);
    if ((database = fopen( "h.database", "wb")) != NULL) {
      fwrite( run, sizeof( run), 1, database);
      fclose( database);
    }
    else {
    }
    printf( "---------------------------------------------------------------------------------------------------------------\n");
    printf( "=========================================\n");
    printf( "      tag  events   date   started   time\n");
    printf( "-----------------------------------------\n");
    for (i=0x2000; i<0xF000; i++) {
      if (usefull[i].nb_of_events) {
        char note[2] = { '*', ' '};
        nb_of_usefull_runs++;
        total_nb_of_usefull_events   += usefull[i].nb_of_events;
        total_time_of_usefull_events += usefull[i].duration;
        strcpy( ascii_time, (char *)(ctime(&usefull[i].started)+4));
        ascii_time[15] = '\0';
        min = usefull[i].duration / 60;
        sec = usefull[i].duration % 60;
        printf( "%4d %4.4X %7d  %s %3d:%02d %c\n", 
                nb_of_usefull_runs, 
                i, 
                usefull[i].nb_of_events, 
                ascii_time, 
                min, 
                sec,
                note[usefull[i].calibration_present]);
      }
    }
    printf( "=========================================\n");
    min  = (total_time_of_usefull_events + 30) / 60;
    hour = min / 60;
    min  = min % 60;
    printf( "Total nb. of runs = %d\n", nb_of_usefull_runs);
    printf( "Total nb. of events = %d\n", total_nb_of_usefull_events);
    if (hour) printf( "Total time = %d hours %d mins\n", hour, min);
    else      printf( "Total time = %d mins\n", min);
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
