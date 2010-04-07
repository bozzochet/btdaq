//  $Id: TrMCCluster.C,v 1.2 2008/04/02 11:37:13 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.C
///\brief Source file of TrMCCluster class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date: 2008/04/02 11:37:13 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrMCCluster.h"
#include "TkDBc.h"
#include "TkCoo.h"
#include "TkSens.h"

#include <cmath>


// TRMCFFKEY
float TrMCCluster::alpha = 250;
float TrMCCluster::beta  = 0.46;
// inverse linear density of primary electrons
float TrMCCluster::fastswitch = 5.e-5;
float TrMCCluster::dedx2nprel = 0.33e6;
float TrMCCluster::delta [2] = { 0.9, 1.0 }; //{ -0.67, 1.0 };
float TrMCCluster::gammaA[2] = { 0.2, 0.1 }; //{ -0.3,  0.1 };

int TrMCCluster::_NoiseMarker(555);

std::vector<TrMCCluster*> TrMCCluster::_cont;

void TrMCCluster::addcontent(char xy, float **p, int noise)
{
  if(noise && _itra!=_NoiseMarker) return;

  int side = (xy == 'x') ? 0 : 1;

  int tkid   = _idsoft%10000-1000;
  int sensor = _idsoft/10000;
  int addr   = tkid+1000+2000*side;
  int layer  = abs(tkid)/100;
  int nchann = (side  == 1) ? TkDBc::Head->_NReadoutChanS
             : (layer == 1 || 
		layer == 8) ? TkDBc::Head->_NReadStripK7 
                            : TkDBc::Head->_NReadStripK5;
  int naddrs = (side  == 0) ? TkDBc::Head->_NReadoutChanK 
                            : TkDBc::Head->_NReadoutChanS;


  for (int i = _left[side]; i <= _right[side]; i++) {
    int strip = (side == 1) ? i : ((sensor-1)*nchann+i)%naddrs;

    if (p[addr] == 0) {
      int isize = naddrs;

      p[addr] = new float[isize];
      if (p[addr] == 0) {
	std::cerr << " AMSTrMCCLUSTER-S-Memory Alloc Failure" << std::endl;
	return;
      }
      for (int j = 0; j < isize; j++) p[addr][j] = 0;
    }

    float *ptr = p[addr]+strip;
    if (_itra != _NoiseMarker) *ptr += _ss[side][i-_left[side]];
    else if (noise)            *ptr  = _ss[side][i-_left[side]];
  }
}

extern "C" double rnormx();


void TrMCCluster::_shower()
{
  //   Non active silicon as float volume introduced ; skip some lines below
  //
  //  // As we have no non-active silicon as float volume 
  //  // we should do something manually in case we are in it;
  //  // Very crude method (but fast)
  //  // gives some overestimation of dead space by ~100-200 mkm == ~10-20%
  //     double sa[2],sb[2];
  //     if((_xca[0]<0 || _xca[0]>id.getmaxsize(0)) ||
  //        (_xca[1]<0 || _xca[1]>id.getmaxsize(1)) ||
  //        (_xcb[0]<0 || _xcb[0]>id.getmaxsize(0)) ||
  //        (_xcb[1]<0 || _xcb[1]>id.getmaxsize(1))   )_sum=0; 
  //
  //  // End active check
  //  //

  AMSPoint entry  = (_xca+_xcb)/2.;
  AMSPoint dentry = (_xcb-_xca)/2;
  AMSDir   dir    =  _xcb-_xca;

  for (int i = 0; i < 5; i++) _ss[0][i] = _ss[1][i] = 0;

  int tkid  = _idsoft%10000-1000;
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

  float xpr = _sum*dedx2nprel;
  if (xpr <= 0) return;

  for (int k = 0; k < 2; k++) {

    // fast algo
//    if(fabs(dentry[k])/(xpr)<fastswitch){
    if(1){

      double s = strip2x(tkid, k, _center[k], mult);
      double e = entry[k]-s;
      for (int i =_left[k]; i <= _right[k]; i++){
	double a1 = strip2x(tkid, k, i, mult)-s;
	double a2 = a1+strip2x(tkid, k, i+1, mult)-strip2x(tkid, k, i, mult);
	_ss[k][i-_left[k]] = delta[k]*xpr*fint2(a1, a2, e, dentry[k], a2-a1);
      }
    }

    //slow algo
//    else std::cerr << "Error: slow algo is not implemented" << std::endl;

    for (int i = _left[k]; i <= _right[k]; i++)
      _ss[k][i-_left[k]] *= 1+rnormx()*gammaA[k];
  }
}

void TrMCCluster::_printEl(std::ostream & stream)
{
  int layer = std::abs(_idsoft%10000-1000)/100;
  stream << "TrMCCluster-Shower x "<<_itra<<" "<<layer<<" "
	 <<_ss[0][0]<<" "<<_ss[0][1]<<" "<<_ss[0][2]<<" "
	 <<_ss[0][3]<<" "<<_ss[0][4]<< std::endl;
  stream << "TrMCCluster-Shower y "<<_itra<<" "<<layer<<" "
	 <<_ss[1][0]<<" "<<_ss[1][1]<<" "<<_ss[1][2]<<" "
	 <<_ss[1][3]<<" "<<_ss[1][4]<< std::endl;
  stream << "TrMCCluster-Coo "<<_itra<<" "<<layer<<" "
	 <<_xca<<" "<<_xcb<<" "<<_xgl<< std::endl;
}

void TrMCCluster::sitkhits(int idsoft, float vect[],
			   float edep, float step, int itra)
{
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;

  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,
		TkDBc::Head->_ssize_active[1]/2,
		TkDBc::Head->_silicon_z/2);

  int tkid   = idsoft%10000-1000;
  int sensor = idsoft/10000-1;

  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa);
  TkSens tksb(tkid, ppb);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];

  // Range check
  for (int i = 0; i < 3; i++) {
    if (pa[i] < 0) pa[i] = 0;
    if (pb[i] < 0) pb[i] = 0;
    if (pa[i] > 2*size[i]) pa[i] = 2*size[i];
    if (pb[i] > 2*size[i]) pb[i] = 2*size[i];
  }

  // Create a new object
  _cont.push_back(new TrMCCluster(idsoft, pa, pb, pgl, edep, itra));
}

float TrMCCluster::strip2x(int tkid, int side, int strip, int mult)
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

double TrMCCluster::fints(double a, double b)
{
  if      (a >  0 && b >  0) return fdiff(a, 0)-fdiff(b, 0);
  else if (a <= 0 && b <= 0) return fdiff(b, 0)-fdiff(a, 0);
  return 2*fdiff(0, 0)-fdiff(a, 0)-fdiff(b, 0);
}

double TrMCCluster::fint2(double a, double b, 
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

double TrMCCluster::fintc(double a, double b, double c, double d)
{
  return 2*fdiff(0,0)-(fdiff(b-d,1)-fdiff(b-c,1))/(d-c)
                     +(fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCCluster::fintr(double a, double b, double c, double d)
{
  return (-fdiff(b-d,1)+fdiff(b-c,1)
	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCCluster::fintl(double a, double b, double c, double d)
{
  return (-fdiff(b-d,1)+fdiff(b-c,1)
	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCCluster::fdiff(double a, int ialpha)
{
  double xl = std::abs(a)*alpha;
  if (xl > 70) return 0;

  double diff = beta*std::exp(-xl);
  if (ialpha) diff /= alpha;

  return diff;
}
