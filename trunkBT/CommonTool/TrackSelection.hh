/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

bool CleanEvent(Event* ev, RHClass *rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS=0, int safetyK=0);
bool ChargeSelection(Event *_ev, RHClass *_rh,float charge_center, float lower_limit, float higher_limit); 
