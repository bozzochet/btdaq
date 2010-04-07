//  $Id: io.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <sys/types.h>
#include <sys/stat.h>
#include "io.h"
#include <sys/time.h>
#include <sys/times.h>
#include "dcards.h"
#include "amsdbc.h"
#include "mceventg.h"
#ifdef __G4AMS__
#include "CLHEP/Random/Random.h"
#endif

integer AMSIO::_Ntot=0;
char * AMSIO::fnam=0;
fstream AMSIO::fbin;
AMSIO::AMSIO(integer run, integer event, time_t time, time_t nsec, integer ipart, 
integer seed[], AMSPoint coo,AMSDir dir, number mom, number pole,
number stheta, number sphi, integer nskip, number rad, number velt, number velp,number yaw, number roll, number pitch, number angvel): _run(run),_event(event),
  _ipart(ipart),_mom(mom),_time(time),_polephi(pole),_stationtheta(stheta),
_stationphi(sphi),_nskip(nskip),_rflight(rad),_veltheta(velt), _velphi(velp),
_yaw(yaw),_pitch(pitch),_roll(roll),_angvel(angvel),_nsec(nsec){
_seed[0]=seed[0];
_seed[1]=seed[1];
int i;
for (i=0;i<3;i++)_coo[i]=coo[i];
for (i=0;i<3;i++)_dir[i]=dir[i];
}

void AMSIO::setfile(char *f){
  if(f){
   fnam=new char[strlen(f)+1];
   strcpy(fnam,f);
  }
}

void AMSIO::init(integer mode,integer format){
  enum open_mode{binary=0x80};

  if(fnam){
    if(mode%10 ==1 ) {
        fbin.open(fnam,ios::in);
        if(fbin && SELECTFFKEY.Run){
          // read one by one 
          AMSIO io;
          integer ipos=0;
          integer iposr=0;
          integer runold=0;
          integer pidold=0;
          integer ok=1;
          integer seed0,seed1,skip;
          number theta,phi,pole;
          time_t time;
          while(ok){
           ok=mode/10?io.readA():io.read();
           if(ok){
             seed0=io.getseed(0);
             seed1=io.getseed(1);
             theta=io.getstheta();
             phi=io.getsphi();
             pole=io.getpolephi();
             time=io.gettime();
             skip=io.getskip();
           }
           if(format==1 && (io.getrun()!=runold || ok==0)){
             if(iposr>0)cout <<"AMSIO::init-I-Run "<<runold<<" has "<<iposr<<
                          " events with pid = "<<pidold<<endl;
 //            if(io.getrun()<0){
 //             cout <<"AMSIO::init-F-Negative run number "<< io.getrun()<<endl;
 //            exit(1);
 //         }
             iposr=0;
             pidold=io.getpid();
             runold=io.getrun();
           }
           ipos++;
           iposr++;
           if(SELECTFFKEY.Event >=0 && io.getrun() == SELECTFFKEY.Run && 
            io.getevent() >= SELECTFFKEY.Event)break; 
           if(io.getrun() == SELECTFFKEY.Run && iposr == 
             -SELECTFFKEY.Event)break; 
          }
          // pos back if fbin.good
          if(ok){
            fbin.seekg(integer(fbin.tellg())-sizeof(io));
            cout<<"AMSIO::init-I-Selected Run = "<<SELECTFFKEY.Run<<
              " Event = "<<io.getevent()<< " Position = "<<ipos<<endl;
          
          }
          else {
            if(format==0)cerr <<"AMSIO::init-F-Failed to select Run = "<<SELECTFFKEY.Run<<
              " Event >= "<<SELECTFFKEY.Event<<endl;
          if(format==1){
            fbin.seekg(integer(fbin.tellg())-sizeof(io));
            fbin.clear();
            ok=io.read();
            theta=theta*AMSDBc::raddeg;
            phi=phi*AMSDBc::raddeg;
            pole=pole*AMSDBc::raddeg;
             cout<<"AMSIO::init-I-Total of "<<ipos-1
             <<" events have been read."<<endl;
             cout <<" Last Run & Event "<<io.getrun()<<" "<<io.getevent()<<endl;
            cout<<"Last MC Event "<<skip<<endl;
            cout << " Last Random Number "<<seed0<<" "<<seed1<<" "   <<endl;
             cout << " Theta "<< theta<< " Phi "<<phi<<" Pole "<<pole<<endl;
             cout << " Time "<<ctime(&time)<<endl;
#ifdef __ALPHA__ 
 fbin.close();
#else
             if(fbin.is_open())fbin.close();
#endif
          }
          else  exit(1);
          }
          return;
        }
    }
    else if (mode ==3){


        fbin.open(fnam,ios::in|ios::ate);
        uinteger fs=0; 
        if(fbin ){
          // Check if fsize is o.k.
          struct stat  buf;
          stat(fnam,&buf);
          fs=buf.st_size;       
          if(fs%sizeof(AMSIO) !=0){
            cerr <<"AMSIO-init-S-File size error "<<fs<<" "<<fs%sizeof(AMSIO)<<endl;
          }
          // read one by one 
          AMSIO io;
          integer ok;
          number otheta;
             fbin.clear();
             fbin.seekg(integer(fbin.tellg())-2*sizeof(AMSIO)-fs%sizeof(AMSIO));
             ok=io.read();
             otheta=io.getstheta()*AMSDBc::raddeg;
             ok=io.read();
#ifdef __ALPHA__ 
 fbin.close();
#else
             if(fbin.is_open())fbin.close();
#endif
             if(ok){
              time_t time;
              number theta,phi,pole;
              integer seed[2];
              integer event=io.getevent();
              theta=io.getstheta()*AMSDBc::raddeg;
              phi=io.getsphi()*AMSDBc::raddeg;
              pole=io.getpolephi()*AMSDBc::raddeg;
              _Ntot=io.getskip();
              time=io.gettime();
              seed[0]=io.getseed(0);
              seed[1]=io.getseed(1);
              cout<<"AMSIO::init-I-Last Event "<<event<<endl;
              cout<<"Last MC Event "<<_Ntot<<endl;
              cout << " Last time "<<ctime(&time)    <<endl;
              cout << " Last Random Number "<<seed[0]<<" "<<seed[1]<<endl;
              cout << " Theta "<< theta<< " Phi "<<phi<<" Pole "<<pole<<endl;
              GCFLAG.NRNDM[0]=seed[0];
              GCFLAG.NRNDM[1]=seed[1];
              GRNDMQ(seed[0],seed[1],1,"S");
#ifdef __G4AMS__
              long seedl[3]={0,0,0};
              seedl[0]=seed[0];
              seedl[1]=seed[1];
              HepRandom::setTheSeeds(seedl);
#endif              
              CCFFKEY.theta=theta;
              CCFFKEY.phi=phi;
              CCFFKEY.polephi=pole;
              GCFLAG.IEVENT=GCFLAG.IEVENT+event;
              if(otheta > theta)CCFFKEY.sdir=-1;
              else CCFFKEY.sdir=1;
              tm *pb;
              pb=localtime(&time);
              CCFFKEY.begindate=1900+pb->tm_year+10000*(pb->tm_mon+1)+
              1000000*pb->tm_mday;
              CCFFKEY.begintime=pb->tm_sec+100*pb->tm_min+
              10000*(pb->tm_isdst?pb->tm_hour-1:pb->tm_hour);
             }
        }




     if(fs%sizeof(AMSIO) == 0)fbin.open(fnam,ios::out|ios::app);
             else {
               fbin.open(fnam,ios::out|ios::ate);
               fbin.seekg(integer(fbin.tellg())-fs%sizeof(AMSIO));
               cerr <<"AMSIO-init-I-Recovering... "<<endl;
             }
    }
    else fbin.open(fnam,ios::out|ios::app);
    if(!fbin){
      cerr<<"AMSIO::init-F-cannot open file "<<fnam<<" in mode "<<mode<<endl;
      exit(1);
    }
    static char buffer[32*sizeof(AMSIO)+1];
    // Associate buffer
#if defined(__USE_STD_IOSTREAM) || defined(__STDC_HOSTED__) || defined(sun) || defined(__ICC__)
    (fbin.rdbuf())->pubsetbuf(buffer,32*sizeof(AMSIO));
#else
    (fbin.rdbuf())->setbuf(buffer,32*sizeof(AMSIO));
#endif
  }
  else {
    cerr <<"AMSIO::init-F-no file to init "<<endl;
    exit(1);
  }
}
void AMSIO::write(){
    
   convert();
   fbin.write((char*)this,sizeof(*this));
    //
    // Stupid DEC has no setbuf,
    // but stupid global buffer about 1 kbytes
   // 
#ifndef __USE_STD_IOSTREAM
#ifdef __ALPHA__ 
   static integer counter=0;
   static integer nb=800/sizeof(AMSIO);
   counter=(counter+1)%nb;
   if(!counter)fbin.flush();   
#endif
#endif
}
integer AMSIO::readA(){
   if(fbin.good() && !fbin.eof()){

fbin >>_run >> _event >> _time >> _ipart ;
fbin >>_seed[0]>>_seed[1];
fbin >>_coo[0]>>_coo[1]>>_coo[2];
fbin >>_dir[0]>>_dir[1]>>_dir[2];
fbin >>_mom>>_polephi>>_stationtheta>>_stationphi;
fbin>>_nskip>>_rflight>>_velphi>>_veltheta>>_yaw>>_pitch>>_roll;
fbin>>_angvel>>_nsec;
}
   return fbin.good() && !fbin.eof();
}

integer AMSIO::read(){
   if(fbin.good() && !fbin.eof()){
     fbin.read((char*)this,sizeof(AMSIO)-CCFFKEY.oldformat*8*sizeof(geant));
     convert();
     if(CCFFKEY.oldformat){
      _rflight=AMSmceventg::Orbit.AlphaAltitude;
      _velphi=0;
      _veltheta=0;
      _yaw=0;
      _pitch=0;
      _roll=0;
      _angvel=AMSmceventg::Orbit.AlphaSpeed;
      _nsec=0;
     }
   }
   return fbin.good() && !fbin.eof();
}


integer AMSIOI::_Count=0;
AMSIOI::AMSIOI(){
  _Count++;
}
AMSIOI::~AMSIOI(){
  if(--_Count==0){
#ifdef __ALPHA__ 
 AMSIO::fbin.close();
#else
   if(AMSIO::fbin.is_open())AMSIO::fbin.close();
#endif
  }
}
ostream & operator << (ostream &o, const AMSIO &b ){
  return o<<b._run<<b._event<<b._time<<b._ipart<<b._coo[0]<<
    b._coo[1]<<b._coo[2]<<b._dir[0]<<b._dir[1]<<b._dir[2]<<b._mom<<b._seed[0]
          <<b._seed[1];
}   

void AMSIO::convert(){
  if(AMSDBc::BigEndian){
   // Let's convert   to little endian...
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)this;
   int i;
   int n=sizeof(*this)/sizeof(integer);
   for(i=0;i<n;i++){
     tmp=*pc;
     *pc=*(pc+3);
     *(pc+3)=tmp;
     tmp=*(pc+1);
     *(pc+1)=*(pc+2);
     *(pc+2)=tmp;
     pc=pc+sizeof(integer);
   }    
  }
}
