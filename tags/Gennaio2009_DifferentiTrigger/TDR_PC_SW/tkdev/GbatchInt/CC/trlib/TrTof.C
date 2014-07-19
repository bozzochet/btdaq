//  $Id: TrTof.C,v 1.3 2008/11/14 10:06:44 haino Exp $

#include "TrTof.h"

#include <cmath>
#include <iostream>
#include <fstream>

int TrTof::verbose = 0;

double TrTof::tunit   = 0.0244141;
double TrTof::tbas[2] = { 9, -13 };

double TrTof::exvert[16*4];
double TrTof::tofpxu[9];
double TrTof::tofpyu[9];
double TrTof::tofpzu[9];
double TrTof::tofpxl[9];
double TrTof::tofpyl[9];
double TrTof::tofpzl[9];

ClassImp(TrTof)

TrTof::TrTof(void)
{
}

TrTof::~TrTof(void)
{
}

int TrTof::brtype(int ilay, int ibar)
{
  int cnum = 0;
  for (int i = 0; i < ilay; i++) cnum += _bperpl[i];
  cnum += ibar;
  return _brtype[cnum];
}

double TrTof::gettsc(int il, int ib)
{
  double dxi  = _plnstr[4];
  double dxo  = _outcp[il][0];
  double dxti = (_bperpl[il]-3)*(dxi-_plnstr[3]);
  double dxto = dxti+dxo+dxi-2*_plnstr[3];
  double x;
  if      (ib==0)               x=-0.5*dxto;
  else if (ib==(_bperpl[il]-1)) x= 0.5*dxto;
  else x=-0.5*dxti+(ib-1)*(dxi-_plnstr[3]);
  if (_plrotm[il]==0) x += (il<2) ? _supstr[2] : _supstr[4];
  if (_plrotm[il]==1) x += (il<2) ? _supstr[3] : _supstr[5];
  return x;
}

double TrTof::getzsc(int il, int ib)
{
  double zc=0, dz=_plnstr[5];
  if      (il==0) zc=_supstr[0]-_plnstr[0]-dz/2.;
  else if (il==1) zc=_supstr[0]-_plnstr[1]-dz/2.;
  else if (il==2) zc=_supstr[1]+_plnstr[1]+dz/2.;
  else if (il==3) zc=_supstr[1]+_plnstr[0]+dz/2.;
  if      (il==0) zc=zc-(ib%2)*_plnstr[2];
  else if (il==3) zc=zc+(ib%2)*_plnstr[2];
  else if (il==1) zc=zc-((ib+1)%2)*_plnstr[2];
  else if (il==2) zc=zc+((ib+1)%2)*_plnstr[2];
  return zc;
}

AMSPoint TrTof::getpos(int ip, int ib)
{
  double x0 = (ip < 2) ? _supstr[3-1] : _supstr[5-1];
  double y0 = (ip < 2) ? _supstr[4-1] : _supstr[6-1];
  return AMSPoint((_plrotm[ip] == 0) ? gettsc(ip, ib) : x0,
		  (_plrotm[ip] == 1) ? gettsc(ip, ib) : y0,
		  getzsc(ip, ib));
}

AMSPoint TrTof::getsize(int ip, int ib)
{
  int    nbm = _bperpl[ip];
  double dxi = _plnstr[5-1];
  double dxo = _outcp[ip][0];
  double len = _brlen[brtype(ip, ib)-1];
  double wid = (ib == 0 || ib == nbm-1) ? dxo : dxi;
  return AMSPoint(wid/2, len/2, _plnstr[6-1]/2);
}

double TrTof::getwid(int ip, int ib, double y, int ss)
{
  double dxi = _plnstr[5-1];
  double dxo = _outcp[ip][0];

  y = abs(y);
  if (0 < ib && ib < _bperpl[ip]-1) return dxi/2;
  if (y < exvert[16*ip+1]) return dxo/2;
  if ((ib == 0 && ss > 0) || 
      (ib == _bperpl[ip]-1 && ss < 0)) return dxo/2;

  double y1 = exvert[16*ip+1], y2 = exvert[16*ip+3];
  double x1 = dxo/2, x2 = dxo/2-_outcp[ip][1];
  return x1+(x2-x1)/(y2-y1)*(y-y1);
}

int TrTof::tofdbc(const char *fname)
{
  std::ifstream fin(fname);
  if (!fin) {
    std::cout << "TOF DBc file not found: " << fname << std::endl;
    return -1;
  }

  fin >> _planes;
  for (int i = 0; i < _planes; i++) fin >> _bperpl[i];
  int icm = 0;
  for (int i = 0; i < _planes; i++) icm += _bperpl[i];
  for (int i = 0; i < icm;     i++) fin >> _brtype[i];
  for (int i = 0; i < _planes; i++) fin >> _plrotm[i];
  for (int i = 0; i < SCBTPN;  i++) fin >> _brlen[i];
  for (int i = 0; i < SCBTPN;  i++) fin >> _lglen[i];
  for (int i = 0; i < 4;       i++) 
  for (int j = 0; j < _planes; j++) fin >> _outcp[j][i];
  for (int i = 0; i < 12;      i++) fin >> _supstr[i];
  for (int i = 0; i < 20;      i++) fin >> _plnstr[i];
  fin >> _nsespar;
  for (int i = 0; i < _nsespar; i++)
  for (int j = 0; j < SCBTPN;   j++) fin >> _sespar[j][i];

  if (fin.good()) {
    std::cout << "TofDBc data read from: " << fname << std::endl;
    return 0;
  }

  std::cout << "Error found in: " << fname << std::endl;
  return 2;
}

void TrTof::setdbpars(void)
{
  double ZShift = 0;
  _supstr[0] += ZShift;
  _supstr[1] -= ZShift;

  for (int ip = 0; ip < 4; ip++) {
    double len = _brlen[brtype(ip, 0)-1];
    double wid = _outcp[ip][0];
    double xc  = _outcp[ip][1];
    double yc  = _outcp[ip][2];
    double dx  = wid/2-xc/4;
    double dy  = len/2-yc/2;
    double by  = len/2;

    exvert[16*ip+0] = exvert[16*ip+6] =  xc*0.25+dx;
    exvert[16*ip+2] = exvert[16*ip+4] = -xc*0.75+dx;
    exvert[16*ip+1] = -yc*0.5+dy; exvert[16*ip+3] =  by;
    exvert[16*ip+7] =  yc*0.5-dy; exvert[16*ip+5] = -by;
    for (int j = 0; j < 8; j++) exvert[16*ip+8+j] = exvert[16*ip+j];

    double *tofpx, *tofpy, *tofpz;
    if      (ip == 1) { tofpx = tofpxu; tofpy = tofpyu; tofpz = tofpzu; }
    else if (ip == 3) { tofpx = tofpxl; tofpy = tofpyl; tofpz = tofpzl; }
    else continue;

    int i1 = (ip == 1) ? 0 : 3;
    int i2 = (ip == 1) ? 1 : 2;
    double px = -(gettsc(i2, 1)+gettsc(i2, 2))/2, py = -gettsc(i1, 0);
    tofpx[0] = exvert[16*i1+3];      tofpx[1] = px;
    tofpy[0] = exvert[16*i1+2]+py;   tofpy[1] = _brlen[brtype(i2, 2)-1]/2; 
    tofpx[2] = tofpx[5] = -tofpx[1]; tofpy[2] = tofpy[1];
    tofpx[3] = tofpx[4] = -tofpx[0]; tofpy[3] = tofpy[8] =  tofpy[0];
    tofpx[7] = tofpx[8] =  tofpx[0]; tofpy[4] = tofpy[7] = -tofpy[0];
    tofpx[6] = tofpx[1];             tofpy[5] = tofpy[6] = -tofpy[1];
    for (int j = 0; j < 9; j++) tofpz[j] = (getzsc(i1, 0)+getzsc(i2, 0))/2;
  }    
}

int TrTof::tofcal(const char *fname)
{
  ifstream fin(fname);
  if (!fin) {
    cout << "File not found: " << fname << endl;
    return -1;
  }
  int dummy;
  for (int i = 0; i < 80; i++) {
    fin >> dummy >> dummy >> dummy >> tofped[i] >> tofgain[i] 
	>> timeoffs[i] >> timecorr[i];
  }

  if (fin.good())
    std::cout << "TofCal data read from: " << fname << std::endl;
  else
    std::cout << "Error found in: " << fname << std::endl;

  return 1;
}

void TrTof::Clear(void)
{
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      int jj = i*2+j;
      HitPattern[jj] = 0;
      for (int k = 0; k < 10; k++) {
	Adc [jj*10+k] = Tdc[jj*10+k] = 0;
	Time[jj*10+k] = Vpos[i*10+k] = 0;
      }
      FtTdc[jj*2] = FtTdc[jj*2+1] = 0;
    }
    AdcHit[i] = Nhit[i] = 0;
  }
  NplHit = 0;
}

int TrTof::Reconstruct(void)
{
  for (int ip = 0; ip < _planes; ip++) {
    int tpat1 = 0, tpat2 = 0;
    for (int ib = 0; ib < _bperpl[ip]; ib++) {
      int ip1 = ip*2*10+ib, ip2 = (ip*2+1)*10+ib;
      if (tofgain[ip1] > 0) {
	double chg = (Adc[ip1]-tofped[ip1])/tofgain[ip1];
	if (chg > 0.5) tpat1 |= (1<<ib);
      }
      if (tofgain[ip2] > 0) {
	double chg = (Adc[ip2]-tofped[ip2])/tofgain[ip2];
	if (chg > 0.5) tpat2 |= (1<<ib);
      }
    }
    AdcHit[ip] = tpat1 & tpat2;

    for (int ib = 0; ib < _bperpl[ip]; ib++) {
      if (!(AdcHit[ip] & (1<<ib))) continue;

      if (Nhit[ip]++ == 0) NplHit++;

      int ip1 = ip*2*10+ib,   ip2 = (ip*2+1)*10+ib;
      int is1 = ip*2*2 +ib%2, is2 = (ip*2+1)*2 +ib%2;
      if (Tdc[ip1] <= 0 || Tdc[ip2] <= 0) continue;

      double tm1 = (Tdc[ip1]-FtTdc[is1])*tunit-timeoffs[ip1];
      double tm2 = (Tdc[ip2]-FtTdc[is2])*tunit-timeoffs[ip2];
      if (ip == 0) { tm1 += tbas[0]; tm2 += tbas[1]; }
      else         { tm1 -= tbas[0]; tm2 -= tbas[1]; }

      double ttm1 = tm1, ttm2 = tm2;
      for (int kk = 0; kk < 3; kk++) {
	double vptmp = (ttm1-ttm2)/2/0.07;
	ttm1 = tm1-timecorr[ip1]*vptmp*0.01;;
	ttm2 = tm2-timecorr[ip2]*vptmp*0.01;
      }

      Time[ip1] = ttm1;
      Time[ip2] = ttm2;
      Vpos[ip*10+ib] = (Time[ip1]-Time[ip2])/2/0.07;
    }
  }

  return NplHit;
}

int TrTof::Decode(const int offset, const int jsize, unsigned short *bblock)
{
  unsigned short status = bblock[offset+jsize];
  int JinfNum = ((unsigned int)status)&0x1f;

  // Check JINF num
  if (JinfNum != 4 && JinfNum != 6 && JinfNum != 18 && JinfNum != 20) 
    return -1;

  int toffset = offset+1;

  // Extract and Decode ADC 
  for (int i = 0; i < 90; i++) {
    unsigned short w = bblock[toffset+i];
    int id = (w & 0xf000) >> 12;
    int qq = (w & 0x0fff);
    if (i%9 != id) {
      if (verbose)
	std::cout << "Warning in TOF-ADC-Decode: position mismatch " 
		  << i << " " << i%9 << " != " << id << ":" 
		  << hex << qq << dec << std::endl;
    }
    else if (id < 4 && i/9 < 5) {
      int iplane = 0, iside = 0;
      int ich = (i/9)*2+(id%2);
      if (JinfNum == 4 || JinfNum == 6) {
	iplane = (id/2)%2;
	iside  = (id/2)%2;
	if (JinfNum == 4) iside = 1-iside;
      }
      else if (JinfNum == 18 || JinfNum == 20) {
	iplane = 3-(id/2)%2;
	iside  = (JinfNum == 18) ? 1 : 0;
      }
      Adc[(iplane*2+iside)*10+ich] = qq;
    }
  }

  // Extract TDC
  int ntdc[5], tdc[5*100];
  for (int i = 0; i < 5; i++) ntdc[i] = 0;
  for (int i = 0; i < jsize-94-11; i+=2) {
    unsigned short w1 = bblock[toffset+94+i];
    unsigned short w2 = bblock[toffset+94+i+1];
    int ty =  (w1 & 0xf000) >> 12;
    int id =  (w1 & 0x0f00) >>  8;
    int qq = ((w1 & 0x00ff) << 16)+w2;
    if (ty != 8 && ty != 2 && ty != 4 && ty != 3) {
      if (verbose)
	std::cout << "Warning in TOF-TDC-Decode: InvalidWordType " 
		  << i << " " << ty << " " << id << " " 
		  << hex << qq << dec << std::endl;
      continue;
    }
    if (id < 0 || 5 <= id) {
      if (verbose)
	std::cout << "Warning in TOF-TDC-Decode: Invalid slot " 
		  << i << " " << ty << " " << id << " " 
		  << hex << qq << dec << std::endl;
      continue;
    }
    if (ntdc[id] == 0 && ty != 8) {
      if (verbose)
	std::cout << "Warning in TOF-TDC-Decode: InvalidWordType (0) " 
		  << i << " " << ty << " " << id << " " 
		  << hex << qq << dec << std::endl;
      continue;
    }
    if (ntdc[id] < 2 && ty == 3) {
      if (verbose)
	std::cout << "Warning in TOF-TDC-Decode: InvalidWordType (1) " 
		  << i << " " << ty << " " << id << " " 
		  << hex << qq << dec << std::endl;
      continue;
    }

    if (ty == 3) {
      int evt1 = (tdc[id+5]&0xFFF000L)>>12;
      int evt2 = (qq&0xFFF000L)>>12;
      int ntbf =  qq&0xFFF;
      if (evt1 != evt2) {
      if (verbose)
	std::cout << "Warning in TOF-TDC-Decode: inconsistent eventID " 
		  << i << " " << ty << " " << id << " " 
		  << evt1 << " != " << evt2 << std::endl;
	continue;
      }
      if (ntbf != ntdc[id]) {
	if (verbose)
	  std::cout << "Warning in TOF-TDC-Decode: inconsistent tdc num " 
		    << i << " " << ty << " " << id << " " 
		    << ntbf << " != " << ntdc[id] << std::endl;
	continue;
      }
    }

    if (ntdc[id] >= 100) {
      if (verbose)
	std::cout << "Warning in TOF-TDC-Decode: buffer overflow " 
		  << i << " " << ty << " " << id << " " 
		  << ntdc[id] << std::endl;
    }
    else
      tdc[id+ntdc[id]*5] = qq;
    ntdc[id]++;
  }

  // Decode TDC
  for (int i = 0; i < 4; i++) {
    for (int j = 2; j < ntdc[i]-1; j++) {
      int ich = ((tdc[j*5+i]&0x00E00000L)>>21)*2+i%2;
      int tch = ((tdc[j*5+i]&0x0007FFFFL)<< 2)
	        | ((tdc[j*5+i]&0x00180000L)>>19);
      if (ich >= 12) continue;

      int iplane = 0, iside = 0;
      if (JinfNum == 4 || JinfNum == 6) {
	iplane = i/2;
	iside  = i/2;
	if (JinfNum == 4) iside = 1-iside;
      }
      else if (JinfNum == 18 || JinfNum == 20) {
	iplane = 3-i/2;
	iside  = (JinfNum == 18) ? 1 : 0;
      }
      if (ich < 10)
	Tdc[(iplane*2+iside)*10+ich] = tch;
      if (ich/2 == 5)
	FtTdc[(iplane*2+iside)*2+ich-10] = tch;
    }
  }

  // Extract and Decode HitPattern
  int *tpat1 = &HitPattern[0];
  int *tpat2 = &HitPattern[3];
  if (JinfNum == 6) {
    tpat1 = &HitPattern[1];
    tpat2 = &HitPattern[2];
  }
  if (JinfNum == 18) {
    tpat1 = &HitPattern[4];
    tpat2 = &HitPattern[6];
  }
  if (JinfNum == 20) {
    tpat1 = &HitPattern[5];
    tpat2 = &HitPattern[7];
  }

  unsigned short w1 = bblock[toffset+91];
  unsigned short w2 = bblock[toffset+92];

  if (w1&1)    *tpat1 |= 1;
  if (w1>>1&1) *tpat1 |= 1<<2;
  if (w1>>2&1) *tpat1 |= 1<<4;
  if (w1>>3&1) *tpat1 |= 1<<6;
  if (w1>>5&1) *tpat1 |= 1<<1;
  if (w1>>6&1) *tpat1 |= 1<<3;
  if (w1>>7&1) *tpat1 |= 1<<5;
  if (w1>>8&1) *tpat1 |= 1<<7;
  if (w1>>4&1) *tpat1 |= 1<<8;
  if (w1>>9&1) *tpat1 |= 1<<9;

  if (w2&1)    *tpat2 |= 1;
  if (w2>>1&1) *tpat2 |= 1<<2;
  if (w2>>2&1) *tpat2 |= 1<<4;
  if (w2>>3&1) *tpat2 |= 1<<6;
  if (w2>>5&1) *tpat2 |= 1<<1;
  if (w2>>6&1) *tpat2 |= 1<<3;
  if (w2>>7&1) *tpat2 |= 1<<5;
  if (w2>>8&1) *tpat2 |= 1<<7;
  if (w2>>4&1) *tpat2 |= 1<<8;
  if (w2>>9&1) *tpat2 |= 1<<9;

  return 0;
}

//
// Default values taken from /amssw/AMSDataDir/v4.00/tof2const011207.dat
//
int TrTof::_planes  = 4;
int TrTof::_nsespar = 2;
int TrTof::_brtype[TrTof::SCBLMX] = {  1,  2,  2,  2,  2,  2,  2,  1, 
				       5,  6,  7,  7,  7,  7,  6,  5,
				   8,  9, 10, 11, 11, 11, 11, 10,  9,  8,
				       3,  4,  4,  4,  4,  4,  4,  3 }; 
//
int TrTof::_plrotm[TrTof::SCLRS] = { 1, 0,  0, 1 };
int TrTof::_bperpl[TrTof::SCLRS] = { 8, 8, 10, 8 };
//
float TrTof::_brlen [TrTof::SCBTPN] 
 = { 126.5, 130.5, 130., 134.,  117.2, 127., 132.2, 110., 117.2, 127., 132.2 };
//
float TrTof::_lglen [TrTof::SCBTPN]
 = {   9.,    6.,    9.,   6.,   16.,   15.2, 16.,   16.,  16.,   15.,  16., };
//
float TrTof::_outcp [TrTof::SCLRS][4] 
 = { { 22.5, 5,   25, 17.5 }, { 25.5, 13.5, 24.3,  12 },
     { 18.5, 6.5, 20, 12   }, { 26,    8.5, 34.65, 17.5 } };
//
float TrTof::_supstr[12]
 = { 65.59, -65.61, 0., 0., 0., 0., 129.4, 139.9, 10., 131., 132., 5. };
//
float TrTof::_plnstr[20]
 = { 0.145, 3.295, 1.55, 0.5, 12., 1.,  0., 0.34, 81.5, 0.,
     8., 100., 6., 0., 0., 8., 6., 60., 0., 70. };
//
float TrTof::_sespar[TrTof::SCBTPN][TrTof::SESPMX];


//
// Default values taken as of RUN 1213377198
//
double TrTof::tofped[80]
 = { 184.06, 228.38, 187.01, 209.61, 132.08, 161.81, 182.57, 195.88, 
     170.56, 200.69, 151.00, 241.16, 109.25, 193.18, 250.97, 211.05, 
     122.33, 172.78, 137.36, 201.07, 177.88, 246.47, 146.06, 191.41, 
     200.05, 231.42, 236.46, 211.99, 141.56, 178.02, 113.90, 177.75, 
     173.11, 162.56, 170.03, 211.73, 157.01, 192.69, 182.08, 158.79, 
     236.04, 248.27, 175.41, 244.73, 175.37, 138.75, 251.52, 147.42, 
     252.76, 138.58, 237.63, 162.20, 205.69, 201.86, 162.56, 267.57, 
     165.82, 194.45, 229.59, 215.31, 205.73, 185.93, 124.39, 208.74, 
      83.63, 163.73, 137.27, 217.58, 198.28, 169.84, 324.20, 165.84, 
     221.05, 162.98, 198.85, 116.60, 259.08, 176.27, 152.66, 171.17 };
//
double TrTof::tofgain[80]
 = { 67.28, 38.39, 41.82, 58.80, 26.33, 42.25, 45.11, 55.93,  0.00,  0.00, 
     57.46, 51.05, 54.22, 22.21, 21.73, 33.36, 40.83, 62.26,  0.00,  0.00, 
     36.55, 60.02, 64.94, 39.72, 43.15, 27.41, 41.77, 58.02,  0.00,  0.00, 
     23.64, 16.82, 23.82, 44.01, 44.66, 25.82, 47.77, 22.17,  0.00,  0.00, 
     69.78, 48.32, 57.56, 49.50, 43.06, 69.57, 43.02, 47.26, 82.18, 33.95, 
      0.00,  0.00, 42.79, 47.18, 94.84, 46.60, 49.37, 24.79,  0.00,  0.00, 
     68.47, 34.36, 29.54, 51.06, 34.42, 41.93, 81.72, 63.45,  0.00,  0.00, 
     82.25, 50.37, 35.10, 30.82, 51.08, 31.37, 47.64, 78.80,  0.00,  0.00 };
//
double TrTof::timeoffs[80]
 = { -0.326, -0.790, -1.786, -2.080, -2.755, -1.982, -2.183, -1.401, 
      0.000,  0.000, -1.400, -2.337, -1.628, -1.904, -1.051, -0.363, 
     -1.136,  1.195,  0.000,  0.000,  0.287, -0.859, -1.323, -1.465,
     -0.568, -0.779, -2.302, -4.599,  0.000,  0.000, -4.402, -4.028,
     -2.228, -1.143, -1.335, -2.728, -1.289, -2.439,  0.000,  0.000,
      0.000,  0.000, -3.523, -3.711, -2.734, -3.104, -2.310, -2.752,
      0.000,  0.000,  0.000,  0.000,  2.774,  0.648, -0.164,  1.084,
      0.432, -0.685,  0.000,  0.000, -1.088, -1.546, -1.583, -1.240,
     -0.210,  0.046,  0.599,  1.717,  0.000,  0.000,  7.832,  6.366,
      6.533,  5.632,  5.432,  4.923,  4.017,  5.959,  0.000,  0.000 };
//
double TrTof::timecorr[80]
 = { -0.206,  0.005, -0.254,  0.041, -0.017,  0.293, -0.147,  0.033,
      0.000,  0.000,  0.084,  0.211,  0.060, -0.270, -0.290, -0.165,
      0.200,  0.139,  0.000,  0.000,  0.278, -0.309, -0.246, -0.042,
     -0.163,  0.153, -0.376, -0.518,  0.000,  0.000, -0.796,  0.099,
     -0.030,  0.312, -0.135,  0.200,  0.066, -0.220,  0.000,  0.000,
      0.000,  0.000,  0.068,  0.052,  0.132, -0.179,  0.138, -0.209,
      0.000,  0.000,  0.000,  0.000, -0.075,  0.019,  0.181, -0.073,
     -0.061, -0.003,  0.000,  0.000,  0.098,  0.006,  0.305,  0.171,
      0.262,  0.184,  0.213,  0.235,  0.000,  0.000, -0.074, -0.150,
     -0.311, -0.170, -0.268, -0.294,  0.161, -0.086,  0.000,  0.000 };
