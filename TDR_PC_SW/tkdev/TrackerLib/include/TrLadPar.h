#ifndef __TrLadPar__
#define __TrLadPar__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrLadPar.h
///\brief Header file of TrLadPar class
///
///\class TrLadPar
///\brief The ladder parameters data base
///\ingroup tkdbc
///
/// author: A. Oliva -- INFN Perugia 19/06/2008
///
///\date  2008/06/19 AO  First version
///
//////////////////////////////////////////////////////////////////////////

#include "typedefs.h"
#include "TObject.h"


//!Class used to store Tracker Ladder parameters
class TrLadPar : public TObject {
  
 public:  

  /// Hardware ID (HwId = iTDR + iCrate*100)
  short int _hwid;
  
 public:

  /// Ladder Gains (XY) (average of VAs:  11, ..., 16 and 1, ..., 10)
  float _gain[2];
  /// Ladder Gain for each VA (mean of 64 strip signals) 
  float _vagain[16];
  
 public:

  /// Default constructor
  TrLadPar(short int hwid = 0);
  /// Copy constructor
  TrLadPar(const TrLadPar& orig);
  /// Default destructor
  ~TrLadPar(){}
  /// Clear data members
  void Clear();
  
  /// Set Ladder Gain (XY) 
  inline void  SetGain(float* gain)           { for (int ii=0; ii< 2; ii++) _gain[ii] = gain[ii]; }  
  /// Set Ladder Gain (XY) by index
  inline void  SetGain(int icoo, float gain)  { _gain[icoo] = gain; }
  /// Set Gain for each VA (fine correction) 
  inline void  SetVAGain(float* gain)         { for (int ii=0; ii<16; ii++) _vagain[ii] = gain[ii]; }
  /// Set Gain for each VA (fine correction) by index
  inline void  SetVAGain(int iva, float gain) { _vagain[iva] = gain; }

  /// Get Ladder Gain (XY) 
  inline float GetGain(int icoo)  { return _gain[icoo]; }
  /// Get Gain for each iVA (fine correction) 
  inline float GetVAGain(int iva) { return _vagain[iva]; }

  /// Print info 
  void PrintInfo();

  ClassDef(TrLadPar,1);
};

#endif
