#ifndef Test_hh
#define Test_hh

#include "TObject.h"
#include "TClonesArray.h" 

class Test:public TObject{

public:

  int itest;
  
  //! Default contructor

  Test();
  //! Default destructor
  ~Test(){};

  ClassDef(Test,1)
};


#endif


