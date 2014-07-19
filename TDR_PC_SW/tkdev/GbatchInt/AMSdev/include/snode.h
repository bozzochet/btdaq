//  $Id: snode.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSNODEMAP__
#define __AMSNODEMAP__
#include "typedefs.h"
#include "upool.h"
#include "node.h"
class AMSNodeMap 
{
 protected:
  
  integer _numo;   // number of elements
  integer _hsize; // _hash size
  AMSNode ** _hash;
  AMSNode *_mother;
  void _count(AMSNode *const ptr);
  void _rehash(AMSNode *const ptr);
  static const integer _tols;
 public:
  static integer debug; 
  AMSNodeMap(): _hash(0), _mother(0),_numo(0),_hsize(0) {}
  virtual ~AMSNodeMap(){delete[] _hash;}
  void map ( AMSNode  &  mother){_mother=&mother;remap();}
  void remap(); 
  void unmap(); 
  integer add( AMSNode&);  // return -1: such element  already exists
  void print(int init=0); 
  AMSNode * getp  (  const AMSID& id,  char hint='\0' ) const ;
  integer getpos  (  const AMSID& id,  char hint='\0' ) const ;
  AMSNode * getid  ( const integer id ) const 
   {return id<_numo && id>-1? _hash[id]:0;}
};





class AMSNodeMapError{
private:
 char msg[256];
public:
 AMSNodeMapError(char * name){
   if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
   }
 }
 char * getmessage(){return msg;}
};




#endif
