///   \example run.stlv.C
///   This is an example of how to load stlv.C (to run: root;  .x run.stlv.C)   
{
// Global ROOT Reset
      gROOT->Reset();

// Set AMSWD directory
      gSystem->Setenv("AMSWD","/Offline/vdev");
    //gSystem->Setenv("AMSWD","/f2users/choutko/AMS");

// Set include path
      gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));

// Load AMS shared library
// For Linux load
      gSystem->Load("$AMSWD/lib/linux/ntuple.so");
// For Digital Unix, i.e. ams.cern.ch load 
      //gSystem->Load("$AMSWD/lib/osf1/ntuple.so");

// Add all AMS Root files to be analyzed 
      TChain chain("AMSRoot");
// Usual ROOT file
chain.Add("/f0dah3/MC/AMS02/2005A/deuterons/el.pl1.0510/1355.0*.root");
//      chain.Add("/f2users/choutko/g3v1g3.5.root");
//      chain.Add("/f2users/alcaraz/ams/gamma/protons_el.pl1.10200.root");

// Add a ROOT file on the WeB
      //chain.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");
// Add a ROOT file on Castor
      //chain.Add("rfio:/castor/cern.ch/amsfMC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

// Process the file and loop over events
// Inside stlv.C, you have to implement the class "stlv"
// Everything works OK if you change all "stlv" by "ANYTHING" ==>
//     "ANYTHING.C" + "class ANYTHING" + chain.Process("ANYTHING.C+")
      chain.Process("stlv.C+");
// Write created histograms in file "outputfilename");
      //chain.Process("stlv.C+","outputfilename");
// Do not compile, interpret it interactively
      //chain.Process("stlv.C");
// Force recompilation of library at any time
      //chain.Process("stlv.C++");

// Open Root browser (optional; interactive use only)
      // new TBrowser;
      // gStyle->SetOptFit(1111);
      // ...

}
