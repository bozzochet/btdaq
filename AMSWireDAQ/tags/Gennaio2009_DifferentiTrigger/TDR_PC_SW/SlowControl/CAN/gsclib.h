// file gsclib.h
//
// see gsclib.c
//

#ifndef _GSCLIB_H
#define _GSCLIB_H

#include "mylib.h"

//~----------------------------------------------------------------------------

#define IDLE           0
#define BLOCK_LENGTH_1 1
#define BLOCK_LENGTH_2 2
#define BLOCK_BODY     3

typedef struct _AMSframe {
  int16 dat[2048];     // AMS Frame is placed here
  int16 err;
  bool  shf;           // Secondary Frame Header flag
  int16 APID;
  int16 len;           // Packet Length in 16-bit words
  int32 time;          // Packet Time
  int8  timeID;
  bool  chk;
  bool  ZOE;
  int16 seq;           // Frame Sequence number from CCSDS header
  int16 SEQ;           // Frame Sequence number from AMS block header (for ISS1553, APID=876 only)
  bool  rnd;           // Randomize Flag
  bool  crc;           // CRC present Flag
  bool  ext;           // External Command Flag
  bool  CHD;           // CHD present Flag
  int16 lnk;           // Link to 1St AMS Block
  int16 CRC;
  int16 CHK;
  int   bgn;           // 
  int   end;
  int   ind;
  bool  wri;
} AMSframe, *pAMSframe;

typedef struct _AMSblock {
  int   sta;           // state of deframing state machine
  int16 was;           // previous Frame Sequence number
  int32 wc;            // block length in bytes
  int16 dat[1000000];
  int   ind;
} AMSblock, *pAMSblock;

typedef struct _AMSCHD {
  time_t time;
  int16  APID;
  int16  seq;
  int8   dat[10];
} AMSCHD, *pAMSCHD;

char file_name[80];
int32 frame_number;

//~----------------------------------------------------------------------------

//~-- General library ---

void remove_trailing_slash(char *s);
FILE *open_file_for_reading(char *directory, int file);
FILE *open_temp_file_for_writing(char *directory, int temp_file);
void rename_temp_file(char *directory, int temp_file, int file);
FILE *open_perm_file_for_writing(char *directory, int file);
int last_file_in(char *DirPath, int n);
int the_last_file_in(char *directory);

bool read_AMS_block(FILE *file, int16 *dat, int32 *len);
bool unpack16_AMS_block(int16 *dat, int32 len, bool shf, AMSBlock *pBlock);

//~-- Specific deframing library ---

bool get_frame_from_file(FILE *file, pAMSframe p);
bool check_frame(pAMSframe p, pAMSblock pp);
void extract_CHD(pAMSframe p, FILE *file);
void deframe_frame(pAMSframe p, pAMSblock pp, FILE *file);

//~-- Reed-Solomon encoding/decoding ---

void initialize_RS( const int k);
void encode_RS(unsigned char data[/*K*/], unsigned char T[/*N-K*/]);
int decode_RS(unsigned char data[/*N*/]);

//~============================================================================

#endif // _GSCLIB_H
