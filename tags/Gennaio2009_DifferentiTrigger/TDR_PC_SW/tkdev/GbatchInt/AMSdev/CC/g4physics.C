//  $Id: g4physics.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: g4physics.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// GEANT4 tag $Name:  $
//
// 


#include "g4physics.h"
#include "g4xray.h"
#include "cern.h"
#include "commons.h"
#include "amsstl.h"
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4IonTable.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include <iomanip.h>   
#include "G4UserSpecialCuts.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4EmStandardPhysics.hh"
#include "HadronPhysicsQGSP.hh"
#include "HadronPhysicsQGSC.hh"
#include "G4IonPhysics.hh"

AMSG4Physics::AMSG4Physics():  AMSNode(AMSID("AMSG4Physics",0)),G4VUserPhysicsList(),_pg3tog4(0),_pg4tog3(0),_Ng3tog4(0)
{
  // default cut value  (1.0mm) 
  defaultCutValue = 1.0*mm;

  SetVerboseLevel(AMSFFKEY.Debug);
  
}


AMSG4Physics::~AMSG4Physics(){
 if(_pg3tog4)delete [] _pg3tog4;
 if(_pg4tog3)delete [] _pg4tog3;
}


void AMSG4Physics::ConstructParticle()
{

  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  // create all particles
   ConstructAllBosons();
   ConstructAllLeptons();
   ConstructAllMesons();
   ConstructAllBarions();
   ConstructAllIons();
   ConstructAllShortLiveds();
   G4XRay::XRayDefinition();
   _init();
  
}

void AMSG4Physics::ConstructProcess()
{
  AddTransportation();
  theParticleIterator->reset();
  bool first=true;
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if(G4FFKEY.Geant3CutsOn)pmanager->AddDiscreteProcess(new AMSUserSpecialCuts());
    else if(first){
      cout <<"  AMSG4Physics-I-Geant3LikeCutsSwitchedOff"<<endl;
      first=false;
    }
  }

   if(G4FFKEY.PhysicsListUsed==0){
    cout<<"Old Physics List will be used. "<<endl;
   if(GCPHYS.ILOSS){
     ConstructEM();
    }
   if(GCPHYS.IHADR)ConstructHad();
   }
   else if(G4FFKEY.PhysicsListUsed==1){
    cout<<"QGSP Physics List will be used. "<<endl;
    
    if(GCPHYS.ILOSS){
     G4EmStandardPhysics*    pem=new G4EmStandardPhysics();
     pem->ConstructProcess();
    }
    if(GCPHYS.IHADR){
     HadronPhysicsQGSP* pqgsp=new HadronPhysicsQGSP();
     pqgsp->ConstructProcess();    
     G4IonPhysics *pion=new G4IonPhysics("ion");
     pion->ConstructProcess();
    }
   }
   else if(G4FFKEY.PhysicsListUsed==2){
    cout<<"QGSC Physics List will be used. "<<endl;
    
    if(GCPHYS.ILOSS){
     G4EmStandardPhysics*    pem=new G4EmStandardPhysics();
     pem->ConstructProcess();
    }
    if(GCPHYS.IHADR){
     HadronPhysicsQGSC* pqgsp=new HadronPhysicsQGSC();
     pqgsp->ConstructProcess();    
     G4IonPhysics *pion=new G4IonPhysics("ion");
     pion->ConstructProcess();
    }
   }
   else{
    cerr<<"Physics List no "<<G4FFKEY.PhysicsListUsed<<" Not Yet Supported"<<endl;
    abort();
   }
    ConstructGeneral();
  if(TRDMCFFKEY.mode>=0)ConstructXRay();
  if(GCTLIT.ITCKOV)ConstructOp();
}


#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4LowEnergyCompton.hh"
#include "G4LowEnergyGammaConversion.hh"
#include "G4LowEnergyPhotoElectric.hh"
#include "G4LowEnergyRayleigh.hh"
#include "G4LowEnergyIonisation.hh"
#include "G4LowEnergyBremsstrahlung.hh"

#include "G4MultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"
//#include "G4ionIonisation.hh"

void AMSG4Physics::ConstructEM()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    //cout <<"  particle "<<particleName<<" "<<G4FFKEY.LowEMagProcUsed<<endl; 
    if (particleName == "gamma") {
    // gamma
    if(!G4FFKEY.LowEMagProcUsed){
      // Construct processes for gamma
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());
      pmanager->AddDiscreteProcess(new G4ComptonScattering());      
      pmanager->AddDiscreteProcess(new G4GammaConversion());
    }
    else{
      pmanager->AddDiscreteProcess(new G4LowEnergyPhotoElectric() );
      pmanager->AddDiscreteProcess(new G4LowEnergyCompton());
      pmanager->AddDiscreteProcess(new G4LowEnergyRayleigh());
      pmanager->AddDiscreteProcess(new G4LowEnergyGammaConversion());
    }
    }
    else if (particleName == "e-") {
    //electron
    if(G4FFKEY.LowEMagProcUsed){
      G4MultipleScattering* aMultipleScattering = new G4MultipleScattering();
                pmanager->AddProcess(aMultipleScattering,     -1, 1, 1);
                pmanager->AddProcess(new G4LowEnergyIonisation(),-1,2,2);
                pmanager->AddProcess(new G4LowEnergyBremsstrahlung(),-1,-1,3); 
    }
    else{   

      // Construct processes for electron
      G4VProcess* theeminusMultipleScattering = new G4MultipleScattering();
      G4VProcess* theeminusIonisation = new G4eIonisation();
      G4VProcess* theeminusBremsstrahlung = new G4eBremsstrahlung();
      // add processes
      pmanager->AddProcess(theeminusMultipleScattering);
      pmanager->AddProcess(theeminusIonisation);
      pmanager->AddProcess(theeminusBremsstrahlung);      
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering, idxAlongStep,  1);
      pmanager->SetProcessOrdering(theeminusIonisation, idxAlongStep,  2);
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering, idxPostStep, 1);
      pmanager->SetProcessOrdering(theeminusIonisation, idxPostStep, 2);
      pmanager->SetProcessOrdering(theeminusBremsstrahlung, idxPostStep, 3);

    }

    } else if (particleName == "e+") {
    //positron
      // Construct processes for positron


    if(G4FFKEY.LowEMagProcUsed){

        G4MultipleScattering* aMultipleScattering = new G4MultipleScattering();
        pmanager->AddProcess(aMultipleScattering,     -1, 1, 1);
        pmanager->AddProcess(new G4eIonisation(),     -1, 2, 2);
        pmanager->AddProcess(new G4eBremsstrahlung(), -1,-1, 3);
        pmanager->AddProcess(new G4eplusAnnihilation(),0,-1, 4);


    }
    else{   

    
      G4VProcess* theeplusMultipleScattering = new G4MultipleScattering();
      G4VProcess* theeplusIonisation = new G4eIonisation();
      G4VProcess* theeplusBremsstrahlung = new G4eBremsstrahlung();
      G4VProcess* theeplusAnnihilation = new G4eplusAnnihilation();
      // add processes
      pmanager->AddProcess(theeplusMultipleScattering);
      pmanager->AddProcess(theeplusIonisation);
      pmanager->AddProcess(theeplusBremsstrahlung);
      pmanager->AddProcess(theeplusAnnihilation);
      // set ordering for AtRestDoIt
      pmanager->SetProcessOrderingToFirst(theeplusAnnihilation, idxAtRest);
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxAlongStep,  1);
      pmanager->SetProcessOrdering(theeplusIonisation, idxAlongStep,  2);
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxPostStep, 1);
      pmanager->SetProcessOrdering(theeplusIonisation, idxPostStep, 2);
      pmanager->SetProcessOrdering(theeplusBremsstrahlung, idxPostStep, 3);
      pmanager->SetProcessOrdering(theeplusAnnihilation, idxPostStep, 4);


    }

  
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
    //muon  
     // Construct processes for muon+
     G4VProcess* aMultipleScattering = new G4MultipleScattering();
     G4VProcess* aBremsstrahlung = new G4MuBremsstrahlung();
     G4VProcess* aPairProduction = new G4MuPairProduction();
     G4VProcess* anIonisation = new G4MuIonisation();
      // add processes
     pmanager->AddProcess(anIonisation);
     pmanager->AddProcess(aMultipleScattering);
     pmanager->AddProcess(aBremsstrahlung);
     pmanager->AddProcess(aPairProduction);
     // set ordering for AlongStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
     pmanager->SetProcessOrdering(anIonisation, idxAlongStep,  2);
     // set ordering for PostStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
     pmanager->SetProcessOrdering(anIonisation, idxPostStep, 2);
     pmanager->SetProcessOrdering(aBremsstrahlung, idxPostStep, 3);
     pmanager->SetProcessOrdering(aPairProduction, idxPostStep, 4);
     
    } else if( particleName == "GenericIon" ) {
//     G4VProcess* aionIonization = new G4ionIonisation;
     G4VProcess* aionIonization = new G4hIonisation;
     G4VProcess* aMultipleScattering = new G4MultipleScattering();
     pmanager->AddProcess(aionIonization);
     pmanager->AddProcess(aMultipleScattering);
     // set ordering for AlongStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
     pmanager->SetProcessOrdering(aionIonization, idxAlongStep,  2);
     // set ordering for PostStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
     pmanager->SetProcessOrdering(aionIonization, idxPostStep, 2);

   } else if ((!particle->IsShortLived()) &&
	      (particle->GetPDGCharge() != 0.0) && 
	      (particle->GetParticleName() != "chargedgeantino")) {
     // all others charged particles except geantino
     G4VProcess* aMultipleScattering = new G4MultipleScattering();
     G4VProcess* anIonisation = new G4hIonisation();
     // add processes
     pmanager->AddProcess(anIonisation);
     pmanager->AddProcess(aMultipleScattering);
     // set ordering for AlongStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
     pmanager->SetProcessOrdering(anIonisation, idxAlongStep,  2);
     // set ordering for PostStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
     pmanager->SetProcessOrdering(anIonisation, idxPostStep, 2);
    }
  }
}


// Hadron Processes

#include "G4HadronElasticProcess.hh"

#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4KaonPlusInelasticProcess.hh"
#include "G4KaonZeroSInelasticProcess.hh"
#include "G4KaonZeroLInelasticProcess.hh"
#include "G4KaonMinusInelasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4LambdaInelasticProcess.hh"
#include "G4AntiLambdaInelasticProcess.hh"
#include "G4SigmaPlusInelasticProcess.hh"
#include "G4SigmaMinusInelasticProcess.hh"
#include "G4AntiSigmaPlusInelasticProcess.hh"
#include "G4AntiSigmaMinusInelasticProcess.hh"
#include "G4XiZeroInelasticProcess.hh"
#include "G4XiMinusInelasticProcess.hh"
#include "G4AntiXiZeroInelasticProcess.hh"
#include "G4AntiXiMinusInelasticProcess.hh"
#include "G4DeuteronInelasticProcess.hh"
#include "G4TritonInelasticProcess.hh"
#include "G4AlphaInelasticProcess.hh"
#include "G4OmegaMinusInelasticProcess.hh"
#include "G4AntiOmegaMinusInelasticProcess.hh"

// Low-energy Models

#include "G4LElastic.hh"

#include "G4LEPionPlusInelastic.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4LEKaonPlusInelastic.hh"
#include "G4LEKaonZeroSInelastic.hh"
#include "G4LEKaonZeroLInelastic.hh"
#include "G4LEKaonMinusInelastic.hh"
#include "G4LEProtonInelastic.hh"
#include "G4LEAntiProtonInelastic.hh"
#include "G4LENeutronInelastic.hh"
#include "G4LEAntiNeutronInelastic.hh"
#include "G4LELambdaInelastic.hh"
#include "G4LEAntiLambdaInelastic.hh"
#include "G4LESigmaPlusInelastic.hh"
#include "G4LESigmaMinusInelastic.hh"
#include "G4LEAntiSigmaPlusInelastic.hh"
#include "G4LEAntiSigmaMinusInelastic.hh"
#include "G4LEXiZeroInelastic.hh"
#include "G4LEXiMinusInelastic.hh"
#include "G4LEAntiXiZeroInelastic.hh"
#include "G4LEAntiXiMinusInelastic.hh"
#include "G4LEDeuteronInelastic.hh"
#include "G4LETritonInelastic.hh"
#include "G4LEAlphaInelastic.hh"
#include "G4LEOmegaMinusInelastic.hh"
#include "G4LEAntiOmegaMinusInelastic.hh"

// High-energy Models

#include "G4HEPionPlusInelastic.hh"
#include "G4HEPionMinusInelastic.hh"
#include "G4HEKaonPlusInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonMinusInelastic.hh"
#include "G4HEProtonInelastic.hh"
#include "G4HEAntiProtonInelastic.hh"
#include "G4HENeutronInelastic.hh"
#include "G4HEAntiNeutronInelastic.hh"
#include "G4HELambdaInelastic.hh"
#include "G4HEAntiLambdaInelastic.hh"
#include "G4HESigmaPlusInelastic.hh"
#include "G4HESigmaMinusInelastic.hh"
#include "G4HEAntiSigmaPlusInelastic.hh"
#include "G4HEAntiSigmaMinusInelastic.hh"
#include "G4HEXiZeroInelastic.hh"
#include "G4HEXiMinusInelastic.hh"
#include "G4HEAntiXiZeroInelastic.hh"
#include "G4HEAntiXiMinusInelastic.hh"
#include "G4HEOmegaMinusInelastic.hh"
#include "G4HEAntiOmegaMinusInelastic.hh"

// Stopping processes

#ifdef TRIUMF_STOP_PIMINUS
#include "G4PionMinusAbsorptionAtRest.hh"
#else
#include "G4PiMinusAbsorptionAtRest.hh"
#endif
#ifdef TRIUMF_STOP_KMINUS
#include "G4KaonMinusAbsorption.hh"
#else
#include "G4KaonMinusAbsorptionAtRest.hh"
#endif

//
// ConstructHad()
//
// Makes discrete physics processes for the hadrons, at present limited
// to those particles with GHEISHA interactions (INTRC > 0).
// The processes are: Elastic scattering and Inelastic scattering.
//
// F.W.Jones  09-JUL-1998
//

void AMSG4Physics::ConstructHad()
{
   G4HadronElasticProcess* theElasticProcess = 
                                    new G4HadronElasticProcess;
   G4LElastic* theElasticModel = new G4LElastic;
   theElasticProcess->RegisterMe(theElasticModel);

   theParticleIterator->reset();
   while ((*theParticleIterator)()) {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
     
      if (particleName == "pi+") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4PionPlusInelasticProcess* theInelasticProcess = 
                                new G4PionPlusInelasticProcess("inelastic");
         G4LEPionPlusInelastic* theLEInelasticModel = 
                                new G4LEPionPlusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEPionPlusInelastic* theHEInelasticModel = 
                                new G4HEPionPlusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "pi-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4PionMinusInelasticProcess* theInelasticProcess = 
                                new G4PionMinusInelasticProcess("inelastic");
         G4LEPionMinusInelastic* theLEInelasticModel = 
                                new G4LEPionMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEPionMinusInelastic* theHEInelasticModel = 
                                new G4HEPionMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
#ifdef TRIUMF_STOP_PIMINUS
         pmanager->AddRestProcess(new G4PionMinusAbsorptionAtRest, ordDefault);
#else
         G4String prcNam;
         pmanager->AddRestProcess(
           new G4PiMinusAbsorptionAtRest(
                prcNam="PiMinusAbsorptionAtRest"), ordDefault);
#endif
      }
      else if (particleName == "kaon+") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4KaonPlusInelasticProcess* theInelasticProcess = 
                                  new G4KaonPlusInelasticProcess("inelastic");
         G4LEKaonPlusInelastic* theLEInelasticModel = 
                                  new G4LEKaonPlusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEKaonPlusInelastic* theHEInelasticModel = 
                                  new G4HEKaonPlusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "kaon0S") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4KaonZeroSInelasticProcess* theInelasticProcess = 
                             new G4KaonZeroSInelasticProcess("inelastic");
         G4LEKaonZeroSInelastic* theLEInelasticModel = 
                             new G4LEKaonZeroSInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEKaonZeroInelastic* theHEInelasticModel = 
                             new G4HEKaonZeroInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "kaon0L") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4KaonZeroLInelasticProcess* theInelasticProcess = 
                             new G4KaonZeroLInelasticProcess("inelastic");
         G4LEKaonZeroLInelastic* theLEInelasticModel = 
                             new G4LEKaonZeroLInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEKaonZeroInelastic* theHEInelasticModel = 
                             new G4HEKaonZeroInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "kaon-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4KaonMinusInelasticProcess* theInelasticProcess = 
                                 new G4KaonMinusInelasticProcess("inelastic");
         G4LEKaonMinusInelastic* theLEInelasticModel = 
                                 new G4LEKaonMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEKaonMinusInelastic* theHEInelasticModel = 
                                 new G4HEKaonMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
#ifdef TRIUMF_STOP_KMINUS
         pmanager->AddRestProcess(new G4KaonMinusAbsorption, ordDefault);
#else
         pmanager->AddRestProcess(new G4KaonMinusAbsorptionAtRest, ordDefault);
#endif
      }
      else if (particleName == "proton") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4ProtonInelasticProcess* theInelasticProcess = 
                                    new G4ProtonInelasticProcess("inelastic");
         G4LEProtonInelastic* theLEInelasticModel = new G4LEProtonInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEProtonInelastic* theHEInelasticModel = new G4HEProtonInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "anti_proton") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiProtonInelasticProcess* theInelasticProcess = 
                                new G4AntiProtonInelasticProcess("inelastic");
         G4LEAntiProtonInelastic* theLEInelasticModel = 
                                new G4LEAntiProtonInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiProtonInelastic* theHEInelasticModel = 
                                new G4HEAntiProtonInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "neutron") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4NeutronInelasticProcess* theInelasticProcess = 
                                    new G4NeutronInelasticProcess("inelastic");
         G4LENeutronInelastic* theLEInelasticModel = 
                                    new G4LENeutronInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HENeutronInelastic* theHEInelasticModel = 
                                    new G4HENeutronInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }  
      else if (particleName == "anti_neutron") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiNeutronInelasticProcess* theInelasticProcess = 
                               new G4AntiNeutronInelasticProcess("inelastic");
         G4LEAntiNeutronInelastic* theLEInelasticModel = 
                               new G4LEAntiNeutronInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiNeutronInelastic* theHEInelasticModel = 
                               new G4HEAntiNeutronInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "lambda") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4LambdaInelasticProcess* theInelasticProcess = 
                                    new G4LambdaInelasticProcess("inelastic");
         G4LELambdaInelastic* theLEInelasticModel = new G4LELambdaInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HELambdaInelastic* theHEInelasticModel = new G4HELambdaInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "anti_lambda") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiLambdaInelasticProcess* theInelasticProcess = 
                                new G4AntiLambdaInelasticProcess("inelastic");
         G4LEAntiLambdaInelastic* theLEInelasticModel = 
                                new G4LEAntiLambdaInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiLambdaInelastic* theHEInelasticModel = 
                                new G4HEAntiLambdaInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "sigma+") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4SigmaPlusInelasticProcess* theInelasticProcess = 
                                 new G4SigmaPlusInelasticProcess("inelastic");
         G4LESigmaPlusInelastic* theLEInelasticModel = 
                                 new G4LESigmaPlusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HESigmaPlusInelastic* theHEInelasticModel = 
                                 new G4HESigmaPlusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "sigma-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4SigmaMinusInelasticProcess* theInelasticProcess = 
                                 new G4SigmaMinusInelasticProcess("inelastic");
         G4LESigmaMinusInelastic* theLEInelasticModel = 
                                 new G4LESigmaMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HESigmaMinusInelastic* theHEInelasticModel = 
                                 new G4HESigmaMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "anti_sigma+") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiSigmaPlusInelasticProcess* theInelasticProcess = 
                             new G4AntiSigmaPlusInelasticProcess("inelastic");
         G4LEAntiSigmaPlusInelastic* theLEInelasticModel = 
                                 new G4LEAntiSigmaPlusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiSigmaPlusInelastic* theHEInelasticModel = 
                                 new G4HEAntiSigmaPlusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "anti_sigma-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiSigmaMinusInelasticProcess* theInelasticProcess = 
                            new G4AntiSigmaMinusInelasticProcess("inelastic");
         G4LEAntiSigmaMinusInelastic* theLEInelasticModel = 
                                 new G4LEAntiSigmaMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiSigmaMinusInelastic* theHEInelasticModel = 
                                 new G4HEAntiSigmaMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "xi0") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4XiZeroInelasticProcess* theInelasticProcess = 
                            new G4XiZeroInelasticProcess("inelastic");
         G4LEXiZeroInelastic* theLEInelasticModel = 
                                 new G4LEXiZeroInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEXiZeroInelastic* theHEInelasticModel = 
                                 new G4HEXiZeroInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "xi-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4XiMinusInelasticProcess* theInelasticProcess = 
                            new G4XiMinusInelasticProcess("inelastic");
         G4LEXiMinusInelastic* theLEInelasticModel = 
                                 new G4LEXiMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEXiMinusInelastic* theHEInelasticModel = 
                                 new G4HEXiMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "anti_xi0") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiXiZeroInelasticProcess* theInelasticProcess = 
                            new G4AntiXiZeroInelasticProcess("inelastic");
         G4LEAntiXiZeroInelastic* theLEInelasticModel = 
                                 new G4LEAntiXiZeroInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiXiZeroInelastic* theHEInelasticModel = 
                                 new G4HEAntiXiZeroInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "anti_xi-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiXiMinusInelasticProcess* theInelasticProcess = 
                            new G4AntiXiMinusInelasticProcess("inelastic");
         G4LEAntiXiMinusInelastic* theLEInelasticModel = 
                                 new G4LEAntiXiMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiXiMinusInelastic* theHEInelasticModel = 
                                 new G4HEAntiXiMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "deuteron") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4DeuteronInelasticProcess* theInelasticProcess = 
                            new G4DeuteronInelasticProcess("inelastic");
         G4LEDeuteronInelastic* theLEInelasticModel = 
                                 new G4LEDeuteronInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "triton") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4TritonInelasticProcess* theInelasticProcess = 
                            new G4TritonInelasticProcess("inelastic");
         G4LETritonInelastic* theLEInelasticModel = 
                                 new G4LETritonInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "alpha") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AlphaInelasticProcess* theInelasticProcess = 
                            new G4AlphaInelasticProcess("inelastic");
         G4LEAlphaInelastic* theLEInelasticModel = 
                                 new G4LEAlphaInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "omega-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4OmegaMinusInelasticProcess* theInelasticProcess = 
                            new G4OmegaMinusInelasticProcess("inelastic");
         G4LEOmegaMinusInelastic* theLEInelasticModel = 
                                 new G4LEOmegaMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEOmegaMinusInelastic* theHEInelasticModel = 
                                 new G4HEOmegaMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
      else if (particleName == "anti_omega-") {
         pmanager->AddDiscreteProcess(theElasticProcess);
         G4AntiOmegaMinusInelasticProcess* theInelasticProcess = 
                            new G4AntiOmegaMinusInelasticProcess("inelastic");
         G4LEAntiOmegaMinusInelastic* theLEInelasticModel = 
                                 new G4LEAntiOmegaMinusInelastic;
         theInelasticProcess->RegisterMe(theLEInelasticModel);
         G4HEAntiOmegaMinusInelastic* theHEInelasticModel = 
                                 new G4HEAntiOmegaMinusInelastic;
         theInelasticProcess->RegisterMe(theHEInelasticModel);
         pmanager->AddDiscreteProcess(theInelasticProcess);
      }
   }
}

#include "G4Cerenkov.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"

void AMSG4Physics::ConstructOp()
{
  G4cout << " Construction Optical Processes "<<endl;
  G4Cerenkov*   theCerenkovProcess = new G4Cerenkov("Cerenkov");
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
  G4OpRayleigh*   theRayleighScatteringProcess = new G4OpRayleigh();
  G4OpBoundaryProcess* theBoundaryProcess = new G4OpBoundaryProcess();

//  theCerenkovProcess->DumpPhysicsTable();
//  theAbsorptionProcess->DumpPhysicsTable();
//  theRayleighScatteringProcess->DumpPhysicsTable();
#ifdef __AMSDEBUG__
  theCerenkovProcess->SetVerboseLevel(AMSFFKEY.Debug);
  theAbsorptionProcess->SetVerboseLevel(AMSFFKEY.Debug);
  theRayleighScatteringProcess->SetVerboseLevel(AMSFFKEY.Debug);
  theBoundaryProcess->SetVerboseLevel(AMSFFKEY.Debug);
#endif
  G4int MaxNumPhotons = 3000;

  theCerenkovProcess->SetTrackSecondariesFirst(true);
  theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumPhotons);

  G4OpticalSurfaceModel themodel = unified;
  theBoundaryProcess->SetModel(themodel);

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (theCerenkovProcess->IsApplicable(*particle) && !particle->IsShortLived()) {
      pmanager->AddContinuousProcess(theCerenkovProcess);
    }
    if (particleName == "opticalphoton") {
      G4cout << " AddDiscreteProcess to OpticalPhoton " << endl;
      pmanager->AddDiscreteProcess(theAbsorptionProcess);
      pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
      pmanager->AddDiscreteProcess(theBoundaryProcess);
    }
  }
}

void AMSG4Physics::ConstructXRay()
{
  G4cout << " Construction TR Processes "<<endl;
  G4XRayTRDP*   pd = new G4XRayTRDP("XRayDiscrete");
  G4XRayTRCP*   pc = new G4XRayTRCP("XRayCont");

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
//    if (pd->IsApplicable(*particle))pmanager->AddDiscreteProcess(pd);
    if (pc->IsApplicable(*particle))pmanager->AddContinuousProcess(pc);
      if (particleName == "xrayphoton") {
      cout <<" Add Discrete Procs to xrayphoton "<<endl; 
      pmanager->AddDiscreteProcess(new G4LowEnergyPhotoElectric() );
      pmanager->AddDiscreteProcess(new G4LowEnergyCompton());
//      pmanager->AddDiscreteProcess(new G4LowEnergyRayleigh());
      pmanager->AddDiscreteProcess(new G4LowEnergyGammaConversion());
/*
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect() );
      pmanager->AddDiscreteProcess(new G4ComptonScattering());
      pmanager->AddDiscreteProcess(new G4GammaConversion());
*/
    } 
  }
}



#include "G4Decay.hh"
void AMSG4Physics::ConstructGeneral()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

void AMSG4Physics::SetCuts()
{
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
         SetCutsWithDefault();   

   G4double cut = defaultCutValue;
   cout <<"AMSG4Physics::SetCuts-I-DefaultCut "<<cut<<endl;

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cut/10., "gamma");
  SetCutValue(cut/10., "xrayphoton");
  SetCutValue(cut/10., "e-");
  SetCutValue(cut/10., "e+");

  // set cut values for proton and anti_proton before all other hadrons
  // because some processes for hadrons need cut values for proton/anti_proton
  SetCutValue(cut, "proton");
  SetCutValue(cut, "anti_proton");
 



#ifdef __AMSDEBUG__
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    cout <<particleName<<endl;
  }
#endif  
}

void AMSG4Physics::ConstructAllBosons()
{
  // Construct all bosons
  G4BosonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllLeptons()
{
  // Construct all leptons
  G4LeptonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllMesons()
{
  //  Construct all mesons
  G4MesonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllBarions()
{
  //  Construct all barions
  G4BaryonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllIons()
{
  //  Construct light ions
  G4IonConstructor pConstructor;
  pConstructor.ConstructParticle();  

  
}

void AMSG4Physics::ConstructAllShortLiveds()
{
  //  Construct  resonaces and quarks
  G4ShortLivedConstructor pConstructor;
  pConstructor.ConstructParticle();  
}





   const char* AMSG4Physics::G3toG4(G4int pid){

   int found=AMSbins(_pg3tog4,AMSIDi(pid),_Ng3tog4);
   if(found>0){
    return _pg3tog4[found-1].getname();
   }
   else {
     cerr<<"AMSG4Physics::G3toG4-F-NoG4ParticleFoundFor"<<pid<<endl;
     exit(1);
   }
 }

 G4int AMSG4Physics::G4toG3(const G4String & particle){
   AMSIDs ids((const char*)(particle));
   int found=AMSbins(_pg4tog3,ids,_Ng3tog4);
   if(found>0){
    return _pg4tog3[found-1].getid();
   }
   else if(GetVerboseLevel()){
     cerr<<"AMSG4Physics::G4toG3-I-NoG3ParticleFoundFor"<<(const char *)(particle)<<endl;
     return _G3DummyParticle;
   }

 }

 void AMSG4Physics::_init(){
integer g3part=0;
int ipart;
for(ipart=0;ipart<1000;ipart++){
    char chp[22]="";
    integer itrtyp=0;
    geant mass=0;
    geant charge=0;
    geant tlife=0;
    geant ub[1];
    integer nwb=0;
    GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
    if(itrtyp && ipart!=4)g3part++;
}
    cout <<"AMSG4Physics::_init-I-"<<g3part<<" Geant3 Particles Found"<<endl;
    integer *g3pid = new integer[g3part];
    geant *g3mass = new geant[g3part];
    geant *g3charge = new geant[g3part];
    G4ParticleDefinition** g3tog4p= new G4ParticleDefinition*[g3part];
    g3part=0;
for(ipart=0;ipart<1000;ipart++){
    char chp[22]="";
    integer itrtyp=0;
    geant mass=0;
    geant charge=0;
    geant tlife=0;
    geant ub[1];
    integer nwb=0;
    GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
    if(itrtyp && ipart!=4){
      g3pid[g3part]=ipart;
      g3mass[g3part]=mass;
      g3charge[g3part++]=charge;
    }
}


   G4ParticleTable *ppart=G4ParticleTable::GetParticleTable();
    const G4IonTable *pIonT= ppart->GetIonTable();
    for(ipart=0;ipart<g3part;ipart++){
     double fdelta=1000000;
     G4ParticleDefinition* cand=0;
     theParticleIterator->reset();
     while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      if(g3charge[ipart] == particle->GetPDGCharge() && particle->GetParticleName()!=G4String("GenericIon")){
        if(fabs(g3mass[ipart]*GeV-particle->GetPDGMass())<fdelta){
          fdelta=fabs(g3mass[ipart]*GeV-particle->GetPDGMass());
          cand=particle;
        }
      }
     }
     if(fdelta<0.01*g3mass[ipart]*GeV){
      if(g3pid[ipart]==13){
          g3tog4p[ipart]=ppart->FindParticle("neutron");
      }  
      else {
       g3tog4p[ipart]=cand;
     }
     }
     else if (fdelta==0){
        if(g3pid[ipart]==1){
          g3tog4p[ipart]=ppart->FindParticle("gamma");
        }
        else if(g3pid[ipart]==48){
          g3tog4p[ipart]=ppart->FindParticle("geantino");
        }
        else if(g3pid[ipart]==50){
          g3tog4p[ipart]=ppart->FindParticle("opticalphoton");
        }
        else{
          g3tog4p[ipart]=ppart->FindParticle("xrayphoton");
         } 
//       cout <<" b "<<g3pid[ipart]<<" "<<g3tog4p[ipart]->GetParticleName()<<endl;
     }
     else{
       G4int Z=abs(g3charge[ipart]);
       G4int A=g3mass[ipart]/.93;
       G4int J=0;
       G4int Q=g3charge[ipart];
       if(Q<0){
//         cout <<"  starting anti "<<endl;
       }
       G4ParticleDefinition* dummy=((G4IonTable *)pIonT)->GetIon(Z,A,J,Q);
       double fdelta=1000000;
       G4ParticleDefinition* cand=0;
       theParticleIterator->reset();
       while( (*theParticleIterator)() ){
        G4ParticleDefinition* particle = theParticleIterator->value();
        if(g3charge[ipart] == particle->GetPDGCharge()){
          if(fabs(g3mass[ipart]*GeV-particle->GetPDGMass())<fdelta){
            fdelta=fabs(g3mass[ipart]*GeV-particle->GetPDGMass());
            cand=particle;
          }
        }
       }
       if(fdelta<0.01*g3mass[ipart]*GeV){
        g3tog4p[ipart]=cand;
       // cout <<" c "<<g3pid[ipart]<<" "<<g3tog4p[ipart]->GetParticleName()<<endl;
       }
       else{
        cerr <<"AMSG4Physics-WG4IonNotFound "<<g3pid[ipart]<<" "<<fdelta<<" "<<g3mass[ipart]*GeV<<endl;
       }
     }
    }

//  NowBuildTable
      _Ng3tog4=0;
     for(ipart=0;ipart<g3part;ipart++){
      if(g3tog4p[ipart])_Ng3tog4++;
     }
      cout <<"AMSG4Physics::_init-I-"<<_Ng3tog4<<"G4ParticlesFoundOutOf"<<g3part<<endl;
     _pg3tog4= new AMSIDi[_Ng3tog4];
     _pg4tog3= new AMSIDs[_Ng3tog4];
      _Ng3tog4=0;
    for(ipart=0;ipart<g3part;ipart++){
     if(g3tog4p[ipart]){
      _pg3tog4[_Ng3tog4].setid(g3pid[ipart]);
      _pg4tog3[_Ng3tog4].setid(g3pid[ipart]);
      g3tog4p[ipart]->SetAntiPDGEncoding(g3pid[ipart]);
      _pg3tog4[_Ng3tog4].setname(g3tog4p[ipart]->GetParticleName());
      _pg4tog3[_Ng3tog4++].setname(g3tog4p[ipart]->GetParticleName());
     }
    }

     delete[] g3pid;
     delete[] g3mass;
     delete[] g3charge;
     delete[] g3tog4p;
     AMSsortNAGa(_pg3tog4, _Ng3tog4);
     AMSsortNAGa(_pg4tog3, _Ng3tog4);
#ifdef __AMSDEBUG__
  cout << "G4toG3 Conversion Table:"<<endl;
  for(int ipt=0;ipt<_Ng3tog4;ipt++)cout<<_pg4tog3[ipt];
#endif


}

    



/* 


integer g4part=0;

     theParticleIterator->reset();
   while( (*theParticleIterator)() ){
     G4ParticleDefinition* particle = theParticleIterator->value();
     G4String particleName = particle->GetParticleName();
     g4part++;
      cout <<particleName <<" "<<g4part<<" "<<particle->GetPDGCharge()<<" "<<particle->GetPDGMass()<<endl;


  // Construct antiions


   cout << " ent " <<  pIonT->Entries()<<endl;
   int idx;
   int nentries=pIonT->Entries();
  for (idx= 0; idx < nentries ; idx++) {
    const G4ParticleDefinition *pIon=pIonT->GetParticle(idx);
    G4String aname("a");
    aname+=pIon->GetParticleName();
    new G4VIon(aname,pIon->GetPDGMass(),pIon->GetPDGWidth(),
    -pIon->GetPDGCharge(),pIon->GetPDGiSpin(),pIon->GetPDGiParity(),
    pIon->GetPDGiConjugation(),pIon->GetPDGiIsospin(),pIon->GetPDGiIsospin3(),
    pIon->GetPDGiGParity(),pIon->GetParticleType(),pIon->GetLeptonNumber(),
    pIon->GetBaryonNumber(),pIon->GetPDGEncoding(),pIon->GetPDGStable(),
    pIon->GetPDGLifeTime(),0);
  }  

*/



G4int AMSG4Physics::_G3DummyParticle=556;



//     AMSUserSpecialCuts  Class
G4double AMSUserSpecialCuts::PostStepGetPhysicalInteractionLength(
                             const G4Track& aTrack,
                             G4double   previousStepSize,
                             G4ForceCondition* condition){
  // condition is set to "Not Forced"
  *condition = NotForced;

   G4double ProposedStep = DBL_MAX;
   AMSUserLimits* pUserLimits = (AMSUserLimits*)aTrack.GetVolume()->GetLogicalVolume()->GetUserLimits();
   if (pUserLimits){ 
       G4ParticleDefinition* ParticleDef = aTrack.GetDefinition();
       G4double Ekine    = aTrack.GetKineticEnergy();
//       const G4DynamicParticle* Particle = aTrack.GetDynamicParticle();
       //ekin & range limits
//        G4String particleName = ParticleDef->GetParticleName();
        G4String particleType = ParticleDef->GetParticleType();
        int g3=ParticleDef->GetAntiPDGEncoding();
//        cout <<particleName<<" "<<particleType<<" "<<ParticleDef->GetAntiPDGEncoding()<<" "<<ParticleDef->GetBaryonNumber();
        G4double Emin=0;
        G4double Charge=ParticleDef->GetPDGCharge();
//        if(particleName=="gamma"){
        if(g3==1){       
          Emin=pUserLimits->PhotonECut();
        }
//        else if (particleName=="e-" || particleName=="e+"){
        else if (g3==2 || g3==3){
          Emin=pUserLimits->ElectronECut();
        }         
//        else if (particleName=="mu-" || particleName=="mu+"){
        else if (g3==5 || g3==6){
          Emin=pUserLimits->MuonECut();
        }         
        else if ((g3>6 && g3<33) || particleType=="baryon" || particleType=="meson"){
          if(Charge != 0.){
           Emin=pUserLimits->HadronECut();
          }
          else{
           Emin=pUserLimits->HNeutralECut();
          }
        }         
        else if (particleType=="nucleus"){
           Emin=pUserLimits->HadronECut()*abs(ParticleDef->GetBaryonNumber());
        }
        else{
         Emin=pUserLimits->GetUserMinEkine(aTrack);
        }
        if(Ekine<Emin)return 0;
/*
        else if(Charge !=0){
           G4Material* Material = aTrack.GetMaterial();
           G4double RangeNow = G4EnergyLossTables::GetRange(ParticleDef,Ekine,Material);
           G4double RangeMin = G4EnergyLossTables::GetRange(ParticleDef,Emin, Material);
           if(pUserLimits->GetUserMinRange(aTrack)> RangeMin ){
               RangeMin=pUserLimits->GetUserMinRange(aTrack);    
               if(RangeNow<RangeMin)return 0;
           }
           if(ProposedStep > RangeNow-RangeMin) ProposedStep =RangeNow-RangeMin;
        }
*/
/*         
       //max time limit
       G4double beta = (Particle->GetTotalMomentum())/(aTrack.GetTotalEnergy());
       G4double dTime= (pUserLimits->GetUserMaxTime(aTrack) - aTrack.GetGlobalTime());
       G4double temp = beta*c_light*dTime;
       if (temp < 0.) return 0.;
       if (ProposedStep > temp) ProposedStep = temp;
*/
   }
   return ProposedStep;
}



G4VParticleChange* AMSUserSpecialCuts::PostStepDoIt(
                             const G4Track& aTrack,
                             const G4Step& step){
//
// Stop the current particle, if requested by G4UserLimits
//
#ifdef __AMSDEBUG__
       G4ParticleDefinition* ParticleDef = aTrack.GetDefinition();
       G4double Ekine    = aTrack.GetKineticEnergy();
 const G4DynamicParticle* Particle = aTrack.GetDynamicParticle();
        G4String particleName = ParticleDef->GetParticleName();
        G4String particleType = ParticleDef->GetParticleType();
        G4Material* Material = aTrack.GetMaterial();
        cout <<"stopped: "<< particleName<<" "<<ParticleDef->GetAntiPDGEncoding()<<" "<<  Ekine<<" "<<Particle->GetKineticEnergy()<<" at "<<Material->GetName()<<endl;
#endif
   aParticleChange.Initialize(aTrack);
   aParticleChange.ProposeEnergy(0.) ;
   aParticleChange.ProposeLocalEnergyDeposit (aTrack.GetKineticEnergy()) ;
   aParticleChange.ProposeTrackStatus(fStopAndKill);
   return &aParticleChange;
}
