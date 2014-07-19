//  $Id: typedefs.h,v 1.4 2008/11/14 10:03:54 haino Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __TYPEDEFS__
#define __TYPEDEFS__
typedef float geant;
typedef double number;
typedef int integer;
typedef float float_r;
typedef double float_d;
typedef unsigned char uchar;
typedef unsigned short int int16u;
typedef unsigned short int uint16;
typedef  short int int16;
typedef long int_L;
typedef unsigned int uinteger;
typedef unsigned long int ulong;
typedef unsigned long long uint64;
#include <assert.h>
#include <iostream>
//#include <strstream>

using namespace std;

//! implementation of a 64 bit unsigned integer type
class uintl{
protected:
  //! internal representation
  uinteger _a[2];  // low 
public:
  //! copy constructor
  uintl(const uintl &a){_a[0]=a._a[0];_a[1]=a._a[1];}
  uintl(integer a, integer b){_a[0]=a;_a[1]=b;}
  //! Constructor \param a= LSB \param b=MSB
  uintl(uinteger a=0, uinteger b=0){_a[0]=a;_a[1]=b;}
  //! Constructor \param a[0]= LSB  a[1]=MSB
  uintl(uinteger a[2]){_a[0]=a[0];_a[1]=a[1];}
  //!accessor to the internal representation
  uinteger v(uinteger l)const {return l<sizeof(_a)/sizeof(_a[0])?_a[l]:0;}

  //! operator accessing to the internal representation
  uinteger & operator () (uinteger l) {return _a[l];}
  bool operator < (const uintl&a) const{ if(v(1)==a.v(1))return v(0)<a.v(0); else return v(1)<a.v(1);  }
  //! equal to operator
  bool operator == (const uintl&a) const{ return v(1)==a.v(1) && v(0)==a.v(0);  }
  //! equal to operator
  bool operator == (uinteger a) const{ return v(1)==0 && v(0)==a;  }
  // stream operator
  friend ostream &operator << (ostream &o, const uintl &b ){return (o<<" [0] "<<b.v(0)<<" [1] "<<b.v(1));}
};


//! implementation of a 128 bit unsigned integer
class uint128{
protected:
  //! internal representation
  uint64 _a[2];  // low 
public:
  //! copy constructor
  uint128(const uint128 &a){_a[0]=a._a[0];_a[1]=a._a[1];}
  uint128(integer a, integer b){_a[0]=a;_a[1]=b;}
  uint128(uint64 a=0, uint64 b=0){_a[0]=a;_a[1]=b;}
  uint128(uint64 a[2]){_a[0]=a[0];_a[1]=a[1];}

  //!accessor to the internal representation
  uint64 v(uint64 l)const {return l<sizeof(_a)/sizeof(_a[0])?_a[l]:0;}

  //!operator accessing to the internal representation
  uint64 & operator () (uint64 l) {return _a[l];}
  bool operator < (const uint128&a) const{ if(v(1)==a.v(1))return v(0)<a.v(0); else return v(1)<a.v(1);  }
  //! equal to operator
  bool operator == (const uint128&a) const{ return v(1)==a.v(1) && v(0)==a.v(0);  }
  //! equal to operator
  bool operator == (uint64 a) const{ return v(1)==0 && v(0)==a;  }
  //! stream operator
  friend ostream &operator << (ostream &o, const uint128 &b ){return (o<<" [0] "<<b.v(0)<<" [1] "<<b.v(1));}
};


#endif
