//  $Id: arr.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "typedefs.h"
#include <iostream.h>
#include <iomanip.h>
#include "cern.h"
#include <stdlib.h>
#include <math.h>
template <class T> class AMSArr{
private:
T* _first;
T* _last;
T* _cur;
integer _i;
integer _j;
integer _k;
integer _l;
integer _i1;
integer _i2;
integer _i3;
integer _numel;
void _inita(integer,integer,integer,integer);
void _copy( AMSArr<T> &o);
ostream & _printa(ostream &);
T* _hash(integer,integer,integer ,integer );
public:
static integer debug;
friend ostream &operator << (ostream & ost,  AMSArr<T>& o){return o._printa(ost);}

AMSArr(integer i=100,integer j=1,integer k=1,integer l=1){_inita(i,j,k,l);}
~AMSArr(){delete[] _first;}
T& pp();
T& mm();
T& operator()(integer i=0,integer j=0,integer k=0,integer l=0)
              {return *_hash(i,j,k,l);}
AMSArr(  AMSArr<T> &o);
AMSArr<T> & operator =(T&);
AMSArr<T> & operator =(AMSArr<T> &);
AMSArr<T> & operator +(const AMSArr<T> &);
AMSArr<T> & operator -(const AMSArr<T> &);
T  operator *(const AMSArr<T> &);
};


template <class T>
void AMSArr<T>::_inita(integer i,integer j,integer k,integer l){
   _i=i;  
   _j=j;  
   _k=k;  
   _l=l;  
   if(_i<0)_i=-i;
   if(_j<0)_j=-j;
   if(_k<0)_k=-k;
   if(_l<0)_l=-l;
   if(!_i)_i=1;
   if(!_j)_j=1;
   if(!_k)_k=1;
   if(!_l)_l=1;
   _i1=_i;
   _i2=_i1*_j;
  _i3=_i2*_k;
  _numel=_i3*_l;
  _first=new T[_numel];
  _cur=_first;
  _last=_first+_numel;
  if(!_first){
  if(AMSArr<T>::debug)
    cerr <<"AMSArr<T>-F-Constructor failed for "<<_numel<< " elements"<<endl;
  _numel=0;
  _last=0;
  }
  }

template <class T>
void AMSArr<T>::_copy(AMSArr<T> &o){
_first=new T[o._numel];
if(_first){
  UCOPY(o._first,_first,sizeof(T)*o._numel/4);
_i1=o._i1;
_i2=o._i2;
_i3=o._i3;
_numel=o._numel;
_last=_first+_numel;
_i=o._i;
_j=o._j;
_k=o._k;
_l=o._l;
}
else{
 if(AMSArr<T>::debug)
    cerr <<"AMSArr<T>-F-Copy failed for "<<o._numel<< " elements"<<endl;

_i1=0;
_i2=0;
_i3=0;
_numel=0;
_last=0;
_i=0;
_j=0;
_k=0;
_l=0;
}
}
template <class T>
AMSArr<T>::AMSArr( AMSArr<T> &o){
_copy(o);
}
template <class T>
AMSArr<T> & AMSArr<T>::operator = ( AMSArr<T> &o){
  if (this != &o){
  delete[] _first;
  _copy(o);
}
return *this;
}
template <class T>
AMSArr<T> & AMSArr<T>::operator = ( T & o){
*_cur=o;
return *this;
}
template <class T>
AMSArr<T> & AMSArr<T>::operator + ( const AMSArr<T> &o){
integer min=_numel>o._numel?o._numel:_numel;
for(int i=0;i<min;i++)_first[i]=_first[i]+o._first[i];
return *this;
}
template <class T>
AMSArr<T> & AMSArr<T>::operator - ( const AMSArr<T> &o){
integer min=_numel>o._numel?o._numel:_numel;
for(int i=0;i<min;i++)_first[i]=_first[i]-o._first[i];
return *this;
}
template <class T>
T AMSArr<T>::operator * ( const AMSArr<T> &o){
integer min=_numel>o._numel?o._numel:_numel;
T product=0;
for(int i=0;i<min;i++)product=product+_first[i]*o._first[i];
return product;
}
template <class T>
//integer i4=i+j+k+l;
//integer i3=i+j+k;
//integer i2=i+j;
//integer 
//iout=(i4*i4*i4+6*i4*i4+11*i4+6)/2*i4/12+(i3*i3+3*i3+2)/2*i3/3+i2*(i2+1)/2+i;
//   cout <<setw(5)<<" res "<<i<<j<<k<<l<<" "<<iout<<endl;

T & AMSArr<T>::pp(){
if(_cur <_last)return *(_cur++);
else return *_last;
}
template <class T>
T & AMSArr<T>::mm(){
if(_cur>_first)return *(_cur--);
else return *_first;
}
template <class T>
T * AMSArr<T>::_hash(integer i, integer j,integer k, integer l){
integer idir=i-1+_i1*(j-1)+_i2*(k-1)+_i3*(l-1);
if(idir>=0 && idir <_numel)_cur=_first+idir;
else  if(AMSArr<T>::debug)
        cerr<<"AMSArr-[]-F-Wrong index"<<setw(6)<<i<<j<<k<<l<<endl;
  return _cur;
}
template <class T>
ostream & AMSArr<T>::_printa(ostream & ost){
integer i,j,k,l;
for (l=1;l<_l+1;l++){
  for(k=1;k<_k+1;k++){
    for(j=1;j<_j+1;j++){
      for(i=1;i<_i+1;i++){
        ost<<" ( "<<i<<" , "<<j<<" , "<<k<<" , "<<l<<") "<<*_hash(i,j,k,l)<<endl;
      }
    }
  }
}
return ost;
}
