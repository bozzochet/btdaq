//  $Id: upool.C,v 1.2 2008/11/14 10:03:54 haino Exp $
// Author V. Choutko 24-may-1996
 
#include "upool.h"
#ifdef __UPOOL__
AMSuPool UPool(512000);
#endif
AMSuPool RPool(512000);
AMSuPool PPool(512000);
    
void * AMSuPool::insert(size_t st){
  union ALIGN{void*p; long l; long double d;};
  // Align request
  st=st+sizeof(ALIGN)-1;
  st-=st%sizeof(ALIGN);
#ifdef __AMSDEBUG__
  _Count++;
#endif
  if(_free==0 || (int)(_lc+st) > _head->_length  ){
    AMSuPool::_grow(st); 
      
  }
  if(_free){
    char * p=(char*)_free; 
    _free=(void*)(p+st);
    _lc=_lc+st;
    //cout<< "insert "<<_lc<<" "<<st <<endl;
    return p; }
  else{
    _Count--; 
    cerr <<" AMSuPool-F-Memory exhausted: Was "<<(_Nblocks-1)*_size<<
      " Requested "<<st<<" bytes"<<endl;
    throw AMSuPoolError("AMSuPool-F-Memory exhausted");
    return 0;
  }
}
void AMSuPool::_grow(size_t st){
  _Nblocks++;
#ifdef __AMSDEBUG__
#endif 
  if(_head==0) {
    _Nreq++;
    _head =new dlink();
    _head->_length=_size > (int)st ? st*(_size/st): st; 
    _head->_address = new char[_head->_length];
    _free=(void*)(_head->_address);
    _lc=0;
    //cout <<" grow 0 "<<_head->_length<<endl;
  }
  else if(_head->_next==0){
    _Nreq++;
    _head->_next=new dlink;
    (_head->_next)->_prev=_head;
    _head=_head->_next;   
    _head->_length=_size > (int)st ? st*(_size/st): st;
    _head->_address = new char[_head->_length];
    _free=(void*)(_head->_address);
    _lc=0;
    //cout <<" grow 1 "<<_head->_length<<endl;
  }
  else{
    _head=_head->_next;
    //cout <<" grow 2 "<<_head->_length<<endl;
    _free=(void*)(_head->_address);
    _lc=0; 
  }
}
void AMSuPool::erase(integer tol){
  _Totalbl+=_Nreq;
  if(_Minbl>_Nreq)_Minbl=_Nreq;
  if(_Maxbl<_Nreq)_Maxbl=_Nreq;
  _Nreq=0;
 
  if(tol && _Count)cerr <<"AMSuPool::erase-S-Objects-Exist "<<_Count<<endl;
  if(_head){
    tol=(tol+_size/2)/_size;
    while(_head->_prev)_head=_head->_prev;       
    dlink *gl=_head;
    for(int i=0;i<tol;i++){
      if(_head){
	_head=_head->_next;
      }
    }              
    if(_head==gl){
      _head->_erase(_Nblocks);
      _head=0;
      _free=0;
#ifdef __AMSDEBUG__
      cout << "Upool Statistics: Min request # "<< _Minbl<<" Max "<<_Maxbl<<
	" Total "<<_Totalbl<<endl; 
#endif         
    }
    else{
      if(_head)_head->_erase(_Nblocks);
      _head=gl;
      _free=(void*)(_head->_address);
    }
  }
  _lc=0;
}

AMSuPoolError::AMSuPoolError(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
}

char * AMSuPoolError::getmessage(){return msg;}


void AMSuPool::SetLastResort(integer i){
  // sets back some memoryheap
}

void AMSuPool::ReleaseLastResort(){
  // Release  memory setted by SetLastResort
}


void AMSuPool::StHandler(){
  cerr << "AMSuPool::StHandler-E-NoMemory"<<endl;
}
