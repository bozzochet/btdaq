#include "TrackSelection.hh"

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

bool CleanEvent(Event* ev, RHClass *rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS, int safetyK){

  int NClusTot = ev->GetNClusTot();
  if(NClusTot<(minclus-1) ||  NClusTot>(maxclus+1)) return false;
  
  int nclusS[NJINF*NTDRS];
  int nclusK[NJINF*NTDRS];
  memset(nclusS, 0, sizeof(nclusS[0])*NJINF*NTDRS);
  memset(nclusK, 0, sizeof(nclusK[0])*NJINF*NTDRS);

  int safetySspent = safetyS;
  int safetyKspent = safetyK;
  
  for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
    Cluster *cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, rh->FindPos(ladder));
    int side=cl->side;
    if (side==0) {
      nclusS[rh->FindPos(ladder)]++;
      if (nclusS[rh->FindPos(ladder)]>=(perladdS+safetyS)) {
	safetySspent--;
	if (safetySspent<0) return false;
      }
    }
    else {
      nclusK[rh->FindPos(ladder)]++;
      if (nclusK[rh->FindPos(ladder)]>=(perladdK+safetyK)) {
	safetyKspent--;
	if (safetyKspent<0) return false;
      }
    }
  }

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
bool ChargeSelection(Event *ev, RHClass *_rh, float charge_center, float lower_limit, float higher_limit){
  bool chargeselection=false;
  
  float charge[NTDRS];
  for (int ii=0; ii<NTDRS; ii++) {
    charge[ii] = 0.0;
  }
  
  for (int index_cluster=0; index_cluster<ev->GetNClusTot(); index_cluster++) {
    //    if (!ev->IsClusterUsedInTrack(index_cluster)) continue;
    
    Cluster *_cl = ev->GetCluster(index_cluster);
    int ladder = _cl->ladder;
    
    //    printf("%d --> %d\n", ladder, _rh->tdrCmpMap[ladder]);
    // printf("%d --> %d\n", ladder, _rh->FindPos(ladder));
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
