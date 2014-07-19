// $Id: TrRecon.h,v 1.18 2008/12/13 15:01:47 haino Exp $ 
#ifndef __TrRecon__
#define __TrRecon__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrRecon
///\brief Procedures for the Tracker Reconstruction 
///
///\date  2008/02/16 AO  First version
///\date  2008/02/21 AO  BuildTrClusters(), BuildTrRecHits()
///\date  2008/02/24 AO  BuildPatterns(), some bug fixed
///\date  2008/02/26 AO  BuildTrTracks()
///\date  2008/02/28 AO  Bug fixing
///\date  2008/03/11 AO  Some change in clustering methods 
///\date  2008/06/19 AO  Updating TrCluster building 
///\date  2008/07/01 PZ  Global review and various improvements 
///
/// $Date: 2008/12/13 15:01:47 $
///
/// $Revision: 1.18 $
///
//////////////////////////////////////////////////////////////////////////
#include "typedefs.h"
#include "point.h"
#include "cont.h"
#include "TrRawCluster.h"
#include "TrCluster.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "TkDBc.h"
#include "TrFit.h"
#include "TkDBc.h"
#include "VCon.h"
#include "TMath.h"

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>



class TrRecon {

public:
  /// Static parameter containing VCon
  static VCon* TRCon;

protected:
  /// Map association between TkId and 2 p/n side AMSTrClusters lists 
  map< int, pair< vector<AMSTrCluster*>, vector<AMSTrCluster*> > > _ClusterTkIdMap;
  typedef map< int, pair< vector<AMSTrCluster*>, vector<AMSTrCluster*> > >::iterator ITClusterMap;
  /// TrRecHits list for a given layer
  map< int, vector<AMSTrRecHit*> > _RecHitLayerMap;
   
  /// --- Parameters --- ///
public:
  /// Clustering - Seed S/N threshold 
  static float ThrSeed[2];
  /// Clustering - Neighboring S/N strips threshold 
  static float ThrNeig[2];
  /// Clustering - Minimum distance between two seeds
  static int   SeedDist[2];

  /// Correlation - Double gaussian distribution parameters (for muons/protons)
  static float GGpars[6];
  /// Correlation - Double gaussian distribution normalization (for muons/protons)
  static float GGintegral;
  /// Correlation - Probability of correlation threshold
  static float ThrProb;


 protected:

  /// --- Analysis Structures --- ///
  /// clustering - size of ADC/signal/status temporary buffers
  enum { BUFSIZE = 1024 };
  /// clustering - ADC buffer to expand raw clusters
  static float _adcbuf[BUFSIZE];
  static float _sigbuf[BUFSIZE];
  /// clustering - status buffer to expand raw clusters
  static int   _stabuf[BUFSIZE];
  /// clustering - list of the seeds in the subbuffer
  static vector<int> _seedaddresses;   

  static float _adcbuf2[BUFSIZE];
  static float _sigbuf2[BUFSIZE];
  static int   _stabuf2[BUFSIZE];
private:
  //! Private constructor, class is a singleton
  TrRecon(){Clear();  cal=0;}
  //! do not want copy constructor
  TrRecon(TrRecon&){}

  static TrCalDB* _trcaldb;
  TrLadCal* cal;
public:
  //! static pointer to the single instance
  static TrRecon*  Head;
  //! reurnrs the static pointer to the single instance
  static TrRecon*  gethead() {return Head;}
  //! create the single instance
  static TrRecon*  Create(int recreate=0);
  //! std destructor
  ~TrRecon();
  /// Clear data members
  void   Clear(Option_t *option = "");
  /// It sets the Reconstruction Paramenters from the data card
  void SetParFromDataCards();



 /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB() { return _trcaldb; }


  /// Parameters for the full reconstruction
  /// Maximum number of raw clusters allowed for the next step
  static int MaxNrawCls;
  /// Maximum number of clusters allowed for the next step
  static int MaxNtrCls;
  /// Maximum number of hits allowed for the next step
  static int MaxNtrHit;

  /// option is temporary, 0:No_reco 1:TrCluster 2:TrRecHit 3:Full
  /// Start full reconstruction (TrCluster, TrRecHit and TrTrack)
  int Build(int option = 3);
  
  // --- Clustering Methods --- // 
  /// Set the seed S/N threshold
  void   SetThrSeed(int side, float thr) { ThrSeed[side] = thr; }
  /// Set the neigtboring strips S/N threshold
  void   SetThrNeig(int side, float thr) { ThrNeig[side] = thr; }
  /// Set the mimnimum strip distance between two seeds
  void   SetSeedDist(int side, int dist) { SeedDist[side] = dist; }

  /// Clear buffer
  void ClearBuffer();
  /// Expand the TrRawCluster in the buffer structure
  int  ExpandClusterInBuffer(AMSTrRawCluster* cluster);


  /// Builds all the TrClusters (returns the number of TrCluster built) 
  int  BuildTrClusters(int rebuild=0);


  /// Builds the TrClusters in a buffer interval (cyclicity if !=0) 
  int  BuildTrClustersInSubBuffer(int tkid, int first, int last, int cyclicity = 0);
  /// Builds the seed list in the buffer interval (cyclicity if !=0) 
  int  BuildSeedListInSubBuffer(int first, int last, int cyclicity = 0);
  /// Search for the addresses of the boundaries (cyclicity if !=0), returns 10000*leftaddress+rightaddress 
  int  GetBoundariesInSubBuffer(int index, int first, int last, int cyclicity = 0);
  /// Get a well defined address inside an interval (imposing cyclicity if !=0) 
  int  GetAddressInSubBuffer(int address, int first, int last, int ciclicity = 0);


 // --- Hit Signal Correlation (only muons for the moment) --- //
  // --- No reordering implemented --- // 

  /// Set the correlation probability threshold
  void  SetThrProb(float prob) { ThrProb = prob; }
  /// Get correlation between 2 signal (p/n) (protons)
  float GetCorrelation(float n, float p) { return (p - n)/(p + n); }
  float GetCorrelation(TrClusterR* cln, TrClusterR* clp);
  /// Get probability of correlation between 2 signal (with all corrections p/n) (protons)
  float GetProbToBeCorrelated(float n, float p);
  float GetProbToBeCorrelated(TrClusterR* cln, TrClusterR* clp);
 
  /// Builds the Cluster TkId map (for the binary fast search of clusters)
  void  BuildClusterTkIdMap();
  /// Get the number of active (with cluster) ladders by side
  int   GetnActiveLadders() { return _ClusterTkIdMap.size(); } 
 
  /// Get the number of clusters in the tkid/side ladder
  int   GetnTrClusters(int tkid, int side);
  /// Get the cluster in the tkid/side by index
  TrClusterR* GetTrCluster(int tkid, int side, int iclus);

public:
  /// Builds all the TrRecHits (combinations and spares) (returns the number of TrRecHits built)
  int  BuildTrRecHits(int rebuild = 0);

  /// Fild a hit with the combination of xcls and ycls
  AMSTrRecHit* FindHit(AMSTrCluster* xcls,AMSTrCluster* ycls);

////////////////////////////////////////////////////////////////
// ------------------------- TRACKS ------------------------- //
////////////////////////////////////////////////////////////////

//========================================================
// Performance tuning parameters for track reconstruction
//========================================================
public:  
 /// Number of maximum layers
  enum { MAXLAY = 8 };
  /// Status bits (temporary)
  enum { AMBIG = 0x04, USED = 0x20 };
  //
  /// Maximum number of tracks allowed to search for
  static int MaxNtrack;
  /// Minimum number of hits with X- and Y- clusters required for a track
  static int MinNhitXY;
  /// Minimum number of X(p)-side-clusters required for a track
  static int MinNhitX;
  /// Minimum number of Y(n)-side-clusters required for a track
  static int MinNhitY;
  //
  /// PatAllowOption bits
  enum { NO_EXTP = 1, NO_INTP = 2, ALL_5L = 4};
  /// Exception to allow pattern
  static int PatAllowOption;
  //
  /// Range in cm to search for Ladders used in ScanLadders
  static double LadderScanRange;
  /// Range in cm to search for clusters used in ScanClusters
  static double ClusterScanRange;
  /// Maximum chisquare allowed for Ladder scan
  static double MaxChisqForLScan;
  /// Maximum chisquare allowed for Cluster scan
  static double MaxChisqAllowed;
  /// Fitting error in X used for fast fitting in the scan
  static double ErrXForScan;
  /// Fitting error in Y used for fast fitting in the scan
  static double ErrYForScan;
  //
  /// For debug - number of trials in ScanLadders
  int Ntrials1;
  /// For debug - number of trials in ScanClusters
  int Ntrials2;
  /// Debug switch
  static int TrDEBUG;

//========================================================
// HitPatterns
//========================================================
protected:
  /// HitPatternAttrib bits
  enum { O_NMASK = 100000, 
	 O_NP1 = 10000, O_NP2 = 1000, O_NP3 = 100, O_NP4 = 10, O_NP5 = 1 };
  //
  /// Number of hit patterns
  static int NHitPatterns;
  /// Hit pattern masked hit bits
  static int *HitPatternMask;
  /// Hit pattern attribute ( =Nm*100000 +N1*10000 +N2*1000 +N3*100 +N4*10 +N5)
  static int *HitPatternAttrib;
  /// Convert HitPattern number from mask bit
  static int *HitPatternIndex;
  /// First HitPattern number with n masks
  static int *HitPatternFirst;

//========================================================
// HitPattern utilities
//========================================================
public:
  /// Initialize hit patterns, recursively called
  static void BuildHitPatterns(int n = -1, int i = 0, int mask = 0);

  /// Get HitPatternMask
  static int GetNHitPatterns(void) {
    return NHitPatterns;
  }
  /// Test HitPatternMask
  static bool TestHitPatternMask(int i, int layer) {
    return (HitPatternMask && 0 <= i && i < NHitPatterns) 
      ? (HitPatternMask[i] & (1 << (MAXLAY-layer))): 0;
  }
  /// Get HitPatternMask
  static int GetHitPatternMask(int i) {
    return (HitPatternMask && 0 <= i && i < NHitPatterns) ? HitPatternMask[i] : 0;
  }
  /// Get HitPatternAttrib
  static int GetHitPatternAttrib(int i) {
    return (HitPatternAttrib && 0 <= i && i < NHitPatterns) ? HitPatternAttrib[i] : 0;
  }
  /// Get number of hits masked for the pattern i
  static int GetHitPatternNmask(int i) {
    return (HitPatternAttrib && 0 <= i && i < NHitPatterns) ? abs(HitPatternAttrib[i]/O_NMASK) : 0;
  }
  /// Get HitPatternIndex
  static int GetHitPatternIndex(int mask) {
    return (HitPatternIndex && 0 <= mask && mask < 256) ? HitPatternIndex[mask] : -1;
  }
  /// Get HitPatternFirst
  static int GetHitPatternFirst(int nm) {
    return (HitPatternFirst && 0 <= nm && nm < 6) ? HitPatternFirst[nm] : NHitPatterns;
  }
  /// Get HitPatternMask as a string
  static const char *GetHitPatternStr(int i, char con = '1', char coff = '0');

//========================================================
// Temporary buffers for fast hit scanning
//========================================================
protected:
  /// Array of vectors of TkId at each layer
  vector<int> _LadderHitMap[MAXLAY];
  /// Number of ladders with both p(X) and n(Y) clusters at each layer
  int _NladderXY[MAXLAY];

  /// Virtual 2D array to store TrRecHits at [iclx][icly]
  class Hits2DArray : public vector<AMSTrRecHit*> {
  public:
    int Ncls[2];        ///< Number of X and Y clusters
    /// Get index of a hit at [iclx][icly]
    int index(int iclx, int icly) const { 
      return (0 <= iclx && iclx < Ncls[0] && 
	      0 <= icly && icly < Ncls[1]) ? icly*Ncls[0]+iclx : -1; 
    }
    /// Get a hit at [iclx][icly]
    AMSTrRecHit *at(int iclx, int icly) const { 
      int i = index(iclx, icly);
      return (0 <= i && i < size()) ? vector<AMSTrRecHit*>::at(i) : 0;
    }
  };

  /// Map of TrRecHits with a key as TkId
  typedef map<int, Hits2DArray> HitsTkIdMap;
  /// Iterator of HitsTkIdMap
  typedef HitsTkIdMap::iterator ITHitsTkIdMap;

  /// Map of virtual 2D array to store TrRecHits at [tkid][iclx][icly]
  HitsTkIdMap _HitsTkIdMap;

  /// Get Hits2DArray with tkid
  const Hits2DArray *GetHits2DArray(int tkid) const {
    HitsTkIdMap::const_iterator it = _HitsTkIdMap.find(tkid);
    return (it == _HitsTkIdMap.end()) ? 0 : &it->second;
  }

  /// A predicate to sort hits in descending order of prob.
  class CompProb {
  public:
    bool operator() (const AMSTrRecHit* lhit, const AMSTrRecHit* rhit) const {
      return lhit->GetProb() > rhit->GetProb();
    }
  };
  
public:
  /// Builds up the map RecHit Layer
  void BuildHitsTkIdMap();

  /// Builds up LadderHitMap of TrRecHits, used for standard reconstruction
  void BuildLadderHitMap();

  /// Builds up LadderHitMap of TrClusters, used for TDisplay
  void BuildLadderClusterMap();

  /// Get number of ladders with cluster signals at the layer
  int GetNladder(int layer) const { 
    return (0 <= layer && layer < MAXLAY) ? _LadderHitMap[layer-1].size() : 1;
  }
  /// Get tkid from _LadderHitMap
  int GetLadderHit(int layer, int i) const {
    return (0 <= i && i < GetNladder(layer)) ? _LadderHitMap[layer-1][i] : 0;
  }

  /// Get number of hits for all X-Y combinations with tkid
  int GetnTrRecHits(int tkid) const {
    const Hits2DArray *hits = GetHits2DArray(tkid);
    return (hits) ? hits->size() : 0;
  }
  /// Get number of projection hits with tkid and side
  int GetnTrRecHits(int tkid, int side) const {
    const Hits2DArray *hits = GetHits2DArray(tkid);
    return (hits && (side == 0 || side == 1)) ? hits->Ncls[side] : 0;
  }
  /// Get TrRecHit with tkid, iclx and icly
  AMSTrRecHit *GetTrRecHit(int tkid, int iclx, int icly) const {
    const Hits2DArray *hits = GetHits2DArray(tkid);
    return (hits) ? hits->at(iclx, icly) : 0;
  }
  /// Remove a row of hits from _HitsTkIdMap;
  void RemoveHits(int tkid, int icls, int side);

//========================================================
// Structures
//========================================================
public:
  /// Structure of hit iterators
  typedef struct TrHitIterStruct {
    int    pattern;            ///< Hit pattern of current scan
    int    side;               ///< Side of current scan
    int    nlayer;             ///< Number of layers to be scanned
    int    ilay [MAXLAY];      ///< Scanning order for effective pre-selection
    int    tkid [MAXLAY];      ///< TkId list
    int    iscan[MAXLAY][2];   ///< Current candidate hit index
    int    imult[MAXLAY];      ///< Current candidate multiplicity index
    AMSPoint coo[MAXLAY];      ///< Current candidate 3D-coordinate
    double psrange;            ///< Pre-selection range
    double param[4];           ///< Pre-selection parameter
    double chisq[2];           ///< Chisquare in X and Y
    int    nhitc;              ///< Number of hits with X and Y cls.
    int &Iscan(int i) { return iscan[i][side]; }
  } TrHitIter;

//========================================================
// Track reconstruction methods
//========================================================
public:
  /// Reconstruct tracks, returns number of tracks reconstructed
  int BuildTrTracks(int refit = 0);

  /// Build one track from the best candidate iterator
  int BuildATrTrack(TrHitIter &cand);

  /// Purge "ghost" hits and assign hit index to tracks
  void PurgeGhostHits();

  /// General recursion function for Ladder/Hit scan
  /*!
   *\param[in]  idx    Layer index, it should be 0 for the first call
   *
   *\param[in]  it     Scan iterator. it.pattern, it.side, it.nlayer, 
   *                    it.tkid, and it.ilay must be filled before calling. 
   *                    it.tkid can be like (layer)*100. 
   *                    The other elements to be filled during the recursion.
   *
   *\param[out] itcand Iterator containing the best candidate found so far
   *
   *\param[in]  Coord  Static reentrant function with 3 modes; 
   *                    mode=1: returns number of hit candidates.
   *                    mode=2: sets multiplicity range
   *                    mode=3: fills it.coo[idx] from it.iscan and it.imult
   *
   *\param[in]  Eval   Static reentrant function to evaluate the current 
   *                    combination, to be called at the last of layer loop. 
   *                    The second argument, itcand is to be filled 
   *                    if the best candidate has been found. 
   *  
   * This function should be reentrant as far as 
   *  Coord and Eval are also reentrant.
   */
  int ScanRecursive(int idx, TrHitIter &it, TrHitIter &itcand, 
		    int (*Coord)(int idx, TrHitIter &it, int mode), 
		    int (*Eval) (TrHitIter &it, TrHitIter &itcand)) const;

  /// Pre-selection (interpolation check) on the current scan
  bool PreScan(int nlay, TrHitIter &iter) const;
  /// Fill the order of scanning layers to make pre-selection most effectie
  int SetLayerOrder(TrHitIter &iter) const;

  /// Scan ladders, returns 1 and itcand is filled if candidate found
  int ScanLadders(int pattern, TrHitIter &itcand) const;
  /// Coord.manager for ladders scan to be put in ScanRecursive
  int LadderCoordMgr(int idx, TrHitIter &it, int mode) const;
  /// Evaluator for Ladder scan to be put in ScanRecursive
  int LadderScanEval(TrHitIter &it, TrHitIter &cand) const;

  /// Scan X and Y hits, returns 1 and itcand is filled if candidate found
  int ScanHits(TrHitIter &it, TrHitIter &itcand) const;
  /// Coord.manager for hits scan to be put in ScanRecursive
  int HitCoordMgr(int idx, TrHitIter &it, int mode) const;
  /// Evaluator for hits scan to be put in ScanRecursive
  int HitScanEval(const TrHitIter &it, TrHitIter &cand) const;

  /// Estimate multiplicity and readout strip by interpolation
  int EstimateXCoord(int il, TrHitIter &iter) const;

//========================================================
// Utilities for debugging
//========================================================
public:
  /// FFKEY.init() called from ROOT CINT
  static void InitFFKEYs(int magstat);
  /// TrField::Read() called from ROOT CINT
  static int ReadTrField(const char *fname, int magstat = 1);
  /// TrField::GuFld() called from ROOT CINT
  static void GetTrField(float *pos, float *bf);
  /// TrField::TkFld() called from ROOT CINT
  static void GetTkFld(float *pos, float *hxy);


/////////////////////////
// --- MC clusters --- //
/////////////////////////

private:
 static void DSP_Clusterize(int tkid,float *buf);

public: 
  //! Generate the TrMCCluster from the geant info
  static void sitkhits(int idsoft, float vect[],
		      float edep, float step, int itra);
  //! Generate Non Gaussians Noise Cluster (dummy for the moment)
  static void sitknoise(){}

  //! Generate the TrRawClusters from the MC info and from a calibration
  static void sitkdigi(); 
};

#endif


