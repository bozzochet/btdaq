#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF1.h"
#include "TMath.h"
#include "TSpectrum.h"
#include <fstream>
#include <vector>
#include <initializer_list>

/* from the 'Decode' API */
#include "GenericEvent.hpp"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hpp"
#include "Utilities.hpp"
/* end */

//for an easy one-line initialization
std::vector<int> ladderS_to_ignore_jinf0 = {};
std::vector<int> ladderK_to_ignore_jinf0 = {};
// std::vector<int> ladderS_to_ignore_jinf0 = {2, 3, 6, 7, 10, 11, 14, 15, 18, 19};
// std::vector<int> ladderK_to_ignore_jinf0 = {2, 3, 6, 7, 10, 11, 14, 15, 18, 19};

int _npointsS=3;
int _npointsK=3;

double _chisqcutvalue=2;

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

float*** deltaalign;
bool** toprint;

//the real used ones...
std::vector<int>* ladderS_to_ignore;
std::vector<int>* ladderK_to_ignore;
template <class Event, class RH>
void AllocateGlobalVectors() {
  ladderS_to_ignore = new std::vector<int>[Event::GetNJINF()];
  for (int ii=0; ii<((int)(ladderS_to_ignore_jinf0.size())); ii++) {
    ladderS_to_ignore[0][ii] = ladderS_to_ignore_jinf0[ii]; 
  }
  ladderK_to_ignore = new std::vector<int>[Event::GetNJINF()];
  for (int ii=0; ii<((int)(ladderS_to_ignore_jinf0.size())); ii++) {
    ladderK_to_ignore[0][ii] = ladderK_to_ignore_jinf0[ii]; 
  }
}

template <class Event, class RH>
int Run(int argc, char* argv[],
	TString align_filename, TString gaincorrection_filename);
template <class Event, class RH>
int SingleRun(int argc, char* argv[],
	      TString align_filename, TString gaincorrection_filename,
	      int indexalignment,
	      int alignmeth=0, bool chisqcut=false, bool residualexcludedpoint=false, bool writealign=true);
template <class Event, class RH>
void ReadDeltaAlignment(TString filename="delta_alignment.dat");

using EventOCA = GenericEvent<1, 24, 64, 5, 10, 0>;
using calibOCA = calib<EventOCA::GetNCHAVA() * EventOCA::GetNVAS()>;
using RHClassOCA = RHClass<EventOCA::GetNJINF(), EventOCA::GetNTDRS()>;
//---
using EventFOOT = GenericEvent<1, 24, 64, 5, 10, 0>;
using calibFOOT = calib<EventFOOT::GetNCHAVA() * EventFOOT::GetNVAS()>;
using RHClassFOOT = RHClass<EventFOOT::GetNJINF(), EventFOOT::GetNTDRS()>;
//---
using EventAMS = GenericEvent<1, 24, 64, 3, 16, 10>;
using calibAMS = calib<EventAMS::GetNCHAVA() * EventAMS::GetNVAS()>;
using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;

int main(int argc, char* argv[]) {
  
  if (argc<2) {
    printf("Usage:\n");
    printf("%s <first input root-filename> [second input root-filename] ...\n", argv[0]);
    printf("\n");
    printf("%s                     : perform the full procedure\n", argv[0]);
    printf("NoFirstStepAlign       : perform the full procedure without the first rough pass with the Bruna's method\n");
    printf("FirstPassAlign         : perform just the first pass of the procedure (Bruna's method + one pass with residuals)\n");
    printf("NoLastStepAlign        : perform the full procedure without the last rough pass to center {S0,K0}\n");
    printf("DryRun{Bruna,Residual} : run once as in the alignment, but without updating the alignment.dat file\n");
    printf("OnlyFinalRun           : run once but with the chisq cut and the excluded point residuals, and without updating the alignment.dat file\n");
    return 1;
  }
  
  TString progname = argv[0];

  bool kOca = false;
  bool kFoot = false;
  
  if (progname.Contains("OCA")) {
    kOca = true;
  }
  else if (progname.Contains("FOOT")) {
    kFoot = true;
  }

  TString align_filename = "alignment.dat";
  TString gaincorrection_filename = "gaincorrection.dat";
  if (kOca) {
    align_filename = "alignment_OCA.dat";
    gaincorrection_filename = "gaincorrection_OCA.dat";
    return Run<EventOCA, RHClassOCA>(argc, argv, align_filename, gaincorrection_filename);
  }
  else if (kFoot) {
    align_filename = "alignment_FOOT.dat";
    gaincorrection_filename = "gaincorrection_FOOT.dat";
    return Run<EventFOOT, RHClassFOOT>(argc, argv, align_filename, gaincorrection_filename);
  }
  else {
    align_filename = "alignment.dat";
    gaincorrection_filename = "gaincorrection.dat";
    return Run<EventAMS, RHClassAMS>(argc, argv, align_filename, gaincorrection_filename);
  }
  
  return -8;
}

template <class Event, class RH>
int Run(int argc, char* argv[],
	TString align_filename, TString gaincorrection_filename){

  int ret=0;

  TString progname = argv[0];
  
  static int NJINF = Event::GetNJINF();
  static int NTDRS = Event::GetNTDRS();
  static int NVAS = Event::GetNVAS();
  static int NCHAVA = Event::GetNCHAVA();
  static int NADCS = Event::GetNADCS();

  static int firsttime=true;
  if (firsttime) {
    firsttime=false;
    
    deltaalign = new float**[NJINF];
    toprint = new bool*[NJINF];
    for (int ii=0; ii<NJINF; ii++) {
      deltaalign[ii] = new float*[NTDRS];
      toprint[ii] = new bool[NTDRS];
      for (int jj=0; jj<NJINF; jj++) {
	deltaalign[ii][jj] = new float[3];
      }
    }
    
    AllocateGlobalVectors<Event, RH>();
  }
  
  if (progname.Contains("Align")) {
    
    bool aligned[NJINF][NTDRS][2];
    for (int jj=0; jj<NJINF; jj++) {
      for (int tt=0; tt<NTDRS; tt++) {
	for (int cc=0; cc<2; cc++) {
	  aligned[jj][tt][cc]=false;
	  deltaalign[jj][tt][cc]=0.0;
	}
	if (std::find(ladderS_to_ignore[jj].begin(), ladderS_to_ignore[jj].end(), tt) != ladderS_to_ignore[jj].end()) {
	  aligned[jj][tt][0]=true;
	}
	if (std::find(ladderK_to_ignore[jj].begin(), ladderK_to_ignore[jj].end(), tt) != ladderK_to_ignore[jj].end()) {
	  aligned[jj][tt][1]=true;
	}
	toprint[jj][tt]=false;
      }
    }
    
    int indexalignment = 0;
    
    if (!progname.Contains("NoFirstStep")) {
      ret = SingleRun<Event, RH>(argc, argv, align_filename, gaincorrection_filename, indexalignment++, 1);//first alignment with the 'Bruna's method'
    }
    if (ret) return ret;
    
    bool allaligned=false;
    while (!allaligned) {
      
      ret = SingleRun<Event, RH>(argc, argv, align_filename, gaincorrection_filename, indexalignment);
      if (ret) return ret;
      indexalignment++;
      
      float deltaalign_old[NJINF][NTDRS][3];
      for (int jj=0; jj<NJINF; jj++) {
	for (int tt=0; tt<NTDRS; tt++) {
	  for (int cc=0; cc<2; cc++) {
	    deltaalign_old[jj][tt][cc] = deltaalign[jj][tt][cc];
	  }
	}
      }
      
      ReadDeltaAlignment<Event, RH>(Form("delta_%s", align_filename.Data()));
      
      for (int jj=0; jj<NJINF; jj++) {
	for (int tt=0; tt<NTDRS; tt++) {
	  for (int cc=0; cc<2; cc++) {
	    if (cc==0 && (std::find(ladderS_to_ignore[jj].begin(), ladderS_to_ignore[jj].end(), tt) != ladderS_to_ignore[jj].end())) continue;
	    if (cc==1 && (std::find(ladderK_to_ignore[jj].begin(), ladderK_to_ignore[jj].end(), tt) != ladderK_to_ignore[jj].end())) continue;
	    if (
		//		!aligned[jj][tt][cc] &&
		(fabs(deltaalign[jj][tt][cc])<0.001)
		) {
	      if (toprint[jj][tt]) {
		if (cc==0) printf("S ");
		else if (cc==1) printf("K ");
		printf("JINF %02d TDR %02d) OK!\n", jj, tt);
	      }
	      aligned[jj][tt][cc]=true;
	    }
	    else {
	      if (toprint[jj][tt]) {
		if (cc==0) printf("S ");
		else if (cc==1) printf("K ");
		printf("JINF %02d TDR %02d) KO\n", jj, tt);
	      }
	    }
	  }
	}
      }
      
      for (int jj=0; jj<NJINF; jj++) {
	for (int tt=0; tt<NTDRS; tt++) {
	  for (int cc=0; cc<2; cc++) {
	    if (!aligned[jj][tt][cc] &&
		fabs(deltaalign_old[jj][tt][cc])>1.0e-5 && (fabs(deltaalign[jj][tt][cc])-fabs(deltaalign_old[jj][tt][cc]))>0.1
		) {
	      if (toprint[jj][tt]) {
		if (cc==0) printf("S ");
		else if (cc==1) printf("K ");
		printf("JINF %02d TDR %02d) alignment didn't converge...\n", jj, tt);
	      }
	      aligned[jj][tt][cc]=true;
	    }
	  }
	}
      }
      
      // for (int jj=0; jj<NJINF; jj++) {
      //   for (int tt=0; tt<NTDRS; tt++) {
      // 	for (int cc=0; cc<5; cc++) {
      // 	  if (cc==0) printf("JINF %02d TDR %02d)\t", jj, tt);
      // 	  if (cc>=0 && cc<3) printf("%f\t", deltaalign[jj][tt][cc]);
      // 	  if (cc>=3 && cc<5) printf("-->\t%d\t", aligned[jj][tt][cc-3]);
      // 	  if (cc==4) printf("\n");
      //     }
      //   }
      // }
      
      bool allaligned_temp = true;
      for (int jj=0; jj<NJINF; jj++) {
	for (int tt=0; tt<NTDRS; tt++) {
	  for (int cc=0; cc<2; cc++) {
	    allaligned_temp &= aligned[jj][tt][cc];
	  }
	}
      }
      allaligned = allaligned_temp;
      //    printf("allaligned %d\n", allaligned);
      
      if (progname.Contains("FirstPass")) return 0;
    }
    
    if (!progname.Contains("NoLastStep")) {
      ret = SingleRun<Event, RH>(argc, argv, align_filename, gaincorrection_filename, indexalignment++, 2);//last alignment just to shift all the ladder of the same quantity to have <S0>=0 and <K0>=0 
      if (ret) return ret;
    }
    
    ret = SingleRun<Event, RH>(argc, argv, align_filename, gaincorrection_filename, indexalignment++, 0, true, true, false);//let's run again to have the plots with the final alignment, now adding chisq cut and residual with excluded point
    if (ret) return ret;
    
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("AGGIUNGERE UN REPORT ALLA FINE PER SAPERE QUELLI CHE NON HANNO CONVERGIUTO\n");
    printf("SE CI METTE TROPPO A CONVERGERE CHIAMARLO COMUNQUE COME NON CONVERGIUTO\n");
    printf("IN GENERALE INFATTI L'ALGORITMO CHE DECIDE PER LA NON CONVERGENZA E' MOLTO ROZZO\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("PROVARE AD AGGIUNGERE L'ALLINEAMENTO PER ROTAZIONI\n");
    printf("I PLOT CI SONO (PER LE ROTAZIONI SUL PIANO X-Y) E SONO DEI TGRAPH (FACILMENTE FITTABILI CON pol1)\n");
    printf("INFATTI:\n");
    printf("x = x'*Cos(alpha)-y'*Sin(alpha)\n");
    printf("y = y'*Cos(alpha)+x'*Sin(alpha)\n");
    printf("quindi:\n");
    printf("Dx = x-x' = x'*(Cos(alpha)-1)-y'*Sin(alpha)\n");
    printf("Dy = y-y' = y'*(Cos(alpha)-1)+x'*Sin(alpha)\n");
    printf("i termini a sinistra sono i residui. Derivando:\n");
    printf("dDx/dy = -Sin(alpha)");
    printf("quindi, ad esempio, fittando il residuo in X vs la posizione in Y con una pol1, il coefficiente angolare ci darà Sin(alpha)\n");
    printf("e così anche per gli altri\n");
    printf("dDx/dx = Cos(alpha)-1");
    printf("dDy/dx = Cos(alpha)-1");
    printf("dDy/dy = Sin(alpha)");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    
    return ret;
  }
  else if (progname.Contains("DryRun")) {

    if (progname.Contains("Residual")) {
      ret = SingleRun<Event, RH>(argc, argv, align_filename, gaincorrection_filename, 0, 0, false, false, false);//without the cut on chisq and without the residuals with the excluded point
    }
    else if (progname.Contains("Bruna")) {
      ret = SingleRun<Event, RH>(argc, argv, align_filename, gaincorrection_filename, 0, 1, false, false, false);//without the cut on chisq and without the residuals with the excluded point
    }
      
    return ret;
  }
  else if (progname.Contains("OnlyFinalRun")) {
    
    ret = SingleRun<Event, RH>(argc, argv, align_filename, gaincorrection_filename, 0, 0, true, true, false);//with the cut on chisq and with the residuals with the excluded point

    return ret;
  }

  return -9;
}

template <class Event, class RH>
int SingleRun(int argc, char* argv[],
	      TString align_filename, TString gaincorrection_filename,
	      int indexalignment,
	      int alignmeth, bool chisqcut, bool residualexcludedpoint, bool writealign){

  static constexpr int NJINF = Event::GetNJINF();
  static constexpr int NTDRS = Event::GetNTDRS();
  static constexpr int NVAS = Event::GetNVAS();
  static constexpr int NCHAVA = Event::GetNCHAVA();
  static constexpr int NADCS = Event::GetNADCS();

  using TS = TrackSelection<Event, RH>;
  TS* ts = new TS();
  using UT = Utilities<Event, RH>;
  UT* ut = new UT();
  
  TChain *chain = new TChain("t4");
     
  for (int ii=1; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  PRINTDEBUG;
  
  Event::ReadAlignment(align_filename.Data());
  Event::ReadGainCorrection(gaincorrection_filename.Data());
  
  PRINTDEBUG;
  
  Event *ev;
  Cluster *cl;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);
  
  ev = new Event();
  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;
  
  ut->GetRH(chain)->Print();
  _maxtdr = ut->GetRH(chain)->GetNTdrs();
  //  printf("%d\n", _maxladd);
  
  TFile* foutput = new TFile(Form("foutput_%d.root", indexalignment), "RECREATE");
  foutput->cd();
 
  TH1F* occupancy_all[NJINF*NTDRS];
  TH1F* occupancy_posS_all[NJINF*NTDRS];
  TH1F* occupancy_posK_all[NJINF*NTDRS];
  TH1F* occupancy[NJINF*NTDRS];
  TH1F* occupancy_posS[NJINF*NTDRS];
  TH1F* occupancy_posK[NJINF*NTDRS];
  TH2F* occupancy_posS_vs_posS[NJINF*NTDRS][NJINF*NTDRS];
  TH2F* occupancy_posK_vs_posK[NJINF*NTDRS][NJINF*NTDRS];
  TH1F* residual_S[NJINF*NTDRS];
  TH1F* residual_K[NJINF*NTDRS];
  TProfile* residualS_vs_posS[NJINF*NTDRS];
  TProfile* residualS_vs_posK[NJINF*NTDRS];
  TProfile* residualK_vs_posK[NJINF*NTDRS];
  TProfile* residualK_vs_posS[NJINF*NTDRS];
  TH1F* TrackS[NJINF*NTDRS];
  TH1F* TrackK[NJINF*NTDRS];
  TH1F* hcooreldiff_S[NJINF*NTDRS];
  TH1F* hcooreldiff_K[NJINF*NTDRS];

  int NSTRIPSS=Cluster::GetNChannels(0);
  int NSTRIPSK=Cluster::GetNChannels(1);
  for (int tt=0; tt<_maxtdr; tt++) {
    int jinfnum = 0;
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    int tdr0   = ut->GetRH(chain)->GetTdrNum(0);
    double pitchS = std::max(Cluster::GetPitch(jinfnum, tdrnum, 0), 0.01);
    double pitchK = std::max(Cluster::GetPitch(jinfnum, tdrnum, 1), 0.01);
    occupancy_all[tt] = new TH1F(
				 Form("occupancy_%02d_%02d_all", jinfnum, tdrnum),
				 Form("occupancy_%02d_%02d_all;Channel number;Occupancy", jinfnum, tdrnum),
				 1024, 0, 1024
				 );
    occupancy_posS_all[tt] = new TH1F(
				      Form("occupancy_posS_%02d_%02d_all", jinfnum, tdrnum),
				      Form("occupancy_posS_%02d_%02d_all;Position_{S} (mm);Occupancy", jinfnum, tdrnum),
				      2*NSTRIPSS, -NSTRIPSS*pitchS, NSTRIPSS*pitchS
				      );
    occupancy_posK_all[tt] = new TH1F(
				      Form("occupancy_posK_%02d_%02d_all", jinfnum, tdrnum),
				      Form("occupancy_posK_%02d_%02d_all;Position_{K} (mm);Occupancy", jinfnum, tdrnum),
				      2*NSTRIPSK, -NSTRIPSK*pitchK, NSTRIPSK*pitchK
				      );
    occupancy[tt] = new TH1F(
			     Form("occupancy_%02d_%02d", jinfnum, tdrnum),
			     Form("occupancy_%02d_%02d;Channel number;Occupancy", jinfnum, tdrnum),
			     1024, 0, 1024
			     );
    occupancy_posS[tt] = new TH1F(
				  Form("occupancy_posS_%02d_%02d", jinfnum, tdrnum),
				  Form("occupancy_posS_%02d_%02d;Position_{S} (mm);Occupancy", jinfnum, tdrnum),
				  2*NSTRIPSS, -NSTRIPSS*pitchS, NSTRIPSS*pitchS
				  );
    occupancy_posK[tt] = new TH1F(
				  Form("occupancy_posK_%02d_%02d", jinfnum, tdrnum),
				  Form("occupancy_posK_%02d_%02d;Position_{K} (mm);Occupancy", jinfnum, tdrnum),
				  2*NSTRIPSK, -NSTRIPSK*pitchK, NSTRIPSK*pitchK
				  );
    for (int tt2=0; tt2<_maxtdr; tt2++) {
      int jinfnum2 = 0;
      int tdrnum2 = ut->GetRH(chain)->GetTdrNum(tt2);
      double pitchS2 = std::max(Cluster::GetPitch(jinfnum2, tdrnum2, 0), 0.01);
      double pitchK2 = std::max(Cluster::GetPitch(jinfnum2, tdrnum2, 1), 0.01);
      occupancy_posS_vs_posS[tt][tt2] = new TH2F(
						 Form("occupancy_posS_%02d_%02d_vs_%02d_%02d", jinfnum, tdrnum, jinfnum2, tdrnum2),
						 Form("occupancy_posS_%02d_%02d_vs_%02d_%02d;Position_{S} (mm);Position_{S} (mm);Occupancy", jinfnum, tdrnum, jinfnum2, tdrnum2),
						 2*NSTRIPSS, -NSTRIPSS*pitchS, NSTRIPSS*pitchS,
						 2*NSTRIPSS, -NSTRIPSS*pitchS2, NSTRIPSS*pitchS2
						 );
      occupancy_posK_vs_posK[tt][tt2] = new TH2F(
						 Form("occupancy_posK_%02d_%02d_vs_%02d_%02d", jinfnum, tdrnum, jinfnum2, tdrnum2),
						 Form("occupancy_posK_%02d_%02d_vs_%02d_%02d;Position_{K} (mm);Position_{K} (mm);Occupancy", jinfnum, tdrnum, jinfnum2, tdrnum2),
						 2*NSTRIPSK, -NSTRIPSK*pitchK, NSTRIPSK*pitchK,
						 2*NSTRIPSK, -NSTRIPSK*pitchK2, NSTRIPSK*pitchK2
						 );
    }
    residual_S[tt] = new TH1F(Form("residual_S_0_%02d", tdrnum), Form("residual_S_0_%02d;Residual_{S} (mm);Entries", tdrnum), 
			      pow(2,15), -2000*Cluster::GetNominalResolution(0, tdrnum, 0), 2000*Cluster::GetNominalResolution(0, tdrnum, 0));
    residual_K[tt] = new TH1F(Form("residual_K_0_%02d", tdrnum), Form("residual_K_0_%02d;Residual_{K} (mm);Entries", tdrnum), 
			      pow(2,15), -2000*Cluster::GetNominalResolution(0, tdrnum, 1), 2000*Cluster::GetNominalResolution(0, tdrnum, 1));
    TrackS[tt] = new TH1F(Form("TrackS_0_%02d", tdrnum), Form("TrackS;X_{Z%02d} (mm);Entries", tdrnum), 1000, -100, 100);
    TrackK[tt] = new TH1F(Form("TrackK_0_%02d", tdrnum), Form("TrackK;Y_{Z%02d} (mm);Entries", tdrnum), 1000, -100, 100);
    residualS_vs_posS[tt] = new TProfile(Form("residualS_vs_posS_%02d", tdrnum), Form("residualS_vs_posS_%02d", tdrnum), 2*NSTRIPSS, -NSTRIPSS*pitchS, NSTRIPSS*pitchS);
    residualS_vs_posK[tt] = new TProfile(Form("residualS_vs_posK_%02d", tdrnum), Form("residualS_vs_posK_%02d", tdrnum), 2*NSTRIPSK, -NSTRIPSK*pitchK, NSTRIPSK*pitchK);
    residualK_vs_posK[tt] = new TProfile(Form("residualK_vs_posK_%02d", tdrnum), Form("residualK_vs_posK_%02d", tdrnum), 2*NSTRIPSK, -NSTRIPSK*pitchK, NSTRIPSK*pitchK);
    residualK_vs_posS[tt] = new TProfile(Form("residualK_vs_posS_%02d", tdrnum), Form("residualK_vs_posS_%02d", tdrnum), 2*NSTRIPSS, -NSTRIPSS*pitchS, NSTRIPSS*pitchS);
    hcooreldiff_S[tt] = new TH1F(Form("hcooreldiff_S_%02d", tdrnum), Form("RelDiffWRTprevplane;Pos_{S}[%d]-Pos_{S}[%d] (mm);Occupancy", tdrnum, tdr0),
				 pow(2,17), -12000*Cluster::GetNominalResolution(0, tdrnum, 0), 12000*Cluster::GetNominalResolution(0, tdrnum, 0));
    hcooreldiff_K[tt] = new TH1F(Form("hcooreldiff_K_%02d", tdrnum), Form("RelDiffWRTprevplane;Pos_{K}[%d]-Pos_{K}[%d] (mm);Occupancy", tdrnum, tdr0),
				 pow(2,17), -12000*Cluster::GetNominalResolution(0, tdrnum, 1), 12000*Cluster::GetNominalResolution(0, tdrnum, 1));
  }
  
  PRINTDEBUG;
  
  TH1F* nhits = new TH1F("nhits", "nhits;# of hits;Entries", 6, -0.5, 5.5);
  TH1F* nhitsx = new TH1F("nhitsx", "nhitsx;# of hits_{x};Entries", 6, -0.5, 5.5);
  TH1F* nhitsy = new TH1F("nhitsy", "nhitsy;# of hits_{y};Entries", 6, -0.5, 5.5);
  TH1F* chi = new TH1F("chi", "chi;log10(#chi^{2});Entries", 1000, -5, 10);
  TH1F* chiS = new TH1F("chiS", "chiS;log10(#chi^{2}_{S});Entries", 1000, -5, 10);
  TH1F* chiK = new TH1F("chiK", "chiK;log10(#chi^{2}_{K});Entries", 1000, -5, 10);
  TH1F* theta = new TH1F("theta", "theta;#theta (rad);Entries", 10000, -1.0, 1.0);
  TH1F* phi = new TH1F("phi", "phi;#phi (rad);Entries", 1000, -TMath::Pi(), TMath::Pi());
  TH2F* thetaphi = new TH2F("thetaphi", "thetaphi;#theta (rad);#phi (rad);Entries", 10000, -1.0, 1.0, 1000, -TMath::Pi(), TMath::Pi());
  TH1F* S0 = new TH1F("S0", "S0;S_{Z=0} (mm);Entries", 1000, -100, 100);
  TH1F* K0 = new TH1F("K0", "K0;K_{Z=0} (mm);Entries", 1000, -100, 100);
  TH2F* S0K0 = new TH2F("S0K0", "S0K0;S_{Z=0} (mm);K_{Z=0} (mm);Entries", 1000, -100, 100, 1000, -100, 100);

  TH1F* hclusSladd = new TH1F("hclusSladd", "hclusSladd;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusSladdtrack = new TH1F("hclusSladdtrack", "hclusSladdtrack;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusKladd = new TH1F("hclusKladd", "hclusKladd;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusKladdtrack = new TH1F("hclusKladdtrack", "hclusKladdtrack;Ladder;Clusters", 24, 0, 24);
  TH1F* hclus = new TH1F("hclus", "hclus;Clusters", 1000, 0, 1000);
  TH1F* hclusS = new TH1F("hclusS", "hclusS;ClustersS", 1000, 0, 1000);
  TH1F* hclusK = new TH1F("hclusK", "hclusK;ClustersK", 1000, 0, 1000);
  TH1F* hclusperladd = new TH1F("hclusperladd", "hclusperladd;Clusters per ladder", 1000, 0, 1000);
  TH1F* hclusSperladd = new TH1F("hclusSperladd", "hclusSperladd;ClustersS per ladder", 1000, 0, 1000);
  TH1F* hclusKperladd = new TH1F("hclusKperladd", "hclusKperladd;ClustersK per ladder", 1000, 0, 1000);

  PRINTDEBUG;

  int cleanevs=0;
  int preselevs=0;
  int goodtracks=0;
  int goldtracks=0;
  int goldStracks=0;
  int goldKtracks=0;

  int perc=0;
  
  for (int jj=0; jj<NJINF; jj++) {
    for (int i_tt=0; i_tt<ladderS_to_ignore[jj].size(); i_tt++) {
      int tt = ladderS_to_ignore[jj].at(i_tt);
      ts->ExcludeTDR(ev, jj, tt, 0);
    }
    for (int i_tt=0; i_tt<ladderK_to_ignore[jj].size(); i_tt++) {
      int tt = ladderK_to_ignore[jj].at(i_tt);
      ts->ExcludeTDR(ev, jj, tt, 1);
    }
  }
  
  int firstS=0;
  int firstK=0;
  for (int jj=0; jj<NJINF; jj++) {
    for(int tt=0; tt<_maxtdr; tt++) {
      int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
      if (!(std::find(ladderS_to_ignore[jj].begin(), ladderS_to_ignore[jj].end(), tdrnum) != ladderS_to_ignore[jj].end())) {
	firstS=tt;
	break;
      }
    }
    for(int tt=0; tt<_maxtdr; tt++) {
      int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
      if (!(std::find(ladderK_to_ignore[jj].begin(), ladderK_to_ignore[jj].end(), tdrnum) != ladderK_to_ignore[jj].end())) {
	firstK=tt;
	break;
      }
    }
  }
  printf("FirstS is %d (pos: %d)\n", ut->GetRH(chain)->GetTdrNum(firstS), firstS);
  printf("FirstK is %d (pos: %d)\n", ut->GetRH(chain)->GetTdrNum(firstK), firstK);
  //    sleep(10);
  for (int jj=0; jj<NJINF; jj++) {
    for(int tt=0; tt<_maxtdr; tt++) {
      int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
      hcooreldiff_S[tt]->GetXaxis()->SetTitle(Form("Pos_{S}[%d]-Pos_{S}[%d] (mm)", tdrnum, ut->GetRH(chain)->GetTdrNum(firstS)));
      hcooreldiff_K[tt]->GetXaxis()->SetTitle(Form("Pos_{K}[%d]-Pos_{K}[%d] (mm)", tdrnum, ut->GetRH(chain)->GetTdrNum(firstK)));
    }
  }
  
  //  for (int index_event=14; index_event<15; index_event++) {
  for (int index_event=0; index_event<entries; index_event++) {
    
    //    printf("----- new event %d (out of %d)\n", index_event, entries);

    Double_t pperc=100.0*((index_event+1.0)/entries);
    if (pperc>=perc) {
      printf("Processed %d out of %lld: %d%%\n", (index_event+1), entries, (int)(100.0*(index_event+1.0)/entries));
      perc+=10;
      printf("This run has %lld entries\n", entries);
      printf("    --> %d clean events\n", cleanevs);
      printf("    --> %d preselected events\n", preselevs);
      printf("    --> %d good tracks (%f%%)\n", goodtracks, 100.0*goodtracks/preselevs);
      printf("    --> %d goldS tracks (%f%%)\n", goldStracks, 100.0*goldStracks/preselevs);
      printf("    --> %d goldK tracks (%f%%)\n", goldKtracks, 100.0*goldKtracks/preselevs);
      printf("    --> %d gold tracks (%f%%)\n", goldtracks, 100.0*goldtracks/preselevs);
    }
    //    printf("----- new event %d\n", index_event);
    
    PRINTDEBUG;
    chain->GetEntry(index_event);
        
    PRINTDEBUG;

    int NClusTot = ev->GetNClusTot();
    //    printf("\t\tnclusters = %d\n", NClusTot);
    PRINTDEBUG;
    
    //at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    //and at most 50 (to avoid too much noise around and too much combinatorial)
    //at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    // ts->CleanEvent(ev, ut->GetRH(chain), 4, 50, 6, 6, 0, 0);

    bool cleanevent = ts->CleanEvent(ev, ut->GetRH(chain), 2, 9999, 9999, 9999, 9999, 9999);
    if (!cleanevent) continue;
    cleanevs++;//in this way this number is giving the "complete reasonable sample"

    bool preselevent = ts->CleanEvent(ev, ut->GetRH(chain), 4, 50, 3, 3, 0, 0);
    if (!preselevent) continue;
    preselevs++;

    PRINTDEBUG;
    
    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];

    PRINTDEBUG;

    //at least 3 points on S, and 3 points on K, not verbose
    bool trackfitok = ev->FindTrackAndFit(_npointsS, _npointsK, false);
    //    printf("%d\n", trackfitok);

    PRINTDEBUG;
    
    if (trackfitok) {
      //remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas on K
      //(but only if removing them still we'll have more or equal than 3 (2 if 'HigherCharge') hits on S and 3 (2 if 'HigherCharge') hits on K)
      //and perform the fit again
      //      ev->RefineTrack(6.0, 2, 6.0, 2);
    }
    /* we do not need to "recover" efficiency during alignment
    else {
      //let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      //let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be used also for ions)
      //and requiring an higher S/N for the cluster
      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);
    }
    */
    //    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetS0Track(), ev->GetK0Track());    
    if (trackfitok) {
      goodtracks++;
    }
    
    PRINTDEBUG;
    
    //    printf("S %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(0), ev->IsTDRInTrack(0, 0), ev->IsTDRInTrack(0, 4), ev->IsTDRInTrack(0, 8), ev->IsTDRInTrack(0, 12), ev->IsTDRInTrack(0, 14));
    //    printf("K %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(1), ev->IsTDRInTrack(1, 0), ev->IsTDRInTrack(1, 4), ev->IsTDRInTrack(1, 8), ev->IsTDRInTrack(1, 12), ev->IsTDRInTrack(1, 14));
    
    // //                              321098765432109876543210
    // if (ev->GetTrackHitPattern(0) <                100010001) continue;
    // if (ev->GetTrackHitPattern(1) <                100010001) continue;
    
    double logchi = log10(ev->GetChiTrack());
    double logchiS = log10(ev->GetChiTrackS());
    double logchiK = log10(ev->GetChiTrackK());
    //    printf("%d %f (%f)\n", indexalignment, logchi, fiftycent);
    //    if (chisqcut && logchi>_chisqcutvalue) continue;

    PRINTDEBUG;
    
    //    printf("Qui!\n");
    
    bool strackok = false;
    bool ktrackok = false;

    /*
    if (
	ev->IsTDRInTrack(0, 0) &&
	ev->IsTDRInTrack(0, 4) &&
	ev->IsTDRInTrack(0, 8) &&
	(ev->IsTDRInTrack(0, 12) || ev->IsTDRInTrack(0, 14)) ) strackok=true;

    if (
	ev->IsTDRInTrack(1, 0) &&
	ev->IsTDRInTrack(1, 4) &&
	ev->IsTDRInTrack(1, 8) &&
	(ev->IsTDRInTrack(1, 12) || ev->IsTDRInTrack(1, 14)) ) ktrackok=true;
    */
    int snhits=ev->GetNHitsSTrack();
    if (snhits>=3) {
      strackok=true;
      goldStracks++;
    }
    
    int knhits= ev->GetNHitsKTrack();
    if (knhits>=3) {
      ktrackok=true;
      goldKtracks++;
    }

    // printf("S -> %d %d\n", snhits, strackok);
    // printf("K -> %d %d\n", knhits, ktrackok);

    if (strackok & ktrackok) goldtracks++;

    if (trackfitok && !(chisqcut && logchi>_chisqcutvalue)) {
      nhits->Fill(ev->GetNHitsTrack());
      nhitsx->Fill(ev->GetNHitsSTrack());
      nhitsy->Fill(ev->GetNHitsKTrack());
      chi->Fill(log10(ev->GetChiTrack()));
      chiS->Fill(log10(ev->GetChiTrackS()));
      chiK->Fill(log10(ev->GetChiTrackK()));
      theta->Fill(ev->GetThetaTrack());
      phi->Fill(ev->GetPhiTrack());
      thetaphi->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
      S0->Fill(ev->GetS0Track());
      K0->Fill(ev->GetK0Track());
      S0K0->Fill(ev->GetS0Track(), ev->GetK0Track());
    }

    PRINTDEBUG;
    
    hclus->Fill(NClusTot);

    PRINTDEBUG;
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {

      PRINTDEBUG;
      cl = ev->GetCluster(index_cluster);

      PRINTDEBUG;
      
      int ladder = cl->ladder;
      int side=cl->side;
      int ladder_pos = ut->GetRH(chain)->FindPos(ladder);
      if (ladder<0 || ladder>=24 || ladder_pos<0 || ladder_pos>=24) {
	printf("Ladder %d --> %d. Side = %d\n", ladder, ladder_pos, side);
      }
      //printf("Ladder %d --> %d. Side = %d\n", ladder, ladder_pos, side);
      
      PRINTDEBUG;
      
      occupancy_all[ladder_pos]->Fill(cl->GetCoG());
      if (trackfitok && !(chisqcut && logchi>_chisqcutvalue)) {
	occupancy[ladder_pos]->Fill(cl->GetCoG());
      }
      
      PRINTDEBUG;
      
      if (side==0) {
	occupancy_posS_all[ladder_pos]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddS[ladder_pos].push_back(cl->GetAlignedPosition());
	//	if (cl->GetAlignedPosition()>NSTRIPSS*pitchS) printf("%d) %f\n", ladder_pos, cl->GetCoG());
      }
      else if (side==1) {
	occupancy_posK_all[ladder_pos]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddK[ladder_pos].push_back(cl->GetAlignedPosition());
      }
      else {
	printf("What? The cluster is of type %d...\n", side);
      }

      PRINTDEBUG;
      
      if (!trackfitok && (chisqcut && logchi>_chisqcutvalue)) continue
      
      PRINTDEBUG;
      
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      PRINTDEBUG;
      
      if (side==0) {
	if (strackok) {
	  occupancy_posS[ladder_pos]->Fill(cl->GetAlignedPosition());
	  v_cog_laddS[ladder_pos].push_back(cl->GetAlignedPosition());
	  hclusSladdtrack->Fill(ladder);
	}
      }
      else {
	if (ktrackok) {
	  occupancy_posK[ladder_pos]->Fill(cl->GetAlignedPosition());
	  v_cog_laddK[ladder_pos].push_back(cl->GetAlignedPosition());
	  hclusKladdtrack->Fill(ladder);
	}
      }
      
    }

    int NClusTot_check = 0;
    int NClusTotS = 0;
    int NClusTotK = 0;
    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      int tdrnum = ut->GetRH(chain)->GetTdrNum(ll);
      hclusSladd->Fill(tdrnum, v_cog_all_laddS[ll].size());
      NClusTot_check+=v_cog_all_laddS[ll].size();
      NClusTotS+=v_cog_all_laddS[ll].size();
      hclusKladd->Fill(tdrnum, v_cog_all_laddK[ll].size());
      NClusTot_check+=v_cog_all_laddK[ll].size();
      NClusTotK+=v_cog_all_laddK[ll].size();
    }
    if (NClusTot_check!=NClusTot) {
      printf("Number of clusters doesn't match: %d vs %d\n", NClusTot, NClusTot_check);
    }
    hclusS->Fill(NClusTotS);
    hclusK->Fill(NClusTotK);

    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      hclusperladd->Fill(v_cog_all_laddS[ll].size()+v_cog_all_laddK[ll].size());
      hclusSperladd->Fill(v_cog_all_laddS[ll].size());
      hclusKperladd->Fill(v_cog_all_laddK[ll].size());
    }
    
    //Relative Disalignment
    for(int tt=0; tt<_maxtdr; tt++) {
      for(int ihit=0; ihit<(int)v_cog_laddS[tt].size(); ihit++) {
	for(int jhit=0; jhit<(int)v_cog_laddS[firstS].size(); jhit++) {
	  hcooreldiff_S[tt]->Fill(v_cog_laddS[tt].at(ihit) - v_cog_laddS[firstS].at(jhit)); 
	}
      }
      for(int ihit=0; ihit<(int)v_cog_laddK[tt].size(); ihit++) {
	for(int jhit=0; jhit<(int)v_cog_laddK[firstK].size(); jhit++) {
	  hcooreldiff_K[tt]->Fill(v_cog_laddK[tt].at(ihit) - v_cog_laddK[firstK].at(jhit));
	}
      }
    }

    //Occupancy correlation
    for(int tt=0; tt<_maxtdr; tt++) {
      for(int tt2=0; tt2<_maxtdr; tt2++) {
	for(int ihit=0; ihit<(int)v_cog_laddS[tt].size(); ihit++) {
	  for(int jhit=0; jhit<(int)v_cog_laddS[tt2].size(); jhit++) {
	    occupancy_posS_vs_posS[tt][tt2]->Fill(v_cog_laddS[tt].at(ihit), v_cog_laddS[tt2].at(jhit));
	  }
	}
	for(int ihit=0; ihit<(int)v_cog_laddK[tt].size(); ihit++) {
	  for(int jhit=0; jhit<(int)v_cog_laddK[tt2].size(); jhit++) {
	    occupancy_posK_vs_posK[tt][tt2]->Fill(v_cog_laddK[tt].at(ihit), v_cog_laddK[tt2].at(jhit));
	  }
	}
      }
    }

    //FIXME: works just for one jinf
    std::vector<std::pair<int, std::pair<int, int> > > vec_hit = ev->GetHitVector();
    for (unsigned int tt=0; tt<vec_hit.size(); tt++) {
      
      int ladder = vec_hit.at(tt).first;
      int ladder_pos = ut->GetRH(chain)->FindPos(ladder);
      if (ladder<0 || ladder>=24 || ladder_pos<0 || ladder_pos>=24) {
	printf("Ladder %d --> %d\n", ladder, ladder_pos);
      }
      int index_cluster_S = vec_hit.at(tt).second.first;
      int index_cluster_K = vec_hit.at(tt).second.second;
      
      Cluster* cl_S = NULL;
      Cluster* cl_K = NULL;
      double posS = -9999.9;
      double resS = -9999.9;
      double posK = -9999.9;
      double resK = -9999.9;

      if (index_cluster_S>=0) cl_S = ev->GetCluster(index_cluster_S);
      if (index_cluster_K>=0) cl_K = ev->GetCluster(index_cluster_K);

      bool verbose=false;

      if (residualexcludedpoint) {
	if (cl_S) ev->ExcludeTDRFromTrack(0, ladder, 0, verbose);
	if (cl_K) ev->ExcludeTDRFromTrack(0, ladder, 1, verbose);
	ev->FindTrackAndFit(_npointsS-1, _npointsK-1);
	if (verbose) {
	  printf("S %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(0, 0)).to_string().c_str());
	  printf("K %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(1, 0)).to_string().c_str());
	}
      }
      
      if (cl_S) {
	posS = cl_S->GetAlignedPosition();
	resS = posS-ev->ExtrapolateTrack(cl_S->GetZPosition(), 0);
	residual_S[ladder_pos]->Fill(resS);
	residualS_vs_posS[ladder_pos]->Fill(posS, resS);
      }
      if (cl_K) {
	posK = cl_K->GetAlignedPosition();
	resK = posK-ev->ExtrapolateTrack(cl_K->GetZPosition(), 1);
	residual_K[ladder_pos]->Fill(resK);
	residualK_vs_posK[ladder_pos]->Fill(posK, resK);
      }
      if (cl_S && cl_K) {
	residualS_vs_posK[ladder_pos]->Fill(posK, resS);
	residualK_vs_posS[ladder_pos]->Fill(posS, resK);
      }

      if (residualexcludedpoint) {
	if (cl_S) ev->IncludeBackTDRFromTrack(0, ladder, 0, verbose);
	if (cl_K) ev->IncludeBackTDRFromTrack(0, ladder, 1, verbose);
	ev->FindTrackAndFit(_npointsS, _npointsK);
	if (verbose) {
	  printf("S %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(0, 0)).to_string().c_str());
	  printf("K %s\n", std::bitset<NTDRS>(ev->GetTrackHitPattern(1, 0)).to_string().c_str());
	}
      }
      
    }
    
    for (int tt=0; tt<_maxtdr; tt++) {
      int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
      //      printf("%f\n", ev->GetAlignPar(0, tdrnum, 2));
      TrackS[tt]->Fill(ev->ExtrapolateTrack(ev->GetAlignPar(0, tdrnum, 2), 0));
      TrackK[tt]->Fill(ev->ExtrapolateTrack(ev->GetAlignPar(0, tdrnum, 2), 1));
    }
    
    //    printf(" \n ");
    //    exit(1);
  }

  printf("This run has %lld entries\n", entries);
  printf("    --> %d clean events\n", cleanevs);
  printf("    --> %d preselected events\n", preselevs);
  printf("    --> %d good tracks (%f%%)\n", goodtracks, 100.0*goodtracks/preselevs);
  printf("    --> %d goldS tracks (%f%%)\n", goldStracks, 100.0*goldStracks/preselevs);
  printf("    --> %d goldK tracks (%f%%)\n", goldKtracks, 100.0*goldKtracks/preselevs);
  printf("    --> %d gold tracks (%f%%)\n", goldtracks, 100.0*goldtracks/preselevs);
    
  PRINTDEBUG;

  ofstream new_align_file;
  ofstream delta_align_file;
  
  if (writealign) {
    new_align_file.open(align_filename.Data(), ios_base::out);
    delta_align_file.open(Form("delta_%s", align_filename.Data()), ios_base::out);
    
    new_align_file<<"#JINF \t TDR \t S (mm) \t K (mm) \t Z (mm) \t MultiplicyFlip"<<endl;
    delta_align_file<<"#JINF \t TDR \t S (mm) \t K (mm) \t Z (mm) \t MultiplicyFlip"<<endl;
  }
  
  TCanvas c;

  int jj=0; //FIX ME: HARD-CODED "only one Jinf"
  for (int tt=0; tt<_maxtdr; tt++) {
    int tdrnum = ut->GetRH(chain)->GetTdrNum(tt);
    printf("%d) TDR %d\n", tt, tdrnum);
    
    double Smean = 0.0;
    double Kmean = 0.0;

    for (int kk=0; kk<3; kk++) {//0 is Residuals, 1 is Bruna's, 2 S0/K0 shift
      printf("Method=%d\n", kk);

      TH1* h2fitS = NULL;
      TH1* h2fitK = NULL;
      
      double safetyfactor=0.5;//half resolution
      if (kk==0) {
	h2fitS = residual_S[tt];
	h2fitK = residual_K[tt];
      }
      else if (kk==1) {
	safetyfactor=10.0;
	h2fitS = hcooreldiff_S[tt];
	h2fitK = hcooreldiff_K[tt];
      }
      else {
	h2fitS = S0;
	h2fitK = K0;
      }
      
      // printf("%d) %d %d\n", tt, h2fitS->GetNbinsX(), h2fitK->GetNbinsX());
      // printf("%d) %f %f\n", tt, h2fitS->GetEntries(), h2fitK->GetEntries());

      double peakcontentreq = 0.04;// the h->GetMaximum()/h->GetEntries() of a gaussian with 100 bins in [ -5σ, 5σ] is 0.04
      
      while (
	     h2fitS->GetMaximum()<peakcontentreq*h2fitS->GetEntries() &&
	     h2fitS->GetXaxis()->GetBinWidth(1)<safetyfactor*Cluster::GetNominalResolution(jj, tdrnum, 0)
	     )
	h2fitS->Rebin(2);
      while (
	     h2fitK->GetMaximum()<peakcontentreq*h2fitK->GetEntries() &&
	     h2fitK->GetXaxis()->GetBinWidth(1)<safetyfactor*Cluster::GetNominalResolution(jj, tdrnum, 1)
	     )
	h2fitK->Rebin(2);
      
      TF1* gauss = new TF1("gauss", "gaus", -100.0, 100.0);

      double _smean = 0.0;
      double _kmean = 0.0;
      float fit_limit[2]={-30.0, 30.0};

      for (int ss=0; ss<2; ss++) {
	
	TH1* h2fit = NULL;
	std::vector<int> *ladder_to_ignore = NULL;
	int first=-99;
	char _sidename[256];
	int _sidenum=-1;
	double _mean=0;
	if (ss==0) {
	  h2fit = h2fitS;
	  ladder_to_ignore = ladderS_to_ignore;
	  first=firstS;
	  sprintf(_sidename, "S");
	  _sidenum=0;
	}
	else if (ss==1) {
	  h2fit = h2fitK;
	  ladder_to_ignore = ladderK_to_ignore;
	  first=firstK;
	  sprintf(_sidename, "K");
	  _sidenum=1;
	}

	if (!(std::find(ladder_to_ignore[jj].begin(), ladder_to_ignore[jj].end(), tdrnum) != ladder_to_ignore[jj].end())) {
	  //cases
	  if (kk==2) {//last shift for S0 and K0
	    _smean = h2fitS->GetMean();
	    _kmean = h2fitK->GetMean();
	    //	printf("%d) %f %f\n", tt, _smean, _kmean);
	  }
	  else if (kk==1 && tt==first) {//in the Bruna's method there's no reason to shift the first
	    if (ss==0) Smean=0;
	    else if (ss==1) Kmean=0;
	  }
	  else {//kk==1 (but the reference ones) and kk==0

	    TSpectrum s(1);
	    s.Search(h2fit, Cluster::GetNominalResolution(jj, tdrnum, _sidenum), "newgoff");
	    Double_t *xpeaks = s.GetPositionX();
	    double width=1.0*Cluster::GetNominalResolution(jj, tdrnum, _sidenum);
	    if (kk==1) width*=100.0;
	    h2fit->GetXaxis()->SetRangeUser(xpeaks[0]-width, xpeaks[0]+width);
	    
	    gauss->SetParameter(0, h2fit->GetMaximum());
	    gauss->SetParameter(1, h2fit->GetMean());
	    gauss->SetParameter(2, h2fit->GetRMS());
	    if (kk==alignmeth) printf("Starting with: %f %f %f\n", h2fit->GetMaximum(), h2fit->GetMean(), h2fit->GetRMS());
	    
	    h2fit->Fit(gauss, "QI", "");
	    h2fit->Fit(gauss, "QI", "");
	    _mean = gauss->GetParameter(1);
	    if (kk==alignmeth) printf("fabs(%smean)=%f+-%f, sigma=%f, resolution=%f\n", _sidename, fabs(_mean), gauss->GetParError(1), gauss->GetParameter(2), Cluster::GetNominalResolution(jj, tdrnum, _sidenum));

	    h2fit->GetXaxis()->UnZoom();
	    
	    _mean = gauss->GetParameter(1);	  
	    fit_limit[0]=_mean-5.0*gauss->GetParameter(2);
	    fit_limit[1]=_mean+5.0*gauss->GetParameter(2);
	    
	    if (kk==alignmeth) printf("Fit in [%f,%f]\n", fit_limit[0], fit_limit[1]);
	    h2fit->Fit(gauss, "QI", "", fit_limit[0], fit_limit[1]);
	    h2fit->Fit(gauss, "QI", "", fit_limit[0], fit_limit[1]);

	    _mean = gauss->GetParameter(1);
	    fit_limit[0]=_mean-1.0*gauss->GetParameter(2);
	    fit_limit[1]=_mean+1.0*gauss->GetParameter(2);
	    
	    if (kk==alignmeth) printf("Fit in [%f,%f]\n", fit_limit[0], fit_limit[1]);
	    h2fit->Fit(gauss, "QI", "", fit_limit[0], fit_limit[1]);
	    h2fit->Fit(gauss, "QI", "", fit_limit[0], fit_limit[1]);
	    
	    double squeezefactor=1.0;
	    while(gauss->Eval(0)<0.7*h2fit->GetMaximum() && squeezefactor>0.5) {
	      _mean = gauss->GetParameter(1);
	      double _width=squeezefactor*gauss->GetParameter(2);
	      fit_limit[0]=_mean-_width;
	      fit_limit[1]=_mean+_width;
	      
	      if (kk==alignmeth) printf("Fit in [%f,%f]\n", fit_limit[0], fit_limit[1]);
	      h2fit->Fit(gauss, "QI", "", fit_limit[0], fit_limit[1]);
	      h2fit->Fit(gauss, "QI", "", fit_limit[0], fit_limit[1]);

	      if (squeezefactor*gauss->GetParameter(2)>=(_width-0.1*_width)) squeezefactor-=0.1;
	    }
	    
	    fit_limit[0]=_mean-15.0*gauss->GetParameter(2);
	    fit_limit[1]=_mean+15.0*gauss->GetParameter(2);
	    h2fit->GetXaxis()->SetRangeUser(fit_limit[0], fit_limit[1]);
	    
	    _mean = gauss->GetParameter(1);
	    if (kk==alignmeth) printf("fabs(%smean)=%f, error=%f, resolution=%f -> ", _sidename, fabs(_mean), gauss->GetParError(1), Cluster::GetNominalResolution(jj, tdrnum, _sidenum));
	    if (fabs(_mean)<3.0*gauss->GetParError(1) || fabs(_mean)<0.1*Cluster::GetNominalResolution(jj, tdrnum, _sidenum)) _smean=0.0;
	    if (kk==alignmeth) {
	      printf("%smean=%f\n", _sidename, _mean);
	      // cout<<" Fit between "<<fit_limit[0]
	      //     <<" and "<<	   fit_limit[1]
	      //     <<endl;
	      printf("%s align par = %f (old = %f + mean = %f)\n", _sidename, _mean+ev->GetAlignPar(jj, tdrnum, _sidenum), ev->GetAlignPar(jj, tdrnum, _sidenum), _mean);
	    }
	    
	  }
	}
	if (ss==0) _smean=_mean;
	else if (ss==1) _kmean=_mean;
      }
      
      if (alignmeth==kk) {//only for the choosen align method
	Smean = _smean;
	Kmean = _kmean;
      }
    }
    
    if (writealign) {
      new_align_file<<" 0 \t"<<tdrnum
		    <<" \t " <<Smean+ev->GetAlignPar(0, tdrnum, 0)
		    <<" \t " <<Kmean+ev->GetAlignPar(0, tdrnum, 1)
		    <<" \t " <<ev->GetAlignPar(0, tdrnum, 2)
		    <<" \t " <<ev->GetMultiplicityFlip(0, tdrnum)
		    <<endl;
      
      delta_align_file<<" 0 \t"<<tdrnum
		      <<" \t " <<Smean
		      <<" \t " <<Kmean
		      <<" \t " <<ev->GetAlignPar(0, tdrnum, 2)
		      <<" \t " <<ev->GetMultiplicityFlip(0, tdrnum)
		      <<endl;
      
      //    printf("%d) %f -> %f, %f -> %f\n", tdrnum, ev->GetAlignPar(0, tdrnum, 0), Smean+ev->GetAlignPar(0, tdrnum, 0), ev->GetAlignPar(0, tdrnum, 1), Kmean+ev->GetAlignPar(0, tdrnum, 1));
    }
    
    PRINTDEBUG;
  }

  if (writealign) {
    new_align_file.close();
    delta_align_file.close();
  }
 
  foutput->Write();
  foutput->Close();
  
  return 0;
}

template <class Event, class RH>
void ReadDeltaAlignment(TString filename){

  static constexpr int NJINF = Event::GetNJINF();
  static constexpr int NTDRS = Event::GetNTDRS();
  static constexpr int NVAS = Event::GetNVAS();
  static constexpr int NCHAVA = Event::GetNCHAVA();
  static constexpr int NADCS = Event::GetNADCS();
  
  //  printf("Reading delta-alignment from %s:\n", filename.Data());
  
  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      for (int cc=0; cc<3; cc++) {
	deltaalign[jj][tt][cc]=0.0;
      }
    }
  }
  
  int const dimline=255;
  char line[dimline];
  float dummy;
  int jinfnum=0;
  int tdrnum=0;
  
  FILE* ft = fopen(filename.Data(),"r");
  if(ft==NULL){ 
    printf("Error: cannot open %s \n", filename.Data());
    return;
  }
  else {
    while(1){
      if (fgets(line, dimline, ft)!=NULL) {
	if (*line == '#') continue; /* ignore comment line */
	else {
	  sscanf(line, "%d\t%d\t%f\t%f\t%f", &jinfnum, &tdrnum, &dummy, &dummy, &dummy);
	  if (jinfnum<NJINF && tdrnum<NTDRS) {
	    sscanf(line,"%d\t%d\t%f\t%f\t%f", &jinfnum, &tdrnum, &deltaalign[jinfnum][tdrnum][0], &deltaalign[jinfnum][tdrnum][1], &deltaalign[jinfnum][tdrnum][2]);
	    toprint[jinfnum][tdrnum]=true;
	    if (
		(std::find(ladderS_to_ignore[jinfnum].begin(), ladderS_to_ignore[jinfnum].end(), tdrnum) != ladderS_to_ignore[jinfnum].end()) &&
		(std::find(ladderK_to_ignore[jinfnum].begin(), ladderK_to_ignore[jinfnum].end(), tdrnum) != ladderK_to_ignore[jinfnum].end())
		) {
	      toprint[jinfnum][tdrnum]=false;
	    } 
	  }
	  else {
	    printf("Wrong JINF/TDR (%d, %d): maximum is (%d,%d)\n", jinfnum, tdrnum, NJINF, NTDRS);
	  }
	}
      }
      else {
	//	printf(" closing delta-alignment file \n");
	fclose(ft);
	break;
      }
    }
  }
  
  
  // for (int jj=0; jj<NJINF; jj++) {
  //   for (int tt=0; tt<NTDRS; tt++) {
  //     for (int cc=0; cc<3; cc++) {
  // 	if (cc==0) printf("JINF %02d TDR %02d)\t", jj, tt);
  // 	printf("%f\t", deltaalign[jj][tt][cc]);
  // 	if (cc==2) printf("\n");
  //     }
  //   }
  // }
  
  return;
}
