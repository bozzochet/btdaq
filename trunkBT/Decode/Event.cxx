#include "Event.hh"
#include "Cluster.hh"
#include "TMinuit.h"
#include "TH1F.h"
#include "TMath.h"

ClassImp(Event);

bool Event::alignmentnotread=true;
float Event::alignpar[NJINF][NTDRS][3];

std::vector<std::pair<int, std::pair<double, double> > > _v_trackS_tmp;
std::vector<std::pair<int, std::pair<double, double> > > _v_trackK_tmp;
double _chisq_tmp;

static double _compchisq(std::vector<std::pair<int, std::pair<double, double> > > vec, double theta, double phi, double x0, double y0, int side);
static Double_t* _func(double z, double theta, double phi, double x0, double y0);
static void _fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

Event::Event(){

  Evtnum=0;
  JINJStatus=0;
  for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;
  
  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0;
  }
  for(int ii=0;ii<1024;ii++)
    for(int kk=0;kk<8;kk++) {
      Signal[kk][ii]=0;
      SoN[kk][ii]=0.0;
    }
  
  //  RawLadder = new TClonesArray("RawData", NJINF*8);//NJINFS*8 is the maximum number of ladder in raw mode that can me read by a single jinf.
  
  NClusTot=0;
  notgood=0;
  Cls = new TClonesArray("Cluster", NJINF*NTDRS);//if more than NJINFS*NTDRS anyhow the array will be expanded
  Cls->SetOwner();

  if (alignmentnotread) ReadAlignment("alignment.dat");

  _theta = -9999.9;
  _phi = -9999.9;
  _X0 = -9999.9;
  _Y0 = -9999.9;
  _v_trackS.clear();
  _v_trackK.clear();
  _chisq = 999999999.9;
  
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
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0; 
  }

  for(int ii=0;ii<8;ii++){ 
    for(int kk=0;kk<1024;kk++) {
      Signal[ii][kk]=0;
      SoN[ii][kk]=0.0;
    }
  }

  if(Cls) Cls->Delete();
  
  //   for (int ii=Cls->GetEntries();ii>-1;ii--){
  //     Cluster* ff=(Cluster*) Cls->RemoveAt(ii);
  //     if(ff) delete ff;
  //   }

  _theta = -9999.9;
  _phi = -9999.9;
  _X0 = -9999.9;
  _Y0 = -9999.9;
  _v_trackS.clear();
  _v_trackK.clear();
  _chisq = 999999999.9;
  
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

int Event::NGoldenClus(int lad, int side){
  int num=0;
  for (int ii=0;ii<NClusTot;ii++){
    Cluster* cl=GetCluster(ii);
    if(cl->ladder==lad&&cl->side==side&&cl->golden==1) num++;
  }
  return num;
}

void Event::ReadAlignment(TString filename){
  
  printf("Reading alignment from %s:\n", filename.Data());

  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      for (int cc=0; cc<3; cc++) {
	alignpar[jj][tt][cc]=0.0;
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
    while(!feof(ft)){
      if (fgets(line, dimline, ft)!=NULL) {
	//	printf("%s\n", line);
	if (*line == '#') continue; /* ignore comment line */
	else {
	  sscanf(line, "%d\t%d\t%f\t%f\t%f", &jinfnum, &tdrnum, &dummy, &dummy, &dummy);
	  if (jinfnum<NJINF && tdrnum<NTDRS) {
	    sscanf(line, "%d\t%d\t%f\t%f\t%f", &jinfnum, &tdrnum, &alignpar[jinfnum][tdrnum][0], &alignpar[jinfnum][tdrnum][1], &alignpar[jinfnum][tdrnum][2]);
	  }
	  else {
	    printf("Wrong JINF/TDR (%d, %d): maximum is (%d,%d)\n", jinfnum, tdrnum, NJINF, NTDRS);
	  }
	}
      }
      else {
	fclose(ft);
      }
    }
  }

  fclose(ft);
  
  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      for (int cc=0; cc<3; cc++) {
	if (cc==0) printf("JINF %02d TDR %02d)\t", jj, tt);
	printf("%f\t", alignpar[jj][tt][cc]);
	if (cc==2) printf("\n");
      }
    }
  }

  alignmentnotread=false;
  
  return;
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

bool Event::FindTrackAndFit(int nptsS, int nptsK, bool verbose) {

  _theta = -9999.9;
  _phi = -9999.9;
  _X0 = -9999.9;
  _Y0 = -9999.9;
  _v_trackS.clear();
  _v_trackK.clear();
  _chisq = 999999999.9;
  
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

  int totmult=1.0;
  for (int jj=0; jj<NJINF; jj++) {
    for (int tt=0; tt<NTDRS; tt++) {
      totmult *= std::max((int)(v_cog_laddS[jj][tt].size()), 1);
      totmult *= std::max((int)(v_cog_laddK[jj][tt].size()), 1);
      if (verbose) printf("multiplicity ladder 100*%d+%d: %lu %lu\n", jj, tt, (long unsigned int)(v_cog_laddS[jj][tt].size()), (long unsigned int)(v_cog_laddK[jj][tt].size()));
    }
  }
  
  if (totmult>pow(2*NTDRS, 2)) {//more than 2 clusters per ladder (24, not the ones really present...), per side: give up!
    return false;
  }
  //  printf("Total multiplicy = %d\n", totmult);
  
  std::vector<std::pair<int, std::pair<double, double> > > vecS;//actually used just for compatibility with the telescopic function
  std::vector<std::pair<int, std::pair<double, double> > > vecK;//actually used just for compatibility with the telescopic function
  double cc = CombinatorialFit(v_cog_laddS, v_cog_laddK, NJINF, NTDRS, vecS, vecK, nptsS, nptsK, verbose);

  //  printf("cc = %f\n", cc);
  
  bool ret = false;
  if (cc>=999999999.9) ret =false;
  else if (cc<-0.000000001) ret = false;
  else ret = true;
  
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
  
  if (ijinf!=0) {
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
  else {
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
      /* debug
      static TH1F hchi("hchi", "hchi", 1000, 0.0, 10.0);
      static TH1F htheta("htheta", "htheta", 1000, -TMath::Pi()/2.0, TMath::Pi()/2.0);
      static TH1F hphi("hphi", "hphi", 1000, -TMath::Pi(), TMath::Pi());
      static TH1F hx0("hx0", "hx0", 1000, -1000.0, 1000.0);
      static TH1F hy0("hy0", "hy0", 1000, -1000.0, 1000.0); 
      */
      double theta, thetaerr;
      double phi, phierr;
      double x0, x0err;
      double y0, y0err;
      double chisq;
      chisq = SingleFit(v_cog_trackS, v_cog_trackK, theta, thetaerr, phi, phierr, x0, x0err, y0, y0err, verbose);
      if (chisq<_chisq) {
	if (verbose) printf("Best track) new chisq %f, old one %f\n", chisq, _chisq);
	_chisq=chisq;
	_theta=theta;
	_phi=phi;
	_X0=x0;
	_Y0=y0;
	_v_trackS = v_cog_trackS;
	_v_trackK = v_cog_trackK;
      }
      /*
      hchi.Fill(log10(chisq));
      htheta.Fill(theta);
      hphi.Fill(phi);
      hx0.Fill(x0);
      hy0.Fill(y0);
      */
    }
    if (verbose) {
      printf("----------------------\n");
    }
  }

  //  sleep(1);
  
  return _chisq;
}

double Event::SingleFit(std::vector<std::pair<int, std::pair<double, double> > > vS, std::vector<std::pair<int, std::pair<double, double> > > vK, double& theta, double& thetaerr, double& phi, double& phierr, double& x0, double& x0err, double& y0, double& y0err, bool verbose){

  _v_trackS_tmp = vS;
  _v_trackK_tmp = vK;
  
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
  static Double_t step[5] =   {0.001 , 0.001 , 0.001 , 0.001};
  minuit->mnparm(0, "theta", vstart[0], step[0], -TMath::Pi()/2.0, TMath::Pi()/2.0, ierflg);
  minuit->mnparm(1, "phi",   vstart[1], step[1], -TMath::Pi(), TMath::Pi(), ierflg);
  minuit->mnparm(2, "X0",    vstart[2], step[2], 0,0, ierflg);
  minuit->mnparm(3, "Y0",    vstart[3], step[3], 0,0, ierflg);
  
  // Now ready for minimization step
  arglist[0] = 500;
  arglist[1] = 1.;
  minuit->mnexcm("MIGRAD", arglist, 2, ierflg);
  
  // Print results
  // Double_t amin,edm,errdef;
  // Int_t nvpar,nparx,icstat;
  // minuit->mnstat(amin, edm, errdef, nvpar, nparx, icstat);
  // minuit->mnprin(3,amin);

  minuit->GetParameter (0, theta, thetaerr);
  minuit->GetParameter (1, phi, phierr);
  minuit->GetParameter (2, x0, x0err);
  minuit->GetParameter (3, y0, y0err);

  int ndofS = vS.size() - 2;
  int ndofK = vK.size() - 2;

  double chisqS = 0.0;
  double chisqK = 0.0;
  double chisq = 0.0;
  if (ndofS>0) {
    chisqS = _compchisq(vS, theta, phi, x0, y0, 0);
    chisq += chisqS;
  }
  if (ndofK>0) {
    chisqK = _compchisq(vK, theta, phi, x0, y0, 1);
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
  
  if (verbose) printf("chisq/ndof = %f/%d = %f\n", chisq, ndof, ret);
  
  return ret;
}

void _fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  
  f = _compchisq(_v_trackS_tmp, par[0], par[1], par[2], par[3], 0) + _compchisq(_v_trackK_tmp, par[0], par[1], par[2], par[3], 1);
  
  return;
}

double _compchisq(std::vector<std::pair<int, std::pair<double, double> > > vec, double theta, double phi, double x0, double y0, int side){
  
  static Double_t chisq;
  chisq = 0.0;
  static Double_t delta;
  delta = 0.0;
  static double error;
  error = Cluster::GetNominalResolution(side);
  for (int pp=0; pp<(int)(vec.size()); pp++) {
    delta = (vec.at(pp).second.first - _func(vec.at(pp).second.second, theta, phi, x0, y0)[side])/error;
    chisq += delta*delta;
  }
  
  return chisq;
}

Double_t* _func(double z, double theta, double phi, double x0, double y0) {
  
  static double iDir[3];
  
  // --- directions ---
  iDir[0] = sin(theta)*cos(phi);
  iDir[1] = sin(theta)*sin(phi);
  iDir[2] = cos(theta);

  static double tt1;
  tt1 = z/iDir[2];
  
  static Double_t point[2];
  point[0] = x0 + tt1*iDir[0];
  point[1] = y0 + tt1*iDir[1];
  
  return point;
}

double Event::ExtrapolateBestTrack(double z, int component) {
  return _func(z, _theta, _phi, _X0, _Y0)[component];
}

bool Event::IsClusterUsedInBestTrack(int index_cluster){

  //  printf("IsClusterUsedInBestTrack\n");
  
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
    for (int jj=0;jj<16;jj++){
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
  //     for (int jj=0;jj<16;jj++)
  //       printf(" %6.2f ",CNMean[ii][jj]);
  //     printf(" \n");
  //     for (int jj=0;jj<16;jj++)
  //       printf(" %6.2f ",CNSigma[ii][jj]);
  //     printf(" \n");
  //     printf(" \n");
  //   }
  printf("---------------------------------------------\n");
  return;
}

int RHClass::FindPos(int tdrnum){

  // for (int ii=0;ii<ntdrCmp;ii++) {
  //   printf("CMP: %d -> %d\n", ii, tdrCmp[ii]);
  // }
  
  for (int ii=0;ii<ntdrCmp;ii++)
    if(tdrCmpMap[ii]==tdrnum)  return ii;
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
