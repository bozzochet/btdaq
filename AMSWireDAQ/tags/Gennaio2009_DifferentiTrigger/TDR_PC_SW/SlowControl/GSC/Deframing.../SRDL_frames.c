// file SRDL_frames.c
// pieces of deframing program for SRDL link

#include "SRDL_frames.h"

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


//********************************************************************
//                                                                   *
//                                                                   *
//                    functions used for deframing                   *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------

void initialize_everything( void) {

  if (sizeof( integer16) != 2) {
    printf( "The size of 'integer16' is wrong! Good bye!\n");
    exit( 0);
  }
  if (sizeof( integer32) != 4) {
    printf( "The size of 'integer32' is wrong! Good bye!\n");
    exit( 0);
  }

  printf("\n\t********************************************* A.Lebedev ******\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*       (S)LOW RATE DATA LINK = SRDL DEFRAMING PROGRAM       *\n");  
  printf(  "\t*                                                            *\n");  
  printf(  "\t*                     trace level %d                          *\n",
  TRACE_LEVEL);  
  printf(  "\t*                                                            *\n");  
  printf(  "\t**************************************************************\n\n");

  frame_sync = FALSE;		// initialize the deframing process
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
      write_AMS_status_block_file();
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

//--------------------------------------------------------------------

void print_SRDL_deframing_log( void) {

      printf( "%s * %s : RD=%d SYNCe=%d 1553e=%d CRCe=%d DFe=%d DFw=%d DF=%d=%d+%d+%d WR=%d=%d+%d+%d ACK=%d ERR=%d ST=%d STe=%d\n",
      
          mytime(),
          frame_path,
          
          nb_of.SRDL.frames.read, 
          nb_of.SRDL.frames.with_SYNC_errors, 
          nb_of.SRDL.frames.with_1553_errors, 
          nb_of.SRDL.frames.with_CRC_errors, 
          nb_of.SRDL.frames.with_deframing_errors,
          nb_of.SRDL.frames.with_deframing_warnings,
          
          nb_of.event.blocks.deframed
           + nb_of.HKdata.blocks.deframed 
           + nb_of.reply.blocks.deframed,
          nb_of.event.blocks.deframed,
          nb_of.HKdata.blocks.deframed,
          nb_of.reply.blocks.deframed,
          
          nb_of.event.blocks.written
           + nb_of.HKdata.blocks.written 
           + nb_of.reply.blocks.written,
          nb_of.event.blocks.written,
          nb_of.HKdata.blocks.written,
          nb_of.reply.blocks.written,
          
          nb_of.type_6_blocks,
          nb_of.command_failures,
          
          nb_of.status.blocks.written,
          nb_of.block_ID_decoding_failures);
}

//--------------------------------------------------------------------


//********************************************************************
//                                                                   *
//                                                                   *
//              functions used for testing of deframing              *
//                                                                   *
//                                                                   *
//********************************************************************


//--------------------------------------------------------------------
