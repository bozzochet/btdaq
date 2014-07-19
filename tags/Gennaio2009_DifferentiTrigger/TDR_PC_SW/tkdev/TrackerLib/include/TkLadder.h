// $Id: TkLadder.h,v 1.12 2008/11/19 18:28:12 zuccon Exp $

#ifndef __TkLadder__
#define __TkLadder__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkLadder
///\brief An AMS Tracker class for the ladder specific information
///\ingroup tkdbc
///
/// An AMS Tracker class used to put the ladder specific 
/// information in the TKDBc
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 SH  Some utils for MC geometry are added
///\date  2008/04/02 SH  Update for alignment correction
///$Date: 2008/11/19 18:28:12 $
///
///$Revision: 1.12 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkObject.h"

class TkPlane;

class TkLadder :public TkObject{

protected:
  //                  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
  static int pwg[24];
  //                   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
  static int pwgp[24];
  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  ostream& putout(ostream& s);
  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  istream& putin(istream& s);


  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  ostream& putoutA(ostream& s);
  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  istream& putinA(istream& s);

public:
  //! the layer the ladder belongs to [1-8]
  int _layer;
  //! the slot on the layer [1-15]
  /*! the notation is that slot 1 has the largest Y coordinate. 
    A slot can contain up to two ladders which are disposed one 
    facing the other. Positive slot value means a ladder with 
    electronics on the positive X side and negative slot number
    means a ladder with electronics on the negative X side.
   */
  int _slot;
  //! The crate number the ladder is connected to
  int _crate;
  //! The TDR number within the crate
  int _tdr;
  //! The number of sensors composing the ladder
  int _nsensors;
  //! Flag determining if the ladder is used by the laser aligment
  bool _laser_align;

  //! Pointer to the plane object the ladder is mounted to
  TkPlane* _plane;

  //! Default constructor set all properties to zero
  TkLadder();
  //! explicit constructor
  TkLadder(TkPlane* plane,char* name,int layer,int slot,int crate,int tdr,int nsensors);
  //! explicit constructor with TkId= signof(slot) * (abs(slot)+100*layer) and HwId= tdr+ crate*100
  TkLadder(TkPlane* plane,char* name,int TkId,int HwId,int nsensors);
  ~TkLadder(){}

  //! it returns the layer
  int GetLayer(){return _layer;}
  //! it returns the slot
  int GetSlot(){return _slot;}
  //! it returns the side. +1 positive X side  0 negative X side
  int GetSide(){return  _slot>0?1:0;}
  //! it returns the octant in the Philippe notation
  int GetOctant();
  //! it returns the crate number
  int GetCrate(){return _crate;}
  //! it returns the TDR number [0-23]
  int GetTdr(){return _tdr;}
  //! it returns the TkId =  signof(slot) * (abs(slot)+100*layer)
  int GetTkId(){return abs(_slot)/_slot*(abs(_slot)+100*_layer);}
  //! Returns the power group
  int GetPwGroup(){return pwg[_tdr];}
  //! Returns the position within the power group
  int GetPwgroupPos(){return pwgp[_tdr];}
  //! Returns the JMDC absolute node number
  int GetJMDCNum(){ return 282+24*_crate+_tdr;}

  //! it returns the AssemblyId = the slot numbering in the assembly drawings + 100 * layer
  int GetAssemblyId(){return abs(_slot)/_slot*( GetSide()*16 -_slot  +100*_layer);}
  //! it returns the HwId= tdr+ crate*100
  int GetHwId(){return _crate*100+_tdr;}
  //! it sets the Hwid
  void SetHwId(int HwId);
  //! it sets the TkId
  void SetTkId(int Tkid);
  //! returns true is the ladder is used in the laser alignment
  bool IsAlignemnt(){return _laser_align;}
  //! sets the laser alignment flag
  void SetLaserFlag(){ _laser_align=1;}


  void Print(){ cout << putout(cout);}
  //! stream out the content of a the class
  friend ostream &operator << (ostream &o,  TkLadder& b)
  {return b.putout(o);}
  //! fill the class form a stream
  friend istream &operator >> (istream &o,  TkLadder& b)
  {return b.putin(o);}

  //! reads in the Alignement pars from file
  void ReadA(istream &o){putinA(o);} 
  //! writes out the Alignement pars to file
  void WriteA(ostream &o){putoutA(o);}


};
#endif
