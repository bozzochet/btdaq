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

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

/* from the CommonTool dir */
#include "TrackSelection.hh"
/* end */

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

float deltaalign[NJINF][NTDRS][3];

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
	  aligned[jj][tt][cc]=0.0;
	  deltaalign[jj][tt][cc]=0.0;
	}
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
	    if (
		//		!aligned[jj][tt][cc] &&
		(fabs(deltaalign[jj][tt][cc])<0.001)
		) {
	      if (cc==0) printf("S ");
	      else if (cc==1) printf("K ");
	      printf("JINF %02d TDR %02d) OK!\n", jj, tt);
	      aligned[jj][tt][cc]=true;
	    }
	    else {
	      if (cc==0) printf("S ");
	      else if (cc==1) printf("K ");
	      printf("JINF %02d TDR %02d) KO\n", jj, tt);
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
	      if (cc==0) printf("S ");
	      else if (cc==1) printf("K ");
	      printf("JINF %02d TDR %02d) alignment didn't converge...\n", jj, tt);
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

    ret = SingleAlign(argc, argv, indexalignment++, 2);//last alignment just to shift all the ladder of the same quantity to have <X0>=0 and <Y0>=0 
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
  RHClass *rh;
  
  Long64_t entries = chain->GetEntries();
  printf("This run has %lld entries\n", entries);
  
  ev = new Event();
  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;
  
  //  chain->GetTree()->GetUserInfo()->Print();
  rh = (RHClass*)chain->GetTree()->GetUserInfo()->At(0);
  //  printf("%p\n", rh);
  if (rh) {
    rh->Print();
    _maxtdr = rh->ntdrCmp;
  }
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
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_0_%02d;Channel number;Occupancy", rh->tdrCmpMap[tt]), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_posS_0_%02d;Position_{S} (mm);Occupancy", rh->tdrCmpMap[tt]), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_posK_0_%02d;Position_{K} (mm);Occupancy", rh->tdrCmpMap[tt]), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(1), NSTRIPSK*Cluster::GetPitch(1));
    residual_S[tt] = new TH1F(Form("residual_S_0_%02d", rh->tdrCmpMap[tt]), Form("residual_S_0_%02d;Residual_{S} (mm);Entries", rh->tdrCmpMap[tt]), 
			      40*NSTRIPSS, -20*float(NSTRIPSS)/100.*Cluster::GetPitch(0), 20*float(NSTRIPSS)/100.*Cluster::GetPitch(0));
    residual_K[tt] = new TH1F(Form("residual_K_0_%02d", rh->tdrCmpMap[tt]), Form("residual_K_0_%02d;Residual_{K} (mm);Entries", rh->tdrCmpMap[tt]), 
			      40*NSTRIPSK, -20*float(NSTRIPSK)/100.*Cluster::GetPitch(1), 20*float(NSTRIPSK)/100.*Cluster::GetPitch(1));
    TrackS[tt] = new TH1F(Form("TrackS_0_%02d", rh->tdrCmpMap[tt]), Form("TrackS;X_{Z%02d} (mm);Entries", rh->tdrCmpMap[tt]), 1000, -100, 100);
    TrackK[tt] = new TH1F(Form("TrackK_0_%02d", rh->tdrCmpMap[tt]), Form("TrackK;Y_{Z%02d} (mm);Entries", rh->tdrCmpMap[tt]), 1000, -100, 100);
    residualS_vs_posS[tt] = new TProfile(Form("residualS_vs_posS_%02d", rh->tdrCmpMap[tt]), Form("residualS_vs_posS_%02d", rh->tdrCmpMap[tt]), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    residualS_vs_posK[tt] = new TProfile(Form("residualS_vs_posK_%02d", rh->tdrCmpMap[tt]), Form("residualS_vs_posK_%02d", rh->tdrCmpMap[tt]), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(1), NSTRIPSK*Cluster::GetPitch(1));
    residualK_vs_posK[tt] = new TProfile(Form("residualK_vs_posK_%02d", rh->tdrCmpMap[tt]), Form("residualK_vs_posK_%02d", rh->tdrCmpMap[tt]), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(1), NSTRIPSK*Cluster::GetPitch(1));
    residualK_vs_posS[tt] = new TProfile(Form("residualK_vs_posS_%02d", rh->tdrCmpMap[tt]), Form("residualK_vs_posS_%02d", rh->tdrCmpMap[tt]), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    hcooreldiff_S[tt] = new TH1F(Form("hcooreldiff_S_%02d", rh->tdrCmpMap[tt]), Form("RelDiffWRTprevplane;Pos_{S}[%d]-Pos_{S}[%d] (mm);Occupancy", rh->tdrCmpMap[tt], rh->tdrCmpMap[0]), 30*2*NSTRIPSS, -2.0*float(NSTRIPSS)*Cluster::GetPitch(0), 2.0*float(NSTRIPSS)*Cluster::GetPitch(0));
    hcooreldiff_K[tt] = new TH1F(Form("hcooreldiff_K_%02d", rh->tdrCmpMap[tt]), Form("RelDiffWRTprevplane;Pos_{K}[%d]-Pos_{K}[%d] (mm);Occupancy", rh->tdrCmpMap[tt], rh->tdrCmpMap[0]), 30*40*NSTRIPSK, -2.0*float(NSTRIPSK)*Cluster::GetPitch(1), 2.0*float(NSTRIPSK)*Cluster::GetPitch(1));
  }
  
  PRINTDEBUG;

  TH1F* nhits = new TH1F("nhits", "nhits;# of hits;Entries", 6, -0.5, 5.5);
  TH1F* nhitsx = new TH1F("nhitsx", "nhitsx;# of hits_{x};Entries", 6, -0.5, 5.5);
  TH1F* nhitsy = new TH1F("nhitsy", "nhitsy;# of hits_{y};Entries", 6, -0.5, 5.5);
  TH1F* chi = new TH1F("chi", "chi;log10(#chi^{2});Entries", 1000, -5, 10);
  TH1F* chix = new TH1F("chix", "chix;log10(#chi^{2}_{x});Entries", 1000, -5, 10);
  TH1F* chiy = new TH1F("chiy", "chiy;log10(#chi^{2}_{y});Entries", 1000, -5, 10);
  TH1F* theta = new TH1F("theta", "theta;#theta (rad);Entries", 10000, -1.0, 1.0);
  TH1F* phi = new TH1F("phi", "phi;#phi (rad);Entries", 1000, -TMath::Pi(), TMath::Pi());
  TH2F* thetaphi = new TH2F("thetaphi", "thetaphi;#theta (rad);#phi (rad);Entries", 10000, -1.0, 1.0, 1000, -TMath::Pi(), TMath::Pi());
  TH1F* X0 = new TH1F("X0", "X0;X_{Z=0} (mm);Entries", 1000, -100, 100);
  TH1F* Y0 = new TH1F("Y0", "Y0;Y_{Z=0} (mm);Entries", 1000, -100, 100);
  TH2F* X0Y0 = new TH2F("X0Y0", "X0Y0;X_{Z=0} (mm);Y_{Z=0} (mm);Entries", 1000, -100, 100, 1000, -100, 100);

  TH1F* hclusSladd = new TH1F("hclusSladd", "hclusSladd;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusSladdtrack = new TH1F("hclusSladdtrack", "hclusSladdtrack;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusKladd = new TH1F("hclusKladd", "hclusKladd;Ladder;Clusters", 24, 0, 24);
  TH1F* hclusKladdtrack = new TH1F("hclusKladdtrack", "hclusKladdtrack;Ladder;Clusters", 24, 0, 24);
  TH1F* hclus = new TH1F("hclus", "hclus;Clusters", 1000, 0, 1000);

  PRINTDEBUG;

  int cleanevs=0;
  int goodtracks=0;
  
  //  for (int index_event=14; index_event<15; index_event++) {
  for (int index_event=0; index_event<entries; index_event++) {
    //    printf("----- new event %d\n", index_event);
    if (rh->ntdrCmp>5) {
      printf("0) Troppi cluster...\n");
    }
    PRINTDEBUG;
    chain->GetEntry(index_event);

    PRINTDEBUG;
    if (rh->ntdrCmp>5) {
      printf("1) Troppi cluster...\n");
    }

    int NClusTot = ev->GetNClusTot();
    //    printf("\t\tnclusters = %d\n", NClusTot);
    PRINTDEBUG;

    if (rh->ntdrCmp>5) {
      printf("2) Troppi cluster...\n");
    }
    
    //at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    //and at most 50 (to avoid too much noise around and too much combinatorial)
    //at most 6 clusters per ladder (per side) + 0 additional clusters in total (per side)
    bool cleanevent = CleanEvent(ev, rh, 4, 50, 6, 6, 0, 0);
    if (!cleanevent) continue;
    cleanevs++;
    
    if (rh->ntdrCmp>5) {
      printf("3) Troppi cluster...\n");
      sleep(300000);
    }
    
    PRINTDEBUG;
    
    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];

    PRINTDEBUG;

    //at least 3 points on S, and 3 points on K, not verbose
    bool trackfitok = ev->FindTrackAndFit(3, 3, false);
    //    printf("%d\n", trackfitok);
    if (trackfitok) {
      //remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas on K
      //(but only if removing them still we'll have more or equal than 3 (2 if 'HigherCharge') hits on S and 3 (2 if 'HigherCharge') hits on K)
      //and perform the fit again
      ev->RefineTrack(6.0, 2, 6.0, 2);
    }
    /* better to waste these 2 hits events...
    else {
      //let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      //let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be used also for ions)
      //and requiring an higher S/N for the cluster
      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);
    }
    */
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetX0Track(), ev->GetY0Track());    
    goodtracks++;
    
    PRINTDEBUG;
    
    //    printf("S %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(0), ev->IsTDRInTrack(0, 0), ev->IsTDRInTrack(0, 4), ev->IsTDRInTrack(0, 8), ev->IsTDRInTrack(0, 12), ev->IsTDRInTrack(0, 14));
    //    printf("K %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(1), ev->IsTDRInTrack(1, 0), ev->IsTDRInTrack(1, 4), ev->IsTDRInTrack(1, 8), ev->IsTDRInTrack(1, 12), ev->IsTDRInTrack(1, 14));
    
    // //                              321098765432109876543210
    // if (ev->GetTrackHitPattern(0) <                100010001) continue;
    // if (ev->GetTrackHitPattern(1) <                100010001) continue;
    
    double logchi = log10(ev->GetChiTrack());
    double logchix = log10(ev->GetChiTrackX());
    double logchiy = log10(ev->GetChiTrackY());
    //    printf("%d %f (%f)\n", indexalignment, logchi, fiftycent);
    if (chisqcut && logchi>2) continue;

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
    strackok=true;
    ktrackok=true;

    nhits->Fill(ev->GetNHitsTrack());
    nhitsx->Fill(ev->GetNHitsXTrack());
    nhitsy->Fill(ev->GetNHitsYTrack());
    chi->Fill(log10(ev->GetChiTrack()));
    chix->Fill(log10(ev->GetChiTrackX()));
    chiy->Fill(log10(ev->GetChiTrackY()));
    theta->Fill(ev->GetThetaTrack());
    phi->Fill(ev->GetPhiTrack());
    thetaphi->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
    X0->Fill(ev->GetX0Track());
    Y0->Fill(ev->GetY0Track());
    X0Y0->Fill(ev->GetX0Track(), ev->GetY0Track());

    PRINTDEBUG;
    
    hclus->Fill(NClusTot);

    PRINTDEBUG;
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {

      PRINTDEBUG;
      cl = ev->GetCluster(index_cluster);

      PRINTDEBUG;
      
      int ladder = cl->ladder;
      //printf("ladder %d --> %d\n", ladder, rh->FindPos(ladder));
      PRINTDEBUG;
      
      occupancy[rh->FindPos(ladder)]->Fill(cl->GetCoG());
      PRINTDEBUG;
      
      int side=cl->side;

      PRINTDEBUG;
      
      if (side==0) {
	occupancy_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	//	if (cl->GetAlignedPosition()>NSTRIPSS*Cluster::GetPitch(0)) printf("%d) %f\n", rh->FindPos(ladder), cl->GetCoG());
	v_cog_all_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }

      PRINTDEBUG;
      
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      PRINTDEBUG;
      
      if (side==0) {
	if (strackok) {
	  residual_S[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 0));
	  v_cog_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusSladdtrack->Fill(ladder);
	}
      }
      else {
	if (ktrackok) {
	  residual_K[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 1));
	  v_cog_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusKladdtrack->Fill(ladder);
	}
      }
      
    }

    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      hclusSladd->Fill(rh->tdrCmpMap[ll], v_cog_all_laddS[ll].size());
      hclusKladd->Fill(rh->tdrCmpMap[ll], v_cog_all_laddK[ll].size());
    }

    //Relative Disalignment
    for(int jj=0; jj<_maxtdr; jj++) {
      for(int ihit=0; ihit<(int)v_cog_all_laddS[jj].size(); ihit++) {
	for(int jhit=0; jhit<(int)v_cog_all_laddS[0].size(); jhit++) {
	  hcooreldiff_S[jj]->Fill(v_cog_all_laddS[jj].at(ihit) - v_cog_all_laddS[0].at(jhit)); 
	}
      }
      for(int ihit=0; ihit<(int)v_cog_all_laddK[jj].size(); ihit++) {
	for(int jhit=0; jhit<(int)v_cog_all_laddK[0].size(); jhit++) {
	  hcooreldiff_K[jj]->Fill(v_cog_all_laddK[jj].at(ihit) - v_cog_all_laddK[0].at(jhit));
	}
      }
    }

    std::vector<std::pair<int, std::pair<int, int> > > vec_charge = ev->GetHitVector();
    for (unsigned int tt=0; tt<vec_charge.size(); tt++) {
      int ladder = vec_charge.at(tt).first;
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
	residualS_vs_posS[rh->FindPos(ladder)]->Fill(posS, resS);
      }
      if (index_cluster_K>=0) {
	cl_K = ev->GetCluster(index_cluster_K);
	posK = cl_K->GetAlignedPosition();
	resK = posK-ev->ExtrapolateTrack(cl_K->GetZPosition(), 1);
	residualK_vs_posK[rh->FindPos(ladder)]->Fill(posK, resK);
      }
      if (index_cluster_S>=0 && index_cluster_K>=0) {
	residualS_vs_posK[rh->FindPos(ladder)]->Fill(posK, resS);
	residualK_vs_posS[rh->FindPos(ladder)]->Fill(posS, resK);
      }
    }
    
    for (int tt=0; tt<_maxtdr; tt++) {
      //      printf("%f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2));
      TrackS[tt]->Fill(ev->ExtrapolateTrack(ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2), 0));
      TrackK[tt]->Fill(ev->ExtrapolateTrack(ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2), 1));
    }
    
    //    printf(" \n ");
    //    exit(1);
  }

  printf("This run has %lld entries\n", entries);
  printf("    --> %d clean events\n", cleanevs);
  printf("    --> %d good tracks\n", goodtracks);
    
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
  
  for (int tt=0; tt<_maxtdr; tt++) {

    for (int kk=0; kk<3; kk++) {//0 is Residuals, 1 is Bruna's, 2 X0/Y0 shift

      if (kk==0) {
	h2fitS = residual_S[tt];
	h2fitK = residual_K[tt];
      }
      else if (kk==1) {
	h2fitS = hcooreldiff_S[tt];
	h2fitK = hcooreldiff_K[tt];
      }
      else {
	h2fitS = X0;
	h2fitK = Y0;
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
	
	_smean = h2fitS->GetBinCenter(h2fitS->GetMaximumBin());
	fit_limit[0]=_smean-0.3;
	fit_limit[1]=_smean+0.3;
	
	h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	
	fit_limit[0]=gauss->GetParameter(1)-1.0*gauss->GetParameter(2);
	fit_limit[1]=gauss->GetParameter(1)+1.0*gauss->GetParameter(2);
	
	h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	h2fitS->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	
	_smean = gauss->GetParameter(1);
	if (fabs(_smean)<3.0*gauss->GetParError(1) || fabs(_smean)<0.1*Cluster::GetNominalResolution(0)) _smean=0.0;
	// cout<<" Fit between "<<fit_limit[0]
	//     <<" and "<<	   fit_limit[1]
	//     <<endl;
	// printf("S align par = %f (old = %f)\n", _smean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0), ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0));
	
	//----------
	
	_kmean = h2fitK->GetBinCenter(h2fitK->GetMaximumBin());
	fit_limit[0]=_kmean-0.3;
	fit_limit[1]=_kmean+0.3;
	
	h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	
	fit_limit[0]=gauss->GetParameter(1)-1.0*gauss->GetParameter(2);
	fit_limit[1]=gauss->GetParameter(1)+1.0*gauss->GetParameter(2);
	
	h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	h2fitK->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
	
	_kmean = gauss->GetParameter(1);
	if (fabs(_kmean)<3.0*gauss->GetParError(1) || fabs(_smean)<0.1*Cluster::GetNominalResolution(1)) _kmean=0.0;
	// cout<<" Fit between "<<fit_limit[0]
	//     <<" and "<<	   fit_limit[1]
	//     <<endl;    
	// printf("K align par = %f (old = %f)\n", _kmean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1), ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1));
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
      new_align_file<<" 0 \t"<<rh->tdrCmpMap[tt]
		    <<" \t " <<Smean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0)
		    <<" \t " <<Kmean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1)
		    <<" \t " <<ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2)
		    <<" \t " <<ev->GetMultiplicityFlip(0, rh->tdrCmpMap[tt])
		    <<endl;
      
      delta_align_file<<" 0 \t"<<rh->tdrCmpMap[tt]
		      <<" \t " <<Smean
		      <<" \t " <<Kmean
		      <<" \t " <<ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2)
		      <<" \t " <<ev->GetMultiplicityFlip(0, rh->tdrCmpMap[tt])
		      <<endl;
      
      //    printf("%d) %f -> %f, %f -> %f\n", rh->tdrCmpMap[tt], ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0), Smean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0), ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1), Kmean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1));
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
