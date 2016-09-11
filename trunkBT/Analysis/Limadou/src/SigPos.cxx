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
#include "TPaveText.h"
#include "TTimeStamp.h"

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


bool GoodSelection(int va);
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
    _maxtdr = GetRH(chain)->ntdrCmp + GetRH(chain)->ntdrRaw;
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
  vector<TH2F*> hcllengthtotsig[16];
  int NSTRIPSS=10*64;
  int NSTRIPSK=6*64;

  //Histogram axis
  // Int_t seedsigbins[2] = {100, 100};
  // Float_t seedsigmin[2] = {-10, -50};
  // Float_t seedsigmax[2] = {90, 200};

  Int_t seedsigbins = 250;
  Float_t seedsigmin = -50;
  Float_t seedsigmax = 200;

  Int_t totsigbins=100;
  Float_t totsigmin=-150;
  Float_t totsigmax=+500;

  Int_t cllengthbins=11;
  Float_t cllengthmin=-0.5;
  Float_t cllengthmax=10.5;
  

  for (int tt=0; tt<_maxtdr; tt++) {
    int tdr=GetRH(chain)->tdrCmpMap[tt];
    if (tdr<0) {
      tdr=GetRH(chain)->tdrRawMap[tt];
    }
    //    printf("tdr = %d\n", tdr);
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
    for(int iva=0; iva<16; iva++)
      {
	hcllengthtotsig[iva].push_back( new TH2F( Form("hcllengthtotsig_VA%02d_%02d", iva, tdr), Form("seed VA[%02d] - [%02d];Cluster Length; Cluster ADC counts", iva, tdr), cllengthbins, cllengthmin, cllengthmax, totsigbins, totsigmin, totsigmax) );
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

    //if(index_event==1000) break;

    chain->GetEntry(index_event);

    int NClusTot = ev->GetNClusTot();
    //    printf("Found %d clusters\n", NClusTot);

    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {

      cl = ev->GetCluster(index_cluster);

      int ladder=cl->ladder;
      int side=cl->side;
      int tdr=GetRH(chain)->FindPos(ladder);
      if (tdr<0) tdr=GetRH(chain)->FindPosRaw(ladder);
      //      printf("tdr = %d\n", tdr);

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

      if( !GoodSelection(VAseed) ) continue;
 
     hseedocc[side][tdr]->Fill( seedadd );
      hseedsig[side][tdr]->Fill( seedadd, seedsignal );
      htotsig[side][tdr]->Fill( seedadd, totsig );
      hcllength[side][tdr]->Fill( seedadd, length );
      hcllengthfirst[side][tdr]->Fill( first, length );
      hcllengthtotsig[VAseed][tdr]->Fill( length, totsig );
    }
    
  }

  TString pdfname( foutput->GetName() ); pdfname.ReplaceAll(".root",".pdf");
  TCanvas *cdummy = new TCanvas();
  TPaveText *text = new TPaveText(0.05, 0.30, 0.95, 0.70); text->SetBorderSize(0); text->SetFillStyle(0);
  text->AddText( argv[0] );
  text->AddText( Form("Output: %s", argv[1]) );
  TTimeStamp tst; text->AddText( tst.AsString("s") );
  text->Draw("");
  cdummy->SaveAs( Form("%s(",pdfname.Data()) );
  TPaveStats *st=NULL;

  for(int tdr=0; tdr<_maxtdr; tdr++)
    {
      //if(tdr!=0) break;

      TCanvas *cseedsig = new TCanvas( Form("cseedsig_tdr%d",tdr), Form("cseedsig_tdr%d",tdr) );
      cseedsig->cd();
      hseedsig[0][tdr]->SetStats(0);
      hseedsig[0][tdr]->Draw("COLZ");
      hseedsig[1][tdr]->Draw("COL same");
      for(int i=1; i<16; i++){ TLine *l = new TLine(64*i,seedsigmin,64*i,seedsigmax); l->SetLineStyle( i==10 ? 1 : 3); l->Draw("same"); }
      cseedsig->SaveAs( Form("%s",pdfname.Data()) );

      for(int i=0; i<16; i++){ 
	if(!GoodSelection(i)) continue;
	TCanvas *cseedsigVA = new TCanvas( Form("cseedsig_tdr%d_VA%02d",tdr,i), Form("cseedsig_tdr%d_VA%02d",tdr,i) );
	cseedsigVA->cd()->SetLogy();
	int side =  (i<10) ? 0 : 1;
	TH2F* h2 = hseedsig[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("hseedsig_tdr_%d_VA%02d",tdr,i), 64*i, 64*(i+1));
	h->GetYaxis()->SetTitle("Occurrence");
	h->SetTitle(Form("%s - VA[%02d]", h2->GetTitle(), i));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	//	cseedsigVA->SaveAs( Form("%s",pdfname.Data()) );
      }

      for(int i=0; i<3; i++){ 
	int j=i*5;
	TCanvas *cseedsig3VA = new TCanvas( Form("cseedsig_tdr%d_3VA%02d",tdr,i), Form("cseedsig_tdr%d_3VA%02d",tdr,i) );
	cseedsig3VA->cd()->SetLogy();
	int side =  (j<10) ? 0 : 1;
	TH2F* h2 = hseedsig[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("hseedsig_tdr_%d_3VA%02d",tdr,i), 64*j, 64*(j+3));
	h->GetYaxis()->SetTitle("Occurrence");
	h->SetTitle(Form("%s - VA[%02d-%02d-%02d]", h2->GetTitle(), j, j+1, j+2));
	h->SetLineColor( sscolor[side]  );
	if( side==0 ) h->GetXaxis()->SetRangeUser(-10,90);
	h->Draw("");
	cseedsig3VA->SaveAs( Form("%s",pdfname.Data()) );
      }


      TCanvas *ctotsig = new TCanvas( Form("ctotsig_tdr%d",tdr), Form("ctotsig_tdr%d",tdr) );
      ctotsig->cd();
      htotsig[0][tdr]->SetStats(0);
      htotsig[0][tdr]->Draw("COLZ");
      htotsig[1][tdr]->Draw("COL same");
      for(int i=1; i<16; i++){ TLine *l = new TLine(64*i,totsigmin,64*i,totsigmax); l->SetLineStyle( i==10 ? 1 : 3); l->Draw("same"); }
      //ctotsig->SaveAs( Form("%s",pdfname.Data()) );

      for(int i=0; i<16; i++){ 
	if(!GoodSelection(i)) continue;
	TCanvas *ctotsigVA = new TCanvas( Form("ctotsig_tdr%d_VA%02d",tdr,i), Form("ctotsig_tdr%d_VA%02d",tdr,i) );
	ctotsigVA->cd()->SetLogy();
	int side =  (i<10) ? 0 : 1;
	TH2F* h2 = htotsig[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("htotsig_tdr%d_VA%02d",tdr,i), 64*i, 64*(i+1));
	h->GetYaxis()->SetTitle("Occurrence");
	h->SetTitle(Form("%s - VA[%02d]", h2->GetTitle(), i));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	//ctotsigVA->SaveAs( Form("%s",pdfname.Data()) );
      }

      for(int i=0; i<3; i++){ 
	int j=i*5;
	TCanvas *ctotsig3VA = new TCanvas( Form("ctotsig_tdr%d_3VA%02d",tdr,i), Form("ctotsig_tdr%d_3VA%02d",tdr,i) );
	ctotsig3VA->cd()->SetLogy();
	int side =  (j<10) ? 0 : 1;
	TH2F* h2 = htotsig[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("htotsig_tdr_%d_3VA%02d",tdr,i), 64*j, 64*(j+3));
	h->GetYaxis()->SetTitle("Occurrence");
	h->SetTitle(Form("%s - VA[%02d-%02d-%02d]", h2->GetTitle(), j, j+1, j+2));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	//ctotsig3VA->SaveAs( Form("%s",pdfname.Data()) );
      }


      TCanvas *ccllength = new TCanvas( Form("ccllength_tdr%d",tdr), Form("ccllength_tdr%d",tdr) );
      ccllength->cd();
      hcllengthfirst[0][tdr]->SetStats(0);
      hcllengthfirst[0][tdr]->Draw("COLZ");
      hcllengthfirst[1][tdr]->Draw("COL same");
      for(int i=1; i<16; i++){ TLine *l = new TLine(64*i,cllengthmin,64*i,cllengthmax); l->SetLineStyle( i==10 ? 1 : 3); l->Draw("same"); }
      ccllength->SaveAs( Form("%s",pdfname.Data()) );

      for(int i=0; i<16; i++){ 
	if(!GoodSelection(i)) continue;
	TCanvas *ccllengthVA = new TCanvas( Form("ccllength_tdr%d_VA%02d",tdr,i), Form("ccllength_tdr%d_VA%02d",tdr,i) );
	ccllengthVA->cd()->SetLogy();
	int side =  (i<10) ? 0 : 1;
	TH2F* h2 = hcllengthfirst[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("hcllength_tdr%d_VA%02d",tdr,i), 64*i, 64*(i+1));
	h->GetYaxis()->SetTitle("Occurrence");
	h->SetTitle(Form("%s - VA[%02d]", h2->GetTitle(), i));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	//ccllengthVA->SaveAs( Form("%s",pdfname.Data()) );
      }

      for(int i=0; i<3; i++){ 
	int j=i*5;
	TCanvas *ccllenght3VA = new TCanvas( Form("ccllenght_tdr%d_3VA%02d",tdr,i), Form("ccllenght_tdr%d_3VA%02d",tdr,i) );
	ccllenght3VA->cd()->SetLogy();
	int side =  (j<10) ? 0 : 1;
	TH2F* h2 = hcllengthfirst[side][tdr];
	TH1F* h = (TH1F*)h2->ProjectionY( Form("hcllenght_tdr_%d_3VA%02d",tdr,i), 64*j, 64*(j+3));
	h->GetYaxis()->SetTitle("Occurrence");
	h->SetTitle(Form("%s - VA[%02d-%02d-%02d]", h2->GetTitle(), j, j+1, j+2));
	h->SetLineColor( sscolor[side]  );
	h->Draw("");
	ccllenght3VA->SaveAs( Form("%s",pdfname.Data()) );
      }

      for( int i=0; i<3; i++){
	TCanvas *ccllengthtotsig = new TCanvas( Form("ccllengthtotsig_VA%d_tdr%d",i,tdr), Form("ccllengthtotsig_VA%d_tdr%d",i,tdr) );
	ccllengthtotsig->cd()->SetLogz(); ccllengthtotsig->cd()->SetGrid(); ccllengthtotsig->cd()->SetTicks();
	TH2F *h3VA = (TH2F*)hcllengthtotsig[i*5][tdr]->Clone( Form("h3VA_%d",i ) );
	h3VA->SetNameTitle( Form("hcllengthtotsig_3VA%02d_tdr%02d",i,GetRH(chain)->tdrCmpMap[tdr]), Form("seed VA [%02d-%02d-%02d] -- TDR[%02d]", i*5, i*5+1, i*5+2, GetRH(chain)->tdrCmpMap[tdr]>0?GetRH(chain)->tdrCmpMap[tdr]:GetRH(chain)->tdrRawMap[tdr]) );
	h3VA->Add( hcllengthtotsig[i*5+1][tdr] );
	h3VA->Add( hcllengthtotsig[i*5+2][tdr] );
	h3VA->SetStats(0);;
	h3VA->Draw("COLZ");
	ccllengthtotsig->SaveAs( Form("%s",pdfname.Data()) );
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

bool GoodSelection( int va ){
  //return true;
  if( !goodVA(va) ) return false; else return true;
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
