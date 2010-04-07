//  $Id: throw.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <iostream.h>
#include <stdio.h>

#include <stdlib.h>
class AMSTrTrackError{
private:
 char msg[256];
public:
 AMSTrTrackError(char * name);
 char * getmessage();
};
AMSTrTrackError::AMSTrTrackError(char * name){
  if(name){
    int n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
}
char * AMSTrTrackError::getmessage(){return msg;}

typedef int  (*pBuilder)(int refit);
class B{
 pBuilder _pb;
int _i;
public:
 void run(){
   _pb(_i);
 }
  B(pBuilder pb, int i):_pb(pb),_i(i){};
};

class A{
public:
 static int exc(int i);
  static int rec(int i){
   if(i>0){
    cout <<" gogo i"<<i<<endl;
    throw AMSTrTrackError(" cpu time jipa");
   }
   else return rec(i+1);
  }
};

int A::exc(int i)  {
    rec(i);
}

main(){
int i;
for(;;){
i=-50;
B b(&A::exc,-50);
  try{
b.run();
}
catch (int i){
  cerr <<" This version of icc is OK"<<endl;
}
catch (AMSTrTrackError e){
  cerr <<e.getmessage()<<endl;
}
exit(1);
}
return 0;
}
