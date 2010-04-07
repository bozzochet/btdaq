#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
//
 PROTOCCALLSFFUN3(INT,IFNTREAD,ifntread,STRING,INT,INT)
#define IFNTREAD(A2,A3,A4)  CCALLSFFUN3(IFNTREAD,ifntread,STRING,INT,INT,A2,A3,A4)
//input
//  filename
//  nevents
//  0/1/>1   ntuple,root,root+jou
//  lastevent
//  verbose
//
//-----------------------------------------------------------------|
//output:a) Bit 8 is set -> bad ntuple(+bits (1-7)=1->open problem;|
//            =2->read probl; =3->NevIn>NevRead; =4->NevIn<NevRead;| 
//            =5->LastEventProbablyWrong
//            =6->miss.arguments)                                  |
//       b) Bit 8 is not set --> bits 1-7 give % of events with    |
//          EventStatus=bad;                                     |
//-----------------------------------------------------------------|
//
 int rootread(char * fname,int nevents,int iver, int& lastevent,bool jou);
 int main(int argc, char * argv[])
 {
//
   int iflg;
   char fname[256];
//
   int lastevent=0;
   bool verbose=false;
   bool root=false; 
   bool jou=false;
   int iver=0;
   iflg=0;
   if(argc>2){
     strcpy(fname,argv[1]);
     int nevents=atoi(argv[2]);
      if(argc>3){
       int iot=atoi(argv[3]);
        if(iot)root=true;
        if(iot>1)jou=true; 
      }
     if(argc>4){
      lastevent=atoi(argv[4]);
     }
     if(argc>5){
      verbose=true;
      iver=1;
     }
     if(iver)cout<<"Requested file: "<<fname<<" imply "<<nevents<<" events"<<endl;
      if(root){
       iflg=rootread(fname,nevents,iver,lastevent,jou);
      }
      else {
       iflg=IFNTREAD(fname,nevents,iver);
      }
     if(iflg>=0){
//       float proc;
//       proc=float(iflg%128);
//       cout<<" proc="<<proc<<endl;
     }
     else{
       iflg=abs(iflg)+128; // abs(-1/-2/-3/-4) + bit 8 for "-"
     }
//     cout<<"Flag="<<iflg<<endl;
   }
   else
   {
//     cout<<"FastNtupleRead-Error: missing arguments ???"<<endl;
     iflg=6+128; // abs(-6) + bit 8 for "-"
   }
     if(verbose)cout<<"Flag="<<iflg<<endl;
     return(iflg);
 }
