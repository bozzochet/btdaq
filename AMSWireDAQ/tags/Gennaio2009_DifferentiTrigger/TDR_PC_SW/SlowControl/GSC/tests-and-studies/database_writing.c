// 1.c
// file database_writing.c
//
// special version of deframing program for SRDL link
// - reads Timo's frame files
// - deframes them
// - writes individual block files ( a la STATUS files)
//

#include "SRDL_frames.h"

//-- this was for the STS-91 flight data
//char *frame_directory = "/stage/srdl/HK/";
//char *block_directory = "/dats1/SRDL/DATABASE/";
//char *log_file_name   = "/stage/logs/database_writing_log.file";

//-- this is for the GSI testbeam data
char *frame_directory = "/dat0/SlowDir/GSI/stage/srdl/HK/";
char *block_directory = "/dat0/SlowDir/GSI/dats/SRDL/DATABASE/";
char *log_file_name   = "/dat0/SlowDir/GSI/stage/logs/database_writing_log.file";

FILE *blocks[8192];

//
//----- PDI/HK/SRDL frame data structure description ---------
//     (it is "compatible" with HRDL frame description)
//    (it is so funny to be similar to that of AMS block)
//

#define PDI_FRAME_LENGTH_IN_BYTES 160

#define SRDL_FRAME_LENGTH_IN_BYTES 124
#define SRDL_FRAME_BODY_LENGTH 60

union {
  unsigned char bytes[PDI_FRAME_LENGTH_IN_BYTES];
  integer16     words[PDI_FRAME_LENGTH_IN_BYTES/2];
  integer32     wordl[PDI_FRAME_LENGTH_IN_BYTES/4];
} PDI_frame;

#define         PDI_sync_pattern          PDI_frame.wordl[0]
integer16      *OIU_health_and_status =  &PDI_frame.words[2];
integer16      *AMS_health_and_status =  &PDI_frame.words[6];
  unsigned char AMS_health_discreets;
  unsigned char AMC_GPC_command_counter;
  unsigned char AMS_command_counter;
#define         SRDL_frame_header         PDI_frame.words[8]
  unsigned char SRDL_frame_ID;                         
  unsigned char SRDL_frame_first_block_link;
integer16      *SRDL_frame_body =        &PDI_frame.words[9];
#define         SRDL_frame_CRC            PDI_frame.words[69]
integer16      *OIU_transaction_status = &PDI_frame.words[70]; 
integer16      *OIU_fill_words =         &PDI_frame.words[72]; 

//--------------------------------------------------------------------

void initialize( void) {
  static done = FALSE;
  integer16 block_type;
  int i, n;
  char *command, *object, *dataset;
  char path[81];


  printf("\n\t*********************** A.Lebedev ******\n");  
  printf(  "\t*                                      *\n");  
  printf(  "\t*       DATABASE WRITING PROGRAM       *\n");  
  printf(  "\t*                                      *\n");  
  printf(  "\t*            trace level %d             *\n",
  TRACE_LEVEL);  
  printf(  "\t*                                      *\n");  
  printf(  "\t****************************************\n\n");
  
  if (done) return;
  
  done = TRUE;

  if (sizeof( integer16) != 2) {
    printf( "The size of 'integer16' is wrong! Good bye!\n");
    exit( 0);
  }
  if (sizeof( integer32) != 4) {
    printf( "The size of 'integer32' is wrong! Good bye!\n");
    exit( 0);
  }

  frame_sync = FALSE;		// initialize the deframing process

  for (i=0; i<8192; i++) blocks[i] = NULL;

  n = 0;
  AMS_block_ID=0;
  while (1) {
    block_type = (AMS_block_ID >> 13) & 0x07;
    if (block_type == 0x1 || block_type == 0x3 || block_type == 0x4) {
      if (decode_AMS_block_ID( AMS_block_ID, &command, &object, &dataset)) {
        if (TRACE_LEVEL > 2) {
          n++;
          printf( "%4d %4.4X %s %s %s\n", n, AMS_block_ID, command, object, dataset);
        }
        sprintf( path, "%s%s.%s", block_directory, object, dataset);
        i = AMS_block_ID & 0x1FFF;
        if (blocks[i] != NULL) fclose( blocks[i]);
        if ((blocks[i] = fopen( path, "wb")) == NULL) {
          printf( "Failed to 'fopen( %s, ...)'!\n", path);
          exit( 0);
        }
      }
    }
    AMS_block_ID++;
    if (AMS_block_ID == 0) break;
  }
  n = 0;
  for (i=0; i<8192; i++) if (blocks[i] != NULL) n++;
  printf( "%d block files opened in %s\n", n, block_directory);
}

//------------------------------------------------------------------------------

void redirect_stdout_to_file( char *file_name) {
  FILE *file;

  errno = 0;
  chmod( file_name, S_IRWXU | S_IRWXG | S_IRWXO);
  if ((file = freopen( file_name, "a", stdout)) == NULL) {
    printf( "freopen(%s) : %s\n", file_name, strerror( errno));
    exit( 0);
  }
  chmod( file_name, S_IRWXU | S_IRWXG | S_IRWXO);
  setbuf( file, NULL);

}

//--------------------------------------------------------------------

char *mytime( void) {

  time_t ltime;
  char *t;
  static char tt[26];
  
  time( &ltime);
  t = ctime( &ltime);
  strcpy( tt, t);
  tt[24] = '\000';
  return tt;
}

//--------------------------------------------------------------------

int open_SRDL_frame_file( void) {

  extern char *frame_directory;
  
  sprintf( frame_path, "%s%6.6u", frame_directory, frame_file_number);
  
  if ((frames = fopen( frame_path, "rb")) == NULL) {
    if (TRACE_LEVEL > 0) {
      printf( "fopen(%s,\"rb\") : %s\n", frame_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "open      %s\n", frame_path);
    return OK;
  }
}

//--------------------------------------------------------------------

int read_SRDL_frame_file( void) {
  int ltot;
  integer16 *p;

  errno = 0;

// the number "160" is a magic number introduced by Timo & Timo

  ltot = fread( PDI_frame.bytes, sizeof( unsigned char), 160, frames);
  
  if (ltot == 160) {
    for (p=PDI_frame.words; 
         p<PDI_frame.words+PDI_FRAME_LENGTH_IN_BYTES/2;
         p++) {
      (*p) = ((*p) >> 8) | ((*p) << 8);
    }
    nb_of.SRDL.frames.read++;
    return OK;
  }
  else {
    if (ltot > 0) {
      if (TRACE_LEVEL > 0) {
        printf( "file %s: last frame has only %d bytes.\n", 
                 frame_path, ltot);
      }
    }
    if (errno) {
      printf( "fread(%s) : %s\n", frame_path, strerror( errno));
      printf( "Execution terminated.\n");
      exit( 0);
    }
    return FAIL;
  }
}


//--------------------------------------------------------------------

/****************************************************************************/
/*                                                                          */
/*  cyclic redundancy check source was "borrowed" from VK (and "reworked")  */
/*                                                                          */
/****************************************************************************/

int decode_SRDL_frame( void) {

  integer16 *p, d, i, FCS;
  integer16 g = 0x1021;

  PDI_sync_pattern = (PDI_sync_pattern >> 16) | (PDI_sync_pattern << 16);

  if ((PDI_sync_pattern & 0xFFFFFF00) != 0xC95A6300) {
    if (TRACE_LEVEL > 0) {
      printf( "%s #%d: SYNC pattern error: PDI_sync_pattern=%8.8X (C95A63xx) \n",
              frame_path, 
              nb_of.SRDL.frames.read,
              PDI_sync_pattern);
    }
    nb_of.SRDL.frames.with_SYNC_errors++;
    frame_sync = FALSE;
    return FAIL; 
  }

  if (((OIU_transaction_status[0] & 0x8010) != 0x8010) ||
      ((OIU_transaction_status[1] & 0x8010) != 0x8010)) {
    if (TRACE_LEVEL > 0) {
      printf( "%s #%d: 1553 error: block status words = %4.4X %4.4X\n",
              frame_path,
              nb_of.SRDL.frames.read,
              OIU_transaction_status[0], OIU_transaction_status[1]);
    }
    nb_of.SRDL.frames.with_1553_errors++;
    frame_sync = FALSE;
    return FAIL; 
  }

  FCS = 0xFFFF;
  for (p=&SRDL_frame_header; p<&SRDL_frame_CRC; p++) {
    d = *p;
    for (i=0; i<16; i++) {
      if ((FCS ^ d) & 0x8000)
        FCS = (FCS << 1) ^ g;
      else
        FCS = (FCS << 1);
      d <<= 1;
    }
  }

  if (FCS != SRDL_frame_CRC) {
    if (TRACE_LEVEL > 0) {
      printf( "%s #%d: CRC16 error: SRDL_frame_CRC=%8.8X FCS=%8.8X\n",
              frame_path,
              nb_of.SRDL.frames.read,
              SRDL_frame_CRC, FCS);
    }
    nb_of.SRDL.frames.with_CRC_errors++;
    frame_sync = FALSE;
    return FAIL;
  }
  return OK;

}

//--------------------------------------------------------------------

void deframe_SRDL_frame( void) {

#define E(msg) \
  if (TRACE_LEVEL > 0) \
  printf( "%s #%d: deframing error '%s': SRDL_frame_ID=%2.2X\n", \
          frame_path, \
          nb_of.SRDL.frames.read, \
          msg, (unsigned char) SRDL_frame_ID); \
  nb_of.SRDL.frames.with_deframing_errors++;

#define W(msg) \
  if (TRACE_LEVEL > 0) \
  printf( "%s #%d: deframing warning '%s': SRDL_frame_ID=%2.2X\n", \
          frame_path, \
          nb_of.SRDL.frames.read, \
          msg, (unsigned char) SRDL_frame_ID); \
  nb_of.SRDL.frames.with_deframing_warnings++;
    
  static int           block_open;
  integer16            frame_index;
  static integer16     block_index;
  integer32            expected_first_block_link;
  static unsigned char old_frame_ID;
        
  EOF_encountered = FALSE;

  SRDL_frame_ID = SRDL_frame_header >> 8;
  SRDL_frame_first_block_link = SRDL_frame_header & 0x00FF;
  
  if (frame_sync && ((++old_frame_ID) % 100) != SRDL_frame_ID) {
    E("wrong frame sequence");
    frame_sync = FALSE;
  }

  old_frame_ID = SRDL_frame_ID;

  if (frame_sync) {
    frame_index = 0;
    if (block_open) {
      expected_first_block_link = AMS_block_length - block_index;
      if (expected_first_block_link < SRDL_FRAME_BODY_LENGTH) {
        if (SRDL_frame_first_block_link != expected_first_block_link) {
          E("invalid link found!");
          frame_sync = FALSE;
          block_open = FALSE;
          return;
        }
      }
      else {
        if (SRDL_frame_first_block_link != 0xFF) {
          E("invalid link found!!");
          frame_sync = FALSE;
          block_open = FALSE;          
          return;
        }
      }
    }
  }
  else {
    if (SRDL_frame_first_block_link == 0xFF) {
      W("frame is not locked");
      return;
    }
    if (SRDL_frame_first_block_link >= SRDL_FRAME_BODY_LENGTH) {
      W("invalid link found");
      return;
    }
    else {
      frame_index = SRDL_frame_first_block_link;
      frame_sync = TRUE;
      if (block_open) {
        block_open = FALSE;
      }
    }
  }

  LOOP:
    if (!block_open) {
      if ((AMS_block_length = SRDL_frame_body[frame_index++]) == 0) {
        W("EOF encountered");
        nb_of.SRDL.frames.with_EOFs++;
        EOF_encountered = TRUE;
        frame_sync = FALSE;
        return;
      }
      if (AMS_block_length > MAX_AMS_BLOCK_LENGTH) {
        E("tooo big block rejected");
        frame_sync = FALSE;
        return;
      }
      block_open = TRUE;
      block_index = 0;
    }
    while (block_index < AMS_block_length 
        && frame_index < SRDL_FRAME_BODY_LENGTH) {
      AMS_block_data[block_index++] = SRDL_frame_body[frame_index++];
    }   
    if (block_index == AMS_block_length) {
      block_open = FALSE;
      process_AMS_block();
    }
    if (block_index > AMS_block_length) {
      E("impossible situation!");
      frame_sync = FALSE;
      return;
    }
    if (frame_index == SRDL_FRAME_BODY_LENGTH) {
      return;
    }
    if (frame_index > SRDL_FRAME_BODY_LENGTH) {
      E("impossible situation!!");
      frame_sync = FALSE;
      return;
    }
  goto LOOP;
}

//--------------------------------------------------------------------

int close_SRDL_frame_file( void) {

  if (fclose( frames)) {
    if (TRACE_LEVEL > 0) {
      printf( "fclose(%s) : %s\n", frame_path, strerror( errno));
    }
    return FAIL;
  }
  else {
    if (TRACE_LEVEL > 1) printf( "close     %s\n", frame_path);
    return OK;
  }
}

//------------------------------------------------------------------------------

void process_AMS_block( void) {

  int block_type, block_status;
  int i;
  
  nb_of.blocks.deframed++;
  
  block_type = (AMS_block_ID >> 13) & 0x07;
  if (block_type != 0x1 && block_type != 0x3 && block_type != 0x4) return;
  block_status = (AMS_block_body[0] >> 12) & 0xF;
  if (block_status) return;
  
  i = AMS_block_ID & 0x1FFF;
  if (blocks[i] == NULL) {
    printf( "%4.4X\n", AMS_block_ID);
    nb_of.block_ID_decoding_failures++;
    return;
  }
  
  if (fwrite( AMS_block, sizeof( integer16), AMS_block_length+1, blocks[i])
      != AMS_block_length+1) {
    printf( "Block file writing troubles!\n");
    exit( 0);
  }
  else {
    nb_of.blocks.written++;
  }
}

//------------------------------------------------------------------------------

void print_database_writing_log( void) {

      printf( "%s * %s : RD=%d SYNCe=%d 1553e=%d CRCe=%d DFe=%d DFw=%d DF=%d WR=%d IDe=%d\n",
      
          mytime(),
          frame_path,
          
          nb_of.SRDL.frames.read, 
          nb_of.SRDL.frames.with_SYNC_errors, 
          nb_of.SRDL.frames.with_1553_errors, 
          nb_of.SRDL.frames.with_CRC_errors, 
          nb_of.SRDL.frames.with_deframing_errors,
          nb_of.SRDL.frames.with_deframing_warnings,
          
          nb_of.blocks.deframed,
          
          nb_of.blocks.written,
          
          nb_of.block_ID_decoding_failures);
}

//------------------------------------------------------------------------------

void main( int argc, char *argv[]) {

  integer32 first_frame_file_number, last_frame_file_number;
  integer32 i;

  setbuf( stdout, NULL);	// set "no buffering" for stdout
  TRACE_LEVEL = 0;
  if (argc > 3) TRACE_LEVEL = atoi( argv[3]);
  if (TRACE_LEVEL > 9) TRACE_LEVEL = 9;
  initialize();
  
#ifndef do_not_use_logfile
  printf( "\n\t see my output in '%s'\n", log_file_name);
  redirect_stdout_to_file( log_file_name);
  initialize();
#endif
    
  first_frame_file_number = 0;
  if (argc > 1) {
    i = atoi( argv[1]);
    if (i > 0 && i <= 999999) {
      first_frame_file_number = i;
    }
  }
//if (!first_frame_file_number) first_frame_file_number = 32218;  // STS-91 flight
  if (!first_frame_file_number) first_frame_file_number = 5000;  // STS-91 flight
  
  last_frame_file_number = 0;
  if (argc > 2) {
    i = atoi( argv[2]);
    if (i > 0 && i <= 999999) {
      last_frame_file_number = i;
    }
  }
//if (!last_frame_file_number) last_frame_file_number = 45282;  // STS-91 flight
  if (!last_frame_file_number) last_frame_file_number = 22412;  // STS-91 flight
  
  printf( "Will read frame files %6.6u..%6.6u\n",
           first_frame_file_number, last_frame_file_number);

  do_not_write_AMS_block_file_PLS = TRUE;
  for (frame_file_number  = first_frame_file_number-1;
       frame_file_number <=  last_frame_file_number;
       frame_file_number++) {
    if (open_SRDL_frame_file() == OK) {
      bzero( &nb_of, sizeof( nb_of));
      {
        // float r = (float)(frame_file_number-first_frame_file_number) / 
        //           (float)(last_frame_file_number-first_frame_file_number);
        // if (r > 0.0) printf( "\r%4.2f", r);
      }
      while (read_SRDL_frame_file() == OK) {
        if (decode_SRDL_frame() == OK) {
          deframe_SRDL_frame();
        }
      }
      close_SRDL_frame_file();
      print_database_writing_log();
    }
    do_not_write_AMS_block_file_PLS = FALSE;
  }
  printf( "\r");
}

//------------------------------------------------------------------------------

