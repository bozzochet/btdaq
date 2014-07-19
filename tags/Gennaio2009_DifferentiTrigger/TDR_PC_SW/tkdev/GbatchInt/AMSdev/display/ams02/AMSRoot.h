//  $Id: AMSRoot.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSRoot_H
#define AMSRoot_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot                                                              //
//                                                                      //
// Main class to control the AMSRoot program.                           //
//                                                                      //
// This class :                                                         //
//   - Initialises the run default parameters                           //
//   - Provides API to Set/Get run parameters                           //
//   - Creates the support lists (TClonesArrays) for the Event structure//
//   - Creates the physics objects makers                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TTree
#include <TTree.h>
#endif
#ifndef AMSHistBrowser_H
#include "AMSHistBrowser.h"
#endif
#ifndef AMSMaker_H
#include "AMSMaker.h"
#endif
#include <time.h>

class TBrowser;
//class AMSMCMaker;
class AMSToFClusterReader;
class AMSTrackReader;
class AMSSiHitReader;
class AMSAntiClusterReader;
class AMSTrMCClusterReader;
class AMSParticleReader;
class AMSMCParticleReader;
//class ATLFElectronMaker;
//class ATLFMuonMaker;
//class ATLFPhotonMaker;
//class ATLFJetMaker;
//class AMSTriggerMaker;
//class AMSMiscMaker;
class AMSVirtualDisplay;

class AMSRoot : public TNamed {

private:
   Int_t                m_Version;           //AMSRoot version number
   Int_t                m_VersionDate;       //AMSRoot version date
   Int_t                m_RunNum;            //Run number
   Int_t                m_EventNum;          //Event number
   Int_t                m_Mode;              //Run mode
   Int_t		m_Event;             //Event counter in the input tree
   time_t                m_time[2];           //Time
   TTree               *m_Tree;              //Pointer to the Root tree
   TTree               *m_Input;             //Pointer to the input data tree
   TList               *m_Makers;            //List of Makers

//    pointers to standard Makers
   AMSToFClusterReader *m_ToFClusterMaker;   //Pointer to ToFClusterMaker
   AMSTrackReader      *m_TrackMaker;        //Pointer to TrackMaker
   AMSSiHitReader      *m_SiHitMaker;        //Pointer to SiHitMaker
   AMSAntiClusterReader *m_AntiClusterMaker;   //Pointer to AntiClusterMaker
   AMSTrMCClusterReader *m_TrMCClusterMaker;   //Pointer to AntiClusterMaker
   AMSParticleReader   *m_ParticleMaker;     //Pointer to ParticleMaker
   AMSMCParticleReader   *m_MCParticleMaker;     //Pointer to ParticleMaker

// AMSMCMaker          *m_MCMaker;           //Pointer to MCMaker
// AMSSiClusterMaker   *m_SiClusterMaker;    //Pointer to SiClusterMaker
// AMSACCClusterMaker  *m_ACCClusterMaker;   //Pointer to ACCClusterMaker
// ATLFElectronMaker   *m_ElectronMaker;     //Pointer to ElectronMaker
// ATLFMuonMaker       *m_MuonMaker;         //Pointer to MuonMaker
// ATLFPhotonMaker     *m_PhotonMaker;       //Pointer to PhotonMaker
// ATLFJetMaker        *m_JetMaker;          //Pointer to JetMaker
// AMSTriggerMaker     *m_TriggerMaker;      //Pointer to TriggerMaker
// AMSMiscMaker        *m_MiscMaker;         //Pointer to MiscMaker
//    flags and switches
// Bool_t               m_Bfield;            //B-field (on=1,off=0)
// Int_t                m_TrackFinding;      //Track/finding on=1,off=0

   AMSHistBrowser       m_HistBrowser;       //Object to Browse Maker histograms
   AMSVirtualDisplay   *m_Display;         //!Pointer to Event display object

   void                 CommonConstruct();   //Common part of all constructors
   Int_t                SelectedEvent;
public:
                      AMSRoot();
                      AMSRoot(const char *name, const char *title="The AMS Display with Root");
   Int_t              SetSelectedEvent(const char * event);
   Int_t             IsGolden();
   virtual           ~AMSRoot();
   virtual void       Browse(TBrowser *b);
   virtual Int_t       GetEvent(Int_t event=1);      // *MENU*
   virtual void       SelectEvent();      // *MENU*
   virtual void       Init(TTree * h1=0);           //Initialize to read from h1
   virtual void       Finish();                     //Finish a run
   AMSVirtualDisplay *Display() {return m_Display;}
   virtual void       Draw(Option_t *option="");    // *MENU*
   virtual void       Paint(Option_t *option="");
   virtual void       Clear(Option_t *option="");
   Bool_t             IsFolder() {return kTRUE;}
   virtual void       PrintInfo();

//
//    Getters
//
   Int_t          GetVersion()     {return m_Version;}
   Int_t          GetVersionDate() {return m_VersionDate;}
   Int_t          RunNum()         {return m_RunNum;}
   char*          GetTime()         {return ctime(m_time);}
   Int_t          EventNum()       {return m_EventNum;}
   Int_t          Event()          {return m_Event;}
   Int_t          Mode()           {return m_Mode;}
   TTree         *Tree()           {return m_Tree;}
   TTree         *GetInput()       {return m_Input;}
   TList         *Makers()         {return m_Makers;}
   AMSMaker          *Maker(const char *name) {return (AMSMaker*)m_Makers->FindObject(name);}
   AMSToFClusterReader *ToFClusterMaker() {return m_ToFClusterMaker;}
   AMSTrackReader      *TrackMaker()      {return m_TrackMaker;}
   AMSSiHitReader      *SiHitMaker()      {return m_SiHitMaker;}
   AMSAntiClusterReader *AntiClusterMaker() {return m_AntiClusterMaker;}
   AMSTrMCClusterReader *TrMCClusterMaker() {return m_TrMCClusterMaker;}
   AMSParticleReader   *ParticleMaker()   {return m_ParticleMaker;}
   AMSMCParticleReader   *MCParticleMaker()   {return m_MCParticleMaker;}
// AMSMCMaker         *MCMaker()         {return m_MCMaker;}
// AMSSiClusterMaker  *SiClusterMaker()  {return m_SiClusterMaker;}
// AMSACCClusterMaker *ACCClusterMaker() {return m_ACCClusterMaker;}
// ATLFElectronMaker *ElectronMaker() {return m_ElectronMaker;}
// ATLFMuonMaker     *MuonMaker()     {return m_MuonMaker;}
// ATLFPhotonMaker   *PhotonMaker()   {return m_PhotonMaker;}
// ATLFJetMaker      *JetMaker()      {return m_JetMaker;}
//   AMSTrackMaker     *TrackMaker()    {return m_TrackMaker;}
//   AMSTriggerMaker   *TriggerMaker()  {return m_TriggerMaker;}
//   AMSMiscMaker      *MiscMaker()     {return m_MiscMaker;}

//   Bool_t            Bfield()       {return m_Bfield;}
//   Bool_t            TrackFinding() {return m_TrackFinding;}

//
//    Setters
//
   virtual void   SetDefaultParameters();
// virtual void   SetRunNum(Int_t run=1)     {m_RunNum=run;}
// virtual void   SetEventNum(Int_t event=1) {m_EventNum=event;}
   virtual void   SetEvent(Int_t event=1) {m_Event=event;}
   virtual void   SetMode(Int_t mode=0)   {m_Mode=mode;}
   void           SetDisplay(AMSVirtualDisplay *display) {m_Display=display;}
   virtual void   Make(Int_t i=0);
   void           MakeTree(const char* name="AMS Event", const char*title="AMSRoot tree");
   void           FillTree();

//    Setters for flags and switches
//   void           SetBfield(Bool_t field=1)     {m_Bfield=field;}
//   void           SetTrackFinding(Bool_t val=0) {m_TrackFinding=val;}


   void           SortDown(Int_t n, Float_t *a, Int_t *index, Bool_t down=kTRUE);

   ClassDef(AMSRoot, 1)   //AMSRoot control class
};

extern AMSRoot *gAMSRoot;

#endif
