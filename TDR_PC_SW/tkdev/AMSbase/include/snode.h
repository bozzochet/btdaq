//  $Id: snode.h,v 1.2 2008/07/01 13:22:09 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSNODEMAP__
#define __AMSNODEMAP__
#include "typedefs.h"
#include "upool.h"
#include "node.h"

//! Class implementing a map of the AMSNodes with Binary search functions
class AMSNodeMap 
{
 protected:
 
  //! Pointer to an array of AMSNode(s)
  AMSNode ** _hash;
  //! Root of the tree of AMSnode(s) to be mapped
  AMSNode *_mother;
  //! Number of elements in the tree
  integer _numo;   // number of elements
  //! size of the _hash array
  integer _hsize; // _hash size

  static const integer _tols;

  //! Recoursevly go into the tree hanging from ptr to count the elements
  void _count(AMSNode *const ptr);
  //! Recoursevely go into the tree hanging from ptr to build the array (_hash) of pointers to AMSNode(s)
  void _rehash(AMSNode *const ptr);


 public:
  static integer debug; 
  //! std constructor
  AMSNodeMap(): _hash(0), _mother(0),_numo(0),_hsize(0) {}
  //! std destructor
  virtual ~AMSNodeMap(){delete[] _hash;}
  //! Build the map of the AMSNode(s) hanging below mother
  void map ( AMSNode  &  mother){_mother=&mother;remap();}
  //! Rebuild  the map of the AMSNode(s)
  void remap(); 
  //! Reset the map 
  void unmap(); 
  //! Add an element to the map (It is ever used ???) (but not to the tree??) 
  //! if returns -1: such element  already exists
  integer add( AMSNode&);
  //! prints out all the AMSNode(s) pointed by the array _hash
  void print(int init=0); 
  //! Returns the pointer to  the AMSNode with AMSID id
  AMSNode * getp  (  const AMSID& id,  char hint='\0' ) const ;
  //! Returns the position in the array of the AMSNode with AMSID id
  integer getpos  (  const AMSID& id,  char hint='\0' ) const ;

  //! returns the id-th element of the array _hash (vector like access)
  //! the consistency of the id is checked against the array boundaries
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
