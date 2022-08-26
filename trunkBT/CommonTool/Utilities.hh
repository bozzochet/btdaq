#ifndef Utilities_hh
#define Utilities_hh

#include "TChain.h"
#include "TList.h"

/* from the 'Decode' API */
#include "GenericEvent.hpp"
/* end */

template <class Event, class RH> class Utilities {
  
public:

  static RH* GetRH(TChain* chain);

};
  
#endif
