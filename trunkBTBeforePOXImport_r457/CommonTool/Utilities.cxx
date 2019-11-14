#include "Utilities.hh"

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

RHClass* GetRH(TChain* chain){
  //should be done like this (versus doing once at the beginning and using always the same pointer) since
  //at the change of file, along the chain, the retrieved object should change
  
  //  chain->GetTree()->GetUserInfo()->Print();
  RHClass* rh = (RHClass*)chain->GetTree()->GetUserInfo()->At(0);
  //  printf("%p\n", rh);

  return rh;
}
