//  $Id: trigger302.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "tofdbc02.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "trigger102.h"
#include "trigger302.h"
#include "ecaldbc.h"
#include "ecalrec.h"
#include "event.h"
#include "mccluster.h"
#include "amsdbc.h"
#include "trrawcluster.h"
#include "trdsim.h"
#include "ntuple.h"
using namespace std;
using namespace trconst;
using namespace trigger302const;
using namespace ecalconst;
void TriggerAuxLVL302::addnoisetk(integer crate){




  if(!AMSJob::gethead()->isReconstruction()){
  // AddNoise first ( a bit conservative)
  int i,j;
  integer ierr=0;
  integer nn=0;
  int16u drp,strip,va,side;
  geant d=0;
  for(i=0;i<2;i++){
   geant xn=LVL3SIMFFKEY.NoiseProb[i]*NTRHDRP/2*TKDBc::NStripsDrp(1,i);
   POISSN(xn,nn,ierr);
   for(j=0;j<nn;j++){
    side=i;
    drp=RNDM(d)*NTRHDRP;
    if(drp >= NTRHDRP)drp=NTRHDRP-1;
    strip=RNDM(d)*TKDBc::NStripsDrp(1,i);
    if(strip >= TKDBc::NStripsDrp(1,i))strip=TKDBc::NStripsDrp(1,i)-1;
    AMSTrIdSoft idd(crate,drp,i,strip);
    if(!idd.dead()){ 
     if(maxtr-_ltr > 4){
      _ptr[_ltr]=1;   // length + max strp
      _ptr[_ltr+1]=idd.gethaddr();
      _ptr[_ltr+2]=1000;
      _ptr[_ltr+3]=1000;
      _ltr+=4;
     }    
     else return;
    }
    }  
  }
}
}
void TriggerAuxLVL302::filltk(integer crate){


  //
  // fillTracker
  //

   
   AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
   getheadC("AMSTrRawCluster",crate);
   while (ptr){
    _ltr+=ptr->lvl3format(_ptr+_ltr,maxtr-_ltr);
    ptr=ptr->next();
   }

}


void TriggerAuxLVL302::filltrd(integer crate){

   
  //
  // fillTrd
  //
 
   AMSTRDRawHit::HardWareCompatibilityMode()=true;
   AMSTRDRawHit *ptr=(AMSTRDRawHit*)AMSEvent::gethead()->
   getheadC("AMSTRDRawHit",crate,2);
   while (ptr){
    _ltr+=ptr->lvl3format(_ptr+_ltr,maxtr-_ltr);
    //cout <<" filling TRD .....################## "<<crate<<" "<<_ltr<<" "<<_ptr[_ltr-2]<<AMSTRDIdSoft(crate,_ptr[_ltr-2])<<endl;
    ptr=ptr->next();
   }
//   cout <<"  ltr "<<_ltr<<endl;
   AMSTRDRawHit::HardWareCompatibilityMode()=false;

}
//------------------------------------------------
void TriggerAuxLVL302::filltof(){
//
// fill TOF
//
  TOF2RawSide *ptr;
//
  ptr=(TOF2RawSide*)AMSEvent::gethead()
                        ->getheadC("TOF2RawSide",0,1);//last 1 to sort
//(for MC i use "daq-decoded" class TOF2RawSide as input)  
  while(ptr){
    _ltr+=ptr->lvl3format(_ptr+_ltr,maxtr-_ltr);//filling TriggerAuxLVL3
    ptr=ptr->next();
//
  }
}
//
//---
//
int16 *  TriggerAuxLVL302::readtof(integer begin){
  if(begin)_ctr=0;
  else if (_ctr < _ltr)_ctr=_ctr+3;
  return _ctr < _ltr ? _ptr+_ctr : 0;
}
//------------------------------------------------
void TriggerAuxLVL302::fillecal(){
//
// fill ECAL
//
  AMSEcalRawEvent *ptr;
//
  for(int isl=0;isl<AMSECIds::ncrates();isl++){ // <-------------- crates loop
    ptr=(AMSEcalRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSEcalRawEvent",isl,0);
//(for MC i use "daq-decoded" class AMSEcalRawEvent as input)  
    while(ptr){
      _ltr+=ptr->lvl3format(_ptr+_ltr,maxtr-_ltr);//filling TriggerAuxLVL3
      ptr=ptr->next();
//
    }
  } // ---> end of cr loop
}
//------------
//
int16 *  TriggerAuxLVL302::readecal(integer begin){
  if(begin)_ctr=0;
  else if (_ctr < _ltr)_ctr=_ctr+3;
  return _ctr < _ltr ? _ptr+_ctr : 0;
}
//------------------------------------------------


TriggerAuxLVL302::TriggerAuxLVL302(){
_ltr=0;
_ctr=0;
_ptr[0]=0;

}


int16 *  TriggerAuxLVL302::readtracker(integer begin){
if(begin)_ctr=0;
else if (_ctr < _ltr)_ctr=_ctr+(((_ptr[_ctr])&63)+3);
return _ctr < _ltr ? _ptr+_ctr : 0;
}

int16 *  TriggerAuxLVL302::readtrd(integer begin){
if(begin)_ctr=0;
else if (_ctr < _ltr)_ctr=_ctr+2;
return _ctr < _ltr ? _ptr+_ctr : 0;
}

  integer TriggerLVL302::_TOFPattern[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  integer TriggerLVL302::_TOFStatus[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  geant TriggerLVL302::_TOFTzero[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  integer TriggerLVL302::_TOFOr[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  integer TriggerLVL302::_TrackerStatus[NTRHDRP2];
  integer TriggerLVL302::_TrackerAux[NTRHDRP][trid::ncrt];
  integer TriggerLVL302::_TOFAux[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  integer TriggerLVL302::_NTOF[TOF2GC::SCLRS];
  geant TriggerLVL302::_TOFCoo[TOF2GC::SCLRS][TOF2GC::SCMXBR][3];
  geant TriggerLVL302::_TrackerCoo[NTRHDRP][trid::ncrt][3];
 geant TriggerLVL302::_TrackerDir[NTRHDRP][trid::ncrt];
  geant TriggerLVL302::_TrackerCooZ[maxlay];
  integer TriggerLVL302::_TrackerDRP2Layer[NTRHDRP][trid::ncrt];
  number TriggerLVL302::_stripsize=0.0055;
integer TriggerLVL302::_TrackerOtherTDR[NTRHDRP][trid::ncrt];
//
  integer TriggerLVL302::_flowc[15];

// ECAL
  geant TriggerLVL302::_ECgains[ECSLMX][ECPMSMX];
  geant TriggerLVL302::_ECadc2mev;
  geant TriggerLVL302::_ECh2lrat;
  geant TriggerLVL302::_ECpedsig;
  geant TriggerLVL302::_ECpmdx;
  geant TriggerLVL302::_ECpmx0;
  geant TriggerLVL302::_ECpmy0;
  int   TriggerLVL302::_ECpmpsl;
  geant TriggerLVL302::_ECpmdz; 
  geant TriggerLVL302::_ECpmz;
  geant TriggerLVL302::_ECcrz3;
  geant TriggerLVL302::_ECcrz4;




    geant TriggerLVL302::TRDAux_DEF::_CooLimits[2];
    geant TriggerLVL302::TRDAux_DEF::_CooBinSize;
    geant TriggerLVL302::TRDAux_DEF::_TanLimits[2];
    geant TriggerLVL302::TRDAux_DEF::_TanBinSize;
 uinteger TriggerLVL302::TRDAux_DEF::_Dir[trdid::nufe][trdid::nudr][trdid::ncrt];
 geant TriggerLVL302::TRDAux_DEF::_Coo[trdid::nufe][trdid::nudr][trdid::ncrt][3];
 geant TriggerLVL302::TRDAux_DEF::_CooZ[trdid::nute][trdconst::maxtube];
 geant TriggerLVL302::TRDAux_DEF::_CooT[trdid::nute][trdconst::maxtube];

 geant TriggerLVL302::TRDAux_DEF::_IncMatrix[trdid::nute][trdconst::maxtube][trdid::nute-1][trdconst::maxtube];
 geant TriggerLVL302::TRDAux_DEF::_CooMatrix[trdid::nute][trdconst::maxtube][trdid::nute-1][trdconst::maxtube];






TriggerLVL302::TriggerLVL302(bool tofin, bool trdin ):_TRDTrigger(0),_TOFTrigger(0),_MainTrigger(0),
_TrackerTrigger(0),_NPatFound(0),_Time(0),_NTrHits(0),_TrEnergyLoss(0),_TriggerInputs(0),TRDAux(),_TOFDirection(0){
  
  if(!trdin)_TriggerInputs =_TriggerInputs | TRDIN; 
  if(!tofin)_TriggerInputs =_TriggerInputs | TOFIN; 

  // Ctor

_Residual[0]=0;
_Residual[1]=0;
_Pattern[0]=-1;
_Pattern[1]=-1;
VZERO(_NTOF,TOF2GC::SCLRS);
VZERO(_TrackerAux,AMSTrIdSoft::ndrp());
VZERO(_nhits,sizeof(_nhits)/sizeof(integer));

}



 integer TriggerLVL302::TOFOr(uinteger paddle,uinteger plane){
         return plane<AMSTOFCluster::planes() && paddle<AMSTOFCluster::padspl(plane) ? _TOFOr[plane][paddle]:-1;}
//
 integer TriggerLVL302::TOFInFastTrigger(uinteger paddle, uinteger plane){
       return plane<AMSTOFCluster::planes() && paddle<AMSTOFCluster::padspl(plane) ? !_TOFStatus[plane][paddle]:-1;}


//------------------------------------------------------
void TriggerLVL302::init(){
  cout<<"====> TriggerLVL302_init: reading the parameters..."<<endl;
  if(LVL3FFKEY.Stat){
   HBOOK1(-400001,"time",100,-1.,9.,0.);
  }
 int i,j;
 integer ltop,lbot,planes,padspl[TOF2GC::SCLRS];
//
    for(i=0;i<15;i++)_flowc[i]=0;// clear prog.flow counters

    if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
      cout<<"      AMS02-setup selected !"<<endl;
// TOF
      ltop=0;//top layer used in matrix-trigger
      lbot=3;//bot ..........
      if(TOF2DBc::plrotm(ltop)==0)ltop=1;
      if(TOF2DBc::plrotm(lbot)==0)lbot=2;
      planes=TOF2DBc::getnplns();
      for(i=0;i<planes;i++)padspl[i]=TOF2DBc::getbppl(i);

      for(i=0;i<TOF2GC::SCMXBR;i++){
        for(j=0;j<TOF2GC::SCLRS;j++)_TOFStatus[j][i]=0;
      }
    }
    else
    {
          cout <<"<---- TOFLVL302-E-Unknown setup !!?"<<endl<<endl;
	  exit(10);
    }
  if(LVL3FFKEY.UseTightTOF){
    char fnam[256]="";
    strcpy(fnam,AMSDATADIR.amsdatadir);
    strcat(fnam,"TOFLVL3.");
    strcat(fnam,AMSJob::gethead()->getsetup());
    ifstream iftxt(fnam,ios::in);
    if(!iftxt){
     cerr <<"<---- TriggerLVL302::init-F-Error open file !!? "<<fnam<<endl;
     exit(1);
    }
    for(i=0;i<padspl[ltop];i++){
      for(j=0;j<padspl[lbot];j++)iftxt>>_TOFPattern[j][i];
    }
    for(i=0;i<planes;i++){
      for(j=0;j<padspl[i];j++)iftxt>>_TOFOr[i][j];
    }
    for(i=0;i<planes;i++){
      for(j=0;j<padspl[i];j++)iftxt>>_TOFStatus[i][j];
    }
    if(iftxt.eof() ){
      cerr<< "<---- TriggerLVL302::init-F-Unexpected EOF !!?"<<endl;
      exit(1);
    }
  }
  else{
    for(i=0;i<padspl[ltop];i++){
      for(j=0;j<padspl[lbot];j++)_TOFPattern[j][i]=1;
    }
    for(j=0;j<planes;j++){
      for(i=0;i<padspl[j];i++)_TOFOr[j][i]=1;
    }
  }
//
    int ii,kk;
    char inum[11];
    char in[2]="0";
    char vname[5];
    strcpy(inum,"0123456789");
//
    for(j=0;j<planes;j++){
      for(i=0;i<padspl[j];i++){
        strcpy(vname,"TF");
        kk=j*TOF2GC::SCMXBR+i+1;//counter ID used in volume name
        ii=kk/10;
        in[0]=inum[ii];
        strcat(vname,in);
        ii=kk%10;
        in[0]=inum[ii];
        strcat(vname,in);
        AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(AMSID(vname,100*(j+1)+i+1));
        if( ptr){
         AMSPoint loc(0,0,0);
         AMSPoint global=ptr->loc2gl(loc);
         for(int k=0;k<3;k++)_TOFCoo[j][i][k]=global[k];         
        }
        else cerr <<"      Init-S-TOFVolumeNotFound"<<AMSID(vname,100*(j+1)+i+1);
      }
    }
//----------------  
//
// ---> read TOF T0's calibration for direction algorithm:
//
 char fname[80];
 char name[80];
 char vers1[3]="mc";
 char vers2[3]="rl";
 strcpy(name,"TOFT0LVL3.AMS02");
 if(AMSJob::gethead()->isMCData()) //      for MC-event
 {
       cout <<"      MC: TOF_t0-calibration is used..."<<endl;
       strcat(name,vers1);
 }
 else                              //      for Real events
 {
       cout <<"      RData: TOF_t0-calibration is used..."<<endl;
       strcat(name,vers2);
 }
//
 char setu1[4]="12b";
 char setu2[4]="8p";
 if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
   if(strstr(AMSJob::gethead()->getsetup(),"TOF:12PAD")){
     cout <<"      TOF: 12PAD setup selected."<<endl;
     strcat(name,setu1);
   }
   else{
    cout <<"      TOF: 8/8/10/8-pads/layer setup selected."<<endl;
    strcat(name,setu2);
   }
 }
 else
 {
    cout <<"      TOF: Unknown setup for TOF_t0-calibr.!!?"<<endl;
    exit(10);
 }
//
 if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Open file : "<<fname<<'\n';
 ifstream tzcfile(fname,ios::in); // open  file for reading
 if(!tzcfile){
   cerr <<"<---- TriggerLVL302_build: missing TOF_t0-file !!? "<<fname<<endl;
   exit(1);
 }
//
// ---> read Tzero's:
 for(int ila=0;ila<TOF2DBc::getnplns();ila++){ 
   for(int ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){
     tzcfile >> _TOFTzero[ila][ibr];
   } 
 }
 tzcfile.close();
//----------------- 
//  ECAL: fill in PM-gains, abs.normalization, h2lratio, pedsig:
//
 for(int isl=0;isl<ecalconst::ECSLMX;isl++){
   for(int ipm=0;ipm<ecalconst::ECPMSMX;ipm++){
     _ECgains[isl][ipm]=1;
   }
 }
 _ECadc2mev=1.15;
 _ECh2lrat=36;
 _ECpedsig=2;//do not need now
 _ECpmdx=ECALDBc::rdcell(7);//transv.pitch(1.8cm)
 _ECpmx0=ECALDBc::gendim(5);//EC-center X-shift
 _ECpmy0=ECALDBc::gendim(6);//EC-center Y-shift
 _ECpmpsl=ECALDBc::slstruc(4);//pms/sl(36)
 _ECpmdz=ECALDBc::gendim(9)+2*ECALDBc::gendim(10);//PM-dz(1.85cm) 
 _ECpmz=ECALDBc::gendim(7);//1st sl front z-pos
// _ECcrz3=0.5*(TOF2DBc::getzsc(2,3)+TOF2DBc::getzsc(2,4));//z-pos TOF-3(X)(~ cm)
// _ECcrz4=0.5*(TOF2DBc::getzsc(2,3)+TOF2DBc::getzsc(2,4));//z-pos TOF-4(Y)(~ cm)
 _ECcrz3=_TrackerCooZ[trconst::maxlay-1];//z-pos last trk-layer(X)(~ cm)
 _ECcrz4=_TrackerCooZ[trconst::maxlay-1];//z-pos last trk-layer(Y)(~ cm)
//-----------------   
  
   

  // Tracker

    for(i=0;i<AMSTrIdSoft::ndrp();i++){
     _TrackerStatus[i]=0;
    }


    for (i=0;i<TKDBc::nlay();i++){
      _TrackerCooZ[i]=0;
      int nsf=0;
      for(int j=0;j<TKDBc::nlad(i+1);j++){
        for(int k=0;k<TKDBc::nsen(i+1,j+1);k++){
         AMSTrIdGeom id(i+1,j+1,k+1,0,0);
         AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(id.crgid());
         if( ptr){
          AMSPoint loc(0,0,0);
          AMSPoint global=ptr->loc2gl(loc);
          _TrackerCooZ[i]+=global[2];
          nsf++;         
         }
        }
      }
      if(nsf==0){
        cerr <<"      TRK: -S-no sensors found for layer "<<i+1<<endl;
        //exit(1);
      }
      else{
          _TrackerCooZ[i]=_TrackerCooZ[i]/nsf;
#ifdef __AMSDEBUG__
          cout <<"      TRK: -I-"<<nsf<<" sensors found for layer "<<i+1<<endl;
#endif
      }
    }
    for(i=0;i<trid::ntdr;i++){
       for( int k=0;k<AMSTrIdSoft::ncrates();k++){
        AMSTrIdSoft id(k,i,1,0);
        if(!id.dead()){         
             _TrackerCoo[i][k][0]=0;         
             _TrackerCoo[i][k][1]=0;
             _TrackerCoo[i][k][2]=0;
             _TrackerDir[i][k]=0;
              int nfs=0;
              int half=id.gethalf(); 
              int a= half==0?0:TKDBc::nhalf(id.getlayer(),id.getdrp());
              int b= half==0?TKDBc::nhalf(id.getlayer(),id.getdrp()):
                          TKDBc::nsen(id.getlayer(),id.getdrp());
/*
              if(a>=b){
               // warning force dead
               cout<<"TriggerLVL302::Init-W-DummyLadderFound "<<id.getdrp()<<" "<<id.getlayer()<<endl;
               id.kill();
               _TrackerCoo[i][k][0]=0;
               _TrackerCoo[i][k][1]=0;
               _TrackerCoo[i][k][2]=0;
               _TrackerDir[i][k]=0;
               break;
              }
*/
              for(int l=a;l<b;l++){
              AMSTrIdGeom idg(id.getlayer(),id.getdrp(),l+1,0,0); 
              AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(idg.crgid());
              if( ptr){
                AMSPoint loc(0,-ptr->getpar(1),0);
                AMSPoint global=ptr->loc2gl(loc);
                _TrackerCoo[i][k][0]+=global[0];
                _TrackerCoo[i][k][1]+=global[1];
                _TrackerCoo[i][k][2]+=global[2];
                _TrackerDir[i][k]+=ptr->getnrmA(1,1);
                nfs++;
              }
          }
          if(nfs==0){
             cerr <<"      TRK: -S-no sensors found for DRP "<<i+1<<" crate "<<k<<" "<<id.getlayer()<<" "<<id.getdrp()<<endl;
             //exit(1);
          }
          else{
#ifdef __AMSDEBUG__
          cout <<"      TRK: -I-"<<nfs<<" sensors found for DRP "<<i+1<<" crate "<<k<<" "<<id.getlayer()<<" "<<id.getdrp()<<endl;
#endif
          _TrackerCoo[i][k][0]=_TrackerCoo[i][k][0]/nfs;
          _TrackerCoo[i][k][1]=_TrackerCoo[i][k][1]/nfs;
          _TrackerCoo[i][k][2]=_TrackerCoo[i][k][2]/nfs;
          _TrackerDir[i][k]=_TrackerDir[i][k]/nfs;
          }
        }
        else {
          _TrackerCoo[i][k][0]=0;
          _TrackerCoo[i][k][1]=0;
          _TrackerCoo[i][k][2]=0;
          _TrackerDir[i][k]=0;

        }
       }
    }
    int k;
    for (i=0;i<trid::ntdr;i++){
      for(k=0;k<AMSTrIdSoft::ncrates();k++){
       AMSTrIdSoft id(k,i,1,0);
       if(!id.dead())_TrackerDRP2Layer[i][k]=id.getlayer()-1;
       else _TrackerDRP2Layer[i][k]=-1;
#ifdef __AMSDEBUG__
       cout <<"      _TrackerDRP2Layer[i][k] "<<i<<" "<<k<<" "<<_TrackerDRP2Layer[i][k]<<endl;
#endif      
      } 
   }

    for (i=0;i<NTRHDRP;i++){
      for(k=0;k<AMSTrIdSoft::ncrates();k++){
       AMSTrIdSoft idx(k,i,0,0);
       if(!idx.dead()){
         AMSTrIdSoft idy(idx.getlayer(),idx.getdrp(),idx.gethalf(),1,0);
         _TrackerOtherTDR[i][k]=idy.gettdr();
      }
       else _TrackerOtherTDR[i][k]=-1;
      } 
   }




// TRD   

  TRDAux_DEF::_CooLimits[0]=-90.;
  TRDAux_DEF::_CooLimits[1]=90.;
  TRDAux_DEF::_CooBinSize=(TRDAux_DEF::_CooLimits[1]-TRDAux_DEF::_CooLimits[0])/trigger302const::matrixsize;
  TRDAux_DEF::_TanLimits[0]=-1.5;
  TRDAux_DEF::_TanLimits[1]=+1.5;
  TRDAux_DEF::_TanBinSize=(TRDAux_DEF::_TanLimits[1]-TRDAux_DEF::_TanLimits[0])/trigger302const::matrixsize;
   for(i=0;i<trdid::nufe;i++){
    for(int j=0;j<trdid::nudr;j++){
     for(int ij=0;ij<trdid::ncrt;ij++){
      for(int k=0;k<3;k++)TRDAux_DEF::_Coo[i][j][ij][k]=0;
                          TRDAux_DEF::_Dir[i][j][ij]   =0;
   
      int nsf=0;
      for(int k=0;k<trdconst::maxtube;k++){
        for(int l=0;l<trdid::nute;l++){
         AMSTRDIdSoft ids(ij,j,i,l,k);
         if(!ids.dead()){
         
          AMSTRDIdGeom id(ids);
          AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(id.crgid());
          if( ptr){
            AMSPoint loc(0,0,0);
            AMSPoint global=ptr->loc2gl(loc);
            nsf++;
            for(int m=0;m<3;m++)TRDAux_DEF::_Coo[ids.getufe()][ids.getudr()][ids.getcrate()][m]+=global[m];
           TRDAux_DEF::_Dir[ids.getufe()][ids.getudr()][ids.getcrate()]+=TRDDBc::LadderOrientation(0,id.getlayer());
        }
       }
//       else if(l==0 && k==0)cerr <<" ids dead " << ij << " " << j << " " << i << endl;
      }
     }

      if(nsf==0){
        cerr <<"      TRK: -W-no TRD Tubes found for segment "<<i<<" "<<j<<ij<<endl;
      }
      else{
           for(int m=0;m<3;m++)TRDAux_DEF::_Coo[i][j][ij][m]/=nsf;
           TRDAux_DEF::_Dir[i][j][ij]/=nsf;
           TRDAux_DEF::_Dir[i][j][ij]=TRDAux_DEF::_Dir[i][j][ij]==0?1:0;
//           cout <<" i "<<i<<" "<<j<<" "<<ij<<" "<<TRDAux_DEF::_Coo[i][j][ij][0]<<" "<<TRDAux_DEF::_Coo[i][j][ij][1]<<" "<<TRDAux_DEF::_Coo[i][j][ij][2]<<" "<<TRDAux_DEF::_Dir[i][j][ij]<<endl;
#ifdef __AMSDEBUG__
//        cout <<"Trigger3::Init-I-"<<nsf<<" TRD Tubes found for segment "<<i<<" "<<j<<" "<<ij<<endl;
#endif
      }
     


   }
   }
   }
 

   for(i=0;i<trdid::nute;i++){
    for(int j=0;j<trdconst::maxtube;j++){
     TRDAux_DEF::_CooZ[i][j]=0;
     TRDAux_DEF::_CooT[i][j]=0;
     int nsf=0;
      for (int k=0;k<trdid::ncrt;k++){
        for(int l=0;l<trdid::nudr;l++){
         for(int m=0;m<trdid::nufe;m++){
         AMSTRDIdSoft ids(k,l,m,i,j);
         if(!ids.dead()){
         AMSTRDIdGeom id(ids);     
          AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(id.crgid());
          if( ptr){
            AMSPoint loc(0,0,0);
            AMSPoint global=ptr->loc2gl(loc);
            nsf++;
            TRDAux_DEF::_CooZ[i][j]+=global[2]-TRDAux_DEF::_Coo[m][l][k][2];
           TRDAux_DEF::_CooT[i][j]+=global[TRDAux_DEF::_Dir[m][l][k]]-TRDAux_DEF::_Coo[m][l][k][TRDAux_DEF::_Dir[m][l][k]];
        }
        }
       }
     }
     }

      if(nsf==0){
        cerr <<"<---- Error: Trigger3::Init-F-no TRD found "<<i<<" "<<j<<endl;
        exit(1);
      }
      else{
           TRDAux_DEF::_CooZ[i][j]/=nsf;
           TRDAux_DEF::_CooT[i][j]/=nsf;
//           cout <<" ii "<<i<<" "<<j<<" "<<TRDAux_DEF::_CooZ[i][j]<<" "<<TRDAux_DEF::_CooT[i][j]<<endl;
      }


}
}

        for(i=0;i<trdid::nute;i++){
         for(int j=0;j<trdconst::maxtube;j++){
           for(int k=i+1;k<trdid::nute;k++){
            for(int l=0;l<trdconst::maxtube;l++){
              TRDAux_DEF::_IncMatrix[i][j][k-1][l]=(TRDAux_DEF::_CooT[i][j]-TRDAux_DEF::_CooT[k][l])/
              (TRDAux_DEF::_CooZ[i][j]-TRDAux_DEF::_CooZ[k][l]);
              TRDAux_DEF::_CooMatrix[i][j][k-1][l]=-TRDAux_DEF::_IncMatrix[i][j][k-1][l]*(TRDAux_DEF::_CooZ[i][j]+TRDAux_DEF::_CooZ[k][l])/2+(TRDAux_DEF::_CooT[i][j]+TRDAux_DEF::_CooT[k][l])/2;
/*
              cout <<"IncMatrix["<<i<<"]["<<j<<"]["<<k-1<<"]["<<l<<"]="<<TRDAux_DEF::_IncMatrix[i][j][k-1][l]<<endl;
              cout <<"CooMatrix["<<i<<"]["<<j<<"]["<<k-1<<"]["<<l<<"]="<<TRDAux_DEF::_CooMatrix[i][j][k-1][l]<<endl;
*/
            }
           }
          }
         } 



    // Now print it in the some file

#ifdef __AMSDEBUG__
    {

    char fnam[256]="lvl3.constants";
    ofstream oftxt(fnam,ios::out);
    if(oftxt){
      oftxt<<endl;
      oftxt << "      TOFPattern[i][14]"<<endl;
      int i,j,k;
      for(i=0;i<padspl[ltop];i++){
        oftxt <<"i "<<i<<" ";
        for(j=0;j<padspl[lbot];j++)oftxt <<_TOFPattern[j][i]<<" ";
        oftxt <<endl;
      }    
      oftxt<<endl;
      oftxt<<endl;
      oftxt << "      TOFOr[i][14]"<<endl;
      for(i=0;i<planes;i++){
        oftxt <<"i "<<i<<" ";
        for(j=0;j<padspl[i];j++)oftxt <<_TOFOr[i][j]<<" ";
        oftxt <<endl;
      }    
      oftxt<<endl;
      oftxt << "      TOFStatus[i][14]"<<endl;
      for(i=0;i<planes;i++){
        oftxt <<"i "<<i<<" ";
        for(j=0;j<padspl[i];j++)oftxt <<_TOFStatus[i][j]<<" ";
        oftxt <<endl;
      }    
      oftxt<<endl;
      oftxt << "      TOFCoo[i][j][3]"<<endl;
      for(i=0;i<planes;i++){
        oftxt <<"i "<<i<<endl;
        for(j=0;j<padspl[i];j++){
         oftxt <<"j "<<j<<" ";
         for(k=0;k<3;k++)oftxt <<_TOFCoo[i][j][k]<<" ";
         oftxt <<endl;
        }
        oftxt <<endl;
      }  
    

      oftxt << "      Trackerstatus[64]"<<endl;
      for(i=0;i<AMSTrIdSoft::ndrp();i++){
       oftxt<<_TrackerStatus[i]<<" ";
      }
      oftxt<<endl;
      oftxt<<endl;

      oftxt << "      TrackerCooZ[nl]"<<endl;
      for(i=0;i<TKDBc::nlay();i++){
       oftxt<<_TrackerCooZ[i]<<" ";
      }
      oftxt<<endl;
      oftxt<<endl;

      oftxt << "      TrackerCoo[i][j][3]"<<endl;
      for(i=0;i<NTRHDRP;i++){
        oftxt <<"i "<<i<<endl;
        for(j=0;j<AMSTrIdSoft::ncrates();j++){
         oftxt <<"j "<<j<<" ";
         for(k=0;k<3;k++)oftxt <<_TrackerCoo[i][j][k]<<" ";
         oftxt <<endl;
        }
        oftxt <<endl;
      }  
      oftxt<<endl;

      oftxt << "      TrackerDir[i][2]"<<endl;
      for(i=0;i<NTRHDRP;i++){
        oftxt <<"i "<<i<<endl;
         for(k=0;k<AMSTrIdSoft::ncrates();k++)oftxt <<_TrackerDir[i][k]<<" ";
         oftxt <<endl;
      }

      oftxt<<endl;


      oftxt << "      TrackerDRP2Layer[i][2]"<<endl;
      for(i=0;i<NTRHDRP;i++){
        oftxt <<"i "<<i<<endl;
         for(k=0;k<AMSTrIdSoft::ncrates();k++)oftxt <<_TrackerDRP2Layer[i][k]<<" ";
         oftxt <<endl;
        }
        
    
      oftxt<<endl;

      oftxt << "      TrackerOtherTDR[i][2]"<<endl;
      for(i=0;i<NTRHDRP;i++){
         oftxt <<"i "<<i<<endl;
         for(k=0;k<AMSTrIdSoft::ncrates();k++)oftxt <<_TrackerOtherTDR[i][k]<<" ";
         oftxt <<endl;
      }
    




    }
    }
#endif

  cout<<"<---- TriggerLVL302::init: successfully done !"<<endl<<endl;    
}

//---------------------------------------------------------------- 
void TriggerLVL302::addtof(int16 plane, int16 paddle){
  if(plane>=0 && plane<AMSTOFCluster::planes() && paddle>=0 && paddle<AMSTOFCluster::padspl(plane) && 
  _TOFStatus[plane][paddle] ==0){
   _TOFAux[plane][_NTOF[plane]]=paddle;
   _NTOF[plane]=_NTOF[plane]+1;
  }

}
//----
void TriggerLVL302::settofdir(int dir){
  _TOFDirection=dir;
}
//----------------------------------------------------------------
void TriggerLVL302::setecemag(integer val){
  _ECemag=val;
}
//----
void TriggerLVL302::setecmatc(integer val){
  _ECmatc=val;
}
void TriggerLVL302::setectofcr(geant co[2], geant tg[2]){
  _ECtofcr[0]=co[0];
  _ECtofcr[1]=co[1];
  _ECtofcr[2]=tg[0];
  _ECtofcr[3]=tg[1];
}
//----------------------------------------------------------------
  integer TriggerLVL302::toftrdok(){ 
// Now we allow 1tof  at top and 1tof  at botton if no trd 
// Take nearest to trd if trd exist
// In case trd/tof mismatch TOF Prevails if one and only one tof exists

     _TOFTrigger=0;
    int i,j;
    int ntof=0;
      for(i=0;i<_NTOF[0];i++){
        for(j=0;j<_NTOF[AMSTOFCluster::planes()-1];j++){
          if(_TOFPattern[_TOFAux[0][i]][_TOFAux[AMSTOFCluster::planes()-1][j]]){
              goto out;
          }
        }
      }
     _TOFTrigger=-1;
      return 0;
out:

     for(i =0;i<AMSTOFCluster::planes();i++){
      if (_NTOF[i]>0 && _NTOF[i]<=2){
        _TOFTrigger+= (1<<i);
        if(_NTOF[i]>1 && abs(_TOFAux[i][0]-_TOFAux[i][1]) > 1)_TOFTrigger+=((1<<AMSTOFCluster::planes())<<i);
       }
    }
    if((_TOFTrigger&1) ==0 || (_TOFTrigger>>(AMSTOFCluster::planes()-1)&1) ==0){
     return (_TRDTrigger>>1)&1;
    }
    else return 1; 
 }
//---------------------------------------------
  void TriggerLVL302::preparetracker(){
   int toftop[2]={1,0};
    int tofbot[2]={2,3};
   for(int icase=0;icase<2;icase++){       
    geant cooup=0;
    geant coodown=0;
    geant z12=(_TOFCoo[toftop[icase]][0][2]+_TOFCoo[tofbot[icase]][0][2])/2;
    geant dz12=(_TOFCoo[toftop[icase]][0][2]-_TOFCoo[tofbot[icase]][0][2])/2;
    if(((_TOFTrigger>>toftop[icase])&1) && (_TOFTrigger>>(tofbot[icase])&1)){
    for(int i=0;i<_NTOF[toftop[icase]];i++)cooup+=_TOFCoo[toftop[icase]][_TOFAux[toftop[icase]][i]][icase];
    cooup=cooup/_NTOF[toftop[icase]];   
    for(int i=0;i<_NTOF[tofbot[icase]];i++)coodown+=_TOFCoo[tofbot[icase]][_TOFAux[tofbot[icase]][i]][icase];
    coodown=coodown/_NTOF[tofbot[icase]];   
      geant addtofup=TOF2DBc::plnstr(5)*0.6;
      geant addtofdown=TOF2DBc::plnstr(5)*0.6;
      if((_TOFTrigger>>(AMSTOFCluster::planes()+toftop[icase])) & 1){
       addtofup+=abs(_TOFAux[toftop[icase]][0]-_TOFAux[toftop[icase]][1]-1)*TOF2DBc::plnstr(5);
      }
      if((_TOFTrigger>>(AMSTOFCluster::planes()+tofbot[icase])) & 1){
       addtofdown+=abs(_TOFAux[tofbot[icase]][0]-_TOFAux[tofbot[icase]][1])*TOF2DBc::plnstr(5);
      }
    for(int i=0;i<TKDBc::nlay();i++){
      geant coo=coodown+(cooup-coodown)/(_TOFCoo[toftop[icase]][0][2]-_TOFCoo[tofbot[icase]][0][2])*
      (_TrackerCooZ[i]-_TOFCoo[tofbot[icase]][0][2]);
      geant addmagfield=(icase==0?0.1:1)*LVL3FFKEY.TrTOFSearchReg*(1-fabs(_TrackerCooZ[i]-z12)/dz12);
      geant addtof=addtofup+(_TOFCoo[toftop[icase]][0][2]-_TrackerCooZ[i])/dz12/2*(addtofdown-addtofup);
      _lowlimitTOF[icase][i]=-addmagfield-addtof+coo;
      _upperlimitTOF[icase][i]=addtof+addmagfield+coo;
    }
    }
    else{
     for(int i=0;i<TKDBc::nlay();i++){
       _lowlimitTOF[icase][i]=-FLT_MAX;
       _upperlimitTOF[icase][i]=FLT_MAX;
     }
    }
   
   if((_TRDTrigger>>icase)&1){
//  
//  Upgrade parameters in case there is a single tof
//  
   number ztof=0;
   number ttof=0;
   number z2tof=0;
   number tztof=0;
   number etof=0;
   number err=1/(0.5*TOF2DBc::plnstr(5));
   err=err*err;
   if(((_TOFTrigger>>(AMSTOFCluster::planes()+toftop[icase])) & 1)==0 &&
      ((_TOFTrigger>>toftop[icase])&1)){
      ztof+=_TOFCoo[toftop[icase]][_TOFAux[toftop[icase]][0]][2]*err;
      z2tof+=_TOFCoo[toftop[icase]][_TOFAux[toftop[icase]][0]][2]*_TOFCoo[toftop[icase]][_TOFAux[toftop[icase]][0]][2]*err;
      etof+=err;
      ttof+=_TOFCoo[toftop[icase]][_TOFAux[toftop[icase]][0]][icase]*err;
      tztof+=_TOFCoo[toftop[icase]][_TOFAux[toftop[icase]][0]][icase]*_TOFCoo[toftop[icase]][_TOFAux[toftop[icase]][0]][2]*err;
   }
   if(((_TOFTrigger>>(AMSTOFCluster::planes()+tofbot[icase])) & 1)==0 &&
      ((_TOFTrigger>>tofbot[icase])&1)){
      ztof+=_TOFCoo[tofbot[icase]][_TOFAux[tofbot[icase]][0]][2]*err;
      z2tof+=_TOFCoo[tofbot[icase]][_TOFAux[tofbot[icase]][0]][2]*_TOFCoo[tofbot[icase]][_TOFAux[tofbot[icase]][0]][2]*err;
      etof+=err;
      ttof+=_TOFCoo[tofbot[icase]][_TOFAux[tofbot[icase]][0]][icase]*err;
      tztof+=_TOFCoo[tofbot[icase]][_TOFAux[tofbot[icase]][0]][icase]*_TOFCoo[tofbot[icase]][_TOFAux[tofbot[icase]][0]][2]*err;
   }
   if(etof){
    err=1./(0.5*TRDDBc::TubeInnerDiameter());
    err=err*err;
    number t=ttof+TRDAux._Par[icase][1]*TRDAux._NHits[icase]*err;
    number z=ztof+TRDAux._Par[icase][2]*TRDAux._NHits[icase]*err;
    number tz=TRDAux._Par[icase][4]*TRDAux._NHits[icase]*err;
    number z2=TRDAux._Par[icase][5]*TRDAux._NHits[icase]*err;
    number e=etof+TRDAux._NHits[icase]*err;
    t/=e;
    z/=e;
    tz/=e;
    z2/=e;
    TRDAux._Par[icase][0]=(tz-t*z)/(z2-z*z);
    TRDAux._Par[icase][1]=t;
    TRDAux._Par[icase][2]=z;
   }
   for(int i=0;i<TKDBc::nlay();i++){
     geant coo=TRDAux._Par[icase][0]*(_TrackerCooZ[i]-TRDAux._Par[icase][2])+TRDAux._Par[icase][1];
     geant addtrd=fabs(3*TRDAux._Par[icase][3]*(_TrackerCooZ[i]-TRDAux._Par[icase][2]));
      geant addmagfield=(icase==0?0.2:2)*LVL3FFKEY.TrTOFSearchReg*fabs((_TrackerCooZ[i]-_TrackerCooZ[0])/(_TrackerCooZ[TKDBc::nlay()-1]-_TrackerCooZ[0]));
      _lowlimitTRD[icase][i]=-addmagfield-addtrd+coo;
      _upperlimitTRD[icase][i]=addmagfield+addtrd+coo;
   }
   }
   else{
     for(int i=0;i<TKDBc::nlay();i++){
       _lowlimitTRD[icase][i]=-FLT_MAX;
       _upperlimitTRD[icase][i]=FLT_MAX;
     }
   }
  }
   for(int i=0;i<TKDBc::nlay();i++){
   if(_lowlimitTRD[0][i]>=_lowlimitTOF[0][i] && _upperlimitTRD[0][i]<=_upperlimitTOF[0][i]){
    _lowlimitX[i]=_lowlimitTRD[0][i];
    _upperlimitX[i]=_upperlimitTRD[0][i];
   }
   else if(_lowlimitTRD[0][i]<=_lowlimitTOF[0][i] && _upperlimitTRD[0][i]>=_upperlimitTOF[0][i]){
    _lowlimitX[i]=_lowlimitTOF[0][i];
    _upperlimitX[i]=_upperlimitTOF[0][i];
   }
   else{ 
    _lowlimitX[i]=0.5*(_lowlimitTOF[0][i]+_lowlimitTRD[0][i]);
    _upperlimitX[i]=0.5*(_upperlimitTOF[0][i]+_upperlimitTRD[0][i]);
   }

   if(_lowlimitTRD[1][i]>=_lowlimitTOF[1][i] && _upperlimitTRD[1][i]<=_upperlimitTOF[1][i]){
    _lowlimitY[i]=_lowlimitTRD[1][i];
    _upperlimitY[i]=_upperlimitTRD[1][i];
   }
   else if(_lowlimitTRD[1][i]<=_lowlimitTOF[1][i] && _upperlimitTRD[1][i]>=_upperlimitTOF[1][i]){
    _lowlimitY[i]=_lowlimitTOF[1][i];
    _upperlimitY[i]=_upperlimitTOF[1][i];
   }
   else{ 
    _lowlimitY[i]=0.5*(_lowlimitTOF[1][i]+_lowlimitTRD[1][i]);
    _upperlimitY[i]=0.5*(_upperlimitTOF[1][i]+_upperlimitTRD[1][i]);
   }

   }
}


geant TriggerLVL302::Discriminator(integer nht){

   return max(LVL3FFKEY.TrMaxResidual[2],LVL3FFKEY.TrMaxResidual[0]-LVL3FFKEY.TrMaxResidual[1]*nht);
}
//-------------------------------------------------------------
int TriggerLVL302::eccrosscheck(geant ect){
//
  geant dx,dy,xl,yl,xh,yh,xcut,ycut;
  geant x0,y0;
//
  xl=_lowlimitX[trconst::maxlay-1];
  xh=_upperlimitX[trconst::maxlay-1];
  yl=_lowlimitY[trconst::maxlay-1];
  yh=_upperlimitY[trconst::maxlay-1];
  x0=0.5*(_lowlimitX[trconst::maxlay-1]+_upperlimitX[trconst::maxlay-1]);
  y0=0.5*(_lowlimitY[trconst::maxlay-1]+_upperlimitY[trconst::maxlay-1]);
//for "out-of-window distance" methode:
//  if((_ECtofcr[0]-xl)<(xh-_ECtofcr[0]))dx=_ECtofcr[0]-xl;
//  else dx=xh-_ECtofcr[0];
//  if((_ECtofcr[1]-yl)<(yh-_ECtofcr[1]))dy=_ECtofcr[1]-yl;
//  else dy=yh-_ECtofcr[1];
  dx=_ECtofcr[0]-x0;//for "mid-of-window distance" methode
  dy=_ECtofcr[1]-y0;
  if(ECREFFKEY.reprtf[0]!=0){
//    HF1(ECHISTR+45,dx,1.);
//    HF1(ECHISTR+48,dy,1.);
    HF1(ECHISTR+53,xh-xl,1.);
    HF1(ECHISTR+54,yh-yl,1.);
    HF1(ECHISTR+55,dx,1.);
    HF1(ECHISTR+56,dy,1.);
  }
//  xcut=19.6+24.75/sqrt(ect);
//  if(xcut>44)xcut=44;
//  ycut=19.1+61.9/sqrt(ect);
//  if(ycut>54)ycut=54;
//  if(dx>-xcut && dy>-ycut)return(1);//match ok
  xcut=27+42/sqrt(ect);
  if(xcut>57)xcut=57;
  ycut=27+47/sqrt(ect);
  if(ycut>60)ycut=60;
  if(fabs(dx)<xcut && fabs(dy)<ycut)return(1);//match ok
  else return(0);
}

//-----------------------------------------------------------
  void TriggerLVL302::build(){

    if(LVL3FFKEY.RebuildLVL3==1){
      AMSEvent::gethead()->getC("TriggerLVL3",0)->eraseC();
    }
    // Shuttle Now Station
//tempor    
     AMSgObj::BookTimer.start("LVL3");
     _flowc[0]+=1;
     Trigger2LVL1 * plvl1= 
       (Trigger2LVL1*)AMSEvent::gethead()->
       getheadC("TriggerLVL1",0);
     if(plvl1){
       int16 * ptr;
       number tt1,tt2;
       _flowc[1]+=1;
       TriggerAuxLVL302 aux[trid::ncrt];
       for(int icrt=0;icrt<AMSTrIdSoft::ncrates();icrt++){
          aux[icrt].addnoisetk(icrt);
          aux[icrt].filltk(icrt);
       }

//     fill trd  


       TriggerAuxLVL302 auxtrd[trdid::ncrt];
       for(int icrt=0;icrt<AMSTRDIdSoft::ncrates();icrt++){
        auxtrd[icrt].filltrd(icrt);
       }

//
//---> fill tof:  
//
       TriggerAuxLVL302 auxtof;
       auxtof.filltof();
//
//---> fill ecal:  
//
       TriggerAuxLVL302 auxecal;
       auxecal.fillecal();
//
//===> create LVL3-object:
//
       int idum,i,j;
       TriggerLVL302 *plvl3=0;
       tt1=HighResTime();
       for(idum=0;idum<LVL3FFKEY.NRep;idum++){
         delete plvl3;
         plvl3 = new TriggerLVL302();
//
//-----------> START the lvl3 algorithm:
//
//===> 1st: read TOF raw-time data, define part.direction:
//
    geant tofrt[TOF2GC::SCLRS][TOF2GC::SCMXBR],toft[TOF2GC::SCLRS];
    geant ttop,tbot,dt;
    int16 ntofrt[TOF2GC::SCLRS][TOF2GC::SCMXBR];
    int16 idd,id,il,ib,is;
    plvl3->settofdir(0);//reset TOF-dir flag
//
    if(plvl3->UseTOFTime()){// <--- generate/use TOF-dir info
    _flowc[2]+=1;
    for(i=0;i<TOF2GC::SCLRS;i++){
      for(j=0;j<TOF2GC::SCMXBR;j++){
        tofrt[i][j]=0;
        ntofrt[i][j]=0;
      }
    }
//
    ptr=auxtof.readtof(1);
    while(ptr){
      idd=*(ptr+1);//SoftID=LBBS 
      id=idd/10;// short id=LBB, where L=1,4 BB=1,12
      il=id/100-1;
      ib=id%100-1;
      is=idd%10-1;
      tofrt[il][ib]+=(*(ptr+2))*TOF2DBc::tdcbin(1);//sum raw side-time, TDCch->ns
      ntofrt[il][ib]+=1;
      ptr=auxtof.readtof();//to take next
    }
//
    for(i=0;i<AMSTOFCluster::planes();i++){
      toft[i]=0;
      for(j=0;j<AMSTOFCluster::padspl(i);j++){
        if(ntofrt[i][j]==2){
	  tofrt[i][j]/=2;//(t1+t2)/2
	  tofrt[i][j]+=_TOFTzero[i][j];//add calibration
	}
	else tofrt[i][j]=0;
	if(plvl1->checktofpattand(i,j)){//look at "lvl1-fired" paddles only
	  if(tofrt[i][j]>toft[i])toft[i]=tofrt[i][j];//select max time in layer(first in real time)
	}
      }
    }
//
    if(toft[0]>0 && toft[1]>0)ttop=(toft[0]+toft[1])/2;
    else if(toft[0]>0)ttop=toft[0];
    else if(toft[1]>0)ttop=toft[1];
    else ttop=0;
    if(toft[2]>0 && toft[3]>0)tbot=(toft[2]+toft[3])/2;
    else if(toft[2]>0)tbot=toft[2];
    else if(toft[3]>0)tbot=toft[3];
    else tbot=0;
//
    if(ttop>0 && tbot>0){
      _flowc[3]+=1;
      dt=ttop-tbot;
      if(TFREFFKEY.reprtf[2]!=0)HF1(1092,ttop-tbot,1.);
      if(dt>=1.5)plvl3->settofdir(1);
      else if(dt<-1.5)plvl3->settofdir(-1);
      else plvl3->settofdir(0);
    }
    }//---> endof UseTOFTime() check
//---------
//
//===> 2nd: accumulate patterns of TOF fired paddles:
//
    for(i=0;i<AMSTOFCluster::planes();i++){
      for(j=0;j<AMSTOFCluster::padspl(i);j++){
        if(plvl1->checktofpattand(i,j) ||
            (_TOFOr[i][j] && plvl1->checktofpattor(i,j))){
          plvl3->addtof(i,j);
        }
      }
    }
//---------
//
//===> ECAL: create Edep-pattern, check EM, Tracker-matching:
//
    int pmc,pm,sl,proj,ip;
    number ah,al,amp,ectot,efrnt,ebase,epeak;
    number EClprof[ECSLMX];
    number ECemap[ECSLMX][ECPMSMX];
    geant ecolx,ecoly;
    geant ech2x(-1),ech2y(-1);
    geant ectfcr[2]={999,999};
    geant ecrtg[2]={999,999};
    int ectrackok,match;
    int ncolx,ncoly;
    plvl3->setecemag(0);//reset EC-emag flag(noECactivity)
    plvl3->setecmatc(0);//reset EC-match flag(noECTrack)
    plvl3->setectofcr(ectfcr,ecrtg);//reset EC-track cross.point/slope
    ectot=0;
    ectrackok=0;
    match=0;
//
  if(plvl3->UseECEMinfo() || plvl3->UseECMATinfo()){// <--- use ECAL info
      _flowc[4]+=1;
      for(int i=0;i<ECSLMX;i++){
        EClprof[i]=0;
	for(int j=0;j<ECPMSMX;j++){
	  ECemap[i][j]=0;
	}
      }
      ptr=auxecal.readecal(1);
      while(ptr){
        id=*ptr;//SSPPC
        idd=id/10;
        pmc=id%10-1;//PMSubCell(0-3)
        pm=idd%100-1;//PM(0-...)
	sl=idd/100-1;//superlayer
	ah=number(*(ptr+1));//for real algor. may be need /16
	al=number(*(ptr+2));
	amp=0;
	if(ah>0){//(some readout thresh. is assumed to be done on prev. stages of readout)
	  if(ah<number(ECADCMX[0]))amp=ah;// no ovfl
	  else{
	    if(al>0)amp=al*_ECh2lrat;
	  }
	  amp*=_ECadc2mev;//ADCch->mev
	  EClprof[sl]+=amp;
	  ectot+=amp;
	  ECemap[sl][pm]+=amp;
	}
        ptr=auxecal.readecal();//to take next
      }//---> endof raw hit loop
//
//
//---> calc. electromagneticity:
//
      efrnt=EClprof[0]+EClprof[1]+EClprof[2];
      ebase=EClprof[0]+EClprof[ECSLMX-1];
      epeak=EClprof[1]+EClprof[2]+EClprof[3];
      geant etcut(120);
      geant efrcut(400);
      geant p2bcut(3.2);
      geant esep1(15000);
      geant esep2(20000);
      geant ewthr(18);
      int wxcut(10);
      int wycut(14);
      geant p2brat,p2frat;
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTR+31,geant(ectot),1.);
      if(ectot>=etcut){// >= Mip
        _flowc[5]+=1;
        if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTR+32,geant(efrnt),1.);
	if(efrnt<efrcut){
	  plvl3->setecemag(-1);//nonEM
	  goto ecfin1;
	}
        if(ebase>0)p2brat=epeak/ebase;
        else p2brat=39.5;
        if(p2brat>39.5)p2brat=39.5;
	if(ectot<esep1){
            if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTR+33,p2brat,1.);
//            if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTR+34,p2frat,1.);
	    if(p2brat<p2bcut){
	      plvl3->setecemag(-1);//nonEM
	      goto ecfin1;
	    }
	}
	if(ectot<esep2){//shower width check
          ncolx=0;
          ncoly=0;
          for(pm=0;pm<ECPMSMX;pm++){ // "shower width" calculation
            ecolx=0;
            ecoly=0;
            for(sl=0;sl<ECSLMX-1;sl+=2)ecoly+=ECemap[sl][pm];
            if(ecoly>ewthr)ncoly+=1;
            for(sl=1;sl<ECSLMX-1;sl+=2)ecolx+=ECemap[sl][pm];
            if(ecolx>ewthr)ncolx+=1;
            if(ECREFFKEY.reprtf[0]!=0){
              if(ecolx>0)HF1(ECHISTR+37,ecolx,1.);
              if(ecoly>0)HF1(ECHISTR+38,ecoly,1.);
            }
          }//--->endof width calc.
          if(ECREFFKEY.reprtf[0]!=0){
	    HF1(ECHISTR+35,geant(ncolx),1.);
            HF1(ECHISTR+36,geant(ncoly),1.);
	  }
	  if(ncolx>wxcut || ncoly>wycut){
	    plvl3->setecemag(-1);//nonEM
	    goto ecfin1;
	  }
	}
	plvl3->setecemag(1);//EM + Etot>Esep2
        _flowc[6]+=1;
      }//---> endof >=Mip check
 ecfin1:
      if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTR+39,geant(plvl3->getecemag()),1.);
//
// ---> calc. sl-COG's :
//
   number ecogt[ECSLMX],ecogl,crms[ECSLMX];
   geant ectx(0),ecty(0),ecx0(0),ecy0(0);
   geant eccrxe(0),eccrye(0),ectxe(0),ectye(0);
   number etl[ECSLMX];
   number cel2,epm,elsx,elsy,epmin;
   geant epmmx,eclt;
   int npmcl[ECSLMX],npmmx,pmin,pmax,pmngb;
   geant ecnoisl(5);//to discrim. from noise(mev)
   geant eshcut;
   eshcut=ECALVarp::ecalvpar.rtcuts(9);//boundary-cut between "aver" and "peak" methods
//
   ectrackok=0;
   if(plvl3->UseECMATinfo()){//use EC-Match info
   _flowc[7]+=1;
   if(ectot>=etcut){//>=MIP check
      _flowc[8]+=1;
      ecogl=0;
      elsx=0;
      elsy=0;
      for(sl=0;sl<ECSLMX;sl++){//<==== SL-loop to calc. coo/rms
        etl[sl]=0;
	ecogt[sl]=0;
	npmcl[sl]=0;
	cel2=0;
	crms[sl]=0;
        epmmx=0;
        for(pm=0;pm<ECPMSMX;pm++){//first find max.signal PM
	  if(ECemap[sl][pm]>epmmx){
	    npmmx=pm;//store max pm-signal in given SL
	    epmmx=ECemap[sl][pm];
	  }
	}
	if(epmmx<ecnoisl)continue;//use only layer with peak-signal > 5 mev(well above the noise)
	epmin=ecnoisl;
	if(sl<=1)pmngb=1;
	else pmngb=2;
	pmin=npmmx-pmngb;
	if(pmin<0)pmin=0;
	pmax=npmmx+pmngb;
	if(pmax>ECPMSMX)pmax=ECPMSMX;
	eclt=0;
	for(pm=pmin;pm<pmax;pm++)eclt+=ECemap[sl][pm];// Peak-Cluster Etot
	if(eclt/EClprof[sl]<=eshcut){//wide Edep distribution->use "Average" methode
	  pmin=0;//redefine limits for "average" method
	  pmax=ECPMSMX;
          epmin=0.05*EClprof[sl];
	  if(sl==0)epmin=0.15*EClprof[sl];//higher thr(to suppress low-en bremstr-hits)
	  if(epmin<ecnoisl)epmin=ecnoisl; 
	}
        for(pm=pmin;pm<pmax;pm++){//<==== calc. cluster COG
	  epm=ECemap[sl][pm];
	  if(epm>epmin){
	    etl[sl]+=epm;
	    ecogt[sl]+=(pm+1)*epm;
	    cel2+=(pm+1)*(pm+1)*epm;
	    npmcl[sl]+=1;
	  }
	}
	if(etl[sl]>0){//calc. cog/rms
	  ecogt[sl]/=etl[sl];//sl CoG in pm-units
	  cel2/=etl[sl];
	  crms[sl]=cel2-ecogt[sl]*ecogt[sl];//rms**2
	  if(crms[sl]<0)crms[sl]=0;
	  else crms[sl]=sqrt(crms[sl])*_ECpmdx;// in cm
	  if(crms[sl]==0)crms[sl]=_ECpmdx/sqrt(12.);//for 1-pm cluster
	  ecogt[sl]=-0.5*_ECpmdx*_ECpmpsl+_ECpmdx*(ecogt[sl]-0.5);//cog in cm
	  if(sl%2==0)ecogt[sl]+=_ECpmy0;//tempor 1st sl->Y
	  else ecogt[sl]+=_ECpmx0;
	  ecogl+=(sl+1)*etl[sl];
	  if(sl%2==0)elsy+=etl[sl];
	  else elsx+=etl[sl];
	  if(ECREFFKEY.reprtf[0]!=0){
	    if(sl==0)HF1(ECHISTR+41,geant(ecogt[sl]),1.);
	    if(sl==1)HF1(ECHISTR+42,geant(ecogt[sl]),1.);
	    if(sl==0)HF1(ECHISTR+49,geant(crms[sl]),1.);
	    if(sl==1)HF1(ECHISTR+50,geant(crms[sl]),1.);
	    if(sl==7)HF1(ECHISTR+51,geant(crms[sl]),1.);
	    if(sl==8)HF1(ECHISTR+52,geant(crms[sl]),1.);
	  }
	}
      }//--->endof sl-loop
//
      if((elsx+elsy)>0){//calc. z-cog(longit)
        ecogl/=(elsx+elsy);
	ecogl=_ECpmz-_ECpmdz*(ecogl-0.5);//z-cog in cm
	if(ECREFFKEY.reprtf[0]!=0)HF1(ECHISTR+40,geant(ecogl),1.);
      }
//
//---> linear fit in proj:
//
      geant fpnt,suz,sux,suxz,suz2,sud,sg2,ct,cz,ctsg,slim(15),ass;
      geant etlr[ECSLMX];
      fpnt=0;
      suz=0;
      sux=0;
      suxz=0;
      suz2=0;
      sud=0.;
      for(sl=0;sl<ECSLMX;sl+=2){//for Y-proj
        ct=ecogt[sl];
	ctsg=crms[sl];
	cz=_ECpmz-_ECpmdz*(sl+0.5);
	if(ctsg>0){
	  etlr[sl]=etl[sl]/elsy;//get relative energy dep.
	  sg2=ctsg*ctsg/etlr[sl];
	  fpnt+=1;
          sud+=1./sg2;
          sux+=ct/sg2;
          suz+=cz/sg2;
          suz2+=cz*cz/sg2;
          suxz+=ct*cz/sg2;
	}
      }
      if(fpnt>=2){//calc. tan, cross, chi2
        ecty=(sud*suxz-sux*suz)/(sud*suz2-suz*suz);
        ecy0=(sux*suz2-suxz*suz)/(sud*suz2-suz*suz);
        ech2y=0;
	if(fpnt>2){
        for(sl=0;sl<ECSLMX;sl+=2){//calc. chi2
	  ctsg=crms[sl];
	  if(ctsg>0){
	    cz=_ECpmz-_ECpmdz*(sl+0.5);
            ct=ecogt[sl];
            ech2y+=etlr[sl]*pow((ecy0+ecty*cz-ct)/ctsg,2);
	  }
        }
        ech2y/=geant(fpnt-2);
	}
	ectfcr[1]=ecty*_ECcrz4+ecy0;//Y-cross with last trk-layer(was TOF4)
	ecrtg[1]=ecty;
        if(ECREFFKEY.reprtf[0]!=0){
	  HF1(ECHISTR+43,ech2y,1.);
	  HF1(ECHISTR+44,ecty,1.);
	}
      }
//------
      fpnt=0;
      suz=0;
      sux=0;
      suxz=0;
      suz2=0;
      sud=0.;
      for(sl=1;sl<ECSLMX-1;sl+=2){//for X-proj
        ct=ecogt[sl];
	ctsg=crms[sl];
	cz=_ECpmz-_ECpmdz*(sl+0.5);
	if(ctsg>0){
	  etlr[sl]=etl[sl]/elsx;//get relative energy dep.
	  sg2=ctsg*ctsg/etlr[sl];
	  fpnt+=1;
          sud+=1./sg2;
          sux+=ct/sg2;
          suz+=cz/sg2;
          suz2+=cz*cz/sg2;
          suxz+=ct*cz/sg2;
	}
      }
      if(fpnt>=2){//calc. tan, cross, chi2
        ectx=(sud*suxz-sux*suz)/(sud*suz2-suz*suz);
        ecx0=(sux*suz2-suxz*suz)/(sud*suz2-suz*suz);
        ech2x=0;
	if(fpnt>2){
        for(sl=1;sl<ECSLMX-1;sl+=2){//calc. chi2
	  ctsg=crms[sl];
	  if(ctsg>0){
	    cz=_ECpmz-_ECpmdz*(sl+0.5);
            ct=ecogt[sl];
            ech2x+=etlr[sl]*pow((ecx0+ectx*cz-ct)/ctsg,2);
	  }
        }
        ech2x/=geant(fpnt-2);
	}
	ectfcr[0]=ectx*_ECcrz3+ecx0;//X-crossing with last trk-layer(was TOF3)
	ecrtg[0]=ectx;
        if(ECREFFKEY.reprtf[0]!=0){
	  HF1(ECHISTR+46,ech2x,1.);
	  HF1(ECHISTR+47,ectx,1.);
	}
        if(ech2x>=0 && ech2x<1.5 && ech2y>=0 && ech2y<2.){
          _flowc[9]+=1;
	  ectrackok=1;
          plvl3->setecmatc(-1);//set EC-track flag to "noMatch"(but it may be reset later)
          plvl3->setectofcr(ectfcr,ecrtg);//set x/y-cr.points and slopes(for ntuple)
	}
      }
//----
   }//endof >=MIP check
//----
//
  }//---> endof UseECMATinfo check
  }//---> endof UseECinfo check
//-------------------------------
//
//===> TRD
    
    if(plvl3->UseTRD()){
        integer crate;
      for(crate=0;crate<AMSTRDIdSoft::ncrates();crate++){
      ptr=auxtrd[crate].readtrd(1);  
      while(ptr){
        uinteger udr,ufe,ute,chan;
        AMSTRDRawHit::lvl3CompatibilityAddress(ptr[0],udr,ufe,ute,chan);
           if(!(plvl3->TRDAux.addnewhit(crate,udr,ufe,ute,chan,ptr[1]))){
             plvl3->TRDTrigger()=4;
             goto formed;
           }
          ptr = auxtrd[crate].readtrd();    
      }
     }
    }   

    if(plvl3->TRDAux._nufe>=0){
     plvl3->TRDAux.build();
     for(int i=0;i<2;i++)if(plvl3->TRDAux._SegmentFound[i])plvl3->_TRDTrigger+= (1<<i);
     if(plvl3->TRDAux._HMult && float(plvl3->TRDAux._HMult)/float(plvl3->TRDAux._NHits[0]+plvl3->TRDAux._NHits[1])>LVL3FFKEY.TRDHMulPart)plvl3->_TRDTrigger+=8;
    }
    if(plvl3->toftrdok() == 0) goto formed;
    
    
    _flowc[10]+=1;


  //
  // now Tracker Part
  //   
  plvl3->preparetracker();
//
// ---> ECAL-TOF/TRD match ?
//
  if(plvl3->UseECMATinfo()){//use EC-Match info
    if(ectrackok){
      _flowc[11]+=1;
      ectot/=1000;
      match=plvl3->eccrosscheck(ectot);//check TOF/TRD/ECAL space matching
      if(match){
        plvl3->setecmatc(1);//set EC-match flag to "Match"
        _flowc[12]+=1;
      }
    }
  }
//---
  integer crate;
  for(crate=0;crate<AMSTrIdSoft::ncrates();crate++){
   ptr=aux[crate].readtracker(1);  
  while(ptr){
     integer drp,va,strip,side;
     AMSTrRawCluster::lvl3CompatibilityAddress
     (ptr[1],strip,va,side,drp);
     if(side == 0  && 
         _TrackerStatus[_TrackerOtherTDR[drp][crate]+crate*NTRHDRP] == 0)
         _TrackerAux[_TrackerOtherTDR[drp][crate]][crate]=1;
   ptr = aux[crate].readtracker();    
  }
  }
  for(crate=0;crate<AMSTrIdSoft::ncrates();crate++){
  ptr=aux[crate].readtracker(1);  
  while(ptr){
     integer drp,va,strip,side;
     AMSTrRawCluster::lvl3CompatibilityAddress
      (ptr[1],strip,va,side,drp);
     if(side != 0 && (LVL3FFKEY.NoK || _TrackerAux[drp][crate])){
      integer layer=_TrackerDRP2Layer[drp][crate];
#ifdef __AMSDEBUG__
      if(layer<0){
       cerr<<"TriggerLVL302::build-S-wronglayer "<<layer<<" "<<drp<<" "<<crate<<" "<<ptr[1]<<" "<<strip<<" "<<side<<endl;
       goto next;
      }
       //cout<<"TriggerLVL302::build-I-layer "<<layer<<" "<<drp<<" "<<crate<<" "<<ptr[1]<<" "<<strip<<" "<<side<<endl;
#endif      
      integer num = ((*ptr)&63);
      if(abs(LVL3FFKEY.SeedThr)>0 ){
        if(((*((int16u*)ptr)>>6) & 63) <abs(LVL3FFKEY.SeedThr) && ((*((int16u*)ptr)>>6) & 63)>0)goto next;
        int count=0;
        for(int k=2;k<num+3;k++){
          if(*(ptr+k)>=6)count++;    //  Here very important parameter (6)
                                     //  should be studied and taken from data
          else if(count)count--;
          if(count>=2)break;
        }
        if(count<2  )goto next;
      }
      else{
        // set failsafe cluster def > 1 strip || ( > 2 && two adj to max >=0)
        integer nmax= (*(int16u*)ptr)>>6;
        if(nmax == 0 || nmax==num){
          // probably reduced mode
          if(num == 0)goto next;
        }  
        else {
          // Probably normal mode
          if(*(ptr+1+nmax)< 16 && *(ptr+3+nmax) < 16)goto next;
        } 
      }
              float coo=0;
            float ss=0;
            float amp=0;
            for(i=0;i<num+1;i++){
             integer s2;
             switch (strip+i) {
              case 0:
               s2=0;
               break;
              case 639:
               s2=1282;
               break;
              default:
               s2=(strip+i)*2+2;
             }
             ss+= ((int)*(ptr+i+2)) * s2;
             amp+=((int)*(ptr+i+2));
  
            }
            if(amp)ss=ss/amp;

     
      coo=_TrackerCoo[drp][crate][1]+
      _TrackerDir[drp][crate]*_stripsize*(0.5+ss);
//       cout << "Trigger3  s/n*8 "<<((*((int16u*)ptr)>>6)&63)<<" lay "<<layer<<" strip "<<strip<<" coo "<<coo<<endl;
      if(AMSFFKEY.Debug){
       cout << "Trigger3  s/n*8 "<<((*((int16u*)ptr)>>6)&63)<<" lay "<<layer<<" strip "<<strip<<" coo "<<coo<<endl;
      }
      if (coo > plvl3->getlowlimitY(layer) && coo < plvl3->getupperlimitY(layer)){
//           cout << "added  y ok "<<endl;
        if((_TrackerCoo[drp][crate][0]-0.1 <0 && plvl3->getlowlimitX(layer) < 0) ||
           (_TrackerCoo[drp][crate][0]+0.1>0 && plvl3->getupperlimitX(layer) > 0) ){
//           cout << "added ok "<<endl;
           if(!(plvl3->addnewhit(coo,amp,layer,drp+crate*NTRHDRP))){
           plvl3->TrackerTrigger()=2;
           goto formed;
           }             
        }
      }
     }
     next:
     ptr = aux[crate].readtracker();    
  }
 }
  plvl3->fit(idum);

 formed:
   plvl3->Finalize(); 
   tt2=HighResTime();
      }
       if(plvl3->MainTrigger()>=LVL3FFKEY.Accept){ 
         plvl3->settime(tt2-tt1);
         if(LVL3FFKEY.Stat){
           HF1(-400001,float(plvl3->_Time),1.);
         }
         AMSEvent::gethead()->addnext(AMSID("TriggerLVL3",0),plvl3);
         _flowc[13]+=1;
       }
       else delete plvl3;
       AMSgObj::BookTimer.stop("LVL3");

     }
    }



void TriggerLVL302::printfc(){
  printf("\n");
  printf("    ====================== JOB LVL3-statistics ======================\n");
  printf("\n");
  printf(" LVL3-build entries         : % 6d\n",_flowc[0]);
  printf(" LVL1 OK                    : % 6d\n",_flowc[1]);
  printf(" TOF-dir build entries      : % 6d\n",_flowc[2]);
  printf("     dir info found         : % 6d\n",_flowc[3]);
  printf(" EC-EM/TRACK build entries  : % 6d\n",_flowc[4]);
  printf("    fired(>=MIP)            : % 6d\n",_flowc[5]);
  printf("    EM or HE(>20gev) found  : % 6d\n",_flowc[6]);
  printf(" EC-TRACK-build entries     : % 6d\n",_flowc[7]);
  printf("    fired(>=MIP)            : % 6d\n",_flowc[8]);
  printf("    EC-track found          : % 6d\n",_flowc[9]);
  printf(" TOF/TRD-path found         : % 6d\n",_flowc[10]);
  printf("    +  EC-track found       : % 6d\n",_flowc[11]);
  printf("    +  EC-track matched     : % 6d\n",_flowc[12]);
  printf(" LVL3 OK                    : % 6d\n",_flowc[13]);
  printf("\n");
  printf("    =================================================================\n");
}






void TriggerLVL302::_writeEl(){
  LVL3Ntuple02* lvl3N = AMSJob::gethead()->getntuple()->Get_lvl302();

  if (lvl3N->Nlvl3>=MAXLVL3) return;

// Fill the ntuple
#ifdef __WRITEROOT__
    int trdhits = TRDAux._NHits[0]+TRDAux._NHits[1];
    int hmult   = TRDAux._HMult;
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  lvl3N->TOFTr[lvl3N->Nlvl3]=_TOFTrigger;
  lvl3N->TRDTr[lvl3N->Nlvl3]=_TRDTrigger;
  lvl3N->TrackerTr[lvl3N->Nlvl3]=_TrackerTrigger;
  lvl3N->MainTr[lvl3N->Nlvl3]=_MainTrigger;
  lvl3N->Direction[lvl3N->Nlvl3]=_TOFDirection;
  lvl3N->NTrHits[lvl3N->Nlvl3]=_NTrHits;
  lvl3N->NPatFound[lvl3N->Nlvl3]=_NPatFound;
  lvl3N->Residual[lvl3N->Nlvl3][0]=_Residual[0];
  lvl3N->Residual[lvl3N->Nlvl3][1]=_Residual[1];
  lvl3N->Pattern[lvl3N->Nlvl3][0]=_Pattern[0];
  lvl3N->Pattern[lvl3N->Nlvl3][1]=_Pattern[1];
  lvl3N->Time[lvl3N->Nlvl3]=_Time;
  lvl3N->ELoss[lvl3N->Nlvl3]=_TrEnergyLoss;
  lvl3N->TRDHits[lvl3N->Nlvl3]=TRDAux._NHits[0]+TRDAux._NHits[1];
  lvl3N->HMult[lvl3N->Nlvl3]=TRDAux._HMult;
  for(int i=0;i<2;i++)lvl3N->TRDPar[lvl3N->Nlvl3][i]=TRDAux._Par[i][0];
  lvl3N->ECemag[lvl3N->Nlvl3]=_ECemag;
  lvl3N->ECmatc[lvl3N->Nlvl3]=_ECmatc;
  for(int i=0;i<4;i++)lvl3N->ECTOFcr[lvl3N->Nlvl3][i]=_ECtofcr[i];
  lvl3N->Nlvl3++;
}

integer TriggerLVL302::TRDAux_DEF::addnewhit(uinteger crate,uinteger udr, uinteger ufe,uinteger ute, uinteger tube, int16u amp){
  //
  // strongly assumin ufe are following each other!!!!!
  //
  uinteger newaddr=ufe|(udr<<3)|(crate<<6);
  if(newaddr!=_lasthaddr){
     if(_nufe>=0 && _nhits[_nufe]<2){
      //cout <<" suppressed "<<AMSTRDIdSoft((_lasthaddr>>6)&1,(_lasthaddr>>3)&7,(_lasthaddr&7),_coo[_nufe][0][0],_coo[_nufe][0][1])<<endl;
     }
     _lasthaddr=newaddr;
  //  suppress guys with only one hit
     if(_nufe<0 || _nhits[_nufe]>1){
      _nufe++;
      if(_nufe>=trigger302const::maxufe)return 0;
     }
     _nhits[_nufe]=0;
     _haddrs[_nufe]=_lasthaddr;
  }
  if(_nhits[_nufe]<trigger302const::maxhitstrd){
   _coo[_nufe][_nhits[_nufe]][0]=ute;
   _coo[_nufe][_nhits[_nufe]][1]=tube;
   _coo[_nufe][_nhits[_nufe]][2]=amp;
   _nhits[_nufe]++;
   //cout <<" added "<<AMSTRDIdSoft((_lasthaddr>>6)&1,(_lasthaddr>>3)&7,(_lasthaddr&7),_coo[_nufe][_nhits[_nufe]-1][0],_coo[_nufe][_nhits[_nufe]-1][1])<<endl;
  }
  return 1;
}


integer TriggerLVL302::addnewhit(geant coo, geant amp, integer layer, integer drp){
  if( _nhits[layer]>= maxtrpl)return 0; 
  _coo[layer][_nhits[layer]]=coo;
  _eloss[layer][_nhits[layer]]=amp;
  _drp[layer][_nhits[layer]]=drp;
  _nhits[layer]=_nhits[layer]+1;
  
  return 1;
  }


void TriggerLVL302::fit(integer idum){
  //
  // Here  LVL3 Tracker Algorithm ( extremely stupid but very effective)
  //
  int i,j,k,l,ic,n1,n2,n3,n4;
  //
  // suppress splitted hits
  //
  for(i=0;i<TKDBc::nlay();i++){
   AMSsortNAGa(_coo[i],_nhits[i]);
   for(j=_nhits[i]-1;j>0;j--){
     if(_coo[i][j]-_coo[i][j-1] < LVL3FFKEY.Splitting){
       for(k=j;k<_nhits[i]-1;k++){
        _coo[i][k]=_coo[i][k+1];
       }
       (_nhits[i])--;
     }
   }
  }

  _NTrHits=0;
  int lla=-1;
  int ula=-1;
  for(int i=0;i<TKDBc::nlay();i++){
   if(_nhits[i]){
    if(lla<0)lla=i;
    ula=i;
    _NTrHits+=_nhits[i];
   }
  }
  if(_NTrHits > LVL3FFKEY.TrMaxHits  ){
   _TrackerTrigger=2;
   return;
  }   

  if(_NTrHits < 3)ic=TKDBc::nlay()-2;
  else if (_NTrHits > TKDBc::nlay())ic=0;
  else ic=TKDBc::nlay()-_NTrHits;
  for (i=ic;i<TKDBc::nlay()-2;i++){      // Start from 4
    for( j=TKDBc::patd(i);j<TKDBc::patd(i+1);j++){
      if(TKDBc::patallowFalseX(j)){ 
      for(k=0;k<_nhits[TKDBc::patconf3(j,0)];k++){
         coou=_coo[TKDBc::patconf3(j,0)][k];
         amp[0]=_eloss[TKDBc::patconf3(j,0)][k];
         for(l=0;l<_nhits[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)];l++){
            cood=_coo[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)][l];
            amp[TKDBc::patpoints(j)-1]=_eloss[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)][l];
            b=_TrackerCooZ[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)]-
            _TrackerCooZ[TKDBc::patconf3(j,0)];
            zmean=_TrackerCooZ[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)]+
            _TrackerCooZ[TKDBc::patconf3(j,0)];
            factor=1/fabs(b);
            s=sqrt((cood-coou)*(cood-coou)+b*b);
            a=(cood-coou);
            dscr=s*Discriminator(_NTrHits-TKDBc::patpoints(j));
            if(!_Level3Searcher(1,j))goto done;
         }
      }
     }
    }
    if(_NPatFound)break;
  }
  done:

   if( _NPatFound  == 0){
     int nht=0;
     for(int i=0;i<TKDBc::nlay();i++)if(_nhits[i])nht++;
     if(nht>TKDBc::nlay()/2 - (ula-lla==TKDBc::nlay()-1?1:0))_TrackerTrigger=5;
     else  _TrackerTrigger=1;
  }
   else if ( _NPatFound >2)_TrackerTrigger = 4;
   else if (_NPatFound ==1){
    if(_Residual[0] > LVL3FFKEY.TrMinResidual )_TrackerTrigger=3;
    else _TrackerTrigger=6;
   }
   else {
    if(_Residual[0] > LVL3FFKEY.TrMinResidual && _Residual[1] > LVL3FFKEY.TrMinResidual )
    _TrackerTrigger=3;
    else if(_Residual[0] <= LVL3FFKEY.TrMinResidual && _Residual[1] <= LVL3FFKEY.TrMinResidual )
    _TrackerTrigger=6;
    else _TrackerTrigger=4;

   }
   if(_TrEnergyLoss > LVL3FFKEY.TrHeavyIonThr*8)_TrackerTrigger+= 8;
     
}

void TriggerLVL302::settime(number time){
   _Time=time/LVL3FFKEY.NRep;
}
  
integer TriggerLVL302::_UpdateOK(geant s,  geant res[], geant amp[],integer pat){
  int i;
  geant maxa;
  if(_NPatFound > 1){
     _NPatFound++;
     return 0;
  }
  else {
     int n=(TKDBc::patpoints(pat)-2);
     for(i=0;i<n;i++)_Residual[_NPatFound]+=res[i];
     _Residual[_NPatFound]=_Residual[_NPatFound]/n/s;     
     _Pattern[_NPatFound]=pat;
     if(_NPatFound++==0){
      maxa=0;
      for(i=0;i<TKDBc::patpoints(pat);i++){
        if(amp[i]>maxa)maxa=amp[i];
        _TrEnergyLoss+=amp[i];       
      }
      _TrEnergyLoss=(_TrEnergyLoss-maxa)/(TKDBc::patpoints(pat)-1);
     }
   return 1;
  }


}



void TriggerLVL302::builddaq(integer i, integer n, int16u *p){
  TriggerLVL302 *ptr=dynamic_cast<TriggerLVL302*>(AMSEvent::gethead()->
  getheadC("TriggerLVL3",i));
  *p=getdaqid(i);
  if(ptr){
    integer toftr=ptr->_TOFTrigger;
    if(toftr==-1)toftr=255;
    uinteger tra=ptr->MainTrigger();
    *(p+1)=int16u(tra);
    *(p+2)=(toftr) | ((ptr->_TRDTrigger)<<8);
    int16 res=int16(ptr->_Residual[0]*1000);
    *(p+3)=int16u(res);
    res=int16(ptr->_Residual[1]*1000);
    *(p+4)=int16u(res);
    *(p+5)=(ptr->_NPatFound) | ((ptr->_NTrHits)<<4);
    *(p+6)= (ptr->_Pattern[0]) |( (ptr->_Pattern[1])<<6);
    *(p+7)=int16u(ptr->_TrEnergyLoss);
  }
  else {
    cerr <<"TriggerLVL302::builddaq-E-No Trigger for "<<i<<endl;
  }
}

void TriggerLVL302::buildraw(integer n, int16u *p){
  integer ic=checkdaqid(*p)-1;
  {
    AMSContainer *ptr=AMSEvent::gethead()->getC("TriggerLVL3",ic);
   if(ptr)ptr->eraseC();
   else cerr <<"TriggerLVL302::buildraw-S-NoContainer"<<endl;
  }

  // not Yet Defined

}

integer TriggerLVL302::MainTrigger(){
 return _MainTrigger;
}




integer TriggerLVL302::checkdaqid(int16u id){
int i;
for(i=0;i<getmaxblocks();i++){
 if(id==getdaqid(i)){
  return i+1;
 }
}
return 0;
}




integer TriggerLVL302::calcdaqlength(integer i){
  TriggerLVL302 *ptr=dynamic_cast<TriggerLVL302*>(AMSEvent::gethead()->
  getheadC("TriggerLVL3",i));
  if(ptr)return 8;
  else return 0;
}

void TriggerLVL302::_printEl(ostream & stream){

  stream << " Lvl3Output " << _TrackerTrigger<<" "<<_TOFTrigger<<" "<<
    _TRDTrigger<<" res "<<_Residual[0]<<" "<<_Residual[1]<<" "<<_NPatFound <<
    " pat "<<_Pattern[0]<<" "<<_Pattern[1]<<" eloss "<<_TrEnergyLoss<<" nhits "<<_NTrHits<<endl;
 for(int j=0;j<TKDBc::nlay();j++){
  for(int i=0;i<_nhits[j];i++){
    stream<<"Lvl3-Layer "<<j+1<<" hit "<<i<<" "<<_coo[j][i]<<endl;    
  }
 }
}

integer TriggerLVL302::LVL3OK(){
  if(_TrackerTrigger ==0  || _TrackerTrigger ==2 ||
     (TRFITFFKEY.FastTracking>1 && _TrackerTrigger ==3) ||
     (TRFITFFKEY.FastTracking>2 && _TrackerTrigger <8 )  ||
     (TRFITFFKEY.FastTracking>3 && _TrackerTrigger <32 ) 
     )return 0;
  else return 1;
}

AMSLVL3Error::AMSLVL3Error(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
}
char * AMSLVL3Error::getmessage(){return msg;}


integer TriggerLVL302::_Level3Searcher(int call, int j){
//              cout <<"Searcher "<<call<<" "<<TKDBc::patconf3(j,call)<<" "<<_nhits[TKDBc::patconf3(j,call)]<<" "<<j<<endl;
             for(int n1=0;n1<_nhits[TKDBc::patconf3(j,call)];n1++){
               r=a*(_TrackerCooZ[TKDBc::patconf3(j,call)]-_TrackerCooZ[TKDBc::patconf3(j,0)])+
               b*(coou-_coo[TKDBc::patconf3(j,call)][n1]);
                 if(fabs(r) < (1-fabs(2*_TrackerCooZ[TKDBc::patconf3(j,call)]-zmean)*factor)*dscr){
                   resid[call-1]=r;
                   amp[call]=_eloss[TKDBc::patconf3(j,call)][n1];
                   if(TKDBc::patpoints(j) > call+2){
                     return _Level3Searcher(++call,j);
                   }
                   else{
//                     cout << "qqs "<<j<<endl;
                     return _UpdateOK(s,resid,amp,j);
                   }
                }
              }
              return -1;
              
}


void TriggerLVL302::Finalize(){
// Finalize main trigger output
  if(_TrackerTrigger%8==1 || _TrackerTrigger==0)_MainTrigger|=1;
  if(((_TRDTrigger>>1)&1) ==0 && UseTRD())_MainTrigger|= 16;
  if(_TrackerTrigger%8==2 )_MainTrigger|=2;
  if(_TRDTrigger%8==4)_MainTrigger|=4;
  if(_TOFTrigger==0)_MainTrigger|=8;
  if((_TOFDirection==0)  && UseTOFTime())_MainTrigger|= 64;
  if((_TOFDirection==-1) )_MainTrigger|= 32;
  if(_TrackerTrigger%8==3  && _TOFDirection==-1)_MainTrigger|=1024;
  else if(_TrackerTrigger%8==3)_MainTrigger|=128;
  if(_TrackerTrigger%8==4 )_MainTrigger|=256;
  if(_TrackerTrigger%8==5 )_MainTrigger|=512;
  if(_TrackerTrigger%8==6 && _TOFDirection==-1)_MainTrigger|=128;
  else if(_TrackerTrigger%8==6)_MainTrigger|=1024;
  if((_TrackerTrigger>>3)&1 )_MainTrigger|=4096;
  if((_TRDTrigger>>3)&1 )_MainTrigger|=2048;
//  bit 13 not set up
  if(UseECEMinfo() && (_ECemag==0))_MainTrigger|=8192*2;//no ECactivity
  if(_ECemag==1)_MainTrigger|=8192*4;//ECelectromagneticity found
  if(UseECMATinfo() && _ECmatc!=0)_MainTrigger|=8192*8;//ECtrack found
  if(_ECmatc==1)_MainTrigger|=8192*16;//ECtrack matched with TOF/TRD path
}


void TriggerLVL302::TRDAux_DEF::build(){


  integer xmax[2]={-1,-1};
  integer tmax[2]={-1,-1};
  integer cmax[2]={-1,-1};
 for(int i=0;i<_nufe;i++){
   uinteger ufe=(_haddrs[i])&7;
   uinteger udr=(_haddrs[i]>>3)&7;
   uinteger crt=(_haddrs[i]>>6)&(trdid::ncrt-1);
   for(int j=0;j<_nhits[i];j++){
    for(int k=j+1;k<_nhits[i];k++){
      if(_coo[i][j][0] < _coo[i][k][0]){
        int l1=_coo[i][j][0];
        int l2=_coo[i][k][0]-1;
        int t1=_coo[i][j][1];
        int t2=_coo[i][k][1];
        geant ta=_IncMatrix[l1][t1][l2][t2];
        geant coo=_CooMatrix[l1][t1][l2][t2];
//      translate to global coordinates;
        coo+=ta*(_Coo[0][0][0][2]-_Coo[ufe][udr][crt][2])+_Coo[ufe][udr][crt][_Dir[ufe][udr][crt]];
//        cout <<" ufe/udr/crt "<<ufe<<" "<<udr<< " "<<crt<<" "<<l1<<" "<<l2<<" "<<t1<<" "<<t2<<" "<<_Dir[ufe][udr][crt]<<" "<<ta<<" "<<coo<<endl;        
        uinteger itan=(ta-_TanLimits[0])/_TanBinSize+0.5;
        uinteger icoo=(coo-_CooLimits[0])/_CooBinSize+0.5;
         if(itan<trigger302const::matrixsize && icoo<trigger302const::matrixsize){
        _DistrMatrix[_Dir[ufe][udr][crt]][itan][icoo]++;
        if( xmax[_Dir[ufe][udr][crt]]<_DistrMatrix[_Dir[ufe][udr][crt]][itan][icoo]){
         xmax[_Dir[ufe][udr][crt]]=_DistrMatrix[_Dir[ufe][udr][crt]][itan][icoo];
         tmax[_Dir[ufe][udr][crt]]=itan;
         cmax[_Dir[ufe][udr][crt]]=icoo;
        }
      }
     }
    }
   }
   }
   for(int i=0;i<2;i++){
   int count =0;
   // search for max
   if(xmax[i]>0){
    for(int j=tmax[i]-1<0?0:tmax[i]-1;j<(tmax[i]+2>trigger302const::matrixsize?trigger302const::matrixsize:tmax[i]+2);j++){
     for(int k=cmax[i]-1<0?0:cmax[i]-1;k<(cmax[i]+2>trigger302const::matrixsize?trigger302const::matrixsize:cmax[i]+2);k++){
      count+=_DistrMatrix[i][j][k];
      
     }
    }
    if(count>2){
      number tlimit[2],climit[2];
      tlimit[0]=(tmax[i]-1)*_TanBinSize+_TanLimits[0];  
      tlimit[1]=tlimit[0]+2*_TanBinSize;
      climit[0]=(cmax[i]-1)*_CooBinSize+_CooLimits[0];  
      climit[1]=climit[0]+2*_CooBinSize;
      //  calculate parameters
      number z=0;
      number z2=0;
      number t=0;
      number tz=0;
 for(int ip=0;ip<_nufe;ip++){
   uinteger ufe=(_haddrs[ip])&7;
   uinteger udr=(_haddrs[ip]>>3)&7;
   uinteger crt=(_haddrs[ip]>>6)&(trdid::ncrt-1);
   if(_Dir[ufe][udr][crt]==i){
   for(int j=0;j<_nhits[ip];j++){
    for(int k=0;k<_nhits[ip];k++){
      if(_coo[i][j][0] != _coo[i][k][0]){
        int l1=_coo[ip][j][0];
        int l2=_coo[ip][k][0];
        int t1=_coo[ip][j][1];
        int t2=_coo[ip][k][1];
        geant ta=l1<l2?_IncMatrix[l1][t1][l2-1][t2]:_IncMatrix[l2][t2][l1-1][t1];
        if(ta>tlimit[0] && ta<tlimit[1]){
         geant coo=l1<l2?_CooMatrix[l1][t1][l2-1][t2]:_CooMatrix[l2][t2][l1-1][t1];
//       translate to global coordinates;
         coo+=ta*(_Coo[0][0][0][2]-_Coo[ufe][udr][crt][2])+_Coo[ufe][udr][crt][i];
         if(coo>climit[0] && coo<climit[1]){
           z+=_CooZ[l1][t1]+_Coo[ufe][udr][crt][2];
           t+=_CooT[l1][t1]+_Coo[ufe][udr][crt][i];
           if(_coo[ip][j][2]>LVL3FFKEY.TRDHMulThr)_HMult++;
           _NHits[i]++;
           z2+=(_CooZ[l1][t1]+_Coo[ufe][udr][crt][2])*(_CooZ[l1][t1]+_Coo[ufe][udr][crt][2]);
           tz+=(_CooZ[l1][t1]+_Coo[ufe][udr][crt][2])*(_CooT[l1][t1]+_Coo[ufe][udr][crt][i]);
            break;
         }
         }
        }
      }
     }
     }
    }
   if(_NHits[i]){
    z/=_NHits[i];
    t/=_NHits[i];
    z2/=_NHits[i];
    tz/=_NHits[i];
    if(z2>z*z){
     _SegmentFound[i]=true;
     _Par[i][0]=(tz-t*z)/(z2-z*z);
     _Par[i][1]=t;
     _Par[i][2]=z;
     _Par[i][3]=0.5*TRDDBc::TubeInnerDiameter()/sqrt((z2-z*z)*_NHits[i]);
     _Par[i][4]=tz;
     _Par[i][5]=z2;
    }
  }
   }
   }
   }
      
    
    


}
