#include "TkDBc.h"
#include "TrCalDB.h"
#include "timeid.h"
#include "amscommonsi.h"

int main(int argc,char** argv){
  if(argc!=2) {
    printf("% Errore\n",argv[0]);
    return -1;
  }
  
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();
  //  TrCalDB::Load(argv[1]);
  TrCalDB* cc=new TrCalDB();

  cc->init();
  cc->CreateLinear();

  time_t bb=0;
  tm begin;
  tm end;
  tm* mtim=localtime_r(&bb,&begin);
  cout <<  asctime(&begin)<<endl;
  
  begin.tm_isdst=0;
  begin.tm_sec=0;
  begin.tm_min=0;
  begin.tm_hour=0;
  begin.tm_mday=0;
  begin.tm_mon=0;
  begin.tm_year=0;

  end.tm_isdst=0;
  end.tm_sec=0;
  end.tm_min=0;
  end.tm_hour=0;
  end.tm_mday=0;
  end.tm_mon=0;
  end.tm_year=0;

  time_t nowl=1211022000; 
  //  time(&nowl);
  //nowl=atoi(argv[1]);
  tm now;
  tm* ptim=localtime_r(&nowl,&now);
  printf("TEST:   %s \n",asctime(&now));
  AMSTimeID* tt= new 
    AMSTimeID(AMSID("TrackerCTest",1),begin,end,
	      TrCalDB::GetLinearSize()
	      ,TrCalDB::linear,AMSTimeID::Standalone,1);


  tt->validate(nowl);
  time_t in,bg,en;
  tt-> gettime(in,  bg,  en);  
  tm pout;
  printf("BEGIN:   %s \n",asctime(localtime_r(&bg,&pout)));
  printf("END:     %s \n",asctime(localtime_r(&en,&pout)));
  printf("INSERT:  %s \n",asctime(localtime_r(&in,&pout)));
  cc->Lin2CalDB();

  if(TrLadCal* aa=cc->FindCal_HwId(atoi(argv[1])) )aa->PrintInfo(1);

  TFile* ff= new TFile("calout.root","recreate");
  cc->Write();
  ff->Close();
  return 0;

}
