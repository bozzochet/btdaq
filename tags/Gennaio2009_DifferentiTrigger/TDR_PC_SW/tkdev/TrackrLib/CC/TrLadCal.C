//////////////////////////////////////////////////////////////////////////
///
///\file  TrLadCal.C
///\brief Source file of TrLadCal class
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 AO  Some methods are added 
///
//////////////////////////////////////////////////////////////////////////
#include "TrLadCal.h"
#include "TkDBc.h"

#include <cstdio>
#include <string>
#include <cstring>    // for memset
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

ClassImp(TrLadCal);

int TrLadCal::dead=0x2;


geant TrLadCal::_getnum(geant * pp, int ii){
  // if (!_filled||ii<0||ii>1023) return -9999;
  return pp[ii];
}


short int TrLadCal::_getnum(short int * pp, int ii){
  // if (!_filled||ii<0||ii>1023) return -9999;
  return pp[ii];
}


void TrLadCal::_setnum(geant * pp, int ii,geant val){
  if (!_filled||ii<0||ii>1023) return;
  pp[ii]=val;
}

void TrLadCal::_setnum(short int * pp, int ii,short int val){
  if (!_filled||ii<0||ii>1023) return;
  pp[ii]=val;
}


TrLadCal::TrLadCal(short int hwid) {
  HwId=hwid;
  _filled=0;
  Clear();
}


void TrLadCal::Fill(CaloutDSP* cc) {
  for (int ii=0;ii<1024;ii++){
    _Pedestal[ii]  = cc->ped[ii]/8.;
    _SigmaRaw[ii]  = cc->rsig[ii]/8./(cc->sigrawthres*1.);
    _Sigma[ii]     = cc->sig[ii]/(cc->lowthres*1.);
    _Status[ii]    = cc->status[ii];
  }

  dspver=cc->dspver;
  lowthres=cc->lowthres;
  highthres=cc->highthres;
  sigrawthres=cc->sigrawthres;
  calstatus=cc->calstatus;    
  
  _filled=1;
}

void TrLadCal::CopyCont(TrLadCal& orig){
  for (int ii=0;ii<1024;ii++){
    _Pedestal[ii]   = orig._Pedestal[ii];
    _SigmaRaw[ii]   = orig._SigmaRaw[ii];
    _Sigma[ii]      = orig._Sigma[ii];
    _Status[ii]     = orig._Status[ii];
  }

}
TrLadCal::TrLadCal(const TrLadCal& orig):TObject(orig){
  HwId  = orig.HwId;
  for (int ii=0;ii<1024;ii++){
    _Pedestal[ii]   = orig._Pedestal[ii];
    _SigmaRaw[ii]   = orig._SigmaRaw[ii];
    _Sigma[ii]      = orig._Sigma[ii];
    _Status[ii]     = orig._Status[ii];
  }

  dspver=orig.dspver;
  lowthres=orig.lowthres;
  highthres=orig.highthres;
  sigrawthres=orig.sigrawthres;
  calstatus=orig.calstatus;    
}


void TrLadCal::Clear(){
  _filled=0;
  memset(_Pedestal,0,sizeof(_Pedestal[0])*1024);
  memset(_SigmaRaw,0,sizeof(_SigmaRaw[0])*1024);
  memset(_Sigma   ,0,sizeof(_Sigma[0])*1024);
  memset(_Status  ,0,sizeof(_Status[0])*1024);
  dspver=0;
  lowthres=0.;
  highthres=0.;
  sigrawthres=0.;
  calstatus=0;
}


void TrLadCal::PrintInfo(int long_format){
  TkLadder* lad=TkDBc::Head->FindHwId(HwId);
  if(!lad) {
    printf("TrLadCal::PrintInfo() - Error - Can't find the ladder with HwId %4d\n",HwId);
    return;
  }
  printf("Ladder Name: %s  Filled: %d\n",lad->name,_filled);
  printf("Layer: %d Slot: %2d  Side: %d \n", 
	 lad->GetLayer(),lad->GetSlot(),lad->GetSide()); 
  printf("Octant: %d %s Crate: %d TDR:  %2d  PwGroup: %d PwgrupPos: %d\n",
	 lad->GetOctant(),TkDBc::Head->GetOctName(lad->GetOctant()),
	 lad->GetCrate(),lad->GetTdr(),
	 lad->GetPwGroup(),lad->GetPwgroupPos());
  if(long_format){
    printf(" Chan     Ped  SigRaw  Sigma     Bit0 Bit1 Bit2 Bit3 Bit4 Bit5 Bit6 Bit7\n");
    for (int ii=0;ii<1024;ii++)
      // printf(" %4d %7.3f  %7.3f %7.3f  %4d \n",
      // 	     ii,Pedestal(ii),SigmaRaw(ii),Sigma(ii),Status(ii));
      printf(" %4d %7.3f  %7.3f %7.3f  %1d %1d %1d %1d %1d %1d %1d %1d \n",
	     ii,GetPedestal(ii),GetSigmaRaw(ii),GetSigma(ii),
	     GetStatus(ii)&1,GetStatus(ii)>>1&1,GetStatus(ii)>>2&1,GetStatus(ii)>>3&1,
	     GetStatus(ii)>>4&1,GetStatus(ii)>>5&1,GetStatus(ii)>>6&1,GetStatus(ii)>>7&1);
  }
}



TrLadCal& TrLadCal::operator-(TrLadCal &orig){
  TrLadCal* pp= new TrLadCal(*this);
  for(int ii=0;ii<1024;ii++){
     pp->SetPedestal (ii,GetPedestal(ii)-orig.Pedestal(ii));
     pp->SetSigmaRaw (ii,GetSigmaRaw(ii)-orig.SigmaRaw(ii));
     pp->SetSigma    (ii,GetSigma(ii)-orig.Sigma(ii));
     pp->SetStatus   (ii,GetStatus(ii)^orig.Status(ii));
  }
  return *pp;
}
 
TrLadCal& TrLadCal::operator=(TrLadCal& orig){
  HwId=orig.HwId;
  for (int ii=0;ii<1024;ii++){
    SetPedestal(ii,orig._Pedestal[ii]);
    SetSigmaRaw(ii,orig._SigmaRaw[ii]);
    SetSigma(ii,orig._Sigma[ii]);
    SetStatus(ii,orig._Status[ii]);
  }
  return *this;
}


//Finds the kth smallest number 
short int TrLadCal::DSPSmallest(short int a[], int n, int k){
  short int i,j,l,m ;
  short int x ;
  l=0 ; m=n-1 ;
  while (l<m) {
    x=a[k] ;
    i=l ;
    j=m ;
    do {
      while (a[i]<x) i++ ;
      while (x<a[j]) j-- ;
      if (i<=j) {
	//	ELEM_SWAP(a[i],a[j]) ;
	int tt=a[i];
	a[i]=a[j];
	a[j]=tt;
	i++ ; j-- ;
      }
    } while (i<=j) ;
    if (j<k) l=i ;
    if (k<i) m=j ;
  }
  return a[k] ;
}

geant TrLadCal::GetDSPSigmaRawOnMediane(int ii) {
  int iVA   = ii/64;
  int first = iVA*64;
  int last  = (iVA+1)*64;
  short int raw[64];
  for (int jj=first; jj<last; jj++) raw[jj-first] = (short int) (8*GetSigmaRaw(jj)); 
  short int mediane  = DSPSmallest(raw,64,32);
  short int sigmaraw = (short int) (8*GetSigmaRaw(ii));
  return 1.*sigmaraw/mediane;
}

geant TrLadCal::GetDSPSigmaOnMediane(int ii) {
  int iADC  = (ii<640) ? ii/320       : 3;
  int first = (iADC<2) ? iADC*320     : 640;
  int last  = (iADC<2) ? (iADC+1)*320 : 1024;
  short int sig[384];
  for (int jj=first; jj<last; jj++) sig[jj-first] = (short int) (8*GetSigma(jj));
  short int mediane = (iADC<2) ? DSPSmallest(sig,320,160) : DSPSmallest(sig,384,192);
  short int sigma   = (short int) (8*GetSigma(ii));
  return 1.*sigma/mediane;
}


geant TrLadCal::GetSigmaMean(int side) {
  int first = (side==0) ? 640  : 0;
  int last  = (side==0) ? 1023 : 639;
  int   ngoodchannels = 0;
  geant mean = 0.;
  for (int ii=first; ii<=last; ii++) {
    if (GetStatus(ii)!=0) continue;
    mean += GetSigma(ii);
    ngoodchannels++;
  }
  return mean/ngoodchannels;
}


geant TrLadCal::GetSigmaRMS(int side) {
  int first = (side==0) ? 640  : 0;
  int last  = (side==0) ? 1023 : 639;
  int   ngoodchannels = 0;
  geant mean = GetSigmaMean(side);
  geant rms = 0.;
  for (int ii=first; ii<=last; ii++) {
    if (GetStatus(ii)!=0) continue;
    rms += pow(GetSigma(ii)-mean,2);
    ngoodchannels++;
  }
  return sqrt(rms/ngoodchannels);
}
  
int TrLadCal::GetnChannelsByStatus(int statusbit) {
  int nchannels = 0;
  for (int ii=0; ii<=1023; ii++) {
    if (!(GetStatus(ii)>>statusbit&1)) continue;
    nchannels++;
  }
  return nchannels;
} 

int  TrLadCal::Cal2Lin(float* offset){

  if(!offset) return -1;

  offset[0]= GetHwId();
  offset[1]= (float) dspver;
  offset[2]= lowthres;
  offset[3]= highthres;
  offset[4]= sigrawthres;
  offset[5]= (float) calstatus;

  float* off2=&(offset[6]);
  for (int ii=0;ii<1024;ii++){
    off2[ii]          = _Pedestal[ii];
    off2[ii+  1024]   = _Sigma[ii];
    off2[ii+2*1024]   = _SigmaRaw[ii];
    off2[ii+3*1024]   = (float)_Status[ii];
  }
  return 0;
}



int  TrLadCal::Lin2Cal(float* offset){

  if(!offset) return -1;

  HwId        = offset[0];
  dspver      = (int)offset[1];
  lowthres    = offset[2];
  highthres   = offset[3];
  sigrawthres = offset[4];
  calstatus   = (int)offset[5];


  float* off2=&(offset[6]);
  for (int ii=0;ii<1024;ii++){
     _Pedestal[ii] =       off2[ii];
     _Sigma[ii]    =       off2[ii+  1024];
     _SigmaRaw[ii] =       off2[ii+2*1024];
     _Status[ii]   = (int) off2[ii+3*1024];
  }
  return 0;

}
