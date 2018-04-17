#ifndef Align_hh
#define Align_hh
#include "TObject.h"

class Align:public TObject{

public:
  int Run;
  int ntdrRaw;
  int ntdrCmp;
  int nJinf;
  
  //! Default contructor

  Align(){
  Run=0;
  ntdrRaw=0;
  ntdrCmp=0;
  nJinf=0;
  };
  //! Default destructor
  ~Align();

  ClassDef(Align,1)
};


#endif

ClassImp(Align);


