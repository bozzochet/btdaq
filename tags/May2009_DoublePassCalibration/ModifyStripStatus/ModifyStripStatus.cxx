#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

typedef unsigned short ushort;


int main(int argc,char ** argv){

  int tdrnum;
  int thrnum;
  char namein[200];
  int inon=0;
  FILE *in=0;

  if(argc==3){
    sscanf(argv[1],"%x",&tdrnum);    
    // tdrnum=atoi(argv[1]);
    thrnum=atoi(argv[2]);
  } else if (argc==4){
    sscanf(argv[1],"%x",&tdrnum);
    // tdrnum=atoi(argv[1]);
    thrnum=atoi(argv[2]);    
    sprintf(namein,"%s",argv[3]);
    inon=1;
  } else {
    printf("Takes input from stdin or the specified file and add\n");
    printf("a flag to bad channel and put the new cal to stdout\n\n\n");
    printf("Usage: %s [tdrnum] [thrnum] <file_in> \n\n\n",argv[0]);
    exit(2);
  }

  ushort newstatus[1024];
  memset(newstatus,0,1024*sizeof(newstatus[0]));

  // badstrip masking
  char  badname[100];
  sprintf(badname,"/home/testsys/DAQ/ModifyStripStatus/data/badstriplist_tdr%03d_thr%03d.txt",500+tdrnum,thrnum);
  FILE* badstripfile = fopen(badname,"r");
  if (!badstripfile) { cout<< "!!!" << endl; exit(1); }
  while (!feof(badstripfile)) { 
    int mval;    
    fscanf(badstripfile,"%d",&mval);
    if(mval!=0) newstatus[mval]=32;
  }
  fclose(badstripfile);

  // changing
  if(inon){
    in=fopen(namein,"r");
    //   char name2[100];
    //   sprintf(name2,"calload.cal");
    //   FILE *out=fopen(name2,"w");
    if( !in){
      printf("%s: problems with file %s  !\n",argv[0],namein);
      exit(2);
    } 
  }
  for (int ii=0;ii<16;ii++){
    int aa;
    float bb,cc;
    if(inon)
      fscanf(in,"%d, %f, %f",&aa,&bb,&cc);
    else
      scanf("%d, %f, %f",&aa,&bb,&cc);
    printf("%d,   %5.3f,  %5.3f\n",aa,bb,cc);
  }

  for( int ii=0;ii<1024;ii++){
    int aa,bb,cc;
    float dd,ee,ff,gg;
    ushort status;
    if(inon)
      fscanf(in,"%d %d %d  %f %f %f %f %hd",
	     &aa,&bb,&cc, &dd,&ee,&ff,&gg,&status);
    else
      scanf("%d %d %d  %f %f %f %f %hd",
	    &aa,&bb,&cc, &dd,&ee,&ff,&gg,&status);
    
    status |= newstatus[ii]; 
    
    printf("%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",
	   aa,bb,cc, dd,ee,ff,gg,status);
  }
  
  unsigned short int zz1,zz2,zz3,zz4,zz5,zz6,zz7,zz8;
  if(inon)
    fscanf(in,"%hx %hx %hx %hx %hx %hx %hx %hx",
	   &zz1,&zz2,&zz3,&zz4,&zz5,&zz6,&zz7,&zz8);
  else
    scanf("%hx %hx %hx %hx %hx %hx %hx %hx",
	  &zz1,&zz2,&zz3,&zz4,&zz5,&zz6,&zz7,&zz8);
  
  
  printf("%04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
	 zz1,zz2,zz3,zz4,zz5,zz6,zz7,zz8);
  
  if(inon) fclose(in);
  
  return 0;
}
