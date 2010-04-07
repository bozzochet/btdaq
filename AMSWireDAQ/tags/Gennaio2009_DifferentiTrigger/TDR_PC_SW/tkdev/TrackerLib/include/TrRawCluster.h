// $Id: TrRawCluster.h,v 1.28 2009/01/12 15:22:49 oliva Exp $ 
#ifndef __TrRawCluster__
#define __TrRawCluster__

//////////////////////////////////////////////////////////////////////////
///
///
///\class AMSTrRawCluster
///\brief A class implemnting the Tracker Raw Clusters
///\ingroup tkrec
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  Some comments and utils are added
///\date  2008/02/13 SH  ADC,sigma,status arrays are changed as vector
///\date  2008/02/16 AO  Renaming and new methods
///\date  2008/06/19 AO  Using TrCalDB instead of data members 
///
/// $Date: 2009/01/12 15:22:49 $
///
/// $Revision: 1.28 $
///
//////////////////////////////////////////////////////////////////////////
#include "TObject.h"
#include "link.h"
#include "TrCalDB.h"
#include "TrLadCal.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>



//class AMSTrRawCluster : public AMSlink, public TObject {
class AMSTrRawCluster : public AMSlink {
 public:
  /// TkLadder ID (layer *100 + slot)*side
  short int _tkid;
  /// First strip number
  short int _address;
  /// Number of strips
  short int _nelem;
   
  /// ADC data array
  std::vector<float> _signal;
  
 protected:
  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
  
 public:
  /// Default constructor
  AMSTrRawCluster(void);
  /// Copy constructor
  AMSTrRawCluster(const AMSTrRawCluster& orig);
  /// Constructors with raw data
  AMSTrRawCluster(int tkid, int add, int nelem, short int *adc);
  AMSTrRawCluster(int tkid, int add, int nelem, float *adc);
  /// Destructor
  ~AMSTrRawCluster(){}
  /// Clear
  void Clear();

  /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB() { return _trcaldb; }

  /// Get ladder TkId identifier 
  int GetTkId()      const { return _tkid; }
  /// Get ladder HwId identifier 
  int GetHwId()      const{
    TkLadder* lad=TkDBc::Head->FindTkId(_tkid);
    if(lad) return lad->GetHwId();
    else return -1;
  }
  /// Get ladder plane identifier 
  int GetLayer()     const { return abs(_tkid/100); }
  /// Get ladder X side on plane identifier 
  int GetLayerSide() const { return (_tkid>0)?1:-1; }
  /// Get the slotidentifier 
  int GetSlot()      const { return (abs(_tkid)-GetLayer()*100)*GetLayerSide(); }
  /// Get cluster side (0 -> p, 1 -> n)
  int GetSide()      const { return(_address>639) ? 0 : 1; }
  /// Get the cluster first strip number  
  int GetAddress()   const { return _address; }
  /// Get i-th strip address
  int GetAddress(int ii)   { return GetAddress() + ii; } 
  /// Get the cluster strip multiplicity
  int GetNelem()     const { return _nelem; }
  //  /// Get the gain
  //   double GetGain()   const { return _gain; }
  /// Get i-th strip signal
  float GetSignal(int ii) const { return _signal.at(ii); }
  /// Same as GetNoise() 
  float GetSigma (int ii) { return GetNoise(ii); }
  /// Get i-th strip sigma (from calibration file)
  float GetNoise(int ii);   
  /// Get i-th signal to noise ratio
  float GetSN(int ii, const char* options = "AG") { return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii)/GetNoise(ii); }

  /// Get i-th strip status (from calibration)
  short GetStatus(int ii);
  /*
    Status definition (thresholds...???):
    bit 0 set to 1: dead channel on the sigma raw base (pedestal subtraction)
    bit 1 set to 1: noisy channel on the sigma raw base (pedestal subtraction) 
    bit 2 set to 1: dead channel on the sigma base (pedestal and CN subtraction)
    bit 3 set to 1: noisy channel on the sigma base (pedestal and CN subtraction)
    bit 4 set to 1: non gaussian channel 
  */
  /// Check if the cluster contains strip for the side
  int CheckSide(int side) const { return (side == 0) ? (640 <= _address+_nelem) : (_address < 640); }

  /// Print information
  std::ostream& putout(std::ostream &ostr = std::cout) const;
  /// ostream operator
  friend std::ostream &operator << 
    (std::ostream &ostr, const AMSTrRawCluster &cls) { 
    return cls.putout(ostr);
  }
  /// Print raw cluster strip variables 
  void Print();
  void Info();

  /// Get the seed index in the cluster with re-clustering (<0 if wrong)
  int   GetSeedIndex(float thseed = 3.);
  /// Get the seed address with re-clustering (<0 if wrong)
  int   GetSeedAddress(float thseed = 3.);
  /// Get seed signal with re-clustering (<0 if wrong)
  float GetSeedSignal(float thseed = 3.);
  /// Get seed signal with re-clustering (<0 if wrong)
  float GetSeedNoise(float thseed = 3.);
  /// Get seed signal to noise ratio with re-clustering (<0 if wrong)
  float GetSeedSN(float thseed = 3.);
  /// Get the numeber of strips on the left of the seed strip with re-clustering (<0 if wrong)
  int   GetLeftLength(float thseed = 3.,float thneig = 1.);
  /// Get the number of strips on the right of the seed strip with re-clustering (<0 if wrong)
  int   GetRightLength(float thseed = 3.,float thneig = 1.);
  /// Get cluster length with re-clustering (<0 if wrong)
  int   GetLength(float thseed = 3.,float thneig = 1.);
  /// Get cluster amplitude with re-clustering (<0 if wrong)
  float GetTotSignal(float thseed = 3.,float thneig = 1.);
  float GetTotSignal2(float thseed = 3.,float thneig = 1.);
  /// Get the status of the n higher strips of the cluster with re-clustering (<0 if wrong)
  int   GetStatusnStrips(int nstrips, float thseed = 3., float thneig = 1.);
  /// Get cluster eta (center of gravity with the two higher strips)
  float GetEta(float thseed = 3.,float thneig = 1.);
  /// Get cluster address with eta
  float GetEtaAddress(float thseed = 3., float thneig = 1.);
  /// For compatibility with trigger lvl3
  integer lvl3format(int16 * adc, integer nmax,int lvl3dcard_par=1  ,integer matchedonly=0);

  static std::string sout;

  static void lvl3CompatibilityAddress(int16u address, integer & strip, integer & va, integer & side,  integer &drp){
    drp=address%100;
    strip=address/1000;
    va=strip/64;
    side=va>9 ? 0 : 1;
  }
  

  AMSTrRawCluster* next(){ return (AMSTrRawCluster*) _next;}
  void _copyEl(){}
  void _printEl(std::ostream&){}
  void _writeEl(){}
  void * operator new(size_t t) {return TObject::operator new(t); }
  void operator delete(void *p)   {TObject::operator delete(p);p=0;}


  /// ROOT definition
  ClassDef(AMSTrRawCluster, 1)
};
typedef AMSTrRawCluster TrRawClusterR;
#endif

