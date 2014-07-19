{
gROOT->Reset();

// for linux load
gSystem->Load("/offline/vdev/lib/linux/ntuple.so");
//
//  for dunix aka ams.cern.ch load 
//  gSystem->Load("/offline/vdev/lib/osf1/ntuple.so");
//
TChain chain("AMSRoot");
chain.Add("g3v1g3.root");
chain.Draw("ev.trtrack_rigidity(0)", "ev.trtrack_rigidity(0)>0");
}


