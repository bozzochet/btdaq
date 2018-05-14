#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>

#ifndef _CONSTANTS
#define _CONSTANTS

// ==========================================
// ============== CONSTANTS =================
// ==========================================

/*
 * Calocube
 */
#if defined(SPS2015) || defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
#define ROC_HEADER 0xA5A5
#else
#define ROC_HEADER 0x5A5A
#endif

#define CASIS_HEADER 0xF0F0

#if defined(TIC)
const int NCASIS_CHIPS_X_BOARD = 3;
const int NCASISCHIPS = 54; //3*18
const int NCASISCHANNELS = 28;
const int NCASISCHIPS4CN = NCASISCHIPS * 2;
const int NCASISCHANNELS4CN = NCASISCHANNELS/2;
const int NGAINWORDS = 2;
const int NROWS = 6;
const int NCOLS = 5;
const int NINSTRUMCASISCHANNELS = 24;
const int NLAYERS = 17;
const int NSENSORS = 2;
#elif defined(SPS2017Aug) || defined(SPS2017Nov)
const int NCASIS_CHIPS_X_BOARD = 3;
const int NCASISCHIPS = 54; //3*18
const int NCASISCHANNELS = 28;
const int NCASISCHIPS4CN = NCASISCHIPS * 2;
const int NCASISCHANNELS4CN = NCASISCHANNELS/2;
const int NGAINWORDS = 2;
const int NROWS = 6;
const int NCOLS = 5;
const int NINSTRUMCASISCHANNELS = 24;
const int NLAYERS = 18;
const int NSENSORS = 2;
#elif SPS2016
const int NCASIS_CHIPS_X_BOARD = 3;
const int NCASISCHIPS = 54; //3*18
const int NCASISCHANNELS = 28;
const int NCASISCHIPS4CN = NCASISCHIPS * 2;
const int NCASISCHANNELS4CN = NCASISCHANNELS/2;
const int NGAINWORDS = 2;
const int NROWS = 6;
const int NCOLS = 5;
const int NINSTRUMCASISCHANNELS = 24;
const int NLAYERS = 18;
const int NSENSORS = 2;
#else
const int NCASIS_CHIPS_X_BOARD = 3;
const int NCASISCHIPS = 9; //3*3
const int NCASISCHANNELS = 16;
const int NCASISCHIPS4CN = NCASISCHIPS;
const int NCASISCHANNELS4CN = NCASISCHANNELS;
const int NGAINWORDS = 1;
const int NROWS = 3;
const int NCOLS = 3;
const int NINSTRUMCASISCHANNELS = 15;
const int NLAYERS = 15;
const int NSENSORS = 1;
#endif

const int NXY = 2;
const int NSIDE = 2;

const float SIZE = 4.0;
const int NTOTCASISCHANNELS = NCASISCHIPS * NCASISCHANNELS;
const int NTOTCHANNELS = NCASISCHIPS * NCASISCHANNELS;
const int TOTCUBES = NROWS * NCOLS * NLAYERS;
const int NCUBES = (NROWS-1) * NCOLS;

// Trigger types
const unsigned short int RANDOMTRIGGER = 0x40;
const unsigned short int PHYSICSTRIGGER = 0x20;
// CALOCUBES good event flag
const unsigned short int GOODEVENTFLAG = 0x02;
const unsigned short int BADEVENTFLAG = 0x04;

const double CASIS_TIME_START = 80.;// *10 ns
const double CASIS_TIME_END = 1400.; // *10 ns

const float MIPCUTHIT_LARGE = 0.6; //MIP
const float MIPCUTHIT_SMALL = 0.6;
const float MIPCUTHIT_CMB = 0.6;

const float CsI_SIZE = 3.6; //cm
const float CsI_GAP = 0.4;  //cm

const float RATIO_THR = 25./100.; //% tolerance

const float mipCutHit = MIPCUTHIT_CMB;
const float tCutHit = 2; // cut for hit finding: |ADC-PED| > tCutHit * SIG //2015
const double tCut = 10.; //cut for CN and PED-SIG computation: NB |ADC-PED| < tCut * SIG

/*
 * ADAMO
 */
const int NLAYER_ADAMO = 5;
const int NXY_ADAMO = 2;
const int NCHIP_ADAMO = 8;
const int NSTRIP_ADAMO = 1024;
const double XSTRIP_ADAMO = 51./1000.; //mm
const double YSTRIP_ADAMO = 66.5/1000.; //mm
const double X_ADAMO = ((double)NSTRIP_ADAMO)*XSTRIP_ADAMO; //mm
const double Y_ADAMO = ((double)NSTRIP_ADAMO)*YSTRIP_ADAMO; //mm
const double Z_ADAMO[NLAYER_ADAMO] = {0.,8.450,138.450,251.550,260.000}; //mm

/*
 * AMS
 */
const int  NJINF = 1;
const int  NTDRS = 24;
//const int NLAYER_AMS = 5;
const int NLAYER_AMS = NJINF*NTDRS;
const int NXY_AMS = 2;
const int NCHIP_AMS = 8;
const int NSTRIP_AMS = 1024;
const int  NSTRIPSS = 640;
const int  NSTRIPSK = 384;
const double SPITCH = 0.110;
const double KPITCH = 0.208;

#endif
