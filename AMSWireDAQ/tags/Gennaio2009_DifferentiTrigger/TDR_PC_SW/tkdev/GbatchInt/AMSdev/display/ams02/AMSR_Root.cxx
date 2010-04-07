//  $Id: AMSR_Root.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root                                                              //
//                                                                      //
// Main class to control the AMSR_Root program.                           //
//                                                                      //
// This class is created by modifying Rene Brun's ATLFast program       // 
// This class is a general framework for programs that needs to:        //
//    - Initialise some parameters                                      //
//    - Loop on events                                                  //
//    - Print results and save histograms, etc                          //
//                                                                      //
// The event processor AMSR_Root::Make loops on a list of Makers          //
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
//    CTCClusterMaker     NO                  NO                        //
//    ACCClusterMaker     NO                  NO                        //
//    ElectronMaker       NO                  NO                        //
//    MuonMaker           NO                  NO                        //
//    PhotonMaker         NO                  NO                        //
//    JetMaker            NO                  NO                        //
//    TrackMaker          NO                  NO                        //
//    TriggerMaker        NO                  NO                        //
//    MiscMaker           NO                  NO                        //
//                                                                      //
// Makers must derive from the base class AMSR_Maker.                     //
// AMSR_Maker provides a common interface to all Makers.                  //
// Each Maker is responsible for defining its own parameters and        //
// histograms.                                                          //
// Each Maker has its own list of histograms.                           //
// Each Maker has an associated companion class corresponding to the    //
// type of physics object reconstructed by the Maker.                   //
// For example, AMSR_ToFClusterMaker creates AMSR_ToFCluster objects.       //
//              AMSR_TriggerMaker creates one single AMSR_Trigger object.   //
// The pointer supporting the created object(s) is defined in AMSR_Maker  //
//   m_Fruits may point to a single object (eg. AMSR_Trigger) or to a     //
//           TClonesArray of objects (eg. AMSR_Cluster).                  //
//                                                                      //
// The function AMSR_Root::Maketree must be called after the creation     //
// of the AMSR_Root object to create a Root Tree.                         //
//                                                                      //
// An example of main program/macro to use AMSR_Root is given below:      //
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
//   TFile file("atlfast.root","recreate","AMSR_Root root file",2);
//   
//   AMSR_Root atlfast("atlfast");     // create main object to run atlfast
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
//    - Initialise AMSR_Root                                              //
//    - Load some user code (interpreted)                               //
//      This user code may redefine some Maker parameters               //
//    - Make a loop on events                                           //
//    - Save histograms and the main AMSR_Root object and its Makers      //
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
//class AMSR_Root;
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
//   gAMSR_Root->SetSmearMuonOpt(0);
//   gAMSR_Root->ClusterMaker()->SetGranBarrelEta(0.12);
//   gAMSR_Root->PhotonMaker()->SetMinPT(6.);
//   gAMSR_Root->TriggerMaker()->SetMuoEtaCoverage(2.8);
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
#include <TFile.h>

#include "AMSR_Root.h"
#include "AMSR_ToFClusterReader.h"
#include "AMSR_TrdClusterReader.h"
#include "AMSR_TrackReader.h"
#include "AMSR_TrdTrackReader.h"
#include "AMSR_EcalShowerReader.h"
#include "AMSR_SiHitReader.h"
#include "AMSR_AntiClusterReader.h"
#include "AMSR_TrMCClusterReader.h"
#include "AMSR_ParticleReader.h"
#include "AMSR_MCParticleReader.h"
#include "AMSR_Ntuple.h"

//#include "AMSR_MCMaker.h"
// #include "ATLFElectronMaker.h"
// #include "ATLFMuonMaker.h"
// #include "ATLFPhotonMaker.h"
// #include "ATLFJetMaker.h"
//#include "AMSR_TriggerMaker.h"
//#include "AMSR_MiscMaker.h"
//#include "AMSR_HistBrowser.h"

#include "AMSR_VirtualDisplay.h"

AMSR_Root *gAMSR_Root;

ClassImp(AMSR_Root)


//_____________________________________________________________________________
void AMSR_Root::CommonConstruct()
{
   // Performs common construction of AMSR_Root object
   //

   //
   // point global AMSR_Root object to this
   //
   gAMSR_Root      = this;

   //
   // initialize some data members
   //
   m_DataFileName = 0;
   m_Version     = 000;		// AMSR_Root  version number and release date
   m_VersionDate = 19971110;	// 10-Nov-1997
   m_RunNum      = 0;
   m_EventNum    = 0;
   m_Mode        = 0;
   m_NEvent      = 0;
   m_Event       = -1;
   m_Tree        = 0;
   m_Display     = 0;
}


//_____________________________________________________________________________
AMSR_Root::AMSR_Root() : TNamed("AMSR_Root","The AMS Display with Root")
{

   CommonConstruct();

   m_Ntuple          = 0;
   m_Makers          = 0;
   m_ToFClusterMaker = 0;
   m_TrdClusterMaker = 0;
   m_SiHitMaker      = 0;
   m_AntiClusterMaker = 0;
   m_TrMCClusterMaker = 0;
   m_TrackMaker      = 0;
   m_TrdTrackMaker      = 0;
   m_EcalShowerMaker      = 0;
   m_ParticleMaker   = 0;
   m_MCParticleMaker   = 0;

//   m_MCMaker       = 0;
//   m_SiClusterMaker  = 0;
//   m_ACCClusterMaker = 0;

//   m_TriggerMaker  = 0;
//   m_MiscMaker     = 0;
}

//_____________________________________________________________________________
AMSR_Root::AMSR_Root(const char *name, const char *title)
        : TNamed(name,title)
{

   CommonConstruct();
   
   SetDefaultParameters();

   gROOT->GetListOfBrowsables()->Add(this,"AMSR_Root");

// create a ntuple/tree handler
   m_Ntuple = new AMSR_Ntuple();

// create the support list for the various lists of AMSR_Root objects
   m_Makers  = new TList();

// create "standard" makers and add them to the list of makers 
// (in AMSR_Root constructor)
// Note that the order in which makers are added to the list of makers 
// is important ---  makers will be processed in this order !!

   m_AntiClusterMaker  = new AMSR_AntiClusterReader("AntiClusterReader",
			    "Read AMSR_Root Anti Counter Clusters");

   m_Makers->Add(m_AntiClusterMaker);


   m_TrMCClusterMaker  = new AMSR_TrMCClusterReader("TrMCClusterReader",
			    "Read AMSR_Root TrMC Counter Clusters");
   m_Makers->Add(m_TrMCClusterMaker);

   m_ToFClusterMaker  = new AMSR_ToFClusterReader("ToFClusterReader",
			    "Read AMSR_Root ToF clusters from ntuple root file");
   m_Makers->Add(m_ToFClusterMaker);


   m_TrdClusterMaker  = new AMSR_TrdClusterReader("TrdClusterReader",
			    "Read AMSR_Root Trd clusters from ntuple root file");
   m_Makers->Add(m_TrdClusterMaker);


   m_SiHitMaker       = new AMSR_SiHitReader("SiHitReader","Read AMSR_Root SiHits");
   m_Makers->Add(m_SiHitMaker);

   m_TrackMaker       = new AMSR_TrackReader("TrackReader","Read AMSR_Root tracks");
   m_Makers->Add(m_TrackMaker);

   m_TrdTrackMaker       = new AMSR_TrdTrackReader("TrdTrackReader","Read AMSR_Root Trdtracks");
   m_Makers->Add(m_TrdTrackMaker);

   m_EcalShowerMaker       = new AMSR_EcalShowerReader("EcalShowerReader","Read AMSR_Root ecsh");
   m_Makers->Add(m_EcalShowerMaker);

   m_ParticleMaker    = new AMSR_ParticleReader("ParticleReader",
			    "Read AMSR_Root particles");
   m_MCParticleMaker    = new AMSR_MCParticleReader("MCParticleReader",
			    "Read AMSR_Root MC particles");
   m_Makers->Add(m_ParticleMaker);
   m_Makers->Add(m_MCParticleMaker);

//   m_MCMaker       = new AMSR_MCMaker("MCMaker","Make MC events");
//   m_SiClusterMaker  = new AMSR_SiClusterMaker("SiClusterMaker","Make AMSR_Root Si Tracker clusters");
//   m_ACCClusterMaker  = new AMSR_ACCClusterMaker("ACCClusterMaker","Make AMSR_Root Anti Coincidence Counter clusters");
//   m_MiscMaker     = new AMSR_MiscMaker("MiscMaker","Make AMSR_Root miscelaneous");
//   m_TriggerMaker  = new AMSR_TriggerMaker("TriggerMaker","Make AMSR_Root trigger");

}


//_____________________________________________________________________________
AMSR_Root::~AMSR_Root()
{
  // to be written

}

//______________________________________________________________________________
Int_t AMSR_Root::OpenDataFile(char * filename, EDataFileType type)
{
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //  Open a file for reading, also initializes all readers so they
  //  are able to read their parts of events from the data file.
  //
  //  Return values:
  //	0	successful
  //	1	can't open file
  //	2	can't initialize readers
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

  static TFile * f = 0;		// pointer to the opened root file
  static TTree * t = 0;		// pointer to the opened root tree

  //
  //Check if it is open already
  //
  if (m_DataFileName != 0) {
     if ( strcmp(m_DataFileName, filename) == 0 ) {
        Error("AMSR_Root::OpenDataFile"," %s already opened\n",filename);
        return 0;
     }
  }

  //
  // determine data type if type is specified as "kUnknown"
  //
  //Ntuple: .ext = .ntp* or .hbk*
  //Root:   .ext = .root*
  //ObjectivityDB: others
  //
  if ( type == kUnknown ) {
     char *slash = strrchr(filename, '/');
     if ( slash==0 ) slash = filename;

     if ( strlen(slash)==1 ) {  // nothing after '/'
        cerr << "\nAMSR_Canvas::OpenFileCB\n" << filename
          << " looks like a directory, not a filename ?" << endl;
	return 1;
     }
     
     char *dot = strrchr(slash, '.');
     
//     if ( !dot ) type = kObjectivityFile;
     if ( !dot ) type = kRootFile;
     else if ( strstr(dot+1, "root") == dot+1 ) type = kRootFile;
     else if ( strstr(dot+1, "ntp") == dot+1 )  type = kNtupleFile;
     else if ( strstr(dot+1, "hbk") == dot+1 )  type = kNtupleFile;
//     else type = kObjectivityFile;
     else type = kRootFile;
  }

  //
  // Open file according to data type
  //
  if (type == kRootFile) {
    TFile *fNew = TFile::Open(filename);   // support opening remote file
    if ( ! fNew ) {                        // cannot open this file
      return 1;
    }

    //
    //  Open the tree
    //
    char treeName[10];
    int  ntpID       = m_Ntuple->GetNextID();
    sprintf(treeName,"h%d",ntpID);
//    TTree * tNew = (TTree *)fNew->Get(treeName);
    TTree * tNew = (TTree *)fNew->Get("AMSRoot");
    debugger.Print("Number of entries in AMSRoot tree: %d \n",tNew->GetEntries());
    if (! tNew) {
      return 2;
    }

    //
    //  Now the new file are opened and read, close the old ones
    //
    delete t;
    delete f;
    f = fNew;
    t = tNew;

    //
    //  Initialize readers
    //
    Init(t);
     
  } else if ( type == kObjectivityFile ) {

    Error("AMSR_Root::OpenDataFile",
          "Sorry ! Data type of ObjectivityFile not implemented.\n");
    return 1;

  } else if ( type == kNtupleFile ) {

    int openStat = m_Ntuple->OpenNtuple(filename);

    if (openStat == 0) Init(0);
    else return openStat;
  }

  //
  //  Here the file is opened successfully, record its name and type
  //

  debugger.Print("Come to save filename : type=%s : %d\n",filename,type);
  if ( m_DataFileName )  delete m_DataFileName;
  m_DataFileName = new char[strlen(filename)+1];
  strcpy(m_DataFileName, filename);

  m_DataFileType = type;

  //
  //  Done
  //
  return 0;
}

//______________________________________________________________________________
void AMSR_Root::Browse(TBrowser *b)
{

  if( b == 0) return;

  if (m_Tree) b->Add(m_Tree,m_Tree->GetName());

  b->Add(&m_HistBrowser, "Histograms");

  TIter next(m_Makers);
  AMSR_Maker *maker;
  while (maker = (AMSR_Maker*)next()) {
     b->Add(maker,maker->GetName());
   }
}

//_____________________________________________________________________________
Int_t AMSR_Root::IsGolden(){

  AMSR_Maker * p = (AMSR_Maker *) ParticleMaker();
   
  TObject *fruits = p->Fruits();
  TClonesArray *clones = (TClonesArray*)fruits;
   return clones->GetEntries();

}



//_____________________________________________________________________________
Bool_t AMSR_Root::GetEvent(Int_t event)
{
   // Get one event into the result tree
   //
//   if (m_Ntuple==0 || m_Ntuple->GetTree() == 0) return kFALSE;
   if (m_Ntuple==0) return kFALSE;

   if (event<0 || event>m_NEvent || event==m_Event) return kFALSE;

   //   cout << " Select Event No : ";
   //   cin >> event;
   //   cin.ignore(INT_MAX,'\n');
   //    Read event in input tree
   debugger.Print("AMSR_Root::GetEvent(): doing m_Ntuple->GetEvent(%d)\n", event);

   m_Ntuple->GetEvent(event);
   m_RunNum   = m_Ntuple->GetRunNum();
   m_EventNum = m_Ntuple->GetEvtNum();

   m_Mode     = m_Ntuple->GetRunType();
   m_Ntuple->GetRunTime(m_Time);

   debugger.Print("AMSR_Root::GetEvent(): run,event %d %d %d\n", m_RunNum,m_EventNum,m_Mode);


   //
   // clear the makers first before making it
   // Make them into the result tree
   //
   Clear();

   TIter next(m_Makers);
   AMSR_Maker *maker;
   while (maker = (AMSR_Maker*)next()) {
      debugger.Print("starting Maker %s...\n", maker->GetName());
      maker->Make();
   }

   //
   // Keep a record of current event number
   //
   // For data this is read in automatically
   m_Event = event;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t AMSR_Root::GetEvent(Int_t run, Int_t event)
{
//   if (m_Ntuple==0 || m_Ntuple->GetTree() == 0) return kFALSE;
   if (m_Ntuple==0) return kFALSE;
   if (run==m_RunNum && event==m_EventNum) return kFALSE;

   //
   //disable all branches except "eventno" and "run" to quick up
   //
   AMSR_Ntuple *t = m_Ntuple;
   t->SetBranchStatus("*", 0);
   t->SetBranchStatus("eventno", 1);
   t->SetBranchStatus("run", 1);

   //
   //loop over the tree to find event with RunNum=run# and EventNum>=event
   //
   for (Int_t i=0; i<=m_NEvent; i++) {
      t->GetEvent(i);
      m_RunNum = m_Ntuple->GetRunNum();
      m_EventNum = m_Ntuple->GetEvtNum();
      if (m_RunNum==run && m_EventNum>=event) {
         t->SetBranchStatus("*", 1);         //recover to enable all branches
         return GetEvent(i);
      }
   }

   t->GetEvent(m_Event);               //resume m_RunNum/m_EventNum
   t->SetBranchStatus("*", 1);         //recover to enable all branches
   return kFALSE;
}


//_____________________________________________________________________________
void AMSR_Root::Init(TTree *h1)
{
   //
   // Initialize run/event number, the AMSR_Root object and its makers
   //

   m_RunNum      = 0;
   m_EventNum    = 0;
   m_Event       = -1;

   m_Ntuple->SetTree(h1);

   m_NEvent = m_Ntuple->GetEntries();

//   h1->SetBranchAddress("eventno",&m_EventNum);
//   h1->SetBranchAddress("run",&m_RunNum);
//   h1->SetBranchAddress("runtype",&m_Mode);
//   h1->SetBranchAddress("time",m_time);

   //
   //    Initialise makers
   //
   TIter next(m_Makers);
   AMSR_Maker *maker;
   TObject *objfirst, *objlast;
   while (maker = (AMSR_Maker*)next()) {
     char const * name = maker->ClassName();
     // save last created histogram in current Root directory
      objlast = gDirectory->GetList()->Last();

     // Add Maker histograms in Maker list of histograms
      if (objlast) objfirst = gDirectory->GetList()->After(objlast);
      else         objfirst = gDirectory->GetList()->First();
      while (objfirst) {
         maker->Histograms()->Add(objfirst);
         objfirst = gDirectory->GetList()->After(objfirst);
      }
   }

   debugger.Print("<=== Returning from AMSR_Root::Init()\n");
}

//_____________________________________________________________________________
void AMSR_Root::Finish()
{
//    Terminate a run
//   place to make operations on histograms, normalization,etc.

   TIter next(m_Makers);
   AMSR_Maker *maker;
   while (maker = (AMSR_Maker*)next()) {
      maker->Finish();
   }
}

//_____________________________________________________________________________
void AMSR_Root::Draw(Option_t *option)
{
//    Insert current event in graphics pad list

    // Check if the Event Display object has been created
   if (!m_Display) {
      Error("Draw","You must create an AMSR_Display object first");
      return;
   }

   m_Display->Draw(option);
}

//_____________________________________________________________________________
void AMSR_Root::Paint(Option_t *option)
{
//    Paint AMSR_Root objects

   if (m_Display) m_Display->Paint(option);
}

//_____________________________________________________________________________
void AMSR_Root::Clear(Option_t *option)
{
//    Reset lists of event objects

   TIter next(m_Makers);
   AMSR_Maker *maker;
   while (maker = (AMSR_Maker*)next()) {
//debugger.Print("clearing maker:%s\n",maker->GetName());
      maker->Clear(option);
   }
//debugger.Print("Clearing display\n");
   if (m_Display) m_Display->Clear();
}

//_____________________________________________________________________________
void AMSR_Root::PrintInfo()
{
//     Gives information about versions etc.
   printf("\n\n");
   printf("**************************************************************\n");
   printf("*             AMSR_Root version:%3d released at %8d       *\n",m_Version, m_VersionDate);
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
   AMSR_Maker *maker;
   while (maker = (AMSR_Maker*)next()) {
      maker->PrintInfo();
   }
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             Setters                                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
void AMSR_Root::SetDefaultParameters()
{

//    Setters for flags and switches
//   SetBfield();
//   SetTrackFinding();
}

//_____________________________________________________________________________
void AMSR_Root::Make(Int_t i)
{
   m_Event = i;

//   Loop on all makers
   TIter next(m_Makers);
   AMSR_Maker *maker;
   while (maker = (AMSR_Maker*)next()) {
      maker->Make();
   }

}


//_____________________________________________________________________________
void AMSR_Root::SortDown(Int_t n1, Float_t *a, Int_t *index, Bool_t down)
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
void AMSR_Root::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Root.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      TNamed::Streamer(R__b);
      gAMSR_Root = this;
      gROOT->GetListOfBrowsables()->Add(this,"AMSR_Root");
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

//      R__b >> m_TriggerMaker;
//      R__b >> m_MiscMaker;
//      R__b >> m_Bfield;
//      R__b >> m_TrackFinding;
      m_HistBrowser.Streamer(R__b);
   } else {
      R__b.WriteVersion(AMSR_Root::IsA());
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
      R__b << m_TrdClusterMaker;
//      R__b << m_SiClusterMaker;
//      R__b << m_CTCClusterMaker;
//      R__b << m_ACCClusterMaker;

//      R__b << m_TriggerMaker;
//      R__b << m_MiscMaker;
//      R__b << m_Bfield;
//      R__b << m_TrackFinding;
      m_HistBrowser.Streamer(R__b);
   }
}

