#include "Event.hh"
#include "Cluster.hh"

#include "TrackSelection.hh"
#include "Utilities.hh"

#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"
#include "TH2D.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TRandom3.h"

using namespace std;

class MCTruth{
  map<int, int> MCTid;
  map<int,TVector3> MCTmom;
public:
  MCTruth(Event* ev, int nHits, int chXY[], int hVol[], int pID[],
	  double xMom[], double yMom[], double zMom[]){
    int Ncl=ev->GetNClusTot();
    Cluster* cc;
    int tdr;
    bool noise;
    for(int icl=0;icl<Ncl;icl++){
      cc=ev->GetCluster(icl);
      tdr=cc->GetTDR();
      MCTid.insert(make_pair(icl,2));
      noise=true;
      ////int x0=cc->GetAddress()+112;
      //printf("address: %d\n",ev->GetCluster(icl)->GetAddress());
      //      int x0=cc->GetAddress()+((cc->side)?177:224);
      int x0=cc->GetAddress()+112;
      int x1=x0+cc->GetLength();
      //printf("(%d) %d - %d:\n",tdr,x0,x1);
      for(int h=0;h<nHits;h++){
	//if(hVol[h]==tdr)cout<<chXY[h]<<endl;
	if (chXY[h]>=x0 && chXY[h]<x1 && hVol[h]==tdr){
	  noise=false;
	  if (pID[h]==2 || MCTid[icl]==0) continue;
	  MCTmom.insert(make_pair(icl,TVector3(xMom[h],yMom[h],zMom[h])));
	  if (MCTid[icl]==2) MCTid[icl]=pID[h];
	  else if (MCTid[icl]!=pID[h]) MCTid[icl]=0;
	}
      }
      if (noise) MCTid[icl]=-2;
    }
  }
  int ID(int i){
    return MCTid[i];
  }
  TVector3 Mom(int i){
    return MCTmom[i];
  }
};

double PolarQuantile(TH2* h,double res){
  int Tot=h->GetEntries();
  double step=h->GetXaxis()->GetBinWidth(10);
  for(double raggio=0;raggio<1;raggio+=step){
    double Int=0;
    for(double y=-raggio;y<=raggio;y+=step){
      Int+=h->Integral(h->GetXaxis()->FindBin(-sqrt(raggio*raggio-y*y)),
		       h->GetXaxis()->FindBin(sqrt(raggio*raggio-y*y)),
		       h->GetYaxis()->FindBin(y),
		       h->GetYaxis()->FindBin(y));
    }
    //   if (raggio-0.009<0.001) cout<<Int<<" <-- QUESTO\n";
    if ( double(Int/Tot) > res ) return raggio;
  }
  return 0;;
}

TMultiGraph *GraphHits(trackColl* trc){
  //  printf("linea %d\n",__LINE__);
  TMultiGraph* MG=new TMultiGraph();
  for (int i=0;i<trc->size();i++) {
    //    printf("linea %d\n",__LINE__);
    TGraph* g=new TGraph();
    for (int j=0;j<(trc->at(i)).hits.size();j++){
      //printf("linea %d\n",__LINE__);
      g->SetPoint( j, (trc->at(i)).hits[j].second.second, (trc->at(i)).hits[j].second.first );
      //  printf("linea %d\n",__LINE__);
    }
    g->SetLineColor(2+i);
    MG->Add(g);
  }
  //  printf("linea %d\n",__LINE__);
  return MG;
}

bool valuta_prod(track &rico, MCTruth &truth){
  if (rico.hits[1].second.second-rico.hits[0].second.second>61.) return false;
  int a=truth.ID(rico.hits[0].first);
  int b=truth.ID(rico.hits[1].first);
  if (a==2 || b==2) return false;
  if (a==0 || b==0) return true;
  if (a==b) return true;
  else return false;
}
bool valuta_exit(track &rico, MCTruth &truth){
  if (rico.hits.back().second.second-rico.hits.end()[-2].second.second>61.) return false;
  int a=truth.ID(rico.hits.back().first);
  int b=truth.ID(rico.hits.end()[-2].first);
  if (a==2 || b==2) return false;
  if (a==0 || b==0) return true;
  if (a==b) return true;
  else return false;
}
bool valuta_track(track &rico, MCTruth &truth){
  if (rico.hits[1].second.second-rico.hits[0].second.second>61.) return false;
  int a=truth.ID(rico.hits[0].first);
  int b=truth.ID(rico.hits[1].first);
  int c=truth.ID(rico.hits.back().first);
  if (a==2 || b==2 || c==2) return false;
  if (!valuta_prod(rico,truth)) return false;
  if (c==0 || (a==0 && b==0)) return true;
  if (a==c || b==c) return true;
  else return false;
}

int main(int argc,char* argv[]){
  std::clock_t start=std::clock();
  int run=100;
  if(argc>1){
    char *a=argv[1];
    run=atoi(a);
  }
  int ang=0;
  if (argc>2) {
    char *a=argv[2];
    ang=atoi(a);
  }
  TRandom3* rnd=new TRandom3();
  //GRAFICI
  TH2D* vert=new TH2D("vert","Vertice XY Ricostruito",2000,-1,1,2000,-1,1);
  TH2D* vertvero=new TH2D("vertvero","Vertice XY Vero",2000,-.5,.5,2000,-.5,.5);
  TH2D* vert2=new TH2D("vert2","Vertice XY Ricostruito",2000,-25,25,2000,-25,25);
  // TH1D* vistaX=new TH1D("vistaX","vista x",2000,-2,2);
  // TH1D* vistaY=new TH1D("vistaY","vista y",2000,-1,2);
  // TH1D* errvertZ=new TH1D("errvertZ","Scarto Z vertici ZX-ZY",200,0,300);
  TH1D* errvert=new TH1D("errvert","Scarto tra i vertici ricostruiti e reali",200,-100,100);
  TH2D* trksS=new TH2D("trksS","Tracce vista S",100,-800,1200,100,-300,300);
  TH2D* trksK=new TH2D("trksK","Tracce vista K",100,-800,1200,100,-300,300);
  TH2D* corr=new TH2D("corr","Studio correlazione rico z e xy",200,-150,150,200,-4,4);
  TH2D* corrV=new TH2D("corrV","Studio correlazione rico z e xy negli eventi a vertice singolo",200,-150,150,200,-4,4);
  TH2D* PSF=new TH2D("psf","PSF;#theta_{x} (rad);#theta_{y} (rad)",2000,-1,1,2000,-1,1);
  TH1D* polPSF=new TH1D("polpsf","Polar PSF",1000,0,1);
  
  TH1D* sig=new TH1D("sig","Intensità segnale cluster isolati",200,0,400);
  TH1D* sige=new TH1D("sige","Intensità segnale cluster elettrone puro",200,0,400);
  TH1D* sigp=new TH1D("sigp","Intensità segnale cluster positrone puro",200,0,400);
  TH1D* sigm=new TH1D("sigm","Intensità segnale cluster misto",2000,0,5000);
  TH1D* sigv=new TH1D("sigv","Intensità segnale cluster vertice",200,0,400);
  TH1D* sign=new TH1D("sign","Intensità segnale cluster rumore",400,0,200);
  TH1D* noise=new TH1D("noise","cluster di rumore per evento",50,0,50);
  //  sigm->SetCanExtend(TH1::kAllAxes);
  noise->SetCanExtend(TH1::kAllAxes);
  TH1D* ppclus=new TH1D("ppclus","cluster sul layer di produzione",5,0,5);
  TH1D* lene=new TH1D("lene","Lunghezza cluster elettrone",10,0,10);
  TH1D* lenp=new TH1D("lenp","Lunghezza cluster positrone",10,0,10);
  TH1D* lenm=new TH1D("lenm","Lunghezza cluster misto",10,0,10);
  TH1D* lenv=new TH1D("lenv","Lunghezza cluster vertice",10,0,10);

  TH2D* hMCSx=new TH2D("mcsx","",100,0,100/*run*/,200,0,0.1);
  TH2D* hMCSy=new TH2D("mcsy","",100,0,100/*run*/,200,0,0.1);
  TH2D* hMCS2=new TH2D("mcs2","",100,0,100/*run*/,100,0,0.1);
  TH2D* h=new TH2D("nonsocomechiamarlo","",200,0,0.1,200,0,0.1);
  //  TH2D* source=new TH2D("source","Posizione Sorgente",2000,-2,2,2000,-2,2);
  TH1D* Ang=new TH1D("Ang","angle distribution",800,-0.05,0.05);


  TObjArray* trkarr=new TObjArray();

  TFile* mcf=new TFile(Form("../Decode/RootData/run_%06d.root",run),"READ");
  TTree* mct=(TTree*)mcf->Get("t4");
  TTree* mctt=(TTree*)mcf->Get("hitTree");
  
  TString align_filename = "../Decode/alignment_mc.dat";
  TString gaincorrection_filename = "../Decode/gaincorrection.dat";
  
  printf("---------------------------------------------\n");
  Event::ReadAlignment(align_filename);
  Event::ReadGainCorrection(gaincorrection_filename);
  printf("---------------------------------------------\n");

  int nEvs;
  Event* ev=new Event();
  Cluster* cc;
  int nCl;
  pair<double,double> vx,vy;
  pair<double,double> posV;
  pair<double,double> dirV;

  int nHits;
  int chxy[MAXNHITS];
  int hVol[MAXNHITS];
  int pID[MAXNHITS];
  int PDG[MAXNHITS];
  int parID[MAXNHITS];
  //  fill(pID,pID+100,0);
  double xCoord[MAXNHITS];
  double yCoord[MAXNHITS];
  double zCoord[MAXNHITS];
  double xMom[MAXNHITS];
  double yMom[MAXNHITS];
  double zMom[MAXNHITS];
  int hitStrips[MAXNHITS];
  vector<int>* hitChan;
  TVector3 TrueVert;
  int pptdr;
  
  mctt->SetBranchAddress("nTotalHits",&nHits);
  mctt->SetBranchAddress("chXY",&chxy);
  mctt->SetBranchAddress("hVol",&hVol);
  //  mctt->SetBranchAddress("pID",&pID);
  mctt->SetBranchAddress("PDG",&PDG);
  mctt->SetBranchAddress("ParID",&parID);
  mctt->SetBranchAddress("xCoord",&xCoord);
  mctt->SetBranchAddress("yCoord",&yCoord);
  mctt->SetBranchAddress("zCoord",&zCoord);
  mctt->SetBranchAddress("xMom",&xMom);
  mctt->SetBranchAddress("yMom",&yMom);
  mctt->SetBranchAddress("zMom",&zMom);
  mctt->SetBranchAddress("hitStrips",&hitStrips);
  //  mctt->SetBranchAddress("hitChan",&hitChan);
  
  // printf("pippo\n");
  mct->SetBranchAddress("cluster_branch",&ev);
  nEvs = mct->GetEntries();
  bool tracciato=0;

  int conta_rico=0;
  int conta_prod=0;
  int conta_exit=0;
  int conta_track=0;
  int ricombinati=0;
  int imp2ricom=0;
  int conta_vert=0;
  int giuste=0;
  for(int Ev=0; Ev<nEvs; Ev++){
    ev->Clear();
    vx=make_pair(9999.,9999.);
    vy=make_pair(9999.,9999.);
    nCl=0;
    tracciato=false;
    printf("+----------------------------+\n");
    printf("|+++ ANALISI EVENTO N° %d +++|\n",Ev);    
    printf("+----------------------------+\n");
    mct->GetEntry(Ev);
    mctt->GetEntry(Ev);
    printf("Via!!\n");
    //cout<<hitChan[0].size()<<" questo qua -------------------------"<<endl;
    if(0){
    for (int j=0;j<54;j++){
      printf("\n%d",j);
      printf("\nchXY:");
      for (int i=0;i<nHits;i++){
	if (hVol[i]==j)
	  printf("%d, ",chxy[i]);
      }
      int ncl=ev->GetNClusTot();
      printf("\ncls.address:");
      for (int i=0;i<ncl;i++){
	if (ev->GetCluster(i)->GetTDR()==j)
	  printf("%d, ",ev->GetCluster(i)->GetAddress());
      }
    }
    }
    for(int i=0;i<nHits;i++) pID[i]=(parID[i]==1)?PDG[i]:2;
    MCTruth truth=MCTruth(ev,nHits,chxy,hVol,pID,xMom,yMom,zMom);
    //printf("x=%f, y=%f\n",xCoord[0],yCoord[0]);
    TrueVert=TVector3(xCoord[0],yCoord[0],zCoord[0])*10;
    //printf("x=%f, y=%f\n",TrueVert[0],TrueVert[1]);
    pptdr=hVol[0];
    TGraph* grS1=new TGraph();
    TGraph* grS2=new TGraph();
    TGraph* grK1=new TGraph();
    TGraph* grK2=new TGraph();
    TMultiGraph* mgS=new TMultiGraph();
    TMultiGraph* mgK=new TMultiGraph();
    nCl=ev->GetNClusTot();
    cout<<nCl<<endl;
    int noise_cluster=0;
    int cctdr;
    int pp_cluster=0;
    bool merged=false;
    for(int ic=0;ic<nCl;ic++){
      cc=ev->GetCluster(ic);
      cctdr=cc->GetTDR();
      double z=ev->GetAlignPar(0,cctdr,2);
      double x=cc->GetAlignedPositionMC();
      // printf("tdr=%d\nx%d=%f\n",cctdr,ic,x);
      // cout<<"z"<<ic<<"="<<z<<endl;
      if(cc->side){ //1->K
	trksK->Fill(z,x);
	if(truth.ID(ic)==11 || truth.ID(ic)==0) grK1->SetPoint(grK1->GetN(),z,x);
	  //questo é un elettrone
	if(truth.ID(ic)==-11 || truth.ID(ic)==0) grK2->SetPoint(grK2->GetN(),z,x);
	  //questo é un positrone
      }
      else { //0->S
	trksS->Fill(z,x);
        if(truth.ID(ic)==11 || truth.ID(ic)==0) grS1->SetPoint(grS1->GetN(),z,x);
	  //questo é un elettrone
	if(truth.ID(ic)==-11 || truth.ID(ic)==0) grS2->SetPoint(grS2->GetN(),z,x);
	  //questo é un positrone
      }
      merged=(merged || ((cctdr==20 || cctdr==21) && truth.ID(ic)==0));
      if(  (ic!=0) && (cctdr<NTDRS) && (cctdr!=ev->GetCluster(ic-1)->GetTDR()) &&
      	  ((ic==nCl-1) || (cctdr!=ev->GetCluster(ic+1)->GetTDR())) )
      	sig->Fill((cc->GetTotSig()));
      if (truth.ID(ic)==0 && ic!=0){
      	sigm->Fill(cc->GetTotSig());
      	lenm->Fill(cc->GetLength());
      }
      // if (truth.ID(ic)==0 && ic==0){
      	
      // }
      if (truth.ID(ic)==11){
      	sige->Fill(cc->GetTotSig());
      	lene->Fill(cc->GetLength());
      }
      if (truth.ID(ic)==-11){
      	sigp->Fill(cc->GetTotSig());
      	lenp->Fill(cc->GetLength());
      }
      if (truth.ID(ic)==-2){
      	sign->Fill(cc->GetTotSig());
      	noise_cluster++;
      }
      if(cctdr==pptdr && truth.ID(ic)!=-2){
      	pp_cluster++;
      	sigv->Fill(cc->GetTotSig());
      	lenv->Fill(cc->GetLength());
      }
    }
    noise->Fill(noise_cluster);
    ppclus->Fill(pp_cluster);

    // int Strips=0;
    // for(int i=0;i<nHits;i++) Strips+=hitStrips[i];
    // // if (Strips==hitChan[0].size()) giuste++;
    // // printf("la somma delle hitStrips è %d, mentre hitChan è lungo %lu\n",Strips,hitChan[0].size());
    tracciato=ev->FindTracksAndVertex(0);
    //    if(ang!=0 && ev->GetNTracks()==2) ev->RecombineXY(ang);
    if(tracciato /*&& ev->GetNTracks()>1*/){
      printf("EVENTO TRACCIATO\n");

      vx=ev->GetVertexK();
      vy=ev->GetVertexS();

      posV=make_pair(vx.first,vy.first);
      vert->Fill(vx.first,vy.first);
      
      vertvero->Fill(xCoord[0],yCoord[0]);
      vertvero->Fill(xCoord[1],yCoord[1]);

      vert2->Fill(vx.first,vy.first);
      errvert->Fill(vx.second-TrueVert[2]);
      errvert->Fill(vy.second-TrueVert[2]);
      if (ev->GetCluster(0)->side)
	corr->Fill(vx.second-TrueVert[2],vx.first);
      else
	corr->Fill(vy.second-TrueVert[2],vy.first);
      //--------------------------//

      if(ev->GetNTracks()==2){
	grS1->SetMarkerColor(2);//colore
	grS1->SetMarkerStyle(2);
	grS1->SetLineWidth(0);
	grS2->SetMarkerColor(3);
	grS2->SetMarkerStyle(2);
	grS2->SetLineWidth(0);
	mgS=GraphHits( ev->GetTracks(0) );
	mgS->Add(grS1);
	mgS->Add(grS2);
	mgS->SetNameTitle(Form("Sgr%d",Ev),"proiezione S hit;z;y");
	trkarr->Add(mgS);
	// {
	//   pair<double,double> dir1=Event::Hough(ev->GetTracks(0)->at(0).hits);
	//   pair<double,double> dir2=Event::Hough(ev->GetTracks(0)->at(1).hits);
	//   TCanvas c1;
	//   TF1 *f1=new TF1("f1","tan([0]*x+[1]/cos([0]))");
	//   TF1 *f2=new TF1("f2","tan([0]*x+[1]/cos([0]))");
	//   f1->SetParameters(dir1.first,dir1.second);
	//   f2->SetParameters(dir2.first,dir2.second);
	//   mgS->Draw();
	//   f1->Draw("same");
	//   f2->Draw("same");
	//   getchar();
	//   delete f1;
	//   delete f2;
	// }
	grK1->SetMarkerColor(2);
	grK1->SetMarkerStyle(2);
	grK1->SetLineWidth(0);
	grK2->SetMarkerColor(3);//colore
	grK2->SetMarkerStyle(2);
	grK2->SetLineWidth(0);
	mgK=GraphHits( ev->GetTracks(1) );
	mgK->Add(grK1);
	mgK->Add(grK2);
	mgK->SetNameTitle(Form("Kgr%d",Ev),"proiezione K hit;z;x");
	trkarr->Add(mgK);
      
      //--------------------------//
	bool raccordo=(truth.ID(ev->GetTracks(0)->at(0).hits.back().first) ==
		       truth.ID(ev->GetTracks(1)->at(0).hits.back().first) &&
		       truth.ID(ev->GetTracks(0)->at(1).hits.back().first) ==
		       truth.ID(ev->GetTracks(1)->at(1).hits.back().first) );
	
	bool imp= (abs(truth.ID(ev->GetTracks(0)->at(0).hits.back().first)) != 11 ||
		   abs(truth.ID(ev->GetTracks(1)->at(0).hits.back().first)) != 11 ||
		   abs(truth.ID(ev->GetTracks(0)->at(1).hits.back().first)) != 11 ||
		   abs(truth.ID(ev->GetTracks(1)->at(1).hits.back().first)) != 11 );
	if (imp) { imp2ricom++;}
	if (raccordo) {ricombinati++;cout<<"ricombinati bene a quanto pare\n";}
	double a[4],amom[2];
	int s[4];
	double dth[4]={0.};
	bool exit=true;
	bool track=true;
	for(int side=0;side<4;side++){
	  conta_rico++;
	  if (valuta_prod(ev->GetTracks(side/2)->at(side%2),truth)) conta_prod++;
	  exit=(/*exit &&*/ (valuta_exit(ev->GetTracks(side/2)->at(side%2),truth)));
	  if (exit) conta_exit++;
	  track=(track && (valuta_track(ev->GetTracks(side/2)->at(side%2),truth)));
	  if (track) conta_track++;
	  //if (merged) {cout<<"merged!!!!\n";getchar();}
	  //}
	  //for (int p=0;p<2;p++){
	  auto hits=ev->GetTracks(side/2)->at(side%2).hits;
	  //auto hits2=ev->GetTracks(1)->at(raccordo?p:(1-p)).hits;
	  double Mom=truth.Mom(hits.back().first).Mag();
	  //double mom=truth.Mom(hits.back().first).Perp(TVector3(!side/2,side/2,0));
	  //	  if(Mom<=1)getchar();
	  //a[side]=ev->GetTracks(side/2)->at(side%2).prod_angle;
	  if(side<2) amom[side]=Mom;
	  s[side]=hits.size();
	  //s2=hits2.size();
	  if(s[side]<3) continue;
	  for(int i=0;i<s[side]-2;i++){
	    double th1=atan((hits[i+1].second.first-hits[i].second.first) /
			    (hits[i+1].second.second-hits[i].second.second));
	    double th2=atan((hits[i+2].second.first-hits[i+1].second.first) /
			    (hits[i+2].second.second-hits[i+1].second.second));
	    dth[side]+=pow((th1-th2),2);
	    Ang->Fill(pow(th2-th1,2));
	  }
	  //dth/=(s-2);
	  //dth=sqrt(dth);
	  a[side]=sqrt(dth[side]/(s[side]-2));
	}
	int s1=s[0]+s[raccordo?2:3];
	int s2=s[1]+s[raccordo?3:2];
	double Dth1=sqrt((dth[0]+dth[raccordo?2:3])/(s1-4));
	double Dth2=sqrt((dth[1]+dth[raccordo?3:2])/(s2-4));
	// int s1=s[0]+s[2];
	// int s2=s[1]+s[3];
	// double Dth1=sqrt((dth[0]+dth[2])/(s1-4));
	// double Dth2=sqrt((dth[1]+dth[3])/(s2-4));
	////2*amom[0]*amom[0]*(1/sqrt(1+amom[0]*amom[0]*4))*
	if(s1>3 && track) {
	  hMCSy->Fill(amom[0],Dth1);
	  h->Fill(a[0],a[raccordo?2:3]);
	}
	if(s2>3 && track) {
	  hMCSy->Fill(amom[1],Dth2);
	  h->Fill(a[1],a[raccordo?3:2]);
	}
	// 
	// 
	//h->Fill(a[0],a[2]);
        //h->Fill(a[1],a[3]);

	hMCSx->Fill(amom[0],Dth1);
	hMCSx->Fill(amom[1],Dth2);
	
	if(s1>3 && amom[0]<10 && Dth1<0.02){
	  printf("%f + %f = %f\n",amom[0],amom[1],amom[0]+amom[1]);
	  //getchar();
	}
	if(s2>3 && amom[1]<10 && Dth2<0.02){
	  printf("%f + %f = %f\n",amom[0],amom[1],amom[0]+amom[1]);
	  //getchar();
	}
	//	hMCS2->Fill(Mom,dth);

	//if (!raccordo && !imp && !merged && exit) getchar();

	//h->Fill(amom[0],TMath::Abs(a[0]*a[raccordo?2:3]));
	//h->Fill(amom[1],TMath::Abs(a[1]*a[raccordo?3:2]));

	//PSF
	double th1x=ev->GetTracks(0)->at(0).prod_angle,
	  //th1y=ev->GetTracks(1)->at(raccordo?0:1).prod_angle,
	  th1y=ev->GetTracks(1)->at(0).prod_angle,
	  th2x=ev->GetTracks(0)->at(1).prod_angle,
	  //th2y=ev->GetTracks(1)->at(raccordo?1:0).prod_angle;
	  th2y=ev->GetTracks(1)->at(1).prod_angle;
	double p1=truth.Mom(ev->GetTracks(0)->at(0).hits.back().first).Mag();
	double p2=truth.Mom(ev->GetTracks(0)->at(1).hits.back().first).Mag();
	p1 = 1/(rnd -> Gaus( 1/p1, 1/(5*p1) ));
	p2 = 1/(rnd -> Gaus( 1/p2, 1/(5*p2) ));
	double denom1=TVector3(tan(th1x),tan(th1y),1).Mag(),
	  denom2=TVector3(tan(th2x),tan(th2y),1).Mag();
	double p1x=p1*tan(th1x)/denom1,
	  p1y=p1*tan(th1y)/denom1,
	  p2x=p2*tan(th2x)/denom2,
	  p2y=p2*tan(th2y)/denom2;
	double Etot=sqrt(p1*p1+0.25)+sqrt(p2*p2+0.25);
	double px=(p1x+p2x)/Etot,
	  py=(p1y+p2y)/Etot;
	PSF->Fill(px,py);
	polPSF->Fill(sqrt(px*px+py*py));
	dirV=make_pair(px,py);

	// double hz=1000;
	// source->Fill((-hz*tan(dirV.first)+posV.first)/hz,(-hz*tan(dirV.second)+posV.second)/hz);
	// double mom[2];
	// TVector3 vers[2]={TVector3(1,0,0),TVector3(0,1,0)};
	// //Al primo giro del loop ricostruiamo la vista x, al secondo la vista y
	// for(int side=0;side<2;side++) {
	//   //proiettiamo il vero momento dell'ultimo cluster delle tracce sulla vista in esame
	//   double p1=truth.Mom(ev->GetTracks(side)->at(0).hits[0].first).Perp(vers[1-side]);
	//   double p2=truth.Mom(ev->GetTracks(side)->at(1).hits[0].first).Perp(vers[1-side]);
	//   p1 = 1/(rnd -> Gaus( 1/p1, 1/(5*p1) ));
	//   p2 = 1/(rnd -> Gaus( 1/p2, 1/(5*p2) ));
	//   //useremo il momento totale per normalizzare le componenti
	//   double p=(truth.Mom(ev->GetTracks(side)->at(0).hits[0].first) +
	// 	    truth.Mom(ev->GetTracks(side)->at(1).hits[0].first) ).Mag();
	//   p = 1/(rnd -> Gaus( 1/p, 1/(5*p) ));
	//   //sommiamo e normalizziamo per ottenere la componente del versore del fotone
	//   mom[side] = (p1*sin(ev->GetTracks(side)->at(0).prod_angle) +
	// 	       p2*sin(ev->GetTracks(side)->at(1).prod_angle)) / p;
	// }
	// PSF->Fill(mom[0],mom[1]);
      } 
      if (ev->GetCluster(0)->GetTDR()!=ev->GetCluster(1)->GetTDR()){
      	conta_vert++;
      	if (ev->GetCluster(0)->side)
      	  corrV->Fill(vx.second-TrueVert[2],vx.first-0.3);
      	else
      	  corrV->Fill(vy.second-TrueVert[2],vy.first-0.3);
      }
    }
    else {
      delete grS1;
      delete grK1;
      delete grS2;
      delete grK2;
    }
    
  }
  delete mct;
  delete mctt;
  delete mcf;

  TH1D *vertX,*vertY,*projs;//=new TH1D("projs","Proiezioni vertice",2000,-25,25);
  // vertX=(TH1D*)PSF->ProjectionX();
  // vertY=(TH1D*)PSF->ProjectionY();
  // projs->Add(vertX,vertY);
  // TF1* gg=new TF1("gg","[1]*TMath::Exp(-0.5*TMath::Power((x-[0])/[2],2)) / ([2]*TMath::Sqrt(2*TMath::Pi()))+[3]*TMath::Exp(-0.5*TMath::Power((x-[0])/[4],2)) / ([4]*TMath::Sqrt(2*TMath::Pi()))");
  // gg->SetParNames("#mu","c_{1}","#sigma_{1}","c_{2}","#sigma_{2}");
  // gg->SetParameter(0,0);
  // // gg->SetParameter(1,168);
  // gg->SetParameter(2,.03);
  // // gg->SetParameter(3,2800);
  // gg->SetParameter(4,.007);
  // //printf("integrale prima del fit %f\n",gg->Integral(-100,100));
  // projs->Fit("gg");
  // //printf("integrale dopo del fit %f\n",gg->Integral(-100,100));
  // //printf("integrale dell'istogramma %f\n",projs->Integral(1,200,"width"));

  double frac=0.68;
  double ang_resol=PolarQuantile(PSF,frac);
  cout<<"La risoluzione angolare ottenuta in cartesiane è "<<ang_resol<<" radianti\n";
  int Tot=polPSF->GetEntries();
  int bin=1;
  double Int;
  for (;bin<1000;bin++){
    Int=polPSF->Integral(1,bin);
    if (Int/Tot > frac) break;
  }
  ang_resol=polPSF->GetBinCenter(bin);
  double err_frac=sqrt(frac*(1-frac)/Tot);
  cout<<"errore sul conteggio è "<<err_frac<<endl;
  double err_res=ang_resol*err_frac/(2*(frac-1)*log(1-frac));
  cout<<"La risoluzione angolare ottenuta in polari è "<<ang_resol<<"+-"<<err_res<<" radianti\n";

  // double source_ang_resol=PolarQuantile(source,frac);
  // cout<<"La risoluzione angolare ottenuta dalla proiezione verso la sorgente è "<<source_ang_resol<<" radianti\n";

  // TF1* polg=new TF1("polg","[0]*exp(-x*x/(2*[1]*[1]))");
  // polg->SetParNames("c","#sigma");
  // polPSF->Fit("polg");


  const int n_out=8;
  TString etichette[n_out]={"Conversion",
			    "Reconstruction",
			    "#splitline{Head of}{the track}",
			    "#splitline{Tail of}{the track}",
			    "#splitline{Link}{head-to-tail}",
			    "Angular resolution",
			    "Recombination efficiency",
			    "Angular resolution incertainty"};

  double val[n_out]={(double)nEvs/1e6,
		     (double)conta_rico/(4*nEvs),
		     (double)conta_prod/conta_rico,
		     (double)conta_exit/conta_rico,
		     (double)conta_track/conta_rico,
		     4.*ricombinati/conta_rico,
		     ang_resol,err_res};
  TH1D* eff=new TH1D("eff","Prestazioni",n_out,0,n_out);
  eff->SetStats(0);
  for(int i=0;i<n_out;i++){
    eff->GetXaxis()->SetBinLabel(i+1,etichette[i]);
    eff->SetBinContent(i+1,val[i]);
  }
  printf("-------------------------\n");
  printf("Ricostruite %d tracce\n",conta_rico);
  printf("Efficienza ricostruzione direzione di produzione: %f\n",val[2]);
  printf("Efficienza ricostruzione direzione di uscita: %f\n",val[3]);
  printf("Efficienza ricostruzione traccia: %f\n",val[4]);
  printf("Eventi a vertice singolo: %d\n",conta_vert);
  printf("Eventi ricombinati correttamente: %d su %d -> %f\n",ricombinati,conta_rico/4,4.*ricombinati/conta_rico);
  printf("Eventi impossibili da ricombinare: %d su %d -> %f\n",imp2ricom,conta_rico/4,4.*imp2ricom/conta_rico);
  
  printf("-------------------------\n");

  cout<<"giuste = "<<giuste<<endl;
  
  TFile* f1=new TFile(Form("out/QAout%d.root",run),"RECREATE");
  trkarr->Write("tracce",1);
  eff->Write();
  
  vert->Write("vertXY");
  vertvero->Write("vertVero");
  vert2->Write("vertXYlargo");
  // vertX->Write("vx");
  // vertY->Write("vy");
  // projs->Write("proj");

  // vistaX->Write();
  // vistaY->Write();
  
  errvert->Write("scartiZ");
  
  corr->Write("corr");
  corrV->Write("corrV");

  PSF->Write();
  polPSF->Write();

  //  source->Write("source");
  
  hMCSx->Write("hMCSx");
  hMCSy->Write("hMCSy");
  hMCS2->Write("hMCS2");
  h->Write("bo");
  Ang->Write("Ang");
  
  trksS->Write("TracceY");
  trksK->Write("TracceX");

  sig->Write();
  sige->Write();
  sigp->Write();
  sigm->Write();
  sigv->Write();
  sign->Write();
  noise->Write();
  ppclus->Write();
  lene->Write();
  lenp->Write();
  lenm->Write();
  lenv->Write();

  f1->Close();

  trkarr->Delete();
  
  delete trkarr;
  delete ev;
  delete vert;
  delete errvert;
  delete f1;
  cout<<"tempo impiegato = "<<(std::clock()-start)/CLOCKS_PER_SEC<<" s\n";
}
