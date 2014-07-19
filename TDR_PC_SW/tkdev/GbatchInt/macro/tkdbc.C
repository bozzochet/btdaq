
#include "TkDBc.h"
#include "TkLadder.h"
#include "TkCoo.h"

#include "TFile.h"
#include "TTree.h"

void tkdbc(const char *fname = "tkdbc.dat")
{
  TkDBc::CreateTkDBc();
  TkDBc::Head->init((char*)fname);

  TString sfn = fname;
  sfn.ReplaceAll("dat", "root");

  TFile of(sfn, "recreate");
  TTree *tree = new TTree("tree", "tkdbc");

  Int_t tkid;
  Double_t data[30];

  tree->Branch("tkid", &tkid, "tkid/I");
  tree->Branch("data", data, 
	       "pos[3]/D:rot[3][3]/D:posA[3]/D:rotA[3][3]/D:"
	       "posG0[3]/D:posG1[3]/D");

  for (Int_t i = 0; i < 5; i++) {
    TkPlane *pl = TkDBc::Head->_layd[i];
    Double_t al, bt, gm;
    pl->rotA.GetRotAngles(al, bt, gm);
    cout << Form("Plane %d %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f", 
		 i+1, pl->posA[0]*10, pl->posA[1]*10, pl->posA[2]*10,
		 al*600, bt*600, gm*600) << endl;
  }

  for (Int_t i = 0; i < TkDBc::Head->GetEntries(); i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(i);
    if (!lad) continue;

    tkid = lad->GetTkId();

    Double_t x0 = 0;
    Double_t x1 = (lad->_nsensors-1)*TkDBc::Head->_SensorPitchK
                                    +TkDBc::Head->_ssize_active[0];

    AMSPoint coo0 = TkCoo::GetGlobalA(tkid, x0, 0);
    AMSPoint coo1 = TkCoo::GetGlobalA(tkid, x1, 0);

    for (Int_t j = 0; j < 3; j++) {
      data[j]    = lad->pos [j];
      data[j+12] = lad->posA[j];
      for (Int_t k = 0; k < 3; k++) {
	data[j*3+k+ 3] = lad->rot .GetEl(j, k);
	data[j*3+k+15] = lad->rotA.GetEl(j, k);
      }
      data[j+24] = coo0[j];
      data[j+27] = coo1[j];
    }

    tree->Fill();
  }

  tree->Write();
}
