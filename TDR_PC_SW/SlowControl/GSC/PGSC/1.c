// file 1.c
//
// creates the file to be used in 1553 BUS CONTROLLER tests
//

#include <stdio.h>
#include <stdlib.h>

#define integer16 unsigned short
#define MAX_AMS_COMMAND_BLOCK_LENGTH 61         // 16-bit words

integer16                  AMS_command_block[MAX_AMS_COMMAND_BLOCK_LENGTH+2];
#define block_length       AMS_command_block[0]
#define block_ID           AMS_command_block[1]
#define command_ID         AMS_command_block[2]
integer16 *command_time = &AMS_command_block[3];
integer16 *block_body   = &AMS_command_block[5];
#define block_CRC          AMS_command_block[block_length+1]  // it is outside block !

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

//------------------------------------------------------------------------------

void main( void) {
  FILE *file;
  int BOL = 1;
  int i, j, k, l, n = 0;
  file = fopen( "1000", "w");
  for (k=0; k<1000; k++) {
    fprintf( file, "// PING MCC\n");
    block_length    = 56 + 4;
    block_ID        = 0x7000;
    command_ID      = k + 1;
    command_time[0] = 0;
    command_time[1] = 0;
    fprintf( file, " %X\n", block_length);
    fprintf( file, " %4.4X %4.4X %4.4X %4.4X \n", 
            block_ID, command_ID, command_time[0], command_time[1]);
    l = 0;
    for (j=0; j<8; j++) {
      for (i=0; i<8; i++) {
        if (l >= block_length - 4) break;
        if (l == k % 12) block_body[l] = 0xFFFF;
        else             block_body[l] = n;
        fprintf( file, " %4.4X", block_body[l]);
        BOL = 0;
        l++;
        n++;
        if (i == 7) {
          fprintf( file, "\n");
          BOL = 1;
        }
      }
    }
    fill_block_CRC();
    if (!BOL) fprintf( file, "\n");
    fprintf( file, " %4.4X\n", block_CRC);
  }
}
