//  $Id: main.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include <TRegexp.h>
#include <TChain.h>
#include <TRootApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGeometry.h>
#include "AMSDisplay.h"
#include <iostream>
#include <fstream>
#ifndef WIN32
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#endif
#ifndef __APPLE__
#include <TRFIOFile.h>
#include <TXNetFile.h>
#endif
#include <stdlib.h>
#include <signal.h>
#include "AMSNtupleV.h"
#include "main.h"
#include <TString.h>
TString * Selector;
extern void * gAMSUserFunction;
void OpenChain(TChain & chain, char * filename);
#ifndef WIN32
static int Selectsdir(
#ifndef __APPLE__
const dirent *entry
#else
dirent *entry
#endif
=0);
#endif
void Myapp::HandleIdleTimer(){
  
  if(fDisplay){
   StopIdleing();
   fDisplay->ShowNextEvent(1);
   StartIdleing();
  }
//  SetReturnFromRun(1);
//Terminate();
  cout <<"exiting handler"<<endl;
}

void (handler)(int);
int main(int argc, char *argv[]){

// First create application environment. If you replace TApplication
// by TRint (which inherits from TApplication) you will be able
// to execute CINT commands once in the eventloop (via Run()).
#ifndef WIN32
     *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
     *signal(SIGTERM, handler);
     *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);
#endif
  char * filename = 0;		// default file name

  if ( argc > 1 ) {		// now take the file name
    filename = *++argv;
  }
  AMSNtupleV *pntuple=0;
  TChain chain("AMSRoot");
  if(filename){
   OpenChain(chain,filename); 
   pntuple= new AMSNtupleV(&chain);
  }  
  printf("opening file %s...\n", filename);
  int err=0;
  int argcc=1;
#if defined(WIN32) || defined(__APPLE__)
    Myapp *theApp = new Myapp("App", &argcc, argv);
#else
  gVirtualX=new TGX11("X11","Root Interface to X11");
  gGuiFactory=new TRootGuiFactory();
  Myapp *theApp = new Myapp("App", &argcc, argv);
  theApp->SetStatic();
#endif
//  gDebug=6; 



  //
  //Get the Geometry from the ROOT file  (obsolete, should be done via geomanager)
  //        --------
  char *geo_dir=getenv("AMSGeoDir");
  if ( !geo_dir || strlen(geo_dir) == 0 ) {
    cerr <<"AMSEventDisplay-F-AMSGeoDir Not Defined"<<endl;
    exit(1);
  }

  char geoFile[256];
  strcpy(geoFile,geo_dir);

  char *geoFile_new = "ams02.geom";
  strcat(geoFile,geoFile_new);
  TFile fgeo(geoFile);

  TGeometry *geo = (TGeometry *)fgeo.Get("ams02");
  if (geo==0) {
    cerr << "AMSEventDisplay-F-Failed to open " << geoFile
         << "\n or cannot find the Geometry in it" << endl;
    exit(1);
  }


  AMSDisplay * amd= new AMSDisplay("AMSRoot Offline Display",geo,pntuple);
  amd->SetApplication(theApp);
  amd->Init();
   theApp->SetDisplay(amd);  
    theApp->RemoveIdleTimer();
    amd->DrawEvent();
    theApp->Run();
      return 0;
  


}
//---------------------------------------




void (handler)(int sig){
#ifndef WIN32
  switch(sig){
  case  SIGFPE:
   cerr <<" FPE intercepted"<<endl;
   break;
  case  SIGTERM:
   cerr <<" SIGTERM intercepted"<<endl;
   exit(1);
   break;
  case  SIGINT:
   cerr <<" SIGTERM intercepted"<<endl;
   exit(1);
   break;
  case  SIGQUIT:
   cerr <<" Process suspended"<<endl;
   pause();
   break;
  case  SIGCONT:
   cerr <<" Process resumed"<<endl;
   break;
  }
#endif
}


void OpenChain(TChain & chain, char * filenam){
//  
//  root can;t cope with multiple wild card so we must do it ourselves
//
   char filename[255];
   if(strlen(filenam)==0 || strlen(filenam)>sizeof(filename)-1){
       cerr <<"OpenChain-F-InvalidFileName "<<filenam<<endl;
       return;
   }
   TString a(filenam);
   TRegexp b("^castor:",false);
   TRegexp c("^http:",false);
   TRegexp d("^root:",false);
   TRegexp e("^rfio:",false);
   TRegexp f("^/castor",false);
   bool wildsubdir=false;
   if(a.Contains(b)){
#if !defined(WIN32) && !defined(__APPLE__)
    TRFIOFile f("");
    TXNetFile g("");
#endif
    strcpy(filename,filenam);
   }
   else if(a.Contains(c)){
    strcpy(filename,filenam);
   }
   else if(a.Contains(d)){
    strcpy(filename,filenam);
   }
   else if(a.Contains(e)){
    strcpy(filename,filenam);
   }
   else if(a.Contains(f)){
    strcpy(filename,"rfio:");
    strcat(filename,filenam);
   }
   else{ 
#ifndef WIN32
    if(filenam[0]!='/'){
     strcpy(filename,"./");
     strcat(filename,filenam);
    }
   else strcpy(filename,filenam); 
#else
   strcpy(filename,filenam); 
#endif
   bool go=false;
   for(int i=strlen(filename)-1;i>=0;i--){
     if(filename[i]=='/')go=true;
     if(go && filename[i]=='*'){
       wildsubdir=true;
       break;
     }
   }
   }
#ifndef WIN32
   if(wildsubdir){
    for(int i=0;i<strlen(filename);i++){
     if (filename[i]=='*'){
        for(int k=i-1;k>=0;k--){
           if(filename[k]=='/'){
              TString ts(filename,k+1);
              for(int l=i+1;l<strlen(filename);l++){
               if(filename[l]=='/'){
                
                if(l-k-1>0)Selector= new TString(filename+k+1,l-k-1);
                else Selector=0;
                dirent ** namelistsubdir;
                cout <<"  scanning "<<ts<<" "<<Selector<<" l "<<l<<" "<<i<<endl;
                int nptrdir=scandir(ts.Data(),&namelistsubdir,Selectsdir,NULL);
                for( int nsd=0;nsd<nptrdir;nsd++){
                  char fsdir[1023];
                  strcpy(fsdir,ts.Data());
                  strcat(fsdir,namelistsubdir[nsd]->d_name);
                  strcat(fsdir,filename+l);
                  cout << "found dir "<<fsdir<<endl;
                  OpenChain(chain,fsdir);
                } 
                return;               
               }
               }
              }
             }                  
           }
        }
      }
#endif
    chain.Add(filename);
}



#ifndef WIN32
int Selectsdir(
#ifndef __APPLE__
const dirent *entry){
#else
dirent *entry){
#endif
if(Selector){
 TString a(entry->d_name);
 TRegexp b(Selector->Data(),true);
 return a.Contains(b) && entry->d_name[0]!='.';
}
else return entry->d_name[0]!='.';
}
#endif
