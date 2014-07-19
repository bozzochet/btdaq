/// $Id: TkCoo.h,v 1.13 2008/10/15 18:50:18 zuccon Exp $ 
#ifndef  _TKCOO_
#define  _TKCOO_

//////////////////////////////////////////////////////////////////////////
///
///\file  TkCoo.h
///\brief Header file of TkCoo class
///
///\class TkCoo
///\brief A library for the calculation of the local and global coordinate
///\ingroup tkrec
///
///\date  2008/02/21 PZ  First version
///\date  2008/03/19 PZ  Add some features to TkSens
///\date  2008/04/10 AO  GetLocalCoo(float) of interstrip position  
///\date  2008/04/22 AO  Swiching back some methods  
///$Date: 2008/10/15 18:50:18 $
///
/// $Revision: 1.13 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"

class TkSens;

/// Class that hold the routines to get local and global Coordiantes for the tracker strips.
class TkCoo{

 public:

  /// \brief Returns the max Coo multiplicity due to K side ambiguity. 
  /// A tkid and a readout channel number must be provided. The answer for and S-side readout channel is always 1.  
  static int   GetMaxMult(int tkid, int   readchann) { return GetMaxMult(tkid,(float)readchann); }
  static int   GetMaxMult(int tkid, float readchann);
  /// \brief Returns the local coordinate corresponding on the readout channel \param readchann on the ladder indicated by \param tkid.
  /// The third field correspond to the requested multiplicity ( 0 to GetMaxMult(tkid,readchann)-1 ).
  /// If  mult is > GetMaxMult(tkid,readchann)-1   the returned coordinate correspont to mult= GetMaxMult(tkid,readchann)-1
  static float GetLocalCoo(int tkid, int   readchann, int mult);
  static float GetLocalCoo(int tkid, float readchann, int mult);

  /// \brief Returns the Global coordinate (Nominal) for the point on ladder 
  /// \param tkid with local Coo X and Y
  static AMSPoint GetGlobalN(int tkid,float X, float Y);
  /// \brief Returns the Global coordinate (Nominal) for the point on ladder 
  /// \param tkid with local readout chann and mult
  static AMSPoint GetGlobalNC(int tkid,float readchanK, float readchanS,int mult=0);
 
  /// \brief Returns the Global coordinate (Nominal) for the point on ladder 
  /// \param tkid with local given by the AMSPoint loc (Z value is ignored)
  static AMSPoint GetGlobalN(int tkid,AMSPoint& loc);
  /// \brief Returns the Global coordinate (Alignment Corrected) for the point on ladder 
  /// \param tkid with local Coo X and Y
  static AMSPoint GetGlobalA(int tkid,float X, float Y);
  /// \brief Returns the Global coordinate (Alignment Corrected) for the point on ladder 
  /// \param tkid with local readout chann and mult
  static AMSPoint GetGlobalAC(int tkid,float readchanK, float readchanS,int mult=0);
  /// \brief Returns the Global coordinate (Alignment Corrected) for the point on ladder 
  /// \param tkid with local given by the AMSPoint loc (Z value is ignored)
  static AMSPoint GetGlobalA(int tkid,AMSPoint& loc);

 private:

  /// Local coo from the S channel
  static float GetLocalCooS(int readchann);
  /// Local coo from the K5 channel
  static float GetLocalCooK5(int readchann, int mult);
  /// Local coo from the K7 channel
  static float GetLocalCooK7(int readchann, int mult);
  /// Implanted Strip on a sensor from bonded strip index on the sensor
  static int   ImplantedFromReadK7(int readstrip);

  friend class TkSens;

public:
  /// Returns the lenght of ladder
  static double GetLadderLength(int tkid);
  /// Return the X center of a ladder
  static double GetLadderCenterX(int tkid); 
  /// Return the Y center of a ladder
  static double GetLadderCenterY(int tkid);
};

#endif
