//  $Id: amsdbc.h,v 1.2 2008/07/02 14:22:13 zuccon Exp $
// Author V. Choutko 24-may-1996
#ifndef __AMSDBC__
#define __AMSDBC__
#include "typedefs.h" 
#include "astring.h"
#include "point.h"

//! Used to store the strings associated to a  global AMSerror integer ID
class amsglobalerror{
private:
  AString _a;
  uinteger _level;
public:
  const char * getmessage(){return (const char*)_a;};
  uinteger getlevel(){return _level;}
  amsglobalerror(char* a,uinteger level=1):_a(a),_level(level){};
  amsglobalerror(const char* a=0,uinteger level=1):_a(a),_level(level){};
};

namespace AMSChargConst{
  const int MaxZTypes=10;
} 

/*! \class AMSDBc
  \brief Geometrical and other constants in a Static Class
*/
class AMSDBc {
public:

#ifdef __DB__
  friend class AMSDBcD;
#endif
  //! Sting pointing to the AMSDataBase path (default $AMSDataDir/DataBase)
  static char * amsdatabase;
  //! constants
  static const number raddeg;
  static const number pi;
  static const number twopi;
  static const integer MaxZTypes;
  // AMS parameters
  static  geant ams_size[3];
  //! AMS rotation matrix
  static number ams_nrm[3][3];
  static const integer ams_rotmno;
  static const char ams_name[129];
  static  geant ams_coo[3];
  static const geant amsdext;
  //bits
  static const uinteger AMBIG;
  static const uinteger BAD;
  static const uinteger USED;
  static const uinteger DELETED;
  static const uinteger RELEASED;
  static const uinteger REFITTED;
  static const uinteger WIDE;
  static const uinteger BADHIS;
  static const uinteger ONESIDE;
  static const uinteger BADTIME;
  static const uinteger NEAR;
  static const uinteger WEAK;
  static const uinteger AwayTOF;
  static const uinteger FalseX;
  static const uinteger FalseTOFX;
  static const uinteger RECOVERED;
  static const uinteger LocalDB;
  static const uinteger GlobalDB;
  static const uinteger CHARGEUSED;
  static const uinteger GOOD;
  static const uinteger BADINTERPOL;
  static const uinteger NOTRACK;
  static const uinteger AOVERFLOW;
  static const uinteger LOWGCHUSED;
  static const uinteger TRDTRACK;
  static const uinteger LEAK;
  static const uinteger CATLEAK;
  static const uinteger JUNK;
  static const uinteger ECALTRACK;
  static const uinteger S_AMBIG;
  static const uinteger GOLDEN;
  static const uinteger TOFFORGAMMA;
  // machine dependent values

  static integer BigEndian;
  static integer debug;
  //! init a rotation matrix(ams_nrm) around the Y AMS axis
  static void init(float angle);
  //! Transform the the coo with the matrix (ams_nrm)
  static void transform(geant coo[3]);
  //! Transform the the AMSPoint dir  with the matrix (ams_nrm)
  static void transform(AMSPoint & dir);
};




#endif
