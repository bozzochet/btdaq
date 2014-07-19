{     
// Resetting and loading AMS library
      gROOT->Reset();
      gSystem->Load("/afs/cern.ch/exp/ams/Offline/vdev/lib/linux/ntuple.so");   
      
// Input AMS ROOT Chain
      AMSChain *ams = new AMSChain;              
      ams->Add("/f2users/choutko/g3v1g3.root");

// Initialize selection list
      AMSEventList list;
// Alternative if you have already a list
      //AMSEventList other_list("amstest.list");

// Loop to analyze entries
      int ndata = ams->GetEntries();
      for (int entry=0; entry<ndata; entry++) {
            AMSEventR* pev = ams->GetEvent();
            if (pev==NULL) break;

// Examples of use, mainly for previously created lists
            //if (other_list.Contains(pev)) continue;
            //if (pev->nBeta()>1) { other_list.Remove(pev);}

// Typical example: add to list if it satisfies your requirements
            if (pev->nBeta()==1) {
               BetaR beta = pev->Beta(0);
               if (fabs(beta.Beta-1.)>2*beta.Error) list.Add(pev);
            }
      }

      // BEGIN of block: How to write just few branches ->
      ams->SetBranchStatus("*",0); // this disables all branches by default
      ams->SetBranchStatus("ev.fHeader",1); // HeaderR class information IN
      ams->SetBranchStatus("ev.fParticle",1); // ParticleR class information IN
      // END of block

// Write selected events from chain to a new AMS ROOT file
      TFile* out_file = new TFile("amstest.root","RECREATE");
      list.Write(ams,out_file); 

//Write "run event" list to standard output
      //list.Write();

//Write "run event" list to ASCII file
      //list.Write("amstest.list");

      cout << "Events in the old tree: " << ndata << endl;
      cout << "Events in the new tree: " << list->GetEntries() << endl;
}
