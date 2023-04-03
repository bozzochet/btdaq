#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>
#include <utility>      // std::pair, std::make_pair
#include <numeric>      // std::accumulate
//----------------ROOT includes---------------------------
#include "TCanvas.h"
#include "TPad.h"
#include "TNtuple.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TLine.h"
#include "TText.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TString.h"
#include "TApplication.h"
#include "TFile.h"
#include "TGClient.h"
#include "TF1.h"
#include "TRandom.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "RQ_OBJECT.h"
#include "TGMenu.h"
#include "TGNumberEntry.h"
#include "TGResourcePool.h"
#include "TFrame.h"

bool calnohist=false;
bool calred=false;

/* for GUI START */
double y1_min=0;
double y1_max=3000;
double x1_min=0;
double x1_max=1024;
double x2_min=1200;
double x2_max=2200;
double x3_min=-100;
double x3_max=100;
double x4_min=-100;
double x4_max=100;

bool first_call = true;
std::vector<std::vector<unsigned short>> vec_of_signals; 
std::vector<unsigned short> vec_of_ped; //ped-common noise, one for each channel
std::vector<unsigned short> vec_of_noise;

//#define EV_DISP
/*   for GUI END  */

enum ETestCommandIdentifiers {
  M_TEST_NUMBERENTRY
};
  
class MyMainFrame {
  RQ_OBJECT("MyMainFrame")
  private:
  TGMainFrame         *fMain;
  TGPopupMenu        *fMenuTest;
  TRootEmbeddedCanvas *fEcanvas;
   TGVerticalFrame      *fF1;
  TGLayoutHints        *fL1;
  TGNumberEntryField* fNumericEntries;
  public:
  MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~MyMainFrame();
  void DoDraw(int nn_ev);
  void SaveCurrentRanges();
};

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {
  TGGC myGC = *gClient->GetResourcePool()->GetFrameGC();
  TGFont *myfont = gClient->GetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");

   // Create a main frame
  fMain = new TGMainFrame(p,w,h);

  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,w,h);
  fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX |
                                              kLHintsExpandY, 10,10,10,1));
  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->Divide(2,2);
  
  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40);
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  TGLabel *fLabel = new TGLabel(hframe, "Event number:", myGC(), myfont->GetFontStruct());
  hframe->AddFrame(fLabel, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  
  TGHorizontalFrame *hframe2 = new TGHorizontalFrame(fMain,200,40);
  fMain->AddFrame(hframe2, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  TGTextButton *prev = new TGTextButton(hframe2,"&Prev");
  prev->Connect("Clicked()","MyMainFrame",this,"SaveCurrentRanges()");
  prev->Connect("Clicked()","MyMainFrame",this,"DoDraw(=-1)");
  hframe2->AddFrame(prev, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  
  fNumericEntries = new TGNumberEntryField(hframe2, 15, 0, (TGNumberFormat::EStyle) 0);
  hframe2->AddFrame(fNumericEntries, new TGLayoutHints(kLHintsCenterX,
                                                      5,5,3,4));

  TGTextButton *next = new TGTextButton(hframe2,"&Next");
  next->Connect("Clicked()","MyMainFrame",this,"SaveCurrentRanges()");
  next->Connect("Clicked()","MyMainFrame",this,"DoDraw(=1)");
  hframe2->AddFrame(next, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  
  TGHorizontalFrame *hframe3 = new TGHorizontalFrame(fMain,200,40);
  fMain->AddFrame(hframe3, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
  
  TGTextButton *draw = new TGTextButton(hframe3,"&Draw");
  draw->Connect("Clicked()","MyMainFrame",this,"SaveCurrentRanges()");
  draw->Connect("Clicked()","MyMainFrame",this,"DoDraw(=0)");
  hframe3->AddFrame(draw, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));

  // Set a name to the main frame
  //  fMain->SetWindowName("Simple Example");
  
  // Map all subwindows of main frame
  fMain->MapSubwindows();

  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());
  
  // Map main frame
  fMain->MapWindow();
} 

#ifndef _BYTESWAP_H
#define _BYTESWAP_H

//#warning "byteswap.h is an unportable GNU extension! Don't use!"

static inline unsigned short bswap_16(unsigned short x) {
  return (x>>8) | (x<<8);
}

static inline unsigned int bswap_32(unsigned int x) {
  return (bswap_16(x&0xffff)<<16) | (bswap_16(x>>16));
}

static inline unsigned long long bswap_64(unsigned long long x) {
  return (((unsigned long long)bswap_32(x&0xffffffffull))<<32) |
    (bswap_32(x>>32));
}

#endif

void LinesVas(int nva=16, int nch_for_va=64);
int InitStyle();

void OpenAMSL0VladimirFile(TString filename, std::vector<std::vector<TH1F*>>& histos);
void OpenAMSL0FEPFile(TString filename, std::vector<std::vector<TH1F*>>& histos, bool kCal=true);
void OpenAMSL0FEPFile_EvDisp(TString filename, std::vector<std::vector<TH1F*>>& histos, bool kCal=true);
int openL0FEP_debug_level = 0;
//int openL0FEP_debug_level = 1;
//int openL0FEP_debug_level = 2;
//int openL0FEP_debug_level = 3;
//int openL0FEP_debug_level = 4;
//int openL0FEP_debug_level = 5;//with also the sleep

void ReOrderVladimir(std::vector<unsigned char>& data,  std::vector<unsigned short>& data_ord);
void ComputeCalibrationVladimir(std::vector<std::vector<std::vector<unsigned short>>>& signals_by_ev,
				std::vector<std::vector<std::vector<unsigned short>>>& signals,
				int nev, TString filename, std::vector<std::vector<TH1F*>>& histos);
//#define COMPCALVLAD_DEBUG
void ComputeBeamVladimir(std::vector<std::vector<std::vector<unsigned short>>>& signals_by_ev,
			 std::vector<std::vector<std::vector<unsigned short>>>& signals,
			 int nev, TString filename, std::vector<std::vector<TH1F*>>& histos);

int ReadFile(void* ptr, size_t size, size_t nitems, FILE* stream);
unsigned short int bit(int bitno, unsigned short int data);
unsigned short int bit(int bitnofirst, int bitnolast, unsigned short int data);
int ReadAMSBlockFile(unsigned int& dummy, unsigned int& ize_consumed, unsigned int& size_to_read, FILE* stream);
int ReadAMSBlockFile(unsigned short int& dummy, unsigned int& ize_consumed, unsigned int& size_to_read, FILE* stream);
int ReadAMSBlockSize(unsigned int& size_full, unsigned int& size_consumed, unsigned int& size_to_read, FILE* file);
int ReadAMSBlockRPRWNADT(bool& RP, bool& RW, unsigned short int& na, unsigned int& dt_full, unsigned int& size_consumed, unsigned int& size_to_read, FILE* file);
int ReadAMSBlockStatusTag(unsigned short int& status, unsigned short int& tag, unsigned int& size_consumed, unsigned int& size_to_read, FILE* file);
int ReadAMSBlockFineTime(unsigned int& size_consumed, unsigned int& size_to_read, FILE* file);
int ReadAMSBlockTime(unsigned int& size_consumed, unsigned int& size_to_read, FILE* file);
int ProcessAMSBlock(FILE* file, unsigned int& size_consumed, int& ev_found,
		    std::vector<std::vector<std::vector<unsigned short>>>& signals_by_ev,
		    int nesting_level=0);

void OpenGigiFile(TString filename, std::vector<std::vector<TH1F*>>& histos);
std::pair<TH1F*, TH1F*> GigiGraphToHisto(const char* name, const char* filename, bool dummy=false);

void CreateAMSFlightHistos(std::vector<std::vector<TH1F*>>& histos);

std::pair<TH1F*, TH1F*> VectorToHisto(std::vector<double> values, const char* name, const char* filename, int nch=640, int ladder=-999, int nspread=4096);
std::vector<double> HistoToVector(TH1F* histo);

void SummaryCal(std::vector<std::vector<TH1F*>> histos, const char* filename, const char* nameout, int type=0);
void SummaryBeam(std::vector<std::vector<TH1F*>> histos, const char* filename, const char* nameout, int type=0);
void FillComparison(std::vector<std::vector<TH1F*>>* comparison,
		    std::vector<std::vector<TH1F*>> histos);
void Comparison(std::vector<std::vector<TH1F*>> histos[3], const char* nameout);

TString Path2Name(const char *name, const char *sep, const char *exten);

//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------
void EventDisplay(TString filename){

  InitStyle();

  gROOT->SetStyle("StyleWhite");
  //  gStyle->SetPaperSize(27,20);
  
  std::vector<std::vector<TH1F*>> histos;
  OpenAMSL0FEPFile(filename, histos);
   
  return;
}
void plotta_beam(){

  InitStyle();
  
  gROOT->SetStyle("StyleWhite");
  //  gStyle->SetPaperSize(27,20);

  char nameout[255];
  snprintf(nameout, sizeof(nameout), "Beam.pdf");
  
  // TString amsl0fepcalfile = "./Data/L0/BLOCKS/USBL0_PG_LEFV2BEAM1/0005/519";
  // TString amsl0fepcalfile = "./Data/L0/BLOCKS/USBL0_PG_LEFV2BEAM1/0000/235";
  //  TString amsl0fepcalfile = "/media/gsilvest/gigi/USBL0_PG_LSR00/0000/002";
  TString amsl0fepcalfile = "./Data/L0/BLOCKS/PG/USBLF_PG_TRENTO2023/0000/031";//USB-LF with 2 LEFs, CAL
  
  //  TString amsl0fepbeamfile = "./Data_hacked/L0/BLOCKS/USBL0_PG_LEFV2BEAM1/0005/525";
  //  TString amsl0fepbeamfile = "./Data_hacked/L0/BLOCKS/USBL0_PG_LEFV2BEAM1/0000/238_244";
  //  TString amsl0fepbeamfile = "./Data_hacked/L0/BLOCKS/USBL0_PG_LEFV2BEAM1/0000/238_244";
  //  TString amsl0fepbeamfile = "/media/gsilvest/gigi/USBL0_PG_LSR00/0000/014";
  TString amsl0fepbeamfile = "./Data/L0/BLOCKS/PG/USBLF_PG_TRENTO2023/0000/031";//USB-LF with 2 LEFs, CAL

  std::vector<std::vector<TH1F*>> histos_cal;
  OpenAMSL0FEPFile(amsl0fepcalfile, histos_cal, true);

  std::vector<std::vector<TH1F*>> histos_beam = histos_cal;//I have to pass the cal (will be overwritten) to the below function
  OpenAMSL0FEPFile(amsl0fepbeamfile, histos_beam, false);

  char nameouttemp[255];
  snprintf(nameouttemp, sizeof(nameouttemp), "%s", nameout);
  SummaryBeam(histos_beam, amsl0fepbeamfile, nameouttemp, 1);
  
  return;  
}

void FillComparison(std::vector<std::vector<TH1F*>>* comparison,
		    std::vector<std::vector<TH1F*>> histos) {
  
#define COMPARISON_BETWEEN_LADDERS_OF_SAME_FILE
  
  for (int ll=0; ll<((int)histos.size()); ll++) {
    if (histos[ll].size()>0) {
      for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmaraws
#ifdef COMPARISON_BETWEEN_LADDERS_OF_SAME_FILE
	comparison[jj].resize(1);
	comparison[jj][0].push_back(histos[ll][4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma
#else
	comparison[jj].resize(histos.size());
	comparison[jj][ll].push_back(histos[ll][4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
#endif
      }
    }
  }

  return;
}

void plotta_cal(){
  
  bool amsflight=false;
  bool pox=false;
  bool amsl0vlad=false;
  bool amsl0fep=true;
  
  bool nocomparison=false;
  
  InitStyle();
  
  gROOT->SetStyle("StyleWhite");
  //  gStyle->SetPaperSize(27,20);
  
  char nameout[255];
  snprintf(nameout, sizeof(nameout), "Calibrations.pdf");
  
  const int npoxfiles = 4;
  TString poxfiles[npoxfiles] = {"POX02.root", "POX12_newVA.root", "POX12_VAonly.root", "POX12_completo_50Vbias.root"};
  
  /*
    const int namsl0vladfiles = 3;
    TString amsl0vladfiles[namsl0vladfiles] = {"LEFP01_noVA.bin", "LEFP03_2.bin", "LEFP03_3.bin"};
  */
  /*
    const int namsl0vladfiles = 1;
    TString amsl0vladfiles[namsl0vladfiles] = {"LEFP03_17.bin"};
  */
  const int namsl0vladfiles = 5;
  TString amsl0vladfiles[namsl0vladfiles] = {"LEFP03_18.bin", "LEFP03_19.bin", "LEFP03_20.bin", "LEFP03_21.bin", "LEFP03_22.bin"};
  
  const int namsl0fepfiles = 1;
  //  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBL0_PG_LEFV2BEAM1/0005/519"};
  //  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBL0_PG_LEFV2BEAM1/0000/626"};
  //  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBL0_PG_SIPMTRG/0003/917"};
  //  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBL0_PG_BLatPS/0008/090"};//USB-LEF
  //  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBL0_PG_MUONS23/0007/119"}; //USB-LEF
  //  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBLF_PG_TRENTO2023/0000/019"};//USB-LF with XX LEFs... Not sure...
  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBLF_PG_TRENTO2023/0000/031"};//USB-LF with 2 LEFs, CAL
  //  TString amsl0fepfiles[namsl0vladfiles] = {"./Data/L0/BLOCKS/PG/USBLF_PG_TRENTO2023/0000/221"};//USB-LF with 1 LEF, no silicon, lab in PG

  std::vector<std::vector<TH1F*>> comparison[3];//3 since there're 3 comparisons: pedestal, sigma and sigmawas
  
  {
    std::vector<std::vector<TH1F*>> histos(1);
    char nameouttemp[255];
    snprintf(nameouttemp, sizeof(nameouttemp), "%s[", nameout);
    SummaryCal(histos, "", nameouttemp);
  }
  
  if (amsflight) {
    std::vector<std::vector<TH1F*>> histos(1);//(1) since only one ladder
    CreateAMSFlightHistos(histos);
    char nameouttemp[255];
    snprintf(nameouttemp, sizeof(nameouttemp), "%s", nameout);
    SummaryCal(histos, "AMS-flight", nameouttemp);
    for (int ll=0; ll<((int)histos.size()); ll++) {
      if (histos[ll].size()>0) {
	for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmawas
	  comparison[jj].resize(histos.size());
	  comparison[jj][ll].push_back(histos[0][4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
	}
      }
    }
  }
  
  if (pox) {
    for (int ii=0; ii<npoxfiles; ii++) {
      std::vector<std::vector<TH1F*>> histos(1);//(1) since only one ladder
      OpenGigiFile(poxfiles[ii], histos);
      char nameouttemp[255];
      snprintf(nameouttemp, sizeof(nameouttemp), "%s", nameout);
      SummaryCal(histos, poxfiles[ii], nameouttemp);
      if (poxfiles[ii]=="POX12_completo_50Vbias.root") {
	for (int ll=0; ll<((int)histos.size()); ll++) {
	  if (histos[ll].size()>0) {
	    for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmawas
	      comparison[jj].resize(histos.size());
	      comparison[jj][ll].push_back(histos[0][4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
	    }
	  }
	}
      }
    }
  }

  if (amsl0vlad) {
    for (int ii=0; ii<namsl0vladfiles; ii++) {
      std::vector<std::vector<TH1F*>> histos;
      OpenAMSL0VladimirFile(amsl0vladfiles[ii], histos);
      char nameouttemp[255];
      snprintf(nameouttemp, sizeof(nameouttemp), "%s", nameout);
      SummaryCal(histos, amsl0vladfiles[ii], nameouttemp, 1);
      if (amsl0vladfiles[ii].Contains("LEFP03_")) {
	for (int ll=0; ll<((int)histos.size()); ll++) {
	  if (histos[ll].size()>0) {
	    for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmawas
	      comparison[jj].resize(histos.size());
	      comparison[jj][ll].push_back(histos[ll][4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
	    }
	  }
	}
      }
    }
  }

  if (amsl0fep) {
    for (int ii=0; ii<namsl0fepfiles; ii++) {
      std::vector<std::vector<TH1F*>> histos;
      OpenAMSL0FEPFile(amsl0fepfiles[ii], histos);
      char nameouttemp[255];
      snprintf(nameouttemp, sizeof(nameouttemp), "%s", nameout);
      SummaryCal(histos, amsl0fepfiles[ii], nameouttemp, 1);
      //      if (amsl0fepfiles[ii].Contains("LEFP03_")) {
      FillComparison(comparison, histos);	
      //      }
    }
  }
  
  if (!nocomparison){
    char nameouttemp[255];
    snprintf(nameouttemp, sizeof(nameouttemp), "%s)", nameout);
    Comparison(comparison, nameouttemp);
  }
  else {
    std::vector<std::vector<TH1F*>> histos;
    char nameouttemp[255];
    snprintf(nameouttemp, sizeof(nameouttemp), "%s]", nameout);
    SummaryCal(histos, "", nameouttemp);
  }
      
  return;
}

void OpenAMSL0VladimirFile(TString filename, std::vector<std::vector<TH1F*>>& histos){

  FILE* rawfile = fopen(filename.Data(), "r");
  if (rawfile == NULL) {
    printf("Error file %s not found\n", filename.Data());
    exit(2);
  }
  else {
    printf("File %s opened\n", filename.Data());
  }

  int ret=0;

  histos.resize(1);//since is a single ladder
  std::vector<std::vector<std::vector<unsigned short>>> signals_by_ev(1);//(1) since is a single ladder
  std::vector<std::vector<std::vector<unsigned short>>> signals(1);//(1) since is a single ladder
  
  int nev=0;
  while (ret>=0) {
    std::vector<unsigned char> data;
    
    unsigned short size=0;
    ret = ReadFile(&size, sizeof(size), 1, rawfile);
    if (ret<=0) break;
    //    printf("%d) %04hx (%d)\n", ret, size, size);
    if (size<=2) break;
    size = size-2;//ha contato la size stessa nella size
    data.resize(2*size);
    //    printf("data size (bin): %d\n", 2*size);
    
    unsigned short dt=0;
    ret = ReadFile(&dt, sizeof(dt), 1, rawfile);
    if (dt != 0x8013) {
      printf("Wrong DT: %04hx\n", dt);
      break;
    }
    //    printf("%d) %04hx\n", ret, dt);

    unsigned short evtn=0;
    ret = ReadFile(&evtn, sizeof(evtn), 1, rawfile);
    int evtn_int = evtn>>8;
    //    printf("%d) %04hx (%d)\n", ret, evtn, evtn_int);
    
    ret = ReadFile(&data[0], size*sizeof(short), 1, rawfile);
    /*
    for (int ii=0; ii<((int)data.size()); ii++) {
      printf("0x%02x\n", data[ii]);
    }
    */

    std::vector<unsigned short> data_ord;
    ReOrderVladimir(data, data_ord);
    /*
    for (int ii=0; ii<((int)data_ord.size()); ii++) {
      printf("%d\n", data_ord[ii]);
    }
    */
    signals_by_ev[0].push_back(data_ord);
    
    nev++;
    //    printf("We read %d events\n", nev);
  }
  printf("We read %d events\n", nev);

  if (rawfile)
    fclose(rawfile);
  rawfile = NULL;
  
  ComputeCalibrationVladimir(signals_by_ev, signals, nev, filename, histos);
  
  return;
}

void ComputeBeamVladimir(std::vector<std::vector<std::vector<unsigned short>>>& signals_by_ev,
			 std::vector<std::vector<std::vector<unsigned short>>>& signals,
			 int nev, TString filename, std::vector<std::vector<TH1F*>>& histos){

  //we expect the caller to have equally sized the input (signals_by_ev) and the output (signals and histos), but to be sure...
  if ((histos.size() != signals_by_ev.size()) || (signals.size() != signals_by_ev.size())) {
    printf("We have different sizes for the input (signals_by_ev=%d) and the output (signals=%d, histos=%d)\n",
	   ((int)(histos.size())),
	   ((int)(signals_by_ev.size())),
	   ((int)(signals_by_ev.size())));
    histos.resize(signals_by_ev.size());
    signals.resize(signals_by_ev.size());
  }
  
  //loop on ladders
  for (int ll=0; ll<((int)signals_by_ev.size()); ll++) {

    if (((int)(signals_by_ev[ll].size()))<=0) //this ladders has zero events
      continue;
    
    int nch = ((int)(signals_by_ev[ll][0].size()));
    // printf("nch = %d\n", nch);
    // printf("nev = %d\n", nev);
    signals[ll].resize(nch);
    for (int ch = 0; ch < ((int)(signals[ll].size())); ch++) {
      signals[ll][ch].resize(nev);
      for (int ev = 0; ev < ((int)(signals[ll][ch].size())); ev++) {
	signals[ll][ch][ev] = signals_by_ev[ll][ev][ch];
      }
    }

  
    std::vector<double> values[4];
    for (int ii=0; ii<4; ii++) {
      values[ii] = HistoToVector(histos[ll][ii]);
    }

    TH1F* histo_example = histos[ll][0];
  
    histos[ll].clear();
    // clone, clear and rename histos
    histos[ll].push_back((TH1F*)histo_example->Clone());
    histos[ll][0]->Reset();
    histos[ll][0]->SetName(Form("%s_%s_%d", "occupancy", filename.Data(), ll));
    histos[ll][0]->SetTitle(Form("%s_%s_%d", "occupancy", filename.Data(), ll));
    histos[ll].push_back((TH1F*)histo_example->Clone());
    histos[ll][1]->Reset();
    histos[ll][1]->SetName(Form("%s_%s_%d", "weighted_occupancy", filename.Data(), ll));
    histos[ll][1]->SetTitle(Form("%s_%s_%d", "weighted_occupancy", filename.Data(), ll));
    {
      TH1F* h = new TH1F(Form("%s_%s_%d", "signal", filename.Data(), ll), Form("%s_%s_%d", "signal", filename.Data(), ll), 4096, 0, 4096);
      histos[ll].push_back(h);
    }
    {
      TH1F* h = new TH1F(Form("%s_%s_%d", "signal_abovethresh", filename.Data(), ll), Form("%s_%s_%d", "signal", filename.Data(), ll), 4096, 0, 4096);
      histos[ll].push_back(h);
    }
    {
      TH1F* h = new TH1F(Form("%s_%s_%d", "signal_to_noise", filename.Data(), ll), Form("%s_%s_%d", "signal", filename.Data(), ll), 1000, 0, 100);
      histos[ll].push_back(h);
    }
  
    /*
      for (int ii=0; ii<4; ii++) {
      for (int ch=0; ch<((int)(values[ii].size())); ch++){
      printf("values[%d][%d] = %f\n", ii, ch, values[ii][ch]);
      }
      }
    */
  
    {
      unsigned int NVAS = 16;
      unsigned int NCHAVA = 64;     
      unsigned int lastVA = std::numeric_limits<unsigned int>::max();
      std::vector<float> common_noise(NVAS);
      for (unsigned int iEv = 0; iEv < signals_by_ev[ll].size(); ++iEv) {
	for (unsigned int iCh = 0; iCh < (NVAS * NCHAVA); ++iCh) {
	  unsigned int thisVA = iCh / NCHAVA;
	  if (thisVA != lastVA) {
	  
	    std::vector<float> sig_mean_sub;
	    for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
	      double sig = signals[ll][thisVA * NCHAVA + iVACh][iEv] - values[0][thisVA * NCHAVA + iVACh];
	      double noise = values[1][thisVA * NCHAVA + iVACh];
	      double sig_to_noise = sig/noise;
	      if (fabs(sig_to_noise)<3.5) {
		sig_mean_sub.push_back(sig);
		//	      printf("%f\n", sig_mean_sub[iVACh]);
	      }
	    }

	    if (((int)(sig_mean_sub.size()))>0) {
	      if (((int)(sig_mean_sub.size()))>16) {
		// get the median
		std::sort(begin(sig_mean_sub), end(sig_mean_sub));
		//	  common_noise[thisVA] = 0.5 * (sig_mean_sub[(NCHAVA / 2) - 1] + sig_mean_sub[NCHAVA / 2]);
		common_noise[thisVA] = 0.5 * (sig_mean_sub[(((int)(sig_mean_sub.size())) / 2) - 1] + sig_mean_sub[((int)(sig_mean_sub.size())) / 2]);
	      }
	      else {
		printf("event=%d) the common noise vector (VA=%d) has less than 16 entries (%d)...\n", iEv, thisVA, ((int)(sig_mean_sub.size())));
		common_noise[thisVA] = 0.0;
	      }
	    }
	    else {
	      printf("event=%d) The common noise vector (VA=%d) is empty...\n", iEv, thisVA);
	      common_noise[thisVA] = 0.0;
	    }
	  }

	  double sig = (signals[ll][iCh][iEv] - values[0][iCh] - common_noise[thisVA]);
	  double noise = values[1][iCh];
	  double sig_to_noise = sig/noise;
          if (sig_to_noise>0) {
	    if (sig_to_noise>3.5) {
	      histos[ll][0]->Fill(iCh);
	      histos[ll][3]->Fill(sig);
	    }
	    histos[ll][1]->Fill(iCh, sig);
	  }
	  histos[ll][2]->Fill(sig);
	  histos[ll][4]->Fill(sig_to_noise);
#ifdef EV_DISP
	  if (iEv==0){
	    for (unsigned int iCh = 0; iCh < (NVAS * NCHAVA); ++iCh) {
	      unsigned int thisVA = iCh / NCHAVA;
	      if (thisVA != lastVA) {
		vec_of_ped.push_back(values[0][iCh]);
		vec_of_noise.push_back(values[1][iCh]);
	      }
	    }
	  }
#endif
	  lastVA = thisVA;
	}
#ifdef EV_DISP
	vec_of_signals.push_back(signals_by_ev[ll][iEv]);
#endif
      }
    }
  }

  return;
}

void ComputeCalibrationVladimir(std::vector<std::vector<std::vector<unsigned short>>>& signals_by_ev,
				std::vector<std::vector<std::vector<unsigned short>>>& signals,
				int nev, TString filename, std::vector<std::vector<TH1F*>>& histos){

#ifdef COMPCALVLAD_DEBUG
  TFile* fdebug = new TFile(Form("fdebug_%s.root", Path2Name(filename.Data(), "/", "").Data()), "RECREATE");
  fdebug->cd();
#endif

  //we expect the caller to have equally sized the input (signals_by_ev) and the output (signals and histos), but to be sure...
  if ((histos.size() != signals_by_ev.size()) || (signals.size() != signals_by_ev.size())) {
    printf("We have different sizes for the input (signals_by_ev=%d) and the output (signals=%d, histos=%d)\n",
	   ((int)(histos.size())),
	   ((int)(signals_by_ev.size())),
	   ((int)(signals_by_ev.size())));
    histos.resize(signals_by_ev.size());
    signals.resize(signals_by_ev.size());
  }
  
  //loop on ladders
  for (int ll=0; ll<((int)signals_by_ev.size()); ll++) {
    
    if (((int)(signals_by_ev[ll].size()))<=0) //this ladders has zero events
      continue;

    int nch = ((int)(signals_by_ev[ll][0].size()));
    signals[ll].resize(nch);
    for (int ch = 0; ch < ((int)(signals[ll].size())); ch++) {
      signals[ll][ch].resize(nev);
      for (int ev = 0; ev < ((int)(signals[ll][ch].size())); ev++) {
	signals[ll][ch][ev] = signals_by_ev[ll][ev][ch];
      }
    }

    std::vector<double> values[4];
    for (int ii=0; ii<4; ii++) {
      values[ii].resize(signals[ll].size());
    }

#ifdef COMPCALVLAD_DEBUG
    TH1F** hsig = NULL;
    hsig = new TH1F*[((int)(signals[ll].size()))];
    for (int ch = 0; ch < ((int)(signals[ll].size())); ch++) {
      hsig[ch] = new TH1F(Form("hsig_%d_%d", ch, ll), Form("hsig_%d_%d", ch, ll), 4096, 0, 4096);
    }
#endif
    
    for (int ch = 0; ch < ((int)(signals[ll].size())); ch++) {
      
#ifdef COMPCALVLAD_DEBUG
      for (int ev=0; ev<((int)(signals[ll][ch].size())); ev++) {
	hsig[ch]->Fill(signals[ll][ch].at(ev));
      }
#endif
      
      values[0][ch] = std::accumulate(begin(signals[ll][ch]), end(signals[ll][ch]), 0.0) /
	static_cast<float>(signals[ll][ch].size());
      //    printf("ped[%d]) %f\n", ch, values[0][ch]);
    
      values[2][ch] =
	std::sqrt(std::accumulate(begin(signals[ll][ch]), end(signals[ll][ch]), 0.0,
				  [&](float acc, float curr) {
				    return acc + (curr - values[0][ch]) * (curr - values[0][ch]);
				  }) /
		  // this is the `real` RMS. Not ok
		  //      std::sqrt((std::inner_product(signals[ch].begin(), signals[ch].end(), signals[ch].begin(), 0.0f) - values[0][ch]*values[0][ch]) /
		  static_cast<float>(signals[ll][ch].size()));
      //    printf("rsig[%d]) %f\n", ch, values[2][ch]);
    
      // initialize this for later
      values[1][ch] = 0;
      values[3][ch] = 0;
    }

    {
      unsigned int NVAS = 16;
      unsigned int NCHAVA = 64;     
      unsigned int lastVA = std::numeric_limits<unsigned int>::max();
      std::vector<float> common_noise(NVAS);
      std::vector<unsigned int> processed_events(NVAS * NCHAVA);
      for (unsigned int iEv = 0; iEv < signals_by_ev[ll].size(); ++iEv) {
	for (unsigned int iCh = 0; iCh < (NVAS * NCHAVA); ++iCh) {
	  unsigned int thisVA = iCh / NCHAVA;

	  //	  printf("this = %d, last = %d\n", thisVA, lastVA);
	  if (thisVA != lastVA) {

	    std::vector<float> sig_mean_sub;
	    for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
	      double sig = signals[ll][thisVA * NCHAVA + iVACh][iEv] - values[0][thisVA * NCHAVA + iVACh];
	      double rawnoise = values[2][thisVA * NCHAVA + iVACh];
	      double sig_to_rawnoise = sig/rawnoise;
	      if (fabs(sig_to_rawnoise)<3) {
		sig_mean_sub.push_back(sig);
	      }
	      /* else { */
	      /*   printf("%d) %f\n", thisVA * NCHAVA + iVACh, sig_to_rawnoise); */
	      /* } */
	    }
	  
	    // get the median
	    std::sort(begin(sig_mean_sub), end(sig_mean_sub));
	    //	  common_noise[thisVA] = 0.5 * (sig_mean_sub[(NCHAVA / 2) - 1] + sig_mean_sub[NCHAVA / 2]);
	    if (sig_mean_sub.size()>16) {
	      common_noise[thisVA] = 0.5 * (sig_mean_sub[(((int)(sig_mean_sub.size())) / 2) - 1] + sig_mean_sub[((int)(sig_mean_sub.size())) / 2]);
	    }
	    else {
	      common_noise[thisVA] = 0.0;
	    }
	    //	    printf("VA=%d) CN for sigma=%f (%lu/%d)\n", thisVA, common_noise[thisVA], sig_mean_sub.size(), NCHAVA);
	  }
	
	  if (std::fabs(common_noise[thisVA]) > 10) {//not used for the sigma evaluation
	    continue;
	  }

	  ++processed_events[iCh];
	
	  values[1][iCh] += (signals[ll][iCh][iEv] - values[0][iCh] - common_noise[thisVA]) *
	    (signals[ll][iCh][iEv] - values[0][iCh] - common_noise[thisVA]);
	
	  lastVA = thisVA;
	}
      }
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
	values[1][iCh] = std::sqrt(values[1][iCh] / static_cast<float>(processed_events[iCh]));
	//	printf("sig[%d]) %f\n", iCh, values[1][iCh]);
      }

      for (unsigned int iEv = 0; iEv < signals[ll][0].size(); ++iEv) {
	for (unsigned int iCh = 0; iCh < (NVAS * NCHAVA); ++iCh) {
	  unsigned int thisVA = iCh / NCHAVA;

	  if (thisVA != lastVA) {
	  
	    std::vector<float> sig_mean_sub;
	    for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
	      double sig = signals[ll][thisVA * NCHAVA + iVACh][iEv] - values[0][thisVA * NCHAVA + iVACh];
	      double noise = values[1][thisVA * NCHAVA + iVACh];
	      double sig_to_noise = sig/noise;
	      if (fabs(sig_to_noise)<3.5) {
		sig_mean_sub.push_back(sig);
		//	      printf("%f\n", sig_mean_sub[iVACh]);
	      }
	    }

	    // get the median
	    std::sort(begin(sig_mean_sub), end(sig_mean_sub));
	    //	  common_noise[thisVA] = 0.5 * (sig_mean_sub[(NCHAVA / 2) - 1] + sig_mean_sub[NCHAVA / 2]);
	    if (sig_mean_sub.size()>16) {
	      common_noise[thisVA] = 0.5 * (sig_mean_sub[(((int)(sig_mean_sub.size())) / 2) - 1] + sig_mean_sub[((int)(sig_mean_sub.size())) / 2]);
	    }
	    else {
	      common_noise[thisVA] = 0.0;
	    }
	    //	  if (iEv==0) printf("VA=%d) CN for average=%f (%lu/%d)\n", thisVA, common_noise[thisVA], sig_mean_sub.size(), NCHAVA);
	    /* if (fabs(common_noise[thisVA])>100.0){ */
	    /*   for (int ii=0; ii<((int)(sig_mean_sub.size())); ii++) { */
	    /*     printf("%d) %f (%f)\n", ii, sig_mean_sub[ii], common_noise[thisVA]); */
	    /*   } */
	    /* } */
	  }

	  double sig = (signals[ll][iCh][iEv] - values[0][iCh] - common_noise[thisVA]);
	  double noise = values[1][iCh];
	  double sig_to_noise = sig/noise;
	  if (sig_to_noise>3.5) {
	    values[3][iCh] += sig;
	  }
	  /* if (iCh==959 && fabs(sig_to_noise)>100.0) */
	  /*   printf("%d %d -> %f (%f) [%hu %f %f %f]\n", iEv, iCh, sig_to_noise, values[3][iCh], signals[ll][iCh][iEv], values[0][iCh], common_noise[thisVA], values[1][iCh]); */
		
#ifdef EV_DISP
	  if (iEv==0){
	    for (unsigned int iCh = 0; iCh < (NVAS * NCHAVA); ++iCh) {
	      unsigned int thisVA = iCh / NCHAVA;
	      if (thisVA != lastVA) {
		vec_of_ped.push_back(values[0][iCh] - common_noise[thisVA]);
		vec_of_noise.push_back(values[1][iCh]);
	      }
	    }
	  }
#endif
	  lastVA = thisVA;
	}
#ifdef EV_DISP
	vec_of_signals.push_back(signals_by_ev[ll][iEv]);
#endif
      }
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
	values[3][iCh] = (values[3][iCh] / static_cast<float>(signals[ll][0].size()));
	//      printf("ave_sig[%d]) %f\n", iCh, values[3][iCh]);	    
      }
    }
    /*
      {
      unsigned int NVAS = 16;
      unsigned int NCHAVA = 64;     
      for (unsigned int ch = 0; ch < NVAS * NCHAVA; ch++) {
      printf("%04d)", ch);
      printf("\t ped=%f", values[0][ch]);
      printf("\t rsig=%f", values[2][ch]);
      printf("\t sig=%f", values[1][ch]);
      printf("\t ave_sig=%f\n", values[3][ch]);
      }
      }
    */

    const char* names[4] = {"Pedestals", "Sigma", "RawSigma", "AverageSignal"};

    std::vector<TH1F*> histos2;

    for (int ii=0; ii<4; ii++) {
      
      std::pair<TH1F*, TH1F*> coppia = VectorToHisto(values[ii], names[ii], filename.Data(), nch, ll);
      TH1F* h = coppia.first;
      TH1F* h2 = coppia.second;
      //      printf("%s %s %p %p\n", "AMS-L0", names[ii], h, h2);
      /*
	h->Dump();
	for (int bb=0; bb<=h->GetNbinsX()+1; bb++){
	printf("%d) %f\n", bb, h->GetBinContent(bb));
	}
      */
      histos[ll].push_back(h);
      histos2.push_back(h2);
    }
    
    for (int ii=0; ii<3; ii++) {//not including the spread on the average signal
      histos[ll].push_back(histos2.at(ii));
    }
    
  }
    
#ifdef COMPCALVLAD_DEBUG
  fdebug->Write();
  fdebug->Close();
  // for (int ch = 0; ch < ((int)(signals.size())); ch++) {
  //   if (hsig[ch]) delete hsig[ch]; 
  // }
  // delete[] hsig;
#endif
  
  return;
}

void ReOrderVladimir(std::vector<unsigned char>& data,  std::vector<unsigned short>& data_ord){

  std::vector<unsigned short> data_ord_tmp;
  data_ord_tmp.resize(((int)((8.0/14.0)*((int)(data.size())))));
  
  data_ord.resize(((int)((8.0/14.0)*((int)(data.size())))));
  //  printf("**** data.size=%d, nch = %d, data_ord.size=%d\n", ((int)(data.size())), ((int)((8.0/14.0)*((int)(data.size())))), ((int)(data_ord.size())));
  
  int ch_in_2va = -1;
  
  for (int byte=0; byte<((int)data.size()); byte+=14) {
    ch_in_2va++;
    for (int bit_nec=0; bit_nec<14; bit_nec++) {
      //      int bit = bit_nec%2?(bit_nec-1):(bit_nec+1); //endianess correction, not needed
      int bit = bit_nec;
      //      printf("byte %dth) 0x%02hx\n", byte+bit_nec+1, data[byte+bit_nec]);
      for (int adc=0; adc<8; adc++){
	int ch = adc*128+ch_in_2va;
	data_ord_tmp[ch] |= ((data[byte+bit_nec] & (1<<adc))>>adc)<<bit;
	//	printf("%d %d %d) 0x%02hx\n", adc, ch_in_2va, ch, data_ord[ch]);
      }
    }
  }

  for (int ch=0; ch<((int)data_ord_tmp.size()); ch++) {
    int va = ch/64;
    int ch_in_va = ch%64;
    //    printf("%d -> (%d,%d)\n", ch, va, ch_in_va);
    //    int new_ch = (16-va-1)*64 + (64-ch_in_va-1);//MD: I undersootd like this, but VK did differently (31 Mar 2022)
    int new_ch = (16-va-1)*64 + ch_in_va;
    //    printf("%d --> %d\n", ch, new_ch);
    data_ord[new_ch] = data_ord_tmp[ch];
  }
  
  /*
    for (int ch=0; ch<((int)data_ord.size()); ch++) {
    printf("%d) %02hx (%d)\n", ch, data_ord[ch], data_ord[ch]);
    }
  */
    
  return;
}

void OpenGigiFile(TString filename, std::vector<std::vector<TH1F*>>& histos){

  std::vector<TH1F*> histos2;
  
  const char* names[4] = {"Pedestals", "Sigma", "RawSigma", "AverageSignal"};
  
  for (int ii=0; ii<3; ii++) {
    std::pair<TH1F*, TH1F*> coppia = GigiGraphToHisto(names[ii], filename.Data());
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    //    printf("%s %s %p %p\n", filename.Data(), names[ii], h, h2);
    histos[0].push_back(h);
    histos2.push_back(h2);
  }
  {//dummy
    int ii=3;
    std::pair<TH1F*, TH1F*> coppia = GigiGraphToHisto(names[ii], filename.Data(), true);
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    //    printf("%s %s %p %p\n", filename.Data(), names[ii], h, h2);
    histos[0].push_back(h);
    histos2.push_back(h2);
  }
  
  for (int ii=0; ii<3; ii++) {
    histos[0].push_back(histos2.at(ii));
  }
  
  return;
}

std::pair<TH1F*, TH1F*> GigiGraphToHisto(const char* name, const char* filename, bool dummy){

  TH1F* h = new TH1F(Form("%s_%s", name, filename), Form("%s_%s", name, filename), 640, 0, 640);
  TH1F* h2 = new TH1F(Form("%s_spread_%s", name, filename), Form("%s_spread_%s", name, filename), 409600, 0, 4096);

  if (!dummy) {
    TFile* f = new TFile(filename);
    TGraph* graph = (TGraph*)(f->Get(name));
    //  printf("%p\n", graph);
    //  graph->Dump();
    
    auto nPoints = graph->GetN();
    for(int i=0; i < nPoints; ++i) {
      double x,y;
      graph->GetPoint(i, x, y);
      h->Fill(x, y);
      h2->Fill(y);
    }
    
    f->Close();
  }

  std::pair<TH1F*, TH1F*> coppia = std::make_pair(h, h2);
  
  return coppia;
}

void CreateAMSFlightHistos(std::vector<std::vector<TH1F*>>& histos){
  
  // HwID 010, TkID +109: JLayer 2, Slot 9
  // extracted from CalDB_1646913042.root
  
  double pedestals[640] = {584.625000, 527.875000, 549.875000, 531.500000, 585.125000, 543.250000, 572.875000, 497.875000, 566.250000, 555.375000, 565.875000, 601.500000, 557.875000, 611.000000, 569.250000, 610.500000, 658.500000, 571.500000, 681.750000, 650.500000, 640.250000, 602.125000, 606.875000, 700.500000, 668.625000, 684.125000, 620.000000, 621.000000, 693.125000, 669.875000, 570.000000, 611.125000, 630.875000, 638.375000, 628.875000, 617.125000, 617.500000, 611.875000, 554.625000, 581.125000, 601.250000, 562.875000, 569.625000, 599.125000, 572.125000, 572.250000, 548.125000, 608.625000, 642.750000, 554.375000, 616.250000, 546.125000, 596.250000, 649.625000, 550.875000, 621.750000, 566.250000, 650.625000, 561.625000, 603.375000, 608.375000, 593.375000, 570.875000, 531.375000, 356.875000, 315.500000, 346.000000, 347.000000, 384.875000, 337.250000, 338.625000, 319.000000, 384.625000, 426.625000, 370.750000, 407.250000, 406.375000, 370.000000, 401.250000, 458.750000, 481.875000, 392.625000, 434.000000, 459.500000, 410.875000, 433.750000, 439.125000, 452.750000, 457.500000, 541.875000, 449.250000, 515.375000, 477.125000, 486.375000, 535.125000, 516.375000, 597.625000, 525.750000, 601.375000, 582.125000, 603.500000, 614.750000, 578.625000, 620.500000, 647.125000, 602.625000, 613.625000, 579.125000, 617.750000, 502.000000, 608.875000, 594.000000, 587.000000, 527.500000, 536.750000, 544.125000, 581.750000, 560.000000, 599.875000, 541.500000, 555.125000, 485.250000, 544.250000, 570.000000, 554.750000, 550.500000, 572.750000, 576.875000, 482.500000, 418.375000, 435.625000, 444.250000, 431.125000, 395.500000, 459.500000, 445.625000, 439.250000, 476.500000, 421.375000, 445.750000, 377.125000, 460.375000, 456.625000, 484.250000, 459.375000, 404.875000, 422.125000, 458.125000, 468.000000, 495.000000, 485.750000, 508.125000, 534.375000, 529.625000, 483.250000, 495.875000, 521.875000, 459.250000, 555.625000, 476.250000, 548.500000, 542.250000, 503.500000, 500.625000, 473.000000, 422.375000, 539.250000, 515.625000, 551.125000, 477.000000, 490.875000, 519.375000, 511.375000, 544.625000, 516.000000, 486.750000, 522.000000, 486.125000, 479.375000, 498.875000, 491.250000, 495.625000, 495.375000, 476.625000, 476.875000, 457.625000, 429.375000, 448.750000, 523.000000, 470.750000, 452.000000, 476.000000, 385.000000, 353.500000, 391.750000, 378.000000, 318.750000, 413.875000, 356.750000, 412.125000, 451.000000, 401.375000, 414.625000, 390.875000, 402.375000, 431.375000, 415.125000, 498.750000, 452.875000, 342.750000, 443.500000, 370.750000, 437.000000, 453.125000, 499.875000, 449.625000, 434.875000, 430.000000, 484.375000, 420.000000, 434.750000, 457.250000, 456.500000, 533.750000, 508.875000, 477.500000, 477.750000, 415.250000, 494.750000, 471.250000, 520.625000, 499.875000, 548.625000, 467.875000, 510.625000, 489.000000, 537.625000, 452.000000, 505.125000, 524.875000, 632.000000, 541.125000, 471.750000, 542.125000, 511.875000, 552.875000, 532.750000, 581.250000, 572.875000, 552.375000, 510.250000, 538.625000, 484.875000, 426.000000, 496.375000, 535.500000, 370.500000, 348.250000, 344.375000, 346.250000, 353.875000, 317.875000, 334.500000, 347.000000, 320.875000, 325.000000, 351.000000, 319.750000, 363.500000, 377.250000, 340.875000, 376.625000, 404.875000, 356.125000, 417.750000, 385.000000, 408.625000, 357.375000, 385.375000, 416.250000, 423.500000, 412.250000, 447.125000, 389.500000, 353.750000, 350.125000, 426.875000, 405.500000, 452.625000, 398.875000, 409.500000, 425.250000, 405.125000, 481.500000, 455.750000, 475.500000, 462.000000, 515.250000, 475.875000, 515.875000, 506.250000, 472.875000, 472.125000, 470.000000, 535.875000, 535.375000, 513.000000, 498.875000, 486.750000, 512.875000, 486.125000, 527.500000, 482.875000, 539.000000, 545.375000, 492.875000, 507.875000, 453.000000, 475.375000, 525.000000, 367.375000, 368.500000, 313.375000, 338.625000, 331.875000, 307.375000, 330.125000, 368.750000, 337.500000, 380.000000, 346.000000, 413.750000, 439.125000, 425.500000, 427.750000, 451.250000, 467.875000, 394.125000, 426.000000, 437.250000, 339.125000, 406.500000, 435.000000, 465.750000, 505.625000, 510.250000, 432.125000, 447.125000, 424.750000, 383.000000, 378.625000, 393.375000, 432.750000, 378.750000, 354.875000, 374.875000, 351.375000, 432.125000, 415.750000, 418.375000, 424.375000, 383.625000, 386.375000, 419.750000, 438.750000, 461.500000, 456.375000, 406.375000, 426.375000, 428.250000, 382.500000, 426.875000, 450.000000, 433.125000, 418.375000, 440.875000, 428.250000, 479.750000, 425.000000, 426.625000, 446.500000, 414.125000, 379.500000, 417.375000, 364.500000, 385.250000, 394.250000, 363.375000, 336.625000, 332.125000, 370.750000, 398.500000, 364.625000, 361.500000, 335.625000, 352.125000, 398.500000, 383.250000, 425.750000, 301.875000, 366.500000, 390.125000, 350.250000, 447.500000, 346.500000, 307.500000, 295.625000, 333.750000, 294.750000, 322.625000, 386.000000, 354.000000, 389.000000, 353.125000, 290.125000, 409.375000, 400.375000, 379.750000, 362.750000, 351.750000, 424.000000, 458.625000, 411.125000, 431.250000, 404.625000, 443.875000, 355.250000, 429.750000, 424.250000, 405.375000, 422.750000, 433.375000, 432.125000, 412.875000, 523.375000, 411.250000, 398.750000, 405.125000, 413.375000, 416.500000, 420.500000, 373.625000, 409.000000, 400.000000, 421.625000, 460.125000, 461.750000, 354.250000, 301.000000, 271.875000, 244.500000, 336.250000, 322.250000, 312.375000, 300.625000, 326.500000, 302.375000, 290.250000, 313.125000, 245.625000, 246.375000, 298.250000, 324.625000, 322.625000, 340.125000, 353.500000, 326.125000, 358.750000, 352.875000, 339.375000, 354.125000, 341.250000, 334.875000, 343.500000, 418.000000, 376.000000, 322.625000, 309.625000, 347.500000, 396.875000, 318.000000, 323.750000, 342.875000, 341.250000, 368.375000, 379.125000, 388.125000, 400.750000, 375.500000, 311.375000, 403.625000, 391.875000, 353.750000, 405.750000, 415.875000, 420.000000, 419.750000, 418.250000, 381.625000, 440.875000, 429.250000, 429.375000, 408.750000, 437.875000, 410.500000, 420.875000, 380.625000, 400.625000, 378.875000, 365.250000, 387.875000, 406.625000, 200.000000, 268.125000, 254.375000, 233.875000, 230.250000, 229.000000, 262.500000, 303.125000, 231.000000, 267.750000, 232.250000, 247.750000, 290.750000, 279.875000, 282.500000, 318.500000, 299.750000, 347.750000, 313.375000, 357.500000, 340.375000, 339.750000, 316.875000, 313.500000, 321.500000, 295.875000, 277.625000, 336.000000, 366.875000, 389.250000, 303.125000, 297.500000, 309.375000, 326.625000, 360.625000, 367.875000, 365.250000, 356.250000, 403.375000, 394.000000, 500.750000, 476.750000, 431.500000, 432.750000, 466.250000, 459.500000, 553.500000, 546.125000, 487.125000, 489.000000, 544.625000, 502.500000, 519.375000, 463.250000, 539.250000, 542.500000, 504.750000, 557.625000, 599.750000, 531.750000, 497.250000, 572.125000, 520.000000, 542.875000, 353.500000, 439.250000, 387.000000, 411.500000, 468.375000, 413.000000, 446.625000, 452.375000, 464.625000, 460.375000, 394.250000, 399.625000, 443.125000, 411.375000, 385.625000, 476.875000, 434.875000, 437.375000, 438.125000, 345.750000, 464.500000, 439.750000, 418.875000, 477.125000, 457.625000, 453.125000, 486.375000, 374.250000, 445.250000, 425.250000, 477.875000, 517.875000, 458.375000, 468.000000, 443.500000, 415.250000, 467.375000, 432.750000, 464.875000, 433.375000, 414.500000, 464.500000, 442.000000, 410.000000, 419.125000, 425.000000, 395.000000, 442.375000, 422.875000, 461.250000, 398.625000, 413.500000, 415.125000, 484.750000, 450.500000, 507.750000, 502.125000, 474.500000, 485.875000, 476.500000, 478.875000, 493.000000, 485.125000, 512.625000}; 

  double sigmas[640] = {4.250000, 3.375000, 3.000000, 2.875000, 2.750000, 2.875000, 2.750000, 3.000000, 2.875000, 2.875000, 3.000000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.875000, 2.875000, 2.625000, 2.750000, 2.875000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 3.000000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.875000, 2.875000, 2.625000, 2.875000, 2.625000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 3.000000, 2.875000, 3.000000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.875000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 8.125000, 2.625000, 2.750000, 2.750000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.500000, 2.625000, 2.750000, 2.625000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.500000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.625000, 2.750000, 2.750000, 3.125000, 2.875000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.500000, 2.500000, 2.625000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.500000, 2.500000, 4.500000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.500000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.875000, 2.750000, 2.875000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.875000, 2.625000, 2.625000, 2.750000, 2.500000, 2.500000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 7.250000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.375000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.500000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.500000, 2.625000, 2.500000, 2.375000, 2.625000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 4.125000, 3.250000, 2.625000, 3.000000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.750000, 2.500000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.750000, 2.500000, 2.625000, 2.500000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.875000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.375000, 2.500000, 2.625000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.875000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.500000, 2.750000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.500000, 2.625000, 2.500000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.875000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 3.250000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.500000, 2.750000, 24.250000, 2.750000, 2.625000, 2.500000, 2.375000, 2.625000, 7.000000, 2.625000, 7.125000, 2.750000, 8.625000, 2.750000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.500000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.875000, 3.250000};

  double sigmaraws[640] = {11.750000, 11.625000, 10.250000, 9.750000, 9.750000, 9.750000, 9.750000, 9.875000, 9.750000, 9.750000, 9.875000, 10.000000, 9.625000, 10.000000, 9.750000, 9.625000, 9.750000, 9.375000, 9.750000, 9.750000, 9.625000, 9.750000, 9.500000, 9.750000, 9.625000, 9.500000, 9.625000, 9.625000, 9.625000, 9.625000, 9.500000, 9.500000, 9.500000, 9.750000, 9.625000, 9.625000, 9.500000, 9.500000, 9.500000, 9.375000, 9.625000, 9.500000, 9.250000, 9.375000, 9.375000, 9.375000, 9.375000, 9.625000, 9.625000, 9.625000, 9.750000, 9.750000, 9.625000, 9.875000, 9.625000, 9.500000, 9.375000, 9.750000, 9.500000, 9.750000, 9.625000, 9.500000, 9.375000, 9.375000, 9.500000, 9.125000, 9.125000, 9.375000, 9.125000, 9.000000, 9.000000, 8.875000, 9.125000, 8.875000, 9.000000, 9.125000, 9.000000, 8.875000, 9.000000, 9.125000, 9.125000, 9.000000, 9.000000, 8.750000, 9.125000, 9.000000, 8.875000, 9.000000, 8.875000, 9.125000, 9.000000, 9.000000, 9.000000, 8.875000, 9.125000, 9.000000, 9.125000, 8.875000, 9.125000, 8.875000, 9.250000, 2.500000, 8.750000, 8.875000, 8.875000, 9.125000, 9.125000, 9.125000, 9.000000, 9.375000, 9.250000, 9.125000, 9.125000, 9.125000, 9.000000, 9.125000, 9.000000, 9.000000, 9.000000, 9.000000, 9.000000, 9.000000, 9.000000, 8.750000, 8.875000, 8.750000, 8.750000, 8.625000, 9.250000, 9.125000, 8.750000, 8.875000, 8.625000, 8.875000, 8.875000, 8.750000, 8.875000, 8.875000, 8.625000, 8.875000, 8.750000, 8.750000, 8.625000, 8.625000, 8.750000, 8.750000, 8.750000, 8.625000, 8.750000, 8.625000, 8.875000, 8.625000, 8.750000, 8.875000, 8.625000, 8.750000, 8.625000, 8.375000, 8.625000, 8.625000, 8.625000, 8.500000, 8.625000, 8.750000, 8.500000, 8.500000, 8.625000, 8.500000, 8.375000, 8.500000, 8.375000, 8.375000, 8.375000, 8.250000, 8.500000, 8.375000, 8.500000, 8.250000, 8.375000, 8.500000, 8.500000, 8.250000, 8.250000, 8.125000, 8.375000, 8.250000, 8.250000, 8.375000, 8.250000, 8.125000, 8.125000, 8.000000, 9.000000, 9.000000, 8.750000, 9.000000, 8.375000, 8.625000, 8.625000, 8.625000, 8.500000, 8.625000, 8.625000, 8.625000, 8.375000, 8.375000, 8.500000, 8.500000, 8.500000, 8.250000, 8.625000, 8.500000, 8.500000, 8.375000, 8.500000, 8.125000, 8.375000, 8.250000, 8.375000, 8.250000, 8.250000, 8.250000, 8.375000, 6.375000, 8.375000, 8.250000, 8.500000, 8.375000, 8.375000, 8.500000, 8.500000, 8.500000, 8.500000, 8.500000, 8.375000, 8.875000, 8.500000, 8.500000, 8.625000, 8.375000, 8.625000, 8.500000, 8.625000, 8.500000, 8.625000, 8.625000, 8.500000, 8.500000, 8.250000, 8.500000, 8.500000, 8.250000, 8.500000, 8.250000, 8.125000, 8.125000, 9.000000, 8.875000, 8.625000, 8.750000, 8.750000, 8.750000, 8.375000, 8.500000, 8.375000, 8.375000, 8.500000, 8.250000, 8.625000, 8.375000, 8.500000, 8.500000, 8.375000, 8.375000, 8.375000, 8.500000, 8.625000, 8.375000, 8.250000, 8.625000, 8.125000, 8.375000, 8.250000, 8.125000, 8.250000, 8.375000, 8.000000, 8.000000, 8.125000, 8.250000, 8.250000, 8.125000, 8.125000, 8.000000, 8.000000, 8.125000, 7.875000, 8.000000, 7.875000, 8.250000, 7.875000, 8.000000, 8.000000, 8.125000, 8.000000, 8.125000, 8.125000, 8.000000, 7.625000, 7.875000, 8.000000, 7.875000, 8.000000, 8.000000, 7.875000, 8.000000, 8.125000, 7.875000, 7.750000, 7.625000, 2.250000, 7.500000, 7.375000, 7.500000, 7.375000, 7.375000, 7.500000, 7.500000, 7.500000, 7.375000, 7.625000, 7.500000, 7.625000, 7.500000, 7.500000, 7.375000, 7.500000, 7.750000, 7.375000, 7.500000, 7.500000, 7.625000, 7.500000, 7.500000, 7.750000, 7.625000, 7.500000, 7.500000, 7.625000, 7.625000, 7.375000, 7.375000, 7.500000, 7.500000, 7.500000, 7.500000, 7.500000, 7.500000, 7.375000, 7.375000, 7.375000, 7.500000, 7.375000, 7.375000, 7.500000, 7.750000, 7.625000, 7.500000, 7.500000, 7.375000, 7.375000, 7.500000, 7.250000, 7.250000, 7.375000, 7.250000, 7.250000, 7.125000, 7.125000, 7.250000, 7.500000, 7.250000, 7.500000, 7.125000, 8.875000, 8.625000, 8.375000, 8.500000, 8.125000, 8.250000, 8.250000, 8.125000, 8.375000, 8.250000, 8.125000, 8.125000, 8.250000, 8.125000, 8.000000, 7.750000, 7.750000, 7.875000, 7.750000, 7.750000, 7.875000, 8.000000, 7.875000, 7.625000, 8.000000, 7.875000, 7.750000, 7.750000, 7.750000, 7.625000, 7.750000, 7.750000, 7.875000, 7.625000, 7.750000, 7.875000, 8.000000, 7.625000, 7.625000, 7.750000, 7.750000, 7.750000, 7.625000, 7.500000, 7.625000, 7.500000, 7.500000, 7.500000, 7.375000, 7.500000, 7.500000, 7.500000, 7.625000, 7.500000, 7.500000, 7.375000, 7.625000, 7.500000, 7.375000, 7.375000, 7.500000, 7.125000, 7.250000, 7.250000, 8.875000, 8.750000, 8.625000, 8.250000, 8.500000, 8.375000, 8.250000, 8.375000, 8.250000, 8.125000, 8.375000, 8.375000, 8.125000, 8.125000, 8.250000, 8.125000, 8.125000, 8.000000, 8.000000, 8.125000, 8.125000, 8.000000, 8.000000, 8.125000, 7.875000, 8.125000, 8.000000, 8.000000, 8.000000, 7.750000, 7.875000, 7.875000, 7.750000, 7.625000, 7.625000, 7.750000, 7.625000, 7.750000, 7.625000, 7.750000, 7.750000, 7.375000, 7.500000, 7.500000, 7.750000, 7.500000, 7.375000, 7.250000, 7.375000, 7.500000, 7.500000, 7.375000, 7.375000, 7.500000, 7.375000, 7.375000, 7.500000, 7.625000, 7.500000, 7.500000, 7.500000, 7.375000, 7.375000, 7.250000, 9.250000, 9.125000, 9.000000, 9.000000, 9.000000, 9.000000, 8.750000, 8.875000, 8.875000, 9.000000, 9.000000, 8.875000, 8.875000, 9.000000, 8.750000, 8.875000, 8.750000, 8.875000, 8.875000, 9.000000, 9.000000, 9.000000, 8.750000, 8.750000, 8.625000, 8.875000, 8.875000, 8.625000, 8.625000, 8.750000, 8.625000, 8.500000, 8.875000, 8.500000, 8.750000, 8.625000, 8.750000, 8.625000, 8.500000, 8.875000, 8.750000, 8.875000, 8.750000, 8.625000, 8.625000, 8.625000, 8.750000, 8.750000, 8.625000, 8.750000, 8.750000, 9.000000, 8.750000, 8.875000, 8.875000, 8.625000, 9.000000, 8.875000, 8.875000, 8.750000, 9.000000, 8.750000, 8.750000, 8.750000, 8.750000, 8.500000, 8.375000, 8.250000, 8.250000, 8.250000, 8.250000, 8.125000, 8.250000, 8.125000, 8.375000, 8.250000, 8.125000, 8.125000, 8.250000, 8.125000, 8.125000, 8.125000, 7.875000, 8.000000, 8.125000, 7.875000, 8.000000, 8.000000, 7.750000, 8.000000, 24.750000, 8.125000, 8.000000, 8.125000, 8.125000, 8.125000, 3.250000, 8.250000, 3.250000, 8.000000, 15.125000, 8.000000, 7.875000, 8.000000, 7.875000, 7.875000, 7.750000, 7.875000, 7.625000, 7.875000, 7.875000, 7.875000, 8.000000, 7.875000, 8.000000, 7.750000, 7.875000, 8.000000, 7.625000, 7.500000, 7.750000, 7.625000, 7.750000, 7.625000, 7.750000, 8.000000, 9.000000, 9.375000};
  
  std::vector<double> values[4];
  values[0].assign(pedestals, pedestals+640);
  values[1].assign(sigmas, sigmas+640);
  values[2].assign(sigmaraws, sigmaraws+640);
  values[3].resize(640);

  std::vector<TH1F*> histos2;
  
  const char* names[4] = {"Pedestals", "Sigma", "RawSigma", "AverageSignal"};
  
  for (int ii=0; ii<4; ii++) {
    std::pair<TH1F*, TH1F*> coppia = VectorToHisto(values[ii], names[ii], "AMS-flight");
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    // if (ii!=3) {
    //   printf("%s %s %p %p\n", "AMS-flight", names[ii], h, h2);
    // }
    histos[0].push_back(h);
    histos2.push_back(h2);
  }

  for (int ii=0; ii<3; ii++) {//not including the spread on the average signal
    histos[0].push_back(histos2.at(ii));
  }
  
  return;
}

std::vector<double> HistoToVector(TH1F* histo){

  std::vector<double> values(histo->GetNbinsX());

  for (int bb=1; bb<=histo->GetNbinsX(); bb++) {
    values[bb-1] = histo->GetBinContent(bb);
  }

  return values;
}

std::pair<TH1F*, TH1F*> VectorToHisto(std::vector<double> values, const char* name, const char* filename, int nch, int ladder, int nspread){

  TH1F* h = NULL;
  TH1F* h2 = NULL;
  if (ladder==-999) {
    h = new TH1F(Form("%s_%s", name, filename), Form("%s_%s", name, filename), nch, 0, nch);
    h2 = new TH1F(Form("%s_spread_%s", name, filename), Form("%s_spread_%s", name, filename), nspread*100, 0, nspread);
  }
  else {
    h = new TH1F(Form("%s_%s_%d", name, filename, ladder), Form("%s_%s_%d", name, filename, ladder), nch, 0, nch);
    h2 = new TH1F(Form("%s_spread_%s_%d", name, filename, ladder), Form("%s_spread_%s_%d", name, filename, ladder), nspread*100, 0, nspread);
  }

  
  TString name_ = name;
  TString filename_ = filename;
  
  auto nPoints = (int)(values.size());
  for(int i=0; i < nPoints; ++i) {
    /* if (name_ == "AverageSignal") { */
    /*   printf("%d) %f\n", i, values[i]); */
    /* } */
    h->Fill(i+0.5, values[i]);
    double val_for_comparison = values[i];
    if (filename_.Contains("LEF") || filename_.Contains("L0")) {
      if (i==0) printf("filename=%s (LEF/L0) so scaling values for comparison...\n", filename);
      val_for_comparison/=4.0;
    }
    h2->Fill(val_for_comparison);
  }

  std::pair<TH1F*, TH1F*> coppia = std::make_pair(h, h2);
			    
  return coppia;
}

void SummaryBeam(std::vector<std::vector<TH1F*>> histos, const char* filename, const char* nameout, int type) {

  int nva = 10;
  int nch_for_va = 64;
  double max_occ = 0.01;
  double max_wei = 0.01;

  if (type==1) {//AMS-L0
    nva = 16;
    nch_for_va = 64;
  }
  else {//type=0, AMS, FOOT/POX, ...
    nva=10;
    nch_for_va = 64;
  }

  int nch = nva*nch_for_va;

  //loop on ladders
  for (int ll=0; ll<((int)histos.size()); ll++) {
    
    static int count=-1;
    count++;
    
    TCanvas *c;
    
    if (((int)histos[ll].size())!=0) {
      TLatex *comment=new TLatex(1-0.01,0.01, Form("%s", filename));
      comment->SetNDC();  
      comment->SetTextAngle(90);
      comment->SetTextAlign(11);
      comment->SetTextSize(0.025);
      
      c = new TCanvas(Form("c_%d", count), Form("%s_%d", filename, ll),0*100,0*10,900,400);
      comment->Draw();
      c->Update();
      c->Divide(3,2);
      c->SetFillStyle(0);
      c->cd(1);
      gPad->SetFillStyle(0);
      
      c->cd(1);
      TH2F *fram=new TH2F(Form("fram1_%d", count), Form("%s_%d: occupancy", filename, ll),nch,1,nch,1000,0,max_occ);
      fram->SetStats(0);
      fram->Draw();
      TH1F *isto = histos[ll].at(0);
      isto->DrawNormalized("samehist");
      gPad->Update();
      LinesVas(nva, ((int)(nch/nva)));
      gPad->Update();
      
      c->cd(4);
      gPad->SetFillStyle(0);
      TH2F *fram4=new TH2F(Form("fram4_%d", count), Form("%s_%d: weighted_occupancy", filename, ll),nch,1,nch,1000,0,max_wei);
      fram4->SetStats(0);
      fram4->Draw();
      TH1F *isto4 = histos[ll].at(1);
      isto4->DrawNormalized("samehist");
      gPad->Update();
      LinesVas(nva, ((int)(nch/nva)));
      gPad->Update();
      
      c->cd(2);
      gPad->SetLogy();
      gPad->SetFillStyle(0);
      TH2F *fram2=new TH2F(Form("fram2_%d", count), Form("%s_%d: signal", filename, ll),200,0,200, 1000, 0, 1);
      fram2->SetStats(0);
      fram2->Draw();
      TH1F *isto2 = histos[ll].at(2);
      isto2->DrawNormalized("samehist");
      gPad->Update();
      
      c->cd(5);
      gPad->SetLogy();
      gPad->SetFillStyle(0);
      TH2F *fram3=new TH2F(Form("fram3_%d", count), Form("%s_%d: signal_abovethresh", filename, ll),200,0,200, 1000, 0, 1);
      fram3->SetStats(0);
      fram3->Draw();
      TH1F *isto3 = histos[ll].at(3);
      isto3->DrawNormalized("samehist");
      gPad->Update();
      
      c->cd(3);
      gPad->SetLogy();
      gPad->SetFillStyle(0);
      TH2F *fram5=new TH2F(Form("fram5_%d", count), Form("%s_%d: signal_to_noise", filename, ll),10,0,10, 1000, 0, 1);
      fram5->SetStats(0);
      fram5->Draw();
      TH1F *isto5 = histos[ll].at(4);
      isto5->DrawNormalized("samehist");
      gPad->Update();
    }
    else {
      c = new TCanvas("c", "",0*100,0*10,600,400);
    }
    
    gROOT->SetStyle("StyleWhite");
    c->Update();
    
    c->Print(nameout);
    
    if (((int)histos[ll].size())==0) {
      if (c) delete c;
    }

  }
    
  return;
}

void SummaryCal(std::vector<std::vector<TH1F*>> histos, const char* filename, const char* nameout, int type) {
  
  int nva = 10;
  int nch_for_va = 64;
  double max_ped = 2000;
  double max_ave = 10.0;
  double max_sig = 10;
  double max_rsig = 10;

  if (type==1) {//AMS-L0
    nva = 16;
    nch_for_va = 64;
    max_ped = 4000;
    max_ave = 10.0;
    max_sig = 15;
    max_rsig = 15;
  }
  else {//type=0, AMS, FOOT/POX, ...
    nva=10;
    nch_for_va = 64;
    max_ped = 2000;
    max_ave = 10.0;
    max_sig = 10;
    max_rsig = 10;
  }

  int nch = nva*nch_for_va;

  //loop on ladders
  for (int ll=0; ll<((int)histos.size()); ll++) {
    
    static int count=-1;
    count++;
    
    TCanvas *c;
    
    if (((int)histos[ll].size())!=0) {
      TLatex *comment=new TLatex(1-0.01,0.01, Form("%s",filename));
      comment->SetNDC();  
      comment->SetTextAngle(90);
      comment->SetTextAlign(11);
      comment->SetTextSize(0.025);
      
      c = new TCanvas(Form("c_%d", count), Form("%s_%d", filename, ll),0*100,0*10,600,400);
      comment->Draw();
      c->Update();
      c->Divide(2,2);
      c->SetFillStyle(0);
      c->cd(1);
      gPad->SetFillStyle(0);
      
      c->cd(1);
      TH2F *fram=new TH2F(Form("fram_%d", count), Form("%s_%d: pedestals", filename, ll),nch,1,nch,max_ped,0,max_ped);
      fram->SetStats(0);
      fram->Draw();
      TH1F *isto = histos[ll].at(0);
      //  printf("%p\n", isto);
      if (calnohist) {
	for (int ii=0; ii<=isto->GetNbinsX()+1; ii++) {
	  isto->SetBinError(ii, 0);
	}
	isto->Draw("sameP");
      }
      else isto->Draw("samehist");
      gPad->Update();
      LinesVas(nva, ((int)(nch/nva)));
      gPad->Update();
      
      c->cd(2);
      gPad->SetFillStyle(0);
      TH2F *fram4=new TH2F(Form("fram4_%d", count), Form("%s_%d: average signal", filename, ll),nch,1,nch,max_ave,0,max_ave);
      fram4->SetStats(0);
      fram4->Draw();
      TH1F *isto4 = histos[ll].at(3);
      /* isto4->Dump(); */
      /* for (int bb=0; bb<=isto4->GetNbinsX()+1; bb++){ */
      /*   printf("%d) %f\n", bb, isto4->GetBinContent(bb)); */
      /* } */
      if (calnohist) {
	for (int ii=0; ii<=isto4->GetNbinsX()+1; ii++) {
	  isto4->SetBinError(ii, 0);
	}
	isto4->Draw("sameP");
      }
      else isto4->Draw("samehist");
      gPad->Update();
      LinesVas(nva, ((int)(nch/nva)));
      gPad->Update();
      
      c->cd(3);
      gPad->SetFillStyle(0);
      TH2F *fram2=new TH2F(Form("fram2_%d", count), Form("%s_%d: sigma", filename, ll),nch,1,nch,max_sig,0,max_sig);
      fram2->SetStats(0);
      fram2->Draw();
      TH1F *isto2 = histos[ll].at(1);
      if (calnohist) {
	for (int ii=0; ii<=isto2->GetNbinsX()+1; ii++) {
	  isto2->SetBinError(ii, 0);
	}
	isto2->Draw("sameP");
      }
      else isto2->Draw("samehist");
      gPad->Update();
      LinesVas(nva, ((int)(nch/nva)));
      gPad->Update();
      
      c->cd(4);
      gPad->SetFillStyle(0);
      TH2F *fram3=new TH2F(Form("fram3_%d", count), Form("%s_%d: sigma raw", filename, ll),nch,1,nch,max_rsig,0,max_rsig);
      fram3->SetStats(0);
      fram3->Draw();
      TH1F *isto3 = histos[ll].at(2);
      if (calnohist) {
	for (int ii=0; ii<=isto3->GetNbinsX()+1; ii++) {
	  isto3->SetBinError(ii, 0);
	}
	isto3->Draw("sameP");
      }
      else isto3->Draw("samehist");
      gPad->Update();
      LinesVas(nva, ((int)(nch/nva)));
      gPad->Update();
    }
    else {
      c = new TCanvas("c", "",0*100,0*10,600,400);
    }
    
    gROOT->SetStyle("StyleWhite");
    c->Update();
    
    c->Print(nameout);
    
    if (((int)histos[ll].size())==0) {
      /* if (comment) delete comment; */
      if (c) delete c;
      /* if (fram) delete fram; */
      /* if (fram2) delete fram2; */
      /* if (fram3) delete fram3; */
      /* if (isto) delete isto; */
      /* if (isto2) delete isto2; */
      /* if (isto3) delete isto3; */
    }

  }

  return;
}

void LinesVas(int nva, int nch_for_va) {
  Double_t xmin,ymin,xmax,ymax;
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  TLine *line=new TLine();
  line->SetLineColor(kBlue);
  TText *text=new TText();
  text->SetTextAlign(21); // vert and hor. alignment: horiz centered, top
  text->SetTextColor(kBlue);
  line->SetLineStyle(3);
  for (Int_t va=1; va<=nva; va++) {// first and last separation lines are not drawn
    // line->SetLineStyle((va==10)?1:3);
    // if (va==11) {//this change the color from now on, since the `line` is always the same
    //   line->SetLineColor(kRed); text->SetTextColor(kRed);
    // }
    if (va*nch_for_va>xmin && va*nch_for_va<xmax) line->DrawLine(1+va*nch_for_va,ymin,1+va*nch_for_va,ymax);
    if (va*nch_for_va>xmin && va*nch_for_va<=xmax) text->DrawText((va-0.5)*nch_for_va,ymax+(ymax-ymin)*0.01,Form("%d",va));
  }

  if (line) delete line;
  if (text) delete text;
}

int InitStyle() {
  gROOT->SetStyle("Plain");

  TStyle *myStyle[2], *tempo;
  myStyle[0]=new TStyle("StyleWhite", "white");
  myStyle[1]=new TStyle("StyleBlack", "black");

  tempo=gStyle;
  Int_t linecol, bkgndcol, histcol;

  for(Int_t style=0; style<2; style++) {

    linecol=kWhite*style+kBlack*(1-style);
    bkgndcol=kBlack*style+kWhite*(1-style);
    if (calred) histcol=kRed+2;
    else histcol=kYellow*style+kBlack*(1-style); // was 95

    myStyle[style]->Copy(*tempo);

    myStyle[style]->SetCanvasBorderMode(0);
    myStyle[style]->SetCanvasBorderSize(1);
    myStyle[style]->SetFrameBorderSize(1);
    myStyle[style]->SetFrameBorderMode(0);
    myStyle[style]->SetPadBorderSize(1);
    myStyle[style]->SetStatBorderSize(1);
    myStyle[style]->SetTitleBorderSize(1);
    myStyle[style]->SetPadBorderMode(0);
    myStyle[style]->SetPalette(1,0);
    myStyle[style]->SetPaperSize(20,27);	
    myStyle[style]->SetFuncColor(kRed);
    myStyle[style]->SetFuncWidth(1);
    myStyle[style]->SetLineScalePS(1); 
    myStyle[style]->SetCanvasColor(bkgndcol);
    myStyle[style]->SetAxisColor(linecol,"XYZ");
    myStyle[style]->SetFrameFillColor(bkgndcol);
    myStyle[style]->SetFrameLineColor(linecol);
    myStyle[style]->SetLabelColor(linecol,"XYZ");
    myStyle[style]->SetPadColor(bkgndcol);
    myStyle[style]->SetStatColor(bkgndcol);
    myStyle[style]->SetStatTextColor(linecol);
    myStyle[style]->SetTitleColor(linecol,"XYZ");
    myStyle[style]->SetTitleFillColor(bkgndcol);
    myStyle[style]->SetTitleTextColor(linecol);
    myStyle[style]->SetLineColor(linecol);
    myStyle[style]->SetMarkerColor(histcol);
    myStyle[style]->SetMarkerStyle(20);
    if (calnohist) {
      myStyle[style]->SetMarkerSize(0.5);
    }
    myStyle[style]->SetTextColor(linecol);

    myStyle[style]->SetGridColor((style)?13:kBlack);
    myStyle[style]->SetHistFillStyle(1001*(1-style));
    myStyle[style]->SetHistLineColor(histcol);
    if (!calred) {
      myStyle[style]->SetHistFillColor((style)?bkgndcol:kYellow);
      //      myStyle[style]->SetHistFillColor(histcol);
    }
    myStyle[style]->SetOptDate(22);
    myStyle[style]->GetAttDate()->SetTextColor(linecol);
    myStyle[style]->GetAttDate()->SetTextAngle(90);
    myStyle[style]->GetAttDate()->SetTextSize(0.01);
    myStyle[style]->SetDateY(1);
    myStyle[style]->SetDateX(1);
  }

  myStyle[1]->cd();

  gROOT->ForceStyle();

  //if (myStyle[0]) delete myStyle[0];
  //if (myStyle[1]) delete myStyle[1];

  return 0;
}

void Comparison(std::vector<std::vector<TH1F*>> histos[3], const char* nameout) {
  
  //loop on ladders
  for (int ll=0; ll<((int)histos[0].size()); ll++) {//component 0, 1 and 2 must be identical...

    if (histos[0][ll].size()<=0) continue; 
    
    static int count=-1;
    count++;

    Color_t colors[11] = {kRed+2, kGreen+2, kBlue+2, kCyan, kBlack, kPink, kRed, kGreen, kBlue, kOrange, kYellow};
    if ((int)(histos[0][ll].size())>11) {
      printf("I'll crash...\n");
    }
  
    TCanvas *c;
    
    c = new TCanvas(Form("comp_c_%d", count), Form("%d", ll),0*100,0*10,600,400);
    c->Update();
    c->Divide(2,2);
    c->SetFillStyle(0);
    c->cd(1);
    gPad->SetFillStyle(0);
  
    c->cd(1);
    TH2F *fram=new TH2F(Form("comp_fram_%d", count), Form("pedestals %d", ll), 200, 0, 2000, 100, 0, 100);
    fram->GetXaxis()->SetTitle("ADC");
    fram->GetYaxis()->SetTitle("Channels");
    fram->SetStats(0);
    fram->Draw();
    for (int ii=0; ii<(int)(histos[0][ll].size()); ii++) {
      histos[0][ll].at(ii)->SetFillStyle(0);
      histos[0][ll].at(ii)->SetLineColor(colors[ii]);
      histos[0][ll].at(ii)->SetLineWidth(3);
      histos[0][ll].at(ii)->Rebin(pow(2, 10));
      histos[0][ll].at(ii)->Draw("samehist");
    }
    gPad->Update();
  
    c->cd(3);
    gPad->SetFillStyle(0);
    TH2F *fram2=new TH2F(Form("comp_fram2_%d", count), Form("sigma %d", ll), 1000, 0, 10, 450, 0, 450);
    fram2->GetXaxis()->SetTitle("ADC");
    fram2->GetYaxis()->SetTitle("Channels");
    fram2->SetStats(0);
    fram2->Draw();
    for (int ii=0; ii<(int)(histos[1][ll].size()); ii++) {
      histos[1][ll].at(ii)->SetFillStyle(0);
      histos[1][ll].at(ii)->SetLineColor(colors[ii]);
      histos[1][ll].at(ii)->SetLineWidth(3);
      histos[1][ll].at(ii)->Rebin(pow(2, 2));
      histos[1][ll].at(ii)->Draw("samehist");
    }
    gPad->Update();
  
    c->cd(4);
    gPad->SetFillStyle(0);
    TH2F *fram3=new TH2F(Form("comp_fram3_%d", count), Form("sigma raw %d", ll), 1500, 0, 15, 200, 0, 200);
    fram3->GetXaxis()->SetTitle("ADC");
    fram3->GetYaxis()->SetTitle("Channels");
    fram3->SetStats(0);
    fram3->Draw();
    for (int ii=0; ii<(int)(histos[2][ll].size()); ii++) {
      histos[2][ll].at(ii)->SetFillStyle(0);
      histos[2][ll].at(ii)->SetLineColor(colors[ii]);
      histos[2][ll].at(ii)->SetLineWidth(3);
      histos[2][ll].at(ii)->Rebin(pow(2, 2));
      histos[2][ll].at(ii)->Draw("samehist");
    }
    gPad->Update();

    gROOT->SetStyle("StyleWhite");
    c->Update();

    c->Print(nameout);

  }

  return;
}

int ReadFile(void* ptr, size_t size, size_t nitems, FILE* stream){
  
  int ret = 0;
  ret = fread(ptr, size, nitems, stream);
  if (feof(stream)) {
    printf("\n");
    printf("End of File \n");
    return -1;
  }
  if (ferror(stream)) {
    printf("Error reading \n");
    return -2;
  }

  return ret;
}

unsigned short int bit(int bitno, unsigned short int data) {

  return (data & 1<<bitno)>>bitno;
}

unsigned short int bit(int bitnofirst, int bitnolast, unsigned short int data) {

  unsigned short int mask = 0;
  for (int bb=bitnofirst; bb<=bitnolast; bb++) {
    mask |= (1<<bb);
  }
  
  return (data & mask)>>bitnofirst;
}

int ReadAMSBlockFile(unsigned int& dummy, unsigned int& size_consumed, unsigned int& size_to_read, FILE* stream){
  int fstat = 0;

  fstat = ReadFile(&dummy, sizeof(dummy), 1, stream);
  size_consumed+=sizeof(dummy);
  size_to_read-=sizeof(dummy);
  
  return fstat;
}

int ReadAMSBlockFile(unsigned short int& dummy, unsigned int& size_consumed, unsigned int& size_to_read, FILE* stream){
  int fstat = 0;

  fstat = ReadFile(&dummy, sizeof(dummy), 1, stream);
  size_consumed+=sizeof(dummy);
  size_to_read-=sizeof(dummy);
  
  return fstat;
}

int ReadAMSBlockSize(unsigned int& size_full, unsigned int& size_consumed, unsigned int& size_to_read, FILE* file){

  int fstat=0;
  
  unsigned short int size;
  unsigned short int size_ext;
  
  fstat = ReadFile(&size, sizeof(size), 1, file);
  size_consumed+=sizeof(size);
  if (fstat == -1) return fstat;
  if (openL0FEP_debug_level>1)
    printf("size: %u\n", size);
  
  if (bit(15, size)) {
    fstat = ReadFile(&size_ext, sizeof(size_ext), 1, file);
    size_consumed+=sizeof(size_ext);
    if (fstat == -1) return fstat;
    if (openL0FEP_debug_level>1)
      printf("size_ext: %u\n", size_ext);
    size &= 0x7fff;//mask the first bit
    size_full = (size<<16)  + size_ext;
  }
  else {
    size &= 0x7fff;//mask the first bit
    size_full = size;
  }
  
  if (openL0FEP_debug_level>0)
    printf("size_full: %u\n", size_full);
  size_to_read = size_full;

  return fstat;
}

int ReadAMSBlockRPRWNADT(bool& RP, bool& RW, unsigned short int& na, unsigned int& dt_full, unsigned int& size_consumed, unsigned int& size_to_read, FILE* file){

  int fstat=0;
    
  unsigned short int dummy16;//16 bit word

  unsigned short int dt;
  unsigned short int dt_ext;
  
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return fstat;

  RP = (bool)(bit(15, dummy16));
  if (openL0FEP_debug_level>3)
    printf("15: %d\n", RP);
  if (openL0FEP_debug_level>1)
    printf("Reply: %s\n", RP?"yes":"no");
  
  RW = (bool)(bit(14, dummy16));
  if (openL0FEP_debug_level>3)
    printf("14 (RW): %d\n", RW);
    
  na = bit(5, 13, dummy16); //from bit 5 to bit 13
  if (openL0FEP_debug_level>3)
    printf("5-13 (NA): 0x%hx\n", na);
  if (openL0FEP_debug_level>1)
    printf("NA: 0x%hx\n", na);
    
  dt = bit(0, 4, dummy16); //from bit 0 to bit 4
  if (openL0FEP_debug_level>3)
    printf("0-4 (DT): 0x%hx\n", dt);
  if (openL0FEP_debug_level>1)
    printf("DT: 0x%hx\n", dt);
    
  if (dt == 0x1f) {
    fstat = ReadAMSBlockFile(dt_ext, size_consumed, size_to_read, file);
    if (fstat == -1) return fstat;
    if (openL0FEP_debug_level>2)
      printf("DT (ext): 0x%hx\n", dt_ext);
    dt_full = (dt<<16)  + dt_ext;
  }
  else {
    dt_full = dt;
  }
    
  if (openL0FEP_debug_level>1)
    printf("DT (full): 0x%x\n", dt_full);

  return fstat;
}

int ReadAMSBlockStatusTag(unsigned short int& status, unsigned short int& tag, unsigned int& size_consumed, unsigned int& size_to_read, FILE* file){

  int fstat = 0;
  
  unsigned short int dummy16;//16 bit word
  
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return 1;

  status = bit(12, 15, dummy16); //from bit 12 to bit 15
  if (openL0FEP_debug_level>3)
    printf("12-15 (Status): 0x%hx\n", status);
  if (openL0FEP_debug_level>1)
    printf("Status: 0x%hx\n", status);
      
  tag = bit(0, 11, dummy16); //from bit 0 to bit 11
  if (openL0FEP_debug_level>3)
    printf("0-11 (Tag): 0x%hx\n", tag);
  if (openL0FEP_debug_level>1)
    printf("Tag: 0x%hx\n", tag);

  return fstat;
}

int ReadAMSBlockFineTime(unsigned int& size_consumed, unsigned int& size_to_read, FILE* file){

  int fstat = 0;

  unsigned short int dummy16;//16 bit word
    
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return fstat;
      
  unsigned short int utime_sec_msb = bit(0, 15, dummy16);
  if (openL0FEP_debug_level>2)
    printf("utime_sec_msb: %u\n", utime_sec_msb);
      
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return fstat;
      
  unsigned short int utime_sec_lsb = bit(0, 15, dummy16);
  if (openL0FEP_debug_level>2)
    printf("utime_sec_lsb: %u\n", utime_sec_lsb);
      
  unsigned int utime_sec = (utime_sec_msb<<16) + utime_sec_lsb;
  if (openL0FEP_debug_level>2)
    printf("UTime_sec: %u\n", utime_sec);
      
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return fstat;
      
  unsigned short int utime_usec_msb = bit(0, 15, dummy16);
  if (openL0FEP_debug_level>2)
    printf("utime_usec_msb: %u\n", utime_usec_msb);
      
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return fstat;
      
  unsigned short int utime_usec_lsb = bit(0, 15, dummy16);
  if (openL0FEP_debug_level>2)
    printf("utime_usec_lsb: %u\n", utime_usec_lsb);
      
  unsigned int utime_usec = (utime_usec_msb<<16) + utime_usec_lsb;
  if (openL0FEP_debug_level>2)
    printf("UTime_usec: %u\n", utime_usec);

  return fstat;
}

int ReadAMSBlockTime(unsigned int& size_consumed, unsigned int& size_to_read, FILE* file){

  int fstat = 0;

  unsigned short int dummy16;//16 bit word
  
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return fstat;
      
  unsigned short int utime_sec_msb = bit(0, 15, dummy16);
  if (openL0FEP_debug_level>2)
    printf("utime_sec_msb: %u\n", utime_sec_msb);
      
  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
  if (fstat == -1) return fstat;
      
  unsigned short int utime_sec_lsb = bit(0, 15, dummy16);
  if (openL0FEP_debug_level>2)
    printf("utime_sec_lsb: %u\n", utime_sec_lsb);
      
  unsigned int utime_sec = (utime_sec_msb<<16) + utime_sec_lsb;
  if (openL0FEP_debug_level>2)
    printf("UTime_sec: %u\n", utime_sec);

  return fstat;
}

int ProcessAMSBlock(FILE* file, unsigned int& size_consumed, int& ev_found,
		    std::vector<std::vector<std::vector<unsigned short>>>& signals_by_ev,
		    int nesting_level){

  if (openL0FEP_debug_level>4 && nesting_level==0)
    sleep(10);
  
  int fstat = 0;
  unsigned int size_to_read;
  size_consumed = 0;
  unsigned short int dummy16;//16 bit word
  unsigned int dummy32;//32 bit word
    
  unsigned int size_full;

  bool RP=false;
  bool RW=false;
  unsigned short int na;
  unsigned int dt_full;

  unsigned short int status;
  unsigned short int tag;

  bool call_recursively=false;
  
  if (openL0FEP_debug_level>0)
    printf("*************** New block (level %d) **************\n", nesting_level);

  fstat = ReadAMSBlockSize(size_full, size_consumed, size_to_read, file);
  if (fstat == -1) return 1;

  fstat = ReadAMSBlockRPRWNADT(RP, RW, na, dt_full, size_consumed, size_to_read, file);
  if (fstat == -1) return 1;

  if (RP==false) {//RP=false, command
    printf("Decoding of commands, not implemented. Only replies...\n");
  }
  else {//RP=true, reply 
    // DT cases
    if (dt_full == 0x1f0205) {//control Q-List (but not sure at all)
      if (RW) {
	fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
	if (fstat == -1) return 1;
	if (openL0FEP_debug_level>1)
	  printf("Item number: 0x%hx\n", dummy16);
      
	if (size_to_read==0) {//"Delete item in time based Q-List"
	}
	else if (size_to_read==2) {//"Control or delete a range of items in time based Q-List"
	  printf("Decoding not implemented\n");
	}
	else if (size_to_read>=4) {//"Change item start time in time based Q-List" or following
	  fstat = ReadAMSBlockFile(dummy32, size_consumed, size_to_read, file);
	  if (fstat == -1) return 1;
	  if (openL0FEP_debug_level>1)
	    printf("Start time: %u\n", dummy32);
	
	  if (size_to_read>=10) {//"Add items in time based Q-List"
	    fstat = ReadAMSBlockFile(dummy32, size_consumed, size_to_read, file);
	    if (fstat == -1) return 1;
	    if (openL0FEP_debug_level>1)
	      printf("Repeat time: %u\n", dummy32);
	  }
	}
      }
      else {//RW=0
	//"Read one item in time based Q-List" or //"Read whole time based Q-List" ? Something else?
	fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
	if (fstat == -1) return 1;
	if (openL0FEP_debug_level>1)
	  printf("Item number: 0x%hx\n", dummy16);

	if (size_to_read>=2) {
	  fstat = ReadAMSBlockFile(dummy32, size_consumed, size_to_read, file);
	  if (fstat == -1) return 1;
	  if (openL0FEP_debug_level>1)
	    printf("Start time: %u\n", dummy32);
	}

	if (size_to_read>=2) {
	  fstat = ReadAMSBlockFile(dummy32, size_consumed, size_to_read, file);
	  if (fstat == -1) return 1;
	  if (openL0FEP_debug_level>1)
	    printf("Repeat time: %u\n", dummy32);
	}

	/* not working since I find a size that is crazy...
	   call_recursively=true;
	*/
      }
    }
    else if (dt_full == 0x1f0383) {//fine time envelope
      fstat = ReadAMSBlockStatusTag(status, tag, size_consumed, size_to_read, file);
      if (fstat == -1) return 1;

      fstat = ReadAMSBlockFineTime(size_consumed, size_to_read, file);
      if (fstat == -1) return 1;

      call_recursively=true;
    }
    else if (dt_full == 0x13) {// SCI/CAL/CFG/HK/ (0x5, 0x6, 0x7, 0x8) + LVL3 + GPS data

      fstat = ReadAMSBlockStatusTag(status, tag, size_consumed, size_to_read, file);
      if (fstat == -1) return 1;

      fstat = ReadAMSBlockTime(size_consumed, size_to_read, file);
      if (fstat == -1) return 1;

      // and now, finally, the event...
      unsigned int raweventsize = 1794;
      if (size_to_read==0 && openL0FEP_debug_level>0) {
	printf("Empty event...\n");
      }
      else if (size_to_read%raweventsize==0) {//Vladimir raw event
	if (openL0FEP_debug_level>3) {
	  printf("Data to read: %u\n", size_to_read);
	}
	unsigned int total_size_to_read = size_to_read;
	for (int ii=0; ii<((int)(total_size_to_read/raweventsize)); ii++) {
	  
	  fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
	  if (fstat == -1) return 1;
	  
	  unsigned short int evtn = bit(0, 7, dummy16);//from bit 0 to bit 7
	  if (openL0FEP_debug_level>1)
	    printf("Event number: %u\n", evtn);
	  
	  unsigned short int link = bit(8, 15, dummy16);//from bit 8 to bit 15
	  if (openL0FEP_debug_level>1)
	    printf("Link: %u\n", link);
	  if ((link+1)>signals_by_ev.size()) {
	    signals_by_ev.resize(link+1);
	  }
	  
	  std::vector<unsigned char> data_end_nc;
	  std::vector<unsigned char> data;
	  data_end_nc.resize(raweventsize-2);
	  data.resize(raweventsize-2);
	  
	  fstat = ReadFile(&data_end_nc[0], raweventsize-2, 1, file);
	  size_consumed+=(raweventsize-2);
	  size_to_read-=(raweventsize-2);
	  if (fstat == -1) return 1;
	  
	  //Endianess correction
	  for (int ii=0; ii<((int)(data_end_nc.size())); ii+=2) {
	    data[ii] = data_end_nc[ii+1];
	    data[ii+1] = data_end_nc[ii];
	  }
	  
	  if (openL0FEP_debug_level>3) {
	    for (int ii=0; ii<((int)data.size()); ii++) {
	      printf("0x%02x\n", data[ii]);
	    }
	  }
	  
	  std::vector<unsigned short> data_ord;
	  ReOrderVladimir(data, data_ord);
	  if (openL0FEP_debug_level>3) {
	    for (int ii=0; ii<((int)data_ord.size()); ii++) {
	      printf("%d\n", data_ord[ii]);
	    }
	  }
	  
	  if (openL0FEP_debug_level>1) {
	    printf("Array of signals has size %d\n", ((int)(data_ord.size())));
	  }
	  signals_by_ev[((int)link)].push_back(data_ord);
	}

	if (openL0FEP_debug_level>1) {
	  for (int ll=0; ll<((int)(signals_by_ev.size())); ll++) {
	    printf("Component %d of array of events has size %d\n", ll, ((int)(signals_by_ev[ll].size())));
	  }
	}
	ev_found++;
      }
      else {
	if (openL0FEP_debug_level>1) {
	  printf("Wrong raw event size (%d): it should be 0 or 1794...\n", size_to_read);
	}
      }
    }
    else if (dt_full == 0x4) {//Trigger and DAQ Control write/read (still not clear and complete...)
      //      if (RW) {
      fstat = ReadAMSBlockStatusTag(status, tag, size_consumed, size_to_read, file);
      if (fstat == -1) return 1;
      
      fstat = ReadAMSBlockTime(size_consumed, size_to_read, file);
      if (fstat == -1) return 1;
      //      }
    }
  }

  if (call_recursively) {
    //let's call recursively the same function to process the sub-block...
    unsigned int read_bytes = 0;
    int ret = ProcessAMSBlock(file, read_bytes, ev_found, signals_by_ev, nesting_level+1);
    if (ret!=0) return ret;
    size_consumed+=read_bytes;
    size_to_read-=read_bytes;
  }

  if (size_to_read>0) {
    if (openL0FEP_debug_level>0)
      printf("Still to read %d bytes. Skipping them...\n", size_to_read);

    if (openL0FEP_debug_level>3) {
      printf("Skipped 16 bit words:\n");
      while (size_to_read>0) {
	fstat = ReadAMSBlockFile(dummy16, size_consumed, size_to_read, file);
	if (fstat == -1) return 1;
	printf("0x%hx\n", dummy16);
      }
    }
    else {
      // skip all the block data
      if (int ret0 = fseek(file, size_to_read, SEEK_CUR)) {
	printf("Fatal: error during file skip ret0=%d \n", ret0);
	return -99;
      }
      size_consumed+=size_to_read;
    }
  }
  
  if (openL0FEP_debug_level>0)
    printf("Size consumed: %d\n", size_consumed);

  if (openL0FEP_debug_level>0)
    printf("************** End block (level %d) ***************\n", nesting_level);
  
  return 0;
}

void OpenAMSL0FEPFile(TString filename, std::vector<std::vector<TH1F*>>& histos, bool kCal){
  
  // File open
  FILE* file = fopen(filename.Data(), "rb");
  if (file == NULL) {
    printf("Error file %s not found\n", filename.Data());
    exit(2);
  }
  else {
    printf("File %s opened\n", filename.Data());
  }
  
  std::vector<std::vector<std::vector<unsigned short>>> signals_by_ev;
  std::vector<std::vector<std::vector<unsigned short>>> signals;
  
  int nev=0;
  while (1) {
    unsigned int read_bytes = 0;
    int ret = ProcessAMSBlock(file, read_bytes, nev, signals_by_ev, 0);
    if (ret!=0) break;
  }
  printf("We read %d events\n", nev);

  if (openL0FEP_debug_level>0) {
    printf("Signals has %d components\n", ((int)(signals_by_ev.size())));
    for (int ii=0; ii<((int)(signals_by_ev.size())); ii++) {
      printf("Component %d has %d events\n", ii, ((int)(signals_by_ev[ii].size())));
      if (((int)(signals_by_ev[ii].size()))>0) {
	printf("Event 0 has %d values\n", ((int)(signals_by_ev[ii][0].size())));
      }
    }
  }

  if (file)
    fclose(file);
  file = NULL;

  //the ComputeXXXVladimir functions will re-do this to be sure, but would be correct to provide equally sized vectors
  histos.resize(signals_by_ev.size());
  signals.resize(signals_by_ev.size());
  
  if (kCal) {
    ComputeCalibrationVladimir(signals_by_ev, signals, nev, filename, histos);
  }
  else {
    ComputeBeamVladimir(signals_by_ev, signals, nev, filename, histos);
  }
  
#ifdef EV_DISP
  new MyMainFrame(gClient->GetRoot(),1200,800);
#endif 
  return;
}

void SaveAMSL0FEPFileGigi(TString filename){
  
  // File open
  FILE* file = fopen(filename.Data(), "rb");
  if (file == NULL) {
    printf("Error file %s not found\n", filename.Data());
    exit(2);
  }
  else {
    printf("File %s opened\n", filename.Data());
  }
  
  std::vector<std::vector<std::vector<unsigned short>>> signals_by_ev(1);
  std::vector<std::vector<std::vector<unsigned short>>> signals(1);
  
  int nev=0;
  while (1) {
    unsigned int read_bytes = 0;
    int ret = ProcessAMSBlock(file, read_bytes, nev, signals_by_ev, 0);
    if (ret!=0) break;
  }
  printf("We read %d events\n", nev);

  std::vector<unsigned short> raw_event_buffer;
  TFile *foutput = new TFile(filename + "_conv.root", "RECREATE", "Foot data");
  foutput->cd();

  TTree *raw_events_tree = new TTree("raw_events", "raw_events");
  raw_events_tree->Branch("RAW Event", &raw_event_buffer);
  
  for (auto & evt: signals_by_ev[0])
  {
    raw_event_buffer.clear();
    raw_event_buffer = evt;
    raw_events_tree->Fill();
  } 

  raw_events_tree->Write();
  foutput->Close();

  if (file)
    fclose(file);
  file = NULL;

  return;
}


TString Path2Name(const char *name, const char *sep, const char *exten){
   // Extract name from full path
   // sep is path separator and exten is name extension

   TString outname = TString(name);
   char   *tmpname = new char[strlen(name) + 1];
   char   *delname = tmpname;

   tmpname = strtok(strcpy(tmpname,name),sep);
   while(tmpname) {
      outname = tmpname;
      tmpname = strtok(NULL,sep);
   }//while
      
   if (strcmp(exten,"") != 0) {
      Int_t i = outname.Index(exten);
      if (i > 0) {outname = outname.Remove(i);}
   }//if

   delete [] delname;

   return outname;
}

//MD: provare a mettere i range degli isto automatici
void MyMainFrame::SaveCurrentRanges(){
  if (first_call==false){
    TCanvas *fCanvas = fEcanvas->GetCanvas();
    
    y1_min = fCanvas->GetPad(1)->GetFrame()->GetY1();
    y1_max = fCanvas->GetPad(1)->GetFrame()->GetY2();
    x1_min = fCanvas->GetPad(1)->GetFrame()->GetX1();
    x1_max = fCanvas->GetPad(1)->GetFrame()->GetX2();
    x2_min = fCanvas->GetPad(2)->GetFrame()->GetX1();
    x2_max = fCanvas->GetPad(2)->GetFrame()->GetX2();
    x3_min = fCanvas->GetPad(3)->GetFrame()->GetX1();
    x3_max = fCanvas->GetPad(3)->GetFrame()->GetX2();
    x4_min = fCanvas->GetPad(4)->GetFrame()->GetX1();
    x4_max = fCanvas->GetPad(4)->GetFrame()->GetX2();
  }
  else
    first_call=false;
}

//MD: provare a mettere i range degli isto automatici
void MyMainFrame::DoDraw(int nn_ev) {
  // Draws function graphics in randomly chosen interval
  int num_ev = fNumericEntries->GetIntNumber()+nn_ev;  
  fNumericEntries->SetIntNumber(num_ev);

  TH1F* h_ped = new TH1F(Form("Pedestal_%d",num_ev),"; Channel; Pedestal - Common Noise (ADC)",16*64,0,16*64);
  h_ped->GetYaxis()->SetRangeUser(y1_min,y1_max);
  h_ped->GetXaxis()->SetRangeUser(x1_min,x1_max);
 
  TH1F* h_sig  = new TH1F(Form("Sig_%d",num_ev),Form("#Event = %d; Raw Signal (ADC); Entries",num_ev),5000,0,5000);
  h_sig->GetXaxis()->SetRangeUser(x2_min,x2_max);

  TH1F* h_corr_sig  = new TH1F(Form("SigCorr_%d",num_ev),Form("#Event = %d; Signal (ADC); Entries",num_ev),5000,-2500,2500);
  h_corr_sig->GetXaxis()->SetRangeUser(x3_min,x3_max);

  TH1F* h_sig_over_noise  = new TH1F(Form("SigCorr_over_noise_%d",num_ev),Form("#Event = %d; Signal / Noise; Entries",num_ev),5000,-2500,2500);
  h_sig_over_noise->GetXaxis()->SetRangeUser(x4_min,x4_max);


  //MD: provare a mettere i range degli isto automatici
  for (int ii=0; ii<16*64; ii++)
    {
      h_ped->SetBinContent(ii,vec_of_ped[ii]);
      h_sig->Fill(vec_of_signals[num_ev][ii]);
      h_corr_sig->Fill(vec_of_signals[num_ev][ii]-vec_of_ped[ii]);
      h_sig_over_noise->Fill((vec_of_signals[num_ev][ii]-vec_of_ped[ii])/vec_of_noise[ii]);
    }
  
  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->cd();

  fCanvas->cd(1);
  h_ped->Draw();
  fCanvas->cd(2);
  h_sig->Draw();
  fCanvas->cd(3);
  h_corr_sig->Draw();
  fCanvas->cd(4);
  h_sig_over_noise->Draw();
  fCanvas->Update();

}
MyMainFrame::~MyMainFrame() {
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

