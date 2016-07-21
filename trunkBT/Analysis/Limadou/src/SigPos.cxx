#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF1.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TColor.h"
#include "TPaveStats.h"
#include <fstream>
#include <vector>

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hh"
#include "Utilities.hh"
/* end */

using namespace std;


bool goodVA(int va);
void SetStyle();

//originally copied from GainCalibration

int main(int argc, char* argv[]) {

  SetStyle();
  gStyle->SetOptStat("nemruo");

  if (argc<3) {
    printf("Usage:\n");
    printf("%s <output root-filename> <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }
  
  TChain *chain = new TChain("t4");
     
  for (int ii=2; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString output_filename = argv[1];

  /* in case we have to read the alignment
  TString align_filename = "alignment.dat";
  TString gaincorrection_filename = "gaincorrection.dat";
  printf("---------------------------------------------\n");
  Event::ReadAlignment(align_filename.Data());
  Event::ReadGainCorrection(gaincorrection_filename.Data());
  printf("---------------------------------------------\n");
  */

  Event *ev;
  Cluster *cl;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);
  
  ev = new Event();
  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;
  
  if (GetRH(chain)) {
    GetRH(chain)->Print();
    _maxtdr = GetRH(chain)->ntdrCmp;
  }
  else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }
  
  TFile* foutput = new TFile(output_filename.Data(), "RECREATE");
  foutput->cd();

  const char* ss[2] = {"S","K"};
  const Int_t sscolor[2] = {kBlue+1,kRed+1};
  vector<TH1F*> hseedocc[2];
  vector<TH2F*> hseedsig[2];
  vector<TH2F*> htotsig[2];
  vector<TH2F*> hcllength[2];
  vector<TH2F*> hcllengthfirst[2];
  int NSTRIPSS=10*64;
  int NSTRIPSK=6*64;

  Int_t seedsigbins=100;
  Float_t seedsigmin=-10;
  Float_t seedsigmax=+120;

  Int_t totsigbins=100;
  Float_t totsigmin=-150;
  Float_t totsigmax=+700;

  Int_t cllengthbins=70;
  Float_t cllengthmin=0;
  Float_t cllengthmax=70;
  

  for (int tt=0; tt<_maxtdr; tt++) {
    int tdr=GetRH(chain)->tdrCmpMap[tt];
    for(int iside=0; iside<2; iside++)
      { 
	hseedocc[iside].push_back( new TH1F( Form("hseedocc_%s_%02d", ss[iside], tdr), Form("Seed Occupancy - %s - [%02d];Seed ADC Channel;Occurrence", ss[iside], tdr), NSTRIPSS+NSTRIPSK, -0.5, NSTRIPSS+NSTRIPSK-0.5 ) );
	hseedocc[iside][tt]->SetLineColor( sscolor[iside] );
	hseedsig[iside].push_back( new TH2F( Form("hseedsig_%s_%02d", ss[iside], tdr), Form("Seed Signal - %s - [%02d];Seed ADC Channel;Seed ADC counts", ss[iside], tdr), NSTRIPSS+NSTRIPSK, -0.5, NSTRIPSS+NSTRIPSK-0.5, seedsigbins, seedsigmin, seedsigmax ) );
	hseedsig[iside][tt]->SetLineColor( sscolor[iside] );
	htotsig[iside].push_back( new TH2F( Form("htotsig_%s_%02d", ss[iside], tdr), Form("Cluster Signal - %s - [%02d];Seed ADC Channel; Cluster ADC counts", ss[iside], tdr), NSTRIPSS+NSTRIPSK, -0.5, NSTRIPSS+NSTRIPSK-0.5, totsigbins, totsigmin, totsigmax) );
	htotsig[iside][tt]->SetLineColor( sscolor[iside] );

	hcllength[iside].push_back( new TH2F( Form("hcllength_%s_%02d", ss[iside], tdr), Form("Cluster Length - %s - [%02d];Seed ADC Channel;Cluster Length", ss[iside], tdr), NSTRIPSS+NSTRIPSK, -0.5, NSTRIPSS+NSTRIPSK-0.5, cllengthbins, cllengthmin,cllengthmax) );
	hcllength[iside][tt]->SetLineColor( sscolor[iside] );
	hcllengthfirst[iside].push_back( new TH2F( Form("hcllengthfirst_%s_%02d", ss[iside], tdr), Form("Cluster Length - %s - [%02d];First strip ADC Channel;Cluster Length", ss[iside], tdr), NSTRIPSS+NSTRIPSK, -0.5, NSTRIPSS+NSTRIPSK-0.5, cllengthbins, cllengthmin, cllengthmax) );
	hcllengthfirst[iside][tt]->SetLineColor( sscolor[iside] );
	
      }
  }
  
  TStopwatch sw;
  sw.Start();

  double perc=0;
  
  for (int index_event=0; index_event<entries; index_event++) {
    Double_t pperc=20.0*((index_event+1.0)/entries);
    if (pperc>=perc) {
      printf("Processed %d out of %lld: %d%%\n", (index_event+1), entries, (int)(100.0*(index_event+1.0)/entries));
      perc++;
    }

    // if(index_event==10000) break;

    chain->GetEntry(index_event);
    
    int NClusTot = ev->GetNClusTot();
 
    // *** Clean Event if needed **
    ////at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    ////and at most 50 (to avoid too much noise around and too much combinatorial)
    ////at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    /* bool cleanevent = CleanEvent(ev, GetRH(chain), 4, 50, 6, 6, 0, 0);
    if (!cleanevent) continue;
    cleanevs++;
    
    //at least 3 points on S, and 3 points on K, not verbose
    bool trackfitok = ev->FindTrackAndFit(3, 3, false);
    //    printf("%d\n", trackfitok);
    if (trackfitok) {
      //remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas on K
      //(but only if removing them still we'll have more or equal than 3 hits on S and 3 (2 if 'HigherCharge') hits on K)
      //and perform the fit again
      ev->RefineTrack(6.0, 2, 6.0, 2);
    }
    else {
      //let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      //let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be used also for ions)
      //and requiring an higher S/N for the cluster
      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);
    }
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetX0Track(), ev->GetY0Track());    
    tracks++;
    
    //    printf("S %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(0), ev->IsTDRInTrack(0, 0), ev->IsTDRInTrack(0, 4), ev->IsTDRInTrack(0, 8), ev->IsTDRInTrack(0, 12), ev->IsTDRInTrack(0, 14));
    //    printf("K %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(1), ev->IsTDRInTrack(1, 0), ev->IsTDRInTrack(1, 4), ev->IsTDRInTrack(1, 8), ev->IsTDRInTrack(1, 12), ev->IsTDRInTrack(1, 14));
    
    // //                              321098765432109876543210
    // if (ev->GetTrackHitPattern(0) <                100010001) continue;
    // if (ev->GetTrackHitPattern(1) <                100010001) continue;
    
    double logchi = log10(ev->GetChiTrack());
    if (logchi>2) continue;
    goodtracks++;
    
    bool strackok = false;
    bool ktrackok = false;
    
    // example of additional selection
    if (
	ev->IsTDRInTrack(0, 0) &&
	ev->IsTDRInTrack(0, 4) &&
	ev->IsTDRInTrack(0, 8) &&
	(ev->IsTDRInTrack(0, 12) || ev->IsTDRInTrack(0, 14)) ) {
      strackok=true;
      goodStracks++;
    }

    if (
	ev->IsTDRInTrack(1, 0) &&
	ev->IsTDRInTrack(1, 4) &&
	ev->IsTDRInTrack(1, 8) &&
	(ev->IsTDRInTrack(1, 12) || ev->IsTDRInTrack(1, 14)) ) {
      ktrackok=true;
      goodKtracks++;
    }
    
    strackok=true;
    ktrackok=true;

    // if (ev->GetNHitsTrack()>5) {
    //   printf("Nhits: %u (S: %u, K: %u)\n", ev->GetNHitsTrack(), ev->GetNHitsSTrack(), ev->GetNHitsKTrack());
    // }

    chi->Fill(log10(ev->GetChiTrack()));
    theta->Fill(ev->GetThetaTrack());
    phi->Fill(ev->GetPhiTrack());
    */
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
      
      cl = ev->GetCluster(index_cluster);

      int ladder=cl->ladder;
      int side=cl->side;
      int tdr=GetRH(chain)->FindPos(ladder);
      
      // the lenght of the cluster
      int length = cl->GetLength();
      // the address of the first (not the seed!) strip in the cluster
      int first = cl->GetAddress();
      // the position of the seed in the Signal vector ([0-lenght])
      int seed = cl->GetSeed();
      // the strip number of the seed ([0-1024], seed+first)
      int seedadd = cl->GetSeedAdd();
      
      int VAseed  = Cluster::GetVA(seedadd);
      int VAfirst = Cluster::GetVA(first);
      int VAlast  = Cluster::GetVA(first+length);

      // the signal of the seed strip
      float seedsignal = cl->GetSeedVal();
      // the SN of the seed strip
      float seedSN = cl->GetSeedSN();
      // the total CLe noise (sq. mean)
      float totnoise = cl->GetTotNoise();
      // the CoG of the cluster using 2 strips
      float cog = cl->GetCoG();
      // the Total Signal of the cluster using 2 strips
      float sig = cl->GetSig();
      // the Total Signal of the cluster
      float totsig = cl->GetTotSig();
      // the Cluster SN
      float totsn = cl->GetTotSN();
      // the charge (using TotSig()) and dividing by a tempary 'MIP' sig
      double charge=cl->GetCharge();
      
      hseedocc[side][tdr]->Fill( seedadd );
      hseedsig[side][tdr]->Fill( seedadd, seedsignal );
      htotsig[side][tdr]->Fill( seedadd, totsig );
      hcllength[side][tdr]->Fill( seedadd, length );
      hcllengthfirst[side][tdr]->Fill( first, length );
    }
    
  }

  TString pdfname( foutput->GetName() ); pdfname.ReplaceAll(".root",".pdf");
  TCanvas *cdummy = new TCanvas();
  cdummy->SaveAs( Form("%s(",pdfname.Data()) );
  TPaveStats *st=NULL;

  for(int tdr=0; tdr<_maxtdr; tdr++)
    {
      if(tdr!=0) break;

      TCanvas *cseedsig = new TCanvas( Form("cseedsig_tdr%d",tdr), Form("cseedsig_tdr%d",tdr) );
      cseedsig->cd();
      hseedsig[0][tdr]->SetStats(0);
      hseedsig[0][tdr]->Draw("COLZ");
      hseedsig[1][tdr]->Draw("COL same");
      for(int i=1; i<16; i++){ TLine *l = new TLine(64*i,seedsigmin,64*i,seedsigmax); l->SetLineStyle( i==10 ? 1 : 3); l->Draw("same"); }
      cseedsig->SaveAs( Form("%s",pdfname.Data()) );

      for(int i=0; i<16; i++){ 
	if(!goodVA(i)) continue;
	TCanvas *cseedsigVA = new TCanvas( Form("cseedsig_tdr%d_VA%02d",tdr,i), Form("cseedsig_tdr%d_VA%02d",tdr,i) );
	cseedsigVA->cd()->SetLogy();
	int side =  (i<10) ? 0 : 1;
	TH2F* h2 = hseedsig[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("hseedsig_tdr_%d_VA%02d",tdr,i), 64*i, 64*(i+1));
	h->GetYaxis()->SetTitle("Occurence");
	h->SetTitle(Form("%s - VA[%02d]", h2->GetTitle(), i));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	cseedsigVA->SaveAs( Form("%s",pdfname.Data()) );
      }

      TCanvas *ctotsig = new TCanvas( Form("ctotsig_tdr%d",tdr), Form("ctotsig_tdr%d",tdr) );
      ctotsig->cd();
      htotsig[0][tdr]->SetStats(0);
      htotsig[0][tdr]->Draw("COLZ");
      htotsig[1][tdr]->Draw("COL same");
      for(int i=1; i<16; i++){ TLine *l = new TLine(64*i,totsigmin,64*i,totsigmax); l->SetLineStyle( i==10 ? 1 : 3); l->Draw("same"); }
      ctotsig->SaveAs( Form("%s",pdfname.Data()) );

      for(int i=0; i<16; i++){ 
	if(!goodVA(i)) continue;
	TCanvas *ctotsigVA = new TCanvas( Form("ctotsig_tdr%d_VA%02d",tdr,i), Form("ctotsig_tdr%d_VA%02d",tdr,i) );
	ctotsigVA->cd()->SetLogy();
	int side =  (i<10) ? 0 : 1;
	TH2F* h2 = htotsig[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("htotsig_tdr%d_VA%02d",tdr,i), 64*i, 64*(i+1));
	h->GetYaxis()->SetTitle("Occurence");
	h->SetTitle(Form("%s - VA[%02d]", h2->GetTitle(), i));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	ctotsigVA->SaveAs( Form("%s",pdfname.Data()) );
      }

      TCanvas *ccllength = new TCanvas( Form("ccllength_tdr%d",tdr), Form("ccllength_tdr%d",tdr) );
      ccllength->cd();
      hcllengthfirst[0][tdr]->SetStats(0);
      hcllengthfirst[0][tdr]->Draw("COLZ");
      hcllengthfirst[1][tdr]->Draw("COL same");
      for(int i=1; i<16; i++){ TLine *l = new TLine(64*i,cllengthmin,64*i,cllengthmax); l->SetLineStyle( i==10 ? 1 : 3); l->Draw("same"); }
      ccllength->SaveAs( Form("%s",pdfname.Data()) );

      for(int i=0; i<16; i++){ 
	if(!goodVA(i)) continue;
	TCanvas *ccllengthVA = new TCanvas( Form("ccllength_tdr%d_VA%02d",tdr,i), Form("ccllength_tdr%d_VA%02d",tdr,i) );
	ccllengthVA->cd()->SetLogy();
	int side =  (i<10) ? 0 : 1;
	TH2F* h2 = hcllengthfirst[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("hcllength_tdr%d_VA%02d",tdr,i), 64*i, 64*(i+1));
	h->GetYaxis()->SetTitle("Occurence");
	h->SetTitle(Form("%s - VA[%02d]", h2->GetTitle(), i));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	ccllengthVA->SaveAs( Form("%s",pdfname.Data()) );
      }
  }

  cdummy->SaveAs( Form("%s)",pdfname.Data()) );

  sw.Stop();
  sw.Print();
  
  
  foutput->Write();
  foutput->Close();
  
  return 0;
}

bool goodVA(int va){
  if( va<=2 || (va>=5&&va<=7) || (va>=10&&va<=12) ) return true;
  else return false; 
}

void SetStyle(){
  //Canvas
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gStyle->SetStatColor(kWhite);
  gStyle->SetPalette(1,0);
  gStyle->SetOptFit(1111);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetLegendBorderSize();
  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetFrameBorderSize(2);
  gStyle->SetGridWidth(0.5);
  gStyle->SetTickLength(0.05);
  gStyle->SetStatX(0.84);
  gStyle->SetStatY(0.89);
  gStyle->SetStatH(0.1);
  gStyle->SetStatW(0.20);
  
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetPadBottomMargin(0.15);
  
  //Legend
  gStyle->SetLegendBorderSize(1);
  
  //Graphs
  gStyle->SetEndErrorSize(0);
  gStyle->SetErrorX(0);
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1);
  gStyle->SetLineWidth(2);

  //Axis Labels
  gStyle->SetLabelSize(0.06,"X");
  gStyle->SetLabelSize(0.06,"Y");
  gStyle->SetLabelSize(0.06,"Z");
  gStyle->SetTitleSize(0.06,"X");
  gStyle->SetTitleSize(0.06,"Y");
  gStyle->SetTitleSize(0.06,"Z");
  gStyle->SetTitleSize(0.07,"title");
  gStyle->SetTitleOffset(1.10,"X");
  gStyle->SetTitleOffset(1.05,"Y");
  gStyle->SetTitleOffset(1.00,"Z");
  gStyle->SetTitleFont(62,"X");
  gStyle->SetTitleFont(62,"Y");
  gStyle->SetTitleFont(62,"Z");
  gStyle->SetTitleFont(62,"title");

  //Color Palette
  const int NRGBs = 5;
  const int NCont = 255;
  double stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  double red[NRGBs] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  double green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  double blue[NRGBs] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs,stops,red,green,blue,NCont);
  gStyle->SetNumberContours(NCont);   
  
  return;
}
