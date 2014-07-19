// $Id: TrCluster.h,v 1.41 2009/01/12 15:21:09 oliva Exp $ 
#ifndef __AMSTrCluster__
#define __AMSTrCluster__

#include "TrRawCluster.h"
#include "TkDBc.h"
#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TrParDB.h"
#include "TrLadPar.h"
#include "TkCoo.h"
#include "link.h"
#include <cmath>
#include <vector>
#include <string>

/*!
\class AMSTrCluster
\brief A class to manage reconstructed cluster in AMS Tracker
\ingroup tkrec

 AMSTrCluster is the core of the Tracker reconstruction. 
 New ladder geometry (TKDBc) and calibration databases (TrCalDB) 
 and strip database (TkStrip) are used instead of 
 the original TKDBc, TrIdSoft, and TrIdGeom. 

\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
\date  2008/01/14 SH  First stable vertion after a refinement
\date  2008/01/20 SH  Imported to tkdev (test version)
\date  2008/02/13 SH  _signal array is changed as vector
\date  2008/02/16 AO  New data format 
\date  2008/02/19 AO  Signal corrections
\date  2008/02/26 AO  Eta based local coordinate (see TkCoo.h)
\date  2008/03/01 AO  Added member _seedind
\date  2008/03/06 AO  Changing some data members and methods
\date  2008/03/31 AO  Eta and CofG methods changing
\date  2008/04/11 AO  XEta and XCofG coordinate based on TkCoo
\date  2008/06/19 AO  Using TrCalDB instead of data members 
\date  2008/12/11 AO  Some method update

 $Date: 2009/01/12 15:21:09 $

 $Revision: 1.41 $

*/

//class AMSTrCluster : public TObject, public AMSlink {
class AMSTrCluster : public AMSlink {

 public:
  
  enum CorrectionOptions {
    /// No Correction Applied
    kNoCorr       = 0x00,       
    /// Signal Corr.: Cluster Asymmetry Correction (left/right)
    kAsym         = 0x01, 
    /// Total Signal Corr.: Energy Loss Normalization at 300 um [cos(Theta)^-1]
    kAngle        = 0x02,
    /// Total Signal Corr.: Gain Correction
    kGain         = 0x04, 
    /// Total Signal Corr.: VA Gain Correction (appliable only with gain correction)
    kVAGain       = (0x08 & 0x04), 
    /// Total Signal Corr.: Charge Loss Correction 
    kLoss         = 0x10,
    /// Total Signal Corr.: P/N Normalization Correction     
    kPN           = 0x20
  };
  static int DefaultCorrOpt;
  static int DefaultUsedStrips;

 private:

  /// Multiplicity 
  short int     _mult;   //!
  /// Local coordinate by multiplicity index 
  vector<float> _coord;  //!
  /// Global coordinate by multiplicity index
  vector<float> _gcoord; //!
 
 public:
  
  /// TkLadder ID (layer *100 + slot)*side 
  short int    _tkid;
  /// First strip address (0-639 for p-side, 640-1023 for n-side)
  short int    _address;
  /// Number of strips
  short int    _nelem;
  /// Seed index in the cluster (without correction, 0<=_seedind<_nelem)
  short int    _seedind;
  /// ADC data array
  std::vector<float> _signal;
  /// Cluster status 
  unsigned int _status;
  /// tan(ThetaXZ)
  float        _dxdz;
  /// tan(ThetaYZ)
  float        _dydz;
  
 protected:
  
  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
  /// Pointer to the parameters database
  static TrParDB* _trpardb;
 
 public:
  
  /// Default constructor
  AMSTrCluster(void);
  /// Constructor with data
  AMSTrCluster(int tkid, int side, int add, int nelem, int seedind, float* adc, unsigned int status);
  /// Constructor divided is several instructions 
  AMSTrCluster(int tkid, int side, int add, int seedind, unsigned int status);
  /// Insert a strip in the cluster
  void push_back(float adc);
  /// Copy constructor
  AMSTrCluster(const AMSTrCluster& orig);
  /// Destructor
  ~AMSTrCluster();
  /// Clear
  void Clear();

  /// Using this calibration database
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB() { return _trcaldb; }
  /// Using this parameter database
  static void UsingTrParDB(TrParDB* trpardb) { _trpardb = trpardb; }
  /// Get the current parameter database
  TrParDB*    GetTrParDB() { return _trpardb; }

  /// Set track interpolation angle tan(ThetaXZ)
  inline void  SetDxDz(float dxdz) { _dxdz = dxdz; }
  /// Set track interpolation angle tan(ThetaYZ)
  inline void  SetDyDz(float dydz) { _dydz = dydz; }
  /// Get track interpolation angle tan(ThetaXZ)
  inline float GetDxDz()  { return _dxdz; }
  /// Get track interpolation angle tan(ThetaYZ)
  inline float GetDyDz()  { return _dydz; }
  /// Get track interpolation angle theta
  inline float GetTheta() { return acos(1./(1.+_dxdz*_dxdz+_dydz*_dydz)); }

  /// Get ladder TkId identifier 
  int   GetTkId()          const { return _tkid; }
  /// Get ladder layer
  int   GetLayer()         const { return abs(_tkid/100); }
  /// Get ladder slot
  int   GetSlot()          const { return abs(_tkid%100); }
  /// Get cluster side (0: n-side, 1: p-side)
  int   GetSide()          const { return (_address<640)? 1 : 0; }
  /// Get the cluster first strip number (0, ..., 639 for p-side, 0, ..., 383 for n-side)  
  int   GetAddress()       const { return _address; }
  /// Get i-th strip address
  int   GetAddress(int ii);
  /// Get the local coordinate for i-th strip
  float GetX(int ii, int imult = 0) { return TkCoo::GetLocalCoo(GetTkId(),GetAddress(ii),imult); }
  /// Get the cluster strip multiplicity
  int   GetNelem()         const { return _nelem; }
  int   GetLength()              { return GetNelem(); }
  /// Get cluster status
  int   GetStatus()        const { return _status; } 

  /// Get i-th strip signal
  float GetSignal(int ii, int opt = DefaultCorrOpt);
  /// Get i-th strip noise (from calibration)
  float GetSigma(int ii) { return GetNoise(ii);  }
  float GetNoise(int ii);   
  /// Get i-th signal to noise ratio 
  float GetSN(int ii, int opt = DefaultCorrOpt) { return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii,opt)/GetNoise(ii); }
  /// Get i-th strip status (from calibration)
  short GetStatus(int ii);

  /// Build the coordinates (with multiplicity)
  void  BuildCoordinates();
  /// Get multiplicity
  int   GetMultiplicity();
  /// Get local coordinate by multiplicity index
  float GetCoord(int imult); 
  /// Get global coordinate by multiplicity index
  float GetGCoord(int imult);

  /// Get the seed index 
  int   GetSeedIndex(int opt = DefaultCorrOpt);
  /// Get the seed address 
  int   GetSeedAddress(int opt = DefaultCorrOpt) { return GetAddress(GetSeedIndex(opt)); }
  /// Get seed signal 
  float GetSeedSignal(int opt = DefaultCorrOpt)  { return GetSignal(GetSeedIndex(opt),opt); }
  /// Get seed noise 
  float GetSeedNoise(int opt = DefaultCorrOpt)   { return GetNoise(GetSeedIndex(opt)); }
  /// Get seed signal to noise ratio 
  float GetSeedSN(int opt = DefaultCorrOpt)      { return GetSN(GetSeedIndex(opt)); }
  /// Get the numeber of strips on the left of the seed strip
  int   GetLeftLength(int opt = DefaultCorrOpt)  { return GetSeedIndex(opt); }
  /// Get the number of strips on the right of the seed strip
  int   GetRightLength(int opt = DefaultCorrOpt) { return GetNelem() - GetSeedIndex(opt) - 1; } 

  /// Get cluster amplitude
  float GetTotSignal(int opt = DefaultCorrOpt);

  /// Get cluster bounds for a given number of strips (gerarchic order...)  
  /*           _        
   *          | |_      
   *       _ _| | |     
   *     _| | | | |_    
   *  __|_|_|_|_|_|_|__                   
   *     5 3 2 0 1 4                       
   *  Seed is used as reference (position 0) */
  void  GetBounds(int &leftindex, int &rightindex, int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  /// Get the Center of Gravity with the n highest consecutive strips
  float GetCofG(int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  /// Get local coordinate with center of gravity on nstrips
  float GetXCofG(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultCorrOpt) { return TkCoo::GetLocalCoo(GetTkId(),GetSeedAddress(opt)+GetCofG(nstrips,opt),imult); }
  /// Get Eta (center of gravity with the two higher strips)
  /*! Eta = center of gravity with the two higher strips = Q_{R} / ( Q_{L} + Q_{R} )
   *      _                                    _ 
   *    l|c|r          c*0 + r*1    r        l|c|r            l*0 + c*1    c
   *     | |_    eta = --------- = ---       _| |       eta = --------- = ---
   *    _| | |           c + r     c+r      | | |_              l + c     l+c
   * __|_|_|_|__                          __|_|_|_|__
   *      0 1                                0 1
   *  Eta is 1 for particle near to the right strip,
   *  while is approx 0 when is near to the left strip (old definition) */
  float GetEta(int opt=DefaultCorrOpt) { float eta = GetCofG(2,opt); return (eta>0.) ? eta : eta + 1.; }
  /// Digital Head-Tail method
  float GetDHT(int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  /// Get local coordinate with center of gravity on nstrips
  float GetXDHT(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultCorrOpt) { return TkCoo::GetLocalCoo(GetTkId(),GetSeedAddress(opt)+GetDHT(nstrips,opt),imult); }
  /// Analog Head-Tail method
  float GetAHT(int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  float GetXAHT(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultCorrOpt) { return TkCoo::GetLocalCoo(GetTkId(),GetSeedAddress(opt)+GetAHT(nstrips,opt),imult); } 


  /// Print cluster basic information
  std::ostream& putout(std::ostream &ostr = std::cout) const;
  friend std::ostream &operator << (std::ostream &ostr, const AMSTrCluster &cls) { return cls.putout(ostr); }
  /// Print cluster strip variables (A: apply asimmetry correction, G: apply gain correction)
  void Print(int opt = DefaultCorrOpt);
  void Info(int opt = DefaultCorrOpt);

  static string sout;

  void _copyEl(){}
  void _printEl(std::ostream& ostr) { putout(ostr); }
  void _writeEl(){}

  void* operator new(size_t t)   { return TObject::operator new(t);  }
  void  operator delete(void *p) { TObject::operator delete(p); p=0; }
  
  AMSTrCluster* next() { return (AMSTrCluster*) _next;}

  /// ROOT definition
  ClassDef(AMSTrCluster, 1)
};

typedef AMSTrCluster TrClusterR;

#endif
