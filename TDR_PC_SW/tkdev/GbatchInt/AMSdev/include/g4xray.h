//  $Id: g4xray.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __G4XRAY__
#define __G4XRAY__
#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsTable.hh"
#include "G4Step.hh"
// ######################################################################
// ###                           xray                                 ###
// ######################################################################



class G4XRay : public G4ParticleDefinition
{
 private:
   static G4XRay* theInstance;

 private: // hide constructor as private
   G4XRay(){}

 public:
   ~G4XRay(){}

   static G4XRay* Definition();
   static G4XRay* XRayDefinition();
   static G4XRay* XRay();
};





#include "G4VDiscreteProcess.hh"
#include "G4VContinuousProcess.hh"
class G4XRayTRDP : public G4VDiscreteProcess{
public:
G4XRayTRDP(G4String name):G4VDiscreteProcess(name){};
G4double GetMeanFreePath(const  G4Track & track, G4double step, G4ForceCondition *){return DBL_MAX;}
G4double PostStepGetPhysicalInteractionLength(const  G4Track & track, G4double step, G4ForceCondition *);
virtual G4bool IsApplicable(const G4ParticleDefinition& p){
return (p.GetPDGCharge() !=0 && !p.IsShortLived()) ;
}
G4VParticleChange* PostStepDoIt( const G4Track& track, const G4Step& stepData );
};
class G4XRayTRCP : public G4VContinuousProcess{
public:
G4XRayTRCP(G4String name):G4VContinuousProcess(name){};
virtual G4bool IsApplicable(const G4ParticleDefinition& p){
return (p.GetPDGCharge() !=0 && !p.IsShortLived()) ;
}
G4VParticleChange* AlongStepDoIt( const G4Track& track, const G4Step& stepData );
        G4double GetContinuousStepLimit(const G4Track& aTrack,
                                        G4double  ,
                                        G4double  ,
                                        G4double& ){return DBL_MAX;}

};
#endif
