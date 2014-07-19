//  $Id: geant3.C,v 1.1 2008/09/11 17:19:22 zuccon Exp $

#include "typedefs.h"
#include "TrRecon.h"
#include "cern.h"

#include "amsgobj.h"
#include "geant321g.h"


extern "C" void gustep_(){
 
  
  //  Tracker
  int lvl= GCVOLU.nlevel-1;  
  
  if(GCVOLU.nlevel>2 && GCTRAK.destep != 0 && GCTMED.isvol != 0 && 
     GCVOLU.names[2][0]== 'S' &&     GCVOLU.names[2][1]=='T' && 
     GCVOLU.names[2][2]=='K'){

    
    TrRecon::sitkhits(GCVOLU.number[lvl],GCTRAK.vect,
		      GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
    
    
  }
  
}

