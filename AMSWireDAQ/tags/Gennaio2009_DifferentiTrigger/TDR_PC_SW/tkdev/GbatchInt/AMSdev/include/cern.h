//  $Id: cern.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __CERN__
#define __CERN__
#include "typedefs.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

//
// GEANT part
#include "hbook.h"
#include "geant321.h"
#include "geant321g.h"

//CERNLIB part
//PROTOCCALLSFSUB3(UCOPY,ucopy,FLOATV,FLOATV,INT)
//#define UCOPY(A,B,I) CCALLSFSUB3(UCOPY,ucopy,FLOATV,FLOATV,INT,A,B,I)
extern "C" void timel_(float & );
extern "C" void timex_(float & t);
extern "C" void abinelget_(int &t,geant coo[]);
#define TIMEX timex_  
#define TIMEL timel_  
extern "C" void ucopy_(const void *, void*, const int &);
#define UCOPY ucopy_
extern "C" void ucopy2_(void *, void*, const int &);
#define UCOPY2 ucopy2_
extern "C" void vzero_(void *,  const int &);
#define VZERO  vzero_
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
  PROTOCCALLSFSUB4(FFKEY,ffkey,STRING,FLOATV,INT,STRING)
#define FFKEY(A1,A2,A3,A4) CCALLSFSUB4(FFKEY,ffkey,STRING,FLOATV,INT,STRING,A1,A2,A3,A4)

  PROTOCCALLSFFUN1(DOUBLE,DERFC,derfc,DOUBLE)
#define DERFC(A2)  CCALLSFFUN1(DERFC,derfc,DOUBLE,A2)

  PROTOCCALLSFFUN1(FLOAT,FREQ,freq,FLOAT)
#define FREQ(A2) CCALLSFFUN1(FREQ,freq,FLOAT,A2)


  PROTOCCALLSFSUB2(VBLANK,vblank,INTV,INT)
#define VBLANK(A,I) CCALLSFSUB2(VBLANK,vblank,INTV,INT,A,I)
  PROTOCCALLSFSUB4(UHTOC,uhtoc,INTV,INT,PSTRING,INT)
#define UHTOC(H,IH,S,IS) CCALLSFSUB4(UHTOC,uhtoc,INTV,INT,PSTRING,INT,H,IH,S,IS)
  PROTOCCALLSFSUB4(UCTOH,uctoh,PSTRING,INTV,INT,INT)
#define UCTOH(H,IH,S,IS) CCALLSFSUB4(UCTOH,uctoh,PSTRING,INTV,INT,INT,H,IH,S,IS)
// some Minuit routines :
  PROTOCCALLSFSUB3(MNINIT,mninit,INT,INT,INT)
#define MNINIT(I1,I2,I3) CCALLSFSUB3(MNINIT,mninit,INT,INT,INT,I1,I2,I3)

  PROTOCCALLSFSUB1(MNSETI,mnseti,STRING)
#define MNSETI(COMMENT) CCALLSFSUB1(MNSETI,mnseti,STRING,COMMENT)

  PROTOCCALLSFSUB7(MNPARM,mnparm,INT,STRING,DOUBLE,DOUBLE,DOUBLE,DOUBLE,PINT)
#define MNPARM(IP,PN,PI,PS,PL,PH,IE) CCALLSFSUB7(MNPARM,mnparm,INT,STRING,DOUBLE,DOUBLE,DOUBLE,DOUBLE,PINT,IP,PN,PI,PS,PL,PH,IE)

  PROTOCCALLSFSUB6(MNEXCM,mnexcm,ROUTINE,STRING,DOUBLEV,INT,PINT,INT)
#define MNEXCM(FCN,CN,AL,NA,IE,IF) CCALLSFSUB6(MNEXCM,mnexcm,ROUTINE,STRING,DOUBLEV,INT,PINT,INT,FCN,CN,AL,NA,IE,IF)

  PROTOCCALLSFSUB7(MNPOUT,mnpout,INT,PSTRING,PDOUBLE,PDOUBLE,PDOUBLE,PDOUBLE,PINT)
#define MNPOUT(IP,PN,PI,PS,PL,PH,IE) CCALLSFSUB7(MNPOUT,mnpout,INT,PSTRING,PDOUBLE,PDOUBLE,PDOUBLE,PDOUBLE,PINT,IP,PN,PI,PS,PL,PH,IE)


  
// Private part  
//
  namespace amsprotected{
    extern void mm3( number [3][3],   number [3][3], int choise);
    extern void transpose( number [][3],   number [][3]);
  }
PROTOCCALLSFSUB1(CLOSEF,closef,INT)
#define CLOSEF(I1) CCALLSFSUB1(CLOSEF,closef,INT,I1)

#endif
