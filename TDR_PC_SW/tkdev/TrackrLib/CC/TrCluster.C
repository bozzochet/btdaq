/// $Id: TrCluster.C,v 1.27 2008/09/16 10:43:46 zuccon Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TrCluster.C
///\brief Source file of AMSTrCluster class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/02/18 AO  New data format 
///\date  2008/02/19 AO  Signal corrections
///\date  2008/02/26 AO  Eta based local coordinate (see TkCoo.h)
///\date  2008/03/01 AO  Added member _seedind
///\data  2008/03/06 AO  Changing some data members and methods
///\date  2008/03/06 AO  Changing some data members and methods
///\date  2008/03/31 AO  Eta and CofG methods changing
///\date  2008/04/11 AO  XEta and XCofG coordinate based on TkCoo
///\date  2008/06/19 AO  Using TrCalDB instead of data members 
///
/// $Date: 2008/09/16 10:43:46 $
///
/// $Revision: 1.27 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkCoo.h"
#include "TrCluster.h"

ClassImp(AMSTrCluster);

// Asimmetry correction defaults from test beam (no effect if {0.,0.})
float AMSTrCluster::AsimmetryCorr[2] = {0.027,0.034};
// By default tha gain correction is 1 (no effect)
float AMSTrCluster::GainCorr[2] = {1.00,1.00};
// Charge collection loss correction for protons (readout-middle-central)
float AMSTrCluster::ChargeLossCorr[2][3] = { {1.00,0.83,0.72}, {1.00,0.87,0.84} };

TrCalDB* AMSTrCluster::_trcaldb = NULL;
string AMSTrCluster::sout;


//--------------------------------------------------
AMSTrCluster::AMSTrCluster(void) {
  Clear();
}


//--------------------------------------------------
AMSTrCluster::AMSTrCluster(const AMSTrCluster &orig) :  AMSlink(orig) {
  _tkid    = orig._tkid;
  _side    = orig._side;
  _address = orig._address;
  _nelem   = orig._nelem;
  _seedind = orig._seedind;
  _gain    = orig._gain; 
  for (int i = 0; i<_nelem; i++) _signal.push_back(orig._signal.at(i));
  _clstatus  = orig._clstatus;
  _mult      = orig._mult;
  _coord     = orig._coord;
}


//--------------------------------------------------
AMSTrCluster::AMSTrCluster(int tkid, int side, int add, int seedind, unsigned int clstatus) : AMSlink() {
  Clear();
  _tkid    =  tkid;
  _side    =  side;
  _address =  add;
  _seedind =  seedind;
  _gain    =  8;
  _clstatus = clstatus;
}


//--------------------------------------------------
AMSTrCluster::AMSTrCluster(int tkid, int side, int add, int nelem, int seedind, 
			   float* adc, unsigned int clstatus) :  AMSlink() {
  Clear();
  _tkid    =  tkid;
  _side    =  side;
  _address =  add;
  _nelem   =  nelem;
  _seedind =  seedind;
  _signal.reserve(_nelem);
  _gain    =  8;
  for (int i = 0; i<_nelem; i++) _signal.push_back(adc[i]);
  _clstatus = clstatus;
  // multiplicity calculation
  _mult    = TkCoo::GetMaxMult(GetTkId(),GetAddress())+1;
  for (int imult=0; imult<_mult; imult++) _coord.push_back(GetXCofG(3,imult));  
}


//--------------------------------------------------
AMSTrCluster::~AMSTrCluster() {
  Clear();
}


//--------------------------------------------------
float AMSTrCluster::GetNoise(int ii) {
  if (_trcaldb==0) {
    printf("AMSTrClusters::GetStatus Error, no _trcaldb specified.\n");
    return -9999.; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("AMSTrCluster::GetNoise, WARNING calibration not found!!\n"); return -9999;}
  int address = _address+ii;
  return (float)ladcal->GetSigma(address);
}


//--------------------------------------------------
short AMSTrCluster::GetStatus(int ii) {
  if (_trcaldb==0) {
    printf("AMSTrClusters::GetStatus Error, no _trcaldb specified.\n");
    return -1; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("AMSTrCluster::GetNoise, WARNING calibration not found!!\n"); return -9999;}
  int address = _address+ii;
  return (short)ladcal->GetStatus(address);
}


//--------------------------------------------------
void AMSTrCluster::Clear() {
  _tkid    =  0;
  _address = -1;
  _nelem   =  0;
  _seedind =  0;
  _gain    =  8;
  _signal.clear();
  _clstatus = 0;
  _mult     = 0;
  _coord.clear();
}


//--------------------------------------------------
void AMSTrCluster::push_back(float adc) {
  _signal.push_back(adc);
  _nelem = (int) _signal.size();
}


//--------------------------------------------------
void AMSTrCluster::BuildCoordinates() {
  // multiplicity calculation
  _mult    = TkCoo::GetMaxMult(GetTkId(),GetAddress())+1;
  for (int imult=0; imult<_mult; imult++) _coord.push_back(GetXCofG(3,imult));  
}


//--------------------------------------------------
void AMSTrCluster::Print(const char* options) { 
  Info(options);
  cout <<sout;
}

void AMSTrCluster::Info(const char* options) { 
  char msg[1000];
  sout.clear();
  sprintf(msg,"TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d  ClStatus: %d\n",
	  GetTkId(),GetSide(),GetAddress(),GetNelem(),GetClusterStatus() );
  sout.append(msg);
  char isseed[10];
  for (int ii=0; ii<GetNelem(); ii++) {
    sprintf(isseed," ");
    if (ii==GetSeedIndex()) sprintf(isseed,"<<< SEED");
    sprintf(msg,"Address: %4d  Signal: %10.5f  Sigma: %10.5f  Status: %3d  %10s\n",
	   GetAddress(ii),GetSignal(ii,options),GetSigma(ii),GetStatus(ii),isseed);
    sout.append(msg);
  }
}


//--------------------------------------------------
std::ostream &AMSTrCluster::putout(std::ostream &ostr) const {
  return ostr << "TkID: " << GetTkId() << " "
	      << "Side: " << GetSide() << " "
	      << "Addr: " << GetAddress() << " "
	      << "Nelm: " << GetNelem() << " " 
	      << "ClStatus: " << GetClusterStatus() << std::endl;
}


//--------------------------------------------------
float AMSTrCluster::GetSignal(int ii, const char* options) {
  // parsing options
  bool ApplyAsimmetryCorr = false;
  bool ApplyGainCorr      = false;
  char character = ' ';
  int  cc = 0;
  while (character!='\0') {
    character = *(options+cc);
    if ( (character=='A')||(character=='a') ) ApplyAsimmetryCorr = true;
    if ( (character=='G')||(character=='g') ) ApplyGainCorr      = true;
    cc++;
  }
  float signal = _signal.at(ii);
  int   side   = GetSide();
  if (ApplyAsimmetryCorr) {
    // subtraction of a portion of the right strip
    if (ii>0) signal = signal - _signal.at(ii-1)*AsimmetryCorr[side];
  }
  // LoadGain() ???
  if (ApplyGainCorr) {
    signal = signal*GainCorr[side];
  }
  return signal;
}


//--------------------------------------------------
int AMSTrCluster::GetAddress(int ii) {
  int address = GetAddress() + ii;
  int side    = GetSide();
  // for cyclicity 
  if ( (side==0)&&(address>1023) ) address = address - 384; //  640 + (address - 1024);
  if ( (side==0)&&(address< 640) ) address = address + 384; // 1024 - (640  - address);
  if ( (side==1)&&(address> 639) ) address = address - 640; //    0 + (address -  640);
  if ( (side==1)&&(address<   0) ) address = address + 640; //  640 - (   0 - address);
  return address;
}

/*
//--------------------------------------------------
int AMSTrCluster::GetSeedIndex() {
  float maxadc  = -1000.;
  int   seedadd = -1;
  for (int ii=0; ii<GetNelem(); ii++) { 
    if ( (GetSignal(ii)>maxadc)&&(GetStatus(ii)==0) ) { 
      maxadc  = GetSignal(ii); 
      seedadd = ii; 
    } 
  }
  return seedadd;
}
*/


//--------------------------------------------------
int AMSTrCluster::GetSecondIndex() {
  int cstrip = GetSeedIndex();
  int nleft  = GetLeftLength();
  int nright = GetRightLength();
  // 1 strip
  if ( (nleft<=0)&&(nright<=0) ) return -1; 
  // 2 strips
  if (nleft<=0)  return cstrip+1;
  if (nright<=0) return cstrip-1;
  // >2 strips
  if ( GetSignal(cstrip+1)>GetSignal(cstrip-1) ) return cstrip+1;
  else                                           return cstrip-1;
  return -1;
}


//--------------------------------------------------
float AMSTrCluster::GetTotSignal(const char* options) {
  // parsing options
  bool ApplyChargeLossCorr = false;
  bool ApplyPNCorr         = false;
  char character = ' ';
  int  cc = 0;
  while (character!='\0') {
    character = *(options+cc);
    if ( (character=='L')||(character=='l') ) ApplyChargeLossCorr = true;
    if ( (character=='N')||(character=='n') ) ApplyPNCorr         = true;
    cc++;
  }
  float sum = 0;
  for (int ii=0; ii<GetNelem(); ii++) {
    sum += GetSignal(ii,options);
  } 
  if (ApplyChargeLossCorr) {
    int side     = GetSide();
    int etaindex = GetEtaIndex(options);
    if (etaindex>=0) sum = sum/ChargeLossCorr[side][etaindex];
  }
  return sum;
}


//--------------------------------------------------
float AMSTrCluster::GetEta(const char* options) {
  /*
    eta = center of gravity with the two higher strips = Q_{R} / ( Q_{L} + Q_{R} )
    
        _                                    _
      l|c|r          c*0 + r*1    r        l|c|r            l*0 + c*1    c
       | |_    eta = --------- = ---       _| |       eta = --------- = ---
      _| | |           c + r     c+r      | | |_              l + c     l+c
   __|_|_|_|__                          __|_|_|_|__
        0 1                                0 1

    eta is 1 when the particle is near to the right strip,
    while is approx 0 when is near to the left strip
  */
  int cstrip = GetSeedIndex();
  int nleft  = GetLeftLength();
  int nright = GetRightLength();
  // 1 strip
  if ( (nleft<=0)&&(nright<=0) ) return -1.; 
  // 2 strips
  if (nleft<=0)  return GetSignal(cstrip+1,options)/(GetSignal(cstrip+1,options) + GetSignal(cstrip,options));
  if (nright<=0) return GetSignal(cstrip  ,options)/(GetSignal(cstrip-1,options) + GetSignal(cstrip,options));
  // >2 strips
  if ( GetSignal(cstrip+1)>=GetSignal(cstrip-1) ) 
    return GetSignal(cstrip+1,options)/(GetSignal(cstrip+1,options) + GetSignal(cstrip,options));
  else
    return GetSignal(cstrip  ,options)/(GetSignal(cstrip-1,options) + GetSignal(cstrip,options));
  return -1.;
}


//--------------------------------------------------
int AMSTrCluster::GetEtaIndex(const char* options) {
  float eta = GetEta(options);
  if (eta<0.) return -1;
  if      ( (fabs(eta-0.5)>=0.35)&&(fabs(eta-0.5)<=0.50) ) return  0; // readout
  else if ( (fabs(eta-0.5)>=0.15)&&(fabs(eta-0.5)< 0.35) ) return  1; // middle
  else if ( (fabs(eta-0.5)>=0.00)&&(fabs(eta-0.5)< 0.15) ) return  2; // central
  return -1;
}


//--------------------------------------------------
float AMSTrCluster::GetCofG(int nstrips, const char* options) {
  
  if (nstrips<=1) return -1.;
  /*
    CofG(n) = center of gravity with the n consecutive higher strips 
             _        
            | |_                - 2*s(-2) - 1*s(-1) + 0*s(0) + 1*s(1)
         _ _| | |     CofG(4) = -------------------------------------
       _| | | | |_                  s(-2) + s(-1) + s(0) + s(1)
    __|_|_|_|_|_|_|__                   
             0                         
 
    in order to be coherent with the given eta definition 
    CofG(2) = Eta ==> if CofG<0 then CofG = CofG + 1.
  */
  int cstrip = GetSeedIndex();
  int nleft  = GetLeftLength();
  int nright = GetRightLength();
  int nside  = nstrips/2;
  int lindex = cstrip - nside;
  int rindex = cstrip + nside;

  if ( (nleft<=0)&&(nright<=0) ) return -1.; // 1 strip

  // nstrip is even && if the two (nstrip/2)-th strips exist 
  if ( (nstrips%2==0)&&(nleft>=nside)&&(nright>=nside) ) { 
    if ( GetSignal(lindex,options)>=GetSignal(rindex,options) ) rindex = rindex - 1;
    else                                                        lindex = lindex + 1;
  }

  // the calculation must be in the cluster
  lindex = max(lindex, cstrip - nleft);  
  rindex = min(rindex, cstrip + nright);  

  // calculation
  float numerator   = 0.;
  float denominator = 0.;
  for (int index=lindex; index<=rindex; index++) {
    float weight   = GetSignal(index,options);
    float position = (float) (index - cstrip);
    numerator   += weight*position;
    denominator += weight;
  }
  float CofG = numerator/denominator;
  if (CofG<0.) CofG += 1.;

  return CofG;
}

/* GetXCofG without TkCoo
//--------------------------------------------------
float AMSTrCluster::GetXCofG(int nstrips, int imult, char* options) {
  if (nstrips<=1) return -1000.;
  int cstrip = GetSeedIndex();
  int nleft  = GetLeftLength();
  int nright = GetRightLength();
  int nside  = nstrips/2;
  int lindex = cstrip - nside;
  int rindex = cstrip + nside;
  if ( (nleft<=0)&&(nright<=0) ) return -1000.; 
  // nstrip is even && if the two (nstrip/2)-th strips exist 
  if ( ((nstrips%2)==0)&&(nleft>=nside)&&(nright>=nside) ) { 
    if ( GetSignal(lindex,options)>=GetSignal(rindex,options) ) rindex = rindex - 1;
    else                                                        lindex = lindex + 1;
  }
  // the calculation must be in the cluster
  lindex = max(lindex, cstrip - nleft);  
  rindex = min(rindex, cstrip + nright);  
  // calculation
  float numerator   = 0.;
  float denominator = 0.;
  for (int index=lindex; index<=rindex; index++) {
    float weight   = GetSignal(index,options);
    float position = GetX(index,imult);
    numerator   += weight*position;
    denominator += weight;
  }
  float CofG = numerator/denominator;
  if (CofG<0.) CofG += 1.;
  return CofG;
}
*/

//--------------------------------------------------
float AMSTrCluster::GetX(float interpos, int imult) {
  if (interpos<0. || interpos>1.) return -1000.;
  // from the interstrip convention to coordinate
  if (interpos>0.5) interpos = - (1. - interpos);
  // position in strip units
  float position = (float) interpos + (float) GetSeedAddress();
  return TkCoo::GetLocalCoo(GetTkId(),position,imult);
}


//--------------------------------------------------
float AMSTrCluster::GetXEta(int imult, const char* options) {
  if      (GetNelem()>1)  return GetX(GetEta(options),imult);
  else if (GetNelem()==1) return GetX((float)0.,imult);
  else                    return -1000.;
  return -1000.;
} 	


//--------------------------------------------------
float AMSTrCluster::GetXCofG(int nstrips, int imult, const char* options) { 
  if      (GetNelem()>1)  return GetX(GetCofG(nstrips,options),imult);
  else if (GetNelem()==1) return GetX((float)0.,imult);
  else                    return -1000.;
  return -1000.;
}	
