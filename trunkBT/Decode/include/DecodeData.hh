#ifndef DecodeData_h
#define DecodeData_h
#include <unistd.h>
#include <cstdio>
#include <vector>

#include "TBranch.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "EventUtils.hh"

template <size_t NCh> class calib {
public:
  float *ped;
  float *rsig;
  float *sig;
  int *status;

  calib() {
    ped = new float[NCh];  // was [1024]: is backward compatible?
    rsig = new float[NCh]; // was [1024]: is backward compatible?
    sig = new float[NCh];  // was [1024]: is backward compatible?
    status = new int[NCh]; // was [1024]: is backward compatible?
  };
};

#pragma pack(push, 1)

// typedef struct header { // gcc 4.3, considers 'typedef' useless // (what??)
struct header {  // for file writing NOT in AMSBlock
  int run;       // run number
  char date[50]; // date // why we put 50! In the RHClass is 30! Should be shorter, but anyhow it will be truncated when
                 // passed to RHClass
  double gonpar[4];       // goniometer parameters
  unsigned int refmaskjj; // 16/08/2014 - On Mac this is seen as long 8 (instead of 4) and the reader is read wrongly
  unsigned int refmask[24];
};

#pragma pack(pop)

#pragma pack(push, 1)

struct wholeheader {
  //---- Primary and secondary header ---------------------//
  unsigned short SIZE;
  unsigned short RRRWNODETYPE;
  unsigned short FBITAG;
  unsigned short TIMEMSB;
  unsigned short TIMELSB;
  //---- JMDC data block ----------------------------------//
  unsigned short JMDCSIZE;
  unsigned short JMDCRRRWNODETYPE;

  unsigned short RUNNUMMSB;
  unsigned short RUNNUMLSB;
  unsigned short RUNTAGMSB;
  unsigned short RUNTAGLSB;
  unsigned short EVTNUMMSB;
  unsigned short EVTNUMLSB;
  unsigned short JMDCTIMEMSB;
  unsigned short JMDCTIMELSB;
  unsigned short JMDCTIMEFINEMSB;
  unsigned short JMDCTIMEFINELSB;
  unsigned short GReservedGroups;
  //---- DSP Slave Format ----------------------------------//
  unsigned short DSPSIZE;
  unsigned short DSPRRRWNODETYPE;
};

#pragma pack(pop)

struct FlavorConfig;

class DecodeData {

protected:
  FILE *rawfile;

  int runn;
  bool pri = false;
  bool evpri = false;
  int ntdrRaw = 0;
  int ntdrCmp = 0;
  laddernumtype *tdrMap;
  int *tdrAlign; // added by Viviana, credo...
  double m_adcUnits = 8.0;
  unsigned int m_defaultShift = 640;
  unsigned int m_defaultArraySize = 384;

  int FindPos(int tdrnum);
  int FindCalPos(int tdrnum);
  int ReadFile(void *ptr, size_t size, size_t nitems, FILE *stream);

  bool kMC = false;

public:
  // TODO: put in DecodeDataAMS

  TH1F **hocc;
  TH1F **hoccseed;
  TH1F ***hcharge;
  TH1F ***hsignal;
  TH2F **hchargevsocc;
  TH2F **hsignalvsocc;
  TH1F ***hson;

  double shighthreshold;
  double slowthreshold;
  double khighthreshold;
  double klowthreshold;

  bool kClusterize;
  int cworkaround;

  virtual void DumpRunHeader() = 0;

public:
  DecodeData() = default;
  virtual ~DecodeData() = default;

  virtual ::FlavorConfig FlavorConfig() = 0;
  virtual TString EventClassname() = 0;

  // generic
  template <class Event, class calib>
  void AddCluster(Event *ev, calib *cals, int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus,
                  int CNStatus, int PowBits, int bad, float *sig, bool kRaw = false);
  template <class Event, class calib> void Clusterize(int numnum, int Jinfnum, Event *ev, calib *cal);
  template <class Event, class calib> void FillRawHistos(int numnum, int Jinfnum, Event *ev, calib *cal);

  virtual inline int GetNTdrRaw() { return ntdrRaw; }
  virtual inline int GetNTdrCmp() { return ntdrCmp; }
  virtual int GetTdrNum(size_t pos) = 0;
  virtual int GetTdrType(size_t pos) = 0;

  virtual int SkipOneEvent(int evskip = 1) = 0;
  virtual int ReadOneEvent() = 0;
  virtual void ClearEvent() = 0;

  virtual void OpenFile(const char *ifname, const char *caldir, int run, int ancillary) = 0;
  virtual void CloseFile();
  virtual int EndOfFile();

  // mc
  void SetPrintOff() { pri = 0; }
  void SetPrintOn() { pri = 1; }
  void SetEvPrintOff() { evpri = 0; }
  void SetEvPrintOn() { evpri = 1; }

  virtual TTree *GetMCTruth() { return nullptr; }; // CB
};

template <class Event, class calib>
inline void DecodeData::AddCluster(Event *ev, calib *cal, int numnum, int Jinfnum, int clusadd, int cluslen,
                                   int Sig2NoiStatus, int CNStatus, int PowBits, int bad, float *sig, bool kRaw) {

  //  constexpr auto NJINF = Event::GetNJINF();
  constexpr auto NTDRS = Event::GetNTDRS();
  //  constexpr auto NVAS = Event::GetNVAS();
  constexpr auto NVASS = Event::GetNVASS();
  //  constexpr auto NVASK = Event::GetNVASK();
  constexpr auto NCHAVA = Event::GetNCHAVA();

  pri = 1;

  LadderConf *ladderconf = LadderConf::Instance();
  int _bondingtype = 0;

  if (cworkaround != 0) {
    _bondingtype = cworkaround;
  } else {
    _bondingtype = ladderconf->GetBondingType(Jinfnum, numnum);
  }

  //  printf("bondingtype = %d\n", _bondingtype);

  int newclusadd = clusadd;
  
  if (!kRaw) { // otherwise the swap has been already done when clustering (if we were clustering, otherwise the cluster
               // is not present and we never reach this function...)
    if (_bondingtype == 2) {
      // if (clusadd>=(3*NCHAVA) && clusadd<(5*NCHAVA)) newclusadd+=3*NCHAVA;
      // if (clusadd>=(5*NCHAVA) && clusadd<(8*NCHAVA)) newclusadd-=2*NCHAVA;
      // questo cura in parte dei problemi sui bordi ancora da capire
      if (clusadd >= (3 * NCHAVA) && (clusadd + cluslen - 1) < (5 * NCHAVA))
        newclusadd += 3 * NCHAVA;
      if ((clusadd + cluslen - 1) >= (5 * NCHAVA) && clusadd < (8 * NCHAVA))
        newclusadd -= 2 * NCHAVA;
    }
  }

  int sid = 0;
  if (!kMC && clusadd >= (NCHAVA*NVASS))
    sid = 1;
  if (kMC)
    sid = !tdrAlign[numnum]; // check alignment. MD: why we need this in the MC case?

  if (ladderconf->GetSideSwap(Jinfnum, numnum)) {
    if (sid==0) sid=1;
    else if (sid==1) sid=0;
    else {
      printf("Side is %d so I don't know hot to swap...\n", sid);
    }
  }
  
  Cluster *pp = ev->AddCluster(Jinfnum, numnum + 100 * Jinfnum, sid);
  pp->SetLadderConf(ladderconf);

  //  pp->Build(numnum+100*Jinfnum,sid,clusadd,cluslen,sig,&(cal->sig[clusadd]),
  //	    &(cal->status[clusadd]), Sig2NoiStatus, CNStatus, PowBits, bad);
  // ONLY the 3rd field should be changed (clusadd->newclusadd) to move the cluster.
  // The 'clusadd' passed to the array should be left as it is to read the same signal values
  // for the 'sig' array there's no problem since already starting from 0
  pp->Build(numnum + 100 * Jinfnum, sid, newclusadd, cluslen, sig, &(cal->sig[clusadd]), &(cal->status[clusadd]),
            Sig2NoiStatus, CNStatus, PowBits, bad);

  double cog = pp->GetCoG();
  double seedadd = pp->GetSeedAdd();

  hocc[numnum + NTDRS * Jinfnum]->Fill(cog);
  hoccseed[numnum + NTDRS * Jinfnum]->Fill(seedadd);
  //#define TOTCHARGE
#ifndef TOTCHARGE
  hcharge[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetSeedCharge());
  hsignal[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetSeedVal());
  hchargevsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetSeedCharge());
  hsignalvsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetSeedVal());
  hson[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetSeedSN());
#else
  hcharge[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetCharge());
  hsignal[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetTotSig());
  hchargevsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetCharge());
  hsignalvsocc[numnum + NTDRS * Jinfnum]->Fill(cog, pp->GetTotSig());
  hson[numnum + NTDRS * Jinfnum][sid]->Fill(pp->GetTotSN());
#endif

  // if (pp->GetSeedSN()<3.5) {
  //     printf("%s", stringtodump.Data());
  //   for (int ii=0; ii<cluslen; ii++) {
  //     printf("AddCluster) %d %f\n", ii, sig[ii]);
  //   }
  //  printf("***** SoN: %f, Sig: %f, SeedAdd: %d side %d \n", pp->GetSeedSN(), pp->GetSeedVal(), pp->GetSeed(),
  //  pp->side);
  // pp->Print();
  //  sleep(3);
  // }
  // if (pp->GetSeedVal()<1.0) {
  //   printf("%s", stringtodump.Data());
  //   for (int ii=0; ii<cluslen; ii++) {
  //     printf("AddCluster) %d %f\n", ii, sig[ii]);
  //   }
  //   printf("***** SoN: %f, Sig: %f, SeedAdd: %d\n", pp->GetSeedSN(), pp->GetSeedVal(), pp->GetSeed());
  //   pp->Print();
  //   sleep(3);
  // }

  //  if (pri)
  //    pp->Print();

  return;
}

template <class Event, class calib> inline void DecodeData::FillRawHistos(int numnum, int Jinfnum, Event *ev, calib *cal) {
  //  constexpr auto NJINF = Event::GetNJINF();
  constexpr auto NTDRS = Event::GetNTDRS();
  constexpr auto NVAS = Event::GetNVAS();
  constexpr auto NVASS = Event::GetNVASS();
  //  constexpr auto NVASK = Event::GetNVASK();
  constexpr auto NCHAVA = Event::GetNCHAVA();

  pri = 1;

  int tdrnumraw = FindPos(numnum + 100 * Jinfnum);

  LadderConf *ladderconf = LadderConf::Instance();
  double shithresh = ladderconf->GetSHiThreshold(Jinfnum, numnum);
  double khithresh = ladderconf->GetKHiThreshold(Jinfnum, numnum);
  // double slothresh = ladderconf->GetSLoThreshold(Jinfnum, numnum);
  // double klothresh = ladderconf->GetKLoThreshold(Jinfnum, numnum);  
  
  // printf("Thresholds: %f %f\n", shithresh, khithresh);
  // sleep(3);
  
  for (int cc = 0; cc < (NCHAVA*NVAS); cc++) {
    
    double threshold = shithresh;
    int side=0;
    if (cc >= NCHAVA*NVASS) {
      threshold = khithresh;
      side=1;
    }
    
    if (ladderconf->GetSideSwap(Jinfnum, numnum)) {
      if (side==0) {
	side=1;
	threshold = khithresh;
      }
      else {
	side=0;
	threshold = shithresh;
      }
    }

    if (ev->RawSoN[Jinfnum][tdrnumraw][cc] > threshold) {
      //	    printf("%04d) %f %f %f -> %f\n", cc, ((double)ev->RawSignal[tdrnumraw][cc])/8.0, cal->ped[cc],
      // cal->sig[cc], (ev->RawSignal[tdrnumraw][cc]/8.0-cal->ped[cc])/cal->sig[cc]);
      // printf("%04d) %f\n", cc, ev->RawSoN[tdrnumraw][cc]);
      // this fills the histogram for the raw events when NOT clustering,
      // if kClusterize anyhow, ALL the histos as for the compressed data, will be filled
      hocc[numnum + NTDRS * Jinfnum]->Fill(cc, ev->RawSoN[Jinfnum][tdrnumraw][cc]);
      // hoccseed not filled in this case...
      // hcharge not filled in this case...
      hsignal[numnum + NTDRS * Jinfnum][side]->Fill(ev->RawSignal[Jinfnum][tdrnumraw][cc]/m_adcUnits);
      hson[numnum + NTDRS * Jinfnum][side]->Fill(ev->RawSoN[Jinfnum][tdrnumraw][cc]);
    }
  }
  
  return;  
}

template <class Event, class calib> inline void DecodeData::Clusterize(int numnum, int Jinfnum, Event *ev, calib *cal) {
  constexpr auto NJINF = Event::GetNJINF();
  constexpr auto NTDRS = Event::GetNTDRS();
  constexpr auto NVAS = Event::GetNVAS();
  constexpr auto NVASS = Event::GetNVASS();
  constexpr auto NVASK = Event::GetNVASK();
  constexpr auto NCHAVA = Event::GetNCHAVA();

  pri = 1;

  int _bondingtype = 0;
  bool defaultThresholds =
      (shighthreshold == 3.5 && khighthreshold == 3.5 && slowthreshold == 1.0 && klowthreshold == 1.0);

  LadderConf *ladderconf = LadderConf::Instance();

  static bool printed = false;
  if (!printed) {
    printf("\nClustering with:\n");
    printf("    %f %f for S-side\n", shighthreshold, slowthreshold);
    printf("    %f %f for K-side\n", khighthreshold, klowthreshold);
    if (!defaultThresholds) {
      printf("    thresholds used FOR ALL THE LADDERS\n");
    }

    if (defaultThresholds && ladderconf) {
      printf("Except for: \n");

      for (size_t jj = 0; jj < NJINF; jj++) {
        for (size_t tt = 0; tt < NTDRS; tt++) {
          double shithresh = ladderconf->GetSHiThreshold(jj, tt);
          double khithresh = ladderconf->GetKHiThreshold(jj, tt);
          double slothresh = ladderconf->GetSLoThreshold(jj, tt);
          double klothresh = ladderconf->GetKLoThreshold(jj, tt);

          if (shithresh != 3.5 || khithresh != 3.5 || slothresh != 1.0 || klothresh != 1.0) {
            printf("    JINF=%ld, TDR=%ld\n", jj, tt);
            printf("    %f %f for S-side\n", shithresh, slothresh);
            printf("    %f %f for K-side\n", khithresh, klothresh);
          }
        }
      }
    }

    if (cworkaround != 0) {
      printf("\n    %d as workaround FOR ALL THE LADDERS\n", cworkaround);
      _bondingtype = cworkaround;
    } else {
      printf("\n");
      for (size_t jj = 0; jj < NJINF; jj++) {
        for (size_t tt = 0; tt < NTDRS; tt++) {
          if (ladderconf->GetBondingType(jj, tt) != 0)
            printf("    %d as workaround for JINF=%ld, TDR=%ld\n", ladderconf->GetBondingType(jj, tt), jj, tt);
        }
      }
    }
    printed = true;
  }

  _bondingtype = ladderconf->GetBondingType(Jinfnum, numnum);

  int tdrnumraw = FindPos(numnum + 100 * Jinfnum);

  //// numnum -> mapped to find the ntdr(=nlayer)
  if (kMC)
    tdrnumraw = numnum;

  //// nvas were 16 total summing S and K
  //  int nvasS=10;
  //  int nvasK= 6;
  int nvasS = NVASS; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  int nvasK = NVASK; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  //// hardcoded nchannels per va was 64
  //  int nchavaS=64;
  //  int nchavaK=64;
  // defined in
  int nchavaS = NCHAVA; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  int nchavaK = NCHAVA; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  if (_bondingtype == 1) {
    nchavaS = 32;
  }

  int nvas = NVAS;     // changed by Viviana
  int nchava = NCHAVA; // changed by Viviana
  //// hardcoded nchannels per tdr was 1024
  short int array[nvas * nchava]; // changed by Viviana
  float arraySoN[nvas * nchava];  // changed by Viviana
  float pede[nvas * nchava];      // changed by Viviana
  float sigma[nvas * nchava];     // changed by Viviana
  int status[nvas * nchava];      // changed by Viviana
  int arraysize = nvas * nchava;  // changed by Viviana

  double highthreshold = 3.5;
  double lowthreshold = 1.0;

  int shift = 0;

  TString headerstringtodump = "New clusterize:\n";
  bool added = false; // added by Viviana

  // MD: there're still a couple of 640, 320, 384, etc... hardcoded
  // are inside particular "bondingtype"s so let's keep hardcoded...
  
  for (int side = 0; side < 2; side++) {
    if (added)
      continue;
    if (side == 0) {
      nvas = nvasS;
      nchava = nchavaS;
      if (defaultThresholds) {
        highthreshold = ladderconf->GetSHiThreshold(Jinfnum, numnum);
        lowthreshold = ladderconf->GetSLoThreshold(Jinfnum, numnum);
      } else {
        highthreshold = shighthreshold;
        lowthreshold = slowthreshold;
      }

      shift = 0;
      if (_bondingtype == 1) {
        arraysize = 320;
        for (int cc = 0; cc < arraysize; cc++) {
          array[cc] = ev->RawSignal[Jinfnum][tdrnumraw][cc * 2];
          arraySoN[cc] = ev->RawSoN[Jinfnum][tdrnumraw][cc * 2];
          pede[cc] = cal->ped[cc * 2];
          sigma[cc] = cal->sig[cc * 2];
          status[cc] = cal->status[cc * 2];
        }
      } else if (_bondingtype == 2) {
        arraysize = 384;
        int halfarraysize = ((int)(arraysize / 2));
        int shift = ((int)(640 / 2));
        for (int cc = 0; cc < halfarraysize; cc++) {
          //	  printf("%d %d %d\n", cc, cc+halfarraysize, cc+shift);
          array[cc] = ev->RawSignal[Jinfnum][tdrnumraw][cc];
          arraySoN[cc] = ev->RawSoN[Jinfnum][tdrnumraw][cc];
          pede[cc] = cal->ped[cc];
          sigma[cc] = cal->sig[cc];
          status[cc] = cal->status[cc];
          array[cc + halfarraysize] = ev->RawSignal[Jinfnum][tdrnumraw][cc + shift];
          arraySoN[cc + halfarraysize] = ev->RawSoN[Jinfnum][tdrnumraw][cc + shift];
          pede[cc + halfarraysize] = cal->ped[cc + shift];
          sigma[cc + halfarraysize] = cal->sig[cc + shift];
          status[cc + halfarraysize] = cal->status[cc + shift];
        }
      } else {
        // arraysize=640;
        //        arraysize = nvas * nchava; // changed by Viviana
        memcpy(array, ev->RawSignal[Jinfnum][tdrnumraw], arraysize * sizeof(ev->RawSignal[Jinfnum][tdrnumraw][0]));
        memcpy(arraySoN, ev->RawSoN[Jinfnum][tdrnumraw], arraysize * sizeof(ev->RawSoN[Jinfnum][tdrnumraw][0]));
        memcpy(pede, cal->ped, arraysize * sizeof(cal->ped[0]));
        memcpy(sigma, cal->sig, arraysize * sizeof(cal->sig[0]));
        memcpy(status, cal->status, arraysize * sizeof(cal->status[0]));
        if (kMC) { // added by Viviana
          added = true;
        }
      }
    } // side=0
    else { // side=1
      if (_bondingtype == 2) {
        continue;
      } else {
        nvas = nvasK;
        nchava = nchavaK;

        if (defaultThresholds) {
          highthreshold = ladderconf->GetKHiThreshold(Jinfnum, numnum);
          lowthreshold = ladderconf->GetKLoThreshold(Jinfnum, numnum);
        } else {
          highthreshold = khighthreshold;
          lowthreshold = klowthreshold;
        }

        shift = !kMC ? m_defaultShift : 0;                     // changed by Viviana
        arraysize = !kMC ? m_defaultArraySize : nvas * nchava; // changed by Viviana
        ////arraysize=!kMC?384:1024;//changed by Viviana
        // the src is the same array as in the S-side case but passing the reference to the first element of K-side
        // (640)

        memcpy(array, &(ev->RawSignal[Jinfnum][tdrnumraw][shift]),
               arraysize * sizeof(ev->RawSignal[Jinfnum][tdrnumraw][0]));
        memcpy(arraySoN, &(ev->RawSoN[Jinfnum][tdrnumraw][shift]),
               arraysize * sizeof(ev->RawSoN[Jinfnum][tdrnumraw][0]));
        memcpy(pede, &(cal->ped[shift]), arraysize * sizeof(cal->ped[0]));
        memcpy(sigma, &(cal->sig[shift]), arraysize * sizeof(cal->sig[0]));
        memcpy(status, &(cal->status[shift]), arraysize * sizeof(cal->sig[0]));
      }
    }

    double CN[nvas];

    for (int va = 0; va < nvas; va++) {
      CN[va] = Event::ComputeCN(nchava, &(array[va * nchava]), &(pede[va * nchava]), &(arraySoN[va * nchava]),
                                &(status[va * nchava]));
      //      printf("%d) %f\n", va, CN[va]);
      //      headerstringtodump += Form("CN[%d] = %f\n", va, CN[va]);
    }

    int bad = 0;
    int cluslen = 0;       // to fill
    int Sig2NoiStatus = 0; // boh
    int clusadd = 0;       // to fill
    int CNStatus = 0;      // boh
    int PowBits = 0;       // boh
    float sig[MAXLENGHT];

    bool firstfound = false;
    bool seedfound = false;

    float ssonmax = -999;
    int seedaddmax = -1;

    TString clusterstringtodump = "--> New cluster\n";
    // printf("--> New cluster\n");

    for (int count = 0; count < arraysize; count++) {

      if (status[count])
        continue;

      // printf("count = %d\n", count);
      //      clusterstringtodump += Form("count = %d\n", count);

      int va = (int)(count / nchava);

      float ssun = (array[count] / m_adcUnits - pede[count] - CN[va]) / sigma[count];
      // if (ssun>highthreshold) printf("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va],
      // sigma[count], ssun);

      // printf("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va], sigma[count], ssun);

      //      clusterstringtodump += Form("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va],
      //      sigma[count], ssun);
      TString stringtodump;
      if (ssun >= highthreshold) { // the seed that can also be the first of the cluster
        //	printf("%d) >high\n", count);
        //	clusterstringtodump += Form("%d) >high\n", count);
        //	clusterstringtodump += Form("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va],
        // sigma[count], ssun);

        if (ssun > ssonmax) {
          seedaddmax = count;
          ssonmax = ssun;
          //	  printf("%d) New max %d %f\n", count, seedaddmax, ssonmax);
          //	  clusterstringtodump += Form("%d) New max %d %f\n", count, seedaddmax, ssonmax);
        }

        if (firstfound) { // this is the seed (maybe there was another seed previously, but doesn't matter...)
          seedfound = true;
          cluslen++;
          //	  printf("%d) >high and first found already (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >high and first found already (cluslen=%d)\n", count, cluslen);
        } else { // is the seed but also the first of the cluster...
          firstfound = true;
          seedfound = true;
          clusadd = count;
          cluslen = 1;
          //	  printf("%d) >high, is the seed and also the first of the cluster (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >high, is the seed and also the first of the cluster (cluslen=%d)\n",
          // count, cluslen);
        }
      } else if (ssun >= lowthreshold) { // potentially the start of a cluster, or maybe another neighbour...
        //	clusterstringtodump += Form("%d) >low\n", count);
        if (!firstfound) { // is the first of the potential cluster
          firstfound = true;
          clusadd = count;
          cluslen = 1;
          //	  printf("%d) >low, is the first of the potential cluster (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >low, is the first of the potential cluster (cluslen=%d)\n", count,
          // cluslen);
        } else { // there was already a 'first' so this can be a neighbour between the first and the seed or a neighbour
                 // after
          cluslen++;
          //	  printf("%d) >low but firstfound already (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >low but firstfound already (cluslen=%d)\n", count, cluslen);
        }
      } else if (ssun < lowthreshold ||
                 count == (arraysize - 1)) { // end of a cluster or end of a "potential" cluster or simply nothing
        //	printf("%d) <low (cluslen=%d)\n", count, cluslen);
        //	clusterstringtodump += Form("%d) <low (cluslen=%d)\n", count, cluslen);
        if (seedfound) { // the cluster is done, let's save it!
          //	  printf("%d) <low, seed found already: let's save the cluster (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) <low, seed found already: let's save the cluster (cluslen=%d)\n",
          // count, cluslen);
          //          if (pri)
          //            printf("Cluster: add=%d  lenght=%d, seed=%d\n", clusadd + shift, cluslen, seedaddmax + shift);
          //	  clusterstringtodump += Form("Cluster: add=%d  lenght=%d, seed=%d\n", clusadd+shift, cluslen,
          // seedaddmax+shift);
          for (int hh = clusadd; hh < (clusadd + cluslen); hh++) {
            int _va = (int)(hh / nchava);
            float s = array[hh] / m_adcUnits - pede[hh] - CN[_va];
            //	    clusterstringtodump += Form("Sig=%f from Array = %d, Ped=%f, CN=%f\n", s, array[hh], pede[hh],
            // CN[_va]);
            //            if (pri)
            //              printf("Signal: %d, Pos:%d\n", (int)(m_adcUnits * s), hh + shift);
            //	    clusterstringtodump += Form("Signal: %d, Pos:%d\n", (int)(8*s), hh+shift);
            if ((hh - clusadd) < MAXLENGHT) {
              sig[hh - clusadd] = s;
              //              if (pri)
              //                printf("        %f, Pos: %d\n", sig[hh - clusadd], hh + shift);
              //	      clusterstringtodump += Form("        %f, Pos: %d\n", sig[hh-clusadd], hh+shift);
            } else
              bad = 1;
          }
          //	  clusterstringtodump += Form("Status[%d] = %d\n", seedaddmax+shift, status[seedaddmax]);
          stringtodump = headerstringtodump + clusterstringtodump;
          if (!(status[seedaddmax] & (1 << 3))) { // if is not a bad cluster
            //	    printf("numnum = %d\n", numnum);
            AddCluster(ev, cal, numnum, Jinfnum, clusadd + shift, cluslen, Sig2NoiStatus, CNStatus, PowBits, bad, sig);
          }
          clusterstringtodump = "--> New cluster\n";
        }
        // there was no seed found: "potential" cluster not promoted or even "nothing"
        seedfound = false;
        firstfound = false;
        memset(sig, 0, MAXLENGHT * sizeof(sig[0]));
        ssonmax = -9999;
        seedaddmax = -1;
        stringtodump = headerstringtodump + clusterstringtodump;
        clusterstringtodump = "--> New cluster\n";
      }
    }
  }

  return;
}

#endif
