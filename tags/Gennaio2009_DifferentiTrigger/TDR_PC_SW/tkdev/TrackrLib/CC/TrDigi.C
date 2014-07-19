//  $Id: TrDigi.C,v 1.1 2008/07/30 16:58:29 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrDigi.C
///\brief Source file of TrDigi class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc
///$Date: 2008/07/30 16:58:29 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrDigi.h"
#include "TrMCCluster.h"
#include "TrGeom.h"
#include "TrCalDB.h"

#include <cmath>

int   TrDigi::adcoverflow = 32700;
float TrDigi::thr1R[2] = { 3, 3.5 };//{ -2.75, 3.5 };
float TrDigi::thr2R[2] = { 1, 1 };
int   TrDigi::neib [2] = { 0, 1 };
int   TrDigi::CalcCmnNoise[2] = { 1, 0 };

int TrDigi::VAchannels = 64;

const int TrDigi::MATCHED=1;

float TrDigi::CmnNoise[10][4000];

std::vector<TrDigi *> TrDigi::_cont;

TrDigi::~TrDigi(){
  delete [] _array;
}

TrDigi::TrDigi(int ad, int left, int right, 
	       float *p, float s2n)
  :_address(ad),_strip(left),_nelem(right-left+1),_s2n(s2n){
    _array=new int[_nelem];
    for(int k=0;k<_nelem;k++)_array[k]=int(*(p+k));  
}

void TrDigi::_printEl(ostream & stream)
{
  stream <<_address<<" "<<_strip<<" "<<_nelem<<endl;
}

void TrDigi::matchKS()
{
  // Get rid of K without S 
  for (int i = 0; i < TrDigi::GetN(); i++) {
    TrDigi *pk=TrDigi::At(i);

    int ktkid = pk->_address%2000-1000;
    int kside = pk->_address/2000;
    if (kside != 0) continue;

    for (int j = i; j < TrDigi::GetN(); j++) {
      TrDigi *ps=TrDigi::At(j);

      int stkid = ps->_address%2000-1000;
      int sside = ps->_address/2000;

      if(sside == 1 && stkid == ktkid) {
	pk->setstatus(TrDigi::MATCHED);
	break;
      }
    }

  }
}

extern "C" double rnormx();

void TrDigi::sitkdigi()
{
  // set defaults
  for (int i = 0; i < TrMCCluster::GetN(); i++) {
    TrMCCluster *ptrhit = TrMCCluster::At(i);
    if(ptrhit->IsNoise())ptrhit->setstatus(TrMCCluster::AwayTOF);
  }

  const int ms=4000;
  const int maxva=64;
  static float* ida[ms];
  float idlocal[maxva];

  for (int i = 0; i < ms; i++) ida[i] = 0;

  for (int i = 0; i < TrMCCluster::GetN(); i++) {
    TrMCCluster *ptr = TrMCCluster::At(i);
    ptr->addcontent('x',ida);
    ptr->addcontent('y',ida);
  }

  //
  // add sigmas & calculate properly cmnnoise
  //

  for (int i = 0; i < ms; i++) {
    if(!ida[i]) continue;

    int tkid  = i%2000-1000;
    int side  = i/2000;
    int offs  = (side == 0) ? TkDBc::Head->_NReadoutChanS : 0;
    int layer = std::abs(tkid)/100;

    TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
    if (!tcal) {
      continue;
      std::cerr << "ERROR(1) tcal not found: " << tkid << std::endl;
    }

    int ndrp = (side == 0) ? TkDBc::Head->_NReadoutChanK 
                           : TkDBc::Head->_NReadoutChanS;

    for (int j = 0; j < ndrp; j++) {
      if(*(ida[i]+j)){
	int  vanum = j/VAchannels;
	int  strpa = j%VAchannels;
	int  vamin = j-strpa;
	int  vamax = j+maxva-strpa;
	float ecmn = CmnNoise[vanum][i]*rnormx();

	for (int l = vamin; l < vamax; l++) {
	  *(ida[i]+l) += tcal->Sigma(l+offs)*rnormx()+ecmn;
	  idlocal[l-vamin] = *(ida[i]+l);
	}

	float cmn = 0;
	for (int l = CalcCmnNoise[1]; l < maxva-CalcCmnNoise[1]; l++)
	  cmn += idlocal[l];
	cmn = cmn/(maxva-2*CalcCmnNoise[1]);

	for(int l = vamin; l < vamax; l++)
	  *(ida[i]+l) = *(ida[i]+l)-cmn;
	j = vamax;
      }
    }
  }

  // Add noise now
  for (int i = 0; i < TrMCCluster::GetN(); i++) {
    TrMCCluster *ptr = TrMCCluster::At(i);
    ptr->addcontent('x',ida,1);
    ptr->addcontent('y',ida,1);
  }

  for (int i = 0; i < ms; i++) {
    if (!ida[i]) continue;

    int tkid  = i%2000-1000;
    int side  = i/2000;
    int layer = std::abs(tkid)/100;
    int offs  = (side == 0) ? TkDBc::Head->_NReadoutChanS : 0;
    int ndrp  = (side == 0) ? TkDBc::Head->_NReadoutChanK 
                            : TkDBc::Head->_NReadoutChanS;

    TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
    if (!tcal) {
      continue;
      std::cerr << "ERROR(2) tcal not found: " << tkid << std::endl;
    }

    TrDigi *pcl = 0;
    int nlmin;
    int nleft=0;
    int nright=0;

    for (int j = 0; j < ndrp; j++) {
      float s2n=0;
      if(tcal->Sigma(j+offs)>0 && 
	 *(ida[i]+j)> thr1R[side]*tcal->Sigma(j+offs)){
	s2n= *(ida[i]+j)/tcal->Sigma(j+offs);
	nlmin = nright==0?0:nright+1;

	nleft = std::max(j-neib[side],nlmin); 
	for (int nlf = nleft; nlf > nlmin; --nlf) {
	  if(*(ida[i]+nlf)<= thr2R[side]*tcal->Sigma(nlf+offs))break;
	  else nleft=nlf;
	}

	nright = std::min(j+neib[side],ndrp-1);
	for (int nrt = nright; nrt < ndrp-1; ++nrt) {
	  if(*(ida[i]+nrt)<= thr2R[side]*tcal->Sigma(nrt+offs))break;
	  else nright=nrt;
	}

	double sum = 0;
	for (int k = nleft; k <= nright; k++) sum += *(ida[i]+k);

	for (int k = nleft; k <= nright; k++) {
	  int vanum = k/VAchannels;
	  (*(ida[i]+k))*=8;
	  double addon=(tcal->Pedestal(k+offs)+
			CmnNoise[vanum][i]+sum/maxva)*8;
	  if(*(ida[i]+k) +addon> adcoverflow){
	    *(ida[i]+k)=adcoverflow-addon;
	  }
	  else if(*(ida[i]+k) +addon<0){
	    *(ida[i]+k)=0-addon;
	  }            
	  if(*(ida[i]+k)>32767)*(ida[i]+k)=32767;
	  if(*(ida[i]+k)<-32767)*(ida[i]+k)=-32767;
	}
	pcl= new TrDigi(i,nleft,nright,ida[i]+nleft,s2n);
	_cont.push_back(pcl);
	j=nright+1;           
      }
    }
    delete [] ida[i];
    ida[i] = 0;
  }

  matchKS();
}

#include <fstream>

int TrDigi::ReadCmnNoise(const char *fname)
{
  std::ifstream fin(fname);
  if (!fin) return -1;

  fin.read((char*)CmnNoise, 4*10*4000);
  std::cout <<"AMSTimeID::read-I-Open file " << fname << std::endl;
  return 0;
}
