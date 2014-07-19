//  $Id: trid.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include <assert.h>
#include "trid.h"
#include "cern.h"
#include "commons.h"
#include "amsstl.h"
#include <stdio.h>
using trid::ms;
using trid::ncrt;
using trid::ntdr;
#ifndef __ALPHA__ 
using std::ostrstream;
#endif
using namespace trconst;
AMSID AMSTrIdGeom::crgid() const{
         static char name[5];
         static ostrstream ost(name,sizeof(name));
         ost.clear();
         ost.seekp(0);
         ost<<"S"<<getlayer()<<2*getladder()+gethalf()<<ends;
         return AMSID(name,cmpt());
}
void AMSTrIdGeom::_check(){
if(AMSTrIdGeom::debug){
  if(_layer <1 || _layer>TKDBc::nlay())
  cerr <<"AMSTrIdGeom-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_ladder<1 || _ladder>TKDBc::nlad(_layer))
  cerr <<"AMSTrIdGeom-ctor-ladder out of bounds "<<_ladder<<endl; 
  else if(_sensor<1 || _sensor>TKDBc::nsen(_layer,_ladder))
  cerr <<"AMSTrIdGeom-ctor-sensor out of bounds "<<_sensor<<endl; 
  else if(_stripx<0 || _stripx>TKDBc::NStripsSen(_layer,0)-1)
  cerr <<"AMSTrIdGeom-ctor-stripx out of bounds "<<_sensor<<endl; 
  else if(_stripy<0 || _stripy>TKDBc::NStripsSen(_layer,1)-1)
  cerr <<"AMSTrIdGeom-ctor-stripy out of bounds "<<_sensor<<endl; 
}
}
integer AMSTrIdGeom::debug=1;
AMSTrIdGeom::AMSTrIdGeom(integer layer,integer ladder,integer sensor,
                         integer stripx, integer stripy):
_layer(layer),_ladder(ladder),_sensor(sensor),_stripx(stripx),_stripy(stripy){
#ifdef __AMSDEBUG__
_check();
#endif
}
AMSTrIdGeom & AMSTrIdGeom::operator = (const AMSTrIdGeom &o){
  if (this != &o){
 _layer=o._layer;
 _ladder=o._ladder;
 _sensor=o._sensor;
 _stripx=o._stripx;
 _stripy=o._stripy;
   }
  return *this;
}
integer AMSTrIdGeom::operator == (const AMSTrIdGeom &o){
  return (_layer==o._layer && _ladder==o._ladder && _sensor==o._sensor);
}

AMSTrIdGeom::AMSTrIdGeom(integer idsoft, integer stripx, integer stripy){
_layer=idsoft%10;
_ladder=(idsoft/10)%100;
_sensor=(idsoft/1000)%100;
_stripx=stripx;
_stripy=stripy;
#ifdef __AMSDEBUG__
_check();
#endif
}
number AMSTrIdGeom::getsize(integer side) const{
if(side==0)return _swxy[_layer-1][0][_stripx];
else return _swxy[_layer-1][1][_stripy];
}

number AMSTrIdGeom::getcofg(integer side,integer shift, integer readoutch, integer & error) const{
if(side !=0)side=1;
// get right strip
#ifdef __AMSDEBUG__
integer rch=readoutch;
if((readoutch+shift<0 || readoutch+shift>=TKDBc::NStripsDrp(_layer,side)) &&
   (side!=0 || (_layer>0 && _layer<TKDBc::nlay()-1))){
  cerr<<"AMSTridGeom::getcofg-E-ClusterProblem "<<side<<" "<<shift<<" "<<readoutch<<" "<<*this;
}
#endif
 readoutch=(readoutch+shift+TKDBc::NStripsDrp(_layer,side))%TKDBc::NStripsDrp(_layer,side);
if(side==1){
 // ok
}
else{
  integer isen=_sensor
  -(_sensor>TKDBc::nhalf(_layer,_ladder)? TKDBc::nhalf(_layer,_ladder):0);
  integer alpha=(isen*TKDBc::NStripsSen(_layer,side)-readoutch)/TKDBc::NStripsDrp(_layer,side);
  readoutch =(readoutch+alpha*TKDBc::NStripsDrp(_layer,side))%TKDBc::NStripsSen(_layer,side);
}

integer strip = side==0?_R2Gx(readoutch):_R2Gy(readoutch);
if(shift==0 && strip != (side==0?_stripx:_stripy)){
  cerr <<"AMSTrIdGeom::getcofg-E-StripNoVerificationError "<<side<<" "<<strip<<" "<<" "<<readoutch<<" "<<*this;
 error=1;
 return 0;
}
else if((strip -(side==0?_stripx:_stripy))*shift<0 && (side==0?_stripx:_stripy)+shift>0){
#ifdef __AMSDEBUG__
  cerr <<"AMSTrIdGeom::getcofg-W-ClusterGeomError "<<side<<" "<<strip<<" "<<readoutch<<" "<<rch<<" "<<shift<<" "<<*this;
#endif
 error=1;
 return 0;

}
else {
 error=0;
 return  AMSJob::gethead()->isRealData()?
   _swxyRl[_layer-1][side][strip]+_swxyR[_layer-1][side][strip]/2:
  _swxyl[_layer-1][side][strip]+_swxy[_layer-1][side][strip]/2;
}
}


number * AMSTrIdGeom::_swxyl[maxlay][2];
number * AMSTrIdGeom::_swxy[maxlay][2];
number * AMSTrIdGeom::_swxyRl[maxlay][2];
number * AMSTrIdGeom::_swxyR[maxlay][2];
geant AMSTrIdSoft::laser[maxlay][2]={   1.,  1.,
                                         1.,  1.,
                                         2.,  2.,
                                         10., 12.,
                                         70., 90.,
                                         200.,240.,
                                         0.,0.,
                                         0.,0.};                                 
geant * AMSTrIdSoft::gains=0;
geant * AMSTrIdSoft::peds=0;
integer * AMSTrIdSoft::status=0;
geant * AMSTrIdSoft::sigmas=0;
geant * AMSTrIdSoft::sigmaraws=0;
uinteger * AMSTrIdSoft::rhomatrix=0;
geant  AMSTrIdSoft:: cmnnoise[10][trid::ms];
geant * AMSTrIdSoft:: indnoise=0;
integer AMSTrIdSoft::_numel=0;
integer AMSTrIdSoft::_numell=0;
integer AMSTrIdSoft::idsoft2linear[trid::ms];
const integer AMSTrIdSoft::_VAChannels=64;
integer AMSTrIdSoft::getprob (geant r){
integer iad=_strip+AMSbiel(&(indnoise[idsoft2linear[_addr]+_strip]),
_strip>0? r+getindnoise(): r ,getmaxstrips()-_strip );
if(iad<getmaxstrips())_strip=iad;
return iad;
}

void AMSTrIdSoft::_check(){
if(AMSTrIdSoft::debug){
  if(_dead)return;
  if(_layer <1 || _layer>TKDBc::nlay())
  cerr <<"AMSTrIdSoft-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_drp<1 || _drp>TKDBc::nlad(_layer))
  cerr <<"AMSTrIdSoft-ctor-drp out of bounds "<<_drp<<endl; 
  else if(_side<0 || _side>1)
  cerr <<"AMSTrIdSoft-ctor-side out of bounds "<<_side<<endl; 
  else if((_strip<0 ) || _strip>=TKDBc::NStripsDrp(_layer,_side))
  cerr <<"AMSTrIdSoft-ctor-strip out of bounds "<<_strip<<endl; 
  if(_half<0 || _half>1)
  cerr <<"AMSTrIdSoft-ctor-halfladder out of bounds "<<_half<<endl; 
  if(_VANumber <0 || _VANumber >TKDBc::NStripsDrp(_layer,_side)/_VAChannels-1)  
  cerr <<"AMSTrIdSoft-ctor-VANumber out of bounds "<<_VANumber<<endl; 
}
}
integer AMSTrIdSoft::debug=1;

AMSTrIdSoft::AMSTrIdSoft(integer layer,integer drp,integer half, integer side, integer strip):
_layer(layer),_drp(drp),_half(half),_side(side),_strip(strip),_pid(0),_dead(0){
_addr=_layer+10*_drp+1000*_half+2000*_side;
_VANumber=_strip/_VAChannels;
_mkcrate();
_mkhaddr();
#ifdef __AMSDEBUG__
_check();
#endif
}
AMSTrIdSoft & AMSTrIdSoft::operator = (const AMSTrIdSoft &o){
  if (this != &o){
 _dead=o._dead;
 _layer=o._layer;
 _drp=o._drp;
 _half=o._half;
 _side=o._side;
 _strip=o._strip;
 _addr=o._addr;
 _VANumber=o._VANumber;
 _crate=o._crate;
 _haddr=o._haddr;
 _pid=0;
   }
  return *this;
}
integer AMSTrIdSoft::operator == (const AMSTrIdSoft &o){
  return (_layer==o._layer && _drp==o._drp && _strip==
o._strip && _half == o._half && _side==o._side && _dead==o._dead);
}

AMSTrIdSoft::AMSTrIdSoft(integer idsoft){
 _dead=0;
_layer=idsoft%10;
_drp=(idsoft/10)%100;
_half=(idsoft/1000)%2;
_side=((idsoft/1000)%10)/2;
_strip=(idsoft/10000)%1000;
_addr=_layer+10*_drp+1000*_half+2000*_side;
_VANumber=_strip/_VAChannels;
_pid=0;
_mkcrate();
_mkhaddr();
#ifdef __AMSDEBUG__
_check();
#endif
}

AMSTrIdSoft::AMSTrIdSoft(const AMSTrIdGeom& idg, integer side){
 _dead=0;
 _pid=0;
 _layer=idg._layer;
 _drp=  idg._ladder;
 _side=side;
 if(idg._sensor <= TKDBc::nhalf(_layer,_drp))_half=0;
    else _half=1;
_addr=_layer+10*_drp+1000*_half+2000*_side;
if(side==1){
 _strip=idg._stripy;
}
else{
  integer tot=(idg._sensor-1-TKDBc::nhalf(_layer,_drp)*_half)*
    TKDBc::NStripsSen(_layer,0)+idg._stripx;
  _strip=tot%TKDBc::NStripsDrp(_layer,0);
}
_VANumber=_strip/_VAChannels;
_mkcrate();
_mkhaddr();
#ifdef __AMSDEBUG__
_check();
#endif


}

 integer AMSTrIdSoft::_GetGeo[ncrt][ntdr][2][3];
 integer AMSTrIdSoft::_GetHard[maxlay][maxlad][2][3];
AMSTrIdSoft::AMSTrIdSoft(int16u crate, int16u haddr):_crate(crate),
_haddr(haddr),_dead(0),_pid(0){
_VANumber=(_haddr>>6)&15;
_side=_VANumber>9?0:1;
_VANumber=_VANumber%10;
_strip=((_haddr)&63)+_VANumber*64;
uinteger tdrs=gettdr();
#ifdef __AMSDEBUG__
 assert (tdrs < ntdr) ;
#endif
_layer=_GetGeo[_crate][tdrs][_side][0];
if(_layer<0)_dead=1;
_drp=_GetGeo[_crate][tdrs][_side][1];
_half=_GetGeo[_crate][tdrs][_side][2];
_addr=_layer+10*_drp+1000*_half+2000*_side;
#ifdef __AMSDEBUG__
_check();
#endif

}


void AMSTrIdSoft::kill(){
  _GetGeo[_crate][gettdr()][_side][0]=-1;
  _dead=1;
}

AMSTrIdSoft::AMSTrIdSoft(int16u crate, int16u tdrs, int16u side, int16u strip)
:_crate(crate),_dead(0),_strip(strip),_side(side),_pid(0){
#ifdef __AMSDEBUG__
 assert ( tdrs < ntdr) ;
#endif
_layer=_GetGeo[_crate][tdrs][_side][0];
_drp=_GetGeo[_crate][tdrs][_side][1];
_half=_GetGeo[_crate][tdrs][_side][2];
_VANumber=_strip/_VAChannels;
if(_half < 0 || _layer <0){
  // ugly dead condition
     _dead=1;
}
else {
_addr=_layer+10*_drp+1000*_half+2000*_side;
_mkhaddr();
}
#ifdef __AMSDEBUG__
_check();
#endif

}


void AMSTrIdSoft::_mkcrate(){
_crate=_GetHard[_layer-1][_drp-1][_half][2];
}

void AMSTrIdSoft::_mkhaddr(){
integer va= getva();
integer strip=getstripa();
integer tdrs=_GetHard[_layer-1][_drp-1][_half][_side];
 if (tdrs>=0 && tdrs < ntdr)
  _haddr=strip | va<<6 | tdrs << 10 ;
 else _dead=1;
}

void  AMSTrIdGeom::R2Gy(integer stripy){
_stripy=_R2Gy(stripy);
}

void  AMSTrIdGeom::R2Gx(integer stripx){
_stripx=_R2Gx(stripx);
}

integer  AMSTrIdGeom::_R2Gy(integer stripy)const {
   integer __stripy;
   if(AMSJob::gethead()->isRealData()){
      if(stripy==0) __stripy=0;
      else if(stripy== TKDBc::NStripsSen(_layer,1)-1)
        __stripy=stripy*2+4;
      else  __stripy=stripy*2+2;
   }
   else __stripy=stripy;
   return __stripy;
}

integer AMSTrIdGeom::_R2Gx(integer stripx)const {
   integer __stripx;
   if(AMSJob::gethead()->isRealData()){
     if(_layer == 1 || _layer ==TKDBc::nlay()){  // still old bonding scheme!!
       //K7
       if(stripx<64)__stripx=3*stripx;
       else if (stripx<64+97)__stripx=63*3+(stripx-63)*4;
       else __stripx=63*3+97*4+(stripx-64-96)*3;
     }
     else{
       //K5
        __stripx=4*stripx+1;   // new bonding scheme !!!  
       
     }
   }

   else __stripx=stripx;
   return __stripx;
}

void AMSTrIdGeom::R2G(const AMSTrIdSoft &id){
   integer strip = id.getstrip();
   if (id.getside()==0) R2Gx(strip);
   else R2Gy(strip);
}


AMSTrIdGeom * AMSTrIdSoft::ambig(const AMSTrIdSoft &o, integer & namb) {
  static AMSTrIdGeom spid[20];
    integer strip=_strip;
    integer stripx;
    namb=0; 
    _pid=spid;
  if( _side==0 && o._side==1) {
     integer isen=strip/TKDBc::NStripsSen(_layer,_side)+1;
     do {
      (_pid+namb)->_layer=_layer;
      (_pid+namb)->_ladder=_drp;
      (_pid+namb)->_sensor=isen+(_half==0?0:TKDBc::nhalf(_layer,_drp));
      (_pid+namb)->R2Gy(o._strip);
      stripx=strip%TKDBc::NStripsSen(_layer,_side);
      (_pid+namb)->R2Gx(stripx);
      strip=strip+TKDBc::NStripsDrp(_layer,_side);
      isen=strip/TKDBc::NStripsSen(_layer,_side)+1;
      namb++;    
      
#ifdef __AMSDEBUG__
      assert(namb<19);
#endif    
     }while (isen<=(_half==0?
                           TKDBc::nhalf(_layer,_drp):
                           TKDBc::nsen(_layer,_drp)-TKDBc::nhalf(_layer,_drp)));


   
     
  }
  else{
   namb=0;
   _pid=0;
   if(AMSTrIdSoft::debug)cerr <<"AMSTrIdSoft::ambig-S-Side Error "<<
    _side<<" "<<o._side<<endl;
  } 
  
return _pid; 
}


integer AMSTrIdSoft::operator < (const AMSTrIdSoft & o)const {
       if(getlayer() < o.getlayer())return 1;
  else if(o.getlayer() < getlayer())return 0;
  else if(getdrp()< o.getdrp())return 1;
  else if(o.getdrp()< getdrp())return 0;
  else if(getside()< o.getside())return 1;
  else if(o.getside()< getside())return 0;
  else if(gethalf()< o.gethalf())return 1;
  else if(o.gethalf()< gethalf())return 0;
  else if(getstrip()< o.getstrip())return 1;
  else return 0; 
}
void AMSTrIdSoft::init(){
  integer num=0;
  integer nc=0;
  integer idsoft;
  VZERO(idsoft2linear,trid::ms*sizeof(integer)/4);
  int i,k;
  int j;
  for(k=0;k<2;k++){
    for ( i=0;i<TKDBc::nlay();i++){
      for ( j=0;j<TKDBc::nlad(i+1);j++){
	AMSTrIdSoft id(i+1,j+1,k,0,0);
	if(id.dead())continue;
	idsoft=id.getaddr();
	idsoft2linear[idsoft]=num;
	num=num+TKDBc::NStripsDrp(i+1,0);
	nc++;
      }
      for (j=0;j<TKDBc::nlad(i+1);j++){
	AMSTrIdSoft id=AMSTrIdSoft(i+1,j+1,k,1,0);
	if(id.dead())continue;
	idsoft=id.getaddr();
	idsoft2linear[idsoft]=num;
	num=num+TKDBc::NStripsDrp(i+1,1);
	nc++;
      }
    }
    if(k==0)_numell=num;
  }
  _numel=num;
  cout <<
    "AMSTrIdSoft::init-I-Total of " <<num<< " channels  and "<<
    nc<<" drps found."<<endl;
  status=new integer[num];
  for(int l=0;l<num;l++)status[l]=AMSDBc::BAD;    // All are bad
  peds=new geant[num];
  gains=new geant[num];
  sigmas=new geant[num];
  sigmaraws=new geant[num];
  for(i=0;i<num;i++)sigmaraws[i]=1;
  rhomatrix=new uinteger[2*num];
  for(i=0;i<2*num;i++)rhomatrix[i]=~0;     

  for(i=0;i<10;i++){
    for( k=0;k<trid::ms;k++)cmnnoise[i][k]=0;
  }     
  indnoise=new geant[num];
  assert(status && peds && gains && sigmas && sigmaraws && indnoise);


}


void AMSTrIdSoft::inittable(integer setup){



  //     integer AMSTrIdSoft::_GetGeo[ncrt][ntdr][2][3];
  //     integer AMSTrIdSoft::_GetHard[maxlay][maxlad][2][3];     
  int i,j,k;
  for(i=0;i<ncrt;i++){
    for( j=0;j<ntdr;j++){
      for(k=0;k<2;k++){
	for(int l=0;l<3;l++)_GetGeo[i][j][k][l]=-1;
      }
    }
  }
  for(i=0;i<TKDBc::nlay();i++){
    for( j=0;j<maxlad;j++){
      for(k=0;k<2;k++){
	for(int l=0;l<3;l++)_GetHard[i][j][k][l]=-1;
      }
    }
  }

  if(setup==1){

    //     integer AMSTrIdSoft::_GetHard[nl][nld][2][3];     

    // legend :  
    // bits:       0-2      3-4
    // Side x:  conn (0:7)  va(0:3) (in fact 8:11)
    // bits:       0-1      2-4
    // Side y:  conn (0:3)  va(0:7) 

    _ncrates=2;
    _CrateNo[0]=2;
    _CrateNo[1]=5;
    // Layer 1

    _GetHard[0][10][0][2]=0;            //crate
    _GetHard[0][10][0][0]=7 | 3<<3;     //side x
    _GetHard[0][10][0][1]=3 | 6<<2;     //side y

    _GetHard[0][9][0][2]=0;            //crate
    _GetHard[0][9][0][0]=6 | 3<<3;     //side x
    _GetHard[0][9][0][1]=2 | 6<<2;     //side y

    _GetHard[0][8][0][2]=0;            //crate
    _GetHard[0][8][0][0]=5 | 3<<3;     //side x
    _GetHard[0][8][0][1]=1 | 6<<2;     //side y

    _GetHard[0][7][0][2]=0;            //crate
    _GetHard[0][7][0][0]=4 | 3<<3;     //side x
    _GetHard[0][7][0][1]=0 | 6<<2;     //side y


    _GetHard[0][10][1][2]=1;            //crate
    _GetHard[0][10][1][0]=4 | 3<<3;     //side x
    _GetHard[0][10][1][1]=0 | 6<<2;     //side y


    _GetHard[0][9][1][2]=1;            //crate
    _GetHard[0][9][1][0]=5 | 3<<3;     //side x
    _GetHard[0][9][1][1]=1 | 6<<2;     //side y


    _GetHard[0][8][1][2]=1;            //crate
    _GetHard[0][8][1][0]=6 | 3<<3;     //side x
    _GetHard[0][8][1][1]=2 | 6<<2;     //side y


    _GetHard[0][7][1][2]=1;            //crate
    _GetHard[0][7][1][0]=7 | 3<<3;     //side x
    _GetHard[0][7][1][1]=3 | 6<<2;     //side y

    // Layer 2


    _GetHard[1][4][0][2]=0;            //crate
    _GetHard[1][4][0][0]=1 | 2<<3;     //side x
    _GetHard[1][4][0][1]=1 | 5<<2;     //side y


    _GetHard[1][5][0][2]=0;            //crate
    _GetHard[1][5][0][0]=3 | 2<<3;     //side x
    _GetHard[1][5][0][1]=3 | 5<<2;     //side y


    _GetHard[1][6][0][2]=0;            //crate
    _GetHard[1][6][0][0]=5 | 2<<3;     //side x
    _GetHard[1][6][0][1]=1 | 4<<2;     //side y


    _GetHard[1][7][0][2]=0;            //crate
    _GetHard[1][7][0][0]=7 | 2<<3;     //side x
    _GetHard[1][7][0][1]=3 | 4<<2;     //side y


    _GetHard[1][8][0][2]=0;            //crate
    _GetHard[1][8][0][0]=1 | 3<<3;     //side x
    _GetHard[1][8][0][1]=1 | 7<<2;     //side y


    _GetHard[1][9][0][2]=0;            //crate
    _GetHard[1][9][0][0]=3 | 3<<3;     //side x
    _GetHard[1][9][0][1]=3 | 7<<2;     //side y

    _GetHard[1][4][1][2]=1;            //crate
    _GetHard[1][4][1][0]=3 | 3<<3;     //side x
    _GetHard[1][4][1][1]=3 | 7<<2;     //side y

    _GetHard[1][5][1][2]=1;            //crate
    _GetHard[1][5][1][0]=1 | 3<<3;     //side x
    _GetHard[1][5][1][1]=1 | 7<<2;     //side y

    _GetHard[1][6][1][2]=1;            //crate
    _GetHard[1][6][1][0]=7 | 2<<3;     //side x
    _GetHard[1][6][1][1]=3 | 4<<2;     //side y

    _GetHard[1][7][1][2]=1;            //crate
    _GetHard[1][7][1][0]=5 | 2<<3;     //side x
    _GetHard[1][7][1][1]=1 | 4<<2;     //side y

    _GetHard[1][8][1][2]=1;            //crate
    _GetHard[1][8][1][0]=3 | 2<<3;     //side x
    _GetHard[1][8][1][1]=3 | 5<<2;     //side y

    _GetHard[1][9][1][2]=1;            //crate
    _GetHard[1][9][1][0]=1 | 2<<3;     //side x
    _GetHard[1][9][1][1]=1 | 5<<2;     //side y

    // layer 3

    _GetHard[2][5][0][2]=0;            //crate
    _GetHard[2][5][0][0]=2 | 2<<3;     //side x
    _GetHard[2][5][0][1]=2 | 5<<2;     //side y

    _GetHard[2][6][0][2]=0;            //crate
    _GetHard[2][6][0][0]=4 | 2<<3;     //side x
    _GetHard[2][6][0][1]=0 | 4<<2;     //side y

    _GetHard[2][7][0][2]=0;            //crate
    _GetHard[2][7][0][0]=6 | 2<<3;     //side x
    _GetHard[2][7][0][1]=2 | 4<<2;     //side y

    _GetHard[2][8][0][2]=0;            //crate
    _GetHard[2][8][0][0]=0 | 3<<3;     //side x
    _GetHard[2][8][0][1]=0 | 7<<2;     //side y

    _GetHard[2][5][1][2]=1;            //crate
    _GetHard[2][5][1][0]=0 | 3<<3;     //side x
    _GetHard[2][5][1][1]=0 | 7<<2;     //side y

    _GetHard[2][6][1][2]=1;            //crate
    _GetHard[2][6][1][0]=6 | 2<<3;     //side x
    _GetHard[2][6][1][1]=2 | 4<<2;     //side y

    _GetHard[2][7][1][2]=1;            //crate
    _GetHard[2][7][1][0]=4 | 2<<3;     //side x
    _GetHard[2][7][1][1]=0 | 4<<2;     //side y

    _GetHard[2][8][1][2]=1;            //crate
    _GetHard[2][8][1][0]=2 | 2<<3;     //side x
    _GetHard[2][8][1][1]=2 | 5<<2;     //side y

    _GetHard[2][9][1][2]=1;            //crate
    _GetHard[2][9][1][0]=0 | 2<<3;     //side x
    _GetHard[2][9][1][1]=0 | 5<<2;     //side y

    // layer 4


    _GetHard[3][8][0][2]=0;            //crate
    _GetHard[3][8][0][0]=5 | 1<<3;     //side x
    _GetHard[3][8][0][1]=1 | 2<<2;     //side y

    _GetHard[3][7][0][2]=0;            //crate
    _GetHard[3][7][0][0]=3 | 1<<3;     //side x
    _GetHard[3][7][0][1]=3 | 3<<2;     //side y

    _GetHard[3][6][0][2]=0;            //crate
    _GetHard[3][6][0][0]=1 | 1<<3;     //side x
    _GetHard[3][6][0][1]=1 | 3<<2;     //side y

    _GetHard[3][5][0][2]=0;            //crate
    _GetHard[3][5][0][0]=7 | 0<<3;     //side x
    _GetHard[3][5][0][1]=3 | 0<<2;     //side y

    _GetHard[3][9][1][2]=1;            //crate
    _GetHard[3][9][1][0]=5 | 0<<3;     //side x
    _GetHard[3][9][1][1]=1 | 0<<2;     //side y

    _GetHard[3][8][1][2]=1;            //crate
    _GetHard[3][8][1][0]=7 | 0<<3;     //side x
    _GetHard[3][8][1][1]=3 | 0<<2;     //side y

    _GetHard[3][7][1][2]=1;            //crate
    _GetHard[3][7][1][0]=1 | 1<<3;     //side x
    _GetHard[3][7][1][1]=1 | 3<<2;     //side y

    _GetHard[3][6][1][2]=1;            //crate
    _GetHard[3][6][1][0]=3 | 1<<3;     //side x
    _GetHard[3][6][1][1]=3 | 3<<2;     //side y

    _GetHard[3][5][1][2]=1;            //crate
    _GetHard[3][5][1][0]=5 | 1<<3;     //side x
    _GetHard[3][5][1][1]=1 | 2<<2;     //side y

    _GetHard[3][4][1][2]=1;            //crate
    _GetHard[3][4][1][0]=7 | 1<<3;     //side x
    _GetHard[3][4][1][1]=3 | 2<<2;     //side y

    // layer 5

    _GetHard[4][9][0][2]=0;            //crate
    _GetHard[4][9][0][0]=6 | 1<<3;     //side x
    _GetHard[4][9][0][1]=2 | 2<<2;     //side y

    _GetHard[4][8][0][2]=0;            //crate
    _GetHard[4][8][0][0]=4 | 1<<3;     //side x
    _GetHard[4][8][0][1]=0 | 2<<2;     //side y

    _GetHard[4][7][0][2]=0;            //crate
    _GetHard[4][7][0][0]=2 | 1<<3;     //side x
    _GetHard[4][7][0][1]=2 | 3<<2;     //side y

    _GetHard[4][6][0][2]=0;            //crate
    _GetHard[4][6][0][0]=0 | 1<<3;     //side x
    _GetHard[4][6][0][1]=0 | 3<<2;     //side y

    _GetHard[4][5][0][2]=0;            //crate
    _GetHard[4][5][0][0]=6 | 0<<3;     //side x
    _GetHard[4][5][0][1]=2 | 0<<2;     //side y

    _GetHard[4][4][0][2]=0;            //crate
    _GetHard[4][4][0][0]=4 | 0<<3;     //side x
    _GetHard[4][4][0][1]=0 | 0<<2;     //side y

    _GetHard[4][9][1][2]=1;            //crate
    _GetHard[4][9][1][0]=4 | 0<<3;     //side x
    _GetHard[4][9][1][1]=0 | 0<<2;     //side y

    _GetHard[4][8][1][2]=1;            //crate
    _GetHard[4][8][1][0]=6 | 0<<3;     //side x
    _GetHard[4][8][1][1]=2 | 0<<2;     //side y

    _GetHard[4][7][1][2]=1;            //crate
    _GetHard[4][7][1][0]=0 | 1<<3;     //side x
    _GetHard[4][7][1][1]=0 | 3<<2;     //side y

    _GetHard[4][6][1][2]=1;            //crate
    _GetHard[4][6][1][0]=2 | 1<<3;     //side x
    _GetHard[4][6][1][1]=2 | 3<<2;     //side y

    _GetHard[4][5][1][2]=1;            //crate
    _GetHard[4][5][1][0]=4 | 1<<3;     //side x
    _GetHard[4][5][1][1]=0 | 2<<2;     //side y

    _GetHard[4][4][1][2]=1;            //crate
    _GetHard[4][4][1][0]=6 | 1<<3;     //side x
    _GetHard[4][4][1][1]=2 | 2<<2;     //side y

    // layer 6

    _GetHard[5][7][0][2]=0;            //crate
    _GetHard[5][7][0][0]=1 | 0<<3;     //side x
    _GetHard[5][7][0][1]=1 | 1<<2;     //side y

    _GetHard[5][8][0][2]=0;            //crate
    _GetHard[5][8][0][0]=2 | 0<<3;     //side x
    _GetHard[5][8][0][1]=2 | 1<<2;     //side y

    _GetHard[5][9][0][2]=0;            //crate
    _GetHard[5][9][0][0]=3 | 0<<3;     //side x
    _GetHard[5][9][0][1]=3 | 1<<2;     //side y

    _GetHard[5][7][1][2]=1;            //crate
    _GetHard[5][7][1][0]=2 | 0<<3;     //side x
    _GetHard[5][7][1][1]=2 | 1<<2;     //side y

    _GetHard[5][8][1][2]=1;            //crate
    _GetHard[5][8][1][0]=1 | 0<<3;     //side x
    _GetHard[5][8][1][1]=1 | 1<<2;     //side y

    _GetHard[5][9][1][2]=1;            //crate
    _GetHard[5][9][1][0]=0 | 0<<3;     //side x
    _GetHard[5][9][1][1]=0 | 1<<2;     //side y

    int l,i1,j1,k1,l1;
    for(k=0;k<2;k++){
      for(l=0;l<2;l++){
	int tot=0;
	for(i=0;i<TKDBc::nlay();i++){
	  for(j=0;j<maxlad;j++){
	    int a=_GetHard[i][j][k][l];
	    if(a>=0){
	      if(l==0){
		//side x
		int conn=a&7;
		switch (conn){
		  case 0:
		    conn=2;
		    break;
		  case 1:
		    conn=3;
		    break;
		  case 2:
		    conn=0;
		    break;
		  case 3:
		    conn=1;
		    break;
		  case 4:
		    conn=6;
		    break;
		  case 5:
		    conn=7;
		    break;
		  case 6:
		    conn=4;
		    break;
		  case 7:
		    conn=5;
		    break;
		}
		a=a&(~7);
		_GetHard[i][j][k][l]= a | conn;
	      }
	      else {
		//side y
		int conn=a&3;
		switch (conn){
		  case 0:
		    conn=1;
		    break;
		  case 1:
		    conn=0;
		    break;
		  case 2:
		    conn=3;
		    break;
		  case 3:
		    conn=2;
		    break;
		}
		a=a&(~3);
		_GetHard[i][j][k][l]= a | conn;
	      }

	    }

	  }
	}
      }
    }

    _TPed[0]=new char[strlen("TrackerPedestals.l")+1];
    strcpy(_TPed[0],"TrackerPedestals.l");
    _TPed[1]=new char[strlen("TrackerPedestals.r")+1];
    strcpy(_TPed[1],"TrackerPedestals.r");

    _TSig[0]=new char[strlen("TrackerSigmas.l")+1];
    strcpy(_TSig[0],"TrackerSigmas.l");
    _TSig[1]=new char[strlen("TrackerSigmas.r")+1];
    strcpy(_TSig[1],"TrackerSigmas.r");

    _TGa[0]=new char[strlen("TrackerGains.l")+1];
    strcpy(_TGa[0],"TrackerGains.l");
    _TGa[1]=new char[strlen("TrackerGains.r")+1];
    strcpy(_TGa[1],"TrackerGains.r");

    _TRSig[0]=new char[strlen("TrackerRawSigmas.l")+1];
    strcpy(_TRSig[0],"TrackerRawSigmas.l");
    _TRSig[1]=new char[strlen("TrackerRawSigmas.r")+1];
    strcpy(_TRSig[1],"TrackerRawSigmas.r");

    _TSt[0]=new char[strlen("TrackerStatus.l")+1];
    strcpy(_TSt[0],"TrackerStatus.l");
    _TSt[1]=new char[strlen("TrackerStatus.r")+1];
    strcpy(_TSt[1],"TrackerStatus.r");

    _TCm=new char[strlen("TrackerCmnNoise")+1];
    strcpy(_TCm,"TrackerCmnNoise");

  }
  else if(setup==2){


    _TPed[0]=new char[strlen("TrackerPedestals.2l")+1];
    strcpy(_TPed[0],"TrackerPedestals.2l");
    _TPed[1]=new char[strlen("TrackerPedestals.2r")+1];
    strcpy(_TPed[1],"TrackerPedestals.2r");

    _TSig[0]=new char[strlen("TrackerSigmas.2l")+1];
    strcpy(_TSig[0],"TrackerSigmas.2l");
    _TSig[1]=new char[strlen("TrackerSigmas.2r")+1];
    strcpy(_TSig[1],"TrackerSigmas.2r");

    _TGa[0]=new char[strlen("TrackerGains.2l")+1];
    strcpy(_TGa[0],"TrackerGains.2l");
    _TGa[1]=new char[strlen("TrackerGains.2r")+1];
    strcpy(_TGa[1],"TrackerGains.2r");

    _TRSig[0]=new char[strlen("TrackerRawSigmas.2l")+1];
    strcpy(_TRSig[0],"TrackerRawSigmas.2l");
    _TRSig[1]=new char[strlen("TrackerRawSigmas.2r")+1];
    strcpy(_TRSig[1],"TrackerRawSigmas.2r");

    _TSt[0]=new char[strlen("TrackerStatus.2l")+1];
    strcpy(_TSt[0],"TrackerStatus.2l");
    _TSt[1]=new char[strlen("TrackerStatus.2r")+1];
    strcpy(_TSt[1],"TrackerStatus.2r");

    _TCm=new char[strlen("TrackerCmnNoise2")+1];
    strcpy(_TCm,"TrackerCmnNoise2");


    _ncrates=8;
    const int ntdr24=24;
    // Dummy setup
    int s;
    for(s=0;s<_ncrates;s++)_CrateNo[s]=s;
    int count=0;
    // crate contains 24 drp
    for(s=0;s<2;s++){
      for(i=0;i<TKDBc::nlay();i++){
	for(j=0;j<TKDBc::nlad(i+1);j++){
	  if(TKDBc::activeladdshuttle(i+1,j+1,s)){

	    if(count/ntdr24 >= _ncrates){
	      cerr<<"AMSTrIdSoft::inttab-F-Wrong crates no "<<count<<endl;
	      exit(1);
	    }
	    _GetHard[i][j][s][2]=count/ntdr24;            //crate
	    _GetHard[i][j][s][0]=count%ntdr24;     //side x
	    _GetHard[i][j][s][1]=count%ntdr24;     //side y
	    count++;
	  }
	}
      }
    }
    cout <<" max crate number "<<(count-1)/ntdr24<<endl;
  }




  int l,i1,j1,k1,l1;
#ifdef __AMSDEBUG__
  // perform duplicate check
  for(l=0;l<2;l++){
    int tot=0;
    for(k=0;k<2;k++){
      for(i=0;i<TKDBc::nlay();i++){
	for(j=0;j<maxlad;j++){
	  int a=_GetHard[i][j][k][l];
	  int cr=_GetHard[i][j][k][2];
	  if(a>=0){
	    tot++;
	    for(k1=0;k1<2;k1++){
	      for(i1=0;i1<TKDBc::nlay();i1++){
		for(j1=0;j1<maxlad;j1++){
		  if(j1==j && i1==i || (cr!=_GetHard[i1][j1][k1][2]))continue;
		  if(a == _GetHard[i1][j1][k1][l]){
		    cerr <<"AMSTrIdSoft::inittable-F-duplicate entry found "<<
		      a<<" i "<<i<<" j "<<j<<" k "<<k<<" l "<<l<<" i1 "<<i1<<
		      " j1 "<<j1<<endl;
		    exit(1);
		  }
		}
	      }            
	    }
	  }

	}
      }
      cout <<" k "<<k<<" l "<<l<<" total "<<tot<<endl; 
    }
  }
#endif


  //     integer AMSTrIdSoft::_GetGeo[ncrt][ntdr][2][3];
  for(k=0;k<2;k++){            
    for(i=0;i<ncrt;i++){
      for(j=0;j<ntdr;j++){
	for(i1=0;i1<TKDBc::nlay();i1++){
	  for(j1=0;j1<maxlad;j1++){
	    for(k1=0;k1<2;k1++){
	      if(_GetHard[i1][j1][k1][2]==i && _GetHard[i1][j1][k1][k]==j){
		_GetGeo[i][j][k][0]=i1+1;
		_GetGeo[i][j][k][1]=j1+1;
		_GetGeo[i][j][k][2]=k1;
	      }
	    }

	  }
	}      
      }
    }
  }

}


void AMSTrIdGeom::init(){
  integer i,j,k;
  for (i=0;i<TKDBc::nlay();i++){
    for(j=0;j<2;j++){
      _swxy[i][j]=new number[TKDBc::NStripsSen(i+1,j)];
      if(j==0){
	if(i==0 || i==TKDBc::nlay()-1){
	  number st=0.0052;
	  _swxy[i][j][0]=2*st;
	  for(k=1;k<63;k++)_swxy[i][j][k]=3*st;
	  _swxy[i][j][63]=3.5*st;
	  for(k=64;k<64+96;k++)_swxy[i][j][k]=4*st;
	  _swxy[i][j][160]=3.5*st;
	  for(k=161;k<161+62;k++)_swxy[i][j][k]=3*st;
	  _swxy[i][j][223]=2*st;
	  assert(TKDBc::NStripsSen(i+1,j)-1  == 223);
	}
	else{
	  for(k=1;k<TKDBc::NStripsSen(i+1,j)-2;k++)_swxy[i][j][k]=0.0208;
	  _swxy[i][j][0]=0.0052*2.5;
	  _swxy[i][j][TKDBc::NStripsSen(i+1,j)-2]=0.0052*5;
	  _swxy[i][j][TKDBc::NStripsSen(i+1,j)-1]=0.0052*3.5;
	}
      }
      else{
	for(k=2;k< TKDBc::NStripsSen(i+1,j)-2;k++)_swxy[i][j][k]=0.011;
	_swxy[i][j][0]=2.5*0.0055; 
	_swxy[i][j][1]=3*0.0055; 
	_swxy[i][j][TKDBc::NStripsSen(i+1,j)-2]=3*0.0055; 
	_swxy[i][j][TKDBc::NStripsSen(i+1,j)-1]=3.5*0.0055; 
      }
    }
  }


  for (i=0;i<TKDBc::nlay();i++){
    for(j=0;j<2;j++){
      _swxyR[i][j]=new number[TKDBc::NStripsSenR(i+1,j)];
      if(j==0){
	for(k=0;k<TKDBc::NStripsSenR(i+1,j);k++)_swxyR[i][j][k]=0.0052;
      }
      else{
	for(k=0;k< TKDBc::NStripsSenR(i+1,j);k++)_swxyR[i][j][k]=0.0055;

      }
    }
  }




  for(i=0;i<TKDBc::nlay();i++){
    for(j=0;j<2;j++){
      _swxyl[i][j]=new number[TKDBc::NStripsSen(i+1,j)+1];
      _swxyl[i][j][0]=0;
      for(k=1;k<TKDBc::NStripsSen(i+1,j)+1;k++){
	_swxyl[i][j][k]=_swxyl[i][j][k-1]+_swxy[i][j][k-1];
      }
      if(fabs(_swxyl[i][j][TKDBc::NStripsSen(i+1,j)] -
	    TKDBc::ssize_active(i,j)) > 1.e-4){
	cerr <<"AMSTrIdGeom::init-F-SizeDoesNotMatch "<<i<<" "<<j<<" "<<
	  _swxyl[i][j][TKDBc::NStripsSen(i+1,j)]<<" "<<
	  TKDBc::ssize_active(i,j)<<endl;
	exit(1);
      }
    }
  }
  for(i=0;i<TKDBc::nlay();i++){
    for(j=0;j<2;j++){
      _swxyRl[i][j]=new number[TKDBc::NStripsSenR(i+1,j)+1];
      _swxyRl[i][j][0]=0;
      for(k=1;k<TKDBc::NStripsSenR(i+1,j)+1;k++){
	_swxyRl[i][j][k]=_swxyRl[i][j][k-1]+_swxyR[i][j][k-1];
      }
      if(fabs(_swxyRl[i][j][TKDBc::NStripsSenR(i+1,j)] -
	    TKDBc::ssize_active(i,j)) > 1.e-4){
	cerr <<"AMSTrIdGeom::init-F-SizeDoesNotMatch (R) "<<i<<" "<<j<<" "<<
	  _swxyRl[i][j][TKDBc::NStripsSenR(i+1,j)]<<" "<<
	  TKDBc::ssize_active(i,j)<<endl;
	exit(1);
      }
    }
  }

}

integer AMSTrIdGeom::size2strip(integer side, number size){
#ifdef __AMSDEBUG__
  assert(side>=0 && side<2 &&  !AMSJob::gethead()->isRealData());
#endif
  integer strip;
  strip= AMSbiel( _swxyl[_layer-1][side],  size,  
      TKDBc::NStripsSen(_layer,side)+1)-1;
#ifdef __AMSDEBUG__  
  if(strip <0 || strip >= TKDBc::NStripsSen(_layer,side))
    cerr<<"AMSTrIdGeom::size2strip-E-InvalidStrip "<<strip<<" "<<
      size<<endl;
#endif       
  if(strip<0)strip=0;
  if(strip>=TKDBc::NStripsSen(_layer,side))
    strip=TKDBc::NStripsSen(_layer,side)-1;
  return strip;
}


AMSTrIdSoftI::AMSTrIdSoftI(){
  if(_Count++==0){
    //       AMSTrIdSoft::init();
  }
}
integer AMSTrIdSoftI::_Count=0;




integer  AMSTrIdSoft::getrhomatrix(integer bit){
  integer word=bit/32;
  bit=bit%32;
  return ((rhomatrix[2*(idsoft2linear[_addr]+_strip)+word] & (1<<bit)) !=0)?1:0;

}


void AMSTrIdSoft::setrhomatrix(integer bit){


  integer word=bit/32;
  bit=bit%32;
  rhomatrix[2*(idsoft2linear[_addr]+_strip)+word]=rhomatrix[2*(idsoft2linear[_addr]+_strip)+word] | (1<<bit);

}



void AMSTrIdSoft::clearrhomatrix(integer bit){


  integer word=bit/32;
  bit=bit%32;
  rhomatrix[2*(idsoft2linear[_addr]+_strip)+word]=rhomatrix[2*(idsoft2linear[_addr]+_strip)+word] & ~(1<<bit);

}

integer AMSTrIdGeom::FindAtt(const AMSPoint & pnt,  AMSPoint  size){
  // Assuming known layer, Find ladder & sensor and return 1;
  // or return 0;
  AMSPoint p2=pnt;
  AMSgvolume *p =AMSJob::gethead()->getgeomvolume(crgid());
  if(p){  
    AMSPoint loc=p->gl2loc(pnt);
    geant dl=floor(0.5+loc[1]/TKDBc::c2c(_layer-1));
    _ladder+=-dl;
    if(_ladder<=0)_ladder=1;
    if(_ladder>TKDBc::nlad(_layer))_ladder=TKDBc::nlad(_layer);
    if(p2[0]<0){
      _sensor=5;
    }
    else{
      _sensor= TKDBc::nsen(_layer,_ladder)-5;
    }
    p=  AMSJob::gethead()->getgeomvolume(crgid());
    if(p){
      loc=p->gl2loc(pnt);
      geant dl=floor(0.5+loc[0]/TKDBc::ssize_inactive(_layer-1,0));
      _sensor+=dl;
      p=  AMSJob::gethead()->getgeomvolume(crgid());
      if(p){
	AMSPoint loc=p->gl2loc(pnt);
	loc=loc+size;
	if( 
	    loc[1]>2*size[1]+0.1  ||
	    loc[1]<-0.1   ){
#ifdef __AMSDEBUG__
	  cerr << "AMSgSen::FindAtt-S-Error loc " 
	    << loc << size << endl;
#endif
	  return 0;
	}
	else return 1;   
      }
    }
  }
  return 0;
}

uinteger AMSTrIdSoft::_ncrates=0;
uinteger AMSTrIdSoft::_CrateNo[ncrt];


char *   AMSTrIdSoft::_TSig[2]={0,0};
char *   AMSTrIdSoft::_TRSig[2]={0,0};
char *   AMSTrIdSoft::_TPed[2]={0,0};
char *   AMSTrIdSoft::_TGa[2]={0,0};
char *   AMSTrIdSoft::_TSt[2]={0,0};
char *   AMSTrIdSoft::_TRM[2]={0,0};
char *   AMSTrIdSoft::_TCm=0;

integer AMSTrIdSoft::match(AMSTrIdGeom *pid){
  if( gethalf()==pid->gethalf() && getdrp() == pid->getladder() && getlayer() ==pid->getlayer())return 1;
  else{
    cerr <<"AMSTrIdsoft::match-E-SoftGeoMismatch "<<*pid<<" "<<*this<<endl;
    return 0;

  }
}
