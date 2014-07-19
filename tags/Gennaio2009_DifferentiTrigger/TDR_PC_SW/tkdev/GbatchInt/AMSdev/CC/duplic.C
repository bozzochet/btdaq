//  $Id: duplic.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "io.h"
#include "commons.h"
#include "amsstl.h"
int cmpt (const void * p1, const void *p2);
AMSDATADIR_DEF AMSDATADIR;
int main(int argc, char* argv[]){ 
  char *fnam=0;
  switch (argc) {
  case 1:
    cerr <<"catio-F-Please give an input file name as first parameter "<<endl;
    return 1;
  default:
   break;
  }
   const int mp=1000000;
   static AMSIO io[mp+1];
   const int mgl=70000000;
   static long seed[mgl+1];
   integer ipos=0;
   integer glnum=0;
   SELECTFFKEY.Run=0;
  int i,k,ok;
  for(i=1;i<argc;i++){
   delete [] fnam;
   fnam = new char[strlen(argv[i])+1];
   cout <<"readio-I-Open I file "<<argv[i]<<endl;
   AMSIO::setfile(argv[i]);
   SELECTFFKEY.Run=0;
   AMSIO::init(1,1);
   do {
    ok=io[ipos++].read();
    if(ok){
     io[ipos-1].setpid(2);
     integer sd0=io[ipos-1].getseed(0);
     integer sd1=io[ipos-1].getseed(1);
     seed[glnum++]=sd0+sd1<<32;
 
    }
   }while(ok && ipos < mp && glnum < mgl);
   if(ipos  > mp -1){
     cerr << " too big input file "<<endl;
     exit(1);
   }
   if(glnum  > mgl-1){
     cerr << " too many input events "<<glnum<<endl;
     cout <<" num "<<glnum <<" seed "<<seed[glnum-1]<<endl;
     io[0].close();   
     delete [] fnam;
     ipos=0;
     break;
   }
   cout <<" num "<<glnum <<" seed "<<seed[glnum-1]<<endl;
   io[0].close();   
   delete [] fnam;
   ipos=0;
  }   
  
  qsort(seed,glnum,sizeof(integer),cmpt);
  integer duplic=0;
  for( int kl=1;kl<glnum;kl++){
    if(seed[kl] == seed[kl-1] && seed[kl]!=0){
      /// cout <<" Two identical seeds found "<<kl<<" "<<seed[kl]<<endl;
      duplic++;
    }
  }
  cout << " Total of "<<glnum<<" events checked."<<endl;
  cout << " Total of "<<duplic<< " duplicated events found."<<endl;
  long * idup=new long[duplic];
  if(idup == 0)cerr << " No memory "<<endl;
  duplic=0;
  for( kl=1;kl<glnum;kl++){
    if(seed[kl] == seed[kl-1] && seed[kl]!=0){
      //   cout <<" Two identical seeds found "<<kl<<" "<<seed[kl]<<endl;
      idup[duplic++]=seed[kl];
    }
  }
  fstream fbin;
  char fnamo[]="out";
  fbin.open(fnamo,ios::out);
  fbin.write((char*)idup,sizeof(long)*duplic);      
  fbin.close();
return 0;
}
int cmpt (const void * p1, const void *p2){
if( *((int*)p1) < *((int*)p2))return -1;
else if( *((int*)p1) == *((int*)p2))return 0;
else return 1;
}
