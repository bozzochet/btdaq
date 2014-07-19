//  $Id: gbatch.C,v 1.1 2008/09/11 17:19:22 zuccon Exp $
#include "typedefs.h"
#include <iostream>
#include <signal.h>
#include <unistd.h> 
#include <new>
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"
#include "hbook.h"
#include "geant321.h"
#include "geant321g.h"
#include "geantnamespace.h"

const int NWGEAN=13000000;
const int NWPAW=1200000;

struct PAWC_DEF{
  float q[NWPAW];
};
struct GCBANK_DEF{
  float q[NWGEAN];
};
//
#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);
GCBANK_DEF GCBANK;

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
 
int main(int argc, char * argv[] ){
  using namespace gams;
  
  // Initialize Geant and HBOOK Zebra system
  GZEBRA(NWGEAN);
  HLIMIT(-NWPAW);
  
  //User Geant init
  UGINIT(argc,argv);
  // Go for A Geant simulation
  GRUN();
  // Termination phase
  UGLAST();
  return 0;
}

