//  $Id: snode.C,v 1.2 2008/07/01 13:22:09 zuccon Exp $
// Author V. Choutko 24-may-1996

#include <typeinfo> 
#include "node.h"
#include "snode.h"
#include "amsstl.h"
integer AMSNodeMap::debug=1;
const integer AMSNodeMap::_tols=10000;

void AMSNodeMap::_count(AMSNode *const  ptr){
  AMSNode *cur;
  cur=ptr;
  while(cur){    
    _numo++;
    _count(cur->down());
    cur=cur->next();
  }
} 

void AMSNodeMap::_rehash(AMSNode *const ptr){
  AMSNode *cur;
  cur=ptr;
  while(cur){  
    _hash[_numo++]=cur;
    _rehash(cur->down());
    cur=cur->next();
  }
} 

void AMSNodeMap::unmap(){
  _numo=0;
  if(_hash){
    if(_hsize>_tols){
      delete[] _hash;
      _hash=0;
      _hsize=0;
    }
  }
  _mother=0;
}

void AMSNodeMap::remap(){
  _numo=0;
  _count(_mother);
  if(_numo>0){
    if(_numo>_hsize){
      if(_hash)delete[] _hash;
      try{
	_hash=new AMSNode*[_numo];
      }
      catch(bad_alloc aba){
	_hash=0;
      }
      if(!_hash){
	if(AMSNodeMap::debug)cerr <<"AMSNodeMap::remap-F-Can not allocate " <<_numo
				  <<" words"<<endl;
	throw AMSNodeMapError("AMSNodeMap::remap-F-CNNTALLMEM");
      }
    }
   
    _hsize=_numo;
    _numo=0;
    _rehash(_mother);
    AMSsortNAG(_hash,_numo); 
    //    AMSsort(_hash,_numo); 
    //        Check for duplicalte elements
    int nel;
    for(nel=1;nel<_numo;nel++){
      if((typeid(*(_hash[nel]))==typeid(*(_hash[nel-1]))) && *(_hash[nel])==*(_hash[nel-1])){
	cerr <<"AMSNodeMap::remap-E-duplicate element found "<<*(_hash[nel])
	     <<endl;
      } 
    }
  }
   
} 

integer AMSNodeMap::add(AMSNode & o){
  integer i;
  i=AMSbi(_hash,o,_numo);
  if(i==-1){
    if(AMSNodeMap::debug)
      cerr << "AMSNodeMap::add-E-Element "<<o <<" already exists."<<endl;
    return i;
  }
  else {
    if(_hsize<=_numo){
      int size=++_numo;
      AMSNode ** tmp;
      do {
        _hsize=_numo+size;
	try{
	  tmp=new AMSNode*[_hsize];
	}
	catch(bad_alloc aba){
	  tmp=0;
	}
        size=(size/2>1)?size/2:1;
      }while (!tmp && size>1);
      if(!tmp){
        if(AMSNodeMap::debug)cerr <<"AMSNodeMap::add-F-Can not allocate " <<size
				  <<" words"<<endl;
        throw AMSNodeMapError("AMSNodeMap::add-F-CNNTALLMEM");
      }
      //UCOPY(_hash,tmp,sizeof(_hash[0])*i/4);
      int j;
      for ( j=0;j<i;j++)tmp[j]=_hash[j];
      tmp[i]=&o;
      //UCOPY(_hash+i,tmp+i+1,sizeof(_hash[0])*(_numo-i)/4);
      for (j=i+1;j<_numo;j++)tmp[j]=_hash[j-1];
      delete[] _hash;
      _hash=tmp;
        
    }
    else {
      //        UCOPY2(_hash+i,_hash+i+1,sizeof(_hash[0])*(_numo++-i)/4);
      for (int j=_numo++;j>i;j--)_hash[j]=_hash[j-1];
      _hash[i]=&o;
    }
  }
  return 0;
} 


AMSNode * AMSNodeMap::getp  (  const AMSID& id, char hint ) const {

   
  integer i= AMSbsi((AMSID**)_hash,id,_numo, hint);

  if(i<0){
    //       cerr <<"AMSNodeMap::getp-S-PointerNotFound "<<id<<endl;
    return 0;
  }
  else return _hash[i];
}

integer AMSNodeMap::getpos  (  const AMSID& id, char hint ) const {
   
  return AMSbsi((AMSID**)_hash,id,_numo, hint);
}


void AMSNodeMap::print (int init){
  AMSNode * cur;
  for (int i=init;;){
    cur=getid(i++);   // get one by one
    if(cur)cout << *cur;
    else break;
  }
}
