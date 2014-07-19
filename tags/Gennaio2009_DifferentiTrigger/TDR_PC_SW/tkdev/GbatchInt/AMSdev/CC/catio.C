//  $Id: catio.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "io.h"
#include "commons.h"
AMSDATADIR_DEF AMSDATADIR;
int main(int argc, char* argv[]){ 
  char *fnam=0;
  switch (argc) {
  case 1:
    cerr <<"catio-F-Please give an input file name as first parameter "<<endl;
    return 1;
  case 2:
    cerr <<"catio-F-Please give an output file name as last parameter "<<endl;
    return 1;
  default:
   break;
  }
   const int mp=1000000;
   static AMSIO io[mp];
   integer ipos=0;
   integer glnum=0;
   SELECTFFKEY.Run=0;
  int i,k,ok;
  for(i=1;i<argc-1;i++){
   delete [] fnam;
   fnam = new char[strlen(argv[i])+1];
   cout <<"readio-I-Open I file "<<argv[i]<<endl;
   AMSIO::setfile(argv[i]);
   SELECTFFKEY.Run=0;
   AMSIO::init(1,1);
   do {
    ok=io[ipos++].read();
   }while(ok || ipos > mp-1);
   if(ipos  > mp -1){
     cerr << " to big output file "<<endl;
     exit(1);
   }
   io[0].close();   
   delete [] fnam;
   fnam = new char[strlen(argv[argc-1])+1];
   cout <<"readio-I-Open O file "<<argv[argc-1]<<endl;
   AMSIO::setfile(argv[argc-1]);
   SELECTFFKEY.Run=0;
   AMSIO::init(2,1);
   for(k=0;k<ipos-1;k++){
     io[k].write();
     glnum++;
   }
   io[0].close();
   ipos=0;
  }   
  cout << " Total of "<<glnum<<" events copied."<<endl;
return 0;
}
