/// $Id: TrCluster.C,v 1.31 2009/01/12 15:21:09 oliva Exp $ 

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
///\date  2008/12/11 AO  Some method update
///
/// $Date: 2009/01/12 15:21:09 $
///
/// $Revision: 1.31 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkCoo.h"
#include "TrCluster.h"

ClassImp(AMSTrCluster);

TrCalDB* AMSTrCluster::_trcaldb = NULL;
TrParDB* AMSTrCluster::_trpardb = NULL;
string   AMSTrCluster::sout;
int      AMSTrCluster::DefaultCorrOpt = (AMSTrCluster::kAsym|AMSTrCluster::kAngle);
int      AMSTrCluster::DefaultUsedStrips = 3;

AMSTrCluster::AMSTrCluster(void) {
  Clear();
}

AMSTrCluster::AMSTrCluster(const AMSTrCluster &orig) :  AMSlink(orig) {
  _tkid    = orig._tkid;
  _address = orig._address;
  _nelem   = orig._nelem;
  _seedind = orig._seedind;
  for (int i = 0; i<_nelem; i++) _signal.push_back(orig._signal.at(i));
  _status  = orig._status;
  _mult    = orig._mult;
  _dxdz    = orig._dxdz;
  _dydz    = orig._dydz;
  _coord   = orig._coord;
  _gcoord  = orig._gcoord;
}

AMSTrCluster::AMSTrCluster(int tkid, int side, int add, int seedind, unsigned int status) : AMSlink() {
  Clear();
  _tkid    = tkid;
  _address = add;
  _seedind = seedind;
  _status  = status;
}

AMSTrCluster::AMSTrCluster(int tkid, int side, int add, int nelem, int seedind, 
			   float* adc, unsigned int status) :  AMSlink() {
  Clear();
  _tkid    =  tkid;
  _address =  add;
  _nelem   =  nelem;
  _seedind =  seedind;
  _signal.reserve(_nelem);
  for (int i = 0; i<_nelem; i++) _signal.push_back(adc[i]);
  _status  = status;
  _mult    = 0;
  // BuilCoordinates();
}

AMSTrCluster::~AMSTrCluster() {
  Clear();
}

int AMSTrCluster::GetMultiplicity()  {
  if (_mult==0) _mult = TkCoo::GetMaxMult(GetTkId(),GetAddress())+1;
  return _mult;
}

float AMSTrCluster::GetCoord(int imult)  {
  if (_coord.empty()) this->BuildCoordinates();
  return _coord.at(imult);
}

float AMSTrCluster::GetGCoord(int imult)  {
  if (_gcoord.empty()) this->BuildCoordinates();
  return _gcoord.at(imult);
}

float AMSTrCluster::GetNoise(int ii) {
  if (_trcaldb==0) {
    printf("AMSTrClusters::GetStatus Error, no _trcaldb specified.\n");
    return -9999.; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) { printf ("AMSTrCluster::GetNoise, WARNING calibration not found!!\n"); return -9999; }
  int address = _address+ii;
  return (float) ladcal->GetSigma(address);
}

short AMSTrCluster::GetStatus(int ii) {
  if (_trcaldb==0) {
    printf("AMSTrClusters::GetStatus Error, no _trcaldb specified.\n");
    return -1; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("AMSTrCluster::GetNoise, WARNING calibration not found!!\n"); return -9999;}
  int address = _address+ii;
  return (short) ladcal->GetStatus(address);
}

void AMSTrCluster::Clear() {
  _tkid    =   0;
  _address =  -1;
  _nelem   =   0;
  _seedind =   0;
  _signal.clear();
  _status  =   0;
  _mult    =   0;
  _dxdz    = 100;
  _dydz    = 100;
  _coord.clear();
  _gcoord.clear();
}

void AMSTrCluster::push_back(float adc) {
  _signal.push_back(adc);
  _nelem = (int) _signal.size();
}

void AMSTrCluster::BuildCoordinates() {
  if (_mult > 0 && (int)_coord.size() == _mult && (int)_gcoord.size() == _mult) return;
  // multiplicity calculation
  _mult    = TkCoo::GetMaxMult(GetTkId(),GetAddress())+1;
  for (int imult=0; imult<_mult; imult++) {
    float lcoo = GetXCofG(3,imult);
    _coord.push_back(lcoo);
    if (GetSide() == 0) _gcoord.push_back(TkCoo::GetGlobalA(_tkid, lcoo, TkDBc::Head->_ssize_active[1]/2).x());
    else                _gcoord.push_back(TkCoo::GetGlobalA(_tkid, TkDBc::Head->_ssize_active[0]/2, lcoo).y());
  }
}

void AMSTrCluster::Print(int opt) { 
  Info(opt);
  cout << sout;
}

void AMSTrCluster::Info(int opt) { 
  char msg[1000];
  sout.clear();
  sprintf(msg,"TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d  Status: %d\n",
	  GetTkId(),GetSide(),GetAddress(),GetNelem(),GetStatus() );
  sout.append(msg);
  char isseed[10];
  for (int ii=0; ii<GetNelem(); ii++) {
    sprintf(isseed," ");
    if (ii==GetSeedIndex()) sprintf(isseed,"<<< SEED");
    sprintf(msg,"Address: %4d  Signal: %10.5f  Sigma: %10.5f  S/N: %10.5f  Status: %3d  %10s\n",
	   GetAddress(ii),GetSignal(ii,opt),GetSigma(ii),GetSN(ii,opt),GetStatus(ii),isseed);
    sout.append(msg);
  }
}

std::ostream &AMSTrCluster::putout(std::ostream &ostr) const {
  return ostr << "TkID:   " << GetTkId()    << " "
	      << "Side:   " << GetSide()    << " "
	      << "Addr:   " << GetAddress() << " "
	      << "Nelm:   " << GetNelem()   << " " 
	      << "Status: " << GetStatus()  << std::endl;
}

float AMSTrCluster::GetSignal(int ii, int opt) {
  float signal = _signal.at(ii);
  if ( (kAsym&opt)&&(ii>0) ) signal = signal - _signal.at(ii-1)*GetTrParDB()->GetAsymmetry(GetSide());
  return signal;
}

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

int AMSTrCluster::GetSeedIndex(int opt) {
  if (opt==0x00) return _seedind;  
  float maxadc  = -9999.;
  int   seedind = -1;
  for (int ii=0; ii<GetNelem(); ii++) { 
    if (GetSignal(ii,opt)>maxadc) { 
      maxadc  = GetSignal(ii,opt); 
      seedind = ii; 
    } 
  }
  return seedind;
}

float AMSTrCluster::GetTotSignal(int opt) {
  float sum = 0.;
  for (int ii=0; ii<GetNelem(); ii++) sum += GetSignal(ii,opt);
  if (kVAGain&opt) for (int ii=0; ii<GetNelem(); ii++) sum += GetSignal(ii,opt)*GetTrParDB()->FindPar_TkId(GetTkId())->GetVAGain(int(ii/64));
  if (kLoss&opt)   sum = sum;
  if (kAngle&opt)  sum = sum*(1./(1.+_dxdz*_dxdz+_dydz*_dydz));
  if (kGain&opt)   sum = sum*GetTrParDB()->FindPar_TkId(GetTkId())->GetGain(GetSide()); 
  return sum;
}

void AMSTrCluster::GetBounds(int &leftindex, int &rightindex,int nstrips, int opt) {
  // loop on strips (adding strips the greatest near strip)
  int cstrip     = GetSeedIndex(opt);
  int nleft      = GetLeftLength(opt);
  int nright     = GetRightLength(opt);
  int nleftused  = 0; 
  int nrightused = 0; 
  for (int ii=0; ii<nstrips-1; ii++) {
    if      ( (nleft==nleftused)&&(nright==nrightused) ) break;  
    else if (nleft  == nleftused)  nrightused++; 
    else if (nright == nrightused) nleftused++; 
    else    ( GetSignal(cstrip-nleftused-1,opt)>GetSignal(cstrip+nrightused+1,opt) ) ? nleftused++ : nrightused++; 
  }
  leftindex  = cstrip - nleftused;
  rightindex = cstrip + nrightused;
}

float AMSTrCluster::GetCofG(int nstrips, int opt) {
  if (nstrips==1) return 0.;
  int leftindex; 
  int rightindex;
  float numerator   = 0.;
  float denominator = 0.;  
  GetBounds(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  for (int index=leftindex; index<=rightindex; index++) {
    float weight = GetSignal(index,opt);
    numerator   += weight*(index-cstrip);
    denominator += weight;
  }
  float CofG = numerator/denominator;
  return CofG;
}

float AMSTrCluster::GetDHT(int nstrips, int opt) {
  if (nstrips==1) return 0.;
  int leftindex; 
  int rightindex;
  GetBounds(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  return (leftindex + rightindex)/2. - cstrip;
}

float AMSTrCluster::GetAHT(int nstrips, int opt) {
  if (nstrips==1) return 0.;
  if (nstrips==2) return GetDHT(2,opt);
  int leftindex; 
  int rightindex;
  GetBounds(leftindex,rightindex,nstrips,opt);
  if (fabs(rightindex-leftindex+1)<3) return GetDHT(nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  float mean = 0.;
  for (int index=leftindex+1; index<=rightindex-1; index++) mean += GetSignal(index,opt);
  mean /= (leftindex+rightindex+1-2);
  return GetDHT(nstrips,opt) + (GetSignal(rightindex,opt) - GetSignal(leftindex,opt))/mean/2.;
}
