//  $Id: readdaq.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "daqevt.h"
#include "commons.h"
AMSDATADIR_DEF AMSDATADIR;
int main(int argc, char* argv[]){ 
  int id=0;
  char *pnull=0;
  char *fnam=0;
  char fnamd[]="/scr/choutko/AMS/datacards/daq.sea.dat";
  switch (argc) {
  case 1:
    cerr <<"readdaq-F-Please give a file name as first parameter "<<endl;
    id=1;
    argc=2;
  default:
   break;
  }
  int i;
  for(i=1;i<argc;i++){
   delete [] fnam;
   if(id==0){
     fnam = new char[strlen(argv[i])+1];
     cout <<"readdaq-I-Open file "<<argv[i]<<endl;
     DAQEvent::setfiles(argv[i],pnull);
   }
   else {
      
     cout <<"readdaq-I-Open file "<<fnamd<<endl;
     DAQEvent::setfiles(fnamd,pnull);
   }
   SELECTFFKEY.Run=-1;
   DAQEvent::init(1,1);
  }
return 0;
}
