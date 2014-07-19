// file cceblib.c
//
// A library of functions needed to run CCEB and CC
//
// A.Lebedev Jun-2007...

#include "cceblib.h"

//~============================================================================

int16 cernox_T_to_code(real T, real M, struct _cernox *Table) {

  int i;
  real x = T;
  real x1 = 0, x2 = 0, y = 0, y1 = 0, y2 = 0;
  
  for (i=0; i<Table->n-1; i++) {
    x1 =       Table->_[i  ].T;
    x2 =       Table->_[i+1].T;
    y1 = 1.0 / Table->_[i  ].R;
    y2 = 1.0 / Table->_[i+1].R;
    if (x < x1 || (x1 <= x && x < x2)) break;
  }

  y = y1 + (y2 - y1) / (x2 - x1) * (x - x1);
  return MIN(0x0FFF, (int16)(M * y));
}

//~----------------------------------------------------------------------------

real cernox_code_to_T(int16 cod, real M, struct _cernox *Table) {

  int i;
  real x = ((real)cod + 0.5) / M;
  real x1 = 0, x2 = 0, y = 0, y1 = 0, y2 = 0;

  for (i=0; i<Table->n-1; i++) {
    x1 = 1.0 / Table->_[i  ].R;
    x2 = 1.0 / Table->_[i+1].R;
    y1 =       Table->_[i  ].T;
    y2 =       Table->_[i+1].T;
    if (x < x1 || (x1 <= x && x < x2)) break;
  }
  y = y1 + (y2 - y1) / (x2 - x1) * (x - x1);
  return MAX(0, y);
}

//~============================================================================
