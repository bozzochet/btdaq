// file uscmdef.h
// 
// USCM Logical ID
// 
// A.Lebedev Feb-2006...
// 
 
//~---------------------------------------------------------------------------- 
 
#ifndef _USCMDEF_H 
#define _USCMDEF_H 
 
//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#include "mylib.h"

typedef struct {
  int8 ID;
  char *name;
  bool ok;
} _USCM_table;

static _USCM_table USCM_table[] = {

   {0x00, "JMDC-0-CAN", 1},
   {0x01, "JMDC-1-CAN", 1},
   {0x02, "JMDC-2-CAN", 1},
   {0x03, "JMDC-3-CAN", 1},
   {0x18, "CAB-A",      1},
   {0x19, "CAB-B",      1},
   {0x1A, "CAB-P",      0},
   {0x1B, "CAB-S",      0},
   {0x1C, "CC-Crate-A", 1},
   {0x1D, "CC-Crate-B", 1},
   {0x1E, "CC-Crate-P", 0},
   {0x1F, "CC-Crate-S", 0},
   {0x28, "JPD-A",      1},
   {0x29, "JPD-B",      1},
   {0x2A, "JPD-P",      0},
   {0x2B, "JPD-S",      0},
   {0x2C, "M-Crate-A",  1},
   {0x2D, "M-Crate-B",  1},
   {0x2E, "M-Crate-P",  0},
   {0x2F, "M-Crate-S",  0},
   {0x30, "PDS-A",      1},
   {0x31, "PDS-B",      1},
   {0x32, "PDS-P",      0},
   {0x33, "PDS-S",      0},
   {0x6C, "TTCE-A",     1},
   {0x6D, "TTCE-B",     1},
   {0x6E, "TTCE-P",     0},
   {0x6F, "TTCE-S",     0},
   {0x78, "UG-Crate-A", 1},
   {0x79, "UG-Crate-B", 1},
   {0x7A, "UG-Crate-P", 0},
   {0x7B, "UG-Crate-S", 0}};

#endif // _USCMDEF_H 

//~============================================================================
