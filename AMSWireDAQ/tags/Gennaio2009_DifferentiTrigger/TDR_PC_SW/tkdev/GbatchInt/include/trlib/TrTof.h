//  $Id: TrTof.h,v 1.2 2008/11/13 21:23:40 haino Exp $
#ifndef __TrTof__
#define __TrTof__

//////////////////////////////////////////////////////////////////////////
///
///\class TrTof
///\brief A class to manage TOF information for Tracker analysis
/// TOF data are temporarily added for tracker efficiency analysis
/// Assignment scheme as of 29/01/2008 was taken from: 
///  http://ams.cern.ch/AMS/Electronics/SubD/Scint/Docs/numbers-v7.1.pdf
///
/// TOF Plane definitions
/// Plane:1  8 Pads parallel to X-axis at Z= 64.0 and  65.5
/// Plane:2  8 Pads parallel to Y-axis at Z= 61.0 and  62.5
/// Plane:3 10 Pads parallel to Y-axis at Z=-61.0 and -62.5
/// Plane:4  8 Pads parallel to X-axis at Z=-64.0 and -65.5
///
/// TOF Side definitions
/// Side:1 Negative (RAM (-Y) for Plane 2/3, STAR(-X) for Plane 1/4)
/// Side:2 Positive (WAKE(+Y) for Plane 2/3, PORT(+X) for Plane 1/4)
///
/// TOF Crates
/// JinfNum: 4 = SDR-1 = Plane 1(N-side) and Plane 2(P-side)
/// JinfNum: 6 = SDR-0 = Plane 1(P-side) and Plane 2(N-side)
/// JinfNum:18 = SDR-2 = Plane 4(P-side) and Plane 3(P-side)
/// JinfNum:20 = SDR-3 = Plane 4(N-side) and Plane 3(N-side)
///
/// ADC data (uncompressed)
/// Number of channels: 90 = 9 slots*10 ch
/// ch 0-3: LT (LowThreshold)
/// ch 4-8: ??
///
/// TDC data (compressed)
/// Number of channels: 40 = 5 slots*8 ch
/// ch 0-3: LT (LowThreshold)
/// ch 5  : FT (FastTrigger)
/// ch 6  : sum HT  (HighThreshold)
/// ch 7  : sum SHT (SuperHighThreshold)
///
///$Date: 2008/11/13 21:23:40 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"

class TrTof {

  ///////////////////////////////////////////////////////////////
  // TOF event data
  ///////////////////////////////////////////////////////////////
public:
  /// Hit pattern [i*2+j] for plane:i+1 [1:4] and side:j+1 [1:2]
  int HitPattern[4*2];

  /// LT-ADC [(i*2+j)*10+k] for plane:i+1, side:j+1 and pad:k+1 [1:10]
  int Adc[4*2*10];

  /// LT-TDC [(i*2+j)*10+k] for plane:i+1, side:j+1 and pad:k+1 [1:10]
  int Tdc[4*2*10];

  /// FT-TDC [(i*2+j)*2+k] for plane:i+1, side:j+1 and odd/even:k+1 [1:2]
  int FtTdc[4*2*2];

  /// Hit pattern for plane:i+1 estimated from the conincidence of ADC signl
  int AdcHit[4];

  /// Number of hits at each plane
  int Nhit[4];

  /// Number of planes with at leas one hit
  int NplHit;

  /// Hit time in nsec [(i*2+j)*10+k] for plane:i+1, side:j+1 and pad:k+1
  double Time[4*2*10];

  /// Hit position along the pad [i*10+j] for plane:i+1 and pad:j+1
  double Vpos[4*10];

  /// Verbose option
  static int verbose;

  ///////////////////////////////////////////////////////////////
  // TOF geometry database (TOF2DBc)
  ///////////////////////////////////////////////////////////////
public:
  /// max nmb of bars/layer
  enum { SCMXBR = 10 };
  /// max nmb of bars/top(bot) TOF-subsystem
  enum { SCMXBR2 = 18 };
  /// max nmb of layers in TOF-systems
  enum { SCLRS = 4 };
  /// start nmb of abs. numbering of TOF rot. matrixes
  enum { SCROTN = 2 };
  /// Real nmb of sc. bar types (different by length now)
  enum { SCBTPN = 11 };
  /// MAX scint. channels(layers*bars*sides)
  enum { SCCHMX = SCLRS*SCMXBR*2 };
  /// MAX scint. bars*layers
  enum { SCBLMX = SCLRS*SCMXBR };
  /// MAX number of PMTs per side
  enum { PMTSMX = 3 };
  /// Max number of PMT single elecr.spectrum parameters
  enum { SESPMX = 3 };

public:
  /// Number of planes
  static int   _planes;
  /// number of PMT SingleElectrSpectrum(SES) parameters
  static int   _nsespar;
  /// layer-map of bar types (diff. by length)
  static int   _brtype[SCBLMX];
  /// TOF planes rotation mask
  static int   _plrotm[SCLRS];
  /// real bars per plane
  static int   _bperpl[SCLRS];
  /// bar lengthes for SCBTPN types
  static float _brlen [SCBTPN];
  /// eff.light-guide lengthes for SCBTPN types
  static float _lglen [SCBTPN];
  /// outer counters param(wid/xc/yc/lgw) vs layer
  static float _outcp [SCLRS][4];
  /// supporting structure parameters
  static float _supstr[12];
  /// sc. plane structure parameters
  static float _plnstr[20];
  /// SES params for each btyp
  static float _sespar[SCBTPN][SESPMX];

  ///////////////////////////////////////////////////////////////
  // TOF offline calibration data
  ///////////////////////////////////////////////////////////////
public:
  /// ADC pedestal
  static double tofped[80];
  /// ADC gain (ADC ch/MIP)
  static double tofgain[80];
  /// TDC time offset (nsec)
  static double timeoffs[80];
  /// TDC time correction factor
  static double timecorr[80];

  /// TDC unit (nsec/CH)
  static double tunit;
  /// TDC base offset (nsec)
  static double tbas[2];

  ///////////////////////////////////////////////////////////////
  // Temporary data for event display
  ///////////////////////////////////////////////////////////////
public:
  static double exvert[16*4];
  static double tofpxu[9], tofpyu[9], tofpzu[9];
  static double tofpxl[9], tofpyl[9], tofpzl[9];

public:
  /// Construtor
  TrTof(void);

  /// Destrutor
  virtual ~TrTof();

  /// Get bar type for layer il [0-3] and pad ib[0-9]
  static int      brtype (int il, int ib);
  /// Get pad u- position for layer il [0-3] and pad ib[0-9]
  static double   gettsc (int il, int ib);
  /// Get pad z- position for layer il [0-3] and pad ib[0-9]
  static double   getzsc (int il, int ib);
  /// Get pad 3D position for layer il [0-3] and pad ib[0-9]
  static AMSPoint getpos (int ip, int ib);
  /// Get pad size for layer il [0-3] and pad ib[0-9]
  static AMSPoint getsize(int ip, int ib);
  /// Get pad width for il [0-3], ib[0-9], v-position, and side, ss
  static double   getwid (int ip, int ib, double v, int ss);

  /// Read geometry data base from file
  static int tofdbc(const char *fname = "dat/tof2const.dat");
  /// Init geometry data
  static void setdbpars(void);
  /// Read calibration data base from file
  static int tofcal(const char *fname = "dat/tofcal.dat");

  /// Clear ADC/TDC data
  void Clear(void);

  /// Decode ADC/TDC data
  int Decode(int offset, int size, unsigned short *bblock);

  /// Reconstruct hits
  int Reconstruct(void);

  /// ROOT definition
  ClassDef(TrTof, 1)
};

#endif
