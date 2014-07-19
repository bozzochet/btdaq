//  $Id: TrFit.h,v 1.14 2008/12/13 15:03:52 haino Exp $
#ifndef __TrFit__
#define __TrFit__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrFit
///\brief A class to manage general track fitting 
///\ingroup tkrec
///
/// Track fitting routines imported from TrackFit_utils.F
///
/// Usage: 
///  TrFit fit;   // Create an instance
///  fit.Add(x, y, z, ex, ey, ez);   // Add a hit point
///  ...
///  fit.Fit(method);    // Do fitting
/// 
/// Fitting methods (shown in EMethods)
/// LINEAR  : Linear fitting in X-Z and Y-Z planes (used in no-B)
/// CIRCLE  : Circlar fitting in Y-Z and linear in X-S (used in const.B)
/// SIMPLE  : Simple fitting, no multiple scattering, and coarse B field
///           (used for pattern scan)
/// ALCARAZ : Fitting method by J.Alcaraz (NIMA 533 (2005) 613)
/// CHOUTKO : Fitting method by V.Choutko (default method in tkfitg)
///
///
///\date  2007/12/12 SH  First import (SimpleFit)
///\date  2007/12/14 SH  First import (tkfitg)
///\date  2007/12/20 SH  First stable version after a refinement
///\date  2007/12/20 SH  All the parameters are defined in double
///\date  2007/12/21 SH  New methods (LINEAR, CIRCLE) added, not yet tested
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/11/25 SH  Splitted into TrProp and TrFit
///\date  2008/12/01 SH  Fits methods debugged and checked
///\date  2008/12/11 SH  NORMAL renamed as CHOUTKO, and ALCARAZ fit added
///
///$Date: 2008/12/13 15:03:52 $
///
///$Revision: 1.14 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"

/// Particle propagation manager
/*!
 *  Propagation algorithm imported by SH from TrackFit_utils by V.Choutko
 */
class TrProp {

public:
  enum { 
    /// Number of dimensions for the propagation matrix
    NDIM = 5
  };

  /// Proton mass in GeV
  static double Mproton; 

  /// Speed of light in m/s
  static double Clight;

protected:
  // Track parameters
  double _p0x;           ///< Track reference point P0(X)
  double _p0y;           ///< Track reference point P0(Y)
  double _p0z;           ///< Track reference point P0(Z)
  double _dxdz;          ///< Track direction (dX/dZ) at P0
  double _dydz;          ///< Track direction (dY/dZ) at P0
  double _rigidity;      ///< Rigidity (GV)

  // Particle information used for propagation and multiple scattering
  double _mass;          ///< Particle mass (GeV/c2)
  double _chrg;          ///< Particle charge (e)

public:
  /// Constructor with default values
  TrProp(double p0x   = 0, double p0y = 0, double p0z = 0, 
	 double theta = 0, double phi = 0, double rigidity = 0);
  /// Constructor with AMSPoint and AMSDir
  TrProp(AMSPoint p0, AMSDir dir, double rigidity = 0);

  // Access functions
  double GetP0x (void) const { return _p0x;  }
  double GetP0y (void) const { return _p0y;  }
  double GetP0z (void) const { return _p0z;  }
  double GetDxDz(void) const { return _dxdz; }
  double GetDyDz(void) const { return _dydz; }

  double GetRigidity(void) const { return _rigidity; }

  /// Set particle mass and charge (Z)
  void SetMassChrg(double mass, double chrg) { _mass = mass; _chrg = chrg; }

  /// Clear data members
  void Clear();

  /// Propagete on the track up to Z=zpl plane
  /*
   *  _p0x,_p0y,_p0z,_dxdz,_dydz are changed as of _p0z = zpl */
  double Propagate(double zpl);

  /// Build interpolation lists onto Z=zpl[i] (0<=i<nz) planes given
  void Interpolate(int nz, double *zpl,
		   AMSPoint *plist, AMSDir *dlist = 0, double *llist = 0);

  /// Track interpolation (onto general plane)
  /*
   *  Another Interpolate method is recomended to use 
   *  for the interpolation onto Z=const. planes.
   */
  double Interpolate(AMSPoint &pnt, AMSDir &dir);

  /// Track interpolation (onto cylindrical surface)
  double InterpolateCyl(AMSPoint &pnt, AMSDir &dir, double radius, int idir);

protected:
  /// Transportation (for VC's method)
  double VCFitPar(double*, double*, double*, double[][5], int);

  /// Transportation for cylinder (for VC's method)
  double VCFitParCyl(double*, double*, double*);

  /// Transportation (for VC's method)
  void VCFuncXY(double*, double*, double*, int);

  /// Transportation (for JA's method)
  int JAStepPin(double*, double*, double*, double*, double*, double, int);

public:
  /// 5x5 matrix multiplication imported from Fortran
  static void Dmm55(double[][5], double[][5], double[][5]);

  /// Runge-Kutta transportation imported from Fortran
  static void Rkuta(double, double, double*, double*);
};

class TrFit : public TrProp {

public:
  enum ENums{ 
    /// Maximum number of layer
    LMAX = 8, 
    /// Parameter buffer size
    PMAX = 10 
  };
  enum EMethods{ LINEAR, CIRCLE, SIMPLE, ALCARAZ, CHOUTKO };

  /// Multiple scattering switch
  static int _mscat;

  /// Energy loss correction switch
  static int _eloss;

protected:
  int    _nhit;          ///< Number of hits
  int    _nhitx;         ///< Number of hits in X
  int    _nhity;         ///< Number of hits in Y
  int    _nhitxy;        ///< Number of hits in X and Y

  double _xh[LMAX];      ///< Hit position in X
  double _yh[LMAX];      ///< Hit position in Y
  double _zh[LMAX];      ///< Hit position in Z
  double _xs[LMAX];      ///< Fitting error in X
  double _ys[LMAX];      ///< Fitting error in Y
  double _zs[LMAX];      ///< Fitting error in Z
  double _xr[LMAX];      ///< Fitting residual in X
  double _yr[LMAX];      ///< Fitting residual in Y
  double _zr[LMAX];      ///< Fitting residual in Z

  double _bx[LMAX];      ///< Magnetic field in X
  double _by[LMAX];      ///< Magnetic field in Y
  double _bz[LMAX];      ///< Magnetic field in Z

  double _chisqx;        ///< Fitting chisquare in X (Not normalized)
  double _chisqy;        ///< Fitting chisquare in Y (Not normalized)
  int    _ndofx;         ///< Number of degree of freedom in X
  int    _ndofy;         ///< Number of degree of freedom in Y

  // Normalized chisquare
  double _chisq;         ///< Fitting chisquare, _chisq(x+y)/_ndof(x+y)
  double _errrinv;       ///< Error of 1/Rigidity (1/GV)

  /// Fitting parameters temporary buffer
  double _param[PMAX];   //!

public:
  /// Default constructor
  TrFit(void);

  /// Destructor
 ~TrFit();

  int    GetNhit  (void) const { return _nhit;   }
  int    GetNhitX (void) const { return _nhitx;  }
  int    GetNhitY (void) const { return _nhity;  }
  int    GetNhitXY(void) const { return _nhitxy; }

  int    GetNdofX (void) const { return _ndofx;  }
  int    GetNdofY (void) const { return _ndofy;  }
  double GetChisqX(void) const { return _chisqx; }
  double GetChisqY(void) const { return _chisqy; }

  double GetChisq   (void) const { return _chisq; }
  double GetErrRinv (void) const { return _errrinv;  }

  double GetXh(int i) const { return (0 <= i && i < PMAX)? _xh[i] : 0; }
  double GetYh(int i) const { return (0 <= i && i < PMAX)? _yh[i] : 0; }
  double GetZh(int i) const { return (0 <= i && i < PMAX)? _zh[i] : 0; }

  double GetXs(int i) const { return (0 <= i && i < PMAX)? _xs[i] : 0; }
  double GetYs(int i) const { return (0 <= i && i < PMAX)? _ys[i] : 0; }
  double GetZs(int i) const { return (0 <= i && i < PMAX)? _zs[i] : 0; }

  double GetXr(int i) const { return (0 <= i && i < PMAX)? _xr[i] : 0; }
  double GetYr(int i) const { return (0 <= i && i < PMAX)? _yr[i] : 0; }
  double GetZr(int i) const { return (0 <= i && i < PMAX)? _zr[i] : 0; }

  /// Recomended to use GetP0x(), GetDxDz(),. etc. instead of Getparam
  double GetParam(int i) const {
    return (0 <= i && i < PMAX)? _param[i] : 0;
  }

  /// Clear data members
  void Clear();

  /// Add a new hit with a position by AMSPoint and its error (ex,ey,ez)
  int Add(AMSPoint pos, double ex, double ey, double ez, int at = -1) {
    return Add(pos.x(), pos.y(), pos.z(), ex, ey, ez, at);
  }

  /// Add a new hit with a position and its error by AMSPoint
  int Add(AMSPoint pos, AMSPoint err, int at = -1) {
    return Add(pos.x(), pos.y(), pos.z(), err.x(), err.y(), err.z(), at);
  }

  /// Add a new hit with a position (x,y,z) and its error (ex,ey,ez)
  int Add(double xh, double y,  double z,
	  double ex, double ey, double ez, int at = -1);

  /// Add a new hit with a position, its error, and B field
  int Add(double x,  double y,  double z,
	  double ex, double ey, double ez, 
	  double bx, double by, double bz, int at = -1);

  /// Do fit, fitting algorithm to be selected
  double Fit(int method = CHOUTKO);

  /// Linear fitting in X-Z and Y-Z planes
  double LinearFit(void);

  /// Circlar fitting in Y-Z plane and liear fitting in X-S plane
  double CircleFit(void);

  /// Simple fit (no scattering, coarse field)
  double SimpleFit(void);

  /// Alcaraz fit
  double AlcarazFit(void);

  /// Choutko fit
  double ChoutkoFit(void);

  /// Linear fitting for X-Z (side=1), Y-Z (side=2) or X-S (side=3)
  double LinearFit(int side);

  /// Circlar fitting for X-Z (side=1) or Y-Z (side=2)
  double CircleFit(int side);

  /// Analytic polynominal fitting, ndim=2,3,4 are available
  double PolynomialFit(int side, int ndim);

protected:
  /// Weighted sum of vectors (for CircleFit)
  double VKAverage(int, double*, double*, double*);

  /// Fitting residual (for CircleFit)
  double VKResidual(double, double, double*);

  /// Fill F and G matrices (for JA's method)
  int JAFillFGmtx(double*, double*, double*, double*, int);

  /// Fill V and W matrices (for JA's method)
  int JAFillVWmtx(double*, double*, double*, double*, int*);

  /// Minimize parameters (for JA's method)
  int JAMinParams(double*, double*, int, int);

  /// Get error matrix (for VC's method)
  void VCErrMtx(int, double, double*, double*, double &, double &);

public:
  /// 3x3 Matrix inversion imported from ROOT
  static int Inv33(double mtx[3][3]);
  /// 4x4 Matrix inversion imported from ROOT
  static int Inv44(double mtx[4][4]);
  /// 5x5 Matrix inversion imported from ROOT
  static int Inv55(double mtx[5][5]);
  /// 5x5 Matrix inversion imported from ROOT
  static int Inv66(double mtx[6][6]);
};

#endif
