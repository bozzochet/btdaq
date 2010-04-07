//  $Id: id.C,v 1.1 2008/06/24 11:18:29 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "id.h"

// Static variables initialization
char * AMSID::_error=0;
char AMSID::_pull[256]="";

AMSID ::AMSID(integer i, const char name[] ):_id(i){
  _copyname(name);
}
AMSID ::AMSID(const char name[],integer i ):_id(i){
  _copyname(name);
}
AMSID ::AMSID(const AMSID & o){
  _copyname(o._name);
  _id=o._id;
}
AMSID &AMSID::operator= (const AMSID &o){
  if (this != &o){
    delete[] _name;
    _copyname(o._name);
    _id=o._id;
  }
  return *this;
}
AMSID::~AMSID(){
  delete[] _name;
  _name=0;
}

// Copy function
void AMSID::_copyname( const char   name  [] ){
  if(name){
    _name=new char[strlen(name)+1];
    if(_name)strcpy(_name,name);
    else{
      _name=0;

    }
  }
  else _name=0;
}
// Error handling
void   AMSID::ResetError(){_error=0;strcpy(_pull,"");}
char *  AMSID::IsError(){return _error;}
void  AMSID::PrintError(){cout << _error <<endl;}
// Print structure
ostream &operator << (ostream &o, const AMSID &b )
{return b.print(o);}  
ostream & AMSID::print(ostream &o)const{
  return(o <<_name << "  id = "<<_id );
}
//compare

int AMSID::operator <(const AMSID&a)const{
  int i;
  if(getname() && a.getname())i=strcmp(getname(),a.getname());
  else if (!a.getname() && !getname())i=0;
  else if (!a.getname())i=1;
  else i=-1;
  if(i<0)return 1;
  else if (0<i) return 0;
  if(getid() < a.getid())return 1;
  else return 0;

}

int AMSID::operator ==(const AMSID&a)const{
  if (getid() != a.getid()) return 0;
  else{  
    if(getname() && a.getname())return !strcmp(getname(),a.getname());
    else if (!a.getname() && !getname())return 1;
    else return 0;

  }

}
AMSID AMSID::operator +( const AMSID & o){
  return AMSID(_id+o._id,getname());
}
AMSID AMSID::operator -( const AMSID &o){
  return AMSID(_id-o._id,getname());
}
AMSID AMSID::operator *( const AMSID &o){
  return AMSID(_id*o._id,getname());
}


// AMSi64 stuff
