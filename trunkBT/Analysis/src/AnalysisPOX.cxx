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

//raw value. To put the real distance from the most upward AMS ladder to the desired position in the ICC
#define ZHERD 1500

int main(int argc, char* argv[]) {
  
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

  //// TEMPORARY HARDCODED THINGS
  // xyalignment of 50x50 detector with a core read by 4096 strips pitch 0.11
  double ddzone=24.75; // shift for getting 4096 strips in decode -> to check
  // double ddzone=24.75; // shift for getting 4096 strips in decode -> to check

  
  //TString align_filename = "alignment_mc.dat";
  //TString align_filename = "alignment_mc_300.dat";
  TString align_filename = "alignment_mc_150.dat";
  TString gaincorrection_filename = "gaincorrection_mc.dat";
  TString output_filename = argv[1];

  //
  //TFile *mcf=new TFile("/home/vivi/POXsoft/NEW/POXsim/anaOut1M_gamma_p1_50x50_3_150_edep_INIT.root");
  
  TFile *mcf=new TFile("../RawData/anaOut.root");

  printf("---------------------------------------------\n");
  Event::ReadAlignment(align_filename.Data());
  Event::ReadGainCorrection(gaincorrection_filename.Data());
  //  exit(1);
  printf("---------------------------------------------\n");
  
  Event *ev;
  Cluster *cl;

  Long64_t entries = chain->GetEntries();
  //  entries=10000;
  printf("This run has %lld entries\n", entries);
  
  ev = new Event();

  chain->SetBranchAddress("cluster_branch", &ev);
  chain->GetEntry(0);

  int _maxtdr = NJINF*NTDRS;

  if (GetRH(chain)) {

    GetRH(chain)->Print();
     _maxtdr = GetRH(chain)->GetNTdrs();
    printf("This run has %d layers\n", _maxtdr);
  }
  else {
    printf("Not able to find the RHClass header in the UserInfo...\n");
    return -9;
  }
  
  //  printf("%d\n", _maxladd);

  /// montecarlo file to get expected deviation for e- 
  double ezmom[MAXNHITS];
  int pphit=-1;

  TTree *mcht=(TTree*)mcf->Get("hitTree");
  mcht->SetBranchAddress("zMom",&ezmom);

  

  TFile* foutput = new TFile(output_filename.Data(), "RECREATE");
  foutput->cd();
  
  TH1F* occupancy[NJINF*NTDRS];
  TH1F* occupancy_posS[NJINF*NTDRS];
  TH1F* occupancy_posK[NJINF*NTDRS];
  TH1F* residual_S[NJINF*NTDRS];
  TH1F* residual_K[NJINF*NTDRS];
  TH1F* chargeS[NJINF*NTDRS];
  TH1F* chargeK[NJINF*NTDRS];
  TH2F* charge2D[NJINF*NTDRS];
  TH1F* chargeS_ave;
  TH1F* chargeK_ave;
  TH2F* charge2D_ave;
  
  int NSTRIPS=SCHANN; // FAKE S LAYERS MEASURING Y

  //  int NSTRIPSS=640;
  int NSTRIPSS=SCHANN; // FAKE S LAYERS MEASURING Y
  //int NSTRIPSS=4096; // FAKE S LAYERS MEASURING Y
  
  //  int NSTRIPSK=384; 
  //int NSTRIPSK=640; // FAKE K LAYERS MEASURING X
  int NSTRIPSK=KCHANN; // FAKE K LAYERS MEASURING X
  //int NSTRIPSK=4096; // FAKE K LAYERS MEASURING X
  
  // GetPitch used always with side=0 to get (S pitch)
  
  for (int tt=0; tt<_maxtdr; tt++) {
    // VV compatibility with newer version of RHClass (without tdrRawMap and findPosRaw)
    int tdrnum = GetRH(chain)->GetTdrNum(tt);
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", tdrnum), Form("occupancy_0_%02d;Channel number;Occupancy", tdrnum), NSTRIPS, 0, NSTRIPS); // was 1024
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_0_%02d", tdrnum), Form("occupancy_posS_0_%02d;Position_{S} (mm);Occupancy", tdrnum), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0,tt,0), NSTRIPSS*Cluster::GetPitch(0,tt,0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_0_%02d", tdrnum), Form("occupancy_posK_0_%02d;Position_{K} (mm);Occupancy", tdrnum), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(0,tt,0), NSTRIPSK*Cluster::GetPitch(0,tt,0));
    residual_S[tt] = new TH1F(Form("residual_S_0_%02d", tdrnum), Form("residual_S_0_%02d;Residual_{S} (mm);Entries", tdrnum), 
			      2*NSTRIPSS, -float(NSTRIPSS)/100.*Cluster::GetPitch(0,tt,0), float(NSTRIPSS)/100.*Cluster::GetPitch(0,tt,0));
    residual_K[tt] = new TH1F(Form("residual_K_0_%02d", tdrnum), Form("residual_K_0_%02d;Residual_{K} (mm);Entries",tdrnum), 40*NSTRIPSK, -20*float(NSTRIPSK)/100.*Cluster::GetPitch(0,tt,0), 20*float(NSTRIPSK)/100.*Cluster::GetPitch(0,tt,0));
    chargeS[tt] = new TH1F(Form("chargeS_0_%02d", tdrnum), Form("chargeS_0_%02d;Q_{S} (c.u.);Entries", tdrnum), 1000, 0, 100);
    chargeK[tt] = new TH1F(Form("chargeK_0_%02d", tdrnum), Form("chargeK_0_%02d;Q_{K} (c.u.);Entries", tdrnum), 1000, 0, 100);
    charge2D[tt] = new TH2F(Form("charge_0_%02d", tdrnum), Form("charge_0_%02d;Q_{S} (c.u.);Q_{K} (c.u.);Entries", tdrnum), 1000, 0, 100, 1000, 0, 100);
  }
  chargeS_ave = new TH1F("chargeS", "chargeS;Q_{S} (c.u.);Entries", 1000, 0, 100);
  chargeK_ave = new TH1F("chargeK", "chargeK;Q_{K} (c.u.);Entries", 1000, 0, 100);
  charge2D_ave = new TH2F("charge", "charge;Q_{S} (c.u.);Q_{K} (c.u.);Entries", 1000, 0, 100, 1000, 0, 100);

  TH1F* chi = new TH1F("chi", "chi;log10(#chi^{2});Entries", 1000, -5, 10);
  TH1F* theta = new TH1F("theta", "theta;#theta (rad);Entries", 10000, -1.0, 1.0);
  TH1F* phi = new TH1F("phi", "phi;#phi (rad);Entries", 1000, -TMath::Pi(), TMath::Pi());
  TH2F* thetaphi = new TH2F("thetaphi", "thetaphi;#theta (rad);#phi (rad);Entries", 10000, -1.0, 1.0, 1000, -TMath::Pi(), TMath::Pi());
  TH1F* X0 = new TH1F("X0", "X0;X_{Z=0} (mm);Entries", 1000, -100, 100);
  TH1F* Y0 = new TH1F("Y0", "Y0;Y_{Z=0} (mm);Entries", 1000, -100, 100);
  TH2F* X0Y0 = new TH2F("X0Y0", "X0Y0;X_{Z=0} (mm);Y_{Z=0} (mm);Entries", 1000, -100, 100, 1000, -100, 100);
  TH1F* X0HERD = new TH1F("X0HERD", "X0HERD;X_{Z=HERD} (mm);Entries", 1000, -100, 100);
  TH1F* Y0HERD = new TH1F("Y0HERD", "Y0HERD;Y_{Z=HERD} (mm);Entries", 1000, -100, 100);
  TH2F* X0Y0HERD = new TH2F("X0Y0HERD", "X0Y0HERD;X_{Z=HERD} (mm);Y_{Z=HERD} (mm);Entries", 1000, -100, 100, 1000, -100, 100);

  TH1F* hclusSladd = new TH1F("hclusSladd", "hclusSladd;Ladder;Clusters_{S}", 24, 0, 24);
  TH1F* hclusSladdtrack = new TH1F("hclusSladdtrack", "hclusSladdtrack;Ladder;Clusters_{S,OnTrack}", 24, 0, 24);
  TH1F* hclusKladd = new TH1F("hclusKladd", "hclusKladd;Ladder;Clusters_{K}", 24, 0, 24);
  TH1F* hclusKladdtrack = new TH1F("hclusKladdtrack", "hclusKladdtrack;Ladder;Clusters_{K,OnTrack}", 24, 0, 24);
  TH1F* hclus = new TH1F("hclus", "hclus;Clusters;Entries", 1000, 0, 1000);
  TH1F* hclus_aftersel = new TH1F("hclus_aftersel", "hclus_aftersel;Clusters;Entries", 1000, 0, 1000);

  TH1F* dyHist = new TH1F("dyHist", "Reconstructed Y bending ;dY_{rec} (mm);Entries", 1000, -50, 50);
  
  TH1F* dyRes = new TH1F("dyRes", "True-Reconstructed Y bending ;dY_{true}-dY_{rec} (mm);Entries", 1000, -50, 50);



  Long64_t cleanevs=0;
  Long64_t preselevs=0;
  Long64_t tracks=0;
  Long64_t goodtracks=0;
  Long64_t goodStracks=0;
  Long64_t goodKtracks=0;

  TStopwatch sw;
  sw.Start();

  int perc=0;
  
  for (int index_event=0; index_event<entries; index_event++) {
    Double_t pperc=100.0*((index_event+1.0)/entries);
    if (pperc>=perc) {
      printf("Processed %d out of %lld: %d%%\n", (index_event+1), entries, (int)(100.0*(index_event+1.0)/entries));
      perc++;
    }
    //    printf("----- new event %d/%lld = %d%%\n", index_event, entries, (int)(100.0*(index_event+1.0)/entries));
    chain->GetEntry(index_event);
   
    int NClusTot = ev->GetNClusTot();
    //    printf("\t\tnclusters = %d\n", NClusTot);

    hclus->Fill(NClusTot);
    
    //at least 4 clusters (if we want 2 on S and 2 on K this is really the sindacal minimum...)
    //and at most 50 (to avoid too much noise around and too much combinatorial)
    //at most 4 clusters per ladder (per side) + 0 additional clusters in total (per side)
    // CleanEvent(ev, GetRH(chain), 4, 50, 6, 6, 0, 0);
    
    bool cleanevent = CleanEvent(ev, GetRH(chain), 4, 9999, 9999, 9999, 9999, 9999);
    if (!cleanevent) continue;
    cleanevs++;//in this way this number is giving the "complete reasonable sample"
    
    bool preselevent = CleanEvent(ev, GetRH(chain), 4, 50, 4, 50, 0, 0);
    if (!preselevent) continue;
    preselevs++;
    
    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];

    //at least 4 points on S, and 4 points on K, not verbose
    // ev->FindTrackAndFit(4, 4, false);
    bool trackfitok = ev->FindTrackAndFit(3, 4, true);
    //    printf("%d\n", trackfitok);

    if (trackfitok) {

      //remove from the best fit track the worst hit if giving a residual greater than 6.0 sigmas on S and 6.0 sigmas on K
      //(but only if removing them still we'll have more or equal than 3 hits on S and 3 (2 if 'HigherCharge') hits on K)
      //and perform the fit again
      // ev->RefineTrack(6.0, 3, 6.0, 3);

      int minSclus=3;
      int minKclus=3; 
      for (int ii=0; ii<_maxtdr-std::min(minSclus, minKclus); ii++) {
	ev->RefineTrack(3.0, minSclus, 3.0, minKclus);
      }
    }
    else {
      //let's downscale to 2 (on S) and 2 (on K) hits but even in this no-chisq case
      //let's garantee a certain relaiability of the track fitting the one with the higher charge (this method can be used also for ions)
      //and requiring an higher S/N (>5.0) for the cluster
      // trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false);

      ///      trackfitok = ev->FindHigherChargeTrackAndFit(2, 5.0, 2, 5.0, false); 
      trackfitok = ev->FindHigherChargeTrackAndFit(3, 5.0, 4, 5.0, false);
      
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
    //    if (logchi>2) continue;
    goodtracks++;
    
    bool strackok = false;
    bool ktrackok = false;

    /*
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
    */
    strackok=true;
    ktrackok=true;

    // if (ev->GetNHitsTrack()>5) {
    //   printf("Nhits: %u (S: %u, K: %u)\n", ev->GetNHitsTrack(), ev->GetNHitsSTrack(), ev->GetNHitsKTrack());
    // }

    // true Y bending for proton
    //double mom=100.934;  // mom in GV (not geant4 ene)
    //double mom=10.898;  // mom in GV (not geant4 ene)
    //double mom=1.69604;  // mom in GV (not geant4 ene)


    //// for e- produced by gamma it should come from simrootfile 
    //double mom=0.622826; // first ev K=1 GeV
    //double mom=0.311349; // first ev K=0.5 GeV
    //double mom=0.031011;// first ev K=0.05 GeV
    mcht->GetEntry(index_event);
    double mom=ezmom[0];
    double magf=0.05; // mag field [tesla]
    double magz=0.2; // 2* mag z size [m]
    double ro=mom/magf/0.3;  // radius of curvature [m]
    double dYtrue = ro*(1-sqrt(1-(magz/ro)*(magz/ro)));
    double thtrue = atan2(dYtrue,magz);
    std::cout<<"DYRES ev: "<<index_event<<" "<<mom<<"GV "<<ro<<"m DY true: "<<dYtrue*1000<<"mm DY rec: "<<1e3*magz*tan(ev->GetThetaTrack())<<"mm -> RES: "<<1e3*(dYtrue-magz*tan(ev->GetThetaTrack()))<<"  RES THETA: "<<thtrue<<" "<<ev->GetThetaTrack()<<" "<<thtrue-(ev->GetThetaTrack())<<std::endl;
    
    chi->Fill(log10(ev->GetChiTrack()));
    theta->Fill(ev->GetThetaTrack());
    dyHist->Fill(1e3*magz*tan(ev->GetThetaTrack()));
    dyRes->Fill(1e3*(dYtrue-magz*tan(ev->GetThetaTrack())));
    phi->Fill(ev->GetPhiTrack());
    thetaphi->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
    // VV compatibility with newer version X0 -> S0 and Y0 -> K0
    // X0->Fill(ev->GetX0Track());
    // Y0->Fill(ddzone+ev->GetY0Track());
    X0->Fill(ev->GetS0Track());
    Y0->Fill(ddzone+ev->GetK0Track());

    printf("TRACK PARAMETERS chi th phi X0 Y0 -> %f %f %f %f %f\n", ev->GetChiTrack(),ev->GetThetaTrack(),ev->GetPhiTrack(),ev->GetS0Track(),ev->GetK0Track());
    X0Y0->Fill(ev->GetS0Track(), ddzone+ev->GetK0Track());
    X0HERD->Fill(ev->ExtrapolateTrack(ZHERD, 0));
    Y0HERD->Fill(ddzone+ev->ExtrapolateTrack(ZHERD, 1));
    X0Y0HERD->Fill(ev->ExtrapolateTrack(ZHERD, 0), ddzone+ev->ExtrapolateTrack(ZHERD, 1));

    printf("EXTRAPOLATE TRACK X at Z 1000 -> %f \n", ev->ExtrapolateTrack(1000,0));
    printf("EXTRAPOLATE TRACK Y at Z 1000 -> %f \n", ev->ExtrapolateTrack(1000,1));

    
    hclus_aftersel->Fill(NClusTot);
  
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
	
      cl = ev->GetCluster(index_cluster);
      
      int ladder = cl->ladder;
      //      printf("%d --> %d\n", ladder, GetRH(chain)->FindPos(ladder));

      // VV: compatibility change FindPosRaw -> FindPos
      occupancy[GetRH(chain)->FindPos(ladder)]->Fill(cl->GetCoG());
      int side=cl->side;

      if (side==0) {
	occupancy_posS[GetRH(chain)->FindPos(ladder)]->Fill(ddzone+cl->GetAlignedPositionMC());
	v_cog_all_laddS[GetRH(chain)->FindPos(ladder)].push_back(ddzone+cl->GetAlignedPositionMC());
      }
      else {
	occupancy_posK[GetRH(chain)->FindPos(ladder)]->Fill(ddzone+cl->GetAlignedPositionMC());
	v_cog_all_laddK[GetRH(chain)->FindPos(ladder)].push_back(ddzone+cl->GetAlignedPositionMC());
      }
            
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;

      double charge=cl->GetCharge();//unused for now
      
      if (side==0) {
	if (strackok) {
	  residual_S[GetRH(chain)->FindPos(ladder)]->Fill(ddzone+cl->GetAlignedPositionMC()-ev->ExtrapolateTrack(cl->GetZPosition(), 1));// was 0 for Y
	  v_cog_laddS[GetRH(chain)->FindPos(ladder)].push_back(ddzone+cl->GetAlignedPositionMC());
	  hclusSladdtrack->Fill(ladder);
	}
      }
      else {
	if (ktrackok) {
	  residual_K[GetRH(chain)->FindPos(ladder)]->Fill(ddzone+cl->GetAlignedPositionMC()-ev->ExtrapolateTrack(cl->GetZPosition(), 0));// was 1 for Y
	  v_cog_laddK[GetRH(chain)->FindPos(ladder)].push_back(ddzone+cl->GetAlignedPositionMC());
	  hclusKladdtrack->Fill(ladder);
	}
      }
      
    }

    std::vector<std::pair<int, std::pair<int, int> > > vec_charge = ev->GetHitVector();
    for (unsigned int tt=0; tt<vec_charge.size(); tt++) {
      int ladder = vec_charge.at(tt).first;
      int index_cluster_S = vec_charge.at(tt).second.first;
      int index_cluster_K = vec_charge.at(tt).second.second;
      if (index_cluster_S>=0) {
	chargeS[GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge());
      }
      if (index_cluster_K>=0) {
	chargeK[GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_K)->GetCharge());
      }
      if (index_cluster_S>=0 && index_cluster_K>=0) {
	charge2D[GetRH(chain)->FindPos(ladder)]->Fill(ev->GetCluster(index_cluster_S)->GetCharge(), ev->GetCluster(index_cluster_K)->GetCharge());
      }
    }
    
    chargeS_ave->Fill(ev->GetChargeTrack(0));
    chargeK_ave->Fill(ev->GetChargeTrack(1));
    charge2D_ave->Fill(ev->GetChargeTrack(0), ev->GetChargeTrack(1));

    // VV: changed for compatibility with newer version of RHClass
    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      //      hclusSladd->Fill(GetRH(chain)->tdrRawMap[ll], v_cog_all_laddS[ll].size());
      // hclusKladd->Fill(GetRH(chain)->tdrRawMap[ll], v_cog_all_laddK[ll].size());
      hclusSladd->Fill(GetRH(chain)->GetTdrNum(ll), v_cog_all_laddS[ll].size());
      hclusKladd->Fill(GetRH(chain)->GetTdrNum(ll), v_cog_all_laddK[ll].size());
    }
    
    //    printf("\n",);
  }

  sw.Stop();
  sw.Print();
  
  printf("---------------------------------------------\n");
  printf("\t%lld events analyzed\n", entries);
  printf("\t\t%lld clean events\n", cleanevs);
  printf("\t\t%lld preselected events\n", preselevs);
  printf("\t\t%lld tracks fitted\n", tracks);
  printf("\t\t%lld good tracks found\n", goodtracks);
  // printf("\t\t%lld good S tracks found\n", goodStracks);
  // printf("\t\t%lld good K tracks found\n", goodKtracks);
  printf("---------------------------------------------\n");
  
  foutput->Write();
  foutput->Close();


  return 0;
}
