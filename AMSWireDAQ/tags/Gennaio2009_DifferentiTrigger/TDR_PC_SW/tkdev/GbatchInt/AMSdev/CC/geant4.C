//  $Id: geant4.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "job.h"
#include "event.h"
#include "trrec.h"
#include "richdbc.h"
#include "richid.h"
#include "mccluster.h"
#include "daqevt.h"
#include "mceventg.h"
#include "geant4.h"
#include "astring.h"
#include "g4physics.h"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4SimpleHeum.hh"
#include "G4ImplicitEuler.hh"
#include "G4ExplicitEuler.hh"
#include "G4ClassicalRK4.hh"
#include "G4TransportationManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Event.hh"
#include "G4PVPlacement.hh"
#include "G4StateManager.hh"
#include "G4ApplicationState.hh"
#include "g4xray.h"
#include "producer.h"
#ifdef G4VIS_USE
#include "g4visman.h"
#endif
 extern "C" void getfield_(geant& a);

void g4ams::G4INIT(){

// Initialize Random Number Generator


HepRandom::setTheEngine(new RanecuEngine());
long seed[3]={0,0,0};
seed[0]=GCFLAG.NRNDM[0];
seed[1]=GCFLAG.NRNDM[1];
HepRandom::setTheSeeds(seed);

 
G4RunManager * pmgr = new G4RunManager();




//   Detector

    pmgr->SetUserInitialization(new AMSG4DetectorInterface(AMSJob::gethead()->getgeom()->pg4v())); 
   AMSG4Physics * pph = new AMSG4Physics();
//   AMSJob::gethead()->addup(pph);
    AMSJob::gethead()->getg4physics()=pph;
    pmgr->SetUserInitialization(pph);


     pmgr->Initialize();





     AMSG4GeneratorInterface* ppg=new AMSG4GeneratorInterface(CCFFKEY.npat);
     AMSJob::gethead()->getg4generator()=ppg;
     pmgr->SetUserAction(ppg);
     pmgr->SetUserAction(new AMSG4EventAction);
     pmgr->SetUserAction(new AMSG4SteppingAction);
     pmgr->SetUserAction(new AMSG4StackingAction);
//    pmgr->SetUserAction(new AMSG4RunAction);


#ifdef G4VIS_USE
   AMSG4VisManager::create();
#endif

}
void g4ams::G4RUN(){
// Initialize GEANT3
    
    GCFLAG.IEVENT=1;
    TIMEL(GCTIME.TIMINT);
#ifndef G4VIS_USE
   G4RunManager::GetRunManager()->BeamOn(GCFLAG.NEVENT-GCFLAG.IEVENT+1);
#endif
}


void g4ams::G4LAST(){
        if(MISCFFKEY.G3On)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
        else {
          GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
          GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
        }
cout <<"           **** RANDOM NUMBER GENERATOR AFTER LAST COMPLETE EVENT "<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
float TIMLFT;
 TIMEL(TIMLFT);
 geant xmean  = (GCTIME.TIMINT - TIMLFT)/GCFLAG.IEVENT;
  cout <<"           **** NUMBER OF EVENTS PROCESSED = "<<GCFLAG.IEVENT<<endl;
  cout <<"           **** TIME TO PROCESS ONE EVENT IS = "<<xmean<<" SECONDS "<<endl;
delete  G4RunManager::GetRunManager();
#ifdef G4VIS_USE
AMSG4VisManager::kill();
#endif
}



void AMSG4MagneticField::GetFieldValue(const double x[3], double *B) const{
 int i;
   geant _v[3],_b[3];
 for(i=0;i<3;i++)_v[i]=x[i]/cm;
 GUFLD(_v,_b);
 for(i=0;i<3;i++)*(B+i)=_b[i]*kilogauss;

//if(B[0])cout <<x[0]<<" "<<x[1]<<" "<<x[2]<<" "<<B[0]<<endl;
}



#include "G4ParticleGun.hh"
AMSG4GeneratorInterface::AMSG4GeneratorInterface(G4int npart):_npart(npart),_cpart(0),AMSNode(AMSID("AMSG4GeneratorInterface",0)),G4VUserPrimaryGeneratorAction(){
  _particleGun = new G4ParticleGun[_npart];

}



void AMSG4GeneratorInterface::SetParticleGun(int ipart, number mom, AMSPoint  Pos, AMSPoint   Dir){
#ifdef __AMSDEBUG__
assert(_cpart < _npart);
#endif
 G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  const G4String pname(AMSJob::gethead()->getg4physics()->G3toG4(ipart));
_particleGun[_cpart].SetParticleDefinition(particleTable->FindParticle(pname));
_particleGun[_cpart].SetParticleEnergy(0);
_particleGun[_cpart].SetParticleMomentum(mom*GeV);
_particleGun[_cpart].SetParticleMomentumDirection(G4ThreeVector(Dir[0],Dir[1],Dir[2]));
_particleGun[_cpart].SetParticlePosition(G4ThreeVector(Pos[0]*cm,Pos[1]*cm,Pos[2]*cm));
     _cpart++;
  
}

void AMSG4GeneratorInterface::GeneratePrimaries(G4Event* anEvent){

static integer event=0;

AMSJob::gethead()->getg4generator()->Reset();


// create new event & initialize it
  if(AMSJob::gethead()->isSimulation()){
    AMSgObj::BookTimer.start("GEANTTRACKING");
   if(IOPA.mode%10 !=1 ){
    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
        if(MISCFFKEY.G3On)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
        else {
          GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
          GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
        }
        AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);
    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     genp->runG4(GCKINE.ikine);
    }
    }
   }
   else {
    AMSIO io;
    if(IOPA.mode/10?io.readA():io.read()){
     AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
     new AMSEvent(AMSID("Event",io.getevent()),io.getrun(),0,io.gettime(),io.getnsec(),
     io.getpolephi(),io.getstheta(),io.getsphi(),io.getveltheta(),
     io.getvelphi(),io.getrad(),io.getyaw(),io.getpitch(),io.getroll(),io.getangvel())));
     AMSmceventg* genp=new AMSmceventg(io);
     if(genp){
      AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
      genp->runG4();
      //genp->_printEl(cout);
     }
    }
    else{
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return;
    }   
   }



  for(int ipart=0;ipart<_cpart;ipart++){
//     cout <<_particleGun[ipart].GetParticleDefinition()->GetParticleName()<<endl;
     _particleGun[ipart].GeneratePrimaryVertex(anEvent);
  }

  }

}



AMSG4GeneratorInterface::~AMSG4GeneratorInterface(){
delete[] _particleGun;
}


void  AMSG4EventAction::BeginOfEventAction(const G4Event* anEvent){


 DAQEvent * pdaq=0;
 if(!AMSJob::gethead()->isSimulation()){
    //
    // read daq    
    //
    DAQEvent * pdaq=0;
    DAQEvent::InitResult res=DAQEvent::init();
  for(;;){
     if(res==DAQEvent::OK){ 
       pdaq = new DAQEvent();
       if(pdaq->read()){
      AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
      new AMSEvent(AMSID("Event",pdaq->eventno()),pdaq->runno(),
      pdaq->runtype(),pdaq->time(),pdaq->usec())));
//      pdaq->runtype(),tm,pdaq->usec()))); // tempor introduced to read PC-made files
//<------      
      AMSEvent::gethead()->addnext(AMSID("DAQEvent",pdaq->GetBlType()), pdaq);
   if(GCFLAG.IEORUN==2){
      // if production 
      // try to update the badrun list
         if(AMSJob::gethead()->isProduction() && AMSJob::gethead()->isRealData()){
           char fname[256];
           char * logdir = getenv("ProductionLogDirLocal");
           if(logdir){
            strcpy(fname,logdir);
           }
           else {
             cerr<<"gukine-E-NoProductionLogDirLocalFound"<<endl;
             strcpy(fname,"/Offline/local/logs");
           }
           strcat(fname,"/BadRunsList");
           ofstream ftxt;
           ftxt.open(fname,ios::app);
           if(ftxt){
            ftxt <<pdaq->runno()<<" "<<pdaq->eventno()<<endl;
            ftxt.close();
           }
           else{
            cerr<<"gukine-S-CouldNotOPenFile "<<fname<<endl;
            exit(1);
           }
           
         }
        pdaq->SetEOFIn();    
        GCFLAG.IEORUN=-2;
   }
   else if (GCFLAG.IEORUN==-2){
        GCFLAG.IEORUN=0;
      //  AMSJob::gethead()->uhend();
      //  AMSJob::gethead()->uhinit(pdaq->runno(),pdaq->eventno());
   }
      AMSG4EventAction::EndOfEventAction(anEvent);
      if(GCFLAG.IEOTRI || GCFLAG.IEVENT >= GCFLAG.NEVENT)break;
      GCFLAG.IEVENT++;
  }
  else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend();
     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo();
    }
    catch (AMSClient::Error a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     break;
#endif
    }
   }
   else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend();
     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo();
    }
    catch (AMSClient::Error a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     break;
#endif
}
}  
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return; 
 }   



}



void  AMSG4EventAction::EndOfEventAction(const G4Event* anEvent){
//   cout <<" guout in"<<endl;
   if(AMSJob::gethead()->isSimulation())
   AMSgObj::BookTimer.stop("GEANTTRACKING");

   try{
          if(anEvent && AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
   }
   catch (AMSuPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSaPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(0);
      return;
   }
   catch (amsglobalerror e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(0);
      return;
   }
      if(GCFLAG.IEVENT%abs(GCFLAG.ITEST)==0 ||     GCFLAG.IEORUN || GCFLAG.IEOTRI || 
         GCFLAG.IEVENT>=GCFLAG.NEVENT){
      AMSEvent::gethead()->printA(AMSEvent::debug);
        if(MISCFFKEY.G3On)GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
        else {
          GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
          GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
        }
       cout <<" RNDM "<<GCFLAG.NRNDM[0]<<" " <<GCFLAG.NRNDM[1]<<endl;
}
     integer trig;
     if(AMSJob::gethead()->gettriggerOr()){
      trig=0;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trig+=p->getnelem();
         else break;
        }
       }
     }
     else{
      trig=1;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        integer trigl=0;
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trigl+=p->getnelem();
         else break;
        }
        if(trigl==0)trig=0;
       }
     }
// try to manipulate the conditions for writing....
//   if(trig ){ 
//     AMSEvent::gethead()->copy();
//   }
     AMSEvent::gethead()->write(trig);

     UPool.Release(0);
   AMSEvent::gethead()->remove();
     UPool.Release(1);
   AMSEvent::sethead(0);
   UPool.erase(2000000);

   GCFLAG.IEVENT++;
   if(GCFLAG.IEVENT>=GCFLAG.NEVENT){
    GCFLAG.IEOTRI=1;
    GCFLAG.IEORUN=1;
   }
   if(GCFLAG.IEOTRI || GCFLAG.IEORUN)G4RunManager::GetRunManager()->AbortRun();
}




 G4VPhysicalVolume* AMSG4DetectorInterface::Construct(){

//Mag Field
    static AMSG4MagneticField * pf=0;
     G4FieldManager*  fieldMgr =G4TransportationManager::GetTransportationManager()->GetFieldManager();
    if(!pf && G4FFKEY.UniformMagField!=-1){
     AMSG4MagneticField * pf = new AMSG4MagneticField();
     fieldMgr->SetDetectorField(pf);
     G4double delta =G4FFKEY.Delta*cm;
     if(G4FFKEY.BFOrder){
      G4ChordFinder *pchord;
      G4Mag_EqRhs* fEquation = new G4Mag_UsualEqRhs(pf);
      const G4double stepMinimum = 1.e-4 * cm;
      if(G4FFKEY.BFOrder ==3){
        pchord=new G4ChordFinder(pf,stepMinimum,new G4SimpleHeum(fEquation));
      }
      else if(G4FFKEY.BFOrder ==2){
        pchord=new G4ChordFinder(pf,stepMinimum,new G4ImplicitEuler(fEquation));
      }
      else if(G4FFKEY.BFOrder ==1){
       pchord=new G4ChordFinder(pf,stepMinimum,new G4ExplicitEuler(fEquation));
      }
      else if(G4FFKEY.BFOrder==4){
       pchord=new G4ChordFinder(pf,stepMinimum,new G4ClassicalRK4(fEquation));
      }
      else {
       cerr<<"gams::G4INIT-F-No"<<G4FFKEY.BFOrder<<"OrderRunge-KuttaFound"<<endl;
       exit(1);
      }
      fieldMgr->SetChordFinder(pchord);
      cout <<"g4ams::G4INIT-I-"<<G4FFKEY.BFOrder<<"Order Runge-Kutta Selected "<<endl;
     }
     else{
      cout <<"g4ams::G4INIT-I-DefaultTrackingSelected "<<endl;
      fieldMgr->CreateChordFinder(pf);
     }
     fieldMgr->GetChordFinder()->SetDeltaChord(delta);
     cout << "chord "<<fieldMgr->GetChordFinder()->GetDeltaChord()<<endl;
 } 

if(!_pv){
  cout << "AMSG4DetectorInterface::Construct-I-Building Geometry "<<endl;
  AMSJob::gethead()->getgeom()->MakeG4Volumes();
// Attention as step volumes are linked to false_mother, not mother as other ones
 AString fnam(AMSDATADIR.amsdatadir);
 fnam+="amsstp_";
 fnam+=AMSJob::gethead()->getsetup();
 fnam+= AMSJob::gethead()->isRealData()?".1":".0";
// as usual doesn't work...
//  AMSJob::gethead()->getgeom()->ReadG4StepVolumes((char*)fnam);  
  cout << "AMSG4DetectorInterface::Construct-I-"<<AMSgvolume::getNpv()<<" Physical volumes, "<<AMSgvolume::getNlv()<<" logical volumes and "<<AMSgvolume::getNrm()<<" rotation matrixes have been created "<<endl;
 _pv=AMSJob::gethead()->getgeom()->pg4v();
}
 
 if(!_pv){
   cerr <<"g4ams::G4INIT()-W-DummyDetectorWillBeCreated "<<endl;
   double kfac=1;
   G4LogicalVolume* dummyl=
      new G4LogicalVolume(new G4Box("Box",AMSDBc::ams_size[0]/2*cm,AMSDBc::ams_size[1]/2*cm,AMSDBc::ams_size[2]/2*cm),
      new G4Material("Vacuum",1,g/mole,kfac*universe_mean_density,kStateGas,0.1*kelvin,1.e-19*pascal*kfac),"Vacuum_log",0,0,0);
  _pv=new G4PVPlacement(0,G4ThreeVector(),"AMS",
                        dummyl,0,false,1);

 }
  _pv->GetLogicalVolume()->SetFieldManager(fieldMgr,true);
 return _pv;



}

AMSG4RotationMatrix::AMSG4RotationMatrix(number nrm[3][3]):G4RotationMatrix(nrm[0][0],nrm[0][1],nrm[0][2],nrm[1][0],nrm[1][1],nrm[1][2],nrm[2][0],nrm[2][1],nrm[2][2]){
//AMSG4RotationMatrix::AMSG4RotationMatrix(number nrm[3][3]):G4RotationMatrix(nrm[0][0],nrm[1][0],nrm[2][0],nrm[0][1],nrm[1][1],nrm[2][1],nrm[0][2],nrm[1][2],nrm[2][2]){
#ifdef __AMSDEBUG__
 for (int i=0;i<3;i++){
  double norm=0;
  for(int j=0;j<3;j++){
   norm+=nrm[j][i]*nrm[j][i];
  }
  if(fabs(norm-1)>1.e-6){
   cerr <<"AMSG4RotationMatrix::AMSG4RotationMatrix-E-MatrixNotNormalized "<<i<<" "<<norm<<" "<<nrm[0][i]<<" "<<nrm[1][i]<<" "<<nrm[2][i]<<" "<<endl;
   exit(1);
  }
 }
#endif

}

void AMSG4RotationMatrix::Test(){
   AMSmceventg::SaveSeeds();   
   AMSPoint xp,yp,zp;
   float d;
   number nrm[3][3];
// Test against possible CLHEP changes in the future
   for (int i=0;i<10;i++){
    int j,k;
    for(j=0;j<3;j++){
      xp[j]=RNDM(d);
      yp[j]=RNDM(d);
      zp[j]=RNDM(d);
    }
    AMSDir x(xp);
    AMSDir y(yp);
    AMSDir z(zp);
     for(j=0;j<3;j++){
        nrm[j][0]=x[j];
        nrm[j][1]=y[j];
        nrm[j][2]=z[j];
     }
     AMSG4RotationMatrix mtest(nrm);
     geant r[3],sph,cth,cph,sth,theta[3],phi[3];
     integer rt=0;
     for (j=0;j<3;j++){
      for (k=0;k<3;k++) r[k]=nrm[k][j];
       GFANG(r, cth,  sth, cph, sph,  rt);
       theta[j]=atan2((double)sth,(double)cth);
       phi[j]=atan2((double)sph,(double)cph);
      }
     geant t1[3],p1[3];
     p1[0]=mtest.phiX(); 
     p1[1]=mtest.phiY(); 
     p1[2]=mtest.phiZ(); 
     t1[0]=mtest.thetaX(); 
     t1[1]=mtest.thetaY(); 
     t1[2]=mtest.thetaZ(); 
     AMSPoint P1(p1); 
     AMSPoint T1(t1); 
     AMSPoint Phi(phi); 
     AMSPoint Theta(theta); 
     if(P1.dist(Phi)>1.e-6 ||  T1.dist(Theta)>1.e-6){
      cerr<<"AMSG4RotationMatrix::Test-F-TestFailed at "<<i<<" "<<
      P1<< " " <<Phi <<" "<<T1<< " "<<Theta<<" "<<P1.dist(Phi)<<" "<<T1.dist(Theta)<<endl;
      exit(1);
     }
   }
#ifdef __AMSDEBUG__
     cout <<"AMSG4RotationMatrix::Test-I-TestPassed "<<endl;
#endif
   
   AMSmceventg::RestoreSeeds();   

}


AMSG4DummySD** AMSG4DummySD::_pSD=0;
integer AMSG4DummySDI::_Count=0;

AMSG4DummySDI::AMSG4DummySDI(){
if(!_Count++){
 AMSG4DummySD::_pSD=new AMSG4DummySD*[3];
 AMSG4DummySD::pSD()=new AMSG4DummySD();
 AMSG4DummySD::pSD(1)=new AMSG4DummySD("AMSG4TRDRadiator");
 AMSG4DummySD::pSD(2)=new AMSG4DummySD("AMSG4TRDGas");
}
}

AMSG4DummySDI::~AMSG4DummySDI(){
 if(--_Count==0){
  for(int i=0;i<3;i++)delete AMSG4DummySD::pSD(i);
  delete[] AMSG4DummySD::_pSD;
}
}
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

void AMSG4SteppingAction::UserSteppingAction(const G4Step * Step){

// just do as in example N04
// don't really understand the stuff
     
     GCTRAK.istop=0;

/* 
    Some stuff about step
G4StepPoint* GetPreStepPoint() const
void SetPreStepPoint(G4StepPoint* value)
G4StepPoint* GetPostStepPoint() const
void SetPostStepPoint(G4StepPoint* value)
G4double GetStepLength() const
void SetStepLength(G4double value)
G4Track* GetTrack() const
void SetTrack(G4Track* value)
G4ThreeVector GetDeltaPosition() const
G4double GetDeltaTime() const
G4ThreeVector GetDeltaMomentum() const
G4double GetDeltaEnergy() const
G4double GetTotalEnergyDeposit() const
void SetTotalEnergyDeposit(G4double value)
void AddTotalEnergyDeposit(G4double value)
void ResetTotalEnergyDeposit()
G4SteppingControl GetControlFlag() const
void SetControlFlag(G4SteppingControl StepControlFlag)

//
// TR Radiation Here
// 

*/
//       cout <<" stepping in"<<endl;
//       AMSmceventg::PrintSeeds(cout);
//       AMSmceventg::SaveSeeds();
//       if(GCFLAG.NRNDM[0]==2130120 && GCFLAG.NRNDM[1]==1154959891){
//        cout <<" jopa "<<endl;
//       }
//{
//       G4Track * Track = Step->GetTrack();
//       G4ParticleDefinition* particle =Track->GetDefinition();
//       cout << "particle "<<particle->GetParticleName()<<endl;
//}         
//     geant d;
     // Checking if Volume is sensitive one 
    G4StepPoint * PostPoint = Step->GetPostStepPoint();
    G4VPhysicalVolume * PostPV = PostPoint->GetPhysicalVolume();
     G4StepPoint * PrePoint = Step->GetPreStepPoint();
     G4VPhysicalVolume * PrePV = PrePoint->GetPhysicalVolume();
    if(PostPV && PrePV){
  //cout << "Stepping Pre "<<" "<<PrePV->GetName()<<" "<<PrePV->GetCopyNo()<<" "<<PrePoint->GetPosition()<<endl;
//      cout << "Stepping  Post"<<" "<<PostPV->GetName()<<" "<<PostPV->GetCopyNo()<<" "<<PostPoint->GetPosition()<<" "<<PostPoint->GetKineticEnergy()/GeV<<" "<<Step->GetStepLength()/cm<<endl;

    GCTMED.isvol=PostPV->GetLogicalVolume()->GetSensitiveDetector()!=0 ||
                 PrePV->GetLogicalVolume()->GetSensitiveDetector()!=0;
     GCTRAK.destep=Step->GetTotalEnergyDeposit()/GeV;
     
//   if(PrePoint->GetProcessDefinedStep())cout<<" b "<<PrePoint->GetProcessDefinedStep()->GetProcessName()<<endl;
//   if(PostPoint->GetProcessDefinedStep())cout<<"a "<<PostPoint->GetProcessDefinedStep()->GetProcessName()<<endl;
     G4Track * Track = Step->GetTrack();
   GCTRAK.nstep=Track->GetCurrentStepNumber()-1;
   GCKINE.itra=Track->GetParentID();
   //cout << " track id "<<GCKINE.itra<<" "<<GCTRAK.nstep<<endl;
   G4ParticleDefinition* particle =Track->GetDefinition();
   GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName());
   GCKINE.charge=particle->GetPDGCharge();
//    if(GCKINE.ipart==51){
//       cout <<" xray "<<PrePV->GetName()<<" "<<PostPV->GetName()<<" "<<PostPoint->GetPosition()<<endl;
//    }
    if(GCKINE.ipart==Cerenkov_photon){
//       cout <<" cerenkov "<<PrePV->GetName()<<" "<<PostPV->GetName()<<" "<<PostPoint->GetPosition()<<endl;
      if((PrePV->GetName())(0)=='R' && (PrePV->GetName())(1)=='I' &&
        (PrePV->GetName())(2)=='C' && (PrePV->GetName())(3)=='H'){
        if(PostPoint->GetProcessDefinedStep() && PostPoint->GetProcessDefinedStep()->GetProcessName() == "Boundary")RICHDB::numrefm++;
      }

      if(PrePV->GetName()(0)=='R' && PrePV->GetName()(1)=='A' &&
        PrePV->GetName()(2)=='D' && PrePV->GetName()(3)==' '){
        if(GCTRAK.nstep){
        if(PostPoint->GetProcessDefinedStep() && PostPoint->GetProcessDefinedStep()->GetProcessName() == "Rayleigh Scattering")RICHDB::numrayl++;
        else{
         RICHDB::numrayl=0;
	 RICHDB::numrefm=0;
//          RICHDB::nphgen++;  
         if(!RICHDB::detcer(GCTRAK.vect[6])) {
          }
           else RICHDB::nphgen++;
        }

        }
      }
    }

        

    if(GCTMED.isvol){// <========== we are in sensitive volume !!!
//
//      cout << "Stepping  sensitive"<<" "<<PrePV->GetName()<<" "<<PrePV->GetCopyNo()<<" "<<PrePoint->GetPosition()<<endl;
     // gothering some info and put it into geant3 commons

     GCTRAK.inwvol= PostPV != PrePV;
     GCTRAK.step=Step->GetStepLength()/cm;
     GCTRAK.vect[0]=PostPoint->GetPosition().x()/cm; 
     GCTRAK.vect[1]=PostPoint->GetPosition().y()/cm; 
     GCTRAK.vect[2]=PostPoint->GetPosition().z()/cm; 
     GCTRAK.vect[3]=PostPoint->GetMomentumDirection().x(); 
     GCTRAK.vect[4]=PostPoint->GetMomentumDirection().y(); 
     GCTRAK.vect[5]=PostPoint->GetMomentumDirection().z(); 
     GCTRAK.getot=PostPoint->GetTotalEnergy()/GeV;
     GCTRAK.gekin=PostPoint->GetKineticEnergy()/GeV;
     GCTRAK.vect[6]=GCTRAK.getot*PostPoint->GetBeta();
     GCTRAK.tofg=PostPoint->GetGlobalTime()/second;
     { int i;
       for(i=0;i<3;i++)GCKINE.vert[i]=Track->GetVertexPosition()[i]/cm; 
       for(i=0;i<3;i++)GCKINE.pvert[i]=Track->GetVertexMomentumDirection()[i]; 


/* 
     Some stuff about track
 
 
 const G4ThreeVector& GetPosition() const
 void SetPosition(const G4ThreeVector& aValue)
 G4double GetGlobalTime() const
 void SetGlobalTime(const G4double aValue)
 // Time since the event in which the track belongs is created.
 G4double GetLocalTime() const
 void SetLocalTime(const G4double aValue)
 // Time since the current track is created.
 G4double GetProperTime() const
 void SetProperTime(const G4double aValue)
 // Proper time of the current track
 G4double GetTrackLength() const
 void AddTrackLength(const G4double aValue)
 // Accumulated the track length
 G4int GetParentID() const
 void SetParentID(const G4int aValue)
 G4int GetTrackID() const
 void SetTrackID(const G4int aValue)
 G4VPhysicalVolume* GetVolume() const
 G4VPhysicalVolume* GetNextVolume() const
 G4Material* GetMaterial() const
 G4Material* GetNextMaterial() const
 G4VTouchable* GetTouchable() const
 void SetTouchable(G4VTouchable* apValue)
 G4VTouchable* GetNextTouchable() const
 void SetNextTouchable(G4VTouchable* apValue)
 G4double GetKineticEnergy() const
 void SetKineticEnergy(const G4double aValue)
 G4double GetVelocity() const
 const G4ThreeVector& GetMomentumDirection() const
 void SetMomentumDirection(const G4ThreeVector& aValue)
 const G4ThreeVector& GetPolarization() const
 void SetPolarization(const G4ThreeVector& aValue)
 G4TrackStatus GetTrackStatus() const
 void SetTrackStatus(const G4TrackStatus aTrackStatus)
 G4bool IsBelowThreshold() const
 void SetBelowThresholdFlag(G4bool value = true)
 G4bool IsGoodForTracking() const
 void SetGoodForTrackingFlag(G4bool value = true)
 G4int GetCurrentStepNumber() const
 void IncrementCurrentStepNumber()
 G4double GetTotalEnergy() const
 G4ThreeVector GetMomentum() const
 G4DynamicParticle* GetDynamicParticle() const
 G4ParticleDefinition* GetDefinition() const
 G4double GetStepLength() const
 void SetStepLength(G4double value)
 G4Step* GetStep() const
 void SetStep(G4Step* aValue)
 const G4ThreeVector& GetVertexPosition() const
 void SetVertexPosition(const G4ThreeVector& aValue)
 const G4ThreeVector& GetVertexMomentumDirection() const
 void SetVertexMomentumDirection(const G4ThreeVector& aValue)
 G4double GetVertexKineticEnergy() const
 void SetVertexKineticEnergy(const G4double aValue)
 G4LogicalVolume* GetLogicalVolumeAtVertex() const
 void SetLogicalVolumeAtVertex(G4LogicalVolume* )
 const G4VProcess* GetCreatorProcess() const
 void SetCreatorProcess(G4VProcess* aValue)
 G4double GetWeight() const
 void SetWeight(G4double aValue)



*/
   G4ParticleDefinition* particle =Track->GetDefinition();
   GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName());
   GCKINE.charge=particle->GetPDGCharge();
//      cout << "Stepping  sensitive"<<" "<<PostPV->GetName()<<" "<<PostPV->GetCopyNo()<<" "<<PostPoint->GetPosition()<<" "<<GCKINE.ipart<<" "<<GCTRAK.destep<<" "<<GCTRAK.step<<endl;

  try{
  // Now one has decide based on the names of volumes (or their parents)
     G4VPhysicalVolume * Mother=PrePV->GetMother();
     G4VPhysicalVolume * GrandMother= Mother?Mother->GetMother():0;
//-------------------------------------------------------------
// TRD
     if(GCTRAK.destep && PrePV->GetName()(0)=='T' && PrePV->GetName()(1)=='R' 
     &&  PrePV->GetName()(2)=='D' && PrePV->GetName()(3)=='T'){
       //cout <<" trd "<<GCKINE.itra<<" "<<GCKINE.ipart<<endl;
        AMSTRDMCCluster::sitrdhits(PrePV->GetCopyNo(),GCTRAK.vect,
        GCTRAK.destep,GCTRAK.gekin,GCTRAK.step,GCKINE.ipart,GCKINE.itra);   
     }



//------------------------------------------------------------      
//  Tracker
     if(GCTRAK.destep && GrandMother && GrandMother->GetName()(0)=='S' 
     &&  GrandMother->GetName()(1)=='T' && GrandMother->GetName()(2)=='K'){
//       cout <<" tracker "<<endl;
      AMSTrMCCluster::sitkhits(PrePV->GetCopyNo(),GCTRAK.vect,
      GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
     }
//------------------------------------------------------------
//    TOF: (imply here that Pre or Post volume is sensitive as defined by above check !!!)
//
     geant t,x,y,z;
     char name[5]="dumm";
     char media[21]="dummy_media         ";
     geant de,dee,dtr2,div,tof,prtq,pstep;
     static geant xpr(0.),ypr(0.),zpr(0.),tpr(0.);
     static geant stepsum(0.),estepsum(0.);
     static geant sscoo[3]={0.,0.,0.};
     static geant sstime(0.);
     static int nsmstps(0);
     geant trcut2(0.25);// Max. transv.shift (0.5cm)**2
     geant stepmin(0.25);//(cm) min. step/cumul.step to store hit(0.5cm/2)
     geant estepmin(1.e-5);//10kev
     geant coo[3],dx,dy,dz,dt;
     geant wvect[6],snext,safety;
     int i,nd,numv,iprt,numl,numvp,tfprf(0);
     static int numvo(-999),iprto(-999);
     integer tbegtof(0);
     integer tendtof(0);
     integer intof(0);
     if(PrePV->GetName()(0)== 'T' && PrePV->GetName()(1)=='F')tbegtof=1;
     if(PostPV->GetName()(0)== 'T' && PostPV->GetName()(1)=='F')tendtof=1;
     if(tbegtof==1 || tendtof==1)intof=1;
//
     if(tendtof==1 && GCTRAK.inwvol==1){// just enter TFnn : store/reset param.
       numv=PostPV->GetCopyNo();
       iprt=GCKINE.ipart;
       x=GCTRAK.vect[0];
       y=GCTRAK.vect[1];
       z=GCTRAK.vect[2];
       t=GCTRAK.tofg;
       iprto=iprt;
       numvo=numv;
       stepsum=0.;
       estepsum=0.;
       nsmstps=0;
       sstime=0;
       sscoo[0]=0;
       sscoo[1]=0;
       sscoo[2]=0;
       xpr=x;
       ypr=y;
       zpr=z;
       tpr=t;
     }// ---> endof "just enter TOF"
//
     if(tbegtof==1){// <====== step starts in TOF
       numv=PrePV->GetCopyNo();
       iprt=GCKINE.ipart;
       prtq=GCKINE.charge;
       pstep=GCTRAK.step;
       x=GCTRAK.vect[0];
       y=GCTRAK.vect[1];
       z=GCTRAK.vect[2];
       t=GCTRAK.tofg;
       de=GCTRAK.destep;
//
       if(iprt==iprto && numv==numvo && de!=0.){// <--- same part. in the same volume
         dx=(x-xpr);
         dy=(y-ypr);
         dz=(z-zpr);
         dt=(t-tpr);
         dtr2=dx*dx+dy*dy;
//----
         if(prtq!=0.){// <--- charged part.
//
           if(pstep>=stepmin){// <---- big step
//
             if(dtr2>trcut2){// <--- big transv. shift: subdivide step
               nd=integer(sqrt(dtr2/trcut2));
               nd+=1;
               dx=dx/geant(nd);
               dy=dy/geant(nd);
               dz=dz/geant(nd);
               dt=dt/geant(nd);
               GCTRAK.destep=de/geant(nd);
               for(i=1;i<=nd;i++){//loop over subdivisions
                 coo[0]=xpr+dx*(i-0.5);
                 coo[1]=ypr+dy*(i-0.5);
                 coo[2]=zpr+dz*(i-0.5);
                 tof=tpr+dt*(i-0.5);
                 dee=GCTRAK.destep;
                 //if(TOFMCFFKEY.birks)GBIRK(dee);
                 AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
               }
             }
             else{// <--- small transv. shift: take step as it is
               coo[0]=xpr+0.5*dx;
               coo[1]=ypr+0.5*dy;
               coo[2]=zpr+0.5*dz;
               tof=tpr+0.5*dt;
               dee=GCTRAK.destep;
               //if(TOFMCFFKEY.birks)GBIRK(dee);
               AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
             }// ---> endof "small transv.shift"
//
             if(estepsum>estepmin){// <--- After BigStep write previous "small steps" buffer if any
	       sscoo[0]/=geant(nsmstps);
	       sscoo[1]/=geant(nsmstps);
	       sscoo[2]/=geant(nsmstps);
	       sstime/=geant(nsmstps);
               //if(TFMCFFKEY.birks)GBIRK(estepsum);
               AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
	       stepsum=0.;
	       estepsum=0.;
               nsmstps=0;
               sstime=0;
               sscoo[0]=0;
               sscoo[1]=0;
               sscoo[2]=0;
	     }//---> endof "store prev. small steps"
//
           }// ---> endof "big step"
//
	   else{//      <--- small step
	     stepsum+=pstep;
	     estepsum+=GCTRAK.destep;
	     nsmstps+=1;
	     sscoo[0]+=x;
	     sscoo[1]+=y;
	     sscoo[2]+=z;
	     sstime+=t;
	     if(stepsum>=stepmin){// <--- "small steps" sum is already big: write it
	       sscoo[0]/=geant(nsmstps);
	       sscoo[1]/=geant(nsmstps);
	       sscoo[2]/=geant(nsmstps);
	       sstime/=geant(nsmstps);
               //if(TFMCFFKEY.birks)GBIRK(estepsum);
               AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
	       stepsum=0.;
	       estepsum=0.;
               nsmstps=0;
               sstime=0;
               sscoo[0]=0;
               sscoo[1]=0;
               sscoo[2]=0;
	     }
	   }//---> endof "small step" 
//
           if(GCTRAK.inwvol==1 && estepsum>estepmin){// leaving TOF: write "small steps" buffer if any
	     sscoo[0]/=geant(nsmstps);
	     sscoo[1]/=geant(nsmstps);
	     sscoo[2]/=geant(nsmstps);
	     sstime/=geant(nsmstps);
             //if(TFMCFFKEY.birks)GBIRK(estepsum);
             AMSTOFMCCluster::sitofhits(numv,sscoo,estepsum,sstime);
	   }//---> endof "leaving TOF"
//
         }// ---> endof "charged part"
//---- 
	 else{//<--- neutral part(use end-of-step coo)
           coo[0]=x;
           coo[1]=y;
           coo[2]=z;
           tof=t;
           dee=GCTRAK.destep;
           //if(TFMCFFKEY.birks)GBIRK(dee);
           AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
	 }// ---> endof "neutral part" 
//----
         xpr=x;
         ypr=y;
         zpr=z;
         tpr=t;
       }// ---> end of "same part/vol, de>0"
//
     }// ======> endof "step starts in TOF"
//------------------------------------------------------------------
//  ANTI :
//
     integer isphys,islog;
     if(PrePV->GetName()(0)== 'A' && PrePV->GetName()(1)=='N' &&
       PrePV->GetName()(2)=='T' && PrePV->GetName()(3)=='S' && GCTRAK.destep>0.){
       dee=GCTRAK.destep;
       isphys=PrePV->GetCopyNo();
       islog=floor(0.5*(isphys-1))+1;//not used now
       AMSAntiMCCluster::siantihits(isphys,GCTRAK.vect,
                                                   dee,GCTRAK.tofg);
     }// <--- end of "in ANTS"
//------------------------------------------------------------------
//  ECAL :
//
     if(PrePV->GetName()(0)== 'E' && PrePV->GetName()(1)=='C' && 
       PrePV->GetName()(2)=='F' && PrePV->GetName()(3)=='C'){
       if(GCTRAK.destep>0.){
       dee=GCTRAK.destep;
//cout<<"... in ECAL: numv="<<PrePV->GetCopyNo()<<" "<<dee<<" "<<PrePV->GetMother()->GetCopyNo()<<" "<<PrePV->GetName()<<" "<<GCTRAK.vect[0]<<" "<<GCTRAK.vect[1]<<" "<<GCTRAK.vect[2]<<" "<<PrePV->GetMother()->GetName()<<" "<<PrePV->GetMother()->GetLogicalVolume()<<endl;
       GBIRK(dee);
       AMSEcalMCHit::siecalhits(PrePV->GetMother()->GetCopyNo(),GCTRAK.vect,dee,GCTRAK.tofg);
       }
     }
//------------------------------------------------------------------
// CJM : RICH (preliminary and slow version)
//

     if(GCKINE.itra==1 && GCKINE.ipart!=Cerenkov_photon && GCTRAK.inwvol==1
	&& RICCONTROLFFKEY.iflgk_flag){  // To be checked
       
       if(PrePV->GetName()(0)=='R' && PrePV->GetName()(1)=='A' &&
	  PrePV->GetName()(2)=='D' && PrePV->GetName()(3)==' ')
	 AMSRichMCHit::sirichhits(GCKINE.ipart,
				  0,
				  GCTRAK.vect,
				  GCTRAK.vect,
				  GCTRAK.vect+3,
				  Status_primary_rad);
       else
	 
	 if(PrePV->GetName()(0)=='R' && PrePV->GetName()(1)=='A' &&
	    PrePV->GetName()(2)=='D' && PrePV->GetName()(3)=='B')
	   AMSRichMCHit::sirichhits(GCKINE.ipart,
				    0,
				    GCTRAK.vect,
				    GCTRAK.vect,
				    GCTRAK.vect+3,
				    Status_primary_radb);
       
	 else
	   
	   if(PrePV->GetName()(0)=='S' && PrePV->GetName()(1)=='T' &&
	      PrePV->GetName()(2)=='K' && PrePV->GetName()(3)==' ')
	     AMSRichMCHit::sirichhits(GCKINE.ipart,
				      0,
				      GCTRAK.vect,
				      GCTRAK.vect,
				      GCTRAK.vect+3,
				      Status_primary_tracker);
       
       
	   else
	     
	     if(PrePV->GetName()(0)=='T' && PrePV->GetName()(1)=='O' &&
		PrePV->GetName()(2)=='F' && PrePV->GetName()(3)=='H')
	       AMSRichMCHit::sirichhits(GCKINE.ipart,
					0,
					GCTRAK.vect,
					GCTRAK.vect,
					GCTRAK.vect+3,
					Status_primary_tof);
	       


     }


//    if(PrePV->GetName()(0)=='C' && PrePV->GetName()(1)=='A' &&
//       PrePV->GetName()(2)=='T' && PrePV->GetName()(3)=='O' && 
//       GCTRAK.inwvol==1){
    if(PostPV->GetName()(0)=='C' && PostPV->GetName()(1)=='A' &&
       PostPV->GetName()(2)=='T' && PostPV->GetName()(3)=='O' && 
       GCTRAK.inwvol==1){

      if(GCKINE.ipart==Cerenkov_photon){
        geant xl=(AMSRICHIdGeom::pmt_pos(1,2)-RICHDB::cato_pos()+RICHDB::RICradpos()-RICotherthk/2-
                   GCTRAK.vect[2])/GCTRAK.vect[5];
                 
        geant vect[3];
        vect[0]=GCTRAK.vect[0]+xl*GCTRAK.vect[3];
        vect[1]=GCTRAK.vect[1]+xl*GCTRAK.vect[4];
        vect[2]=GCTRAK.vect[2]+xl*GCTRAK.vect[5];
        //cout <<" get ipart "<<GCKINE.ipart <<" "<<GCTRAK.nstep<<" "<<GCTRAK.istop<<endl;
        if(!RICHDB::detcer(GCTRAK.vect[6])){
           GCTRAK.istop=1;
        }

        if( GCTRAK.nstep==0 && GCTRAK.istop==0){
         if(1 || RICHDB::detcer(GCTRAK.vect[6])) {
          GCTRAK.istop=2;
          AMSRichMCHit::sirichhits(GCKINE.ipart,
                                   PostPV->GetMother()->GetCopyNo()-1,
                                   //GCTRAK.vect,
                                   vect,
                                   GCKINE.vert,
                                   GCKINE.pvert,
                                   Status_Window-
                                   (GCKINE.itra!=1?100:0));
         }
       }
       else if(GCTRAK.istop==0 ){
	         GCTRAK.istop=2; // Absorb it
//	 if(GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::height)
         if(GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::rich_height-
           RICHDB::foil_height-RICradmirgap-RIClgdmirgap // in LG
           || (GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height &&
               GCKINE.vert[2]>RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height))
	   AMSRichMCHit::sirichhits(GCKINE.ipart,
				    PostPV->GetMother()->GetCopyNo()-1,
				    //GCTRAK.vect,
                                    vect,
				    GCKINE.vert,
				    GCKINE.pvert,
				    Status_LG_origin-
				    (GCKINE.itra!=1?100:0));	  
	 else
	   AMSRichMCHit::sirichhits(GCKINE.ipart,
				    PostPV->GetMother()->GetCopyNo()-1,
				    //GCTRAK.vect,
   				    vect,
				    GCKINE.vert,
				    GCKINE.pvert,
				    (GCKINE.itra!=1?100:0)+
				    RICHDB::numrefm*10+
				    (RICHDB::numrayl>0?Status_Rayleigh:0));
       }
      }
      else if(GCTRAK.nstep!=0){	 
        AMSRichMCHit::sirichhits(GCKINE.ipart,
				 PostPV->GetMother()->GetCopyNo()-1,
				 GCTRAK.vect,
				 GCKINE.vert,
				 GCKINE.pvert,
                                 Status_No_Cerenkov-
                                 (GCKINE.itra!=1?100:0));
      }				   
    }



// end of RICH
//----------------------------------------------------------------


  } // <--- end of "try" ---
//
   catch (AMSuPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
     AMSEvent::gethead()->Recovery();
     return;
   }
   catch (AMSaPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->Recovery();
      return;
   }
   }
//   cout <<"leaving stepping "<<endl;
  }
  if(GCTRAK.istop){
      Track->SetTrackStatus(fStopAndKill);
  }

}

}

G4ClassificationOfNewTrack AMSG4StackingAction::ClassifyNewTrack(const G4Track * aTrack)
{ 
   return fWaiting;
   G4ParticleDefinition* particle =aTrack->GetDefinition();
   GCKINE.ipart=AMSJob::gethead()->getg4physics()->G4toG3(particle->GetParticleName());
//  cerenkov photons should be properly stopped here (via detcer function)
    if(0 &&GCKINE.ipart==Cerenkov_photon){
     return fKill;
    }
    else if(GCKINE.ipart==51){
      return fWaiting;
    }
    else return fWaiting;
}


