// file uscmsimlib.c
//
// Program to simulate internal USCM functions.
// May be helpfull to develop internal USCM software
//
// A.Lebedev - Mar 2007...
//

#include "uscmsimlib.h"

//~=============================================================================

void DebugPrnMsg(char *msg) {

  printf(msg);
}

//~-----------------------------------------------------------------------------

void DebugPrnVal(volatile void *val, uint8 nbytes) {

  int i;
  int8 *byte = (int8*)val;
  
  for (i=nbytes-1; i>=0; i--) printf("%02hX", byte[i]);
}

//~=============================================================================
 
int32 TimeGet(void) {
 
  struct timeval now;
                                                                                                                                             
  gettimeofday(&now, NULL);
                                                                                                                                             
  return (1000 * (now.tv_sec & 0x003FFF) + now.tv_usec / 1000);
}
                                                                                                                                             
//~-----------------------------------------------------------------------------
                                                                                                                                             
int32 TimeLapse(int32 tstamp) {
                                                                                                                                             
  struct timeval now;
                                                                                                                                             
  gettimeofday(&now, NULL);
                                                                                                                                             
  return (1000 * (now.tv_sec & 0x003FFF) + now.tv_usec / 1000 - tstamp);
}
                                                                                                                                             
//~=============================================================================

int16 DacRead(int8 cha) {

  int16 err;
  int16 dat[16];

  if (cha > 15) PANIC("Wrong cha in DacRead.\n");
  read_DAC(dat, &err);
  if (err) PANIC("DacRead failed.\n");

  return dat[cha];
}

//~-----------------------------------------------------------------------------

void DacWrite(int8 cha, int16 val) {

  int i;
  int16 err;
  int16 dat[16];
  
  for (i=0; i<16; i++) dat[i] = (i == cha) ? (val & 0xFFF) : 0x8000;
  
  if (cha > 15) PANIC("Wrong cha in DacWrite.\n");
  write_DAC(dat, &err);
  if (err) PANIC("DacWrite failed.\n");
}

//~=============================================================================

int16 AdcRead(int8 cha) {

  int16 err;
  int16 dat[32];

  if (cha > 31) PANIC("Wrong cha in AdcRead.\n");
  read_ADC(dat, &err);
  if (err) PANIC("AdcRead failed.\n");

  return dat[cha];
}

//~=============================================================================

bool LvdsRead(int8 adr, volatile int16 *val) {

  int16 err;
  int16 dat;

  read_LVDS_bus(adr, &dat, &err);
  if (err && err <= 0x0006) PANIC("LvdsRead failed.\n");
  
  if (err) return 1;
  
  *val = dat;
  return 0;
}
                                                                                                                                             
//~-----------------------------------------------------------------------------

bool LvdsWrite(int8 adr, int16 val) {

  int16 err;

  write_LVDS_bus(adr, val, &err);
  if (err && err <= 0x0006) PANIC("LvdsWrite failed.\n");
  
  if (err) return 1;
  else     return 0;
}

//~=============================================================================

void DTS_simulated_task(void) {

  bool debug = FALSE;
  int i, j;
  bool ok[64];
  float tmp[64];
  int8 bus[4] = {1, 2, 5, 6};
  int16 err;
  
  for (i=0; i<4; i++) {
    int k = bus[i];
    read_DS1820_temp_short(k, &DtsNumb[k], ok, tmp, &err);
    if (err && err <= 0x0006) PANIC("DTS_simulated_task failed.\n");
    if (debug) printf("%d %2d: ", k, DtsNumb[k]);
    for (j=0; j<64; j++) {
      int index = 64 * k + j;
      DtsTable[index].tstamp = TimeGet();
      DtsTable[index].temp1  = (j < DtsNumb[k] && ok[j]) ? tmp[j] : 0x80;
      if (debug) printf("%+4d ", DtsTable[index].temp1);
    }
    if (debug) printf("\n");
  }
  if (debug) printf("\n");
}

//~=============================================================================
