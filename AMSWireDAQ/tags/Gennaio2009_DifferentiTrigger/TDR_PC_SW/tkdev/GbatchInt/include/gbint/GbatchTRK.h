//  $Id: GbatchTRK.h,v 1.4 2008/04/02 11:34:41 haino Exp $
#ifndef __GbatchTRK__
#define __GbatchTRK__

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchTRK.h
///\brief Header file of GbatchTRK class
///
///\class GbatchTRK
///\brief A Gbatch interface to run with only tracker geometry
///\ingroup gbint
///
/// To run GbatchTRK, you can do: \n
/// root [0] GbatchTRK gb        // Create an instance \n
/// root [1] gb.DataCard("...")  // Input datacards \n
/// root [2] gb.DataCard("...")  // Input datacards \n
/// ... \n
/// root [..] gb.Main()          // Run the simulation \n
///
/// Otherwise you can call gbtest.C 
/// for more details please see gbtest.C
///
/// Track reconstruction code can be changed with 
/// SetRecCodes(Int_t r), where r=1 (Gbatch) or 2 (\ref tkrec)
///
///\date  2007/11/24 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/04/02 11:34:41 $
///
///$Revision: 1.4 $
///
///\class GbatchTest
///\brief A class inherited from GbatchTRK to be defined by user
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchInterface.h"

class Event;

class GbatchTRK : public GbatchInterface {

protected:
  Int_t _nevent;         ///< Number of events processed

public:
  GbatchTRK(void);
  virtual ~GbatchTRK();

  /// Main routine to be called after setting datacards
  Int_t Main(void);

  /// Get reconstruction codes, 1: Gbatch (original) or 2: \ref tkrec (new)
  static Int_t GetRecCodes(void);

  /// Set reconstruction codes, 1: Gbatch (original) or 2: \ref tkrec (new)
  static void  SetRecCodes(Int_t);

  /// Get debug message level of \ref tkrec
  static Int_t GetRecDebug(void);

  /// Set debug message level of \ref tkrec
  static void  SetRecDebug(Int_t);

  /// Initialize magnetic field map (in AMS format)
  static Int_t InitBfield(const char *fname);

  /// Disable ladders
  static Int_t DisableLadders(const char *fname);

  /// Get pointer to Event
  static Event *GetEvent(void);

  /// Set pointer to Event
  static void SetEvent(Event *event);

protected:
//-----------------------------------------------
// Virtual methods to be implemented if needed
//-----------------------------------------------
  virtual void PreInit  (void){} ///< Called before the init
  virtual void PreRun   (void){} ///< Called before the run
  virtual void PostRun  (void){} ///< Called after the run
  virtual void PreEvent (void){} ///< Called before eacn event
  virtual void PostEvent(void){} ///< Called after eacn event

//-----------------------------------------------
// Virtual methods overridden
//-----------------------------------------------
  virtual void NewJob   (void);  ///< Crate a new AMSJob object
  virtual void Dcards   (void);  ///< Define datacards
  virtual void InitJob  (void);  ///< Set datacards and initialize
  virtual void Remap    (void);  ///< Remap links
  virtual void DelJob   (void);  ///< Delete the AMSJob object

  virtual void Materials(void);  ///< Define materials and tracking mediums
  virtual void Geometry (void);  ///< Define geometry

  virtual void UserKine (void);  ///< User routine to create primaries
  virtual void UserStep (void);  ///< User routine alled at each step
  virtual void UserOut  (void);  ///< User routine called at the end of event

  ClassDef(GbatchTRK, 1)  // Gbatch interface class for only tracker geometry
};

#endif
