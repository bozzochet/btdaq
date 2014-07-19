//  $Id: amsdbc.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include <math.h>
#include "amsdbc.h"
#include "amsstl.h"
//

char* AMSDBc::amsdatabase=0;
const uinteger AMSDBc::REFITTED=1;
const uinteger AMSDBc::WIDE=2;
const uinteger AMSDBc::AMBIG=4;
const uinteger AMSDBc::RELEASED=8;
const uinteger AMSDBc::BAD=16;
const uinteger AMSDBc::USED=32;
const uinteger AMSDBc::DELETED=64;
const uinteger AMSDBc::BADHIS=128;
const uinteger AMSDBc::ONESIDE=256;
const uinteger AMSDBc::BADTIME=512;
const uinteger AMSDBc::NEAR=1024;
const uinteger AMSDBc::WEAK=2048;
const uinteger AMSDBc::AwayTOF=4096;
const uinteger AMSDBc::FalseX=8192;
const uinteger AMSDBc::FalseTOFX=16384;
const uinteger AMSDBc::RECOVERED=16384*2;
const uinteger AMSDBc::LocalDB=16384*2*2;
const uinteger AMSDBc::GlobalDB=16384*2*2*2;
const uinteger AMSDBc::CHARGEUSED=16384*2*2*2*2;
const uinteger AMSDBc::GOOD      =16384*2*2*2*2*2;
const uinteger AMSDBc::BADINTERPOL =16384*2*2*2*2*2*2;
const uinteger AMSDBc::NOTRACK     =16384*2*2*2*2*2*2*2;
const uinteger AMSDBc::AOVERFLOW    =16384*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::TRDTRACK     =16384*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::LEAK         =16384*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::CATLEAK      =16384*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::JUNK         =16384*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::ECALTRACK    =16384*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::LOWGCHUSED   =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::S_AMBIG      =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::GOLDEN       =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::TOFFORGAMMA  =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2U;

integer AMSDBc::debug=1;
integer AMSDBc::BigEndian=0;
const number AMSDBc::raddeg=57.295779;
const number AMSDBc::pi=3.14159265358979;
const number AMSDBc::twopi=2*pi;
geant AMSDBc::ams_size[3]={390.,390.,390.};
const char AMSDBc::ams_name[]="AMSG";
geant AMSDBc::ams_coo[3]={0.,0.,0.};
const integer AMSDBc::ams_rotmno=1;
number AMSDBc::ams_nrm[3][3];
const geant AMSDBc::amsdext=43.;

void AMSDBc::init(){
 number angle=0;
 ams_nrm[0][0]=cos(angle);
 ams_nrm[1][0]=0;
 ams_nrm[2][0]=-sin(angle);
 ams_nrm[0][1]=0;
 ams_nrm[1][1]=1;
 ams_nrm[2][1]=0;
 ams_nrm[0][2]=sin(angle);
 ams_nrm[1][2]=0;
 ams_nrm[2][2]=cos(angle);

}
