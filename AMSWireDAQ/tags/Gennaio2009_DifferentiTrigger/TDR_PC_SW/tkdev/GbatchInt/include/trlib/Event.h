#ifndef __Event_h__
#define __Event_h__

#include "TObject.h"
#include "link.h"
#include "TrRawCluster.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrMCCluster.h"
#include "TrTof.h"

#include <vector>
#include <ctime>
#include <cstdio>
#include <string>


//! class for a temporary output of the events 
class Event : public TObject{
 public:
  /// Run number
  int    runnum;
  /// Run tag
  int    runtag;
  /// Event number
  int    eventnum;
  /// Event time
  time_t event_time;
  /// Event fine time in us (rounded to ms)
  int    event_ftime;
  /// Event finest time in us
  double    event_fftime;
  /// Size in bytes of the event
  int    event_size;
  /// Event fine time in us (rounded to ms) (previous event)
  int    event_ftimeP;
  /// Event  time in s (previous event)
  int    event_timeP;
  /// Event finest time in us (previous event)
  double    event_fftimeP;
  /// TOF First Plane word (bit 0-7 Tof Slabs 101-108)
  int T1;
  /// TOF Second Plane word (bit 0-7 Tof Slabs 201-208)
  int T2;
  /// Describe which tof planes took part to the trigger
  /// As in the official Code 0= 4 plane 1-4 = 3 planes number enocodes the missing 1 
  /// 5-8= 2 missing but one up one down, 9= missing the two up 10= missing the two down
  /// 12= no plane fired
  int TofFlag1;

  /// TOF data (temporary, used only for the tracker analysis)
  TrTof TofData;

  /// Number of MC clusters
  int NTrMCClusters;
  /// Collection of TrMCCluster
  std::vector<TrMCClusterR*> fTrMCClusters;


  /// Number of raw clusters
  int NTrRawClusters;
  /// Collection of TrRawCluster
  std::vector<TrRawClusterR*> fTrRawClusters;

  /// Number of clusters
  int NTrClusters;
  /// Collection of TrCluster
  std::vector<TrClusterR*> fTrClusters;

  /// Number of rec hits
  int NTrRecHits;
  /// Collection of TrRecHit
  std::vector<TrRecHitR*> fTrRecHits;

  /// Number of tracks
  int NTrTracks;
  /// Collection of TrTrack
  std::vector<TrTrackR*> fTrTracks;

 public:
  /// Default constructor 
  Event();
  /// Copy constructor
  Event(Event& orig);
  /// Default destroyer
  ~Event();
  /// Clear data members
  void Clear(int reallydelete=1);
  /// Print data information
  void Info();
  /// Set run/event information
  void Setinfo(int run, int tag, int evnum, time_t evtime, int evftime, int evsize, int evftimeP, int evtimeP, double evfftimeP) {
    runnum = run; runtag = tag; eventnum = evnum; event_time = evtime;
    event_ftime = evftime; event_size = evsize; event_ftimeP = evftimeP; 
    event_timeP = evtimeP; event_fftimeP = evfftimeP;
  }
  /// Get the time difference wrt the previous event
  int GetDtime();


  /// Get the number of clusters in the fTrMCClusters collection
  int  GetnTrMCClusters() { return NTrMCClusters; }
  /// Add a Tr cluster to the collection
  TrMCClusterR* AddTrMCCluster(TrMCClusterR* pp){ 
    fTrMCClusters.push_back(pp); 
    NTrMCClusters++; 
    return fTrMCClusters.back(); 
  }
  /// Get cluster by the vector index
  TrMCClusterR* GetTrMCCluster(int ii){ return fTrMCClusters.at(ii); }
  /// Clear clusters collection 
  void ClearTrMCClusters(int reallydelete=0) { 
    if(reallydelete)     for (int i = 0; i < (int)fTrMCClusters.size(); i++) 
      if(fTrMCClusters.at(i))delete fTrMCClusters.at(i);
    fTrMCClusters.clear();
    NTrMCClusters = 0;
  }
  /// Remove cluster at i
  void RemoveTrMCCluster(int ii) { 
    if (0 <= ii && ii < fTrMCClusters.size()) {
      fTrMCClusters.erase(fTrMCClusters.begin()+ii);
      NTrMCClusters--;
    }
  }



  
  /// Get the number of raw clusters in the fTrRawClusters collection
  int  GetnTrRawClusters() { return NTrRawClusters; }
  /// Add a raw cluster to the collection
  TrRawClusterR* AddTrRawCluster(TrRawClusterR* pp){ 
    fTrRawClusters.push_back(pp); 
    NTrRawClusters++; 
    return fTrRawClusters.back(); 
  }
  /// Get raw cluster by the vector index
  TrRawClusterR* GetTrRawCluster(int ii) { return fTrRawClusters.at(ii); }
  /// Clear raw clusters collection 
  void ClearTrRawClusters(int reallydelete=0) {
    if(reallydelete)   {
      int lim=fTrRawClusters.size();
      for (int i = 0; i < lim; i++) {
	TrRawClusterR * pp=fTrRawClusters[i];
	if( pp)
	  delete pp;
	}}
    fTrRawClusters.clear();
    NTrRawClusters = 0;
  }
  /// Remove cluster at i
  void RemoveTrRawCluster(int ii) { 
    if (0 <= ii && ii < fTrRawClusters.size()) {
      fTrRawClusters.erase(fTrRawClusters.begin()+ii);
      NTrRawClusters--;
    }
  }
  
  /// Get the number of clusters in the fTrClusters collection
  int  GetnTrClusters() { return NTrClusters; }
  /// Add a Tr cluster to the collection
  TrClusterR* AddTrCluster(TrClusterR* pp){ 
    fTrClusters.push_back(pp); 
    NTrClusters++; 
    return fTrClusters.back(); 
  }
  /// Get cluster by the vector index
  TrClusterR* GetTrCluster(int ii){ return fTrClusters.at(ii); }
  /// Clear clusters collection 
  void ClearTrClusters(int reallydelete=0) { 
    if(reallydelete)     for (int i = 0; i < (int)fTrClusters.size(); i++) 
      if (fTrClusters.at(i)) delete fTrClusters.at(i);
    fTrClusters.clear();
    NTrClusters = 0;
  }
  /// Remove cluster at i
  void RemoveTrCluster(int ii) { 
    if (0 <= ii && ii < fTrClusters.size()) {
      fTrClusters.erase(fTrClusters.begin()+ii);
      NTrClusters--;
    }
  }

  
  /// Get the number of rec hits in the fTrRecHits collection
  int  GetnTrRecHits() { return NTrRecHits; }
  /// Add a rec hit to the collection
  TrRecHitR* AddTrRecHit(TrRecHitR* pp){ 
    fTrRecHits.push_back(pp); 
    NTrRecHits++; 
    return fTrRecHits.back(); 
  }
  /// Get a rec hit by the vector index
  TrRecHitR* GetTrRecHit(int ii) { return fTrRecHits.at(ii); }
  /// Clear raw clusters collection 
  void ClearTrRecHits(int reallydelete=0) {
    if(reallydelete) for (int i = 0; i < (int)fTrRecHits.size(); i++) 
      if (fTrRecHits.at(i)) delete fTrRecHits.at(i);
    fTrRecHits.clear();
    NTrRecHits = 0;
  }
  /// Remove cluster at i
  void RemoveTrRecHit(int ii) { 
    if (0 <= ii && ii < fTrRecHits.size()) {
      fTrRecHits.erase(fTrRecHits.begin()+ii);
      NTrRecHits--;
    }
  }

  /// Get the number of tracks in the fTrTracks collection
  int  GetnTrTracks() { return NTrTracks; }
  /// Add a track to the collection
  TrTrackR* AddTrTrack(TrTrackR* pp){ 
    fTrTracks.push_back(pp); 
    NTrTracks++; 
    return fTrTracks.back(); 
  }
  /// Get a track by the vector index
  TrTrackR* GetTrTrack(int ii){ return fTrTracks.at(ii); }
   /// Clear raw clusters collection 
  void ClearTrTracks(int reallydelete=0) { 
    if(reallydelete)  for (int i = 0; i < (int)fTrTracks.size(); i++) 
      if(fTrTracks.at(i)) delete fTrTracks.at(i);
    fTrTracks.clear();
    NTrTracks = 0;
  }
  /// Remove cluster at i
  void RemoveTrTrack(int ii) { 
    if (0 <= ii && ii < fTrTracks.size()) {
      fTrTracks.erase(fTrTracks.begin()+ii);
      NTrTracks--;
    }
  }

  string sout; //!
  static Event* currev; //!
private:

  void GetNTrClusterPlaneSide(int num[8][2]);
  void GetNRecHitPlane(int num[8][2]);
  void GetNRawClusterPlaneSide(int num[8][2]);


  ClassDef(Event,4);
};

#endif
