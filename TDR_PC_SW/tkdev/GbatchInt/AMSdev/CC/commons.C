//  $Id: commons.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $


#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "commons.h"
#include <stdlib.h>
#include <iostream.h>
#include "cern.h"
#include "amsdbc.h"
#include <fstream.h>
//
// Geant  commons

GCFLAG_DEF GCFLAG;
GCLINK_DEF GCLINK;
GCVOLU_DEF GCVOLU;
GCTMED_DEF GCTMED;
GCTRAK_DEF GCTRAK;
GCKINE_DEF GCKINE;

// Fortran commons
// TKFIELD_DEF TKFIELD;
// AMSDATADIR_DEF AMSDATADIR

char AMSCommonsI::_version[]="v4.00";
uinteger AMSCommonsI::_build=189;
float AMSCommonsI::_mips=1000;
uinteger AMSCommonsI::_os=0;
char AMSCommonsI::_osname[255];
char AMSCommonsI::_osversion[255];
pid_t AMSCommonsI::_pid=0;
bool AMSCommonsI::_remote=false;
AMSCommonsI::AMSCommonsI(){
  init();
}
#include <sys/utsname.h>
void AMSCommonsI::init(){ 
  if(_Count++==0){
   struct utsname u;
   uname(&u);
   strcpy(_osversion,"0.0");
     if(strstr(u.sysname,"OSF1")){
      strcpy(_osname,"osf1");
      _os=1;
     }
     else if(strstr(u.sysname,"Linux")){
      strcpy(_osname,"linux");
      _os=2;
      strcpy(_osversion,u.release);  
     }
     else if(strstr(u.sysname,"SunOS")){
      strcpy(_osname,"sunos");
      _os=3;
     }
     else {
      _os=0;
      _osname[0]='\0';
     }
     if(_os){
      cout <<"AMSCommonsI-I-HardwareIdentifiedAs "<< u.sysname<<" "<<u.release<<" "<<u.machine<<endl;
     }
     else{
      cerr<<"AMSCommonsI-E-CouldNotMap "<<u.sysname<<" "<<u.machine<<endl;
      cerr<<"Production Job Will Be Aborted"<<endl;
     }
   char dt[128]="/afs/cern.ch/exp/ams/Offline/AMSDataDir";
   char* gtv=getenv("AMSDataDir");
   if(gtv && strlen(gtv)>0){
    AMSDATADIR.amsdlength=strlen(gtv)+strlen(getversion())+2;
    if(AMSDATADIR.amsdlength>127){
      cerr <<"AMSCommonsI::init-F-AMSDataDirLength>127 "<<
        AMSDATADIR.amsdlength<<endl;
       exit(1);  
    }
      strcpy(AMSDATADIR.amsdatadir,gtv);
      strcat(AMSDATADIR.amsdatadir,"/");
      strcat(AMSDATADIR.amsdatadir,getversion());
      strcat(AMSDATADIR.amsdatadir,"/");
      AMSDATADIR.amsdlength=strlen(AMSDATADIR.amsdatadir);
      strcpy(AMSDATADIR.amsdatabase,gtv);
      strcat(AMSDATADIR.amsdatabase,"/");
      strcat(AMSDATADIR.amsdatabase,"DataBase");
      strcat(AMSDATADIR.amsdatabase,"/");
      AMSDATADIR.amsdblength=strlen(AMSDATADIR.amsdatabase);
     
   }
   else {
     cout<<"AMSCommonsI-W-AMSDataDir variable is not defined."<<endl;
     cout <<"AMSCommonsI-W-Default value "<<
        dt<< " will be used."<<endl;
      strcpy(AMSDATADIR.amsdatadir,dt);
      strcat(AMSDATADIR.amsdatadir,"/");
      strcat(AMSDATADIR.amsdatadir,getversion());
      strcat(AMSDATADIR.amsdatadir,"/");
      AMSDATADIR.amsdlength=strlen(AMSDATADIR.amsdatadir);
      strcpy(AMSDATADIR.amsdatabase,dt);
      strcat(AMSDATADIR.amsdatabase,"/");
      strcat(AMSDATADIR.amsdatabase,"DataBase");
      strcat(AMSDATADIR.amsdatabase,"/");
      AMSDATADIR.amsdblength=strlen(AMSDATADIR.amsdatabase);
      
   }
          if(sizeof(int) <= sizeof(short int)){
         cerr<<"AMSTrIdSoftI-F-16 bit machine is not supported."<<endl;
         exit(1);
       }
       integer b64=0;
       if(sizeof(ulong)>sizeof(uinteger))b64=1;
       uinteger test1,test2;
       test1=1;
       test1+=2<<8;
       test1+=4<<16;
       test1+=8<<24;
       unsigned char *pt= (unsigned char*)&test1;
       test2=pt[0];
       test2+=pt[1]<<8;
       test2+=pt[2]<<16;
       test2+=pt[3]<<24;
       integer lend = test1==test2;
       if(lend)cout <<"AMSTrIdSoftI-I-Identified as LittleEndian";
       else {
         cout <<"AMSTrIdSoftI-I-Identified as BigEndian";
         AMSDBc::BigEndian=1;
       }
       if(b64)cout <<" 64 bit machine."<<endl;
       else cout <<" 32 bit machine."<<endl;
       AMSDBc dummy;
       AMSDBc::amsdatabase=new char[strlen(AMSDATADIR.amsdatabase)+1];
       strcpy(AMSDBc::amsdatabase,AMSDATADIR.amsdatabase);
       _pid=getpid();
       //  get mips
       char *fname = tempnam("/tmp",NULL);
       char syscom[255];
{
       strcpy(syscom,"cat /proc/cpuinfo | grep -i -e 'cpu mhz'  > ");
       strcat(syscom,fname);
       int i=system(syscom);
       if(i==0){
        ifstream fbin;
        fbin.open(fname);
        if(fbin){
          fbin.ignore(255,':');
          fbin>>_mips;
          if(_mips<100)_mips=100;
          fbin.close();
        }
       }  
}

{
       strcpy(syscom,"cat /proc/cpuinfo | grep -i -e name  > ");
       strcat(syscom,fname);
       int i=system(syscom);
       if(i==0){
        ifstream fbin;
        fbin.open(fname);
        if(fbin){
          fbin.ignore(255,':');
          fbin.getline(syscom,254,'\n');
          fbin.close();
          float _cor=1;
          cout <<"AMSCommonsI-I-SystemIdentified as "<<syscom<<endl;
          if(strstr(syscom,"Pentium II"))_cor=1.07;
          else if(strstr(syscom,"Pentium III"))_cor=1.0;
           else if(strstr(syscom,"Pentium(R) III"))_cor=1.0;
          else if(strstr(syscom,"Pentium(R) 4")){
           if(_mips<2400) _cor=0.7;
           else _cor=0.85;
          }
          else if(strstr(syscom,"Pentium(R) M"))_cor=1.07;
          else if(strstr(syscom,"E5") or strstr(syscom,"X5"))_cor=1.45;
          else if(strstr(syscom,"Xeon"))_cor=0.8;
          else if(strstr(syscom,"Athlon"))_cor=1.15;
          else cerr<<"AMSCommonsI-E-UnableToMatchName "<<syscom<<endl;
          _mips*=_cor;
        }
       }
       cout <<"AMSCommonsI-I-ComputerEvaluatedAsMips "<<getmips()<<endl;  
}


  }
}
integer AMSCommonsI::_Count=0;
