//  $Id: TkObject.C,v 1.6 2008/10/23 10:12:39 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkObject.C
///\brief Source file of TkObject class
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/02/21 PZ  Updates for alignment correction
///\date  2008/04/02 SH  putin/putout updated for the alignment correction
///$Date: 2008/10/23 10:12:39 $
///
///$Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkObject.h"


TkObject::TkObject(char* namein){
  sprintf(name,"%s",namein);
  pos=0;
  rot.Reset();

  posA=0;
  rotA.Reset();
}


TkObject::TkObject(char* namein,AMSPoint posin,AMSRotMat rotin){

  sprintf(name,"%s",namein);
  pos=posin;
  rot=rotin;
  posA=0;
  rotA.Reset();

}


istream& TkObject::putin(istream& s){

  s.width(50);

  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> name;
  if (s.eof()) return s;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> pos;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rot;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> posA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rotA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }

  return s;
}





istream& TkObject::putinA(istream& s){

  s.width(50);

  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> name;
  if (s.eof()) return s;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> posA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rotA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }

  return s;
}






void TkObject::RotToMat(number nrm[][3]){
  
  for (int row=0;row<3;row++)
    for (int col=0;col<3;col++)
      nrm[row][col]=rot.GetEl(row,col);
  return;
}
