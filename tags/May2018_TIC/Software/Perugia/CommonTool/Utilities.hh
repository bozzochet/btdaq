#ifndef Utilities_hh
#define Utilities_hh

#include "TChain.h"
#include "TList.h"

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

RHClass* GetRH(TChain* chain);

#endif
