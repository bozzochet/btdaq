//  $Id: copy.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <typedefs.h>
#include <iostream.h>
#include <cern.h>
#include <link.h>
integer *pointer=0;
class A :public AMSlink{
private:
integer j1;
number j2;
integer j3;
integer *p4;
public:
  integer getj1(){return j1;}
  number getj2(){return j2;}
  integer getj3(){return j3;}
  A(integer _j1,number _j2, integer _j3): j1(_j1), j2(_j2), j3(_j3),p4(0){}
  void write();
  void _printEl(){}
};
void A::write(){
static integer init=0;
if(init++ == 0){
pointer= new integer[10000];
}
UCOPY(this,pointer, sizeof(A)/4);
}
main (){
A me(11,3.5,170);
me.write();
return 0;
}
