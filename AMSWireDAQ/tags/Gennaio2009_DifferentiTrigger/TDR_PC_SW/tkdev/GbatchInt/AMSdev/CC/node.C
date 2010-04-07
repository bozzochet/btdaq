//  $Id: node.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "typedefs.h"
#include "node.h"
 AMSNode * AMSNode::add(  AMSNode *ptr){
//
// Add new node
//
  AMSNode *cur;
  cur=_down;
  if(cur){
   while(cur->_next)cur=cur->_next;
   cur->_next=ptr;
   ptr->_prev=cur;
  }
  else{
   _down=ptr;
   ptr->_prev=0;
  }
   ptr->_up=this;
   ptr->_init();
   return ptr;
 } 
 AMSNode * AMSNode::addup(  AMSNode *ptr){
//
// Add new node on virtual top
//
  AMSNode *cur;
  cur=_down;
  if(cur){
   while(cur->_next)cur=cur->_next;
   cur->_next=ptr;
   ptr->_prev=cur;
  }
  else{
   _down=ptr;
   ptr->_prev=0;
  }
   ptr->_up=0;
   //   ptr->_init();
   return ptr;
 } 
 void  AMSNode::remove(){
//
// Remove itself + all daughters
// if not at the top
//
  while (_down){
   _down->remove();
  } 
  if(_down==0 && _up){ 
     if(_prev){
      _prev->_next=_next;
      if(_next)_next->_prev=_prev;
     }
     else {
      _up->_down=_next;
       if(_next)_next->_prev=0;
     }
     delete this;
  }
 }

void AMSNode::printN(ostream & stream, int i){
    stream<<*this;
    if(down())down()->printN(stream,1);
    if(i && next())next()->printN(stream,1);
}


void AMSNode::setMessage(const char * message){
if(message){
if(_message)delete[] _message;
_message=new char[strlen(message)+1];
strcpy(_message,message);
}
}
