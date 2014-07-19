//  $Id: mnoj.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <iostream.h>
 
class A
{ public:
  virtual void function() { cout << "Class A"; }
};
class B : public A
{ public:
  void function() { cout << "Class B"; }
};
class C : public A
{ public:
  void function() { cout << "Class C"; }
};
class D : public B, public C
{ public:
  void function() { cout << "Class D"; }
};
ostream& another_function(ostream &s, A * ptr)
{ ptr -> function(); return s; }
 
void main()
{ D *p = new D;
if (p) { cout = another_function(cout, p); delete p; }
return 0;
}
 
