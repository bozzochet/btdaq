// $Id: job.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
// TOF,CTC codes added 29-sep-1996 by E.Choumilov 
// ANTI codes added 5.08.97 E.Choumilov
// Sep 17, 1997. ak. don't write timeid to files in dbase version
// Oct  1, 1997. ak. add FindTheBestTDV, FillTDVTable functions
// Nov   , 1997. ak. FindTheBestTDV, check id's
// Dec   , 1997. ak. dbendjob is modified
//
// Last Edit : Sept 11, 2007. pz.
// Dec 27, 2005 and befor: lots of changes for TOF,ANTI,ECAL,TrigLVL1,...by E.Choumilov. 
//
//  All the datacard default values have been moved to dcards.C
//

#include "tofdbc02.h"
#include "ecaldbc.h"
#include "daqecblock.h"
#include "ecalcalib.h"
#include "antidbc02.h"
#include "amsgobj.h"
#include "astring.h"
#include "cern.h"
#include <math.h>
#include "amsdbc.h"
#include "commons.h"
#include <iostream.h>
#include <fstream.h>
//#include "trid.h"
#include "mccluster.h"
#include "extC.h"
#include "job.h"
#include "event.h"
#include "charge.h"
#include "timeid.h"
#include "mceventg.h"
#include "trcalib.h"
#include "antidbc02.h"
#include "tofsim02.h"
#include "tofrec02.h"
#include "tofcalib02.h"
#include "anticalib02.h"
#include "trigger102.h"
//#include <trigger3.h>
#include "trigger302.h"
#include "bcorr.h"
#include "trid.h"
#include "trrawcluster.h"
#include "ecalrec.h"
#include "daqevt.h"
#include "daqs2block.h"
#include "ntuple.h"
#include "user.h"
#include "tralig.h"
#include "status.h"
#include "richdbc.h"
#include "richid.h"
#include "richradid.h"
#include "mccluster.h"
#include <sys/stat.h>
#include "producer.h"
#include "trdid.h"
#include "ecid.h"
#include "daqecblock.h"
#include "tofid.h"
#include "charge.h"
#include "trkelospdf.h"
#ifdef __DB__
//+
integer        ntdvNames;               // number of TDV's types
char*          tdvNameTab[maxtdv];      // TDV's nomenclature
int            tdvIdTab[maxtdv];
//

#include <dbS.h>
integer*   AMSJob::_ptr_start;
integer*   AMSJob::_ptr_end;
tdv_time*  AMSJob::_tdv;

extern LMS* lms;

#endif
//-
//
using namespace AMSChargConst;
//
char AMSJob::_ntuplefilename[256]="";
char AMSJob::_rootfilename[256]="";

long AMSJob::GetNtupleFileSize(){
  struct stat buffer;
  long size_n=stat(_ntuplefilename,&buffer)? 0:buffer.st_size;
  long size_r=stat(_rootfilename,&buffer)? 0:buffer.st_size;
  return size_n>size_r?size_n:size_r;
}
time_t AMSJob::GetNtupleFileTime(){
  struct stat buffer;
  time_t timenow=0;
  time(&timenow);
  time_t size_n=stat(_ntuplefilename,&buffer)? 0:buffer.st_atime;
  time_t size_r=stat(_rootfilename,&buffer)? 0:buffer.st_atime;
  return size_n>size_r?timenow-size_n:timenow-size_r;
}

AMSNtuple* AMSJob::_pntuple=0;
AMSJob* AMSJob::_Head=0;
AMSNodeMap AMSJob::JobMap;
integer AMSJob::debug=0;
extern "C" void npq_();
const uinteger AMSJob::Reconstruction=1;
const uinteger AMSJob::RealData=2;
const uinteger AMSJob::CTracker=4;
const uinteger AMSJob::CTOF=8;
const uinteger AMSJob::CAnti=16;
const uinteger AMSJob::CCerenkov=32;
const uinteger AMSJob::CMagnet=64;
const uinteger AMSJob::CRICH=128;
const uinteger AMSJob::CTRD=256;
const uinteger AMSJob::CSRD=32;       // Same as cerenkov 
const uinteger AMSJob::CAMS=512;
const uinteger AMSJob::CEcal=1024;
const uinteger AMSJob::MTracker=2048;
const uinteger AMSJob::MTOF=4096;
const uinteger AMSJob::MAnti=4096*2;
const uinteger AMSJob::MCerenkov=4096*4;
const uinteger AMSJob::MLVL1=4096*8;
const uinteger AMSJob::MLVL3=4096*16;
const uinteger AMSJob::MAxAMS=4096*32;
const uinteger AMSJob::MAll=4096*64;
const uinteger AMSJob::MEcal=4096*128;
const uinteger AMSJob::Production=4096*256;
const uinteger AMSJob::Calibration=AMSJob::CTracker+
  AMSJob::CTOF+
  AMSJob::CAnti+
  AMSJob::CCerenkov+
  AMSJob::CMagnet+
  AMSJob::CRICH+
  AMSJob::CTRD+
  AMSJob::CEcal+
  AMSJob::CAMS;

const uinteger AMSJob::Monitoring=(AMSJob::MTracker)+
  (AMSJob::MTOF)+
  (AMSJob::MAnti)+
  (AMSJob::MEcal)+
  (AMSJob::MCerenkov)+
  (AMSJob::MLVL1)+
  (AMSJob::MLVL3)+
  (AMSJob::MAxAMS)+
  (AMSJob::MAll);
//


//
void AMSJob::data(){
#ifdef __HPUX__
  AMSgObj::GTrMedMap=*(new  AMSNodeMap() );
  AMSgObj::GVolMap= *(new  AMSNodeMap() );
  AMSgObj::BookTimer= *(new AMSStat() );
#endif  

  GCSTAK.NJTMIN=1;
  GCTRAK.upwght=0;
  GCTIME.TIMINT=10000000;
  GCTIME.TIMEND=10;
  GCTIME.ITIME=0;

  char * SELECT="SELECT";
  char * MIXED="MIXED";
  SELECTFFKEY.init();
  FFKEY(SELECT,(float*)&SELECTFFKEY,sizeof(SELECTFFKEY_DEF)/sizeof(integer),MIXED);

  STATUSFFKEY.init();
  FFKEY("ESTATUS",(float*)&STATUSFFKEY,sizeof(STATUSFFKEY_DEF)/sizeof(integer),"MIXED");

 
  // Set Defaults
  AMSFFKEY.init();
  FFKEY("AMSJOB",(float*)&AMSFFKEY+2,sizeof(AMSFFKEY_DEF)/sizeof(integer),"MIXED");
  _siamsdata();
  _reamsdata();
}

void AMSJob::_siamsdata(){
  IOPA.init();
  FFKEY("IOPA",(float*)&IOPA,sizeof(IOPA_DEF)/sizeof(integer),"MIXED");

  TRMFFKEY.init();
  FFKEY("TERM",(float*)&TRMFFKEY,sizeof(TRMFFKEY_DEF)/sizeof(integer),"MIXED");

  MISCFFKEY.init();
  FFKEY("MISC",(float*)&MISCFFKEY,sizeof(MISCFFKEY_DEF)/sizeof(integer),"MIXED");

  G4FFKEY.init();
  FFKEY("G4FF",(float*)&G4FFKEY,sizeof(G4FFKEY_DEF)/sizeof(integer),"MIXED");
  
  PRODFFKEY.init();
  FFKEY("PROD",(float*)&PRODFFKEY,sizeof(PRODFFKEY_DEF)/sizeof(integer),"MIXED");
  

  _sitkdata();
  _signdata();
  
  _sitof2data();
  _sianti2data();
  _sitrig2data();
  
  _sirichdata();
  _siecaldata();
  _sitrddata();
  _sisrddata();
  _simag2data();
 
}
//
//========================================================================
//
void AMSJob::_sirichdata(){
  
  // Geometry defaults: unused
  //RICGEOM.init();
  //  FFKEY("RICGEOM",(float *)&RICGEOM,sizeof(RICGEOM_DEF)/sizeof(integer),"REAL");
  
  // Move information to fortran commons
  RICGTKOV.usrcla=RICHDB::rad_clarity;
  RICGTKOV.scatprob=RICHDB::scatprob;
  RICGTKOV.scatang=RICHDB::scatang;

  RICCONTROLFFKEY.init();
  FFKEY("RICCONT",(float *)&RICCONTROLFFKEY,sizeof(RICCONTROLFFKEY_DEF)/sizeof(integer),"MIXED");

  RICRADSETUPFFKEY.init();
  FFKEY("RICSET",(float *)&RICRADSETUPFFKEY,sizeof(RICRADSETUPFFKEY_DEF)/sizeof(integer),"MIXED");
  
  RICRECFFKEY.init();
  FFKEY("RICREC",(float *)&RICRECFFKEY,sizeof(RICRECFFKEY_DEF)/sizeof(integer),"MIXED");

  RICFFKEY.init();  
  FFKEY("RICAL",(float*)&RICFFKEY,sizeof(RICFFKEY_DEF)/sizeof(integer),"INTEGER");



}

//------------------------------------------------------
void AMSJob::_sitrig2data(){
  TGL1FFKEY.init();
  FFKEY("TGL1",(float*)&TGL1FFKEY,sizeof(TGL1FFKEY_DEF)/sizeof(integer),"MIXED");
  //----
  LVL3SIMFFKEY.init();
  FFKEY("L3SIM",(float*)&LVL3SIMFFKEY,sizeof(LVL3SIMFFKEY_DEF)/sizeof(integer),"MIXED");


  LVL3EXPFFKEY.init();
  FFKEY("L3EXP",(float*)&LVL3EXPFFKEY,sizeof(LVL3EXPFFKEY_DEF)/sizeof(integer),"MIXED");

  LVL3FFKEY.init();
  FFKEY("L3REC",(float*)&LVL3FFKEY,sizeof(LVL3FFKEY_DEF)/sizeof(integer),"MIXED");

}
//
//===============================================================================
//
void AMSJob::_sitkdata(){
  TKGEOMFFKEY.init();
  FFKEY("TKGE",(float*)&TKGEOMFFKEY,sizeof(TKGEOMFFKEY_DEF)/sizeof(integer),
	"MIXED");
  
  TRMCFFKEY.init();
  FFKEY("TRMC",(float*)&TRMCFFKEY,sizeof(TRMCFFKEY_DEF)/sizeof(integer),"MIXED");
  
  TRCALIB.init();
  FFKEY("TRCALIB",(float*)&TRCALIB,sizeof(TRCALIB_DEF)/sizeof(integer),"MIXED");
  
  TRALIG.init();
  FFKEY("TRALIG",(float*)&TRALIG,sizeof(TRALIG_DEF)/sizeof(integer),"MIXED");    
 
}

void AMSJob::_signdata(){
  //event generator
  CCFFKEY.init();
  FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");
  
  GMFFKEY.init();
  FFKEY("GMSRC",(float*)&GMFFKEY,sizeof(GMFFKEY_DEF)/sizeof(integer),"MIXED");
}

//=================================================================================
//
void AMSJob::_sitof2data(){
  TFMCFFKEY.init();
  FFKEY("TFMC",(float*)&TFMCFFKEY,sizeof(TFMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
void AMSJob::_simag2data(){
  FFKEY("MAGS",(float*)&MAGSFFKEY,sizeof(MAGSFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================

void AMSJob::_siecaldata(){

  ECMCFFKEY.init();
  FFKEY("ECMC",(float*)&ECMCFFKEY,sizeof(ECMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//---------------------------


void AMSJob::_reecaldata(){
  ECREFFKEY.init();
  FFKEY("ECRE",(float*)&ECREFFKEY,sizeof(ECREFFKEY_DEF)/sizeof(integer),"MIXED");
  //
  // REUN-Calibration  parameters:
  ECCAFFKEY.init();
  FFKEY("ECCA",(float*)&ECCAFFKEY,sizeof(ECCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
//
void AMSJob::_sianti2data(){
  
  ATGEFFKEY.init();
  FFKEY("ATGE",(float*)&ATGEFFKEY,sizeof(ATGEFFKEY_DEF)/sizeof(integer),
	"MIXED");
  ATMCFFKEY.init();
  FFKEY("ATMC",(float*)&ATMCFFKEY,sizeof(ATMCFFKEY_DEF)/sizeof(integer),
	"MIXED");
}
//================================================================================

void AMSJob::_sitrddata(){
  TRDMCFFKEY.init();
  FFKEY("TRDMC",(float*)&TRDMCFFKEY,sizeof(TRDMCFFKEY_DEF)/sizeof(integer),"MIXED");

  TRDRECFFKEY.init();
  FFKEY("TRDRE",(float*)&TRDRECFFKEY,sizeof(TRDRECFFKEY_DEF)/sizeof(integer),"MIXED");

  TRDFITFFKEY.init();  
  FFKEY("TRDFI",(float*)&TRDFITFFKEY,sizeof(TRDFITFFKEY_DEF)/sizeof(integer),"MIXED");

  //warning it must be called after LVL3 and TRDMCFFKEY
  TRDCLFFKEY.init();
  FFKEY("TRDCL",(float*)&TRDCLFFKEY,sizeof(TRDCLFFKEY_DEF)/sizeof(integer),"MIXED");

}

void AMSJob::_sisrddata(){
}

void AMSJob:: _reamsdata(){
  _remfdata();
  _retkdata();
 
  _retof2data();
  _reanti2data();
 
  _reecaldata();
  _retrddata();
  _resrddata();
  _reaxdata();
  _redaqdata();
}

void AMSJob::_remfdata(){
  TKFIELD.init();
  FFKEY("BMAP",(float*)&TKFIELD,60,"MIXED");

}

void AMSJob::_retkdata(){


  TRCLFFKEY.init();
  FFKEY("TRCL",(float*)&TRCLFFKEY,sizeof(TRCLFFKEY_DEF)/sizeof(integer),"MIXED");
  
  // Fit Par
  TRFITFFKEY.init();
  FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
  
  TKFINI();
}


//=================================================================================
//
void AMSJob::_retof2data(){
  TFREFFKEY.init();
  FFKEY("TFRE",(float*)&TFREFFKEY,sizeof(TFREFFKEY_DEF)/sizeof(integer),
	"MIXED");

  //    defaults for calibration:
  TFCAFFKEY.init();
  FFKEY("TFCA",(float*)&TFCAFFKEY,sizeof(TFCAFFKEY_DEF)/sizeof(integer),"MIXED");
}

//======================================================================
void AMSJob::_reanti2data(){
  ATREFFKEY.init();
  FFKEY("ATRE",(float*)&ATREFFKEY,sizeof(ATREFFKEY_DEF)/sizeof(integer),"MIXED");

  // defaults for calibration:
  ATCAFFKEY.init();
  FFKEY("ATCA",(float*)&ATCAFFKEY,sizeof(ATCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//========================================================================
void AMSJob::_redaqdata(){
  DAQCFFKEY.init();
  FFKEY("DAQC",(float*)&DAQCFFKEY,sizeof(DAQCFFKEY_DEF)/sizeof(integer),"MIXED");

}

//----------------------------------
void AMSJob::_reaxdata(){
  // Fit beta & charge
  //-------
  BETAFITFFKEY.init();
  FFKEY("BETAFIT",(float*)&BETAFITFFKEY,sizeof(BETAFITFFKEY_DEF)/sizeof(integer),"MIXED");
  
  CHARGEFITFFKEY.init();
  FFKEY("CHARGEFIT",(float*)&CHARGEFITFFKEY,sizeof(CHARGEFITFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_retrddata(){
}
void AMSJob::_resrddata(){
}


//=========================================================================
//=========================================================================
void AMSJob::udata(){

  if(CCFFKEY.StrMass<0){
    CCFFKEY.StrMass=0.938*pow(CCFFKEY.StrCharge/0.3,1.5);
  }
  if(GCKINE.ipart==113){
    cout <<" Stranglet Parameters Are Charge: "<<CCFFKEY.StrCharge<<" Mass: "<<CCFFKEY.StrMass<<endl;
  }


  GCTIME.TIMEND=GCTIME.TIMINT;
  GCTIME.ITIME=0;
  if(!MISCFFKEY.G4On && !MISCFFKEY.G3On){
    cerr<<"<---- AMSJob::udata-F-NeitherGeant3NorGeant4Selected"<<endl;
    exit(1);
  }
  else if(MISCFFKEY.G4On && MISCFFKEY.G3On){
    cerr<<"<---- AMSJob::udata-W-BothGeant3AndGeant4Selected"<<endl;
    exit(1);
  }
  else if(MISCFFKEY.G4On)cout<<"<---- AMSJob::udata-I-Geant4Selected"<<endl<<endl;
  else cout<<"<---- AMSJob::udata-I-Geant3Selected"<<endl<<endl;

  if(MISCFFKEY.BZCorr !=1){
    cout <<"<---- AMSJob::udata-W-magneticFieldRescaleModeOnWithFactor "<<MISCFFKEY.BZCorr<<endl<<endl;
  }

  if(CCFFKEY.Fast){
    GCPHYS.IHADR=0;
    GCPHYS.IMULS=0;
    GCPHYS.ILOSS=4;
    GCPHYS.IPAIR=0;
    GCPHYS.IBREM=0;
    GCPHYS.ICOMP=0;
    GCPHYS.IPHOT=0;
    GCPHYS.IANNI=0;
    TRMCFFKEY.NoiseOn=0;
    TRFITFFKEY.FastTracking=4;
    LVL3FFKEY.Accept=32;
    cout <<"AMSJob::udata-W-FastSimSelectedSomeDatacardsWereRedefined"<<endl;
  }

  {

    STATUSFFKEY.status[32]=0;
    for(int i=0;i<32;i++){
      STATUSFFKEY.status[32]=STATUSFFKEY.status[32] | 
	STATUSFFKEY.status[i];
    }
  }


  //#ifdef __MASSP__
  if(TRMFFKEY .OKAY!= 1234567890){
    cerr << "Datacards not terminated properly "<<endl;
    cerr << "Please add TERM=1234567890 in your datacards "<<endl;
    exit(1);  
  }
  //#endif




  {
    int i,j,k;
    for(i=0;i<2;i++){
      for(j=0;j<2;j++){
	if(TRMCFFKEY.RawModeOn[i][j][31]==1){
	  for(k=0;k<32;k++)TRMCFFKEY.RawModeOn[i][j][k]=1;
	}
      }
    }
  }


  const integer cl=161;
  char jobname[cl];
  char setupname[cl];
  char triggername[cl];
  char tdvname[1601];
  char ffile[cl];
  char ifile[cl];
  char ofile[cl];
  char sfile[cl];
  UHTOC(SELECTFFKEY.File,cl/sizeof(integer),sfile,cl-1);
  UHTOC(DAQCFFKEY.ifile,cl/sizeof(integer),ifile,cl-1);
  UHTOC(DAQCFFKEY.ofile,cl/sizeof(integer),ofile,cl-1);
  UHTOC(AMSFFKEY.Jobname,cl/sizeof(integer),jobname,cl-1);
  UHTOC(AMSFFKEY.Setupname,cl/sizeof(integer),setupname,cl-1);
  UHTOC(IOPA.ffile,cl/sizeof(integer),ffile,cl-1);
  UHTOC(IOPA.TriggerC,cl/sizeof(integer),triggername,cl-1);
  int i;

  //+
  for (i=cl-2; i>0; i--) {        // should be at least 1 char
    if(jobname[i] == ' ') jobname[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(setupname[i] == ' ') setupname[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(ffile[i] == ' ') ffile[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(ifile[i] == ' ') ifile[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(sfile[i] == ' ') sfile[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(ofile[i] == ' ') ofile[i]='\0';
    else break;
  }

  //-

  int len;
  //=======
  setsetup(setupname);
  if(getsetup())setname(strcat(jobname,getsetup()));
  else{
    cerr<<"AMSJOB::udata-F-NULLSETUP- Setup not defined"<<endl;
    exit(1);
  }
  if(DAQCFFKEY.SCrateinDAQ<0){
    DAQCFFKEY.SCrateinDAQ=0;
  }
  TKDBc::init(TKGEOMFFKEY.ZShift);
  TRDDBc::init();
  {
    int len=cl-1;

    for(i=cl-2;i>=0;i--){
      if(triggername[i]==' '){
	triggername[i]='\0';
	len=i+1;
      }
      else break;
    }
    integer ntrig=0;
    integer nold=0;
    integer orr=0;
    for (i=0;i<len;i++){
      if(triggername[i]=='|' || triggername[i]=='\0' || triggername[i]=='&'){
	// new trigger found
	if(triggername[i]=='|')orr=1;
	triggername[i]='\0';
	if(i-nold>0)settrigger(triggername+nold,ntrig++,orr);
	nold=i+1;
      }
    }
  }
  setjobtype(AMSFFKEY.Jobtype%10 != 0);
  setjobtype(((AMSFFKEY.Jobtype/10)%10 != 0)<<(RealData-1));
  uinteger ical=(AMSFFKEY.Jobtype/100)%10;//1-9
  uinteger ucal=1;
  if(ical)setjobtype(ucal<<(ical+1));
  uinteger imon=(AMSFFKEY.Jobtype/1000)%10;//1-9
  uinteger umon=1;
  if(imon)setjobtype(umon<<(imon+1+9));
  uinteger iprod=(AMSFFKEY.Jobtype/10000)%10;
  if(iprod){
    setjobtype(Production);
    if(IOPA.BuildMin<0){
      cerr<<"AMSJob::udata-F-IOPA.BuildNoNotSetWhileInProductionMode "<< IOPA.BuildMin<<" "<<endl;
      abort();
    }
    else if(IOPA.BuildMin>AMSCommonsI::getbuildno()){
      cerr<<"AMSJob::udata-F-IOPA.MinBuildNoRequired "<< IOPA.BuildMin<<" "<<endl;
      abort();
    }

  }

  if(IOPA.mode && isSimulation()){
    AMSIO::setfile(ffile);
    AMSIO::init(IOPA.mode);
  }
  if(isReconstruction() && DAQCFFKEY.mode%10 ==0)DAQCFFKEY.mode=DAQCFFKEY.mode+1;
  if(isSimulation() && DAQCFFKEY.mode%10 == 1)DAQCFFKEY.mode=DAQCFFKEY.mode-1;
  if(DAQCFFKEY.mode){
    AMSEvent::setfile(sfile);
    DAQEvent::setfiles(ifile,ofile);
  }



  //
  // Read/Write Synchronization
  if(AMSFFKEY.Read > 10 && AMSFFKEY.Read%2==0)AMSFFKEY.Read++;
  if(AMSFFKEY.Write > 0 && AMSFFKEY.Write%2==0)AMSFFKEY.Write++;

  // TDV
  if(AMSFFKEY.Update){
    UHTOC(AMSFFKEY.TDVC,400,tdvname,1600);
    {
      int len=1600;
      for(i=1599;i>=0;i--){
	if(tdvname[i]==' '){
	  tdvname[i]='\0';
	  len=i+1;
	}
	else break;
      }
      integer ntdv=0;
      integer nold=0;
      for (i=0;i<len;i++){
	if(tdvname[i]=='|' || tdvname[i]=='\0'){
	  // new tdv found
	  tdvname[i]='\0';
	  if(i-nold>0)settdv(tdvname+nold,ntdv++);
	  nold=i+1;
	}
      }
    }
  }
  AMSTrIdGeom::init();
  if(strstr(getsetup(),"AMS02") ){    
    AMSTrIdSoft::inittable(2);
    AMSTrIdSoft::init();
    AMSTRDIdSoft::init();
    AMSRICHIdGeom::Init();
    RichRadiatorTileManager::Init();	
    AMSRICHIdSoft::Init();
    AMSTRDIdSoft::inittable();
    AMSECIds::inittable();
  }
  else {
    cerr<<"AMSJob::udate-E-NoAMSTrIdSoftTable exists for setup "<<
      getsetup()<< "yet "<<endl;
    exit(1);
  }


  // TraligGlobalFit
  if(TRALIG.GlobalFit && TRALIG.MaxPatternsPerJob!=1){
    cout <<"AMSJob::udata-I-GlobalFitRequested ";
    TRALIG.MaxPatternsPerJob=1;
    TRALIG.MaxEventsPerFit=499999;
    TRALIG.MinEventsPerFit=999; 
    cout <<"Parameters Changed "<<TRALIG.MaxEventsPerFit<<" "<<TRALIG.MinEventsPerFit<<endl;
  }


  UHTOC(TKFIELD.mfile,160/sizeof(integer),AMSDATADIR.fname,160);

  for(i=159;i>=0;i--){
    if(AMSDATADIR.fname[i]!=' '){
      AMSDATADIR.fname[i+1]=0;
      break;
    }
    if(strlen(AMSDATADIR.fname)<=1)strcpy(AMSDATADIR.fname,"fld97int.txt");
  }


  if(TRDMCFFKEY.mode==-1){
    if(!strstr(getsetup(),"AMSSHUTTLE")){
      TRDMCFFKEY.mode=0;
    }
  }
  if(TRFITFFKEY.FastTracking==-1){
    TRFITFFKEY.FastTracking=1;
  }

  // check delta etc
  if(TRMCFFKEY.gammaA[0]<0){
    TRMCFFKEY.gammaA[0]=0.2;
  }
  if(TRMCFFKEY.delta[0]<0){
    TRMCFFKEY.delta[0]=0.9;
  }
  if(TRMCFFKEY.thr1R[0]<0){
    TRMCFFKEY.thr1R[0]=3;
  }

  if(TRCLFFKEY.Thr1R[0]<0){
    TRCLFFKEY.Thr1R[0]=3;
  }

  char hfile[161];
  UHTOC(IOPA.hfile,40,hfile,160);  
  _hextname=hfile;

  char rfile[161];
  UHTOC(IOPA.rfile,40,rfile,160);  
  _rextname=rfile;

  // check lvl3
  if(LVL3FFKEY.TrMaxResidual[0]<0){
    for (int i=0;i<3;i++)LVL3FFKEY.TrMaxResidual[i]=6*fabs(LVL3FFKEY.TrMaxResidual[i]);
  }

}


void AMSJob::init(){

  AMSEvent::debug=AMSFFKEY.Debug;

  //AMSgObj::BookTimer.book("AMSEvent::event");
  AMSgObj::BookTimer.book("GUOUT");
  AMSgObj::BookTimer.book("GUKINE");

  _siamsinitjob();
  _reamsinitjob();



  if(isCalibration())_caamsinitjob();
  _timeinitjob();
  map(1);
  _dbinitjob();
  cout << *this;
}
void AMSJob::_siamsinitjob(){
  AMSgObj::BookTimer.book("SIAMSEVENT");
  _sitkinitjob();
  _signinitjob();
  _sitof2initjob();
  _sianti2initjob();
  _siecalinitjob();
  _sirichinitjob();
  _sitrdinitjob();
  _sisrdinitjob();
  _sitrig2initjob();
}


void AMSJob::_dbinitjob(){
  AMSgObj::BookTimer.book("EventStatus");
  AMSStatus::init();
}

//-------------------------------------------------------------------------------------------
void AMSJob::_sitrig2initjob(){
  //
  Trigger2LVL1::init();
  if(TGL1FFKEY.Lvl1ConfRead%10>0){//(N) Take TrigConfig(mc|rd) from raw files
    Trigger2LVL1::l1trigconf.read();
  }
  Trigger2LVL1::scalmon.setdefs();//set scalmon(static) defaults for rd/mc
  //--------
  if(TGL1FFKEY.RebuildLVL1)
    cout <<"<---- AMSJob::sitrig2initjob:TriggerLvl1 Will be rebuild from TOF data. Original info will be lost"<<endl<<endl;
  if(LVL3FFKEY.RebuildLVL3==1)
    cout <<"<---- AMSJob::sitrig2initjob:TriggerLvl3 will be rebuild from TOF/Trk data. Original info will be lost"<<endl<<endl;
  else if(LVL3FFKEY.RebuildLVL3)
    cout <<"<---- AMSJob::sitrig2initjob:TriggerLvl3 will be rebuild from TOF/LVL1 data. Original info will be kept"<<endl<<endl;
  AMSgObj::BookTimer.book("LVL3");
  TriggerLVL302::init();  
}
//-------------------------------------------------------------------------------------------
void AMSJob::_sitkinitjob(){
  if(TRMCFFKEY.GenerateConst){
    AString fnam(AMSDATADIR.amsdatadir);
    fnam+="trsigma.hbk";
    int iostat;
    int rstat=1024;
    HROPEN(1,"input",fnam,"P",rstat,iostat);
    if(iostat){
      cerr << "Error opening TrSigmaFile file "<<fnam<<endl;
      throw amsglobalerror("UnableToOpenHistoFile",3);
    }
    char input[]="//input";
    HCDIR(input," ");
    HRIN(0,9999,0);
    //    HPRINT(101);
    //    HPRINT(102);
    //    HPRINT(201);
    //    HPRINT(202);
    for(int l=0;l<2;l++){
      for (int i=0;i<TKDBc::nlay();i++){
	for (int j=0;j<TKDBc::nlad(i+1);j++){
	  for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            number oldone=0;
            for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
	      id.upd(k);
	      geant d;
	      id.setped()=TRMCFFKEY.ped[l]*(1+RNDM(d));
	      id.clearstatus(~0);
	      if(RNDM(d)<TRMCFFKEY.BadCh[l]){
		id.setstatus(AMSDBc::BAD);
		id.setsig()=0.85*HRNDM1(201+l);
	      }
	      else{
		id.setsig()=(l==0?0.76:0.84)*HRNDM1(101+l);
		if(id.getsig()<1){
		  id.setstatus(AMSDBc::BAD);
		}
		else if(id.getsig()>20){
		  id.setsig()=HRNDM1(101+l);
		  if(id.getsig()<1){
		    id.setstatus(AMSDBc::BAD);
		  }
		}
		else if(id.getsig()>10){
		  if(RNDM(d)<0.1*(id.getsig()-10)){
		    id.setsig()=HRNDM1(101+l);
		    if(id.getsig()<1){
		      id.setstatus(AMSDBc::BAD);
		    }
		  }
		}
               
	      } 
	      if(TRMCFFKEY.GenerateConst>1)id.setsig()=TRMCFFKEY.sigma[l]*(0.9+0.2*RNDM(d));
	      id.setgain()=TRMCFFKEY.gain[l];
	      id.setcmnnoise()=TRMCFFKEY.cmn[l]*(1+RNDM(d));
	      id.setindnoise()=oldone+
		AMSTrMCCluster::sitknoiseprob(id ,id.getsig()*TRMCFFKEY.thr1R[l]);
	      oldone=id.getindnoise();
            }
	  }
	}
      }
    }
    HREND("input");
    CLOSEF(1);
    char pawc[]="//PAWC";
    HCDIR(pawc," " );
    HDELET(101);
    HDELET(102);
    HDELET(201);
    HDELET(202);
    HBOOK1(101,"sigx",800,0.,200.,0.);
    HBOOK1(102,"sigy",800,0.,200.,0.);
    HBOOK1(201,"sigx",800,0.,200.,0.);
    HBOOK1(202,"sigy",800,0.,200.,0.);
    for(int l=0;l<2;l++){
      for (int i=0;i<TKDBc::nlay();i++){
	for (int j=0;j<TKDBc::nlad(i+1);j++){
	  for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
	      id.upd(k);
	      float sig=id.getsig();
	      if(id.checkstatus(AMSDBc::BAD)){
		HF1(201+l,sig,1.);
	      }
	      else {
		HF1(101+l,sig,1.);
	      }
            }
	  }
	}
      }
    }

  }

  else TRMCFFKEY.year[1]=TRMCFFKEY.year[0]-1;

  for(int l=0;l<2;l++){
    // book noise prob
    geant a=TRMCFFKEY.thr1R[l]-AMSTrMCCluster::step()/2.;
    geant b=a+AMSTrMCCluster::ncha()*AMSTrMCCluster::step();
    HBOOK1(AMSTrMCCluster::hid(l),"x prob",AMSTrMCCluster::ncha(),a,b,0.);
  }
  AMSgObj::BookTimer.book("GEANTTRACKING");
  AMSgObj::BookTimer.book("GUSTEP",10);
  AMSgObj::BookTimer.book("AMSGUSTEP",10);
  AMSgObj::BookTimer.book("SYSGUSTEP",10);
  AMSgObj::BookTimer.book("TrdRadiationGen",10);
  AMSgObj::BookTimer.book("SITKHITS");
  AMSgObj::BookTimer.book("SITKNOISE");
  AMSgObj::BookTimer.book("SITKDIGIa");
  AMSgObj::BookTimer.book("SITKDIGIb");
  AMSgObj::BookTimer.book("SITKDIGIc");
  AMSgObj::BookTimer.book("TrMCCluster");
}

void AMSJob::_signinitjob(){
  AMSgObj::BookTimer.book("SetTimeCoo");

  AMSmceventg::setcuts(CCFFKEY.coo,CCFFKEY.dir,CCFFKEY.momr,
		       CCFFKEY.fixp,CCFFKEY.albedor,CCFFKEY.albedocz);
  //
  AMSmceventg::setspectra(CCFFKEY.begindate,CCFFKEY.begintime,
			  CCFFKEY.enddate, CCFFKEY.endtime, GCKINE.ikine,CCFFKEY.low);
  
}
//----------------------------------------------------------------------------------------
void AMSJob::_sitof2initjob(){
  //
  if(TFMCFFKEY.daqfmt==0)cout<<"<----- sitof2init-I-Raw Format selected"<<endl;
  else if(TFMCFFKEY.daqfmt==1)cout<<"<----- sitof2init-I-Reduced Format selected"<<endl;
  else {
    cout<<"<----- sitof2init:Error: TOF/aNTI: Unknown Format was requested !!!"<<endl;
    exit(1);
  }
  //
  AMSgObj::BookTimer.book("SITOFDIGI");
  AMSgObj::BookTimer.book("TOF:Ghit->Tovt");
  AMSgObj::BookTimer.book("TOF:Tovt->RwEv");
  AMSgObj::BookTimer.book("TovtPM1loop");
  AMSgObj::BookTimer.book("TovtPM2loop");
  AMSgObj::BookTimer.book("TovtPM2sloopscpmesp");
  AMSgObj::BookTimer.book("TovtPM2sloopscmcscan");
  AMSgObj::BookTimer.book("TovtPM2sloopsum");
  AMSgObj::BookTimer.book("TovtOther");
  //---------(.ReadConstFiles card convention : tfmc->PTS, tfre->DPC)
  if((TFMCFFKEY.ReadConstFiles%100)/10==1
     || TFREFFKEY.ReadConstFiles%10==1){//(T) MC-Time-Distr from ext.files
    //  
    TOFWScan::build();//create scmcscan-objects (MC Bars Time/Eff-distributions)
  }
  if(TFMCFFKEY.ReadConstFiles%10==1 && !isRealData()){//(S)
    TOFBrcalMS::build();//create sc.bar "MC Seeds" scbrcal-objects
  }
  //---------
  if((TFMCFFKEY.ReadConstFiles%1000)/100>0 && !isRealData()){//(P) def.MCPeds from ext.files
    TOFBPeds::mcbuild();//create sc.bar scbrped-objects
  }
  //---------
  //
  TOF2JobStat::bookhistmc();//Book histograms for MC
}
//----------------------------------------------------------------------------------------
void AMSJob::_siecalinitjob(){
  if(ECMCFFKEY.fastsim==1)cout <<"<----- siecinit-I-Fast(Crude) simulation algorithm selected."<<endl;
  else cout <<"<----- siecalinitjob:-I-Slow(Accurate) simulation algorithm selected !!!"<<endl;
  //
  //
  AMSgObj::BookTimer.book("SIECALDIGI");
  AMSgObj::BookTimer.book("SIECALEVENT");
  //
  // ===> Book histograms for MC :
  //
  EcalJobStat::bookhistmc();
  //
  //make Toy RLGA-files[ ...sd.cof(mcseeds) and ...mc.cof(calib.outp)]:
  //    ECREUNcalib::makeToyRLGAfile();//tempor
  //
  // ===> Read Peds from file, otherwise they will be taken from DB
  if(!isRealData()){
    if(ECMCFFKEY.ReadConstFiles%10>0){//P
      ECPMPeds::build();
    }
    if((ECMCFFKEY.ReadConstFiles%100)/10>0){//C(only Seeds parameters(rlga+fiat))
      ECcalibMS::build();
    }
  }
  //
}
//-----------------------------------------------------------------------
void AMSJob::_sirichinitjob(){
  AMSgObj::BookTimer.book("SIRICH");
  AMSgObj::BookTimer.book("SIRINoise");
  RICHDB::bookhist();
}

void AMSJob::_rerichinitjob(){
  AMSgObj::BookTimer.book("RERICH");
  AMSgObj::BookTimer.book("RERICHLIP"); 
  AMSgObj::BookTimer.book("RERICHZ");   
  AMSgObj::BookTimer.book("RERICHBETA");
  AMSgObj::BookTimer.book("RERICHHITS");
}

//-----------------------------------------------------------------------
void AMSJob::_sianti2initjob(){
  AMSgObj::BookTimer.book("SIANTIEVENT");
  //  ATMC: 4=Sp|Rp->Seed/Real MC-peds
  if(ATMCFFKEY.ReadConstFiles%10>0 && !isRealData()){//Real MC-Peds from ext.files
    ANTIPeds::mcbuild();//create anti-sector ANTIPeds::anscped-objects
  }
  if(ATMCFFKEY.ReadConstFiles/10>0 && !isRealData()){//Seed MC Peds from ext.files
    ANTIPedsMS::build();//create anti-sector ANTIPedsMS::anscped-objects
  }
  if(ATMCFFKEY.mcprtf)ANTI2JobStat::bookmch();
}
//-----------------------------------------------------------------------
void AMSJob::_sitrdinitjob(){

  if(TRDMCFFKEY.GenerateConst){
    HBOOK1(8001,"trd ped",1000,0.,1000.,0.);
    HBOOK1(8002,"trd sig",1000,0.,50.,0.);
    HBOOK1(8003,"trd gain",1000,0.,2.,0.);
    for (int i=0;i<TRDDBc::LayersNo(0);i++){
      for (int j=0;j<TRDDBc::LaddersNo(0,i);j++){
	for (int k=0;k<TRDDBc::TubesNo(0,i,j);k++){
	  geant d;
	  AMSTRDIdSoft id(i,j,k);
	  id.setped()=TRDMCFFKEY.ped+rnormx()*TRDMCFFKEY.pedsig;
	  id.clearstatus(~0);
	  id.setsig()=TRDMCFFKEY.sigma*(0.9+0.2*RNDM(d));
	  if(RNDM(d)<TRDMCFFKEY.NoiseLevel){
	    id.setsig()=id.getsig()*10;
	  }
	  id.setgain()=TRDMCFFKEY.gain*(0.9+0.2*RNDM(d));
	  if(RNDM(d)<TRDMCFFKEY.DeadLevel){
	    id.setgain()=id.getgain()*0.1;
	    id.setstatus(AMSDBc::BAD);
	  }
	  HF1(8001,id.getped(),1.);
	  HF1(8002,id.getsig(),1.);
	  HF1(8003,id.getgain(),1.);
	  //
	  //  common noise not yet defined
	  //             id.setcmnnoise()=TRDMCFFKEY.cmn*(0.5+RNDM(d));
	}
      }
    }
  }
  else TRDMCFFKEY.year[1]=TRDMCFFKEY.year[0]-1;

  AMSTRDMCCluster::init();
  AMSgObj::BookTimer.book("SITRDDigi");
 
  if (CCFFKEY.npat>1){
    AMSTRDRawHit::init_delay_table();
  }

}
void AMSJob::_sisrdinitjob(){
}


void AMSJob::_reamsinitjob(){
  AMSgObj::BookTimer.book("WriteEvent");
  AMSgObj::BookTimer.book("REAMSEVENT"); 
  AMSgObj::BookTimer.book("Vtx");

  _remfinitjob();
  _redaq2initjob();
  _retkinitjob();
  _retof2initjob();
  _reanti2initjob();
  _reecalinitjob();
  _retrdinitjob();
  _rerichinitjob();
  _resrdinitjob();
  _reaxinitjob();
  AMSUser::InitJob();
}
//-----------------------------------------------------
void AMSJob::_caamsinitjob(){
  if(isCalibration() & CTracker)_catkinitjob();
  if(isCalibration() & CTOF)_catof2initjob();
  if(isCalibration() & CAnti)_cant2initjob();
  if(isCalibration() & CEcal)_caecinitjob();
  if(isCalibration() & CTRD)_catrdinitjob();
  if(isCalibration() & CSRD)_casrdinitjob();

  if(isCalibration() & CAMS)_caaxinitjob();
  //if(isCalibration() & CCerenkov)_cactcinitjob();
}
//-----------------------------------------------------
void AMSJob::_catkinitjob(){
  cout <<" AMSJob::_catkinitjob()-i_Initialized for Proc no  "<<TRCALIB.CalibProcedureNo<<endl;
  AMSgObj::BookTimer.book("CalTrFill");
  AMSgObj::BookTimer.book("CalTrFit");
  if(TRALIG.UpdateDB){

    gethead()->addup( new AMSTrAligFit());
  }
  if(TRCALIB.CalibProcedureNo == 1 || TRCALIB.CalibProcedureNo==4){
    AMSTrIdCalib::initcalib();
  }
  else if(TRCALIB.CalibProcedureNo == 2){
    int i,j;
    for(i=0;i<nalg;i++){
      for(j=0;j<tkcalpat;j++){
	AMSTrCalibFit::setHead(i,j, new 
			       AMSTrCalibFit(j,TRCALIB.EventsPerIteration[i],TRCALIB.NumberOfIterations[i],i,GCKINE.ikine));
      }
    }
  }
}
//============================================================================
void AMSJob::_catof2initjob(){
  if(TFREFFKEY.relogic[0]==1){
    TOF2STRRcalib::init();// TOF STRR-calibr.
    cout<<"<----- TOF2STRRcalib-init done !!!"<<'\n';
  }
  if(TFREFFKEY.relogic[0]==2){
    TOF2TDIFcalib::init();// TOF TDIF-calibr.
    cout<<"<----- TOF2TDIFcalib-init done !!!"<<'\n';
  }
  if(TFREFFKEY.relogic[0]==3){
    TOF2TZSLcalib::init();// TOF TzSl-calibr.
    cout<<"<----- TOF2TZSLcalib-init done !!!"<<'\n';
  }
  if(TFREFFKEY.relogic[0]==4){
    TOF2AMPLcalib::init();// TOF AMPL-calibr.
    cout<<"<----- TOF2AMPLcalib-init done !!!"<<'\n';
    //   TOF2AVSDcalib::init();// TOF AVSD-calibr.
    //   cout<<"TOF2AVSDcalib-init done !!!"<<'\n';
  }
  if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6 || TFREFFKEY.relogic[0]==7){
    TOFPedCalib::init();// TOF Ped-calibr.
  }
  //
}
//=================================================================================
void AMSJob::_catrdinitjob(){
}
void AMSJob::_casrdinitjob(){
}

//==========================================
void AMSJob::_cant2initjob(){
  if(ATREFFKEY.relogic==1){
    AntiCalib::init();
  }
  if(ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3 || ATREFFKEY.relogic==4){
    ANTPedCalib::init();
  }
}
//==========================================
void AMSJob::_caecinitjob(){
  if(ECREFFKEY.relogic[1]>0 && ECREFFKEY.relogic[1]<4){
    ECREUNcalib::init();// ECAL REUN-calibr.
    cout<<"<----- ECREUNcalib-init done !!!"<<'\n';
  }
  if(ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5 || ECREFFKEY.relogic[1]==6){
    ECPedCalib::init();
  }
 
 
  // AMSECIdCalib::init();
  // AMSECIdCalib::clear();


}
//==========================================

void AMSJob::_caaxinitjob(){
}

void AMSJob::_remfinitjob(){
  READMFIELD();
  if(MISCFFKEY.BTempCorrection){
    cout <<"<---- AMSJob::_remfinitjob-I-Magnetic Field Temp Corrections will be used"<<endl<<endl; 
  }
}


void AMSJob::_retkinitjob(){
  AMSgObj::BookTimer.book("RETKEVENT");
  AMSgObj::BookTimer.book("TrCluster");
  AMSgObj::BookTimer.book("TrClusterRefit");
  AMSgObj::BookTimer.book("TrRecHit");
  AMSgObj::BookTimer.book("TrTrack");
  AMSgObj::BookTimer.book("TrFalseX");
  if(CHARGEFITFFKEY.TrkPDFileRead%10>0){//(Q) Take ChargeCalibPDFs(mc|rd) from raw files
    //
    TrkElosPDF::build();//create TrkElosPDF-objects from TRK raw ChargeCalibFile
  }
}
//===================================================================================
//
void AMSJob::_retof2initjob(){
  //
  AMSgObj::BookTimer.book("RETOFEVENT");
  AMSgObj::BookTimer.book("TOF:DAQ->RwEv");
  AMSgObj::BookTimer.book("TOF:validation");
  AMSgObj::BookTimer.book("TOF:RwEv->RwCl");
  AMSgObj::BookTimer.book("TOF:RwCl->Cl");

  //
  // ===> Clear JOB-statistics counters for SIM/REC :
  //
  TOF2JobStat::clear();
  //
  // ===> Book histograms for REC :
  //
  TOF2JobStat::bookhist();
  //
  //-------------------------
  if((TFREFFKEY.ReadConstFiles%1000)/100>0){//(QDPC) Take ThreshCuts-set from Data-Cards
    TOF2Varp::tofvpar.init(TFREFFKEY.daqthr, TFREFFKEY.cuts);//create ThrCuts-obj from Data-Cards
  }
  //--------
  if(TFREFFKEY.ReadConstFiles%10>0){//(QDPC) Take Paddles CalibConst from CalibFiles
    TOF2Brcal::build();//create scbrcal-objects from CalibFiles
  }
  //--------
  //
  if((TFREFFKEY.ReadConstFiles%100)/10>0 && isRealData()){//(QDPC) Take default RealData-Peds from file
    TOFBPeds::build();//create RealData scbrped-objects from peds-file
  }
  // 
  //--------
  if(TFREFFKEY.ReadConstFiles/1000>0){//(QDPC) Take ChargeCalibPDFs(mc|rd) from files
    TofElosPDF::build();//create PDF-objects from CalibFiles
  }
  //
  //-----------
  AMSTOFCluster::init();
  AMSSCIds::inittable();
  AMSSCIds::selftest();
}
//=============================================================================================
void AMSJob::_reanti2initjob(){
  //
  AMSgObj::BookTimer.book("REANTIEVENT");
  if(ATREFFKEY.reprtf[0]>0)ANTI2JobStat::bookh();
  //
  ANTI2JobStat::clear();
  //-----------
  //
  if(ATREFFKEY.ReadConstFiles/10>0 && isRealData()){// (PC) RealData Peds from ext.files
    //
    ANTIPeds::build();//create sc.bar anscped-objects
  }
  // 
  if(ATREFFKEY.ReadConstFiles%10){//(PVS) StableCalibParams(MC/RD) from ext.files
    ANTI2SPcal::build();//create antispcal-objects for each sector
  }
  if((ATREFFKEY.ReadConstFiles/10)%10>0){//(PVS) VariableCalibParams(MC/RD) from ext.files
    ANTI2VPcal::build();//create antivpcal-objects for each sector
  }
}
//============================================================================================
void AMSJob::_reecalinitjob(){
  //

  cout<<"====> AMSJob::reecalinitjob: starting..."<<endl;
  // setup - data type dep init
  if(ECREFFKEY.SimpleRearLeak[0]<0){
    cout<<"      EcalRearLeak parameters are set to :"<<endl;
    if(isRealData() ){
      ECREFFKEY.SimpleRearLeak[0]=0.015;
      ECREFFKEY.SimpleRearLeak[1]=0.99e-3;
      ECREFFKEY.SimpleRearLeak[2]=3.6;
      ECREFFKEY.SimpleRearLeak[3]=1.045e-3;
      for (int i=0;i<4;i++)cout <<"  "<<ECREFFKEY.SimpleRearLeak[i]<<endl;
    }
    else{
      for(int k=0;k<4;k++){
	if( ECREFFKEY.SimpleRearLeak[k]<0) ECREFFKEY.SimpleRearLeak[k]*=-1;
      }
      for (int i=0;i<4;i++)cout <<"  "<<ECREFFKEY.SimpleRearLeak[i]<<endl;
    }
  }





  integer pr,pl,cell;
  number ct,cl,cz;
  integer fid,cid[4]={0,0,0,0};
  number w[4]={0.,0.,0.,0.};
  //
  AMSgObj::BookTimer.book("REECALEVENT");
  AMSgObj::BookTimer.book("ReEcalShowerFit");
  //
  ECALDBc::getscinfoa(0,0,0,pr,pl,cell,ct,cl,cz);// <--- init. PMCell-readout tables
  //
  EcalJobStat::clear();// Clear JOB-statistics counters for SIM/REC
  //
  EcalJobStat::bookhist();// Book histograms for REC
  //
  //-----------
  // 
  // ===> create Cuts/Thresh parameters (ecalvpar structure) fr.data-cards :
  //              ECREFFKEY.ReadConstFiles=DCP
  if(ECREFFKEY.ReadConstFiles/100>0){//D Thresh/Cuts-object will be created from data-cards  
    ECALVarp::ecalvpar.init(ECREFFKEY.thresh, ECREFFKEY.cuts);
  }
  //
  // ===> create ecpmcal-calib-objects:
  //
  if((ECREFFKEY.ReadConstFiles%100)/10>0){//C  Calib.const-object(MC/RD) will be created from calib-files 
    ECcalib::build();
  } 
  //-------------------------
  // ===> create EC-SubCell peds/sigs (RD):
  //
  if(ECREFFKEY.ReadConstFiles%10>0 && isRealData()){//P  ECPMPeds-Objects will be created from def. calib-file
    ECPMPeds::build();
  }
  //-----------
  cout<<"<---- AMSJob::reecalinitjob: is successfully done !"<<endl<<endl;
}
//===================================================================

void AMSJob::_reaxinitjob(){
  AMSgObj::BookTimer.book("REAXEVENT");
  AMSgObj::BookTimer.book("ReAxRefit");
  AMSgObj::BookTimer.book("ReTRDRefit"); 
  AMSgObj::BookTimer.book("ReTKRefit"); 
  AMSgObj::BookTimer.book("ReGeaneRefit"); 
  AMSgObj::BookTimer.book("ReECRefit"); 
  AMSgObj::BookTimer.book("ReRICHRefit"); 
  AMSgObj::BookTimer.book("ReTOFRefit"); 
  AMSgObj::BookTimer.book("ReAxPid");
  AMSgObj::BookTimer.book("part::loc2gl");


  if (AMSJob::gethead()->isMonitoring()) {

    const   int nids = 17;
  
    int16u ids[nids] =
      { 0x200,
	0x1401,0x1441,0x1481,0x14C1,0x1501,0x1541,0x1581,0x15C1,  //  TOF Raw
	0x1680, 0x1740,                                           //  TRK Reduced
	0x1681, 0x1741,                                           //  TRK Raw
	0x168C, 0x174C,                                           //  TRK Mixed
	0x0440,                                                   //  Level-1
	0x1200                                                    //  Level-3
      };
  
  
    char   *ids_names[]  = {"Length (words) GenBlock    ",
			    "Length (words) TOF(raw) 01 ",
			    "Length (words) TOF(raw) 31 ",
			    "Length (words) TOF(raw) 41 ",
			    "Length (words) TOF(raw) 71 ",
			    "Length (words) TOF(raw) 03 ",
			    "Length (words) TOF(raw) 33 ",
			    "Length (words) TOF(raw) 43 ",
			    "Length (words) TOF(raw) 73 ",
			    "Length (words) TRK(red) 32",
			    "Length (words) TRK(red) 72",
			    "Length (words) TRK(raw) 32",
			    "Length (words) TRK(raw) 72",
			    "Length (words) TRK(mix) 32",
			    "Length (words) TRK(mix) 72",
			    "Length (words) Level1 block ",
			    "Length (words) Level3 block "};

    int nchans[nids] ={100,
		       100,100,100,100,100,100,100,100,
		       1000,1000,
		       800,800,
		       800,800,
		       100, 100};
                    
    for (int i=0; i<nids; i++) {
      int hid  = 300000 + ids[i];
      geant f = nchans[i];
      integer nbin = 50;
      if (i>17) nbin = 100;
      HBOOK1(hid,ids_names[i],nbin,0.,f,0.);
    }
    HBOOK1(300000,"Length (words) event",300,0.,1500.,0.);
    HBOOK1(300001,"Length (words) TOF",200,0.,600.,0.);
    HBOOK1(300002,"Length (words) Tracker",300,0.,1500.,0.);
    HBOOK1(300003,"Time Difference(msec)",500,-0.02,80.-0.02,0);


  }


}

void AMSJob::_retrdinitjob(){
  AMSgObj::BookTimer.book("RETRDEVENT"); 
}
void AMSJob::_resrdinitjob(){
  AMSgObj::BookTimer.book("RESRDEVENT"); 
}

AMSgvolume * AMSJob::getgeom(AMSID id){
  if(id.getname() ==0 && id.getid()==0)id=AMSID(AMSDBc::ams_name,1);
  return (AMSgvolume*)AMSJob::JobMap.getp(id);
}
AMSG4Physics* & AMSJob::getg4physics(){
  //    AMSID id("AMSG4Physics",0);
  //    return (AMSG4Physics*)AMSJob::JobMap.getp(id);
  return _pAMSG4Physics;
}

AMSG4GeneratorInterface* & AMSJob::getg4generator(){
  //    AMSID id("AMSG4GeneratorInterface",0);
  //    return (AMSG4GeneratorInterface*)AMSJob::JobMap.getp(id);
  return _pAMSG4GeneratorInterface;
}

AMSStatus * AMSJob::getstatustable(){
  static AMSStatus * _Head=0;
  if(!_Head){
    _Head=AMSStatus::create(!strstr(getsetup(),"AMSSHUTTLE") );
  }
  return _Head;
}


AMSgmat * AMSJob::getmat(AMSID id){
  if(id.getname() ==0 && id.getid()==0)id=AMSID("Materials:",0);
  return (AMSgmat*)AMSJob::JobMap.getp(id);
}

AMSgtmed * AMSJob::getmed(AMSID id){
  if(id.getname() ==0 && id.getid()==0)id=AMSID("TrackingMedia:",0);
  return (AMSgtmed*)AMSJob::JobMap.getp(id);
}


void AMSJob::setsetup(char *setup){
  if(setup && (strlen(setup)>1 || AMSFFKEY.ZeroSetupOk))strcpy(_Setup,setup);
  else strcpy(_Setup,"AMSSHUTTLE");   //defaults
  
}
void AMSJob::settrigger(char *setup, integer N,integer orr){
  assert(N < maxtrig);
  if(setup){
    strcpy(_TriggerC[N],setup);
  }
  _TriggerI=1;
  _TriggerOr=orr;
  _TriggerN=N+1;
}
void AMSJob::settdv(char *setup, integer N){
  assert(N < maxtdv);
  if(setup){
    if(strlen(setup)<maxtdvsize)strcpy(_TDVC[N],setup);
    else cerr << "AMSJOB::settdv-E-length of "<<setup<< " "<<strlen(setup)<<
      ", exceeds "<<maxtdvsize<<". Card ignored"<<endl;
  }
  _TDVN=N+1;
}
//==========================================================
void AMSJob::_timeinitjob(){
  AMSgObj::BookTimer.book("TDV");
#ifdef __DB__
  AMSgObj::BookTimer.book("TDVdb");
#endif
  static AMSTimeID TID(AMSID("TDV:",0));
  gethead()->addup( &TID);
  //
  // Magnetic Field Map
  //
  AMSTimeID::CType server=AMSTimeID::Standalone;
#ifdef __CORBA__
  if(!AMSProducer::gethead()->IsSolo())server=AMSTimeID::Client;
#endif
  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec=TKFIELD.isec[0];
    begin.tm_min=TKFIELD.imin[0];
    begin.tm_hour=TKFIELD.ihour[0];
    begin.tm_mday=TKFIELD.iday[0];
    begin.tm_mon=TKFIELD.imon[0];
    begin.tm_year=TKFIELD.iyear[0];
    end.tm_sec=TKFIELD.isec[1];
    end.tm_min=TKFIELD.imin[1];
    end.tm_hour=TKFIELD.ihour[1];
    end.tm_mday=TKFIELD.iday[1];
    end.tm_mon=TKFIELD.imon[1];
    end.tm_year=TKFIELD.iyear[1];
    if(strstr(getsetup(),"AMS02D") ){    

      TID.add (new AMSTimeID(AMSID("MagneticFieldMapD",isRealData()),
			     begin,end,sizeof(TKFIELD_DEF)-sizeof(TKFIELD.mfile),(void*)&TKFIELD.iniok,server,1));

    }
    else{
      TID.add (new AMSTimeID(AMSID("MagneticFieldMap",isRealData()),
			     begin,end,sizeof(TKFIELD_DEF)-sizeof(TKFIELD.mfile),(void*)&TKFIELD.iniok,server,1));
    }
    //TID.add (new AMSTimeID(AMSID("MagneticFieldMapAddOn",isRealData()),
    //   begin,end,sizeof(TKFIELDADDON_DEF),(void*)&TKFIELDADDON.iqx,server));
  }
  bool NeededByDefault=isSimulation();
  //----------------------------
  //
  // Pedestals, Gains,  Sigmas & commons noise for tracker
  //      

  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec=TRMCFFKEY.sec[0];
    begin.tm_min=TRMCFFKEY.min[0];
    begin.tm_hour=TRMCFFKEY.hour[0];
    begin.tm_mday=TRMCFFKEY.day[0];
    begin.tm_mon=TRMCFFKEY.mon[0];
    begin.tm_year=TRMCFFKEY.year[0];

    end.tm_sec=TRMCFFKEY.sec[1];
    end.tm_min=TRMCFFKEY.min[1];
    end.tm_hour=TRMCFFKEY.hour[1];
    end.tm_mday=TRMCFFKEY.day[1];
    end.tm_mon=TRMCFFKEY.mon[1];
    end.tm_year=TRMCFFKEY.year[1];
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(0),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::peds[0])*AMSTrIdSoft::_numell,
			   (void*)AMSTrIdSoft::peds,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(1),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::peds[0])*
			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
			   (void*)(AMSTrIdSoft::peds+AMSTrIdSoft::_numell),server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(0),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*AMSTrIdSoft::_numell,
			   (void*)AMSTrIdSoft::sigmaraws,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(1),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*
			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
			   (void*)(AMSTrIdSoft::sigmaraws+AMSTrIdSoft::_numell),server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(0),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numell,
			   (void*)AMSTrIdSoft::gains,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(1),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::gains[0])*
			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
			   (void*)(AMSTrIdSoft::gains+AMSTrIdSoft::_numell),server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(0),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numell,
			   (void*)AMSTrIdSoft::sigmas,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(1),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::sigmas[0])*
			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
			   (void*)(AMSTrIdSoft::sigmas+AMSTrIdSoft::_numell),server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(0),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numell,
			   (void*)AMSTrIdSoft::status,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(1),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::status[0])*
			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
			   (void*)(AMSTrIdSoft::status+AMSTrIdSoft::_numell),server,NeededByDefault));
    /*
      TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.l",isRealData()),
      begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*AMSTrIdSoft::_numell*2,
      (void*)AMSTrIdSoft::rhomatrix,server,NeededByDefault));
      TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.r",isRealData()),
      begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*
      (2*AMSTrIdSoft::_numel-2*AMSTrIdSoft::_numell),
      (void*)(AMSTrIdSoft::rhomatrix+2*AMSTrIdSoft::_numell),server,NeededByDefault));
    */
    TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerCmnNoise(),isRealData()),
			   begin,end,sizeof(AMSTrIdSoft::cmnnoise),
			   (void*)AMSTrIdSoft::cmnnoise,server,NeededByDefault));
    //TID.add (new AMSTimeID(AMSID("TrackerIndNoise",isRealData()),
    //   begin,end,sizeof(AMSTrIdSoft::indnoise[0])*AMSTrIdSoft::_numel,
    //   (void*)AMSTrIdSoft::indnoise,server,NeededByDefault));
  }





  //----------------------------
  //
  // Pedestals, Gains,  Sigmas & commons noise for trd
  //      

  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec=TRDMCFFKEY.sec[0];
    begin.tm_min=TRDMCFFKEY.min[0];
    begin.tm_hour=TRDMCFFKEY.hour[0];
    begin.tm_mday=TRDMCFFKEY.day[0];
    begin.tm_mon=TRDMCFFKEY.mon[0];
    begin.tm_year=TRDMCFFKEY.year[0];

    end.tm_sec=TRDMCFFKEY.sec[1];
    end.tm_min=TRDMCFFKEY.min[1];
    end.tm_hour=TRDMCFFKEY.hour[1];
    end.tm_mday=TRDMCFFKEY.day[1];
    end.tm_mon=TRDMCFFKEY.mon[1];
    end.tm_year=TRDMCFFKEY.year[1];
    TID.add (new AMSTimeID(AMSID("TRDPedestals",isRealData()),
			   begin,end,sizeof(AMSTRDIdSoft::_ped[0])*AMSTRDIdSoft::getpedsize(),
			   (void*)AMSTRDIdSoft::_ped,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID("TRDGains",isRealData()),
			   begin,end,sizeof(AMSTRDIdSoft::_gain[0])*AMSTRDIdSoft::getgaisize(),
			   (void*)AMSTRDIdSoft::_gain,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID("TRDSigmas",isRealData()),
			   begin,end,sizeof(AMSTRDIdSoft::_sig[0])*AMSTRDIdSoft::getsigsize(),
			   (void*)AMSTRDIdSoft::_sig,server,NeededByDefault));
    TID.add (new AMSTimeID(AMSID("TRDStatus",isRealData()),
			   begin,end,sizeof(AMSTRDIdSoft::_status[0])*AMSTRDIdSoft::getstasize(),
			   (void*)AMSTRDIdSoft::_status,server,NeededByDefault));
  }





  //---------------------------------------
  //
  //   TOF : TDV-reservation for calibration parameters of all sc.bars
  //
  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
 
#ifdef __TFADBW__
    time_t bdbw=MISCFFKEY.dbwrbeg;
    time_t edbw=MISCFFKEY.dbwrend;
    int jobt=AMSFFKEY.Jobtype;
    if(jobt==211){//real data reco/calib
      tm * begdbw = localtime(& bdbw);
      TFREFFKEY.sec[0]=begdbw->tm_sec;
      TFREFFKEY.min[0]=begdbw->tm_min;
      TFREFFKEY.hour[0]=begdbw->tm_hour;
      TFREFFKEY.day[0]=begdbw->tm_mday;
      TFREFFKEY.mon[0]=begdbw->tm_mon;
      TFREFFKEY.year[0]=begdbw->tm_year;
      begin.tm_isdst=begdbw->tm_isdst;//to bypass isdst-problem 
      cout<<" Beg YY-M-D :"<<(TFREFFKEY.year[0]+1900)<<" "<<(TFREFFKEY.mon[0]+1)<<" "<<TFREFFKEY.day[0];
      cout<<"  hh:mm:ss :"<<TFREFFKEY.hour[0]<<" "<<TFREFFKEY.min[0]<<" "<<TFREFFKEY.sec[0]<<endl;
      //
      tm * enddbw = localtime(& edbw);
      TFREFFKEY.sec[1]=enddbw->tm_sec;
      TFREFFKEY.min[1]=enddbw->tm_min;
      TFREFFKEY.hour[1]=enddbw->tm_hour;
      TFREFFKEY.day[1]=enddbw->tm_mday;
      TFREFFKEY.mon[1]=enddbw->tm_mon;
      TFREFFKEY.year[1]=enddbw->tm_year;
      end.tm_isdst=enddbw->tm_isdst;
      cout<<" End YY-M-D :"<<(TFREFFKEY.year[1]+1900)<<" "<<(TFREFFKEY.mon[1]+1)<<" "<<TFREFFKEY.day[1];
      cout<<"  hh:mm:ss :"<<TFREFFKEY.hour[1]<<" "<<TFREFFKEY.min[1]<<" "<<TFREFFKEY.sec[1]<<endl;
    }
#endif

    begin.tm_sec=TFREFFKEY.sec[0];
    begin.tm_min=TFREFFKEY.min[0];
    begin.tm_hour=TFREFFKEY.hour[0];
    begin.tm_mday=TFREFFKEY.day[0];
    begin.tm_mon=TFREFFKEY.mon[0];
    begin.tm_year=TFREFFKEY.year[0];

    end.tm_sec=TFREFFKEY.sec[1];
    end.tm_min=TFREFFKEY.min[1];
    end.tm_hour=TFREFFKEY.hour[1];
    end.tm_mday=TFREFFKEY.day[1];
    end.tm_mon=TFREFFKEY.mon[1];
    end.tm_year=TFREFFKEY.year[1];
    //-----
    //tfre->QDPC,tfmc->PTS
    //-----
    if(TFREFFKEY.ReadConstFiles%10==0)end.tm_year=TFREFFKEY.year[0]-1;//CalibConst from DB
  
    TID.add (new AMSTimeID(AMSID("Tofbarcal2",isRealData()),
			   begin,end,TOF2GC::SCBLMX*sizeof(TOF2Brcal::scbrcal[0][0]),
			   (void*)&TOF2Brcal::scbrcal[0][0],server,NeededByDefault));
   
    end.tm_year=TFREFFKEY.year[1];
    //-----
    if((TFREFFKEY.ReadConstFiles%1000)/100==0)end.tm_year=TFREFFKEY.year[0]-1;//ThreshCuts-set from DB

    TID.add (new AMSTimeID(AMSID("Tofvpar2",isRealData()),
			   begin,end,sizeof(TOF2Varp::tofvpar),
			   (void*)&TOF2Varp::tofvpar,server,NeededByDefault));
    
    end.tm_year=TFREFFKEY.year[1];
    //----- 
    if((TFREFFKEY.ReadConstFiles/1000)==0)end.tm_year=TFREFFKEY.year[0]-1;//ChargeCalibPDFs from DB

    TID.add (new AMSTimeID(AMSID("Tofcpdfs",isRealData()),
			   begin,end,sizeof(TofElosPDF::TofEPDFs),
			   (void*)&TofElosPDF::TofEPDFs[0],server,NeededByDefault));
    
    TID.add (new AMSTimeID(AMSID("ChargeIndxTof",isRealData()),
			   begin,end,MaxZTypes*sizeof(AMSCharge::_chargeTOF[0]),
			   (void*)AMSCharge::_chargeTOF,server,NeededByDefault));
   
    end.tm_year=TFREFFKEY.year[1];
    //----- 
    if((TFREFFKEY.ReadConstFiles%100)/10==0 &&
       isRealData())end.tm_year=TFREFFKEY.year[0]-1;//Real data Peds.fromDB
    if((TFMCFFKEY.ReadConstFiles%1000)/100==0 &&
       !isRealData())end.tm_year=TFREFFKEY.year[0]-1;//MC data Peds.fromDB
    TID.add (new AMSTimeID(AMSID("Tofpeds",isRealData()),
			   begin,end,TOF2GC::SCBLMX*sizeof(TOFBPeds::scbrped[0][0]),
			   (void*)&TOFBPeds::scbrped[0][0],server,NeededByDefault));
   
    end.tm_year=TFREFFKEY.year[1];
    //----- 
    if((TFMCFFKEY.ReadConstFiles%100)/10==0 && !isRealData())end.tm_year=TFREFFKEY.year[0]-1;//MCTimeDistr.fromDB

    TID.add (new AMSTimeID(AMSID("Tofmcscans2",0),
			   begin,end,TOF2GC::SCBTPN*sizeof(TOFWScan::scmcscan[0]),
			   (void*)&TOFWScan::scmcscan[0],server,NeededByDefault));
    
    end.tm_year=TFREFFKEY.year[1];
    //-----
    if(!isRealData()){ 
      if(TFMCFFKEY.ReadConstFiles%10==0)end.tm_year=TFREFFKEY.year[0]-1;//"MC Seeds" fromDB
    
      TID.add (new AMSTimeID(AMSID("TofbarcalMS",isRealData()),
			     begin,end,TOF2GC::SCBLMX*sizeof(TOFBrcalMS::scbrcal[0][0]),
			     (void*)&TOFBrcalMS::scbrcal[0][0],server,NeededByDefault));
    
      end.tm_year=TFREFFKEY.year[1];
    }
 
    //
   
  }
  //---------------------------------------
  //
  //   LVL1 : TDV-reservation for Lvl1Config data
  //
  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;


    begin.tm_sec=TGL1FFKEY.sec[0];
    begin.tm_min=TGL1FFKEY.min[0];
    begin.tm_hour=TGL1FFKEY.hour[0];
    begin.tm_mday=TGL1FFKEY.day[0];
    begin.tm_mon=TGL1FFKEY.mon[0];
    begin.tm_year=TGL1FFKEY.year[0];

    end.tm_sec=TGL1FFKEY.sec[1];
    end.tm_min=TGL1FFKEY.min[1];
    end.tm_hour=TGL1FFKEY.hour[1];
    end.tm_mday=TGL1FFKEY.day[1];
    end.tm_mon=TGL1FFKEY.mon[1];
    end.tm_year=TGL1FFKEY.year[1];
    //-----
    //tgl1->MN
    //-----
    if(TGL1FFKEY.Lvl1ConfRead%10==0)end.tm_year=TGL1FFKEY.year[0]-1;//(N)Lvl1Config-data from DB
  
    TID.add (new AMSTimeID(AMSID("Lvl1Config",isRealData()),
			   begin,end,sizeof(Trigger2LVL1::l1trigconf),
			   (void*)&Trigger2LVL1::l1trigconf,server,NeededByDefault));
   
    end.tm_year=TGL1FFKEY.year[1];
  }
  //---------------------------------------
  //
  //   ANTI : TDV-reservation for calibration parameters of all sc.sectors
  //
  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
 
#ifdef __TFADBW__
    time_t bdbw=MISCFFKEY.dbwrbeg;
    time_t edbw=MISCFFKEY.dbwrend;
    int jobt=AMSFFKEY.Jobtype;
    if(jobt==311){//real data reco/calib
      tm * begdbw = localtime(& bdbw);
      ATREFFKEY.sec[0]=begdbw->tm_sec;
      ATREFFKEY.min[0]=begdbw->tm_min;
      ATREFFKEY.hour[0]=begdbw->tm_hour;
      ATREFFKEY.day[0]=begdbw->tm_mday;
      ATREFFKEY.mon[0]=begdbw->tm_mon;
      ATREFFKEY.year[0]=begdbw->tm_year;
      begin.tm_isdst=begdbw->tm_isdst;//to bypass isdst-problem 
      cout<<" Beg YY-M-D :"<<(ATREFFKEY.year[0]+1900)<<" "<<(ATREFFKEY.mon[0]+1)<<" "<<ATREFFKEY.day[0];
      cout<<"  hh:mm:ss :"<<ATREFFKEY.hour[0]<<" "<<ATREFFKEY.min[0]<<" "<<ATREFFKEY.sec[0]<<endl;
      //
      tm * enddbw = localtime(& edbw);
      ATREFFKEY.sec[1]=enddbw->tm_sec;
      ATREFFKEY.min[1]=enddbw->tm_min;
      ATREFFKEY.hour[1]=enddbw->tm_hour;
      ATREFFKEY.day[1]=enddbw->tm_mday;
      ATREFFKEY.mon[1]=enddbw->tm_mon;
      ATREFFKEY.year[1]=enddbw->tm_year;
      end.tm_isdst=enddbw->tm_isdst;
      cout<<" End YY-M-D :"<<(ATREFFKEY.year[1]+1900)<<" "<<(ATREFFKEY.mon[1]+1)<<" "<<ATREFFKEY.day[1];
      cout<<"  hh:mm:ss :"<<ATREFFKEY.hour[1]<<" "<<ATREFFKEY.min[1]<<" "<<ATREFFKEY.sec[1]<<endl;
    }
#endif

    begin.tm_sec=ATREFFKEY.sec[0];
    begin.tm_min=ATREFFKEY.min[0];
    begin.tm_hour=ATREFFKEY.hour[0];
    begin.tm_mday=ATREFFKEY.day[0];
    begin.tm_mon=ATREFFKEY.mon[0];
    begin.tm_year=ATREFFKEY.year[0];

    end.tm_sec=ATREFFKEY.sec[1];
    end.tm_min=ATREFFKEY.min[1];
    end.tm_hour=ATREFFKEY.hour[1];
    end.tm_mday=ATREFFKEY.day[1];
    end.tm_mon=ATREFFKEY.mon[1];
    end.tm_year=ATREFFKEY.year[1];
    //---------
    //atre->PVS, atmc->SeedPed|RealPed
    if(ATREFFKEY.ReadConstFiles%10==0)end.tm_year=ATREFFKEY.year[0]-1;//StableParams from DB
    //
    TID.add (new AMSTimeID(AMSID("Antispcal2",isRealData()),
			   begin,end,ANTI2C::MAXANTI*sizeof(ANTI2SPcal::antispcal[0]),
			   (void*)&ANTI2SPcal::antispcal[0],server,NeededByDefault));
    //
    end.tm_year=ATREFFKEY.year[1];
    //---------
    if((ATREFFKEY.ReadConstFiles/10)%10==0)end.tm_year=ATREFFKEY.year[0]-1;//VariableParams from DB
    //
    TID.add (new AMSTimeID(AMSID("Antivpcal2",isRealData()),
			   begin,end,ANTI2C::MAXANTI*sizeof(ANTI2VPcal::antivpcal[0]),
			   (void*)&ANTI2VPcal::antivpcal[0],server,NeededByDefault));
    //
    end.tm_year=ATREFFKEY.year[1];
    //---------
    if(ATREFFKEY.ReadConstFiles/100==0 &&
       isRealData())end.tm_year=ATREFFKEY.year[0]-1;//Real data Peds. fromDB
    
    if(ATMCFFKEY.ReadConstFiles%10==0 &&
       !isRealData())end.tm_year=ATREFFKEY.year[0]-1;//MC data RealPeds. fromDB
    //
    TID.add (new AMSTimeID(AMSID("Antipeds",isRealData()),
			   begin,end,ANTI2C::MAXANTI*sizeof(ANTIPeds::anscped[0]),
			   (void*)&ANTIPeds::anscped[0],server,NeededByDefault));
    end.tm_year=ATREFFKEY.year[1];
    //---------
    if(ATMCFFKEY.ReadConstFiles/10==0 &&
       !isRealData())end.tm_year=ATREFFKEY.year[0]-1;//MC data SeedPeds. fromDB
    //
    TID.add (new AMSTimeID(AMSID("AntipedsMS",isRealData()),
			   begin,end,ANTI2C::MAXANTI*sizeof(ANTIPedsMS::anscped[0]),
			   (void*)&ANTIPedsMS::anscped[0],server,NeededByDefault));
    end.tm_year=ATREFFKEY.year[1];
    //---------
    //
  }
  //---------------------------------------

  { //RICH: TDV reservation for calibration parameters of all 
  
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
  
    begin.tm_sec=RICFFKEY.sec[0];
    begin.tm_min=RICFFKEY.min[0];
    begin.tm_hour=RICFFKEY.hour[0];
    begin.tm_mday=RICFFKEY.day[0];
    begin.tm_mon=RICFFKEY.mon[0];
    begin.tm_year=RICFFKEY.year[0];


    end.tm_sec=RICFFKEY.sec[1];
    end.tm_min=RICFFKEY.min[1];
    end.tm_hour=RICFFKEY.hour[1];
    end.tm_mday=RICFFKEY.day[1];
    end.tm_mon=RICFFKEY.mon[1];
    end.tm_year=RICFFKEY.year[1];

    TID.add (new AMSTimeID(AMSID("RichPedCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_ped[0]),
			   (void*)&AMSRICHIdSoft::_ped[0],server,NeededByDefault));

    TID.add (new AMSTimeID(AMSID("RichSpedCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_sig_ped[0]),
			   (void*)&AMSRICHIdSoft::_sig_ped[0],server,NeededByDefault));


    TID.add (new AMSTimeID(AMSID("Richlambdacalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_lambda[0]),
			   (void*)&AMSRICHIdSoft::_lambda[0],server,NeededByDefault));


    TID.add (new AMSTimeID(AMSID("RichScaleCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_scale[0]),
			   (void*)&AMSRICHIdSoft::_scale[0],server,NeededByDefault));

    TID.add (new AMSTimeID(AMSID("RichTransmCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_transparency[0]),
			   (void*)&AMSRICHIdSoft::_transparency[0],server,NeededByDefault));

  
    TID.add (new AMSTimeID(AMSID("RichLDynCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_lambda_dyn[0]),
			   (void*)&AMSRICHIdSoft::_lambda_dyn[0],server,NeededByDefault));
  
    TID.add (new AMSTimeID(AMSID("RichSDynCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_scale_dyn[0]),
			   (void*)&AMSRICHIdSoft::_scale_dyn[0],server,NeededByDefault));

    TID.add (new AMSTimeID(AMSID("RichThresholdCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels*2
			   *sizeof(AMSRICHIdSoft::_threshold[0]),
			   (void*)&AMSRICHIdSoft::_threshold[0],server,NeededByDefault));

    TID.add (new AMSTimeID(AMSID("RichModeBoundaryCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels
			   *sizeof(AMSRICHIdSoft::_gain_mode_boundary[0]),
			   (void*)&AMSRICHIdSoft::_gain_mode_boundary[0],server,NeededByDefault));


    TID.add (new AMSTimeID(AMSID("RichStatusCalib",isRealData()),
			   begin,end,AMSRICHIdSoft::_nchannels
			   *sizeof(AMSRICHIdSoft::_status[0]),
			   (void*)&AMSRICHIdSoft::_status[0],server,NeededByDefault));


  }




  //------------------------------------------
  //
  //   ECAL : TDV-reservation for calibration parameters of all PM's:
  //
  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
 
    begin.tm_sec=ECREFFKEY.sec[0];
    begin.tm_min=ECREFFKEY.min[0];
    begin.tm_hour=ECREFFKEY.hour[0];
    begin.tm_mday=ECREFFKEY.day[0];
    begin.tm_mon=ECREFFKEY.mon[0];
    begin.tm_year=ECREFFKEY.year[0];

    end.tm_sec=ECREFFKEY.sec[1];
    end.tm_min=ECREFFKEY.min[1];
    end.tm_hour=ECREFFKEY.hour[1];
    end.tm_mday=ECREFFKEY.day[1];
    end.tm_mon=ECREFFKEY.mon[1];
    end.tm_year=ECREFFKEY.year[1];
    //--------
    //ecre->DCP; ecmc->CP
    //
    if((ECREFFKEY.ReadConstFiles%100)/10==0)end.tm_year=ECREFFKEY.year[0]-1;//Calib(MC/RD).fromDB

    TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVcalib(),
			   begin,end,ecalconst::ECPMSL*sizeof(ECcalib::ecpmcal[0][0]),
			   (void*)&ECcalib::ecpmcal[0][0],server));
    end.tm_year=ECREFFKEY.year[1];
    //--------				  
    if(ECREFFKEY.ReadConstFiles/100==0)end.tm_year=ECREFFKEY.year[0]-1;//DataCardThresh/Cuts fromDB

    TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVvpar(),
			   begin,end,sizeof(ECALVarp::ecalvpar),
			   (void*)&ECALVarp::ecalvpar,server));
    end.tm_year=ECREFFKEY.year[1];
    //--------
    if(!isRealData()){//"MC.Seeds" TDV only for MC-run.    
      if((ECMCFFKEY.ReadConstFiles%100)/10==0)end.tm_year=ECREFFKEY.year[0]-1;//Calib"MC.Seeds" fromDB
	     
      TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVcalibMS(),
			     begin,end,ecalconst::ECPMSL*sizeof(ECcalibMS::ecpmcal[0][0]),
			     (void*)&ECcalibMS::ecpmcal[0][0],server));
      end.tm_year=ECREFFKEY.year[1];
    }
    //--------
    if(ECREFFKEY.ReadConstFiles%10==0 &&
       isRealData())end.tm_year=ECREFFKEY.year[0]-1;//Real data Peds.fromDB
    if(ECMCFFKEY.ReadConstFiles%10==0 &&
       !isRealData())end.tm_year=ECREFFKEY.year[0]-1;//MC Peds fromDB
	     
    TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVped(),
			   begin,end,ecalconst::ECPMSL*sizeof(ECPMPeds::pmpeds[0][0]),
			   (void*)&ECPMPeds::pmpeds[0][0],server));
    end.tm_year=ECREFFKEY.year[1];
    //--------
  }
  //
  //---------------------------------------
  //
  // Data to fit particle charge magnitude(now for tracker only)
  //
  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec=CHARGEFITFFKEY.sec[0];
    begin.tm_min=CHARGEFITFFKEY.min[0];
    begin.tm_hour=CHARGEFITFFKEY.hour[0];
    begin.tm_mday=CHARGEFITFFKEY.day[0];
    begin.tm_mon=CHARGEFITFFKEY.mon[0];
    begin.tm_year=CHARGEFITFFKEY.year[0];

    end.tm_sec=CHARGEFITFFKEY.sec[1];
    end.tm_min=CHARGEFITFFKEY.min[1];
    end.tm_hour=CHARGEFITFFKEY.hour[1];
    end.tm_mday=CHARGEFITFFKEY.day[1];
    end.tm_mon=CHARGEFITFFKEY.mon[1];
    end.tm_year=CHARGEFITFFKEY.year[1];

    if(CHARGEFITFFKEY.TrkPDFileRead==0)end.tm_year=CHARGEFITFFKEY.year[0]-1;//ChargeCalibPDFs from DB

    TID.add (new AMSTimeID(AMSID("Trkcpdfs",isRealData()),
			   begin,end,sizeof(TrkElosPDF::TrkEPDFs),
			   (void*)&TrkElosPDF::TrkEPDFs[0],server,NeededByDefault));
    
    TID.add (new AMSTimeID(AMSID("ChargeIndxTrk",isRealData()),
			   begin,end,MaxZTypes*sizeof(AMSCharge::_chargeTracker[0]),
			   (void*)AMSCharge::_chargeTracker,server,NeededByDefault));
    
    end.tm_year=CHARGEFITFFKEY.year[1];




  }
  //---------------------------
  {
    // TOF Temperature data

    tm begin=AMSmceventg::Orbit.End;
    tm end=AMSmceventg::Orbit.Begin;
   
    TID.add (new AMSTimeID(AMSID("TOF2Temperature",isRealData()),
			   begin,end,
			   TOF2Varp::tofvpar.gettoftsize(),(void*)TOF2Varp::tofvpar.gettoftp(),server));
  }
  //-----------------------------
  {
    // Magnet Temperature data

    tm begin=AMSmceventg::Orbit.End;
    tm end=AMSmceventg::Orbit.Begin;
    TID.add (new AMSTimeID(AMSID("MagnetTemperature",isRealData()),
			   begin,end,
			   MagnetVarp::getmagnettsize(),(void*)MagnetVarp::getmagnettp(),server));
   
  }
  //-----------------------------
  {
    // Scaler Data

    tm begin=AMSmceventg::Orbit.End;
    tm end=AMSmceventg::Orbit.Begin;
    TID.add (new AMSTimeID(AMSID("ScalerN",isRealData()),
			   begin,end,
			   Trigger2LVL1::getscalerssize(),(void*)Trigger2LVL1::getscalersp(),server));
   
  }
  //-----------------------------
  {
    tm begin;
    tm end;
    if(AMSFFKEY.Update==87){
      begin=AMSmceventg::Orbit.Begin;
      end=AMSmceventg::Orbit.End;
      AMSEvent::SetShuttlePar();
    }
    else{
      begin=AMSmceventg::Orbit.End;
      end=AMSmceventg::Orbit.Begin;
    }
    TID.add (new AMSTimeID(AMSID("ShuttlePar",isRealData()),
			   begin,end,
			   sizeof(AMSEvent::Array),(void*)AMSEvent::Array,server));
  }


  if(MISCFFKEY.BeamTest){
    tm begin=AMSmceventg::Orbit.End;
    tm end=AMSmceventg::Orbit.Begin;
    TID.add (new AMSTimeID(AMSID("BeamPar",isRealData()),
			   begin,end,
			   sizeof(AMSEvent::ArrayB),(void*)AMSEvent::ArrayB,server));
  }




  {
    tm begin;
    tm end;
    if(AMSFFKEY.Update==88){
      begin=AMSmceventg::Orbit.Begin;
      end=AMSmceventg::Orbit.End;
    }
    else{
      begin=AMSmceventg::Orbit.End;
      end=AMSmceventg::Orbit.Begin;
    }
    AMSTimeID * ptdv= (AMSTimeID*) TID.add(new AMSTimeID(AMSID(getstatustable()->getname(),
							       isRealData()),begin,end,getstatustable()->getsize(),
							 getstatustable()->getptr(),server));
  }

  {
    tm begin;
    tm end;
    if(AMSFFKEY.Update==89){
      begin=AMSmceventg::Orbit.Begin;
      end=AMSmceventg::Orbit.End;
      if(TRALIG.InitDB)AMSTrAligPar::InitDB();
    }
    else{
      begin=AMSmceventg::Orbit.End;
      end=AMSmceventg::Orbit.Begin;
    }
    AMSTimeID * ptdv= (AMSTimeID*) TID.add(new AMSTimeID(AMSID("TrAligDB",
							       isRealData()),begin,end,AMSTrAligPar::gettraligdbsize(),
							 AMSTrAligPar::gettraligdbp(),server));
  }



  {
    tm begin;
    tm end;
    AMSTrAligFit::InitDB();
    if(AMSFFKEY.Update==90){
      begin=AMSmceventg::Orbit.Begin;
      end=AMSmceventg::Orbit.End;
    }
    else{
      begin=AMSmceventg::Orbit.End;
      end=AMSmceventg::Orbit.Begin;
    }
    /*
      AMSTimeID * ptdv= (AMSTimeID*) TID.add(new AMSTimeID(AMSID("TrAligglDB",
      isRealData()),begin,end,AMSTrAligFit::gettraliggldbsize(),

      AMSTrAligFit::gettraliggldbp(),server));
    */
  }



  if (AMSFFKEY.Update){

#ifdef __DB__
    if (AMSFFKEY.Write == 0 ) {
#endif
      // Here update dbase

      AMSTimeID * offspring=(AMSTimeID*)TID.down();
      while(offspring){
	for(int i=0;i<AMSJob::gethead()->gettdvn();i++){     
	  offspring->checkupdate(AMSJob::gethead()->gettdvc(i));
	}
	if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
	  cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
	offspring=(AMSTimeID*)offspring->next();
      }
#ifdef __DB__
    } 
#endif  
  }
}

//===============================================================================

AMSTimeID * AMSJob::gettimestructure(){
  AMSID id("TDV:",0);    
  AMSNode *p=JobMap.getp(id);
  if(!p){
    cerr << "AMSJob::gettimestructure-F-no time structure found"<<endl;
    exit(1);
    return 0;
  }
  else return  dynamic_cast<AMSTimeID*>(p);
}
AMSNode * AMSJob::getaligstructure(){
  AMSTrAligFit a;
  AMSNode *p=JobMap.getp(AMSID(a.getname(),a.getid()));
  if(!p){
    cerr << "AMSJob::getaligtructure-F-no alig structure found"<<endl;
    exit(1);
    return 0;
  }
  else return  p;
}

AMSTimeID * AMSJob::gettimestructure(const AMSID & id){
  AMSNode *p=JobMap.getp(id);
  if(!p){
    cerr << "AMSJob::gettimestructure-F-no time structure found "<<id<<endl;
    exit(1);
    return 0;
  }
  else return  dynamic_cast<AMSTimeID*>(p);
}

AMSJob::AMSJob(AMSID id, uinteger jobtype):AMSNode(id),_jobtype(jobtype),_pAMSG4Physics(0),_pAMSG4GeneratorInterface(0),_NtupleActive(false)
{
  _Setup[0]='\0';_TriggerC[0][0]='\0';_TriggerI=1;_TriggerN=0;
  _TDVC[0][0]='\0';
  _TDVN=0;
  if(_Head){
    cerr<<"AMSJob-ctor-F-OnlyOnejobAlowed"<<endl;
    exit(1);
  }
  else _Head=this;
  cout <<"   AMS Software version "<<AMSCommonsI::getversion()<<"/"<<AMSCommonsI::getbuildno()<<"/"<<AMSCommonsI::getosno()<<endl;
}

AMSJob::~AMSJob(){
  cout << "   ~AMSJob called "<<endl;
  HPRINT(9797+1);
  _signendjob();
  if(isSimulation())uhend(CCFFKEY.run,GCFLAG.IEVENT,CCFFKEY.curtime);
  else uhend();
  cout <<"   uhend finished"<<endl;
  _tkendjob();
  cout <<"   tkendjob finished"<<endl;
  _tof2endjob();
  cout <<"   tof2endjob finished"<<endl;
  _anti2endjob();
  cout <<"   anti2endjob finished"<<endl;
  _ecalendjob();
  cout <<"   ecalendjob finished"<<endl;
  _trdendjob();
  cout <<"   trdendjob finished"<<endl;
  _srdendjob();
  cout <<"   srdendjob finished"<<endl;
  _richendjob();
  cout <<"   richendjob finished"<<endl;

  _dbendjob();
  cout <<"   dbendjob finished"<<endl;
  _axendjob();
  cout <<"   axendjob finished"<<endl;
#ifndef __TFADBW__  
  TGL1JobStat::printstat();
  TriggerLVL302::printfc();
#endif
 
#ifdef __CORBA__
  if(isSimulation())AMSProducer::gethead()->sendRunEndMC();
#endif
  delete _pntuple;
  cout <<"pntuple deleted"<<endl;
#ifdef __CORBA__
  delete AMSProducer::gethead();
  cout <<"producer deleted"<<endl;
#endif
#ifdef __G4AMS__
  if(MISCFFKEY.G4On){
    // due to infinite geant4 destructors
#ifndef G4VIS_USE
    delete &(AMSgObj::BookTimer);
#endif
    abort();
  }
#endif
}
#include "producer.h"
void AMSJob::uhend(uinteger r, uinteger e, time_t t){
  if(IOPA.hlun && _NtupleActive){
    _NtupleActive=false;
    int ntuple_entries;
    HNOENT(IOPA.ntuple, ntuple_entries);
    cout << " Closing AMS ntuple at unit " << IOPA.hlun << " with ";
    cout << ntuple_entries << " events" << endl << endl;
    char hpawc[256]="//PAWC";
    HCDIR (hpawc, " ");
    char houtput[9]="//output";
    HCDIR (houtput, " ");
    integer ICYCL=0;
    HROUT (0, ICYCL, " ");
    HREND ("output");
    CLOSEF(IOPA.hlun);
#ifdef __CORBA__
    if(AMSEvent::gethead())AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::Ntuple,ntuple_entries,AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime(),true);
    else AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::Ntuple,ntuple_entries,e,t,true);
#endif
  }
  else if(IOPA.WriteRoot  ){
    if(_pntuple)_pntuple->endR();
#ifdef __CORBA__
    if(_pntuple){
      int root_entries=_pntuple->getentries();
      if(AMSEvent::gethead())AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RootFile,root_entries,AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime(),true);
      else AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RootFile,root_entries,e,t,true);
    }
#endif
  }
}

void AMSJob::urinit(integer run, integer eventno, time_t tt)
  throw (amsglobalerror){
  if(_pntuple)_pntuple->endR();
  if(IOPA.WriteRoot){
    //   trail leading blancs if any
    //  
    int offset=-1; 
    for(int i=0;i<strlen((const char*)_rextname);i++){
      if(*((const char*)_rextname+i)!=' ')break;
      else offset=i;
    } 
    strcpy(_rootfilename,(const char*)_rextname+offset+1);
    AString mdir("mkdir -p ");
    mdir+=_rootfilename;
    integer iostat;
    integer rsize=8000;
    if(eventno){
      char event[80];  
      if(isProduction()){
	system((const char*)mdir);
	strcat(_rootfilename,"/");
      }
      sprintf(event,"%d",run);
      strcat(_rootfilename,event);
      sprintf(event,".%08d",eventno);
      strcat(_rootfilename,event);
      strcat(_rootfilename,".root");
    }
    if(_pntuple)_pntuple->initR(_rootfilename);
    else{
      _pntuple = new AMSNtuple(_rootfilename);
      _pntuple->initR(_rootfilename);
    }
#ifdef __CORBA__
    AMSProducer::gethead()->sendNtupleStart(DPS::Producer::RootFile,_rootfilename,run,eventno,tt);
#endif
  }
}


void AMSJob::uhinit(integer run, integer eventno, time_t tt) 
  throw (amsglobalerror){
  if(_NtupleActive)uhend();
  if(IOPA.hlun ){
   
    strcpy(_ntuplefilename,(const char *)_hextname);
    AString mdir("mkdir -p ");
    mdir+=_ntuplefilename;
    integer iostat;
    integer rsize=8000;
    if(eventno){
      char event[80];  
      if(isProduction()){
	system((const char*)mdir);
	strcat(_ntuplefilename,"/");
      }
      sprintf(event,"%d",run);
      strcat(_ntuplefilename,event);
      sprintf(event,".%d",eventno);
      strcat(_ntuplefilename,event);
      strcat(_ntuplefilename,".hbk");
    }
    
    cout <<"Trying to open histo file "<<_ntuplefilename<<endl;
    npq_(); 
    HROPEN(IOPA.hlun,"output",_ntuplefilename,"NPQ",rsize,iostat);
    if(iostat){
      cerr << "Error opening Histo file "<<_ntuplefilename<<endl;
      throw amsglobalerror("UnableToOpenHistoFile",3);
    }
    else cout <<"Histo file "<<_ntuplefilename<<" opened."<<endl;

    // Open the n-tuple
    if(_pntuple)_pntuple->init();
    else{
      _pntuple = new AMSNtuple(IOPA.ntuple,"AMS Ntuple");
    }
    _NtupleActive=true;
#ifdef __CORBA__
    AMSProducer::gethead()->sendNtupleStart(DPS::Producer::Ntuple,_ntuplefilename,run,eventno,tt);
#endif
  }
  urinit(run,eventno,tt);
  HBOOK1(200101,"Number of Nois Hits x",100,-0.5,99.5,0.);
  HBOOK1(200102,"Number of Nois Hits y",100,-0.5,99.5,0.);
  HBOOK1(200105,"Above threshold spectrum x",200,-0.5,49.5,0.);
  HBOOK1(200106,"Above threshold spectrum y",200,-0.5,49.5,0.);
  HBOOK1(200107," adc over",3000,29999.5,32999.5,0.);
}

void AMSJob::_tkendjob(){
  if(isCalibration() & CTracker){
    if(TRALIG.UpdateDB){
      if(!TRALIG.GlobalFit)AMSTrAligFit::Test(1);
      else AMSTrAligFit::Testgl(2);
    }
  }
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 1){
    AMSTrIdCalib::check(1);
    AMSTrIdCalib::printbadchanlist();
  }
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 2){
    int i,j;
    for(i=0;i<nalg;i++){
      if(TRCALIB.Method==0 || TRCALIB.Method==i){
	for(j=TRCALIB.PatStart;j<tkcalpat;j++){
	  if(AMSTrCalibFit::getHead(i,j)->Test(1)){
	    AMSTrCalibFit::getHead(i,j)->Fit();
	  }
	}
      }
    }

    for(i=0;i<nalg;i++){
      if(TRCALIB.Method==0 || TRCALIB.Method==i){
	for(j=TRCALIB.PatStart;j<tkcalpat;j++){
	  AMSTrCalibFit::getHead(i,j)->Anal();
	}
      }
    }
    AMSTrCalibFit::SAnal();

  }
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 3){
    AMSTrIdCalib::ntuple(AMSEvent::getSRun());
  }
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 4){
    AMSTrIdCalib::getaverage();
  }
  if(isMonitoring() & (AMSJob::MTracker | AMSJob::MAll)){
    AMSTrIdCalib::offmonhist();    
  }



}

void AMSJob::_signendjob(){
  AMSmceventg::endjob();
}

//
//-------------------------------------------------------------------
void AMSJob::_tof2endjob(){
  if(isSimulation())TOF2JobStat::outpmc();
  TOF2JobStat::outp();
  TOF2JobStat::printstat(); // Print JOB-TOF statistics
  if((isCalibration() & CTOF) && (TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6)){
    TOFPedCalib::outp(TFCAFFKEY.pedoutf);// 0/1/2->HistOnly/Wr2DB+File/Wr2File 
  }
  if((isCalibration() & CTOF) && TFREFFKEY.relogic[0]==7){
    TOFPedCalib::ntuple_close();// close OnBoardPedsTable ntuple 
  }
}
//-----------------------------------------------------------------------
void AMSJob::_anti2endjob(){
  ANTI2JobStat::printstat();
  if(isSimulation() && ATMCFFKEY.mcprtf>0)ANTI2JobStat::outpmc();
  ANTI2JobStat::outp();//this routine keep AmplCalib staff inside !
  if((isCalibration() & CAnti) && (ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3)){
    ANTPedCalib::outp(ATCAFFKEY.pedoutf);// 0/1/2->HistOnly/Wr2DB+File/Wr2File 
  }
  if((isCalibration() & CAnti) && ATREFFKEY.relogic==4){
    ANTPedCalib::ntuple_close();// close ntuple for OnBoardPedTables 
  }
}
//-----------------------------------------------------------------------
void AMSJob::_ecalendjob(){
  //

  if((isCalibration() & AMSJob::CEcal) && ECREFFKEY.relogic[1]==6 ){
    //    AMSECIdCalib::getaverage();
    //    AMSECIdCalib::write();
    ECPedCalib::ntuple_close();
  }
  if((isCalibration() & AMSJob::CEcal) && (ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5) ){
    ECPedCalib::outp(ECCAFFKEY.pedoutf);// 0/1/2->HistOnly/Writ2DB+File/Write2File 
  }
#ifndef __TFADBW__
  EcalJobStat::printstat(); // Print JOB-Ecal statistics
  if(isSimulation())EcalJobStat::outpmc();
  EcalJobStat::outp();
#endif
}
//-----------------------------------------------------------------------
void AMSJob::_trdendjob(){

}

void AMSJob::_srdendjob(){

}

void AMSJob::_richendjob(){
  // print some statistics
  // perform calibration
  RichRadiatorTileManager::Finish();
}


void AMSJob::_axendjob(){

  if(AMSJob::debug){
    // AMSJob::gethead()->printN(cout);
    AMSNode* p=AMSJob::gethead()->getmat();
    assert(p!=NULL);
    p=AMSJob::gethead()->getmed();
    assert(p!=NULL);
    AMSgvolume * pg=AMSJob::gethead()->getgeom();
    assert(pg!=NULL);
  }

  if(isProduction() && isRealData()){
    AMSEvent::_endofrun();
  }
  AMSUser::EndJob(); 
}

void AMSJob::_dbendjob(){


  //Status Stuff
#ifndef __CORBA__
  if( AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
    AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::gethead()->getTDVStatus());
    ptdv->UpdateMe()=1;
    ptdv->UpdCRC();
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
  }
#endif
  if( AMSFFKEY.Update && AMSStatus::isDBUpdateR()   ){
    AMSStatus::UpdateStatusTableDB();
  }


#ifdef __DB__
  if (AMSEvent::_checkUpdate() == 1) {
    Message("====> AMSJob::_dbendjob -I- UpdateMe is set. Update database and tables.");
    int rstatus = lms -> AddAllTDV();
  } else {
    Message("====> AMSJob::_dbendjob -I- UpdateMe != 1. NO UPDATE");
  }
#else
  //    if (AMSFFKEY.Update && !isCalibration()){
  if (AMSFFKEY.Update){
    AMSTimeID * offspring = 
      (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
    while(offspring){
      if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
	cerr <<"<---- AMSJob::_dbendjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
    }
  }
#endif
}



//=================================================================================================
void AMSJob::_redaq2initjob(){
  AMSgObj::BookTimer.book("SIDAQ");
  AMSgObj::BookTimer.book("REDAQ");
  if(IOPA.Portion<1. && isMonitoring())cout <<"AMSJob::_redaq2initjob()-W-Only about "<<IOPA.Portion*100<<"% events will be processed."<<endl; 
  // Add subdetectors to daq
  //
     
  // ecal
  if((isCalibration() & CEcal) && ECREFFKEY.relogic[1]==0){
    DAQEvent::addsubdetector(&AMSEcalRawEvent::checkdaqid,&AMSECIdCalib::buildSigmaPed);
  }
  else if((isCalibration() & CEcal) && ECREFFKEY.relogic[1]==-1){
    DAQEvent::addsubdetector(&AMSEcalRawEvent::checkdaqid,&AMSECIdCalib::buildPedDiff);
  }
  else{
    //    DAQEvent::addsubdetector(&AMSEcalRawEvent::checkdaqid,&AMSEcalRawEvent::buildrawRaw);
    DAQEvent::addsubdetector(&DAQECBlock::checkblockid,&DAQECBlock::buildraw);// for RD
    DAQEvent::addblocktype(&DAQECBlock::getmaxblocks,&DAQECBlock::calcblocklength,
                           &DAQECBlock::buildblock);//for MC ?
  }

  {  // mc
    if(!isRealData()){
      DAQEvent::addsubdetector(&AMSmceventg::checkdaqid,&AMSmceventg::buildraw);
      DAQEvent::addblocktype(&AMSmceventg::getmaxblocks,&AMSmceventg::calcdaqlength,
			     &AMSmceventg::builddaq);

      DAQEvent::addsubdetector(&AMSTrMCCluster::checkdaqid,&AMSTrMCCluster::buildraw);
      DAQEvent::addblocktype(&AMSTrMCCluster::getmaxblocks,&AMSTrMCCluster::calcdaqlength,
			     &AMSTrMCCluster::builddaq);



      DAQEvent::addsubdetector(&AMSEvent::checkdaqidSh,&AMSEvent::buildrawSh);
      DAQEvent::addblocktype(&AMSEvent::getmaxblocksSh,
			     &AMSEvent::calcdaqlengthSh,&AMSEvent::builddaqSh);
    }

  }


  {
    // lvl1

    DAQEvent::addsubdetector(&Trigger2LVL1::checkdaqid,&Trigger2LVL1::buildraw);
    DAQEvent::addblocktype(&Trigger2LVL1::getmaxblocks,&Trigger2LVL1::calcdaqlength,
			   &Trigger2LVL1::builddaq);


  }

  {
    //           Header
    DAQEvent::addblocktype(&AMSEvent::getmaxblocks,&AMSEvent::calcdaqlength,
			   &AMSEvent::builddaq);
    //           Header Tracker HK
    DAQEvent::addblocktype(&AMSEvent::getmaxblocks,&AMSEvent::calcTrackerHKl,
			   &AMSEvent::buildTrackerHKdaq,4);
  }


  {
    //           lvl3
    DAQEvent::addsubdetector(&TriggerLVL302::checkdaqid,&TriggerLVL302::buildraw);
    DAQEvent::addblocktype(&TriggerLVL302::getmaxblocks,&TriggerLVL302::calcdaqlength,
			   &TriggerLVL302::builddaq);

  }    
  if(DAQCFFKEY.SCrateinDAQ){
    //         tof+anti+???
    DAQEvent::addsubdetector(&DAQS2Block::checkblockid,&DAQS2Block::buildraw);
    DAQEvent::addblocktype(&DAQS2Block::getmaxblocks,&DAQS2Block::calcblocklength,
                           &DAQS2Block::buildblock);
  }    

  if((AMSJob::gethead()->isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 1){
    {

      if(DAQCFFKEY.OldFormat || !isRealData()){
	// special tracker ped/sigma calc
	if(TRCALIB.Method == 1)
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrIdCalib::buildSigmaPed);
	else
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,
				   &AMSTrIdCalib::buildSigmaPedA);
      }
      else{
	DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrIdCalib::buildSigmaPedB);
	DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidMixed,&AMSTrIdCalib::buildSigmaPedB);
 
	if(TRMCFFKEY.GenerateConst){
	  //Tracker ped/sigma etc ( "Event" mode)

	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSRawid,&AMSTrRawCluster::updpedSRaw);
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkcmnSRawid,&AMSTrRawCluster::updcmnSRaw);
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checksigSRawid,&AMSTrRawCluster::updsigSRaw);
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSRawid,&AMSTrRawCluster::updstatusSRaw);
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidParameters,&AMSTrRawCluster::buildrawParameters);


	}
      }

    }
  }
  else {

    if(DAQCFFKEY.LCrateinDAQ){
      //           tracker reduced

      DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidMixed,&AMSTrRawCluster::buildrawMixed);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkcmnSRawid,&AMSTrRawCluster::updcmnSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidCompressed,&AMSTrRawCluster::buildrawCompressed);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqid,&AMSTrRawCluster::buildraw);
      DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblocks,
			     &AMSTrRawCluster::calcdaqlength,&AMSTrRawCluster::builddaq);



      //Tracker ped/sigma etc ( "Event" mode)

      DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSRawid,&AMSTrRawCluster::updpedSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checksigSRawid,&AMSTrRawCluster::updsigSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSRawid,&AMSTrRawCluster::updstatusSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidParameters,&AMSTrRawCluster::buildrawParameters);


    }   


    if(DAQCFFKEY.LCrateinDAQ ){
      //           tracker raw

      if(DAQCFFKEY.OldFormat || !isRealData()){
	if(TRCALIB.Method == 1)
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrRawCluster::buildrawRaw);
	else
	  DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrRawCluster::buildrawRawA);
      }
      else{
	DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrRawCluster::buildrawRawB);
      }

      DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblocksRaw,
			     &AMSTrRawCluster::calcdaqlengthRaw,&AMSTrRawCluster::builddaqRaw);



    }    


    {
      //           tracker H/K Static

      DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSid,&AMSTrRawCluster::updstatusS,4);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSid,&AMSTrRawCluster::updpedS,4);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checksigmaSid,&AMSTrRawCluster::updsigmaS,4);
      DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblockS,
			     &AMSTrRawCluster::calcstatusSl,&AMSTrRawCluster::writestatusS,4);
      DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblockS,
			     &AMSTrRawCluster::calcpedSl,&AMSTrRawCluster::writepedS,4);
      DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblockS,
			     &AMSTrRawCluster::calcsigmaSl,&AMSTrRawCluster::writesigmaS,4);


    }    

  }
}
//==========================================================================================
#ifdef __DB__

integer AMSJob::FindTheBestTDV(char* name, integer id, time_t timeV, 
			       integer &S, time_t &I, time_t &B, time_t &E)
//
//  (1) find TDV by name in tdvNameTab
//  (2) find timeB =< timeV =< timeE
//  for all TDV's which satisfies (2) choose one with the latest 
//  insert time
//
// name   - object name
// id     - object id
// time   - object time
// S      - object size
// I,B,E  - insert, begin, end time the most satisfies to 'time'
//
// rstatus = -1  not found
// rstatus =  1  found
{
  integer rstatus = -1;
#ifdef __DB__
  //geant t1, t2;
  //TIMEX(t1);
  for (int i=0; i<ntdvNames; i++) {
    if(strcmp(name,tdvNameTab[i]) == 0) {
      if(_ptr_start[i] > -1 && _ptr_end[i] > -1) {
	integer  ptr = -1;
	time_t   insert = 0;
	for (int j=_ptr_start[i]; j<_ptr_end[i]; j++) {
	  if(_tdv[j]._id == id) {
	    if (timeV >= _tdv[j]._begin && timeV <= _tdv[j]._end) {
	      if (_ptr_end[i] - _ptr_start[i] - 1 != 0) {
		if (insert == 0) {
		  insert = _tdv[j]._insert;
		  ptr    = j;
		} else {
		  if (insert < _tdv[j]._insert) {
		    insert = _tdv[j]._insert;
		    ptr    = j;
		  }
		}
	      } else {
		ptr = j;
	      }
	    }
	  } // compare id's
	}
	if (ptr > -1) {
	  I = _tdv[ptr]._insert;
	  B = _tdv[ptr]._begin;
	  E = _tdv[ptr]._end;
	  S = _tdv[ptr]._size;
#ifdef __AMSDEBUG__
	  //TIMEX(t2);
	  //cout <<t2-t1<<" sec."<<endl;
	  //cout <<"found TDV for "<<tdvNameTab[i]<<", size "<<S<<endl;
	  //cout <<"Time of Event "<<asctime(localtime(&timeV))<<endl;
	  //cout <<"i/b/e "<<asctime(localtime(&_tdv[ptr]._insert))
	  //<<"      "<<asctime(localtime(&_tdv[ptr]._begin))
	  //<<"      "<<asctime(localtime(&_tdv[ptr]._end))<<endl;
#endif
	  rstatus = 1;
	} else {
	  cerr <<"AMSJob::FindTheBestTDV -W- cannot find TDV for "<<name<<endl;
	  cerr <<"time "<<asctime(localtime(&timeV))<<endl;
	}
	break;
      } else {
	cout<<"AMSJob::FindTheBestTDV -W- no TDV for "<<name<<endl;
	rstatus = -1;
      }
    }
  }
#endif
  return rstatus;
}

#endif

#ifdef __DB__

integer AMSJob::FillJobTDV(integer nobj, tdv_time *tdv)
//
// Fill TDV table by values read from database
//
{
  integer rstatus = -1;

  
  if (nobj < 1) {
    cerr <<"AMSJob::FillJobTDV -E- value out of range "<<nobj<<endl;
    return rstatus;
  }
  _tdv = new tdv_time[nobj];
  for (int i=0; i<nobj; i++) { _tdv[i] = tdv[i];}
 
  return 1;
}
#endif


#ifdef __DB__

integer AMSJob::SetTDVPtrs(integer start[], integer end[])
//
// Set start/end ptrs for TDV table 
//
{
  for (int i=0; i<ntdvNames; i++) {
    _ptr_start[i] = start[i];
    _ptr_end[i]   = end[i];
  }
  return 1;
}
integer AMSJob::FillTDVTable(){

  AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
  AMSTimeID * offspring=(AMSTimeID*)ptid->down();
  integer nobj = 0;
  while(offspring){
    tdvNameTab[nobj] = offspring -> getname();
    tdvIdTab[nobj]   = offspring -> getid();
    nobj++;
    offspring=(AMSTimeID*)offspring->next();
  }
  if (_ptr_start) delete [] _ptr_start;
  if (_ptr_end)   delete [] _ptr_end;
  _ptr_start = new integer[nobj];
  _ptr_end   = new integer[nobj];
  ntdvNames  = nobj;
  return nobj;
}

#endif
