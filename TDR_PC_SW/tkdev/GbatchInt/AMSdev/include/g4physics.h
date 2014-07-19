//  $Id: g4physics.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __G4PHYSICS__
#define __G4PHYSICS__

#include "G4VUserPhysicsList.hh"
#include "globals.hh"
#include "node.h"
class AMSG4Physics: public AMSNode, public G4VUserPhysicsList
{
public:
  AMSG4Physics();
  virtual ~AMSG4Physics();
  public:
  const char* G3toG4(G4int pid);
  G4int G4toG3(const G4String&  particle);
  void _init();
  // Construct particle and physics
  virtual void ConstructParticle();
  virtual void ConstructProcess();
  
  // 
  virtual void SetCuts();
  
protected:
  static G4int _G3DummyParticle;
  AMSIDi* _pg3tog4;
  AMSIDs* _pg4tog3;
  integer _Ng3tog4; 
 // these methods Construct physics processes and register them
  virtual void ConstructGeneral();
  virtual void ConstructEM();
  virtual void ConstructHad();
  virtual void ConstructOp();
  void ConstructXRay();
  // these methods Construct all particles in each category
  virtual void ConstructAllBosons();
  virtual void ConstructAllLeptons();
  virtual void ConstructAllMesons();
  virtual void ConstructAllBarions();
  virtual void ConstructAllIons();
  virtual void ConstructAllShortLiveds();
};




//     AMSUserLimits  Class

#include "G4UserLimits.hh"

class AMSUserLimits: public G4UserLimits{

protected:

number _ElectronECut;
number _PhotonECut;
number _HadronECut;
number _HNeutralECut;
number _MuonECut;
public:
  AMSUserLimits(G4double ustepMax = DBL_MAX,
               G4double utrakMax = -1,
               G4double utimeMax = -1,
               G4double uekinMin = -1,
               G4double urangMin = -1 ): G4UserLimits(ustepMax,utrakMax,utimeMax,uekinMin,urangMin){};
number & ElectronECut(){return _ElectronECut;}
number & PhotonECut(){return _PhotonECut;}
number & HadronECut(){return _HadronECut;}
number & HNeutralECut(){return _HNeutralECut;}
number & MuonECut(){return _MuonECut;}

};


// AMSUserSpecialCuts Class
#include "G4UserSpecialCuts.hh"

class AMSUserSpecialCuts: public G4UserSpecialCuts{
public:
AMSUserSpecialCuts(const G4String& processName ="AMSSpecialCut" ):
G4UserSpecialCuts(processName){};
G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition
                            );
G4VParticleChange* PostStepDoIt(
                             const G4Track& aTrack,
                             const G4Step& step
                            );

};



#endif

