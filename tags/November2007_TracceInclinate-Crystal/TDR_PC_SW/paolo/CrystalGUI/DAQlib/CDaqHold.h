#ifndef CDaqHold_h
#define CDaqHold_h

#include "MainDaq.hh"

class CDaqHold{

public:

  static MainDaq* mdaq;
  static DaqClass* pDComm; 
  CDaqHold(){};
  

};
#endif
