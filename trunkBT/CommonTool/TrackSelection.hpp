#include "TrackSelection.hh"
#include <algorithm>
#include <unistd.h>

template <class Event, class RH>
void TrackSelection<Event, RH>::ExcludeTDR(Event* ev, int jinfnum, int tdrnum, int side){
  
  ev->ExcludeTDRFromTrack(jinfnum, tdrnum, side);

  int item = jinfnum*100+tdrnum;
  
  if (side==0) {
    _v_ladderS_to_ignore.push_back(item);
  }
  else {
    _v_ladderK_to_ignore.push_back(item);
  }
  
  return;
}

template <class Event, class RH>
bool TrackSelection<Event, RH>::CleanEvent(Event* ev, RH* rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS, int safetyK){

  static int NJINF = Event::GetNJINF();
  static int NTDRS = Event::GetNTDRS();
  
  PRINTDEBUG;
  
  int NClusTot = ev->GetNClusTot();
  if (NClusTot<1) return false;
  //  if (NClusTot<minclus || NClusTot>maxclus) return false; //we have to count just the one not excluded
  int NClusTot_notexcl = 0;

  PRINTDEBUG;
  
  int nclusS[NJINF*NTDRS];
  int nclusK[NJINF*NTDRS];
  memset(nclusS, 0, sizeof(nclusS[0])*NJINF*NTDRS);
  memset(nclusK, 0, sizeof(nclusK[0])*NJINF*NTDRS);

  PRINTDEBUG;
  
  int safetySspent = safetyS;
  int safetyKspent = safetyK;

  PRINTDEBUG;
  
  for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {

    PRINTDEBUG;
    
    Cluster *cl = ev->GetCluster(index_cluster);

    PRINTDEBUG;
    
    int ladder = cl->ladder;
    //    printf("%d --> %d\n", ladder, rh->FindPos(ladder));

    int jinfnum = cl->GetJinf();
    int tdrnum = cl->GetTDR();
    int item = jinfnum*100+tdrnum;

    PRINTDEBUG;
    
    int side=cl->side;

    //    printf("%d %d %d %d\n", ladder, jinfnum, tdrnum, side);

    PRINTDEBUG;

    if (side==0) {
      if (std::find(_v_ladderS_to_ignore.begin(), _v_ladderS_to_ignore.end(), item)!=_v_ladderS_to_ignore.end()) continue;
    }
    else {
      if (std::find(_v_ladderK_to_ignore.begin(), _v_ladderK_to_ignore.end(), item)!=_v_ladderK_to_ignore.end()) continue;
    }

    NClusTot_notexcl++;

    //    printf("AFTER: %d %d %d %d\n", ladder, jinfnum, tdrnum, side);

    PRINTDEBUG;
    
    int ladder_pos=rh->FindPos(ladder);
    if (ladder_pos>NJINF*NTDRS) {
      printf("WTF? ladder_pos is %d out of %d\n", ladder_pos, NJINF*NTDRS);
      sleep(10);
    }
    
    PRINTDEBUG;
    
    if (side==0) {
      PRINTDEBUG;
      nclusS[ladder_pos]++;
      PRINTDEBUG;
      if (nclusS[ladder_pos]>=(perladdS+safetyS)) {
	PRINTDEBUG;
	safetySspent--;
	PRINTDEBUG;
	if (safetySspent<0) return false;
	PRINTDEBUG;
      }
    }
    else {
      PRINTDEBUG;
      nclusK[ladder_pos]++;
      PRINTDEBUG;
      if (nclusK[ladder_pos]>=(perladdK+safetyK)) {
	PRINTDEBUG;
	safetyKspent--;
	PRINTDEBUG;
	if (safetyKspent<0) return false;
	PRINTDEBUG;
      }
    }

    PRINTDEBUG;
    
  }

  //  if (NClusTot<minclus || NClusTot>maxclus) printf("%d -> %d\n", NClusTot, NClusTot_notexcl);
  //  if (NClusTot>maxclus) printf("%d -> %d\n", NClusTot, NClusTot_notexcl);
  if (NClusTot_notexcl<minclus || NClusTot_notexcl>maxclus) return false;

  PRINTDEBUG;
  
  /*
  for (int jj=0; jj<NJINF*NTDRS; jj++) {
    printf("%d %d\n", nclusS[jj], nclusK[jj]);
  }
  */
  
  return true;
}

// GENERALIZE ME! IS ALSO WRONG:
// - IF FOR ONE LADDER THERE ARE TWO CLUSTERS ONLY THE SECOND IS CONSIDERED.
// - IF ON LADDER HAS NO CLUSTERS THE CUT IS NOT PASSED
// - IT ASSUMES THAT ONLY ONE JINF IS PRESENT (THIS MAYBE IS SAFE AT THIS LEVEL)
template <class Event, class RH>
bool TrackSelection<Event, RH>::ChargeSelection(Event* ev, RH* rh, float charge_center, float lower_limit, float higher_limit){
  bool chargeselection=false;

  static int NJINF = Event::GetNJINF();
  static int NTDRS = Event::GetNTDRS();
  
  float charge[NTDRS];
  for (int ii=0; ii<NTDRS; ii++) {
    charge[ii] = 0.0;
  }
  
  for (int index_cluster=0; index_cluster<ev->GetNClusTot(); index_cluster++) {
    //    if (!ev->IsClusterUsedInTrack(index_cluster)) continue;
    
    Cluster *_cl = ev->GetCluster(index_cluster);
    int ladder = _cl->ladder;
    
    //    printf("%d --> %d\n", ladder, rh->tdrCmpMap[ladder]);
    // printf("%d --> %d\n", ladder, rh->FindPos(ladder));
    charge[ladder]=_cl->GetCharge();
  }
  if(    ((charge[0] > (charge_center-lower_limit)) && (charge[0] < (charge_center+higher_limit)))
	 && ((charge[4] > (charge_center-lower_limit)) && (charge[4] < (charge_center+higher_limit)))
	 && ((charge[8] > (charge_center-lower_limit)) && (charge[8] < (charge_center+higher_limit)))
	 && ((charge[14] > (charge_center-lower_limit)) && (charge[14] < (charge_center+higher_limit)))
	 )
    chargeselection=true;
  
  return chargeselection;
}
