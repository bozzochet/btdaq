// $Id: TrField.C,v 1.6 2008/11/22 09:18:06 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrField.C
///\brief Source file of TrField class
///
///\date  2007/12/12 SH  First test version
///\date  2007/12/16 SH  First import (as TkMfield)
///\date  2007/12/18 SH  First stable vertion after a refinement
///\date  2007/12/20 SH  All the parameters are defined in double
///\date  2008/01/20 SH  Imported to tkdev (test version)
///$Date: 2008/11/22 09:18:06 $
///
///$Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrField.h"

#include <iostream>
#include <fstream>
#include <cmath>

PROTOCCALLSFSUB2(VBLANK,vblank,INTV,INT)
#define VBLANK(A,I) CCALLSFSUB2(VBLANK,vblank,INTV,INT,A,I)

PROTOCCALLSFSUB4(UHTOC,uhtoc,INTV,INT,PSTRING,INT)
#define UHTOC(H,IH,S,IS) CCALLSFSUB4(UHTOC,uhtoc,INTV,INT,PSTRING,INT,H,IH,S,IS)

PROTOCCALLSFSUB4(UCTOH,uctoh,PSTRING,INTV,INT,INT)
#define UCTOH(H,IH,S,IS) CCALLSFSUB4(UCTOH,uctoh,PSTRING,INTV,INT,INT,H,IH,S,IS)


void MAGFIELDFFKEY_DEF::init(){


  iniok=1;

  char mapfilename[160]="fld97int.txt";
  UCTOH(mapfilename,mfile,4,160);
  //  memset(mfile,40,sizeof(mfile[0]));
  isec[0]=0;
  isec[1]=0;
  imin[0]=0;
  imin[1]=0;
  ihour[0]=0;
  ihour[1]=0;
  imon[0]=0;
  imon[1]=0;
  iyear[0]=0;
  iyear[1]=0;
  
  magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
  fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
  ecutge=0.001; //(3) e/g ener.cut for tracking in magnet materials(Gev) 

  BTempCorrection=0;
  BZCorr=1; 

  return;  

}

MAGFIELDFFKEY_DEF MAGFIELDFFKEY;




TrField *TrField::_ptr = 0;

TrField::TrField(void)
{
  iniok=0;
  _dx=_dy=_dz=0;
  for(int ii=0;ii<2;ii++)
    isec[ii]=imin[ii]=ihour[ii]=iday[ii]=imon[ii]=iyear[ii]=0;
  na[0]=na[1]=na[2]=0;
  for (int ix=0;ix<_nx;ix++)
    for (int iy=0;iy<_ny;iy++)
      for (int iz=0;iz<_nz;iz++){
	
	_x[ix]=0;
	_y[iy]=0;
	_z[iz]=0;
	  _bx[iz][iy][ix]=0;
	  _by[iz][iy][ix]=0;
	  _bz[iz][iy][ix]=0;
	  _xyz[ix+iy+iz]=0;
	  _bdx[0][iz][iy][ix]=0;
	  _bdy[0][iz][iy][ix]=0;
	  _bdz[0][iz][iy][ix]=0;
	  _bdx[1][iz][iy][ix]=0;
	  _bdy[1][iz][iy][ix]=0;
	  _bdz[1][iz][iy][ix]=0;
	  _bxc[iz][iy][ix]=0;
	  _byc[iz][iy][ix]=0;
	  _bzc[iz][iy][ix]=0;
	}
  _ptr = this;
}

TrField::~TrField()
{
  _ptr=0;
}

int TrField::Read(const char *fname)
{
  std::ifstream fin(fname);
  if (!fin) {
    std::cerr << "TrField::ReadAMS: File not found: " << fname << std::endl;
    return -1;
  }

//int dummy[13], size[3], nn;  // For old filed map
  int dummy[12], size[3], nn;  // For new (07) field map
  fin.read((char *)dummy, sizeof(dummy));
  fin.read((char *)size,  sizeof(size));

  if (_nx != size[0] || _ny != size[1] || _nz != size[2]) {
    std::cerr << "Error in TrField::Read Size mismatch: " 
	      << _nx << " " << size[0] << " " << _ny << " " << size[1] << " "
	      << _nz << " " << size[2] << std::endl;
    return -1;
  }

  int _nb = _nx*_ny*_nz;
  nn = _nx+_ny+_nz;

  std::cout << "TrField::ReadAMS: Reading field map... ";
  std::cout.flush();

  float data;
  

  char *ptr = (char *)&data;
  float *fptr=0;
  fptr=&(_x[0])            ;for (int i = 0; i < _nx;   i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_y[0])            ;for (int i = 0; i < _ny;   i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_z[0])            ;for (int i = 0; i < _nz;   i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_bx[0][0][0])     ;for (int i = 0; i < _nb;   i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_by[0][0][0])     ;for (int i = 0; i < _nb;   i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_bz[0][0][0])     ;for (int i = 0; i < _nb;   i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_xyz[0])          ;for (int i = 0; i <  nn;   i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_bdx[0][0][0][0]) ;for (int i = 0; i < _nb*2; i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_bdy[0][0][0][0]) ;for (int i = 0; i < _nb*2; i++) { fin.read(ptr, 4); fptr[i] = data; }
  fptr=&(_bdz[0][0][0][0]) ;for (int i = 0; i < _nb*2; i++) { fin.read(ptr, 4); fptr[i] = data; }

  std::cout << _nb << std::endl;

  _dx = _x[1]-_x[0];
  _dy = _y[1]-_y[0];
  _dz = _z[1]-_z[0];

  return _nb;
}

void TrField::GuFld(float *xx, float *b)
{
  _dx = _x[1]-_x[0];
  _dy = _y[1]-_y[0];
  _dz = _z[1]-_z[0];
  b[0] = b[1] = b[2] = 0;
  if (MAGFIELDFFKEY.magstat <= 0 ) return;


  double ax = std::fabs(xx[0]), sx = (ax > 0) ? xx[0]/ax : 1;
  double ay = std::fabs(xx[1]), sy = (ay > 0) ? xx[1]/ay : 1;
  double az = std::fabs(xx[2]), sz = (az > 0) ? xx[2]/az : 1;
  az *= MAGFIELDFFKEY.BZCorr;

  int idx[8];
  double ww[8];
  _Fint(ax, ay, az, idx, ww);

  float *mbx=&(_bx[0][0][0]);
  float *mby=&(_by[0][0][0]);
  float *mbz=&(_bz[0][0][0]);

  for (int i = 0; i < 8; i++) {
    b[0] += mbx[idx[i]]*ww[i];
    b[1] += mby[idx[i]]*ww[i];
    b[2] += mbz[idx[i]]*ww[i];
  }
//   printf ("X: %+7.3f %+7.3f %+7.3f B: %f  \n",xx[0],xx[1],xx[2],
// 	  sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]));

  b[1] *= sx*sy;
  b[2] *= sx*sz;
}

void TrField::TkFld(float *xx, float hxy[][3])
{
  hxy[0][0] = hxy[0][1] = hxy[0][2] = 
  hxy[1][0] = hxy[1][1] = hxy[1][2] = 0;
  if (MAGFIELDFFKEY.magstat <= 0 || !_bx || !_by || !_bz) return;

  double ax = std::fabs(xx[0]);
  double ay = std::fabs(xx[1]);
  double az = std::fabs(xx[2]);
  az *= MAGFIELDFFKEY.BZCorr;

  int idx[8];
  double ww[8];
  _Fint(ax, ay, az, idx, ww);

  int _nb = _nx*_ny*_nz;

  float *mbdx=&(_bdx[0][0][0][0]);
  float *mbdy=&(_bdy[0][0][0][0]);
  float *mbdz=&(_bdz[0][0][0][0]);

  for (int i = 0; i < 8; i++) {
    hxy[0][0] += mbdx[idx[i]]*ww[i]; hxy[1][0] += mbdx[idx[i]+_nb]*ww[i];
    hxy[0][1] += mbdy[idx[i]]*ww[i]; hxy[1][1] += mbdy[idx[i]+_nb]*ww[i];
    hxy[0][2] += mbdz[idx[i]]*ww[i]; hxy[1][2] += mbdz[idx[i]+_nb]*ww[i];
  }

}

void TrField::_Fint(double x, double y, double z, int *index, double *weight)
{
  for (int i = 0; i < 8; i++) {
    index [i] = 0;
    weight[i] = 0;
  }
  int _nb = _nx*_ny*_nz;

  int idx0 = _GetIndex(x, y, z);
  if (idx0 < 0 || _nb <= idx0) return;

  int ix = idx0    %_nx; if (ix >= _nx-1) ix = _nx-2;
  int iy = idx0/_nx%_ny; if (iy >= _ny-1) iy = _ny-2;
  int iz = idx0/_nx/_ny; if (iz >= _nz-1) iz = _nz-2;

  double dx[2], dy[2], dz[2];
  dx[1] = (x-_x[ix])/(_x[ix+1]-_x[ix]); dx[0] = 1-dx[1];
  dy[1] = (y-_y[iy])/(_y[iy+1]-_y[iy]); dy[0] = 1-dy[1];
  dz[1] = (z-_z[iz])/(_z[iz+1]-_z[iz]); dz[0] = 1-dz[1];

  int l = 0;
  for (int k = 0; k < 2; k++)
    for (int j = 0; j < 2; j++)
      for (int i = 0; i < 2; i++) {
	index [l] = _Index(ix+i, iy+j, iz+k);
	weight[l] = dx[i]*dy[j]*dz[k];
	l++;
      }
}

int TrField::_GetIndex(double x, double y, double z) const
{
  if (!_x || !_y || !_z) return -1;

  int ix = (int)((x-_x[0])/_dx);
  int iy = (int)((y-_y[0])/_dy);
  int iz = (int)((z-_z[0])/_dz);

  return _Index(ix, iy, iz);
}





inline int TrField::_Index(int i, int j, int k) const
{
    return (0 <= i && i < _nx && 0 <= j && j < _ny && 0 <= k && k < _nz) 
          ? (k*_ny+j)*_nx+i : -1; 
}




void TrField::Print(){

  printf("mfile  %s \n",(char*)mfile);
  printf("iniok  %d \n",iniok);
  for (int ix=0;ix<_nx;ix++)
    for (int iy=0;iy<_ny;iy++)
      for (int iz=0;iz<_nz;iz++)
	
	printf(" %2d %2d %2d X: %+6.1f Y: %+6.1f Z: %+6.1f BX: %+f BY: %+f BZ: %+f BdX0: %+f BdY0: %+f BdZ0: %+f BdX1: %+f BdY1: %+f BdZ1: %+f  BcX0: %+f BcY0: %+f BcZ0: %+f \n",
	       ix,iy,iz,_x[ix],_y[iy],_z[iz],_bx[iz][iy][ix],_by[iz][iy][ix],_bz[iz][iy][ix],
	       _bdx[0][iz][iy][ix],_bdy[0][iz][iy][ix],_bdz[0][iz][iy][ix],
	       _bdx[1][iz][iy][ix],_bdy[1][iz][iy][ix],_bdz[1][iz][iy][ix],
	       _bxc[iz][iy][ix],_byc[iz][iy][ix],_bzc[iz][iy][ix]);





}
