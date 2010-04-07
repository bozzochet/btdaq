//  $Id: upool.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSUPOOL__
#define __AMSUPOOL__
//
//  Universal  linear memory pool for any/all class(es).
// Do NOT requires redefinition of new/delete operators.
// To use it include the following  line in your .h or .C file:
//#include "upool.h"
//
// There is predefined instance of AMSuPool called UPool(512000), 
// compiled in upool.o
//
// Then to create an element of class T use e.g.
// T* p=new(Upool.insert(sizeof(T))) T(...);
// To delete the element use delete;
//
// The pool can be used also for non-class objects  e.g.
//  void * p= Upool.insert(obj_size);
//  UCOPY(&Obj,p,obj_size/4);
//
// To release and/or delete  linear memory use 
// Upool.erase(k); 
// where k is the residual buffer capacity in bytes ; e.g.
//erase(0) will release then delete all memory; erase(1000000) will release all
//memory then delete the memory after 1000000 bytes.
//
// Instead of UPool you can create and use your own instance of AMSuPool;
//e.g. 
// AMSuPool my_pool(n); // n - initial buffer size in bytes
// then use my_pool as UPool;
//
// Performance measured at cernsp 1.7 sec first then 0.9 sec for 60 mbytes alloc
// prompt new[60 mbytes] 0.5 sec
//
#include "typedefs.h"
#include <stddef.h>
#include <iostream.h>
#include <new.h>
#include <string.h>
class AMSuPoolError{
private:
 char msg[256];
public:
 AMSuPoolError(char * name);
   char * getmessage();
};
class AMSuPool {
 class dlink{
 public:
 integer _length;
 dlink * _prev;
 dlink * _next;
 char * _address;
 dlink():_prev(0),_next(0),_length(0),_address(0){}
 ~dlink(){delete[] _address;_address=0;}
 void _erase(integer &nbl )
 {while(_next)_next->_erase(nbl);if(_prev)_prev->_next=0;nbl--;delete this;}
 };
private: 
 integer * _LRS;
 integer _Count;
 integer _Nblocks;
 integer _Minbl;
 integer _Maxbl;
 integer _Totalbl;
 integer _Nreq;
 void _grow(size_t);
 dlink *_head;
 void *_free;
 integer _lc;  // current length
 integer _size;  // size in bytes
public:
 AMSuPool(integer blsize):_head(0),_free(0),_lc(0),_LRS(0),
  _size(blsize),_Count(0),_Nblocks(0),_Minbl(10000000),_Maxbl(0),_Totalbl(0),
  _Nreq(0){SetLastResort(10000);}
 ~AMSuPool(){erase(0);};
 void Release(integer i){};
  void erase(integer);
  void ReleaseLastResort();
  void SetLastResort(integer i);
 void * insert(size_t); 
 static void StHandler();
 void udelete(void *p){
#ifdef __AMSDEBUG__
 _Count--;
#endif
 }
 };

#ifdef __UPOOL__
extern AMSuPool UPool;
#endif
extern AMSuPool RPool;
extern AMSuPool PPool;


#endif
