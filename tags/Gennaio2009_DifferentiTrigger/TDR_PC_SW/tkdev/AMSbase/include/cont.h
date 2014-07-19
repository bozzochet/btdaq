//  $Id: cont.h,v 1.3 2008/07/02 14:22:13 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSCONTAINER__
#define __AMSCONTAINER__
#include "typedefs.h"
#include "link.h"
#include "node.h"
#include "upool.h"

//! type for object builder function
typedef integer  (*pBuilder)(integer refit);

//! \brief Class implementig the AMS SIM/RECO objects container
//! It containes a double linked list of AMSlink objects
//! gethead() returns the pointer to the first element of the list
//! getlast() returns the pointer to the last element of the list
class AMSContainer: public AMSNode{
private:
  //! Number of elements within the container
  integer _nelem;   // number of elements in containers
  //! Succesful excutuon of the builder function
  integer _BuildSuccess;
  //! Pointer to the object building function
  pBuilder _pb;
  //! pointer to the last object in the (double) linked list
  AMSlink *_Last; 
  //! pointer to the first object in the (double) linked list
  AMSlink *_Head;
  //! is the container sorted
  integer _sorted;   // For Nevsky 
  //! dummy init function (to prevent automatic execution when calling node add)
  void _init(){};
  //! print function
  void _printEl(ostream & stream){stream <<_name<<" "<<_id<<" Elements: "<<_nelem<<endl;}

public:
  //! Vector like access
  AMSlink* at(int ii);
  //! Returnd the BuildSuccess flag
  integer buildOK(){ return _BuildSuccess ;}
  //!  Sets the pointer to the build function
  void setbuilder(pBuilder pb){_pb=pb;}
  //! Runs the Container build function (if any)
  void runbuilder(integer par);
  //void * operator new(size_t t, void *p) {return p;}
  //! UPool based new operator
  void * operator new(size_t t) {return UPool.insert(t);}
  //! UPool based delete operator
  void operator delete(void *p) {UPool.udelete(p);p=0;}

  AMSContainer(AMSID id,integer nelem=0):
    AMSNode(id),_nelem(nelem),_BuildSuccess(0),_pb(0),
    _Last(0),_Head(0),_sorted(0){}
  AMSContainer(AMSID id,pBuilder pb,integer nelem=0):
    AMSNode(id),_nelem(nelem),_BuildSuccess(0),_pb(pb),
    _Last(0),_Head(0),_sorted(0){}

  //! Delete all the elements in the container
  void eraseC();
  //! Dump the container info and calls _printEl for all the contained objects
  void printC(ostream & stream){_printEl(stream);for( AMSlink *ptr=_Head;ptr;ptr=ptr->_next)ptr->_printEl(stream);}
  //! calls _writeEl for all the contained objects
  void writeC(){for(AMSlink *ptr=_Head;ptr;ptr=ptr->_next)ptr->_writeEl();}
  //! calls _copyEl for all the contained objects
  void copyC(){for(AMSlink *ptr=_Head;ptr;ptr=ptr->_next)ptr->_copyEl();}
  //! add an object at the end of the container
  void addnext(AMSlink* ptr);
  //! Replaces the element hanging at prev->next() (which is NOT destroyed)with the element pointed by ptr
  void replaceEl(AMSlink* ptr, AMSlink* prev);
  //! Removes from the container the elememnt hanging at prev->next()(which is NOT destroyed).
  //! If restore is set (default) also the vpos counter within AMSlink objects is updated
  void removeEl(AMSlink* prev, integer restore=1);
  //! exchange the position of two elements within the container
  void exchangeEl(AMSlink* pel1, AMSlink *pel2);
  //! returns the pointer to the first element of the list
  AMSlink* gethead()const {return _Head;}
  //!  returns the pointer to the last element of the list
  AMSlink* getlast()const {return _Last;}
  /*! set the object head as the first element of the list. 
    If other object are atteched(next()) to head they are 
    also added to the container.
  */
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
  //! returns the number of elements in the container (fast)
  integer getnelem() const{return _nelem;}
  //! it sorts the container. If forced is set force the sorting even if the sorted flag is raised
  void sort(int forced);
  integer& setnelem() {return _nelem;}
  //! destructor
  ~AMSContainer(){eraseC();}
};
#endif
