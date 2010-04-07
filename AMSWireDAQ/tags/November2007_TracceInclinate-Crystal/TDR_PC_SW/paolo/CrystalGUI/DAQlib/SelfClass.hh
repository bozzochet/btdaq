#ifndef SelfClass_hh
#define SelfClass_hh

#include "CommClass.hh"


class SelfClass :public CommClass {

public:


  SelfClass(char* address="localhost",int port=6666);
  virtual ~SelfClass(){};

  int CalibPG(int run,float timeout=150);
  //  int Status();

};

#endif
