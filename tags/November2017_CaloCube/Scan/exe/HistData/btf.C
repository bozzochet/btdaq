void calib() {

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1,0);

  TCanvas* c0=new TCanvas("c0","piedestaux",0,0,650,650);
  c0->Divide(2,2,0.0001,0.0001);
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

  // lecture 0, position faisceau 3eme AMS02
  c0->cd(1);
  f->cd("0016");
  Piedestaux_0016->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0016->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0016->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0900->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0016->SetLineColor(1);
  Piedestaux_0016->Draw("hist");  
  // lecture 1, position faisceau 1ere AMS02
  c0->cd(2);
  f->cd("0017");
  Piedestaux_0017->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0017->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0017->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0901->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0017->SetLineColor(1);
  Piedestaux_0017->Draw("hist");
  // lecture 2, position faisceau 4eme AMS02
  c0->cd(3);
  f->cd("0012");
  Piedestaux_0012->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0012->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0012->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0904->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0012->SetLineColor(1);
  Piedestaux_0012->Draw("hist");
  // lecture 3, position faisceau 2eme AMS02
  c0->cd(4);
  f->cd("0013");
  Piedestaux_0013->GetXaxis()->SetLabelSize(0.065);
  Piedestaux_0013->GetXaxis()->SetLabelOffset(0.015);
  Piedestaux_0013->GetYaxis()->SetLabelSize(0.060);
  //  Piedestaux_0905->GetXaxis()->SetRangeUser(0.,1.);
  Piedestaux_0013->SetLineColor(1);
  Piedestaux_0013->Draw("hist");

  TCanvas* c1=new TCanvas("c1","sigma",0,0,650,650);
  c1->Divide(2,2,0.0001,0.0001);
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

  int nplaq[4] = { 4, 4, 12, 12 };
  double sig_moyen_long[2][4];
  double err_sig_moyen_long[2][4];
  for (int i=0; i<2; i++)
    for (int j=0; j<4; j++) {
      sig_moyen_long[i][j] = 0.;
      err_sig_moyen_long[i][j] = 0.;
    }  

  // lecture 0, position faisceau 3eme AMS02
  c1->cd(1);
  f->cd("0016");
  Sigma_0016->GetXaxis()->SetLabelSize(0.065);
  Sigma_0016->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0016->GetYaxis()->SetLabelSize(0.060);
  Sigma_0016->SetMaximum(5.);
  int nbins = Sigma_0016->GetNbinsX();
  printf(" det 0 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  int ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0016->GetBinContent(ib) == 0) {
      double cont = Sigma_0016->GetBinContent(ib);
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
    if (Statut_0016->GetBinContent(ib) == 0) {
      double cont = Sigma_0016->GetBinContent(ib);
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
  Sigma_0016->SetLineColor(1);
  Sigma_0016->Draw("hist");  
  // lecture 1, position faisceau 1ere AMS02
  c1->cd(2);
  f->cd("0017");
  Sigma_0017->GetXaxis()->SetLabelSize(0.065);
  Sigma_0017->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0017->GetYaxis()->SetLabelSize(0.060);
  Sigma_0017->SetMaximum(5.);
  //  Sigma_0901->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_0017->SetLineColor(1);
  Sigma_0017->Draw("hist");
  nbins = Sigma_0017->GetNbinsX();
  printf(" det 1 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0017->GetBinContent(ib) == 0) {
      double cont = Sigma_0017->GetBinContent(ib);
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
    if (Statut_0017->GetBinContent(ib) == 0) {
      double cont = Sigma_0017->GetBinContent(ib);
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
  c1->cd(3);
  f->cd("0012");
  Sigma_0012->GetXaxis()->SetLabelSize(0.065);
  Sigma_0012->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0012->GetYaxis()->SetLabelSize(0.060);
  Sigma_0012->SetMaximum(5.);
  //  Sigma_0904->GetXaxis()->SetRangeUser(0.,1.);
  Sigma_0012->SetLineColor(1);
  Sigma_0012->Draw("hist");
  nbins = Sigma_0012->GetNbinsX();
  printf(" det 1 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0012->GetBinContent(ib) == 0) {
      double cont = Sigma_0012->GetBinContent(ib);
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
    if (Statut_0012->GetBinContent(ib) == 0) {
      double cont = Sigma_0012->GetBinContent(ib);
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
  c1->cd(4);
  f->cd("0013");
  Sigma_0013->GetXaxis()->SetLabelSize(0.065);
  Sigma_0013->GetXaxis()->SetLabelOffset(0.015);
  Sigma_0013->GetYaxis()->SetLabelSize(0.060);
  Sigma_0013->SetMaximum(5.);
  nbins = Sigma_0013->GetNbinsX();
  printf(" det 1 nbins %d\n",nbins);
  //  printf("%lf %lf %lf %lf\n",sig_moyen_long[0][0],err_sig_moyen_long[0][0],
  //	 sig_moyen_long[1][0],err_sig_moyen_long[1][0]);
  ncha = 0;
  for (int ib=0; ib<640; ib++) {
    if (Statut_0013->GetBinContent(ib) == 0) {
      double cont = Sigma_0013->GetBinContent(ib);
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
    if (Statut_0013->GetBinContent(ib) == 0) {
      double cont = Sigma_0013->GetBinContent(ib);
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
  Sigma_0013->SetLineColor(1);
  Sigma_0013->Draw("hist");


  TCanvas* c2=new TCanvas("c2","statut",0,0,650,650);
  c2->Divide(2,2,0.0001,0.0001);
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

  // lecture 0, position faisceau 3eme AMS02
  c2->cd(1);
  f->cd("0016");
  Statut_0016->GetXaxis()->SetLabelSize(0.065);
  Statut_0016->GetXaxis()->SetLabelOffset(0.015);
  Statut_0016->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0900->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0016->SetLineColor(1);
  Statut_0016->Draw("hist");  
  // lecture 1, position faisceau 1ere AMS02
  c2->cd(2);
  f->cd("0017");
  Statut_0017->GetXaxis()->SetLabelSize(0.065);
  Statut_0017->GetXaxis()->SetLabelOffset(0.015);
  Statut_0017->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0901->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0017->SetLineColor(1);
  Statut_0017->Draw("hist");
  // lecture 2, position faisceau 4eme AMS02
  c2->cd(3);
  f->cd("0012");
  Statut_0012->GetXaxis()->SetLabelSize(0.065);
  Statut_0012->GetXaxis()->SetLabelOffset(0.015);
  Statut_0012->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0904->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0012->SetLineColor(1);
  Statut_0012->Draw("hist");
  // lecture 3, position faisceau 2eme AMS02
  c2->cd(4);
  f->cd("0013");
  Statut_0013->GetXaxis()->SetLabelSize(0.065);
  Statut_0013->GetXaxis()->SetLabelOffset(0.015);
  Statut_0013->GetYaxis()->SetLabelSize(0.060);
  //  Statut_0905->GetXaxis()->SetRangeUser(0.,1.);
  Statut_0013->SetLineColor(1);
  Statut_0013->Draw("hist");

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

void faisceau() {

  TFile *f1 = new TFile("Hist_1248864558.root");
  //  TFile *f1 = new TFile("Hist_1248872158.root");
  //  TFile *f2 = new TFile("Hist_1248872411.root");
  //  TFile *f1 = new TFile("Hist_1248873492.root");
  TFile *f2 = new TFile("Hist_1248874263.root");

  char chist[60];
  TH1D* cev[2];
  sprintf(chist,"cluster_ev");
  cev[0] = (TH1D*) f1->Get(chist);
  cev[1] = (TH1D*) f2->Get(chist);

  gStyle->SetLabelSize(0.04,"X");
  gStyle->SetLabelSize(0.04,"Y");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);


  TCanvas* c0=new TCanvas("c0","occupation",0,0,500,600);
  c0->SetFillColor(10);
  c0->Divide(2,3,0.0001,0.0001);
  for (int i=0; i<6; i++) {
      c0->cd(i+1);
      gPad->SetFillColor(10);
      gPad->SetFillStyle(4000);
      gPad->Range(-10,-10,10,10);
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      gPad->SetRightMargin(0.12);
      gPad->SetTopMargin(0.10);
      gPad->SetLogy(0);
  }

  TH1D* occ1[4];
  TH1D* occ2[4];
  for (int i=0; i<4; i++) {
    switch (i) {
    case 0: sprintf(chist,"0012/Occupancy_0012"); break;
    case 1: sprintf(chist,"0013/Occupancy_0013"); break;
    case 2: sprintf(chist,"0016/Occupancy_0016"); break;
    case 3: sprintf(chist,"0017/Occupancy_0017"); break;
    default: break;
    }
    printf("chist %s\n",chist);
    occ1[i] = (TH1D*) f1->Get(chist)->Clone();
    printf("occ1 %d f1 %d\n",occ1[i],f1);
    occ1[i]->GetXaxis()->SetTitle("Canal");
    occ1[i]->GetXaxis()->SetTitleSize(0.050);
    occ1[i]->GetXaxis()->CenterTitle();
    //    occ1[i]->SetMinimum(0.5);
    //    occ1[1]->GetYaxis()->SetTitle("Entrees");
    //    occ1[1]->GetYaxis()->SetTitleSize(0.050);
    //    occ1[1]->GetYaxis()->SetTitleOffset(1.50);
    //    occ1[1]->GetYaxis()->CenterTitle();
    occ2[i] = (TH1D*) f2->Get(chist)->Clone();
    occ2[i]->GetXaxis()->SetTitle("Canal");
    occ2[i]->GetXaxis()->SetTitleSize(0.050);
    occ2[i]->GetXaxis()->CenterTitle();
    //    occ2[i]->SetMinimum(0.5);
    printf("occ2 %d f2 %d\n",occ2[i],f2);
    occ2[i]->SetLineColor(kRed);
    c0->cd(i+1);
    if (i != 3) {
    occ1[i]->Draw("hist");
    occ2[i]->Draw("samehist");
    }
    else {
    occ1[i]->Draw("hist");
    occ2[i]->Draw("samehist");
    }
  }

  /*  printf("cev1 %d f1 %d\n",cev1,f1);
  cev1->GetXaxis()->SetTitle("event");
  cev1->GetXaxis()->SetTitleSize(0.050);
  cev1->GetXaxis()->CenterTitle();
  cev1->GetYaxis()->SetTitle("multiplicity");
  cev1->GetYaxis()->SetTitleSize(0.050);
  cev1->GetYaxis()->SetTitleOffset(1.50); 
  cev1->GetYaxis()->CenterTitle(); */
  cev[0]->GetXaxis()->SetRangeUser(0.,10024.);
  c0->cd(5);
  cev[0]->Draw();
  /*  printf("cev2 %d f2 %d\n",cev2,f2);
  cev2->GetXaxis()->SetTitle("event");
  cev2->GetXaxis()->SetTitleSize(0.050);
  cev2->GetXaxis()->CenterTitle();
  cev2->GetYaxis()->SetTitle("multiplicity");
  cev2->GetYaxis()->SetTitleSize(0.050);
  cev2->GetYaxis()->SetTitleOffset(1.50);
  cev2->GetYaxis()->CenterTitle(); */
  cev[1]->SetLineColor(kRed); 
  cev[1]->GetXaxis()->SetRangeUser(0.,10024.);
  c0->cd(6);
  cev[1]->Draw();
 
  return;
}
