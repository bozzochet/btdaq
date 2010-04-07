//  $Id: mainc.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeometry.h>
#include "AMSDisplay.h"
#include "Debugger.h"
#include <iostream.h>
#include <fstream.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "main.h"
#include "AMSAntiHist.h"
#include "AMSTrackerHist.h"
#include "AMSTOFHist.h"
#include "AMSLVL1Hist.h"
#include "AMSLVL3Hist.h"
#include "AMSCTCHist.h"
#include "AMSAxAMSHist.h"
#include "AMSGenHist.h"

void Myapp::HandleIdleTimer(){
SetReturnFromRun(1);
Terminate();
}
void (handler)(int);
extern void InitGui(); // loads the device dependent graphics system
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
int Error; // needed by Motif

TROOT root("AMS", "AMS ROOT", initfuncs);

int main(int argc, char *argv[])
{
     *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
     *signal(SIGTERM, handler);
     *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);


// First create application environment. If you replace TApplication
// by TRint (which inherits from TApplication) you will be able
// to execute CINT commands once in the eventloop (via Run()).
Myapp *theApp = new Myapp("App", &argc, argv);
theApp->SetIdleTimer(20,"");

  debugger.Off();
  
  char * filename = "test.root";		// default file name

  char *gtv=getenv("AMSEDDataDir");
  if(!gtv){
    cerr <<"amsedc-F-AMSEDDataDir Not defined"<<endl;
    exit(1);
  }
  char fnama[1024]="\0";
  char fnam[1024]="\0";
  char fnami[1024]="\0";
  char fnamo[1024]="\0";
  strcpy(fnama,gtv);
  strcat(fnama,"newroot");
  {  
  ifstream iftxt(fnama,ios::in);
  if(!iftxt){
     cerr <<"amsedc::init-F-ErrorOpenFile "<<fnama<<endl;
     return 1;
  }
  else {
    fnam[0]=0;
    strcpy(fnam,gtv);
    iftxt >> fnami;

    strcat(fnam,fnami);
    iftxt.close();
  }
  }
out:
    
   TFile f(fnam);


   printf("opening file %s...\n", fnam);
  AMSOnDisplay * amd= new AMSOnDisplay("AMSRoot Offline Display",&f);
  AMSAntiHist  antih("ANTI","Anti counter Hists",1,1);
  amd->AddSubDet(antih);
  AMSTrackerHist  trackerh("Tracker","Tracker  Hists",7,1);
  amd->AddSubDet(trackerh);
  AMSLVL1Hist  LVL1h("LVL1","LVL1  Hists",4,1);
  amd->AddSubDet(LVL1h);
  AMSLVL3Hist  LVL3h("LVL3","LVL3  Hists",2,1);
  amd->AddSubDet(LVL3h);
  AMSTOFHist  TOFh("TOF","TOF  Hists",9,1);
  amd->AddSubDet(TOFh);
  AMSCTCHist  CTCh("CTC","CTC  Hists",2,1);
  amd->AddSubDet(CTCh);
  AMSAxAMSHist  AxAMSh("AxAMS","AxAMS  Hists",4,1);
  amd->AddSubDet(AxAMSh);
  AMSGenHist  Genh("Gen","Gen  Hists",6,1);
  amd->AddSubDet(Genh);

  amd->Init();
  amd->SetApplication(theApp);
  amd->Begin()=0;
  amd->Sample()=4000;
      for(;;){
        amd->Fill();
        amd->DispatchProcesses();  
        theApp->Run();

                    {
                      ifstream iftxt(fnama,ios::in);
                      if(iftxt){
                       fnamo[0]=0;
                       strcpy(fnamo,gtv);
                       iftxt >> fnami;
                       strcat(fnamo,fnami);
                       iftxt.close();
                       if(strcmp(fnamo,fnam)){
                        strcpy(fnam,fnamo);
                        //                        goto out;
                        goto end;
                       }
                      }
                    }


        
      }        
  end:
      return 0;
  


}


void (handler)(int sig){
  if(sig==SIGFPE)cerr <<" FPE intercepted"<<endl;
  else if (sig==SIGTERM || sig==SIGINT){
    cerr <<" SIGTERM intercepted"<<endl;
    exit(1);
  }
  else if(sig==SIGQUIT){
      cerr <<" Process suspended"<<endl;
     pause();
  }
  else if(sig==SIGCONT){
      cerr <<" Process resumed"<<endl;
  }
}
