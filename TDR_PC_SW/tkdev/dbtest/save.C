#include "TkDBc.h"
#include "TrCalDB.h"
#include "timeid.h"
#include "amscommonsi.h"



int main(int argc,char** argv){
  
  
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();
  TrCalDB::Load("cal.root");
  TrCalDB* cc= TrCalDB::Head;

  cc->CreateLinear();
  cc->CalDB2Lin();
  time_t bb=cc->GetRun();
  tm begin;
  tm end;
  tm* mtim=localtime_r(&bb,&begin);

  time_t nowl;
  time(&nowl);
  tm* mtim2=localtime_r(&nowl,&end);



  
  AMSTimeID* tt= new 
    AMSTimeID(AMSID("TrackerCTest",1),begin,end,
	      TrCalDB::GetLinearSize()
	      ,TrCalDB::linear,AMSTimeID::Standalone,1);
  tt->UpdateMe();
  int pp=tt->write(AMSDATADIR.amsdatabase);

  return 0;

}
