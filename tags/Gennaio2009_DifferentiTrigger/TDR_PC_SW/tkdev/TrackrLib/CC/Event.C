#include "Event.h"

ClassImp(Event);
Event* Event::currev=0;
using namespace std;

Event::Event():TObject(){
  Clear();
  fTrMCClusters.reserve(20);
  fTrClusters.reserve(20);
  fTrRawClusters.reserve(20);
  fTrRecHits.reserve(20);
  fTrTracks.reserve(20);
  currev=this;
}

Event::Event(Event& orig):TObject(orig) {
  runnum          = orig.runnum;
  runtag          = orig.runtag;
  eventnum        = orig.eventnum;
  event_time      = orig.event_time;
  event_ftime     = orig.event_ftime;
  event_fftime    = orig.event_fftime;
  event_timeP     = orig.event_timeP;
  event_ftimeP    = orig.event_ftimeP;
  event_fftimeP    = orig.event_fftimeP;
  event_size      = orig.event_size;
  NTrRawClusters  = orig.NTrRawClusters;
  fTrRawClusters  = orig.fTrRawClusters;
  NTrMCClusters     = orig.NTrMCClusters;
  NTrClusters     = orig.NTrClusters;
  fTrClusters     = orig.fTrClusters;
  NTrRecHits      = orig.NTrRecHits;
  fTrRecHits      = orig.fTrRecHits;
  NTrTracks       = orig.NTrTracks;
  fTrTracks       = orig.fTrTracks;
  T1              = orig.T1;
  T2              = orig.T2;
  TofFlag1        = orig.TofFlag1;
}

void Event::Clear(int reallydelete) {
  runnum       = 0;
  runtag       = 0;
  eventnum     = 0;
  event_time   = 0;
  event_ftime  = 0;
  event_fftime  = 0;
  event_timeP  = 0;
  event_ftimeP = 0;
  event_fftimeP = 0;
  event_size   = 0;
  T1            =0;
  T2            =0;
  TofFlag1      =12;
  ClearTrMCClusters(reallydelete);
 ClearTrRawClusters(reallydelete);
 ClearTrClusters(reallydelete);
 ClearTrRecHits(reallydelete);
 ClearTrTracks(reallydelete);
}

Event::~Event(){
  Clear();
}

void Event::Info(){
  char msg[1024];
  sout.clear();
 
  // sprintf(msg, "Run: %10d\nTag: %X\nEvent: %7d\nFineTime: %d\nTime: %s \nnRawClusters: %3d\nnClusters: %3d\nnRecHits: %3d\nnTracks: %3d\n",
// 	  runnum, runtag, eventnum, event_ftime, ctime(&event_time),
// 	 GetnTrRawClusters(), GetnTrClusters(), GetnTrRecHits(), GetnTrTracks());
  

  sprintf(msg, "Run: %10d\nTag: %X\nEvent: %7d\nFineTime: %d\nTime: %s \nnRawClusters: %3d\nnClusters: %3d\nnRecHits: %3d\nnTracks: %3d\n",
 	  runnum, runtag, eventnum, event_ftime, ctime(&event_time),
	  GetnTrRawClusters(), GetnTrClusters(), GetnTrRecHits(), GetnTrTracks());
  
  int num[8][2];
  sout.append(msg);
  sprintf(msg,"Tracks:\n");  
  sout.append(msg);
  for (int ii=0;ii<GetnTrTracks();ii++)
    GetTrTrack(ii)->_printEl(sout);
  for (int ii=0;ii<8;ii++) {num[ii][0]=0; num[ii][1]=0;}
  sprintf(msg,"RawClusters:\n");  
  sout.append(msg);
  GetNRawClusterPlaneSide(num);
  for(int ii=0;ii<8;ii++){
    sprintf(msg,"Layer %d:    S: %3d    K: %3d  \n",ii+1,num[ii][0],num[ii][1]);  
    sout.append(msg);
  }
  sout.append("\n");
  for (int ii=0;ii<8;ii++) {num[ii][0]=0; num[ii][1]=0;}
  
  sprintf(msg,"Clusters:\n");  
  sout.append(msg);
  GetNTrClusterPlaneSide(num);
  for(int ii=0;ii<8;ii++){
    sprintf(msg,"Layer %d:    S: %3d    K: %3d  \n",ii+1,num[ii][0],num[ii][1]);  
    sout.append(msg);
  }
  sout.append("\n");

  for (int ii=0;ii<8;ii++) {num[ii][0]=0; num[ii][1]=0;}
  
  sprintf(msg,"RecHits:\n");  
  sout.append(msg);
  GetNRecHitPlane(num);
  for(int ii=0;ii<8;ii++){
    sprintf(msg,"Layer %d:     %3d     \n",ii+1,num[ii][0]);  
    sout.append(msg);
  }
  sout.append("\n");

   
  
}

int Event::GetDtime(){
  return (event_time-event_timeP)*1000000 + int(event_ftime) - int(event_ftimeP);
}


void Event::GetNRawClusterPlaneSide(int num[8][2]){
  for(int ii=0;ii<fTrRawClusters.size();ii++){
    TrRawClusterR* cc=fTrRawClusters.at(ii);
    num[cc->GetLayer()-1][cc->GetSide()]++; 
  }
  return;
}



void Event::GetNTrClusterPlaneSide(int num[8][2]){
  for(int ii=0;ii<fTrClusters.size();ii++){
    TrClusterR* cc=fTrClusters.at(ii);
    num[cc->GetLayer()-1][cc->GetSide()]++; 
  }
  return;
}

void Event::GetNRecHitPlane(int num[8][2]){
  for(int ii=0;ii<fTrRecHits.size();ii++){
    TrRecHitR* cc=fTrRecHits.at(ii);
    num[cc->GetLayer()-1][0]++; 
  }
  return;
}
