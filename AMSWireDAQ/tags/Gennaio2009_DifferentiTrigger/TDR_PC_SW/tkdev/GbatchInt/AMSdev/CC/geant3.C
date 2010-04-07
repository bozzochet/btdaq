//  $Id: geant3.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $

#include "typedefs.h"
#include "cern.h"
#include "mceventg.h"
#include "amsgobj.h"
#include "commons.h"
#include <math.h>
#include "trid.h"
//#include <new.h>
#include <limits.h>
#include "extC.h"
//#include <trigger3.h>
#include "job.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "gvolume.h"
#include "gmat.h"
#include "mccluster.h"
#include "event.h"
#include "cont.h"
#include "trrec.h"
#include "daqevt.h"
#include <iostream.h>
#include "richdbc.h"
#include "richid.h"
#include "producer.h"
#include "geantnamespace.h"         
#include "status.h"
#include "ntuple.h"
#ifdef __AMSDEBUG__
static integer globalbadthinghappened=0;

void DumpG3Commons(ostream & o){
  o<< " DumpG3Commons  "<<endl;
  o<< GCKINE.ipart<<endl;
  for(int i=0;i<7;i++)o<<GCTRAK.vect[i]<<" ";
  o<<endl;
  o<<GCTRAK.gekin<<" "<<GCKING.ngkine<<" "<<GCTMED.numed<<endl;
}
#endif

extern "C" void simde_(int&);
extern "C" void trphoton_(int&);
extern "C" void simtrd_(int& );
extern "C" void getscanfl_(int &scan);
extern "C" void gustep_(){
if(    !AMSEvent::gethead()->HasNoCriticalErrors())return;


//AMSmceventg::SaveSeeds();
//cout <<" in gustep "<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
//if (GCTRAK.istop ){
//cout <<GCKINE.ipart<<" "<<GCTRAK.vect[2]<<" "<<GCTRAK.getot<<" "<<GCTMED.numed<<endl;
//}


#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" a " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
    DumpG3Commons(cerr);
  }
#endif
  if(!AMSEvent::gethead()->HasNoCriticalErrors()){
   GCTRAK.istop=1;
   return;
  }
  static integer freq=10;
  static integer trig=0;
  trig=(trig+1)%freq;
//  if(trig==0 && freq>1)AMSgObj::BookTimer.start("GUSTEP");
  if(trig==0 && AMSgObj::BookTimer.check("GEANTTRACKING")>AMSFFKEY.CpuLimit){
//    if(freq>1)AMSgObj::BookTimer.stop("GUSTEP");
    freq=1;
    GCTRAK.istop =1;
    return;
  }
  else if(freq<10)freq=10;

//
// TRD here
//
//  if(trig==0 && freq>1)AMSgObj::BookTimer.start("TrdRadiationGen");
   int scan=0;
   getscanfl_(scan);
  if(!scan){
  if(TRDMCFFKEY.mode <3 && TRDMCFFKEY.mode >=0) {
    //saveliev
    simtrd_(TRDMCFFKEY.g3trd);
    if(TRDMCFFKEY.mode<2){
         if(GCTRAK.gekin != GCTRAK.gekin){
          cerr <<"  gekin problem "<<endl;
          GCTRAK.istop =1;
          AMSEvent::gethead()->seterror(2);
         }
         trphoton_(TRDMCFFKEY.g3trd);
    }
    if(TRDMCFFKEY.mode==0)simde_(TRDMCFFKEY.g3trd);
  }
  else if(TRDMCFFKEY.mode ==3){
    // garibyan
   
  }
  }
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" b " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
//  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("TrdRadiationGen");
/*
{
cout << "gustep "<<GCTRAK.vect[0]<<" "<<GCTRAK.vect[1]<<" "<<GCTRAK.vect[2]<<endl;
 int lvl=GCVOLU.nlevel-1;
 cout <<lvl <<" "<<GCVOLU.names[lvl][0]<<GCVOLU.names[lvl][1]<<GCVOLU.names[lvl][2]<<GCVOLU.names[lvl][3]<<endl;
}
*/

  try{
{
#ifdef __AMSDEV__
 // special rads
 int lvl=GCVOLU.nlevel-1;
 if(GCVOLU.names[lvl][0]=='R' &&
GCVOLU.names[lvl][1]=='A'  && GCVOLU.names[lvl][3]=='T' && (
GCVOLU.names[lvl][2]=='3' )){
static AMSgvolume *pvol3=0;
if(!pvol3)pvol3=AMSJob::gethead()->getgeomvolume(AMSID("RA3T",1));
AMSPoint loc=pvol3->gl2loc(AMSPoint(GCTRAK.vect));
AMSEvent::gethead()->addnext(AMSID("Test",0),new Test(GCKINE.ipart,loc));

}
else  if(GCVOLU.names[lvl][0]=='R' &&
GCVOLU.names[lvl][1]=='A'  && GCVOLU.names[lvl][3]=='T' && (
GCVOLU.names[lvl][2]=='4' )){
static AMSgvolume *pvol4=0;
if(!pvol4)pvol4=AMSJob::gethead()->getgeomvolume(AMSID("RA4T",1));
AMSPoint loc=pvol4->gl2loc(AMSPoint(GCTRAK.vect));
AMSEvent::gethead()->addnext(AMSID("Test",0),new Test(GCKINE.ipart,loc));

}

#endif
}






{
   // TRD
 int lvl=GCVOLU.nlevel-1;
 if(GCTRAK.destep != 0 && GCTMED.isvol != 0 && GCVOLU.names[lvl][0]=='T' && GCVOLU.names[lvl][1]=='R'  && GCVOLU.names[lvl][2]=='D' && GCVOLU.names[lvl][3]=='T'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");


      // Secondary particles are tagged with - for ipart
            AMSTRDMCCluster::sitrdhits(GCVOLU.number[lvl],GCTRAK.vect,
      	   GCTRAK.destep,GCTRAK.gekin,GCTRAK.step,GCKINE.istak?-GCKINE.ipart:GCKINE.ipart,GCKINE.itra);   

      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
} 
}
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" c " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

  //  Tracker
  int lvl= GCVOLU.nlevel-1;  

  if(GCVOLU.nlevel>2 && GCTRAK.destep != 0 && GCTMED.isvol != 0 && 
  GCVOLU.names[2][0]== 'S' &&     GCVOLU.names[2][1]=='T' && 
  GCVOLU.names[2][2]=='K'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
     AMSTrMCCluster::sitkhits(GCVOLU.number[lvl],GCTRAK.vect,
     GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" d " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
  }
  // TOF

  int tflv;  
  char name[4];
  char media[20];
  geant t,x,y,z;
  geant de,dee,dtr2,div,tof,prtq,pstep;
  static geant xpr(0.),ypr(0.),zpr(0.),tpr(0.);
  static geant stepsum(0.),estepsum(0.);
  static geant sscoo[3]={0.,0.,0.};
  static geant sstime(0.);
  static int nsmstps(0);
  static geant ssbx(0),ssby(0),ssbz(0);
  geant trcut2(0.25);// Max. transv.shift (0.5cm)**2
  geant stepmin(0.25);//(cm) min. step/cumul.step to store hit(0.5cm/2)
  geant estepmin(1.e-5);//"small steps" buffer is considered as empty when Eaccumulated below 10kev
  geant coo[3],dx,dy,dz,dt,stepel;
  geant wvect[6],snext,safety;
  int i,nd,numv,iprt,numl,numvp,tfprf(0);
  static int numvo(-999),iprto(-999);
  tflv=GCVOLU.nlevel-1;  
//---> print some GEANT standard values(for debugging):
//  if(GCFLAG.IEVENT==3118)tfprf=1;
//  if(GCFLAG.IEVENT==3118)TOF2DBc::debug=1;
  numl=GCVOLU.nlevel;
  numv=GCVOLU.number[numl-1];
  numvp=GCVOLU.number[numl-2];
//  for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-1][i];
//  UHTOC(GCTMED.natmed,4,media,20);
//
  if(GCVOLU.nlevel==3 && GCTMED.isvol != 0 && GCKINE.charge !=0
            && GCVOLU.names[tflv][0]=='T' && GCVOLU.names[tflv][1]=='F'){// <=== charged part. in "TFnn"
    if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
    iprt=GCKINE.ipart;
    prtq=GCKINE.charge;
    pstep=GCTRAK.step;
    numv=GCVOLU.number[tflv];
    x=GCTRAK.vect[0];
    y=GCTRAK.vect[1];
    z=GCTRAK.vect[2];
    t=GCTRAK.tofg;
    de=GCTRAK.destep;
// cout<<"=====>In TOFsensvol: part="<<iprt<<" numv="<<numv<<" z="<<z<<" t/de="<<t<<" "<<de<<endl;
//
    if(GCTRAK.inwvol==1){// <--- new volume or track : store param.
      iprto=iprt;
      numvo=numv;
      stepsum=0.;
      estepsum=0.;
      nsmstps=0;
      sstime=0;
      sscoo[0]=0;
      sscoo[1]=0;
      sscoo[2]=0;
      xpr=x;
      ypr=y;
      zpr=z;
      tpr=t;
//      if(tfprf)cout<<"---> 1st entry in vol#:"<<numv<<" part="<<iprt<<" time="<<t<<" z="<<z<<endl;
    }
//
    else{// <--- inwvol!=1(still running through given volume or leaving it)
//
      if(iprt==iprto && numv==numvo && de!=0.){// <-- same part. in the same volume, de!=0
//if(tfprf)cout<<"--->The same vol#/part:"<<numv<<" "<<iprt<<" time="<<t<<" z="<<z<<" de="<<de<<" step="<<
//                                                        pstep<<" stop="<<GCTRAK.istop<<endl;
        dx=(x-xpr);
        dy=(y-ypr);
        dz=(z-zpr);
        dt=(t-tpr);
        dtr2=dx*dx+dy*dy;
//
        if(pstep>=stepmin){ // <------ big step
//
          if(dtr2>trcut2){//  big transv. shift: subdivide step
            nd=integer(sqrt(dtr2/trcut2));
            nd+=1;
            dx=dx/geant(nd);
            dy=dy/geant(nd);
            dz=dz/geant(nd);
            dt=dt/geant(nd);
            GCTRAK.destep=de/geant(nd);
            GCTRAK.step=pstep/geant(nd);
            for(i=1;i<=nd;i++){//loop over subdivisions
              coo[0]=xpr+dx*(i-0.5);
              coo[1]=ypr+dy*(i-0.5);
              coo[2]=zpr+dz*(i-0.5);
              tof=tpr+dt*(i-0.5);
              dee=GCTRAK.destep;
//  if(tfprf)cout<<"-->WroBigTrsHit:numv="<<numv<<"x/y/z="<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" de="<<dee<<" tof="<<tof<<endl;
              if(TFMCFFKEY.birks)GBIRK(dee);
              AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
            }
          }// ---> end of "big transv.shift"
//
          else{// <---  small transv.shift(use middle of step params)
//
            coo[0]=xpr+0.5*dx;
            coo[1]=ypr+0.5*dy;
            coo[2]=zpr+0.5*dz;
            tof=tpr+0.5*dt;
            dee=GCTRAK.destep;
//  if(tfprf)cout<<"-->WroSmalTrsHit:numv="<<numv<<"x/y/z="<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" de="<<dee<<" tof="<<tof<<endl;
            if(TFMCFFKEY.birks){
//	      cout<<"----->Bef.Birks:Edep="<<dee<<"  Q="<<GCKINE.charge<<" step="<<GCTRAK.step<<endl;
	      GBIRK(dee);
//	      cout<<"----->Aft.Birks:Edep="<<dee<<endl;
	    }
            AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
          }// ---> endof "small transv.shift"
//
        }// ------> endof "big step"
//--------
	else{//      <--- small step - accumulate in buffer
          if(pstep==0 && GCTRAK.istop>0){//abnorm.case: step=0(stop/decay/inel_inter)->just write hit 
//	    if(tfprf)cout<<"WroStep=0:numv="<<numv<<" de="<<de<<" z/t="<<z<<" "<<t<<" stop="<<GCTRAK.istop<<endl;
	    GCTRAK.step=0.005;//fict.step
	    coo[0]=x;
	    coo[1]=y;
	    coo[2]=z;
	    tof=t;
	    dee=GCTRAK.destep;
            if(TFMCFFKEY.birks)GBIRK(dee);
            AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
	  }
	  else{//normal case (step>0)
	    if(nsmstps>0){//buffer is not empty, check on broken sequence
	      if(ssbx!=xpr || ssby!=ypr || ssbz!=zpr){//broken sequence: flush buffer and fill with current step
	        sscoo[0]/=geant(nsmstps);
	        sscoo[1]/=geant(nsmstps);
	        sscoo[2]/=geant(nsmstps);
	        sstime/=geant(nsmstps);
  if(tfprf){
    cout<<"-->WroSSBufBrokenSeq:numv="<<numv<<"SSB_last:x/y/z="<<ssbx<<" "<<ssby<<" "<<ssbz<<endl;
    cout<<" CurrentStepBegin:x/y/z="<<xpr<<" "<<ypr<<" "<<zpr<<" SSB:x/y/z="<<sscoo[0]<<" "<<sscoo[1]<<" "<<sscoo[2]<<endl;
    cout<<" Eacc/nst="<<estepsum<<" "<<nsmstps<<" tof:ssb/curr="<<sstime<<" "<<t<<endl;
  }
                GCTRAK.destep=estepsum;
                GCTRAK.step=stepsum;
                if(TFMCFFKEY.birks)GBIRK(estepsum);
                AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
	        stepsum=pstep;//fill with current small step...
	        estepsum=de;
                nsmstps=1;
                sstime=t;
                sscoo[0]=x;
                sscoo[1]=y;
                sscoo[2]=z;
	        ssbx=x;
	        ssby=y;
	        ssbz=z;
	      }
	      else{//continious sequence: add current small step
	        stepsum+=pstep;
	        estepsum+=GCTRAK.destep;
	        nsmstps+=1;
	        sscoo[0]+=x;
	        sscoo[1]+=y;
	        sscoo[2]+=z;
	        sstime+=t;
	        ssbx=x;
	        ssby=y;
	        ssbz=z;
	        if(stepsum>=stepmin){//flush buffer if accumulated enough small steps 
	          sscoo[0]/=geant(nsmstps);
	          sscoo[1]/=geant(nsmstps);
	          sscoo[2]/=geant(nsmstps);
	          sstime/=geant(nsmstps);
//  if(tfprf)cout<<"-->WroSSBufContSeq:numv="<<numv<<"SSB:x/y/z="<<sscoo[0]<<" "<<sscoo[1]<<" "<<sscoo[2]
//                        <<" Eacc/nst="<<estepsum<<" "<<nsmstps<<" SSB:tof="<<sstime<<endl;
                  GCTRAK.destep=estepsum;
                  GCTRAK.step=stepsum;
                  if(TFMCFFKEY.birks)GBIRK(estepsum);
                  AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
	          stepsum=0.;
	          estepsum=0.;
                  nsmstps=0;
                  sstime=0;
                  sscoo[0]=0;
                  sscoo[1]=0;
                  sscoo[2]=0;
	        }
	      }//--->endof "cont.sequence"
	    }//--->endof "non-empty buffer"
	    else{//buffer is empty: fill with current smal step
	      stepsum=pstep;//fill with current small step...
	      estepsum=GCTRAK.destep;
              nsmstps=1;
              sstime=t;
              sscoo[0]=x;
              sscoo[1]=y;
              sscoo[2]=z;
	      ssbx=x;
	      ssby=y;
	      ssbz=z;
	    }//--->endof "buffer is empty"
	  }//---> endof "normal case step>0" 
	}//---> endof "small step"
//--------
        if((GCTRAK.inwvol>=2 || GCTRAK.istop>0)  && estepsum>estepmin){// on leave/stop: write "small steps" buffer if not empty
	  sscoo[0]/=geant(nsmstps);
	  sscoo[1]/=geant(nsmstps);
	  sscoo[2]/=geant(nsmstps);
	  sstime/=geant(nsmstps);
//  if(tfprf){
//    cout<<"-->WroSSBufOnLeaveStop:numv="<<numv<<"SSB:x/y/z="<<sscoo[0]<<" "<<sscoo[1]<<" "<<sscoo[2]<<endl;
//    cout<<" STacc/Eacc/nst="<<stepsum<<" "<<estepsum<<" "<<nsmstps<<" SSB:tof="<<sstime<<" stop="<<GCTRAK.istop<<endl;
//  }
          GCTRAK.destep=estepsum;
          GCTRAK.step=stepsum;
          if(TFMCFFKEY.birks)GBIRK(estepsum);
          AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
	}//---> endof "on leave/stop" actions
//
        xpr=x;
        ypr=y;
        zpr=z;
        tpr=t;
      }// ===> end of "same part/vol, de>0"
//
    }// ===> endof "inwvol!=0"(still running throuhg given volume or leave it)
//
  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
//
  }// ===> endof "in TFnn"
//-------------------------------------

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" e " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif


  // =======>  ANTI,  mod. by E.C.
//---> read some GEANT standard values(for debugging): 
//  numl=GCVOLU.nlevel;
//  numv=GCVOLU.number[numl-1];
//  numvp=GCVOLU.number[numl-2];
//  for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-1][i];
//  cout<<"Volume "<<name<<" number="<<numv<<" level="<<numl<<" sens="<<GCTMED.isvol<<endl;
//  iprt=GCKINE.ipart;
//  x=GCTRAK.vect[0];
//  y=GCTRAK.vect[1];
//  z=GCTRAK.vect[2];
//  t=GCTRAK.tofg;
//  de=GCTRAK.destep;
//  cout<<"Part="<<iprt<<" x/y/z="<<x<<" "<<y<<" "<<z<<" de="<<de<<endl;
//  UHTOC(GCTMED.natmed,4,media,20);
//  cout<<" Media "<<media<<endl;
//--->
  integer manti(0),isphys,islog;
  if(lvl==3 && GCVOLU.names[lvl][0]== 'A' && GCVOLU.names[lvl][1]=='N'
                                       && GCVOLU.names[lvl][2]=='T')manti=1;
  if(GCTRAK.destep != 0  && GCTMED.isvol != 0 && manti==1){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
     dee=GCTRAK.destep; 
     if(TFMCFFKEY.birks)GBIRK(dee);
     isphys=GCVOLU.number[lvl];
     islog=floor(0.5*(isphys-1))+1;//not used now
     AMSAntiMCCluster::siantihits(isphys,GCTRAK.vect,dee,GCTRAK.tofg);
//     HF1(1510,geant(iprt),1.);
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
  }
//
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" f " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
//
// ---------------> ECAL 1.0-version by E.C.
//
// lines below are for check of TRK imp.point accuracy:
//    if(GCTRAK.inwvol==1
//     && GCVOLU.names[lvl][0]=='F' && GCVOLU.names[lvl][1]=='L'
//     && GCVOLU.names[lvl][2]=='0'&& GCVOLU.names[lvl][3]=='5'
//    ){
//      if(GCKINE.ipart==47){//"47" if He-calibration
//        AMSEcalMCHit::impoint[0]=GCTRAK.vect[0];
//        AMSEcalMCHit::impoint[1]=GCTRAK.vect[1];
//      }
//    }
    if(GCTRAK.destep != 0.){
      if(lvl==6 && GCVOLU.names[lvl][0]== 'E' && GCVOLU.names[lvl][1]=='C'
               ){
//               && GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]=='C'){
//       
//       cout<<"lev/vol="<<numl<<" "<<numv<<" name="<<name<<" x/y="<<x<<" "<<y<<" z="<<z<<" de="<<de<<endl;
//       for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-2][i];
//       cout<<"vol(lev-1)="<<numvp<<" name="<<name<<endl;
        if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
        dee=GCTRAK.destep;
	if(GCTRAK.destep<0)cout<<"----> destep<0 "<<GCTRAK.destep<<endl;
        if(TFMCFFKEY.birks)GBIRK(dee);
	if(dee<=0)cout<<"---->Birks:dee<0 "<<dee<<" step="<<GCTRAK.step<<" destep="<<GCTRAK.destep
	<<" ipart="<<GCKINE.ipart<<endl;
        //cout << GCVOLU.names[lvl][0]<< GCVOLU.names[lvl][1]<< GCVOLU.names[lvl][2]<< GCVOLU.names[lvl][3]<<endl;
        //cout <<" ecal "<<GCVOLU.number[lvl-1]<< " "<<dee<<endl;
        AMSEcalMCHit::siecalhits(GCVOLU.number[lvl-1],GCTRAK.vect,dee,GCTRAK.tofg);

        if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
      }
    }
//

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" g " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

    // RICH simulation code
    static int fscatcounter;
    if(GCKINE.ipart==Cerenkov_photon){
// Cut by hand 
     if(GCTRAK.nstep>6000) GCTRAK.istop=1;
     if(GCVOLU.names[lvl][0]=='R' && GCVOLU.names[lvl][1]=='I' &&
       GCVOLU.names[lvl][2]=='C' && GCVOLU.names[lvl][3]=='H'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
      for(integer i=0;i<GCTRAK.nmec;i++){
        if(GCTRAK.lmec[i]==106) RICHDB::numrefm++;
        if(GCTRAK.lmec[i]==2000) fscatcounter=1;
      }
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
     }



     // Added the counters for the NAF

     if((GCVOLU.names[lvl][0]=='R' && GCVOLU.names[lvl][1]=='A' &&
	 GCVOLU.names[lvl][2]=='D' && GCVOLU.names[lvl][3]==' ')||
	(GCVOLU.names[lvl][0]=='N' && GCVOLU.names[lvl][1]=='A' &&
	 GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]==' ')){
       if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");

      if(GCTRAK.nstep!=0){
       for(integer i=0;i<GCTRAK.nmec;i++)
          if(GCTRAK.lmec[i]==1999) RICHDB::numrayl++;
      }        
      else{
	RICHDB::numrayl=0;
	RICHDB::numrefm=0;
	fscatcounter=0;
#ifdef __AMSDEBUG__
	//	static float max=-74.81;
	//	if(GCTRAK.vect[2]>max){
	//	  max=GCTRAK.vect[2];
	//	  cout<<"Current upper limit at "<<max<<endl;
	//	  cout<<"Interference "<<max-RICHDB::RICradpos()<<endl;
	//	}
#endif

//        if(!RICHDB::detcer(GCTRAK.vect[6])) GCTRAK.istop=1; 
//          else RICHDB::nphgen++; 
         RICHDB::nphgen++;
      }          
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");

     }

}

    // THIS IS A TEST
        if(GCTRAK.inwvol==2 && GCVOLU.names[lvl][0]=='F' &&
           GCVOLU.names[lvl][1]=='O' && GCVOLU.names[lvl][2]=='I' &&
           GCVOLU.names[lvl][3]=='L' && GCTRAK.nstep>1 && GCTRAK.vect[5]<0 && RICHDB::numrayl==0){
	  RICHDB::nphrad++;
	}
        if(GCTRAK.inwvol==1 && GCVOLU.names[lvl][0]=='S' &&
           GCVOLU.names[lvl][1]=='L' && GCVOLU.names[lvl][2]=='G' &&
           GCVOLU.names[lvl][3]=='C' && GCTRAK.nstep>1 && GCTRAK.vect[5]<0 && RICHDB::numrayl==0){
	  RICHDB::nphbas++;
	}

#ifdef __AMSDEBUG__
    //    if(GCTRAK.inwvol==1 && GCVOLU.names[lvl][0]=='S' &&
    //       GCVOLU.names[lvl][1]=='L' && GCVOLU.names[lvl][2]=='G' &&
    //       GCVOLU.names[lvl][3]=='C' && GCTRAK.nstep>1){
    //      cout <<"Entering in LG at "<<GCTRAK.vect[0]<<" "<<GCTRAK.vect[1]<<" "
    //	   <<GCTRAK.vect[2]<<endl;
    //      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
    //      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
    //    }
#endif


    if(GCVOLU.names[lvl][0]=='C' && GCVOLU.names[lvl][1]=='A' &&
       GCVOLU.names[lvl][2]=='T' && GCVOLU.names[lvl][3]=='O' && 
       GCTRAK.inwvol==1){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
#ifdef __AMSDEBUG__
      if(GCKINE.vert[2]>-85. && GCKINE.ipart==Cerenkov_photon ){
	cout << "Emited at "<<GCKINE.vert[2]<<endl;
      }
#endif

      if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep==0){

        GCTRAK.istop=1;

//        if(RICHDB::detcer(GCTRAK.vect[6])) {
//          GCTRAK.istop=2;

          geant xl=(AMSRICHIdGeom::pmt_pos(1,2)-RICHDB::cato_pos()+RICHDB::RICradpos()-RICotherthk/2-
                   GCTRAK.vect[2])/GCTRAK.vect[5];


          geant vect[3];
          vect[0]=GCTRAK.vect[0]+xl*GCTRAK.vect[3];
          vect[1]=GCTRAK.vect[1]+xl*GCTRAK.vect[4];
          vect[2]=GCTRAK.vect[2]+xl*GCTRAK.vect[5];

#ifdef __AMSDEBUG__
/*
    cout <<"************** vect vs orig vect "<<vect[2]<<" "<<GCTRAK.vect[2]<<endl;
    cout <<"Decompose as "<<AMSRICHIdGeom::pmt_pos(1,2)<<" "<<-RICHDB::cato_pos()<<
           " "<<RICHDB::RICradpos()<<" "<<-RICotherthk<<" compared with "<<GCTRAK.vect[2]<<endl;
*/
#endif


          AMSRichMCHit::sirichhits(GCKINE.ipart,
                                   GCVOLU.number[lvl-1]-1,
//                                   GCTRAK.vect,
                                   vect,
                                   GCKINE.vert,
                                   GCKINE.pvert,
                                   Status_Window-
                                   (GCKINE.itra!=1?100:0));
//        }
      }



      if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep!=0){
        GCTRAK.istop=2; // Absorb it
        geant xl=(AMSRICHIdGeom::pmt_pos(1,2)-RICHDB::cato_pos()+RICHDB::RICradpos()-RICotherthk/2-
                   GCTRAK.vect[2])/GCTRAK.vect[5];
                 
        geant vect[3];
        vect[0]=GCTRAK.vect[0]+xl*GCTRAK.vect[3];
        vect[1]=GCTRAK.vect[1]+xl*GCTRAK.vect[4];
        vect[2]=GCTRAK.vect[2]+xl*GCTRAK.vect[5];
#ifdef __AMSDEBUG__
/*
    cout <<"************** vect vs orig vect"<<vect[2]<<" "<<GCTRAK.vect[2]<<endl;
    cout <<"Decompose as "<<AMSRICHIdGeom::pmt_pos(1,2)<<" "<<-RICHDB::cato_pos()<<
           " "<<RICHDB::RICradpos()<<" "<<-RICotherthk<<" compared with "<<GCTRAK.vect[2]<<endl;
*/
#endif
        if(GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::rich_height-
           RICHDB::foil_height-RICradmirgap-RIClgdmirgap // in LG
           || (GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height &&
               GCKINE.vert[2]>RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height))
	  AMSRichMCHit::sirichhits(GCKINE.ipart,
				   GCVOLU.number[lvl-1]-1,
				   //GCTRAK.vect,
                                   vect,
				   GCKINE.vert,
				   GCKINE.pvert,
				   Status_LG_origin-
				   (GCKINE.itra!=1?100:0));	  
        else
	  AMSRichMCHit::sirichhits(GCKINE.ipart,
				   GCVOLU.number[lvl-1]-1,
				   //GCTRAK.vect,
                                   vect,
				   GCKINE.vert,
				   GCKINE.pvert,
				   fscatcounter*1000+
				   (GCKINE.itra!=1?100:0)+
				   RICHDB::numrefm*10+
				   (RICHDB::numrayl>0?Status_Rayleigh:0));
      }else if(GCTRAK.nstep!=0){	 
        AMSRichMCHit::sirichhits(GCKINE.ipart,
				 GCVOLU.number[lvl-1]-1,
				 GCTRAK.vect,
				 GCKINE.vert,
				 GCKINE.pvert,
                                 Status_No_Cerenkov-
                                 (GCKINE.itra!=1?100:0));
      }				   
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
    }

   
    // More RICH information added for MC
    if(GCKINE.itra==1 && GCVOLU.names[lvl][0]=='R' && 
      GCVOLU.names[lvl][1]=='A' &&GCVOLU.names[lvl][2]=='D' && 
      GCVOLU.names[lvl][3]==' ' && GCKINE.ipart!=50 &&
      GCTRAK.inwvol==1 && GCKINE.charge!=0 && GCTRAK.vect[5]<0)
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_rad);



    if(GCKINE.itra==1 && GCVOLU.names[lvl][0]=='N' && 
      GCVOLU.names[lvl][1]=='A' &&GCVOLU.names[lvl][2]=='F' && 
      GCVOLU.names[lvl][3]==' ' && GCKINE.ipart!=50 &&
      GCTRAK.inwvol==1 && GCKINE.charge!=0 && GCTRAK.vect[5]<0)
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_rad);


    if(RICCONTROLFFKEY.iflgk_flag){  // This to be checked


      if(GCKINE.itra==1 &&GCVOLU.names[lvl][0]=='R' && 
	 GCVOLU.names[lvl][1]=='A' &&GCVOLU.names[lvl][2]=='D' && 
	 GCVOLU.names[lvl][3]=='B' && GCKINE.ipart!=50 &&
	 GCTRAK.inwvol==1){
	AMSRichMCHit::sirichhits(GCKINE.ipart,
				 0,
				 GCTRAK.vect,
				 GCTRAK.vect,
				 GCTRAK.vect+3,
				 Status_primary_radb);
      }
    
    if(GCKINE.itra==1 &&GCVOLU.names[lvl][0]=='S' && 
       GCVOLU.names[lvl][1]=='T' &&GCVOLU.names[lvl][2]=='K' 
       && GCKINE.ipart!=50 && GCTRAK.inwvol==1) //Tracker
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_tracker);

    if(GCKINE.itra==1 &&GCVOLU.names[lvl][0]=='T' && 
       GCVOLU.names[lvl][1]=='O' &&GCVOLU.names[lvl][2]=='F' && 
       GCVOLU.names[lvl][2]=='H'&& GCKINE.ipart!=50 
       && GCTRAK.inwvol==1) //TOF
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_tof); 

    }



#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" h " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

//  if(trig==0 && freq>1)AMSgObj::BookTimer.start("SYSGUSTEP");
  AMSmceventg::FillMCInfo();

  if(RICCONTROLFFKEY.iflgk_flag){
   for(int i=0;i<GCKING.ngkine;i++){
     if(GCKING.iflgk[i]==0) GCKING.iflgk[i]=1;
//     GCKING.iflgk[i]=1;
   }
  }
  GCTRAK.upwght=0;
 //cout <<" nalive "<<GCNUMX.NALIVE+GCKING.ngkine<<endl;

//  if(GCNUMX.NALIVE+GCKING.ngkine>2000)throw AMSTrTrackError("SecondaryStackOverflows"); 
  GSKING(0);
  GCTRAK.upwght=1; //cerenkov tracked first  
  for(integer i=0;i<GCKIN2.ngphot;i++){
    if(RICHDB::detcer(GCKIN2.xphot[i][6])){
       if(GCNUMX.NALIVE>5000)throw AMSTrTrackError("SecondaryPhStackOverflows");
       GSKPHO(i+1);
//       cout << " NALIVE " <<GCNUMX.NALIVE<< " "<<GCKIN2.ngphot<<endl;
    }
  }
  GCTRAK.upwght=0;  
//  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("SYSGUSTEP");
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" i " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
#ifndef __BATCH__
//GSXYZ();
GDCXYZ();
#endif
  }
  
   catch (AMSuPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->seterror(2);
//     AMSEvent::gethead()->Recovery();
    }
   catch (AMSaPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->seterror(2);
//    AMSEvent::gethead()->Recovery();
    }
   catch (AMSTrTrackError e){
    cerr << "GUSTEP  "<< e.getmessage()<<endl;
     AMSEvent::gethead()->seterror(2);
   }
//  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("GUSTEP");
   //  cout <<" gustep out"<<endl;

}
//-----------------------------------------------------------------------
extern "C" void guout_(){
/*
AMSgvolume *p =AMSJob::gethead()->getgeomvolume(AMSID("OMIR",1));
   for(int k=0;k<3;k++)cout<<p->getcooA(k)<<" ";
   cout <<endl;
   for(int k=0;k<5;k++)cout<<p->getpar(k)<<"  ";
   cout <<endl;
    abort();
*/
if(    AMSEvent::gethead()->HasNoCriticalErrors()){
  RICHDB::Nph()=0;
   try{
   if(AMSJob::gethead()->isSimulation()){
         CCFFKEY.curtime=AMSEvent::gethead()->gettime();
      if(GCFLAG.IEOTRI)AMSJob::gethead()->uhend(AMSEvent::gethead()->getrun(),
AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime());
      number tt=AMSgObj::BookTimer.stop("GEANTTRACKING");
      AMSTrTrack::TimeLimit()=AMSFFKEY.CpuLimit-tt;
//        cout <<  "  tt   " <<tt<<endl;
#ifdef __AMSDEBUG__
      globalbadthinghappened=0;
#endif
      if(tt > AMSFFKEY.CpuLimit){
       int nsec=(AMSEvent::gethead()->getC("AMSmceventg",0))->getnelem();
        cerr<<" GEANTTRACKING Time Spent"<<tt<<" "<<nsec<<" Secondaries Generated"<<endl;
        if(nsec==1 && tt>AMSFFKEY.CpuLimit*1.2 ){
// bad thing
          (AMSEvent::gethead()->getC("AMSmceventg",0))->printC(cerr);
#ifdef __AMSDEBUG__
      globalbadthinghappened=1;
#endif
        }
       throw AMSTrTrackError("SimCPULimit exceeded");
      }
   }
          if(AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
   }
   catch (AMSuPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSaPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     AMSEvent::gethead()->seterror(2);
/*
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(512000);
      return;
*/
   }
   catch (amsglobalerror e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     AMSEvent::gethead()->seterror(e.getlevel());
     if(e.getlevel()>2)throw e; 
     
/*
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(512000);
      return;
*/
   }
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
      AMSgObj::BookTimer.start("GUOUT");
      if(GCFLAG.IEVENT%abs(GCFLAG.ITEST)==0 ||     GCFLAG.IEORUN || GCFLAG.IEOTRI || 
         GCFLAG.IEVENT>=GCFLAG.NEVENT)
      AMSEvent::gethead()->printA(AMSEvent::debug);
     integer trig;
     if(AMSJob::gethead()->gettriggerOr()){
      trig=0;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trig+=p->getnelem();
         else break;
        }
       }
     }
     else{
      trig=1;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        integer trigl=0;
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trigl+=p->getnelem();
         else break;
        }
        if(trigl==0)trig=0;
       }
     }
//   if(trig ){ 
//     AMSEvent::gethead()->copy();
//   }
     AMSEvent::gethead()->write(trig);
}
     UPool.Release(0);
   AMSEvent::gethead()->remove();
     UPool.Release(1);
   AMSEvent::sethead(0);
   UPool.erase(2000000);
   AMSgObj::BookTimer.stop("GUOUT");

// allow termination via time via datacard
{  
   float xx,yy;
   TIMEX(xx);   
   TIMEL(yy);   
   if(GCTIME.TIMEND < xx || (yy>0 && yy<AMSFFKEY.CpuLimit) ){
    GCTIME.ITIME=1;
   }
}
}

extern "C" void abinelclear_();
extern "C" void gukine_(){
AMSgObj::BookTimer.start("GUKINE");

// Set update flag to zero, as geant3 sometimes doesn;t do it itself
 GCTMED.iupd=0;

abinelclear_();
static integer event=0;

#ifdef __DB_All__
  if (AMSFFKEY.Read > 1) {
    readDB();
     AMSgObj::BookTimer.stop("GUKINE");
     return;
  }
#endif
try{
// create new event & initialize it
  if(AMSJob::gethead()->isSimulation()){
    AMSgObj::BookTimer.start("GEANTTRACKING");
   if(IOPA.mode%10 !=1 ){
    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
     GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
     AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);

    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     genp->run(GCKINE.ikine);
     if (CCFFKEY.npat>1 && TRDMCFFKEY.multiples==1){
	 if (i>0){
	     genp->gendelay();
	   }
       }
     //genp->_printEl(cout);

    }
   }
   }
   else {
    AMSIO io;
    if(IOPA.mode/10?io.readA():io.read()){
     AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
     new AMSEvent(AMSID("Event",io.getevent()),io.getrun(),0,io.gettime(),io.getnsec(),
     io.getpolephi(),io.getstheta(),io.getsphi(),io.getveltheta(),
     io.getvelphi(),io.getrad(),io.getyaw(),io.getpitch(),io.getroll(),io.getangvel())));
     AMSmceventg* genp=new AMSmceventg(io);
     if(genp){
      AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
      genp->run();
      //genp->_printEl(cout);
     }
    }
    else{
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     AMSgObj::BookTimer.stop("GUKINE");
     return;
    }   
   }

  }
  else {
    //
    // read daq    
    //
    DAQEvent * pdaq=0;
    DAQEvent::InitResult res=DAQEvent::init();
    for(;;){
     if(res==DAQEvent::OK){ 
        AMSJob::gethead()->gettimestructure(AMSEvent::getTDVStatus());
       pdaq = new DAQEvent();
       uinteger run;
       uinteger event;
        time_t tt;
       if(pdaq->read()){
         run=pdaq->runno();
         event=pdaq->eventno();
         tt=pdaq->time();   
        AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
        new AMSEvent(AMSID("Event",pdaq->eventno()),pdaq->runno(),
        pdaq->runtype(),pdaq->time(),pdaq->usec())));
        AMSEvent::gethead()->addnext(AMSID("DAQEvent",pdaq->GetBlType()), pdaq);
        if(SELECTFFKEY.Run==SELECTFFKEY.RunE && SELECTFFKEY.EventE && AMSEvent::gethead()->getid()>=SELECTFFKEY.EventE){
         pdaq->SetEOFIn();    
        } 
        if(GCFLAG.IEORUN==2){
      // if production 
      // try to update the badrun list
         if(AMSJob::gethead()->isProduction() && AMSJob::gethead()->isRealData()){
           char fname[256];
           char * logdir = getenv("ProductionLogDirLocal");
           if(logdir){
            strcpy(fname,logdir);
           }
           else {
             cerr<<"gukine-E-NoProductionLogDirLocalFound"<<endl;
             strcpy(fname,"/Offline/local/logs");
           }
           strcat(fname,"/BadRunsList");
           ofstream ftxt;
           ftxt.open(fname,ios::app);
           if(ftxt){
            ftxt <<pdaq->runno()<<" "<<pdaq->eventno()<<endl;
            ftxt.close();
           }
           else{
            cerr<<"gukine-S-CouldNotOPenFile "<<fname<<endl;
            exit(1);
           }
           
         }
        pdaq->SetEOFIn();    
        GCFLAG.IEORUN=-2;
      }
      else if (GCFLAG.IEORUN==-2){
        GCFLAG.IEORUN=0;
      //  AMSJob::gethead()->uhinit(pdaq->runno(),pdaq->eventno());
      }
      guout_();
      if(GCFLAG.IEOTRI || GCFLAG.IEVENT >= GCFLAG.NEVENT)break;
      GCFLAG.IEVENT++;
    }
    else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend(run,event,tt);
     AMSID tdvs=AMSEvent::getTDVStatus();
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(tdvs);
  if(AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
     AMSID tdvs=AMSEvent::getTDVStatus();
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(tdvs);
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time_t begino,endo,inserto;
      ptdv->gettime(inserto,begino,endo);
      time_t begin,end,insert;
      begin=AMSJob::gethead()->getstatustable()->getbegin();
      end=AMSJob::gethead()->getstatustable()->getend();
      time(&insert);
      ptdv->SetTime(insert,begin,end);
      cout <<" Event Status info  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
      cout << " Starting to update "<<*ptdv; 
      bool fail=false;
      if(  !ptdv->write(AMSDATADIR.amsdatabase)){
         cerr <<"AMSEvent::_init-S-ProblemtoUpdate "<<*ptdv;
          fail=true;
      }
      AMSStatus *p=AMSJob::gethead()->getstatustable();
      uinteger first,last;
      p->getFL(first,last);
      AMSProducer::gethead()->sendEventTagEnd(ptdv->getname(),p->getrun(),insert,begin,end,first,last,p->getnelem(),fail);       
      ptdv->SetTime(inserto,begino,endo);
      AMSJob::gethead()->getstatustable()->reset();      
  }

     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo();
     res=DAQEvent::init();
    }
    catch (AMSClientError a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     break;
#endif
    }
   }
   else if (res==DAQEvent::Interrupt)break;
   else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend();
     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo();
     res=DAQEvent::init();
    }
    catch (AMSClientError a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     break;
#endif
   }
   }
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
      AMSgObj::BookTimer.stop("GUKINE");
     return; 
  }
}
catch (amsglobalerror & a){
 cerr<<a.getmessage()<< endl;
#ifdef __CORBA__
  if(a.getlevel()>2){
  AMSClientError ab((const char*)a.getmessage(),DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
 }
#endif
    gams::UGLAST(a.getmessage());
    exit(1);
}
    AMSgObj::BookTimer.stop("GUKINE");

}


