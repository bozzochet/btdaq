//  $Id: point.C,v 1.3 2008/10/01 09:54:35 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "typedefs.h"
#include "point.h"
#include <cmath>
ClassImp(AMSPoint);

AMSPoint  AMSPoint::mm3(number m1[][3]){
   number x,y,z;
    x=m1[0][0]*_x+m1[0][1]*_y+m1[0][2]*_z;
    y=m1[1][0]*_x+m1[1][1]*_y+m1[1][2]*_z;
    z=m1[2][0]*_x+m1[2][1]*_y+m1[2][2]*_z;
   return AMSPoint(x,y,z);
  }
AMSPoint  AMSPoint::mm3i(number m1[][3]){
   number x,y,z;
    x=m1[0][0]*_x+m1[1][0]*_y+m1[2][0]*_z;
    y=m1[0][1]*_x+m1[1][1]*_y+m1[2][1]*_z;
    z=m1[0][2]*_x+m1[1][2]*_y+m1[2][2]*_z;
   return AMSPoint(x,y,z);
  }
AMSPoint  AMSPoint::mm3i(AMSRotMat m1){
   number x,y,z;
    x=m1.GetEl(0, 0)*_x+m1.GetEl(1, 0)*_y+m1.GetEl(2, 0)*_z;
    y=m1.GetEl(0, 1)*_x+m1.GetEl(1, 1)*_y+m1.GetEl(2, 1)*_z;
    z=m1.GetEl(0, 2)*_x+m1.GetEl(1, 2)*_y+m1.GetEl(2, 2)*_z;
   return AMSPoint(x,y,z);
  }

ClassImp(AMSDir);

void AMSDir::_copy(number u, number v, number w){
  if(u==0 && v==0 && w==0){
   _x=0;
   _y=0;
   _z=0;
  }
  else{
   number s=sqrt(u*u+v*v+w*w);
   if(s !=0){
    _x=u/s; 
    _y=v/s; 
    _z=w/s; 
   }
  }
}

AMSDir::AMSDir(number theta, number phi){
 _z=cos(theta);
 _x=sin(theta)*cos(phi);
 _y=sin(theta)*sin(phi);
}

AMSDir AMSDir::cross(const AMSDir & o){
 number x=_y*o._z-_z*o._y;
 number y=_z*o._x-_x*o._z;
 number z=_x*o._y-_y*o._x;
 return AMSDir(x,y,z);
}
AMSPoint AMSPoint::crossp(const AMSPoint & o){
 number x=_y*o._z-_z*o._y;
 number y=_z*o._x-_x*o._z;
 number z=_x*o._y-_y*o._x;
 return AMSPoint(x,y,z);
}


AMSRotMat::AMSRotMat(number** nin){
  for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      _nrm[ii][jj]=nin[ii][jj];

}
AMSRotMat::AMSRotMat(number nin[][3]){
  
   for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      _nrm[ii][jj]=nin[ii][jj];
}


AMSRotMat::AMSRotMat(const AMSRotMat& orig){
  for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      _nrm[ii][jj]=orig._nrm[ii][jj];
  
}


void AMSRotMat::Reset(){
  for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      if(ii==jj) _nrm[ii][jj]=1;
      else _nrm[ii][jj]=0;
  
}

void AMSRotMat::SetMat(number** nin){
  for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      _nrm[ii][jj]=nin[ii][jj];

}
void AMSRotMat::SetMat(number nin[][3]){
  
   for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      _nrm[ii][jj]=nin[ii][jj];
}

AMSRotMat& AMSRotMat::operator=(const AMSRotMat& orig){
   for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      _nrm[ii][jj]=orig._nrm[ii][jj];
  
   return *this;
}



AMSRotMat AMSRotMat::operator*(const AMSRotMat& orig){
  number nrm2[3][3];
  for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      nrm2[ii][jj]=
	_nrm[ii][0]*orig._nrm[0][jj]+
	_nrm[ii][1]*orig._nrm[1][jj]+
	_nrm[ii][2]*orig._nrm[2][jj];
 //  for(int ii=0;ii<3;ii++)
//     for(int jj=0;jj<3;jj++)
//        _nrm[ii][jj]=nrm2[ii][jj];
  
  return AMSRotMat(nrm2);
}

bool AMSRotMat::operator==(const AMSRotMat& orig){
   for(int ii=0;ii<3;ii++)
    for(int jj=0;jj<3;jj++)
      if(_nrm[ii][jj]!=orig._nrm[ii][jj]) return 0;
  
   return 1;
}

AMSPoint operator*(const AMSRotMat& mat, const AMSPoint& Point){

  AMSPoint out;
  for(int ii=0;ii<3;ii++)
    out[ii]=mat._nrm[ii][0]*Point._x+
      mat._nrm[ii][1]*Point._y+
      mat._nrm[ii][2]*Point._z;
  return out;
  
}


void AMSRotMat::XParity(){
  number nn[3][3]={{-1,0,0},{0,1,0},{0,0,1}};
  AMSRotMat aa(nn);
  (*this)=(*this)*aa;
  return;
}

void AMSRotMat::YParity(){
  number nn[3][3]={{1,0,0},{0,-1,0},{0,0,1}};
  AMSRotMat aa(nn);
  (*this)=(*this)*aa;

  return;
}

void AMSRotMat::ZParity(){
  number nn[3][3]={{1,0,0},{0,1,0},{0,0,-1}};
  AMSRotMat aa(nn);
  (*this)=(*this)*aa;
  return;
}

void  AMSRotMat::SetRotAngles(double alpha, double beta, double gamma){
  
  number nn1[3][3];
  nn1[0][0]=cos(alpha);
  nn1[0][1]=sin(alpha);
  nn1[0][2]=0;

  nn1[1][0]=-sin(alpha);
  nn1[1][1]=cos(alpha);
  nn1[1][2]=0;

  nn1[2][0]=0;
  nn1[2][1]=0;
  nn1[2][2]=1;
  
  AMSRotMat aa(nn1);


  number nn2[3][3];
  nn2[0][0]=cos(beta);
  nn2[0][1]=0;
  nn2[0][2]=-sin(beta);

  nn2[1][0]=0;
  nn2[1][1]=1;
  nn2[1][2]=0;

  nn2[2][0]=sin(beta);
  nn2[2][1]=0;
  nn2[2][2]=cos(beta);

  
  AMSRotMat bb(nn2);

  number nn3[3][3];
  nn3[0][0]=1;
  nn3[0][1]=0;
  nn3[0][2]=0;

  nn3[1][0]=0;
  nn3[1][1]=cos(gamma);
  nn3[1][2]=sin(gamma);

  nn3[2][0]=0;
  nn3[2][1]=-sin(gamma);
  nn3[2][2]=cos(gamma);

  
  AMSRotMat cc(nn3);

  (*this)=cc*bb*aa;
  //  (*this)*bb;
  //  (*this)*aa;
  return;
}


void  AMSRotMat::GetRotAngles(double& alpha, double& beta, double& gamma){

  gamma = atan2(_nrm[1][2],_nrm[2][2]);
  alpha = atan2(_nrm[0][1],_nrm[0][0]);
  beta  = atan2( (-1*_nrm[0][2]),(_nrm[2][2]/cos(gamma)));
  return;
}

AMSRotMat &AMSRotMat::Invert(void)
{
  number c00 = _nrm[1][1]*_nrm[2][2]-_nrm[1][2]*_nrm[2][1];
  number c01 = _nrm[1][2]*_nrm[2][0]-_nrm[1][0]*_nrm[2][2];
  number c02 = _nrm[1][0]*_nrm[2][1]-_nrm[1][1]*_nrm[2][0];
  number c10 = _nrm[2][1]*_nrm[0][2]-_nrm[2][2]*_nrm[0][1];
  number c11 = _nrm[2][2]*_nrm[0][0]-_nrm[2][0]*_nrm[0][2];
  number c12 = _nrm[2][0]*_nrm[0][1]-_nrm[2][1]*_nrm[0][0];
  number c20 = _nrm[0][1]*_nrm[1][2]-_nrm[0][2]*_nrm[1][1];
  number c21 = _nrm[0][2]*_nrm[1][0]-_nrm[0][0]*_nrm[1][2];
  number c22 = _nrm[0][0]*_nrm[1][1]-_nrm[0][1]*_nrm[1][0];

  number t0 = std::abs(_nrm[0][0]);
  number t1 = std::abs(_nrm[1][0]);
  number t2 = std::abs(_nrm[2][0]);
  number det, tmp;

  if (t0 >= t1) {
    if (t2 >= t0) {
      tmp = _nrm[2][0];
      det = c12*c01-c11*c02;
    }
    else {
      tmp = _nrm[0][0];
      det = c11*c22-c12*c21;
    }
  }
  else if (t2 >= t1) {
    tmp = _nrm[2][0];
    det = c12*c01-c11*c02;
  }
  else {
    tmp = _nrm[1][0];
    det = c02*c21-c01*c22;
  }

  if (det == 0 || tmp == 0) return *this;

  number s = tmp/det;
  _nrm[0][0] = s*c00;
  _nrm[0][1] = s*c10;
  _nrm[0][2] = s*c20;
  _nrm[1][0] = s*c01;
  _nrm[1][1] = s*c11;
  _nrm[1][2] = s*c21;
  _nrm[2][0] = s*c02;
  _nrm[2][1] = s*c12;
  _nrm[2][2] = s*c22;

  return *this;
}
