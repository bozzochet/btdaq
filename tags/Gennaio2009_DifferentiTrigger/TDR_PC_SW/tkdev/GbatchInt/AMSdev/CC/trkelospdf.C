#include "amsdbc.h"
#include "trkelospdf.h"
#include <math.h>
#include "dcards.h"
#include "charge.h"

using namespace AMSChargConst;
using namespace trconst;

TrkElosPDF TrkElosPDF::TrkEPDFs[MaxZTypes];
//-----------------------------------------------
TrkElosPDF::TrkElosPDF(int ich, int ch, int nb[], geant stp[],
                        geant nrm[], geant slop[], 
			       geant distr[trconst::TrkTypes][trconst::TrkPdfBins]){
//valid only for current(inconvenient) format of input Elos-files
  ichar=ich;
  charge=ch;
  for(int itp=0;itp<TrkTypes;itp++)nbins[itp]=nb[itp];
  for(int itp=0;itp<TrkTypes;itp++)stpx[itp]=stp[itp];
  for(int itp=0;itp<TrkTypes;itp++)xmin[itp]=0;// !!!
  for(int itp=0;itp<TrkTypes;itp++)norm[itp]=nrm[itp];
  for(int itp=0;itp<TrkTypes;itp++)slope[itp]=slop[itp];
  for(int itp=0;itp<TrkTypes;itp++){
    for(int i=0;i<nb[itp];i++)elpdf[itp][i]=number(distr[itp][i]/nrm[itp]);
  }
//  cout<<"iZ/Z="<<ich<<" "<<ch<<endl;
//  cout<<"Typ=1:nb/bw/xmin="<<nbins[0]<<" "<<stpx[0]<<" "<<xmin[0]<<endl;
//  cout<<"      nor/slp="<<norm[0]<<" "<<slope[0]<<endl;
//  cout<<"  dis(0)/dis(n-1)="<<distr[0][0]<<" "<<distr[0][nbins[0]-1]<<endl;
}
//---
void TrkElosPDF::build(){// create TofElosPDF-objects array for real/mc data
//
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  int mrfp;
  char in[2]="0";
  char inum[11];
  int mcvn,rlvn,dig,endflab(0);
  integer charge,nbns;
  int chref;
  geant xmin(0),norm[TrkTypes][MaxZTypes],binw[TrkTypes][MaxZTypes];
  geant slop[TrkTypes][MaxZTypes],distr[TrkTypes][MaxZTypes][TrkPdfBins];
  geant bw[TrkTypes],nm[TrkTypes],sl[TrkTypes],dis[TrkTypes][TrkPdfBins];
  int nb[TrkTypes];
//
  strcpy(inum,"0123456789");
//
//------------------------------------------------
 strcpy(name,"trkpdff");//generic ElosPDF's raw file name
 if(AMSJob::gethead()->isMCData())           // for MC-data
 {
   cout <<" TrkElosPDF::build: Elos-vx-Charge distr. for MC-data are requested"<<endl;
   mcvn=CHARGEFITFFKEY.TrkPDFileMCVers%1000;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real-data
 {
   cout <<" TrkElosPDF::build: Elos-vx-Charge distr. for Real-data are requested"<<endl;
   rlvn=CHARGEFITFFKEY.TrkPDFileRDVers%1000;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   strcpy(fname,AMSDATADIR.amsdatadir);
//   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream pdfile(fname,ios::in); // open file for reading
   if(!pdfile){
     cerr <<"TrkElosPDF::build: Missing Elos-vx-Charge distr. file "<<fname<<endl;
     exit(1);
   }
//
  nbns=TrkPdfBins;
  for(int itp=0;itp<TrkTypes;itp++){
    for(int ich=0;ich<MaxZTypes;ich++)pdfile >> binw[itp][ich];
    for(int ich=0;ich<MaxZTypes;ich++)pdfile >> norm[itp][ich];
    for(int ich=0;ich<MaxZTypes;ich++)pdfile >> slop[itp][ich];
    for(int ich=0;ich<MaxZTypes;ich++){
      for(int ib=0;ib<nbns;ib++)pdfile >> distr[itp][ich][ib];
      if(pdfile.eof()){
        cerr<<"TrkElosPDF::build:Unexpected EOF"<<endl;
	exit(1);
      }
    }
  }
  for(int ich=0;ich<MaxZTypes;ich++){
    for(int itp=0;itp<TrkTypes;itp++)nb[itp]=nbns;
    for(int itp=0;itp<TrkTypes;itp++)bw[itp]=binw[itp][ich];
    for(int itp=0;itp<TrkTypes;itp++)nm[itp]=norm[itp][ich];
    for(int itp=0;itp<TrkTypes;itp++)sl[itp]=slop[itp][ich];
    for(int itp=0;itp<TrkTypes;itp++){
      for(int ib=0;ib<nb[itp];ib++)dis[itp][ib]=distr[itp][ich][ib];
    }
    chref=AMSCharge::ind2charge(1,ich+1);//charge from standard list("0"->TOF,"1"->TRK)
    TrkEPDFs[ich]=TrkElosPDF(ich+1, chref, nb, bw, nm, sl, dis);
  }
//
  pdfile>>endflab;
//
  pdfile.close();
//
  if(endflab==12345){
    cout<<"TrkElosPDF::build: Elos-vx-Charge distr. file is successfully read !"<<endl;
  }
  else{cout<<"TrkElosPDF::build: ERROR(READ-problem with Elos-vx-Charge distr. file)"<<endl;
    exit(1);
  }
//
}
//---
number TrkElosPDF::getlkhd(int nhits, int htype[], number ehit[], number beta){
  number eh,cor,fac,betapow,betacor,betamax(0.94),lkhd(0);
  int ia,htyp;
  betapow=1.78;//my estimation, works better than 5/3, the same for all nucl<=C12)
  betacor=ichar?pow(min(fabs(beta/betamax),1.),betapow):1;//corr to "mip"(=1 for ichar=0(electrons))
  for(int ih=0;ih<nhits;ih++){//hits loop
    htyp=htype[ih];
    if(htyp>=0){
      eh=ehit[ih]*betacor;
      ia=int(floor(eh/stpx[htyp]));
      if(ia<0)ia=0;
      cor=0;
      if(ia>=TrkPdfBins){
        cor=(ia+1-TrkPdfBins)*slope[htyp]*stpx[htyp];
	ia=TrkPdfBins-1;
      }
      lkhd+=-log(elpdf[htyp][ia])+cor;//no old "fac"-correction, because orig.dist. is normalized in constructor
    }
  }
  return lkhd;
}

