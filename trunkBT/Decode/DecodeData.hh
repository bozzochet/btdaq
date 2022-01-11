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

class calib{
public:
  float* ped;
  float* rsig;
  float* sig;
  int* status;

  calib(){
    ped = new float[Event::NVAS*Event::NCHAVA]; // was [1024]: is backward compatible?
    rsig = new float[Event::NVAS*Event::NCHAVA]; // was [1024]: is backward compatible?
    sig = new float[Event::NVAS*Event::NCHAVA]; // was [1024]: is backward compatible?
    status = new int[Event::NVAS*Event::NCHAVA]; // was [1024]: is backward compatible?    
  };
};

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
  
protected:
  FILE* rawfile;
  
  TFile *mcf = nullptr;//added by Viviana
  TTree *mcrt = nullptr;//added by Viviana
  TTree *mcht = nullptr;//added by Viviana
  
  char rawname[300];
  char rawdir[300];
  char rawCaldir[300];
  //  int Xstant;
  int runn;
  int tdroffset;
  calib* cals;
  int pri = 0;
  int evpri;
  int nJinf;
  int* JinfMap;
  int ntdrRaw = 0;
  int ntdrCmp = 0;
  laddernumtype* tdrMap;
  int ntdrMC;//added by Viviana, controllare se matcha con le modifiche che avevo fatto alle Map
  int* tdrAlign;//added by Viviana, credo...
  int out_flag;
  double m_adcUnits = 8.0;
  unsigned int m_defaultShift = 640;
  unsigned int m_defaultArraySize = 384;
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
    
  TH1F** hocc;
  TH1F** hoccseed;
  TH1F*** hcharge;
  TH1F*** hsignal;
  TH2F** hchargevsocc;
  TH2F** hsignalvsocc;
  TH1F*** hson;

  double shighthreshold;
  double slowthreshold;
  double khighthreshold;
  double klowthreshold;

  bool kClusterize;
  int cworkaround;
  
  virtual void DumpRunHeader();
  
public:
  DecodeData() = default;
  DecodeData(char * ifname, char* caldir, int run, int ancillary, bool _kMC=false);
  virtual ~DecodeData();

  //generic
  void AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus, int PowBits, int bad, float* sig, bool kRaw=false);
  virtual void Clusterize(int numnum, int Jinfnum, calib* cal);
  //moved to Event class
  //  double ComputeCN(int size, short int* Signal, float* pede, float* SoN, double threshold=3.0);

  virtual inline int GetNTdrRaw() { return ntdrRaw;}
  virtual inline int GetNTdrCmp() { return ntdrCmp;}
  int GetTdrNum(int pos);
  int GetTdrType(int pos);
  
  int SkipOneEvent(int evskip=1);
  virtual int ReadOneEvent();

  virtual void OpenFile(const char* ifname, const char* caldir, int run, int ancillary);
  void CloseFile();
  int EndOfFile();

  //data
  void OpenFile_data(const char* ifname, const char* caldir, int run, int ancillary);
  int ReadOneEvent_data();
  int ReadOneTDR(int jinf=0);
  int ReadOneJINF();
  int SkipOneEvent_data(int evskip=1);
  int EndOfFile_data();
  
  //mc
  //  void OpenFile_mc(char* ifname, char* caldir, int run, int ancillary);
  void OpenFile_mc(const char* ifname);
  int ReadOneEvent_mc();
  int SkipOneEvent_mc(int evskip=1);
  
  int EndOfFile_mc();
  
  void SetPrintOff(){pri=0;}
  void SetPrintOn(){pri=1;}
  void SetEvPrintOff(){evpri=0;}
  void SetEvPrintOn(){evpri=1;}

  TTree *GetMCTruth(){return mcht;};//CB
};

#endif
