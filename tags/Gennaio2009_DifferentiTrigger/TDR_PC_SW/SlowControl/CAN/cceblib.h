// file cceblib.h
//
// see cceblib.c
//
// A.Lebedev Jun-2007...

#ifndef _CCEBLIB_H
#define _CCEBLIB_H

#include "uscmlib.h"

struct _DAC {
  char *nam;
};

struct _ADC {
  char *nam;
  float A;
  float B;
};

struct _CCBT {
  char *nam;
  float A;
  float B;
};

struct _cernox {
  char *nam;
  int n;
  struct {
    real T;
    real R;
  } _[1000];
};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int16 cernox_T_to_code(real T,   real M, struct _cernox *Table);
real cernox_code_to_T(int16 cod, real M, struct _cernox *Table);

//~============================================================================

#endif // _CCEBLIB_H
