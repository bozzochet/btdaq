//  $Id: geant4.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __GEANT4H__
#define __GEANT4H__
#include "cern.h"
#include "geantnamespace.h"
#include "point.h"
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"

#include "G4MagneticField.hh"
#include "G4VUserPhysicsList.hh"

#include "node.h"

class G4VPhysicalVolume;
class AMSG4DetectorInterface: public G4VUserDetectorConstruction{
protected:
G4VPhysicalVolume * _pv;
public:
 AMSG4DetectorInterface(G4VPhysicalVolume * pv=0):_pv(pv){}
 G4VPhysicalVolume* Construct();
};


class AMSG4MagneticField : public G4MagneticField{
  public:

    void GetFieldValue( const  G4double Point[3],
                               G4double *Bfield ) const;
};




#include "G4VUserPrimaryGeneratorAction.hh"
class G4ParticleGun;
class G4Event;

class AMSG4GeneratorInterface: public AMSNode, public G4VUserPrimaryGeneratorAction{

public:
    AMSG4GeneratorInterface(G4int npart=1);
    virtual ~AMSG4GeneratorInterface();
    void GeneratePrimaries(G4Event* anEvent);
    void SetParticleGun(int part, number mom, AMSPoint  Pos,  AMSPoint Dir);
    void Reset(){_cpart=0;}
private:
    G4int        _npart;
    G4int        _cpart;
    G4ParticleGun* _particleGun;
    void _init(){};

};


#include "G4UserEventAction.hh"
class AMSG4EventAction: public G4UserEventAction{

public:
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

};






class AMSG4RotationMatrix: public G4RotationMatrix{

public:
AMSG4RotationMatrix(number _nrm[3][3]);
AMSG4RotationMatrix():G4RotationMatrix(){};
static  void Test();
};



#include "G4UserSteppingAction.hh"

class AMSG4SteppingAction : public G4UserSteppingAction
{
  public:

    virtual void UserSteppingAction(const G4Step*);
};



#include "G4VSensitiveDetector.hh"
class G4Step;

class AMSG4DummySD : public G4VSensitiveDetector
{
protected:
static AMSG4DummySD** _pSD;
public:
  AMSG4DummySD(char * name=0):G4VSensitiveDetector(name==0?"AMSG4dummySD":name){};
  ~AMSG4DummySD() {};

  void Initialize(G4HCofThisEvent*HCE) {};
  G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist) {return false;}
  void EndOfEvent(G4HCofThisEvent*HCE) {};
  static AMSG4DummySD* & pSD(uinteger i=0){return _pSD[i];}
friend class AMSG4DummySDI;
};

class AMSG4DummySDI{
public:
AMSG4DummySDI();
~AMSG4DummySDI();
private:
 static integer _Count;
};
static AMSG4DummySDI AMSG4DummySDI;



#include "G4UserStackingAction.hh"

class AMSG4StackingAction: public G4UserStackingAction{

public:
 AMSG4StackingAction(){};
 virtual ~AMSG4StackingAction(){};
   virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage(){};
    virtual void PrepareNewEvent(){};

};
#endif

