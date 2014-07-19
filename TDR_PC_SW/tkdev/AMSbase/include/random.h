#include "typedefs.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

#ifdef __G4AMS__

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoissonQ.h"
extern "C" void poissn_(float &, int &, int&);
PROTOCCALLSFFUN1(FLOAT,RNDM,rndm,FLOAT)
#define RNDMG3(A) CCALLSFFUN1(RNDM,rndm,FLOAT,A)
#define RNDM(A) (MISCFFKEY.G4On?(RandFlat::shoot()):RNDMG3(A))
#define POISSN(A,B,C) if(MISCFFKEY.G4On){C=0;B=RandPoissonQ::shoot(A);} else poissn_(A,B,C)

#else

extern "C" void poissn_(float &, int &, int&);
#define POISSN poissn_
PROTOCCALLSFFUN1(FLOAT,RNDM,rndm,FLOAT)
#define RNDM(A) CCALLSFFUN1(RNDM,rndm,FLOAT,A)

#endif
