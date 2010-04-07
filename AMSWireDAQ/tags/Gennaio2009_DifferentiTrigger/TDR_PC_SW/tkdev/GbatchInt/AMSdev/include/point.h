//  $Id: point.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSPOINT__
#define __AMSPOINT__
#include "typedefs.h"
#include <iostream.h>
#include <math.h>
class AMSPoint {
protected:
number _x;
number _y;
number _z;
public:
AMSPoint():_x(0),_y(0),_z(0){};
AMSPoint(number x, number y,number z):_x(x),_y(y),_z(z){};
AMSPoint(number coo[3]): _x(coo[0]),_y(coo[1]),_z(coo[2]){};
AMSPoint(geant coo[3]): _x(coo[0]),_y(coo[1]),_z(coo[2]){};
void setp(number x,number y,number z){_x=x;_y=y;_z=z;}
void setp(number x[]){_x=x[0];_y=x[1];_z=x[2];}
void getp(number &x, number &y, number &z)const{x=_x;y=_y;z=_z;}
void getp(geant &x, geant &y, geant &z){x=_x;y=_y;z=_z;}
void getp(number x[]){x[0]=_x;x[1]=_y;x[2]=_z;}
AMSPoint mm3(number [][3]);
AMSPoint mm3i(number [][3]);
integer operator ==(const AMSPoint & o) const{return _x==o._x && _y==o._y &&_z==o._z;}
integer operator <(AMSPoint o) const{return _x<o._x && _y<o._y &&_z<o._z;}
integer operator <=(AMSPoint o) const{return _x<=o._x && _y<=o._y &&_z<=o._z;}
integer operator >=(AMSPoint o) const{return _x>=o._x && _y>=o._y &&_z>=o._z;}
AMSPoint abs(){return AMSPoint(fabs(_x),fabs(_y),fabs(_z));}
AMSPoint  operator+( AMSPoint o)
          const{return AMSPoint(_x+o._x,_y+o._y,_z+o._z);}
AMSPoint  operator-( AMSPoint o)const
          {return AMSPoint(_x-o._x,_y-o._y,_z-o._z);}
AMSPoint  operator*( AMSPoint o)const{
          return AMSPoint(_x*o._x,_y*o._y,_z*o._z);}
AMSPoint  operator*( number o)const{return AMSPoint(_x*o,_y*o,_z*o);}
AMSPoint  operator/( AMSPoint o)const{
                     return AMSPoint(_x/o._x,_y/o._y,_z/o._z);}
AMSPoint  operator+( number o)const{return AMSPoint(_x+o,_y+o,_z+o);}
AMSPoint  operator/( number o)const{return AMSPoint(_x/o,_y/o,_z/o);}
AMSPoint  operator/( integer o)const{return AMSPoint(_x/o,_y/o,_z/o);}
AMSPoint&  operator =(integer o){_x=o;_y=o;_z=o;return *this;}
AMSPoint&  operator =(number o){_x=o;_y=o;_z=o;return *this;}
AMSPoint&  operator =(const AMSPoint & o){_x=o._x;_y=o._y;_z=o._z;return *this;}
number prod(AMSPoint o)const{return (_x*o._x+_y*o._y+_z*o._z);}
number norm()const {return sqrt(_x*_x+_y*_y+_z*_z);}
number dist(AMSPoint o)const{return sqrt((_x-o._x)*(_x-o._x)+
                                         (_y-o._y)*(_y-o._y)+
                                         (_z-o._z)*(_z-o._z));}
number &  operator[](integer i){
  if(i<=0)return _x;else if(i==1)return _y; else return _z;}
number   operator[](integer i) const{
  if(i<=0)return _x;else if(i==1)return _y; else return _z;}

friend ostream &operator << (ostream &o, const  AMSPoint &b )
   {return o<<" "<<b._x<<" "<<b._y<<" "<<b._z;}
 AMSPoint crossp(const AMSPoint & o);
#ifdef __WRITEROOT__
 friend class AMSPointRoot;
#endif
};
class AMSDir :public AMSPoint{
private:
void _copy(number x,number y,number z);
public:
AMSDir():AMSPoint(){};
AMSDir(const AMSPoint& o){number x,y,z;o.getp( x,y,z);_copy(x,y,z);}
AMSDir(number theta, number phi);
number gettheta() const{return acos(_z);}
number getphi() const{return atan2(_y,_x);}
AMSDir(const AMSDir& o){_copy(o._x,o._y,o._z);}
AMSDir(number x, number y,number z){_copy(x,y,z);}
AMSDir(number x[]){_copy(x[0],x[1],x[2]);}
void setd(number x,number y,number z){_copy(x,y,z);}
void setd(number x[]){_copy(x[0],x[1],x[2]);}
AMSDir cross(const AMSDir & o);
};
#endif
