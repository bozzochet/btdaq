// this is file c.c
//
// AMS 1553 bus controller for STS-91 PGSC computer 
// **** (PCMCIA 1553 interface only) ****
//
// - main program
//

#include "bc.h"
#include "pgsc.h"

void exit_handler(void);
void ctrlc_handler(void);
void PDI_processing( void);
void PSP_processing( void);
void do_something_stupied( void);
void fill_block_CRC( void);

long nb_of_frames_received = 0;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define MAX_AMS_COMMAND_BLOCK_LENGTH 61         // 16-bit words


integer16                  AMS_command_block[MAX_AMS_COMMAND_BLOCK_LENGTH+2];
#define block_length       AMS_command_block[0]
#define block_ID           AMS_command_block[1]
#define command_ID         AMS_command_block[2]
integer16 *command_time = &AMS_command_block[3];
integer16 *block_body   = &AMS_command_block[5];
#define block_CRC          AMS_command_block[block_length+1]  // it is outside block !

//-------------------------------------------------------------------

void main(void) {
  time_t t1, t2;
  long total_nb_of_frames_received;

  signal( SIGINT, ctrlc_handler);
  if (atexit( exit_handler)) printf( "exit handler declaration error\n");
  
  if (enable_bus_controller()) printf( "BC enabled\n");
  else                         exit( 0);
  I_am_in_BM_mode = FALSE;
  I_am_in_BC_mode = TRUE;
  I_am_in_RT_mode = FALSE;
  I_am_DEC_ALPHA  = FALSE;
  
  setup_display();
  open_log_pipe_for_writing();

LOOP:  
  total_nb_of_frames_received = 0;
  t1 = time( NULL);
  while(1) {
    PSP_processing();
    PDI_processing();
//  do_something_stupied();
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

//-------------------------------------------------------------------

void PDI_processing( void) {
  unsigned char *PDI_frame; 
  int i;
  if (take_PDI( &PDI_frame) == 1) {
    nb_of_frames_received++;
    for (i=0; i<64; i++) {
      HK.word[i] = (PDI_frame[2*i+12] << 8) | PDI_frame[2*i+13];
    }
    block_status[0] = (PDI_frame[140] << 8) | PDI_frame[141];
    block_status[1] = (PDI_frame[142] << 8) | PDI_frame[143];
    process_PDI_frame();
  }
}

//-------------------------------------------------------------------

void PSP_processing( void) {
  if (PSP_request && PSP_status) {
    send_PSP( PSP_frame, &PSP_status);
    PSP_request = 0; 
  }
}

//-------------------------------------------------------------------

void do_something_stupied( void) {

  int i;
  static int sw = 0;
  
  if (PSP_request) return;
  
  block_length    = 4;
  block_type      = 0x1;
  node_type       = 0x8;
  node_number     = 0x0;
  if (sw) {
    sw = 0;
    data_type     = 0x1;
  }
  else {
    sw = 1;
    data_type     = 0x2;
  }
  block_ID        = block_type<<13 | node_type<<9 | node_number<<6 | data_type;
  command_ID      = ++command_ID & 0x0FFF;
  command_time[0] = 0x1234;
  command_time[1] = 0x5678;

  fill_block_CRC();
  
  PSP_frame[0] = 0;
  PSP_frame[1] = 2 * (block_length + 3);
  
  for (i=0; i<block_length+2; i++) {
    PSP_frame[2*i+2] = AMS_command_block[i] >> 8;
    PSP_frame[2*i+3] = AMS_command_block[i];
  }
  PSP_request = 1;
}

//-------------------------------------------------------------------

/****************************************************************************/
/*                                                                          */
/*  cyclic redundancy check source was "borrowed" from VK (and "reworked")  */
/*                                                                          */
/****************************************************************************/

void fill_block_CRC( void) {
  integer16 *p, FCS, d, i;
  integer16 g = 0x1021;
  FCS = 0xFFFF;
  for (p=AMS_command_block; p<&block_CRC; p++) {
    d = *p;
    for (i=0; i<16; i++) {
      if ((FCS ^ d) & 0x8000)
        FCS = (FCS << 1) ^ g;
      else
        FCS = (FCS << 1);
      d <<= 1;
    }
  }
  block_CRC = FCS;
}

//-------------------------------------------------------------------

void ctrlc_handler( void) {
  exit( 0);
}

//-------------------------------------------------------------------

void exit_handler( void) {
  disable_bus_controller(); 
  printf("\nLet us try to switch off this damned card\n");
}

//-------------------------------------------------------------------

