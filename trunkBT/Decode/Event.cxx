#include "Event.hh"
#include "Cluster.hh"
#include "TMinuit.h"
#include "TH1F.h"
#include "TMath.h"
#include <unistd.h>
#include <iostream>

using namespace std;

#include <cmath>

//#define USEMINUIT

ClassImp(Event);

bool Event::alignmentnotread=true;
float Event::alignpar[NJINF][NTDRS][3];
bool Event::multflip[NJINF][NTDRS];

bool Event::gaincorrectionnotread=true;
float Event::gaincorrectionpar[NJINF][NTDRS][NVAS][3];

//they store temporarily the result of the fit----------------------------
double mX_sf, mXerr_sf;
double mY_sf, mYerr_sf;
double iDirX_sf, iDirXerr_sf;
double iDirY_sf, iDirYerr_sf;
double iDirZ_sf, iDirZerr_sf;
double theta_sf, thetaerr_sf;
double phi_sf, phierr_sf;
double X0_sf, X0err_sf;
double Y0_sf, Y0err_sf;
double chisq_sf;
double chisqx_sf;
double chisqy_sf;
std::vector<std::pair<int, std::pair<double, double> > > v_trackS_sf;
std::vector<std::pair<int, std::pair<double, double> > > v_trackK_sf;
std::vector<double> v_chilayS_sf;
std::vector<double> v_chilayK_sf;
//-------------------------------------------------------------------------

static double _compchisq(std::vector<std::pair<int, std::pair<double, double> > > vec, std::vector<double>& v_chilay, double iDir, double iX, double iXerr, double Z0=0);
static Double_t _func(double z, double imX, double iX, double Z0=0);
#ifdef USEMINUIT
static void _fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
#endif

Event::Event(){

  Evtnum=0;
  JINJStatus=0;
  for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;
  
  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<NVAS;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0;
  }
  
  for(int kk=0;kk<8;kk++) {
    for(int ii=0;ii<1024;ii++) {
      CalSigma[kk][ii]=0.0;
      CalPed[kk][ii]=0.0;
      RawSignal[kk][ii]=0;
      RawSoN[kk][ii]=0.0;
    }
  }
  
  //  RawLadder = new TClonesArray("RawData", NJINF*8);//NJINFS*8 is the maximum number of ladder in raw mode that can me read by a single jinf.
  
  NClusTot=0;
  notgood=0;
  Cls = new TClonesArray("Cluster", NJINF*NTDRS);//if more than NJINFS*NTDRS anyhow the array will be expanded
  Cls->SetOwner();

  if (alignmentnotread) ReadAlignment("alignment.dat");

  if (gaincorrectionnotread) ReadGainCorrection("gaincorrection.dat");

  ClearTrack();
  
  return;
}

Event::~Event(){
  if(Cls) {Cls->Delete(); delete Cls;}  
}

void Event::Clear(){
  JINJStatus=0;
  for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;
  
  NClusTot=0;

  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<NVAS;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0; 
  }

  for(int ii=0;ii<8;ii++){ 
    for(int kk=0;kk<1024;kk++) {
      CalSigma[ii][kk]=0.0;
      CalPed[ii][kk]=0.0;
      RawSignal[ii][kk]=0;
      RawSoN[ii][kk]=0.0;
    }
  }
  
  if(Cls) Cls->Delete();
  
  //   for (int ii=Cls->GetEntries();ii>-1;ii--){
  //     Cluster* ff=(Cluster*) Cls->RemoveAt(ii);
  //     if(ff) delete ff;
  //   }

  ClearTrack();
  
  return;
}

Cluster* Event::AddCluster(int lad,int side){
  Cluster* pp=(Cluster*)Cls->New(NClusTot);
  
  NClus[lad][side]++;
  NClusTot++;
  return pp;
}


Cluster* Event::GetCluster(int ii){
  return (Cluster*)Cls->At(ii);
}

/*
int Event::NGoldenClus(int lad, int side){
  int num=0;
  for (int ii=0;ii<NClusTot;ii++){
    Cluster* cl=GetCluster(ii);
    if(cl->ladder==lad&&cl->side==side&&cl->golden==1) num++;
  }
  return num;
}
*/

void Event::ReadAlignment(TString filename, bool DEBUG){
  
  printf("Reading alignment from %s:\n", filename.Data());

  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      for (int cc=0; cc<3; cc++) {
	alignpar[jj][tt][cc]=0.0;
      }
      multflip[jj][tt]=false;
    }
  }
  
  int const dimline=255;
  char line[dimline];
  float dummy;
  int dummyint;
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
	  sscanf(line, "%d\t%d\t%f\t%f\t%f\t%d", &jinfnum, &tdrnum, &dummy, &dummy, &dummy, &dummyint);
	  if (jinfnum<NJINF && tdrnum<NTDRS) {
	    sscanf(line,"%d\t%d\t%f\t%f\t%f\t%d", &jinfnum, &tdrnum, &alignpar[jinfnum][tdrnum][0], &alignpar[jinfnum][tdrnum][1], &alignpar[jinfnum][tdrnum][2], &dummyint);
	    multflip[jinfnum][tdrnum]=(bool)(dummyint);
	  }
	  else {
	    printf("Wrong JINF/TDR (%d, %d): maximum is (%d,%d)\n", jinfnum, tdrnum, NJINF, NTDRS);
	  }
	}
      }
      else {
	printf(" closing alignment file \n");
	  fclose(ft);
	  break;
      }
    }
  }

  alignmentnotread=false;

  if(DEBUG==false) return;

  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      for (int cc=0; cc<3; cc++) {
	if (cc==0) printf("JINF %02d TDR %02d)\t", jj, tt);
	printf("%f\t", alignpar[jj][tt][cc]);
	if (cc==2) printf("%d\n", (int)(multflip[jj][tt]));
      }
    }
  }


  
  return;
}
void Event::ReadGainCorrection(TString filename, bool DEBUG){
  
  printf("Reading Gain Correction from %s:\n", filename.Data());

  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      for (int vv=0; vv<NVAS; vv++) {
	for (int cc=0; cc<3; cc++) {
	  gaincorrectionpar[jj][tt][vv][cc]=-9999.0;
	}
      }
    }
  }
  
  int const dimline=255;
  char line[dimline];
  int jinfnum=0;
  int tdrnum=0;
  int vanum=0;
  float dummy=0.;
  
  FILE* ft = fopen(filename.Data(),"r");

  if(ft==NULL){ 
    printf("Error: cannot open %s , setting all gain corrections to 1 \n", filename.Data());
    for (int jj=0; jj<NJINF; jj++) {
      for (int tt=0; tt<NTDRS; tt++) {
	for (int vv=0; vv<NVAS; vv++) {
	  for (int cc=0; cc<3; cc++) {
	    gaincorrectionpar[jj][tt][vv][cc]=1;
	  }
	} 
      }
    }

    return;
  }
  else {
    while(1){
      if (fgets(line, dimline, ft)!=NULL) {
	if (*line == '#') continue; /* ignore comment line */
	else {
	  sscanf(line, "%d\t%d\t%d\t%f\t%f\t%f", 
		 &jinfnum, &tdrnum, &vanum, &dummy, &dummy, &dummy);
	  if (jinfnum<NJINF && tdrnum<NTDRS && vanum<NVAS ) {
	    sscanf(line,"%d \t %d \t %d \t %f \t %f \t %f", 
		   &jinfnum, &tdrnum, &vanum, 
		   &gaincorrectionpar[jinfnum][tdrnum][vanum][0],
		   &gaincorrectionpar[jinfnum][tdrnum][vanum][1],
		   &gaincorrectionpar[jinfnum][tdrnum][vanum][2]);
	  }
	  else {
	    printf("Wrong JINF/TDR/VA (%d, %d, %d): maximum is (%d,%d, %d)\n", jinfnum, tdrnum, vanum, NJINF, NTDRS, NVAS);
	  }
	}
      }
      else {
	printf(" closing gain correction file \n");
	fclose(ft);
	  break;
      }
    }
  }

  gaincorrectionnotread=false;

  if(DEBUG==false) return;
  
  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      for (int vv=0; vv<NVAS; vv++) {
	for (int cc=0; cc<3; cc++) {
	  if(gaincorrectionpar[jj][tt][vv][cc] == -9999) continue;
	  if (cc==0) printf("JINF %02d TDR %02d VA %02d)\t", jj, tt, vv);
	  printf("%f\t", gaincorrectionpar[jj][tt][vv][cc]);
	  if (cc==1) 	printf("\n");
	}
      }
    }
  }
  
  return;
}

float Event::GetGainCorrectionPar(int jinfnum, int tdrnum, int vanum, int component){
  if (jinfnum>=NJINF || jinfnum<0) {
    printf("Jinf %d: not possible, the maximum is %d...\n", jinfnum, NJINF-1);
    return -9999;
  }
  if (tdrnum>=NTDRS || tdrnum<0) {
    printf("TDR %d: not possible, the maximum is %d...\n", tdrnum, NTDRS-1);
    return -9999;
  }
  if (vanum>=NVAS || vanum<0) {
    printf("VA %d: not possible, the maximum is %d...\n", vanum, NVAS-1);
    return -9999;
  }
  if (component<0 || component >=3) {
    printf("Component %d not valid: it can be only up to 2\n", component);
    return -9999;
  }
  
  return gaincorrectionpar[jinfnum][tdrnum][vanum][component];
}

float Event::GetAlignPar(int jinfnum, int tdrnum, int component) {

  if (jinfnum>=NJINF || jinfnum<0) {
    printf("Jinf %d: not possible, the maximum is %d...\n", jinfnum, NJINF-1);
    return -9999;
  }
  if (tdrnum>=NTDRS || tdrnum<0) {
    printf("TDR %d: not possible, the maximum is %d...\n", tdrnum, NTDRS-1);
    return -9999;
  }
  if (component<0 || component >=3) {
    printf("Component %d not valid: it can be only up to 2\n", component);
    return -9999;
  }
  
  return alignpar[jinfnum][tdrnum][component];
}

float Event::GetMultiplicityFlip(int jinfnum, int tdrnum) {

  if (jinfnum>=NJINF || jinfnum<0) {
    printf("Jinf %d: not possible, the maximum is %d...\n", jinfnum, NJINF-1);
    return -9999;
  }
  if (tdrnum>=NTDRS || tdrnum<0) {
    printf("TDR %d: not possible, the maximum is %d...\n", tdrnum, NTDRS-1);
    return -9999;
  }
  
  return multflip[jinfnum][tdrnum];
}

void Event::ClearTrack(){

  _chisq = 999999999.9;
  _chisqx = 999999999.9;
  _chisqy = 999999999.9;
  
  _iDirX = -9999.9;
  _iDirY = -9999.9;
  _iDirZ = -9999.9;
  _iDirXerr = -9999.9;
  _iDirYerr = -9999.9;
  _iDirZerr = -9999.9;
  
  _mX = -9999.9;
  _mY = -9999.9;
  _mXerr = -9999.9;
  _mYerr = -9999.9;

  _theta = -9999.9;
  _phi = -9999.9;
  _thetaerr = -9999.9;
  _phierr = -9999.9;
  
  _X0 = -9999.9;
  _Y0 = -9999.9;
  _X0err = -9999.9;
  _Y0err = -9999.9;
  
  _v_trackS.clear();
  _v_trackK.clear();
  
  _v_chilayS.clear();
  _v_chilayK.clear();
  
  _v_trackhit.clear();
    
  for (int ii=0; ii<NJINF; ii++) {;
    for (int ss=0; ss<2; ss++) {
      _track_cluster_pattern[ii][ss]=0;
    }
  }

  return;
}

void Event::ClearTrack_sf(){

  chisq_sf = 999999999.9;
  chisqx_sf = 999999999.9;
  chisqy_sf = 999999999.9;
  
  iDirX_sf = -9999.9;
  iDirY_sf = -9999.9;
  iDirZ_sf = -9999.9;
  iDirXerr_sf = -9999.9;
  iDirYerr_sf = -9999.9;
  iDirZerr_sf = -9999.9;
  
  mX_sf = -9999.9;
  mY_sf = -9999.9;
  mXerr_sf = -9999.9;
  mYerr_sf = -9999.9;

  theta_sf = -9999.9;
  phi_sf = -9999.9;
  thetaerr_sf = -9999.9;
  phierr_sf = -9999.9;
  
  X0_sf = -9999.9;
  Y0_sf = -9999.9;
  X0err_sf = -9999.9;
  Y0err_sf = -9999.9;
  
  v_trackS_sf.clear();
  v_trackK_sf.clear();
  
  v_chilayS_sf.clear();
  v_chilayK_sf.clear();
  
  return;
}

bool Event::FindTrackAndFit(int nptsS, int nptsK, bool verbose) {

  ClearTrack();
  
  std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddS[NJINF][NTDRS];
  std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddK[NJINF][NTDRS];
  
  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {
    
    Cluster* current_cluster = GetCluster(index_cluster);
    
    int jinfnum = current_cluster->GetJinf();
    int tdrnum = current_cluster->GetTDR();

    int side=current_cluster->side;
    if (side==0) {
      v_cog_laddS[jinfnum][tdrnum].push_back(std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
    }
    else {
      v_cog_laddK[jinfnum][tdrnum].push_back(std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
    }
  }

  std::vector<std::pair<int, std::pair<double, double> > > vecS;//actually used just for compatibility with the telescopic function
  std::vector<std::pair<int, std::pair<double, double> > > vecK;//actually used just for compatibility with the telescopic function
  double chisq = CombinatorialFit(v_cog_laddS, v_cog_laddK, NJINF, NTDRS, vecS, vecK, nptsS, nptsK, verbose);
  //  printf("chisq = %f\n", chisq);
  
  bool ret = true;
  if (chisq>=999999999.9) ret =false;
  else if (chisq<-0.000000001) ret = false;
  
  return ret;
}

bool Event::FindHigherChargeTrackAndFit(int nptsS, double threshS, int nptsK, double threshK, bool verbose) {

  ClearTrack();

  std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddS[NJINF][NTDRS];
  std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddK[NJINF][NTDRS];

  std::vector<double> v_q_laddS[NJINF][NTDRS];
  std::vector<double> v_q_laddK[NJINF][NTDRS];

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    Cluster* current_cluster = GetCluster(index_cluster);
    
    int jinfnum = current_cluster->GetJinf();
    int tdrnum = current_cluster->GetTDR();

    int side=current_cluster->side;
    if (side==0) {
      if (current_cluster->GetTotSN()>threshS) {
	if (v_q_laddS[jinfnum][tdrnum].size()==0) {
	  v_cog_laddS[jinfnum][tdrnum].push_back(std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
	  v_q_laddS[jinfnum][tdrnum].push_back(current_cluster->GetCharge());
	}
	else {
	  if (current_cluster->GetCharge()>v_q_laddS[jinfnum][tdrnum][0]) {
	    v_cog_laddS[jinfnum][tdrnum][0] = std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2)));
	    v_q_laddS[jinfnum][tdrnum][0] = current_cluster->GetCharge();
	  }
	}
      }
    }
    else {
      if (current_cluster->GetTotSN()>threshK) {
	if (v_q_laddK[jinfnum][tdrnum].size()==0) {
	  v_cog_laddK[jinfnum][tdrnum].push_back(std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
	  v_q_laddK[jinfnum][tdrnum].push_back(current_cluster->GetCharge());
	}
	else {
	  if (current_cluster->GetCharge()>v_q_laddK[jinfnum][tdrnum][0]) {
	    v_cog_laddK[jinfnum][tdrnum][0] = std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2)));
	    v_q_laddK[jinfnum][tdrnum][0] = current_cluster->GetCharge();
	  }
	}
      }
    }
  }
  
  //let's use CombinatorialFit just for simplicity but the vector above are with at most one cluster per ladder
  std::vector<std::pair<int, std::pair<double, double> > > vecS;//actually used just for compatibility with the telescopic function
  std::vector<std::pair<int, std::pair<double, double> > > vecK;//actually used just for compatibility with the telescopic function
  double chisq = CombinatorialFit(v_cog_laddS, v_cog_laddK, NJINF, NTDRS, vecS, vecK, nptsS, nptsK, verbose);
  //  printf("chisq = %f\n", chisq);
  
  bool ret = true;
  if (chisq>=999999999.9) ret =false;
  else if (chisq<-0.000000001) ret = false;
  
  return ret;
}

double Event::CombinatorialFit(
			     std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddS[NJINF][NTDRS],
			     std::vector<std::pair<int, std::pair<double, double> > > v_cog_laddK[NJINF][NTDRS],
			     int ijinf, int itdr,
			     std::vector<std::pair<int, std::pair<double, double> > > v_cog_trackS,
			     std::vector<std::pair<int, std::pair<double, double> > > v_cog_trackK,
			     int nptsS, int nptsK, 
			     bool verbose
			     ){
  //  printf("ijinf = %d, itdr = %d\n", ijinf, itdr);
  
  if (itdr==0) {
    itdr=NTDRS;
    ijinf--;
  }
  
  if (ijinf!=0) {//recursion
    int sizeS = v_cog_laddS[ijinf-1][itdr-1].size();
    int sizeK = v_cog_laddK[ijinf-1][itdr-1].size();
    //    printf("size: %d %d\n", sizeS, sizeK);
    for (int ss=0; ss<std::max(sizeS, 1); ss++) {
      for (int kk=0; kk<std::max(sizeK, 1); kk++) {
	//	printf("ss=%d, kk=%d\n", ss, kk);
	std::vector<std::pair<int, std::pair<double, double> > > _vecS = v_cog_trackS;
	std::vector<std::pair<int, std::pair<double, double> > > _vecK = v_cog_trackK;
	if (sizeS>0) {
	  _vecS.push_back(v_cog_laddS[ijinf-1][itdr-1].at(ss));
	  if (verbose) printf("S_push_back: %f %f\n", v_cog_laddS[ijinf-1][itdr-1].at(ss).second.first, v_cog_laddS[ijinf-1][itdr-1].at(ss).second.second);
	}
	if (sizeK>0) {
	  _vecK.push_back(v_cog_laddK[ijinf-1][itdr-1].at(kk));
	  if (verbose) printf("K_push_back: %f %f\n", v_cog_laddK[ijinf-1][itdr-1].at(kk).second.first, v_cog_laddK[ijinf-1][itdr-1].at(kk).second.second);
	}
	CombinatorialFit(v_cog_laddS, v_cog_laddK, ijinf, itdr-1, _vecS, _vecK, nptsS, nptsK, verbose);
      }
    }
  }
  else {//now is time to fit!
    if (verbose) {
      printf("new track to fit\n");
      printf("S: ");
      for (int ss=0; ss<(int)(v_cog_trackS.size()); ss++) {
	printf("(%f,%f)", v_cog_trackS.at(ss).second.first, v_cog_trackS.at(ss).second.second);
      }
      printf("\n");
      printf("K: ");
      for (int kk=0; kk<(int)(v_cog_trackK.size()); kk++) {
	printf("(%f,%f)", v_cog_trackK.at(kk).second.first, v_cog_trackK.at(kk).second.second);
      }
      printf("\n");
    }
    if ((int)(v_cog_trackS.size())>=nptsS && (int)(v_cog_trackK.size())>=nptsK) {
      double chisq = SingleFit(v_cog_trackS, v_cog_trackK, verbose);
      if (chisq<_chisq) {
	if (verbose) printf("Best track) new chisq %f, old one %f\n", chisq, _chisq);
	AssignAsBestTrackFit();
      }
    }
    if (verbose) {
      printf("----------------------\n");
    }
  }
  
  return _chisq;
}

void Event::AssignAsBestTrackFit(){

  _chisq = chisq_sf;
  _chisqx = chisqx_sf;
  _chisqy = chisqy_sf;
  _mX = mX_sf;
  _mY = mY_sf;
  _mXerr = mXerr_sf;
  _mYerr = mYerr_sf;
  _iDirX = iDirX_sf;
  _iDirY = iDirY_sf;
  _iDirZ = iDirZ_sf;
  _iDirXerr = iDirXerr_sf;
  _iDirYerr = iDirYerr_sf;
  _iDirZerr = iDirZerr_sf;
  _theta = theta_sf;
  _phi = phi_sf;
  _thetaerr = thetaerr_sf;
  _phierr = phierr_sf;
  _X0 = X0_sf;
  _Y0 = Y0_sf;
  _X0err = X0err_sf;
  _Y0err = Y0err_sf;
  _v_trackS = v_trackS_sf;
  _v_trackK = v_trackK_sf;
  _v_chilayS = v_chilayS_sf;
  _v_chilayK = v_chilayK_sf;

  StoreTrackClusterPatterns();
  FillHitVector();
  
  return;  
}

double Event::SingleFit(
			std::vector<std::pair<int, std::pair<double, double> > > vS,
			std::vector<std::pair<int, std::pair<double, double> > > vK,
			bool verbose
			){

  ClearTrack_sf();
  
  /* debug
     static TH1F hchi("hchi", "hchi", 1000, 0.0, 10.0);
     static TH1F htheta("htheta", "htheta", 1000, -TMath::Pi()/2.0, TMath::Pi()/2.0);
     static TH1F hphi("hphi", "hphi", 1000, -TMath::Pi(), TMath::Pi());
     static TH1F hx0("hx0", "hx0", 1000, -1000.0, 1000.0);
     static TH1F hy0("hy0", "hy0", 1000, -1000.0, 1000.0); 
  */
  
  chisq_sf = SingleFit(vS, vK, v_chilayS_sf, v_chilayK_sf, theta_sf, thetaerr_sf, phi_sf, phierr_sf, iDirX_sf, iDirXerr_sf, iDirY_sf, iDirYerr_sf, iDirZ_sf, iDirZerr_sf, mX_sf, mXerr_sf, mY_sf, mYerr_sf, X0_sf, X0err_sf, Y0_sf, Y0err_sf, chisqx_sf, chisqy_sf, verbose);
  
  /*
    hchi.Fill(log10(chisq_sf));
    htheta.Fill(theta_sf);
    hphi.Fill(phi_sf);
    hx0.Fill(X0_sf);
    hy0.Fill(Y0_sf);
  */

  if (verbose) printf("chisq: %f, chisqx: %f, chisqy: %f, theta = %f, phi = %f, X0 = %f, Y0 = %f\n", chisq_sf, chisqx_sf, chisqy_sf, theta_sf, phi_sf, X0_sf, Y0_sf);
  
  return chisq_sf;
}

double Event::SingleFit(
			std::vector<std::pair<int, std::pair<double, double> > > vS,
			std::vector<std::pair<int, std::pair<double, double> > > vK,
			std::vector<double>& v_chilayS,
			std::vector<double>& v_chilayK,
			double& theta, double& thetaerr,
			double& phi, double& phierr,
			double& iDirX, double& iDirXerr,
			double& iDirY, double& iDirYerr,
			double& iDirZ, double& iDirZerr,
			double& mX, double& mXerr,
			double& mY, double& mYerr,
			double& X0, double& X0err,
			double& Y0, double& Y0err,
			double& chisqx, double& chisqy,
			bool verbose){

  v_trackS_sf = vS;
  v_trackK_sf = vK;

  Double_t corrXmX, corrYmY;
  
  //The fit is done independently in the two X and Y views
  //The fit returns X0 and mX (the angular coefficient)
  //mX = vx/vz, where vx and vz are the projection of the straight line versors into the X and Z axis
  //Considering the definition of directive cosines
  //dirX = vx / |v|   with |v| = sqrt( vx*vx + vy*vy + vz*vz) 
  //dirY = vy / |v|   with |v| = sqrt( vx*vx + vy*vy + vz*vz) 
  //dirZ = vZ / |v|   with |v| = sqrt( vx*vx + vy*vy + vz*vz)
  //dirX*dirX + dirY*dirY + dirZ*dirZ = 1
  //then we have
  //mX = dirX/dirZ
  //mY = dirY/dirZ
  //and after both two fits, we can calculate
  //dirZ = 1  / sqrt( 1 + mX*mX + mY*mY)
  //dirX = mX / sqrt( 1 + mX*mX + mY*mY)
  //dirY = mY / sqrt( 1 + mX*mX + mY*mY)
  
#ifdef USEMINUIT
  //Minuit fit 
  static TMinuit* minuit = NULL;
  if (!minuit) minuit = new TMinuit();
  //  minuit->Clear();
  minuit->SetPrintLevel((int)(verbose)-1);
  minuit->SetFCN(_fcn);
  
  Double_t arglist[10];
  Int_t ierflg = 0;
  arglist[0] = 1;//chi-sq err-def
  minuit->mnexcm("SET ERR", arglist, 1, ierflg);
  
  // Set starting values and step sizes for parameters
  static Double_t vstart[5] = {0.0, 0.0 , 0.0 , 0.0, 0.0};
  static Double_t step[5] =   {1.0e-5 , 1.0e-5 , 1.0e-5 , 1.0e-5};
  minuit->mnparm(0, "mX", vstart[0], step[0], 0, 0, ierflg);
  minuit->mnparm(1, "mY", vstart[1], step[1], 0, 0, ierflg);
  minuit->mnparm(2, "X0", vstart[2], step[2], 0,0, ierflg);
  minuit->mnparm(3, "Y0", vstart[3], step[3], 0,0, ierflg);
  
  // Now ready for minimization step
  arglist[0] = 50000;
  arglist[1] = 1.;
  minuit->mnexcm("MIGRAD", arglist, 2, ierflg);
  
  // Print results
  // Double_t amin,edm,errdef;
  // Int_t nvpar,nparx,icstat;
  // minuit->mnstat(amin, edm, errdef, nvpar, nparx, icstat);
  // minuit->mnprin(3,amin);
  
  minuit->GetParameter (0, mX, mXerr);
  minuit->GetParameter (1, mY, mYerr);
  minuit->GetParameter (2, X0, X0err);
  minuit->GetParameter (3, Y0, Y0err);
  
  Double_t covmat[4][4];
  minuit->mnemat(&covmat[0][0],4);
  corrXmX=-covmat[0][2]/(sqrt(covmat[0][0]*covmat[2][2])); //minus is because they shold be anticorrelated
  corrYmY=-covmat[1][3]/(sqrt(covmat[1][1]*covmat[3][3]));
#else
  //Analytical Fit
  
  //Fit X
  int nx = (int)(vS.size());
  Double_t S1=0;   for(int i=0; i<(int)nx; i++) S1  += 1./pow(Cluster::GetNominalResolution(0),2);
  Double_t Sz=0;   for(int i=0; i<(int)nx; i++) Sz  += vS.at(i).second.second/pow(Cluster::GetNominalResolution(0),2);
  Double_t Szz=0;  for(int i=0; i<(int)nx; i++) Szz += pow(vS.at(i).second.second,2)/pow(Cluster::GetNominalResolution(0),2);
  Double_t Sx=0;   for(int i=0; i<(int)nx; i++) Sx  += vS.at(i).second.first/pow(Cluster::GetNominalResolution(0),2);
  Double_t Szx=0;  for(int i=0; i<(int)nx; i++) Szx += (vS.at(i).second.first*vS.at(i).second.second)/pow(Cluster::GetNominalResolution(0),2);
  Double_t Dx = S1*Szz - Sz*Sz;
  X0 = (Sx*Szz-Sz*Szx)/Dx;
  //iDirX = (S1*Szx-Sz*Sx)/Dx; iDirX=-iDirX;
  mX = (S1*Szx-Sz*Sx)/Dx; //mX=-mX;
  X0err = sqrt(Szz/Dx);
  //iDirXerr = sqrt(S1/Dx);
  mXerr = sqrt(S1/Dx);
  corrXmX = -Sz/sqrt(Szz*S1);
  
  //Fit Y
  int ny = (int)(vK.size());
           S1=0;   for(int i=0; i<(int)ny; i++) S1  += 1./pow(Cluster::GetNominalResolution(1),2);
           Sz=0;   for(int i=0; i<(int)ny; i++) Sz  += vK.at(i).second.second/pow(Cluster::GetNominalResolution(1),2);
           Szz=0;  for(int i=0; i<(int)ny; i++) Szz += pow(vK.at(i).second.second,2)/pow(Cluster::GetNominalResolution(1),2);
  Double_t Sy=0;   for(int i=0; i<(int)ny; i++) Sy  += vK.at(i).second.first/pow(Cluster::GetNominalResolution(1),2);
  Double_t Szy=0;  for(int i=0; i<(int)ny; i++) Szy += (vK.at(i).second.first*vK.at(i).second.second)/pow(Cluster::GetNominalResolution(1),2);
  Double_t Dy = S1*Szz - Sz*Sz;
  Y0 = (Sy*Szz-Sz*Szy)/Dy;
  //iDirY = (S1*Szy-Sz*Sy)/Dy; iDirY=-iDirY;
  mY = (S1*Szy-Sz*Sy)/Dy; //mY=-mY;
  Y0err = sqrt(Szz/Dy);
  //iDirYerr = sqrt(S1/Dy);
  mYerr = sqrt(S1/Dy);
  corrYmY = -Sz/sqrt(Szz*S1);
#endif

  //  printf("%f %f %f %f\n", mX, mY, X0, Y0);
  
  //    dirX = mX * dirZ                 -->       dirX = mX / sqrt(1 + mX^2 + mY^2)
  //    dirY = mY * dirZ                 -->       dirX = mY / sqrt(1 + mX^2 + mY^2)
  //    dirZ = 1./sqrt(1 + mX^2 + mY^2)
  
  //    ∂dirX/∂mX = +dirZ^3 * (1+mY^2)        ∂dirY/∂mX = -dirZ^3 * mX * mY       ∂dirZ/∂mX = -dirZ^3 * mX
  //    ∂dirX/∂mY = -dirZ^3 * mX * mY         ∂dirY/∂mY = +dirZ^3 * (1+mX^2)      ∂dirZ/∂mY = -dirZ^3 * mY
  //    corr(mX,mY)=0  since they come from independent fits
  
  iDirZ = 1./sqrt(1 + mX*mX + mY*mY);
  iDirX = mX * iDirZ;
  iDirY = mY * iDirZ;
  Double_t dDirXdmX = +iDirZ*iDirZ*iDirZ * (1+mY*mY);
  Double_t dDirXdmY = -iDirZ*iDirZ*iDirZ * mX * mY;
  Double_t dDirYdmX = -iDirZ*iDirZ*iDirZ * mX * mY;
  Double_t dDirYdmY = +iDirZ*iDirZ*iDirZ * (1+mX*mX);
  Double_t dDirZdmX = -iDirZ*iDirZ*iDirZ * mX;
  Double_t dDirZdmY = -iDirZ*iDirZ*iDirZ * mY;
  iDirXerr = sqrt( pow(dDirXdmX * mXerr, 2) + pow(dDirXdmY * mYerr, 2) );
  iDirYerr = sqrt( pow(dDirYdmX * mXerr, 2) + pow(dDirYdmY * mYerr, 2) );
  iDirZerr = sqrt( pow(dDirZdmX * mXerr, 2) + pow(dDirZdmY * mYerr, 2) );

  //------------------------------------------------------------------------------------------
  
  theta = std::acos(iDirZ);
  phi = std::atan2(iDirY, iDirX);
  
  //should not happen ------------
  if (theta<0) {
    theta = fabs(theta);
    phi+=TMath::Pi();
  }
  if (phi>TMath::Pi()) {
    phi-=2.0*TMath::Pi();
  }
  if (phi<-TMath::Pi()) {
    phi+=2.0*TMath::Pi();
  }
  //------------------------------
  
  //theta = acos( dirZ )        --> theta(mX,mY) = acos( 1./sqrt(1+mX*mX+mY*mY) )
  //phi = atan (dirY/dirX)      --> phi(mX,mY)   = atan( mY/mX )
  //
  //∂phi/∂mX = -mY / (mX^2 + mY^2)                            ∂phi/∂mY = +mX / (mX^2 + mY^2)
  //∂theta/∂mX = [(1+mX^2+mY^2)*sqrt(mX^2+mY^2)]^{-1}         ∂theta/∂mY = ∂theta/∂mX 
  
  double dthetadmX = 1./( (1 + mX*mX * mY*mY) * sqrt(mX*mX + mY*mY) );
  double dthetadmY = 1./( (1 + mX*mX * mY*mY) * sqrt(mX*mX + mY*mY) );
  double dphidmX   = -mY / (mX*mX + mY*mY);
  double dphidmY   = +mX / (mX*mX + mY*mY);
  thetaerr = sqrt( pow(dthetadmX*mXerr,2) + pow(dthetadmY*mYerr,2) );
  phierr   = sqrt( pow(dphidmX*mXerr,2)   + pow(dphidmY*mYerr,2) );

  //------------------------------------------------------------------------------------------
  
  int ndofS = vS.size() - 2;
  int ndofK = vK.size() - 2;

  double chisqS = 0.0;
  double chisqK = 0.0;
  double chisq = 0.0;
  if (ndofS>=0) {
    chisqS = _compchisq(vS, v_chilayS, mX, X0, Cluster::GetNominalResolution(0));
    chisq += chisqS;
  }
  if (ndofK>=0) {
    chisqK = _compchisq(vK, v_chilayK, mY, Y0, Cluster::GetNominalResolution(1));
    chisq += chisqK;
  }
  
  int ndof = ndofS + ndofK;
  double ret = chisq/ndof;
  if (ndof<=0) {
    if (ndofS>0) ret = chisqS/ndofS;
    else if (ndofK>0) ret = chisqK/ndofK;
    else if (ndof==0) ret = 0.0;
    else ret = -1.0;
  }
  chisqx = -1.0;
  if (ndofS>0) chisqx = chisqS/ndofS;
  else if (ndofS==0) chisqx = 0.0;
  chisqy = -1.0;
  if (ndofK>0) chisqy = chisqK/ndofK;
  else if (ndofK==0) chisqy = 0.0;

  if (verbose) printf("chisq/ndof = %f/%d = %f, chisqS/ndofS = %f/%d = %f, chisqK/ndofK = %f/%d = %f\n", chisq, ndof, ret, chisqS, ndofS, chisqx, chisqK, ndofK, chisqy);

  return ret;
}

#ifdef USEMINUIT
void _fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {

  std::vector<double> v_chilay;
  
  f = _compchisq(v_trackS_sf, v_chilay, par[0], par[2], Cluster::GetNominalResolution(0)) + _compchisq(v_trackK_sf, v_chilay, par[1], par[3], Cluster::GetNominalResolution(1));
  
  return;
}
#endif

double _compchisq(std::vector<std::pair<int, std::pair<double, double> > > vec, std::vector<double>& v_chilay, double imX, double iX, double iXerr, double Z0){

  v_chilay.clear();
  
  static Double_t chisq;
  chisq = 0.0;
  static Double_t delta;
  delta = 0.0;
  for (int pp=0; pp<(int)(vec.size()); pp++) {
    delta = (vec.at(pp).second.first - _func(vec.at(pp).second.second, imX, iX, Z0))/iXerr;
    v_chilay.push_back(delta*delta);
    chisq += delta*delta;
  }
  
  return chisq;
}

Double_t _func(double z, double imX, double iX, double Z0) {
  return iX + (z-Z0)*imX;
}

double Event::ExtrapolateTrack(double z, int component) {
  if (component==0) return _func(z, _mX, _X0);
  else if (component==1) return _func(z, _mY, _Y0);
  else return -9999.99;
}

bool Event::IsClusterUsedInTrack(int index_cluster){

  //  printf("IsClusterUsedInTrack\n");
  
  for (int ii=0; ii<(int)(_v_trackS.size()); ii++){
    //    printf("%d cluster (S) in track\n", _v_trackS.at(ii).first);
    if (_v_trackS.at(ii).first==index_cluster) return true;
  }
  for (int ii=0; ii<(int)(_v_trackK.size()); ii++){
    //    printf("%d cluster (K) in track\n", _v_trackK.at(ii).first);
    if (_v_trackK.at(ii).first==index_cluster) return true;
  }

  return false;
}

void Event::StoreTrackClusterPatterns(){

  for (int ii=0; ii<NJINF; ii++) {;
    for (int ss=0; ss<2; ss++) {
      _track_cluster_pattern[ii][ss]=0;
    }
  }
  
  std::vector<std::pair<int, std::pair<double, double> > > _v_track_tmp;
  _v_track_tmp.clear();
  
  for (int i_side=0; i_side<2; i_side++){
    if(i_side==0) _v_track_tmp = _v_trackS;
    else if(i_side==1) _v_track_tmp = _v_trackK;
    
    for (int ii=0; ii<(int)(_v_track_tmp.size()); ii++){
      int index_cluster=_v_track_tmp.at(ii).first;
      Cluster* cl=GetCluster(index_cluster);
      int tdrnum=cl->GetTDR();
      int jinfnum=cl->GetJinf();
      //      printf("JINF %d, TDR %d , %d cluster (%d) in track\n", jinfnum, tdrnum, index_cluster, i_side);
      
      unsigned long long int tdr_index = (int)(pow(10.0, (double)(tdrnum)));
      //      printf("TDR %d %d --> %lld\n", tdrnum, i_side, tdr_index);
      _track_cluster_pattern[jinfnum][i_side] +=  tdr_index;
    }
  }

  return;
}

bool Event::IsTDRInTrack(int side, int tdrnum, int jinfnum) {
  return ((bool)(((unsigned long long int)(_track_cluster_pattern[jinfnum][side]/((int)(pow((double)10, (double)tdrnum)))))%10));
}

void Event::FillHitVector(){

  _v_trackhit.clear();
  
  std::pair<int,int> coopair[NJINF*NTDRS];
  for (int pp=0; pp<NJINF*NTDRS; pp++) {
    coopair[pp].first=-1;
    coopair[pp].second=-1;
  }
  
  for (int index_cluster = 0; index_cluster<NClusTot; index_cluster++) {

    if (!IsClusterUsedInTrack(index_cluster)) continue;
    Cluster* current_cluster = GetCluster(index_cluster);
    
    int ladder = current_cluster->ladder;
    int side=current_cluster->side;
    
    if (side==0) {
      coopair[ladder].first=index_cluster;
    }
    else {
      coopair[ladder].second=index_cluster;
    }
  }
  
  for (int tt=0; tt<NJINF*NTDRS; tt++) {
    if (coopair[tt].first>=0 || coopair[tt].second>=0) {
      _v_trackhit.push_back(std::make_pair(tt, coopair[tt]));      
    }
  }

  return;
}

struct sort_pred {
  bool operator()(const std::pair<int,double> &left, const std::pair<int,double> &right) {
    return left.second < right.second;
  }
};

double Event::RefineTrack(double nsigmaS, int nptsS, double nsigmaK, int nptsK, bool verbose){
  
  std::vector<std::pair<int, std::pair<double, double> > > _v_trackS_tmp = _v_trackS;
  std::vector<std::pair<int, std::pair<double, double> > > _v_trackK_tmp = _v_trackK;
  
  std::vector<std::pair<int, double> > _v_chilayS_tmp;
  for (unsigned int ii=0; ii<_v_chilayS.size(); ii++) {
    _v_chilayS_tmp.push_back(std::make_pair(ii, _v_chilayS.at(ii)));
  }
  std::sort(_v_chilayS_tmp.begin(), _v_chilayS_tmp.end(), sort_pred());
  
  std::vector<std::pair<int, double> > _v_chilayK_tmp;
  for (unsigned int ii=0; ii<_v_chilayK.size(); ii++) {
    _v_chilayK_tmp.push_back(std::make_pair(ii, _v_chilayK.at(ii)));
  }
  std::sort(_v_chilayK_tmp.begin(), _v_chilayK_tmp.end(), sort_pred());

  if (((int)(_v_trackS_tmp.size()))>nptsS+1) {//so that even removing one we have at least nptsS hits
    if (sqrt(_v_chilayS_tmp.at(_v_chilayS_tmp.size()-1).second)>nsigmaS) {//if the worst residual is above threshold is removed
      _v_trackS_tmp.erase(_v_trackS_tmp.begin()+_v_chilayS_tmp.at(_v_chilayS_tmp.size()-1).first);
    }
  }
  if (((int)(_v_trackK_tmp.size()))>nptsK+1) {//so that even removing one we have at least nptsK hits
    if (sqrt(_v_chilayK_tmp.at(_v_chilayK_tmp.size()-1).second)>nsigmaK) {//if the worst residual is above threshold is removed
      _v_trackK_tmp.erase(_v_trackK_tmp.begin()+_v_chilayK_tmp.at(_v_chilayK_tmp.size()-1).first);
    }
  }

  double ret = SingleFit(_v_trackS_tmp, _v_trackK_tmp, false);
  AssignAsBestTrackFit();
  
  return ret;
}

// A TRUNCATED MEAN WOULD BE BETTER BUT STICAZZI FOR NOW...
double Event::GetChargeTrack(int side){

  if (side<0 || side>1) {
    printf("Not a valid side: %d\n", side);
    return -99999.9;
  }
  
  int npts=0;
  double charge=0.0;
  
  for (unsigned int ii=0; ii<_v_trackhit.size(); ii++) {
    int index_cluster=-1;
    if (side==0) index_cluster=_v_trackhit.at(ii).second.first;
    else index_cluster=_v_trackhit.at(ii).second.second;
    if (index_cluster>=0) {
      Cluster* cl = GetCluster(index_cluster);
      charge += cl->GetCharge();
      npts++;
    }
  }
  
  charge/=npts;

  return charge;
}

double Event::GetCalPed_PosNum(int tdrnum, int channel, int Jinfnum){
  return CalPed[tdrnum][channel];
}

double Event::GetCalSigma_PosNum(int tdrnum, int channel, int Jinfnum){
  return CalSigma[tdrnum][channel];
}

double Event::GetRawSignal_PosNum(int tdrnum, int channel, int Jinfnum){
  return RawSignal[tdrnum][channel]/8.0;
}

double Event::GetCN_PosNum(int tdrnum, int va, int Jinfnum){
  
  short int array[1024];
  float arraySoN[1024];
  float pede[1024];

  for(int chan=0; chan <1024; chan++){
    array[chan]=RawSignal[tdrnum][chan];
    arraySoN[chan]=RawSoN[tdrnum][chan];
    pede[chan]=CalPed[tdrnum][chan];
  }

  return ComputeCN(64, &(array[va*64]), &(pede[va*64]), &(arraySoN[va*64]));
}

float Event::GetRawSoN_PosNum(int tdrnum, int channel, int Jinfnum) {
  return (RawSignal[tdrnum][channel]/8.0-CalPed[tdrnum][channel])/CalSigma[tdrnum][channel];
}

double Event::GetCalPed(RHClass* rh, int tdrnum, int channel, int Jinfnum){
  int tdrnumraw=rh->FindPosRaw(tdrnum+100*Jinfnum);
  return GetCalPed_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::GetCalSigma(RHClass* rh, int tdrnum, int channel, int Jinfnum){
  int tdrnumraw=rh->FindPosRaw(tdrnum+100*Jinfnum);
  return GetCalSigma_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::GetRawSignal(RHClass* rh, int tdrnum, int channel, int Jinfnum){
  int tdrnumraw=rh->FindPosRaw(tdrnum+100*Jinfnum);
  return GetRawSignal_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::GetCN(RHClass* rh, int tdrnum, int va, int Jinfnum){
  int tdrnumraw=rh->FindPosRaw(tdrnum+100*Jinfnum);
  return GetCN_PosNum(tdrnumraw, va, Jinfnum);
}

float Event::GetRawSoN(RHClass* rh, int tdrnum, int channel, int Jinfnum) {
  int tdrnumraw=rh->FindPosRaw(tdrnum+100*Jinfnum);
  return GetRawSoN_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::ComputeCN(int size, short int* RawSignal, float* pede, float* RawSoN, double threshold){
  
  double mean=0.0;
  int n=0;
  
  for (int ii=0; ii<size; ii++) {
    if (RawSoN[ii]<threshold) {//to avoid real signal...
      n++;
      //      printf("    %d) %f %f\n", ii, RawSignal[ii]/8.0, pede[ii]);
      mean+=(RawSignal[ii]/8.0-pede[ii]);
    }
  }
  if (n>1) {
    mean/=n;
  }
  else { //let's try again with an higher threshold
    mean = ComputeCN(size, RawSignal, pede, RawSoN, threshold+1.0);
  }
  //  printf("    CN = %f\n", mean);
  
  return mean;
}

//-------------------------------------------------------------------------------------

ClassImp(RHClass);

RHClass::RHClass(){
  Run=0;
  ntdrRaw=0;
  ntdrCmp=0;
  nJinf=0;
  sprintf(date," ");
  memset(JinfMap,-1,NJINF*sizeof(JinfMap[0]));
  memset(tdrRawMap,-1,NTDRS*sizeof(tdrRawMap[0]));
  memset(tdrCmpMap,-1,NTDRS*sizeof(tdrCmpMap[0]));
  
  for (int ii=0;ii<NTDRS;ii++)
    for (int jj=0;jj<NVAS;jj++){
      CNMean[ii][jj]=0.;
      CNSigma[ii][jj]=0.;
    }
}

void RHClass::Print(){
  printf("---------------------------------------------\n");
  printf("The header says:\n");
  printf("Run: %d Date: %s\n",Run,date);
  printf("# Jinf = %d\n",nJinf);
  for (int ii=0;ii<nJinf;ii++)
    printf("Jinf Map pos: %d Jinf num: %d\n", ii, JinfMap[ii]);
  
  printf("# TDR RAW = %d\n",ntdrRaw);
  for (int ii=0;ii<ntdrRaw;ii++)
    printf("TDR RAW Map pos: %d tdrnum: %d\n", ii, tdrRawMap[ii]);
  
  printf("# TDR CMP = %d\n",ntdrCmp);
  for (int ii=0;ii<ntdrCmp;ii++)
    printf("TDR CMP Map pos: %d tdrnum: %d\n", ii, tdrCmpMap[ii]);
  //   for (int ii=0;ii<NTDRS;ii++){
  //     printf("TDR: %d\n",ii);
  //     for (int jj=0;jj<NVAS;jj++)
  //       printf(" %6.2f ",CNMean[ii][jj]);
  //     printf(" \n");
  //     for (int jj=0;jj<NVAS;jj++)
  //       printf(" %6.2f ",CNSigma[ii][jj]);
  //     printf(" \n");
  //     printf(" \n");
  //   }
  printf("---------------------------------------------\n");
  return;
}

int RHClass::FindPos(int tdrnum){
  
  // Print();
  // printf("ntdrCmp = %d\n", ntdrCmp);
  // for (int ii=0; ii<ntdrCmp; ii++) {
  //   printf("CMP: %d -> %d\n", ii, tdrCmpMap[ii]);
  // }
  
  for (int ii=0; ii<ntdrCmp; ii++)
    if (tdrCmpMap[ii]==tdrnum) return ii;
  
  return -1;
}

int RHClass::FindPosRaw(int tdrnum){
  
  // for (int ii=0;ii<ntdrRaw;ii++) {
  //   printf("RAW: %d -> %d\n", ii, tdrRaw[ii]);
  // }
  
  for (int ii=0;ii<ntdrRaw;ii++)
    if(tdrRawMap[ii]==tdrnum)  return ii;
  return -1;
}


