/// $Id: TrRawCluster.C,v 1.21 2009/01/12 15:22:49 oliva Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TrRawCluster.C
///\brief Source file of AMSTrRawCluster class
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/18 SH  Updates for TrCluster support
///\date  2008/01/18 AO  Some analysis methods 
///\date  2008/06/19 AO  Using TrCalDB instead of data members 
///
/// $Date: 2009/01/12 15:22:49 $
///
/// $Revision: 1.21 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrRawCluster.h"
#include <iostream>

using namespace std;
ClassImp(AMSTrRawCluster);

TrCalDB* AMSTrRawCluster::_trcaldb = NULL;
string AMSTrRawCluster::sout;

AMSTrRawCluster::AMSTrRawCluster(void) {
  Clear();
}

void AMSTrRawCluster::Clear() {
  _tkid    =  0;
  _address = -1;
  _nelem   =  0;
  _signal.clear();
}

AMSTrRawCluster::AMSTrRawCluster(const AMSTrRawCluster &orig) : AMSlink(orig) {
  _tkid    = orig._tkid;
  _address = orig._address;
  _nelem   = orig._nelem;
  for (int i = 0; i < _nelem; i++) _signal.push_back(orig._signal.at(i));
}

AMSTrRawCluster::AMSTrRawCluster(int tkid, int add, int nelem, short* adc) {
  _tkid    =  tkid;
  _address =  add;
  _nelem   =  nelem;
  _signal.reserve(_nelem);
  if(adc) for (int i = 0; i < _nelem; i++) _signal.push_back(adc[i]/8.);
}

AMSTrRawCluster::AMSTrRawCluster(int tkid, int add, int nelem, float *adc) {
  _tkid    =  tkid;
  _address =  add;
  _nelem   =  nelem;
  _signal.reserve(_nelem);
  if(adc) for (int i = 0; i < _nelem; i++) _signal.push_back(adc[i]);
}

float AMSTrRawCluster::GetNoise(int ii) {
  if (_trcaldb==0) {
    printf("TrRawClusters::GetStatus Error, no _trcaldb specified.\n");
    return -9999.; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("AMSTrRawCluster::GetNoise, WARNING calibration not found!!\n"); return -9999;} 
  int address = _address+ii;
  return (float)ladcal->GetSigma(address);
}

short AMSTrRawCluster::GetStatus(int ii) {
  if (_trcaldb==0) {
    printf("TrRawClusters::GetStatus Error, no _trcaldb specified.\n");
    return -1; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("AMSTrRawCluster::GetNoise, WARNING calibration not found!!\n"); return -9999;} 
  int address = _address+ii;
  return (short)ladcal->GetStatus(address);
}

std::ostream &AMSTrRawCluster::putout(std::ostream &ostr) const {
  return ostr << "TkID: " << _tkid << " "
	      << "Addr: " << _address << " "
	      << "Nelm: " << _nelem << std::endl;
}

void AMSTrRawCluster::Print() { 
  Info();
  cout<<sout;
}

void AMSTrRawCluster::Info() { 
  char msg[1000];
  sout.clear();
  sprintf(msg,"TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d\n",
	 GetTkId(),GetSide(),GetAddress(),GetNelem());
  sout.append(msg);
  for (int ii=0; ii<GetNelem(); ii++) {
    sprintf(msg,"Address: %4d  Signal: %10.5f  Sigma: %10.5f  Status: %3d\n",
	   ii+GetAddress(),GetSignal(ii),GetSigma(ii),GetStatus(ii));
    sout.append(msg);
  }
}

int AMSTrRawCluster::GetSeedIndex(float thseed){
  float maxadc  = -1000.;
  int   seedadd = -1;
#ifdef PZVERS
  for (int ii=0; ii<GetNelem(); ii++) { 
    if ( (GetSN(ii)>3.5)&&(GetStatus(ii)==0) ) { 
      maxadc  = GetSignal(ii); 
      seedadd = ii; 
      break;
    }
  }	 
  if(seedadd>=0) {
    if(GetSN(seedadd)>thseed) return seedadd;
    else return seedadd*-1.;
  }
  return -1;
#else
  for (int ii=0; ii<GetNelem(); ii++) { 
    if ( (GetSignal(ii)>maxadc)&& (GetSN(ii)>thseed) &&(GetStatus(ii)==0) ) { 
      maxadc  = GetSignal(ii); 
      seedadd = ii; 
    }
  }
  return seedadd;
#endif
}

int AMSTrRawCluster::GetSeedAddress(float thseed){
  Int_t cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1;
  return cstrip + GetAddress();
}

int AMSTrRawCluster::GetStatusnStrips(int nstrips, float thseed, float thneig){
  Int_t cstrip = GetSeedIndex(thseed);
  Int_t nleft  = GetLeftLength(thseed,thneig);
  Int_t nright = GetRightLength(thseed,thneig);
  if ( (cstrip<0)||(nleft<0)||(nright<0) ) return -1; 

  Int_t lindex = 0;
  Int_t rindex = 0;
  Int_t nside  = 0;
  Int_t sum    = 0;
  // number strip selection
  if (nstrips%2){ // odd (same number of left/right strips)
    nside  = (nstrips-1)/2;
    lindex = cstrip-nside;
    rindex = cstrip+nside;
  }
  else { // even (take the highest strip between the n-ths left and right strips)
    nside = nstrips/2;
    if ( (nleft<=nside)&&(nright<=nside) ) { // se posso fare il confronto
      if (GetSignal(cstrip-nside)>=GetSignal(cstrip+nside)){
	lindex = cstrip-nside;
	rindex = cstrip+nside - 1;
      }
      else {
	lindex = cstrip-nside + 1;
	rindex = cstrip+nside;
      }
    }
    else { // altrimenti prendo il massimo dal lato in cui posso
      lindex = cstrip-nside;
      rindex = cstrip+nside;
    }
  }
  // check sull'overflow
  if (lindex<0)          lindex = 0;
  if (rindex>GetNelem()) rindex = GetNelem();
  for (int ii=lindex; ii<rindex; ii++) sum += GetStatus(ii);

  return sum;
}

int AMSTrRawCluster::GetLength(float thseed,float thneig) {
  if ( (GetLeftLength(thseed,thneig)<0)||(GetRightLength(thseed,thneig)<0) ) return -1;
  return GetLeftLength(thseed,thneig) + GetRightLength(thseed,thneig) + 1;
}

int AMSTrRawCluster::GetLeftLength(float thseed,float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1;
  int length = 0;
  for (int ii=cstrip-1; ii>=0; ii--) {
    if (GetSN(ii)<thneig) break;
    length++;
  }
  return length;
}

int AMSTrRawCluster::GetRightLength(float thseed,float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1;
  int length = 0;
  for (int ii=cstrip+1; ii<GetNelem(); ii++) {
    if (GetSN(ii)<thneig) break;
    length++;
  }
  return length;
}

float AMSTrRawCluster::GetTotSignal(float thseed,float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  float sum = GetSignal(cstrip);
  for (int ii=cstrip-1; ii>=0; ii--) {
    if (GetSN(ii)<thneig) break;
    sum += GetSignal(ii);
  }
  for (int ii=cstrip+1; ii<GetNelem(); ii++) {
    if (GetSN(ii)<thneig) break;
    sum += GetSignal(ii);
  }
  return sum;
}

float AMSTrRawCluster::GetTotSignal2(float thseed,float thneig) {
  int cstrip = abs(GetSeedIndex(thseed));
  float sum = GetSignal(cstrip);
  float left=0;
  float right=0;
  float add=0;
  if(cstrip>0) left= GetSignal(cstrip-1);
  else left=0;
  if(cstrip<((int)_signal.size()-1)) right= GetSignal(cstrip+1);
  else right=0;

  if( right > left) add=right;
  else add=left;
  if(add>0) sum+=add;
    
  
  if (GetSeedIndex(thseed)<0) return sum*-1.;
  return sum;
}

float AMSTrRawCluster::GetSeedSignal(float thseed){
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  return GetSignal(cstrip);
}

float AMSTrRawCluster::GetSeedNoise(float thseed){
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  return GetNoise(cstrip);
}

float AMSTrRawCluster::GetSeedSN(float thseed){
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  return GetSN(cstrip);
}

float AMSTrRawCluster::GetEta(float thseed,float thneig){
  /*
    eta = center of gravity with the two higher strips = Q_{R} / ( Q_{L} + Q_{R} )

       _                                    _
     l|c|r          c*0 + r*1    r        l|c|r            l*0 + c*1    c
      | |_    eta = --------- = ---       _| |       eta = --------- = ---
     _| | |           c + r     c+r      | | |_              l + c     l+c
  __|_|_|_|__                          __|_|_|_|__
       0 1                                0 1

       - eta is 1 when the particle is near to the right strip,
       while is approx 0 when is near to the left strip
  */
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  int nleft  = GetLeftLength(thseed,thneig);
  int nright = GetRightLength(thseed,thneig);
  // 1 strip
  if ( (nleft<=0)&&(nright<=0) ) return 0.; 
  // 2 strips
  if (nleft<=0)  return GetSignal(cstrip+1)/(GetSignal(cstrip+1) + GetSignal(cstrip));
  if (nright<=0) return GetSignal(cstrip)/(GetSignal(cstrip-1) + GetSignal(cstrip));
  // >2 strips
  if ( GetSignal(cstrip+1)>GetSignal(cstrip-1) ) return GetSignal(cstrip+1)/(GetSignal(cstrip+1) + GetSignal(cstrip));
  else                                           return GetSignal(cstrip)/(GetSignal(cstrip-1) + GetSignal(cstrip));
  return -1.;
}

float AMSTrRawCluster::GetEtaAddress(float thseed, float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  int address = GetSeedAddress(thseed);
  int nleft   = GetLeftLength(thseed,thneig);
  int nright  = GetRightLength(thseed,thneig);
  // 1 strip
  if ( (nleft<=0)&&(nright<=0) ) return (float)address; 
  // 2 strips
  if (nleft<=0)  return ( (address+1.)*GetSignal(cstrip+1) + address*GetSignal(cstrip) )/( GetSignal(cstrip+1) + GetSignal(cstrip) );
  if (nright<=0) return ( (address-1.)*GetSignal(cstrip-1) + address*GetSignal(cstrip) )/( GetSignal(cstrip-1) + GetSignal(cstrip) );
  // >2 strips
  if ( GetSignal(cstrip+1)>GetSignal(cstrip-1) ) 
    return ( (address+1.)*GetSignal(cstrip+1) + address*GetSignal(cstrip) )/( GetSignal(cstrip+1) + GetSignal(cstrip) );
  else 
    return ( (address-1.)*GetSignal(cstrip-1) + address*GetSignal(cstrip) )/( GetSignal(cstrip-1) + GetSignal(cstrip) );
  return -1.;
}

integer AMSTrRawCluster::lvl3format(int16 * adc, integer nmax,int lvl3dcard_par  ,integer matchedonly){
  //
  // convert my stupid format to lvl3 one for shuttle flight (mb also stupid)(vc)
  //
  int16 pos =0;
  if (
      nmax-pos < 2+_nelem || _nelem > 63 || _nelem==0 
      ||
      //PZ FIXME      (GetSide()==0 && matchedonly && !checkstatus(MATCHED))
      (GetSide()==0 && matchedonly )
      ) return pos;
  
  adc[pos+1]=1000*_address+GetHwId();
  //if(id.getside())cout <<" haddr "<<id.gethaddr()<<" "<<id.gettdr()<<" "<<id.getcrate()<<" "<<id.getdrp()<<endl;
  integer imax=0;
  geant rmax=-1000000;
  int16 sn;
  for (int i=0;i<_nelem;i++){
    
    if(GetSigma(i) && GetSN(i) > rmax ){
      rmax=GetSN(i);
      sn=(int16)(rmax+0.5);
      if(sn>63)sn=63;
      if(sn<0)sn=0;
      imax=i;
    }
    adc[pos+i+2]=int16(GetSignal(i));
  }
  //  if(id.getside()==1)cout <<"sn "<<sn<<endl;

  if(lvl3dcard_par>0)
    adc[pos]=(_nelem-1) | (int16u(GetSeedSN()*8)<<6);
  
  else if(lvl3dcard_par<0)
    adc[pos]=(_nelem-1) | (  int16u(GetSeedSN()) <<6);
  
  else  
    adc[pos]=(_nelem-1) | (imax<<6);
  
  pos+=2+_nelem;
 return pos;
}




