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

/* from the 'Decode' API */
#include "Cluster.hh"
#include "Event.hh"
/* end */

#include <vector>
#define PRINTDEBUG printf("%s) This is the line number %d\n", __FILE__, __LINE__);

using namespace std;

bool ChargeSelection(Event *_ev, RHClass *_rh,float charge_center, float lower_limit, float higher_limit); 


int main(int argc, char* argv[]) {

  if (argc<1) {
    printf("No ROOT file passed...\n");
    return 1;
  }

  TChain *chain = new TChain("t4");
     
  for (int ii=3; ii<argc; ii++) {
    printf("Adding file %s to the chain...\n", argv[ii]);
    chain->Add(argv[ii]);
  }


  TString align_filename = argv[1];
  TString output_filename = argv[2];
  TString filename = argv[3];

  PRINTDEBUG
    Event::ReadAlignment(align_filename.Data());
  PRINTDEBUG
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
    occupancy[tt] = new TH1F(Form("occupancy_%02d", tt), Form("occupancy_%02d", tt), 1024, 0, 1024);
    occupancy_posS[tt] = new TH1F(Form("occupancy_posS_%02d", tt), Form("occupancy_posS_%02d", tt), 2*NSTRIPSS, -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    occupancy_posK[tt] = new TH1F(Form("occupancy_posK_%02d", tt), Form("occupancy_posK_%02d", tt), 2*NSTRIPSK, -NSTRIPSK*Cluster::GetPitch(1), NSTRIPSK*Cluster::GetPitch(1));
    residual_posS[tt] = new TH1F(Form("residual_posS_%02d", tt), Form("residual_posS_%02d", tt), 
				 2*NSTRIPSS, -float(NSTRIPSS)/100.*Cluster::GetPitch(0), float(NSTRIPSS)/100.*Cluster::GetPitch(0));
    residual_posK[tt] = new TH1F(Form("residual_posK_%02d", tt), Form("residual_posK_%02d", tt), 
				 4*NSTRIPSK, -20*float(NSTRIPSK)/100.*Cluster::GetPitch(1), 20*float(NSTRIPSK)/100.*Cluster::GetPitch(1));
  }
  PRINTDEBUG
  TH1F* htheta = new TH1F("htheta", "htheta", 1000, -0.005*TMath::Pi(), 0.005*TMath::Pi());
  TH1F* htphi = new TH1F("hphi", "hphi", 1000, TMath::Pi(), TMath::Pi());
  TH1F* hX0 = new TH1F("hX0", "hX0", 1000, -100, 100);
  TH1F* hY0 = new TH1F("hY0", "hY0", 1000, -100, 100);
  TH1F* hchi = new TH1F("hchi", "hchi", 1000, -5, 10);
  PRINTDEBUG
  //  for (int index_event=405; index_event<406; index_event++) {
  for (int index_event=0; index_event<entries; index_event++) {
    //    printf("----- new event %d\n", index_event);
    chain->GetEntry(index_event);
    
    int NClusTot = ev->NClusTot;
    if(NClusTot<1 ||  NClusTot>10) continue;


    bool chargeselection = ChargeSelection(ev, rh, 1, 0.9, 3) ; 
    if (chargeselection == false ) continue;

     

    std::vector<double> v_cog_laddS[NJINF*NTDRS];
    std::vector<double> v_cog_laddK[NJINF*NTDRS];

    bool trackfitok = ev->FindTrackAndFit(3, 3, false);//at least 2 points on S, and 2 points on K, not verbose

    //    printf("%d\n", trackfitok);
    if (!trackfitok) continue;
    //    printf("%f %f %f %f %f\n", ev->GetChiBestTrack(), ev->GetThetaBestTrack(), ev->GetPhiBestTrack(), ev->GetX0BestTrack(), ev->GetY0BestTrack());
    /* 
       cout<<" Tracker Pattern "<<ev->GetTrackHitPattern(0)
	<<" \t "<<ev->GetTrackHitPattern(1)
	<<endl;
    */
    if(ev->GetTrackHitPattern(1) != 111) continue;
    //

    htheta->Fill(ev->GetThetaBestTrack()/180*TMath::Pi());
    htphi->Fill(ev->GetPhiBestTrack()/180*TMath::Pi());
    hX0->Fill(ev->GetX0BestTrack());
    hY0->Fill(ev->GetY0BestTrack());
    hchi->Fill(log10(ev->GetChiBestTrack()));
    bool makeme_exit=false;
    for (int index_cluster=0; index_cluster<NClusTot; index_cluster++) {
      if (!ev->IsClusterUsedInBestTrack(index_cluster)) continue;
      
      cl = ev->GetCluster(index_cluster);

      int ladder = cl->ladder;
      occupancy[rh->FindPos(ladder)]->Fill(cl->GetCoG());
      int side=cl->side;

      if( (side==1) && (cl->GetTDR()== 12 || cl->GetTDR()== 14)){
	makeme_exit=true;
      }
      if(makeme_exit)
	printf(" TDR %d Pattern %d\n", cl->GetTDR(), ev->GetTrackHitPattern(1));       


      if (side==0) {
	occupancy_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	residual_posS[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateBestTrack(cl->GetZPosition(), 0));
	v_cog_laddS[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      else {
	occupancy_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition());
	residual_posK[rh->FindPos(ladder)]->Fill(cl->GetAlignedPosition()-ev->ExtrapolateBestTrack(cl->GetZPosition(), 1));
	v_cog_laddK[rh->FindPos(ladder)].push_back(cl->GetAlignedPosition());
      }
      if(makeme_exit) exit(1);
    }

    //    printf(" \n ");
    //    exit(1);
  }
  PRINTDEBUG
    float n_RMS_for_fit=3.;    
  float fit_limit[2]={0.,0.};
  for (int tt=0; tt<_maxtdr; tt++) {
    TF1* gauss = new TF1("gauss", "gaus", -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    //    occupancy_posS[tt]->Fit("gauss", "Q", "", -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
    //    occupancy_posS[tt]->Fit("gauss", "Q", "", -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));

    fit_limit[0]=occupancy_posS[tt]->GetMean()-n_RMS_for_fit*occupancy_posS[tt]->GetRMS();
    fit_limit[1]=occupancy_posS[tt]->GetMean()+n_RMS_for_fit*occupancy_posS[tt]->GetRMS();
    occupancy_posS[tt]->Fit("gauss", "Q", "", 
			    fit_limit[0],
			    fit_limit[1]
			    );
    double Smean = /*ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0) +*/ gauss->GetParameter(1);
    //    printf("S align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0));
    fit_limit[0]=occupancy_posK[tt]->GetMean()-n_RMS_for_fit*occupancy_posK[tt]->GetRMS();
    fit_limit[1]=occupancy_posK[tt]->GetMean()+n_RMS_for_fit*occupancy_posK[tt]->GetRMS();

    occupancy_posK[tt]->Fit("gauss", "Q", "", 
			    fit_limit[0],
                            fit_limit[1]			    
			    );
    //    occupancy_posK[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    //    occupancy_posK[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
    double Kmean = /*ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1) +*/ gauss->GetParameter(1);
    //    printf("K align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1));
    // printf("%d -> %d) %f %f\n", rh->tdrCmpMap[tt], tt, Smean, Kmean);
  }
  PRINTDEBUG
    ofstream new_align_file(Form("new_%s",align_filename.Data(),ios_base::out));
  
  new_align_file<<"#JINF \t TDR \t S (mm) \t K (mm) \t Z (mm)"<<endl;

  for (int tt=0; tt<_maxtdr; tt++) {
    if (rh->tdrCmpMap[tt] == 12) n_RMS_for_fit = 1.;
    else n_RMS_for_fit=3.;
      TF1* gauss = new TF1("gauss", "gaus", -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
      //    residual_posS[tt]->Fit("gauss", "Q", "", -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
      //    residual_posS[tt]->Fit("gauss", "Q", "", -NSTRIPSS*Cluster::GetPitch(0), NSTRIPSS*Cluster::GetPitch(0));
      fit_limit[0]=residual_posS[tt]->GetMean()-n_RMS_for_fit*residual_posS[tt]->GetRMS();      
      fit_limit[1]=residual_posS[tt]->GetMean()+n_RMS_for_fit*residual_posS[tt]->GetRMS();

      residual_posS[tt]->Fit("gauss", "Q", "", 
			     fit_limit[0],
			     fit_limit[1]			     
			     );

      double Smean = /*ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0) +*/ gauss->GetParameter(1);
      cout<<" Fit between "<<fit_limit[0]
	  <<" and "<<	     fit_limit[1]
	  <<endl;
      printf("S align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0));
     //    residual_posK[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));
      //    residual_posK[tt]->Fit("gauss", "Q", "", -640*Cluster::GetPitch(0), 640*Cluster::GetPitch(0));

      int peak_bin=residual_posK[tt]->GetMaximumBin();
      float peak_xposition = residual_posK[tt]->GetBinCenter(peak_bin);
            
      cout<<" @@@@@@@@@@@@@@@@  PEAK "<< peak_xposition <<endl;
      n_RMS_for_fit=1;
      fit_limit[0]=peak_xposition-n_RMS_for_fit*residual_posK[tt]->GetRMS();//residual_posK[tt]->GetMean()-n_RMS_for_fit*residual_posK[tt]->GetRMS(),
      fit_limit[1]=peak_xposition+n_RMS_for_fit*residual_posK[tt]->GetRMS();//residual_posK[tt]->GetMean()+n_RMS_for_fit*residual_posK[tt]->GetRMS()
      
      residual_posK[tt]->Fit("gauss", "Q", "", 
			     fit_limit[0],
                             fit_limit[1]
			     );
      double Kmean = /*ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1) +*/ gauss->GetParameter(1);
      cout<<" Fit between "<<fit_limit[0]
	  <<" and "<<	     fit_limit[1]
	  <<endl;

      printf("K align par = %f\n", ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1));
      printf("%d -> %d) %f %f\n", rh->tdrCmpMap[tt], tt, Smean, Kmean);
      if (rh->tdrCmpMap[tt] == 12 || rh->tdrCmpMap[tt] == 14 ){
	Smean=0.;
	Kmean=0.;
      }
      new_align_file<<" 0 \t"<<rh->tdrCmpMap[tt]
		    <<" \t " <<Smean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 0)
		    <<" \t " <<Kmean+ev->GetAlignPar(0, rh->tdrCmpMap[tt], 1)
		    <<" \t " <<ev->GetAlignPar(0, rh->tdrCmpMap[tt], 2)
		    <<endl;

    }
  PRINTDEBUG
    new_align_file.close();
  foutput->Write();
  foutput->Close();
  
  return 0;
}



bool ChargeSelection(Event *ev, RHClass *_rh,float charge_center, float lower_limit, float higher_limit){
  bool chargeselection=false;

  float charge[NTDRS];
  for (int index_cluster=0; index_cluster<ev->NClusTot; index_cluster++) {
    //    if (!ev->IsClusterUsedInBestTrack(index_cluster)) continue;
      
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

