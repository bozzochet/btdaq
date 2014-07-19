//  $Id: pool.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <pool.h>
#include <cern.h>
#include <iostream.h>
class test: public AMSlink{
float a;
double b[2];
char c;
public:
int d;
  test(int _d=0):d(_d){};
  void * operator new(size_t t) {return AMSPool<test>::insert();}
  void operator delete(void *p){AMSPool<test>::udelete(p);}
};
AMSlink ** AMSPool<test>::_iadd=0;
AMSlink * AMSPool<test>::_head=0;
integer AMSPool<test>::_nblocks=0;
integer AMSPool<test>::_nelem=1000;
integer AMSPool<test>::_nchunk=100;
integer AMSPool<test>::_esize=sizeof(test);
main(){
float st1,st2,st3;
test *vt;
integer i,k;
for(;;){
cout <<" nu poneslasx test i,k"<<endl;
cin >> i >> k;
test first;
TIMEX(st1);
AMSlink *cur=&first;
for(int j=0;j<i;j++){
   cur->_next=new test(j);
   cur=cur->_next;
}
TIMEX(st2);
delete cur;
AMSPool<test>::erase(k);
TIMEX(st3);
cur =&first;
double sum;
sum=0;
while(cur){
sum=sum+((test*)cur)->d;
cur=cur->_next;
}
cout << "other test "<<st2-st1<<" " <<st3-st1<<" "<<sum<<endl;
integer ii;
cin >> ii;
TIMEX(st1);
vt=new test[ii];
TIMEX(st2);
cout <<"opa "<<st2-st1<<endl;
delete[] vt;
}
return 0;
}
