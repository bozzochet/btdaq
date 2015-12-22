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

class DecodeData;

class Event: public TObject{
  friend class DecodeData;
  
public:  
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

  inline int GetNClusTot(){ return NClusTot;};
  
  static int GetNTDRS() { return NTDRS;};
  static int GetNJINFS() { return NJINF;};  

  //  int NGoldenClus(int lad, int side);

  static void ReadAlignment(TString filename);
  static float GetAlignPar(int jinfnum, int tdrnum, int component);

  bool FindTrackAndFit(int nptsS, int nptsK, bool verbose=false);
  double GetThetaTrack() { return _theta; };
  double GetPhiTrack() { return _phi; };
  double GetX0Track() { return _X0; };
  double GetY0Track() { return _Y0; };
  double GetChiTrack() { return _chisq; };
  double ExtrapolateTrack(double z, int component);
  bool IsClusterUsedInTrack(int index_cluster);
  void StoreTrackClusterPatterns();
  inline unsigned long long int GetTrackHitPattern(int side, int jinfnum=0){ return _track_cluster_pattern[jinfnum][side];};
  bool IsTDRInTrack(int side, int tdrnum, int jinfnum=0);
  inline std::vector<std::pair<int, std::pair<int, int> > > GetHitVector(){ return _v_trackhit; }
  inline unsigned int GetNHitsTrack(){ return (unsigned int)(_v_trackhit.size()); }
  inline unsigned int GetNHitsSTrack(){ return (unsigned int)(_v_trackS.size()); }
  inline unsigned int GetNHitsKTrack(){ return (unsigned int)(_v_trackK.size()); }
  double GetChargeTrack(int side);
  
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
  double SingleFit(std::vector<std::pair<int, std::pair<double, double> > > vS, std::vector<std::pair<int, std::pair<double, double> > > vK, double& theta, double& thetaerr, double& phi, double& phierr, double& iDirX, double& iDirXerr, double& iDirY, double& iDirYerr, double& x0, double& x0err, double& y0, double& y0err, bool verbose=false);

  void FillHitVector();
  
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
  //! Cluster number for (side 0(S) 1(K))
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
  
  //track parameters and points
  double _iDirX;//!
  double _iDirY;//!
  double _theta;//!
  double _phi;//!
  double _X0;//!
  double _Y0;//!
  std::vector<std::pair<int, std::pair<double, double> > > _v_trackS;//!
  std::vector<std::pair<int, std::pair<double, double> > > _v_trackK;//!
  //! filled by FillHitVector. Here the int is the ladder number and the second pair is <cluster index X, cluster index Y>
  std::vector<std::pair<int, std::pair<int, int> > > _v_trackhit;//!
  double _chisq;//!
  unsigned long long int _track_cluster_pattern[NJINF][2];//!
  
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
 
