#ifndef ANALYZE_H_
#define ANALYZE_H_

#include <iostream>
#include <iomanip>
#include <fstream>

#include <TApplication.h>
#include <TSystem.h>
#include "TBrowser.h"
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#define COINCIDENCE_DT    100E-9            // Min time interval allowed between signals of same event
#define BOUNCE_DT         250E-9            // Time interval where sig on same ch are tagged as bouces
#define TRIG_DIFF_CAEN    145E-9            // Time difference between trigger in V1731 and V1720 (V1720 is delayed)
#define NTOTEV            1E+6              // Max # of ev to be analyzed
#define NSAMPFORPED        100              // # of samples for pedestal calculation
#define NSAMPFORPED1731     30              // # of samples for pedestal calculation
#define NSAMPFORPED1720     20              // # of samples for pedestal calculation for V1720
#define N_PMTs               9              // # of PMTs
#define N_HeTubes            5              // # of Helim-3 tubes
#define EV_MAX_SIZE        1E5              // Max size of 1 ev in file (int value)

#define PMT_THR_V1720        4              // Thr (in ADCs) to find PMT sig on V1720 (1=0.5mV, 40=20.0 mV)
#define PMT_THR_V1731        2              // Thr (in ADCs) to find PMT sig on V1731 (1=3.9mV,  5=19.5 mV)
#define HeTUBE_THR_V1720    50              // Thr (in ADCs) to find He tube sig on V1720 (1=0.5mV, 30=15.0 mV)

#define PMT_SCHM_THR_V1720     2            // Threshold for Schmitt trig implemented to find signals
#define PMT_SCHM_THR_V1731     1            // Threshold for Schmitt trig implemented to find signals
#define HeTUBE_SCHM_THR_V1720 40            // Threshold for Schmitt trig implemented to find signals

#define TRIG_THR_V1720       100            // Thr (in ADCs) to identify raising trigger on V1720
#define TRIG_THR_V1731        40            // Thr (in ADCs) to identify raising trigger on V1731
#define PISADATASIZE  (512*9+1024*2+512)
#include "TBrowser.h"

//----------------------------------------------------------------------------------------------------------
//----- Definitions of physics quantities ------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
Double_t     PMT_MIP[N_PMTs]={100,100,100,100,100,100,100,100,100};//Equivalent of a MIP signal in channel ADC
Double_t HeTubes_MIP[N_HeTubes]={100,100,100,100,100};             //Equivalent of a MIP signal in channel ADC

//----------------------------------------------------------------------------------------------------------
//----- Definitions for file management --------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
bool v1731=false, v1720=false, pisa=false;           // Boolean flags to identify the type pf data file
CAEN_V1731 *ev_v1731;                                // Object to access CAEN V1731
CAEN_V1720 *ev_v1720;                                // Object to access CAEN V1720
ifstream datafile[2];				     // Data file
ifstream pisadatafile;			             // Data file (for PISA data file)
Double_t ped1731[V1731_NCH];			     // Pedestal of each channel of board V1731
Double_t ped1720[V1720_NCH];			     // Pedestal of each channel of board V1720
string filename[2];				     // Name of input data file for both boards
string pisafilename;				     // Name of input Pisa data file

//----------------------------------------------------------------------------------------------------------
//----- Definitions for data analysis ----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
Int_t err_1731=0;
Int_t err_1720=0;
Int_t iev=0;                                           // Index of event
Int_t iev_sel=0;                                       // Counts event read in requested interval
Int_t iev_v1731_bad=0;                                 // Counts bad written event              for V1731
Int_t iev_v1731_wronglen=0;                            // Counts ev with too long len codified  for V1731
Int_t iev_v1731_toomanysamp=0;                         // Counts ev with too many samples       for V1731
Int_t iev_v1731_extracterror=0;                        // Counts ev which cannot be "extracted" for V1731
Int_t iev_v1731_multitrig=0;                           // Counts ev with multiple triggers      for V1731
Int_t iev_v1720_bad=0;                                 // Counts bad written event              for V1720
Int_t iev_v1720_wronglen=0;                            // Counts ev with too long len codified  for V1720
Int_t iev_v1720_toomanysamp=0;                         // Counts ev with too many samples       for V1720
Int_t iev_v1720_extracterror=0;                        // Counts ev which cannot be "extracted" for V1720
Int_t iev_v1720_multitrig=0;                           // Counts ev with multiple triggers      for V1720
Int_t firstev=1;                                       // First event to be considered for analysis
Int_t nevtot=(int)NTOTEV;                              // Total number of events to analize 
Int_t evlength[2]={0,0};                               // Total length of 1 ev for both boards
Int_t pisaevlen=0;                                     // Total len of 1 ev in Pisa file
Int_t evnum[2]={-1,-1};                               // ID of event written in data file
Int_t *buff[2];                                        // Buffer to read 1 ev from file (both boards)
Int_t counts=0;                                        // Generic counter (used to counts samplings)
Int_t kfound=-1;                                       // Identify first sample over thr in one signal
Double_t ymin=0;			  	       // y-coord (adc ) of max amplitude of signal
Double_t ymax=0;			  	       // used to look for sample where signal return under thr
Double_t ylev=0;			  	       // base level to look for signal (adc)
Double_t xmax=0;				       // x-coord (time) of max of signal
Double_t xstart=0;				       // x-coord (time) of beginning of signal
Int_t firsttosave=-1;                                  // Identify the fist sample of a signal
Int_t lastsaved=-1;                                    // Identify the last sample of a signal
Double_t charge;		  		       // Charge (integral) of one signal
Int_t nentry=0;                                        // Number of entries in output ROOT file
bool display=false;                                    // Flag to select DISPLAY behaviour
bool enddata=false;                                    // Flag to identify the end of data file
bool raising;                                          // This flag tells if signal rising time has been found 
Double_t *xxx,*yyy;
Int_t counthitsPMT_single;                             // Count # of single signals for PMTs for one event	 
Int_t counthitsPMT_all;                                // Count all signals on PMTs for one event		 
Int_t counthitsPMT_coinc;                              // Count # of signals in coincidence on PMTs for one event
Int_t counthitsHe_single;                              // Count # of single signals for tubes for one event	 
Int_t counthitsHe_all;                                 // Count all signals on tubes for one event		 
Int_t counthitsHe_coinc;                               // Count # of signals in coincidence on tubes for one event
Int_t nsamp;                                           // # of samplings for one channel
Double_t time1,time2;                                  // Used to measure signal time of arrival
unsigned long timetag[2];                              // Time tag given by V1731 and V1720
Int_t pisaoffset;                                      // Offset where the put pointer is located
UInt_t tail[4];                                        // Buffer for reading file trailer

struct EVENT_V1731 {
  Int_t    num[V1731_NCH];                               // # of good bunches of data in each channel
  Int_t    len[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];     // Length of single bunches on each channel
  Int_t    pos[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];     // Sampling position of 1st datum of each bunch 
  vector<Double_t> adc[V1731_NCH];                       // All adc values for each channel
  vector<Double_t> sam[V1731_NCH];                       // All samp numbers for each channel
  vector<Double_t> sigadc[V1731_NCH];                    // Signal adc values for each channel
  vector<Double_t> sigsam[V1731_NCH];                    // Signal samp numbers for each channel
  Int_t    signum[V1731_NCH];                            // # of found signals for each channel
  Int_t    siglen[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];  // length of found signals for each channel
  Int_t    sigamp[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];  // amplitude (ADC ch) of found signals for each ch
  Double_t trigtime;                                     // Time of trigger arrival on board V1731
  Double_t timeatmax[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];  // Center in time of each signal for each channel
  Double_t arrivaltime[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];// Center in time of each signal for each channel
  Double_t charge[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];     // Charge of each signal for each channel
  bool   flag_coinc[V1731_NCH][V1731_MAXNPULSESPERCH_ZS]; // Flag to tag signals in coincidence with other signals (charged particles?) 
  bool   flag_bounce[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];// Flag for signals near main (first) signal
} ev1731;

struct EVENT_V1720 {
  Int_t    num[V1720_NCH];                               // # of good bunches of data in each channels
  Int_t    len[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];     // Length of single bunches on each channel
  Int_t    pos[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];     // Sampling position of 1st datum of each bunch 
  vector<Double_t> adc[V1720_NCH];                       // All samplings for each channel
  vector<Double_t> sam[V1720_NCH];                       // All samplings for each channel
  vector<Double_t> sigadc[V1720_NCH];                    // Signal adc values for each channel
  vector<Double_t> sigsam[V1720_NCH];                    // Signal samp numbers for each channel
  Int_t    signum[V1720_NCH];                            // # of found signals for each channels
  Int_t    siglen[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];  // length of found signals for each channels
  Double_t sigamp[V1731_NCH][V1731_MAXNPULSESPERCH_ZS];  // amplitude (ADC ch) of found signals for each ch
  Double_t trigtime;                                     // Time of trigger arrival on board V1720
  Double_t timeatmax[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];// Center in time of each signal for each channel
  Double_t arrivaltime[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];// Center in time of each signal for each channel
  Double_t charge[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];  // Charge of each signal for each channel
  bool   flag_coinc[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];// Charge of each signal for each channel
  bool   flag_bounce[V1720_NCH][V1720_MAXNPULSESPERCH_ZS];// Flag for signals near main (first) signal
} ev1720;

//----------------------------------------------------------------------------------------------------------
//----- Definitions for ROOT display and graphics ----------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
Int_t icanv;
TApplication *a;
TFile *rootfile;                                     // Pointer to root file
TTree *SignalTree;                                   // Tree to store all signal info
TTree *LonelySignalTree;                             // Tree to store lonely signal info (after cuts)
TTree *CoincSignalTree;                              // Tree to store lonely signal info (after cuts)
struct TCanvas *canv;                                // Pointer to canvas for event display
TGraph *graph1731[V1731_NCH];
TGraph *graph1720[V1720_NCH];

//----------------------------------------------------------------------------------------------------------
//----- Definitions of structure for root-ple filling ------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
struct Signal_Data{
  UInt_t   ev1731;                                   // Event number taken from V1731 data
  UInt_t   ev1720;                                   // Event number taken from V1720 data 
  UInt_t   ttag1731;                                 // Time tag stored in event data for V1731
  UInt_t   ttag1720;                                 // Time tag stored in event data for V1720
  UInt_t   det;                                      // 0 = PMT ; 1 = He tube
  UInt_t   mod;                                      // If PMT: 0...8 ; if He tube: 0...4
  UInt_t   num;                                      // ID of signal on its module (starting from 0)
  UInt_t   single;                                   // 1=lonely (single) signal, 0=other signals in coincidence
  UInt_t   bounce;                                   // 1=bounce signal (within 200ns from previous), 0=good signals
  Double_t trig;                                     // Trigger arrival time estimated at rising
  Double_t t1;                                       // Signal arrival time estimated at rising
  Double_t t2;                                       // Signal arrival time estimated at maximum
  Double_t dt1;                                      // Time enlapsed from last signal on same module (=0 if first)
  Double_t q1;                                       // Signal strenght: (integral of signal shape in time)/ (50 ohm) (in pC)
  Double_t q2;                                       // Signal strenght: maximum amplitude of signal (in Volt)
};
struct Signal_Data sigdat;                           // Structure to hold single signal data
 
//----------------------------------------------------------------------------------------------------------
//----- Definitions of ROOT histograms ---------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
double inflim,suplim;                                //----- Inf and sup limit for histogram definitions

TH1F *TOTtdistPMT_all,*TOTqdistPMT_all,*TOTtdistPMT_single,*TOTqdistPMT_single,*TOTtdistPMT_coinc,*TOTqdistPMT_coinc;
TH1F *EvLen1720;                                     //----- Distrib. of event length for V1720
TH1F *EvLen1731;                                     //----- Distrib. of event length for V1731

TH1F *nhitsPMT_single;                               //----- Total # of hits (signals) in all PMTs
TH1F *nhitsPMT_all;                                  //----- Total # of hits (signals) in all PMTs
TH1F *nhitsPMT_coinc;                                //----- Total # of hits (signals) in all PMTs
TH1F *nhitsPMTHe_single;                             //----- Total # of hits (signals) in all PMTs + He tubes
TH1F *nhitsPMTHe_all;                                //----- Total # of hits (signals) in all PMTs + He tubes
TH1F *nhitsPMTHe_coinc;                              //----- Total # of hits (signals) in all PMTs + He tubes
TH1F *qdistAllPMT,*qdistAllHe;                       //----- Global signal distrib.for PMTs and He tubes 

TH1F *trigtime1731;                                  //----- Trigger arrival time distrib
TH1F *trigtime1720;                                  //----- Trigger arrival time distrib

TH1F *pedPMT[N_PMTs];                                //----- Pedestal distribution for single PMT
TH1F *pedHe[N_HeTubes];                              //----- Pedestal distribution for single He tube

TH1F *tdistPMT_all[N_PMTs];                          //----- Time distrib. for signal on each PMT
TH1F *tdistPMT_single[N_PMTs];                       //----- Time distrib. for 'singlet' signals on each PMT (excluded coinc. signals)
TH1F *tdistPMT_coinc[N_PMTs];                        //----- Time distrib. for coinc signals on each PMT
TH1F *tdistHe_all[N_HeTubes];                        //----- Time distrib. for signal on each He tube
TH1F *tdistHe_single[N_HeTubes];                     //----- Time distrib. for 'singlet' signals on each He tube (excluded coinc. signals)
TH1F *tdistHe_coinc[N_HeTubes];                      //----- Time distrib. for coinc signals on each He tube

TH1F *qdistPMT_single[N_PMTs];                       //----- Distrib. of signal amplitude for each PMT (no coinc.)
TH1F *qdistPMT_coinc[N_PMTs];                        //----- Distrib. of signal amplitude for each PMT (only coinc. signals)
TH1F *qdistPMT_all[N_PMTs];                          //----- Distrib. of signal amplitude for each PMT (all signals)
TH1F *qdistHe_single[N_HeTubes];                     //----- Distrib. of signal amplitude for each He tube (no coinc.)
TH1F *qdistHe_coinc[N_HeTubes];                      //----- Distrib. of signal amplitude for each He tube (only coinc. signals)
TH1F *qdistHe_all[N_HeTubes];                        //----- Distrib. of signal amplitude for each He tube (all signals)

#endif
