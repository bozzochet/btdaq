//  $Id: gbatch.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <iostream.h>
#include <signal.h>
#include <unistd.h> 
#include <new.h>
#include "upool.h"
#include "apool.h"
#include "status.h"
#include "dcards.h"
#include "geantnamespace.h"
#include "producer.h"

//-------------------------------------
// ---------- ZEBRA STUFF -------------
//-------------------------------------

const int NWGEAN=13000000;
const int NWPAW=1200000;

struct PAWC_DEF{
  float q[NWPAW];
};

struct GCBANK_DEF{
  float q[NWGEAN];
};
//
#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);
GCBANK_DEF GCBANK;

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;

//-------------------------------------

void (handler)(int);
namespace glconst{
  integer cpul=1;
}

int main(int argc, char * argv[] ){
  //for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
  //  pchar++;
  //  switch (*pchar){
  //   case 'v':    //version , return buildno
  //   return AMSCommonsI::getbuildno();
  //  } 
  //}

  // so that we can write Ex. UGINIT in place of gams::UGINIT
  using namespace gams;
  //setup signal handling
  *signal(SIGFPE, handler);
  *signal(SIGCONT, handler);
  *signal(SIGTERM, handler);
  *signal(SIGXCPU,handler);
  *signal(SIGINT, handler);
  *signal(SIGQUIT, handler);
  *signal(SIGUSR1, handler); 
  *signal(SIGUSR2, handler); 
  *signal(SIGHUP, handler); 
  
  // init ZEBRA
  GZEBRA(NWGEAN);
  HLIMIT(-NWPAW);

  try{
   // Initialize the AMS Setup
    UGINIT(argc,argv);
    
   // Run the Simulation 
#ifdef __G4AMS__
    if(MISCFFKEY.G4On)g4ams::G4RUN();
    else if(MISCFFKEY.G3On)GRUN();
#else
    // run Geant3 job
    GRUN();
#endif
  } 
//-------------------------------------------------------------------  
// Handle exceptions  
//-------------------------------------------------------------------  
  catch (amsglobalerror & a){
    cerr<<a.getmessage()<< endl;
#ifdef __CORBA__
    AMSClientError ab((const char*)a.getmessage(),DPS::Client::CInAbort);
    if(AMSProducer::gethead()){
      cerr<<"setting errror"<< endl;
      AMSProducer::gethead()->Error()=ab;
    }
#endif
    // if error call a proper termination
    UGLAST(a.getmessage());
    return 1;
  }
  catch (std::bad_alloc aba){
    cerr <<"catch-F-NoMemoryAvailable "<<endl;
    UGLAST("catch-F-NoMemoryAvailable ");
    return 1;
  }
#ifdef __CORBA__
  catch (AMSClientError & ab){
    cerr<<ab.getMessage()<<" 1"<<endl;
    if(AMSProducer::gethead()){
      AMSProducer::gethead()->Error()=ab;
    }
  }
#endif
  try{
    UGLAST();
  }
  catch (amsglobalerror & a){
    cerr<<a.getmessage()<< " in UGLAST" <<endl;
    return 1;
  }    
  return 0;
}
void (handler)(int sig){
  using namespace glconst;
  switch(sig){
    case SIGFPE:
      cerr <<" FPE intercepted"<<endl;
      break;
    case SIGXCPU:
      if(cpul){
	cerr <<" Job Cpu limit exceeded"<<endl;
	cpul=0;
	GCFLAG.IEORUN=1;
	GCFLAG.IEOTRI=1;
	AMSStatus::setmode(0);
      }
      break;
    case SIGTERM: case SIGINT: 
      cerr <<" SIGTERM intercepted"<<endl;
      GCFLAG.IEORUN=1;
      GCFLAG.IEOTRI=1;
      AMSStatus::setmode(0);
      AMSFFKEY.CpuLimit=15;
      break;
    case SIGQUIT:
      cerr <<" Process suspended"<<endl;
      pause();
      break;
    case SIGCONT:
      cerr <<" Process resumed"<<endl;
      break;
    case SIGHUP:
#ifdef __CORBA__
      cout <<"got sighup "<<endl;
      if(AMSProducer::gethead() && AMSProducer::gethead()->Progressing()){
	cout <<" sending ... "<<endl;
	AMSProducer::gethead()->sendCurrentRunInfo(true);
      }
      cout << " sighup sended "<<endl;
#endif
      break;
    case SIGUSR1:
      cerr<< "New Run Forced"<<endl;
      if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
      break;
    case SIGUSR2:
      cerr<< "New Ntuple Forced"<<endl;
      if(GCFLAG.ITEST>0)GCFLAG.ITEST=-GCFLAG.ITEST;
      break;
  }
}

