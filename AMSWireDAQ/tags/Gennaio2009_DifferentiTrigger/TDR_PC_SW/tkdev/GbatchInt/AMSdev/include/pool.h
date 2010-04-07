//  $Id: pool.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
//
// Template for linear memory pool for class T
// In the contrary to utpool.h, 
// EVERY derived from T class SHOULD HAS its own new/delete operators!!!!
// To use template include the following three lines in your class <T> .h file:
//#include "pool.h"
// void * operator new(size_t t) {return AMSPool<T>::insert();}
// void operator delete(void *p){AMSPool<T>::udelete(p);}
//
// ...and following six lines in any  .C file:
// AMSlink ** AMSPool<T>::_iadd=0;
// AMSlink *  AMSPool<T>::_head=0;
// integer    AMSPool<T>::_nblocks=0;
// integer  AMSPool<T>::_nelem=512; //buffer size in sizeof(T), may be changed
// integer    AMSPool<T>::_nchunk=10;
// integer    AMSPool<T>::_esize=sizeof(T);
//
// To delete class instances  with non-trivial destructor use delete operator;
// N.B. AMSutPool.udelete DOES release linear memory also.
// To  delete  linear memory use
// AMSPool<T>::erase(k);
// where k is the residual buffer capacity in sizeof(T); e.g.
// erase(0) will delete all memory; erase(1000000) will 
// delete the memory after 1000000 element.
// 
// pool.h has about same performance as utpool.h
//
#include "link.h"
 template <class T> 
class AMSPool {
private: 
 static void _grow();
public:
 static AMSlink** _iadd;
 static AMSlink *_head;
 static integer _esize;
 static integer _nblocks;
 static integer _nelem;
 static integer _nchunk;
 AMSPool(){};
 ~AMSPool(){AMSPool<T>::erase(0);};
 static void erase(integer);
 static void * insert(); 
 static void udelete(void*); 
 };

template <class T>
void * AMSPool<T>::insert(){
 if(_head==0)AMSPool<T>::_grow(); 
 if(_head){
 AMSlink *p=_head; 
 _head=p->_next; 
 return p; }
 else{
   cerr <<" Alloc<T>-F-Memory exhausted"<<endl;
 return 0;
 }
}
template <class T>
void AMSPool<T>::udelete(void *b){
 ((T*)b)->~T();
AMSlink *p=(AMSlink *)b;
 p->_next=_head; 
 _head=p;
}
template <class T> 
 void AMSPool<T>::_grow(){
  char *start =new char[_nelem*_esize];
  if( start){
 char* last= start+(_nelem-1)*_esize;
 for(char *p=start;p<last;p+=_esize)((AMSlink*)p)->_next=(AMSlink*)(p+_esize);
((AMSlink*)last)->_next=0; 
_head=(AMSlink*)start;
if(_nblocks%_nchunk==0)
_iadd=(AMSlink**)realloc(_iadd,(_nblocks+_nchunk)*sizeof(AMSlink*));
_iadd[_nblocks++]=(AMSlink*)start;
}
 }

template <class T>
   void AMSPool<T>::erase(integer tol){
integer i;
tol=(tol+_nelem/2)/_nelem;
for (i=tol;i<_nblocks;i++)delete[] (char*)(_iadd[i]);
_nblocks=_nblocks>tol?tol:_nblocks;
if(_nblocks>0){
_iadd=(AMSlink**)realloc(_iadd,_nblocks*_nchunk*sizeof(AMSlink*));
_head=_iadd[0];
for(i=0;i<_nblocks;i++){
 char *last=(char*)(_iadd[i])+(_nelem-1)*_esize;
 for (char *p=(char*)(_iadd[i]);p<last;p+=_esize){
  ((AMSlink*)p)->_next=(AMSlink*)(p+_esize);
   if(i+1<_nblocks)((AMSlink*)last)->_next=_iadd[i+1];
   else ((AMSlink*)last)->_next=0;
 }
}
}
else{
free(_iadd);
_iadd=0;
_head=0;
}
}
