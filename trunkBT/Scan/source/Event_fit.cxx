#include "Event.hh"
#include "Cluster.hh"
#include "RHClass.hh"
#include "Event_Fit.hh"
#include <math.h>
#include <iostream>
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TFormula.h"
#include "TLinearFitter.h"


ClassImp(Event);

double Event::algpar[11][4]={
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0}
};
double Event::algpar_fich[8][11][4];

double Event::hz[4]={0.0,10.0,20.0,30.0};

Event::Event(int irawtdrnum){

  rawtdrnum=irawtdrnum;
  Evtnum=0;
  JINJStatus=0;
  for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;

  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0;
  }
  for(int ii=0;ii<1024;ii++)
    for(int kk=0;kk<8;kk++)
    Signal[kk][ii]=0;
  
  //  RawLadder = new TClonesArray("RawData",rawtdrnum);

  NClusTot=0;
  notgood=0;
  Cls = new TClonesArray("Cluster",2);
  Cls->SetOwner();

}

Event::~Event(){

  if(Cls) {Cls->Delete(); delete Cls;}
   
}

void Event::Clear(){
  JINJStatus=0;
   for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;

  NClusTot=0;
  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0;

  }
  for(int ii=0;ii<8;ii++){ 
    for(int kk=0;kk<1024;kk++)
      Signal[ii][kk]=0;
  }
  if(Cls)  Cls->Delete();
    
  
  //   for (int ii=Cls->GetEntries();ii>-1;ii--){
//     Cluster* ff=(Cluster*) Cls->RemoveAt(ii);
//     if(ff) delete ff;
//   }
  
  return;
}

Cluster* Event::AddCluster(int lad,int side){
  Cluster* pp=(Cluster*)Cls->New(NClusTot);
    NClus[lad][side]++;
  NClusTot++;
  return pp;

}

Cluster* Event::GetCluster(int ii){
 return (Cluster*)Cls->At(ii);

}

int Event::bon_evenement(RHClass* rh) {
  int bon = 0;
  int nas = 0;
  int nak = 0;
  for (int ii=0; ii<Cls->GetEntries(); ii++) {
    Cluster* clu = (Cluster*)Cls->At(ii);
    if (clu->ladder != -1)
    AddCluster(rh->FindPos(clu->ladder),clu->side);
  }
  for (int ii=0; ii<4; ii++) {
     if (NClus[ii][0]==1) nas++;
     if (NClus[ii][1]==1) nak++;
  }
  if (nas == 4) bon++;
  if (nak == 4) bon+=2;
  return(bon);

}


int Event::NGoldenClus(int lad, int side){
  int num=0;
  for (int ii=0;ii<NClusTot;ii++){
    Cluster* cl=GetCluster(ii);
    if(cl->ladder==lad&&cl->side==side&&cl->golden==1) num++;
  }
  return num;
}

void Event::EcrireAlgPar(char *filename){
 
  printf("Ecriture de %s\n",filename);
  /*  if(strlen(scanname)<2){
    sprintf(filename,"align.dat");
  }
  else{
    sprintf(filename,"align_%s.dat",scanname);
    
    } */
  FILE * ft=fopen(filename,"w");
  if(!ft){ 
    printf("Error cannot open %s \n",filename);
    return;
  }

  for (int i=0; i<9; i+=2) {
   fprintf(ft,"%f %f %f %f %f %f %f %f\n",algpar[i][0],algpar[i+1][0],
    algpar[i][1],algpar[i+1][1],algpar[i][2],algpar[i+1][2],algpar[i][3],algpar[i+1][3]);
  }
  fprintf(ft,"%f %f %f %f\n",algpar[10][0],algpar[10][1],algpar[10][2],algpar[10][3]);

  fclose(ft);
  return;
}

void Event::LireAlgPar(){
 

  char filename[40]="../align/algpar.dat";
  char ligne[100];
  printf("Lecture de %s\n",filename);

  FILE * ft=fopen(filename,"r");
  if(!ft){ 
    printf("Error cannot open %s \n",filename);
    return;
  }
  
  float x00,y00,x01,y01,x02,y02,x03,y03;
  for (int i=0; i<9; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f %f %f \n",&x00,&y00,
   &x01,&y01,&x02,&y02,&x03,&y03);
   algpar[i][0] = x00;
   algpar[i+1][0] = y00;
   algpar[i][1] = x01;
   algpar[i+1][1] = y01;
   algpar[i][2] = x02;
   algpar[i+1][2] = y02;
   algpar[i][3] = x03;
   algpar[i+1][3] = y03;
   printf("%f %f %f %f %f %f %f %f\n",algpar[i][0],algpar[i+1][0],
    algpar[i][1],algpar[i+1][1],algpar[i][2],algpar[i+1][2],algpar[i][3],algpar[i+1][3]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f\n",&x00,&x01,&x02,&x03);
  algpar[10][0] = x00;
  algpar[10][1] = x01;
  algpar[10][2] = x02;
  algpar[10][3] = x03;
  printf("%f %f %f %f\n",algpar[10][0],algpar[10][1],algpar[10][2],algpar[10][3]);
  /* 
  float x00,y00,x01,y01,x02,y02,x03,y03;
  int ret=fscanf(ft ,"%f %f %f %f %f %f %f %f \n",&x00,&y00,&x01,&y01,&x02,&y02,&x03,&y03);
  // printf("%d %f %f %f %f %f %f %f %f\n",ret,x00,y00,x01,y01,x02,y02,x03,y03);
  algpar[0][0]=x00;
  algpar[1][0]=y00;

  algpar[0][1]=x01;
  algpar[1][1]=y01;
	  
  algpar[0][2]=x02;
  algpar[1][2]=y02;

  algpar[0][3]=x03;
  algpar[1][3]=y03;
  */
  fclose(ft);
  return;
}

void Event::LireFichAlgPar(){
 
  for (int h=0; h<8; h++)
    for (int i=0; i<11; i++)
      for (int j=0; j<4; j++) algpar_fich[h][i][j]=0;

  char ligne[100];
  char filename[40];
  float x00,y00,x01,y01,x02,y02,x03,y03;
  FILE *ft;

  for (int a=0; a<8; a++) {

  if (a == 0) strcpy(filename,"../align/algpar_50296.dat");
  if (a == 1) strcpy(filename,"../align/algpar_60067.dat");
  if (a == 2) strcpy(filename,"../align/algpar_60625.dat");
  if (a == 3) strcpy(filename,"../align/algpar_60592.dat");
  if (a == 4) strcpy(filename,"../align/algpar_60603.dat");
  if (a == 5) strcpy(filename,"../align/algpar_60614.dat");
  if (a == 6) strcpy(filename,"../align/algpar_60637.dat");
  if (a == 7) strcpy(filename,"../align/algpar_60644.dat");

  printf("Lecture de %s\n",filename);
  ft = fopen(filename,"r");
  if(!ft){ 
    printf("Error cannot open %s \n",filename);
    return;
  }

  for (int i=0; i<9; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f %f %f \n",&x00,&y00,
   &x01,&y01,&x02,&y02,&x03,&y03);
   algpar_fich[a][i][0] = x00;
   algpar_fich[a][i+1][0] = y00;
   algpar_fich[a][i][1] = x01;
   algpar_fich[a][i+1][1] = y01;
   algpar_fich[a][i][2] = x02;
   algpar_fich[a][i+1][2] = y02;
   algpar_fich[a][i][3] = x03;
   algpar_fich[a][i+1][3] = y03;
   printf("%f %f %f %f %f %f %f %f\n",algpar_fich[a][i][0],algpar_fich[a][i+1][0],
    algpar_fich[a][i][1],algpar_fich[a][i+1][1],algpar_fich[a][i][2],algpar_fich[a][i+1][2],
    algpar_fich[a][i][3],algpar_fich[a][i+1][3]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f\n",&x00,&x01,&x02,&x03);
  algpar_fich[a][10][0] = x00;
  algpar_fich[a][10][1] = x01;
  algpar_fich[a][10][2] = x02;
  algpar_fich[a][10][3] = x03;
  printf("%f %f %f %f\n",algpar_fich[a][10][0],algpar_fich[a][10][1],algpar_fich[a][10][2],algpar_fich[a][10][3]);
  fclose(ft);

}

  return;
}

double Event::ligne_projx(int i, int j, int k) {
  double xpos = -999;
  double m,b;
  if (apos(i,2) == apos(j,2)) return(xpos);
  m = (apos(i,0)-apos(j,0))/(apos(i,2)-apos(j,2));
  b = apos(j,2)*apos(i,0) - apos(i,2)*apos(j,0);
  b/=(apos(j,2)-apos(i,2));
  xpos = m*apos(k,2) + b;
  //  printf("neven %d i %d j %d k %d xpos %f\n",nevent,i,j,k,xpos);
  return(xpos);
}

double Event::ligne_projy(int i, int j, int k) {
  double ypos = -999;
  double m,b;
  if (apos(i,2) == apos(j,2)) return(ypos);
  m = (apos(i,1)-apos(j,1))/(apos(i,2)-apos(j,2));
  b = apos(j,2)*apos(i,1) - apos(i,2)*apos(j,1);
  b/=(apos(j,2)-apos(i,2));
  ypos = m*apos(k,2) + b;
  return(ypos);
}

double Event::ligne_projxz(int i, int j, float z) {
  double xpos = -999;
  double m,b;
  //  printf("x1 %f y1 %f z1 %f x2 %f y2 %f z2 %f\n",apos(i,0),
  //	 apos(i,1),apos(i,2),apos(j,0),apos(j,1),apos(j,2));
  if (apos(i,2) == apos(j,2)) return(xpos);
  m = (apos(i,0)-apos(j,0))/(apos(i,2)-apos(j,2));
  b = apos(j,2)*apos(i,0) - apos(i,2)*apos(j,0);
  b/=(apos(j,2)-apos(i,2));
  xpos = m*z + b;
 //  printf("neven %d i %d j %d z %f xpos %f\n",nevent,i,j,z,xpos);
  return(xpos);
}

double Event::ligne_projxz(int h, int i, int j, float z) {
  double xpos = -999;
  double m,b;
  //  printf("h %d x1 %f y1 %f z1 %f x2 %f y2 %f z2 %f\n",h,apos(h,i,0),
  //	 apos(h,i,1),apos(h,i,2),apos(h,j,0),apos(h,j,1),apos(h,j,2));
  if (apos(h,i,2) == apos(h,j,2)) return(xpos);
  m = (apos(h,i,0)-apos(h,j,0))/(apos(h,i,2)-apos(h,j,2));
  b = apos(h,j,2)*apos(h,i,0) - apos(h,i,2)*apos(h,j,0);
  b/=(apos(h,j,2)-apos(h,i,2));
  xpos = m*z + b;
  //  printf("neven %d i %d j %d z %f xpos %f\n",nevent,i,j,z,xpos);
  return(xpos);
}

double Event::ligne_projyz(int i, int j, float z) {
  double ypos = -999;
  double m,b;
  if (apos(i,2) == apos(j,2)) return(ypos);
  m = (apos(i,1)-apos(j,1))/(apos(i,2)-apos(j,2));
  b = apos(j,2)*apos(i,1) - apos(i,2)*apos(j,1);
  b/=(apos(j,2)-apos(i,2));
  ypos = m*z + b;
  return(ypos);
}

double Event::ligne_projyz(int h, int i, int j, float z) {
  double ypos = -999;
  double m,b;
  if (apos(h,i,2) == apos(h,j,2)) return(ypos);
  m = (apos(h,i,1)-apos(h,j,1))/(apos(h,i,2)-apos(h,j,2));
  b = apos(h,j,2)*apos(h,i,1) - apos(h,i,2)*apos(h,j,1);
  b/=(apos(h,j,2)-apos(h,i,2));
  ypos = m*z + b;
  return(ypos);
}

double Event::div_dxdz(int i, int j) {

  double dx = apos(j,0) - apos(i,0);
  double dz = apos(j,2) - apos(i,2);
  double val = dx/dz;
  if (i > j) val=-val;;
  return(val);
}

double Event::div_dxdz(int h, int i, int j) {

  double dx = apos(h,j,0) - apos(h,i,0);
  double dz = apos(h,j,2) - apos(h,i,2);
  double val = dx/dz;
  if (i > j) val=-val;;
  return(val);
}

double Event::div_dydz(int i, int j) {

  double dy = apos(j,1) - apos(i,1);
  double dz = apos(j,2) - apos(i,2);
  double val = dy/dz;
  if (i > j) val=-val;;
  return(val);
}

double Event::div_dydz(int h, int i, int j) {

  double dy = apos(h,j,1) - apos(h,i,1);
  double dz = apos(h,j,2) - apos(h,i,2);
  double val = dy/dz;
  if (i > j) val=-val;;
  return(val);
}

double Event::div_dxdz_lin() {

   Double_t *xp = new Double_t[3];
   Double_t *zp = new Double_t[3];
   Double_t *e = new Double_t[3];
   int n = 0;
   for (int i=1; i<4; i++) {
     xp[n] = apos(i,0);
     zp[n] = apos(i,2);
     e[n] = 1.;
     n++;
   }
   //   printf("fit dxdz\n");
   TLinearFitter *fitxz = new TLinearFitter(1);
  //     TLinearFitter *resfitx = new TLinearFitter(1,"pol1");
  //  resfit->StoreData(0);  
   fitxz->AssignData(n,1,zp,xp,e);
   fitxz->SetFormula("pol1");
   fitxz->Eval();
       //       printf("iret %d\n",iret);
   //   float chisqr=fitxz->GetChisquare();
   //   fitxz->PrintResults(3,0.);
   TVectorD retpar;
   fitxz->GetParameters(retpar);
       //       printf("chisqr %f parametres %f %f\n",chisqr,retpar[0],retpar[1]);
   double val=retpar[1];
   delete fitxz;
   fitxz=NULL;
   return(val);
}

double Event::div_dxdz_lin(int h) {

  //   Double_t *xp = new Double_t[3];
  //   Double_t *zp = new Double_t[3];
  //   Double_t *e = new Double_t[3];
   Double_t xp[3];
   Double_t zp[3];
   Double_t e[3];

   int n = 0;
   for (int i=1; i<4; i++) {
     xp[n] = apos(h,i,0);
     zp[n] = apos(h,i,2);
     e[n] = 1.;
     n++;
   }
   //   printf("fit dxdz\n");
   TLinearFitter fitxz(1);
   //   TLinearFitter *fitxz = new TLinearFitter(1);
  //     TLinearFitter *resfitx = new TLinearFitter(1,"pol1");
  //  resfit->StoreData(0);  
   //   fitxz->AssignData(n,1,zp,xp,e);
   //   fitxz->SetFormula("pol1");
   //   fitxz->Eval();
   fitxz.AssignData(n,1,zp,xp,e);
   fitxz.SetFormula("pol1");
   fitxz.Eval();
       //       printf("iret %d\n",iret);
   //   float chisqr=fitxz->GetChisquare();
   //   fitxz->PrintResults(3,0.);
   TVectorD retpar;
   //   fitxz->GetParameters(retpar);
   fitxz.GetParameters(retpar);
       //       printf("chisqr %f parametres %f %f\n",chisqr,retpar[0],retpar[1]);
   double val=retpar[1];
   //   delete fitxz;
   //   fitxz=NULL;
   //   delete xp;
   //   delete zp;
   //   delete e;
   return(val);
}

double Event::div_dydz_lin() {

  //   Double_t *yp = new Double_t[3];
  //   Double_t *zp = new Double_t[3];
  //   Double_t *e = new Double_t[3];
   Double_t yp[3];
   Double_t zp[3];
   Double_t e[3];
   int n = 0;
   for (int i=1; i<4; i++) {
     yp[n] = apos(i,1);
     zp[n] = apos(i,2);
     e[n] = 1.;
     n++;
   }
   //   printf("fit dydz\n");
   TLinearFitter fityz(1);
  //     TLinearFitter *resfitx = new TLinearFitter(1,"pol1");
  //  resfit->StoreData(0);  
   fityz.AssignData(n,1,zp,yp,e);
   fityz.SetFormula("pol1");
   fityz.Eval();
       //       printf("iret %d\n",iret);
   //   float chisqr=fitxz->GetChisquare();
   //   fityz->PrintResults(3,0.);
   TVectorD retpar;
   fityz.GetParameters(retpar);
       //       printf("chisqr %f parametres %f %f\n",chisqr,retpar[0],retpar[1]);
   double val= retpar[1];
   //   delete fityz;
   return(val);
}

double Event::div_dydz_lin(int h) {

  //   Double_t *yp = new Double_t[3];
  //   Double_t *zp = new Double_t[3];
  //   Double_t *e = new Double_t[3];
   Double_t yp[3];
   Double_t zp[3];
   Double_t e[3];
   int n = 0;
   for (int i=1; i<4; i++) {
     yp[n] = apos(h,i,1);
     zp[n] = apos(h,i,2);
     e[n] = 1.;
     n++;
   }
   //   printf("fit dydz\n");
   TLinearFitter fityz(1);
  //     TLinearFitter *resfitx = new TLinearFitter(1,"pol1");
  //  resfit->StoreData(0);  
   fityz.AssignData(n,1,zp,yp,e);
   fityz.SetFormula("pol1");
   fityz.Eval();
       //       printf("iret %d\n",iret);
   //   float chisqr=fitxz->GetChisquare();
   //   fityz->PrintResults(3,0.);
   TVectorD retpar;
   fityz.GetParameters(retpar);
       //       printf("chisqr %f parametres %f %f\n",chisqr,retpar[0],retpar[1]);
   double val= retpar[1];
   //   delete fityz;
   return(val);
}




