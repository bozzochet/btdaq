float algpar[15][6];
float algpar_2[15][6];
float pi = 3.1415928;

void compare(char *csel, int pos) {
 
  char cnome[6][5] = { "2111", "0901", "0905", "0900", "2110", "0904" };
  char chist[30];
  char cfich[50];
  int run[2] = { 1246, 1247 };

  TFile* f[2];
  TH1D* h1[2];
  
  for (int i=0; i<2; i++) {
    sprintf(cfich,"Hist_%d_a6_1_2_6_3p_etan.root",run[i]);
    printf("cfich %s\n",cfich);
    f[i] = new TFile(cfich);
    sprintf(chist,"%s/%s_%s",cnome[pos],csel,cnome[pos]);
    printf("chist %s\n",chist);
    h1[i] = (TH1D *) f[i]->Get(chist)->Clone();
    int nent = h1[i]->GetEntries();
    h1[i]->Scale(1./nent);
    h1[i]->GetXaxis()->SetRangeUser(675.,720.);
  }

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","occupation",0,0,500,400);
  c0->SetFillColor(10);
  c0->Divide(1,2,0.0001,0.0001);
  for (int i=0; i<2; i++) {
      c0->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.12);
      gPad->SetTopMargin(0.10);
      gPad->SetLogy(1);
      h1[i]->Draw("hist");
  }

  return;
}

void eff() {
     TH1D* temp[6];
     temp[0] = (TH1D *) f->Get("xeff");
     temp[1] = (TH1D *) f->Get("yeff");
     temp[2] = (TH1D *) f->Get("xineff");
     temp[3] = (TH1D *) f->Get("yineff");
     temp[4] = (TH1D *) f->Get("xbruit");
     temp[5] = (TH1D *) f->Get("ybruit");
     temp[0]->Draw();

     float bcont[6][6];
     for (int ib=0; ib<6; ib++) 
       for (int icote=0; icote<2; icote++) {
          bcont[icote][ib] = temp[icote]->GetBinContent(ib+1);
          bcont[icote+2][ib] = temp[icote+2]->GetBinContent(ib+1);
          bcont[icote+4][ib] = temp[icote+4]->GetBinContent(ib+1);
     }
     for (int ib=0; ib<6; ib++) {
       printf("   ech %d xeff %f xineff %f xbruit %f yeff %f yineff %f ybruit %f\n",ib,
	      bcont[0][ib],bcont[2][ib],bcont[4][ib],bcont[1][ib],bcont[3][ib],bcont[5][ib]);
     }
     return;
}

void residpos_etan()
{
  const int lec_pos[6] = { 3, 1, 5, 2, 4, 0 };
  char chist[50];

  TFile *f1 = new TFile("Hist_1234_a6_1_2_6.root");
  TFile *f2 = new TFile("Hist_1234_a6_1_2_6_etan.root");
  //  TFile *f1 = new TFile("Hist_1507_a6_1_2_6.root");
  //  TFile *f2 = new TFile("Hist_1507_a6_1_2_6_etan.root");
  f1->cd();
  TH2D *f1_reseposy[6];
  TH1D *f1_reseposy_proy[6];
  for (int i=0; i<6; i++) {
    sprintf(chist,"reseposy%d",i);
    f1_reseposy[lec_pos[i]] = (TH2D*) f1->Get(chist)->Clone();
    f1_reseposy[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.015);
    f1_reseposy[lec_pos[i]]->GetXaxis()->SetLabelSize(0.040);
    f1_reseposy[lec_pos[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f1_reseposy[lec_pos[i]]->GetYaxis()->SetLabelSize(0.050);
    f1_reseposy[lec_pos[i]]->GetXaxis()->SetTitle("gap position");
    f1_reseposy[lec_pos[i]]->GetXaxis()->SetTitleSize(0.06);
    f1_reseposy[lec_pos[i]]->GetXaxis()->CenterTitle();
    f1_reseposy[lec_pos[i]]->GetYaxis()->SetTitle("#DeltaY (mm)");
    f1_reseposy[lec_pos[i]]->GetYaxis()->SetTitleSize(0.06);
    f1_reseposy[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.20);
    f1_reseposy[lec_pos[i]]->GetYaxis()->CenterTitle();
    f1_reseposy[lec_pos[i]]->GetYaxis()->SetRangeUser(-0.35,0.35);
    float nentrees = f1_reseposy[lec_pos[i]]->GetEntries();
    //    f1_reseposy[lec_pos[i]]->Scale(1./nentrees);
    f1_reseposy[lec_pos[i]]->SetLineColor(1);
    f1_reseposy[lec_pos[i]]->SetLineWidth(1);
    sprintf(chist,"reseposy%d_py",i);
    f1_reseposy_proy[lec_pos[i]] = f1_reseposy[lec_pos[i]]->ProjectionY(chist);
    f1_reseposy_proy[lec_pos[i]]->GetXaxis()->SetRangeUser(-0.25,0.25);
    f1_reseposy_proy[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.020);
    f1_reseposy_proy[lec_pos[i]]->GetYaxis()->SetTitle("Entrees");
    f1_reseposy_proy[lec_pos[i]]->GetYaxis()->SetTitleSize(0.055);
    f1_reseposy_proy[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.5);
    f1_reseposy_proy[lec_pos[i]]->GetYaxis()->CenterTitle();
  }
  TH2D *f1_resesposy[6];
  TH1D *f1_resesposy_proy[6];
  for (int i=0; i<4; i++) {
    sprintf(chist,"resesposy%d",i);
    printf(" i %d chist %s\n",i,chist);
    f1_resesposy[lec_pos[i]] = (TH2D*) f1->Get(chist)->Clone();
    f1_resesposy[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.015);
    f1_resesposy[lec_pos[i]]->GetXaxis()->SetLabelSize(0.040);
    f1_resesposy[lec_pos[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f1_resesposy[lec_pos[i]]->GetYaxis()->SetLabelSize(0.050);
    f1_resesposy[lec_pos[i]]->GetXaxis()->SetTitle("gap position");
    f1_resesposy[lec_pos[i]]->GetXaxis()->SetTitleSize(0.06);
    f1_resesposy[lec_pos[i]]->GetXaxis()->CenterTitle();
    f1_resesposy[lec_pos[i]]->GetYaxis()->SetTitle("#DeltaY (mm)");
    f1_resesposy[lec_pos[i]]->GetYaxis()->SetTitleSize(0.06);
    f1_resesposy[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.20);
    f1_resesposy[lec_pos[i]]->GetYaxis()->CenterTitle();
    f1_resesposy[lec_pos[i]]->GetYaxis()->SetRangeUser(-0.35,0.35);
    float nentrees = f1_resesposy[lec_pos[i]]->GetEntries();
    //    f1_reseposy[lec_pos[i]]->Scale(1./nentrees);
    f1_resesposy[lec_pos[i]]->SetLineColor(1);
    f1_resesposy[lec_pos[i]]->SetLineWidth(1);
    sprintf(chist,"resesposy%d_py",i);
    f1_resesposy_proy[lec_pos[i]] = f1_resesposy[lec_pos[i]]->ProjectionY(chist);
  }
  TH1D *eta_n[6], *feta_n[6]; 
  eta_n[0] = (TH1D*) f1->Get("0900/Eta_0900_K")->Clone();
  eta_n[1] = (TH1D*) f1->Get("0901/Eta_0901_K")->Clone();
  eta_n[2] = (TH1D*) f1->Get("0904/Eta_0904_K")->Clone();
  eta_n[3] = (TH1D*) f1->Get("0904/Eta_0904_K")->Clone();
  eta_n[4] = (TH1D*) f1->Get("2110/Eta_2110_K")->Clone();
  eta_n[5] = (TH1D*) f1->Get("2111/Eta_2111_K")->Clone();
  for (int i=0; i<6; i++) {
    eta_n[i]->GetXaxis()->SetLabelSize(0.065);
    eta_n[i]->GetXaxis()->SetLabelOffset(0.015);
    eta_n[i]->GetYaxis()->SetLabelSize(0.060);
    eta_n[i]->GetXaxis()->SetTitle("#eta");
    eta_n[i]->GetXaxis()->CenterTitle();
    eta_n[i]->GetXaxis()->SetTitleSize(0.10);
    eta_n[i]->GetXaxis()->SetTitleOffset(0.8);
    eta_n[i]->GetXaxis()->SetRangeUser(0.,500.);
    eta_n[i]->GetXaxis()->SetRangeUser(0.,1.);
    eta_n[i]->SetLineColor(4);
    float nent = 1./eta_n[i]->GetEntries();
    printf("i %d nent %f\n",i,nent);
    eta_n[i]->Scale(nent);
    int nbins = eta_n[i]->GetNbinsX();
    printf("nbinsx %d\n",nbins);
    float val = 0.;
    sprintf(chist,"feta_n%d",lec_pos[i]);
    feta_n[lec_pos[i]] = (TH1D*) eta_n[i]->Clone(chist);
    feta_n[lec_pos[i]]->Reset("");
    for (int ib=0; ib<nbins+1; ib++) {
      val += eta_n[i]->GetBinContent(ib);
      printf("ib %d centre %f contenu %f val %f\n",ib,eta_n[i]->GetBinCenter(ib),eta_n[i]->GetBinContent(ib),val);
      feta_n[lec_pos[i]]->SetBinContent(ib,val);
    }
    feta_n[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.015);
    feta_n[lec_pos[i]]->GetXaxis()->SetLabelSize(0.045);
    feta_n[lec_pos[i]]->GetYaxis()->SetLabelOffset(0.0025);
    feta_n[lec_pos[i]]->GetYaxis()->SetLabelSize(0.045);
    feta_n[lec_pos[i]]->GetXaxis()->SetTitle("#eta");
    feta_n[lec_pos[i]]->GetXaxis()->SetTitleSize(0.070);
    feta_n[lec_pos[i]]->GetXaxis()->CenterTitle();
    feta_n[lec_pos[i]]->GetYaxis()->SetTitle("#int_{0}^{#eta}f(#eta)");
    feta_n[lec_pos[i]]->GetYaxis()->SetTitleSize(0.0425);
    feta_n[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.60);
    feta_n[lec_pos[i]]->GetYaxis()->CenterTitle();
    feta_n[lec_pos[i]]->SetLineColor(4);
  }
  f2->cd();
  TH2D *f2_reseposy[6];
  TH1D *f2_reseposy_proy[6];
  for (int i=0; i<6; i++) {
    sprintf(chist,"reseposy%d",i);
    f2_reseposy[lec_pos[i]] = (TH2D*) f2->Get(chist)->Clone();
    f2_reseposy[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.015);
    f2_reseposy[lec_pos[i]]->GetXaxis()->SetLabelSize(0.040);
    f2_reseposy[lec_pos[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f2_reseposy[lec_pos[i]]->GetYaxis()->SetLabelSize(0.050);
    f2_reseposy[lec_pos[i]]->GetXaxis()->SetTitle("gap position");
    f2_reseposy[lec_pos[i]]->GetXaxis()->SetTitleSize(0.06);
    f2_reseposy[lec_pos[i]]->GetXaxis()->CenterTitle();
    f2_reseposy[lec_pos[i]]->GetYaxis()->SetTitle("#DeltaY (mm)");
    f2_reseposy[lec_pos[i]]->GetYaxis()->SetTitleSize(0.06);
    f2_reseposy[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.20);
    f2_reseposy[lec_pos[i]]->GetYaxis()->SetRangeUser(-0.35,0.35);
    f2_reseposy[lec_pos[i]]->GetYaxis()->CenterTitle();
    f2_reseposy[lec_pos[i]]->GetZaxis()->CenterTitle();
    float nentrees = f2_reseposy[lec_pos[i]]->GetEntries();
    //    f2_reseposy[lec_pos[i]]->Scale(1./nentrees);
    f2_reseposy[lec_pos[i]]->SetLineColor(1);
    f2_reseposy[lec_pos[i]]->SetLineWidth(1);
    sprintf(chist,"reseposy%d_py",i);
    f2_reseposy_proy[lec_pos[i]] = (TH1D*) f2_reseposy[lec_pos[i]]->ProjectionY(chist)->Clone();
    f2_reseposy_proy[lec_pos[i]]->GetXaxis()->SetRangeUser(-0.25,0.25);
    f2_reseposy_proy[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.020);
    f2_reseposy_proy[lec_pos[i]]->GetYaxis()->SetTitle("Entrees");
    f2_reseposy_proy[lec_pos[i]]->GetYaxis()->SetTitleSize(0.055);
    f2_reseposy_proy[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.5);
    f2_reseposy_proy[lec_pos[i]]->GetYaxis()->CenterTitle();
  }
  TH2D *f2_resesposy[6];
  TH1D *f2_resesposy_proy[6];
  for (int i=0; i<4; i++) {
    sprintf(chist,"resesposy%d",i);
    f2_resesposy[lec_pos[i]] = (TH2D*) f2->Get(chist)->Clone();
    f2_resesposy[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.015);
    f2_resesposy[lec_pos[i]]->GetXaxis()->SetLabelSize(0.040);
    f2_resesposy[lec_pos[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f2_resesposy[lec_pos[i]]->GetYaxis()->SetLabelSize(0.050);
    f2_resesposy[lec_pos[i]]->GetXaxis()->SetTitle("gap position");
    f2_resesposy[lec_pos[i]]->GetXaxis()->SetTitleSize(0.06);
    f2_resesposy[lec_pos[i]]->GetXaxis()->CenterTitle();
    f2_resesposy[lec_pos[i]]->GetYaxis()->SetTitle("#DeltaY (mm)");
    f2_resesposy[lec_pos[i]]->GetYaxis()->SetTitleSize(0.06);
    f2_resesposy[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.20);
    f2_resesposy[lec_pos[i]]->GetYaxis()->SetRangeUser(-0.35,0.35);
    f2_resesposy[lec_pos[i]]->GetYaxis()->CenterTitle();
    f2_resesposy[lec_pos[i]]->GetZaxis()->CenterTitle();
    float nentrees = f2_reseposy[lec_pos[i]]->GetEntries();
    //    f2_resesposy[lec_pos[i]]->Scale(1./nentrees);
    f2_resesposy[lec_pos[i]]->SetLineColor(1);
    f2_resesposy[lec_pos[i]]->SetLineWidth(1);
    sprintf(chist,"resesposy%d_py",i);
    f2_resesposy_proy[lec_pos[i]] = (TH1D*) f2_resesposy[lec_pos[i]]->ProjectionY(chist)->Clone();
    f2_resesposy_proy[lec_pos[i]]->GetXaxis()->SetRangeUser(-0.25,0.25);
    f2_resesposy_proy[lec_pos[i]]->GetXaxis()->SetLabelOffset(0.020);
    f2_resesposy_proy[lec_pos[i]]->GetYaxis()->SetTitle("Entrees");
    f2_resesposy_proy[lec_pos[i]]->GetYaxis()->SetTitleSize(0.055);
    f2_resesposy_proy[lec_pos[i]]->GetYaxis()->SetTitleOffset(1.5);
    f2_resesposy_proy[lec_pos[i]]->GetYaxis()->CenterTitle();
  }


  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","ams02 4 plaq",0,0,650,650);
  c0->SetFillColor(10);
  c0->Divide(2,2,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->SetFillStyle(4000);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.18);
  c0_1->SetBottomMargin(0.15);
  c0_1->SetRightMargin(0.15);
  c0_1->SetTopMargin(0.12);
  c0_1->SetLogz(1);
  c0_2->SetFillColor(10);
  c0_2->SetFillStyle(4000);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.18);
  c0_2->SetBottomMargin(0.15);
  c0_2->SetRightMargin(0.15);
  c0_2->SetTopMargin(0.12);
  c0_2->SetLogz(1);
  c0_3->SetFillColor(10);
  c0_3->SetFillStyle(4000);
  c0_3->Range(-10,-10,10,10);
  c0_3->SetLeftMargin(0.18);
  c0_3->SetBottomMargin(0.15);
  c0_3->SetRightMargin(0.15);
  c0_3->SetTopMargin(0.12);
  c0_3->SetLogz(1);
  c0_4->SetFillColor(10);
  c0_4->SetFillStyle(4000);
  c0_4->Range(-10,-10,10,10);
  c0_4->SetLeftMargin(0.18);
  c0_4->SetBottomMargin(0.15);
  c0_4->SetRightMargin(0.15);
  c0_4->SetTopMargin(0.12);
  c0_4->SetLogz(1);
  c0->cd(1);
  f1_reseposy[1]->Draw("colz");
  c0->cd(2);
  f2_reseposy[1]->Draw("colz");
  c0->cd(3);
  f1_reseposy[3]->Draw("colz");
  c0->cd(4);
  f2_reseposy[3]->Draw("colz");

  TCanvas* c02=new TCanvas("c02","ams02 4 plaq",0,0,650,650);
  c02->SetFillColor(10);
  c02->Divide(2,2,0.0001,0.0001);
  c02_1->SetFillColor(10);
  c02_1->SetFillStyle(4000);
  c02_1->Range(-10,-10,10,10);
  c02_1->SetLeftMargin(0.18);
  c02_1->SetBottomMargin(0.15);
  c02_1->SetRightMargin(0.15);
  c02_1->SetTopMargin(0.12);
  c02_1->SetLogz(1);
  c02_2->SetFillColor(10);
  c02_2->SetFillStyle(4000);
  c02_2->Range(-10,-10,10,10);
  c02_2->SetLeftMargin(0.18);
  c02_2->SetBottomMargin(0.15);
  c02_2->SetRightMargin(0.15);
  c02_2->SetTopMargin(0.12);
  c02_2->SetLogz(1);
  c02_3->SetFillColor(10);
  c02_3->SetFillStyle(4000);
  c02_3->Range(-10,-10,10,10);
  c02_3->SetLeftMargin(0.18);
  c02_3->SetBottomMargin(0.15);
  c02_3->SetRightMargin(0.15);
  c02_3->SetTopMargin(0.12);
  c02_3->SetLogz(1);
  c02_4->SetFillColor(10);
  c02_4->SetFillStyle(4000);
  c02_4->Range(-10,-10,10,10);
  c02_4->SetLeftMargin(0.18);
  c02_4->SetBottomMargin(0.15);
  c02_4->SetRightMargin(0.15);
  c02_4->SetTopMargin(0.12);
  c02_4->SetLogz(1);
  c02->cd(1);
  f2_reseposy[1]->Draw("colz");
  c02->cd(2);
  f2_reseposy[3]->Draw("colz");
  c02->cd(3);
  feta_n[1]->Draw("hist");
  c02->cd(4);
  feta_n[3]->Draw("hist");

  TCanvas* c1=new TCanvas("c1","ams02 12 plaq",0,0,650,650);
  c1->SetFillColor(10);
  c1->Divide(2,2,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->SetFillStyle(4000);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.18);
  c1_1->SetBottomMargin(0.15);
  c1_1->SetRightMargin(0.15);
  c1_1->SetTopMargin(0.12);
  c1_1->SetLogz(1);
  c1_2->SetFillColor(10);
  c1_2->SetFillStyle(4000);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.18);
  c1_2->SetBottomMargin(0.15);
  c1_2->SetRightMargin(0.15);
  c1_2->SetTopMargin(0.12);
  c1_2->SetLogz(1);
  c1_3->SetFillColor(10);
  c1_3->SetFillStyle(4000);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.18);
  c1_3->SetBottomMargin(0.15);
  c1_3->SetRightMargin(0.15);
  c1_3->SetTopMargin(0.12);
  c1_3->SetLogz(1);
  c1_4->SetFillColor(10);
  c1_4->SetFillStyle(4000);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.18);
  c1_4->SetBottomMargin(0.15);
  c1_4->SetRightMargin(0.15);
  c1_4->SetTopMargin(0.12);
  c1_4->SetLogz(1);
  c1->cd(1);
  f1_reseposy[2]->Draw("colz");
  c1->cd(2);
  f2_reseposy[2]->Draw("colz");
  c1->cd(3);
  f1_reseposy[5]->Draw("colz");
  c1->cd(4);
  f2_reseposy[5]->Draw("colz");

  TCanvas* c12=new TCanvas("c12","ams02 12 plaq",0,0,650,650);
  c12->SetFillColor(10);
  c12->Divide(2,2,0.0001,0.0001);
  c12_1->SetFillColor(10);
  c12_1->SetFillStyle(4000);
  c12_1->Range(-10,-10,10,10);
  c12_1->SetLeftMargin(0.18);
  c12_1->SetBottomMargin(0.15);
  c12_1->SetRightMargin(0.15);
  c12_1->SetTopMargin(0.12);
  c12_1->SetLogz(1);
  c12_2->SetFillColor(10);
  c12_2->SetFillStyle(4000);
  c12_2->Range(-10,-10,10,10);
  c12_2->SetLeftMargin(0.18);
  c12_2->SetBottomMargin(0.15);
  c12_2->SetRightMargin(0.15);
  c12_2->SetTopMargin(0.12);
  c12_2->SetLogz(1);
  c12_3->SetFillColor(10);
  c12_3->SetFillStyle(4000);
  c12_3->Range(-10,-10,10,10);
  c12_3->SetLeftMargin(0.18);
  c12_3->SetBottomMargin(0.15);
  c12_3->SetRightMargin(0.15);
  c12_3->SetTopMargin(0.12);
  c12_3->SetLogz(1);
  c12_4->SetFillColor(10);
  c12_4->SetFillStyle(4000);
  c12_4->Range(-10,-10,10,10);
  c12_4->SetLeftMargin(0.18);
  c12_4->SetBottomMargin(0.15);
  c12_4->SetRightMargin(0.15);
  c12_4->SetTopMargin(0.12);
  c12_4->SetLogz(1);
  c12->cd(1);
  f2_reseposy[2]->Draw("colz");
  c12->cd(2);
  f2_reseposy[5]->Draw("colz");
  c12->cd(3);
  feta_n[2]->Draw("hist");
  c12->cd(4);
  feta_n[5]->Draw("hist");

  TCanvas* c2=new TCanvas("c2","ref",0,0,650,650);
  c2->SetFillColor(10);
  c2->Divide(2,2,0.0001,0.0001);
  c2_1->SetFillColor(10);
  c2_1->SetFillStyle(4000);
  c2_1->Range(-10,-10,10,10);
  c2_1->SetLeftMargin(0.18);
  c2_1->SetBottomMargin(0.15);
  c2_1->SetRightMargin(0.15);
  c2_1->SetTopMargin(0.12);
  c2_1->SetLogz(0);
  c2_2->SetFillColor(10);
  c2_2->SetFillStyle(4000);
  c2_2->Range(-10,-10,10,10);
  c2_2->SetLeftMargin(0.18);
  c2_2->SetBottomMargin(0.15);
  c2_2->SetRightMargin(0.15);
  c2_2->SetTopMargin(0.12);
  c2_2->SetLogz(0);
  c2_3->SetFillColor(10);
  c2_3->SetFillStyle(4000);
  c2_3->Range(-10,-10,10,10);
  c2_3->SetLeftMargin(0.18);
  c2_3->SetBottomMargin(0.15);
  c2_3->SetRightMargin(0.15);
  c2_3->SetTopMargin(0.12);
  c2_3->SetLogz(0);
  c2_4->SetFillColor(10);
  c2_4->SetFillStyle(4000);
  c2_4->Range(-10,-10,10,10);
  c2_4->SetLeftMargin(0.18);
  c2_4->SetBottomMargin(0.15);
  c2_4->SetRightMargin(0.15);
  c2_4->SetTopMargin(0.12);
  c2_4->SetLogz(0);
  c2->cd(1);
  f1_reseposy[0]->Draw("colz");
  c2->cd(2);
  f1_reseposy[4]->Draw("colz");
  c2->cd(3);
  f2_reseposy[0]->Draw("colz");
  c2->cd(4);
  f2_reseposy[4]->Draw("colz");

  TCanvas* c20=new TCanvas("c20","ref pos 0",0,0,650,650);
  c20->SetFillColor(10);
  c20->Divide(2,2,0.0001,0.0001);
  c20_1->SetFillColor(10);
  c20_1->SetFillStyle(4000);
  c20_1->Range(-10,-10,10,10);
  c20_1->SetLeftMargin(0.18);
  c20_1->SetBottomMargin(0.15);
  c20_1->SetRightMargin(0.15);
  c20_1->SetTopMargin(0.12);
  c20_1->SetLogz(0);
  c20_2->SetFillColor(10);
  c20_2->SetFillStyle(4000);
  c20_2->Range(-10,-10,10,10);
  c20_2->SetLeftMargin(0.18);
  c20_2->SetBottomMargin(0.15);
  c20_2->SetRightMargin(0.15);
  c20_2->SetTopMargin(0.12);
  c20_2->SetLogz(0);
  c20_3->SetFillColor(10);
  c20_3->SetFillStyle(4000);
  c20_3->Range(-10,-10,10,10);
  c20_3->SetLeftMargin(0.18);
  c20_3->SetBottomMargin(0.25);
  c20_3->SetRightMargin(0.15);
  c20_3->SetTopMargin(0.005);
  c20_3->SetLogz(0);
  c20_4->SetFillColor(10);
  c20_4->SetFillStyle(4000);
  c20_4->Range(-10,-10,10,10);
  c20_4->SetLeftMargin(0.18);
  c20_4->SetBottomMargin(0.25);
  c20_4->SetRightMargin(0.15);
  c20_4->SetTopMargin(0.005);
  c20_4->SetLogz(0);
  c20->cd(1);
  f1_reseposy[0]->Draw("colz");
  tcog = new TLatex(0.475,0.90,"COG");
  tcog->SetNDC();
  tcog->SetTextSize(0.06);
  tcog->Draw();
  c20->cd(2);
  f2_reseposy[0]->Draw("colz");
  teta = new TLatex(0.475,0.90,"f(#eta)");
  teta->SetNDC();
  teta->SetTextSize(0.06);
  teta->Draw();
  c20->cd(4);
  feta_n[4]->Draw("hist");
  c20->cd(3);
  f2_reseposy_proy[0]->SetLineStyle(2);
  f2_reseposy_proy[0]->Draw("hist");
  f1_reseposy_proy[0]->Draw("samehist");
  lcog = new TLine();
  lcog->SetLineWidth(1);
  lcog->SetLineStyle(1);
  lcog->DrawLineNDC(0.650,0.870,0.720,0.870);
  tcog2 = new TLatex(0.740,0.850,"COG");
  tcog2->SetNDC();
  tcog2->SetTextSize(0.05);
  tcog2->Draw();
  leta = new TLine();
  leta->SetLineWidth(1);
  leta->SetLineStyle(2);
  leta->DrawLineNDC(0.650,0.790,0.720,0.790);
  teta2 = new TLatex(0.740,0.775,"f(#eta)");
  teta2->SetNDC();
  teta2->SetTextSize(0.05);
  teta2->Draw();

  TCanvas* c24=new TCanvas("c24","ref pos 4",0,0,650,650);
  c24->SetFillColor(10);
  c24->Divide(2,2,0.0001,0.0001);
  c24_1->SetFillColor(10);
  c24_1->SetFillStyle(4000);
  c24_1->Range(-10,-10,10,10);
  c24_1->SetLeftMargin(0.18);
  c24_1->SetBottomMargin(0.15);
  c24_1->SetRightMargin(0.15);
  c24_1->SetTopMargin(0.12);
  c24_1->SetLogz(0);
  c24_2->SetFillColor(10);
  c24_2->SetFillStyle(4000);
  c24_2->Range(-10,-10,10,10);
  c24_2->SetLeftMargin(0.18);
  c24_2->SetBottomMargin(0.15);
  c24_2->SetRightMargin(0.15);
  c24_2->SetTopMargin(0.12);
  c24_2->SetLogz(0);
  c24_3->SetFillColor(10);
  c24_3->SetFillStyle(4000);
  c24_3->Range(-10,-10,10,10);
  c24_3->SetLeftMargin(0.18);
  c24_3->SetBottomMargin(0.25);
  c24_3->SetRightMargin(0.15);
  c24_3->SetTopMargin(0.005);
  c24_3->SetLogz(0);
  c24_4->SetFillColor(10);
  c24_4->SetFillStyle(4000);
  c24_4->Range(-10,-10,10,10);
  c24_4->SetLeftMargin(0.18);
  c24_4->SetBottomMargin(0.25);
  c24_4->SetRightMargin(0.15);
  c24_4->SetTopMargin(0.005);
  c24_4->SetLogz(0);
  c24->cd(1);
  f1_reseposy[4]->Draw("colz");
  tcog->Draw();
  c24->cd(2);
  f2_reseposy[4]->Draw("colz");
  teta->Draw();
  c24->cd(4);
  feta_n[4]->Draw("hist");
  c24->cd(3);
  f1_reseposy_proy[4]->Draw("hist");
  f2_reseposy_proy[4]->SetLineStyle(2);
  f2_reseposy_proy[4]->Draw("samehist");
  lcog = new TLine();
  lcog->SetLineWidth(1);
  lcog->SetLineStyle(1);
  lcog->DrawLineNDC(0.650,0.870,0.720,0.870);
  tcog2 = new TLatex(0.740,0.850,"COG");
  tcog2->SetNDC();
  tcog2->SetTextSize(0.05);
  tcog2->Draw();
  leta = new TLine();
  leta->SetLineWidth(1);
  leta->SetLineStyle(2);
  leta->DrawLineNDC(0.650,0.790,0.720,0.790);
  teta2 = new TLatex(0.740,0.775,"f(#eta)");
  teta2->SetNDC();
  teta2->SetTextSize(0.05);
  teta2->Draw();

  TCanvas* c3=new TCanvas("c3","pos 1 pos 2",0,0,650,650);
  c3->SetFillColor(10);
  c3->Divide(2,2,0.0001,0.0001);
  c3_1->SetFillColor(10);
  c3_1->SetFillStyle(4000);
  c3_1->Range(-10,-10,10,10);
  c3_1->SetLeftMargin(0.18);
  c3_1->SetBottomMargin(0.15);
  c3_1->SetRightMargin(0.15);
  c3_1->SetTopMargin(0.12);
  c3_1->SetLogz(0);
  c3_2->SetFillColor(10);
  c3_2->SetFillStyle(4000);
  c3_2->Range(-10,-10,10,10);
  c3_2->SetLeftMargin(0.18);
  c3_2->SetBottomMargin(0.15);
  c3_2->SetRightMargin(0.15);
  c3_2->SetTopMargin(0.12);
  c3_2->SetLogz(0);
  c3_3->SetFillColor(10);
  c3_3->SetFillStyle(4000);
  c3_3->Range(-10,-10,10,10);
  c3_3->SetLeftMargin(0.18);
  c3_3->SetBottomMargin(0.15);
  c3_3->SetRightMargin(0.15);
  c3_3->SetTopMargin(0.12);
  c3_3->SetLogz(0);
  c3_4->SetFillColor(10);
  c3_4->SetFillStyle(4000);
  c3_4->Range(-10,-10,10,10);
  c3_4->SetLeftMargin(0.18);
  c3_4->SetBottomMargin(0.15);
  c3_4->SetRightMargin(0.15);
  c3_4->SetTopMargin(0.12);
  c3_4->SetLogz(0);
  c3->cd(1);
  f1_reseposy[1]->Draw("colz");
  c3->cd(2);
  f1_reseposy[2]->Draw("colz");
  c3->cd(3);
  f2_reseposy[1]->Draw("colz");
  c3->cd(4);
  f2_reseposy[2]->Draw("colz");

  TCanvas* c32=new TCanvas("c32","pos 1 pos 2 ",0,0,650,650);
  c32->SetFillColor(10);
  c32->Divide(2,2,0.0001,0.0001);
  c32_1->SetFillColor(10);
  c32_1->SetFillStyle(4000);
  c32_1->Range(-10,-10,10,10);
  c32_1->SetLeftMargin(0.18);
  c32_1->SetBottomMargin(0.15);
  c32_1->SetRightMargin(0.15);
  c32_1->SetTopMargin(0.12);
  c32_1->SetLogz(0);
  c32_2->SetFillColor(10);
  c32_2->SetFillStyle(4000);
  c32_2->Range(-10,-10,10,10);
  c32_2->SetLeftMargin(0.18);
  c32_2->SetBottomMargin(0.15);
  c32_2->SetRightMargin(0.15);
  c32_2->SetTopMargin(0.12);
  c32_2->SetLogz(0);
  c32_3->SetFillColor(10);
  c32_3->SetFillStyle(4000);
  c32_3->Range(-10,-10,10,10);
  c32_3->SetLeftMargin(0.18);
  c32_3->SetBottomMargin(0.25);
  c32_3->SetRightMargin(0.15);
  c32_3->SetTopMargin(0.005);
  c32_3->SetLogz(0);
  c32_4->SetFillColor(10);
  c32_4->SetFillStyle(4000);
  c32_4->Range(-10,-10,10,10);
  c32_4->SetLeftMargin(0.18);
  c32_4->SetBottomMargin(0.25);
  c32_4->SetRightMargin(0.15);
  c32_4->SetTopMargin(0.005);
  c32_4->SetLogz(0);
  c32->cd(1);
  f2_reseposy[1]->Draw("colz");
  c32->cd(2);
  f2_reseposy[2]->Draw("colz");
  c32->cd(3);
  feta_n[1]->Draw("hist");
  c32->cd(4);
  feta_n[2]->Draw("hist");

  TCanvas* c4=new TCanvas("c4","pos 3 pos 5",0,0,650,650);
  c4->SetFillColor(10);
  c4->Divide(2,2,0.0001,0.0001);
  c4_1->SetFillColor(10);
  c4_1->SetFillStyle(4000);
  c4_1->Range(-10,-10,10,10);
  c4_1->SetLeftMargin(0.18);
  c4_1->SetBottomMargin(0.15);
  c4_1->SetRightMargin(0.15);
  c4_1->SetTopMargin(0.12);
  c4_1->SetLogz(0);
  c4_2->SetFillColor(10);
  c4_2->SetFillStyle(4000);
  c4_2->Range(-10,-10,10,10);
  c4_2->SetLeftMargin(0.18);
  c4_2->SetBottomMargin(0.15);
  c4_2->SetRightMargin(0.15);
  c4_2->SetTopMargin(0.12);
  c4_2->SetLogz(0);
  c4_3->SetFillColor(10);
  c4_3->SetFillStyle(4000);
  c4_3->Range(-10,-10,10,10);
  c4_3->SetLeftMargin(0.18);
  c4_3->SetBottomMargin(0.15);
  c4_3->SetRightMargin(0.15);
  c4_3->SetTopMargin(0.12);
  c4_3->SetLogz(0);
  c4_4->SetFillColor(10);
  c4_4->SetFillStyle(4000);
  c4_4->Range(-10,-10,10,10);
  c4_4->SetLeftMargin(0.18);
  c4_4->SetBottomMargin(0.15);
  c4_4->SetRightMargin(0.15);
  c4_4->SetTopMargin(0.12);
  c4_4->SetLogz(0);
  c4->cd(1);
  f1_reseposy[3]->Draw("colz");
  c4->cd(2);
  f1_reseposy[5]->Draw("colz");
  c4->cd(3);
  f2_reseposy[3]->Draw("colz");
  c4->cd(4);
  f2_reseposy[5]->Draw("colz");

  TCanvas* c42=new TCanvas("c42","pos 3 pos 5",0,0,650,650);
  c42->SetFillColor(10);
  c42->Divide(2,2,0.0001,0.0001);
  c42_1->SetFillColor(10);
  c42_1->SetFillStyle(4000);
  c42_1->Range(-10,-10,10,10);
  c42_1->SetLeftMargin(0.18);
  c42_1->SetBottomMargin(0.15);
  c42_1->SetRightMargin(0.15);
  c42_1->SetTopMargin(0.12);
  c42_1->SetLogz(0);
  c42_2->SetFillColor(10);
  c42_2->SetFillStyle(4000);
  c42_2->Range(-10,-10,10,10);
  c42_2->SetLeftMargin(0.18);
  c42_2->SetBottomMargin(0.15);
  c42_2->SetRightMargin(0.15);
  c42_2->SetTopMargin(0.12);
  c42_2->SetLogz(0);
  c42_3->SetFillColor(10);
  c42_3->SetFillStyle(4000);
  c42_3->Range(-10,-10,10,10);
  c42_3->SetLeftMargin(0.18);
  c42_3->SetBottomMargin(0.25);
  c42_3->SetRightMargin(0.15);
  c42_3->SetTopMargin(0.005);
  c42_3->SetLogz(1);
  c42_4->SetFillColor(10);
  c42_4->SetFillStyle(4000);
  c42_4->Range(-10,-10,10,10);
  c42_4->SetLeftMargin(0.18);
  c42_4->SetBottomMargin(0.25);
  c42_4->SetRightMargin(0.15);
  c42_4->SetTopMargin(0.005);
  c42_4->SetLogz(1);
  c42->cd(1);
  f2_reseposy[3]->Draw("colz");
  c42->cd(2);
  f2_reseposy[5]->Draw("colz");
  c42->cd(3);
  feta_n[3]->Draw("hist");
  c42->cd(4);
  feta_n[5]->Draw("hist");

  TCanvas* c5=new TCanvas("c5","pos 3 5pt",0,0,650,650);
  c5->SetFillColor(10);
  c5->Divide(2,2,0.0001,0.0001);
  c5_1->SetFillColor(10);
  c5_1->SetFillStyle(4000);
  c5_1->Range(-10,-10,10,10);
  c5_1->SetLeftMargin(0.18);
  c5_1->SetBottomMargin(0.15);
  c5_1->SetRightMargin(0.15);
  c5_1->SetTopMargin(0.12);
  c5_1->SetLogz(0);
  c5_2->SetFillColor(10);
  c5_2->SetFillStyle(4000);
  c5_2->Range(-10,-10,10,10);
  c5_2->SetLeftMargin(0.18);
  c5_2->SetBottomMargin(0.15);
  c5_2->SetRightMargin(0.15);
  c5_2->SetTopMargin(0.12);
  c5_2->SetLogz(0);
  c5_3->SetFillColor(10);
  c5_3->SetFillStyle(4000);
  c5_3->Range(-10,-10,10,10);
  c5_3->SetLeftMargin(0.18);
  c5_3->SetBottomMargin(0.25);
  c5_3->SetRightMargin(0.15);
  c5_3->SetTopMargin(0.005);
  c5_3->SetLogz(1);
  c5_4->SetFillColor(10);
  c5_4->SetFillStyle(4000);
  c5_4->Range(-10,-10,10,10);
  c5_4->SetLeftMargin(0.18);
  c5_4->SetBottomMargin(0.25);
  c5_4->SetRightMargin(0.15);
  c5_4->SetTopMargin(0.005);
  c5_4->SetLogz(1);
  c5->cd(1);
  f1_reseposy[3]->Draw("colz");
  tcog = new TLatex(0.475,0.90,"COG");
  tcog->SetNDC();
  tcog->SetTextSize(0.06);
  tcog->Draw();
  c5->cd(2);
  f2_reseposy[3]->Draw("colz");
  teta = new TLatex(0.475,0.90,"f(#eta)");
  teta->SetNDC();
  teta->SetTextSize(0.06);
  teta->Draw();
  c5->cd(4);
  feta_n[3]->Draw("hist");
  c5->cd(3);
  f1_reseposy_proy[3]->Draw("hist");
  f2_reseposy_proy[3]->SetLineStyle(2);
  f2_reseposy_proy[3]->Draw("samehist");
  lcog = new TLine();
  lcog->SetLineWidth(1);
  lcog->SetLineStyle(1);
  lcog->DrawLineNDC(0.650,0.870,0.720,0.870);
  tcog2 = new TLatex(0.740,0.850,"COG");
  tcog2->SetNDC();
  tcog2->SetTextSize(0.05);
  tcog2->Draw();
  leta = new TLine();
  leta->SetLineWidth(1);
  leta->SetLineStyle(2);
  leta->DrawLineNDC(0.650,0.790,0.720,0.790);
  teta2 = new TLatex(0.740,0.775,"f(#eta)");
  teta2->SetNDC();
  teta2->SetTextSize(0.05);
  teta2->Draw();

  TCanvas* c6=new TCanvas("c61","pos 1 2pt",0,0,650,650);
  c61->SetFillColor(10);
  c61->Divide(2,2,0.0001,0.0001);
  c61_1->SetFillColor(10);
  c61_1->SetFillStyle(4000);
  c61_1->Range(-10,-10,10,10);
  c61_1->SetLeftMargin(0.18);
  c61_1->SetBottomMargin(0.15);
  c61_1->SetRightMargin(0.15);
  c61_1->SetTopMargin(0.12);
  c61_1->SetLogz(0);
  c61_2->SetFillColor(10);
  c61_2->SetFillStyle(4000);
  c61_2->Range(-10,-10,10,10);
  c61_2->SetLeftMargin(0.18);
  c61_2->SetBottomMargin(0.15);
  c61_2->SetRightMargin(0.15);
  c61_2->SetTopMargin(0.12);
  c61_2->SetLogz(0);
  c61_3->SetFillColor(10);
  c61_3->SetFillStyle(4000);
  c61_3->Range(-10,-10,10,10);
  c61_3->SetLeftMargin(0.18);
  c61_3->SetBottomMargin(0.25);
  c61_3->SetRightMargin(0.15);
  c61_3->SetTopMargin(0.005);
  c61_3->SetLogz(1);
  c61_4->SetFillColor(10);
  c61_4->SetFillStyle(4000);
  c61_4->Range(-10,-10,10,10);
  c61_4->SetLeftMargin(0.18);
  c61_4->SetBottomMargin(0.25);
  c61_4->SetRightMargin(0.15);
  c61_4->SetTopMargin(0.005);
  c61_4->SetLogz(1);
  c61->cd(1);
  f1_resesposy[1]->Draw("colz");
  tcog = new TLatex(0.475,0.90,"COG");
  tcog->SetNDC();
  tcog->SetTextSize(0.06);
  tcog->Draw();
  c61->cd(2);
  f2_resesposy[1]->Draw("colz");
  teta = new TLatex(0.475,0.90,"f(#eta)");
  teta->SetNDC();
  teta->SetTextSize(0.06);
  teta->Draw();
  c61->cd(4);
  feta_n[1]->Draw("hist");
  c61->cd(3);
  f2_resesposy_proy[1]->SetLineStyle(2);
  f2_resesposy_proy[1]->Draw("hist");
  f1_resesposy_proy[1]->Draw("samehist");
  lcog = new TLine();
  lcog->SetLineWidth(1);
  lcog->SetLineStyle(1);
  lcog->DrawLineNDC(0.650,0.870,0.720,0.870);
  tcog2 = new TLatex(0.740,0.850,"COG");
  tcog2->SetNDC();
  tcog2->SetTextSize(0.05);
  tcog2->Draw();
  leta = new TLine();
  leta->SetLineWidth(1);
  leta->SetLineStyle(2);
  leta->DrawLineNDC(0.650,0.790,0.720,0.790);
  teta2 = new TLatex(0.740,0.775,"f(#eta)");
  teta2->SetNDC();
  teta2->SetTextSize(0.05);
  teta2->Draw();

  TCanvas* c6=new TCanvas("c62","pos 2 2pt",0,0,650,650);
  c62->SetFillColor(10);
  c62->Divide(2,2,0.0001,0.0001);
  c62_1->SetFillColor(10);
  c62_1->SetFillStyle(4000);
  c62_1->Range(-10,-10,10,10);
  c62_1->SetLeftMargin(0.18);
  c62_1->SetBottomMargin(0.15);
  c62_1->SetRightMargin(0.15);
  c62_1->SetTopMargin(0.12);
  c62_1->SetLogz(0);
  c62_2->SetFillColor(10);
  c62_2->SetFillStyle(4000);
  c62_2->Range(-10,-10,10,10);
  c62_2->SetLeftMargin(0.18);
  c62_2->SetBottomMargin(0.15);
  c62_2->SetRightMargin(0.15);
  c62_2->SetTopMargin(0.12);
  c62_2->SetLogz(0);
  c62_3->SetFillColor(10);
  c62_3->SetFillStyle(4000);
  c62_3->Range(-10,-10,10,10);
  c62_3->SetLeftMargin(0.18);
  c62_3->SetBottomMargin(0.25);
  c62_3->SetRightMargin(0.15);
  c62_3->SetTopMargin(0.005);
  c62_3->SetLogz(1);
  c62_4->SetFillColor(10);
  c62_4->SetFillStyle(4000);
  c62_4->Range(-10,-10,10,10);
  c62_4->SetLeftMargin(0.18);
  c62_4->SetBottomMargin(0.25);
  c62_4->SetRightMargin(0.15);
  c62_4->SetTopMargin(0.005);
  c62_4->SetLogz(1);
  c62->cd(1);
  f1_resesposy[2]->Draw("colz");
  tcog = new TLatex(0.475,0.90,"COG");
  tcog->SetNDC();
  tcog->SetTextSize(0.06);
  tcog->Draw();
  c62->cd(2);
  f2_resesposy[2]->Draw("colz");
  teta = new TLatex(0.475,0.90,"f(#eta)");
  teta->SetNDC();
  teta->SetTextSize(0.06);
  teta->Draw();
  c62->cd(4);
  feta_n[2]->Draw("hist");
  c62->cd(3);
  f2_resesposy_proy[2]->SetLineStyle(2);
  f2_resesposy_proy[2]->Draw("hist");
  f1_resesposy_proy[2]->Draw("samehist");
  lcog->DrawLineNDC(0.650,0.870,0.720,0.870);
  tcog2->Draw();
  leta->DrawLineNDC(0.650,0.790,0.720,0.790);
  teta2->Draw();

  TCanvas* c6=new TCanvas("c63","pos 3 2pt",0,0,650,650);
  c63->SetFillColor(10);
  c63->Divide(2,2,0.0001,0.0001);
  c63_1->SetFillColor(10);
  c63_1->SetFillStyle(4000);
  c63_1->Range(-10,-10,10,10);
  c63_1->SetLeftMargin(0.18);
  c63_1->SetBottomMargin(0.15);
  c63_1->SetRightMargin(0.15);
  c63_1->SetTopMargin(0.12);
  c63_1->SetLogz(0);
  c63_2->SetFillColor(10);
  c63_2->SetFillStyle(4000);
  c63_2->Range(-10,-10,10,10);
  c63_2->SetLeftMargin(0.18);
  c63_2->SetBottomMargin(0.15);
  c63_2->SetRightMargin(0.15);
  c63_2->SetTopMargin(0.12);
  c63_2->SetLogz(0);
  c63_3->SetFillColor(10);
  c63_3->SetFillStyle(4000);
  c63_3->Range(-10,-10,10,10);
  c63_3->SetLeftMargin(0.18);
  c63_3->SetBottomMargin(0.25);
  c63_3->SetRightMargin(0.15);
  c63_3->SetTopMargin(0.005);
  c63_3->SetLogz(1);
  c63_4->SetFillColor(10);
  c63_4->SetFillStyle(4000);
  c63_4->Range(-10,-10,10,10);
  c63_4->SetLeftMargin(0.18);
  c63_4->SetBottomMargin(0.25);
  c63_4->SetRightMargin(0.15);
  c63_4->SetTopMargin(0.005);
  c63_4->SetLogz(1);
  c63->cd(1);
  f1_resesposy[3]->Draw("colz");
  tcog = new TLatex(0.475,0.90,"COG");
  tcog->SetNDC();
  tcog->SetTextSize(0.06);
  tcog->Draw();
  c63->cd(2);
  f2_resesposy[3]->Draw("colz");
  teta = new TLatex(0.475,0.90,"f(#eta)");
  teta->SetNDC();
  teta->SetTextSize(0.06);
  teta->Draw();
  c63->cd(4);
  feta_n[3]->Draw("hist");
  c63->cd(3);
  f2_resesposy_proy[3]->SetLineStyle(2);
  f2_resesposy_proy[3]->Draw("hist");
  f1_resesposy_proy[3]->Draw("samehist");
  lcog->DrawLineNDC(0.650,0.870,0.720,0.870);
  tcog2->Draw();
  leta->DrawLineNDC(0.650,0.790,0.720,0.790);
  teta2->Draw();

  TCanvas* c65=new TCanvas("c65","pos 5 2pt",0,0,650,650);
  c65->SetFillColor(10);
  c65->Divide(2,2,0.0001,0.0001);
  c65_1->SetFillColor(10);
  c65_1->SetFillStyle(4000);
  c65_1->Range(-10,-10,10,10);
  c65_1->SetLeftMargin(0.18);
  c65_1->SetBottomMargin(0.15);
  c65_1->SetRightMargin(0.15);
  c65_1->SetTopMargin(0.12);
  c65_1->SetLogz(0);
  c65_2->SetFillColor(10);
  c65_2->SetFillStyle(4000);
  c65_2->Range(-10,-10,10,10);
  c65_2->SetLeftMargin(0.18);
  c65_2->SetBottomMargin(0.15);
  c65_2->SetRightMargin(0.15);
  c65_2->SetTopMargin(0.12);
  c65_2->SetLogz(0);
  c65_3->SetFillColor(10);
  c65_3->SetFillStyle(4000);
  c65_3->Range(-10,-10,10,10);
  c65_3->SetLeftMargin(0.18);
  c65_3->SetBottomMargin(0.25);
  c65_3->SetRightMargin(0.15);
  c65_3->SetTopMargin(0.005);
  c65_3->SetLogz(1);
  c65_4->SetFillColor(10);
  c65_4->SetFillStyle(4000);
  c65_4->Range(-10,-10,10,10);
  c65_4->SetLeftMargin(0.18);
  c65_4->SetBottomMargin(0.25);
  c65_4->SetRightMargin(0.15);
  c65_4->SetTopMargin(0.005);
  c65_4->SetLogz(1);
  c65->cd(1);
  f1_resesposy[5]->Draw("colz");
  tcog = new TLatex(0.475,0.90,"COG");
  tcog->SetNDC();
  tcog->SetTextSize(0.06);
  tcog->Draw();
  c65->cd(2);
  f2_resesposy[5]->Draw("colz");
  teta = new TLatex(0.475,0.90,"f(#eta)");
  teta->SetNDC();
  teta->SetTextSize(0.06);
  teta->Draw();
  c65->cd(4);
  feta_n[5]->Draw("hist");
  c65->cd(3);
  f2_resesposy_proy[5]->SetLineStyle(2);
  f2_resesposy_proy[5]->Draw("hist");
  f1_resesposy_proy[5]->Draw("samehist");
  lcog->DrawLineNDC(0.650,0.870,0.720,0.870);
  tcog2->Draw();
  leta->DrawLineNDC(0.650,0.790,0.720,0.790);
  teta2->Draw();
  return;
}

void cote_n() {

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  float fac = 1000000.;
  const int lec_pos_conv[6] = { 3, 1, 5, 2, 4, 0 };
    
  TFile *f1 = new TFile("Hist_1234_a6_1_2_6_etan.root");
  TH1D *f1_tra_1py = (TH1D*) f1->Get("tra_1py")->Clone();
  f1_tra_1py->GetXaxis()->SetLabelSize(0.065);
  f1_tra_1py->GetXaxis()->SetLabelOffset(0.015);
  //  f1_tra_1py->GetXaxis()->SetRangeUser(0.5,2.5);
  f1_tra_1py->GetYaxis()->SetLabelSize(0.060);
  f1_tra_1py->GetYaxis()->SetLabelOffset(0.015);
  f1_tra_1py->SetLineColor(1);
  TH1D *f1_tra_m1py = (TH1D*) f1->Get("tra_m1py")->Clone();
  f1_tra_m1py->GetXaxis()->SetLabelSize(0.065);
  f1_tra_m1py->GetXaxis()->SetLabelOffset(0.015);
  //  f1_tra_m1py->GetXaxis()->SetRangeUser(0.5,2.5);
  f1_tra_m1py->GetYaxis()->SetLabelSize(0.060);
  f1_tra_m1py->GetYaxis()->SetLabelOffset(0.015);
  f1_tra_m1py->SetLineColor(1);
  TH1D *f1_no_1px = (TH1D*) f1->Get("no_1px")->Clone();
  float nentrees = f1_no_1px->GetEntries();
  f1_no_1px->Scale(1/nentrees);
  f1_no_1px->GetXaxis()->SetLabelSize(0.065);
  f1_no_1px->GetXaxis()->SetLabelOffset(0.015);
  f1_no_1px->GetYaxis()->SetLabelSize(0.060);
  f1_no_1px->GetYaxis()->SetLabelOffset(0.015);
  f1_no_1px->SetLineWidth(2);
  f1_no_1px->SetLineColor(1);
  TH1D *f1_no_1py = (TH1D*) f1->Get("no_1py")->Clone();
  nentrees = f1_no_1px->GetEntries();
  f1_no_1py->Scale(1/nentrees);
  f1_no_1py->GetXaxis()->SetLabelSize(0.065);
  f1_no_1py->GetXaxis()->SetLabelOffset(0.015);
  f1_no_1py->GetYaxis()->SetLabelSize(0.060);
  f1_no_1py->GetYaxis()->SetLabelOffset(0.015);
  f1_no_1py->SetLineWidth(2);
  f1_no_1py->SetLineColor(1);
  TH1D *f1_aytai[6];
  f1_aytai[3] = (TH1D*) f1->Get("0900/NPiste_0900_K")->Clone();
  f1_aytai[1] = (TH1D*) f1->Get("0901/NPiste_0901_K")->Clone();
  f1_aytai[5] = (TH1D*) f1->Get("0904/NPiste_0904_K")->Clone();
  f1_aytai[2] = (TH1D*) f1->Get("0905/NPiste_0905_K")->Clone();
  f1_aytai[4] = (TH1D*) f1->Get("2110/NPiste_2110_K")->Clone();
  f1_aytai[0] = (TH1D*) f1->Get("2111/NPiste_2111_K")->Clone();
  for (int i=0; i<6; i++) {
   float entrees = f1_aytai[i]->GetEntries();
   f1_aytai[i]->Scale(1./entrees);
   printf(" i %d entrees %f\n",i,entrees);
   f1_aytai[i]->GetXaxis()->SetLabelSize(0.065);
   f1_aytai[i]->GetXaxis()->SetLabelOffset(0.015);
   f1_aytai[i]->GetXaxis()->SetRangeUser(0.5,2.5);
   f1_aytai[i]->GetXaxis()->SetTitle("Strips");
   f1_aytai[i]->GetXaxis()->SetTitleSize(0.075);
   f1_aytai[i]->GetXaxis()->SetTitleOffset(1.00);
   f1_aytai[i]->GetXaxis()->CenterTitle();
   f1_aytai[i]->GetYaxis()->SetLabelSize(0.060);
   f1_aytai[i]->GetYaxis()->SetLabelOffset(0.015);
   f1_aytai[i]->GetYaxis()->SetTitle("Fraction");
   f1_aytai[i]->GetYaxis()->SetTitleSize(0.065);
   f1_aytai[i]->GetYaxis()->SetTitleOffset(1.00);
   f1_aytai[i]->GetYaxis()->CenterTitle();
   f1_aytai[i]->SetLineColor(1);
   f1_aytai[i]->SetLineWidth(2);
  }
  TH1D *f1_ayocc[6];
  f1_ayocc[3] = (TH1D*) f1->Get("0900/OccupancyCOG_0900")->Clone();
  f1_ayocc[1] = (TH1D*) f1->Get("0901/OccupancyCOG_0901")->Clone();
  f1_ayocc[5] = (TH1D*) f1->Get("0904/OccupancyCOG_0904")->Clone();
  f1_ayocc[2] = (TH1D*) f1->Get("0905/OccupancyCOG_0905")->Clone();
  f1_ayocc[4] = (TH1D*) f1->Get("2110/OccupancyCOG_2110")->Clone();
  f1_ayocc[0] = (TH1D*) f1->Get("2111/OccupancyCOG_2111")->Clone();
  for (int i=0; i<6; i++) {
   float entrees = f1_ayocc[i]->GetEntries();
   f1_ayocc[i]->Scale(2./entrees);
   printf(" i %d entrees %f\n",i,entrees);
   f1_ayocc[i]->GetXaxis()->SetLabelSize(0.065);
   f1_ayocc[i]->GetXaxis()->SetLabelOffset(0.015);
   f1_ayocc[i]->GetXaxis()->SetTitle("Canal");
   f1_ayocc[i]->GetXaxis()->SetTitleSize(0.075);
   f1_ayocc[i]->GetXaxis()->SetTitleOffset(1.00);
   f1_ayocc[i]->GetXaxis()->CenterTitle();
   switch (i) {
    case 0:
     f1_ayocc[i]->GetXaxis()->SetRangeUser(640.,896.);
     break;
    case 1:
     f1_ayocc[i]->GetXaxis()->SetRangeUser(700.,780.);
     break;
    case 2:
     f1_ayocc[i]->GetXaxis()->SetRangeUser(950.,1023);
     break;
    case 3:
     f1_ayocc[i]->GetXaxis()->SetRangeUser(690.,770.);
     break;
    case 4:
     f1_ayocc[i]->GetXaxis()->SetRangeUser(640.,896.);
     break;
    case 5:
     f1_ayocc[i]->GetXaxis()->SetRangeUser(680.,760.);
     break;
    default:
     break;
   }
   f1_ayocc[i]->GetYaxis()->SetLabelSize(0.065);
   f1_ayocc[i]->GetYaxis()->SetLabelOffset(0.0025);
   f1_ayocc[i]->GetYaxis()->SetTitle("Fraction");
   f1_ayocc[i]->GetYaxis()->SetTitleSize(0.065);
   f1_ayocc[i]->GetYaxis()->SetTitleOffset(1.00);
   f1_ayocc[i]->GetYaxis()->CenterTitle();
   f1_ayocc[i]->SetLineColor(1);
  }
  TH1D *f1_ayeta[6];
  f1_ayeta[3] = (TH1D*) f1->Get("0900/Eta_0900_K")->Clone();
  f1_ayeta[1] = (TH1D*) f1->Get("0901/Eta_0901_K")->Clone();
  f1_ayeta[5] = (TH1D*) f1->Get("0904/Eta_0904_K")->Clone();
  f1_ayeta[2] = (TH1D*) f1->Get("0905/Eta_0905_K")->Clone();
  f1_ayeta[4] = (TH1D*) f1->Get("2110/Eta_2110_K")->Clone();
  f1_ayeta[0] = (TH1D*) f1->Get("2111/Eta_2111_K")->Clone();
  for (int i=0; i<6; i++) {
   float entrees = f1_ayeta[i]->GetEntries();
   f1_ayeta[i]->Scale(1./entrees);
   printf(" i %d entrees %f\n",i,entrees);
   f1_ayeta[i]->GetXaxis()->SetLabelSize(0.065);
   f1_ayeta[i]->GetXaxis()->SetLabelOffset(0.015);
   f1_ayeta[i]->GetXaxis()->SetTitle("#eta");
   f1_ayeta[i]->GetXaxis()->SetTitleSize(0.075);
   f1_ayeta[i]->GetXaxis()->SetTitleOffset(1.00);
   f1_ayeta[i]->GetXaxis()->CenterTitle();
   f1_ayeta[i]->GetYaxis()->SetLabelSize(0.060);
   f1_ayeta[i]->GetYaxis()->SetLabelOffset(0.015);
   f1_ayeta[i]->GetYaxis()->SetTitle("Fraction");
   f1_ayeta[i]->GetYaxis()->SetTitleSize(0.065);
   f1_ayeta[i]->GetYaxis()->SetTitleOffset(1.40);
   f1_ayeta[i]->GetYaxis()->CenterTitle();
   f1_ayeta[i]->SetLineColor(1);
  }
  TH1D *f1_yposhr[6];
  char chist[50];
  for (int i=0; i<6; i++) {
    sprintf(chist,"y%dhr",i);
    printf("%s\n",chist);
    f1_yposhr[lec_pos_conv[i]] = (TH1D*) f1->Get(chist)->Clone();
    f1_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelSize(0.065);
    f1_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelOffset(0.015);
    f1_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitle("Y (mm)");
    f1_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleSize(0.075);
    f1_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleOffset(1.00);
    f1_yposhr[lec_pos_conv[i]]->GetXaxis()->CenterTitle();
    f1_yposhr[lec_pos_conv[i]]->GetXaxis()->SetRangeUser(-0.3,0.3);
    f1_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelSize(0.065);
    f1_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f1_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitle("Entrees");
    f1_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleSize(0.065);
    f1_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleOffset(1.00);
    f1_yposhr[lec_pos_conv[i]]->GetYaxis()->CenterTitle();
    f1_yposhr[lec_pos_conv[i]]->GetYaxis()->SetRangeUser(0.5,300.);
    f1_yposhr[lec_pos_conv[i]]->SetLineColor(4);
    f1_yposhr[lec_pos_conv[i]]->SetLineStyle(2);
  }
  TH1D *f1_ypos1p[6];
  for (int i=0; i<6; i++) {
    sprintf(chist,"y%d1p",i);
    printf("%s\n",chist);
    f1_ypos1p[lec_pos_conv[i]] = (TH1D*) f1->Get(chist)->Clone();
    f1_ypos1p[lec_pos_conv[i]]->SetLineColor(1);
    f1_ypos1p[lec_pos_conv[i]]->SetFillColor(1);
    f1_ypos1p[lec_pos_conv[i]]->SetLineStyle(1);
  }
  TH1D *f1_ptdydz_fais = (TH1D*) f1->Get("dydz_fais")->Clone();
  f1_ptdydz_fais->SetLineColor(4);
  f1_ptdydz_fais->SetLineStyle(2);
  f1_ptdydz_fais->SetLineWidth(2);
  TH2D *f1_ptdydz = (TH2D*) f1->Get("dydz")->Clone();
  f1_ptdydz->SetMarkerColor(4);
  TH1D *f1_dydz_proy[15];
  for (int ib=1; ib<16; ib++) {
    sprintf(chist,"proy_%d",ib);
    f1_dydz_proy[ib-1] = f1_ptdydz->ProjectionY(chist,ib,ib,"");
    printf(" bin %d moyen %f rms %f\n",ib,fac*f1_dydz_proy[ib-1]->GetMean(),fac*f1_dydz_proy[ib-1]->GetRMS());
  }

  TFile *f2 = new TFile("Hist_1251_a6_1_2_6_etan.root");
  TH1D* f2_tra_1py = (TH1D*) f2->Get("tra_1py")->Clone();
  f2_tra_1py->GetXaxis()->SetLabelSize(0.065);
  f2_tra_1py->GetXaxis()->SetLabelOffset(0.015);
  //  f2_tra_1py->GetXaxis()->SetRangeUser(0.5,2.5);
  f2_tra_1py->GetYaxis()->SetLabelSize(0.060);
  f2_tra_1py->GetYaxis()->SetLabelOffset(0.015);
  f2_tra_1py->SetLineColor(14);
  TH1D* f2_tra_m1py = (TH1D*) f2->Get("tra_m1py")->Clone();
  f2_tra_m1py->GetXaxis()->SetLabelSize(0.065);
  f2_tra_m1py->GetXaxis()->SetLabelOffset(0.015);
  //  f2_tra_m1py->GetXaxis()->SetRangeUser(0.5,2.5);
  f2_tra_m1py->GetYaxis()->SetLabelSize(0.060);
  f2_tra_m1py->GetYaxis()->SetLabelOffset(0.015);
  f2_tra_m1py->SetLineColor(14);
  TH1D *f2_no_1px = (TH1D*) f2->Get("no_1px")->Clone();
  nentrees = f2_no_1px->GetEntries();
  f2_no_1px->Scale(1/nentrees);
  f2_no_1px->GetXaxis()->SetLabelSize(0.065);
  f2_no_1px->GetXaxis()->SetLabelOffset(0.001);
  f2_no_1px->GetYaxis()->SetLabelSize(0.060);
  f2_no_1px->GetYaxis()->SetLabelOffset(0.001);
  f2_no_1px->GetYaxis()->SetTitle("fraction");
  f2_no_1px->GetYaxis()->SetTitleSize(0.065);
  f2_no_1px->GetYaxis()->SetTitleOffset(1.45);
  f2_no_1px->GetYaxis()->CenterTitle();
  f2_no_1px->SetLineWidth(2);
  f2_no_1px->SetLineStyle(2);
  f2_no_1px->SetMaximum(0.5);
  f2_no_1px->SetMinimum(0.00005);
  f2_no_1px->SetLineColor(14);
  TH1D *f2_no_1py = (TH1D*) f2->Get("no_1py")->Clone();
  nentrees = f2_no_1px->GetEntries();
  f2_no_1py->Scale(1/nentrees);
  f2_no_1py->GetXaxis()->SetLabelSize(0.065);
  f2_no_1py->GetXaxis()->SetLabelOffset(0.001);
  f2_no_1py->GetYaxis()->SetLabelSize(0.060);
  f2_no_1py->GetYaxis()->SetLabelOffset(0.015);
  f2_no_1py->GetYaxis()->SetTitle("fraction");
  f2_no_1py->GetYaxis()->SetTitleSize(0.065);
  f2_no_1py->GetYaxis()->SetTitleOffset(1.45);
  f2_no_1py->GetYaxis()->CenterTitle();
  f2_no_1py->SetLineStyle(2);
  f2_no_1py->SetLineWidth(2);
  f2_no_1py->SetMaximum(0.5);
  f2_no_1py->SetMinimum(0.00005);
  f2_no_1py->SetLineColor(14);
  TH1D *f2_aytai[6];
  f2_aytai[3] = (TH1D*) f2->Get("0900/NPiste_0900_K")->Clone();
  f2_aytai[1] = (TH1D*) f2->Get("0901/NPiste_0901_K")->Clone();
  f2_aytai[5] = (TH1D*) f2->Get("0904/NPiste_0904_K")->Clone();
  f2_aytai[2] = (TH1D*) f2->Get("0905/NPiste_0905_K")->Clone();
  f2_aytai[4] = (TH1D*) f2->Get("2110/NPiste_2110_K")->Clone();
  f2_aytai[0] = (TH1D*) f2->Get("2111/NPiste_2111_K")->Clone();
  for (int i=0; i<6; i++) {
   float entrees = f2_aytai[i]->GetEntries();
   printf(" i %d entrees %f\n",i,entrees);
   f2_aytai[i]->Scale(1./entrees);
   f2_aytai[i]->GetXaxis()->SetLabelSize(0.065);
   f2_aytai[i]->GetXaxis()->SetLabelOffset(0.015);
   f2_aytai[i]->GetYaxis()->SetLabelSize(0.060);
   f2_aytai[i]->GetYaxis()->SetLabelOffset(0.015);
   f2_aytai[i]->SetLineColor(14);
   f2_aytai[i]->SetLineStyle(2);
   f2_aytai[i]->SetLineWidth(2);
  }
  TH1D *f2_ayocc[6];
  f2_ayocc[3] = (TH1D*) f2->Get("0900/OccupancyCOG_0900")->Clone();
  f2_ayocc[1] = (TH1D*) f2->Get("0901/OccupancyCOG_0901")->Clone();
  f2_ayocc[5] = (TH1D*) f2->Get("0904/OccupancyCOG_0904")->Clone();
  f2_ayocc[2] = (TH1D*) f2->Get("0905/OccupancyCOG_0905")->Clone();
  f2_ayocc[4] = (TH1D*) f2->Get("2110/OccupancyCOG_2110")->Clone();
  f2_ayocc[0] = (TH1D*) f2->Get("2111/OccupancyCOG_2111")->Clone();
  for (int i=0; i<6; i++) {
   float entrees = f1_ayocc[i]->GetEntries();
   f2_ayocc[i]->Scale(2./entrees);
   printf(" i %d entrees %f\n",i,entrees);
   f2_ayocc[i]->GetXaxis()->SetLabelSize(0.065);
   f2_ayocc[i]->GetXaxis()->SetLabelOffset(0.015);
   f2_ayocc[i]->GetXaxis()->SetRangeUser(640.,1023);
   f2_ayocc[i]->GetYaxis()->SetLabelSize(0.060);
   f2_ayocc[i]->GetYaxis()->SetLabelOffset(0.015);
   f2_ayocc[i]->SetLineColor(14);
  }
  TH1D *f2_ayeta[6];
  f2_ayeta[3] = (TH1D*) f2->Get("0900/Eta_0900_K")->Clone();
  f2_ayeta[1] = (TH1D*) f2->Get("0901/Eta_0901_K")->Clone();
  f2_ayeta[5] = (TH1D*) f2->Get("0904/Eta_0904_K")->Clone();
  f2_ayeta[2] = (TH1D*) f2->Get("0905/Eta_0905_K")->Clone();
  f2_ayeta[4] = (TH1D*) f2->Get("2110/Eta_2110_K")->Clone();
  f2_ayeta[0] = (TH1D*) f2->Get("2111/Eta_2111_K")->Clone();
  for (int i=0; i<6; i++) {
   float entrees = f2_ayeta[i]->GetEntries();
   f2_ayeta[i]->Scale(1./entrees);
   printf(" i %d entrees %f\n",i,entrees);
   f2_ayeta[i]->GetXaxis()->SetLabelSize(0.065);
   f2_ayeta[i]->GetXaxis()->SetLabelOffset(0.015);
   f2_ayeta[i]->GetXaxis()->SetTitle("#eta");
   f2_ayeta[i]->GetXaxis()->SetTitleSize(0.075);
   f2_ayeta[i]->GetXaxis()->SetTitleOffset(1.00);
   f2_ayeta[i]->GetXaxis()->CenterTitle();
   f2_ayeta[i]->GetYaxis()->SetLabelSize(0.060);
   f2_ayeta[i]->GetYaxis()->SetLabelOffset(0.015);
   f2_ayeta[i]->GetYaxis()->SetTitle("Fraction");
   f2_ayeta[i]->GetYaxis()->SetTitleSize(0.065);
   f2_ayeta[i]->GetYaxis()->SetTitleOffset(1.40);
   f2_ayeta[i]->GetYaxis()->CenterTitle();
   f2_ayeta[i]->SetLineColor(14);
  }
  TH1D *f2_yposhr[6];
  for (int i=0; i<6; i++) {
    sprintf(chist,"y%dhr",i);
    printf("%s\n",chist);
    f2_yposhr[lec_pos_conv[i]] = (TH1D*) f2->Get(chist)->Clone();
    f2_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelSize(0.065);
    f2_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelOffset(0.015);
    f2_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitle("Y (mm)");
    f2_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleSize(0.075);
    f2_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleOffset(1.00);
    f2_yposhr[lec_pos_conv[i]]->GetXaxis()->CenterTitle();
    f2_yposhr[lec_pos_conv[i]]->GetXaxis()->SetRangeUser(-0.3,0.3);
    f2_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelSize(0.065);
    f2_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f2_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitle("Entrees");
    f2_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleSize(0.065);
    f2_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleOffset(1.00);
    f2_yposhr[lec_pos_conv[i]]->GetYaxis()->CenterTitle();
    f2_yposhr[lec_pos_conv[i]]->GetYaxis()->SetRangeUser(0.5,300.);
    f2_yposhr[lec_pos_conv[i]]->SetLineColor(4);
    f2_yposhr[lec_pos_conv[i]]->SetLineStyle(2);
  }
  TH1D *f2_ypos1p[6];
  for (int i=0; i<6; i++) {
    sprintf(chist,"y%d1p",i);
    printf("%s\n",chist);
    f2_ypos1p[lec_pos_conv[i]] = (TH1D*) f2->Get(chist)->Clone();
    f2_ypos1p[lec_pos_conv[i]]->SetLineColor(1);
    f2_ypos1p[lec_pos_conv[i]]->SetFillColor(1);
    f2_ypos1p[lec_pos_conv[i]]->SetLineStyle(1);
  }
  TH1D *f2_ptdydz_fais = (TH1D*) f2->Get("dydz_fais")->Clone();
  f2_ptdydz_fais->SetLineColor(4);
  f2_ptdydz_fais->SetLineStyle(2);
  f2_ptdydz_fais->SetLineWidth(2);
  TH2D *f2_ptdydz = (TH2D*) f2->Get("dydz")->Clone();
  f2_ptdydz->SetMarkerColor(4);
  TH1D *f2_dydz_proy[15];
  for (int ib=1; ib<16; ib++) {
    sprintf(chist,"proy_%d",ib);
    f2_dydz_proy[ib-1] = f2_ptdydz->ProjectionY(chist,ib,ib,"");
    printf(" bin %d moyen %f rms %f\n",ib,fac*f2_dydz_proy[ib-1]->GetMean(),fac*f2_dydz_proy[ib-1]->GetRMS());
  }

  TFile *f3 = new TFile("Hist_1234_a6_1_2_6.root");
  TH1D *f3_yposhr[6];
  for (int i=0; i<6; i++) {
    sprintf(chist,"y%dhr",i);
    f3_yposhr[lec_pos_conv[i]] = (TH1D*) f3->Get(chist)->Clone();
    f3_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelSize(0.065);
    f3_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelOffset(0.015);
    f3_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitle("Y (mm)");
    f3_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleSize(0.075);
    f3_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleOffset(1.00);
    f3_yposhr[lec_pos_conv[i]]->GetXaxis()->CenterTitle();
    f3_yposhr[lec_pos_conv[i]]->GetXaxis()->SetRangeUser(-0.3,0.3);
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelSize(0.065);
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitle("Entrees");
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleSize(0.065);
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleOffset(1.00);
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->CenterTitle();
    if ( i < 4 )
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->SetRangeUser(1.0,500.);
    else
    f3_yposhr[lec_pos_conv[i]]->GetYaxis()->SetRangeUser(10.0,300.);
    f3_yposhr[lec_pos_conv[i]]->SetLineColor(4);
  }
  TH1D *f3_ptdydz_fais = (TH1D*) f3->Get("dydz_fais")->Clone();
  f3_ptdydz_fais->SetLineColor(4);
  f3_ptdydz_fais->SetLineStyle(1);
  f3_ptdydz_fais->SetLineWidth(2);
  TH2D *f3_ptdydz = (TH2D*) f3->Get("dydz")->Clone();
  TH1D *f3_dydz_proy[15];
  for (int ib=1; ib<16; ib++) {
    sprintf(chist,"proy_%d",ib);
    f3_dydz_proy[ib-1] = f3_ptdydz->ProjectionY(chist,ib,ib,"");
    printf(" bin %d moyen %f rms %f\n",ib,fac*f3_dydz_proy[ib-1]->GetMean(),fac*f3_dydz_proy[ib-1]->GetRMS());
  }

  TFile *f4 = new TFile("Hist_1234_a6_1_2_6.root");
  TH1D *f4_yposhr[6];
  for (int i=0; i<6; i++) {
    sprintf(chist,"y%dhr",i);
    f4_yposhr[lec_pos_conv[i]] = (TH1D*) f4->Get(chist)->Clone();
    f4_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelSize(0.065);
    f4_yposhr[lec_pos_conv[i]]->GetXaxis()->SetLabelOffset(0.015);
    f4_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitle("Y (mm)");
    f4_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleSize(0.075);
    f4_yposhr[lec_pos_conv[i]]->GetXaxis()->SetTitleOffset(1.00);
    f4_yposhr[lec_pos_conv[i]]->GetXaxis()->CenterTitle();
    f4_yposhr[lec_pos_conv[i]]->GetXaxis()->SetRangeUser(-0.3,0.3);
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelSize(0.065);
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->SetLabelOffset(0.0025);
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitle("Entrees");
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleSize(0.065);
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->SetTitleOffset(1.00);
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->CenterTitle();
    if ( i < 4 )
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->SetRangeUser(1.0,500.);
    else
    f4_yposhr[lec_pos_conv[i]]->GetYaxis()->SetRangeUser(10.0,300.);
    f4_yposhr[lec_pos_conv[i]]->SetLineColor(4);
  }
  TH1D *f4_ptdydz_fais = (TH1D*) f4->Get("dydz_fais")->Clone();
  f4_ptdydz_fais->SetLineColor(4);
  f4_ptdydz_fais->SetLineStyle(1);
  f4_ptdydz_fais->SetLineWidth(2);
  TH2D *f4_ptdydz = (TH2D*) f4->Get("dydz")->Clone();
  TH1D *f4_dydz_proy[15];
  for (int ib=1; ib<16; ib++) {
    sprintf(chist,"proy_%d",ib);
    f4_dydz_proy[ib-1] = f4_ptdydz->ProjectionY(chist,ib,ib,"");
    printf(" bin %d moyen %f rms %f\n",ib,fac*f4_dydz_proy[ib-1]->GetMean(),fac*f4_dydz_proy[ib-1]->GetRMS());
  }

  TCanvas* c0=new TCanvas("c0","no_1p",0,0,650,600);
  tx = new TLatex(0.25,0.04,"no. single-strip clusters per track");
  tx->SetNDC();
  tx->SetTextSize(0.04);
  tx->Draw();
  c0->Divide(2,1,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->SetFillStyle(4000);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.18);
  c0_1->SetBottomMargin(0.15);
  c0_1->SetRightMargin(0.05);
  c0_1->SetTopMargin(0.05);
  c0_1->SetLogy(1);
  c0_2->SetFillColor(10);
  c0_2->SetFillStyle(4000);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.18);
  c0_2->SetBottomMargin(0.15);
  c0_2->SetRightMargin(0.05);
  c0_2->SetTopMargin(0.05);
  c0_2->SetLogy(1);
  c0->cd(1);
  f2_no_1px->Draw("hist");
  f1_no_1px->Draw("samehist");
  tcotep = new TLatex(0.47,0.90,"p-side");
  tcotep->SetNDC();
  tcotep->SetTextSize(0.065);
  tcotep->SetTextColor(2);
  tcotep->Draw();
  c0->cd(2);
  f2_no_1py->Draw("hist");
  f1_no_1py->Draw("samehist");
  tcoten = new TLatex(0.47,0.90,"n-side");
  tcoten->SetNDC();
  tcoten->SetTextSize(0.065);
  tcoten->SetTextColor(4);
  tcoten->Draw();

  TCanvas* c1=new TCanvas("c1","taille amas y",0,0,650,650);
  c1->Divide(2,3,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.15);
  c1_1->SetBottomMargin(0.18);
  c1_1->SetRightMargin(0.05);
  c1_1->SetTopMargin(0.05);
  c1_1->SetLogy(1);
  c1_2->SetFillColor(10);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.15);
  c1_2->SetBottomMargin(0.18);
  c1_2->SetRightMargin(0.05);
  c1_2->SetTopMargin(0.05);
  c1_2->SetLogy(1);
  c1_3->SetFillColor(10);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.15);
  c1_3->SetBottomMargin(0.18);
  c1_3->SetRightMargin(0.05);
  c1_3->SetTopMargin(0.05);
  c1_3->SetLogy(1);
  c1_4->SetFillColor(10);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.15);
  c1_4->SetBottomMargin(0.18);
  c1_4->SetRightMargin(0.05);
  c1_4->SetTopMargin(0.05);
  c1_4->SetLogy(1);
  c1_5->SetFillColor(10);
  c1_5->Range(-10,-10,10,10);
  c1_5->SetLeftMargin(0.15);
  c1_5->SetBottomMargin(0.18);
  c1_5->SetRightMargin(0.05);
  c1_5->SetTopMargin(0.05);
  c1_5->SetLogy(1);
  c1_6->SetFillColor(10);
  c1_6->Range(-10,-10,10,10);
  c1_6->SetLeftMargin(0.15);
  c1_6->SetBottomMargin(0.18);
  c1_6->SetRightMargin(0.05);
  c1_6->SetTopMargin(0.05);
  c1_6->SetLogy(1);
  for (int i=1; i<7; i++) {
    c1->cd(i);
    f1_aytai[i-1]->Draw("hist");
    f2_aytai[i-1]->Draw("samehist");    
    char cpos[10];
    sprintf(cpos,"Y%d",i-1);
    tpos = new TLatex(0.85,0.85,cpos);
    tpos->SetTextSize(0.080);
    tpos->SetTextColor(4);
    tpos->SetNDC();
    tpos->Draw();
    if (i == 3) {
      tang = new TLatex(0.86,0.77,"0^{o}");
      tang->SetTextSize(0.080);
      tang->SetTextColor(4);
      tang->SetNDC();
      tang->Draw();
    }   
  }
  for (int i=0; i<6; i++) {
    float frac_f1 = f1_aytai[i]->GetBinContent(1);
    float frac_f2 = f2_aytai[i]->GetBinContent(1);
    printf("i %d ff1 %f ff2 %f rapp %f\n",i,frac_f1,frac_f2,frac_f2/frac_f1);
  }

  TCanvas* c2=new TCanvas("c2","occ amas y",0,0,650,650);
  c2->Divide(2,3,0.0001,0.0001);
  c2_1->SetFillColor(10);
  c2_1->Range(-10,-10,10,10);
  c2_1->SetLeftMargin(0.15);
  c2_1->SetBottomMargin(0.18);
  c2_1->SetRightMargin(0.05);
  c2_1->SetTopMargin(0.05);
  c2_1->SetLogy(1);
  c2_2->SetFillColor(10);
  c2_2->Range(-10,-10,10,10);
  c2_2->SetLeftMargin(0.15);
  c2_2->SetBottomMargin(0.18);
  c2_2->SetRightMargin(0.05);
  c2_2->SetTopMargin(0.05);
  c2_2->SetLogy(1);
  c2_3->SetFillColor(10);
  c2_3->Range(-10,-10,10,10);
  c2_3->SetLeftMargin(0.15);
  c2_3->SetBottomMargin(0.18);
  c2_3->SetRightMargin(0.05);
  c2_3->SetTopMargin(0.05);
  c2_3->SetLogy(1);
  c2_4->SetFillColor(10);
  c2_4->Range(-10,-10,10,10);
  c2_4->SetLeftMargin(0.15);
  c2_4->SetBottomMargin(0.18);
  c2_4->SetRightMargin(0.05);
  c2_4->SetTopMargin(0.05);
  c2_4->SetLogy(1);
  c2_5->SetFillColor(10);
  c2_5->Range(-10,-10,10,10);
  c2_5->SetLeftMargin(0.15);
  c2_5->SetBottomMargin(0.18);
  c2_5->SetRightMargin(0.05);
  c2_5->SetTopMargin(0.05);
  c2_5->SetLogy(1);
  c2_6->SetFillColor(10);
  c2_6->Range(-10,-10,10,10);
  c2_6->SetLeftMargin(0.15);
  c2_6->SetBottomMargin(0.18);
  c2_6->SetRightMargin(0.05);
  c2_6->SetTopMargin(0.05);
  c2_6->SetLogy(1);
  for (int i=1; i<7; i++) {
    c2->cd(i);
    f1_ayocc[i-1]->Draw("hist");
    f2_ayocc[i-1]->Draw("samehist");
    char cpos[10];
    sprintf(cpos,"Y%d",i-1);
    tpos = new TLatex(0.85,0.85,cpos);
    tpos->SetTextSize(0.080);
    tpos->SetTextColor(4);
    tpos->SetNDC();
    tpos->Draw();
    if (i == 3) {
      tang = new TLatex(0.86,0.77,"0^{o}");
      tang->SetTextSize(0.080);
      tang->SetTextColor(4);
      tang->SetNDC();
      tang->Draw();
    }   
  }

  TCanvas* c3=new TCanvas("c3","eta amas y",0,0,650,650);
  c3->Divide(2,3,0.0001,0.0001);
  c3_1->SetFillColor(10);
  c3_1->Range(-10,-10,10,10);
  c3_1->SetLeftMargin(0.18);
  c3_1->SetBottomMargin(0.18);
  c3_1->SetRightMargin(0.05);
  c3_1->SetTopMargin(0.05);
  c3_1->SetLogy(0);
  c3_2->SetFillColor(10);
  c3_2->Range(-10,-10,10,10);
  c3_2->SetLeftMargin(0.18);
  c3_2->SetBottomMargin(0.18);
  c3_2->SetRightMargin(0.05);
  c3_2->SetTopMargin(0.05);
  c3_2->SetLogy(0);
  c3_3->SetFillColor(10);
  c3_3->Range(-10,-10,10,10);
  c3_3->SetLeftMargin(0.18);
  c3_3->SetBottomMargin(0.18);
  c3_3->SetRightMargin(0.05);
  c3_3->SetTopMargin(0.05);
  c3_3->SetLogy(0);
  c3_4->SetFillColor(10);
  c3_4->Range(-10,-10,10,10);
  c3_4->SetLeftMargin(0.18);
  c3_4->SetBottomMargin(0.18);
  c3_4->SetRightMargin(0.05);
  c3_4->SetTopMargin(0.05);
  c3_4->SetLogy(0);
  c3_5->SetFillColor(10);
  c3_5->Range(-10,-10,10,10);
  c3_5->SetLeftMargin(0.18);
  c3_5->SetBottomMargin(0.18);
  c3_5->SetRightMargin(0.05);
  c3_5->SetTopMargin(0.05);
  c3_5->SetLogy(0);
  c3_6->SetFillColor(10);
  c3_6->Range(-10,-10,10,10);
  c3_6->SetLeftMargin(0.18);
  c3_6->SetBottomMargin(0.18);
  c3_6->SetRightMargin(0.05);
  c3_6->SetTopMargin(0.05);
  c3_6->SetLogy(0);
  for (int i=1; i<7; i++) {
    c3->cd(i);
    if (i == 1 || i == 3) {
    f2_ayeta[i-1]->Draw("hist");
    f1_ayeta[i-1]->Draw("samehist");
    }
    else {
    f1_ayeta[i-1]->Draw("hist");
    f2_ayeta[i-1]->Draw("samehist");
    }    
    char cpos[10];
    sprintf(cpos,"Y%d",i-1);
    tpos = new TLatex(0.85,0.85,cpos);
    tpos->SetTextSize(0.080);
    tpos->SetTextColor(4);
    tpos->SetNDC();
    tpos->Draw();
    if (i == 3) {
      tang = new TLatex(0.86,0.77,"0^{o}");
      tang->SetTextSize(0.080);
      tang->SetTextColor(4);
      tang->SetNDC();
      tang->Draw();
    }   
  }

  TCanvas* c4=new TCanvas("c4","ypos hr",0,0,650,650);
  c4->Divide(2,3,0.0001,0.0001);
  c4_1->SetFillColor(10);
  c4_1->Range(-10,-10,10,10);
  c4_1->SetLeftMargin(0.15);
  c4_1->SetBottomMargin(0.18);
  c4_1->SetRightMargin(0.05);
  c4_1->SetTopMargin(0.05);
  c4_1->SetLogy(1);
  c4_2->SetFillColor(10);
  c4_2->Range(-10,-10,10,10);
  c4_2->SetLeftMargin(0.15);
  c4_2->SetBottomMargin(0.18);
  c4_2->SetRightMargin(0.05);
  c4_2->SetTopMargin(0.05);
  c4_2->SetLogy(1);
  c4_3->SetFillColor(10);
  c4_3->Range(-10,-10,10,10);
  c4_3->SetLeftMargin(0.15);
  c4_3->SetBottomMargin(0.18);
  c4_3->SetRightMargin(0.05);
  c4_3->SetTopMargin(0.05);
  c4_3->SetLogy(1);
  c4_4->SetFillColor(10);
  c4_4->Range(-10,-10,10,10);
  c4_4->SetLeftMargin(0.15);
  c4_4->SetBottomMargin(0.18);
  c4_4->SetRightMargin(0.05);
  c4_4->SetTopMargin(0.05);
  c4_4->SetLogy(1);
  c4_5->SetFillColor(10);
  c4_5->Range(-10,-10,10,10);
  c4_5->SetLeftMargin(0.15);
  c4_5->SetBottomMargin(0.18);
  c4_5->SetRightMargin(0.05);
  c4_5->SetTopMargin(0.05);
  c4_5->SetLogy(1);
  c4_6->SetFillColor(10);
  c4_6->Range(-10,-10,10,10);
  c4_6->SetLeftMargin(0.15);
  c4_6->SetBottomMargin(0.18);
  c4_6->SetRightMargin(0.05);
  c4_6->SetTopMargin(0.05);
  c4_6->SetLogy(1);
  for (int i=1; i<7; i++) {
    c4->cd(i);
    f4_yposhr[i-1]->Draw("hist");
    f2_yposhr[i-1]->Draw("samehist");
    f2_ypos1p[i-1]->Draw("samehist");
    char cpos[10];
    sprintf(cpos,"Y%d",i-1);
    tpos = new TLatex(0.85,0.85,cpos);
    tpos->SetTextSize(0.080);
    tpos->SetTextColor(4);
    tpos->SetNDC();
    tpos->Draw();
    if (i == 3) {
      tang = new TLatex(0.86,0.77,"0^{o}");
      tang->SetTextSize(0.080);
      tang->SetTextColor(4);
      tang->SetNDC();
      tang->Draw();
    }   
  }

  TF1 *gau2yz1 = new TF1("gau2yz1","gaus(0)+gaus(3)",-0.0003,0.0003);
  gau2yz1->SetLineWidth(2);
  gau2yz1->SetLineColor(1);
  gau2yz1->SetLineStyle(2);

  TF1 *gau2yz2 = new TF1("gau2yz2","gaus(0)+gaus(3)",-0.0003,0.0003);
  gau2yz2->SetLineWidth(2);
  gau2yz2->SetLineColor(1);
  gau2yz2->SetLineStyle(2);

  Double_t par[6];

  TCanvas* c5=new TCanvas("c5","dydz hr 1234",0,0,450,450);
  c5->SetFillColor(10);
  c5->Range(-10,-10,10,10);
  c5->SetLeftMargin(0.15);
  c5->SetBottomMargin(0.15);
  c5->SetRightMargin(0.12);
  c5->SetTopMargin(0.12);
  c5->SetLogy(0);
  c5->cd();
  f3_ptdydz_fais->GetXaxis()->SetTitle("#DeltaY/#DeltaZ (rad)");
  f3_ptdydz_fais->GetXaxis()->SetTitleSize(0.050);
  f3_ptdydz_fais->GetXaxis()->CenterTitle();
  f3_ptdydz_fais->GetYaxis()->SetTitle("Entrees");
  f3_ptdydz_fais->GetYaxis()->SetTitleSize(0.050);
  f3_ptdydz_fais->GetYaxis()->SetTitleOffset(1.50);
  f3_ptdydz_fais->GetYaxis()->CenterTitle();
  gau2yz1->SetParameters(1500.,0.,0.00004,1000.,0.,0.00004);
  f1_ptdydz_fais->Fit(gau2yz1,"V+"); 
  gau2yz1->GetParameters(par);
  f3_ptdydz_fais->Draw("hist");
  f1_ptdydz_fais->Draw("samehist");
  gau2yz1->Draw("same");
  float atot = par[0]*par[2] + par[3]*par[5];
  printf("sig1 %f centre %f (urad) frac %f\n",1000000.*par[2],
	 1000000.*par[1],par[0]*par[2]/atot); 
  printf("sig2 %f centre %f (urad) frac %f\n",1000000.*par[5],
	 1000000.*par[4],par[3]*par[5]/atot); 
  tsig1_1 = new TLatex(0.60,0.80,"#sigma_{1} = 47.9 #murad");
  tsig1_1->SetNDC();
  tsig1_1->SetTextSize(0.04);
  tsig1_1->Draw();
  tsig2_1 = new TLatex(0.60,0.75,"#sigma_{2} = 21.8 #murad");
  tsig2_1->SetNDC();
  tsig2_1->SetTextSize(0.04);
  tsig2_1->Draw();

  TLatex *tcomb[15];
  for (int i=0; i<15; i++) {
    printf("i %d\n",i);
    switch(i) {
    case 0: tcomb[i] = new TLatex(0.80,0.75,"0-1"); break;
    case 1: tcomb[i] = new TLatex(0.80,0.75,"0-2"); break;
    case 2: tcomb[i] = new TLatex(0.80,0.75,"0-3"); break;
    case 3: tcomb[i] = new TLatex(0.80,0.75,"0-4"); break;
    case 4: tcomb[i] = new TLatex(0.80,0.75,"0-5"); break;
    case 5: tcomb[i] = new TLatex(0.80,0.75,"1-2"); break;
    case 6: tcomb[i] = new TLatex(0.80,0.75,"1-3"); break;
    case 7: tcomb[i] = new TLatex(0.80,0.75,"1-4"); break;
    case 8: tcomb[i] = new TLatex(0.80,0.75,"1-5"); break;
    case 9: tcomb[i] = new TLatex(0.80,0.75,"2-3"); break;
    case 10: tcomb[i] = new TLatex(0.80,0.75,"2-4"); break;
    case 11: tcomb[i] = new TLatex(0.80,0.75,"2-5"); break;
    case 12: tcomb[i] = new TLatex(0.80,0.75,"3-4"); break;
    case 13: tcomb[i] = new TLatex(0.80,0.75,"3-5"); break;
    case 14: tcomb[i] = new TLatex(0.80,0.75,"4-5"); break;
    default:
    }
    tcomb[i]->SetNDC();
    tcomb[i]->SetTextColor(1);
    tcomb[i]->SetTextSize(0.070);
  }

  TCanvas* c6=new TCanvas("c6","dydz proy 1234",0,0,800,800);
  c6->SetFillColor(10);
  c6->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<15; i++) {
      c6->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.20);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.05);
      gPad->SetTopMargin(0.10);
      gPad->SetLogy(0);
      f1_dydz_proy[i]->SetLineColor(4);
      f1_dydz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      f1_dydz_proy[i]->GetXaxis()->SetLabelSize(0.055);
      f1_dydz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      f1_dydz_proy[i]->GetYaxis()->SetLabelSize(0.050);
      f1_dydz_proy[i]->GetXaxis()->SetNdivisions(-402);
      f1_dydz_proy[i]->GetXaxis()->SetTitle("dy/dz (rad)");
      f1_dydz_proy[i]->GetXaxis()->SetTitleSize(0.065);
      f1_dydz_proy[i]->GetXaxis()->CenterTitle();
      f1_dydz_proy[i]->GetYaxis()->SetTitle("Entrees");
      f1_dydz_proy[i]->GetYaxis()->SetTitleSize(0.060);
      f1_dydz_proy[i]->GetYaxis()->SetTitleOffset(1.40);
      f1_dydz_proy[i]->GetYaxis()->CenterTitle();
      f1_dydz_proy[i]->SetLineStyle(2);
      f3_dydz_proy[i]->SetLineColor(4);
      f3_dydz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      f3_dydz_proy[i]->GetXaxis()->SetLabelSize(0.050);
      f3_dydz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      f3_dydz_proy[i]->GetYaxis()->SetLabelSize(0.050);
      f3_dydz_proy[i]->GetXaxis()->SetNdivisions(-402);
      f3_dydz_proy[i]->GetXaxis()->SetTitle("dy/dz (rad)");
      f3_dydz_proy[i]->GetXaxis()->SetTitleSize(0.06);
      f3_dydz_proy[i]->GetXaxis()->CenterTitle();
      f3_dydz_proy[i]->GetYaxis()->SetTitle("Entrees");
      f3_dydz_proy[i]->GetYaxis()->SetTitleSize(0.060);
      f3_dydz_proy[i]->GetYaxis()->SetTitleOffset(1.40);
      f3_dydz_proy[i]->GetYaxis()->CenterTitle();
      switch(i) {
       case 0:
          f1_dydz_proy[i]->Draw("hist");
          f3_dydz_proy[i]->Draw("samehist");
          break;
       case 1: 
          f1_dydz_proy[i]->Draw("hist");
          f3_dydz_proy[i]->Draw("samehist");
          break;
       case 2:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 3:
          f1_dydz_proy[i]->Draw("hist");
          f3_dydz_proy[i]->Draw("samehist");
          break;
       case 4:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 5:
          f1_dydz_proy[i]->Draw("hist");
          f3_dydz_proy[i]->Draw("samehist");
          break;
       case 6:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 7:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 8:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 9:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 10:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 11:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 12:
          f1_dydz_proy[i]->Draw("hist");
          f3_dydz_proy[i]->Draw("samehist");
          break;
       case 13:
          f3_dydz_proy[i]->Draw("hist");
          f1_dydz_proy[i]->Draw("samehist");
          break;
       case 14:
          f1_dydz_proy[i]->Draw("hist");
          f3_dydz_proy[i]->Draw("samehist");
          break;
       default:
      }
      tcomb[i]->Draw();
  }

  TCanvas* c7=new TCanvas("c7","dydz hr 1251",0,0,450,450);
  c7->SetFillColor(10);
  c7->Range(-10,-10,10,10);
  c7->SetLeftMargin(0.15);
  c7->SetBottomMargin(0.15);
  c7->SetRightMargin(0.12);
  c7->SetTopMargin(0.12);
  c7->SetLogy(0);
  c7->cd();
  f4_ptdydz_fais->GetXaxis()->SetTitle("#DeltaY/#DeltaZ (rad)");
  f4_ptdydz_fais->GetXaxis()->SetTitleSize(0.050);
  f4_ptdydz_fais->GetXaxis()->CenterTitle();
  f4_ptdydz_fais->GetYaxis()->SetTitle("Entrees");
  f4_ptdydz_fais->GetYaxis()->SetTitleSize(0.050);
  f4_ptdydz_fais->GetYaxis()->SetTitleOffset(1.50);
  f4_ptdydz_fais->GetYaxis()->CenterTitle();
  gau2yz2->SetParameters(1500.,0.,0.00004,100.,0.,0.00010);
  f2_ptdydz_fais->Fit(gau2yz2,"V+"); 
  gau2yz2->GetParameters(par);
  f4_ptdydz_fais->Draw("hist");
  f2_ptdydz_fais->Draw("samehist");
  gau2yz2->Draw("same");
  atot = par[0]*par[2] + par[3]*par[5];
  printf("sig1 %f centre %f (urad) frac %f\n",1000000.*par[2],
	 1000000.*par[1],par[0]*par[2]/atot); 
  printf("sig2 %f centre %f (urad) frac %f\n",1000000.*par[5],
	 1000000.*par[4],par[3]*par[5]/atot); 
  float sig12 = par[0]*par[2]*par[2] + par[3]*par[5]*par[5];
  sig12/=atot;
  printf("sig12 %f (urad)\n",1000000.*sig12);
  tsig1_2 = new TLatex(0.60,0.80,"#sigma_{1} = 30.3 #murad");
  tsig1_2->SetNDC();
  tsig1_2->SetTextSize(0.04);
  tsig1_2->Draw();
  tsig12_2 = new TLatex(0.60,0.75,"#sigma_{12} = 35.7 #murad");
  tsig12_2->SetNDC();
  tsig12_2->SetTextSize(0.04);
  tsig12_2->Draw();

  TCanvas* c8=new TCanvas("c8","dydz proy 1251",0,0,800,800);
  c8->SetFillColor(10);
  c8->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<15; i++) {
      c8->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.20);
      gPad->SetBottomMargin(0.15);
      gPad->SetRightMargin(0.05);
      gPad->SetTopMargin(0.10);
      gPad->SetLogy(0);
      f2_dydz_proy[i]->SetLineColor(4);
      f2_dydz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      f2_dydz_proy[i]->GetXaxis()->SetLabelSize(0.055);
      f2_dydz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      f2_dydz_proy[i]->GetYaxis()->SetLabelSize(0.050);
      f2_dydz_proy[i]->GetXaxis()->SetNdivisions(-402);
      f2_dydz_proy[i]->GetXaxis()->SetTitle("dy/dz (rad)");
      f2_dydz_proy[i]->GetXaxis()->SetTitleSize(0.065);
      f2_dydz_proy[i]->GetXaxis()->CenterTitle();
      f2_dydz_proy[i]->GetYaxis()->SetTitleSize(0.060);
      f2_dydz_proy[i]->GetYaxis()->SetTitleOffset(1.40);
      f2_dydz_proy[i]->GetYaxis()->SetTitle("Entrees");
      f2_dydz_proy[i]->GetYaxis()->CenterTitle();
      f2_dydz_proy[i]->SetLineStyle(2);
      f4_dydz_proy[i]->SetLineColor(4);
      f4_dydz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      f4_dydz_proy[i]->GetXaxis()->SetLabelSize(0.055);
      f4_dydz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      f4_dydz_proy[i]->GetYaxis()->SetLabelSize(0.050);
      f4_dydz_proy[i]->GetXaxis()->SetNdivisions(-402);
      f4_dydz_proy[i]->GetXaxis()->SetTitle("dy/dz (rad)");
      f4_dydz_proy[i]->GetXaxis()->SetTitleSize(0.065);
      f4_dydz_proy[i]->GetXaxis()->CenterTitle();
      f4_dydz_proy[i]->GetYaxis()->SetTitle("Entrees");
      f4_dydz_proy[i]->GetYaxis()->SetTitleSize(0.060);
      f4_dydz_proy[i]->GetYaxis()->SetTitleOffset(1.40);
      f4_dydz_proy[i]->GetYaxis()->CenterTitle();
      switch(i) {
       case 0:
          f2_dydz_proy[i]->Draw("hist");
          f4_dydz_proy[i]->Draw("samehist");
          break;
       case 1: 
          f2_dydz_proy[i]->Draw("hist");
          f4_dydz_proy[i]->Draw("samehist");
          break;
       case 2:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 3:
          f2_dydz_proy[i]->Draw("hist");
          f4_dydz_proy[i]->Draw("samehist");
          break;
       case 4:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 5:
          f2_dydz_proy[i]->Draw("hist");
          f4_dydz_proy[i]->Draw("samehist");
          break;
       case 6:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 7:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 8:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 9:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 10:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 11:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 12:
          f2_dydz_proy[i]->Draw("hist");
          f4_dydz_proy[i]->Draw("samehist");
          break;
       case 13:
          f4_dydz_proy[i]->Draw("hist");
          f2_dydz_proy[i]->Draw("samehist");
          break;
       case 14:
          f2_dydz_proy[i]->Draw("hist");
          f4_dydz_proy[i]->Draw("samehist");
          break;
       default:
      }
      tcomb[i]->Draw();
  }

  return;

  TCanvas* c9=new TCanvas("c9","dydz comb 1234",0,0,500,500);
  c9->SetFillColor(10);
  c9->Range(-10,-10,10,10);
  c9->SetLeftMargin(0.15);
  c9->SetBottomMargin(0.18);
  c9->SetRightMargin(0.05);
  c9->SetTopMargin(0.05);
  c9->SetLogy(0);
  c9->cd();
  f1_ptdydz->SetMarkerColor(1);
  f1_ptdydz->Draw("");
  f4_ptdydz->SetMarkerColor(14);
  //  f4_ptdydz->Draw("same");

  TCanvas* c10=new TCanvas("c10","dydz comb 1251",0,0,500,500);
  c10->SetFillColor(10);
  c10->Range(-10,-10,10,10);
  c10->SetLeftMargin(0.15);
  c10->SetBottomMargin(0.18);
  c10->SetRightMargin(0.05);
  c10->SetTopMargin(0.05);
  c10->SetLogy(0);
  c10->cd();
  f2_ptdydz->SetMarkerColor(1);
  f2_ptdydz->Draw("");
  //  f3_ptdydz->SetMarkerColor(14);
  //  f3_ptdydz->Draw("same");
  
  return;
}

void occ() {

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","occupation",0,0,650,650);
  c0->Divide(2,3,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.12);
  c0_1->SetBottomMargin(0.18);
  c0_1->SetRightMargin(0.05);
  c0_1->SetTopMargin(0.05);
  c0_1->SetLogy(1);
  c0_2->SetFillColor(10);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.12);
  c0_2->SetBottomMargin(0.18);
  c0_2->SetRightMargin(0.05);
  c0_2->SetTopMargin(0.05);
  c0_2->SetLogy(1);
  c0_3->SetFillColor(10);
  c0_3->Range(-10,-10,10,10);
  c0_3->SetLeftMargin(0.12);
  c0_3->SetBottomMargin(0.18);
  c0_3->SetRightMargin(0.05);
  c0_3->SetTopMargin(0.05);
  c0_3->SetLogy(1);
  c0_4->SetFillColor(10);
  c0_4->Range(-10,-10,10,10);
  c0_4->SetLeftMargin(0.12);
  c0_4->SetBottomMargin(0.18);
  c0_4->SetRightMargin(0.05);
  c0_4->SetTopMargin(0.05);
  c0_4->SetLogy(1);
  c0_5->SetFillColor(10);
  c0_5->Range(-10,-10,10,10);
  c0_5->SetLeftMargin(0.12);
  c0_5->SetBottomMargin(0.18);
  c0_5->SetRightMargin(0.05);
  c0_5->SetTopMargin(0.05);
  c0_5->SetLogy(1);
  c0_6->SetFillColor(10);
  c0_6->Range(-10,-10,10,10);
  c0_6->SetLeftMargin(0.12);
  c0_6->SetBottomMargin(0.18);
  c0_6->SetRightMargin(0.05);
  c0_6->SetTopMargin(0.05);
  c0_6->SetLogz(1);

  // lecture 0, position faisceau 4eme AMS02
  c0->cd(4);
  f->cd("0900");
  Occupancy_0900->GetXaxis()->SetLabelSize(0.065);
  Occupancy_0900->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0900->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0900->Draw("hist");  
  // lecture 1, position faisceau 2eme AMS02
  c0->cd(2);
  f->cd("0901");
  Occupancy_0901->GetXaxis()->SetLabelSize(0.065);
  Occupancy_0901->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0901->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0901->Draw("hist");
  // lecture 2, position faisceau 6eme AMS02
  c0->cd(6);
  f->cd("0904");
  Occupancy_0904->GetXaxis()->SetLabelSize(0.065);
  Occupancy_0904->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0904->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0904->Draw("hist");
  // lecture 3, position faisceau 3eme AMS02
  c0->cd(3);
  f->cd("0905");
  Occupancy_0905->GetXaxis()->SetLabelSize(0.065);
  Occupancy_0905->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0905->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0905->Draw("hist");
  // lecture 4, position faisceau 5eme AMS02
  c0->cd(5);
  f->cd("2110");
  Occupancy_2110->GetXaxis()->SetLabelSize(0.065);
  Occupancy_2110->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_2110->GetYaxis()->SetLabelSize(0.060);
  Occupancy_2110->Draw("hist");
  // lecture 5, position faisceau 1ere AMS02
  c0->cd(1);
  f->cd("2111");
  Occupancy_2111->GetXaxis()->SetLabelSize(0.065);
  Occupancy_2111->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_2111->GetYaxis()->SetLabelSize(0.060);
  Occupancy_2111->Draw("hist");

  TCanvas* c1=new TCanvas("c1","occupation (norm)",0,0,650,650);
  c1->Divide(2,3,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.12);
  c1_1->SetBottomMargin(0.18);
  c1_1->SetRightMargin(0.05);
  c1_1->SetTopMargin(0.05);
  c1_1->SetLogy(1);
  c1_2->SetFillColor(10);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.12);
  c1_2->SetBottomMargin(0.18);
  c1_2->SetRightMargin(0.05);
  c1_2->SetTopMargin(0.05);
  c1_2->SetLogy(1);
  c1_3->SetFillColor(10);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.12);
  c1_3->SetBottomMargin(0.18);
  c1_3->SetRightMargin(0.05);
  c1_3->SetTopMargin(0.05);
  c1_3->SetLogy(1);
  c1_4->SetFillColor(10);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.12);
  c1_4->SetBottomMargin(0.18);
  c1_4->SetRightMargin(0.05);
  c1_4->SetTopMargin(0.05);
  c1_4->SetLogy(1);
  c1_5->SetFillColor(10);
  c1_5->Range(-10,-10,10,10);
  c1_5->SetLeftMargin(0.12);
  c1_5->SetBottomMargin(0.18);
  c1_5->SetRightMargin(0.05);
  c1_5->SetTopMargin(0.05);
  c1_5->SetLogy(1);
  c1_6->SetFillColor(10);
  c1_6->Range(-10,-10,10,10);
  c1_6->SetLeftMargin(0.12);
  c1_6->SetBottomMargin(0.18);
  c1_6->SetRightMargin(0.05);
  c1_6->SetTopMargin(0.05);
  c1_6->SetLogy(1);

  tx0 = new TLatex(0.210,0.885,"X0");  
  tx0->SetNDC();
  tx0->SetTextSize(0.07);  
  ty0 = new TLatex(0.725,0.885,"Y0");  
  ty0->SetNDC();
  ty0->SetTextSize(0.07);  
  tx1 = new TLatex(0.375,0.885,"X1");  
  tx1->SetNDC();
  tx1->SetTextSize(0.07);  
  ty1 = new TLatex(0.70,0.885,"Y1");  
  ty1->SetNDC();
  ty1->SetTextSize(0.07);  
  tx2 = new TLatex(0.375,0.885,"X2");  
  tx2->SetNDC();
  tx2->SetTextSize(0.07);  
  ty2 = new TLatex(0.85,0.885,"Y2");  
  ty2->SetNDC();
  ty2->SetTextSize(0.07);  
  tx3 = new TLatex(0.375,0.885,"X3");  
  tx3->SetNDC();
  tx3->SetTextSize(0.07);  
  ty3 = new TLatex(0.70,0.885,"Y3");  
  ty3->SetNDC();
  ty3->SetTextSize(0.07);  
  tx4 = new TLatex(0.21,0.885,"X4");  
  tx4->SetNDC();
  tx4->SetTextSize(0.07);  
  ty4 = new TLatex(0.725,0.885,"Y4");  
  ty4->SetNDC();
  ty4->SetTextSize(0.07);  
  tx5 = new TLatex(0.375,0.885,"X5");  
  tx5->SetNDC();
  tx5->SetTextSize(0.07);  
  ty5 = new TLatex(0.70,0.885,"Y5");  
  ty5->SetNDC();
  ty5->SetTextSize(0.07);  

  // lecture 0, position faisceau 4eme AMS02
  float max = 0.2;
  float min = 0.00001;
  c1->cd(4);
  TH1D* h0900 = f->Get("0900/Occupancy_0900")->Clone();
  h0900->GetXaxis()->SetLabelSize(0.065);
  h0900->GetXaxis()->SetLabelOffset(0.015);
  h0900->GetXaxis()->SetTitleSize(0.07);
  h0900->GetXaxis()->SetTitleOffset(1.0);
  h0900->GetXaxis()->SetTitle("channel");
  h0900->GetXaxis()->CenterTitle();
  h0900->GetYaxis()->SetTitleSize(0.07);
  h0900->GetYaxis()->SetTitleOffset(0.85);
  h0900->GetYaxis()->SetTitle("fraction");
  h0900->GetYaxis()->CenterTitle();
  h0900->GetYaxis()->SetLabelSize(0.060);
  int nb = h0900->GetNbinsX();
  int tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0900->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0900->GetBinContent(ib);
    h0900->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0900->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0900->GetBinContent(ib);
    h0900->SetBinContent(ib,val/tot);
  }
  h0900->SetMaximum(max);
  h0900->SetMinimum(min);
  h0900->Draw("hist");  
  TH1D* h0900cog = f->Get("0900/OccupancyCOG_0900")->Clone();
  nb = h0900cog->GetNbinsX();
  tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0900cog->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0900cog->GetBinContent(ib);
    h0900cog->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0900cog->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0900cog->GetBinContent(ib);
    h0900cog->SetBinContent(ib,val/tot);
  }
  h0900cog->SetLineColor(2);
  h0900cog->Draw("samehist");
  l640 = new TLine(640.,min,640.,max);
  l640->SetLineStyle(1);
  l640->SetLineColor(4);
  l640->SetLineWidth(2);
  l640->Draw();  
  l832 = new TLine(832.,min,832.,max);
  l832->SetLineStyle(2);
  l832->SetLineColor(4);
  l832->SetLineWidth(2);
  l832->Draw();
  tx3->Draw();
  ty3->Draw();
  
  // lecture 1, position faisceau 2eme AMS02
  c1->cd(2);
  TH1D* h0901 = f->Get("0901/Occupancy_0901")->Clone();
  h0901->GetXaxis()->SetLabelOffset(0.015);
  h0901->GetXaxis()->SetTitleSize(0.07);
  h0901->GetXaxis()->SetTitleOffset(1.0);
  h0901->GetXaxis()->SetTitle("channel");
  h0901->GetXaxis()->SetLabelSize(0.065);
  h0901->GetXaxis()->CenterTitle();
  h0901->GetYaxis()->SetLabelSize(0.060);
  h0901->GetYaxis()->SetTitleSize(0.07);
  h0901->GetYaxis()->SetTitleOffset(0.85);
  h0901->GetYaxis()->SetTitle("fraction");
  h0901->GetYaxis()->CenterTitle();
  h0901->GetYaxis()->SetLabelSize(0.060);
  int tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0901->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0901->GetBinContent(ib);
    h0901->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0901->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0901->GetBinContent(ib);
    h0901->SetBinContent(ib,val/tot);
  }
  h0901->SetMaximum(max);
  h0901->SetMinimum(min);
  h0901->Draw("hist");
  TH1D* h0901cog = f->Get("0901/OccupancyCOG_0901")->Clone();
  tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0901cog->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0901cog->GetBinContent(ib);
    h0901cog->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0901cog->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0901cog->GetBinContent(ib);
    h0901cog->SetBinContent(ib,val/tot);
  }
  h0901cog->SetLineColor(2);
  h0901cog->Draw("samehist");  
  l640->Draw();  
  l832->Draw();  
  tx1->Draw();
  ty1->Draw();

  // lecture 2, position faisceau 6eme AMS02
  c1->cd(6);
  TH1D* h0904 = f->Get("0904/Occupancy_0904")->Clone();
  h0904->GetXaxis()->SetLabelSize(0.065);
  h0904->GetXaxis()->SetLabelOffset(0.015);
  h0904->GetXaxis()->SetTitleSize(0.07);
  h0904->GetXaxis()->SetTitleOffset(1.0);
  h0904->GetXaxis()->SetTitle("channel");
  h0904->GetXaxis()->CenterTitle();
  h0904->GetYaxis()->SetLabelSize(0.060);
  h0904->GetYaxis()->SetTitleSize(0.07);
  h0904->GetYaxis()->SetTitleOffset(0.85);
  h0904->GetYaxis()->SetTitle("fraction");
  h0904->GetYaxis()->CenterTitle();
  h0904->GetYaxis()->SetLabelSize(0.060);
  int tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0904->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0904->GetBinContent(ib);
    h0904->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0904->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0904->GetBinContent(ib);
    h0904->SetBinContent(ib,val/tot);
  }
  h0904->SetMaximum(max);
  h0904->SetMinimum(min);
  h0904->Draw("hist");
  TH1D* h0904cog = f->Get("0904/OccupancyCOG_0904")->Clone();
  tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0904cog->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0904cog->GetBinContent(ib);
    h0904cog->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0904cog->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0904cog->GetBinContent(ib);
    h0904cog->SetBinContent(ib,val/tot);
  }
  h0904cog->SetLineColor(2);
  h0904cog->Draw("samehist");  
  l640->Draw();  
  l832->Draw();
  tx5->Draw();
  ty5->Draw();
  
  // lecture 3, position faisceau 3eme AMS02
  c1->cd(3);
  TH1D* h0905 = f->Get("0905/Occupancy_0905")->Clone();
  h0905->GetXaxis()->SetLabelSize(0.065);
  h0905->GetXaxis()->SetLabelOffset(0.015);
  h0905->GetXaxis()->SetTitleSize(0.07);
  h0905->GetXaxis()->SetTitleOffset(1.0);
  h0905->GetXaxis()->SetTitle("channel");
  h0905->GetXaxis()->CenterTitle();
  h0905->GetYaxis()->SetLabelSize(0.060);
  h0905->GetYaxis()->SetTitleSize(0.07);
  h0905->GetYaxis()->SetTitleOffset(0.85);
  h0905->GetYaxis()->SetTitle("fraction");
  h0905->GetYaxis()->CenterTitle();
  h0905->GetYaxis()->SetLabelSize(0.060);
  int tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0905->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0905->GetBinContent(ib);
    h0905->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0905->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0905->GetBinContent(ib);
    h0905->SetBinContent(ib,val/tot);
  }
  h0905->SetMaximum(max);
  h0905->SetMinimum(min);
  h0905->Draw("hist");
  l640->Draw();  
  l832->Draw();  
  TH1D* h0905cog = f->Get("0905/OccupancyCOG_0905")->Clone();
  tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h0905cog->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h0905cog->GetBinContent(ib);
    h0905cog->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h0905cog->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h0905cog->GetBinContent(ib);
    h0905cog->SetBinContent(ib,val/tot);
  }
  h0905cog->SetLineColor(2);
  h0905cog->Draw("samehist");  
  tx2->Draw();
  ty2->Draw();

  // lecture 4, position faisceau 5eme AMS02
  c1->cd(5);
  float maxb = 0.03;
  TH1D* h2110 = f->Get("2110/Occupancy_2110")->Clone();
  h2110->GetXaxis()->SetLabelSize(0.065);
  h2110->GetXaxis()->SetLabelOffset(0.015);
  h2110->GetXaxis()->SetTitleSize(0.07);
  h2110->GetXaxis()->SetTitleOffset(1.0);
  h2110->GetXaxis()->SetTitle("channel");
  h2110->GetXaxis()->CenterTitle();
  h2110->GetYaxis()->SetLabelSize(0.060);
  h2110->GetYaxis()->SetTitleSize(0.07);
  h2110->GetYaxis()->SetTitleOffset(0.85);
  h2110->GetYaxis()->SetTitle("fraction");
  h2110->GetYaxis()->CenterTitle();
  h2110->GetYaxis()->SetLabelSize(0.060);
  int tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h2110->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h2110->GetBinContent(ib);
    h2110->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h2110->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h2110->GetBinContent(ib);
    h2110->SetBinContent(ib,val/tot);
  }
  h2110->SetMaximum(maxb);
  h2110->SetMinimum(min);
  h2110->Draw("hist");
  TH1D* h2110cog = f->Get("2110/OccupancyCOG_2110")->Clone();
  tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h2110cog->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h2110cog->GetBinContent(ib);
    h2110cog->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h2110cog->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h2110cog->GetBinContent(ib);
    h2110cog->SetBinContent(ib,val/tot);
  }
  h2110cog->SetLineColor(2);
  h2110cog->Draw("samehist");  
  l640b = new TLine(640.,min,640.,maxb);
  l640b->SetLineStyle(1);
  l640b->SetLineColor(4);
  l640b->SetLineWidth(2);
  l640b->Draw();  
  l256b = new TLine(256.,min,256.,maxb);
  l256b->SetLineStyle(1);
  l256b->SetLineColor(1);
  l256b->SetLineWidth(2);
  l256b->Draw();  
  l896b = new TLine(896.,min,896.,maxb);
  l896b->SetLineStyle(1);
  l896b->SetLineColor(1);
  l896b->SetLineWidth(2);
  l896b->Draw();  
  tx4->Draw();
  ty4->Draw();

  // lecture 5, position faisceau 1ere AMS02
  c1->cd(1);
  TH1D* h2111 = f->Get("2111/Occupancy_2111")->Clone();
  h2111->GetXaxis()->SetLabelSize(0.065);
  h2111->GetXaxis()->SetLabelOffset(0.015);
  h2111->GetXaxis()->SetTitleSize(0.07);
  h2111->GetXaxis()->SetTitleOffset(1.0);
  h2111->GetXaxis()->SetTitle("channel");
  h2111->GetXaxis()->CenterTitle();
  h2111->GetYaxis()->SetLabelSize(0.060);
  h2111->GetYaxis()->SetTitleSize(0.07);
  h2111->GetYaxis()->SetTitleOffset(0.85);
  h2111->GetYaxis()->SetTitle("fraction");
  h2111->GetYaxis()->CenterTitle();
  h2111->GetYaxis()->SetLabelSize(0.060);
  int tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h2111->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h2111->GetBinContent(ib);
    h2111->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h2111->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h2111->GetBinContent(ib);
    h2111->SetBinContent(ib,val/tot);
  }
  h2111->SetMaximum(maxb);
  h2111->SetMinimum(min);
  h2111->Draw("hist");
  TH1D* h2111cog = f->Get("2111/OccupancyCOG_2111")->Clone();
  tot = 0;
  for (int ib=0; ib<640; ib++) tot+=h2111cog->GetBinContent(ib);
  for (int ib=0; ib<640; ib++) {
    float val = h2111cog->GetBinContent(ib);
    h2111cog->SetBinContent(ib,val/tot);
  }
  tot = 0;
  for (int ib=640; ib<1024; ib++) tot+=h2111cog->GetBinContent(ib);
  for (int ib=640; ib<1024; ib++) {
    float val = h2111cog->GetBinContent(ib);
    h2111cog->SetBinContent(ib,val/tot);
  }
  h2111cog->SetLineColor(2);
  h2111cog->Draw("samehist");  
  l640b->Draw();   
  l256b->Draw();  
  l896b->Draw();  
  tx0->Draw();
  ty0->Draw();

  float bc;
  float cont;
  int nc = 0;
  for (int ib=0; ib<nb; ib++) {
    bc = h2111->GetXaxis()->GetBinCenter(ib);
    cont = h2111->GetBinContent(ib);
    if (cont > 0.0) {
    printf("ib %d cen %f cont %f\n",ib,bc,cont);
    nc++;
    }
  }  
  printf("nc %d\n",nc);  

  TCanvas* c2=new TCanvas("c2","occupation",0,0,650,650);
  c2->Divide(2,2,0.0001,0.0001);
  c2_1->SetFillColor(10);
  c2_1->Range(-10,-10,10,10);
  c2_1->SetLeftMargin(0.12);
  c2_1->SetBottomMargin(0.18);
  c2_1->SetRightMargin(0.05);
  c2_1->SetTopMargin(0.05);
  c2_1->SetLogy(1);
  c2_2->SetFillColor(10);
  c2_2->Range(-10,-10,10,10);
  c2_2->SetLeftMargin(0.12);
  c2_2->SetBottomMargin(0.18);
  c2_2->SetRightMargin(0.05);
  c2_2->SetTopMargin(0.05);
  c2_2->SetLogy(1);
  c2_3->SetFillColor(10);
  c2_3->Range(-10,-10,10,10);
  c2_3->SetLeftMargin(0.12);
  c2_3->SetBottomMargin(0.18);
  c2_3->SetRightMargin(0.05);
  c2_3->SetTopMargin(0.05);
  c2_3->SetLogy(1);
  c2_4->SetFillColor(10);
  c2_4->Range(-10,-10,10,10);
  c2_4->SetLeftMargin(0.12);
  c2_4->SetBottomMargin(0.18);
  c2_4->SetRightMargin(0.05);
  c2_4->SetTopMargin(0.05);
  c2_4->SetLogy(1);

  // lecture 1, position faisceau 1ere AMS02
  c2->cd(1);
  f->cd("0901");
  Occupancy_0901->GetXaxis()->SetLabelSize(0.05);
  Occupancy_0901->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0901->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0901->GetXaxis()->SetRangeUser(641.,1024.);
  Occupancy_0901->Draw("hist");  
  // lecture 3, position faisceau 2eme AMS02
  c2->cd(2);
  f->cd("0905");
  Occupancy_0905->GetXaxis()->SetLabelSize(0.05);
  Occupancy_0905->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0905->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0905->GetXaxis()->SetRangeUser(641.,1024.);
  Occupancy_0905->Draw("hist");
  // lecture 0, position faisceau 3eme AMS02
  c2->cd(3);
  f->cd("0900");
  Occupancy_0900->GetXaxis()->SetLabelSize(0.05);
  Occupancy_0900->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0900->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0900->GetXaxis()->SetRangeUser(641.,1024.);
  Occupancy_0900->Draw("hist");
  c2->cd(4);
  // lecture 2, position faisceau 4eme AMS02
  f->cd("0904");
  Occupancy_0904->GetXaxis()->SetLabelSize(0.05);
  Occupancy_0904->GetXaxis()->SetLabelOffset(0.015);
  Occupancy_0904->GetYaxis()->SetLabelSize(0.060);
  Occupancy_0904->GetXaxis()->SetRangeUser(641.,1024.);
  Occupancy_0904->Draw("hist");
 
  TCanvas* c3=new TCanvas("c3","occ xy2",0,0,600,400);
  c3->SetFillColor(10);
  c3->Range(-10,-10,10,10);
  c3->SetLeftMargin(0.15);
  c3->SetBottomMargin(0.18);
  c3->SetRightMargin(0.05);
  c3->SetTopMargin(0.05);
  c3->SetLogy(1);
  c3->cd();
  h0905cog->GetXaxis()->SetLabelSize(0.065);
  h0905cog->GetXaxis()->SetLabelOffset(0.015);
  h0905cog->GetXaxis()->SetTitleSize(0.07);
  h0905cog->GetXaxis()->SetTitleOffset(1.0);
  h0905cog->GetXaxis()->SetTitle("canal");
  h0905cog->GetXaxis()->CenterTitle();
  h0905cog->GetYaxis()->SetLabelSize(0.060);
  h0905cog->GetYaxis()->SetTitleSize(0.07);
  h0905cog->GetYaxis()->SetTitleOffset(1.0);
  h0905cog->GetYaxis()->SetTitle("fraction");
  h0905cog->GetYaxis()->CenterTitle();
  h0905cog->SetMinimum(min);
  h0905cog->SetMaximum(max);
  h0905cog->Draw("hist");
  l640->Draw();  
  l832->Draw();  
  h0905->Draw("samehist");  
  tx2->Draw();
  ty2->Draw();

  TCanvas* cc=new TCanvas("cc","void",0,0,500,500);
  cc->SetFillColor(10);
  h2111->Draw("hist");

  return;
} 

void select() {
 
  char fich[80];
  //  sprintf(fich,"largeurs_%s.dat",run);
  sprintf(fich,"largeurs.dat");

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  TCanvas* c1=new TCanvas("c1","signal select",0,0,650,650);
  c1->Divide(2,3,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.12);
  c1_1->SetBottomMargin(0.18);
  c1_1->SetRightMargin(0.05);
  c1_1->SetTopMargin(0.05);
  c1_2->SetFillColor(10);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.12);
  c1_2->SetBottomMargin(0.18);
  c1_2->SetRightMargin(0.05);
  c1_2->SetTopMargin(0.05);
  c1_3->SetFillColor(10);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.12);
  c1_3->SetBottomMargin(0.18);
  c1_3->SetRightMargin(0.05);
  c1_3->SetTopMargin(0.05);
  c1_4->SetFillColor(10);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.12);
  c1_4->SetBottomMargin(0.18);
  c1_4->SetRightMargin(0.05);
  c1_4->SetTopMargin(0.05);
  c1_5->SetFillColor(10);
  c1_5->Range(-10,-10,10,10);
  c1_5->SetLeftMargin(0.12);
  c1_5->SetBottomMargin(0.18);
  c1_5->SetRightMargin(0.05);
  c1_5->SetTopMargin(0.05);
  c1_6->SetFillColor(10);
  c1_6->Range(-10,-10,10,10);
  c1_6->SetLeftMargin(0.12);
  c1_6->SetBottomMargin(0.18);
  c1_6->SetRightMargin(0.05);
  c1_6->SetTopMargin(0.05);

  // lecture 0, position faisceau 3eme AMS02
  c1->cd(3);
  f->cd("0900");
  Signal_0900_S->GetXaxis()->SetLabelSize(0.065);
  Signal_0900_S->GetXaxis()->SetLabelOffset(0.015);
  Signal_0900_S->GetYaxis()->SetLabelSize(0.060);
  Signal_0900_S->GetXaxis()->SetRangeUser(0.,120.);
  Signal_0900_S->SetLineColor(2);
  Signal_0900_S->Draw("hist");  
  Signal_0900_K->SetLineColor(4);
  Signal_0900_K->Draw("samehist");
  // lecture 1, position faisceau 1ere AMS02
  c1->cd(1);
  f->cd("0901");
  Signal_0901_S->GetXaxis()->SetLabelSize(0.065);
  Signal_0901_S->GetXaxis()->SetLabelOffset(0.015);
  Signal_0901_S->GetYaxis()->SetLabelSize(0.060);
  Signal_0901_S->GetXaxis()->SetRangeUser(0.,120.);
  Signal_0901_S->SetLineColor(2);
  Signal_0901_S->Draw("hist");
  Signal_0901_K->SetLineColor(4);
  Signal_0901_K->Draw("samehist");
  // lecture 2, position faisceau 4eme AMS02
  c1->cd(4);
  f->cd("0904");
  Signal_0904_S->GetXaxis()->SetLabelSize(0.065);
  Signal_0904_S->GetXaxis()->SetLabelOffset(0.015);
  Signal_0904_S->GetYaxis()->SetLabelSize(0.060);
  Signal_0904_S->GetXaxis()->SetRangeUser(0.,120.);
  Signal_0904_S->SetLineColor(2);
  Signal_0904_S->Draw("hist");
  Signal_0904_K->SetLineColor(4);
  Signal_0904_K->Draw("samehist");
  // lecture 3, position faisceau 2eme AMS02
  c1->cd(2);
  f->cd("0905");
  Signal_0905_S->GetXaxis()->SetLabelSize(0.065);
  Signal_0905_S->GetXaxis()->SetLabelOffset(0.015);
  Signal_0905_S->GetYaxis()->SetLabelSize(0.060);
  Signal_0905_S->GetXaxis()->SetRangeUser(0.,120.);
  Signal_0905_S->SetLineColor(2);
  Signal_0905_S->Draw("hist");
  Signal_0905_K->SetLineColor(4);
  Signal_0905_K->Draw("samehist");
  c1->cd(5);
  f->cd("2110");
  Signal_2110_S->GetXaxis()->SetLabelSize(0.065);
  Signal_2110_S->GetXaxis()->SetLabelOffset(0.015);
  Signal_2110_S->GetYaxis()->SetLabelSize(0.060);
  Signal_2110_S->GetXaxis()->SetRangeUser(0.,700.);
  Signal_2110_S->SetLineColor(2);
  Signal_2110_S->Draw("hist");
  Signal_2110_K->SetLineColor(4);
  Signal_2110_K->Draw("samehist");
  c1->cd(6);
  f->cd("2111");
  Signal_2111_S->GetXaxis()->SetLabelSize(0.065);
  Signal_2111_S->GetXaxis()->SetLabelOffset(0.015);
  Signal_2111_S->GetYaxis()->SetLabelSize(0.060);
  Signal_2111_S->GetXaxis()->SetRangeUser(0.,700.);
  Signal_2111_S->SetLineColor(2);
  Signal_2111_S->Draw("hist");
  Signal_2111_K->SetLineColor(4);
  Signal_2111_K->Draw("samehist");

  TCanvas* c2=new TCanvas("c2","signal/bruit select",0,0,650,650);
  c2->Divide(2,3,0.0001,0.0001);
  c2_1->SetFillColor(10);
  c2_1->Range(-10,-10,10,10);
  c2_1->SetLeftMargin(0.12);
  c2_1->SetBottomMargin(0.18);
  c2_1->SetRightMargin(0.05);
  c2_1->SetTopMargin(0.05);
  c2_2->SetFillColor(10);
  c2_2->Range(-10,-10,10,10);
  c2_2->SetLeftMargin(0.12);
  c2_2->SetBottomMargin(0.18);
  c2_2->SetRightMargin(0.05);
  c2_2->SetTopMargin(0.05);
  c2_3->SetFillColor(10);
  c2_3->Range(-10,-10,10,10);
  c2_3->SetLeftMargin(0.12);
  c2_3->SetBottomMargin(0.18);
  c2_3->SetRightMargin(0.05);
  c2_3->SetTopMargin(0.05);
  c2_4->SetFillColor(10);
  c2_4->Range(-10,-10,10,10);
  c2_4->SetLeftMargin(0.12);
  c2_4->SetBottomMargin(0.18);
  c2_4->SetRightMargin(0.05);
  c2_4->SetTopMargin(0.05);
  c2_5->SetFillColor(10);
  c2_5->Range(-10,-10,10,10);
  c2_5->SetLeftMargin(0.12);
  c2_5->SetBottomMargin(0.18);
  c2_5->SetRightMargin(0.05);
  c2_5->SetTopMargin(0.05);
  c2_6->SetFillColor(10);
  c2_6->Range(-10,-10,10,10);
  c2_6->SetLeftMargin(0.12);
  c2_6->SetBottomMargin(0.18);
  c2_6->SetRightMargin(0.05);
  c2_6->SetTopMargin(0.05);

  // lecture 0, position faisceau 3eme AMS02
  c2->cd(3);
  f->cd("0900");
  SN_0900_S->GetXaxis()->SetLabelSize(0.065);
  SN_0900_S->GetXaxis()->SetLabelOffset(0.015);
  SN_0900_S->GetYaxis()->SetLabelSize(0.060);
  SN_0900_S->GetXaxis()->SetRangeUser(0.,30.);
  SN_0900_S->SetLineColor(2);
  SN_0900_S->Draw("hist");  
  SN_0900_K->SetLineColor(4);
  SN_0900_K->Draw("samehist");
  // lecture 1, position faisceau 1ere AMS02
  c2->cd(1);
  f->cd("0901");
  SN_0901_S->GetXaxis()->SetLabelSize(0.065);
  SN_0901_S->GetXaxis()->SetLabelOffset(0.015);
  SN_0901_S->GetYaxis()->SetLabelSize(0.060);
  SN_0901_S->GetXaxis()->SetRangeUser(0.,30.);
  SN_0901_S->SetLineColor(2);
  SN_0901_S->Draw("hist");
  SN_0901_K->SetLineColor(4);
  SN_0901_K->Draw("samehist");
  // lecture 2, position faisceau 4eme AMS02
  c2->cd(4);
  f->cd("0904");
  SN_0904_S->GetXaxis()->SetLabelSize(0.065);
  SN_0904_S->GetXaxis()->SetLabelOffset(0.015);
  SN_0904_S->GetYaxis()->SetLabelSize(0.060);
  SN_0904_S->GetXaxis()->SetRangeUser(0.,30.);
  SN_0904_S->SetLineColor(2);
  SN_0904_S->Draw("hist");
  SN_0904_K->SetLineColor(4);
  SN_0904_K->Draw("samehist");
  // lecture 3, position faisceau 2eme AMS02
  c2->cd(2);
  f->cd("0905");
  SN_0905_S->GetXaxis()->SetLabelSize(0.065);
  SN_0905_S->GetXaxis()->SetLabelOffset(0.015);
  SN_0905_S->GetYaxis()->SetLabelSize(0.060);
  SN_0905_S->GetXaxis()->SetRangeUser(0.,30.);
  SN_0905_S->SetLineColor(2);
  SN_0905_S->Draw("hist");
  SN_0905_K->SetLineColor(4);
  SN_0905_K->Draw("samehist");
  c2->cd(5);
  f->cd("2110");
  SN_2110_K->GetXaxis()->SetLabelSize(0.065);
  SN_2110_K->GetXaxis()->SetLabelOffset(0.015);
  SN_2110_K->GetYaxis()->SetLabelSize(0.060);
  SN_2110_K->GetXaxis()->SetRangeUser(0.,100.);
  SN_2110_K->SetLineColor(4);
  SN_2110_K->Draw("hist");
  SN_2110_S->SetLineColor(2);
  SN_2110_S->Draw("samehist");
  c2->cd(6);
  f->cd("2111");
  SN_2111_K->GetXaxis()->SetLabelSize(0.065);
  SN_2111_K->GetXaxis()->SetLabelOffset(0.015);
  SN_2111_K->GetYaxis()->SetLabelSize(0.060);
  SN_2111_K->GetXaxis()->SetRangeUser(0.,100.);
  SN_2111_K->SetLineColor(4);
  SN_2111_K->Draw("hist");
  SN_2111_S->SetLineColor(2);
  SN_2111_S->Draw("samehist");

  TCanvas* c3=new TCanvas("c3","signal/bruit(s) select",0,0,650,650);
  c3->Divide(2,3,0.0001,0.0001);
  c3_1->SetFillColor(10);
  c3_1->Range(-10,-10,10,10);
  c3_1->SetLeftMargin(0.12);
  c3_1->SetBottomMargin(0.18);
  c3_1->SetRightMargin(0.05);
  c3_1->SetTopMargin(0.05);
  c3_2->SetFillColor(10);
  c3_2->Range(-10,-10,10,10);
  c3_2->SetLeftMargin(0.12);
  c3_2->SetBottomMargin(0.18);
  c3_2->SetRightMargin(0.05);
  c3_2->SetTopMargin(0.05);
  c3_3->SetFillColor(10);
  c3_3->Range(-10,-10,10,10);
  c3_3->SetLeftMargin(0.12);
  c3_3->SetBottomMargin(0.18);
  c3_3->SetRightMargin(0.05);
  c3_3->SetTopMargin(0.05);
  c3_4->SetFillColor(10);
  c3_4->Range(-10,-10,10,10);
  c3_4->SetLeftMargin(0.12);
  c3_4->SetBottomMargin(0.18);
  c3_4->SetRightMargin(0.05);
  c3_4->SetTopMargin(0.05);
  c3_5->SetFillColor(10);
  c3_5->Range(-10,-10,10,10);
  c3_5->SetLeftMargin(0.12);
  c3_5->SetBottomMargin(0.18);
  c3_5->SetRightMargin(0.05);
  c3_5->SetTopMargin(0.05);
  c3_6->SetFillColor(10);
  c3_6->Range(-10,-10,10,10);
  c3_6->SetLeftMargin(0.12);
  c3_6->SetBottomMargin(0.18);
  c3_6->SetRightMargin(0.05);
  c3_6->SetTopMargin(0.05);

  // lecture 0, position faisceau 3eme AMS02
  c3->cd(3);
  f->cd("0900");
  SeedSN_0900_K->GetXaxis()->SetLabelSize(0.065);
  SeedSN_0900_K->GetXaxis()->SetLabelOffset(0.015);
  SeedSN_0900_K->GetYaxis()->SetLabelSize(0.060);
  SeedSN_0900_K->GetXaxis()->SetRangeUser(0.,25.);
  SeedSN_0900_K->SetLineColor(4);
  SeedSN_0900_K->Draw("hist");  
  SeedSN_0900_S->SetLineColor(2);
  SeedSN_0900_S->Draw("samehist");
  // lecture 1, position faisceau 1ere AMS02
  c3->cd(1);
  f->cd("0901");
  SeedSN_0901_K->GetXaxis()->SetLabelSize(0.065);
  SeedSN_0901_K->GetXaxis()->SetLabelOffset(0.015);
  SeedSN_0901_K->GetYaxis()->SetLabelSize(0.060);
  SeedSN_0901_K->GetXaxis()->SetRangeUser(0.,30.);
  SeedSN_0901_K->SetLineColor(4);
  SeedSN_0901_K->Draw("hist");
  SeedSN_0901_S->SetLineColor(2);
  SeedSN_0901_S->Draw("samehist");
  // lecture 2, position faisceau 4eme AMS02
  c3->cd(4);
  f->cd("0904");
  SeedSN_0904_K->GetXaxis()->SetLabelSize(0.065);
  SeedSN_0904_K->GetXaxis()->SetLabelOffset(0.015);
  SeedSN_0904_K->GetYaxis()->SetLabelSize(0.060);
  SeedSN_0904_K->GetXaxis()->SetRangeUser(0.,30.);
  SeedSN_0904_K->SetLineColor(4);
  SeedSN_0904_K->Draw("hist");
  SeedSN_0904_S->SetLineColor(2);
  SeedSN_0904_S->Draw("samehist");
  // lecture 3, position faisceau 2eme AMS02
  c3->cd(2);
  f->cd("0905");
  SeedSN_0905_K->GetXaxis()->SetLabelSize(0.065);
  SeedSN_0905_K->GetXaxis()->SetLabelOffset(0.015);
  SeedSN_0905_K->GetYaxis()->SetLabelSize(0.060);
  SeedSN_0905_K->GetXaxis()->SetRangeUser(0.,30.);
  SeedSN_0905_K->SetLineColor(4);
  SeedSN_0905_K->Draw("hist");
  SeedSN_0905_S->SetLineColor(2);
  SeedSN_0905_S->Draw("samehist");
  c3->cd(5);
  f->cd("2110");
  SeedSN_2110_K->GetXaxis()->SetLabelSize(0.065);
  SeedSN_2110_K->GetXaxis()->SetLabelOffset(0.015);
  SeedSN_2110_K->GetYaxis()->SetLabelSize(0.060);
  SeedSN_2110_K->GetXaxis()->SetRangeUser(0.,100.);
  SeedSN_2110_K->SetLineColor(4);
  SeedSN_2110_K->Draw("hist");
  SeedSN_2110_S->SetLineColor(2);
  SeedSN_2110_S->Draw("samehist");
  c3->cd(6);
  f->cd("2111");
  SeedSN_2111_K->GetXaxis()->SetLabelSize(0.065);
  SeedSN_2111_K->GetXaxis()->SetLabelOffset(0.015);
  SeedSN_2111_K->GetYaxis()->SetLabelSize(0.060);
  SeedSN_2111_K->GetXaxis()->SetRangeUser(0.,100.);
  SeedSN_2111_K->SetLineColor(4);
  SeedSN_2111_K->Draw("hist");
  SeedSN_2111_S->SetLineColor(2);
  SeedSN_2111_S->Draw("samehist");

  TCanvas* c4=new TCanvas("c4","largeur TDR select",0,0,650,650);
  c4->Divide(2,3,0.0001,0.0001);
  c4_1->SetFillColor(10);
  c4_1->Range(-10,-10,10,10);
  c4_1->SetLeftMargin(0.12);
  c4_1->SetBottomMargin(0.18);
  c4_1->SetRightMargin(0.05);
  c4_1->SetTopMargin(0.05);
  c4_1->SetLogy(1);
  c4_2->SetFillColor(10);
  c4_2->Range(-10,-10,10,10);
  c4_2->SetLeftMargin(0.12);
  c4_2->SetBottomMargin(0.18);
  c4_2->SetRightMargin(0.05);
  c4_2->SetTopMargin(0.05);
  c4_2->SetLogy(1);
  c4_3->SetFillColor(10);
  c4_3->Range(-10,-10,10,10);
  c4_3->SetLeftMargin(0.12);
  c4_3->SetBottomMargin(0.18);
  c4_3->SetRightMargin(0.05);
  c4_3->SetTopMargin(0.05);
  c4_3->SetLogy(1);
  c4_4->SetFillColor(10);
  c4_4->Range(-10,-10,10,10);
  c4_4->SetLeftMargin(0.12);
  c4_4->SetBottomMargin(0.18);
  c4_4->SetRightMargin(0.05);
  c4_4->SetTopMargin(0.05);
  c4_4->SetLogy(1);
  c4_5->SetFillColor(10);
  c4_5->Range(-10,-10,10,10);
  c4_5->SetLeftMargin(0.12);
  c4_5->SetBottomMargin(0.18);
  c4_5->SetRightMargin(0.05);
  c4_5->SetTopMargin(0.05);
  c4_5->SetLogy(1);
  c4_6->SetFillColor(10);
  c4_6->Range(-10,-10,10,10);
  c4_6->SetLeftMargin(0.12);
  c4_6->SetBottomMargin(0.18);
  c4_6->SetRightMargin(0.05);
  c4_6->SetTopMargin(0.05);
  c4_6->SetLogy(1);

  float lmoyen[2][6];

  // lecture 0, position faisceau 4eme
  c4->cd(4);
  f->cd("0900");
  Width2_0900_S->GetXaxis()->SetLabelSize(0.065);
  Width2_0900_S->GetXaxis()->SetLabelOffset(0.015);
  Width2_0900_S->GetYaxis()->SetLabelSize(0.060);
  Width2_0900_S->GetXaxis()->SetRangeUser(0.,15.);
  Width2_0900_S->SetLineColor(2);
  Width2_0900_S->SetLineStyle(2);
  Width2_0900_S->SetMinimum(0.5);
  Width2_0900_S->Draw("hist");  
  Width2_0900_K->SetLineColor(4);
  Width2_0900_K->SetLineStyle(2);
  Width2_0900_K->Draw("samehist");
  lmoyen[0][3] = Width2_0900_S->GetMean();
  lmoyen[1][3] = Width2_0900_K->GetMean();
  // lecture 1, position faisceau 2eme 
  c4->cd(2);
  f->cd("0901");
  Width2_0901_S->GetXaxis()->SetLabelSize(0.065);
  Width2_0901_S->GetXaxis()->SetLabelOffset(0.015);
  Width2_0901_S->GetYaxis()->SetLabelSize(0.060);
  Width2_0901_S->GetXaxis()->SetRangeUser(0.,15.);
  Width2_0901_S->SetLineColor(2);
  Width2_0901_S->SetLineStyle(2);
  Width2_0901_S->SetMinimum(0.5);
  Width2_0901_S->Draw("hist");
  Width2_0901_K->SetLineColor(4);
  Width2_0901_K->SetLineStyle(2);
  Width2_0901_K->Draw("samehist");
  lmoyen[0][1] = Width2_0901_S->GetMean();
  lmoyen[1][1] = Width2_0901_K->GetMean();
  // lecture 2, position faisceau 6eme
  c4->cd(6);
  f->cd("0904");
  Width2_0904_S->GetXaxis()->SetLabelSize(0.065);
  Width2_0904_S->GetXaxis()->SetLabelOffset(0.015);
  Width2_0904_S->GetYaxis()->SetLabelSize(0.060);
  Width2_0904_S->GetXaxis()->SetRangeUser(0.,15.);
  Width2_0904_S->SetLineColor(2);
  Width2_0904_S->SetLineStyle(2);
  Width2_0904_S->SetMinimum(0.5);
  Width2_0904_S->Draw("hist");
  Width2_0904_K->SetLineColor(4);
  Width2_0904_K->SetLineStyle(2);
  Width2_0904_K->Draw("samehist");
  lmoyen[0][5] = Width2_0904_S->GetMean();
  lmoyen[1][5] = Width2_0904_K->GetMean();
  // lecture 3, position faisceau 3eme 
  c4->cd(3);
  f->cd("0905");
  Width2_0905_S->GetXaxis()->SetLabelSize(0.065);
  Width2_0905_S->GetXaxis()->SetLabelOffset(0.015);
  Width2_0905_S->GetYaxis()->SetLabelSize(0.060);
  Width2_0905_S->GetXaxis()->SetRangeUser(0.,15.);
  Width2_0905_S->SetLineColor(2);
  Width2_0905_S->SetLineStyle(2);
  Width2_0905_S->SetMinimum(0.5);
  Width2_0905_S->Draw("hist");
  Width2_0905_K->SetLineColor(4);
  Width2_0905_K->SetLineStyle(2);
  Width2_0905_K->Draw("samehist");
  lmoyen[0][2] = Width2_0905_S->GetMean();
  lmoyen[1][2] = Width2_0905_K->GetMean();
  // lecture 4, position faisceau 5eme 
  c4->cd(5);
  f->cd("2110");
  Width2_2110_S->GetXaxis()->SetLabelSize(0.065);
  Width2_2110_S->GetXaxis()->SetLabelOffset(0.015);
  Width2_2110_S->GetYaxis()->SetLabelSize(0.060);
  Width2_2110_S->GetXaxis()->SetRangeUser(0.,15.);
  Width2_2110_S->SetLineColor(2);
  Width2_2110_S->SetLineStyle(2);
  Width2_2110_S->SetMinimum(0.5);
  Width2_2110_S->Draw("hist");
  Width2_2110_K->SetLineColor(4);
  Width2_2110_K->SetLineStyle(2);
  Width2_2110_K->Draw("samehist");
  lmoyen[0][4] = Width2_2110_S->GetMean();
  lmoyen[1][4] = Width2_2110_K->GetMean();
  // lecture 5, position faisceau 1ere 
  c4->cd(1);
  f->cd("2111");
  Width2_2111_S->GetXaxis()->SetLabelSize(0.065);
  Width2_2111_S->GetXaxis()->SetLabelOffset(0.015);
  Width2_2111_S->GetYaxis()->SetLabelSize(0.060);
  Width2_2111_S->GetXaxis()->SetRangeUser(0.,15.);
  Width2_2111_S->SetLineColor(2);
  Width2_2111_S->SetLineStyle(2);
  Width2_2111_S->SetMinimum(0.5);
  Width2_2111_S->Draw("hist");
  Width2_2111_K->SetLineColor(4);
  Width2_2111_K->SetLineStyle(2);
  Width2_2111_K->Draw("samehist");
  lmoyen[0][0] = Width2_2111_S->GetMean();
  lmoyen[1][0] = Width2_2111_K->GetMean();

  printf("largeurs TDR   pos       p        n\n");
  for (int i=0; i<6; i++)
    printf("                %d   %f   %f\n",i,lmoyen[0][i],lmoyen[1][i]);

  printf("fich %s\n",fich);
  FILE* f2 = fopen(fich,"w");
  fprintf(f2,"largeurs TDR\n");
  for (int i=0; i<6; i++)
   fprintf(f2,"%d %f %f\n",i,lmoyen[0][i],lmoyen[1][i]);

  // lecture 0, position faisceau 4eme 
  c4->cd(4);
  f->cd("0900");
  Width_0900_K->GetXaxis()->SetLabelSize(0.065);
  Width_0900_K->GetXaxis()->SetLabelOffset(0.015);
  Width_0900_K->GetYaxis()->SetLabelSize(0.060);
  Width_0900_K->GetXaxis()->SetRangeUser(0.,15.);
  Width_0900_K->SetLineColor(4);
  Width_0900_K->Draw("samehist");  
  Width_0900_S->SetLineColor(2);
  Width_0900_S->Draw("samehist");
  lmoyen[0][3] = Width_0900_S->GetMean();
  lmoyen[1][3] = Width_0900_K->GetMean();
  // lecture 1, position faisceau 2eme 
  c4->cd(2);
  f->cd("0901");
  Width_0901_K->GetXaxis()->SetLabelSize(0.065);
  Width_0901_K->GetXaxis()->SetLabelOffset(0.015);
  Width_0901_K->GetYaxis()->SetLabelSize(0.060);
  Width_0901_K->GetXaxis()->SetRangeUser(0.,15.);
  Width_0901_K->SetLineColor(4);
  Width_0901_K->Draw("samehist");
  Width_0901_S->SetLineColor(2);
  Width_0901_S->Draw("samehist");
  lmoyen[0][1] = Width_0901_S->GetMean();
  lmoyen[1][1] = Width_0901_K->GetMean();
  // lecture 2, position faisceau 6eme
  c4->cd(6);
  f->cd("0904");
  Width_0904_K->GetXaxis()->SetLabelSize(0.065);
  Width_0904_K->GetXaxis()->SetLabelOffset(0.015);
  Width_0904_K->GetYaxis()->SetLabelSize(0.060);
  Width_0904_K->GetXaxis()->SetRangeUser(0.,15.);
  Width_0904_K->SetLineColor(4);
  Width_0904_K->Draw("samehist");
  Width_0904_S->SetLineColor(2);
  Width_0904_S->Draw("samehist");
  lmoyen[0][5] = Width_0904_S->GetMean();
  lmoyen[1][5] = Width_0904_K->GetMean();
  // lecture 3, position faisceau 3eme
  c4->cd(3);
  f->cd("0905");
  Width_0905_K->GetXaxis()->SetLabelSize(0.065);
  Width_0905_K->GetXaxis()->SetLabelOffset(0.015);
  Width_0905_K->GetYaxis()->SetLabelSize(0.060);
  Width_0905_K->GetXaxis()->SetRangeUser(0.,15.);
  Width_0905_K->SetLineColor(4);
  Width_0905_K->Draw("samehist");
  Width_0905_S->SetLineColor(2);
  Width_0905_S->Draw("samehist");
  lmoyen[0][2] = Width_0905_S->GetMean();
  lmoyen[1][2] = Width_0905_K->GetMean();
  // lecture 4, position faisceau 5eme
  c4->cd(5);
  f->cd("2110");
  Width_2110_K->GetXaxis()->SetLabelSize(0.065);
  Width_2110_K->GetXaxis()->SetLabelOffset(0.015);
  Width_2110_K->GetYaxis()->SetLabelSize(0.060);
  Width_2110_K->GetXaxis()->SetRangeUser(0.,15.);
  Width_2110_K->SetLineColor(4);
  Width_2110_K->Draw("samehist");
  Width_2110_S->SetLineColor(2);
  Width_2110_S->Draw("samehist");
  lmoyen[0][4] = Width_2110_S->GetMean();
  lmoyen[1][4] = Width_2110_K->GetMean();
  // lecture 5, position faisceau 1ere
  c4->cd(1);
  f->cd("2111");
  Width_2111_K->GetXaxis()->SetLabelSize(0.065);
  Width_2111_K->GetXaxis()->SetLabelOffset(0.015);
  Width_2111_K->GetYaxis()->SetLabelSize(0.060);
  Width_2111_K->GetXaxis()->SetRangeUser(0.,15.);
  Width_2111_K->SetLineColor(4);
  Width_2111_K->Draw("samehist");
  Width_2111_S->SetLineColor(2);
  Width_2111_S->Draw("samehist");
  lmoyen[0][0] = Width_2111_S->GetMean();
  lmoyen[1][0] = Width_2111_K->GetMean();

  printf("largeurs sel.    pos       p        n\n");
  for (int i=0; i<6; i++)
    printf("                %d   %f   %f\n",i,lmoyen[0][i],lmoyen[1][i]);
  fprintf(f2,"largeurs sel.\n");
  for (int i=0; i<6; i++)
    fprintf(f2,"%d %f %f\n",i,lmoyen[0][i],lmoyen[1][i]);

  // lecture 0, position faisceau 4eme 
  c4->cd(4);
  f->cd("0900");
  Larg_0900_K->GetXaxis()->SetLabelSize(0.065);
  Larg_0900_K->GetXaxis()->SetLabelOffset(0.015);
  Larg_0900_K->GetYaxis()->SetLabelSize(0.060);
  Larg_0900_K->GetXaxis()->SetRangeUser(0.,15.);
  Larg_0900_K->SetLineColor(4);
  Larg_0900_K->Draw("samehist");  
  Larg_0900_S->SetLineColor(2);
  Larg_0900_S->Draw("samehist");
  lmoyen[0][3] = Larg_0900_S->GetMean();
  lmoyen[1][3] = Larg_0900_K->GetMean();
  // lecture 1, position faisceau 2eme 
  c4->cd(2);
  f->cd("0901");
  Larg_0901_K->GetXaxis()->SetLabelSize(0.065);
  Larg_0901_K->GetXaxis()->SetLabelOffset(0.015);
  Larg_0901_K->GetYaxis()->SetLabelSize(0.060);
  Larg_0901_K->GetXaxis()->SetRangeUser(0.,15.);
  Larg_0901_K->SetLineColor(4);
  Larg_0901_K->Draw("samehist");
  Larg_0901_S->SetLineColor(2);
  Larg_0901_S->Draw("samehist");
  lmoyen[0][1] = Larg_0901_S->GetMean();
  lmoyen[1][1] = Larg_0901_K->GetMean();
  // lecture 2, position faisceau 6eme
  c4->cd(6);
  f->cd("0904");
  Larg_0904_K->GetXaxis()->SetLabelSize(0.065);
  Larg_0904_K->GetXaxis()->SetLabelOffset(0.015);
  Larg_0904_K->GetYaxis()->SetLabelSize(0.060);
  Larg_0904_K->GetXaxis()->SetRangeUser(0.,15.);
  Larg_0904_K->SetLineColor(4);
  Larg_0904_K->Draw("samehist");
  Larg_0904_S->SetLineColor(2);
  Larg_0904_S->Draw("samehist");
  lmoyen[0][5] = Larg_0904_S->GetMean();
  lmoyen[1][5] = Larg_0904_K->GetMean();
  // lecture 3, position faisceau 3eme
  c4->cd(3);
  f->cd("0905");
  Larg_0905_K->GetXaxis()->SetLabelSize(0.065);
  Larg_0905_K->GetXaxis()->SetLabelOffset(0.015);
  Larg_0905_K->GetYaxis()->SetLabelSize(0.060);
  Larg_0905_K->GetXaxis()->SetRangeUser(0.,15.);
  Larg_0905_K->SetLineColor(4);
  Larg_0905_K->Draw("samehist");
  Larg_0905_S->SetLineColor(2);
  Larg_0905_S->Draw("samehist");
  lmoyen[0][2] = Larg_0905_S->GetMean();
  lmoyen[1][2] = Larg_0905_K->GetMean();
  // lecture 4, position faisceau 5eme
  c4->cd(5);
  f->cd("2110");
  Larg_2110_K->GetXaxis()->SetLabelSize(0.065);
  Larg_2110_K->GetXaxis()->SetLabelOffset(0.015);
  Larg_2110_K->GetYaxis()->SetLabelSize(0.060);
  Larg_2110_K->GetXaxis()->SetRangeUser(0.,15.);
  Larg_2110_K->SetLineColor(4);
  Larg_2110_K->Draw("samehist");
  Larg_2110_S->SetLineColor(2);
  Larg_2110_S->Draw("samehist");
  lmoyen[0][4] = Larg_2110_S->GetMean();
  lmoyen[1][4] = Larg_2110_K->GetMean();
  // lecture 5, position faisceau 1ere
  c4->cd(1);
  f->cd("2111");
  Larg_2111_K->GetXaxis()->SetLabelSize(0.065);
  Larg_2111_K->GetXaxis()->SetLabelOffset(0.015);
  Larg_2111_K->GetYaxis()->SetLabelSize(0.060);
  Larg_2111_K->GetXaxis()->SetRangeUser(0.,15.);
  Larg_2111_K->SetLineColor(4);
  Larg_2111_K->Draw("samehist");
  Larg_2111_S->SetLineColor(2);
  Larg_2111_S->Draw("samehist");
  lmoyen[0][0] = Larg_2111_S->GetMean();
  lmoyen[1][0] = Larg_2111_K->GetMean();

  printf("largeurs sn>=1   pos       p        n\n");
  for (int i=0; i<6; i++)
    printf("                %d   %f   %f\n",i,lmoyen[0][i],lmoyen[1][i]);
  fprintf(f2,"largeurs sn>=1.\n");
  for (int i=0; i<6; i++)
    fprintf(f2,"%d %f %f\n",i,lmoyen[0][i],lmoyen[1][i]);

  // lecture 0, position faisceau 4eme 
  c4->cd(4);
  f->cd("0900");
  NPiste_0900_K->GetXaxis()->SetLabelSize(0.065);
  NPiste_0900_K->GetXaxis()->SetLabelOffset(0.015);
  NPiste_0900_K->GetYaxis()->SetLabelSize(0.060);
  NPiste_0900_K->GetXaxis()->SetRangeUser(0.,15.);
  NPiste_0900_K->SetLineColor(4);
  NPiste_0900_K->SetFillColor(4);
  NPiste_0900_K->SetFillStyle(3004);
  NPiste_0900_K->Draw("samehist");  
  NPiste_0900_S->SetLineColor(2);
  NPiste_0900_S->SetFillColor(2);
  NPiste_0900_S->SetFillStyle(3005);
  NPiste_0900_S->Draw("samehist");
  lmoyen[0][3] = NPiste_0900_S->GetMean();
  lmoyen[1][3] = NPiste_0900_K->GetMean();
  // lecture 1, position faisceau 2eme 
  c4->cd(2);
  f->cd("0901");
  NPiste_0901_K->GetXaxis()->SetLabelSize(0.065);
  NPiste_0901_K->GetXaxis()->SetLabelOffset(0.015);
  NPiste_0901_K->GetYaxis()->SetLabelSize(0.060);
  NPiste_0901_K->GetXaxis()->SetRangeUser(0.,15.);
  NPiste_0901_K->SetLineColor(4);
  NPiste_0901_K->SetFillColor(4);
  NPiste_0901_K->SetFillStyle(3004);
  NPiste_0901_K->Draw("samehist");
  NPiste_0901_S->SetLineColor(2);
  NPiste_0901_S->SetFillColor(2);
  NPiste_0901_S->SetFillStyle(3005);
  NPiste_0901_S->Draw("samehist");
  lmoyen[0][1] = NPiste_0901_S->GetMean();
  lmoyen[1][1] = NPiste_0901_K->GetMean();
  // lecture 2, position faisceau 6eme
  c4->cd(6);
  f->cd("0904");
  NPiste_0904_K->GetXaxis()->SetLabelSize(0.065);
  NPiste_0904_K->GetXaxis()->SetLabelOffset(0.015);
  NPiste_0904_K->GetYaxis()->SetLabelSize(0.060);
  NPiste_0904_K->GetXaxis()->SetRangeUser(0.,15.);
  NPiste_0904_K->SetLineColor(4);
  NPiste_0904_K->SetFillColor(4);
  NPiste_0904_K->SetFillStyle(3004);
  NPiste_0904_K->Draw("samehist");
  NPiste_0904_S->SetLineColor(2);
  NPiste_0904_S->SetFillColor(2);
  NPiste_0904_S->SetFillStyle(3005);
  NPiste_0904_S->Draw("samehist");
  lmoyen[0][5] = NPiste_0904_S->GetMean();
  lmoyen[1][5] = NPiste_0904_K->GetMean();
  // lecture 3, position faisceau 3eme
  c4->cd(3);
  f->cd("0905");
  NPiste_0905_K->GetXaxis()->SetLabelSize(0.065);
  NPiste_0905_K->GetXaxis()->SetLabelOffset(0.015);
  NPiste_0905_K->GetYaxis()->SetLabelSize(0.060);
  NPiste_0905_K->GetXaxis()->SetRangeUser(0.,15.);
  NPiste_0905_K->SetLineColor(4);
  NPiste_0905_K->SetFillColor(4);
  NPiste_0905_K->SetFillStyle(3004);
  NPiste_0905_K->Draw("samehist");
  NPiste_0905_S->SetLineColor(2);
  NPiste_0905_S->SetFillColor(2);
  NPiste_0905_S->SetFillStyle(3005);
  NPiste_0905_S->Draw("samehist");
  lmoyen[0][2] = NPiste_0905_S->GetMean();
  lmoyen[1][2] = NPiste_0905_K->GetMean();
  // lecture 4, position faisceau 5eme
  c4->cd(5);
  f->cd("2110");
  NPiste_2110_K->GetXaxis()->SetLabelSize(0.065);
  NPiste_2110_K->GetXaxis()->SetLabelOffset(0.015);
  NPiste_2110_K->GetYaxis()->SetLabelSize(0.060);
  NPiste_2110_K->GetXaxis()->SetRangeUser(0.,15.);
  NPiste_2110_K->SetLineColor(4);
  NPiste_2110_K->SetFillColor(4);
  NPiste_2110_K->SetFillStyle(3004);
  NPiste_2110_K->Draw("samehist");
  NPiste_2110_S->SetLineColor(2);
  NPiste_2110_S->SetFillColor(2);
  NPiste_2110_S->SetFillStyle(3005);
  NPiste_2110_S->Draw("samehist");
  lmoyen[0][4] = NPiste_2110_S->GetMean();
  lmoyen[1][4] = NPiste_2110_K->GetMean();
  // lecture 5, position faisceau 1ere
  c4->cd(1);
  f->cd("2111");
  NPiste_2111_K->GetXaxis()->SetLabelSize(0.065);
  NPiste_2111_K->GetXaxis()->SetLabelOffset(0.015);
  NPiste_2111_K->GetYaxis()->SetLabelSize(0.060);
  NPiste_2111_K->GetXaxis()->SetRangeUser(0.,15.);
  NPiste_2111_K->SetLineColor(4);
  NPiste_2111_K->SetFillColor(4);
  NPiste_2111_K->SetFillStyle(3004);
  NPiste_2111_K->Draw("samehist");
  NPiste_2111_S->SetLineColor(2);
  NPiste_2111_S->SetFillColor(2);
  NPiste_2111_S->SetFillStyle(3005);
  NPiste_2111_S->Draw("samehist");
  lmoyen[0][0] = NPiste_2111_S->GetMean();
  lmoyen[1][0] = NPiste_2111_K->GetMean();

  printf("npiste      .    pos       p        n\n");
  for (int i=0; i<6; i++)
    printf("                %d   %f   %f\n",i,lmoyen[0][i],lmoyen[1][i]);
  fprintf(f2,"npiste\n");
  for (int i=0; i<6; i++)
    fprintf(f2,"%d %f %f\n",i,lmoyen[0][i],lmoyen[1][i]);

  int status = fclose(f2);
  printf("status %d\n",status);

  TCanvas* c5=new TCanvas("c5","eta select",0,0,650,650);
  c5->Divide(2,3,0.0001,0.0001);
  c5_1->SetFillColor(10);
  c5_1->Range(-10,-10,10,10);
  c5_1->SetLeftMargin(0.12);
  c5_1->SetBottomMargin(0.18);
  c5_1->SetRightMargin(0.05);
  c5_1->SetTopMargin(0.05);
  c5_2->SetFillColor(10);
  c5_2->Range(-10,-10,10,10);
  c5_2->SetLeftMargin(0.12);
  c5_2->SetBottomMargin(0.18);
  c5_2->SetRightMargin(0.05);
  c5_2->SetTopMargin(0.05);
  c5_3->SetFillColor(10);
  c5_3->Range(-10,-10,10,10);
  c5_3->SetLeftMargin(0.12);
  c5_3->SetBottomMargin(0.18);
  c5_3->SetRightMargin(0.05);
  c5_3->SetTopMargin(0.05);
  c5_4->SetFillColor(10);
  c5_4->Range(-10,-10,10,10);
  c5_4->SetLeftMargin(0.12);
  c5_4->SetBottomMargin(0.18);
  c5_4->SetRightMargin(0.05);
  c5_4->SetTopMargin(0.05);
  c5_5->SetFillColor(10);
  c5_5->Range(-10,-10,10,10);
  c5_5->SetLeftMargin(0.12);
  c5_5->SetBottomMargin(0.18);
  c5_5->SetRightMargin(0.05);
  c5_5->SetTopMargin(0.05);
  c5_6->SetFillColor(10);
  c5_6->Range(-10,-10,10,10);
  c5_6->SetLeftMargin(0.12);
  c5_6->SetBottomMargin(0.18);
  c5_6->SetRightMargin(0.05);
  c5_6->SetTopMargin(0.05);

  // lecture 0, position dans le faisceau 4eme
  c5->cd(4);
  f->cd("0900");
  Eta_0900_K->GetXaxis()->SetLabelSize(0.065);
  Eta_0900_K->GetXaxis()->SetLabelOffset(0.015);
  Eta_0900_K->GetXaxis()->SetTitle("#eta");
  Eta_0900_K->GetXaxis()->CenterTitle();
  Eta_0900_K->GetXaxis()->SetTitleSize(0.10);
  Eta_0900_K->GetXaxis()->SetTitleOffset(0.8);
  Eta_0900_K->GetYaxis()->SetLabelSize(0.060);
  Eta_0900_K->GetXaxis()->SetRangeUser(0.,1.);
  Eta_0900_K->SetLineColor(4);
  Eta_0900_K->Draw("hist");  
  Eta_0900_S->SetLineColor(2);
  Eta_0900_S->Draw("samehist");
  // lecture 1, position dans le faisceau 2eme
  c5->cd(2);
  f->cd("0901");
  Eta_0901_K->GetXaxis()->SetLabelSize(0.065);
  Eta_0901_K->GetXaxis()->SetLabelOffset(0.015);
  Eta_0901_K->GetYaxis()->SetLabelSize(0.060); 
  Eta_0901_K->GetXaxis()->SetTitle("#eta");
  Eta_0901_K->GetXaxis()->CenterTitle();
  Eta_0901_K->GetXaxis()->SetTitleSize(0.10);
  Eta_0901_K->GetXaxis()->SetTitleOffset(0.8);
  Eta_0901_K->GetXaxis()->SetRangeUser(0.,1.);
  Eta_0901_K->SetLineColor(4);
  Eta_0901_K->Draw("hist");
  Eta_0901_S->SetLineColor(2);
  Eta_0901_S->Draw("samehist");
  // lecture 2, position dans le faisceau 6eme
  c5->cd(6);
  f->cd("0904");
  Eta_0904_K->GetXaxis()->SetLabelSize(0.065);
  Eta_0904_K->GetXaxis()->SetLabelOffset(0.015);
  Eta_0904_K->GetXaxis()->SetTitle("#eta");
  Eta_0904_K->GetXaxis()->CenterTitle();
  Eta_0904_K->GetXaxis()->SetTitleSize(0.10);
  Eta_0904_K->GetXaxis()->SetTitleOffset(0.8);
  Eta_0904_K->GetYaxis()->SetLabelSize(0.060);
  Eta_0904_K->GetXaxis()->SetRangeUser(0.,1.);
  Eta_0904_K->SetLineColor(4);
  Eta_0904_K->Draw("hist");
  Eta_0904_S->SetLineColor(2);
  Eta_0904_S->Draw("samehist");
  // lecture 3, position dans le faisceau 3eme
  c5->cd(3);
  f->cd("0905");
  Eta_0905_K->GetXaxis()->SetLabelSize(0.065);
  Eta_0905_K->GetXaxis()->SetLabelOffset(0.015);
  Eta_0905_K->GetXaxis()->SetTitle("#eta");
  Eta_0905_K->GetXaxis()->CenterTitle();
  Eta_0905_K->GetXaxis()->SetTitleSize(0.10);
  Eta_0905_K->GetXaxis()->SetTitleOffset(0.8);
  Eta_0905_K->GetYaxis()->SetLabelSize(0.060);
  Eta_0905_K->GetXaxis()->SetRangeUser(0.,1.);
  Eta_0905_K->SetLineColor(4);
  Eta_0905_K->Draw("hist");
  Eta_0905_S->SetLineColor(2);
  Eta_0905_S->Draw("samehist");
  // lecture 4, position dans le faisceau 5eme
  c5->cd(5);
  f->cd("2110");
  Eta_2110_K->GetXaxis()->SetLabelSize(0.065);
  Eta_2110_K->GetXaxis()->SetLabelOffset(0.015);
  Eta_2110_K->GetYaxis()->SetLabelSize(0.060);
  Eta_2110_K->GetXaxis()->SetTitle("#eta");
  Eta_2110_K->GetXaxis()->CenterTitle();
  Eta_2110_K->GetXaxis()->SetTitleSize(0.10);
  Eta_2110_K->GetXaxis()->SetTitleOffset(0.8);
  Eta_2110_K->GetXaxis()->SetRangeUser(0.,500.);
  Eta_2110_K->GetXaxis()->SetRangeUser(0.,1.);
  Eta_2110_K->SetLineColor(4);
  Eta_2110_K->Draw("hist");
  Eta_2110_S->SetLineColor(2);
  Eta_2110_S->Draw("samehist");
  // lecture 5, position dans le faisceau 1ere
  c5->cd(1);
  f->cd("2111");
  Eta_2111_K->GetXaxis()->SetLabelSize(0.065);
  Eta_2111_K->GetXaxis()->SetLabelOffset(0.015);
  Eta_2111_K->GetXaxis()->SetTitle("#eta");
  Eta_2111_K->GetXaxis()->CenterTitle();
  Eta_2111_K->GetXaxis()->SetTitleSize(0.10);
  Eta_2111_K->GetXaxis()->SetTitleOffset(0.8);
  Eta_2111_K->GetYaxis()->SetLabelSize(0.060);
  Eta_2111_K->GetXaxis()->SetRangeUser(0.,500.);
  Eta_2111_K->GetXaxis()->SetRangeUser(0.,1.);
  Eta_2111_K->SetLineColor(4);
  Eta_2111_K->Draw("hist");
  Eta_2111_S->SetLineColor(2);
  Eta_2111_S->Draw("samehist");

  TCanvas* c6=new TCanvas("c6","position gap",0,0,650,650);
  c6->Divide(2,3,0.0001,0.0001);
  c6_1->SetFillColor(10);
  c6_1->Range(-10,-10,10,10);
  c6_1->SetLeftMargin(0.12);
  c6_1->SetBottomMargin(0.18);
  c6_1->SetRightMargin(0.05);
  c6_1->SetTopMargin(0.05);
  c6_2->SetFillColor(10);
  c6_2->Range(-10,-10,10,10);
  c6_2->SetLeftMargin(0.12);
  c6_2->SetBottomMargin(0.18);
  c6_2->SetRightMargin(0.05);
  c6_2->SetTopMargin(0.05);
  c6_3->SetFillColor(10);
  c6_3->Range(-10,-10,10,10);
  c6_3->SetLeftMargin(0.12);
  c6_3->SetBottomMargin(0.18);
  c6_3->SetRightMargin(0.05);
  c6_3->SetTopMargin(0.05);
  c6_4->SetFillColor(10);
  c6_4->Range(-10,-10,10,10);
  c6_4->SetLeftMargin(0.12);
  c6_4->SetBottomMargin(0.18);
  c6_4->SetRightMargin(0.05);
  c6_4->SetTopMargin(0.05);
  c6_5->SetFillColor(10);
  c6_5->Range(-10,-10,10,10);
  c6_5->SetLeftMargin(0.12);
  c6_5->SetBottomMargin(0.18);
  c6_5->SetRightMargin(0.05);
  c6_5->SetTopMargin(0.05);
  c6_6->SetFillColor(10);
  c6_6->Range(-10,-10,10,10);
  c6_6->SetLeftMargin(0.12);
  c6_6->SetBottomMargin(0.18);
  c6_6->SetRightMargin(0.05);
  c6_6->SetTopMargin(0.05);

  // lecture 0, position faisceau 3eme AMS02
  c6->cd(3);
  f->cd("0900");
  OccupancyGap_0900_K->GetXaxis()->SetLabelSize(0.065);
  OccupancyGap_0900_K->GetXaxis()->SetLabelOffset(0.015);
  OccupancyGap_0900_K->GetYaxis()->SetLabelSize(0.060);
  OccupancyGap_0900_K->GetXaxis()->SetRangeUser(0.,1.);
  OccupancyGap_0900_K->SetLineColor(4);
  OccupancyGap_0900_K->Draw("hist");  
  OccupancyGap_0900_S->SetLineColor(2);
  OccupancyGap_0900_S->Draw("samehist");
  // lecture 1, position faisceau 1ere AMS02
  c6->cd(1);
  f->cd("0901");
  OccupancyGap_0901_K->GetXaxis()->SetLabelSize(0.065);
  OccupancyGap_0901_K->GetXaxis()->SetLabelOffset(0.015);
  OccupancyGap_0901_K->GetYaxis()->SetLabelSize(0.060);
  OccupancyGap_0901_K->GetXaxis()->SetRangeUser(0.,1.);
  OccupancyGap_0901_K->SetLineColor(4);
  OccupancyGap_0901_K->Draw("hist");
  OccupancyGap_0901_S->SetLineColor(2);
  OccupancyGap_0901_S->Draw("samehist");
  // lecture 2, position faisceau 4eme AMS02
  c6->cd(4);
  f->cd("0904");
  OccupancyGap_0904_K->GetXaxis()->SetLabelSize(0.065);
  OccupancyGap_0904_K->GetXaxis()->SetLabelOffset(0.015);
  OccupancyGap_0904_K->GetYaxis()->SetLabelSize(0.060);
  OccupancyGap_0904_K->GetXaxis()->SetRangeUser(0.,1.);
  OccupancyGap_0904_K->SetLineColor(4);
  OccupancyGap_0904_K->Draw("hist");
  OccupancyGap_0904_S->SetLineColor(2);
  OccupancyGap_0904_S->Draw("samehist");
  // lecture 3, position faisceau 2eme AMS02
  c6->cd(2);
  f->cd("0905");
  OccupancyGap_0905_K->GetXaxis()->SetLabelSize(0.065);
  OccupancyGap_0905_K->GetXaxis()->SetLabelOffset(0.015);
  OccupancyGap_0905_K->GetYaxis()->SetLabelSize(0.060);
  OccupancyGap_0905_K->GetXaxis()->SetRangeUser(0.,1.);
  OccupancyGap_0905_K->SetLineColor(4);
  OccupancyGap_0905_K->Draw("hist");
  OccupancyGap_0905_S->SetLineColor(2);
  OccupancyGap_0905_S->Draw("samehist");
  c6->cd(5);
  f->cd("2110");
  OccupancyGap_2110_K->GetXaxis()->SetLabelSize(0.065);
  OccupancyGap_2110_K->GetXaxis()->SetLabelOffset(0.015);
  OccupancyGap_2110_K->GetYaxis()->SetLabelSize(0.060);
  OccupancyGap_2110_K->GetXaxis()->SetRangeUser(0.,500.);
  OccupancyGap_2110_K->GetXaxis()->SetRangeUser(0.,1.);
  OccupancyGap_2110_K->SetLineColor(4);
  OccupancyGap_2110_K->Draw("hist");
  OccupancyGap_2110_S->SetLineColor(2);
  OccupancyGap_2110_S->Draw("samehist");
  c6->cd(6);
  f->cd("2111");
  OccupancyGap_2111_K->GetXaxis()->SetLabelSize(0.065);
  OccupancyGap_2111_K->GetXaxis()->SetLabelOffset(0.015);
  OccupancyGap_2111_K->GetYaxis()->SetLabelSize(0.060);
  OccupancyGap_2111_K->GetXaxis()->SetRangeUser(0.,500.);
  OccupancyGap_2111_K->GetXaxis()->SetRangeUser(0.,1.);
  OccupancyGap_2111_K->SetLineColor(4);
  OccupancyGap_2111_K->Draw("hist");
  OccupancyGap_2111_S->SetLineColor(2);
  OccupancyGap_2111_S->Draw("samehist");

  return;
  char cpad[20];
  TCanvas* c8=new TCanvas("c8","amas > 7 pistes",0,0,650,650);
  c8->Divide(2,3,0.0001,0.0001);
  c8_1->SetFillColor(10);
  c8_1->Range(-10,-10,10,10);
  c8_1->SetLeftMargin(0.12);
  c8_1->SetBottomMargin(0.18);
  c8_1->SetRightMargin(0.10);
  c8_1->SetTopMargin(0.05);
  c8_1->SetLogy(1);
  c8_2->SetFillColor(10);
  c8_2->Range(-10,-10,10,10);
  c8_2->SetLeftMargin(0.12);
  c8_2->SetBottomMargin(0.18);
  c8_2->SetRightMargin(0.10);
  c8_2->SetTopMargin(0.05);
  c8_2->SetLogy(1);
  c8_3->SetFillColor(10);
  c8_3->Range(-10,-10,10,10);
  c8_3->SetLeftMargin(0.12);
  c8_3->SetBottomMargin(0.18);
  c8_3->SetRightMargin(0.10);
  c8_3->SetTopMargin(0.05);
  c8_3->SetLogy(1);
  c8_4->SetFillColor(10);
  c8_4->Range(-10,-10,10,10);
  c8_4->SetLeftMargin(0.12);
  c8_4->SetBottomMargin(0.18);
  c8_4->SetRightMargin(0.10);
  c8_4->SetTopMargin(0.05);
  c8_4->SetLogy(1);
  c8_5->SetFillColor(10);
  c8_5->Range(-10,-10,10,10);
  c8_5->SetLeftMargin(0.12);
  c8_5->SetBottomMargin(0.18);
  c8_5->SetRightMargin(0.10);
  c8_5->SetTopMargin(0.05);
  c8_5->SetLogy(1);
  c8_6->SetFillColor(10);
  c8_6->Range(-10,-10,10,10);
  c8_6->SetLeftMargin(0.12);
  c8_6->SetBottomMargin(0.18);
  c8_6->SetRightMargin(0.10);
  c8_6->SetTopMargin(0.05);
  c8_6->SetLogy(1);

  int pge8[6][2];
  for (int i=0; i<6; i++) 
    for (int j=0; j<2; j++)
    pge8[i][j] = 0;

  // lecture 0, position faisceau 4eme position
  c8->cd(4);
  f->cd();
  Accept_ev->GetXaxis()->SetLabelSize(0.065);
  Accept_ev->GetXaxis()->SetLabelOffset(0.015);
  Accept_ev->GetYaxis()->SetLabelSize(0.060);
  Accept_ev->SetMaximum(500.);
  Accept_ev->SetMinimum(0.5);
  Accept_ev->SetLineColor(1);
  Accept_ev->Draw("hist");
  f->cd("0900");
  EVpge8_0900_K->SetLineColor(4);
  EVpge8_0900_K->Draw("samehist");  
  EVpge8_0900_S->SetLineColor(2);
  EVpge8_0900_S->Draw("samehist");
  pge8[3][0] = EVpge8_0900_S->GetEntries();
  pge8[3][1] = EVpge8_0900_K->GetEntries();
  // lecture 1, position faisceau 2eme position
  c8->cd(2);
  f->cd();
  Accept_ev->Draw("hist");
  f->cd("0901");
  EVpge8_0901_K->SetLineColor(4);
  EVpge8_0901_K->Draw("samehist");
  EVpge8_0901_S->SetLineColor(2);
  EVpge8_0901_S->Draw("samehist");
  pge8[1][0] = EVpge8_0901_S->GetEntries();
  pge8[1][1] = EVpge8_0901_K->GetEntries();
  // lecture 2, position faisceau 6eme position
  c8->cd(6);
  f->cd();
  Accept_ev->Draw("hist");
  f->cd("0904");
  EVpge8_0904_K->SetLineColor(4);
  EVpge8_0904_K->Draw("samehist");
  EVpge8_0904_S->SetLineColor(2);
  EVpge8_0904_S->Draw("samehist");
  pge8[5][0] = EVpge8_0904_S->GetEntries();
  pge8[5][1] = EVpge8_0904_K->GetEntries();
  // lecture 3, position faisceau 3eme position
  c8->cd(3);
  f->cd();
  Accept_ev->Draw("hist");
  f->cd("0905");
  EVpge8_0905_K->SetLineColor(4);
  EVpge8_0905_K->Draw("samehist");
  EVpge8_0905_S->SetLineColor(2);
  EVpge8_0905_S->Draw("samehist");
  pge8[2][0] = EVpge8_0905_S->GetEntries();
  pge8[2][1] = EVpge8_0905_K->GetEntries();
  // lecture 4, position faisceau 5eme position
  c8->cd(5);
  f->cd();
  Accept_ev->Draw("hist");
  f->cd("2110");
  EVpge8_2110_K->SetLineColor(4);
  EVpge8_2110_K->Draw("samehist");
  EVpge8_2110_S->SetLineColor(2);
  EVpge8_2110_S->Draw("samehist");
  pge8[4][0] = EVpge8_2110_S->GetEntries();
  pge8[4][1] = EVpge8_2110_K->GetEntries();
  // lecture 5, position faisceau 1ere position
  c8->cd(1);
  f->cd();
  Accept_ev->Draw("hist");
  f->cd("2111");
  EVpge8_2111_K->SetLineColor(4);
  EVpge8_2111_K->Draw("samehist");
  EVpge8_2111_S->SetLineColor(2);
  EVpge8_2111_S->Draw("samehist");
  pge8[0][0] = EVpge8_2111_S->GetEntries();
  pge8[0][1] = EVpge8_2111_K->GetEntries();

  printf("       piste >= 8   p     n\n");
  for (int i=0; i<6; i++)
  printf("pos %d              %d     %d\n",i+1,pge8[i][0],pge8[i][1]);
  printf("\n");

  TCanvas* c9=new TCanvas("c9","profile sn",0,0,650,650);
  c9->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c9->cd(i+1);
    sprintf(cpad,"c9_%d",i+1);
    TPad *ptpad = c9->FindObject(cpad);
    //    printf("ptpad %d\n",ptpad2);
   ptpad->SetFillColor(10);
   ptpad->Range(-10,-10,10,10);
   ptpad->SetLeftMargin(0.12);
   ptpad->SetBottomMargin(0.18);
   ptpad->SetRightMargin(0.05);
   ptpad->SetTopMargin(0.05);
  }

  // lecture 0, position faisceau 4eme 
  char chist[80];
  TH1D* hsns[6]; 
  TH1D* hnsns[6];
  TH1D* hsnk[6]; 
  TH1D* hnsnk[6];
  c9->cd(4);
  sprintf(chist,"0900/SNpiste_0900_S");
  hsns[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNpiste_0900_S");
  hnsns[0] = (TH1D*) f->Get(chist)->Clone();
  hsns[0]->Divide(hnsns[0]);
  sprintf(chist,"0900/SNpiste_0900_K");
  hsnk[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNpiste_0900_K");
  hnsnk[0] = (TH1D*) f->Get(chist)->Clone();
  hsnk[0]->Divide(hnsnk[0]);
  hsns[0]->SetMaximum(15.);
  hsns[0]->GetXaxis()->SetLabelSize(0.065);
  hsns[0]->GetXaxis()->SetLabelOffset(0.015);
  hsns[0]->GetYaxis()->SetLabelSize(0.060);
  hsns[0]->SetLineColor(2);
  hsns[0]->Draw("hist");  
  hsnk[0]->SetLineColor(4);
  hsnk[0]->Draw("samehist");
  // lecture 1, position faisceau 2eme
  c9->cd(2);
  sprintf(chist,"0901/SNpiste_0901_S");
  hsns[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNpiste_0901_S");
  hnsns[1] = (TH1D*) f->Get(chist)->Clone();
  hsns[1]->Divide(hnsns[1]);
  sprintf(chist,"0901/SNpiste_0901_K");
  hsnk[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNpiste_0901_K");
  hnsnk[1] = (TH1D*) f->Get(chist)->Clone();
  hsnk[1]->Divide(hnsnk[1]);
  hsns[1]->SetMaximum(15.);
  hsns[1]->GetXaxis()->SetLabelSize(0.065);
  hsns[1]->GetXaxis()->SetLabelOffset(0.015);
  hsns[1]->GetYaxis()->SetLabelSize(0.060);
  hsns[1]->SetLineColor(2);
  hsns[1]->Draw("hist");  
  hsnk[1]->SetLineColor(4);
  hsnk[1]->Draw("samehist");
  // lecture 2, position faisceau 6eme
  c9->cd(6);
  sprintf(chist,"0904/SNpiste_0904_S");
  hsns[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNpiste_0904_S");
  hnsns[2] = (TH1D*) f->Get(chist)->Clone();
  hsns[2]->Divide(hnsns[2]);
  sprintf(chist,"0904/SNpiste_0904_K");
  hsnk[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNpiste_0904_K");
  hnsnk[2] = (TH1D*) f->Get(chist)->Clone();
  hsnk[2]->Divide(hnsnk[2]);
  hsns[2]->SetMaximum(15.);
  hsns[2]->GetXaxis()->SetLabelSize(0.065);
  hsns[2]->GetXaxis()->SetLabelOffset(0.015);
  hsns[2]->GetYaxis()->SetLabelSize(0.060);
  hsns[2]->SetLineColor(2);
  hsns[2]->Draw("hist");  
  hsnk[2]->SetLineColor(4);
  hsnk[2]->Draw("samehist");
  // lecture 3, position faisceau 3eme
  c9->cd(3);
  sprintf(chist,"0905/SNpiste_0905_S");
  hsns[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNpiste_0905_S");
  hnsns[3] = (TH1D*) f->Get(chist)->Clone();
  hsns[3]->Divide(hnsns[3]);
  sprintf(chist,"0905/SNpiste_0905_K");
  hsnk[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNpiste_0905_K");
  hnsnk[3] = (TH1D*) f->Get(chist)->Clone();
  hsnk[3]->Divide(hnsnk[3]);
  hsns[3]->SetMaximum(15.);
  hsns[3]->GetXaxis()->SetLabelSize(0.065);
  hsns[3]->GetXaxis()->SetLabelOffset(0.015);
  hsns[3]->GetYaxis()->SetLabelSize(0.060);
  hsns[3]->SetLineColor(2);
  hsns[3]->Draw("hist");  
  hsnk[3]->SetLineColor(4);
  hsnk[3]->Draw("samehist");
  // lecture 4, position faisceau 5eme
  c9->cd(5);
  sprintf(chist,"2110/SNpiste_2110_S");
  hsns[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNpiste_2110_S");
  hnsns[4] = (TH1D*) f->Get(chist)->Clone();
  hsns[4]->Divide(hnsns[4]);
  sprintf(chist,"2110/SNpiste_2110_K");
  hsnk[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNpiste_2110_K");
  hnsnk[4] = (TH1D*) f->Get(chist)->Clone();
  hsnk[4]->Divide(hnsnk[4]);
  hsns[4]->SetMaximum(60.);
  hsns[4]->GetXaxis()->SetLabelSize(0.065);
  hsns[4]->GetXaxis()->SetLabelOffset(0.015);
  hsns[4]->GetYaxis()->SetLabelSize(0.060);
  hsns[4]->SetLineColor(2);
  hsns[4]->Draw("hist");  
  hsnk[4]->SetLineColor(4);
  hsnk[4]->Draw("samehist");
  // lecture 5, position faisceau 1ere
  c9->cd(1);
  sprintf(chist,"2111/SNpiste_2111_S");
  hsns[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNpiste_2111_S");
  hnsns[5] = (TH1D*) f->Get(chist)->Clone();
  hsns[5]->Divide(hnsns[5]);
  sprintf(chist,"2111/SNpiste_2111_K");
  hsnk[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNpiste_2111_K");
  hnsnk[5] = (TH1D*) f->Get(chist)->Clone();
  hsnk[5]->Divide(hnsnk[5]);
  hsns[5]->SetMaximum(60.);
  hsns[5]->GetXaxis()->SetLabelSize(0.065);
  hsns[5]->GetXaxis()->SetLabelOffset(0.015);
  hsns[5]->GetYaxis()->SetLabelSize(0.060);
  hsns[5]->SetLineColor(2);
  hsns[5]->Draw("hist");  
  hsnk[5]->SetLineColor(4);
  hsnk[5]->Draw("samehist");

  /*  TCanvas* c10=new TCanvas("c10","profile sn2",0,0,650,650);
  c10->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c10->cd(i+1);
    sprintf(cpad,"c10_%d",i+1);
    TPad *ptpad2 = c10->FindObject(cpad);
    //    printf("ptpad %d\n",ptpad2);
   ptpad2->SetFillColor(10);
   ptpad2->Range(-10,-10,10,10);
   ptpad2->SetLeftMargin(0.12);
   ptpad2->SetBottomMargin(0.18);
   ptpad2->SetRightMargin(0.05);
   ptpad2->SetTopMargin(0.05);
   } */

  TH1D* hsns2[6]; 
  TH1D* hnsns2[6];
  TH1D* hsnk2[6]; 
  TH1D* hnsnk2[6];
  // lecture 0, position faisceau 4eme 
  c9->cd(4);
  sprintf(chist,"0900/SNpiste2_0900_S");
  hsns2[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNpiste2_0900_S");
  hnsns2[0] = (TH1D*) f->Get(chist)->Clone();
  hsns2[0]->Divide(hnsns2[0]);
  sprintf(chist,"0900/SNpiste2_0900_K");
  hsnk2[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNpiste2_0900_K");
  hnsnk2[0] = (TH1D*) f->Get(chist)->Clone();
  hsnk2[0]->Divide(hnsnk2[0]);
  hsns2[0]->GetXaxis()->SetLabelSize(0.065);
  hsns2[0]->GetXaxis()->SetLabelOffset(0.015);
  hsns2[0]->GetYaxis()->SetLabelSize(0.060);
  hsns2[0]->SetLineColor(2);
  hsns2[0]->SetLineStyle(2);
  hsns2[0]->Draw("samehist");  
  hsnk2[0]->SetLineColor(4);
  hsnk2[0]->SetLineStyle(2);
  hsnk2[0]->Draw("samehist");
  // lecture 1, position faisceau 2eme
  c9->cd(2);
  sprintf(chist,"0901/SNpiste2_0901_S");
  hsns2[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNpiste2_0901_S");
  hnsns2[1] = (TH1D*) f->Get(chist)->Clone();
  hsns2[1]->Divide(hnsns2[1]);
  sprintf(chist,"0901/SNpiste2_0901_K");
  hsnk2[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNpiste2_0901_K");
  hnsnk2[1] = (TH1D*) f->Get(chist)->Clone();
  hsnk2[1]->Divide(hnsnk2[1]);
  hsns2[1]->GetXaxis()->SetLabelSize(0.065);
  hsns2[1]->GetXaxis()->SetLabelOffset(0.015);
  hsns2[1]->GetYaxis()->SetLabelSize(0.060);
  hsns2[1]->SetLineColor(2);
  hsns2[1]->SetLineStyle(2);
  hsns2[1]->Draw("samehist");  
  hsnk2[1]->SetLineColor(4);
  hsnk2[1]->SetLineStyle(2);
  hsnk2[1]->Draw("samehist");
  // lecture 2, position faisceau 6eme
  c9->cd(6);
  sprintf(chist,"0904/SNpiste2_0904_S");
  hsns2[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNpiste2_0904_S");
  hnsns2[2] = (TH1D*) f->Get(chist)->Clone();
  hsns2[2]->Divide(hnsns2[2]);
  sprintf(chist,"0904/SNpiste2_0904_K");
  hsnk2[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNpiste2_0904_K");
  hnsnk2[2] = (TH1D*) f->Get(chist)->Clone();
  hsnk2[2]->Divide(hnsnk2[2]);
  hsns2[2]->GetXaxis()->SetLabelSize(0.065);
  hsns2[2]->GetXaxis()->SetLabelOffset(0.015);
  hsns2[2]->GetYaxis()->SetLabelSize(0.060);
  hsns2[2]->SetLineColor(2);
  hsns2[2]->SetLineStyle(2);
  hsns2[2]->Draw("samehist");  
  hsnk2[2]->SetLineColor(4);
  hsnk2[2]->SetLineStyle(2);
  hsnk2[2]->Draw("samehist");
  // lecture 3, position faisceau 3eme
  c9->cd(3);
  sprintf(chist,"0905/SNpiste2_0905_S");
  hsns2[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNpiste2_0905_S");
  hnsns2[3] = (TH1D*) f->Get(chist)->Clone();
  hsns2[3]->Divide(hnsns2[3]);
  sprintf(chist,"0905/SNpiste2_0905_K");
  hsnk2[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNpiste2_0905_K");
  hnsnk2[3] = (TH1D*) f->Get(chist)->Clone();
  hsnk2[3]->Divide(hnsnk2[3]);
  hsns2[3]->GetXaxis()->SetLabelSize(0.065);
  hsns2[3]->GetXaxis()->SetLabelOffset(0.015);
  hsns2[3]->GetYaxis()->SetLabelSize(0.060);
  hsns2[3]->SetLineColor(2);
  hsns2[3]->SetLineStyle(2);
  hsns2[3]->Draw("samehist");  
  hsnk2[3]->SetLineColor(4);
  hsnk2[3]->SetLineStyle(2);
  hsnk2[3]->Draw("samehist");
  // lecture 4, position faisceau 5eme
  c9->cd(5);
  sprintf(chist,"2110/SNpiste2_2110_S");
  hsns2[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNpiste2_2110_S");
  hnsns2[4] = (TH1D*) f->Get(chist)->Clone();
  hsns2[4]->Divide(hnsns2[4]);
  sprintf(chist,"2110/SNpiste2_2110_K");
  hsnk2[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNpiste2_2110_K");
  hnsnk2[4] = (TH1D*) f->Get(chist)->Clone();
  hsnk2[4]->Divide(hnsnk2[4]);
  hsns2[4]->GetXaxis()->SetLabelSize(0.065);
  hsns2[4]->GetXaxis()->SetLabelOffset(0.015);
  hsns2[4]->GetYaxis()->SetLabelSize(0.060);
  hsns2[4]->SetLineColor(2);
  hsns2[4]->SetLineStyle(2);
  hsns2[4]->Draw("samehist");  
  hsnk2[4]->SetLineColor(4);
  hsnk2[4]->SetLineStyle(2);
  hsnk2[4]->Draw("samehist");
  // lecture 5, position faisceau 1ere
  c9->cd(1);
  sprintf(chist,"2111/SNpiste2_2111_S");
  hsns2[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNpiste2_2111_S");
  hnsns2[5] = (TH1D*) f->Get(chist)->Clone();
  hsns2[5]->Divide(hnsns2[5]);
  sprintf(chist,"2111/SNpiste2_2111_K");
  hsnk2[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNpiste2_2111_K");
  hnsnk2[5] = (TH1D*) f->Get(chist)->Clone();
  hsnk2[5]->Divide(hnsnk2[5]);
  hsns2[5]->GetXaxis()->SetLabelSize(0.065);
  hsns2[5]->GetXaxis()->SetLabelOffset(0.015);
  hsns2[5]->GetYaxis()->SetLabelSize(0.060);
  hsns2[5]->SetLineColor(2);
  hsns2[5]->SetLineStyle(2);
  hsns2[5]->Draw("samehist");  
  hsnk2[5]->SetLineColor(4);
  hsnk2[5]->SetLineStyle(2);
  hsnk2[5]->Draw("samehist");

  TCanvas* c10=new TCanvas("c10","profile > 8 sn ",0,0,650,650);
  c10->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c10->cd(i+1);
    sprintf(cpad,"c10_%d",i+1);
    TPad *ptpad2 = c10->FindObject(cpad);
    //    printf("ptpad %d\n",ptpad2);
   ptpad2->SetFillColor(10);
   ptpad2->Range(-10,-10,10,10);
   ptpad2->SetLeftMargin(0.12);
   ptpad2->SetBottomMargin(0.18);
   ptpad2->SetRightMargin(0.05);
   ptpad2->SetTopMargin(0.05);
  }

  // lecture 0, position faisceau 4eme 
  TH1D* hsnsg[6]; 
  TH1D* hnsnsg[6];
  TH1D* hsnkg[6]; 
  TH1D* hnsnkg[6];
  c10->cd(4);
  sprintf(chist,"0900/SNp_0900_S");
  hsnsg[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNp_0900_S");
  hnsnsg[0] = (TH1D*) f->Get(chist)->Clone();
  hsnsg[0]->Divide(hnsnsg[0]);
  sprintf(chist,"0900/SNp_0900_K");
  hsnkg[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNp_0900_K");
  hnsnkg[0] = (TH1D*) f->Get(chist)->Clone();
  hsnkg[0]->Divide(hnsnkg[0]);
  hsnsg[0]->SetMaximum(100.);
  hsnsg[0]->GetXaxis()->SetLabelSize(0.065);
  hsnsg[0]->GetXaxis()->SetLabelOffset(0.015);
  hsnsg[0]->GetYaxis()->SetLabelSize(0.060);
  hsnsg[0]->SetLineColor(2);
  hsnsg[0]->Draw("hist");  
  hsnkg[0]->SetLineColor(4);
  hsnkg[0]->Draw("samehist");
  // lecture 1, position faisceau 2eme
  c10->cd(2);
  sprintf(chist,"0901/SNp_0901_S");
  hsnsg[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNp_0901_S");
  hnsnsg[1] = (TH1D*) f->Get(chist)->Clone();
  hsnsg[1]->Divide(hnsnsg[1]);
  sprintf(chist,"0901/SNp_0901_K");
  hsnkg[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNp_0901_K");
  hnsnkg[1] = (TH1D*) f->Get(chist)->Clone();
  hsnkg[1]->Divide(hnsnkg[1]);
  hsnsg[1]->SetMaximum(100.);
  hsnsg[1]->GetXaxis()->SetLabelSize(0.065);
  hsnsg[1]->GetXaxis()->SetLabelOffset(0.015);
  hsnsg[1]->GetYaxis()->SetLabelSize(0.060);
  hsnsg[1]->SetLineColor(2);
  hsnsg[1]->Draw("hist");  
  hsnkg[1]->SetLineColor(4);
  hsnkg[1]->Draw("samehist");
  // lecture 2, position faisceau 6eme
  c10->cd(6);
  sprintf(chist,"0904/SNp_0904_S");
  hsnsg[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNp_0904_S");
  hnsnsg[2] = (TH1D*) f->Get(chist)->Clone();
  hsnsg[2]->Divide(hnsnsg[2]);
  sprintf(chist,"0904/SNp_0904_K");
  hsnkg[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNp_0904_K");
  hnsnkg[2] = (TH1D*) f->Get(chist)->Clone();
  hsnkg[2]->Divide(hnsnkg[2]);
  hsnsg[2]->SetMaximum(100.);
  hsnsg[2]->GetXaxis()->SetLabelSize(0.065);
  hsnsg[2]->GetXaxis()->SetLabelOffset(0.015);
  hsnsg[2]->GetYaxis()->SetLabelSize(0.060);
  hsnsg[2]->SetLineColor(2);
  hsnsg[2]->Draw("hist");  
  hsnkg[2]->SetLineColor(4);
  hsnkg[2]->Draw("samehist");
  // lecture 3, position faisceau 3eme
  c10->cd(3);
  sprintf(chist,"0905/SNp_0905_S");
  hsnsg[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNp_0905_S");
  hnsnsg[3] = (TH1D*) f->Get(chist)->Clone();
  hsnsg[3]->Divide(hnsnsg[3]);
  sprintf(chist,"0905/SNp_0905_K");
  hsnkg[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNp_0905_K");
  hnsnkg[3] = (TH1D*) f->Get(chist)->Clone();
  hsnkg[3]->Divide(hnsnkg[3]);
  hsnsg[3]->SetMaximum(100.);
  hsnsg[3]->GetXaxis()->SetLabelSize(0.065);
  hsnsg[3]->GetXaxis()->SetLabelOffset(0.015);
  hsnsg[3]->GetYaxis()->SetLabelSize(0.060);
  hsnsg[3]->SetLineColor(2);
  hsnsg[3]->Draw("hist");  
  hsnkg[3]->SetLineColor(4);
  hsnkg[3]->Draw("samehist");
  // lecture 4, position faisceau 5eme
  c10->cd(5);
  sprintf(chist,"2110/SNp_2110_S");
  hsnsg[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNp_2110_S");
  hnsnsg[4] = (TH1D*) f->Get(chist)->Clone();
  hsnsg[4]->Divide(hnsnsg[4]);
  sprintf(chist,"2110/SNp_2110_K");
  hsnkg[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNp_2110_K");
  hnsnkg[4] = (TH1D*) f->Get(chist)->Clone();
  hsnkg[4]->Divide(hnsnkg[4]);
  hsnsg[4]->SetMaximum(100.);
  hsnsg[4]->GetXaxis()->SetLabelSize(0.065);
  hsnsg[4]->GetXaxis()->SetLabelOffset(0.015);
  hsnsg[4]->GetYaxis()->SetLabelSize(0.060);
  hsnsg[4]->SetLineColor(2);
  hsnsg[4]->Draw("hist");  
  hsnkg[4]->SetLineColor(4);
  hsnkg[4]->Draw("samehist");
  // lecture 5, position faisceau 1ere
  c10->cd(1);
  sprintf(chist,"2111/SNp_2111_S");
  hsnsg[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNp_2111_S");
  hnsnsg[5] = (TH1D*) f->Get(chist)->Clone();
  hsnsg[5]->Divide(hnsnsg[5]);
  sprintf(chist,"2111/SNp_2111_K");
  hsnkg[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNp_2111_K");
  hnsnkg[5] = (TH1D*) f->Get(chist)->Clone();
  hsnkg[5]->Divide(hnsnkg[5]);
  hsnsg[5]->SetMaximum(100.);
  hsnsg[5]->GetXaxis()->SetLabelSize(0.065);
  hsnsg[5]->GetXaxis()->SetLabelOffset(0.015);
  hsnsg[5]->GetYaxis()->SetLabelSize(0.060);
  hsnsg[5]->SetLineColor(2);
  hsnsg[5]->Draw("hist");  
  hsnkg[5]->SetLineColor(4);
  hsnkg[5]->Draw("samehist");

  /*  TCanvas* c10=new TCanvas("c10","profile sn2",0,0,650,650);
  c10->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c10->cd(i+1);
    sprintf(cpad,"c10_%d",i+1);
    TPad *ptpad2 = c10->FindObject(cpad);
    //    printf("ptpad %d\n",ptpad2);
   ptpad2->SetFillColor(10);
   ptpad2->Range(-10,-10,10,10);
   ptpad2->SetLeftMargin(0.12);
   ptpad2->SetBottomMargin(0.18);
   ptpad2->SetRightMargin(0.05);
   ptpad2->SetTopMargin(0.05);
   } */

  TH1D* hsns2g[6]; 
  TH1D* hnsns2g[6];
  TH1D* hsnk2g[6]; 
  TH1D* hnsnk2g[6];
  // lecture 0, position faisceau 4eme 
  c10->cd(4);
  sprintf(chist,"0900/SNp2_0900_S");
  hsns2g[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNp2_0900_S");
  hnsns2g[0] = (TH1D*) f->Get(chist)->Clone();
  hsns2g[0]->Divide(hnsns2g[0]);
  sprintf(chist,"0900/SNp2_0900_K");
  hsnk2g[0] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0900/NSNp2_0900_K");
  hnsnk2g[0] = (TH1D*) f->Get(chist)->Clone();
  hsnk2g[0]->Divide(hnsnk2g[0]);
  hsns2g[0]->GetXaxis()->SetLabelSize(0.065);
  hsns2g[0]->GetXaxis()->SetLabelOffset(0.015);
  hsns2g[0]->GetYaxis()->SetLabelSize(0.060);
  hsns2g[0]->SetLineColor(2);
  hsns2g[0]->SetLineStyle(2);
  hsns2g[0]->Draw("samehist");  
  hsnk2g[0]->SetLineColor(4);
  hsnk2g[0]->SetLineStyle(2);
  hsnk2g[0]->Draw("samehist");
  // lecture 1, position faisceau 2eme
  c10->cd(2);
  sprintf(chist,"0901/SNp2_0901_S");
  hsns2g[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNp2_0901_S");
  hnsns2g[1] = (TH1D*) f->Get(chist)->Clone();
  hsns2g[1]->Divide(hnsns2g[1]);
  sprintf(chist,"0901/SNp2_0901_K");
  hsnk2g[1] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0901/NSNp2_0901_K");
  hnsnk2g[1] = (TH1D*) f->Get(chist)->Clone();
  hsnk2g[1]->Divide(hnsnk2g[1]);
  hsns2g[1]->GetXaxis()->SetLabelSize(0.065);
  hsns2g[1]->GetXaxis()->SetLabelOffset(0.015);
  hsns2g[1]->GetYaxis()->SetLabelSize(0.060);
  hsns2g[1]->SetLineColor(2);
  hsns2g[1]->SetLineStyle(2);
  hsns2g[1]->Draw("samehist");  
  hsnk2g[1]->SetLineColor(4);
  hsnk2g[1]->SetLineStyle(2);
  hsnk2g[1]->Draw("samehist");
  // lecture 2, position faisceau 6eme
  c10->cd(6);
  sprintf(chist,"0904/SNp2_0904_S");
  hsns2g[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNp2_0904_S");
  hnsns2g[2] = (TH1D*) f->Get(chist)->Clone();
  hsns2g[2]->Divide(hnsns2g[2]);
  sprintf(chist,"0904/SNp2_0904_K");
  hsnk2g[2] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0904/NSNp2_0904_K");
  hnsnk2g[2] = (TH1D*) f->Get(chist)->Clone();
  hsnk2g[2]->Divide(hnsnk2g[2]);
  hsns2g[2]->GetXaxis()->SetLabelSize(0.065);
  hsns2g[2]->GetXaxis()->SetLabelOffset(0.015);
  hsns2g[2]->GetYaxis()->SetLabelSize(0.060);
  hsns2g[2]->SetLineColor(2);
  hsns2g[2]->SetLineStyle(2);
  hsns2g[2]->Draw("samehist");  
  hsnk2g[2]->SetLineColor(4);
  hsnk2g[2]->SetLineStyle(2);
  hsnk2g[2]->Draw("samehist");
  // lecture 3, position faisceau 3eme
  c10->cd(3);
  sprintf(chist,"0905/SNp2_0905_S");
  hsns2g[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNp2_0905_S");
  hnsns2g[3] = (TH1D*) f->Get(chist)->Clone();
  hsns2g[3]->Divide(hnsns2g[3]);
  sprintf(chist,"0905/SNp2_0905_K");
  hsnk2g[3] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"0905/NSNp2_0905_K");
  hnsnk2g[3] = (TH1D*) f->Get(chist)->Clone();
  hsnk2g[3]->Divide(hnsnk2g[3]);
  hsns2g[3]->GetXaxis()->SetLabelSize(0.065);
  hsns2g[3]->GetXaxis()->SetLabelOffset(0.015);
  hsns2g[3]->GetYaxis()->SetLabelSize(0.060);
  hsns2g[3]->SetLineColor(2);
  hsns2g[3]->SetLineStyle(2);
  hsns2g[3]->Draw("samehist");  
  hsnk2g[3]->SetLineColor(4);
  hsnk2g[3]->SetLineStyle(2);
  hsnk2g[3]->Draw("samehist");
  // lecture 4, position faisceau 5eme
  c10->cd(5);
  sprintf(chist,"2110/SNp2_2110_S");
  hsns2g[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNp2_2110_S");
  hnsns2g[4] = (TH1D*) f->Get(chist)->Clone();
  hsns2g[4]->Divide(hnsns2g[4]);
  sprintf(chist,"2110/SNp2_2110_K");
  hsnk2g[4] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2110/NSNp2_2110_K");
  hnsnk2g[4] = (TH1D*) f->Get(chist)->Clone();
  hsnk2g[4]->Divide(hnsnk2g[4]);
  hsns2g[4]->GetXaxis()->SetLabelSize(0.065);
  hsns2g[4]->GetXaxis()->SetLabelOffset(0.015);
  hsns2g[4]->GetYaxis()->SetLabelSize(0.060);
  hsns2g[4]->SetLineColor(2);
  hsns2g[4]->SetLineStyle(2);
  hsns2g[4]->Draw("samehist");  
  hsnk2g[4]->SetLineColor(4);
  hsnk2g[4]->SetLineStyle(2);
  hsnk2g[4]->Draw("samehist");
  // lecture 5, position faisceau 1ere
  c10->cd(1);
  sprintf(chist,"2111/SNp2_2111_S");
  hsns2g[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNp2_2111_S");
  hnsns2g[5] = (TH1D*) f->Get(chist)->Clone();
  hsns2g[5]->Divide(hnsns2g[5]);
  sprintf(chist,"2111/SNp2_2111_K");
  hsnk2g[5] = (TH1D*) f->Get(chist)->Clone();
  sprintf(chist,"2111/NSNp2_2111_K");
  hnsnk2g[5] = (TH1D*) f->Get(chist)->Clone();
  hsnk2g[5]->Divide(hnsnk2g[5]);
  hsns2g[5]->GetXaxis()->SetLabelSize(0.065);
  hsns2g[5]->GetXaxis()->SetLabelOffset(0.015);
  hsns2g[5]->GetYaxis()->SetLabelSize(0.060);
  hsns2g[5]->SetLineColor(2);
  hsns2g[5]->SetLineStyle(2);
  hsns2g[5]->Draw("samehist");  
  hsnk2g[5]->SetLineColor(4);
  hsnk2g[5]->SetLineStyle(2);
  hsnk2g[5]->Draw("samehist");

  TCanvas* c11=new TCanvas("c11","profile statistiques ",0,0,650,650);
  c11->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c11->cd(i+1);
    sprintf(cpad,"c11_%d",i+1);
    TPad *ptpad3 = c11->FindObject(cpad);
    //    printf("ptpad %d\n",ptpad2);
   ptpad3->SetFillColor(10);
   ptpad3->Range(-10,-10,10,10);
   ptpad3->SetLeftMargin(0.12);
   ptpad3->SetBottomMargin(0.18);
   ptpad3->SetRightMargin(0.05);
   ptpad3->SetTopMargin(0.05);
   ptpad3->SetLogy(1);
  }

  // lecture 0, position faisceau 4eme 
  c11->cd(4);
  hnsns[0]->SetMinimum(0.5);
  hnsns[0]->GetXaxis()->SetLabelSize(0.065);
  hnsns[0]->GetXaxis()->SetLabelOffset(0.015);
  hnsns[0]->GetYaxis()->SetLabelSize(0.060);
  hnsns[0]->SetLineColor(2);
  hnsns[0]->Draw("hist");  
  hnsnk[0]->SetLineColor(4);
  hnsnk[0]->Draw("samehist");
  hnsnsg[0]->SetLineColor(2);
  hnsnsg[0]->SetLineStyle(2);
  hnsnsg[0]->Draw("samehist");  
  hnsnkg[0]->SetLineColor(4);
  hnsnkg[0]->SetLineStyle(2);
  hnsnkg[0]->Draw("samehist");
  // lecture 1, position faisceau 2eme
  c11->cd(2);
  hnsns[1]->SetMinimum(0.5);
  hnsns[1]->GetXaxis()->SetLabelSize(0.065);
  hnsns[1]->GetXaxis()->SetLabelOffset(0.015);
  hnsns[1]->GetYaxis()->SetLabelSize(0.060);
  hnsns[1]->SetLineColor(2);
  hnsns[1]->Draw("hist");  
  hnsnk[1]->SetLineColor(4);
  hnsnk[1]->Draw("samehist");
  hnsnsg[1]->SetLineColor(2);
  hnsnsg[1]->SetLineStyle(2);
  hnsnsg[1]->Draw("samehist");  
  hnsnkg[1]->SetLineColor(4);
  hnsnkg[1]->SetLineStyle(2);
  hnsnkg[1]->Draw("samehist");
  // lecture 2, position faisceau 6eme
  c11->cd(6);
  hnsns[2]->SetMinimum(0.5);
  hnsns[2]->GetXaxis()->SetLabelSize(0.065);
  hnsns[2]->GetXaxis()->SetLabelOffset(0.015);
  hnsns[2]->GetYaxis()->SetLabelSize(0.060);
  hnsns[2]->SetLineColor(2);
  hnsns[2]->Draw("hist");  
  hnsnk[2]->SetLineColor(4);
  hnsnk[2]->Draw("samehist");
  hnsnsg[2]->SetLineColor(2);
  hnsnsg[2]->SetLineStyle(2);
  hnsnsg[2]->Draw("samehist");  
  hnsnkg[2]->SetLineColor(4);
  hnsnkg[2]->SetLineStyle(2);
  hnsnkg[2]->Draw("samehist");
  // lecture 3, position faisceau 3eme
  c11->cd(3);
  hnsns[3]->SetMinimum(0.5);
  hnsns[3]->GetXaxis()->SetLabelSize(0.065);
  hnsns[3]->GetXaxis()->SetLabelOffset(0.015);
  hnsns[3]->GetYaxis()->SetLabelSize(0.060);
  hnsns[3]->SetLineColor(2);
  hnsns[3]->Draw("hist");  
  hnsnk[3]->SetLineColor(4);
  hnsnk[3]->Draw("samehist");
  hnsnsg[3]->SetLineColor(2);
  hnsnsg[3]->SetLineStyle(2);
  hnsnsg[3]->Draw("samehist");  
  hnsnkg[3]->SetLineColor(4);
  hnsnkg[3]->SetLineStyle(2);
  hnsnkg[3]->Draw("samehist");
  // lecture 4, position faisceau 5eme
  c11->cd(5);
  hnsns[4]->SetMinimum(0.5);
  hnsns[4]->GetXaxis()->SetLabelSize(0.065);
  hnsns[4]->GetXaxis()->SetLabelOffset(0.015);
  hnsns[4]->GetYaxis()->SetLabelSize(0.060);
  hnsns[4]->SetLineColor(2);
  hnsns[4]->Draw("hist");  
  hnsnk[4]->SetLineColor(4);
  hnsnk[4]->Draw("samehist");
  hnsnsg[4]->SetLineColor(2);
  hnsnsg[4]->SetLineStyle(2);
  hnsnsg[4]->Draw("samehist");  
  hnsnkg[4]->SetLineColor(4);
  hnsnkg[4]->SetLineStyle(2);
  hnsnkg[4]->Draw("samehist");
  // lecture 5, position faisceau 1ere
  c11->cd(1);
  hnsns[5]->SetMinimum(0.5);
  hnsns[5]->GetXaxis()->SetLabelSize(0.065);
  hnsns[5]->GetXaxis()->SetLabelOffset(0.015);
  hnsns[5]->GetYaxis()->SetLabelSize(0.060);
  hnsns[5]->SetLineColor(2);
  hnsns[5]->Draw("hist");  
  hnsnk[5]->SetLineColor(4);
  hnsnk[5]->Draw("samehist");
  hnsnsg[5]->SetLineColor(2);
  hnsnsg[5]->SetLineStyle(2);
  hnsnsg[5]->Draw("samehist");  
  hnsnkg[5]->SetLineColor(4);
  hnsnkg[5]->SetLineStyle(2);
  hnsnkg[5]->Draw("samehist");

  char cpad[20];
  TCanvas* c12=new TCanvas("c12","pistes grand amas",0,0,650,650);
  c12->Divide(2,3,0.0001,0.0001);
  c12_1->SetFillColor(10);
  c12_1->Range(-10,-10,10,10);
  c12_1->SetLeftMargin(0.12);
  c12_1->SetBottomMargin(0.18);
  c12_1->SetRightMargin(0.10);
  c12_1->SetTopMargin(0.05);
  c12_1->SetLogy(1);
  c12_2->SetFillColor(10);
  c12_2->Range(-10,-10,10,10);
  c12_2->SetLeftMargin(0.12);
  c12_2->SetBottomMargin(0.18);
  c12_2->SetRightMargin(0.10);
  c12_2->SetTopMargin(0.05);
  c12_2->SetLogy(1);
  c12_3->SetFillColor(10);
  c12_3->Range(-10,-10,10,10);
  c12_3->SetLeftMargin(0.12);
  c12_3->SetBottomMargin(0.18);
  c12_3->SetRightMargin(0.10);
  c12_3->SetTopMargin(0.05);
  c12_3->SetLogy(1);
  c12_4->SetFillColor(10);
  c12_4->Range(-10,-10,10,10);
  c12_4->SetLeftMargin(0.12);
  c12_4->SetBottomMargin(0.18);
  c12_4->SetRightMargin(0.10);
  c12_4->SetTopMargin(0.05);
  c12_4->SetLogy(1);
  c12_5->SetFillColor(10);
  c12_5->Range(-10,-10,10,10);
  c12_5->SetLeftMargin(0.12);
  c12_5->SetBottomMargin(0.18);
  c12_5->SetRightMargin(0.10);
  c12_5->SetTopMargin(0.05);
  c12_5->SetLogy(1);
  c12_6->SetFillColor(10);
  c12_6->Range(-10,-10,10,10);
  c12_6->SetLeftMargin(0.12);
  c12_6->SetBottomMargin(0.18);
  c12_6->SetRightMargin(0.10);
  c12_6->SetTopMargin(0.05);
  c12_6->SetLogy(1);

  // lecture 0, position faisceau 4eme position
  c12->cd(4);
  f->cd("0900");
  Pge8_0900->GetXaxis()->SetLabelSize(0.065);
  Pge8_0900->GetXaxis()->SetLabelOffset(0.015);
  Pge8_0900->GetYaxis()->SetLabelSize(0.060);
  Pge8_0900->SetMinimum(0.5);
  Pge8_0900->SetLineColor(1);
  Pge8_0900->Draw("hist");
  // lecture 1, position faisceau 2eme position
  c12->cd(2);
  f->cd("0901");
  Pge8_0901->GetXaxis()->SetLabelSize(0.065);
  Pge8_0901->GetXaxis()->SetLabelOffset(0.015);
  Pge8_0901->GetYaxis()->SetLabelSize(0.060);
  Pge8_0901->SetMaximum(500.);
  Pge8_0901->SetMinimum(0.5);
  Pge8_0901->SetLineColor(1);
  Pge8_0901->Draw("hist");
  // lecture 2, position faisceau 6eme position
  c12->cd(6);
  f->cd("0904");
  Pge8_0904->GetXaxis()->SetLabelSize(0.065);
  Pge8_0904->GetXaxis()->SetLabelOffset(0.015);
  Pge8_0904->GetYaxis()->SetLabelSize(0.060);
  Pge8_0904->SetMaximum(500.);
  Pge8_0904->SetMinimum(0.5);
  Pge8_0904->SetLineColor(1);
  Pge8_0904->Draw("hist");
  // lecture 3, position faisceau 3eme position
  c12->cd(3);
  f->cd("0905");
  Pge8_0905->GetXaxis()->SetLabelSize(0.065);
  Pge8_0905->GetXaxis()->SetLabelOffset(0.015);
  Pge8_0905->GetYaxis()->SetLabelSize(0.060);
  Pge8_0905->SetMaximum(500.);
  Pge8_0905->SetMinimum(0.5);
  Pge8_0905->SetLineColor(1);
  Pge8_0905->Draw("hist");
  // lecture 4, position faisceau 5eme position
  c12->cd(5);
  f->cd("2110");
  Pge8_2110->GetXaxis()->SetLabelSize(0.065);
  Pge8_2110->GetXaxis()->SetLabelOffset(0.015);
  Pge8_2110->GetYaxis()->SetLabelSize(0.060);
  Pge8_2110->SetMaximum(500.);
  Pge8_2110->SetMinimum(0.5);
  Pge8_2110->SetLineColor(1);
  Pge8_2110->Draw("hist");
  // lecture 5, position faisceau 1ere position
  c12->cd(1);
  f->cd("2111");
  Pge8_2111->GetXaxis()->SetLabelSize(0.065);
  Pge8_2111->GetXaxis()->SetLabelOffset(0.015);
  Pge8_2111->GetYaxis()->SetLabelSize(0.060);
  Pge8_2111->SetMaximum(500.);
  Pge8_2111->SetMinimum(0.5);
  Pge8_2111->SetLineColor(1);
  Pge8_2111->Draw("hist");

  TCanvas* c7=new TCanvas("c7","eta select xy2",0,0,600,500);
  c7->SetFillColor(10);
  c7->Range(-10,-10,10,10);
  c7->SetLeftMargin(0.15);
  c7->SetBottomMargin(0.15);
  c7->SetRightMargin(0.05);
  c7->SetTopMargin(0.05);
  // lecture 3, position dans le faisceau 3eme
  c7->cd(1);
  f->cd("0905");
  Eta_0905_K->GetXaxis()->SetLabelSize(0.055);
  Eta_0905_K->GetXaxis()->SetLabelOffset(0.015);
  Eta_0905_K->GetXaxis()->SetTitle("#eta");
  Eta_0905_K->GetXaxis()->CenterTitle();
  Eta_0905_K->GetXaxis()->SetTitleSize(0.07);
  Eta_0905_K->GetXaxis()->SetTitleOffset(0.8);
  Eta_0905_K->GetYaxis()->SetTitle("Entre#acute{e}es");
  Eta_0905_K->GetYaxis()->CenterTitle();
  Eta_0905_K->GetYaxis()->SetTitleSize(0.07);
  Eta_0905_K->GetYaxis()->SetTitleOffset(1.1);
  Eta_0905_K->GetYaxis()->SetLabelSize(0.055);
  Eta_0905_K->GetXaxis()->SetRangeUser(0.,1.);
  Eta_0905_K->SetLineColor(4);
  Eta_0905_K->Draw("hist");
  Eta_0905_S->SetLineColor(2);
  Eta_0905_S->Draw("samehist");

  return;
} 

void npiste_ang() {
 
  char ctdr[6][5] = { "2111", "0901", "0905", "0900", "2110", "0904" };
  char hist[80];
  char fich[80];
  //  sprintf(fich,"largeurs_%s.dat",run);
  sprintf(fich,"largeurs.dat");

  int const nfich0 = 2;
  int run_ang0[nfich0] = { 1234, 1251 };
  float ang0_p[nfich0];
  float ang0_n[nfich0];
  float lmoyen_tdr0[nfich0][2][6];
  float rms_lmoyen_tdr0[nfich0][2][6];
  float lmoyen_sel0[nfich0][2][6];
  float rms_lmoyen_sel0[nfich0][2][6];
  float lmoyen_sn10[nfich0][2][6];
  float rms_lmoyen_sn10[nfich0][2][6];
  float lmoyen_pos0[nfich0][2][6];
  float rms_lmoyen_pos0[nfich0][2][6];
  TH1D *h_npp_ang0[nfich0];
  TH1D *h_npn_ang0[nfich0];

  for (int f=0; f<nfich0; f++) {
    int ouvert = lire_align(run_ang0[f],2);
    if (ouvert) {
      float val = 1. - fabs(algpar[2][3]);
      ang0_p[f] = 180.*acos(val)/pi;
      val = 1. - fabs(algpar[4][3]);
      ang0_n[f] = 180.*acos(val)/pi;
      printf("run %d ang_p %f ang_n %f\n",run_ang0[f],ang0_p[f],ang0_n[f]);
    }
    sprintf(fich,"Hist_%d_a6_1_2_6_etan.root",run_ang0[f]);
    TFile *ff  = new TFile(fich);
    //    ff->ls();
    for (int i=0; i<6; i++) {
      sprintf(hist,"%s/Width2_%s_S",ctdr[i],ctdr[i]);
      TH1D *h = (TH1D*) ff->Get(hist);
      lmoyen_tdr0[f][0][i] = h->GetMean();
      rms_lmoyen_tdr0[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Width2_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_tdr0[f][1][i] = h->GetMean();
      rms_lmoyen_tdr0[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens tdr %f %f \n",run_ang0[f],hist,lmoyen_tdr0[f][0][i],lmoyen_tdr0[f][1][i]);
      sprintf(hist,"%s/Width_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_sel0[f][0][i] = h->GetMean();
      rms_lmoyen_sel0[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Width_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_sel0[f][1][i] = h->GetMean();
      rms_lmoyen_sel0[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens sel %f %f \n",run_ang0[f],hist,lmoyen_sel0[f][0][i],lmoyen_sel0[f][1][i]);
      sprintf(hist,"%s/Larg_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      if (i == 2) h_npp_ang0[f]=(TH1D*) ff->Get(hist)->Clone();
      lmoyen_sn10[f][0][i] = h->GetMean();
      rms_lmoyen_sn10[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Larg_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      if (i == 2) h_npn_ang0[f]=(TH1D*) ff->Get(hist)->Clone();
      lmoyen_sn10[f][1][i] = h->GetMean();
      rms_lmoyen_sn10[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens sn1 %f %f \n",run_ang0[f],hist,lmoyen_sn10[f][0][i],lmoyen_sn10[f][1][i]);
      sprintf(hist,"%s/NPiste_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_pos0[f][0][i] = h->GetMean();
      rms_lmoyen_pos0[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/NPiste_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_pos0[f][1][i] = h->GetMean();
      rms_lmoyen_pos0[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens pos %f %f \n",run_ang0[f],hist,lmoyen_pos0[f][0][i],lmoyen_pos0[f][1][i]);
    }      
  }

  int const nfichp = 6;
  int run_angp[nfichp] = { 1227, 1225, 1230, 1232, 1258, 1261 };
  float angp_p[nfichp];
  float angp_n[nfichp];
  int npiste_pos3_p[nfichp] = {  2, -3, -3, 3, 4, 4 };  
  float lmoyen_tdrp[nfichp][2][6];
  float rms_lmoyen_tdrp[nfichp][2][6];
  float lmoyen_selp[nfichp][2][6];
  float rms_lmoyen_selp[nfichp][2][6];
  float lmoyen_sn1p[nfichp][2][6];
  float rms_lmoyen_sn1p[nfichp][2][6];
  float lmoyen_posp[nfichp][2][6];
  float rms_lmoyen_posp[nfichp][2][6];
  TH1D *h_npp_angp[nfichp];

  for (int f=0; f<nfichp; f++) {
    int ouvert = lire_align(run_angp[f],npiste_pos3_p[f]);
    if (ouvert) {
      float val = 1. - fabs(algpar[2][3]);
      angp_p[f] = 180.*acos(val)/pi;
      val = 1. - fabs(algpar[4][3]);
      angp_n[f] = 180.*acos(val)/pi;
      printf("run %d ang_p %f ang_n %f\n",run_angp[f],angp_p[f],angp_n[f]);
    }
    if (npiste_pos3_p[f] <= 2)
       sprintf(fich,"Hist_%d_a6_1_2_6_etan.root",run_angp[f]);
    else
      sprintf(fich,"Hist_%d_a6_1_2_6_%dp_etan.root",run_angp[f],npiste_pos3_p[f]);
    printf("fichier  root %s\n",fich);
    TFile *ff  = new TFile(fich);
    //    ff->ls();
    for (int i=0; i<6; i++) {
      sprintf(hist,"%s/Width2_%s_S",ctdr[i],ctdr[i]);
      TH1D *h = (TH1D*) ff->Get(hist);
      lmoyen_tdrp[f][0][i] = h->GetMean();
      rms_lmoyen_tdrp[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Width2_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_tdrp[f][1][i] = h->GetMean();
      rms_lmoyen_tdrp[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens tdr %f %f \n",run_angp[f],hist,lmoyen_tdrp[f][0][i],lmoyen_tdrp[f][1][i]);
      sprintf(hist,"%s/Width_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_selp[f][0][i] = h->GetMean();
      rms_lmoyen_selp[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Width_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_selp[f][1][i] = h->GetMean();
      rms_lmoyen_selp[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens sel %f %f \n",run_angp[f],hist,lmoyen_selp[f][0][i],lmoyen_selp[f][1][i]);
      sprintf(hist,"%s/Larg_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      if (i == 2) h_npp_angp[f]=(TH1D*) ff->Get(hist)->Clone();
      lmoyen_sn1p[f][0][i] = h->GetMean();
      rms_lmoyen_sn1p[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Larg_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_sn1p[f][1][i] = h->GetMean();
      rms_lmoyen_sn1p[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens sn1 %f %f \n",run_angp[f],hist,lmoyen_sn1p[f][0][i],lmoyen_sn1p[f][1][i]);
      sprintf(hist,"%s/NPiste_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_posp[f][0][i] = h->GetMean();
      rms_lmoyen_posp[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/NPiste_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_posp[f][1][i] = h->GetMean();
      rms_lmoyen_posp[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens pos %f %f \n",run_angp[f],hist,lmoyen_posp[f][0][i],lmoyen_posp[f][1][i]);
    }      
  }

  int const nfichn = 4;
  int run_angn[nfichn] = { 1235, 1237, 1239, 1263 };
  float angn_p[nfichn];
  float angn_n[nfichn];
  int npiste_pos3_n[nfichn] = {  2, 2, 2, 4 };  
  float lmoyen_tdrn[nfichn][2][6];
  float rms_lmoyen_tdrn[nfichn][2][6];
  float lmoyen_seln[nfichn][2][6];
  float rms_lmoyen_seln[nfichn][2][6];
  float lmoyen_sn1n[nfichn][2][6];
  float rms_lmoyen_sn1n[nfichn][2][6];
  float lmoyen_posn[nfichn][2][6];
  float rms_lmoyen_posn[nfichn][2][6];
  TH1D *h_npn_angn[nfichn];

  for (int f=0; f<nfichn; f++) {
    int ouvert = lire_align(run_angn[f],npiste_pos3_n[f]);
    if (ouvert) {
      float val = 1. - fabs(algpar[2][3]);
      angn_p[f] = 180.*acos(val)/pi;
      val = 1. - fabs(algpar[4][3]);
      angn_n[f] = 180.*acos(val)/pi;
      printf("run %d ang_p %f ang_n %f\n",run_angn[f],angn_p[f],angn_n[f]);
    }
    if (npiste_pos3_n[f] <= 2)
       sprintf(fich,"Hist_%d_a6_1_2_6_etan.root",run_angn[f]);
    else
      sprintf(fich,"Hist_%d_a6_1_2_6_%dp_etan.root",run_angn[f],npiste_pos3_n[f]);
    printf("fichier  root %s\n",fich);
    TFile *ff  = new TFile(fich);
    //    ff->ls();
    for (int i=0; i<6; i++) {
      sprintf(hist,"%s/Width2_%s_S",ctdr[i],ctdr[i]);
      TH1D *h = (TH1D*) ff->Get(hist);
      lmoyen_tdrn[f][0][i] = h->GetMean();
      rms_lmoyen_tdrn[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Width2_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_tdrn[f][1][i] = h->GetMean();
      rms_lmoyen_tdrn[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens tdr %f %f \n",run_angn[f],hist,lmoyen_tdrn[f][0][i],lmoyen_tdrn[f][1][i]);
      sprintf(hist,"%s/Width_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_seln[f][0][i] = h->GetMean();
      rms_lmoyen_seln[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Width_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_seln[f][1][i] = h->GetMean();
      rms_lmoyen_seln[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens sel %f %f \n",run_angn[f],hist,lmoyen_seln[f][0][i],lmoyen_seln[f][1][i]);
      sprintf(hist,"%s/Larg_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_sn1n[f][0][i] = h->GetMean();
      rms_lmoyen_sn1n[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/Larg_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      if (i == 2) h_npn_angn[f]=(TH1D*) ff->Get(hist)->Clone();
      lmoyen_sn1n[f][1][i] = h->GetMean();
      rms_lmoyen_sn1n[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens sn1 %f %f \n",run_angn[f],hist,lmoyen_sn1n[f][0][i],lmoyen_sn1n[f][1][i]);
      sprintf(hist,"%s/NPiste_%s_S",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_posn[f][0][i] = h->GetMean();
      rms_lmoyen_posn[f][0][i] = h->GetRMS();
      sprintf(hist,"%s/NPiste_%s_K",ctdr[i],ctdr[i]);
      h = (TH1D*) ff->Get(hist);
      lmoyen_posn[f][1][i] = h->GetMean();
      rms_lmoyen_posn[f][1][i] = h->GetRMS();
      //      printf("run %d hist %s moyens pos %f %f \n",run_angn[f],hist,lmoyen_posn[f][0][i],lmoyen_posn[f][1][i]);
    }      
  }

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  TH1F *hlarg = new TH1F("hlarg","npiste vs angle",10,-5,60);
  hlarg->SetMaximum(6.);
  hlarg->SetMinimum(0.);
  hlarg->GetXaxis()->SetLabelOffset(0.015);
  hlarg->GetXaxis()->SetLabelSize(0.050);
  hlarg->GetYaxis()->SetLabelOffset(0.012);
  hlarg->GetYaxis()->SetLabelSize(0.050);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  hlarg->GetXaxis()->SetTitle("#theta (deg)");
  hlarg->GetXaxis()->SetTitleSize(0.060);
  hlarg->GetXaxis()->CenterTitle();
  hlarg->GetYaxis()->SetTitle("<nombre de pistes>");
  hlarg->GetYaxis()->SetTitleSize(0.060);
  hlarg->GetYaxis()->SetTitleOffset(1.10);
  hlarg->GetYaxis()->CenterTitle();

  float xpts[50],ypts[50],expts[50],eypts[50];

  int npts = 0;
  for (int i=0; i<nfich0; i++) {
    xpts[npts] = ang0_p[i]; 
    ypts[npts] = lmoyen_sn10[i][0][2];
    expts[npts] = 0.;
    eypts[npts] = rms_lmoyen_sn10[i][0][2];
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  for (int i=0; i<nfichp; i++) {
    xpts[npts] = angp_p[i]; 
    ypts[npts] = lmoyen_sn1p[i][0][2];
    expts[npts] = 0.;
    eypts[npts] = rms_lmoyen_sn1p[i][0][2];
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  TGraphErrors *angp_sn1_p = new TGraphErrors(npts,xpts,ypts,expts,eypts);
  angp_sn1_p->SetMarkerStyle(20);
  angp_sn1_p->SetMarkerSize(1.2);
  angp_sn1_p->SetMarkerColor(2); 
  angp_sn1_p->SetLineColor(2);
  angp_sn1_p->SetLineStyle(1); 

  npts = 0;
  for (int i=0; i<nfich0; i++) {
    xpts[npts] = ang0_n[i]; 
    ypts[npts] = lmoyen_sn10[i][0][2];
    expts[npts] = 0.;
    //    eypts[npts] = rms_lmoyen_sn10[i][0][2];
    eypts[npts] = 0.;
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  for (int i=0; i<nfichn; i++) {
    xpts[npts] = angn_n[i]; 
    ypts[npts] = lmoyen_sn1n[i][0][2];
    expts[npts] = 0.;
    //    eypts[npts] = rms_lmoyen_sn1n[i][0][2];
    eypts[npts] = 0.;
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  TGraphErrors *angn_sn1_p = new TGraphErrors(npts,xpts,ypts,expts,eypts);
  angn_sn1_p->SetMarkerStyle(24);
  angn_sn1_p->SetMarkerSize(1.2);
  angn_sn1_p->SetMarkerColor(2); 
  angn_sn1_p->SetLineColor(2);
  angn_sn1_p->SetLineStyle(2); 

  npts = 0;
  for (int i=0; i<nfich0; i++) {
    xpts[npts] = ang0_n[i]; 
    ypts[npts] = lmoyen_sn10[i][1][2];
    expts[npts] = 0.;
    eypts[npts] = rms_lmoyen_sn10[i][1][2];
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  for (int i=0; i<nfichn; i++) {
    xpts[npts] = angn_n[i]; 
    ypts[npts] = lmoyen_sn1n[i][1][2];
    expts[npts] = 0.;
    eypts[npts] = rms_lmoyen_sn1n[i][1][2];
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  TGraphErrors *angn_sn1_n = new TGraphErrors(npts,xpts,ypts,expts,eypts);
  angn_sn1_n->SetMarkerStyle(20);
  angn_sn1_n->SetMarkerSize(1.2);
  angn_sn1_n->SetMarkerColor(4); 
  angn_sn1_n->SetLineColor(4);
  angn_sn1_n->SetLineStyle(1); 

  npts = 0;
  for (int i=0; i<nfich0; i++) {
    xpts[npts] = ang0_p[i]; 
    ypts[npts] = lmoyen_sn10[i][1][2];
    expts[npts] = 0.;
    eypts[npts] = 0.;
    //    eypts[npts] = rms_lmoyen_sn10[i][1][2];
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  for (int i=0; i<nfichp; i++) {
    xpts[npts] = angp_p[i]; 
    ypts[npts] = lmoyen_sn1p[i][1][2];
    expts[npts] = 0.;
    eypts[npts] = 0.;
    //    eypts[npts] = rms_lmoyen_sn1p[i][1][2];
    printf("npts %d x %f y %f ey %f\n",npts,xpts[npts],ypts[npts],eypts[npts]);
    npts++;
  }
  TGraphErrors *angp_sn1_n = new TGraphErrors(npts,xpts,ypts,expts,eypts);
  angp_sn1_n->SetMarkerStyle(24);
  angp_sn1_n->SetMarkerSize(1.2);
  angp_sn1_n->SetMarkerColor(4); 
  angp_sn1_n->SetLineColor(4);
  angp_sn1_n->SetLineStyle(2); 

  TCanvas* c0=new TCanvas("c0","largeur sn1",0,0,500,500);
  c0->SetFillColor(10);
  c0->Range(-10,-10,10,10);
  c0->SetLeftMargin(0.15);
  c0->SetBottomMargin(0.15);
  c0->SetRightMargin(0.10);
  c0->SetTopMargin(0.10);
  c0->SetLogy(0);
  hlarg->Draw();
  angp_sn1_p->Draw("PLZ");
  angn_sn1_p->Draw("PLZ");
  angn_sn1_n->Draw("PLZ");
  angp_sn1_n->Draw("PLZ");

  for (int i=0; i<nfich0; i++) {
    float nent = 1./h_npp_ang0[i]->GetEntries();
    h_npp_ang0[i]->Scale(nent);
    h_npp_ang0[i]->SetMaximum(0.50);
    h_npp_ang0[i]->SetMinimum(0.);
    h_npp_ang0[i]->GetXaxis()->SetLabelOffset(0.015);
    h_npp_ang0[i]->GetXaxis()->SetLabelSize(0.050);
    h_npp_ang0[i]->GetYaxis()->SetLabelOffset(0.012);
    h_npp_ang0[i]->GetYaxis()->SetLabelSize(0.050);
    h_npp_ang0[i]->GetXaxis()->SetTitle("<nombre de pistes>");
    h_npp_ang0[i]->GetXaxis()->SetTitleSize(0.060);
    h_npp_ang0[i]->GetXaxis()->CenterTitle();
    h_npp_ang0[i]->GetXaxis()->SetRangeUser(0.,8.);
    h_npp_ang0[i]->GetYaxis()->SetTitle("fraction");
    h_npp_ang0[i]->GetYaxis()->SetTitleSize(0.060);
    h_npp_ang0[i]->GetYaxis()->SetTitleOffset(1.10);
    h_npp_ang0[i]->GetYaxis()->CenterTitle();
    h_npp_ang0[i]->SetFillColor(0);
    float nent = 1./h_npn_ang0[i]->GetEntries();
    h_npn_ang0[i]->Scale(nent);
    h_npn_ang0[i]->SetMaximum(0.55);
    h_npn_ang0[i]->SetMinimum(0.);
    h_npn_ang0[i]->GetXaxis()->SetLabelOffset(0.015);
    h_npn_ang0[i]->GetXaxis()->SetLabelSize(0.050);
    h_npn_ang0[i]->GetYaxis()->SetLabelOffset(0.012);
    h_npn_ang0[i]->GetYaxis()->SetLabelSize(0.050);
    h_npn_ang0[i]->GetXaxis()->SetTitle("<nombre de pistes>");
    h_npn_ang0[i]->GetXaxis()->SetTitleSize(0.060);
    h_npn_ang0[i]->GetXaxis()->CenterTitle();
    h_npn_ang0[i]->GetXaxis()->SetRangeUser(0.,8.);
    h_npn_ang0[i]->GetYaxis()->SetTitle("fraction");
    h_npn_ang0[i]->GetYaxis()->SetTitleSize(0.060);
    h_npn_ang0[i]->GetYaxis()->SetTitleOffset(1.10);
    h_npn_ang0[i]->GetYaxis()->CenterTitle();
  }

  for (int i=0; i<nfichp; i++) {
    float nent = 1./h_npp_angp[i]->GetEntries();
    h_npp_angp[i]->Scale(nent);
  }

  for (int i=0; i<nfichn; i++) {
    float nent = 1./h_npn_angn[i]->GetEntries();
    h_npn_angn[i]->Scale(nent);
  }
  
  TCanvas* c1=new TCanvas("c1","largeur angle cote p",0,0,500,500);
  c1->SetFillColor(10);
  c1->Range(-10,-10,10,10);
  c1->SetLeftMargin(0.15);
  c1->SetBottomMargin(0.15);
  c1->SetRightMargin(0.10);
  c1->SetTopMargin(0.10);
  c1->SetLogy(0);
  h_npp_ang0[0]->Draw("hist");
  for (int i=0; i<nfichp; i++) {
    switch(i) {
    case 0: h_npp_angp[i]->SetFillColor(0);  break;
    case 1: h_npp_angp[i]->SetFillColor(17);  break; 
    case 2: h_npp_angp[i]->SetFillColor(16);  break;
    case 3: h_npp_angp[i]->SetFillColor(15);  break;
    case 4: h_npp_angp[i]->SetFillColor(14);  break;
    case 5: h_npp_angp[i]->SetFillColor(1);  break;
    default:
    }
    h_npp_angp[i]->Draw("samehist");
  }
  
  TCanvas* c2=new TCanvas("c2","largeur angle cote n",0,0,500,500);
  c2->SetFillColor(10);
  c2->Range(-10,-10,10,10);
  c2->SetLeftMargin(0.15);
  c2->SetBottomMargin(0.15);
  c2->SetRightMargin(0.10);
  c2->SetTopMargin(0.10);
  c2->SetLogy(0);
  h_npn_ang0[0]->Draw("hist");
  for (int i=0; i<nfichn; i++) {
    switch(i) {
    case 0: h_npn_angn[i]->SetFillColor(0);  break;
    case 1: h_npn_angn[i]->SetFillColor(17);  break; 
    case 2: h_npn_angn[i]->SetFillColor(16);  break;
    case 3: h_npn_angn[i]->SetFillColor(15);  break;
    case 4: h_npn_angn[i]->SetFillColor(14);  break;
    case 5: h_npn_angn[i]->SetFillColor(1);  break;
    default:
    }
    h_npn_angn[i]->Draw("samehist");
  }

  return;
}

void etafn(int run) {

  char cfich[50]; 
  sprintf(cfich,"Hist_%d.root",run);
  //  sprintf(cfich,"Hist_%d_a6_1_2_6.root",run);

  TFile *f = new TFile(cfich);
  printf("s\n",cfich);

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  char chist[30];

  TH1D *eta_p[6],*feta_p[6];
  TH1D *eta_n[6],*feta_n[6];

  eta_p[0] = (TH1D*) f->Get("0900/Eta_0900_S")->Clone();
  eta_p[1] = (TH1D*) f->Get("0901/Eta_0901_S")->Clone();
  eta_p[2] = (TH1D*) f->Get("0904/Eta_0904_S")->Clone();
  eta_p[3] = (TH1D*) f->Get("0905/Eta_0905_S")->Clone();
  eta_p[4] = (TH1D*) f->Get("2110/Eta_2110_S")->Clone();
  eta_p[5] = (TH1D*) f->Get("2111/Eta_2111_S")->Clone();
  sprintf(chist,"feta_p0");
  feta_p[0] = (TH1D*) f->Get("0900/Eta_0900_S")->Clone(chist);
  sprintf(chist,"feta_p1");
  feta_p[1] = (TH1D*) f->Get("0901/Eta_0901_S")->Clone(chist);
  sprintf(chist,"feta_p2");
  feta_p[2] = (TH1D*) f->Get("0904/Eta_0904_S")->Clone(chist);
  sprintf(chist,"feta_p3");
  feta_p[3] = (TH1D*) f->Get("0905/Eta_0905_S")->Clone(chist);
  sprintf(chist,"feta_p4");
  feta_p[4] = (TH1D*) f->Get("2110/Eta_2110_S")->Clone(chist);
  sprintf(chist,"feta_p5");
  feta_p[5] = (TH1D*) f->Get("2111/Eta_2111_S")->Clone(chist);

  for (int i=0; i<6; i++) {
    eta_p[i]->GetXaxis()->SetLabelSize(0.065);
    eta_p[i]->GetXaxis()->SetLabelOffset(0.015);
    eta_p[i]->GetYaxis()->SetLabelSize(0.060);
    eta_p[i]->GetXaxis()->SetTitle("#eta");
    eta_p[i]->GetXaxis()->CenterTitle();
    eta_p[i]->GetXaxis()->SetTitleSize(0.10);
    eta_p[i]->GetXaxis()->SetTitleOffset(0.8);
    eta_p[i]->GetXaxis()->SetRangeUser(0.,500.);
    eta_p[i]->GetXaxis()->SetRangeUser(0.,1.);
    eta_p[i]->SetLineColor(2);
    float nent = 1./eta_p[i]->GetEntries();
    eta_p[i]->Scale(nent);
    int nbins = eta_p[i]->GetNbinsX();
    printf("nbinsx %d\n",nbins);
    float val = 0.;
    feta_p[i]->Reset("");
    for (int ib=0; ib<nbins+1; ib++) {
      val += eta_p[i]->GetBinContent(ib);
      printf("ib %d centre %f contenu %lf val %f\n",ib,eta_p[i]->GetBinCenter(ib),eta_p[i]->GetBinContent(ib),val);
      feta_p[i]->SetBinContent(ib,val);
    }
    feta_p[i]->GetXaxis()->SetLabelSize(0.065);
    feta_p[i]->GetXaxis()->SetLabelOffset(0.015);
    feta_p[i]->GetYaxis()->SetLabelSize(0.060);
    feta_p[i]->GetXaxis()->SetTitle("#eta");
    feta_p[i]->GetXaxis()->CenterTitle();
    feta_p[i]->GetXaxis()->SetTitleSize(0.10);
    feta_p[i]->GetXaxis()->SetTitleOffset(0.8);
    feta_p[i]->GetXaxis()->SetRangeUser(0.,500.);
    feta_p[i]->GetXaxis()->SetRangeUser(0.,1.);
    feta_p[i]->SetLineColor(2);
  }

  eta_n[0] = (TH1D*) f->Get("0900/Eta_0900_K")->Clone();
  eta_n[1] = (TH1D*) f->Get("0901/Eta_0901_K")->Clone();
  eta_n[2] = (TH1D*) f->Get("0904/Eta_0904_K")->Clone();
  eta_n[3] = (TH1D*) f->Get("0905/Eta_0905_K")->Clone();
  eta_n[4] = (TH1D*) f->Get("2110/Eta_2110_K")->Clone();
  eta_n[5] = (TH1D*) f->Get("2111/Eta_2111_K")->Clone();
  sprintf(chist,"feta_n0");
  feta_n[0] = (TH1D*) f->Get("0900/Eta_0900_K")->Clone(chist);
  sprintf(chist,"feta_n1");
  feta_n[1] = (TH1D*) f->Get("0901/Eta_0901_K")->Clone(chist);
  sprintf(chist,"feta_n2");
  feta_n[2] = (TH1D*) f->Get("0904/Eta_0904_K")->Clone(chist);
  sprintf(chist,"feta_n3");
  feta_n[3] = (TH1D*) f->Get("0905/Eta_0905_K")->Clone(chist);
  sprintf(chist,"feta_n4");
  feta_n[4] = (TH1D*) f->Get("2110/Eta_2110_K")->Clone(chist);
  sprintf(chist,"feta_n5");
  feta_n[5] = (TH1D*) f->Get("2111/Eta_2111_K")->Clone(chist);

  for (int i=0; i<6; i++) {
    eta_n[i]->GetXaxis()->SetLabelSize(0.065);
    eta_n[i]->GetXaxis()->SetLabelOffset(0.015);
    eta_n[i]->GetYaxis()->SetLabelSize(0.060);
    eta_n[i]->GetXaxis()->SetTitle("#eta");
    eta_n[i]->GetXaxis()->CenterTitle();
    eta_n[i]->GetXaxis()->SetTitleSize(0.10);
    eta_n[i]->GetXaxis()->SetTitleOffset(0.8);
    eta_n[i]->GetXaxis()->SetRangeUser(0.,500.);
    eta_n[i]->GetXaxis()->SetRangeUser(0.,1.);
    eta_n[i]->SetLineColor(4);
    float nent = 1./eta_n[i]->GetEntries();
    printf("i %d nent %f\n",i,nent);
    eta_n[i]->Scale(nent);
    int nbins = eta_n[i]->GetNbinsX();
    printf("nbinsx %d\n",nbins);
    float val = 0.;
    feta_n[i]->Reset("");
    for (int ib=0; ib<nbins+1; ib++) {
      val += eta_n[i]->GetBinContent(ib);
      printf("ib %d centre %f contenu %f val %f\n",ib,eta_n[i]->GetBinCenter(ib),eta_n[i]->GetBinContent(ib),val);
      feta_n[i]->SetBinContent(ib,val);
    }
    feta_n[i]->GetXaxis()->SetLabelSize(0.065);
    feta_n[i]->GetXaxis()->SetLabelOffset(0.015);
    feta_n[i]->GetYaxis()->SetLabelSize(0.060);
    feta_n[i]->GetXaxis()->SetTitle("#eta");
    feta_n[i]->GetXaxis()->CenterTitle();
    feta_n[i]->GetXaxis()->SetTitleSize(0.10);
    feta_n[i]->GetXaxis()->SetTitleOffset(0.8);
    feta_n[i]->GetXaxis()->SetRangeUser(0.,500.);
    feta_n[i]->GetXaxis()->SetRangeUser(0.,1.);
    feta_n[i]->SetLineColor(4);
  }

  TCanvas* c0=new TCanvas("c0","eta select",0,0,650,650);
  c0->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
     c0->cd(i+1);
     gPad->SetFillColor(10);
     gPad->SetFillStyle(4000);
     gPad->Range(-10,-10,10,10);
     gPad->SetLeftMargin(0.12);
     gPad->SetBottomMargin(0.18);
     gPad->SetRightMargin(0.05);
     gPad->SetTopMargin(0.05);
     gPad->SetLogy(0);
  }

  // lecture 0, position dans le faisceau 4eme
  c0->cd(4);
  eta_n[0]->Draw("hist");
  eta_p[0]->Draw("samehist");
  tpos3 = new TLatex(0.675,0.85,"position 3");
  tpos3->SetNDC();
  tpos3->SetTextSize(0.080);
  tpos3->Draw();
  // lecture 1, position dans le faisceau 2eme
  c0->cd(2);
  eta_n[1]->Draw("hist");
  eta_p[1]->Draw("samehist");
  tpos1 = new TLatex(0.675,0.85,"position 1");
  tpos1->SetNDC();
  tpos1->SetTextSize(0.080);
  tpos1->Draw();
  // lecture 2, position dans le faisceau 6eme
  c0->cd(6);
  eta_n[2]->Draw("hist");
  eta_p[2]->Draw("samehist");
  tpos5 = new TLatex(0.675,0.85,"position 5");
  tpos5->SetNDC();
  tpos5->SetTextSize(0.080);
  tpos5->Draw();
  // lecture 3, position dans le faisceau 3eme
  c0->cd(3);
  eta_n[3]->Draw("hist");
  eta_p[3]->Draw("samehist");
  tpos2 = new TLatex(0.675,0.85,"position 2");
  tpos2->SetNDC();
  tpos2->SetTextSize(0.080);
  tpos2->Draw();
  tangle = new TLatex(0.760,0.76,"20^{o}");
  tangle->SetNDC();
  tangle->SetTextSize(0.080);
  tangle->Draw();
  // lecture 4, position dans le faisceau 5eme
  c0->cd(5);
  eta_n[4]->Draw("hist");
  eta_p[4]->Draw("samehist");
  tpos4 = new TLatex(0.675,0.85,"position 4");
  tpos4->SetNDC();
  tpos4->SetTextSize(0.080);
  tpos4->Draw();
  // lecture 5, position dans le faisceau 1ere
  c0->cd(1);
  eta_n[5]->Draw("hist");
  eta_p[5]->Draw("samehist");
  tpos0 = new TLatex(0.675,0.85,"position 0");
  tpos0->SetNDC();
  tpos0->SetTextSize(0.080);
  tpos0->Draw();

  TCanvas* c1=new TCanvas("c1","feta select",0,0,650,650);
  c1->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
     c1->cd(i+1);
     gPad->SetFillColor(10);
     gPad->SetFillStyle(4000);
     gPad->Range(-10,-10,10,10);
     gPad->SetLeftMargin(0.12);
     gPad->SetBottomMargin(0.18);
     gPad->SetRightMargin(0.05);
     gPad->SetTopMargin(0.05);
     gPad->SetLogy(0);
  }

  // lecture 0, position dans le faisceau 4eme
  c1->cd(4);
  feta_n[0]->Draw("hist");
  feta_p[0]->Draw("samehist");
  // lecture 1, position dans le faisceau 2eme
  c1->cd(2);
  feta_n[1]->Draw("hist");
  feta_p[1]->Draw("samehist");
  // lecture 2, position dans le faisceau 6eme
  c1->cd(6);
  feta_n[2]->Draw("hist");
  feta_p[2]->Draw("samehist");
  // lecture 3, position dans le faisceau 3eme
  c1->cd(3);
  feta_n[3]->Draw("hist");
  feta_p[3]->Draw("samehist");
  // lecture 4, position dans le faisceau 5eme
  c1->cd(5);
  feta_n[4]->Draw("hist");
  feta_p[4]->Draw("samehist");
  // lecture 5, position dans le faisceau 1ere
  c1->cd(1);
  feta_n[5]->Draw("hist");
  feta_p[5]->Draw("samehist");

  sprintf(cfich,"feta_%d.root",run);
  TFile *fhist = new TFile(cfich,"RECREATE");
  if (fhist->IsOpen()) printf("%s ouvert\n",cfich);
  for (int i=0; i<6; i++) {
    feta_p[i]->Write();
    feta_n[i]->Write();
  }
  fhist->Close();

  return;
}

void fneta3(int run) {

  char cfich[50];
  sprintf(cfich,"Hist_%d_a6_1_2_6_3p_etan.root",run);
 
  TFile *f = new TFile(cfich);
  printf("s\n",cfich);

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  char chist[30];

  TH1D *eta3_p[6],*feta3_p[6];
  TH1D *eta3_n[6],*feta3_n[6];

  eta3_p[0] = (TH1D*) f->Get("0900/Eta3_0900_S")->Clone();
  eta3_p[1] = (TH1D*) f->Get("0901/Eta3_0901_S")->Clone();
  eta3_p[2] = (TH1D*) f->Get("0904/Eta3_0904_S")->Clone();
  eta3_p[3] = (TH1D*) f->Get("0905/Eta3_0905_S")->Clone();
  eta3_p[4] = (TH1D*) f->Get("2110/Eta3_2110_S")->Clone();
  eta3_p[5] = (TH1D*) f->Get("2111/Eta3_2111_S")->Clone();
  for (int i=0; i<6; i++) eta3_p[i]->SetLineColor(2);
  sprintf(chist,"feta3_p0");
  feta3_p[0] = (TH1D*) f->Get("0900/Eta3_0900_S")->Clone(chist);
  sprintf(chist,"feta3_p1");
  feta3_p[1] = (TH1D*) f->Get("0901/Eta3_0901_S")->Clone(chist);
  sprintf(chist,"feta3_p2");
  feta3_p[2] = (TH1D*) f->Get("0904/Eta3_0904_S")->Clone(chist);
  sprintf(chist,"feta3_p3");
  feta3_p[3] = (TH1D*) f->Get("0905/Eta3_0905_S")->Clone(chist);
  sprintf(chist,"feta3_p4");
  feta3_p[4] = (TH1D*) f->Get("2110/Eta3_2110_S")->Clone(chist);
  sprintf(chist,"feta3_p5");
  feta3_p[5] = (TH1D*) f->Get("2111/Eta3_2111_S")->Clone(chist);

  eta3_n[0] = (TH1D*) f->Get("0900/Eta3_0900_K")->Clone();
  eta3_n[1] = (TH1D*) f->Get("0901/Eta3_0901_K")->Clone();
  eta3_n[2] = (TH1D*) f->Get("0904/Eta3_0904_K")->Clone();
  eta3_n[3] = (TH1D*) f->Get("0905/Eta3_0905_K")->Clone();
  eta3_n[4] = (TH1D*) f->Get("2110/Eta3_2110_K")->Clone();
  eta3_n[5] = (TH1D*) f->Get("2111/Eta3_2111_K")->Clone();
  for (int i=0; i<6; i++) eta3_n[i]->SetLineColor(4);
  sprintf(chist,"feta3_n0");
  feta3_n[0] = (TH1D*) f->Get("0900/Eta3_0900_K")->Clone(chist);
  sprintf(chist,"feta3_n1");
  feta3_n[1] = (TH1D*) f->Get("0901/Eta3_0901_K")->Clone(chist);
  sprintf(chist,"feta3_n2");
  feta3_n[2] = (TH1D*) f->Get("0904/Eta3_0904_K")->Clone(chist);
  sprintf(chist,"feta3_n3");
  feta3_n[3] = (TH1D*) f->Get("0905/Eta3_0905_K")->Clone(chist);
  float nent = 1./(eta3_n[3]->GetEntries()-eta3_n[3]->GetBinContent(0));
  printf("i %d nent %f\n",i,nent);
  eta3_n[3]->Scale(nent);
  int nbins = eta3_n[3]->GetNbinsX();
  printf("nbinsx %d\n",nbins);
  float val = 0.;
  feta3_n[3]->Reset("");
  for (int ib=1; ib<nbins+1; ib++) {
    val += eta3_n[3]->GetBinContent(ib);
    printf("ib %d centre %f contenu %f val %f\n",ib,eta3_n[3]->GetBinCenter(ib),eta3_n[3]->GetBinContent(ib),val);
    feta3_n[3]->SetBinContent(ib,val);
    }
  sprintf(chist,"feta3_n4");
  feta3_n[4] = (TH1D*) f->Get("2110/Eta3_2110_K")->Clone(chist);
  sprintf(chist,"feta3_n5");
  feta3_n[5] = (TH1D*) f->Get("2111/Eta3_2111_K")->Clone(chist);

  TCanvas* c0=new TCanvas("c0","eta3 select",0,0,650,650);
  c0->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
     c0->cd(i+1);
     gPad->SetFillColor(10);
     gPad->SetFillStyle(4000);
     gPad->Range(-10,-10,10,10);
     gPad->SetLeftMargin(0.12);
     gPad->SetBottomMargin(0.18);
     gPad->SetRightMargin(0.05);
     gPad->SetTopMargin(0.05);
     gPad->SetLogy(0);
  }

  // lecture 0, position dans le faisceau 4eme
  c0->cd(4);
  eta3_n[0]->Draw("hist");
  eta3_p[0]->Draw("samehist");
  tpos3 = new TLatex(0.675,0.85,"position 3");
  tpos3->SetNDC();
  tpos3->SetTextSize(0.080);
  tpos3->Draw();
  // lecture 1, position dans le faisceau 2eme
  c0->cd(2);
  eta3_n[1]->Draw("hist");
  eta3_p[1]->Draw("samehist");
  tpos1 = new TLatex(0.675,0.85,"position 1");
  tpos1->SetNDC();
  tpos1->SetTextSize(0.080);
  tpos1->Draw();
  // lecture 2, position dans le faisceau 6eme
  c0->cd(6);
  eta3_n[2]->Draw("hist");
  eta3_p[2]->Draw("samehist");
  tpos5 = new TLatex(0.675,0.85,"position 5");
  tpos5->SetNDC();
  tpos5->SetTextSize(0.080);
  tpos5->Draw();
  // lecture 3, position dans le faisceau 3eme
  c0->cd(3);
  eta3_n[3]->Draw("hist");
  eta3_p[3]->Draw("samehist");
  tpos2 = new TLatex(0.675,0.85,"position 2");
  tpos2->SetNDC();
  tpos2->SetTextSize(0.080);
  tpos2->Draw();
  tangle = new TLatex(0.760,0.76,"20^{o}");
  tangle->SetNDC();
  tangle->SetTextSize(0.080);
  tangle->Draw();
  // lecture 4, position dans le faisceau 5eme
  c0->cd(5);
  eta3_n[4]->Draw("hist");
  eta3_p[4]->Draw("samehist");
  tpos4 = new TLatex(0.675,0.85,"position 4");
  tpos4->SetNDC();
  tpos4->SetTextSize(0.080);
  tpos4->Draw();
  // lecture 5, position dans le faisceau 1ere
  c0->cd(1);
  eta3_n[5]->Draw("hist");
  eta3_p[5]->Draw("samehist");
  tpos0 = new TLatex(0.675,0.85,"position 0");
  tpos0->SetNDC();
  tpos0->SetTextSize(0.080);
  tpos0->Draw();

  TCanvas* c2=new TCanvas("c2","feta3 select pos2",0,0,500,500);
  c2->SetFillColor(10);
  c2->SetFillStyle(4000);
  c2->Range(-10,-10,10,10);
  c2->SetLeftMargin(0.12);
  c2->SetBottomMargin(0.18);
  c2->SetRightMargin(0.05);
  c2->SetTopMargin(0.05);
  c2->SetLogy(0);

  c2->cd();
  feta3_n[3]->Draw("hist");

  sprintf(cfich,"feta3_%d.root",run);
  TFile *fhist = new TFile(cfich,"RECREATE");
  if (fhist->IsOpen()) {
   printf("%s ouvert\n",cfich);
   feta3_n[3]->Write();
  }
  fhist->Close();

  return;
}

void amult() {

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  TCanvas* c5=new TCanvas("c10","occupation",0,0,650,650);
  c10->Divide(2,3,0.0001,0.0001);
  c10_1->SetFillColor(10);
  c10_1->Range(-10,-10,10,10);
  c10_1->SetLeftMargin(0.12);
  c10_1->SetBottomMargin(0.18);
  c10_1->SetRightMargin(0.05);
  c10_1->SetTopMargin(0.05);
  c10_2->SetFillColor(10);
  c10_2->Range(-10,-10,10,10);
  c10_2->SetLeftMargin(0.12);
  c10_2->SetBottomMargin(0.18);
  c10_2->SetRightMargin(0.05);
  c10_2->SetTopMargin(0.05);
  c10_3->SetFillColor(10);
  c10_3->Range(-10,-10,10,10);
  c10_3->SetLeftMargin(0.12);
  c10_3->SetBottomMargin(0.18);
  c10_3->SetRightMargin(0.05);
  c10_3->SetTopMargin(0.05);
  c10_4->SetFillColor(10);
  c10_4->Range(-10,-10,10,10);
  c10_4->SetLeftMargin(0.12);
  c10_4->SetBottomMargin(0.18);
  c10_4->SetRightMargin(0.05);
  c10_4->SetTopMargin(0.05);
  c10_5->SetFillColor(10);
  c10_5->Range(-10,-10,10,10);
  c10_5->SetLeftMargin(0.12);
  c10_5->SetBottomMargin(0.18);
  c10_5->SetRightMargin(0.05);
  c10_5->SetTopMargin(0.05);
  c10_6->SetFillColor(10);
  c10_6->Range(-10,-10,10,10);
  c10_6->SetLeftMargin(0.12);
  c10_6->SetBottomMargin(0.18);
  c10_6->SetRightMargin(0.05);
  c10_6->SetTopMargin(0.05);

  // lecture 0, position faisceau 3eme AMS02
  c10->cd(3);
  f->cd("0900");
  Multiplicity_0900_S->GetXaxis()->SetLabelSize(0.065);
  Multiplicity_0900_S->GetXaxis()->SetLabelOffset(0.015);
  Multiplicity_0900_S->GetYaxis()->SetLabelSize(0.060);
  Multiplicity_0900_S->SetLineColor(4);
  Multiplicity_0900_S->Draw("hist");  
  Multiplicity_0900_S->GetXaxis()->SetRangeUser(0.,10.);
  Multiplicity_0900_K->SetLineColor(2);
  Multiplicity_0900_K->Draw("samehist");  
  // lecture 1, position faisceau 1ere AMS02
  c10->cd(1);
  f->cd("0901");
  Multiplicity_0901_S->GetXaxis()->SetLabelSize(0.065);
  Multiplicity_0901_S->GetXaxis()->SetLabelOffset(0.015);
  Multiplicity_0901_S->GetYaxis()->SetLabelSize(0.060);
  Multiplicity_0901_S->SetLineColor(4);
  Multiplicity_0901_S->GetXaxis()->SetRangeUser(0.,10.);
  Multiplicity_0901_S->Draw("hist");
  Multiplicity_0901_K->SetLineColor(2);
  Multiplicity_0901_K->Draw("samehist");  
  // lecture 2, position faisceau 4eme AMS02
  c10->cd(4);
  f->cd("0904");
  Multiplicity_0904_S->GetXaxis()->SetLabelSize(0.065);
  Multiplicity_0904_S->GetXaxis()->SetLabelOffset(0.015);
  Multiplicity_0904_S->GetYaxis()->SetLabelSize(0.060);
  Multiplicity_0904_S->SetLineColor(4);
  Multiplicity_0904_S->GetXaxis()->SetRangeUser(0.,10.);
  Multiplicity_0904_S->Draw("hist");
  Multiplicity_0904_K->SetLineColor(2);
  Multiplicity_0904_K->Draw("samehist");  
  // lecture 3, position faisceau 2eme AMS02
  c10->cd(2);
  f->cd("0905");
  Multiplicity_0905_S->GetXaxis()->SetLabelSize(0.065);
  Multiplicity_0905_S->GetXaxis()->SetLabelOffset(0.015);
  Multiplicity_0905_S->GetYaxis()->SetLabelSize(0.060);
  Multiplicity_0905_S->SetLineColor(4);
  Multiplicity_0905_S->GetXaxis()->SetRangeUser(0.,10.);
  Multiplicity_0905_S->Draw("hist");
  Multiplicity_0905_K->SetLineColor(2);
  Multiplicity_0905_K->Draw("samehist");  
  c10->cd(5);
  f->cd("2110");
  Multiplicity_2110_S->GetXaxis()->SetLabelSize(0.065);
  Multiplicity_2110_S->GetXaxis()->SetLabelOffset(0.015);
  Multiplicity_2110_S->GetYaxis()->SetLabelSize(0.060);
  Multiplicity_2110_S->SetLineColor(4);
  Multiplicity_2110_S->GetXaxis()->SetRangeUser(0.,10.);
  Multiplicity_2110_S->Draw("hist");
  Multiplicity_2110_K->SetLineColor(2);
  Multiplicity_2110_K->Draw("samehist");  
  c10->cd(6);
  f->cd("2111");
  Multiplicity_2111_S->GetXaxis()->SetLabelSize(0.065);
  Multiplicity_2111_S->GetXaxis()->SetLabelOffset(0.015);
  Multiplicity_2111_S->GetYaxis()->SetLabelSize(0.060);
  Multiplicity_2111_S->SetLineColor(4);
  Multiplicity_2111_S->GetXaxis()->SetRangeUser(0.,10.);
  Multiplicity_2111_S->Draw("hist");
  Multiplicity_2111_K->SetLineColor(2);
  Multiplicity_2111_K->Draw("samehist");  

  return;
} 

void accept() {

  int ibin;
  float val;
  float tots = 0.;
  float totk = 0.;
  float totsk = 0.;
  TH2D* ptmult = (TH2D*)f->Get("Amult");
  ibin =ptmult->FindBin(0.,4.);
  val = ptmult->GetBinContent(ibin);
  totk+=val;
  printf("0 s 4 k val %f tot %f\n",val,totk); 
  ibin =ptmult->FindBin(1.,4.);
  val = ptmult->GetBinContent(ibin);
  totk+=val;
  printf("1 s 4 k val %f tot %f\n",val,totk); 
  ibin =ptmult->FindBin(2.,4.);
  val = ptmult->GetBinContent(ibin);
  totk+=val;
  printf("2 s 4 k val %f tot %f\n",val,totk); 
  ibin =ptmult->FindBin(3.,4.);
  val = ptmult->GetBinContent(ibin);
  totk+=val;
  printf("3 s 4 k val %f tot %f\n",val,totk); 
  ibin =ptmult->FindBin(5.,4.);
  val = ptmult->GetBinContent(ibin);
  totk+=val;
  printf("5 s 4 k val %f tot %f\n",val,totk); 
  ibin =ptmult->FindBin(6.,4.);
  val = ptmult->GetBinContent(ibin);
  totk+=val;
  printf("6 s 4 k val %f totk %f\n",val,totk); 
  ibin =ptmult->FindBin(4.,0.);
  val = ptmult->GetBinContent(ibin);
  tots+=val;
  printf("4 s 0 k val %f tot %f\n",val,tots); 
  ibin =ptmult->FindBin(4.,1.);
  val = ptmult->GetBinContent(ibin);
  tots+=val;
  printf("4 s 1 k val %f tot %f\n",val,tots); 
  ibin =ptmult->FindBin(4.,2.);
  val = ptmult->GetBinContent(ibin);
  tots+=val;
  printf("4 s 2 k val %f tot %f\n",val,tots); 
  ibin =ptmult->FindBin(4.,3.);
  val = ptmult->GetBinContent(ibin);
  tots+=val;
  printf("4 s 3 k val %f tot %f\n",val,tots); 
  ibin =ptmult->FindBin(4.,5.);
  val = ptmult->GetBinContent(ibin);
  tots+=val;
  printf("4 s 5 k val %f tot %f\n",val,tots); 
  ibin =ptmult->FindBin(4.,6.);
  val = ptmult->GetBinContent(ibin);
  tots+=val;
  printf("4 s 6 k val %f tots %f\n",val,tots); 
  ibin =ptmult->FindBin(4.,4.);
  val = ptmult->GetBinContent(ibin);
  totsk+=val;
  printf("4 s 4 k val %f totsk %f\n",val,totsk); 
  TH1D* ptaccept = (TH1D*)f->Get("Accept");
  ibin =ptaccept->FindBin(0.);
  val = ptaccept->GetBinContent(ibin);
  printf("accept 0 tot %f\n",val); 
  ibin =ptaccept->FindBin(1.);
  val = ptaccept->GetBinContent(ibin);
  printf("accept 1 tot %f tots %f\n",val,tots); 
  ibin =ptaccept->FindBin(2.);
  val = ptaccept->GetBinContent(ibin);
  printf("accept 2 tot %f totk %f\n",val,totk); 
  ibin =ptaccept->FindBin(3.);
  val = ptaccept->GetBinContent(ibin);
  printf("accept 3 tot %f totsk %f\n",val,totsk); 

}

void ang_ref() {

  char ligne[100];
  FILE *ft = fopen("../align/algpar.dat","r");
  printf("ft %d\n",ft);
  float algpar[11][6];
  for (int i=0; i<9; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i][0],&algpar[i][1],
    &algpar[i][2],&algpar[i][3],&algpar[i][4],&algpar[i][5]);
   printf("%f %f %f %f %f %f\n",algpar[i][0],algpar[i][1],
    algpar[i][2],algpar[i][3],algpar[i][4],algpar[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i+1][0],&algpar[i+1][1],
    &algpar[i+1][2],&algpar[i+1][3],&algpar[i+1][4],&algpar[i+1][5]);
   printf("%f %f %f %f %f %f\n",algpar[i+1][0],algpar[i+1][1],
    algpar[i+1][2],algpar[i+1][3],algpar[i+1][4],algpar[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[10][0],&algpar[10][1],&algpar[10][2],&algpar[10][3],
   &algpar[10][4],&algpar[10][5]);
  fclose(ft);

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","dxy ref",0,0,650,650);

  float pente1 = algpar[6][1];
  float inter1 = algpar[7][1];
  printf("pente1 %f inter1 %f\n",pente1,inter1);
  float xpt1[2] = {-1.5, 1.5};
  float ypt1[2];
  for (int i=0; i<2; i++)
    ypt1[i] = pente1*xpt1[i] + inter1;
  gfit1 = new TGraph(2,xpt1,ypt1);

  c0->cd();
  f->cd();
  dxy_ref->GetXaxis()->SetLabelSize(0.045);
  dxy_ref->GetXaxis()->SetLabelOffset(0.015);
  dxy_ref->GetYaxis()->SetLabelSize(0.050);
  dxy_ref->SetLineColor(2);
  dxy_ref->Draw("box");
  gfit1->Draw();

  TCanvas* c1=new TCanvas("c1","dyx ref",0,0,650,650);

  float pente2 = algpar[8][1];
  float inter2 = algpar[9][1];
  printf("pente2 %f inter2 %f\n",pente2,inter2);
  float xpt2[2] = {-1.5, 1.5};
  float ypt2[2];
  for (int i=0; i<2; i++)
    ypt2[i] = pente2*xpt2[i] + inter2;
  gfit2 = new TGraph(2,xpt2,ypt2);

  c1->cd();
  f->cd();
  dyx_ref->GetXaxis()->SetLabelSize(0.045);
  dyx_ref->GetXaxis()->SetLabelOffset(0.015);
  dyx_ref->GetYaxis()->SetLabelSize(0.050);
  dyx_ref->SetLineColor(4);
  dyx_ref->Draw("box");
  gfit2->Draw();

  return;
}

void ang_dxx_dyy() {

  char ligne[100];
  lire_align();

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  // pos2 lec3 dxx dyy
  float pente1 = algpar[2][3];
  float inter1 = algpar[3][3];
  float pente2 = algpar[4][3];
  float inter2 = algpar[5][3];
  // pos3 lec0 dxx dyy
  float pente3 = algpar[2][0];
  float inter3 = algpar[3][0];
  float pente4 = algpar[4][0];
  float inter4 = algpar[5][0];
  // pos0 lec5 dxx dyy
  float pente5 = algpar[2][5];
  float inter5 = algpar[3][5];
  float pente6 = algpar[4][5];
  float inter6 = algpar[5][5];
  // pos4 lec4 dxx dyy
  float pente7 = algpar[2][4];
  float inter7 = algpar[3][4];
  float pente8 = algpar[4][4];
  float inter8 = algpar[5][4];

  TCanvas* c0=new TCanvas("c0","dxx pos2 lec3",0,0,650,650);

  float xpt1[2] = {-1.5, 1.5};
  float ypt1[2];
  for (int i=0; i<2; i++)
    ypt1[i] = pente1*xpt1[i] + inter1;
  gfit1 = new TGraph(2,xpt1,ypt1);

  c0->cd();
  f->cd();
  dxx_pos3->GetXaxis()->SetLabelSize(0.045);
  dxx_pos3->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos3->GetYaxis()->SetLabelSize(0.050);
  dxx_pos3->SetLineColor(2);
  dxx_pos3->Draw("box");
  gfit1->Draw();

  TCanvas* c1=new TCanvas("c1","dyy pos2 lec3",0,0,650,650);

  float xpt2[2] = {-1.5, 1.5};
  float ypt2[2];
  for (int i=0; i<2; i++)
    ypt2[i] = pente2*xpt2[i] + inter2;
  gfit2 = new TGraph(2,xpt2,ypt2);

  c1->cd();
  f->cd();
  dyy_pos3->GetXaxis()->SetLabelSize(0.045);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos3->GetYaxis()->SetLabelSize(0.050);
  dyy_pos3->SetLineColor(4);
  dyy_pos3->Draw("box");
  gfit2->Draw();

  TCanvas* c2=new TCanvas("c2","dxx pos3 lec0",0,0,650,650);

  float xpt3[2] = {-1.5, 1.5};
  float ypt3[2];
  for (int i=0; i<2; i++)
    ypt3[i] = pente3*xpt3[i] + inter3;
  gfit3 = new TGraph(2,xpt3,ypt3);

  c2->cd();
  f->cd();
  dxx_pos4->GetXaxis()->SetLabelSize(0.045);
  dxx_pos4->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos4->GetYaxis()->SetLabelSize(0.050);
  dxx_pos4->SetLineColor(2);
  dxx_pos4->Draw("box");
  gfit3->Draw();

  TCanvas* c3=new TCanvas("c3","dyy pos3 lec0",0,0,650,650);

  float xpt4[2] = {-1.5, 1.5};
  float ypt4[2];
  for (int i=0; i<2; i++)
    ypt4[i] = pente4*xpt4[i] + inter4;
  gfit4 = new TGraph(2,xpt4,ypt4);

  c3->cd();
  f->cd();
  dyy_pos4->GetXaxis()->SetLabelSize(0.045);
  dyy_pos4->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos4->GetYaxis()->SetLabelSize(0.050);
  dyy_pos4->SetLineColor(4);
  dyy_pos4->Draw("box");
  gfit4->Draw();

  TCanvas* c4=new TCanvas("c4","dxx pos0 lec5",0,0,650,650);

  float xpt5[2] = {-1.5, 1.5};
  float ypt5[2];
  for (int i=0; i<2; i++)
    ypt5[i] = pente5*xpt5[i] + inter5;
  gfit5 = new TGraph(2,xpt5,ypt5);

  c4->cd();
  f->cd();
  dxx_pos1p->GetXaxis()->SetLabelSize(0.045);
  dxx_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos1p->GetYaxis()->SetLabelSize(0.050);
  dxx_pos1p->SetLineColor(2);
  dxx_pos1p->Draw("box");
  gfit5->Draw();

  TCanvas* c5=new TCanvas("c5","dyy pos0 lec5",0,0,650,650);

  float xpt6[2] = {-1.5, 1.5};
  float ypt6[2];
  for (int i=0; i<2; i++)
    ypt6[i] = pente6*xpt6[i] + inter6;
  gfit6 = new TGraph(2,xpt6,ypt6);
  printf("pente6 %f inter6 %f\n",pente6,inter6);

  c5->cd();
  f->cd();
  dyy_pos1p->GetXaxis()->SetLabelSize(0.045);
  dyy_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos1p->GetYaxis()->SetLabelSize(0.050);
  dyy_pos1p->SetLineColor(4);
  dyy_pos1p->Draw("box");
  gfit6->Draw();

  TCanvas* c6=new TCanvas("c6","dxx pos4 lec4",0,0,650,650);

  float xpt7[2] = {-1.5, 1.5};
  float ypt7[2];
  for (int i=0; i<2; i++)
    ypt7[i] = pente7*xpt7[i] + inter7;
  gfit7 = new TGraph(2,xpt7,ypt7);

  c6->cd();
  f->cd();
  dxx_pos2p->GetXaxis()->SetLabelSize(0.045);
  dxx_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos2p->GetYaxis()->SetLabelSize(0.050);
  dxx_pos2p->SetLineColor(2);
  dxx_pos2p->Draw("box");
  gfit7->Draw();

  TCanvas* c7=new TCanvas("c7","dyy pos4 lec4",0,0,650,650);

  float xpt8[2] = {-1.5, 1.5};
  float ypt8[2];
  for (int i=0; i<2; i++)
    ypt8[i] = pente8*xpt8[i] + inter8;
  gfit8 = new TGraph(2,xpt8,ypt8);

  c7->cd();
  f->cd();
  dyy_pos2p->GetXaxis()->SetLabelSize(0.045);
  dyy_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos2p->GetYaxis()->SetLabelSize(0.050);
  dyy_pos2p->SetLineColor(4);
  dyy_pos2p->Draw("box");
  gfit8->Draw();

  TCanvas* c8=new TCanvas("c8","dyy pos2 lec3",0,0,550,550);
  c8->SetFillColor(10);
  c8->Range(-10,-10,10,10);
  c8->SetLeftMargin(0.15);
  c8->SetBottomMargin(0.15);
  c8->SetRightMargin(0.10);
  c8->SetTopMargin(0.10);

  c8->cd();
  f->cd();
  dyy_pos3->GetXaxis()->SetLabelSize(0.045);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos3->GetYaxis()->SetLabelSize(0.050);
  dyy_pos3->SetLineColor(4);
  dyy_pos3->GetXaxis()->SetTitle("Y (cm)");
  dyy_pos3->GetXaxis()->CenterTitle();
  dyy_pos3->GetXaxis()->SetTitleOffset(1.20);
  dyy_pos3->GetXaxis()->SetTitleSize(0.05);
  dyy_pos3->GetXaxis()->SetLabelSize(0.0425);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.010);
  dyy_pos3->GetYaxis()->SetTitle("Y - Y_{track} (cm)");
  dyy_pos3->GetYaxis()->CenterTitle();
  dyy_pos3->GetYaxis()->SetTitleOffset(1.40);
  dyy_pos3->GetYaxis()->SetTitleSize(0.05);
  dyy_pos3->GetYaxis()->SetLabelSize(0.040);
  dyy_pos3->GetYaxis()->SetLabelOffset(0.010);
  dyy_pos3->Draw("box");
  gfit2->Draw();

  ty2 = new TLatex(0.30,0.80,"Y2");
  ty2->SetNDC();
  ty2->SetTextSize(0.045);
  ty2->SetTextColor(4);
  ty2->Draw();
  t20d = new TLatex(0.305,0.75.0,"20^{o}");
  t20d->SetNDC();
  t20d->SetTextSize(0.04);
  t20d->SetTextColor(4);
  t20d->Draw();
  talpha = new TLatex(0.68,0.50,"#alpha = 0.0302");
  talpha->SetNDC();
  talpha->SetTextSize(0.04);
  talpha->Draw();
  talphaa = new TLatex(0.70,0.45,"( 14.1 ^{o} )");
  talphaa->SetNDC();
  talphaa->SetTextSize(0.04);
  talphaa->Draw();

  return;
}

void ang_dxx_dyy_1237() {

  TFile *f1 = new TFile("Hist_1237_a3_1_2_6_etan_027.root");
  TFile *f2 = new TFile("Hist_1237_a3_1_2_6_etan_027_2plaq.root");

  char ligne[100];
  lire_align_1237();

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  // pos2 lec3 dxx dyy
  float pente1 = algpar[2][3];
  float inter1 = algpar[3][3];
  float pente2 = algpar[4][3];
  float inter2 = algpar[5][3];
  float pente2_2 = algpar_2[4][3];
  float inter2_2 = algpar_2[5][3];
  // pos3 lec0 dxx dyy
  float pente3 = algpar[2][0];
  float inter3 = algpar[3][0];
  float pente4 = algpar[4][0];
  float inter4 = algpar[5][0];
  // pos0 lec5 dxx dyy
  float pente5 = algpar[2][5];
  float inter5 = algpar[3][5];
  float pente6 = algpar[4][5];
  float inter6 = algpar[5][5];
  // pos4 lec4 dxx dyy
  float pente7 = algpar[2][4];
  float inter7 = algpar[3][4];
  float pente8 = algpar[4][4];
  float inter8 = algpar[5][4];

  TCanvas* c0=new TCanvas("c0","dxx pos2 lec3",0,0,650,650);

  float xpt1[2] = {-1.5, 1.5};
  float ypt1[2];
  for (int i=0; i<2; i++)
    ypt1[i] = pente1*xpt1[i] + inter1;
  gfit1 = new TGraph(2,xpt1,ypt1);

  c0->cd();
  f->cd();
  dxx_pos3->GetXaxis()->SetLabelSize(0.045);
  dxx_pos3->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos3->GetYaxis()->SetLabelSize(0.050);
  dxx_pos3->SetLineColor(2);
  dxx_pos3->Draw("box");
  gfit1->Draw();

  TCanvas* c1=new TCanvas("c1","dyy pos2 lec3",0,0,650,650);

  float xpt2[2] = {0.0, 1.5};
  float ypt2[2];
  for (int i=0; i<2; i++)
    ypt2[i] = pente2*xpt2[i] + inter2;
  gfit2 = new TGraph(2,xpt2,ypt2);

  float xpt2[2] = {-1.5, 0.0};
  float ypt2[2];
  for (int i=0; i<2; i++)
    ypt2[i] = pente2_2*xpt2[i] + inter2_2;
  gfit2_2 = new TGraph(2,xpt2,ypt2);

  c1->cd();
  f->cd();
  dyy_pos3->GetXaxis()->SetLabelSize(0.045);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos3->GetYaxis()->SetLabelSize(0.050);
  dyy_pos3->SetLineColor(4);
  dyy_pos3->Draw("box");
  gfit2->Draw();
  gfit2_2->Draw();

  TCanvas* c2=new TCanvas("c2","dxx pos3 lec0",0,0,650,650);

  float xpt3[2] = {-1.5, 1.5};
  float ypt3[2];
  for (int i=0; i<2; i++)
    ypt3[i] = pente3*xpt3[i] + inter3;
  gfit3 = new TGraph(2,xpt3,ypt3);

  c2->cd();
  f->cd();
  dxx_pos4->GetXaxis()->SetLabelSize(0.045);
  dxx_pos4->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos4->GetYaxis()->SetLabelSize(0.050);
  dxx_pos4->SetLineColor(2);
  dxx_pos4->Draw("box");
  gfit3->Draw();

  TCanvas* c3=new TCanvas("c3","dyy pos3 lec0",0,0,650,650);

  float xpt4[2] = {-1.5, 1.5};
  float ypt4[2];
  for (int i=0; i<2; i++)
    ypt4[i] = pente4*xpt4[i] + inter4;
  gfit4 = new TGraph(2,xpt4,ypt4);

  c3->cd();
  f->cd();
  dyy_pos4->GetXaxis()->SetLabelSize(0.045);
  dyy_pos4->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos4->GetYaxis()->SetLabelSize(0.050);
  dyy_pos4->SetLineColor(4);
  dyy_pos4->Draw("box");
  gfit4->Draw();

  TCanvas* c4=new TCanvas("c4","dxx pos0 lec5",0,0,650,650);

  float xpt5[2] = {-1.5, 1.5};
  float ypt5[2];
  for (int i=0; i<2; i++)
    ypt5[i] = pente5*xpt5[i] + inter5;
  gfit5 = new TGraph(2,xpt5,ypt5);

  c4->cd();
  f->cd();
  dxx_pos1p->GetXaxis()->SetLabelSize(0.045);
  dxx_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos1p->GetYaxis()->SetLabelSize(0.050);
  dxx_pos1p->SetLineColor(2);
  dxx_pos1p->Draw("box");
  gfit5->Draw();

  TCanvas* c5=new TCanvas("c5","dyy pos0 lec5",0,0,650,650);

  float xpt6[2] = {-1.5, 1.5};
  float ypt6[2];
  for (int i=0; i<2; i++)
    ypt6[i] = pente6*xpt6[i] + inter6;
  gfit6 = new TGraph(2,xpt6,ypt6);
  printf("pente6 %f inter6 %f\n",pente6,inter6);

  c5->cd();
  f->cd();
  dyy_pos1p->GetXaxis()->SetLabelSize(0.045);
  dyy_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos1p->GetYaxis()->SetLabelSize(0.050);
  dyy_pos1p->SetLineColor(4);
  dyy_pos1p->Draw("box");
  gfit6->Draw();

  TCanvas* c6=new TCanvas("c6","dxx pos4 lec4",0,0,650,650);

  float xpt7[2] = {-1.5, 1.5};
  float ypt7[2];
  for (int i=0; i<2; i++)
    ypt7[i] = pente7*xpt7[i] + inter7;
  gfit7 = new TGraph(2,xpt7,ypt7);

  c6->cd();
  f->cd();
  dxx_pos2p->GetXaxis()->SetLabelSize(0.045);
  dxx_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos2p->GetYaxis()->SetLabelSize(0.050);
  dxx_pos2p->SetLineColor(2);
  dxx_pos2p->Draw("box");
  gfit7->Draw();

  TCanvas* c7=new TCanvas("c7","dyy pos4 lec4",0,0,650,650);

  float xpt8[2] = {-1.5, 1.5};
  float ypt8[2];
  for (int i=0; i<2; i++)
    ypt8[i] = pente8*xpt8[i] + inter8;
  gfit8 = new TGraph(2,xpt8,ypt8);

  c7->cd();
  f->cd();
  dyy_pos2p->GetXaxis()->SetLabelSize(0.045);
  dyy_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos2p->GetYaxis()->SetLabelSize(0.050);
  dyy_pos2p->SetLineColor(4);
  dyy_pos2p->Draw("box");
  gfit8->Draw();

  TCanvas* c8=new TCanvas("c8","dyy pos2 lec3",0,0,550,550);
  c8->SetFillColor(10);
  c8->Range(-10,-10,10,10);
  c8->SetLeftMargin(0.15);
  c8->SetBottomMargin(0.15);
  c8->SetRightMargin(0.10);
  c8->SetTopMargin(0.10);

  c8->cd();
  f->cd();
  dyy_pos3->GetXaxis()->SetLabelSize(0.045);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos3->GetYaxis()->SetLabelSize(0.050);
  dyy_pos3->SetLineColor(4);
  dyy_pos3->GetXaxis()->SetTitle("Y (cm)");
  dyy_pos3->GetXaxis()->CenterTitle();
  dyy_pos3->GetXaxis()->SetTitleOffset(1.20);
  dyy_pos3->GetXaxis()->SetTitleSize(0.05);
  dyy_pos3->GetXaxis()->SetLabelSize(0.0425);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.010);
  dyy_pos3->GetYaxis()->SetTitle("Y - Y_{track} (cm)");
  dyy_pos3->GetYaxis()->CenterTitle();
  dyy_pos3->GetYaxis()->SetTitleOffset(1.40);
  dyy_pos3->GetYaxis()->SetTitleSize(0.05);
  dyy_pos3->GetYaxis()->SetLabelSize(0.040);
  dyy_pos3->GetYaxis()->SetLabelOffset(0.010);
  dyy_pos3->Draw("box");
  gfit2->Draw();
  gfit2_2->Draw();

  ty2 = new TLatex(0.30,0.80,"Y2");
  ty2->SetNDC();
  ty2->SetTextSize(0.045);
  ty2->SetTextColor(4);
  ty2->Draw();
  t20d = new TLatex(0.305,0.75.0,"20^{o}");
  t20d->SetNDC();
  t20d->SetTextSize(0.04);
  t20d->SetTextColor(4);
  t20d->Draw();
  talpha1 = new TLatex(0.22,0.25,"#alpha_{1} = 0.5022");
  talpha1->SetNDC();
  talpha1->SetTextSize(0.04);
  talpha1->Draw();
  talphaa1 = new TLatex(0.24,0.20,"( 18.2 ^{o} )");
  talphaa1->SetNDC();
  talphaa1->SetTextSize(0.04);
  talphaa1->Draw();
  talpha2 = new TLatex(0.68,0.75,"#alpha_{2} = 0.4848");
  talpha2->SetNDC();
  talpha2->SetTextSize(0.04);
  talpha2->Draw();
  talphaa2 = new TLatex(0.70,0.70,"( 17.9 ^{o} )");
  talphaa2->SetNDC();
  talphaa2->SetTextSize(0.04);
  talphaa2->Draw();

  return;
}

void ang_dxx_dyy_1237_bis() {

  TFile *f = new TFile("../align/Hist_1237_a3_1_2_6_etan_027_2plaq.root");

  char ligne[100];
  lire_align_1237(1237);

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  // pos2 lec3 dxx dyy
  float pente1 = algpar[2][3];
  float inter1 = algpar[3][3];
  float pente2 = algpar[4][3];
  float inter2 = algpar[5][3];
  float pente2_2 = algpar_2[4][3];
  float inter2_2 = algpar_2[5][3];
  // pos3 lec0 dxx dyy
  float pente3 = algpar[2][0];
  float inter3 = algpar[3][0];
  float pente4 = algpar[4][0];
  float inter4 = algpar[5][0];
  // pos0 lec5 dxx dyy
  float pente5 = algpar[2][5];
  float inter5 = algpar[3][5];
  float pente6 = algpar[4][5];
  float inter6 = algpar[5][5];
  // pos4 lec4 dxx dyy
  float pente7 = algpar[2][4];
  float inter7 = algpar[3][4];
  float pente8 = algpar[4][4];
  float inter8 = algpar[5][4];

  TCanvas* c0=new TCanvas("c0","dxx pos2 lec3",0,0,650,650);

  float xpt1[2] = {-1.5, 1.5};
  float ypt1[2];
  for (int i=0; i<2; i++)
    ypt1[i] = pente1*xpt1[i] + inter1;
  gfit1 = new TGraph(2,xpt1,ypt1);

  c0->cd();
  f->cd();
  dxx_pos3->GetXaxis()->SetLabelSize(0.045);
  dxx_pos3->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos3->GetYaxis()->SetLabelSize(0.050);
  dxx_pos3->SetLineColor(2);
  dxx_pos3->Draw("box");
  gfit1->Draw();

  TCanvas* c1=new TCanvas("c1","dyy pos2 lec3",0,0,650,650);

  float xpt2[2] = {0.0, 1.5};
  float ypt2[2];
  for (int i=0; i<2; i++)
    ypt2[i] = pente2*xpt2[i] + inter2;
  gfit2 = new TGraph(2,xpt2,ypt2);

  float xpt2[2] = {-1.5, 0.0};
  float ypt2[2];
  for (int i=0; i<2; i++)
    ypt2[i] = pente2_2*xpt2[i] + inter2_2;
  gfit2_2 = new TGraph(2,xpt2,ypt2);

  c1->cd();
  f->cd();
  dyy_pos3->GetXaxis()->SetLabelSize(0.045);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos3->GetYaxis()->SetLabelSize(0.050);
  dyy_pos3->SetLineColor(4);
  dyy_pos3->Draw("box");
  gfit2->Draw();
  gfit2_2->Draw();

  TCanvas* c2=new TCanvas("c2","dxx pos3 lec0",0,0,650,650);

  float xpt3[2] = {-1.5, 1.5};
  float ypt3[2];
  for (int i=0; i<2; i++)
    ypt3[i] = pente3*xpt3[i] + inter3;
  gfit3 = new TGraph(2,xpt3,ypt3);

  c2->cd();
  f->cd();
  dxx_pos4->GetXaxis()->SetLabelSize(0.045);
  dxx_pos4->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos4->GetYaxis()->SetLabelSize(0.050);
  dxx_pos4->SetLineColor(2);
  dxx_pos4->Draw("box");
  gfit3->Draw();

  TCanvas* c3=new TCanvas("c3","dyy pos3 lec0",0,0,650,650);

  float xpt4[2] = {-1.5, 1.5};
  float ypt4[2];
  for (int i=0; i<2; i++)
    ypt4[i] = pente4*xpt4[i] + inter4;
  gfit4 = new TGraph(2,xpt4,ypt4);

  c3->cd();
  f->cd();
  dyy_pos4->GetXaxis()->SetLabelSize(0.045);
  dyy_pos4->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos4->GetYaxis()->SetLabelSize(0.050);
  dyy_pos4->SetLineColor(4);
  dyy_pos4->Draw("box");
  gfit4->Draw();

  TCanvas* c4=new TCanvas("c4","dxx pos0 lec5",0,0,650,650);

  float xpt5[2] = {-1.5, 1.5};
  float ypt5[2];
  for (int i=0; i<2; i++)
    ypt5[i] = pente5*xpt5[i] + inter5;
  gfit5 = new TGraph(2,xpt5,ypt5);

  c4->cd();
  f->cd();
  dxx_pos1p->GetXaxis()->SetLabelSize(0.045);
  dxx_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos1p->GetYaxis()->SetLabelSize(0.050);
  dxx_pos1p->SetLineColor(2);
  dxx_pos1p->Draw("box");
  gfit5->Draw();

  TCanvas* c5=new TCanvas("c5","dyy pos0 lec5",0,0,650,650);

  float xpt6[2] = {-1.5, 1.5};
  float ypt6[2];
  for (int i=0; i<2; i++)
    ypt6[i] = pente6*xpt6[i] + inter6;
  gfit6 = new TGraph(2,xpt6,ypt6);
  printf("pente6 %f inter6 %f\n",pente6,inter6);

  c5->cd();
  f->cd();
  dyy_pos1p->GetXaxis()->SetLabelSize(0.045);
  dyy_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos1p->GetYaxis()->SetLabelSize(0.050);
  dyy_pos1p->SetLineColor(4);
  dyy_pos1p->Draw("box");
  gfit6->Draw();

  TCanvas* c6=new TCanvas("c6","dxx pos4 lec4",0,0,650,650);

  float xpt7[2] = {-1.5, 1.5};
  float ypt7[2];
  for (int i=0; i<2; i++)
    ypt7[i] = pente7*xpt7[i] + inter7;
  gfit7 = new TGraph(2,xpt7,ypt7);

  c6->cd();
  f->cd();
  dxx_pos2p->GetXaxis()->SetLabelSize(0.045);
  dxx_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dxx_pos2p->GetYaxis()->SetLabelSize(0.050);
  dxx_pos2p->SetLineColor(2);
  dxx_pos2p->Draw("box");
  gfit7->Draw();

  TCanvas* c7=new TCanvas("c7","dyy pos4 lec4",0,0,650,650);

  float xpt8[2] = {-1.5, 1.5};
  float ypt8[2];
  for (int i=0; i<2; i++)
    ypt8[i] = pente8*xpt8[i] + inter8;
  gfit8 = new TGraph(2,xpt8,ypt8);

  c7->cd();
  f->cd();
  dyy_pos2p->GetXaxis()->SetLabelSize(0.045);
  dyy_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos2p->GetYaxis()->SetLabelSize(0.050);
  dyy_pos2p->SetLineColor(4);
  dyy_pos2p->Draw("box");
  gfit8->Draw();

  TCanvas* c8=new TCanvas("c8","dyy pos2 lec3",0,0,550,550);
  c8->SetFillColor(10);
  c8->Range(-10,-10,10,10);
  c8->SetLeftMargin(0.15);
  c8->SetBottomMargin(0.15);
  c8->SetRightMargin(0.10);
  c8->SetTopMargin(0.10);

  c8->cd();
  f->cd();
  dyy_pos3->GetXaxis()->SetLabelSize(0.045);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.015);
  dyy_pos3->GetYaxis()->SetLabelSize(0.050);
  dyy_pos3->SetLineColor(4);
  dyy_pos3->GetXaxis()->SetTitle("Y (cm)");
  dyy_pos3->GetXaxis()->CenterTitle();
  dyy_pos3->GetXaxis()->SetTitleOffset(1.20);
  dyy_pos3->GetXaxis()->SetTitleSize(0.05);
  dyy_pos3->GetXaxis()->SetLabelSize(0.0425);
  dyy_pos3->GetXaxis()->SetLabelOffset(0.010);
  dyy_pos3->GetYaxis()->SetTitle("Y - Y_{track} (cm)");
  dyy_pos3->GetYaxis()->CenterTitle();
  dyy_pos3->GetYaxis()->SetTitleOffset(1.40);
  dyy_pos3->GetYaxis()->SetTitleSize(0.05);
  dyy_pos3->GetYaxis()->SetLabelSize(0.040);
  dyy_pos3->GetYaxis()->SetLabelOffset(0.010);
  dyy_pos3->Draw("box");
  gfit2->Draw();
  gfit2_2->Draw();

  ty2 = new TLatex(0.30,0.80,"Y2");
  ty2->SetNDC();
  ty2->SetTextSize(0.045);
  ty2->SetTextColor(4);
  ty2->Draw();
  t20d = new TLatex(0.305,0.75.0,"20^{o}");
  t20d->SetNDC();
  t20d->SetTextSize(0.04);
  t20d->SetTextColor(4);
  t20d->Draw();
  talpha1 = new TLatex(0.22,0.25,"#alpha_{1} = 0.5022");
  talpha1->SetNDC();
  talpha1->SetTextSize(0.04);
  talpha1->Draw();
  talphaa1 = new TLatex(0.24,0.20,"( 18.2 ^{o} )");
  talphaa1->SetNDC();
  talphaa1->SetTextSize(0.04);
  talphaa1->Draw();
  talpha2 = new TLatex(0.68,0.75,"#alpha_{2} = 0.4848");
  talpha2->SetNDC();
  talpha2->SetTextSize(0.04);
  talpha2->Draw();
  talphaa2 = new TLatex(0.70,0.70,"( 17.9 ^{o} )");
  talphaa2->SetNDC();
  talphaa2->SetTextSize(0.04);
  talphaa2->Draw();

  return;
}

void ang_dxy_dyx() {

  char ligne[100];
  FILE *ft = fopen("../align/algpar.dat","r");
  printf("ft %d\n",ft);
  float algpar[11][6];
  for (int i=0; i<9; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i][0],&algpar[i][1],
    &algpar[i][2],&algpar[i][3],&algpar[i][4],&algpar[i][5]);
   printf("%f %f %f %f %f %f\n",algpar[i][0],algpar[i][1],
    algpar[i][2],algpar[i][3],algpar[i][4],algpar[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i+1][0],&algpar[i+1][1],
    &algpar[i+1][2],&algpar[i+1][3],&algpar[i+1][4],&algpar[i+1][5]);
   printf("%f %f %f %f %f %f\n",algpar[i+1][0],algpar[i+1][1],
    algpar[i+1][2],algpar[i+1][3],algpar[i+1][4],algpar[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[10][0],&algpar[10][1],&algpar[10][2],&algpar[10][3],
   &algpar[10][4],&algpar[10][5]);
  fclose(ft);

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  // pos2 lec3 dxy dyx
  float pente1 = algpar[6][3];
  float inter1 = algpar[7][3];
  float pente2 = algpar[8][3];
  float inter2 = algpar[9][3];
  // pos3 lec0 dxy dyx
  float pente3 = algpar[6][0];
  float inter3 = algpar[7][0];
  float pente4 = algpar[8][0];
  float inter4 = algpar[9][0];
  // pos0 lec5 dxy dyx
  float pente5 = algpar[6][5];
  float inter5 = algpar[7][5];
  float pente6 = algpar[8][5];
  float inter6 = algpar[9][5];
  // pos4 lec4 dxy dyx
  float pente7 = algpar[6][4];
  float inter7 = algpar[7][4];
  float pente8 = algpar[8][4];
  float inter8 = algpar[9][4];

  TCanvas* c0=new TCanvas("c0","dxy pos2 lec3",0,0,650,650);

  float xpt1[2] = {-1.5, 1.5};
  float ypt1[2];
  for (int i=0; i<2; i++)
    ypt1[i] = pente1*xpt1[i] + inter1;
  gfit1 = new TGraph(2,xpt1,ypt1);

  c0->cd();
  f->cd();
  dxy_pos3->GetXaxis()->SetLabelSize(0.045);
  dxy_pos3->GetXaxis()->SetLabelOffset(0.015);
  dxy_pos3->GetYaxis()->SetLabelSize(0.050);
  dxy_pos3->SetLineColor(2);
  dxy_pos3->Draw("box");
  gfit1->Draw();

  TCanvas* c1=new TCanvas("c1","dyx pos2 lec3",0,0,650,650);

  float xpt2[2] = {-1.5, 1.5};
  float ypt2[2];
  for (int i=0; i<2; i++)
    ypt2[i] = pente2*xpt2[i] + inter2;
  gfit2 = new TGraph(2,xpt2,ypt2);

  c1->cd();
  f->cd();
  dyx_pos3->GetXaxis()->SetLabelSize(0.045);
  dyx_pos3->GetXaxis()->SetLabelOffset(0.015);
  dyx_pos3->GetYaxis()->SetLabelSize(0.050);
  dyx_pos3->SetLineColor(4);
  dyx_pos3->Draw("box");
  gfit2->Draw();

  TCanvas* c2=new TCanvas("c2","dxy pos3 lec0",0,0,650,650);

  float xpt3[2] = {-1.5, 1.5};
  float ypt3[2];
  for (int i=0; i<2; i++)
    ypt3[i] = pente3*xpt3[i] + inter3;
  gfit3 = new TGraph(2,xpt3,ypt3);

  c2->cd();
  f->cd();
  dxy_pos4->GetXaxis()->SetLabelSize(0.045);
  dxy_pos4->GetXaxis()->SetLabelOffset(0.015);
  dxy_pos4->GetYaxis()->SetLabelSize(0.050);
  dxy_pos4->SetLineColor(2);
  dxy_pos4->Draw("box");
  gfit3->Draw();

  TCanvas* c3=new TCanvas("c3","dyx pos3 lec0",0,0,650,650);

  float xpt4[2] = {-1.5, 1.5};
  float ypt4[2];
  for (int i=0; i<2; i++)
    ypt4[i] = pente4*xpt4[i] + inter4;
  gfit4 = new TGraph(2,xpt4,ypt4);

  c3->cd();
  f->cd();
  dyx_pos4->GetXaxis()->SetLabelSize(0.045);
  dyx_pos4->GetXaxis()->SetLabelOffset(0.015);
  dyx_pos4->GetYaxis()->SetLabelSize(0.050);
  dyx_pos4->SetLineColor(4);
  dyx_pos4->Draw("box");
  gfit4->Draw();

  TCanvas* c4=new TCanvas("c4","dxy pos0 lec5",0,0,650,650);

  float xpt5[2] = {-1.5, 1.5};
  float ypt5[2];
  for (int i=0; i<2; i++)
    ypt5[i] = pente5*xpt5[i] + inter5;
  gfit5 = new TGraph(2,xpt5,ypt5);

  c4->cd();
  f->cd();
  dxy_pos1p->GetXaxis()->SetLabelSize(0.045);
  dxy_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dxy_pos1p->GetYaxis()->SetLabelSize(0.050);
  dxy_pos1p->SetLineColor(2);
  dxy_pos1p->Draw("box");
  gfit5->Draw();

  TCanvas* c5=new TCanvas("c5","dyx pos0 lec5",0,0,650,650);

  float xpt6[2] = {-1.5, 1.5};
  float ypt6[2];
  for (int i=0; i<2; i++)
    ypt6[i] = pente6*xpt6[i] + inter6;
  gfit6 = new TGraph(2,xpt6,ypt6);

  c5->cd();
  f->cd();
  dyx_pos1p->GetXaxis()->SetLabelSize(0.045);
  dyx_pos1p->GetXaxis()->SetLabelOffset(0.015);
  dyx_pos1p->GetYaxis()->SetLabelSize(0.050);
  dyx_pos1p->SetLineColor(4);
  dyx_pos1p->Draw("box");
  gfit6->Draw();

  TCanvas* c6=new TCanvas("c6","dxy pos4 lec4",0,0,650,650);

  float xpt7[2] = {-1.5, 1.5};
  float ypt7[2];
  for (int i=0; i<2; i++)
    ypt7[i] = pente7*xpt7[i] + inter7;
  gfit7 = new TGraph(2,xpt7,ypt7);

  c6->cd();
  f->cd();
  dxy_pos2p->GetXaxis()->SetLabelSize(0.045);
  dxy_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dxy_pos2p->GetYaxis()->SetLabelSize(0.050);
  dxy_pos2p->SetLineColor(2);
  dxy_pos2p->Draw("box");
  gfit7->Draw();

  TCanvas* c7=new TCanvas("c7","dyx pos4 lec4",0,0,650,650);

  float xpt8[2] = {-1.5, 1.5};
  float ypt8[2];
  for (int i=0; i<2; i++)
    ypt8[i] = pente8*xpt8[i] + inter8;
  gfit8 = new TGraph(2,xpt8,ypt8);

  c7->cd();
  f->cd();
  dyx_pos2p->GetXaxis()->SetLabelSize(0.045);
  dyx_pos2p->GetXaxis()->SetLabelOffset(0.015);
  dyx_pos2p->GetYaxis()->SetLabelSize(0.050);
  dyx_pos2p->SetLineColor(4);
  dyx_pos2p->Draw("box");
  gfit8->Draw();

  return;
}

void xypos(int run, int npiste)
{

  float xy[6][2];
  float xy_rms[6][2];

  char cfich[80];
  if (npiste <= 2)
  sprintf(cfich,"Hist_%d_a6_1_2_6_etan.root",run);
  else
  sprintf(cfich,"Hist_%d_a6_1_2_6_%dp_etan.root",run,npiste);
  printf("cfich %s\n",cfich);

  TFile *f = new TFile(cfich);

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  TCanvas* c0=new TCanvas("c0","xpos",0,0,650,650);
  c0->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 1510 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  c0->Divide(2,3,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.12);
  c0_1->SetBottomMargin(0.18);
  c0_1->SetRightMargin(0.05);
  c0_1->SetTopMargin(0.05);
  c0_1->SetLogy(1);
  c0_2->SetFillColor(10);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.12);
  c0_2->SetBottomMargin(0.18);
  c0_2->SetRightMargin(0.05);
  c0_2->SetTopMargin(0.05);
  c0_2->SetLogy(1);
  c0_3->SetFillColor(10);
  c0_3->Range(-10,-10,10,10);
  c0_3->SetLeftMargin(0.12);
  c0_3->SetBottomMargin(0.18);
  c0_3->SetRightMargin(0.05);
  c0_3->SetTopMargin(0.05);
  c0_3->SetLogy(1);
  c0_4->SetFillColor(10);
  c0_4->Range(-10,-10,10,10);
  c0_4->SetLeftMargin(0.12);
  c0_4->SetBottomMargin(0.18);
  c0_4->SetRightMargin(0.05);
  c0_4->SetTopMargin(0.05);
  c0_4->SetLogy(1);
  c0_5->SetFillColor(10);
  c0_5->Range(-10,-10,10,10);
  c0_5->SetLeftMargin(0.12);
  c0_5->SetBottomMargin(0.18);
  c0_5->SetRightMargin(0.05);
  c0_5->SetTopMargin(0.05);
  c0_5->SetLogy(1);
  c0_6->SetFillColor(10);
  c0_6->Range(-10,-10,10,10);
  c0_6->SetLeftMargin(0.12);
  c0_6->SetBottomMargin(0.18);
  c0_6->SetRightMargin(0.05);
  c0_6->SetTopMargin(0.05);
  c0_6->SetLogy(1);
  c0->cd(1);
  x5->GetXaxis()->SetLabelOffset(0.015);
  x5->GetXaxis()->SetLabelSize(0.040);
  x5->GetYaxis()->SetLabelOffset(0.0025);
  x5->GetYaxis()->SetLabelSize(0.050);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  x5->GetXaxis()->SetTitle("position X (cm)");
  x5->GetXaxis()->SetTitleSize(0.06);
  x5->GetXaxis()->CenterTitle();
  x5->GetYaxis()->SetTitle("Entrees");
  x5->GetYaxis()->SetTitleSize(0.06);
  x5->GetYaxis()->SetTitleOffset(1.20);
  x5->GetYaxis()->CenterTitle();
  x5->SetLineColor(2);
  x5->SetLineWidth(1);
  x5->Draw(); 
  tx5 = new TLatex(0.78,0.78,"X0");
  tx5->SetNDC();
  tx5->SetTextColor(2);
  tx5->SetTextSize(0.06);
  tx5->Draw();
  xy[0][0] = x5->GetMean();
  xy_rms[0][0] = x5->GetRMS();
  c0->cd(5);
  x4->GetXaxis()->SetLabelOffset(0.015);
  x4->GetXaxis()->SetLabelSize(0.040);
  x4->GetYaxis()->SetLabelOffset(0.0025);
  x4->GetYaxis()->SetLabelSize(0.050);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  x4->GetXaxis()->SetTitle("position X (cm)");
  x4->GetXaxis()->SetTitleSize(0.06);
  x4->GetXaxis()->CenterTitle();
  x4->GetYaxis()->SetTitle("Entrees");
  x4->GetYaxis()->SetTitleSize(0.06);
  x4->GetYaxis()->SetTitleOffset(1.20);
  x4->GetYaxis()->CenterTitle();
  x4->SetLineColor(2);
  x4->SetLineWidth(1);
  x4->Draw(); 
  xy[4][0] = x4->GetMean();
  xy_rms[4][0] = x4->GetRMS();
  tx4 = new TLatex(0.78,0.78,"X4");
  tx4->SetNDC();
  tx4->SetTextColor(2);
  tx4->SetTextSize(0.06);
  tx4->Draw();
  c0->cd(4);
  x0->GetXaxis()->SetLabelOffset(0.015);
  x0->GetXaxis()->SetLabelSize(0.040);
  x0->GetYaxis()->SetLabelOffset(0.0025);
  x0->GetYaxis()->SetLabelSize(0.050);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  x0->GetXaxis()->SetTitle("position X (cm)");
  x0->GetXaxis()->SetTitleSize(0.06);
  x0->GetXaxis()->CenterTitle();
  x0->GetYaxis()->SetTitle("Entrees");
  x0->GetYaxis()->SetTitleSize(0.06);
  x0->GetYaxis()->SetTitleOffset(1.20);
  x0->GetYaxis()->CenterTitle();
  x0->SetLineColor(2);
  x0->SetLineWidth(1);
  x0->Draw(); 
  xy[3][0] = x0->GetMean();
  xy_rms[3][0] = x0->GetRMS();
  tx0 = new TLatex(0.78,0.78,"X3");
  tx0->SetNDC();
  tx0->SetTextColor(2);
  tx0->SetTextSize(0.06);
  tx0->Draw();
  c0->cd(2);
  x1->GetXaxis()->SetLabelOffset(0.015);
  x1->GetXaxis()->SetLabelSize(0.040);
  x1->GetYaxis()->SetLabelOffset(0.0025);
  x1->GetYaxis()->SetLabelSize(0.050);
  x1->GetXaxis()->SetTitle("position X (cm)");
  x1->GetXaxis()->SetTitleSize(0.06);
  x1->GetXaxis()->CenterTitle();
  x1->GetYaxis()->SetTitle("Entrees");
  x1->GetYaxis()->SetTitleSize(0.06);
  x1->GetYaxis()->CenterTitle();
  x1->SetLineColor(2);
  x1->SetLineWidth(1);
  x1->Draw(); 
  xy[1][0] = x1->GetMean();
  xy_rms[1][0] = x1->GetRMS();
  tx1 = new TLatex(0.78,0.75,"X1");
  tx1->SetNDC();
  tx1->SetTextColor(2);
  tx1->SetTextSize(0.06);
  tx1->Draw();
  c0->cd(6);
  x2->GetXaxis()->SetLabelOffset(0.015);
  x2->GetXaxis()->SetLabelSize(0.040);
  x2->GetYaxis()->SetLabelOffset(0.0025);
  x2->GetYaxis()->SetLabelSize(0.050);
  x2->GetXaxis()->SetTitle("position X (cm)");
  x2->GetXaxis()->SetTitleSize(0.06);
  x2->GetXaxis()->CenterTitle();
  x2->GetYaxis()->SetTitle("Entrees");
  x2->GetYaxis()->SetTitleSize(0.06);
  x2->GetYaxis()->CenterTitle();
  x2->SetLineColor(2);
  x2->SetLineWidth(1);
  x2->Draw();
  xy[5][0] = x2->GetMean();
  xy_rms[5][0] = x2->GetRMS();
  tx2 = new TLatex(0.78,0.78,"X5");
  tx2->SetNDC();
  tx2->SetTextColor(2);
  tx2->SetTextSize(0.06);
  tx2->Draw();
  c0->cd(3);
  x3->GetXaxis()->SetLabelOffset(0.015);
  x3->GetXaxis()->SetLabelSize(0.040);
  x3->GetYaxis()->SetLabelOffset(0.0025);
  x3->GetYaxis()->SetLabelSize(0.050);
  x3->GetXaxis()->SetTitle("position X (cm)");
  x3->GetXaxis()->SetTitleSize(0.06);
  x3->GetYaxis()->SetTitleSize(0.06);
  x3->GetYaxis()->SetTitle("Entrees");
  x3->GetXaxis()->CenterTitle();
  x3->GetYaxis()->CenterTitle();
  x3->SetLineColor(2);
  x3->SetLineWidth(1);
  x3->Draw();
  xy[2][0] = x3->GetMean();
  xy_rms[2][0] = x3->GetRMS();
  tx3 = new TLatex(0.78,0.75,"X2");
  tx3->SetNDC();
  tx3->SetTextColor(2);
  tx3->SetTextSize(0.06);
  tx3->Draw();
  txangle = new TLatex(0.78,0.65,"5^{o}");
  txangle->SetNDC();
  txangle->SetTextColor(2);
  txangle->SetTextSize(0.06);
  txangle->Draw();

  TCanvas* c1=new TCanvas("c1","ypos",0,0,650,650);
  c1->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 60067 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  c1->Divide(2,3,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.12);
  c1_1->SetBottomMargin(0.18);
  c1_1->SetRightMargin(0.05);
  c1_1->SetTopMargin(0.05);
  c1_1->SetLogy(1);
  c1_2->SetFillColor(10);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.12);
  c1_2->SetBottomMargin(0.18);
  c1_2->SetRightMargin(0.05);
  c1_2->SetTopMargin(0.05);
  c1_2->SetLogy(1);
  c1_3->SetFillColor(10);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.12);
  c1_3->SetBottomMargin(0.18);
  c1_3->SetRightMargin(0.05);
  c1_3->SetTopMargin(0.05);
  c1_3->SetLogy(1);
  c1_4->SetFillColor(10);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.12);
  c1_4->SetBottomMargin(0.18);
  c1_4->SetRightMargin(0.05);
  c1_4->SetTopMargin(0.05);
  c1_4->SetLogy(1);
  c1_5->SetFillColor(10);
  c1_5->Range(-10,-10,10,10);
  c1_5->SetLeftMargin(0.12);
  c1_5->SetBottomMargin(0.18);
  c1_5->SetRightMargin(0.05);
  c1_5->SetTopMargin(0.05);
  c1_5->SetLogy(1);
  c1_6->SetFillColor(10);
  c1_6->Range(-10,-10,10,10);
  c1_6->SetLeftMargin(0.12);
  c1_6->SetBottomMargin(0.18);
  c1_6->SetRightMargin(0.05);
  c1_6->SetTopMargin(0.05);
  c1_6->SetLogy(1);
  c1->cd(1);
  y5->GetXaxis()->SetLabelOffset(0.015);
  y5->GetXaxis()->SetLabelSize(0.040);
  y5->GetYaxis()->SetLabelOffset(0.0025);
  y5->GetYaxis()->SetLabelSize(0.050);
  y5->GetXaxis()->SetTitle("position Y (cm)");
  y5->GetXaxis()->SetTitleSize(0.06);
  y5->GetXaxis()->CenterTitle();
  y5->GetYaxis()->SetTitle("Entrees");
  y5->GetYaxis()->SetTitleSize(0.06);
  y5->GetYaxis()->SetTitleOffset(1.20);
  y5->GetYaxis()->CenterTitle();
  y5->SetLineColor(4);
  y5->SetLineWidth(1);
  y5->Draw();
  xy[0][1] = y5->GetMean();
  xy_rms[0][1] = y5->GetRMS();
  ty5 = new TLatex(0.78,0.78,"Y0");
  ty5->SetNDC();
  ty5->SetTextColor(4);
  ty5->SetTextSize(0.06);
  ty5->Draw();
  c1->cd(5);
  y4->GetXaxis()->SetLabelOffset(0.015);
  y4->GetXaxis()->SetLabelSize(0.040);
  y4->GetYaxis()->SetLabelOffset(0.0025);
  y4->GetYaxis()->SetLabelSize(0.050);
  y4->GetXaxis()->SetTitle("position Y (cm)");
  y4->GetXaxis()->SetTitleSize(0.06);
  y4->GetXaxis()->CenterTitle();
  y4->GetYaxis()->SetTitle("Entrees");
  y4->GetYaxis()->SetTitleSize(0.06);
  y4->GetYaxis()->SetTitleOffset(1.20);
  y4->GetYaxis()->CenterTitle();
  y4->SetLineColor(4);
  y4->SetLineWidth(1);
  y4->Draw();
  xy[4][1] = y4->GetMean();
  xy_rms[4][1] = y4->GetRMS();
  ty4 = new TLatex(0.78,0.78,"Y4");
  ty4->SetNDC();
  ty4->SetTextColor(4);
  ty4->SetTextSize(0.06);
  ty4->Draw();
  c1->cd(4);
  y0->GetXaxis()->SetLabelOffset(0.015);
  y0->GetXaxis()->SetLabelSize(0.040);
  y0->GetYaxis()->SetLabelOffset(0.0025);
  y0->GetYaxis()->SetLabelSize(0.050);
  y0->GetXaxis()->SetTitle("position Y (cm)");
  y0->GetXaxis()->SetTitleSize(0.06);
  y0->GetXaxis()->CenterTitle();
  y0->GetYaxis()->SetTitle("Entrees");
  y0->GetYaxis()->SetTitleSize(0.06);
  y0->GetYaxis()->SetTitleOffset(1.20);
  y0->GetYaxis()->CenterTitle();
  y0->SetLineColor(4);
  y0->SetLineWidth(1);
  y0->Draw();
  xy[3][1] = y0->GetMean();
  xy_rms[3][1] = y0->GetRMS();
  ty0 = new TLatex(0.78,0.78,"Y3");
  ty0->SetNDC();
  ty0->SetTextColor(4);
  ty0->SetTextSize(0.06);
  ty0->Draw();
  c1->cd(2);
  y1->GetXaxis()->SetLabelOffset(0.015);
  y1->GetXaxis()->SetLabelSize(0.040);
  y1->GetYaxis()->SetLabelOffset(0.0025);
  y1->GetYaxis()->SetLabelSize(0.050);
  y1->GetXaxis()->SetTitle("position Y (cm)");
  y1->GetXaxis()->SetTitleSize(0.06);
  y1->GetXaxis()->CenterTitle();
  y1->GetYaxis()->SetTitle("Entrees");
  y1->GetYaxis()->SetTitleSize(0.06);
  y1->GetYaxis()->CenterTitle();
  y1->SetLineColor(4);
  y1->SetLineWidth(1);
  y1->Draw();
  xy[1][1] = y1->GetMean();
  xy_rms[1][1] = y1->GetRMS();
  ty1 = new TLatex(0.78,0.75,"Y1");
  ty1->SetNDC();
  ty1->SetTextColor(4);
  ty1->SetTextSize(0.06);
  ty1->Draw(); 
  c1->cd(6);
  y2->GetXaxis()->SetLabelOffset(0.015);
  y2->GetXaxis()->SetLabelSize(0.040);
  y2->GetYaxis()->SetLabelOffset(0.0025);
  y2->GetYaxis()->SetLabelSize(0.050);
  y2->GetXaxis()->SetTitle("position Y (cm)");
  y2->GetXaxis()->SetTitleSize(0.06);
  y2->GetXaxis()->CenterTitle();
  y2->GetYaxis()->SetTitle("Entrees");
  y2->GetYaxis()->SetTitleSize(0.06);
  y2->GetYaxis()->CenterTitle();
  y2->SetLineColor(4);
  y2->SetLineWidth(1);
  y2->Draw();
  ty2 = new TLatex(0.78,0.78,"Y5");
  ty2->SetNDC();
  ty2->SetTextColor(4);
  ty2->SetTextSize(0.06);
  ty2->Draw();
  xy[5][1] = y2->GetMean();
  xy_rms[5][1] = y2->GetRMS();
  c1->cd(3);
  y3->GetXaxis()->SetLabelOffset(0.015);
  y3->GetXaxis()->SetLabelSize(0.040);
  y3->GetYaxis()->SetLabelOffset(0.0025);
  y3->GetYaxis()->SetLabelSize(0.050);
  y3->GetXaxis()->SetTitle("position Y (cm)");
  y3->GetXaxis()->SetTitleSize(0.06);
  y3->GetYaxis()->SetTitle("Entrees");
  y3->GetYaxis()->SetTitleSize(0.06);
  y3->GetXaxis()->CenterTitle();
  y3->GetYaxis()->CenterTitle();
  y3->SetLineColor(4);
  y3->SetLineWidth(1);
  y3->Draw();
  xy[2][1] = y3->GetMean();
  xy_rms[2][1] = y3->GetRMS();
  ty3 = new TLatex(0.78,0.75,"Y2");
  ty3->SetNDC();
  ty3->SetTextColor(4);
  ty3->SetTextSize(0.06);
  ty3->Draw();
  tyangle = new TLatex(0.78,0.65,"5^{o}");
  tyangle->SetNDC();
  tyangle->SetTextColor(4);
  tyangle->SetTextSize(0.06);
  tyangle->Draw();

  char cfxy[80];
  sprintf(cfxy,"xypos_%d.dat",run);
  FILE *fxy = fopen(cfxy,"w");
  for (int i=0; i<6; i++) 
    fprintf(fxy,"pos %d x %7.4f rms %7.4f y %7.4f rms %7.4f (mm)\n",i,10*xy[i][0],10*xy_rms[i][0],10*xy[i][1],10*xy_rms[i][1]);
  fclose(fxy);
  return;
}


void xypos_hr()
{
  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  TH1D* hxhr[6];
  TH1D* hyhr[6];
  TH1D* hx1p[6];
  TH1D* hy1p[6];

  char cpad[20];
  char chist[20];

  for (int i=0; i<6; i++) {
    sprintf(chist,"x%dhr",i);
    hxhr[i] = (TH1D*) f->Get(chist)->Clone();
    hxhr[i]->GetXaxis()->SetLabelOffset(0.015);
    hxhr[i]->GetXaxis()->SetLabelSize(0.065);
    hxhr[i]->GetYaxis()->SetLabelOffset(0.0025);
    hxhr[i]->GetYaxis()->SetLabelSize(0.065);
    hxhr[i]->GetXaxis()->SetRangeUser(-0.15,0.15);
    hxhr[i]->GetXaxis()->SetTitle("position X (mm)");
    hxhr[i]->GetXaxis()->SetTitleSize(0.08);
    hxhr[i]->GetXaxis()->CenterTitle();
    hxhr[i]->GetYaxis()->SetTitle("Entrees");
    hxhr[i]->GetYaxis()->SetTitleSize(0.08);
    hxhr[i]->GetYaxis()->SetTitleOffset(0.6);
    hxhr[i]->GetYaxis()->CenterTitle();
    hxhr[i]->SetLineColor(2);
    hxhr[i]->SetLineWidth(1);
    hxhr[i]->SetMinimum(0.5);
    hxhr[i]->GetXaxis()->SetLabelOffset(0.015);
    hxhr[i]->GetXaxis()->SetLabelSize(0.065);
    hxhr[i]->GetYaxis()->SetLabelOffset(0.0025);
    hxhr[i]->GetYaxis()->SetLabelSize(0.065);
    hxhr[i]->GetXaxis()->SetRangeUser(-0.15,0.15);
    hxhr[i]->GetXaxis()->SetTitle("position X (mm)");
    hxhr[i]->GetXaxis()->SetTitleSize(0.08);
    hxhr[i]->GetXaxis()->CenterTitle();
    hxhr[i]->GetYaxis()->SetTitle("Entrees");
    hxhr[i]->GetYaxis()->SetTitleSize(0.08);
    hxhr[i]->GetYaxis()->SetTitleOffset(0.6);
    hxhr[i]->GetYaxis()->CenterTitle();
    hxhr[i]->SetLineColor(2);
    hxhr[i]->SetLineWidth(1);
    hxhr[i]->SetMinimum(0.5);
    sprintf(chist,"y%dhr",i);
    hyhr[i] = (TH1D*) f->Get(chist)->Clone();
    hyhr[i]->GetXaxis()->SetLabelOffset(0.015);
    hyhr[i]->GetXaxis()->SetLabelSize(0.065);
    hyhr[i]->GetYaxis()->SetLabelOffset(0.0025);
    hyhr[i]->GetYaxis()->SetLabelSize(0.065);
    hyhr[i]->GetXaxis()->SetTitle("position Y (mm)");
    hyhr[i]->GetXaxis()->SetTitleSize(0.08);
    hyhr[i]->GetXaxis()->CenterTitle();
    hyhr[i]->GetXaxis()->SetRangeUser(-0.3,0.3);
    hyhr[i]->GetYaxis()->SetTitle("Entrees");
    hyhr[i]->GetYaxis()->SetTitleSize(0.08);
    hyhr[i]->GetYaxis()->SetTitleOffset(0.6);
    hyhr[i]->GetYaxis()->CenterTitle();
    hyhr[i]->SetLineColor(4);
    hyhr[i]->SetLineWidth(1);
    hyhr[i]->SetMinimum(0.5);
    sprintf(chist,"x%d1p",i);
    hx1p[i] = (TH1D*) f->Get(chist)->Clone();
    sprintf(chist,"y%d1p",i);
    hy1p[i] = (TH1D*) f->Get(chist)->Clone();
  }

  TCanvas* c0=new TCanvas("c0","xpos",0,0,650,650);
  c0->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 1510 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  c0->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c0->cd(i+1);
    sprintf(cpad,"c0_%d",i+1);
    TPad *ptpad1 = c0->FindObject(cpad);
    ptpad1->SetFillColor(10);
    ptpad1->Range(-10,-10,10,10);
    ptpad1->SetLeftMargin(0.12);
    ptpad1->SetBottomMargin(0.18);
    ptpad1->SetRightMargin(0.05);
    ptpad1->SetTopMargin(0.05);
    ptpad1->SetLogy(1);
  }
  c0->cd(1);
  x5hr->GetXaxis()->SetLabelOffset(0.015);
  x5hr->GetXaxis()->SetLabelSize(0.065);
  x5hr->GetYaxis()->SetLabelOffset(0.0025);
  x5hr->GetYaxis()->SetLabelSize(0.065);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  x5hr->GetXaxis()->SetTitle("position X (mm)");
  x5hr->GetXaxis()->SetTitleSize(0.08);
  x5hr->GetXaxis()->CenterTitle();
  x5hr->GetYaxis()->SetTitle("Entrees");
  x5hr->GetYaxis()->SetTitleSize(0.08);
  x5hr->GetYaxis()->SetTitleOffset(0.6);
  x5hr->GetYaxis()->CenterTitle();
  x5hr->SetLineColor(2);
  x5hr->SetLineWidth(1);
  x5hr->Draw("hist"); 
  x51p->Draw("samehist"); 
  tx5 = new TLatex(0.78,0.78,"X0");
  tx5->SetNDC();
  tx5->SetTextColor(2);
  tx5->SetTextSize(0.08);
  tx5->Draw();
  c0->cd(5);
  x4hr->GetXaxis()->SetLabelOffset(0.015);
  x4hr->GetXaxis()->SetLabelSize(0.065);
  x4hr->GetYaxis()->SetLabelOffset(0.0025);
  x4hr->GetYaxis()->SetLabelSize(0.065);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  x4hr->GetXaxis()->SetTitle("position X (mm)");
  x4hr->GetXaxis()->SetTitleSize(0.08);
  x4hr->GetXaxis()->CenterTitle();
  x4hr->GetYaxis()->SetTitle("Entrees");
  x4hr->GetYaxis()->SetTitleSize(0.08);
  x4hr->GetYaxis()->SetTitleOffset(0.6);
  x4hr->GetYaxis()->CenterTitle();
  x4hr->SetLineColor(2);
  x4hr->SetLineWidth(1);
  x4hr->Draw("hist"); 
  x41p->Draw("samehist");
  tx4 = new TLatex(0.78,0.78,"X4");
  tx4->SetNDC();
  tx4->SetTextColor(2);
  tx4->SetTextSize(0.08);
  tx4->Draw();
  c0->cd(4);
  x0hr->GetXaxis()->SetLabelOffset(0.015);
  x0hr->GetXaxis()->SetLabelSize(0.065);
  x0hr->GetYaxis()->SetLabelOffset(0.0025);
  x0hr->GetYaxis()->SetLabelSize(0.065);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  x0hr->GetXaxis()->SetTitle("position X (mm)");
  x0hr->GetXaxis()->SetTitleSize(0.08);
  x0hr->GetXaxis()->CenterTitle();
  x0hr->GetYaxis()->SetTitle("Entrees");
  x0hr->GetYaxis()->SetTitleSize(0.08);
  x0hr->GetYaxis()->SetTitleOffset(0.6);
  x0hr->GetYaxis()->CenterTitle();
  x0hr->SetLineColor(2);
  x0hr->SetLineWidth(1);
  x0hr->Draw("hist"); 
  x01p->Draw("samehist"); 
  tx0 = new TLatex(0.78,0.78,"X3");
  tx0->SetNDC();
  tx0->SetTextColor(2);
  tx0->SetTextSize(0.08);
  tx0->Draw();
  c0->cd(2);
  x1hr->GetXaxis()->SetLabelOffset(0.015);
  x1hr->GetXaxis()->SetLabelSize(0.065);
  x1hr->GetYaxis()->SetLabelOffset(0.0025);
  x1hr->GetYaxis()->SetLabelSize(0.065);
  x1hr->GetXaxis()->SetTitle("position X (mm)");
  x1hr->GetXaxis()->SetTitleSize(0.08);
  x1hr->GetXaxis()->CenterTitle();
  x1hr->GetYaxis()->SetTitle("Entrees");
  x1hr->GetYaxis()->SetTitleSize(0.08);
  x1hr->GetYaxis()->SetTitleOffset(0.6);
  x1hr->GetYaxis()->CenterTitle();
  x1hr->SetLineColor(2);
  x1hr->SetLineWidth(1);
  x1hr->Draw("hist"); 
  x11p->Draw("samehist"); 
  tx1 = new TLatex(0.78,0.75,"X1");
  tx1->SetNDC();
  tx1->SetTextColor(2);
  tx1->SetTextSize(0.08);
  tx1->Draw();
  c0->cd(6);
  x2hr->GetXaxis()->SetLabelOffset(0.015);
  x2hr->GetXaxis()->SetLabelSize(0.065);
  x2hr->GetYaxis()->SetLabelOffset(0.0025);
  x2hr->GetYaxis()->SetLabelSize(0.065);
  x2hr->GetXaxis()->SetTitle("position X (mm)");
  x2hr->GetXaxis()->SetTitleSize(0.08);
  x2hr->GetXaxis()->CenterTitle();
  x2hr->GetYaxis()->SetTitle("Entrees");
  x2hr->GetYaxis()->SetTitleSize(0.08);
  x2hr->GetYaxis()->SetTitleOffset(0.6);
  x2hr->GetYaxis()->CenterTitle();
  x2hr->SetLineColor(2);
  x2hr->SetLineWidth(1);
  x2hr->Draw("hist");
  x21p->Draw("samehist");
  tx2 = new TLatex(0.78,0.78,"X5");
  tx2->SetNDC();
  tx2->SetTextColor(2);
  tx2->SetTextSize(0.08);
  tx2->Draw();
  c0->cd(3);
  x3hr->GetXaxis()->SetLabelOffset(0.015);
  x3hr->GetXaxis()->SetLabelSize(0.065);
  x3hr->GetYaxis()->SetLabelOffset(0.0025);
  x3hr->GetYaxis()->SetLabelSize(0.065);
  x3hr->GetXaxis()->SetTitle("position X (mm)");
  x3hr->GetXaxis()->SetTitleSize(0.08);
  x3hr->GetYaxis()->SetTitleSize(0.08);
  x3hr->GetYaxis()->SetTitleOffset(0.6);
  x3hr->GetYaxis()->SetTitle("Entrees");
  x3hr->GetXaxis()->CenterTitle();
  x3hr->GetYaxis()->CenterTitle();
  x3hr->SetLineColor(2);
  x3hr->SetLineWidth(1);
  x3hr->Draw("hist");
  x31p->Draw("samehist");
  tx3 = new TLatex(0.78,0.75,"X2");
  tx3->SetNDC();
  tx3->SetTextColor(2);
  tx3->SetTextSize(0.08);
  tx3->Draw();
  txangle = new TLatex(0.78,0.65,"0^{o}");
  txangle->SetNDC();
  txangle->SetTextColor(2);
  txangle->SetTextSize(0.08);
  txangle->Draw();

  TCanvas* c1=new TCanvas("c1","ypos",0,0,650,650);
  c1->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 60067 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  c1->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c1->cd(i+1);
    sprintf(cpad,"c1_%d",i+1);
    TPad *ptpad2 = c1->FindObject(cpad);
    ptpad2->SetFillColor(10);
    ptpad2->Range(-10,-10,10,10);
    ptpad2->SetLeftMargin(0.12);
    ptpad2->SetBottomMargin(0.18);
    ptpad2->SetRightMargin(0.05);
    ptpad2->SetTopMargin(0.05);
    ptpad2->SetLogy(1);
  }
  c1->cd(1);
  y5hr->GetXaxis()->SetLabelOffset(0.015);
  y5hr->GetXaxis()->SetLabelSize(0.065);
  y5hr->GetYaxis()->SetLabelOffset(0.0025);
  y5hr->GetYaxis()->SetLabelSize(0.065);
  y5hr->GetXaxis()->SetTitle("position Y (mm)");
  y5hr->GetXaxis()->SetTitleSize(0.08);
  y5hr->GetXaxis()->CenterTitle();
  y5hr->GetYaxis()->SetTitle("Entrees");
  y5hr->GetYaxis()->SetTitleSize(0.08);
  y5hr->GetYaxis()->SetTitleOffset(0.6);
  y5hr->GetYaxis()->CenterTitle();
  y5hr->SetLineColor(4);
  y5hr->SetLineWidth(1);
  y5hr->Draw("hist");
  y51p->Draw("samehist");
  ty5 = new TLatex(0.78,0.78,"Y0");
  ty5->SetNDC();
  ty5->SetTextColor(4);
  ty5->SetTextSize(0.08);
  ty5->Draw();
  c1->cd(5);
  y4hr->GetXaxis()->SetLabelOffset(0.015);
  y4hr->GetXaxis()->SetLabelSize(0.065);
  y4hr->GetYaxis()->SetLabelOffset(0.0025);
  y4hr->GetYaxis()->SetLabelSize(0.065);
  y4hr->GetXaxis()->SetTitle("position Y (mm)");
  y4hr->GetXaxis()->SetTitleSize(0.08);
  y4hr->GetXaxis()->CenterTitle();
  y4hr->GetYaxis()->SetTitle("Entrees");
  y4hr->GetYaxis()->SetTitleSize(0.08);
  y4hr->GetYaxis()->SetTitleOffset(0.6);
  y4hr->GetYaxis()->CenterTitle();
  y4hr->SetLineColor(4);
  y4hr->SetLineWidth(1);
  y4hr->Draw("hist");
  y41p->Draw("samehist");
  ty4 = new TLatex(0.78,0.78,"Y4");
  ty4->SetNDC();
  ty4->SetTextColor(4);
  ty4->SetTextSize(0.08);
  ty4->Draw();
  c1->cd(4);
  y0hr->GetXaxis()->SetLabelOffset(0.015);
  y0hr->GetXaxis()->SetLabelSize(0.065);
  y0hr->GetYaxis()->SetLabelOffset(0.0025);
  y0hr->GetYaxis()->SetLabelSize(0.065);
  y0hr->GetXaxis()->SetTitle("position Y (mm)");
  y0hr->GetXaxis()->SetTitleSize(0.08);
  y0hr->GetXaxis()->CenterTitle();
  y0hr->GetYaxis()->SetTitle("Entrees");
  y0hr->GetYaxis()->SetTitleSize(0.08);
  y0hr->GetYaxis()->SetTitleOffset(0.6);
  y0hr->GetYaxis()->CenterTitle();
  y0hr->SetLineColor(4);
  y0hr->SetLineWidth(1);
  y0hr->Draw("hist");
  y01p->Draw("samehist");
  ty0 = new TLatex(0.78,0.78,"Y3");
  ty0->SetNDC();
  ty0->SetTextColor(4);
  ty0->SetTextSize(0.08);
  ty0->Draw();
  c1->cd(2);
  y1hr->GetXaxis()->SetLabelOffset(0.015);
  y1hr->GetXaxis()->SetLabelSize(0.065);
  y1hr->GetYaxis()->SetLabelOffset(0.0025);
  y1hr->GetYaxis()->SetLabelSize(0.065);
  y1hr->GetXaxis()->SetTitle("position Y (mm)");
  y1hr->GetXaxis()->SetTitleSize(0.08);
  y1hr->GetXaxis()->CenterTitle();
  y1hr->GetYaxis()->SetTitle("Entrees");
  y1hr->GetYaxis()->SetTitleSize(0.08);
  y1hr->GetYaxis()->SetTitleOffset(0.6);
  y1hr->GetYaxis()->CenterTitle();
  y1hr->SetLineColor(4);
  y1hr->SetLineWidth(1);
  y1hr->Draw("hist");
  y11p->Draw("samehist");
  ty1 = new TLatex(0.78,0.75,"Y1");
  ty1->SetNDC();
  ty1->SetTextColor(4);
  ty1->SetTextSize(0.08);
  ty1->Draw(); 
  c1->cd(6);
  y2hr->GetXaxis()->SetLabelOffset(0.015);
  y2hr->GetXaxis()->SetLabelSize(0.065);
  y2hr->GetYaxis()->SetLabelOffset(0.0025);
  y2hr->GetYaxis()->SetLabelSize(0.065);
  y2hr->GetXaxis()->SetTitle("position Y (mm)");
  y2hr->GetXaxis()->SetTitleSize(0.08);
  y2hr->GetXaxis()->CenterTitle();
  y2hr->GetYaxis()->SetTitle("Entrees");
  y2hr->GetYaxis()->SetTitleSize(0.08);
  y2hr->GetYaxis()->SetTitleOffset(0.6);
  y2hr->GetYaxis()->CenterTitle();
  y2hr->SetLineColor(4);
  y2hr->SetLineWidth(1);
  y2hr->Draw("hist");
  y21p->Draw("samehist");
  ty2 = new TLatex(0.78,0.78,"Y5");
  ty2->SetNDC();
  ty2->SetTextColor(4);
  ty2->SetTextSize(0.08);
  ty2->Draw();
  c1->cd(3);
  y3hr->GetXaxis()->SetLabelOffset(0.015);
  y3hr->GetXaxis()->SetLabelSize(0.065);
  y3hr->GetYaxis()->SetLabelOffset(0.0025);
  y3hr->GetYaxis()->SetLabelSize(0.065);
  y3hr->GetXaxis()->SetTitle("position Y (mm)");
  y3hr->GetXaxis()->SetTitleSize(0.08);
  y3hr->GetYaxis()->SetTitle("Entrees");
  y3hr->GetYaxis()->SetTitleSize(0.08);
  y3hr->GetXaxis()->CenterTitle();
  y3hr->GetYaxis()->SetTitleOffset(0.6);
  y3hr->GetYaxis()->CenterTitle();
  y3hr->SetLineColor(4);
  y3hr->SetLineWidth(1);
  y3hr->Draw("hist");
  y31p->Draw("samehist");
  ty3 = new TLatex(0.78,0.75,"Y2");
  ty3->SetNDC();
  ty3->SetTextColor(4);
  ty3->SetTextSize(0.08);
  ty3->Draw();
  tyangle = new TLatex(0.78,0.65,"0^{o}");
  tyangle->SetNDC();
  tyangle->SetTextColor(4);
  tyangle->SetTextSize(0.08);
  tyangle->Draw();

  TCanvas* c2=new TCanvas("c2","xpos 1p",0,0,650,650);
  c2->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 1510 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  c2->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c2->cd(i+1);
    sprintf(cpad,"c2_%d",i+1);
    TPad *ptpad3 = c2->FindObject(cpad);
    ptpad3->SetFillColor(10);
    ptpad3->Range(-10,-10,10,10);
    ptpad3->SetLeftMargin(0.12);
    ptpad3->SetBottomMargin(0.18);
    ptpad3->SetRightMargin(0.05);
    ptpad3->SetTopMargin(0.05);
    ptpad3->SetLogy(1);
  }
  c2->cd(1);
  hxhr[5]->Draw("hist"); 
  hx1p[5]->Draw("samehist"); 
  tx5 = new TLatex(0.78,0.38,"X0");
  tx5->SetNDC();
  tx5->SetTextColor(2);
  tx5->SetTextSize(0.08);
  tx5->Draw();
  c2->cd(5);
  hxhr[4]->Draw("hist"); 
  hx1p[4]->Draw("samehist");
  tx4 = new TLatex(0.78,0.38,"X4");
  tx4->SetNDC();
  tx4->SetTextColor(2);
  tx4->SetTextSize(0.08);
  tx4->Draw();
  c2->cd(4);
  hxhr[0]->Draw("hist"); 
  hx1p[0]->Draw("samehist"); 
  tx0 = new TLatex(0.78,0.38,"X3");
  tx0->SetNDC();
  tx0->SetTextColor(2);
  tx0->SetTextSize(0.08);
  tx0->Draw();
  c2->cd(2);
  hxhr[1]->Draw("hist"); 
  hx1p[1]->Draw("samehist"); 
  tx1 = new TLatex(0.78,0.38,"X1");
  tx1->SetNDC();
  tx1->SetTextColor(2);
  tx1->SetTextSize(0.08);
  tx1->Draw();
  c2->cd(6);
  hxhr[2]->Draw("hist");
  hx1p[2]->Draw("samehist");
  tx2 = new TLatex(0.78,0.38,"X5");
  tx2->SetNDC();
  tx2->SetTextColor(2);
  tx2->SetTextSize(0.08);
  tx2->Draw();
  c2->cd(3);
  hxhr[3]->Draw("hist");
  hx1p[3]->Draw("samehist");
  tx3 = new TLatex(0.78,0.38,"X2");
  tx3->SetNDC();
  tx3->SetTextColor(2);
  tx3->SetTextSize(0.08);
  tx3->Draw();
  txangle = new TLatex(0.78,0.28,"0^{o}");
  txangle->SetNDC();
  txangle->SetTextColor(2);
  txangle->SetTextSize(0.08);
  txangle->Draw();

  TCanvas* c3=new TCanvas("c3","ypos 1p",0,0,650,650);
  c3->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 60067 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  c3->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
    c3->cd(i+1);
    sprintf(cpad,"c3_%d",i+1);
    TPad *ptpad4 = c3->FindObject(cpad);
    ptpad4->SetFillColor(10);
    ptpad4->Range(-10,-10,10,10);
    ptpad4->SetLeftMargin(0.12);
    ptpad4->SetBottomMargin(0.18);
    ptpad4->SetRightMargin(0.05);
    ptpad4->SetTopMargin(0.05);
    ptpad4->SetLogy(1);
  }
  c3->cd(1);
  hyhr[5]->Draw("hist");
  hy1p[5]->Draw("samehist");
  ty5 = new TLatex(0.78,0.38,"Y0");
  ty5->SetNDC();
  ty5->SetTextColor(4);
  ty5->SetTextSize(0.08);
  ty5->Draw();
  c3->cd(5);
  hyhr[4]->Draw("hist");
  hy1p[4]->Draw("samehist");
  ty4 = new TLatex(0.78,0.38,"Y4");
  ty4->SetNDC();
  ty4->SetTextColor(4);
  ty4->SetTextSize(0.08);
  ty4->Draw();
  c3->cd(4);
  hyhr[0]->Draw("hist");
  hy1p[0]->Draw("samehist");
  ty0 = new TLatex(0.78,0.38,"Y3");
  ty0->SetNDC();
  ty0->SetTextColor(4);
  ty0->SetTextSize(0.08);
  ty0->Draw();
  c3->cd(2);
  hyhr[1]->Draw("hist");
  hy1p[1]->Draw("samehist");
  ty1 = new TLatex(0.78,0.38,"Y1");
  ty1->SetNDC();
  ty1->SetTextColor(4);
  ty1->SetTextSize(0.08);
  ty1->Draw(); 
  c3->cd(6);
  hyhr[2]->Draw("hist");
  hy1p[2]->Draw("samehist");
  ty2 = new TLatex(0.78,0.38,"Y5");
  ty2->SetNDC();
  ty2->SetTextColor(4);
  ty2->SetTextSize(0.08);
  ty2->Draw();
  c3->cd(3);
  hyhr[3]->Draw("hist");
  hy1p[3]->Draw("samehist");
  ty3 = new TLatex(0.78,0.38,"Y2");
  ty3->SetNDC();
  ty3->SetTextColor(4);
  ty3->SetTextSize(0.08);
  ty3->Draw();
  tyangle = new TLatex(0.78,0.28,"0^{o}");
  tyangle->SetNDC();
  tyangle->SetTextColor(4);
  tyangle->SetTextSize(0.08);
  tyangle->Draw();

  return;
}

void residpos6()
{
  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","residus et pos x",0,0,650,650);
  c0->Divide(2,3,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.12);
  c0_1->SetBottomMargin(0.18);
  c0_1->SetRightMargin(0.05);
  c0_1->SetTopMargin(0.05);
  c0_1->SetLogz(1);
  c0_2->SetFillColor(10);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.12);
  c0_2->SetBottomMargin(0.18);
  c0_2->SetRightMargin(0.05);
  c0_2->SetTopMargin(0.05);
  c0_2->SetLogz(1);
  c0_3->SetFillColor(10);
  c0_3->Range(-10,-10,10,10);
  c0_3->SetLeftMargin(0.12);
  c0_3->SetBottomMargin(0.18);
  c0_3->SetRightMargin(0.05);
  c0_3->SetTopMargin(0.05);
  c0_3->SetLogz(1);
  c0_4->SetFillColor(10);
  c0_4->Range(-10,-10,10,10);
  c0_4->SetLeftMargin(0.12);
  c0_4->SetBottomMargin(0.18);
  c0_4->SetRightMargin(0.05);
  c0_4->SetTopMargin(0.05);
  c0_4->SetLogz(1);
  c0_5->SetFillColor(10);
  c0_5->Range(-10,-10,10,10);
  c0_5->SetLeftMargin(0.12);
  c0_5->SetBottomMargin(0.18);
  c0_5->SetRightMargin(0.05);
  c0_5->SetTopMargin(0.05);
  c0_5->SetLogz(1);
  c0_6->SetFillColor(10);
  c0_6->Range(-10,-10,10,10);
  c0_6->SetLeftMargin(0.12);
  c0_6->SetBottomMargin(0.18);
  c0_6->SetRightMargin(0.05);
  c0_6->SetTopMargin(0.05);
  c0_6->SetLogz(1);
  c0->cd(1);
  reseposx5->GetXaxis()->SetLabelOffset(0.015);
  reseposx5->GetXaxis()->SetLabelSize(0.040);
  reseposx5->GetYaxis()->SetLabelOffset(0.0025);
  reseposx5->GetYaxis()->SetLabelSize(0.045);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  reseposx5->GetXaxis()->SetTitle("gap position");
  reseposx5->GetXaxis()->SetTitleSize(0.06);
  reseposx5->GetXaxis()->CenterTitle();
  reseposx5->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx5->GetYaxis()->SetTitleSize(0.06);
  reseposx5->GetYaxis()->SetTitleOffset(1.20);
  reseposx5->GetYaxis()->CenterTitle();
  reseposx5->SetLineColor(1);
  reseposx5->SetLineWidth(1);
  reseposx5->Draw("colz"); 
  tx5 = new TLatex(0.78,0.78,"X0");
  tx5->SetNDC();
  tx5->SetTextColor(1);
  tx5->SetTextSize(0.06);
  tx5->Draw();
  c0->cd(5);
  reseposx4->GetXaxis()->SetLabelOffset(0.015);
  reseposx4->GetXaxis()->SetLabelSize(0.040);
  reseposx4->GetYaxis()->SetLabelOffset(0.0025);
  reseposx4->GetYaxis()->SetLabelSize(0.045);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  reseposx4->GetXaxis()->SetTitle("gap position");
  reseposx4->GetXaxis()->SetTitleSize(0.06);
  reseposx4->GetXaxis()->CenterTitle();
  reseposx4->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx4->GetYaxis()->SetTitleSize(0.06);
  reseposx4->GetYaxis()->SetTitleOffset(1.20);
  reseposx4->GetYaxis()->CenterTitle();
  reseposx4->SetLineColor(1);
  reseposx4->SetLineWidth(1);
  reseposx4->Draw("colz"); 
  tx4 = new TLatex(0.78,0.78,"X4");
  tx4->SetNDC();
  tx4->SetTextColor(1);
  tx4->SetTextSize(0.06);
  tx4->Draw();
  c0->cd(4);
  reseposx0->GetXaxis()->SetLabelOffset(0.015);
  reseposx0->GetXaxis()->SetLabelSize(0.040);
  reseposx0->GetYaxis()->SetLabelOffset(0.0025);
  reseposx0->GetYaxis()->SetLabelSize(0.045);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  reseposx0->GetXaxis()->SetTitle("gap position");
  reseposx0->GetXaxis()->SetTitleSize(0.06);
  reseposx0->GetXaxis()->CenterTitle();
  reseposx0->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx0->GetYaxis()->SetTitleSize(0.06);
  reseposx0->GetYaxis()->SetTitleOffset(1.20);
  reseposx0->GetYaxis()->CenterTitle();
  reseposx0->SetLineColor(1);
  reseposx0->SetLineWidth(1);
  reseposx0->Draw("colz"); 
  tx0 = new TLatex(0.78,0.78,"X3");
  tx0->SetNDC();
  tx0->SetTextColor(1);
  tx0->SetTextSize(0.06);
  tx0->Draw();
  c0->cd(2);
  reseposx1->GetXaxis()->SetLabelOffset(0.015);
  reseposx1->GetXaxis()->SetLabelSize(0.040);
  reseposx1->GetYaxis()->SetLabelOffset(0.0025);
  reseposx1->GetYaxis()->SetLabelSize(0.045);
  reseposx1->GetXaxis()->SetTitle("gap position");
  reseposx1->GetXaxis()->SetTitleSize(0.06);
  reseposx1->GetXaxis()->CenterTitle();
  reseposx1->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx1->GetYaxis()->SetTitleSize(0.06);
  reseposx1->GetYaxis()->SetTitleOffset(1.20);
  reseposx1->GetYaxis()->CenterTitle();
  reseposx1->SetLineColor(1);
  reseposx1->SetLineWidth(1);
  reseposx1->Draw("colz"); 
  tx1 = new TLatex(0.78,0.75,"X1");
  tx1->SetNDC();
  tx1->SetTextColor(1);
  tx1->SetTextSize(0.06);
  tx1->Draw();
  c0->cd(6);
  reseposx2->GetXaxis()->SetLabelOffset(0.015);
  reseposx2->GetXaxis()->SetLabelSize(0.040);
  reseposx2->GetYaxis()->SetLabelOffset(0.0025);
  reseposx2->GetYaxis()->SetLabelSize(0.045);
  reseposx2->GetXaxis()->SetTitle("gap position");
  reseposx2->GetXaxis()->SetTitleSize(0.06);
  reseposx2->GetXaxis()->CenterTitle();
  reseposx2->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx2->GetYaxis()->SetTitleSize(0.06);
  reseposx2->GetYaxis()->SetTitleOffset(1.20);
  reseposx2->GetYaxis()->CenterTitle();
  reseposx2->SetLineColor(1);
  reseposx2->SetLineWidth(1);
  reseposx2->Draw("colz");
  tx2 = new TLatex(0.78,0.78,"X5");
  tx2->SetNDC();
  tx2->SetTextColor(1);
  tx2->SetTextSize(0.06);
  tx2->Draw();
  c0->cd(3);
  reseposx3->GetXaxis()->SetLabelOffset(0.015);
  reseposx3->GetXaxis()->SetLabelSize(0.040);
  reseposx3->GetYaxis()->SetLabelOffset(0.0025);
  reseposx3->GetYaxis()->SetLabelSize(0.045);
  reseposx3->GetXaxis()->SetTitleSize(0.06);
  reseposx3->GetXaxis()->SetTitle("gap position");
  reseposx3->GetYaxis()->SetTitleSize(0.06);
  reseposx3->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx3->GetYaxis()->SetTitleOffset(1.20);
  reseposx3->GetXaxis()->CenterTitle();
  reseposx3->GetYaxis()->CenterTitle();
  reseposx3->SetLineColor(1);
  reseposx3->SetLineWidth(1);
  reseposx3->Draw("colz");
  tx3 = new TLatex(0.78,0.75,"X2");
  tx3->SetNDC();
  tx3->SetTextColor(1);
  tx3->SetTextSize(0.06);
  tx3->Draw();
  txangle = new TLatex(0.76,0.65,"30^{o}");
  txangle->SetNDC();
  txangle->SetTextColor(1);
  txangle->SetTextSize(0.06);
  txangle->Draw();

  TCanvas* c1=new TCanvas("c1","residus et pos y",0,0,650,650);
  c1->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 60067 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  c1->Divide(2,3,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.12);
  c1_1->SetBottomMargin(0.18);
  c1_1->SetRightMargin(0.05);
  c1_1->SetTopMargin(0.05);
  c1_1->SetLogz(1);
  c1_2->SetFillColor(10);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.12);
  c1_2->SetBottomMargin(0.18);
  c1_2->SetRightMargin(0.05);
  c1_2->SetTopMargin(0.05);
  c1_2->SetLogz(1);
  c1_3->SetFillColor(10);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.12);
  c1_3->SetBottomMargin(0.18);
  c1_3->SetRightMargin(0.05);
  c1_3->SetTopMargin(0.05);
  c1_3->SetLogz(1);
  c1_4->SetFillColor(10);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.12);
  c1_4->SetBottomMargin(0.18);
  c1_4->SetRightMargin(0.05);
  c1_4->SetTopMargin(0.05);
  c1_4->SetLogz(1);
  c1_5->SetFillColor(10);
  c1_5->Range(-10,-10,10,10);
  c1_5->SetLeftMargin(0.12);
  c1_5->SetBottomMargin(0.18);
  c1_5->SetRightMargin(0.05);
  c1_5->SetTopMargin(0.05);
  c1_5->SetLogz(1);
  c1_6->SetFillColor(10);
  c1_6->Range(-10,-10,10,10);
  c1_6->SetLeftMargin(0.12);
  c1_6->SetBottomMargin(0.18);
  c1_6->SetRightMargin(0.05);
  c1_6->SetTopMargin(0.05);
  c1_6->SetLogz(1);
  c1->cd(1);
  reseposy5->GetXaxis()->SetLabelOffset(0.015);
  reseposy5->GetXaxis()->SetLabelSize(0.040);
  reseposy5->GetYaxis()->SetLabelOffset(0.0025);
  reseposy5->GetYaxis()->SetLabelSize(0.050);
  reseposy5->GetXaxis()->SetTitle("gap position");
  reseposy5->GetXaxis()->SetTitleSize(0.06);
  reseposy5->GetXaxis()->CenterTitle();
  reseposy5->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy5->GetYaxis()->SetTitleSize(0.06);
  reseposy5->GetYaxis()->SetTitleOffset(1.20);
  reseposy5->GetYaxis()->CenterTitle();
  reseposy5->SetLineColor(1);
  reseposy5->SetLineWidth(1);
  reseposy5->Draw("colz");
  ty5 = new TLatex(0.78,0.78,"Y0");
  ty5->SetNDC();
  ty5->SetTextColor(1);
  ty5->SetTextSize(0.06);
  ty5->Draw();
  c1->cd(5);
  reseposy4->GetXaxis()->SetLabelOffset(0.015);
  reseposy4->GetXaxis()->SetLabelSize(0.040);
  reseposy4->GetYaxis()->SetLabelOffset(0.0025);
  reseposy4->GetYaxis()->SetLabelSize(0.050);
  reseposy4->GetXaxis()->SetTitle("gap position");
  reseposy4->GetXaxis()->SetTitleSize(0.06);
  reseposy4->GetXaxis()->CenterTitle();
  reseposy4->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy4->GetYaxis()->SetTitleSize(0.06);
  reseposy4->GetYaxis()->SetTitleOffset(1.20);
  reseposy4->GetYaxis()->CenterTitle();
  reseposy4->SetLineColor(1);
  reseposy4->SetLineWidth(1);
  reseposy4->Draw("colz");
  ty4 = new TLatex(0.78,0.78,"Y4");
  ty4->SetNDC();
  ty4->SetTextColor(1);
  ty4->SetTextSize(0.06);
  ty4->Draw();
  c1->cd(4);
  reseposy0->GetXaxis()->SetLabelOffset(0.015);
  reseposy0->GetXaxis()->SetLabelSize(0.040);
  reseposy0->GetYaxis()->SetLabelOffset(0.0025);
  reseposy0->GetYaxis()->SetLabelSize(0.050);
  reseposy0->GetXaxis()->SetTitle("gap position");
  reseposy0->GetXaxis()->SetTitleSize(0.06);
  reseposy0->GetXaxis()->CenterTitle();
  reseposy0->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy0->GetYaxis()->SetTitleSize(0.06);
  reseposy0->GetYaxis()->SetTitleOffset(1.20);
  reseposy0->GetYaxis()->CenterTitle();
  reseposy0->SetLineColor(1);
  reseposy0->SetLineWidth(1);
  reseposy0->Draw("colz");
  ty0 = new TLatex(0.78,0.78,"Y3");
  ty0->SetNDC();
  ty0->SetTextColor(1);
  ty0->SetTextSize(0.06);
  ty0->Draw();
  c1->cd(2);
  reseposy1->GetXaxis()->SetLabelOffset(0.015);
  reseposy1->GetXaxis()->SetLabelSize(0.040);
  reseposy1->GetYaxis()->SetLabelOffset(0.0025);
  reseposy1->GetYaxis()->SetLabelSize(0.050);
  reseposy1->GetXaxis()->SetTitle("gap position");
  reseposy1->GetXaxis()->SetTitleSize(0.06);
  reseposy1->GetXaxis()->CenterTitle();
  reseposy1->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy1->GetYaxis()->SetTitleSize(0.06);
  reseposy1->GetYaxis()->CenterTitle();
  reseposy1->SetLineColor(1);
  reseposy1->SetLineWidth(1);
  reseposy1->Draw("colz");
  ty1 = new TLatex(0.78,0.75,"Y1");
  ty1->SetNDC();
  ty1->SetTextColor(1);
  ty1->SetTextSize(0.06);
  ty1->Draw(); 
  c1->cd(6);
  reseposy2->GetXaxis()->SetLabelOffset(0.015);
  reseposy2->GetXaxis()->SetLabelSize(0.040);
  reseposy2->GetYaxis()->SetLabelOffset(0.0025);
  reseposy2->GetYaxis()->SetLabelSize(0.050);
  reseposy2->GetXaxis()->SetTitle("gap position");
  reseposy2->GetXaxis()->SetTitleSize(0.06);
  reseposy2->GetXaxis()->CenterTitle();
  reseposy2->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy2->GetYaxis()->SetTitleSize(0.06);
  reseposy2->GetYaxis()->CenterTitle();
  reseposy2->SetLineColor(1);
  reseposy2->SetLineWidth(1);
  reseposy2->Draw("colz");
  ty2 = new TLatex(0.78,0.78,"Y5");
  ty2->SetNDC();
  ty2->SetTextColor(1);
  ty2->SetTextSize(0.06);
  ty2->Draw();
  c1->cd(3);
  reseposy3->GetXaxis()->SetLabelOffset(0.015);
  reseposy3->GetXaxis()->SetLabelSize(0.040);
  reseposy3->GetYaxis()->SetLabelOffset(0.0025);
  reseposy3->GetYaxis()->SetLabelSize(0.050);
  reseposy3->GetXaxis()->SetTitle("gap position");
  reseposy3->GetXaxis()->SetTitleSize(0.06);
  reseposy3->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy3->GetYaxis()->SetTitleSize(0.06);
  reseposy3->GetXaxis()->CenterTitle();
  reseposy3->GetYaxis()->CenterTitle();
  reseposy3->SetLineColor(1);
  reseposy3->SetLineWidth(1);
  reseposy3->Draw("colz");
  ty3 = new TLatex(0.78,0.75,"Y2");
  ty3->SetNDC();
  ty3->SetTextColor(1);
  ty3->SetTextSize(0.06);
  ty3->Draw();
  tyangle = new TLatex(0.78,0.65,"0^{o}");
  tyangle->SetNDC();
  tyangle->SetTextColor(1);
  tyangle->SetTextSize(0.06);
  tyangle->Draw();

  //  TCanvas* cc=new TCanvas("cc","void",0,0,500,500);
  //  cc->SetFillColor(10);

  return;
}

void residual (char* hist) {

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);

  TF1 *gau2x = new TF1("gau2x","gaus(0)+gaus(3)",-0.025,0.025);
  gau2x->SetLineWidth(2);
  gau2x->SetLineColor(1);
  gau2x->SetLineStyle(1);
  TF1 *gau2y = new TF1("gau2y","gaus(0)+gaus(3)",-0.05,0.05);
  gau2y->SetLineWidth(2);
  gau2y->SetLineColor(1);
  gau2y->SetLineStyle(1);

  TCanvas* c0=new TCanvas("c0","residus",0,0,650,650);
  c0->SetFillColor(10);

  TH1D *hres = (TH1D*) f->Get(hist)->Clone();
  hres->Draw();
  gau2y->SetParameters(80.,0.000,0.0020,5.,0.,0.0050); 
  Double_t par[9];
  hres->Fit(gau2y,"V+");
  gau2y->GetParameters(par);
  printf("para %f %f %f %f %f %f\n",par[0],par[1],par[2],par[3],par[4],par[5]);
  return;

}

void residpos()
{
  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","residus et pos x",0,0,650,650);
  c0->SetFillColor(10);
  c0->Divide(2,2,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->SetFillStyle(4000);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.15);
  c0_1->SetBottomMargin(0.12);
  c0_1->SetRightMargin(0.15);
  c0_1->SetTopMargin(0.15);
  c0_1->SetLogz(1);
  c0_2->SetFillColor(10);
  c0_2->SetFillStyle(4000);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.15);
  c0_2->SetBottomMargin(0.12);
  c0_2->SetRightMargin(0.15);
  c0_2->SetTopMargin(0.15);
  c0_2->SetLogz(1);
  c0_3->SetFillColor(10);
  c0_3->SetFillStyle(4000);
  c0_3->Range(-10,-10,10,10);
  c0_3->SetLeftMargin(0.15);
  c0_3->SetBottomMargin(0.15);
  c0_3->SetRightMargin(0.15);
  c0_3->SetTopMargin(0.12);
  c0_3->SetLogz(1);
  c0_4->SetFillColor(10);
  c0_4->SetFillStyle(4000);
  c0_4->Range(-10,-10,10,10);
  c0_4->SetLeftMargin(0.15);
  c0_4->SetBottomMargin(0.15);
  c0_4->SetRightMargin(0.15);
  c0_4->SetTopMargin(0.12);
  c0_4->SetLogz(1);
  c0->cd(3);
  reseposx0->GetXaxis()->SetLabelOffset(0.015);
  reseposx0->GetXaxis()->SetLabelSize(0.040);
  reseposx0->GetYaxis()->SetLabelOffset(0.0025);
  reseposx0->GetYaxis()->SetLabelSize(0.045);
  //  resx0->GetXaxis()->SetRangeUser(-0.02,0.02);
  reseposx0->GetXaxis()->SetTitle("gap position");
  reseposx0->GetXaxis()->SetTitleSize(0.06);
  reseposx0->GetXaxis()->CenterTitle();
  reseposx0->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx0->GetYaxis()->SetTitleSize(0.06);
  reseposx0->GetYaxis()->SetTitleOffset(1.20);
  reseposx0->GetYaxis()->CenterTitle();
  reseposx0->SetLineColor(1);
  reseposx0->SetLineWidth(1);
  reseposx0->Draw("colz"); 
  tx0 = new TLatex(0.78,0.78,"X3");
  tx0->SetNDC();
  tx0->SetTextColor(1);
  tx0->SetTextSize(0.06);
  tx0->Draw();
  c0->cd(1);
  reseposx1->GetXaxis()->SetLabelOffset(0.015);
  reseposx1->GetXaxis()->SetLabelSize(0.040);
  reseposx1->GetYaxis()->SetLabelOffset(0.0025);
  reseposx1->GetYaxis()->SetLabelSize(0.045);
  reseposx1->GetXaxis()->SetTitle("gap position");
  reseposx1->GetXaxis()->SetTitleSize(0.06);
  reseposx1->GetXaxis()->CenterTitle();
  reseposx1->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx1->GetYaxis()->SetTitleSize(0.06);
  reseposx1->GetYaxis()->SetTitleOffset(1.20);
  reseposx1->GetYaxis()->CenterTitle();
  reseposx1->SetLineColor(1);
  reseposx1->SetLineWidth(1);
  reseposx1->Draw("colz"); 
  tx1 = new TLatex(0.78,0.75,"X1");
  tx1->SetNDC();
  tx1->SetTextColor(1);
  tx1->SetTextSize(0.06);
  tx1->Draw();
  c0->cd(4);
  reseposx2->GetXaxis()->SetLabelOffset(0.015);
  reseposx2->GetXaxis()->SetLabelSize(0.040);
  reseposx2->GetYaxis()->SetLabelOffset(0.0025);
  reseposx2->GetYaxis()->SetLabelSize(0.045);
  reseposx2->GetXaxis()->SetTitle("gap position");
  reseposx2->GetXaxis()->SetTitleSize(0.06);
  reseposx2->GetXaxis()->CenterTitle();
  reseposx2->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx2->GetYaxis()->SetTitleSize(0.06);
  reseposx2->GetYaxis()->SetTitleOffset(1.20);
  reseposx2->GetYaxis()->CenterTitle();
  reseposx2->SetLineColor(1);
  reseposx2->SetLineWidth(1);
  reseposx2->Draw("colz");
  tx2 = new TLatex(0.78,0.78,"X5");
  tx2->SetNDC();
  tx2->SetTextColor(1);
  tx2->SetTextSize(0.06);
  tx2->Draw();
  c0->cd(2);
  reseposx3->GetXaxis()->SetLabelOffset(0.015);
  reseposx3->GetXaxis()->SetLabelSize(0.040);
  reseposx3->GetYaxis()->SetLabelOffset(0.0025);
  reseposx3->GetYaxis()->SetLabelSize(0.045);
  reseposx3->GetXaxis()->SetTitleSize(0.06);
  reseposx3->GetXaxis()->SetTitle("gap position");
  reseposx3->GetYaxis()->SetTitleSize(0.06);
  reseposx3->GetYaxis()->SetTitle("#DeltaX (mm)");
  reseposx3->GetYaxis()->SetTitleOffset(1.20);
  reseposx3->GetXaxis()->CenterTitle();
  reseposx3->GetYaxis()->CenterTitle();
  reseposx3->SetLineColor(1);
  reseposx3->SetLineWidth(1);
  reseposx3->Draw("colz");
  tx3 = new TLatex(0.78,0.75,"X2");
  tx3->SetNDC();
  tx3->SetTextColor(1);
  tx3->SetTextSize(0.06);
  tx3->Draw();
  txangle = new TLatex(0.76,0.65,"30^{o}");
  txangle->SetNDC();
  txangle->SetTextColor(1);
  txangle->SetTextSize(0.06);
  txangle->Draw();

  TCanvas* c1=new TCanvas("c1","residus et pos y",0,0,650,650);
  c1->SetFillColor(10);
  tscan = new TLatex(0.15,0.95,"Run 60067 4 point fit residual distributions");
  tscan->SetNDC();
  tscan->SetTextSize(0.040);
  //  tscan->Draw();
  c1->Divide(2,2,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->SetFillStyle(4000);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.15);
  c1_1->SetBottomMargin(0.12);
  c1_1->SetRightMargin(0.15);
  c1_1->SetTopMargin(0.15);
  c1_1->SetLogz(1);
  c1_2->SetFillColor(10);
  c1_2->SetFillStyle(4000);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.15);
  c1_2->SetBottomMargin(0.12);
  c1_2->SetRightMargin(0.15);
  c1_2->SetTopMargin(0.15);
  c1_2->SetLogz(1);
  c1_3->SetFillColor(10);
  c1_3->SetFillStyle(4000);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.15);
  c1_3->SetBottomMargin(0.15);
  c1_3->SetRightMargin(0.15);
  c1_3->SetTopMargin(0.12);
  c1_3->SetLogz(1);
  c1_4->SetFillColor(10);
  c1_4->SetFillStyle(4000);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.15);
  c1_4->SetBottomMargin(0.15);
  c1_4->SetRightMargin(0.15);
  c1_4->SetTopMargin(0.12);
  c1_4->SetLogz(1);
  c1->cd(3);
  reseposy0->GetXaxis()->SetLabelOffset(0.015);
  reseposy0->GetXaxis()->SetLabelSize(0.040);
  reseposy0->GetYaxis()->SetLabelOffset(0.0025);
  reseposy0->GetYaxis()->SetLabelSize(0.050);
  reseposy0->GetXaxis()->SetTitle("gap position");
  reseposy0->GetXaxis()->SetTitleSize(0.06);
  reseposy0->GetXaxis()->CenterTitle();
  reseposy0->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy0->GetYaxis()->SetTitleSize(0.06);
  reseposy0->GetYaxis()->SetTitleOffset(1.20);
  reseposy0->GetYaxis()->CenterTitle();
  reseposy0->SetLineColor(1);
  reseposy0->SetLineWidth(1);
  reseposy0->Draw("colz");
  ty0 = new TLatex(0.78,0.78,"Y3");
  ty0->SetNDC();
  ty0->SetTextColor(1);
  ty0->SetTextSize(0.06);
  ty0->Draw();
  c1->cd(1);
  reseposy1->GetXaxis()->SetLabelOffset(0.015);
  reseposy1->GetXaxis()->SetLabelSize(0.040);
  reseposy1->GetYaxis()->SetLabelOffset(0.0025);
  reseposy1->GetYaxis()->SetLabelSize(0.050);
  reseposy1->GetXaxis()->SetTitle("gap position");
  reseposy1->GetXaxis()->SetTitleSize(0.06);
  reseposy1->GetXaxis()->CenterTitle();
  reseposy1->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy1->GetYaxis()->SetTitleSize(0.06);
  reseposy1->GetYaxis()->CenterTitle();
  reseposy1->SetLineColor(1);
  reseposy1->SetLineWidth(1);
  reseposy1->Draw("colz");
  ty1 = new TLatex(0.78,0.75,"Y1");
  ty1->SetNDC();
  ty1->SetTextColor(1);
  ty1->SetTextSize(0.06);
  ty1->Draw(); 
  c1->cd(4);
  reseposy2->GetXaxis()->SetLabelOffset(0.015);
  reseposy2->GetXaxis()->SetLabelSize(0.040);
  reseposy2->GetYaxis()->SetLabelOffset(0.0025);
  reseposy2->GetYaxis()->SetLabelSize(0.050);
  reseposy2->GetXaxis()->SetTitle("gap position");
  reseposy2->GetXaxis()->SetTitleSize(0.06);
  reseposy2->GetXaxis()->CenterTitle();
  reseposy2->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy2->GetYaxis()->SetTitleSize(0.06);
  reseposy2->GetYaxis()->CenterTitle();
  reseposy2->SetLineColor(1);
  reseposy2->SetLineWidth(1);
  reseposy2->Draw("colz");
  ty2 = new TLatex(0.78,0.78,"Y5");
  ty2->SetNDC();
  ty2->SetTextColor(1);
  ty2->SetTextSize(0.06);
  ty2->Draw();
  c1->cd(2);
  reseposy3->GetXaxis()->SetLabelOffset(0.015);
  reseposy3->GetXaxis()->SetLabelSize(0.040);
  reseposy3->GetYaxis()->SetLabelOffset(0.0025);
  reseposy3->GetYaxis()->SetLabelSize(0.050);
  reseposy3->GetXaxis()->SetTitle("gap position");
  reseposy3->GetXaxis()->SetTitleSize(0.06);
  reseposy3->GetYaxis()->SetTitle("#DeltaY (mm)");
  reseposy3->GetYaxis()->SetTitleSize(0.06);
  reseposy3->GetXaxis()->CenterTitle();
  reseposy3->GetYaxis()->CenterTitle();
  reseposy3->SetLineColor(1);
  reseposy3->SetLineWidth(1);
  reseposy3->Draw("colz");
  ty3 = new TLatex(0.78,0.75,"Y2");
  ty3->SetNDC();
  ty3->SetTextColor(1);
  ty3->SetTextSize(0.06);
  ty3->Draw();
  tyangle = new TLatex(0.78,0.65,"0^{o}");
  tyangle->SetNDC();
  tyangle->SetTextColor(1);
  tyangle->SetTextSize(0.06);
  tyangle->Draw();

  //  TCanvas* cc=new TCanvas("cc","void",0,0,500,500);
  //  cc->SetFillColor(10);

  return;
}

void ecrire_dxdz_dydz(int run, int npiste) {

  char cfich[80];
  if (npiste <= 2)
  sprintf(cfich,"Hist_%d_a6_1_2_6_etan.root",run);
  else 
  sprintf(cfich,"Hist_%d_a6_1_2_6_%dp_etan.root",run,npiste);
  printf("cfich %s\n",cfich);

  TFile *f = new TFile(cfich);

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);

  TCanvas* c2=new TCanvas("c2","diver",0,0,500,500);
  c2->Divide(1,2,0.0001,0.0001);
  c2_1->SetFillColor(10);
  c2_1->Range(-10,-10,10,10);
  c2_1->SetLeftMargin(0.12);
  c2_1->SetBottomMargin(0.18);
  c2_1->SetRightMargin(0.05);
  c2_1->SetTopMargin(0.05);
  c2_2->SetFillColor(10);
  c2_2->Range(-10,-10,10,10);
  c2_2->SetLeftMargin(0.12);
  c2_2->SetBottomMargin(0.18);
  c2_2->SetRightMargin(0.05);
  c2_2->SetTopMargin(0.05);
  Double_t par[6];
  TF1 *gauxz = new TF1("gauxz","gaus(0)+gaus(3)",-0.0003,0.00003);
  gauxz->SetLineWidth(1);
  gauxz->SetLineColor(1);
  gauxz->SetLineStyle(1);
  gauxz->SetParameters(5000.,0.,0.00003,500.,0.,0.0001);
  if (run >= 1500 && run <= 1520)
  gauxz->SetParameters(400.,-0.000075,0.00003,200.,0.00005,0.0001);
  gauxz->GetParameters(par);
  for (int i=0; i<6; i++) printf("i %d par %f\n",i,par[i]);
  TF1 *gauyz = new TF1("gauyz","gaus(0)+gaus(3)",-0.0003,0.00003);
  gauyz->SetLineWidth(1);
  gauyz->SetLineColor(1);
  gauyz->SetLineStyle(1);
  sprintf(cfich,"diver_%d.dat",run);
  FILE *fdiverf = fopen(cfich,"w");
  c2->cd(1);
  dxdz_fais->SetLineColor(2);
  dxdz_fais->Fit("gauxz");
  dxdz_fais->Draw("hist");
  gauxz->Draw("same");
  gauxz->GetParameters(par);
  float atot = par[0]*par[2] + par[3]*par[5];
  printf("dxdz\n");
  printf("sig1 %f centre %f (urad) frac %f\n",1000000.*par[2],
	 1000000.*par[1],par[0]*par[2]/atot); 
  printf("sig2 %f centre %f (urad) frac %f\n",1000000.*par[5],
	 1000000.*par[4],par[3]*par[5]/atot); 
  fprintf(fdiverf,"dxdz\n");
  fprintf(fdiverf,"sig1 %7.3f centre %7.3f (urad) frac %5.4f\n",1000000.*par[2],
	 1000000.*par[1],par[0]*par[2]/atot); 
  fprintf(fdiverf,"sig2 %7.3f centre %7.3f (urad) frac %5.4f\n",1000000.*par[5],
	 1000000.*par[4],par[3]*par[5]/atot); 
  c2->cd(2);
  dydz_fais->SetLineColor(4);
  //  gauyz->SetParameters(2000.,0.00000,0.00003,200.,0.00004,0.00003);
  gauyz->SetParameters(1500.,0.,0.00004,1000.,0.,0.00004);
  if (run >= 1500 && run <= 1520)
  gauyz->SetParameters(200.,-0.0005,0.000100,200.,0.0001,0.000100);
  //  gauyz->SetParameters(1500.,0.,0.00004,1000.,0.,0.00004);
  dydz_fais->Fit("gauyz");
  dydz_fais->Draw("hist");
  gauyz->Draw("same");
  gauyz->GetParameters(par);
  atot = par[0]*par[2] + par[3]*par[5];
  printf("dydz\n");
  printf("sig1 %f centre %f (urad) frac %f\n",1000000.*par[2],
	 1000000.*par[1],par[0]*par[2]/atot); 
  printf("sig2 %f centre %f (urad) frac %f\n",1000000.*par[5],
	 1000000.*par[4],par[3]*par[5]/atot); 
  fprintf(fdiverf,"dydz\n");
  fprintf(fdiverf,"sig1 %7.3f centre %7.3f (urad) frac %5.4f\n",1000000.*par[2],
	 1000000.*par[1],par[0]*par[2]/atot); 
  fprintf(fdiverf,"sig2 %7.3f centre %7.3f (urad) frac %5.4f\n",1000000.*par[5],
	 1000000.*par[4],par[3]*par[5]/atot); 
  fclose(fdiverf);

  int nx = dxdz_fais->GetNbinsX();
  printf("nx %d\n",nx);
  //  for (int i=0; i<nx+2; i++)
  //    printf("i %d centre %f cont %f\n",i,dxdz_fais->GetBinCenter(i),dxdz_fais->GetBinContent(i));
  sprintf(cfich,"diver_%d_2007.dat",run);
  FILE *fdiver = fopen(cfich,"w");
  fprintf(fdiver,"distribution dxdz_fais nbins\n");
  fprintf(fdiver,"%d\n",nx);
  for (int i=1; i<=nx; i++)
    fprintf(fdiver,"centre %f contenu %f\n",dxdz_fais->GetBinCenter(i),dxdz_fais->GetBinContent(i));
  int ny = dydz_fais->GetNbinsX();
  printf("ny %d\n",ny);
  //  for (int i=0; i<ny+2; i++)
  //    printf("i %d centre %f cont %f\n",i,dydz_fais->GetBinCenter(i),dydz_fais->GetBinContent(i));
  fprintf(fdiver,"distribution dydz_fais nbins\n");
  fprintf(fdiver,"%d\n",ny);
  for (int i=1; i<=ny; i++)
    fprintf(fdiver,"centre %f contenu %f\n",dydz_fais->GetBinCenter(i),dydz_fais->GetBinContent(i));
  fclose(fdiver);
  f->cd();

  return;
}

void diver() {
  char cproj[20];
  char ccanv[20];

  /* par la position dans le faisceau */
  float fac = 1000000.;
  float zpos[6] = { 0.0, 56.0, 384.0, 757.0, 852.0, 945.0 };
  float resx[6] = { 0.00075, 0.0015, 0.0015, 0.0015, 0.00075, 0.0015 };
  float resy[6] = { 15.0, 30.0, 30.0, 30.0, 15.0, 30.0 }; 
  int paire[15][2] = { {0,1}, {0,2}, {0,3}, {0,4}, {0,5},
                       {1,2}, {1,3}, {1,4}, {1,5},
                       {2,3}, {2,4}, {2,5},
                       {3,4}, {3,5}, {4,5} };
  float mcs_si_300um = 0.0000015; /* pour 400 GeV protons (rad) */
  float div_horz = 0.000010;
  float div_vert = 0.000010;

  float par_dxdz[15][8];

  TF1 *gau2xz = new TF1("gau2xz","gaus(0)+gaus(3)",-0.0003,0.00003);
  gau2xz->SetLineWidth(1);
  gau2xz->SetLineColor(1);
  gau2xz->SetLineStyle(1);
  TF1 *gau2yz = new TF1("gau2yz","gaus(0)+gaus(3)",-0.0003,0.0003);
  gau2yz->SetLineWidth(1);
  gau2yz->SetLineColor(1);
  gau2yz->SetLineStyle(1);

  TLatex *tcomb[15];
  for (int i=0; i<15; i++) {
    printf("i %d\n",i);
    switch(i) {
    case 0: tcomb[i] = new TLatex(0.70,0.75,"0-1"); break;
    case 1: tcomb[i] = new TLatex(0.70,0.75,"0-2"); break;
    case 2: tcomb[i] = new TLatex(0.70,0.75,"0-3"); break;
    case 3: tcomb[i] = new TLatex(0.70,0.75,"0-4"); break;
    case 4: tcomb[i] = new TLatex(0.70,0.75,"0-5"); break;
    case 5: tcomb[i] = new TLatex(0.70,0.75,"1-2"); break;
    case 6: tcomb[i] = new TLatex(0.70,0.75,"1-3"); break;
    case 7: tcomb[i] = new TLatex(0.70,0.75,"1-4"); break;
    case 8: tcomb[i] = new TLatex(0.70,0.75,"1-5"); break;
    case 9: tcomb[i] = new TLatex(0.70,0.75,"2-3"); break;
    case 10: tcomb[i] = new TLatex(0.70,0.75,"2-4"); break;
    case 11: tcomb[i] = new TLatex(0.70,0.75,"2-5"); break;
    case 12: tcomb[i] = new TLatex(0.70,0.75,"3-4"); break;
    case 13: tcomb[i] = new TLatex(0.70,0.75,"3-5"); break;
    case 14: tcomb[i] = new TLatex(0.70,0.75,"4-5"); break;
    default:
    }
    tcomb[i]->SetNDC();
    tcomb[i]->SetTextColor(1);
    tcomb[i]->SetTextSize(0.070);
  }

  TH1D* dxdz_proy[15];
  for (int ib=1; ib<16; ib++) {
    sprintf(cproj,"prox_%d",ib);
    dxdz_proy[ib-1] = dxdz->ProjectionY(cproj,ib,ib,"");
    printf(" bin %d moyen %f rms\n",ib,fac*dxdz_proy[ib-1]->GetMean(),fac*dxdz_proy[ib-1]->GetRMS());
  }

  TCanvas* c3=new TCanvas("c3","dxdz proy",0,0,800,800);
  c3->SetFillColor(10);
  c3->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<15; i++) { 
    switch(i) {
    case 0: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    case 1: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 2: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000040); break;
    case 3: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000040); break;
    case 4: gau2xz->SetParameters(5000.,0.000,0.000020,50.,0.,0.000060); break;
    case 5: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 6: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 7: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 8: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 9: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 10: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 11: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 12: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    case 13: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    case 14: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    default: return;
    }
     c3->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(1);
      dxdz_proy[i]->SetLineColor(2);
      dxdz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      dxdz_proy[i]->GetXaxis()->SetLabelSize(0.050);
      dxdz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      dxdz_proy[i]->GetYaxis()->SetLabelSize(0.060);
      dxdz_proy[i]->GetXaxis()->SetTitle("dx/dz (rad)");
      dxdz_proy[i]->GetXaxis()->SetTitleSize(0.06);
      //      dxdz_proy[i]->GetYaxis()->SetTitle("#DeltaY (mm)");
      //      dxdz_proy[i]->GetYaxis()->SetTitleSize(0.06);
      dxdz_proy[i]->GetXaxis()->CenterTitle();
      //      dxdz_proy[i]->GetYaxis()->CenterTitle();
      dxdz_proy[i]->Fit(gau2xz,"V+"); 
      Double_t par[6];
      gau2xz->GetParameters(par);
      for(int j=0; j<6; j++) par_dxdz[i][j]=par[j];
      //      printf("para %f %f %f %f %f %f\n",par[0],fac*par[1],fac*par[2],par[3],fac*par[4],fac*par[5]);
      //      dxdz_proy[i]->Draw("hist");
      tcomb[i]->Draw();
  }
  printf("--- divergences dxdz (urad) ---\n");
  for (int ib=0; ib<15; ib++) {
    float resxz = resx[paire[ib][0]]*resx[paire[ib][0]] + resx[paire[ib][1]]*resx[paire[ib][1]];
    resxz = sqrt(resxz);
    resxz = resxz/(zpos[paire[ib][1]]-zpos[paire[ib][0]]);
    int nsi = paire[ib][1] - paire[ib][0];
    if (nsi > 0) resxz = sqrt((resxz*resxz) + (nsi*mcs_si_300um*mcs_si_300um) + (div_vert*div_vert));
    float atot = par_dxdz[ib][0]*par_dxdz[ib][2] + par_dxdz[ib][3]*par_dxdz[ib][5];
    par_dxdz[ib][6] = par_dxdz[ib][0]*par_dxdz[ib][2]*par_dxdz[ib][2];
    par_dxdz[ib][6]+=par_dxdz[ib][3]*par_dxdz[ib][5]*par_dxdz[ib][5];
    par_dxdz[ib][6]/=atot; 
    if (par_dxdz[ib][2] <= par_dxdz[ib][5]) {
       par_dxdz[ib][7] = (par_dxdz[ib][0]*par_dxdz[ib][2])/atot;
    printf(" bin %d moyen %f rms %f sig1 %f sig12 %f frac %f est. %f rap %f\n",ib,fac*dxdz_proy[ib]->GetMean(),fac*dxdz_proy[ib]->GetRMS(),fac*par_dxdz[ib][2],fac*par_dxdz[ib][6],par_dxdz[ib][7],fac*resxz,par_dxdz[ib][2]/resxz);
    }
    else { 
       par_dxdz[ib][7] = (par_dxdz[ib][3]*par_dxdz[ib][5])/atot;
    printf(" bin %d moyen %f rms %f sig1 %f sig12 %f frac %f est. %f rap %f\n",ib,fac*dxdz_proy[ib]->GetMean(),fac*dxdz_proy[ib]->GetRMS(),fac*par_dxdz[ib][5],fac*par_dxdz[ib][6],par_dxdz[ib][7],fac*resxz,par_dxdz[ib][5]/resxz);
    }
  }

  TH1D* dydz_proy[15];
  printf("--- divergences dydz (urad)  ---\n");
  for (int ib=1; ib<16; ib++) {
    sprintf(cproj,"proy_%d",ib);
    dydz_proy[ib-1] = dydz->ProjectionY(cproj,ib,ib,"");
    printf(" bin %d moyen %f rms %f\n",ib,fac*dydz_proy[ib-1]->GetMean(),fac*dydz_proy[ib-1]->GetRMS());
  }

  TCanvas* c4=new TCanvas("c4","dydz proy",0,0,800,800);
  c4->SetFillColor(10);
  c4->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<15; i++) {
      c4->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(0);
      dydz_proy[i]->SetLineColor(4);
      dydz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      dydz_proy[i]->GetXaxis()->SetLabelSize(0.050);
      dydz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      dydz_proy[i]->GetYaxis()->SetLabelSize(0.050);
      dydz_proy[i]->GetXaxis()->SetTitle("dy/dz (rad)");
      dydz_proy[i]->GetXaxis()->SetTitleSize(0.06);
      //      dydz_proy[i]->GetYaxis()->SetTitle("#DeltaY (mm)");
      //      dydz_proy[i]->GetYaxis()->SetTitleSize(0.06);
      dydz_proy[i]->GetXaxis()->CenterTitle();
      //      dydz_proy[i]->GetYaxis()->CenterTitle();
      dydz_proy[i]->Draw("hist");
      tcomb[i]->Draw();
  }

  f->cd();
  TCanvas* c0=new TCanvas("c0","diver",0,0,500,500);
  c0->Divide(1,2,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.12);
  c0_1->SetBottomMargin(0.18);
  c0_1->SetRightMargin(0.05);
  c0_1->SetTopMargin(0.05);
  c0_2->SetFillColor(10);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.12);
  c0_2->SetBottomMargin(0.18);
  c0_2->SetRightMargin(0.05);
  c0_2->SetTopMargin(0.05);
  c0->cd(1);
  dxdz->Draw();
  c0->cd(2);
  dydz->Draw();

  TCanvas* c1=new TCanvas("c1","diver",0,0,500,500);
  c1->Divide(1,3,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.12);
  c1_1->SetBottomMargin(0.18);
  c1_1->SetRightMargin(0.05);
  c1_1->SetTopMargin(0.05);
  c1_2->SetFillColor(10);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.12);
  c1_2->SetBottomMargin(0.18);
  c1_2->SetRightMargin(0.05);
  c1_2->SetTopMargin(0.05);
  c1_3->SetFillColor(10);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.12);
  c1_3->SetBottomMargin(0.18);
  c1_3->SetRightMargin(0.05);
  c1_3->SetTopMargin(0.05);
  c1->cd(1);
  dydz_y1->Draw();
  c1->cd(2);
  dydz_y3->Draw();
  c1->cd(3);
  y1_y3->Draw();

  TCanvas* c2=new TCanvas("c2","diver",0,0,500,500);
  c2->Divide(1,2,0.0001,0.0001);
  c2_1->SetFillColor(10);
  c2_1->Range(-10,-10,10,10);
  c2_1->SetLeftMargin(0.12);
  c2_1->SetBottomMargin(0.18);
  c2_1->SetRightMargin(0.05);
  c2_1->SetTopMargin(0.05);
  c2_2->SetFillColor(10);
  c2_2->Range(-10,-10,10,10);
  c2_2->SetLeftMargin(0.12);
  c2_2->SetBottomMargin(0.18);
  c2_2->SetRightMargin(0.05);
  c2_2->SetTopMargin(0.05);
  TF1 *gauxz = new TF1("gauxz","gaus(0)",-0.0003,0.00003);
  gauxz->SetLineWidth(1);
  gauxz->SetLineColor(1);
  gauxz->SetLineStyle(1);
  gauxz->SetParameters(5000.,0.,0.00003);
  c2->cd(1);
  dxdz_fais->Fit("gauxz");
  c2->cd(2);
  dydz_fais->Draw("hist");

  /*  TCanvas* c3=new TCanvas("c3","y1_y3",0,0,500,500);
  c3->cd()
  y1_y3->Draw(); */

  int nx = dxdz_fais->GetNbinsX();
  printf("nx %d\n",nx);
  for (int i=0; i<nx+2; i++)
    printf("i %d centre %f cont %f\n",i,dxdz_fais->GetBinCenter(i),dxdz_fais->GetBinContent(i));
  FILE *fdiver = fopen("diver_2007_1234.dat","w");
  fprintf(fdiver,"distribution dxdz_fais nbins\n");
  fprintf(fdiver,"%d\n",nx);
  for (int i=1; i<=nx; i++)
    fprintf(fdiver,"centre %f contenu %f\n",dxdz_fais->GetBinCenter(i),dxdz_fais->GetBinContent(i));
  int ny = dydz_fais->GetNbinsX();
  printf("ny %d\n",ny);
  for (int i=0; i<ny+2; i++)
    printf("i %d centre %f cont %f\n",i,dydz_fais->GetBinCenter(i),dydz_fais->GetBinContent(i));
  fprintf(fdiver,"distribution dydz_fais nbins\n");
  fprintf(fdiver,"%d\n",ny);
  for (int i=1; i<=ny; i++)
    fprintf(fdiver,"centre %f contenu %f\n",dydz_fais->GetBinCenter(i),dydz_fais->GetBinContent(i));
  fclose(fdiver);
  f->cd();
   
  return;
}

void diver_1500s() {

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);
  char cproj[20];
  char ccanv[20];

  /* par la position dans le faisceau */
  float fac = 1000000.;
  float zpos[6] = { 0.0, 56.0, 384.0, 757.0, 852.0, 945.0 };
  float resx[6] = { 0.00075, 0.0015, 0.0015, 0.0015, 0.00075, 0.0015 };
  float resy[6] = { 15.0, 30.0, 30.0, 30.0, 15.0, 30.0 }; 
  int paire[15][2] = { {0,1}, {0,2}, {0,3}, {0,4}, {0,5},
                       {1,2}, {1,3}, {1,4}, {1,5},
                       {2,3}, {2,4}, {2,5},
                       {3,4}, {3,5}, {4,5} };
  float mcs_si_300um = 0.0000015; /* pour 400 GeV protons (rad) */
  float div_horz = 0.000010;
  float div_vert = 0.000010;

  float par_dxdz[15][8];

  TF1 *gau2xz = new TF1("gau2xz","gaus(0)+gaus(3)",-0.0003,0.00003);
  gau2xz->SetLineWidth(1);
  gau2xz->SetLineColor(1);
  gau2xz->SetLineStyle(1);
  TF1 *gau2yz = new TF1("gau2yz","gaus(0)+gaus(3)",-0.0003,0.0003);
  gau2yz->SetLineWidth(1);
  gau2yz->SetLineColor(1);
  gau2yz->SetLineStyle(1);

  TLatex *trun[16];
  char crun[5];
  for (int i=1500; i<1516; i++) {
    sprintf(crun,"%d",i);
    trun[i-1500] = new TLatex(0.70,0.75,crun);
    trun[i-1500]->SetNDC();
    trun[i-1500]->SetTextColor(1);
    trun[i-1500]->SetTextSize(0.070);
  }

  char cfich[100];
  TH1D* dxdz_proy[16];
  TH1D* dydz_proy[16];
  TH1D* dxdz_fais[16];
  TH1D* dydz_fais[16];
  for (int ib=1500; ib<1516; ib++) {
    sprintf(cfich,"Hist_%d_a6_1_2_6_etan.root",ib);
    TFile *f = new TFile(cfich);
    TH2D* dxdz_clone = f->Get("dxdz")->Clone();
    dxdz_fais[ib-1500] = (TH1D*) f->Get("dxdz_fais")->Clone();
    dydz_fais[ib-1500] = (TH1D*) f->Get("dydz_fais")->Clone();
    sprintf(cproj,"prox_%d",3);
    dxdz_proy[ib-1500] = dxdz_clone->ProjectionY(cproj,3,3,"");
    TH2D* dydz_clone = f->Get("dydz")->Clone();
    sprintf(cproj,"proy_%d",3);
    dydz_proy[ib-1500] = dydz_clone->ProjectionY(cproj,3,3,"");
    if (ib == 1507) {
      int nx = dxdz_proy[ib-1500]->GetNbinsX();
      printf("nx %d\n",nx);
      for (int i=0; i<nx+2; i++)
      printf("i %d centre %f cont %f\n",i,dxdz_proy[ib-1500]->GetBinCenter(i),dxdz_proy[ib-1500]->GetBinContent(i));
      FILE *fdiver = fopen("diver_2007_1507.dat","w");
      fprintf(fdiver,"distribution dxdz_ref12 nbins\n");
      fprintf(fdiver,"%d\n",nx);
      for (int i=1; i<=nx; i++)
        fprintf(fdiver,"centre %f contenu %f\n",dxdz_proy[ib-1500]->GetBinCenter(i),dxdz_proy[ib-1500]->GetBinContent(i));
      int ny = dydz_proy[ib-1500]->GetNbinsX();
      printf("ny %d\n",ny);
      for (int i=0; i<ny+2; i++)
        printf("i %d centre %f cont %f\n",i,dydz_proy[ib-1500]->GetBinCenter(i),dydz_proy[ib-1500]->GetBinContent(i));
      fprintf(fdiver,"distribution dydz_ref12 nbins\n");
      fprintf(fdiver,"%d\n",ny);
      for (int i=1; i<=ny; i++)
        fprintf(fdiver,"centre %f contenu %f\n",dydz_proy[ib-1500]->GetBinCenter(i),dydz_proy[ib-1500]->GetBinContent(i));
      fclose(fdiver);
    }
  }

  TCanvas* c1=new TCanvas("c1","dxdz proy",0,0,800,800);
  c1->SetFillColor(10);
  c1->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<16; i++) {
      c1->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(1);
      dxdz_proy[i]->SetLineColor(2);
      dxdz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      dxdz_proy[i]->GetXaxis()->SetLabelSize(0.050);
      dxdz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      dxdz_proy[i]->GetYaxis()->SetLabelSize(0.060);
      dxdz_proy[i]->GetXaxis()->SetTitle("dx/dz (rad)");
      dxdz_proy[i]->GetXaxis()->SetTitleSize(0.06);
      //      dxdz_proy[i]->GetYaxis()->SetTitle("#DeltaY (mm)");
      //      dxdz_proy[i]->GetYaxis()->SetTitleSize(0.06);
      dxdz_proy[i]->GetXaxis()->CenterTitle();
      //      dxdz_proy[i]->GetYaxis()->CenterTitle();
      //      dxdz_proy[i]->Fit(gau2xz,"V+");
      dxdz_proy[i]->Draw("hist"); 
      Double_t par[6];
      //      gau2xz->GetParameters(par);
      //      for(int j=0; j<6; j++) par_dxdz[i][j]=par[j];
      //      printf("para %f %f %f %f %f %f\n",par[0],fac*par[1],fac*par[2],par[3],fac*par[4],fac*par[5]);
      //      dxdz_proy[i]->Draw("hist");
      trun[i]->Draw();
  }

  printf("--- divergences dydz (urad)  ---\n");

  TCanvas* c2=new TCanvas("c2","dydz proy",0,0,800,800);
  c2->SetFillColor(10);
  c2->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<16; i++) {
      c2->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(0);
      dydz_proy[i]->SetLineColor(4);
      dydz_proy[i]->GetXaxis()->SetLabelOffset(0.020);
      dydz_proy[i]->GetXaxis()->SetLabelSize(0.050);
      dydz_proy[i]->GetYaxis()->SetLabelOffset(0.0025);
      dydz_proy[i]->GetYaxis()->SetLabelSize(0.050);
      dydz_proy[i]->GetXaxis()->SetTitle("dy/dz (rad)");
      dydz_proy[i]->GetXaxis()->SetTitleSize(0.06);
      //      dydz_proy[i]->GetYaxis()->SetTitle("#DeltaY (mm)");
      //      dydz_proy[i]->GetYaxis()->SetTitleSize(0.06);
      dydz_proy[i]->GetXaxis()->CenterTitle();
      //      dydz_proy[i]->GetYaxis()->CenterTitle();
      dydz_proy[i]->Draw("hist");
      trun[i]->Draw();
  }

  TCanvas* c3=new TCanvas("c3","dxdz fais",0,0,800,800);
  c3->SetFillColor(10);
  c3->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<16; i++) { 
    switch(i) {
    case 0: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    case 1: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 2: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000040); break;
    case 3: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000040); break;
    case 4: gau2xz->SetParameters(5000.,0.000,0.000020,50.,0.,0.000060); break;
    case 5: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 6: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 7: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 8: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 9: gau2xz->SetParameters(8000.,0.000,0.000020,50.,0.,0.000060); break;
    case 10: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 11: gau2xz->SetParameters(8000.,0.000,0.000020,20.,0.,0.000080); break;
    case 12: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    case 13: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    case 14: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    case 15: gau2xz->SetParameters(5000.,0.000,0.000050,100.,0.,0.00030); break;
    default: return;
    }
     c3->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(1);
      dxdz_fais[i]->SetLineColor(2);
      dxdz_fais[i]->GetXaxis()->SetLabelOffset(0.020);
      dxdz_fais[i]->GetXaxis()->SetLabelSize(0.050);
      dxdz_fais[i]->GetYaxis()->SetLabelOffset(0.0025);
      dxdz_fais[i]->GetYaxis()->SetLabelSize(0.060);
      dxdz_fais[i]->GetXaxis()->SetTitle("dx/dz (rad)");
      dxdz_fais[i]->GetXaxis()->SetTitleSize(0.06);
      //      dxdz_proy[i]->GetYaxis()->SetTitle("#DeltaY (mm)");
      //      dxdz_proy[i]->GetYaxis()->SetTitleSize(0.06);
      dxdz_fais[i]->GetXaxis()->CenterTitle();
      //      dxdz_proy[i]->GetYaxis()->CenterTitle();
      //      dxdz_proy[i]->Fit(gau2xz,"V+");
      dxdz_fais[i]->Draw("hist"); 
      Double_t par[6];
      //      gau2xz->GetParameters(par);
      //      for(int j=0; j<6; j++) par_dxdz[i][j]=par[j];
      //      printf("para %f %f %f %f %f %f\n",par[0],fac*par[1],fac*par[2],par[3],fac*par[4],fac*par[5]);
      //      dxdz_proy[i]->Draw("hist");
      trun[i]->Draw();
  }

  printf("--- divergences dydz (urad)  ---\n");

  TCanvas* c4=new TCanvas("c4","dydz fais",0,0,800,800);
  c4->SetFillColor(10);
  c4->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<16; i++) {
      c4->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(0);
      dydz_fais[i]->SetLineColor(4);
      dydz_fais[i]->GetXaxis()->SetLabelOffset(0.020);
      dydz_fais[i]->GetXaxis()->SetLabelSize(0.050);
      dydz_fais[i]->GetYaxis()->SetLabelOffset(0.0025);
      dydz_fais[i]->GetYaxis()->SetLabelSize(0.050);
      dydz_fais[i]->GetXaxis()->SetTitle("dy/dz (rad)");
      dydz_fais[i]->GetXaxis()->SetTitleSize(0.06);
      //      dydz_proy[i]->GetYaxis()->SetTitle("#DeltaY (mm)");
      //      dydz_proy[i]->GetYaxis()->SetTitleSize(0.06);
      dydz_fais[i]->GetXaxis()->CenterTitle();
      //      dydz_proy[i]->GetYaxis()->CenterTitle();
      dydz_fais[i]->Draw("hist");
      trun[i]->Draw();
  }

  return;
  f->cd();


   
  return;
}

void pos_hr_1500s(int det) {

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);

  TLatex *trun[16];
  char crun[5];
  for (int i=1500; i<1516; i++) {
    sprintf(crun,"%d",i);
    trun[i-1500] = new TLatex(0.70,0.75,crun);
    trun[i-1500]->SetNDC();
    trun[i-1500]->SetTextColor(1);
    trun[i-1500]->SetTextSize(0.070);
  }

  char cfich[100];
  char chist[20];
  TH1D* xhr[16];
  TH1D* x1p[16];
  TH1D* yhr[16];
  TH1D* y1p[16];
  for (int ib=1500; ib<1516; ib++) {
    sprintf(cfich,"Hist_%d_a6_1_2_6.root",ib);
    TFile *f = new TFile(cfich);
    sprintf(chist,"x%dhr",det);
    xhr[ib-1500] = (TH1D *) f->Get(chist)->Clone();
    sprintf(chist,"x%d1p",det);
    x1p[ib-1500] = (TH1D *) f->Get(chist)->Clone();
    sprintf(chist,"y%dhr",det);
    yhr[ib-1500] = (TH1D *)f->Get(chist)->Clone();
    sprintf(chist,"y%d1p",det);
    y1p[ib-1500] = (TH1D *)f->Get(chist)->Clone();
  }
  TCanvas* c1=new TCanvas("c1","xpos ref1",0,0,800,800);
  c1->SetFillColor(10);
  c1->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<16; i++) { 
      c1->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(0);
      xhr[i]->SetLineColor(2);
      xhr[i]->GetXaxis()->SetLabelOffset(0.020);
      xhr[i]->GetXaxis()->SetLabelSize(0.050);
      xhr[i]->GetYaxis()->SetLabelOffset(0.0025);
      xhr[i]->GetYaxis()->SetLabelSize(0.060);
      xhr[i]->GetXaxis()->SetTitle("y (mm)");
      xhr[i]->GetXaxis()->SetTitleSize(0.06);
      xhr[i]->GetXaxis()->CenterTitle();
      if (det < 4)
       xhr[i]->GetXaxis()->SetRangeUser(-0.3,0.3);
      xhr[i]->Draw("hist"); 
      x1p[i]->Draw("samehist");
      trun[i]->Draw();
  }

  TCanvas* c1=new TCanvas("c2","ypos ref1",0,0,800,800);
  c2->SetFillColor(10);
  c2->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<16; i++) { 
      c1->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.15);
      gPad->SetLogy(0);
      yhr[i]->SetLineColor(4);
      yhr[i]->GetXaxis()->SetLabelOffset(0.020);
      yhr[i]->GetXaxis()->SetLabelSize(0.050);
      yhr[i]->GetYaxis()->SetLabelOffset(0.0025);
      yhr[i]->GetYaxis()->SetLabelSize(0.060);
      yhr[i]->GetXaxis()->SetTitle("y (mm)");
      yhr[i]->GetXaxis()->SetTitleSize(0.06);
      yhr[i]->GetXaxis()->CenterTitle();
      if (det < 4)      
         yhr[i]->GetXaxis()->SetRangeUser(-0.3,0.3);
      yhr[i]->Draw("hist"); 
      trun[i]->Draw();
  }
   
  return;
}

void calib(int run) {

  char cfich[80];
  sprintf(cfich,"Hist_%d_a6_1_2_6_etan.root",run);
  printf("%s\n",cfich);
  TFile *f = new TFile(cfich);

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","piedestaux",0,0,650,650);
  c0->Divide(2,3,0.0001,0.0001);
  c0_1->SetFillColor(10);
  c0_1->Range(-10,-10,10,10);
  c0_1->SetLeftMargin(0.12);
  c0_1->SetBottomMargin(0.18);
  c0_1->SetRightMargin(0.05);
  c0_1->SetTopMargin(0.05);
  c0_2->SetFillColor(10);
  c0_2->Range(-10,-10,10,10);
  c0_2->SetLeftMargin(0.12);
  c0_2->SetBottomMargin(0.18);
  c0_2->SetRightMargin(0.05);
  c0_2->SetTopMargin(0.05);
  c0_3->SetFillColor(10);
  c0_3->Range(-10,-10,10,10);
  c0_3->SetLeftMargin(0.12);
  c0_3->SetBottomMargin(0.18);
  c0_3->SetRightMargin(0.05);
  c0_3->SetTopMargin(0.05);
  c0_4->SetFillColor(10);
  c0_4->Range(-10,-10,10,10);
  c0_4->SetLeftMargin(0.12);
  c0_4->SetBottomMargin(0.18);
  c0_4->SetRightMargin(0.05);
  c0_4->SetTopMargin(0.05);
  c0_5->SetFillColor(10);
  c0_5->Range(-10,-10,10,10);
  c0_5->SetLeftMargin(0.12);
  c0_5->SetBottomMargin(0.18);
  c0_5->SetRightMargin(0.05);
  c0_5->SetTopMargin(0.05);
  c0_6->SetFillColor(10);
  c0_6->Range(-10,-10,10,10);
  c0_6->SetLeftMargin(0.12);
  c0_6->SetBottomMargin(0.18);
  c0_6->SetRightMargin(0.05);
  c0_6->SetTopMargin(0.05);

  // lecture 0, position faisceau 3eme AMS02
  c0->cd(3);
  f->cd("0900");
  Piedestaux_0900->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0900->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0900->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0900->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0900->SetLineColor(1);
  Piedestaux_0900->Draw("hist");  
  // lecture 1, position faisceau 1ere AMS02
  c0->cd(1);
  f->cd("0901");
  Piedestaux_0901->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0901->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0901->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0901->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0901->SetLineColor(1);
  Piedestaux_0901->Draw("hist");
  // lecture 2, position faisceau 4eme AMS02
  c0->cd(4);
  f->cd("0904");
  Piedestaux_0904->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0904->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0904->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0904->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0904->SetLineColor(1);
  Piedestaux_0904->Draw("hist");
  // lecture 3, position faisceau 2eme AMS02
  c0->cd(2);
  f->cd("0905");
  Piedestaux_0905->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0905->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0905->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0905->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0905->SetLineColor(1);
  Piedestaux_0905->Draw("hist");
  c0->cd(5);
  f->cd("2110");
  Piedestaux_2110->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_2110->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_2110->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_2110->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_2110->SetLineColor(1);
  Piedestaux_2110->Draw("hist");
  c0->cd(6);
  f->cd("2111");
  Piedestaux_2111->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_2111->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_2111->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_2111->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_2111->SetLineColor(1);
  Piedestaux_2111->Draw("hist");

  TCanvas* c1=new TCanvas("c1","sigma",0,0,650,650);
  c1->Divide(2,3,0.0001,0.0001);
  c1_1->SetFillColor(10);
  c1_1->Range(-10,-10,10,10);
  c1_1->SetLeftMargin(0.12);
  c1_1->SetBottomMargin(0.18);
  c1_1->SetRightMargin(0.05);
  c1_1->SetTopMargin(0.05);
  c1_2->SetFillColor(10);
  c1_2->Range(-10,-10,10,10);
  c1_2->SetLeftMargin(0.12);
  c1_2->SetBottomMargin(0.18);
  c1_2->SetRightMargin(0.05);
  c1_2->SetTopMargin(0.05);
  c1_3->SetFillColor(10);
  c1_3->Range(-10,-10,10,10);
  c1_3->SetLeftMargin(0.12);
  c1_3->SetBottomMargin(0.18);
  c1_3->SetRightMargin(0.05);
  c1_3->SetTopMargin(0.05);
  c1_4->SetFillColor(10);
  c1_4->Range(-10,-10,10,10);
  c1_4->SetLeftMargin(0.12);
  c1_4->SetBottomMargin(0.18);
  c1_4->SetRightMargin(0.05);
  c1_4->SetTopMargin(0.05);
  c1_5->SetFillColor(10);
  c1_5->Range(-10,-10,10,10);
  c1_5->SetLeftMargin(0.12);
  c1_5->SetBottomMargin(0.18);
  c1_5->SetRightMargin(0.05);
  c1_5->SetTopMargin(0.05);
  c1_6->SetFillColor(10);
  c1_6->Range(-10,-10,10,10);
  c1_6->SetLeftMargin(0.12);
  c1_6->SetBottomMargin(0.18);
  c1_6->SetRightMargin(0.05);
  c1_6->SetTopMargin(0.05);

  int nplaq[4] = { 4, 4, 12, 12 };
  double sig_moyen_long[2][4];
  double err_sig_moyen_long[2][4];
  for (int i=0; i<2; i++)
    for (int j=0; j<4; j++) {
      sig_moyen_long[i][j] = 0.;
      err_sig_moyen_long[i][j] = 0.;
    }  

  // lecture 0, position faisceau 3eme AMS02
  c1->cd(3);
  f->cd("0900");
  Sigma_0900->GetXaxis()->SetLabelSize(0.065);
  Sigma_0900->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0900->GetYaxis()->SetLabelSize(0.060);
  Sigma_0900->SetMaximum(5.);
  int nbins = Sigma_0900->GetNbinsX();
  printf(" det 0 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  int ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0900->GetBinContent(ib) == 0) {
      double cont = Sigma_0900->GetBinContent(ib);
      sig_moyen_long[0][0]+=cont;
      //      printf("ib %d cont %lf sig_moyen %lf\n",ib,cont,sig_moyen_long[0][0]);
      err_sig_moyen_long[0][0]+=(cont*cont);
      ncha++;
    }
  }
  printf("ncha %d\n",ncha);
  if (ncha > 0) {
    sig_moyen_long[0][0]/=ncha;
    printf("sig_moyen_long %lf\n",sig_moyen_long[0][0]);
    err_sig_moyen_long[0][0]-=(sig_moyen_long[0][0]*sig_moyen_long[0][0]);
    err_sig_moyen_long[0][0]=sqrt(err_sig_moyen_long[0][0])/ncha;
  }
  ncha = 0;
  for (int ib=640; ib<1024; ib++) {
    if (Statut_0900->GetBinContent(ib) == 0) {
      double cont = Sigma_0900->GetBinContent(ib);
      sig_moyen_long[1][0]+=cont;
      err_sig_moyen_long[1][0]+=(cont*cont);
      ncha++;
    }
  }
  if (ncha > 0) {
    sig_moyen_long[1][0]/=ncha;
    err_sig_moyen_long[1][0]-=(sig_moyen_long[1][0]*sig_moyen_long[1][0]);
    err_sig_moyen_long[1][0]=sqrt(err_sig_moyen_long[1][0])/ncha;
  }
  printf("det 0 plaq %d p %lf +- %lf n %lf +- %lf\n",nplaq[0],sig_moyen_long[0][0],err_sig_moyen_long[0][0],
	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  //  Sigma_0900->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_0900->SetLineColor(1);
  Sigma_0900->Draw("hist");  
  // lecture 1, position faisceau 1ere AMS02
  c1->cd(1);
  f->cd("0901");
  Sigma_0901->GetXaxis()->SetLabelSize(0.065);
  Sigma_0901->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0901->GetYaxis()->SetLabelSize(0.060);
  Sigma_0901->SetMaximum(5.);
  //  Sigma_0901->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_0901->SetLineColor(1);
  Sigma_0901->Draw("hist");
  nbins = Sigma_0901->GetNbinsX();
  printf(" det 1 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0901->GetBinContent(ib) == 0) {
      double cont = Sigma_0901->GetBinContent(ib);
      sig_moyen_long[0][1]+=cont;
      //      printf("ib %d cont %lf sig_moyen %lf\n",ib,cont,sig_moyen_long[0][0]);
      err_sig_moyen_long[0][1]+=(cont*cont);
      ncha++;
    }
  }
  printf("ncha %d\n",ncha);
  if (ncha > 0) {
    sig_moyen_long[0][1]/=ncha;
    printf("sig_moyen_long %lf\n",sig_moyen_long[0][1]);
    err_sig_moyen_long[0][1]-=(sig_moyen_long[0][1]*sig_moyen_long[0][1]);
    err_sig_moyen_long[0][1]=sqrt(err_sig_moyen_long[0][1])/ncha;
  }
  ncha = 0;
  for (int ib=640; ib<1024; ib++) {
    if (Statut_0901->GetBinContent(ib) == 0) {
      double cont = Sigma_0901->GetBinContent(ib);
      sig_moyen_long[1][1]+=cont;
      err_sig_moyen_long[1][1]+=(cont*cont);
      ncha++;
    }
  }
  if (ncha > 0) {
    sig_moyen_long[1][1]/=ncha;
    err_sig_moyen_long[1][1]-=(sig_moyen_long[1][1]*sig_moyen_long[1][1]);
    err_sig_moyen_long[1][1]=sqrt(err_sig_moyen_long[1][1])/ncha;
  }
  printf("det 1 plaq %d p %lf +- %lf n %lf +- %lf\n",nplaq[1],sig_moyen_long[0][1],err_sig_moyen_long[0][1],
	 sig_moyen_long[1][1],err_sig_moyen_long[1][1]);
  // lecture 2, position faisceau 4eme AMS02
  c1->cd(4);
  f->cd("0904");
  Sigma_0904->GetXaxis()->SetLabelSize(0.065);
  Sigma_0904->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0904->GetYaxis()->SetLabelSize(0.060);
  Sigma_0904->SetMaximum(5.);
  //  Sigma_0904->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_0904->SetLineColor(1);
  Sigma_0904->Draw("hist");
  nbins = Sigma_0904->GetNbinsX();
  printf(" det 1 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0904->GetBinContent(ib) == 0) {
      double cont = Sigma_0904->GetBinContent(ib);
      sig_moyen_long[0][2]+=cont;
      //      printf("ib %d cont %lf sig_moyen %lf\n",ib,cont,sig_moyen_long[0][0]);
      err_sig_moyen_long[0][2]+=(cont*cont);
      ncha++;
    }
  }
  printf("ncha %d\n",ncha);
  if (ncha > 0) {
    sig_moyen_long[0][2]/=ncha;
    printf("sig_moyen_long %lf\n",sig_moyen_long[0][2]);
    err_sig_moyen_long[0][2]-=(sig_moyen_long[0][2]*sig_moyen_long[0][2]);
    err_sig_moyen_long[0][2]=sqrt(err_sig_moyen_long[0][2])/ncha;
  }
  ncha = 0;
  for (int ib=640; ib<1024; ib++) {
    if (Statut_0904->GetBinContent(ib) == 0) {
      double cont = Sigma_0904->GetBinContent(ib);
      sig_moyen_long[1][2]+=cont;
      err_sig_moyen_long[1][2]+=(cont*cont);
      ncha++;
    }
  }
  if (ncha > 0) {
    sig_moyen_long[1][2]/=ncha;
    err_sig_moyen_long[1][2]-=(sig_moyen_long[1][2]*sig_moyen_long[1][2]);
    err_sig_moyen_long[1][2]=sqrt(err_sig_moyen_long[1][2])/ncha;
  }
  printf("det 2 plaq %d p %lf +- %lf n %lf +- %lf\n",nplaq[2],sig_moyen_long[0][2],err_sig_moyen_long[0][2],
	 sig_moyen_long[1][2],err_sig_moyen_long[1][2]);
  // lecture 3, position faisceau 2eme AMS02
  c1->cd(2);
  f->cd("0905");
  Sigma_0905->GetXaxis()->SetLabelSize(0.065);
  Sigma_0905->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0905->GetYaxis()->SetLabelSize(0.060);
  Sigma_0905->SetMaximum(5.);
  nbins = Sigma_0905->GetNbinsX();
  printf(" det 1 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0905->GetBinContent(ib) == 0) {
      double cont = Sigma_0905->GetBinContent(ib);
      sig_moyen_long[0][3]+=cont;
      //      printf("ib %d cont %lf sig_moyen %lf\n",ib,cont,sig_moyen_long[0][0]);
      err_sig_moyen_long[0][3]+=(cont*cont);
      ncha++;
    }
  }
  printf("ncha %d\n",ncha);
  if (ncha > 0) {
    sig_moyen_long[0][3]/=ncha;
    printf("sig_moyen_long %lf\n",sig_moyen_long[0][3]);
    err_sig_moyen_long[0][3]-=(sig_moyen_long[0][3]*sig_moyen_long[0][3]);
    err_sig_moyen_long[0][3]=sqrt(err_sig_moyen_long[0][3])/ncha;
  }
  ncha = 0;
  for (int ib=640; ib<1024; ib++) {
    if (Statut_0905->GetBinContent(ib) == 0) {
      double cont = Sigma_0905->GetBinContent(ib);
      sig_moyen_long[1][3]+=cont;
      err_sig_moyen_long[1][3]+=(cont*cont);
      ncha++;
    }
  }
  if (ncha > 0) {
    sig_moyen_long[1][3]/=ncha;
    err_sig_moyen_long[1][3]-=(sig_moyen_long[1][3]*sig_moyen_long[1][3]);
    err_sig_moyen_long[1][3]=sqrt(err_sig_moyen_long[1][3])/ncha;
  }
  printf("det 3 plaq %d p %lf +- %lf n %lf +- %lf\n",nplaq[3],sig_moyen_long[0][3],err_sig_moyen_long[0][3],
	 sig_moyen_long[1][3],err_sig_moyen_long[1][3]);
  //  Sigma_0905->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_0905->SetLineColor(1);
  Sigma_0905->Draw("hist");
  c1->cd(5);
  f->cd("2110");
  Sigma_2110->GetXaxis()->SetLabelSize(0.065);
  Sigma_2110->GetXaxis()->SetLabelOffset(0.015);
  Sigma_2110->GetYaxis()->SetLabelSize(0.060);
  Sigma_2110->SetMaximum(10.);
  //  Sigma_2110->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_2110->SetLineColor(1);
  Sigma_2110->Draw("hist");
  c1->cd(6);
  f->cd("2111");
  Sigma_2111->GetXaxis()->SetLabelSize(0.065);
  Sigma_2111->GetXaxis()->SetLabelOffset(0.015);
  Sigma_2111->GetYaxis()->SetLabelSize(0.060);
  Sigma_2111->SetMaximum(10.);
  //  Sigma_2111->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_2111->SetLineColor(1);
  Sigma_2111->Draw("hist");


  TCanvas* c2=new TCanvas("c2","statut",0,0,650,650);
  c2->Divide(2,3,0.0001,0.0001);
  c2_1->SetFillColor(10);
  c2_1->Range(-10,-10,10,10);
  c2_1->SetLeftMargin(0.12);
  c2_1->SetBottomMargin(0.18);
  c2_1->SetRightMargin(0.05);
  c2_1->SetTopMargin(0.05);
  c2_2->SetFillColor(10);
  c2_2->Range(-10,-10,10,10);
  c2_2->SetLeftMargin(0.12);
  c2_2->SetBottomMargin(0.18);
  c2_2->SetRightMargin(0.05);
  c2_2->SetTopMargin(0.05);
  c2_3->SetFillColor(10);
  c2_3->Range(-10,-10,10,10);
  c2_3->SetLeftMargin(0.12);
  c2_3->SetBottomMargin(0.18);
  c2_3->SetRightMargin(0.05);
  c2_3->SetTopMargin(0.05);
  c2_4->SetFillColor(10);
  c2_4->Range(-10,-10,10,10);
  c2_4->SetLeftMargin(0.12);
  c2_4->SetBottomMargin(0.18);
  c2_4->SetRightMargin(0.05);
  c2_4->SetTopMargin(0.05);
  c2_5->SetFillColor(10);
  c2_5->Range(-10,-10,10,10);
  c2_5->SetLeftMargin(0.12);
  c2_5->SetBottomMargin(0.18);
  c2_5->SetRightMargin(0.05);
  c2_5->SetTopMargin(0.05);
  c2_6->SetFillColor(10);
  c2_6->Range(-10,-10,10,10);
  c2_6->SetLeftMargin(0.12);
  c2_6->SetBottomMargin(0.18);
  c2_6->SetRightMargin(0.05);
  c2_6->SetTopMargin(0.05);

  // lecture 0, position faisceau 3eme AMS02
  c2->cd(3);
  f->cd("0900");
  Statut_0900->GetXaxis()->SetLabelSize(0.065);
  Statut_0900->GetXaxis()->SetLabelOffset(0.015);
  Statut_0900->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0900->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0900->SetLineColor(1);
  Statut_0900->Draw("hist");  
  // lecture 1, position faisceau 1ere AMS02
  c2->cd(1);
  f->cd("0901");
  Statut_0901->GetXaxis()->SetLabelSize(0.065);
  Statut_0901->GetXaxis()->SetLabelOffset(0.015);
  Statut_0901->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0901->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0901->SetLineColor(1);
  Statut_0901->Draw("hist");
  // lecture 2, position faisceau 4eme AMS02
  c2->cd(4);
  f->cd("0904");
  Statut_0904->GetXaxis()->SetLabelSize(0.065);
  Statut_0904->GetXaxis()->SetLabelOffset(0.015);
  Statut_0904->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0904->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0904->SetLineColor(1);
  Statut_0904->Draw("hist");
  // lecture 3, position faisceau 2eme AMS02
  c2->cd(2);
  f->cd("0905");
  Statut_0905->GetXaxis()->SetLabelSize(0.065);
  Statut_0905->GetXaxis()->SetLabelOffset(0.015);
  Statut_0905->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0905->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0905->SetLineColor(1);
  Statut_0905->Draw("hist");
  c2->cd(5);
  f->cd("2110");
  Statut_2110->GetXaxis()->SetLabelSize(0.065);
  Statut_2110->GetXaxis()->SetLabelOffset(0.015);
  Statut_2110->GetYaxis()->SetLabelSize(0.060);
  //  Statut_2110->GetXaxis()->SetRangeUser(0.,1.);
  Statut_2110->SetLineColor(1);
  Statut_2110->Draw("hist");
  c2->cd(6);
  f->cd("2111");
  Statut_2111->GetXaxis()->SetLabelSize(0.065);
  Statut_2111->GetXaxis()->SetLabelOffset(0.015);
  Statut_2111->GetYaxis()->SetLabelSize(0.060);
  //  Statut_2111->GetXaxis()->SetRangeUser(0.,1.);
  Statut_2111->SetLineColor(1);
  Statut_2111->Draw("hist");

  char cfich[30];
  sprintf(cfich,"calib_bruit_%d.dat",run);
  printf("%s\n",cfich);
  FILE *fich = fopen(cfich,"w");
  fprintf(fich,"#plaq  cote p sig moyenne +- incertitude cote n sig moyenne +- incertitude\n");
  for (int i=0; i<4; i++)
    fprintf(fich,"%d %f %f %f %f\n",nplaq[i],sig_moyen_long[0][i],err_sig_moyen_long[0][i],sig_moyen_long[1][i],err_sig_moyen_long[1][i]);
  int status = fclose(fich);
  printf("status %d\n",status);

  return;
}

Double_t gclan10(Double_t *x, Double_t *par) {
  //10-Jan-95
  // Landau convoluted with a Gaussian  Phys.Rev. A28 615
  //
  // par[0] = global scaling facor
  // par[1] = Most probable value of Landau distribution
  // par[2] = 1/ksi,  in lamda= 1/ksi ( Delta -Delta(mp) )
  // par[3] = sigma in Gaussian convolution

  Double_t fval = 0.;
  float pi =3.1415928;

  if(par[3] <= 0.) par[3]=0.5;
  Double_t arg = (x[0]-par[1])*par[2];  // the lamda
  Double_t sigma = par[3];

  int n = 10;        // competing with CPU
  Double_t range = 6.*sigma;  // any need to go large?
  Double_t dbin = range/n;
  Double_t a = 1./(sqrt(2.*pi)*sigma);
  Double_t sum = 0.;
  Double_t sumgd = 0.;
  //  printf("a %lf \n",a);

  for (int i=-n; i<=n; i++) {   // does the integral
    Double_t y = arg + i*dbin;
    Double_t val = -.5* ((arg-y)/sigma)**2;
    //    printf("val %lf exp %lf \n",val,exp(val));
    Double_t gd = a*exp(val);
    //   printf("i %d y %lf gd %lf\n",i,y,gd);
    sum+=gd*DenLan(y);
    sumgd+=gd;
  }

  fval = par[0]*sum/sumgd;

  return(fval);
}

Double_t DenLan(Double_t v)
{
   // The LANDAU function with mpv(most probable value) and sigma.
   // This function has been adapted from the CERNLIB routine G110 denlan.

   static Double_t p1[5] = {0.4259894875,-0.1249762550, 0.03984243700, -0.006298287635,   0.001511162253};
   static Double_t q1[5] = {1.0         ,-0.3388260629, 0.09594393323, -0.01608042283,    0.003778942063};

   static Double_t p2[5] = {0.1788541609, 0.1173957403, 0.01488850518, -0.001394989411,   0.0001283617211};
   static Double_t q2[5] = {1.0         , 0.7428795082, 0.3153932961,   0.06694219548,    0.008790609714};

   static Double_t p3[5] = {0.1788544503, 0.09359161662,0.006325387654, 0.00006611667319,-0.000002031049101};
   static Double_t q3[5] = {1.0         , 0.6097809921, 0.2560616665,   0.04746722384,    0.006957301675};

   static Double_t p4[5] = {0.9874054407, 118.6723273,  849.2794360,   -743.7792444,      427.0262186};
   static Double_t q4[5] = {1.0         , 106.8615961,  337.6496214,    2016.712389,      1597.063511};

   static Double_t p5[5] = {1.003675074,  167.5702434,  4789.711289,    21217.86767,     -22324.94910};
   static Double_t q5[5] = {1.0         , 156.9424537,  3745.310488,    9834.698876,      66924.28357};

   static Double_t p6[5] = {1.000827619,  664.9143136,  62972.92665,    475554.6998,     -5743609.109};
   static Double_t q6[5] = {1.0         , 651.4101098,  56974.73333,    165917.4725,     -2815759.939};

   static Double_t a1[3] = {0.04166666667,-0.01996527778, 0.02709538966};

   static Double_t a2[2] = {-1.845568670,-4.284640743};

   //   if (sigma <= 0) return 0;
   //   Double_t v = (x-mpv)/sigma;
   Double_t u, ue, us, den;
   if (v < -5.5) {
      u   = TMath::Exp(v+1.0);
      if (u < 1e-10) return 0.0;
      ue  = TMath::Exp(-1/u);
      us  = TMath::Sqrt(u);
      den = 0.3989422803*(ue/us)*(1+(a1[0]+(a1[1]+a1[2]*u)*u)*u);
   } else if(v < -1) {
      u   = TMath::Exp(-v-1);
      den = TMath::Exp(-u)*TMath::Sqrt(u)*
             (p1[0]+(p1[1]+(p1[2]+(p1[3]+p1[4]*v)*v)*v)*v)/
             (q1[0]+(q1[1]+(q1[2]+(q1[3]+q1[4]*v)*v)*v)*v);
   } else if(v < 1) {
      den = (p2[0]+(p2[1]+(p2[2]+(p2[3]+p2[4]*v)*v)*v)*v)/
            (q2[0]+(q2[1]+(q2[2]+(q2[3]+q2[4]*v)*v)*v)*v);
   } else if(v < 5) {
      den = (p3[0]+(p3[1]+(p3[2]+(p3[3]+p3[4]*v)*v)*v)*v)/
            (q3[0]+(q3[1]+(q3[2]+(q3[3]+q3[4]*v)*v)*v)*v);
   } else if(v < 12) {
      u   = 1/v;
      den = u*u*(p4[0]+(p4[1]+(p4[2]+(p4[3]+p4[4]*u)*u)*u)*u)/
                (q4[0]+(q4[1]+(q4[2]+(q4[3]+q4[4]*u)*u)*u)*u);
   } else if(v < 50) {
      u   = 1/v;
      den = u*u*(p5[0]+(p5[1]+(p5[2]+(p5[3]+p5[4]*u)*u)*u)*u)/
                (q5[0]+(q5[1]+(q5[2]+(q5[3]+q5[4]*u)*u)*u)*u);
   } else if(v < 300) {
      u   = 1/v;
      den = u*u*(p6[0]+(p6[1]+(p6[2]+(p6[3]+p6[4]*u)*u)*u)*u)/
                (q6[0]+(q6[1]+(q6[2]+(q6[3]+q6[4]*u)*u)*u)*u);
   } else {
      u   = 1/(v-v*TMath::Log(v)/(v+1));
      den = u*u*(1+(a2[0]+a2[1]*u)*u);
   }
   return den;
}

void fit_langau() {

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  //  TFile *f = new TFile("Hist_1234_a5_1_2.root");
  TFile *f = new TFile("Hist_1510.root");
  TH1D *hfit;
  f->GetObject("2111/Signal_2111_K",hfit);

  TF1 *langau = new TF1("langau",gclan10,125,500,4);
  langau->SetLineWidth(1);
  langau->SetParameters(5000.,250.,0.5,2.);

  TCanvas* c1=new TCanvas("c0","fit landau",0,0,650,650);
  c1->cd();

  hfit->GetXaxis()->SetLabelSize(0.065);
  hfit->GetXaxis()->SetLabelOffset(0.015);
  hfit->GetYaxis()->SetLabelSize(0.060);
  hfit->GetXaxis()->SetRangeUser(125.,500.);
  hfit->SetLineColor(1);
  //  hfit->Draw();
  hfit->Fit("langau");
 
  return;
}

void lire_align() {
  FILE *ft = fopen("../align/algpar.dat","r");
  char ligne[80];
  for (int i=0; i<13; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i][0],&algpar[i][1],
    &algpar[i][2],&algpar[i][3],&algpar[i][4],&algpar[i][5]);
   printf("%f %f %f %f %f %f\n",algpar[i][0],algpar[i][1],
    algpar[i][2],algpar[i][3],algpar[i][4],algpar[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i+1][0],&algpar[i+1][1],
    &algpar[i+1][2],&algpar[i+1][3],&algpar[i+1][4],&algpar[i+1][5]);
   printf("%f %f %f %f %f %f\n",algpar[i+1][0],algpar[i+1][1],
    algpar[i+1][2],algpar[i+1][3],algpar[i+1][4],algpar[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[14][0],&algpar[14][1],&algpar[14][2],&algpar[14][3],
   &algpar[14][4],&algpar[14][5]);
  printf("%f %f %f %f %f %f\n",algpar[14][0],algpar[14][1],
    algpar[14][2],algpar[14][3],algpar[14][4],algpar[14][5]);
  fclose(ft);
}

void lire_align_1237() {
  FILE *ft = fopen("../align/algpar.dat","r");
  char ligne[80];
  for (int i=0; i<13; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i][0],&algpar[i][1],
    &algpar[i][2],&algpar[i][3],&algpar[i][4],&algpar[i][5]);
   printf("%f %f %f %f %f %f\n",algpar[i][0],algpar[i][1],
    algpar[i][2],algpar[i][3],algpar[i][4],algpar[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i+1][0],&algpar[i+1][1],
    &algpar[i+1][2],&algpar[i+1][3],&algpar[i+1][4],&algpar[i+1][5]);
   printf("%f %f %f %f %f %f\n",algpar[i+1][0],algpar[i+1][1],
    algpar[i+1][2],algpar[i+1][3],algpar[i+1][4],algpar[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[14][0],&algpar[14][1],&algpar[14][2],&algpar[14][3],
   &algpar[14][4],&algpar[14][5]);
  printf("%f %f %f %f %f %f\n",algpar[14][0],algpar[14][1],
    algpar[14][2],algpar[14][3],algpar[14][4],algpar[14][5]);
  for (int i=0; i<13; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar_2[i][0],&algpar_2[i][1],
    &algpar_2[i][2],&algpar_2[i][3],&algpar_2[i][4],&algpar_2[i][5]);
   printf("%f %f %f %f %f %f\n",algpar_2[i][0],algpar_2[i][1],
    algpar_2[i][2],algpar_2[i][3],algpar_2[i][4],algpar_2[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar_2[i+1][0],&algpar_2[i+1][1],
    &algpar_2[i+1][2],&algpar_2[i+1][3],&algpar_2[i+1][4],&algpar_2[i+1][5]);
   printf("%f %f %f %f %f %f\n",algpar_2[i+1][0],algpar_2[i+1][1],
    algpar_2[i+1][2],algpar_2[i+1][3],algpar_2[i+1][4],algpar_2[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar_2[14][0],&algpar_2[14][1],&algpar_2[14][2],&algpar_2[14][3],
   &algpar_2[14][4],&algpar_2[14][5]);
  printf("%f %f %f %f %f %f\n",algpar_2[14][0],algpar_2[14][1],
    algpar_2[14][2],algpar_2[14][3],algpar_2[14][4],algpar_2[14][5]);
  fclose(ft);
}

void scan_eta(int det) {
  TH1D *hetay[16][6];
  char cfich[60];
  char chist[60];
  for (int i=0; i<16; i++) {
    sprintf(cfich,"Hist_%d_a6_1_2_6.root",1500+i);
    //    printf("fichier %s\n",cfich);
    TFile *f = new TFile(cfich);
    hetay[i][3] = (TH1D*) f->Get("0900/Eta_0900_K")->Clone();
    hetay[i][1] = (TH1D*) f->Get("0901/Eta_0901_K")->Clone();
    hetay[i][5] = (TH1D*) f->Get("0904/Eta_0904_K")->Clone();
    hetay[i][2] = (TH1D*) f->Get("0905/Eta_0905_K")->Clone();
    hetay[i][4] = (TH1D*) f->Get("2110/Eta_2110_K")->Clone();
    hetay[i][0] = (TH1D*) f->Get("2111/Eta_2111_K")->Clone();
    for (int j=0; j<6; j++) hetay[i][j]->SetDirectory(0);
    int stat = f->Close();
    //    printf("stat %d\n",stat);
  }


  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  char cpad[40];
  TPad *ptpad;
  TCanvas *c0 = new TCanvas("c0","eta y",0,0,650,650);
  c0->Divide(4,4,0.0001,0.0001);
  for (int i=0; i<16; i++) {
    c0->cd(i+1);
    sprintf(cpad,"c0_%d",i+1);
    ptpad = (TPad*) c0->FindObject(cpad);
    ptpad->SetFillColor(10);
    ptpad->Range(-10,-10,10,10);
    ptpad->SetLeftMargin(0.15);
    ptpad->SetBottomMargin(0.15);
    ptpad->SetRightMargin(0.12);
    ptpad->SetTopMargin(0.12);
    hetay[i][det]->Draw("hist");
  }

  float rap[16][6];
  float err_rap[16][6];
  for (int i=0; i<6; i++)
  for (int j=0; j<16; j++) {
        int nbinx = hetay[j][i]->GetNbinsX();
        printf("nbinx %d\n",nbinx);
        float sum1 = 0.;
        float sum2 = 0.;
        for (int ib=1; ib<nbinx+1; ib++) {
          float bcen = hetay[j][i]->GetBinCenter(ib);
          printf("ib %d cen %f\n",ib,bcen);
          if(ib <= nbinx/2) sum1+=hetay[j][i]->GetBinContent(ib);
          else sum2+=hetay[j][i]->GetBinContent(ib);
        }
        rap[j][i] = sum1/sum2;
        err_rap[j][i] = rap[j][i]*sqrt((1./sum1)+(1./sum2));
  }

  float delai_p[16],delai_n[16];
  for (int i=0; i<16; i++) {
    delai_p[i] = 0.820 + 1.700. + i*0.2;
    delai_n[i] = delai_p[i] + 1.0;
  }

  float x[16],y[16],ex[16],ey[16];
  TGraph *grap[6];
  for (int i=0; i<6; i++) {
    for (int j=0; j<16; j++) {
      x[j] = delai_n[j];
      y[j] = rap[j][i];
      ex[j] = 0.;
      ey[j] = err_rap[j][i];
    }
    grap[i] = new TGraphErrors(16,x,y,ex,ey);
    grap[i]->SetMarkerStyle(20);
    switch (i) {
    case 0: grap[i]->SetMarkerColor(1); printf("case 0\n"); 
            grap[i]->SetLineColor(1); break;
    case 1: grap[i]->SetMarkerColor(2); printf("case 1\n");
            grap[i]->SetLineColor(2); break;
    case 2: grap[i]->SetMarkerColor(4); printf("case 2\n");
            grap[i]->SetLineColor(4); break;
    case 3: grap[i]->SetMarkerColor(46); printf("case 3\n");
	    grap[i]->SetLineColor(46); break;
    case 4: grap[i]->SetMarkerColor(20); printf("case 4\n");
            grap[i]->SetMarkerStyle(20);
            grap[i]->SetLineColor(20); break;
    case 5: grap[i]->SetMarkerColor(38); printf("case 5 i %d\n",i);
            grap[i]->SetMarkerStyle(20);
            grap[i]->SetLineColor(38); break;
    default: break;
    }
  }

  TH1F *deta = new TH1F("deta","eta vs delai",10,2.75,7.25);
  deta->SetMaximum(1.50);
  deta->SetMinimum(0.85);
  deta->GetXaxis()->SetTitle("Hold Delay (#mus)");
  deta->GetXaxis()->CenterTitle();
  deta->GetXaxis()->SetTitleOffset(1.20);
  deta->GetXaxis()->SetTitleSize(0.05);
  deta->GetXaxis()->SetLabelSize(0.0425);
  deta->GetXaxis()->SetLabelOffset(0.010);
  deta->GetYaxis()->SetTitle("#eta(0.0,0.5) / #eta(0.5,1.0)");
  deta->GetYaxis()->CenterTitle();
  deta->GetYaxis()->SetTitleOffset(1.40);
  deta->GetYaxis()->SetTitleSize(0.05);
  deta->GetYaxis()->SetLabelSize(0.0425);
  deta->GetYaxis()->SetLabelOffset(0.010);

  //  for (int i=0; i<16; i++) printf("run %d rap %lf\n",i+1500,rap[i][5]);

  TCanvas *c1 = new TCanvas("c0","eta et delai VA",0,0,500,500);
  c1->SetFillColor(10);
  c1->Range(-10,-10,10,10);
  c1->SetLeftMargin(0.15);
  c1->SetBottomMargin(0.15);
  c1->SetRightMargin(0.10);
  c1->SetTopMargin(0.10);
  c1->cd();
  deta->Draw();
  for (int i=0; i<6; i++) grap[i]->Draw("ZPL");

  tref1 = new TLatex(0.725,0.80,"Ref.1");
  tref1->SetNDC();
  tref1->SetTextColor(1);
  tref1->SetTextSize(0.04);
  tref1->Draw();

  tech3 = new TLatex(0.725,0.75,"l04pt001");
  tech3->SetNDC();
  tech3->SetTextColor(46);
  tech3->SetTextSize(0.04);
  tech3->Draw();

  tech1 = new TLatex(0.725,0.70,"l04pt002");
  tech1->SetNDC();
  tech1->SetTextColor(2);
  tech1->SetTextSize(0.04);
  tech1->Draw();

  tech2 = new TLatex(0.725,0.30,"l12ai009");
  tech2->SetNDC();
  tech2->SetTextColor(4);
  tech2->SetTextSize(0.04);
  tech2->Draw();

  tech4 = new TLatex(0.725,0.25,"l12ai002");
  tech4->SetNDC();
  tech4->SetTextColor(38);
  tech4->SetTextSize(0.04);
  tech4->Draw();

  tref2 = new TLatex(0.725,0.20,"Ref.2");
  tref2->SetNDC();
  tref2->SetTextColor(20);
  tref2->SetTextSize(0.04);
  tref2->Draw();

  return;
}

int lire_align(int run, int npiste_pos3) {
  char cfich[60];
  if (npiste_pos3 == 3)
  sprintf(cfich,"../align/algpar_%d_1_2_6_3p_etan.dat",run);
  if (npiste_pos3 == 4)
  sprintf(cfich,"../align/algpar_%d_1_2_6_4p_etan.dat",run);
  if (npiste_pos3 == 2 || npiste_pos3 < 0)
  sprintf(cfich,"../align/algpar_%d_1_2_6_etan.dat",run);
  FILE *ft = fopen(cfich,"r");
  if (ft == NULL) {
    printf("fichier %s pas trouve\n",cfich);
    return(0);
  }
  char ligne[80];
  for (int i=0; i<13; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i][0],&algpar[i][1],
    &algpar[i][2],&algpar[i][3],&algpar[i][4],&algpar[i][5]);
   //      printf("%f %f %f %f %f %f\n",algpar[i][0],algpar[i][1],
   //       algpar[i][2],algpar[i][3],algpar[i][4],algpar[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i+1][0],&algpar[i+1][1],
    &algpar[i+1][2],&algpar[i+1][3],&algpar[i+1][4],&algpar[i+1][5]);
   //      printf("%f %f %f %f %f %f\n",algpar[i+1][0],algpar[i+1][1],
   //      algpar[i+1][2],algpar[i+1][3],algpar[i+1][4],algpar[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[14][0],&algpar[14][1],&algpar[14][2],&algpar[14][3],
   &algpar[14][4],&algpar[14][5]);
  //    printf("%f %f %f %f %f %f\n",algpar[14][0],algpar[14][1],
  //      algpar[14][2],algpar[14][3],algpar[14][4],algpar[14][5]);
  fclose(ft);
  return(1);
}

int lire_align_1237(int run) {
  char cfich[60];
  sprintf(cfich,"../align/algpar_%d_1_2_6_etan.dat",run);
  FILE *ft = fopen(cfich,"r");
  if (ft == NULL) {
    printf("fichier %s pas trouve\n",cfich);
    return 0;
  }
  FILE *ft = fopen(cfich,"r");
  char ligne[80];
  for (int i=0; i<13; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i][0],&algpar[i][1],
    &algpar[i][2],&algpar[i][3],&algpar[i][4],&algpar[i][5]);
   //   printf("%f %f %f %f %f %f\n",algpar[i][0],algpar[i][1],
   //    algpar[i][2],algpar[i][3],algpar[i][4],algpar[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[i+1][0],&algpar[i+1][1],
    &algpar[i+1][2],&algpar[i+1][3],&algpar[i+1][4],&algpar[i+1][5]);
   //   printf("%f %f %f %f %f %f\n",algpar[i+1][0],algpar[i+1][1],
   //    algpar[i+1][2],algpar[i+1][3],algpar[i+1][4],algpar[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar[14][0],&algpar[14][1],&algpar[14][2],&algpar[14][3],
   &algpar[14][4],&algpar[14][5]);
  //  printf("%f %f %f %f %f %f\n",algpar[14][0],algpar[14][1],
  //    algpar[14][2],algpar[14][3],algpar[14][4],algpar[14][5]);
  for (int i=0; i<13; i+=2) {
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar_2[i][0],&algpar_2[i][1],
    &algpar_2[i][2],&algpar_2[i][3],&algpar_2[i][4],&algpar_2[i][5]);
   //   printf("%f %f %f %f %f %f\n",algpar_2[i][0],algpar_2[i][1],
   //    algpar_2[i][2],algpar_2[i][3],algpar_2[i][4],algpar_2[i][5]);
   fgets(ligne,sizeof(ligne),ft);
   sscanf(ligne,"%f %f %f %f %f %f\n",&algpar_2[i+1][0],&algpar_2[i+1][1],
    &algpar_2[i+1][2],&algpar_2[i+1][3],&algpar_2[i+1][4],&algpar_2[i+1][5]);
   //   printf("%f %f %f %f %f %f\n",algpar_2[i+1][0],algpar_2[i+1][1],
   //    algpar_2[i+1][2],algpar_2[i+1][3],algpar_2[i+1][4],algpar_2[i+1][5]);
  }
  fgets(ligne,sizeof(ligne),ft);
  sscanf(ligne,"%f %f %f %f %f %f\n",&algpar_2[14][0],&algpar_2[14][1],&algpar_2[14][2],&algpar_2[14][3],
   &algpar_2[14][4],&algpar_2[14][5]);
  //  printf("%f %f %f %f %f %f\n",algpar_2[14][0],algpar_2[14][1],
  //    algpar_2[14][2],algpar_2[14][3],algpar_2[14][4],algpar_2[14][5]);
  fclose(ft);
  return 1;
}

void p_reso_sb() {

  float angxz[6] = { 3.2, 5.0, 9.4, 17.9, 20.4, 38.7 };
  float preso[6] = { 8.79, 8.95, 8.30, 8.03, 7.64, 7.05 };
  float err_preso[6] = { 0.05, 0.07, 0.05, 0.08, 0.05, 0.05 };
  float prese[6] = { 13.29, 14.3, 12.70, 12.36, 11.81, 10.29 };
  float err_prese[6] = { 0.51, 0.68, 0.50, 0.86, 0.59, 0.74 };
  float psab[6] = { 8.15, 8.25, 8.53, 8.94, 9.59, 11.30 };

  float x[50],y[50],ex[50],ey[50];

  for (int i=0; i<50; i++) ex[i]=0.;
  for (int i=0; i<50; i++) ey[i]=0.;

  float res_min = 5.;  // x1
  float res_max = 16.; // x2
  float sab_min = 5.;  // y1
  float sab_max = 13.; // y2

  float m = (sab_max - sab_min)/(res_max - res_min);
  float b = (res_max*sab_min) - (res_min*sab_max);
  b /= (res_max - res_min);
  printf("m %f b %f\n",m,b);

  TH1F *hp = new TH1F("hp","p res",10,-2.,42.);
  hp->SetMaximum(res_max);
  hp->SetMinimum(res_min);
  hp->GetXaxis()->SetNdivisions(506);
  hp->GetXaxis()->SetTitle("#theta_{xz}");
  hp->GetXaxis()->CenterTitle();
  hp->GetXaxis()->SetTitleSize(0.05);
  hp->GetYaxis()->SetNdivisions(508);
  hp->GetYaxis()->SetTitle("Resolution (#mum)");
  hp->GetYaxis()->CenterTitle();
  hp->GetYaxis()->SetTitleSize(0.05);

  int npt = 0;
  for (int i=0; i<6; i++) {
    x[i] = angxz[i];
    y[i] = prese[i];
    ey[i] = err_prese[i];
    npt++;
  }
  TGraphErrors *gprese = new TGraphErrors(npt,x,y,ex,ey);
  gprese->SetLineColor(2);
  gprese->SetLineStyle(1);
  gprese->SetMarkerStyle(24);
  gprese->SetMarkerSize(1.2);
  gprese->SetMarkerColor(2);
  TGraphErrors *gprese_bis = new TGraphErrors(npt,x,y,ex,ey);
  gprese_bis->SetLineColor(2);
  gprese_bis->SetLineStyle(2);
  gprese_bis->SetMarkerStyle(24);
  gprese_bis->SetMarkerSize(1.2);
  gprese_bis->SetMarkerColor(2);

  int npt = 0;
  for (int i=0; i<6; i++) {
    x[i] = angxz[i];
    y[i] = preso[i];
    ey[i] = err_preso[i];
    npt++;
  }
  TGraphErrors *gpreso = new TGraphErrors(npt,x,y,ex,ey);
  gpreso->SetLineColor(2);
  gpreso->SetLineStyle(1);
  gpreso->SetMarkerStyle(20);
  gpreso->SetMarkerSize(1.2);
  gpreso->SetMarkerColor(2);

  npt = 0;
  for (int i=0; i<50; i++) ey[i]=0.;
  for (int i=0; i<6; i++) {
    x[i] = angxz[i];
    y[i] = (psab[i] - b)/m;
    printf("sab %f y %f\n",psab[i],y[i]);
    npt++;
  }
  TGraphErrors *gpsab = new TGraphErrors(npt,x,y,ex,ey);
  gpsab->SetLineColor(kRed-3);
  gpsab->SetLineStyle(1);
  gpsab->SetMarkerSize(1.2);
  gpsab->SetMarkerStyle(22);
  gpsab->SetMarkerColor(kRed-3);

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetFrameFillColor(10);

  TCanvas *c0 = new TCanvas( "c0", "c0", 0, 0, 550, 550 );
  c0->SetFillColor(10);
  c0->SetTopMargin(0.125);
  c0->SetBottomMargin(0.125);
  c0->SetRightMargin(0.125);
  c0->SetLeftMargin(0.125);

  //  c0->cd();
  hp->Draw();

  printf("%f %f %f %f\n",gPad->GetUxmax(),gPad->GetUymin(),
	 gPad->GetUxmax(), gPad->GetUymax()); 
  //  TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),
  //        gPad->GetUxmax(), gPad->GetUymax(),sab_min,sab_max,510,"+L");
  TGaxis *axis = new TGaxis(42.,res_min,
        42.,res_max,sab_min,sab_max,406,"+L");
  axis->SetLineColor(kRed-3);
  axis->SetLabelColor(kRed-3);
  axis->SetTitle("Cluster Signal-to-Noise");
  axis->CenterTitle();
  axis->SetTitleSize(0.05);
  axis->SetTitleColor(kRed-3);
  axis->Draw();
  gpreso->Draw("PLZ");
  gprese->Draw("PZ");
  gprese_bis->Draw("LZ");
  gpsab->Draw("PLZ");

  topt = new TLatex(20.,15.3,"optimal");
  topt->SetTextSize(0.0375);
  topt->Draw();
  TMarker *mopt = new TMarker(18.7,15.45,20);
  mopt->Draw();
  teff = new TLatex(20.,14.7,"effective");
  teff->SetTextSize(0.0375);
  teff->Draw();
  TMarker *meff = new TMarker(18.7,14.85,24);
  meff->Draw();
  tsn = new TLatex(20.,14.1,"S/N");
  tsn->SetTextSize(0.0375);
  tsn->Draw();
  TMarker *msn = new TMarker(18.7,14.30,22);
  msn->Draw();

  return;

}

void n_reso_sb() {

  float angyz[8] = { 1.7, 1.9, 10.0, 13.5, 18.47, 30.4, 38.4, 48.5 };
  float nreso[8] = { 31.6, 32.1, 31.3, 30.4, 30.3, 29.4, 29.8, 30.7 };
  float err_nreso[8] = { 0.8, 1.3, 0.8, 0.7, 0.7, 0.9, 1.0, 0.8 };
  float nrese[8] = { 35.3, 39.9, 34.6, 34.1, 33.3, 32.3, 34.2, 32.2 };
  float err_nrese[8] = { 0.8, 1.3, 0.8, 0.7, 0.7, 0.9, 1.1, 0.9 };
  float nsab[8] = { 6.34, 6.52, 6.45, 6.51, 6.62, 7.00, 7.60, 9.13 };

  float x[50],y[50],ex[50],ey[50];

  for (int i=0; i<50; i++) ex[i]=0.;
  for (int i=0; i<50; i++) ey[i]=0.;

  float res_min = 25.;  // x1
  float res_max = 42.; // x2
  float sab_min = 3s.;  // y1
  float sab_max = 10.; // y2

  float m = (sab_max - sab_min)/(res_max - res_min);
  float b = (res_max*sab_min) - (res_min*sab_max);
  b /= (res_max - res_min);
  printf("m %f b %f\n",m,b);

  TH1F *hn = new TH1F("hn","n res",10,-2.,52.);
  hn->SetMaximum(res_max);
  hn->SetMinimum(res_min);
  hn->GetXaxis()->SetNdivisions(506);
  hn->GetXaxis()->SetTitle("#theta_{xz}");
  hn->GetXaxis()->CenterTitle();
  hn->GetXaxis()->SetTitleSize(0.05);
  hn->GetYaxis()->SetNdivisions(508);
  hn->GetYaxis()->SetTitle("Resolution (#mum)");
  hn->GetYaxis()->CenterTitle();
  hn->GetYaxis()->SetTitleSize(0.05);

  int npt = 0;
  for (int i=0; i<8; i++) {
    x[i] = angyz[i];
    y[i] = nrese[i];
    ey[i] = err_nrese[i];
    npt++;
  }
  TGraphErrors *gnrese = new TGraphErrors(npt,x,y,ex,ey);
  gnrese->SetLineColor(4);
  gnrese->SetLineStyle(1);
  gnrese->SetMarkerStyle(24);
  gnrese->SetMarkerSize(1.2);
  gnrese->SetMarkerColor(4);
  TGraphErrors *gnrese_bis = new TGraphErrors(npt,x,y,ex,ey);
  gnrese_bis->SetLineColor(4);
  gnrese_bis->SetLineStyle(2);
  gnrese_bis->SetMarkerStyle(24);
  gnrese_bis->SetMarkerSize(1.2);
  gnrese_bis->SetMarkerColor(4);

  int npt = 0;
  for (int i=0; i<8; i++) {
    x[i] = angyz[i];
    y[i] = nreso[i];
    ey[i] = err_nreso[i];
    npt++;
  }
  TGraphErrors *gnreso = new TGraphErrors(npt,x,y,ex,ey);
  gnreso->SetLineColor(4);
  gnreso->SetLineStyle(1);
  gnreso->SetMarkerStyle(20);
  gnreso->SetMarkerSize(1.2);
  gnreso->SetMarkerColor(4);

  npt = 0;
  for (int i=0; i<50; i++) ey[i]=0.;
  for (int i=0; i<8; i++) {
    x[i] = angyz[i];
    y[i] = (nsab[i] - b)/m;
    printf("sab %f y %f\n",nsab[i],y[i]);
    npt++;
  }
  TGraphErrors *gnsab = new TGraphErrors(npt,x,y,ex,ey);
  gnsab->SetLineColor(kBlue-3);
  gnsab->SetLineStyle(1);
  gnsab->SetMarkerSize(1.2);
  gnsab->SetMarkerStyle(22);
  gnsab->SetMarkerColor(kBlue-3);

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetFrameFillColor(10);

  TCanvas *c0 = new TCanvas( "c0", "c0", 0, 0, 550, 550 );
  c0->SetFillColor(10);
  c0->SetTopMargin(0.125);
  c0->SetBottomMargin(0.125);
  c0->SetRightMargin(0.125);
  c0->SetLeftMargin(0.125);

  //  c0->cd();
  hn->Draw();

  printf("%f %f %f %f\n",gPad->GetUxmax(),gPad->GetUymin(),
	 gPad->GetUxmax(), gPad->GetUymax()); 
  //  TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),
  //        gPad->GetUxmax(), gPad->GetUymax(),sab_min,sab_max,510,"+L");
  TGaxis *axis = new TGaxis(52.,res_min,
        52.,res_max,sab_min,sab_max,406,"+L");
  axis->SetLineColor(kBlue-3);
  axis->SetLabelColor(kBlue-3);
  axis->SetTitle("Cluster Signal-to-Noise");
  axis->CenterTitle();
  axis->SetTitleSize(0.05);
  axis->SetTitleColor(kBlue-3);
  axis->Draw();
  gnreso->Draw("PLZ");
  gnrese->Draw("PZ");
  gnrese_bis->Draw("LZ");
  gnsab->Draw("PLZ");

  topt = new TLatex(25.,41.,"optimal");
  topt->SetTextSize(0.0375);
  topt->Draw();
  TMarker *mopt = new TMarker(23.,41.2,20);
  mopt->Draw();
  teff = new TLatex(25.,40.,"effective");
  teff->SetTextSize(0.0375);
  teff->Draw();
  TMarker *meff = new TMarker(23.,40.3,24);
  meff->Draw();
  tsn = new TLatex(25.,39.,"S/N");
  tsn->SetTextSize(0.0375);
  tsn->Draw();
  TMarker *msn = new TMarker(23.,39.3,22);
  msn->Draw();

  return;

}

void faisceau() {

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetPalette(1,0);

  int nrun = 17;
  float xy[17][6][2];
  float xy_rms[17][6][2];
  float diver[17][6][2];
  float divx[17];
  float divy[17];
  float ang_p[17];
  float ang_n[17];
  int run[17] = { 1234, 1251, 1227, 1225, 1230, 1232,
                  1258, 1261, 1235, 1237, 1239, 1263,
                  1506, 1246, 1247, 1243, 1244 };
  int npiste[17] = { 2, 2, 2, 2, 2, 3,
                     4, 4, 2, 2, 2, 3,
                     2, 3, 3, 4, 4 };
  char ligne[80];
  char cfich[80];
  char cpos[4], cx[3], cy[3], crms[5];
  char csig[5], ccentre[6], curad[12], cfrac[5];
  int pos;

  TH2F *hrmsxang = new TH2F("rmsxang","ang_pn",28,-0.5,55.5,23,-0.5,45.5);
  hrmsxang->GetYaxis()->SetTitle("#theta_{n}");
  hrmsxang->GetYaxis()->CenterTitle();
  hrmsxang->GetYaxis()->SetTitleSize(0.065);
  hrmsxang->GetYaxis()->SetTitleOffset(1.20);
  hrmsxang->GetYaxis()->SetLabelSize(0.050);
  hrmsxang->GetYaxis()->SetLabelOffset(0.015);
  hrmsxang->GetZaxis()->SetLabelSize(0.050);
  hrmsxang->GetZaxis()->SetLabelOffset(0.0225);
  hrmsxang->GetXaxis()->SetLabelSize(0.050);
  hrmsxang->GetXaxis()->SetLabelOffset(0.010);
  hrmsxang->GetXaxis()->SetTitle("#theta_{p}");
  hrmsxang->GetXaxis()->CenterTitle();
  hrmsxang->GetXaxis()->SetTitleSize(0.065);
  hrmsxang->GetXaxis()->SetTitleOffset(1.00);

  TH2F *hrmsyang = new TH2F("rmsyang","ang_pn",28,-0.5,55.5,23,-0.5,45.5);
  hrmsyang->GetYaxis()->SetTitle("#theta_{n}");
  hrmsyang->GetYaxis()->CenterTitle();
  hrmsyang->GetYaxis()->SetTitleSize(0.065);
  hrmsyang->GetYaxis()->SetTitleOffset(1.20);
  hrmsyang->GetYaxis()->SetLabelSize(0.050);
  hrmsyang->GetYaxis()->SetLabelOffset(0.015);
  hrmsyang->GetZaxis()->SetLabelSize(0.050);
  hrmsyang->GetZaxis()->SetLabelOffset(0.0225);
  hrmsyang->GetXaxis()->SetLabelSize(0.050);
  hrmsyang->GetXaxis()->SetLabelOffset(0.010);
  hrmsyang->GetXaxis()->SetTitle("#theta_{p}");
  hrmsyang->GetXaxis()->CenterTitle();
  hrmsyang->GetXaxis()->SetTitleSize(0.065);
  hrmsyang->GetXaxis()->SetTitleOffset(1.00);

  TH2F *hdivxang = new TH2F("rmsxang","ang_pn",28,-0.5,55.5,23,-0.5,45.5);
  hdivxang->GetYaxis()->SetTitle("#theta_{n}");
  hdivxang->GetYaxis()->CenterTitle();
  hdivxang->GetYaxis()->SetTitleSize(0.065);
  hdivxang->GetYaxis()->SetTitleOffset(1.20);
  hdivxang->GetYaxis()->SetLabelSize(0.050);
  hdivxang->GetYaxis()->SetLabelOffset(0.015);
  hdivxang->GetZaxis()->SetLabelSize(0.050);
  hdivxang->GetZaxis()->SetLabelOffset(0.0225);
  hdivxang->GetXaxis()->SetLabelSize(0.050);
  hdivxang->GetXaxis()->SetLabelOffset(0.010);
  hdivxang->GetXaxis()->SetTitle("#theta_{p}");
  hdivxang->GetXaxis()->CenterTitle();
  hdivxang->GetXaxis()->SetTitleSize(0.065);
  hdivxang->GetXaxis()->SetTitleOffset(1.00);

  TH2F *hdivyang = new TH2F("rmsyang","ang_pn",28,-0.5,55.5,23,-0.5,45.5);
  hdivyang->GetYaxis()->SetTitle("#theta_{n}");
  hdivyang->GetYaxis()->CenterTitle();
  hdivyang->GetYaxis()->SetTitleSize(0.065);
  hdivyang->GetYaxis()->SetTitleOffset(1.20);
  hdivyang->GetYaxis()->SetLabelSize(0.050);
  hdivyang->GetYaxis()->SetLabelOffset(0.015);
  hdivyang->GetZaxis()->SetLabelSize(0.050);
  hdivyang->GetZaxis()->SetLabelOffset(0.0225);
  hdivyang->GetXaxis()->SetLabelSize(0.050);
  hdivyang->GetXaxis()->SetLabelOffset(0.010);
  hdivyang->GetXaxis()->SetTitle("#theta_{p}");
  hdivyang->GetXaxis()->CenterTitle();
  hdivyang->GetXaxis()->SetTitleSize(0.065);
  hdivyang->GetXaxis()->SetTitleOffset(1.00);

  FILE *fich;
  
  for (int i=0; i<nrun; i++) {
    sprintf(cfich,"xypos_%d.dat",run[i]);
    //    printf("cfich %s\n",cfich);
    fich = fopen(cfich,"r");
    for (int j=0; j<6; j++) {
       fgets(ligne,sizeof(ligne),fich);
       //       printf("%s\n",ligne);
       sscanf(ligne,"%s %d %s %f %s %f %s %f %s %f\n",cpos,&pos,cx,&xy[i][j][0],crms,
              &xy_rms[i][j][0],cy,&xy[i][j][1],crms,&xy_rms[i][j][1]);
       //       printf("%s %d %s %s\n",cpos,pos,cx,crms);
       //       printf(" %7.4f %7.4f %7.4f %7.4f\n",xy[i][j][0],xy_rms[i][j][0],
       //	      xy[i][j][1],xy_rms[i][j][1]);
    }
    fclose(fich); 
  }
  
  for (int i=0; i<nrun; i++) {
    sprintf(cfich,"diver_%d.dat",run[i]);
    printf("cfich %s\n",cfich);
    fich = fopen(cfich,"r");
    fgets(ligne,sizeof(ligne),fich);
    printf("%s\n",ligne);
    for (int j=0; j<2; j++) {
       fgets(ligne,sizeof(ligne),fich);
       printf("%s\n",ligne);
       sscanf(ligne,"%s %f %s %f %s %s %f\n",csig,&diver[i][0+j*3][0],ccentre,&diver[i][1+j*3][0],curad,
             cfrac,&diver[i][2+j*3][0]);
       printf("%s %s %s %s\n",csig,ccentre,curad,cfrac);
       printf(" %f %f %f\n",diver[i][0+j*3][0],diver[i][1+j*3][0],diver[i][2+j*3][0]);
    }
    if (run[i] == 1506) {
      float limb = 0.;
      float limh = 0.;
      if (diver[i][1][0] < 0. && diver[i][4][0] > 0.) {
	limb = diver[i][1][0] - diver[i][0][0];
        limh = diver[i][4][0] + diver[i][3][0];
      }
      if (diver[i][4][0] < 0. && diver[i][1][0] > 0.) {
	limb = diver[i][4][0] - diver[i][3][0];
        limh = diver[i][1][0] + diver[i][0][0];
      }
      divx[i] = limh - limb;
    }
    else
    divx[i] = diver[i][0][0]*diver[i][2][0] + diver[i][3][0]*diver[i][5][0];
    fgets(ligne,sizeof(ligne),fich);
    printf("%s\n",ligne);
    for (int j=0; j<2; j++) {
       fgets(ligne,sizeof(ligne),fich);
       printf("%s\n",ligne);
       sscanf(ligne,"%s %f %s %f %s %s %f\n",csig,&diver[i][0+j*3][1],ccentre,&diver[i][1+j*3][1],curad,
             cfrac,&diver[i][2+j*3][1]);
       printf("%s %s %s %s\n",csig,ccentre,curad,cfrac);
       printf(" %f %f %f\n",diver[i][0+j*3][1],diver[i][1+j*3][1],diver[i][2+j*3][1]);
    }
    if (run[i] == 1506) {
      float limb = 0.;
      float limh = 0.;
      if (diver[i][1][1] < 0. && diver[i][4][1] > 0.) {
	limb = diver[i][1][1] - diver[i][0][1];
        limh = diver[i][4][1] + diver[i][3][1];
      }
      if (diver[i][4][1] < 0. && diver[i][1][1] > 0.) {
	limb = diver[i][4][1] - diver[i][3][1];
        limh = diver[i][1][1] + diver[i][0][1];
      }
      divy[i] = limh - limb;
    }
    else
    divy[i] = diver[i][0][1]*diver[i][2][1] + diver[i][3][0]*diver[i][5][1];
    fclose(fich); 
  }
  
  for (int i=0; i<nrun; i++) {
    int ouvert = lire_align(run[i],npiste[i]);
    if (ouvert) {
      float val = 1. - fabs(algpar[2][3]);
      ang_p[i] = 180.*acos(val)/pi;
      val = 1. - fabs(algpar[4][3]);
      ang_n[i] = 180.*acos(val)/pi;
      //      printf("run %d ang_p %f ang_n %f\n",run[i],ang_p[i],ang_n[i]);
      hrmsxang->Fill(ang_p[i],ang_n[i],xy_rms[i][0][0]);
      hrmsyang->Fill(ang_p[i],ang_n[i],xy_rms[i][0][1]);
      //      printf("angp %f angn %f yrms %f\n",ang_p[i],ang_n[i],xy_rms[i][2][1]);
      if (divx[i] >= 0. && divx[i] < 800.) hdivxang->Fill(ang_p[i],ang_n[i],divx[i]);
      if (divy[i] >= 0. && divy[i] < 800.) hdivyang->Fill(ang_p[i],ang_n[i],divy[i]);
      printf("angp %f angn %f divx %f divy %f\n",ang_p[i],ang_n[i],divx[i],divy[i]);
    }
  }

  char cpad[20];
  TCanvas* c0=new TCanvas("c0","angp - angn tracker",0,0,500,500);
  c0->SetFillColor(10);
  c0->Divide(2,2,0.0001,0.0001);
  for (int i=0; i<4; i++) {
    c0->cd(i+1);
    sprintf(cpad,"c0_%d",i+1);
    TPad* ptpad = (TPad*) c0->FindObject(cpad);
    printf("ptpad %d\n",ptpad);
    ptpad->SetFillColor(10);
    ptpad->SetLogy(0);
    ptpad->SetLeftMargin(0.18);
    ptpad->SetBottomMargin(0.18);
    ptpad->SetRightMargin(0.18);
    ptpad->SetTopMargin(0.18);
  }

  c0->cd(1);
  hrmsxang->Draw("colz");
  trmsx = new TLatex(0.425,0.85,"X RMS");
  trmsx->SetNDC();
  trmsx->SetTextSize(0.06);
  trmsx->Draw();
  tmm = new TLatex(0.830,0.85,"(mm)");
  tmm->SetNDC();
  tmm->SetTextSize(0.06);
  tmm->Draw();
  c0->cd(2);
  hrmsyang->Draw("colz");
  trmsy = new TLatex(0.425,0.85,"Y RMS");
  trmsy->SetNDC();
  trmsy->SetTextSize(0.06);
  trmsy->Draw();
  tmm->Draw();
  c0->cd(3);
  hdivxang->Draw("colz");
  tdivx = new TLatex(0.425,0.85,"X DIV");
  tdivx->SetNDC();
  tdivx->SetTextSize(0.06);
  tdivx->Draw();
  turad = new TLatex(0.830,0.85,"(#murad)");
  turad->SetNDC();
  turad->SetTextSize(0.06);
  turad->Draw();
  c0->cd(4);
  hdivyang->Draw("colz");
  tdivy = new TLatex(0.425,0.85,"Y DIV");
  tdivy->SetNDC();
  tdivy->SetTextSize(0.06);
  tdivy->Draw();
  turad->Draw();

}
