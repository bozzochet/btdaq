#ifndef RHClass_hh
#define RHClass_hh
  
#include "TObject.h"
#include "TClonesArray.h" 

#define NTDRS 24  
#define NJINF 2

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
  //! conversion
  int FindPos(int tdrnum);
  //! Prints the Header infos
  void Print();
 
  ClassDef(RHClass,1)
};


#endif
