#ifndef Utilities_hh
#define Utilities_hh

#include "TChain.h"
#include "TList.h"

/* from the 'Decode' API */
#include "GenericEvent.hpp"
#include "LadderConf.hh"
/* end */

template <class Event, class RH> class Utilities {
  
public:

  static RH* GetRH(TChain* chain);
  static LadderConf* GetLadderConf(TChain* chain);

};
  
#endif
