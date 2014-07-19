//  $Id: link.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
// 
// Oct 04, 1996. add _ContPos
// 
// last edit : Oct 04, 1996. ak.
// 

#ifndef __AMSlink__
#define __AMSlink__
#include "typedefs.h"
#include "upool.h"
#include "apool.h"
#include "id.h"
class TObject;
class AMSlink {
protected:
 uinteger _status;
 integer _pos; 
 int _vpos;
  virtual void _copyEl()=0;
  virtual void _printEl(ostream &stream)=0;
  virtual void _writeEl()=0;
public:
 uinteger checkstatus(integer checker) const{return _status & checker;}
 uinteger getstatus() const{return _status;}
 void setstatus(uinteger status){_status=_status | status;}
 void clearstatus(uinteger status){_status=_status & ~status;}
 int      GetClonePointer(){return _vpos;}
 void SetClonePointer(int vpos=-1){
 _vpos=vpos;
}
 bool PointerNotSet()const {return _vpos<0;}
 virtual void resethash(integer id, AMSlink *head){};
  virtual AMSID crgid(integer i=0){return AMSID();}
  virtual integer operator < ( AMSlink & o) const;
  AMSlink * _next;
  AMSlink(uinteger status,AMSlink * n=0): 
  _next(n),_pos(0), _status(status){
  _vpos=-1;
};
  AMSlink(integer status=0): 
  _next(0),_pos(0), _status(status),_vpos(-1){};
  virtual ~AMSlink(){};
  integer getpos()const{return _pos;}
  void setpos(integer pos){_pos=pos;}
//  void * operator new(size_t t, void *p) {return p;}
//  void  operator delete(size_t t, void *p) {p=0;}
  void * operator new(size_t t) {return UPool.insert(t);}
  void operator delete(void *p)
   {UPool.udelete(p);p=0;}
  inline AMSlink*  next(){return _next;}
  AMSlink*  next(integer & last);
  virtual integer   testlast();
  friend class AMSContainer;
   void resetnext(){_next=0;}

};
#endif
