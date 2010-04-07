// file uscmsimlib.h
//
// See uscmsimlib.c
//
// A.Lebedev - Mar 2007...
//

//~----------------------------------------------------------------------------

#ifndef _USCMSIMLIB_H
#define _USCMSIMLIB_H

//~----------------------------------------------------------------------------

#include "uscmlib.h"
 
//~----------------------------------------------------------------------------

typedef struct dts {
  uint8  id[8];         // sensor id
  uint8  priority;      // measure frequence, 0 is never, 9 is always
  uint32 tstamp;        // time stamp
  uint8  lsb;           // raw data
  uint8  msb;           // raw data
  uint8  cfg;           // raw data
  uint8  remain;        // raw data
  uint8  perc;          // raw data
  int8   temp1;         // temperature in 1 byte format
  int16  temp2;         // temperature in 2 byte format
} DTS_T;

int8  DtsNumb[10];
DTS_T DtsTable[640];
 
void DebugPrnMsg(char *msg);
void DebugPrnVal(volatile void *val, uint8 nbytes);

int32 TimeGet(void);
int32 TimeLapse(int32 tstamp);

int16 DacRead(int8 cha);
void DacWrite(int8 cha, int16 val);

int16 AdcRead(int8 cha);

bool LvdsRead(int8 adr, volatile int16 *val);
bool LvdsWrite(int8 adr, int16 val);

void DTS_simulated_task(void);

//~=============================================================================

#endif // _USCMSIMLIB_H
