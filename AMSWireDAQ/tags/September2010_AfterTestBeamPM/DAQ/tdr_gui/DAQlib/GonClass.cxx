#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include  "GonClass.hh"

using namespace std;

GonClass::GonClass(char* address,int port):CommClass(address,port){
  pos.Reset();
  setpos.Reset();
}

//---------------------------------------------------------------------------

int GonClass::GetPos(int no_close){
  double a[10];
  char answer[80];
  char* ff;
  char **endptr;
  GonPos pos2;
  int ret;
  int count=0;
  char sep[5]="|";
  sleep(1);
  
  pos.Reset();
  ret=SendCommand("GONIO -c pos -p 1 npos ",answer,20,1);
  if(ret<0 ) return -1;

  int ii=0;
  //  cout<<"this is answer" <<answer<<endl;
  ff=strtok(answer," ");
  endptr=NULL; 
  a[0]=strtod(ff,endptr);
  while(ff=strtok(NULL," ")){ 
    endptr=NULL;
    ii++;  
    //cout<<"these are tokens answer "<< ii << " " <<ff<<endl;
    if(strstr(ff,sep)==0)
      a[ii]=strtod(ff,endptr);
  }

  //cout<<"ANSW 1:  "<<answer<<" a:  "<<a[0]<<" b: "<<a[1]<<" c: "<<a[2]<<" d: "<<a[3]<<" e: "<<a[4]<<endl;
  //FIXME controlla la sequenza
  pos.SetPosGrad(a[0],a[1],a[4],a[2]);
  return 1;
  count++;
  cout <<"----------------"<<endl;

  for (int ii=0;ii<4;ii++){
    usleep(50000);
    if(SendCommand("GONIO -c pos -p 1 npos ",answer,20,1)){
      int jj=0;
      ff=strtok(answer," ");
      endptr=NULL;
      a[0]=strtod(ff,endptr);
      while(ff=strtok(NULL," ")){ endptr=NULL;
	jj++; if(strstr(ff,sep)==0) a[jj]=strtod(ff,endptr);
      }
      //  cout<<"ANSW 1:  "<<answer<<" a:  "<<a[0]<<" b: "<<a[1]<<" c: "<<a[2]<<endl;
      //FIXME controlla la sequenza
      pos2.SetPosGrad(a[0],a[1],a[4],a[2]); 
      //      pos2.Print();
      pos+=pos2;
      //      pos.Print();
      count++;
    }
  }
  if(!no_close)CloseConnection();
  pos/=count;
  pos.Print();
  return 1;  
}


//---------------------------------------------------------------------------

double GonClass::GetLin1(){
  GetPos();
  return  pos.GetLin1();
}
//---------------------------------------------------------------------------

double GonClass::GetLin2(){
  GetPos();
  return  pos.GetLin2();
}
//---------------------------------------------------------------------------

double GonClass::GetAlpha(){
  GetPos();
  return  pos.GetAlpha();
}

//---------------------------------------------------------------------------

double GonClass::GetCraddle(){
  GetPos();
  return  pos.GetCraddle();
}
//---------------------------------------------------------------------------

int GonClass::MoveTo(){
  char command[80];
  char answer[80];
  int ret;

 //  pos=setpos;
//   return 0;

  for (int ii=0;ii<2;ii++){
    sprintf(command,"GONIO -c %f %f %f   move -p %f 1 nm ",setpos.GetAlphaGrad(),setpos.GetCraddleGrad(),setpos.GetLin2(),setpos.GetLin1());
    ret=SendCommand(command,answer,100,1);
    if( !ret &&pri) {
      printf("GonClass::MoveTo():  COMM errror\n");
      return -1;
    }
    GetPos(1);
  }
  CloseConnection();
  return 1;
}
//---------------------------------------------------------------------------

int GonClass::MoveToRel(){
  char command[80];
  char answer[80];
  int ret;
  GetPos(1);
  setpos+=pos;
  MoveTo();
  return 1;
}

//---------------------------------------------------------------------------


int GonClass::Reset(){
  char command[80];
  char answer[80];
  int ret;

  sprintf(command,"GONIO -c cal -p 1 ncal ");
  ret=SendCommand(command,answer,100);
  if( !ret &&pri) {
    printf("GonClass::Reset():  COMM errror\n");
    return -1;
  }
  SetZero();
  sleep(1);
  GetPos();
  return 1;
}
//---------------------------------------------------------------------------


int GonClass::Abort(){
  char command[80];
  char answer[80];
  int ret;

  sprintf(command,"GONIO -A ");
  ret=SendCommand(command,answer,100);
  if( !ret &&pri) {
    printf("GonClass::Abort():  COMM errror\n");
    return -1;
  }
  return 1;
}

//---------------------------------------------------------------------------
int GonClass::SendRawCommand(const char *comm){
  char command[80];
  char answer[80];
  int ret;

  sprintf(command,"GONIO %s",comm);
  ret=SendCommand(command,answer,50);
  if( !ret &&pri) {
    printf("GonClass::SendRawCommand():  COMM errror\n");
    return -1;
  }
  cout <<" SendRawCommand: answer= "<<answer<<endl;
  sleep(1);
  GetPos();
  return 1;
}


//---------------------------------------------------------------------------
int GonClass::Calibrate(){

  Reset();  
  sleep(1);
  SetPosGrad(32.,6.,0.,0.);
  MoveTo();
  sleep(1);
  
  SendRawCommand(" -c 5 refmove ");
  sleep(1);
  SendRawCommand("-c 57.3 0 0 r ");
  sleep(1);

  SetZero();
  GetPos();
  return 1;
}
  

//---------------------------------------------------------------------------
int GonClass::GotoZero() { SetPos(0,0,0,0); return MoveTo(); }

//---------------------------------------------------------------------------
int GonClass::SetZero(){
  char command[80];
  char answer[80];
  int ret;
  sprintf(command,"GONIO -c 0 0 0 setpos -p 0.0 1 setnpos");
  ret=SendCommand(command,answer,10);
  if( !ret &&pri) {
    printf("GonClass::MoveTo():  COMM errror\n");
    return -1;
  }
  
  return 1; 
}


//---------------------------------------------------------------------------
//! possibile bug come si misura l'angolo
int GonClass::RotateInPlace(double dtheta,double off2){

  GetPos();
  double newan=pos.GetAlpha()+dtheta;
  double l1=pos.GetLin1()+
    (tan(newan)*sin(pos.GetAlpha()) -cos(pos.GetAlpha()))*(pos.GetLin2()-off2);
  double l2=(pos.GetLin2()-off2)*sin(pos.GetAlpha())/sin(newan);
  SetPos(newan,pos.GetCraddle(),l1,l2);
  return   MoveTo();
}
  
