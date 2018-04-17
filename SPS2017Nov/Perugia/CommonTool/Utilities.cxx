#include "Utilities.hh"

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

RHClass* GetRH(TChain* chain){
  
  //  chain->GetTree()->GetUserInfo()->Print();
  RHClass* rh = (RHClass*)chain->GetTree()->GetUserInfo()->At(0);
  //  printf("%p\n", rh);

  return rh;
}
