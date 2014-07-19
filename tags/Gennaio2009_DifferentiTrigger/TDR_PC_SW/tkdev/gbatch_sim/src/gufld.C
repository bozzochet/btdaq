
#include "TrField.h"

// GEANT part
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"




PROTOCCALLSFSUB2(GUFLD,gufld,FLOATV,FLOATV)
#define GUFLD(A1,A2)  CCALLSFSUB2(GUFLD,gufld,FLOATV,FLOATV,A1,A2)




/// Get field value
extern "C" void gufld_(float *x, float *b) {
  TrField::GetPtr()->GuFld(x, b);
}


