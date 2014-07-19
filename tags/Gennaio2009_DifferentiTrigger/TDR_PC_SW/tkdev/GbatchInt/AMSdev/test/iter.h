//  $Id: iter.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

#include <typedefs.h>
class AMSNodeArr {
// up to 7 dim array with var bounds and random access 
private:
  integer _numo;   // number of elements
  integer _hsize; // _hash size
  AMSNode ** _hash;
  AMSNode *_mother;
  static const integer rank;
  static const integer bsize;
  void _count(AMSNode *const ptr);
  void _rehash(AMSNode *const ptr);
public:
  static integer debug;
  AMSModeIArr(): _hash(0), _mother(0),_numo(0) {}
  ~AMSNodeMap(){delete[] _hash;}
  void map ( AMSNode  &  mother){_mother=&mother;remap();}
  void remap();
  void unmap();
  integer add( AMSNode&);  // return -1: such element is already exist
  void print();
  AMSNode * getp  (  const AMSID& id ) const ;

  };








