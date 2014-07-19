// file manwoo-2.c
//
// Back and Forth Conversion Studies for CERNOX Sensors
//
// A.Lebedev Jun-2007...

#include "ccebdef.h"

struct _ADC *adc;

_cernox_calibration *Ref_Table;
_cernox_calibration Table;

//~----------------------------------------------------------------------------
#if 0
int16 cernox_T_to_code(real T, real M, _cernox_calibration *Table) {

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

real cernox_code_to_T(int16 cod, real M, _cernox_calibration *Table) {

  int i;
  real x = (real)cod / M;
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
#endif
//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, j, k, n;
  int16 cod;
  real R, T, TT;

  adc = &EM1_ADC[0];

  for (j=0; j<sizeof(ref_table)/sizeof(ref_table[0]); j++) {
    Ref_Table = &ref_table[j];
    n = Ref_Table->n;
    printf("CERNOX %s --------------------\n", Ref_Table->nam);
    Table.n = n / 2 + 1;
    for (i=0, k=0; k<=n/2; i+=2, k++) {
      Table._[k].T = Ref_Table->_[i].T;
      Table._[k].R = Ref_Table->_[i].R;
    }
    for (i=0; i<n; i++) {
      T   = Ref_Table->_[i].T;
      R   = Ref_Table->_[i].R;
      cod = cernox_T_to_code(T,   adc[CC0_COLD_TIP].A, &Table);
      TT  = cernox_code_to_T(cod, adc[CC0_COLD_TIP].A, &Table);
      printf("i = %2d, T = %7.3Lf, R = %7.3Lf, cod = %4d(%04X), TT = %7.3Lf, T-TT = %8.6Lf\n",
              i,       T,          R,          cod,       cod,  TT,          T-TT);
    }
  }
  
  return 0;
}

//~============================================================================
