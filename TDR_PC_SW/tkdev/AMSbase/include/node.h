//  $Id: node.h,v 1.1 2008/06/24 11:17:14 zuccon Exp $
// Author V. Choutko 24-may-1996
// 
// September 03, 1996. ak. add set pointers functions
//
#ifndef __AMSNODE__
#define __AMSNODE__
#include "typedefs.h"
#include "id.h"
#include "astring.h"

//! class that implements the base Node type for the access to AMS objects
class AMSNode : public AMSID{
  // Simple Node Class V.Choutko 3/20/96

protected:
  //! Pointer to the upper node in the hierarchy
  AMSNode *_up;
  //! Pointer to the lower node in the hierarchy 
  AMSNode *_down;
  //! Pointer to the previous node
  AMSNode *_prev;
  //! Pointer to the next  node
  AMSNode *_next;
  //! ???????????????????????
  char * _message;
  //! (protected) constructor
  AMSNode(AMSID _amsid=0):
    AMSID(_amsid),_up(0),_down(0),_prev(0),_next(0),_message(0){}

private:
  //! do not want copy constructor so it is set as private
  AMSNode( AMSNode &):AMSID(0){}  // do not have copy ctor at the moment
  //AMSNode & operator =(const AMSNode & o){return *this;}

public:
  //! std destructor
  virtual ~AMSNode(){if(_message)delete[] _message;_message=0;}

  //! returns the message string
  const char * getMessage()const {return (const char *)_message;}

  //! Sets the message string
  void setMessage(const char * message);

  //! Add a node at the end of the list hanging at the _down link, 
  //!if this list it is not present create it whit the Node pointed by ptr
  //! IMPORTANT this method call internaly ptr->_init()
  AMSNode *  add(  AMSNode * ptr);

  //! Add a node at the end of the list hanging at the _down link, 
  //!if this list it is not present create it whit the Node pointed by ptr
  //! it reset ptr->_up to zero.
  AMSNode * addup( AMSNode *ptr);   // add at virtual top

  //! Remove all nodes attached to _down then if  _up!=0 delete itself
  void  remove();
  //! Virtual method for the self initialization. Called in the add() method.
  virtual void _init()=0;
  //! Print itself, the node pointed by _down, and is (i!=0) the node pointed by _next
  void printN(ostream & stream, int i=0);

  //
  // Node movements
  //

  inline AMSNode * next(){
    return _next;
  }
  inline AMSNode * up(){
    return _up;
  } 
  inline AMSNode * down(){
    return _down;
  } 
  inline AMSNode * prev(){
    return _prev;
  }
  //+
  void setNext(AMSNode* next) { _next = next;}
  void setPrev(AMSNode* prev) { _prev = prev;}
  void setUp(AMSNode* up)     { _up = up;}
  void setDown(AMSNode* down) { _down = down;}
  //-
};


#endif
