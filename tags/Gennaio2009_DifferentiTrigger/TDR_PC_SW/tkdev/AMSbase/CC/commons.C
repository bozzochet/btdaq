//  $Id: commons.C,v 1.7 2008/10/23 10:12:39 zuccon Exp $


#include "commons.h"
//
STATUSFFKEY_DEF STATUSFFKEY;
SELECTFFKEY_DEF SELECTFFKEY;
AMSFFKEY_DEF AMSFFKEY;

void IOPA_DEF::init(){
  hlun=0;
  memset(hfile,40,sizeof(int));
  ntuple=1;
  Portion=.2;
  WriteAll=102;
  memset(TriggerC,40,sizeof(int));

  mode=0;
  memset(ffile,40,sizeof(int));
  MaxNtupleEntries=10000000;
  MaxFileSize=4000000;
  MaxFileTime=86400*3;
  BuildMin=-1;
  WriteRoot=0;
  memset(rfile,40,sizeof(int));
}


IOPA_DEF IOPA;
//=============================
TFMCFFKEY_DEF TFMCFFKEY;
TFREFFKEY_DEF TFREFFKEY;
TFCAFFKEY_DEF TFCAFFKEY;
//=============================
void MAGSFFKEY_DEF::init(){
  magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
  fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
  ecutge=0.001; //(3) e/g ener.cut for tracking in magnet materials(Gev) 
  BTempCorrection=1;
  BZCorr=1;
}

MAGSFFKEY_DEF MAGSFFKEY;
//=============================
ECMCFFKEY_DEF ECMCFFKEY;
ECREFFKEY_DEF ECREFFKEY;
ECCAFFKEY_DEF ECCAFFKEY;
//=============================
ATGEFFKEY_DEF ATGEFFKEY;
ATMCFFKEY_DEF ATMCFFKEY;
ATREFFKEY_DEF ATREFFKEY;
ATCAFFKEY_DEF ATCAFFKEY;
//=============================
TRDMCFFKEY_DEF TRDMCFFKEY;
TRDFITFFKEY_DEF TRDFITFFKEY;
TRDRECFFKEY_DEF TRDRECFFKEY;
TRDCLFFKEY_DEF TRDCLFFKEY;
MISCFFKEY_DEF MISCFFKEY;
BETAFITFFKEY_DEF BETAFITFFKEY;
CHARGEFITFFKEY_DEF CHARGEFITFFKEY;


void CCFFKEY_DEF::init(){
  for (int ii=0;ii<6;ii++){
    coo[ii]=0;       
    dir[ii]=0;       //7-12
  }
  momr[0]=1.;      //13-14
  momr[1]=10.;      //13-14


 fixp=0;       //15
 albedor=0;      //16
 albedocz=0;     //17
 npat=1;       //18
 run=0;
 low=0;
 earth=0;      // earth mafnetic field modulation
 theta=0;         // station theta at begin
 phi=0;           // station phi at begin
 polephi=0;       // north pole phi
 begindate=0;      // time begin format ddmmyyyy
 begintime=0;      // time begin format hhmmss
 enddate=0;        // time end   format ddmmyyyy
 endtime=0;        // time end   format hhmmss
 sdir=0;            // direction + 1 to higher -1 to lower theta
 oldformat=0;
 Fast=0;            // fast generation
 StrCharge=0;
 StrMass=0; 
 SpecialCut=0;     // special cut to accept
                          //  1:  focus on ecal 
 memset(FluxFile,40,sizeof(FluxFile[0]));
 memset(DirFile,40,sizeof(DirFile[0]));
 memset(DirFilePositions,2,sizeof(DirFilePositions[0]));
 curtime=0;        // current event time
 Angle=0;
}


CCFFKEY_DEF CCFFKEY;
GMFFKEY_DEF GMFFKEY; //ISN
CALIB_DEF CALIB;
TRDCALIB_DEF TRDCALIB;
G4FFKEY_DEF G4FFKEY;
//=============================
TGL1FFKEY_DEF TGL1FFKEY;
//=============================
DAQCFFKEY_DEF DAQCFFKEY;
LVL3FFKEY_DEF LVL3FFKEY;
LVL3SIMFFKEY_DEF LVL3SIMFFKEY;
LVL3EXPFFKEY_DEF LVL3EXPFFKEY;
TRMFFKEY_DEF TRMFFKEY;
//
RICGEOM_DEF RICGEOM;
RICCONTROLFFKEY_DEF RICCONTROLFFKEY;
RICFFKEY_DEF RICFFKEY;
RICRECFFKEY_DEF RICRECFFKEY;
RICRADSETUPFFKEY_DEF RICRADSETUPFFKEY;
PRODFFKEY_DEF PRODFFKEY;
// Geant  commons

//GCFLAG_DEF GCFLAG;
//GCLINK_DEF GCLINK;
//GCVOLU_DEF GCVOLU;
//GCTMED_DEF GCTMED;
//GCTRAK_DEF GCTRAK;
//GCKINE_DEF GCKINE;

// Fortran commons
TKFIELD_DEF TKFIELD;
// AMSDATADIR_DEF AMSDATADIR

