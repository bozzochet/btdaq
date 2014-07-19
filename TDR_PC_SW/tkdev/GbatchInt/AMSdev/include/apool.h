//  $Id: apool.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 19-jul-1996
 
#ifndef __AMSAPOOL__
#define __AMSAPOOL__
//
//  Universal  linear memory pool for any/all class(es).
// Do NOT requires redefinition of new/delete operators.
// To use it include the following  line in your .h or .C file:
//#include "apool.h"
//
// There is predefined instance of AMSaPool called APool(512000), 
// compiled in apool.o
//
// Then to create an element of class T use e.g.
// T* p=new(Apool.insert(sizeof(T))) T(...);
// To delete the element use delete;
//
// The pool can be used also for non-class objects  e.g.
//  void * p= Apool.insert(obj_size);
//  UCOPY(&Obj,p,obj_size/4);
// to delete the elements use udelete(p);
// N.B. AMSaPool.udelete TRIES to release linear memory also
// (not in case of AMSuPool)
//
// To release and/or delete  linear memory use 
// Apool.erase(k); 
// where k is the residual buffer capacity in bytes ; e.g.
//erase(0) will release then delete all memory; erase(1000000) will release all
//memory then delete the memory after 1000000 bytes.
//
// Instead of APool you can create and use your own instance of AMSaPool;
//e.g. 
// AMSaPool my_pool(n); // n - initial buffer size in bytes
// then use my_pool as APool;
//
// Performance measured at cernsp 1.7 sec first then 0.9 sec for 60 mbytes alloc
// prompt new[60 mbytes] 0.5 sec
//
#include "typedefs.h"
#include <stddef.h>
#include <iostream.h>
#include <new.h>
#include <string.h>
#include "node.h"
#include "snode.h"
class AMSNodePool: public AMSNode{
private:
void * _address;
void _init(){};
virtual ostream & print(ostream &) const;  
public:
AMSNodePool():_address(0),AMSNode(){}
AMSNodePool(integer id, void *p):_address(p),AMSNode(id){}
void setaddress(void *p){_address=p;}
void*  getaddress(){return _address;}
};

class AMSNodeMapPool: public AMSNodeMap{
public:
  AMSNodeMapPool():AMSNodeMap(){}
  char * get(integer o);
  void put(  integer * p );
  integer getnum(){return _numo;}
  friend class AMSNodePool;
};


class AMSaPoolError{
private:
 char msg[256];
public:
 AMSaPoolError(char * name);
   char * getmessage();
};
class AMSaPool {
 class dlink{
 public:
 integer _length;
 dlink * _prev;
 dlink * _next;
 char * _address;
 dlink():_prev(0),_next(0),_length(0),_address(0){}
 ~dlink(){delete[] _address;_address=0;}
 void _erase(integer &nbl );
 };
private: 
 integer * _LRS;
 integer _Count;
 integer _Nblocks;
 integer _Minbl;
 integer _Maxbl;
 integer _Totalbl;
 integer _MinNodes;
 integer _MaxNodes;
 integer _TotalNodes;
 integer _Nreq;
 void _grow(size_t);
 dlink *_head;
 void *_free;
 integer _lc;  // current length
 integer _size;  // size in bytes
 static integer _Mask;  // for some protection 
 static integer _Release;
 AMSNodePool poolNode;
 AMSNodeMapPool poolMap;
public:
 AMSaPool(integer blsize);
 ~AMSaPool(){erase(0);};
  void erase(integer);
  void ReleaseLastResort();
  void SetLastResort(integer i);
  void Release(integer r){ _Release=r;}
 void * insert(size_t); 
 void udelete(void *p);
  static void StHandler();
 };


#ifndef __UPOOL__
extern AMSaPool UPool;
#endif
extern AMSaPool APool;



#endif
