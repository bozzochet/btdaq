#ifndef _Ladder_included_
#define _Ladder_included_

#include <math.h> // for exp floor
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <iostream>
#include <iomanip>
//#include <strstream.h>
#include <fstream>

#include <fftw3.h>

#include "Cluster.h"

#include "TDRbits.h"
#define va_per_ladder  16
#define channel_per_va  64
#define va_per_adc_S 5
#define va_per_adc_K 6
#define max_channel va_per_ladder*channel_per_va

#include "Queue.h"
#undef  Q_CTL
#define Q_LOG
#undef  Q_GUI
#undef  Q_RUN
#undef  Q_DSP

#include "Share.h"
#define S_CONTROL
#define S_DATA
#define S_HISTO

typedef int boolean;
#define false 0
#define true 1


// End of Common Definitions
class Ladder {
 public:
  Ladder();
  Ladder(char aVoltage, char aSensors[3], char aAssembly, 
	 char aLayer  , char aSerial[4]);
  Ladder(char aName[15]);
  virtual ~Ladder();

  void ReadEvent(int aSlot);
  int  GetBin(int aHist, double aValue);
  void UpdateScope();
  void UpdatePedSub();
  void UpdateSignal(int dynped=0);
  void UpdateSignalX(int dynped=0);
  void UpdateSpectrum();

  void Reset(int aPass);
  void PassCal0();
  void FinishPassCal0();
  void PassCal1();
  void FinishPassCal1();
  void PassCal2();
  void FinishPassCal2();
  void PassCal3();
  void FinishPassCal3();
  void PassCal4();
  void FinishPassCal4();
  void WriteSummary(int aSaveTest);
  void CalcSummary(int firstva, int lastva, FILE *outFile);
  void QualityLadder(int firstva, int lastva);
  void SaveCalibration(int aSaveTest);
  int ReadCalibration();
  int ReadRefCalibration();
  void ReadDSPCalib();

  void Gain();
  void FinishGain();
  void SaveGain();

  void Channel();

  void FillStatus();
  void FillPedestal();
  void FillSigma();
  void FillSigmaRaw();
  void FillRefPedestal();
  void FillRefSigma();
  void FillRefSigmaRaw();
  void FillDiffPedestal();
  void FillRawCal();
  void FillSummaryPlots();
  void SubstractPedestal();
  void ComputeCN1();
  void ComputeCN2();
  void SubstractCN();
  void Reduction(int dynped=0);
  void DoCluster(char aSorK, cluster **aCluster);
  void FreeCluster(cluster **aCluster);
  void FreeChannel(channel **aChannel);


  void OpenOutputFile();
  void CloseOutputFile();
  void SaveEvent();

  char       *GetName(){return fName;}
  char     GetVoltage(){return fVoltage;}
  char    *GetSensors(){return fSensors;}
  char    GetAssembly(){return fAssembly;}
  char       GetLayer(){return fLayer;}
  char     *GetSerial(){return fSerial;}

  int Getva(int aChannel);
  int Getch(int aChannel);

  void ComputeGain();

  void StartFFT();
  void DoFFT(double* array, int nech, double* his);


  void fit(double x[], double y[], double sig[], int ndata,
	   double *a, double *b, double *siga, double *sigb,
	   double *chi2, double *q);
  double gammln(double xx);
  double gammq(double a, double x);
  void gser(double *gamser, double a, double x, double *gln);
  void gcf(double *gammcf, double a, double x, double *gln);

 protected:
  char  fName[15];     // Name of the ladder fName=
  char  fVoltage;     // (L)ow or (H)igh voltaje
  char  fSensors[3];  // # of Silicon sensors (01,02,....10,11...)
  char  fAssembly;    // Assembly Line (G)eneve, (P)erugia, (Z)urich
  char  fLayer;       // (I)nner, (O)uter, (T)est 
  char  fSerial[4];   // serial number per assembly line (001,..,090,..,230,..)

 private:
  double fPedestal[va_per_ladder][channel_per_va]; // Pedestal values
  double fRefPedestal[va_per_ladder][channel_per_va]; // Reference Pedestal values
  double fSigma[va_per_ladder][channel_per_va]; // Sigma values
  double fRefSigma[va_per_ladder][channel_per_va]; // Reference Sigma values
  int fStatus[va_per_ladder][channel_per_va]; // Status values 
  int fRefStatus[va_per_ladder][channel_per_va]; // Reference Status values 
  int fNonGaussian[va_per_ladder][channel_per_va]; // Non gaussian channels 


  unsigned short fRAW[va_per_ladder][channel_per_va]; // Event ReadOut (RAW)
  double fReadOut[va_per_ladder][channel_per_va]; // Event ReadOut (RAW)
  double fSigmaRaw[va_per_ladder][channel_per_va]; // Sigma Raw values
  double fRefSigmaRaw[va_per_ladder][channel_per_va]; // Reference Sigma Raw values
  double fCN[va_per_ladder];                 // Event Common Noise
  double fRefCN[va_per_ladder];                 // Event Common Noise

  double fSRmean[va_per_ladder];              // Mean of status dist.
  double fSRsigma[va_per_ladder];             // Sigma of status dist.

  double fCNmean[va_per_ladder];              // Mean of CN dist.
  double fCNsigma[va_per_ladder];             // Sigma of CN dist.


  int   fNst[va_per_ladder];                  // # channels ok per VA
  int   fNevent;                              // Total number of events  
  int   fGood[va_per_ladder][channel_per_va]; // Good events (calib.)

  double fPedcal[va_per_ladder][channel_per_va];  // Pedestal values (calib.)
  double fSigcal[va_per_ladder][channel_per_va];  // Sigma values (calib.)

  double fGain[GAIN_MAX][va_per_ladder][channel_per_va];      // Mean Gain
  double fSigmaGain[GAIN_MAX][va_per_ladder][channel_per_va]; // Sigma Gain
  double fGainSlope[va_per_ladder][channel_per_va];
  double fRefGainSlope[va_per_ladder][channel_per_va];
  double fGainSlopeErr[va_per_ladder][channel_per_va];

  double fSignal[va_per_ladder][channel_per_va];
  double fSignalMean[va_per_ladder][channel_per_va];    
  double fSignalHits[va_per_ladder][channel_per_va];  

  cluster *Scluster;
  cluster *Kcluster;

  double fSpectrumS1[va_per_adc_S*channel_per_va];
  double fSpectrumS2[va_per_adc_S*channel_per_va];
  double fSpectrumK[va_per_adc_K*channel_per_va];


  FILE *fRawDataFile;
  FILE *fTDRDataFile;

  int fSRawlow[va_per_ladder+3];         // va_per_ladder   = all VAs    
  int fSRawhigh[va_per_ladder+3];        // va_per_ladder+1 = S side
  int fSigmalow[va_per_ladder+3];        // va_per_ladder+2 = K side
  int fSigmahigh[va_per_ladder+3];
  int funder[va_per_ladder+3];       
  int fover[va_per_ladder+3];    
  int fnongaus[va_per_ladder+3]; 
  int fgainlow[va_per_ladder+3];

  float fmeanped[va_per_ladder];
  float fsigped[va_per_ladder]; 
  float fmeansig[va_per_ladder];
  float fsigsig[va_per_ladder]; 
  float fmeangan[va_per_ladder];
  float fsiggan[va_per_ladder]; 

  int fbadSRawlow[2];
  int fbadSRawhigh[2];
  int fbadSigmalow[2];
  int fbadSigmahigh[2];
  int fbadunder[2];
  int fbadover[2];
  int fbadnongaus[2];
  int fbadgainlow[2];


  int fGoodLadder[va_per_ladder+1];  // bit 0  =va SRawlownoise
                                     // bit 1  =va SRawhighnoise
                                     // bit 2  =va Sigmalownoise
                                     // bit 3  =va Sigmahighnoise
                                     // bit 4  =va under
                                     // bit 5  =va over
                                     // bit 6  =va nongaus
                                     // bit 7  =va gainlow
                                     // bit 8  =any va SRawlownoise
                                     // bit 9  =any va SRawhighnoise
                                     // bit 10 =any va Sigmalownoise
                                     // bit 11 =any va Sigmahighnoise
                                     // bit 12 =any va under
                                     // bit 13 =any va over
                                     // bit 14 =any va nongaus
                                     // bit 15 =any va gainlow

#include "msg_define.code"
#include "shm_define.code"

};

// Documentation
// fStatus[va][ch] - Status bit word
//     bit 0: Low noise  - Pass 2 
//     bit 1: High noise - Pass 2 
//     bit 2: 
//     bit 3: Underflow - Pass 2
//     bit 4: Overflow  - Pass 2
//     bit 5: NonGaussian behaviour - Pass 4
//     bit 6: Non valid for CN calculation (Signal-Pedestal)>3Sigma

//  NEW NEW NEW NEW 24-april-2002
// fStatus[va][ch] - Status bit word
//     bit 0: Sigma Raw Low noise  - Pass 2 
//     bit 1: Sigma Raw High noise - Pass 2 
//     bit 2: Underflow - Pass 2
//     bit 3: Overflow - Pass 2
//     bit 4: Sigma Low  - Pass 3
//     bit 5: Sigma High - Pass 3 
//     bit 6: NonGaussian behaviour - Pass 4
//     bit 7: Gain Lo
//     bit 8: Non valid for CN calculation (Signal-Pedestal)>3Sigma





// End of Class Ladder
#endif






