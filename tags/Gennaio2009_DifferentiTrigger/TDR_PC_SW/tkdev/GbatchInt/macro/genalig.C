
#include "TkDBc.h"
#include "TkCoo.h"
#include "TRandom.h"

Int_t check(Int_t tkid, AMSPoint coo);

void genalig(Int_t seed)
{
  gRandom->SetSeed(seed);

  // Initialize TKDBc
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();

  // All the units are in mm
  // Random displacement/rotation for each plane
  Double_t pdx = 0.200;
  Double_t pdy = 0.200;
  Double_t pdz = 0.200;
  Double_t pda = 0.200;   // Alpha (dy/dx), dy(mm)/(dx=60cm)
  Double_t pdb = 0.200;   // Beta  (dz/dx), dz(mm)/(dx=60cm)
  Double_t pdg = 0.200;   // Gamma (dz/dy), dz(mm)/(dy=60cm)

  for (Int_t i = 0; i < 5; i++) {
    TkPlane *pl = TkDBc::Head->GetPlane(i+1);
    
    Double_t al = 0, bt = 0, gm = 0;
    if (100 <= seed && seed < 300) {
      pl->posA[0] = 0;
      pl->posA[1] = 0;
      pl->posA[2] = 0;
    }
    else if (300 <= seed && seed < 400) {
      pl->posA[0] = 0;
      pl->posA[1] = 0;
      pl->posA[2] = 0;

      if (i == 2) {
	if ((seed%100)/10 == 0) pl->posA[0] = 0.5*0.1;
	if ((seed%100)/10 == 1) pl->posA[1] = 0.5*0.1;
	if ((seed%100)/10 == 2) pl->posA[2] = 0.8*0.1;
	if ((seed%100)/10 == 3) al = 0.8/600;
	if ((seed%100)/10 == 4) bt = 0.8/600;
	if ((seed%100)/10 == 5) gm = 0.8/600;
      }
      if ((seed%100)/10 == 6) pl->posA[0] = 1; // cm
      if ((seed%100)/10 == 7) pl->posA[1] = 1; // cm
      if ((seed%100)/10 == 8) pl->posA[2] = 1; // cm
    }
    else if (500 <= seed && seed < 600) {
      pl->posA[0] = 0;
      pl->posA[1] = 0;
      pl->posA[2] = 0;
    }
    else {
      pl->posA[0] = gRandom->Gaus()*pdx*0.1;
      pl->posA[1] = gRandom->Gaus()*pdy*0.1;
      pl->posA[2] = gRandom->Gaus()*pdz*0.1;
      al = gRandom->Gaus()*pda/600;
      bt = gRandom->Gaus()*pdb/600;
      gm = gRandom->Gaus()*pdg/600;
    }

    pl->rotA.SetRotAngles(al, bt, gm);

    cout << Form("Plane     %d %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f", i+1,
		 pl->posA[0]*10, pl->posA[1]*10, pl->posA[2]*10, 
		 al*600, bt*600, gm*600)
	 << endl;
  }
  cout << endl;

  // All the units are in mm
  // Random displacement/rotation for each ladder
  Double_t ldx = 0.150;
  Double_t ldy = 0.075;
  Double_t ldz = 0.200;
  Double_t lda = 0.150;   // Alpha (dy/dx), dy(mm)/(dx=60cm)
  Double_t ldb = 0.150;   // Beta  (dz/dx), dz(mm)/(dx=60cm)
  Double_t ldg = 0.150;   // Gamma (dz/dy), dz(mm)/(dy=60cm)

  Int_t nlad = TkDBc::Head->GetEntries();
  for (Int_t i = 0; i < nlad; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(i);
    if (!lad) continue;

    Double_t al = 0, bt = 0, gm = 0;
    if (100 <= seed && seed < 200) {
      lad->posA[0] = 0;
      lad->posA[1] = 0;
      lad->posA[2] = 0;
	
      if (lad->GetLayer() == 4) {
	if ((seed%100)/10 == 0) lad->posA[0] = 0.5*0.1;
	if ((seed%100)/10 == 1) lad->posA[1] = 0.5*0.1;
	if ((seed%100)/10 == 2) lad->posA[2] = 0.8*0.1;
	if ((seed%100)/10 == 3) al = 0.8/600;
	if ((seed%100)/10 == 4) bt = 0.8/600;
	if ((seed%100)/10 == 5) gm = 0.8/600;
      }
    }
    else if (200 <= seed && seed < 300) {
      lad->posA[0] = 0;
      lad->posA[1] = 0;
      lad->posA[2] = 0;
	
      if (lad->GetLayer() == 3) {
	if ((seed%100)/10 == 0) lad->posA[0] = 0.5*0.1;
	if ((seed%100)/10 == 1) lad->posA[1] = 0.5*0.1;
	if ((seed%100)/10 == 2) lad->posA[2] = 0.8*0.1;
	if ((seed%100)/10 == 3) al = 0.8/600;
	if ((seed%100)/10 == 4) bt = 0.8/600;
	if ((seed%100)/10 == 5) gm = 0.8/600;
      }
    }
    else if (300 <= seed && seed < 400) {
      lad->posA[0] = 0;
      lad->posA[1] = 0;
      lad->posA[2] = 0;
    }
    else if (500 <= seed && seed < 600) {
      lad->posA[0] = 0;
      lad->posA[1] = 0;
      lad->posA[2] = gRandom->Gaus()*0.3*0.1;
    }
    else {
      lad->posA[0] = gRandom->Gaus()*ldx*0.1;
      lad->posA[1] = gRandom->Gaus()*ldy*0.1;
      lad->posA[2] = gRandom->Gaus()*ldz*0.1;
      al = gRandom->Gaus()*lda/600;
      bt = gRandom->Gaus()*ldb/600;
      gm = gRandom->Gaus()*ldg/600;
    }

    lad->rotA.SetRotAngles(al, bt, gm);

    cout << Form("Ladder %4d %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f",
		 lad->GetTkId(),
		 lad->posA[0]*10, lad->posA[1]*10, lad->posA[2]*10, 
		 al*600, bt*600, gm*600)
	 << endl;
  }
  cout << endl;


  for (Int_t i = 0; i < nlad; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(i);
    if (!lad) continue;

    Int_t tkid = lad->GetTkId();
    Double_t ee = 1e-10;
    Double_t sx = 190*TkDBc::Head->_PitchK5
      +(lad->_nsensors-1)*TkDBc::Head->_SensorPitchK;

    Double_t sy = 641*TkDBc::Head->_PitchS;
    AMSPoint coon = TkCoo::GetGlobalN(tkid, ee, ee);
    AMSPoint coo0 = TkCoo::GetGlobalA(tkid, ee, ee);
    AMSPoint coo1 = TkCoo::GetGlobalA(tkid, sx, ee);
    AMSPoint coo2 = TkCoo::GetGlobalA(tkid, ee, sy);

    check(tkid, coo0);
    check(tkid, coo1);
    check(tkid, coo2);

    Double_t al = -(coo1[1]-coo0[1])/(coo1[0]-coo0[0]);  // dY/dX
    Double_t bt =  (coo1[2]-coo0[2])/(coo1[0]-coo0[0]);  // dZ/dX
    Double_t gm = -(coo2[2]-coo0[2])/(coo2[1]-coo0[1]);  // dZ/dY

    cout << Form("Global %4d %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f", tkid,
		 (coo0[0]-coon[0])*10, (coo0[1]-coon[1])*10, 
		 (coo0[2]-coon[2])*10, al*600, bt*600, gm*600)
	 << endl;
  }

  TkDBc::Head->write(Form("tkdbc.dat.da%03d", seed));
}


#include "TkSens.h"
#include "TrCluster.h"

Int_t check(Int_t tkid, AMSPoint coo)
{
  TkSens tks(tkid, coo);
  Int_t sen = tks.GetSensor();
  Int_t sx  = tks.GetStripX()+640;
  Int_t sy  = tks.GetStripY();

  Double_t ipx = tks.GetImpactPointX();
  Double_t ipy = tks.GetImpactPointY();


  int layer = abs(tkid)/100;
  int nchx  = (layer == 1 || layer == 8) ? TkDBc::Head->_NReadStripK7 
                                         : TkDBc::Head->_NReadStripK5;
  Int_t idx;
  if (layer == 1 || layer == 8) {
    int nad = TkDBc::Head->_NReadoutChanK;
    idx = ((sen+1)*nchx-(sx-640))/nad;
  }
  else idx = sen/2;

  TrCluster clx(tkid, 0, sx, 0, 0); clx.push_back(10,1,0);
  TrCluster cly(tkid, 1, sy, 0, 0); cly.push_back(10,1,0);

  Double_t lx = clx.GetXEta(idx);
  Double_t ly = cly.GetXEta(idx);

  AMSPoint prec = TkCoo::GetGlobalA(tkid, lx, ly);

  Int_t diff = 0;
  Double_t dd = 0;
  if (TMath::Abs(prec[0]-coo[0]) > 1e-6) { diff += 1; dd = prec[0]-coo[0]; }
  if (TMath::Abs(prec[1]-coo[1]) > 1e-6) { diff += 2; dd = prec[1]-coo[1]; }
  if (TMath::Abs(prec[2]-coo[2]) > 1e-6) { diff += 4; dd = prec[2]-coo[2]; }

  if (diff) {
  cout << Form("Diff %4d %d | %11.8f | %3d %3d | %5.3f %5.3f | "
	       "%8.4f %8.4f | %8.4f %8.4f | %8.4f %8.4f",
	       tkid, diff, dd, sx, sy, lx, ly,
	       coo[0], prec[0], coo[1], prec[1], coo[2], prec[2]) << endl; 
  }

  return diff;
}
