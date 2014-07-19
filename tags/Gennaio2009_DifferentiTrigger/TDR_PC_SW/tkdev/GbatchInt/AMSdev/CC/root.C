//

#include "root.h"
#include "ntuple.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "antirec02.h"
#include "beta.h"
#include "ecalrec.h"
#include "mccluster.h"
#include "mceventg.h"
#include "particle.h"
#include "richrec.h"
#include "tofdbc02.h"
#include "tofsim02.h"
#include "tofrec02.h"
#include "antirec02.h"
#include "trdrec.h"
#include "trigger102.h"
#include "trigger302.h"
#include "trrawcluster.h"
#include "trrec.h"
#include "astro.h"
#include "amsdbc.h"
#include "vtx.h"

//#include <values.h>
#include <limits.h>
#include <float.h>
#endif
using namespace root;
#ifdef __WRITEROOT__

/*
  ClassImp(HeaderR)
  ClassImp(EcalHitR)
  ClassImp(EcalClusterR)
  ClassImp(Ecal2DClusterR)
  ClassImp(EcalShowerR)
  ClassImp(RichHitR)
  ClassImp(RichRingR)
  ClassImp(TofRawClusterR)
  ClassImp(TofRawSideR)
  ClassImp(TofClusterR)
  ClassImp(AntiClusterR)
  ClassImp(TrRawClusterR)
  ClassImp(TrClusterR)
  ClassImp(TrRecHitR)
  ClassImp(TrTrackR)
  ClassImp(TrdRawHitR)
  ClassImp(TrdClusterR)
  ClassImp(TrdSegmentR)
  ClassImp(TrdTrackR)
  ClassImp(Level1R)
  ClassImp(Level3R)
  ClassImp(BetaR)
  ClassImp(ChargeR)
  ClassImp(ParticleR)
  ClassImp(AntiMCClusterR)
  ClassImp(TrMCClusterR)
  ClassImp(TofMCClusterR)
  ClassImp(TrdMCClusterR)
  ClassImp(RichMCClusterR)
  ClassImp(MCTrackR)
  ClassImp(MCEventgR)
*/
  ClassImp(AMSEventR)
  ClassImp(AMSChain)
  ClassImp(AMSEventList)



//------------------- constructors -----------------------

AMSEventR::Service::hb1_d AMSEventR::Service::hb1;
AMSEventR::Service::hb2_d AMSEventR::Service::hb2;
AMSEventR::Service::hbp_d AMSEventR::Service::hbp;
char AMSEventR::Service::Dir[1024]="";

void AMSEventR::hcopy(char dir[],int id1,int id2){
 char save[1024];
 strcpy(save,Service::Dir);
 hcdir(dir);
  TH1F *h1p = h1(id1);
  if(h1p){
    float a=h1p->GetBinLowEdge(1); 
    int n=h1p->GetNbinsX();
    float b=h1p->GetBinLowEdge(n)+h1p->GetBinWidth(n); 
    const char *title=h1p->GetTitle();
    hcdir(save);
    hbook1(id2,title,n,a,b);
    TH1F *h2p = h1(id2);
    if(h2p){
    for (int i=0;i<n+2;i++){
     h2p->SetBinContent(i,h1p->GetBinContent(i));
     h2p->SetBinError(i,h1p->GetBinError(i));
      
    }
    }
  }
  else{
   TH2F *h2p = h2(id1);
   if(h2p){
   }
   else{
    TProfile *hpp=hp(id1);
    if(hpp){
    }
   }
  }
 
}

void AMSEventR::hcopy(int id1,int id2){
  TH1F *h1p = h1(id1);
  if(h1p){
    float a=h1p->GetBinLowEdge(1); 
    int n=h1p->GetNbinsX();
    float b=h1p->GetBinLowEdge(n)+h1p->GetBinWidth(n); 
    const char *title=h1p->GetTitle();
    hbook1(id2,title,n,a,b);
    TH1F *h2p = h1(id2);
    if(h2p){
    for (int i=0;i<n+2;i++){
     h2p->SetBinContent(i,h1p->GetBinContent(i));
     h2p->SetBinError(i,h1p->GetBinError(i));
      
    }
    }
  }
  else{
   TH2F *h2p = h2(id1);
   if(h2p){
   }
   else{
    TProfile *hpp=hp(id1);
    if(hpp){
    }
   }
  }
}
void AMSEventR::hdivide(int id1,int id2,int id3){
  TH1F *h2p = h1(id2);
  if(h2p){
//   h2p->Sumw2();
   hcopy(id1,id3);
   TH1F *h1p = h1(id3);
   if(h1p){
//    h1p->Sumw2();
    h1p->Divide(h2p);
   }   
  }
}
void AMSEventR::hscale(int id1,double fac){
  TH1F *h2p = h1(id1);
  if(h2p){
   h2p->Sumw2();
   h2p->Scale(fac);
  }
}
void AMSEventR::hsub(int id1,int id2,int id3){
  TH1F *h2p = h1(id2);
  if(h2p){
   h2p->Sumw2();
   hcopy(id1,id3);
   TH1F *h1p = h1(id3);
   if(h1p){
    h1p->Sumw2();
    h1p->Add(h2p,-1);
   }   
  }
}
void AMSEventR::hadd(int id1,int id2,int id3){
  TH1F *h2p = h1(id2);
  if(h2p){
   h2p->Sumw2();
   hcopy(id1,id3);
   TH1F *h1p = h1(id3);
   if(h1p){
    h1p->Sumw2();
    h1p->Add(h2p,1);
   }   
  }
}
void AMSEventR::hbook1s(int id,const char title[], int ncha, float  a, float b,int howmany,int shift){
  for (int i=0;i<howmany;i++){
   hbook1(id+shift*i,title,ncha,a,b);
  }
}

void AMSEventR::hbook1(int idd,const char title[], int ncha, float  a, float b){ 
AMSID id(idd,Service::Dir);
 if(Service::hb1.find(id) != Service::hb1.end()){
  delete Service::hb1.find(id)->second;
  Service::hb1.erase((Service::hb1.find(id)));
  cerr<<"  AMSEventR::hbook1-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
 }
  char hid[1025];
  sprintf(hid,"hb1_%d_%s",idd,title); 
  TH1F * p= new TH1F(hid,title,ncha,a,b);
  (Service::hb1).insert(make_pair(id,p));
}

void AMSEventR::hbookp(int idd,const char title[], int ncha, float  a, float b){
   AMSID id(idd,Service::Dir);
 if(Service::hbp.find(id) == Service::hbp.end()){
  char hid[1025];
  sprintf(hid,"hbp_%d_%s",idd,title); 
  TProfile * p= new TProfile(hid,title,ncha,a,b);
  Service::hbp.insert(make_pair(id,p));
 }
 else{
  cerr<<"  AMSEventR::hbookp-S-Histogram "<<id<<" AlreadyExistsResetting "<<endl;
  Service::hbp.find(id)->second->Reset();
 }
}

void AMSEventR::hbook2s(int id, const char title[], int ncha, float  a, float b, int nchaa, float aa, float ba,int howmany,int shift){
  for (int i=0;i<howmany;i++){
   hbook2(id+shift*i,title,ncha,a,b,nchaa,aa,ba);
  }
}
void AMSEventR::hbook2(int idd,const char title[], int ncha, float  a, float b, int nchaa, float aa, float ba){
AMSID id(idd,Service::Dir);
 if(Service::hb2.find(id) != Service::hb2.end()){
  delete Service::hb2.find(id)->second;
  Service::hb2.erase((Service::hb2.find(id)));
  cerr<<"  AMSEventR::hbook2-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
 }
  char hid[1025];
  sprintf(hid,"hb2_%d_%s",idd,title); 
  TH2F * p= new TH2F(hid,title,ncha,a,b,nchaa,aa,ba);
  Service::hb2.insert(make_pair(id,p));
}

void AMSEventR::hfill(int idd, float a, float b=0, float w=1){
AMSID id(idd,Service::Dir);
   Service::hb1i i1=Service::hb1.find(id);
 if(i1 != Service::hb1.end()){
  i1->second->Fill(a,w);
 }
 else{
  Service::hb2i i2=Service::hb2.find(id);
  if(i2 != Service::hb2.end()){
    i2->second->Fill(a,b,w);
  }
  else{
   Service::hbpi ip=Service::hbp.find(id);
    if(ip != Service::hbp.end()){
     ip->second->Fill(a,w);
   }
 }
 }
}

void AMSEventR::hfit1(int id, char func[]){
   AMSID idd(id,Service::Dir);
   char fit[255];
   Service::hb1i i1=Service::hb1.find(idd);
 if(i1 != Service::hb1.end()){
  if(func[0] == 'g')strcpy(fit,"gaus");
  else if(func[0] =='e')strcpy(fit,"expo");
  else if(func[0]=='p'){
   strcpy(fit,"pol");
   strcat(fit,func+1);
  }
  else strcpy(fit,func);
  i1->second->Fit(fit);
 }
}

void AMSEventR::hprint(int idd, char opt[]){
   AMSID id(idd,Service::Dir);
   Service::hb1i i1=Service::hb1.find(id);
 if(i1 != Service::hb1.end()){
  i1->second->Draw(opt);
 }
 else{
  Service::hb2i i2=Service::hb2.find(id);
  if(i2 != Service::hb2.end()){
    i2->second->Draw(opt);
  }
  else{
   Service::hbpi ip=Service::hbp.find(id);
    if(ip != Service::hbp.end()){
     ip->second->Draw(opt);
   }
 }
 }
}


void AMSEventR::hlist(char ptit[]){
for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
if(strlen(ptit)<1 || strstr(i->second->GetTitle(),ptit))cout<<" 1D "<<i->first<<" "<<i->second->GetTitle()<<endl;
}
for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++){
if(strlen(ptit)<1 || strstr(i->second->GetTitle(),ptit))cout<<" 2D "<<i->first<<" "<<i->second->GetTitle()<<endl;
}
for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++){
 if(strlen(ptit)<1 || strstr(i->second->GetTitle(),ptit))cout<<" Profile "<<i->first<<" "<<i->second->GetTitle()<<endl;
}
}


void AMSEventR::hdelete(int idd){
if(idd ==0){
for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
delete i->second;
}
Service::hb1.clear();
for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++){
delete i->second;
}
Service::hb2.clear();
for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++){
delete i->second;
}
Service::hbp.clear();

}
else{
   AMSID id(idd,Service::Dir);
   Service::hb1i i1=Service::hb1.find(idd);
 if(i1 != Service::hb1.end()){
  delete i1->second;
  Service::hb1.erase(i1);
 }
 else{
  Service::hb2i i2=Service::hb2.find(id);
  if(i2 != Service::hb2.end()){
  delete i2->second;
  Service::hb2.erase(i2);

  }
  else{
   Service::hbpi ip=Service::hbp.find(id);
    if(ip != Service::hbp.end()){
  delete ip->second;
  Service::hbp.erase(ip);

   }
 }
 }

}
}
void AMSEventR::chdir(const char dir[]){
  strcpy(Service::Dir,dir);
}

void AMSEventR::hcdir(const char dir[]){
  strcpy(Service::Dir,dir);
}

void AMSEventR::hfetch(const char file[]){
 TFile *f= new TFile(file);
 hfetch(*f,file);
 }
#include <TKey.h>
void AMSEventR::hfetch(TFile &f, const char dir[]){
   chdir(dir);
   int fetch1=0;
   int fetch2=0;
   int fetchp=0;
   TIter nextkey(f.GetListOfKeys());
   TKey *key;
   while (key = (TKey*)nextkey()) {
      TH1F * f1 = dynamic_cast<TH1F*>(key->ReadObj());
      if(f1){
       TString t(f1->GetName());
       if(t.BeginsWith("hb1")){
        for(int i=4;i<strlen(f1->GetName());i++){
          if(i>4 && f1->GetName()[i] =='_'){
             TString st(f1->GetName()+4,i-4);
             if(st.IsDigit()){
              int idd=st.Atoi(); 
              AMSID id(idd,Service::Dir);
              fetch1++;
              if(Service::hb1.find(id) == Service::hb1.end()){
              }
              else{
               cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
               Service::hb1.erase((Service::hb1.find(id)));
              }
              (Service::hb1).insert(make_pair(id,f1));
             }
             else cerr<<"TH1F "<<t<<" IdNotDigitalSkipped"<<endl;
           }
        }
       }
       else cerr<<"TH1F "<<t<<" NotCreatedByHBOOK1Skipped"<<endl;
      }
      else{
       TH2F * f1 = dynamic_cast<TH2F*>(key->ReadObj());
      if(f1){
       TString t(f1->GetName());
       if(t.BeginsWith("hb2")){
        for(int i=4;i<strlen(f1->GetName());i++){
          if(i>4 && f1->GetName()[i] =='_'){
             TString st(f1->GetName()+4,i-4);
             if(st.IsDigit()){
              int idd=st.Atoi(); 
              AMSID id(idd,Service::Dir);
              fetch2++;
              if(Service::hb2.find(id) == Service::hb2.end()){
              }
              else{
               cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
               Service::hb2.erase((Service::hb2.find(id)));
              }
              (Service::hb2).insert(make_pair(id,f1));
             }
             else cerr<<"TH2F "<<t<<" IdNotDigitalSkipped"<<endl;
           }
        }
       }
       else cerr<<"TH2F "<<t<<" NotCreatedByHBOOK1Skipped"<<endl;
      }
      else{
       TProfile * f1 = dynamic_cast<TProfile*>(key->ReadObj());
      if(f1){
       TString t(f1->GetName());
       if(t.BeginsWith("hbp")){
        for(int i=4;i<strlen(f1->GetName());i++){
          if(i>4 && f1->GetName()[i] =='_'){
             TString st(f1->GetName()+4,i-4);
             if(st.IsDigit()){
              int idd=st.Atoi(); 
              AMSID id(idd,Service::Dir);
              fetchp++;
              if(Service::hbp.find(id) == Service::hbp.end()){
              }
              else{
               cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
               Service::hbp.erase((Service::hbp.find(id)));
              }
              (Service::hbp).insert(make_pair(id,f1));
             }
             else cerr<<"TProfile "<<t<<" IdNotDigitalSkipped"<<endl;
           }
        }
       }
       else cerr<<"TProfile "<<t<<" NotCreatedByHBOOK1Skipped"<<endl;
      }


      }


      }
   } 
   if(fetch1>0)cout <<fetch1<<" 1D Histos fetched"<<endl;
   if(fetch2>0)cout <<fetch2<<" 2D Histos fetched"<<endl;
   if(fetchp>0)cout <<fetchp<<" Profile Histos fetched"<<endl;
}

TH1F * AMSEventR::h1(int idd){
//
// must be used with care as may be changed after map update operation
//
   AMSID id(idd,Service::Dir);
   Service::hb1i i1=Service::hb1.find(id);
   if(i1 != Service::hb1.end())return i1->second;
   else return 0; 
}

TH2F * AMSEventR::h2(int idd){
   AMSID id(idd,Service::Dir);
   Service::hb2i i1=Service::hb2.find(id);
   if(i1 != Service::hb2.end())return i1->second;
   else return 0; 
}


TProfile * AMSEventR::hp(int idd){
   AMSID id(idd,Service::Dir);
   Service::hbpi i1=Service::hbp.find(id);
   if(i1 != Service::hbp.end())return i1->second;
   else return 0; 
}

void AMSEventR::hreset(int idd){
if(idd ==0){
 for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++)i->second->Reset();
 for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++)i->second->Reset();
 for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++)i->second->Reset();
}
else{
   AMSID id(idd,Service::Dir);
   Service::hb1i i1=Service::hb1.find(id);
 if(i1 != Service::hb1.end()){
  i1->second->Reset();
 }
 else{
  Service::hb2i i2=Service::hb2.find(id);
  if(i2 != Service::hb2.end()){
    i2->second->Reset();
  }
  else{
   Service::hbpi ip=Service::hbp.find(id);
    if(ip != Service::hbp.end()){
     ip->second->Reset();
   }
 }
 }
}
}

void AMSEventR::hf1s(int id, float a, bool cuts[], int ncuts, int icut,int shift,float w){
//  Fill the series of histos 
//  1st before cuts
//  before cut icut-1
//  before icut-1 as last cut
//  after icut-1 as last cut
//  after last cut as first cut
//  after cut icut-1 as first cut
    hf1(id,a,w);
     bool cut=true;
    if(icut-1>0)for(int k=0;k<icut-1;k++)cut=cut && cuts[k];
    if(cut)hf1(id+shift,a,w);
    for(int k=icut;k<ncuts;k++)cut=cut && cuts[k];
    if(cut)hf1(id+shift+shift,a,w);
    if(cut && cuts[icut-1])hf1(id+shift+shift+shift,a,w);             
    if(cuts[ncuts-1])hf1(id+shift+shift+shift+shift,a,w);             
    if(icut-1>=0 && cuts[icut-1])hf1(id+shift+shift+shift+shift+shift,a,w);             
}

void AMSEventR::hf1(int idd, float a, float w){
   AMSID id(idd,Service::Dir);
   Service::hb1i i1=Service::hb1.find(id);
 if(i1 != Service::hb1.end()){
  i1->second->Fill(a,w);
 }
}

void AMSEventR::hfp(int idd, float a, float w=1){
   AMSID id(idd,Service::Dir);
   Service::hbpi i1=Service::hbp.find(id);
 if(i1 != Service::hbp.end()){
  i1->second->Fill(a,w);
 }
}


void AMSEventR::hf2(int idd, float a, float b, float w=1){
   AMSID id(idd,Service::Dir);
   Service::hb2i i1=Service::hb2.find(id);
 if(i1 != Service::hb2.end()){
  i1->second->Fill(a,b,w);
 }
}

TBranch* AMSEventR::bHeader;
TBranch* AMSEventR::bEcalHit;
TBranch* AMSEventR::bEcalCluster;
TBranch* AMSEventR::bEcal2DCluster;
TBranch* AMSEventR::bEcalShower;
TBranch* AMSEventR::bRichHit;
TBranch* AMSEventR::bRichRing;
TBranch* AMSEventR::bTofRawCluster;
TBranch* AMSEventR::bTofRawSide;
TBranch* AMSEventR::bTofCluster;
TBranch* AMSEventR::bAntiCluster;
TBranch* AMSEventR::bTrRawCluster;
TBranch* AMSEventR::bTrCluster;
TBranch* AMSEventR::bTrRecHit;
TBranch* AMSEventR::bTrTrack;
TBranch* AMSEventR::bTrdRawHit;
TBranch* AMSEventR::bTrdCluster;
TBranch* AMSEventR::bTrdSegment;
TBranch* AMSEventR::bTrdTrack;
TBranch* AMSEventR::bLevel1;
TBranch* AMSEventR::bLevel3;
TBranch* AMSEventR::bBeta;
TBranch* AMSEventR::bVertex;
TBranch* AMSEventR::bCharge;
TBranch* AMSEventR::bParticle;
TBranch* AMSEventR::bAntiMCCluster;
TBranch* AMSEventR::bTrMCCluster;
TBranch* AMSEventR::bTofMCCluster;
TBranch* AMSEventR::bTrdMCCluster;
TBranch* AMSEventR::bRichMCCluster;
TBranch* AMSEventR::bMCTrack;
TBranch* AMSEventR::bMCEventg;
TBranch* AMSEventR::bAux;

void* AMSEventR::vHeader=0;
void* AMSEventR::vEcalHit=0;
void* AMSEventR::vEcalCluster=0;
void* AMSEventR::vEcal2DCluster=0;
void* AMSEventR::vEcalShower=0;
void* AMSEventR::vRichHit=0;
void* AMSEventR::vRichRing=0;
void* AMSEventR::vTofRawCluster=0;
void* AMSEventR::vTofRawSide=0;
void* AMSEventR::vTofCluster=0;
void* AMSEventR::vAntiCluster=0;
void* AMSEventR::vTrRawCluster=0;
void* AMSEventR::vTrCluster=0;
void* AMSEventR::vTrRecHit=0;
void* AMSEventR::vTrTrack=0;
void* AMSEventR::vTrdRawHit=0;
void* AMSEventR::vTrdCluster=0;
void* AMSEventR::vTrdSegment=0;
void* AMSEventR::vTrdTrack=0;
void* AMSEventR::vLevel1=0;
void* AMSEventR::vLevel3=0;
void* AMSEventR::vBeta=0;
void* AMSEventR::vVertex=0;
void* AMSEventR::vCharge=0;
void* AMSEventR::vParticle=0;
void* AMSEventR::vAntiMCCluster=0;
void* AMSEventR::vTrMCCluster=0;
void* AMSEventR::vTofMCCluster=0;
void* AMSEventR::vTrdMCCluster=0;
void* AMSEventR::vRichMCCluster=0;
void* AMSEventR::vMCTrack=0;
void* AMSEventR::vMCEventg=0;
void* AMSEventR::vAux=0;


char  AntiClusterR::_Info[255];
char  TofClusterR::_Info[255];
char  ParticleR::_Info[255];
char TrRecHitR::_Info[255];
char TrTrackR::_Info[255];
char TrdTrackR::_Info[255];
char TrdClusterR::_Info[255];
char EcalClusterR::_Info[255];
char EcalShowerR::_Info[255];
char RichHitR::_Info[255];
char RichRingR::_Info[255];
char TrMCClusterR::_Info[255];
char MCEventgR::_Info[255];
char Level1R::_Info[255];
char Level3R::_Info[512];
char HeaderR::_Info[255];


TTree*     AMSEventR::_Tree=0;
TTree*     AMSEventR::_ClonedTree=0;
AMSEventR* AMSEventR::_Head=0;
int AMSEventR::_Count=0;
int AMSEventR::_Entry=-1;
char* AMSEventR::_Name="ev.";   

void AMSEventR::SetBranchA(TTree *fChain){
     _Head=this;
     char tmp[255];
     

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fHeader");
     vHeader=&fHeader;
     fChain->SetBranchAddress(tmp,&fHeader);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalHit");
     vEcalHit=&fEcalHit;
     fChain->SetBranchAddress(tmp,&fEcalHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalCluster");
     vEcalCluster=&fEcalCluster;
     fChain->SetBranchAddress(tmp,&fEcalCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcal2DCluster");
     vEcal2DCluster=&fEcal2DCluster;
     fChain->SetBranchAddress(tmp,&fEcal2DCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalShower");
     vEcalShower=&fEcalShower;
     fChain->SetBranchAddress(tmp,&fEcalShower);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichHit");
     vRichHit=&fRichHit;
     fChain->SetBranchAddress(tmp,&fRichHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichRing");
     vRichRing=&fRichRing;
     fChain->SetBranchAddress(tmp,&fRichRing);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawCluster");
     vTofRawCluster=&fTofRawCluster;
     fChain->SetBranchAddress(tmp,&fTofRawCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawSide");
     vTofRawSide=&fTofRawSide;
     fChain->SetBranchAddress(tmp,&fTofRawSide);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofCluster");
     vTofCluster=&fTofCluster;
     fChain->SetBranchAddress(tmp,&fTofCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiCluster");
     vAntiCluster=&fAntiCluster;
     fChain->SetBranchAddress(tmp,&fAntiCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRawCluster");
     vTrRawCluster=&fTrRawCluster;
     fChain->SetBranchAddress(tmp,&fTrRawCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrCluster");
     vTrCluster=&fTrCluster;
     fChain->SetBranchAddress(tmp,&fTrCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRecHit");
     vTrRecHit=&fTrRecHit;
     fChain->SetBranchAddress(tmp,&fTrRecHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrTrack");
     vTrTrack=&fTrTrack;
     fChain->SetBranchAddress(tmp,&fTrTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdRawHit");
     vTrdRawHit=&fTrdRawHit;
     fChain->SetBranchAddress(tmp,&fTrdRawHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdCluster");
     vTrdCluster=&fTrdCluster;
     fChain->SetBranchAddress(tmp,&fTrdCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdSegment");
     vTrdSegment=&fTrdSegment;
     fChain->SetBranchAddress(tmp,&fTrdSegment);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdTrack");
     vTrdTrack=&fTrdTrack;
     fChain->SetBranchAddress(tmp,&fTrdTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel1");
     vLevel1=&fLevel1;
     fChain->SetBranchAddress(tmp,&fLevel1);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel3");
     vLevel3=&fLevel3;
     fChain->SetBranchAddress(tmp,&fLevel3);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fBeta");
     vBeta=&fBeta;
     fChain->SetBranchAddress(tmp,&fBeta);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fCharge");
     vCharge=&fCharge;
     fChain->SetBranchAddress(tmp,&fCharge);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fVertex");
     vVertex=&fVertex;
     fChain->SetBranchAddress(tmp,&fVertex);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fParticle");
     vParticle=&fParticle;
     fChain->SetBranchAddress(tmp,&fParticle);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiMCCluster");
     vAntiMCCluster=&fAntiMCCluster;
     fChain->SetBranchAddress(tmp,&fAntiMCCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrMCCluster");
     vTrMCCluster=&fTrMCCluster;
     fChain->SetBranchAddress(tmp,&fTrMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofMCCluster");
     vTofMCCluster=&fTofMCCluster;
     fChain->SetBranchAddress(tmp,&fTofMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdMCCluster");
     vTrdMCCluster=&fTrdMCCluster;
     fChain->SetBranchAddress(tmp,&fTrdMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichMCCluster");
     vRichMCCluster=&fRichMCCluster;
     fChain->SetBranchAddress(tmp,&fRichMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCTrack");
     vMCTrack=&fMCTrack;
     fChain->SetBranchAddress(tmp,&fMCTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCEventg");
     vMCEventg=&fMCEventg;
     fChain->SetBranchAddress(tmp,&fMCEventg);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAux");
     vAux=&fAux;
     fChain->SetBranchAddress(tmp,&fAux);
    }

}



void AMSEventR::ReSetBranchA(TTree *fChain){
     char tmp[255];

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fHeader");
     fChain->SetBranchAddress(tmp,vHeader);
    }
   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalHit");
     fChain->SetBranchAddress(tmp,vEcalHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalCluster");
     fChain->SetBranchAddress(tmp,vEcalCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcal2DCluster");
     fChain->SetBranchAddress(tmp,vEcal2DCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalShower");
     fChain->SetBranchAddress(tmp,vEcalShower);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichHit");
     fChain->SetBranchAddress(tmp,vRichHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichRing");
     fChain->SetBranchAddress(tmp,vRichRing);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawCluster");
     fChain->SetBranchAddress(tmp,vTofRawCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawSide");
     fChain->SetBranchAddress(tmp,vTofRawSide);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofCluster");
     fChain->SetBranchAddress(tmp,vTofCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiCluster");
     fChain->SetBranchAddress(tmp,vAntiCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRawCluster");
     fChain->SetBranchAddress(tmp,vTrRawCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrCluster");
     fChain->SetBranchAddress(tmp,vTrCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRecHit");
     fChain->SetBranchAddress(tmp,vTrRecHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrTrack");
     fChain->SetBranchAddress(tmp,vTrTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdRawHit");
     fChain->SetBranchAddress(tmp,vTrdRawHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdCluster");
     fChain->SetBranchAddress(tmp,vTrdCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdSegment");
     fChain->SetBranchAddress(tmp,vTrdSegment);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdTrack");
     fChain->SetBranchAddress(tmp,vTrdTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel1");
     fChain->SetBranchAddress(tmp,vLevel1);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel3");
     fChain->SetBranchAddress(tmp,vLevel3);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fBeta");
     fChain->SetBranchAddress(tmp,vBeta);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fCharge");
     fChain->SetBranchAddress(tmp,vCharge);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fVertex");
     fChain->SetBranchAddress(tmp,vVertex);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fParticle");
     fChain->SetBranchAddress(tmp,vParticle);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiMCCluster");
     fChain->SetBranchAddress(tmp,vAntiMCCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrMCCluster");
     fChain->SetBranchAddress(tmp,vTrMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofMCCluster");
     fChain->SetBranchAddress(tmp,vTofMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdMCCluster");
     fChain->SetBranchAddress(tmp,vTrdMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichMCCluster");
     fChain->SetBranchAddress(tmp,vRichMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCTrack");
     fChain->SetBranchAddress(tmp,vMCTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCEventg");
     fChain->SetBranchAddress(tmp,vMCEventg);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAux");
     fChain->SetBranchAddress(tmp,vAux);
    }
}


void AMSEventR::GetBranch(TTree *fChain){
     char tmp[255];

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fHeader");
     bHeader = fChain->GetBranch(tmp);
    }
   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalHit");
     bEcalHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalCluster");
     bEcalCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcal2DCluster");
     bEcal2DCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalShower");
     bEcalShower=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichHit");
     bRichHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichRing");
     bRichRing=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawCluster");
     bTofRawCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawSide");
     bTofRawSide=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofCluster");
     bTofCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiCluster");
     bAntiCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRawCluster");
     bTrRawCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrCluster");
     bTrCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRecHit");
     bTrRecHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrTrack");
     bTrTrack=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdRawHit");
     bTrdRawHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdCluster");
     bTrdCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdSegment");
     bTrdSegment=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdTrack");
     bTrdTrack=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel1");
     bLevel1=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel3");
     bLevel3=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fBeta");
     bBeta=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fCharge");
     bCharge=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fVertex");
     bVertex=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fParticle");
     bParticle=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiMCCluster");
     bAntiMCCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrMCCluster");
     bTrMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofMCCluster");
     bTofMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdMCCluster");
     bTrdMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichMCCluster");
     bRichMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCTrack");
     bMCTrack=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCEventg");
     bMCEventg=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAux");
     bAux=fChain->GetBranch(tmp);
    }
}



void AMSEventR::GetBranchA(TTree *fChain){
     char tmp[255];

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fHeader");
     vHeader = fChain->GetBranch(tmp)->GetAddress();
    }
   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalHit");
     vEcalHit=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalCluster");
     vEcalCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcal2DCluster");
     vEcal2DCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalShower");
     vEcalShower=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichHit");
     vRichHit=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichRing");
     vRichRing=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawCluster");
     vTofRawCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawSide");
     vTofRawSide=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofCluster");
     vTofCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiCluster");
     vAntiCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRawCluster");
     vTrRawCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrCluster");
     vTrCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRecHit");
     vTrRecHit=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrTrack");
     vTrTrack=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdRawHit");
     vTrdRawHit=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdCluster");
     vTrdCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdSegment");
     vTrdSegment=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdTrack");
     vTrdTrack=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel1");
     vLevel1=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel3");
     vLevel3=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fBeta");
     vBeta=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fCharge");
     vCharge=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fVertex");
     vVertex=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fParticle");
     vParticle=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiMCCluster");
     vAntiMCCluster=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrMCCluster");
     vTrMCCluster=fChain->GetBranch(tmp)->GetAddress();
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofMCCluster");
     vTofMCCluster=fChain->GetBranch(tmp)->GetAddress();
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdMCCluster");
     vTrdMCCluster=fChain->GetBranch(tmp)->GetAddress();
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichMCCluster");
     vRichMCCluster=fChain->GetBranch(tmp)->GetAddress();
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCTrack");
     vMCTrack=fChain->GetBranch(tmp)->GetAddress();
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCEventg");
     vMCEventg=fChain->GetBranch(tmp)->GetAddress();
    }

}



void AMSEventR::SetCont(){
// Set container sizes;
 fHeader.EcalHits=fEcalHit.size();
 fHeader.EcalClusters=fEcalCluster.size();
 fHeader.Ecal2DClusters=fEcal2DCluster.size();
 fHeader.EcalShowers=fEcalShower.size();
 fHeader.RichHits=fRichHit.size();
 fHeader.RichRings=fRichRing.size();
 fHeader.TofRawClusters=fTofRawCluster.size();
 fHeader.TofRawSides=fTofRawSide.size();
 fHeader.TofClusters=fTofCluster.size();
 fHeader.AntiClusters=fAntiCluster.size();
 fHeader.TrRawClusters=fTrRawCluster.size();
 fHeader.TrClusters=fTrCluster.size();
 fHeader.TrRecHits=fTrRecHit.size();
 fHeader.TrTracks=fTrTrack.size();
 fHeader.TrdRawHits=fTrdRawHit.size();
 fHeader.TrdClusters=fTrdCluster.size();
 fHeader.TrdSegments=fTrdSegment.size();
 fHeader.TrdTracks=fTrdTrack.size();
 fHeader.Level1s=fLevel1.size();
 fHeader.Level3s=fLevel3.size();
 fHeader.Betas=fBeta.size();
 fHeader.Vertexs=fVertex.size();
 fHeader.Charges=fCharge.size();
 fHeader.Particles=fParticle.size();
 fHeader.AntiMCClusters=fAntiMCCluster.size();
 fHeader.TrMCClusters=fTrMCCluster.size();
 fHeader.TofMCClusters=fTofMCCluster.size();
 fHeader.TrdMCClusters=fTrdMCCluster.size();
 fHeader.RichMCClusters=fRichMCCluster.size();
 fHeader.MCTracks=fMCTrack.size();
 fHeader.MCEventgs=fMCEventg.size();
// cout <<" fHeader.TrRecHits "<<fHeader.TrRecHits<<endl;
}
bool AMSEventR::ReadHeader(int entry){
    static unsigned int runo=0;
    static unsigned int evento=0;
    static const int probe=100;
    static double dif;
    static double difm;
    static double dif2;
    if(this!=_Head){
       cerr<<" AMSEventR::ReadHeader-S-WrongHeadPointer"<<endl;
      _Entry=entry;
       clear();
       return false;
     }
   _Entry=entry;
    memset((char*)(&fHeader)+sizeof(void*),0,sizeof(fHeader)-sizeof(void*));
    int i=bHeader->GetEntry(entry);
    clear();
   if(i>0){
    if(_Entry==0)cout <<"AMSEventR::ReadHeader-I-Version/OS "<<Version()<<"/"<<OS()<<endl;
     if(Version()<160){
// Fix rich rings
      NRichHit();
      for(int i=0;i<NRichRing();i++){
        RichRingR *ring=pRichRing(i);
        ring->FillRichHits(i);
      }
     }
    if(fHeader.Run!=runo){
     cout <<"AMSEventR::ReadHeader-I-NewRun "<<fHeader.Run<<endl;
     runo=fHeader.Run;
     if(evento>0){
      fService.TotalTrig+=(int)dif/2; 
     }
     dif=0;
     difm=0;
     dif2=0;
    }
    else if(_Entry<probe){
     if(difm==0){
      fService.TotalTrig+=Event()-evento;
     }
     if(Event()-evento>difm)difm=Event()-evento;
     dif+=Event()-evento;
     dif2+=(Event()-evento)*(Event()-evento);
     fService.TotalTrig+=Event()-evento;
    }
    else if(_Entry==probe){
     dif/=probe-1;
     dif2/=probe-1;
     dif2=sqrt(dif2-dif*dif);
     dif2=difm+20*dif2+1;
     fService.TotalTrig+=Event()-evento;
    }
    else{
     if(Event()-evento<dif2 || dif2!=dif2){
      fService.TotalTrig+=Event()-evento;
     }
     else{
      cerr<<"HeaderR-W-EventSeqSeemsToBeBroken "<<Event()<<" "<<evento<<" "<<dif2<<endl;
      fService.TotalTrig++;
     }
    }
    evento=Event();
   }
   else fService.BadEv++;    
    fService.TotalEv++;
  
    return i>0;
}



AMSEventR::AMSEventR():TSelector(){
 if(_Count++)cerr<<"AMSEventR::ctor-W-OnlyOneSingletonAllowed "<<this<<" "<<_Count<<endl;
 else cout<<"AMSEventR::ctor-I-SingletonInitialized "<<this<<endl;


fEcalHit.reserve(MAXECHITS);
fEcalCluster.reserve(MAXECCLUST);
fEcal2DCluster.reserve(MAXEC2DCLUST);
fEcalShower.reserve(MAXECSHOW);

fRichHit.reserve(MAXRICHITS);
fRichRing.reserve(MAXRICHRIN);

fTofRawCluster.reserve(MAXTOFRAW);
fTofRawSide.reserve(MAXTOFRAWS);
fTofCluster.reserve(MAXTOF);
fAntiCluster.reserve(MAXANTICL);

fTrRawCluster.reserve(MAXTRRAW);
fTrCluster.reserve(MAXTRCL);
fTrRecHit.reserve(MAXTRRH02);
fTrTrack.reserve(MAXTRTR02);

fTrdRawHit.reserve(MAXTRDRHT );
fTrdCluster.reserve(MAXTRDCL );
fTrdSegment.reserve(MAXTRDSEG);
fTrdTrack.reserve(MAXTRDTRK);

fLevel1.reserve(MAXLVL1);
fLevel3.reserve(MAXLVL3);

fBeta.reserve(MAXBETA02);
fCharge.reserve(MAXCHARGE02);
fVertex.reserve(2);
fParticle.reserve(MAXPART02);

fAntiMCCluster.reserve(MAXANTIMC);
fTofMCCluster.reserve(MAXTOFMC);
fTrMCCluster.reserve(MAXTRCLMC);
fTrdMCCluster.reserve(MAXTRDCLMC);
fRichMCCluster.reserve(MAXRICMC);

fMCTrack.reserve(MAXMCVOL);
fMCEventg.reserve(MAXMCG02);
}

void AMSEventR::clear(){
fEcalHit.clear();
fEcalCluster.clear();
fEcal2DCluster.clear();
fEcalShower.clear();

fRichHit.clear();
fRichRing.clear();

fTofRawCluster.clear();
fTofRawSide.clear();
fTofCluster.clear();
fAntiCluster.clear();

fTrRawCluster.clear();
fTrCluster.clear();
fTrRecHit.clear();
fTrTrack.clear();

fTrdRawHit.clear();
fTrdCluster.clear();
fTrdSegment.clear();
fTrdTrack.clear();

fLevel1.clear();
fLevel3.clear();

fBeta.clear();
fCharge.clear();
fVertex.clear();
fParticle.clear();

fAntiMCCluster.clear();
fTofMCCluster.clear();
fTrMCCluster.clear();
fTrdMCCluster.clear();
fRichMCCluster.clear();

fMCTrack.clear();
fMCEventg.clear();

}



//------------- AddAMSObject 
#ifndef __ROOTSHAREDLIBRARY__



void AMSEventR::AddAMSObject(AMSEcalHit *ptr)
{
  fEcalHit.push_back(EcalHitR(ptr));
  ptr->SetClonePointer(fEcalHit.size()-1);
}

void AMSEventR::AddAMSObject(Ecal1DCluster *ptr) 
{
  if (ptr) {
  fEcalCluster.push_back(EcalClusterR(ptr));
  ptr->SetClonePointer(fEcalCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- Ecal1DCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSEcal2DCluster *ptr) {
  if (ptr) {
  fEcal2DCluster.push_back(Ecal2DClusterR(ptr));
  ptr->SetClonePointer(fEcal2DCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- (Ecal2DCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSEcalShower *ptr) 
{
  if (ptr) {
  fEcalShower.push_back(EcalShowerR(ptr));
  ptr->SetClonePointer(fEcalShower.size()-1);
  }  else {
   cout<<"AddAMSObject -E- EcalShower ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSRichRawEvent *ptr, float x, float y, float z)
{
  if (ptr) {
  fRichHit.push_back(RichHitR(ptr,x,y,z));
  ptr->SetClonePointer(fRichHit.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSRichRing *ptr)
{
  fRichRing.push_back(RichRingR(ptr));
  ptr->SetClonePointer(fRichRing.size()-1);
}


void AMSEventR::AddAMSObject(AMSVtx *ptr){
  fVertex.push_back(VertexR(ptr));
  ptr->SetClonePointer(fVertex.size()-1);
}

void AMSEventR::AddAMSObject(AMSTOFCluster *ptr){
  if (ptr) {
  fTofCluster.push_back(TofClusterR(ptr));
  ptr->SetClonePointer(fTofCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTofCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TOF2RawCluster *ptr)
{
  if (ptr) {
  fTofRawCluster.push_back(TofRawClusterR(ptr));
  ptr->SetClonePointer(fTofRawCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTOF2RawCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TOF2RawSide *ptr)
{
  if (ptr) {
  fTofRawSide.push_back(TofRawSideR(ptr));
  ptr->SetClonePointer(fTofRawSide.size()-1);
  }  else {
   cout<<"AddAMSObject -E- TOF2RawSide ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSAntiCluster *ptr)
{
  if (ptr) {
  fAntiCluster.push_back(AntiClusterR(ptr));
  ptr->SetClonePointer(fAntiCluster.size()-1);  
  }  else {
    cout<<"AddAMSObject -E- AMSAntiCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrRawCluster *ptr)
{
  if (ptr) {
  fTrRawCluster.push_back(TrRawClusterR(ptr));
  ptr->SetClonePointer( fTrRawCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- TrRawCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrCluster *ptr){
  if (ptr) {
  fTrCluster.push_back(TrClusterR(ptr));
  ptr->SetClonePointer(fTrCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTrRecHit *ptr)
{
  if (ptr) {
    if(fTrRecHit.size()>root::MAXTRRH02*2 && !ptr->checkstatus(AMSDBc::USED))return;
  fTrRecHit.push_back(TrRecHitR(ptr));
  ptr->SetClonePointer(fTrRecHit.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrRecHit ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrTrack *ptr)
{
  if (ptr) {
  fTrTrack.push_back(TrTrackR(ptr));
  ptr->SetClonePointer( fTrTrack.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrTrack ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDRawHit *ptr)
{
  if (ptr) {
  fTrdRawHit.push_back(TrdRawHitR(ptr));
  ptr->SetClonePointer( fTrdRawHit.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDRawHit ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDCluster *ptr)
{
  if (ptr) {
  fTrdCluster.push_back(TrdClusterR(ptr));
  ptr->SetClonePointer( fTrdCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDSegment *ptr)
{
  if (ptr) {
  fTrdSegment.push_back(TrdSegmentR(ptr));
  ptr->SetClonePointer(fTrdSegment.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDSegment ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDTrack *ptr)
{
  if (ptr) {
  fTrdTrack.push_back(TrdTrackR(ptr));
  ptr->SetClonePointer(fTrdTrack.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDTrack ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(Trigger2LVL1 *ptr)
{
  if (ptr) {
  fLevel1.push_back(Level1R(ptr));
  ptr->SetClonePointer(fLevel1.size()-1);
  }  else {
   cout<<"AddAMSObject -E- Trigger2LVL1 ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TriggerLVL302 *ptr)
{
  if (ptr) {
  fLevel3.push_back(Level3R(ptr));
  ptr->SetClonePointer(fLevel3.size()-1);
  }  else {
   cout<<"AddAMSObject -E- TriggerLVL302 ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSBeta *ptr)
{
  if (ptr) {
   fBeta.push_back(BetaR(ptr));
   ptr->SetClonePointer(fBeta.size()-1);
   }  else {
     cout<<"AddAMSObject -E- AMSBeta ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSCharge *ptr, float probtof[],int chintof[], 
                               float probtr[], int chintr[], float probrc[], 
                               int chinrc[], float proballtr)
{
  if (ptr) {
   fCharge.push_back(ChargeR(ptr, probtof, chintof, probtr, chintr, probrc, chinrc, proballtr));
   ptr->SetClonePointer(fCharge.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSCharge ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSParticle *ptr, float phi, float phigl)
{
  if (ptr) {
  fParticle.push_back(ParticleR(ptr, phi, phigl));
  ptr->SetClonePointer(fParticle.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSParticle ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSAntiMCCluster *ptr)
{
  if (ptr) {
   if( fAntiMCCluster.size()>root::MAXANTIMC*2)return; 
  fAntiMCCluster.push_back(AntiMCClusterR(ptr));
  ptr->SetClonePointer(fAntiMCCluster.size()-1);  
  }  else {
    cout<<"AddAMSObject -E- AMSAntiMCCluster ptr is NULL"<<endl;
  }
}






void AMSEventR::AddAMSObject(AMSRichMCHit *ptr, int _numgen)
{
  fRichMCCluster.push_back(RichMCClusterR(ptr,_numgen));
  ptr->SetClonePointer(fRichMCCluster.size()-1);
}





void AMSEventR::AddAMSObject(AMSTOFMCCluster *ptr)
{
  if (ptr) {
  fTofMCCluster.push_back(TofMCClusterR(ptr));
  ptr->SetClonePointer(fTofMCCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTofMCCluster ptr is NULL"<<endl;
  }
}




void AMSEventR::AddAMSObject(AMSTrMCCluster *ptr)
{
  if (ptr) {
   if( fTrMCCluster.size() > root::MAXTRCLMC * 2)return;
  fTrMCCluster.push_back(TrMCClusterR(ptr));
  ptr->SetClonePointer( fTrMCCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrMCCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDMCCluster *ptr)
{
  if (ptr) {
  fTrdMCCluster.push_back(TrdMCClusterR(ptr));
  ptr->SetClonePointer(fTrdMCCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDMCCluster ptr is NULL"<<endl;
  }
}



void AMSEventR::AddAMSObject(AMSmceventg *ptr)
{
  if (ptr) {
  if(fMCEventg.size()>root::MAXMCG02 * 2)return;
  fMCEventg.push_back(MCEventgR(ptr));
  ptr->SetClonePointer( fMCEventg.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSmceventg ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSmctrack *ptr)
{
  if (ptr) {
  fMCTrack.push_back(MCTrackR(ptr));
  ptr->SetClonePointer(fMCTrack.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSmctrack ptr is NULL"<<endl;
  }
}

#endif




#ifndef __ROOTSHAREDLIBRARY__
void HeaderR::Set(EventNtuple02* ptr){

    Run=       ptr->Run;
    RunType=   ptr->RunType;
    Event=     ptr->Eventno;
    Status[0]= ptr->EventStatus[0];
    Status[1]= ptr->EventStatus[1];
    Raw=       ptr->RawWords%(1<<18);
    Version=   (ptr->RawWords)>>18;
    Time[0]=   ptr->Time[0];
    Time[1]=   ptr->Time[1];
    RadS=      ptr->RadS;
    ThetaS=    ptr->ThetaS;
    PhiS=      ptr->PhiS;
    ISSEqAsc=        ptr->ISSEqAsc;
    ISSEqDec=       ptr->ISSEqDec;
    ISSGalLat=      ptr->ISSGalLat;
    ISSGalLong=     ptr->ISSGalLong;
    AMSEqAsc=     ptr->AMSEqAsc;
    AMSEqDec=    ptr->AMSEqDec;
    AMSGalLat=   ptr->AMSGalLat;
    AMSGalLong=  ptr->AMSGalLong;
    Yaw=       ptr->Yaw;
    Pitch=     ptr->Pitch;
    Roll=      ptr->Roll;
    VelocityS= ptr->VelocityS;
    VelTheta=  ptr->VelTheta;
    VelPhi=    ptr->VelPhi;
    ThetaM=    ptr->ThetaM;

    
}
#endif





AntiClusterR::AntiClusterR(AMSAntiCluster *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Sector = ptr->_sector;
  Ntimes = ptr->_ntimes;
  Npairs = ptr->_npairs;
  for(int i=0;i<Ntimes;i++)Times[i] = ptr->_times[i];
  for(int i=0;i<Ntimes;i++)Timese[i] = ptr->_etimes[i];
  Edep   = ptr->_edep;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) ErrorCoo[i] = ptr->_ecoo[i];
#endif
}

AntiMCClusterR::AntiMCClusterR(AMSAntiMCCluster *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->_idsoft;
  for (int i=0; i<3; i++) Coo[i]=ptr->_xcoo[i];
  TOF    = ptr->_tof;
  Edep   = ptr->_edep;
#endif
}



BetaR::BetaR(AMSBeta *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status   = ptr->_status;
  Pattern  = ptr->_Pattern;
  Beta     = ptr->_Beta;
  BetaC    = ptr->_BetaC;
  Error    = ptr->_InvErrBeta;
  ErrorC   = ptr->_InvErrBetaC;
  Chi2     = ptr->_Chi2;
  Chi2S    = ptr->_Chi2Space;
 fTrTrack    = -1;
#endif
}


ChargeR::ChargeR(AMSCharge *ptr, float probtof[],int chintof[], 
                           float probtr[], int chintr[], float probrc[],
                           int chinrc[], float proballtr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  ChargeTOF     = ptr->_ChargeTOF;
  ChargeTracker = ptr->_ChargeTracker;
  ChargeRich    = ptr->_ChargeRich;
  for (int i=0; i<4; i++) {
    ProbTOF[i] = probtof[i];
    ChInTOF[i] = chintof[i];
    ProbTracker[i] = probtr[i];
    ChInTracker[i] = chintr[i];
    ProbRich[i] = probrc[i];
    ChInRich[i] = chinrc[i];
  }
  ProbAllTracker= proballtr;
  TrunTOF       = ptr->_TrMeanTOF;
  TrunTOFD      = ptr->_TrMeanTOFD;
  TrunTracker   = ptr->_TrMeanTracker;
 fBeta=-1;
 fRichRing=-1;
#endif
}


Ecal2DClusterR::Ecal2DClusterR(AMSEcal2DCluster *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->getstatus();
  Proj   = ptr->_proj;
  Edep   = ptr->_EnergyC;
  Coo    = ptr->_Coo;
  Tan    = ptr->_Tan;
  Chi2   = ptr->_Chi2;
#endif
}

EcalClusterR::EcalClusterR(Ecal1DCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status  = ptr->_status;
  Proj    = ptr->_proj;
  Plane   = ptr->_plane;
  Left    = ptr->_Left;
  Center  = ptr->_MaxCell;
  Right   = ptr->_Right;
  Edep    = ptr->_EnergyC;
  SideLeak = ptr->_SideLeak;
  DeadLeak = ptr->_DeadLeak;
  for (int i=0; i<3; i++)Coo[i] = ptr->_Coo[i];
#endif
}

EcalShowerR::EcalShowerR(AMSEcalShower *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status      = ptr->_status;
  for (int i=0; i<3; i++) {
   Dir[i]   = ptr->_Dir[i];
   EMDir[i] = ptr->_EMDir[i];
   Entry[i] = ptr->_EntryPoint[i];
   Exit[i]  = ptr->_ExitPoint[i];
   CofG[i]  = ptr->_CofG[i];
  }
  ErDir   = ptr->_Angle3DError;
  Chi2Dir   = ptr->_AngleTrue3DChi2;
  FirstLayerEdep = ptr->_FrontEnergyDep;
  EnergyC   =   ptr->_EnergyC;
  Energy3C[0] = ptr->_Energy3C;
  Energy3C[1] = ptr->_Energy5C;
  Energy3C[2] = ptr->_Energy9C;
  ErEnergyC   = ptr->_ErrEnergyC;
  DifoSum     = ptr->_DifoSum;
  SideLeak    = ptr->_SideLeak;
  RearLeak    = ptr->_RearLeak;
  DeadLeak    = ptr->_DeadLeak;
  AttLeak     = ptr->_AttLeak;
  OrpLeak     = ptr->_OrpLeak;
  Orp2DEnergy = ptr->_Orp2DEnergy;
  Chi2Profile = ptr->_ProfilePar[4+ptr->_Direction*5];
  for (int i=0; i<4; i++) ParProfile[i] = ptr->_ProfilePar[i+ptr->_Direction*5];
  Chi2Trans = ptr->_TransFitChi2;
  for (int i=0; i<3; i++) SphericityEV[i] = ptr->_SphericityEV[i];
#endif
}

Level1R::Level1R(Trigger2LVL1 *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  PhysBPatt = ptr->_PhysBPatt;
  JMembPatt = ptr->_JMembPatt;
  TofFlag1 = ptr->_tofflag1;
  TofFlag2 = ptr->_tofflag1;
  for (int i=0; i<4; i++) {
    TofPatt1[i] = ptr->_tofpatt1[i];
    TofPatt2[i] = ptr->_tofpatt2[i];
  }
  AntiPatt = ptr->_antipatt;
  EcalFlag = ptr->_ecalflag;
  for(int i=0;i<6;i++){
    for(int j=0;j<3;j++)EcalPatt[i][j] = ptr->_ectrpatt[i][j];
  }
  EcalTrSum= ptr->_ectrsum;
  LiveTime   = ptr->_LiveTime;
  for(int i=0; i<6; i++)TrigRates[i]  = ptr->_TrigRates[i];
#endif
}

Level3R::Level3R(TriggerLVL302 *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  TOFTr     = ptr->_TOFTrigger;
  TRDTr     = ptr->_TRDTrigger;
  TrackerTr = ptr->_TrackerTrigger;
  MainTr    = ptr->_MainTrigger;
  Direction = ptr->_TOFDirection;
  NTrHits   = ptr->_NTrHits;
  NPatFound = ptr->_NPatFound;
  for (int i=0; i<2; i++) {Pattern[i] = ptr->_Pattern[i];}
  for (int i=0; i<2; i++) {Residual[i]= ptr->_Residual[i];}
  Time      = ptr->_Time;
  ELoss     = ptr->_TrEnergyLoss;
  TRDHits   = ptr->TRDAux._NHits[0]+ptr->TRDAux._NHits[1];;
  HMult     = ptr->TRDAux._HMult;;
  for (int i=0; i<2; i++) {TRDPar[i] = ptr->TRDAux._Par[i][0];}
  ECemag    = ptr->_ECemag;
  ECmatc    = ptr->_ECmatc;
  for (int i=0; i<4; i++) {ECTOFcr[i] = ptr->_ECtofcr[i];}

#endif
}

MCEventgR::MCEventgR(AMSmceventg *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Nskip    = ptr->_nskip;
  Particle = ptr->_ipart;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) Dir[i] = ptr->_dir[i];
  Momentum = ptr->_mom;
  Mass     = ptr->_mass;
  Charge   = ptr->_charge;
#endif
}           

MCTrackR::MCTrackR(AMSmctrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  RadL = ptr->_radl;
  AbsL = ptr->_absl;
  for (int i=0; i<3; i++) Pos[i]   = ptr->_pos[i];
  for (int i=0; i<4; i++) VolName[i] = ptr->_vname[i];
  VolName[4]='\0';
#endif
}

VertexR::VertexR(AMSVtx *ptr){
#ifndef __ROOTSHAREDLIBRARY__
 Momentum=ptr->getmom();
 ErrMomentum=ptr->geterrmom();
 Theta=ptr->gettheta();
 Phi=ptr->getphi();
 Mass=ptr->getmass();
 Status=ptr->getstatus();
 Charge=ptr->getcharge();
 Chi2=ptr->getchi2();
 Ndof=ptr->getndof();
 for(int i=0;i<3;i++)Vertex[i]=ptr->getvert()[i];
#endif
}


ParticleR::ParticleR(AMSParticle *ptr, float phi, float phigl)
{
#ifndef __ROOTSHAREDLIBRARY__
  Phi      = phi;
  PhiGl    = phigl;
  fBeta   = -1;
  fCharge = -1;
  fTrTrack  = -1;
  fTrdTrack    = -1;
  fRichRing   = -1;
  fEcalShower = -1;
  fVertex = -1;
  Particle     = ptr->_gpart[0];
  ParticleVice = ptr->_gpart[1];
  for (int i=0; i<2; i++) {Prob[i] = ptr->_prob[i];}
  FitMom   = ptr->_fittedmom[0];
  Mass     = ptr->_Mass;
  ErrMass  = ptr->_ErrMass;
  Momentum = ptr->_Momentum;
  ErrMomentum = ptr->_ErrMomentum;
  Beta     = ptr->_Beta;
  ErrBeta  = ptr->_ErrBeta;
  Charge   = ptr->_Charge;
  Theta    = ptr->_Theta;
  ThetaGl  = ptr->_ThetaGl;
  for (int i=0; i<3; i++) {Coo[i] = ptr->_Coo[i];}
  Cutoff   = ptr->_CutoffMomentum;
  for (int i=0; i<4; i++) {
    for (int j=0; j<3; j++) {
      TOFCoo[i][j] = ptr->_TOFCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      AntiCoo[i][j] = ptr->_AntiCoo[i][j];
    }
  }
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      EcalCoo[i][j] = ptr->_EcalSCoo[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    for (int j=0; j<3; j++) {
      TrCoo[i][j] = ptr->_TrCoo[i][j];
    }
  }
  for (int i=0; i<8; i++)  Local[i] = ptr->_Local[i];

  for (int i=0; i<3; i++) {TRDCoo[0][i] = ptr->_TRDCoo[0][i];}
  for (int i=0; i<3; i++) {TRDCoo[1][i] = ptr->_TRDCoo[1][i];}
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      RichCoo[i][j] = ptr->_RichCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {RichPath[i] = ptr->_RichPath[i];
                           RichPathBeta[i] = ptr->_RichPathBeta[i];}
  RichLength = ptr->_RichLength;
  RichParticles= ptr->_RichParticles;

  TRDLikelihood = ptr->_TRDLikelihood;
#endif
}




TofClusterR::TofClusterR(AMSTOFCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  Edep   = ptr->_edep;
  Edepd  = ptr->_edepd;
  Time   = ptr->_time;
  ErrTime= ptr->_etime;
  for (int i=0; i<3; i++) {
    Coo[i] = ptr->_Coo[i];
    ErrorCoo[i] = ptr->_ErrorCoo[i];
  }
#endif
}

TofMCClusterR::TofMCClusterR(AMSTOFMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->idsoft;
  for (int i=0; i<3; i++) {Coo[i] = ptr->xcoo[i];}
  TOF = ptr->tof;
  Edep= ptr->edep;
#endif
}

TofRawClusterR::TofRawClusterR(TOF2RawCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  for (int i=0; i<2; i++) adca[i]=ptr->_adca[i];
  for (int i=0; i<2; i++) adcd[i]=ptr->_adcd[i];
  for (int i=0; i<2; i++){
     for(int ip=0;ip<TOF2GC::PMTSMX;ip++){
       adcdr[i][ip]=ptr->_adcdr[i][ip];
     }
  }
  for (int i=0; i<2; i++) sdtm[i] =ptr->_sdtm[i];
  edepa  = ptr->_edepa;
  edepd  = ptr->_edepd;
  time   = ptr->_time;
  cool   = ptr->_timeD;
#endif
}

TofRawSideR::TofRawSideR(TOF2RawSide *ptr){
#ifndef __ROOTSHAREDLIBRARY__

  swid=ptr->_swid;
  hwid=ptr->_hwid;
  nftdc=ptr->_nftdc;
  for(int i=0; i<nftdc; i++)ftdc[i]=ptr->_ftdc[i];
  nstdc=ptr->_nstdc;
  for(int i=0; i<nstdc; i++)stdc[i]=ptr->_stdc[i];
  nsumh=ptr->_nsumh;
  for(int i=0; i<nsumh; i++)sumht[i]=ptr->_sumht[i];
  nsumsh=ptr->_nsumsh;
  for(int i=0; i<nsumsh; i++)sumsht[i]=ptr->_sumsht[i];
  adca=ptr->_adca;
  nadcd=ptr->_nadcd;
  for(int ip=0;ip<TOF2GC::PMTSMX;ip++)adcd[ip]=ptr->_adcd[ip];
  temp=ptr->_temp;
#endif
}

TrdClusterR::TrdClusterR(AMSTRDCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_layer;
  for (int i=0; i<3; i++) Coo[i]= ptr->_Coo[i];
  if(ptr->_CooDir[0]>0.9)Direction=0;
  else Direction=1;
  ClSizeR=ptr->_ClSizeR;
  ClSizeZ=ptr->_ClSizeZ;
  Multip = ptr->_Multiplicity;
  HMultip= ptr->_HighMultiplicity;
  EDep   = ptr->_Edep;
  fTrdRawHit =-1;
#endif
}

TrdMCClusterR::TrdMCClusterR(AMSTRDMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Layer  = ptr->_idsoft.getlayer();
  Ladder = ptr->_idsoft.getladder();
  Tube   = ptr->_idsoft.gettube();
  ParticleNo= ptr->_ipart;
  //  TrackNo= ptr->_itra;
  Edep   = ptr->_edep;
  Ekin   = ptr->_ekin;
  for (int i=0; i<3; i++) {Xgl[i] = ptr->_xgl[i];}
  Step   = ptr->_step;
#endif
}

TrdRawHitR::TrdRawHitR(AMSTRDRawHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Layer  = ptr->_id.getlayer();
  Ladder = ptr->_id.getladder();
  Tube   = ptr->_id.gettube();
  Amp    = ptr->Amp();
#endif
}

TrdSegmentR::TrdSegmentR(AMSTRDSegment *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status        = ptr->_status;
  Orientation   = ptr->_Orientation;
  for (int i=0; i<2; i++) {FitPar[i] = ptr->_FitPar[i];}
  Chi2          = ptr->_Chi2;
  Pattern       = ptr->_Pattern;
#endif
}

TrdTrackR::TrdTrackR(AMSTRDTrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  for (int i=0; i<3; i++) {
    Coo[i]   = ptr->_StrLine._Coo[i];
    ErCoo[i] = ptr->_StrLine._ErCoo[i];
  }
  Phi   = ptr->_StrLine._Phi;
  Theta = ptr->_StrLine._Theta;
  Chi2  = ptr->_StrLine._Chi2;
  Pattern = ptr->_BaseS._Pattern;
#endif
}

TrClusterR::TrClusterR(AMSTrCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr-> _Id.cmpt();
  Status = ptr->_status;
  NelemL = ptr->_NelemL;
  NelemR = ptr->_NelemR;
  Sum    = ptr->_Sum;
  Sigma  = ptr->_Sigma;
  Mean   = ptr->_Mean;
  RMS    = ptr->_Rms;
  ErrorMean = ptr->_ErrorMean;
  for (int i=0; i<ptr->getnelem(); i++)Amplitude.push_back(ptr->_pValues[i]);
#endif
}

TrMCClusterR::TrMCClusterR(AMSTrMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft  = ptr->_idsoft;
  TrackNo = ptr->_itra;
  for (int i=0; i<2; i++) {
         Left[i]   = ptr->_left[i];
         Center[i] = ptr->_center[i];
         Right[i]  = ptr->_right[i];
  }
  for (int i=0; i<3; i++) {
    Xca[i] = ptr->_xca[i];
    Xcb[i] = ptr->_xcb[i];
    Xgl[i] = ptr->_xgl[i];
  }
  Sum = ptr->_sum;
  for (int i=0; i<2; i++) {
    for (int j=0; j<5; j++) {
      SS[i][j] = ptr->_ss[i][j];
    }
  }
#endif
}

TrRawClusterR::TrRawClusterR(AMSTrRawCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  address = ptr->_address+ptr->_strip*10000;
  nelem   = ptr->_nelem;
  s2n     = ptr->_s2n;
#endif
}

TrRecHitR::TrRecHitR(AMSTrRecHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  fTrClusterX = -1;
  fTrClusterY = -1;
  Status= ptr->_status;
  Layer = ptr->_Layer;
  for (int i=0; i<3; i++) Hit[i]  = ptr->_Hit[i];
  for (int i=0; i<3; i++) EHit[i] = ptr->_EHit[i];
  Sum     = ptr->_Sum;
  DifoSum = ptr->_DifoSum;
  CofgX   = ptr->_cofgx;
  CofgY   = ptr->_cofgy;
  for (int i=0; i<3; i++) Bfield[i] = ptr->_Bfield[i];
#endif
}

TrTrackR::TrTrackR(AMSTrTrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status    = ptr->_status;
  Pattern   = ptr->_Pattern;
  Address   = ptr->_Address;
  LocDBAver       = ptr->_Dbase[0];
  GeaneFitDone    = ptr->_GeaneFitDone;
  AdvancedFitDone = ptr->_AdvancedFitDone;
  Chi2StrLine     = ptr->_Chi2StrLine;
  Chi2WithoutMS   = ptr->_Chi2WithoutMS;
  RigidityWithoutMS  = ptr->_RigidityWithoutMS;
  Chi2FastFit     = ptr->_Chi2FastFit;
  Rigidity        = ptr->_Ridgidity;
  ErrRigidity     = ptr->_ErrRidgidity;
  Theta           = ptr->_Theta;
  Phi             = ptr->_Phi;
  for (int i=0; i<3; i++) P0[i] = ptr->_P0[i];
  GChi2           = (float)ptr->_GChi2;
  GRigidity       = (float)ptr->_GRidgidity;;
  GErrRigidity    = (float)ptr->_GErrRidgidity;
  if(AdvancedFitDone){
  for (int i=0; i<2; i++) {
        HChi2[i]        = (float)ptr->_HChi2[i];
        HRigidity[i]    = (float)ptr->_HRidgidity[i];
        HErrRigidity[i] = (float)ptr->_HErrRidgidity[i];
        HTheta[i]       = (float)ptr->_HTheta[i];
        HPhi[i]         = (float)ptr->_HPhi[i];
        for (int j=0; j<3; j++)  HP0[i][j] = (float)ptr->_HP0[i][j];
  }
 } 
 else{
  for (int i=0; i<2; i++) {
        HChi2[i]        = 0;
        HRigidity[i]    = 0;
        HErrRigidity[i] = 0;
        HTheta[i]       = 0;
        HPhi[i]         = 0;
        for (int j=0; j<3; j++)  HP0[i][j] = 0;
  }
 }
  FChi2MS         = ptr->_Chi2MS;
  PiErrRig        = ptr->_PIErrRigidity;
  RigidityMS      = ptr->_RidgidityMS;
  PiRigidity      = ptr->_PIRigidity;
#endif
}


RichMCClusterR::RichMCClusterR(AMSRichMCHit *ptr, int _numgen){
#ifndef __ROOTSHAREDLIBRARY__
  Id        = ptr->_id;
  for (int i=0; i<3; i++) {
   Origin[i]    = ptr->_origin[i];
   Direction[i] = ptr->_direction[i];
  }
  Status       = ptr->_status;
  fRichHit = ptr->_hit;
  NumGen       = _numgen;
#endif
}

RichHitR::RichHitR(AMSRichRawEvent *ptr, float x, float y, float z){
#ifndef __ROOTSHAREDLIBRARY__
  if (ptr) {
   Channel = ptr->_channel;
   Counts  = ptr->_counts;
   Status  = ptr->_status;
   Npe     = ptr->getnpe();
   Coo[0]=x;
   Coo[1]=y;
   Coo[2]=z;
  } else {
    cout<<"RICEventR -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
#endif
}

void RichRingR::FillRichHits(int ring){
  fRichHit.clear();
  for(int i=0;i<AMSEventR::Head()->NRichHit();i++){
   RichHitR hit=AMSEventR::Head()->RichHit(i);
   if((hit.Status>>ring)%2){
    fRichHit.push_back(i);
   }
  }
  if(Used!=fRichHit.size())cerr<<" problem hits for ring "<<ring<<" "<<Used<<" "<<fRichHit.size()<<endl;
}

RichRingR::RichRingR(AMSRichRing *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  fTrTrack = -1;
  if (ptr) {
    Used  = ptr->_used;
    UsedM = ptr->_mused;
    Beta  = ptr->_beta;
    ErrorBeta = ptr->_errorbeta;
    Chi2   = ptr->_quality;
    BetaRefit = ptr->_wbeta;
    Status    = ptr->_status;
    NpCol= ptr->_collected_npe;
    NpExp     = ptr->_npexp;
    Prob    = ptr->_probkl;
    KDist   = ptr->_kdist;  
    PhiSpread = ptr->_phi_spread;
    UDist = ptr->_unused_dist;
    Theta   =ptr->_theta;
    ErrorTheta  =ptr->_errortheta;
    for(int i=0;i<3;i++){
      TrRadPos[i]  = ptr->_radpos[i];
      TrPMTPos[i]  = ptr->_pmtpos[i];
    }
    //+LIP
    lipHitsUsed           = ptr->_liphused;
    lipThetaC             = ptr->_lipthc;
    lipBeta               = ptr->_lipbeta;
    lipErrorBeta          = ptr->_lipebeta;
    lipLikelihoodProb     = ptr->_liplikep;
    lipChi2               = ptr->_lipchi2;
    lipRecProb            = ptr->_liprprob;
    //ENDofLIP    
  } else {
    cout<<"RICRingR -E- AMSRichRing ptr is NULL"<<endl;
  }
#endif
}

EcalHitR::EcalHitR(AMSEcalHit *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Idsoft = ptr->_idsoft;
  Proj   = ptr->_proj;
  Plane  = ptr->_plane;
  Cell   = ptr->_cell;
  Edep   = ptr->_edep;
  EdCorr = ptr->_edepc;
  if (Proj) {
   Coo[0]= ptr->_cool;
   Coo[1]= ptr->_coot;
  }
  else{     //<-- x-proj
    Coo[0]= ptr->_coot;
    Coo[1]= ptr->_cool;
  }
  Coo[2]  = ptr->_cooz;

  AttCor  = ptr->_attcor;
  AMSECIds ic(ptr->getid());
  ADC[0] = ptr->_adc[0];
  ADC[1] = ptr->_adc[1]*ic.gethi2lowr();
  ADC[2] = ptr->_adc[2]*ic.getan2dyr();

  Ped[0] = ic.getped(0);
  Ped[1] = ic.getped(1);
  Ped[2] = ic.getpedd();
  Gain   = ic.getgain();

#endif
}



// Pointer related functions advanced users only
//


   EcalHitR* EcalClusterR::pEcalHit(unsigned int i){
     return (AMSEventR::Head() && i<fEcalHit.size())?AMSEventR::Head()->pEcalHit(fEcalHit[i]):0;
   }



   EcalClusterR* Ecal2DClusterR::pEcalCluster(unsigned int i){
     return (AMSEventR::Head() && i<fEcalCluster.size())?AMSEventR::Head()->pEcalCluster(fEcalCluster[i]):0;
   }

   Ecal2DClusterR* EcalShowerR::pEcal2DCluster(unsigned int i){
     return (AMSEventR::Head() && i<fEcal2DCluster.size())?AMSEventR::Head()->pEcal2DCluster(fEcal2DCluster[i]):0;
   }


   TrTrackR* RichRingR::pTrTrack(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
   }


   TofRawClusterR* TofClusterR::pTofRawCluster(unsigned int i){
     return (AMSEventR::Head() && i<fTofRawCluster.size())?AMSEventR::Head()->pTofRawCluster(fTofRawCluster[i]):0;
   }


   TrClusterR* TrRecHitR::pTrCluster(char xy){
     return (AMSEventR::Head() )?AMSEventR::Head()->pTrCluster(xy=='x'?fTrClusterX:fTrClusterY):0;
   }


   TrRecHitR* TrTrackR::pTrRecHit(unsigned int i){
     return (AMSEventR::Head() && i<fTrRecHit.size())?AMSEventR::Head()->pTrRecHit(fTrRecHit[i]):0;
   }

   TrdRawHitR* TrdClusterR::pTrdRawHit(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pTrdRawHit(fTrdRawHit):0;
   }


   TrdClusterR* TrdSegmentR::pTrdCluster(unsigned int i){
     return (AMSEventR::Head() && i<fTrdCluster.size())?AMSEventR::Head()->pTrdCluster(fTrdCluster[i]):0;
   }

   TrdSegmentR* TrdTrackR::pTrdSegment(unsigned int i){
     return (AMSEventR::Head() && i<fTrdSegment.size())?AMSEventR::Head()->pTrdSegment(fTrdSegment[i]):0;
   }


   TofClusterR* BetaR::pTofCluster(unsigned int i){
     return (AMSEventR::Head() && i<fTofCluster.size())?AMSEventR::Head()->pTofCluster(fTofCluster[i]):0;
   }

   TrTrackR* BetaR::pTrTrack(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
   }

   BetaR* ChargeR::pBeta(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pBeta(fBeta):0;
   }
   RichRingR* ChargeR::pRichRing(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pRichRing(fRichRing):0;
   }


   RichRingR* ParticleR::pRichRing(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pRichRing(fRichRing):0;
   }

   BetaR* ParticleR::pBeta(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pBeta(fBeta):0;
   }

   ChargeR* ParticleR::pCharge(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pCharge(fCharge):0;
   }

   TrTrackR* VertexR::pTrTrack(unsigned int i){
     return (AMSEventR::Head() && i<fTrTrack.size())?AMSEventR::Head()->pTrTrack(fTrTrack[i]):0;
   }

   TrTrackR* ParticleR::pTrTrack(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
   }

   TrdTrackR* ParticleR::pTrdTrack(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pTrdTrack(fTrdTrack):0;
   }

   EcalShowerR* ParticleR::pEcalShower(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pEcalShower(fEcalShower):0;
   }


   VertexR* ParticleR::pVertex(){
     return (AMSEventR::Head() )?AMSEventR::Head()->pVertex(fVertex):0;
   }

   void AMSEventR::CreateBranch(TTree *tree, int branchSplit){
   if(tree){
     _Head=this;
     tree->Branch(BranchName(),"AMSEventR",&_Head,64000,branchSplit);
     tree->SetBranchStatus("ev.TSelector",false);
//     tree->SetBranchStatus("ev.fService",false);
}
}



void AMSEventR::Begin(TTree *tree){
   // Function called before starting the event loop.
   // Initialize the tree branches.
   Init(tree);
   TString option = GetOption();

   // open file if...
   
   if(option.Length()>1){
    fService._pOut=new TFile(option,"RECREATE");
    cout <<"AMSEventR::Begin-I-WriteFileOpened "<<option<< endl;
   }

   fService._w.Start(); 
   UBegin();     
}


void AMSEventR::Init(TTree *tree){
//   Set branch addresses
   if (tree == 0) return;
   _Tree    = tree;
   //_Tree->SetMakeClass(1);
   //SetBranchA(_Tree);
   Head() = this;
   _Tree->SetBranchAddress("ev.",&Head());
   
}

void AMSEventR::ProcessFill(int entry){ 
   if (!UProcessCut()) return; 
   UProcessFill();
}
 


void AMSEventR::Terminate()
{
   // Function called at the end of the event loop.
   //_Tree->SetMakeClass(0);
   UTerminate();
   _ClonedTree=0; 
   fService._w.Stop();
   cout <<"AMSEventR::Terminate-I-CputimeSpent "<<fService._w.CpuTime()<<" sec"<<endl;
   cout <<"AMSEventR::Terminate-I-Total/Bad "<<fService.TotalEv<<"/"<<fService.BadEv<<" events processed "<<endl;
   cout <<"AMSEventR::Terminate-I-ApproxTotal of "<<fService.TotalTrig<<" triggers processed "<<endl;
   if(fService._pOut){
     fService._pOut->Write();
     fService._pOut->Close();
     fService._pOut=0;
           
//for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
//delete i->second;
//}
Service::hb1.clear();
//for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++){
//delete i->second;
//}
Service::hb2.clear();
//for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++){
//delete i->second;
//}
Service::hbp.clear();

   cout <<"AMSEventR::Terminate-I-WriteFileClosed "<<GetOption()<<endl;
   }
  
}

Int_t AMSEventR::Fill()
{
        if (_ClonedTree==NULL) _ClonedTree = _Tree->GetTree()->CloneTree(0);
        return _ClonedTree->Fill();
}

void AMSEventR::UBegin(){
}
void AMSEventR::UProcessFill(){
}
void AMSEventR::UTerminate(){
}

AMSChain::AMSChain(const char* name):TChain(name),_ENTRY(-1),_NAME(name),_EVENT(NULL),_TREENUMBER(-1){
}

void AMSChain::Init(){
      if (_EVENT==NULL) {
            _EVENT = new AMSEventR;
            this->SetBranchAddress("ev.",&_EVENT);
            _EVENT->Head() = _EVENT;
            _EVENT->Tree() = NULL;
      }
}

AMSEventR* AMSChain::GetEvent(Int_t entry){
      Init();
      _ENTRY = entry;
      Int_t tree_entry = LoadTree(_ENTRY);
      if (GetTreeNumber()!=_TREENUMBER) {
            _TREENUMBER = GetTreeNumber();
            _EVENT->Tree() = GetTree();
            _EVENT->GetBranch(_EVENT->Tree());
      }
      if (_EVENT->ReadHeader(tree_entry)==false) {
              delete _EVENT; _EVENT = NULL;
              _ENTRY = -1;
      }
      return _EVENT;
};

AMSEventR* AMSChain::GetEvent(){ 
        GetEvent(_ENTRY+1);
        return _EVENT;
};

AMSEventR* AMSChain::GetEvent(Int_t run, Int_t ev){
      Rewind();//Go to start of chain
      // Get events in turn
      while  (GetEvent() &&
         !(_EVENT->Run()==run && _EVENT->Event()==ev) ) ;
                                                                                
      return _EVENT; 
};

Int_t AMSChain::Entry() {return _ENTRY;};
AMSEventR* AMSChain::pEvent() {return _EVENT;};
const char* AMSChain::ChainName() {return _NAME;};

AMSEventList::AMSEventList(){
            _RUNs.reserve(10000);
            _EVENTs.reserve(10000);
};

AMSEventList::AMSEventList(const char* filename){
            _RUNs.reserve(10000);
            _EVENTs.reserve(10000);
            Read(filename);
};
void AMSEventList::Add(int run, int event){
        _RUNs.push_back(run);
        _EVENTs.push_back(event);
};

void AMSEventList::Add(AMSEventR* pev){
        Add(pev->Run(),pev->Event());
};

void AMSEventList::Remove(int run, int event){
        for (int j=0; j<_RUNs.size(); j++) {
            if (run==_RUNs[j] && event==_EVENTs[j]) {
                  vector<int>::iterator jiter = _RUNs.begin() + j;
                  _RUNs.erase(jiter);
                  jiter = _EVENTs.begin() + j;
                  _EVENTs.erase(jiter);
                  j--;
            }
        }
};

void AMSEventList::Remove(AMSEventR* pev){
        Remove(pev->Run(),pev->Event());
};

bool AMSEventList::Contains(int run, int event){
        for (int j=0; j<_RUNs.size(); j++) {
            if (run==_RUNs[j] && event==_EVENTs[j]) {
                  return true;
            }
        }
        return false;
};

bool AMSEventList::Contains(AMSEventR* pev){
        return Contains(pev->Run(),pev->Event());
};

void AMSEventList::Reset(){
        _RUNs.clear();
        _EVENTs.clear();
};

void AMSEventList::Read(const char* filename){
        FILE* listfile = fopen(filename,"r");
        if (listfile) {
            int run, event;
            while ( fscanf(listfile,"%d %d\n", &run, &event)==2 ) Add(run, event);
            fclose(listfile);
        } else {
            cout << "AMSEventlist: Error opening file '" << filename << "';";
            cout << " assuming an empty list" << endl;
        }
};

void AMSEventList::Write(){
        cout << "AMSEventList::Dumping a list with ";
        cout << this->GetEntries(); 
        cout << " selected events..." << endl;
        for (int j=0; j<_RUNs.size(); j++) {
            cout << _RUNs[j] << "\t" << _EVENTs[j] << endl;
        }
};

void AMSEventList::Write(const char* filename){
        cout << "AMSEventList::Writing ASCII file \"";
        cout << filename << "\" with " << this->GetEntries(); 
        cout << " selected events" << endl;
        FILE* listfile = fopen(filename,"w");
        for (int j=0; j<_RUNs.size(); j++) {
            fprintf(listfile,"%10d %10d\n",_RUNs[j],_EVENTs[j]);
        }
        fclose(listfile);
};


void AMSEventList::Write(AMSChain* chain, TFile* file){
        TTree *amsnew = chain->CloneTree(0);
        chain->Rewind();
        AMSEventR* ev = NULL;
        while ((ev=chain->GetEvent())) {
                bool found = false;
                for (int j=0; j<_RUNs.size(); j++) {
                  if (ev->Run()==_RUNs[j] && ev->Event()==_EVENTs[j]) {
                        found=true;
                        break;
                  }
                }
                if (!found) continue;
                printf("AMSEventList::Writing event ............ %12d %12d\n"
                                    , ev->Run(), ev->Event());
                ev->GetAllContents();
                amsnew->Fill();
        }
        cout << "AMSEventList::Writing AMS ROOT file \"";
        cout << file->GetName() << "\" with " << this->GetEntries(); 
        cout << " selected events" << endl;
        file->Write();
};

int AMSEventList::GetEntries(){return _RUNs.size();};
int AMSEventList::GetRun(int i){return _RUNs[i];};
int AMSEventList::GetEvent(int i){return _EVENTs[i];};


void AMSEventR::GetAllContents() {
            clear();
            bHeader->GetEntry(_Entry);
            bEcalHit->GetEntry(_Entry);
            bEcalCluster->GetEntry(_Entry);
            bEcal2DCluster->GetEntry(_Entry);
            bEcalShower->GetEntry(_Entry);
            bRichHit->GetEntry(_Entry);
            bRichRing->GetEntry(_Entry);
            bTofRawCluster->GetEntry(_Entry);
            NTofRawSide();
            bTofCluster->GetEntry(_Entry);
            bAntiCluster->GetEntry(_Entry);
            bTrRawCluster->GetEntry(_Entry);
            bTrCluster->GetEntry(_Entry);
            bTrRecHit->GetEntry(_Entry);
            bTrTrack->GetEntry(_Entry);
            bTrdRawHit->GetEntry(_Entry);
            bTrdCluster->GetEntry(_Entry);
            bTrdSegment->GetEntry(_Entry);
            bTrdTrack->GetEntry(_Entry);
            bLevel1->GetEntry(_Entry);
            bLevel3->GetEntry(_Entry);
            bBeta->GetEntry(_Entry);
            bVertex->GetEntry(_Entry);
            bCharge->GetEntry(_Entry);
            bParticle->GetEntry(_Entry);
            bAntiMCCluster->GetEntry(_Entry);
            bTrMCCluster->GetEntry(_Entry);
            bTofMCCluster->GetEntry(_Entry);
            bTrdMCCluster->GetEntry(_Entry);
            bRichMCCluster->GetEntry(_Entry);
            bMCTrack->GetEntry(_Entry);
            bMCEventg->GetEntry(_Entry);
      }


#endif
