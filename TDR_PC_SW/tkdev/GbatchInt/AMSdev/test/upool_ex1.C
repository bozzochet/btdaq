//  $Id: upool_ex1.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <link.h>
#include <upool.h>
#include <cern.h>
#include <iostream.h>
#include <new.h>
class test: public AMSlink{
float a;
double b[2];
char c;
  void _printEl(){}
void _copyEl(){}
  void _writeEl(){}
public:
int d;
  test(int _d=0):d(_d){};
  //  void * operator new(size_t t, void *p) {return p;}
  //  void * operator new(size_t t){return ::operator new(t);}
  virtual ~test(){}
};
class test2: public test{
public:
integer a[20];
  test2(int _d=0):test(_d){};
  ~test2(){cout <<"opa"<<endl;}
};
AMSuPool CPool(2000);
main(){
float st1,st2,st3;
test *vt;
test2 *vt2;
integer i,k;
integer isize=10000;
for(;;){
  cout <<" nu poneslasx enter i k"<<endl;
cin >> i >> k;
test first;
TIMEX(st1);
AMSlink *cur=&first;
int j,it;
it=0;
try{



for(;;){
for( j=0;j<599;j++){
   cur->_next=new(CPool.insert(sizeof(test))) test(it);
   cur=cur->_next;
   //   cout <<" main "<<it<<endl;
   it++;
}
for( j=0;j<599;j++){
   cur->_next=new(CPool.insert(sizeof(test2))) test2(it);
   cur=cur->_next;
   //   cout <<" main "<<it<<endl;
   it++;
}
if (it>i)break;
}
void *p=CPool.insert(isize);
cur->_next=new test2(it);
}
catch (AMSuPoolError e){
 cerr << e.getmessage()<<endl;
 return 1;
}

TIMEX(st2);
delete cur->_next;
cur->_next=0;
CPool.erase(k);
TIMEX(st3);
cur =&first;
double sum;
integer cit=0;
sum=0;
while(cur){
sum=sum+((test*)cur)->d;
cur=cur->_next;
cit++;
}
cout << " test "<<st2-st1<<" " <<st3-st1<<" "<<sum<<" "<<cit<<endl;
}
return 0;
}
