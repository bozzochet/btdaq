//  $Id: cont.C,v 1.3 2008/07/02 14:22:13 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "cont.h"
#include "amsstl.h"
void AMSContainer::runbuilder(integer par){
  if(_pb)_BuildSuccess=_pb(par);
#ifdef __AMSDEBUG__
  else cerr <<"AMSContainer::runbuilder-E-NoBuilderFound for "<<getname()<<" "<<getid()<<endl;
#endif 
}


AMSlink*  AMSContainer::at(int ii){
  if (ii<0|| ii>getnelem() ) return 0;
  if(!_Head) return 0;
  AMSlink* pp=_Head;
  int count=0;
  while (pp && count!=ii){
    pp=pp->next(); 
    count++;
  }
  if(count==ii) return pp;
  else return 0;
}

void AMSContainer::addnext(AMSlink *ptr){
  if(_Last){
    _Last->_next=ptr;
    _Last=_Last->_next;
    _Last->_next=0;
  }
  else _Head=_Last=ptr;
  _nelem++;
  _Last->setpos(_nelem);
}

void AMSContainer::replaceEl(AMSlink *ptr, AMSlink *prev){
  if(prev){
    if(prev->_next == _Last)_Last=ptr;
    else ptr->_next=(prev->_next)->_next; 
    ptr->setpos((prev->_next)->getpos());
    delete prev->_next;
    prev->_next=ptr;
  }
  else {
    if(_Head==_Last){
      _Last=ptr;
      _Head=ptr;
      ptr->_next=0;
    }
    else {
      ptr->_next=_Head->_next;
      delete _Head;
      _Head=ptr;
    }
    ptr->setpos(1);
  }
}

void AMSContainer::exchangeEl(AMSlink *pr1, AMSlink *pr2){
  AMSlink* pel1;
  AMSlink* pel2;
  AMSlink* tmp;
  if(pr1)pel1=pr1->_next;
  else pel1=_Head;
  if(pr2)pel2=pr2->_next;
  else pel2=_Head;
  if(pel1 !=pel2 && pel1 && pel2){
    integer ip1=pel1->getpos();
    integer ip2=pel2->getpos();
    if(pr1)pr1->_next=pel2;
    else _Head=pel2;
    tmp=pel2->_next;
    pel2->_next=pel1->_next; 
    if(pr2)pr2->_next=pel1;
    else _Head=pel1;
    pel1->_next=tmp;
    pel1->setpos(ip2);      
    pel2->setpos(ip1);      
    if(pel1==_Last)_Last=pel2;
    else if (pel2==_Last)_Last=pel1;
  }
}

void AMSContainer::removeEl( AMSlink *prev, integer restore){
  // remove next element !!!

  AMSlink * pel=prev != 0?prev->_next:_Head;
#ifdef __AMSDEBUG__
  assert(pel != NULL);
#endif
  if(pel == _Last){
    if(prev)prev->_next=0;
    else _Head=0;
    _Last=prev;
  }
  else{
    if(prev)prev->_next=pel->_next;
    else _Head=pel->_next;
    if(restore){
      AMSlink * ptmp=pel->_next;
      while(ptmp){
	ptmp->setpos(ptmp->getpos()-1);
	ptmp=ptmp->_next;
      }
    }
  }
  _nelem--;
  delete pel;
}


void AMSContainer::sort(int forced){
  if((!forced && _sorted) || (_nelem < 2) )return;
  _sorted=1;
  AMSlink ** arr=(AMSlink **)UPool.insert(sizeof(AMSlink*)*_nelem);
  int i;
  arr[0]=_Head;
  for(i=1;i<_nelem;i++){
    arr[i]=arr[i-1]->_next;
  }
  AMSsortNAG(arr,_nelem);
  _Head=arr[0];
  _Head->setpos(1);
  _Last=arr[_nelem-1];
  _Last->_next=0;
  for(i=1;i<_nelem;i++){
    arr[i-1]->_next=arr[i];
    arr[i]->setpos(i+1);
  }
  UPool.udelete(arr);
}



void AMSContainer::eraseC(){
  //
  // Deletes all cont elements
  // 
  AMSlink *next=0;
  for(AMSlink *ptr=_Head;ptr;ptr=next){
    next=ptr->_next;
    delete ptr;
  }
  _BuildSuccess=0;
  _Head=_Last=0;
  _nelem=0;
}
