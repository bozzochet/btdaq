//  $Id: TrMCCluster.C,v 1.3 2008/09/11 16:56:02 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.C
///\brief Source file of AMSTrMCCluster class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date: 2008/09/11 16:56:02 $
///
///$Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"
#include "TkCoo.h"
#include "TkSens.h"
#include "tkdcards.h"
#include <cmath>
#include "TrMCCluster.h"
#include <cstdlib>



ClassImp(AMSTrMCCluster);

int AMSTrMCCluster::_NoiseMarker(555);


extern "C" double rnormx();

void AMSTrMCCluster::_shower()
{


  AMSPoint entry  = (_xca+_xcb)/2.;
  AMSPoint dentry = (_xcb-_xca)/2;
  AMSDir   dir    =  _xcb-_xca;

  for (int i = 0; i < 5; i++) _ss[0][i] = _ss[1][i] = 0;

  int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;

  int layer = abs(tkid)/100;
  int nchx  = (layer == 1 || layer == 8) ? TkDBc::Head->_NReadStripK7 
                                         : TkDBc::Head->_NReadStripK5;
  int nchy  = TkDBc::Head->_NReadoutChanS;

  TkSens tks(tkid, _xgl);
  int bcen0 = tks.GetStripX();
  int bcen1 = tks.GetStripY();

  int mult = 0;

  // Convert from TDR address into sensor strip
  if (layer == 1 || layer == 8) {
    int nad = TkDBc::Head->_NReadoutChanK;
    int sen = tks.GetSensor();
    int idx = ((sen+1)*nchx-bcen0)/nad;
    bcen0 = bcen0+idx*nad-sen*nchx;
  }
  else if (tks.GetSensor()%2 == 1) bcen0 -= 192;

  if (bcen0 < 0) bcen0 = 0;
  if (bcen1 < 0) bcen1 = 0;
  if (bcen0 >= nchx) bcen0 = nchx-1;
  if (bcen1 >= nchy) bcen1 = nchy-1;

  _center[0] = bcen0;
  _center[1] = bcen1;
  _left  [0] = max(0,_center[0]-(5/2));
  _left  [1] = max(0,_center[1]-(5/2));
  _right [0] = min(nchx-1, _center[0]+(5/2));
  _right [1] = min(nchy-1, _center[1]+(5/2));

  float xpr = _sum*TRMCFFKEY.dedx2nprel;
  if (xpr <= 0) return;

  for (int k = 0; k < 2; k++) {

    // fast algo
//    if(fabs(dentry[k])/(xpr)<TRMCFFKEY.fastswitch){
    if(1){

      double s = strip2x(tkid, k, _center[k], mult);
      double e = entry[k]-s;
      for (int i =_left[k]; i <= _right[k]; i++){
	double a1 = strip2x(tkid, k, i, mult)-s;
	double a2 = a1+strip2x(tkid, k, i+1, mult)-strip2x(tkid, k, i, mult);
	_ss[k][i-_left[k]] = TRMCFFKEY.delta[k]*xpr*fint2(a1, a2, e, dentry[k], a2-a1);
      }
    }

    //slow algo
//    else std::cerr << "Error: slow algo is not implemented" << std::endl;

    for (int i = _left[k]; i <= _right[k]; i++)
      _ss[k][i-_left[k]] *= 1+rnormx()*TRMCFFKEY.gammaA[k];
  }
  _left[0]+=640;  _center[0]+=640;  _right[0]+=640;
}



int AMSTrMCCluster::GetTkId(){ 
  int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  
  return tkid;
}

void AMSTrMCCluster::_printEl(std::ostream & stream)
{
  int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;

  int layer = abs(tkid)/100;
  stream << "Impinging Particle: "<<_itra<<" Px: "<<_Momentum[0]
	 <<" Py: "<<_Momentum[1]<<" Pz: "<<_Momentum[2]<<endl;
  stream << "AMSTrMCCluster-Shower x "<<_itra<<" "<<layer<<" "
         << _left[0]<< " "<<_center[0]<<" "<<_right[0]<<" "
	 <<_ss[0][0]<<" "<<_ss[0][1]<<" "<<_ss[0][2]<<" "
	 <<_ss[0][3]<<" "<<_ss[0][4]<< std::endl;
  stream << "AMSTrMCCluster-Shower y "<<_itra<<" "<<layer<<" "
         << _left[1]<< " "<<_center[1]<<" "<<_right[1]<<" "
	 <<_ss[1][0]<<" "<<_ss[1][1]<<" "<<_ss[1][2]<<" "
	 <<_ss[1][3]<<" "<<_ss[1][4]<< std::endl;
  stream << "AMSTrMCCluster-Coo "<<tkid<<" "<<_idsoft<<" "<<layer<<" "
	 <<_xca<<" "<<_xcb<<" "<<_xgl<< std::endl;
}


float AMSTrMCCluster::strip2x(int tkid, int side, int strip, int mult)
{
  int layer = abs(tkid)/100;
  int nch   = (side  == 1) ? TkDBc::Head->_NReadoutChanS
    : (layer == 1 || 
       layer == 8) ? TkDBc::Head->_NReadStripK7 
    : TkDBc::Head->_NReadStripK5;


  int ss = strip;
  if (ss <=   0) ss = 1;
  if (ss >= nch) ss = nch-1;

  ss += (side == 0) ? TkDBc::Head->_NReadoutChanS : 0;
  float ss0 = TkCoo::GetLocalCoo(tkid, ss-1, mult);
  float ss1 = TkCoo::GetLocalCoo(tkid, ss,   mult);

  if (strip <=   0) return ss0-(ss0+ss1)/2;
  if (strip >= nch) return ss1+(ss0+ss1)/2;

  return (ss0+ss1)/2;
}

double AMSTrMCCluster::fints(double a, double b)
{
  if      (a >  0 && b >  0) return fdiff(a, 0)-fdiff(b, 0);
  else if (a <= 0 && b <= 0) return fdiff(b, 0)-fdiff(a, 0);
  return 2*fdiff(0, 0)-fdiff(a, 0)-fdiff(b, 0);
}

double AMSTrMCCluster::fint2(double a, double b, 
			  double av, double dav, double dix)
{
  if (std::abs(dav)/dix <= 0.01) return fints(a-av, b-av);

  double dlmin = av-std::abs(dav);
  double dlmax = av+std::abs(dav);
  if (a <= dlmin && b >= dlmax) return fintc(a, b, dlmin, dlmax);
  if (a <= dlmin && b <= dlmin) return fintl(a, b, dlmin, dlmax);
  if (a >= dlmax && b >= dlmax) return fintr(a, b, dlmin, dlmax);

  if (a <= dlmin && b <= dlmax)
    return fintc(a, b, dlmin, b)*(b-dlmin)/(dlmax-dlmin)
          +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);

  if (a >= dlmin && b >= dlmax)
    return fintc(a, b, a, dlmax)*(dlmax-a)/(dlmax-dlmin)
          +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin);

  return fintc(a, b, a,     b)*    (b-a)/(dlmax-dlmin)
        +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin)
        +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);
}

double AMSTrMCCluster::fintc(double a, double b, double c, double d)
{
  return 2*fdiff(0,0)-(fdiff(b-d,1)-fdiff(b-c,1))/(d-c)
                     +(fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double AMSTrMCCluster::fintr(double a, double b, double c, double d)
{
  return (-fdiff(b-d,1)+fdiff(b-c,1)
	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double AMSTrMCCluster::fintl(double a, double b, double c, double d)
{
  return (-fdiff(b-d,1)+fdiff(b-c,1)
	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double AMSTrMCCluster::fdiff(double a, int ialpha)
{
  double xl = std::abs(a)*TRMCFFKEY.alpha;
  if (xl > 70) return 0;

  double diff = TRMCFFKEY.beta*std::exp(-xl);
  if (ialpha) diff /= TRMCFFKEY.alpha;

  return diff;
}
