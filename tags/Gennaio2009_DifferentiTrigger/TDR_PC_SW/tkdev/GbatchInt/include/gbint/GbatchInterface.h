// $Id: GbatchInterface.h,v 1.1 2008/01/21 21:58:33 haino Exp $
#ifndef __GbatchInterface__
#define __GbatchInterface__

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchInterface.h
///\brief Header file of GbatchInterface
///
///\class GbatchInterface
///\brief A base class of Gbatch interface for ROOT CINT
///\ingroup gbint
///
/// Gbatch interface to enable us to call Gbatch routines from ROOT CINT.
///
/// You can make your own class inheriting GbathInterface 
/// and override some methods as GbatchAMS and GbatchTRK.
///
///\date  2007/11/21 SH  First test version
///\date  2007/11/26 SH  First stable version
///$Date: 2008/01/21 21:58:33 $ 
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class AMSgvolume;

class GbatchInterface : public TObject {

public:
  /// Enums to indicate the status of the simulation
  enum EStatus{ NOT_INITIALIZED, GET_DATACARD, READY_TO_INIT, 
		READY_TO_RUN, RUN_FINISHED, TERMINATED };

  enum EGvolOptions{ GVOL_ONLY = 1, GVOL_POSP = 2, GVOL_REL = 4,
		     GVOL_NROT = 8 };

private:
  /// Status of the simulation
  EStatus _status;

  /// Self static pointer
  static GbatchInterface* _ptr;

protected:
  AMSgvolume* _mother;     //!< Pointer to mother volume

  Int_t   _verbose;        //!< Verbose level

  Int_t   _particle;       //!< Geant3 particle code for the primary
  Float_t _momentum;       //!< Primary momentum in GeV/c
  Float_t _vertex   [3];   //!< Primary vertex position in cm
  Float_t _direction[3];   //!< Primary momentum direction in unit vector

public:
  GbatchInterface(void);
  virtual ~GbatchInterface();

  /// Get status code
  EStatus GetStatus(void) const { return _status; }

  /// Set verbose level
  void SetVerbose  (Int_t   v) { _verbose  = v; }
  /// Set Geant3 partcile code for the primary
  void SetPart     (Int_t   p) { _particle = p; }
  /// Set primary momentum in GeV/c
  void SetMomentum (Float_t p) { _momentum = p; }
  /// Set primary vertex position in cm
  void SetVertex   (Float_t x, Float_t y, Float_t z);
  /// Set momentum direction, automatically converted to unit vector
  void SetDirection(Float_t x, Float_t y, Float_t z);

  /// Set datacards
  Int_t DataCard(const char *str);

//-----------------------------------------------
// Methods usually not to be overridden
//-----------------------------------------------
  virtual void InitFirst (void);   ///< Initialization before FFGO
  virtual void InitSecond(void);   ///< Initialization after FFGO  
  virtual void StartRun  (void);   ///< Start the simulation
  virtual void Terminate (void);   ///< Process after the simulation

protected:
  Bool_t CheckStatus (EStatus status, const char *name);
  void   ChangeStatus(EStatus status) { _status = status; }

//-----------------------------------------------
// Virtual methods to be implemented by user
//-----------------------------------------------
  virtual void NewJob (void) {}    ///< Crate a new AMSJob object
  virtual void Dcards (void) {}    ///< Define datacards
  virtual void InitJob(void) {}    ///< Set datacards and initialize
  virtual void Remap  (void) {}    ///< Remap links
  virtual void DelJob (void) {}    ///< Delete the AMSJob object

  virtual void Particles(void);    ///< Define particles
  virtual void Physics  (void);    ///< Define physics interface
  virtual void Materials(void) {}  ///< Define materials and tracking mediums
  virtual void Geometry (void) {}  ///< Define geometry

  virtual void UserKine(void);     ///< User routine to create primaries
  virtual void UserStep(void);     ///< User routine called at each step
  virtual void UserOut (void);     ///< User routine called at the end of event

//-----------------------------------------------
// Static methods called by Geant3
//-----------------------------------------------
public:
  static void Gukine(void);        ///< Calls UserKine
  static void Gustep(void);        ///< Calls UserStep
  static void Guout (void);        ///< Calls UserOut

  ClassDef(GbatchInterface, 1)    // Gbatch interface class
};

#endif
