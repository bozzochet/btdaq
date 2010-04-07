//  $Id: gsen.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "gsen.h"
#include "trid.h"
integer AMSgSen::getsenstrip(AMSPoint & loc){
#ifdef __AMSDEBUG__
  AMSPoint tmp=loc;
#endif
  AMSPoint size=AMSPoint(_par[0],_par[1],_par[2]);
  loc=gl2loc(loc);
  loc=loc+size;
  if(loc[0]>2*size[0]+2*AMSgvolume::dgeant || 
     loc[1]>2*size[1]+2*AMSgvolume::dgeant  ||
     loc[0]<-2*AMSgvolume::dgeant ||
     loc[1]<-2*AMSgvolume::dgeant   ){
       cerr << "AMSgSen::getsenstrip-S-Error loc " 
       << loc << size << endl;
       return 0;
  }
  if(loc[0]<0)loc[0]=0;
  if(loc[1]<0)loc[1]=0;
  if(loc[2]<0)loc[2]=0;
  if(loc[0]>2*size[0])loc[0]=2*size[0];
  if(loc[1]>2*size[1])loc[1]=2*size[1];
  if(loc[2]>2*size[2])loc[2]=2*size[2];
  return 1;
}
 
AMSPoint AMSgSen::str2pnt(number cofgx,number cofgy){
  AMSPoint size=AMSPoint(_par[0],_par[1], _par[2]);
  return loc2gl(AMSPoint(cofgx-size[0],cofgy-size[1],0.));
}   
AMSPoint AMSgSen::tkidfn(const AMSPoint &xv, const AMSPoint &yv, 
                         const AMSDir &nxv,const AMSDir &nyv){
 number s;
 AMSPoint xf,yf;
 s=nxv.prod(yv-xv);
 xf=xv+nxv*s;
 s=nyv.prod(xv-yv);
 yf=yv+nyv*s;
 return (xf+yf)*0.5;
 
}
