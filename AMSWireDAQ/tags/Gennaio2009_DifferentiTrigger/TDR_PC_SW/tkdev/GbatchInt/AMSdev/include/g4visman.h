//  $Id: g4visman.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSG4VISMAN__
#define __AMSG4VISMAN__
#ifdef G4VIS_USE

#include "G4VisManager.hh"
 #include "G4UImanager.hh"
 #include "G4UIterminal.hh"


class AMSG4VisManager: public G4VisManager {

public:

  AMSG4VisManager (){};
  static void create();
  static void kill();
 private:
  static G4UIsession* _pses;
  static G4VisManager* _pman;

  void RegisterGraphicsSystems ();

};

#endif

#endif
