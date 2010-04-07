#include "VCon_root.h"


VCon_root::VCon_root(char * cc,Event* eve){
  if(!cc) sprintf(contname,"empty");
  else
  sprintf(contname,"%s",cc);
  ev=eve;
}

VCon* VCon_root::GetCont(char * name){
  if(
     strstr(name,"TrMCCluster")||
     strstr(name,"TrCluster")||
     strstr(name,"TrRawCluster")||
     strstr(name,"TrRecHit")||
     strstr(name,"TrTrack")
     )
    return (VCon*)(new VCon_root(name,Event::currev));
  else return 0;

}

int VCon_root::getnelem(){
  if(!ev)  return -1;
  if( strstr(contname,"TrMCCluster"))
    return ev->fTrMCClusters.size();
  if( strstr(contname,"TrCluster"))
    return ev->fTrClusters.size();
  if( strstr(contname,"TrRawCluster"))
    return ev->fTrRawClusters.size();
  if( strstr(contname,"TrRecHit"))
    return ev->fTrRecHits.size();
  if( strstr(contname,"TrTrack"))
    return ev->fTrTracks.size();
}

void VCon_root::eraseC(){
 if(!ev)  return ;
  if( strstr(contname,"TrMCCluster"))
    return ev->ClearTrMCClusters(1);
  if( strstr(contname,"TrCluster"))
    return ev->ClearTrClusters(1);
  if( strstr(contname,"TrRawCluster"))
    return ev->ClearTrRawClusters(1);
  if( strstr(contname,"TrRecHit"))
    return ev->ClearTrRecHits(1);
  if( strstr(contname,"TrTrack"))
    return ev->ClearTrTracks(1);
}

AMSlink* VCon_root::getelem(int ii){
 if(!ev)  return 0;
  if( strstr(contname,"TrMCCluster"))
    return (AMSlink*) ev->fTrMCClusters[ii];
  if( strstr(contname,"TrCluster"))
    return  (AMSlink*) ev->fTrClusters[ii];
  if( strstr(contname,"TrRawCluster"))
    return  (AMSlink*) ev->fTrRawClusters[ii];
  if( strstr(contname,"TrRecHit"))
    return  (AMSlink*) ev->fTrRecHits[ii];
  if( strstr(contname,"TrTrack"))
    return  (AMSlink*) ev->fTrTracks[ii];
}

void  VCon_root::addnext(AMSlink* aa){
   if(!ev)  return ;
  if( strstr(contname,"TrMCCluster"))
    ev->AddTrMCCluster((TrMCClusterR*)aa);
  if( strstr(contname,"TrCluster"))
    ev->AddTrCluster((TrClusterR*)aa);
  if( strstr(contname,"TrRawCluster"))
    ev->AddTrRawCluster((TrRawClusterR*)aa);
  if( strstr(contname,"TrRecHit"))
    ev->AddTrRecHit((TrRecHitR*)aa);
  if( strstr(contname,"TrTrack"))
    ev->AddTrTrack((TrTrackR*)aa);
}
