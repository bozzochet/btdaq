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
  return 0;
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
  return 0;
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


int  VCon_root::getindex(AMSlink* aa){
 if(!ev)  return 0;
 if( strstr(contname,"TrMCCluster"))
   for(int ii=0;ii<(int)ev->fTrMCClusters.size();ii++)
     if(ev->fTrMCClusters[ii]==aa) return ii;
   

  if( strstr(contname,"TrCluster"))
   for(int ii=0;ii<(int)ev->fTrClusters.size();ii++)
     if(ev->fTrClusters[ii]==aa) return ii;

  if( strstr(contname,"TrRawCluster"))
   for(int ii=0;ii<(int)ev->fTrRawClusters.size();ii++)
     if(ev->fTrRawClusters[ii]==aa) return ii;

  if( strstr(contname,"TrRecHit"))
   for(int ii=0;ii<(int)ev->fTrRecHits.size();ii++)
     if(ev->fTrRecHits[ii]==aa) return ii;

  if( strstr(contname,"TrTrack"))
   for(int ii=0;ii<(int)ev->fTrTracks.size();ii++)
     if(ev->fTrTracks[ii]==aa) return ii;

  return -1;
}

void VCon_root::removeEl(AMSlink* aa, integer)
{
  // remove next element !!!

   if(!ev)  return ;
  if( strstr(contname,"TrMCCluster"))
    ev->RemoveTrMCCluster(getindex((TrMCClusterR*)aa)+1);
  if( strstr(contname,"TrCluster"))
    ev->RemoveTrCluster(getindex((TrClusterR*)aa)+1);
  if( strstr(contname,"TrRawCluster"))
    ev->RemoveTrRawCluster(getindex((TrRawClusterR*)aa)+1);
  if( strstr(contname,"TrRecHit"))
    ev->RemoveTrRecHit(getindex((TrRecHitR*)aa)+1);
  if( strstr(contname,"TrTrack"))
    ev->RemoveTrTrack(getindex((TrTrackR*)aa)+1);
}
