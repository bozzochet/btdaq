//  $Id: utpool.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <link.h>
#include <utpool.h>
#include <cern.h>
#include <iostream.h>
class test: public AMSlink{
float a;
double b[2];
char c;
public:
int d;
  test(int _d=0):d(_d){};
  void * operator new(size_t t) {return AMSutPool<test>::insert(t);}
  void operator delete(void *p){AMSutPool<test>::udelete(p);}
  virtual ~test(){}
};
class test2: public test{
public:
integer a[20];
  test2(int _d=0):test(_d){};
  ~test2(){cout <<"opa"<<endl;}
};
AMSutPool<test>::dlink * AMSutPool<test>::_head=0;
integer AMSutPool<test>::_nelem=512;
test * AMSutPool<test>::_free=0;
integer AMSutPool<test>::_lc=0;
main(){
float st1,st2,st3;
test *vt;
test2 *vt2;
integer i,k;
for(;;){
  cout <<" nu poneslasx test i,k"<<endl;
cin >> i >> k;
test first;
TIMEX(st1);
AMSlink *cur=&first;
int j,it;
it=0;
for(;;){
for( j=0;j<599;j++){
   cur->_next=new test(it);
   cur=cur->_next;
   //   cout <<" main "<<it<<endl;
   it++;
}
for( j=0;j<599;j++){
   cur->_next=new test2(it);
   cur=cur->_next;
   //   cout <<" main "<<it<<endl;
   it++;
}
if (it>i)break;
}
cur->_next=new test2(it);
TIMEX(st2);
delete cur->_next;
cur->_next=0;
AMSutPool<test>::erase(k);
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
cout << "other test "<<st2-st1<<" " <<st3-st1<<" "<<sum<<" "<<cit<<endl;
integer ii;
cin >> ii;
TIMEX(st1);
vt=new test[ii/2];
vt2=new test2[ii/2];
TIMEX(st2);
cout <<"opa "<<st2-st1<<endl;
delete[] vt;
delete[] vt2;
}
return 0;
}
