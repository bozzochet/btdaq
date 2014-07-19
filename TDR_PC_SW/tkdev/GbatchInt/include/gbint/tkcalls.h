// $Id: tkcalls.h,v 1.3 2008/04/02 11:35:12 haino Exp $
#ifndef __tkcalls__
#define __tkcalls__

//////////////////////////////////////////////////////////////////////////
///
///\file  tkcalls.h
///\brief Definition of Geant3 user calles for only tracker setup
///
///\date  2007/11/27 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/04/02 11:35:12 $
///
///$Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////////

class Event;

/// A namespace which includes functions for tracker simulation
namespace gbtrk {

  /// Enums to select reconstruction codes
  enum ERecCodes{ 
    GBATCH = 1,    ///< Original codes in Gbatch (trrec.C)
    TKREC  = 2,    ///< New codes (\ref tkrec)
    TKRAW  = 3     ///< New codes (\ref tkrec) only raw cluster
  };

  /// Switch to select reconstruction codes
  extern ERecCodes reccode;

  /// Debug switch
  extern int debug;

  /// Called at every event from GUKINE
  void tkkine(void);

  /// Called at every step  from GUSTEP
  void tkstep(void);

  /// Called at every event from GUOUT, digitization and reconstruction
  void tkout(void);

  /// called at every event from GUOUT, clears the current event object
  void tkclr(void);

  /// Pointer to Event
  extern Event *event;
}

#endif
