//  $Id: id.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSID__
#define __AMSID__
#include "typedefs.h"
#include <strstream>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
class AMSID
{
 protected:
// realization
  integer _id;
  char * _name;
  void _copyname( char  const [] );
  virtual ostream & print(ostream &) const;
// Error handling
  static char *_error;
  static char _pull[];
 public:
   AMSID (): _id(0),_name(0){}
   AMSID(integer id, const char name[]=0);
   AMSID(const char name[],integer id=0);
   AMSID (const AMSID&);
   virtual ~AMSID();

   AMSID &operator=(const AMSID&);
   AMSID operator+(const AMSID&);
   AMSID operator-(const AMSID&);
   AMSID operator*(const AMSID&);
   friend ostream &operator << (ostream &o, const AMSID &b );
// compare
   virtual int operator < (const AMSID&) const;
   virtual int operator == (const AMSID&) const;
// get
   inline integer getid() const{return _id;}
   inline const char *  getname() const{return _name;}
// set
   inline integer & id(){return _id;}  //getsetid
   inline void setid(integer id){_id=id;}
   inline void setname(const char name []){delete[] _name;_copyname(name);}
// Error handling decl
  static void ResetError();
  static char * IsError();
  static void PrintError();

};
class AMSIDi : public AMSID{
 public:
   AMSIDi (): AMSID(){};
   AMSIDi (const AMSID&a): AMSID(a){}
   AMSIDi (int i): AMSID(i,0){}
 int operator < (const AMSID& a) const{
  return getid() < a.getid()? 1:0;
 }

};
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
