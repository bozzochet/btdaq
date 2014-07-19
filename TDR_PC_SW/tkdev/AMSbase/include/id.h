//  $Id: id.h,v 1.1 2008/06/24 11:17:14 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSID__
#define __AMSID__
#include "typedefs.h"
#include <sstream>
#include <cstddef>
#include <cstdlib>
#include <string>

//! Base AMS class implementing a Named object
class AMSID {
protected:
  // realization
  //! integer type identificator
  integer _id;
  //! Object name
  char * _name;
  //! Used internally to generate the name
  void _copyname( char  const [] );
  //! printout to an ostream
  virtual ostream & print(ostream &) const;
  //! Error handling
  static char *_error;
  static char _pull[];

public:
  //! Std constructor (id=0;name=0;);
  AMSID (): _id(0),_name(0){}
  //! explicit constructor
  AMSID(integer id, const char name[]=0);
  //! explicit constructor
  AMSID(const char name[],integer id=0);
  //! copy constructor
  AMSID (const AMSID&);
  //! destructor
  virtual ~AMSID();
  //! copy operator;
  AMSID &operator=(const AMSID&);
  //! returns a new AMSid with the name of this and id= sum of the ids
  AMSID operator+(const AMSID&); 
  //! returns a new AMSid with the name of this and id= diff of the ids
  AMSID operator-(const AMSID&);
  //! returns a new AMSid with the name of this and id= product of the ids
  AMSID operator*(const AMSID&);
  //! Printout operator
  friend ostream &operator << (ostream &o, const AMSID &b );
  //! compare operator (see code for the implementation)
  virtual int operator < (const AMSID&) const;
  //! return true if the name and the id are the same
  virtual int operator == (const AMSID&) const;
  //! returns the numeric id
  inline integer getid() const{return _id;}
  //! returns the name
  inline const char *  getname() const{return _name;}
  //! returns a reference to id (breaks the encapsulation?)
  inline integer & id(){return _id;}  //getsetid
  //! Set the integer id
  inline void setid(integer id){_id=id;}
  //! Set the name
  inline void setname(const char name []){delete[] _name;_copyname(name);}
  //! Error handling decl
  static void ResetError();
  //! Error handling decl
  static char * IsError();
  //! Error handling decl
  static void PrintError();

};


//! AMSID class based only on integer id
class AMSIDi : public AMSID{
public:
  AMSIDi (): AMSID(){};
  AMSIDi (const AMSID&a): AMSID(a){}
  AMSIDi (int i): AMSID(i,0){}
  int operator < (const AMSID& a) const{
    return getid() < a.getid()? 1:0;
  }

};


//! AMSID class based only on name
class AMSIDs : public AMSID{
public:
  AMSIDs (): AMSID(){};
  AMSIDs (const AMSID&a): AMSID(a){}
  AMSIDs (const char name[]):AMSID(0,name){}
  int operator < (const AMSID&a) const{
    int i;
    if(getname() && a.getname())i=strcmp(getname(),a.getname());
    else if (!a.getname() && !getname())i=0;
    else if (!a.getname())i=1;
    else i=-1;
    return i<0?1:0;
  }

};

#endif
