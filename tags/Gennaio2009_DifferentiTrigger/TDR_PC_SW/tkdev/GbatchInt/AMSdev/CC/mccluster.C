//  $Id: mccluster.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "trid.h"
#include "mccluster.h"
#include "extC.h"
#include "commons.h"
#include <math.h>
#include "job.h"
#include "event.h"
#include "amsstl.h"
#include "cont.h"
#include "ntuple.h"
#include "richid.h"
#include "richdbc.h"
#ifdef __G4AMS__
#include "g4util.h"
#endif
using namespace std;
extern "C" void indetra_();

number AMSEcalMCHit::impoint[2];

integer AMSTRDMCCluster::_NoiseMarker(555);

void AMSTRDMCCluster::sitrdhits(
integer idsoft , geant vect[],geant edep, geant ekin, geant step, integer ipart, integer itra ){


        AMSPoint xgl(vect[0],vect[1],vect[2]);
        AMSDir xvec(vect[3],vect[4],vect[5]);
      AMSEvent::gethead()->addnext(AMSID("AMSTRDMCCluster",0),
      new AMSTRDMCCluster(idsoft,xgl,xvec,step,ekin,edep,ipart,itra));

}

void AMSTRDMCCluster::sitrdnoise(){
}


void AMSTRDMCCluster::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED));
  if(AMSTRDMCCluster::Out(flag)){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TRDMCClusterNtuple* TRDMCClusterN = AMSJob::gethead()->getntuple()->Get_trdclmc();
  
  if (TRDMCClusterN->Ntrdclmc>=MAXTRDCLMC) return;

    TRDMCClusterN->Layer[TRDMCClusterN->Ntrdclmc]=_idsoft.getlayer();
    TRDMCClusterN->Ladder[TRDMCClusterN->Ntrdclmc]=_idsoft.getladder();
    TRDMCClusterN->Tube[TRDMCClusterN->Ntrdclmc]=_idsoft.gettube();
    TRDMCClusterN->Edep[TRDMCClusterN->Ntrdclmc]=_edep;
    TRDMCClusterN->Ekin[TRDMCClusterN->Ntrdclmc]=_ekin;
    TRDMCClusterN->ParticleNo[TRDMCClusterN->Ntrdclmc]=_ipart;
//    TRDMCClusterN->TrackNo[TRDMCClusterN->Ntrdclmc]=_itra;
    for(int i=0;i<3;i++)TRDMCClusterN->Xgl[TRDMCClusterN->Ntrdclmc][i]=_xgl[i];
    TRDMCClusterN->Step[TRDMCClusterN->Ntrdclmc]=_step;
    TRDMCClusterN->Ntrdclmc++;
  }   
}

void AMSTRDMCCluster::_copyEl(){
}

void AMSTRDMCCluster::_printEl(ostream & stream){
stream <<"AMSTRDMCCluster "<<_idsoft<<" "
 <<_edep<<" "<<_ekin<<" "<<_itra<<" "<<_xgl<<endl;
}

void AMSTRDMCCluster::init(){
// at the momenent : homemade trd also for g4

//#ifdef __G4AMS__
//if(MISCFFKEY.G3On){
//#endif
if(TRDMCFFKEY.mode<3 && TRDMCFFKEY.mode>=0){
indetra_();
cout<< "<---- AMSJob::_sitrdinitjob-I-TRDOption "<<TRDMCFFKEY.mode<<" Initialized"<<endl<<endl;
}
else if(TRDMCFFKEY.mode==3){
cerr<< "<---- AMSJob::_sitrdinitjob-F-Option "<<TRDMCFFKEY.mode<<" NotYetImplemented"<<endl<<endl;
exit(1);
}
//#ifdef __G4AMS__
//}
//#endif
}


integer AMSTRDMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTRDMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}



integer AMSTrMCCluster::debug(1);
integer AMSTrMCCluster::_ncha(100);
geant   AMSTrMCCluster::_step(0.05);
integer AMSTrMCCluster::_hid[2]={200103,200104};  
integer AMSTrMCCluster::_NoiseMarker(555);
AMSTrMCCluster::AMSTrMCCluster
(const AMSTrIdGeom & id,integer side,integer nel, number ss[], integer itra){
_next=0;
_idsoft=id.cmpt();
_xca=0;
_xcb=0;
_xgl=-1000;
_itra=itra;
_sum=0;
if(nel>5)nel=5;
VZERO(_ss,10*sizeof(geant)/4);
 integer cside= side==0?1:0;
_left[side]=max(0,id.getstrip(side)-nel/2); 
_right[cside]=0; 
_right[side]=min(id.getstrip(side)+nel/2,id.getmaxstrips(side)-1); 
_left[cside]=1;
_center[side]=id.getstrip(side);
_center[cside]=0;
for(int i=_left[side];i<_right[side]+1;i++)
 _ss[side][i-_left[side]]=ss[i+nel-_right[side]-1];
if(side==1){
 number cofgy=id.strip2size(1);
 number cofgx=0;
          AMSgSen *p=
          (AMSgSen*)AMSJob::gethead()->getgeomvolume(id.crgid());
 if(p){
  _xgl=p->str2pnt(cofgx,cofgy);
 }
}
}

void AMSTrMCCluster::addcontent(const AMSTrIdSoft &  idl, geant *adc){
integer i;
integer side=idl.getside();
if(side==0){
   for(i=_left[side];i<=_right[side];i++){
    AMSTrIdSoft id(AMSTrIdGeom(_idsoft,i,0),side);
    if(idl.getaddr() != id.getaddr())return;
    if(id.dead())continue; 
    adc[id.getstrip()]=  adc[id.getstrip()]+_ss[side][i-_left[side]];
   }
}  
else {
   for(i=_left[side];i<=_right[side];i++){
    AMSTrIdSoft id(AMSTrIdGeom(_idsoft,0,i),side);
    if(idl.getaddr() != id.getaddr()  )return;
    if(id.dead())continue; 
    adc[id.getstrip()]=  adc[id.getstrip()]+_ss[side][i-_left[side]];
   }
}
}

void AMSTrMCCluster::addcontent(char xy, geant ** p, integer noise){
 integer i;
 if(noise && _itra!=_NoiseMarker)return;
 if(xy=='x'){
     for(i=_left[0];i<=_right[0];i++){
     AMSTrIdSoft id(AMSTrIdGeom(_idsoft,i,0),0);
     if(id.dead()){
      continue;
     }
     if(p[id.getaddr()]==0){
      integer isize=sizeof(geant)*id.getmaxstrips();
      p[id.getaddr()]=(geant*)UPool.insert(isize);
      if(p[id.getaddr()]==0){
        cerr <<" AMSTrMCCLUSTER-S-Memory Alloc Failure"<<endl;
        return;
      }
      VZERO(p[id.getaddr()],isize/4);
     }
     if(_itra!=_NoiseMarker)(*(p[id.getaddr()]+id.getstrip()))+= _ss[0][i-_left[0]];
     else if(noise)(*(p[id.getaddr()]+id.getstrip()))=_ss[0][i-_left[0]];
     //    cout <<id<<" side 0 adc + "<<id.getstrip()<<" "<<_ss[0][i-_left[0]]<< endl;
    }
 }
 else{
   for(i=_left[1];i<=_right[1];i++){
    AMSTrIdSoft id(AMSTrIdGeom(_idsoft,0,i),1);
    if(id.dead()){
     continue;
    }
    if(p[id.getaddr()]==0){
     integer isize=sizeof(geant)*id.getmaxstrips();
     p[id.getaddr()]=(geant*)UPool.insert(isize);
     if(p[id.getaddr()]==0){
       cerr <<" AMSTrMCCLUSTER-S-Memory Alloc Failure"<<endl;
       return;
     }
     VZERO(p[id.getaddr()],isize/4);
    }
     if(_itra!=_NoiseMarker)(*(p[id.getaddr()]+id.getstrip()))+= _ss[1][i-_left[1]];
     else if(noise) (*(p[id.getaddr()]+id.getstrip()))=_ss[1][i-_left[1]];
    //    cout <<id <<" side 1 adc + "<<id.getstrip()<<" "<<_ss[1][i-_left[1]]<< endl;
   }
 }
}

void AMSTrMCCluster::_shower(){
  //  cout <<"*** start shower " <<endl;
  AMSgObj::BookTimer.start("TrMCCluster");
  //   Non active silicon as geant volume introduced ; skip some lines below
  //
  //  // As we have no non-active silicon as geant volume 
  //  // we should do something manually in case we are in it;
  //  // Very crude method (but fast)
  //  // gives some overestimation of dead space by ~100-200 mkm == ~10-20%
  //     number sa[2],sb[2];
  //     if((_xca[0]<0 || _xca[0]>id.getmaxsize(0)) ||
  //        (_xca[1]<0 || _xca[1]>id.getmaxsize(1)) ||
  //        (_xcb[0]<0 || _xcb[0]>id.getmaxsize(0)) ||
  //        (_xcb[1]<0 || _xcb[1]>id.getmaxsize(1))   )_sum=0; 
  //
  //  // End active check
  //  //

AMSTrIdGeom id(_idsoft);
AMSPoint entry=(_xca+_xcb)/2.;
AMSPoint dentry=(_xcb-_xca)/2;
AMSDir dir=_xcb-_xca;
geant cofg=0;
geant esumx=0;
geant esumy=0;
integer ierr;
integer i;
geant xpr=_sum*TRMCFFKEY.dedx2nprel;
for(i=0;i<_nel;i++)_ss[0][i]=0;
for(i=0;i<_nel;i++)_ss[1][i]=0;
_center[0]=id.size2strip(0,entry[0]);
_center[1]=id.size2strip(1,entry[1]);
_left[0]= max(0,_center[0]-(_nel/2));
_left[1]= max(0,_center[1]-(_nel/2));
_right[0]=min(id.getmaxstrips(0)-1,_center[0]+(_nel/2));
_right[1]=min(id.getmaxstrips(1)-1,_center[1]+(_nel/2));
if(xpr >0.){
for (integer k=0;k<2;k++){
  if(fabs(dentry[k])/(xpr)<TRMCFFKEY.fastswitch){
    // fast algo
     id.upd(k,_center[k]);  // update geom id
     number e=entry[k]-id.strip2size(k);
     number s=id.strip2size(k);
     for (int ii=_left[k];ii<=_right[k];ii++){
       number a1,a2;
       id.upd(k,ii);  // update geom id
       a1=id.strip2size(k)-s;
       a2=a1+id.getsize(k);
       number r=a2-a1;
      _ss[k][ii-_left[k]]=TRMCFFKEY.delta[k]*xpr*sitkfint2_(a1,a2,e,dentry[k],r);
     }     
  }
  else{
    //slow algo
    for(int kk=0;kk<floor(xpr);kk++){
     geant e,dummy=0;
     e=entry[k]-dentry[k]+2*RNDM(dummy)*dentry[k];
     number a1,a2;
     for (int ii=_left[k];ii<=_right[k];ii++){
       id.upd(k,ii);  // update geom id
       a1=id.strip2size(k)-e;
       a2=a1+id.getsize(k);
       _ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]+TRMCFFKEY.delta[k]*sitkfints_(a1,a2);
//       _ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]+TRMCFFKEY.delta[k]*sitkfint2_(a1,a2,0.,0.,1.);
     }
    }
  }
  integer nedx;
  for(int ii=_left[k];ii<=_right[k];ii++){
   _ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]*(1+rnormx()*TRMCFFKEY.gammaA[k]);
  }
}
}

  AMSgObj::BookTimer.stop("TrMCCluster");
  // cout <<"** end shower"<<endl;
}




void AMSTrMCCluster::_printEl(ostream & stream){
   AMSTrIdGeom id(_idsoft);
   stream << "AMSTrMCCluster-Shower x "<<_itra<<" "<<id.getlayer()<<" "
   <<_ss[0][0]<<" "<<_ss[0][1]<<" "<<_ss[0][2]<<" "
   <<_ss[0][3]<<" "<<_ss[0][4]<<endl;
   stream << "AMSTrMCCluster-Shower y "<<_itra<<" "<<id.getlayer()<<" "
   <<_ss[1][0]<<" "<<_ss[1][1]<<" "<<_ss[1][2]<<" "
   <<_ss[1][3]<<" "<<_ss[1][4]<<endl;
   stream << "AMSTrMCCluster-Coo "<<_itra<<" "<<id.getlayer()<<" "
   <<_xca<<" "<<_xcb<<" "<<_xgl<<endl;
}

void AMSTrMCCluster::_copyEl(){
}

void AMSTrMCCluster::_writeEl(){

  integer flag =    (IOPA.WriteAll%10==1)
                 || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED))
                 || (IOPA.WriteAll%10==2 && !checkstatus(AMSDBc::AwayTOF));

  
  if(AMSTrMCCluster::Out(flag)){
    int i;
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TrMCClusterNtuple* TrMCClusterN = AMSJob::gethead()->getntuple()->Get_trclmc();
  
  if (TrMCClusterN->Ntrclmc>=MAXTRCLMC) return;

// Fill the ntuple
    TrMCClusterN->Idsoft[TrMCClusterN->Ntrclmc]=_idsoft;
    TrMCClusterN->TrackNo[TrMCClusterN->Ntrclmc]=_itra;
    for(i=0;i<2;i++)TrMCClusterN->Left[TrMCClusterN->Ntrclmc][i]=_left[i];
    for(i=0;i<2;i++)TrMCClusterN->Center[TrMCClusterN->Ntrclmc][i]=_center[i];
    for(i=0;i<2;i++)TrMCClusterN->Right[TrMCClusterN->Ntrclmc][i]=_right[i];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][0]=_ss[i][0];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][1]=_ss[i][1];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][2]=_ss[i][2];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][3]=_ss[i][3];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][4]=_ss[i][4];
    for(i=0;i<3;i++)TrMCClusterN->Xca[TrMCClusterN->Ntrclmc][i]=_xca[i];
    for(i=0;i<3;i++)TrMCClusterN->Xcb[TrMCClusterN->Ntrclmc][i]=_xcb[i];
    for(i=0;i<3;i++)TrMCClusterN->Xgl[TrMCClusterN->Ntrclmc][i]=_xgl[i];
    TrMCClusterN->Sum[TrMCClusterN->Ntrclmc]=_sum;
/*
    cout <<" *** start *** "<<getpos()<<endl;
    cout <<TrMCClusterN->Idsoft[TrMCClusterN->Ntrclmc]<<" " <<_idsoft<<endl;
    cout <<TrMCClusterN->TrackNo[TrMCClusterN->Ntrclmc]<<" " <<_itra<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->Left[TrMCClusterN->Ntrclmc][i]<<" " <<_left[i]<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->Center[TrMCClusterN->Ntrclmc][i]<<" " <<_center[i]<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->Right[TrMCClusterN->Ntrclmc][i]<<" " <<_right[i]<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][0]<<" " <<_ss[i][0]<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][1]<<" " <<_ss[i][1]<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][2]<<" " <<_ss[i][2]<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][3]<<" " <<_ss[i][3]<<endl;
    for(i=0;i<2;i++)cout <<TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][4]<<" " <<_ss[i][4]<<endl;
    for(i=0;i<3;i++)cout <<TrMCClusterN->Xca[TrMCClusterN->Ntrclmc][i]<<" " <<_xca[i]<<endl;
    for(i=0;i<3;i++)cout <<TrMCClusterN->Xcb[TrMCClusterN->Ntrclmc][i]<<" " <<_xcb[i]<<endl;
    for(i=0;i<3;i++)cout <<TrMCClusterN->Xgl[TrMCClusterN->Ntrclmc][i]<<" " <<_xgl[i]<<endl;
    cout <<TrMCClusterN->Sum[TrMCClusterN->Ntrclmc]<<" " <<_sum<<endl;
*/
    TrMCClusterN->Ntrclmc++;
  }

}

number AMSTrMCCluster::sitknoiseprob(const AMSTrIdSoft & id, number threshold){
number z;
number sigma=id.getsig();
if(sigma >0)z=threshold/sigma/sqrt(2.);
else z=FLT_MAX;
return DERFC(z)/2;  

}

number AMSTrMCCluster::sitknoiseprobU(number threshold, number step){
 return 0.5*(DERFC(threshold)-DERFC(threshold+step));  
}

void  AMSTrMCCluster::sitknoisespectrum(const AMSTrIdSoft & id, number ss[],
                                        number prob){
   geant d=0;
   if(RNDM(d)>=prob){
#ifdef __G4AMS__
if(MISCFFKEY.G4On)ss[0]=id.getsig()*AMSRandGeneral::hrndm1(hid(id.getside()));
#else
     ss[0]=id.getsig()*HRNDM1(hid(id.getside()));
#endif
   }
   else  ss[0]=id.getsig()*rnormx();
}


void AMSTrMCCluster::sitkhits(
integer idsoft , geant vect[],geant edep, geant step, integer itra ){
  AMSgObj::BookTimer.start("SITKHITS");
 AMSPoint pa(vect[0],vect[1],vect[2]);
 AMSPoint dirg(vect[3],vect[4],vect[5]);
 AMSPoint pb=pa-dirg*step;
 AMSPoint pgl=pa-dirg*step/2;
 AMSgSen *p=(AMSgSen*)AMSJob::gethead()->getgeomvolume(
  AMSTrIdGeom(idsoft).crgid());
 if(p){
  integer ilay=AMSTrIdGeom(p->getid()).getlayer();
#ifdef __AMSDEBUG__
  //  cout <<"Sitkhits - "<<ilay<<" "<<pgl<<endl;
#endif
 
 if(p->getsenstrip(pa) && p->getsenstrip(pb)){
      AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0),
      new AMSTrMCCluster(idsoft,pa,pb,pgl,edep,itra));
 }
 }
 else{
   cerr << "sitkhits Error" << idsoft <<" "
        <<AMSTrIdGeom(idsoft).crgid()<<endl;
 }
 AMSgObj::BookTimer.stop("SITKHITS");
}




void AMSTrMCCluster::sitknoise(){
  // Only add noise when not in raw mode
  AMSgObj::BookTimer.start("SITKNOISE");
   geant dummy=0;
   number noise,oldone=0;
   integer itra=_NoiseMarker;
   number ss[5];
   number const probthr=0.05;
   float totn[2]={0,0};
   for(int l=0;l<2;l++){
     for (int i=0;i<TKDBc::nlay();i++){
       for (int j=0;j<TKDBc::nlad(i+1);j++){
         for (int s=0;s<2;s++){
           if(TKDBc::activeladdshuttle(i+1,j+1,s)){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
             if(!id.dead()){
              int icmpt=id.gettdr();
             if(TRMCFFKEY.RawModeOn[id.getcrate()][id.getside()][icmpt])continue;
            id.upd(id.getmaxstrips()-1);
            geant r=RNDM(dummy);
            if(r<id.getindnoise()){
              //bsearch
             id.upd(0);
             while(id.getprob(r) <id.getmaxstrips()){
              r=RNDM(dummy);

              totn[l]=totn[l]+1;
              AMSTrIdSoft idx,idy;
              if(l==0){
                  idx=id;
                  idy=AMSTrIdSoft(i+1,j+1,s,1,0);
              }
              else {
                  idy=id;
                  idx=AMSTrIdSoft(i+1,j+1,s,0,0);
              }
              integer nambig;
              AMSTrIdGeom *pid=idx.ambig(idy,nambig);
              if(pid){
                 // calculate prob to avoid double counitng
                 number prob=(1+TRMCFFKEY.NonGaussianPart[l])*
                 exp(-id.getmaxstrips()*
                 sitknoiseprob(id,TRMCFFKEY.thr1R[l]*id.getsig()));

                 sitknoisespectrum(id,ss,1-prob);
                 HF1(200105+id.getside(),ss[0],1.);
                  AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0),
                  new AMSTrMCCluster(*pid,id.getside(),1,ss,itra));
              }
             }
            }
           }
          }
         }
       }

     }
       HF1(200101+l,totn[l],1.);

   }

   AMSgObj::BookTimer.stop("SITKNOISE");

}

void  AMSTrMCCluster::sitkcrosstalk(){
}


//------------- TOF -------------


void AMSTOFMCCluster::sitofhits(integer idsoft , geant vect[],geant edep, 
geant tofg){
  //        Very Temporary
  AMSPoint pnt(vect[0],vect[1],vect[2]);
   AMSEvent::gethead()->addnext(AMSID("AMSTOFMCCluster",0),
   new AMSTOFMCCluster(idsoft,pnt,edep,tofg));
}

//------------- ANTI -------------

void AMSAntiMCCluster::siantihits(integer idsoft , geant vect[],geant edep, 
geant tofg){
  //        Very Temporary
  AMSPoint pnt(vect[0],vect[1],vect[2]);
   AMSEvent::gethead()->addnext(AMSID("AMSAntiMCCluster",0),
   new AMSAntiMCCluster(idsoft,pnt,edep,tofg));
}

//------------- ECAL --------------

void AMSEcalMCHit::siecalhits(integer idsoft , geant vect[],geant edep, 
                                                           geant tofg){
//     
  AMSPoint pnt(vect[0],vect[1],vect[2]);
  int isl;
//
  isl=idsoft/100000;//s-layer(1-9)  
  AMSEvent::gethead()->addnext(AMSID("AMSEcalMCHit",isl-1),
                      new AMSEcalMCHit(idsoft,pnt,edep,tofg));
  //cout <<isl<<" "<<pnt<<endl;
}
void AMSEcalMCHit::_writeEl(){
}


//--------RICH------------

void AMSRichMCHit::sirichhits(integer id,
			      integer pmt, geant position[], // used to compute channel
			      geant origin[],geant momentum[],integer status)
{
  AMSPoint r(origin[0],origin[1],origin[2]);
  double norma=sqrt(momentum[0]*momentum[0]+
		    momentum[1]*momentum[1]+
		    momentum[2]*momentum[2]);
  AMSPoint u(momentum[0]/norma,momentum[1]/norma,momentum[2]/norma);
  geant adc;

  adc=0;
 

 if(pmt>RICHDB::total || pmt<0){
    cerr<<"AMSRichMCHit::sirichhits-ErrorNoPMT " << pmt<<endl;
    return;
  }

  
  AMSRICHIdGeom channel(pmt,position[0],position[1]);

#ifdef __AMSDEBUG__
/*
  cout <<"Hit position "<<position[0]<<","<<position[1]<<endl
       <<"PMT number "<<pmt<<endl
       <<"RICHDB position "<<RICHDB::pmt_p[pmt][0]<<" "<<RICHDB::pmt_p[pmt][1]<<endl
       <<"RICHIdGeom pos "<<AMSRICHIdGeom::pmt_pos(pmt,0)<<" "<<AMSRICHIdGeom::pmt_pos(pmt,1)<<endl
       <<"Asigned channel "<<channel.getchannel()<<endl
       <<"Asigned pixel "<<channel.getpixel()<<endl
       <<"Asigned position "<<channel.x()<<","<<channel.y()<<endl;
*/
#endif

  // The primary history does not has good channel info so we do it by hand

  if(status==Status_primary_rad ||
     status==Status_primary_tracker ||
     status==Status_primary_tof ||
     status==Status_primary_radb)
    AMSEvent::gethead()->addnext(AMSID("AMSRichMCHit",0),
				 new AMSRichMCHit(id,-1,0,
						  r,u,status));
  
  
  else
    
    
    if(channel.getchannel()>=0){
      AMSEvent::gethead()->addnext(AMSID("AMSRichMCHit",0),
				   new AMSRichMCHit(id,channel.getchannel(),adc,
						    r,u,status));
}
}


void AMSRichMCHit::noisyhit(integer channel,int mode){
  AMSPoint r(0,0,0);
  AMSPoint u(0,0,0);
  AMSEvent::gethead()->addnext(AMSID("AMSRichMCHit",0),
			       new AMSRichMCHit(Noise,channel,AMSRichMCHit::noise(channel,mode),r,u,Status_Noise));
}


geant AMSRichMCHit::adc_hit(integer n,integer channel,int mode){ // ADC counts for a set of hits
  geant u1,u2,dummy,r;
  AMSRICHIdSoft calibration(channel);

  /*
  //  r=sqrt(-2.*log(1.-RNDM(dummy)));
  //  u1=r*sin(RNDM(dummy)*6.28318595886);   // This constant (2pi)should be moved to a namespace


  return rnormx()*calibration.getsgain(mode)*sqrt(double(n))+n*calibration.getgain(mode);
  */
  geant value=0;
  for(int i=0;i<n;i++){  // Start adding hits
    value+=calibration.simulate_single_pe(mode);
  }
  return value;


}

geant AMSRichMCHit::adc_empty(integer channel,integer mode){ // ADC count without a hit
  geant u1,u2,dummy,r;
  AMSRICHIdSoft calibration(channel);


  //  r=sqrt(-2.*log(1.-RNDM(dummy)));
  //  u1=r*sin(RNDM(dummy)*6.28318595886);

  
  return rnormx()*calibration.getsped(mode)+calibration.getped(mode);
}


geant AMSRichMCHit::noise(int channel,integer mode){ // ADC counts above the pedestal
  AMSgObj::BookTimer.start("SIRINoise");
  
  geant u1,u2,dummy,r;

  AMSRICHIdSoft current(channel);
  
  int loops=0;
  do{
    float limit=int(current.getthreshold(mode)*current.getsped(mode))+1;
    r=sqrt(limit*limit-2.*log(1.-RNDM(dummy)));
    geant par=RNDM(dummy)*6.28318595886;
    u1=r*sin(par);
    if(u1<current.getthreshold(mode)*current.getsped(mode))
      u1=r*cos(par);
    loops++;

#ifdef __AMSDEBUG__
    if(loops>10){
      cout <<"Looping..."<<loops<<endl;
    }
#endif


    if(loops>100){
      cout<<"AMSRichMCHit::noise--too many loops"<<endl; 
      AMSgObj::BookTimer.stop("SIRINoise");
      return current.getthreshold(mode)*current.getsped(mode)+current.getped(mode)+1.;
    }
  }while(integer(u1+current.getped(mode))<integer(current.getthreshold(mode)*current.getsped(mode)+current.getped(mode)));

  
  //  do u1=current.getped(mode)+current.getsped(mode)*rnormx(); 
  //  while(integer(u1)<=integer(current.getthreshold(1)*current.getsped(mode)+current.getped(mode)));
  //  return u1;

  //  return u1*current.getsped(mode)+current.getped(mode);

#ifdef __AMSDEBUG__
  cout <<"Returning "<<u1<<" limit at "<<current.getsped(mode)*current.getthreshold(mode) <<endl;
#endif

  AMSgObj::BookTimer.stop("SIRINoise");

  return u1+current.getped(mode);
}



void AMSRichMCHit::_writeEl(){

  RICMCNtuple* cluster=AMSJob::gethead()->getntuple()->Get_richmc();

  if(cluster->NMC>=MAXRICMC) return; 

  if(_status==Status_Fake) return; // Fake hit
// Here we need a flag with the IOPA to write it or not
#ifdef __WRITEROOT__
     int numgen = 0;
     if(cluster->NMC==0) numgen = RICHDB::nphgen;
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this,numgen);
#endif
  if(cluster->NMC==0)
    cluster->numgen=RICHDB::nphgen;
  cluster->id[cluster->NMC]=_id;
  cluster->origin[cluster->NMC][0]=_origin[0];
  cluster->origin[cluster->NMC][1]=_origin[1];
  cluster->origin[cluster->NMC][2]=_origin[2];
  cluster->direction[cluster->NMC][0]=_direction[0];
  cluster->direction[cluster->NMC][1]=_direction[1];
  cluster->direction[cluster->NMC][2]=_direction[2];
  cluster->status[cluster->NMC]=_status;
  cluster->eventpointer[cluster->NMC]=_hit;

  cluster->NMC++;



}

//--------END--------------


void AMSTOFMCCluster::_writeEl(){

  if(AMSTOFMCCluster::Out( IOPA.WriteAll%10==1)){
#ifdef __WRITEROOT__
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TOFMCClusterNtuple* TOFMCClusterN = AMSJob::gethead()->getntuple()->Get_tofmc();

  if (TOFMCClusterN->Ntofmc>=MAXTOFMC) return;
  
// Fill the ntuple
    TOFMCClusterN->Idsoft[TOFMCClusterN->Ntofmc]=idsoft;
    for(int i=0;i<3;i++)TOFMCClusterN->Coo[TOFMCClusterN->Ntofmc][i]=xcoo[i];
    TOFMCClusterN->TOF[TOFMCClusterN->Ntofmc]=tof;
    TOFMCClusterN->Edep[TOFMCClusterN->Ntofmc]=edep;
    TOFMCClusterN->Ntofmc++;
  }

}

void AMSAntiMCCluster::_writeEl(){


  if(AMSAntiMCCluster::Out( IOPA.WriteAll%10==1)){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// fill the ntuple
  ANTIMCClusterNtuple* AntiMCClusterN = AMSJob::gethead()->getntuple()->Get_antimc();
    if (AntiMCClusterN->Nantimc>=MAXANTIMC) return;
    AntiMCClusterN->Idsoft[AntiMCClusterN->Nantimc]=_idsoft;
    for(int i=0;i<3;i++)AntiMCClusterN->Coo[AntiMCClusterN->Nantimc][i]=_xcoo[i];
    AntiMCClusterN->TOF[AntiMCClusterN->Nantimc]=_tof;
    AntiMCClusterN->Edep[AntiMCClusterN->Nantimc]=_edep;
    AntiMCClusterN->Nantimc++;
  }
}





integer AMSTrMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTrMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}

integer AMSTOFMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}




integer AMSAntiMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSAntiMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}




// write some trmc cluster to daq files
//only itra + xgl
// V. Choutko 8-jul-1999


integer AMSTrMCCluster::checkdaqid(int16u id){
if(id==getdaqid())return 1;
else return 0;
}

void AMSTrMCCluster::builddaq(integer i, integer length, int16u *p){
  AMSTrMCCluster *ptr=(AMSTrMCCluster*)AMSEvent::gethead()->
  getheadC("AMSTrMCCluster",0);
 *p=getdaqid();
while(ptr){ 
 const uinteger c=65535;
 *(p+1)=ptr->_itra;
 integer big=10000;
 for(int k=0;k<3;k++){
 if(ptr->_xgl[k]+big<=0){
  goto metka;
 }
 uinteger cd=(ptr->_xgl[k]+big)*10000;
 *(p+3+2*k)=int16u(cd&c);
 *(p+2+2*k)=int16u((cd>>16)&c);
 }
 p+=7;
metka:
 ptr=ptr->next();
}

}


void AMSTrMCCluster::buildraw(integer n, int16u *p){
 integer ip;
 geant mom;
 for(int16u *ptr=p+1;ptr<p+n;ptr+=7){ 
  ip=*(ptr);
  uinteger cdx=  (*(ptr+2)) | (*(ptr+1))<<16;  
  uinteger cdy=  (*(ptr+4)) | (*(ptr+3))<<16;  
  uinteger cdz=  (*(ptr+6)) | (*(ptr+5))<<16;  
  AMSPoint coo(cdx/10000.-10000.,cdy/10000.-10000.,cdz/10000.-10000.);
 //  cout <<ip<<" "<<coo<<endl;
        AMSTrMCCluster *ptrhit=(AMSTrMCCluster*)AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0), new
        AMSTrMCCluster(coo,ip));
        if(ptrhit->IsNoise())ptrhit->setstatus(AMSDBc::AwayTOF);

}

} 

integer AMSTrMCCluster::calcdaqlength(integer i){
 AMSContainer *p = AMSEvent::gethead()->getC("AMSTrMCCluster");
 int len=1; 
 if(p){
  AMSTrMCCluster *ptr=(AMSTrMCCluster*)AMSEvent::gethead()->
  getheadC("AMSTrMCCluster",0);
 while(ptr){ 
  const integer big=10000;
  if(ptr->_xgl[0]+big>0 && ptr->_xgl[1]+big>0 && ptr->_xgl[2]+big>0)len+=7; 
  ptr=ptr->next();
 }
 }
 return len;
}



