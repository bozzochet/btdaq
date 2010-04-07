
#include "typedefs.h"

#include "amsgobj.h"
//#include "commons.h"
#include "AMSJob.h"
#include "gvolume.h"
#include "gmat.h"
#include "geantnamespace.h"
#include "AMSEvent.h"
#include "commons.h"
#include "mceventg.h"

GCFLAG_DEF GCFLAG;
GCLINK_DEF GCLINK;
GCVOLU_DEF GCVOLU;
GCTMED_DEF GCTMED;
GCTRAK_DEF GCTRAK;
GCKINE_DEF GCKINE;

//extern "C" void npq_();
//extern "C" void timest_(float & t);
//extern "C" void gstran_(int & itrt,float & ch, float &mas);

void gams::UGINIT(int argc,  char * argv[]){
  float zero=0;
  // needed only on VMS!!!!!!!!!!!!!!!!!
  //timest_(zero);
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



  // Geant3 initialization
  GZINIT();
  GPART();
  // Defines a subset of the 'stable' most common elements   in the Nature 
  GPIONS(4);
  int itrt=4;

  //define the stranglet particle
  //gstran_(itrt,CCFFKEY.StrCharge,CCFFKEY.StrMass);

  // defines materials for AMS detector
  AMSgmat::amsmat();
  // defines mediums for AMS detector
  AMSgtmed::amstmed();
  // defines geometry AMS detector
  AMSgvolume::amsgeom();

  // GRFILE(1,"geomstr.dat","Q");
  // GROUT("VOLU",1," ");
  // GREND(1);

  //GRFILE(1,"geomstr.dat","N");
  //GROUT("VOLU",1," ");
  //GREND(1);

  AMSJob::map();

  AMSJob::gethead()->init();
  //if(!AMSJob::gethead()->isProduction())AMSJob::gethead()->uhinit();
  //  AMSJob::gethead()->urinit();

  GPHYSI();
  AMSJob::map(1);
}

//===============================================================================================


void gams::UGLAST(const char *message){
  if(message)AMSJob::gethead()->setMessage(message);
  GLAST();

  AMSJob::gethead()->end();
  delete AMSJob::gethead();


}

//-----------------------------------------------------------------------
extern "C" void guout_(){
  
    
  CCFFKEY.curtime=AMSEvent::gethead()->gettime();
  
  number tt=AMSgObj::BookTimer.stop("GEANTTRACKING");
  
  AMSEvent::gethead()->event();
  
  AMSgObj::BookTimer.start("GUOUT");
  
  
  
  AMSEvent::gethead()->write();
  
  UPool.Release(0);
  AMSEvent::gethead()->remove();
  UPool.Release(1);
  AMSEvent::sethead(0);
  UPool.erase(2000000);
  AMSgObj::BookTimer.stop("GUOUT");
  

}

extern "C" void abinelclear_();


extern "C" void gukine_(){
  AMSgObj::BookTimer.start("GUKINE");

  // Set update flag to zero, as geant3 sometimes doesn;t do it itself
  GCTMED.iupd=0;

  abinelclear_();
  static integer event=0;

  // create new event & initialize it
  //  if(AMSJob::gethead()->isSimulation()){

  AMSgObj::BookTimer.start("GEANTTRACKING");


  AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
	new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));

  for(integer i=0;i<CCFFKEY.npat;i++){
    GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
    AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);

    if(genp){
      AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
      genp->run(GCKINE.ikine);
      
      genp->_printEl(cout);

    }
  }

  //}


}


