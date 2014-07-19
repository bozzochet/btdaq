/// $Id: TkSens.h,v 1.11 2008/12/02 23:27:50 haino Exp $ 
#ifndef _TKSENS_
#define _TKSENS_

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkCoo
///\brief A library for the calculation from the global to local coordinate
///\ingroup tkrec
///
///\date  2008/03/18 PZ  First version
///\date  2008/04/02 SH  Some bugs are fixed
///\date  2008/04/18 SH  Updated for alignment study
///\date  2008/04/21 AO  Ladder local coordinate
///$Date: 2008/12/02 23:27:50 $
///
/// $Revision: 1.11 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"

//! Tracker Utility class to get the ladder sensor readout channel from a point in the Global frame 
class TkSens{

private:
  //! The input
  AMSPoint GlobalCoo;

  // From now on all the data part is calculated
  //! The ladder identifier ( -1 if none)
  int lad_tkid;
  //! sensor on the ladder
  int sens;
  //! Multiplicity number
  int mult;
  //! local coo on the sensor
  AMSPoint SensCoo;
  /// Local coordinate in the ladder frame
  AMSPoint LaddCoo;
  //! nearest K side readout channel
  int ReadChanX;
  //! nearest S side readout channel
  int ReadChanY;
  //! The Impact point in strip pitch unit wrt the nearest strip
  number ImpactPointX;

  //! The Impact point in strip pitch unit wrt the nearest strip
  number ImpactPointY;

  //! checks if the point is on a layer
  int  GetLayer();
  //! checks if it is inside the ladder pointed by lad
  bool IsInsideLadder(int tkid);
  //! checks if it is inside the ladder pointed by lad
  bool IsInsideLadder(TkLadder* lad);

  //! Determines on which ladder (if any) is the point
  int  FindLadder();

  //! find the sensor number on the ladder(0-n) and the local coo on it
  int  GetSens();

  //! find the nearest readout channel from sensor S local coo
  int GetStripFromLocalCooS(number Y);
  //! find the nearest readout channel from sensor K5 local coo
  int GetStripFromLocalCooK5(number X,int sens);
  //! find the nearest readout channel from sensor K7 local coo
  int GetStripFromLocalCooK7(number X,int sens);

public:
  // Default Constructor
  TkSens();

  //! Constructor calculates all the quantities from a point in global coo
  TkSens(AMSPoint& GCoo);

  //! Constructor calculates all the quantities from the tkid and  a point in global coo
  TkSens(int tkid,AMSPoint& GCoo);

  //! (re)set the global coo
  void SetGlobalCoo(AMSPoint& pp) {Clear(); GlobalCoo=pp; Recalc();}
  void SetGlobalCoo(int tkid, AMSPoint& pp) {Clear(); lad_tkid=tkid; GlobalCoo=pp; Recalc();} 	
  //! Recalc everything from the global coo
  void Recalc();  

  //! True if the point is on a ladder
  bool     LadFound()     { return lad_tkid!=0;}
  //! Returns the ladder id ( 0 if not on ladder)
  int      GetLadTkID()   { return lad_tkid;}
  //! Returns the sensor number
  int      GetSensor()    { return sens;}
  //! Returns the multiplicity index
  int      GetMultIndex() { return mult; }
  //! Returns the Global Coo
  AMSPoint GetGlobalCoo() { return GlobalCoo;}
  //! Returns the Local Coo on the sensor
  AMSPoint GetSensCoo()   { return SensCoo;}
  //! Returns the Local Coo on the ladder
  AMSPoint GetLaddCoo()   { return LaddCoo;}
 //! Returns the nearest readout channel from K side
  int      GetStripX()    { return ReadChanX;}
  //! Returns the nearest readout channel from S side
  int      GetStripY()    { return ReadChanY;}
  //! Returns the Impact point in strip pitch unit wrt the nearest strip
  number GetImpactPointX(){return ImpactPointX;}

  //! Returns the Impact point in strip pitch unit wrt the nearest strip
  number GetImpactPointY(){return ImpactPointY;}
  //! Clear the calculated content
  void Clear();

};

#endif
