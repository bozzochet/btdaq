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
#include <fstream>
#include <vector>

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

//#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);
#define PRINTDEBUG

using namespace std;

float deltaalign[NJINF][NTDRS][3];

int SingleAlign(int argc, char* argv[], bool donotwritealign=false);
void ReadDeltaAlignment(TString filename="delta_alignment.dat");
bool CleanEvent(Event* ev, RHClass *rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS=0, int safetyK=0);
bool ChargeSelection(Event *_ev, RHClass *_rh,float charge_center, float lower_limit, float higher_limit); 

int main(int argc, char* argv[]) {
  
  if (argc<3) {
    printf("Usage:\n");
    printf("%s <output root-filename> <first input root-filename> [second input root-filename] ...\n", argv[0]);
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
    
    bool allaligned=false;
    while (!allaligned) {
      
      ret = SingleAlign(argc, argv);
      
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
	    if (!aligned[jj][tt][cc] &&
		(fabs(deltaalign[jj][tt][cc])<0.003)
		) {
	      if (cc==0) printf("S ");
	      else if (cc==1) printf("K ");
	      printf("JINF %02d TDR %02d) aligned!\n", jj, tt);
	      aligned[jj][tt][cc]=true;
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
      
      if (ret) return ret;
    }

    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("FORSE LA SELEZIONE MESSA IN Analysis VA COPIATA QUI\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("L'ALLINEAMENTO E' MIGLIORE, AD ESEMPIO THETA, DANDO UNA PRIMA ALLINEATA ROZZA SEMPLICEMENTE SPOSTANDO LE OCCUPANCY A ZERO");
    printf("SENZA QUESTA COSA SI ALLINEA MA DIREI ATTORNO AD UNA TRAIETTORIA ARBITRARIA. QUINDI TIPO AD ESEMPIO THETA NON E' A MEDIA ZERO");
    printf("AGGIUNGERE DUNQUE UNA PRODUZIONE DI UN raw_aligment.dat, NEL CASO RUNONCE, CON LE SEMPLICI h->GetMean()\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("AGGIUNGERE UN REPORT ALLA FINE PER SAPERE QUELLI CHE NON HANNO CONVERGIUTO\n");
    printf("SE CI METTE TROPPO A CONVERGERE CHIAMARLO COMUNQUE COME NON CONVERGIUTO\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    printf("METTERE UN REBIN SUL PLOT DEI RESIDUI, PRIMA DI FITTARLO, BASATO SULLE ENTRIES: SE POCHE REBINNARE\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
    
    return ret;
  }
  else if (progname.Contains("RunOnce")) {

    ret = SingleAlign(argc, argv, true);

    return ret;
  }

  return -9;
}

int SingleAlign(int argc, char* argv[], bool donotwritealign){
  
  TChain *chain = new TChain("t4");
     
  for (int ii=2; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }

  TString align_filename = "alignment.dat";
  TString output_filename = argv[1];

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
  
  TFile* foutput = new TFile(output_filename.Data(), "RECREATE");
  foutput->cd();
  
  TH1F* occupancy[NJINF*NTDRS];
  TH1F* occupancy_posS[NJINF*NTDRS];
  TH1F* occupancy_posK[NJINF*NTDRS];
  TH1F* residual_posS[NJINF*NTDRS];
  TH1F* residual_posK[NJINF*NTDRS];
  int NSTRIPSS=640;
  int NSTRIPSK=384;
  for (int tt=0; tt<_maxtdr; tt++) {
    occupancy[tt] = new TH1F(Form("occupancy_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_0_%02d;Channel number;Occupancy", rh->tdrCmpMap[tt]), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_posS_0_%02d;Position_{S} (mm);Occupancy", rh->tdrCmpMap[tt]), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_0_%02d", rh->tdrCmpMap[tt]), Form("occupancy_posK_0_%02d;Position_{K} (mm);Occupancy", rh->tdrCmpMap[tt]), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(1), NSTRIPSK*Cluster::GetPitch(1));
    residual_posS[tt] = new TH1F(Form("residual_posS_0_%02d", rh->tdrCmpMap[tt]), Form("residual_posS_0_%02d;Residual_{S} (mm);Entries", rh->tdrCmpMap[tt]), 
				 2*NSTRIPSS, -float(NSTRIPSS)/100.*Cluster::GetPitch(0), float(NSTRIPSS)/100.*Cluster::GetPitch(0));
    residual_posK[tt] = new TH1F(Form("residual_posK_0_%02d", rh->tdrCmpMap[tt]), Form("residual_posK_0_%02d;Residual_{K} (mm);Entries", rh->tdrCmpMap[tt]), 
				 40*NSTRIPSK, -20*float(NSTRIPSK)/100.*Cluster::GetPitch(1), 20*float(NSTRIPSK)/100.*Cluster::GetPitch(1));
  }
  
  PRINTDEBUG;

  TH1F* chi = new TH1F("chi", "chi;log10(#chi^{2});Entries", 1000, -5, 10);
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
  
  //  for (int index_event=405; index_event<406; index_event++) {
  for (int index_event=0; index_event<entries; index_event++) {
    //    printf("----- new event %d\n", index_event);
    chain->GetEntry(index_event);

    int NClusTot = ev->GetNClusTot();
    //    printf("\t\tnclusters = %d\n", NClusTot);
    
    //at least 6 clusters and at most 12
    //at most 3 clusters per ladder (per side) + 1 additional clusters in total (per side)
    bool cleanevent = CleanEvent(ev, rh, 6, 30, 3, 3, 0, 0);
    if (!cleanevent) continue;

    bool chargeselection = ChargeSelection(ev, rh, 1, 0.9, 3) ; 
    if (!chargeselection) continue;

    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_all_laddK[NJINF*NTDRS];

    bool trackfitok = ev->FindTrackAndFit(3, 3, false);//at least 2 points on S, and 2 points on K, not verbose

    //    printf("%d\n", trackfitok);
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiTrack(), ev->GetThetaTrack(), ev->GetPhiTrack(), ev->GetX0Track(), ev->GetY0Track());

    //    printf("S %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(0), ev->IsTDRInTrack(0, 0), ev->IsTDRInTrack(0, 4), ev->IsTDRInTrack(0, 8), ev->IsTDRInTrack(0, 12), ev->IsTDRInTrack(0, 14));
    //    printf("K %024lld: %d %d %d %d %d\n", ev->GetTrackHitPattern(1), ev->IsTDRInTrack(1, 0), ev->IsTDRInTrack(1, 4), ev->IsTDRInTrack(1, 8), ev->IsTDRInTrack(1, 12), ev->IsTDRInTrack(1, 14));
    
    // //                              321098765432109876543210
    // if (ev->GetTrackHitPattern(0) <                100010001) continue;
    // if (ev->GetTrackHitPattern(1) <                100010001) continue;

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

    chi->Fill(log10(ev->GetChiTrack()));
    theta->Fill(ev->GetThetaTrack());
    phi->Fill(ev->GetPhiTrack());
    thetaphi->Fill(ev->GetThetaTrack(), ev->GetPhiTrack());
    X0->Fill(ev->GetX0Track());
    Y0->Fill(ev->GetY0Track());
    X0Y0->Fill(ev->GetX0Track(), ev->GetY0Track());

    hclus->Fill(NClusTot);
    
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
	
      cl = ev->GetCluster(index_cluster);
      
      int ladder = cl->ladder;
      //      printf("%d --> %d\n", ladder, rh->FindPos(ladder));
      occupancy[rh->FindPos(ladder)]->Fill(cl->GetCoG());
      int side=cl->side;
      
      if (side==0) {
	occupancy_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	v_cog_all_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()>0.5*NSTRIPSK*Cluster::GetPitch(1)?cl->GetAlignedPosition()-NSTRIPSK*Cluster::GetPitch(1):cl->GetAlignedPosition());
	v_cog_all_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
            
      if (!ev->IsClusterUsedInTrack(index_cluster)) continue;
      
      if (side==0) {
	if (strackok) {
	  residual_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 0));
	  v_cog_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusSladdtrack->Fill(ladder);
	}
      }
      else {
	if (ktrackok) {
	  residual_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateTrack(cl->GetZPosition(), 1));
	  v_cog_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
	  hclusKladdtrack->Fill(ladder);
	}
      }
      
    }

    for (int ll=0; ll<NJINF*NTDRS; ll++) {
      hclusSladd->Fill(rh->tdrCmpMap[ll], v_cog_all_laddS[ll].size());
      hclusKladd->Fill(rh->tdrCmpMap[ll], v_cog_all_laddK[ll].size());
    }
      
    //    printf(" \n ");
    //    exit(1);
  }
  
  PRINTDEBUG;

  if (!donotwritealign) {
    ofstream new_align_file(align_filename.Data(), ios_base::out);
    
    ofstream delta_align_file(Form("delta_%s", align_filename.Data()), ios_base::out);
    
    new_align_file<<"#JINF \t TDR \t S (mm) \t K (mm) \t Z (mm)"<<endl;
    delta_align_file<<"#JINF \t TDR \t S (mm) \t K (mm) \t Z (mm)"<<endl;
    
    float fit_limit[2]={-30.0, 30.0};
    for (int tt=0; tt<_maxtdr; tt++) {
      TF1* gauss = new TF1("gauss", "gaus", -100.0, 100.0);
      
      //----------
      
      //    double Smean = residual_posS[tt]->GetMean();
      double Smean = residual_posS[tt]->GetBinCenter(residual_posS[tt]->GetMaximumBin());
      fit_limit[0]=Smean-0.3;
      fit_limit[1]=Smean+0.3;
      
      residual_posS[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      residual_posS[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      
      fit_limit[0]=gauss->GetParameter(1)-3.0*gauss->GetParameter(2);
      fit_limit[1]=gauss->GetParameter(1)+3.0*gauss->GetParameter(2);
      
      residual_posS[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      residual_posS[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      
      Smean = gauss->GetParameter(1);
      // cout<<" Fit between "<<fit_limit[0]
      //  	<<" and "<<	   fit_limit[1]
      //  	<<endl;
      // printf("S align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0));
      
      //----------
      
      //    double Kmean = residual_posK[tt]->GetMean();
      double Kmean = residual_posK[tt]->GetBinCenter(residual_posK[tt]->GetMaximumBin());
      fit_limit[0]=Kmean-0.3;
      fit_limit[1]=Kmean+0.3;
      
      residual_posK[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      residual_posK[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      
      fit_limit[0]=gauss->GetParameter(1)-3.0*gauss->GetParameter(2);
      fit_limit[1]=gauss->GetParameter(1)+3.0*gauss->GetParameter(2);
      
      residual_posK[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      residual_posK[tt]->Fit("gauss", "Q", "", fit_limit[0], fit_limit[1]);
      
      Kmean = gauss->GetParameter(1);
      // cout<<" Fit between "<<fit_limit[0]
      //  	<<" and "<<	   fit_limit[1]
      //  	<<endl;    
      // printf("K align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1));
      
      new_align_file<<" 0 \t"<<rh->tdrCmpMap[tt]
		    <<" \t " <<Smean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0)
		    <<" \t " <<Kmean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1)
		    <<" \t " <<ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2)
		    <<endl;
      
      delta_align_file<<" 0 \t"<<rh->tdrCmpMap[tt]
		      <<" \t " <<Smean
		      <<" \t " <<Kmean
		      <<" \t " <<ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2)
		      <<endl;
      
      //    printf("%d) %f -> %f, %f -> %f\n", rh->tdrCmpMap[tt], ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0), Smean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0), ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1), Kmean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1));
    }
    
    PRINTDEBUG;
    
    new_align_file.close();
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

bool CleanEvent(Event* ev, RHClass *rh, int minclus, int maxclus, int perladdS, int perladdK, int safetyS, int safetyK){

  int NClusTot = ev->GetNClusTot();
  if(NClusTot<(minclus-1) ||  NClusTot>(maxclus+1)) return false;
  
  int nclusS[NJINF*NTDRS];
  int nclusK[NJINF*NTDRS];
  memset(nclusS, 0, sizeof(nclusS[0])*NJINF*NTDRS);
  memset(nclusK, 0, sizeof(nclusK[0])*NJINF*NTDRS);

  int safetySspent = safetyS;
  int safetyKspent = safetyK;
  
  for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
    Cluster *cl = ev->GetCluster(index_cluster);
    int ladder = cl->ladder;
    //      printf("%d --> %d\n", ladder, rh->FindPos(ladder));
    int side=cl->side;
    if (side==0) {
      nclusS[rh->FindPos(ladder)]++;
      if (nclusS[rh->FindPos(ladder)]>=(perladdS+safetyS)) {
	safetySspent--;
	if (safetySspent<0) return false;
      }
    }
    else {
      nclusK[rh->FindPos(ladder)]++;
      if (nclusK[rh->FindPos(ladder)]>=(perladdK+safetyK)) {
	safetyKspent--;
	if (safetyKspent<0) return false;
      }
    }
  }

  /*
  for (int jj=0; jj<NJINF*NTDRS; jj++) {
    printf("%d %d\n", nclusS[jj], nclusK[jj]);
  }
  */
  
  return true;
}

// GENERALIZE ME! IS ALSO WRONG:
// - IF FOR ONE LADDER THERE ARE TWO CLUSTERS ONLY THE SECOND IS CONSIDERED.
// - IF ON LADDER HAS NO CLUSTERS THE CUT IS NOT PASSED
// - IT ASSUMES THAT ONLY ONE JINF IS PRESENT (THIS MAYBE IS SAFE AT THIS LEVEL)
bool ChargeSelection(Event *ev, RHClass *_rh, float charge_center, float lower_limit, float higher_limit){
  bool chargeselection=false;
  
  float charge[NTDRS];
  for (int ii=0; ii<NTDRS; ii++) {
    charge[ii] = 0.0;
  }
  
  for (int index_cluster=0; index_cluster<ev->GetNClusTot(); index_cluster++) {
    //    if (!ev->IsClusterUsedInTrack(index_cluster)) continue;
    
    Cluster *_cl = ev->GetCluster(index_cluster);
    int ladder = _cl->ladder;
    
    //    printf("%d --> %d\n", ladder, _rh->tdrCmpMap[ladder]);
    // printf("%d --> %d\n", ladder, _rh->FindPos(ladder));
    charge[ladder]=_cl->GetCharge();
  }
  if(    ((charge[0] > (charge_center-lower_limit)) && (charge[0] < (charge_center+higher_limit)))
	 && ((charge[4] > (charge_center-lower_limit)) && (charge[4] < (charge_center+higher_limit)))
	 && ((charge[8] > (charge_center-lower_limit)) && (charge[8] < (charge_center+higher_limit)))
	 && ((charge[14] > (charge_center-lower_limit)) && (charge[14] < (charge_center+higher_limit)))
	 
	 )
    chargeselection=true;
  
  return chargeselection;
}
