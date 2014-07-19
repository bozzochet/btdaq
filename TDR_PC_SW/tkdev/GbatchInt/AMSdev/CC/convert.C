//  $Id: convert.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <iostream.h>
#include <stdio.h>
#include "cern.h"
PROTOCCALLSFSUB1(CONVERT,convert,STRING)
#define CONVERT(A1) CCALLSFSUB1(CONVERT,convert,STRING,A1)
PROTOCCALLSFSUB1(PREPARE,prepare,STRING)
#define PREPARE(A1) CCALLSFSUB1(PREPARE,prepare,STRING,A1)
const int NWPAW=18000000;
struct PAWC_DEF{
float q[NWPAW];
};
//
#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;


int main(int argc, char* argv[]){ 
  switch (argc) {
  case 1:
    cerr <<"convert-F-Please give an input file name as first parameter "<<endl;
            return 1;
  case 2:
    cerr <<"convert-F-Please give an output file name as last parameter "<<endl;
            return 1;
  default:
   break;
  }
  HLIMIT(NWPAW);
  int i,k,ok;
   for(i=1;i<argc-1;i++){
     cout <<"convert-I-Open I file "<<argv[i]<<endl;
               PREPARE(argv[i]); 
   }
   //     PREPARE("../datacards/mc.w.hbk"); 
  cout <<"convert-I-Open O file "<<argv[argc-1]<<endl;
  CONVERT(argv[argc-1]);
  //CONVERT("/dev/null");
return 0;
}
