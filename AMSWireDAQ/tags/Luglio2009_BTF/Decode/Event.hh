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



class Event:public TObject{

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


  //!number of TDRS in raw mode
  int rawtdrnum;
  //! pointer to the data
  short int Signal[8][1024];   //


  short int ReadTDR[NTDRS];
  
  //! Default contructor

  Event(int tdrrawnum=0);
  //! Default destructor
  ~Event();

  //! Clear the event
  void Clear();
 
  //! Add an Cluster to the array
  Cluster* AddCluster(int lad,int side);
  //! Get the Cluster in the postion ii of the array
  Cluster* GetCluster(int ii);
 

  int NGoldenClus(int lad, int side);


  ClassDef(Event,1)

};


//! Run Header Class
class RHClass:public TObject{

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
  

  ClassDef(RHClass,1)
};


#endif
 
