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
#include "Cluster.hh"
#include "Event.hh"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hh"
#include "Utilities.hh"
/* end */

std::vector<int> ladderS_to_ignore[NJINF] = {{2, 3, 6, 7, 10, 11, 14, 15, 18, 19}};
std::vector<int> ladderK_to_ignore[NJINF] = {{2, 3, 6, 7, 10, 11, 14, 15, 18, 19}};

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

float deltaalign[NJINF][NTDRS][3];
bool toprint[NJINF][NTDRS];

int SingleAlign(int argc, char* argv[], int indexalignment, int alignmeth=0, bool chisqcut=false, bool writealign=true);
void ReadDeltaAlignment(TString filename="delta_alignment.dat");

int main(int argc, char* argv[]) {
  
  if (argc<2) {
    printf("Usage:\n");
    printf("%s <first input root-filename> [second input root-filename] ...\n", argv[0]);
    return 1;
  }

  TString progname = argv[0];
  TString align_filename = "alignment.dat";

  int ret=0;
  
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
    
    ret = SingleAlign(argc, argv, indexalignment++, 1);//first alignment with the 'Bruna's method'
    if (ret) return ret;
    
    bool allaligned=false;
    while (!allaligned) {
      
      ret = SingleAlign(argc, argv, indexalignment);
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
      
      ReadDeltaAlignment(Form("delta_%s", align_filename.Data()));
      
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

      if (progname.Contains("FirstStepAlign")) return 0;
    }

    ret = SingleAlign(argc, argv, indexalignment++, 2);//last alignment just to shift all the ladder of the same quantity to have <S0>=0 and <K0>=0 
    if (ret) return ret;
    ret = SingleAlign(argc, argv, indexalignment++, 0, true, false);//let's run again to have the plots with the final alignment
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
  else if (progname.Contains("RunOnce")) {

    ret = SingleAlign(argc, argv, 0, 0, false, false);//without the cut on chisq

    return ret;
  }

  return -9;
}

int SingleAlign(int argc, char* argv[], int indexalignment, int alignmeth, bool chisqcut, bool writealign){
    
  TChain *chain = new TChain("t4");
     
  for (int ii=1; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString align_filename = "alignment.dat";

  PRINTDEBUG;
  
  Event::ReadAlignment(align_filename.Data());
  
  PRINTDEBUG;
  
  Event *ev;
  Cluster *cl;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);
  
  ev = new Event();
  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;
  
  GetRH(chain)->Print();
  _maxtdr = GetRH(chain)->GetNTdrsCmp();
  //  printf("%d\n", _maxladd);
  
  TFile* foutput = new TFile(Form("foutput_%d.root", indexalignment), "RECREATE");
  foutput->cd();
 
  TH1F* occupancy[NJINF*NTDRS];
  TH1F* occupancy_posS[NJINF*NTDRS];
  TH1F* occupancy_posK[NJINF*NTDRS];
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
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("occupancy_0_%02d;Channel number;Occupancy", GetRH(chain)->FindLadderNumCmp(tt)), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_0_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("occupancy_posS_0_%02d;Position_{S} (mm);Occupancy", GetRH(chain)->FindLadderNumCmp(tt)), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 0), NSTRIPSS*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_0_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("occupancy_posK_0_%02d;Position_{K} (mm);Occupancy", GetRH(chain)->FindLadderNumCmp(tt)), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 1), NSTRIPSK*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 1));
    residual_S[tt] = new TH1F(Form("residual_S_0_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("residual_S_0_%02d;Residual_{S} (mm);Entries", GetRH(chain)->FindLadderNumCmp(tt)), 
			      4000, -20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 0), 20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 0));
    residual_K[tt] = new TH1F(Form("residual_K_0_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("residual_K_0_%02d;Residual_{K} (mm);Entries", GetRH(chain)->FindLadderNumCmp(tt)), 
			      4000, -20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 1), 20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 1));
    TrackS[tt] = new TH1F(Form("TrackS_0_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("TrackS;X_{Z%02d} (mm);Entries", GetRH(chain)->FindLadderNumCmp(tt)), 1000, -100, 100);
    TrackK[tt] = new TH1F(Form("TrackK_0_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("TrackK;Y_{Z%02d} (mm);Entries", GetRH(chain)->FindLadderNumCmp(tt)), 1000, -100, 100);
    residualS_vs_posS[tt] = new TProfile(Form("residualS_vs_posS_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("residualS_vs_posS_%02d", GetRH(chain)->FindLadderNumCmp(tt)), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 0), NSTRIPSS*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 0));
    residualS_vs_posK[tt] = new TProfile(Form("residualS_vs_posK_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("residualS_vs_posK_%02d", GetRH(chain)->FindLadderNumCmp(tt)), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 1), NSTRIPSK*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 1));
    residualK_vs_posK[tt] = new TProfile(Form("residualK_vs_posK_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("residualK_vs_posK_%02d", GetRH(chain)->FindLadderNumCmp(tt)), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 1), NSTRIPSK*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 1));
    residualK_vs_posS[tt] = new TProfile(Form("residualK_vs_posS_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("residualK_vs_posS_%02d", GetRH(chain)->FindLadderNumCmp(tt)), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 0), NSTRIPSS*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 0));
    hcooreldiff_S[tt] = new TH1F(Form("hcooreldiff_S_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("RelDiffWRTprevplane;Pos_{S}[%d]-Pos_{S}[%d] (mm);Occupancy", GetRH(chain)->FindLadderNumCmp(tt), GetRH(chain)->FindLadderNumCmp(0)),
				 4000, -20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 0), 20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 0));
    hcooreldiff_K[tt] = new TH1F(Form("hcooreldiff_K_%02d", GetRH(chain)->FindLadderNumCmp(tt)), Form("RelDiffWRTprevplane;Pos_{K}[%d]-Pos_{K}[%d] (mm);Occupancy", GetRH(chain)->FindLadderNumCmp(tt), GetRH(chain)->FindLadderNumCmp(0)),
				 4000, -20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 1), 20*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 1));
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
      ExcludeTDR(ev, jj, tt, 0);
    }
    for (int i_tt=0; i_tt<ladderK_to_ignore[jj].size(); i_tt++) {
      int tt = ladderK_to_ignore[jj].at(i_tt);
      ExcludeTDR(ev, jj, tt, 1);
    }
  }
  
  //  for (int index_event=14; index_event<15; index_event++) {
  for (int index_event=0; index_event<entries; index_event++) {
    
    //    printf("----- new event %d (out of %d)\n", index_event, entries);

    Double_t pperc=100.0*((index_event+1.0)/entries);
    if (pperc>=perc) {
      printf("Processed %d out of %lld: %d%%\n", (index_event+1), entries, (int)(100.0*(index_event+1.0)/entries));
      perc+=10;
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
    // CleanEvent(ev, GetRH(chain), 4, 50, 6, 6, 0, 0);

    bool cleanevent = CleanEvent(ev, GetRH(chain), 2, 9999, 9999, 9999, 9999, 9999);
    if (!cleanevent) continue;
    cleanevs++;//in this way this number is giving the "complete reasonable sample"

    bool preselevent = CleanEvent(ev, GetRH(chain), 4, 100, 6, 6, 0, 0);
    if (!preselevent) continue;
    preselevs++;

    PRINTDEBUG;
    
    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];

    PRINTDEBUG;

    //at least 2 points on S, and 2 points on K, not verbose
    bool trackfitok = ev->FindTrackAndFit(2, 2, false);
    //    printf("%d\n", trackfitok);

    PRINTDEBUG;
    
    /* we do not need to "recover" efficiency during alignment
    if (trackfitok) {
      //remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas on K
      //(but only if removing them still we'll have more or equal than 3 (2 if 'HigherCharge') hits on S and 3 (2 if 'HigherCharge') hits on K)
      //and perform the fit again
      ev->RefineTrack(1.0, 2, 1.0, 2);
    }
    else {
      //let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      //let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be used also for ions)
      //and requiring an higher S/N for the cluster
      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);
    }
    */
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetS0Track(), ev->GetK0Track());    
    goodtracks++;
    
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
    //    if (chisqcut && logchi>2) continue;
    if (chisqcut && logchi>9) continue;

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

    PRINTDEBUG;
    
    hclus->Fill(NClusTot);

    PRINTDEBUG;
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {

      PRINTDEBUG;
      cl = ev->GetCluster(index_cluster);

      PRINTDEBUG;
      
      int ladder = cl->ladder;
      int side=cl->side;
      int ladder_pos = GetRH(chain)->FindPosCmp(ladder);
      if (ladder<0 || ladder>=24 || ladder_pos<0 || ladder_pos>=24) {
	printf("Ladder %d --> %d. Side = %d\n", ladder, ladder_pos, side);
      }
      //printf("Ladder %d --> %d. Side = %d\n", ladder, ladder_pos, side);
      
      PRINTDEBUG;
      
      occupancy[ladder_pos]->Fill(cl->GetCoG());
      
      PRINTDEBUG;
      
      if (side==0) {
	occupancy_posS[ladder_pos]->Fill(cl->GetAlignedPosition());
	//	if (cl->GetAlignedPosition()>NSTRIPSS*Cluster::GetPitch(0, GetRH(chain)->FindLadderNumCmp(tt), 0)) printf("%d) %f\n", ladder_pos, cl->GetCoG());
	v_cog_all_laddS[ladder_pos].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[ladder_pos]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddK[ladder_pos].push_back(cl->GetAlignedPosition());
      }

      PRINTDEBUG;
      
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      PRINTDEBUG;
      
      if (side==0) {
	if (strackok) {
	  residual_S[ladder_pos]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 0));
	  v_cog_laddS[ladder_pos].push_back(cl->GetAlignedPosition());
	  hclusSladdtrack->Fill(ladder);
	}
      }
      else {
	if (ktrackok) {
	  residual_K[ladder_pos]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 1));
	  v_cog_laddK[ladder_pos].push_back(cl->GetAlignedPosition());
	  hclusKladdtrack->Fill(ladder);
	}
      }
      
    }

    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      hclusSladd->Fill(GetRH(chain)->FindLadderNumCmp(ll), v_cog_all_laddS[ll].size());
      hclusKladd->Fill(GetRH(chain)->FindLadderNumCmp(ll), v_cog_all_laddK[ll].size());
    }

    int firstS=0;
    int firstK=0;
    for (int jj=0; jj<NJINF; jj++) {
      for(int tt=0; tt<_maxtdr; tt++) {
	if (!(std::find(ladderS_to_ignore[jj].begin(), ladderS_to_ignore[jj].end(), GetRH(chain)->FindLadderNumCmp(tt)) != ladderS_to_ignore[jj].end())) {
	  firstS=tt;
	  break;
	}
      }
      for(int tt=0; tt<_maxtdr; tt++) {
	if (!(std::find(ladderK_to_ignore[jj].begin(), ladderK_to_ignore[jj].end(), GetRH(chain)->FindLadderNumCmp(tt)) != ladderK_to_ignore[jj].end())) {
	  firstK=tt;
	  break;
	}
      }
    }
    for (int jj=0; jj<NJINF; jj++) {
      for(int tt=0; tt<_maxtdr; tt++) {
	hcooreldiff_S[jj]->GetXaxis()->SetTitle(Form("Pos_{S}[%d]-Pos_{S}[%d] (mm)", GetRH(chain)->FindLadderNumCmp(tt), GetRH(chain)->FindLadderNumCmp(firstS)));
	hcooreldiff_K[jj]->GetXaxis()->SetTitle(Form("Pos_{K}[%d]-Pos_{K}[%d] (mm)", GetRH(chain)->FindLadderNumCmp(tt), GetRH(chain)->FindLadderNumCmp(firstK)));
      }
    }
    
    //Relative Disalignment
    for(int tt=0; tt<_maxtdr; tt++) {
      for(int ihit=0; ihit<(int)v_cog_all_laddS[tt].size(); ihit++) {
	for(int jhit=0; jhit<(int)v_cog_all_laddS[firstS].size(); jhit++) {
	  hcooreldiff_S[tt]->Fill(v_cog_all_laddS[tt].at(ihit) - v_cog_all_laddS[firstS].at(jhit)); 
	}
      }
      for(int ihit=0; ihit<(int)v_cog_all_laddK[tt].size(); ihit++) {
	for(int jhit=0; jhit<(int)v_cog_all_laddK[firstK].size(); jhit++) {
	  hcooreldiff_K[tt]->Fill(v_cog_all_laddK[tt].at(ihit) - v_cog_all_laddK[firstK].at(jhit));
	}
      }
    }

    std::vector<std::pair<int, std::pair<int, int> > > vec_charge = ev->GetHitVector();
    for (unsigned int tt=0; tt<vec_charge.size(); tt++) {
      int ladder = vec_charge.at(tt).first;
      int ladder_pos = GetRH(chain)->FindPosCmp(ladder);
      if (ladder<0 || ladder>=24 || ladder_pos<0 || ladder_pos>=24) {
	printf("Ladder %d --> %d\n", ladder, ladder_pos);
      }
      int index_cluster_S = vec_charge.at(tt).second.first;
      int index_cluster_K = vec_charge.at(tt).second.second;
      Cluster* cl_S = NULL;
      Cluster* cl_K = NULL;
      double posS = -9999.9;
      double resS = -9999.9;
      double posK = -9999.9;
      double resK = -9999.9;
      if (index_cluster_S>=0) {
	cl_S = ev->GetCluster(index_cluster_S);
	posS = cl_S->GetAlignedPosition();
	resS = posS-ev->ExtrapolateTrack(cl_S->GetZPosition(), 0);
	residualS_vs_posS[ladder_pos]->Fill(posS, resS);
      }
      if (index_cluster_K>=0) {
	cl_K = ev->GetCluster(index_cluster_K);
	posK = cl_K->GetAlignedPosition();
	resK = posK-ev->ExtrapolateTrack(cl_K->GetZPosition(), 1);
	residualK_vs_posK[ladder_pos]->Fill(posK, resK);
      }
      if (index_cluster_S>=0 && index_cluster_K>=0) {
	residualS_vs_posK[ladder_pos]->Fill(posK, resS);
	residualK_vs_posS[ladder_pos]->Fill(posS, resK);
      }
    }
    
    for (int tt=0; tt<_maxtdr; tt++) {
      
      //      printf("%f\n", ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 2));
      TrackS[tt]->Fill(ev->ExtrapolateTrack(ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 2), 0));
      TrackK[tt]->Fill(ev->ExtrapolateTrack(ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 2), 1));
    }
    
    //    printf(" \n ");
    //    exit(1);
  }

  printf("This run has %lld entries\n", entries);
  printf("    --> %d clean events\n", cleanevs);
  printf("    --> %d preselected events\n", preselevs);
  printf("    --> %d good tracks\n", goodtracks);
  printf("    --> %d goldS tracks\n", goldStracks);
  printf("    --> %d goldK tracks\n", goldKtracks);
  printf("    --> %d gold tracks\n", goldtracks);
    
  PRINTDEBUG;

  ofstream new_align_file;
  ofstream delta_align_file;
  
  if (writealign) {
    new_align_file.open(align_filename.Data(), ios_base::out);
    delta_align_file.open(Form("delta_%s", align_filename.Data()), ios_base::out);
    
    new_align_file<<"#JINF \t TDR \t S (mm) \t K (mm) \t Z (mm) \t MultiplicyFlip"<<endl;
    delta_align_file<<"#JINF \t TDR \t S (mm) \t K (mm) \t Z (mm) \t MultiplicyFlip"<<endl;
  }
  
  double Smean = 0.0;
  double Kmean = 0.0;
  
  float fit_limit[2]={-30.0, 30.0};
  
  TH1* h2fitS = NULL;
  TH1* h2fitK = NULL;

  TCanvas c;

  int jj=0; //FIX ME: HARD-CODED "only one Jinf"
  for (int tt=0; tt<_maxtdr; tt++) {

    for (int kk=0; kk<3; kk++) {//0 is Residuals, 1 is Bruna's, 2 S0/K0 shift

      if (kk==0) {
	h2fitS = residual_S[tt];
	h2fitK = residual_K[tt];
      }
      else if (kk==1) {
	h2fitS = hcooreldiff_S[tt];
	h2fitK = hcooreldiff_K[tt];
      }
      else {
	h2fitS = S0;
	h2fitK = K0;
      }
      
      // printf("%d) %d %d\n", tt, h2fitS->GetNbinsX(), h2fitK->GetNbinsX());
      // printf("%d) %f %f\n", tt, h2fitS->GetEntries(), h2fitK->GetEntries());
      
      if (h2fitS->GetEntries()<1500) h2fitS->Rebin(2);
      else if (h2fitS->GetEntries()<500) h2fitS->Rebin(64);
      if (h2fitK->GetEntries()<1500) h2fitS->Rebin(2);
      else if (h2fitK->GetEntries()<500) h2fitS->Rebin(64);

      TF1* gauss = new TF1("gauss", "gaus", -100.0, 100.0);

      double _smean = 0.0;
      double _kmean = 0.0;

      if (kk==2) {
	_smean = h2fitS->GetMean();
	_kmean = h2fitK->GetMean();
	//	printf("%d) %f %f\n", tt, _smean, _kmean);
      }
      else {
	//----------

	if (!(std::find(ladderS_to_ignore[jj].begin(), ladderS_to_ignore[jj].end(), GetRH(chain)->FindLadderNumCmp(tt)) != ladderS_to_ignore[jj].end())) {
	  
	  _smean = h2fitS->GetBinCenter(h2fitS->GetMaximumBin());
	  // fit_limit[0]=_smean-0.3;
	  // fit_limit[1]=_smean+0.3;
	  fit_limit[0]=_smean-1.0;
	  fit_limit[1]=_smean+1.0;

	  
	  h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  
	  fit_limit[0]=gauss->GetParameter(1)-1.0*gauss->GetParameter(2);
	  fit_limit[1]=gauss->GetParameter(1)+1.0*gauss->GetParameter(2);
	  
	  h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  
	  _smean = gauss->GetParameter(1);
	  if (fabs(_smean)<3.0*gauss->GetParError(1) || fabs(_smean)<0.1*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 0)) _smean=0.0;
	  // cout<<" Fit between "<<fit_limit[0]
	  //     <<" and "<<	   fit_limit[1]
	  //     <<endl;
	  // printf("S align par = %f (old = %f)\n", _smean+ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 0), ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 0));

	}
	//----------
	if (!(std::find(ladderK_to_ignore[jj].begin(), ladderK_to_ignore[jj].end(), GetRH(chain)->FindLadderNumCmp(tt)) != ladderK_to_ignore[jj].end())) {
	  
	  _kmean = h2fitK->GetBinCenter(h2fitK->GetMaximumBin());
	  // fit_limit[0]=_kmean-0.3;
	  // fit_limit[1]=_kmean+0.3;
	  fit_limit[0]=_kmean-1.0;
	  fit_limit[1]=_kmean+1.0;

	  
	  h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  
	  fit_limit[0]=gauss->GetParameter(1)-1.0*gauss->GetParameter(2);
	  fit_limit[1]=gauss->GetParameter(1)+1.0*gauss->GetParameter(2);
	  
	  h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	  
	  _kmean = gauss->GetParameter(1);
	  if (fabs(_kmean)<3.0*gauss->GetParError(1) || fabs(_smean)<0.1*Cluster::GetNominalResolution(0, GetRH(chain)->FindLadderNumCmp(tt), 1)) _kmean=0.0;
	  // cout<<" Fit between "<<fit_limit[0]
	  //     <<" and "<<	   fit_limit[1]
	  //     <<endl;    
	  // printf("K align par = %f (old = %f)\n", _kmean+ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 1), ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 1));
	  
	}

      }
      
      if (alignmeth==kk) {//only for the choosen align method
	Smean = _smean;
	Kmean = _kmean;
	if (alignmeth==1 && kk==1) {//in the Bruna's method has no reason to shift the first layer...
	  if (tt==0) {
	    Smean = 0;
	    Kmean = 0;
	  }
	}
      }
      
    }

    if (writealign) {
      new_align_file<<" 0 \t"<<GetRH(chain)->FindLadderNumCmp(tt)
		    <<" \t " <<Smean+ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 0)
		    <<" \t " <<Kmean+ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 1)
		    <<" \t " <<ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 2)
		    <<" \t " <<ev->GetMultiplicityFlip(0, GetRH(chain)->FindLadderNumCmp(tt))
		    <<endl;
      
      delta_align_file<<" 0 \t"<<GetRH(chain)->FindLadderNumCmp(tt)
		      <<" \t " <<Smean
		      <<" \t " <<Kmean
		      <<" \t " <<ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 2)
		      <<" \t " <<ev->GetMultiplicityFlip(0, GetRH(chain)->FindLadderNumCmp(tt))
		      <<endl;
      
      //    printf("%d) %f -> %f, %f -> %f\n", GetRH(chain)->FindLadderNumCmp(tt), ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 0), Smean+ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 0), ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 1), Kmean+ev->GetAlignPar(0, GetRH(chain)->FindLadderNumCmp(tt), 1));
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

void ReadDeltaAlignment(TString filename){
  
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
