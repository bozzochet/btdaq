#include "AMSNtupleHelper.h"
static AMSNtupleHelper * fgHelper=0;
extern "C" AMSNtupleHelper * gethelper();
void* __dso_handle=0;
class AMSNtupleSelect: public AMSNtupleHelper{
public:
AMSNtupleSelect(){};
bool IsGolden(AMSEventR *ev){
// 
// This is a user function to be modified
//  return true if event has to be drawn false otherwise
//
 if(ev && ev->nParticle()>0 && ev->nTrTrack()>0){
   cout <<"working"<<endl;
   return true;
 }
  else return false;
}
};

//
//  The code below should not be modified
//
#ifndef WIN32
extern "C" void fgSelect(){
  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); 
  cout <<"  Handle Loadedd "<<endl;
}

#else
#include <windows.h>
BOOL DllMain(HINSTANCE hinstDLL,  // DLL module handle
    DWORD fdwReason,              // reason called 
    LPVOID lpvReserved)           // reserved 
{ 
 
    switch (fdwReason) 
    { 
        // The DLL is loading due to process 
        // initialization or a call to LoadLibrary. 
 
          case DLL_PROCESS_ATTACH: 
fgHelper=new AMSNtupleSelect();
cout <<"  Handle Loadedd "<<fgHelper<<endl;
break;

        case DLL_PROCESS_DETACH: 
        if(fgHelper){
         delete fgHelper;
         fgHelper=0;
        } 
 
            break; 
 
        default: 
          break; 
     } 
     return TRUE;
}
extern "C" AMSNtupleHelper * gethelper(){return fgHelper;}

#endif
