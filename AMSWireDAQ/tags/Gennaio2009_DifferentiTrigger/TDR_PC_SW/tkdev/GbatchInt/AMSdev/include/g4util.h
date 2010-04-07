//  $Id: g4util.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include "cern.h"
#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/RandGeneral.h"
class AMSRandGeneral{
class service{
public:
RandGeneral * _pgenerator;
integer _id;
geant _xmi;
geant _range;
service * _next;
service(geant parray[],uinteger larray, geant xmi, geant xma, integer id):
_id(id),_next(0),_xmi(xmi),_range(xma-xmi){
 if(larray){
  number * _parray=new number[larray];
  for (int i=0;i<larray;i++)_parray[i]=parray[i];
  _pgenerator=new RandGeneral(_parray,larray);
  delete[] _parray;
 } 
 else _pgenerator=0;
}
~service(){
 delete _next;
 _next=0;
  delete _pgenerator;
  _pgenerator=0;
}
};
uinteger _ngen;
service * _first;
static AMSRandGeneral * _pstatic;
public:
AMSRandGeneral():_first(0),_ngen(0){}
~AMSRandGeneral(){delete _first;}
static void book(integer hid);
static void book(geant parray[], uinteger larray, geant xmi,geant xma, integer hid);
static number hrndm1(integer hid);
};
