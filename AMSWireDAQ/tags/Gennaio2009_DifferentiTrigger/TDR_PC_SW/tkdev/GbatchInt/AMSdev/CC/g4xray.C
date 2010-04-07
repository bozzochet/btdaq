
//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: g4xray.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// GEANT4 tag $Name:  $
//
// 

#include "G4ios.hh"
#include <fstream>
#include <iomanip>
 
#include "g4xray.h"
#include "geant4.h"
#include "mceventg.h"

#include "G4ParticleTable.hh"

// ######################################################################
// ###                            GAMMA                               ###
// ######################################################################
G4XRay* G4XRay::theInstance = 0;


G4XRay*  G4XRay::Definition()
{
  if (theInstance !=0) return theInstance;

  const G4String name = "xrayphoton";
  // search in particle table]
  G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* anInstance = pTable->FindParticle(name);
  if (anInstance ==0)
  {
  // create particle
  //
  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding
   anInstance = new G4ParticleDefinition(
                 name,         0.0*MeV,       0.0*MeV,         0.0,
                    2,              -1,            -1,
                    0,               0,             0,
              "xrayphoton",               0,             0,          22,
              true,                0.0,          NULL,
             false,           "xrayphoton",          22
              );
  }
  theInstance = reinterpret_cast<G4XRay*>(anInstance);
  return theInstance;
}

G4XRay*  G4XRay::XRayDefinition()
{
  return Definition();
}

G4XRay*  G4XRay::XRay()
{
  return Definition();
}







#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
extern "C" void gentrd_(float &, float &, float&, float &,int &, float[],float[]);
extern "C" void gentrdi_(float &, float &, float &,int &, float[],float[]);

G4VParticleChange* G4XRayTRCP::AlongStepDoIt( const G4Track& Track, const G4Step& Step ){
     pParticleChange->Initialize(Track);
     G4StepPoint * PrePoint = Step.GetPreStepPoint();
     G4VPhysicalVolume * PrePV = PrePoint->GetPhysicalVolume();
     if (PrePV ){
//    Check volumes
      if(PrePV->GetLogicalVolume()->GetSensitiveDetector() ==AMSG4DummySD::pSD(1)){

       G4ParticleDefinition* particle =Track.GetDefinition();
       geant gamma=Track.GetTotalEnergy()/particle->GetPDGMass();
//      Check Gamma
       if(gamma>100){
//       Run
         integer ntr=0;
         float etr[1000],str[1000];
         float vect[7];
         geant step=Step.GetStepLength()/cm;
         geant charge=fabs(particle->GetPDGCharge());
//       Set RNDM
         AMSmceventg::SaveSeeds(); 
         GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],1,"S");
//         Radiator
         gentrd_(charge,gamma,TRDMCFFKEY.cor,step,ntr,etr,str);
// Restore RNDM;
         GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
         AMSmceventg::RestoreSeeds();
//         cout << " invoked along "<<endl; 
         if (ntr){                  
//        Change Primary & Add Xrays
          int i;
          number ekin=aParticleChange.GetEnergy();
          for(i=0;i<ntr;i++)ekin=ekin-etr[i]*GeV;
          if(ekin<0){
           ekin=0;
           pParticleChange->ProposeTrackStatus(fStopButAlive);
          }
          aParticleChange.ProposeEnergy(ekin);
          pParticleChange->SetNumberOfSecondaries(ntr);
          pParticleChange->ProposeTrackStatus(fSuspend);          
          G4StepPoint * PostPoint = Step.GetPostStepPoint();
          G4ThreeVector dir(PostPoint->GetMomentumDirection());
          for(i=0;i<ntr;i++){
           G4DynamicParticle* ap =
           new G4DynamicParticle(G4XRay::XRay(),dir);
//           new G4DynamicParticle(G4Gamma::Gamma(),dir);
           ap->SetKineticEnergy(etr[i]*GeV);
           G4ThreeVector pos(PostPoint->GetPosition()-(str[i]*cm)*dir);
           G4double t=PrePoint->GetGlobalTime()+str[i]/step*
           (PostPoint->GetGlobalTime()-PrePoint->GetGlobalTime());
            G4Track* at =new G4Track(ap,t,pos);
            at->SetParentID(Track.GetTrackID());
            pParticleChange->AddSecondary(at);
          }
        }
      }
     }
    }
           return pParticleChange;
}

G4double G4XRayTRDP::PostStepGetPhysicalInteractionLength( const G4Track& Track,
                                                 G4double previousStepSize,
                                                 G4ForceCondition* condition ){
     *condition=Forced;
     return DBL_MAX;

}

G4VParticleChange* G4XRayTRDP::PostStepDoIt( const G4Track& Track, const G4Step& Step ){
     pParticleChange->Initialize(Track);
     G4StepPoint * PostPoint = Step.GetPostStepPoint();
     G4StepPoint * PrePoint = Step.GetPreStepPoint();
     G4VPhysicalVolume * PostPV = PostPoint->GetPhysicalVolume();
     G4VPhysicalVolume * PrePV = PrePoint->GetPhysicalVolume();
     if(PostPV && PrePV){
     if( PostPV->GetLogicalVolume()->GetSensitiveDetector() ==AMSG4DummySD::pSD(2) && (PostPV != PrePV)){
      G4ParticleDefinition* particle =Track.GetDefinition();
      geant gamma=Track.GetTotalEnergy()/particle->GetPDGMass();
//      Check Gamma
        if(gamma>100){
//       Run
         integer ntr=0;
         float etr[1000],str[1000];
         float vect[7];
         geant step=Step.GetStepLength()/cm;
         geant charge=fabs(particle->GetPDGCharge());
//       Set RNDM
         AMSmceventg::SaveSeeds(); 
         GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],1,"S");
         gentrdi_(charge,gamma,step,ntr,etr,str);
// Restore RNDM;
         GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
         AMSmceventg::RestoreSeeds();
//         cout << " invoked poststep "<<endl; 
         if (ntr){                  
//        Change Primary & Add Xrays
          int i;
          number ekin=aParticleChange.GetEnergy();
          for(i=0;i<ntr;i++)ekin=ekin-etr[i]*GeV;
          if(ekin<0){
           ekin=0;
           pParticleChange->ProposeTrackStatus(fStopButAlive);
          }
          aParticleChange.ProposeEnergy(ekin);
          pParticleChange->SetNumberOfSecondaries(ntr);
          G4StepPoint * PostPoint = Step.GetPostStepPoint();
          G4ThreeVector dir(PostPoint->GetMomentumDirection());
          for(i=0;i<ntr;i++){
           G4DynamicParticle* ap =
           new G4DynamicParticle(G4XRay::XRay(),dir);
           ap->SetKineticEnergy(etr[i]*GeV);
            G4Track* at =new G4Track(ap,PostPoint->GetGlobalTime(),PostPoint->GetPosition());
            at->SetParentID(Track.GetTrackID());
            pParticleChange->AddSecondary(at);
          }
          
         }
        }
      }
     }
        return pParticleChange;
}


