#include "TkDBc.h"
#include "TrCalDB.h"
#include "timeid.h"
#include "amscommonsi.h"

int SaveCal2DB(TrCalDB* cc){

  cc->CreateLinear();
  cc->CalDB2Lin();
  time_t bb=cc->GetRun();
  tm begin;
  tm end;
  tm* mtim=localtime_r(&bb,&begin);
  cout << bb<<"  " <<asctime(&begin)<<endl;
  
  end.tm_isdst=0;
  end.tm_sec=0;
  end.tm_min=0;
  end.tm_hour=0;
  end.tm_mday=0;
  end.tm_mon=0;
  end.tm_year=0;
  
  AMSTimeID* tt= new AMSTimeID(AMSID("TrackerCals",1),begin,end,TrCalDB::GetLinearSize(),TrCalDB::linear,AMSTimeID::Standalone,1);
  tt->UpdateMe();
  int pp=tt->write(AMSDATADIR.amsdatabase);

  return 0;

}
