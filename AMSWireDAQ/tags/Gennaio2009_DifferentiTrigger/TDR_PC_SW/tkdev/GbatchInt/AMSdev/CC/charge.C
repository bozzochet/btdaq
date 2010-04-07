//  $Id: charge.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 5-june-1996
//
//
// Lat Edit : Mar 20, 1997. ak. AMSCharge::Build() check if psen == NULL
//
// Modified by E.Choumilov 17.01.2005 for AMS02 
//
#include "beta.h"
#include "commons.h"
#include <math.h>
#include <limits.h>
#include "amsgobj.h"
#include "extC.h"
#include "upool.h"
#include "charge.h"
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include "amsstl.h"
#include "ntuple.h"
#include "cern.h"
#include "trrawcluster.h"
#include "job.h"
#include "trkelospdf.h"
using namespace std;
using namespace trconst;
using namespace AMSChargConst;

integer AMSCharge::_chargeTracker[MaxZTypes]={1,1,2,3,4,5,6,7,8,9};
integer AMSCharge::_chargeTOF[MaxZTypes]={1,1,2,3,4,5,6,7,8,9};
integer AMSCharge::_chargeRich[MaxZTypes]={1,1,2,3,4,5,6,7,8,9};

PROTOCCALLSFFUN2(FLOAT,PROB,prob,FLOAT,INT)
#define PROB(A2,A3)  CCALLSFFUN2(PROB,prob,FLOAT,INT,A2,A3)

number AMSCharge::getprobcharge(integer charge){
charge=abs(charge);
if(charge>_chargeTracker[MaxZTypes-1]){
  cerr <<" AMSCharge::getprobcharge-E-charge too big "<<charge<<endl;
  return 0;
}
 int index;
 int voted=getvotedcharge(index);
 int i=charge;
 if(_ChargeTracker){
   if(voted<=CHARGEFITFFKEY.TrackerOnly && voted<=CHARGEFITFFKEY.TrackerProbOnly) {
     return _ProbTOF[i]*_ProbTracker[i]/_ProbTOF[index]/_ProbTracker[index];
   }
   else return _ProbTracker[i]/_ProbTracker[_iTracker];
 }
 else return _ProbTOF[i]/_ProbTOF[_iTOF];
}

integer AMSCharge::getvotedcharge(int & index){
  int i;
  int charge=0;
// Only tracker above this value
  if (_ChargeTOF>CHARGEFITFFKEY.TrackerOnly && _ChargeTracker){
    index=_iTracker;
    return _ChargeTracker;
  }
// Exclude bad measurement if possible
  number probmin=CHARGEFITFFKEY.ProbMin;
  int usetof=_ProbTOF[_iTOF]>probmin?1:0;
  int usetrk=_ProbTracker[_iTracker]>probmin?1:0;
  int userich= _ProbRich[_iRich]>probmin?1:0;
  if(!_ChargeTracker) usetof=1;
  else if(!usetof && !usetrk){
    usetof=1;
    usetrk=1;
  }
//--- codes below are suspicous to me (TOF and TRK Lkhd's have differente normalization !)
//  number minlkhd=FLT_MAX;
//  for(i=0; i<MaxZTypes; i++){
//    number lkhdall=(usetof?_LkhdTOF[i]:0)+(usetrk?_LkhdTracker[i]:0);
//    if(lkhdall<minlkhd){
//      minlkhd=lkhdall;
//      charge=_chargeTracker[i];
//      index=i;
//    }
//  }
//--- so i propose below the replacement codes(each of TOF and TRK prob's are normalized to SUMprob(i)=1)   
  number maxprob=0;
  for(i=0; i<MaxZTypes; i++){
    number proball=(usetof?_ProbTOF[i]:1)*(usetrk?_ProbTracker[i]:1);
//    cout<<"IZ="<<i<<" prTOF="<<_ProbTOF[i]<<" prTRK="<<_ProbTracker[i]<<" prRIC="<<_ProbRich[i]<<endl;
    if(proball>maxprob){
      maxprob=proball;
      charge=_chargeTracker[i];
      index=i;
    }
  }
  return charge;
}


integer AMSCharge::build(integer refit){
  number etof[TOF2GC::SCLRS],etrk[TrackerMaxHits];
  number EdepTOF[TOF2GC::SCLRS],EdepTracker[TrkTypes-1][TrackerMaxHits];
  AMSTOFCluster *pTOFc[TOF2GC::SCLRS];
  AMSTrCluster  *pTrackerc[TrkTypes-1][TrackerMaxHits];
  integer TypeTOF[TOF2GC::SCLRS];
  integer TypeTracker[TrackerMaxHits];
  const number fac=AMSTrRawCluster::ADC2KeV();

// Temporary fix for simulation
  if (!AMSJob::gethead()->isRealData()){
    CHARGEFITFFKEY.ResCut[0]=-1.;     // no incompatible TOF clus exclusion
    CHARGEFITFFKEY.ResCut[1]=-1.;     // no incompatible Tracker clus exclusion
    CHARGEFITFFKEY.TrMeanRes=0;       // use normal(0)/"-incomp.hit"(1)truncated mean
    CHARGEFITFFKEY.ChrgMaxAnode=9;   // spare 
    CHARGEFITFFKEY.BetaPowAnode=0;    // no corr. on anode beta dependence for z>1
    CHARGEFITFFKEY.TrackerForceSK=1;  // force tracker hit energies to be x+y
    CHARGEFITFFKEY.TrackerKSRatio=1.; // average x/y tracker energy ratio
    static integer first=1;
    if(first){
      first=0;
      cout<<"AMSCharge::build - MC forced with following datacards:"<<endl;
      cout<<" ResCut[0]: "<<CHARGEFITFFKEY.ResCut[0]<<endl
          <<" ResCut[1]: "<<CHARGEFITFFKEY.ResCut[1]<<endl
          <<" TrMeanRes: "<<CHARGEFITFFKEY.TrMeanRes<<endl
          <<" ChrgMaxAnode: "<<CHARGEFITFFKEY.ChrgMaxAnode<<endl
          <<" BetaPowAnode: "<<CHARGEFITFFKEY.BetaPowAnode<<endl
          <<" TrackerForceSK: "<<CHARGEFITFFKEY.TrackerForceSK<<endl
          <<" TrackerKSRatio: "<<CHARGEFITFFKEY.TrackerKSRatio<<endl;
    }
  }

  int patb;
  for(patb=0; patb<npatb; patb++){//<--- beta-patterns loop
    AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
    while(pbeta){//<--- betas-loop(with given pattern)

// init
      integer nhitTOF=0, nhitTracker=0;
      integer nallTOF=0, nallTracker=0;
      number expRich=0, useRich=0;
      AMSTrTrack *ptrack=pbeta->getptrack();
      number rid=ptrack->getrid();
      number beta=pbeta->getbeta();
      number theta, phi, sleng;
      AMSPoint P1;
      int i,j,weak;
      number pathcor;

//====> Select TOF hits:

      weak=0;
      while(1){
        nhitTOF=0;
        nallTOF=0;
        for(i=0; i<TOF2GC::SCLRS; i++){
	  EdepTOF[i]=0;
          AMSTOFCluster *pcluster=pbeta->getpcluster(i);
          if(pcluster){
            number edep=pcluster->getedep();
            if(edep>0) etof[nallTOF++]=pcluster->getedep();//store/counts all raw hits
            if(pcluster->getnmemb()<=CHARGEFITFFKEY.NmembMax || weak){
              EdepTOF[nhitTOF]=0;
              AMSDir ScDir(0,0,1); // good approximation for TOF
              AMSPoint SCPnt=pcluster->getcoo();
              ptrack->interpolate(SCPnt, ScDir, P1, theta, phi, sleng);
              AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
              pathcor=fabs(ScDir.prod(DTr));
              pTOFc[nhitTOF]=pcluster;
              EdepTOF[nhitTOF]=edep*pathcor;//store selected and pass-corrected hits
              nhitTOF++;//count selected hits
            }
          }
        }//---> endof hits loop
        if(nhitTOF<2){//too little, try to add "nmemb>NmembMax"(weak) hits
          if(!weak && nallTOF>0 && CHARGEFITFFKEY.NmembMax<2)weak=1;
//         (try to use clust with nmemb>NmembMax(if<2), by TOF-clust definition nmemb<=2 !!!)
          else{
            cerr<<"AMSCharge::build:TOF -E- Low clust.mult. and No weak clusters to add"<<endl;
            break;
          }
        }
	else break;
      }//-->endof while
//cout<<"TOFh-pcor:N="<<nhitTOF<<"ed="<<EdepTOF[0]<<" "<<EdepTOF[1]<<" "<<EdepTOF[2]<<" "<<EdepTOF[3]<<endl;
//cout<<"TOFh-orig:N="<<nallTOF<<"ed="<<etof[0]<<" "<<etof[1]<<" "<<etof[2]<<" "<<etof[3]<<endl;

//====> Select Tracker hits:

      weak=0;
      while(1 ){
        nhitTracker=0;
        nallTracker=0;
        for(i=0; i<ptrack->getnhits(); i++){
          AMSTrRecHit *phit=ptrack->getphit(i);
          if(phit){
            if (phit->getpsen()){
              for(j=0; j<TrkTypes-1; j++) EdepTracker[j][nhitTracker]=0;
              AMSDir SenDir((phit->getpsen())->getnrmA(2,0),
               (phit->getpsen())->getnrmA(2,1),(phit->getpsen())->getnrmA(2,2));
              AMSPoint SenPnt=phit->getHit();
              ptrack->interpolate(SenPnt, SenDir, P1, theta, phi, sleng);
              AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
              pathcor=fabs(SenDir.prod(DTr));
              AMSTrCluster *pcls;
              int etaok[2];
              for(j=0; j<2; j++){
                etaok[j]=0;
                pcls=phit->getClusterP(j);
                if(pcls){
                  number eta=max(pcls->geteta(),0.);
                  number etamin=CHARGEFITFFKEY.EtaMin[j];
                  number etamax=CHARGEFITFFKEY.EtaMax[j];
                  if( ((eta>etamin||etamin<=0)&&(eta<etamax||etamax>=1)) || weak)
                   etaok[j]=1;
                  pTrackerc[j][nhitTracker]=pcls;
                }
              }
              number sums=phit->getsonly()/2;
              number sumk=phit->getkonly()/2;
              if (sums>0) etrk[nallTracker++]=2*sums;
              if (etaok[0]) EdepTracker[0][nhitTracker]=sumk>0?fac*sumk*pathcor:0;
              if (etaok[1]) EdepTracker[1][nhitTracker]=sums>0?fac*sums*pathcor:0;
              nhitTracker++;
            }
            else{
             cout<<"AMSCharge::build -E- phit -> getpsen == NULL "<<" for hit wit pos "<<phit ->getpos();
            }
          }
        }
        if(nhitTracker<2){
          if(!weak && nallTracker) weak=1;
          else{
//            cerr<<"AMSCharge::build -E- nallTracker = 0"<<endl;
            break;
          }
        }else break;
      }

//====> Select RICH Ring:

      AMSRichRing *pring=NULL;
      AMSRichRing *prcri=(AMSRichRing *)AMSEvent::gethead()->getheadC("AMSRichRing",0);
      while(prcri){
       AMSTrTrack *prctk=prcri->gettrack();
       if(prctk==ptrack){
         pring=prcri;
         expRich=pring->getnpexp();
         useRich=pring->getcollnpe();
       }
       prcri=prcri->next(); 
      }
      

//====> Compute TOF/TRK truncated means:

      int imx;
      number TrMeanTOF, TrMeanTracker;
      number resmx,mean,trunres,trunmax,rescut;
      rescut=CHARGEFITFFKEY.ResCut[0];//use/not(>=0/-1) incomp.clus exclusion
      resmx=resmax(etof,nallTOF,0,rescut,imx,mean,trunres,trunmax);//TOF(raw(non-corr!) hits trunc.mean)
      if(!CHARGEFITFFKEY.TrMeanRes) TrMeanTOF=trunmax;//normal("-highest hit") TruncMean
      else TrMeanTOF=trunres;//"-incomp.cluster" TruncMean
      rescut=CHARGEFITFFKEY.ResCut[1];
      resmx=resmax(etrk,nallTracker,0,rescut,imx,mean,trunres,trunmax);//Tracker
      if(!CHARGEFITFFKEY.TrMeanRes) TrMeanTracker=trunmax;
      else TrMeanTracker=trunres;

// Add new entry
      addnext(pbeta,pring,nhitTOF,nhitTracker,pTOFc,EdepTOF,pTrackerc,EdepTracker,TrMeanTracker,TrMeanTOF,expRich,useRich);
      pbeta=pbeta->next();//next beta from current pattern-group
    }//--->endof loop over all betas in current pattern-group    
  }//--->endof beta-patterns loop
  return 1;
}


void AMSCharge::addnext(AMSBeta *pbeta, AMSRichRing *pring, integer nhitTOF, integer nhitTracker,
                                           AMSTOFCluster *pTOFc[], number EdepTOF[TOF2GC::SCLRS], 
			                     AMSTrCluster *pTrackerc[TrkTypes-1][TrackerMaxHits], 
			             number EdepTracker[TrkTypes-1][TrackerMaxHits], number trtr, 
			                           number trtof, number expRich, number useRich){
   number beta=pbeta->getbeta();
   int bstatus=!pbeta->checkstatus(AMSDBc::AMBIG);
   AMSCharge *pcharge=new AMSCharge(pbeta, pring, trtr, trtof);
  int tofok=pcharge->FitTOF(0,beta,bstatus,nhitTOF,pTOFc,EdepTOF);
  int trkok=pcharge->FitTracker(0,beta,bstatus,nhitTracker,pTrackerc,EdepTracker);
  int ricok=pcharge->FitRich(0,expRich,useRich);
  number probmin=CHARGEFITFFKEY.ProbMin;
  int tofgood=pcharge->_ProbTOF[pcharge->_iTOF]>probmin?1:0;
  int trkgood=pcharge->_ProbTracker[pcharge->_iTracker]>probmin?1:0;
  if((!tofok&&!trkok) || (!tofgood&&!trkgood)) pcharge->setstatus(AMSDBc::BAD);
  AMSEvent::gethead()->addnext(AMSID("AMSCharge",0),pcharge);
}

integer AMSCharge::FitTOF(int refit, number beta, int bstatus, int nhitTOF, AMSTOFCluster *pTOFc[], number etof[TOF2GC::SCLRS]){
  static number ETOF[TOF2GC::SCLRS];
  int typetof[TOF2GC::SCLRS], nhittoftyp;
  int nhtt[2]={0,0};
  number TOFresmax, etofh[TOF2GC::SCLRS], x[TOF2GC::SCLRS];
  int TOFhitmax;
  int i,j;
// init
  if (!refit){
    _iTOF=0;
    _ChargeTOF=0;
    for(i=0; i<MaxZTypes; i++) _ProbTOF[i]=0;
    for(i=0; i<MaxZTypes; i++) _IndxTOF[i]=i;
    UCOPY(etof,ETOF,TOF2GC::SCLRS*sizeof(etof[0])/4);//save initial edep-array
  }

// find/remove furthest(incomp) hits(if requested)
  if(!refit){
    number rescut=CHARGEFITFFKEY.ResCut[0];//>=0/-1->use/not incomp.clus exclusion
    number mean,trunres,trunmax;
    int hitmax;
    for(j=0; j<nhitTOF; j++) x[j]=etof[j];
    TOFresmax=resmax(x,nhitTOF,refit,rescut,hitmax,mean,trunres,trunmax);
    TOFhitmax=hitmax;//incomp.clus.index/-1(if not requested)
    for(j=0; j<nhitTOF; j++) if(j==TOFhitmax) etof[j]=0;//delete incomp.hit(if requested)
  }

// Mark good(used)/deleted hits:
  int failtof=0;
  int nhittof=0;
  nhittoftyp=0;
  for(i=0; i<nhitTOF; i++){
    typetof[i]=-1;
    etofh[i]=0;
    if(refit<=0 && etof[i]>0){
      typetof[i]=0;//mark good(not_incomp) hits with "0"
      etofh[i]=etof[i];//store hits for lkhc-calc(incomp.hits with etof=0 will be mark typetof=-1) 
    }
    if(typetof[i]>=0){
      nhittoftyp++;//counts all good hits
      if(refit>=0 && bstatus) pTOFc[i]->setstatus(AMSDBc::CHARGEUSED);
      nhittof++;
    }
  }
  if(!nhittof) failtof=1;//no good hits left
  if(!failtof){
// likelihood values and charge probabilities
    number lkhtof[MaxZTypes];
    lkhcalc(0,beta,nhitTOF,etofh,typetof,lkhtof);//"0" means TOF
    nhtt[0]=nhittoftyp;//just to be compat. with TRK, where nhtt is array
    number probtof=_probcalc(0,refit,nhtt,lkhtof);
    _ChargeTOF=_chargeTOF[_iTOF];
  }
  else{
    cerr<<"AMSCharge::TofFit -E- no TOF cluster found"<<endl;
  }

  return !failtof;
}

 
integer AMSCharge::FitTracker(int trkfit, number beta, int bstatus, int nhitTracker, 
                                AMSTrCluster *pTrackerc[TrkTypes-1][TrackerMaxHits], 
                                           number etrk[TrkTypes-1][TrackerMaxHits]){
  static number ETRK[TrkTypes-1][TrackerMaxHits];
  int typetrk[TrackerMaxHits], nhittrktyp[TrkTypes];
  number Trackerresmax[TrkTypes-1], etrkh[TrackerMaxHits], x[TrackerMaxHits];
  int Trackerhitmax[TrkTypes-1];
  int i,j;

// init
  if(!trkfit){
    _iTracker=0;
    _ChargeTracker=0;
    _ProbAllTracker=0;
    for(i=0; i<MaxZTypes; i++) _ProbTracker[i]=0;
    for(i=0; i<MaxZTypes; i++) _IndxTracker[i]=i;
    UCOPY(etrk[0],ETRK[0],(TrkTypes-1)*TrackerMaxHits*sizeof(etrk[0][0])/4);
  }

// determine furthest hits
  if(trkfit>=0){
    number rescut=!trkfit?CHARGEFITFFKEY.ResCut[1]:0;
    for(i=0; i<TrkTypes-1; i++){
      number mean,trunres,trunmax;
      int hitmax;
      for(j=0; j<nhitTracker; j++) x[j]=etrk[i][j];
      Trackerresmax[i]=resmax(x,nhitTracker,trkfit,rescut,hitmax,mean,trunres,trunmax);
      Trackerhitmax[i]=hitmax;
    }
    for(i=0; i<TrkTypes-1; i++){
      for(j=0; j<nhitTracker; j++) if(j==Trackerhitmax[i]) etrk[i][j]=0;
    }
  }

// Use either S+K, S or K energies
  int failtrk=0;
  int nhittrk=0;
  for(i=0; i<TrkTypes; i++) nhittrktyp[i]=0;
  for(i=0; i<nhitTracker; i++){
    typetrk[i]=-1;
    if (CHARGEFITFFKEY.TrackerForceSK){
      if(etrk[1][i]>0){
        typetrk[i]=2;
        etrkh[i]=etrk[0][i]>0?etrk[1][i]+etrk[0][i]:etrk[1][i]*(1.+CHARGEFITFFKEY.TrackerKSRatio);
        if(trkfit>=0 && bstatus){
          pTrackerc[1][i]->setstatus(AMSDBc::CHARGEUSED);
          if(etrk[0][i]>0) pTrackerc[0][i]->setstatus(AMSDBc::CHARGEUSED);
        }
      }
    }
    else{
      if (CHARGEFITFFKEY.Tracker>0 && etrk[1][i]>0 && etrk[0][i]>0){
        typetrk[i]=2;
        etrkh[i]=etrk[1][i]+etrk[0][i];
        if(trkfit>=0 && bstatus) pTrackerc[1][i]->setstatus(AMSDBc::CHARGEUSED);
        if(trkfit>=0 && bstatus) pTrackerc[0][i]->setstatus(AMSDBc::CHARGEUSED);
      }
      else if(etrk[1][i]>0){
        typetrk[i]=0;
        etrkh[i]=etrk[1][i];
        if(trkfit>=0 && bstatus) pTrackerc[1][i]->setstatus(AMSDBc::CHARGEUSED);
      }
      else if(CHARGEFITFFKEY.Tracker>1 && etrk[0][i]>0){
        typetrk[i]=1;
        etrkh[i]=etrk[0][i];
        if(trkfit>=0 && bstatus) pTrackerc[0][i]->setstatus(AMSDBc::CHARGEUSED);
      }
    }
    if(typetrk[i]>=0){
      nhittrktyp[typetrk[i]]++;
      nhittrk++;
    }
  }
  if(!nhittrk) failtrk=1;

  if(!failtrk){

// likelihood values and charge probabilities
    number lkhtrk[MaxZTypes];
    lkhcalc(1,beta,nhitTracker,etrkh,typetrk,lkhtrk);
    number probtrk=_probcalc(1,trkfit,nhittrktyp,lkhtrk);

// refit tracker if required
    _ChargeTracker=_chargeTracker[_iTracker];
    if(!trkfit && probtrk<CHARGEFITFFKEY.ProbTrkRefit){
      for(i=0; i<nhitTracker; i++){
        if(etrk[1][i]>0 && (typetrk[i]==0 || typetrk[i]==2) && bstatus)
         pTrackerc[1][i]->clearstatus(AMSDBc::CHARGEUSED);
        if(etrk[0][i]>0 && (typetrk[i]==1 || typetrk[i]==2) && bstatus)
         pTrackerc[0][i]->clearstatus(AMSDBc::CHARGEUSED);
      }
      trkfit++;
      failtrk=!FitTracker(trkfit,beta,bstatus,nhitTracker,pTrackerc,etrk);
    }
    else if(trkfit>0) setstatus(AMSDBc::REFITTED);
    else _ProbAllTracker=probtrk;

// get tracker probability using all the hits
    if(trkfit!=-1) failtrk=!FitTracker(-1,beta,bstatus,nhitTracker,pTrackerc,ETRK);
  }
  else{
    //cerr<<"AMSCharge::Fit -E- no Tracker hit found"<<endl;
  }

  return !failtrk;
}

integer AMSCharge::FitRich(int ricfit, number expRich, number useRich){
  number lkhrich[MaxZTypes],probrich;
  int i;
  int failrich=0;

// init
  if(!ricfit){
    _iRich=0;
    _ChargeRich=0;
    for(i=0; i<MaxZTypes; i++) _ProbRich[i]=0;
    for(i=0; i<MaxZTypes; i++) _IndxRich[i]=i;
  }

  if(expRich>0){
    for (i=0; i<MaxZTypes; i++){
     number zz=_chargeRich[i]*_chargeRich[i];
     lkhrich[i]=expRich*zz-useRich*log(expRich*zz);
    }
    probrich=_probrich(expRich,useRich,lkhrich);
    _ChargeRich=_chargeRich[_iRich];
  }
  else failrich=1;

  return !failrich;
}

void AMSCharge::lkhcalc(int mode, number beta, int nhit, number ehit[], int typeh[], number lkh[]){
  for(int i=0; i<MaxZTypes; i++){
    lkh[i]=0;
    if(mode==0)lkh[i]+=TofElosPDF::TofEPDFs[i].getlkhd(nhit,typeh,ehit,beta);//TOF
    else if(mode==1)lkh[i]+=TrkElosPDF::TrkEPDFs[i].getlkhd(nhit,typeh,ehit,beta);//TRK
  }
}


number AMSCharge::resmax(number x[],int ntot,int refit,number rescut,int &imax,number &mean,number &trres,number &trmax){
  int i,j,n;
  imax=-1;
  mean=ntot?x[0]:0;
  trres=ntot?x[0]:0;
  trmax=ntot?x[0]:0;

  if(ntot<2) return 0;

  number sigmin=CHARGEFITFFKEY.SigMin;
  number sigmax=refit?1:CHARGEFITFFKEY.SigMax;
  assert(sigmin>0 && sigmax>0 && sigmax>sigmin);

  number rsmx=0;
  for(i=0; i<ntot; i++){
    if (x[i]>0){
      number xm=0, xxm=0;
      n=0;
      for(j=0; j<ntot; j++){
        if(j!=i && x[j]>0){
          n++;
          xm+=x[j];
          xxm+=pow(x[j],2);
        }
      }
      if (n>1){//>=2hits in the rest group)
        number sig,rs;
        sig=sqrt(max((n*xxm-pow(xm,2))/n/(n-1),0.));
        sig=max(sig,sigmin*xm/n);
        sig=min(sig,sigmax*xm/n);
        rs=(x[i]-xm/n)/sig;//deviation in sigmas of the rest
        if (fabs(rs)>rsmx){
          rsmx=fabs(rs);
          imax=i;//store index of hit with max.deviation from the average of the rest
        }
      }
    }
  }

  int cut=(rsmx>rescut&&rescut>=0)?1:0;//=0, if incomp.hit removal was not requested
  imax=cut?imax:-1;//imax =-1 if incomp.hit removal was not requested

  mean=0;
  trres=0;
  n=0;
  number xmx=0;
  int imx=-1;
  for(i=0; i<ntot; i++){//ntot>=2
    if(x[i]>0){
      n++;
      mean+=x[i];
      if(i!=imax) trres+=x[i];
      if(x[i]>xmx){
        xmx=x[i];//store max.hit value
	imx=i;//index
      }
    }
  }

  trmax=mean;
  if(n>1) {
    mean=mean/n;//mean
    trmax=(trmax-xmx)/(n-1);//normal("-max.hit") trunc.mean
    trres=cut?trres/(n-1):trres/n;//"-incomp.hit" trunc.mean/normal_mean
  }

  return rsmx;//return max deviation (if incomp.hit removal requested) or 0
}


number AMSCharge::_probcalc(int mode, int fit, int nhittyp[],number lkhd[]){
//  mode=0/1->TOF/TRK
  number prob[MaxZTypes];
  int i;
  if(fit>=0){
    if(!mode){ 
      for(i=0; i<MaxZTypes; i++)_LkhdTOF[i]=lkhd[i];
      _iTOF=_sortlkhd(mode);//Z-index with max prob.
    }
    else{ 
      for(i=0; i<MaxZTypes; i++)_LkhdTracker[i]=lkhd[i];
      _iTracker=_sortlkhd(mode);
    }
  }

  int types=mode?TrkTypes:1;
  
  int nhit=0;
  for(i=0; i<types; i++) nhit+=nhittyp[i];

  number probmx=1;
//  probmx=0.;
//  for(i=0; i<types; i++){
//    number lkhdnorm=mode?TrkElosPDF::TrkEPDFs[_iTracker].getstep(i):TofElosPDF::TofEPDFs[_iTOF].getstep();
//    if (nhittyp[i]>0) probmx+=nhittyp[i]*log(lkhdnorm);
//  }
//  probmx=1./exp(min(probmx/nhit,powmx));

  number probs(0);
  for(i=0; i<MaxZTypes; i++){
    prob[i]=1./exp(min(lkhd[i]/nhit,powmx))/probmx;
    probs+=prob[i];
  }
  for(i=0; i<MaxZTypes; i++)prob[i]/=probs;// to have total(all charges) prob =1

  if(fit>=0){
    if(!mode){
      for(i=0; i<MaxZTypes; i++){
        _ProbTOF[i]=prob[i];
      }
    }
    else{ 
      for(i=0; i<MaxZTypes; i++){
        _ProbTracker[i]=prob[i];
      }
    }
  }
  int index=mode?_iTracker:_iTOF;
  return prob[index];
} 

number AMSCharge::_probrich(number expRich, number useRich, number lkhd[]){
  number prob[MaxZTypes];
  int i;
  for(i=0; i<MaxZTypes; i++) _LkhdRich[i]=lkhd[i];
  _iRich=_sortlkhd(2);

  for (i=0; i<MaxZTypes; i++){
    number zz=_chargeRich[i]*_chargeRich[i];
    number f=(useRich-zz*expRich)*(useRich-zz*expRich)/zz/expRich;
    _ProbRich[i]=PROB((geant)f,1);
  }
  return _ProbRich[_iRich];
}

int AMSCharge::_sortlkhd(int sort){
//  sort=0/1/2->TOF/TRK/RICH
  number lkhd[MaxZTypes];
  number *pntr[MaxZTypes];
  int index[MaxZTypes];
  int i,j,imax;

  if(!sort){ for(i=0; i<MaxZTypes; i++) lkhd[i]=_LkhdTOF[i];}
  else if (sort==1){ for(i=0; i<MaxZTypes; i++) lkhd[i]=_LkhdTracker[i];}
  else { for(i=0; i<MaxZTypes; i++) lkhd[i]=_LkhdRich[i];}

  for (i=0; i<MaxZTypes; i++) pntr[i]=&lkhd[i];

  AMSsortNAG(pntr,MaxZTypes);//sort in increasing order(1st place - min(-logPw(i)) 
//                                                                ie max.prob)
  for(i=0; i<MaxZTypes; i++) index[i]=-1;
  for(i=0; i<MaxZTypes; i++){
    for(j=0; j<MaxZTypes; j++){
      if (lkhd[j]==(*pntr[i])) {
        index[j]=i;
        lkhd[j]=-999;
        if(!i) imax=j;//store Z-index with max prob.
        break;
      }
    }
    if(index[j]==-1) {
      cerr << " AMSCharge::sortlkhd-E-badly sorted "<<endl;
      return 0;
    }
  }

  if (!sort) { for(i=0; i<MaxZTypes; i++) _IndxTOF[i]=index[i]; }
  else if (sort==1){ for(i=0; i<MaxZTypes; i++) _IndxTracker[i]=index[i]; }
  else{ for(i=0; i<MaxZTypes; i++) _IndxRich[i]=index[i]; }

  return imax;
}


void AMSCharge::_writeEl(){

   int i,j;
#ifdef __WRITEROOT__
    float probtof[4];
    int   chintof[4];
    float probtr[4];
    int   chintr[4];
    float probrc[4];
    int   chinrc[4];
    float proballtr;
  for(i=0; i<4; i++){
    for(j=0; j<MaxZTypes; j++){
      if(_IndxTOF[j]==i){
        probtof[i]=_ProbTOF[j];
        chintof[i]=j+1;
      }
      if(_IndxTracker[j]==i){
        probtr[i]=_ProbTracker[j];
        chintr[i]=j+1;
        if(!i) proballtr=_ProbAllTracker;
      }
      if(_IndxRich[j]==i){
        probrc[i]=_ProbRich[j];
        chinrc[i]=j+1;
      }
    }
  }
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this,
     probtof, chintof, probtr, chintr, probrc, chinrc, proballtr);
#endif
  ChargeNtuple02* CN = AMSJob::gethead()->getntuple()->Get_charge02();

  if (CN->Ncharge>=MAXCHARGE02) return;
// Fill the ntuple
  CN->Status[CN->Ncharge]=_status;
  CN->BetaP[CN->Ncharge]=_pbeta->getpos();
  CN->RingP[CN->Ncharge]=_pring?_pring->getpos():-1;
  CN->ChargeTOF[CN->Ncharge]=_ChargeTOF;
  CN->ChargeTracker[CN->Ncharge]=_ChargeTracker;
  CN->ChargeRich[CN->Ncharge]=_ChargeRich;
  for(i=0; i<4; i++){
    for(j=0; j<MaxZTypes; j++){
      if(_IndxTOF[j]==i){
        CN->ProbTOF[CN->Ncharge][i]=_ProbTOF[j];
        CN->ChInTOF[CN->Ncharge][i]=j+1;
      }
      if(_IndxTracker[j]==i){
        CN->ProbTracker[CN->Ncharge][i]=_ProbTracker[j];
        CN->ChInTracker[CN->Ncharge][i]=j+1;
        if(!i) CN->ProbAllTracker[CN->Ncharge]=_ProbAllTracker;
      }
      if(_IndxRich[j]==i){
        CN->ProbRich[CN->Ncharge][i]=_ProbRich[j];
        CN->ChInRich[CN->Ncharge][i]=j+1;
      }
    }
  }
  CN->TrunTOF[CN->Ncharge]=_TrMeanTOF;
  CN->TrunTOFD[CN->Ncharge]=_TrMeanTOFD;//for the moment(just to keep old format)
  CN->TrunTracker[CN->Ncharge]=_TrMeanTracker;
  CN->Ncharge++;
}


void AMSCharge::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
  ChargeR &ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->Charge(_vpos);
    if (_pbeta) ptr.fBeta=_pbeta->GetClonePointer();
    else ptr.fBeta=-1;
    if (_pring) ptr.fRichRing=_pring->GetClonePointer();
    else ptr.fRichRing=-1;
#endif

}


void AMSCharge::print(){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSCharge",0);
 if(p)p->printC(cout);
}

integer AMSCharge::ind2charge(int idet, int ind){
  if(idet==0)return(_chargeTOF[ind-1]);
  else if(idet==1)return(_chargeTracker[ind-1]);
  else if(idet==2)return(_chargeRich[ind-1]);
  else{
    cout<<"AMSCharge::ind2charge-E-invalid detector id="<<idet<<endl;
    exit(1);
  }
}



AMSChargeI::AMSChargeI(){
  // if(_Count++==0)AMSCharge::init();
}
integer AMSChargeI::_Count=0;

