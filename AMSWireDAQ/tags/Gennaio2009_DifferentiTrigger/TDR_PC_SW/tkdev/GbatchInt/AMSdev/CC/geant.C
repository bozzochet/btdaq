//  $Id: geant.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Original program by V.Choutko, the date of creation is unknown
//
// Last Edit 
//           May      1996  ak, add OBJY part
//                              session manager is modified
//                              V106
//           July     1996  ak. V109
//                              add event to the database if trig = 1
//                              Read/Write keys
//                              UGLAST definition is added
//                              print ooRunStatus();
//           Aug      1996  ak. V1.24
//                              ReadStartEnd  =  1 start read transaction
//                             (WriteStartEnd)= -1 commit read transaction
//                                            = -2 read/commit
//           Sep      1996 ak.  V1.25, VArray of numbers
//                              call AddList at the beginning of the run
//                              The argument list of AddList is modified
//                              Count number of transactions
//                              Print dbase statistics at the end
//           Oct  02, 1996 ak.  modification in Read and Write cards 
//                              put AMSFFKey.Read/Write interpretation in 
//                              uginit, eventR, eventW
//                              use CCFFKEY.run  
//                              write geometry to dbase before event processing
//           Feb  14, 1997 ak.  AMSmceventD
//           Mar  19, 1997 ak.  see db_comm.h about new eventR/eventW
//                              call dbend from uglast
//                              eventRtype 
//           Oct    , 1997 ak.  TDV dbase version implementation
//           Nov    , 1997 ak.  TKDBc dbase version implementation
//
//  Last Edit: Nov 20, 1997. ak
//

#include "typedefs.h"

#include "cern.h"
#include "mceventg.h"
#include "amsgobj.h"
#include "commons.h"
#include <math.h>
#include "trid.h"
#include <new.h>
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
#include "status.h"
#include "geantnamespace.h"
#include "timeid.h"
#ifdef __DB__
#include <db_comm.h>
integer trigEvents;               // number of events written to the DBase
integer notrigEvents;             // ALL events - trigEvents
integer readEvents;               // events read from the DBase
static  integer eventR;           // = AMSFFKEY.Read
static  integer eventW;           // = AMSFFKEY.Write
static  integer ReadStartEnd = 1; // Start a transaction
static  integer eventNu;          // event number to read from DB
static  integer jobtype;          // AMSJob::jobtype()

//class AMSEventList;
//class AMSEventTagList;

#include <dbS.h>

// declaration ooVArray(...)
#include <commonsD_ref.h>     
#include <commonsD.h>

#include <dbcatalog_ref.h>
#include <dbcatalog.h>

#include <ooVArray_uint16.h>

#include <tkdbcV_ref.h>
#include <tkdbcV.h>


#include <gmatD_ref.h>     
#include <gmatD.h>


//#include <list_ref.h>
//#include <list.h>
//
//#include <listTag_ref.h>
//#include <listTag.h>

// -
implement (ooVArray, uint16)
implement (ooVArray, geant)   
implement (ooVArray, integer) 
implement (ooVArray, ooRef(ooDBObj)) 
implement (ooVArray, TKDBcD) 
  //implement (ooVArray, ooRef(ooAMSEventList)) 
  //implement (ooVArray, ooRef(ooAMSEventTagList)) 

LMS	               dbout;
LMS*                   lms;

#endif

extern "C" void npq_();
extern "C" void timest_(float & t);
extern "C" void gstran_(int & itrt,float & ch, float &mas);
#include "producer.h"

void gams::UGINIT(int argc,  char * argv[]){
  float zero=0;
  timest_(zero);
  cout.sync_with_stdio();   
  GINIT();
  new AMSJob();
  // setup the datacards (default values and add to FFKEY queue)
  AMSJob::gethead()->data();
  // set some relevan Geant3 paramenter
  GCTLIT.ITCKOV=1;
  GCPHYS.IRAYL=1;
  integer mone=-1;
  // read the data cards
  GFFGO();
  //??????????????????????????? (set some values according to the datacards readings)
  AMSJob::gethead()->udata();

  //initialize the global AMS rotation matrix
  AMSDBc::init();  

// why do not to initialize here subdetectors db too?
  
#ifdef __CORBA__
  AMSJob::gethead()->add( new AMSProducer(argc,argv,PRODFFKEY.Debug));
  AMSProducer::gethead()->sendid();
  AMSJob::gethead()->setjobtype(AMSJob::Production);  
#endif


  // Geant3 initialization
  GZINIT();
  GPART();
  // Defines a subset of the 'stable' most common elements   in the Nature 
  GPIONS(4);
  int itrt=4;
  //define the stranglet particle
  gstran_(itrt,CCFFKEY.StrCharge,CCFFKEY.StrMass);

#ifdef __DB__
  initDB();
  lms = &dbout;
  readSetup();
  if ((AMSFFKEY.Read%2) == 1) lms -> CheckConstants();
#else
  // defines materials for AMS detector
  AMSgmat::amsmat();
  // defines mediums for AMS detector
  AMSgtmed::amstmed();
  // defines geometry AMS detector
  AMSgvolume::amsgeom();

  // GRFILE(1,"geomstr.dat","Q");
  // GROUT("VOLU",1," ");
  // GREND(1);

#endif
  //GRFILE(1,"geomstr.dat","N");
  //GROUT("VOLU",1," ");
  //GREND(1);

  AMSJob::map();

  AMSJob::gethead()->init();
#ifdef __CORBA__
  AMSTimeID * phead=AMSJob::gethead()->gettimestructure();
  AMSTimeID * down=(AMSTimeID *)phead->down();
  int nb=down->GetNbytes();
  AMSProducer::gethead()->getRunEventInfo();
#endif
  if(!AMSJob::gethead()->isProduction())AMSJob::gethead()->uhinit();
  //  AMSJob::gethead()->urinit();
#ifndef __BATCH__
#ifdef __G4AMS__
  if(MISCFFKEY.G3On)
#endif
    GDINIT();
#endif

#ifdef __DB__
  if ((AMSFFKEY.Read%2) == 1) dbout.CheckCommons();
  writeSetup();
  //  int n = AMSJob::gethead()->FillTDVTable();
  //  ooStatus rstatus = dbout.FillTDV(n);
  //  if (rstatus != oocSuccess) cerr<<"uginit_ -W- FillTDV"<<endl;
  AMSJob::gethead() -> seteventRtype(eventR);
#endif
#ifdef __G4AMS__
  if(MISCFFKEY.G4On)g4ams::G4INIT();
#endif
  GPHYSI();
  AMSJob::map(1);
}



void gams::UGLAST(const char *message){
#ifdef __CORBA__
  try{
#endif
    if(message)AMSJob::gethead()->setMessage(message);
#ifdef __G4AMS__
    if(MISCFFKEY.G4On)g4ams::G4LAST();
    if(MISCFFKEY.G3On)
#endif
      GLAST();

    delete AMSJob::gethead();

#ifdef __CORBA__
  }
  catch (AMSClientError & ab){
    cerr<<ab.getMessage()<< endl;
    return ;
  }
#endif

}
//------------------------------------------------------------------------------------
extern "C" void readDB(){

#ifdef __DB_All__

  ooHandle(AMSEventList) listH;
  ooStatus               rstatus;

  integer nevents = GCFLAG.NEVENT;
  char* jobname   = AMSJob::gethead()->getname();
  char* setup     = AMSJob::gethead() -> getsetup();
  uinteger run    = SELECTFFKEY.Run;         // run number to read
  integer  eventF = SELECTFFKEY.Event;       // event number to start from
  uinteger eventn;
  time_t   time;

  if (eventF > 0 ) {
    eventn = eventF;
  } else {
    eventn       = 0;
  }
  if (run < 0)     run = 0;

  ReadStartEnd = 0; 
  integer nST = dbout.nTransStart();
  integer nCT = dbout.nTransCommit();
  integer nAT = dbout.nTransAbort();
  number  pole, stationT, stationP;
  integer rtype;
  if (readEvents == 0) AMSJob::gethead() -> seteventRtype(eventR);
  if ((readEvents == 0) || (nST == (nCT + nAT))) ReadStartEnd = 1;
  if (dbout.recoevents()) {
    for (;;) {
      rstatus = dbout.ReadEvents(run, eventn, nevents, time, ReadStartEnd);
      if (rstatus != oocSuccess) break;
      ReadStartEnd = 0;
    }
  }
  if (dbout.mcevents() || dbout.mceventg())
    rstatus =dbout.ReadMCEvents(run, eventn, nevents, time, ReadStartEnd);
  if (rstatus == oocSuccess) {
    readEvents++;
    if ((eventR < DBWriteMC) || dbout.mceventg()) {
      eventNu++;
      return; }
  }
  gams::UGLAST();
  exit(0);
#endif
}


extern "C" void writeSetup(){

#ifdef __DB__
  if ( (AMSFFKEY.Write%2) == 1) {
    integer eventW = AMSFFKEY.Write;
    char* jobname = AMSJob::gethead()->getname();
    char* setup = AMSJob::gethead() -> getsetup();
    cout <<"uglast -I- geometry and setup will be written to database"<<endl;
    cout <<"           for the job "<<jobname<<endl;
    cout <<"           with setup  "<<setup<<endl;
    dbout.AddMaterial();
    dbout.AddTMedia();
    dbout.AddGeometry();
    dbout.Addamsdbc();
    dbout.AddAllTDV();
  }
#endif
}

extern "C" void readSetup(){

#ifdef __DB__

  if ( (AMSFFKEY.Read%2) == 0) {
    AMSgmat::amsmat();
    AMSgtmed::amstmed();
    AMSgvolume::amsgeom();
  }  
  if ((AMSFFKEY.Read%2) == 1)   {
    dbout.ReadMaterial();
    GPMATE(0);
    dbout.ReadTMedia();
    GPTMED(0);
    //AMSgmat::amsmat();
    //AMSgvolume::amsgeom();
    dbout.ReadGeometry();
    ooStatus rstatus = dbout.ReadTKDBc();
    if (rstatus != oocSuccess) {
      cerr<<"readSetup -E- Error in ReadTKDBc"<<endl;
      exit(0);
    }
  }
#endif
}

extern "C" void initDB() 
{
#ifdef __DB__
  ooMode   mode;
  ooMode   mrowmode;
  eventR = AMSFFKEY.Read;
  eventW = AMSFFKEY.Write;
  if (eventW > 0) mode = oocUpdate;
  if (eventW < 1) {
    mode = oocRead;
    mrowmode = oocMROW;
  }
  char* jobname   = AMSJob::gethead()-> getname();
  char* setup     = AMSJob::gethead() -> getsetup();
  int   jobtype   = AMSJob::gethead() -> AMSJob::jobtype();
  const char* version   = AMSCommonsI::getversion();

  cout <<"_uginit -I- LMS init for job     "<<jobname<<endl;
  cout <<"                         setup   "<<setup<<endl;
  cout <<"                         jobtype "<<jobtype<<endl;
  cout <<"                         version "<<version<<endl;
  dbout.setapplicationName(jobname);
  dbout.setprefix(jobname);
  dbout.setsetup(setup);
  dbout.setversion(version);
  dbout.setjobtype(jobtype);
  dbout.settypeR(eventR);
  dbout.settypeW(eventW);
  dbout.ClusteringInit(mode,mrowmode);
#endif
}
