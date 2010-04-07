// $Id: TkObject.h,v 1.8 2008/10/23 10:12:39 zuccon Exp $

#ifndef __TkObject__
#define __TkObject__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkObject
///\brief An AMS Tracker class for the generic tracker object in the TKDBc
///\ingroup tkdbc
///
/// This class contains only the name of the object, 
/// an AMSPoint for the position of the object
/// and a AMSRotMat to define the position.
/// The TkObject class must not be used directly but instead you should
/// derive a new specific class for the object the you want to represent.
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/02/21 PZ  Updates for alignment correction
///\date  2008/04/02 SH  putin/putout updated for the alignment correction
///$Date: 2008/10/23 10:12:39 $
///
///$Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
#include <iostream>

class TkObject {

protected:
  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  virtual ostream& putout(ostream& s) { return s<<name<<endl <<pos <<endl<<rot
						             <<posA<<endl<<rotA;}

  virtual ostream& putoutA(ostream& s) { return s<<name<<endl<<posA<<endl<<rotA;}

  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  virtual istream& putin(istream& s);

  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  virtual istream& putinA(istream& s);

public:
  //! the name of the TkObject
  char name[50];
  //! the 3D position of the TkObject 
  AMSPoint   pos;
  //! the orientation in space of the TkObject
  AMSRotMat  rot;

  //! Alignement Correction to  3D position of the TkObject 
  AMSPoint   posA;
  //! Alignement Correction to  the orientation in space of the TkObject
  AMSRotMat  rotA;


public:
  //! explicit constructor
  TkObject(char* namein,AMSPoint posin,AMSRotMat rotin);
  //! Standard constructor. It sets the position to (0,0,0) and rotmat to the identy matrix
  TkObject(char* namein="");
  //! The destructor
  virtual ~TkObject(){}
  //! set the postion from a set of 3 coordinates
  void setpos(number x, number y, number z){pos.setp(x,y,z);}
  //! set the rotation matric from an array[3][3]
  void setrot(number rr[][3]){rot.SetMat(rr);}
  //! set the rotation matric from an array[3][3]
  void setrot(number ** rr){rot.SetMat(rr);}
  //! it returns a copy of the AMSPoint defining the position of the TkObject
  AMSPoint  GetPos()   { return pos;}
  //! it returns a copy of the AMSRotMat defining the orientation of the TkObject
  AMSRotMat GetRotMat(){ return rot;}




  //! set the Alignment correction from a set of 3 coordinates
  void setposA(number x, number y, number z){posA.setp(x,y,z);}
  //! set the Alignment correction rotation matrix from an array[3][3]
  void setrotA(number rr[][3]){rotA.SetMat(rr);}
  //! set the Alignment correction rotation matrix from an array[3][3]
  void setrotA(number ** rr){rotA.SetMat(rr);}
  //! it returns a copy of the AMSPoint defining Alignment correction to the position of the TkObject
  AMSPoint  GetPosA()   { return posA;}
  //! it returns a copy of the AMSRotMat defining Alignment correction to the orientation of the TkObject
  AMSRotMat GetRotMatA(){ return rotA;}
  //! Create a rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
void SetRotAngles(double alpha, double beta, double gamma){
    rot.SetRotAngles(alpha, beta, gamma);
  }
  //!copy the rot mat to a 3x3 array
  void RotToMat(number nrm[][3]);


  //! Create a rotation matrix(alignemnt) according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
void SetRotAnglesA(double alpha, double beta, double gamma){
    rotA.SetRotAngles(alpha, beta, gamma);
  }


  //! Get the angles corresponding to te current rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  void GetRotAngles(double& alpha, double& beta, double& gamma){
    rot.GetRotAngles(alpha, beta, gamma);
  }

  //! Get the angles corresponding to te current  (aligment) rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  void GetRotAnglesA(double& alpha, double& beta, double& gamma){
    rotA.GetRotAngles(alpha, beta, gamma);
  }

  //! operator to stream out (stdout or a file) the data of the class
  friend ostream &operator << (ostream &o,  TkObject& b)
  {return b.putout(o);}

  //! operator to read the class data from an input  stream  (stdin or a file) 
  friend istream &operator >> (istream &o,  TkObject& b)
  {return b.putin(o);}

  void ReadA(istream &o){putinA(o);}
  void WriteA(ostream &o){putoutA(o);}

};




#endif


