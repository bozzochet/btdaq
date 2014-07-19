#ifndef SHARE_H 
#define SHARE_H

#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
  
#define GAIN_MAX 16
 
#define max_number_va 16  
#define channel_per_va 64 
                      
#define HISTO_SCOPE_BIN              1024 
#define HISTO_SIGNAL_BIN             1024 
#define HISTO_PEDSUB_BIN             1024 
#define HISTO_SIGNAL_MEAN_BIN        1024 
#define HISTO_SIGNAL_HITS_BIN        1024 
#define HISTO_RAWCAL_BIN              800 
#define HISTO_PEDESTAL_BIN           1024 
#define HISTO_PEDSUM_BIN              100 
#define HISTO_SIGMARAW_BIN           1024 
#define HISTO_SIGRAWSUM_BIN           100 
#define HISTO_CN_BIN                  256 
#define HISTO_SIGMA_BIN              1024 
#define HISTO_SIGSUM_BIN              100 
#define HISTO_GAIN_BIN               1024 
#define HISTO_GAIN_VALUE_BIN         1024 
#define HISTO_GAINPLOT_BIN            256 
#define HISTO_SHAPING_BIN              90 
#define HISTO_CLUSTER_LENGTH_S_BIN     10 
#define HISTO_CLUSTER_LENGTH_K_BIN     10 
#define HISTO_CLUSTER_NUMBER_S_BIN     20 
#define HISTO_CLUSTER_NUMBER_K_BIN     20 
#define HISTO_CLUSTER_COG_BIN        1024 
#define HISTO_CLUSTER_MAXLOC_BIN      101 
#define HISTO_CLUSTER_INTEGRAL_S_BIN  300 
#define HISTO_CLUSTER_INTEGRAL_K_BIN  300 
#define HISTO_CLUSTER_SOVERN_S_BIN     60 
#define HISTO_CLUSTER_SOVERN_K_BIN     60 
#define HISTO_STATS_BIN              1024
#define HISTO_SPECTRUMS_BIN           320
#define HISTO_SPECTRUMK_BIN           384


enum sHISTO{  
  kHISTO_SCOPE=1, 
  kHISTO_SIGNAL=2,
  kHISTO_SIGNAL_MEAN=3, 
  kHISTO_SIGNAL_HITS=4, 
  kHISTO_RAWCAL=5, 
  kHISTO_PEDESTAL=6, 
  kHISTO_PEDSUM=7, 
  kHISTO_SIGMARAW=8, 
  kHISTO_SIGRAWSUM=9, 
  kHISTO_CN=10, 
  kHISTO_SIGMA=11, 
  kHISTO_SIGSUM=12, 
  kHISTO_GAIN=13,
  kHISTO_GAIN_VALUE=14, 
  kHISTO_GAINPLOT=15,
  kHISTO_SHAPING=16, 
  kHISTO_CLUSTER_LENGTH_S=21, 
  kHISTO_CLUSTER_LENGTH_K=22, 
  kHISTO_CLUSTER_NUMBER_S=23, 
  kHISTO_CLUSTER_NUMBER_K=24, 
  kHISTO_CLUSTER_COG=25, 
  kHISTO_CLUSTER_MAXLOC=26, 
  kHISTO_CLUSTER_INTEGRAL_S=27, 
  kHISTO_CLUSTER_INTEGRAL_K=28, 
  kHISTO_CLUSTER_SOVERN_S=29, 
  kHISTO_CLUSTER_SOVERN_K=30, 
  kHISTO_PEDSUB=31,
  kHISTO_PEDREF=32,
  kHISTO_PEDDIFF=33,
  kHISTO_SIGMACOMP=34,
  kHISTO_SPECTRUM_S1=35,
  kHISTO_SPECTRUM_S2=36,
  kHISTO_SPECTRUM_K=37,
  kHISTO_SIGMARAWCOMP=38,
  kSAVE_HISTO_RAWCAL=39, 
}; 

 
//#define HISTO_SIZE 15371848
// according to sizeof, the real size is 15371848
#define HISTO_SIZE 30554432 
//#define HISTO_SIZE 40000000 <--- makes the program crash
struct Histo_buf { 
  int Nbin[40];         
  float Minimum[40];    
  float Maximum[40];    
  float MinY[40];       
  float MaxY[40];       
                        
  int fhGainMin[max_number_va][channel_per_va];
  int fhGainMax[max_number_va][channel_per_va];

  int fhStatus[HISTO_STATS_BIN+2];
  int fhDSPStatus[HISTO_STATS_BIN+2];
    
  double fhScope[HISTO_SCOPE_BIN+2];   
  double fhSignal[HISTO_SIGNAL_BIN+2];    
  double fhSignalMean[HISTO_SIGNAL_MEAN_BIN+2]; 
  double fhSignalHits[HISTO_SIGNAL_HITS_BIN+2]; 
    double fhRawCal[max_number_va][channel_per_va][HISTO_RAWCAL_BIN+2]; 
  //double fhRawCal[1][channel_per_va][HISTO_RAWCAL_BIN+2]; 
  double fhPedestal[HISTO_PEDESTAL_BIN+2];
  double fhRefPedestal[HISTO_PEDESTAL_BIN+2];
  double fhDSPPedestal[HISTO_PEDESTAL_BIN+2];
  double fhDiffPedestal[HISTO_PEDESTAL_BIN+2];
  double fhPedSum[HISTO_PEDSUM_BIN+2]; 
  double fhSigmaRaw[HISTO_SIGMARAW_BIN+2]; 
  double fhRefSigmaRaw[HISTO_SIGMARAW_BIN+2]; 
  double fhDSPSigmaRaw[HISTO_SIGMARAW_BIN+2]; 
  double fhSigRawSum[HISTO_SIGRAWSUM_BIN+2]; 
  double fhCN[max_number_va][HISTO_CN_BIN+2]; 
  double fhSigma[HISTO_SIGMA_BIN+2];  
  double fhRefSigma[HISTO_SIGMA_BIN+2];  
  double fhDSPSigma[HISTO_SIGMA_BIN+2];  
  double fhSigSum[HISTO_SIGSUM_BIN+2];  
  double fhGain[HISTO_GAIN_BIN+2];  
  double fhGainValue[max_number_va][channel_per_va][HISTO_GAIN_VALUE_BIN+2]; 
  double fhDACGain[GAIN_MAX]; 
  double fhDACGainError[GAIN_MAX]; 
  double fhMeanGain[max_number_va][channel_per_va][GAIN_MAX]; 
  double fhSigmaGain[max_number_va][channel_per_va][GAIN_MAX]; 
  double fhShaping[HISTO_SHAPING_BIN+2]; 
  double fhClusLength_S[HISTO_CLUSTER_LENGTH_S_BIN+2]; 
  double fhClusLength_K[HISTO_CLUSTER_LENGTH_K_BIN+2]; 
  double fhClusNumber_S[HISTO_CLUSTER_NUMBER_S_BIN+2]; 
  double fhClusNumber_K[HISTO_CLUSTER_NUMBER_K_BIN+2]; 
  double fhClusCog[HISTO_CLUSTER_COG_BIN+2]; 
  double fhClusMaxloc[HISTO_CLUSTER_MAXLOC_BIN+2]; 
  double fhClusIntegral_S[HISTO_CLUSTER_INTEGRAL_S_BIN+2]; 
  double fhClusIntegral_K[HISTO_CLUSTER_INTEGRAL_K_BIN+2]; 
  double fhClusSovern_S[HISTO_CLUSTER_SOVERN_S_BIN+2]; 
  double fhClusSovern_K[HISTO_CLUSTER_SOVERN_K_BIN+2]; 
  double fhPedSub[HISTO_PEDSUB_BIN+2];    
  double fhSpectrumS1[HISTO_SPECTRUMS_BIN+2];
  double fhSpectrumS2[HISTO_SPECTRUMS_BIN+2];
  double fhSpectrumK[HISTO_SPECTRUMK_BIN+2];
}; 
 
 
 
 
#define CONTROL_SIZE 5120
struct Control_buf {                                           // C R D G L 
  char Version[20]; // program version

  int On;          // Running=1  CloseApplication=0               X R R - R  
 
  int CTL;         // Status Control process 1=Ok                 X - - R - 
  int LOG;         // Status Log process 1=Ok                     X - - R - 
  int RUN;         // Status Run process 1=Ok                     X - - R - 
  int DSP;         // Status DSP process 1=Ok                     X - - R - 
  int GUI;         // Status GUI process 1=Ok                     X - - R -   
 
  pid_t CTL_pid;    // CTL pid 
  pid_t LOG_pid;    // LOG pid 
  pid_t RUN_pid;    // RUN pid 
  pid_t DSP_pid;    // DSP pid 
  pid_t GUI_pid;    // GUI pid 
 
  int Station;      // Test Station in use
  char StationLine[20];  //  
  char StationCode[3];   //

  int RunStatus;    // Stop=0 Run=1 
  int Enable_START_RUN;             
  int Enable_STOP_RUN; 
 
  int LogStatus; // Not Logging (=0) or Logging (=1) 
  int DataMode;  // Raw=1 Calibration=2 Reduction=3 Gain=4 Channel=5 
  int Trigger;   // Random (=0) External (=1) 
  int Source;    // Real (=0) or Simulate (=1) event 
 
  int ActiveS1;  // 1:Active analog chain S1 
  int ActiveS2;  // 1:Active analog chain S2 
  int ActiveK;   // 1:Active analog chain K 
 
  float Shaping;   // Shaping time in 100ns steps (for ext trigger)

  char  TempId[16];   // Id of temperature sensor 
  float HybridTemp;  // Hybrid temperature
  float RoomTemp;    // Room temperature  
  float Ileak80;     // Leakage current @ 80 V
  float Ileak90;     // Leakage current @ 90 V
  char  Comment[200];// Comments

  char AMSTest;    // AMS official test id
  int AMSTestNumber; // =01,02,....99
  int TestSequence; //    
  int TestOn;       // =1 if runnig any calibration =0 otherwise
  int CloseTestFile;// =1 if we want to close output data file =0 otherwise
  
  int NRun;        // Run Number 
  int NEvents;     // Number of Events in this run 
  int NEvents_max; // Maximum Number of Events 
  int NBadPwrS;    // Number of events for which there is power problem (+/-2.0 V or 5.6 V)
  int NBadPwrK;    // Number of events for which there is power problem (+/-2.0 V or 5.6 V)
 
  char TimeStartRun[30]; // Time at start run 
  int RunTime;     // RunTime in miliseconds 
  float EventRate;   // In Hertzs  \ 
  float LiveTime;    // In percent  |-> Updated once per second 
  float InsLiveTime; // In percent / 
 
 
 
  int OnlyHybrid;    // =1 Just calibrating hybrids. Only information purposes 
  int FullCalib;     // =1 if full calibration =0 otherwise 
  int CalibSequence;
  int CalStep;       // =0,1,2,3,4     
  int MaxCalStep; 
  int CurrentCalStep; 
  int CalSequence[10];//  
  int EventsStep[5]; // Number of events for different calibration steps 
                     //  0=throw away                 (128) 
                     //  1=pedestal computation      (2048)  
                     //  2=sigma raw computation     (512) 
                     //  3=final sigma computation   (1024)  
                     //  4=bad channel computation   (2048) 
  
 
 
  int DACValue;         // DAC value for Channel mode = [0,255] 
  int DACva;            // VA [1,16]  
  int DACch;            // Channel [1,64]  
  int DACpos;           // channel absolute value [1,384]
  int DACchain;
 
  int DAC[GAIN_MAX];          // DAC value = [0,255] 
  int DACEvent[GAIN_MAX];     // Number of DAC events to be taken 
  int DACmeasure[GAIN_MAX];   // 0=DAC value not taken 
  int CurrentGainStep;   
  int MaxGainStep;            // max=GAIN_MAX 
  int GainSequence[GAIN_MAX];  
  int GainIndex;              // GainIndex=GainSequence[CurrentGainStep]; 
  float GainLow;     // Cut for low gain 
 
  float SRSigmaCut;  // Parametros del ajuste en SigmaRaw 
  int   SRBin;       //  
  float SRLow;       // Cut in Sigma Raw low  
  float SRHigh;      // Cut in Sigma Raw high 
  float SigmaLow;    // Cut in Sigma low
  float SigmaHigh;   // Cut in Sigma high
  float GASigmaCut;  // Number of sigmas in gaussianity test
  float GAOut;       // % of event out of GASigmaCut


  float fCut1;
  float fCut2;
  float fMinInt;
  int   fMaskClu;

  int CosmicSequence;  // =0 calibration  =1 data
  int CosmicId;        // Run number of first calibration

  int SourceSequence;  // =0 calibration =1,2,3 positions 2b,2c,2d

  int LadderType;    // Ladder description (=test =AMS =external ...) 
  char fLadderName[15]; 
  char fLadderNameAtExit[15];

  int TrackerXtant; // tracker Octant or Sextant under test
  char LadderNameOctant[24][20];  // ladder names in one octant

  int TDRref;    // TDR under observation (in JINF mode)
  int EPPport;   // EPP board port to which the JINF is connected
  int Hardware;  // Test box, JINF EPP, JINF PCI, ...?...
  int PCIcard;   // PCI access card, normally 0, but could be up to 3
  int PCIport;   // each PCI card has 8 ports. 

  unsigned int TDRmask; // slave mask in the Jinf

  int JINJprog; // flash program to load
  int JINFprog; // flash program to load
  int TDRprog; // flash program to load
  unsigned int JINFaddr; // amswire address of the Jinf

  int ShowBadChannels; // clear enough, no ?
  int RecordGifMovie; // this one too...
  int SaveHistoRawcal;

  float ChannelFrequency; // channel readout frequency: for FFT

   //char fVoltage; 
    //char fSensors[3]; 
    //char fAssembly; 
    //char fLayer; 
    //char fSerial[4]; 
  
    //char fHybridName[15];
    //char fHybridType[5];
    //char fHybridBatch[4];
    //char fHybridSerial[4];
    

  int DSPCalibStep;
  int TDRDSPselect;

  int ReadoutPWG;
  int DynPed; // option to turn on/off dynamic pedestals

  // Directories 
  char fDirMain[200];         // Main data directory   
  char fDirLadder[200];       // =fDirFile/LadderName 
  char fRunFileName[200];     // = fDirLadder/Run.Summary 
 
 
  char fDirRawData[200];      // =fDirLadder/Raw  
  char fFileName[200];        // =fDirLadder/ 

  char fDirTest[200];         // =fDirLadder/Test-Axx  xx=01,02,...,99 

  char Printer[30]; // printer name
  char PrintCmd[20]; // print command
  char PrintOpt[20]; // print option

  char LogMessage[2000];       // line message added in the log window

}; 
 
enum sCONTROL{  
  kLOGSTATUS_NOTLOG=1, 
  kLOGSTATUS_BINARY=2, 
  kLOGSTATUS_ASCII=3,   
  kLOGSTATUS_TEST=4,

  kDATAMODE_RAW=11, 
  kDATAMODE_CALIBRATION=12, 
                cPASS_0=120, 
                cPASS_1=121, 
                cPASS_2=122,   
                cPASS_3=123, 
                cPASS_4=124, 
                cPASS_5=125, 
                cPASS_ALL=126,
  kDSPCALIBRATION=1200,
  kOCTANTCALIBRATION=1201,
  kDATAMODE_REDUCTION=13, 
  kDATAMODE_GAIN=14, 
  kDATAMODE_SCOPE=15, 
  kFULLCALIBRATION=16,
  kDATAMODE_COSMIC=17,
  kDATAMODE_SOURCE=18,
  kDATAMODE_FFT=19,
  
  kTRIGGER_RANDOM=21, 
  kTRIGGER_EXTERNAL=22, 
 
  kSOURCE_SIMULATION=31, 
  kSOURCE_REALDATA=32, // from the test box (GSE in the code) 
  kSOURCE_BINARYFILE=33, 
  kSOURCE_ASCIIFILE=34, 
  kSOURCE_BEAMTESTX5=35, 
  kSOURCE_TDRCDP=36, 
  kSOURCE_JINF=37, 
  kSOURCE_TDR=38, 

  kRUNSTATUS_RUN=41, 
  kRUNSTATUS_STOP=42, 

  kSTATION_GENEVA=51,
  kSTATION_PERUGIA=52,
  kSTATION_GYA=53,
  kSTATION_PREVESSIN=54,
  kSTATION_EDUARDO=55,
  kSTATION_GIOVANNI=56,
}; 
 

enum sHARDWARE{
  kHARDWARE_TESTBOX_EPP=0,
  kHARDWARE_JINF_EPP=1,
  kHARDWARE_JINF_PCI=2,
};
 
enum sCalibration{ 
}; 
 
enum sLadderTypes{ 
  kLadderTest = 1, 
  kLadderAMS  = 2,
  kLadderHybrid = 3,
  kLadderLast=4,
}; 
 
#define max_buf 10 
#define DATA_SIZE 1024*8*max_buf+2000 
struct Data_buf {                                           // C R D G L     
  int fIn;                                                  // - X - - - 
  int fOut;                                                 // - X R - -  
  unsigned short Data[max_buf][max_number_va][channel_per_va];// - X R - -   
}; 
 
 
 
class Share { 
 public: 
  Share(char *file, int id, int size); 
  ~Share(); 
   
  char *GetAddress(){return shmaddr;} 
  void Detach(); 
  void Delete(); 
  void Empty();
 
 private: 
  key_t key; 
  int shmid; 
  int shm_size;
  char *shmaddr; 
  struct shmid_ds shmdesc; 
}; 
 
 
#endif /*SHARE_H*/ 
                 
