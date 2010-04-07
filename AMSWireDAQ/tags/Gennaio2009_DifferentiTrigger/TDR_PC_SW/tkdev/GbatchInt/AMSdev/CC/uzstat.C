//  $Id: uzstat.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "uzstat.h"
#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fstream.h>
#include "commons.h"
 extern "C" float etime_(float ar[]);

AMSStat::AMSStat():AMSNodeMap(){
map(Timer);
}
AMSStat::~AMSStat(){
print();
Timer.remove();
}
void AMSStat::book(char *name, int freq){
if( add(*(Timer.add(new AMSStatNode(name,freq))))==-1)
cerr<<" AMSStat-Book-E-Name "<<name<<" already exists"<<endl;
}
void AMSStat::start(char *name){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
if(p){
p->_time=HighResTime();
p->_startstop=1;
}
else cerr<<"AMSStat-Start-E-Name "<<name<<" does not exist"<<endl;
}
number AMSStat::check(char * name){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
 if(p){
   return HighResTime()-p->_time;
 }
 else return 0;
}
number AMSStat::stop(char *name, integer force){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
number time=0;
if(p){
  if(p->_startstop==1){
    p->_entry=p->_entry+1;
    number tt=HighResTime();
    time=tt-p->_time;
    p->_time=tt;
    p->_sum=p->_sum+time;
    if(time > p->_max)p->_max=time;
    if(time < p->_min)p->_min=time;
    p->_startstop==0;
  }
  else if(!force)cerr<<"AMSStat-Stop-W-NTSTRTD "<<name<<" was not started"<<endl;
}
else cerr<<"AMSStat-Stop-E-Name "<<name<<" does not exist"<<endl;
return time;
}

void AMSStat::print(){
cout <<"           Name      Entries          Min      Average          Max"
<<"          Sum"<<endl;
AMSNodeMap::print(1);
}

ostream & AMSStatNode::print(ostream &stream) const{
static char *name=0;
if(!name || strlen(name)<strlen(getname())){
 delete name;
 name=new char[strlen(getname())+1];
}
strcpy(name,getname());
if(name && strlen(name)>15)name[14]='\0';

return _entry >0 ? stream <<setw(15)<<name<<" "<<setw(12)<<_entry*_freq<<" "<<setw(12)<<_min
 <<" "<<setw(12)<<_sum/(_entry+1.e-20)<<" "<<setw(12)<<_max<<" "<<setw(12)<<_sum*_freq<<endl:stream;
}

#include <sys/time.h>
extern "C" number HighResTime(){

 static float ar[2];
 static unsigned int count=0;
  static float ltime=0;
#ifdef __ALPHAOLD__
 static number ETimeLast;
 static timeval  TPSLast;
 static struct timezone  TZ;
 static timeval TPS;
 static integer init=0;
  const number TRes=0.002;

if(init++ ==0){
gettimeofday(&TPSLast,&TZ);
ETimeLast=etime_(ar);
}
geant time=etime_(ar);
if(time -ETimeLast  > TRes){
gettimeofday(&TPSLast,&TZ);
ETimeLast=time;

return time;
}
else {
  //
  // Try to get more high res
  //
gettimeofday(&TPS,&TZ);
number(TPS.tv_sec-TPSLast.tv_sec)+1.e-6*(TPS.tv_usec-TPSLast.tv_usec);
TPSLast.tv_sec=TPS.tv_sec;
TPSLast.tv_usec=TPS.tv_usec;
if(ltime<= TRes )ETimeLast+=ltime;
else ETimeLast=time;
return ETimeLast;
}

#else
#ifdef sun
 hrtime_t nsec=gethrtime();
  return double(nsec)*1e-9;
#else
if(!AMSCommonsI::remote()){
  ltime=etime_(ar);
  count++;
}
else if((count++)%128==0){
// clock_t clicks = clock();
//  ltime=( (float) clicks / CLOCKS_PER_SEC );
ltime=etime_(ar);
}

if((count)%2048==0){
       ifstream fbin;
        fbin.open("/proc/self/where");
        if(fbin){
         int node=-1;
          fbin>>node;
         if(node!=0){
          if(!AMSCommonsI::remote())cout <<"AMSTimeID::validate-I-Remote Client Detected "<<endl;
          AMSCommonsI::setremote(true);
         }
         else{
           if(AMSCommonsI::remote())cout <<"AMSTimeID::validate-I-Local Client Detected "<<endl;

           AMSCommonsI::setremote(false); 
        }
        }
        else{
         AMSCommonsI::setremote(false); 
        }    
        fbin.close();
}
return ltime;
#endif
#endif

}


AMSStatErr::AMSStatErr():AMSNodeMap(){
map(Timer);
}
AMSStatErr::~AMSStatErr(){
print();
Timer.remove();
}
void AMSStatErr::book(char *name, char severity){
if( add(*(Timer.add(new AMSStatErrNode(name,severity))))==-1)
cerr<<" AMSStatErr-Book-E-Name "<<name<<" already exists"<<endl;
}

void AMSStatErr::print(char *name, char *message, uinteger maxprint){
AMSStatErrNode *p=(AMSStatErrNode*)getp(AMSID(name,0));
if(p){
    p->_entry=p->_entry+1;
    if(p->_entry<=maxprint){
     if(p->_severity=='I'){
      cout <<name<<"-I-"<<message<<endl;
     }
     else{
      cerr <<name<<"-"<<p->_severity<<"-"<<message<<endl;
     }
   }
   if(p->_entry==maxprint){
     cout <<"LastMessageFor "<<name<<" TooManyEntries"<<endl;
   }
   if(p->_severity=='F'){
     abort();
   }
}
else if(!( add(*(Timer.add(new AMSStatErrNode(name,message[0]))))==-1)){
  print(name,message,maxprint);
}
}

void AMSStatErr::print(){
cout <<"           Name      Type     Entries        "<<endl;
AMSNodeMap::print(1);
}


ostream & AMSStatErrNode::print(ostream &stream) const{
static char *name=0;
if(!name || strlen(name)<strlen(getname())){
 delete name;
 name=new char[strlen(getname())+1];
}
strcpy(name,getname());
if(name && strlen(name)>15)name[14]='\0';

return _entry >0 ? stream <<setw(15)<<name<<" "<<setw(12)<<_severity<<setw(12)<<_entry<<" "<<endl:stream;
}
