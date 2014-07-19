// v1.0 8.12.2000 by E.Choumilov
//
#include <iostream.h>
#include <stdio.h>
#include "typedefs.h"
#include "cern.h"
#include "extC.h"
#include <math.h>
#include "point.h"
#include "amsgobj.h"
#include "event.h"
#include "amsstl.h"
#include "commons.h"
#include "mccluster.h"
#include "trrec.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antirec02.h"
#include "ecaldbc.h"
#include "ecalrec.h"
#include "ecalcalib.h"
#include "trigger102.h"
#include "trigger302.h"
#include "particle.h"
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
#include "timeid.h"
using namespace ecalconst;
//
//
number ECREUNcalib::slslow;
number ECREUNcalib::slfast;
number ECREUNcalib::fastfr;
integer ECREUNcalib::nfits; 
number ECREUNcalib::pxrgain[ECPMSL][4];
integer ECREUNcalib::pxstat[ECPMSL][4]; 
number ECREUNcalib::pmrgain[ECPMSL];    
number ECREUNcalib::pmlres[ECPMSL][ECCLBMX];    
number ECREUNcalib::pmlres2[ECPMSL][ECCLBMX];    
number ECREUNcalib::sbcres[ECPMSL][4];    
number ECREUNcalib::hi2lowr[ECPMSL][4]; 
number ECREUNcalib::an2dynr[ECPMSL]; 
number ECREUNcalib::tevpml[ECPMSL][ECCLBMX]; 
number ECREUNcalib::tevsbf[ECPMSL][4]; 
number ECREUNcalib::tevsbc[ECPMSL][4]; 
number ECREUNcalib::tevpmc[ECPMSL];
number ECREUNcalib::tevpmf[ECPMSL];
number ECREUNcalib::tevpmm[ECPMSL];
number ECREUNcalib::values[ECCLBMX]; 
number ECREUNcalib::errors[ECCLBMX]; 
number ECREUNcalib::coords[ECCLBMX];
integer ECREUNcalib::nbins; 
number ECREUNcalib::tevhlc[ECPMSL*4][ECCHBMX]; 
number ECREUNcalib::sbchlc[ECPMSL*4][ECCHBMX]; 
number ECREUNcalib::sbchlc2[ECPMSL*4][ECCHBMX];
number ECREUNcalib::et2momr;
integer ECREUNcalib::net2mom; 
//-----------------------------
void ECREUNcalib::init(){
  int i,j,k;
//
  cout<<"====> ECREUNcalibration run started....."<<endl;
//
  for(i=0;i<ECPMSL;i++){//pm*sl
    for(j=0;j<ECCLBMX;j++){//Lbin
      pmlres[i][j]=0.;
      pmlres2[i][j]=0.;
      tevpml[i][j]=0.;
    }
    tevpmc[i]=0.;
    tevpmf[i]=0.;
    tevpmm[i]=0.;
    pmrgain[i]=1.;//default value
    an2dynr[i]=10.;//default value
    for(j=0;j<4;j++){//subcell
      sbcres[i][j]=0.;
      tevsbc[i][j]=0.;
      tevsbf[i][j]=0.;
      pxrgain[i][j]=1.;//default value
      pxstat[i][j]=0;//default value
      hi2lowr[i][j]=16.;//default value
    }
  }
// for hi2low calibr.
  for(i=0;i<ECPMSL*4;i++){//pm*sl*sbc
    for(j=0;j<ECCHBMX;j++){//Hg-chan. bin
      tevhlc[i][j]=0;
      sbchlc[i][j]=0;
      sbchlc2[i][j]=0;
    }
  }
  et2momr=0.;
  net2mom=0;
}
//-----------------------------
void ECREUNcalib::select(){
  int i,j,k;
  integer sta,status,dbstat,id,idd,isl,pmt,sbc,pmsl,rdir;
  geant padc[2];
  integer npmx,nslmx,maxpl,maxcl,nraw,ovfl[2],proj,plane,cell,lbin;
  number radc[2],binw,trc[3],pmb[3],rrr,sbl,sbr,crl,crr;
  number ct1,ct2,cl1,cl2,dct,z1,z2,hflen,ctcr,ctpm,clcr,clsh,clpm,acorr;
  number sumh[ECPMSMX][4],suml[ECPMSMX][4],pmsum,pmdis; 
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr,ph,pl,sh,sl,apmt;
  geant pmpit,pxsiz;
  AMSEcalRawEvent * ptr;
  AMSEcalHit * ptr1;
//
  AMSContainer *cptr;
  AMSParticle *ppart;
  AMSTrTrack *ptrack;
  AMSCharge  *pcharge;
  AMSBeta  *pbeta;
  AMSPoint C0,Cout1,Cout2,Cpmt;
  AMSDir dir(0,0,1.);
  int ntrk,ipatt;
  number chi2,the,phi,rid,err,trl,beta,ebeta,momentum;
  integer chargeTOF,chargeTracker;
//
  npmx=ECALDBc::slstruc(4);//numb.of PM's/Sl
  nslmx=ECALDBc::slstruc(3);//numb.of Sl
  pxsiz=ECALDBc::rdcell(5);// pm(lg)-pixel size(imply =pm_size/2)
  pmpit=ECALDBc::rdcell(7);// pm transv.pitch
  maxpl=2*ECALDBc::slstruc(3);// SubCell planes
  maxcl=2*ECALDBc::slstruc(4);// SubCells per plane
//
// ---------------> get track info :
//
  EcalJobStat::addca(0);
// 
  ntrk=0;
  sta=1;
  cptr=AMSEvent::gethead()->getC("AMSParticle",0);// get TOF-matched track
  if(cptr)
          ntrk+=cptr->getnelem();
  if(ntrk!=1)return;// require events with 1 track.
  ppart=(AMSParticle*)AMSEvent::gethead()->
                                           getheadC("AMSParticle",0);
  if(ppart){
    ptrack=ppart->getptrack();
    ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
    pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
    chargeTracker=pcharge->getchargeTracker();
    chargeTOF=pcharge->getchargeTOF();
    pbeta=pcharge->getpbeta();
    beta=0.;
    if(pbeta)beta=pbeta->getbeta();
    ebeta=pbeta->getebeta()*beta*beta;
    momentum=rid*chargeTracker;
  } 
  else{
    rid=0.;
    the=0.;
    chi2=-1.;
  }
//
  if(chi2<0)return;
  EcalJobStat::addca(1);
// ---------------> check that track is high-mom,top->bot and may hit ECAL:
//
  if(ECREFFKEY.reprtf[0]!=0){
    HF1(ECHISTC+12,geant(fabs(rid)),1.);
    HF1(ECHISTC+6,geant(chi2),1.);
    HF1(ECHISTC+35,geant(beta),1.);
    HF1(ECHISTC+36,geant(chargeTracker),1.);
  }
//
  if(ECCAFFKEY.truse==1){//select He4
    if(chargeTracker!=2)return;
  }
  else{//select Protons
    if(chargeTracker!=1)return;
  }
  EcalJobStat::addca(2);
//
  if(!(beta>0. && rid>0.))return;//---> it is not He4(prot)(charge>0) from AMS_top->bot
  if(chi2>20.)return;//---> bad chi2
  if(fabs(rid)<ECCAFFKEY.trmin)return;//---> too low rigidity(too high mult.scatt)
  if(beta<0.88)return;// ----> low beta
  EcalJobStat::addca(3);
//
  number dx,dy,sfg(0.5);
  int icr(0);
//
  C0[2]=ECALDBc::gendim(7);// Z-top of ECAL
  C0[0]=0.;
  C0[1]=0.;
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Ztop of EC
  if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC,geant(cos(the)),1.);
  if(fabs(cos(the))<0.94)return;// ----> check the impact angle(should be < 20degr)
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
  C0[2]=ECALDBc::gendim(7)-nslmx*(ECALDBc::gendim(9)+2.*ECALDBc::gendim(10));// Z-bot of ECAL
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Zbot of EC
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
  if(icr<=1)return;// ----> check crossing 
//
  EcalJobStat::addca(4);
//
//-------------------------> select Punch-Through(non-interecting) events :
//   (use EcalHits with Edep already gain-corrected(though using Previous(!) calibration)
//
  number edthr,edep,edlpr[2*ECSLMX],edtpr[2*ECPMSMX];
  integer nsclpr[2*ECSLMX];
  integer refsl,refpm,ipl,imax,imin;
  int badsc,badscl(0);
  geant ad2mv;
//
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  ad2mv=ECcalib::ecpmcal[refsl-1][refpm-1].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
//
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell-Planes loop
    ptr1=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);
    for(i=0;i<maxcl;i++){
      edtpr[i]=0.;
    }
    edlpr[ipl]=0.;
    nsclpr[ipl]=0;
    while(ptr1){ // <--- EcalHits(fired subcells) loop in plane:
      edep=ptr1->getedep();//(mev) is already gain-corrected using previous calibration data !!!
      proj=ptr1->getproj();//0/1->X/Y (have to be corresponding to ipl)
      cell=ptr1->getcell();// 0,...
      isl=ipl/2;
      pmt=cell/2;
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+24,geant(edep/ad2mv),1.);
      if(edep>=(ad2mv*ECCAFFKEY.adcmin)){// because adcmin-threshold is in ADC-units
        edlpr[ipl]+=edep;
	edtpr[cell]+=edep;
      }
      ptr1=ptr1->next();  
    } // ---> end of EcalHits loop in scPlane
    badsc=0;
    imin=-1;
    imax=999;
    for(i=0;i<maxcl;i++){// <--- exam. patt. of fired subcells in plane
      if(edtpr[i]>ECCAFFKEY.adcpmx*ad2mv)badsc+=1;//count sc with too high signal(interact.,ovfl,...)
      if(edtpr[i]>0.){
        nsclpr[ipl]+=1;//count fired SubCell's per plane
	imax=i;//to find most right fired sc
	if(imin<0)imin=i;//to find most left fired sc
      }
    }//---> end of SubCell loop in plane
    k=imax-imin;
//
    if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+25,geant(nsclpr[ipl]),1.);
    if(nsclpr[ipl]>2 || badsc>0 || k>1)badscl+=1;
//                        bad Plane(>2 sc(or 2 but separated) or at least one with high signal)
  }//---> end of scPlane-loop
//
// ----------> mark scPlanes with highest Edep:
//
  number *pntro[2*ECSLMX];
  number *pntrn[2*ECSLMX];
  int plmask[2*ECSLMX],ngd(0);
  number ampo,ampn,edept;
  for(ipl=0;ipl<maxpl;ipl++){
    plmask[ipl]=1;//mask of good Sclayers
    pntro[ipl]=&edlpr[ipl];
    pntrn[ipl]=&edlpr[ipl];
  }
  AMSsortNAG(pntrn,maxpl);//sort ScL-Energies in increasing order
  for(i=0;i<ECCAFFKEY.ntruncl;i++){// select ntruncl(2) scPlanes with highest Edep
    for(ipl=0;ipl<maxpl;ipl++){
      if(pntro[ipl]==pntrn[maxpl-1-i])plmask[ipl]=0;//mark scPlanes with highest Edep
    }
  }
  if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+17,geant(badscl),1.);
//
  if(ECCAFFKEY.truse==1){// He4
    if(badscl>ECCAFFKEY.badplmx)return;// <----limit on number of bad sc planes
  }
  else{// Prot
    if(badscl>ECCAFFKEY.badplmx+1)return;// <----limit on number of bad sc planes
  }
  EcalJobStat::addca(5);
// ----> check Etrunc (to be sure that have needed particle(p or He or ...):
  edept=0.;
  for(isl=0;isl<nslmx;isl++){
    if((edlpr[2*isl]*plmask[2*isl]>0) && (edlpr[2*isl+1]*plmask[2*isl+1]>0)){
      edept+=(edlpr[2*isl]+edlpr[2*isl+1]);
      ngd+=1;
      EcalJobStat::nprofac[isl]+=1.;
      EcalJobStat::zprofac[isl]+=(edlpr[2*isl]+edlpr[2*isl+1]);
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+28,geant(isl+1),1.);
    }
  }
  edept/=ngd;
  if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+16,geant(edept),1.);
  if(ECCAFFKEY.truse==1){//select He4
    if(edept>ECCAFFKEY.etrunmx || edept<ECCAFFKEY.etrunmn)return;//--->Etrunc->He4 ?
  }
  else{//select Protons
    if(edept>ECCAFFKEY.etrunmx/4 || edept<ECCAFFKEY.etrunmn/4)return;//--->Etrunc->prot ?
  }
  EcalJobStat::addca(6);
// 
//
// ==========================> find all crossed or fired/matched ECAL RawHits  in SL:
//
  number expm;//extention of PM transv.size for preselection of PM-Track crossings 
  number exsc;//extention of SubCell transv.size for search of Sub_Cell-Track crossings
  number zpass;//particle pass from ECAL Zfront to middle of SL
  number mscat;//mult.scatt.par.
  int sccr;
  mscat=pow((13.6/fabs(rid)/1000.),2)/3./1.04;// ~ SigmaMS**2 /dz**3 (use 1X0~1.04cm)
  mscat=mscat*pow(ECCAFFKEY.mscatp,2);// empirical fine tuning (increase of SigmaMS)
//
  for(isl=0;isl<ECALDBc::slstruc(3);isl++){ // <-------------- super-layer loop
    ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
               getheadC("AMSEcalRawEvent",isl,0);
    for(i=0;i<ECALDBc::slstruc(4);i++){
      for(k=0;k<4;k++){
        sumh[i][k]=0.;
        suml[i][k]=0.;
      }
    }
    if(plmask[2*isl]==0 || plmask[2*isl+1]==0)continue;//bad SL(with high Edep) --> try next
//
    nraw=0;
    while(ptr){ // <--- RawEvent-hits loop in superlayer:
      nraw+=1;
      id=ptr->getid();//SSPPC
      idd=id/10;
      sbc=id%10-1;//SubCell(0-3)
      pmt=idd%100-1;//PMCell(0-...)
      ptr->getpadc(padc);
      radc[0]=number(padc[0]);//ADC-high-chain
      radc[1]=number(padc[1]);//ADC-low-chain
      sumh[pmt][sbc]+=radc[0];
      suml[pmt][sbc]+=radc[1];
      ptr=ptr->next();  
    } // ---> end of RawEvent-hits loop in superlayer
    if(nraw<1)continue;// 0 multiplicity in S-Layer isl -> take next SL
//
// ---> extrapolate track to S-Layer "isl" :
//
    ECALDBc::getscinfoa(isl,0,0,proj,plane,cell,ct1,cl1,z1);//1stPM,1stPixel
    ECALDBc::getscinfoa(isl,0,2,proj,plane,cell,ct2,cl2,z2);//1stPM,3rdPixel
    C0[2]=(z1+z2)/2.+ECALDBc::rdcell(5);// Z-top of PM(LG) window(no gap betw.LG-1/4parts?)
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<---cross with SL at Z=PMtop
    if(proj==0)trc[0]=Cout1[0];//x-proj(0)
    else trc[0]=Cout1[1];//y-proj(1)
    if(ECREFFKEY.reprtf[0]!=0 && isl==0){
      HF1(ECHISTC+1,geant(Cout1[0]),1.);
      HF1(ECHISTC+2,geant(Cout1[1]),1.);
    }
    C0[2]=(z1+z2)/2.-ECALDBc::rdcell(5);// Z-bot of PM(LG) window(no gap betw.LG-1/4parts?)
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout2,the,phi,trl);//<---cross with SL at Z=PMbottom
    if(proj==0)trc[2]=Cout2[0];//x-proj(0)
    else trc[2]=Cout2[1];//y-proj(1)
    trc[1]=(trc[0]+trc[2])/2.;//trc[0->2] are transv.cross.coo with PM at Z-top/mid/bot
    k=0;
    if(trc[0]>trc[2]){//arrange in increasing order
      rrr=trc[0];
      trc[0]=trc[2];
      trc[2]=rrr;
      k=1;
    }
//
// ---> check track-SL longit. matching (transv.matching with PM/cell done below ):
//
    Cpmt=(Cout1+Cout2)/2.;//cross.coo at middle of SL(PM)
//    if(isl==0){//<--- check TRK imp.point accuracy(MC only)
//      HF1(ECHISTC+33,geant(Cpmt[0]-AMSEcalMCHit::impoint[0]),1.);
//      HF1(ECHISTC+34,geant(Cpmt[1]-AMSEcalMCHit::impoint[1]),1.);
//    }
    zpass=ECALDBc::gendim(7)-Cpmt[2];//dist.from ECAL Zfront till SL-middle
    if(proj==0){// <--- x-proj (0)
      ctcr=Cpmt[0];
      clcr=Cpmt[1];
      rdir=(1-2*(pmt%2))*ECALDBc::slstruc(5);//readout dir(+-1->along pos/neg Y)
      hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir
      clsh=ECALDBc::gendim(6);//EC-pos in y
      acorr=1./sqrt(1.+pow(sin(the)*sin(phi)/cos(the),2));
      exsc=sqrt(pow(ECCAFFKEY.trxac,2)+mscat*pow(zpass,3));//imp.point accuracy vs SL
      exsc=ECCAFFKEY.nsigtrk*(ECCAFFKEY.trxac+ECCAFFKEY.tryac)-exsc;//N-Sig extent. to SC transv.size
    }
    else{//        <--- y-proj (1)
      ctcr=Cpmt[1];
      clcr=Cpmt[0];
      rdir=(1-2*(pmt%2))*ECALDBc::slstruc(6);//readout dir(+-1->along pos/neg X)
      hflen=ECALDBc::gendim(1)/2.;// 0.5*fiber(EC) length in X-dir
      clsh=ECALDBc::gendim(5);//EC-pos in x
      acorr=1./sqrt(1.+pow(sin(the)*cos(phi)/cos(the),2));
      exsc=sqrt(pow(ECCAFFKEY.tryac,2)+mscat*pow(zpass,3));
      exsc=ECCAFFKEY.nsigtrk*(ECCAFFKEY.trxac+ECCAFFKEY.tryac)-exsc;
    }
    if(fabs(clcr-clsh)>hflen)continue;//track out of SL ===> try next SL
    clpm=(cl1+cl2)/2.;// middle of fiber in AMS r.s.!
    if(rdir>0){
      clpm=clpm-hflen;//pm-pos if pm is at -x(-y) (i.e. rdir=1)
      pmdis=clcr-clpm;//0-2*hflen
    }
    else{
      clpm=clpm+hflen;//pm-pos if pm is at +x(+y) (i.e. rdir=-1)
      pmdis=clpm-clcr;//0-2*hflen
    }
    pmdis=fabs(clcr-clpm);//cr.point dist. from PM (0-2*hflen)
    binw=2.*hflen/ECCLBMX;
    lbin=integer(floor(pmdis/binw));//longit.crossing bin
    expm=2.*exsc; 
//
    for(pmt=0;pmt<npmx;pmt++){ // <======== loop over PM's to find crossed and fired cells
//
      pmsl=pmt+ECPMSMX*isl;//sequential numbering of PM's over all superlayers
      pmb[1]=-(npmx-1)*pmpit/2.+pmt*pmpit;//transv.coo for middle of PM
      pmb[0]=pmb[1]-pxsiz;//transv.coo for left edge of PM
      pmb[2]=pmb[1]+pxsiz;//transv.coo for right edge of PM
      sbl=pmb[0];
      sbr=pmb[2];
      sbl-=expm;//increase PM-transv.size for safe PM-Track crossing check
      sbr+=expm;
//-------> PM-cross. pre-check (to speed up SubCell-crossing search):
      if(sbr<trc[0] || sbl>trc[2]){
      }//                                   <--- No crossing with PM
//
      else{//          <--------- Found Pre-Crossing with PM, now check SubCells
//
        apmt=0.;
	sccr=0;
        for(sbc=0;sbc<4;sbc++){// <-------- sub-cell loop
          if(sbc%2==0){//<-left column of subcells
	    sbl=pmb[0];//subcell left/right coo
	    sbr=pmb[1];
	  }
	  else{//        <-right column of subcells
	    sbl=pmb[1];
	    sbr=pmb[2];
	  }
//
	  if(sbc<=1){//<---top pair of subcells
	    if(k==0){
	      crl=trc[0];//crossing left/right coo
	      crr=trc[1];
	    }
	    else{
	      crl=trc[1];
	      crr=trc[2];
	    }
	  }
	  else{//      <---bot pair of subcells
	    if(k==0){
	      crl=trc[1];//crossing left/right coo
	      crr=trc[2];
	    }
	    else{
	      crl=trc[0];
	      crr=trc[1];
	    }
	  }
          sbl-=exsc;//increase SC-transv.size for TRK_accuracy+MulScat compensation !!!
          sbr+=exsc;
	  if(sbr<crl || sbl>crr){//  <--- No crossing with subcell
	  }
	  else{//                 <------------ Found crossing with subcell
	    sccr+=1;
	    tevsbc[pmsl][sbc]+=1.;//count crossed subcells
            ECPMPeds::pmpeds[isl][pmt].getpedh(pedh);
            ECPMPeds::pmpeds[isl][pmt].getsigh(sigh);
            ECPMPeds::pmpeds[isl][pmt].getpedl(pedl);
            ECPMPeds::pmpeds[isl][pmt].getsigl(sigl);
            ph=pedh[sbc];
            sh=sigh[sbc];
            pl=pedl[sbc];
            sl=sigl[sbc];
	    radc[0]=sumh[pmt][sbc];// hi-ch
	    radc[1]=suml[pmt][sbc];// low-ch
            ovfl[0]=0;
            ovfl[1]=0;
            if(radc[0]>0.)
	               if((ECADCMX[0]-(radc[0]+ph))<=4.*sh)ovfl[0]=1;// mark as ADC-Overflow
            if(radc[0]>ECCAFFKEY.adcmin && ovfl[0]==0){//<=== fired pixel(hi-channel)
	      sta=0;
	      acorr=1.;
	      radc[0]*=acorr;//norm. to norm.incidence(for fiber long.dir only !!)
	      apmt+=radc[0];
	      ECREUNcalib::fill_1(isl,pmt,sbc,lbin,radc[0]);//<--- fill arrays(PM/SC gain calibr)
            }//---> endif of fired subcell
//
          }//---> endif of found subcell crossing
        }//---> end of subcell loop
//
	if(sccr>0){//at least 1 crossed SC, i.e. PM pre-crossing confirmed !
          tevpmc[pmsl]+=1.;//count crossed PM's
	  if(ECCAFFKEY.nortyp==0){// <-- if want normalize by crossings
            if(lbin>=(ECCLBMX/2-ECLBMID) && lbin<=(ECCLBMX/2+ECLBMID-1))
	                  tevpmm[pmsl]+=1.;//crossed at EC-center(+-ECLBMID bins from center)
	  }
	  if(ECREFFKEY.reprtf[0]!=0){
	    if(isl==0 && pmt==17)HF1(ECHISTC+5,geant(pmdis),1.);
	  }
	}
	if(apmt>0.){
	  tevpmf[pmsl]+=1.;//count fired PM's
	}
        if(apmt>0. && ECREFFKEY.reprtf[0]!=0){
	  if(proj==0)HF1(ECHISTC+29,apmt,1.);
	  else HF1(ECHISTC+30,apmt,1.);
	}
      }//---> endif of found PM crossing 
//
    } // ---> end of PM-loop loop in S-layer
//
  } // ---> end of super-layer loop
//
  if(sta)return;
  EcalJobStat::addca(7);
}
//-----------------------------------------------------------
void ECREUNcalib::makeToyRLGAfile(){
  int i,j,k,bad;
  integer sl,pm,sc,pmsl,lb,npmx,slpmc;
  integer pmslr,refsl,refpm;
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
  npmx=ECALDBc::slstruc(4);//numb.of PM's/sl
  char fname[80];
  char frdate[30];
  char vers1[3]="mc";
  char vers2[3]="sd";
  integer cfvn;
  int dig;
  geant pmrg,pxrg[4],hi2lr,a2dr;
//
//--> create RLGA-type 2 files(PM/SubCell rel.gains,Hi2Low-ratios,An2Dyn-ratios):
//    ...sd.cof simulate copy of RealData RLGA-calib.file used as MCSeed
//    ...mc.cof simulate RLGA-calibration OUTPUT-file using seeds from ...sd.cof file
//                                i.e. simulating calibration procedure accuracies !!!
// 
  integer endfm(12345);
  strcpy(inum,"0123456789");
  cfvn=ECCAFFKEY.cfvers%1000;
  strcpy(fname,"ecalrlga");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  strcat(fname,vers2);//sd = copy of rl
  strcat(fname,".cof");
//---
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"ECREUNcalib::makeToyRLGAfile:Error opening ToyRLGA-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"Open file for ToyRLGA-calibration output(MC-Seeds) "<<fname<<endl;
//---
    cout<<"Pixels/Dynode status will be written(MC-Seeds) !"<<endl;
    tcfile.setf(ios::fixed);
    tcfile << endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile.width(3);
        tcfile.precision(2);// precision for status
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile <<0<<" ";//all pixels are good
        }
        tcfile << endl;
      }
      tcfile.width(3);
      tcfile.precision(2);// precision for status
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile <<0<<" ";//all dynodes are good
      }
      tcfile << endl;
      tcfile << endl;
    }
    tcfile << endl;
    cout<<"...done!"<<endl;
//---
    cout<<"PM RelGains will be written(MC-Seeds) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile.width(5);
      tcfile.precision(2);// precision for PM Rel.gain
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	pmrg=1.+0.2*rnormx();
	if(pmrg>2)pmrg=2;
	if(pmrg<0.3)pmrg=0.3;
	pmrgain[pmsl]=pmrg;
        tcfile <<pmrgain[pmsl]<<" ";
      }
      tcfile << endl;
    }
    tcfile << endl;
    tcfile << endl;
    cout<<"...done!"<<endl;
//
//--- create pixel rel.gains:
    geant spxrg;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering
        spxrg=0;
        for(sc=0;sc<4;sc++){
	  pxrg[sc]=1.+0.05*rnormx();//5%
	  spxrg+=pxrg[sc];
	}
        for(sc=0;sc<4;sc++)pxrgain[pmsl][sc]=4*pxrg[sc]/spxrg;//normalise
      }
    }
//---    
    cout<<"Pixel RelGains will be written(MC-Seeds) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile.width(5);
        tcfile.precision(2);// precision for Pixel rel.gain
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile <<pxrgain[pmsl][sc]<<" ";
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
    cout<<"...done!"<<endl;
//---
    cout<<"Pixel Hi/Low Gain ratios will be written(MC-Seeds) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile.width(4);
        tcfile.precision(1);// precision for Hi/Low ratio
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	  hi2lr=36*(1.+0.05*rnormx());//5%
	  hi2lowr[pmsl][sc]=hi2lr;
          tcfile <<hi2lowr[pmsl][sc]<<" ";
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
    cout<<"...done!"<<endl;
//
    cout<<"PM Anode/Dynode ratios will be written !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile.width(4);
      tcfile.precision(1);// precision for PM Anode/Dynode ratio
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	a2dr=25*(1.+0.05*rnormx());//5%
	an2dynr[pmsl]=a2dr;
        tcfile <<an2dynr[pmsl]<<" ";
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    tcfile <<" "<<endfm<<endl;
    tcfile << endl;
//
    cout<<"...done!"<<endl;
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" Toy MCSeeds RLGA-file  is done !"<<endl;
    tcfile.close();
    cout<<"ECREUNcalib: Toy MCSeeds RLGA-file closed !"<<endl;
//------------------------
// now simulate RLGA-calibr.procedure and create ...mc.cof file:
//
  strcpy(fname,"ecalrlga");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  strcat(fname,vers1);//mc
  strcat(fname,".cof");
//
    ofstream tcfile1(fname,ios::out|ios::trunc);
    if(!tcfile1){
      cerr<<"ECREUNcalib::makeToyRLGAfile:error opening RLGA-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"Open file for ToyRLGA-calibration output(MC-calib) "<<fname<<endl;
//---
    cout<<"Pixels/Dynodes status will be written(MC-calib) !"<<endl;
    tcfile1.setf(ios::fixed);
    tcfile1 << endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile1.width(3);
        tcfile1.precision(2);// precision for status
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile1 <<0<<" ";//all are good
        }
        tcfile1 << endl;
      }
      tcfile1.width(3);
      tcfile1.precision(2);// precision for status
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile1 <<0<<" ";//all dynodes are good
      }
      tcfile1 << endl;
      tcfile1 << endl;
    }
    tcfile1 << endl;
    cout<<"...done!"<<endl;
//---
    cout<<"PM RelGains will be written(MC-calib) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile1.width(5);
      tcfile1.precision(2);// precision for PM Rel.gain
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile1 <<pmrgain[pmsl]*(1.+0.02*rnormx())<<" ";//2% calib.accuracy
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
    tcfile1 << endl;
    cout<<"...done!"<<endl;
//
//--- create pixel rel.gains:
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering
	spxrg=0;
        for(sc=0;sc<4;sc++){
	  pxrg[sc]=pxrgain[pmsl][sc]*(1.+0.02*rnormx());//2% calib.accuracy
	  spxrg+=pxrg[sc];
	}
	for(sc=0;sc<4;sc++)pxrgain[pmsl][sc]=4*pxrg[sc]/spxrg;//normalise
      }
    }
//---    
    cout<<"Pixel RelGains will be written(MC-calib) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile1.width(5);
        tcfile1.precision(2);// precision for Pixel rel.gain
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile1 <<pxrgain[pmsl][sc]<<" ";
        }
        tcfile1 << endl;
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
    cout<<"...done!"<<endl;
//
    cout<<"Pixel Hi/Low Gain ratios will be written(MC-calib) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile1.width(4);
        tcfile1.precision(1);// precision for Hi/Low ratio
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile1 <<hi2lowr[pmsl][sc]*(1.+0.01*rnormx())<<" ";//1% calib.accuracy
        }
        tcfile1 << endl;
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
//
    cout<<"PM Anode/Dynode ratios will be written(MC-calib) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile1.width(4);
      tcfile1.precision(1);// precision for PM Anode/Dynode ratio
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile1 <<an2dynr[pmsl]*(1.+0.01*rnormx())<<" ";//1% calib.accuracy
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
//
    tcfile1 <<" "<<endfm<<endl;
    tcfile1 << endl;
//
    cout<<"...done!"<<endl;
    tcfile1 << endl<<"======================================================"<<endl;
    tcfile1 << endl<<" Toy RLGA-MC-calib file is done !"<<endl;
    tcfile1.close();
    cout<<"ECREUNcalib: Toy RLGA-MC-calib file closed !"<<endl;
}
//----------------------------------------------------------------------------
void ECREUNcalib::mfit(){
//
  int i,j,k,bad;
  integer sl,pm,sc,pmsl,lb,npmx,slpmc;
  integer pmslr,refsl,refpm;
  geant arr[ECPMSMX*2];
  geant glscscan[2*ECSLMX];
  int nlscan,nof4;
  number rrr,sum4,eff,hflen,binw;
  char htit1[60];
  char htit2[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
  npmx=ECALDBc::slstruc(4);//numb.of PM's/sl
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  pmslr=refpm-1+ECPMSMX*(refsl-1);//sequential numbering for ref. PM
  hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir(=X)
  binw=2.*hflen/ECCLBMX;
//
//---------> print hist. of event multiplicity/eff in cells:
//
  if(ECREFFKEY.reprtf[0]>1){
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
      arr[2*pm]=tevsbc[pmsl][0];
      arr[2*pm+1]=tevsbc[pmsl][1];
    }
    strcpy(htit1,"Crossings, top cells of SL ");
    in[0]=inum[sl+1];
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;
      arr[2*pm]=tevsbc[pmsl][2];
      arr[2*pm+1]=tevsbc[pmsl][3];
    }
    strcpy(htit1,"Crossings, bot cells of SL ");
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
  }
// now SC-efficiencies:
//  
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
      arr[2*pm]=0.;
      arr[2*pm+1]=0.;
      if(tevsbc[pmsl][0]>0)arr[2*pm]=tevsbf[pmsl][0]/tevsbc[pmsl][0];
      if(tevsbc[pmsl][1]>0)arr[2*pm+1]=tevsbf[pmsl][1]/tevsbc[pmsl][1];
    }
    strcpy(htit1,"Efficiency, top cells of SL ");
    in[0]=inum[sl+1];
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    if(ECCAFFKEY.truse==1){//He4
      HMINIM(ECHISTC+7,0.6);
      HMAXIM(ECHISTC+7,1.4);
    }
    else{//prot
      HMINIM(ECHISTC+7,0.3);
      HMAXIM(ECHISTC+7,0.9);
    }
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
//
    nlscan=0;
    glscscan[2*sl]=0.;
    for(i=0;i<2*npmx;i++){
      if(arr[i]>0.){
        glscscan[2*sl]+=arr[i];
	nlscan+=1;
      }
    }
    if(nlscan>0)glscscan[2*sl]/=geant(nlscan);
//
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;
      arr[2*pm]=0.;
      arr[2*pm+1]=0.;
      if(tevsbc[pmsl][2]>0)arr[2*pm]=tevsbf[pmsl][2]/tevsbc[pmsl][2];
      if(tevsbc[pmsl][3]>0)arr[2*pm+1]=tevsbf[pmsl][3]/tevsbc[pmsl][3];
    }
    strcpy(htit1,"Efficiency, bot cells of SL ");
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    if(ECCAFFKEY.truse==1){//He4
      HMINIM(ECHISTC+7,0.6);
      HMAXIM(ECHISTC+7,1.4);
    }
    else{//prot
      HMINIM(ECHISTC+7,0.3);
      HMAXIM(ECHISTC+7,0.9);
    }
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
//
    nlscan=0;
    glscscan[2*sl+1]=0.;
    for(i=0;i<2*npmx;i++){
      if(arr[i]>0.){
        glscscan[2*sl+1]+=arr[i];
	nlscan+=1;
      }
    }
    if(nlscan>0)glscscan[2*sl+1]/=geant(nlscan);
//
  }//---> end of sl loop
  HPAK(ECHISTC+14,glscscan);
  }//---> endif of printing
//
//------------> look at SubCell/PM's efficiency in projections:
//
  geant pmeflsc[ECSLMX];
  if(ECREFFKEY.reprtf[0]>0){
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      k=0;
      pmeflsc[sl]=0.;
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        for(i=0;i<4;i++){
	  eff=0.;
          if(tevsbc[pmsl][i]>0.)eff=tevsbf[pmsl][i]/tevsbc[pmsl][i];
          if(sl%2==0)HF1(ECHISTC+26,geant(eff),1.);
          if(sl%2==1)HF1(ECHISTC+27,geant(eff),1.);
	}
	eff=0.;
	if(tevpmc[pmsl]>0.)eff=tevpmf[pmsl]/tevpmc[pmsl];
        if(sl%2==0)HF1(ECHISTC+31,geant(eff),1.);
        if(sl%2==1)HF1(ECHISTC+32,geant(eff),1.);
	if(eff>0.){
	  k+=1;
	  pmeflsc[sl]+=eff;
	}
      }//---> pm loop
      if(k>0)pmeflsc[sl]/=geant(k);
    }//---> sl loop
    HPAK(ECHISTC+15,pmeflsc);
  }
//
//
// ------------> check that ref.pm OK (pm & sc  eff. are reasonable):
//
  cout<<"ECREUNcalib:: RefPM: SL="<<refsl<<" PM="<<refpm<<" Crossings :"<<tevpmc[pmslr]<<endl;
  cout<<"Crossings in 4-subc: "<<tevsbc[pmslr][0]<<" "<<tevsbc[pmslr][1]                    
                            <<" "<<tevsbc[pmslr][2]<<" "<<tevsbc[pmslr][3]<<endl;
  bad=0;
  if(tevpmc[pmslr]>5.){
    for(i=0;i<4;i++){
      if(tevsbc[pmslr][i]<15.)bad=1;//too low cr.statistics/subc
      else{
        eff=tevsbf[pmslr][i]/tevsbc[pmslr][i];
	if(eff<0.3)bad=1;//too low eff
      }
    }
  }
  else bad=1;
  if(bad){
    cout<<"ECREUNcalib: Problem with reference PM, no calibration done..."<<endl;
    return;
  }
  cout<<endl<<endl;
//
// --------------> Calc. SubCell(pixel) relative gains:
//
  for(i=0;i<ECPMSL;i++){// <--- PM*SL loop
    pm=i%ECPMSMX+1;
    sl=integer(floor(number(i)/ECPMSMX))+1;
    sum4=0.;
    nof4=0;
    for(j=0;j<4;j++){// <--- 4 subcell loop
      eff=0;
      if(tevsbc[i][j]>25){
        eff=tevsbf[i][j]/tevsbc[i][j];
	sbcres[i][j]/=tevsbc[i][j];// aver.signal(using "crossed" counter)
//	sbcres[i][j]/=tevsbf[i][j];// aver.signal(using "fired" counter)
      } 
      if(eff<0.3){
        if(eff>0.1)pxstat[i][j]=10;//limited quality subc(low eff)
        else pxstat[i][j]=90;//unusable subc(very low eff or statistics)
      }
      if(pxstat[i][j]==90)
          cout<<"ECREUNcalib:unusable SubCell!!,sl="<<sl<<" pm="<<pm<<" sc="<<j<<endl;
      if(pxstat[i][j]>=0){
        sum4+=sbcres[i][j];
	nof4+=1;
      }
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+8,geant(eff),1.);
    }// ---> end of 4sc loop 
//
    for(j=0;j<4;j++){
      if(sum4>0 && pxstat[i][j]>=0)pxrgain[i][j]=nof4*sbcres[i][j]/sum4;//"nof4" to have pxrg~1
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+9,geant(pxrgain[i][j]),1.);
    }
  }// ---> end of PM*SL loop
  cout<<endl<<endl;
//
//--------------> calc./FIT average resp. in each long.bin:
//
  geant pmprof[ECCLBMX],pmprer[ECCLBMX],pmres[ECPMSL],pmcrs[ECPMSL];
  int first(1); 
  int ier;  
  int ifit[4];
  char pnam[4][6],pnm[6];
  number argl[10];
  int iargl[10];
  number start[4],pstep[4],plow[4],phigh[4];
  strcpy(pnam[0],"anorm");
  strcpy(pnam[1],"lenhi");
  strcpy(pnam[2],"admix");
  strcpy(pnam[3],"lenlo");
  start[0]=50.;//anorm
  start[1]=368.;//lenhi
  start[2]=0.15;//admix
  start[3]=32.5;//lenlow
  pstep[0]=5.;
  pstep[1]=50.;
  pstep[2]=0.05;
  pstep[3]=5.;
  plow[0]=1.;
  plow[1]=10.;
  plow[2]=0.;
  plow[3]=5.;
  phigh[0]=1000.;
  phigh[1]=1000.;
  phigh[2]=1.;
  phigh[3]=100.;
  for(i=0;i<4;i++)ifit[i]=1;//release all par's by default
  ifit[2]=0;//fix admix
//  ifit[3]=0;//fix lenlow
  slslow=0.;
  slfast=0.;
  fastfr=0.;
  nfits=0;
  cout<<"Start Init. parameters..."<<endl;
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("ECAL Longit.Resp.Uniformity-calibration");
  argl[0]=number(-1);
  MNEXCM(mfun,"SET PRINT",argl,1,ier,0);
  for(i=0;i<4;i++){
    strcpy(pnm,pnam[i]);
    ier=0;
    MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
    if(ier!=0){
      cout<<"ECREUN-calib: Param-init problem for par-id="<<pnam[i]<<'\n';
      exit(10);
    }
    argl[0]=number(i+1);
    if(ifit[i]==0){
      ier=0;
      MNEXCM(mfun,"FIX",argl,1,ier,0);
      if(ier!=0){
        cout<<"ECREUN-calib: Param-fix problem for par-id="<<pnam[i]<<'\n';
        exit(10);
      }
    }
  }
  cout<<"... init done, start fit ..."<<endl;
// ---> calc. profiles and fitting for monitored pm's:
//
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){ // <---- SL-loop
    for(pm=0;pm<npmx;pm++){ // <---- PM-loop
      pmsl=pm+ECPMSMX*sl;
      pmres[pmsl]=0.;
      pmcrs[pmsl]=tevpmm[pmsl];
      nbins=0;
      for(lb=0;lb<ECCLBMX;lb++){ // <---- Bin-loop (lb)
        if(lb>=(ECCLBMX/2-ECLBMID) && lb<=(ECCLBMX/2+ECLBMID-1)){
          pmres[pmsl]+=pmlres[pmsl][lb];//sum PM-resp. for +-ECLBMID central bins(PM RelGain Calib)
	}
	if(pmsl==pmslr)cout<<"lb="<<lb<<" ev="<<tevpml[pmsl][lb]<<endl;
        if(tevpml[pmsl][lb]>15.){
	  pmlres[pmsl][lb]/=tevpml[pmsl][lb];//bin-average
	  pmlres2[pmsl][lb]/=tevpml[pmsl][lb];
	  rrr=pmlres2[pmsl][lb]-pmlres[pmsl][lb]*pmlres[pmsl][lb];//rms**2
	  if(pmsl==pmslr)cout<<"aver="<<pmlres[pmsl][lb]<<" rms2="<<rrr<<" av2="<<pmlres2[pmsl][lb]<<endl;
	  if(rrr>=0)
	        pmlres2[pmsl][lb]=sqrt(rrr/tevpml[pmsl][lb]);//err.of aver.
	  else {
	    pmlres[pmsl][lb]=0;
	    pmlres2[pmsl][lb]=0;
	  }
        }
	else{
	  pmlres[pmsl][lb]=0;
	  pmlres2[pmsl][lb]=0;
	}
	pmprof[lb]=pmlres[pmsl][lb];// = or > 0
	pmprer[lb]=pmlres2[pmsl][lb];
	if(pmprof[lb]>0){//non-zero bin -> fill work-arrays for fit
	  values[nbins]=pmprof[lb];
	  errors[nbins]=pmprer[lb];
	  coords[nbins]=binw/2.+lb*binw;
	  nbins+=1;
	}
      }//--->end of lb loop
//      
      if(sl==(refsl-1) && pm==(refpm-1)){
        if(ECREFFKEY.reprtf[0]!=0){
	  HPAK(ECHISTC+10,pmprof);
          HPAKE(ECHISTC+10,pmprer);
          HPRINT(ECHISTC+10);
	}
      }
      if((pm==1 || pm==17 || pm==(npmx-2))){// <---- fit for monitored PM's
        if(nbins>(ECCLBMX-2)){//<-- do fit if have enough bins
          cout<<"Start uniformity-fit for sl/pm="<<sl<<" "<<pm<<" nbins="<<nbins<<endl;
          i=0;//<--- reinit for param #1 with realistic value from 1st bin
	  start[i]=values[0];
          strcpy(pnm,pnam[i]);
          ier=0;
          argl[0]=number(0);
          MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
          if(ier!=0){
            cout<<"ECREUN-calib: Param-ReInit problem for param="<<pnam[i]<<'\n';
            exit(10);
          }
          argl[0]=0.;
          ier=0;
          MNEXCM(mfun,"MINIMIZE",argl,0,ier,0);
          if(ier!=0){
            cout<<"ECREUN-calib: MINIMIZE problem !"<<'\n';
            continue;
          }  
          MNEXCM(mfun,"MINOS",argl,0,ier,0);
          if(ier!=0){
            cout<<"ECREUN-calib: MINOS problem !"<<'\n';
            continue;
          }
          argl[0]=number(3);
          ier=0;
          MNEXCM(mfun,"CALL FCN",argl,1,ier,0);
          if(ier!=0){
            cout<<"ECREUN-calib: final CALL_FCN problem !"<<'\n';
            continue;
          }
          cout<<"fit OK"<<endl<<endl;
        }//--> end of fit
	else{
	  cout<<"SL/PM="<<sl<<" "<<pm<<" not enought lbins to fit!, nbins="<<nbins<<endl; 
	}
      }//--> endif of monitored pm
    }//---> end of pm loop
  }//--->end of sl loop
  cout<<endl<<endl;
  cout<<"REUN(FIAT)-calibration: nfits="<<nfits<<endl;
  if(nfits>0){
    slslow/=number(nfits);
    slfast/=number(nfits);
    fastfr/=number(nfits);
    cout<<" slow/fast/frac="<<slslow<<" "<<slfast<<" "<<fastfr<<endl;
  }
//
// ----------------> calc. PM relative gains (wrt ref.PM):
//
  geant avr;
  if(pmcrs[pmslr]>25){
    avr=pmres[pmslr]/pmcrs[pmslr];//ref.PM ok
    cout<<"PM-rel-gain calibr: ref.PM events/averresp="<<pmcrs[pmslr]<<" "<<avr<<endl;
  }
  else{
    cout<<"No PM-rel-gain calibr. done(PM-ref low stat) "<<pmcrs[pmslr]<<endl;
    return; 
  }
  geant glscan[ECSLMX],gtscan[ECPMSMX];
  number totr1(0),totr2(0),tote1(0),tote2(0);
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){
    glscan[sl]=0;
    nlscan=0;
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;
      if(pmcrs[pmsl]>0){
        if(sl==0){
	  totr1+=pmres[pmsl];
	  tote1+=pmcrs[pmsl];
	}
        if(sl==1){
	  totr2+=pmres[pmsl];
	  tote2+=pmcrs[pmsl];
	}
        pmres[pmsl]/=pmcrs[pmsl];
        pmrgain[pmsl]=pmres[pmsl]/avr;
        nlscan+=1;
	glscan[sl]+=geant(pmrgain[pmsl]);
        if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+11,geant(pmrgain[pmsl]),1.);
      }
    }//---> end of pm loop
    if(nlscan>0)glscan[sl]/=geant(nlscan);
  }
  if(ECREFFKEY.reprtf[0]!=0)HPAK(ECHISTC+13,glscan);
  cout<<"totr1/totr2="<<totr1<<" "<<totr2<<" tote1/2="<<tote1<<" "<<tote2<<endl;
//
// ---------> print PM rel.gains:
//
  if(ECREFFKEY.reprtf[0]>0){
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;
        gtscan[pm]=pmrgain[pmsl];
      }
      strcpy(htit1,"PM RelGains in SL ");
      in[0]=inum[sl+1];
      strcat(htit1,in);
      HBOOK1(ECHISTC+7,htit1,npmx,1.,geant(npmx+1),0.);
      HMINIM(ECHISTC+7,0.6);
      HMAXIM(ECHISTC+7,1.4);
      HPAK(ECHISTC+7,gtscan);
      HPRINT(ECHISTC+7);
      HDELET(ECHISTC+7);
    }
//
// ---------> print SC rel.gains:
//
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;
        arr[2*pm]=0.;
        arr[2*pm+1]=0.;
        if(pxstat[pmsl][0]>=0)arr[2*pm]=pxrgain[pmsl][0];
        if(pxstat[pmsl][1]>=0)arr[2*pm+1]=pxrgain[pmsl][1];
      }
      strcpy(htit1,"RelGains of top cells in SL ");
      in[0]=inum[sl+1];
      strcat(htit1,in);
      HBOOK1(ECHISTC+7,htit1,npmx*2,1.,geant(2*npmx+1),0.);
      HMINIM(ECHISTC+7,0.6);
      HMAXIM(ECHISTC+7,1.4);
      HPAK(ECHISTC+7,arr);
      HPRINT(ECHISTC+7);
      HDELET(ECHISTC+7);
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;
        arr[2*pm]=0.;
        arr[2*pm+1]=0.;
        if(pxstat[pmsl][2]>=0)arr[2*pm]=pxrgain[pmsl][2];
        if(pxstat[pmsl][3]>=0)arr[2*pm+1]=pxrgain[pmsl][3];
      }
      strcpy(htit1,"RelGains of bot cells in SL ");
      strcat(htit1,in);
      HBOOK1(ECHISTC+7,htit1,npmx*2,1.,geant(2*npmx+1),0.);
      HMINIM(ECHISTC+7,0.6);
      HMAXIM(ECHISTC+7,1.4);
      HPAK(ECHISTC+7,arr);
      HPRINT(ECHISTC+7);
      HDELET(ECHISTC+7);
    }
  }//---> endif of printing
//
// ----------------> calc. hi2low ratios for each subcell:
//
  number t0,slo,t,tq,co,dis,nevf,bins,avs,avo;
  number sumc,sumt,sumct,sumc2,sumt2,sumid,nonzer;
  number chi2[ECPMSL*4],slop[ECPMSL*4],offs[ECPMSL*4];
  integer hchok[ECPMSL*4],ibinw;
//
  ibinw=integer(floor(number(ECCADCR)/ECCHBMX));
  avs=0.;
  avo=0.;
  nonzer=0.;
  for(i=0;i<ECPMSL*4;i++){ // <-- SubCell-chan. loop
    sumc=0.;
    sumt=0.;
    sumct=0.;
    sumc2=0.;
    sumt2=0.;
    sumid=0.;
    bins=0.;
    t0=0.;
    slo=0.;
    slop[i]=0.;
    offs[i]=0.;
    chi2[i]=0.;
    hchok[i]=0;
    for(j=0;j<ECCHBMX;j++){//<-- HchADC bin loop
      nevf=number(tevhlc[i][j]);
      co=(number(j)+0.5)*ibinw;// mid. of High-chan. bin
      if(nevf>5.){ // min. 5 events
        t=sbchlc[i][j]/nevf; // get mean
        tq=sbchlc2[i][j]/nevf; // mean square
        dis=tq-t*t;// rms**2
        if(dis>0.){
          dis/=(nevf-1.);// rms**2 of the mean
          bins+=1.;
          sumc+=(co/dis);
          sumt+=(t/dis);
          sumid+=(1./dis);
          sumct+=(co*t/dis);
          sumc2+=(co*co/dis);
          sumt2+=(t*t/dis);
          if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+23,geant(sqrt(dis)/t),1.);
        }
      }
    }//--> end of bin loop
//
    if(bins>=4){
      t0=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      slo=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      chi2[i]=sumt2+t0*t0*sumid+slo*slo*sumc2
        -2.*t0*sumt-2.*slo*sumct+2.*t0*slo*sumc;
      chi2[i]/=(bins-2.);
      slop[i]=1./slo;// goto "hi vs low" slope
      offs[i]=t0;
      nonzer+=1.;
      avs+=slop[i];
      avo+=offs[i];
      hchok[i]=1;
      if(ECREFFKEY.reprtf[0]!=0){
        HF1(ECHISTC+20,geant(slop[i]),1.);
        HF1(ECHISTC+21,geant(offs[i]),1.);
        HF1(ECHISTC+22,geant(chi2[i]),1.);
      }
    }
  }//--> end of chan. loop
//
  if(nonzer>0){
    avs/=nonzer;
    avo/=nonzer;
  }
  for(i=0;i<ECPMSL*4;i++){ // <-- SubCell-chan. loop
    pmsl=i/4;
    sc=i%4;
    if(hchok[i]==1){
      if(chi2[i]>0 && chi2[i]<10){
        hi2lowr[pmsl][sc]=slop[i];
      } 
      else if(chi2[i]>10){
        hi2lowr[pmsl][sc]=slop[i];
        pxstat[pmsl][sc]+=1;//suspicious LchSubCel
      }
      else{
        hi2lowr[pmsl][sc]=avs;
        pxstat[pmsl][sc]+=9;//bad LchSubCel
      }
    }
  }
//------------> write outp.files :
//  
  char fname[80];
  char frdate[30];
  char vers1[3]="mc";
  char vers2[3]="rl";
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
//
//--> create RLGA output file(PM/SubCell rel.gains,Hi2Low-ratios,An2Dyn-ratios):
// 
  integer endfm(12345);
  strcpy(inum,"0123456789");
  cfvn=ECCAFFKEY.cfvers%1000;
  strcpy(fname,"ecalrlga");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".cof");
//
//--> get run/time of the first event
//
  StartRun=AMSEcalRawEvent::getsrun();
  StartTime=AMSEcalRawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"ECREUNcalib:error opening RLGA-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"Open file for RLGA-calibration output, fname:"<<fname<<endl;
    cout<<" First run used for calibration is "<<StartRun<<endl;
    cout<<" Date of the first event : "<<frdate<<endl;
    cout<<"Pixels status will be written !"<<endl;
    tcfile.setf(ios::fixed);
    tcfile.width(3);
    tcfile.precision(1);// precision for status
    tcfile << endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile <<" "<<pxstat[pmsl][sc];
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    cout<<"PM RelGains will be written !"<<endl;
    tcfile.width(5);
    tcfile.precision(2);// precision for PM Rel.gain
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile <<" "<<pmrgain[pmsl];
      }
      tcfile << endl;
    }
    tcfile << endl;
    tcfile << endl;
//
    cout<<"Pixel RelGains will be written !"<<endl;
    tcfile.width(5);
    tcfile.precision(2);// precision for Pixel rel.gain
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile <<" "<<pxrgain[pmsl][sc];
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    cout<<"Pixel Hi/Low Gain ratios will be written !"<<'\n';
    tcfile.width(4);
    tcfile.precision(1);// precision for Hi/Low ratio
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile <<" "<<hi2lowr[pmsl][sc];
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    cout<<"PM Anode/Dynode ratios will be written !"<<'\n';
    tcfile.width(4);
    tcfile.precision(1);// precision for PM Anode/Dynode ratio
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile <<" "<<an2dynr[pmsl];
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    tcfile <<" "<<endfm<<endl;
    tcfile << endl;
//
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"ECREUNcalib: RLGA output file closed !"<<endl;
//-------------------------------------------------------
//
//--> create FIAT output file(fiber att.parameters):
//
  if(ECREFFKEY.relogic[1]==2){//only if both RLGA and FIAT parts are requested 
    strcpy(fname,"ecalfiat");
    dig=cfvn/100;
    in[0]=inum[dig];
    strcat(fname,in);
    dig=(cfvn%100)/10;
    in[0]=inum[dig];
    strcat(fname,in);
    dig=cfvn%10;
    in[0]=inum[dig];
    strcat(fname,in);
    if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
    else strcat(fname,vers2);
    strcat(fname,".cof");
    ofstream fifile(fname,ios::out|ios::trunc);
    if(!fifile){
      cerr<<"ECREUNcalib:error opening FIAT-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"Open file for FIAT-calibration output, fname:"<<fname<<endl;
    cout<<" First run used for calibration is "<<StartRun<<endl;
    cout<<" Date of the first event : "<<frdate<<endl;
    cout<<"lfast/lslow/frac will be written !"<<endl;
    fifile.setf(ios::fixed);
    fifile.width(4);
    fifile.precision(1);// precision for slfast
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        fifile <<" "<<slfast;
      }
      fifile << endl;
    }
    fifile << endl;
//
    fifile.width(4);
    fifile.precision(0);// precision for slslow
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        fifile <<" "<<slslow;
      }
      fifile << endl;
    }
    fifile << endl;
//
    fifile.width(4);
    fifile.precision(2);// precision for fastfr
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        fifile <<" "<<fastfr;
      }
      fifile << endl;
    }
    fifile << endl;
//
    fifile <<" "<<endfm;
    fifile << endl;
//
    fifile << endl<<"======================================================"<<endl;
    fifile << endl<<" First run used for calibration is "<<StartRun<<endl;
    fifile << endl<<" Date of the first event : "<<frdate<<endl;
    fifile.close();
    cout<<"ECREUNcalib: FIAT output file closed !"<<endl;
  }
//
}
//--------
void ECREUNcalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number ff;
  f=0.;
//
//
  for(i=0;i<nbins;i++){
    ff=(values[i]-x[0]*((1.-x[2])*exp(-coords[i]/x[1])
      +x[2]*exp(-coords[i]/x[3])))/errors[i];
    f+=(ff*ff);
  }
  if(flg==3){
    f=f/number(nbins-4);
    cout<<"  chi2="<<f<<endl;
    for(i=0;i<4;i++){
      cout<<" par-"<<i<<" ---> "<<x[i]<<endl;
    }
    if(f<5.){//chi2 ok
      nfits+=1;
      slslow+=x[1];
      fastfr+=x[2];
      slfast+=x[3];
    }
  }
}
//---
void ECREUNcalib::fill_1(integer sl, integer pm, integer sc, integer lb, number adc){
//
  integer pmsl;
//
  pmsl=pm+ECPMSMX*sl;//PM continious numbering through all SL's
  if(lb<ECCLBMX){
    pmlres[pmsl][lb]+=adc;
    pmlres2[pmsl][lb]+=(adc*adc);
    tevpml[pmsl][lb]+=1.;//count fired longit.bins
  }
  if(ECCAFFKEY.nortyp==1){// <-- if want to normalize by firings
    if(lb>=(ECCLBMX/2-ECLBMID) && lb<=(ECCLBMX/2+ECLBMID-1))
	                  tevpmm[pmsl]+=1.;//fired at EC-center(+-ECLBMID bins from center)
  }
  sbcres[pmsl][sc]+=adc;
  tevsbf[pmsl][sc]+=1.;
//
}
//---
void ECREUNcalib::fill_2(integer sl, integer pm, integer sc, number adc[2]){
  integer i,slpmc;
  static integer slpmcr,binw,first(0);
//
  if(first++==0){
    slpmcr=4*(ECCAFFKEY.refpid%100-1)+4*ECPMSMX*(ECCAFFKEY.refpid/100-1);//ref.SC
    binw=integer(floor(number(ECCADCR)/ECCHBMX));
  }
//
  slpmc=sc+pm*4+sl*4*ECPMSMX;//continious numbering of sl,pm,sc
  i=integer(floor(adc[0]/binw));// Hchannel bin (in Lch vs Hch dependence)
  if(i<ECCHBMX){
    tevhlc[slpmc][i]+=1.;
    sbchlc[slpmc][i]+=adc[1];
    sbchlc2[slpmc][i]+=(adc[1]*adc[1]);
  }
  if(ECREFFKEY.reprtf[0]!=0 && slpmc==slpmcr)HF2(ECHISTC+19,geant(adc[0]),geant(adc[1]),1.);
}
//
//=============================================================================
void ECREUNcalib::selecte(){// <--- for ANOR calibration
  int i,j,k;
  integer sta,status,dbstat,padc[2],id,idd,isl,pmt,sbc,pmsl,rdir;
  integer npmx,nslmx,maxpl,maxcl,nraw,ovfl[2],proj,plane,cell,lbin;
  number radc[2],binw,trc[3],pmb[3],rrr,sbl,sbr,crl,crr;
  number ct,cl,dct,z,hflen,ctcr,ctpm,clcr,clsh,clpm,acorr;
  number sumh[ECPMSMX][4],suml[ECPMSMX][4],pmsum,pmdis; 
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr,ph,pl,sh,sl,apmt;
  geant pmpit,pxsiz;
  AMSEcalRawEvent * ptr;
  AMSEcalHit * ptr1;
  AMSAntiCluster *ptra;
//
  AMSContainer *cptr;
  AMSParticle *ppart;
  AMSTrTrack *ptrack;
  AMSCharge  *pcharge;
  AMSBeta  *pbeta;
  AMSTrRecHit *phit;
  AMSTrCluster *pxcl;
  AMSTrCluster *pycl;
  AMSTrCluster *x;
  AMSTrCluster *y;
  AMSTrIdSoft idx;
  AMSTrIdSoft idy;
  integer ilay,hitla,nxcl[trconst::maxlay],nycl[trconst::maxlay];
  number axtcl[trconst::maxlay],aytcl[trconst::maxlay];
  number axbcl[trconst::maxlay],aybcl[trconst::maxlay];
  number amp;
//
  integer chargeTOF,chargeTracker,AdvFit,ntrh;
  AMSPoint C0,gC0,Cout1,Cout2,Cpmt,Cout[2];
  AMSDir dir(0,0,1.);
  int ntrk,ipatt;
  number chi2,the,phi,rid,err,trl,hchi2[2],hrid[2],herr[2],hthe[2],hphi[2],ass,beta;
  number gchi2,gthe,gphi,grid,gerr;
  int badtrlx,badtrly,badtrl,badebkg,badclam;
//
  npmx=ECALDBc::slstruc(4);//numb.of PM's/Sl
  nslmx=ECALDBc::slstruc(3);//numb.of Sl
  pxsiz=ECALDBc::rdcell(5);// pm(lg)-pixel size(imply =pm_size/2)
  pmpit=ECALDBc::rdcell(7);// pm transv.pitch
  maxpl=2*ECALDBc::slstruc(3);// SubCell planes
  maxcl=2*ECALDBc::slstruc(4);// SubCells per plane
//
  EcalJobStat::addca(0);
//
//----------------> check Anti :
//
  number eacut=0.6;// thresh. on E-anti-sector (mev)
  integer sector,nanti;
  number eacl,eanti;
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over AMSANTIRawCluster hits
    status=ptra->getstatus();
    if(status==0){ //select only good hits
      sector=(ptra->getsector())-1;
      eacl=ptra->getedep();
      eanti=eanti+(ptra->getedep());
      if(eacl>eacut)nanti+=1;
      HF1(ECHISTC+39,geant(eacl),1.);
    }
    ptra=ptra->next();
  }// --- end of hits loop --->
  HF1(ECHISTC+40,geant(nanti),1.);
//
  if(nanti>1)return;// remove events with >1 sector(e>ecut) in Anti
//
  EcalJobStat::addca(1);
//
// ---------------> get track info :
//
  ntrk=0;
  sta=1;
  AdvFit=0;
  chi2=-1.;
  badtrlx=0;
  badtrly=0;
  badtrl=0;
  badebkg=0;
  badclam=0;
  for(i=0;i<trconst::maxlay;i++){
    nxcl[i]=0;
    nycl[i]=0;
    axtcl[i]=0.;//track cluster amp.
    aytcl[i]=0.;
    axbcl[i]=0.;//backgr.clusters amp.
    aybcl[i]=0.;
  } 
  cptr=AMSEvent::gethead()->getC("AMSParticle",0);// get TOF-matched track
  if(cptr)
          ntrk+=cptr->getnelem();
  if(ntrk!=1)return;// require events with 1 matched track.
  ppart=(AMSParticle*)AMSEvent::gethead()->
                                           getheadC("AMSParticle",0);
  if(ppart){
    ptrack=ppart->getptrack();
    ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
    AdvFit=ptrack->AdvancedFitDone();
    if(AdvFit)ptrack->
        getParAdvancedFit(gchi2,grid,gerr,gthe,gphi,gC0,hchi2,hrid,herr,hthe,hphi,Cout);
    pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
    chargeTracker=pcharge->getchargeTracker();
    pbeta=pcharge->getpbeta();
    beta=0.;
    if(pbeta)beta=pbeta->getbeta();
//
    ntrh=ptrack->getnhits();
    for(i=0;i<ntrh;i++){//<---track hits(2Dclust) loop
      phit=ptrack->getphit(i);
      hitla=phit->getLayer();
//
      pxcl=phit->getClusterP(0);
      if(pxcl){
        axtcl[hitla-1]+=pxcl->getVal();
        x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
        while (x){//<--- x-clust loop
          idx=x->getid();
          ilay=idx.getlayer();
	  if(hitla==ilay){//the same layer
	    if(x->checkstatus(AMSDBc::BAD)==0){
	      if(pxcl==x){
	      }
              else{//count background
	        axbcl[ilay-1]+=x->getVal();
	        nxcl[ilay-1]+=1;
	      }
	    }
	  }
          x=x->next();
        }//---> end of x-clust loop
      }  
//
      pycl=phit->getClusterP(1);
      if(pycl){
        aytcl[hitla-1]+=pycl->getVal();
        y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0);
        while (y){//<--- y-clust loop
          idy=y->getid();
          ilay=idy.getlayer();
	  if(hitla==ilay){//the same layer
	    if(y->checkstatus(AMSDBc::BAD)==0){
	      if(pycl==y){
	      }
              else{//count background
	        aybcl[ilay-1]+=y->getVal();
	        nycl[ilay-1]+=1;
	      }
	    }
	  }
          y=y->next();
        }//---> end of y-clust loop
      }  
//
    }//---> end of track hits loop
  }//---> end if(ppart)
//-----------------------
  if(chi2<0.)return;//--->track not found
  EcalJobStat::addca(2);
//
//---> look at track clusters:
//
  for(i=0;i<trconst::maxlay;i++)HF1(ECHISTC+30,geant(nycl[i]),1.);
  for(i=0;i<trconst::maxlay;i++)HF1(ECHISTC+31,geant(nxcl[i]),1.);
//
  for(i=0;i<trconst::maxlay;i++){
    rrr=0.;
    if(axtcl[i]>0.)rrr=axbcl[i]/axtcl[i];
    if(rrr>0.4)badebkg=1;
    if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+32,geant(rrr),1.);
    if(ECREFFKEY.reprtf[0]!=0)if(axtcl[i]>0.)HF1(ECHISTC+34,geant(axtcl[i]),1.);
//    if(nxcl[i]>4 || rrr>1. || axtcl[i]>100.)badtrlx+=1;
    if(axtcl[i]>200.)badclam=1;
    rrr=0.;
    if(aytcl[i]>0.)rrr=aybcl[i]/aytcl[i];
    if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+33,geant(rrr),1.);
    if(rrr>0.6)badebkg=1;
    if(ECREFFKEY.reprtf[0]!=0)if(aytcl[i]>0.)HF1(ECHISTC+35,geant(aytcl[i]),1.);
//    if(nycl[i]>4 || rrr>1. || aytcl[i]>100.)badtrly+=1;
    if(aytcl[i]>200.)badclam=1;
    if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+42,geant(axtcl[i]+aytcl[i]),1.);
    if((axtcl[i]+aytcl[i])>150.)badtrl+=1;
  }
//
// ---------------> check particle(electron) quality :
//
  ass=0.;
  if(AdvFit){
    if(fabs(hrid[0]+hrid[1])>0.)ass=(hrid[0]-hrid[1])/(hrid[0]+hrid[1]);
  }
  if(ECREFFKEY.reprtf[0]!=0){
    HF1(ECHISTC+1,geant(chi2),1.);
    HF1(ECHISTC+2,geant(fabs(rid)),1.);// R
    HF1(ECHISTC+3,geant(fabs(rid*err)),1.);//dR/R
    HF1(ECHISTC+4,geant(ass),1.);//half-rig. ass.
    HF1(ECHISTC+44,geant(beta),1.);//beta
    if(badclam)HF1(ECHISTC+36,geant(fabs(rid)),1.);
    if(badebkg)HF1(ECHISTC+43,geant(fabs(rid)),1.);
  }
  if(fabs(rid)<ECCAFFKEY.pmin || fabs(rid)>ECCAFFKEY.pmax)return;//--->out of needed range
//
  if(!(beta>0. && rid<0.))return;//---> it is not electron(charge<0) from AMS_top->bot
//
  if(chi2>20.)return;//---> bad chi2
//
  if(fabs(beta-1.)>0.1)return;//---> bad beta (too low to be electron)
// 
  if(fabs(ass)>0.06)return;//---> bad half-rigidities assimetry
//
//  if(fabs(rid*err)>0.014)return;// do not affect
//  if(badclam>0)return;//do not affect on low-Rig tail(at least for MC)
//  if(badebkg>0)return;
//
  if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+41,geant(fabs(rid)),1.);
//
  EcalJobStat::addca(3);
//
// ---------------> check that track  hits ECAL:
//
  number dx,dy,sfg;
  int icr(0);
//
  C0[2]=ECALDBc::gendim(7);// Z-top of ECAL
  C0[0]=0.;
  C0[1]=0.;
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Ztop of EC
  if(ECREFFKEY.reprtf[0]!=0){
    HF1(ECHISTC,geant(cos(the)),1.);
    HF1(ECHISTC+37,geant(Cout1[0]),1.);
    HF1(ECHISTC+38,geant(Cout1[1]),1.);
  }
  if(fabs(cos(the))<0.94)return;// ----> check the impact angle(should be < 20degr)
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  sfg=-1.;
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
//
  C0[2]=ECALDBc::gendim(7)-nslmx*(ECALDBc::gendim(9)+2.*ECALDBc::gendim(10));// Z-bot of ECAL
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Zbot of EC
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  sfg=-4.;
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
  if(icr<=1)return;// ----> no good crossing with ECAL 
//
  EcalJobStat::addca(4);
//
//-------------------------> select True-electron events :
// (EcalHits  ALREADY gain-corrected using RLGA/(FIAT) part(s) of REUN-calibration)
// (Absolute normalization(mev) is STILL from previous (OLD) calibration !!!)
//
  number edthr,edep,edept,esleak,ebleak,edlpr[2*ECSLMX],edtpr[2*ECSLMX][2*ECPMSMX];
  integer nsclpr[2*ECSLMX];
  integer refsl,refpm,ipl,imax,imin,plholes[2*ECSLMX];
  int nnn,bad1,bad2,bad3,spikes,badscl(0);
  int badhl[6]={0,0,0,0,0,0};
  geant ad2mv;
  number scadcmx;
//
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  ad2mv=ECcalib::ecpmcal[refsl-1][refpm-1].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
//
  edept=0.;
  esleak=0.;
  scadcmx=0.;
  bad3=0;
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell-Planes loop
    ptr1=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);
    for(i=0;i<maxcl;i++){
      edtpr[ipl][i]=0.;
    }
    edlpr[ipl]=0.;
    nsclpr[ipl]=0;
    while(ptr1){ // <--- EcalHits(fired subcells) loop in plane:
      edep=ptr1->getedep();//(mev)
      proj=ptr1->getproj();//0/1->X/Y (have to be corresponding to ipl)
      cell=ptr1->getcell();// 0,...
      isl=ipl/2;
      pmt=cell/2;
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+5,geant(edep/ad2mv),1.);
      if(edep>=(ad2mv*ECCAFFKEY.scmin)){// because scmin-threshold is in ADC-units
        edlpr[ipl]+=edep;
	edtpr[ipl][cell]+=edep;
	edept+=edep;
	if(pmt==0 || pmt==(npmx-1))esleak+=edep;
      }
      ptr1=ptr1->next();  
    } // ---> end of EcalHits loop in Plane ipl
//
    plholes[ipl]=0;
    spikes=0;
    bad1=0;
    bad2=0;
    imin=-1;
    imax=0;
    for(i=0;i<maxcl;i++){// <--- patt. of fired subcells in plane
      if(edtpr[ipl][i]>ECCAFFKEY.scmax*ad2mv)spikes+=1;//count spikes(sparks,ovfl,...)
      if(edtpr[ipl][i]/ad2mv>scadcmx)scadcmx=edtpr[ipl][i]/ad2mv;
      if(edtpr[ipl][i]>0.){
        nsclpr[ipl]+=1;//count fired SubCell's per plane
	imax=i;//to find most right fired sc
	if(imin<0)imin=i;//to find most left fired sc
      }
    }//---> end of SubCell loop in plane
    k=imax-imin;
    if(nsclpr[ipl]>0)plholes[ipl]=k+1-nsclpr[ipl];
//
    if(ECREFFKEY.reprtf[0]!=0){
      HF1(ECHISTC+6,geant(nsclpr[ipl]),1.);
      nnn=nsclpr[ipl];
      if(nnn>19)nnn=19;
      if(ipl<=5)HF1(ECHISTC+7,geant(nnn+ipl*20),1.);
      nnn=plholes[ipl];
      if(nnn>19)nnn=19;
      if(ipl==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==1 && badhl[0]==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==2 && badhl[0]==0 && badhl[1]==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==3 && badhl[0]==0 && badhl[1]==0 && badhl[2]==0
                                              && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==4 && badhl[0]==0 && badhl[1]==0 && badhl[2]==0
                               && badhl[3]==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==5 && badhl[0]==0 && badhl[1]==0 && badhl[2]==0
                               && badhl[3]==0 && badhl[4]==0
			                      && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      HF1(ECHISTC+24,geant(spikes),1.);
    }
    if(nsclpr[ipl]>0){//non empty plane
      if(spikes>ECCAFFKEY.spikmx)bad1=1;//mark plane with "spike(s)" 
      if(nsclpr[ipl]>ECCAFFKEY.lmulmx)bad2=1;//abnorm.multiplicity(any layer)
      if(ipl<=5 && (nsclpr[ipl]>ECCAFFKEY.nhtlmx[ipl] 
                           || plholes[ipl]>ECCAFFKEY.nholmx[ipl]))badhl[ipl]=1;//mark early showering
    }
    if(bad1>0 || bad2>0)badscl+=1;//mark bad Plane(spikes or too high multiplicity)
//
  }//---> end of Planes-loop
//
  if(ECREFFKEY.reprtf[0]!=0){
    HF1(ECHISTC+8,geant(badscl),1.);
    HF1(ECHISTC+19,geant(scadcmx),1.);
  }
//
  if(badscl>ECCAFFKEY.nbplmx)return;//too many planes with spikes,...
//
  nnn=badhl[0]+badhl[1]+badhl[4]+badhl[5];//only pl 1-2,5-6 are really working...
  if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+25,geant(nnn),1.);
//
  if(ECREFFKEY.relogic[2]==0 || ECREFFKEY.relogic[2]==2){//use "holes" cut if requested
    if(nnn>0)return;//early shower("nnn>1" works slightly worse but with 1.5 times higher eff)
  }
  EcalJobStat::addca(5);
//
// ---->electron identification(soft):
//
  geant e4x0,epeaka,etaila,rr,rematch;
  e4x0=edlpr[0]+edlpr[1]+edlpr[2]+edlpr[3];//energy(mev) in 1st 4X0's(1-4pl) of Z-profile
  epeaka=(edlpr[4]+edlpr[5]+edlpr[6])/3.;//aver.energy/plane in peak(5-7pl) of Z-profile
  etaila=(edlpr[maxpl-2]+edlpr[maxpl-1])/2.;//aver.energy/plane in tail(17-18pl) ...
  rr=0.;
  if(epeaka>0.)rr=etaila/epeaka;
  if(rr>0.99)rr=0.99;
//
  if(ECREFFKEY.reprtf[0]!=0){
    HF1(ECHISTC+9,geant(edept),1.);
  }
  int ecflg(0);
  if(edept>ECCAFFKEY.edtmin && (0.001*edept)<(1.5*ECCAFFKEY.pmax)){// not MIP, Et not exeed max. mom
    esleak/=edept;//Esleak fraction
    ebleak=edlpr[maxpl-1]/edept;
    if(ECREFFKEY.reprtf[0]!=0){
      HF1(ECHISTC+10,geant(esleak),1.);
      HF1(ECHISTC+17,geant(ebleak),1.);
    }
    if(esleak<ECCAFFKEY.esleakmx && ebleak<ECCAFFKEY.ebleakmx){
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+11,e4x0,1.);
      if(e4x0>ECCAFFKEY.edfrmn){
        if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+12,rr,1.);
	if(rr<ECCAFFKEY.edt2pmx){
	  rematch=0.001*edept/fabs(rid)-1.;
	  if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+13,rematch,1.);
	  if(fabs(rematch)<ECCAFFKEY.ed2momc)ecflg=1;
	}
      }
    }
  }
  if(ecflg==0)return;//---> Not electron 
  EcalJobStat::addca(6);
//
//---------> check track-ECHits matching in SC-planes:
//
  geant lfs,lsl,ffr,dctmx;
  geant mismcut[2*ECSLMX];
  number edeptnc,attf,cog,edpl;
  number emism[2*ECSLMX],ematch[2*ECSLMX];
  int cmism,nlmism,cogmism;
  int mism[6]={0,0,0,0,0,0};
//
  mismcut[0]=ECCAFFKEY.scdismx[0];
  mismcut[1]=ECCAFFKEY.scdismx[1];
  mismcut[2]=ECCAFFKEY.scdismx[2];
  mismcut[3]=ECCAFFKEY.scdismx[3];
  mismcut[4]=ECCAFFKEY.scdismx[4];
  mismcut[5]=ECCAFFKEY.scdismx[5];
  for(i=6;i<2*ECSLMX;i++)mismcut[i]=ECCAFFKEY.scdisrs;
//
  edept=0.;
  edeptnc=0.;
  cogmism=0;
  nlmism=0;//layers with early showering(mism.cells)
  sta=0;//good
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell-Planes loop
    emism[ipl]=0.;
    ematch[ipl]=0.;
//
//  ---> extrapolate track to plane :
    isl=ipl/2;
    if(ipl%2==0)ECALDBc::getscinfoa(isl,0,0,proj,plane,cell,ct,cl,z);//1stPM,1stPixel
    else ECALDBc::getscinfoa(isl,0,2,proj,plane,cell,ct,cl,z);//1stPM,3rdPixel
    C0[2]=z;
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<---cross with SC
    if(proj==0){// <--- x-proj (0)
      ctcr=Cout1[0];
      clcr=Cout1[1];
      hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir
      clsh=ECALDBc::gendim(6);//EC-pos in y
      acorr=1./sqrt(1.+pow(sin(the)*sin(phi)/cos(the),2));
    }
    else{//        <--- y-proj (1)
      ctcr=Cout1[1];
      clcr=Cout1[0];
      hflen=ECALDBc::gendim(1)/2.;// 0.5*fiber(EC) length in X-dir
      clsh=ECALDBc::gendim(5);//EC-pos in x
      acorr=1./sqrt(1.+pow(sin(the)*cos(phi)/cos(the),2));
    }
    if(fabs(clcr-clsh)>hflen)continue;//track out of plane ===> try next plane
    clpm=cl;// middle of fiber in AMS r.s.!
//
    edpl=0.;
    cog=0.;
    dctmx=-1.;
    cmism=0;//count cells out of track(from early shower)
//
    for(cell=0;cell<maxcl;cell++){ // <-------------- SubCell loop in plane
      pmt=cell/2;
      ad2mv=ECcalib::ecpmcal[isl][pmt].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
      edep=edtpr[ipl][cell];//mev
      if(edep>0.){
        if(proj==0){// <--- x-proj (0)
          rdir=(1-2*(pmt%2))*ECALDBc::slstruc(5);//readout dir(+-1->along pos/neg Y)
        }
        else{//        <--- y-proj (1)
          rdir=(1-2*(pmt%2))*ECALDBc::slstruc(6);//readout dir(+-1->along pos/neg X)
        }
        if(rdir>0){
          clpm=clpm-hflen;//pm-pos if pm is at -x(-y) (i.e. rdir=1)
          pmdis=clcr-clpm;//0-2*hflen
        }
        else{
          clpm=clpm+hflen;//pm-pos if pm is at +x(+y) (i.e. rdir=-1)
          pmdis=clpm-clcr;//0-2*hflen
        }
	lfs=ECcalib::ecpmcal[isl][pmt].alfast();//att_len(fast comp) from DB
	lsl=ECcalib::ecpmcal[isl][pmt].alslow();//att_len(slow comp) from DB
	ffr=ECcalib::ecpmcal[isl][pmt].fastfr();//fast comp. fraction from DB
        attf=(1.-ffr)*exp(-pmdis/lsl)+ffr*exp(-pmdis/lfs);//fiber attenuation factor
	if(ipl%2==0){//sbc=0,1
	  sbc=cell%2;
	}
	else{//sbc==2,3
	  sbc=(cell%2)+2;
	}
	ECALDBc::getscinfoa(isl,pmt,sbc,proj,plane,i,ct,cl,z);//get cell transv.coo
	dct=ct-ctcr;
	if(ECREFFKEY.reprtf[0]!=0){
	  if(ipl==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==1 && mism[0]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==2 && mism[0]==0 && mism[1]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==3 && mism[0]==0 && mism[1]==0 && mism[2]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==4 && mism[0]==0 && mism[1]==0 && mism[2]==0
	                                        && mism[3]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==5 && mism[0]==0 && mism[1]==0 && mism[2]==0
	                          && mism[3]==0 && mism[4]==0)HF1(ECHISTC+50+ipl,dct,1.);
	}
//
	if(fabs(dct)>mismcut[ipl]){//count mismatched(backgr) cells\energy
	  cmism+=1;
	  emism[ipl]+=edep;
	}
	else ematch[ipl]+=edep;//count matched energy
//
	if(fabs(dct)>dctmx)dctmx=fabs(dct);
	if(fabs(dct)<18.){// tempor simulate clustering algorithm(replace later) 
	  edeptnc+=edep;
//	  if(fabs(dct)<1.8)edep/=attf;//"nearby only" fib.att. correction(don't help !)
	  edpl+=edep;
	  cog+=edep*ct;
	  edept+=edep;
	}
      }//---> endif of Edep>0
    }//----------> end of SC-loop
//
    if(ipl<=5 && cmism>0)mism[ipl]=1;//count planes(among 1st 6) with early showering(mism.cells)
//
    if(ECREFFKEY.reprtf[0]!=0 && dctmx>0.){
      HF1(ECHISTC+14,dctmx,1.);
      if(ipl==0){
        HF1(ECHISTC+21,dctmx,1.);
      }
    }
    if(edpl>0.){
      cog/=edpl;// plane CenterOfGravity(in transv.dir)
      dct=cog-ctcr;
      if(ECREFFKEY.reprtf[0]!=0){
        if(ipl>3 && ipl<=5)HF1(ECHISTC+15,geant(dct),1.);
        if(ipl<=1)HF1(ECHISTC+22,geant(dct),1.);
        if(ipl>1 && ipl<=3)HF1(ECHISTC+23,geant(dct),1.);
	HF1(ECHISTC+29,geant(ipl),geant(edpl/1000.));
      }
      if(ipl<=5 && fabs(dct)>ECCAFFKEY.cog1cut)cogmism+=1;
    }
//
  }//--------> end of SC-planes loop
//------------------------------
//
  bad3=0;
  for(i=0;i<6;i++){
    rrr=0.;
    if(ematch[i]>0.)rrr=emism[i]/ematch[i];
    if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTC+56+i,geant(rrr),1.);
    if(rrr>ECCAFFKEY.b2scut[i])bad3+=1;
  }
//
//
  if(cogmism>0)sta=1;//---> too high COG-mismatching in plane 1,2 or 3
  if(sta>0)return;
  EcalJobStat::addca(7);
//
  if(ECREFFKEY.relogic[2]==1 || ECREFFKEY.relogic[2]==2){
    if(bad3>0)sta=1;//---> Ebkg/Esignal too high in one of the 1st 6 planes
  }
  if(sta>0)return;
  EcalJobStat::addca(8);
//
//g.chen
  if(ECCAFFKEY.ecshswit==1){ // add shower info here
    AMSEcalShower * ptsh;
    number ecshen,ecshener,efront,chi2dir,difosum,ecshsleak,ecshrleak,ecshdleak,ecsholeak;
    number ecsherdir,profchi2,transchi2;
    AMSDir ecshdir(0.,0.,0.);
    AMSPoint ecshentry,ecshexit,ecshcog;
    int  ecshnum;
    ecshnum=0;
    ecshen=0.;
    ecshener=0.;
    efront=0.;
    ptsh=(AMSEcalShower*)AMSEvent::gethead()->
      getheadC("EcalShower",0);
    while(ptsh){ // <------- ecal shower
      ecshnum++;
      ptsh=ptsh->next();
    }
    if(ecshnum!=1)return;          // only use event with one shower  
    
    ptsh=(AMSEcalShower*)AMSEvent::gethead()->
      getheadC("EcalShower",0);
    //  while(ptsh){ // <------- ecal shower
    ecshen=ptsh->getEnergy();
    //    ecshener=ptsh->getEnergyErr();
    ecshdir=ptsh->getDir();
    ecshentry=ptsh->getEntryPoint();
    ecshexit=ptsh->getExitPoint();
    ecshcog=ptsh->getCofG();
    efront=ptsh->getEnFront();
    chi2dir=ptsh->getDirChi2();
    ecsherdir=ptsh->getErDir();
    difosum=ptsh->getDifoSum();
    ecshsleak=ptsh->getSLeak();
    ecshrleak=ptsh->getRLeak();
    ecshdleak=ptsh->getDLeak();
    ecsholeak=ptsh->getOLeak();
    profchi2=ptsh->getProfChi2();
    transchi2=ptsh->getTransChi2();
    
    if(ECREFFKEY.reprtf[0]!=0){
      HF1(ECHISTC+62,ecshen,1.);
      HF1(ECHISTC+63,chi2dir,1.);
      HF1(ECHISTC+64,efront,1.);
      HF1(ECHISTC+65,difosum,1.);
      HF1(ECHISTC+66,ecshsleak,1.);
      HF1(ECHISTC+67,ecshrleak,1.);
      HF1(ECHISTC+68,ecshdleak,1.);
      HF1(ECHISTC+69,ecsholeak,1.);
      HF1(ECHISTC+70,profchi2,1.);
      HF1(ECHISTC+71,transchi2,1.);
      HF1(ECHISTC+79,ecshen/fabs(rid),1.);
      HF1(ECHISTC+102,ecshener,1.);
      HF1(ECHISTC+103,geant(1000.*fabs(rid)),1.);
    }
    if(chi2dir>ECCAFFKEY.chi2dirmx) return;
    if(profchi2>ECCAFFKEY.prchi2mx) return;   //profile fit (long.)
    if(transchi2>ECCAFFKEY.trchi2mx) return; //trans. fit
    if(ECREFFKEY.reprtf[0]!=0){
      HF1(ECHISTC+104,ecshen,1.);
      HF1(ECHISTC+105,geant(1000.*fabs(rid)),1.);
    }
    icr=0;
    sfg=-1.;
    if(fabs(ecshentry[0])<(ECALDBc::gendim(1)/2.+sfg) && fabs(ecshentry[1])<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
    sfg=-4.;
    if(fabs(ecshexit[0])<(ECALDBc::gendim(1)/2.+sfg) && fabs(ecshexit[1])<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
    if(icr<=1) return; //shower entry and exit should cross with ECAL
    //    if(ecshener>ECCAFFKEY.eshermax) return;
    if(ecshsleak>ECCAFFKEY.eshsleakmx) return;  // side leakage
    if(ecshrleak>ECCAFFKEY.eshrleakmx) return;  // rear leakage
    if(ECREFFKEY.reprtf[0]!=0){
      HF1(ECHISTC+106,ecshen,1.);
      HF1(ECHISTC+107,geant(1000.*fabs(rid)),1.);
    }
    if(ecshdleak>ECCAFFKEY.eshdleakmx) return;  // dead leakage
    if(ecsholeak>ECCAFFKEY.esholeakmx) return;   // orphan hits
    if(ECREFFKEY.reprtf[0]!=0){
      HF1(ECHISTC+108,ecshen,1.);
      HF1(ECHISTC+109,geant(1000.*fabs(rid)),1.);
    }
    if(difosum>ECCAFFKEY.difsummx) return; // (E_x-E_y)/(E_x+E_y)
    //
    EcalJobStat::addca(9);
    //
    // ---->track match with shower:
    //
    number dxent, dyent, dxext, dyext, rematch;
    C0[2]=ECALDBc::gendim(7);// Z-top of ECAL
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Ztop of EC
    dxent=Cout1[0]-ecshentry[0];
    dyent=Cout1[1]-ecshentry[1];
    C0[2]=ECALDBc::gendim(7)-nslmx*(ECALDBc::gendim(9)+2.*ECALDBc::gendim(10));// Z-bot of ECAL
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Zbot of EC
    dxext=Cout1[0]-ecshexit[0];
    dyext=Cout1[1]-ecshexit[1];
    rematch=ecshen/fabs(rid)-1.;
    
    if(ECREFFKEY.reprtf[0]!=0){
      HF1(ECHISTC+72,fabs(rid)/ecshen,1.);
      HF1(ECHISTC+73,rematch,1.);
      HF1(ECHISTC+74,dxent,1.);
      HF1(ECHISTC+75,dyent,1.);
      HF1(ECHISTC+76,dxext,1.);
      HF1(ECHISTC+77,dyext,1.);
      HF1(ECHISTC+110,ecshen,1.);
      HF1(ECHISTC+111,geant(1000.*fabs(rid)),1.);
    }
    if(fabs(rematch)>ECCAFFKEY.ed2momc) return;
    if(fabs(dxent)>ECCAFFKEY.trentmax[0]) return;
    if(fabs(dyent)>ECCAFFKEY.trentmax[1]) return;
    if(fabs(dxext)>ECCAFFKEY.trextmax[0]) return;
    if(fabs(dyext)>ECCAFFKEY.trextmax[1]) return;
    EcalJobStat::addca(10);
    HF1(ECHISTC+112,ecshen,1.);
    //
  }//---> end of "g.chen" check
//
  if(ECREFFKEY.reprtf[0]!=0){
    HF1(ECHISTC+16,geant(0.001*edept/fabs(rid)),1.);
    HF1(ECHISTC+18,geant(1000.*fabs(rid)/edept),1.);
    HF1(ECHISTC+26,geant(edept),1.);
    HF1(ECHISTC+27,geant(edeptnc),1.);
    HF1(ECHISTC+28,geant(1000.*fabs(rid)),1.);
  }
}
//----------------------------
void ECREUNcalib::mfite(){
//
  int i,j,k,bad;
  integer sl,pm,sc,pmsl,lb,npmx,slpmc;
  integer pmslr,refsl,refpm;
  geant ad2mv,anorf;
  number rrr,sum4,eff,hflen,binw;
  char htit1[60];
  char htit2[60];
  char inum[11];
  char in[2]="0";
  char chopt[6]="qb";
  char chfun[6]="g";
  char chopt1[5]="LOGY";
  geant par[3],step[3],pmin[3],pmax[3],sigp[3],chi2;
//
  strcpy(inum,"0123456789");
  npmx=ECALDBc::slstruc(4);//numb.of PM's/sl
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  pmslr=refpm-1+ECPMSMX*(refsl-1);//sequential numbering for ref. PM
  hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir(=X)
  ad2mv=ECcalib::ecpmcal[refsl-1][refpm-1].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
//
// ---> fit histogr. Epart/Eecal with gaussian:
//
  par[0]=200.;//amplitude
  par[1]=1.;//most prob.
  par[2]=0.1;//sigma
  step[0]=50.;
  step[1]=0.1;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=0.2;
  pmin[2]=0.01;
  pmax[0]=10000.;
  pmax[1]=1.8;
  pmax[2]=0.9;
  HFITHN(ECHISTC+18,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" ECCALIB-ANOR-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(ECHISTC+18);
  anorf=ad2mv*par[1];// New (1) ad2mv factor
  cout<<" ECCALIB-ANOR: old/new ADC->Mev factor = "<<ad2mv<<" / "<<anorf<<endl;
//
//------------> write outp.files :
//  
  char fname[80];
  char frdate[30];
  char vers1[3]="mc";
  char vers2[3]="rl";
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
  integer endfm(12345);
//
//--> create ANOR output file(absolute normalization factor(the same for all PM's):
// 
  strcpy(inum,"0123456789");
  cfvn=ECCAFFKEY.cfvers%1000;
  strcpy(fname,"ecalanor");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".cof");
//
//--> get run/time of the first event
//
  StartRun=AMSEcalRawEvent::getsrun();
  StartTime=AMSEcalRawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"ECREUNcalib:error opening ANOR-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"Open file for ANOR-calibration output, fname:"<<fname<<endl;
    cout<<" First run used for calibration is "<<StartRun<<endl;
    cout<<" Date of the first event : "<<frdate<<endl;
    cout<<"Abs.normaliz.factors will be written !"<<endl;
    tcfile.setf(ios::fixed);
    tcfile.width(5);
    tcfile.precision(3);// precision
    tcfile << endl;
    tcfile <<" "<<anorf;//hope it is common for all cells
    tcfile << endl;
    tcfile << endl;
//
    tcfile <<" "<<endfm<<endl;
    tcfile << endl;
//
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"ECREUNcalib: ANOR output file closed !"<<endl;
//
}
//------------------------------------------------------------------------------------
//
//=======================> "On-Data" + "OnBoardPedTable" Pedestals/Sigmas Calibration
//
 time_t ECPedCalib::BeginTime;
 uinteger ECPedCalib::BeginRun;
 number ECPedCalib::adc[ECPMSL][5][2];
 number ECPedCalib::adc2[ECPMSL][5][2];//**2
 number ECPedCalib::adcm[ECPMSL][5][2][ECPCSTMX];//stack of ECPCSS max's (Highest is 1st)
 integer ECPedCalib::nevt[ECPMSL][5][2];
 integer ECPedCalib::hiamap[ecalconst::ECSLMX][ecalconst::ECPMSMX];//high signal PMTs map (1 event) 
 geant ECPedCalib::peds[ECPMSL][5][2];
 geant ECPedCalib::sigs[ECPMSL][5][2];
 uinteger ECPedCalib::stas[ECPMSL][5][2];
 integer ECPedCalib::nstacksz;//needed stack size (ev2rem*ECPCEVMX)
 ECPedCalib::ECPedCalib_ntpl  ECPedCalib::ECPedCalNT;
//-----
 void ECPedCalib::init(){//called in caecinitjob() 
   int16u i,sl,pm,pix,gn,gnm;
   int hnm,ch;
   char buf[6];
   char htit[80];
//
   cout<<endl;
   if(ECREFFKEY.relogic[1]==6){//open Ntuple file (for OnBoardTable only for the moment)
     char hfile[161];
     UHTOC(IOPA.hfile,40,hfile,160);  
     char filename[256];
     strcpy(filename,hfile);
     integer iostat;
     integer rsize=1024;
     char event[80];  
     HROPEN(IOPA.hlun+1,"ecpedsig",filename,"NP",rsize,iostat);
     if(iostat){
       cerr << "<==== ECPedCalib::init: Error opening ecpedsig ntuple file "<<filename<<endl;
       exit(1);
     }
     else cout <<"====> ECPedCalib::init: Ntuple file "<<filename<<" opened..."<<endl;
     HBNT(IOPA.ntuple,"EcalPedSigmas"," ");
     HBNAME(IOPA.ntuple,"ECPedSig",(int*)(&ECPedCalNT),"Run:I,SLayer:I,PMTNo:I,PedH(5):R,SigH(5):R,"
                                                                               "PedL(5):R,SigL(5):R,"
									       "StaH(5):I,StaL(5):I");
     return;
   }
//
   nstacksz=floor(ECCAFFKEY.pedcpr*ECPCEVMX+0.5);
   if(nstacksz>ECPCSTMX){
     cout<<"====> ECPedCalib::init-W-Stack too small, change Trunc-value or max.events/ch !!!"<<nstacksz<<endl;
     cout<<"                Its size set back to max-possible:"<<ECPCSTMX<<endl;
     nstacksz=ECPCSTMX;
   }
   if(nstacksz<1)nstacksz=1;
   cout<<"====> ECPedCalib::init: real stack-size="<<nstacksz<<endl;
//
   for(sl=0;sl<ECSLMX;sl++){
     for(pm=0;pm<ECPMSMX;pm++){
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){
	   adc[ch][pix][gn]=0;
	   adc2[ch][pix][gn]=0;
	   for(i=0;i<ECPCSTMX;i++)adcm[ch][pix][gn][i]=0;
	   nevt[ch][pix][gn]=0;
	   peds[ch][pix][gn]=0;
	   sigs[ch][pix][gn]=0;
	   stas[ch][pix][gn]=1;//bad
	 }
       }
     }
   }
//
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Peds vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+100+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+100+hnm,100.);
       HMAXIM(ECHISTC+100+hnm,200.);
     }
   }
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Sigs vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+136+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+136+hnm,0.2);
       HMAXIM(ECHISTC+136+hnm,2.);
     }
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Peds vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+172+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+172+sl,50.);
     HMAXIM(ECHISTC+172+sl,150.);
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Sigs vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+181+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+181+sl,0.2);
     HMAXIM(ECHISTC+181+sl,2.);
   }
   HBOOK1(ECHISTC+190,"Sl/Pm/Pix=5/18/2 AnodeH",100,50.,250.,0.);
   HBOOK1(ECHISTC+191,"Sl/Pm/Pix=5/18/2 AnodeL",100,50.,250.,0.);
   HBOOK1(ECHISTC+192,"Sl/Pm=5/18 Dynode",100,50.,250.,0.);
   HBOOK1(ECHISTC+193,"AllChannels Anode(HiGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+194,"AllChannels Anode(LoGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+195,"AllChannels Dynode PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+196,"AllChannels Anode(HiGain) PedDiff",50,-2.5,2.5,0.);
   HBOOK1(ECHISTC+197,"AllChannels Anode(LoGain) PedDiff",50,-2.5,2.5,0.);
   HBOOK1(ECHISTC+198,"AllChannels Dynode PedDiff",50,-2.5,2.5,0.);
   cout<<"====> ECPedCalib::init done..."<<endl<<endl;;
 }
//-----
 void ECPedCalib::resetb(){//init for OnBoardPedTable processing,
//called in buildraw() when 1st pedtable sub-block was found
   int16u i,sl,pm,pix,gn,gnm;
   int hnm,ch;
   char buf[6];
   char htit[80];
   static int first(0);
   char hmod[2]=" ";
//
   cout<<endl;
//
  if(first==0){
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Peds vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+100+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+100+hnm,100.);
       HMAXIM(ECHISTC+100+hnm,200.);
     }
   }
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Sigs vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+136+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+136+hnm,0.2);
       HMAXIM(ECHISTC+136+hnm,2.);
     }
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Peds vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+172+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+172+sl,50.);
     HMAXIM(ECHISTC+172+sl,150.);
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Sigs vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+181+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+181+sl,0.2);
     HMAXIM(ECHISTC+181+sl,2.);
   }
   HBOOK1(ECHISTC+193,"AllChannels Anode(HiGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+194,"AllChannels Anode(LoGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+195,"AllChannels Dynode PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+196,"AllChannels Anode(HiGain) PedDiff",50,-2.5,2.5,0.);
   HBOOK1(ECHISTC+197,"AllChannels Anode(LoGain) PedDiff",50,-2.5,2.5,0.);
   HBOOK1(ECHISTC+198,"AllChannels Dynode PedDiff",50,-2.5,2.5,0.);
   first=1;
  }
  else{
    for(i=0;i<100;i++)HRESET(ECHISTC+100+i,hmod);
  }
//
   for(sl=0;sl<ECSLMX;sl++){
     for(pm=0;pm<ECPMSMX;pm++){
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){
	   peds[ch][pix][gn]=0;
	   sigs[ch][pix][gn]=0;
	   stas[ch][pix][gn]=1;//bad
	 }
       }
     }
   }
//
   cout<<"====> ECPedCalib::OnBoardPedTable/Histos Reset done..."<<endl<<endl;;
 }
//-----
 void ECPedCalib::fill(integer swid, geant val){
//
   int16u i,sl,pm,pix,gn,ch,nev,is;
   geant lohil[2]={0,9999};//means no limits on val, if partial ped is bad
// sl=0->, pm=0->, pix=1-3=>anodes,=4->dynode, gn=0/1->hi/low(for dynodes only hi)
// swid=>LTTPG(SupLayer/PMTube/Pixel/Gain)
   geant ped,sig,sig2;
   geant hi2lr,a2dr,gainf,spikethr;
   bool accept(true);
   int pml,pmr;
//
   integer evs2rem;
//
   sl=swid/10000;
   sl=sl-1;//0-8
   pm=(swid%10000)/100-1;//0-35
   ch=ECPMSMX*sl+pm;//seq.# of sl*pm
   pix=(swid%100)/10;
   pix=pix-1;//0-4 (4->dynode)
   gn=(swid%10)-1;//0/1->high/low
   if(pix==4)gn=0;
//
   nev=nevt[ch][pix][gn];
// peds[ch][pix][gn];//SmalSample(SS) ped (set to "0" at init)
   if(peds[ch][pix][gn]==0 && nev==ECPCEVMN){//calc. SS-ped/sig when ECPCEVMN events is collected
     evs2rem=floor(ECCAFFKEY.pedcpr*nev+0.5);
     if(evs2rem>nstacksz)evs2rem=nstacksz;
     if(evs2rem<1)evs2rem=1;
     for(i=0;i<evs2rem;i++){//remove "evs2rem" highest amplitudes
       adc[ch][pix][gn]=adc[ch][pix][gn]-adcm[ch][pix][gn][i];
       adc2[ch][pix][gn]=adc2[ch][pix][gn]-adcm[ch][pix][gn][i]*adcm[ch][pix][gn][i];
     }
     ped=adc[ch][pix][gn]/number(nev-evs2rem);//truncated average
     sig2=adc2[ch][pix][gn]/number(nev-evs2rem);
     sig2=sig2-ped*ped;// truncated rms**2
     if(sig2>0 && sig2<=(2.25*ECPCSIMX*ECPCSIMX)){//2.25->1.5*SigMax
       sigs[ch][pix][gn]=sqrt(sig2);
       peds[ch][pix][gn]=ped;//is used now as flag that SS-PedS ok
     }
     adc[ch][pix][gn]=0;//reset to start new life(with real selection limits)
     adc2[ch][pix][gn]=0;
     nevt[ch][pix][gn]=0;
     for(i=0;i<ECPCSTMX;i++)adcm[ch][pix][gn][i]=0;
   }
   ped=peds[ch][pix][gn];//now !=0 or =0 
   sig=sigs[ch][pix][gn];
//
   if(ped>0){//set val-limits if partial ped OK
     lohil[0]=ped-3*sig;
     lohil[1]=ped+5*sig;
     if(gn==0){//hi(an or dyn)
       if(pix<4)gainf=1.;//hi an
       else gainf=ECcalib::ecpmcal[sl][pm].an2dyr();//dyn
     }
     else{//low an
       gainf=ECcalib::ecpmcal[sl][pm].hi2lowr(pix);//low an
     }
     spikethr=max(5*sig,ECPCSPIK/gainf);
     if(val>(ped+spikethr)){//spike(>~1mips in higain chan)
       hiamap[sl][pm]=1;//put it into map
       accept=false;//mark as bad for filling
     }
     else{//candidate for "fill"
       if(pm>0)pml=pm-1;
       else pml=0;
       if(pm<(ECPMSMX-1))pmr=pm+1;
       else pmr=ECPMSMX-1;
       accept=(hiamap[sl][pml]==0 && hiamap[sl][pmr]==0);//not accept if there is any neighbour(horizontal)
     }
   }
//
//   accept=true;//tempor to switch-off spike algorithm
//
   if(val>lohil[0] && val<lohil[1] && accept){//check "in_limits/not_spike"
     if(nev<ECPCEVMX){//limit statistics(to keep max-stack size small)
       adc[ch][pix][gn]+=val;
       adc2[ch][pix][gn]+=(val*val);
       nevt[ch][pix][gn]+=1;
       for(is=0;is<nstacksz;is++){//try to position val in stack of nstacksz highest max-values
          if(val>adcm[ch][pix][gn][is]){
	    for(i=nstacksz-1;i>is;i--)adcm[ch][pix][gn][i]=adcm[ch][pix][gn][i-1];//move stack -->
	    adcm[ch][pix][gn][is]=val;//store max.val
	    break;
	  }
       }
     }
   }//-->endof "in limits" check
   if(sl==4 && pm==17 && accept){
     if(pix==1 && gn==0)HF1(ECHISTC+190,val,1.);
     if(pix==1 && gn==1)HF1(ECHISTC+191,val,1.);
     if(pix==4 && gn==0)HF1(ECHISTC+192,val,1.);
   } 
 }
//-----
 void ECPedCalib::filltb(integer swid, geant ped, geant sig, int16u sta){
//
   int16u i,sl,pm,pix,gn,ch,is;
// sl=0->, pm=0->, pix=1-3=>anodes,=4->dynode, gn=0/1->hi/low(for dynodes only hi)
// swid=>LTTPG(SupLayer/PMTube/Pixel/Gain)
//
   sl=swid/10000;
   sl=sl-1;//0-8
   pm=(swid%10000)/100-1;//0-35
   ch=ECPMSMX*sl+pm;//seq.# of sl*pm
   pix=(swid%100)/10;
   pix=pix-1;//0-4 (4->dynode)
   gn=(swid%10)-1;//0/1->high/low
   if(pix==4)gn=0;
   peds[ch][pix][gn]=ped;
   sigs[ch][pix][gn]=sig;
   stas[ch][pix][gn]=uinteger(sta);
 }
//-----
 void ECPedCalib::outp(int flg){
// flg=0/1/2=>HistosOnly/write2DB+File/write2file
   int i,j;
   geant pdiff;
   integer statmin(9999);
   int16u sl,pm,pix,gn,gnm,pln,cll,ch;
   time_t begin=BTime();//begin time = 1st_event_time(filled at 1st "ped-block" arrival) 
   uinteger runn=BRun();//1st event run# 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
//   strcpy(DataDate,asctime(localtime(&begin)));
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   integer evs2rem;
//
   cout<<endl;
   cout<<"=====> ECPedCalib-Report:"<<endl<<endl;
   for(sl=0;sl<ECSLMX;sl++){
     for(pm=0;pm<ECPMSMX;pm++){
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){
	   if(nevt[ch][pix][gn]>=ECPCEVMN){//statistics ok
	     evs2rem=floor(ECCAFFKEY.pedcpr*nevt[ch][pix][gn]+0.5);
	     if(evs2rem>nstacksz)evs2rem=nstacksz;
//             if(evs2rem<1)evs2rem=1;
	     for(i=0;i<evs2rem;i++){//remove highest amplitudes
	       adc[ch][pix][gn]=adc[ch][pix][gn]-adcm[ch][pix][gn][i];
	       adc2[ch][pix][gn]=adc2[ch][pix][gn]-adcm[ch][pix][gn][i]*adcm[ch][pix][gn][i];
	     }
	     adc[ch][pix][gn]/=number(nevt[ch][pix][gn]-evs2rem);//truncated average
	     adc2[ch][pix][gn]/=number(nevt[ch][pix][gn]-evs2rem);
	     adc2[ch][pix][gn]=adc2[ch][pix][gn]-adc[ch][pix][gn]*adc[ch][pix][gn];//truncated rms**2
	     if(adc2[ch][pix][gn]>0
	                     && adc2[ch][pix][gn]<=(ECPCSIMX*ECPCSIMX)
		                                    && adc[ch][pix][gn]<300){//chan.OK
	       peds[ch][pix][gn]=geant(adc[ch][pix][gn]);
	       sigs[ch][pix][gn]=geant(sqrt(adc2[ch][pix][gn]));
	       stas[ch][pix][gn]=0;//ok
//update ped-object in memory:
	       if(pix<4){//anodes
	         pdiff=peds[ch][pix][gn]-ECPMPeds::pmpeds[sl][pm].ped(pix,gn);
	         ECPMPeds::pmpeds[sl][pm].ped(pix,gn)=peds[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sig(pix,gn)=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sta(pix,gn)=stas[ch][pix][gn];
		 pln=sl*2+(pix/2);//0-17
		 cll=pm*2+(pix%2);//0-71
		 HF1(ECHISTC+100+2*pln+gn,geant(cll+1),ECPMPeds::pmpeds[sl][pm].ped(pix,gn));
		 HF1(ECHISTC+136+2*pln+gn,geant(cll+1),ECPMPeds::pmpeds[sl][pm].sig(pix,gn));
		 if(gn==0)HF1(ECHISTC+193,sigs[ch][pix][gn],1.);
		 else HF1(ECHISTC+194,sigs[ch][pix][gn],1.);
		 if(gn==0)HF1(ECHISTC+196,pdiff,1.);
		 else HF1(ECHISTC+197,pdiff,1.);
	       }
	       else{//dynodes
	         pdiff=peds[ch][pix][gn]-ECPMPeds::pmpeds[sl][pm].pedd();
	         ECPMPeds::pmpeds[sl][pm].pedd()=peds[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sigd()=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].stad()=stas[ch][pix][gn];
		 HF1(ECHISTC+172+sl,geant(pm+1),ECPMPeds::pmpeds[sl][pm].pedd());
		 HF1(ECHISTC+181+sl,geant(pm+1),ECPMPeds::pmpeds[sl][pm].sigd());
		 HF1(ECHISTC+195,sigs[ch][pix][gn],1.);
		 HF1(ECHISTC+198,pdiff,1.);
	       }
	       if(statmin>nevt[ch][pix][gn])statmin=nevt[ch][pix][gn];
	     }
	     else{//bad chan
	       cout<<"       BadCh:Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	       cout<<"                     Nevents="<<nevt[ch][pix][gn]<<endl;    
	       cout<<"                  ped/sig**2="<<adc[ch][pix][gn]<<" "<<adc2[ch][pix][gn]<<endl;    
	     }
	   }//--->endof "good statistics" check
	   else{
	     cout<<"       LowStatCh:Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	     cout<<"                         Nevents="<<nevt[ch][pix][gn]<<endl;    
	   } 
	 }//--->endof gain-loop
       }//--->endof pixel-loop
     }//--->endof pmt-loop
   }//--->endof Slayer-loop
   cout<<"       MinAcceptableStatistics/channel was:"<<statmin<<endl; 
//   
// ---> prepare update of DB :
   if(flg==1){
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVped());
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     end=begin+86400*30;
     ptdv->SetTime(insert,begin,end);
   }
// ---> write RD default-peds file:
   if(flg==1 || flg==2){
     integer endflab(12345);
     char fname[80];
     char name[80];
     char buf[20];
//
     if(ECREFFKEY.relogic[1]==4)strcpy(name,"eclp_cl");
     if(ECREFFKEY.relogic[1]==5)strcpy(name,"eclp_ds");
     if(AMSJob::gethead()->isMCData())           // for MC-event
     {
       cout <<"       new MC peds-file will be written..."<<endl;
       strcat(name,"_mc.");
     }
     else                                       // for Real events
     {
       cout <<"       new RD peds-file will be written..."<<endl;
       strcat(name,"_rl.");
     }
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
     if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
     strcat(fname,name);
     cout<<"       Open file : "<<fname<<'\n';
     cout<<"       Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- Problems to write new pedestals-file "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write HighGain peds/sigmas/stat:
//
     icfile.width(6);
     icfile.precision(1);// precision
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][0];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write LowGain peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][1];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write Dynode peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][4][0];
         }
         icfile << endl;
     } 
     icfile << endl;
//
// ---> write EndFileLabel :
//
     icfile << endflab;
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   for(i=0;i<99;i++)HPRINT(ECHISTC+100+i);
   cout<<endl;
   cout<<"====================== ECPedCalib: job is completed ! ======================"<<endl;
   cout<<endl;
//
 }
//----------
 void ECPedCalib::outptb(int flg){
// flg=0/1/2=>No/write2DB+file/write2file
   int i,j;
   int totch(0),goodtbch(0),goodch(0);
   geant pedo,sigo;
   int stao;
   geant pdiff;
   int16u sl,pm,pix,gn,gnm,pln,cll,ch;
   time_t begin=BTime();//begin time = 1st_event_time(filled at 1st "ped-block" arrival) 
   uinteger runn=BRun();//1st event run# 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
//   strcpy(DataDate,asctime(localtime(&begin)));
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   cout<<endl;
   cout<<"=====> ECPedCalib:OnBoardTable-Report:"<<endl<<endl;
//---> fill ntuple:
   ECPedCalNT.Run=BRun();
   for(sl=0;sl<ECSLMX;sl++){//<---sup/layer loop
     ECPedCalNT.SLayer=sl+1;
     for(pm=0;pm<ECPMSMX;pm++){//<--- pmt loop
       ECPedCalNT.PmtNo=pm+1;
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){//<--- pixel loop
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){//<--- gain loop
           if(gn==0){
             ECPedCalNT.Pedh[pix]=peds[ch][pix][gn];
             ECPedCalNT.Sigh[pix]=sigs[ch][pix][gn];
             ECPedCalNT.Stah[pix]=uinteger(stas[ch][pix][gn]);
           }
           else{
             ECPedCalNT.Pedh[pix]=peds[ch][pix][gn];
             ECPedCalNT.Sigh[pix]=sigs[ch][pix][gn];
             ECPedCalNT.Stah[pix]=uinteger(stas[ch][pix][gn]);
           }
	 }
       }
       HFNT(IOPA.ntuple);
     }
   }
   cout<<"      <-- Ntuple filled..."<<endl<<endl;
//----
   for(sl=0;sl<ECSLMX;sl++){//<---sup/layer loop
     for(pm=0;pm<ECPMSMX;pm++){//<--- pmt loop
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){//<--- pixel loop
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){//<--- gain loop
	   totch+=1;
	   if(pix<4){//anode
	     pedo=ECPMPeds::pmpeds[sl][pm].ped(pix,gn);
	     sigo=ECPMPeds::pmpeds[sl][pm].sig(pix,gn);
	     stao=ECPMPeds::pmpeds[sl][pm].sta(pix,gn);
	   }
	   else{
	     pedo=ECPMPeds::pmpeds[sl][pm].pedd();
	     sigo=ECPMPeds::pmpeds[sl][pm].sigd();
	     stao=ECPMPeds::pmpeds[sl][pm].stad();
	   }
	   pdiff=peds[ch][pix][gn]-pedo;
//
	   if(peds[ch][pix][gn]>0 && stas[ch][pix][gn]==0){// channel OK in table ? tempor: stas-definition from Kunin ?
	     goodtbch+=1;
	     if(sigs[ch][pix][gn]>0 && sigs[ch][pix][gn]<=ECPCSIMX
		          && peds[ch][pix][gn]<200 && fabs(pdiff)<11){//MyCriteria:chan.OK
	       goodch+=1;
	       if(pix<4){//anodes
	         ECPMPeds::pmpeds[sl][pm].ped(pix,gn)=peds[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sig(pix,gn)=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sta(pix,gn)=stas[ch][pix][gn];
		 pln=sl*2+(pix/2);//0-17
		 cll=pm*2+(pix%2);//0-71
		 HF1(ECHISTC+100+2*pln+gn,geant(cll+1),ECPMPeds::pmpeds[sl][pm].ped(pix,gn));
		 HF1(ECHISTC+136+2*pln+gn,geant(cll+1),ECPMPeds::pmpeds[sl][pm].sig(pix,gn));
		 if(gn==0)HF1(ECHISTC+193,sigs[ch][pix][gn],1.);
		 else HF1(ECHISTC+194,sigs[ch][pix][gn],1.);
		 if(gn==0)HF1(ECHISTC+196,pdiff,1.);
		 else HF1(ECHISTC+197,pdiff,1.);
	       }
	       else{//dynodes
	         ECPMPeds::pmpeds[sl][pm].pedd()=peds[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sigd()=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].stad()=stas[ch][pix][gn];
		 HF1(ECHISTC+172+sl,geant(pm+1),ECPMPeds::pmpeds[sl][pm].pedd());
		 HF1(ECHISTC+181+sl,geant(pm+1),ECPMPeds::pmpeds[sl][pm].sigd());
		 HF1(ECHISTC+195,sigs[ch][pix][gn],1.);
		 HF1(ECHISTC+198,pdiff,1.);
	       }
	     }
	     else{//MyCriteria: bad chan
	       cout<<"       MyCriteriaBadCh: Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	       cout<<"                        ped/sig="<<peds[ch][pix][gn]<<" "<<sigs[ch][pix][gn]<<endl;    
	       cout<<"                        PedDiff="<<pdiff<<endl;    
	     }
	   }//--->endof "channel OK in table ?" check
	   else{
	     cout<<"       BadTableChan: Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	     cout<<"                        ped/sig="<<peds[ch][pix][gn]<<" "<<sigs[ch][pix][gn]<<endl;    
	   } 
	 }//--->endof gain-loop
       }//--->endof pixel-loop
     }//--->endof pmt-loop
   }//--->endof Slayer-loop
//
   cout<<"       BadChannels(Table/My)="<<goodtbch<<" "<<goodch<<" from total="<<totch<<endl;  
//   
// ---> prepare update of DB :
   if(goodch==goodtbch && flg==1){//Update DB "on flight"
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVped());
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     end=begin+86400*30;
     ptdv->SetTime(insert,begin,end);
//
     if(AMSFFKEY.Update==2 ){
       AMSTimeID * offspring = 
         (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());//get 1st timeid instance
       while(offspring){
         if(offspring->UpdateMe())cout << "         Start update Ecal-peds DB "<<*offspring; 
         if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
         cerr <<"         Problem To Update Ecal-peds in DB"<<*offspring;
         offspring=(AMSTimeID*)offspring->next();//get one-by-one
       }
     }
   }
// ---> write OnBoardPedTable to ped-file:
   if(flg==1 || flg==2){
     integer endflab(12345);
     char fname[80];
     char name[80];
     char buf[20];
//
     strcpy(name,"eclp_tb_rl.");//from OnBoardTable
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
     if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
     strcat(fname,name);
     cout<<"       Open file : "<<fname<<'\n';
     cout<<"       Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- ECPedCalib: Problems to write new ONBT-Peds file !!? "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write HighGain peds/sigmas/stat:
//
     icfile.width(6);
     icfile.precision(1);// precision
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][0];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write LowGain peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][1];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write Dynode peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][4][0];
         }
         icfile << endl;
     } 
     icfile << endl;
//
// ---> write EndFileLabel :
//
     icfile << endflab;
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   for(i=0;i<99;i++)HPRINT(ECHISTC+100+i);
   cout<<endl;
   cout<<"====================== ECPedCalib:OnBoardTable job is completed ! ======================"<<endl;
   cout<<endl;
//
 }
//--------------
void ECPedCalib::ntuple_close(){
//
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//ecpedsig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("ecpedsig");
  CLOSEF(IOPA.hlun+1);
//
}
//-----------------------------------------------------------

#include "timeid.h"

// This PED-calibration is used when both (Raw and Compressed) format are
// present event-by-event. The Ped-object in memory can be updated and saved
// into DB and/or into ped-file at the end of the job.

// peds

integer  AMSECIdCalib::_Count[ECPMSMX][ECSLMX][4][3];
integer  AMSECIdCalib::_BadCh[ECPMSMX][ECSLMX][4][3];
number   AMSECIdCalib::_ADC[ECPMSMX][ECSLMX][4][3];
number   AMSECIdCalib::_ADCMax[ECPMSMX][ECSLMX][4][3];
number  AMSECIdCalib::_ADC2[ECPMSMX][ECSLMX][4][3];
time_t AMSECIdCalib::_BeginTime;
uinteger AMSECIdCalib::_CurRun=0;
AMSECIdCalib::ECCalib_def  AMSECIdCalib::ECCALIB;


void AMSECIdCalib::clear(){
/*
for(int i=0;i<ECPMSMX;i++){
  for(int j=0;j<ECSLMX;j++){
   for(int k=0;k<4;k++){
    for(int l=0;l<3;l++){
     _Count[i][j][k][l]=0;
     _ADC[i][j][k][l]=0;
     _ADCMax[i][j][k][l]=0;
     _ADC2[i][j][k][l]=0;
    }
     ECPMPeds::pmpeds[j][i].sta(k,0)=0;
     ECPMPeds::pmpeds[j][i].sta(k,1)=0;
     if(i==1 && j==5){
       ECPMPeds::pmpeds[j][i].sta(k,0)|=AMSDBc::BAD;
       ECPMPeds::pmpeds[j][i].sta(k,1)|=AMSDBc::BAD;
     }
   }
  }
}
*/
}


void AMSECIdCalib::updADC(uinteger adc, uinteger gain){
/*
 if(!adc && gain<1)return;
 if(gain<3){
    (_Count[_pmt][_slay][_pixel][gain])++;
    (_ADC[_pmt][_slay][_pixel][gain])+=adc;
    (_ADC2[_pmt][_slay][_pixel][gain])+=adc*adc;
    if(_ADCMax[_pmt][_slay][_pixel][gain]<adc)_ADCMax[_pmt][_slay][_pixel][gain]=adc;
    HF1(-(gain+1)*10000-makeshortid(),adc-getped(gain),1.);
 }
 else{
  cerr <<"AMSECIdCalib::updADC-S-WrongGain "<<gain<<endl;
 }
*/
}

void AMSECIdCalib::init(){

// clear pedestals
/*
for(int i=0;i<ECPMSMX;i++){
  for(int j=0;j<ECSLMX;j++){
     ECcalib::ecpmcal[j][i].adc2mev()=1;
     ECcalib::ecpmcal[j][i].an2dyr()=30;
     ECcalib::ecpmcal[j][i].pmrgain()=1;
  for(int k=0;k<4;k++){
     ECcalib::ecpmcal[j][i].pmscgain(k)=1;
    for(int l=0;l<2;l++){
       ECPMPeds::pmpeds[j][i].ped(k,l)=4095;
       ECPMPeds::pmpeds[j][i].sta(k,l)=0;
       AMSECIds ids(i,j,k,l);
       if((ids.getlayer()==4 && ids.getcell()==4) ||
          (ids.getlayer()==5 && ids.getcell()==4) ||
          (ids.getlayer()==17 && ids.getcell()==13)){
          ECPMPeds::pmpeds[j][i].sta(k,l)=AMSDBc::BAD;
       }
   }
     if(i==0 && j==0 && k==0 )cout <<" hi2lowr **** "<<ECcalib::ecpmcal[j][i].hi2lowr(k)<<" "<<ECcalib::ecpmcal[j][i].adc2mev()<<" "<<ECcalib::ecpmcal[j][i].an2dyr()<<endl;
     ECcalib::ecpmcal[j][i].hi2lowr(k)=36;
      AMSECIds ids(i,j,k,0);
     if(ids.getlayer()==14 && ids.getcell()==6){
      ECcalib::ecpmcal[j][i].hi2lowr(k)=12;
     }  
    }
  }
}

{
geant gains[18][14];
ifstream fbin;
fbin.open("gains.china");
if(!fbin){
cerr<<"UnableToOpenfile gains.china"<<endl;
abort();
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)fbin >> gains[i][j];
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)cout<< gains[i][j]<<" ";
cout <<endl;
}

   
   for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<7;j++){
              ECcalib::ecpmcal[i][j].pmrgain()=1;
         for (int k=0;k<4;k++){
             AMSECIds ids(i,j,k,0);
           cout <<"  old gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<endl;             
              ECcalib::ecpmcal[i][j].pmscgain(k)=ECcalib::ecpmcal[i][j].pmscgain(k)*gains[ids.getlayer()][ids.getcell()];
           cout <<"  new gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<" "<<ids.getlayer()<<" "<<ids.getcell()<<endl;             
              
         }
       }
      } 

}
{
geant gains[18][14];
ifstream fbin;
fbin.open("h2lr.dat");
if(!fbin){
 cerr<<" Unable to open fbin "<<endl;
 abort();
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)fbin >> gains[i][j];
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)cout<< gains[i][j]<<" ";
cout <<endl;
}

fbin.close();
   
   for(int i=0;i<ecalconst::ECSLMX;i++){
       for(int j=0;j<7;j++){
          for (int k=0;k<4;k++){
             AMSECIds ids(i,j,k,0);
           cout <<"  old hi2lowr "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].hi2lowr(k)<<endl;             
              ECcalib::ecpmcal[i][j].hi2lowr(k)=36./gains[ids.getlayer()][ids.getcell()];
           cout <<"  new hi2lowr "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].hi2lowr(k)<<" "<<ids.getlayer()<<" "<<ids.getcell()<<endl;             
              
         }
       }
      } 

}

    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    char filename[256];
    strcpy(filename,hfile);
    char tmp[80];
    if(_CurRun<1000){
     sprintf(tmp,".0%d",_CurRun);
    }
    else{
     sprintf(tmp,".%d",_CurRun);
    }
//    strcat(filename,tmp);
    integer iostat;
    integer rsize=1024;
    char event[80];  
    HROPEN(IOPA.hlun+1,"ecpedsig",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening ecpedsig ntuple file "<<filename<<endl;
     exit(1);
    }
    else cout <<"ecpedsig ntuple file "<<filename<<" opened."<<endl;
   HBNT(IOPA.ntuple,"EcalPedSigmas"," ");
     for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<7;j++){
         for (int k=0;k<4;k++){
           for( int g=0;g<3;g++){
             AMSECIds ids(i,j,k,g);
             int id=ids.makeshortid(); 
             HBOOK1(-(g+1)*10000-id,"peds",100,-100.,100.,0.);
           }
         }
       }
     }
   HBNAME(IOPA.ntuple,"ECPedSig",(int*)(&ECCALIB),"Run:I,SLayer:I,PMTNo:I,Channel:I,Gain:I, Ped:R,ADCMax:R,Sigma:R,BadCh:I");
*/
}

void AMSECIdCalib::getaverage(){


/*
     int acount=0;
     int bad=0;
     for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<ECPMSMX;j++){
         for (int k=0;k<4;k++){
           for( int g=0;g<3;g++){ 
            if(g==2 && k)continue;
            AMSECIds cid(i,j,k,g);
            AMSECIdCalib id(cid);
            if(id.dead())continue;
            if(id.getcount(g)>1){
              acount++;
              id.setADC(g)=(id.getADC(g)-id.getADCMax(g))/(id.getcount(g)-1);
              id.setADC2(g)=(id.getADC2(g)-id.getADCMax(g)*id.getADCMax(g))/(id.getcount(g)-1);
              id.setADC2(g)=sqrt(id.getADC2(g)-id.getADC(g)*id.getADC(g)); 
              if(id.getADC2(g)>10){
               cerr<<"AMSECIdCalib::getaverage-I-SigmaTooHigh "<<id.getADC2(g)<<" "<<g<<" "<<id.getADC(g)<<" "<<id.getADCMax(g)<<" "<<id.getcount(g)<<" "<<i<<" "<<j<<" "<<k<<endl;
              }
              if(g<2){
                // update pedestals & sigmas here
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].ped(id.getpixel(),g)=id.getADC(g);
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sig(id.getpixel(),g)=id.getADC2(g);
                // update status
                if(id.getADC(g)>4000){
                 ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sta(id.getpixek(),g)|=AMSDBc::BAD;
                 bad++;
                }                
              }
              else{
                // update pedestals & sigmas here
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].pedd()=id.getADC(g);
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sigd()=id.getADC2(g);
              }
             }
             else if(id.getped(g)<0){
                 ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sta(id.getpixel(),g)|=AMSDBc::BAD;
                 bad++;
             }
            }
           }
          }
      }


    cout <<"  AMSECUdCalib::getaverage-I-"<<acount<< " Pedestals/Sigmas Calculated"<<endl;
    cout <<"  AMSECUdCalib::getaverage-I-"<<bad<< " Bad Pedestals/Sigmas Found"<<endl;


   //update db
{
    AMSTimeID *ptdv;
     time_t begin,end,insert;
      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVped());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=_BeginTime;
      end=_BeginTime+86400*30;
      ptdv->SetTime(insert,begin,end);
}
{
    AMSTimeID *ptdv;
     time_t begin,end,insert;
      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVcalib());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=_BeginTime;
      end=_BeginTime+86400*30;
      ptdv->SetTime(insert,begin,end);
}

    if (AMSFFKEY.Update==2 ){
     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_dbendjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }

   // write ntuple


     int count=0;
     for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<ECPMSMX;j++){
         for (int k=0;k<4;k++){
           for( int g=0;g<3;g++){ 
            if(g==2 && k)continue;
            AMSECIds cid(i,j,k,g);
            AMSECIdCalib id(cid);
            if(id.dead() || !id.getcount(g))continue;
            count++;
          ECCALIB.Run=_CurRun;
          ECCALIB.SLayer=i+1;
          ECCALIB.PmtNo=j+1;
          ECCALIB.Channel=k+1;
          ECCALIB.Gain=g+1;
          ECCALIB.Ped=id.getADC(g);
          ECCALIB.ADCMax=id.getADCMax(g);
          ECCALIB.Sigma=id.getADC2(g);
          ECCALIB.BadCh=ECPMPeds::pmpeds[j][i].sta(k,g) & AMSDBc::BAD>0;
          HFNT(IOPA.ntuple);
         }
        }
       }
     }
    cout <<"  AMSECUdCalib::write-I-"<<count<< " Pedestals/Sigmas Written"<<endl;
*/
     }

void AMSECIdCalib::write(){
/*
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//ecpedsig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("ecpedsig");
  CLOSEF(IOPA.hlun+1);
*/

  

}



void AMSECIdCalib::buildSigmaPed(integer n, int16u *p){

/*
  integer ic=AMSEcalRawEvent::checkdaqid(*p)-1;
   
  int leng=0;
  int16u * ptr;
  int count=0; 
  int dynode=0;
  int dead=0;
// first pass :  get total amplitude

  geant sum=0;
  int pmtn;
  int slay;
  int chan;
{
  for(ptr=p+1;ptr<p+n;ptr++){  
   int16u pmt=count%36;
            int16u gain=((*ptr)>>14)&1;
            int16u value=( (*ptr))&4095; 
            int16u anode=(*ptr>>15)& 1;
            int16u channel=((*ptr)>>12)&3;
            if(!anode){
               channel=4;
               gain=2;
            }
            else gain=1-gain;
  
           AMSECIds id(ic,pmt,channel);
           if(!id.dead() && gain==0){
             // high gain only
              if(sum<value-id.getped(gain)){
               chan=id.getchannel();
               pmtn=id.getpmtno();
               slay=id.getslay();
               sum=value-id.getped(gain);
             }
           }
   count++;
  }
//  cout << "sum "<<sum<<endl;
}
  number Threshold=100;
  if(sum<Threshold ){
  count=0;
  for(ptr=p+1;ptr<p+n;ptr++){  
   int16u pmt=count%36;
            int16u anode=(*ptr>>15)& 1;
            int16u channel=((*ptr)>>12)&3;
            int16u gain=((*ptr)>>14)&1;
            int16u value=( (*ptr))&4095; 
            if(!anode){
               channel=4;
               gain=2;
            }
            else gain=1-gain;
  
           AMSECIds id(ic,pmt,channel);
           if(!id.dead()){
             AMSECIdCalib idc(id);
                         
             idc.updADC(value,gain);
            }
 count++;
           }

}
else{
 cout <<"  sum "<<sum<<" "<<pmtn+1<<" "<<slay+1<<" "<<chan+1<<endl;
}
*/
}


void AMSECIdCalib::buildPedDiff(integer n, int16u *p){

/*
  integer ic=AMSEcalRawEvent::checkdaqid(*p)-1;
   
  int leng=0;
  int16u * ptr;
  int count=0; 
  int dynode=0;
  int dead=0;
  for(ptr=p+1;ptr<p+n;ptr++){  
   int16u pmt=count%36;
            int16u anode=(*ptr>>15)& 1;
            int16u channel=((*ptr)>>12)&3;
            int16u gain=((*ptr)>>14)&1;
            int16u value=( (*ptr))&4095; 
            if(!anode){
               channel=4;
               gain=2;
            }
            else gain=1-gain;
  
           AMSECIds id(ic,pmt,channel);
           if(!id.dead()){
             AMSECIdCalib idc(id);
              idc.updADC(uinteger((value-id.getped(gain))*8),gain);
           }
 count++;
}
*/
}
