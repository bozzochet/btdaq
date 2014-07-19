//  $Id: cont.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSCONTAINER__
#define __AMSCONTAINER__
#include "typedefs.h"
#include "link.h"
#include "node.h"
#include "upool.h"

typedef integer  (*pBuilder)(integer refit);
class AMSContainer: public AMSNode{
private:
pBuilder _pb;
integer _BuildSuccess;
AMSlink *_Head;
AMSlink *_Last;
integer _nelem;   // number of elements in containers
integer _sorted;   // For Nevsky 
void _init(){};
void _printEl(ostream & stream){stream <<_name<<" "<<_id<<" Elements: "<<_nelem<<endl;}
public:
integer buildOK(){ return _BuildSuccess ;}
void setbuilder(pBuilder pb){_pb=pb;}
 void runbuilder(integer par);
//void * operator new(size_t t, void *p) {return p;}
void * operator new(size_t t) {return UPool.insert(t);}
void operator delete(void *p)
  {UPool.udelete(p);p=0;}

AMSContainer(AMSID id,integer nelem=0):
AMSNode(id),_nelem(nelem),_BuildSuccess(0),_pb(0),
_Last(0),_Head(0),_sorted(0){}
AMSContainer(AMSID id,pBuilder pb,integer nelem=0):
AMSNode(id),_nelem(nelem),_BuildSuccess(0),_pb(pb),
_Last(0),_Head(0),_sorted(0){}
void eraseC();
void printC(ostream & stream){_printEl(stream);for( AMSlink *ptr=_Head;ptr;ptr=ptr->_next)ptr->_printEl(stream);}
void writeC(){for(AMSlink *ptr=_Head;ptr;ptr=ptr->_next)ptr->_writeEl();}
void copyC(){for(AMSlink *ptr=_Head;ptr;ptr=ptr->_next)ptr->_copyEl();}
void addnext(AMSlink* ptr);
void replaceEl(AMSlink* ptr, AMSlink* prev);
void removeEl(AMSlink* prev, integer restore=1);
void exchangeEl(AMSlink* pel1, AMSlink *pel2);
AMSlink* gethead()const {return _Head;}
AMSlink* getlast()const {return _Last;}
void sethead(AMSlink *head){ 
 _Head=head;
 _Last=head;
 if(_Head){
  _nelem=1;
  while(_Last->next()){
   _nelem++;
   _Last=_Last->next();
  }
 }
 else _nelem=0;
}

integer getnelem() const{return _nelem;}
void sort(int forced);
integer& setnelem() {return _nelem;}
~AMSContainer(){eraseC();}
};
#endif
