//  $Id: event.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
// TOF parts changed 25-sep-1996 by E.Choumilov.
//  ECAL added 28-sep-1999 by E.Choumilov
// add TDV/dbase version October 1, 1997. a.k.
// Jan 07, 1999. ak.
//   print EOF statistics, compare number of processed and expected 
//   events and put '*' if difference > 5%
//
// Last Edit : Jan 07, 1999. ak.
//

#include "trrawcluster.h"
#include "typedefs.h" 
#include "tofdbc02.h" 
#include "daqecblock.h"
#include "event.h"
#include "cont.h"
#include "commons.h"
#include "amsgobj.h"
#include "trrec.h"
#include "mccluster.h"
#include "tofrec02.h"
#include "beta.h"
#include "charge.h"
#include "particle.h"
#include <iostream.h>
#include <fstream.h>
#include "tofsim02.h"
#include <stdlib.h>
#include "tofcalib02.h"
#include "anticalib02.h"
#include "daqs2block.h"
#include "ntuple.h"
#include "timeid.h"
#include "trcalib.h"
#include "tralig.h"
#include "trigger102.h"
#include "trigger302.h"
#include "bcorr.h"
#include "antirec02.h"
#include "user.h"
#include <signal.h>
#include "ecaldbc.h"
#include "ecalrec.h"
#include "ecalcalib.h"
#include "richrec.h"
#include "richdbc.h"
#include "geantnamespace.h"
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "status.h"
#include "trdsim.h"
#include "trdrec.h"
#include "vtx.h"
#ifdef __G4AMS__
#include "g4util.h"
#endif
static geant   Tcpu0 = 0; 
static time_t  T0    = 0;

#ifdef __DB__

#include <dbS.h>
extern LMS* lms;

#endif
#ifdef __CORBA__
#include "producer.h"
#endif
//
//
integer AMSEvent::debug=0;
AMSEvent* AMSEvent::_Head=0;
AMSNodeMap AMSEvent::EventMap;
integer AMSEvent::SRun=0;
integer AMSEvent::PosInRun=0;
integer AMSEvent::PosGlobal=0;
void AMSEvent::_init(){
  SetTimeCoo(IOPA.mode==1);
  // Status stuff
  if(AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
    if(AMSJob::gethead()->getstatustable()->isFull(getrun(),getid(),gettime())){
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVStatus());
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
#ifdef __CORBA__
      AMSStatus *p=AMSJob::gethead()->getstatustable();
      uinteger first,last;
      p->getFL(first,last);
      AMSProducer::gethead()->sendEventTagEnd(ptdv->getname(),p->getrun(),insert,begin,end,first,last,p->getnelem(),fail);       
#endif
      ptdv->SetTime(inserto,begino,endo);
      AMSJob::gethead()->getstatustable()->reset();      
    }
#ifdef __CORBA__
    else if(AMSJob::gethead()->getstatustable()->getnelem()==1){
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVStatus());
      AMSStatus *p=AMSJob::gethead()->getstatustable();
      uinteger first,last;
      p->getFL(first,last);
     AMSProducer::gethead()->sendEventTagBegin(ptdv->getname(),p->getrun(),first);       
    }
#endif
  }
  // check old run & 
   if(_run!= SRun || !AMSJob::gethead()->isMonitoring())_validate();
  if(_run != SRun){
   cout <<" AMS-I-New Run "<<_run<<endl;
   // get rid of crazy runs
   if(_run<TRMFFKEY.OKAY/10 && AMSJob::gethead()->isRealData()){
     cerr<<"AMSEvent::_init-S-CrazyRunFound "<<_run<<endl;
     //raise(SIGTERM);
   }
   DAQEvent::initO(_run);
   
   if(AMSJob::gethead()->isSimulation())_siamsinitrun();
   _reamsinitrun();
  }

  // Initialize containers & aob
  if(AMSJob::gethead()->isSimulation())_siamsinitevent();
  _reamsinitevent();
  if(AMSJob::gethead()->isCalibration())_caamsinitevent();

#ifdef __DB__
  if (_checkUpdate() == 1 || _run != SRun) {
   cout <<"AMSEvent:: -I- UpdateMe is set. Update database and tables. "<<endl;
   int rstatus = lms -> AddAllTDV();
   int n       = AMSJob::gethead()->FillTDVTable();
   lms -> FillTDV(n);
  } else {
    // cout <<"AMSEvent:: -I- UpdateMe != 1. NO UPDATE"<<endl;
  }
  //_validateDB();
  _validate();
#endif



  if(_run != SRun){
    if (AMSFFKEY.Update && !AMSJob::gethead()->isCalibration()){
     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
       if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
       cerr <<"AMSEvent::_init-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }
    if(SRun  && TRCALIB.CalibProcedureNo == 3)AMSTrIdCalib::ntuple(SRun);
    if(SRun  && TRCALIB.CalibProcedureNo == 4){
      if(gettime()>TRCALIB.MultiRun){
       raise(SIGTERM);
      }
      AMSTrIdCalib::addonemorecalib();
    }
    SRun=_run;
    PosInRun=0;
#ifndef __DB__
   _validate(1);
#endif
  }
  SetTimeCoo(1);
  PosInRun++;
  PosGlobal++;
}


void AMSEvent::_startofrun() {

    char   prodlogdir[256];
    char   time1[26];
    char   time11[17];
    char*  logdir;

    strcpy(time1,ctime(&T0));
    for(int j=0; j<15; j++) time11[j] = time1[j+4];
    time11[15] = '\0';


    logdir = getenv("ProductionLogDirLocal");
    if (logdir) {
      strcpy(prodlogdir,logdir);
      strcat(prodlogdir,"/run_in_progress.log");
     } else {
      strcpy(prodlogdir,"/offline/logs.local/run_in_progress.log");
     }
    ofstream cfile(prodlogdir,ios::out|ios::out);
    cfile.setf(ios::dec);
    cfile<<"Production started : "<<time11<<" ***Run "<<setw(6)<<SRun<<endl;
    cfile.close();

    TIMEX(Tcpu0);
    T0 = time((time_t)0);
}

void AMSEvent::_endofrun() {


 const int NSUBS  = 3;
 const int RUN    = 0;
 const int EVENTS = 1;
 
 geant  Tcpu1;
 time_t T1;
 char   time1[26];
 char   time11[17];
 time_t Td;
 char   time0[26];
 char   time00[17];

 T1 = time((time_t)0);
 TIMEX(Tcpu1);
 

  FILE *runs;

  int   events = -1;       /* events in file   */
  int   eventsp= PosInRun; /* events processed */
  float fp     = eventsp;
  float fe;
  char* host;              /* hostname         */
  char* logdir;
  char* logsum;

  char  hh[12];

  int  found = 0;

  char line[256];
  char *ptr[NSUBS];

  char prodlogdir[256];
  char logsumf[256];

  char he[4]  = "He";
  char ahe[4] = "aHe";
  char c[4]   = "C";
  char ac[4]  = "aC";
  char p0[4]  = "P0";
  char p1[4]  = "P1";
  char u1[4]  = "U1";
  char p[4]   = "p";
  char a0[4]  = "A0";
  char ams[4] = "AMS";

  char comp[4]="";

  char aster[4]  = "  *";
  
  geant cputime = Tcpu1 - Tcpu0;
  int   icputime=cputime;

  if (SRun > 0) {
   strcpy(time1,ctime(&T0));
   int j;
   for(j=0; j<15; j++) time11[j] = time1[j+4];
   time11[15] = '\0';

   Td = SRun;
   strcpy(time0,ctime(&Td));
   for(j=0; j<15; j++) time00[j] = time0[j+4];
   time00[15] = '\0';

   logsum = getenv("RunsSummaryFile");
   if (logsum) 
     strcpy(logsumf,logsum);
   else
     strcpy(logsumf,"/offline/runs_STS91.log");
  
   if ((runs=fopen(logsumf,"r"))==NULL)
    { 
      cout<<"AMSEvent::_endofrun -W- file "<<logsumf
          <<" not found or cannot be opened"<<endl;
    } else {
     found = 0;
     while (fgets(line,255,runs)) {
       ptr[0] = strtok(line," ");
       ptr[1] = strtok(NULL," ");
       // hack
       if(atoi(ptr[0])>900578135)ptr[1] = strtok(NULL," ");
       ptr[2] = strtok(NULL," ");
       if (SRun == atoi(ptr[RUN])) {
         found = 1;
         events = atoi(ptr[EVENTS]);
         break;
       }
     }
     fclose(runs);
    }
    logdir = getenv("ProductionLogDirLocal");
    if (logdir) {
      strcpy(prodlogdir,logdir);
      strcat(prodlogdir,"/run_prod.log");
     } else {
      strcpy(prodlogdir,"/offline/logs.local/run_prod.log");
     }
     ofstream rfile(prodlogdir,ios::out|ios::app);
     if(rfile){
      rfile.setf(ios::dec);
      host = getenv("HOST");
      if (host) strcpy(hh,host);
      if (hh[0] == 'H' || hh[0] == 'h') strcpy(comp,he);
      if (hh[0] == 'C' || hh[0] == 'c') strcpy(comp,c);
      if (hh[1] == 'H' || hh[1] == 'h') strcpy(comp,ahe);
      if (hh[1] == 'C' || hh[1] == 'c') strcpy(comp,ac);
      if (hh[0] == 'A' || hh[0] == 'a') {
                        if (hh[5] == '1')                 strcpy(comp,u1);
                        if (hh[1] == 'm' || hh[1] == 'M') strcpy(comp,ams);
      }
      if (hh[0] == 'P' || hh[0] == 'p') {
       if (hh[5]=='p' || hh[5] == 'P') {
         if (hh[6] == '0') strcpy(comp,p0);
         if (hh[6] == '1') strcpy(comp,p1);
       }
       if (hh[5] == 'a' || hh[5] == 'A') strcpy(comp,a0);
       if (hh[1] == 'r') strcpy(comp,p);
      }
      icputime = cputime;
      rfile<<setw(10)<<SRun<<" "<<setw(7)<<events<<" "<<setw(7)
          <<eventsp<<" "<<setw(7)<<T1-T0<<setw(7)<<icputime<<" "<<setw(16)
          <<time11<<setw(7)<<comp;
      if (events != eventsp) {
        rfile<<aster;
        if (events > 0) {
         fe = events;
         if (fp > fe || (fe-fp)/fe > 0.049) rfile<<"*"<<endl;
        }
      }
      rfile<<endl;
       rfile.close();
     }
     else{
      cerr<<"AMSEvent::_endofrun-E-CouldNotOpenFile "<<rfile<<endl;
     }
  } // if SRun > 0
  cout<<"EndOfRun statistics."<<endl;
  cout<<"Events processed : "<<eventsp<<endl;
  cout<<"Elapsed time (s) : "<<T1-T0<<endl;
  cout<<"CPU time     (s) : "<<cputime<<endl;
  if (eventsp > 100) {
   float ft = T1 - T0;
   cout<<"Performance      : "<<ft/fp<<"/"<<cputime/fp<<endl;
  }
}

void AMSEvent::_siamsinitrun(){
_sitkinitrun();
_sitofinitrun();
_siantiinitrun();
_siecalinitrun();
_sitrdinitrun();
_sirichinitrun();
}


void AMSEvent::_reamsinitrun(){
if(AMSJob::gethead()->isProduction() ){
 if(SRun){
  HDELET(0);
  AMSJob::gethead()->uhinit(getrun(),getid(),gettime());
 }
 else if(AMSJob::gethead()->isRealData()){
   AMSJob::gethead()->uhinit(getrun(),getid(),gettime());
 }
 else{
#ifdef __CORBA__
  AMSJob::gethead()->uhinit(getrun(),getid(),gettime());
#endif
 }

}
if(AMSJob::gethead()->isProduction() && AMSJob::gethead()->isRealData()){
 if(!SRun){
 }
 else{
  _endofrun();
 }
 _startofrun();
}

_retkinitrun();
_retofinitrun();
_reantiinitrun();
_reecalinitrun();
_retrdinitrun();
_rerichinitrun();
_reaxinitrun();
AMSUser::InitRun();
}


void AMSEvent::_siamsinitevent(){
 TOF2DBc::debug=0;
 AMSBitstr::setclkphase();//set trig.electronics clock-pulse phase
 _sitofinitevent();
 _siantiinitevent();
 _siecalinitevent();
 _sitrdinitevent();
 _sirichinitevent();
}

void AMSEvent::_reamsinitevent(){
#ifdef __CORBA__
 if( getrun() != SELECTFFKEY.Run){
   seterror(2);
   throw amsglobalerror("AMSProducer-E-RunIsDifferent ",2);
 }
#endif
 _signinitevent();
 _sitkinitevent();
 _redaqinitevent();
 _retkinitevent();
 _retriginitevent();
 _retof2initevent();
 _reantiinitevent();
 _reecalinitevent();
 _retrdinitevent();
 _rerichinitevent();
 _reaxinitevent();
}

void AMSEvent::_signinitevent(){
  AMSNode *ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSmceventg",0),0));
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSmctrack",0),0));
}

 void AMSEvent::SetTimeCoo(integer rec){    
  AMSgObj::BookTimer.start("SetTimeCoo");
  // Allocate time & define the geographic coordinates
  if(AMSJob::gethead()->isSimulation() && !rec){
    static number dtime=AMSmceventg::Orbit.FlightTime/
      (GCFLAG.NEVENT+1);
    static number curtime=0;
    geant dd; 
    int i;
    number xsec=0;
    if(CCFFKEY.low==0&&GMFFKEY.GammaSource==0){ //equispaced events for sources for now
       xsec+=-dtime*(AMSmceventg::Orbit.Nskip+1)*log(RNDM(dd)+1.e-30);
      }
      else xsec+=dtime*(AMSmceventg::Orbit.Nskip+1);
    curtime+=xsec;
    if(curtime>AMSmceventg::Orbit.FlightTime){
      curtime=AMSmceventg::Orbit.FlightTime;
      GCFLAG.IEORUN=1;
    }
//    cout <<" AMSmceventg::Orbit.FlightTime "<<AMSmceventg::Orbit.FlightTime<<" "<<xsec<<" "<<curtime<<" "<<dtime<< " "<<AMSmceventg::Orbit.Nskip<<endl;
    GCFLAG.IEVENT=GCFLAG.IEVENT+AMSmceventg::Orbit.Nskip;
//    if(GCFLAG.IEVENT>GCFLAG.NEVENT){
//      GCFLAG.IEORUN=1;
//      GCFLAG.IEOTRI=1;
//      return;
//    }
    _NorthPolePhi=AMSmceventg::Orbit.PolePhi;
    AMSmceventg::Orbit.UpdateOrbit(curtime,_StationTheta,_StationPhi,_NorthPolePhi,_StationEqAsc,_StationEqDec,_StationGalLat,_StationGalLong,_time);

    _usec=(curtime-integer(curtime))*1000000000;  // nsec for mc
    AMSmceventg::Orbit.Nskip=0;        
    AMSmceventg::Orbit.Ntot++;
    _Yaw=0;
    _Roll=0;
    _Pitch=0;
    _StationSpeed=AMSmceventg::Orbit.AlphaSpeed;
    _StationRad=AMSmceventg::Orbit.AlphaAltitude;
    _SunRad=0;
    // get velocity parameters from orbit par
    AMSDir ax1(AMSDBc::pi/2-_StationTheta,_StationPhi);
    AMSDir ax2=AMSmceventg::Orbit.Axis.cross(ax1);
    //cout <<" 2 "<<AMSmceventg::Orbit.Axis<<" "<<ax1.prod(AMSmceventg::Orbit.Axis)<<endl;
    _VelTheta=AMSDBc::pi/2-ax2.gettheta();
    _VelPhi=ax2.getphi();
    
    // Once ISS celestial coo have been calculated, obtain AMS celestial coo
    number raf,decf;
    const float tilt=0.20944; // tilt of about 12 degrees around forward direction 
    
    skyposition forwardpos(_VelTheta,fmod(_VelPhi-(_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi),_StationRad,_time); // calculate celestial position towards which ISS advances (X_lvlh)
    forwardpos.GetRa(raf);  
    forwardpos.GetDec(decf);  
    
    geant ziss[3],xiss[3],yiss[3],zams[3];
    
    ziss[0] = cos(_StationEqAsc)*cos(_StationEqDec); // ISS pointing direction
    ziss[1] = sin(_StationEqAsc)*cos(_StationEqDec);
    ziss[2] = sin(_StationEqDec);  
    yiss[0] = -cos(raf)*cos(decf); // vector yiss points "backwards"
    yiss[1] = -sin(raf)*cos(decf);
    yiss[2] = -sin(decf);  
    xiss[0] = yiss[1]*ziss[2]-ziss[1]*yiss[2]; // yiss x ziss
    xiss[1] = -(yiss[0]*ziss[2]-ziss[0]*yiss[2]);
    xiss[2] = yiss[0]*ziss[1]-ziss[0]*yiss[1];
    zams[0] = sin(tilt)*xiss[0]+cos(tilt)*ziss[0]; // AMS pointing direction
    zams[1] = sin(tilt)*xiss[1]+cos(tilt)*ziss[1];
    zams[2] = sin(tilt)*xiss[2]+cos(tilt)*ziss[2];

    _AMSEqAsc=fmod(atan2(zams[1],zams[0])+AMSDBc::twopi,AMSDBc::twopi);
    _AMSEqDec=asin(zams[2]);
    skyposition amspos(_AMSEqAsc,_AMSEqDec);
    amspos.GetLong(_AMSGalLong);
    amspos.GetLat(_AMSGalLat);
  }
  else if(AMSJob::gethead()->isSimulation() && rec){
  if(CCFFKEY.oldformat){
   static number StTheta=0;
    _Yaw=0;
    _Roll=0;
    _Pitch=0;
    _StationSpeed=AMSmceventg::Orbit.AlphaSpeed;
    _StationRad=AMSmceventg::Orbit.AlphaAltitude;
    // get velocity parameters from orbit par
    AMSDir ax1(AMSDBc::pi/2-_StationTheta,_StationPhi);
    AMSDir ax2=AMSmceventg::Orbit.Axis.cross(ax1);
    if(ax1.prod(AMSmceventg::Orbit.Axis)>1e-4){
     cerr<<"AMSEvent::SetTimeCoo-W-RedefinitionOfOrbit.AxisWillBeDone "<<ax1.prod(AMSmceventg::Orbit.Axis)<<endl;
     AMSmceventg::Orbit.UpdateOrbit(_StationTheta,_StationPhi,_StationTheta-StTheta>00?1:-1);
     StTheta=_StationTheta;
    }
    _VelTheta=AMSDBc::pi/2-ax2.gettheta();
    _VelPhi=ax2.getphi();
   }
    _SunRad=0;
  }
  else if((!AMSJob::gethead()->isSimulation() && rec) || (AMSJob::gethead()->isSimulation() && IsTest())){
    static integer hint=0;
    //get right record
    if( Array[hint].Time<=_time && 
       _time<Array[hint+2>=sizeof(Array)/sizeof(Array[0])?
                  sizeof(Array)/sizeof(Array[0])-1:hint+2].Time){
      // got it
      if(_time>=Array[hint+1].Time)hint++;
    }
    else{
      //find from scratch
      for(hint=1;hint<sizeof(Array)/sizeof(Array[0]);hint++){
        if(Array[hint].Time>_time){
          hint--;
          break;
        }
      }
      if(hint>=sizeof(Array)/sizeof(Array[0]))hint=sizeof(Array)/sizeof(Array[0])-1;
    }
    
    
    if(_time == Array[hint].Time){
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[hint].GrMedPhi+AMSDBc::twopi,AMSDBc::twopi);
      _StationTheta=Array[hint].StationTheta;
      _StationPhi=fmod(Array[hint].StationPhi+AMSDBc::twopi,AMSDBc::twopi);
      _Yaw=Array[hint].StationYaw;
      _Roll=Array[hint].StationRoll;
      _Pitch=Array[hint].StationPitch;
      _StationSpeed=Array[hint].StationSpeed;
      _StationRad=Array[hint].StationR;
      _SunRad=Array[hint].SunR;
      _VelTheta=Array[hint].VelTheta;
      _VelPhi=Array[hint].VelPhi;
    }
    else if(_time == Array[hint+1].Time){
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[hint+1].GrMedPhi+AMSDBc::twopi,AMSDBc::twopi);
      _StationTheta=Array[hint+1].StationTheta;
      _StationPhi=fmod(Array[hint+1].StationPhi+AMSDBc::twopi,AMSDBc::twopi);
      _Yaw=Array[hint+1].StationYaw;
      _Roll=Array[hint+1].StationRoll;
      _Pitch=Array[hint+1].StationPitch;
      _StationSpeed=Array[hint+1].StationSpeed;
      _StationRad=Array[hint+1].StationR;
      _SunRad=Array[hint+1].SunR;
      _VelTheta=Array[hint+1].VelTheta;
      _VelPhi=Array[hint+1].VelPhi;
    }
    else if(!MISCFFKEY.BeamTest && _StationRad>6.4e8){
      //interpolation needed
      number xsec=_time-Array[hint].Time;
      number dt=Array[hint+1].Time-Array[hint].Time;
      number sYaw,cYaw,sRoll,cRoll,sPitch,cPitch;
      if(dt<xsec){
        sYaw=sin(Array[hint].StationYaw)+xsec/dt*(sin(Array[hint+1].StationYaw)-sin(Array[hint].StationYaw));
        cYaw=cos(Array[hint].StationYaw)+xsec/dt*(cos(Array[hint+1].StationYaw)-cos(Array[hint].StationYaw));
         _Yaw=atan2(sYaw,cYaw);

        sPitch=sin(Array[hint].StationPitch)+xsec/dt*(sin(Array[hint+1].StationPitch)-sin(Array[hint].StationPitch));
        cPitch=cos(Array[hint].StationPitch)+xsec/dt*(cos(Array[hint+1].StationPitch)-cos(Array[hint].StationPitch));
         _Pitch=atan2(sPitch,cPitch);

        sRoll=sin(Array[hint].StationRoll)+xsec/dt*(sin(Array[hint+1].StationRoll)-sin(Array[hint].StationRoll));
        cRoll=cos(Array[hint].StationRoll)+xsec/dt*(cos(Array[hint+1].StationRoll)-cos(Array[hint].StationRoll));
         _Roll=atan2(sRoll,cRoll);

        _StationSpeed=Array[hint].StationSpeed+xsec/dt*(Array[hint+1].StationSpeed-Array[hint].StationSpeed);
        _StationRad=Array[hint].StationR+((Array[hint].StationR>=0)?xsec/dt*(Array[hint+1].StationR-Array[hint].StationR):0);
        _SunRad=Array[hint].SunR+xsec/dt*(Array[hint+1].SunR-Array[hint].SunR);
        _VelTheta=Array[hint].VelTheta;
        _VelPhi=Array[hint].VelPhi;
      }
      else {
        _Yaw=Array[hint].StationYaw;
        _Roll=Array[hint].StationRoll;
        _Pitch=Array[hint].StationPitch;
        _StationSpeed=Array[hint].StationSpeed;
        _StationRad=Array[hint].StationR;
        _SunRad=Array[hint].SunR;
        _VelTheta=Array[hint].VelTheta;
        _VelPhi=Array[hint].VelPhi;
      }
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[hint].GrMedPhi+
                         AMSmceventg::Orbit.EarthSpeed*xsec,AMSDBc::twopi);
       // getorbit parameters from velocity
        AMSDir ax1(AMSDBc::pi/2-Array[hint].StationTheta,Array[hint].StationPhi);
        AMSDir ax2(AMSDBc::pi/2-_VelTheta,_VelPhi);
        AMSmceventg::Orbit.Axis=ax1.cross(ax2);
        AMSmceventg::Orbit.AlphaTanThetaMax=tan(acos(AMSmceventg::Orbit.Axis[2]));
       number r= tan(Array[hint].StationTheta)/
         AMSmceventg::Orbit.AlphaTanThetaMax;
       if(r > 1 || r < -1){
         cerr <<"AMSEvent::SetTimeCoo-ThetaI too high "<<
           Array[hint].StationTheta*AMSDBc::raddeg<<endl;
           AMSmceventg::Orbit.AlphaTanThetaMax=tan(fabs(Array[hint].StationTheta));
         if(r < 0 )r=-1;
         else r=1;
       }
       number idir=Array[hint+1].StationTheta>Array[hint].StationTheta?1:-1;
       number PhiZero=Array[hint].StationPhi-atan2(r,idir*sqrt(1-r*r));

      number t2=
        AMSmceventg::Orbit.AlphaTanThetaMax*AMSmceventg::Orbit.AlphaTanThetaMax;
      number philocal=
        atan2(sin(Array[hint].StationPhi-PhiZero)*sqrt(1+t2),
              cos(Array[hint].StationPhi-PhiZero));
      philocal=fmod(philocal+_StationSpeed*xsec,AMSDBc::twopi);
      number phi=atan2(sin(philocal),cos(philocal)*sqrt(1+t2));
      if(phi < 0)phi=phi+AMSDBc::twopi;
      _StationTheta=atan(AMSmceventg::Orbit.AlphaTanThetaMax*sin(phi));
      _StationPhi=fmod(phi+PhiZero+AMSDBc::twopi,AMSDBc::twopi);
//      _StationSpeed*=idir;
      // Recalculate VelTheta,VelPhi
{
        AMSDir ax1(AMSDBc::pi/2-_StationTheta,_StationPhi);
        AMSDir ax2= AMSmceventg::Orbit.Axis.cross(ax1);
        _VelPhi=ax2.getphi();
        _VelTheta= AMSDBc::pi/2-ax2.gettheta();
}

     
    }
    else {
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[0].GrMedPhi+AMSDBc::twopi,AMSDBc::twopi);
      _StationTheta=Array[0].StationTheta;
      _StationPhi=fmod(Array[0].StationPhi+AMSDBc::twopi,AMSDBc::twopi);
      _Yaw=Array[0].StationYaw;
      _Roll=Array[0].StationRoll;
      _Pitch=Array[0].StationPitch;
      _StationSpeed=Array[0].StationSpeed;
      _StationRad=Array[0].StationR;
      _SunRad=Array[0].SunR;
      _VelTheta=Array[0].VelTheta;
      _VelPhi=Array[0].VelPhi;
    }
  }

  AMSgObj::BookTimer.stop("SetTimeCoo");
}

void AMSEvent::_regnevent(){

    // Add mceventg if BeamTest
    if(MISCFFKEY.BeamTest){


    static integer hintb=0;
    //get right record
    if( ArrayB[hintb].Time<=_time && 
       _time<ArrayB[hintb+2>=sizeof(ArrayB)/sizeof(ArrayB[0])?
                  sizeof(ArrayB)/sizeof(ArrayB[0])-1:hintb+2].Time){
      // got it
      if(_time>=ArrayB[hintb+1].Time)hintb++;
    }
    else{
      //find from scratch
      for(hintb=1;hintb<sizeof(ArrayB)/sizeof(ArrayB[0]);hintb++){
        if(ArrayB[hintb].Time>_time){
          hintb--;
          break;
        }
      }
      if(hintb>=sizeof(ArrayB)/sizeof(ArrayB[0]))hintb=sizeof(ArrayB)/sizeof(ArrayB[0])-1;
    }
    integer chint;
    if(_time < ArrayB[hintb].Time){
     chint=hintb-1;
     if(chint<0)cerr<<"BeamTime-S-LogicError-chint<0 "<<_time<<" "<< ArrayB[hintb].Time<<endl;
    }
    else chint=hintb;
     // check the runtag
     
     if(ArrayB[chint].RunTag%32768!= AMSEvent::gethead()->getruntype()%32768){
        seterror();
        int chinta, chintb;
        if (chint>0)chinta=chint-1;
        else chinta=chint;
        if (chint<59)chintb=chint+1;
        else chintb=59;
        cerr<<"Event & BeamPar disagree event says runtype = "<<hex<<
        AMSEvent::gethead()->getruntype()%32768<<" BeamPar says "<<hex<<
        ArrayB[chint].RunTag<<" "<<dec<<chint<<" "<<_time <<" "<<ArrayB[chint].Time<<" "<<ArrayB[chinta].Time<<" "<<ArrayB[chintb].Time<< endl;
     }
      
      geant mom(ArrayB[chint].Mom);
      integer part(ArrayB[chint].Pid);
      AMSDir dir(ArrayB[chint].Theta,ArrayB[chint].Phi);
      AMSPoint x(ArrayB[chint].X,ArrayB[chint].Y,ArrayB[chint].Z);
      AMSmceventg *pgen=new AMSmceventg(part,mom,x,dir,ArrayB[chint].Cond);
      if(pgen->acceptio())AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0),pgen);
  }

}


void AMSEvent::_sitkinitevent(){
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}


void AMSEvent::_retriginitevent(){

   AMSEvent::gethead()->add (
     new AMSContainer(AMSID("AMSContainer:TriggerLVL1",0),0));
   AMSEvent::gethead()->add (
     new AMSContainer(AMSID("AMSContainer:TriggerLVL3",0),0));
}

void AMSEvent::_siantiinitevent(){
 AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSAntiMCCluster",0),0));
}


void AMSEvent::_siecalinitevent(){
  int i;
  AMSNode *ptr;
//
  for(i=0;i<ECALDBc::slstruc(3);i++){// book containers for EcalMCHit-object
    ptr = AMSEvent::gethead()->add (
      new AMSContainer(AMSID("AMSContainer:AMSEcalMCHit",i),0));
  }
  AMSEcalRawEvent::init();//reset EC-trig. patts.,flags...
}

void AMSEvent::_sirichinitevent(){
  AMSNode *ptr;
  ptr=AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSRichMCHit",0),0));
  if(!ptr) cout << "AMSEvent::_sirichinitevent() No container Error" <<endl;
  RICHDB::nphgen=0;
  RICHDB::nphrad=0;
  RICHDB::nphbas=0;
  RICHDB::numrefm=0;
  RICHDB::numrayl=0;
}

void AMSEvent::_sitrdinitevent(){
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDMCCluster",0),0));
}

//--------------
void AMSEvent::_sitofinitevent(){
  int il;
  AMSNode *ptr;
  integer trpatt[TOF2GC::SCLRS]={0,0,0,0};
//
//           declare some TOF containers for MC:
//
// container for geant hits:
//
  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTOFMCCluster",0),0));
//
//    container for time_over_threshold hits (digi step):
//
  for(il=0;il<TOF2GC::SCLRS;il++){
    ptr=AMSEvent::gethead()->add(
        new AMSContainer(AMSID("AMSContainer:TOF2Tovt",il),0));
  }
//<--- clear arrays for SumHT(SHT)-channel
  for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
    for(int sf=0;sf<TOF2GC::SCFETA-1;sf++){
      TOF2Tovt::SumHTh[cr][sf]=0;
      TOF2Tovt::SumSHTh[cr][sf]=0;
      for(int ht=0;ht<TOF2GC::SCTHMX2;ht++){
        TOF2Tovt::SumHTt[cr][sf][ht]=0;
        TOF2Tovt::SumSHTt[cr][sf][ht]=0;
      }
    }
  }
//
}




//=====================================================================

void AMSEvent::_reantiinitevent(){

      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:Anti2RawEvent",0),0));
      
      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AntiRawSide",0),0));
      
      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiRawCluster",0),0));

      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiRawCluster",1),0));

      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiCluster",0),0));
//
}
//=====================================================================
void AMSEvent::_retof2initevent(){
  integer i;
  AMSNode *ptr;
//---
// container for TOF2RawSide hits(same structure for MC/REAL events) : 
//
   ptr=AMSEvent::gethead()->add(
       new AMSContainer(AMSID("AMSContainer:TOF2RawSide",0),0));
//
//<--- clear static arrays for SumHT(SHT)-channel:
  for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
    for(int sf=0;sf<TOF2GC::SCFETA-1;sf++){
      TOF2RawSide::SumHTh[cr][sf]=0;
      TOF2RawSide::SumSHTh[cr][sf]=0;
      for(int ht=0;ht<TOF2GC::SCTHMX2;ht++){
        TOF2RawSide::SumHTt[cr][sf][ht]=0;
        TOF2RawSide::SumSHTt[cr][sf][ht]=0;
      }
    }
  }
//<--- clear static arrays for FTtime-channel:
  for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
    for(int sf=0;sf<TOF2GC::SCFETA;sf++){
      TOF2RawSide::FThits[cr][sf]=0;
      for(int ht=0;ht<TOF2GC::SCTHMX1;ht++){
        TOF2RawSide::FTtime[cr][sf][ht]=0;
      }
    }
  }
//
  TOF2RawSide::settrcode(-1);// reset  TOF-trigger flag
  TOF2RawSide::settrcodez(-1);// reset  TOF-trigger flag
  TOF2RawSide::setcpcode(0);// reset  TOF-trigger flag
  TOF2RawSide::setbzflag(-1);// reset  TOF-trigger flag
  TOF2RawSide::setftpatt(0);// reset glob.FT-trigger pattern
  int trpatt[TOF2GC::SCLRS]={0,0,0,0};
  TOF2RawSide::setpatt(trpatt);// reset TOF-trigger pattern
  TOF2RawSide::setpattz(trpatt);// reset TOF-trigger pattern
//---
//  container for RawCluster hits :
//
   ptr=  AMSEvent::gethead()->add (
      new AMSContainer(AMSID("AMSContainer:TOF2RawCluster",0),0));
//---
// container for Cluster hits :
//
   for( i=0;i<TOF2GC::SCLRS;i++)  ptr = AMSEvent::gethead()->add (
       new AMSContainer(AMSID("AMSContainer:AMSTOFCluster",i),0));
//
//  DAQS2Block::clrtbll();//clear sc.data length
}
//=====================================================================
void AMSEvent::_reecalinitevent(){
  integer i,maxp,maxc;
  AMSNode *ptr;
  maxp=2*ECALDBc::slstruc(3);// max SubCell-planes
  maxc=4*ECALDBc::slstruc(3)*ECALDBc::slstruc(4);// max number of SubCell
  for(i=0;i<AMSECIds::ncrates();i++){// <-- book crate type containers for EcalRawEvent
    ptr=AMSEvent::gethead()->add (
      new AMSContainer(AMSID("AMSContainer:AMSEcalRawEvent",i),0));
  }
//
  for(i=0;i<maxp;i++){// <-- book  SubCell-plane containers for EcalHit
    ptr=AMSEvent::gethead()->add (
      new AMSContainer(AMSID("AMSContainer:AMSEcalHit",i),0));
  }



  for(i=0;i<2;i++){// <-- book  proj
      AMSEvent::gethead()->add (
      new AMSContainer(AMSID("AMSContainer:Ecal1DCluster",i),&Ecal1DCluster::build,0));
 }
    AMSEvent::gethead()->add (
      new AMSContainer(AMSID("AMSContainer:Ecal2DCluster",0),&AMSEcal2DCluster::build,0));

  AMSEvent::gethead()->add (
      new AMSContainer(AMSID("AMSContainer:EcalShower",0),&AMSEcalShower::build,0));
}
void AMSEvent::_retrdinitevent(){

  for(int i=0;i<AMSTRDIdSoft::ncrates();i++) AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDRawHit",i),0));

  for(int i=0;i<trdconst::maxlay;i++) AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDCluster",i),&AMSTRDCluster::build,0));

  for(int i=0;i<trdconst::maxseg;i++) AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDSegment",i),&AMSTRDSegment::build,0));

  for(int i=0;i<1;i++) AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDTrack",i),&AMSTRDTrack::build,0));


}
void AMSEvent::_rerichinitevent(){
  AMSNode *ptr;
    ptr=AMSEvent::gethead()->add(
     new AMSContainer(AMSID("AMSContainer:AMSRichRawEvent",0),0));
  ptr=AMSEvent::gethead()->add(
    new AMSContainer(AMSID("AMSContainer:AMSRichRing",0),0));
}
//=====================================================================
void AMSEvent::_reaxinitevent(){
  integer i;
  AMSNode *ptr;
  for( i=0;i<npatb;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSBeta",i),&AMSBeta::build,0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSCharge",0),&AMSCharge::build,0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSParticle",0),&AMSParticle::build,0));
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSParticle",1),&AMSParticle::build,0));


  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AntiMatter",0),0));
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:NotAProton",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:HeavyIon",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:Test",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:Dummy",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:WriteAll",0),0));





}

void AMSEvent::_retkinitevent(){
  integer i;
  AMSNode *ptr;
  for(i=0;i<AMSTrIdSoft::ncrates();i++) AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",i),0));

  for( i=0;i<2;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrCluster",i),&AMSTrCluster::build,0));
  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrClusterWeak",i),&AMSTrCluster::buildWeak,0));

  for( i=0;i<TKDBc::nlay();i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",i),&AMSTrRecHit::build,0));

  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHitWeak",i),&AMSTrRecHit::buildWeak,0));


  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrack",i),&AMSTrTrack::build,0));

  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackWeak",i),&AMSTrTrack::buildWeak,0));

  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackFalseX",i),&AMSTrTrack::buildFalseX,0));
  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackFalseTOFX",i),&AMSTrTrack::buildFalseTOFX,0));

  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrack_PathIntegral",i),&AMSTrTrack::buildPathIntegral,0));
  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackWeak_PathIntegral",i),&AMSTrTrack::buildWeakPathIntegral,0));

  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSVtx",0),&AMSVtx::build,0));

}

void  AMSEvent::write(int trig){
    AMSgObj::BookTimer.start("WriteEvent");
  
// Sort before by "Used" variable : 
// AMSTrTrack & AMSTrCluster

  AMSEvent::gethead()->getheadC("AMSTrCluster",0,2); 
  AMSEvent::gethead()->getheadC("AMSTrCluster",1,2); 
  AMSEvent::gethead()->getheadC("AMSmceventg",0,2); 
 
for(int il=0;il<TKDBc::nlay();il++){
  AMSEvent::gethead()->getheadC("AMSTrRecHit",il,2); 
}
for(int il=0;il<AMSTRDIdSoft::ncrates();il++){
  AMSEvent::gethead()->getheadC("AMSTRDRawHit",il,2); 
}
for(int il=0;il<trdconst::maxlay;il++){
  AMSEvent::gethead()->getheadC("AMSTRDCluster",il,2); 
}

for(int il=0;il<trdconst::maxseg;il++){
  AMSEvent::gethead()->getheadC("AMSTRDSegment",il,2); 
}


for(int il=0;il<2*ECALDBc::slstruc(3);il++){
  AMSEvent::gethead()->getheadC("AMSEcalHit",il,2); 
}

for(int il=0;il<2;il++){
  AMSEvent::gethead()->getheadC("Ecal1DCluster",il,2); 
}

  AMSEvent::gethead()->getheadC("Ecal2DCluster",0,2); 
  AMSEvent::gethead()->getheadC("EcalShower",0,2); 


   
  if(IOPA.hlun || IOPA.WriteRoot){
    AMSJob::gethead()->getntuple()->reset(IOPA.WriteRoot);
    _writeEl();
    AMSNode * cur;
    for (int i=0;;){
      cur=AMSEvent::EventMap.getid(i++);   // get one by one
      if(cur){
        if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->writeC();
      }
      else break;
    }
// second pass Root Only
   if (IOPA.WriteRoot) copy();
//
    if(trig || PosInRun< (IOPA.WriteAll/1000)*1000){
// if event has been selected write it straight away
    // oh nono check for errors first
     if(HasNoErrors() || (IOPA.WriteAll/100)*100){
            AMSJob::gethead()->getntuple()->write(1);
            AMSJob::gethead()->getntuple()->writeR();
     }
     else{
      AMSJob::gethead()->getntuple()->reset(IOPA.WriteRoot);
      AMSJob::gethead()->getntuple()->write();
      AMSJob::gethead()->getntuple()->writeR();
     }
    }
    else {
// if event was not selected check if at least header should be written
// in the ntuples
      if((IOPA.WriteAll/10)%10){
      AMSJob::gethead()->getntuple()->reset(IOPA.WriteRoot);
      AMSJob::gethead()->getntuple()->write();
      AMSJob::gethead()->getntuple()->writeR();
    }
   }
    // check if one want to close ntuple 
    if( IOPA.MaxNtupleEntries){
      //cout <<"qq "<<AMSJob::gethead()->getntuple()->getentries()<<" "<<IOPA.MaxNtupleEntries<<endl;
      // cout <<"op "<<AMSJob::gethead()->GetNtupleFileSize()<<" "<<IOPA.MaxFileSize<<endl;
       if(AMSJob::gethead()->getntuple()->getentries()>=IOPA.MaxNtupleEntries || GCFLAG.ITEST<0 || AMSJob::gethead()->GetNtupleFileSize()>IOPA.MaxFileSize
      || AMSJob::gethead()->GetNtupleFileTime()>IOPA.MaxFileTime){
        if(GCFLAG.ITEST<0)GCFLAG.ITEST=-GCFLAG.ITEST;
        AMSJob::gethead()->uhend();
        AMSJob::gethead()->uhinit(_run,getid()+1,gettime());
      }
    }        
  }


    AMSgObj::BookTimer.stop("WriteEvent");

}
void  AMSEvent::printA(integer debugl){
if(debugl < 2){
_printEl(cout);
if(debugl==0)return;
AMSNode * cur;
for (int i=0;;){
  cur=AMSEvent::EventMap.getid(i++);   // get one by one
 if(cur){
   if(strncmp(cur->getname(),"AMSContainer:",13)==0 && strcmp(cur->getname(),"MC")!=0)
   ((AMSContainer*)cur)->printC(cout);

 }
 else break;
}
}
else{
_printEl(cerr);
AMSNode * cur;
for (int i=0;;){
  cur=AMSEvent::EventMap.getid(i++);   // get one by one
 if(cur){
   if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->
   printC(cerr);
 }
 else break;
}
}

}

void AMSEvent::copy(){
_copyEl();
AMSNode * cur;
for (int i=0;;){
  cur=AMSEvent::EventMap.getid(i++);   // get one by one
 if(cur){
   if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->
   copyC();
 }
 else break;
}



}
//------------------------------------------------------------------
void AMSEvent::event(){
     addnext(AMSID("WriteAll",0),new Test());
  // First Selected Events
  if(_SelectedEvents){
    if(_SelectedEvents->Run==0){
       GCFLAG.IEORUN=1;
       GCFLAG.IEOTRI=1;
       return;
    }
    else{
      EventId o(getrun(),getid());
      if(_SelectedEvents->Run > o.Run){
         if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
         return;
      }
      else if(*_SelectedEvents < o){
       while(*_SelectedEvents<o && _SelectedEvents->Run){
         if(_SelectedEvents->Event)cerr<<"AMSEvent::event-E-SelectedRunEventNotFound"<<_SelectedEvents->Run<<" "<<_SelectedEvents->Event<<endl;
         _SelectedEvents++;
       }
         if(_SelectedEvents->Run!=o.Run){
          return;
         }
       }
      if(_SelectedEvents->Event){
       if(o!=*_SelectedEvents){
         AMSJob::gethead()->getstatustable()->geteventpos(_SelectedEvents->Run,_SelectedEvents->Event,o.Event);
         return;
       }
       _SelectedEvents++;
       }       
      }
    }
    AMSgObj::BookTimer.start("EventStatus");
  if(STATUSFFKEY.status[32]){
    integer ok=AMSJob::gethead()->getstatustable()->statusok(getid(),getrun());
    if(!ok){
      int skipped=AMSJob::gethead()->getstatustable()->getnextok();
      GCFLAG.IEVENT+=skipped;
      PosInRun+=skipped;
      AMSgObj::BookTimer.stop("EventStatus");
      return;
    }
   }
   AMSgObj::BookTimer.stop("EventStatus");
    AMSUser::InitEvent();
   try{
    if(AMSJob::gethead()->isSimulation())_siamsevent();
    AMSmceventg *ptr=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    

      if(!CCFFKEY.Fast && !(!IOPA.hlun && !IOPA.WriteRoot && (DAQCFFKEY.mode/10)%10)){

        _reamsevent();
        if(AMSJob::gethead()->isCalibration())_caamsevent();
        _collectstatus();
      }
   }
   catch (AMSLVL3Error e){
    _collectstatus();
     // No LVL3
//   if(AMSStatus::isDBWriteR() || AMSStatus::isDBUpdateR()){
//    setstatus((AMSJob::gethead()->getstatustable()->getstatus(getid(),getrun())).getp());
//   }
   }
    if(AMSStatus::isDBWriteR()){
      AMSJob::gethead()->getstatustable()->adds(getrun(),getid(),getstatus(),gettime());
    }
    else if(AMSStatus::isDBUpdateR()){
      AMSJob::gethead()->getstatustable()->updates(getrun(),getid(),getstatus(),gettime());
    }
}
  //------------------------------------------------------------------
  void AMSEvent::_siamsevent(){
  AMSgObj::BookTimer.start("SIAMSEVENT");
  int cftr;  
    AMSmceventg *ptr=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(ptr){
     int iset;
     geant coo[7];
     abinelget_(iset,coo);
     if(iset){
       AMSmceventg* genp=new AMSmceventg(iset+256,coo[6],AMSPoint(coo[0],coo[1],coo[2]),AMSDir(coo[3],coo[4],coo[5]));
       AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);

     }
    }
    _siecalevent();
    _sitof2event(cftr);//important to call after _siecalevent to use FT from EC
//                       in case of absence FT from TOF; cftr=1/0 ==> was/not  
//                       (TOF+ECAL)-combined FastTrigger(FT), this flag may be used by other subr.
    _sianti2event();//Anti(as TOF) is digitized only by combined FT (checked inside of subr.!)
    _sitrdevent(); 
    _sirichevent();
    _sitkevent(); 
    _sitrigevent();//create lev1/lev3 trig.object
    _sidaqevent(); //DAQ-simulation 
  AMSgObj::BookTimer.stop("SIAMSEVENT");
  }
  
void AMSEvent::_reamsevent(){
  AMSgObj::BookTimer.start("REAMSEVENT");  

   // get beam par if any;
   _regnevent();
   if(AMSJob::gethead()->isReconstruction() && MISCFFKEY.BeamTest){
      // skip event if there is no mceventg record
      AMSContainer *p=getC("AMSmceventg",0);
      if(!p || p->getnelem()==0){
        cerr <<"_reamsevent-E-NomceventgRecord" <<getrun()<<endl;
        if(!GCFLAG.IEORUN && MISCFFKEY.BeamTest>1)GCFLAG.IEORUN=2;  //skip entire run
        AMSgObj::BookTimer.stop("REAMSEVENT");  
        return;
      }
   }
  geant d;
  if(AMSJob::gethead()->isMonitoring() && RNDM(d)>IOPA.Portion && GCFLAG.NEVENT>100){
    // skip event
     AMSgObj::BookTimer.stop("REAMSEVENT");  
     return;    
  }



#ifndef __AMSDEBUG__  
  if(AMSJob::gethead()->isReconstruction() )_redaqevent();//create subdetectors RawEvent-Objects
//                                                         +lvl1(but some det.trig.patt still missing!)
#else
 // temporary : no daq for ams02 yet
  if(AMSJob::gethead()->isReconstruction() || !strstr(AMSJob::gethead()->getsetup(),"AMS02")) _redaqevent();
#endif
  // Skip EveryThing 
  if(DAQCFFKEY.NoRecAtAll){
    AMSgObj::BookTimer.stop("REAMSEVENT");  
    return;
  }
  if(AMSJob::gethead()->isReconstruction() )_retrigevent();//add missing parts to existing(!) lvl1-obj
//                              using subdet.RawEvent objects, created at simu-stage or DAQ reco-stage 
  if(AMSEvent::gethead()->getC("TriggerLVL1",0)->getnelem() ){
    _retof2event();
    _reanti2event();
    _retrdevent();
    _retkevent(); 
    _rerichevent();
    _reecalevent();
  }
  _reaxevent();
   AMSUser::Event();
   AMSgObj::BookTimer.stop("REAMSEVENT");  
}

void AMSEvent::_caamsinitevent(){
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CAnti)_cantinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxinitevent();
}

void AMSEvent::_catkinitevent(){

  if(TRCALIB.CalibProcedureNo == 2){
   AMSEvent::gethead()->add (
   new AMSContainer(AMSID("AMSContainer:AMSTrCalibration",0),0));
  }
}

void AMSEvent::_catofinitevent(){
}


void AMSEvent::_cantinitevent(){
}


void AMSEvent::_caaxinitevent(){
}


void AMSEvent::_caamsevent(){
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CAnti)_cantievent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CEcal)_caecevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxevent();
}

void AMSEvent::_catkevent(){
  AMSgObj::BookTimer.start("CalTrFill");
    if(TRALIG.UpdateDB){
      if(!TRALIG.GlobalFit)AMSTrAligFit::Test();
      else AMSTrAligFit::Testgl();
    }
  if(TRCALIB.CalibProcedureNo == 1){
    AMSTrIdCalib::check();
  }
  else if(TRCALIB.CalibProcedureNo == 2){
int i,j;
for(i=0;i<nalg;i++){
  if(TRCALIB.Method==0 || TRCALIB.Method==i){
   int ps=TRCALIB.PatStart;
   int pe=tkcalpat;
   if(TRCALIB.MultiRun)pe=ps+2;
   for(j=ps;j<pe;j++){
     if(AMSTrCalibFit::getHead(i,j)->Test()){
      AMSgObj::BookTimer.start("CalTrFit");
      AMSTrCalibFit::getHead(i,j)->Fit();
      AMSgObj::BookTimer.stop("CalTrFit");
     }
  }
 }
}
  }
  AMSgObj::BookTimer.stop("CalTrFill");

}

//---------------------------------------------------------------------------
void AMSEvent::_catofevent(){
  bool tofft(0);
  Trigger2LVL1 *ptr2;
//
    ptr2=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr2)tofft=ptr2->TofFasTrigOK();
    if(!tofft)return;// use only H/W-triggered event
    if(TFREFFKEY.relogic[0]==2)
             TOF2TDIFcalib::select();// event selection for TOF TDIF-calibration
    if(TFREFFKEY.relogic[0]==3){
      TOF2TZSLcalib::select();// event selection for TOF TZSL-calibration
    }
    if(TFREFFKEY.relogic[0]==4)
             TOF2AMPLcalib::select();// event selection for TOF AMPL-calibration
}
//---------------------------------------------------------------------------

void AMSEvent::_cantievent(){
  bool globft(0);
  Trigger2LVL1 *ptr2;
//
    ptr2=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr2)globft=ptr2->GlobFasTrigOK();
    if(!globft)return;// use only H/W-triggered event
    if(ATREFFKEY.relogic==1)AntiCalib::select();
}
//--------------------------------------------------------------------------
void AMSEvent::_caecevent(){
  bool globft(0);
  Trigger2LVL1 *ptr;
//
    ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr){
      globft=ptr->GlobFasTrigOK();
    }
    if(!globft)return;// use only H/W-triggered event
    if(ECREFFKEY.relogic[1]>0){
      if(ECREFFKEY.relogic[1]<=2)ECREUNcalib::select();// RLGA/FIAT part of REUN-calibration
      if(ECREFFKEY.relogic[1]==3)ECREUNcalib::selecte();// ANOR part of REUN-calibration
// PedCal mode has no special select-routine( select/fill is done at validate stages)
    }
}
//--------------------------------------------------------------------------
void AMSEvent::_caaxevent(){
}
//============================================================================
void AMSEvent::_retkevent(integer refit){


// do not reconstruct events without lvl3 if  LVL3FFKEY.Accept
 
    AMSlink *ptr=getheadC("TriggerLVL3",0);
    AMSlink *ptr1=getheadC("TriggerLVL1",0);

    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
     
if(ptr1 && (!LVL3FFKEY.Accept ||  (ptr1 && ptr && (ptr302 && ptr302->LVL3OK())))){//tempor
 AMSgObj::BookTimer.start("RETKEVENT");
  AMSgObj::BookTimer.start("TrCluster");
  buildC("AMSTrCluster",refit);
  AMSgObj::BookTimer.stop("TrCluster");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTrCluster::print();
#endif
  AMSgObj::BookTimer.start("TrRecHit");
  buildC("AMSTrRecHit",refit);
  AMSgObj::BookTimer.stop("TrRecHit");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTrRecHit::print();
#endif
  
  AMSgObj::BookTimer.start("TrTrack");
  

  bool veto=true;
  bool hmul=false;
  for( AMSTRDTrack *ptrd=(AMSTRDTrack*)getheadC("AMSTRDTrack",0);ptrd;ptrd=ptrd->next()){
    if(ptrd->IsHighGammaTrack()){
     hmul=true;
     break;
    }
    else if(!(veto=ptrd->Veto(trdconst::maxlad-1))){
     break;
    }
  }
//   cout <<" ptrd "<<hmul<<"  "<<veto<<endl;
  Trigger2LVL1 * ptr12= dynamic_cast<Trigger2LVL1*>(ptr1);
  if(ptr12 && (ptr12->IsECHighEnergy() || ptr12->IsECEMagEnergy() || TRFITFFKEY.LowMargin || hmul  || veto)){
    AMSTrTrack::setMargin(1);
  }
  else{
   AMSTrTrack::setMargin(0);
  }
  integer itrk=1;


//   if(TRFITFFKEY.OldTracking && !veto){
   if(TRFITFFKEY.OldTracking ){
     //  Old Stuff
//     cout <<" old stuff "<<TRFITFFKEY.OldTracking<<endl; 
  // Default reconstruction: 4S + 4K or more
  if(TRFITFFKEY.FalseXTracking && !TRFITFFKEY.FastTracking)
    itrk = buildC("AMSTrTrackFalseX",TKDBc::nlay());
  if(itrk>0)itrk=buildC("AMSTrTrack",refit);
  // Reconstruction with looser cuts on the K side
  if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.WeakTracking ){
    buildC("AMSTrClusterWeak",refit);
    buildC("AMSTrRecHitWeak",refit);
    itrk = buildC("AMSTrTrackWeak",refit);
  }

  if(TRFITFFKEY.FastTracking){
    // Reconstruction of 4S + 3K
    if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.FalseXTracking ){
      itrk=buildC("AMSTrTrackFalseX",TKDBc::nlay()-3);
      if(itrk>0) itrk=buildC("AMSTrTrack",refit);
#ifdef __AMSDEBUG__
      if(itrk>0)cout << "FalseX - Track found "<<itrk<<endl; 
#endif
    }
  }
  // Reconstruction of 4S + TOF
  int flag =    (itrk<=0 && TRFITFFKEY.FalseTOFXTracking)
             || (TRFITFFKEY.FullReco && TRFITFFKEY.FalseTOFXTracking)
             || TRFITFFKEY.ForceFalseTOFX;
  if ( flag) {
    itrk=buildC("AMSTrTrackFalseTOFX",refit);
#ifdef __AMSDEBUG__
    if (itrk>0) cout << "FalseTOFX - Track found "<< itrk << endl;
#endif
  }
 }
 else{
  //
  // New (JA) Stuff
  //  Different logic too
  //
//     cout <<" new stuff "<<TRFITFFKEY.OldTracking<<endl; 
    // Default reconstruction: 4S + 4K or more
  if(TRFITFFKEY.FalseXTracking && !TRFITFFKEY.FastTracking)
    itrk = buildC("AMSTrTrackFalseX",TKDBc::nlay());
  itrk=buildC("AMSTrTrack_PathIntegral",refit);
  // Reconstruction with looser cuts on the K side
  if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.WeakTracking ){
    buildC("AMSTrClusterWeak",refit);
    buildC("AMSTrRecHitWeak",refit);
    itrk = buildC("AMSTrTrackWeak_PathIntegral",refit);
  }

  if(TRFITFFKEY.FastTracking){
    // Reconstruction of 4S + 3K
    if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.FalseXTracking ){
      itrk=buildC("AMSTrTrackFalseX",TKDBc::nlay()-3);
      itrk=buildC("AMSTrTrack_PathIntegral",refit);
#ifdef __AMSDEBUG__
      if(itrk>0)cout << "FalseX - Track found "<<itrk<<endl; 
#endif
    }
  }
  // Reconstruction of 4S + TOF
  int flag =    (itrk<=0 && TRFITFFKEY.FalseTOFXTracking)
             || (TRFITFFKEY.FullReco && TRFITFFKEY.FalseTOFXTracking)
             || TRFITFFKEY.ForceFalseTOFX;
  if ( flag) {
    itrk=buildC("AMSTrTrackFalseTOFX",refit);
#ifdef __AMSDEBUG__
    if (itrk>0) cout << "FalseTOFX - Track found "<< itrk << endl;
#endif
  }

 }
  
  AMSgObj::BookTimer.stop("TrTrack");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTrTrack::print();
#endif
  
 AMSgObj::BookTimer.stop("RETKEVENT");
}
 else throw AMSLVL3Error("LVL3NotCreated");  
}
//----------------------------------------
void AMSEvent::_reanti2event(){
  int stat;
  bool tofftok(0),ecalftok(0),extrigok(0);
//
//
  AMSgObj::BookTimer.start("REANTIEVENT");
//
  ANTI2JobStat::addre(0);
//
//
//
    Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr){
      tofftok=ptr->TofFasTrigOK();
      ecalftok=ptr->EcalFasTrigOK();
      extrigok=ptr->ExternTrigOK();
    }
    if(!tofftok && !ecalftok && !extrigok){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;// "no TOF/EC/Ext in LVL1-trigger"
    }
    ANTI2JobStat::addre(1);
    if(tofftok)ANTI2JobStat::addre(5);
    if(!tofftok && ecalftok)ANTI2JobStat::addre(6);
//
    Anti2RawEvent::validate(stat);// RawEvent-->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;
    }
    ANTI2JobStat::addre(2);
//
//
    AMSAntiCluster::build2(stat);// RawEvent->Cluster
    if(stat!=0){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;
    }
    ANTI2JobStat::addre(3);
// 
  #ifdef __AMSDEBUG__
   if(AMSEvent::debug)AMSAntiCluster::print();
  #endif
//
  AMSgObj::BookTimer.stop("REANTIEVENT");
}
//===============================================================================
void AMSEvent::_retof2event(){
int stat;
bool tofftok(0),ecalftok(0),extrigok(0);
//
//
  AMSgObj::BookTimer.start("RETOFEVENT");
    TOF2JobStat::addre(0);
    Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr){
      tofftok=ptr->TofFasTrigOK();
      ecalftok=ptr->EcalFasTrigOK();
      extrigok=ptr->ExternTrigOK();
    }
    if(!tofftok && !ecalftok && !extrigok){
      AMSgObj::BookTimer.stop("RETOFEVENT");
      return;// "no TOF/EC/Ext in LVL1-trigger"
    }   
    TOF2JobStat::addre(1);
    if(tofftok)TOF2JobStat::addre(33);
    if(!tofftok && ecalftok)TOF2JobStat::addre(34);
//
//                   ===> reco of real or MC events :
//
//
      AMSgObj::BookTimer.start("TOF:validation");
      TOF2RawSide::validate(stat);// RawEvent-->RawEvent
      AMSgObj::BookTimer.stop("TOF:validation");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOF2JobStat::addre(2);
//
      AMSgObj::BookTimer.start("TOF:RwEv->RwCl");
      TOF2RawCluster::build(stat); // RawEvent-->RawCluster
      AMSgObj::BookTimer.stop("TOF:RwEv->RwCl");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOF2JobStat::addre(3);
//
      AMSgObj::BookTimer.start("TOF:RwCl->Cl");
      AMSTOFCluster::build2(stat); // RawCluster-->Cluster
      AMSgObj::BookTimer.stop("TOF:RwCl->Cl");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOF2JobStat::addre(4);
//
  #ifdef __AMSDEBUG__
  if(AMSEvent::debug){
    AMSTOFCluster::print();
  }
  #endif
  AMSgObj::BookTimer.stop("RETOFEVENT");
}
//========================================================================
void AMSEvent::_reecalevent(){
  Trigger2LVL1 *ptr;
  int stat(0),nsuc(0);
  bool tofftok(0),ecalftok(0),extrigok(0);
//
  AMSgObj::BookTimer.start("REECALEVENT");
//
  EcalJobStat::addre(0);
  ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr){
    tofftok=ptr->TofFasTrigOK();
    ecalftok=ptr->EcalFasTrigOK();
    extrigok=ptr->ExternTrigOK();
  }
  if(!tofftok && !ecalftok && !extrigok){
    AMSgObj::BookTimer.stop("REECALEVENT");
    return;// "no FT/Ext-trig in LVL1"   
  }
//  if((ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5) && ecalftok)return;//if PedCalib->remove events with EcalFT IN
// !!! not needed to my mind, Truncation+SpikeSuppression do the job (need to recheck with real data !) 
  EcalJobStat::addre(1);
  if(ECMCFFKEY.fastsim==0){//           ===> slow algorithm:
//
    stat=0;
    AMSEcalRawEvent::validate(stat);// EcalRawEvent->EcalRawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("REECALEVENT");
      return;
    }
    EcalJobStat::addre(2);
//
    stat=0;
    AMSEcalHit::build(stat);// EcalRawEvent->EcalHit
    if(stat!=0){
      AMSgObj::BookTimer.stop("REECALEVENT");
      return;
    }
    EcalJobStat::addre(3);
//
      
       AMSgObj::BookTimer.start("ReEcalShowerFit");
       //two phase run due to att corr 
      for(int i=0;i<2;i++){
       buildC("Ecal1DCluster",i);
       buildC("Ecal2DCluster",i);
       int suc=buildC("EcalShower",i);
//        cout <<" succ*** "<<i<<" "<<suc<<endl;
       if(!suc)break;
       nsuc+=1;
      }
      if(nsuc==2)EcalJobStat::addre(4);

       AMSgObj::BookTimer.stop("ReEcalShowerFit");

//
//
  }
  else{    //                         ===> fast algorithm:
//
//    to be done
  }
//
//
  AMSgObj::BookTimer.stop("REECALEVENT");
}
//========================================================================
void AMSEvent::_retrdevent(){
//

  AMSgObj::BookTimer.start("RETRDEVENT");
  buildC("AMSTRDCluster");
#ifdef __AMSDEBUG__
  for(int i=0;i<trdconst::maxlay;i++){
   AMSContainer *p =getC("AMSTRDCluster",i);
   if(p && AMSEvent::debug)p->printC(cout);
//   if(p )p->printC(cout);
  }
#endif

  int nseg=buildC("AMSTRDSegment");
#ifdef __AMSDEBUG__
  for(int i=0;i<trdconst::maxseg;i++){
   AMSContainer *p =getC("AMSTRDSegment",i);
   if(p && AMSEvent::debug)p->printC(cout);
  }
#endif
  buildC("AMSTRDTrack");   
#ifdef __AMSDEBUG__
  for(int i=0;i<1;i++){
   AMSContainer *p =getC("AMSTRDTrack",i);
   if(p && AMSEvent::debug)p->printC(cout);
  }
#endif

//
//
  AMSgObj::BookTimer.stop("RETRDEVENT");
}
void AMSEvent::_rerichevent(){
  Trigger2LVL1 *ptr;
  int stat;
  bool globftok(0),extrigok(0);
//
  AMSgObj::BookTimer.start("RERICH");

  AMSRichRawEvent::build(); // Preprocess the hits

//
  ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr){
    globftok=ptr->GlobFasTrigOK();
    extrigok=ptr->ExternTrigOK();
  }
  if(!globftok && !extrigok){
    AMSgObj::BookTimer.stop("RERICH");
    return;// "no FT/Ext-trig in LVL1 trigger"   
  }
  // Reconstruction CJD
  if(RICRECFFKEY.recon[0])
    AMSRichRing::build();
//
//
  AMSgObj::BookTimer.stop("RERICH");
}
//========================================================================
void AMSEvent::_reaxevent(){
  AMSgObj::BookTimer.start("REAXEVENT");


  buildC("AMSBeta");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSBeta::print();
#endif

  buildC("AMSCharge");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSCharge::print();
#endif

  // Flag golden tracks here
  AMSTrTrack::flag_golden_tracks();

  // Vertexing
  AMSgObj::BookTimer.start("Vtx");
  buildC("AMSVtx");
  AMSgObj::BookTimer.stop("Vtx");


  buildC("AMSParticle");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSParticle::print();
#endif


  AMSgObj::BookTimer.stop("REAXEVENT");
}


void AMSEvent::_sitkinitrun(){
     int l;
     for(l=0;l<2;l++){
       for (int i=0;i<TKDBc::nlay();i++){
         for (int j=0;j<TKDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            number oldone=0;
            for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
             id.upd(k);
             geant d;
             id.setindnoise()=oldone+
             AMSTrMCCluster::sitknoiseprob(id ,id.getsig()*TRMCFFKEY.thr1R[l]);
             oldone=id.getindnoise();
            }
           }
         }
       }
     }
     // Fill the histos
     for(l=0;l<2;l++){
      geant p0=AMSTrMCCluster::sitknoiseprobU(TRMCFFKEY.thr1R[l],AMSTrMCCluster::step());
      for(int i=0;i<AMSTrMCCluster::ncha();i++){
        geant x=TRMCFFKEY.thr1R[l]+i*AMSTrMCCluster::step();
        geant prob=AMSTrMCCluster::sitknoiseprobU(x,AMSTrMCCluster::step());
        HF1(AMSTrMCCluster::hid(l),x,prob/p0);
      }
#ifdef __AMSDEBUG__
      HPRINT(AMSTrMCCluster::hid(l));
#endif
#ifdef __G4AMS__
        if(MISCFFKEY.G4On)AMSRandGeneral::book(AMSTrMCCluster::hid(l));
#endif
     }
}




void AMSEvent::_sitofinitrun(){
}

void AMSEvent::_siantiinitrun(){
}


void AMSEvent::_siecalinitrun(){
}
void AMSEvent::_sitrdinitrun(){
}
void AMSEvent::_sirichinitrun(){
}


void AMSEvent::_retkinitrun(){
  // Warning if TRFITFFKEY.FastTracking is on...
  if(TRFITFFKEY.FastTracking){
    cout <<" "<<endl<<
    "AMSEvent::Retkinitrun-W-TRFITFFKEY.FastTracking selected. "<<TRFITFFKEY.FastTracking<<endl;
  }
  for (int half=0;half<2;half++){
    for(int side=0;side<2;side++){
      cout <<"AMSEvent::_retkevent-I-"<<AMSTrIdCalib::CalcBadCh(half,side)<<
      " bad channels found for half "<<half<<" and side "<<side<<endl;
    }
  }
}


void AMSEvent::_retofinitrun(){
}

void AMSEvent::_reantiinitrun(){
}


void AMSEvent::_reecalinitrun(){





  if((AMSJob::gethead()->isCalibration() & AMSJob::CEcal) && ECREFFKEY.relogic[1]<=0){
    if(AMSECIdCalib::Run()!=0){
     AMSECIdCalib::getaverage();
     AMSECIdCalib::clear();
   }
   AMSECIdCalib::Run()=getrun();
   AMSECIdCalib::Time()=gettime();   
  }





/*
  if(AMSJob::gethead()->isRealData()){
geant gains[18][14];
ifstream fbin;
fbin.open("gains.gains");
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)fbin >> gains[i][j];
   for(int j=0;j<14;j++)gains[i][j]=1;
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)cout<< gains[i][j]<<" ";
cout <<endl;
}

   
   for(int i=0;i<ecalconst::ECSLMX;i++){
       for(int j=0;j<7;j++){
              ECcalib::ecpmcal[i][j].pmrgain()=1;
         for (int k=0;k<4;k++){
             AMSECIds ids(i,j,k,0);
           cout <<"  old gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<endl;             
              ECcalib::ecpmcal[i][j].pmscgain(k)=gains[ids.getlayer()][ids.getcell()];
           cout <<"  new gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<" "<<ids.getlayer()<<" "<<ids.getcell()<<endl;             
              
         }
       }
      } 

}
*/


}
void AMSEvent::_retrdinitrun(){
   for (int i=0;i<AMSTRDIdSoft::ncrates();i++){
      cout <<"AMSEvent::_retrdevent-I-"<<AMSTRDIdSoft::CalcBadCh(i)<<
      " bad channels found for crate "<<i<<endl;
}
}
void AMSEvent::_rerichinitrun(){
}

void AMSEvent::_reaxinitrun(){
}

void AMSEvent:: _sitkevent(){
bool fastrigger= TOF2RawSide::GlobFasTrigOK();
  if(TRMCFFKEY.NoiseOn && fastrigger )AMSTrMCCluster::sitknoise();
  AMSTrMCCluster::sitkcrosstalk();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSTrMCCluster",0);
//  if(p && AMSEvent::debug>1 )p->printC(cout);
   if(p && AMSEvent::debug )p->printC(cout);
#endif
    AMSTrRawCluster::sitkdigi();
#ifdef __AMSDEBUG__
  p =getC("AMSTrRawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
  p =getC("AMSTrRawCluster",1);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}

//----------------------------------------------------------------
void AMSEvent:: _sianti2event(){
  int stat;
  AMSgObj::BookTimer.start("SIANTIEVENT");
  ANTI2JobStat::addmc(0);
//
//
    Anti2RawEvent::mc_build(stat);// Geant_hit->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("SIANTIEVENT");
      return;// 
    }
    ANTI2JobStat::addmc(6);
//
//
#ifdef __AMSDEBUG__
  AMSContainer *p;
  p=getC("AMSAntiMCCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SIANTIEVENT");
}
//----------------------------------------------------------------
void AMSEvent:: _sitrdevent(){

  AMSgObj::BookTimer.start("SITRDDigi");
    AMSTRDRawHit::sitrddigi();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSTRDRawHit",0);
  if(p && AMSEvent::debug )p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SITRDDigi");

}
//----------------------------------------------------------------
void AMSEvent:: _siecalevent(){
  int stat;
//
/*
   int16u tesarr[200];
   int16u *p;
   char fname[80];
   strcpy(fname,"/f2users/choumilo/ams02wrk/ecalcftes1.dat");
   cout<<"Opening RawFormat test file: "<<fname<<'\n';
   ifstream stfile(fname,ios::in|ios::binary); // open file for reading
   if(!stfile){
     cout <<"siecinitevent(): missing Ecal RawFormat test-file "<<fname<<endl;
     exit(1);
   }
   stfile >>hex>> tesarr[0];//read 1st block length
   integer leng1=tesarr[0];
   cout<<"siecinitevent(): 1st block has length="<<dec<<leng1<<endl;
   if(leng1>0 && (leng1+1)<200){
     for(int i=0;i<leng1;i++){
       stfile >>hex>> tesarr[i+1];
     }
     cout<<dec<<endl;
     p=&tesarr[0];
     DAQECBlock::buildraw(leng1, p);
     cout<<"siecinitevent(): 1st block was processed !"<<endl;
//     exit(10);
   }
   else{
     cout <<"siecinitevent(): 1st block length 0 or too big - exit ! "<<endl;
     exit(1);
   }
   stfile >>hex>> tesarr[leng1+1];//read 2nd block length
   integer leng2=tesarr[leng1+1];
   cout<<"siecinitevent(): 2nd block has length="<<dec<<leng2<<endl;
   if(leng2>0 && (leng1+leng2+2)<200){
     for(int i=0;i<leng2;i++){
       stfile >>hex>> tesarr[i+leng1+2];
     }
     cout<<dec<<endl;
     p=&tesarr[leng1+1];
     DAQECBlock::buildraw(leng2, p);
     cout<<"siecinitevent(): 2nd block was processed !"<<endl;
//     exit(10);
   }
   else{
     cout <<"siecinitevent(): 2nd block length 0 or too big "<<endl;
     exit(1);
   }
*/
//
  AMSgObj::BookTimer.start("SIECALEVENT");
  EcalJobStat::addmc(0);
//
  if(ECMCFFKEY.fastsim==0){//           ===> slow algorithm:
//
    AMSEcalRawEvent::mc_build(stat);// Geant_hit->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("SIECALEVENT");
      return;
    }
    EcalJobStat::addmc(1);
//
  }
  else{    //                         ===> fast algorithm:
//
//    to be done
  }
//
#ifdef __AMSDEBUG__
  AMSContainer *p;
  p=getC("AMSEcalMCHit",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SIECALEVENT");
}
//----------------------------------------------------------------


void AMSEvent::_sirichevent(){
  AMSgObj::BookTimer.start("SIRICH");
  AMSRichRawEvent::mc_build();
  AMSPoint r(0,0,0);
  AMSPoint u(0,0,0);
  AMSEvent::gethead()->addnext(AMSID("AMSRichMCHit",0),
                               new AMSRichMCHit(Noise,0,0,r,u,Status_Fake));
  AMSgObj::BookTimer.stop("SIRICH");
}

//---------------------------------------------------------------
void AMSEvent:: _sitrigevent(){
    Trigger2LVL1::build();//build complete(!) lvl1-obj 
    TriggerLVL302::build();

}

//---------------------------------------------------------------
void AMSEvent:: _retrigevent(){
//Add missing info to HW-created(in redaqevent) lvl1-obj; can "simulate" trigger 1 & 3 for rec data
  
   Trigger2LVL1::build();
//   if(LVL3FFKEY.RebuildLVL3)TriggerLVL302::build();
}


//===============================================================
void AMSEvent:: _sitof2event(int &cftr){
  AMSContainer *p;
  int stat,i;
//
/*
   int16u tesarr[250];
   char fname[80];
//   strcpy(fname,"/f2users/choumilo/ams02wrk/tof_rfmt_tes1.dat");
//   strcpy(fname,"/f2users/choumilo/ams02wrk/tof_cfmt_tes1.dat");
   strcpy(fname,"/f2users/choumilo/ams02wrk/tof_mfmt_tes1.dat");
   cout<<"Opening DAQ-Format test file: "<<fname<<'\n';
   ifstream stfile(fname,ios::in|ios::binary); // open file for reading
   if(!stfile){
     cout <<"retof2initevent(): missing TOF DAQ-Format test-file "<<fname<<endl;
     exit(1);
   }
   stfile >>hex>> tesarr[0];
   integer leng=tesarr[0];
   cout<<"retof2initevent(): test-file has length="<<dec<<leng+1<<endl;
   if(tesarr[0]>0 && tesarr[0]<250){
     for(i=1;i<=tesarr[0];i++){
       stfile >>hex>> tesarr[i];
     }
     cout<<dec<<endl;
     DAQS2Block::buildraw(leng, tesarr);
     cout<<"retof2initevent(): test-file processed !"<<endl;
//     exit(10);
   }
   else{
     cout <<"retof2initevent(): 0 or too big test-file "<<fname<<endl;
     exit(1);
   }
*/
//
  AMSgObj::BookTimer.start("SITOFDIGI");
   AMSgObj::BookTimer.start("TOF:Ghit->Tovt");
   TOF2JobStat::addmc(0);
//
//   cout << "before build "<<endl;
//   AMSmceventg::PrintSeeds(cout);
   TOF2Tovt::build();// Ghits->TovT-hits
//    cout <<"after build "<<endl;
   AMSgObj::BookTimer.stop("TOF:Ghit->Tovt");
   TOF2JobStat::addmc(1);
//
   AMSgObj::BookTimer.start("TOF:Tovt->RwEv");
   TOF2RawSide::mc_build(stat);//Tovt_hit->RawSide_hit
   AMSgObj::BookTimer.stop("TOF:Tovt->RwEv");
   if(stat!=0){
     cftr=0;//no (TOF+ECAL)-combined fast trigger
     AMSgObj::BookTimer.stop("SITOFDIGI");
     return; // no FTrigger from TOF or ECAL
   }
   cftr=1;//found (TOF+ECAL)-combined fast trigger
   TOF2JobStat::addmc(2);
  AMSgObj::BookTimer.stop("SITOFDIGI");
//
#ifdef __AMSDEBUG__
  p=getC("AMSTOFMCCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
  p =getC("TOF2RawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}
//=============================================================

void AMSEvent::_findC(AMSID & id){

  static char names[1024]="AMSContainer:";
  static char * name=names;
#ifdef __AMSDEBUG__
  int nc=0; 
  if(13+strlen(id.getname())+1  > 1024){
   name=new char[13+strlen(id.getname())+1];
   nc=1;
   name[0]='\0';
   strcat(name,"AMSContainer:");
  }
#endif
  name[13]='\0';
  if(id.getname())strcat(name,id.getname());
  id.setname(name); 
#ifdef __AMSDEBUG__
  if(nc)delete[] name;
#endif


}


integer AMSEvent::setbuilderC(char name[], pBuilder pb){
  AMSID id(name,0);
  _findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
   p->setbuilder(pb);
   return 1;
  }
  else return 0;

}

integer AMSEvent::buildC(char name[], integer par){
   AMSID id(name,0);
   _findC(id);
   AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p){
     p->runbuilder(par);
     return p->buildOK();
   }
   else return 0; 
  
}

integer AMSEvent::rebuildC(char name[], integer par){
   AMSID id(name,0);
   _findC(id);
  for(int i=0;;i++){
   id.setid(i);
   AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p){
     p->eraseC();
   }
   else break;
  }
   id.setid(0);
   AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p){
     p->runbuilder(par);
     return p->buildOK();
   }
   else return 0; 


}


AMSlink * AMSEvent::_getheadC( AMSID id, integer sorted){
_findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    if(sorted)p->sort(sorted-1);
    // Reset global ref if any
    if(p->gethead())(p->gethead())->resethash(id.getid(),p->gethead());
    return p->gethead();
  }
  else return 0;
}
AMSlink * AMSEvent::_getlastC( AMSID id){
_findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    return p->getlast();
  }
  else return 0;
}

integer AMSEvent::_setheadC( AMSID id, AMSlink *head){
_findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p && head){
   p->sethead(head);
   return 1;
  }
  else return 0;
}

AMSContainer * AMSEvent::_getC( AMSID  id){
_findC(id);  
  AMSContainer *p = dynamic_cast<AMSContainer*>(AMSEvent::gethead()->getp(id));
  return p;
}

integer AMSEvent::getnC(char n[]){
  AMSID id;
  _findC(id);
  AMSContainer *p;
  for(int i=0;;i++){
   id.setid(i);
   p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p==0) return i;
  }
}

void AMSEvent::_copyEl(){
}

void AMSEvent::_printEl(ostream & stream){
  stream << "Run "<<_run<<" "<<getname()<<" "<< getid()<<" Time "<< 
   ctime(&_time)<<"."<<_usec<<" R "<<_StationRad<<" Theta "<<_StationTheta*AMSDBc::raddeg<<" Phi "<<_StationPhi*AMSDBc::raddeg<<" Speed "<<_StationSpeed<<
   " Pole "<<_NorthPolePhi*AMSDBc::raddeg<<endl;
  stream <<"TOFTemperature(crate_1,sensor_1): "<<TOF2JobStat::gettemp(0,0);
  Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr){
    stream <<" FastTrigRate/LiveTime "<<Trigger2LVL1::scalmon.FTrate()<<"  "<<ptr->getlivetime()<<endl;
//   stream <<" Average Scaler Rate & LifeTime "<<Trigger2LVL1::getscalersp()->getsum(gettime())<<"  "<<Trigger2LVL1::getscalersp()->getlifetime(gettime())<<endl;
  }
  stream <<" Average Magnet Temperature "<<MagnetVarp::getmeanmagnetmtemp()<<endl;
}
//=====================================================================
void AMSEvent::_writeEl(){
// Get event length
  DAQEvent *myp=(DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0);
  int nws=myp?myp->getlength():0;
// Fill the ntuple
  EventNtuple02* EN = AMSJob::gethead()->getntuple()->Get_event02();
  EN->EventStatus[0]=getstatus()[0];
  EN->EventStatus[1]=getstatus()[1];
  EN->Eventno=_id;
  EN->RawWords=nws<(1<<18)?nws:((1<<18)-1);
  EN->RawWords+=(AMSCommonsI::getosno())<<18;
  EN->RawWords+=(AMSCommonsI::getbuildno())<<20;
  EN->Run=_run;
  EN->RunType=_runtype;
  EN->Time[0]=_time;
  EN->Time[1]=AMSJob::gethead()->isRealData()?_usec:_usec/1000;
   if(!AMSJob::gethead()->isRealData()){
     AMSmceventg *p =(AMSmceventg*)getheadC("AMSmceventg",0);
     if(p){
      EN->RunType=p->getseed(0);
      EN->Time[1]=p->getseed(1);
     }
     
   }
  //EN->GrMedPhi=_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic;;
  EN->ThetaS=_StationTheta;
  EN->PhiS=fmod(_StationPhi-(_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi);
  EN->RadS=_StationRad;
  EN->Yaw=_Yaw;
  EN->Pitch=_Pitch;
  EN->Roll=_Roll;
  EN->VelocityS=_StationSpeed;
  EN->VelTheta=_VelTheta;
  EN->VelPhi=fmod(_VelPhi-(_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi);
  EN->ISSEqAsc=_StationEqAsc*(360/AMSDBc::twopi);// ISN 
  EN->ISSEqDec=_StationEqDec*(360/AMSDBc::twopi); // ISN 
  EN->ISSGalLat=_StationGalLat*(360/AMSDBc::twopi); //ISN 
  EN->ISSGalLong=_StationGalLong*(360/AMSDBc::twopi);//ISN 
  EN->AMSEqAsc=_AMSEqAsc*(360/AMSDBc::twopi);// ISN 
  EN->AMSEqDec=_AMSEqDec*(360/AMSDBc::twopi); // ISN 
  EN->AMSGalLat=_AMSGalLat*(360/AMSDBc::twopi); //ISN 
  EN->AMSGalLong=_AMSGalLong*(360/AMSDBc::twopi);//ISN     
  integer  i,nc;
  AMSContainer *p;
//  EN->Particles=0;
  EN->Tracks=0;
//  EN->Betas=0;
//  EN->Charges=0;
  EN->TrRecHits=0;
  EN->TrClusters=0;
  EN->TrRawClusters=0;
  EN->TrMCClusters=0;
  EN->TRDMCClusters=0;
  EN->TRDClusters=0;
  EN->TRDRawHits=0;
  EN->TRDSegments=0;
  EN->TRDTracks=0;
  EN->TOFClusters=0;
  EN->TOFMCClusters=0;
  EN->AntiClusters=0;
  EN->AntiMCClusters=0;
  EN->EcalClusters=0;
  EN->EcalHits=0;
  EN->RICMCClusters=0;//CJM
  EN->RICHits=0;//CJM
  getmag(EN->ThetaM,EN->PhiM);

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrTrack",i);
   if(p) EN->Tracks+=p->getnelem();
   else break;
  }



  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrRecHit",i);
   if(p) EN->TrRecHits+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrCluster",i);
   if(p) EN->TrClusters+=p->getnelem();
   else break;
  }
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrRawCluster",i);
   if(p) EN->TrRawClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrMCCluster",i);
   if(p) EN->TrMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTRDMCCluster",i);
   if(p) EN->TRDMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTRDCluster",i);
   if(p) EN->TRDClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTRDRawHit",i);
   if(p) EN->TRDRawHits+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTRDTrack",i);
   if(p) EN->TRDTracks+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTRDSegment",i);
   if(p) EN->TRDSegments+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTOFCluster",i);
   if(p) EN->TOFClusters+=p->getnelem();
   else break;
  }
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTOFMCCluster",i);
   if(p) EN->TOFMCClusters+=p->getnelem();
   else break;
  }


  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSAntiCluster",i);
   if(p) EN->AntiClusters+=p->getnelem();
   else break;
  }
  
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSAntiMCCluster",i);
   if(p) EN->AntiMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("Ecal1DCluster",i);
   if(p) EN->EcalClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSEcalHit",i);
   if(p) EN->EcalHits+=p->getnelem();
   else break;
  }

  for(i=0;;i++){//CJM
   p=AMSEvent::gethead()->getC("AMSRichMCHit",i);
   if(p) EN->RICMCClusters+=p->getnelem()-1; //Due to fake hit
   else break;
  }

  for(i=0;;i++){//CJM
   p=AMSEvent::gethead()->getC("AMSRichRawEvent",i);
   if(p) EN->RICHits+=p->getnelem();
   else break;
  }
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->fHeader.Set(EN);
#endif
  
}
//====================================================================

AMSlink * AMSEvent::addnext(AMSID id, AMSlink *p){
 AMSContainer * ptr= AMSEvent::gethead()->getC(id);
   if(ptr){
     ptr->addnext(p);
     return p;
   }
   else {
    cerr<<"AMSEvent::addnext-S-NoContainer "<<id<<endl;
#ifdef __AMSDEBUG__
    exit(1);
#endif
    return 0;
   }
}

integer AMSEvent::replace(AMSID id, AMSlink *p, AMSlink *prev){
 AMSContainer * ptr= AMSEvent::gethead()->getC(id);
   if(ptr){
     ptr->replaceEl(p,prev);
     return 1;
   }
   else {
    cerr<<"AMSEvent::replace-S-NoContainer "<<id<<endl;
#ifdef __AMSDEBUG__
    exit(1);
#endif
    return 0;
   }
}

void AMSEvent::_validate(int print){
AMSgObj::BookTimer.start("TDV");

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=dynamic_cast<AMSTimeID*>(ptid->down());
while(offspring){
  if(!offspring->Verify()){
    offspring=(AMSTimeID*)offspring->next();
    continue;
  }
  integer nb=offspring->GetNbytes();
#ifdef __AMSDEBUG__
  //          char * tmp =new char[nb];
  //          assert(tmp !=NULL);
  //          integer ncp=offspring->CopyOut((uinteger*)tmp);
  //          ncp=offspring->CopyIn((uinteger*)tmp);
  //          delete[] tmp;
#endif
  if(offspring->validate(_time)){
#ifdef __AMSDEBUG__
    if(print)cout <<"AMSEvent::_validate-I-"<<offspring->getname()<<" "<<offspring->getid()<<
      " validated. ("<<nb-sizeof(uinteger)<<" bytes ) CRC = "<<
      offspring->getCRC()<<endl;                                              
#endif
   }
    else {
      cerr<<"AMSEvent::_validate-F-"<<offspring->getname()<<" not validated."<<endl;
      time_t b,e,i;
      offspring->gettime(i,b,e);
      cerr<<" Time: "<<ctime(&_time)<<endl;
      cerr<<" Begin : " <<ctime(&b)<<endl; 
      cerr<<" End : " <<ctime(&e)<<endl; 
      cerr<<" Insert : " <<ctime(&i)<<endl; 
      throw amsglobalerror("TDV Not    Validated ",3);
 
    }
    offspring=(AMSTimeID*)offspring->next();
  }
     AMSgObj::BookTimer.stop("TDV");

}

void AMSEvent::_validateDB()
//
// compare TDV in memory with ones in DBASE
// read from DBase and update memory tables is necessary
//
{
#ifdef __DB__
AMSgObj::BookTimer.start("TDVdb");
AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
while(offspring){
    if (AMSJob::gethead()->isReadSetup()) {
     char *name=offspring -> getname();
     int  id   =offspring -> getid();
     time_t I, B, E;
     time_t i, b, e;
     integer S;
     int rstat = AMSJob::FindTheBestTDV(name, id, _time, S, I, B, E); 
     if (rstat == 1) {
       offspring -> gettime(i,b,e);
       if (i==I && b==B && e==E) {
       } else {
         cout<<"AMSEvent::_validate -I- "<<name
             <<", i,b,e need to be updated "<<endl;
#ifdef __AMSDEBUG__
         cout<<"Insert "<<ctime(&i);
         cout<<"       "<<ctime(&I);
         cout<<"Begin  "<<ctime(&b);
         cout<<"       "<<ctime(&B);
         cout<<"End    "<<ctime(&e);
         cout<<"       "<<ctime(&E)<<endl;
#endif 

        uinteger* buff = new uinteger[S];

#ifdef __AMSDEBUG__
         cout<<"EventTime "<<ctime(&_time)<<endl;
#endif
         int rstatus = lms -> ReadTDV(name, id, I, B, E, buff);
         if (rstatus == oocSuccess) {
           offspring -> CopyIn((uinteger*)buff);
           offspring -> SetTime(I,B,E);
         } 
         delete [] buff;
       }
     } 
    }
    offspring=(AMSTimeID*)offspring->next();
 }
 AMSgObj::BookTimer.stop("TDVdb");
#endif
}


integer AMSEvent::removeC(){
  // find and nullify _Head in all containers in case of memory chock;

AMSNode * cur;
int i,n=0;
for (i=0;;){
  cur=AMSEvent::EventMap.getid(i++);   // get one by one
 if(cur){
   if(strncmp(cur->getname(),"AMSContainer:",13)==0){
   n++;
   ((AMSContainer*)cur)->sethead(0);
   }
 }
 else break;
}
return n;
}

void AMSEvent::Recovery(){
      cerr <<"AMSEvent::Recovery-I-Event dump follows"<<endl;
      printA(0);
      cerr <<"AMSEvent::Recovery-I-Cleanup started"<<endl;
      UPool.ReleaseLastResort();
      cerr << "AMSEvent::Recovery-I-Last resort released"<<endl;
      //#ifndef __BIGSTACK__
      //      // line below is very unsafe/risky one.
      //      // use __BIGSTACK__ together with "limit stack 20000" 
      //      command to make safe recovery
      //       integer n=removeC();
      //      cerr <<"AMSEvent::Recovery-I- "<<n<<
      //      " containers succesfully nullified"<<endl;
      //#endif
      UPool.Release(0);
      remove();
      UPool.Release(1);
      cerr <<"AMSEvent::Recovery-I-Event structure removed"<<endl;
      sethead(0);
      UPool.erase(0);
      cerr <<"AMSEvent::Recovery-I-Memory pool released"<<endl;
      UPool.SetLastResort(10000);
      cerr <<"AMSEvent::Recovery-I-Cleanup done"<<endl;

}


void AMSEvent::_redaqinitevent(){
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",0),0));
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",4),0));
}

void AMSEvent::_redaqevent(){
  //  Add Dummy for Copying ....
  addnext(AMSID("Dummy",0),new Test());

  // Erase some containers if in debug mode
#ifdef __AMSDEBUG__
  int i;
  for(i=0;;i++){
    AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrRawCluster",i);
      if(pctr)pctr->eraseC();
      else break ;
  }
#endif


  AMSgObj::BookTimer.start("REDAQ");

   DAQEvent * pdaq = (DAQEvent*)AMSEvent::gethead()->
   getheadC("DAQEvent",0);
   if(pdaq)pdaq->buildRawStructures();
   pdaq = (DAQEvent*)AMSEvent::gethead()->
   getheadC("DAQEvent",4);
   if(pdaq)pdaq->buildRawStructures();
  AMSgObj::BookTimer.stop("REDAQ");
}


void AMSEvent::_sidaqevent(){
  AMSgObj::BookTimer.start("SIDAQ");

DAQEvent *  pdaq = new DAQEvent();
AMSEvent::gethead()->addnext(AMSID("DAQEvent",0), pdaq);      
pdaq->buildDAQ();

// H/K simulation 

pdaq=new DAQEvent();
AMSEvent::gethead()->addnext(AMSID("DAQEvent",4), pdaq);      
pdaq->buildDAQ(4);


  AMSgObj::BookTimer.stop("SIDAQ");
}







void AMSEvent::builddaq(integer i, integer length, int16u *p){

*p= getdaqid(0);
*(p+2)=int16u(_Head->_run&65535);
*(p+1)=int16u((_Head->_run>>16)&65535);
*(p+4)=int16u(_Head->_runtype&65535);
*(p+3)=int16u(_Head->_runtype>>16&65535);
uinteger _event=uinteger(_Head->_id);
*(p+6)=int16u(_event&65535);
*(p+5)=int16u((_event>>16)&65535);
*(p+8)=int16u(_Head->_time&65535);
*(p+7)=int16u((_Head->_time>>16)&65535);
*(p+10)=int16u(_Head->_usec&65535);
*(p+9)=int16u((_Head->_usec>>16)&65535);
}


void AMSEvent::buildTrackerHKdaq(integer i, integer length, int16u *p){

*p= getdaqid(4);
*(p+2)=int16u(_Head->_run&65535);
*(p+1)=int16u((_Head->_run>>16)&65535);
*(p+4)=int16u(_Head->_runtype&65535);
*(p+3)=int16u(_Head->_runtype>>16&65535);
uinteger _event=uinteger(_Head->_id);
*(p+6)=int16u(_event&65535);
*(p+5)=int16u((_event>>16)&65535);
*(p+8)=int16u(_Head->_time&65535);
*(p+7)=int16u((_Head->_time>>16)&65535);
*(p+10)=int16u(_Head->_usec&65535);
*(p+9)=int16u((_Head->_usec>>16)&65535);
}




void AMSEvent::builddaqSh(integer i, integer length, int16u *p){

*p= getdaqidSh();
uinteger tmp;
memcpy(&tmp,&_Head->_StationRad,sizeof(tmp));
*(p+2)=int16u(tmp&65535);
*(p+1)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head->_StationTheta,sizeof(tmp));
*(p+4)=int16u(tmp&65535);
*(p+3)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head->_StationPhi,sizeof(tmp));
*(p+6)=int16u(tmp&65535);
*(p+5)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head->_Yaw,sizeof(tmp));
*(p+8)=int16u(tmp&65535);
*(p+7)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head->_Pitch,sizeof(tmp));
*(p+10)=int16u(tmp&65535);
*(p+9)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head->_Roll,sizeof(tmp));
*(p+12)=int16u(tmp&65535);
*(p+11)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head->_StationSpeed,sizeof(tmp));
*(p+14)=int16u(tmp&65535);
*(p+13)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head->_VelTheta,sizeof(tmp));
*(p+16)=int16u(tmp&65535);
*(p+15)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head->_VelPhi,sizeof(tmp));
*(p+18)=int16u(tmp&65535);
*(p+17)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head->_NorthPolePhi,sizeof(tmp));
*(p+20)=int16u(tmp&65535);
*(p+19)=int16u((tmp>>16)&65535);

}



void AMSEvent::buildraw(
              integer length, int16u *p, uinteger & run, uinteger &id,
              uinteger &runtype, time_t & time, uinteger &usec){
  //  run=(*(p+1)) |  (*(p+2))<<16;
  //  runtype=(*(p+3)) |  (*(p+4))<<16;
  //  id=(*(p+5)) |  (*(p+6))<<16;
  //  time=(*(p+7)) |  (*(p+8))<<16;
  //  usec=(*(p+9)) |  (*(p+10))<<16;

    run=(*(p+2)) |  (*(p+1))<<16;
    runtype=(*(p+4)) |  (*(p+3))<<16;
    id=(*(p+6)) |  (*(p+5))<<16;
    time=(*(p+8)) |  (*(p+7))<<16;
    usec=(*(p+10)) |  (*(p+9))<<16;


}

void AMSEvent::buildrawSh(integer length, int16u *p){

    uinteger tmp=(*(p+2)) |  (*(p+1))<<16;
    memcpy(&_Head->_StationRad,&tmp,sizeof(tmp));
     tmp=(*(p+4)) |  (*(p+3))<<16;
    memcpy(&_Head->_StationTheta,&tmp,sizeof(tmp));
     tmp=(*(p+6)) |  (*(p+5))<<16;
    memcpy(&_Head->_StationPhi,&tmp,sizeof(tmp));
     tmp=(*(p+8)) |  (*(p+7))<<16;
    memcpy(&_Head->_Yaw,&tmp,sizeof(tmp));
     tmp=(*(p+10)) |  (*(p+9))<<16;
    memcpy(&_Head->_Pitch,&tmp,sizeof(tmp));
     tmp=(*(p+12)) |  (*(p+11))<<16;
    memcpy(&_Head->_Roll,&tmp,sizeof(tmp));
     tmp=(*(p+14)) |  (*(p+13))<<16;
    memcpy(&_Head->_StationSpeed,&tmp,sizeof(tmp));
     tmp=(*(p+16)) |  (*(p+15))<<16;
    memcpy(&_Head->_VelTheta,&tmp,sizeof(tmp));
     tmp=(*(p+18)) |  (*(p+17))<<16;
    memcpy(&_Head->_VelPhi,&tmp,sizeof(tmp));
     tmp=(*(p+20)) |  (*(p+19))<<16;
    memcpy(&_Head->_NorthPolePhi,&tmp,sizeof(tmp));


}


integer AMSEvent::checkdaqid(int16u id){
if(id==getdaqid(0))return 1;
else if(id==getdaqid(4))return 5 ;
else return 0;


}

integer AMSEvent::checkdaqidSh(int16u id){
 if(id==getdaqidSh())return 1;
 else return 0;
}

integer AMSEvent::calcTrackerHKl(integer i){
static integer init =0;
if(!TRMCFFKEY.WriteHK || abs(++init-TRMCFFKEY.WriteHK-1) >1)return 0;
return 1+2+2+2+4;
}

integer AMSEvent::_checkUpdate(){

integer True  = 1;
integer False = 0;

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
while(offspring){
  if(offspring->UpdateMe()) return True;
  offspring=(AMSTimeID*)offspring->next();
}
return False;
}


void AMSEvent::SetShuttlePar(){
  for(int i=0;i<sizeof(Array)/sizeof(Array[0]);i++){
    Array[i].StationR=AMSmceventg::Orbit.AlphaAltitude;
    Array[i].StationTheta=AMSmceventg::Orbit.ThetaI;  
    Array[i].StationPhi=AMSmceventg::Orbit.PhiI;
    Array[i].GrMedPhi=AMSmceventg::Orbit.PolePhi-AMSmceventg::Orbit.PolePhiStatic;
    Array[i].StationYaw=0;
    Array[i].StationPitch=0;
    Array[i].StationRoll=0;
    Array[i].StationSpeed=AMSmceventg::Orbit.AlphaSpeed;
    Array[i].SunR=0;
    Array[i].VelTheta=0;
    Array[i].VelPhi=0;
    Array[i].Time=mktime(&AMSmceventg::Orbit.Begin);
  }
}

AMSEvent::ShuttlePar AMSEvent::Array[60];
AMSEvent::BeamPar AMSEvent::ArrayB[60];


AMSID AMSEvent::getTDVStatus(){
  return AMSID(AMSJob::gethead()->getstatustable()->getname(),AMSJob::gethead()->isRealData());
}


void AMSEvent::_collectstatus(){



//  if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){
//      uinteger __status=0;
//
//  {
//    TriggerLVL3 *ptr=dynamic_cast<TriggerLVL3*>(getheadC("TriggerLVL3",0));
//    if(ptr){
//      integer lvl3o=ptr->TrackerTrigger();
//      integer prescale = lvl3o>=32?1:0;
//      lvl3o=lvl3o%32;
//      __status=__status | lvl3o | (prescale<<4);
      
//    }
//   }
//  {
//    AMSParticle *ptr=(AMSParticle*)getheadC("AMSParticle",0);
//    AMSParticle *ptr1=(AMSParticle*)getheadC("AMSParticle",1);
//    int npart=0;
//    AMSContainer *ptrc;
//    if(ptr){
//      ptrc=getC("AMSParticle",0);
//    }
//    else if(ptr1){
//      ptr=ptr1;     
//      ptrc=getC("AMSParticle",1);
//    }
//    if(ptr){
//      npart=ptrc->getnelem();
//      if(npart>3)npart=3;
//      __status= __status | (npart<<21);
//      integer charge=ptr->getcharge()-1;
//      if(charge>7)charge=7;
//      __status=__status | (charge<<5);
//      number pmom=ptr->getmomentum();
//      integer sign=pmom<0?0:1;
//      __status=__status | (sign<<8);
//      sign=(ptr->getpbeta())->getbeta()<0?0:1;
//      __status=__status | (sign<<9);
//      integer pat=(ptr->getptrack())->getpattern();
//      if(pat>31)pat=31;
//      if(pat<0)pat=31;
//      __status=__status | (pat<<10);
//       pat=(ptr->getpbeta())->getpattern();
//      uinteger spat=TKDBc::nlay()-TKDBc::patpoints((ptr->getptrack())->getpattern());
//      if(spat>3)spat=3;
//      __status=__status | (spat<<15);       
//      number rig=fabs(pmom)/(charge+1);
//      uinteger srig;
//      if(rig<2)srig=0;
//      else if(rig<8)srig=1;
//      else if(rig<20)srig=2;
//      else srig=3;
//      __status=__status | (srig<<23);
//      uinteger trquality;
//      if((ptr->getptrack())->checkstatus(AMSDBc::FalseTOFX))trquality=3;
//      else if( (ptr->getptrack())->checkstatus(AMSDBc::FalseX))trquality=2;
//      else if((ptr->getptrack())->checkstatus(AMSDBc::WEAK) )trquality=1;
//      else trquality=0;   
//      __status=__status | (trquality<<25);
//       uinteger localdb=0;
//       if((ptr->getptrack())->checkstatus(AMSDBc::LocalDB))localdb=1;
//      __status=__status | (localdb<<29);
//   }
//  }
//  {
//      AMSContainer *ptr=getC("AMSAntiCluster",0);
//      integer ncl=ptr->getnelem();
//      if(ncl>3)ncl=3;
//      __status=__status | (ncl<<19);
//  }
     
  // Now collect geomag latitude 
//     geant thetam,phim;
//     getmag(thetam,phim);
//     number cosgm=fabs(sin(thetam));
//     integer icos;
//     if(cosgm<0.1736)icos=0;
//     else if(cosgm<0.5)icos=1;
//     else if(cosgm<0.766)icos=2;
//     else icos=3;
//      __status=__status | (icos<<27);
  // Now Set Event Error
//     if(_Error==1){
//      __status=__status | (1<<30);
//     }
//      _status[0]=__status;
//      _status[1]=0;
//}
//else{
 uinteger __status=0;
 uinteger __status1=0;
  // Now Set Event Error
     if(_Error==1){
      __status=__status | (1<<30);

     }   

      _status[0]=__status;
      _status[1]=__status1;

//}
}



integer AMSEvent::IsTest(){
 
 if(MISCFFKEY.BeamTest || AMSmceventg::fixeddir() || AMSmceventg::fixedmom() )return 1;
 else return 0;   
}

AMSEvent::EventId * AMSEvent::_SelectedEvents=0;

void AMSEvent::setfile(char file[]){
  if(strlen(file)){
    ifstream ifile;
    ifile.open(file);
    if(ifile){
     // get total number of lines;
     int nline=0;
     EventId o;
     while(1){
      ifile >>o.Run >>o.Event;
      ifile.ignore(INT_MAX,'\n');        
      if(ifile.eof())break;
      if(o.Run )nline++;
     }    
     if(nline>0){ 
       int nar=nline+1;
       _SelectedEvents=new EventId[nar];
       if(_SelectedEvents){
         cout << " AMSEvent::setfile-I-SelectedEventsFound "<<nline<<endl;
         ifile.close();
         ifile.open(file);
         nline=0;
         if(ifile){ 
          while(1){
                ifile >>o.Run >> o.Event;
                ifile.ignore(INT_MAX,'\n');        
                if(ifile.eof())break;
                if(o.Run )_SelectedEvents[nline++]=o;
          }
          if(nar!=nline+1){
           cerr<<"AMSEvent::setfile-F-LogicError "<<nar<< " "<<nline+1<<endl;
           exit(1);
          }
          // order
          AMSsortNAGa(_SelectedEvents,nline);
          _SelectedEvents[nline].Run=0;
          _SelectedEvents[nline].Event=0;
          SELECTFFKEY.Run=_SelectedEvents[0].Run;
          SELECTFFKEY.Event=-1;
          // check for same events;
          int nlineo=nline;
          for( int nev=nline-1;nev>0;nev--){
            if(_SelectedEvents[nev].Run==_SelectedEvents[nev-1].Run &&
               _SelectedEvents[nev].Event==_SelectedEvents[nev-1].Event){
              // squeeze
              cerr<< " AMSEvent::setfile-W-DuplicatedEventFound "<<_SelectedEvents[nev-1].Run<<" "<<_SelectedEvents[nev-1].Event<<endl;
             for(int isq=nev;isq<nline;isq++){
               _SelectedEvents[isq]=_SelectedEvents[isq+1];    
             }
             nline--;
            }
          }
          if(nlineo!=nline){
            cout <<"AMSEvent::setfile-I-"<<nline<<" events will be processed"<<endl;
          }
         }
       }
       else cerr<<" AMSEvent::setfile-E-UnableToAllMemory for "<<nline<< 
       "events"<<endl;
     }
    }
    else{
      cerr<<"AMSEvent::setfile-E-UnableToOpenFile" <<file<<endl;
    }
  }
}

// extern "C" void geocoor_(const number & gt, const number &gp, const number &gr, geant &mt, geant &mp, geant &mr);

void AMSEvent::getmag(float & thetam, float & phim){
  number DipoleR      =AMSmceventg::Orbit.DipoleR;
  number DipoleTheta  =AMSmceventg::Orbit.DipoleTheta;
  number DipolePhi    =AMSmceventg::Orbit.DipolePhi;
  number PoleTheta    =AMSmceventg::Orbit.PoleTheta;
  number PolePhiStatic=AMSmceventg::Orbit.PolePhiStatic;

//  geant drad;
//  geocoor_(_StationTheta,_StationPhi,_StationRad/100000.,thetam,phim,drad);

  number zero=0, one=1;
  number StationTheta=_StationTheta;
  number StationPhi=_StationPhi-(_NorthPolePhi-PolePhiStatic);
  number StationRad=_StationRad;

// Station GTOD coordinates referred to dipole center
  AMSPoint StationGOTD(cos(StationTheta)*cos(StationPhi),
                       cos(StationTheta)*sin(StationPhi),sin(StationTheta));
  AMSPoint DipoleGTOD(cos(DipoleTheta)*cos(DipolePhi),
                      cos(DipoleTheta)*sin(DipolePhi),sin(DipoleTheta));
  AMSDir StationRedDirGTOD=AMSDir(StationGOTD*StationRad-DipoleGTOD*DipoleR);

// Dipole direction
  AMSDir DipoleDirGTOD=AMSDir(cos(PoleTheta)*cos(PolePhiStatic),
                              cos(PoleTheta)*sin(PolePhiStatic),sin(PoleTheta));

// For sake of clearness GEOM z axis is reversed, i.e. z = -DipoleDirGTOD, y= z x S
   AMSDir GTODs=AMSDir(zero,zero,-one);

//   AMSDir GEOMz=DipoleDirGTOD;       // conventional GEOM z coordinate definition
   AMSDir GEOMz=DipoleDirGTOD*-one;  // unconventional GEOM z coordinate definition
   AMSDir GEOMy=GEOMz.cross(GTODs);
   AMSDir GEOMx=GEOMy.cross(GEOMz);

   AMSDir StationGEOM(GEOMx.prod(StationRedDirGTOD),GEOMy.prod(StationRedDirGTOD),GEOMz.prod(StationRedDirGTOD));

   thetam=(float)(AMSDBc::pi/2-StationGEOM.gettheta());
   phim=(float)StationGEOM.getphi();

}

integer AMSEvent::getselrun(integer i){
 if(_SelectedEvents)return _SelectedEvents[i].Run;
 else return -1;
}

