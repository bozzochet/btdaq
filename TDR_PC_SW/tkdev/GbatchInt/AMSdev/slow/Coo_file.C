//  $Id: Coo_file.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <typedefs.h>
#include <cern.h>
#include <ctype.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdio.h>
#include <iomanip.h>
#include <dirent.h>                   
#include <ams/types.h>
#include <ams/castcp.h>
#include "Coordinates.h"
#include <astring.h>

int NAME;
const int NWPAW=700000;
struct PAWC_DEF{
float q[NWPAW];
};
#define PAWC COMMON_BLOCK(PAWC,pawc)
#define N_bad 500   // maximum number of bad CAS blocks

COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;


   static const int debug=0;
  class ShuttlePar{
  public:
    float StationR;
    float StationTheta;
    float StationPhi;
    float GrMedPhi;
    float StationYaw;
    float StationPitch;
    float StationRoll;
    float StationSpeed;
    float SunR;
    float SunTheta;
    float SunPhi;
    float StThetaGTOD;
    float StPhiGTOD;
    float VelTheta;
    float VelPhi;
    float VelThetaGTOD;
    float VelPhiGTOD;
   time_t Time;
  };

   char *CAS_dir;
   char *Coo_db_dir;
   integer td2f(double d);
   float d2f(double d);
   integer Cas2Shuttle(CASblock & b1, ShuttlePar & b32);
   int updShuttle(ShuttlePar &b, integer close);
   int _select(dirent *entry);
   int _sort(dirent ** e1, dirent ** e2);
   void convert(int,int, char*);
   int ClockTune(time_t Time);
   double CASvalue (CASblock & buffer, int offset, int length, uint &ofset);
   double CAStime(CASblock & buffer, int offset);
   int badlist_open(uint & cur);
   int badlist_search(time_t t, uint & cur);
   int badlist_close(uint & cur);
   time_t badlist[N_bad];
   FILE *fpbad;
   uint nach=0, cur=0, bad_s;

int main (int argc, char *argv[]) {

        switch (argc) {
        case 1:
         cerr <<"ShuttlePar-F-Please give input file name(s) as  parameter(s) or + for whole directory"<<endl;
	 return 1;
        default:
         break;
        }
         CAS_dir=getenv("CASDataDir");
         Coo_db_dir=getenv("CooDbDir");
//             convert(0,0,"037000");
        int iostat,rsize=1024;
        HLIMIT(NWPAW);
        HROPEN(3,"output","cas_time","N",rsize,iostat);
           if(iostat){
             cerr << "Error opening Histo file cas"<<endl;
             exit(1);
           }
           else cout <<"Histo file "<<"cas"<<" opened."<<endl;

	   if(argv[1][0]=='+'){
	   if (1) {
	     	   
	     sscanf(argv[1],"%d",&NAME);
         cout <<" Only files with name >= " <<NAME<<" will be scanned"<<endl;
         int i;
         cout <<"ShuttlePar-I-Scanning directory "<<CAS_dir<<endl;
         dirent ** namelist;                            
         int nptr=scandir(CAS_dir,&namelist,&_select,&_sort);    
         if(nptr>0){
          cout <<"ShuttlePar-I-Found "<<nptr <<" entries"<< endl;
           int i;
           //for(i=0;i<nptr;i++)cout<<namelist[i]->d_name<<endl;    
	   
	   bad_s=badlist_open(cur);

           for(i=0;i<nptr;i++){
             convert(i,nptr-i-1,namelist[i]->d_name);
           }
	   if (badlist)
	     badlist_close(cur);
	   
         }
         else{
           cerr<<"ShuttlePar-F-NoCasFilefound in "<<CAS_dir<<endl;
           exit(1);
         } 
        }
        else{ 
         int i;
          cout <<"ShuttlePar-I-Found "<<argc-1 <<" entrie(s)"<< endl;
         for(i=1;i<argc;i++){
            convert(i-1,argc-i-1,argv[i]);
          }
        }

        return 0;
}



void convert (int begin, int end,char file[]){
           ifstream fin;   // CAS 
           static integer count=0;
           uinteger length;
           CASblock buffer;
           ShuttlePar block;
	   time_t badt=0;
           if(begin==0){
              HBNT(1,"CAS Data"," ");
              HBNAME(1,"CASData",(int*)(&block),"StationR,StationTheta,StationPhi,GrMedPhi,StationYaw,StationPitch,StationRoll,StationSpeed,SunR,SunTheta,SunPhi,StThetaGTOD,StPhiGTOD,VelTheta,VelPhi,VelThetaGTOD,VelPhiGTOD,Time:I");

           }
           // open fin;
           AString fname(CAS_dir);
           fname+="/";
           fname+=file;
           fin.open(fname);
           if(fin){
              cout <<"convert-I-Openfile "<<fname<<" ("<<++count<<")"<<endl;
              while(!fin.eof()){
                fin.read((char*)(&length),sizeof(integer));
                fin.seekg(fin.tellg()+20*sizeof(integer));
                fin.read((char*)(&(buffer.CASmsg)),length-20*sizeof(integer)); 
                int ret=Cas2Shuttle(buffer,block);
                if(ret>0){
		  if(updShuttle(block,0))HFNT(1);
                 }
                 else {
		   cerr<<"Error no "<< ret <<" decoding block in file "<<fname<<endl;
		 }
	      }
              fin.close();
	   }
           else{
             cerr <<"convert-E-UnableToOpenfile "<<fname<<endl;
             return;
           }
           if(end==0){
                // last file 
                // close everything
	     updShuttle(block,1);
                int ntuple_entries;
                HNOENT(1, ntuple_entries);
                cout << " Closing CAS ntuple  with "<< ntuple_entries 
                << " events" << endl;
                char hpawc[256]="//PAWC";
                HCDIR (hpawc, " ");
                char houtput[9]="//output";
                HCDIR (houtput, " ");
                integer ICYCL=0;
                HROUT (1, ICYCL, " ");
                HREND ("output");
                CLOSEF(1);


           }
      
       }


       int _select(dirent *entry){
        for(int i=0;i<strlen(entry->d_name);i++){
         if(!isdigit((entry->d_name)[i]))return 0;
        }
        int e1;
         sscanf(entry->d_name,"%d",&e1);
         if(e1<NAME)return 0;
         else return 1;


       }
       
int ClockTune(time_t Time){
         static time_t begin=Year1998+153*86400;
         return 4+(Time-begin)/50000;
       }
float d2f(double d){
  if(fabs(d)<1.e30)return float(d);
  else if (d>1.e30)return 1.e30;
  else return -1e30;
}
integer td2f(double d, double lim_low, double lim_up){
  if((fabs(d)<=lim_up)&&(fabs(d)>=lim_low)) return 1;
  else return 0;
}
integer Cas2Shuttle(CASblock & buffer, ShuttlePar & block){
        /* Reading M1950 coordinates and velocities from CAs */
      const int foot_cm=30.48;
double Coo_M1950[3], Vel_M1950[3], q[4], Greenw_Phi, Vel_angle;
polar Geo,Geo_G,Solar,Geo_Vel,Geo_G_Vel;
Euler Euler_LVLH;
uint para,statu;

      para=1;  
      Coo_M1950[0]=CASvalue(buffer,(4490),8,statu) * foot_cm;
      if (statu!=0) para=0;
      Coo_M1950[1]=CASvalue(buffer,(4508),8,statu) * foot_cm;
      if (statu!=0) para=0;
      Coo_M1950[2]=CASvalue(buffer,(4526),8,statu) * foot_cm;
      if (statu!=0) para=0;

      Vel_M1950[0]=CASvalue(buffer,(4687),8,statu) * foot_cm;
      if (statu!=0) para=0;
      Vel_M1950[1]=CASvalue(buffer,(4701),8,statu) * foot_cm;
      if (statu!=0) para=0;
      Vel_M1950[2]=CASvalue(buffer,(4715),8,statu) * foot_cm;
      if (statu!=0) para=0;

      q[0]=CASvalue(buffer,(3291),8,statu);
      if (statu!=0) para=0;
      q[1]=CASvalue(buffer,(3305),8,statu);
      if (statu!=0) para=0;      
      q[2]=CASvalue(buffer,(3319),8,statu);
      if (statu!=0) para=0;
      q[3]=CASvalue(buffer,(3333),8,statu);
      if (statu!=0) para=0;
      double times= CAStime(buffer,10);                
      time_t utime = time_t(times+0.5) + Year1998;      
      // check record

      if (utime>896825174) nach=1;
	
      if((Radius(Coo_M1950)>.1) && (Radius(Vel_M1950)>.1) && (times>0)) {

	
     // convert here 
	Coordinates(Coo_M1950,Vel_M1950,q,utime,&Geo,&Geo_G,&Vel_angle,&Euler_LVLH,&Solar,&Greenw_Phi,&Geo_Vel,&Geo_G_Vel);
      
	if(!td2f(Geo.Teta,-.92, 1.))para=0;
	if(!td2f(Geo.Phi,-3.1416, 3.1416))para=0;
	if(!td2f(Geo.R,6.25e8,6.7e8))para=0;
	if(!td2f(Greenw_Phi,0,1.e20))para=0;
	if(!td2f(Euler_LVLH.Yaw,-1.571,1.571))para=0;
	if(!td2f(Euler_LVLH.Pitch,-3.1416,3.1416))para=0;
	if(!td2f(Euler_LVLH.Roll,-3.1416,3.1416))para=0;
	if(!td2f(Vel_angle,0,0.00125))para=0;
	if(!td2f(Solar.R,0,1.e20))para=0;
	if(!td2f(Solar.Phi,0,1.e20))para=0;
	if(!td2f(Solar.Teta,0,1.e20))para=0;
	if(!td2f(Geo_G.Teta,-.92, 1.))para=0;
	if(!td2f(Geo_G.Phi,-3.1416, 3.1416))para=0;
	if(!td2f(Geo_Vel.Teta,-1.571, 1.571))para=0;
	if(!td2f(Geo_Vel.Phi,-3.1416, 3.1416))para=0;
	if(!td2f(Geo_G_Vel.Teta,-1.57, 1.57))para=0;
	if(!td2f(Geo_G_Vel.Phi,-3.1416, 3.1416))para=0;
	if(!td2f(utime+0.,1.,897678150.))para=0;
      }
      else {
	para=0;
      }
	
      	if ((badlist) && (para)) {
	  if (badlist_search(utime, cur)) {
	    para=0;
	  }
	}
      if (para==0) {
	return -1;
      }

      if (!nach) return -1;

      block.StationTheta=d2f(Geo.Teta);
      block.StationPhi=d2f(Geo.Phi);
      block.StationR=d2f(Geo.R);
      block.GrMedPhi=d2f(Greenw_Phi);
      block.StationYaw=d2f(Euler_LVLH.Yaw);
      block.StationPitch=d2f(Euler_LVLH.Pitch);  
      block.StationRoll=d2f(  Euler_LVLH.Roll);
      block.StationSpeed=d2f(  Vel_angle);
      block.SunR=d2f(Solar.R);
      block.SunPhi=d2f(Solar.Phi);
      block.SunTheta=d2f(Solar.Teta);
      block.StThetaGTOD=d2f(Geo_G.Teta);
      block.StPhiGTOD=d2f(Geo_G.Phi);
      block.VelThetaGTOD=d2f(Geo_G_Vel.Teta);
      block.VelPhiGTOD=d2f(Geo_G_Vel.Phi);
      block.VelTheta=d2f(Geo_Vel.Teta);
      block.VelPhi=d2f(Geo_Vel.Phi);


      //cout <<" "<<ClockTune(utime)<<" "<<ctime(&utime)<<endl;
      block.Time=utime-ClockTune(utime);
}

int updShuttle(ShuttlePar & record, int close){
   static ofstream fout;
   static integer Arrp=-1;
   static ShuttlePar Array[60];
   static time_t Insert=0;
   if(close){
     if(fout && Arrp>=0){
      for( int i=Arrp+1;i<60;i++)Array[i]=record;
      uinteger crc=_CalcCRC((uinteger*)Array,sizeof(Array));
      AString fname(Coo_db_dir);
      fname+="/";
      fname+="ShuttlePar.1.";
      char file[256];
      sprintf(file,"%d",Array[0].Time);
      fname+=file;
      fout.open(fname);
      if(fout){
        fout.write((char*)Array,sizeof(Array));
        fout.write((char*)(&crc),sizeof(crc));
        time_t newi;
        time(&newi);
        if(newi!=Insert)Insert=newi;
        else Insert++;
        fout.write((char*)(&Insert),sizeof(Insert)); 
        fout.write((char*)(&(Array[0].Time)),sizeof(time_t)); 
        time_t end=(Array[59].Time+61);
        fout.write((char*)(&end),sizeof(time_t)); 
        fout.close();
        cout <<"ShuttlePar-I-writeFile "<<fname<<" "<<endl;
        cout <<"Insert "<<ctime(&Insert)<<endl;
        cout <<"Begin "<<ctime(&Array[0].Time)<<endl;
        cout <<"End "<<ctime(&end)<<endl;
      }
      else{
        cerr <<"ShuttlePAr-F-UnableToOPenFile "<<fname<<endl;
        exit(1);
      }
     }
   }
   else {
       // validate record first
       if(Arrp<0 || (record.Time>Array[Arrp].Time && record.Time<Array[Arrp].Time+36000))Arrp++;
       else {
         cerr<<"Errror no 1 "<<Arrp<< " "<<record.Time<<" "<<Array[Arrp].Time<<endl;
         return 0;
       }
     if(Arrp==60 || (Arrp>0 && record.Time>Array[Arrp-1].Time+3600)){
       for( int i=Arrp;i<60;i++)Array[i]=Array[Arrp-1];
          // close file
        AString fname(Coo_db_dir);
        fname+="/";
        fname+="ShuttlePar.1.";
        char file[256];
        sprintf(file,"%d",Array[0].Time);
        fname+=file;
        fout.open(fname);
        if(fout){
          uinteger crc=_CalcCRC((uinteger*)Array,sizeof(Array));
          fout.write((char*)Array,sizeof(Array));
          fout.write((char*)(&crc),sizeof(crc));
          time_t newi;
          time(&newi);
          if(newi!=Insert)Insert=newi;
          else Insert++;
          fout.write((char*)(&Insert),sizeof(Insert)); 
          fout.write((char*)(&(Array[0].Time)),sizeof(time_t)); 
          time_t end=(Array[59].Time+1);
          fout.write((char*)(&end),sizeof(time_t)); 
          fout.close();
          cout <<"ShuttlePar-I-writeFile "<<fname<<endl;
          cout <<"Insert "<<ctime(&Insert)<<endl;
          cout <<"Begin "<<ctime(&Array[0].Time)<<endl;
          cout <<"End "<<ctime(&end)<<endl;
        }
        else{
          cerr <<"ShuttlePAr-F-UnableToOPenFile "<<fname<<endl;
          exit(1);
        }
          Arrp=0;
     }
     Array[Arrp]=record;
   }
   return 1;
}

        

double CASvalue (CASblock & buffer, int ofset, int length, uint &statu) {
  byte val[8];
  double dval=0;
  int i;

  statu=buffer.CASmsg[ofset-4];
  for (i=0; i<length; i++) {
    val[i]=buffer.CASmsg[ofset+length-i-4];
    if (debug){
      cout <<" value = "<<val[i]<<endl;
    }
  }

  dval=(double)*((double*)val);
  return dval;
}


double CAStime(CASblock & buffer, int offset) { /* based on H.Suter's function */
  ulong musec, sec, min, hours, days, time;
  double times;
  int i;
  union {
    ulong val;
    struct {
      uint msec : 13;           // microseconds
      uint nil  :  5;           // void
      uint usec :  4;           // units of seconds
      uint tsec :  3;           // tens of seconds
      uint umin :  4;           // units of minutes
      uint tmin :  3;           // tens of minute
      uint uhour:  4;           // units of hours
      uint thour:  2;           // tens of hours
      uint uday :  4;           // units of days
      uint tday :  4;           // tens of days
      uint hday :  2;           // hundreds of days
    } CASdate;
  } t;

  if (buffer.CASmsg[offset-4] == 0) {
    time = buffer.CASmsg[offset-3];
    for (i=1; i<6; i++)
      time = time << 8 | buffer.CASmsg[offset-3+i];
    t.val=time;
    musec = t.CASdate.msec / 8;
    sec   = t.CASdate.usec  + 10*t.CASdate.tsec;
    min   = t.CASdate.umin  + 10*t.CASdate.tmin;
    hours = t.CASdate.uhour + 10*t.CASdate.thour;
    days  = t.CASdate.uday  + 10*t.CASdate.tday +  100*t.CASdate.hday;

    times = (days*86400 + hours*3600 + min*60 + sec) + musec/1000.;

    return times;
  }
  else
    return 0;
}

int _sort(dirent ** e1, dirent ** e2){
 return strcmp((*e1)->d_name,(*e2)->d_name);
}

// opening badlist file
int badlist_open(uint & cur) {
  int i,n=0;
  char * ch, ch80[80];
  
  fpbad=fopen("CAS_bad_manual.list","r");
  if (fpbad==NULL) {
    cout<<"WARNING:cannot open CAS_bad_manual.list"<<endl;
    sleep(2);
    return 0;
  }
  ch=fgets(ch80,80,fpbad);

  while (!feof(fpbad)) {
    ch=fgets(ch80,80,fpbad);
    badlist[n]=strtoul(ch,NULL,10);
    n++;
  }
  for (i=n; i<N_bad; i++)
    badlist[i]=899000000;
  cur=0;

  return 1;
}

//c losing badlist file
int badlist_close(uint & cur) {
  fclose(fpbad);
  return 1;
}

// search badlist file, assuming it is sorted
int badlist_search(time_t t, uint &cur) {
  int i;

  for (i=cur; i<N_bad; i++) {
    if (badlist[i]==t) {
      cur=i;
      return 1;
    }
    if (badlist[i]>t) {
      cur=i;
      return 0;
    }
  }
  cur=N_bad;
  cout<<">>>>>>>>>> time= "<<t<<" badlist["<<i<<"] = "<<badlist[i]<<endl;
  exit(1);
  return 0;
}
