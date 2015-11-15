#ifndef Event_hh
#define Event_hh

#include "TObject.h"
#include "TClonesArray.h" 
#include "Cluster.hh"

#define NTDRS 24
#define NJINF 1

//!  Tracker Event class. 
/*!  Tracker Event class contains all the information about a Event
 */

class Event: public TObject{

public:
  //! Progressive Event number
  int Evtnum;
  //! Jinj Status 
  int JINJStatus;
  //! Jinf Status 
  int JINFStatus[NJINF];
  //! Status word for the TDRs (  TDRStatus & 0x1f == TDR ID)
  int TDRStatus[NTDRS];
  //! Common Noise from Calibration
  double CNoise[NTDRS][16];
  //! Cluster number for (ladder 0-5, side 0(S) 1(K))
  int NClus[NTDRS][2];
  //! Total number of clusters
  int NClusTot;
  //! 0 if there are hits on all the ladders
  int notgood;
  //! (TClones) Array of the recontructed clusters
  TClonesArray *Cls;
  //! (TClones) Array of the recontructed hits

  //! pointer to the data
  short int Signal[8][1024];   //8 since more than 8 raw TDRs cannot be read by a single Jinf
  float        SoN[8][1024];   //8 since more than 8 raw TDRs cannot be read by a single Jinf

  short int ReadTDR[NTDRS];
  
  //! Default contructor

  Event();
  //! Default destructor
  ~Event();

  //! Clear the event
  void Clear();
 
  //! Add an Cluster to the array
  Cluster* AddCluster(int lad,int side);
  //! Get the Cluster in the postion ii of the array
  Cluster* GetCluster(int ii);

  static int GetNTDRS() { return NTDRS;};
  static int GetNJINFS() { return NJINF;};  

  int NGoldenClus(int lad, int side);

  static void ReadAlignment(TString filename);
  static float GetAlignPar(int jinfnum, int tdrnum, int component);

  bool FindTrackAndFit(int nptsS, int nptsK, bool verbose=false);
  double GetThetaBestTrack() { return _theta; };
  double GetPhiBestTrack() { return _phi; };
  double GetX0BestTrack() { return _X0; };
  double GetY0BestTrack() { return _Y0; };
  double GetChiBestTrack() { return _chisq; };
  double ExtrapolateBestTrack(double z, int component);
  bool IsClusterUsedInBestTrack(int index_cluster);
  void StoreTrackClusterPatterns();
  inline int GetTrackHitPattern(int side){ return _track_cluster_pattern[side];};
  
private:
  static bool alignmentnotread;
  static float alignpar[NJINF][NTDRS][3];

  double CombinatorialFit(
			std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddS[NJINF][NTDRS],
			std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddK[NJINF][NTDRS],
			int ijinf, int itdr,
			std::vector<std::pair<int, std::pair<double, double> > > v_cog_trackS,
			std::vector<std::pair<int, std::pair<double, double> > > v_cog_trackK,
			int nptsS, int nptsK, 
			bool verbose=false
			);
  double SingleFit(std::vector<std::pair<int, std::pair<double, double> > > vS, std::vector<std::pair<int, std::pair<double, double> > > vK, double& theta, double& thetaerr, double& phi, double& phierr, double& x0, double& x0err, double& y0, double& y0err, bool verbose=false);

private:
  //track parameters and points
  double _theta;//!
  double _phi;//!
  double _X0;//!
  double _Y0;//!
  std::vector<std::pair<int, std::pair<double, double> > > _v_trackS;//!
  std::vector<std::pair<int, std::pair<double, double> > > _v_trackK;//!
  double _chisq;//!
  int _track_cluster_pattern[2];//!
  
  ClassDef(Event,3)
};

//! Run Header Class
class RHClass: public TObject{

public:
  //! Run number
  int Run;
  char date[30];
  int nJinf;
  int JinfMap[NJINF];
  int ntdrRaw;
  int tdrRawMap[24];
  int ntdrCmp;
  int tdrCmpMap[24];
  double CNMean[NTDRS][16];
  double CNSigma[NTDRS][16];

  
  //! default constructor
  RHClass();
  //! default destructor
  virtual ~RHClass(){};
  //! Prints the Header infos
  void Print();

  int FindPos(int tdrnum);
  int FindPosRaw(int tdrnum);

  ClassDef(RHClass,1)
};


#endif
 
