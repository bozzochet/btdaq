#ifndef TrackSelection_hh
#define TrackSelection_hh

/* from the 'Decode' API */
#include "GenericEvent.hpp"
/* end */

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

template <class Event, class RH> class TrackSelection {
private:
  // int is jinfnum*100+tdrnum
  std::vector<int> _v_ladderS_to_ignore;//!
  std::vector<int> _v_ladderK_to_ignore;//! 
  
public:
  
  void ExcludeTDR(Event* ev, int jinfnum, int tdrnum, int side);//to be called just one, before event loop
  bool CleanEvent(Event* ev, RH* rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS=0, int safetyK=0);
  bool ChargeSelection(Event* ev, RH* rh,float charge_center, float lower_limit, float higher_limit); 
  
};

#endif
