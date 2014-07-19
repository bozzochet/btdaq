///   \example stlv.C
///
#define stlv_cxx
#include "../include/root_RVS.h"
#include "TF1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TEventList.h"
#include "TSelector.h"
#include "TChain.h"
#include <TROOT.h>
#include <TTree.h>
/** 
 * This is an example of how to work with AMS Root Files. 
 *  \sa stlv
 */



///  This is an example of user class to process AMS Root Files 
/*!
 This class is derived from the AMSEventR class. \n

 The following member functions must be called by the user: \n
    void UBegin():       called everytime a loop on the tree starts,
                         a convenient place to create your histograms. \n
    void UProcessFill(): called in the entry loop for all entries \n
    void UTerminate():   called at the end of a loop on the tree,
                         a convenient place to draw/fit your histograms. \n
 
 The following member function is optional: \n
    bool UProcessCut(): called in the entry loop to accept/reject an event \n

   To use this file, try the following session on your Tree T: \n
 Root > T->Process("stlv.C+") -> compile library if not up to date \n
 Root > T->Process("stlv.C++") -> force always compilation \n
 Root > T->Process("stlv.C")  -> non-compiled (interpreted) mode \n
*/

class ad : public AMSEventR {
   public :

   ad(TTree *tree=0){ };


/// This is necessary if you want to run without compilation as:
///     chain.Process("stlv.C");
#ifdef __CINT__
#include <process_cint.h>
#endif

   /// User Function called before starting the event loop.
   /// Book Histos
   virtual void    UBegin();

   /// User Function called to preselect events (NOT MANDATORY).
   /// Called for all entries.
   virtual bool    UProcessCut();

   /// User Function called to analyze each event.
   /// Called for all entries.
   /// Fills histograms.
   virtual void    UProcessFill();

   /// Called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   virtual void    UTerminate();
};


//create here pointers to histos and/or array of histos


       



void ad::UBegin(){
    cout << " Begin calling"<<endl;

   //here create histograms

   float al1=log(0.5);
   float al2=log(382.54);
   int nch=72;
      for(int i=0;i<40;i++){
       hbook1(100+i,"Acceptance",nch,al1,al2);
      }
    hbook1s(1,"ntof",10,-0.5,9.5);
    hbook1s(2,"nanti",10,-0.5,9.5);
    hbook1s(3,"betap",8,-0.5,7.5);
    hbook1s(4,"velocity tof",200,0.5,2.);
    hbook1s(5,"velocity agl",200,0.94,1.06);
    hbook1s(6,"velocity naf",200,0.7,1.41);
    hbook1s(7,"chi2 beta space",200,0.,10.);
    hbook1s(8,"chi2 beta",200,0.,100.);
    hbook1s(9,"chi2ms",200,0.,1000.);
    hbook1s(229,"chi2",200,0.,1000.);
    hbook1s(10,"rig",200,0.,20.);
    hbook1s(11,"rig/rigpath",200,-1.,3.);
    hbook1s(12,"rig/rigtof",200,-1.,3.);
    hbook1s(13,"rig/rigyale",200,-1.,3.);
    hbook1s(14,"rig/rig1",200,-1.,3.);
    hbook1s(15,"rig/rig2",200,-1.,3.);
    hbook1s(16,"mass",2000,-2.,12.);
    hbook1s(17,"prob",200,0.,1.);
    hbook1s(20,"probtr",200,0.,1.);
    hbook1s(18,"pattern",25,-0.5,24.5);
    hbook1s(19,"summis",200,-0.5,199.5);
    hbook1s(219,"summis1",200,-0.5,199.5);
    hbook1s(220,"summis2",200,-0.5,199.5);
    hbook1s(21,"ktof",2,-0.5,1.5);
    hbook1s(22,"numtrk",200,-1.,10.);
    hbook1s(23,"dedx tr",80,0.3,1.1);           
    hbook2s(24,"dedx tr",40,0.3,1.1,40,0.,100.);           
    hbook1s(25,"dedx tof max",200,0.,20);
    hbook1s(145,"dedx tof up",200,0.,20);
    hbook1s(146,"dedx tof down",200,0.,20);
    hbook2s(26,"dedx tof",100,0.,10.,100,0.3,1.3);
    hbook1s(226,"tof distx",100,0,10.);
    hbook1s(227,"tof disty",100,0,10.);
    hbook1s(228,"tof distx+disty",100,0,10.);
    hbook1s(27,"trd hav",200,5.,25.);
    hbook1s(28,"trd tracks",10,-0.5,9.5);
    hbook1s(29,"trd seg",20,-0.5,19.5);
    hbook1s(30,"trd mul",21,-0.5,20.5);
    hbook1s(31,"trd hmul",21,-0.5,20.5);
    hbook1s(231,"trd hmul2",21,-0.5,20.5);
    hbook1s(32,"trd dedx",200,0.,20.);
    hbook1s(33,"trd hmax",200,5.8,55.8);
    hbook1s(34,"trd stray",21,-0.5,20.5);
    hbook1s(35,"betarich",400,0.7,1.1);
    hbook1s(36,"ecalenratio",200,0.,10.);
    hbook1s(37,"momentum",400,-20,20);
    hbook1s(38,"proballtracker",200,0,1);
    hbook1s(1001,"mass",2000,-2.,12.,50,1);
    hbook1s(2000,"sliced mass k 0.1 to 1.2 ",2000,-2.,12.,20,1);
    hbook2(2100,"mass vs k ",400,-2.,4.,15,0.,1.5);
    hbook1(51,"Ekin ",100,0.,5.);
    hbook1(52,"Ekin ",100,0.,5.);
    cout << " Begin called now"<<endl;
    
   

}

// UProcessCut() is not mandatory
bool ad::UProcessCut(){
     if (nParticle()!=1 || Particle(0).iBeta()<0 || Particle(0).iCharge()<0 || Particle(0).iTrTrack()<0 || Particle(0).iTrdTrack()<0 ) return false;
     return true;
}

void ad::UProcessFill()
{
   // User Function called for all entries .
   // Entry is the entry number in the current tree.
   // Fills histograms.

     static int written=0;
     const int maxw=200;
     bool cuts[23];
     if(Particle(0).Charge==1 && fabs(Particle(0).Momentum)<20 && fabs(Particle(0).Momentum)>0 && Particle(0).Beta>0){




//
//  Redefinition of vars
// 
// tracker

    TrTrackR track=TrTrack(Particle(0).iTrTrack());
    float rig=track.Rigidity;
    float pirig=track.PiRigidity;
    float rig1=track.HRigidity[0];
    float rig2=track.HRigidity[1];
    float rigy=track.GRigidity;
    int ktofonly=(track.Status/16384)%2;
    float mass=Particle(0).Mass;
//
//  find energy around the track
//
     float summis=0;
     float summis1=0;
     float summis2=0;
     for(int i=0;i<8;i++){
        float distm=10000;
        float distm1=10000;
        int kcand=-1;
        int kcand1=-1;
         for(int j=0;j<nTrRecHit();j++){
           TrRecHitR hit=TrRecHit(j);
           if(hit.Layer==i+1){
            float dist=0;
            float distx=Particle(0).TrCoo[i][0]-hit.Hit[0];
            float disty=Particle(0).TrCoo[i][1]-hit.Hit[1];
            for(int k=0;k<2;k++)dist+=(Particle(0).TrCoo[i][k]-hit.Hit[k])*(Particle(0).TrCoo[i][k]-hit.Hit[k]);
             dist=sqrt(dist);
            if(distm>dist){
              distm=dist;
               kcand=j;
            }
            else if(distm1>dist){
              distm1=dist;
              kcand1=j;
            }
            if(fabs(distx)<5 && fabs(disty)<5 && (hit.Status/32)%2==0){
             for(int k=0;k<track.NTrRecHit();k++){
              if(TrRecHit(track.iTrRecHit(k)).Layer==i+1){
               if(TrRecHit(track.iTrRecHit(k)).iTrCluster('y') !=hit.iTrCluster('y')){
                summis1+=hit.Sum;
                summis2++;
                break;
               }
              }
             }     
           }
         }
      }
      if(kcand>=0){
             for(int k=0;k<track.NTrRecHit();k++){
               if(TrRecHit(track.iTrRecHit(k)).Layer==i+1){
                goto nah;
               }
             }
             summis+=TrRecHit(kcand).Sum;
nah:   
             continue;
      }
   }
//
//  Find rigtof
//

    float rtof=0;
    if(Version()>156){
    if(ktofonly==0 and nBeta()>1){
     for(int i=0;i<nBeta();i++){
      if(Particle(0).iBeta() != i && Beta(i).iTrTrack()>=0){
       rtof=TrTrack(Beta(i).iTrTrack()).Rigidity;
//       cout << " rtof "<<rtof<<endl;
      }
      }
    } 
    }
    else if(ktofonly==0){
//   bug in prev versions, work around

      bool ok;
      for (int i=0;i<NTrTrack();i++){
       TrTrackR cand=TrTrack(i);
       ok=true;
       if((cand.Status/16384)%2==1){
        for(int l=0;l<track.NTrRecHit();l++){
         bool found=false;
         for(int k=0;k<cand.NTrRecHit();k++){
          TrRecHitR hit=TrRecHit(cand.iTrRecHit(k));
           if(TrRecHit(track.iTrRecHit(l)).iTrCluster('y') == hit.iTrCluster('y')){
             found=true;
             break;
           }
          }
          if(not found){
            ok=false;
            break;
          }
        }
        if(ok){
         rtof=cand.Rigidity;
         //cout << " rtof "<<rtof<<endl;
         break;
        }
       }
      }
    }
   if(rtof==0)rtof=rig; 
   float rtofotr=rtof/rig;
   BetaR beta=Beta(Particle(0).iBeta());
//
// Find tof
//
    float tofav=0;
    float ntofav=0;
    float tofmax=0;
    float tofup=0;
    float tofdown=0;
    float distx=0;
    float disty=0;
    for(int i=0;i<beta.NTofCluster();i++){
     TofClusterR tofc=TofCluster(beta.iTofCluster(i));
     ntofav++;
    if(tofc.Layer==2 or tofc.Layer==3){
     if(disty<fabs(Particle(0).TOFCoo[tofc.Layer-1][1]-tofc.Coo[1])){
        disty=fabs(Particle(0).TOFCoo[tofc.Layer-1][1]-tofc.Coo[1]);
    }
    }
    else{
     if(distx<fabs(Particle(0).TOFCoo[tofc.Layer-1][0]-tofc.Coo[0])){
        distx=fabs(Particle(0).TOFCoo[tofc.Layer-1][0]-tofc.Coo[0]);
    }
    }
     if(tofc.Layer<3){
      tofup+=tofc.Edep;
     }
     else tofdown+=tofc.Edep; 
     tofav+=tofc.Edep;
     if(tofmax<tofc.Edep)tofmax=tofc.Edep;
    }
    if(ntofav>0)tofav/=ntofav;

   float rtofopi=pirig/rig;
   float r1=rig/rig1;     
   float r2=rig/rig2;     


//  TRD vars
    

    TrdTrackR trdt=TrdTrack(Particle(0).iTrdTrack());
     float htrdmul=0;
     float trdmul=0;
     float trdav=0;
     float ntrdav=0;
     float xtrdhm=0;
     float xtrdhmax=0;
     float xtrdh2=0;
     double xbeta=fabs(Particle(0).Beta);
     double a166=1.66;
     float xf=pow(1./(xbeta+0.03),a166);
     if(xf<1)xf=1;
     float xthr=5.7+0.3*log(fabs(Particle(0).Momentum));
     xthr*=xf;
     for(int i=0;i<trdt.NTrdSegment();i++){
      for(int j=0;j<TrdSegment(trdt.iTrdSegment(i)).NTrdCluster();j++){
       TrdClusterR trdc=TrdCluster(TrdSegment(trdt.iTrdSegment(i)).iTrdCluster(j));
       if(trdc.EDep>xtrdhmax)xtrdhmax=trdc.EDep;
       if(trdc.EDep>xthr)xtrdh2++;
       if(trdc.Multip>1)xtrdhm++;
       xtrdhm+=trdc.HMultip;
       htrdmul+=trdc.HMultip;
       trdmul+=trdc.Multip;
        trdav+=trdc.EDep;
        ntrdav++;
      }
     }
     if(ntrdav>0)trdav/=ntrdav;

// Rich vars
     float betarich=0;
     if(Particle(0).iRichRing()>=0){
        RichRingR rich=RichRing(Particle(0).iRichRing());
        betarich=rich.Beta;
      }
     else{
        for(int k=0;k<nRichRing();k++){
          RichRingR rich=RichRing(k);
          if(rich.Beta>betarich){
            betarich=rich.Beta;
          }
        }
     }
//Ecal vars

      float ecalen=0;
     if(Particle(0).iEcalShower()>=0){
        EcalShowerR ecal=EcalShower(Particle(0).iEcalShower());
        ecalen=ecal.EnergyC;
      }
    

//  Tof Cuts
    cuts[0]=nTofCluster()<5;
    cuts[1]=nAntiCluster()<1 || (ecalen>0.5 && nAntiCluster()<2);
    cuts[2]=beta.Pattern==0;
    cuts[3]=fabs(fabs(beta.Beta)-1)>0.166 && fabs(rig)<8;
//    cuts[3]=true;
    cuts[4]=beta.Chi2S<8;
    cuts[5]=beta.Chi2<12;
    cuts[6]=track.FChi2MS*beta.Beta*beta.Beta<10000;
    cuts[6]=track.Chi2FastFit<500;
    cuts[7]=rtofotr>0.6 && rtofotr<1.5;
    cuts[7]=cuts[7] && rtofopi>0.6 && rtofopi<1.5;
    cuts[8]=r1>0.4 && r1<2.5;
    cuts[9]=r2>0.4 && r2<2.5;
    cuts[10]=ktofonly ==0;
    cuts[11]=nTrdTrack()==1;
    cuts[12]=nTrdSegment()<6;
    cuts[13]=tofmax*pow(xbeta,a166)<6 and tofmax*pow(xbeta,a166)>1;
    cuts[13]=cuts[13] && tofup*pow(xbeta,a166)<6.;
    cuts[21]=tofdown*pow(xbeta,a166)<6.;
    cuts[14]=summis<60;
    cuts[15]=summis1<400;
    cuts[16]=xtrdh2<4;
    cuts[17]=fabs(betarich-1)>0.1;
    cuts[18]=ecalen/fabs(Particle(0).Momentum)<1;
    cuts[19]=distx<3.9;
    cuts[20]=Particle(0).Momentum>0;
//    cuts[21]=Charge(Particle(0).iCharge()).ProbAllTracker>0.05;
//    cuts[21]=true;
    cuts[22]=fabs(mass)>1.71 && fabs(mass)<2.2;
    //for(int k=21;k<sizeof(cuts)/sizeof(cuts[0]);k++)cuts[k]=true;
    int nc=sizeof(cuts)/sizeof(cuts[0]);
    hf1s(1,nTofCluster(),cuts,nc,1);
    hf1s(2,nAntiCluster(),cuts,nc,2);
    hf1s(3,beta.Pattern,cuts,nc,3);
    hf1s(4,fabs(beta.Beta),cuts,nc,4);
    hf1s(10,fabs(rig),cuts,nc,4);
    hf1s(7,beta.Chi2S,cuts,nc,5);
    hf1s(8,beta.Chi2,cuts,nc,6);
    hf1s(9,track.FChi2MS*beta.Beta*beta.Beta,cuts,nc,7);
    hf1s(229,track.Chi2FastFit,cuts,nc,7);
    hf1s(11,track.Rigidity/track.PiRigidity,cuts,nc,8);
    hf1s(12,rtofotr,cuts,nc,8);
    hf1s(13,rig/rigy,cuts,nc,8);
    hf1s(14,rig/rig1,cuts,nc,9);
    hf1s(15,rig/rig2,cuts,nc,10);
    hf1s(21,ktofonly,cuts,nc,11);
    hf1s(28,nTrdTrack(),cuts,nc,12);
    hf1s(29,nTrdSegment(),cuts,nc,13);
    hf1s(30,trdmul,cuts,nc,17);
    hf1s(31,htrdmul,cuts,nc,17);
    hf1s(231,xtrdh2,cuts,nc,17);
    hf1s(32,trdav,cuts,nc,17);
    hf1s(33,xtrdhmax,cuts,nc,17);
    hf1s(34,xtrdhm,cuts,nc,17);
    hf1s(35,betarich,cuts,nc,18);
    hf1s(36,ecalen/fabs(Particle(0).Momentum),cuts,nc,19);
    hf1s(25,tofmax*pow(xbeta,a166),cuts,nc,14);
    hf1s(145,tofup*pow(xbeta,a166),cuts,nc,14);
    hf1s(146,tofdown*pow(xbeta,a166),cuts,nc,22);
    hf1s(19,summis,cuts,nc,15);
    hf1s(219,summis1,cuts,nc,16);
    hf1s(220,summis2,cuts,nc,16);
    hf2(26,tofav*pow(xbeta,a166),fabs(beta.Beta),1);
    hf1s(226,distx,cuts,nc,20);
    hf1s(227,disty,cuts,nc,20);
    hf1s(228,distx+disty,cuts,nc,20);
    hf1s(37,Particle(0).Momentum,cuts,nc,21);
//    hf1s(38,Charge(Particle(0).iCharge()).ProbAllTracker,cuts,nc,22);
    hf1s(16,mass,cuts,nc,23);
    float xm=0;
   float ekin=0;
    if(nMCEventg()>0){		
     MCEventgR mc_ev=MCEventg(0);
      xm = log(mc_ev.Momentum);
      double betagen=mc_ev.Momentum/sqrt(mc_ev.Momentum*mc_ev.Momentum+mc_ev.Mass*mc_ev.Mass);
      ekin=1/sqrt(1-betagen*betagen)-1;
      
    }
    bool cut=true;
    for (int k=0;k<sizeof(cuts)/sizeof(cuts[0]);k++){
     if(cut){
      hf1(100+k+1,xm);
     }
      cut=cut && cuts[k];       
    }
 
    if(cut){
      if(xbeta>0){
       float ebeta=1/xbeta;
       if(ebeta<1)ebeta=2-ebeta;
       ebeta=1/ebeta;
      float ke=1/sqrt(1-ebeta*ebeta)-1;
      int kbin=(ke+0.05)/0.1;
      hf1(2000+kbin,mass);
      hf2(2100,mass,ke,1);
      }    
          
        hf1(100+sizeof(cuts)/sizeof(cuts[0])+1,xm);
        if(fabs(mass)>1.65 && written++<maxw){
            cout<<Run()<<" "<<Event()<<" "<<mass<<" "<<beta.Chi2S<<" "<<tofmax<<" "<<xbeta<<" "<<Particle(0).Beta<<" "<<Particle(0).iRichRing()<<" "<<nTofRawSide()<<" "<<nRichRing()<<" "<<distx<<" "<<disty<<endl;
              GetAllContents();
              Fill();
        }
    }
   for (int k=0;k<sizeof(cuts)/sizeof(cuts[0])+1;k++){
      cut=true;
      for(int l=0;l<sizeof(cuts)/sizeof(cuts[0]);l++){
       if(k !=l)cut=cut && cuts[l];
      }
      if(cut)hf1(1000+k+1,mass);
   }
   bool xcut=true;
    for (int k=0;k<sizeof(cuts)/sizeof(cuts[0])-1;k++){
      if(k!=3)xcut=xcut && cuts[k];       
    }
    if(xcut)hf1(51,ekin,1.);
    if(xcut && cuts[sizeof(cuts)/sizeof(cuts[0])-1])hf1(52,ekin,1.);
}
}


void ad::UTerminate()
{
  
}

