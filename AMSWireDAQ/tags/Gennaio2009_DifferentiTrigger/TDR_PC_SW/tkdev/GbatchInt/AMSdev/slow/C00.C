//  $Id: C00.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <ams/types.h>
#include <ams/castcp.h>

#include "Coordinates.h" // cooradinate recalculation 
#include "file_reading.h" // reading CAS files


#define foot_cm 30.48
#define debug 0

CASblock  buffer;
#if 0

typedef struct  {
  int   size;                        /* Actual size in little endian         */
  int   pktNum;                      /* Packet number for first NASCOM block */
  int   gsedTime[CAS_TAGS_INDEX+1];  /* gsed time for each NASCOM block      */
  int   nasTime[CAS_TAGS_INDEX+1];   /* NASCOM time for each NASCOM block    */
  byte  CASmsg[16*600];              /* Unmodified CAS message.              */
                                     /* (Can't be bigger than this.)         */
} CASblock;

#endif
int day, current_block=0;


double CASvalue (int offset, int length) {
  byte val[8];
  double dval=0;
  int i;

  for (i=0; i<length; i++) {
    val[i]=buffer.CASmsg[offset+length-i-4];
    if (debug){
      cout <<" value = "<<val[i]<<endl;
    }
  }
   
  dval=(double)*((double*)val);
  return dval;
}

double CAStime(int offset) { /* based on H.Suter's function */
  ulong musec, sec, min, hours, days, time;
  double times;
  int i;
  union {
    ulong val;
    struct {
      uint msec : 13;		// microseconds
      uint nil  :  5;		// void
      uint usec :  4;		// units of seconds
      uint tsec :  3;		// tens of seconds
      uint umin :  4;		// units of minutes
      uint tmin :  3;		// tens of minute
      uint uhour:  4;		// units of hours
      uint thour:  2;		// tens of hours
      uint uday :  4;		// units of days
      uint tday :  4;		// tens of days
      uint hday :  2;		// hundreds of days
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

float double_float(double *a) {
  float b;
  if ((*a == 0) || (abs(*a)>3.4e+30) || (abs(*a)<3.4e-30))
    return 0.;
  b=(float) (*a);
  return b;
}

int main(int argc, char *argv[]) {

double Coo_M1950[3], Vel_M1950[3], q[4], Greenw_Phi, Vel_angle, times;
polar Geo,Solar;
Euler Euler_LVLH;
FILE *fp, *ftemp, *fout, *fl;
int i,k, file_nu, ii=30, bad_stat=0;
uint Data[Record_nbytes/4], *pData;
uint crc;
time_t cur_time;
float fbuf;
int block_size;
char current_f_name[120], chb80[120];


file_n=0;
record_n=Umax=0;
Umin = 999999999;

  CAS_dir=getenv("CASDataDir");
  Coo_db_dir=getenv("CooDbDir");
  setbuf(stdout,NULL);

  //puts("********************");
  cout <<"********************"<<endl;
  while(1) {  /*GLOBAL LOOP */

NEW_FILES: /* look for new files in the directory */
    file_n=0;
    record_n=Umax=0;
    Umin = 999999999;
    new_files=0;
    new_file_c=0;
    new_files=new_files_coming();
    //cout <<" new files "<<new_files<<" "<<new_file_c<<endl;
    if (!new_files) {
      ii++;
      if (ii>3) {
	time(&cur_time);
	//printf("no new files. waiting......%s\n",ctime(&cur_time));
        cout <<"no new files. waiting.... "<<ctime(&cur_time)<<endl;
	ii=0;
      }
      sleep(60);
      goto NEW_FILES;
    }
    
    for (file_nu=0; file_nu<= new_file_c; file_nu++) { /* Main Loop */
      fp = open_file(file_nu);
         if (fp == NULL){
           cerr <<"cannot open file "<<new_file_n[file_nu]<<endl;
	   continue;
         }      
      int trys=0;
  BEG: ;
      if (feof(fp)) {
	fclose(fp);
	goto END_LOOP;
      }
    /*reading data */
      fread(&block_size,sizeof(int),1,fp);
      trys++;
      for (i=0; i<20; i++) 
	fread(&k, sizeof(int),1,fp);
      fread(buffer.CASmsg,1,(block_size-20*4),fp);


      if (record_n>=Record_in_file) {
	fclose(ftemp);
	ftemp=fopen(temp_file,"r");
	fread(Data,Record_nbytes,1,ftemp);
	fclose(ftemp);
	pData = &Data[0];
	crc = _CalcCRC(pData);
	time(&cur_time);
	sprintf(current_f_name,"%s/ShuttlePar.1.000000000",Coo_db_dir);
	fout=fopen(current_f_name,"w");
        if(fout){
	  fwrite(Data,1,Record_nbytes,fout);
	  fwrite(&crc,sizeof(time_t),1,fout);
	  fwrite(&cur_time,sizeof(time_t),1,fout);
	  fwrite(&Umin,sizeof(time_t),1,fout);
	  fwrite(&Umax,sizeof(time_t),1,fout);
	  fclose(fout);
        }
	else {
         cerr<<" Unable to open file "<<current_f_name<<endl;
        }
	sprintf(chb80,"rm %s",temp_file);
	system(chb80);
	//printf("======================%d\n",file_n);
	cout <<"======================"<<file_n<<endl;
	file_n++;
	record_n=Umax=0;
	Umin = 999999999;
	fclose(fout);
      }
	/* Reading M1950 coordinates and velocities from CAs */
      bad_stat=0;
      Coo_M1950[0]=CASvalue((4490),8) * foot_cm;
      Coo_M1950[1]=CASvalue((4508),8) * foot_cm;
      Coo_M1950[2]=CASvalue((4526),8) * foot_cm;

      Vel_M1950[0]=CASvalue((4687),8) * foot_cm;
      Vel_M1950[1]=CASvalue((4701),8) * foot_cm;
      Vel_M1950[2]=CASvalue((4715),8) * foot_cm;
//        printf(".......................... %d %d\n",record_n,Umin); 

	/* Reading quaterions from CAS */
      q[0]=CASvalue((3291),8);
      q[1]=CASvalue((3305),8);
      q[2]=CASvalue((3319),8);
      q[3]=CASvalue((3333),8);

      times = CAStime(10);   
      utime = (time_t) (times) + Year1998;
      
      if ((Radius(Coo_M1950)<=.1) || (Radius(Vel_M1950)<=.1) || (times<=0)){
	cerr <<" Some Parameters are Zero, record skipped"<<endl;
	goto BEG;
	bad_stat=1;
      }
      if ((Radius(Coo_M1950)>.1) && (Radius(Vel_M1950)>.1)){
	//cout <<" call "<<endl;
	Coordinates(Coo_M1950,Vel_M1950,q,utime,&Geo,&Vel_angle,&Euler_LVLH,&Solar,&Greenw_Phi);
      }
        
      if (record_n == 0)
	ftemp=fopen(temp_file,"w");
      
      if (!bad_stat) {
	fbuf=(float)(-Geo.R);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Geo.Teta);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Geo.Phi);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Greenw_Phi);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(0*Euler_LVLH.Yaw);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(0*Euler_LVLH.Pitch);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(0*Euler_LVLH.Roll);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Vel_angle);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Solar.R);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Solar.Teta);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Solar.Phi);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fwrite(&utime,sizeof(time_t),1,ftemp);
	if (utime<Umin) Umin=utime;
	if (utime>=Umax) Umax=utime+1;
	record_n++;
      }

      memset( (char *) &buffer, 0, sizeof(buffer));
      goto BEG;

    END_LOOP:;
      fl=fopen(last_name,"w");
      //fprintf(fl,"%s\n",new_file_n[new_file_c]);
      //
      fprintf(fl,"%u\n",new_file_t[new_file_c]);
      fclose(fl);
    }
    //cout <<" record "<<record_n<<endl;
    if (record_n>0) { 
      for (i=record_n; i<Record_in_file; i++) {
	fbuf=(float)(-Geo.R);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Geo.Teta);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Geo.Phi);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Greenw_Phi);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(0*Euler_LVLH.Yaw);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(0*Euler_LVLH.Pitch);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(0*Euler_LVLH.Roll);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Vel_angle);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Solar.R);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Solar.Teta);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fbuf=(float)(Solar.Phi);
	fwrite(&fbuf,sizeof(float),1,ftemp);
	fwrite(&utime,sizeof(time_t),1,ftemp);
      }
      fclose(ftemp);
      ftemp=fopen(temp_file,"r");
      fread(Data,Record_nbytes,1,ftemp);
      fclose(ftemp);
      pData = &Data[0];
      crc = _CalcCRC(pData);
      time(&cur_time);
      sprintf(current_f_name,"%s/ShuttlePar.1.000000000",Coo_db_dir);
      fout=fopen(current_f_name,"w");
      fwrite(Data,1,Record_nbytes,fout);
      fwrite(&crc,sizeof(time_t),1,fout);
      fwrite(&cur_time,sizeof(time_t),1,fout);
      fwrite(&Umin,sizeof(time_t),1,fout);
      fwrite(&Umax,sizeof(time_t),1,fout);
      fclose(fout);    
      if (((Umax-Umin)>3600) || (Umax<Umin)) {
	sprintf(chb80,"rm %s",current_f_name);
	cerr<<"Timing corrupted. File skipped"<<endl;
      }
    }
    goto NEW_FILES;
  }
  return(0);
}
