//  $Id: utpool.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
//
// Template for linear memory pool for class T.
// Derived from T classes CAN use the new/delete operators from  T.
// To use template include the following three lines in your class <T> .h file:
//#include "utpool.h"
// void * operator new(size_t t) {return AMSutPool<T>::insert(t);}
// void operator delete(void *p){AMSutPool<T>::udelete(p);}
//
// ...and following four lines in any  .C file:  
//  AMSutPool<T>::dlink * AMSutPool<T>::_head=0;
// integer AMSutPool<T>::_nelem=512;//buffer length in sizeof(T), may be changed 
// T * AMSutPool<T>::_free=0;
// integer AMSutPool<T>::_lc=0;
//
// To delete class instances  with non-trivial destructor use delete operator;
// N.B. AMSutPool.udelete DOES NOT release linear memory at all.
// To release and/or delete  linear memory use 
// AMSutPool<T>::erase(k); 
// where k is the residual buffer capacity in sizeof(T); e.g.
//erase(0) will release then delete all memory; erase(1000000) will release
//then delete the memory after 1000000 element.
//
// Performance measured at cernsp 1.8 sec first then 1.0 sec for 60 mbytes alloc
// prompt new[60 mbytes] 0.5 sec
//
#include "typedefs.h" 
#include <stddef.h>
 template <class T> 
class AMSutPool {
private: 
 static void _grow(size_t);
public:
 class dlink{
 public:
 integer _length;
 dlink * _prev;
 dlink * _next;
 void * _address;
 dlink():_prev(0),_next(0),_length(0),_address(0){}
 ~dlink(){delete[] _address;_address=0;}
 void _erase()
 {while(_next)_next->_erase();if(_prev)_prev->_next=0;delete this;}
};
 static dlink *_head;

 static T *_free;
 static integer _lc;  // current length
 static integer _nelem; // number of elem
 ~AMSutPool(){AMSutPool<T>::erase(0);};
 static void erase(integer);
 static void * insert(size_t); 
 static void udelete(void*); 
 };

template <class T>
void * AMSutPool<T>::insert(size_t st){
if(_free==0 || _lc+st > _head->_length  )AMSutPool<T>::_grow(st); 
 if(_free){
 char * p=(char*)_free; 
 _free=(T*)(p+st);
 _lc=_lc+st;
//cout<< "insert "<<_lc<<" "<<st <<endl;
 return p; }
 else{
   cerr <<" AMSutPool<T>-F-Memory exhausted"<<endl;
 return 0;
 }
}
template <class T>
void AMSutPool<T>::udelete(void *b){
// only call destructor for object
 if(b)((T*)b)->~T();
  b=0;
}
template <class T> 
 void AMSutPool<T>::_grow(size_t st){
  if(_head==0) {
    
     _head =new dlink();
     _head->_length=_nelem*st;
     _head->_address = new char[_head->_length];
     _free=(T*)(_head->_address);
     _lc=0;
   //cout <<" grow 0 "<<_head->_length<<endl;
  }
  else if(_head->_next==0){
   _head->_next=new dlink;
   (_head->_next)->_prev=_head;
   _head=_head->_next;   
     _head->_length=_nelem*st;
     _head->_address = new char[_head->_length];
     _free=(T*)(_head->_address);
     _lc=0;
   //cout <<" grow 1 "<<_head->_length<<endl;
  }
  else{
     _head=_head->_next;
     //cout <<" grow 2 "<<_head->_length<<endl;
     _free=(T*)(_head->_address);
     _lc=0; 
  }
 }
template <class T>
   void AMSutPool<T>::erase(integer tol){
     if(_head){
       tol=(tol+_nelem/2)/_nelem;
       while(_head->_prev)_head=_head->_prev;       
       dlink *gl=_head;
       for(int i=0;i<tol;i++)if(_head)_head=_head->_next;              
       if(_head==gl){
         _head->_erase();
         _head=0;
         _free=0;
       }
       else{
        if(_head)_head->_erase();
        _head=gl;
        _free=(T*)(_head->_address);
       }
     }
     _lc=0;
   }
