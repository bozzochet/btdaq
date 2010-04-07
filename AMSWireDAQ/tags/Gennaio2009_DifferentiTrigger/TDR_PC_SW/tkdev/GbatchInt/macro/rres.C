//  $Id: rres.C,v 1.1 2008/01/21 21:58:35 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  rres.C
///\brief Make a figure of rigidity resolution comparison
///
/// between gbatch and TRKdev
///
///\date  2007/11/29 SH  First test version
///$Date: 2008/01/21 21:58:35 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

void rres(Int_t mode)
{
  if (mode != 1 && mode != 2) {
    cout << "mode should be 1 or 2" << endl;
    return;
  }

  Float_t rgt[8] = { 1, 3, 10, 30, 100, 300, 1000, 3000 };

  TGraphErrors *gr = new TGraphErrors;

  for (Int_t i = 0; i < 8; i++) {
    TH1D *hist = 0;

    if (mode == 1) 
      hist = rres(Form("root/g3tktest.root.%04.0f", (Double_t)rgt[i]));

    if (mode == 2) {
      TFile f(Form("root/gbtrk.root.%04.0f", (Double_t)rgt[i]));
      if (f.IsOpen()) hist = (TH1D *)f.Get("hist1");
    }
    if (!hist) continue;

    gROOT->cd();
    hist->Clone(Form("hist%d",i+1));

    Double_t range = hist->GetRMS()*10;
    hist->Fit("gaus", "q0", "", -range, range);
    TF1 *func = hist->GetFunction("gaus");
    gr->SetPoint(i, rgt[i], func->GetParameter(2)*rgt[i]);
    gr->SetPointError(i, 0, func->GetParError(2));
  }

  gStyle->SetOptLogx();
  gStyle->SetOptLogy();

  gr->SetMarkerStyle(20);
  gr->Draw("alp");

  TF1 *func = new TF1("func", "[0]*x");
  gr->Fit(func, "q0", "", 200, 5000);
  cout << Form("MDR= %4.2f TV", 1e-3/func->GetParameter(0)) << endl;

  TFile of(Form("root/rres%d.root", mode), "recreate");
  gr->Write("gr");
  gROOT->GetList()->Write();
}

TH1D *rres(const char *fname)
{
  TFile f(fname);
  if (!f.IsOpen()) return;

  TTree *tree = (TTree *)f.Get("h1");
  if (!tree) return;

  gROOT->cd();
  gROOT->Delete("htmp");
  TH1D *htmp = new TH1D("htmp", "d(1/R)", 1000, -0.1, 0.1);

  tree->Draw("1/Rigidity[0]-1/momentum>>htmp", "nhits[0]>=7", "goff");

  return htmp;
}


void rres(void)
{
  TFile f1("root/rres1.root"); if (!f1.IsOpen()) return;
  TFile f2("root/rres2.root"); if (!f2.IsOpen()) return;

  TGraphErrors *gr1 = (TGraphErrors *)f1.Get("gr");
  TGraphErrors *gr2 = (TGraphErrors *)f2.Get("gr");
  if (!gr1 || !gr2) return;

  gROOT->cd();

  TString pname = "rres.ps";

  TCanvas *c1 = new TCanvas;
  c1->SetLogx();
  c1->SetLogy();

  c1->SaveAs(pname+"[");

  TH1F *axis = new TH1F("axis", "Rigidity resolution", 100, 0.5, 5000);
  axis->SetStats(kFALSE);
  axis->SetXTitle("Input rigidity R (GV)");
  axis->SetYTitle("Rigidity resolution dR/R");
  axis->SetMinimum(1e-2);
  axis->SetMaximum(2e+0);
  axis->Draw();

  gr1->SetMarkerStyle(20); gr2->SetMarkerStyle(24); 
  gr1->SetMarkerSize(1.0); gr2->SetMarkerSize(1.2);
  gr1->SetMarkerColor(1);  gr2->SetMarkerColor(2); 
  gr1->SetLineColor(1);    gr2->SetLineColor(2);
  
  gr1->Draw("lp");
  gr2->Draw("lp");

  TLegend *lgd = new TLegend(0.20, 0.50, 0.40, 0.65);
  lgd->AddEntry(gr1, "Gbatch", "lp");
  lgd->AddEntry(gr2, "TRKdev", "lp");
  lgd->Draw();

  c1->SaveAs(pname);


  c1->Clear();
  c1->SetLogx(0);
  c1->SetLogy(0);

  TH1D *hist11 = (TH1D *)f1.Get("hist1")->Clone("hist11");
  TH1D *hist21 = (TH1D *)f2.Get("hist1")->Clone("hist21");
  hist11->Rebin(10); 
  hist21->Rebin(10); 

  hist11->SetTitle("1/R_{rec}-1/R_{ini} (R_{ini}=1GV)");
  hist11->SetXTitle("1/R_{rec}-1/R_{ini} (GV^{-1})");
  hist11->SetStats(kFALSE);

  hist21->SetLineColor(2);
  hist11->Draw();
  hist21->Draw("same");
  hist11->SetMaximum(hist21->GetMaximum()*1.05);

  TLegend *lgd = new TLegend(0.20, 0.50, 0.35, 0.62);
  lgd->AddEntry(hist11, "Gbatch", "l");
  lgd->AddEntry(hist21, "TRKdev", "l");
  lgd->Draw();

  c1->SaveAs(pname);

  c1->Clear();
  c1->SetLogx(0);
  c1->SetLogy(0);

  TH1D *hist15 = (TH1D *)f1.Get("hist5")->Clone("hist15");
  TH1D *hist25 = (TH1D *)f2.Get("hist5")->Clone("hist25");
  hist15->GetXaxis()->SetRangeUser(-0.005, 0.005);
  hist25->GetXaxis()->SetRangeUser(-0.005, 0.005);

  hist15->SetTitle("1/R_{rec}-1/R_{ini} (R_{ini}=100GV)");
  hist15->SetXTitle("1/R_{rec}-1/R_{ini} (GV^{-1})");
  hist15->SetStats(kFALSE);

  hist25->SetLineColor(2);
  hist15->Draw();
  hist25->Draw("same");

  TLegend *lgd = new TLegend(0.20, 0.50, 0.35, 0.62);
  lgd->AddEntry(hist11, "Gbatch", "l");
  lgd->AddEntry(hist21, "TRKdev", "l");
  lgd->Draw();

  c1->SaveAs(pname);

  c1->SaveAs(pname+"]");
}
