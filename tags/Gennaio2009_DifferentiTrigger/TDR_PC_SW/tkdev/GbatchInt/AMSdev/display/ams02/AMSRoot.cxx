//  $Id: AMSRoot.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot                                                              //
//                                                                      //
// Main class to control the AMSRoot program.                           //
//                                                                      //
// This class is created by modifying Rene Brun's ATLFast program       // 
// This class is a general framework for programs that needs to:        //
//    - Initialise some parameters                                      //
//    - Loop on events                                                  //
//    - Print results and save histograms, etc                          //
//                                                                      //
// The event processor AMSRoot::Make loops on a list of Makers          //
// where each maker performs some task on the event data and generates  //
// results.                                                             //
// New Makers can be inserted by a user without modifying this class.   //
// Note that the order in which the Makers are called is the order      //
// of insertion in the list of Makers.                                  //
// Each Maker is responsible for creating its branch of the Tree.       //
// The following table shows the list of makers currently implemented   //
// The default option to Save the Maker info in the Tree is mentioned.  //
//                                                                      //
//    Maker name        Implemented      Save in Tree                   //
//    ==========        ============     =============                  //
//    MCMaker             NO                  NO                        //
//    ToFClusterMaker     NO                  NO                        //
//    SiClusterMaker      NO                  NO                        //
//    ACCClusterMaker     NO                  NO                        //
//    ElectronMaker       NO                  NO                        //
//    MuonMaker           NO                  NO                        //
//    PhotonMaker         NO                  NO                        //
//    JetMaker            NO                  NO                        //
//    TrackMaker          NO                  NO                        //
//    TriggerMaker        NO                  NO                        //
//    MiscMaker           NO                  NO                        //
//                                                                      //
// Makers must derive from the base class AMSMaker.                     //
// AMSMaker provides a common interface to all Makers.                  //
// Each Maker is responsible for defining its own parameters and        //
// histograms.                                                          //
// Each Maker has its own list of histograms.                           //
// Each Maker has an associated companion class corresponding to the    //
// type of physics object reconstructed by the Maker.                   //
// For example, AMSToFClusterMaker creates AMSToFCluster objects.       //
//              AMSTriggerMaker creates one single AMSTrigger object.   //
// The pointer supporting the created object(s) is defined in AMSMaker  //
//   m_Fruits may point to a single object (eg. AMSTrigger) or to a     //
//           TClonesArray of objects (eg. AMSCluster).                  //
//                                                                      //
// The function AMSRoot::Maketree must be called after the creation     //
// of the AMSRoot object to create a Root Tree.                         //
//                                                                      //
// An example of main program/macro to use AMSRoot is given below:      //
//========================================================================
//  ****** WARNING ********   
//  The following ATLFast example is not working yet on alpha / OSF1
//========================================================================
//void umain(Int_t nevents=100)
//{
//   gROOT->Reset();
//   gSystem->Load("libatlfast.so");  // dynamically link the compiled shared library
//
//   // Open the root output file
//   TFile file("atlfast.root","recreate","AMSRoot root file",2);
//   
//   AMSRoot atlfast("atlfast");     // create main object to run atlfast
//
//   User user;           // create an object of the User class defined in user.C
//
//   atlfast.Init();      // Initialise event (maker histograms,etc)
//   atlfast.MakeTree();  // Create the Root tree
//
//   gROOT->LoadMacro("user.C");  // compile/interpret user file
//
//   for (Int_t i=0; i<nevents; i++) {
//      if (i%100 == 0) printf("In loop:%d\n",i);
//      atlfast.Make(i);       // Generate and reconstruct event
//      user.FillHistograms(); // User has possibility to decide if store event here!
//      atlfast.FillTree();
//      atlfast.Clear();       // Clear all event lists
//   }
//   atlfast.Finish();
//
//   // save objects in Root file
//   atlfast.Write();  //save main atlfast object (and run parameters)
//}
//========================================================================
//                                                                      //
// This example illustrates how to:                                     //
//    - Load a shared library                                           //
//    - Open a Root file                                                //
//    - Initialise AMSRoot                                              //
//    - Load some user code (interpreted)                               //
//      This user code may redefine some Maker parameters               //
//    - Make a loop on events                                           //
//    - Save histograms and the main AMSRoot object and its Makers      //
//                                                                      //
//========================================================================
//  An example of a User class is given below:                          //
//========================================================================
//
//#ifndef user_H
//#define user_H
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// User                                                                 //
//                                                                      //
// Example of a user class to perform user specific tasks when running  //
// the ATLfast program.                                                 //
//                                                                      //
// This class illustrates:                                              //
//   - How to set run parameters                                        //
//   - How to create and fill histograms                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//
//class TH1F;
//class AMSRoot;
//class ATLFClusterMaker;
//class ATLFPhotonMaker;
//
//class User {
//
//private:
//   TH1F             *m_hist1;       //pointer to histogram
//   TH1F             *m_hist2;       //pointer to histogram
//   TH1F             *m_hist3;       //pointer to histogram
//public:
//               User();
//   void        FillHistograms();
//   void        SetRunParameters();
//
//#endif
//};
//
//_________________________________________________________________________
//User::User() 
//{
//   SetRunParameters();  //change default parameters
//
//         Create a few histograms
//   m_hist1 = new TH1F("hist1","Number of tracks per event",100,0,100);
//   m_hist2 = new TH1F("hist2","Number of clusters",100,0,100);
//   m_hist3 = new TH1F("hist3","Number of isolated muons",20,0,20);
//}
//
//_________________________________________________________________________
//void User::FillHistograms()
//{
////   m_hist1.Fill(event->GetNtracks());
////   m_hist2.Fill(event->GetNclusters));
////   m_hist3.Fill(event->GetNIsoMuons());
//}
//
//_________________________________________________________________________
//void User::SetRunParameters()
//{
//  // change ATLfast default parameters
//
//   gAMSRoot->SetSmearMuonOpt(0);
//   gAMSRoot->ClusterMaker()->SetGranBarrelEta(0.12);
//   gAMSRoot->PhotonMaker()->SetMinPT(6.);
//   gAMSRoot->TriggerMaker()->SetMuoEtaCoverage(2.8);
//
//}
//======================end of User class=================================
//
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <ctype.h>
#include <TROOT.h>
#include <TTree.h>
#include <TBrowser.h>
#include <TClonesArray.h>

#include "AMSRoot.h"
#include "AMSToFClusterReader.h"
#include "AMSTrackReader.h"
#include "AMSSiHitReader.h"
#include "AMSAntiClusterReader.h"
#include "AMSTrMCClusterReader.h"
#include "AMSParticleReader.h"
#include "AMSMCParticleReader.h"

//#include "AMSMCMaker.h"
// #include "ATLFElectronMaker.h"
// #include "ATLFMuonMaker.h"
// #include "ATLFPhotonMaker.h"
// #include "ATLFJetMaker.h"
//#include "AMSTriggerMaker.h"
//#include "AMSMiscMaker.h"
//#include "AMSHistBrowser.h"

#include "AMSVirtualDisplay.h"

AMSRoot *gAMSRoot;

ClassImp(AMSRoot)





//_____________________________________________________________________________
void AMSRoot::CommonConstruct()
{
   // Performs common construction of AMSRoot object
   //

   //
   // point global AMSRoot object to this
   //
   gAMSRoot      = this;

   //
   // initialize some data members
   //
   m_Version     = 000;		// AMSRoot  version number and release date
   m_VersionDate = 19971110;	// 10-Nov-1997
   m_RunNum      = 0;
   m_EventNum    = 0;
   m_Mode        = 0;
   m_Event       = 0;
   m_Tree        = 0;
   m_Display     = 0;
   SelectedEvent=0;
}


//_____________________________________________________________________________
AMSRoot::AMSRoot() : TNamed("AMSRoot","The AMS Display with Root")
{

   CommonConstruct();

   m_Makers          = 0;
   m_ToFClusterMaker = 0;
   m_SiHitMaker      = 0;
   m_AntiClusterMaker = 0;
   m_TrMCClusterMaker = 0;
   m_TrackMaker      = 0;
   m_ParticleMaker   = 0;
   m_MCParticleMaker   = 0;

//   m_MCMaker       = 0;
//   m_SiClusterMaker  = 0;
//   m_ACCClusterMaker = 0;

//   m_ElectronMaker = 0;
//   m_MuonMaker     = 0;
//   m_PhotonMaker   = 0;
//   m_JetMaker      = 0;
//   m_TrackMaker    = 0;
//   m_TriggerMaker  = 0;
//   m_MiscMaker     = 0;
}

//_____________________________________________________________________________
AMSRoot::AMSRoot(const char *name, const char *title)
        : TNamed(name,title)
{

   CommonConstruct();
   
   SetDefaultParameters();

   gROOT->GetListOfBrowsables()->Add(this,"AMSRoot");

// create the support list for the various lists of AMSRoot objects
   m_Makers  = new TList();

// create "standard" makers and add them to the list of makers 
// (in AMSRoot constructor)
// Note that the order in which makers are added to the list of makers 
// is important ---  makers will be processed in this order !!

   m_AntiClusterMaker  = new AMSAntiClusterReader("AntiClusterReader",
			    "Read AMSRoot Anti Counter Clusters");
   m_Makers->Add(m_AntiClusterMaker);

   m_TrMCClusterMaker  = new AMSTrMCClusterReader("TrMCClusterReader",
			    "Read AMSRoot TrMC Counter Clusters");
   m_Makers->Add(m_TrMCClusterMaker);




   m_ToFClusterMaker  = new AMSToFClusterReader("ToFClusterReader",
			    "Read AMSRoot ToF clusters from ntuple root file");
   m_Makers->Add(m_ToFClusterMaker);

   m_SiHitMaker       = new AMSSiHitReader("SiHitReader","Read AMSRoot SiHits");
   m_Makers->Add(m_SiHitMaker);

   m_TrackMaker       = new AMSTrackReader("TrackReader","Read AMSRoot tracks");
   m_Makers->Add(m_TrackMaker);

   m_ParticleMaker    = new AMSParticleReader("ParticleReader",
			    "Read AMSRoot particles");
   m_MCParticleMaker    = new AMSMCParticleReader("MCParticleReader",
			    "Read AMSRoot MC particles");
   m_Makers->Add(m_ParticleMaker);
   m_Makers->Add(m_MCParticleMaker);

//   m_MCMaker       = new AMSMCMaker("MCMaker","Make MC events");
//   m_SiClusterMaker  = new AMSSiClusterMaker("SiClusterMaker","Make AMSRoot Si Tracker clusters");
//   m_ACCClusterMaker  = new AMSACCClusterMaker("ACCClusterMaker","Make AMSRoot Anti Coincidence Counter clusters");
//   m_MuonMaker     = new ATLFMuonMaker("MuonMaker","Make AMSRoot muons");
//   m_ElectronMaker = new ATLFElectronMaker("ElectronMaker","Make AMSRoot electrons");
//   m_PhotonMaker   = new ATLFPhotonMaker("PhotonMaker","Make AMSRoot photons");
//   m_JetMaker      = new ATLFJetMaker("JetMaker","Make AMSRoot jets");
//   m_MiscMaker     = new AMSMiscMaker("MiscMaker","Make AMSRoot miscelaneous");
//   m_TriggerMaker  = new AMSTriggerMaker("TriggerMaker","Make AMSRoot trigger");

}


//_____________________________________________________________________________
AMSRoot::~AMSRoot()
{
  // to be written

}


//______________________________________________________________________________
void AMSRoot::Browse(TBrowser *b)
{

  if( b == 0) return;

  if (m_Tree) b->Add(m_Tree,m_Tree->GetName());

  b->Add(&m_HistBrowser, "Histograms");

  TIter next(m_Makers);
  AMSMaker *maker;
  while (maker = (AMSMaker*)next()) {
     b->Add(maker,maker->GetName());
   }
}

//_____________________________________________________________________________

Int_t AMSRoot::IsGolden(){

  AMSMaker * p = (AMSMaker *) ParticleMaker();
   
  TObject *fruits = p->Fruits();
  TClonesArray *clones = (TClonesArray*)fruits;
   return clones->GetEntries();

}



Int_t AMSRoot::GetEvent(Int_t event)
{
   Int_t ret=0;
   // Get one event into the result tree
   //

   if (event<0) return 0;
   //   cout << " Select Event No : ";
   //   cin >> event;
   //   cin.ignore(INT_MAX,'\n');
   //    Read event in input tree
   if (m_Input) {
     debugger.Print("AMSRoot::GetEvent(): doing m_Input->GetEvent(%d)\n", event);
        ret= m_Input->GetEvent(event);
        if(ret==0)return 0;
   }
   else return 0;
   //
   // Make them into the result tree
   //
   TIter next(m_Makers);
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      debugger.Print("starting Maker %s...\n", maker->GetName());
      maker->Make();
   }

   //
   // Keep a record of current event number
   //
   // For data this is read in automatically
   m_Event = event;
   return ret;
}


void AMSRoot::SelectEvent()
{
   // Get one event into the result tree
   //
   int mev;
   int event=SelectedEvent;
      if(event<=0)event=1;
   //    Read event in input tree
   if (m_Input) {
     debugger.Print("AMSRoot::GetEvent(): doing m_Input->GetEvent(%d)\n", event);
     for (int i=0;i<event;i++){
          m_Input->GetEvent(i);
          if(m_EventNum  >= event)break;          
     }
      cout <<m_EventNum <<" event selected."<<endl;
     mev=i;
   }
   //
   // Make them into the result tree
   //
   TIter next(m_Makers);
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      debugger.Print("starting Maker %s...\n", maker->GetName());
      maker->Make();
   }

   //
   // Keep a record of current event number
   //
   // For data this is read in automatically
   m_Event = mev;
}



//_____________________________________________________________________________
void AMSRoot::Init(TTree * h1)
{
   // Initialize the AMSRoot object and its makers
   //

   m_Input = h1;

   h1->SetBranchAddress("eventno",&m_EventNum);
   h1->SetBranchAddress("run",&m_RunNum);
   h1->SetBranchAddress("runtype",&m_Mode);
   h1->SetBranchAddress("time",m_time);

   //
   //    Initialise makers
   //
   TIter next(m_Makers);
   AMSMaker *maker;
   TObject *objfirst, *objlast;
   while (maker = (AMSMaker*)next()) {
     char const * name = maker->ClassName();
     debugger.Print("initialize %s\n", name);
     // save last created histogram in current Root directory
      objlast = gDirectory->GetList()->Last();

     // Initialise maker
      maker->Init(h1);
     debugger.Print("%s initialized\n", name);

     // Add Maker histograms in Maker list of histograms
      if (objlast) objfirst = gDirectory->GetList()->After(objlast);
      else         objfirst = gDirectory->GetList()->First();
      while (objfirst) {
         maker->Histograms()->Add(objfirst);
         objfirst = gDirectory->GetList()->After(objfirst);
      }
   }

   debugger.Print("<=== Returning from AMSRoot::Init()\n");
}

//_____________________________________________________________________________
void AMSRoot::Finish()
{
//    Terminate a run
//   place to make operations on histograms, normalization,etc.

   TIter next(m_Makers);
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      maker->Finish();
   }
}

//_____________________________________________________________________________
void AMSRoot::Draw(Option_t *option)
{
//    Insert current event in graphics pad list

    // Check if the Event Display object has been created
   if (!m_Display) {
      Error("Draw","You must create an AMSDisplay object first");
      return;
   }

   m_Display->Draw(option);
}

//_____________________________________________________________________________
void AMSRoot::Paint(Option_t *option)
{
//    Paint AMSRoot objects

   if (m_Display) m_Display->Paint(option);
}

//_____________________________________________________________________________
void AMSRoot::Clear(Option_t *option)
{
//    Reset lists of event objects

   TIter next(m_Makers);
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
//debugger.Print("clearing maker:%s\n",maker->GetName());
      maker->Clear(option);
   }
//debugger.Print("Clearing display\n");
   if (m_Display) m_Display->Clear();
}

//_____________________________________________________________________________
void AMSRoot::PrintInfo()
{
//     Gives information about versions etc.
   printf("\n\n");
   printf("**************************************************************\n");
   printf("*             AMSRoot version:%3d released at %8d       *\n",m_Version, m_VersionDate);
   printf("**************************************************************\n");
   printf("*                                                            *\n");
   printf("*           Display AMS events                               *\n");
   printf("*                                                            *\n");
   printf("*                          by   Ping Yeh,  Academia Sinica   *\n");
   printf("*                          based on ATLFast program          *\n");
   printf("*                                                            *\n");
//   printf("*     Contributed code:                                      *\n");
//   printf("*         ntuples format (in ATLFAST)                        *\n");
   printf("**************************************************************\n");
   printf("\n\n");

//     Print info for all defined Makers
   TIter next(m_Makers);
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      maker->PrintInfo();
   }
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             Setters                                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
void AMSRoot::SetDefaultParameters()
{

//    Setters for flags and switches
//   SetBfield();
//   SetTrackFinding();
}

//_____________________________________________________________________________
void AMSRoot::Make(Int_t i)
{
   m_Event = i;

//   Loop on all makers
   TIter next(m_Makers);
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      maker->Make();
   }

}

//_____________________________________________________________________________
void AMSRoot::MakeTree(const char* name, const char*title)
{
//  Create a ROOT tree
//  Loop on all makers to create the Root branch (if any)

   if (m_Tree) return;

   m_Tree = new TTree(name,title);

   if (! m_Tree) {
     debugger.Print("Can't make a tree\n");
     return;	// error
   }

   TIter next(m_Makers);
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      debugger.Print("make branch %s\n", (char *)maker->ClassName());
      maker->MakeBranch();
   }
}

//_____________________________________________________________________________
void AMSRoot::FillTree()
{
//  Fill the ROOT tree, looping on all active branches

  // Clean generated particles (depending on option Save)
//   MCMaker()->CleanParticles();

  // Now ready to fill the Root Tree
   if(m_Tree) m_Tree->Fill();
}

//_____________________________________________________________________________
void AMSRoot::SortDown(Int_t n1, Float_t *a, Int_t *index, Bool_t down)
{
//  sort the n1 elements of array a.
//  In output the array index contains the indices of the sorted array.
//  if down is false sort in increasing order (default is decreasing order)
//   This is a translation of the CERNLIB routine sortzv (M101)
//   based on the quicksort algorithm

   Int_t i,i1,n,i2,i3,i33,i22,i222,iswap,n2;
   Float_t ai;
   n = n1;
   if (n <= 0) return;
   if (n == 1) {index[0] = 0; return;}
   for (i=0;i<n;i++) index[i] = i+1;
   for (i1=2;i1<=n;i1++) {
      i3 = i1;
      i33 = index[i3-1];
      ai  = a[i33-1];
      while(1) {
         i2 = i3/2;
         if (i2 <= 0) break;
         i22 = index[i2-1];
         if (ai <= a[i22-1]) break;
         index[i3-1] = i22;
         i3 = i2;
      }
      index[i3-1] = i33;
   }

   while(1) {
      i3 = index[n-1];
      index[n-1] = index[0];
      ai = a[i3-1];
      n--;
      if(n-1 < 0) {index[0] = i3; break;}
      i1 = 1;
      while(2) {
         i2 = i1+i1;
         if (i2 <= n) i22 = index[i2-1];
         if (i2-n > 0) {index[i1-1] = i3; break;}
         if (i2-n < 0) {
            i222 = index[i2];
            if (a[i22-1] - a[i222-1] < 0) {
                i2++;
                i22 = i222;
            }
         }
         if (ai - a[i22-1] > 0) {index[i1-1] = i3; break;}
         index[i1-1] = i22;
         i1 = i2;
      }
   }
   if (!down) return;
   n2 = n1/2;
   for (i=0;i<n1;i++) index[i]--;
   for (i=0;i<n2;i++) {
      iswap         = index[i];
      index[i]      = index[n1-i-1];
      index[n1-i-1] = iswap;
   }
}

//______________________________________________________________________________
void AMSRoot::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSRoot.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      TNamed::Streamer(R__b);
      gAMSRoot = this;
      gROOT->GetListOfBrowsables()->Add(this,"AMSRoot");
      R__b >> m_Version;
      R__b >> m_VersionDate;
      R__b >> m_RunNum;
      R__b >> m_EventNum;
      R__b >> m_Mode;
      R__b >> m_Event;
      R__b >> m_Tree;
      R__b >> m_Makers;
//      R__b >> m_MCMaker;
      R__b >> m_ToFClusterMaker;
//      R__b >> m_SiClusterMaker;
//      R__b >> m_ACCClusterMaker;

//      R__b >> m_ElectronMaker;
//      R__b >> m_MuonMaker;
//      R__b >> m_PhotonMaker;
//      R__b >> m_JetMaker;
//      R__b >> m_TrackMaker;
//      R__b >> m_TriggerMaker;
//      R__b >> m_MiscMaker;
//      R__b >> m_Bfield;
//      R__b >> m_TrackFinding;
      m_HistBrowser.Streamer(R__b);
   } else {
      R__b.WriteVersion(AMSRoot::IsA());
      TNamed::Streamer(R__b);
      R__b << m_Version;
      R__b << m_VersionDate;
      R__b << m_RunNum;
      R__b << m_EventNum;
      R__b << m_Mode;
      R__b << m_Event;
      R__b << m_Tree;
      R__b << m_Makers;
//      R__b << m_MCMaker;
      R__b << m_ToFClusterMaker;
//      R__b << m_SiClusterMaker;
//      R__b << m_ACCClusterMaker;

//      R__b << m_ElectronMaker;
//      R__b << m_MuonMaker;
//      R__b << m_PhotonMaker;
//      R__b << m_JetMaker;
//      R__b << m_TrackMaker;
//      R__b << m_TriggerMaker;
//      R__b << m_MiscMaker;
//      R__b << m_Bfield;
//      R__b << m_TrackFinding;
      m_HistBrowser.Streamer(R__b);
   }
}

Int_t AMSRoot::SetSelectedEvent(const char *event){
  //returns 0 if failure 1 otherwise
  for(int i=0;i<strlen(event);i++){
    char c=event[i];
    if(!isdigit(c))return 0;
  }
  sscanf(event,"%d",&SelectedEvent);
  return 1;
}
