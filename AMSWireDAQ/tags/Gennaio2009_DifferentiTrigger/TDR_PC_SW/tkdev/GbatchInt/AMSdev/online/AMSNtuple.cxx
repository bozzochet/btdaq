//  $Id: AMSNtuple.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <stdlib.h>
#include <iostream>
#include "AMSNtuple.h"
#include "AMSNtupleHelper.h"
ClassImp(AMSNtupleR)
  void* gAMSUserFunction;

  AMSNtupleR::AMSNtupleR(TChain *chain):fCurrentTree(-1),fEntries(0),fCurrentEntry(-1){

   //loop on entries (elist or all entries)

  if(chain){
    Init(chain);
    fEntries=chain->GetEntries();
    cout <<" AMSNtupleR-I-EntriesFound "<<fEntries<<endl;
    if(fEntries>0 && chain->GetTree() && fEntries!=1234567890){
     
//     chain->GetTree()->SetTimerInterval();
//     Int_t interval = chain->GetTree()->GetTimerInterval();
//     fTimer = new TProcessEventTimer(interval);
//     fTimer->TurnOn();
      Clear();
     ReadOneEvent(0);
     return; 
    }
   }
      cerr<<"AMSNtupleR::AMSNtupleR-S-UnableInitializeNtuple"<<endl;
      fHeader.Run=0;
      fHeader.Time[0]=0;
      fEntries=0;
      Clear();
      exit(1);
}


int AMSNtupleR::ReadOneEvent(int entry){
//      if(fTimer->ProcessEvents()){
//          cout <<"qq"<<endl;
//      }
      if(gSystem->ProcessEvents()){
          cout <<"InteruptReceived"<<endl;
          return -1;
      }
      fCurrentEntry=entry;
      int entryNumber = _Tree->LoadTree(entry);
      if (entryNumber < 0) return -1;
      if (_Tree->GetTreeNumber() != fCurrentTree) {
         fCurrentTree = _Tree->GetTreeNumber();
         Notify();
      }
      if(!ProcessCut(entryNumber) || (AMSNtupleHelper::fgHelper && !AMSNtupleHelper::fgHelper->IsGolden(this)))return 0;
      return 1;
}


bool AMSNtupleHelper::IsGolden(AMSEventR *o){
return true;
}

 AMSNtupleHelper *AMSNtupleHelper::fgHelper=0;
