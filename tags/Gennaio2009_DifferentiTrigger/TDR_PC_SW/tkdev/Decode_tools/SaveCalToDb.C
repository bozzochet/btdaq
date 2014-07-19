#include "TkDBc.h"
#include "TrCalDB.h"
#include "timeid.h"
#include "amscommonsi.h"

int  SaveCal2DB(TrCalDB * cc,time_t endtime){

  cc->CreateLinear();
  cc->CalDB2Lin();
  time_t bb=cc->GetRun();
  tm begin;
  tm end;
  tm* mtim=localtime_r(&bb,&begin);

  tm* mtim2=localtime_r(&endtime,&end);



  
  AMSTimeID* tt= new AMSTimeID(AMSID("TrackerCals",1),begin,end,TrCalDB::GetLinearSize(),TrCalDB::linear,AMSTimeID::Standalone,1);
  tt->UpdateMe();
  int pp=tt->write(AMSDATADIR.amsdatabase);

  return 0;

}
