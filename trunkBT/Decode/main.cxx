#include <iostream>

#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include <Compression.h>

#include "DecodeData.hh"
#include "Event.hh"
#include <stdlib.h>
#include <math.h>

#include "anyoption.h"
#include <getopt.h>

using namespace std;

AnyOption *opt;                //Handle the option input
char progname[50];


void CreatePdfWithPlots(DecodeData* dd1, char* pdf_filename);
void PlotsWithFits(TH1* histo, char* name, char* title, char* pdf_filename);

int main(int argc,char** argv){

  char filename[255], pdf_filename[1024];

  char DirRaw[255];
  char DirCal[255];
  char DirRoot[255];

  double shighthreshold=3.5;
  double slowthreshold=1.0;
  double khighthreshold=3.5;
  double klowthreshold=1.0;

  bool kClusterize=false;
  int cworkaround=0;

  int run=110;
  int ancillary=-1;

  int processed=0;
  int jinffailed=0;
  int readfailed=0;

  sprintf(DirRoot,"./RootData/");
  sprintf(DirRaw,"./RawData/");
  sprintf(DirCal,"./CalData/");

  opt = new AnyOption();

  opt->addUsage("Usage: ./Decode [options] [arguments]");
  opt->addUsage("" );
  opt->addUsage("Options: ");
  opt->addUsage(     "  -h, --help  ................................. Print this help " );
  opt->addUsage(Form("  --rawdata <path/to/dir/with/raw> ............ Directory with raw data (%s is the default)", DirRaw));
  opt->addUsage(Form("  --caldata <path/to/dir/with/cal> ............ Directory with cal data (%s is the default)", DirCal));
  opt->addUsage(Form("  --rootdata <path/to/dir/for/root> ........... Directory where to put ROOT file (%s is the default)", DirRoot));
  opt->addUsage(     "  -c, --clusterize ............................ To perform an offline clusterization to the RAW event.");
  opt->addUsage(     "  --shighthreshold <X> ........................ S-side S/N high threshold. Used in the offline clusterization if option -c or to fill the plots for the ladders with raw events (3.5 is the default)");
  opt->addUsage(     "  --slowthreshold  <X> ........................ S-side S/N low threshold. Used in the offline clusterization if option -c or to fill the plots for the ladders with raw events (1.0 is the default)");
  opt->addUsage(     "  --khighthreshold <X> ........................ K-side S/N high threshold. Used in the offline clusterization if option -c or to fill the plots for the ladders with raw events (3.5 is the default)");
  opt->addUsage(     "  --klowthreshold  <X> ........................ K-side S/N low threshold. Used in the offline clusterization if option -c or to fill the plots for the ladders with raw events (1.0 is the default)");
  opt->addUsage(     "  --cworkaround <N> ........................... To choose the workaround clusterization:");
  opt->addUsage(     "                                                    0 is the standard one (default)");
  opt->addUsage(     "                                                    1 for the Limadou monster. On S-side only even channels are bonded");
  opt->addUsage("" );
  opt->addUsage("Arguments: " );
  opt->addUsage("  <runnum> [ancillary code (-1 is the default)]" );

  //***********
  //set Flags
  //***********
  opt->setFlag("help", 'h');
  opt->setFlag("clusterize", 'c');

  //***********
  //set Options
  //***********
  opt->setOption("rawdata");
  opt->setOption("caldata");
  opt->setOption("rootdata");
  opt->setOption("shighthreshold");
  opt->setOption("slowthreshold");
  opt->setOption("khighthreshold");
  opt->setOption("klowthreshold");
  opt->setOption("cworkaround");
  
  //****************
  //Get Line Command
  //****************
  opt->processCommandArgs( argc, argv );

  //*************
  //Get Flags
  //*************
  if (opt->getFlag("help") || opt->getFlag('h')){
    opt->printUsage();
    exit(2);
  }

  if (opt->getFlag("clusterize") || opt->getFlag('c')){
    kClusterize = true;
  }

  //*********
  //Get Options
  //*********
  if (opt->getValue("rawdata")) {
    sprintf(DirRaw,"%s/", opt->getValue("rawdata"));
  }
  
  if (opt->getValue("caldata")) {
    sprintf(DirCal,"%s/", opt->getValue("caldata"));
  }
  
  if (opt->getValue("rootdata")) {
    sprintf(DirRoot,"%s/", opt->getValue("rootdata"));
  }

  if (opt->getValue("shighthreshold")) {
    shighthreshold = atof(opt->getValue("shighthreshold"));
  }

  if (opt->getValue("slowthreshold")) {
    slowthreshold = atof(opt->getValue("slowthreshold"));
  }

  if (opt->getValue("khighthreshold")) {
    khighthreshold = atof(opt->getValue("khighthreshold"));
  }

  if (opt->getValue("klowthreshold")) {
    klowthreshold = atof(opt->getValue("klowthreshold"));
  }

  if (opt->getValue("cworkaround")) {
    cworkaround = atoi(opt->getValue("cworkaround"));
  }
  
  //  printf("%d %f %f %f %f\n", kClusterize, shighthreshold, slowthreshold, khighthreshold, klowthreshold);

  //*************
  //Get Arguments
  //************

  switch(opt->getArgc()){

  case 1:
    run = atoi(opt->getArgv(0));
    ancillary = -1;
    break;

  case 2:
    run = atoi(opt->getArgv(0));
    ancillary = atoi(opt->getArgv(1));
    break;
    
  default:
    opt->printUsage();
    exit(-1);
    break;
  }

  sprintf(progname,"%s", argv[0]);

  printf("Reading Raw Data from %s\n", DirRaw);
  printf("Reading Cal Data from %s\n", DirCal);
  printf("Writing output in %s\n", DirRoot);

  if (ancillary < 0)
    sprintf(filename,"%s/run_%06d.root", DirRoot, run);
  else
    sprintf(filename,"%s/run_%06d_ANC_%d.root", DirRoot, run, ancillary);
  sprintf(pdf_filename, "%s.pdf", filename);

  int complevel=ROOT::CompressionSettings(ROOT::kLZMA, 2);
  printf("The choosen compression level is %d\n", complevel);
  TFile* foutput = new TFile(filename, "RECREATE", "File with the event tree", complevel);
  
  DecodeData *dd1= new DecodeData(DirRaw, DirCal, run, ancillary);

  dd1->shighthreshold=shighthreshold;
  dd1->slowthreshold=slowthreshold;
  dd1->khighthreshold=khighthreshold;
  dd1->klowthreshold=klowthreshold;
  dd1->kClusterize=kClusterize;
  dd1->cworkaround=cworkaround;
  
  dd1->SetPrintOff();
  dd1->SetEvPrintOff();

  // printf("%d + %d\n", dd1->GetNTDRRaw(), dd1->GetNTDRCmp());
  // for (int ii=0; ii<dd1->GetNTDRRaw(); ii++) {
  //   printf("%d) %d\n", ii, dd1->GetIdTDRRaw(ii));
  // }
  // for (int ii=0; ii<dd1->GetNTDRCmp(); ii++) {
  //   printf("%d) %d\n", ii, dd1->GetIdTDRCmp(ii));
  // }
  
  TTree* t4= new TTree("t4","My cluster tree");
  t4->Branch("cluster_branch","Event",&(dd1->ev),32000,2);
  double chaK[24];
  double chaS[24];
  double sigK[24];
  double sigS[24];
  double sonK[24];
  double sonS[24];
  for (int jj=0; jj<2; jj++) {
    int NTDR=0;
    if (jj==0) NTDR = dd1->GetNTDRRaw();
    else if (jj==1) NTDR = dd1->GetNTDRCmp();
    for (int ii=0; ii<NTDR; ii++) {
      int IdTDR=-1;
      if (jj==0) IdTDR = dd1->GetIdTDRRaw(ii);
      else if (jj==1) IdTDR = dd1->GetIdTDRCmp(ii);
      //      printf("%d\n", IdTDR);
      t4->Branch(Form("SignalK_Ladder%02d", IdTDR), &sigK[IdTDR], Form("SignalK_Ladder%02d/D", IdTDR));
      t4->Branch(Form("SignalS_Ladder%02d", IdTDR), &sigS[IdTDR], Form("SignalS_Ladder%02d/D", IdTDR));
      t4->Branch(Form("ChargeK_Ladder%02d", IdTDR), &chaK[IdTDR], Form("ChargeK_Ladder%02d/D", IdTDR));
      t4->Branch(Form("ChargeS_Ladder%02d", IdTDR), &chaS[IdTDR], Form("ChargeS_Ladder%02d/D", IdTDR));
      t4->Branch(Form("SoNK_Ladder%02d", IdTDR), &sonK[IdTDR], Form("SoNK_Ladder%02d/D", IdTDR));
      t4->Branch(Form("SoNS_Ladder%02d", IdTDR), &sonS[IdTDR], Form("SoNS_Ladder%02d/D", IdTDR));
    }
  }
  sleep(3);
  t4->GetUserInfo()->Add(dd1->rh);

  TObjArray* obj = t4->GetListOfBranches();
  for (int ii=0; ii<obj->GetEntries(); ii++) {
    TBranch* branch = (TBranch*)(obj->At(ii));
    branch->SetCompressionLevel(6);
  }
    
  int ret1=0;
  while (1) {
    
    ret1=dd1->EndOfFile();    
    if (ret1) break;
    
    ret1=dd1->ReadOneEvent();
    //    printf("%d\n", ret1);
    
    ret1=dd1->EndOfFile();    
    if (ret1) break;
    
    if (ret1==0) {
      processed++;
      //      printf("This event has %d clusters\n", (dd1->ev)->GetNClusTot());
      memset(chaK, 0, 24*sizeof(chaK[0]));
      memset(chaS, 0, 24*sizeof(chaS[0]));
      memset(sigK, 0, 24*sizeof(sigK[0]));
      memset(sigS, 0, 24*sizeof(sigS[0]));
      for (int cc=0; cc<(dd1->ev)->GetNClusTot(); cc++) {
	Cluster* cl = (dd1->ev)->GetCluster(cc);
	int ladder = cl->ladder;
	//	printf("%d\n", ladder);
	double signal = cl->GetTotSig();
	double charge = cl->GetCharge();
	double son = cl->GetTotSN();
	if (cl->side==1) {
	  if (charge>chaK[ladder]) {
	    chaK[ladder]=charge;
	    sigK[ladder]=signal;
	    sonK[ladder]=son;
	  }
	}
	else{
	  if (charge>chaS[ladder]) {
	    chaS[ladder]=charge;
	    sigS[ladder]=signal;
	    sonS[ladder]=son;
	  }
	}	
      }
      //      printf("%f %f %f %f %f %f\n", sigS[0], sigK[0], sigS[1], sigK[1], sigS[4], sigK[4]);
      t4->Fill();
      if (processed%1000==0) printf("Processed %d events...\n", processed);
    }
    else if(ret1==-1){
      printf("=======================> END of FILE\n");
      break;
    }
    else if(ret1<-1){
      printf("=======================> READ Error Event Skipped\n");
      readfailed++;
      break;
    }
    else{
      jinffailed++;
    }


    dd1->ev->Clear();

  }

  CreatePdfWithPlots(dd1, pdf_filename);
    
  t4->Write("",TObject::kOverwrite);

  printf("\nProcessed %5d  Events\n",processed+readfailed+jinffailed);
  printf("Accepted  %5d  Events\n",processed);
  printf("Rejected  %5d  Events --> Read Error\n",readfailed);
  printf("Rejected  %5d  Events --> Jinf/Jinj Error\n",jinffailed);

  delete dd1;
  
  foutput->Write("",TObject::kOverwrite);
  foutput->Close("R");

  return 0;
}

void PlotsWithFits(TH1* histo, char* name, char* title, char* pdf_filename) {  

  static bool first=true;
  char local_pdf_filename[255];

  TCanvas* canvas = new TCanvas(name, name, 1024, 1024);
  TF1 *fit_s = new TF1("fit_s", "gaus", 0, 639);
  TF1 *fit_k = new TF1("fit_k", "gaus", 640, 1023);
  fit_s->SetLineColor(kBlue);
  fit_k->SetLineColor(kRed);
  int entries = (int)(histo->GetEntries());
  if (entries>=1) {
    TH1F *clone_chartA = (TH1F *)histo->Clone("cloneA");
    TH1F *clone_chartB = (TH1F *)histo->Clone("cloneB");
    clone_chartA->Fit(fit_s, "R");
    clone_chartB->Fit(fit_k, "R");
    clone_chartA->Draw();
    gPad->Modified();
    gPad->Update();
    TPaveStats *statA = (TPaveStats*)(clone_chartA->GetListOfFunctions()->FindObject("stats"));
    clone_chartB->Draw("SAMES");
    gPad->Modified();
    gPad->Update();
    TPaveStats *statB = (TPaveStats*)(clone_chartB->GetListOfFunctions()->FindObject("stats"));
    if(statA && statB) {
      statA->SetTextColor(kBlue);
      statB->SetTextColor(kRed);
      statA->SetX1NDC(0.12); statA->SetX2NDC(0.32); statA->SetY1NDC(0.75);
      statB->SetX1NDC(0.72); statB->SetX2NDC(0.92); statB->SetY1NDC(0.78);
      statA->Draw();
      canvas->Update();
    }
    canvas->Update();
    canvas->Modified();
    canvas->Update();
    canvas->SetTitle(title);
    if (!first) strcpy(local_pdf_filename, pdf_filename);
    else {
      sprintf(local_pdf_filename, "%s(", pdf_filename);
      first=false;
    }
    canvas->Print(local_pdf_filename, "pdf");
    if (clone_chartA) delete clone_chartA;
    if (clone_chartB) delete clone_chartB;
  }
  delete canvas;
  if (fit_s) delete fit_s;
  if (fit_k) delete fit_k;

  return;
}

void CreatePdfWithPlots(DecodeData* dd1, char* pdf_filename){

  char local_pdf_filename[255];
  char title[255];
  char name[255];

  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);

  for (int jj=0; jj<NJINF; jj++){
    for (int hh = 0; hh < NTDRS; hh++) {
      sprintf(name, "ladder %d %d", jj, hh);
      sprintf(title, "ladder %d %d", jj, hh);
      PlotsWithFits(dd1->hocc[NTDRS*jj+hh], name, title, pdf_filename);
    }
  }

  for (int jj=0; jj<NJINF; jj++){
    for (int hh = 0; hh < NTDRS; hh++) {
      sprintf(name, "ladder %d %d", jj, hh);
      sprintf(title, "ladder %d %d", jj, hh);
      PlotsWithFits(dd1->hoccseed[NTDRS*jj+hh], name, title, pdf_filename);
    }
  }

  for (int jj=0; jj<NJINF; jj++){
    for (int hh = 0; hh < NTDRS; hh++) {
      for (int ss = 0; ss < 2; ss++) {
	TCanvas* canvas = new TCanvas("dummy", "dummy", 1024, 1024);
	canvas->SetLogy(true);
	dd1->hsignal[jj*NTDRS+hh][ss]->Draw();
	int entries = (int)(dd1->hsignal[NTDRS*jj+hh][ss]->GetEntries());
	if (entries>=1) {
	  double mean = (dd1->hsignal[NTDRS*jj+hh][ss]->GetMean());
	  double rms = (dd1->hsignal[NTDRS*jj+hh][ss]->GetRMS());
	  //	  printf("%f %f\n", mean, rms);
	  (dd1->hsignal[NTDRS*jj+hh][ss])->GetXaxis()->SetRangeUser(mean-5.0*rms, mean+9.0*rms);
	  canvas->Update();
	  canvas->Modified();
	  canvas->Update();
	  canvas->Print(pdf_filename, "pdf");
	}
	delete canvas;
      }
    }
  }

  for (int jj=0; jj<NJINF; jj++){
    for (int hh = 0; hh < NTDRS; hh++) {
      for (int ss = 0; ss < 2; ss++) {
	TCanvas* canvas = new TCanvas("dummy", "dummy", 1024, 1024);
	canvas->SetLogy(true);
	dd1->hson[jj*NTDRS+hh][ss]->Draw();
	int entries = (int)(dd1->hson[NTDRS*jj+hh][ss]->GetEntries());
	if (entries>=1) {
	  double mean = (dd1->hson[NTDRS*jj+hh][ss]->GetMean());
	  double rms = (dd1->hson[NTDRS*jj+hh][ss]->GetRMS());
	  //	  printf("%f %f\n", mean, rms);
	  (dd1->hson[NTDRS*jj+hh][ss])->GetXaxis()->SetRangeUser(0.0, mean+7.0*rms);
	  canvas->Update();
	  canvas->Modified();
	  canvas->Update();
	  canvas->Print(pdf_filename, "pdf");
	}
	delete canvas;
      }
    }
  }

  TCanvas* c_exit = new TCanvas("dummy", "dummy", 1024, 1024);
  snprintf(local_pdf_filename, 255, "%s]", pdf_filename);
  c_exit->Print(local_pdf_filename, "pdf");
  delete c_exit; 

  return;
}
