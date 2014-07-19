//  $Id: TkLadder.C,v 1.10 2008/10/23 10:12:39 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkLadder.C
///\brief Source file of TkLadder class
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 SH  Some utils for MC geometry are added
///\date  2008/04/02 SH  Update for alignment correction
///$Date: 2008/10/23 10:12:39 $
///
///$Revision: 1.10 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkLadder.h"
#include "TkDBc.h"


   //                   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
int TkLadder::pwg[24]= {1,1,4,4,1,1,4,4,1,1, 4, 4, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3};
   //                   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
int TkLadder::pwgp[24]={1,2,5,6,3,4,3,4,5,6, 1, 2, 1, 2, 5, 6, 3, 4, 3, 4, 5, 6, 1, 2};

TkLadder::TkLadder():TkObject()
{
  _layer=0;
  _slot=0;
  _crate=0;
  _tdr=0;
  _nsensors=0;
  _plane=NULL;
  _laser_align=0;
}

TkLadder::TkLadder(TkPlane* plane,char* name,int layer,int slot,int crate,int tdr, int nsensors):TkObject(name)
{
  _layer=layer;
  _slot=slot;
  _crate=crate;
  _tdr=tdr;
  _nsensors=nsensors;
  _plane=plane;
  _laser_align=0;
}

TkLadder::TkLadder(TkPlane* plane,char* name,int Trid,int HwId,  int nsensors):TkObject(name){
  _layer=abs(Trid)/100;
  _slot=abs(Trid)/Trid*(abs(Trid)-100*_layer);
  _crate=HwId/100;
  _tdr=HwId-_crate*100;
  _nsensors=nsensors;
  _plane=plane;
  _laser_align=0;
}


ostream& TkLadder::putout(ostream& s){

    return TkObject::putout(s)<<
      GetTkId()<<"  "<<
      GetHwId()<<"  "<<_nsensors<<endl;

}

void TkLadder::SetTkId(int Trid){
  _layer=abs(Trid)/100;
  _slot=abs(Trid)/Trid*(abs(Trid)-100*_layer);
}

void TkLadder::SetHwId(int HwId){
  _crate=HwId/100;
  _tdr=HwId-_crate*100;
}

istream& TkLadder::putin(istream& s){
  int hw,tk;
  TkObject::putin(s);
  if(s.eof()) return s;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good before"<<endl;
  }
  s>>tk>>hw>>_nsensors;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good  after"<<endl;
  }
  SetTkId(tk);
  SetHwId(hw);
  
  return s;

}


int TkLadder::GetOctant(){
  
  int aa=GetSide();

  if(abs(_slot)<1||abs(_slot)>15||aa>1||aa<0) return -9999;

  if(_layer==1){
    if(aa==0){
      if(abs(_slot)>=7) return 4;
      if(abs(_slot) <7) return 2;
    } 
    else {
      if(abs(_slot)<=9) return 1;
      if(abs(_slot) >9) return 3;
    }
  }

  else if(_layer==8){
    if(aa==0){
      if(abs(_slot)>=7) return 8;
      if(abs(_slot) <7) return 6;
    } 
    else {
      if(abs(_slot)<=9) return 5;
      if(abs(_slot) >9) return 7;
    }
  }
  
  else if (_layer==2 ||_layer==3 ||_layer==4){
    if(abs(_slot)==8)  return -9999;

    if(abs(_slot)<=7 && aa==0) return 2;  
    if(abs(_slot)>=9 && aa==0) return 4;
    
    if(abs(_slot)>=9 && aa==1) return 3;
    if(abs(_slot)<=7 && aa==1) return 1; 
  }

  else if (_layer==5 ||_layer==6 ||_layer==7){
    if(abs(_slot)==8)  return -9999;
  
    if(abs(_slot)>=9 && aa==0) return 8; 
    if(abs(_slot)<=7 && aa==0) return 6; 
    if(abs(_slot)>=9 && aa==1) return 7;
    if(abs(_slot)<=7 && aa==1) return 5; 
  }
  
  return -9999;


}




istream& TkLadder::putinA(istream& s){
  int hw,tk;
  TkObject::putinA(s);
  if(s.eof()) return s;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good before"<<endl;
  }
  s>>tk>>hw>>_nsensors;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good  after"<<endl;
  }
  
  return s;

}


ostream& TkLadder::putoutA(ostream& s){

    return TkObject::putoutA(s)<<
      GetTkId()<<"  "<<
      GetHwId()<<"  "<<_nsensors<<endl;

}
