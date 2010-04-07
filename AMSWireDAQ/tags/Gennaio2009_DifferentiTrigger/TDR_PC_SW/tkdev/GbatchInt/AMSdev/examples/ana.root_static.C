#include <root.h>
#include "TFile.h"
#include "TH1F.h"
/* NEEDED FOR CASTOR FILES 
#include "TRFIOFile.h"
*/


int main(){

      AMSChain ams;
      ams.Add("/f2users/choutko/g3v1g3.root");

      // THIS SHOULD WORK NICELY WITH Web files:
      //ams.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

      /* NEEDED FOR CASTOR FILES
      char chfile[256] = "rfio:/castor/cern.ch/ams/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root";
      new TRFIOFile(chfile);
      ams.Add(chfile);
      */

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

      hfile->Write();
      list.Write("select.list");

      printf("We have processed %d events\n", ndata);
      printf("Histograms saved in '%s'\n", hfile->GetName());
}
