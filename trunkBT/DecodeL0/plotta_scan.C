{
  TTree* t = new TTree("t", "");
  t->ReadFile("LaserDelayScans.csv", "Delay/D:miniTRB_ADC:miniTRB_Norm:FOOT3.5_ADC:FOOT3.5_Norm:FOOT5.5_ADC:FOOT5.5_Norm");

  {
    TCanvas* c = new TCanvas("ADC");
    TH1* h = c->DrawFrame(0, 0, 20, 800);
    h->GetXaxis()->SetTitle("Delay (#mus)");
    h->GetYaxis()->SetTitle("ADC");
  
    {
      Int_t n = t->Draw("Delay:FOOT5.5_ADC", "", "goff");
      Double_t* vx = t->GetVal(0);
      Double_t* vy = t->GetVal(1);
    
      TGraph* foot55 = new TGraph(n, vx, vy);
      foot55->SetMarkerColor(kRed+1);
      foot55->SetLineColor(kRed+1);
      foot55->Draw("P");
    }

    {
      Int_t n = t->Draw("Delay:FOOT3.5_ADC", "", "goff");
      Double_t* vx = t->GetVal(0);
      Double_t* vy = t->GetVal(1);
    
      TGraph* foot35 = new TGraph(n, vx, vy);
      foot35->SetMarkerColor(kBlue+1);
      foot35->SetLineColor(kBlue+1);
      foot35->Draw("P");
    }
  }

  {
    TCanvas* c = new TCanvas("Norm");
    TH1* h = c->DrawFrame(0, 0.3, 20, 1.2);
    h->GetXaxis()->SetTitle("Delay (#mus)");
    h->GetYaxis()->SetTitle("Norm. ADIC");
  
    {
      Int_t n = t->Draw("Delay:FOOT5.5_Norm", "", "goff");
      Double_t* vx = t->GetVal(0);
      Double_t* vy = t->GetVal(1);
    
      TGraph* foot55 = new TGraph(n, vx, vy);
      foot55->SetMarkerColor(kRed+1);
      foot55->SetLineColor(kRed+1);
      foot55->Draw("P");
    }

    {
      Int_t n = t->Draw("Delay:FOOT3.5_Norm", "", "goff");
      Double_t* vx = t->GetVal(0);
      Double_t* vy = t->GetVal(1);
    
      TGraph* foot35 = new TGraph(n, vx, vy);
      foot35->SetMarkerColor(kBlue+1);
      foot35->SetLineColor(kBlue+1);
      foot35->Draw("P");
    }
  }
  
}
