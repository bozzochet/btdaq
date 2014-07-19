// $Id: TkPlane.h,v 1.4 2008/10/23 10:12:39 zuccon Exp $

#ifndef __TkPlane__
#define __TkPlane__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkPlane
///\brief An AMS Tracker class for the support planes specific information
///\ingroup tkdbc
///
/// An AMS Tracker class used to put the support planes specific 
/// information in the TKDBc
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///$Date: 2008/10/23 10:12:39 $
///
///$Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkObject.h"

class TkPlane :public TkObject{

protected:
  

  ostream& putout(ostream& s);
  istream& putin(istream& s);

  ostream& putoutA(ostream& s);
  istream& putinA(istream& s);

public:
  //! plane number
  short int _pnumber;
  //! number of available slots on the two faces [0] up [1] down
  short int _nslot[2];

  //! the Standard constructor. It put all the variables to 0
  TkPlane();
 
  //! explicit constructor. 
  /*! @param name the name of the plane object
    @param number the plane number
    @param slot  a integer array[2] containing the number of slots on the top face[0] and on bottom face[1] 
  */
  TkPlane(char* name,int number, short int nslot[2]);
  //! the standard destructor
  ~TkPlane(){}

  
  //! operator to stream out (stdout or a file) the data of the class
  friend ostream &operator << (ostream &o,  TkPlane& b)
  {return b.putout(o);}
  
  //! operator to read the class data from an input  stream  (stdin or a file) 
  friend istream &operator >> (istream &o,  TkPlane& b)
  {return b.putin(o);}

  //! reads in the Alignement pars from file
  void ReadA(istream &o){putinA(o);} 
  //! writes out the Alignement pars to file
  void WriteA(ostream &o){putoutA(o);}

};
#endif
