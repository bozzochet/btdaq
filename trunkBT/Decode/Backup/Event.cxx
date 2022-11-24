#include "Event.hh"
#include "Cluster.hh"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TRotation.h"
#include <bitset>
#include <iostream>
#include <unistd.h>

using namespace std;

#include <cmath>

//#define USEMINUIT

ClassImp(Event);

int Event::NJINF = -1;
int Event::NTDRS = -1;
int Event::NCHAVA = -1;
int Event::NADCS = -1;
int Event::NVAS = -1;

Event::Flavour Event::kFlavour = Event::Flavour::UNDEF;

void Event::InitFlavourConfig(Flavour flv) {
  switch (flv) {
  case Flavour::UNDEF:
    printf("********************************\n");
    printf("********************************\n");
    printf("Before using the event\n");
    printf("(even the default-constructed one used when reading a TTree)\n");
    printf("you mandatory need to call\n");
    printf("Event::SetFlavour(Flavour::<AMS,OCA,FOOT,...>)\n");
    printf("********************************\n");
    printf("********************************\n");
    throw std::runtime_error("Undefined Event flavour");
  case Flavour::AMS:
    NJINF = 1;
    NTDRS = 24;  // this a "max number of". For FOOT/OCA remember that we have 2 sensors for board (i.e. 6 DE-10 nano
                 // means 12 "TDR")
    NCHAVA = 64; // for POX MC it was 256 or 128. To be understood...
    NADCS = 3;
    NVAS = 16;
    break;
  case Flavour::OCA:
    NJINF = 1;
    NTDRS = 24;  // this a "max number of". For FOOT/OCA remember that we have 2 sensors for board (i.e. 6 DE-10 nano
                 // means 12 "TDR")
    NCHAVA = 64; // for POX MC it was 256 or 128. To be understood...
    NADCS = 5;
    NVAS = 10;
    break;
  case Flavour::FOOT:
    NJINF = 1;
    NTDRS = 24;  // this a "max number of". For FOOT/OCA remember that we have 2 sensors for board (i.e. 6 DE-10 nano
                 // means 12 "TDR")
    NCHAVA = 64; // for POX MC it was 256 or 128. To be understood...
    NADCS = 5;
    NVAS = 10;
    break;
  }
}

bool Event::ladderconfnotread = true;
bool Event::alignmentnotread = true;
float ***Event::alignpar;
bool **Event::multflip;

LadderConf *Event::ladderconf = nullptr;

bool Event::gaincorrectionnotread = true;
float ****Event::gaincorrectionpar;

// they store temporarily the result of the fit----------------------------
double mS_sf, mSerr_sf;
double mK_sf, mKerr_sf;
double iDirS_sf, iDirSerr_sf;
double iDirK_sf, iDirKerr_sf;
double iDirZ_sf, iDirZerr_sf;
double theta_sf, thetaerr_sf;
double phi_sf, phierr_sf;
double S0_sf, S0err_sf;
double K0_sf, K0err_sf;
double chisq_sf;
double chisqS_sf;
double chisqK_sf;
std::vector<std::pair<int, std::pair<double, double>>> v_trackS_sf;
std::vector<std::pair<int, std::pair<double, double>>> v_trackK_sf;
std::vector<double> v_trackErrS_sf;
std::vector<double> v_trackErrK_sf;
std::vector<double> v_chilayS_sf;
std::vector<double> v_chilayK_sf;
//-------------------------------------------------------------------------

static double _compchisq(std::vector<std::pair<int, std::pair<double, double>>> vec, std::vector<double> &v_chilay,
                         double iDir, double iS, std::vector<double> iSerr, double Z0 = 0);
// static double _compchisq(std::vector<std::pair<int, std::pair<double, double> > > vec, std::vector<double>& v_chilay,
// double iDir, double iS, double iSerr, double Z0=0);
static Double_t _func(double z, double imS, double iS, double Z0 = 0);
#ifdef USEMINUIT
static void _fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
#endif

Event::Event() {

  _NJINF = NJINF;
  _NTDRS = NTDRS;
  _NCHAVA = NCHAVA;
  _NADCS = NADCS;
  _NVAS = NVAS;
  _NCHA = NVAS * NCHAVA;

  std::cout << "NCH: " << NVAS * NCHAVA << '\n';

  alignpar = new float **[NJINF];
  multflip = new bool *[NJINF];
  gaincorrectionpar = new float ***[NJINF];
  JINFStatus = new int[NJINF];
  TDRStatus = new int *[NJINF];
  CNoise = new double **[NJINF];
  NClus = new int **[NJINF];
  CalSigma = new double **[NJINF];
  CalPed = new double **[NJINF];
  RawSignal = new short int **[NJINF];
  RawSoN = new float **[NJINF];
  CalStatus = new int **[NJINF];
  ReadTDR = new short int *[NJINF];
  _track_cluster_pattern = new unsigned int *[NJINF];
  for (int ii = 0; ii < NJINF; ii++) {
    alignpar[ii] = new float *[NTDRS];
    multflip[ii] = new bool[NTDRS];
    gaincorrectionpar[ii] = new float **[NTDRS];
    TDRStatus[ii] = new int[NTDRS];
    CNoise[ii] = new double *[NTDRS];
    NClus[ii] = new int *[NTDRS];
    CalSigma[ii] = new double *[NTDRS];
    CalPed[ii] = new double *[NTDRS];
    RawSignal[ii] = new short int *[NTDRS];
    RawSoN[ii] = new float *[NTDRS];
    CalStatus[ii] = new int *[NTDRS];
    ReadTDR[ii] = new short int[NTDRS];
    _track_cluster_pattern[ii] = new unsigned int[2];
    for (int jj = 0; jj < NTDRS; jj++) {
      alignpar[ii][jj] = new float[3];
      gaincorrectionpar[ii][jj] = new float *[NVAS];
      CNoise[ii][jj] = new double[NVAS];
      NClus[ii][jj] = new int[2];
      CalSigma[ii][jj] = new double[NVAS * NCHAVA];
      CalPed[ii][jj] = new double[NVAS * NCHAVA];
      RawSignal[ii][jj] = new short int[NVAS * NCHAVA];
      RawSoN[ii][jj] = new float[NVAS * NCHAVA];
      CalStatus[ii][jj] = new int[NVAS * NCHAVA];
      for (int kk = 0; kk < NVAS; kk++) {
        gaincorrectionpar[ii][jj][kk] = new float[2];
      }
    }
  }

  Evtnum = 0;
  JINJStatus = 0;
  for (int jj = 0; jj < NJINF; jj++) {
    JINFStatus[jj] = 0;
    for (int ii = 0; ii < NTDRS; ii++) {
      TDRStatus[jj][ii] = 31;
    }
  }

  for (int hh = 0; hh < NJINF; hh++) {
    for (int ii = 0; ii < NTDRS; ii++) {
      ReadTDR[hh][ii] = 0;
      for (int jj = 0; jj < NVAS; jj++) {
        CNoise[hh][ii][jj] = 0;
      }
      NClus[hh][ii][0] = 0;
      NClus[hh][ii][1] = 0;
    }
  }

  for (int jj = 0; jj < NJINF; jj++) {
    // Viviana: hardcoded n
    // -> kk to run onto nlayers
    for (int kk = 0; kk < NTDRS; kk++) {           // Viviana: was kk<8
      for (int ii = 0; ii < NVAS * NCHAVA; ii++) { //// Viviana: was ii<1024
        CalSigma[jj][kk][ii] = 0.0;
        CalPed[jj][kk][ii] = 0.0;
        RawSignal[jj][kk][ii] = 0.;
        RawSoN[jj][kk][ii] = 0.0;
        CalStatus[jj][kk][ii] = 0;
      }
    }
  }

  //  RawLadder = new TClonesArray("RawData", NJINF*8);//NJINFS*8 is the maximum number of ladder in raw mode that can
  //  me read by a single jinf.

  NClusTot = 0;
  notgood = 0;
  Cls = new TClonesArray("Cluster", NJINF * NTDRS); // if more than NJINFS*NTDRS anyhow the array will be expanded
  Cls->SetOwner();

  // MD: this must be fixed, cannot be hardcoded
  if (ladderconfnotread)
    ReadLadderConf("ladderconf_OCA.dat");
  // if (ladderconfnotread) ReadLadderConf("ladderconf_mc.dat");

  //  if (alignmentnotread) ReadAlignment("alignment_mc.dat");
  // if (alignmentnotread) ReadAlignment("alignment_mc_300.dat");
  //  if (alignmentnotread)
  //    ReadAlignment("alignment_mc_150.dat");

  if (gaincorrectionnotread) {
    switch (kFlavour) {
    case Flavour::OCA:
      ReadGainCorrection("gaincorrection_OCA.dat");
      break;
    case Flavour::FOOT:
      ReadGainCorrection("gaincorrection_FOOT.dat");
      break;
    default:
      ReadGainCorrection("gaincorrection.dat");
    }
  }

  ClearTrack();

  return;
}

Event::~Event() {
  if (Cls) {
    Cls->Delete();
    delete Cls;
  }
}

void Event::Clear() {
  JINJStatus = 0;
  for (int jj = 0; jj < NJINF; jj++) {
    JINFStatus[jj] = 0;
    for (int ii = 0; ii < NTDRS; ii++) {
      TDRStatus[jj][ii] = 31;
    }
  }

  NClusTot = 0;

  for (int jj = 0; jj < NJINF; jj++) {
    // Viviana: hardcoded
    // ->kk to run onto nlayers?
    for (int ii = 0; ii < NTDRS; ii++) { // Viviana: was kk<8
      ReadTDR[jj][ii] = 0;
      for (int iv = 0; iv < NVAS; iv++)
        CNoise[jj][ii][iv] = 0;
      NClus[jj][ii][0] = 0;
      NClus[jj][ii][1] = 0;
      for (int kk = 0; kk < NVAS * NCHAVA; kk++) { // Viviana: was 1024
        CalSigma[jj][ii][kk] = 0.0;
        CalPed[jj][ii][kk] = 0.0;
        RawSignal[jj][ii][kk] = 0;
        RawSoN[jj][ii][kk] = 0.0;
        CalStatus[jj][ii][kk] = 0;
      }
    }
  }

  if (Cls)
    Cls->Delete();

  //   for (int ii=Cls->GetEntries();ii>-1;ii--){
  //     Cluster* ff=(Cluster*) Cls->RemoveAt(ii);
  //     if(ff) delete ff;
  //   }

  ClearTrack();

  return;
}

Cluster *Event::AddCluster(int Jinfnum, int lad, int side) {
  Cluster *pp = (Cluster *)Cls->New(NClusTot);
  NClus[Jinfnum][lad][side]++;
  NClusTot++;
  return pp;
}

Cluster *Event::GetCluster(int ii) { return (Cluster *)Cls->At(ii); }

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

void Event::ReadLadderConf(TString filename, bool DEBUG) {

  printf("Reading ladder configuration from %s:\n", filename.Data());

  if (!ladderconf)
    ladderconf = new LadderConf();
  ladderconf->Init(filename, DEBUG);
}

void Event::ReadAlignment(TString filename, bool DEBUG) {

  printf("Reading alignment from %s:\n", filename.Data());

  for (int jj = 0; jj < NJINF; jj++) {
    for (int tt = 0; tt < NTDRS; tt++) {
      for (int cc = 0; cc < 3; cc++) {
        alignpar[jj][tt][cc] = 0.0;
      }
      multflip[jj][tt] = false;
    }
  }

  int const dimline = 255;
  char line[dimline];
  float dummy;
  int dummyint;
  int jinfnum = 0;
  int tdrnum = 0;

  FILE *ft = fopen(filename.Data(), "r");
  if (ft == NULL) {
    printf("Error: cannot open %s \n", filename.Data());
    return;
  } else {
    while (1) {
      if (fgets(line, dimline, ft) != NULL) {
        if (*line == '#')
          continue; /* ignore comment line */
        else {
          sscanf(line, "%d\t%d\t%f\t%f\t%f\t%d", &jinfnum, &tdrnum, &dummy, &dummy, &dummy, &dummyint);
          if (jinfnum < NJINF && tdrnum < NTDRS) {
            sscanf(line, "%d\t%d\t%f\t%f\t%f\t%d", &jinfnum, &tdrnum, &alignpar[jinfnum][tdrnum][0],
                   &alignpar[jinfnum][tdrnum][1], &alignpar[jinfnum][tdrnum][2], &dummyint);
            multflip[jinfnum][tdrnum] = (bool)(dummyint);
          } else {
            printf("Wrong JINF/TDR (%d, %d): maximum is (%d,%d)\n", jinfnum, tdrnum, NJINF, NTDRS);
          }
        }
      } else {
        printf(" closing alignment file \n");
        fclose(ft);
        break;
      }
    }
  }

  alignmentnotread = false;

  if (DEBUG == false)
    return;

  for (int jj = 0; jj < NJINF; jj++) {
    for (int tt = 0; tt < NTDRS; tt++) {
      for (int cc = 0; cc < 3; cc++) {
        if (cc == 0)
          printf("JINF %02d TDR %02d)\t", jj, tt);
        printf("%f\t", alignpar[jj][tt][cc]);
        if (cc == 2)
          printf("%d\n", (int)(multflip[jj][tt]));
      }
    }
  }

  return;
}
void Event::ReadGainCorrection(TString filename, bool DEBUG) {

  printf("Reading Gain Correction from %s:\n", filename.Data());

  for (int jj = 0; jj < NJINF; jj++) {
    for (int tt = 0; tt < NTDRS; tt++) {
      for (int vv = 0; vv < NVAS; vv++) {
        gaincorrectionpar[jj][tt][vv][0] = 0.0;
        gaincorrectionpar[jj][tt][vv][1] = 1.0;
      }
    }
  }

  int const dimline = 255;
  char line[dimline];
  int jinfnum = 0;
  int tdrnum = 0;
  int vanum = 0;
  float dummy = 0.;

  FILE *ft = fopen(filename.Data(), "r");
  if (ft == NULL) {
    printf("Error: cannot open %s \n", filename.Data());
    return;
  } else {
    while (1) {
      if (fgets(line, dimline, ft) != NULL) {
        if (*line == '#')
          continue; /* ignore comment line */
        else {
          sscanf(line, "%d\t%d\t%d\t%f\t%f", &jinfnum, &tdrnum, &vanum, &dummy, &dummy);
          //	  printf("%d %d %d\n", NJINF, NTDRS, NVAS);
          if (jinfnum < NJINF && tdrnum < NTDRS && vanum < NVAS) {
            sscanf(line, "%d \t %d \t %d \t %f \t %f", &jinfnum, &tdrnum, &vanum,
                   &gaincorrectionpar[jinfnum][tdrnum][vanum][0], &gaincorrectionpar[jinfnum][tdrnum][vanum][1]);
          } else {
            printf("Wrong JINF/TDR/VA (%d, %d, %d): maximum is (%d,%d, %d)\n", jinfnum, tdrnum, vanum, NJINF - 1,
                   NTDRS - 1, NVAS - 1);
          }
        }
      } else {
        printf(" closing gain correction file \n");
        fclose(ft);
        break;
      }
    }
  }

  gaincorrectionnotread = false;

  //  if(DEBUG==false) return;
  // per ora (finche' il lavoro non e' finito) utile mostrare la tabellina dei TDR  con valori non di default, perchè
  // NON dovrebbero esserci!
  bool first = true;
  bool everdone = false;
  for (int jj = 0; jj < NJINF; jj++) {
    for (int tt = 0; tt < NTDRS; tt++) {
      for (int vv = 0; vv < NVAS; vv++) {
        if (gaincorrectionpar[jj][tt][vv][0] == 0.0 && gaincorrectionpar[jj][tt][vv][1] == 1.0)
          continue;
        if (first) {
          printf("***************************************\n");
          printf("***************************************\n");
          printf("Non-default gain correction parameters:\n");
        }
        first = false;
        everdone = true;
        printf("JINF %02d TDR %02d VA %02d)\t", jj, tt, vv);
        printf("%f\t", gaincorrectionpar[jj][tt][vv][0]);
        printf("%f\t", gaincorrectionpar[jj][tt][vv][1]);
        printf("\n");
      }
    }
  }
  if (everdone) {
    printf("***************************************\n");
    printf("***************************************\n");
  }

  return;
}

float Event::GetGainCorrectionPar(int jinfnum, int tdrnum, int vanum, int component) {
  if (jinfnum >= NJINF || jinfnum < 0) {
    printf("Jinf %d: not possible, the maximum is %d...\n", jinfnum, NJINF - 1);
    return -9999;
  }
  if (tdrnum >= NTDRS || tdrnum < 0) {
    printf("TDR %d: not possible, the maximum is %d...\n", tdrnum, NTDRS - 1);
    return -9999;
  }
  if (vanum >= NVAS || vanum < 0) {
    printf("VA %d: not possible, the maximum is %d...\n", vanum, NVAS - 1);
    return -9999;
  }
  if (component < 0 || component >= 3) {
    printf("Component %d not valid: it can be only up to 2\n", component);
    return -9999;
  }

  return gaincorrectionpar[jinfnum][tdrnum][vanum][component];
}

float Event::GetAlignPar(int jinfnum, int tdrnum, int component) {

  if (jinfnum >= NJINF || jinfnum < 0) {
    printf("Jinf %d: not possible, the maximum is %d...\n", jinfnum, NJINF - 1);
    return -9999;
  }
  if (tdrnum >= NTDRS || tdrnum < 0) {
    printf("TDR %d: not possible, the maximum is %d...\n", tdrnum, NTDRS - 1);
    return -9999;
  }
  if (component < 0 || component >= 3) {
    printf("Component %d not valid: it can be only up to 2\n", component);
    return -9999;
  }

  return alignpar[jinfnum][tdrnum][component];
}

float Event::GetMultiplicityFlip(int jinfnum, int tdrnum) {

  if (jinfnum >= NJINF || jinfnum < 0) {
    printf("Jinf %d: not possible, the maximum is %d...\n", jinfnum, NJINF - 1);
    return -9999;
  }
  if (tdrnum >= NTDRS || tdrnum < 0) {
    printf("TDR %d: not possible, the maximum is %d...\n", tdrnum, NTDRS - 1);
    return -9999;
  }

  return ladderconf->GetMultiplicityFlip(jinfnum, tdrnum);
  // return multflip[jinfnum][tdrnum];
}

void Event::ClearTrack() {
  // CB:
  _TrS.clear();
  _TrK.clear();
  _vertexK = make_pair(9999., 9999.);
  _vertexS = make_pair(9999., 9999.);

  _chisq = 999999999.9;
  _chisqS = 999999999.9;
  _chisqK = 999999999.9;

  _iDirS = -9999.9;
  _iDirK = -9999.9;
  _iDirZ = -9999.9;
  _iDirSerr = -9999.9;
  _iDirKerr = -9999.9;
  _iDirZerr = -9999.9;

  _mS = -9999.9;
  _mK = -9999.9;
  _mSerr = -9999.9;
  _mKerr = -9999.9;

  _theta = -9999.9;
  _phi = -9999.9;
  _thetaerr = -9999.9;
  _phierr = -9999.9;

  _S0 = -9999.9;
  _K0 = -9999.9;
  _S0err = -9999.9;
  _K0err = -9999.9;

  _v_trackS.clear();
  _v_trackK.clear();

  _v_chilayS.clear();
  _v_chilayK.clear();

  _v_trackhit.clear();

  // not to be cleared since ExcludeTDRFromTrack must be called before the event loop!
  //  _v_ladderS_to_ignore.clear();
  //  _v_ladderK_to_ignore.clear();

  for (int ii = 0; ii < NJINF; ii++) {
    ;
    for (int ss = 0; ss < 2; ss++) {
      _track_cluster_pattern[ii][ss] = 0;
    }
  }

  return;
}

void Event::ClearTrack_sf() {

  chisq_sf = 999999999.9;
  chisqS_sf = 999999999.9;
  chisqK_sf = 999999999.9;

  iDirS_sf = -9999.9;
  iDirK_sf = -9999.9;
  iDirZ_sf = -9999.9;
  iDirSerr_sf = -9999.9;
  iDirKerr_sf = -9999.9;
  iDirZerr_sf = -9999.9;

  mS_sf = -9999.9;
  mK_sf = -9999.9;
  mSerr_sf = -9999.9;
  mKerr_sf = -9999.9;

  theta_sf = -9999.9;
  phi_sf = -9999.9;
  thetaerr_sf = -9999.9;
  phierr_sf = -9999.9;

  S0_sf = -9999.9;
  K0_sf = -9999.9;
  S0err_sf = -9999.9;
  K0err_sf = -9999.9;

  v_trackS_sf.clear();
  v_trackK_sf.clear();

  v_trackErrS_sf.clear();
  v_trackErrK_sf.clear();

  v_chilayS_sf.clear();
  v_chilayK_sf.clear();

  return;
}

void Event::ExcludeTDRFromTrack(int jinfnum, int tdrnum, int side, bool verbose) {

  if (verbose)
    printf("From now on excluding JINF=%d, TDR=%d, Side=%d\n", jinfnum, tdrnum, side);

  int item = jinfnum * 100 + tdrnum;

  if (side == 0) {
    _v_ladderS_to_ignore.push_back(item);
  } else {
    _v_ladderK_to_ignore.push_back(item);
  }

  return;
}

void Event::IncludeBackTDRFromTrack(int jinfnum, int tdrnum, int side, bool verbose) {

  if (verbose)
    printf("From now on including back JINF=%d, TDR=%d, Side=%d\n", jinfnum, tdrnum, side);

  int item = jinfnum * 100 + tdrnum;

  if (side == 0) {
    _v_ladderS_to_ignore.erase(std::remove(_v_ladderS_to_ignore.begin(), _v_ladderS_to_ignore.end(), item),
                               _v_ladderS_to_ignore.end());
  } else {
    _v_ladderK_to_ignore.erase(std::remove(_v_ladderK_to_ignore.begin(), _v_ladderK_to_ignore.end(), item),
                               _v_ladderK_to_ignore.end());
  }

  return;
}

bool Event::FindTrackAndFit(int nptsS, int nptsK, bool verbose) {

  ClearTrack();

  std::vector<std::pair<int, std::pair<double, double>>> **v_cog_laddS;
  std::vector<std::pair<int, std::pair<double, double>>> **v_cog_laddK;
  v_cog_laddS = new std::vector<std::pair<int, std::pair<double, double>>> *[NJINF];
  v_cog_laddK = new std::vector<std::pair<int, std::pair<double, double>>> *[NJINF];
  for (int ii = 0; ii < NJINF; ii++) {
    v_cog_laddS[ii] = new std::vector<std::pair<int, std::pair<double, double>>>[NTDRS];
    v_cog_laddK[ii] = new std::vector<std::pair<int, std::pair<double, double>>>[NTDRS];
  }

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    Cluster *current_cluster = GetCluster(index_cluster);

    int jinfnum = current_cluster->GetJinf();
    int tdrnum = current_cluster->GetTDR();
    int item = jinfnum * 100 + tdrnum;

    int side = current_cluster->side;
    if (side == 0) {
      if (!(std::find(_v_ladderS_to_ignore.begin(), _v_ladderS_to_ignore.end(), item) != _v_ladderS_to_ignore.end())) {
        v_cog_laddS[jinfnum][tdrnum].push_back(std::make_pair(
            index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
      }
    } else {
      if (!(std::find(_v_ladderK_to_ignore.begin(), _v_ladderK_to_ignore.end(), item) != _v_ladderK_to_ignore.end())) {
        v_cog_laddK[jinfnum][tdrnum].push_back(std::make_pair(
            index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
      }
    }
  }

  std::vector<std::pair<int, std::pair<double, double>>>
      vecS; // actually used just for compatibility with the telescopic function
  std::vector<std::pair<int, std::pair<double, double>>>
      vecK; // actually used just for compatibility with the telescopic function
  double chisq = CombinatorialFit(v_cog_laddS, v_cog_laddK, NJINF, NTDRS, vecS, vecK, nptsS, nptsK, verbose);
  //  printf("chisq = %f\n", chisq);

  bool ret = true;
  if (chisq >= 999999999.9)
    ret = false;
  else if (chisq < -0.000000001)
    ret = false;

  return ret;
}

bool Event::FindHigherChargeTrackAndFit(int nptsS, double threshS, int nptsK, double threshK, bool verbose) {

  ClearTrack();

  std::vector<std::pair<int, std::pair<double, double>>> **v_cog_laddS;
  std::vector<std::pair<int, std::pair<double, double>>> **v_cog_laddK;

  std::vector<double> **v_q_laddS;
  std::vector<double> **v_q_laddK;

  v_cog_laddS = new std::vector<std::pair<int, std::pair<double, double>>> *[NJINF];
  v_cog_laddK = new std::vector<std::pair<int, std::pair<double, double>>> *[NJINF];
  v_q_laddS = new std::vector<double> *[NJINF];
  v_q_laddK = new std::vector<double> *[NJINF];
  for (int ii = 0; ii < NJINF; ii++) {
    v_cog_laddS[ii] = new std::vector<std::pair<int, std::pair<double, double>>>[NTDRS];
    v_cog_laddK[ii] = new std::vector<std::pair<int, std::pair<double, double>>>[NTDRS];
    v_q_laddS[ii] = new std::vector<double>[NTDRS];
    v_q_laddK[ii] = new std::vector<double>[NTDRS];
  }

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    Cluster *current_cluster = GetCluster(index_cluster);

    int jinfnum = current_cluster->GetJinf();
    int tdrnum = current_cluster->GetTDR();
    int item = jinfnum * 100 + tdrnum;

    int side = current_cluster->side;
    if (side == 0) {
      if (!(std::find(_v_ladderS_to_ignore.begin(), _v_ladderS_to_ignore.end(), item) != _v_ladderS_to_ignore.end())) {
        if (current_cluster->GetTotSN() > threshS) {
          if (v_q_laddS[jinfnum][tdrnum].size() == 0) {
            v_cog_laddS[jinfnum][tdrnum].push_back(std::make_pair(
                index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
            v_q_laddS[jinfnum][tdrnum].push_back(current_cluster->GetCharge());
          } else {
            if (current_cluster->GetCharge() > v_q_laddS[jinfnum][tdrnum][0]) {
              v_cog_laddS[jinfnum][tdrnum][0] =
                  std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(),
                                                               GetAlignPar(jinfnum, tdrnum, 2)));
              v_q_laddS[jinfnum][tdrnum][0] = current_cluster->GetCharge();
            }
          }
        }
      }
    } else {
      if (!(std::find(_v_ladderK_to_ignore.begin(), _v_ladderK_to_ignore.end(), item) != _v_ladderK_to_ignore.end())) {
        if (current_cluster->GetTotSN() > threshK) {
          if (v_q_laddK[jinfnum][tdrnum].size() == 0) {
            v_cog_laddK[jinfnum][tdrnum].push_back(std::make_pair(
                index_cluster, std::make_pair(current_cluster->GetAlignedPosition(), GetAlignPar(jinfnum, tdrnum, 2))));
            v_q_laddK[jinfnum][tdrnum].push_back(current_cluster->GetCharge());
          } else {
            if (current_cluster->GetCharge() > v_q_laddK[jinfnum][tdrnum][0]) {
              v_cog_laddK[jinfnum][tdrnum][0] =
                  std::make_pair(index_cluster, std::make_pair(current_cluster->GetAlignedPosition(),
                                                               GetAlignPar(jinfnum, tdrnum, 2)));
              v_q_laddK[jinfnum][tdrnum][0] = current_cluster->GetCharge();
            }
          }
        }
      }
    }
  }

  // let's use CombinatorialFit just for simplicity but the vector above are with at most one cluster per ladder
  std::vector<std::pair<int, std::pair<double, double>>>
      vecS; // actually used just for compatibility with the telescopic function
  std::vector<std::pair<int, std::pair<double, double>>>
      vecK; // actually used just for compatibility with the telescopic function
  double chisq = CombinatorialFit(v_cog_laddS, v_cog_laddK, NJINF, NTDRS, vecS, vecK, nptsS, nptsK, verbose);
  //  printf("chisq = %f\n", chisq);

  bool ret = true;
  if (chisq >= 999999999.9)
    ret = false;
  else if (chisq < -0.000000001)
    ret = false;

  return ret;
}

double Event::CombinatorialFit(std::vector<std::pair<int, std::pair<double, double>>> **v_cog_laddS,
                               std::vector<std::pair<int, std::pair<double, double>>> **v_cog_laddK, int ijinf,
                               int itdr, std::vector<std::pair<int, std::pair<double, double>>> v_cog_trackS,
                               std::vector<std::pair<int, std::pair<double, double>>> v_cog_trackK, int nptsS,
                               int nptsK, bool verbose) {
  //    printf("ijinf = %d, itdr = %d\n", ijinf, itdr);

  if (itdr == 0) {
    itdr = NTDRS;
    ijinf--;
  }

  if (ijinf != 0) { // recursion
    int sizeS = v_cog_laddS[ijinf - 1][itdr - 1].size();
    int sizeK = v_cog_laddK[ijinf - 1][itdr - 1].size();
    //        printf("size: %d %d\n", sizeS, sizeK);
    for (int ss = 0; ss < std::max(sizeS, 1); ss++) {
      for (int kk = 0; kk < std::max(sizeK, 1); kk++) {
        //	printf("ss=%d, kk=%d\n", ss, kk);
        std::vector<std::pair<int, std::pair<double, double>>> _vecS = v_cog_trackS;
        std::vector<std::pair<int, std::pair<double, double>>> _vecK = v_cog_trackK;
        if (sizeS > 0) {
          _vecS.push_back(v_cog_laddS[ijinf - 1][itdr - 1].at(ss));
          if (verbose)
            printf("S_push_back: %f %f\n", v_cog_laddS[ijinf - 1][itdr - 1].at(ss).second.first,
                   v_cog_laddS[ijinf - 1][itdr - 1].at(ss).second.second);
        }
        if (sizeK > 0) {
          _vecK.push_back(v_cog_laddK[ijinf - 1][itdr - 1].at(kk));
          if (verbose)
            printf("K_push_back: %f %f\n", v_cog_laddK[ijinf - 1][itdr - 1].at(kk).second.first,
                   v_cog_laddK[ijinf - 1][itdr - 1].at(kk).second.second);
        }
        CombinatorialFit(v_cog_laddS, v_cog_laddK, ijinf, itdr - 1, _vecS, _vecK, nptsS, nptsK, verbose);
      }
    }
  } else { // now is time to fit!
    if (verbose) {
      printf("new track to fit\n");
      printf("S: ");
      for (int ss = 0; ss < (int)(v_cog_trackS.size()); ss++) {
        printf("(%f,%f)", v_cog_trackS.at(ss).second.first, v_cog_trackS.at(ss).second.second);
      }
      printf("\n");
      printf("K: ");
      for (int kk = 0; kk < (int)(v_cog_trackK.size()); kk++) {
        printf("(%f,%f)", v_cog_trackK.at(kk).second.first, v_cog_trackK.at(kk).second.second);
      }
      printf("\n");
    }
    if ((int)(v_cog_trackS.size()) >= nptsS && (int)(v_cog_trackK.size()) >= nptsK) {
      double chisq = SingleFit(v_cog_trackS, v_cog_trackK, verbose);
      if (chisq < _chisq) {
        if (verbose)
          printf("Best track) new chisq %f, old one %f\n", chisq, _chisq);
        AssignAsBestTrackFit();
      }
    }
    if (verbose) {
      printf("----------------------\n");
    }
  }

  return _chisq;
}

void Event::AssignAsBestTrackFit() {

  _chisq = chisq_sf;
  _chisqS = chisqS_sf;
  _chisqK = chisqK_sf;
  _mS = mS_sf;
  _mK = mK_sf;
  _mSerr = mSerr_sf;
  _mKerr = mKerr_sf;
  _iDirS = iDirS_sf;
  _iDirK = iDirK_sf;
  _iDirZ = iDirZ_sf;
  _iDirSerr = iDirSerr_sf;
  _iDirKerr = iDirKerr_sf;
  _iDirZerr = iDirZerr_sf;
  _theta = theta_sf;
  _phi = phi_sf;
  _thetaerr = thetaerr_sf;
  _phierr = phierr_sf;
  _S0 = S0_sf;
  _K0 = K0_sf;
  _S0err = S0err_sf;
  _K0err = K0err_sf;
  _v_trackS = v_trackS_sf;
  _v_trackK = v_trackK_sf;
  _v_chilayS = v_chilayS_sf;
  _v_chilayK = v_chilayK_sf;

  StoreTrackClusterPatterns();
  FillHitVector();

  return;
}

double Event::SingleFit(std::vector<std::pair<int, std::pair<double, double>>> vS,
                        std::vector<std::pair<int, std::pair<double, double>>> vK, bool verbose) {

  ClearTrack_sf();

  /* debug
     static TH1F hchi("hchi", "hchi", 1000, 0.0, 10.0);
     static TH1F htheta("htheta", "htheta", 1000, -TMath::Pi()/2.0, TMath::Pi()/2.0);
     static TH1F hphi("hphi", "hphi", 1000, -TMath::Pi(), TMath::Pi());
     static TH1F hs0("hs0", "hs0", 1000, -1000.0, 1000.0);
     static TH1F hk0("hk0", "hk0", 1000, -1000.0, 1000.0);
  */

  chisq_sf = SingleFit(vS, vK, v_chilayS_sf, v_chilayK_sf, theta_sf, thetaerr_sf, phi_sf, phierr_sf, iDirS_sf,
                       iDirSerr_sf, iDirK_sf, iDirKerr_sf, iDirZ_sf, iDirZerr_sf, mS_sf, mSerr_sf, mK_sf, mKerr_sf,
                       S0_sf, S0err_sf, K0_sf, K0err_sf, chisqS_sf, chisqK_sf, verbose);

  /*
    hchi.Fill(log10(chisq_sf));
    htheta.Fill(theta_sf);
    hphi.Fill(phi_sf);
    hs0.Fill(S0_sf);
    hk0.Fill(K0_sf);
  */

  if (verbose)
    printf("chisq: %f, chisqS: %f, chisqK: %f, theta = %f, phi = %f, S0 = %f, K0 = %f\n", chisq_sf, chisqS_sf,
           chisqK_sf, theta_sf, phi_sf, S0_sf, K0_sf);

  return chisq_sf;
}

double Event::SingleFit(std::vector<std::pair<int, std::pair<double, double>>> vS,
                        std::vector<std::pair<int, std::pair<double, double>>> vK, std::vector<double> &v_chilayS,
                        std::vector<double> &v_chilayK, double &theta, double &thetaerr, double &phi, double &phierr,
                        double &iDirX, double &iDirXerr, double &iDirY, double &iDirYerr, double &iDirZ,
                        double &iDirZerr, double &mX, double &mXerr, double &mY, double &mYerr, double &X0,
                        double &X0err, double &Y0, double &Y0err, double &chisqS, double &chisqK, bool verbose) {

  v_trackS_sf = vS;
  v_trackK_sf = vK;

  for (int ic = 0; ic < (int)vS.size(); ic++)
    v_trackErrS_sf.push_back(GetCluster(vS.at(ic).first)->GetNominalResolution(0));
  for (int ic = 0; ic < (int)vK.size(); ic++)
    v_trackErrK_sf.push_back(GetCluster(vK.at(ic).first)->GetNominalResolution(1));

  Double_t corrXmX, corrYmY;

  // The fit is done independently in the two X and Y views
  // The fit returns X0 and mX (the angular coefficient)
  // mX = vx/vz, where vx and vz are the projection of the straight line versors into the X and Z axis
  // Considering the definition of directive cosines
  // dirX = vx / |v|   with |v| = sqrt( vx*vx + vy*vy + vz*vz)
  // dirY = vy / |v|   with |v| = sqrt( vx*vx + vy*vy + vz*vz)
  // dirZ = vZ / |v|   with |v| = sqrt( vx*vx + vy*vy + vz*vz)
  // dirX*dirX + dirY*dirY + dirZ*dirZ = 1
  // then we have
  // mX = dirX/dirZ
  // mY = dirY/dirZ
  // and after both two fits, we can calculate
  // dirZ = 1  / sqrt( 1 + mX*mX + mY*mY)
  // dirX = mX / sqrt( 1 + mX*mX + mY*mY)
  // dirY = mY / sqrt( 1 + mX*mX + mY*mY)

#ifdef USEMINUIT
  // Minuit fit
  static TMinuit *minuit = NULL;
  if (!minuit)
    minuit = new TMinuit();
  //  minuit->Clear();
  minuit->SetPrintLevel((int)(verbose)-1);
  minuit->SetFCN(_fcn);

  Double_t arglist[10];
  Int_t ierflg = 0;
  arglist[0] = 1; // chi-sq err-def
  minuit->mnexcm("SET ERR", arglist, 1, ierflg);

  // Set starting values and step sizes for parameters
  static Double_t vstart[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  static Double_t step[5] = {1.0e-5, 1.0e-5, 1.0e-5, 1.0e-5};
  minuit->mnparm(0, "mS", vstart[0], step[0], 0, 0, ierflg);
  minuit->mnparm(1, "mK", vstart[1], step[1], 0, 0, ierflg);
  minuit->mnparm(2, "S0", vstart[2], step[2], 0, 0, ierflg);
  minuit->mnparm(3, "K0", vstart[3], step[3], 0, 0, ierflg);

  // Now ready for minimization step
  arglist[0] = 50000;
  arglist[1] = 1.;
  minuit->mnexcm("MIGRAD", arglist, 2, ierflg);

  // Print results
  // Double_t amin,edm,errdef;
  // Int_t nvpar,nparx,icstat;
  // minuit->mnstat(amin, edm, errdef, nvpar, nparx, icstat);
  // minuit->mnprin(3,amin);

  minuit->GetParameter(0, mX, mXerr);
  minuit->GetParameter(1, mY, mYerr);
  minuit->GetParameter(2, X0, X0err);
  minuit->GetParameter(3, Y0, Y0err);

  Double_t covmat[4][4];
  minuit->mnemat(&covmat[0][0], 4);
  corrXmX = -covmat[0][2] / (sqrt(covmat[0][0] * covmat[2][2])); // minus is because they shold be anticorrelated
  corrYmY = -covmat[1][3] / (sqrt(covmat[1][1] * covmat[3][3]));

#else
  // Analytical Fit

  // Viviana claimed that everything following was REVERSED (S<->K). I imported the newer version, erasing Viviana's
  // one. Check if is correct.

  Double_t S1 = 0, Sz = 0, Szz = 0;

  // Fit X
  int nx = (int)(vS.size());
  // Why 5 loops when you can do just one... WHY?!?!?!?
  /*
    Double_t S1=0;   for(int i=0; i<(int)nx; i++) S1  += 1./pow(Cluster::GetNominalResolution(0),2);
    Double_t Sz=0;   for(int i=0; i<(int)nx; i++) Sz  += vS.at(i).second.second/pow(Cluster::GetNominalResolution(0),2);
    Double_t Szz=0;  for(int i=0; i<(int)nx; i++) Szz +=
    pow(vS.at(i).second.second,2)/pow(Cluster::GetNominalResolution(0),2); Double_t Sx=0;   for(int i=0; i<(int)nx; i++)
    Sx  += vS.at(i).second.first/pow(Cluster::GetNominalResolution(0),2); Double_t Szx=0;  for(int i=0; i<(int)nx; i++)
    Szx += (vS.at(i).second.first*vS.at(i).second.second)/pow(Cluster::GetNominalResolution(0),2);
  */
  S1 = 0;
  Sz = 0;
  Szz = 0;
  Double_t Sx = 0, Szx = 0;
  for (int i = 0; i < (int)nx; i++) {
    if (verbose)
      printf("SingleFit: cluster %d, Jinf: %d, TDR %d: Z=%f, X=%f\n", vS.at(i).first,
             GetCluster(vS.at(i).first)->GetJinf(), GetCluster(vS.at(i).first)->GetTDR(), vS.at(i).second.second,
             vS.at(i).second.first);
    double sigmasq = pow(GetCluster(vS.at(i).first)->GetNominalResolution(0), 2);
    S1 += 1. / sigmasq;
    Sz += vS.at(i).second.second / sigmasq;
    Szz += pow(vS.at(i).second.second, 2) / sigmasq;
    Sx += vS.at(i).second.first / sigmasq;
    Szx += (vS.at(i).second.first * vS.at(i).second.second) / sigmasq;
  }

  Double_t Dx = S1 * Szz - Sz * Sz;
  X0 = (Sx * Szz - Sz * Szx) / Dx;
  // iDirX = (S1*Szx-Sz*Sx)/Dx; iDirX=-iDirX;
  mX = (S1 * Szx - Sz * Sx) / Dx; // mX=-mX;
  X0err = sqrt(Szz / Dx);
  // iDirXerr = sqrt(S1/Dx);
  mXerr = sqrt(S1 / Dx);
  corrXmX = -Sz / sqrt(Szz * S1);

  // Fit Y
  int ny = (int)(vK.size());
  // Why 5 loops when you can do just one... WHY?!?!?!?
  /*
    S1=0;   for(int i=0; i<(int)ny; i++) S1  += 1./pow(Cluster::GetNominalResolution(1),2);
    Sz=0;   for(int i=0; i<(int)ny; i++) Sz  += vK.at(i).second.second/pow(Cluster::GetNominalResolution(1),2);
    Szz=0;  for(int i=0; i<(int)ny; i++) Szz += pow(vK.at(i).second.second,2)/pow(Cluster::GetNominalResolution(1),2);
    Double_t Sy=0;   for(int i=0; i<(int)ny; i++) Sy  += vK.at(i).second.first/pow(Cluster::GetNominalResolution(1),2);
    Double_t Szy=0;  for(int i=0; i<(int)ny; i++) Szy +=
    (vK.at(i).second.first*vK.at(i).second.second)/pow(Cluster::GetNominalResolution(1),2);
  */
  S1 = 0;
  Sz = 0;
  Szz = 0;
  Double_t Sy = 0, Szy = 0;
  for (int i = 0; i < (int)ny; i++) {
    if (verbose)
      printf("SingleFit: cluster %d, Jinf: %d, TDR %d: Z=%f, Y=%f\n", vK.at(i).first,
             GetCluster(vK.at(i).first)->GetJinf(), GetCluster(vK.at(i).first)->GetTDR(), vK.at(i).second.second,
             vK.at(i).second.first);
    double sigmasq = pow(GetCluster(vK.at(i).first)->GetNominalResolution(1), 2);
    S1 += 1. / sigmasq;
    Sz += vK.at(i).second.second / sigmasq;
    Szz += pow(vK.at(i).second.second, 2) / sigmasq;
    Sy += vK.at(i).second.first / sigmasq;
    Szy += (vK.at(i).second.first * vK.at(i).second.second) / sigmasq;
  }
  Double_t Dy = S1 * Szz - Sz * Sz;
  Y0 = (Sy * Szz - Sz * Szy) / Dy;
  // iDirY = (S1*Szy-Sz*Sy)/Dy; iDirY=-iDirY;
  mY = (S1 * Szy - Sz * Sy) / Dy; // mY=-mY;
  Y0err = sqrt(Szz / Dy);
  // iDirYerr = sqrt(S1/Dy);
  mYerr = sqrt(S1 / Dy);
  corrYmY = -Sz / sqrt(Szz * S1);
#endif

  //  printf("%f %f %f %f\n", mX, mY, X0, Y0);

  //    dirX = mX * dirZ                 -->       dirX = mX / sqrt(1 + mX^2 + mY^2)
  //    dirY = mY * dirZ                 -->       dirX = mY / sqrt(1 + mX^2 + mY^2)
  //    dirZ = 1./sqrt(1 + mX^2 + mY^2)

  //    ∂dirX/∂mX = +dirZ^3 * (1+mY^2)        ∂dirY/∂mX = -dirZ^3 * mX * mY       ∂dirZ/∂mX = -dirZ^3 * mX
  //    ∂dirX/∂mY = -dirZ^3 * mX * mY         ∂dirY/∂mY = +dirZ^3 * (1+mX^2)      ∂dirZ/∂mY = -dirZ^3 * mY
  //    corr(mX,mY)=0  since they come from independent fits

  iDirZ = 1. / sqrt(1 + mX * mX + mY * mY);
  iDirX = mX * iDirZ;
  iDirY = mY * iDirZ;
  Double_t dDirXdmX = +iDirZ * iDirZ * iDirZ * (1 + mY * mY);
  Double_t dDirXdmY = -iDirZ * iDirZ * iDirZ * mX * mY;
  Double_t dDirYdmX = -iDirZ * iDirZ * iDirZ * mX * mY;
  Double_t dDirYdmY = +iDirZ * iDirZ * iDirZ * (1 + mX * mX);
  Double_t dDirZdmX = -iDirZ * iDirZ * iDirZ * mX;
  Double_t dDirZdmY = -iDirZ * iDirZ * iDirZ * mY;
  iDirXerr = sqrt(pow(dDirXdmX * mXerr, 2) + pow(dDirXdmY * mYerr, 2));
  iDirYerr = sqrt(pow(dDirYdmX * mXerr, 2) + pow(dDirYdmY * mYerr, 2));
  iDirZerr = sqrt(pow(dDirZdmX * mXerr, 2) + pow(dDirZdmY * mYerr, 2));

  //------------------------------------------------------------------------------------------

  theta = std::acos(iDirZ);
  phi = std::atan2(iDirY, iDirX);

  // should not happen ------------
  if (theta < 0) {
    theta = fabs(theta);
    phi += TMath::Pi();
  }
  if (phi > TMath::Pi()) {
    phi -= 2.0 * TMath::Pi();
  }
  if (phi < -TMath::Pi()) {
    phi += 2.0 * TMath::Pi();
  }
  //------------------------------

  // theta = acos( dirZ )        --> theta(mX,mY) = acos( 1./sqrt(1+mX*mX+mY*mY) )
  // phi = atan (dirY/dirX)      --> phi(mX,mY)   = atan( mY/mX )
  //
  //∂phi/∂mX = -mY / (mX^2 + mY^2)                            ∂phi/∂mY = +mX / (mX^2 + mY^2)
  //∂theta/∂mX = [(1+mX^2+mY^2)*sqrt(mX^2+mY^2)]^{-1}         ∂theta/∂mY = ∂theta/∂mX

  double dthetadmX = 1. / ((1 + mX * mX * mY * mY) * sqrt(mX * mX + mY * mY));
  double dthetadmY = 1. / ((1 + mX * mX * mY * mY) * sqrt(mX * mX + mY * mY));
  double dphidmX = -mY / (mX * mX + mY * mY);
  double dphidmY = +mX / (mX * mX + mY * mY);
  thetaerr = sqrt(pow(dthetadmX * mXerr, 2) + pow(dthetadmY * mYerr, 2));
  phierr = sqrt(pow(dphidmX * mXerr, 2) + pow(dphidmY * mYerr, 2));

  //------------------------------------------------------------------------------------------

  int ndofS = vS.size() - 2;
  int ndofK = vK.size() - 2;

  double chisqS_nored = 0.0;
  double chisqK_nored = 0.0;
  double chisq = 0.0;

  // Viviana claimed REVERSED X->Y. I imported the newew version, erasing her one. We need to check
  if (ndofS >= 0) {
    chisqS_nored = _compchisq(vS, v_chilayS, mX, X0, v_trackErrS_sf);
    chisq += chisqS_nored;
  }
  if (ndofK >= 0) {
    chisqK_nored = _compchisq(vK, v_chilayK, mY, Y0, v_trackErrK_sf);
    chisq += chisqK_nored;
  }

  int ndof = ndofS + ndofK;
  double ret = chisq / ndof;
  if (ndof <= 0) {
    if (ndofS > 0)
      ret = chisqS_nored / ndofS;
    else if (ndofK > 0)
      ret = chisqK_nored / ndofK;
    else if (ndof == 0)
      ret = 0.0;
    else
      ret = -1.0;
  }
  // Viviana claimed REVERSED X->Y. I imported the newew version, erasing her one. We need to check
  chisqS = -1.0;
  if (ndofS > 0)
    chisqS = chisqS_nored / ndofS;
  else if (ndofS == 0)
    chisqS = 0.0;
  chisqK = -1.0;
  if (ndofK > 0)
    chisqK = chisqK_nored / ndofK;
  else if (ndofK == 0)
    chisqK = 0.0;

  if (verbose)
    printf("chisq/ndof = %f/%d = %f, chisqS/ndofS = %f/%d = %f, chisqK/ndofK = %f/%d = %f\n", chisq, ndof, ret,
           chisqS_nored, ndofS, chisqS, chisqK_nored, ndofK, chisqK);

  return ret;
}

#ifdef USEMINUIT
void _fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {

  std::vector<double> v_chilay;

  f = _compchisq(v_trackS_sf, v_chilay, par[0], par[2], v_trackErrS_sf) +
      _compchisq(v_trackK_sf, v_chilay, par[1], par[3], v_trackErrK_sf);

  return;
}
#endif

/*
double _compchisq(std::vector<std::pair<int, std::pair<double, double> > > vec, std::vector<double>& v_chilay, double
imS, double iS, double iSerr, double Z0){

  v_chilay.clear();

  static Double_t chisq;
  chisq = 0.0;
  static Double_t delta;
  delta = 0.0;
  for (int pp=0; pp<(int)(vec.size()); pp++) {
    delta = (vec.at(pp).second.first - _func(vec.at(pp).second.second, imS, iS, Z0))/iSerr;
    v_chilay.push_back(delta*delta);
    chisq += delta*delta;
  }

  return chisq;
}
*/

double _compchisq(std::vector<std::pair<int, std::pair<double, double>>> vec, std::vector<double> &v_chilay, double imS,
                  double iS, std::vector<double> iSerr, double Z0) {

  v_chilay.clear();

  static Double_t chisq;
  chisq = 0.0;
  static Double_t delta;
  delta = 0.0;
  for (int pp = 0; pp < (int)(vec.size()); pp++) {
    delta = (vec.at(pp).second.first - _func(vec.at(pp).second.second, imS, iS, Z0)) / iSerr.at(pp);
    v_chilay.push_back(delta * delta);
    chisq += delta * delta;
  }

  return chisq;
}

Double_t _func(double z, double imS, double iS, double Z0) { return iS + (z - Z0) * imS; }

double Event::ExtrapolateTrack(double z, int component) {
  if (component == 0)
    return _func(z, _mS, _S0);
  else if (component == 1)
    return _func(z, _mK, _K0);
  else
    return -9999.99;
}

bool Event::IsClusterUsedInTrack(int index_cluster) {

  //  printf("IsClusterUsedInTrack\n");

  for (int ii = 0; ii < (int)(_v_trackS.size()); ii++) {
    //    printf("%d cluster (S) in track\n", _v_trackS.at(ii).first);
    if (_v_trackS.at(ii).first == index_cluster)
      return true;
  }
  for (int ii = 0; ii < (int)(_v_trackK.size()); ii++) {
    //    printf("%d cluster (K) in track\n", _v_trackK.at(ii).first);
    if (_v_trackK.at(ii).first == index_cluster)
      return true;
  }

  return false;
}

void Event::StoreTrackClusterPatterns() {

  for (int ii = 0; ii < NJINF; ii++) {
    ;
    for (int ss = 0; ss < 2; ss++) {
      _track_cluster_pattern[ii][ss] = 0;
    }
  }

  std::vector<std::pair<int, std::pair<double, double>>> _v_track_tmp;
  _v_track_tmp.clear();

  for (int i_side = 0; i_side < 2; i_side++) {
    if (i_side == 0)
      _v_track_tmp = _v_trackS;
    else if (i_side == 1)
      _v_track_tmp = _v_trackK;

    for (int ii = 0; ii < (int)(_v_track_tmp.size()); ii++) {
      int index_cluster = _v_track_tmp.at(ii).first;
      Cluster *cl = GetCluster(index_cluster);
      int tdrnum = cl->GetTDR();
      int jinfnum = cl->GetJinf();
      //      printf("JINF %d, TDR %d , %d cluster (%d) in track\n", jinfnum, tdrnum, index_cluster, i_side);

      //      unsigned long long int tdr_index = (int)(pow(10.0, (double)(tdrnum)));
      int tdr_index = 1 << tdrnum;
      //      printf("TDR %d %d --> %s\n", tdrnum, i_side, std::bitset<NTDRS>(tdr_index).to_string().c_str());
      if (jinfnum < NJINF && i_side < 2)
        _track_cluster_pattern[jinfnum][i_side] += tdr_index;
      else
        printf("Problem: Jinf %d out of %d and side %d out of %d\n", jinfnum, NJINF, i_side, 2);
    }
  }

  return;
}

bool Event::IsTDRInTrack(int side, int tdrnum, int jinfnum) {
  //  return ((bool)(((unsigned long long int)(_track_cluster_pattern[jinfnum][side]/((int)(pow((double)10,
  //  (double)tdrnum)))))%10));
  return ((bool)(_track_cluster_pattern[jinfnum][side] & (1 << tdrnum)));
}

void Event::FillHitVector() {

  _v_trackhit.clear();

  std::pair<int, int> coopair[NJINF * NTDRS];
  for (int pp = 0; pp < NJINF * NTDRS; pp++) {
    coopair[pp].first = -1;
    coopair[pp].second = -1;
  }

  for (int index_cluster = 0; index_cluster < NClusTot; index_cluster++) {

    if (!IsClusterUsedInTrack(index_cluster))
      continue;
    Cluster *current_cluster = GetCluster(index_cluster);

    int ladder = current_cluster->ladder;
    int side = current_cluster->side;

    if (side == 0) {
      coopair[ladder].first = index_cluster;
    } else {
      coopair[ladder].second = index_cluster;
    }
  }

  for (int tt = 0; tt < NJINF * NTDRS; tt++) {
    if (coopair[tt].first >= 0 || coopair[tt].second >= 0) {
      _v_trackhit.push_back(std::make_pair(tt, coopair[tt]));
    }
  }

  return;
}

struct sort_pred {
  bool operator()(const std::pair<int, double> &left, const std::pair<int, double> &right) {
    return left.second < right.second;
  }
};

double Event::RefineTrack(double nsigmaS, int nptsS, double nsigmaK, int nptsK, bool verbose) {

  std::vector<std::pair<int, std::pair<double, double>>> _v_trackS_tmp = _v_trackS;
  std::vector<std::pair<int, std::pair<double, double>>> _v_trackK_tmp = _v_trackK;

  std::vector<std::pair<int, double>> _v_chilayS_tmp;
  for (unsigned int ii = 0; ii < _v_chilayS.size(); ii++) {
    _v_chilayS_tmp.push_back(std::make_pair(ii, _v_chilayS.at(ii)));
  }
  std::sort(_v_chilayS_tmp.begin(), _v_chilayS_tmp.end(), sort_pred());

  std::vector<std::pair<int, double>> _v_chilayK_tmp;
  for (unsigned int ii = 0; ii < _v_chilayK.size(); ii++) {
    _v_chilayK_tmp.push_back(std::make_pair(ii, _v_chilayK.at(ii)));
  }
  std::sort(_v_chilayK_tmp.begin(), _v_chilayK_tmp.end(), sort_pred());

  if (((int)(_v_trackS_tmp.size())) > nptsS + 1) { // so that even removing one we have at least nptsS hits
    if (sqrt(_v_chilayS_tmp.at(_v_chilayS_tmp.size() - 1).second) >
        nsigmaS) { // if the worst residual is above threshold is removed
      _v_trackS_tmp.erase(_v_trackS_tmp.begin() + _v_chilayS_tmp.at(_v_chilayS_tmp.size() - 1).first);
    }
  }
  if (((int)(_v_trackK_tmp.size())) > nptsK + 1) { // so that even removing one we have at least nptsK hits
    if (sqrt(_v_chilayK_tmp.at(_v_chilayK_tmp.size() - 1).second) >
        nsigmaK) { // if the worst residual is above threshold is removed
      _v_trackK_tmp.erase(_v_trackK_tmp.begin() + _v_chilayK_tmp.at(_v_chilayK_tmp.size() - 1).first);
    }
  }

  double ret = SingleFit(_v_trackS_tmp, _v_trackK_tmp, false);
  AssignAsBestTrackFit();

  return ret;
}

// A TRUNCATED MEAN WOULD BE BETTER BUT STICAZZI FOR NOW...
double Event::GetChargeTrack(int side) {

  if (side < 0 || side > 1) {
    printf("Not a valid side: %d\n", side);
    return -99999.9;
  }

  int npts = 0;
  double charge = 0.0;

  for (unsigned int ii = 0; ii < _v_trackhit.size(); ii++) {
    int index_cluster = -1;
    if (side == 0)
      index_cluster = _v_trackhit.at(ii).second.first;
    else
      index_cluster = _v_trackhit.at(ii).second.second;
    if (index_cluster >= 0) {
      Cluster *cl = GetCluster(index_cluster);
      charge += cl->GetCharge();
      npts++;
    }
  }

  charge /= npts;

  return charge;
}

double Event::GetCalPed_PosNum(int tdrnum, int channel, int Jinfnum) { return CalPed[Jinfnum][tdrnum][channel]; }

double Event::GetCalSigma_PosNum(int tdrnum, int channel, int Jinfnum) { return CalSigma[Jinfnum][tdrnum][channel]; }

double Event::GetRawSignal_PosNum(int tdrnum, int channel, int Jinfnum) {
  return RawSignal[Jinfnum][tdrnum][channel];
}

double Event::GetCalStatus_PosNum(int tdrnum, int channel, int Jinfnum) { return CalStatus[Jinfnum][tdrnum][channel]; }

double Event::GetCN_PosNum(int tdrnum, int va, int Jinfnum) {

  // Viviana: hardcoded n channel
  // array dimension was 1024
  short int array[4096];
  float arraySoN[4096];
  float pede[4096];
  int status[4096];

  for (int chan = 0; chan < 4096; chan++) {
    array[chan] = RawSignal[Jinfnum][tdrnum][chan];
    arraySoN[chan] = RawSoN[Jinfnum][tdrnum][chan];
    pede[chan] = CalPed[Jinfnum][tdrnum][chan];
    status[chan] = CalStatus[Jinfnum][tdrnum][chan];
  }

  // Viviana: hardcoded number of channels per VA
  // MD: but why '256' hardcoded' and not NCHAVA?
  //  return ComputeCN(64, &(array[va*64]), &(pede[va*64]), &(arraySoN[va*64]), &(status[va*64]));
  return ComputeCN(256, &(array[va * 256]), &(pede[va * 256]), &(arraySoN[va * 256]), &(status[va * 256]));
}

float Event::GetRawSoN_PosNum(int tdrnum, int channel, int Jinfnum) {
  return (RawSignal[Jinfnum][tdrnum][channel] - CalPed[Jinfnum][tdrnum][channel]) /
         CalSigma[Jinfnum][tdrnum][channel];
}

double Event::GetCalPed(RHClass *rh, int tdrnum, int channel, int Jinfnum) {
  int tdrnumraw = rh->FindPos(tdrnum + 100 * Jinfnum);
  return GetCalPed_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::GetCalSigma(RHClass *rh, int tdrnum, int channel, int Jinfnum) {
  int tdrnumraw = rh->FindPos(tdrnum + 100 * Jinfnum);
  return GetCalSigma_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::GetRawSignal(RHClass *rh, int tdrnum, int channel, int Jinfnum) {
  int tdrnumraw = rh->FindPos(tdrnum + 100 * Jinfnum);
  return GetRawSignal_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::GetCN(RHClass *rh, int tdrnum, int va, int Jinfnum) {
  int tdrnumraw = rh->FindPos(tdrnum + 100 * Jinfnum);
  return GetCN_PosNum(tdrnumraw, va, Jinfnum);
}

double Event::GetCalStatus(RHClass *rh, int tdrnum, int va, int Jinfnum) {
  int tdrnumraw = rh->FindPos(tdrnum + 100 * Jinfnum);
  return GetCalStatus_PosNum(tdrnumraw, va, Jinfnum);
}

float Event::GetRawSoN(RHClass *rh, int tdrnum, int channel, int Jinfnum) {
  int tdrnumraw = rh->FindPos(tdrnum + 100 * Jinfnum);
  return GetRawSoN_PosNum(tdrnumraw, channel, Jinfnum);
}

double Event::ComputeCN(int size, short int *RawSignal, float *pede, float *RawSoN, int *status, double threshold) {

  double mean = 0.0;
  int n = 0;

  for (int ii = 0; ii < size; ii++) {
    // printf("%d %f %f %d\n", ii, RawSoN[ii], threshold, status[ii]);
    if (RawSoN[ii] < threshold && status[ii] == 0) { // to avoid real signal...
      n++;
      //      printf("    %d) %f %f\n", ii, RawSignal[ii]/8.0, pede[ii]);
      mean += (RawSignal[ii] - pede[ii]);
    }
  }
  if (n > 1) {
    mean /= n;
  } else { // let's try again with an higher threshold
    mean = ComputeCN(size, RawSignal, pede, RawSoN, status, threshold + 1.0);
  }
  //  printf("    CN = %f\n", mean);

  return mean;
}
//------------CB: Qui iniziano le cose che ho aggiunto------------//
bool Event::FindTracksAndVertex(bool vertmode = 0) {
  ClearTrack(); // tra le altre cose fa il clear di _TrS e _TrK
  ////////////////////////
  // PHASE 1.a: Hits Vector//
  ////////////////////////
  int NS = 0, NK = 0;
  // in these vectors we collect the positions of every cluster detected as hits
  std::vector<std::pair<int, std::pair<double, double>>> v_hitsS;
  std::vector<std::pair<int, std::pair<double, double>>> v_hitsK;
  // this loop make all the work (copied from FindTrackAndFit())
  for (int ic = 0; ic < NClusTot; ic++) {
    Cluster *cc = GetCluster(ic);

    int jinfnum = cc->GetJinf();
    int tdrnum = cc->GetTDR();
    int item = jinfnum * 100 + tdrnum;

    double noise_threshold = 0;  // 20;
    double mixed_threshold = 30; // 180;
    if (tdrnum < NTDRS - 4 && cc->GetTotSig() > noise_threshold) {
      int side = cc->side;
      if (side == 0) { // ( index, ( X or Y, Z ) )
        if (find(_v_ladderS_to_ignore.begin(), _v_ladderS_to_ignore.end(), item) == _v_ladderS_to_ignore.end()) {
          v_hitsS.push_back(make_pair(ic, make_pair(cc->GetAlignedPositionMC(), GetAlignPar(jinfnum, tdrnum, 2))));
          // we double a cluster if it's likely that it's been generated by two merged hits
          if (cc->GetTotSig() > mixed_threshold && (ic == NClusTot - 1 || tdrnum != GetCluster(ic + 1)->GetTDR()) &&
              (ic == 0 || tdrnum != GetCluster(ic - 1)->GetTDR()))
            v_hitsS.push_back(v_hitsS.back());
        }
      } else {
        if (find(_v_ladderK_to_ignore.begin(), _v_ladderK_to_ignore.end(), item) == _v_ladderK_to_ignore.end()) {
          v_hitsK.push_back(make_pair(ic, make_pair(cc->GetAlignedPositionMC(), GetAlignPar(jinfnum, tdrnum, 2))));
          if (cc->GetTotSig() > mixed_threshold && (ic == NClusTot - 1 || tdrnum != GetCluster(ic + 1)->GetTDR()) &&
              (ic == 0 || tdrnum != GetCluster(ic - 1)->GetTDR()))
            v_hitsK.push_back(v_hitsK.back());
        }
      }
    }
  } // for loop on clusters
  printf("L'evento ha generato %lu(S) + %lu(K) = %d cluster\n", v_hitsS.size(), v_hitsK.size(), NClusTot);

  if (v_hitsS.size() < 5 || v_hitsK.size() < 5) {
    printf("non arriviamo ad almeno 5 per piano\n");
    return false;
  }

  // we double the first cluster if it is the only one in its ladder-> this should allow a better vertex reco
  int ppside = GetCluster(0)->side, vert = 0;
  if (!ppside) {
    if (v_hitsS[0].second.second != v_hitsS[1].second.second)
      vert = 1;
    // v_hitsS.insert(v_hitsS.begin(),v_hitsS[0]);
  } else {
    if (v_hitsK[0].second.second != v_hitsK[1].second.second)
      vert = 1;
    // v_hitsK.insert(v_hitsK.begin(),v_hitsK[0]);
  }

  /////////////////////////////////
  // PHASE 1.b: Vertex Hits Vector//
  /////////////////////////////////

  // we are going to copy in these vectors the hits recorded in the third 3 layers since the production layer
  std::vector<std::pair<int, std::pair<double, double>>> vert_v_hitsS;
  std::vector<std::pair<int, std::pair<double, double>>> vert_v_hitsK;

  if (vertmode) {
    std::vector<std::pair<int, std::pair<double, double>>>::iterator iter;
    iter = v_hitsS.begin();
    int first_layer = GetCluster((*iter).first)->GetTDR();

    while (GetCluster((*iter).first)->GetTDR() < first_layer + 6) {
      printf("nuova hit aggiunta al set S ristretto\n");
      vert_v_hitsS.push_back(*iter);
      iter++;
      if (iter == v_hitsS.end()) {
        break;
      }
    }
    printf("CAMBIO!!\n");
    iter = v_hitsK.begin();
    first_layer = GetCluster((*iter).first)->GetTDR();
    while (GetCluster((*iter).first)->GetTDR() < first_layer + 6) {
      printf("nuova hit aggiunta al set K ristretto\n");
      vert_v_hitsK.push_back(*iter);
      iter++;
      if (iter == v_hitsK.end()) {
        break;
      }
    }
    printf("vert_hitsS sono %lu\n", vert_v_hitsS.size());
    printf("vert_hitsK sono %lu\n", vert_v_hitsK.size());

    if (vert_v_hitsS.size() < 5 || vert_v_hitsK.size() < 5) {
      printf("non arriviamo ad almeno 5 per piano\n");
      return false;
    }
    printf("abbiamo riempito tutti i v_hits che ci serviranno e tutti contengono almeno 5 hits\n");
  }

  /////////////////////////////////////////////////////////
  // PHASE 2.a:Track Recontruction on the main Hits Vector//
  /////////////////////////////////////////////////////////

  // in these vectors the hits excluded from the tracks
  std::vector<std::pair<int, std::pair<double, double>>> rejectsS;
  std::vector<std::pair<int, std::pair<double, double>>> rejectsK;

  int n_tr_to_search = 2;
  for (int it = 0; it < n_tr_to_search; it++) {
    printf("alla ricerca della %d° traccia\n", it + 1);
    printf("S:\n");
    Track(v_hitsS, rejectsS);
    printf("tracciate->%lu, rigettate->%lu\n", v_hitsS.size(), rejectsS.size());
    if (v_hitsS.size() < 3) {
      printf("poche hit\n"); /*return false;*/
    } else {
      NS++;
      _TrS.push_back(make_track(v_hitsS)); // Here we store the tracks we have already found
    }
    printf("K:\n");
    Track(v_hitsK, rejectsK);
    printf("tracciate->%lu, rigettate->%lu\n", v_hitsK.size(), rejectsK.size());
    if (v_hitsK.size() < 3) {
      printf("poche hit\n"); /*return false;*/
    } else {
      NK++;
      _TrK.push_back(make_track(v_hitsK));
    }
    // rejected hits are now eligible for the next track to find
    if (vert && !ppside)
      rejectsS.insert(rejectsS.begin(), v_hitsS[0]);
    v_hitsS = rejectsS;
    if (vert && ppside)
      rejectsK.insert(rejectsK.begin(), v_hitsK[0]);
    v_hitsK = rejectsK;

    rejectsS.clear();
    rejectsK.clear();
  }
  _NTrks = std::min(NS, NK);
  // printf("S: %lu, %lu\n",_TrS[0].hits.size(),_TrS[1].hits.size());
  // printf("K: %lu, %lu\n",_TrK[0].hits.size(),_TrK[1].hits.size());
  // if ( _TrS[0].hits.size()<3 || _TrS[1].hits.size()<3 ) {printf("poche hit\n"); return false;}
  // if ( _TrK[0].hits.size()<3 || _TrK[1].hits.size()<3 ) {printf("poche hit\n"); return false;}

  ////////////////////////////////////////////////////////////////////////////////////////////
  // PHASE 2.b: Track Reconstruction on the Vertex Hits Vector and merge with the main tracks//
  ////////////////////////////////////////////////////////////////////////////////////////////
  //  bool vertmode=1;
  if (vertmode && _NTrks >= 2) {
    // j_joinS is the index of the track
    int j_joinS, j_joinK;
    for (int it = 0; it < n_tr_to_search; it++) {
      printf("VERT:alla ricerca della %d° traccia\n", it + 1);

      Track(vert_v_hitsS, rejectsS);
      if (vert_v_hitsS.size() < 3) {
        printf("poche hit\n");
        return false;
      }
      printf("S:%lu\n", vert_v_hitsS.size());

      Track(vert_v_hitsK, rejectsK);
      if (vert_v_hitsK.size() < 3) {
        printf("poche hit\n");
        return false;
      }
      printf("K:%lu\n", vert_v_hitsK.size());

      if (it == 0) {
        j_joinS = (TMath::Abs(vert_v_hitsS.back().second.first - _TrS[0].hits[2].second.first) <
                   TMath::Abs(vert_v_hitsS.back().second.first - _TrS[1].hits[2].second.first))
                      ? 0
                      : 1;
        //	printf("j_joinS = %d\n",j_joinS);
        j_joinK = (TMath::Abs(vert_v_hitsK.back().second.first - _TrK[0].hits[2].second.first) <
                   TMath::Abs(vert_v_hitsK.back().second.first - _TrK[1].hits[2].second.first))
                      ? 0
                      : 1;
        //	printf("j_joinK = %d\n",j_joinK);
      } else if (it == 1) {
        j_joinS = 1 - j_joinS;
        j_joinK = 1 - j_joinK;
      }
      printf("Individuato punto di raccordo ottimale\n Procediamo al %d° raccordo\n", it + 1);
      // qui vengono raccolti i vector di hit appartenenti a ciascuna traccia
      if (vert_v_hitsS.size() <= _TrS[j_joinS].hits.size())
        copy(vert_v_hitsS.begin(), vert_v_hitsS.end(), _TrS[j_joinS].hits.begin());
      printf("raccordo sulle hit S effettuato!\n");
      _TrS[j_joinS].update();
      printf("traccia aggiornata\n");

      if (vert_v_hitsK.size() <= _TrK[j_joinK].hits.size())
        copy(vert_v_hitsK.begin(), vert_v_hitsK.end(), _TrK[j_joinK].hits.begin());
      printf("raccordo sulle hit K effettuato!\n");
      _TrK[j_joinK].update();
      printf("traccia aggiornata\n");

      vert_v_hitsS = rejectsS;
      vert_v_hitsK = rejectsK;

      rejectsS.clear();
      rejectsK.clear();
    }
  }

  ////////////////////////////////
  // PHASE 3:Vertex Recontruction//
  ////////////////////////////////
  pair<double, double> vx, vy;
  //  printf("3\n");
  if (_NTrks == 1) {
    vx = make_pair(_TrK[0].hits[0].second.second, _TrK[0].hits[0].second.first);
    vy = make_pair(_TrS[0].hits[0].second.second, _TrS[0].hits[0].second.first);
    if (ppside)
      vy.second = vx.second;
    else
      vx.second = vy.second;
  } else if (std::min(NS, NK) == 2) {
    if (ppside) {
      vx = vertex(_TrK[0], _TrK[1]);
      if (_TrS[0].hits[0].first == _TrS[1].hits[0].first && _TrS[0].hits[1].first == _TrS[1].hits[1].first) {
        vy.first = tan(_TrS[0].prod_angle) * vx.second + _TrS[0].prod_dist / cos(_TrS[0].prod_angle);
      } else
        vy = vertex(_TrS[0], _TrS[1]);
      vy.second = vx.second;
    } else {
      vy = vertex(_TrS[0], _TrS[1]);
      if (_TrK[0].hits[0].first == _TrK[1].hits[0].first && _TrK[0].hits[1].first == _TrK[1].hits[1].first) {
        vx.first = tan(_TrK[0].prod_angle) * vy.second + _TrK[0].prod_dist / cos(_TrK[0].prod_angle);
      } else
        vx = vertex(_TrK[0], _TrK[1]);
      vx.second = vy.second;
    }
  }
  //  printf("4\n");

  if (vx == make_pair(9999., 9999.)) {
    cout << "Impossible to find a vertex on the ZX plane!!!\n";
    return false;
  }
  if (vy == make_pair(9999., 9999.)) {
    cout << "Impossible to find a vertex on the ZY plane!!!\n";
    return false;
  }
  _vertexK = vx;
  _vertexS = vy;
  return true;
}

void Event::Track(std::vector<Hit> &hits, std::vector<Hit> &rejects) {
  rejects.clear();
  std::vector<Hit> _hits;
  std::pair<double, double> dir;
  double pos;

  for (int itdr = NTDRS - 1; itdr >= 0; itdr--) {
    dir = Hough(hits);
    pos = alignpar[0][itdr][2]; // current z position for the study
    //    printf("iter n° %d -> pos = %f\ndir = (%f,%f)\n",itdr,pos,dir.first,dir.second);
    double xexp = dir.second / cos(dir.first) + tan(dir.first) * pos;
    // variables for the search of the minimum distance
    double min = 9999;
    int jmin = -1;
    int s = hits.size();
    for (int j = 0; j < s; j++) {
      if (abs(hits[j].second.second - pos) < 1.) { // layer belonging condition
        //	printf("pos=%f\n",pos);
        //	double xexp = dir.second/cos(dir.first) + tan(dir.first)*hits[j].second.second;
        double d = TMath::Abs(hits[j].second.first - xexp);

        if (d < min) { // among the coplanar hits we look for the nearest to the expected point
          if (jmin > -1)
            rejects.push_back(hits[jmin]);
          min = d;
          jmin = j;
        } else {
          rejects.push_back(hits[j]);
        }
      } else {
        // we keep all the other hits
        _hits.push_back(hits[j]);
      }
    } // ciclo for sugli elementi di hits

    if (jmin > -1) { // we still have to save the winner hit
      hits = {hits[jmin]};
      //      _hits.push_back(hits[jmin]);
    } else
      hits.clear();
    for (int nh = 0; nh < _hits.size(); nh++)
      hits.push_back(_hits[nh]);
    _hits.clear();
  }

  reverse(rejects.begin(), rejects.end());
  return;
}

pair<double, double> Event::Hough(vector<Hit> &vec) {
  int nHits = vec.size();
  HoughSpace h(.00001, .00001);
  double m, th, r;
  for (int ii = 0; ii < nHits; ii++) {
    for (int jj = ii + 1; jj < nHits; jj++) {
      if (vec[ii].second.second != vec[jj].second.second) {
        m = (vec[ii].second.first - vec[jj].second.first) / (vec[ii].second.second - vec[jj].second.second);
        th = atan(m);
        r = cos(th) * vec[ii].second.first - sin(th) * vec[ii].second.second;

        h.Add(th, r);
      }
    }
  }
  return h.GetMax();
}

std::vector<Hit> Event::CleanTrack(std::vector<Hit> &hits) {
  int i = 0, tdr = hits[0].first;
  int imax = 0, smax = 0;
  std::vector<std::vector<Hit>> coll;
  for (auto h : hits) {
    if (h.first - tdr < 4)
      coll[i].push_back(h);
    else {
      if (coll[i].size() > smax) {
        smax = coll[i].size();
        imax = i;
      }
      i++;
    }
    tdr = h.first;
  }
  return coll[imax];
}

void Event::RecombineXY(double ang) {
  vector<double> xhitrot, yhitrot;
  int tdrX = 18;
  int tdrY = 19;
  for (int cl = 0; cl < NClusTot; cl++) {
    if (GetCluster(cl)->GetTDR() == tdrY) // questi numeri hardcoded sono brutti
      yhitrot.push_back(GetCluster(cl)->GetAlignedPositionMC() + 0.07);
    if (GetCluster(cl)->GetTDR() == tdrX)
      xhitrot.push_back(GetCluster(cl)->GetAlignedPositionMC() + 0.07);
  }
  if (xhitrot.size() < 2 || yhitrot.size() < 2)
    return;

  // TGraph* gr=new TGraph();
  TRotation rot;
  rot = rot.RotateZ(ang * TMath::Pi() / 180.); // da controllare l'angolo
  double x[4], y[4];
  for (int i = 0; i < 4; i++) {
    TVector3 bo = rot * TVector3(xhitrot[i % 2], yhitrot[i < 2 ? (i % 2) : (1 - i % 2)], 0);
    x[i] = bo[0];
    y[i] = bo[1];
    //    printf("hit ruotata %d --> (%f, %f)\n",i,x[i],y[i]);
    //    gr->SetPoint(gr->GetN(),x[i],y[i]);
  }
  double a1 = (min(x[0], x[1]) + min(x[2], x[3])) / 2, // dovrebbero essere giusti
      a2 = (max(x[0], x[1]) + max(x[2], x[3])) / 2, b1 = (min(y[0], y[1]) + min(y[2], y[3])) / 2,
         b2 = (max(y[0], y[1]) + max(y[2], y[3])) / 2;

  double y1 = tan(_TrS[0].exit_angle) * alignpar[0][tdrY][2] + _TrS[0].exit_dist / cos(_TrS[0].exit_angle),
         y2 = tan(_TrS[1].exit_angle) * alignpar[0][tdrY][2] + _TrS[1].exit_dist / cos(_TrS[1].exit_angle),
         x1 = tan(_TrK[0].exit_angle) * alignpar[0][tdrX][2] + _TrK[0].exit_dist / cos(_TrK[0].exit_angle),
         x2 = tan(_TrK[1].exit_angle) * alignpar[0][tdrX][2] + _TrK[1].exit_dist / cos(_TrK[1].exit_angle);
  printf("x1=%f, x2=%f, y1=%f, y2=%f\n", x1, x2, y1, y2);
  // gr->SetPoint(gr->SetPoint(),x1,0);
  // gr->SetPoint(gr->SetPoint(),x2,0);
  // gr->SetPoint(gr->SetPoint(),0,y1);
  // gr->SetPoint(gr->SetPoint(),0,y2);
  int option1 = 0;
  if (x1 > a2 || x1 < a1)
    option1++; // ricontrolla tutto
  if (x2 > a2 || x2 < a1)
    option1++;
  if (y1 < b2 && y1 > b1)
    option1++;
  if (y2 < b2 && y2 > b1)
    option1++;
  pair<double, double> hit1, hit2;
  if (option1 < 2) { // this work only if the layer is tilted counterclockwise
    hit1 = make_pair(x[0], y[0]);
    hit2 = make_pair(x[1], y[1]);
  } else {
    hit1 = make_pair(x[2], y[2]);
    hit2 = make_pair(x[3], y[3]);
  }
  if (abs(x1 - hit1.first) > abs(x1 - hit2.first)) {
    std::reverse(_TrK.begin(), _TrK.end());
    // Track box=_TrK[0];
    // _TrK.erase(_TrK.begin());
    // _TrK.push_back(box);
  }
  if (abs(y1 - hit1.second) > abs(y1 - hit2.second)) {
    std::reverse(_TrS.begin(), _TrS.end());
  }
}

std::pair<double, double> Event::GetVertexS() {
  if (_vertexS == make_pair(9999., 9999.))
    FindTracksAndVertex();
  return _vertexS;
}
std::pair<double, double> Event::GetVertexK() {
  if (_vertexK == make_pair(9999., 9999.))
    FindTracksAndVertex();
  return _vertexK;
}

trackColl *Event::GetTracks(int t) {
  if (t)
    return &_TrK;
  else
    return &_TrS;
}

track::track() {
  prod_angle = 9999.;
  exit_angle = 9999.;
  prod_dist = 9999.;
  exit_dist = 9999.;
}

void track::update() {
  unsigned int ss = hits.size();
  if (ss > 1) {
    prod_angle = atan((hits[1].second.first - hits[0].second.first) / (hits[1].second.second - hits[0].second.second));
    exit_angle = atan((hits[ss - 1].second.first - hits[ss - 2].second.first) /
                      (hits[ss - 1].second.second - hits[ss - 2].second.second));

    prod_dist = cos(prod_angle) * hits[0].second.first - sin(prod_angle) * hits[0].second.second;
    exit_dist = cos(exit_angle) * hits[ss - 1].second.first - sin(exit_angle) * hits[ss - 1].second.second;
  }
  return;
}

track make_track(std::vector<std::pair<int, std::pair<double, double>>> &hits) {
  track tr;
  tr.hits = hits;
  tr.update();
  return tr;
}

std::pair<double, double> vertex(track &tr1, track &tr2) {
  if (tr1.hits.size() < 2 || tr2.hits.size() < 2)
    return make_pair(9999., 9999.); // NULL;

  std::pair<double, double> vert; //=new std::pair<double,double>;
  if (tr1.hits[0].first == tr2.hits[0].first && tr1.hits[1].first == tr2.hits[1].first) {
    vert.first = tr1.hits[0].second.first;
    vert.second = tr1.hits[0].second.second;
  } else {
    vert.second = (tr2.prod_dist * cos(tr1.prod_angle) - tr1.prod_dist * cos(tr2.prod_angle)) /
                  (cos(tr2.prod_angle) * sin(tr1.prod_angle) - cos(tr1.prod_angle) * sin(tr2.prod_angle)); // Z
    vert.first = (tr1.prod_dist + sin(tr1.prod_angle) * vert.second) / cos(tr1.prod_angle);                // X or Y
  }
  return vert;
}

HoughSpace::HoughSpace(double th, double r) {
  risth = th;
  risr = r;
  maxfreq = -1;
  maxpos = make_pair(9999., 9999.);
}

void HoughSpace::Add(double th, double r) {
  int f = 1;
  pair<double, double> vote = make_pair(floor(th / risth) * risth, floor(r / risr) * risr);
  if (votes.find(vote) == votes.end())
    votes.insert(make_pair(vote, 1));
  else
    f = ++votes[vote];
  if (f > maxfreq) {
    //    printf("nuovo max\n");
    maxfreq = f;
    maxpos = vote;
  }
}

//-------------------------------------------------------------------------------------

ClassImp(RHClass);

RHClass::RHClass() {

  JinfMap = new int[Event::NJINF];
  tdrMap = new laddernumtype[Event::NJINF * Event::NTDRS];

  Run = 0;
  ntdrRaw = 0;
  ntdrCmp = 0;
  nJinf = 0;
  sprintf(date, " ");
  memset(JinfMap, -1, Event::NJINF * sizeof(JinfMap[0]));
  memset(tdrMap, 0, Event::NTDRS * Event::NJINF * sizeof(tdrMap[0]));

  /* Viviana added (or left) these: do we really need?
  for (int ii=0;ii<NTDRS;ii++)
    for (int jj=0;jj<NVAS;jj++){
      CNMean[ii][jj]=0.;
      CNSigma[ii][jj]=0.;
    }
  }
  */

  return;
}

void RHClass::Print() {
  printf("---------------------------------------------\n");
  printf("The header says:\n");
  printf("Run: %d Date: %s\n", Run, date);
  printf("# Jinf = %d\n", nJinf);
  for (int ii = 0; ii < nJinf; ii++)
    printf("Jinf Map pos: %d Jinf num: %d\n", ii, JinfMap[ii]);

  printf("# TDR RAW = %d\n", ntdrRaw);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (tdrMap[ii].second == 0) {
      printf("TDR RAW:    Map pos: %d tdrnum: %d\n", ii, tdrMap[ii].first);
    }
  }

  printf("# TDR CMP = %d\n", ntdrCmp);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (tdrMap[ii].second == 1) {
      printf("TDR CMP:    Map pos: %d tdrnum: %d\n", ii, tdrMap[ii].first);
    }
  }
  printf("# TDR    = %d\n", ntdrCmp + ntdrRaw);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (!(tdrMap[ii].second == 0 || tdrMap[ii].second == 1)) {
      printf("TDR %2d???: Map pos: %d tdrnum: %d\n", tdrMap[ii].second, ii, tdrMap[ii].first);
    }
  }

  printf("---------------------------------------------\n");
  return;
}

int RHClass::FindPos(int tdrnum) {

  // Print();

  for (int ii = 0; ii < GetNTdrs(); ii++)
    if (tdrMap[ii].first == tdrnum)
      return ii;

  return -1;
}

void RHClass::SetJinfMap(int *_JinfMap) {

  // for (int ii=0;ii<NJINF;ii++) {
  //   JinfMap[ii]=_JinfMap[ii];
  // }
  memcpy(JinfMap, _JinfMap, Event::NJINF * sizeof(JinfMap[0]));

  return;
}

void RHClass::SetTdrMap(laddernumtype *_TdrMap) {

  // for (int ii=0;ii<NTDRS;ii++) {
  //   tdrMap[ii]=_TdrMap[ii];
  // }
  memcpy(tdrMap, _TdrMap, Event::NTDRS * sizeof(tdrMap[0]));

  return;
}

int RHClass::GetTdrNum(int tdrpos) {
  if (tdrpos < Event::NTDRS) {
    return tdrMap[tdrpos].first;
  }
  return -1;
}

int RHClass::GetTdrType(int tdrpos) {
  if (tdrpos < Event::NTDRS) {
    return tdrMap[tdrpos].second;
  }
  return -1;
}
