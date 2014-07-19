//  $Id: hcdir.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <cern.h>
#include <iostream.h>
#include <point.h>
const int NWPAWC=100000;
struct PAWC_DEF{
float q[NWPAWC];
};
//
#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
main(){
HLIMIT(NWPAWC);
    integer rsize=1024;
    integer hlun=20;
    integer iostat;
    char hfile[160]="hist.hbk                 ";
    HROPEN(hlun,"output",hfile,"N",rsize,iostat);
    if(iostat){
     cerr << "Error opening Histo file "<<endl;
     exit(1);
    }
    else cout <<"Histo file opened."<<endl;
    HBOOK1(12," ww",200,0.,100.,0.);
    HBOOK1(13," ww",1000,0.,100.,0.);
    HBOOK1(14," ww",1000,0.,100.,0.);
    HBOOK1(15," ww",1000,0.,100.,0.);
    HBOOK1(16," ww",1000,0.,100.,0.);
    HBNT(1,"Simulation"," ");
     HF1(12,33.,1.);
static integer init=0;
static integer *pointer=0;
if(init++==0){
  // get memory
  pointer =new integer[100];
  assert(pointer!=NULL);
  //book the ntuple block
  HBNAME(1,"MCEventG",pointer,
  "EventNoMCEventG:I*4, Particle:I*4,  Coo(3):R*8, Dir(3):R*8, Momentum:R*8, Mass:R*8, Charge:R*8");

}
  integer event=15;
  integer _ipart=15;
  AMSPoint _coo(-11,-10,-9);
  AMSPoint _dir(-1,0,1);
  number mrm[3]={13.,0.968,1.};
  UCOPY(&event,pointer,sizeof(integer)/4);
  UCOPY(&_ipart,pointer+1,sizeof(integer)/4);
  UCOPY(&_coo,pointer+2,3*sizeof(number)/4);
  UCOPY(&_dir,pointer+8,3*sizeof(number)/4);
  UCOPY(mrm,pointer+14,3*sizeof(number)/4);
  HFNTB(1,"MCEventG");

    char hh[10]="//PAWC";
    HCDIR(hh," ");
char ho[10]="//output";
HCDIR(ho," ");
        integer ICYCL=0;
        HROUT (0, ICYCL, " ");
        HREND ("output");
return 0;
}
