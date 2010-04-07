//  $Id: node.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
// 
// September 03, 1996. ak. add set pointers functions
//
#ifndef __AMSNODE__
#define __AMSNODE__
#include "typedefs.h"
#include "id.h"
#include "astring.h"
class AMSNode : public AMSID{
// Simple Node Class V.Choutko 3/20/96
 protected:
  char * _message;
  AMSNode *_next;
  AMSNode *_prev;
  AMSNode *_up;
  AMSNode *_down;
  AMSNode(AMSID _amsid=0):
          AMSID(_amsid),_up(0),_down(0),_prev(0),_next(0),_message(0){}
 private:
  AMSNode( AMSNode &):AMSID(0){}  // do not have copy ctor at the moment
  //AMSNode & operator =(const AMSNode & o){return *this;}
 public:
 const char * getMessage()const {return (const char *)_message;}
 void setMessage(const char * message);
 virtual ~AMSNode(){if(_message)delete[] _message;_message=0;}
  AMSNode *  add(  AMSNode *);
  AMSNode * addup( AMSNode *);   // add at virtual top
  void  remove();
  virtual void _init()=0;
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
