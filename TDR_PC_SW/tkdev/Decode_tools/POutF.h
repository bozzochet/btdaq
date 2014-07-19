#ifndef _POUTF_
#define _POUTF_
#include <string>
#include <cstdarg>
#include <cstdio>
#include <iostream>
using namespace std;
class POutF{

 private:
  std::string ss;

public:
  POutF();
  ~POutF(){}
  static POutF* pp;
  void Clear(){ss.clear();}
  void Print(){cerr<< ss;}
  void IOutput ( char * fmt, ... );
  
};
#endif
 
#define PRINTF(a...) { POutF::pp->IOutput(a);}
