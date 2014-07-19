#ifndef _AMSJOB_
#define _AMSJOB_
#include "node.h"
#include "cont.h"
#include "gvolume.h"
#include "Event.h"
#include "TTree.h"
#include "TFile.h"
#include "timeid.h"


//! Temporary class for compatibility
class AMSJob :public AMSNode{
public:
  static AMSJob*  _Head;
  AMSJob(AMSID id=0,uinteger jobtype=0);
  ~AMSJob();
  static AMSJob* gethead(){return _Head;}
  
private:
  uinteger _jobtype;    // 0 == simulation
  void _init(){};
  void _siamsdata();
  void _reamsdata();

  void _retkdata();
  void _sitkdata();
  void _sitkinitjob();
  void _retkinitjob();
  void AMSJob::_signdata();

  void _tkendjob();

  void _siamsinitjob();
  void _reamsinitjob();
public:
  Event *ev;
  TTree *t4;
  TFile *ftree;
  //! sets the inital values for the datacard
  void udata();
  void data();
  void init();

  void end();
  
  static AMSNodeMap JobMap;

  static void map(integer remap=0){remap==0?JobMap.map(*_Head):JobMap.remap();}

  AMSgvolume * getgeomvolume(AMSID id){return   (AMSgvolume*)JobMap.getp(id);}


 // bit fields
  // cannot use directly bitfields due to big/little endian
  //
  const static uinteger Reconstruction;
  const static uinteger RealData;
  const static uinteger CTracker;
  const static uinteger CTOF;
  const static uinteger CAnti;
  const static uinteger CEcal;
  const static uinteger CCerenkov;
  const static uinteger CMagnet;
  const static uinteger CRICH;
  const static uinteger CTRD;
  const static uinteger CSRD;
  const static uinteger CAMS;
  const static uinteger Calibration;
  const static uinteger MTracker;
  const static uinteger MTOF;
  const static uinteger MAnti;
  const static uinteger MEcal;
  const static uinteger MCerenkov;
  const static uinteger MLVL1;
  const static uinteger MLVL3;
  const static uinteger MAxAMS;
  const static uinteger MAll;
  const static uinteger Monitoring;
  const static uinteger Production;





  uinteger isReconstruction(){return _jobtype & Reconstruction;}
  uinteger isSimulation(){return !isReconstruction();}
  uinteger isCalibration(){return _jobtype & Calibration;}
  uinteger isMonitoring(){return _jobtype & Monitoring;}
  uinteger isRealData(){return _jobtype & RealData;}
  uinteger isMCData(){ return !isRealData();}
  uinteger isProduction(){ return _jobtype & Production;}

  void _timeinitjob();
  AMSTimeID * gettimestructure();
  AMSTimeID * gettimestructure(const AMSID & id);
};

#endif
