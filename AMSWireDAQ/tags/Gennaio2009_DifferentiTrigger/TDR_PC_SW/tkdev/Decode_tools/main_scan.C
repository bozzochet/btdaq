#include "Event.h"
#include "TFile.h"
#include "TTree.h"



int main(int argc, char** argv){


char fname[120];
int maxn=999999999;
if(argc==2){
  sprintf(fname,"%s",argv[1]);
}
else if(argc==3){
  sprintf(fname,"%s",argv[1]);
  maxn=atoi(argv[2]);
}else{
printf("Usage %s  <filein> [maxev]\n",argv[0]);
exit(2);

}
  Event* ev =new Event();
  TFile f(fname);
  TTree *t4=(TTree*) f.Get("t4");

  TBranch* bb=t4->GetBranch("Tracker");
  bb->SetAddress(&ev);

  int max=(t4->GetEntries()<maxn)?t4->GetEntries():maxn;
  for (int ii=0;ii<max;ii++){
    ev->Clear();
    t4->GetEntry(ii);
    ev->PrintInfo();
  }
return 0;
}
