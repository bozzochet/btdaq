//  $Id: TrGeom.h,v 1.2 2008/04/02 11:36:04 haino Exp $
#ifndef __TrGeom__
#define __TrGeom__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrGeom.h
///\brief Header file of TrGeom class
///
///\class TrGeom
///\brief A class implemnting the Tracker geometry
///\ingroup tksim
///
/// Everything is defined as static.
/// You just need to call TrGeom::Build(mvol) with mother volume, mvol.\n
///
/// ID numbers are defined as follows \n
/// For layer and plane support    : layer number beginning from 1 \n
/// For ladder, hybrid and support : tkid+1000 (negative is forbidden) \n
/// For sensor  : tkid+1000+1000*(sensor+1), sensor beginning from 0 \n
///
///\date  2008/02/19 SH  First import from Gbatch
///\date  2008/03/14 SH  First stable version after a refinement
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Update for alignment study
///$Date: 2008/04/02 11:36:04 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"

class AMSgvolume;

class TrGeom {

public:

  /// Number of layers
  enum { maxlay = trconst::maxlay };

  /// Layer radius in cm
  static double layer_d1[maxlay];
  /// Layer half thickness in cm
  static double layer_d2[maxlay];
  /// Plane support thickness in cm
  static double sup_hc_w[maxlay];
  /// Plane support radius in cm
  static double sup_hc_r[maxlay];
  /// Plane support z-offset in cm
  static double sup_hc_z[maxlay];
  /// Electronics dimension parameters
  static double zelec[3];
  /// Ladder support whickness in cm
  static double sup_foam_w;
  /// Ladder support z-offset in cm
  static double sup_foam_tol;

protected:
  /// Rotation matrix index
  static int _nrot;

public:
  /// Build tracker geometry
  static void Build(AMSgvolume *mother);

protected:
  /// Build layer geometry (layer number begins from 1)
  static AMSgvolume *BuildLayer(AMSgvolume *mvol, int layer);

  /// Build ladder geometry
  static AMSgvolume *BuildLadder(AMSgvolume *mvol, int tkid);

  /// Build sensor geometry (sensor number begins from 0)
  static void BuildSensor(AMSgvolume *mvol, int tkid, int sensor);

  /// Build hybrid geometry
  static void BuildHybrid(AMSgvolume *mvol, int tkid);

  /// Build ladder support geometry
  static void BuildSupport(AMSgvolume *mvol, int tkid);

  /// Build honeycomb support geometry (layer number begins from 1)
  static void BuildHoneycomb(AMSgvolume *mvol, int layer);
};

#endif
