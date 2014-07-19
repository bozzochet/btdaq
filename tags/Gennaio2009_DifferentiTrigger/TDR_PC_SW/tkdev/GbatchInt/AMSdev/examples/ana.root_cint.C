{
      gROOT->Reset();
      // for linux load
      gSystem->Load("$AMSDir/lib/linux/ntuple.so");
      //
      //  for dunix aka ams.cern.ch load 
      //  gSystem->Load("$AMSDir/lib/osf1/ntuple.so");
      //

      AMSChain ams;
      ams.Add("/f2users/choutko/g3v1g3.root");
      //ams.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");
      //ams.Add("rfio:/castor/cern.ch/ams/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

      TFile* hfile = new TFile ("amstest.root", "RECREATE"); 

      AMSEventList list;

      TH1F* hrig = new TH1F ("hrig", "Momentum (GeV)", 50, -10., 10.);
      
      int ndata = ams.GetEntries();
      for (int entry=0; entry<ndata; entry++) {
            AMSEventR *pev = ams.GetEvent();
            if (pev==NULL) break;
            for (int i=0; i<pev->nParticle(); i++) {
                  ParticleR part = pev->Particle(i);
                  hrig->Fill(part.Momentum);
                  if (pev->nVertex()>0) {
                          list.Add(pev); // Add to list of selected events
                          pev->Fill(); // write it into output ROOT file
                  }
            }
      }

      hrig->Draw();
      hfile->Write();
      list.Write("select.list");

      printf("We have processed %d events\n", ndata);
      printf("Histograms saved in '%s'\n", hfile->GetName());
}
