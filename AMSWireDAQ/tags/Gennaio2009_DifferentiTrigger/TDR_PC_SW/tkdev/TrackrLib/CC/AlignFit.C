// $Id: AlignFit.C,v 1.3 2008/05/22 20:30:52 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  AlignFit.C
///\brief Source file of AlignFit
///
///\date  2007/04/02 SH  First test version
///$Date: 2008/05/22 20:30:52 $
///
///$Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////////

#include "AlignFit.h"
#include "TkDBc.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TMath.h"
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>

TObjArray AlignObject::sharray;
Int_t AlignObject::ProfMode = 0;

AlignObject::AlignObject(Int_t id) : tkid(id)
{
  for (Int_t i = 0; i < Nalg; i++) fixp[i] = 0;
}

AlignObject::~AlignObject()
{
}

void AlignObject::Reset(void)
{
  for (Int_t i = 0; i < Ndim; i++) param[i] = 0;
  phi = pitch = roll = 0;
}

void AlignObject::InitSHists(void)
{
  sharray.Clear();
  sharray.Add(new TH1F("hist1", "Fitpar (X)",     256, 0.5, 256.5));
  sharray.Add(new TH1F("hist2", "Fitpar (Y)",     256, 0.5, 256.5));
  sharray.Add(new TH1F("hist3", "Fitpar (Z)",     256, 0.5, 256.5));
  sharray.Add(new TH1F("hist4", "Fitpar (dY/dX)", 256, 0.5, 256.5));
  sharray.Add(new TH1F("hist5", "Fitpar (dZ/dX)", 256, 0.5, 256.5));
  sharray.Add(new TH1F("hist0", "X-Scale",        256, 0.5, 256.5));
  sharray.Add(new TH3F("histx", "Residual VS angle (X)", 
		       256, 0.5, 256.5, 20, -1, 1, 100, -1, 1));
  sharray.Add(new TH3F("histy", "Residual VS angle (Y)", 
		       256, 0.5, 256.5, 20, -1, 1, 100, -1, 1));
  sharray.Add(new TH3F("hists", "Residual (X) VS Sensor", 
		       256, 0.5, 256.5, 15, 0, 15, 100, -1, 1));
  sharray.Add(new TH3F("histp", "Residual (Y) VS Sensor", 
		       256, 0.5, 256.5, 15, 0, 15, 100, -1, 1));
}

void AlignObject::InitHists(TDirectory *dir)
{
  if (harray.GetEntries() > 0) {
    Int_t nh = harray.GetEntries();
    for (Int_t i = 0; i < nh; i++)
      ((TH1 *)harray.At(i))->Reset();
    return;
  }

  ldir = dir;
  ldir->cd();

  Int_t slot = TMath::Abs(tkid)%100;
  Int_t sign = (tkid < 0) ? -1 : 1;
  cdir = new TDirectory(Form("dir%c%02d", (sign > 0) ? 'p' : 'n', slot),
			Form("TkId %d", tkid));
  cdir->cd();

  harray.Add(new TH2F("hist001", "dX VS X",     30, -60, 60, 100, -1, 1));
  harray.Add(new TH2F("hist002", "dY VS X",     30, -60, 60, 100, -1, 1));
  harray.Add(new TH2F("hist003", "dX VS dX/dZ", 20,  -1,  1, 100, -1, 1));
  harray.Add(new TH2F("hist004", "dY VS dY/dZ", 20,  -1,  1, 100, -1, 1));
  harray.Add(new TH1F("hist005", "dZ(X) VS X",  30, -60, 60));
  harray.Add(new TH1F("hist006", "dZ(Y) VS X",  30, -60, 60));

  TH2F *hist1 = (TH2F *)harray.At(0);
  for (Int_t i = 0; i < 30; i++) {
    Double_t x1 = hist1->GetXaxis()->GetBinLowEdge(i+1);
    Double_t x2 = hist1->GetXaxis()->GetBinLowEdge(i+2);
    harray.Add(new TH2F(Form("hist%02d1", i+1), 
			Form("dX VS dX/dZ (%.0f<X<%.0f)", x1, x2), 
			20, -1, 1, 100, -1, 1));
    harray.Add(new TH2F(Form("hist%02d2", i+1), 
			Form("dY VS dX/dZ (%.0f<X<%.0f)", x1, x2), 
			20, -1, 1, 100, -1, 1));
  }
}

void AlignObject::ResetHists(void)
{
  Int_t nh = harray.GetEntries();
  for (Int_t i = 0; i < nh; i++)
    ((TH1 *)harray.At(i))->Reset();
}

void AlignObject::FillHist(Int_t i, Double_t x, Double_t y)
{
  TH2F *hist = (TH2F*)harray.At(i);
  if (hist) hist->Fill(x, y);
}

void AlignObject::ProcHists(Int_t bits)
{
  cdir->cd();

  Double_t par[6], per[6];
  for (Int_t i = 0; i < 6; i++) par[i] = per[i] = 0;

  TH1F *hdzx = (TH1F *)harray.At(4);
  TH1F *hdzy = (TH1F *)harray.At(5);
  Int_t nfit = 0;

  Int_t nh = harray.GetEntries();
  for (Int_t i = 0; i < nh; i++) {
    if (i == 4 || i == 5) continue;

    TH2F *hist = (TH2F *)harray.At(i);
    if (hist->GetSumOfWeights() < 100) continue;

    TH1D *prof = Profile(hist);
    Int_t npt = 0;
    for (Int_t j = 0; j < prof->GetNbinsX(); j++)
      if (prof->GetBinError(j+1) > 0) npt++;
    if (npt < 2) continue;

    prof->Fit("pol1", "q0");
    TF1 *func = prof->GetFunction("pol1");

    if (i == 0) {
      if (bits & XYcorr1) {
	par[0] = -func->GetParameter(0);
	per[0] =  func->GetParError (0);
      }
      else if (bits & XYcorr3)
	SigmaProj(hist, par[0], per[0]);

      par[5] = func->GetParameter(1);
      per[5] = func->GetParError (1);
    }

    else if (i == 1) {
      if (bits & XYcorr1) {
	par[1] = -func->Eval(cool[0]);
	per[1] =  func->GetParError (0);
      }
      else if (bits & XYcorr3)
	SigmaProj(hist, par[1], per[1]);

      if (bits & dYXcorr) {
	par[3] = -func->GetParameter(1);
	per[3] =  func->GetParError (1);
      }
    }

    else if (i == 2) {
      if (bits & XYcorr2) {
	par[0] = -func->GetParameter(0);
	per[0] =  func->GetParError (0);
      }
      if (bits & Zcorr1) {
	par[2] = func->GetParameter(1);
	per[2] = func->GetParError (1);
      }
    }

    else if (i == 3) {
      if (bits & XYcorr2) {
	par[1] = -func->GetParameter(0);
	per[1] =  func->GetParError (0);
      }
      if (bits & Zcorr2) {
	par[2] = func->GetParameter(1);
	per[2] = func->GetParError (1);
      }
    }

    else {
      TH1F *hdz = (i%2 == 0) ? hdzx : hdzy;
      hdz->SetBinContent((i-6)/2+1, func->GetParameter(1));
      hdz->SetBinError  ((i-6)/2+1, func->GetParError (1));
      if (i%2 == 1) nfit++;
    }
  }

  if (nfit > 2 && (bits & dZXcorr)) {
    hdzy->Fit("pol1", "q0");
    TF1 *func2 = hdzy->GetFunction("pol1");
    par[4] = -func2->GetParameter(1);
    per[4] =  func2->GetParError (1);
    par[2] =  func2->Eval(cool[0]);
    per[2] =  func2->GetParError(0);
  }

  Int_t ly = TMath::Abs(tkid)/100;
  Int_t sl = tkid%100;
  Int_t id = (ly-1)*32+sl+16-((tkid > 0) ? 1 : 0);

  for (Int_t i = 0; i < 6; i++) {
    Double_t factor = (i >= 3) ? 600 : 1;
    TH1F *hist = (TH1F *)sharray.At(i);
    hist->SetBinContent(id, par[i]*factor);
    hist->SetBinError  (id, per[i]*factor);

    if (i < 3 && fixp[i] == 1) param[i] += par[i]*0.1;
  }
  if (fixp[4] == 1) phi   += par[3]*0.1;
  if (fixp[3] == 1) pitch += par[4]*0.1;
  if (fixp[5] == 1) roll  += par[5]*0.1;
}

void AlignObject::SigmaProj(TH2 *hist, Double_t &par, Double_t &err)
{
  par = err = 0;

  TH1D *hprj = hist->ProjectionY("hprj");

  Double_t nmin = 100;

  if (ProfMode == 1) {
    Int_t nx = hprj->GetNbinsX();

    Double_t *hx = new Double_t[nx];
    Double_t *hy = new Double_t[nx];
    Double_t *hw = new Double_t[nx];

    Int_t neff = 0;
    Double_t sum = 0;
    for (Int_t i = 0; i < nx; i++) {
      Double_t c = hprj->GetBinContent(i+1);
      hx[i] = hprj->GetBinCenter(i+1);
      hy[i] = hw[i] = 0;

      if (c > 10) {
	Double_t lc =  TMath::Log(c);
	Double_t er = (TMath::Log(c+TMath::Sqrt(c))-lc)/c;
	hy[i] = lc;
	hw[i] = 1/er/er;
	sum += c;
	neff++;
      }
    }

    if (sum > nmin && neff > 3) {
      Double_t pp[3];
      Fit2D(nx, hy, hx, hw, pp);

      if (pp[2] < 0) {
	Double_t gpar1 = -pp[1]/pp[2]/2;
	Double_t gpar2 = TMath::Sqrt(-0.5/pp[2]);
	Double_t gerr1 = gpar2/TMath::Sqrt(sum);
	par = -gpar1;
	err =  gerr1;
      }
    }

    delete [] hx;
    delete [] hy;
    delete [] hw;
  }

  else if (ProfMode == 2) {
    if (hprj->GetSumOfWeights() > nmin) {
      hprj->Fit("gaus", "q0");
      TF1 *func = hprj->GetFunction("gaus");
      par = -func->GetParameter(1);
      err =  func->GetParError (1);
    }
  }

  delete hprj;
}

TH1D *AlignObject::Profile(TH2 *hist)
{
  TString hname = hist->GetName();
  hname.ReplaceAll("hist", "prof");
  if (hname == "hist") hname += "_pf";

  TH1D *prof = hist->ProjectionX(hname);
  TH1D *htmp = 0;
  Int_t nmin = 50;

  prof->Reset();

  Int_t nx = hist->GetNbinsX();
  Int_t ny = hist->GetNbinsY();

  Double_t *hx = new Double_t[ny];
  Double_t *hy = new Double_t[ny];
  Double_t *hw = new Double_t[ny];

  for (Int_t i = 0; i < nx; i++) {
    Int_t neff = 0;
    Double_t sum = 0;
    for (Int_t j = 0; j < ny; j++) {
      hx[j] = hist->GetYaxis()->GetBinCenter(j+1);
      hy[j] = hw[j] = 0;

      Double_t c = hist->GetBinContent(i+1, j+1);
      sum += c;

      if (c > 10) {
	Double_t lc =  TMath::Log(c);
	Double_t er = (TMath::Log(c+TMath::Sqrt(c))-lc)/c;
	hy[j] = lc;
	hw[j] = 1/er/er;
	neff++;
      }
    }

    Double_t par = 0, per = 0;
    if (sum > nmin && neff > 3) {

      // Fast gaussian fit
      if (ProfMode == 1) {
	Double_t pp[3];
	Fit2D(ny, hy, hx, hw, pp);

	if (pp[2] < 0) {
	  Double_t gpar1 = -pp[1]/pp[2]/2;
	  Double_t gpar2 = TMath::Sqrt(-0.5/pp[2]);
	  Double_t gerr1 = gpar2/TMath::Sqrt(sum);
	  par = gpar1;
	  per = gerr1;
	}
      }

      // Normal fit (slow and memory consumption)
      else if (ProfMode == 2) {
	htmp = hist->ProjectionY("htmp", i+1, i+1);
	htmp->Fit("gaus", "q0");
	TF1 *func = htmp->GetFunction("gaus");
	par = func->GetParameter(1);
	per = func->GetParError (1);
      }
    }

    if (sum > nmin/10 && par == 0 && per == 0) {
      htmp = hist->ProjectionY("htmp", i+1, i+1);
      // Simple mean
      if (htmp->GetMeanError() > 1e-4) {
	par = htmp->GetMean();
	per = htmp->GetMeanError();
      }
    }

    prof->SetBinContent(i+1, par);
    prof->SetBinError  (i+1, per);
  }
  prof->SetEntries(hist->GetSumOfWeights());

  delete htmp;

  delete [] hx;
  delete [] hy;
  delete [] hw;

  return prof;
}

Int_t AlignObject::Fit2D(Int_t n, Double_t *x, Double_t *y, 
	  		          Double_t *w, Double_t *par)
{
  // Analytic parabola fitting method

  // check number of hits
  if (n < 4) return -1;

  Int_t np = 3;

  Double_t mtx[3][3], vec[3];
  for (Int_t i = 0; i < 9; i++) mtx[i/3][i%3] = vec[i/3] = 0;

  for (Int_t i = 0; i < n; i++) {
    Double_t m0 = (w) ? w[i] : 1;
    for (Int_t j = 0; j < np; j++) {
      Double_t m = m0;
      for (Int_t k = j; k < np; k++) {
	mtx[j][k] += m;
	if (j == 0) vec[k] += m*x[i];
	m *= y[i];
      }
      m0 *= y[i]*y[i];
    }
  }
  mtx[1][0] = mtx[0][1];
  mtx[2][0] = mtx[0][2];
  mtx[2][1] = mtx[1][2];

  if (Inv3x3(mtx) < 0) return -1;

  for (Int_t i = 0; i < np; i++)
    par[i] = mtx[i][0]*vec[0]+mtx[i][1]*vec[1]+mtx[i][2]*vec[2];
  return 0;
}

Int_t AlignObject::Inv3x3(Double_t mtx[3][3])
{
  Double_t pM[9];
  for (Int_t i = 0; i < 9; i++) pM[i] = mtx[i/3][i%3];

  const Double_t c00 = pM[4] * pM[8] - pM[5] * pM[7];
  const Double_t c01 = pM[5] * pM[6] - pM[3] * pM[8];
  const Double_t c02 = pM[3] * pM[7] - pM[4] * pM[6];
  const Double_t c10 = pM[7] * pM[2] - pM[8] * pM[1];
  const Double_t c11 = pM[8] * pM[0] - pM[6] * pM[2];
  const Double_t c12 = pM[6] * pM[1] - pM[7] * pM[0];
  const Double_t c20 = pM[1] * pM[5] - pM[2] * pM[4];
  const Double_t c21 = pM[2] * pM[3] - pM[0] * pM[5];
  const Double_t c22 = pM[0] * pM[4] - pM[1] * pM[3];

  const Double_t t0 = TMath::Abs(pM[0]);
  const Double_t t1 = TMath::Abs(pM[3]);
  const Double_t t2 = TMath::Abs(pM[6]);
  Double_t det;
  Double_t tmp;
  if (t0 >= t1) {
    if (t2 >= t0) {
    tmp = pM[6];
    det = c12*c01-c11*c02;
    } else {
      tmp = pM[0];
      det = c11*c22-c12*c21;
    }
  } else if (t2 >= t1) {
    tmp = pM[6];
    det = c12*c01-c11*c02;
  } else {
    tmp = pM[3];
    det = c02*c21-c01*c22;
  }

  if ( det == 0 || tmp == 0) {
    std::cout << "Inv3x3 matrix is singular" << std::endl;
    return -1;
  }

  const Double_t s = tmp/det;

  pM[0] = s*c00;
  pM[1] = s*c10;
  pM[2] = s*c20;
  pM[3] = s*c01;
  pM[4] = s*c11;
  pM[5] = s*c21;
  pM[6] = s*c02;
  pM[7] = s*c12;
  pM[8] = s*c22;

  for (Int_t i = 0; i < 9; i++) mtx[i/3][i%3] = pM[i];

  return 0;
}

ClassImp(AlignFit)

TString AlignFit::ofname = "alignfit.root";

AlignFit::AlignFit(void)
{
  fArray = new Float_t[Nbuf];
  fCrray = new char[Ncbf];
  Init();

  fSigma[0] = 500e-4;
  fSigma[1] = 200e-4;

  ofile = 0;
}

AlignFit::~AlignFit()
{
  if (ofile) {
    ofile->Write();
    delete ofile;
  }

  delete [] fArray;
  delete [] fCrray;
}

void AlignFit::Init(void)
{
  fNcases = 0;
  for (Int_t i = 0; i < Nchi2; i++) 
    fXchi2[i][0] = fXchi2[i][1] = 0;

  ofile = 0;
}

void AlignFit::SetHits(Float_t arr[Ndim][Nplan], Int_t prm[Ndim][Nplan])
{
  if (fNcases >= MaxCases) return;

  for (Int_t i = 0; i < Nplan; i++) {

    if (arr[3][i] == 0) {
      for (Int_t j = 0; j < Narr; j++)
	SetArray(fNcases, i, j, 0);
      SetCrray(fNcases, i, 0);
      continue;
    }

    Int_t slot = (Int_t)arr[3][i];
    Int_t sign = (arr[4][i] == 1) ? -1 : 1;
    Int_t tkid = sign*((i+1)*100+slot);
    SetCrray(fNcases, i, slot*sign);

    AlignObject *obj = FindTkId(tkid);
    if (!obj) {
      obj = new AlignObject(tkid);
      aligmap[tkid] = obj;
    }

    for (Int_t j = 0; j < 6; j++) {
      if (j < Narr) SetArray(fNcases, i, j, arr[j][i]);
      if (j < 3) obj->cool[j] = arr[7+j][i];
      if (j < 6) obj->fixp[j] = prm[j][i];
    }
  }
  fNcases++;
}

void AlignFit::Fit(Float_t chi2m, Int_t algo, Float_t xf[2], 
		  Float_t rigmin, Int_t itermin)
{
  std::cout << " fNcases " << fNcases << std::endl;

  TStopwatch timer;
  timer.Start();
  Float_t ptime = 0;

  Double_t chi2max = chi2m;
  Double_t rmax = 0.1;
  Int_t iter = 0, miter = 20;
  Double_t chi2o = 10000000;

  if (!ofile) ofile = new TFile(ofname, "recreate");

  for (aligIT it = aligmap.begin(); it != aligmap.end(); it++) {
    AlignObject *algp = it->second;
    if (algp) algp->Reset();
  }

  gROOT->cd();
  for (Int_t i = 0; i < Nplan; i++) {
    TDirectory *ldir 
      = new TDirectory(Form("dir%d", i+1), Form("Layer %d", i+1));

    for (Int_t j = 0; j < Nlad; j++) {
      for (Int_t k = 0; k < Nhalf; k++) {
	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	AlignObject *algp = FindTkId(tkid);
	if (!algp) continue;

	algp->InitHists(ldir);
      }
    }
  }

 for (iter = 0; iter <= miter; iter++) {

  for (Int_t i = 0; i < Nplan; i++) {
    for (Int_t j = 0; j < Nlad; j++) {
      for (Int_t k = 0; k < Nhalf; k++) {
	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	AlignObject *algp = FindTkId(tkid);
	if (!algp) continue;

	algp->ResetHists();
      }
    }
  }

  ofile->cd();
  TDirectory *dir = new TDirectory(Form("iter%d", iter), 
				   Form("Iteration %d", iter));
  dir->cd();
  AlignObject::InitSHists();


  Double_t chi2a = 0, chi2;
  Int_t ntot = 0;

  for (Int_t i = 0; i < fNcases; i++) {
    Float_t res[Nplan][2];
    Double_t ch[2];

    for (Int_t l = 0; l < 2; l++) {
      Double_t x = 0, y = 0, x2 = 0, y2 = 0, nx = 0, xy = 0;
      Double_t dy[Nplan];
      Float_t z[Nplan];
      for (Int_t j = 0; j < Nplan; j++) {
	Int_t slot = GetCrray(i, j);
	if (slot == 0) continue;

	Int_t sign = TMath::Sign(1, slot);
	Int_t tkid = sign*((j+1)*100+TMath::Abs(slot));

	AlignObject *algp = FindTkId(tkid);
	if (!algp) {
	  std::cout << "WARNING algp not found: " << tkid << std::endl;
	  continue;
	}

	Double_t sy = TMath::Sin(algp->phi);
	Double_t cy = TMath::Cos(algp->phi);
	Double_t sp = TMath::Sin(algp->pitch);
	Double_t cp = TMath::Cos(algp->pitch);
	Double_t sr = TMath::Sin(algp->roll);
	Double_t cr = TMath::Cos(algp->roll);
	Double_t mtx[3][3];
	mtx[0][0]=cy*cp;
	mtx[1][0]=-sy;
	mtx[2][0]=cy*sp;
	mtx[0][1]=cr*sy*cp-sr*sp;
	mtx[1][1]=cr*cy;
	mtx[2][1]=cr*sy*sp+sr*cp;
	mtx[0][2]=-sr*sy*cp-cr*sp;
	mtx[1][2]=-sr*cy;
	mtx[2][2]=-sr*sy*sp+cr*cp;

	Double_t d[2];
	Double_t s1 = GetArray(i,j,0)-algp->cool[0];
	Double_t s2 = GetArray(i,j,1)-algp->cool[1];
	Double_t d3 = mtx[0][2]*s1+mtx[1][2]*s2;
	Double_t s3 = GetArray(i,j,2)-algp->cool[2]+d3;
	d [0] = mtx[0][0]*s1+mtx[1][0]*s2+mtx[2][0]*s3;
	d [1] = mtx[0][1]*s1+mtx[1][1]*s2+mtx[2][1]*s3;
	z [j] = GetArray(i,j,2)+algp->param[2]+d3;
	dy[j] = d[l]+algp->cool[l]+algp->param[l];

	y  = y +dy[j];
	xy = xy+dy[j]*z [j];
	y2 = y2+dy[j]*dy[j];
	x  = x +z [j];
	x2 = x2+z [j]*z [j];
	nx = nx+1;
      }

      if (nx > Nplan-3) {
	chi2 = 0;
	x  =  x/nx;
	y  =  y/nx;
	xy = xy/nx;
	x2 = x2/nx;
	y2 = y2/nx;
	x2 = (x2-x*x);
	Double_t a = (xy-x*y)/x2;
	Double_t b = y-a*x;

	for (Int_t j = 0; j < Nplan; j++) {
	  res[j][l] = dy[j]-b-a*z[j];
	  if (GetCrray(i, j) != 0)
	    chi2 += res[j][l]*res[j][l]/fSigma[l]/fSigma[l];
	}
	ch[l] = chi2;

	if (l == 1 && i < Nchi2) {
	  if (iter == 0) fXchi2[i][0] = ch[0]+ch[1];
	  fXchi2[i][1] = ch[0]+ch[1];
	}
	if ((chi2 < chi2max) || iter < itermin) {
	  for (Int_t j = 0; j < Nplan; j++) {
	    Int_t slot = GetCrray(i, j);
	    if (slot == 0) continue;

	    Int_t sign = TMath::Sign(1, slot);
	    Int_t tkid = sign*((j+1)*100+TMath::Abs(slot));

	    AlignObject *algp = FindTkId(tkid);
	    if (!algp) {
	      std::cout << "WARNING algp not found: " << tkid << std::endl;
	      continue;
	    }
	    if (TMath::Abs(res[j][l]) < rmax) {
	      Double_t xh = GetArray(i, j, 0);
	      Int_t m = (Int_t)((xh+60)/60*15);
	      if (0 <= m && m < 30) {
		algp->FillHist(l,        xh, res[j][l]*10);
		algp->FillHist(l+2,       a, res[j][l]*10);
		algp->FillHist(l+(m+3)*2, a, res[j][l]*10);
	      }

	      TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	      Int_t id = j*32+slot+16-((tkid > 0) ? 1 : 0);
	      Double_t sn = -(xh-lad->pos[0])*sign/TkDBc::Head->_SensorPitchK;
	      TH3F *hist1 = (TH3F*)AlignObject::sharray.At(6+l);
	      TH3F *hist2 = (TH3F*)AlignObject::sharray.At(8+l);
	      hist1->Fill(id,  a, res[j][l]*10);
	      hist2->Fill(id, sn, res[j][l]*10);
	    }
	  }
	  ntot++;
	  chi2a += chi2;
	}
      }
    }
  }

  ifstream ftmp(Form("/proc/%d/status", gSystem->GetPid()));
  TString sbuf;
  Float_t vmsize = 0;
  while (!sbuf.ReadToken(ftmp).eof()) {
    if (sbuf == "VmSize:") {
      ftmp >> vmsize;
      break;
    }
  }

  chi2a = chi2a/ntot;
  std::cout << Form(" iter %3d %8.3f %7d %4.0f sec %5.1f MB" ,
		    iter, chi2a, ntot,
		    timer.CpuTime()-ptime, vmsize/1024) << std::endl;
  ptime = timer.CpuTime();
  timer.Continue();

  if (iter == miter) break;

  Int_t bits = 0;
  if      (iter == 0) bits = AlignObject::XYcorr2;
  else if (iter == 1) bits = AlignObject::Zcorr2;
  else if (iter == 2) bits = AlignObject::XYcorr1 | AlignObject::Zcorr2 |
			     AlignObject::dYXcorr;
  else if (iter < 15) bits = AlignObject::XYcorr1 | 
			     AlignObject::dYXcorr | AlignObject::dZXcorr;
  else                bits = AlignObject::XYcorr3;

  AlignObject::ProfMode = 1;
  if (iter == miter-1) AlignObject::ProfMode = 2;

  for (Int_t i = 0; i < Nplan; i++) {
    for (Int_t j = 0; j < Nlad; j++) {
      for (Int_t k = 0; k < Nhalf; k++) {
	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	AlignObject *algp = FindTkId(tkid);
	if (!algp) continue;

	algp->ProcHists(bits);
      }
    }
  }

  if (iter == 0) xf[0] = chi2a;
  else xf[1] = chi2a;

  chi2o = chi2a;
 }

  std::cout << "CpuTime " << timer.CpuTime() << std::endl;
}

void AlignFit::Get(Float_t arr[Ndim][8], Int_t what)
{
  for (Int_t j = what/2-1; j < Nlad; j++) {
    for (Int_t k = what%2; k < Nhalf; k++) {
      for (Int_t i = 0; i < Nplan; i++) {

	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	AlignObject *algp = FindTkId(tkid);

	for (Int_t l = 0; l < 3; l++) {
	  arr  [l][i] = (algp) ? algp->param[l] : 0;
	  arr[8+l][i] = (algp) ? algp->cool [l] : 0;
	}
	arr[3][i] = (algp) ? algp->pitch : 0;
	arr[4][i] = (algp) ? algp->phi   : 0;
	arr[5][i] = (algp) ? algp->roll  : 0;
	arr[6][i] = j+1;
	arr[7][i] = k+1;
      }
      return;
    }
  }
}
