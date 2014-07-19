//  $Id: gxint.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include <iostream.h>
#include "cern.h"
#include "gvolume.h"
#include <signal.h>
#include <unistd.h>

const int NWPAW=2000000;
struct PAWC_DEF{
float q[NWPAW];
};
const int NWGEAN=12000000;
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
//
void (handler)(int);
int main(){
     *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
     *signal(SIGTERM, handler);
     *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);
   AMSgvolume::debug=0;
  GPAW(NWGEAN,NWPAW);
return 0;
}

void (handler)(int sig){
  if(sig==SIGFPE)cerr <<" FPE intercepted"<<endl;
  else if (sig==SIGTERM || sig==SIGINT){
    cerr <<" SIGTERM intercepted"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
  }
  else if(sig==SIGQUIT){
    cerr <<" Process suspended"<<endl;
     pause();
  }
  else if(sig==SIGCONT){
    cerr <<" Process resumed"<<endl;
  }
}
