//  $Id: link.h,v 1.4 2008/09/30 18:27:47 zuccon Exp $
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

class TObject;
/*! \brief ("abstract")AMS base class for the Reconstructed/Simulated objects
    AMS link implements a  linked list element which must be
    used as base class for all the AMS objects that are into a 
    AMScontainer
 */
class AMSlink {
protected:
  integer  _pos; 
  uinteger _status;
  int      _vpos;
  //! virtual mehtod to implement the copy 
  virtual void _copyEl()=0;
  //! virtual mehtod to implement the printout to the channel stream
  virtual void _printEl(ostream &stream)=0;
  //! virtual mehtod to implement the writing to disk
  virtual void _writeEl()=0;

public:
  //! pointer to the next element in the list
  AMSlink * _next;
  //! constructor
  AMSlink(uinteger status,AMSlink * n=0): 
    _pos(0), _status(status),_vpos(-1),_next(n){}

  //! constructor
  AMSlink(integer status=0): 
    _pos(0), _status(status),_vpos(-1),_next(0){};

  //! destructor
  virtual ~AMSlink(){};

  uinteger checkstatus(integer checker) const{return _status & checker;}
  uinteger getstatus() const{return _status;}
  void     setstatus(uinteger status){_status=_status | status;}
  void     clearstatus(uinteger status){_status=_status & ~status;}
  int      GetClonePointer(){return _vpos;}
  void     SetClonePointer(int vpos=-1){ _vpos=vpos;}
  bool     PointerNotSet()const {return _vpos<0;}
  virtual void resethash(integer id, AMSlink *head){};
  virtual AMSID crgid(integer i=0){return AMSID();}
  //! virtual operator that must be used for ordering
  virtual integer operator < ( AMSlink & o) const;

  integer getpos() const {return _pos;}
  void setpos(integer pos){_pos=pos;}
  //  void * operator new(size_t t, void *p) {return p;}
  //  void  operator delete(size_t t, void *p) {p=0;}
  void * operator new(size_t t) {return UPool.insert(t);}
  void operator delete(void *p)   {UPool.udelete(p);p=0;}
  inline AMSlink*  next(){return _next;}
  //AMSlink*  next(integer & last);
  //! test if next is 0
  virtual integer   testlast();
  friend class AMSContainer;
  //! sets the next pointer to 0
  void resetnext(){_next=0;}

};
#endif
