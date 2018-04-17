#ifndef Event_hh
#define Event_hh
  
#include "TObject.h"
#include "TClonesArray.h" 
#include "Cluster.hh"
#include "RHClass.hh"
#include "Trace.hh"
#include "Align.hh"

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
  double CNoise[NTDRS][16];
  //! Cluster number for (ladder 0-5, side 0(S) 1(K))
  int NClus[NTDRS][2];
  //! Total number of clusters
  int NClusTot;
  //! 0 if there are hits on all the ladders
  int notgood;
  //! (TClones) Array of the recontructed clusters
  TClonesArray *Cls;
  //! nombre des traces par evenement
  int NTraceTot;
  //! type de trace = 0, 4 amas(haut) cote x et y
  //!                 1, 4 amas(unique) cote x
  //!                 2, 4 amas(unique) cote y
  //!                 3  4 amas(unique) cote x et y
  int TraType;
  //! (TClones) liste des traces
  TClonesArray *Tra;
  //!number of TDRS in raw mode
  int rawtdrnum;
  //! pointer to the data
  short int Signal[8][1024];   //

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
  //! selection seule amas cote s =1, k =2, s et k =3 
  int trace(RHClass* rh, Align* alg);
 
  int NGoldenClus(int lad, int side);



  ClassDef(Event,2)

};



#endif
