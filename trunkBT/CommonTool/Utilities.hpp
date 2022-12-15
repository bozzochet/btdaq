#include "Utilities.hh"

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

template <class Event, class RH>
RH* Utilities<Event, RH>::GetRH(TChain* chain){
  //should be done like this (versus doing once at the beginning and using always the same pointer) since
  //at the change of file, along the chain, the retrieved object should change
  
  //  chain->GetTree()->GetUserInfo()->Print();
  RH* rh = (RH*)chain->GetTree()->GetUserInfo()->At(0);
  //  printf("%p\n", rh);

  return rh;
}

template <class Event, class RH>
LadderConf* Utilities<Event, RH>::GetLadderConf(TChain* chain){

  static LadderConf* ladderconf = NULL;

  int oldtreenumber = -10;
  
  if (!ladderconf || oldtreenumber!=chain->GetTreeNumber()) {
    oldtreenumber = chain->GetTreeNumber();
    ladderconf = new LadderConf((LadderParamsMap*)chain->GetTree()->GetUserInfo()->At(1));
  }
  else {
    ladderconf = LadderConf::Instance();
  }
  
  printf("ladderconf = %p\n", ladderconf);
  printf("ladderconf name = %s\n", ladderconf->GetName());
  //  ladderconf->Dump();

  return ladderconf;
}
