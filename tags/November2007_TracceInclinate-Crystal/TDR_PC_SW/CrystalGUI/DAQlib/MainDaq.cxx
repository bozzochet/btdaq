#include "MainDaq.hh"
#include <cstdio>
#include <cstdlib>
#include <csignal>      
#include <unistd.h>

int burstoff;


void SetBurstOff(int ii);

void SetBurstOff(int ii){
  burstoff=1;
  return;
}

MainDaq::MainDaq(){

//   PgDaq0  = new DaqClass("PG0","pgdaq0",6666);
//   PgDaq1  = new DaqClass("PG1","pgdaq1",6666);
//   RmDaq   = new RMSClass("ROMA","rmdaq",6666);
//   SciDaq  = new SciClass("SCINT","fedaq",6666);
//   Self    = new SelfClass("localhost",6666);

  Gon  = new GonClass("localhost",6666);
//   Trig  = new TrigClass("fedaq",6666);
//   //  Russ  = new RuClass("RU0","rus",6666);
//   trigtype=0;
//   run=0;
//   burstoff=0;
  InitRunNumber();


}



MainDaq::~MainDaq(){

//   if(PgDaq0) delete PgDaq0;
//   if(PgDaq1) delete PgDaq1;
//   if(RmDaq) delete RmDaq;
//   if(SciDaq) delete SciDaq;
  if(Gon) delete Gon;
//   if(Trig) delete Trig;
//   if(Self) delete Self;
  //  if(Russ) delete Russ;

}

int MainDaq::InitRunNumber(){

  FILE * rfile;
  char srun[20];
  rfile= fopen("runlog.log","r");
  if(!rfile){
    printf("No runlog.log file!!!!  Run numbers start again from 100\n");
    run=100;
    rfile= fopen("runlog.log","wr");
    if(!rfile){printf("Cannot create runlog.log. Giving up, bye.\n");exit(10);}
    fprintf(rfile,"100 \n");
    fclose(rfile);
  }else{
    fgets(srun,100,rfile);
    run=atoi(srun);
    fclose(rfile);
    rfile= fopen("runlog.log","w");
    fprintf(rfile,"%d \n",run);
    fclose(rfile);    
  }
  
  return run;
}

int MainDaq::SaveRunNumber(int newrun){

  FILE * rfile;
  char srun[20];
  rfile= fopen("runlog.log","r");
  if(!rfile){printf("MainDaq::SaveRunNumber(): runlog.log do not exist! Fatal Error!Giving up, bye.\n");exit(10);}
  fclose(rfile);
  int rout=newrun;
  rfile= fopen("runlog.log","w");
  fprintf(rfile,"%d \n",rout);
  fclose(rfile);
  run=newrun;
  return run;
};

int MainDaq::StartRun(){

  int ret1,ret2,ret3,ret4,ret5;
  int hhh=0;
  int ret;
  int newrun=run+1;
// printf("Stopping the trigger\n");
//   ret=Trig ->StopTrig();
//   //check if trig runs.
// printf("Is trigger still running\n");
//   ret=Trig->IsTriggerRunning();
//   if(ret) {printf("MainDaq::StartRun(): cannot stop the trigger\n");  return 4; }
//   //read the Gon Pos
// printf("Reading the Goniometer Postion\n");
//  ret=Gon->GetPos();
//  if(!ret) {printf("MainDaq::StartRun(): cannot Read the Goniometer Position\n");  return 4; }
  
//   ret1=0;
//   hhh=0;
//   ret3=0;
//   ret4=0;
//   //  ret= Self->CalibPG(newrun);
//   // if(!ret) {printf("MainDaq::StartRun(): PG Calibratin Failed\n");  return 4; }
//   printf("Start PG0 Daq\n");
//   ret1=PgDaq0 ->StartDaq(newrun,Gon);
//   printf("Start PG1 Daq\n");
//   hhh=PgDaq1 ->StartDaq(newrun,Gon);
//   printf("Start Sci Daq\n");
//   ret3=SciDaq ->StartDaq(newrun);
//   printf("Start Rm  Daq\n");
//   ret4=RmDaq  ->StartDaq(newrun,Gon);
  
//   if((ret1>0) && (hhh>0) && (ret3>0) && (ret4>0)     ){ // ready to start!
//       //if((ret1>0) && (hhh>0) && (ret3>0)      ){ // ready to start!

//     //    ret=Trig ->SetTrigType(trigtype);
//     //    if(!ret) return -1;
//     printf("Starting trigger \n");
//     ret=Trig->StartTrig();
//     printf("Is trigger running ?\n");
//     ret=Trig->IsTriggerRunning();
//     if(ret==0) {printf("MainDaq::StartRun(): cannot start the trigger\n");  return 4; }
//     else  SaveRunNumber(newrun);
//     printf(" Run %d has Started \n",newrun);
//     return 0;
//   }
//   else{
//     printf("MainDaq::StartRun(): Error a daq is not ready\n");
//     return -1;
//   }
  return 0;

}

int MainDaq::StopRun(){
  int ret1,ret2,ret3,ret4;
  int ret;

  
 //  //wait for DAQ to free the buffers
//   sleep(2);
//   ret=Trig ->StopTrig();
//   ret1=PgDaq0 ->StopDaq(30);
//   ret2=PgDaq1 ->StopDaq(30);
//   ret3=SciDaq ->StopDaq(30);
//   ret4=RmDaq  ->StopDaq(30);

//   printf("Waiting for RM Daq to exit \n");
// //  ret=Trig->StartTrig();
// //   sleep(20);
// //  ret=Trig ->StopTrig();
// //  if(!ret) return -1;
//   sleep(1);
//   PgDaq0->GetProcEvents();
//   PgDaq1->GetProcEvents();
//   RmDaq->GetProcEvents();
//   SciDaq->GetProcEvents();

//   //  if(!ret1) PgDaq0 ->KillDaq(30);
//   // if(!ret2) PgDaq1 ->KillDaq(30);
//   // if(!ret3) SciDaq ->KillDaq(30);
//   //if(!ret4) RmDaq  ->KillDaq(30);
//   printf("Run %d  has ended\n",run);
  return 0;
}


int MainDaq::CalibratePG(int crun){
  // return Self->CalibPG(crun);
}

int MainDaq::GetEvents(){

  // PgDaq0->GetProcEvents();
  //PgDaq1->GetProcEvents();
 return 0;
}
