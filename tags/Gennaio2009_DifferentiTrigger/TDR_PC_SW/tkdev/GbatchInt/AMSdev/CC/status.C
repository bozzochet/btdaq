//  $Id: status.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V.Choutko.
#include "status.h"
#include "snode.h"
#include "amsgobj.h"
#include "job.h"
#include "amsstl.h"
#include <iostream.h>
#include "event.h"
#include "timeid.h"
#include "dcards.h"

integer AMSStatus::_Mode=0;
void AMSStatus::_init(){
     _Nelem=0;
}
   
ostream & AMSStatus::print(ostream & stream)const{
  return(AMSID::print(stream)  <<  " Status" << endl);
}

AMSStatus* AMSStatus::create(int version){
 if( version==0){ 
   static AMSStatus tstatus("EventStatusTable",version);
   return &tstatus;
 }
else{
   static AMSStatus tstatus("EventStatus2Table",version);
   return &tstatus;
}

}

integer AMSStatus::isFull(uinteger run, uinteger evt, time_t time){
  static time_t oldtime=0;
  integer timechanged= time!=oldtime?1:0;
  oldtime=time;
  if(run==_Run && _Nelem>0 && evt<_Status[0][_Nelem-1]){
    cerr <<"AMSStatus::isFull-E-EventSequenceBroken "<<_Nelem<<" "<<run<<" "<<evt<<" "<<_Status[0][_Nelem-1]<<endl;
   _Errors++;
   return 1;
  }
  if(_Nelem>=MAXDAQRATE+STATUSSIZE){
        cerr <<"AMSSTatus::isFull-E-MaxDAQRateExceeds "<<MAXDAQRATE<<
        " some of the events will be lost"<<endl;
        _Errors++;
        return 1;
}
  return (_Nelem>=STATUSSIZE && timechanged ) || (run!=_Run && _Nelem>0) ;
}

void AMSStatus::adds(uinteger run, uinteger evt, uinteger* status, time_t time){
  if(_Nelem==0  || isFull(run,evt,time)){
    _Nelem=0;
    _Run=run;
    _Begin=time;
    if(_Begin==0){
      cerr <<"AMSStatus::adds-E-BeginTimeIsZeroForRun"<<" "<<_Run<<endl;
      _Errors++;
    }
  }
  _End=time;
  _Status[0][_Nelem]=evt;
  _Status[1][_Nelem]=status[0];
  _Status[2][_Nelem]=((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getoffset();
  _Status[3][_Nelem]=status[1];
  _Nelem++;
}

void AMSStatus::updates(uinteger run, uinteger evt, uinteger* status, time_t time){
  int out= AMSbins(_Status[0],evt,_Nelem);
  if(out>0){
    _Status[1][out-1]=status[0];
    _Status[3][out-1]=status[1];
  }
  else {
      cerr<<"AMSStatus::updates-E--NoMatchFoundRun "<<run<<" " <<evt<<endl;
      _Errors++;
  }
  if(out==_Nelem && AMSFFKEY.Update && isDBUpdateR()){
   UpdateStatusTableDB();
  }
}

AMSStatus::statusI AMSStatus::getstatus(uinteger evt, uinteger run){
  uinteger one=1;
  if(_Run && run != _Run){
   cerr<<"AMSStatus::getstatus-E-WrongRun "<<run<<" Expected "<<_Run<<endl;
   _Errors++;
   return statusI((one<<31),0);
  }
  // try hint +
  int out;
  if(_Hint>=_Nelem || evt!=_Status[0][_Hint])out= AMSbins(_Status[0],evt,_Nelem);
  else out=_Hint+1;
  static int repeat=0;
 if (out>0){
   _Hint=out;
   repeat=0;
   return statusI(_Status[1][out-1],_Status[3][out-1])  ;
 }
 else if(repeat<10  ){
   cerr<<"AMSStatus::getstatus-E-NoMatchFoundRun "<<run<<" "<<out<<" "<<evt<<" "<<_Nelem<<" "<<_Status[0][-out]<<" "<<_Status[0][-out-1]<<endl;
   _Errors++;
   repeat++;
   return statusI((one<<31),0);
 }
 else if(repeat==10 ){
   cerr<<"AMSStatus::getstatus-E-NoMatchFoundLastMessage"<<out<<" "<<evt<<endl;
   _Errors++;
   repeat++;
   return statusI((one<<31),0);
 }
 else {
   return statusI((one<<31),0);
}
}


void AMSStatus::geteventpos(uinteger run, uinteger evt, uinteger curevent){
  if(_Run && run != _Run){
   cerr<<"AMSStatus::geteventpos-E-WrongRun "<<run<<" Expected "<<_Run<<endl;
   return;
  }
  // try hint +
  int out;
  if(_Hint>=_Nelem || evt!=_Status[0][_Hint])out= AMSbins(_Status[0],evt,_Nelem);
  else out=_Hint+1;
 if (out>0){
   _Hint=out;
   //event found;
 ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Status[2][out-1]);
  return;   
 }
 else {
   // No Match Found
   if(evt>_Status[0][_Nelem-1] && curevent<_Status[0][_Nelem-1]){
      ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Status[2][_Nelem-1]);
   }
   else if(curevent>_Status[0][_Nelem-1]){ 
    cerr<<"AMSStatus::geteventpos-E-NoMatchfound "<<run<<" "<<curevent<<" "<<_Status[0][_Nelem-1]<<" "<<endl;
   }
   else if(evt<_Status[0][_Nelem-1]){ 
    cerr<<"AMSStatus::geteventpos-E-NoMatchFoundRun "<<run<<" "<<out<<" "<<evt<<" "<<_Nelem<<" "<<_Status[0][-out]<<" "<<_Status[0][-out-1]<<endl;
      ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Status[2][-out]);
   }
}
}


void AMSStatus::init(){
  _Mode=1;
  if(  AMSFFKEY.Update){
    AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::gethead()->getTDVStatus());
    for(int i=0;i<AMSJob::gethead()->gettdvn();i++){
      if( strcmp(AMSJob::gethead()->gettdvc(i),ptdv->getname())==0 ){
       if(!STATUSFFKEY.status[32]){   
         _Mode=2;
         cout <<"AMSStatus::init-I-WriteStatusDBRequested"<<endl;
         // set begin,end to max
           time_t begin,insert,end;
           ptdv->gettime(insert,begin,end);
           begin=begin-3e7;
           end=end+3e7;
           ptdv->SetTime(insert,begin,end);
          
       }
       else {
         _Mode=3;
         cout <<"AMSStatus::init-I-UpdateStatusDBRequested"<<endl;
        // set begin>end
           time_t begin,insert,end;
           ptdv->gettime(insert,begin,end);
           end=begin-1;
           ptdv->SetTime(insert,begin,end);
       
       }
      }
    }
  }
}


integer AMSStatus::statusok(uinteger event, uinteger run){
    statusI status=getstatus(event,run);
    return _statusok(status);
}

integer AMSStatus::_statusok(statusI status){
    uinteger one=1;
    if(!(status[0] & (one<<31))){    // Status exists
      const int nsta=15;
      uinteger Status[nsta];
      Status[0]=((status[0] & ((1<<4)-1)));
      Status[1]=((status[0]>>4) & ((1<<1)-1));
      Status[2]=((status[0]>>5) & ((1<<3)-1));
      Status[3]=((status[0]>>8) & ((1<<1)-1));
      Status[4]=((status[0]>>9) & ((1<<1)-1));
      Status[5]=((status[0]>>10) & ((1<<5)-1));
      Status[6]=((status[0]>>15) & ((1<<2)-1));
      Status[7]=((status[0]>>17) & ((1<<2)-1));
      Status[8]=((status[0]>>19) & ((1<<2)-1));
      Status[9]=((status[0]>>21) & ((1<<2)-1));
      Status[10]=((status[0]>>23) & ((1<<2)-1));
      Status[11]=((status[0]>>25) & ((1<<2)-1));
      Status[12]=((status[0]>>27) & ((1<<2)-1));
      Status[13]=((status[0]>>29) & ((1<<1)-1));
      Status[14]=((status[0]>>30) & ((1<<1)-1));
        uinteger local=0;
      for(int i=0;i<nsta;i++){
        local=0;
        if(STATUSFFKEY.status[i]==0)continue;
        else {
          uinteger st=STATUSFFKEY.status[i];
          uinteger nbit=32.*log(2.)/log(number(STATUSFFKEY.status[33]))+0.5;
          for (int j=0;j<nbit;j++){
            uinteger stbit=(st%STATUSFFKEY.status[33])>0?1:0;
//            if(i==10)cout <<"qq "<<stbit <<" "<<j<<" "<<Status[i]<<" "<<i<<endl;
            if((stbit<<j) & (1<<Status[i])){
//              if(i==10){
//               cout <<" got it "<<j<<" "<<stbit<<" "<<Status[i]<<endl;
//              }
              local=1;
              break; 
            }
            st=st/10;
          }
        }
        if(!local){
          _Rejected++;
          return 0;
        }
      }
    }
    else if(isDBUpdateR()){
     _Rejected++;
     return 0;
    }
    _Accepted++;
    return 1;

}


integer AMSStatus::getnextok(){
 int skipped=0;
 for(int i=_Hint;i<_Nelem;i++){
   if(_statusok(statusI(_Status[1][i],_Status[3][i]))){
     ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Status[2][i]);
     _Hint=i;
     return skipped;
   }
   skipped++;
 }
 if(_Hint<_Nelem)((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Status[2][_Nelem-1]);
 else  if(AMSFFKEY.Update && isDBUpdateR()){
   UpdateStatusTableDB();
 }
 return skipped;
}

void AMSStatus::UpdateStatusTableDB(){
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::getTDVStatus());
      uinteger crcold=ptdv->getCRC();
      ptdv->UpdCRC();
      if(crcold!=ptdv->getCRC()){
       ptdv->UpdateMe()=1;
       time_t begin,end,insert;
       ptdv->gettime(insert,begin,end);
       time(&insert);
       ptdv->SetTime(insert,begin,end);
       cout <<" Event Status info  info has been updated for "<<*ptdv;
       cout <<" Time Insert "<<ctime(&insert);
       cout <<" Time Begin "<<ctime(&begin);
       cout <<" Time End "<<ctime(&end);
       cout << " Starting to update "<<*ptdv; 
       if(  !ptdv->write(AMSDATADIR.amsdatabase))
        cerr <<"AMSStatus::UpdateStatusTableDB-S-ProblemtoUpdate "<<*ptdv;
      }
  }


