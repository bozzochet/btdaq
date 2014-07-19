// this is file w.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
//  W.C is supposed to prepare some input for 1553 simulation

#include "pgsc.h"

#define VERBOSE FALSE

int nb_of_blocks_framed = 0;
int nb_of_frames_framed = 0;
int EOF_encountered = FALSE;

void get_AMS_block( void) {
  int i;

  nb_of_blocks_framed++;

// goto SIMPLE

  if (nb_of_blocks_framed >= 1000) {
    bl_length = 0;                      /* write EOF */
    EOF_encountered = TRUE;
    return;
  }

  i = rand() % 4;
  if (i == 0) simulate_PSB_block();
  if (i == 1) simulate_HTC_block();
  if (i == 2) simulate_CAN_block();
  if (i == 3) simulate_MON_block();
  if (i == 4) simulate_CNT_block();

  bl_length = AMS_block_length;
  for (i=0; i<bl_length; i++) {
    bl_data[i] = AMS_block_data[i];
  }
  return; 


SIMPLE:  
  bl_length = 4+nb_of_blocks_framed;
  if (nb_of_blocks_framed >= 99) {
    bl_length = 0;                         // write EOF
    EOF_encountered = TRUE;
    printf("please,writeEOF\n");
    return;
  }
  bl_data[0] = bl_length * 16;
  for (i=1; i<bl_length; i++) {
    bl_data[i] = i;
  }
  return;
}
      
void frame_HK_frame( void) {
#define FRAME_HEADER ((HK_frame_ID << 8) | HK_frame_first_block_link)
  static int block_open = FALSE, frame_index, block_index;
  
  nb_of_frames_framed++;
  
  HK_frame_ID = (++HK_frame_ID) % 100;
  HK_frame_first_block_link = 0xff;
  if (HK_frame_ID == 15) HK_frame_first_block_link=1;    // some more error
  HK.frame.header = FRAME_HEADER;
  frame_index = 0;
  HK.frame.CRC = 0x9876;

LOOP:
    if (!block_open) {
      if (HK_frame_first_block_link == 0xff) {
        HK_frame_first_block_link = frame_index;
        if (HK_frame_ID == 15) HK_frame_first_block_link=1;    // some more error
        HK.frame.header = FRAME_HEADER;
      }
      get_AMS_block();
      if ((HK.frame.body[frame_index++] = bl_length) == 0) return;
      block_index = 0;
      block_open = TRUE;
    }
    while (block_index < bl_length && frame_index < HK_FRAME_BODY_LENGTH) {
      HK.frame.body[frame_index++] = bl_data[block_index++];
    }   
    if (block_index >= bl_length) block_open = FALSE;
    if (frame_index >= HK_FRAME_BODY_LENGTH) return;
  goto LOOP;
}    
//--------------------------------------------------------------------------
main() {
  FILE *output;
  int i, j;
  int first = TRUE;
  
  output = fopen("HKframes","w");
  while (1) {
    
    HK.frame.offset[0] = 0x1111;
    HK.frame.offset[1] = 0x2222;
    simulate_MCDS_data();

    frame_HK_frame();
    
    if (VERBOSE) {
      if (first) {
        first = FALSE;   // will be parser error
      }
      else {
        fprintf(output,"1 F811  OIU frame number command word\n");
      }
      fprintf(output,"3 %4.4x  OIU frame number data word : OIU frame number\n",HK_frame_ID);
    }
    else {
      if (first) {
        first = FALSE;   // will be parser error
      }
      else {
        fprintf(output,"1 F811  \n");
      }
      fprintf(output,"3 %4.4x \n",HK_frame_ID);
    }
    for (j=0;j<2;j++) {
      if (j==0) {
        if (VERBOSE) {
          fprintf(output,"1 2420  PDI frame box car #1 command word\n");
          fprintf(output,"2 1234  PDI frame box car #1 status word\n");
        }
        else {
          fprintf(output,"1 2420  \n");
          fprintf(output,"2 1234  \n");
        }
      }
      if (j==1) {
        if (VERBOSE) {
          fprintf(output,"1 2440  PDI frame box car #2 command word\n");
          fprintf(output,"2 1234  PDI frame box car #2 status word\n");
        }
        else {
          fprintf(output,"1 2440  \n");
          fprintf(output,"2 1234  \n");
        }
      }
      for (i=0;i<32;i++) {
        if (VERBOSE) {
          fprintf(output,"3 %4.4x  PDI frame box car #%d data word",HK.box_car[j][i],j+1);
        }
        else {
          fprintf(output,"3 %4.4x  ",HK.box_car[j][i]);
        }
        if (j==0 && i==0) {
          if (VERBOSE) {
            fprintf(output," : AMS Health Discreets & GPC Cmd Cntr");
          }
        }
        if (j==0 && i==1) {
          if (VERBOSE) {
            fprintf(output," : AMS Cmd Cntr         & Free");
          }
        }
        if (j==0 && i==2) {
          if (VERBOSE) {
            fprintf(output," : AMS HK frame header");
          }
        }
        fprintf(output,"\n");
      }
    }
    if (EOF_encountered) {
      printf("nb_of_frames_framed = %d\n", nb_of_frames_framed);
      exit(0);
    }
  }
}


