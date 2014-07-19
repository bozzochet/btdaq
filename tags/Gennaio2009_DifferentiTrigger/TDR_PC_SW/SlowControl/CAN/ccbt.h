#ifdef BUILD_FOR_CC

// file ccbt.h
//
// A.Lebedev Dec 2006 - ...
//

#ifndef _CCBT_H_
#define _CCBT_H_

//~--- State Machine States ---

#define CCBT_START 1
#define CCBT_READ  2

//~--- CCBT Addresses ---

#define CCBT0_STATUS 2
#define CCBT0_DATA   3
#define CCBT1_STATUS 6
#define CCBT1_DATA   7

//~--- Data Types Definition ---

typedef struct {
  bool   use;
  uint8  state;
  uint32 timer;
  bool   ok;
  uint32 tstamp;
  uint16 value[32];
} _ccbt;

//~--- Global Functions ---

extern void read_CCBT_data(void);
extern void initialize_CCBT_task(void);
extern void CCBT_task(void);

#endif // _CCBT_H_

//~============================================================================

#endif   // #ifdef BUILD_FOR_CC

