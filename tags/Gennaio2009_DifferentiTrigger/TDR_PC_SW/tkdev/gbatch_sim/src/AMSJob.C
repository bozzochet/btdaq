#include "TrField.h"
#include "TrRecon.h"
#include "TrCalDB.h"
#include "AMSJob.h"
#include "tkdcards.h"
#include "amsgobj.h"
#include "amscommonsi.h"
#include "TkDBc.h"
#include "amsdbc.h"
#include "commons.h"
#include "cern.h"
#include "geant321g.h"
#include "AMSEvent.h"
#include "VCon_gbatch.h"


AMSContainer * GetCEvent(AMSID id);
AMSContainer * GetCEvent(AMSID id){
	return
   AMSEvent::gethead()->getC(id);
	
}
//======================================================================
//BIT FIELDS
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
//======================================================================

AMSJob*    AMSJob::_Head=0;
AMSNodeMap AMSJob::JobMap;




AMSJob::AMSJob(AMSID id, uinteger jobtype):
  AMSNode(id),_jobtype(jobtype){
  if(_Head){
    cerr<<"AMSJob-ctor-F-OnlyOnejobAlowed"<<endl;
    exit(1);
  }
  else _Head=this;
  cout <<"   AMS Software version "<<AMSCommonsI::getversion()<<"/"
       <<AMSCommonsI::getbuildno()<<"/"<<AMSCommonsI::getosno()<<endl;
}

AMSJob::~AMSJob(){
  cout << "   ~AMSJob called "<<endl;

  //  _signendjob();
  _tkendjob();
  cout <<"   tkendjob finished"<<endl;

}

void AMSJob::_sitkdata(){


  TKGEOMFFKEY.init();
  TRMCFFKEY.init();
  TRCALIB.init();
  TRALIG.init();
  
//   FFKEY("TKGE",(float*)&TKGEOMFFKEY,sizeof(TKGEOMFFKEY_DEF)/sizeof(integer),
// 	"MIXED");
  
//   FFKEY("TRMC",(float*)&TRMCFFKEY,sizeof(TRMCFFKEY_DEF)/sizeof(integer),"MIXED");

//   FFKEY("TRCALIB",(float*)&TRCALIB,sizeof(TRCALIB_DEF)/sizeof(integer),"MIXED");
 
//   FFKEY("TRALIG",(float*)&TRALIG,sizeof(TRALIG_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_retkdata(){

  TRCLFFKEY.init();

  //  FFKEY("TRCL",(float*)&TRCLFFKEY,sizeof(TRCLFFKEY_DEF)/sizeof(integer),"MIXED");

  TRFITFFKEY.init();

  //FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
  //initialization of the fortran fit routines (still needed ?)
  // TKFINI();
}
void AMSJob::_tkendjob(){}

void AMSJob::_sitkinitjob(){

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


void AMSJob::_retkinitjob(){
  AMSgObj::BookTimer.book("RETKEVENT");
  AMSgObj::BookTimer.book("TrCluster");
  AMSgObj::BookTimer.book("TrClusterRefit");
  AMSgObj::BookTimer.book("TrRecHit");
  AMSgObj::BookTimer.book("TrRecHit_build");
  AMSgObj::BookTimer.book("TrRecHit_add");
  AMSgObj::BookTimer.book("TrTrack");
  AMSgObj::BookTimer.book("TrFalseX");
//   if(CHARGEFITFFKEY.TrkPDFileRead%10>0){//(Q) Take ChargeCalibPDFs(mc|rd) from raw files
//     //
//     TrkElosPDF::build();//create TrkElosPDF-objects from TRK raw ChargeCalibFile
//   }

}

//=========================================================
//   DATA PART
//=========================================================


void AMSJob::data(){

  GCSTAK.NJTMIN=1;
  GCTRAK.upwght=0;
  GCTIME.TIMINT=10000000;
  GCTIME.TIMEND=10;
  GCTIME.ITIME=0;
 SELECTFFKEY.Run=0;
  SELECTFFKEY.Event=0;
  SELECTFFKEY.RunE=0;
  SELECTFFKEY.EventE=0;
  VBLANK(SELECTFFKEY.File,40);
  char * SELECT="SELECT";
  char * MIXED="MIXED";
  FFKEY(SELECT,(float*)&SELECTFFKEY,sizeof(SELECTFFKEY_DEF)/sizeof(integer),MIXED);

  { 
    for(int i=0;i<32;i++){
      STATUSFFKEY.status[i]=0;
    }
        STATUSFFKEY.status[33]=10;
  }
  FFKEY("ESTATUS",(float*)&STATUSFFKEY,sizeof(STATUSFFKEY_DEF)/sizeof(integer),"MIXED");

  AMSFFKEY.Simulation=0; // Simulation
  AMSFFKEY.Reconstruction=1; // Reconstruction
  AMSFFKEY.Jobtype=0; // Default
  AMSFFKEY.Debug=1;
  AMSFFKEY.CpuLimit=15;
  AMSFFKEY.Read=0;
  AMSFFKEY.Write=1;
  AMSFFKEY.Update=0;
  VBLANK(AMSFFKEY.Jobname,40);
  VBLANK(AMSFFKEY.Setupname,40);
  char amsetup[16]="AMS02";
  UCTOH(amsetup,AMSFFKEY.Setupname,4,16);
  AMSFFKEY.ZeroSetupOk=0;
  // Set Defaults
FFKEY("AMSJOB",(float*)&AMSFFKEY+2,sizeof(AMSFFKEY_DEF)/sizeof(integer),"MIXED");
 MAGFIELDFFKEY.init();
  FFKEY("BMAP",(float*)&MAGFIELDFFKEY,sizeof(MAGFIELDFFKEY_DEF)/sizeof(integer),"MIXED");

//  MAGSFFKEY.init();
// FFKEY("MAGS",(float*)&MAGSFFKEY,sizeof(MAGSFFKEY_DEF)/sizeof(integer),"MIXED");
  _siamsdata();
  _reamsdata();
}




void AMSJob::_siamsdata(){

  IOPA.init();
  FFKEY("IOPA",(float*)&IOPA,sizeof(IOPA_DEF)/sizeof(integer),"MIXED");
  _signdata();
  _sitkdata();
}


void AMSJob::_reamsdata(){

  _retkdata();
}

void AMSJob::_siamsinitjob(){
  AMSgObj::BookTimer.book("SIAMSEVENT");
  _sitkinitjob();


}
//=========================================================
//   UDATA PART
//=========================================================
void AMSJob::udata(){
 GCTIME.TIMEND=GCTIME.TIMINT;
  GCTIME.ITIME=0;
  AMSDBc::init(0);

 TkDBc::CreateTkDBc();
 TkDBc::Head->init();
 char calibname[300];
 sprintf(calibname,"CalDB.root");

 TrCalDB* cc= new TrCalDB();
 cc->init();
 cc->CreateLinear();
 // TrCalDB::Load(calibname);

 TrRecon::Create();
 TrRecon::UsingTrCalDB(TrCalDB::Head);
 TrRecon::gethead()->SetParFromDataCards();

 TrRecon::TRCon=new VCon_gb();
 AMSTrTrack::vcon= new VCon_gb();
 AMSTrRecHit::vcon= new VCon_gb();
}
//=========================================================
//   Init PART
//=========================================================
void  AMSJob::init(){


  AMSgObj::BookTimer.book("GUOUT");
  AMSgObj::BookTimer.book("GUKINE");
  
  _siamsinitjob();
  _reamsinitjob();
  _timeinitjob();
  map(1);

  const integer cl=161;
  char ffile[cl]; 
  UHTOC(IOPA.rfile,cl/sizeof(integer),ffile,cl-1);
  for (int i=cl-2; i>=0; i--) {
    if(ffile[i] == ' ') ffile[i]='\0';
    else break;
  }

ev= new Event();
  
  // Open the output root file
//  char foutname[400];sprintf(foutname,"sitcazzi.root");
  ftree= new TFile(ffile,"RECREATE");
  //    TTree::SetMaxTreeSize(2900000000);
  t4= new TTree("t4","AMSEvents");
  t4->Branch("Tracker","Event",&(ev));

  TrCalDB::Head->Write();
  

 


  cout << *this;
}

void  AMSJob::_reamsinitjob(){
  AMSgObj::BookTimer.book("REAMSEVENT");
  _retkinitjob();

}


void AMSJob::end(){
  ftree->Write();
  ftree->Close();
}



void AMSJob::_signdata(){
  CCFFKEY.init();
  FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");

GMFFKEY.GammaSource=0; //ISN  
GMFFKEY.SourceCoo[0]=1.46; //RA
GMFFKEY.SourceCoo[1]=0.384; //DEC default:Crab
GMFFKEY.SourceVisib=0.873; //50 degrees
GMFFKEY.GammaBg=0;
GMFFKEY.BgAngle=0.087; //5 degrees
FFKEY("GMSRC",(float*)&GMFFKEY,sizeof(GMFFKEY_DEF)/sizeof(integer),"MIXED");
}





//==========================================================
void AMSJob::_timeinitjob(){
  AMSgObj::BookTimer.book("TDV");

  static AMSTimeID TID(AMSID("TDV:",0));
  gethead()->addup( &TID);
    AMSTimeID::CType server=AMSTimeID::Standalone;
  {
    //
    // Magnetic Field Map
    //
    
    
    
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec=MAGFIELDFFKEY.isec[0];
    begin.tm_min=MAGFIELDFFKEY.imin[0];
    begin.tm_hour=MAGFIELDFFKEY.ihour[0];
    begin.tm_mday=MAGFIELDFFKEY.iday[0];
    begin.tm_mon=MAGFIELDFFKEY.imon[0];
    begin.tm_year=MAGFIELDFFKEY.iyear[0];
    end.tm_sec=MAGFIELDFFKEY.isec[1];
    end.tm_min=MAGFIELDFFKEY.imin[1];
    end.tm_hour=MAGFIELDFFKEY.ihour[1];
    end.tm_mday=MAGFIELDFFKEY.iday[1];
    end.tm_mon=MAGFIELDFFKEY.imon[1];
    end.tm_year=MAGFIELDFFKEY.iyear[1];
    
    TrField*  pp= TrField::GetPtr();
    
    TID.add(new 
            AMSTimeID(AMSID("MagneticFieldMap07",isRealData()),
                      begin,end,pp->GetSizeForDB(),        
                      (void*)pp->GetPointerForDB(),
                      server,1));

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
//     begin.tm_sec=TRMCFFKEY.sec[0];
//     begin.tm_min=TRMCFFKEY.min[0];
//     begin.tm_hour=TRMCFFKEY.hour[0];
//     begin.tm_mday=TRMCFFKEY.day[0];
//     begin.tm_mon=TRMCFFKEY.mon[0];
//     begin.tm_year=TRMCFFKEY.year[0];

//     end.tm_sec=TRMCFFKEY.sec[1];
//     end.tm_min=TRMCFFKEY.min[1];
//     end.tm_hour=TRMCFFKEY.hour[1];
//     end.tm_mday=TRMCFFKEY.day[1];
//     end.tm_mon=TRMCFFKEY.mon[1];
//     end.tm_year=TRMCFFKEY.year[1];


     begin.tm_sec  =0;
     begin.tm_min  =0;
     begin.tm_hour =0;
     begin.tm_mday =0;
     begin.tm_mon  =0;
     begin.tm_year =0;

     end.tm_sec=0;
     end.tm_min=0;
     end.tm_hour=0;
     end.tm_mday=0;
     end.tm_mon=0;
     end.tm_year=0;


     int need=1;
     if(isMonitoring())need=0;
     TID.add (new AMSTimeID(AMSID("TrackerCals",isRealData()),begin,end,
 			   TrCalDB::GetLinearSize(),TrCalDB::linear,
 			   server,need,SLin2CalDB));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(1),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::peds[0])*
// 			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
// 			   (void*)(AMSTrIdSoft::peds+AMSTrIdSoft::_numell),server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(0),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*AMSTrIdSoft::_numell,
// 			   (void*)AMSTrIdSoft::sigmaraws,server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(1),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*
// 			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
// 			   (void*)(AMSTrIdSoft::sigmaraws+AMSTrIdSoft::_numell),server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(0),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numell,
// 			   (void*)AMSTrIdSoft::gains,server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(1),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::gains[0])*
// 			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
// 			   (void*)(AMSTrIdSoft::gains+AMSTrIdSoft::_numell),server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(0),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numell,
// 			   (void*)AMSTrIdSoft::sigmas,server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(1),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::sigmas[0])*
// 			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
// 			   (void*)(AMSTrIdSoft::sigmas+AMSTrIdSoft::_numell),server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(0),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numell,
// 			   (void*)AMSTrIdSoft::status,server,need));
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(1),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::status[0])*
// 			   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
// 			   (void*)(AMSTrIdSoft::status+AMSTrIdSoft::_numell),server,need));
//     /*
//       TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.l",isRealData()),
//       begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*AMSTrIdSoft::_numell*2,
//       (void*)AMSTrIdSoft::rhomatrix,server,NeededByDefault));
//       TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.r",isRealData()),
//       begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*
//       (2*AMSTrIdSoft::_numel-2*AMSTrIdSoft::_numell),
//       (void*)(AMSTrIdSoft::rhomatrix+2*AMSTrIdSoft::_numell),server,NeededByDefault));
//     */
//     TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerCmnNoise(),isRealData()),
// 			   begin,end,sizeof(AMSTrIdSoft::cmnnoise),
// 			   (void*)AMSTrIdSoft::cmnnoise,server,need));
//     //TID.add (new AMSTimeID(AMSID("TrackerIndNoise",isRealData()),
//     //   begin,end,sizeof(AMSTrIdSoft::indnoise[0])*AMSTrIdSoft::_numel,
//     //   (void*)AMSTrIdSoft::indnoise,server,NeededByDefault));
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


AMSTimeID * AMSJob::gettimestructure(const AMSID & id){
  AMSNode *p=JobMap.getp(id);
  if(!p){
    cerr << "AMSJob::gettimestructure-F-no time structure found "<<id<<endl;
    exit(1);
    return 0;
  }
  else return  dynamic_cast<AMSTimeID*>(p);
}
