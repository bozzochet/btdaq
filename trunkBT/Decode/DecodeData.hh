#ifndef DecodeData_h
#define DecodeData_h
#include "Event.hh"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include <cstdio>
#include <vector>

//(could be defined by Makefile)
//#define CALOCUBE 

typedef struct calib{
  float ped[4096]; // was [1024]: is backward compatible?
  float rsig[4096]; // was [1024]: is backward compatible?
  float sig[4096]; // was [1024]: is backward compatible?
  int status[4096]; // was [1024]: is backward compatible?

} calib;

#pragma pack(push, 1)

//typedef struct header { // gcc 4.3, considers 'typedef' useless // (what??)
struct header {			//for file writing NOT in AMSBlock
  int run;    			// run number
  char date[50];        	// date // why we put 50! In the RHClass is 30! Should be shorter, but anyhow it will be truncated when passed to RHClass
  double gonpar[4];		// goniometer parameters
  unsigned int refmaskjj;//16/08/2014 - On Mac this is seen as long 8 (instead of 4) and the reader is read wrongly
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

class DecodeData {
  
private:
  FILE* rawfile;
  
  TFile *mcf;//added by Viviana
  TTree *mcrt;//added by Viviana
  TTree *mcht;//added by Viviana
  
  char rawname[300];
  char rawdir[300];
  char rawCaldir[300];
  //  int Xstant;
  int runn;
  int tdroffset;
  calib cals[NJINF*NTDRS];
  int pri;
  int evpri;
  int nJinf;
  int JinfMap[NJINF];
  int ntdrRaw;
  int ntdrCmp;
  laddernumtype tdrMap[NJINF*NTDRS];
  int ntdrMC;//added by Viviana, controllare se matcha con le modifiche che avevo fatto alle Map
  int tdrAlign[NJINF*NTDRS];//added by Viviana, credo...
  int out_flag;
  char type[10]; //JinF or JinJ
  void FindCalibs ();
  int  ReadCalib(FILE * fil,calib* cal);
  int  ReadCalib_mc(FILE * fil,calib* cal);
  int FindPos(int tdrnum);
  int FindPosMC(int hitnum);//added by Viviana, serve veramente?
  int FindCalPos(int tdrnum);
  int ReadFile(void *ptr, size_t size, size_t nitems, FILE * stream);
  void mysort(laddernumtype* aa,int nel);

  bool kMC;
  
public:
  Event * ev;
  RHClass *rh;

  int nlayers;//added by Viviana
  double gene;//added by Viviana
  int gevt;//added by Viviana
  int evenum;

  int nhits;//added by Viviana
  int ntothits;
  int pphit;//added by Viviana

  //// generalization for nhits!=nlayers
  int hvol[MAXNHITS];//added by Viviana
  double hvolz[MAXNHITS];//added by Viviana
  double xcoord[MAXNHITS]; 
  double ycoord[MAXNHITS]; 
  double zcoord[MAXNHITS]; 
  //int echx[MAXNHITS]; //added by Viviana
  //int echy[MAXNHITS];//added by Viviana
  //double edep[MAXNHITS];  //added by Viviana
  double eEne[MAXNHITS];  //added by Viviana	
  double eDep[MAXNHITS];  //added by Viviana

  int pdg[MAXNHITS];
  int parid[MAXNHITS];
  int trid[MAXNHITS]; 

  //new GGS output for channels and deposits
  int chXY[MAXNHITS]; 
  int hitStrips[MAXNHITS];
  int simStrips[MAXNHITS];
  std::vector<int> * hitChan = 0;
  std::vector<double> * hitDep = 0;
  std::vector<int> *simChan = 0;
  std::vector<double> *simDep = 0;
  
  
  
  TH1F* hocc[NJINF*NTDRS];
  TH1F* hoccseed[NJINF*NTDRS];
  TH1F* hcharge[NJINF*NTDRS][2];
  TH1F* hsignal[NJINF*NTDRS][2];
  TH2F* hchargevsocc[NJINF*NTDRS];
  TH2F* hsignalvsocc[NJINF*NTDRS];
  TH1F* hson[NJINF*NTDRS][2];

  double shighthreshold;
  double slowthreshold;
  double khighthreshold;
  double klowthreshold;

  bool kClusterize;
  int cworkaround;
  
  void DumpRunHeader();
  
public:
  DecodeData(char * ifname, char* caldir, int run, int ancillary, bool _kMC=false);
  ~DecodeData();

  //generic
  void AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus, int PowBits, int bad, float* sig, bool kRaw=false);
  void Clusterize(int numnum, int Jinfnum, calib* cal);
  //moved to Event class
  //  double ComputeCN(int size, short int* Signal, float* pede, float* SoN, double threshold=3.0);

  inline int GetNTdrRaw() { return ntdrRaw;}
  inline int GetNTdrCmp() { return ntdrCmp;}
  int GetTdrNum(int pos);
  int GetTdrType(int pos);
  
  int SkipOneEvent(int evskip=1);
  int ReadOneEvent();

  void OpenFile(char* ifname, char* caldir, int run, int ancillary);
  void CloseFile();
  int EndOfFile();

  //data
  void OpenFile_data(char* ifname, char* caldir, int run, int ancillary);
  int ReadOneEvent_data();
  int ReadOneTDR(int jinf=0);
  int ReadOneJINF();
  int SkipOneEvent_data(int evskip=1);
  int EndOfFile_data();
  
  //mc
  //  void OpenFile_mc(char* ifname, char* caldir, int run, int ancillary);
  void OpenFile_mc(char* ifname);
  int ReadOneEvent_mc();
  int SkipOneEvent_mc(int evskip=1);
  
  int EndOfFile_mc();
  
  void SetPrintOff(){pri=0;}
  void SetPrintOn(){pri=1;}
  void SetEvPrintOff(){evpri=0;}
  void SetEvPrintOn(){evpri=1;}
};

#endif
