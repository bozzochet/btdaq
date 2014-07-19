//  $Id: GbatchAMS.h,v 1.1 2008/01/21 21:58:33 haino Exp $
#ifndef __GbatchAMS__
#define __GbatchAMS__

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchAMS.h
///\brief Header file of GbatchAMS class
///
///\class GbatchAMS
///\brief A Gbatch interface to run whole AMS MC simuletion (Geant3)
///\ingroup gbint
///
/// To run Gbatch, you can do: \n
/// root [0] GbatchAMS gb        // Create an instance \n
/// root [1] gb.DataCard("...")  // Input datacards \n
/// root [2] gb.DataCard("...")  // Input datacards \n
/// ... \n
/// root [..] gb.Main()          // Run the simulation \n
///
/// Otherwise you can call gbatch.C which can take 
/// a job file as an argument to extract datacards
///
///\date  2007/11/22 SH  First test version
///\date  2007/11/26 SH  First stable version
///$Date: 2008/01/21 21:58:33 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchInterface.h"

class GbatchAMS : public GbatchInterface {

public:
  GbatchAMS(void);
  virtual ~GbatchAMS();

  /// Main routine to be called after setting datacards
  Int_t Main(void);

protected:
//-----------------------------------------------
// Virtual methods overridden
//-----------------------------------------------
  virtual void NewJob   (void);  ///< Crate a new AMSJob object
  virtual void Dcards   (void);  ///< Define datacards
  virtual void InitJob  (void);  ///< Set datacards and initialize
  virtual void Remap    (void);  ///< Remap links
  virtual void DelJob   (void);  ///< Delete the AMSJob object

  virtual void Particles(void);  ///< Define particles
  virtual void Physics  (void);  ///< Define physics interface
  virtual void Materials(void);  ///< Define materials and tracking mediums
  virtual void Geometry (void);  ///< Define geometry

  virtual void UserKine (void);  ///< User routine to create primaries
  virtual void UserStep (void);  ///< User routine alled at each step
  virtual void UserOut  (void);  ///< User routine called at the end of event

  ClassDef(GbatchAMS, 1)    // Gbatch interface class for whole AMS MC
};

#endif
