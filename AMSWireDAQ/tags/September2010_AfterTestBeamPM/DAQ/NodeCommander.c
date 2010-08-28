// file paolo-amsw.c
//
//
// A.Lebedev, Mar-2009...
//
// A.Basili May-2009...
// P.Zuccon May-2009...
#include "mylib.h"
#include "amswlib.h"



#define MAXPARS 20*1024
#define ANSW_LIM 20*1024


int main(int argc,char** argv){
  int ii,kk;
  int16 node=0;
  int RW=0;
  int32 RQ_typ=0;  
  int16 RQ_dat[MAXPARS];
  int32 RQ_cnt =0;// argc-3;   //Number of request parameters

  if(argc<3){
    printf("usage: %s <node_number(HEX)> <data_type(HEX)>  [pars]\n",argv[0]);
    return -2;
  }else {
    
   sscanf(argv[2],"%x",&RQ_typ); 
   if((RQ_typ&0x1F0000)==0x1F0000){
     if((RQ_typ&0x400000)==0x400000) RW=1;
   }else{
     if((RQ_typ&0x40)==0x40)  RW=1;
   }
   RQ_typ&=0x1FFFFF;
   sscanf(argv[1],"%hx",&node); 
   NODE_ADR =node&0x1FF; 
   char fname[12];
   if(argc>3){
     if(argv[3][0]=='a'||argv[3][0]=='A'){
       if(argc==3){printf("You must specify a cmdfile name\n");exit(-1);}
       int ll=strlen(argv[4]);
       if(ll>12){printf("Illegal filename there are more than 12 chars\n");exit(-1);}
       char *pp=(char*)RQ_dat;
       for(kk=11;kk>=0;kk--){
	 if(ll>=0)
	   fname[kk]=argv[4][ll--];
	 else
	   fname[kk]=' ';
       }
       for(kk=0;kk<12;kk++)
	 *(pp++)=fname[kk];
       RQ_cnt=6;
       
     }
     else{
       for(ii=3;ii<argc;ii++)
	 sscanf(argv[ii],"%hx",(&RQ_dat[ii-3]));
       RQ_cnt=argc-3;
     }
     
     
   }
  }
   setup_command_path_from_file("slowconf.conf");

 



  int32 RP_siz=ANSW_LIM;   //Max acceptable answer lenght
  int32 RP_cnt=0;          //Actual Answer lenght  
  int16 RP_dat[RP_siz];    //Answer 
  int16 RP_err=0;          //Error code

  to_AMSW(RW,  RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, &RP_err);
  int cnt=0;
  if (RP_cnt < RP_siz){
    for( kk=0;kk<RP_cnt;kk++)	{
      if(kk%4==0) printf("%2d: ",cnt++);
      printf(" %04x ",RP_dat[kk]);
      if(kk!=0&&kk%4==3) printf("\n");
    }
  printf("\n");
  }
  else  
    printf("Warning reply truncated beacuse too large: %d limit is %d \n",RP_cnt,RP_siz);
 
  if (RP_err) 
    return RP_err;
  else
    return 0; 
  }
