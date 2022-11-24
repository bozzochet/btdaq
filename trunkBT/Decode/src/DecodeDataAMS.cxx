#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "DecodeDataAMS.hh"

#include "GenericEvent.hpp"

#include "TRandom3.h"
#include "TString.h"

using namespace std;

extern char progname[50];

static char errmess[16][80] = {"",
                               "",
                               "",
                               "",
                               "",
                               "replying node is CDP",
                               " RAW events",
                               " COMPRESSED events",
                               " cumulative status",
                               "build errors (sequencer errors for event building",
                               "build conditions (FE power fault for event building",
                               "NEXT bit / timeout (if DATA=1), set by master",
                               "ERROR bit / AMSW error (if DATA=1), set by master",
                               "ABORT bit / assembly error (if DATA=1), set by master",
                               "END bit / CRC error (if DATA=1), set by master",
                               "DATA bit, set by master when assembling group reply"};

inline bool file_exists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

static TString stringtodump;

namespace {
constexpr auto NJINF = DecodeDataAMS::EventAMS::GetNJINF();
constexpr auto NTDRS = DecodeDataAMS::EventAMS::GetNTDRS();
constexpr auto NVAS = DecodeDataAMS::EventAMS::GetNVAS();
constexpr auto NCHAVA = DecodeDataAMS::EventAMS::GetNCHAVA();
} // namespace

//=============================================================================================
DecodeDataAMS::DecodeDataAMS(char *ifname, char *caldir, int run, int ancillary, bool _kMC) : kMC{_kMC} {

  tdrMap = new laddernumtype[NJINF * NTDRS];
  tdrAlign = new int[NJINF * NTDRS];

  hocc = new TH1F *[NJINF * NTDRS];
  hoccseed = new TH1F *[NJINF * NTDRS];
  hchargevsocc = new TH2F *[NJINF * NTDRS];
  hsignalvsocc = new TH2F *[NJINF * NTDRS];
  hcharge = new TH1F **[NJINF * NTDRS];
  hsignal = new TH1F **[NJINF * NTDRS];
  hson = new TH1F **[NJINF * NTDRS];
  for (size_t ii = 0; ii < NJINF * NTDRS; ii++) {
    hcharge[ii] = new TH1F *[2];
    hsignal[ii] = new TH1F *[2];
    hson[ii] = new TH1F *[2];
  }

  // Initialize base-class members
  runn = run;
  ntdrRaw = 0;
  ntdrCmp = 0;
  memset(tdrMap, -1, NJINF * NTDRS * sizeof(tdrMap[0]));
  memset(tdrAlign, -1, NJINF * NTDRS * sizeof(tdrAlign[0])); // added by Viviana

  pri = 1;
  evpri = 0;

  OpenFile(ifname, caldir, run, ancillary);
  runn = run;

  shighthreshold = 3.5;
  slowthreshold = 1.0;
  khighthreshold = 3.5;
  klowthreshold = 1.0;

  kClusterize = false;
  cworkaround = 0;

  // Create the ROOT run header
  rh = new RHClassAMS();

  if (!kMC) {
    // Read the run Header
    DumpRunHeader();

    // Try to get some info on how many ladders there are in the data (RAW and CMP)
    long int pos = ftell(rawfile);
    if (pri)
      printf("*************** Reading one event to understand things...\n");
    ReadOneEvent();
    if (pri)
      printf("*************** ONE event read...\n");
    // come back to the previous pos
    if (int ret0 = fseek(rawfile, pos, SEEK_SET)) {
      printf("Fatal: error during file skip  %d\n", ret0);
      exit(3);
    }

    out_flag = true;
  }
  else {
    rh->SetRun(runn);
    time_t now = time(0); // MD: I don't like. Should be decided when simulating, not when analyzing!
    rh->SetDate(ctime(&now));
    nJinf = 0;
    ReadOneEvent();
    ntdrRaw = ntdrMC; // MD: seems ok. MC files are raw by default and we clusterize offline...
    out_flag = true;
  }

  // Create the ROOT Classes for the ROOT data format
  ev = new EventAMS("ladderconf.dat","gaincorrection.dat");

  mysort(tdrMap, ntdrRaw + ntdrCmp);
  // Update the ROOT run header
  rh->SetJinfMap(JinfMap);
  rh->SetNTdrsRaw(ntdrRaw);
  rh->SetNTdrsCmp(ntdrCmp);
  rh->SetTdrMap(tdrMap);

  if (pri) {
    printf("********* READVALS: %ld %d %d %d \n", NTDRS, nJinf, ntdrRaw, ntdrCmp);
    printf("Dumping the file headers that are going to be written in the ROOT files...\n");
  }
  rh->Print();

  // Find the Calibrations
  if (pri)
    printf("Finding a valid calibration...\n");
  FindCalibs();

  // added by Viviana
  int ntotdrs = ntdrRaw + ntdrCmp;
  //  for (int ii=0;ii<NTDRS;ii++) {
  for (int ii = 0; ii < ntotdrs; ii++) { // check if compatible with data struct
    calibAMS *calt = &(cals[ii]);
    printf("CALTEST: TDR %d -> %f %f %d\n", ii, calt->ped[0], calt->sig[0], calt->status[0]);
  }
  //

  char name[255];
  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t hh = 0; hh < NTDRS; hh++) {
      sprintf(name, "occ_%ld_%ld", jj, hh);
      //	  hocc[jj*NTDRS+hh]= new TH1F(name,name,1024,0,1024);
      hocc[jj * NTDRS + hh] = new TH1F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA);
      sprintf(name, "occseed_%ld_%ld", jj, hh);
      //	  hoccseed[jj*NTDRS+hh]= new TH1F(name,name,1024,0,1024);
      hoccseed[jj * NTDRS + hh] = new TH1F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA);

      sprintf(name, "qS_%ld_%ld", jj, hh);
      hcharge[jj * NTDRS + hh][0] = new TH1F(name, name, 1000, 0, 100);
      sprintf(name, "qK_%ld_%ld", jj, hh);
      hcharge[jj * NTDRS + hh][1] = new TH1F(name, name, 1000, 0, 100);

      sprintf(name, "signalS_%ld_%ld", jj, hh);
      hsignal[jj * NTDRS + hh][0] = new TH1F(name, name, 4200, -100, 4100);
      sprintf(name, "signalK_%ld_%ld", jj, hh);
      hsignal[jj * NTDRS + hh][1] = new TH1F(name, name, 4200, -100, 4100);

      sprintf(name, "q_vs_occ_%ld_%ld", jj, hh);
      hchargevsocc[jj * NTDRS + hh] = new TH2F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA, 1000, 0, 100);

      sprintf(name, "signal_vs_occ_%ld_%ld", jj, hh);
      hsignalvsocc[jj * NTDRS + hh] = new TH2F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 4100);

      sprintf(name, "sonS_%ld_%ld", jj, hh);
      hson[jj * NTDRS + hh][0] = new TH1F(name, name, 1000, 0, 100);
      sprintf(name, "sonK_%ld_%ld", jj, hh);
      hson[jj * NTDRS + hh][1] = new TH1F(name, name, 1000, 0, 100);
    }
  }
}
//=============================================================================================

DecodeDataAMS::~DecodeDataAMS() {

  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t hh = 0; hh < NTDRS; hh++) {
      //      printf("%d %d --> %f\n", jj, hh, hocc[jj*NTDRS+hh]->GetEntries());
      if (hocc[jj * NTDRS + hh] && hocc[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		printf("deleting hocc %d %d at %p\n", jj, hh, hocc[jj*NTDRS+hh]);
        delete hocc[jj * NTDRS + hh];
      }
      if (hoccseed[jj * NTDRS + hh] && hoccseed[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		printf("deleting hoccseed %d %d\n", jj, hh);
        delete hoccseed[jj * NTDRS + hh];
      }
      if (hchargevsocc[jj * NTDRS + hh] && hchargevsocc[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		  printf("deleting hchargevsocc %d %d\n", jj, hh);
        delete hchargevsocc[jj * NTDRS + hh];
      }
      if (hsignalvsocc[jj * NTDRS + hh] && hsignalvsocc[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		  printf("deleting hsignalvsocc %d %d\n", jj, hh);
        delete hsignalvsocc[jj * NTDRS + hh];
      }

      for (int ss = 0; ss < 2; ss++) {
        //	printf("%d %d %d --> %f\n", jj, hh, ss, hcharge[jj*NTDRS+hh][ss]->GetEntries());
        if (hcharge[jj * NTDRS + hh][ss] && hcharge[jj * NTDRS + hh][ss]->GetEntries() < 1.0) {
          //	  	  printf("deleting hcharge %d %d %d\n", jj, hh, ss);
          delete hcharge[jj * NTDRS + hh][ss];
        }
        if (hsignal[jj * NTDRS + hh][ss] && hsignal[jj * NTDRS + hh][ss]->GetEntries() < 1.0) {
          //	  	  printf("deleting hsignal %d %d %d\n", jj, hh, ss);
          delete hsignal[jj * NTDRS + hh][ss];
        }
        if (hson[jj * NTDRS + hh][ss] && hson[jj * NTDRS + hh][ss]->GetEntries() < 1.0) {
          //	  	  printf("deleting hson %d %d %d\n", jj, hh, ss);
          delete hson[jj * NTDRS + hh][ss];
        }
      }
    }
  }

  if (pri)
    cout << "In the destructor..." << endl;
  if (rawfile)
    CloseFile();
  if (ev)
    delete ev;
  if (pri)
    cout << "Destroyed." << endl;

  if (kMC) {
    delete mcrt;
    delete mcht;
    delete mcf;
  }

  return;
}

//=============================================================================================

int DecodeDataAMS::FindPosMC(int hitnum) {

  // dummy map function -> to implement for realistic MC
  return hvol[hitnum]; // does not work on old files
  // MD: see also if is really needed to have a separe FindPos for MC
  // return hitnum;
}
//=============================================================================================

void DecodeDataAMS::OpenFile(const char *ifname, const char *caldir, int run, int ancillary) {

  if (kMC) {
    //    OpenFile_mc(ifname, caldir, run, ancillary);
    OpenFile_mc(ifname);
  } else {
    OpenFile_data(ifname, caldir, run, ancillary);
  }

  sprintf(rawdir, "%s", ifname);
  sprintf(rawCaldir, "%s", caldir);

  rawfile = fopen(rawname, "r");
  if (rawfile == NULL) {
    printf("Error file %s not found \n", rawname);
    exit(2);
  }

  return;
}

void DecodeDataAMS::OpenFile_data(const char *ifname, const char *caldir, int run, int ancillary) {

  if (ancillary >= 0) {
    sprintf(rawname, "%s/%06d_ANC_%d.dat", ifname, runn, ancillary);
  } else {
    sprintf(rawname, "%s/%06d.dat", ifname, runn);
    if (!file_exists(rawname)) {
      for (int ii = 0; ii < 10000; ii++) {
        sprintf(rawname, "%s/%06d_ANC_%d.dat", ifname, runn, ii);
        if (file_exists(rawname))
          break;
      }
    }
  }

  return;
}

// void DecodeDataAMS::OpenFile_mc(char* ifname, char* caldir, int run, int ancillary){
void DecodeDataAMS::OpenFile_mc(const char *ifname) {
  sprintf(rawname, "%sanaOut.root", ifname);

  mcf = new TFile(rawname);
  mcrt = (TTree *)mcf->Get("runTree");
  mcrt->SetBranchAddress("nEvts", &gevt);
  mcrt->SetBranchAddress("gEne", &gene);
  mcrt->SetBranchAddress("nLayers", &nlayers);
  mcrt->SetBranchAddress("xyAlign", &tdrAlign);
  mcrt->GetEntry(0);
  printf("OPEN MC FILE %s -> SIM Events %d energy: %f MeV, LAYERS %d \n", rawname, gevt, gene, nlayers);

  mcht = (TTree *)mcf->Get("hitTree");
  int nent = mcht->GetEntries();
  printf("-> PProd Entries: %d\n", nent);

  /// moved here
  mcht->SetBranchAddress("nHits", &nhits);
  mcht->SetBranchAddress("nTotalHits", &ntothits);
  mcht->SetBranchAddress("hVol", &hvol);
  mcht->SetBranchAddress("hVolZ", &hvolz);
  mcht->SetBranchAddress("xCoord", &xcoord);
  mcht->SetBranchAddress("yCoord", &ycoord);
  mcht->SetBranchAddress("zCoord", &zcoord);
  mcht->SetBranchAddress("ppHit", &pphit);

  mcht->SetBranchAddress("eDep", &eDep);
  mcht->SetBranchAddress("eEne", &eEne);

  mcht->SetBranchAddress("PDG", &pdg);
  mcht->SetBranchAddress("ParID", &parid);
  mcht->SetBranchAddress("TrID", &trid);

  /// new output from GGS
  mcht->SetBranchAddress("chXY", &chXY);
  mcht->SetBranchAddress("hitStrips", &hitStrips);
  mcht->SetBranchAddress("simStrips", &simStrips);
  mcht->SetBranchAddress("hitChan", &hitChan);
  mcht->SetBranchAddress("hitDep", &hitDep);
  mcht->SetBranchAddress("simChan", &simChan);
  mcht->SetBranchAddress("simDep", &simDep);

  return;
}

void DecodeDataAMS::CloseFile() {

  if (rawfile)
    fclose(rawfile);
  rawfile = NULL;

  return;
}

int DecodeDataAMS::EndOfFile() {

  int eff = 0;
  if (kMC) {
    eff = EndOfFile_mc();
  } else {
    eff = EndOfFile_data();
  }
  return eff;
}

int DecodeDataAMS::EndOfFile_data() {

  int eff = 0;
  unsigned short int dummy;
  (void)dummy;

  if (rawfile) {
    dummy = 0; // without the declaration of this variable it's not working (not finding the end of the file!!!!!)
    eff = feof(rawfile);
    //    if (eff) printf("EndOfFile: end of file !\n" );
    //    if (pri)
    //      printf("End of file? -> %d\n", eff);
  } else {
    printf("Error file pointer is NULL \n");
    exit(3);
  }

  return eff;
}

int DecodeDataAMS::EndOfFile_mc() {

  int eff = 0;

  if (evenum == mcht->GetEntries())
    eff = -1;

  return eff;
}

//=============================================================================================

void DecodeDataAMS::DumpRunHeader() {
  unsigned short int size;
  header hh;

  // Read The Header Size
  ReadFile(&size, sizeof(size), 1, rawfile);

  size *= 2; // this is the size stored but for some reason we decided (in TakeData) to store in units of word, so
             // divided by 2.0

  if (pri)
    printf("Headersize: %d\n", size);
  /* check the header size */
  if (size != sizeof(hh))
    printf("!!!!!!!!!!!!!!!!!!! WRONG: Headersize = %hu (but sizeof(header) = %zu)\n", size, sizeof(hh));
  // ReadFile(&hh, sizeof(header), 1, rawfile);//this should be fine also
  ReadFile(&hh, size, 1, rawfile);

  rh->SetRun(hh.run);
  rh->SetDate(hh.date);

  printf("Run: %d   Date: %s\n", hh.run, hh.date);
  if (pri) {
    for (int ii = 0; ii < 4; ii++)
      printf("Angle (%d) = %f\n", ii, hh.gonpar[ii]);
    printf("RefmaskJJ: 0x%x\n", hh.refmaskjj);
    for (int ii = 0; ii < 24; ii++)
      printf("Refmask (%d) = 0x%x\n", ii, hh.refmask[ii]);
  }
}

//=============================================================================================

int DecodeDataAMS::SkipOneEvent(int evskip) {

  int ret;

  if (kMC) {
    ret = SkipOneEvent_mc(evskip);
  } else {
    ret = SkipOneEvent_data(evskip);
  }

  return ret;
}

int DecodeDataAMS::SkipOneEvent_data(int evskip) {

  unsigned short int size;
  unsigned short int junk;

  printf(" Run %d SKIPPING %d events on DAQ %s\n", runn, evskip, type);
  for (int kk = 0; kk < evskip; kk++) {
    // Read The Event Size
    if (pri)
      printf("pos:%ld  ", ftell(rawfile) / 2);
    ReadFile(&size, sizeof(size), 1, rawfile);

    if (pri)
      printf("%s size    %d \n", type, size);
    for (int ii = 0; ii < size; ii++) {
      ReadFile(&junk, sizeof(junk), 1, rawfile);
      if (pri)
        printf("%d:  %d\n", ii, junk);
      // printf("pos:%ld  ",ftell(rawfile)/2);
    }
  }

  return 0;
}

int DecodeDataAMS::SkipOneEvent_mc(int evskip) {

  // TO BE IMPLEMENTED
  //  MD: most-likely will never happen

  return 0;
}

//=============================================================================================

int DecodeDataAMS::ReadOneEvent() {

  int ret;

  if (kMC) {
    ret = ReadOneEvent_mc();
  } else {
    ret = ReadOneEvent_data();
  }

  return ret;
}

int DecodeDataAMS::ReadOneEvent_data() {

  // MD: Viviana moved here: I think is wrong
  // ev = new Event();

  unsigned short int size;
  unsigned int tdrnoeventmask; // two words before status
  unsigned short int status;
  unsigned short int num;

  int fstat = 0;

  if (!out_flag) {
    ntdrRaw = 0;
    ntdrCmp = 0;
    nJinf = 0;
  }

  // Read The Event Size
  if (pri)
    printf("ReadOneEvent) pos:%ld\n ", ftell(rawfile) / 2);

  fstat = ReadFile(&size, sizeof(size), 1, rawfile);
  if (fstat == -1)
    return 1;

  long int base = ftell(rawfile) / 2;

  // update the event counters
  evenum++;

  if (out_flag)
    ev->Evtnum = evenum;

  if (pri)
    printf("ReadOneEvent-AfterSizeRead) pos:%ld\n", ftell(rawfile) / 2);

  // Read the Event Number
  fstat = ReadFile(&num, sizeof(num), 1, rawfile);
  if (fstat == -1)
    return 1;

  if (pri)
    printf("Evenum: %d\n", num);
  if (out_flag)
    ev->Evtnum = num;
  if (pri || evpri)
    printf("\n-----------------------------> New event: %d\n", num);

  if (pri || evpri)
    printf("%s size: %d\n", type, size);

  // Store the position on the file
  long int pos = ftell(rawfile);

  // skip all the TDR data
  if (int ret0 = fseek(rawfile, (size - 2) * 2, SEEK_CUR)) {
    printf("Fatal: error during file skip ret0=%d \n", ret0);
    exit(3);
  }

  // read the last word of the event
  if (pri)
    printf("ReadOneEvent-LastWordOfEvent) pos:%ld\n", ftell(rawfile) / 2);
  fstat = ReadFile(&status, sizeof(status), 1, rawfile);
  if (fstat == -1)
    return 1;

  if (pri || evpri)
    printf("%s Status %d: address=%02d  status=", type, status, status & 0x1f);

  int count = 5;
  for (int dase = 32; dase < 0x8000; dase *= 2)
    if (pri || evpri)
      printf(" %2d: %d |", count++, ((status & dase) > 0));
  if (pri || evpri)
    printf("\n");

  count = 5;
  for (int dase = 32; dase < 0x8000; dase *= 2) {
    if ((status & dase) > 0)
      if (pri || evpri)
        printf(" %02d: %s\n", count, errmess[count]);
    count++;
  }
  if (pri || evpri)
    printf("\n\n");

  if (out_flag)
    ev->JINJStatus = status;

  if ((status & 0x400) > 0) {
    // if((status&0x200)>0||(status&0x400)>0){
    printf("TEST\n");
    printf("=======================> %s Error!!! Status %hx\n", type, status);
    if (out_flag)
      printf("=======================> %s Error!!!  Skip this event: %d\n", type, ev->Evtnum);
    printf("TEST\n");
    count = 5;
    for (int dase = 32; dase < 0x8000; dase *= 2) {
      if ((status & dase) > 0)
        printf(" %02d: %s\n", count, errmess[count]);
      count++;
    }
    return 10;
  }

  // come back to the previous pos (i.e. to the first sub-block)
  if (int ret0 = fseek(rawfile, pos, SEEK_SET)) {
    printf("Fatal: error during file skip %d\n", ret0);
    exit(3);
  }

  // Read the TDRs or the JINFs
  if (!out_flag) {
    ntdrRaw = 0;
    ntdrCmp = 0;
  }

  if (strcmp(type, "JinJ") == 0) {
    while (ftell(rawfile) / 2 < base + size - 1) {
      ReadOneJINF();
    }
  } else {
    while (ftell(rawfile) / 2 < base + size - 3) {
      if (pri)
        printf("ReadOneTDR from file position: %ld\n", ftell(rawfile) / 2);
      ReadOneTDR(0);
    }

    // Read the last 2 words before status
    unsigned short int dummy;
    fstat = ReadFile(&dummy, sizeof(dummy), 1, rawfile);
    tdrnoeventmask = dummy << 16;
    dummy = 0;
    fstat = ReadFile(&dummy, sizeof(dummy), 1, rawfile);
    tdrnoeventmask += dummy;
    
    if (fstat == -1)
      return 1;
    if (pri || evpri)
      printf("Tdrs with no event Mask: %d\n", tdrnoeventmask);
    for (size_t ii = 0; ii < NTDRS; ii++) {
      if (tdrnoeventmask & (1 << ii)) {
        if (pri || evpri)
          printf("A tdr (%02ld) replied with no event...\n", ii);
        if (!out_flag) {
          //	  printf("tdrMap[%d].first = %d\n", ntdrRaw+ntdrCmp, ii+100*0);
          tdrMap[ntdrRaw + ntdrCmp].first =
              ii + 100 * 0; // In ReadOneJinf is 100*(status&0x1f) but here is in the case with just one Jinf...
          tdrMap[ntdrRaw + ntdrCmp].second = 1;
          ntdrCmp++;
        }
      } else if (pri || evpri) {
        int tdrnum = FindPos(ii);
        //	  printf("%d\n", tdrnum);
        if (tdrnum >= 0)
          printf("A tdr (%02ld) replied...\n", ii);
        else
          printf("A tdr (%02ld) didn't replied...\n", ii);
      }
    }
  }

  // Reread the last word
  fstat = ReadFile(&status, sizeof(status), 1, rawfile);
  if (fstat == -1)
    return 1;

  if (pri || evpri)
    printf("%s status: %d\n", type, status);

  return 0;
}

int DecodeDataAMS::ReadOneEvent_mc() {

  int jinfnum = 0; // MD: assuming only one Jinf

  // if (evenum==5 || evenum==21 || evenum==26) {evenum++; return 0;}
  TRandom3 rn;
  double dEdX2ADC = 3.5e3;

  hitChan->clear();
  hitDep->clear();
  simChan->clear();
  simDep->clear();

  mcht->GetEntry(evenum);

  std::cout << "ReadOneEventMC entry:" << evenum << " hits " << ntothits << " pphit " << pphit << " datasize "
            << simDep->size() << std::endl;

  ntdrMC = nlayers;

  if (!out_flag) {

    for (int ij = 0; ij < nJinf; ij++) {
      JinfMap[ij] = 0; // MD: all Jinf put at 0, assuming only one jinf?
    }
    for (int ir = 0; ir < nlayers; ir++) {
      //      tdrRaw[ir]=ir;//Viviana
      tdrMap[ir].first = ir; // MD: to check
      tdrMap[ir].second = 0; // MD: RAW, and to check
      printf("ALIGNMENT %d %d\n", ir, tdrAlign[ir]);
    }
    /*
    // Viviana, but not needed anymore
    for (int ic=0;ic<ntdrCmp;ic++){
      tdrCmp[ic]=0;
    }
    */

    // printf("END FIRST ReadOneEventMC entry:%d %d\n",evenum,nhits);

  } else {

    //    //update the event counters ...here?
    // evenum++;

    ev->Evtnum = evenum;
    ev->JINJStatus = 999;

    ev->JINFStatus[jinfnum] = 999;

    // printf("ReadOneEventMC store to event: %d %d %d %f %d %f %d
    // \n",evenum,nlayers,pphit,eEne[0],chXY[0],eDep[0],int(eDep[0]*dEdX2ADC/8.));

    std::cout << "ReadOneEventMC store to event:" << evenum << " layers " << nlayers << " hits " << ntothits
              << " pphit " << pphit << " datasize " << simDep->size() << " first hit: " << pdg[0] << " " << parid[0]
              << " " << eEne[0] << " " << eDep[0] << " alXY(0,1) " << tdrAlign[0] << " CoG " << chXY[0] << std::endl;

    int hitclcount = 0;
    int nhi = 0;

    // for(int nh=0;nh<ntothits;nh++){ // ntothits>nlayers
    for (int nl = 0; nl < ntdrMC; nl++) { // nlayers
      // hvol[nh]=nh;      // now read
      calibAMS *cal = &(cals[nl]);
      ev->TDRStatus[jinfnum][nl] = 999;
      ev->ReadTDR[jinfnum][nl] = 1;
      // calib* cal=&(cals[hvol[nh]]);
      // ev->TDRStatus[hvol[nh]]=999;
      // ev->ReadTDR[hvol[nh]]=1;

      printf("ReadOneEventMC LAYER %d align: %d\n", nl, tdrAlign[nl]);

      /// hardcoded number of channels
      for (size_t kk = 0; kk < NVAS * NCHAVA; kk++) {
        // for (int kk=0;kk<1024;kk++){
        ev->CalPed[jinfnum][nl][kk] = cal->ped[kk];
        ev->CalSigma[jinfnum][nl][kk] = cal->sig[kk];
        ev->RawSignal[jinfnum][nl][kk] = int(rn.Gaus(420., 1.125)); // simnoise --> OK?
        ev->RawSoN[jinfnum][nl][kk] = (ev->RawSignal[jinfnum][nl][kk] - cal->ped[kk]) / cal->sig[kk];
      }

      for (int nh = 0; nh < ntothits; nh++) { // ntothits>nlayers
        if (hvol[nh] != nl)
          continue;
        if (pri)
          printf("LAYER %d HIT %d on CH %d: hitS %d simS %d DEP %f MeV conv %d \n", hvol[nh], nh, chXY[nh],
                 hitStrips[nh], simStrips[nh], eDep[nh], int(eDep[nh] * dEdX2ADC / 8.));

        // for (int kk=0;kk<4096;kk++){
        // for (int kk=0;kk<1024;kk++){

        // for (int si=0;si<hitStrips[nh];si++){
        for (int si = 0; si < simStrips[nh]; si++) {
          // check the shift of -225 chs for 50x50 as done previously
          cout << "*****************" << si << " " << hvol[nh] << " " << nhi << " " << simChan->at(nhi) << " "
               << simDep->at(nhi) << endl;
          int sch = simChan->at(nhi) - (tdrAlign[hvol[nh]] ? 224 : 177);
          // int sch=hitChan->at(nhi)-112;
          // int sch=simChan->at(nhi)-112;
          // ev->RawSignal[hvol[nh]][sch]+=int(hitDep->at(nhi)*dEdX2ADC);
          ev->RawSignal[jinfnum][hvol[nh]][sch] += int(simDep->at(nhi) * dEdX2ADC);
          ev->RawSoN[jinfnum][hvol[nh]][sch] =
              (ev->RawSignal[jinfnum][hvol[nh]][sch] - cal->ped[sch]) / cal->sig[sch];
          if (pri)
            printf("HITSIG %d: %d %f %f %f\n", sch, ev->RawSignal[jinfnum][hvol[nh]][sch],
                   ev->RawSoN[jinfnum][hvol[nh]][sch], cal->ped[sch], cal->sig[sch]);
          nhi++;
        }

        // shift to get 4096 strips
        /* int chh=!tdrAlign[hvol[nh]]?echx[nh]-225:echy[nh]-225; // X or Y according to alignment
        //	if(kk<640){	  //// was for getting 0 on the k side
        if (kk==chh){
          ev->RawSignal[hvol[nh]][kk]=int(eDep[nh]*dEdX2ADC);//
          printf("ReadOneEventMC HITSIG %d: %d %f %f \n",kk,ev->RawSignal[hvol[nh]][kk],cal->ped[kk],cal->sig[kk]);
        }

        ev->RawSoN[nl][kk]=(ev->RawSignal[nl][kk]/8.0-cal->ped[kk])/cal->sig[kk];
        */

        // printf("ReadOneEventMC SIGS %d: %d %f %f -> %f
        // \n",kk,ev->RawSignal[nh][kk],cal->ped[kk],cal->sig[kk],ev->RawSoN[nh][kk]);
        /*}else{
          ev->RawSignal[nh][kk]=0.;//
          ev->RawSoN[nh][kk]=0.;
          }*/

        //}

        hitclcount++;

      } // new loop on hits

      //// numnum is hvol[nhit] -> must be mapped to find the ntdr(=nlayer)
      // int mtdrn=FindPosMC(nl);
      //      printf("ReadOneEventMC clusterize hits %d on vol %d of layer(tdr) %d, hcl:%d \n",nh, hvol[nh], mtdrn,
      //      hitclcount);

      printf("ReadOneEventMC clusterize hits of layer %d , hcl:%d \n", nl, hitclcount);
      // Clusterize(mtdrn, 0, cal);
      Clusterize(nl, 0, ev, cal);
    } // ntdrMC = nlayers

    // update the event counters ...moved here
    evenum++;

  } // out_flag ==1

  return 0;
}

//=============================================================================================

int DecodeDataAMS::ReadOneTDR(int Jinfnum) {
  unsigned short int size;

  // read the size of the TDR events
  if (pri)
    printf("pos:%ld\n", ftell(rawfile) / 2);
  ReadFile(&size, sizeof(size), 1, rawfile);

  if (pri)
    printf("TDRsize: %d \n", size);

  // allocate memory for the TDR event and read the event
  short int *array = (short int *)calloc(size, sizeof(short int));
  if (!array) {
    printf("Fatal error cannot alloc memeory.\n");
    exit(3);
  }

  ReadFile(array, size * sizeof(short int), 1, rawfile);

  // Decode the tdr number (the order of tdrs in the file may change event by event!!!)
  int tdrnum = -1;
  int numnum = ((unsigned int)array[size - 1]) & 0x1f;
  //  printf("JINF=%d, NUMNUM=%d\n", Jinfnum, numnum);
  if (out_flag) {
    tdrnum = FindPos(numnum + 100 * Jinfnum);
    //    printf("JINF=%d TDR=%d -> POS=%d\n", Jinfnum, numnum, tdrnum);
    if (tdrnum < 0) {
      printf("WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("JINF=%d TDR=%d\n", Jinfnum, numnum);
      printf("DecodeDataAMS::ReadOneTDR::Cannot-Find-TDR-in-CMP-or-RAW\n");
      //      exit(4);
      return -1;
    }
  }

  if (pri)
    printf("==========> TDR %02d, Jinf %02d, Size=%4d, TDR Status=%04hx: ", numnum, Jinfnum, size, array[size - 1]);
  if (!out_flag) {
    if ((array[size - 1] & 64) > 0) {
      //      printf("RAW) numnum=%d, Jinfnum=%d, ntdrRaw=%d, ntdrCmp=%d\n", numnum, Jinfnum, ntdrRaw, ntdrCmp);
      tdrMap[ntdrCmp + ntdrRaw].first = numnum + 100 * Jinfnum;
      tdrMap[ntdrCmp + ntdrRaw].second = 0;
      ntdrRaw++;
    }
    if ((array[size - 1] & 128) > 0) {
      //      printf("CMP) numnum=%d, Jinfnum=%d, ntdrRaw=%d, ntdrCmp=%d\n", numnum, Jinfnum, ntdrRaw, ntdrCmp);
      tdrMap[ntdrRaw + ntdrCmp].first = numnum + 100 * Jinfnum;
      tdrMap[ntdrRaw + ntdrCmp].second = 1;
      ntdrCmp++;
    }
  }

  int count = 5;
  for (int base = 32; base < 0x8000; base *= 2)
    if (pri || evpri)
      printf(" %2d: %d |", count++, ((array[size - 1] & base) > 0));
  if (pri || evpri)
    printf("\n");

  count = 5;
  for (int base = 32; base < 0x8000; base *= 2) {
    if ((array[size - 1] & base) > 0)
      if (pri)
        printf(" %02d: %s\n", count, errmess[count]);
    count++;
  }

  if (pri)
    printf("\n");
  // if(pri)for(int ii=0;ii<size;ii++) printf("arr: %d =  %d\n",ii,array[ii]);

  // - bit 15 { DATA bit, set by master when assembling group reply;
  // - bit 14 { END bit / CRC error (if DATA=1), set by master;
  // - bit 13 { ABORT bit / assembly error (if DATA=1), set by master;
  // - bit 12 { ERROR bit / AMSW error (if DATA=1), set by master;
  // - bit 11 { NEXT bit / timeout (if DATA=1), set by master;
  // - bit 10 { build conditions (FE power fault for event building);
  // - bit 9 { build errors (sequencer errors for event building);
  // - bit 8 { cumulative node status;
  // - bit 7 { if the node is building COMPRESSED events;
  // - bit 6 { if the node is building RAW events;
  // - bit 5 { if replying node is CDP;
  // - bits 4-0 { slave ID, set by master;

  if (out_flag)
    ev->TDRStatus[Jinfnum][tdrnum] = array[size - 1];
  if (out_flag)
    ev->ReadTDR[Jinfnum][tdrnum] = 1;

  int RawOffset = 0;
  if (TESTBIT(array[size - 1], 6)) { // RAW data present
    //    printf("size = %d\n", size);
    //    sleep(10);
    RawOffset = 1024; // RAW data present
    if (pri)
      printf("|->RAW data present\n");
    if (pri && out_flag)
      printf("Filling Event and Histograms for JINF %d, TDR %d (RAW)\n", Jinfnum, numnum);
    int tdrnumraw = 0;
    int count = 0;
    if (out_flag) {
      tdrnumraw = FindPos(numnum + 100 * Jinfnum);
      if (tdrnumraw < 0) {
        printf("DecodeDataAMS::ReadOneTDR::Cannot-Find-TDR-%d-RAW\n", numnum + 100 * Jinfnum);
        exit(4);
      }
      calibAMS *cal = &(cals[numnum + 100 * Jinfnum]);
      for (int kk = 0; kk < 320; kk++) {
        ev->RawSignal[Jinfnum][tdrnumraw][kk] = array[count];           // first ADC on S
        ev->RawSignal[Jinfnum][tdrnumraw][320 + kk] = array[count + 1]; // second ADC on S
        ev->RawSignal[Jinfnum][tdrnumraw][640 + kk] = array[count + 2]; // ADC on K
        //	printf("RAW %d %d  %d\n",kk,ev->RawSignal[FindTDRPos(tdrnum)][kk],array[count]);
        count += 3;
      }
      for (int kk = 960; kk < 1024; kk++) { // remaining (320->384) on ADC on K
        ev->RawSignal[Jinfnum][tdrnumraw][kk] = array[kk];
      }
      for (int cc = 0; cc < 1024; cc++) {
        //	printf("%04d) %f %f %f -> %f\n", cc, ((double)ev->RawSignal[tdrnumraw][cc])/8.0, cal->ped[cc],
        // cal->sig[cc], (ev->RawSignal[tdrnumraw][cc]/8.0-cal->ped[cc])/cal->sig[cc]);
        ev->CalPed[Jinfnum][tdrnumraw][cc] = cal->ped[cc];
        ev->CalSigma[Jinfnum][tdrnumraw][cc] = cal->sig[cc];
        ev->CalStatus[Jinfnum][tdrnumraw][cc] = cal->status[cc];
        if (cal->sig[cc] > 0.125 && // not a dead channel
            cal->sig[cc] < 10.0) {  // not a noisy channel
          ev->RawSoN[Jinfnum][tdrnumraw][cc] =
              (ev->RawSignal[Jinfnum][tdrnumraw][cc] - cal->ped[cc]) / cal->sig[cc];
        } else {
          ev->RawSoN[Jinfnum][tdrnumraw][cc] = 0.0;
        }
      }
      
      //	if (!kClusterize) {//otherwise the histos will be filled better with the clusters
      if (!TESTBIT(array[size - 1], 7) && // if we're purely RAW is better to fill the histograms also in this
	  // "naive" way (weighting of strip by signal). But if we're also in CMP
	  // (-> we're in MIXED), the compressed ones would be better
	  !kClusterize // if we're clusterizing the RAW event is better to fill the histograms with the clusters
	  ) {
	FillRawHistos(numnum, Jinfnum, ev, cal);
      }

      //      printf("%d %f %f %f %f\n", kClusterize, shighthreshold, slowthreshold, khighthreshold, klowthreshold);

      // this searches for clusters and if found Fill the histograms as in the CMP case
      if (kClusterize)
        Clusterize(numnum, Jinfnum, ev, cal);
    }
  }

  if (TESTBIT(array[size - 1], 7)) { // Compressed data present
    if (pri)
      printf("|->Compressed data present\n");
    if (pri && out_flag)
      printf("Filling Event and Histograms for JINF %d, TDR %d (CMP)\n", Jinfnum, numnum);
    // dump clusters
    int count = RawOffset;
    while (count < (size - 1)) {
      int bad = 0;
      int lenword = (array[count++]);
      int cluslen = (lenword & 0x7F) + 1;
      int Sig2NoiStatus = (lenword & 0xFF80) >> 7;
      int addword = (array[count++]);
      int clusadd = addword & 0x3FF;
      int CNStatus = (addword & 0x3C00) >> 10;
      int PowBits = (addword & 0xC000) >> 14;
      float sig[MAXLENGHT];
      if (pri)
        printf("Cluster: add=%d  lenght=%d\n", clusadd, cluslen);
      for (int hh = 0; hh < cluslen; hh++) {
        if (pri)
          printf("Signal: %d, Pos:%d\n", array[count], hh);
        if (hh < MAXLENGHT) {
          sig[hh] = array[count] / 8.;
          if (pri)
            printf("        %f, Pos: %d\n", sig[hh], hh);
        } else
          bad = 1;
        count++;
      }

      if (out_flag) {
        //	if (!kClusterize) {//what happens if is mixed mode? I would write the same cluster twice... So let's NOT
        // write the "on-line" clusters in the root file but only the "offline" ones... This remove the "online"
        // clusters even if is not 'mixed' but only 'compressed' but I think is 'safer'...
        if (!(TESTBIT(array[size - 1], 6)) // we're only in compressed
            ||                             // OR
            (TESTBIT(array[size - 1],
                     6) && // we're ALSO in RAW mode --> mixed. There's also the raw event, so this cluster would be
                           // potentially found also by the offline clusterization. This would cause a 'double counting'
             !kClusterize) // we're not clusterizing offline, so is safe to AddCluster and is needed otherwise the
                           // cluster would be not present at all in the Tree
        ) {
          calibAMS *cal = &(cals[numnum + 100 * Jinfnum]);
	  //AddCluster is also taking care of filling the histos
          AddCluster(ev, cal, numnum, Jinfnum, clusadd, cluslen, Sig2NoiStatus, CNStatus, PowBits, bad, sig);
        }
      }
    }

    //      int cc=1;
    //      //Read the  cnoise values
    //      for (int ii=size-17;ii<size-1;ii++){
    //        if(pri)printf("Cnoise %d:  %f \n",cc++,(array[ii])/8.);
    //        if(out_flag)ev->CNoise[tdrnum][ii-size+17]=(array[ii])/8.;
    //      }
  }
  if (pri)
    printf("\n");
  if (pri)
    printf("End of the TDR pos:%ld  \n", ftell(rawfile) / 2);
  // free the memory for the next tdr
  free(array);

  return 0;
}

//=============================================================================================

void DecodeDataAMS::FindCalibs() {

  struct stat buf;
  char name1[350];
  char nameA[350];
  char nameB[350];
  char nameMC[350];
  int run2;
  int runA;
  int runB;
  int runMC = 0; // added by Viviana. MD: see if really needed

  FILE *calfile[NTDRS];
  int old = pri;
  bool afterclose = false;

  if (!kMC) {
    if ((ntdrCmp + ntdrRaw) < 1) {
      printf("No TDR in CMP or RAW mode --> No DSP Calib can be found\n");
      return;
    }

    int tdrprobe = tdrMap[0].first;

    for (runB = runn; runB > 0; runB--) {
      sprintf(nameB, "%s/%06d_%04d.cal", rawCaldir, runB, tdrprobe);
      //    printf("%s\n", nameB);
      if (stat(nameB, &buf) == 0) {
        //      printf("First calib Found %s run %d\n", name1, run2);
        break;
      }
    }

    for (runA = runn; runA < (runn + (runn - runB)); runA++) {
      sprintf(nameA, "%s/%06d_%04d.cal", rawCaldir, runA, tdrprobe);
      //    printf("%s\n", nameA);
      if (stat(nameA, &buf) == 0) {
        //      printf("First calib Found %s run %d\n", name1, run2);
        afterclose = true;
        break;
      }
    }

    if (afterclose) {
      run2 = runA;
      sprintf(name1, "%s/%06d_%04d.cal", rawCaldir, runA, tdrprobe);
    } else {
      run2 = runB;
      sprintf(name1, "%s/%06d_%04d.cal", rawCaldir, runB, tdrprobe);
    }

    printf("Closest calib Found %s run %d\n", name1, run2);

    if (run2 < 40) {
      printf("Cannot find any calibration done before the requested run %d\n", runn);
      printf("I give up. Bye.\n");
      exit(2);
    } else
      printf("Searching the calib files for the other TDRs, if any\n");

    for (int ii = 0; ii < ntdrCmp + ntdrRaw; ii++) {
      int Jinfnum = tdrMap[ii].first / 100;
      int tdrnum = tdrMap[ii].first - Jinfnum * 100;
      int index = Jinfnum * 100 + tdrnum;
      sprintf(name1, "%s/%06d_%02d%02d.cal", rawCaldir, run2, Jinfnum, tdrnum);
      calfile[index] = fopen(name1, "r");
      if (!calfile[index]) {
        printf("Cannot find the calib %s for the requested calib run %d\n", name1, run2);
        printf("I give up. Bye.\n");
        exit(2);
      }
      printf("Found file %s\n", name1);
      //    printf("---- %d\n", index);
      ReadCalib(calfile[index], &(cals[index]));
    }
  }      // end if !kMC
  else { // for MC
    printf("%scalMC_%04d.cal TDRS %d\n", rawCaldir, runMC, ntdrMC);
    for (int iic = 0; iic < ntdrMC; iic++) {
      // sprintf(nameMC,"%s/calMC_%04d.cal", rawCaldir, iic);
      sprintf(nameMC, "%scalMC_0000.cal", rawCaldir);
      printf("%scalMC_0000.cal \n", rawCaldir);
      calfile[iic] = fopen(nameMC, "r");
      printf("Found file %s for MCTDR %d \n", nameMC, iic);
      ReadCalib_mc(calfile[iic], &(cals[iic]));
    }
  }

  pri = old;
}

//=============================================================================================

int DecodeDataAMS::ReadCalib(FILE *fil, calibAMS *cal) {

  int a, b, c;
  float d, e;
  bool failed = false;
  for (int ii = 0; ii < 16; ii++) {
    auto ret = fscanf(fil, "%d, %f, %f", &a, &d, &e);
    if (!ret)
      failed = true;
  }
  for (int ii = 0; ii < 1024; ii++) {
    auto ret = fscanf(fil, "%d  %d  %d  %f %f %f %f %d  ", &a, &b, &c, &(cal->ped[ii]), &(cal->rsig[ii]),
                      &(cal->sig[ii]), &d, &(cal->status[ii]));

    if (!ret)
      failed = true;

    if (pri)
      printf("%d  %f %f %f %d\n", ii, cal->ped[ii], cal->rsig[ii], cal->sig[ii], cal->status[ii]);
  }

  return !failed;
}

//=============================================================================================

int DecodeDataAMS::ReadCalib_mc(FILE *fil, calibAMS *cal) {
  // for now keeping kind of data like calibration file..

  // int a,b,c;
  //	float d,e;
  int old = 1;
  old = pri;
  pri = 0;

  /* for (int ii=0;ii<24;ii++)
     fscanf(fil,"%d, %f, %f",&a,&d,&e);
  */

  ////for (int ii=0;ii<1024;ii++){
  for (size_t ii = 0; ii < NVAS * NCHAVA; ii++) {

    /*
                fscanf(fil,"%d  %d  %d  %f %f %f %f %d  ",&a,&b,&c,
                &(cal->ped[ii]),
                &(cal->rsig[ii]),
                &(cal->sig[ii]),
                &d,
                &(cal->status[ii]));
    */

    //// fix values for now
    cal->ped[ii] = 420.;
    cal->rsig[ii] = 1.250;
    cal->sig[ii] = 1.125;
    cal->status[ii] = 0;

    // if(pri)printf("%d  %f %f %f %d\n",ii,cal->ped[ii],cal->rsig[ii],cal->sig[ii],cal->status[ii]);
  }

  pri = old;

  return 1;
}

//=============================================================================================

int DecodeDataAMS::ReadOneJINF() {
  unsigned short int size;
  unsigned short int noknown;
  unsigned short int tdrnoeventmask;
  unsigned short int status;

  // Read The Event Size
  if (pri)
    printf("pos:%ld  ", ftell(rawfile) / 2);
  ReadFile(&size, sizeof(size), 1, rawfile);
  if (pri)
    printf("JINFsize    %d \n", size);

  long int base = ftell(rawfile) / 2;

  if (pri)
    printf("pos:%ld  \n", ftell(rawfile) / 2);

  // Store the position on the file
  long int pos = ftell(rawfile);

  // skip all the TDR data
  if (int ret0 = fseek(rawfile, (size - 1) * 2, SEEK_CUR)) {
    printf("Fatal: error during file skip ret0=%d \n", ret0);
    exit(3);
  }

  // read the last word of the JINF event
  if (pri)
    printf("pos:%ld  ", ftell(rawfile) / 2);

  ReadFile(&status, sizeof(status), 1, rawfile);

  if (pri || evpri)
    printf("Jinf status    %d : address %02d  status ", status, status & 0x1f);

  // e questo numero se non c'è un jinj master non sò chi glielo potrebbe aver messo
  // (però se sta nello status che comunque ci sarà, al limite sarà 0...)
  int Jinfnum = ((unsigned int)status) & 0x1f;

  int count = 1;
  for (int base = 32; base < 0x8000; base *= 2)
    if (pri || evpri)
      printf(" %2d %d", count++, ((status & base) > 0));
  if (pri || evpri)
    printf("\n");

  count = 5;
  for (int base = 32; base < 0x8000; base *= 2) {
    if ((status & base) > 0)
      if (pri || evpri)
        printf("%02d: %s\n", count, errmess[count]);
    count++;
  }
  if (pri || evpri)
    printf("\n \n");

  if (out_flag)
    ev->JINFStatus[Jinfnum] = status;

  //  if((status&0x400)>0){
  if ((status & 0x200) > 0 || (status & 0x400) > 0) {
    printf("=======================> JINF Error!!!  Skip this event: %d\n", ev->Evtnum);
    count = 5;
    for (int base = 32; base < 0x8000; base *= 2) {
      if ((status & base) > 0)
        printf("%02d: %s\n", count, errmess[count]);
      count++;
    }
    return 10;
  }
  // come back to the previous pos
  if (int ret0 = fseek(rawfile, pos, SEEK_SET)) {
    printf("Fatal: error during file skip %d\n", ret0);
    exit(3);
  }

  if (!out_flag)
    JinfMap[nJinf++] = status & 0x1f;

  // Read the TDR
  while (ftell(rawfile) / 2 < base + size - 1)
    ReadOneTDR(status & 0x1f);
  // Read the last 2 words before status
  ReadFile(&noknown, sizeof(noknown), 1, rawfile);
  ReadFile(&tdrnoeventmask, sizeof(tdrnoeventmask), 1, rawfile);
  if (pri || evpri)
    printf("Tdrs with no event Mask: %d\n", tdrnoeventmask);
  for (size_t ii = 0; ii < NTDRS; ii++) {
    if (tdrnoeventmask & (1 << ii)) {
      if (pri || evpri)
        printf("A tdr (%ld) replied with no event...\n", ii);
      if (!out_flag) {
        tdrMap[ntdrRaw + ntdrCmp].first = ii + 100 * (status & 0x1f);
        tdrMap[ntdrRaw + ntdrCmp].second = 1;
        ntdrCmp++;
      }
    }
  }

  // Reread the last word
  ReadFile(&status, sizeof(status), 1, rawfile);
  if (pri || evpri)
    printf("Jinf status    %d \n ", status);

  return 0;
}

int DecodeDataAMS::GetTdrNum(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].first;
}

int DecodeDataAMS::GetTdrType(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].second;
}

void DecodeDataAMS::mysort(laddernumtype *aa, int nel) {

  laddernumtype *bb = new laddernumtype[nel];

  int min = 10000;
  int max = -1;
  for (int count = 0; count < nel; count++) {
    for (int ii = 0; ii < nel; ii++) {
      if (aa[ii].first < min && aa[ii].first > max) {
        min = aa[ii].first;
        bb[count] = aa[ii];
      }
    }
    max = bb[count].first;
    min = 10000;
  }

  for (int count = 0; count < nel; count++)
    aa[count] = bb[count];

  delete[] bb;
}
